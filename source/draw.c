#include "common.h"

framebuffer_t *framebuffer = (framebuffer_t *)0x23FFFE00;

void clear_screen(const u8 *fb, u32 rgb)
{
	u8 packet[3] = {rgb & 0xFF, (rgb >> 8) & 0xFF, (rgb >> 16) & 0xFF};
	size_t sz = fb_sz(fb)*3, i = 0;

	while (i < sz)
	{
		*(u8*)(fb + i++) = packet[0];
		*(u8*)(fb + i++) = packet[1];
		*(u8*)(fb + i++) = packet[2];
	}
	return;
}

void draw_char(const u16 x, const u16 y, const char c)
{
	u32 _x, _y, _c = (c > 0 && c < 128) ? (c * 8) : (0);
	for(_y = 0; _y < FONT_Y; _y++)
	{
		u8 mask = 0b10000000;
		u8 row = font[_y + _c];
		for(_x = 0; _x < FONT_X; _x++, mask >>= 1)
		{
			if(row & mask)
			{
				set_pixel(framebuffer->top_left, x + _x, y + _y, 0xFFFFFF);
			}
		}
	}
	return;
}
