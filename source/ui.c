#include "common.h"

u16 console_x = 0, console_y = 0;

// Wouldn't rely too much on this function...
u32 wait_for_key()
{
    u32 pad = HID_PAD, newpad;
    while(1)
    {
        newpad = HID_PAD;
        if (newpad != pad)
            break;
    }

    return newpad;
}

void clear_screen(const u8 *fb, const u32 rgb)
{
	u32 sz = fb_sz(fb), i = 0;

	while (i < sz)
	{
		*(u8*)(fb + i++) = rgb & 0xFF;
		*(u8*)(fb + i++) = (rgb >> 8) & 0xFF;
		*(u8*)(fb + i++) = (rgb >> 16) & 0xFF;
	}
}

// routine is from github.com/b1l1s/ctr
void draw_char(u8 *fb, const u16 x, const u16 y, const u8 c)
{
	u16 _x, _y, _c = 0;

	if (c < 0x80)
		_c = c*8;

	for(_y = 0; _y < FONT_Y; _y++)
	{
		u8 mask = 0b10000000;
		u8 row = font[_y + _c];
		for(_x = 0; _x < FONT_X; _x++, mask >>= 1)
		{
			if(row & mask)
				set_pixel(fb, x + _x, y + _y, 0xFFFFFF);
		}
	}
}

void scroll_up(u8 *fb)
{
    for (u32 i = 0; i < (fb == TOP_SCREEN0 ? 400 : 320); i++)
    {
        memmove(fb + (i * 720) + 24, fb + i*720, 696);
        memset(fb + (i * 720), 0, 24); 
    }
}

inline void console_putc(void *fb, char c)
{
	if(c == '\n' || (console_x + FONT_X) > (fb == TOP_SCREEN0 ? 400 : 320))
	{
		console_x = 0;
		console_y += FONT_Y;
	}

	if(console_y >= 240)
    {
        scroll_up(fb);
        console_x = 0;
        console_y = 240 - FONT_Y;
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

	clear_screen(TOP_SCREEN0, 0x00);
    init_printf(TOP_SCREEN0, console_putc);
	return;
}

const char font[1024] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 000 (.)
	0x7E, 0x81, 0xA5, 0x81, 0xBD, 0x99, 0x81, 0x7E,	// Char 001 (.)
	0x7E, 0xFF, 0xDB, 0xFF, 0xC3, 0xE7, 0xFF, 0x7E,	// Char 002 (.)
	0x6C, 0xFE, 0xFE, 0xFE, 0x7C, 0x38, 0x10, 0x00,	// Char 003 (.)
	0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x10, 0x00,	// Char 004 (.)
	0x38, 0x7C, 0x38, 0xFE, 0xFE, 0x7C, 0x38, 0x7C,	// Char 005 (.)
	0x10, 0x10, 0x38, 0x7C, 0xFE, 0x7C, 0x38, 0x7C,	// Char 006 (.)
	0x00, 0x00, 0x18, 0x3C, 0x3C, 0x18, 0x00, 0x00,	// Char 007 (.)
	0xFF, 0xFF, 0xE7, 0xC3, 0xC3, 0xE7, 0xFF, 0xFF,	// Char 008 (.)
	0x00, 0x3C, 0x66, 0x42, 0x42, 0x66, 0x3C, 0x00,	// Char 009 (.)
	0xFF, 0xC3, 0x99, 0xBD, 0xBD, 0x99, 0xC3, 0xFF,	// Char 010 (.)
	0x0F, 0x07, 0x0F, 0x7D, 0xCC, 0xCC, 0xCC, 0x78,	// Char 011 (.)
	0x3C, 0x66, 0x66, 0x66, 0x3C, 0x18, 0x7E, 0x18,	// Char 012 (.)
	0x3F, 0x33, 0x3F, 0x30, 0x30, 0x70, 0xF0, 0xE0,	// Char 013 (.)
	0x7F, 0x63, 0x7F, 0x63, 0x63, 0x67, 0xE6, 0xC0,	// Char 014 (.)
	0x99, 0x5A, 0x3C, 0xE7, 0xE7, 0x3C, 0x5A, 0x99,	// Char 015 (.)
	0x80, 0xE0, 0xF8, 0xFE, 0xF8, 0xE0, 0x80, 0x00,	// Char 016 (.)
	0x02, 0x0E, 0x3E, 0xFE, 0x3E, 0x0E, 0x02, 0x00,	// Char 017 (.)
	0x18, 0x3C, 0x7E, 0x18, 0x18, 0x7E, 0x3C, 0x18,	// Char 018 (.)
	0x66, 0x66, 0x66, 0x66, 0x66, 0x00, 0x66, 0x00,	// Char 019 (.)
	0x7F, 0xDB, 0xDB, 0x7B, 0x1B, 0x1B, 0x1B, 0x00,	// Char 020 (.)
	0x3E, 0x63, 0x38, 0x6C, 0x6C, 0x38, 0xCC, 0x78,	// Char 021 (.)
	0x00, 0x00, 0x00, 0x00, 0x7E, 0x7E, 0x7E, 0x00,	// Char 022 (.)
	0x18, 0x3C, 0x7E, 0x18, 0x7E, 0x3C, 0x18, 0xFF,	// Char 023 (.)
	0x18, 0x3C, 0x7E, 0x18, 0x18, 0x18, 0x18, 0x00,	// Char 024 (.)
	0x18, 0x18, 0x18, 0x18, 0x7E, 0x3C, 0x18, 0x00,	// Char 025 (.)
	0x00, 0x18, 0x0C, 0xFE, 0x0C, 0x18, 0x00, 0x00,	// Char 026 (.)
	0x00, 0x30, 0x60, 0xFE, 0x60, 0x30, 0x00, 0x00,	// Char 027 (.)
	0x00, 0x00, 0xC0, 0xC0, 0xC0, 0xFE, 0x00, 0x00,	// Char 028 (.)
	0x00, 0x24, 0x66, 0xFF, 0x66, 0x24, 0x00, 0x00,	// Char 029 (.)
	0x00, 0x18, 0x3C, 0x7E, 0xFF, 0xFF, 0x00, 0x00,	// Char 030 (.)
	0x00, 0xFF, 0xFF, 0x7E, 0x3C, 0x18, 0x00, 0x00,	// Char 031 (.)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 032 ( )
	0x30, 0x78, 0x78, 0x78, 0x30, 0x00, 0x30, 0x00,	// Char 033 (!)
	0x6C, 0x6C, 0x6C, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 034 (")
	0x6C, 0x6C, 0xFE, 0x6C, 0xFE, 0x6C, 0x6C, 0x00,	// Char 035 (#)
	0x30, 0x7C, 0xC0, 0x78, 0x0C, 0xF8, 0x30, 0x00,	// Char 036 ($)
	0x00, 0xC6, 0xCC, 0x18, 0x30, 0x66, 0xC6, 0x00,	// Char 037 (%)
	0x38, 0x6C, 0x38, 0x76, 0xDC, 0xCC, 0x76, 0x00,	// Char 038 (&)
	0x60, 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 039 (')
	0x18, 0x30, 0x60, 0x60, 0x60, 0x30, 0x18, 0x00,	// Char 040 (()
	0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60, 0x00,	// Char 041 ())
	0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00,	// Char 042 (*)
	0x00, 0x30, 0x30, 0xFC, 0x30, 0x30, 0x00, 0x00,	// Char 043 (+)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x60,	// Char 044 (,)
	0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x00,	// Char 045 (-)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00,	// Char 046 (.)
	0x06, 0x0C, 0x18, 0x30, 0x60, 0xC0, 0x80, 0x00,	// Char 047 (/)
	0x7C, 0xC6, 0xCE, 0xDE, 0xF6, 0xE6, 0x7C, 0x00,	// Char 048 (0)
	0x30, 0x70, 0x30, 0x30, 0x30, 0x30, 0xFC, 0x00,	// Char 049 (1)
	0x78, 0xCC, 0x0C, 0x38, 0x60, 0xCC, 0xFC, 0x00,	// Char 050 (2)
	0x78, 0xCC, 0x0C, 0x38, 0x0C, 0xCC, 0x78, 0x00,	// Char 051 (3)
	0x1C, 0x3C, 0x6C, 0xCC, 0xFE, 0x0C, 0x1E, 0x00,	// Char 052 (4)
	0xFC, 0xC0, 0xF8, 0x0C, 0x0C, 0xCC, 0x78, 0x00,	// Char 053 (5)
	0x38, 0x60, 0xC0, 0xF8, 0xCC, 0xCC, 0x78, 0x00,	// Char 054 (6)
	0xFC, 0xCC, 0x0C, 0x18, 0x30, 0x30, 0x30, 0x00,	// Char 055 (7)
	0x78, 0xCC, 0xCC, 0x78, 0xCC, 0xCC, 0x78, 0x00,	// Char 056 (8)
	0x78, 0xCC, 0xCC, 0x7C, 0x0C, 0x18, 0x70, 0x00,	// Char 057 (9)
	0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x00,	// Char 058 (:)
	0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x60,	// Char 059 (;)
	0x18, 0x30, 0x60, 0xC0, 0x60, 0x30, 0x18, 0x00,	// Char 060 (<)
	0x00, 0x00, 0xFC, 0x00, 0x00, 0xFC, 0x00, 0x00,	// Char 061 (=)
	0x60, 0x30, 0x18, 0x0C, 0x18, 0x30, 0x60, 0x00,	// Char 062 (>)
	0x78, 0xCC, 0x0C, 0x18, 0x30, 0x00, 0x30, 0x00,	// Char 063 (?)
	0x7C, 0xC6, 0xDE, 0xDE, 0xDE, 0xC0, 0x78, 0x00,	// Char 064 (@)
	0x30, 0x78, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0x00,	// Char 065 (A)
	0xFC, 0x66, 0x66, 0x7C, 0x66, 0x66, 0xFC, 0x00,	// Char 066 (B)
	0x3C, 0x66, 0xC0, 0xC0, 0xC0, 0x66, 0x3C, 0x00,	// Char 067 (C)
	0xF8, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0xF8, 0x00,	// Char 068 (D)
	0x7E, 0x60, 0x60, 0x78, 0x60, 0x60, 0x7E, 0x00,	// Char 069 (E)
	0x7E, 0x60, 0x60, 0x78, 0x60, 0x60, 0x60, 0x00,	// Char 070 (F)
	0x3C, 0x66, 0xC0, 0xC0, 0xCE, 0x66, 0x3E, 0x00,	// Char 071 (G)
	0xCC, 0xCC, 0xCC, 0xFC, 0xCC, 0xCC, 0xCC, 0x00,	// Char 072 (H)
	0x78, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00,	// Char 073 (I)
	0x1E, 0x0C, 0x0C, 0x0C, 0xCC, 0xCC, 0x78, 0x00,	// Char 074 (J)
	0xE6, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0xE6, 0x00,	// Char 075 (K)
	0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7E, 0x00,	// Char 076 (L)
	0xC6, 0xEE, 0xFE, 0xFE, 0xD6, 0xC6, 0xC6, 0x00,	// Char 077 (M)
	0xC6, 0xE6, 0xF6, 0xDE, 0xCE, 0xC6, 0xC6, 0x00,	// Char 078 (N)
	0x38, 0x6C, 0xC6, 0xC6, 0xC6, 0x6C, 0x38, 0x00,	// Char 079 (O)
	0xFC, 0x66, 0x66, 0x7C, 0x60, 0x60, 0xF0, 0x00,	// Char 080 (P)
	0x78, 0xCC, 0xCC, 0xCC, 0xDC, 0x78, 0x1C, 0x00,	// Char 081 (Q)
	0xFC, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0xE6, 0x00,	// Char 082 (R)
	0x78, 0xCC, 0xE0, 0x70, 0x1C, 0xCC, 0x78, 0x00,	// Char 083 (S)
	0xFC, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00,	// Char 084 (T)
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xFC, 0x00,	// Char 085 (U)
	0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x00,	// Char 086 (V)
	0xC6, 0xC6, 0xC6, 0xD6, 0xFE, 0xEE, 0xC6, 0x00,	// Char 087 (W)
	0xC6, 0xC6, 0x6C, 0x38, 0x38, 0x6C, 0xC6, 0x00,	// Char 088 (X)
	0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x30, 0x78, 0x00,	// Char 089 (Y)
	0xFE, 0x06, 0x0C, 0x18, 0x30, 0x60, 0xFE, 0x00,	// Char 090 (Z)
	0x78, 0x60, 0x60, 0x60, 0x60, 0x60, 0x78, 0x00,	// Char 091 ([)
	0xC0, 0x60, 0x30, 0x18, 0x0C, 0x06, 0x02, 0x00,	// Char 092 (\)
	0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x00,	// Char 093 (])
	0x10, 0x38, 0x6C, 0xC6, 0x00, 0x00, 0x00, 0x00,	// Char 094 (^)
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,	// Char 095 (_)
	0x30, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 096 (`)
	0x00, 0x00, 0x78, 0x0C, 0x7C, 0xCC, 0x76, 0x00,	// Char 097 (a)
	0xE0, 0x60, 0x60, 0x7C, 0x66, 0x66, 0xDC, 0x00,	// Char 098 (b)
	0x00, 0x00, 0x78, 0xCC, 0xC0, 0xCC, 0x78, 0x00,	// Char 099 (c)
	0x1C, 0x0C, 0x0C, 0x7C, 0xCC, 0xCC, 0x76, 0x00,	// Char 100 (d)
	0x00, 0x00, 0x78, 0xCC, 0xFC, 0xC0, 0x78, 0x00,	// Char 101 (e)
	0x38, 0x6C, 0x60, 0xF0, 0x60, 0x60, 0xF0, 0x00,	// Char 102 (f)
	0x00, 0x00, 0x76, 0xCC, 0xCC, 0x7C, 0x0C, 0xF8,	// Char 103 (g)
	0xE0, 0x60, 0x6C, 0x76, 0x66, 0x66, 0xE6, 0x00,	// Char 104 (h)
	0x30, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00,	// Char 105 (i)
	0x0C, 0x00, 0x0C, 0x0C, 0x0C, 0xCC, 0xCC, 0x78,	// Char 106 (j)
	0xE0, 0x60, 0x66, 0x6C, 0x78, 0x6C, 0xE6, 0x00,	// Char 107 (k)
	0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00,	// Char 108 (l)
	0x00, 0x00, 0xCC, 0xFE, 0xFE, 0xD6, 0xC6, 0x00,	// Char 109 (m)
	0x00, 0x00, 0xF8, 0xCC, 0xCC, 0xCC, 0xCC, 0x00,	// Char 110 (n)
	0x00, 0x00, 0x78, 0xCC, 0xCC, 0xCC, 0x78, 0x00,	// Char 111 (o)
	0x00, 0x00, 0xDC, 0x66, 0x66, 0x7C, 0x60, 0xF0,	// Char 112 (p)
	0x00, 0x00, 0x76, 0xCC, 0xCC, 0x7C, 0x0C, 0x1E,	// Char 113 (q)
	0x00, 0x00, 0xDC, 0x76, 0x66, 0x60, 0xF0, 0x00,	// Char 114 (r)
	0x00, 0x00, 0x7C, 0xC0, 0x78, 0x0C, 0xF8, 0x00,	// Char 115 (s)
	0x10, 0x30, 0x7C, 0x30, 0x30, 0x34, 0x18, 0x00,	// Char 116 (t)
	0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xCC, 0x76, 0x00,	// Char 117 (u)
	0x00, 0x00, 0xCC, 0xCC, 0xCC, 0x78, 0x30, 0x00,	// Char 118 (v)
	0x00, 0x00, 0xC6, 0xD6, 0xFE, 0xFE, 0x6C, 0x00,	// Char 119 (w)
	0x00, 0x00, 0xC6, 0x6C, 0x38, 0x6C, 0xC6, 0x00,	// Char 120 (x)
	0x00, 0x00, 0xCC, 0xCC, 0xCC, 0x7C, 0x0C, 0xF8,	// Char 121 (y)
	0x00, 0x00, 0xFC, 0x98, 0x30, 0x64, 0xFC, 0x00,	// Char 122 (z)
	0x1C, 0x30, 0x30, 0xE0, 0x30, 0x30, 0x1C, 0x00,	// Char 123 ({)
	0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00,	// Char 124 (|)
	0xE0, 0x30, 0x30, 0x1C, 0x30, 0x30, 0xE0, 0x00,	// Char 125 (})
	0x76, 0xDC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Char 126 (~)
	0x00, 0x10, 0x38, 0x6C, 0xC6, 0xC6, 0xFE, 0x00,	// Char 127 (.)
};
