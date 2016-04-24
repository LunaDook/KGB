#include "draw.h"
#include "memory.h"

#define set_pixel(fb, x, y, rgb) \
	u32 fb_off = (240*x + 239 - y) * 3; \
	*((u8*)fb + fb_off++) = rgb & 0xFF; \
	*((u8*)fb + fb_off++) = (rgb >> 8) & 0xFF; \
	*((u8*)fb + fb_off++) = (rgb >> 16) & 0xFF; // Modified from b1l1s' ctr library

#define fb_sz(fb) \
	(fb == fb_loc->bottom ? BOTTOM_FB_SZ : TOP_FB_SZ)

framebuffer_t* fb_loc = (framebuffer_t *)0x23FFFE00; // Location of framebuffers

void clear_screen(u8* fb, u32 rgb){
	u32 sz = fb_sz(fb)/3;
	u8 fb_packet[3] = {rgb & 0xFF, (rgb >> 8) & 0xFF, (rgb >> 16) & 0xFF};

	while (sz--)
		memcpy(fb + sz*3, fb_packet, 3);

	return;
}
