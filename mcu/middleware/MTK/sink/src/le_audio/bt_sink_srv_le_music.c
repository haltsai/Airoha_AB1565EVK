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


#include "bt_sink_srv_le.h"
#include "bt_sink_srv_le_music.h"
#include "bt_sink_srv_le_volume.h"
#include "bt_le_audio_sink.h"
#include "bt_avrcp.h"


#if defined (AIR_LE_AUDIO_ENABLE) && defined (AIR_LE_AUDIO_CIS_ENABLE)
extern bt_le_sink_srv_music_active_handle g_music_active_handle;

void le_sink_srv_music_event_callback(uint8_t event_id, void *p_msg);
static bt_avrcp_operation_id_t le_sink_srv_music_get_play_pause_action();


static bt_status_t le_sink_srv_get_element_attribute_handler(bt_handle_t handle, uint8_t index,void *param)
{
    bt_status_t ret = BT_STATUS_FAIL;
    bt_sink_srv_avrcp_get_element_attributes_parameter_t *attr_param = (bt_sink_srv_avrcp_get_element_attributes_parameter_t *)param;
    bt_sink_srv_assert(attr_param && "Null parameter");
    bt_avrcp_media_attribute_t attri_id = attr_param->attribute_list->attribute_id;
    switch (attri_id) {
        case BT_AVRCP_MEDIA_ATTRIBUTE_TITLE:/**< Display the title of the media. */
        {
            ble_mcp_read_track_title_req(handle, 0xFF);

            break;
        }
        case BT_AVRCP_MEDIA_ATTRIBUTE_ARTIST_NAME:/**< Display the name of the artist. */
        {
            break;
        }
        case BT_AVRCP_MEDIA_ATTRIBUTE_ALBUM_NAME:/**< Display the name of the album. */
        {

            break;
        }
        case BT_AVRCP_MEDIA_ATTRIBUTE_MEDIA_NUMBER:/**< Display the number of the media, such as the track number of the CD. */
        {
            break;
        }
        case BT_AVRCP_MEDIA_ATTRIBUTE_TOTAL_MEDIA_NUMBER:/**< Display the total number of the media, such as the total number of tracks on the CD. */
        {
            break;
        }
        case BT_AVRCP_MEDIA_ATTRIBUTE_GENRE:/**< Display the music genre of the media. */
        {
            break;
        }
        case BT_AVRCP_MEDIA_ATTRIBUTE_PLAYING_TIME:
        {

            break;
        }
        default:
            break;
    }
    return ret;
}


static bt_avrcp_operation_id_t le_sink_srv_music_get_play_pause_action()
{
    le_sink_srv_context_t * cntx = le_sink_srv_get_context(g_music_active_handle.handle);

    bt_avrcp_operation_id_t action_id = 0;
    if (NULL == cntx) {
       return action_id;
    }

    if(cntx->last_play_pause_action == BT_AVRCP_OPERATION_ID_PLAY) {
        action_id = BT_AVRCP_OPERATION_ID_PAUSE;
    } else if (cntx->last_play_pause_action == BT_AVRCP_OPERATION_ID_PAUSE) {
        action_id = BT_AVRCP_OPERATION_ID_PLAY;
    } else if (cntx->music_state == BT_SINK_SRV_STATE_STREAMING) {
        action_id = BT_AVRCP_OPERATION_ID_PAUSE;
    } else if (cntx->music_state  == BT_SINK_SRV_STATE_CONNECTED) {
        action_id = BT_AVRCP_OPERATION_ID_PLAY;
    }
#if 0
    else if (dev->avrcp_status == BT_SINK_SRV_MUSIC_AVRCP_INVALID_STATUS && dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_STREAMING) {
        action_id = BT_AVRCP_OPERATION_ID_PAUSE;
    } else if (dev->avrcp_status == BT_SINK_SRV_MUSIC_AVRCP_INVALID_STATUS && dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_SUSPEND) {
        action_id = BT_AVRCP_OPERATION_ID_PLAY;
    } else if ((dev->avrcp_status == BT_AVRCP_STATUS_PLAY_FWD_SEEK || dev->avrcp_status == BT_AVRCP_STATUS_PLAY_REV_SEEK)
        && dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_STREAMING) {
        action_id = BT_AVRCP_OPERATION_ID_PAUSE;
    } else if ((dev->avrcp_status == BT_AVRCP_STATUS_PLAY_FWD_SEEK || dev->avrcp_status == BT_AVRCP_STATUS_PLAY_REV_SEEK)
        && dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_SUSPEND) {
        action_id = BT_AVRCP_OPERATION_ID_PLAY;
    }
#endif
    return action_id;
}


bt_status_t bt_sink_srv_le_music_action_handler(uint32_t action, void *param)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_handle_t handle = g_music_active_handle.handle;//bt_sink_srv_cap_get_link_handle(0xFF);
    le_sink_srv_context_t *srv_cntx = le_sink_srv_get_context(handle);

    if (NULL == srv_cntx || handle == BT_HANDLE_INVALID || !(srv_cntx->conn_mask & BT_SINK_SRV_PROFILE_A2DP_SINK)) {
        ret = BT_STATUS_FAIL;
        return ret;
    }
    bt_sink_srv_report_id("[Sink]le_sink_srv_music_action_handler, handle:%x, action:%x, conn_mask :%x", 3, handle, action, srv_cntx->conn_mask);
    uint32_t le_action = bt_sink_srv_cap_get_le_audio_action(action);
    bt_le_audio_sink_action_param_t le_param = {
        .service_idx = 0xFF,
    };
    switch (action)
    {
        case BT_SINK_SRV_ACTION_PLAY:
        case BT_SINK_SRV_ACTION_PAUSE:
        case BT_SINK_SRV_ACTION_NEXT_TRACK:
        case BT_SINK_SRV_ACTION_PREV_TRACK:
        case BT_SINK_SRV_ACTION_FAST_FORWARD:
        case BT_SINK_SRV_ACTION_REWIND:
        case BT_SINK_SRV_ACTION_GET_PLAY_STATUS: {
            bt_le_audio_sink_send_action(handle, le_action, &le_param);
            break;
        }
        case BT_SINK_SRV_ACTION_PLAY_PAUSE: {
            le_sink_srv_music_get_play_pause_action();
            if (BLE_MCS_MEDIA_STATE_PLAYING == bt_le_audio_sink_media_get_state(handle, 0xFF)) {
                bt_le_audio_sink_send_action(handle, BT_LE_AUDIO_SINK_ACTION_MEDIA_PAUSE, &le_param);
            }else {
                bt_le_audio_sink_send_action(handle, BT_LE_AUDIO_SINK_ACTION_MEDIA_PLAY, &le_param);
            }
            break;
        }
        case BT_SINK_SRV_ACTION_GET_CAPABILITY: {
            bt_sink_srv_avrcp_get_capability_parameter_t *capability_parameter = (bt_sink_srv_avrcp_get_capability_parameter_t *)param;
            if (BT_AVRCP_CAPABILITY_EVENTS_SUPPORTED == capability_parameter->type) {
                bt_le_audio_sink_send_action(handle, le_action, &le_param);
            }
            break;
        }
        case BT_SINK_SRV_ACTION_GET_ELEMENT_ATTRIBUTE: {
            le_sink_srv_get_element_attribute_handler(handle, 0xFF, param);
            //to do
            break;
        }
        //case BT_SINK_SRV_ACTION_VOLUME_MIN:
        case BT_SINK_SRV_ACTION_VOLUME_UP:
        //case BT_SINK_SRV_ACTION_VOLUME_MAX:
        case BT_SINK_SRV_ACTION_VOLUME_DOWN: {
            bt_sink_srv_le_volume_vcp_send_action(handle, le_action, NULL);
            break;
        }
#if 0
        case BT_SINK_SRV_ACTION_GET_FOLDER_ITEM: {
            bt_sink_srv_avrcp_get_folder_items_parameter_t *folder_parameter = (bt_sink_srv_avrcp_get_folder_items_parameter_t *)param;
            if (dev && (dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
                ret = bt_sink_srv_avrcp_get_folder_item(dev, folder_parameter);
            }

            break;
        }
#endif
        default:
            ret = BT_STATUS_FAIL;
            break;
    }
    bt_sink_srv_report_id("[Sink]le_sink_srv_music_action_handler:ret:%x", 1, ret);
    return ret;
}

bt_status_t bt_sink_srv_le_music_mcp_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    if (NULL == buffer) {
        return BT_STATUS_FAIL;
    }

#if 0
    switch (msg) {
        case BLE_MCP_SET_MEDIA_PLAYER_NAME_NOTIFICATION_CNF:
        case BLE_MCP_SET_TRACK_CHANGED_NOTIFICATION_CNF:
        case BLE_MCP_SET_TRACK_TITLE_NOTIFICATION_CNF:
        case BLE_MCP_SET_TRACK_DURATION_NOTIFICATION_CNF:
        case BLE_MCP_SET_TRACK_POSITION_NOTIFICATION_CNF:
        case BLE_MCP_SET_PLAYBACK_SPEED_NOTIFICATION_CNF:
        case BLE_MCP_SET_SEEKING_SPEED_NOTIFICATION_CNF:
        case BLE_MCP_SET_PLAYING_ORDER_NOTIFICATION_CNF:
        case BLE_MCP_SET_MEDIA_STATE_NOTIFICATION_CNF:
        case BLE_MCP_SET_MEDIA_CONTROL_OPCODES_SUPPORTED_NOTIFICATION_CNF: {
            ble_mcp_event_parameter_t *cnf = (ble_mcp_event_parameter_t *)buffer;
            le_audio_log("[MCP][sink_music] SET_NOTIFICATION_CNF, msg:%x status:%x handle:%x srv_idx:%x", 5, msg, status, cnf->handle, cnf->service_idx);
            break;
        }

        case BLE_MCP_READ_MEDIA_PLAYER_NAME_CNF: {
            ble_mcp_read_media_player_name_cnf_t *cnf = (ble_mcp_read_media_player_name_cnf_t *)buffer;
            le_audio_log("[MCP][sink_music] READ_MEDIA_PLAYER_NAME_CNF, status:%x handle:%x srv_idx:%x len:%x", 4, status, cnf->handle, cnf->service_idx, cnf->media_player_name_length);
            break;
        }
        case BLE_MCP_READ_MEDIA_PLAYER_ICON_URL_CNF: {
            ble_mcp_read_media_play_icon_url_cnf_t *cnf = (ble_mcp_read_media_play_icon_url_cnf_t *)buffer;
            le_audio_log("[MCP][sink_music] READ_MEDIA_PLAYER_ICON_URL_CNF, status:%x handle:%x srv_idx:%x len:%x", 4, status, cnf->handle, cnf->service_idx, cnf->media_player_icon_url_length);
            break;
        }
        case BLE_MCP_READ_TRACK_TITLE_CNF: {
            ble_mcp_read_track_title_cnf_t *cnf = (ble_mcp_read_track_title_cnf_t *)buffer;
            le_audio_log("[MCP][sink_music] READ_TRACK_TITLE_CNF, status:%x handle:%x srv_idx:%x len:%x", 4, status, cnf->handle, cnf->service_idx, cnf->track_title_length);
            break;
        }
        case BLE_MCP_READ_TRACK_DURATION_CNF: {
            ble_mcp_read_track_duration_cnf_t *cnf = (ble_mcp_read_track_duration_cnf_t *)buffer;
            le_audio_log("[MCP][sink_music] READ_TRACK_DURATION_CNF, status:%x handle:%x srv_idx:%x track_duration:%x", 4, status, cnf->handle, cnf->service_idx, cnf->track_duration);
            break;
        }
        case BLE_MCP_READ_TRACK_POSITION_CNF: {
            ble_mcp_read_track_position_cnf_t *cnf = (ble_mcp_read_track_position_cnf_t *)buffer;
            le_audio_log("[MCP][sink_music] READ_TRACK_POSITION_CNF, status:%x handle:%x srv_idx:%x track_position:%x", 4, status, cnf->handle, cnf->service_idx, cnf->track_position);
            break;
        }
        case BLE_MCP_READ_PLAYBACK_SPEED_CNF: {
            ble_mcp_read_playback_speed_cnf_t *cnf = (ble_mcp_read_playback_speed_cnf_t *)buffer;
            le_audio_log("[MCP][sink_music] READ_PLAYBACK_SPEED_CNF, status:%x handle:%x srv_idx:%x playback_speed:%x", 4, status, cnf->handle, cnf->service_idx, cnf->playback_speed);
            break;
        }
        case BLE_MCP_READ_SEEKING_SPEED_CNF: {
            ble_mcp_read_seeking_speed_cnf_t *cnf = (ble_mcp_read_seeking_speed_cnf_t *)buffer;
            le_audio_log("[MCP][sink_music] READ_PLAYBACK_SPEED_CNF, status:%x handle:%x srv_idx:%x seeking_speed:%x", 4, status, cnf->handle, cnf->service_idx, cnf->seeking_speed);
            break;
        }
        case BLE_MCP_READ_PLAYING_ORDER_CNF: {
            ble_mcp_read_playing_order_cnf_t *cnf = (ble_mcp_read_playing_order_cnf_t *)buffer;
            le_audio_log("[MCP][sink_music] READ_PLAYING_ORDER_CNF, status:%x handle:%x srv_idx:%x playing_order:%x", 4, status, cnf->handle, cnf->service_idx, cnf->playing_order);
            break;
        }
        case BLE_MCP_READ_PLAYING_ORDERS_SUPPORTED_CNF: {
            ble_mcp_read_playing_orders_supported_cnf_t *cnf = (ble_mcp_read_playing_orders_supported_cnf_t *)buffer;
            le_audio_log("[MCP][sink_music] READ_PLAYING_ORDERS_SUPPORTED_CNF, status:%x handle:%x srv_idx:%x playing_orders_supported:%x", 4, status, cnf->handle, cnf->service_idx, cnf->playing_order_supported);
            break;
        }
        case BLE_MCP_READ_MEDIA_STATE_CNF: {
            ble_mcp_read_media_state_cnf_t *cnf = (ble_mcp_read_media_state_cnf_t *)buffer;
            le_audio_log("[MCP][sink_music] READ_MEDIA_STATE_CNF, status:%x handle:%x srv_idx:%x media_state:%x", 4, status, cnf->handle, cnf->service_idx, cnf->media_state);
            break;
        }
        case BLE_MCP_READ_MEDIA_CONTROL_OPCODES_SUPPORTED_CNF:{
            ble_mcp_read_media_control_opcodes_supported_cnf_t *cnf = (ble_mcp_read_media_control_opcodes_supported_cnf_t *)buffer;
            le_audio_log("[MCP][sink_music] READ_MEDIA_CONTROL_OPCODES_SUPPORTED_CNF, status:%x handle:%x srv_idx:%x media_control_opcodes_supported:%x", 4, status, cnf->handle, cnf->service_idx, cnf->media_control_opcodes_supported);
            break;
        }
        case BLE_MCP_READ_CONTENT_CONTROL_ID_CNF: {
            ble_mcp_read_content_control_id_cnf_t *cnf = (ble_mcp_read_content_control_id_cnf_t *)buffer;
            le_audio_log("[MCP][sink_music] READ_CONTENT_CONTROL_ID_CNF, status:%x handle:%x srv_idx:%x content_control_id:%x", 4, status, cnf->handle, cnf->service_idx, cnf->content_control_id);
            break;
        }
        case BLE_MCP_MEDIA_PLAYER_NAME_IND: {
            ble_mcp_media_player_name_ind_t *ind = (ble_mcp_media_player_name_ind_t *)buffer;
            le_audio_log("[MCP][sink_music] MEDIA_PLAYER_NAME_IND, status:%x handle:%x srv_idx:%x len:%x", 4, status, ind->handle, ind->service_idx, ind->media_player_name_length);
            break;
        }
        case BLE_MCP_TRACK_CHANGED_IND: {
            ble_mcp_track_changed_ind_t *ind = (ble_mcp_track_changed_ind_t *)buffer;
            le_audio_log("[MCP][sink_music] TRACK_CHANGED_IND, status:%x handle:%x srv_idx:%x", 3, status, ind->handle, ind->service_idx);
            break;
        }
        case BLE_MCP_TRACK_TITLE_IND: {
            ble_mcp_track_title_ind_t *ind = (ble_mcp_track_title_ind_t *)buffer;
            le_audio_log("[MCP][sink_music] TRACK_TITLE_IND, status:%x handle:%x srv_idx:%x len:%x", 4, status, ind->handle, ind->service_idx, ind->track_title_length);
            break;
        }
        case BLE_MCP_TRACK_DURATION_IND: {
            ble_mcp_track_duration_ind_t *ind = (ble_mcp_track_duration_ind_t *)buffer;
            le_audio_log("[MCP][sink_music] TRACK_DURATION_IND, status:%x handle:%x srv_idx:%x track_duration:%x", 4, status, ind->handle, ind->service_idx, ind->track_duration);
            break;
        }
        case BLE_MCP_TRACK_POSITION_IND: {
            ble_mcp_track_position_ind_t *ind = (ble_mcp_track_position_ind_t *)buffer;
            le_audio_log("[MCP][sink_music] TRACK_POSITION_IND, status:%x handle:%x srv_idx:%x track_position:%x", 4, status, ind->handle, ind->service_idx, ind->track_position);
            break;
        }
        case BLE_MCP_PLAYBACK_SPEED_IND: {
            ble_mcp_playback_speed_ind_t *ind = (ble_mcp_playback_speed_ind_t *)buffer;
            le_audio_log("[MCP][sink_music] PLAYBACK_SPEED_IND, status:%x handle:%x srv_idx:%x playback_speed:%x", 4, status, ind->handle, ind->service_idx, ind->playback_speed);
            break;
        }
        case BLE_MCP_SEEKING_SPEED_IND: {
            ble_mcp_seeking_speed_ind_t *ind = (ble_mcp_seeking_speed_ind_t *)buffer;
            le_audio_log("[MCP][sink_music] SEEKING_SPEED_IND, status:%x handle:%x srv_idx:%x seeking_speed:%x", 4, status, ind->handle, ind->service_idx, ind->seeking_speed);
            break;
        }
        case BLE_MCP_PLAYING_ORDER_IND: {
            ble_mcp_playing_order_ind_t *ind = (ble_mcp_playing_order_ind_t *)buffer;
            le_audio_log("[MCP][sink_music] PLAYING_ORDER_IND, status:%x handle:%x srv_idx:%x playing_order:%x", 4, status, ind->handle, ind->service_idx, ind->playing_order);
            break;
        }
        case BLE_MCP_MEDIA_STATE_IND: {
            ble_mcp_media_state_ind_t *ind = (ble_mcp_media_state_ind_t *)buffer;
            le_audio_log("[MCP][sink_music] MEDIA_STATE_IND, status:%x handle:%x srv_idx:%x media_state:%x", 4, status, ind->handle, ind->service_idx, ind->media_state);
            break;
        }
        case BLE_MCP_MEDIA_CONTROL_OPCODES_SUPPORTED_IND: {
            ble_mcp_media_control_opcodes_supported_ind_t *ind = (ble_mcp_media_control_opcodes_supported_ind_t *)buffer;
            le_audio_log("[MCP][sink_music] MEDIA_CONTROL_OPCODES_SUPPORTED_IND, status:%x handle:%x srv_idx:%x media_state:%x", 4, status, ind->handle, ind->service_idx, ind->media_control_opcodes_supported);
            break;
        }
        default:
            break;
    }
#endif

    return BT_STATUS_SUCCESS;
}

#endif

