#pragma once

#include "types.h"
#include "font.h"

#define HEIGHT		240
#define TOP_WIDTH	400
#define SUB_WIDTH	320

#define TOP_RFB_SZ	(TOP_WIDTH * HEIGHT)
#define SUB_RFB_SZ	(SUB_WIDTH * HEIGHT)

#define TOP_FB_SZ	(TOP_WIDTH * HEIGHT * 3)
#define SUB_FB_SZ	(SUB_WIDTH * HEIGHT * 3)

/*#define set_pixel(fb, x, y, rgb) { \
	u32 offset = (HEIGHT * x + HEIGHT - y - 1) * 3; \
	u8 p[3] = {rgb & 0xFF, (rgb >> 8) & 0xFF, (rgb >> 16) & 0xFF}; \
	memcpy(fb + offset, p, 3); }*/ // Doesn't work for some reason

#define fb_sz_reduced(fb) \
	(fb == framebuffer->bottom ? SUB_RFB_SZ : TOP_RFB_SZ)
	
#define fb_sz(fb) \
	(fb == framebuffer->bottom ? SUB_FB_SZ : TOP_FB_SZ)


typedef struct framebuffer_t { // Thanks to mid-kid for fb offsets
    u8 *top_left;
    u8 *top_right;
    u8 *bottom;
} framebuffer_t;

extern framebuffer_t *framebuffer;

void clear_screen(u8* fb, u32 rgb);
void draw_char(u16 x, u16 y, u32 rgb, char c);
void set_pixel(u8 *fb, u16 x, u16 y, u32 rgb);
