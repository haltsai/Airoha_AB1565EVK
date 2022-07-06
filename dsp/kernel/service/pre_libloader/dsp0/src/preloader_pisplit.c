/*
 * Copyright (c) 2012-2013 by Tensilica Inc. ALL RIGHTS RESERVED.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include <stdlib.h>
#include <stdio.h>
#include "elf.h"

#ifdef __XTENSA__
#include <xtensa/hal.h>  /* xthal_memcpy */
#endif
#include <string.h>
#include "hal.h"
#include "assert.h"
#include"preloader_pisplit.h"
#include"preloader_pisplit_configure.h"
#include"preloader_pisplit_log.h"
#include "multi_pool_heap.h"
#include"preloader_pisplit_internal.h"
#include "xt_library_loader.h"
#include "loader_internal.h"
#include "hal_nvic.h"
#include "hal_clock_internal.h"
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
#include "ccci.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif




xtlib_packaged_library * g_p_customer_pre_load_library[]= PIC_LIB_LIST;

pisplit_library_info_t *g_preloader_pisplit_library_info;
unsigned int g_total_pic_lib;

typedef struct {
    uint32_t total_pool_number;
    uint32_t pool_init_index;
    preloader_pisplit_pool_info_t p_pool_info[PRELOADER_PISPLIT_SUPPORT_MAX_MULTI_POOLS];
} preloader_pisplit_total_memory_info_t;

preloader_pisplit_total_memory_info_t preloader_pisplit_total_memory;
static unsigned int preloader_tcb_init = 0;

#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
bool is_dsp1_memory(uint32_t addr)
{
    if( (addr >= 0x60000000) && (addr < 0x6002ffff) )
        return true;
    else
        return false;
}
bool is_dsp1_memory_type(preloader_pisplit_memory_type_t memory_type)
{
    if( (memory_type == PRELOADER_EXT_IRAM) || (memory_type == PRELOADER_EXT_DRAM) )
        return true;
    else
        return false;
}
volatile int preloader_dsp0_lock_dsp1_sleep_and_dcm_counter = 0;
void preloader_dsp0_lock_dsp1_sleep_and_dcm()
{
    // HW condition: When DSP1 Core idle and DSP1 I/DRAM idle and external memory idle etc. will enter DCM
    // HW design: on DSP0 side, have 4 bit, can control the 4 Core whether enabel DCM mode.
    // this function will lock DSP1 sleep and DCM
#ifdef HAL_SLEEP_MANAGER_LOCK_SLEEP_CROSS_CORE
    preloader_dsp0_lock_dsp1_sleep_and_dcm_counter ++;
    PRELOADER_LOG_I(preloader, "preloader_pisplit lock DSP1 sleep and dcm, counter:%d\r\n",1,preloader_dsp0_lock_dsp1_sleep_and_dcm_counter);
    if(preloader_dsp0_lock_dsp1_sleep_and_dcm_counter == 1)
    {
        if((hal_core_status_read(HAL_CORE_DSP1) == HAL_CORE_OFF) || (hal_core_status_read(HAL_CORE_DSP1) == HAL_CORE_ERROR) )
        {
            PRELOADER_LOG_E(preloader, "preloader_pisplit lock DSP1 sleep and dcm ERROR, DSP1 not power on!!!\r\n",0);
        }
        hal_sleep_manager_lock_sleep(SLEEP_LOCK_DSP1_CROSS_CORE_PRELOADER);
    }
#else
    assert(0);
#endif
}

void preloader_dsp0_unlock_dsp1_sleep_and_dcm()
{
    // HW condition: When DSP1 Core idle and DSP1 I/DRAM idle and external memory idle etc. will enter DCM
    // HW design: on DSP0 side, have 4 bit, can control the 4 Core whether enabel DCM mode.
    // this function will lock DSP1 sleep and DCM
#ifdef HAL_SLEEP_MANAGER_LOCK_SLEEP_CROSS_CORE
    preloader_dsp0_lock_dsp1_sleep_and_dcm_counter --;
     PRELOADER_LOG_I(preloader, "preloader_pisplit unlock DSP1 sleep and dcm, counter:%d\r\n",1,preloader_dsp0_lock_dsp1_sleep_and_dcm_counter);
    if(preloader_dsp0_lock_dsp1_sleep_and_dcm_counter == 0)
    {
        if((hal_core_status_read(HAL_CORE_DSP1) == HAL_CORE_OFF) || (hal_core_status_read(HAL_CORE_DSP1) == HAL_CORE_ERROR) )
        {
            PRELOADER_LOG_E(preloader, "preloader_pisplit lock DSP1 sleep and dcm ERROR, DSP1 not power on!!!\r\n",0);
        }
        hal_sleep_manager_unlock_sleep(SLEEP_LOCK_DSP1_CROSS_CORE_PRELOADER);
    }
    else if(preloader_dsp0_lock_dsp1_sleep_and_dcm_counter < 0)
    {
        PRELOADER_LOG_E(preloader, "preloader_pisplit lock DSP1 sleep and dcm ERROR,counter < 0!!!\r\n",0);
        assert(0);
    }
#else
    assert(0);
#endif
}
#endif
// this function should be called one time when DSP init flow(before any dynamic load lib)
preloader_pisplit_error_handling_t preloader_pisplit_init(void)
{
    unsigned int i;
    pisplit_library_info_t *p_temp;
    unsigned int err = PRELOADER_PISPLIT_XTLIB_NO_ERR;
    unsigned int pre_load_tcb_size;

        g_total_pic_lib = sizeof(g_p_customer_pre_load_library) / 4;
        PRELOADER_LOG_I(preloader, "preloader_pisplit_init- total llb:%d\r\n",1,g_total_pic_lib);

        pre_load_tcb_size = sizeof(pisplit_library_info_t) * g_total_pic_lib;
        //Remind!!! for DSP0 load DSP1 PIC library, the memory of TCB must be non-cacheable region for DSP0!!!
        //because under this feature, the TCB memory also will be read by DSP1 side!!!
        g_preloader_pisplit_library_info =(pisplit_library_info_t *) malloc(pre_load_tcb_size);
        if(g_preloader_pisplit_library_info == NULL)
        {
            PRELOADER_LOG_E(preloader, "preloader_pisplit_init-error!!! Allocate preloader control black fail!!!\r\n",0);
            //assert(0);
            return PRELOADER_PISPLIT_ERROR;
        }
        PRELOADER_LOG_I(preloader, "preloader_pisplit_init: Allocate preloader TCB success: addr: 0x%x   len:%d !!!\r\n",2,(unsigned int)g_preloader_pisplit_library_info,pre_load_tcb_size);
        for(i=0;i<g_total_pic_lib;i++)
        {
          p_temp = &g_preloader_pisplit_library_info[i];
          p_temp->p_pi_library = (xtlib_packaged_library *)g_p_customer_pre_load_library[i];
          p_temp->state_machine = PRELOADER_PISPLIT_STATUS_DEFAULT;
        
          //init for DMA tcb
          p_temp->code_mem_copy_request.p_current_handle = p_temp;
          p_temp->code_mem_set_request.p_current_handle = p_temp;
          p_temp->data_mem_copy_request.p_current_handle = p_temp;
          p_temp->data_mem_set_request.p_current_handle = p_temp;
        
          if(p_temp->p_pi_library == NULL)
          {
              return PRELOADER_PISPLIT_ERROR_STATIC_LIB_INFO;
          }
          err = xtlib_split_pi_library_size (p_temp->p_pi_library, &p_temp->code_bytes, &p_temp->data_bytes);
          PRELOADER_LOG_I(preloader, "preloader_pisplit_init: g_preloader_pisplit_library_info[%d]is 0x%x, it's code size:%d,data size:%d\r\n",4,i,(unsigned int)p_temp->p_pi_library,p_temp->code_bytes,p_temp->data_bytes);
        
          if (err != PRELOADER_PISPLIT_XTLIB_NO_ERR)
          {
              p_temp->code_bytes = 0xffffffff;
              p_temp->data_bytes = 0xffffffff;
              PRELOADER_LOG_E(preloader, "preloader_pisplit_init-error!!! The lib of g_preloader_pisplit_library_info[ %d] failed with error: %d\r\n",2, i,err);
              p_temp->state_machine = PRELOADER_PISPLIT_STATUS_ERROR;
          }
          else
          {
              p_temp->state_machine = PRELOADER_PISPLIT_STATUS_INIT_DONE;
          }
        }
        preloader_pisplit_total_memory.total_pool_number = PRELOADER_PISPLIT_SUPPORT_MAX_MULTI_POOLS;
        preloader_pisplit_total_memory.pool_init_index = 0;
        PRELOADER_LOG_I(preloader, "preloader_pisplit_init: there have total support max %d memory pool !!!\r\n",1,preloader_pisplit_total_memory.total_pool_number);

        preloader_tcb_init = 1;//preloader tcb init done!!!
    return (preloader_pisplit_error_handling_t)err;

}


preloader_pisplit_error_handling_t preloader_pisplit_add_pools(preloader_pisplit_pool_info_t preloader_pisplit_pools[],uint32_t pools_number)
{
    unsigned int err = PRELOADER_PISPLIT_XTLIB_NO_ERR;
    unsigned int i,mask,max_free_size;
    unsigned int init_index;
    multi_pool_region_t multi_pool_temp[2] =
    {
        { NULL, 0, 0},
        { NULL, 0, 0} //<< Terminates the array.
    };
    if(preloader_pisplit_total_memory.pool_init_index + pools_number > preloader_pisplit_total_memory.total_pool_number)
    {
        PRELOADER_LOG_E(preloader, "preloader_pisplit_add_pools-error!!! memory pool had been init done!the max support pools:%d,had been init:%d,now want to add:%d\r\n",3,preloader_pisplit_total_memory.total_pool_number,preloader_pisplit_total_memory.pool_init_index,pools_number);
        return PRELOADER_PISPLIT_ERROR_USER_CALLFLOW;
    }


    hal_nvic_save_and_set_interrupt_mask(&mask);
    init_index = preloader_pisplit_total_memory.pool_init_index;
    preloader_pisplit_total_memory.pool_init_index +=pools_number;
    hal_nvic_restore_interrupt_mask(mask);


    for(i=0; i<pools_number;i++,init_index++)
    {
        preloader_pisplit_total_memory.p_pool_info[init_index].pool_start = preloader_pisplit_pools[i].pool_start;
        preloader_pisplit_total_memory.p_pool_info[init_index].pool_end = preloader_pisplit_pools[i].pool_end;
        preloader_pisplit_total_memory.p_pool_info[init_index].pool_type = preloader_pisplit_pools[i].pool_type;

        multi_pool_temp[0].mp_start_address = preloader_pisplit_pools[i].pool_start;
        multi_pool_temp[0].mp_size_in_bytes = (unsigned int)preloader_pisplit_pools[i].pool_end - (unsigned int )preloader_pisplit_pools[i].pool_start;
        multi_pool_temp[0].mp_aligned_rule = PRELOADER_MEMORY_ALIGNMENT;
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
        if( is_dsp1_memory(multi_pool_temp[0].mp_start_address) )
        {
            preloader_dsp0_lock_dsp1_sleep_and_dcm();
        }
#endif
        mp_init(multi_pool_temp);
        max_free_size = mp_get_max_free_block_size(multi_pool_temp[0].mp_start_address);
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
        if( is_dsp1_memory(multi_pool_temp[0].mp_start_address) )
        {
            preloader_dsp0_unlock_dsp1_sleep_and_dcm();
        }
#endif
        PRELOADER_LOG_I(preloader, "preloader_pisplit_add_pools init the pool:0x%x done,the max free size:%d,type:0x%x\r\n",3,multi_pool_temp[0].mp_start_address,max_free_size,preloader_pisplit_pools[i].pool_type);
    }


    PRELOADER_LOG_I(preloader, "preloader_pisplit_add_pools:this time new add %d pools, there are total %d pool init done, total pools is:%d\r\n",3,pools_number,preloader_pisplit_total_memory.pool_init_index,preloader_pisplit_total_memory.total_pool_number);

    return (preloader_pisplit_error_handling_t)err;


}

preloader_pisplit_error_handling_t preloader_pisplit_get_handle(preloader_pisplit_handle_t *handle,xtlib_packaged_library * library,preloader_pisplit_customer_callback_t complete_callback_func)
{
    unsigned int i;
    pisplit_library_info_t *p_temp;
    unsigned int mask;
    pisplit_library_info_t **p_handle = (pisplit_library_info_t **)handle;

    PRELOADER_LOG_I(preloader, "preloader_pisplit_get_handle library(0x%x) get handle!\r\n", 1,(unsigned int)library);
    if(complete_callback_func == NULL)
    {
        PRELOADER_LOG_E(preloader, "preloader_pisplit_get_handle-error!!! library(0x%x) the customer callback is NULL!!!\r\n",1,(unsigned int)library);
        return PRELOADER_PISPLIT_ERROR_CUSTOMER_CALLBACK;
    }

    for(i=0;i<g_total_pic_lib;i++)//query the table to find & get handle and register callback
    {
        p_temp = &g_preloader_pisplit_library_info[i];
        if(p_temp->p_pi_library == NULL)
        {
            PRELOADER_LOG_D(preloader, "preloader_pisplit_get_handle-error!!!(all) The lib of g_preloader_pisplit_library_info[ %d] have NULL point!!!\r\n", 1,i);
            return PRELOADER_PISPLIT_ERROR_STATIC_LIB_INFO;
        }
        if(p_temp->p_pi_library == library)
        {
           PRELOADER_LOG_D(preloader, "preloader_pisplit_get_handle library(0x%x) Find a match lib: 0x%x\r\n", 2,(unsigned int)p_temp->p_pi_library,(unsigned int)library);
            *p_handle = p_temp;

            hal_nvic_save_and_set_interrupt_mask(&mask);
            if( ((*p_handle)->state_machine != PRELOADER_PISPLIT_STATUS_INIT_DONE) && ((*p_handle)->state_machine != PRELOADER_PISPLIT_STATUS_HANDLE_RELEASE) )
            {
                hal_nvic_restore_interrupt_mask(mask);
                PRELOADER_LOG_E(preloader, "preloader_pisplit_get_handle-error!!! library(0x%x) p_handle->state_machine = %d!!! not PRELOADER_PISPLIT_STATUS_INIT_DONE(1) or PRELOADER_PISPLIT_STATUS_HANDLE_RELEASE(12)\r\n", 2,(unsigned int)p_temp->p_pi_library,(*p_handle)->state_machine);
                return PRELOADER_PISPLIT_ERROR_USER_CALLFLOW;
            }

            (*p_handle)->state_machine = PRELOADER_PISPLIT_STATUS_GET_HANDLE_DONE;
            hal_nvic_restore_interrupt_mask(mask);
            (*p_handle)->callback_func = complete_callback_func;
            PRELOADER_LOG_D(preloader, "preloader_pisplit_get_handle library(0x%x) Get handle success!!! handle:0x%x   *handle:0x%x\r\n",3,(unsigned int)p_temp->p_pi_library,(unsigned int)p_handle,((unsigned int)(*p_handle)));
            PRELOADER_LOG_I(preloader, "preloader_pisplit_get_handle library(0x%x) get handle success! g_preloader_pisplit_library_info[%d] is the library of 0x%x\r\n", 3,(unsigned int)p_temp->p_pi_library,i,(unsigned int)library);

            return PRELOADER_PISPLIT_XTLIB_NO_ERR;
        }
    }
    *p_handle = NULL;
    PRELOADER_LOG_E(preloader, "preloader_pisplit_get_handle-error!!!  library(0x%x) not find a match lib, get handle fail!!!\r\n",1,(unsigned int)library);
    return PRELOADER_PISPLIT_ERROR_GET_HANDLE;
}

#if 0 //NOT_SUPPORT_MULTI_LOAD_AT_SAME_TIME
void pisplit_restart_to_load(pisplit_library_info_t *p_handle);
#endif
preloader_pisplit_error_handling_t xtlib_load_split_pi_library_common_mtk_fn (pisplit_library_info_t *p_handle);

typedef struct {
    void *pool_start;
    void *buffer;
} preloader_alloc_info_t;

void find_and_alloc_a_memory_pool(preloader_alloc_info_t *p_info, preloader_pisplit_memory_type_t memory_type,uint32_t size)
{
    unsigned int i;
    preloader_pisplit_pool_info_t *temp_pool_info;
    uint32_t max_size;
    uint32_t total_free_size,ever_min_free_size;
    void *start=NULL;
    p_info->buffer = NULL;
    p_info->pool_start = NULL;
    for(i=0;i<preloader_pisplit_total_memory.pool_init_index;i++)
    {
        temp_pool_info = &preloader_pisplit_total_memory.p_pool_info[i];
        if((temp_pool_info->pool_type & memory_type) != 0)
        {            
            start = mp_malloc(temp_pool_info->pool_start,size);
            if(start != NULL)
            {
                p_info->pool_start = temp_pool_info->pool_start;
                p_info->buffer = start;
                max_size = mp_get_max_free_block_size(temp_pool_info->pool_start);
                total_free_size = mp_get_free_heap_size(temp_pool_info->pool_start);
                ever_min_free_size = mp_get_minimum_ever_free_heap_size(temp_pool_info->pool_start);
                PRELOADER_LOG_I(preloader, "find_and_alloc_a_memory_pool: success to alloc a memory type of 0x%x size:%d,the memory pools index:%d,the memory pool addr:0x%x\r\n",4,temp_pool_info->pool_type,size,i,temp_pool_info->pool_start);
                PRELOADER_LOG_I(preloader, "find_and_alloc_a_memory_pool: successful!!! the max free block size: 0x%x, the total free size: 0x%x, the ever min free size: 0x%x \r\n",3,max_size,total_free_size,ever_min_free_size);
                return ;
            }
            else
            {
                max_size = mp_get_max_free_block_size(temp_pool_info->pool_start);
                total_free_size = mp_get_free_heap_size(temp_pool_info->pool_start);
                ever_min_free_size = mp_get_minimum_ever_free_heap_size(temp_pool_info->pool_start);
                PRELOADER_LOG_E(preloader, "find_and_alloc_a_memory_pool: want to alloc a memory type of 0x%x size:%d,find a the memory pools index:%d,the memory pool addr:0x%x \r\n",4,temp_pool_info->pool_type,size,i,temp_pool_info->pool_start);
                PRELOADER_LOG_E(preloader,"find_and_alloc_a_memory_pool:fail!!! but it just have max free block size: 0x%x, the total free size: 0x%x, the ever min free size: 0x%x \r\n",3,max_size,total_free_size,ever_min_free_size);
            }
        }
    }
    PRELOADER_LOG_D(preloader, "find_and_alloc_a_memory_pool: fail!!!search all of matched pool and alloc fail!!!--need mem type:0x%x,size:%x\r\n",2,temp_pool_info->pool_type,size);

    return ;
}
void* find_a_memory_pool_start_by_buffer_address(void *buffer)
{
    unsigned int i;
    preloader_pisplit_pool_info_t *temp_pool_info;

    for(i=0;i<preloader_pisplit_total_memory.pool_init_index;i++)
    {
        temp_pool_info = &preloader_pisplit_total_memory.p_pool_info[i];
        if(((uint32_t)temp_pool_info->pool_start < (uint32_t)buffer) && ((uint32_t)temp_pool_info->pool_end > (uint32_t)buffer) )
            return temp_pool_info->pool_start;
    }
    return NULL;
}


typedef struct {
    void *pool_start;
    uint32_t max_free_size;
} preloader_pool_free_size_info_t;
void find_max_free_size(preloader_pool_free_size_info_t *p_info,preloader_pisplit_memory_type_t memory_type)
{
    unsigned int i;
    preloader_pisplit_pool_info_t *temp_pool_info;
    uint32_t temp_max_size = 0;
    p_info->pool_start = NULL;
    p_info->max_free_size = 0x0;
    for(i=0;i<preloader_pisplit_total_memory.pool_init_index;i++)
    {
        temp_pool_info = &preloader_pisplit_total_memory.p_pool_info[i];
        if((temp_pool_info->pool_type & memory_type) != 0)
        {            
            temp_max_size = mp_get_max_free_block_size(temp_pool_info->pool_start);
            if(temp_max_size>p_info->max_free_size)
            {
                p_info->max_free_size = temp_max_size;
                p_info->pool_start = temp_pool_info->pool_start;
            }
        }
    }
    return;
}


preloader_pisplit_error_handling_t preloader_pisplit_load(preloader_pisplit_handle_t handle,preloader_pisplit_memory_type_t code_memory_type,preloader_pisplit_memory_type_t data_memory_type)
{
#if 0 //NOT_SUPPORT_MULTI_LOAD_AT_SAME_TIME
    pisplit_library_info_t *p_temp;
#endif
#ifdef MTK_SUPPORT_HEAP_DEBUG  
    BlockLink_t *pxBlock;
#endif
    unsigned int mask;
    pisplit_library_info_t *p_handle = (pisplit_library_info_t *)handle;
    //preloader_pisplit_pool_info_t *p_code_pool_temp,*p_data_pool_temp;
    preloader_alloc_info_t alloc_info;

    if( p_handle == NULL )
    {
        PRELOADER_LOG_E(preloader, "preloader_pisplit_load -error!!! library lib handle is NULL!!!\r\n",0);
        return PRELOADER_PISPLIT_ERROR_HANDLE;
    }
    PRELOADER_LOG_I(preloader, "preloader_pisplit_load library(0x%x) ###debug###important info## start to Load the library",1,(unsigned int)p_handle->p_pi_library);

    hal_nvic_save_and_set_interrupt_mask(&mask);
    if( (p_handle->state_machine != PRELOADER_PISPLIT_STATUS_GET_HANDLE_DONE) && (p_handle->state_machine != PRELOADER_PISPLIT_STATUS_UNLOAD_DONE) )
    {
        hal_nvic_restore_interrupt_mask(mask);
        PRELOADER_LOG_E(preloader, "preloader_pisplit_load library(0x%x)-error!!! p_handle->state_machine=%d,not PRELOADER_PISPLIT_STATUS_GET_HANDLE_DONE or PRELOADER_PISPLIT_STATUS_UNLOAD_DONE\r\n",2,(unsigned int)p_handle->p_pi_library,p_handle->state_machine);
        return PRELOADER_PISPLIT_ERROR_USER_CALLFLOW;
    }
    p_handle->state_machine = PRELOADER_PISPLIT_STATUS_TRY_TO_LOAD;
    hal_nvic_restore_interrupt_mask(mask);
    PRELOADER_LOG_D(preloader, "preloader_pisplit_load library(0x%x)-state_machine: change state_machine to PRELOADER_PISPLIT_STATUS_TRY_TO_LOAD\r\n",1,(unsigned int)p_handle->p_pi_library);

#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
    if((is_dsp1_memory_type(code_memory_type) == true) || (is_dsp1_memory_type(data_memory_type) == true) )
    {
        preloader_dsp0_lock_dsp1_sleep_and_dcm();
    }
#endif
    alloc_info.buffer = NULL;
    alloc_info.pool_start = NULL;
    find_and_alloc_a_memory_pool(&alloc_info,code_memory_type,p_handle->code_bytes+PRELOADER_MEMORY_ALIGNMENT);
    if( (alloc_info.buffer == NULL) || (alloc_info.pool_start == NULL) )
    {
        PRELOADER_LOG_E(preloader, "preloader_pisplit_load library(0x%x) alloc code memory pool fail\r\n",4,(unsigned int)p_handle->p_pi_library);
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
        if((is_dsp1_memory_type(code_memory_type) == true) || (is_dsp1_memory_type(data_memory_type) == true) )
        {
            preloader_dsp0_unlock_dsp1_sleep_and_dcm();
        }
#endif
        return PRELOADER_PISPLIT_ERROR_ALLOC_MEM_FAIL;
    }
    else
    {  
#ifdef MTK_SUPPORT_HEAP_DEBUG
        pxBlock = mp_get_block_to_free(alloc_info.buffer, &(((MultiPoolHeaderLink_t*)alloc_info.pool_start)->xStart), ((MultiPoolHeaderLink_t*)alloc_info.pool_start)->xPoolAlignment);
        if(pxBlock != NULL)
        {
            pxBlock->xMallocLinkRegAddr = (uint32_t)__builtin_return_address(0);
        }
#endif
        p_handle->code_memory_heap = alloc_info.pool_start;
        p_handle->code_dest_memory = alloc_info.buffer;
        PRELOADER_LOG_I(preloader, "preloader_pisplit_load library(0x%x) allocate code memory(pool:0x%x) success, addr:0x%x, len:%d\r\n",4,(unsigned int)p_handle->p_pi_library,p_handle->code_memory_heap,(unsigned int)p_handle->code_dest_memory,p_handle->code_bytes+PRELOADER_MEMORY_ALIGNMENT);
    }

    alloc_info.buffer = NULL;
    alloc_info.pool_start = NULL;
    find_and_alloc_a_memory_pool(&alloc_info,data_memory_type,p_handle->data_bytes+PRELOADER_MEMORY_ALIGNMENT);
    if( (alloc_info.buffer == NULL) || (alloc_info.pool_start == NULL) )
    {
        PRELOADER_LOG_E(preloader, "preloader_pisplit_load library(0x%x) alloc data memory pool fail\r\n",4,(unsigned int)p_handle->p_pi_library);
        mp_free (p_handle->code_memory_heap,p_handle->code_dest_memory);
        PRELOADER_LOG_E(preloader, "preloader_pisplit_load library(0x%x) because alloc data memory pool fail,now free the code memory\r\n",4,(unsigned int)p_handle->p_pi_library);
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
        if((is_dsp1_memory_type(code_memory_type) == true) || (is_dsp1_memory_type(data_memory_type) == true) )
        {
            preloader_dsp0_unlock_dsp1_sleep_and_dcm();
        }
#endif
        return PRELOADER_PISPLIT_ERROR_ALLOC_MEM_FAIL;
    }
    else
    {   
#ifdef MTK_SUPPORT_HEAP_DEBUG
        pxBlock = mp_get_block_to_free(alloc_info.buffer, &(((MultiPoolHeaderLink_t*)alloc_info.pool_start)->xStart), ((MultiPoolHeaderLink_t*)alloc_info.pool_start)->xPoolAlignment);
        if(pxBlock != NULL)
        {
            pxBlock->xMallocLinkRegAddr = (uint32_t)__builtin_return_address(0);
        }
#endif
        p_handle->data_memory_heap = alloc_info.pool_start;
        p_handle->data_dest_memory = alloc_info.buffer;
        PRELOADER_LOG_I(preloader, "preloader_pisplit_load library(0x%x) allocate data memory(pool:0x%x) success, addr:0x%x, len:%d\r\n",4,(unsigned int)p_handle->p_pi_library,p_handle->data_memory_heap,(unsigned int)p_handle->data_dest_memory,p_handle->data_bytes+PRELOADER_MEMORY_ALIGNMENT);
    }

    p_handle->state_machine = PRELOADER_PISPLIT_STATUS_ALLOC_MEMORY_DONE;//memory allocate done
    PRELOADER_LOG_D(preloader, "preloader_pisplit_load library(0x%x) Loading position independent library %d-byte code at 0x%x and %d-byte data at 0x%x...",5,(unsigned int)p_handle->p_pi_library, p_handle->code_bytes, (unsigned int)p_handle->code_dest_memory, p_handle->data_bytes, (unsigned int)p_handle->data_dest_memory);

    p_handle->state_machine = PRELOADER_PISPLIT_STATUS_START_LOADING;

    PRELOADER_LOG_D(preloader, "preloader_pisplit_load library(0x%x)-state_machine: change state_machine to PRELOADER_PISPLIT_STATUS_START_LOADING\r\n",1,(unsigned int)p_handle->p_pi_library);
    xtlib_load_split_pi_library_common_mtk_fn(p_handle);
    PRELOADER_LOG_I(preloader, "preloader_pisplit_load library(0x%x)###debug###important info## loading the library!",1,(unsigned int)p_handle->p_pi_library);

    //when DMA transfer done, on callback to do DCM enable
//#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
//        if((is_dsp1_memory_type(code_memory_type) == true) || (is_dsp1_memory_type(data_memory_type) == true) )
//        {
//            preloader_dsp0_unlock_dsp1_sleep_and_dcm();
//        }
//#endif
    return PRELOADER_PISPLIT_XTLIB_NO_ERR;
}

preloader_pisplit_error_handling_t preloader_pisplit_unload(preloader_pisplit_handle_t handle)
{
    unsigned int mask;
    pisplit_library_info_t *p_handle = (pisplit_library_info_t *)handle;

    if( p_handle == NULL )
    {
        PRELOADER_LOG_E(preloader, "preloader_pisplit_unload-error!!!preloader unloading library error!!! pre load lib handle is NULL!!!\r\n",0);
        return PRELOADER_PISPLIT_ERROR_HANDLE;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    if( p_handle->state_machine != PRELOADER_PISPLIT_STATUS_LOAD_DONE )
    {
        hal_nvic_restore_interrupt_mask(mask);
        PRELOADER_LOG_E(preloader, "preloader_pisplit_unload-error!!! preloader library(0x%x) unloading library error!!! User call flow error!!! state_machine:%d\r\n",2,(unsigned int)p_handle->p_pi_library,p_handle->state_machine);
        return PRELOADER_PISPLIT_ERROR_USER_CALLFLOW;
    }
    p_handle->state_machine = PRELOADER_PISPLIT_STATUS_TRY_TO_UNLOAD;
    hal_nvic_restore_interrupt_mask(mask);

#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
    if((is_dsp1_memory((uint32_t)p_handle->code_memory_heap)==true) ||(is_dsp1_memory((uint32_t)p_handle->data_memory_heap) == true))
    {
        //[Importance start]: disable DCM void access memory fail/////////
        preloader_dsp0_lock_dsp1_sleep_and_dcm();
    }
#endif
    /* unloading library */
#if defined(PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1)
// <finit> is for destruct function, if User no use it, then can bypass this
// if User use it, should do <finit> on DSP1 side, this is a TBD addvance feature
    if( ((uint32_t)p_handle->data_memory_heap > 0x60000000) && ((uint32_t)p_handle->data_memory_heap < 0x60030000))
    {
        PRELOADER_LOG_I(preloader, "preloader_pisplit_unload library(0x%x) unload lib: 0x%x, code memory pool:0x%x,data memory pool:0x%x, no need to do anything, because this is dsp1 lib",4,(unsigned int)p_handle->p_pi_library,(unsigned int)(&p_handle->lib_info),p_handle->code_memory_heap,p_handle->data_memory_heap);
    }
    else
    {
        PRELOADER_LOG_I(preloader, "preloader_pisplit_unload library(0x%x) unload lib: 0x%x, code memory pool:0x%x,data memory pool:0x%x",4,(unsigned int)p_handle->p_pi_library,(unsigned int)(&p_handle->lib_info),p_handle->code_memory_heap,p_handle->data_memory_heap);
        xtlib_unload_pi_library(&p_handle->lib_info);
    }
#else
    PRELOADER_LOG_I(preloader, "preloader_pisplit_unload library(0x%x) unload lib: 0x%x, code memory pool:0x%x,data memory pool:0x%x",4,(unsigned int)p_handle->p_pi_library,(unsigned int)(&p_handle->lib_info),p_handle->code_memory_heap,p_handle->data_memory_heap);
    xtlib_unload_pi_library(&p_handle->lib_info);
#endif
    PRELOADER_LOG_I(preloader, "preloader_pisplit_unload library(0x%x) free memory:0x%x",2,(unsigned int)p_handle->p_pi_library,p_handle->code_dest_memory);
        #if 0 //PIC_LIB_TESTCASE
        {
            uint32_t i;
            for(i=0;i<p_handle->code_bytes;i+=4)
            {
                *(volatile uint32_t*)((uint32_t)p_handle->code_dest_memory + i) = 0xffffffff;
            }
            PRELOADER_LOG_I(preloader,"For Testcase-reset the free memory:0x%x,len:%d",2,p_handle->code_dest_memory,p_handle->code_bytes);
        }
        #endif
    mp_free (p_handle->code_memory_heap,p_handle->code_dest_memory);
    PRELOADER_LOG_I(preloader, "preloader_pisplit_unload library(0x%x) free memory:0x%x",2,(unsigned int)p_handle->p_pi_library,p_handle->data_dest_memory);
        #if 0 //PIC_LIB_TESTCASE
        {
            uint32_t i;
            for(i=0;i<p_handle->data_bytes;i+=4)
            {
                *(volatile uint32_t*)((uint32_t)p_handle->data_dest_memory + i) = 0xffffffff;
            }
            PRELOADER_LOG_I(preloader,"For Testcase-reset the free memory:0x%x,len:%d",2,p_handle->data_dest_memory,p_handle->data_bytes);
        }
        #endif
    mp_free (p_handle->data_memory_heap,p_handle->data_dest_memory);
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
    if((is_dsp1_memory((uint32_t)p_handle->code_memory_heap)==true) ||(is_dsp1_memory((uint32_t)p_handle->data_memory_heap) == true))
    {
        preloader_dsp0_unlock_dsp1_sleep_and_dcm();
    }
#endif
    //p_handle->state_machine = PRELOADER_PISPLIT_STATUS_MEMORY_FREE;
    p_handle->state_machine = PRELOADER_PISPLIT_STATUS_UNLOAD_DONE;
    PRELOADER_LOG_I(preloader, "preloader_pisplit_unload library(0x%x)###debug###important info## un-Load the library!",1,(unsigned int)p_handle->p_pi_library);
    return PRELOADER_PISPLIT_XTLIB_NO_ERR;
}

preloader_pisplit_error_handling_t preloader_pisplit_release_handle(preloader_pisplit_handle_t handle)
{
    pisplit_library_info_t *p_handle = (pisplit_library_info_t *)handle;

    if( p_handle == NULL )
    {
        PRELOADER_LOG_E(preloader, "preloader_pisplit_release_handle-error!!! library pre load lib handle is NULL!!!\r\n",0);
        return PRELOADER_PISPLIT_ERROR_HANDLE;
    }
    if( (p_handle->state_machine != PRELOADER_PISPLIT_STATUS_UNLOAD_DONE) && (p_handle->state_machine != PRELOADER_PISPLIT_STATUS_GET_HANDLE_DONE) )
    {
        PRELOADER_LOG_D(preloader, "preloader_pisplit_release_handle-error!!! library(0x%x) release handle error!!! User call flow error!!!\r\n",1,(unsigned int)p_handle->p_pi_library);
        return PRELOADER_PISPLIT_ERROR_USER_CALLFLOW;
    }

    p_handle->state_machine = PRELOADER_PISPLIT_STATUS_HANDLE_RELEASE;
    PRELOADER_LOG_I(preloader, "preloader_pisplit_release_handle library(0x%x)###debug###important info## release handle!",1,(unsigned int)p_handle->p_pi_library);
    return PRELOADER_PISPLIT_XTLIB_NO_ERR;
}


void* preloader_pisplit_get_export_parameter(preloader_pisplit_handle_t handle,void *p_import_parameters[])
{
    pisplit_library_info_t *p_handle = (pisplit_library_info_t *)handle;

#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
    if((is_dsp1_memory((uint32_t)p_handle->code_memory_heap)==true) ||(is_dsp1_memory((uint32_t)p_handle->data_memory_heap) == true))
    {
     PRELOADER_LOG_E(preloader, "preloader_pisplit_get_export_parameter library(0x%x)### call flow error, this is DSP1 lib, should get parameter on DSP1 side!!!\r\n ",1,(unsigned int)p_handle->p_pi_library);
    return NULL;
    }
#endif

    p_handle->p_import_parameters = p_import_parameters;

    /*this is DSP1 memory, DSP0 just help load lib for DSP1, so this init function should called by DSP1. 
    DSP1 should call this point firstly before call lib symbols.
    so,  If this is DSP0 load lib for DSPP0, should call this function immediately
         if this is DSP0 load lib for DSP1, can not call this function(because will exceed 1G boundary),DSP1 should call it by itself*/
    ((void (*)(void))(p_handle->lib_info.init))();//call library init function
    p_handle->preloader_pisplit_start = p_handle->lib_info.start_sym;
    if(p_handle->preloader_pisplit_start == NULL)
    {
      PRELOADER_LOG_E(preloader, "preloader_pisplit_get_export_parameter library(0x%x)### load error!!! preloader_pisplit_start as NULL!!!\r\n ",1,(unsigned int)p_handle->p_pi_library);
      //assert(0);
       while(1);
    }
    p_handle->p_export_parameters = p_handle->preloader_pisplit_start(p_handle->p_import_parameters);
    PRELOADER_LOG_I(preloader, "preloader_pisplit_get_export_parameter library(0x%x)###debug###important info##get export parameter:0x%x done",2,(unsigned int)p_handle->p_pi_library,(unsigned int)p_handle->p_export_parameters);

    return p_handle->p_export_parameters;
}


void* preloader_pisplit_malloc_memory(preloader_pisplit_memory_type_t memory_type,uint32_t size)
{
    preloader_alloc_info_t alloc_info;
#ifdef MTK_SUPPORT_HEAP_DEBUG  
    BlockLink_t *pxBlock;
#endif

    if(size == 0)
    {
        PRELOADER_LOG_E(preloader, "preloader_pisplit_malloc_memory-error!!!should not alloc type:%d buffer size as 0,",1,memory_type);
        return NULL;
    }
 #ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
    if(is_dsp1_memory_type(memory_type)==true )
    {
        preloader_dsp0_lock_dsp1_sleep_and_dcm();
    }
#endif
    find_and_alloc_a_memory_pool(&alloc_info,memory_type,size);
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
    if(is_dsp1_memory_type(memory_type)==true )
    {
        preloader_dsp0_unlock_dsp1_sleep_and_dcm();
    }
#endif
    if((alloc_info.buffer == NULL) || (alloc_info.pool_start == NULL))
    {
        PRELOADER_LOG_E(preloader, "preloader_pisplit_malloc_memory-error!!! allocate type:0x%x memory fail!!! want size:0x%x",2,memory_type,size);
        return NULL;
    }
    else
    {   
        #ifdef MTK_SUPPORT_HEAP_DEBUG
        pxBlock = mp_get_block_to_free(alloc_info.buffer, &(((MultiPoolHeaderLink_t*)alloc_info.pool_start)->xStart), ((MultiPoolHeaderLink_t*)alloc_info.pool_start)->xPoolAlignment);
        if(pxBlock != NULL)
        {
            pxBlock->xMallocLinkRegAddr = (uint32_t)__builtin_return_address(0);
        }
#endif
        PRELOADER_LOG_I(preloader, "preloader_pisplit_malloc_memory###debug###important info##allocate extra memory(type:0x%x,len:%d) from preloader heaps,heap pool:0x%x,get alloc buffer addr:0x%x",4,(unsigned int)memory_type,size,alloc_info.pool_start,alloc_info.buffer);
        return alloc_info.buffer;
    }
}

preloader_pisplit_error_handling_t preloader_pisplit_free_memory(void *buffer_addr)
{
    void * pool;
  
    uint32_t preloader_xLinkRegAddr = (uint32_t)__builtin_return_address(0);
    ( void )preloader_xLinkRegAddr;

#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
    if( is_dsp1_memory((uint32_t)buffer_addr)==true )
    {
        preloader_dsp0_lock_dsp1_sleep_and_dcm();
    }
#endif

    pool = find_a_memory_pool_start_by_buffer_address(buffer_addr);
    if(pool ==NULL)
    {
        PRELOADER_LOG_E(preloader, "preloader_pisplit_free_memory-error!!!buffer addr:0x%x,not belong any pools!!!",1,buffer_addr);
        return PRELOADER_PISPLIT_ERROR;
    }
    mp_free (pool,buffer_addr);
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
    if( is_dsp1_memory((uint32_t)buffer_addr)==true )
    {
        preloader_dsp0_unlock_dsp1_sleep_and_dcm();
    }
#endif

    PRELOADER_LOG_I(preloader, "preloader_pisplit_free_memory###debug###important info## lr_caller_function:0x%x free extra memory(addr:0x%x) from preloader heaps,heap pool:0x%x",3,preloader_xLinkRegAddr,(unsigned int)buffer_addr,(unsigned int)pool);
    return PRELOADER_PISPLIT_XTLIB_NO_ERR;
}

uint32_t preloader_pisplit_get_max_free_block_size(preloader_pisplit_memory_type_t memory_type)
{
    preloader_pool_free_size_info_t pool_free_size_info;
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
    if(is_dsp1_memory_type(memory_type)==true )
    {
        preloader_dsp0_lock_dsp1_sleep_and_dcm();
    }
#endif
    find_max_free_size(&pool_free_size_info,memory_type);
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
    if(is_dsp1_memory_type(memory_type)==true )
    {
        preloader_dsp0_unlock_dsp1_sleep_and_dcm();
    }
#endif
    PRELOADER_LOG_I(preloader, "preloader_pisplit_get_max_free_block_size###debug###important info## the preloader heap of (pool:0x%x) have the max free block size:%d",2,pool_free_size_info.pool_start,pool_free_size_info.max_free_size);
   return pool_free_size_info.max_free_size;
}

preloader_pisplit_error_handling_t preloader_pisplit_query_library_size(preloader_pisplit_handle_t handle,uint32_t *p_code_size,uint32_t*p_data_size)
{
    pisplit_library_info_t *p_handle = (pisplit_library_info_t *)handle;
    if((p_handle->state_machine >= PRELOADER_PISPLIT_STATUS_HANDLE_RELEASE) || (p_handle->state_machine <= PRELOADER_PISPLIT_STATUS_INIT_DONE))
    {
        *p_code_size = 0xffffffff;
        *p_data_size = 0xffffffff;
        PRELOADER_LOG_E(preloader, "preloader_pisplit_query_library_size-error!!! library(0x%x) error state_machine!!! :%d",2,(unsigned int)p_handle->p_pi_library,p_handle->state_machine);
        return PRELOADER_PISPLIT_ERROR_USER_CALLFLOW;
    }
    else
    {
        *p_code_size = p_handle->code_bytes+PRELOADER_MEMORY_ALIGNMENT;
        *p_data_size = p_handle->data_bytes+PRELOADER_MEMORY_ALIGNMENT;
        return PRELOADER_PISPLIT_XTLIB_NO_ERR;
    }
}
#ifdef MTK_SUPPORT_HEAP_DEBUG
preloader_pisplit_error_handling_t preloader_pisplit_dump_pic_memory_heap_info(preloader_pisplit_memory_type_t memory_type)
{
     preloader_pisplit_pool_info_t *temp_pool_info;
     uint32_t int_mask;
     unsigned int i ;  
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
    if(is_dsp1_memory_type(memory_type)==true )
    {
        preloader_dsp0_lock_dsp1_sleep_and_dcm();
    }
#endif
    for(i=0;i<preloader_pisplit_total_memory.pool_init_index;i++)
    {
        temp_pool_info = &preloader_pisplit_total_memory.p_pool_info[i];
        if((temp_pool_info->pool_type & memory_type) != 0)
        {
        hal_nvic_save_and_set_interrupt_mask(&int_mask);
        mp_dump_status((void *)temp_pool_info->pool_start);
        hal_nvic_restore_interrupt_mask(int_mask);
        }
    }
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
    if(is_dsp1_memory_type(memory_type)==true )
    {
        preloader_dsp0_unlock_dsp1_sleep_and_dcm();
    }
#endif
    return PRELOADER_PISPLIT_XTLIB_NO_ERR;
}
#endif /*MTK_SUPPORT_HEAP_DEBUG*/

/*pre_load_split_pi library public interface end*/

/****************************************************************************************************/
#ifdef __cplusplus
}
#endif


