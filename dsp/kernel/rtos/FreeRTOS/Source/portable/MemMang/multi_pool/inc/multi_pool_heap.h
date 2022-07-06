/*
 * FreeRTOS Kernel V10.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef __MULTI_POOL_HEAP_H__
#define __MULTI_POOL_HEAP_H__

#include <stdint.h>

/* Define the linked list structure.  This is used to link free and allocated blocks in order
of their memory address. */
typedef struct A_BLOCK_LINK {
    struct A_BLOCK_LINK *pxNextFreeBlock;        /*<< The next free block in the list. */
    struct A_BLOCK_LINK *pxNextAllocatedBlock;   /*<< The next allocated block in the list. */
    size_t xBlockSize;                           /*<< The size of the free block. */
#ifdef MTK_SUPPORT_HEAP_DEBUG
    uint32_t xMallocLinkRegAddr;
    uint32_t xFreeLinkRegAddr;
#endif /* MTK_SUPPORT_HEAP_DEBUG */
} BlockLink_t;

/* Define the heap pool header structure.  This is used to mange the heap pool. */
typedef struct A_MULTI_POOL_HEADER_LINK {
    struct A_BLOCK_LINK xStart;             /* 8B << The free block start. */
    struct A_BLOCK_LINK *pxEnd;             /* 4B << The end block of the pool. */
    size_t xFreeBytesRemaining;             /* 4B << The number of free bytes remaining, but says nothing about fragmentation. */
    size_t xMinimumEverFreeBytesRemaining;  /* 4B << The ever minimum free bytes remaining, but says nothing about fragmentation. */
    size_t xPoolAlignment;                  /* 4B << The pool alignment rule, each allocated memory block will be aligned with this rule. */
#if defined(MTK_SUPPORT_HEAP_DEBUG) || defined(MTK_HEAP_SIZE_GUARD_ENABLE)
    struct A_BLOCK_LINK *pxFirstBlock;      /* 4B << Record first block of heap for heap walk */
#endif
    size_t xMagicPattern;                   /* 4B << The magic number for pool validity check */
} MultiPoolHeaderLink_t;

/* Multi heap pool header magic number */
#define MP_MAGIC_NUMBER (uint32_t)0xF1F2F3F4

/* Align to the dsp\kernel\rtos\FreeRTOS\Source\portable\XCC\Xtensa\portmacro.h */
#ifndef portBYTE_ALIGNMENT
#define portBYTE_ALIGNMENT         4
#define portBYTE_ALIGNMENT_MASK    ( 0x0003 )
#endif

/* Assumes 8bit bytes! */
#define heapBITS_PER_BYTE          ( ( size_t ) 8 )

/*-----------------------------------------------------------*/
/*
 * mp_multi_pool_init() takes a single parameter.  The parameter is an array
 * of multi_pool_region_t structures.
 *
 * typedef struct {
 *    void *mp_start_address;       << Start address of a block of memory that will be part of the heap.
 *    size_t mp_size_in_bytes;      << Size of the block of memory.
 *    size_t mp_aligned_rule ;      << Memory alignment rule, must be 2^n (n=2,3,4...)
 * } multi_pool_region_t;
 *
 * The array is terminated using a NULL zero sized region definition,so the following is a valid example of how
 * to use the function.
 *
 * multi_pool_region_t xHeapRegions[] =
 * {
 *    { ( void * ) 0x80000000UL, 0x10000, 0x4 }, << Defines a block of 0x10000 bytes starting at address 0x80000000, every block is 4-Byte align
 *    { ( void * ) 0x90000000UL, 0xa0000, 0x8 }, << Defines a block of 0xa0000 bytes starting at address of 0x90000000, every block is 8-Byte align
 *    { NULL, 0, 0 }                           << Terminates the array.
 * };
 *
 * mp_init( xHeapRegions ); << Pass the array into mp_multi_pool_init().
 *
 */
typedef struct {
    void *mp_start_address;
    size_t mp_size_in_bytes;
    size_t mp_aligned_rule;
} multi_pool_region_t;

void mp_init(const multi_pool_region_t  *const pxHeapRegions);
void *mp_malloc(void *pucPoolBuffer, size_t xWantedSize);
void mp_free(void *pucPoolBuffer, void *pv);
BlockLink_t *mp_get_block_to_free(void *pxBlock, BlockLink_t *pxStart, size_t xAlignment);
size_t mp_get_free_heap_size(void *pucPoolBuffer);
size_t mp_get_max_free_block_size(void *pucPoolBuffer);
size_t mp_get_minimum_ever_free_heap_size(void *pucPoolBuffer);

#if defined(MTK_SUPPORT_HEAP_DEBUG) || defined(MTK_HEAP_SIZE_GUARD_ENABLE)
void mp_dump_status(void *pucPoolBuffer);
#endif /* defined(MTK_SUPPORT_HEAP_DEBUG) || defined(MTK_HEAP_SIZE_GUARD_ENABLE) */

#endif /* __MULTI_POOL_HEAP_H__ */
