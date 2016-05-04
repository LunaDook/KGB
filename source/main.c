#include "fatfs/ff.h"
#include "firm.h"
#include "draw.h"
#include "console.h"
#include "hid.h"
#include "i2c.h"

framebuffer_t *framebuffer;
firm_header *firm;

FATFS sd;

void commit_sudoku(u32 delay)
{
	console_init();
	print("\nGOODBYE CRUEL WORLD\n");
	while(delay--) __asm("andeq r0, r0, r0");
	i2cWriteRegister(I2C_DEV_MCU, 0x20, 1);
	while (1); // Loop until it shuts down
}

void error()
{
	print("Press the ANY button to power off");
	input_wait();
	commit_sudoku(1 << 18);
}

void main() {
	console_init();
	if (f_mount(&sd, "0:", 1) != FR_OK)
	{
		print_rgb("Couldn't mount SD card!", RED);
		error(); // Jumps to the infinite loop
	}

	print("Attepting to load FIRM from \"/firmware\"...\n\n");
	int ret = load_firm("/firmware");
	if (!ret)
	{
		print_rgb("\nCould not load FIRM!\n", RED);
		error();
	}

	print("\nPress [A] to boot FIRM\nPress [B] to power off\n");

	u32 key = 0;

	while(!(key & KEY_B))
	{
		key = input_wait();

		switch (key)
		{
			case KEY_A:
				print_rgb("Launching FIRM", GREEN);
				launch_firm();
				break; // Not necessary, but whatever

			case KEY_B:
				break;

			default:
				continue;
		}
	}

	commit_sudoku(1 << 18);
}
