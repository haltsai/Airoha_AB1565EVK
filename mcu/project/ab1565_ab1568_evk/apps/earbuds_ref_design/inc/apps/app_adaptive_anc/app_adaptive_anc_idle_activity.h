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

#ifndef __UI_SHELL_ADAPTIVE_ANC_IDLE_ACTIVITY_H__
#define __UI_SHELL_ADAPTIVE_ANC_IDLE_ACTIVITY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "anc_control_api.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_ami.h"
#include "ui_shell_activity.h"
#include "ui_shell_manager.h"

#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2

typedef enum {
    APP_ADAPTIVE_ANC_IDLE = 0,
    APP_ADAPTIVE_ANC_SUEPENDING_ANC,
    APP_ADAPTIVE_ANC_STARTED
} app_adaptive_anc_state_t;

typedef enum {
    APP_ADAPTIVE_ANC_TRIGGER_VP,
    APP_ADAPTIVE_ANC_TERMINATED,
    APP_ADAPTIVE_ANC_ENDED
} app_adaptive_anc_audio_event_t;

typedef struct {
    uint8_t                         anc_enable;
    audio_anc_control_filter_id_t   anc_filter_id;
    audio_anc_control_type_t        anc_type;
    int16_t                         anc_runtime_gain;
    uint8_t                         support_hybrid_enable;
    audio_anc_control_misc_t        anc_control_misc;
} app_adaptive_anc_anc_state_t;

typedef struct {
    app_adaptive_anc_anc_state_t    anc_state;
    app_adaptive_anc_state_t        app_state;
    bt_sink_srv_state_t             sink_state;
    vendor_se_event_t               audio_event;
    bool                            in_ear;
    bool                            audio_on;
    bool                            is_rho;
    bool                            reject;
    bool                            aws_connect;
    bool                            peer_reject;
    bool                            anc_suspend;
} app_adaptive_anc_context_t;

app_adaptive_anc_state_t app_adaptive_anc_get_state(void);
#endif

bool app_adaptive_anc_idle_activity_proc(struct _ui_shell_activity *self,
                                         uint32_t event_group,
                                         uint32_t event_id,
                                         void *extra_data,
                                         size_t data_len);

#ifdef __cplusplus
}
#endif

#endif /* __UI_SHELL_ADAPTIVE_ANC_IDLE_ACTIVITY_H__ */

