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
#include "ecnr_portable.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "dsp_feature_interface.h"


/***********************************************************************/
static SemaphoreHandle_t ecnr_load_lib_xSemaphore = NULL;
static preloader_pisplit_handle_t p_ecnr_pisplit_library_handle = NULL;
static uint32_t ecnr_lib_loaded_counter = 0;

/***********************************************************************/

extern void ecnr_default_function_parameter();

#ifdef MTK_INEAR_ENHANCEMENT
 #define ECNR_DECODER_EXPORT_PARAM_NUM (17)
#elif defined(MTK_DUALMIC_INEAR)
 #define ECNR_DECODER_EXPORT_PARAM_NUM (18)
#elif defined(MTK_3RD_PARTY_NR)
 #define ECNR_DECODER_EXPORT_PARAM_NUM (16)
#else
 #define ECNR_DECODER_EXPORT_PARAM_NUM (14)
#endif

void *ecnr_decoder_export_parameters[ECNR_DECODER_EXPORT_PARAM_NUM] = {
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
#if defined(MTK_3RD_PARTY_NR)
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
#elif defined(MTK_INEAR_ENHANCEMENT)
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
#elif defined(MTK_DUALMIC_INEAR)
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
    ecnr_default_function_parameter,
#endif
};

#if PIC_LOGPRINT
void *ecnr_decoder_import_parameters[1] = {printf};//
#else
void *ecnr_decoder_import_parameters[1] = {fake_printf};//
#endif

void ecnr_default_function_parameter()
{
    DSP_MW_LOG_E("function point is NULL!!! ecnr_decoder library not load or had been unload!!!", 0);
    assert(0);
}

void ecnr_load_library_callback(preloader_pisplit_handle_t handle)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t *p_export_parameters;
    p_export_parameters = preloader_pisplit_get_export_parameter(handle,ecnr_decoder_import_parameters);

    if(p_export_parameters == NULL)
    {
       DSP_MW_LOG_E("p_export_parameters is NULL, please check!!!", 0);
    }
    else
    {
        for (uint32_t i = 0; i < ECNR_DECODER_EXPORT_PARAM_NUM; i++) {
            ecnr_decoder_export_parameters[i] = (void *)p_export_parameters[i];
        }
        xSemaphoreGiveFromISR( ecnr_load_lib_xSemaphore, &xHigherPriorityTaskWoken );
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

#if defined(MTK_3RD_PARTY_NR)
#if defined(AIR_IGO_NR_ENABLE) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
static SemaphoreHandle_t igo_txnr_load_lib_xSemaphore = NULL;
extern void igo_txnr_default_function_parameter();

#define IGO_TXNR_EXPORT_PARAM_NUM (3)
void *igo_txnr_export_parameters[IGO_TXNR_EXPORT_PARAM_NUM] = {
    igo_txnr_default_function_parameter,
    igo_txnr_default_function_parameter,
    igo_txnr_default_function_parameter,
};
#if PIC_LOGPRINT
#if defined(AIR_IGO_NR_ENABLE)
void *igo_txnr_import_parameters[] = {printf, hal_trng_init, hal_trng_get_generated_random_number, hal_trng_deinit};
#endif
#if defined(AIR_IGO_NR_PLUS_1_ENABLE)
void *igo_txnr_import_parameters[] = {printf, hal_trng_init, hal_trng_get_generated_random_number, hal_trng_deinit};
#endif
#else
#if defined(AIR_IGO_NR_ENABLE)
void *igo_txnr_import_parameters[] = {fake_printf, hal_trng_init, hal_trng_get_generated_random_number, hal_trng_deinit};
#endif
#if defined(AIR_IGO_NR_PLUS_1_ENABLE)
void *igo_txnr_import_parameters[] = {fake_printf, hal_trng_init, hal_trng_get_generated_random_number, hal_trng_deinit};
#endif

#endif
void igo_txnr_default_function_parameter()
{
    DSP_MW_LOG_E("function point is NULL!!! igo_txnr library not load or had been unload!!!", 0);
    assert(0);
}

static preloader_pisplit_handle_t p_igo_txnr_pisplit_library_handle = NULL;
static uint32_t igo_txnr_lib_loaded_counter = 0;

void igo_txnr_load_library_callback(preloader_pisplit_handle_t handle)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t *p_export_parameters;
    p_export_parameters = preloader_pisplit_get_export_parameter(handle, igo_txnr_import_parameters);

    if(p_export_parameters == NULL)
    {
       DSP_MW_LOG_E("p_export_parameters is NULL, please check!!!", 0);
    }
    else
    {
        for (uint32_t i = 0; i < IGO_TXNR_EXPORT_PARAM_NUM; i++) {
            igo_txnr_export_parameters[i] = (void *)p_export_parameters[i];
        }

        xSemaphoreGiveFromISR( igo_txnr_load_lib_xSemaphore, &xHigherPriorityTaskWoken );
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void igo_txnr_library_load()
{
    if (igo_txnr_load_lib_xSemaphore == NULL) {
        igo_txnr_load_lib_xSemaphore = xSemaphoreCreateBinary();

        if ( igo_txnr_load_lib_xSemaphore == NULL ) {
            assert(0);
        }
    }
    if (p_igo_txnr_pisplit_library_handle == NULL) {
        if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_igo_txnr_pisplit_library_handle,&pisplit_igo_txnr,igo_txnr_load_library_callback)) {
            DSP_MW_LOG_E("pisplit_igo_txnr preloader_pisplit_get_handle() error!!!!", 0);

            assert(0);
        }
    }
    if (igo_txnr_lib_loaded_counter == 0) {
        if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load(p_igo_txnr_pisplit_library_handle,PRELOADER_I_HIGH_PERFORMANCE,PRELOADER_D_HIGH_PERFORMANCE) ) {
            DSP_MW_LOG_E("pisplit_igo_txnr preloader_pisplit_load() error!!!!", 0);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_LOW_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_LOW_PERFORMANCE);
            assert(0);
        }
        igo_txnr_lib_loaded_counter = 1;
        if (xSemaphoreTake( igo_txnr_load_lib_xSemaphore, portMAX_DELAY) == pdTRUE) {
        }
        else {
            assert(0);
        }
        DSP_MW_LOG_I("igo_txnr_library_load finish, cnt:%d", 1, igo_txnr_lib_loaded_counter);
    }
    else {
        igo_txnr_lib_loaded_counter++;
        DSP_MW_LOG_I("igo_txnr_library_load again, cnt:%d", 1, igo_txnr_lib_loaded_counter);
    }
    vSemaphoreDelete(igo_txnr_load_lib_xSemaphore);
    igo_txnr_load_lib_xSemaphore = NULL;
}

void igo_txnr_library_unload()
{
    if (igo_txnr_lib_loaded_counter == 1) {
        if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_unload(p_igo_txnr_pisplit_library_handle)) {
            DSP_MW_LOG_E("pisplit_igo_txnr preloader_pisplit_unload() error!!!!", 0);
            assert(0);
        }
        for (uint32_t i = 0; i < IGO_TXNR_EXPORT_PARAM_NUM; i++) {
            igo_txnr_export_parameters[i] = igo_txnr_default_function_parameter;
        }
        //p_ecnr_pisplit_library_handle = NULL;
        igo_txnr_lib_loaded_counter = 0;
        DSP_MW_LOG_I("igo_txnr_library_unload finish", 0);
    }
    else {
        igo_txnr_lib_loaded_counter--;
        DSP_MW_LOG_I("igo_txnr_library_unload cnt:%d", 1, igo_txnr_lib_loaded_counter);
    }
}
#endif

#ifdef AIR_ECNR_POST_PART_ENABLE

void ecnr_post_ec_library_load(void)
{
#if defined(AIR_IGO_NR_ENABLE) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
    /* Load IGO before EC since the memory size of IGO is larger */
    igo_txnr_library_load();
#endif

    if (ecnr_load_lib_xSemaphore == NULL) {
        ecnr_load_lib_xSemaphore = xSemaphoreCreateBinary();
        if( ecnr_load_lib_xSemaphore == NULL ) {
            assert(0);
        }
    }
    if (p_ecnr_pisplit_library_handle == NULL) {
        if(preloader_pisplit_get_handle(&p_ecnr_pisplit_library_handle, &pisplit_post_ec, ecnr_load_library_callback) != PRELOADER_PISPLIT_XTLIB_NO_ERR) {
            DSP_MW_LOG_E("pisplit_ecnr preloader_pisplit_get_handle() error!!!!", 0);
            assert(0);
        }
    }
    if (ecnr_lib_loaded_counter == 0) {
        if (preloader_pisplit_load(p_ecnr_pisplit_library_handle, PRELOADER_I_HIGH_PERFORMANCE, PRELOADER_D_HIGH_PERFORMANCE) != PRELOADER_PISPLIT_XTLIB_NO_ERR) {
            DSP_MW_LOG_E("pisplit_ecnr preloader_pisplit_load() error!!!!", 0);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_LOW_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_LOW_PERFORMANCE);
            assert(0);
        }
        ecnr_lib_loaded_counter = 1;
        DSP_MW_LOG_I("ecnr_library_load finish", 0);
        if (xSemaphoreTake(ecnr_load_lib_xSemaphore, portMAX_DELAY) != pdTRUE) {
            assert(0);
        }
    } else {
        ecnr_lib_loaded_counter++;
    }

    vSemaphoreDelete(ecnr_load_lib_xSemaphore);
    ecnr_load_lib_xSemaphore = NULL;
}

void ecnr_post_ec_library_unload(void)
{
    uint32_t i;

#if defined(AIR_IGO_NR_ENABLE) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
    igo_txnr_library_unload();
#endif

    if (ecnr_lib_loaded_counter == 1) {
        if (preloader_pisplit_unload(p_ecnr_pisplit_library_handle) != PRELOADER_PISPLIT_XTLIB_NO_ERR) {
            DSP_MW_LOG_E("pisplit_ecnr preloader_pisplit_unload() error!!!!", 0);
            assert(0);
        }
        for (i = 0; i < ECNR_DECODER_EXPORT_PARAM_NUM; i++) {
            ecnr_decoder_export_parameters[i] = ecnr_default_function_parameter;
        }
        ecnr_lib_loaded_counter = 0;
        DSP_MW_LOG_I("ecnr_library_unload finish", 0);

    } else {
        ecnr_lib_loaded_counter--;
    }
}
#endif
#endif /*#if defined(MTK_3RD_PARTY_NR) && defined(AIR_IGO_NR_ENABLE)*/

void ecnr_library_load()
{
#if defined(MTK_3RD_PARTY_NR)
#if defined(AIR_IGO_NR_ENABLE) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
    igo_txnr_library_load(); /* Load IGO before EC since the memory size of IGO is larger */
#endif
#endif

    if (ecnr_load_lib_xSemaphore == NULL)
    {
        ecnr_load_lib_xSemaphore = xSemaphoreCreateBinary();

        if( ecnr_load_lib_xSemaphore == NULL )
        {
            assert(0);
        }
    }
    if(p_ecnr_pisplit_library_handle == NULL)
    {
        #ifdef MTK_INEAR_ENHANCEMENT
        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_ecnr_pisplit_library_handle,&pisplit_ecnr_inear,ecnr_load_library_callback))
        #elif defined(MTK_DUALMIC_INEAR)
        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_ecnr_pisplit_library_handle,&pisplit_ecnr_inear_v2,ecnr_load_library_callback))
        #elif defined(MTK_3RD_PARTY_NR)
        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_ecnr_pisplit_library_handle,&pisplit_ec_rxnr,ecnr_load_library_callback))
        #else
        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_ecnr_pisplit_library_handle,&pisplit_ecnr,ecnr_load_library_callback))
        #endif
        {
            DSP_MW_LOG_E("pisplit_ecnr preloader_pisplit_get_handle() error!!!!", 0);
            assert(0);
        }
    }
  if(ecnr_lib_loaded_counter == 0)
  {
        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load(p_ecnr_pisplit_library_handle,PRELOADER_I_HIGH_PERFORMANCE,PRELOADER_D_HIGH_PERFORMANCE) )
        {
            DSP_MW_LOG_E("pisplit_ecnr preloader_pisplit_load() error!!!!", 0);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_LOW_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_LOW_PERFORMANCE);
            assert(0);
        }
        ecnr_lib_loaded_counter = 1;
        DSP_MW_LOG_I("ecnr_library_load finish", 0);
        if(xSemaphoreTake( ecnr_load_lib_xSemaphore, portMAX_DELAY) == pdTRUE)
        {
        }
        else
        {
            assert(0);
        }

  }
  else
  {
        ecnr_lib_loaded_counter++;
        //printf("ecnr_library_load preloader_pisplit_load() had been loaded!!!");
  }
  vSemaphoreDelete(ecnr_load_lib_xSemaphore);
  ecnr_load_lib_xSemaphore = NULL;
}

void ecnr_library_unload()
{
#if defined(MTK_3RD_PARTY_NR)
#if defined(AIR_IGO_NR_ENABLE) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
    igo_txnr_library_unload();
#endif
#endif

    if(ecnr_lib_loaded_counter == 1)
    {

        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_unload(p_ecnr_pisplit_library_handle))
        {
            DSP_MW_LOG_E("pisplit_ecnr preloader_pisplit_unload() error!!!!", 0);
            assert(0);
        }
        for (uint32_t i = 0; i < ECNR_DECODER_EXPORT_PARAM_NUM; i++) {
            ecnr_decoder_export_parameters[i] = ecnr_default_function_parameter;
        }
        //p_ecnr_pisplit_library_handle = NULL;
        ecnr_lib_loaded_counter = 0;
        DSP_MW_LOG_I("ecnr_library_unload finish", 0);

    }
    else
    {
        ecnr_lib_loaded_counter--;
    }


}


