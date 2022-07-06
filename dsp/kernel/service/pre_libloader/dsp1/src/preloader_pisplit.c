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
#include"preloader_pisplit_internal.h"
#include "xt_library_loader.h"
#include "loader_internal.h"
#include "hal_nvic.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void preloader_disable_dcm_for_memory_access();
extern void preloader_enable_dcm_for_memory_access();

void* preloader_pisplit_get_export_parameter(preloader_pisplit_handle_t handle,void *p_import_parameters[])
{
    pisplit_library_info_t *p_handle = (pisplit_library_info_t *)handle;

    //if( ((uint32_t)p_handle->code_memory_heap > 0x60000000) && ((uint32_t)p_handle->code_memory_heap < 0x60030000))
    //{
    // PRELOADER_LOG_E(preloader, "preloader_pisplit_get_export_parameter library(0x%x)### call flow error, this is DSP1 lib, should get parameter on DSP1 side!!!\r\n ",1,(unsigned int)p_handle->p_pi_library);
    //return NULL;
    //}

    //because of p_handle locate on DSP0 DRAM,so must be do this
    hal_sleep_manager_lock_sleep(SLEEP_LOCK_DSP0_CROSS_CORE_PRELOADER);
    //preloader_disable_dcm_for_memory_access();

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

    //because of p_handle locate on DSP0 DRAM,so must be do this
    //preloader_enable_dcm_for_memory_access();
    hal_sleep_manager_unlock_sleep(SLEEP_LOCK_DSP0_CROSS_CORE_PRELOADER);
    return p_handle->p_export_parameters;
}
/****************************************************************************************************/
#ifdef __cplusplus
}
#endif



