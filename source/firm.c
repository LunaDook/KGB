#include "common.h"

firm_header *firm = (firm_header *)0x24000000;

u8 load_firm(const char *firm_filename)
{
	FIL firm_file;
	size_t bytes_read = 0;

	if (f_open(&firm_file, firm_filename, FA_READ) != FR_OK) // If file could not be read
	{
		return 0;
	}

	f_read(&firm_file, firm, 0x200, &bytes_read); // Attempt to read the FIRM header

	if (bytes_read != 0x200) // If FIRM header size is different than 0x200 (only possibility is lower)...
	{
		return 0;
	}
	
	print("ARM11 entrypoint located @ ");
	print_hex(firm->arm11_entry);
	print("\nARM9 entrypoint located @ ");
	print_hex(firm->arm9_entry);

	print("\n");

	for (u8 id = 0; id < 4 && firm->section[id].size; id++)
	{
		print("\nReading section..");

		f_lseek(&firm_file, firm->section[id].byte_offset); // Seek to the section beggining
		f_read(&firm_file, (void*)firm->section[id].load_address, firm->section[id].size, &bytes_read); // Read the section to its destination address

		print("\nAddr: ");
		print_hex(firm->section[id].load_address);
		print(", sz: ");
		print_hex(firm->section[id].size);
		print(", off: ");
		print_hex(firm->section[id].byte_offset);
		print("\n");

	}

	f_close(&firm_file);
	return 1;
}

void launch_firm()
{
	*(u32 *)0x1FFFFFF8 = firm->arm11_entry;

	print("\nJumping to ARM9 entrypoint @ ");
	print_hex(firm->arm9_entry);
	((void (*)(void))firm->arm9_entry)();
}
