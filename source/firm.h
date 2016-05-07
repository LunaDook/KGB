// KGB by Wolfvak
// FIRM-relatead structures and functions

#pragma once

#include "common.h"

#define FIRM_MAGIC	0x4D524946 // 'FIRM'

//FIRM Header layout ( http://3dbrew.org/wiki/FIRM )
/* FIRM section structure
OFFSET	SIZE			DESCRIPTION
0x000	4				Magic 'FIRM'
0x004	4				Reserved1
0x008	4				ARM11 Entrypoint
0x00C	4				ARM9 Entrypoint
0x010	0x030			Reserved2
0x040	0x0C0(0x030*4)	Firmware Section Headers
0x100	0x100			RSA-2048 signature of the FIRM header's SHA-256 hash. The signature is checked when bootrom/Process9 are doing FIRM-launch (with the public key being hardcoded in each). The signature is not checked when installing FIRM to the NAND firm0/firm1 partitions.

FIRM header structure
OFFSET	SIZE	DESCRIPTION
0x000	4		Byte offset
0x004	4		Physical address where the section is loaded to.
0x008	4		Byte-size. While loading FIRM this is the field used to determine whether the section exists or not, by checking for value 0x0.
0x00C	4		Firmware Type ('0'=ARM9/'1'=ARM11) Process9 doesn't use this field at all.
0x010	0x020	SHA-256 Hash of Firmware Section
*/

typedef struct firm_section
{
    u32 byte_offset;
    u32 load_address;
    size_t size;
    u32 processor_type;
    u8 sha256_hash[0x20];
} firm_section;

typedef struct firm_header
{
    u32 magic;
    u32 reserved1;
    u32 arm11_entry;
    u32 arm9_entry;
    u8 reserved2[0x30];
    firm_section section[4];
	//u8 rsa2048_signature[0x100]; // Unused in KGB
} firm_header;

extern firm_header *firm;

u8 load_firm(const char *firm_filename); // Loads firm_filename to FCRAM
void launch_firm(); // Launches firm
