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

#ifndef __APP_AMA_ACTIVITY_H__
#define __APP_AMA_ACTIVITY_H__

#include "bt_sink_srv.h"
#include "ui_shell_manager.h"
#include "ui_shell_activity.h"
#include "apps_debug.h"
#include "FreeRTOS.h"
#include "apps_events_interaction_event.h"
#include "BtAma.h"
#include "Ama_Interface.h"
#include "app_ama_multi_va.h"

#ifdef MTK_AMA_ENABLE

typedef enum {

    AMA_ACTIVITY_RECOGNITION_STATE_IDLE           = 0x00,
    AMA_ACTIVITY_RECOGNITION_STATE_STARTING       = 0x01,
    AMA_ACTIVITY_RECOGNITION_STATE_RECORDING      = 0x02,
    AMA_ACTIVITY_RECOGNITION_STATE_THINKING       = 0x04,
    AMA_ACTIVITY_RECOGNITION_STATE_SPEAKING       = 0x05,

} ama_activity_recognition_state_t;

typedef struct {
    BD_ADDR_T           context_addr;
    uint8_t             context_recognition_state;
    uint8_t             context_trigger_mode;
    bool                context_avrcp_override;
    bool                context_start_setup;
    bool                context_wake_word_detection_enable;
    bool                context_ptt_start_speech_result;
    bool                context_ptt_recorder_stopped;
} ama_context_t;

#define APP_AMA_ACTI "[AMA]app_ama_activity"

bool app_ama_multi_va_set_configuration(bool selected);

void app_ama_handle_get_locales_ind(AMA_GET_LOCALES_IND_T *get_locales_ind);
void app_ama_handle_set_locale_ind(AMA_SET_LOCALE_IND_T *set_locale_ind);

void app_ama_get_state_ind_handler(AMA_GET_STATE_IND_T *ind);
void app_ama_sync_state_ind_handler(AMA_SYNCHRONIZE_STATE_IND_T *ind);

void app_ama_set_connected_address(uint8_t *src_addr);
void app_ama_reset_connected_address();
bool app_ama_is_connected();
bool app_ama_is_valid_address(uint8_t *addr);
void app_ama_configure_wwe_parameter(uint8_t *language);

uint32_t app_ama_get_mtu();

bool app_ama_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len);

#endif
#endif

