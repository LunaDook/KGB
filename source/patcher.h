#include "common.h"

char *memsearch(char *search_start, u32 search_len, char *search_pattern, u32 pattern_len);

int get_process9_addr(char *section, u32 len, char *proc9_addr, u32 *proc9_len);
int patch_process9(char *proc9_addr, u32 proc9_len);
int patch_firmware();

/** P9 ADDRESSES
	P9 = "Process9"
	P9_LOADADDRESS = P9 + 0x10
	P9_MEDIAUNITS  = P9 - 0x100 + 0xA4
*/
