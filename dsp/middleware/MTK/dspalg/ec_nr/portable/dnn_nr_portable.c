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
#include "dnn_nr_portable.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "dsp_feature_interface.h"


/***********************************************************************/
static SemaphoreHandle_t dnn_nr_load_lib_xSemaphore = NULL;
static preloader_pisplit_handle_t p_dnn_nr_pisplit_library_handle = NULL;
static uint32_t dnn_nr_lib_loaded_counter = 0;

/***********************************************************************/

extern void dnn_nr_default_function_parameter();

#define DNN_NR_EXPORT_PARAM_NUM (2)
void *dnn_nr_export_parameters[DNN_NR_EXPORT_PARAM_NUM] = {dnn_nr_default_function_parameter,
                                                           dnn_nr_default_function_parameter};

void *dnn_nr_import_parameters[1] = {printf};

void dnn_nr_default_function_parameter()
{
    DSP_MW_LOG_E("function point is NULL!!! dnn nr library not load or had been unload!!!", 0);
    assert(0);
}

void dnn_nr_load_library_callback(preloader_pisplit_handle_t handle)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t *p_export_parameters;
    p_export_parameters = preloader_pisplit_get_export_parameter(handle, dnn_nr_import_parameters);

    if(p_export_parameters == NULL)
    {
       DSP_MW_LOG_E("p_export_parameters is NULL, please check!!!", 0);
    }
    else
    {
        for (uint32_t i = 0; i < DNN_NR_EXPORT_PARAM_NUM; i++) {
            dnn_nr_export_parameters[i] = (void *)p_export_parameters[i];
        }

        xSemaphoreGiveFromISR( dnn_nr_load_lib_xSemaphore, &xHigherPriorityTaskWoken );
    }
}

void dnn_nr_library_load()
{
    if (dnn_nr_load_lib_xSemaphore == NULL) {
        dnn_nr_load_lib_xSemaphore = xSemaphoreCreateBinary();

        if ( dnn_nr_load_lib_xSemaphore == NULL ) {
            assert(0);
        }
    }
    if (p_dnn_nr_pisplit_library_handle == NULL) {
        if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_dnn_nr_pisplit_library_handle,&pisplit_igo_txnr,dnn_nr_load_library_callback)) {
            DSP_MW_LOG_E("pisplit_igo_txnr preloader_pisplit_get_handle() error!!!!", 0);
            assert(0);
        }
    }
    if (dnn_nr_lib_loaded_counter == 0) {
        if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load(p_dnn_nr_pisplit_library_handle,PRELOADER_I_HIGH_PERFORMANCE,PRELOADER_D_HIGH_PERFORMANCE) ) {
            DSP_MW_LOG_E("pisplit_igo_txnr preloader_pisplit_load() error!!!!", 0);
            assert(0);
        }
        dnn_nr_lib_loaded_counter = 1;
        if (xSemaphoreTake( dnn_nr_load_lib_xSemaphore, portMAX_DELAY) == pdTRUE) {
        }
        else {
            assert(0);
        }
    }
    else {
        dnn_nr_lib_loaded_counter++;
    }
}

void dnn_nr_library_unload()
{
    if (dnn_nr_lib_loaded_counter == 1) {
        if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_unload(p_dnn_nr_pisplit_library_handle)) {
            DSP_MW_LOG_E("pisplit_igo_txnr preloader_pisplit_unload() error!!!!", 0);
            assert(0);
        }
        for (uint32_t i = 0; i < DNN_NR_EXPORT_PARAM_NUM; i++) {
            dnn_nr_export_parameters[i] = dnn_nr_default_function_parameter;
        }
        dnn_nr_lib_loaded_counter = 0;
    }
    else {
        dnn_nr_lib_loaded_counter--;
    }
}

