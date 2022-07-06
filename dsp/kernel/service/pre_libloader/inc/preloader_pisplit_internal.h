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

#ifndef __PRELOADER_PISPLIT_INTERNAL__
#define __PRELOADER_PISPLIT_INTERNAL__
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#include"preloader_pisplit.h"
#include "xt_library_loader.h"

//#define PRELOADER_VERIFY_IMAGE_COPY
//#define DEBUG_USE_CPU_COPY
#define PRELOADER_MEMORY_ALIGNMENT (32) //for cache line alignment, one cache line 16B, reserved two cache line size


/*split pi library configure part start*/
typedef enum {
  PRELOADER_PISPLIT_STATUS_DEFAULT = 0x0,              // 0-one time
  PRELOADER_PISPLIT_STATUS_INIT_DONE,            // 1-one time
  PRELOADER_PISPLIT_STATUS_GET_HANDLE_DONE,      // 2-better one time
  PRELOADER_PISPLIT_STATUS_TRY_TO_LOAD,          // circular one by one from PRELOADER_PISPLIT_STATUS_TRY_TO_LOAD to PRELOADER_PISPLIT_STATUS_UNLOAD_DONE
  PRELOADER_PISPLIT_STATUS_ALLOC_MEMORY_DONE,    // circular one by one from PRELOADER_PISPLIT_STATUS_TRY_TO_LOAD to PRELOADER_PISPLIT_STATUS_UNLOAD_DONE
  PRELOADER_PISPLIT_STATUS_START_LOADING,        // circular one by one from PRELOADER_PISPLIT_STATUS_TRY_TO_LOAD to PRELOADER_PISPLIT_STATUS_UNLOAD_DONE
  PRELOADER_PISPLIT_STATUS_PENDING_TO_LOAD,      // pending...
  PRELOADER_PISPLIT_STATUS_LOAD_DONE,            // circular one by one from PRELOADER_PISPLIT_STATUS_TRY_TO_LOAD to PRELOADER_PISPLIT_STATUS_UNLOAD_DONE
  PRELOADER_PISPLIT_STATUS_MEMORY_FREE,          // circular one by one from PRELOADER_PISPLIT_STATUS_TRY_TO_LOAD to PRELOADER_PISPLIT_STATUS_UNLOAD_DONE
  PRELOADER_PISPLIT_STATUS_TRY_TO_UNLOAD,        // circular one by one from PRELOADER_PISPLIT_STATUS_TRY_TO_LOAD to PRELOADER_PISPLIT_STATUS_UNLOAD_DONE
  PRELOADER_PISPLIT_STATUS_UNLOAD_DONE,          // circular one by one from PRELOADER_PISPLIT_STATUS_TRY_TO_LOAD to PRELOADER_PISPLIT_STATUS_UNLOAD_DONE
  PRELOADER_PISPLIT_STATUS_HANDLE_RELEASE,       //better one time
  PRELOADER_PISPLIT_STATUS_ERROR                 //
}preloader_pisplit_state_machine_t;

typedef enum {
    PRELOADER_PISPLIT_DO_MEM_INVALID,
    PRELOADER_PISPLIT_DO_CODE_MEM_COPY_START,
    PRELOADER_PISPLIT_DO_CODE_MEM_SET_START,
    PRELOADER_PISPLIT_DO_DATA_MEM_COPY_START,
    PRELOADER_PISPLIT_DO_DATA_MEM_SET_START,
    PRELOADER_PISPLIT_DO_MEM_DONE
}preloader_pisplit_dma_event_t;


typedef struct {
     preloader_pisplit_dma_event_t pre_load_dma_event;
     void *p_current_handle;
}preloader_pisplit_dma_control_t;

typedef void *(*preloader_pisplit_start_ptr_t)(void *);


typedef struct {
     /*record the image start address of the library.*/
     xtlib_packaged_library *p_pi_library;
     /*record the library use which code memory heap.*/
     void * code_memory_heap;
     /*record the library use which data memory heap.*/
     void * data_memory_heap;
     /*record the library need how many bytes code size, query by xtlib_split_pi_library_size().
       memory allocate size = p_handle->code_bytes+PRELOADER_MEMORY_ALIGNMENT*/
     unsigned int code_bytes;
#ifdef PRELOADER_VERIFY_IMAGE_COPY
     /*record the real code copy size,just for debug, on DMA irq handle use CPU to check the correctness of memory copy.*/
     unsigned int code_copy_size;
#endif
     /*record the source code region start address, query from Cadence service.*/
     void *code_source_memory;
     /*record the destination code region start address, allocated by code_memory_heap.
       allocate and free code memory will use it.*/
     void *code_dest_memory;
     /*record the dest code region relocation address.
       this address is real start address of memory copy.
       this address == code_dest_memory + a pad,the pad==(xtlib_host_word (pheader[0].p_paddr)  & (align-1) ).*/
     void *code_dest_memory_align;
     /*record the library need how many bytes data size(RW+ZI), query by xtlib_split_pi_library_size().
       memory allocate size = p_handle->data_bytes+PRELOADER_MEMORY_ALIGNMENT*/
     unsigned int data_bytes;
#ifdef PRELOADER_VERIFY_IMAGE_COPY
     /*record the real data copy size,just for debug, on DMA irq handle use CPU to check the correctness of memory copy.*/
     unsigned int data_copy_size;
     /*record the real ZI size,just for debug, on DMA irq handle use CPU to check the correctness of memset.*/
     unsigned int data_zi_size;
#endif
     /*record the source data region start address, query from Cadence service.*/
     void *data_source_memory;
     /*record the destination data region start address, allocated by data_memory_heap.
       allocate and free code memory will use it.*/
     void *data_dest_memory;
     /*record the dest data region relocation address.
       this address is real start address of memory copy.
       this address == data_dest_memory + a pad,the pad==(xtlib_host_word (pheader[1].p_paddr)  & (align-1) ).*/
     void *data_dest_memory_align;
     /*record all of the lib basic info, this struct will be record by the function of get_dyn_info() when do loading lib.
       On cadence service, need always use this struct as the a handle for called Cadence APIs,like xtlib_unload_pi_library etc.*/
     xtlib_pil_info lib_info;
     /*record the preloader state machine*/
     preloader_pisplit_state_machine_t state_machine;
     /*record the use callback, will notify users when loading lib done.*/
     preloader_pisplit_customer_callback_t callback_func;
     /*record the start function point.*/
     preloader_pisplit_start_ptr_t preloader_pisplit_start;
     /*record import and export parameters.*/
     void *p_import_parameters;
     void *p_export_parameters;
     /*record DMA state when do memcpy and memset use DMA.*/
     preloader_pisplit_dma_control_t code_mem_copy_request;
     preloader_pisplit_dma_control_t code_mem_set_request;
     preloader_pisplit_dma_control_t data_mem_copy_request;
     preloader_pisplit_dma_control_t data_mem_set_request;
 }pisplit_library_info_t;


#define NARROW_UP_TO_4_BYTES_ALIGN(data)  ((data+3)&(~0x3))
#define NARROW_DOWN_TO_4_BYTES_ALIGN(data)  ((data)&(~0x3))

#if (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565)
#define NARROW_UP_TO_ICACHE_BYTES_ALIGN(data)  ((data+63)&(~0x3f))
#define NARROW_UP_TO_DCACHE_BYTES_ALIGN(data)  ((data+31)&(~0x1f))
#define NARROW_DOWN_TO_DCACHE_BYTES_ALIGN(data)  ((data)&(~0x1f))
#endif
#if((PRODUCT_VERSION == 1552) || (PRODUCT_VERSION == 2552))
#define NARROW_UP_TO_ICACHE_BYTES_ALIGN(data)  ((data+15)&(~0xf))
#define NARROW_UP_TO_DCACHE_BYTES_ALIGN(data)  ((data+15)&(~0xf))
#define NARROW_DOWN_TO_DCACHE_BYTES_ALIGN(data)  ((data)&(~0xf))
#endif

#if 0 //NOT_SUPPORT_MULTI_LOAD_AT_SAME_TIME
extern pisplit_library_info_t *pisplit_find_a_lib(preloader_pisplit_state_machine_t state);
#endif



#ifdef __cplusplus
}
#endif

#endif /* __PRELOADER_PISPLIT_INTERNAL__ */
