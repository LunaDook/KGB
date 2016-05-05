// KGB by Wolfvak
// Common definitions and includes

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned long       u32;
typedef unsigned long long  u64;

typedef signed   char       s8;
typedef signed   short      s16;
typedef signed   long       s32;
typedef signed   long long  s64;

typedef volatile u8	        vu8;
typedef volatile u16        vu16;
typedef volatile u32        vu32;
typedef volatile u64        vu64;

typedef volatile s8	        vs8;
typedef volatile s16        vs16;
typedef volatile s32        vs32;
typedef volatile s64        vs64;

typedef u8  bool;
typedef u32 Handle;

#define true 1
#define false 0

#include "console.h"
#include "draw.h"
#include "fatfs/ff.h"
#include "firm.h"
#include "font.h"
#include "hid.h"
#include "i2c.h"
#include "memory.h"
#include "menu.h"
#include "printf.h"
#include "sha.h"

void error(u32 err_id);
