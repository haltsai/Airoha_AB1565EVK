/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */
#include <string.h>
#include "config.h"
#include "types.h"
////#include "os.h"
//#include "Dlist.h"
#include "dlist.h"
#include "dsp_temp.h"
#include "dsp_memory.h"

#include "preloader_pisplit.h"


////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define DSP_MEMORY_USING_MALLOC     (true)


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DSPMEM DSPMEM_DSP;
DSPMEM DSPMEM_DPRT;
DSPMEM DSPMEM_DAVT;
DSPMEM DSPMEM_DHPT;


#define DSP_DSP_MEMSIZE     0//36000
#define DSP_DPRT_MEMSIZE    0
#define DSP_DAVT_MEMSIZE    0//70000
#define DSP_DHPT_MEMSIZE    0

DSP_MEMORY_STRU DSPMemorySize =
{
    /* DSP_DSP_MEMSize*/            DSP_DSP_MEMSIZE,
    /* DSP_DPRT_MEMSize */            DSP_DPRT_MEMSIZE,
    /* DSP_DAVT_MEMSize */            DSP_DAVT_MEMSIZE,
    /* DSP_DHPT_MEMSize */            DSP_DHPT_MEMSIZE,
};

#if (!DSP_MEMORY_USING_MALLOC)
U8 DSPMemoryBlock_DSP[DSP_DSP_MEMSIZE];
U8 DSPMemoryBlock_DPRT[DSP_DPRT_MEMSIZE];
U8 DSPMemoryBlock_DAVT[DSP_DAVT_MEMSIZE];
U8 DSPMemoryBlock_DHPT[DSP_DHPT_MEMSIZE];
#endif
////////////////////////////////////////////////////////////////////////////////
// DSPMEM FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * DSPMEM_GetPtrAddr
 *
 * Get DPRT/DAVT DSPMEM struct address
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 *
 * @DSPTask : Memory of (DPRT/DAVT)task to allocate
 * @Return : DSPMEM_stru address
 */

DSPMEM_PTR DSPMEM_GetPtrAddr(
          TaskHandle_t  DSPTask)
{
    DSPMEM_PTR pDSPMEM;
    #if 1
    if (DSPTask == DSP_TASK_ID)
    {
        pDSPMEM = &DSPMEM_DSP;
    }
    else
    {
        pDSPMEM = &DSPMEM_DAVT;
    }
    #else
    if (DSPTask == DHP_TASK_ID)
    {
        pDSPMEM = &DSPMEM_DHPT;
    }
    else if (DSPTask == DAV_TASK_ID)
    {
        pDSPMEM = &DSPMEM_DAVT;
    }
    else if (DSPTask == DPR_TASK_ID)
    {
        pDSPMEM = &DSPMEM_DPRT;
    }
    else if (DSPTask == DSP_TASK_ID)
    {
        pDSPMEM = &DSPMEM_DSP;
    }
    else
    {
        configASSERT(FALSE);
    }
    #endif
    return pDSPMEM;
}


/**
 * DSPMEM_Init
 *
 * Get memory from OSHEAP_malloc ,and do DSP Memory Initialization
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 *
 * @DSPTask : Memory of (DPRT/DAVT)task to allocate
 */
VOID DSPMEM_Init(
          TaskHandle_t  DSPTask)
{
    DSPMEM_PTR  pDSPMEM;
    SIZE MemSize;
    uint32_t mask;

    pDSPMEM = DSPMEM_GetPtrAddr(DSPTask);

    if (DSPTask == DSP_TASK_ID)
    {
        MemSize = DSPMemorySize.DSP_DSP_MEMSize;
    }
    else
    {
        MemSize = DSPMemorySize.DSP_DAVT_MEMSize;
    }

    if ((pDSPMEM->init_flag == TRUE))
    {
        return;
    }
    hal_nvic_save_and_set_interrupt_mask(&mask);

    pDSPMEM->init_flag = TRUE;

    /* clean DSPMEM */
    pDSPMEM->noMB           = 0;
    pDSPMEM->seqMB          = 0;
    pDSPMEM->rm_size        = 0;
    dlist_init(&pDSPMEM->head);

    hal_nvic_restore_interrupt_mask(mask);
}

/**
 * DSPMEM_Flush
 *
 * Flush all allocated DSP Memory
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 *
 * @DSPTask : Memory of (DPRT/DAVT)task to allocate
 * @Return : number of allocated memory block
 */

U8 DSPMEM_Flush(
          TaskHandle_t  DSPTask)

{
    DSPMEM_PTR pDSPMEM;
    DSPMEM_BLK_PTR pBlk, pBlkpre;
    U8 count = 0;
    uint32_t mask;
    pDSPMEM = DSPMEM_GetPtrAddr(DSPTask);

    hal_nvic_save_and_set_interrupt_mask(&mask);


    pDSPMEM = DSPMEM_GetPtrAddr(DSPTask);
    pBlkpre = (DSPMEM_BLK_PTR)pDSPMEM;//(DSPMEM_BLK_PTR)(pDSPMEM->head.prev);
    pBlk = (DSPMEM_BLK_PTR)(pBlkpre->header.list.prev);

    while(pBlk != (DSPMEM_BLK_PTR)pDSPMEM)
    {
        count++;
        pDSPMEM->noMB--;
        dlist_remove(&pBlk->header.list);
        preloader_pisplit_free_memory(pBlk);
        //preloader_pisplit_free_memory(pBlk->header.malloc_addr);
        pBlk = (DSPMEM_BLK_PTR)(pBlkpre->header.list.prev);
    }

    hal_nvic_restore_interrupt_mask(mask);
    return count;
}

/**
 * DSPMEM_Free
 *
 * Free allocated DSP Memory
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 *
 * @DSPTask : Memory of (DPRT/DAVT)task to allocate
 * @usingPtr : which pointer do free
 * @Return : number of allocated memory block
 */

U8 DSPMEM_Free(
          TaskHandle_t  DSPTask,
          VOID* usingPtr)

{
    DSPMEM_PTR pDSPMEM;
    DSPMEM_BLK_PTR pBlk, pBlkpre;
    U8 count=0;
    uint32_t mask;

    pDSPMEM = DSPMEM_GetPtrAddr(DSPTask);
    pBlkpre = (DSPMEM_BLK_PTR)pDSPMEM;//(DSPMEM_BLK_PTR)(pDSPMEM->head.prev);
    pBlk = (DSPMEM_BLK_PTR)(pBlkpre->header.list.prev);


    hal_nvic_save_and_set_interrupt_mask(&mask);
    while(pBlk != (DSPMEM_BLK_PTR)pDSPMEM)
    {
        if (pBlk->header.using_ptr == usingPtr)
        {
            count++;
            pDSPMEM->noMB--;
            dlist_remove(&pBlk->header.list);
            memset(pBlk->data_space, 0, pBlk->header.blk_size-DSP_ADDITIONAL_BYTES_FOR_MEMORY_ARRAY);
            preloader_pisplit_free_memory(pBlk);
            //preloader_pisplit_free_memory(pBlk->header.malloc_addr);

        }
        else
        {
            pBlkpre = pBlk;
        }

        pBlk = (DSPMEM_BLK_PTR)(pBlkpre->header.list.prev);
    }
    hal_nvic_restore_interrupt_mask(mask);
    DSP_MW_LOG_I("DSP MEM Free count:%d, RemainMB:%d, 0x%x\r\n", 3, count, pDSPMEM->noMB, (uint32_t *)pDSPMEM);
    return count;
}

uint32_t globa[8] = {0};

/**
 * DSPMEM_tmalloc
 *
 * DSP Task Memory allocation
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 *
 * @DSPTask : Memory of (DPRT/DAVT)task to allocate
 * @Size : Number of bytes memory to allocate
 * @usingPtr : which pointer do allocate
 * @Return : Pointer of allocated memory
 */
#ifdef MTK_SUPPORT_HEAP_DEBUG
#include"preloader_pisplit_internal.h"
#include "multi_pool_heap.h"
#endif
VOID* DSPMEM_tmalloc(
          TaskHandle_t  DSPTask,
          SIZE Size,
          VOID* usingPtr)
{
    DSPMEM_PTR pDSPMEM;
    DSPMEM_BLK_PTR pBlk;
    DSPMEM_BLKTAL_PTR pEOM;
    VOID *pMalloc, *pDATA;
    U32 RealBlkSize;
    uint32_t mask;
#ifdef MTK_SUPPORT_HEAP_DEBUG
    BlockLink_t *pxBlock;
    void *pxStart;
#endif
    pDSPMEM = DSPMEM_GetPtrAddr(DSPTask);
    if (Size == 0)
        return NULL;
    hal_nvic_save_and_set_interrupt_mask(&mask);

    globa[0] = RealBlkSize = (Size + DSP_ADDITIONAL_BYTES_FOR_MEMORY_ARRAY + 7) & (~7UL);
    //globa[1] = pDSPMEM->rm_size;

    configASSERT(usingPtr != NULL);

    pMalloc = (DSPMEM_BLK_PTR) preloader_pisplit_malloc_memory( PRELOADER_D_HIGH_PERFORMANCE , RealBlkSize);
#ifdef MTK_SUPPORT_HEAP_DEBUG
   if(pMalloc!=NULL){
        extern void* find_a_memory_pool_start_by_buffer_address(void *buffer);
        pxStart = find_a_memory_pool_start_by_buffer_address(pMalloc);
        pxBlock = mp_get_block_to_free(pMalloc, &(((MultiPoolHeaderLink_t*)pxStart)->xStart), PRELOADER_MEMORY_ALIGNMENT);
        if(pxBlock != NULL)
        {
            pxBlock->xMallocLinkRegAddr = (uint32_t)usingPtr;
        }
    }
    else
    {
        DSP_MW_LOG_I("DSP MEM malloc fail, user:0x%x,size:%d\r\n", 2, (uint32_t *)usingPtr,Size);
        preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_HIGH_PERFORMANCE);
        preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_LOW_PERFORMANCE);
    }
#endif
    configASSERT(pMalloc!=NULL);

    #if 0
    /*8-byte aligned*/
    U32 align8ptr;
    align8ptr = (((U32)pMalloc)+7) & (~7UL);
    RealBlkSize = RealBlkSize - (align8ptr -((U32)pMalloc));
    pBlk = (DSPMEM_BLK_PTR)align8ptr;
    #else
    pBlk = (DSPMEM_BLK_PTR)pMalloc;
    #endif
    pBlk->header.blk_size = RealBlkSize;
    dlist_init(&pBlk->header.list);
    dlist_append(&pBlk->header.list, &pDSPMEM->head);

    pDSPMEM->noMB++;
    pDSPMEM->seqMB++;
    pBlk->header.blk_seQ     = pDSPMEM->seqMB;
    //pBlk->header.task        = (U8)DSPTask;
    pBlk->header.using_ptr   = usingPtr;
    pDATA                    = pBlk->data_space;

    pEOM          = (DSPMEM_BLKTAL_PTR)((uint32_t)((U8 *)pMalloc) + RealBlkSize - DSP_MEMBLK_EOB_SIZE);
    pEOM->tob_ptr = pBlk;

    memset(pDATA, 0, Size);

    hal_nvic_restore_interrupt_mask(mask);
    DSP_MW_LOG_I("DSP MEM malloc No:%d, seq:%d, user:0x%x, 0x%x\r\n", 4, pDSPMEM->noMB, pDSPMEM->seqMB, (uint32_t *)usingPtr, (uint32_t *)pDATA);
    return pDATA;
}


#ifdef AIR_PROMPT_SOUND_MEMORY_DEDICATE_ENABLE
VOID* DSPMEM_tpremalloc(
          TaskHandle_t  DSPTask,
          SIZE Size,
          VOID* usingPtr)
{
    DSPMEM_PTR pDSPMEM;
    DSPMEM_BLK_PTR pBlk;
    DSPMEM_BLKTAL_PTR pEOM;
    VOID *pMalloc, *pDATA;
    U32 RealBlkSize;
    uint32_t mask;

    pDSPMEM = DSPMEM_GetPtrAddr(DSPTask);
    if (Size == 0)
        return NULL;
    hal_nvic_save_and_set_interrupt_mask(&mask);

    globa[0] = RealBlkSize = (Size + DSP_ADDITIONAL_BYTES_FOR_MEMORY_ARRAY + 7) & (~7UL);
    //globa[1] = pDSPMEM->rm_size;

    DSP_MW_LOG_I("[DSPMEM_tpremalloc]Size = %d", 1, Size);


    pMalloc = (DSPMEM_BLK_PTR) preloader_pisplit_malloc_memory( PRELOADER_D_HIGH_PERFORMANCE , RealBlkSize);
    configASSERT(pMalloc!=NULL);

    #if 0
    /*8-byte aligned*/
    U32 align8ptr;
    align8ptr = (((U32)pMalloc)+7) & (~7UL);
    RealBlkSize = RealBlkSize - (align8ptr -((U32)pMalloc));
    pBlk = (DSPMEM_BLK_PTR)align8ptr;
    #else
    pBlk = (DSPMEM_BLK_PTR)pMalloc;
    #endif
    pBlk->header.blk_size = RealBlkSize;
    dlist_init(&pBlk->header.list);
    dlist_append(&pBlk->header.list, &pDSPMEM->head);

    pDSPMEM->noMB++;
    pDSPMEM->seqMB++;
    pBlk->header.blk_seQ     = pDSPMEM->seqMB;
    //pBlk->header.task        = (U8)DSPTask;
    pBlk->header.using_ptr   = usingPtr;
    pDATA                    = pBlk->data_space;

    pEOM          = (DSPMEM_BLKTAL_PTR)((uint32_t)((U8 *)pMalloc) + RealBlkSize - DSP_MEMBLK_EOB_SIZE);
    pEOM->tob_ptr = pBlk;

    memset(pDATA, 0, Size);

    hal_nvic_restore_interrupt_mask(mask);
    DSP_MW_LOG_I("DSP MEM malloc No:%d, seq:%d, user:0x%x, 0x%x\r\n", 4, pDSPMEM->noMB, pDSPMEM->seqMB, (uint32_t *)usingPtr, (uint32_t *)pDATA);
    return pDATA;
}
#endif


/**
 * DSPMEM_Flush
 *
 * Check DPRT/DAVT memory
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 *
 * @DSPTask : Memory of (DPRT/DAVT)task to allocate
 * @Return : check result
 */

U8 DSPMEM_Check(
          TaskHandle_t  DSPTask)
{
    #if 0
    DSPMEM_PTR pDSPMEM;
    DSPMEM_BLK_PTR pBlk;
    U16 noMB;
    U16 BlkSize;


    pDSPMEM = DSPMEM_GetPtrAddr(DSPTask);

    noMB = pDSPMEM->noMB;
    pBlk = (DSPMEM_BLK_PTR)pDSPMEM->head.next;

    while(noMB)
    {
        DSPMEM_BLKTAL_PTR pEOM;
        BlkSize = pBlk->header.blk_size;
        pEOM = (DSPMEM_BLKTAL_PTR)((U8 *)pBlk+BlkSize-DSP_MEMBLK_EOB_SIZE);


        configASSERT(pBlk == pEOM->tob_ptr);
        configASSERT(pBlk->header.list.next == (struct dlist_stru_t *)((U8 *)pBlk+BlkSize));

        pBlk =(DSPMEM_BLK_PTR)((U8 *)pBlk+BlkSize);
        noMB--;
    }
    return noMB;
    #else
    UNUSED(DSPTask);
    return 0;
    #endif
}


/**
 * DSPMEM_Remain
 *
 * Check remaining DPRT/DAVT memory
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 *
 * @DSPTask : Memory of (DPRT/DAVT)task to allocate
 * @Return : remaining memory
 */

U16 DSPMEM_Remain(
          TaskHandle_t   DSPTask)
{
    DSPMEM_PTR pDSPMEM;
    pDSPMEM = DSPMEM_GetPtrAddr(DSPTask);

    return pDSPMEM->rm_size;
}

