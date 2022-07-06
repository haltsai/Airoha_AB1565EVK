/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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


#include "FreeRTOS.h"
#include "bt_type.h"

#include "bt_sink_srv_le_volume.h"

#include "ble_vcs.h"

#include "bt_sink_srv_ami.h"
#include "bt_sink_srv_le_cap_audio_manager.h"
#include "bt_sink_srv_le_cap_stream.h"
#include "bt_sink_srv_le_cap.h"

#include "bt_le_audio_msglog.h"

/**************************************************************************************************
* Structure
**************************************************************************************************/
#define BT_SINK_LE_VOLUME_VALUE_DEFAULT 0x77    /**< Default volume value. */

typedef struct {
    ble_vcs_volume_flags_t flags;
} bt_sink_srv_le_volume_flags_t;

/**************************************************************************************************
* Variables
**************************************************************************************************/
static ble_vcs_volume_state_t g_sink_le_volume_state[CAP_UNICAST_DEVICE_NUM];
static bt_sink_srv_le_volume_flags_t g_sink_le_volume_flags[CAP_UNICAST_DEVICE_NUM];

/**************************************************************************************************
* Prototype
**************************************************************************************************/

/**************************************************************************************************
* Static Functions
**************************************************************************************************/
static ble_vcs_volume_state_t* bt_sink_srv_le_get_volume_state(bt_handle_t handle)
{
    uint8_t link_idx;

    if (BT_HANDLE_INVALID == handle) {
        return NULL;
    }

    if (CAP_UNICAST_DEVICE_NUM <= (link_idx = bt_sink_srv_cap_get_link_index(handle))) {
        return NULL;
    }

    return &g_sink_le_volume_state[link_idx];
}

static bt_sink_srv_le_volume_flags_t* bt_sink_srv_le_get_volume_flags(bt_handle_t handle)
{
    uint8_t link_idx;

    if (BT_HANDLE_INVALID == handle) {
        return NULL;
    }

    if (CAP_UNICAST_DEVICE_NUM <= (link_idx = bt_sink_srv_cap_get_link_index(handle))) {
        return NULL;
    }

    return &g_sink_le_volume_flags[link_idx];
}

static uint8_t bt_sink_srv_le_mapping_am_volume(uint8_t vcs_volume)
{
    return ((vcs_volume / BT_SINK_LE_VOLUME_VALUE_STEP) + ((0 == (vcs_volume % BT_SINK_LE_VOLUME_VALUE_STEP)) ? 0 : 1));
}

static bt_status_t bt_sink_srv_le_set_am_volume(bt_sink_srv_le_stream_type_t type, uint8_t volume)
{
    bt_sink_srv_am_stream_type_t in_out = ((type == BT_SINK_SRV_LE_STREAM_TYPE_IN) ? STREAM_IN : STREAM_OUT);

    if (AUD_EXECUTION_SUCCESS != bt_sink_srv_ami_audio_set_volume(bt_sink_srv_cap_am_get_aid(), volume, in_out)) {
        return BT_STATUS_FAIL;
    }

    return BT_STATUS_SUCCESS;
}

static bt_status_t bt_sink_srv_le_set_am_mute(bt_sink_srv_le_stream_type_t type, bool mute)
{
    bt_sink_srv_am_stream_type_t in_out = ((type == BT_SINK_SRV_LE_STREAM_TYPE_IN) ? STREAM_IN : STREAM_OUT);

    if (AUD_EXECUTION_SUCCESS != bt_sink_srv_ami_audio_set_mute(bt_sink_srv_cap_am_get_aid(), mute, in_out)) {
        return BT_STATUS_FAIL;
    }

    return BT_STATUS_SUCCESS;
}

static void bt_sink_srv_le_adjust_am_volume_state(bool set_mute, uint8_t mute, bool set_volume, uint8_t volume)
{
    bool am_mute = false;

    if (set_mute) {
        am_mute = ((mute) ? true : false);
        bt_sink_srv_le_set_am_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, am_mute);

    }
    if (set_volume && !am_mute) {
        uint8_t am_volume = bt_sink_srv_le_mapping_am_volume(volume);
        if (am_volume) {
            bt_sink_srv_le_set_am_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, false);
            bt_sink_srv_le_set_am_volume(BT_SINK_SRV_LE_STREAM_TYPE_OUT, am_volume);
        } else {
            bt_sink_srv_le_set_am_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, true);
        }
    }
}

static bt_status_t bt_sink_srv_le_set_vcs_volume_flags(bt_handle_t handle, ble_vcs_volume_flags_t volume_flags)
{
    bt_sink_srv_le_volume_flags_t *p_flags = NULL;

    if (NULL == (p_flags = bt_sink_srv_le_get_volume_flags(handle))) {
        le_audio_log("[sink_vol] set_vcs_volume_flags, invalid handle:%x", 1, handle);
        return BT_STATUS_FAIL;
    }

    if (p_flags->flags == volume_flags) {
        return BT_STATUS_SUCCESS;
    }

    p_flags->flags = volume_flags;

    le_audio_log("[sink_vol] set_vcs_volume_flags, volume_flags:%x", 1, p_flags->flags);

    ble_vcs_send_volume_flags_notification(handle, p_flags->flags);

    return BT_STATUS_SUCCESS;
}

static bt_status_t bt_sink_srv_le_set_vcs_absolute_volume(bt_handle_t handle, uint8_t volume)
{
    ble_vcs_volume_state_t *p_vol = NULL;

    if ((NULL == (p_vol = bt_sink_srv_le_get_volume_state(handle))) ||
        (volume > BLE_VCS_VOLUME_MAX) || (volume < BLE_VCS_VOLUME_MIN)) {
        le_audio_log("[sink_vol] set_vcs_absolute_volume, invalid param, handle:%x vol:%x", 2, handle, volume);
        return BT_STATUS_FAIL;
    }

    if (p_vol->volume == volume) {
        return BT_STATUS_SUCCESS;
    }

    p_vol->volume = volume;
    le_audio_log("[sink_vol] set_vcs_absolute_volume, volume:%x", 1, p_vol->volume);

    if (handle == bt_sink_srv_cap_stream_get_service_ble_link()) {
        /* if the last time is mute, it is not necessary to set any am volume */
        if (p_vol->mute) {
            return BT_STATUS_SUCCESS;
        }

        uint8_t am_volume = bt_sink_srv_le_mapping_am_volume(volume);
        if (am_volume) {
            bt_sink_srv_le_set_am_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, false);
            bt_sink_srv_le_set_am_volume(BT_SINK_SRV_LE_STREAM_TYPE_OUT, am_volume);
        } else {
            bt_sink_srv_le_set_am_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, true);
        }
    }

    ble_vcs_send_volume_state_notification(handle, p_vol->volume, p_vol->mute);

    bt_sink_srv_le_set_vcs_volume_flags(handle, BLE_VCS_VOLUME_FLAGS_SETTING_PERSISTED);

    return BT_STATUS_SUCCESS;
}

static bt_status_t bt_sink_srv_le_set_vcs_volume_state(bt_handle_t handle, bool set_mute, uint8_t mute, bool set_volume, bool volumeUp)
{
    ble_vcs_volume_state_t *p_vol = NULL;

    if (NULL == (p_vol = bt_sink_srv_le_get_volume_state(handle))) {
        le_audio_log("[sink_vol] set_vcs_volume_state, invalid handle:%x", 1, handle);
        return BT_STATUS_FAIL;
    }

    le_audio_log("[sink_vol] set_vcs_volume_state, handle:%x curr_volume:%x curr_mute:%x", 3, handle, p_vol->volume, p_vol->mute);
    le_audio_log("[sink_vol] set_vcs_volume_state, set_mute:%x mute:%x set_volume:%x volumeUp:%x", 4, set_mute, mute, set_volume, volumeUp);

    if (set_mute) {
        if (p_vol->mute != mute) {
            p_vol->mute = mute;
        } else {
            set_mute = false;
        }
    }

    if (set_volume) {
        if (volumeUp) {
            if (p_vol->volume != BLE_VCS_VOLUME_MAX) {
                if (p_vol->volume <= (BLE_VCS_VOLUME_MAX - BT_SINK_LE_VOLUME_VALUE_STEP)) {
                    p_vol->volume += BT_SINK_LE_VOLUME_VALUE_STEP;
                } else {
                    p_vol->volume = BLE_VCS_VOLUME_MAX;
                }
            } else {
                set_volume = false;
            }
        } else {
            /* volumeDown */
            if (p_vol->volume != BLE_VCS_VOLUME_MIN) {
                if (p_vol->volume > BLE_VCS_VOLUME_MIN + BT_SINK_LE_VOLUME_VALUE_STEP) {
                    p_vol->volume -= BT_SINK_LE_VOLUME_VALUE_STEP;
                } else {
                    p_vol->volume = BLE_VCS_VOLUME_MIN;
                }
            } else {
                set_volume = false;
            }
        }
    }

	if ((!set_volume) && (!set_mute)) {
        return BT_STATUS_SUCCESS;
    }

    if (p_vol->mute) {
        set_volume = false;

    } else if (set_mute){
        /* mute -> unmute */
        if (BLE_VCS_VOLUME_MIN == p_vol->volume) {
            /* keep am mute */
            set_mute = false;
        }
        set_volume = true;
    }

    if (handle == bt_sink_srv_cap_stream_get_service_ble_link()) {
        bool am_mute = false;

        if (set_mute) {
            am_mute = ((p_vol->mute) ? true : false);
            bt_sink_srv_le_set_am_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, am_mute);
        }
        if (set_volume && !am_mute) {
            uint8_t am_volume = bt_sink_srv_le_mapping_am_volume(p_vol->volume);
            if (am_volume) {
                bt_sink_srv_le_set_am_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, false);
                bt_sink_srv_le_set_am_volume(BT_SINK_SRV_LE_STREAM_TYPE_OUT, am_volume);
            } else {
                bt_sink_srv_le_set_am_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, true);
            }
        }
    }

    ble_vcs_send_volume_state_notification(handle, p_vol->volume, p_vol->mute);

    bt_sink_srv_le_set_vcs_volume_flags(handle, BLE_VCS_VOLUME_FLAGS_SETTING_PERSISTED);

    return BT_STATUS_SUCCESS;
}

/**************************************************************************************************
* Public Functions
**************************************************************************************************/
bt_status_t bt_sink_srv_le_volume_init(void)
{
    uint8_t i = CAP_UNICAST_DEVICE_NUM;

    while (i != 0) {
        i--;
        g_sink_le_volume_state[i].volume = BT_SINK_LE_VOLUME_VALUE_DEFAULT;
        g_sink_le_volume_state[i].mute = BLE_VCS_UNMUTE;
        g_sink_le_volume_flags[i].flags = BLE_VCS_VOLUME_FLAGS_SETTING_NOT_PERSISTED;
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_sink_srv_le_volume_vcp_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bool set_mute = false;
    bool set_volume = false;

    if (NULL == buffer) {
        return BT_STATUS_FAIL;
    }

    le_audio_log("[sink_vol] vcp_callback, msg:%x", 1, msg);

    switch (msg) {
        case BLE_VCS_SET_RELATIVE_VOLUME_DOWN_IND:
        case BLE_VCS_SET_UNMUTE_RELATIVE_VOLUME_DOWN_IND: {
            ble_vcs_set_relative_volume_down_ind_t *ind = (ble_vcs_set_relative_volume_down_ind_t *)buffer;
            ble_vcs_volume_state_t *p_vol = NULL;

            if (NULL == (p_vol = bt_sink_srv_le_get_volume_state(ind->handle))) {
                le_audio_log("[sink_vol] vcp_callback VOLUME_DOWN, invalid handle:%x", 1, ind->handle);
                return BT_STATUS_FAIL;
            }

            if (BLE_VCS_VOLUME_MIN != p_vol->volume) {
                if (p_vol->volume > BLE_VCS_VOLUME_MIN + BT_SINK_LE_VOLUME_VALUE_STEP) {
                    p_vol->volume -= BT_SINK_LE_VOLUME_VALUE_STEP;
                } else {
                    p_vol->volume = BLE_VCS_VOLUME_MIN;
                }
                set_volume = true;
            }

            if ((BLE_VCS_SET_UNMUTE_RELATIVE_VOLUME_DOWN_IND == msg) && (p_vol->mute)) {
                p_vol->mute = 0;
                set_mute = true;
                set_volume = true;
            }

            if ((!set_volume) && (!set_mute)) {
                return BT_STATUS_SUCCESS;
            }

            if ((BLE_VCS_VOLUME_MIN == p_vol->volume) && (0 == p_vol->mute)) {
                /* keep am mute */
                set_mute = false;
            }

            if (ind->handle == bt_sink_srv_cap_stream_get_service_ble_link()) {
                bt_sink_srv_le_adjust_am_volume_state(set_mute, p_vol->mute, set_volume, p_vol->volume);
            }

            ble_vcs_send_volume_state_notification(ind->handle, p_vol->volume, p_vol->mute);

            bt_sink_srv_le_set_vcs_volume_flags(ind->handle, BLE_VCS_VOLUME_FLAGS_SETTING_PERSISTED);

            break;
        }

        case BLE_VCS_SET_RELATIVE_VOLUME_UP_IND:
        case BLE_VCS_SET_UNMUTE_RELATIVE_VOLUME_UP_IND: {
            ble_vcs_set_relative_volume_up_ind_t *ind = (ble_vcs_set_relative_volume_up_ind_t *)buffer;
            ble_vcs_volume_state_t *p_vol = NULL;

            if (NULL == (p_vol = bt_sink_srv_le_get_volume_state(ind->handle))) {
                le_audio_log("[sink_vol] vcp_callback VOLUME_UP, invalid handle:%x", 1, ind->handle);
                return BT_STATUS_FAIL;
            }

            if (BLE_VCS_VOLUME_MAX != p_vol->volume) {
                 if (p_vol->volume <= (BLE_VCS_VOLUME_MAX - BT_SINK_LE_VOLUME_VALUE_STEP)) {
                     p_vol->volume += BT_SINK_LE_VOLUME_VALUE_STEP;
                 } else {
                     p_vol->volume = BLE_VCS_VOLUME_MAX;
                 }
                set_volume = true;
            }

            if (BLE_VCS_SET_UNMUTE_RELATIVE_VOLUME_UP_IND == msg && (p_vol->mute)) {
                p_vol->mute = 0;
                set_mute = true;
                set_volume = true;
            }

            if ((!set_volume) && (!set_mute)) {
                return BT_STATUS_SUCCESS;
            }

            if (p_vol->mute) {
                set_volume = false;
            }

            if (ind->handle == bt_sink_srv_cap_stream_get_service_ble_link()) {
                bt_sink_srv_le_adjust_am_volume_state(set_mute, p_vol->mute, set_volume, p_vol->volume);
            }

            ble_vcs_send_volume_state_notification(ind->handle, p_vol->volume, p_vol->mute);

            bt_sink_srv_le_set_vcs_volume_flags(ind->handle, BLE_VCS_VOLUME_FLAGS_SETTING_PERSISTED);

            break;
        }

        case BLE_VCS_SET_ABSOLUTE_VOLUME_IND: {
            ble_vcs_set_absolute_volume_ind_t *ind = (ble_vcs_set_absolute_volume_ind_t *)buffer;
            ble_vcs_volume_state_t *p_vol = NULL;

            if (NULL == (p_vol = bt_sink_srv_le_get_volume_state(ind->handle))) {
                le_audio_log("[sink_vol] vcp_callback SET_ABSOLUTE_VOLUME, invalid handle:%x", 1, ind->handle);
                return BT_STATUS_FAIL;
            }

            if ((p_vol->volume == ind->volume) ||
                (ind->volume > BLE_VCS_VOLUME_MAX) || (ind->volume < BLE_VCS_VOLUME_MIN)) {
                return BT_STATUS_FAIL;
            }

            p_vol->volume = ind->volume;
            if (!p_vol->mute) {
                set_volume = true;
            }

            if (ind->handle == bt_sink_srv_cap_stream_get_service_ble_link()) {
                bt_sink_srv_le_adjust_am_volume_state(set_mute, p_vol->mute, set_volume, p_vol->volume);
            }

            ble_vcs_send_volume_state_notification(ind->handle, p_vol->volume, p_vol->mute);

            bt_sink_srv_le_set_vcs_volume_flags(ind->handle, BLE_VCS_VOLUME_FLAGS_SETTING_PERSISTED);

            break;
        }

        case BLE_VCS_SET_UNMUTE_IND: {
            ble_vcs_set_unmute_ind_t *ind = (ble_vcs_set_unmute_ind_t *)buffer;
            ble_vcs_volume_state_t *p_vol = NULL;

            if (NULL == (p_vol = bt_sink_srv_le_get_volume_state(ind->handle))) {
                le_audio_log("[sink_vol] vcp_callback UNMUTE, invalid handle:%x", 1, ind->handle);
                return BT_STATUS_FAIL;
            }

            if (!p_vol->mute) {
                return BT_STATUS_SUCCESS;
            }

            p_vol->mute = 0;
            set_mute = true;
            if (p_vol->volume) {
                set_volume = true;
            } else {
                /* keep am mute */
            }

            if (ind->handle == bt_sink_srv_cap_stream_get_service_ble_link()) {
                bt_sink_srv_le_adjust_am_volume_state(set_mute, p_vol->mute, set_volume, p_vol->volume);
            }

            ble_vcs_send_volume_state_notification(ind->handle, p_vol->volume, p_vol->mute);

            break;
        }

        case BLE_VCS_SET_MUTE_IND: {
            ble_vcs_set_mute_ind_t *ind = (ble_vcs_set_mute_ind_t *)buffer;
            ble_vcs_volume_state_t *p_vol = NULL;

            if (NULL == (p_vol = bt_sink_srv_le_get_volume_state(ind->handle))) {
                le_audio_log("[sink_vol] vcp_callback MUTE, invalid handle:%x", 1, ind->handle);
                return BT_STATUS_FAIL;
            }

            if (p_vol->mute) {
                return BT_STATUS_SUCCESS;
            }

            p_vol->mute = 1;
            set_mute = true;

            if (ind->handle == bt_sink_srv_cap_stream_get_service_ble_link()) {
                bt_sink_srv_le_adjust_am_volume_state(set_mute, p_vol->mute, set_volume, p_vol->volume);
            }

            ble_vcs_send_volume_state_notification(ind->handle, p_vol->volume, p_vol->mute);

            break;
        }

        case BLE_VCS_READ_VOLUME_STATE_REQ: {
            ble_vcs_read_volume_state_req_t *req = (ble_vcs_read_volume_state_req_t *)buffer;
            ble_vcs_volume_state_t *p_vol = NULL;

            if (NULL == (p_vol = bt_sink_srv_le_get_volume_state(req->handle))) {
                le_audio_log("[sink_vol] vcp_callback READ_VOLUME_STATE, invalid handle:%x", 1, req->handle);
                return BT_STATUS_FAIL;
            }

            ble_vcs_send_volume_state_read_response(req->handle, 0, p_vol->volume, p_vol->mute);
            break;
        }

        case BLE_VCS_READ_VOLUME_FLAGS_REQ: {
            ble_vcs_read_volume_flags_req_t *req = (ble_vcs_read_volume_flags_req_t *)buffer;
            bt_sink_srv_le_volume_flags_t *p_flags = NULL;

            if (NULL == (p_flags = bt_sink_srv_le_get_volume_flags(req->handle))) {
                le_audio_log("[sink_vol] vcp_callback READ_VOLUME_FLAGS, invalid handle:%x", 1, req->handle);
                return BT_STATUS_FAIL;
            }

            ble_vcs_send_volume_flags_read_response(req->handle, 0, p_flags->flags);
            break;
        }

        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t bt_sink_srv_le_volume_vcp_send_action(bt_handle_t handle, bt_sink_srv_le_vcp_action_t action, void *params)
{
    switch (action) {
        case BT_SINK_SRV_LE_VCS_ACTION_RELATIVE_VOLUME_DOWN: {
            return bt_sink_srv_le_set_vcs_volume_state(handle, false, 0, true, false);
        }

        case BT_SINK_SRV_LE_VCS_ACTION_RELATIVE_VOLUME_UP: {
            return bt_sink_srv_le_set_vcs_volume_state(handle, false, 0, true, true);
        }

        case BT_SINK_SRV_LE_VCS_ACTION_UNMUTE_RELATIVE_VOLUME_DOWN: {
            return bt_sink_srv_le_set_vcs_volume_state(handle, true, 0, true, false);
        }

        case BT_SINK_SRV_LE_VCS_ACTION_UNMUTE_RELATIVE_VOLUME_UP: {
            return bt_sink_srv_le_set_vcs_volume_state(handle, true, 0, true, true);
        }

        case BT_SINK_SRV_LE_VCS_ACTION_SET_ABSOLUTE_VOLUME: {
            if (NULL == params) {
                return BT_STATUS_FAIL;
            }
            return bt_sink_srv_le_set_vcs_absolute_volume(handle, ((bt_sink_srv_le_set_absolute_volume_t *)params)->volume);
        }

        case BT_SINK_SRV_LE_VCS_ACTION_UNMUTE: {
            return bt_sink_srv_le_set_vcs_volume_state(handle, true, 0, false, false);
        }

        case BT_SINK_SRV_LE_VCS_ACTION_MUTE: {
            return bt_sink_srv_le_set_vcs_volume_state(handle, true, 1, false, false);
        }

        default:
            break;
    }
    return BT_STATUS_FAIL;
}

bt_status_t bt_sink_srv_le_volume_set_volume(bt_sink_srv_le_stream_type_t type, uint8_t volume)
{
    return bt_sink_srv_le_set_am_volume(type, volume);
}

bt_status_t bt_sink_srv_le_volume_set_mute(bt_sink_srv_le_stream_type_t type, bool mute)
{
    return bt_sink_srv_le_set_am_mute(type, mute);
}

bt_sink_srv_le_volume_state_t bt_sink_srv_le_volume_get_state(bt_handle_t handle)
{
    ble_vcs_volume_state_t *p_vol = NULL;
    bt_sink_srv_le_volume_state_t state;

    if (NULL != (p_vol = bt_sink_srv_le_get_volume_state(handle))) {
        state.volume = bt_sink_srv_le_mapping_am_volume(p_vol->volume);
        state.mute = ((p_vol->mute) ? true : false);
    }

    return state;
}

bt_status_t bt_sink_srv_le_volume_pts_set_paras(uint8_t volume, uint8_t mute, uint8_t flags)
{
    uint8_t i = CAP_UNICAST_DEVICE_NUM;

    while (i != 0) {
        i--;
        g_sink_le_volume_state[i].volume = volume;
        g_sink_le_volume_state[i].mute = mute;
        g_sink_le_volume_flags[i].flags = flags;
    }
    return BT_STATUS_SUCCESS;
}

