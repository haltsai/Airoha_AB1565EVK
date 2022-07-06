
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
 * File: app_va_xiaoai_device_config.h
 *
 * Description: This file defines the interface of app_va_xiaoai_device_config.c.
 *
 */

#ifndef __APP_VA_XIAOAI_DEVICE_CONFIG_H__
#define __APP_VA_XIAOAI_DEVICE_CONFIG_H__

#ifdef MTK_VA_XIAOAI_ENABLE

#include "stdbool.h"
#include "stdint.h"
#include "bt_customer_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define XIAOAI_BT_NAME_LEN                  (BT_GAP_LE_MAX_DEVICE_NAME_LENGTH)

/* ANC handle event. */
void     app_va_xiaoai_handle_anc_event(bool on_event, void *extra_data);

/* Get ANC state for MIUI HFP AT CMD. */
uint8_t app_va_xiaoai_get_anc_state_for_miui();

/* Get WWE state. */
bool app_va_xiaoai_is_enable_wwe();

/* Get voice_recognition_state. */
uint8_t app_va_xiaoai_get_voice_recognition_state();

/* Get eq state. */
uint8_t app_va_xiaoai_get_eq_mode();

/* Get game mode for MIUI HFP AT CMD. */
uint8_t app_va_xiaoai_get_game_mode_for_miui();

/* Get anti_lost state. */
uint8_t app_va_xiaoai_get_anti_lost_state();

#ifdef MTK_LEAKAGE_DETECTION_ENABLE
void    app_va_xiaoai_notify_leakage_detectable(bool detectable);
void    app_va_xiaoai_agent_handle_partner_leakage_result(uint8_t leakage_result);
bool    app_va_xiaoai_is_ld_ongoing();
#endif

bool    app_va_xiaoai_agent_set_device_name(uint8_t *name, uint8_t len);
void    app_va_xiaoai_partner_set_device_name(uint8_t *name, uint8_t len);

void    app_va_xiaoai_partner_handle_saving_config(uint8_t *data);

void    app_va_xiaoai_handle_chat_event(bool detect_chat);

/* Device Config SET - 0x08/0xF2. */
void    app_va_xiaoai_set_device_config(void *param);

/* Device Config GET - 0x09/0xF3. */
uint32_t app_va_xiaoai_get_device_config(uint8_t type);

/* Device Config Notify - 0xF4. */
void     app_va_xiaoai_notify_device_config(uint8_t type, uint8_t *data, uint8_t len);

void     app_va_xiaoai_init_device_config();

#ifdef __cplusplus
}
#endif

#endif /* MTK_XIAOAI_ENABLED */

#endif /* __APP_VA_XIAOAI_DEVICE_CONFIG_H__ */

