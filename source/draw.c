#include "common.h"

framebuffer_t *framebuffer = (framebuffer_t *)0x23FFFE00;

void clear_screen(const u8 *fb, u32 rgb)
{
	size_t sz = fb_sz(fb)*3, i = 0;

	while (i < sz)
	{
		*(u8*)(fb + i++) = rgb & 0xFF;
		*(u8*)(fb + i++) = (rgb >> 8) & 0xFF;
		*(u8*)(fb + i++) = (rgb >> 16) & 0xFF;
	}
	return;
}

void draw_char(const u16 x, const u16 y, const char c)
{
	u16 _x, _y, _c;

	if (c > 0 && c < 128)
	{
		_c = c*8;
	}
	else
	{
		_c = 0;
	}

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
