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
#include <math.h>
#include "dsp_feature_interface.h"

#include "xt_library_loader.h"
#include "preloader_pisplit.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "wwe_interface.h"

#ifdef MTK_WWE_USE_PIC

#if 0
extern void vec_shift(f24 *restrict y, const f24 *restrict x, int t, int N);
extern int fft_real_24x24(f24 *y, int32_t *x, int N, int scalingOpt);
#else
extern void vec_shift(int *restrict y, const int *restrict x, int t, int N);
extern int fft_real_24x24(int *y, int32_t *x, int N, int scalingOpt);
#endif
extern int vec_bexp32(const int32_t *restrict x, int N);

static SemaphoreHandle_t wwe_load_lib_xSemaphore = NULL;

void wwe_default_function_parameter();

#if PIC_LOGPRINT
#define PRINTF_FUNC printf
#else
#define PRINTF_FUNC fake_printf
#endif

#ifdef MTK_WWE_AMA_USE_PIC
void *wwe_ama_import_parameters[7] = {
    PRINTF_FUNC,
    memcpy,
    memmove,
    strnlen,
    vec_shift,
    fft_real_24x24,
    vec_bexp32
};
#endif

#ifdef MTK_WWE_GSOUND_USE_PIC
void *wwe_gsound_import_parameters[10] = {
    PRINTF_FUNC,
    memcpy,
    memmove,
    _FSinx,
    floorf,
    sqrtf,
    log1pf,
    malloc,
    free,
    ceil
};
#endif

void *wwe_export_parameters[6] = {
    wwe_default_function_parameter,
    wwe_default_function_parameter,
    wwe_default_function_parameter,
    wwe_default_function_parameter,
    wwe_default_function_parameter,
    wwe_default_function_parameter
};


void wwe_default_function_parameter()
{
    DSP_MW_LOG_I("function point is NULL!!! wwe library not load or had been unload!!!", 0);
    assert(0);
}


static preloader_pisplit_handle_t p_wwe_pisplit_library_handle = NULL;
static uint32_t wwe_lib_loaded_counter = 0;

void wwe_load_library_callback(preloader_pisplit_handle_t handle)
{
    DSP_MW_LOG_I("[WWE_PIC CALLBACK]wwe_lib_loaded_counter = %d", 1, wwe_lib_loaded_counter);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t *p_export_parameters = NULL;
    if (g_wwe_mode == WWE_MODE_AMA) {
#ifdef MTK_WWE_AMA_USE_PIC
        p_export_parameters = preloader_pisplit_get_export_parameter(handle, wwe_ama_import_parameters);
#endif
    } else if (g_wwe_mode == WWE_MODE_GSOUND) {
#ifdef MTK_WWE_GSOUND_USE_PIC
        p_export_parameters = preloader_pisplit_get_export_parameter(handle, wwe_gsound_import_parameters);
#endif
    } else {
        DSP_MW_LOG_I("Invalid WWE mode!", 0);
        assert(0);
    }


    if (p_export_parameters == NULL) {
        DSP_MW_LOG_I("p_export_parameters is NULL, please check!!!", 0);
    } else {
        wwe_export_parameters[0] = (void *)p_export_parameters[0];
        wwe_export_parameters[1] = (void *)p_export_parameters[1];
        wwe_export_parameters[2] = (void *)p_export_parameters[2];
        wwe_export_parameters[3] = (void *)p_export_parameters[3];
        wwe_export_parameters[4] = (void *)p_export_parameters[4];
        wwe_export_parameters[5] = (void *)p_export_parameters[5];
        xSemaphoreGiveFromISR(wwe_load_lib_xSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

        DSP_MW_LOG_I("[WWE_PIC CALLBACK] export[0] = 0x%08x, export[1] = 0x%08x, export[2] = 0x%08x, export[3] = 0x%08x, export[4] = 0x%08x, export[5] = 0x%08x", 6, \
                     wwe_export_parameters[0], wwe_export_parameters[1], wwe_export_parameters[2], \
                     wwe_export_parameters[3], wwe_export_parameters[4], wwe_export_parameters[5]);
    }
}

void wwe_library_load()
{
#ifdef MTK_WWE_USE_PIC
    DSP_MW_LOG_I("[WWE_PIC LOAD]g_wwe_mode = %d", 1, g_wwe_mode);
    DSP_MW_LOG_I("[WWE_PIC LOAD]wwe_lib_loaded_counter = %d", 1, wwe_lib_loaded_counter);
    if (wwe_load_lib_xSemaphore == NULL) {
        wwe_load_lib_xSemaphore = xSemaphoreCreateBinary();

        if (wwe_load_lib_xSemaphore == NULL) {
            assert(0);
        }
    }
    if (p_wwe_pisplit_library_handle == NULL) {
        if (g_wwe_mode == WWE_MODE_AMA) {
#ifdef MTK_WWE_AMA_USE_PIC
            if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_wwe_pisplit_library_handle, &pisplit_library_pryon1000, wwe_load_library_callback)) {
                DSP_MW_LOG_I("pisplit_wwe preloader_pisplit_get_handle() error!!!!", 0);
                assert(0);
            }
#else
            DSP_MW_LOG_I("[WWE_PIC LOAD]PIC is not enable, return directly", 0);
            return;
#endif
        } else if (g_wwe_mode == WWE_MODE_GSOUND) {
#ifdef MTK_WWE_GSOUND_USE_PIC
            if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_wwe_pisplit_library_handle, &pisplit_google_hotword_dsp_multi_bank, wwe_load_library_callback)) {
                DSP_MW_LOG_I("pisplit_wwe preloader_pisplit_get_handle() error!!!!", 0);
                assert(0);
            }
#else
            DSP_MW_LOG_I("[WWE_PIC LOAD]PIC is not enable, return directly", 0);
            return;
#endif
        } else {
            DSP_MW_LOG_I("Invalid WWE mode!", 0);
            assert(0);
        }
    }

    if (wwe_lib_loaded_counter == 0) {
        DSP_MW_LOG_I("[WWE_PIC LOAD]Load Process...", 0);
        if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load(p_wwe_pisplit_library_handle, PRELOADER_I_HIGH_PERFORMANCE, PRELOADER_D_HIGH_PERFORMANCE)) {
            DSP_MW_LOG_I("pisplit_wwe preloader_pisplit_load() error!!!!", 0);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_LOW_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_LOW_PERFORMANCE);
            assert(0);
        }
        wwe_lib_loaded_counter = 1;
        if (xSemaphoreTake(wwe_load_lib_xSemaphore, portMAX_DELAY) == pdTRUE) {
        } else {
            assert(0);
        }
    } else {
        wwe_lib_loaded_counter++;
        //printf("wwe_library_load preloader_pisplit_load() had been loaded!!!");
    }
    vSemaphoreDelete(wwe_load_lib_xSemaphore);
    wwe_load_lib_xSemaphore = NULL; 
#endif
}

void wwe_library_unload()
{
#ifdef MTK_WWE_USE_PIC
    if (wwe_lib_loaded_counter == 1) {
        DSP_MW_LOG_I("[WWE_PIC LOAD]Unload Process...", 0);
        if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_unload(p_wwe_pisplit_library_handle)) {
            DSP_MW_LOG_I("pisplit_wwe preloader_pisplit_unload() error!!!!", 0);
            assert(0);
        }
        wwe_export_parameters[0] = wwe_default_function_parameter;
        wwe_export_parameters[1] = wwe_default_function_parameter;
        wwe_export_parameters[2] = wwe_default_function_parameter;
        wwe_export_parameters[3] = wwe_default_function_parameter;
        wwe_export_parameters[4] = wwe_default_function_parameter;
        wwe_export_parameters[5] = wwe_default_function_parameter;

        if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_release_handle(p_wwe_pisplit_library_handle)) {
            DSP_MW_LOG_E("pisplit_wwe preloader_pisplit_release_handle() error!!!!", 0);
            assert(0);
        }
        p_wwe_pisplit_library_handle = NULL;
        wwe_lib_loaded_counter = 0;
    } else {
        //wwe_lib_loaded_counter--;
    }

    DSP_MW_LOG_I("[WWE_PIC UNLOAD]wwe_lib_loaded_counter = %d", 1, wwe_lib_loaded_counter);
#endif
}

#endif
