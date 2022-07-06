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

/*
 * A sample implementation of pvPortMalloc() and vPortFree() that combines
 * (coalescences) adjacent memory blocks as they are freed, and in so doing
 * limits memory fragmentation.
 *
 * See heap_1.c, heap_2.c and heap_3.c for alternative implementations, and the
 * memory management pages of http://www.FreeRTOS.org for more information.
 */
#include <stdlib.h>
#include <string.h>
/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"
#include "multi_pool_heap.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#if( configSUPPORT_DYNAMIC_ALLOCATION == 0 )
	#error This file must not be used if configSUPPORT_DYNAMIC_ALLOCATION is 0
#endif

/* Block sizes must not get too small. */
#define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( xHeapStructSize << 1 ) )

/* Assumes 8bit bytes! */
#define heapBITS_PER_BYTE		( ( size_t ) 8 )

/* Allocate the memory for the heap. */
#if( configAPPLICATION_ALLOCATED_HEAP == 1 )
	/* The application writer has already defined the array used for the RTOS
	heap - probably so it can be placed in a special segment or address. */
	extern uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#else
	uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#endif /* configAPPLICATION_ALLOCATED_HEAP */

static multi_pool_region_t xHeapRegions[] =
{
		{(void*)ucHeap, configTOTAL_HEAP_SIZE, portBYTE_ALIGNMENT},
		{ NULL, 0, 0}
};

/* Create a couple of list links to mark the start and end of the list. */
static BlockLink_t *pxEnd = NULL;
/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
static size_t xBlockAllocatedBit = 0;
/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
static const size_t xHeapStructSize	= ( ( sizeof( BlockLink_t ) + ( portBYTE_ALIGNMENT - 1 ) ) & ~portBYTE_ALIGNMENT_MASK );
/* Assumes 4bit bytes! */
#if 0

/* Define the linked list structure.  This is used to link free blocks in order
of their memory address. */
typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
	size_t xBlockSize;						/*<< The size of the free block. */
#ifdef MTK_SUPPORT_HEAP_DEBUG
	uint32_t xMallocLinkRegAddr;
	uint32_t xFreeLinkRegAddr;
#endif /* MTK_SUPPORT_HEAP_DEBUG */
} BlockLink_t;

/*-----------------------------------------------------------*/

/*
 * Inserts a block of memory that is being freed into the correct position in
 * the list of free memory blocks.  The block being freed will be merged with
 * the block in front it and/or the block behind it if the memory blocks are
 * adjacent to each other.
 */
static void prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert );

/*
 * Called automatically to setup the required heap structures the first time
 * pvPortMalloc() is called.
 */
static void prvHeapInit( void );

/*-----------------------------------------------------------*/

/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
static const size_t xHeapStructSize	= ( sizeof( BlockLink_t ) + ( ( size_t ) ( portBYTE_ALIGNMENT - 1 ) ) ) & ~( ( size_t ) portBYTE_ALIGNMENT_MASK );

/* Create a couple of list links to mark the start and end of the list. */
static BlockLink_t xStart, *pxEnd = NULL;

/* Keeps track of the number of free bytes remaining, but says nothing about
fragmentation. */
static size_t xFreeBytesRemaining = 0U;
static size_t xMinimumEverFreeBytesRemaining = 0U;

/* Gets set to the top bit of an size_t type.  When this bit in the xBlockSize
member of an BlockLink_t structure is set then the block belongs to the
application.  When the bit is free the block is still part of the free heap
space. */
static size_t xBlockAllocatedBit = 0;
#endif
/*-----------------------------------------------------------*/

void *pvPortMalloc( size_t xWantedSize )
{
void *pvReturn = NULL;
MultiPoolHeaderLink_t *pucHeapPool = (MultiPoolHeaderLink_t *)ucHeap;
#ifdef MTK_SUPPORT_HEAP_DEBUG
BlockLink_t *pxBlock;
/* Obtain the return address of caller from link register */
#if defined(__GNUC__)
uint32_t xLinkRegAddr = (uint32_t)__builtin_return_address(0);
#elif defined(__CC_ARM)
uint32_t xLinkRegAddr = __return_address();
#elif defined(__ICCARM__)
uint32_t xLinkRegAddr = __get_LR();
#endif /* __GNUC__ */
#endif /* MTK_SUPPORT_HEAP_DEBUG */

	/* If this is the first call to malloc then the heap will require
	initialisation to setup the list of free blocks. */
	if( pxEnd == NULL )
	{
		mp_init(xHeapRegions);
        /* Set pxEnd to the pxEnd of the MultiPoolHeaderLink_t*/
        pxEnd = pucHeapPool->pxEnd;
        /* Work out the position of the top bit in a size_t variable. */
        xBlockAllocatedBit = ((size_t) 1) << ((sizeof(size_t) * heapBITS_PER_BYTE) - 1);

        /* Register heap dump callback */
        #if 0//defined(MTK_SUPPORT_HEAP_DEBUG)
        extern void prvPortDumpHeapInfo(void);
        prvPortDumpHeapInfo();
        #endif /* MTK_SUPPORT_HEAP_DEBUG */
	}

	pvReturn = mp_malloc((void*)ucHeap, xWantedSize);

	#ifdef MTK_SUPPORT_HEAP_DEBUG
    if(pvReturn != NULL)
    {
        pxBlock = mp_get_block_to_free(pvReturn, &(pucHeapPool->xStart), pucHeapPool->xPoolAlignment);
	    if(pxBlock != NULL)
	    {
		    pxBlock->xMallocLinkRegAddr = xLinkRegAddr;
	    }
    }
	#endif

	return pvReturn;
}
/*-----------------------------------------------------------*/

void vPortFree( void *pv )
{
#ifdef MTK_SUPPORT_HEAP_DEBUG
MultiPoolHeaderLink_t *pucHeapPool = (MultiPoolHeaderLink_t *)ucHeap;
BlockLink_t *pxBlock;
/* Obtain the return address of caller from link register */
#if defined(__GNUC__)
uint32_t xLinkRegAddr = (uint32_t)__builtin_return_address(0);
#elif defined(__CC_ARM)
uint32_t xLinkRegAddr = __return_address();
#elif defined(__ICCARM__)
uint32_t xLinkRegAddr = __get_LR();
#endif /* __GNUC__ */

	if(pv != NULL){
        pxBlock = mp_get_block_to_free(pv, &(pucHeapPool->xStart), pucHeapPool->xPoolAlignment);
	    //pxBlock = (BlockLink_t *)((uint8_t *)pvReturn - xHeapStructSize);
		if(pxBlock != NULL){
			pxBlock->xFreeLinkRegAddr = xLinkRegAddr;
		}
	}
#endif /* MTK_SUPPORT_HEAP_DEBUG */

	mp_free((void*)ucHeap, pv);
}
/*-----------------------------------------------------------*/

size_t xPortGetFreeHeapSize( void )
{
	return mp_get_free_heap_size((void*)ucHeap);
}
/*-----------------------------------------------------------*/

size_t xPortGetMinimumEverFreeHeapSize( void )
{
	return mp_get_minimum_ever_free_heap_size((void*)ucHeap);
}

size_t xPortGetMaxFreeBlockSize( void )
{
	return mp_get_max_free_block_size((void*)ucHeap);
}
/*-----------------------------------------------------------*/

void vPortInitialiseBlocks( void )
{
	/* This just exists to keep the linker quiet. */
}
#if 0
/*-----------------------------------------------------------*/

static void prvHeapInit( void )
{
BlockLink_t *pxFirstFreeBlock;
uint8_t *pucAlignedHeap;
size_t uxAddress;
size_t xTotalHeapSize = configTOTAL_HEAP_SIZE;

	/* Ensure the heap starts on a correctly aligned boundary. */
	uxAddress = ( size_t ) ucHeap;

	if( ( uxAddress & portBYTE_ALIGNMENT_MASK ) != 0 )
	{
		uxAddress += ( portBYTE_ALIGNMENT - 1 );
		uxAddress &= ~( ( size_t ) portBYTE_ALIGNMENT_MASK );
		xTotalHeapSize -= uxAddress - ( size_t ) ucHeap;
	}

	pucAlignedHeap = ( uint8_t * ) uxAddress;

	/* xStart is used to hold a pointer to the first item in the list of free
	blocks.  The void cast is used to prevent compiler warnings. */
	xStart.pxNextFreeBlock = ( void * ) pucAlignedHeap;
	xStart.xBlockSize = ( size_t ) 0;

	/* pxEnd is used to mark the end of the list of free blocks and is inserted
	at the end of the heap space. */
	uxAddress = ( ( size_t ) pucAlignedHeap ) + xTotalHeapSize;
	uxAddress -= xHeapStructSize;
	uxAddress &= ~( ( size_t ) portBYTE_ALIGNMENT_MASK );
	pxEnd = ( void * ) uxAddress;
	pxEnd->xBlockSize = 0;
	pxEnd->pxNextFreeBlock = NULL;

	/* To start with there is a single free block that is sized to take up the
	entire heap space, minus the space taken by pxEnd. */
	pxFirstFreeBlock = ( void * ) pucAlignedHeap;
	pxFirstFreeBlock->xBlockSize = uxAddress - ( size_t ) pxFirstFreeBlock;
	pxFirstFreeBlock->pxNextFreeBlock = pxEnd;

	/* Only one block exists - and it covers the entire usable heap space. */
	xMinimumEverFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;
	xFreeBytesRemaining = pxFirstFreeBlock->xBlockSize;

	/* Work out the position of the top bit in a size_t variable. */
	xBlockAllocatedBit = ( ( size_t ) 1 ) << ( ( sizeof( size_t ) * heapBITS_PER_BYTE ) - 1 );
}
/*-----------------------------------------------------------*/

static void prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert )
{
BlockLink_t *pxIterator;
uint8_t *puc;

	/* Iterate through the list until a block is found that has a higher address
	than the block being inserted. */
	for( pxIterator = &xStart; pxIterator->pxNextFreeBlock < pxBlockToInsert; pxIterator = pxIterator->pxNextFreeBlock )
	{
		/* Nothing to do here, just iterate to the right position. */
	}

	/* Do the block being inserted, and the block it is being inserted after
	make a contiguous block of memory? */
	puc = ( uint8_t * ) pxIterator;
	if( ( puc + pxIterator->xBlockSize ) == ( uint8_t * ) pxBlockToInsert )
	{
		pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
		pxBlockToInsert = pxIterator;
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}

	/* Do the block being inserted, and the block it is being inserted before
	make a contiguous block of memory? */
	puc = ( uint8_t * ) pxBlockToInsert;
	if( ( puc + pxBlockToInsert->xBlockSize ) == ( uint8_t * ) pxIterator->pxNextFreeBlock )
	{
		if( pxIterator->pxNextFreeBlock != pxEnd )
		{
			/* Form one big block from the two blocks. */
			pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;
			pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
		}
		else
		{
			pxBlockToInsert->pxNextFreeBlock = pxEnd;
		}
	}
	else
	{
		pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
	}

	/* If the block being inserted plugged a gab, so was merged with the block
	before and the block after, then it's pxNextFreeBlock pointer will have
	already been set, and should not be set here as that would make it point
	to itself. */
	if( pxIterator != pxBlockToInsert )
	{
		pxIterator->pxNextFreeBlock = pxBlockToInsert;
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}
}
#endif

/*-----------------------------------------------------------*/

void *pvPortCalloc( size_t nmemb, size_t size )
{
    void *pvReturn;

	/* unsigned integer wrap around protection */
	#define __LIM ( 1 << ( sizeof( size_t ) * 8 / 2 ) )
	if( ( nmemb | size ) >= __LIM && size && ( nmemb > SIZE_MAX / size ) )
	{
	    size = 0;
	}

#ifdef MTK_HEAP_SIZE_GUARD_ENABLE
	#if defined(__GNUC__)
	extern void *__wrap_pvPortMalloc(size_t);
	pvReturn = (void *)__wrap_pvPortMalloc(nmemb*size);
	#elif defined(__CC_ARM)
	pvReturn = pvPortMalloc( nmemb*size );
	#endif /* __GNUC__ */
#else
    pvReturn = pvPortMalloc( nmemb*size );
#endif /* MTK_HEAP_SIZE_GUARD_ENABLE */
    if (pvReturn)
    {
        memset(pvReturn, 0, nmemb*size);
    }

    return pvReturn;
}
/*-----------------------------------------------------------*/


void *pvPortRealloc( void *pv, size_t size )
{
    void        *pvReturn   = NULL;
    size_t       xBlockSize = 0;
    BlockLink_t *pxLink     = NULL;
    MultiPoolHeaderLink_t *pucHeapPool = (MultiPoolHeaderLink_t *)ucHeap;

    if((pv != NULL) && (size == 0))
    {
        vPortFree(pv);
        return NULL;
    }
    pvReturn = pvPortCalloc( size, 1 );

    if( (pv != NULL) && (pvReturn != NULL) )
    {
        // The memory being freed will have an BlockLink_t structure immediately before it.
        pxLink = mp_get_block_to_free(pv, &(pucHeapPool->xStart), pucHeapPool->xPoolAlignment);

        // Check the block is actually allocated
        configASSERT( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 );
        configASSERT( pxLink->pxNextFreeBlock == NULL );
        configASSERT(pxLink->pxNextAllocatedBlock != NULL);

        // Get Original Block Size
        xBlockSize = (pxLink->xBlockSize & ~xBlockAllocatedBit);

        // Get Original data length
        xBlockSize = (xBlockSize - ((size_t)pxLink - (size_t)pv));

        if(xBlockSize < size)
            memcpy(pvReturn, pv, xBlockSize);
        else
            memcpy(pvReturn, pv, size);

        // Free Original Ptr
        vPortFree(pv);
    }

    return pvReturn;
}

/* heap is default at the cacheable region for higher performance for general users, 
if non-cacheable heap memory is needed, please use below interface, which ended with NC */
void *pvPortMallocNC( size_t xWantedSize )
{
    return pvPortMalloc(xWantedSize);
}
void vPortFreeNC( void *pv )
{
    vPortFree(pv);
}

/* Wrap c stand library malloc family, include malloc/calloc/realloc/free to FreeRTOS heap service */
#if defined(__GNUC__)
void *__wrap_malloc(size_t size)
{
    void* pvReturn;

    #ifdef MTK_SUPPORT_HEAP_DEBUG
    BlockLink_t *pxBlock;
    MultiPoolHeaderLink_t *pucHeapPool = (MultiPoolHeaderLink_t *)ucHeap;
    uint32_t xLinkRegAddr = (uint32_t)__builtin_return_address(0);
    #endif /* MTK_SUPPORT_HEAP_DEBUG */

    pvReturn = pvPortMalloc(size);

    #ifdef MTK_SUPPORT_HEAP_DEBUG
    if(pvReturn != NULL)
    {
        pxBlock = mp_get_block_to_free(pvReturn, &(pucHeapPool->xStart), pucHeapPool->xPoolAlignment);
        if(pxBlock != NULL)
        {
            pxBlock->xMallocLinkRegAddr = xLinkRegAddr;
        }
    }
    #endif /* MTK_SUPPORT_HEAP_DEBUG */

    return pvReturn;
}
void *__wrap_calloc(size_t nmemb, size_t size )
{
    void* pvReturn;

    #ifdef MTK_SUPPORT_HEAP_DEBUG
    BlockLink_t *pxBlock;
    MultiPoolHeaderLink_t *pucHeapPool = (MultiPoolHeaderLink_t *)ucHeap;
    uint32_t xLinkRegAddr = (uint32_t)__builtin_return_address(0);
    #endif /* MTK_SUPPORT_HEAP_DEBUG */

    pvReturn = pvPortCalloc(nmemb,size);

    #ifdef MTK_SUPPORT_HEAP_DEBUG
    if(pvReturn != NULL)
    {
        pxBlock = mp_get_block_to_free(pvReturn, &(pucHeapPool->xStart), pucHeapPool->xPoolAlignment);
        if(pxBlock != NULL)
        {
            pxBlock->xMallocLinkRegAddr = xLinkRegAddr;
        }
    }
    #endif /* MTK_SUPPORT_HEAP_DEBUG */

    return pvReturn;
}
void *__wrap_realloc(void *pv, size_t size )
{
    void* pvReturn;

    #ifdef MTK_SUPPORT_HEAP_DEBUG
    BlockLink_t *pxBlock;
    MultiPoolHeaderLink_t *pucHeapPool = (MultiPoolHeaderLink_t *)ucHeap;
    uint32_t xLinkRegAddr = (uint32_t)__builtin_return_address(0);
    #endif /* MTK_SUPPORT_HEAP_DEBUG */

    pvReturn =  pvPortRealloc(pv,size);

    #ifdef MTK_SUPPORT_HEAP_DEBUG
    if(pvReturn != NULL)
    {
        pxBlock = mp_get_block_to_free(pvReturn, &(pucHeapPool->xStart), pucHeapPool->xPoolAlignment);
        if(pxBlock != NULL)
        {
            pxBlock->xMallocLinkRegAddr = xLinkRegAddr;
        }
    }
    #endif /* MTK_SUPPORT_HEAP_DEBUG */

    return pvReturn;
}
void __wrap_free(void *pv)
{
     vPortFree(pv);
}
#elif defined(__CC_ARM)
void *$Sub$$malloc(size_t size)
{
    return pvPortMalloc(size);
}
void *$Sub$$calloc(size_t nmemb, size_t size )
{
    return pvPortCalloc(nmemb,size);
}
void *$Sub$$realloc(void *pv, size_t size )
{
    return pvPortRealloc(pv,size);
}
void $Sub$$free(void *pv)
{
     vPortFree(pv);
}
#endif /* __GNUC__ */

/* dump heap block info in exception flow */
#if defined(MTK_SUPPORT_HEAP_DEBUG)
#include "exception_handler.h"

void prvPortDumpCallback(void)
{
    mp_dump_status(&ucHeap);
}
void prvPortDumpHeapInfo(void)
{
    uint32_t ret;
    exception_config_type callback_config;

    /* register callback to exception flow */
    callback_config.init_cb = prvPortDumpCallback;
    callback_config.dump_cb = NULL;

    ret = exception_register_callbacks(&callback_config);
    if (ret) {
        configASSERT(0);
    }
}
#endif /* MTK_SUPPORT_HEAP_DEBUG */
/*-----------------------------------------------------------*/

