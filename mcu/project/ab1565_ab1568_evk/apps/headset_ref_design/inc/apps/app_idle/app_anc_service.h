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

#ifndef __APP_ANC_SERVICE_H__
#define __APP_ANC_SERVICE_H__

#ifdef MTK_ANC_ENABLE
/**
 * File: app_anc_service.h
 *
 * Description: This file defines the interface of app_anc_service.c.
 *
 */
#include <stdbool.h>
#include <stdint.h>
#ifdef MTK_ANC_V2
#include "anc_control_api.h"
#else
#include "anc_control.h"
#endif

typedef struct {
    audio_anc_control_filter_id_t       cur_filter_id;
    audio_anc_control_type_t            cur_type;
    int16_t                             cur_runtime_gain;
} app_anc_srv_result_t;

/**
 * @brief      Initialize ANC service.
 */
void app_anc_service_init(void);

void app_anc_service_save_into_flash(void);

void app_anc_service_play_vp(void);

bool app_anc_service_is_enable(void);

bool app_anc_service_enable(audio_anc_control_filter_id_t filter_id,
                            audio_anc_control_type_t anc_type,
                            audio_anc_control_gain_t runtime_gain,
                            audio_anc_control_misc_t *control_misc);

bool app_anc_service_disable();

bool app_anc_service_set_runtime_gain(audio_anc_control_type_t anc_type,
                                      audio_anc_control_gain_t runtime_gain);

bool app_anc_service_suspend(void);

bool app_anc_service_resume(void);

bool app_anc_service_reinit_nvdm(void);

void app_anc_service_set_user_trigger_state(bool ongoing);

typedef enum {
    APP_ANC_STATE_DISABLE       = 0,
    APP_ANC_STATE_ANC_ENABLE,
    APP_ANC_STATE_PT_ENABLE
} app_anc_service_state_t;

app_anc_service_state_t app_anc_server_get_state(void);

#endif

#endif /* __APPS_EVENTS_AUDIO_EVENTS_H__ */
