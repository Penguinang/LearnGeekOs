/*
 * ELF executable loading
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.29 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/errno.h>
#include <geekos/kassert.h>
#include <geekos/ktypes.h>
#include <geekos/screen.h>  /* for debug Print() statements */
#include <geekos/pfat.h>
#include <geekos/malloc.h>
#include <geekos/string.h>
#include <geekos/user.h>
#include <geekos/elf.h>

#include <geekos/keyboard.h>


/**
 * From the data of an ELF executable, determine how its segments
 * need to be loaded into memory.
 * @param exeFileData buffer containing the executable file
 * @param exeFileLength length of the executable file in bytes
 * @param exeFormat structure describing the executable's segments
 *   and entry address; to be filled in
 * @return 0 if successful, < 0 on error
 */
int Parse_ELF_Executable(char *exeFileData, ulong_t exeFileLength,
    struct Exe_Format *exeFormat)
{
    // Print("This is elf header content\n");
    // for(int i = 0; i<0x34+0x20*3+1; i++){
    //     if(i%16 == 0){
    //         Print("\n");
    //         Print("%04x   ", i);
    //     }

    //     Print(" ");
    //     Print("%02hhx",exeFileData[i]);
    //     if(i%16 == 7)
    //         Print("\t");
    // }
    // Print("\n");

    /**
     * Elf header
     */
    elfHeader elf_header;
    memcpy(&elf_header, exeFileData, sizeof(elfHeader));
    
    /**
     * Num of segments
     */
    // elf_header.phnum = 1;
    exeFormat->numSegments = elf_header.phnum;

    /**
     * Entry addr
     */
    exeFormat->entryAddr = elf_header.entry;

    /**
     * Read segment  
     */
    ulong_t header_start = elf_header.phoff;
    ulong_t ph_size = elf_header.phentsize;
    for(int i = 0; i<elf_header.phnum; i++){
        programHeader p_header;
        memcpy(&p_header, exeFileData+header_start, ph_size);
        struct Exe_Segment segment = {
            p_header.offset, p_header.fileSize, p_header.vaddr, p_header.memSize, p_header.flags | PF_X
        };
        Print("segment %d type is %08x\n", i, p_header.type);
        Print("Read segment data ,offset %d, fileSize %d, vaddr %d, memSize %d, flags %d\n", 
            p_header.offset, p_header.fileSize, p_header.vaddr, p_header.memSize, p_header.flags);
        exeFormat->segmentList[i] = segment;
        header_start += ph_size;
    }
    // Wait_For_Key();
    // TODO("Wait\n");
    return 0;
}
