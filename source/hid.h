#pragma once

#define HID_PAD         ((*(volatile u16*)0x10146000) ^ 0xFFF) // XOR'd with 0xFFF because Nintendo loves us

#define KEY_A			(1 << 0)  // A
#define KEY_B			(1 << 1)  // B
#define KEY_SELECT		(1 << 2)  // SELECT
#define KEY_START		(1 << 3)  // START
#define KEY_DRRIGHT		(1 << 4)  // RIGHT
#define KEY_DLEFT		(1 << 5)  // LEFT
#define KEY_DUP			(1 << 6)  // UP
#define KEY_DDOWN		(1 << 7)  // DOWN
#define KEY_RT			(1 << 8)  // RIGHT TRIGGER
#define KEY_LT			(1 << 9)  // LEFT TRIGGER
#define KEY_X			(1 << 10) // X
#define KEY_Y			(1 << 11) // Y

u32 input_wait() {
	u32 key = HID_PAD;

	while(HID_PAD == key)
		;

	return HID_PAD;
}
