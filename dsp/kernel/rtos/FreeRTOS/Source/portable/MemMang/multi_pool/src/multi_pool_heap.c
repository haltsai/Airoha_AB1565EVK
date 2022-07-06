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

#include <stdlib.h>
#include <string.h>
#include "multi_pool_heap.h"
#include "exception_handler.h"
#include "hal.h"
#include "syslog.h"
#ifdef FREERTOS_ENABLE
#include "FreeRTOS.h"
#endif

#define heapRoundUp(xBuf, xAlignment)  (((xBuf) + (xAlignment) - 1) & (~(xAlignment - 1)))
#define heapPowerof2(x) ((((x) - 1) & (x)) == 0)

/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
static size_t xBlockAllocatedBit = 0;

/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
static const size_t xHeapStructSize	= heapRoundUp(sizeof(BlockLink_t), portBYTE_ALIGNMENT);

/* Block sizes must not get too small. */
#define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( xHeapStructSize * 2 ) )

/*Private functions -----------------------------------------------------------*/
#ifndef configASSERT
#define configASSERT( x ) if( (x) == 0 ) { platform_assert(#x, __FILE__, __LINE__); }
#endif

static void prvHeapInit(void *pucPoolBuffer, size_t xSizeInBytes, size_t xAlignment)
{
    BlockLink_t *pxFirstFreeBlock;
    uint8_t *pucAlignedHeap;
    uint32_t ulAddress;
    size_t xTotalHeapSize = xSizeInBytes;
    size_t xAlignRule = xAlignment;
    MultiPoolHeaderLink_t *pucHeapPoolBuffer = (MultiPoolHeaderLink_t *)pucPoolBuffer;

    /* Heap pool buffer should be portBYTE_ALIGNMENT align */
    configASSERT(((uint32_t)pucHeapPoolBuffer & portBYTE_ALIGNMENT) == 0);
    /* Alignment rule should be 2^n byte align, and n >= 2 */
    configASSERT(heapPowerof2(xAlignRule) && (xAlignRule >= 4));
    /* Heap pool size must not get too small. */
    configASSERT(xSizeInBytes >= sizeof(MultiPoolHeaderLink_t) + xHeapStructSize * 2);

    /* The size of the structure placed at the beginning of each allocated memory block must by correctly byte aligned. */
    pucHeapPoolBuffer->xPoolAlignment = xAlignRule;

    /* Heap pool header init */
    // memset((uint8_t *)pucHeapPoolBuffer, 0, sizeof(MultiPoolHeaderLink_t));
    pucHeapPoolBuffer->xMagicPattern = (uint32_t)MP_MAGIC_NUMBER;

    /* Ensure the heap block starts on a correctly aligned boundary. */
    ulAddress = (uint32_t)pucHeapPoolBuffer;
    ulAddress += sizeof(MultiPoolHeaderLink_t);
    xTotalHeapSize -= sizeof(MultiPoolHeaderLink_t);

    if ((ulAddress & portBYTE_ALIGNMENT_MASK) != 0) {
        ulAddress += (portBYTE_ALIGNMENT - 1);
        ulAddress &= ~portBYTE_ALIGNMENT_MASK;
        xTotalHeapSize -= (ulAddress - (uint32_t) pucHeapPoolBuffer);
    }

    pucAlignedHeap = (uint8_t *) ulAddress;

    /* xStart is used to hold a pointer to the first item in the list of free
    blocks.  The void cast is used to prevent compiler warnings. */
    pucHeapPoolBuffer->xStart.pxNextFreeBlock = (void *) pucAlignedHeap;
    pucHeapPoolBuffer->xStart.xBlockSize = (size_t) 0;
    /* pxEnd is used to mark the end of the list of free blocks and is inserted
    at the end of the heap space. */
    ulAddress = ((uint32_t) pucAlignedHeap) + xTotalHeapSize;
    ulAddress -= xHeapStructSize;
    ulAddress &= ~portBYTE_ALIGNMENT_MASK;
    pucHeapPoolBuffer->pxEnd = (void *) ulAddress;
    pucHeapPoolBuffer->pxEnd->xBlockSize = 0;
    pucHeapPoolBuffer->pxEnd->pxNextFreeBlock = NULL;
    pucHeapPoolBuffer->pxEnd->pxNextAllocatedBlock = NULL;
    pucHeapPoolBuffer->xStart.pxNextAllocatedBlock = pucHeapPoolBuffer->pxEnd;

    /* To start with there is a single free block that is sized to take up the
    entire heap space, minus the space taken by pxEnd. */
    pxFirstFreeBlock = (void *) pucAlignedHeap;
    pxFirstFreeBlock->xBlockSize = ulAddress - (uint32_t) pxFirstFreeBlock;
    pxFirstFreeBlock->pxNextFreeBlock = pucHeapPoolBuffer->pxEnd;
#if defined(MTK_SUPPORT_HEAP_DEBUG) || defined(MTK_HEAP_SIZE_GUARD_ENABLE)
    pucHeapPoolBuffer->pxFirstBlock = pxFirstFreeBlock;
#endif

    /* Only one block exists - and it covers the entire usable heap space. */
    pucHeapPoolBuffer->xMinimumEverFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;
    pucHeapPoolBuffer->xFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;

    /* Work out the position of the top bit in a size_t variable. */
    xBlockAllocatedBit = ((size_t) 1) << ((sizeof(size_t) * heapBITS_PER_BYTE) - 1);
}


static void prvInsertBlockIntoAllocatedList(BlockLink_t *pxBlockToInsert, BlockLink_t *pxStart)
{
    BlockLink_t *pxIterator;

    /* Iterate through the list until a block is found that has a higher address than the block being inserted. */
    for (pxIterator = pxStart; (pxIterator->pxNextAllocatedBlock < pxBlockToInsert) && (pxIterator->pxNextAllocatedBlock != NULL); pxIterator = pxIterator->pxNextAllocatedBlock) {
        /* Nothing to do here, just iterate to the right position. */
    }

    configASSERT(pxIterator->pxNextAllocatedBlock != NULL);

    /* Link the block to the allocated list */
    pxBlockToInsert->pxNextAllocatedBlock = pxIterator->pxNextAllocatedBlock;
    pxIterator->pxNextAllocatedBlock = pxBlockToInsert;
}
static void prvRemoveBlockFromAllocatedList(BlockLink_t *pxBlockToRemove, BlockLink_t *pxStart)
{
    BlockLink_t *pxIterator = pxStart;

    /* Iterate through the list until a block is found. */
    while ((pxIterator->pxNextAllocatedBlock != NULL) && (pxIterator->pxNextAllocatedBlock != pxBlockToRemove)) {
        pxIterator = pxIterator->pxNextAllocatedBlock;
    }

    /* The valid allocated block must be linked in the allocated list*/
    configASSERT(pxIterator->pxNextAllocatedBlock != NULL);

    /* Remove the block from the allocated list */
    pxIterator->pxNextAllocatedBlock = pxBlockToRemove->pxNextAllocatedBlock;
    pxBlockToRemove->pxNextAllocatedBlock = NULL;
}

static void prvInsertBlockIntoFreeList(BlockLink_t *pxBlockToInsert, BlockLink_t *pxStart, BlockLink_t *pxEnd)
{
    BlockLink_t *pxIterator;
    uint8_t *puc;

    /* Iterate through the list until a block is found that has a higher address
    than the block being inserted. */
    for (pxIterator = pxStart; pxIterator->pxNextFreeBlock < pxBlockToInsert; pxIterator = pxIterator->pxNextFreeBlock) {
        /* Nothing to do here, just iterate to the right position. */
    }

    /* Do the block being inserted, and the block it is being inserted after
    make a contiguous block of memory? */
    puc = (uint8_t *) pxIterator;
    if ((puc + pxIterator->xBlockSize) == (uint8_t *) pxBlockToInsert) {
        pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
        pxBlockToInsert = pxIterator;
    } else {
        //mtCOVERAGE_TEST_MARKER();
    }

    /* Do the block being inserted, and the block it is being inserted before
    make a contiguous block of memory? */
    puc = (uint8_t *) pxBlockToInsert;

    configASSERT(((uint32_t)pxIterator->pxNextFreeBlock > (uint32_t)pxBlockToInsert) && ((uint32_t)pxIterator->pxNextFreeBlock) <= (uint32_t)pxEnd);
    if ((puc + pxBlockToInsert->xBlockSize) == (uint8_t *) pxIterator->pxNextFreeBlock) {
        if (pxIterator->pxNextFreeBlock != pxEnd) {
            configASSERT(((uint32_t)pxIterator->pxNextFreeBlock->pxNextFreeBlock > (uint32_t)(pxIterator->pxNextFreeBlock)) && ((uint32_t)pxIterator->pxNextFreeBlock->pxNextFreeBlock <= (uint32_t)pxEnd));
            /* Form one big block from the two blocks. */
            pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;
            pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
        } else {
            pxBlockToInsert->pxNextFreeBlock = pxEnd;
        }
    } else {
        pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
    }

    /* If the block being inserted plugged a gab, so was merged with the block
    before and the block after, then it's pxNextFreeBlock pointer will have
    already been set, and should not be set here as that would make it point
    to itself. */
    if (pxIterator != pxBlockToInsert) {
        pxIterator->pxNextFreeBlock = pxBlockToInsert;
    } else {
        //mtCOVERAGE_TEST_MARKER();
    }
}

/*Public functions -----------------------------------------------------------*/
void mp_init(const multi_pool_region_t *const pxHeapRegions)
{
    uint32_t i;

    for (i = 0; pxHeapRegions[i].mp_size_in_bytes > 0; i++) {
        prvHeapInit(pxHeapRegions[i].mp_start_address, pxHeapRegions[i].mp_size_in_bytes, pxHeapRegions[i].mp_aligned_rule);
    }
}

void *mp_malloc(void *pucPoolBuffer, size_t xWantedSize)
{
    BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
    void *pvReturn = NULL;
    MultiPoolHeaderLink_t *pucHeapPoolBuffer = (MultiPoolHeaderLink_t *)pucPoolBuffer;
    BlockLink_t *pxStart = NULL, *pxEnd = NULL;
    size_t xAlignment;
    uint32_t mask;

#ifdef MTK_SUPPORT_HEAP_DEBUG
    /* Obtain the return address of caller from link register */
#if defined(__GNUC__)
    uint32_t xLinkRegAddr = (uint32_t)__builtin_return_address(0);
#elif defined(__CC_ARM)
    uint32_t xLinkRegAddr = __return_address();
#elif defined(__ICCARM__)
    uint32_t xLinkRegAddr = __get_LR();
#endif /* __GNUC__ */
#endif /* MTK_SUPPORT_HEAP_DEBUG */

    /* pool must be initialized by obvious call mp_init(), then can be used */
    configASSERT(pucHeapPoolBuffer->pxEnd != NULL);

    /* check heap pool validity */
    configASSERT(pucHeapPoolBuffer->xMagicPattern == (uint32_t)MP_MAGIC_NUMBER);

    xAlignment = pucHeapPoolBuffer->xPoolAlignment;
    /* Heap pool buffer should be 2^n byte align, and n >= 2 */
    configASSERT(heapPowerof2(xAlignment) && (xAlignment >= 4));

    pxStart = &(pucHeapPoolBuffer->xStart);
    pxEnd = pucHeapPoolBuffer->pxEnd;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    {
        /* Check the requested block size is not so large that the top bit is
        set.  The top bit of the block size member of the BlockLink_t structure
        is used to determine who owns the block - the application or the
        kernel, so it must be free. */
        if ((xWantedSize & xBlockAllocatedBit) == 0) {
            if ((xWantedSize > 0)
                && (heapRoundUp(xWantedSize + xHeapStructSize, xAlignment) > xWantedSize) /* overflow check */
                && (heapRoundUp(xWantedSize + xHeapStructSize, xAlignment) <= pucHeapPoolBuffer->xFreeBytesRemaining)) {
                /* Traverse the list from the start (lowest address) block until
                one of adequate size is found. */
                pxPreviousBlock = pxStart;
                pxBlock = pxStart->pxNextFreeBlock;

                while (pxBlock->pxNextFreeBlock != NULL) {
                    if (heapRoundUp((size_t)pxBlock + xHeapStructSize, xAlignment) - (size_t)pxBlock + xWantedSize <= pxBlock->xBlockSize) {
                        break;

                    } else {
                        pxPreviousBlock = pxBlock;
                        pxBlock = pxBlock->pxNextFreeBlock;
                    }
                }

                /* If the end marker was reached then a block of adequate size
                was not found. */
                if (pxBlock != pxEnd) {
                    /* Return the memory space pointed to - jumping over the
                    BlockLink_t structure at its start. */
                    pvReturn = (void *)heapRoundUp(((size_t)pxBlock + xHeapStructSize), xAlignment);

                    /* This block is being returned for use so must be taken out
                    of the list of free blocks. */
                    pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

                    /* Tune the block real needed size */
                    xWantedSize += ((size_t) pvReturn - (size_t)pxBlock);
                    xWantedSize = heapRoundUp(xWantedSize, portBYTE_ALIGNMENT);

                    /* If the block is larger than required it can be split into
                    two. */
                    if ((pxBlock->xBlockSize - xWantedSize) > heapMINIMUM_BLOCK_SIZE) {
                        /* This block is to be split into two.  Create a new
                        block following the number of bytes requested. The void
                        cast is used to prevent byte alignment warnings from the
                        compiler. */
                        pxNewBlockLink = (void *)(((uint8_t *) pxBlock) + xWantedSize);
                        configASSERT((((uint32_t) pxNewBlockLink) & portBYTE_ALIGNMENT_MASK) == 0);

                        /* Calculate the sizes of two blocks split from the
                        single block. */
                        pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
                        pxBlock->xBlockSize = xWantedSize;

                        /* Insert the new block into the list of free blocks. */
                        prvInsertBlockIntoFreeList(pxNewBlockLink, pxStart, pxEnd);
                    } else {
                        //mtCOVERAGE_TEST_MARKER();
                    }

                    pucHeapPoolBuffer->xFreeBytesRemaining -= pxBlock->xBlockSize;

                    if (pucHeapPoolBuffer->xFreeBytesRemaining < pucHeapPoolBuffer->xMinimumEverFreeBytesRemaining) {
                        pucHeapPoolBuffer->xMinimumEverFreeBytesRemaining = pucHeapPoolBuffer->xFreeBytesRemaining;
                    } else {
                        //mtCOVERAGE_TEST_MARKER();
                    }

                    /* Insert this block into the list of allocated blocks */
                    prvInsertBlockIntoAllocatedList(pxBlock, pxStart);

                    /* The block is being returned - it is allocated and owned
                    by the application and has no "next" block. */
                    pxBlock->xBlockSize |= xBlockAllocatedBit;
                    pxBlock->pxNextFreeBlock = NULL;
#ifdef MTK_SUPPORT_HEAP_DEBUG
                    pxBlock->xMallocLinkRegAddr = xLinkRegAddr;
#endif /* MTK_SUPPORT_HEAP_DEBUG */
                } else {
                    //mtCOVERAGE_TEST_MARKER();
                }
            } else {
                //mtCOVERAGE_TEST_MARKER();
            }
        } else {
            //mtCOVERAGE_TEST_MARKER();
        }

        //traceMALLOC(pvReturn, xWantedSize);
    }
    hal_nvic_restore_interrupt_mask(mask);

#if( configUSE_MALLOC_FAILED_HOOK == 1 )
    {
        if ((pvReturn == NULL) && (xWantedSize > 0)) {
            extern void vApplicationMallocFailedHook(void *pucPoolBuffer);
            vApplicationMallocFailedHook(pucPoolBuffer);
        } else {
            //mtCOVERAGE_TEST_MARKER();
        }
    }
#endif

    configASSERT((((uint32_t) pvReturn) & (xAlignment - 1)) == 0);
    return pvReturn;
}

/*-----------------------------------------------------------*/
void mp_free(void *pucPoolBuffer, void *pv)
{
    BlockLink_t *pxLink, *pxNextLink;
    MultiPoolHeaderLink_t *pucHeapPoolBuffer;
    BlockLink_t *pxStart = NULL, *pxEnd = NULL;
    uint32_t mask;

#ifdef MTK_SUPPORT_HEAP_DEBUG
    /* Obtain the return address of caller from link register */
#if defined(__GNUC__)
    uint32_t xLinkRegAddr = (uint32_t)__builtin_return_address(0);
#elif defined(__CC_ARM)
    uint32_t xLinkRegAddr = __return_address();
#elif defined(__ICCARM__)
    uint32_t xLinkRegAddr = __get_LR();
#endif /* __GNUC__ */
#endif /* MTK_SUPPORT_HEAP_DEBUG */

    if (pv != NULL) {
        /* This casting is to keep the compiler from issuing warnings. */
        pucHeapPoolBuffer = (MultiPoolHeaderLink_t *)pucPoolBuffer;

        /* Check the pool validity. */
        configASSERT(pucHeapPoolBuffer != NULL);
        configASSERT(pucHeapPoolBuffer->xMagicPattern == (uint32_t)MP_MAGIC_NUMBER);

        /* The memory being freed will have an BlockLink_t structure immediately before it. */
        pxLink = mp_get_block_to_free(pv, &(pucHeapPoolBuffer->xStart), pucHeapPoolBuffer->xPoolAlignment);

        /* Check the block is actually allocated. */
        configASSERT(pxLink != NULL);
        configASSERT((pxLink->xBlockSize & xBlockAllocatedBit) != 0);
        configASSERT(pxLink->pxNextFreeBlock == NULL);
        configASSERT(pxLink->pxNextAllocatedBlock != NULL);

#ifdef MTK_SUPPORT_HEAP_DEBUG
#ifdef FREERTOS_ENABLE
        extern uint8_t ucHeap[];
        if ((uint32_t)pucHeapPoolBuffer != (uint32_t)ucHeap) {
            pxLink->xFreeLinkRegAddr = xLinkRegAddr;
        }
#else
        pxLink->xFreeLinkRegAddr = xLinkRegAddr;
#endif /* FREERTOS_ENABLE */
#endif /* MTK_SUPPORT_HEAP_DEBUG */

        hal_nvic_save_and_set_interrupt_mask(&mask);
        {
            /* The block is being returned to the heap - it is no longer
            allocated. */
            #ifdef MTK_HEAP_SIZE_GUARD_ENABLE
            #define GET_ACTUAL_BLOCK_SIZE(xBlockSize)  ((xBlockSize) & (uint32_t)0x00FFFFFF)    /* mask off top byte*/
            pxLink->xBlockSize = GET_ACTUAL_BLOCK_SIZE(pxLink->xBlockSize);
            #else
            pxLink->xBlockSize &= ~xBlockAllocatedBit;
            #endif /* MTK_HEAP_SIZE_GUARD_ENABLE */

            /* To check is there any memory corruption on the next block.
            ** MTK_HEAP_SIZE_GUARD_ENABLE feature is a internal debug feature, and will use some bits of xBlockSize.
            ** Ignore the check when MTK_HEAP_SIZE_GUARD_ENABLE feature is enabled.
            */
            #ifndef MTK_HEAP_SIZE_GUARD_ENABLE
            {
                /* Get the next block address */
                pxNextLink = ( BlockLink_t * ) ((uint8_t *)pxLink + (pxLink->xBlockSize & ~xBlockAllocatedBit));
                if((pxNextLink != NULL) && (pxNextLink != pucHeapPoolBuffer->pxEnd))
                {
                    /* the next block size must be bigger than the heap header size */
                    configASSERT((pxNextLink->xBlockSize & ~xBlockAllocatedBit) >= xHeapStructSize)
                    /* the next block must be in the valid heap space */
                    configASSERT( ((uint32_t)(pxNextLink) >= (uint32_t)pucHeapPoolBuffer + sizeof(MultiPoolHeaderLink_t)) && ((uint32_t)(pxNextLink) < (uint32_t)(pucHeapPoolBuffer->pxEnd)));

                    if(pxNextLink->pxNextFreeBlock == NULL){
                        /* the block is a in-use block, the allocated-flag must be set */
                        configASSERT( ( pxNextLink->xBlockSize & xBlockAllocatedBit ) != 0 );
                    }else{
                        /* the block is a free block, the next-pointer must be in the valid heap space */
                        configASSERT( ((uint32_t)(pxNextLink->pxNextFreeBlock) >= (uint32_t)pucHeapPoolBuffer + sizeof(MultiPoolHeaderLink_t)) && ((uint32_t)(pxNextLink->pxNextFreeBlock) <= (uint32_t)(pucHeapPoolBuffer->pxEnd)));
                    }
                }
            }
            #endif /* MTK_HEAP_SIZE_GUARD_ENABLE */

            pxStart = &(pucHeapPoolBuffer->xStart);
            pxEnd = pucHeapPoolBuffer->pxEnd;

            /* Remove this block from the list of allocated blocks. */
            prvRemoveBlockFromAllocatedList(pxLink, pxStart);
            pxLink->pxNextAllocatedBlock = NULL;

            /* Add this block to the list of free blocks. */
            pucHeapPoolBuffer->xFreeBytesRemaining += pxLink->xBlockSize;
            //traceFREE(pv, pxLink->xBlockSize);
            prvInsertBlockIntoFreeList((BlockLink_t *) pxLink, pxStart, pxEnd);

        }
        hal_nvic_restore_interrupt_mask(mask);

    }

}

BlockLink_t *mp_get_block_to_free(void *pxBlock, BlockLink_t *pxStart, size_t xAlignment)
{
    BlockLink_t *pxIterator = pxStart;
    uint32_t mask;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    /* Iterate through the list until a block is found. */
    while ((pxIterator->pxNextAllocatedBlock != NULL) && (pxIterator->pxNextAllocatedBlock < (BlockLink_t *)pxBlock)) {
        pxIterator = pxIterator->pxNextAllocatedBlock;
    }
    hal_nvic_restore_interrupt_mask(mask);

    /*
    	head        res                  xUserSize
        |_____|________|______________________|
        p1    p2       p3

      p1: block begin address
      p2: the res begin address
      p3: user address, which is the return address of mp_malloc(), is aligned to the pool alignment rule
      p2 - p1: the block header size (xHeapStructSize)
      p3 - p2: the res size for the pool alignment. the res is different depend on the block location
      */
    configASSERT((pxIterator->pxNextAllocatedBlock != NULL)); /* not find the block to free */
    configASSERT(heapRoundUp(((size_t)pxIterator + xHeapStructSize), xAlignment) == (size_t)pxBlock);

    return pxIterator;
}

/*-----------------------------------------------------------*/
size_t mp_get_free_heap_size(void *pucPoolBuffer)
{
    MultiPoolHeaderLink_t *pucHeapPoolBuffer = (MultiPoolHeaderLink_t *)pucPoolBuffer;

    configASSERT(pucHeapPoolBuffer != NULL);
    configASSERT(pucHeapPoolBuffer->xMagicPattern == (uint32_t)MP_MAGIC_NUMBER);
    return pucHeapPoolBuffer->xFreeBytesRemaining;
}

/*-----------------------------------------------------------*/
size_t mp_get_max_free_block_size(void *pucPoolBuffer)
{
    MultiPoolHeaderLink_t *pucHeapPoolBuffer = (MultiPoolHeaderLink_t *)pucPoolBuffer;
    BlockLink_t *pxStart = NULL;
    BlockLink_t *pxBlock;
    size_t xMaxFreeBlockSize = 0;
    uint32_t mask;

    configASSERT(pucHeapPoolBuffer != NULL);
    configASSERT(pucHeapPoolBuffer->xMagicPattern == (uint32_t)MP_MAGIC_NUMBER);

    pxStart = &(pucHeapPoolBuffer->xStart);

    hal_nvic_save_and_set_interrupt_mask(&mask);

    /* loop the free block list to get the max free block size */
    if (pxStart != NULL) {
        pxBlock = pxStart->pxNextFreeBlock;
        while (pxBlock->pxNextFreeBlock != NULL) {

            if (pxBlock->xBlockSize > xMaxFreeBlockSize) {
                xMaxFreeBlockSize = pxBlock->xBlockSize;
            }

            pxBlock = pxBlock->pxNextFreeBlock;
        }
    }
    hal_nvic_restore_interrupt_mask(mask);

    /* return the user size */
    return (xMaxFreeBlockSize >= xHeapStructSize) ? (xMaxFreeBlockSize - xHeapStructSize) : 0;
}

/*-----------------------------------------------------------*/
size_t mp_get_minimum_ever_free_heap_size(void *pucPoolBuffer)
{
    MultiPoolHeaderLink_t *pucHeapPoolBuffer = (MultiPoolHeaderLink_t *)pucPoolBuffer;

    configASSERT(pucHeapPoolBuffer != NULL);
    configASSERT(pucHeapPoolBuffer->xMagicPattern == (uint32_t)MP_MAGIC_NUMBER);
    return pucHeapPoolBuffer->xMinimumEverFreeBytesRemaining;
}

#if defined(MTK_SUPPORT_HEAP_DEBUG) || defined(MTK_HEAP_SIZE_GUARD_ENABLE)
void mp_dump_status(void *pucPoolBuffer)
{
    MultiPoolHeaderLink_t *pucHeapPoolBuffer = (MultiPoolHeaderLink_t *)pucPoolBuffer;
    BlockLink_t *blk_iter = pucHeapPoolBuffer->pxFirstBlock;
    int32_t blk_size = 0;

    LOG_MSGID_I(freertos, "heap block dump begin \r\n", 0);

    while (blk_iter != pucHeapPoolBuffer->pxEnd) {
        /* DSP don't support unalign access, so must check the alignment before access. */
        if(((uint32_t)blk_iter & 0x3) != 0){
            LOG_MSGID_I(common, "heap crash!!!!!", 0);
            return;
        }
#ifdef MTK_HEAP_SIZE_GUARD_ENABLE
        #define GET_ACTUAL_BLOCK_SIZE(xBlockSize)  ((xBlockSize) & (uint32_t)0x00FFFFFF)	/* mask off top byte*/
        blk_size = GET_ACTUAL_BLOCK_SIZE(blk_iter->xBlockSize);
        LOG_MSGID_I(freertos, "block start = 0x%x,    size = 0x%x", 2, (unsigned int)blk_iter, (unsigned int)blk_iter->xBlockSize);
#else
        blk_size = blk_iter->xBlockSize & ~xBlockAllocatedBit;
        LOG_MSGID_I(freertos, "block start = 0x%x,    pxNext=0x%x,    size = 0x%x,    lr = 0x%x", 4,
            (unsigned int)blk_iter,
            (unsigned int)blk_iter->pxNextFreeBlock,
            (unsigned int)blk_iter->xBlockSize,
            (blk_iter->xBlockSize & xBlockAllocatedBit)? (unsigned int)(blk_iter->xMallocLinkRegAddr) : (unsigned int)(blk_iter->xFreeLinkRegAddr));
#endif /* MTK_HEAP_SIZE_GUARD_ENABLE */

        blk_iter = (BlockLink_t *)((uint32_t)blk_iter + blk_size);

        if (((uint32_t)blk_iter > (uint32_t)pucHeapPoolBuffer->pxEnd) || ((uint32_t)blk_iter < (uint32_t)pucHeapPoolBuffer) || (blk_size <= 0)) {
            LOG_MSGID_I(freertos, "heap crash!!!!!", 0);
            return;
        }
    }

#ifdef MTK_HEAP_SIZE_GUARD_ENABLE
    LOG_MSGID_I(freertos, "block start = 0x%x,    size = 0x%x", 2, (unsigned int)blk_iter, (unsigned int)blk_iter->xBlockSize);
#else
    LOG_MSGID_I(freertos, "block start = 0x%x,    pxNext=0x%x,    size = 0x%x,    lr = 0x%x", 4,
        (unsigned int)blk_iter,
        (unsigned int)blk_iter->pxNextFreeBlock,
        (unsigned int)blk_iter->xBlockSize,
        (blk_iter->xBlockSize & xBlockAllocatedBit)? (unsigned int)(blk_iter->xMallocLinkRegAddr) : (unsigned int)(blk_iter->xFreeLinkRegAddr));
#endif /* MTK_HEAP_SIZE_GUARD_ENABLE */

    LOG_MSGID_I(freertos, "reach blk_end", 0);
}
#endif /* defined(MTK_SUPPORT_HEAP_DEBUG) || defined(MTK_HEAP_SIZE_GUARD_ENABLE) */


/* if w/o FreeRTOS, wrap the c stand library malloc family, include malloc/calloc/realloc/free to multi-pool heap service */
#ifndef FREERTOS_ENABLE
/* please define the system heap regions in your project's main.c
*/
extern multi_pool_region_t system_heap_regions[];
extern uint8_t system_heap[];

static uint32_t heap_init_flag = 0;

void *__wrap_malloc(size_t size)
{
    if (!heap_init_flag) {
        /* Init the system heap before usage*/
        mp_init(system_heap_regions);
        heap_init_flag = 1;
    }

    return mp_malloc((void *)system_heap, size);
}
void __wrap_free(void *pv)
{
    if (!heap_init_flag) {
        configASSERT(0);
    }

    mp_free((void *)system_heap, pv);
}

#endif /* FREERTOS_ENABLE */

/*-----------------------------------------------------------*/

