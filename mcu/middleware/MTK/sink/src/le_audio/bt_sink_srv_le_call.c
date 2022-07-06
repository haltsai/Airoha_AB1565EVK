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


#include "bt_sink_srv_le_call.h"
#include "bt_sink_srv_le_cap.h"
#include "bt_sink_srv_le_volume.h"
#include "bt_sink_srv_le.h"

#if defined (AIR_LE_AUDIO_ENABLE) && defined (AIR_LE_AUDIO_CIS_ENABLE)
#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:bt_sink_srv_aird_support_call_mode=bt_sink_srv_aird_support_call_mode_default")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_sink_srv_aird_support_call_mode = bt_sink_srv_aird_support_call_mode_default
#else
#error "Unsupported Platform"
#endif

extern bt_le_sink_srv_call_active_handle g_call_active_handle;

bool bt_sink_srv_aird_support_call_mode_default(bt_handle_t handle)
{
    return false;
}

bt_status_t bt_sink_srv_le_call_action_handler(uint32_t action, void *param)
{
    bt_status_t ret = BT_STATUS_FAIL;
#if 0
    bt_handle_t handle = bt_sink_srv_cap_get_link_handle(0xFF);
    le_sink_srv_context_t *srv_cntx = le_sink_srv_get_context(handle);
    bt_sink_srv_report_id("[Sink]bt_sink_srv_le_call_action_handler, action: 0x%x, conn_mask:%x", 2, action, srv_cntx->conn_mask);
    if (handle == BT_HANDLE_INVALID || !(srv_cntx->conn_mask & BT_SINK_SRV_PROFILE_HFP)) {
        ret = BT_STATUS_FAIL;
        return ret;
    }
#endif

    uint32_t le_action = bt_sink_srv_cap_get_le_audio_action(action);

    switch (action) {
        /* User Event */
        case BT_SINK_SRV_ACTION_ANSWER: {
            bt_le_audio_sink_call_action_param_t le_param = {
                .service_idx = BLE_CCP_SERVICE_INDEX_GTBS,
                .length = 2,
            };
            uint8_t buf[2] = {0};
            uint8_t call_index = bt_le_audio_sink_call_check_state(g_call_active_handle.accept_handle, BLE_CCP_SERVICE_INDEX_GTBS, BLE_TBS_STATE_INCOMING);

            if (BLE_TBS_INVALID_CALL_INDEX == call_index) {
                return BT_STATUS_FAIL;
            }

            le_param.call_control_point = (ble_tbs_call_control_point_t*)buf;
            le_param.call_control_point->opcode = BLE_TBS_CALL_CONTROL_OPCODE_TYPE_ACCEPT;
            le_param.call_control_point->params.call_index = call_index;
            ret = bt_le_audio_sink_send_action(g_call_active_handle.accept_handle, le_action, &le_param);
            break;
        }
        case BT_SINK_SRV_ACTION_REJECT: {
            bt_le_audio_sink_call_action_param_t le_param = {
                .service_idx = BLE_CCP_SERVICE_INDEX_GTBS,
                .length = 2,
            };
            uint8_t buf[2] = {0};
            uint8_t call_index = bt_le_audio_sink_call_check_state(g_call_active_handle.reject_handle, BLE_CCP_SERVICE_INDEX_GTBS, BLE_TBS_STATE_INCOMING);

            if (BLE_TBS_INVALID_CALL_INDEX == call_index) {
                return BT_STATUS_FAIL;
            }
            le_param.call_control_point = (ble_tbs_call_control_point_t*)buf;
            le_param.call_control_point->opcode = BLE_TBS_CALL_CONTROL_OPCODE_TYPE_TERMINATE;
            le_param.call_control_point->params.call_index = call_index;
            ret = bt_le_audio_sink_send_action(g_call_active_handle.reject_handle, le_action, &le_param);
            break;
        }
        case BT_SINK_SRV_ACTION_HANG_UP: {
            bt_le_audio_sink_call_action_param_t le_param = {
                .service_idx = BLE_CCP_SERVICE_INDEX_GTBS,
                .length = 2,
            };
            uint8_t buf[2] = {0};
            /* find first valid call index for termination */
            ble_tbs_call_index_t call_index = bt_le_audio_sink_call_get_first(g_call_active_handle.reject_handle, BLE_CCP_SERVICE_INDEX_GTBS);

            le_param.call_control_point = (ble_tbs_call_control_point_t*)buf;
            le_param.call_control_point->opcode = BLE_TBS_CALL_CONTROL_OPCODE_TYPE_TERMINATE;
            le_param.call_control_point->params.call_index = call_index;
            ret = bt_le_audio_sink_send_action(g_call_active_handle.reject_handle, le_action, &le_param);
            break;
        }
        case BT_SINK_SRV_ACTION_3WAY_HOLD_SPECIAL:
        case BT_SINK_SRV_ACTION_3WAY_ADD_HELD_CALL_TO_CONVERSATION: {
            //bt_le_audio_sink_send_action(handle, le_action, &le_param);
            break;
        }
        case BT_SINK_SRV_ACTION_DIAL_NUMBER: {
            //bt_le_audio_sink_send_action(handle, le_action, &le_param);
            break;
        }
        case BT_SINK_SRV_ACTION_DIAL_LAST: {
            bt_le_audio_sink_action_param_t le_param = {
                .service_idx = BLE_CCP_SERVICE_INDEX_GTBS,
            };
            if (bt_sink_srv_aird_support_call_mode(g_call_active_handle.dial_handle)) {
                return BT_STATUS_FAIL;
            }
            ret = bt_le_audio_sink_send_action(g_call_active_handle.dial_handle, BT_LE_AUDIO_SINK_ACTION_CALL_DIAL_LAST, &le_param);
            break;
        }
        case BT_SINK_SRV_ACTION_DIAL_MISSED: {
            bt_le_audio_sink_action_param_t le_param = {
                .service_idx = BLE_CCP_SERVICE_INDEX_GTBS,
            };
            ret = bt_le_audio_sink_send_action(g_call_active_handle.dial_handle, BT_LE_AUDIO_SINK_ACTION_CALL_DIAL_MISSED, &le_param);
            break;
        }
        case BT_SINK_SRV_ACTION_QUERY_CALL_LIST: {
            bt_le_audio_sink_call_list_t* call_list = bt_le_audio_sink_call_get_call_list(g_call_active_handle.dial_handle, BLE_CCP_SERVICE_INDEX_GTBS);
            if (NULL != call_list) {
                //To Do
            }
            break;
        }
        case BT_SINK_SRV_ACTION_3WAY_RELEASE_ALL_HELD: {
            bt_le_audio_sink_action_param_t le_param = {
                .service_idx = BLE_CCP_SERVICE_INDEX_GTBS,
            };
            ret = bt_le_audio_sink_send_action(g_call_active_handle.reject_handle, BT_LE_AUDIO_SINK_ACTION_CALL_RELEASE_ALL_HELD, &le_param);
            break;
        }
        case BT_SINK_SRV_ACTION_3WAY_RELEASE_ACTIVE_ACCEPT_OTHER: {
            bt_le_audio_sink_action_param_t le_param = {
                .service_idx = BLE_CCP_SERVICE_INDEX_GTBS,
            };
            ret = bt_le_audio_sink_send_action(g_call_active_handle.reject_handle, BT_LE_AUDIO_SINK_ACTION_CALL_HOLD_ALL_ACTIVE_ACCEPT_OTHERS, &le_param);
            break;
        }
        case BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER: {
            bt_le_audio_sink_action_param_t le_param = {
                .service_idx = BLE_CCP_SERVICE_INDEX_GTBS,
            };
            ret = bt_le_audio_sink_send_action(g_call_active_handle.reject_handle, BT_LE_AUDIO_SINK_ACTION_CALL_HOLD_ALL_ACTIVE_ACCEPT_OTHERS, &le_param);
            break;
        }
        case BT_SINK_SRV_ACTION_3WAY_EXPLICIT_CALL_TRANSFER: {
            //NA;
            break;
        }
        case BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE: {
            //TO Do;
            break;
        }
        case BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH: {
            //TO Do; switch current esco with sp or device
            break;
        }
        case BT_SINK_SRV_ACTION_SWITCH_AUDIO_DEVICE: {
            //TO Do,two sp switch;
            break;
        }
        case BT_SINK_SRV_ACTION_3WAY_RELEASE_SPECIAL: {
            //TO Do ?;
            break;
        }
        case BT_SINK_SRV_ACTION_CALL_VOLUME_UP:
        case BT_SINK_SRV_ACTION_CALL_VOLUME_DOWN:
        case BT_SINK_SRV_ACTION_CALL_VOLUME_MAX:
        case BT_SINK_SRV_ACTION_CALL_VOLUME_MIN:
        case BT_SINK_SRV_ACTION_CALL_SET_VOLUME: {
            bt_sink_srv_le_volume_vcp_send_action(g_call_active_handle.reject_handle, le_action, NULL);
            break;
        }
        case BT_SINK_SRV_ACTION_DTMF: {
            //NA;
            break;
        }
        case BT_SINK_SRV_ACTION_REPORT_BATTERY: {
            //NA
            break;
        }
        case BT_SINK_SRV_ACTION_HF_GET_SIRI_STATE: {
            //NA
            break;
        }
        case BT_SINK_SRV_ACTION_HF_ECNR_ACTIVATE: {
            //NA
            break;
        }
        default: {
            //bt_sink_srv_report_id("[CALL][HF]Unexcepted action:0x%x", 1, action);
            ret = BT_STATUS_FAIL;
            break;
        }
    }
    return ret;
}

extern void bt_app_common_at_cmd_print_report(char *string);

bt_status_t bt_sink_srv_le_call_ccp_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    if (NULL == buffer) {
        return BT_STATUS_FAIL;
    }

#if 0
    switch (msg) {
        case BLE_CCP_READ_BEARER_PROVIDER_NAME_CNF: {
            ble_ccp_read_bearer_provider_name_cnf_t *cnf = (ble_ccp_read_bearer_provider_name_cnf_t *)buffer;
            le_audio_log("[CCP][sink_call] [%d] READ_BEARER_PROVIDER_NAME_CNF, handle:%x status:0x%x", 3, cnf->service_idx, cnf->handle, status);
        } break;

        case BLE_CCP_READ_BEARER_UCI_CNF: {
            ble_ccp_read_bearer_uci_cnf_t *cnf = (ble_ccp_read_bearer_uci_cnf_t *)buffer;
            le_audio_log("[CCP][sink_call] [%d] READ_BEARER_UCI_CNF, handle:%x status:0x%x", 3, cnf->service_idx, cnf->handle, status);
        } break;

        case BLE_CCP_READ_BEARER_TECHNOLOGY_CNF: {
            ble_ccp_read_bearer_technology_cnf_t *cnf = (ble_ccp_read_bearer_technology_cnf_t *)buffer;
            le_audio_log("[CCP][sink_call] [%d] READ_BEARER_TECHNOLOGY_CNF, handle:%x status:0x%x", 3, cnf->service_idx, cnf->handle, status);
        } break;

        case BLE_CCP_READ_BEARER_URI_SCHEMES_SUPPORTED_LIST_CNF: {
            ble_ccp_read_bearer_uri_schemes_supported_list_cnf_t *cnf = (ble_ccp_read_bearer_uri_schemes_supported_list_cnf_t *)buffer;
            le_audio_log("[CCP][sink_call] [%d] READ_BEARER_URI_SCHEMES_SUPPORTED_LIST_CNF, handle:%x status:0x%x", 3, cnf->service_idx, cnf->handle, status);
        } break;

        case BLE_CCP_READ_BEARER_SIGNAL_STRENGTH_CNF: {
            ble_ccp_read_bearer_signal_strength_cnf_t *cnf = (ble_ccp_read_bearer_signal_strength_cnf_t *)buffer;
            le_audio_log("[CCP][sink_call] [%d] READ_BEARER_SIGNAL_STRENGTH_CNF, handle:%x status:0x%x", 3, cnf->service_idx, cnf->handle, status);
        } break;

        case BLE_CCP_READ_BEARER_SIGNAL_STRENGTH_REPORTING_INTERVAL_CNF: {
            ble_ccp_read_bearer_signal_strength_reporting_interval_cnf_t *cnf = (ble_ccp_read_bearer_signal_strength_reporting_interval_cnf_t *)buffer;
            le_audio_log("[CCP][sink_call] [%d] READ_BEARER_SIGNAL_STRENGTH_REPORTING_INTERVAL_CNF, handle:%x status:0x%x", 3, cnf->service_idx, cnf->handle, status);
        } break;

        case BLE_CCP_READ_BEARER_LIST_CURRENT_CALLS_CNF: {
            uint8_t i;
            ble_ccp_read_bearer_list_current_calls_cnf_t *cnf = (ble_ccp_read_bearer_list_current_calls_cnf_t *)buffer;
            le_audio_log("[CCP][sink_call] [%d] READ_BEARER_LIST_CURRENT_CALLS_CNF, handle:%x status:0x%x totalLen:%d", 3,  cnf->service_idx, cnf->handle, status, cnf->length);
        } break;

        case BLE_CCP_READ_CONTENT_CONTROL_ID_CNF: {
            ble_ccp_read_content_control_id_cnf_t *cnf = (ble_ccp_read_content_control_id_cnf_t *)buffer;
            le_audio_log("[CCP][sink_call] [%d] READ_CONTENT_CONTROL_ID_CNF, handle:%x status:0x%x", 3, cnf->service_idx, cnf->handle, status);
        } break;

        case BLE_CCP_READ_STATUS_FLAGS_CNF: {
            ble_ccp_read_status_flags_cnf_t *cnf = (ble_ccp_read_status_flags_cnf_t *)buffer;
            le_audio_log("[CCP][sink_call] [%d] READ_STATUS_FLAGS_CNF, handle:%x status:0x%x", 3, cnf->service_idx, cnf->handle, status);
        } break;

        case BLE_CCP_READ_INCOMING_CALL_TARGET_BEARER_URI_CNF: {
            ble_ccp_read_incoming_call_target_bearer_uri_cnf_t *cnf = (ble_ccp_read_incoming_call_target_bearer_uri_cnf_t *)buffer;
            le_audio_log("[CCP][sink_call] [%d] READ_INCOMING_CALL_TARGET_BEARER_URI_CNF, handle:%x status:0x%x, callId:%d", 3, cnf->service_idx, cnf->handle, status, cnf->incoming_call->call_index);
        } break;

        case BLE_CCP_READ_CALL_STATE_CNF: {
            ble_ccp_read_call_state_cnf_t *event = (ble_ccp_read_call_state_cnf_t *)buffer;
            ble_ccp_call_state_t *pMsg = event->call_state_list;
            uint8_t i, j;
            i = event->length/sizeof(ble_ccp_call_state_t);
            le_audio_log("[CCP][sink_call] [%d] READ_CALL_STATE_CNF, status:0x%x, msgLen:%d", 3, event->service_idx, status, event->length);
            for (j = 0; j < i; j++) {
                le_audio_log("[CCP][sink_call] [%d] CallId:%d State:0x%x CallFlags:0x%x", 4, j, pMsg[j].call_index, pMsg[j].state, pMsg[j].call_flags);
            }
        } break;

        case BLE_CCP_READ_CALL_CONTROL_POINT_OPTIONAL_OPCODES_CNF: {
            ble_ccp_read_call_control_point_optional_opcodes_cnf_t *event = (ble_ccp_read_call_control_point_optional_opcodes_cnf_t *)buffer;
            le_audio_log("[CCP][sink_call] [%d] READ_CALL_CONTROL_POINT_OPTIONAL_OPCODES_CNF, status:0x%x, local_hold:%x join:%x reserved:%x reserved_byte:%x", 6,
                event->service_idx,
                status,
                event->call_control_point_optional_opcodes.local_hold,
                event->call_control_point_optional_opcodes.join,
                event->call_control_point_optional_opcodes.reserved,
                event->call_control_point_optional_opcodes.reserved_byte);
        } break;

        case BLE_CCP_READ_INCOMING_CALL_CNF: {
            ble_ccp_read_incoming_call_cnf_t *event = (ble_ccp_read_incoming_call_cnf_t *)buffer;
            ble_ccp_incoming_call_t *pMsg = event->incoming_call;
            le_audio_log("[CCP][sink_call] [%d] READ_INCOMING_CALL_CNF, status:0x%x, callID:%d", 3, event->service_idx, status, pMsg->call_index);
        } break;

        case BLE_CCP_READ_CALL_FRIENDLY_NAME_CNF: {
            ble_ccp_read_call_friendly_name_cnf_t *event = (ble_ccp_read_call_friendly_name_cnf_t *)buffer;
            ble_ccp_call_friendly_name_t *pMsg = event->friendly_name;
            le_audio_log("[CCP][sink_call] [%d] READ_CALL_FRIENDLY_NAME_CNF, callID:%d", 3, event->service_idx, status, pMsg->call_index);
        } break;

        case BLE_CCP_SET_BEARER_PROVIDER_NAME_NOTIFICATION_CNF: {
            ble_ccp_event_parameter_t *event = (ble_ccp_event_parameter_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] SET_BEARER_PROVIDER_NAME_NOTIFICATION_CNF, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_SET_BEARER_TECHNOLOGY_NOTIFICATION_CNF: {
            ble_ccp_event_parameter_t *event = (ble_ccp_event_parameter_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] SET_BEARER_TECHNOLOGY_NOTIFICATION_CNF, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_SET_BEARER_URI_SCHEMES_SUPPORTED_LIST_NOTIFICATION_CNF: {
            ble_ccp_event_parameter_t *event = (ble_ccp_event_parameter_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] SET_BEARER_URI_SCHEMES_SUPPORTED_LIST_NOTIFICATION_CNF, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_SET_BEARER_SIGNAL_STRENGTH_NOTIFICATION_CNF: {
            ble_ccp_event_parameter_t *event = (ble_ccp_event_parameter_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] SET_BEARER_SIGNAL_STRENGTH_NOTIFICATION_CNF, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_SET_BEARER_LIST_CURRENT_CALLS_NOTIFICATION_CNF: {
            ble_ccp_event_parameter_t *event = (ble_ccp_event_parameter_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] SET_BEARER_LIST_CURRENT_CALLS_NOTIFICATION_CNF, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_SET_STATUS_FLAGS_NOTIFICATION_CNF: {
            ble_ccp_event_parameter_t *event = (ble_ccp_event_parameter_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] SET_STATUS_FLAGS_NOTIFICATION_CNF, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_SET_INCOMING_CALL_TARGET_BEARER_URI_NOTIFICATION_CNF: {
            ble_ccp_event_parameter_t *event = (ble_ccp_event_parameter_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] SET_INCOMING_CALL_TARGET_BEARER_URI_NOTIFICATION_CNF, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_SET_CALL_STATE_NOTIFICATION_CNF: {
            ble_ccp_event_parameter_t *event = (ble_ccp_event_parameter_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] SET_CALL_STATE_NOTIFICATION_CNF, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_SET_TERMINATION_REASON_NOTIFICATION_CNF: {
            ble_ccp_event_parameter_t *event = (ble_ccp_event_parameter_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] SET_TERMINATION_REASON_NOTIFICATION_CNF, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_SET_INCOMING_CALL_NOTIFICATION_CNF: {
            ble_ccp_event_parameter_t *event = (ble_ccp_event_parameter_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] SET_INCOMING_CALL_NOTIFICATION_CNF, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_SET_CALL_FRIENDLY_NAME_NOTIFICATION_CNF: {
            ble_ccp_event_parameter_t *event = (ble_ccp_event_parameter_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] SET_CALL_FRIENDLY_NAME_NOTIFICATION_CNF, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_BEARER_PROVIDER_NAME_IND: {
            ble_ccp_bearer_provider_name_ind_t *event = (ble_ccp_bearer_provider_name_ind_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] BEARER_PROVIDER_NAME_IND, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_BEARER_TECHNOLOGY_IND: {
            ble_ccp_bearer_technology_ind_t *event = (ble_ccp_bearer_technology_ind_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] BEARER_TECHNOLOGY_IND, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_BEARER_URI_SCHEMES_SUPPORTED_LIST_IND: {
            ble_ccp_bearer_uri_schemes_supported_list_ind_t *event = (ble_ccp_bearer_uri_schemes_supported_list_ind_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] BEARER_URI_SCHEMES_SUPPORTED_LIST_IND, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_BEARER_SIGNAL_STRENGTH_IND: {
            ble_ccp_bearer_signal_strength_ind_t *event = (ble_ccp_bearer_signal_strength_ind_t*)buffer;
            le_audio_log("[CCP][sink_call] [%d] BEARER_SIGNAL_STRENGTH_IND, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_BEARER_LIST_CURRENT_CALLS_IND: {
            ble_ccp_bearer_list_current_calls_ind_t *event = (ble_ccp_read_bearer_list_current_calls_cnf_t *)buffer;
            le_audio_log("[CCP][sink_call] [%d] BEARER_LIST_CURRENT_CALLS_IND, status:0x%x, totalLen:%d", 3, event->service_idx, status, event->length);
        } break;

        case BLE_CCP_STATUS_FLAGS_IND: {
            ble_ccp_status_flags_ind_t *event = (ble_ccp_status_flags_ind_t *)buffer;
            le_audio_log("[CCP][sink_call] [%d] BLE_CCP_STATUS_FLAGS_IND, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_INCOMING_CALL_TARGET_BEARER_URI_IND: {
            ble_ccp_incoming_call_target_bearer_uri_ind_t *event = (ble_ccp_incoming_call_target_bearer_uri_ind_t *)buffer;
            ble_ccp_incoming_call_target_bearer_uri_t *pMsg = event->incoming_call;
            le_audio_log("[CCP][sink_call] [%d] BLE_CCP_INCOMING_CALL_TARGET_BEARER_URI_IND, status:0x%x, callId:%d", 3, event->service_idx, status, pMsg->call_index);
        } break;

        case BLE_CCP_CALL_STATE_IND: {
            ble_ccp_call_state_ind_t *event = (ble_ccp_call_state_ind_t *)buffer;
            ble_ccp_call_state_t *pMsg = event->call_state_list;
            uint8_t i, j;
            i = event->length/sizeof(ble_ccp_call_state_t);
            le_audio_log("[CCP][sink_call] [%d] BLE_CCP_CALL_STATE_IND, status:0x%x, msgLen:%x", 3, event->service_idx, status, event->length);
            for (j = 0; j < i; j++) {
                le_audio_log("[CCP][sink_call] [%d] CallId:%d State:0x%x CallFlags:%x", 4, j, pMsg[j].call_index, pMsg[j].state, pMsg[j].call_flags);
            }
        } break;

        case BLE_CCP_TERMINATION_REASON_IND: {
            ble_ccp_termination_reason_ind_t *event = (ble_ccp_termination_reason_ind_t *)buffer;
            le_audio_log("[CCP][sink_call] [%d] BLE_CCP_TERMINATION_REASON_IND, status:0x%x", 2, event->service_idx, status);
        } break;

        case BLE_CCP_INCOMING_CALL_IND: {
            ble_ccp_incoming_call_ind_t *event = (ble_ccp_incoming_call_ind_t *)buffer;
            ble_ccp_incoming_call_t *pMsg = event->incoming_call;
            le_audio_log("[CCP][sink_call] [%d] BLE_CCP_INCOMING_CALL_IND, status:0x%x, callID:%x", 3, event->service_idx, status, pMsg->call_index);
        } break;

        case BLE_CCP_CALL_FRIENDLY_NAME_IND: {
            ble_ccp_call_friendly_name_ind_t *event = (ble_ccp_call_friendly_name_ind_t *)buffer;
            ble_ccp_call_friendly_name_t *pMsg = event->friendly_name;
            le_audio_log("[CCP][sink_call] [%d] BLE_CCP_CALL_FRIENDLY_NAME_IND, status:0x%x, callID:%x", 3, event->service_idx, status, pMsg->call_index);
        } break;
    }
#endif

    return BT_STATUS_SUCCESS;
}

#endif




