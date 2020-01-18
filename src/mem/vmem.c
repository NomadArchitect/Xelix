/* vmem.c: Virtual memory management
 * Copyright © 2013-2019 Lukas Martini
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
 * along with Xelix. If not, see <http://www.gnu.org/licenses/>.
 */

#include "vmem.h"
#include <log.h>
#include <mem/kmalloc.h>
#include <mem/palloc.h>
#include <mem/paging.h>
#include <panic.h>
#include <string.h>
#include <tasks/scheduler.h>

static struct vmem_context* kernel_ctx = NULL;

void vmem_map(struct vmem_context* ctx, void* virt_start, void* phys_start, uintptr_t size, bool user, bool ro) {
	if(!ctx) {
		if(!kernel_ctx) {
			return;
		}
		ctx = kernel_ctx;
	}

	struct vmem_range* range = zmalloc(sizeof(struct vmem_range));
	range->readonly = ro;
	range->user = user;
	range->virt_start = ALIGN_DOWN((uintptr_t)virt_start, PAGE_SIZE);
	range->phys_start = ALIGN_DOWN((uintptr_t)phys_start, PAGE_SIZE);
	range->length = ALIGN(size, PAGE_SIZE);
	range->next = ctx->ranges;
	ctx->ranges = range;

	if(ctx->hwdata) {
		paging_set_range(ctx->hwdata, range);
	}
}

struct vmem_range* vmem_get_range(struct vmem_context* ctx, uintptr_t addr, bool phys) {
	if(!ctx) {
		ctx = kernel_ctx;
	}

	struct vmem_range* range = ctx->ranges;
	for(; range; range = range->next) {
		uintptr_t start = (phys ? range->phys_start : range->virt_start);

		if(addr >= start && addr <= (start + range->length)) {
			return range;
		}
	}
	return NULL;
}

void vmem_rm_context(struct vmem_context* ctx) {
	paging_rm_context(ctx->hwdata);

	struct vmem_range* range = ctx->ranges;
	while(range) {
		struct vmem_range* old_range = range;
		range = range->next;
		kfree(old_range);
	}

	kfree(ctx);
}

void* vmem_get_hwdata(struct vmem_context* ctx) {
	if(!ctx->hwdata) {
		ctx->hwdata = zpalloc(1);
		struct vmem_range* range = ctx->ranges;

		for(; range; range = range->next) {
			paging_set_range(ctx->hwdata, range);
		}
	}
	return ctx->hwdata;
}

void vmem_init() {
	// Initialize kernel context
	kernel_ctx = zmalloc(sizeof(struct vmem_context));
	vmem_map_flat(NULL, KERNEL_START, 0xfffff000, 0, 0);
	vmem_kernel_hwdata = vmem_get_hwdata(kernel_ctx);
	paging_init();
}
