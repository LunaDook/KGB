#include <3ds.h>
#include <string.h>
#include "patcher.h"
#include "common.h"
#include "loader.h"
 
firm_header *firm;
 
// TODO: Optimize memsearch
u8 *memsearch(u8* search_start, u32 search_len, u8* search_pattern, u32 pattern_len)
{
    if (!search_start || !search_len || !search_pattern || !pattern_len || (search_len < pattern_len))
        return NULL;
 
    for (u32 i = 0; i < (search_len - pattern_len); i++)
    {
        if (memcmp(&search_start[i], search_pattern, pattern_len) == 0)
            return &search_start[i];
    }
 
    return NULL;
}
 
s32 patch_process9(u8 *proc9_addr, u32 proc9_len)
{
    s32 err = 0;
    u8 *addr = NULL;
    u8 sigpat1[] = {0xC0, 0x1C, 0x76, 0xE7}, sigpatch1[] = {0x00, 0x20};
 
    // Signature patch 1
    addr = memsearch(proc9_addr, proc9_len, sigpat1, 4);
 
    if (addr)
        memcpy(addr, sigpatch1, 2);
    else
        err -= 1;
 
    // Signature patch 2
    u8 sigpat2[] = {0xB5, 0x22, 0x4D, 0x0C}, sigpatch2[] = {0x00, 0x20, 0x70, 0x47};
 
    addr = memsearch(proc9_addr, proc9_len, sigpat2, 4);
 
    if (addr)
        memcpy(addr - 1, sigpatch2, 4);
    else
        err -= (1 << 1);
 
    // FIRM partition protection patch
    u8 firmprot_pattern[] = {0x65, 0x78, 0x65, 0x3A}, firmprot_pattern_[] = {0x00, 0x28, 0x01, 0xDA},
       firmprot_patch[] = {0x00, 0x20, 0xC0, 0x46};
 
    addr = memsearch(proc9_addr, proc9_len, firmprot_pattern, 4);
 
    if (addr)
    {
        u8 *addr_ = memsearch(addr - 0x100, 0x100, firmprot_pattern_, 4);
        if (addr_)
            memcpy(addr_, firmprot_patch, 4);
        else
            err -= (1 << 2);
    }
 
    else
        err -= (1 << 2);
 
    return err;
}
 
s32 patch_module(u8 *sect_addr, u32 sect_len, char *mod_name, u8 mod_name_len, u8 *mod_cxi, u32 mod_cxi_len)
{
    u32 mod_len = 0;
    u8 *mod_addr = memsearch(sect_addr, sect_len, (u8*)mod_name, mod_name_len);
 
    if (!mod_addr) // Not found
        return -1;
 
    // (mod_addr - 0x200) is the beggining of the NCCH header
    mod_addr -= 0x200;
 
    mod_len = *(u32*)(mod_addr + 0x104) * 0x200;
 
    // Move the rest of the section
    memcpy(mod_addr + mod_cxi_len, mod_addr + mod_len, sect_addr + sect_len - (mod_addr + mod_len));
 
    // Inject the replacement module
    memcpy(mod_addr, mod_cxi, mod_cxi_len);
 
    return 0;
}
 
void reimplementSvcBackdoor(u8 *pos, u32 size)
{
    //Official implementation of svcBackdoor
    const u8  svcBackdoor[40] = {0xFF, 0x10, 0xCD, 0xE3,  //bic   r1, sp, #0xff
                                 0x0F, 0x1C, 0x81, 0xE3,  //orr   r1, r1, #0xf00
                                 0x28, 0x10, 0x81, 0xE2,  //add   r1, r1, #0x28
                                 0x00, 0x20, 0x91, 0xE5,  //ldr   r2, [r1]
                                 0x00, 0x60, 0x22, 0xE9,  //stmdb r2!, {sp, lr}
                                 0x02, 0xD0, 0xA0, 0xE1,  //mov   sp, r2
                                 0x30, 0xFF, 0x2F, 0xE1,  //blx   r0
                                 0x03, 0x00, 0xBD, 0xE8,  //pop   {r0, r1}
                                 0x00, 0xD0, 0xA0, 0xE1,  //mov   sp, r0
                                 0x11, 0xFF, 0x2F, 0xE1}; //bx    r1
 
    u8 pattern[] = {0x00, 0xB0, 0x9C, 0xE5}; //cpsid aif
   
    u32 *exceptionsPage = (u32 *)memsearch(pos, size, pattern, 4) - 0xB;
 
    u32 svcOffset = (-((exceptionsPage[2] & 0xFFFFFF) << 2) & (0xFFFFFF << 2)) - 8; //Branch offset + 8 for prefetch
    u32 *svcTable = (u32 *)(pos + *(u32 *)(pos + 0xFFFF0008 - svcOffset - 0xFFF00000 + 8) - 0xFFF00000); //SVC handler address
    while(*svcTable) svcTable++; //Look for SVC0 (NULL)
 
    if(!svcTable[0x7B])
    {
        u32 *freeSpace;
        for(freeSpace = exceptionsPage; *freeSpace != 0xFFFFFFFF; freeSpace++);
 
        memcpy(freeSpace, svcBackdoor, 40);
 
        svcTable[0x7B] = 0xFFFF0000 + ((u8 *)freeSpace - (u8 *)exceptionsPage);
    }
}
 
s32 patch_firmware()
{
    reimplementSvcBackdoor((u8*)firm->section[1].load_address, firm->section[1].size);
   
    // Find the "Process9" string in ARM9 memory
    u8 *p9_addr = memsearch((u8*)firm->section[2].load_address, firm->section[2].size, (u8*)"Process9", 8) - 0x200;
 
    // ExeFS length (in bytes)
    u32 p9_len = *(u32*)(p9_addr + 0x104) * 0x200;
    // Start of the ExeFS
    p9_addr = (u8*)firm->section[2].load_address + *(u32*)(p9_addr + 0x1A0) * 0x200;
 
    s32 ret = patch_process9(p9_addr, p9_len);
 
    ret += patch_module((u8*)firm->section[0].load_address, firm->section[0].size, "loader", 6, loader_cxi, loader_cxi_len);
    return ret;
}
