#include "common.h"

void* memcpy(void *dst, const void *src, size_t len) {
	while (len--)
		*(u8*)(dst + len) = *(u8*)(src + len);

	return dst;
}

void* memset(void *dst, int n, size_t len) {
	while(len--)
		*(u8*)(dst + len) = n;

	return dst;
}

int memcmp(void *s1, void *s2, size_t len) {
	for ( ; len-- ; s1++, s2++)
		if (*(u8*)s1 != *(u8*)s2)
			return (*(u8*)s1 - *(u8*)s2);

	return 0;
}

/*void *memsearch(void *src, void *pattern, size_t src_len, size_t pattern_len) {
	void *position = src + src_len;

	while(position--)
		if (memcmp(position, pattern, pattern_len) == 0) return position;

    return NULL;
}*/
// Not needed, patching is external for the time being
