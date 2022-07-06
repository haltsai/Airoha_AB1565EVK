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

#include "xa_aac_dec_api.h"

#include "xt_library_loader.h"
#include "preloader_pisplit.h"
#include "aac_decoder_portable.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "dsp_feature_interface.h"

/***********************************************************************/
SemaphoreHandle_t acc_load_lib_xSemaphore = NULL;


/***********************************************************************/

extern void aac_default_function_parameter();


void *aac_decoder_export_parameters[1] = {aac_default_function_parameter};
#if PIC_LOGPRINT
void *aac_decoder_import_parameters[2] = {printf,memcpy};//
#else
void *aac_decoder_import_parameters[2] = {fake_printf,memcpy};//
#endif

void aac_default_function_parameter()
{
    DSP_MW_LOG_E("function point is NULL!!! aac_decoder library not load or had been unload!!!", 0);
    assert(0);
}


preloader_pisplit_handle_t p_aac_pisplit_library_handle = NULL;

void aac_load_library_callback(preloader_pisplit_handle_t handle)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t *p_export_parameters;
    p_export_parameters = (uint32_t *)preloader_pisplit_get_export_parameter(handle,aac_decoder_import_parameters);

    DSP_MW_LOG_I("*p_export_parameters:0x%x", 1,*p_export_parameters);
    if(p_export_parameters == NULL)
    {
       DSP_MW_LOG_E("p_export_parameters is NULL, please check!!!", 0);
    }
    else
    {
        aac_decoder_export_parameters[0] = (void *)p_export_parameters[0];
        #if PIC_LOGPRINT
        DSP_MW_LOG_I("aac_decoder_export_parameters[0]:0x%x", 1,aac_decoder_export_parameters[0]);
        #endif
        xSemaphoreGiveFromISR( acc_load_lib_xSemaphore, &xHigherPriorityTaskWoken );
    }
}

void aac_library_load()
{
    if (acc_load_lib_xSemaphore == NULL)
    {
        acc_load_lib_xSemaphore = xSemaphoreCreateBinary();

        if( acc_load_lib_xSemaphore == NULL )
        {
            assert(0);
        }
    }
      
    if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_aac_pisplit_library_handle,&xa_aac_dec_library,aac_load_library_callback) )
    {
        DSP_MW_LOG_E("xa_aac_dec_library preloader_pisplit_get_handle() error!!!!", 0);
        assert(0);
    }
    if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load(p_aac_pisplit_library_handle,PRELOADER_I_HIGH_PERFORMANCE,PRELOADER_D_HIGH_PERFORMANCE) )
    {
        DSP_MW_LOG_E("xa_aac_dec_library preloader_pisplit_load() error!!!!", 0);
        preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_HIGH_PERFORMANCE);
        preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_LOW_PERFORMANCE);
        preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_HIGH_PERFORMANCE);
        preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_LOW_PERFORMANCE);
        assert(0);
    }
    if(xSemaphoreTake( acc_load_lib_xSemaphore, portMAX_DELAY) == pdTRUE)
    {
    }
    else
    {
        assert(0);
    }
}

void aac_library_unload()
{
    if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_unload(p_aac_pisplit_library_handle))
    {
        DSP_MW_LOG_E("xa_aac_dec_library preloader_pisplit_unload() error!!!!", 0);
        assert(0);
    }
    if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_release_handle(p_aac_pisplit_library_handle))
    {
        DSP_MW_LOG_E("xa_aac_dec_library preloader_pisplit_release_handle() error!!!!", 0);
        assert(0);
    }
    
    aac_decoder_export_parameters[0] = aac_default_function_parameter;
    p_aac_pisplit_library_handle = NULL;
}


