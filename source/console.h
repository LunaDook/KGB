#pragma once

#include "common.h"

void console_init();
void console_putc(const char c);
void print(const char *msg);
void print_hex(const u32 n);
void print_hash(u8 *hash, u32 len);
