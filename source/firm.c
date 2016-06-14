#include "common.h"

firm_header *firm = (firm_header*)0x25000000;

void print_fresult(FRESULT f_ret)
{ // switch, because I got bored of seeing ints
    print("FR_");
    switch(f_ret)
    {
        case FR_OK:
            print("OK");
            break;
        case FR_DISK_ERR:
            print("DISK_ERR");
            break;
        case FR_INT_ERR:
            print("INT_ERR");
            break;
        case FR_NOT_READY:
            print("NOT_READY");
            break;
        case FR_NO_FILE:
            print("NO_FILE");
            break;
        case FR_NO_PATH:
            print("NO_PATH");
            break;
        case FR_INVALID_NAME:
            print("INVALID_NAME");
            break;
        case FR_DENIED:
            print("DENIED");
            break;
        case FR_EXIST:
            print("EXIST");
            break;
        case FR_INVALID_OBJECT:
            print("INVALID_OBJECT");
            break;
        case FR_WRITE_PROTECTED:
            print("WRITE_PROTECTED");
            break;
        case FR_INVALID_DRIVE:
            print("INVALID_DRIVE");
            break;
        case FR_NOT_ENABLED:
            print("NOT_ENABLED");
            break;
        case FR_NO_FILESYSTEM:
            print("NO_FILESYSTEM");
            break;
        case FR_TIMEOUT:
            print("TIMEOUT");
            break;
        case FR_LOCKED:
            print("LOCKED");
            break;
        case FR_NOT_ENOUGH_CORE:
            print("NOT_ENOUGH_CORE");
            break;
        case FR_TOO_MANY_OPEN_FILES:
            print("TOO_MANY_OPEN_FILES");
            break;

        default: // This should never happen
            print("U_NUTS"); // Fun fact: it already happened twice
            break;
    }

    print("\n");
    return;
}

// Stubbed
u8 check_firm_version(char *path)
{
    return 0xFF; // TODO: Actually check the thing, right now just make sure you don't have a GW downgrade or something
}

s32 load_firm()
{
    // Path: "0:/title/00040138/00000002/content/000000??.app"
    FRESULT f_ret;
    DIR     content_dir;
    FILINFO firm_info;
    FIL     firm_file;

    char path[48] = "0:/title/00040138/00000002/content";

    if (N3DS) path[18] = '2'; // As soon as I add N3DS support

    f_ret = f_findfirst(&content_dir, &firm_info, path, "*.app"); // Search for FIRM under the content folder
    print("f_findfirst returned ");
    print_fresult(f_ret);

    while (f_ret == FR_OK && firm_info.fname[0]) {
        if (check_firm_version(firm_info.fname) >= MINVER) // If the found FIRM is good
            break;

        f_ret = f_findnext(&content_dir, &firm_info); // Otherwise, keep searching
    }

    if (!firm_info.fname[0])
        return -1; // FIRM not found (somehow...)

    path[34] = '/';
    memcpy(path + 35, firm_info.fname, 13);
    
    print("Using ");
    print(path);
    print("\n");

    f_ret = f_open(&firm_file, path, FA_READ);
    print("f_open returned ");
    print_fresult(f_ret);

    if (f_ret != FR_OK)
    {
        f_close(&firm_file);
        return -2;
    }

    unsigned int firm_len = firm_info.fsize, br;

    f_ret = f_read(&firm_file, (void*)firm, firm_len, &br);

    print("f_read returned ");
    print_fresult(f_ret);

    f_ret = f_close(&firm_file);

    print("f_close returned ");
    print_fresult(f_ret);

    if (f_ret != FR_OK)
        return -3;

    if (br != firm_len)
        return -4;

    s32 ret = process_firm(firm_len);
    print("process_firm returned ");
    print_hex(ret);
    print("\n");

    if (ret < 0)
        return -5;

    return 0;
}

s32 process_firm()
{
    decrypt_firm_ncch((u8*)firm);

	if (firm->magic != FIRM_MAGIC) // 'FIRM'
        return -1;
    // Check for proper decryption

	// Print some information
	print("\nARM11 entrypoint located @ ");
	print_hex(firm->arm11_entry);
	print("\nARM9 entrypoint located @ ");
	print_hex(firm->arm9_entry);
	print("\n");

	if (N3DS) // Skip arm9loader
	{
		print("N3DS Detected! Fixing ARM9 entrypoint...\n");
        // TODO: Run arm9loader here
		firm->arm9_entry = 0x0801B01C;
	}

	// Do this for sections 0-3, only if the section's size isn't 0
	// Nowadays, only sections used are 0, 1, and 2 for NFIRM
	for (u32 id = 0; id < 4 && firm->section[id].size; id++)
	{
		print("\nSection ");
        print_hex(id);

        print(":\nDest: ");
		print_hex(firm->section[id].load_address);
		print(", len: ");
		print_hex(firm->section[id].size);
		print(", offset: ");
		print_hex(firm->section[id].byte_offset);
		print("\n");

        memcpy((u8*)firm->section[id].load_address, (u8*)firm + firm->section[id].byte_offset, firm->section[id].size);
	}

    s32 pch_ret = patch_firmware();
    print("\npatch_firmware returned ");
    print_hex(pch_ret);
    print("\n");

	return 0;
}

void launch_firm()
{
    print("Setting ARM11 entrypoint to ");
    print_hex(firm->arm11_entry);
    print("\n");
	*(u32*)0x1FFFFFF8 = firm->arm11_entry;

    print("Executing ARM9 entrypoint @ ");
	print_hex(firm->arm9_entry);
	((void (*)())firm->arm9_entry)();
}
