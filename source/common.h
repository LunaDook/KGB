// KGB by Wolfvak
// Common definitions and includes

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define PAYLOAD_NAME "/arm9loaderhax.bin"
#define DEST_ADDR    0x23F00000
// KGB-stage2 will be loaded to 0x24F0000 in the future
// PAYLOAD_NAME will be loaded to 0x23F00000 in case it exists
// Otherwise KGB-stage2 will attempt to perform a firmlaunch


typedef uint8_t       u8;
typedef uint16_t      u16;
typedef uint32_t      u32;
typedef uint64_t      u64;

typedef int8_t        s8;
typedef int16_t       s16;
typedef int32_t       s32;
typedef int64_t       s64;

typedef volatile u8	  vu8;
typedef volatile u16  vu16;
typedef volatile u32  vu32;
typedef volatile u64  vu64;

typedef volatile s8	  vs8;
typedef volatile s16  vs16;
typedef volatile s32  vs32;
typedef volatile s64  vs64;

typedef u32 Handle;

/*
#define true 0
#define false 1
*/ // Happy debugging motherfuckers

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
#include "sha.h"

void error(const char *err_msg);
