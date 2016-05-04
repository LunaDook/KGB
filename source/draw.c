#include "draw.h"
#include "memory.h"

framebuffer_t *framebuffer = (framebuffer_t *)0x23FFFE00;

void clear_screen(u8 *fb, u32 rgb){
	u8 pixeldata[3] = {rgb & 0xFF, (rgb >> 8) & 0xFF, (rgb >> 16) & 0xFF};

	u32 sz;
	
	if (pixeldata[0] == pixeldata[1] && pixeldata[1] == pixeldata[2]) {
		sz = fb_sz(fb);
		memset(fb, pixeldata[0], sz);
		return;
	}

	sz = fb_sz_reduced(fb);
	while (sz--) {
		memcpy(fb + (sz * 3), pixeldata, 3);
	}

	return;
}

void set_pixel(u8 *fb, u16 x, u16 y, u32 rgb) { // Because macros hate me apparently
	u32 offset = (HEIGHT * x + HEIGHT - y - 1) * 3;
	u8 p[3] = {rgb & 0xFF, (rgb >> 8) & 0xFF, (rgb >> 16) & 0xFF};
	memcpy(fb + offset, p, 3);	
}

void draw_char(u16 x, u16 y, u32 rgb, char c)
{
	u32 _x, _y, _c = c * 8;
	for(_y = 0; _y < 8; _y++)
	{
		u8 mask = 0b10000000;
		u8 row = font[_y + _c];
		for(_x = 0; _x < 8; _x++, mask >>= 1)
		{
			if(row & mask)
				set_pixel(framebuffer->top_left, x + _x, y + _y, rgb);
		}
	}
}
