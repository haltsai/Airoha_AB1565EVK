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
#include "hal.h"
#include "elf.h"
#include "xt_library_loader.h"
#include "loader_internal.h"
#include "hal_nvic.h"
#ifdef __XTENSA__
#include <xtensa/hal.h>  /* xthal_memcpy */
#endif
#include "assert.h"
#include <string.h>

#include"preloader_pisplit_internal.h"
#include"preloader_pisplit_log.h"

#include"preloader_pisplit.h"
#include "hal_sw_dma.h"
#include "hal_resource_assignment.h"

#ifdef __cplusplus
extern "C" {
#endif

log_create_module(preloader,PRINT_LEVEL_DEBUG );



/****************************************************************************************************/
/*internal driver start*/
extern unsigned int g_total_pic_lib;
extern int validate_dynamic_splitload (Elf32_Ehdr * header);
extern int find_align(Elf32_Ehdr * header);
extern xt_ptr align_ptr (xt_ptr ptr, xt_uint align);
extern pisplit_library_info_t *g_preloader_pisplit_library_info;
extern int get_dyn_info (Elf32_Ehdr * eheader, xt_ptr dst_addr, xt_uint src_offs, xt_ptr dst_data_addr, xt_uint src_data_offs, xtlib_pil_info * info);


extern void pisplit_library_dma_irq_handle(hal_sw_dma_event_t event, void *argument);


void xtlib_load_seg_mtk_fn(pisplit_library_info_t *p_handle,Elf32_Phdr *pheader, void *src_addr, void *dst_addr);
preloader_pisplit_error_handling_t xtlib_load_split_pi_library_common_mtk_fn (pisplit_library_info_t *p_handle);
#if 0 //NOT_SUPPORT_MULTI_LOAD_AT_SAME_TIME
void pisplit_restart_to_load(pisplit_library_info_t *p_handle);
#endif
void xtlib_load_data_seg_mtk_fn(pisplit_library_info_t *p_handle,Elf32_Phdr *pheader, void *src_addr, void *dst_addr);
void xtlib_load_code_seg_mtk_fn(pisplit_library_info_t *p_handle,Elf32_Phdr *pheader, void *src_addr, void *dst_addr);
void xtlib_target_init_pi_library_mtk_fn (xtlib_pil_info * lib_info);

volatile unsigned int dma_pre_load_memset_src_value = 0;

#if 0 //NOT_SUPPORT_MULTI_LOAD_AT_SAME_TIME
pisplit_library_info_t *pisplit_find_a_lib(preloader_pisplit_state_machine_t state)
{
   unsigned int i;
   for(i=0;i<g_total_pic_lib;i++)//query the table
   {
        if(g_preloader_pisplit_library_info[i].state_machine == state)
            return &g_preloader_pisplit_library_info[i];
   }
   return NULL;
}
#endif

#ifdef PRELOADER_VERIFY_IMAGE_COPY
__attribute__((optimize ("-O0"))) void pisplit_library_dma_irq_handle(hal_sw_dma_event_t event, void *argument)
#else
void pisplit_library_dma_irq_handle(hal_sw_dma_event_t event, void *argument)
#endif
{
#if 0 //NOT_SUPPORT_MULTI_LOAD_AT_SAME_TIME
    pisplit_library_info_t *p_temp;
#endif
    //unsigned int mask;
    preloader_pisplit_dma_control_t *dma_control_temp;
    pisplit_library_info_t *p_handle;

    dma_control_temp = (preloader_pisplit_dma_control_t *)argument;
    p_handle = dma_control_temp->p_current_handle;

    PRELOADER_LOG_D(preloader, "pisplit_library_dma_irq_handle library(0x%x) DMA irq triggered!\r\n",1,(unsigned int)p_handle->p_pi_library);
    if(HAL_SW_DMA_EVENT_TRANSACTION_ERROR == event)
    {
        p_handle->state_machine = PRELOADER_PISPLIT_STATUS_ERROR;
        PRELOADER_LOG_E(preloader, "pisplit_library_dma_irq_handle-error!!! library(0x%x) DMA handle tigger event error: %d !!!\r\n",2,(unsigned int)p_handle->p_pi_library,HAL_SW_DMA_EVENT_TRANSACTION_ERROR);
        assert(0);
        return ;
    }
    if((p_handle->state_machine != PRELOADER_PISPLIT_STATUS_START_LOADING))
    {
          //printf ("error!!!  pre load lib state_machine error!!!\r\n", i);
          p_handle->state_machine = PRELOADER_PISPLIT_STATUS_ERROR;
          
          PRELOADER_LOG_E(preloader, "pisplit_library_dma_irq_handle-error!!! library(0x%x) DMA handle tigger state_machine error,p_handle->state_machine:%d, the status not PRELOADER_PISPLIT_STATUS_START_LOADING\r\n",2,(unsigned int)p_handle->p_pi_library,p_handle->state_machine);
          assert(0);
          while(1);
    }

    dma_control_temp->pre_load_dma_event = PRELOADER_PISPLIT_DO_MEM_DONE;//change status to DONE!!!

    if( (p_handle->code_mem_copy_request.pre_load_dma_event != PRELOADER_PISPLIT_DO_MEM_DONE) ||
        (p_handle->code_mem_set_request.pre_load_dma_event != PRELOADER_PISPLIT_DO_MEM_DONE)  ||
        (p_handle->data_mem_copy_request.pre_load_dma_event != PRELOADER_PISPLIT_DO_MEM_DONE) ||
        (p_handle->data_mem_set_request.pre_load_dma_event != PRELOADER_PISPLIT_DO_MEM_DONE) )
     {
        PRELOADER_LOG_I(preloader, "pisplit_library_dma_irq_handle library(0x%x): a DMA irq, continue!\r\n ",1,(unsigned int)p_handle->p_pi_library);
        return ;// some DMA transfers does not done, need wait for the DMA complete continue!!!
     }      
    PRELOADER_LOG_I(preloader, "pisplit_library_dma_irq_handle library(0x%x): image copy done \r\n ",1,(unsigned int)p_handle->p_pi_library);

#if 0 //NOT_SUPPORT_MULTI_LOAD_AT_SAME_TIME
    hal_nvic_save_and_set_interrupt_mask(&mask);
    p_handle->state_machine = PRELOADER_PISPLIT_STATUS_LOAD_DONE;
    p_temp = pisplit_find_a_lib(PRELOADER_PISPLIT_STATUS_PENDING_TO_LOAD);
    if(p_temp != NULL)
    {
        p_temp->state_machine = PRELOADER_PISPLIT_STATUS_START_LOADING;
        hal_nvic_restore_interrupt_mask(mask);
        PRELOADER_LOG_I(preloader, "pisplit_library_dma_irq_handle###debug###important info## DMA irq- find a pending lib 0x%x, and start to loading it.",1,(unsigned int)p_temp->p_pi_library);
        pisplit_restart_to_load(p_temp);
    }
    else
    {
        hal_nvic_restore_interrupt_mask(mask);
    }
#else
    p_handle->state_machine = PRELOADER_PISPLIT_STATUS_LOAD_DONE;
#endif
   // p_handle->callback_func();//call user callback
#if defined(HAL_CACHE_MODULE_ENABLED)
//If DMA copy, theoretically,not need to cache operation again after memory copy(better to have)
//if CPU copy, should do D cache Flash and I cache invalidate (must)
#if defined(DEBUG_USE_CPU_COPY)
    //if use cpu copy, just do Dcache Flush and I cache invalidate
        PRELOADER_LOG_I(preloader, "xtlib_load_split_pi_library_common_mtk_fn do cache Flush and invalidate(addr:0x%x len:0x%x)\r\n ",2,NARROW_UP_TO_ICACHE_BYTES_ALIGN((uint32_t)p_handle->code_dest_memory_align),NARROW_UP_TO_ICACHE_BYTES_ALIGN(p_handle->code_bytes));
        if(HAL_CACHE_STATUS_OK != hal_cache_flush_multiple_cache_lines(NARROW_UP_TO_ICACHE_BYTES_ALIGN((uint32_t)p_handle->code_dest_memory_align),NARROW_UP_TO_ICACHE_BYTES_ALIGN(p_handle->code_bytes)))
        {
            PRELOADER_LOG_E(preloader, "xtlib_load_split_pi_library_common_mtk_fn Error: the dst code addr not 16byte align,addr:0x%x,len:%d\r\n ",2,NARROW_UP_TO_ICACHE_BYTES_ALIGN((uint32_t)p_handle->code_dest_memory_align),NARROW_UP_TO_ICACHE_BYTES_ALIGN(p_handle->code_bytes));
            assert(0);
        }
        if(HAL_CACHE_STATUS_OK != hal_cache_invalidate_multiple_i_cache_lines(NARROW_UP_TO_ICACHE_BYTES_ALIGN((uint32_t)p_handle->code_dest_memory_align),NARROW_UP_TO_ICACHE_BYTES_ALIGN(p_handle->code_bytes)))
        {
            PRELOADER_LOG_E(preloader, "xtlib_load_split_pi_library_common_mtk_fn Error: the dst code addr not 16byte align,addr:0x%x,len:%d\r\n ",2,NARROW_UP_TO_ICACHE_BYTES_ALIGN((uint32_t)p_handle->code_dest_memory_align),NARROW_UP_TO_ICACHE_BYTES_ALIGN(p_handle->code_bytes));
            assert(0);
        }
#else
    PRELOADER_LOG_D(preloader, "preloader_pisplit_get_export_parameter do cache invalidate(addr:0x%x len:0x%x)\r\n ",2,NARROW_UP_TO_ICACHE_BYTES_ALIGN((uint32_t)p_handle->code_dest_memory_align),NARROW_UP_TO_ICACHE_BYTES_ALIGN(p_handle->code_bytes));

    if(HAL_CACHE_STATUS_OK != hal_cache_invalidate_multiple_i_cache_lines(NARROW_UP_TO_ICACHE_BYTES_ALIGN((uint32_t)p_handle->code_dest_memory_align),NARROW_UP_TO_ICACHE_BYTES_ALIGN(p_handle->code_bytes)))
    {
        PRELOADER_LOG_E(preloader, "preloader_pisplit_get_export_parameter Error: the dst code addr not 16byte align,addr:0x%x,len:%d\r\n ",2,NARROW_UP_TO_ICACHE_BYTES_ALIGN((uint32_t)p_handle->code_dest_memory_align),NARROW_UP_TO_ICACHE_BYTES_ALIGN(p_handle->code_bytes));
        assert(0);
    }
    PRELOADER_LOG_D(preloader, "preloader_pisplit_get_export_parameter do cache invalidate(addr:0x%x len:0x%x)\r\n ",2,NARROW_UP_TO_DCACHE_BYTES_ALIGN((uint32_t)p_handle->data_dest_memory_align),NARROW_UP_TO_DCACHE_BYTES_ALIGN(p_handle->data_bytes));

    if(HAL_CACHE_STATUS_OK != hal_cache_invalidate_multiple_d_cache_lines(NARROW_UP_TO_DCACHE_BYTES_ALIGN((uint32_t)p_handle->data_dest_memory_align),NARROW_UP_TO_DCACHE_BYTES_ALIGN(p_handle->data_bytes)))
    {
        PRELOADER_LOG_E(preloader, "preloader_pisplit_get_export_parameter Error: the dst data addr not 16byte align,addr:0x%x,len:%d\r\n ",2,NARROW_UP_TO_DCACHE_BYTES_ALIGN((uint32_t)p_handle->data_dest_memory_align),NARROW_UP_TO_DCACHE_BYTES_ALIGN(p_handle->data_bytes));
        assert(0);
    }
#endif
#endif

#ifdef PRELOADER_VERIFY_IMAGE_COPY
{
        unsigned int i,j,len,end;
    unsigned int print_flag = 0;
    i = p_handle->code_source_memory;
    j = p_handle->code_dest_memory_align;
    end = p_handle->code_source_memory+p_handle->code_copy_size-4;
    PRELOADER_LOG_I(preloader,"pisplit_library_dma_irq_handle code memory check src-(0x%x:0x%x) dst-(0x%x:0x%x) len:%d",5,i,*(unsigned int *)i,j,*(unsigned int *)j,p_handle->code_copy_size);
    for(;i<end;i+=4,j+=4)
    {
        if(*(unsigned int *)i != *(unsigned int *)j)
        {
            if(print_flag == 0)
            {
                PRELOADER_LOG_I(preloader,"preloader_pisplit_get_export_parameter image code region copy verify err: (0x%x:%x)-(0x%x:%x)",4,i,*(unsigned int *)i,j,*(unsigned int *)j);
              //  while(1);
            }
            print_flag++;
        }
    }
    if(print_flag != 0)
    {
        i-=4;
        j-=4;
        PRELOADER_LOG_I(preloader,"preloader_pisplit_get_export_parameter image code region copy verify err end: (0x%x:%x)-(0x%x:%x)",4,i,*(unsigned int *)i,j,*(unsigned int *)j);
        PRELOADER_LOG_I(preloader,"preloader_pisplit_get_export_parameter image data region copy verify err counter:%d",1,print_flag);
        assert(0);
        while(1);
    }
    PRELOADER_LOG_I(preloader,"preloader_pisplit_get_export_parameter image code region copy verify pass = i:0x%x,j=0x%x!!!",2,i,j);

    print_flag = 0;
    i = p_handle->data_source_memory;
    j = p_handle->data_dest_memory_align;
    end = p_handle->data_source_memory+p_handle->data_copy_size-4;
    PRELOADER_LOG_I(preloader,"pisplit_library_dma_irq_handle RW memory check src-(0x%x:0x%x) dst-(0x%x:0x%x) len:%d ",4,i,*(unsigned int *)i,j,*(unsigned int *)j,p_handle->data_copy_size);
    for(;i<end;i+=4,j+=4)
    {
        if(*(unsigned int *)i != *(unsigned int *)j)
        {
           // if(print_flag == 0)
            {
                PRELOADER_LOG_I(preloader,"preloader_pisplit_get_export_parameter image data region copy verify err: (0x%x:%x)-(0x%x:%x)",4,i,*(unsigned int *)i,j,*(unsigned int *)j);
               //  while(1);
            }
            print_flag++;
        }
    }
    if(print_flag != 0)
    {
        i-=4;
        j-=4;
        PRELOADER_LOG_I(preloader,"preloader_pisplit_get_export_parameter image data region copy verify err end: (0x%x:%x)-(0x%x:%x)",4,i,*(unsigned int *)i,j,*(unsigned int *)j);
        PRELOADER_LOG_I(preloader,"preloader_pisplit_get_export_parameter image data region copy verify err counter:%d",1,print_flag);
        assert(0);
        while(1);

    }
    PRELOADER_LOG_I(preloader,"preloader_pisplit_get_export_parameter image data region copy verify pass = i:0x%x,j=0x%x!!!",2,i,j);
}

#endif
   xtlib_target_init_pi_library_mtk_fn(&p_handle->lib_info);

   p_handle->callback_func(dma_control_temp->p_current_handle);//call user callback
#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
    if((is_dsp1_memory((uint32_t)p_handle->code_memory_heap)==true) ||(is_dsp1_memory((uint32_t)p_handle->data_memory_heap) == true))
    {
        // when DMA transfer done, on callback to do DCM enable
        preloader_dsp0_unlock_dsp1_sleep_and_dcm();
    }
#endif

}
/*internal driver end*/
preloader_pisplit_error_handling_t xtlib_load_split_pi_library_common_mtk_fn (pisplit_library_info_t *p_handle)
{
  Elf32_Ehdr * header = (Elf32_Ehdr *)p_handle->p_pi_library;
  char * library =(char *)p_handle->p_pi_library;
  Elf32_Phdr * pheader;
  unsigned int align;

  int err = validate_dynamic_splitload (header);
  if (err != XTLIB_NO_ERR) {
    xtlib_globals.err = err;
    return PRELOADER_PISPLIT_ERROR;
  }
  align = find_align(header);
  PRELOADER_LOG_D(preloader, "xtlib_load_split_pi_library_common_mtk_fn library(0x%x) Cadence alignment:%d\r\n",2,(unsigned int)p_handle->p_pi_library,align);
  if(align<PRELOADER_MEMORY_ALIGNMENT/2)
    align = PRELOADER_MEMORY_ALIGNMENT/2;
  PRELOADER_LOG_D(preloader, "xtlib_load_split_pi_library_common_mtk_fn library(0x%x) MTK alignment:%d\r\n",2,(unsigned int)p_handle->p_pi_library,align);
  p_handle->code_dest_memory_align= align_ptr (p_handle->code_dest_memory, align);
  p_handle->data_dest_memory_align= align_ptr (p_handle->data_dest_memory, align);

  PRELOADER_LOG_D(preloader, "xtlib_load_split_pi_library_common_mtk_fn The code region relocation addr:0x%x,The data region relocation addr:0x%x",2,p_handle->code_dest_memory_align,p_handle->data_dest_memory_align);

  pheader = (Elf32_Phdr *) ((char *)library + xtlib_host_word (header->e_phoff));

  p_handle->code_dest_memory_align +=  (xtlib_host_word (pheader[0].p_paddr)  & (align-1) );

/*RG2018.9 also have bugs, the data region relocation addr need have a same offset(bit[0:2] or [0:3] or [0:4] ......) with load view.
but should not add on xtlib_host_word (pheader[1].p_paddr), it will cause waste memory
actually,if DSP instruction need data addr 8B align, the workaround should:xtlib_host_word (pheader[1].p_paddr)&0x7
         if DSP instruction need data addr 16B align, the workaround should:xtlib_host_word (pheader[1].p_paddr)&0xf
         if DSP instruction need data addr 32B align, the workaround should:xtlib_host_word (pheader[1].p_paddr)&0x1f
         if DSP instruction need data addr 64B align, the workaround should:xtlib_host_word (pheader[1].p_paddr)&0x3f
         ......and so on
*/
  p_handle->data_dest_memory_align += (xtlib_host_word (pheader[1].p_paddr) & (align-1));


  PRELOADER_LOG_I(preloader, "xtlib_load_split_pi_library_common_mtk_fn The code region relocation addr(RG_2018_9):0x%x,The data region relocation addr:0x%x",2,p_handle->code_dest_memory_align,p_handle->data_dest_memory_align);

  err = get_dyn_info (header,
              (xt_ptr)p_handle->code_dest_memory_align, xtlib_host_word (pheader[0].p_paddr),
              (xt_ptr)p_handle->data_dest_memory_align, xtlib_host_word (pheader[1].p_paddr),
              &p_handle->lib_info);

  if (err != XTLIB_NO_ERR) {
    xtlib_globals.err = err;
    PRELOADER_LOG_E(preloader, "xtlib_load_split_pi_library_common_mtk_fn-error!!! library(0x%x) get get_dyn_info() fail!!!\r\n ",1,(unsigned int)p_handle->p_pi_library);
    return PRELOADER_PISPLIT_ERROR;
  }
  p_handle->code_source_memory = (char *)library + xtlib_host_word (pheader[0].p_offset);
  

  p_handle->code_mem_copy_request.pre_load_dma_event = PRELOADER_PISPLIT_DO_MEM_INVALID;
  p_handle->code_mem_set_request.pre_load_dma_event = PRELOADER_PISPLIT_DO_MEM_INVALID;
  p_handle->data_mem_copy_request.pre_load_dma_event = PRELOADER_PISPLIT_DO_MEM_INVALID;
  p_handle->data_mem_set_request.pre_load_dma_event = PRELOADER_PISPLIT_DO_MEM_INVALID;

#if defined(HAL_CACHE_MODULE_ENABLED)
//If DMA copy, theoretically, need do cache invalidate before DMA cpy, and not need to cache invalidate operation again after memory copy (also do it is better).
//if CPU copy, need do nothing about cache operation before CPU memcpy (do it is better), but should do D cache Flush and I cache invalidate after memory copy.
      PRELOADER_LOG_D(preloader, "xtlib_load_split_pi_library_common_mtk_fn do cache Flush and invalidate(addr:0x%x len:0x%x)\r\n ",2,NARROW_UP_TO_ICACHE_BYTES_ALIGN((uint32_t)p_handle->code_dest_memory_align),NARROW_UP_TO_ICACHE_BYTES_ALIGN(p_handle->code_bytes));
      if(HAL_CACHE_STATUS_OK != hal_cache_invalidate_multiple_i_cache_lines(NARROW_UP_TO_ICACHE_BYTES_ALIGN((uint32_t)p_handle->code_dest_memory_align),NARROW_UP_TO_ICACHE_BYTES_ALIGN(p_handle->code_bytes)))
      {
          PRELOADER_LOG_E(preloader, "xtlib_load_split_pi_library_common_mtk_fn Error: the dst code addr not 16byte align,addr:0x%x,len:%d\r\n ",2,NARROW_UP_TO_ICACHE_BYTES_ALIGN((uint32_t)p_handle->code_dest_memory_align),NARROW_UP_TO_ICACHE_BYTES_ALIGN(p_handle->code_bytes));
          assert(0);
      }
    PRELOADER_LOG_D(preloader, "xtlib_load_split_pi_library_common_mtk_fn do cache invalidate(addr:0x%x len:0x%x)\r\n ",2,NARROW_DOWN_TO_DCACHE_BYTES_ALIGN((uint32_t)p_handle->data_dest_memory_align),NARROW_UP_TO_DCACHE_BYTES_ALIGN(p_handle->data_bytes));
    if(HAL_CACHE_STATUS_OK != hal_cache_invalidate_multiple_d_cache_lines(NARROW_DOWN_TO_DCACHE_BYTES_ALIGN((uint32_t)p_handle->data_dest_memory_align),NARROW_UP_TO_DCACHE_BYTES_ALIGN(p_handle->data_bytes)))
    {
        PRELOADER_LOG_E(preloader, "xtlib_load_split_pi_library_common_mtk_fn Error: the dst data addr not 16byte align,addr:0x%x,len:%d\r\n ",2,NARROW_DOWN_TO_DCACHE_BYTES_ALIGN((uint32_t)p_handle->data_dest_memory_align),NARROW_UP_TO_DCACHE_BYTES_ALIGN(p_handle->data_bytes));
        assert(0);
    }
#endif

  /* loading code */      
  xtlib_load_code_seg_mtk_fn(p_handle,&pheader[0],p_handle->code_source_memory,p_handle->code_dest_memory_align);

  if (p_handle->lib_info.text_addr == 0)
     p_handle->lib_info.text_addr = (xt_ptr) xtlib_xt_word ((Elf32_Word) p_handle->code_dest_memory_align);
  PRELOADER_LOG_W(preloader, "xtlib_load_split_pi_library_common_mtk_fn library(0x%x)###debug###important info## (xtlib_pi_library_debug_addr)lib_info.text_addr:0x%x",2,(uint32_t)p_handle->p_pi_library,(uint32_t)p_handle->lib_info.text_addr);
  p_handle->data_source_memory = (char *)library + xtlib_host_word (pheader[1].p_offset);
  /* loading data */
  xtlib_load_data_seg_mtk_fn (p_handle,&pheader[1],p_handle->data_source_memory,p_handle->data_dest_memory_align);
  return PRELOADER_PISPLIT_XTLIB_NO_ERR;
}


#if defined( DEBUG_USE_CPU_COPY) || defined(PRELOADER_VERIFY_IMAGE_COPY)
//use __attribute__((optimize ("-O0"))) void use dsp co-process instruction to do load&stroe to IRAM, because IRAM only use S32I to do load store!!!
__attribute__((optimize ("-O0"))) void xtlib_load_code_seg_mtk_fn(pisplit_library_info_t *p_handle,Elf32_Phdr *pheader, void *src_addr, void *dst_addr)
#else
void xtlib_load_code_seg_mtk_fn(pisplit_library_info_t *p_handle,Elf32_Phdr *pheader, void *src_addr, void *dst_addr)
#endif
{
    hal_sw_dma_config_info_t temp_dma_cfg;
    unsigned int bytes_to_copy = xtlib_host_word (pheader->p_filesz);
    unsigned int bytes_to_zero = xtlib_host_word (pheader->p_memsz) - bytes_to_copy;

#ifdef PRELOADER_VERIFY_IMAGE_COPY
    p_handle->code_copy_size = NARROW_UP_TO_4_BYTES_ALIGN(bytes_to_copy);
#endif
    xt_ptr zero_addr = dst_addr + bytes_to_copy;
    if (bytes_to_copy > 0)
    {
        p_handle->code_mem_copy_request.pre_load_dma_event = PRELOADER_PISPLIT_DO_CODE_MEM_COPY_START;
        temp_dma_cfg.source_address = hal_cache_cacheable_to_noncacheable((unsigned int)src_addr);
        temp_dma_cfg.destination_address = hal_cache_cacheable_to_noncacheable((unsigned int)dst_addr);
        temp_dma_cfg.length = NARROW_UP_TO_4_BYTES_ALIGN(bytes_to_copy);
        temp_dma_cfg.func = pisplit_library_dma_irq_handle;
        temp_dma_cfg.argument =  (void *)&p_handle->code_mem_copy_request;
        temp_dma_cfg.h_size = HAL_SW_DMA_WORD;
        temp_dma_cfg.dma_type = HAL_SW_DMA_NORMAL_MODE;
        temp_dma_cfg.transfer_type = HAL_SW_DMA_SINGLE_BURST;
        
#ifdef DEBUG_USE_CPU_COPY
        //for test image copy
    {
    unsigned int j,i;
    i = temp_dma_cfg.source_address;
    j = temp_dma_cfg.destination_address;    
    PRELOADER_LOG_I(preloader,"CPU code copy : src start (0x%x:0x%x)- dst start (0x%x:0x%x)",4,i,*(unsigned int *)i,j,*(unsigned int *)j);
        for(;i<temp_dma_cfg.source_address+temp_dma_cfg.length;i+=4,j+=4)
        {
            *(unsigned int *)j = *(unsigned int *)i;
            //if(j%2000 == 0) 
             //   PRELOADER_LOG_I(preloader,"CPU image copy: (0x%x:%x)-(0x%x:%x)",4,i,*(unsigned int *)i,j,*(unsigned int *)j);
        }
        PRELOADER_LOG_I(preloader,"CPU image copy done : (0x%x:%x)-(0x%x:%x)",4,i,*(unsigned int *)i,j,*(unsigned int *)j);
         temp_dma_cfg.length = 4;
    }
#endif
        PRELOADER_LOG_I(preloader, "xtlib_load_code_seg_mtk_fn library(0x%x) -loading code region- start dma to do memcpy,src addr=0x%x,dest addr=0x%x,len=%d narrow up to 4 bytes alignment len:%d\r\n ",5,(unsigned int)p_handle->p_pi_library,temp_dma_cfg.source_address,temp_dma_cfg.destination_address,bytes_to_copy,temp_dma_cfg.length);

        while(HAL_SW_DMA_STATUS_BUFFER_FULL == hal_sw_gdma_start(&temp_dma_cfg))
        {
            PRELOADER_LOG_W(preloader, "xtlib_load_code_seg_mtk_fn library(0x%x) -loading code region- DMA busy!!! try again!!!\r\n ",1,(unsigned int)p_handle->p_pi_library);
            hal_gpt_delay_ms(50);// wait for the SW DMA have idle buffer
        }
    }
    else
    {
       PRELOADER_LOG_I(preloader, "xtlib_load_code_seg_mtk_fn library(0x%x)-loading code region- bytes_to_copy is 0, no need start dma to do memcpy\r\n ",1,(unsigned int)p_handle->p_pi_library);
       p_handle->code_mem_copy_request.pre_load_dma_event = PRELOADER_PISPLIT_DO_MEM_DONE;
    }

    if (bytes_to_zero > 0)
    { 
        PRELOADER_LOG_E(preloader, "xtlib_load_code_seg_mtk_fn-error!!!library(0x%x) the code region should not have the requirement of memset(ZI),the zero_addr:0x%x,len:%d!!!!\r\n ",3,(unsigned int)p_handle->p_pi_library,(unsigned int)zero_addr,(unsigned int)bytes_to_zero);
    }
    else
    {
        PRELOADER_LOG_D(preloader, "xtlib_load_code_seg_mtk_fn library(0x%x)-loading code region- bytes_to_zero is 0, no need start dma to do memset\r\n ",1,(unsigned int)p_handle->p_pi_library);
        p_handle->code_mem_set_request.pre_load_dma_event = PRELOADER_PISPLIT_DO_MEM_DONE;
    }
}


#if defined( DEBUG_USE_CPU_COPY) || defined(PRELOADER_VERIFY_IMAGE_COPY)
__attribute__((optimize ("-O0"))) void xtlib_load_data_seg_mtk_fn(pisplit_library_info_t *p_handle,Elf32_Phdr *pheader, void *src_addr, void *dst_addr)
#else
void xtlib_load_data_seg_mtk_fn(pisplit_library_info_t *p_handle,Elf32_Phdr *pheader, void *src_addr, void *dst_addr)
#endif
{
  unsigned int bytes_to_copy = xtlib_host_word (pheader->p_filesz);
  unsigned int bytes_to_zero = xtlib_host_word (pheader->p_memsz) - bytes_to_copy;


  xt_ptr zero_addr = dst_addr + bytes_to_copy;
  hal_sw_dma_config_info_t temp_dma_cfg;
  unsigned int narrow_up_addr;

#ifdef PRELOADER_VERIFY_IMAGE_COPY
      p_handle->data_copy_size = NARROW_UP_TO_4_BYTES_ALIGN(bytes_to_copy);
      p_handle->data_zi_size = NARROW_UP_TO_4_BYTES_ALIGN(bytes_to_zero);
#endif


  if (bytes_to_copy > 0) {
    p_handle->data_mem_copy_request.pre_load_dma_event = PRELOADER_PISPLIT_DO_DATA_MEM_COPY_START;
    temp_dma_cfg.source_address = hal_cache_cacheable_to_noncacheable((unsigned int)src_addr);
    temp_dma_cfg.destination_address = hal_cache_cacheable_to_noncacheable((unsigned int)dst_addr);
    temp_dma_cfg.length = NARROW_UP_TO_4_BYTES_ALIGN(bytes_to_copy);
    temp_dma_cfg.func = pisplit_library_dma_irq_handle;
    temp_dma_cfg.argument =  (void *)&p_handle->data_mem_copy_request;
    temp_dma_cfg.h_size = HAL_SW_DMA_WORD;
    temp_dma_cfg.dma_type = HAL_SW_DMA_NORMAL_MODE;
    temp_dma_cfg.transfer_type = HAL_SW_DMA_SINGLE_BURST;

#ifdef DEBUG_USE_CPU_COPY
            //for test image copy
        {
        unsigned int j,i;
        i = temp_dma_cfg.source_address;
        j = temp_dma_cfg.destination_address;
        PRELOADER_LOG_I(preloader,"CPU data copy : src start (0x%x:0x%x)- dst start (0x%x:0x%x)",4,i,*(unsigned int *)i,j,*(unsigned int *)j);
            for(;i<temp_dma_cfg.source_address+temp_dma_cfg.length;i+=4,j+=4)
            {
                *(unsigned int *)j = *(unsigned int *)i;
               // if(j%2000 == 0) 
               //     PRELOADER_LOG_I(preloader,"CPU data copy: (0x%x:%x)-(0x%x:%x)",4,i,*(unsigned int *)i,j,*(unsigned int *)j);
            }
            PRELOADER_LOG_I(preloader,"CPU data copy done : (0x%x:%x)-(0x%x:%x)",4,i,*(unsigned int *)i,j,*(unsigned int *)j);
             temp_dma_cfg.length = 4;
        }
#endif



    PRELOADER_LOG_I(preloader, "xtlib_load_data_seg_mtk_fn library(0x%x)-loading data region- start dma to do memcpy,src addr=0x%x,dest addr=0x%x,len=%d,narrow up to 4 bytes alignment len:%d\r\n ",5,(unsigned int)p_handle->p_pi_library,temp_dma_cfg.source_address,temp_dma_cfg.destination_address,bytes_to_copy,temp_dma_cfg.length);
    while(HAL_SW_DMA_STATUS_BUFFER_FULL == hal_sw_gdma_start(&temp_dma_cfg))
    {
            PRELOADER_LOG_W(preloader, "xtlib_load_data_seg_mtk_fn library(0x%x) -loading data region- DMA busy!!! try again!!!\r\n ",1,(unsigned int)p_handle->p_pi_library);
            hal_gpt_delay_ms(50);// wait for the SW DMA have idle buffer
    }
  }
  else
  { 
    PRELOADER_LOG_I(preloader, "xtlib_load_data_seg_mtk_fn library(0x%x)-loading data region- bytes_to_copy is 0, no need start dma to do memcpy\r\n ",1,(unsigned int)p_handle->p_pi_library);
    p_handle->data_mem_copy_request.pre_load_dma_event = PRELOADER_PISPLIT_DO_MEM_DONE;
  }
  if (bytes_to_zero > 0)
  {
      PRELOADER_LOG_D(preloader, "xtlib_load_data_seg_mtk_fn library(0x%x)-loading data region- total to do memset,start addr:0x%x,len:%d\r\n",3,(unsigned int)p_handle->p_pi_library,(unsigned int)zero_addr,bytes_to_zero);
      /* do cpu memory set if the start addr not 4 byte align************************************************/
      //although on C:\usr\xtensa\XtDevTools\install\builds\RG-2017.7-win32\dsp0_core_winabi_xtensac\xtensa-elf\lib\pisplitload\ldscripts\elf32xtensa.xd can make
      //the .bss region start keep 4 or 8 bytes alignment,but we can not get the real .bss_start, just get total_data_region_size-rw_size
      //so the zero_addr and bytes_to_zero also maybe not 4 byte alignment, for code simple and stability, the flow always have this code to handle unalign case.
      PRELOADER_LOG_I(preloader, "xtlib_load_data_seg_mtk_fn library(0x%x)use CPU do byte memset len:%d",2,(unsigned int)p_handle->p_pi_library,NARROW_UP_TO_4_BYTES_ALIGN((unsigned int)zero_addr)-(unsigned int)zero_addr);
      narrow_up_addr = NARROW_UP_TO_4_BYTES_ALIGN((unsigned int)zero_addr);
      for( ;(unsigned int)zero_addr<narrow_up_addr;)
      {
          *(unsigned char*)hal_cache_cacheable_to_noncacheable((unsigned int)zero_addr) = 0x0;
          bytes_to_zero--;
          zero_addr = (xt_ptr)((unsigned int)zero_addr +1);
       }
      //notice: ZI region must be 4 bytes align, If use CPU to do some unalign bytes, need to do cache invalidate!!! but one cache line is 16B, so there will be have a big problem
      /* do cpu memory set if the end addr not 4 byte align**************************************************/
        // no need to do it, because the data buffer size will narrow up to 4 byte alignment to allocate
      /*do cpu memory set done*******************************************************************************/
    if(bytes_to_zero==0)
    {
        PRELOADER_LOG_I(preloader, "xtlib_load_data_seg_mtk_fn library(0x%x)-loading data region- bytes_to_zero too small, use CPU to do memset\r\n ",1,(unsigned int)p_handle->p_pi_library);
        p_handle->data_mem_set_request.pre_load_dma_event = PRELOADER_PISPLIT_DO_MEM_DONE;
        return ;
    } 

        p_handle->data_mem_set_request.pre_load_dma_event = PRELOADER_PISPLIT_DO_DATA_MEM_SET_START;
        temp_dma_cfg.source_address = hal_cache_cacheable_to_noncacheable((unsigned int)(&dma_pre_load_memset_src_value));
        temp_dma_cfg.destination_address = hal_cache_cacheable_to_noncacheable((unsigned int)zero_addr);
        temp_dma_cfg.length = NARROW_UP_TO_4_BYTES_ALIGN(bytes_to_zero);
        temp_dma_cfg.func = pisplit_library_dma_irq_handle;
        temp_dma_cfg.argument =  (void *)&p_handle->data_mem_set_request;
        temp_dma_cfg.h_size = HAL_SW_DMA_WORD;
        temp_dma_cfg.dma_type = HAL_SW_DMA_SOURCE_ADDRESS_FIXED_MODE;
        temp_dma_cfg.transfer_type = HAL_SW_DMA_SINGLE_BURST;
        PRELOADER_LOG_I(preloader, "xtlib_load_data_seg_mtk_fn library(0x%x)-loading data region- start dma to do memset,dest addr=0x%x,len=%d, narrow up to 4 bytes alignment len:%d\r\n ",4,(unsigned int)p_handle->p_pi_library,temp_dma_cfg.destination_address,bytes_to_zero,temp_dma_cfg.length);
#ifdef DEBUG_USE_CPU_COPY
                    //for test image copy
                {
                unsigned int j;
                j = temp_dma_cfg.destination_address;

                    for(;j<temp_dma_cfg.destination_address+temp_dma_cfg.length;j+=4)
                    {
                        *(unsigned int *)j = 0;
                       // if(j%2000 == 0) 
                      //      PRELOADER_LOG_I(preloader,"CPU data ZI:(0x%x:%x)",2,j,*(unsigned int *)j);
                    }
                    PRELOADER_LOG_I(preloader,"CPU data ZI done: (0x%x:%x)",2,j,*(unsigned int *)j);
                     temp_dma_cfg.length = 4;
                }
#endif

        while(HAL_SW_DMA_STATUS_BUFFER_FULL == hal_sw_gdma_start(&temp_dma_cfg))
        {
            PRELOADER_LOG_W(preloader, "xtlib_load_data_seg_mtk_fn library(0x%x) -loading ZI- DMA busy!!! try again!!!\r\n ",1,(unsigned int)p_handle->p_pi_library);
            hal_gpt_delay_ms(50);// wait for the SW DMA have idle buffer
        }
  }
  else
  {
      PRELOADER_LOG_I(preloader, "xtlib_load_data_seg_mtk_fn library(0x%x)-loading data region- bytes_to_zero is 0, no need start dma to do memset\r\n ",1,(unsigned int)p_handle->p_pi_library);
      p_handle->data_mem_set_request.pre_load_dma_event = PRELOADER_PISPLIT_DO_MEM_DONE;
  }
}



void xtlib_target_init_pi_library_mtk_fn (xtlib_pil_info * lib_info)
{
  int err = xtlib_relocate_pi_lib (lib_info);
  if (err != XTLIB_NO_ERR) {
    xtlib_globals.err = err;
    return ;
  }

  xtlib_sync ();

//  ((void (*)(void))(lib_info->init))(); //modify cadence service, call this function later.
 //return lib_info->start_sym;
}

#ifdef __cplusplus
}
#endif

