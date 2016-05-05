// KGB by Wolfvak
// Memory functions

#pragma once

#include "common.h"

void *memcpy(void *dst, const void *src, size_t len);
void *memset(void *dst, int n, size_t len);
int memcmp(void *s1, void *s2, size_t len);
//void *memsearch(void *src, void *pattern, size_t src_len, size_t pattern_len);
