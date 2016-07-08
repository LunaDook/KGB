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
void sha_quick(void* res, const void* src, u32 size, u32 mode)
{
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

s32 decrypt_arm9bin(const u8 *header, const u32 len, const u8 version)
{
    u8 slot = ((version >= 0xF) ? 0x16 : 0x15),
       key_y[0x10], iv[0x10];

    u32 a9l_ver = 0, size = 0;

    if (version == 0x0F)
        a9l_ver = 1;
    // This version is unsupported
    // Mainly due to being annoying overall

    else if (version > 0x0F)
        a9l_ver = 2;

    memcpy(key_y, header + 0x10, 0x10);
    memcpy(iv, header + 0x20, 0x10);

    u8 *arm9_bin = (u8*)(header + 0x800);

    // Nintendo decided to store it as ASCII text (yes, literally text -_-)
    u8 *size_loc = (u8*)(header + 0x30);
    for (u8 i = 0; size_loc[i] != 0; i++)
        size = (size * 10) + (size_loc[i] - '0');

    printf("FIRM %X, arm9loader v%d, arm9bin size is %X bytes\n", version, a9l_ver, size);

    if (a9l_ver)
    {
        u8 slot0x11key96[0x10] = {0x42, 0x3F, 0x81, 0x7A, 0x23, 0x52, 0x58, 0x31, 0x6E, 0x75, 0x8E, 0x3A, 0x39, 0x43, 0x2E, 0xD0},
           slot0x16keyX[0x10];
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

    if(a9l_ver == 2)
    {
        u8  key_firstbyte = 0xDD,
            key[0x10],
            keyx_tmp[0x10],
            key_tmphash[0x20],
            key_hash[0x20] = {0xB9, 0x4D, 0xB1, 0xB1, 0xC3, 0xE0, 0x11, 0x08, 0x9C, 0x19, 0x46, 0x06, 0x4A, 0xBC, 0x40, 0x2A,
                              0x7C, 0x66, 0xF4, 0x4A, 0x74, 0x6F, 0x71, 0x50, 0x32, 0xFD, 0xFF, 0x03, 0x74, 0xD7, 0x45, 0x2C};

        // "These are the New3DS keyslots, where the keyX is generated with keyslot 0x11 by the New3DS arm9 binary loader"
        // Only searches in the arm9loader binary
        for (u32 i = 0; i < (len - size); i += 2)
        // According to chaoskagami the key is 4 byte aligned
        // I use two, just to be safe
        {
            u8 *j = arm9_bin + size + i; // Skips the arm9 binary itself and goes straight to the arm9 loader
            if (*j == key_firstbyte) // First byte matches
            {
                sha_quick(key_tmphash, j, 0x10, SHA256_MODE);
                if (memcmp(key_tmphash, key_hash, 0x20) == 0)
                {
                    memcpy(key, j, 0x10);
                    break;
                }
            }
        }

        // Check again, just in case
        sha_quick(key_tmphash, key, 0x10, SHA256_MODE);
        if (memcmp(key_tmphash, key_hash, 0x20) != 0)
            // This can only happen if the key wasn't found at all
            return -2;

        aes_use_keyslot(0x11);
        for (u32 slot = 0x19; slot < 0x20; slot++)
        {
            aes(keyx_tmp, key, 1, NULL, AES_ECB_DECRYPT_MODE, 0);
            aes_setkey(slot, keyx_tmp, AES_KEYX, AES_INPUT_BE | AES_INPUT_NORMAL);
            key[0xF] += 1;
        }
    }

    if (*(u32*)arm9_bin == ARM9_MAGIC)
        return 0;

    else return -1;
}

s32 set_nctrnand_key()
{
    const u8 keyY_hash[] = {0x98, 0x24, 0x27, 0x14, 0x22, 0xB0, 0x6B, 0xF2, 0x10, 0x96, 0x9C, 0x36, 0x42, 0x53, 0x7C, 0x86,
                            0x62, 0x22, 0x5C, 0xFD, 0x6F, 0xAE, 0x9B, 0x0A, 0x85, 0xA5, 0xCE, 0x21, 0xAA, 0xB6, 0xC8, 0x4D};
    u8 key_firstbyte = 0x4D,
       keyhash_tmp[0x20],
       keyY[0x10];

    u8 *firm_buf = (u8*)FIRM_LOC;
    firm_header *header = (firm_header*)FIRM_LOC;

    s32 ret = dump_firm(firm_buf, 1);
    if (ret)
    {
        printf("dump_firm returned %d\n", ret);
        return -1;
    }

    ret = decrypt_arm9bin((u8*)header + header->section[2].byte_offset, header->section[2].size, 0x10);
    if (ret)
    {
        printf("decrypt_arm9bin returned %d\n", ret);
        return -2;
    }

    for (u32 i = 0; i < header->section[2].size; i += 2)
    {
        u8 *j = (u8*)header + header->section[2].byte_offset + i;
        if (*j == key_firstbyte) // First byte matches
        {
            sha_quick(keyhash_tmp, j, 0x10, SHA256_MODE);
            if (memcmp(keyhash_tmp, keyY_hash, 0x20) == 0)
            {
                memcpy(keyY, j, 0x10);
                break;
            }
        }
    }

    sha_quick(keyhash_tmp, keyY, 0x10, SHA256_MODE);
    if (memcmp(keyhash_tmp, keyY_hash, 0x20) != 0)
        // This can only happen if the key wasn't found at all
        return -3;

    aes_setkey(0x05, keyY, AES_KEYY, AES_INPUT_BE | AES_INPUT_NORMAL);
    return 0;
}

// 0x0B130000 = start of FIRM0 partition, 0x400000 = size of FIRM partition (4MB)
s32 dump_firm(u8 *firm_buffer, const u8 firm_id)
{
    u32 firm_offset = (0x0B130000 + ((firm_id % 2) * 0x400000)),
        firm_size = 0x00100000; // 1MB, because

    u8 ctr[0x10],
       cid[0x10],
       sha[0x20];

    if (sdmmc_nand_readsectors(firm_offset / SECTOR_SIZE, firm_size / SECTOR_SIZE, (u8*)firm_buffer))
        return -1;

    sdmmc_get_cid(1, (u32*)cid);
    sha_quick(sha, cid, 0x10, SHA256_MODE);
    memcpy(ctr, sha, 0x10);
    aes_advctr(ctr, firm_offset / AES_BLOCK_SIZE, AES_INPUT_BE | AES_INPUT_NORMAL);

    aes_use_keyslot(0x06);
    aes_setiv(ctr, AES_INPUT_BE | AES_INPUT_NORMAL);
    aes((u8*)firm_buffer, (u8*)firm_buffer, firm_size / AES_BLOCK_SIZE, ctr, AES_CTR_MODE, AES_INPUT_BE | AES_INPUT_NORMAL);
    return 0;
}
