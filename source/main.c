#include "common.h"

framebuffer_t *framebuffer;
static FATFS nand, sd;

// Drive "0:" is CTRNAND
char *firm_fname = "0:/firmware";

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
	console_init();
	print("KGB - Wolfvak\n\n");

    ctr_nand_interface nand_io;
    ctr_nand_crypto_interface ctr_io;
    ctr_sd_interface sd_io;

    ctr_fatfs_initialize(&nand_io, &ctr_io, NULL, &sd_io);
    ctr_fatfs_default_setup(&ctr_io, NULL, &sd_io);

    FRESULT f_ret;

    f_ret = f_mount(&nand, "0:", 0);
    //f_mount(&sd, "SD:", 0);
    print_fresult(f_ret);
    if (f_ret != FR_OK)
    {
        print("Failed to mount CTRNAND... press a key to reboot\n");
        input_wait();
        ctr_system_reset();
    }

    /*FIL firm_encrypted, firm_dec;
    unsigned int l;

    f_open(&firm_encrypted, "SD:/firm.app", FA_READ);
    u32 length = f_size(&firm_encrypted);
    char firm_buf[length];

    f_read(&firm_encrypted, &firm_buf, length, &l);
    f_close(&firm_encrypted);

    print("read successfully\n");

    decrypt_firm_ncch(firm_buf);

    f_open(&firm_dec, "SD:/firm_dec", FA_WRITE | FA_CREATE_ALWAYS);
    f_write(&firm_dec, &firm_buf, length, &l);
    f_close(&firm_dec);

    print("written\n");*/

    print("Loading FIRM from ");
	print(firm_fname);
	print("\n\n");

	int ret = load_firm(firm_fname);

	if (ret <= 0)
		error("couldn't load firmware");

    patch_firmware();

    // Unmount CTRNAND
    f_mount(NULL, "0:", 0);
    // Unmount SD
    //f_mount(NULL, "SD:", 0);

    // Destroy IO interfaces
    ctr_nand_crypto_interface_destroy(&ctr_io);
    ctr_nand_interface_destroy(&nand_io);
    ctr_sd_interface_destroy(&sd_io);

	print("\nPress [A] to boot FIRM\nPress [B] to power off\n\n");

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
			break;
	}

	ctr_system_poweroff();
}
