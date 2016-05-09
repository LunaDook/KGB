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
	print("ERROR: ");
	print(err_msg);
	print("\nPress the ANY key to power off");
	input_wait();
	poweroff();
}

void main()
{
	console_init();
	print("KGB: rescue mode\n\n");

	if (f_mount(&sd, "0:", 1) != FR_OK)
	{
		error("couldn't mount the SD card");
	}

	print("Attempting to load FIRM from ");
	print(firm_fname);
	print(" \n\n");

	int ret = load_firm(firm_fname);

	if (!ret)
	{
		error("couldn't load firmware");
	}

	print("\nPress [A] to boot FIRM\nPress [B] to power off\n");

	u16 key = 0;

	for(;;)
	{
		key = input_wait();

		if (key & KEY_A)
		{
			print("Booting FIRM");
			launch_firm();
		}

		if (key & KEY_B)
		{
				break;
		}
	}

	poweroff();
}
