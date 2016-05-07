#include "common.h"

firm_header *firm = (firm_header *)0x24000000;

u8 load_firm(const char *firm_filename)
{
	FIL firm_file; // firmware file handle
	size_t bytes_read = 0;

	if (f_open(&firm_file, firm_filename, FA_READ) != FR_OK) // If file could not be read
	{
		return 0;
	}

	f_read(&firm_file, firm, 0x200, &bytes_read); // Attempt to load the FIRM header

	if (bytes_read != 0x200) // If FIRM header size is different than 0x200...
	{
		return 0; // Return an error
	}

	if (firm->magic == FIRM_MAGIC) // Check FIRM magic ('FIRM')
	{
		printf_rgb(GREEN, "FIRM magic is valid\n");
	}

	else
	{
		error("HOUDINI");
	}

	u8 sha256_hash[0x20], sha256_matchflag = 0;

	for (u8 id = 0; id < 4 && firm->section[id].size; id++)
	{
		printf("\nReading %s section...", firm->section[id].processor_type ? "ARM11" : "ARM9");

		f_lseek(&firm_file, firm->section[id].byte_offset); // Seek to the section beggining
		f_read(&firm_file, (void*)firm->section[id].load_address, firm->section[id].size, &bytes_read); // Read the section to its destination address

		printf_rgb(GREEN, " done!\n"); // printf if successful, if it somehow crashed this won't show up
		printf("Address 0x%08X, size 0x%X, offset 0x%X\n", firm->section[id].load_address, firm->section[id].size, firm->section[id].byte_offset); // printf some nice details

		if (firm->section[id].size != bytes_read) // If the section size determined is different than the section size read (only possibility is lower)
		{
			printf_rgb(RED, "\nFIRM section read error\n");
		}

		memset(sha256_hash, 0, 0x20); // Enhanced arm9loader has made me paranoid of using memset everywhere...
		sha_quick(sha256_hash, (void*)firm->section[id].load_address, firm->section[id].size, SHA256_MODE);

		if (memcmp(firm->section[id].sha256_hash, sha256_hash, 0x20) == 0)
		{
			sha256_matchflag = 1;
		}

		else
		{
			sha256_matchflag = 0;

			printf("\nHashes:\n");

			for (u8 i = 0; i < 20; i++)
			{
				printf("%02X", firm->section[id].sha256_hash[i]);
			}

			printf("\n");
			for (u8 i = 0; i < 20; i++)
			{
				printf("%02X", sha256_hash[i]);
			}
			printf("\n");
		}

		printf_rgb(sha256_matchflag ? GREEN : (RED | GREEN), "SHA256 hash %s!\n", sha256_matchflag ? "matches" : "mismatch");
	}

	f_close(&firm_file);
	return 1;
}

void launch_firm()
{
	*(u32 *)0x1FFFFFF8 = firm->arm11_entry;
	((void (*)(void))firm->arm9_entry)();
}
