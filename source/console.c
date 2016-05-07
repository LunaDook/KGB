#include "common.h"

u16 console_x = 0, console_y = 0;
u32 console_bg = 0, console_fg = (1 << 24) - 1;

framebuffer_t *framebuffer;

u32 set_fg(u32 rgb)
{
	u32 col_prev = console_fg;
	console_fg = rgb;
	return col_prev;
}

u32 set_bg(u32 rgb)
{
	u32 col_prev = console_bg;
	console_bg = rgb;
	return col_prev;
}

void console_putc(void *p, char c)
{
	if(c == '\n' || (console_x + FONT_X) > TOP_WIDTH)
	{
		console_x = 0;
		console_y += FONT_Y;
	}

	if(console_y >= HEIGHT)
		console_init(); // Reinitialize the whole console, don't really care about scrolling

	if(c == '\r' || c == '\n') return;

	draw_char(console_x, console_y, console_fg, console_bg, c);
	console_x += FONT_X;
	return;
}

void console_init()
{
	console_x  = 0;
	console_y  = 0;
	console_fg = 0xFFFFFF;

	clear_screen(framebuffer->top_left, console_bg); // Clear top screen to current background color
	init_printf(NULL, console_putc);
	return;
}
