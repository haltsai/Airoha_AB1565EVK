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
#include <xtensa/tie/xt_hifi2.h>
#include <xtensa/hal.h>
#include "assert.h"

#include "xt_library_loader.h"
#include "preloader_pisplit.h"
#include "ec120_portable.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "dsp_feature_interface.h"


/***********************************************************************/
static SemaphoreHandle_t ec120_load_lib_xSemaphore = NULL;


/***********************************************************************/
extern bool g_ecnr_prev_process_enable;
extern void ec120_default_function_parameter();

#define EC120_EXPORT_PARAM_NUM (13)

void *ec120_export_parameters[EC120_EXPORT_PARAM_NUM] = {
    ec120_default_function_parameter,
    ec120_default_function_parameter,
    ec120_default_function_parameter,
    ec120_default_function_parameter,
    ec120_default_function_parameter,
    ec120_default_function_parameter,
    ec120_default_function_parameter,
    ec120_default_function_parameter,
    ec120_default_function_parameter,
    ec120_default_function_parameter,
    ec120_default_function_parameter,
    ec120_default_function_parameter,
    ec120_default_function_parameter,
};

void *ec120_import_parameters[1] = {printf};

void ec120_default_function_parameter(void)
{
    DSP_MW_LOG_E("function point is NULL!!! ec120 library not load or had been unload!!!", 0);
    assert(0);
}

static preloader_pisplit_handle_t p_ec120_pisplit_library_handle = NULL;
static uint32_t ec120_lib_loaded_counter = 0;

void ec120_load_library_callback(preloader_pisplit_handle_t handle)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t *p_export_parameters;
    p_export_parameters = preloader_pisplit_get_export_parameter(handle,ec120_import_parameters);

    if(p_export_parameters == NULL)
    {
       DSP_MW_LOG_E("p_export_parameters is NULL, please check!!!", 0);
    }
    else
    {
        for (uint32_t i = 0; i < EC120_EXPORT_PARAM_NUM; i++) {
            ec120_export_parameters[i] = (void *)p_export_parameters[i];
        }
        xSemaphoreGiveFromISR( ec120_load_lib_xSemaphore, &xHigherPriorityTaskWoken );
    }
}

void ec120_library_load()
{
#if defined(MTK_3RD_PARTY_NR)
#if defined(AIR_IGO_NR_ENABLE) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
    igo_txnr_library_load(); /* Load IGO before EC since the memory size of IGO is larger */
#endif
#endif

    if (ec120_load_lib_xSemaphore == NULL)
    {
        ec120_load_lib_xSemaphore = xSemaphoreCreateBinary();

        if( ec120_load_lib_xSemaphore == NULL )
        {
            assert(0);
        }
    }
    if(p_ec120_pisplit_library_handle == NULL)
    {
        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_ec120_pisplit_library_handle,&pisplit_ec120,ec120_load_library_callback))
        {
            DSP_MW_LOG_E("pisplit_ec120 preloader_pisplit_get_handle() error!!!!", 0);
            assert(0);
        }
    }
  if(ec120_lib_loaded_counter == 0)
  {
        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load(p_ec120_pisplit_library_handle,PRELOADER_I_HIGH_PERFORMANCE,PRELOADER_D_HIGH_PERFORMANCE) )
        {
            DSP_MW_LOG_E("pisplit_ec120 preloader_pisplit_load() error!!!!", 0);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_LOW_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_LOW_PERFORMANCE);
            assert(0);
        }
        ec120_lib_loaded_counter = 1;
        DSP_MW_LOG_I("ec120_library_load finish", 0);
        if(xSemaphoreTake( ec120_load_lib_xSemaphore, portMAX_DELAY) == pdTRUE)
        {
        }
        else
        {
            assert(0);
        }

  }
  else
  {
        ec120_lib_loaded_counter++;
        DSP_MW_LOG_I("ec120_library_load preloader_pisplit_load() had been loaded!!!", 0);
  }
}

void ec120_library_unload()
{
#if defined(MTK_3RD_PARTY_NR)
#if defined(AIR_IGO_NR_ENABLE) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
    igo_txnr_library_unload();
#endif
#endif

    if(ec120_lib_loaded_counter == 1)
    {

        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_unload(p_ec120_pisplit_library_handle))
        {
            DSP_MW_LOG_E("pisplit_ec120 preloader_pisplit_unload() error!!!!", 0);
            assert(0);
        }
        for (uint32_t i = 0; i < EC120_EXPORT_PARAM_NUM; i++) {
            ec120_export_parameters[i] = ec120_default_function_parameter;
        }
        ec120_lib_loaded_counter = 0;
        DSP_MW_LOG_I("ec120_library_unload finish", 0);

    }
    else
    {
        ec120_lib_loaded_counter--;
    }


}

#ifdef AIR_ECNR_PREV_PART_ENABLE
void ecnr_prev_part_library_load(void)
{
    if (ec120_load_lib_xSemaphore == NULL) {
        ec120_load_lib_xSemaphore = xSemaphoreCreateBinary();
        if (ec120_load_lib_xSemaphore == NULL) {
            assert(0);
        }
    }
    if (p_ec120_pisplit_library_handle == NULL) {
        if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_ec120_pisplit_library_handle, &pisplit_ec120, ec120_load_library_callback)) {
            DSP_MW_LOG_E("[DSP][AEC/NR] pisplit_ec120 preloader_pisplit_get_handle() error!!!!", 0);
            assert(0);
        }
    }
    if (ec120_lib_loaded_counter == 0) {
        if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load(p_ec120_pisplit_library_handle, PRELOADER_I_HIGH_PERFORMANCE, PRELOADER_D_HIGH_PERFORMANCE)) {
            DSP_MW_LOG_E("[DSP][AEC/NR] pisplit_ec120 preloader_pisplit_load() error!!!!", 0);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_LOW_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_LOW_PERFORMANCE);
            assert(0);
        }
        ec120_lib_loaded_counter = 1;
        DSP_MW_LOG_I("[DSP][AEC/NR] ec120_library_load finish", 0);
        if (xSemaphoreTake(ec120_load_lib_xSemaphore, portMAX_DELAY) != pdTRUE) {
            assert(0);
        }
        g_ecnr_prev_process_enable = true;
    } else {
        ec120_lib_loaded_counter++;
        DSP_MW_LOG_I("[DSP][AEC/NR] ec120_library_load preloader_pisplit_load() had been loaded!!!", 0);
    }
}

void ecnr_prev_part_library_unload(void)
{
    uint32_t i;

    if (ec120_lib_loaded_counter == 1) {
        if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_unload(p_ec120_pisplit_library_handle)) {
            DSP_MW_LOG_E("[DSP][AEC/NR] pisplit_ec120 preloader_pisplit_unload() error!!!!", 0);
            assert(0);
        }
        for (i = 0; i < EC120_EXPORT_PARAM_NUM; i++) {
            ec120_export_parameters[i] = ec120_default_function_parameter;
        }
        ec120_lib_loaded_counter = 0;
        g_ecnr_prev_process_enable = false;
        DSP_MW_LOG_I("[DSP][AEC/NR] ec120_library_unload finish", 0);
    } else {
        ec120_lib_loaded_counter--;
    }
}
#endif

