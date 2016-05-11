#include "common.h"

firm_header *firm = (firm_header*)0x24000000;

u8 load_firm(const char *firm_filename)
{
	FIL firm_file;
	FRESULT f_ret;
	size_t bytes_read = 0;

	f_ret = f_open(&firm_file, firm_filename, FA_READ);

	// Perform some sanity checks...
	if (f_ret != FR_OK)
	{
		return 0;
	}

	f_ret = f_read(&firm_file, firm, 0x200, &bytes_read);

	if (f_ret != FR_OK || bytes_read != 0x200)
	{
		return 0;
	}
	// End of sanity checks

	// Print some information
	print("ARM11 entrypoint located @ 0x");
	print_hex(firm->arm11_entry);
	print("\nARM9 entrypoint located @ 0x");
	print_hex(firm->arm9_entry);
	print("\n");

	u8 calc_sha256[0x20];

	// Do this for sections 0-3, only if the section's size isn't 0
	// Nowadays, only sections used are 0, 1, and 2
	for (u32 id = 0; id < 4 && firm->section[id].size; id++)
	{
		print("\nReading section...");

		f_lseek(&firm_file, firm->section[id].byte_offset); // Seek to the section beggining
		f_read(&firm_file, (void*)firm->section[id].load_address, firm->section[id].size, &bytes_read); // Read the section to its destination address

		print("\nAddr: 0x");
		print_hex(firm->section[id].load_address);
		print(", size: 0x");
		print_hex(firm->section[id].size);
		print(", offset: 0x");
		print_hex(firm->section[id].byte_offset);
		print("\n");

		sha_quick(calc_sha256, (void*)firm->section[id].load_address, firm->section[id].size, SHA256_MODE);

		if (memcmp(calc_sha256, firm->section[id].sha256_hash, 0x20)) // If sha256 in header doesn't match the calculated one...
		{
			print("SHA256 hash mismatch!\n");
			print_hash(firm->section[id].sha256_hash, 0x20);
			print("\n\n");
			print_hash(calc_sha256, 0x20);
			print("\n");
		}
	}

	f_close(&firm_file);
	return 1;
}

void launch_firm()
{
	*(u32*)0x1FFFFFF8 = firm->arm11_entry;

	print("\nJumping to ARM9 entrypoint @ 0x");
	print_hex(firm->arm9_entry);
	((void (*)())firm->arm9_entry)();
}
