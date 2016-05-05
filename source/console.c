#include "common.h"

typedef struct console_s {
	u32 x;
	u32 y;
	u32 color;
} console_s;

static console_s console;

framebuffer_t *framebuffer;

u32 set_fg(u32 rgb)
{
	u32 col_prev = console.color;
	console.color = rgb;
	return col_prev;
}

void console_putc(void *p, char c)
{
	if(c == '\n' || (console.x + 8) > TOP_WIDTH)
	{
		console.x = 0;
		console.y += 8;
	}

	if(console.y >= HEIGHT)
		console_init(); // Reinitialize the whole console, don't really care about scrolling

	if(c == '\r' || c == '\n') return;

	draw_char(console.x, console.y, console.color, c);
	console.x += 8;
}

size_t strlen(const char *str) {
	size_t retval = 0;

	while(*str++ != '\0')
		retval++;

	return retval;
}

void print(char *str)
{
	size_t len = strlen(str);

	for (u32 i = 0; i < len; i++)
		console_putc(NULL, str[i]);

	return;
}

void print_rgb(char *msg, u32 rgb)
{
	u32 col = set_fg(rgb);
	print(msg);
	set_fg(col);
	return;
}

void console_init()
{
	console.x = 0;
	console.y = 0;
	console.color = (1 << 24) - 1; // 0xFFFFFF (white)

	clear_screen(framebuffer->top_left, 0x00); // Clear top screen to black
	init_printf(NULL, console_putc);
}
