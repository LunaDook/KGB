#include "common.h"

// From CakesFW
void ncch_getctr(char *ncch, char *ctr)
{
	//uint32_t version = ncch->version;
    //const uint8_t* partitionID = ncch->partitionID;
    char *partitionID = (char*)(ncch + 0x108);

    memset(ctr, 0, 16);

    for(u32 i = 0; i < 8; i++)
        ctr[i] = partitionID[7 - i]; // Convert to big endian & normal input
	ctr[8] = 2;

    return;
}

// WARNING: NOT WORKING!
int decrypt_firm_ncch(char *buffer)
{
    /*if (*(u32*)(buffer + 0x100) != 0x4843434E) // Check 'NCCH' magic
        return;*/

    char exefs_key[16] = {0};
    char exefs_iv[16]  = {0};

    memcpy(exefs_key, buffer, 16);

    // 0x104	4	Content size, in media units (1 media unit = 0x200 bytes)
    u32 exefs_len = *(u32*)(buffer + 0x104) * 0x200;

    ncch_getctr(buffer, exefs_iv);
    use_aeskey(0x2C);
    setup_aeskeyY(0x2C, exefs_key);
    set_ctr(exefs_iv);

    u32 blocks = exefs_len/AES_BLOCK_SIZE;

    while(blocks != 0)
    {
        aes_decrypt(buffer + 0x200 + blocks*AES_BLOCK_SIZE, buffer + 0x200 + blocks*AES_BLOCK_SIZE, 1, AES_CNT_TITLEKEY_DECRYPT_MODE);
        add_ctr(exefs_iv, 0x1);
    }

    return 1;
}
