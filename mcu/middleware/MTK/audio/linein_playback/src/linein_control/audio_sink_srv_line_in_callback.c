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

#include "audio_sink_srv_line_in_internal.h"

void audio_sink_srv_line_in_common_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param)
{
    //bt_sink_srv_mutex_lock();
    audio_src_srv_report("[sink][Line-IN_cb]line_in_common_ami_hdr", 1);
    //if(role == BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_NONE) {
    audio_sink_srv_line_in_ami_hdr(aud_id, msg_id, sub_msg, param);
    //}
    //bt_sink_srv_mutex_unlock();
}

audio_sink_srv_line_in_status_t audio_sink_srv_line_in_control_action_handler(audio_sink_srv_line_in_action_t action, void *param)
{
   return audio_sink_srv_line_in_action_handler(action, param);
}

/* Pseudo device common callback handle */
void audio_sink_srv_line_in_play_handle(audio_src_srv_handle_t *handle)
{
    audio_src_srv_report("[sink][Line-IN]play", 0);
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_LINE_IN == handle->type) {
        audio_sink_srv_line_in_play(handle);
    } else {
        audio_src_srv_report("[sink][Line-IN]play(error)--type: %d", 1, handle->type);
    }
}


void audio_sink_srv_line_in_stop_handle(audio_src_srv_handle_t *handle)
{
    audio_src_srv_report("[sink][Line-IN]stop", 0);
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_LINE_IN == handle->type) {
        audio_sink_srv_line_in_stop(handle);
    } else {
        audio_src_srv_report("[sink][Line-IN]stop(error)--type: %d", 1, handle->type);
    }
}


void audio_sink_srv_line_in_suspend_handle(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd)
{
    audio_src_srv_report("[sink][Line-IN]suspend", 0);
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_LINE_IN == handle->type) {
        audio_sink_srv_line_in_suspend(handle, int_hd);
    } else {
        audio_src_srv_report("[sink][Line-IN]suspend(error)--type: %d", 1, handle->type);
    }
}


void audio_sink_srv_line_in_reject_handle(audio_src_srv_handle_t *handle)
{
    audio_src_srv_report("[sink][Line-IN]reject", 0);
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_LINE_IN == handle->type) {
        audio_sink_srv_line_in_reject(handle);
    } else {
        audio_src_srv_report("[sink][Line-IN]reject(error)--type: %d", 1, handle->type);
    }
}


void audio_sink_srv_line_in_exception_handle(audio_src_srv_handle_t *handle, int32_t event, void *param)
{
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_LINE_IN == handle->type) {
        audio_sink_srv_line_in_exception(handle, event, param);
    } else {
        audio_src_srv_report("[sink][Line-IN]exception(error)--type: %d", 1, handle->type);
    }
}

