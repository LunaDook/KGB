#include "common.h"

firm_header *firm = (firm_header *)0x24000000;

u8 section_sha256_match(u8 *sha256_hash, u8 *data_buf, size_t len) {
	u8 data_hash[0x20];
	sha_quick(data_hash, data_buf, len, SHA256_MODE);

	if (memcmp(sha256_hash, data_hash, 0x20) == 0)
		return 1;

	return 0;
}

u8 load_firm(const char *firm_filename)
{
	FIL firm_file;
	size_t br = 0;

	if (f_open(&firm_file, firm_filename, FA_READ) != FR_OK)
	{
		return 0;
	}

	f_read(&firm_file, firm, 0x200, &br); // Only load header

	if (br != 0x200)
	{
		return 0;
	}

	if (firm->magic == FIRM_MAGIC)
	{
		print_rgb("FIRM Magic is valid\n", GREEN);
	}

	else
	{
		error(0x00B1A10E); // BLAINE
	}

	u8 match;
	u32 col;

	for (u8 id = 0; id < 4; id++)
	{
		if (firm->section[id].size)
		{
			printf("\nReading %s section...", firm->section[id].processor_type ? "ARM11" : "ARM9");

			f_lseek(&firm_file, firm->section[id].byte_offset);
			f_read(&firm_file, (void*)firm->section[id].load_address, firm->section[id].size, &br);

			print(" done!\n");
			printf("Address 0x%08X, size 0x%X, offset 0x%X\n", firm->section[id].load_address, firm->section[id].size, firm->section[id].byte_offset);

			if (firm->section[id].size != br)
			{
				print_rgb("\nFIRM section size error\n", RED);
			}

			match = section_sha256_match(firm->section[id].sha256_hash, (u8*)firm->section[id].load_address, firm->section[id].size);

			col = set_fg(match ? GREEN : (RED | GREEN));
			printf("SHA256 hash %s!\n", match ? "matches" : "mismatch");
			set_fg(col);
		}
	}
	f_close(&firm_file);
	return 1;
}

void launch_firm()
{
	*(u32 *)0x1FFFFFF8 = (u32)firm->arm11_entry;
	((void (*)(void))firm->arm9_entry)();
}
