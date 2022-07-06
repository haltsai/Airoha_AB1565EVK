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
 
#include "bt_sink_srv_a2dp.h"
#include "bt_sink_srv_aws_mce_a2dp.h"
#include "bt_sink_srv_avrcp.h"
//#include "bt_sink_srv_aws_a2dp.h"
#include "bt_sink_srv_utils.h"

/* A2DP callback function */
bt_status_t bt_sink_srv_music_a2dp_get_init_params(bt_a2dp_init_params_t *param)
{
    return bt_sink_srv_a2dp_get_init_params(param);
}


void bt_sink_srv_music_a2dp_common_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param)
{
    bt_sink_srv_mutex_lock();
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_sink_srv_report_id("[sink][music_cb]a2dp_common_ami_hdr role is %d", 1, role);
    if(role == BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_NONE) {
        bt_sink_srv_a2dp_ami_hdr(aud_id, msg_id, sub_msg, param);
    }
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
    else {
        extern void bt_sink_srv_aws_mce_a2dp_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param);
        
        bt_sink_srv_aws_mce_a2dp_ami_hdr(aud_id, msg_id, sub_msg, param);
    }
#endif
    bt_sink_srv_mutex_unlock();

}


void bt_sink_srv_music_aws_a2dp_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param)
{
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
    extern void bt_sink_srv_aws_mce_a2dp_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param);

    bt_sink_srv_aws_mce_a2dp_ami_hdr(aud_id, msg_id, sub_msg, param);
#endif
}


bt_status_t bt_sink_srv_music_a2dp_action_handler(bt_sink_srv_action_t action, void *param)
{
   return bt_sink_srv_a2dp_action_handler(action, param);
}


bt_status_t bt_sink_srv_music_a2dp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    return bt_sink_srv_a2dp_common_callback(msg, status, buffer);
}


/* AVRCP callback function */
bt_status_t bt_sink_srv_music_avrcp_action_handler(bt_sink_srv_action_t action, void *param)
{
    return bt_sink_srv_avrcp_action_handler(action, param);
}


#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
bt_status_t bt_sink_srv_aws_mce_music_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    return bt_sink_srv_aws_mce_a2dp_common_callback(msg, status, buffer);
}
#endif


bt_status_t bt_sink_srv_music_aws_a2dp_action_handler(bt_sink_srv_action_t action, void *param)
{
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
    return bt_sink_srv_aws_mce_a2dp_action_handler(action, param);
#endif

    return 0;
}


/* Pseudo device common callback handle */
void bt_sink_srv_music_play_handle(audio_src_srv_handle_t *handle)
{
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP == handle->type) {
        bt_sink_srv_a2dp_play(handle);
    }
#if defined(__BT_AWS_MCE_A2DP_SUPPORT__)
    else if (AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP == handle->type) {
        bt_sink_srv_aws_mce_a2dp_play(handle);
    }
#endif
    else {
        bt_sink_srv_report_id("[sink][music_cb]play(error)--type: %d", 1, handle->type);
    }
}


void bt_sink_srv_music_stop_handle(audio_src_srv_handle_t *handle)
{
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP == handle->type) {
        bt_sink_srv_a2dp_stop(handle);
    } 
#if defined(__BT_AWS_MCE_A2DP_SUPPORT__)
    else if (AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP == handle->type) {
        bt_sink_srv_aws_mce_a2dp_stop(handle);
    }
#endif
    else {
        bt_sink_srv_report_id("[sink][music_cb]stop(error)--type: %d", 1, handle->type);
    }
}


void bt_sink_srv_music_suspend_handle(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd)
{
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP == handle->type) {
        bt_sink_srv_a2dp_suspend(handle, int_hd);
    }
#if defined(__BT_AWS_MCE_A2DP_SUPPORT__)
    else if (AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP == handle->type) {
        bt_sink_srv_aws_mce_a2dp_suspend(handle, int_hd);
    }
#endif
    else {
        bt_sink_srv_report_id("[sink][music_cb]suspend(error)--type: %d", 1, handle->type);
    }
}


void bt_sink_srv_music_reject_handle(audio_src_srv_handle_t *handle)
{
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP == handle->type) {
        bt_sink_srv_a2dp_reject(handle);
    } 
#if defined(__BT_AWS_MCE_A2DP_SUPPORT__)
    else if (AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP == handle->type) {
        bt_sink_srv_aws_mce_a2dp_reject(handle);
    }
#endif
    else {
        bt_sink_srv_report_id("[sink][music_cb]reject(error)--type: %d", 1, handle->type);
    }
}


void bt_sink_srv_music_exception_handle(audio_src_srv_handle_t *handle, int32_t event, void *param)
{
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP == handle->type) {
        bt_sink_srv_a2dp_exception(handle, event, param);
    }
#if defined(__BT_AWS_MCE_A2DP_SUPPORT__)
    else if (AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP == handle->type) {
        bt_sink_srv_aws_mce_a2dp_exception(handle, event, param);
    }
#endif
    else {
        bt_sink_srv_report_id("[sink][music_cb]exception(error)--type: %d", 1, handle->type);
    }
}

