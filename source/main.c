#include "common.h"

framebuffer_t *framebuffer;

FATFS sd;

char *firm_fname = "/firmware";

void chainload()
{
	FIL a9lh_payload;
	unsigned int br;
	
	if (f_open(&a9lh_payload, PAYLOAD_NAME, FA_READ) != FR_OK)
	{
		return;
	}

	if (f_read(&a9lh_payload, (void*)DEST_ADDR, 0x200000, &br) != FR_OK)
	{
		f_close(&a9lh_payload);
		return;
	}
	
	((void (*)())DEST_ADDR)();
}

void poweroff()
{
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1); // Power off
	while(1);
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
	//chainload(); //Not just yet...

	console_init();
	print("KGB: rescue mode\n\n");

	if (f_mount(&sd, "0:", 1) != FR_OK)
	{
		error("couldn't mount the SD card");
	}

	print("Attempting to load FIRM from ");
	print(firm_fname);
	print("\n\n");

	int ret = load_firm(firm_fname);

	if (!ret)
	{
		error("couldn't load firmware");
	}

	print("\nPress [A] to boot FIRM\nPress [B] to power off\n");

	u32 key = 0;

	while(1)
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
