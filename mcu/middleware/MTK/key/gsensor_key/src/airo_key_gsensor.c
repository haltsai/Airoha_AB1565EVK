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

#ifdef MTK_GSENSOR_KEY_ENABLE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "hal_log.h"
#include "bsp_gsensor_key.h"
#include "airo_key_event_internal.h"
#include "hal_gpt.h"

airo_key_gsensor_config_t airo_key_gsensor_config = {AIRO_CONFIG_GSENSOR_KEY};

static void airo_key_gsensor_callback(uint8_t key_name, bsp_gsensor_key_event_t event, void *user_data)
{
    uint32_t time;
    airo_key_mapping_event_t key_event;

    if ((event == BSP_GSENSOR_KEY_SINGLE_TAP) || (event == BSP_GSENSOR_KEY_DOUBLE_TAP)) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time);
        if (event == BSP_GSENSOR_KEY_SINGLE_TAP) {
            key_event.state      = AIRO_KEY_DRIVEN_SINGLE_TAP;
        } else {
            key_event.state      = AIRO_KEY_DRIVEN_DOUBLE_TAP;
        }
        key_event.key_data   = key_name;
        key_event.time_stamp = time;
        airo_key_process_key(&key_event, AIRO_KEY_GSENSOR);
    }
}

bool airo_key_gsensor_init(void)
{
    bsp_gsensor_key_config_t config;

    hal_gpt_status_t ret;

    ret = hal_gpt_sw_get_timer(&airo_gsensor_state.timer_handle);
    if (ret != HAL_GPT_STATUS_OK) {
        LOG_MSGID_I(common, "[airo_key][gsensor]get sw timer fail,handle=0x%x\r\n",1, ret);
    }

    config.callback.callback    = airo_key_gsensor_callback;
    config.callback.user_data   = NULL;

    config.i2c.port             = AIRO_GSENSOR_KEY_I2C_PORT;
    config.i2c.conifg.frequency = HAL_I2C_FREQUENCY_400K;

    config.key_data = airo_key_gsensor_config.key_data;

    bsp_gsenosr_key_init(&config);

    return true;
}



#endif /*MTK_GSENSOR_KEY_ENABLE*/

