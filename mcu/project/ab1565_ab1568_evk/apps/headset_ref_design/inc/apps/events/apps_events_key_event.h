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
 * File: apps_events_key_event.h
 *
 * Description: This file defines the interface and type of apps_events_key_event.c.
 *
 */

#ifndef __APP_EVENTS_KEY_EVENT_H__
#define __APP_EVENTS_KEY_EVENT_H__

#include "airo_key_event.h"
#ifdef AIR_ROTARY_ENCODER_ENABLE
#include "bsp_rotary_encoder.h"
#endif

#define INVALID_KEY_EVENT_ID        (0xFFFF)    /* The key event is not real, it's a simulated key event. */

/**
 * @brief      Initialize the key event, register callback.
 */
void apps_event_key_event_init(void);

/**
 * @brief      Decode the key_id and key_event from a uint32_t value.
 * @param[out] key_id, means which key, ex. DEVICE_KEY_POWER or EINT_KEY_0.
 * @param[out] key_event, means the key event type, refer to airo_key_event_t.
 * @param[in]  event_id, the event_id of ui_shell event, it is a uint32_t and contains key_id and key_event.
 */
void app_event_key_event_decode(uint8_t *key_id, airo_key_event_t *key_event, uint32_t event_id);


#ifdef AIR_ROTARY_ENCODER_ENABLE
/**
 * @brief      Decode the rotary port, event and event_data from a uint32_t value.
 * @param[out] p_port, is the pointer to which rotary port, refer to bsp_rotary_encoder_port_t.
 * @param[out] p_event, is the rotary event type, refer to bsp_rotary_encoder_event_t.
 * @param[out] p_rotary_data, is the rotary event data.
 * @param[in]  event_id, the event_id of ui_shell event, it is a uint32_t and contains port, event and event_data.
 */
void app_event_rotary_event_decode(bsp_rotary_encoder_port_t *p_port, bsp_rotary_encoder_event_t *p_event, uint32_t *p_rotary_data, uint32_t event_id);
#endif

#endif /* __APP_EVENTS_KEY_EVENT_H__ */
