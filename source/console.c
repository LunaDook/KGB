#include "common.h"

u16 console_x = 0, console_y = 0;

framebuffer_t *framebuffer;

void scroll_up(u8 *fb)
{
    for (u32 i = 0; i < 400; i++)
    {
        memmove(fb + i*720 + 24, fb + i*720, 696);
        memset(fb + i*720, 0, 24);
    }
}

inline void console_putc(const char c)
{
	if(c == '\n' || (console_x + FONT_X) > TOP_WIDTH)
	{
		console_x = 0;
		console_y += FONT_Y;
	}

	if(console_y >= HEIGHT)
    {
        scroll_up(framebuffer->top_left);
        console_x = 0;
        console_y = (HEIGHT - FONT_Y);
    }

	if(c == '\r' || c == '\n') return;

	draw_char(console_x, console_y, c);
	console_x += FONT_X;
	return;
}

inline void print(const char *msg)
{
	for (u32 i = 0; i < strlen(msg); i++)
		console_putc(msg[i]);

	return;
}

void print_hex(const u32 n)
{
	char c, i;

    print("0x");

	for (i = 2; i < 9; i += 2) // Pad to 1 byte
	{
		if ((n >> (i*4)) == 0) // TODO: Mix the statements
		{
			break;
		}
	}

	while(i--)
	{
		c = (n >> (i<<2)) & 0xF;
		console_putc((c < 0xA) ? ('0' + c) : ('7' + c));
	}

	return;
}

void console_init()
{
	console_x  = 0;
	console_y  = 0;

	clear_screen(framebuffer->top_left, 0x00);
	return;
}
