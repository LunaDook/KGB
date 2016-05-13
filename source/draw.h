#pragma once

#include "common.h"

#define HEIGHT		240
#define TOP_WIDTH	400
#define SUB_WIDTH	320

#define TOP_FB_SZ	(TOP_WIDTH * HEIGHT)
#define SUB_FB_SZ	(SUB_WIDTH * HEIGHT)

#define fb_sz(fb) \
	(fb == framebuffer->bottom ? SUB_FB_SZ : TOP_FB_SZ)

#define set_pixel(fb, x, y, rgb) \
{ \
	u32 offset = (HEIGHT * (x) + HEIGHT - (y) - 1) * 3; \
	*(u8*)(fb + offset++) = rgb & 0xFF; \
	*(u8*)(fb + offset++) = (rgb >> 8) & 0xFF; \
	*(u8*)(fb + offset++) = (rgb >> 16) & 0xFF; \
}

typedef struct framebuffer_t {
    u8 *top_left;
    u8 *top_right;
    u8 *bottom;
} framebuffer_t;

extern framebuffer_t *framebuffer;

void clear_screen(const u8 *fb, const u32 rgb);
void draw_char(const u16 x, const u16 y, const u8 c);
