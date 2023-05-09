/* fbtext.c: Text drawing on linear frame buffers
 * Copyright © 2019-2023 Lukas Martini
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

#include <gfx/fbtext.h>
#include <gfx/gfx.h>
#include <mem/kmalloc.h>
#include <boot/multiboot.h>
#include <fs/sysfs.h>
#include <tty/console.h>
#include <errno.h>
#include <string.h>
#include <log.h>
#include <bitmap.h>

#define PSF_FONT_MAGIC 0x864ab572
#define BOOT_LOGO_WIDTH 183
#define BOOT_LOGO_HEIGHT 60
#define BOOT_LOGO_PADDING 10
#define BOOT_LOGO_PADDING_BELOW 30

#define PIXEL_PTR(dbuf, x, y) 									\
	((uint32_t*)((uintptr_t)(dbuf)								\
		+ (y)*gfx_handle->ul_desc.pitch							\
		+ (x)*(gfx_handle->ul_desc.bpp / 8)))

#define CHAR_PTR(dbuf, x, y) PIXEL_PTR(dbuf, x * gfx_font.width, \
	y * gfx_font.height)

static struct {
	uint32_t last_x;
	uint32_t last_y;
	uint32_t* last_data;
} cursor_data = {0, 0, 0};

// Font from ter-u16n.psf gets linked into the binary
extern const struct {
	uint32_t magic;
	uint32_t version;
	uint32_t header_size;
	// 1 if unicode table exists, otherwise 0
	uint32_t flags;
	uint32_t num_glyphs;
	uint32_t bytes_per_glyph;
	uint32_t height;
	uint32_t width;
} gfx_font;

static struct gfx_handle* gfx_handle = NULL;
static bool initialized = false;

void gfx_fbtext_write(uint32_t x, uint32_t y, wchar_t chr, uint32_t col_fg, uint32_t col_bg) {
	if(chr > gfx_font.num_glyphs) {
		chr = 0;
	}

	if(chr == ' ') {
		for(int i = 0; i < gfx_font.height; i++) {
			void* mdest = PIXEL_PTR(gfx_handle->ul_desc.addr, x * gfx_font.width, y * gfx_font.height + i);
			memset32(mdest, col_bg, gfx_font.width);
		}
		return;
	}

	const uint8_t* bitmap = (uint8_t*)&gfx_font
			+ gfx_font.header_size
			+ chr * gfx_font.bytes_per_glyph;

	if(unlikely(!bitmap)) {
		return;
	}

	x *= gfx_font.width;
	y *= gfx_font.height;

	for(int cy = 0; cy < gfx_font.height; cy++) {
		uintptr_t cy_ptr = (uintptr_t)(gfx_handle->ul_desc.addr)
			+ (y + cy)*gfx_handle->ul_desc.pitch +
			+ (x)*(gfx_handle->ul_desc.bpp / 8);

		for(int cx = 0; cx < gfx_font.width; cx++) {
			uintptr_t cx_ptr = cy_ptr + cx*(gfx_handle->ul_desc.bpp / 8);

			int fg = bitmap[cy] & (1 << (gfx_font.width - cx - 1));
			*(uint32_t*)cx_ptr = fg ? col_fg : col_bg;
		}
	}
}

void gfx_fbtext_set_cursor(uint32_t x, uint32_t y, bool restore) {
	x *= gfx_font.width;
	y *= gfx_font.height;

	if(!cursor_data.last_data) {
		cursor_data.last_data = zmalloc(gfx_font.height * sizeof(uint32_t));
	} else {
		if(restore) {
			for(int i = 0; i < gfx_font.height; i++) {
				*PIXEL_PTR(gfx_handle->ul_desc.addr, cursor_data.last_x, cursor_data.last_y + i) = cursor_data.last_data[i];
			}
		}
	}

	for(int i = 0; i < gfx_font.height; i++) {
		cursor_data.last_data[i] = *PIXEL_PTR(gfx_handle->ul_desc.addr, x, y + i);
	}

	cursor_data.last_x = x;
	cursor_data.last_y = y;

	for(int i = 0; i < gfx_font.height; i++) {
		int color = 0xffffff;
		if(i == 0 || i == gfx_font.height - 1) {
			color = 0x000000;
		}

		*PIXEL_PTR(gfx_handle->ul_desc.addr, x, y + i) = color;
	}
}

// Switch GFX output to fbtext. This is used during kernel panics
void gfx_fbtext_show() {
	if(!initialized) {
		return;
	}
	gfx_handle_enable(gfx_handle);
}

void gfx_fbtext_init() {
	gfx_handle = gfx_handle_init(VM_KERNEL);
	if(!gfx_handle) {
		log(LOG_ERR, "fbtext: Could not get gfx handle\n");
		return;
	}

	memset32(gfx_handle->ul_desc.addr, 0x000000, gfx_handle->ul_desc.size / 4);
	int cols = gfx_handle->ul_desc.width / gfx_font.width;
	int rows = gfx_handle->ul_desc.height / gfx_font.height;

	log(LOG_DEBUG, "fbtext: font width %d/%d height %d/%d flags %d\n", gfx_font.width, cols, gfx_font.height, rows, gfx_font.flags);

	initialized = true;
	tty_console_init(cols, rows);
	gfx_fbtext_show();
	log_dump();
}
