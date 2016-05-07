#pragma once

#include "common.h"

#define BLACK 0x000000
#define WHITE 0xFFFFFF
#define RED   0xFF0000
#define GREEN 0x00FF00
#define BLUE  0x0000FF

// Print a message with a specified color
#define printf_rgb(rgb, msg, ...) {\
	u32 col = set_fg(rgb); \
	printf(msg, ##__VA_ARGS__);\
	set_fg(col); \
}

void console_init();
void console_putc(void *p, char c);
u32 set_fg(u32 rgb);
u32 set_bg(u32 rgb);
