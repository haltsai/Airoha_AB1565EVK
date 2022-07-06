/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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
#include <xtensa/tie/xt_hifi2.h>
#include <xtensa/hal.h>
#include "assert.h"

#include "xt_library_loader.h"
#include "preloader_pisplit.h"
#include "peq_portable.h"

#include "FreeRTOS.h"
#include "semphr.h"


/***********************************************************************/
static SemaphoreHandle_t peq_load_lib_xSemaphore = NULL;


/***********************************************************************/

extern void peq_default_function_parameter();


void *peq_export_parameters[2] = {peq_default_function_parameter,
                                  peq_default_function_parameter};
#if PIC_LOGPRINT
void *peq_import_parameters[1] = {printf};//
#else
void *peq_import_parameters[1] = {fake_printf};//
#endif
//void *peq_import_parameters[1] = {fake_printf};

void peq_default_function_parameter()
{
    DSP_MW_LOG_E("function point is NULL!!! peq library not load or had been unload!!!", 0);
    assert(0);
}


static preloader_pisplit_handle_t p_peq_pisplit_library_handle = NULL;
static uint32_t peq_lib_loaded_counter = 0;

void peq_load_library_callback(preloader_pisplit_handle_t handle)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t *p_export_parameters;
    p_export_parameters = (uint32_t *)preloader_pisplit_get_export_parameter(handle,peq_import_parameters);

    if(p_export_parameters == NULL)
    {
        DSP_MW_LOG_E("p_export_parameters is NULL, please check!!!", 0);
    }
    else
    {
        peq_export_parameters[0] = (void *)p_export_parameters[0];
        peq_export_parameters[1] = (void *)p_export_parameters[1];

        xSemaphoreGiveFromISR( peq_load_lib_xSemaphore, &xHigherPriorityTaskWoken );
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void peq_library_load()
{
    if (peq_load_lib_xSemaphore == NULL)
    {
        peq_load_lib_xSemaphore = xSemaphoreCreateBinary();

        if( peq_load_lib_xSemaphore == NULL )
        {
            assert(0);
        }
    }
    if(p_peq_pisplit_library_handle == NULL)
    {
        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_peq_pisplit_library_handle, &pisplit_peq2, peq_load_library_callback) )
        {
            DSP_MW_LOG_E("pisplit_peq preloader_pisplit_get_handle() error!!!!", 0);
            assert(0);
        }
    }
    if(peq_lib_loaded_counter == 0)
    {
        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load(p_peq_pisplit_library_handle,PRELOADER_I_HIGH_PERFORMANCE,PRELOADER_D_HIGH_PERFORMANCE) )
        {
            DSP_MW_LOG_E("pisplit_peq preloader_pisplit_load() error!!!!", 0);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_LOW_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_LOW_PERFORMANCE);
            assert(0);
        }
        peq_lib_loaded_counter = 1;
        if(xSemaphoreTake( peq_load_lib_xSemaphore, portMAX_DELAY) == pdTRUE)
        {
        }
        else
        {
            assert(0);
        }
    }
    else
    {
        peq_lib_loaded_counter++;
        //printf("peq_library_load preloader_pisplit_load() had been loaded!!!");
    }
    vSemaphoreDelete(peq_load_lib_xSemaphore);
    peq_load_lib_xSemaphore = NULL;
}

void peq_library_unload()
{
    if(peq_lib_loaded_counter == 1)
    {
        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_unload(p_peq_pisplit_library_handle))
        {
            DSP_MW_LOG_E("pisplit_peq preloader_pisplit_unload() error!!!!", 0);
            assert(0);
        }
        peq_export_parameters[0] = peq_default_function_parameter;
        peq_export_parameters[1] = peq_default_function_parameter;
        //p_peq_pisplit_library_handle = NULL;
        peq_lib_loaded_counter = 0;
    }
    else
    {
        peq_lib_loaded_counter--;
    }

}


