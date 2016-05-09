#include "common.h"

void *memsearch(void *src, void *pattern, size_t src_len, size_t pattern_len) {
	void *position = src + src_len;

	while(position--)
		if (memcmp(position, pattern, pattern_len) == 0) return position;

    return NULL;
}

// Not needed, patching is external for the time being
