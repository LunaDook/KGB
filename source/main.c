#include "common.h"

//framebuffer_t *framebuffer;
static FATFS nand_fs;//, sd; // Disable SD, I don't need it currently

void error(const char *err_msg)
{
	print("ERROR: ");
	print(err_msg);
	print("\nPress any key to power off");
	input_wait();
	ctr_system_poweroff();
}

void main()
{
    // Initialize console output
	console_init();
	print("KGB\n");

    // Initialize IO interfaces
    // TODO: Initialize N3DS CTRNAND keyslots
    ctr_nand_interface nand_io;
    ctr_nand_crypto_interface ctr_io;
    ctr_sd_interface sd_io;

    ctr_fatfs_initialize(&nand_io, &ctr_io, NULL, &sd_io);
    ctr_fatfs_default_setup(&ctr_io, NULL, &sd_io);

    // Attempt to mount FAT filesystems
    FRESULT f_ret;
    f_ret = f_mount(&nand_fs, "0:", 0);
    print("CTRNAND f_mount returned ");
    print_fresult(f_ret);

    if (f_ret != FR_OK)
    {
        print("Failed to mount CTRNAND... press a key to reboot\n");
        input_wait();
        ctr_system_reset();
    }

    // Attempt to load FIRM from CTRNAND
    s32 ret = load_firm();
    print("load_firm returned ");
    print_hex(ret);
    print("\n");

	if (ret != 0)
		error("couldn't load firmware");

    // Unmount CTRNAND
    f_mount(NULL, "0:", 0);

    // Destroy IO interfaces
    ctr_nand_crypto_interface_destroy(&ctr_io);
    ctr_nand_interface_destroy(&nand_io);
    ctr_sd_interface_destroy(&sd_io);

    // TODO: Add support for loading an ARM9 payload off of CTRNAND/SD (if inserted)
	print("\nPress [A] to boot\nPress [B] to power off\nPress [Y] to reboot\n\n");

	u32 key;

	while(1)
	{
		key = input_wait();

		if (key & KEY_A)
		{
			print("Booting FIRM\n");
			launch_firm();
		}

		else if (key & KEY_B)
			ctr_system_poweroff();

        else if (key & KEY_Y)
            ctr_system_reset();
	}
}
