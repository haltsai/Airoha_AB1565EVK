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

#include "hal_dvfs.h"

#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs_internal.h"
#include "hal_ccni.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "hal_uart.h"
#include "hal_log.h"
#include "hal_gpt.h"

#include <stdio.h>
#include <xtensa/hal.h>
#include <xtensa/xtruntime.h>

static const uint32_t dvfs_vcore_frequency[DVFS_VCORE_MODE_MAX_NUM] = {
    52000,156000,312000
};

void dvfs_set_register_value(uint32_t address, short int mask, short int shift, short int value) {
    uint32_t mask_buffer,target_value;
    mask_buffer = (~(mask << shift));
    target_value = *((volatile uint32_t *)(address));
    target_value &= mask_buffer;
    target_value |= (value << shift);
    *((volatile uint32_t *)(address)) = target_value;
}

uint32_t dvfs_get_register_value(uint32_t address, short int mask, short int shift) {
    uint32_t change_value, mask_buffer;
    mask_buffer = (mask << shift);
    change_value = *((volatile uint32_t *)(address));
    change_value &=mask_buffer;
    change_value = (change_value>> shift);
    return change_value;
}

//====[DSP0 CCNI api]===
void dvfs_sent_ccni_to_cm4(uint8_t mode, uint8_t index, dvfs_lock_parameter_t lock) {
    hal_ccni_status_t ret;
    uint32_t status;
    hal_ccni_event_t event = CCNI_DSP0_TO_CM4_DVFS;
    uint32_t msg_array[3];
    switch(mode) {
        case 0: // normal change frequency
        msg_array[0] = index;
        msg_array[1] = 0;
        break;
        case 1 :// change frequency lock frequency
        if(lock == DVFS_LOCK) {
            msg_array[0] = index;
            msg_array[1] = 0x3;
        } else {
            msg_array[0] = index;
            msg_array[1] = 0x4;
        }
        break;
    }
    ret = hal_ccni_query_event_status(event, &status);
    if (HAL_CCNI_STATUS_OK != ret) {
        log_hal_msgid_error("[DVFS] DSP0 CCNI query fail\r\n", 0);
    }
    if (status != HAL_CCNI_EVENT_STATUS_IDLE) {
        log_hal_msgid_error("[DVFS] DSP0 CCNI busy :fail\r\n", 0);
        return HAL_CCNI_STATUS_BUSY;
    }
    ret = hal_ccni_set_event(event, msg_array);
    if (HAL_CCNI_STATUS_OK != ret) {
        log_hal_msgid_error("[DVFS] DSP0 Error handler\r\n", 0);
    }
}

void dvfs_dps0_receive(hal_ccni_event_t event, void *msg) {
    hal_ccni_status_t status;
    status = hal_ccni_mask_event(event);
    uint32_t *pMsg = (uint32_t *)msg;
    status = hal_ccni_clear_event(event);
    status = hal_ccni_unmask_event(event);
}

hal_dvfs_status_t dvfs_lock_control(uint32_t mode,dvfs_frequency_t freq,dvfs_lock_parameter_t lock) {
    dvfs_sent_ccni_to_cm4(mode,freq,lock);
    return HAL_DVFS_STATUS_OK;
}
//====[DVFS Basic api]===
hal_dvfs_status_t hal_dvfs_get_cpu_frequency_list(const uint32_t **list, uint32_t *list_num) {
    *list = dvfs_vcore_frequency;
    *list_num = DVFS_VCORE_MODE_MAX_NUM;
    return HAL_DVFS_STATUS_OK;
}
hal_dvfs_status_t hal_dvfs_target_cpu_frequency(uint32_t target_freq, hal_dvfs_freq_relation_t relation) {
    switch (relation) {
        case HAL_DVFS_FREQ_RELATION_L:
        if((target_freq-1) <= 0) {
            target_freq=0;
        } else {
            target_freq = target_freq-1;
        }
        break;
        case HAL_DVFS_FREQ_RELATION_H:
        break;
    }
    dvfs_sent_ccni_to_cm4(0,target_freq,0);
    return HAL_DVFS_STATUS_OK;
}
uint32_t hal_dvfs_get_cpu_frequency(void) {
    uint8_t val = dvfs_get_register_value(HW_SYSRAM_PRIVATE_MEMORY_DVFS_STATUS_START,DVFS_VOL_FREQ_MASK,DVFS_FREQUENCY_SHIFT);
    if(val==0) {
        return dvfs_vcore_frequency[0];
    } else if(val==1) {
        return dvfs_vcore_frequency[1];
    } else if(val==3) {
        return dvfs_vcore_frequency[2];
    } else {
        log_hal_msgid_error("[DVFS Warning] : get cpu frequency fail\r\n", 0);
        return 0;
    }
}

#endif /* HAL_DVFS_MODULE_ENABLED */

