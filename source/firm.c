#include "fatfs/ff.h"
#include "firm.h"
#include "memory.h"
#include "console.h"
#include "sha.h"
#include "printf.h"

firm_header *firm;

u8 section_sha256_match(u8 *sha256_hash, u8 *data_buf, size_t len) {

	if (!len)
		return 1;

	u8 data_hash[0x20];
	memset(data_hash, 0, 0x20);

	sha_quick(data_hash, data_buf, len, SHA256_MODE);

	if (memcmp(sha256_hash, data_hash, 0x20) == 0)
		return 1;

	else
		return 0;
}

u8 load_firm(const char *firm_filename)
{
	FIL firm_file;
	unsigned int br = 0;
	if (f_open(&firm_file, firm_filename, FA_READ) != FR_OK)
	{
		return 0;
	}

	firm = (firm_header *)0x24000000;
	f_read(&firm_file, firm, 0x200000, &br);
	f_close(&firm_file);

	if (firm->magic != FIRM_MAGIC)
	{
		print_rgb("FIRM MAGIC MISMATCH\n", RED);
	}

	else
	{
		print_rgb("FIRM Magic is valid\n", GREEN);
	}

	u32 firm_size = 0x200; // Header size

	for (u8 i = 0; i < 4; i++)
	{
		firm_size += firm->section[i].size; // Add the secion size
	}

	if (firm_size >= br)
	{
		print_rgb("READ LESS DATA THAN NECESSARY\n", RED);
		print_rgb("ABORTING...\n", RED);
		return 0;
	}

	else
	{
		print("FIRM size is valid\n");
	}

	for (u8 id = 0; id < 4; id++)
	{
		if (firm->section[id].size)
		{
			printf("\nLoading section type %s\n", firm->section[id].processor_type ? "ARM11" : "ARM9");
			printf("Address 0x%08X, size 0x%X, offset 0x%X\n", firm->section[id].load_address, firm->section[id].size, firm->section[id].byte_offset);
			memcpy( (void *)firm->section[id].load_address,
					(void *)firm + firm->section[id].byte_offset,
					firm->section[id].size);

			if (section_sha256_match(firm->section[id].sha256_hash, (u8*)firm->section[id].load_address, firm->section[id].size))
			{
				print_rgb("SHA256 hashes match!\n", GREEN);
			}

			else
			{
				print_rgb("SHA256 HASH MISMATCH!\n", RED | GREEN); // 0xFF0000 | 0x00FF00 = 0xFFFF00 = yellow (AKA go to kinder and mix red with green)
			}
		}
	}
	return 1;
}

void launch_firm()
{
	*(u32 *)0x1FFFFFF8 = (u32)firm->arm11_entry;
	((void (*)(void))firm->arm9_entry)();
}
