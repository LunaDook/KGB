#include "common.h"

/**
	Searches for a pattern in memory
*/
u8 *memsearch(u8 *search_start, u32 search_len, u8 *search_pattern, u32 pattern_len);


/**
	Performs various patches against Process9 (signature and FIRM protection)
*/
s32 patch_process9(u8 *proc9_addr, u32 proc9_len);

/**
	Patches modules
*/
s32 patch_module(u8 *sect_addr, u32 sect_len, char *mod_name, u8 mod_name_len, u8 *mod_cxi, u32 mod_cxi_len);

/**
	Does everything above
*/
s32 patch_firmware();
