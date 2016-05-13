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

void print_hex(const u32 n)
{
	char c;
	u32 i;

	for (i = 2; i < 9; i += 2) // Pad to 1 byte
	{
		if ((n >> (i*4)) == 0) // Should mix the conditionals together, I'll keep it here to keep it clean
		{
			break;
		}
	}

	while(i--)
	{
		char c = (n >> i*4) & 0xF;
		console_putc((c < 0xA) ? ('0' + c) : ('7' + c));
	}

	return;
}

void print_hash(u8 *hash, u32 len)
{
	u32 long_n;
	for (u32 i = 0; i < (len >> 2); i++)
	{
		long_n = 0;
		u32 id = (i << 2);

		if (i == (len >> 3) && len > 12) // Only for formatting purposes
		{
			print("\n");
		}

		for (u32 x = 0; x < 4; x++)
		{
			long_n <<= 8;
			if ((id + x) <= len) // Should take care of the alignment
			{
				long_n += hash[id + x];
			}
		}
		print_hex(long_n);
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
