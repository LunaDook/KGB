// KGB by Wolfvak
// Common definitions and includes

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

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

#include <ctr9/aes.h>
#include <ctr9/ctr_system.h>
#include <ctr9/i2c.h>
#include <ctr9/io.h>
#include <ctr9/io/ctr_fatfs.h>

#include "console.h"
#include "draw.h"
#include "firm.h"
#include "font.h"
#include "hid.h"
#include "patcher.h"

void print_fresult(FRESULT f_ret);
void error(const char *err_msg);
