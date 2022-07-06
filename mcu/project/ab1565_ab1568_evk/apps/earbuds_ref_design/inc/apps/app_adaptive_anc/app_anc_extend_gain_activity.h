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

#ifndef __UI_SHELL_ANC_EXTEND_GAIN_ACTIVITY_H__
#define __UI_SHELL_ANC_EXTEND_GAIN_ACTIVITY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "anc_control_api.h"
#include "bt_system.h"
#include "ui_shell_activity.h"
#include "ui_shell_manager.h"

typedef enum {
    APP_EXTEND_GAIN_WIND_NOISE_CHANGE = 0,
    APP_EXTEND_GAIN_HOWLING_CONTROL,                // unused
    APP_EXTEND_GAIN_USER_UNAWARE_CHANGE,
    APP_EXTEND_GAIN_ENVIRONMENT_DETECTION_CHANGE,
    APP_EXTEND_GAIN_USER_UNAWARE_SET,
    APP_EXTEND_GAIN_ENVIRONMENT_DETECTION_AWS_SYNC,
    APP_EXTEND_GAIN_NOTIFY_WIND_NOISE_TO_PHONE_APP,
    APP_EXTEND_GAIN_NOTIFY_USER_UNAWARE_TO_PHONE_APP,
    APP_EXTEND_GAIN_NOTIFY_ENVIRONMENT_DETECTION_TO_PHONE_APP,
    APP_EXTEND_GAIN_USER_UNAWARE_INIT,
    APP_EXTEND_GAIN_ED_NOTIFY_ED_NOISE
} app_anc_extend_gain_event_t;

bool app_anc_extend_gain_activity_proc(struct _ui_shell_activity *self,
                                       uint32_t event_group,
                                       uint32_t event_id,
                                       void *extra_data,
                                       size_t data_len);

void app_anc_extend_gain_init_control_status(void);
void app_anc_extend_gain_query_status(void *param);


#ifdef __cplusplus
}
#endif

#endif /* __UI_SHELL_ANC_EXTEND_GAIN_ACTIVITY_H__ */

