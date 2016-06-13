#include "common.h"

firm_header *firm;

// This is HIGHLY unoptimized, but again, this is a learning project :)
char *memsearch(char* search_start, u32 search_len, char* search_pattern, u32 pattern_len)
{
    if (!search_start || !search_len || !search_pattern || !pattern_len || search_len < pattern_len)
        return NULL;

    for (u32 i = 0; i < search_len; i++)
    {
        if (memcmp(search_start + i, search_pattern, pattern_len) == 0)
            return (search_start + i);
    }

    return NULL;
}

int get_process9_addr(char *section, u32 len, char *proc9_addr, u32 *proc9_len)
{
    proc9_addr = memsearch(section, len, "Process9", 8);

    if (!proc9_addr)
        return -1;

    if (proc9_len)
        *proc9_len = *(proc9_addr - 0x5C) * 0x200;

    return 0;
}

int patch_process9(char *proc9_addr, u32 proc9_len)
{
    char *addr = NULL;
    char sigpat1[] = {0xC0, 0x1C, 0x76, 0xE7}, sigpatch1[] = {0x00, 0x20};

    // Signature patch 1
    addr = memsearch(proc9_addr, proc9_len, sigpat1, 4);

    if (addr)
        memcpy(addr, sigpatch1, 2);

    // Signature patch 2

    char sigpat2[] = {0xB5, 0x22, 0x4D, 0x0C}, sigpatch2[] = {0x00, 0x20, 0x70, 0x47};

    addr = memsearch(proc9_addr, proc9_len, sigpat2, 4);

    if (addr)
        memcpy(addr - 1, sigpatch2, 4);

    // FIRM partition protection patche

    char firmprot_pattern[] = {0x65, 0x78, 0x65, 0x3A}, firmprot_pattern_[] = {0x00, 0x28, 0x01, 0xDA},
         firmprot_patch[] = {0x00, 0x20, 0xC0, 0x46};

    addr = memsearch(proc9_addr, proc9_len, firmprot_pattern, 4);

    if (addr)
    {
        char *addr_ = memsearch(addr - 0x100, 0x100, firmprot_pattern_, 4);
        if (addr_)
            memcpy(addr_, firmprot_patch, 4);
    }

    return 0;
}

int patch_firmware()
{
    char *proc9_addr = NULL;
    u32 proc9_len = 0;

    if(!get_process9_addr((char*)firm->section[2].load_address, firm->section[2].size, proc9_addr, &proc9_len))
        patch_process9(proc9_addr, proc9_len);

    return 1;
}
