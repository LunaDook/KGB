#include "fatfs/ff.h"
#include "firm.h"
#include "memory.h"

firm_header *firm[2]; // Here only in case I ever work with AGB/TWL

int load_firm(const char* firm_filename, int firmID) {

	FIL firm_file;
	unsigned int br = 0;
	if (f_open(&firm_file, firm_filename, FA_READ) != FR_OK)
		return 0;

	firm[firmID] = (firm_header *)(!firmID ? 0x24000000 : 0x24200000);
	f_read(&firm_file, firm[firmID], 0x200000, &br); // Load 2MB, biggest FIRM in the wild is TWL_FIRM which is ~1.6MB
	f_close(&firm_file);

	return 1;
}

void launch_firm(int firmID) {
	for (u32 id = 0; id < 4; id++)
		if (firm[firmID]->section[id].size)
			memcpy( (void*)firm[firmID]->section[id].load_address,
					(void*)firm[firmID] + firm[firmID]->section[id].byte_offset,
					firm[firmID]->section[id].size);

	*(u32*)0x1FFFFFF8 = (u32)firm[firmID]->arm11_entry;
	((void (*)(void))firm[firmID]->arm9_entry)();
}
