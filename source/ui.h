#pragma once

#include "common.h"

#define FONT_X 8
#define FONT_Y 8

#define TOP_FB_SZ	0x46500
#define SUB_FB_SZ	0x38400

#define TOP_SCREEN0 (u8*)(*(u32*)0x23FFFE00)
#define TOP_SCREEN1 (u8*)(*(u32*)0x23FFFE00)
#define BOT_SCREEN0 (u8*)(*(u32*)0x23FFFE08)
#define BOT_SCREEN1 (u8*)(*(u32*)0x23FFFE08)

#define fb_sz(fb) \
	(fb == BOT_SCREEN0 ? SUB_FB_SZ : TOP_FB_SZ)

#define set_pixel(fb, x, y, rgb) \
{ \
	u32 offset = (240 * (x) + 240 - (y) - 1) * 3; \
	*(u8*)(fb + offset++) = rgb & 0xFF; \
	*(u8*)(fb + offset++) = (rgb >> 8) & 0xFF; \
	*(u8*)(fb + offset++) = (rgb >> 16) & 0xFF; \
}

#define HID_PAD         ((*(volatile u32*)0x10146000) ^ 0xFFF) // XOR'd with 0xFFF because Nintendo loves us

#define KEY_A			(1 << 0)  // A
#define KEY_B			(1 << 1)  // B
#define KEY_SELECT		(1 << 2)  // SELECT
#define KEY_START		(1 << 3)  // START
#define KEY_DRRIGHT		(1 << 4)  // RIGHT
#define KEY_DLEFT		(1 << 5)  // LEFT
#define KEY_DUP			(1 << 6)  // UP
#define KEY_DDOWN		(1 << 7)  // DOWN
#define KEY_RT			(1 << 8)  // RIGHT TRIGGER
#define KEY_LT			(1 << 9)  // LEFT TRIGGER
#define KEY_X			(1 << 10) // X
#define KEY_Y			(1 << 11) // Y

// All of this was taken from https://3dbrew.org/wiki/HID_Registers

extern const char font[1024];

u32 wait_for_key();

void clear_screen(const u8 *fb, const u32 rgb);
void draw_char(u8 *fb, const u16 x, const u16 y, const u8 c);

void console_init();
void console_putc(void *p, char c);
void scroll_up();
