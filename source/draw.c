#include "common.h"

framebuffer_t *framebuffer = (framebuffer_t *)0x23FFFE00;

void clear_screen(u8 *fb, u32 rgb)
{
	u8 packet[3] = {rgb & 0xFF, (rgb >> 8) & 0xFF, (rgb >> 16) & 0xFF};
	size_t sz = fb_dimension(fb), i = 0;

	while (i < sz) {
		*(u8*)(fb + i++) = packet[0];
		*(u8*)(fb + i++) = packet[1];
		*(u8*)(fb + i++) = packet[2];
	}
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
