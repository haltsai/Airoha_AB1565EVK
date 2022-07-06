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

#include "bt_sink_srv.h"
#include "bt_sink_srv_le_cap.h"
#include "bt_sink_srv_le_cap_stream.h"
#include "bt_sink_srv_le_cap_audio_manager.h"
#include "bt_sink_srv_le_volume.h"

#include "bt_le_audio_sink.h"
#include "bt_le_audio_msglog.h"

#include "bt_gap_le.h"
#include "bt_gap_le_service.h"

#include "bt_callback_manager.h"

#define CAP_INVALID_UINT8   0xFF
/**************************************************************************************************
* Structure
**************************************************************************************************/
typedef struct {
	bt_handle_t connect_handle;
    uint8_t state;
    uint8_t sub_state;
    bt_addr_t peer_addr;
} bt_sink_srv_cap_link_info_t;

typedef struct {
    bt_sink_srv_cap_callback_t callback;
	uint8_t max_link_num;
} bt_sink_srv_cap_ctrl_t;

/**************************************************************************************************
* Variables
**************************************************************************************************/
static bt_sink_srv_cap_ctrl_t g_sink_srv_cap_ctl;
static bt_sink_srv_cap_callback_t g_sink_srv_cap_broadcast_callback = NULL;

#ifdef AIR_LE_AUDIO_CIS_ENABLE
static bt_sink_srv_cap_link_info_t *g_sink_srv_cap_link = NULL;
/**************************************************************************************************
* Prototype
**************************************************************************************************/
uint8_t bt_sink_srv_cap_get_link_index(bt_handle_t connect_handle);
#endif

bool bt_sink_srv_cap_inform_app(bt_sink_srv_cap_event_id_t event_id, void *p_msg);

/**************************************************************************************************
* Static Functions
**************************************************************************************************/
#ifdef AIR_LE_AUDIO_CIS_ENABLE
static void bt_sink_srv_cap_init_link(void)
{
	uint8_t link_index;

	for(link_index = 0; link_index < g_sink_srv_cap_ctl.max_link_num; link_index++)
    {
		g_sink_srv_cap_link[link_index].connect_handle = BT_HANDLE_INVALID;
        g_sink_srv_cap_link[link_index].state = BT_SINK_SRV_CAP_STATE_IDLE;
        g_sink_srv_cap_link[link_index].sub_state = CAP_INVALID_UINT8;
    }
}

static uint8_t bt_sink_srv_cap_get_sub_state(bt_handle_t connect_handle)
{
    uint8_t link_index = bt_sink_srv_cap_get_link_index(connect_handle);

    le_audio_log("[sink cap] get sink srv sub state conn handle:%04x, current sub state:%d", 2, connect_handle, g_sink_srv_cap_link[link_index].sub_state);

    return g_sink_srv_cap_link[link_index].sub_state;
}
#endif

static bt_status_t bt_sink_srv_cap_gap_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
	/*le_audio_log("[sink cap] gap ble callback, id:%x, status:%d", 2, msg, status);

	switch (msg) {
		case BT_GAP_LE_CONNECT_IND:
			if (bt_sink_srv_cap_set_link(((bt_gap_le_connection_ind_t *)buffer)->connection_handle, ((bt_gap_le_connection_ind_t *)buffer)->peer_addr) == false) {
				le_audio_log("[sink cap] cap set link, FAIL", 0);
				configASSERT(0);
			}

            bt_sink_srv_cap_stream_allocate_ase_link(((bt_gap_le_connection_ind_t *)buffer)->connection_handle);

			break;

		case BT_GAP_LE_DISCONNECT_IND:
            bt_sink_srv_cap_stream_clear_all_ase_link(((bt_gap_le_disconnect_ind_t *)buffer)->connection_handle);

			if (bt_sink_srv_cap_clear_link(((bt_gap_le_disconnect_ind_t *)buffer)->connection_handle) == false) {
				le_audio_log("[sink cap] cap clear link, FAIL", 0);
				configASSERT(0);
			}

            bt_sink_srv_cap_am_deinit();

			break;

		default:
			break;
	}*/

	return BT_STATUS_SUCCESS;
}

static bt_status_t bt_sink_srv_cap_event_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
	uint32_t module = msg & 0xFF000000;


    if (msg == BT_POWER_OFF_CNF) {
        //bt_cap_stream_power_off_reset();
    }

	switch (module) {
		case BT_MODULE_GAP:
			bt_sink_srv_cap_gap_callback(msg, status, buffer);
			break;

		case BT_MODULE_GATT:
			break;

        case BT_MODULE_LE_AUDIO: {
            le_audio_log("[sink cap] event callback, msg:%x, status:%x", 2, msg, status);
            uint32_t lea_module = msg & BT_LE_AUDIO_MODULE_MASK;
            switch (lea_module) {
                case BT_LE_AUDIO_MODULE_VCP:
                    bt_sink_srv_le_volume_vcp_callback(msg, status, buffer);
                break;
                case BT_LE_AUDIO_MODULE_MCP:
                    bt_sink_srv_le_music_mcp_callback(msg, status, buffer);
                break;
                case BT_LE_AUDIO_MODULE_CCP:
                    bt_sink_srv_le_call_ccp_callback(msg, status, buffer);
                    break;
                default:
                    break;
            }
            break;
        }
		default:
			break;
	}

	return BT_STATUS_SUCCESS;
}


static void bt_sink_srv_cap_le_audio_callback(bt_le_audio_sink_event_t event, void *msg)
{
    g_sink_srv_cap_ctl.callback(event, msg);
}
/**************************************************************************************************
* Public Functions
**************************************************************************************************/
bt_status_t bt_sink_srv_cap_init(bt_sink_srv_cap_callback_t callback, uint8_t max_link_num)
{
	if (g_sink_srv_cap_ctl.callback) {
		le_audio_log("[sink cap] register callback, FAIL, already registered", 0);
		return BT_STATUS_FAIL;
	}

    if (BT_STATUS_SUCCESS != bt_le_audio_sink_init((BLE_TMAP_ROLE_MASK_CT|BLE_TMAP_ROLE_MASK_UMR|BLE_TMAP_ROLE_MASK_BMR), bt_sink_srv_cap_le_audio_callback, max_link_num)) {
        return BT_STATUS_FAIL;
    }

	if (bt_sink_srv_cap_stream_init(max_link_num) != true) {
		configASSERT(0);
	}

	if (bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_GAP | MODULE_MASK_GATT | MODULE_MASK_SYSTEM | MODULE_MASK_LE_AUDIO, (void *)bt_sink_srv_cap_event_callback) != BT_STATUS_SUCCESS) {
		configASSERT(0);
	}

#ifdef AIR_LE_AUDIO_CIS_ENABLE
	if ((g_sink_srv_cap_link = (bt_sink_srv_cap_link_info_t *)pvPortMalloc(sizeof(bt_sink_srv_cap_link_info_t) * max_link_num)) == (bt_sink_srv_cap_link_info_t *)NULL) {
		configASSERT(0);
	}

#endif

	le_audio_log("[sink cap] register callback, SUCCESS, max link:%d", 1, max_link_num);
	g_sink_srv_cap_ctl.callback = callback;
	g_sink_srv_cap_ctl.max_link_num = max_link_num;

	bt_sink_srv_cap_init_link();

    bt_sink_srv_le_volume_init();

    //bt_bass_init(max_link_num);

	return BT_STATUS_SUCCESS;
}

bt_status_t bt_sink_srv_cap_deinit(bt_sink_srv_cap_callback_t callback)
{
	if (g_sink_srv_cap_ctl.callback != callback) {
		le_audio_log("[sink cap] deregister callback, FAIL, wrong callback", 0);
		return BT_STATUS_FAIL;
	}

	if(bt_sink_srv_cap_stream_deinit() != true)
	{
		configASSERT(0);
	}

	if(bt_callback_manager_deregister_callback(bt_callback_type_app_event, (void *)bt_sink_srv_cap_gap_callback) != BT_STATUS_SUCCESS)
	{
		configASSERT(0);
	}

    g_sink_srv_cap_ctl.callback = NULL;
	le_audio_log("[sink cap] deregister callback, SUCCESS", 0);
	return BT_STATUS_SUCCESS;
}

bt_status_t bt_sink_srv_cap_register_broadcast_callback(bt_sink_srv_cap_callback_t callback)
{
    if (NULL == g_sink_srv_cap_broadcast_callback) {
        g_sink_srv_cap_broadcast_callback = callback;
        le_audio_log("[sink cap] register broadcast callback, SUCCESS", 0);
        return BT_STATUS_SUCCESS;
    }
    le_audio_log("[sink cap] register broadcast callback, FAIL", 0);
    return BT_STATUS_FAIL;
}

bt_status_t bt_sink_srv_cap_deregister_broadcast_callback(void)
{
    if (NULL != g_sink_srv_cap_broadcast_callback) {
        g_sink_srv_cap_broadcast_callback = NULL;
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

#ifdef AIR_LE_AUDIO_CIS_ENABLE
bt_status_t bt_sink_srv_cap_set_link(bt_handle_t connect_handle)
{
	uint8_t link_index;

	le_audio_log("[sink cap] cap set link, max link num:%d", 1, g_sink_srv_cap_ctl.max_link_num);
	
    for(link_index = 0; link_index < g_sink_srv_cap_ctl.max_link_num; link_index++)
    {
    	le_audio_log("[sink cap] cap set link, idx:%d, handle1:0x%x, handle2:0x%x", 3, link_index, g_sink_srv_cap_link[link_index].connect_handle, connect_handle);
		if(g_sink_srv_cap_link[link_index].connect_handle != connect_handle &&
		   g_sink_srv_cap_link[link_index].connect_handle == BT_HANDLE_INVALID)
		{
			le_audio_log("[sink cap] set link, index:%d, connect handle:%d", 2, link_index, connect_handle);

			g_sink_srv_cap_link[link_index].connect_handle = connect_handle;

            bt_gap_le_srv_conn_info_t *le_info = bt_gap_le_srv_get_conn_info(connect_handle);
            memcpy(&g_sink_srv_cap_link[link_index].peer_addr, &le_info->peer_addr, sizeof(bt_addr_t));

            bt_addr_t address = bt_sink_srv_cap_get_peer_bdaddr(link_index);
            le_audio_log("[sink cap] set link, type:%x, addr:%x %x %x %x %x %x", 7, address.type,
                address.addr[0], address.addr[1], address.addr[2],
                address.addr[3], address.addr[4], address.addr[5]);

            bt_sink_srv_cap_stream_allocate_ase_link(connect_handle);

            bt_sink_srv_cap_set_state(connect_handle, BT_SINK_SRV_CAP_STATE_CONNECTED);
            return BT_STATUS_SUCCESS;
		}
    }

	le_audio_log("[sink cap] set link, FAIL", 0);
	return BT_STATUS_FAIL;
}

bt_status_t bt_sink_srv_cap_clear_link(bt_handle_t connect_handle)
{
	uint8_t index;

	if (connect_handle == BT_HANDLE_INVALID)
	{
		le_audio_log("[sink cap] clear link, INVALID connect handle", 0);
		return false;
	}

    for(index = 0; index < g_sink_srv_cap_ctl.max_link_num; index++)
    {
        le_audio_log("[sink cap] clear link, idx:%d, connect handle1:%d, connect handle2:%d", 3, index, g_sink_srv_cap_link[index].connect_handle, connect_handle);

		if(g_sink_srv_cap_link[index].connect_handle == connect_handle)
        {
            bt_sink_srv_cap_stream_clear_all_ase_link(connect_handle);
            bt_sink_srv_cap_set_state(connect_handle, BT_SINK_SRV_CAP_STATE_IDLE);
            bt_sink_srv_cap_am_audio_stop(CAP_AM_UNICAST_CALL_MODE_0 + index);
            bt_sink_srv_cap_am_audio_stop(CAP_AM_UNICAST_MUSIC_MODE_0 + index);
			g_sink_srv_cap_link[index].connect_handle = BT_HANDLE_INVALID;
            memset(&g_sink_srv_cap_link[index].peer_addr, 0, sizeof(bt_addr_t));

            return BT_STATUS_SUCCESS;
        }
    }

	le_audio_log("[sink cap] clear link, FAIL, connect handle:%d", 1, connect_handle);

    return BT_STATUS_FAIL;
}

bt_handle_t bt_sink_srv_cap_get_link_handle(uint8_t index)
{
    if(index == 0xFF)
    {
        return g_sink_srv_cap_link[0].connect_handle;
    }
    else if(index < g_sink_srv_cap_ctl.max_link_num)
    {
        return g_sink_srv_cap_link[index].connect_handle;
    }
    else
    {
        return BT_HANDLE_INVALID;
    }
}

bt_addr_t bt_sink_srv_cap_get_peer_bdaddr(uint8_t index)
{
    if (index == 0xFF) {
        return g_sink_srv_cap_link[0].peer_addr;
    } else if (index < g_sink_srv_cap_ctl.max_link_num) {
        return g_sink_srv_cap_link[index].peer_addr;
    } else {
        bt_addr_t addr = {0};
        return addr;
    }
}

uint32_t bt_sink_srv_cap_get_le_audio_action(uint32_t sink_action)
{
    switch(sink_action)
    {
        /*CCP*/
        case BT_SINK_SRV_ACTION_ANSWER:
            return BT_LE_AUDIO_SINK_ACTION_CALL_ACCEPT;

        case BT_SINK_SRV_ACTION_REJECT:
        case BT_SINK_SRV_ACTION_HANG_UP:
            return BT_LE_AUDIO_SINK_ACTION_CALL_TERMINATE;

        case BT_SINK_SRV_ACTION_3WAY_HOLD_SPECIAL:
            return BT_LE_AUDIO_SINK_ACTION_CALL_HOLD;

        case BT_SINK_SRV_ACTION_DIAL_NUMBER:
            return BT_LE_AUDIO_SINK_ACTION_CALL_ORIGINATE;

        case BT_SINK_SRV_ACTION_3WAY_ADD_HELD_CALL_TO_CONVERSATION:
            return BT_LE_AUDIO_SINK_ACTION_CALL_JOIN;

        /*MCP*/
        case BT_SINK_SRV_ACTION_PLAY:
            return BT_LE_AUDIO_SINK_ACTION_MEDIA_PLAY;

        case BT_SINK_SRV_ACTION_PAUSE:
            return BT_LE_AUDIO_SINK_ACTION_MEDIA_PAUSE;

        case BT_SINK_SRV_ACTION_NEXT_TRACK:
            return BT_LE_AUDIO_SINK_ACTION_MEDIA_NEXT_TRACK;

        case BT_SINK_SRV_ACTION_PREV_TRACK:
            return BT_LE_AUDIO_SINK_ACTION_MEDIA_PREVIOUS_TRACK;

        case BT_SINK_SRV_ACTION_FAST_FORWARD:
            return BT_LE_AUDIO_SINK_ACTION_MEDIA_FAST_FORWARD;

        case BT_SINK_SRV_ACTION_REWIND:
            return BT_LE_AUDIO_SINK_ACTION_MEDIA_FAST_REWIND;

        case BT_SINK_SRV_ACTION_GET_PLAY_STATUS:
            return BT_LE_AUDIO_SINK_ACTION_MEDIA_READ_MEDIA_STATE;

        case BT_SINK_SRV_ACTION_GET_CAPABILITY:
            return BT_LE_AUDIO_SINK_ACTION_MEDIA_READ_MEDIA_CONTROL_OPCODES_SUPPORTED;

        /*VCP*/
        case BT_SINK_SRV_ACTION_VOLUME_UP:
        case BT_SINK_SRV_ACTION_CALL_VOLUME_UP:
            return BT_SINK_SRV_LE_VCS_ACTION_UNMUTE_RELATIVE_VOLUME_UP;

        case BT_SINK_SRV_ACTION_VOLUME_DOWN:
        case BT_SINK_SRV_ACTION_CALL_VOLUME_DOWN:
            return BT_SINK_SRV_LE_VCS_ACTION_UNMUTE_RELATIVE_VOLUME_DOWN;

        default:
            return BT_LE_AUDIO_SINK_ACTION_INVALID;
    }
}


uint8_t bt_sink_srv_cap_get_link_index(bt_handle_t connect_handle)
{
	uint8_t index;

    for(index = 0; index < g_sink_srv_cap_ctl.max_link_num; index++)
    {
        if(g_sink_srv_cap_link[index].connect_handle == connect_handle)
        {
			break;
        }
    }

	le_audio_log("[sink cap] get ble link index:%d, connect handle:%d", 2, index, connect_handle);

    return index;
}

bt_handle_t bt_sink_srv_cap_check_links_state(bt_sink_srv_cap_state state)
{
	uint8_t index, cur_state = BT_SINK_SRV_CAP_STATE_INVALID;
    bt_handle_t handle = BT_HANDLE_INVALID;

    switch (state) {
        case BT_SINK_SRV_CAP_STATE_IDLE:
        case BT_SINK_SRV_CAP_STATE_ASE_STREAMING:
        case BT_SINK_SRV_CAP_STATE_ASE_STREAMING_MUSIC:
        case BT_SINK_SRV_CAP_STATE_ASE_STREAMING_CALL: {
            for (index = 0; index < g_sink_srv_cap_ctl.max_link_num; index++) {
                if (g_sink_srv_cap_link[index].state == state) {
                    handle = g_sink_srv_cap_link[index].connect_handle;
                    cur_state = g_sink_srv_cap_link[index].state;
        			break;
                }
            }
            break;
        }

        case BT_SINK_SRV_CAP_STATE_CONNECTED: {
            for (index = 0; index < g_sink_srv_cap_ctl.max_link_num; index++) {
                if (g_sink_srv_cap_link[index].state >= BT_SINK_SRV_CAP_STATE_CONNECTED) {
                    handle = g_sink_srv_cap_link[index].connect_handle;
                    cur_state = g_sink_srv_cap_link[index].state;
                    break;
                }
            }
            break;
        }

        default:
            break;
    }
    le_audio_log("[sink cap] check all ble links state, handle:%04X, cur_state:%d, check_state:%d", 3, handle, cur_state, state);
    return handle;
}

void bt_sink_srv_cap_set_sub_state(bt_handle_t connect_handle,uint8_t sub_state)
{
    uint8_t link_index = bt_sink_srv_cap_get_link_index(connect_handle);

    le_audio_log("[sink cap] set sink srv sub state conn handle:%04x, current sub state:%d, next sub state:%d", 3,
        connect_handle, g_sink_srv_cap_link[link_index].sub_state, sub_state);

    g_sink_srv_cap_link[link_index].sub_state = sub_state;
}

void bt_sink_srv_cap_set_state(bt_handle_t connect_handle, bt_sink_srv_cap_state state)
{
    uint8_t link_index = bt_sink_srv_cap_get_link_index(connect_handle);
    uint8_t sub_state = bt_sink_srv_cap_get_sub_state(connect_handle);
    bt_sink_srv_cap_event_ase_state_t notify = {0};
    notify.connect_handle = connect_handle;
    notify.pre_state = g_sink_srv_cap_link[link_index].state;

    le_audio_log("[sink cap] set sink srv state conn handle:%04x, current state:%d, next state:%d, sub_state:%d", 4,
        connect_handle, g_sink_srv_cap_link[link_index].state, state, sub_state);

    if(state == BT_SINK_SRV_CAP_STATE_ASE_STREAMING) {
        switch(sub_state) {
            /*Music*/
            case BT_SINK_SRV_CAP_SUB_STATE_ASE_MUSIC_ENABLING:
                g_sink_srv_cap_link[link_index].state = BT_SINK_SRV_CAP_STATE_ASE_STREAMING_MUSIC;
                g_sink_srv_cap_link[link_index].sub_state = CAP_INVALID_UINT8;
                notify.current_state = BT_SINK_SRV_CAP_STATE_ASE_STREAMING_MUSIC;
                break;

            /*Call 2 ASEs*/
            case BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_ENABLING_2_ASE:
                /*wait second ASE ready, need not to notify upper layer*/
                g_sink_srv_cap_link[link_index].sub_state = BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_PREPARE_STREAMING_2_ASE_1;
                return;

            case BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_PREPARE_STREAMING_2_ASE_1:
                g_sink_srv_cap_link[link_index].state = BT_SINK_SRV_CAP_STATE_ASE_STREAMING_CALL;
                g_sink_srv_cap_link[link_index].sub_state = CAP_INVALID_UINT8;
                notify.current_state = BT_SINK_SRV_CAP_STATE_ASE_STREAMING_CALL;
                break;

            /*Call 3 ASEs*/
            case BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_ENABLING_3_ASE:
                /* ASE streaming(1/3) */
                g_sink_srv_cap_link[link_index].sub_state = BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_PREPARE_STREAMING_3_ASE_1;
                return;

            case BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_PREPARE_STREAMING_3_ASE_1:
                /* ASE streaming(2/3) */
                g_sink_srv_cap_link[link_index].sub_state = BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_PREPARE_STREAMING_3_ASE_2;
                return;

            case BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_PREPARE_STREAMING_3_ASE_2:
                g_sink_srv_cap_link[link_index].state = BT_SINK_SRV_CAP_STATE_ASE_STREAMING_CALL;
                g_sink_srv_cap_link[link_index].sub_state = CAP_INVALID_UINT8;
                notify.current_state = BT_SINK_SRV_CAP_STATE_ASE_STREAMING_CALL;
                break;

            default:
                if (BT_SINK_SRV_CAP_STATE_ASE_STREAMING_MUSIC == g_sink_srv_cap_link[link_index].state) {
                    /*2 ASEs streaming*/
                    notify.current_state = BT_SINK_SRV_CAP_STATE_ASE_STREAMING_MUSIC;
                }
                break;
        }
    }else {
        g_sink_srv_cap_link[link_index].state = state;
        notify.current_state = g_sink_srv_cap_link[link_index].state;
        //return;
    }

    if(notify.pre_state == BT_SINK_SRV_CAP_STATE_IDLE && notify.current_state == BT_SINK_SRV_CAP_STATE_CONNECTED) {
        return;
    } else if(notify.current_state != notify.pre_state) {
        bt_sink_srv_cap_inform_app(BT_SINK_SRV_CAP_EVENT_ASE_STATE, &notify);
    }
}

void bt_sink_srv_cap_update_connection_interval(bt_handle_t handle, uint16_t conn_interval)
{
    bt_gap_le_srv_conn_params_t *current_params = (bt_gap_le_srv_conn_params_t *)bt_gap_le_srv_get_current_conn_params(handle);
    bt_gap_le_srv_conn_params_t new_parm;

    if (NULL == current_params || conn_interval == current_params->conn_interval) {
        return;
    }

    new_parm.conn_interval = conn_interval;
    new_parm.conn_latency = current_params->conn_latency;
    new_parm.supervision_timeout = current_params->supervision_timeout;
    bt_gap_le_srv_update_conn_params(handle, &new_parm, NULL);
}
#endif

bool bt_sink_srv_cap_inform_app(bt_sink_srv_cap_event_id_t event_id, void *p_msg)
{
	bool is_callback_ok = false;

	switch(event_id)
	{

#ifdef AIR_LE_AUDIO_CIS_ENABLE
        case BT_SINK_SRV_CAP_EVENT_ASE_STATE:
        case BT_SINK_SRV_CAP_EVENT_ASE_UPDATE_METADATA:
#endif
        case BT_SINK_SRV_CAP_EVENT_BASE_BROADCAST_AUDIO_ANNOUNCEMENTS:
        case BT_SINK_SRV_CAP_EVENT_BASE_PERIODIC_ADV_SYNC_ESTABLISHED:
        case BT_SINK_SRV_CAP_EVENT_BASE_BASIC_AUDIO_ANNOUNCEMENTS:
        case BT_SINK_SRV_CAP_EVENT_BASE_BIGINFO_ADV_REPORT:
        case BT_SINK_SRV_CAP_EVENT_BASE_BIG_SYNC_IND:
        case BT_SINK_SRV_CAP_EVENT_BASE_BIG_SYNC_ESTABLISHED:
        case BT_SINK_SRV_CAP_EVENT_BASE_PERIODIC_ADV_TERMINATE:
        case BT_SINK_SRV_CAP_EVENT_BASE_BIG_TERMINATE_IND:
        case BT_SINK_SRV_CAP_EVENT_BASE_BIG_TERMINATE_CFM:
        case BT_SINK_SRV_CAP_EVENT_BASE_SCAN_TIMEOUT:
        case BT_SINK_SRV_CAP_EVENT_BASE_SCAN_STOPPED:
        case BT_SINK_SRV_CAP_EVENT_BASE_BASS_ADD_SOURCE:

			if(g_sink_srv_cap_ctl.callback)
			{
				is_callback_ok = true;
			}
			break;

		default:
			break;
	}

	le_audio_log("[sink cap] callback to app, event:%x, ok:%d", 2, event_id, is_callback_ok);
	if(is_callback_ok)
	{
		g_sink_srv_cap_ctl.callback(event_id, p_msg);
	}

    if (NULL != g_sink_srv_cap_broadcast_callback && (event_id & BT_LE_AUDIO_MODULE_BASE)) {
        g_sink_srv_cap_broadcast_callback(event_id, p_msg);
    }

	return is_callback_ok;
}


