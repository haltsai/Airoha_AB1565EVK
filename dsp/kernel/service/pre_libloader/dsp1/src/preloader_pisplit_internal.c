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
#include "hal_resource_assignment.h"

#ifdef __cplusplus
extern "C" {
#endif

log_create_module(preloader,PRINT_LEVEL_DEBUG );
#if 0
void preloader_disable_dcm_for_memory_access()
{
#if 0  /* no need to do this, because the function of hal_sleep_manager_lock_sleep(SLEEP_LOCK_DSP0_CROSS_CORE_PRELOADER) will do DSP0 DCM lock 
        User need call hal_sleep_manager_lock_sleep(SLEEP_LOCK_DSP0_CROSS_CORE_PRELOADER) before call preloader_pisplit_get_export_parameter() */
    //Onhe DCM design(AS DSP1 a example)
    // HW condition: When DSP1 Core idle and DSP1 I/DRAM idle and external memory idle etc. will enter DCM
    // HW design: on DSP0 side, have 4 bit, can control the 4 Core whether enabel DCM mode.

    //DISABLE DSP0 DCM
    //*((volatile uint32_t *)(0XA2270400)) = 0;//must!!! because DSP1 will access DSP0 DRAM
    *((volatile uint32_t *)(0XA2270404)) = 0;//must!!! because DSP1 will access DSP0 DRAM
    //*((volatile uint32_t *)(0XA2270408)) = 0;//must!!! because DSP1 will access DSP0 DRAM
    //*((volatile uint32_t *)(0XA227040C)) = 0;//must!!! because DSP1 will access DSP0 DRAM

    //DISABLE DSP1 DCM
    //*((volatile uint32_t *)(0XA2270500)) = 0;//In face,no need this.
    //*((volatile uint32_t *)(0XA2270504)) = 0;//In face,no need this.
    //*((volatile uint32_t *)(0XA2270508)) = 0;//In face,no need this.
    //*((volatile uint32_t *)(0XA227050C)) = 0;//In face,no need this.
#endif
}
void preloader_enable_dcm_for_memory_access()
{
#if 0  /* no need to do this, because the function of hal_sleep_manager_lock_sleep(SLEEP_LOCK_DSP0_CROSS_CORE_PRELOADER) will do DSP0 DCM lock 
        User need call hal_sleep_manager_lock_sleep(SLEEP_LOCK_DSP0_CROSS_CORE_PRELOADER) before call preloader_pisplit_get_export_parameter() */
    //DISABLE DSP0 DCM
    //*((volatile uint32_t *)(0XA2270400)) = 1;//must!!! because DSP1 will access DSP0 DRAM
    *((volatile uint32_t *)(0XA2270404)) = 1;//must!!! because DSP1 will access DSP0 DRAM
    //*((volatile uint32_t *)(0XA2270408)) = 1;//must!!! because DSP1 will access DSP0 DRAM
    //*((volatile uint32_t *)(0XA227040C)) = 1;//must!!! because DSP1 will access DSP0 DRAM

    //DISABLE DSP1 DCM
    //*((volatile uint32_t *)(0XA2270500)) = 1;//In face,no need this.
    //*((volatile uint32_t *)(0XA2270504)) = 1;//In face,no need this.
    //*((volatile uint32_t *)(0XA2270508)) = 1;//In face,no need this.
    //*((volatile uint32_t *)(0XA227050C)) = 1;//In face,no need this.
#endif
}
#endif
#ifdef __cplusplus
}
#endif
