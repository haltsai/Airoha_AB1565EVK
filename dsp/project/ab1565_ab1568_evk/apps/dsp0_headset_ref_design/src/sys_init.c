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

#include <stdint.h>
#include <stdio.h>
#include <xtensa/hal.h>
#include <xtensa/xtruntime.h>
#include "hal.h"
#include "syslog.h"

/**
* @brief       This function is to initialize cache controller.
* @param[in]   None.
* @return      None.
*/
static void cache_init(void)
{
    hal_cache_region_t region, region_number;

    /* Max region number is 8 */
    hal_cache_region_config_t region_cfg_tbl[] = {
        /* TODO: use Macro in feature */
        {0x00000000, 0x20000000}
    };

    region_number = (hal_cache_region_t)(sizeof(region_cfg_tbl) / sizeof(region_cfg_tbl[0]));

    if (HAL_CACHE_STATUS_OK != hal_cache_init()) {
        LOG_MSGID_I(common,"Cache Init Failed\r\n",0);
        return;
    }

    if (HAL_CACHE_STATUS_OK != hal_cache_set_size(HAL_CACHE_SIZE_16KB)) {
        LOG_MSGID_I(common,"Cache Set Size Failed\r\n",0);
        return;
    }

    for (region = HAL_CACHE_REGION_0; region < region_number; region++) {
        if (HAL_CACHE_STATUS_OK == hal_cache_region_config(region, &region_cfg_tbl[region])) {
            if (HAL_CACHE_STATUS_OK != hal_cache_region_enable(region)) {
                LOG_MSGID_I(common,"Cache Region%d Enable Failed\r\n", 1,region);
            }
        } else {
            LOG_MSGID_I(common,"Cache Region%d Config Failed\r\n",1, region);
        }
    }

    if (HAL_CACHE_STATUS_OK != hal_cache_enable()) {
        LOG_MSGID_I(common,"Cache Enable Failed\r\n",0);
        return;
    }
}

/**
* @brief       This function is to do system initialization, eg: system clock, hardware and logging port.
* @param[in]   None.
* @return      None.
*/
void system_init(void)
{
    /* Call this function to indicate the system initialize is ongoing. */
    hal_core_status_write(HAL_CORE_DSP0, HAL_CORE_INIT);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    /* sleep module init */
    hal_sleep_manager_init();
#endif

    /* cache module init */
    cache_init();

    /* nvic module init */
    hal_nvic_init();

    /* ccni module init */
    hal_ccni_init();

    /* log set filter */
    log_set_filter();

    /* sys init done log */
    LOG_I(common, "[DSP0]system initialize done[%s]", build_date_time_str);
}
