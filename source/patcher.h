#include "common.h"

u8 *memsearch(u8 *search_start, u32 search_len, u8 *search_pattern, u32 pattern_len);

s32 patch_process9(u8 *proc9_addr, u32 proc9_len);
s32 patch_firmware();

/**
	P9 = "Process9"
	P9_LOADADDRESS = P9 + 0x10
	P9_MEDIAUNITS  = P9 - 0x100 + 0xA4
*/
