#include <3ds.h>
#include <string.h>
#include "patcher.h"

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

//Quick Search algorithm, adapted from http://igm.univ-mlv.fr/~lecroq/string/node19.html#SECTION00190
static u8 *memsearch(u8 *startPos, const void *pattern, u32 size, u32 patternSize)
{
    const u8 *patternc = (const u8 *)pattern;

    //Preprocessing
    u32 table[256];

    for(u32 i = 0; i < 256; ++i)
        table[i] = patternSize + 1;
    for(u32 i = 0; i < patternSize; ++i)
        table[patternc[i]] = patternSize - i;

    //Searching
    u32 j = 0;

    while(j <= size - patternSize)
    {
        if(memcmp(patternc, startPos + j, patternSize) == 0)
            return startPos + j;
        j += table[startPos[j + patternSize]];
    }

    return NULL;
}

static u32 patch_memory(u8 *start, u32 size, const void *pattern, u32 patSize, int offset, const void *replace, u32 repSize, u32 count)
{
    u32 i;

    for(i = 0; i < count; i++)
    {
        u8 *found = memsearch(start, pattern, size, patSize);

        if(found == NULL) break;

        memcpy(found + offset, replace, repSize);

        u32 at = (u32)(found - start);

        if (at + patSize > size) break;

        size -= at + patSize;
        start = found + patSize;
    }

    return i;
}

void patch_code(u64 progid, u8 *code, u32 size)
{
    switch(progid)
    {
        case 0x0004003000008F02LL: // USA Menu
        case 0x0004003000008202LL: // EUR Menu
        case 0x0004003000009802LL: // JPN Menu
        case 0x000400300000A102LL: // CHN Menu
        case 0x000400300000A902LL: // KOR Menu
        case 0x000400300000B102LL: // TWN Menu
        {
            static const u8 region_free_pattern[] = {0x00, 0x00, 0x55, 0xE3, 0x01, 0x10, 0xA0, 0xE3},
                            region_free_patch[] = {0x01, 0x00, 0xA0, 0xE3, 0x1E, 0xFF, 0x2F, 0xE1};

            //Patch SMDH region checks
            patch_memory(code, size, 
                region_free_pattern, 
                sizeof(region_free_pattern), -16, 
                region_free_patch, 
                sizeof(region_free_patch), 1);

            break;
        }

        case 0x0004013000008002LL: // NS
        {
            static const u8 stop_cart_updates_pattern[] = {0x0C, 0x18, 0xE1, 0xD8},
                            stop_cart_updates_patch[] = {0x0B, 0x18, 0x21, 0xC8};

            //Disable updates from foreign carts (makes carts region-free)
            patch_memory(code, size, 
                stop_cart_updates_pattern, 
                sizeof(stop_cart_updates_pattern), 0, 
                stop_cart_updates_patch,
                sizeof(stop_cart_updates_patch), 2);

            break;
        }
        
        case 0x0004013000002C02LL: // NIM
        {
            static const u8 block_updates_pattern[] = {0x25, 0x79, 0x0B, 0x99},
                            block_updates_patch[] = {0xE3, 0xA0},
                            block_eshop_updates_pattern[] = {0x30, 0xB5, 0xF1, 0xB0},
                            block_eshop_updates_patch[] = {0x00, 0x20, 0x08, 0x60, 0x70, 0x47};

            patch_memory(code, size, 
                block_updates_pattern, 
                sizeof(block_updates_pattern), 0, 
                block_updates_patch, 
                sizeof(block_updates_patch), 1);

            patch_memory(code, size, 
                block_eshop_updates_pattern, 
                sizeof(block_eshop_updates_pattern), 0, 
                block_eshop_updates_patch, 
                sizeof(block_eshop_updates_patch), 1);

            break;
        }
        case 0x0004013000001702LL: // CFG
        {
            static const u8 secureinfo_sig_check_pattern[] = {0x06, 0x46, 0x10, 0x48, 0xFC},
                            secureinfo_sig_check_patch[] = {0x00, 0x26};
            
            patch_memory(code, size, 
                secureinfo_sig_check_pattern, 
                sizeof(secureinfo_sig_check_pattern), 0, 
                secureinfo_sig_check_patch, 
                sizeof(secureinfo_sig_check_patch), 1);

            break;
        }
        
    }

}
