#pragma once

#include "types.h"

#define HEIGHT 240
#define TOP_WIDTH 400
#define BOTTOM_WIDTH 320

#define TOP_FB_SZ (TOP_WIDTH*HEIGHT*3)
#define BOTTOM_FB_SZ (BOTTOM_WIDTH*HEIGHT*3)

typedef struct framebuffer_t { // thsnks to mid-kid for fb offsets
    u8 *top_left;
    u8 *top_right;
    u8 *bottom;
} framebuffer_t;

void clear_screen(u8* fb, u32 rgb);
