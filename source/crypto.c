// From http://github.com/b1l1s/ctr
// Only removed SHA/RSA related functions

#include "common.h"

/* original version by megazig */

#ifndef __thumb__
#define BSWAP32(x) {\
	__asm__\
	(\
		"eor r1, %1, %1, ror #16\n\t"\
		"bic r1, r1, #0xFF0000\n\t"\
		"mov %0, %1, ror #8\n\t"\
		"eor %0, %0, r1, lsr #8\n\t"\
		:"=r"(x)\
		:"0"(x)\
		:"r1"\
	);\
};

#define ADD_u128_u32(u128_0, u128_1, u128_2, u128_3, u32_0) {\
__asm__\
	(\
		"adds %0, %4\n\t"\
		"addcss %1, %1, #1\n\t"\
		"addcss %2, %2, #1\n\t"\
		"addcs %3, %3, #1\n\t"\
		: "+r"(u128_0), "+r"(u128_1), "+r"(u128_2), "+r"(u128_3)\
		: "r"(u32_0)\
		: "cc"\
	);\
}
#else
#define BSWAP32(x) {x = __builtin_bswap32(x);}

#define ADD_u128_u32(u128_0, u128_1, u128_2, u128_3, u32_0) {\
__asm__\
	(\
		"mov r4, #0\n\t"\
		"add %0, %0, %4\n\t"\
		"adc %1, %1, r4\n\t"\
		"adc %2, %2, r4\n\t"\
		"adc %3, %3, r4\n\t"\
		: "+r"(u128_0), "+r"(u128_1), "+r"(u128_2), "+r"(u128_3)\
		: "r"(u32_0)\
		: "cc", "r4"\
	);\
}
#endif /*__thumb__*/

void aes_setkey(u8 keyslot, const void* key, u32 keyType, u32 mode)
{
	if(keyslot <= 0x03) return; // Ignore TWL keys for now

	u32* key32 = (u32*)key;
	*REG_AESCNT = (*REG_AESCNT & ~(AES_CNT_INPUT_ENDIAN | AES_CNT_INPUT_ORDER)) | mode;
	*REG_AESKEYCNT = (*REG_AESKEYCNT >> 6 << 6) | keyslot | AES_KEYCNT_WRITE;

	REG_AESKEYFIFO[keyType] = key32[0];
	REG_AESKEYFIFO[keyType] = key32[1];
	REG_AESKEYFIFO[keyType] = key32[2];
	REG_AESKEYFIFO[keyType] = key32[3];
}

void aes_use_keyslot(u8 keyslot)
{
	if(keyslot > 0x3F)
		return;

	*REG_AESKEYSEL = keyslot;
	*REG_AESCNT = *REG_AESCNT | 0x04000000; /* mystery bit */
}

void aes_setiv(const void* iv, u32 mode)
{
	const u32* iv32 = (const u32*)iv;
	*REG_AESCNT = (*REG_AESCNT & ~(AES_CNT_INPUT_ENDIAN | AES_CNT_INPUT_ORDER)) | mode;

	// Word order for IV can't be changed in REG_AESCNT and always default to reversed
	if(mode & AES_INPUT_NORMAL)
	{
		REG_AESCTR[0] = iv32[3];
		REG_AESCTR[1] = iv32[2];
		REG_AESCTR[2] = iv32[1];
		REG_AESCTR[3] = iv32[0];
	}
	else
	{
		REG_AESCTR[0] = iv32[0];
		REG_AESCTR[1] = iv32[1];
		REG_AESCTR[2] = iv32[2];
		REG_AESCTR[3] = iv32[3];
	}
}

void aes_advctr(void* ctr, u32 val, u32 mode)
{
	u32* ctr32 = (u32*)ctr;
	
	int i;
	if(mode & AES_INPUT_BE)
	{
		for(i = 0; i < 4; ++i) // Endian swap
			BSWAP32(ctr32[i]);
	}
	
	if(mode & AES_INPUT_NORMAL)
	{
		ADD_u128_u32(ctr32[3], ctr32[2], ctr32[1], ctr32[0], val);
	}
	else
	{
		ADD_u128_u32(ctr32[0], ctr32[1], ctr32[2], ctr32[3], val);
	}
	
	if(mode & AES_INPUT_BE)
	{
		for(i = 0; i < 4; ++i) // Endian swap
			BSWAP32(ctr32[i]);
	}
}

void aes_change_ctrmode(void* ctr, u32 fromMode, u32 toMode)
{
	u32* ctr32 = (u32*)ctr;
	int i;
	if((fromMode ^ toMode) & AES_CNT_INPUT_ENDIAN)
	{
		for(i = 0; i < 4; ++i)
			BSWAP32(ctr32[i]);
	}

	if((fromMode ^ toMode) & AES_CNT_INPUT_ORDER)
	{
		u32 temp = ctr32[0];
		ctr32[0] = ctr32[3];
		ctr32[3] = temp;

		temp = ctr32[1];
		ctr32[1] = ctr32[2];
		ctr32[2] = temp;
	}
}

void aes_batch(void* dst, const void* src, u32 blockCount)
{
	*REG_AESBLKCNT = blockCount << 16;
	*REG_AESCNT |=	AES_CNT_START;
	
	const u32* src32	= (const u32*)src;
	u32* dst32			= (u32*)dst;
	
	u32 wbc = blockCount;
	u32 rbc = blockCount;
	
	while(rbc)
	{
		if(wbc && ((*REG_AESCNT & 0x1F) <= 0xC)) // There's space for at least 4 ints
		{
			*REG_AESWRFIFO = *src32++;
			*REG_AESWRFIFO = *src32++;
			*REG_AESWRFIFO = *src32++;
			*REG_AESWRFIFO = *src32++;
			wbc--;
		}
		
		if(rbc && ((*REG_AESCNT & (0x1F << 0x5)) >= (0x4 << 0x5))) // At least 4 ints available for read
		{
			*dst32++ = *REG_AESRDFIFO;
			*dst32++ = *REG_AESRDFIFO;
			*dst32++ = *REG_AESRDFIFO;
			*dst32++ = *REG_AESRDFIFO;
			rbc--;
		}
	}
}

inline void aes_setmode(u32 mode)
{
	*REG_AESCNT =	mode |
					AES_CNT_INPUT_ORDER | AES_CNT_OUTPUT_ORDER |
					AES_CNT_INPUT_ENDIAN | AES_CNT_OUTPUT_ENDIAN |
					AES_CNT_FLUSH_READ | AES_CNT_FLUSH_WRITE;
}

void aes(void* dst, const void* src, u32 blockCount, void* iv, u32 mode, u32 ivMode)
{
	aes_setmode(mode);

	u32 blocks;
	while(blockCount != 0)
	{
		if((mode & AES_ALL_MODES) != AES_ECB_ENCRYPT_MODE
		&& (mode & AES_ALL_MODES) != AES_ECB_DECRYPT_MODE)
			aes_setiv(iv, ivMode);

		blocks = (blockCount >= 0xFFFF) ? 0xFFFF : blockCount;

		// Save the last block for the next decryption CBC batch's iv
		if((mode & AES_ALL_MODES) == AES_CBC_DECRYPT_MODE)
		{
			memcpy(iv, src + (blocks - 1) * AES_BLOCK_SIZE, AES_BLOCK_SIZE);
			aes_change_ctrmode(iv, AES_INPUT_BE | AES_INPUT_NORMAL, ivMode);
		}

		// Process the current batch
		aes_batch(dst, src, blocks);

		// Save the last block for the next encryption CBC batch's iv
		if((mode & AES_ALL_MODES) == AES_CBC_ENCRYPT_MODE)
		{
			memcpy(iv, dst + (blocks - 1) * AES_BLOCK_SIZE, AES_BLOCK_SIZE);
			aes_change_ctrmode(iv, AES_INPUT_BE | AES_INPUT_NORMAL, ivMode);
		}
		
		// Advance counter for CTR mode
		else if((mode & AES_ALL_MODES) == AES_CTR_MODE)
			aes_advctr(iv, blocks, ivMode);

		src += blocks * AES_BLOCK_SIZE;
		dst += blocks * AES_BLOCK_SIZE;
		blockCount -= blocks;
	}
}

// From Decrypt9WIP
void sha_quick(void* res, const void* src, u32 size, u32 mode) {
    sha_init(mode);
    sha_update(src, size);
    sha_get(res);
}

// From CakesFW
void decrypt_firm_cxi(u8 *firm_buffer)
{
    u8 exefs_key[16],
       exefs_iv[16];

    memcpy(exefs_key, firm_buffer, 16);

    /** GET NCCH IV
    0x108   8     Partition ID*/

	const u8 *partitionID = (u8*)(firm_buffer + 0x108);

    memset(exefs_iv, 0, 16);

	for(int i = 0; i < 8; i++)
		exefs_iv[i] = partitionID[7 - i]; // Convert to big endian & normal input

    exefs_iv[8] = 2; // FIRM NCCHs use version 2 (at least 9.0-11.0 do)

    /**
    OFFSET	SIZE	DESCRIPTION
    0x1A0   4       ExeFS offset, in media units
    0x1A4   4       ExeFS size, in media units
    1 mediaunit = 0x200 bytes */

    u8 *exefs_loc = (u8*)firm_buffer + (*(u32*)(firm_buffer + 0x1A0) * 0x200);
    u32 exefs_len = (*(u32*)(firm_buffer + 0x1A4) * 0x200);

    // Set keyY for slot 0x2C
    aes_setkey(0x2C, exefs_key, AES_KEYY, AES_INPUT_BE | AES_INPUT_NORMAL);
    aes_setiv(exefs_iv, AES_INPUT_BE | AES_INPUT_NORMAL);
    aes_use_keyslot(0x2C);

    // Decrypt FIRM ExeFS
    aes(firm_buffer - 0x200, exefs_loc, exefs_len / AES_BLOCK_SIZE, exefs_iv, AES_CTR_MODE, AES_INPUT_BE | AES_INPUT_NORMAL);

    return;
}

u32 decrypt_arm9bin(const u8 *header, const u8 version)
{
    u8 slot = ((version >= 0xF) ? 0x16 : 0x15),
       key_y[16], iv[16];

    u32 arm9loader_version = 0, size = 0;

    if (version == 0x0F)
        arm9loader_version = 1;

    else if (version > 0x0F)
        arm9loader_version = 2;

    memcpy(key_y, header + 0x10, 16);
    memcpy(iv, header + 0x20, 16);

    u8 *arm9_bin = (u8*)(header + 0x800);

    // Apparently, engineers decided to store it as ASCII text (yes, literally text)
    u8 *size_loc = (u8*)(header + 0x30);
    for (u32 i = 0; size_loc[i] != 0; i++)
        size = (size * 10) + (size_loc[i] - '0');

    printf("FIRM 0x%02X, arm9loader v%d, ARM9bin size is %d bytes\n", version, arm9loader_version, size);

    if (arm9loader_version)
    {
        u8 slot0x11key96[16] = {0x42, 0x3F, 0x81, 0x7A, 0x23, 0x52, 0x58, 0x31, 0x6E, 0x75, 0x8E, 0x3A, 0x39, 0x43, 0x2E, 0xD0},
           slot0x16keyX[16];
        // And there it goes...

        aes_setkey(0x11, slot0x11key96, AES_KEYNORMAL, AES_INPUT_BE | AES_INPUT_NORMAL);
        aes_use_keyslot(0x11);

        aes(slot0x16keyX, header + 0x60, 1, NULL, AES_ECB_DECRYPT_MODE, 0);
        aes_setkey(slot, slot0x16keyX, AES_KEYX, AES_INPUT_BE | AES_INPUT_NORMAL);
    }

    aes_setiv(iv, AES_INPUT_BE | AES_INPUT_NORMAL);
    aes_setkey(slot, key_y, AES_KEYY, AES_INPUT_BE | AES_INPUT_NORMAL);

    aes_use_keyslot(slot);
    aes(arm9_bin, arm9_bin, size / AES_BLOCK_SIZE, iv, AES_CTR_MODE, AES_INPUT_BE | AES_INPUT_NORMAL);

    if(arm9loader_version >= 2)
    {
        // TODO: Extract the key from the ARM9 binary itself rather than hardcoding it
        // I think I'll go with a partial key + memsearch, it'll probably be a bit slower, but safer
        u8 keydata[16] = {0xDD, 0xDA, 0xA4, 0xC6, 0x2C, 0xC4, 0x50, 0xE9, 0xDA, 0xB6, 0x9B, 0x0D, 0x9D, 0x2A, 0x21, 0x98},
           keyx[16];

        aes_use_keyslot(0x11);

        for (u32 slot = 0x19; slot < 0x20; slot++)
        {
            aes(keyx, keydata, 1, NULL, AES_ECB_DECRYPT_MODE, 0);
            aes_setkey(slot, keyx, AES_KEYX, AES_INPUT_BE | AES_INPUT_NORMAL);
            *(u8*)(keydata + 0xF) += 1;
        }
    }

    if (*(u32*)arm9_bin == ARM9_MAGIC)
        return 0;

    else return -1;
}

s32 set_nctrnand_key()
{
    const u8 slot0x05keyY[] = {0x4D, 0x80, 0x4F, 0x4E, 0x99, 0x90, 0x19, 0x46, 0x13, 0xA2, 0x04, 0xAC, 0x58, 0x44, 0x60, 0xBE};
    aes_setkey(0x05, slot0x05keyY, AES_KEYY, AES_INPUT_BE | AES_INPUT_NORMAL);
    return 0;
}

/*void dump_firm0(u8 *firm_buffer)
{
    u32 firm0_offset = 0x0B130000, firm0_size = 0x00100000;
    u8 ctr[16], cid[16], sha[32];

    printf("Reading NAND\n");

    sdmmc_nand_readsectors(firm0_offset / SECTOR_SIZE, firm0_size / SECTOR_SIZE, (u8*)firm_buffer);

    printf("Obtaining IV\n");

    sdmmc_get_cid(1, (u32*)cid);
    sha_quick(sha, cid, 16, SHA256_MODE);
    memcpy(ctr, sha, 16);
    aes_advctr(ctr, firm0_offset / AES_BLOCK_SIZE, AES_INPUT_BE | AES_INPUT_NORMAL);

    printf("Decrypting FIRM0\n");

    aes_use_keyslot(0x06);
    aes_setiv(ctr, AES_INPUT_BE | AES_INPUT_NORMAL);
    aes((u8*)firm_buffer, (u8*)firm_buffer, firm0_size / AES_BLOCK_SIZE, ctr, AES_CTR_MODE, AES_INPUT_BE | AES_INPUT_NORMAL);
}*/