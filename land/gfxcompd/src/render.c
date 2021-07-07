#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <cairo/cairo.h>

#include "util.h"
#include "render.h"
#include "window.h"
#include "mouse.h"
#include "text.h"

#define IIR_GAUSS_BLUR_IMPLEMENTATION
#include "blur.h"

static cairo_surface_t* main_surface;
static cairo_surface_t* bg_surface_blurred;
static cairo_t* cr;
static uint32_t last_id = -1;

static inline void area_blit(size_t width, size_t height, size_t x, size_t y) {
	cairo_rectangle(cr, x, y, width, height);
	cairo_clip(cr);

	struct surface* surface = surfaces;
	for(; surface; surface = surface->next) {
		if(surface->show && surface_intersect(surface, width, height, x, y)) {
			cairo_surface_set_device_offset(surface->cs, x, y);
			cairo_set_source_surface(cr, surface->cs, surface->x + x, surface->y + y);
			cairo_paint(cr);
			cairo_surface_set_device_offset(surface->cs, 0, 0);
		}
	}

	cairo_reset_clip(cr);

	struct gfx_ul_blit_cmd cmd = {
		.handle_id = gfx_handle.id,
		.x = x,
		.y = y,
		.width = width,
		.height = height
	};
	ioctl(gfx_fd, 0x2f04, &cmd);
}

void surface_blit(struct surface* surface, size_t width, size_t height, size_t x, size_t y) {
	x = MIN(x, surface->width);
	y = MIN(y, surface->height);
	width = MIN(surface->width - x, width);
	height = MIN(surface->height - y, height);
	area_blit(width, height, surface->x + x, surface->y + y);
}

void surface_move(struct surface* surface, int32_t x, int32_t y) {
	int32_t old_x = surface->x;
	int32_t old_y = surface->y;
	surface->x = x;
	surface->y = y;
	area_blit(surface->width, surface->height, old_x, old_y);
	area_blit(surface->width, surface->height, surface->x, surface->y);
}

void surface_add(struct surface* surface) {
	struct surface* prev = surfaces;
	for(; prev; prev = prev->next) {
		if(prev->z <= surface->z && (!prev->next || prev->next->z >= surface->z)) {
			break;
		}
	}

	if(prev) {
		surface->next = prev->next;
		prev->next = surface;
	} else {
		surfaces = surface;
	}
}

struct surface* surface_new(cairo_surface_t* cs, size_t width, size_t height) {
	struct surface* surface = calloc(1, sizeof(struct surface));
	if(!surface) {
		return NULL;
	}

	surface->id = __sync_add_and_fetch(&last_id, 1);
	surface->show = true;
	surface->width = width;
	surface->height = height;
	surface->z = surface->id;
	surface->name = "unnamed";

	if(cs) {
		surface->cs = cs;
	} else {
		surface->cs = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
		if(!surface->cs) {
			free(surface);
			return NULL;
		}
	}

	return surface;
}

void render_init() {
	surfaces = NULL;

	// Get framebuffer
	gfx_fd = open("/dev/gfx1", O_WRONLY);
	if(!gfx_fd) {
		perror("Could not open /dev/gfx1");
		exit(EXIT_FAILURE);
	}

	if(ioctl(gfx_fd, 0x2f01, &gfx_handle) < 0) {
		perror("Could not get GFX handle");
		exit(EXIT_FAILURE);
	}

	main_surface = cairo_image_surface_create_for_data(
		(unsigned char*)gfx_handle.addr, CAIRO_FORMAT_ARGB32, gfx_handle.width,
		gfx_handle.height, gfx_handle.pitch);

	cr = cairo_create(main_surface);

	cairo_surface_t* bg_surface = cairo_image_surface_create_from_png("/usr/share/gfxcompd/bg.png");
	struct surface* bgs = surface_new(bg_surface, gfx_handle.width, gfx_handle.height);
	bgs->z = -1000;
	bgs->name = "Background";
	surface_add(bgs);

/*	bg_surface_blurred = cairo_image_surface_create_from_png("/usr/share/gfxcompd/bg.png");

	cairo_surface_flush(bg_surface_blurred);
	iir_gauss_blur(
		cairo_image_surface_get_width(bg_surface_blurred),
		cairo_image_surface_get_height(bg_surface_blurred), 4,
		cairo_image_surface_get_data(bg_surface_blurred), 15);
	cairo_surface_mark_dirty(bg_surface_blurred);
*/

	cairo_set_source_surface(cr, bg_surface, 0, 0);
	cairo_paint(cr);

	// Enable gfx handle
	ioctl(gfx_fd, 0x2f02, gfx_handle.id);

	// Full-screen blit
	cairo_surface_flush(main_surface);
	ioctl(gfx_fd, 0x2f03, gfx_handle.id);
}
