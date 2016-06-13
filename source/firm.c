#include "common.h"

// From http://3dbrew.org/wiki/Memory_layout#ARM9_ITCM
firm_header *firm = (firm_header*)0x01FFBB00;

void print_fresult(FRESULT f_ret)
{
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

        default:
            print("U_NUTS");
            break;
    }
    print("\n");
    return;
}

int load_firm(const char *firm_filename)
{
	FIL firm_file;
	FRESULT f_ret;
	size_t bytes_read = 0;

	f_ret = f_open(&firm_file, firm_filename, FA_READ);

    print("f_open returned ");
    print_fresult(f_ret);

    // Perform some sanity checks...
	if (f_ret != FR_OK)
		return -1;

	f_ret = f_read(&firm_file, firm, 0x200, &bytes_read);

    print("FIRM f_read returned ");
    print_fresult(f_ret);

	if (f_ret != FR_OK || bytes_read != 0x200)
		return -2;

    if (firm->magic != FIRM_MAGIC) // 'FIRM'
        return -3;
	// End of sanity checks


	// Print some information
	print("ARM11 entrypoint located @ 0x");
	print_hex(firm->arm11_entry);
	print("\nARM9 entrypoint located @ 0x");
	print_hex(firm->arm9_entry);
	print("\n");

	if (N3DS) // Skip arm9loader
	{
		print("new 3DS Detected! Fixing ARM9 entrypoint...\n");
		firm->arm9_entry = 0x0801B01C;
	}

	// Do this for sections 0-3, only if the section's size isn't 0
	// Nowadays, only sections used are 0, 1, and 2 for NFIRM
	for (u32 id = 0; id < 4 && firm->section[id].size; id++)
	{
		print("Section ");
        print_hex(id);
        print(":\n");

		f_lseek(&firm_file, firm->section[id].byte_offset); // Seek to the beggining of the section
		f_read(&firm_file, (void*)firm->section[id].load_address, firm->section[id].size, &bytes_read); // Read the section to its destination address

        print("Dest: 0x");
		print_hex(firm->section[id].load_address);
		print(", len: 0x");
		print_hex(firm->section[id].size);
		print(", offset: 0x");
		print_hex(firm->section[id].byte_offset);
		print("\n");

        // Doubt this'll ever happen, but just in case
        if (bytes_read != firm->section[id].size)
            print("WARNING! Wrong section size read!\n");
	}

	f_close(&firm_file);
	return 1;
}

void launch_firm()
{
    print("Setting ARM11 entrypoint to 0x");
    print_hex(firm->arm11_entry);
    print("\n");
	*(u32*)0x1FFFFFF8 = firm->arm11_entry;

    print("Executing ARM9 entrypoint @ 0x");
	print_hex(firm->arm9_entry);
	((void (*)())firm->arm9_entry)();
}
