#include "common.h"

framebuffer_t *framebuffer;

FATFS sd;

char *firm_fname = "/firmware";

void poweroff()
{
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1);
	for(;;)
		;
}

void error(const char *err_msg)
{
	printf_rgb(RED, "ERROR: %s\nPress the ANY key to power off", err_msg);
	input_wait();
	poweroff();
}

void main()
{
	console_init();
	printf("\n");

	if (f_mount(&sd, "0:", 1) != FR_OK)
	{
		error("couldn't mount the SD card");
	}

	printf("Attempting to load FIRM from %s...\n\n", firm_fname);
	int ret = load_firm(firm_fname);

	if (!ret)
	{
		error("couldn't load firmware");
	}

	printf("\nPress [A] to boot FIRM\nPress [B] to power off\n");

	u32 key = 0;

	for(;;)
	{
		key = input_wait();

		if (key & KEY_A)
		{
			printf_rgb(GREEN, "Booting FIRM");
			launch_firm();
		}

		if (key & KEY_B)
		{
				break;
		}
	}

	poweroff();
}
