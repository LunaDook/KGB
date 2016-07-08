#include "common.h"
#include "chainloader.h"

FATFS nand_fs, sd_fs;

ctr_nand_interface nand_io;
ctr_nand_crypto_interface ctr_io;
ctr_sd_interface sd_io;

void cleanup()
{
    ctr_nand_interface_destroy(&nand_io);
    ctr_nand_crypto_interface_destroy(&ctr_io);
    ctr_sd_interface_destroy(&sd_io);
    return;
}

void error(char *err_msg)
{
    printf("ERROR: %s\nPress any key to power off", err_msg);
    cleanup();
	wait_for_key();
	ctr_system_poweroff();
}

int main()
{
    // Initialize console output
	console_init();
	printf(BUILD_NAME "\n");

    // Initialize IO interfaces
    ctr_fatfs_initialize(&nand_io, &ctr_io, NULL, &sd_io);
    ctr_fatfs_default_setup(&ctr_io, NULL, &sd_io);

    if (N3DS)
    {
        printf("Setting N3DS CTRNAND key\n");
        if (set_nctrnand_key() != 0)
            error("N3DS key 0x05 could not be set");
    }

    // Attempt to mount FAT filesystems
    const char ctrnand_drive[] = "CTRNAND:", sd_drive[] = "SD:";
    FRESULT f_ret;
    f_ret = f_mount(&nand_fs, ctrnand_drive, 0);
    if (f_ret != FR_OK)
    {
        printf("CTRNAND f_mount returned %s", ff_err[f_ret]);
        printf("Failed to mount CTRNAND... press a key to reboot\n");
        wait_for_key();
        ctr_system_reset();
    }
    
    // Attempt to load FIRM from CTRNAND
    s32 ret = load_firm();
    if (ret != 0)
    {
        printf("load_firm returned %d\n", ret);
		error("couldn't load firmware");
    }

	printf("Press:\n[A] to boot\n[B] to power off\n[Y] load arm9loaderhax.bin from CTRNAND\n%s", SD_INSERTED ? "[X] load arm9loaderhax.bin from SD\n" : "");

	u32 key;

	while(1)
	{
		key = wait_for_key();

		if (key & KEY_A)
		{       
            // Unmount CTRNAND
            f_ret = f_mount(NULL, ctrnand_drive, 0);
            if (f_ret != 0)
                printf("f_mount returned %s", ff_err[f_ret]);

            cleanup();
            printf("Booting FIRM\n");
			launch_firm();
		}

		else if (key & KEY_B)
			break;

        else if (key & KEY_Y)
            chainload(ctrnand_drive);

        else if ((key & KEY_X) && SD_INSERTED)
        {
            f_ret = f_mount(&sd_fs, sd_drive, 0);
            if (f_ret != FR_OK)
            {
                printf("f_mount returned %s", ff_err[f_ret]);
                error("Couldn't mount SD card");
            }

            chainload(sd_drive);
        }
	}

    ctr_flush_cache();
    ctr_system_poweroff();
}

void chainload(const char *drive)
{
    u8 *loader_addr = (u8*)0x25F00000, *payload_addr = (u8*)0x24F00000;

    memcpy(loader_addr, chainloader_bin, chainloader_bin_len);

    FIL payload;
    size_t br;

    char path[0x20] = {0};

    u32 drive_len = strlen(drive);
    memcpy(path, drive, drive_len);
    memcpy(path + drive_len, PAYLOADPATH, strlen(PAYLOADPATH));

    printf("%s\n", path);

    if (f_open(&payload, path, FA_READ) != FR_OK)
        error("Couldn't open payload");
    if (f_read(&payload, payload_addr, f_size(&payload), &br) != FR_OK)
        error("Couldn't read payload");
    f_close(&payload);

    f_mount(NULL, drive, 0);
    cleanup();

    ctr_flush_cache();
    ((void(*)())loader_addr)();
}
