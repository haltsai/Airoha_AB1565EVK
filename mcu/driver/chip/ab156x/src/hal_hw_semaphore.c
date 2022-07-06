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
 
#include "hal_hw_semaphore.h"
#include "hal_platform.h"
#include "hal_nvic.h"
#include "memory_attribute.h"
#ifdef HAL_HW_SEMAPHORE_MODULE_ENABLED

ATTR_TEXT_IN_TCM hal_hw_semaphore_status_t hw_semaphore_take(hal_hw_semaphore_id_t id);
ATTR_TEXT_IN_TCM hal_hw_semaphore_status_t hw_semaphore_give(hal_hw_semaphore_id_t id);


ATTR_TEXT_IN_TCM hal_hw_semaphore_status_t hal_hw_semaphore_take(hal_hw_semaphore_id_t id)
{
    hal_hw_semaphore_status_t g_status;
    if (id >= HAL_HW_SEMAPHORE_ID_MAX || id == HAL_HW_SEMAPHORE_ID_1) {
        return HAL_HW_SEMAPHORE_STATUS_INVALID_PARAMETER;
    }
    g_status = hw_semaphore_take(id);
    return g_status ; 
}

ATTR_TEXT_IN_TCM hal_hw_semaphore_status_t hal_hw_semaphore_give(hal_hw_semaphore_id_t id)
{
    hal_hw_semaphore_status_t g_status;
    if (id >= HAL_HW_SEMAPHORE_ID_MAX || id == HAL_HW_SEMAPHORE_ID_1) {
        return HAL_HW_SEMAPHORE_STATUS_INVALID_PARAMETER;
    }
    g_status = hw_semaphore_give(id);
    return g_status ; 
   
}

ATTR_TEXT_IN_TCM hal_hw_semaphore_channel_t hal_hw_semaphore_query_channel(hal_hw_semaphore_id_t id)
{
    if (id >= HAL_HW_SEMAPHORE_ID_MAX) {
        return HAL_HW_SEMAPHORE_STATUS_INVALID_PARAMETER;
    }

    if (id < 32) {
        if ((SMPH->GLOBAL_STATUS0 & (1 << id)) == 0) {
            return HAL_HW_SEMAPHORE_CHANNEL_NOT_EXIST;
        } else {
            if (SMPH->CH0_STATUS0 & (1 << id)) {
                return HAL_HW_SEMAPHORE_CHANNEL_CM4;
            } else if (SMPH->CH1_STATUS0 & (1 << id)) {
                return HAL_HW_SEMAPHORE_CHANNEL_DSP0;
            }
            return HAL_HW_SEMAPHORE_CHANNEL_NOT_EXIST;
        }
    } else {//id>32
        id -= 32;
        if ((SMPH->GLOBAL_STATUS1 & (1 << id)) == 0) {
            return HAL_HW_SEMAPHORE_CHANNEL_NOT_EXIST;
        } else {
            if (SMPH->CH0_STATUS1 & (1 << id)) {
                return HAL_HW_SEMAPHORE_CHANNEL_CM4;
            } else if (SMPH->CH1_STATUS1 & (1 << id)) {
                return HAL_HW_SEMAPHORE_CHANNEL_DSP0;
            }
            return HAL_HW_SEMAPHORE_CHANNEL_NOT_EXIST;
        }
    }
}

ATTR_TEXT_IN_TCM hal_hw_semaphore_status_t hw_semaphore_take(hal_hw_semaphore_id_t id)
{
    uint32_t int_mask ;
    hal_nvic_save_and_set_interrupt_mask(&int_mask);
    if (id < 32) {
        if ((SMPH->CH0_STATUS0 & (1 << id)) != 0) {
            hal_nvic_restore_interrupt_mask(int_mask);
            return HAL_HW_SEMAPHORE_STATUS_TAKE_ERROR;
        }
        SMPH->CH0_LOCK0 = (1 << id);
        hal_nvic_restore_interrupt_mask(int_mask);
        return ((SMPH->CH0_STATUS0 & (1 << id)) == 0) ? HAL_HW_SEMAPHORE_STATUS_TAKE_ERROR : HAL_HW_SEMAPHORE_STATUS_OK;
    } else {
        id -= 32;
        if ((SMPH->CH0_STATUS1 & (1 << id)) != 0) {
            hal_nvic_restore_interrupt_mask(int_mask);
            return HAL_HW_SEMAPHORE_STATUS_TAKE_ERROR;
        }
        SMPH->CH0_LOCK1 = (1 << id);
        hal_nvic_restore_interrupt_mask(int_mask);
        return ((SMPH->CH0_STATUS1 & (1 << id)) == 0) ? HAL_HW_SEMAPHORE_STATUS_TAKE_ERROR : HAL_HW_SEMAPHORE_STATUS_OK;
    }
}
ATTR_TEXT_IN_TCM hal_hw_semaphore_status_t hw_semaphore_give(hal_hw_semaphore_id_t id)
{
    uint32_t int_mask ;
    hal_nvic_save_and_set_interrupt_mask(&int_mask);
    if (id < 32) {
        if ((SMPH->CH0_STATUS0 & (1 << id)) == 0) {
            hal_nvic_restore_interrupt_mask(int_mask);
            return HAL_HW_SEMAPHORE_STATUS_GIVE_ERROR;
        }
        SMPH->CH0_RELEASE0 = (1 << id);
        hal_nvic_restore_interrupt_mask(int_mask);
        return ((SMPH->CH0_STATUS0 & (1 << id)) == 0) ? HAL_HW_SEMAPHORE_STATUS_OK : HAL_HW_SEMAPHORE_STATUS_GIVE_ERROR;
    } else {
        id -= 32;
        if ((SMPH->CH0_STATUS1 & (1 << id)) == 0) {
            hal_nvic_restore_interrupt_mask(int_mask);
            return HAL_HW_SEMAPHORE_STATUS_GIVE_ERROR;
        }
        SMPH->CH0_RELEASE1 = (1 << id);
        hal_nvic_restore_interrupt_mask(int_mask);
        return ((SMPH->CH0_STATUS1 & (1 << id)) == 0) ? HAL_HW_SEMAPHORE_STATUS_OK : HAL_HW_SEMAPHORE_STATUS_GIVE_ERROR;
    }
}

#endif
