#pragma once

#include "draw.h"

#define BLACK 0x000000
#define WHITE 0xFFFFFF
#define RED   0xFF0000
#define GREEN 0x00FF00
#define BLUE  0x0000FF

void console_init();
void print(char *msg);
void console_putc(void *p, char c);
void print_rgb(char *msg, u32 rgb);

u32 set_fg(u32 rgb);
