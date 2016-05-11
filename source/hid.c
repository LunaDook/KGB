// KGB by Wolfvak
// HID related functions

#include "common.h"

u32 input_wait() {
	u32 pad = HID_PAD;

	while(HID_PAD == pad)
		;

	return pad;
}
