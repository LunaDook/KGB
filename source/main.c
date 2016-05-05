#include "common.h"

framebuffer_t *framebuffer;

FATFS sd;

char *firm_fname = "/firmware";

void poweroff()
{
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1);
	while (1); // Loop until it shuts down
}

void error(u32 err_id)
{
	printf("ERROR 0x%08X\nPress the ANY key to power off", err_id);
	input_wait();
	poweroff();
}

void main() {
	console_init();
	if (f_mount(&sd, "0:", 1) != FR_OK)
	{
		error(0x5D015BAD); // SD IS BAD
	}

	printf("Attempting to load FIRM from %s...\n\n", firm_fname);
	int ret = load_firm(firm_fname);
	if (!ret)
	{
		error(0x5D000000); // SD VOID
	}

	print("\nPress [A] to boot FIRM\nPress [B] to power off\n");

	u32 key = 0;

	while(!(key & KEY_B))
	{
		key = input_wait();

		switch (key)
		{
			case KEY_A:
				print_rgb("Booting FIRM", GREEN);
				launch_firm();

			case KEY_B:
				break;
		}
	}

	poweroff();
}
