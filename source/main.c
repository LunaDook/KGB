#include "fatfs/ff.h"
#include "firm.h"
#include "draw.h"

framebuffer_t* fb = (framebuffer_t *) 0x23FFFE00;

static FATFS sd;

/*void commit_sudoku(unsigned int delay) {
	while(delay--) __asm("andeq r0, r0, r0");
	i2cWriteRegister(DEVICE_MCU, MCU_POWER, 1);
}*/

void main() {
	clear_screen(fb->top_left, 0xFFFFFF); // White
	if (f_mount(&sd, "0:", 1) != FR_OK)
		return; // If screen stays white, then the SD card failed to mount

	clear_screen(fb->top_left, 0xFF0000); // Red
	if (!load_firm("/firmware", 0))
		return; // Red screen, to let the user know shit's fucked

	clear_screen(fb->top_left, 0x00FF00); // Green
	launch_firm(0); // Success! Hopefully?

	return;
}
