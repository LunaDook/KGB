#include "common.h"

firm_header *firm = (firm_header*)0x27800000;

// Input will be "000000XX.app" - return XX as a regular number
// There's definitely a better way to do it
u8 check_firm_version(char *path)
{
    u8 n = 0;

    for (int i = 7; i >= 0; i--)
    {
        char chr = path[i];
        n += (chr - (chr > '9' ? '7' : '0')) << 4*(7-i);
    }

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
    {
        printf("f_findfirst returned ");
        print_fresult(f_ret);
    }

    // FUCKING GATEWAY DOWNGRADES TO 4.X
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
    memcpy(path + 41, firm_info.fname, 13);

    printf("Using %s\n", firm_info.fname);

    f_ret = f_open(&firm_file, path, FA_READ);
    if (f_ret != FR_OK)
    {
        printf("f_open returned ");
        print_fresult(f_ret);
    }

    if (f_ret != FR_OK)
    {
        f_close(&firm_file);
        return -2;
    }

    size_t firm_len = firm_info.fsize, br;

    f_ret = f_read(&firm_file, (u8*)firm, firm_len, &br);
    if (f_ret != FR_OK)
    {
        printf("f_read returned ");
        print_fresult(f_ret);
    }

    f_ret = f_close(&firm_file);
    if (f_ret != FR_OK)
    {
        printf("f_read returned ");
        print_fresult(f_ret);
    }

    if (f_ret != FR_OK || br != firm_len)
        return -3;

    s32 ret = process_firm(ver);

    if (ret)
        printf("process_firm returned %d\n", ret);

    if (ret < 0)
        return -5;

    return 0;
}

s32 process_firm(u8 version)
{
    decrypt_firm_cxi((u8*)firm);

	if (firm->magic != FIRM_MAGIC) // Check for proper decryption
        return -1;

	// Print some information
	printf("\nARM11 entrypoint located @ 0x%08X\n", firm->arm11_entry);
	printf("ARM9 entrypoint located @ 0x%08X\n\n", firm->arm9_entry);

	if (N3DS) // Run arm9loader
	{
		printf("N3DS Detected!\n");
        u32 ret = decrypt_arm9bin((u8*)firm + firm->section[2].byte_offset, version);
        if (ret != 0)
            printf("decrypt_arm9bin returned %d\n", ret);

		firm->arm9_entry = 0x0801B01C;
	}

	// Do this for sections 0-3, only if the section's size isn't 0
	// Nowadays, only sections used are 0, 1, and 2 for NFIRM
	for (u32 id = 0; id < 4 && firm->section[id].size; id++)
	{
        printf("Section %d:\nDest: 0x%X, len: 0x%X, offset: 0x%X\n\n", id, firm->section[id].load_address, firm->section[id].size, firm->section[id].byte_offset);
        memcpy((u8*)firm->section[id].load_address, (u8*)firm + firm->section[id].byte_offset, firm->section[id].size);
	}

    s32 ret = patch_firmware();
    if (ret != 0)
        printf("patch_firmware returned %d\n", ret);

	return 0;
}

void launch_firm()
{
    printf("Setting ARM11 entrypoint to 0x%08X\n", firm->arm11_entry);
	printf("Executing ARM9 entrypoint @ 0x%08X", firm->arm9_entry);

    ctr_flush_cache();
    *(u32*)0x1FFFFFF8 = firm->arm11_entry;
	((void (*)())firm->arm9_entry)();
}
