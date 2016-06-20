#include "common.h"

// TODO: Make the console more flexible

u16 console_x = 0, console_y = 0;

framebuffer_t *framebuffer;

void scroll_up(u8 *fb)
{
    for (u32 i = 0; i < 400; i++) // 320 if bottom screen
    {
        memmove(fb + i*720 + 24, fb + i*720, 696);
        memset(fb + i*720, 0, 24); 
    }
}

inline void console_putc(void *fb, char c)
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

	draw_char(fb, console_x, console_y, c);
	console_x += FONT_X;
	return;
}

void console_init()
{
	console_x  = 0;
	console_y  = 0;

	clear_screen(framebuffer->top_left, 0x00);
    init_printf(framebuffer->top_left, console_putc);
	return;
}
