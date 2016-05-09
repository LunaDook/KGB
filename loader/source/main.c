#include <string.h>

#include "kgb.h"
#include "arm11.h"
#include "i2c.h"

#define A11_PAYLOAD_LOC 0x1FFF4C80
#define A11_ENTRY       0x1FFFFFF8

#define DEST_ADDR 0x23F00000

static void ownArm11()
{
    memcpy((void*)A11_PAYLOAD_LOC, arm11_bin, arm11_bin_len);
    *(vu32 *)A11_ENTRY = 1;
    *(vu32 *)0x1FFAED80 = 0xE51FF004;
    *(vu32 *)0x1FFAED84 = A11_PAYLOAD_LOC;
    *(vu8 *)0x1FFFFFF0 = 2;
    while(*(vu32 *)A11_ENTRY);
}

void setFramebuffers()
{
    //Gateway
    *(vu32*)0x80FFFC0 = 0x18300000;  // framebuffer 1 top left
    *(vu32*)0x80FFFC4 = 0x18300000;  // framebuffer 2 top left
    *(vu32*)0x80FFFC8 = 0x18300000;  // framebuffer 1 top right
    *(vu32*)0x80FFFCC = 0x18300000;  // framebuffer 2 top right
    *(vu32*)0x80FFFD0 = 0x18346500;  // framebuffer 1 bottom
    *(vu32*)0x80FFFD4 = 0x18346500;  // framebuffer 2 bottom
    *(vu32*)0x80FFFD8 = 1;  // framebuffer select top
    *(vu32*)0x80FFFDC = 1;  // framebuffer select bottom

    //CakeBrah
    *(vu32*)0x23FFFE00 = 0x18300000;
    *(vu32*)0x23FFFE04 = 0x18300000;
    *(vu32*)0x23FFFE08 = 0x18346500;
}

void clearScreens()
{
    for(u32 i = 0; i < (400 * 240 * 3 / 4); i++)
    {
        *((vu32*)0x18300000 + i) = 0;
        *((vu32*)0x18346500 + i) = 0;
    }
}

void main()
{
	setFramebuffers();
	ownArm11();
	clearScreens();
	i2cWriteRegister(3, 0x22, 0x2A); // 0x2A -> boot into firm with no backlight
	
	memcpy((void*)DEST_ADDR, (void*)kgb, kgb_len);
	((void (*)())DEST_ADDR)();
}
