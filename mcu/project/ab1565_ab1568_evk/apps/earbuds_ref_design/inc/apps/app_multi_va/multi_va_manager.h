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

/**
 * File: multi_va_manager.h
 *
 * Description: This file defines the function and types of multi_va_manager.c.
 *
 */

#ifndef __MULTI_VA_MANAGER_H__
#define __MULTI_VA_MANAGER_H__

#include "multi_ble_adv_manager.h"
#include <stdint.h>

/**
 * @brief Define The voice assistant type.
 */
#define MULTI_VA_TYPE_AMA       (0)     /**< AMA */
#define MULTI_VA_TYPE_GSOUND    (1)     /**< GSound */
#define MULTI_VA_TYPE_XIAOWEI   (2)     /**< Xiao Wei */
#define MULTI_VA_TYPE_XIAOAI    (3)     /**< XiaoAI */
#define MULTI_VA_TYPE_MAX_NUM   (4)     /**< The count of VA type. If current VA type >= the value, means not connect any VA. */
#define MULTI_VA_TYPE_UNKNOWN   (0xFF)  /**< The initialized value of current VA type. */
typedef uint32_t                multi_va_type_t;

/**
 * @brief This enum defines the return value of on_voice_assistant_type_switch() callback.
 */
typedef enum {
    MULTI_VA_SWITCH_OFF_WAIT_INACTIVE,      /**< The switch off action is not finished, need wait confirm. */
    MULTI_VA_SWITCH_OFF_SET_INACTIVE_DONE,  /**< The switch off action is done. */
} multi_va_switch_off_return_t;

/**
 * @brief This structure defines callback functions of a voice assistant .
 */
typedef struct {
    /**
     * @brief      This function defines the callback for initializing voice assistant.
     * @param[in]  selected, true if current VA is this type.
     */
    void (*voice_assistant_initialize)(bool selected);
    /**
     * @brief      This function defines the of callback for switch VA type.
     * @param[in]  selected, true means switch to this VA type, false means switch from this VA type to other VA type.
     * @return     if switch completed synchronously, return MULTI_VA_SWITCH_OFF_SET_INACTIVE_DONE, else return MULTI_VA_SWITCH_OFF_SET_INACTIVE_DONE.
     */
    multi_va_switch_off_return_t (*on_voice_assistant_type_switch)(bool selected);
    /**
     * @brief      This function defines the callback to get adv information, refer to get_ble_adv_data_func_t.
    */
    get_ble_adv_data_func_t on_get_ble_adv_data;
} multi_va_manager_callbacks_t;

/**
 * @brief      This function start multi_va_manager module.
 */
void multi_va_manager_start(void);

/**
 * @brief      This function get current VA type.
 * @return     VA type.
 */
multi_va_type_t multi_va_manager_get_current_va_type(void);

/**
 * @brief      This function get if VA is connected.
 * @return     true when VA is connected.
 */
bool multi_va_manager_get_current_va_connected(void);

/**
 * @brief      This function register the callbacks for a VA type.
 * @param[in]  va_type, the VA type
 * @param[in]  p_callbacks, the pointer to the struct of callbacks.
 */
void multi_voice_assistant_manager_register_instance(
    multi_va_type_t va_type,
    const multi_va_manager_callbacks_t *p_callbacks);

/**
 * @brief      This function is the VA module notify VA is connected.
 * @param[in]  va_type, the VA type
 */
void multi_voice_assistant_manager_notify_va_connected(multi_va_type_t va_type);

/**
 * @brief      This function is the VA module notify VA is disconnected.
 * @param[in]  va_type, the VA type
 */
void multi_voice_assistant_manager_notify_va_disconnected(multi_va_type_t va_type);

/**
 * @brief      This function is the VA module notify VA inactive action is done.
 * @param[in]  va_type, the VA type
 */
void multi_voice_assistant_manager_set_inactive_done(multi_va_type_t va_type);

/**
 * @brief      This function start to add or remove BLE adv for VA.
 * @param[in]  enable, true means add BLE adv, false means remove BLE adv
 */
bool multi_voice_assistant_manager_enable_adv(bool enable);

/* The functions below is called by app_multi_va_idle_activity only. */
#if defined(MULTI_VA_SUPPORT_COMPETITION) && defined(MTK_AWS_MCE_ENABLE)
/**
 * @brief      This function trigger sending VA type from agent to partner.
 */
void multi_va_manager_send_va_type_to_partner(void);

/**
 * @brief      This function is called when received the VA type sync from agent.
 */
void multi_va_manager_receive_va_change_from_agent(void *p_va_type);

/**
 * @brief      This function is called when received reboot request from agent.
 */
void multi_va_manager_receive_reboot_from_agent(void);

/**
 * @brief      This function is called when agent is disconnected from partner.
 */
void multi_va_manager_on_partner_detached(void);
#endif

/**
 * @brief      This function is called when VA configured by smart phone APP.
 */
void multi_voice_assistant_manager_va_config_changed(void);

/**
 * @brief      This function notify current VA remove pairing. It is called before factory reset.
 */
multi_va_switch_off_return_t multi_voice_assistant_manager_va_remove_pairing(void);

#endif /* __MULTI_VA_MANAGER_H__ */
