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

#include "hal_esc.h"
#include "hal_esc_internal.h"

#ifdef HAL_ESC_MODULE_ENABLED

#include <memory_attribute.h>

/* hal includes */
#include "hal.h"
#include "hal_log.h"
#include "hal_clock.h"
#include "esc_device_config.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED
extern sleep_management_lock_request_t g_esc_sleep_handle;
#endif /* HAL_SLEEP_MANAGER_ENABLED */

ATTR_TEXT_IN_TCM hal_esc_status_t hal_esc_init(void)
{
#ifdef ESC_FLASH_ENABLE
    bool device_is_support = false;

    if (get_initial_state() == true) {
        return HAL_ESC_STATUS_OK;
    }

    hal_clock_enable(HAL_CLOCK_CG_ESC);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_lock_sleep(g_esc_sleep_handle);
#endif
    reset_esc_register();
    device_is_support = esc_judge_device();
    if (device_is_support) {
        esc_internal_init();
        set_initial_state(true);
#ifdef HAL_SLEEP_MANAGER_ENABLED
        hal_sleep_manager_unlock_sleep(g_esc_sleep_handle);
#endif
        return HAL_ESC_STATUS_OK;
    } else {
        log_hal_msgid_warning("unsupport ESC device\r\n", 0);
        set_initial_state(false);
#ifdef HAL_SLEEP_MANAGER_ENABLED
        hal_sleep_manager_unlock_sleep(g_esc_sleep_handle);
#endif
        return HAL_ESC_STATUS_ERROR;
    }
#endif /* ESC_FLASH_ENABLE */

#ifdef ESC_PSRAM_ENABLE
    hal_clock_enable(HAL_CLOCK_CG_ESC);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    /* Can't has unlock sleep action here,
     * otherwise the MCU cannot directly access the data of ESC PSRAM! ! ! */
    hal_sleep_manager_lock_sleep(g_esc_sleep_handle);
#endif
    esc_internal_init();
    return HAL_ESC_STATUS_OK;
#endif /* ESC_PSRAM_ENABLE */
}


ATTR_TEXT_IN_TCM hal_esc_status_t hal_esc_deinit(void)
{
    return HAL_ESC_STATUS_OK;
}

#endif

