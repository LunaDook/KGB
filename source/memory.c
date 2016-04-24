#include "memory.h"

/*
 Although it's pretty trivial for an engineer, I'm pretty proud of myself
 I rolled my own memory functions from scratch
 And somehow they work...
 I'm thinking there was some divine intervention here
*/

void memcpy(void *dest, void *src, u32 size) {
	while (size--)
		*(u8*)(dest + size) = *(u8*)(src + size);

	return;
}

void memset(void *dest, int filler, u32 size) {
	while(size--)
		*(u8*)(dest + size) = filler;

	return;
}

int memcmp(void *s1, void *s2, u32 len) {

	for ( ; len-- ; s1++, s2++)
		if (*(u8*)s1 != *(u8*)s2)
			return (*(u8*)s1 - *(u8*)s2);

	return 0;
}

void *memsearch(void *src, u32 src_len, void *pattern, u32 pattern_len) {
	void *position = src + src_len;

	while(position--)
		if (!memcmp(position, pattern, pattern_len)) return position;

    return NULL;
}
