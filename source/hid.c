// KGB by Wolfvak
// HID related functions

#include "common.h"

u16 input_wait() {
	u16 pad = HID_PAD;

	while(HID_PAD == pad)
		;

	return pad;
}
