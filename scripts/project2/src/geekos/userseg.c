/*
 * Segmentation-based user mode implementation
 * Copyright (c) 2001,2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.23 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/ktypes.h>
#include <geekos/kassert.h>
#include <geekos/defs.h>
#include <geekos/mem.h>
#include <geekos/string.h>
#include <geekos/malloc.h>
#include <geekos/int.h>
#include <geekos/gdt.h>
#include <geekos/segment.h>
#include <geekos/tss.h>
#include <geekos/kthread.h>
#include <geekos/argblock.h>
#include <geekos/user.h>

/* ----------------------------------------------------------------------
 * Variables
 * ---------------------------------------------------------------------- */

#define DEFAULT_USER_STACK_SIZE 8192


/* ----------------------------------------------------------------------
 * Private functions
 * ---------------------------------------------------------------------- */


/*
 * Create a new user context of given size
 */

/* TODO: Implement
static struct User_Context* Create_User_Context(ulong_t size)
*/


static bool Validate_User_Memory(struct User_Context* userContext,
    ulong_t userAddr, ulong_t bufSize)
{
    ulong_t avail;

    if (userAddr >= userContext->size)
        return false;

    avail = userContext->size - userAddr;
    if (bufSize > avail)
        return false;

    return true;
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Destroy a User_Context object, including all memory
 * and other resources allocated within it.
 */
void Destroy_User_Context(struct User_Context* userContext)
{
    /*
     * Hints:
     * - you need to free the memory allocated for the user process
     * - don't forget to free the segment descriptor allocated
     *   for the process's LDT
     */
    KASSERT(userContext != 0);
    Free(userContext->memory);
    Free(userContext);
    // TODO("Destroy a User_Context");
}

/*
 * Load a user executable into memory by creating a User_Context
 * data structure.
 * Params:
 * exeFileData - a buffer containing the executable to load
 * exeFileLength - number of bytes in exeFileData
 * exeFormat - parsed ELF segment information describing how to
 *   load the executable's text and data segments, and the
 *   code entry point address
 * command - string containing the complete command to be executed:
 *   this should be used to create the argument block for the
 *   process
 * pUserContext - reference to the pointer where the User_Context
 *   should be stored
 *
 * Returns:
 *   0 if successful, or an error code (< 0) if unsuccessful
 */
int Load_User_Program(char *exeFileData, ulong_t exeFileLength,
    struct Exe_Format *exeFormat, const char *command,
    struct User_Context **pUserContext)
{
    /*
     * Hints:
     * - Determine where in memory each executable segment will be placed
     * - Determine size of argument block and where it memory it will
     *   be placed
     * - Copy each executable segment into memory
     * - Format argument block in memory
     * - In the created User_Context object, set code entry point
     *   address, argument block address, and initial kernel stack pointer
     *   address
     */
    *pUserContext = Malloc(sizeof(struct User_Context));
    KASSERT(*pUserContext != 0);

    /**
     * Allocate memory
     * I will put code and data segment first, then stack, and the last argument block
     */
    ulong_t maxva = 0;
    for(int i = 0; i<exeFormat->numSegments; i++){
        struct Exe_Segment *segment = &exeFormat->segmentList[i];
        ulong_t topva = segment->startAddress + segment->sizeInMemory;
        if(topva > maxva)
            maxva = topva;
    }
    KASSERT(maxva != 0);
    maxva = Round_Up_To_Page(maxva);
    
    unsigned int arg_num;
    ulong_t arg_block_size;
    Get_Argument_Block_Size(command, &arg_num, &arg_block_size);
    arg_block_size = Round_Up_To_Page(arg_block_size);

    ulong_t stack_size = DEFAULT_USER_STACK_SIZE;

    ulong_t virt_size = maxva + arg_block_size + stack_size;
    ulong_t virt_space = (ulong_t)Malloc(virt_size);
    KASSERT(virt_space != 0);
    memset(virt_space, 0, virt_size);
    ulong_t stack_addr = virt_space + maxva;
    ulong_t arg_block_addr = stack_addr + stack_size;

    /* Load segment into memory */
    for(int i = 0; i<exeFormat->numSegments; i++){
        struct Exe_Segment *segment = &exeFormat->segmentList[i];
        memcpy((void*)(virt_space+segment->startAddress), exeFileData+segment->offsetInFile, segment->lengthInFile);
    }
    (*pUserContext)->memory = (char*)virt_space;
    (*pUserContext)->size = virt_size;
    
    /* LDT */
    struct Segment_Descriptor *ldt_addr = (*pUserContext)->ldt;
    struct Segment_Descriptor *ldtDescriptor = Allocate_Segment_Descriptor();
    KASSERT(ldtDescriptor != 0);
    (*pUserContext)->ldtDescriptor = ldtDescriptor;
    Init_LDT_Descriptor(ldtDescriptor, ldt_addr, NUM_USER_LDT_ENTRIES);
    // LDT sel rpl is 0 or 3 ?
    (*pUserContext)->ldtSelector = Selector(0, true, Get_Descriptor_Index(ldtDescriptor));

    Init_Code_Segment_Descriptor(ldt_addr, virt_space, (virt_size/PAGE_SIZE), 3);
    (*pUserContext)->csSelector = Selector(3, false, 0);
    Init_Data_Segment_Descriptor(ldt_addr+1, virt_space, (virt_size/PAGE_SIZE), 3);
    (*pUserContext)->dsSelector = Selector(3, false, 1);

    // TODO
    // 1. 从逻辑地址映射到线性地址还没有完成，因此拷贝函数没有完成
    // 2. 下面第二行的Print函数两个数值输出本应相等却不想等
    struct Segment_Descriptor *dscp = ldt_addr + 1;
    // Print("virt space %0lx, addrH %0lx, addrL %0lx\n", virt_space, dscp->baseHigh, dscp->baseLow);
    // Print("Original base addr %08x, original dscp %016x, in dscp %0lx\n", 
    //     virt_space, *(unsigned long long*)(dscp), dscp->baseLow);
    /* Addresses */
    (*pUserContext)->entryAddr = exeFormat->entryAddr;
    Format_Argument_Block((char*)arg_block_addr, arg_num, maxva + stack_size, command);
    // Virtual address
    (*pUserContext)->argBlockAddr = arg_block_addr - virt_space;
    (*pUserContext)->stackPointerAddr = arg_block_addr - virt_space;

    (*pUserContext)->refCount = 0;
    return 0;
    // TODO("Load a user executable into a user memory space using segmentation");
}

/*
 * Copy data from user memory into a kernel buffer.
 * Params:
 * destInKernel - address of kernel buffer
 * srcInUser - address of user buffer
 * bufSize - number of bytes to copy
 *
 * Returns:
 *   true if successful, false if user buffer is invalid (i.e.,
 *   doesn't correspond to memory the process has a right to
 *   access)
 */
bool Copy_From_User(void* destInKernel, ulong_t srcInUser, ulong_t bufSize)
{
    /*
     * Hints:
     * - the User_Context of the current process can be found
     *   from g_currentThread->userContext
     * - the user address is an index relative to the chunk
     *   of memory you allocated for it
     * - make sure the user buffer lies entirely in memory belonging
     *   to the process
     */
    bool val = Validate_User_Memory(g_currentThread->userContext, srcInUser, bufSize);
    if(val == false){
        Print("val----1 %d\n", g_currentThread->userContext->size > srcInUser);
    }
    KASSERT(val == true);
    struct User_Context *userContext = g_currentThread->userContext;
    memcpy(destInKernel, srcInUser + userContext->memory, bufSize);
}

/*
 * Copy data from kernel memory into a user buffer.
 * Params:
 * destInUser - address of user buffer
 * srcInKernel - address of kernel buffer
 * bufSize - number of bytes to copy
 *
 * Returns:
 *   true if successful, false if user buffer is invalid (i.e.,
 *   doesn't correspond to memory the process has a right to
 *   access)
 */
bool Copy_To_User(ulong_t destInUser, void* srcInKernel, ulong_t bufSize)
{
    /*
     * Hints: same as for Copy_From_User()
     */
    TODO("Copy memory from kernel buffer to user buffer");
}

/*
* This is defined in lowlevel.asm.
*/
extern void Load_LDTR(ulong_t ldtSel);
/*
 * Switch to user address space belonging to given
 * User_Context object.
 * Params:
 * userContext - the User_Context
 */
void Switch_To_Address_Space(struct User_Context *userContext)
{
    /*
     * Hint: you will need to use the lldt assembly language instruction
     * to load the process's LDT by specifying its LDT selector.
     */

    
    Load_LDTR(userContext->ldtSelector);
    // TODO("Switch to user address space using segmentation/LDT");

}

