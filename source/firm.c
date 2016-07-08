#include "common.h"

vu32 *a11_entry = (vu32*)0x1FFFFFF8;

//https://github.com/mid-kid/CakesForeveryWan/commit/e810bd0b05c7a5b03ff38e1bbb214781cefffa1d
void __attribute__((naked)) disable_lcds()
{
    *a11_entry = 0;  // Don't wait for us

    *(volatile uint32_t *)0x10202A44 = 0;
    *(volatile uint32_t *)0x10202244 = 0;
    *(volatile uint32_t *)0x1020200C = 0;
    *(volatile uint32_t *)0x10202014 = 0;

    while (!*a11_entry);
    ((void (*)())*a11_entry)();
}

firm_header *firm = (firm_header*)FIRM_LOC;

// Input will be "000000XX.app" - return XX as a regular number
// There's definitely a better way to do this
u8 check_firm_version(char *path)
{
    u8 n = 0;

    for (s32 i = 7; i >= 0; i--)
        n += (path[i] - (path[i] > '9' ? '7' : '0')) << 4*(7-i);

    return n;
}

s32 load_firm()
{
    // Path: "CTRNAND:/title/00040138/00000002/content/000000??.app"
    FRESULT f_ret;
    DIR     content_dir;
    FILINFO firm_info;
    FIL     firm_file;

    char path[54] = "CTRNAND:/title/00040138/00000002/content", ver = 0;

    if (N3DS)
        path[24] = '2';

    f_ret = f_findfirst(&content_dir, &firm_info, path, "*.app"); // Search for FIRM under the content folder
    if (f_ret != FR_OK)
        printf("f_findfirst returned %s\n", ff_err[f_ret]);

    // DAMMIT GATEWAY
    while (f_ret == FR_OK && firm_info.fname[0])
    {
        ver = check_firm_version(firm_info.fname);
        if (ver >= MINVER) // FIRM version is supported
            break;

        f_ret = f_findnext(&content_dir, &firm_info); // Keep searching
    }

    if (!firm_info.fname[0])
        return -1; // FIRM not found

    path[40] = '/';

    for (u8 i = 0; i < 12; i++)
        path[41 + i] = firm_info.fname[i];

    path[53] = 0; // zero-terminate

    printf("Using %s\n\n", path + 14); // Skip "CTRNAND:/title"

    f_ret = f_open(&firm_file, path, FA_READ);
    if (f_ret != FR_OK)
    {
        printf("f_open returned %s", ff_err[f_ret]);
        return -2;
    }

    size_t br;

    f_ret = f_read(&firm_file, (u8*)firm, firm_info.fsize, &br);
    f_close(&firm_file);
    if (f_ret != FR_OK || br != firm_info.fsize)
    {
        printf("f_read returned %s", ff_err[f_ret]);
        return -3;
    }

    s32 ret = process_firm(ver);
    if (ret)
    {
        printf("process_firm returned %d\n", ret);
        return -4;
    }

    return 0;
}

s32 process_firm(u8 version)
{
    decrypt_firm_cxi((u8*)firm);

	if (firm->magic != FIRM_MAGIC) // Check for proper decryption
        return -1;

	// Print some information
	printf("ARM11 entry @ 0x%08X\nARM9 entry @ 0x%08X\n\n", firm->arm11_entry, firm->arm9_entry);

	if (N3DS) // Run arm9loader
	{
        s32 ret = decrypt_arm9bin((u8*)firm + firm->section[2].byte_offset, firm->section[2].size, version);
        if (ret != 0)
            printf("decrypt_arm9bin returned %d\n", ret);

		firm->arm9_entry = 0x0801B01C;
	}

	// Nowadays, only sections used are 0, 1, and 2 for NFIRM
	for (u32 id = 0; id < 4 && firm->section[id].size; id++)
	{
        printf("FIRM section %d:\nDest: 0x%08X, len: 0x%X, offset: 0x%X\n\n", id, firm->section[id].load_address, firm->section[id].size, firm->section[id].byte_offset);
        memcpy((u8*)firm->section[id].load_address, (u8*)firm + firm->section[id].byte_offset, firm->section[id].size);
	}

    s32 ret = patch_firmware();
    if (ret != 0)
        printf("patch_firmware returned %d\n", ret);

	return 0;
}

void launch_firm()
{
    ctr_flush_cache();

    *a11_entry = (u32)disable_lcds;
    while (*a11_entry);  // Make sure it jumped there correctly before changing it.

    *(u32*)0x1FFFFFF8 = firm->arm11_entry;
	((void (*)())firm->arm9_entry)();
}
