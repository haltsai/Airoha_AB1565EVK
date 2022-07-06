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

/* Includes ------------------------------------------------------------------*/
#ifdef BSP_SERIAL_FLASH_ENABLED

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* hal includes */
#include "hal.h"
#include "hal_log.h"
#include "bsp_flash.h"

#ifdef BSP_EXTERNAL_SERIAL_FLASH_ENABLED
#include "bsp_external_flash.h"
#include "bsp_external_flash_config.h"
#endif

#ifdef HAL_ESC_MODULE_ENABLED
#include "hal_esc.h"
#include "hal_esc_internal.h"
#endif

uint32_t g_bsp_flash_status = 0;

#define BSP_FLASH_IDLE  (0)
#define BSP_FLASH_BUSY  (1)

#define BSP_FLASH_CHECK_AND_SET_BUSY(busy_status)  \
do{ \
    uint32_t saved_mask; \
    hal_nvic_save_and_set_interrupt_mask(&saved_mask); \
    if(g_bsp_flash_status == BSP_FLASH_BUSY){ \
        busy_status = BSP_FLASH_STATUS_BUSY; \
    } else { \
        g_bsp_flash_status = BSP_FLASH_BUSY;  \
        busy_status = BSP_FLASH_STATUS_OK; \
    } \
       hal_nvic_restore_interrupt_mask(saved_mask); \
}while(0)

#define BSP_FLASH_SET_IDLE()   \
do{ \
    g_bsp_flash_status = BSP_FLASH_IDLE;  \
}while(0)


#define BSP_USE_MSG_ID_LOG
#ifdef BSP_USE_MSG_ID_LOG
    #define     BSP_LOG_I(msg, arg_cnt, ...)    log_hal_msgid_info("[BSP Flash]"msg, arg_cnt, ##__VA_ARGS__)
    #define     BSP_LOG_W(msg, arg_cnt, ...)    log_hal_msgid_warning("[BSP Flash]"msg, arg_cnt, ##__VA_ARGS__)
    #define     BSP_LOG_E(msg, arg_cnt, ...)    log_hal_msgid_error("[BSP Flash]"msg, arg_cnt, ##__VA_ARGS__)
#else
    #define     BSP_LOG_I(msg, arg_cnt, ...)    log_hal_info("[BSP Flash]"msg, ##__VA_ARGS__)
    #define     BSP_LOG_W(msg, arg_cnt, ...)    log_hal_warning("[BSP Flash]"msg, ##__VA_ARGS__)
    #define     BSP_LOG_E(msg, arg_cnt, ...)    log_hal_error("[BSP Flash]"msg, ##__VA_ARGS__)
#endif


static bool bsp_curr_is_in_task(void)
{
    bool result = true;
#ifdef HAL_NVIC_MODULE_ENABLED
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (HAL_NVIC_QUERY_EXCEPTION_NUMBER > HAL_NVIC_NOT_EXCEPTION){
        result = false;
    } else {
        result = true;
    }
    hal_nvic_restore_interrupt_mask(mask);
#endif
    return result;
}


#if !defined(__UBL__) && defined(FREERTOS_ENABLE)
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
static SemaphoreHandle_t g_bsp_flash_mutex = NULL;

void bsp_flash_mutex_creat(void)
{
    if (g_bsp_flash_mutex != NULL) {
        BSP_LOG_E("bsp_flash_mutex already creat!", 0);
        return;
    }

    g_bsp_flash_mutex = xSemaphoreCreateMutex();

    if (g_bsp_flash_mutex == NULL) {
        BSP_LOG_E("bsp_flash_mutex_creat error\r\n", 0);
        return;
    }

    BSP_LOG_I("bsp_flash_mutex_creat successfully", 0);
}

void bsp_flash_mutex_take(void)
{
    assert(bsp_curr_is_in_task());
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        if (xSemaphoreTake(g_bsp_flash_mutex, portMAX_DELAY) == pdFALSE) {
            BSP_LOG_E("bsp_flash_mutex_take error\r\n", 0);
            return;
        }
        BSP_LOG_I("bsp_flash_mutex_take successfully", 0);
    }
}

void bsp_flash_mutex_give(void)
{
    assert(bsp_curr_is_in_task());
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        if (xSemaphoreGive(g_bsp_flash_mutex) == pdFALSE) {
            BSP_LOG_E("bsp_flash_mutex_give error\r\n", 0);
            return;
        }
        BSP_LOG_I("bsp_flash_mutex_give successfully", 0);
    }
}

void bsp_flash_mutex_delete(void)
{
    if (g_bsp_flash_mutex != NULL) {
        vSemaphoreDelete(g_bsp_flash_mutex);
        g_bsp_flash_mutex = NULL;
    }
}
#else
void bsp_flash_mutex_creat(void)
{}
void bsp_flash_mutex_take(void)
{
    assert(bsp_curr_is_in_task());
}
void bsp_flash_mutex_give(void)
{
    assert(bsp_curr_is_in_task());
}
void bsp_flash_mutex_delete(void)
{}
#endif


bsp_flash_status_t bsp_flash_init(void)
{
    bsp_flash_status_t result = BSP_FLASH_STATUS_OK;
    if (g_bsp_flash_status == BSP_FLASH_IDLE) {
        hal_flash_init();

#ifdef BSP_EXTERNAL_SERIAL_FLASH_ENABLED
        bsp_flash_mutex_creat();
        result = (bsp_flash_status_t)bsp_external_flash_init(SPIM_PORT, SPIM_FREQUNCY);
        if (result != BSP_FLASH_STATUS_OK) {
            BSP_LOG_E("bsp external flash init returned %d \r\n", 1, result);
            return result;
        }
#endif

#if defined(HAL_ESC_SUPPORT_FLASH) && defined(ESC_FLASH_ENABLE)
        result = (bsp_flash_status_t)hal_esc_init();
#endif
    }
    return result;
}

bsp_flash_status_t bsp_flash_read(uint32_t address, uint8_t *buffer, uint32_t length)
{
    bsp_flash_status_t result = BSP_FLASH_STATUS_OK;

#if defined(HAL_ESC_SUPPORT_FLASH) && defined(ESC_FLASH_ENABLE)
    if ((ESC_GENERIC_SRAM_BANK_MASK & address) == ESC_GENERIC_SRAM_BANK_MASK){
        hal_esc_read_flash_data(address, buffer, length);
    } else
#endif /* defined(HAL_ESC_SUPPORT_FLASH) && defined(ESC_FLASH_ENABLE) */

#ifdef BSP_EXTERNAL_SERIAL_FLASH_ENABLED
    if ((SPI_SERIAL_FLASH_ADDRESS & address) == SPI_SERIAL_FLASH_ADDRESS) {
        BSP_FLASH_CHECK_AND_SET_BUSY(result);
        if (result != BSP_FLASH_STATUS_OK) {
            return result;
        }
        bsp_flash_mutex_take();
        result = (bsp_flash_status_t)bsp_external_flash_read(address & (~SPI_SERIAL_FLASH_ADDRESS), buffer, length);
        bsp_flash_mutex_give();
        BSP_FLASH_SET_IDLE();
    } else
#endif
    {
        result = (bsp_flash_status_t)hal_flash_read(address, buffer, length);
    }
    return result;
}


bsp_flash_status_t bsp_flash_erase(uint32_t address, bsp_block_size_type_t block_size)
{
    bsp_flash_status_t result = BSP_FLASH_STATUS_OK;

    BSP_LOG_W("erase addr: 0x%08X, type: %d begin \r\n", 2, address, block_size);

#if defined(HAL_ESC_SUPPORT_FLASH) && defined(ESC_FLASH_ENABLE)
    if ((ESC_GENERIC_SRAM_BANK_MASK & address) == ESC_GENERIC_SRAM_BANK_MASK){
        result = (bsp_flash_status_t)(hal_esc_erase_flash_block(address, (hal_esc_erase_type_t)block_size));
    } else
#endif /* defined(HAL_ESC_SUPPORT_FLASH) && defined(ESC_FLASH_ENABLE) */

#ifdef BSP_EXTERNAL_SERIAL_FLASH_ENABLED
    if ((SPI_SERIAL_FLASH_ADDRESS & address) == SPI_SERIAL_FLASH_ADDRESS) {
        BSP_FLASH_CHECK_AND_SET_BUSY(result);
        if (result != BSP_FLASH_STATUS_OK) {
            return result;
        }
        bsp_flash_mutex_take();
        result = (bsp_flash_status_t)bsp_external_flash_erase(address & (~SPI_SERIAL_FLASH_ADDRESS), block_size);
        bsp_flash_mutex_give();
        BSP_FLASH_SET_IDLE();
    } else
#endif
    {
        result = (bsp_flash_status_t)hal_flash_erase(address, block_size);
    }
    BSP_LOG_W("erase addr: 0x%08X, type: %d, ret: %d end \r\n", 3, address, block_size, result);
    return result;
}

bsp_flash_status_t bsp_flash_write(uint32_t address, uint8_t *data, int32_t length)
{
    bsp_flash_status_t result = BSP_FLASH_STATUS_OK;

    BSP_LOG_W("write addr: 0x%08X, data addr: 0x%08X, length: %d begin \r\n", 3, address, data, length);
#if defined(HAL_ESC_SUPPORT_FLASH) && defined(ESC_FLASH_ENABLE)
    if ((ESC_GENERIC_SRAM_BANK_MASK & address) == ESC_GENERIC_SRAM_BANK_MASK){
        result = (bsp_flash_status_t)(hal_esc_write_flash_data(address, data, length));
    } else
#endif /* defined(HAL_ESC_SUPPORT_FLASH) && defined(ESC_FLASH_ENABLE) */

#ifdef BSP_EXTERNAL_SERIAL_FLASH_ENABLED
    if ((SPI_SERIAL_FLASH_ADDRESS & address) == SPI_SERIAL_FLASH_ADDRESS) {
        BSP_FLASH_CHECK_AND_SET_BUSY(result);
        if (result != BSP_FLASH_STATUS_OK) {
            return result;
        }
        bsp_flash_mutex_take();
        result = (bsp_flash_status_t)bsp_external_flash_write(address & (~SPI_SERIAL_FLASH_ADDRESS), data, length);
        bsp_flash_mutex_give();
        BSP_FLASH_SET_IDLE();
    } else
#endif
    {
        result = (bsp_flash_status_t)hal_flash_write(address, data, length);
    }
    BSP_LOG_W("write addr: 0x%08X, data addr: 0x%08X, length: %d, ret: %d end \r\n", 4, address, data, length, result);
    return result;
}

bsp_flash_status_t bsp_flash_deinit(void)
{
    bsp_flash_mutex_delete();
    BSP_FLASH_SET_IDLE();
    return BSP_FLASH_STATUS_OK;
}

#endif//BSP_SERIAL_FLASH_ENABLED

