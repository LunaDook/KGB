#include "common.h"

u16 console_x = 0, console_y = 0;

framebuffer_t *framebuffer;

inline void console_putc(const char c)
{
	if(c == '\n' || (console_x + FONT_X) > TOP_WIDTH)
	{
		console_x = 0;
		console_y += FONT_Y;
	}

	if(console_y >= HEIGHT)
		console_init(); // Reinitialize the whole console, don't really care about scrolling

	if(c == '\r' || c == '\n') return;

	draw_char(console_x, console_y, c);
	console_x += FONT_X;
	return;
}

inline void print(const char *msg)
{
	size_t msg_len = strlen(msg);
	
	for (u32 i = 0; i < msg_len; i++)
	{
		console_putc(msg[i]);
	}

	return;
}

// Prints n as "0x..."
void print_hex(const u32 n)
{
	char *hexArray = "0123456789ABCDEF", str[] = "0x00000000";
	u8 i;

	for (i = 0; i < 9; i++)
	{
		if ((n >> (i*4)) == 0)
		{
			str[i+2] = 0;
			break;
		}
	}

	u8 end = i + 1;

	for (u8 x = 0; x < (end - 1); x++) {
		str[end - x] = hexArray[((n >> x*4) & 0xF)];
	}

	print(str);

	return;
}

void console_init()
{
	console_x  = 0;
	console_y  = 0;

	clear_screen(framebuffer->top_left, 0x00);
	return;
}
