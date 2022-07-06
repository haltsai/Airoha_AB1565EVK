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

/**
 * File: app_ull_idle_activity.h
 *
 * Description: This file defines the interface of app_ull_idle_activity.c.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#ifndef __UI_SHELL_ULL_IDLE_ACTIVITY_H__
#define __UI_SHELL_ULL_IDLE_ACTIVITY_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "ui_shell_activity.h"
#include "ui_shell_manager.h"
#include "bt_type.h"

enum {
    ULL_AUX_USB_AUDIO_STATE_NONE,
    ULL_AUX_USB_AUDIO_STATE_IN,
    ULL_AUX_USB_AUDIO_STATE_OUT,
};
typedef uint8_t ull_aux_or_usb_audio_in_state_t;

typedef struct {
    bt_bd_addr_t dongle_bt_address;
    uint8_t link_mode;                  /** Single or multi mode */
#ifdef AIR_APP_ULL_GAMING_MODE_UI_ENABLE
    uint8_t game_mode;
#endif
    uint8_t current_mix_ratio_level;
    uint8_t mic_vol;
    ull_aux_or_usb_audio_in_state_t aux_state;
    ull_aux_or_usb_audio_in_state_t usb_audio_state;
    bool mic_mute;
    bool adv_paused;
} app_ull_context_t;


/**
* @brief      This function is the interface of the app_ull_idle_activity, and is only called by ui_shell when events are sent.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_group, the current event group to be handled.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool app_ull_idle_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len);

/**
* @brief      This function is the BT come to check if allow a2dp profile connect.
* @param[in]  address, the address of the connecting device.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, means allow a2dp connect.
*/
bool app_ull_idle_activity_allow_a2dp_connect(const bt_bd_addr_t *address);

/**
* @brief      This function can get the context of the ull activity.
* @return     the pointer to the context.
*/
const app_ull_context_t *app_ull_idle_activity_get_current_context(void);

#ifdef __cplusplus
}
#endif

uint8_t app_ull_get_mic_vol(void);
uint8_t app_ull_get_mix_ratio(void);

bool app_ull_is_uplink_open();

#endif /* __UI_SHELL_ULL_IDLE_ACTIVITY_H__ */

