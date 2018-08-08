/* kmalloc.c: Kernel memory allocator
 * Copyright © 2016-2018 Lukas Martini
 *
 * This file is part of Xelix.
 *
 * Xelix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xelix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xelix.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "kmalloc.h"
#include "track.h"
#include "vmem.h"
#include <lib/log.h>
#include <lib/multiboot.h>
#include <lib/panic.h>
#include <lib/spinlock.h>

#define KMALLOC_MAGIC 0xCAFE

/* Enable debugging. This will send out cryptic debug codes to the serial line
 * during kmalloc()/free()'s. Also makes everything horribly slow. */
#ifdef KMALLOC_DEBUG
	char* _g_debug_file = "";
	#define SERIAL_DEBUG(args...) { if(vmem_kernelContext && strcmp(_g_debug_file, "src/memory/vmem.c")) serial_printf(args); }
#else
	#define SERIAL_DEBUG(args...)
#endif

#define GET_FOOTER(x) ((uint32_t*)((intptr_t)x + x->size + sizeof(struct mem_block)))
#define GET_CONTENT(x) ((void*)((intptr_t)x + sizeof(struct mem_block)))
#define GET_FB(x) ((struct free_block*)GET_CONTENT(x))
#define GET_HEADER_FROM_FB(x) ((struct mem_block*)((intptr_t)(x) - sizeof(struct mem_block)))
#define PREV_FOOTER(x) ((uint32_t*)((intptr_t)x - sizeof(uint32_t)))
#define PREV_BLOCK(x) ((struct mem_block*)((intptr_t)PREV_FOOTER(x) \
	- (*PREV_FOOTER(x)) - sizeof(struct mem_block)))
#define NEXT_BLOCK(x) ((struct mem_block*)((intptr_t)GET_FOOTER(x) + sizeof(uint32_t)))
#define FULL_SIZE(x) (x->size + sizeof(uint32_t) + sizeof(struct mem_block))

struct mem_block {
	uint16_t magic;
	uint32_t size;

	enum {
		TYPE_KERNEL,
		TYPE_TASK,
		TYPE_FREE
	} type;

	// legacy
	uint32_t pid;
};

/* For free blocks, this struct gets stored inside the allocated area. As a
 * side effect of this, the minimum size for allocations is the size of this
 * struct.
 */
struct free_block {
	uint16_t magic;
	struct free_block* prev;
	struct free_block* next;
};

static bool initialized = false;
static spinlock_t kmalloc_lock;
static struct free_block* last_free = (struct free_block*)NULL;
static intptr_t alloc_start;
static intptr_t alloc_end;
static intptr_t alloc_max;

/* Do various checks on a header to make sure we're in a safe state
 * before doing anything.
 */
static void check_header(struct mem_block* header) {
	if(unlikely(header->magic != KMALLOC_MAGIC)) {
		panic("kmalloc: Metadata corruption (Block with invalid magic)\n");
	}

	if(likely((intptr_t)header != alloc_start)) {
		if(unlikely(PREV_BLOCK(header)->magic != KMALLOC_MAGIC)) {
			panic("kmalloc: Metadata corruption (previous block with invalid magic)");
		}
	}

	if(header->type == TYPE_FREE) {
		struct free_block* fb = GET_FB(header);

		if(unlikely(fb->magic != KMALLOC_MAGIC)) {
			panic("kmalloc: Metadata corruption (Free block without fb metadata)\n");
		}
	}
}

static inline void unlink_free_block(struct free_block* fb) {
	if(fb->next) {
		fb->next->prev = fb->prev;
	}

	if(fb->prev) {
		fb->prev->next = fb->next;
	}

	if(fb == last_free) {
		last_free = fb->prev;
	}
}

static inline struct mem_block* set_block(size_t sz, intptr_t position) {
	struct mem_block* header = (struct mem_block*)position;
	header->size = sz;
	header->magic = KMALLOC_MAGIC;

	// Add uint32_t footer with size so we can find the header
	*GET_FOOTER(header) = header->size;
	return header;
}

static struct mem_block* free_block(struct mem_block* header, bool check_next) {
	struct mem_block* prev = PREV_BLOCK(header);
	struct free_block* fb = GET_FB(header);

	/* If previous block is free, just increase the size of that block to also
	 * cover this area. Otherwise write free block metadata and add block.
	 */
	if((intptr_t)header > alloc_start && prev->type == TYPE_FREE) {
		check_header(prev);
		header->magic = 0;
		header = set_block(prev->size + FULL_SIZE(header), (intptr_t)prev);
	} else {
		header->type = TYPE_FREE;

		fb->prev = last_free;
		fb->next = (struct free_block*)NULL;
		fb->magic = KMALLOC_MAGIC;

		if(last_free) {
			last_free->next = fb;
		}

		last_free = fb;
	}

	// If next block is free, increase block size and unlink the next fb.
	struct mem_block* next = NEXT_BLOCK(header);
	if(check_next && alloc_end > (intptr_t)next && next->type == TYPE_FREE) {
		check_header(next);

		set_block(header->size + FULL_SIZE(next), header);
		unlink_free_block(GET_FB(next));
		next->magic = 0;
	}

	return header;
}

static inline struct mem_block* split_block(struct mem_block* header, size_t sz) {
	// Make sure the block is big enough to get split first
	if(header->size < sz + sizeof(struct mem_block) + sizeof(uint32_t) + sizeof(struct free_block)) {
		return NULL;
	}

	size_t orig_size = header->size;
	set_block(sz, (intptr_t)header);

	size_t new_size = orig_size - sz - sizeof(struct mem_block) - sizeof(uint32_t);
	return set_block(new_size, (intptr_t)NEXT_BLOCK(header));
}

static size_t get_alignment_offset(void* address) {
	size_t offset = 0;

	// Check if page is not already page aligned by circumstance
	if((intptr_t)address & (PAGE_SIZE - 1)) {
		offset = VMEM_ALIGN(GET_CONTENT(address)) - GET_CONTENT(address);

		/* We need at least x bytes to store the headers and footers of our
		 * block and of the new block we'll create in the offset
		 * FIXME Calc proper value for minimum size
	 	 */
		if(offset < 0x100) {
			offset += PAGE_SIZE;
		}
	}

	return offset;
}

static inline struct mem_block* get_free_block(size_t sz, bool align) {
	SERIAL_DEBUG("FFB ");
	struct free_block* fb = last_free;

	for(struct free_block* fb = last_free; fb; fb = fb->prev) {
		struct mem_block* fblock = GET_HEADER_FROM_FB(fb);
		check_header(fblock);

		if(unlikely(fblock->type != TYPE_FREE)) {
			log(LOG_ERR, "kmalloc: Non-free block in free blocks linked list?\n");
			continue;
		}

		size_t sz_needed = sz;
		if(align) {
			sz_needed += get_alignment_offset(GET_CONTENT(fblock));
			sz_needed += sizeof(struct mem_block) + sizeof(uint32_t) + sizeof(struct free_block);
		}

		if(fblock->size >= sz_needed) {
			SERIAL_DEBUG("HIT 0x%x size 0x%x ", fblock, fblock->size);
			unlink_free_block(fb);

			// Carve a chunk of the required size out of the block
			struct mem_block* new = split_block(fblock, sz_needed);

			if(new) {
				// Already set this to prevent free_block from merging
				fblock->type = TYPE_KERNEL;
				free_block(new, true);
			}

			return fblock;
		}
	}

	return NULL;
}

/* Use the macros instead of directly calling this function.
 * For details on the attributes, see the GCC documentation at http://is.gd/6gmEqk.
 */
void* __attribute__((alloc_size(1))) _kmalloc(size_t sz, bool align, uint32_t pid,
	char* _debug_file, uint32_t _debug_line, const char* _debug_func) {
	#ifdef KMALLOC_DEBUG
	_g_debug_file = _debug_file;
	#endif

	if(unlikely(!initialized)) {
		panic("Attempt to kmalloc before allocator is initialized.\n");
	}

	SERIAL_DEBUG("kmalloc: %s:%d %s 0x%x ", _debug_file, _debug_line, _debug_func, sz);

	if(sz < sizeof(struct free_block)) {
		sz = sizeof(struct free_block);
	}

	#ifdef KMALLOC_DEBUG
		if(sz >= (1024 * 1024)) {
			SERIAL_DEBUG("(%d MB) ", sz / (1024 * 1024));
		} else if(sz >= 1024) {
			SERIAL_DEBUG("(%d KB) ", sz / 1024);
		}
	#endif

	if(unlikely(!spinlock_get(&kmalloc_lock, 30))) {
		SERIAL_DEBUG("Could not get spinlock\n");
		return NULL;
	}

	struct mem_block* header = get_free_block(sz, align);
	bool need_alloc_end = false;

	size_t sz_needed = sz;

	if(!header) {
		SERIAL_DEBUG("NEW ");

		if(alloc_end + sz_needed >= alloc_max) {
			panic("kmalloc: Out of memory");
		}


		if(align) {
			sz_needed += get_alignment_offset(GET_CONTENT(alloc_end));
			sz_needed += sizeof(struct mem_block);
		}

		header = set_block(sz_needed, alloc_end);
		alloc_end = (uint32_t)GET_FOOTER(header) + sizeof(uint32_t);
	}

	if(align) {
		size_t offset_size = get_alignment_offset(GET_CONTENT(header));

		if(offset_size) {
			offset_size -= sizeof(uint32_t);

			SERIAL_DEBUG("ALIGN off 0x%x ", offset_size);

			struct mem_block* new = split_block(header, offset_size);

			new->type = TYPE_KERNEL;
			free_block(header, true);
			header = new;
		}
	}

	header->type = pid ? TYPE_TASK : TYPE_KERNEL;
	header->pid = pid;

	spinlock_release(&kmalloc_lock);
	SERIAL_DEBUG("RESULT 0x%x\n", (intptr_t)GET_CONTENT(header));
	return (void*)GET_CONTENT(header);
}

void _kfree(void *ptr, char* _debug_file, uint32_t _debug_line, const char* _debug_func)
{
	#ifdef KMALLOC_DEBUG
	_g_debug_file = _debug_file;
	#endif

	struct mem_block* header = (struct mem_block*)((intptr_t)ptr - sizeof(struct mem_block));
	SERIAL_DEBUG("kfree: %s:%d %s 0x%x size 0x%x\n", _debug_file, _debug_line, _debug_func, ptr, header->size);

	if(unlikely((intptr_t)header < alloc_start || (intptr_t)ptr >= alloc_end)) {
		SERIAL_DEBUG("INVALID_BOUNDS\n");
		return;
	}

	check_header(header);

	if(unlikely(!spinlock_get(&kmalloc_lock, 30))) {
		SERIAL_DEBUG("Could not get spinlock\n");
		return;
	}

	free_block(header, true);
	spinlock_release(&kmalloc_lock);
}

void kmalloc_init()
{
	memory_track_area_t* largest_area = NULL;
	for(int i = 0; i < memory_track_num_areas; i++) {
		memory_track_area_t* area = &memory_track_areas[i];

		if(area->type == MEMORY_TYPE_FREE && (!largest_area || largest_area->size < area->size)) {
			largest_area = area;
		}
	}

	if(!largest_area) {
		panic("kmalloc: Could not find suitable memory area");
	}

	largest_area->type = MEMORY_TYPE_KMALLOC;
	alloc_start = alloc_end = (intptr_t)largest_area->addr;
	alloc_max = (intptr_t)largest_area->addr + largest_area->size;
	initialized = true;

}

void kmalloc_stats() {
	// Walk allocations
	struct mem_block* header = (struct mem_block*)alloc_start;

	serial_printf("\nkmalloc_stats():\n");
	for(; header < alloc_end; header = NEXT_BLOCK(header)) {
		if(header->magic != KMALLOC_MAGIC) {
			serial_printf("0x%x\tcorrupted header\n", header);
			continue;
		}
		serial_printf("0x%x\tsize 0x%x\tres 0x%x\t", header, header->size, (intptr_t)header + sizeof(struct mem_block));

		if(header->type == TYPE_FREE) {
			struct free_block* fb = GET_FB(header);

			serial_printf("free\tprev free: 0x%x next: 0x%x", fb->prev, fb->next);
		} else if(header->type == TYPE_TASK) {
			serial_printf("task %d", header->pid);
		} else {
			serial_printf("kernel");
		}

		serial_printf("\n");
	}

	serial_printf("\n");
	serial_printf("alloc end:\t0x%x\n", alloc_end);
	serial_printf("last free:\t0x%x\n", last_free);
	serial_printf("\n");
}
