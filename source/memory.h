#pragma once

#include "types.h"

void memcpy(void *dest, void *src, u32 size);
void memset(void *dest, int filler, u32 size);
int  memcmp(void *s1, void *s2, u32 len);
void *memsearch(void *src, u32 src_len,
				void *pattern, u32 pattern_len);
