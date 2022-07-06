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

/**
 * File: apps_state_report.c
 *
 * Description: This file is the idle activity to receive BT event and report state via AT CMD.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */

#include "atci.h"
#include "bt_sink_srv.h"
#include "ui_shell_manager.h"
#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "FreeRTOS.h"
#include "apps_state_report.h"
#include "bt_device_manager.h"
#include "bt_connection_manager.h"

#define STRCPY_N(dest, source) strncpy(dest, source, strlen(source)+1);

void bt_app_report_state_atci(bt_app_report_type_t type, uint32_t params)
{
    atci_response_t *response = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));

    if (NULL != response) {
        memset(response, 0, sizeof(*response));

        switch (type) {
            case BT_APP_REPORT_TYPE_STATE: {
                bt_sink_srv_state_t state = (bt_sink_srv_state_t)params;
                APPS_LOG_MSGID_I("[APP_state] state:0x%x", 1, state);
                STRCPY_N((char *)response->response_buf, "Sink Status: ");
                uint32_t copy_size = sizeof(response->response_buf) - sizeof("Sink Status: ");
                if (state == BT_SINK_SRV_STATE_STREAMING) {
                    strncat((char *)response->response_buf, "STREAMING.\r\n", copy_size);
                } else if (state == BT_SINK_SRV_STATE_MULTIPARTY) {
                    strncat((char *)response->response_buf, "MULTIPARTY.\r\n", copy_size);
                } else if (state == BT_SINK_SRV_STATE_HELD_REMAINING) {
                    strncat((char *)response->response_buf, "HELD_REMAINING.\r\n", copy_size);
                } else if (state == BT_SINK_SRV_STATE_HELD_ACTIVE) {
                    strncat((char *)response->response_buf, "HELD_ACTIVE.\r\n", copy_size);
                } else  if (state == BT_SINK_SRV_STATE_TWC_OUTGOING) {
                    strncat((char *)response->response_buf, "TWC_OUTGOING.\r\n", copy_size);
                } else if (state == BT_SINK_SRV_STATE_TWC_INCOMING) {
                    strncat((char *)response->response_buf, "TWC_INCOMING.\r\n", copy_size);
                } else if (state == BT_SINK_SRV_STATE_ACTIVE) {
                    strncat((char *)response->response_buf, "ACTIVE.\r\n", copy_size);
                } else if (state == BT_SINK_SRV_STATE_OUTGOING) {
                    strncat((char *)response->response_buf, "OUTGOING.\r\n", copy_size);
                } else if (state == BT_SINK_SRV_STATE_INCOMING) {
                    strncat((char *)response->response_buf, "INCOMING.\r\n", copy_size);
                } else  if (state == BT_SINK_SRV_STATE_CONNECTED) {
                    strncat((char *)response->response_buf, "IDLE (CONNECTED).\r\n", copy_size);
                } else if (state == BT_SINK_SRV_STATE_POWER_ON) {
                    strncat((char *)response->response_buf, "BT_POWER_ON (NOT CONNECTED).\r\n", copy_size);
#if 0
                } else if (state == BT_SINK_SRV_STATE_LOCAL_PLAYING) {
                    strncat((char *)response->response_buf, "LOCAL_MUSIC_PLAYING.\r\n");
#endif
                } else if (state == BT_SINK_SRV_STATE_NONE) {
                    strncat((char *)response->response_buf, "BT_SINK_IDLE.\r\n", copy_size);
                } else {
                    strncat((char *)response->response_buf, "ERROR STATE!\r\n", copy_size);
                }
            }
            break;

            case BT_APP_REPORT_TYPE_CALLER: {
                snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                         "Alert: Incoming call:%s\r\n", (char *)params);
            }
            break;

            case BT_APP_REPORT_TYPE_MISSED_CALL: {
                snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                         "Alert: Missed call:%s\r\n", (char *)params);
            }
            break;

            case BT_APP_REPORT_TYPE_LINK_LOST: {
                STRCPY_N((char *)response->response_buf, "Alert: All Profiles disconnected.\r\n");
            }
            break;

            case BT_APP_REPORT_TYPE_VISIBILITY: {
                if (params == 1) {
                    STRCPY_N((char *)response->response_buf, "Alert: Discoverable.\r\n");
                } else {
                    STRCPY_N((char *)response->response_buf, "Alert: Not discoverable.\r\n");
                }
            }
            break;

            case BT_APP_REPORT_TYPE_AWS_MCE_STATE: {
                bt_sink_srv_aws_mce_state_changed_ind_t *state_change = (bt_sink_srv_aws_mce_state_changed_ind_t *)params;
                bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
                uint8_t link_type[22] = {0};
                if (0 == memcmp((void *)&state_change->bt_addr, (void *)local_addr, sizeof(bt_bd_addr_t))) {
                    STRCPY_N((char *)link_type, "Special AWS link.\r\n");
                } else {
                    STRCPY_N((char *)link_type, "AWS link.\r\n");
                }

                if (state_change->aws_state == BT_AWS_MCE_AGENT_STATE_INACTIVE) {
                    snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                             "Alert: Partner Detached the %s", (char *)link_type);
                } else if (state_change->aws_state == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
                    snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                             "Alert: Partner Attached the %s", (char *)link_type);
                }
            }
            break;

#if 0
            case BT_APP_REPORT_TYPE_AWS_ROLE_HANDOVER_CNF: {
                bt_sink_srv_aws_mce_role_handover_update_t *role_change = (bt_sink_srv_aws_mce_role_handover_update_t *)params;
                uint8_t role_string[20] = {0};
                if (role_change->role == BT_AWS_MCE_ROLE_AGENT) {
                    STRCPY_N((char *)role_string, "(Agent now).\r\n");
                } else if (role_change->role == BT_AWS_MCE_ROLE_PARTNER) {
                    STRCPY_N((char *)role_string, "(Partner now).\r\n");
                } else {
                    STRCPY_N((char *)role_string, "(error now)!\r\n");
                }
                if (role_change->result) {
                    snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                             "Alert:RHO succuss%s", (char *)role_string);
                } else {
                    snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                             "Alert:RHO fail%s", (char *)role_string);
                }
            }
            break;
#endif

            case BT_APP_REPORT_TYPE_PROFILE_CONN_STATE: {
                bt_sink_srv_profile_connection_state_update_t *profile_state = (bt_sink_srv_profile_connection_state_update_t *) params;
                STRCPY_N((char *)response->response_buf, "Alert: ");
                uint32_t copy_size = sizeof(response->response_buf) - sizeof("Alert: ");
                if (profile_state->profile == BT_SINK_SRV_PROFILE_HFP) {
                    strncat((char *)response->response_buf, "HFP ", copy_size);
                } else if (profile_state->profile == BT_SINK_SRV_PROFILE_A2DP_SINK) {
                    strncat((char *)response->response_buf, "A2DP ", copy_size);
                } else if (profile_state->profile == BT_SINK_SRV_PROFILE_AVRCP) {
                    strncat((char *)response->response_buf, "AVRCP ", copy_size);
                }
                copy_size = sizeof(response->response_buf) - strlen((char *)response->response_buf);
                if (profile_state->state == BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED) {
                    strncat((char *)response->response_buf, "disconnected.\r\n", copy_size);
                } else if (profile_state->state == BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED) {
                    strncat((char *)response->response_buf, "connected.\r\n", copy_size);
                }
            }
            break;

            case BT_APP_REPORT_TYPE_NOTI_NEW: {
                STRCPY_N((char *)response->response_buf, "Alert: New notification\r\n");
            }
            break;

            case BT_APP_REPORT_TYPE_HF_SCO_STATE: {
                if (params == 1) {
                    STRCPY_N((char *)response->response_buf, "Alert: SCO connected.\r\n");
                } else {
                    STRCPY_N((char *)response->response_buf, "Alert: SCO disconnected.\r\n");
                }
            }
            break;

            case BT_APP_REPORT_TYPE_NOTI_MISSED_CALL: {
                STRCPY_N((char *)response->response_buf, "Alert: Missed call notification\r\n");
            }
            break;

            case BT_APP_REPORT_TYPE_NOTI_SMS: {
                STRCPY_N((char *)response->response_buf, "Alert: New message\r\n");
            }
            break;

            default:
                break;
        }
        response->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
        response->response_len = strlen((char *)response->response_buf);

        /* Send BT info via ATCI. */
        if (response->response_len > 0) {
            atci_send_response(response);
        }
        vPortFree(response);
    }
}

void bt_sink_app_event_handler(bt_sink_srv_event_t event_id, void *parameters)
{
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)parameters;

    switch (event_id) {
        /* BT_SINK_SRV state change. */
        case BT_SINK_SRV_EVENT_STATE_CHANGE:
            bt_app_report_state_atci(BT_APP_REPORT_TYPE_STATE, (uint32_t)event->state_change.current);
            break;

        /* BT HF SCO state update. */
        case BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE: {
            bt_sink_srv_sco_state_update_t *event =
                (bt_sink_srv_sco_state_update_t *)parameters;
            bt_app_report_state_atci(BT_APP_REPORT_TYPE_HF_SCO_STATE, (uint32_t)event->state);
        }
        break;

        /* BT_SINK_SRV HF Caller information. */
        case BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION:
            if (strlen((char *)event->caller_info.name) > 0) {
                bt_app_report_state_atci(BT_APP_REPORT_TYPE_CALLER, (uint32_t)event->caller_info.name);
            } else if (strlen((char *)event->caller_info.number) > 0) {
                bt_app_report_state_atci(BT_APP_REPORT_TYPE_CALLER, (uint32_t)event->caller_info.number);
            }
            break;

        /* BT_SINK_SRV HF missed call. */
        case BT_SINK_SRV_EVENT_HF_MISSED_CALL:
            if (strlen((char *)event->caller_info.name) > 0) {
                bt_app_report_state_atci(BT_APP_REPORT_TYPE_MISSED_CALL, (uint32_t)event->caller_info.name);
            } else if (strlen((char *)event->caller_info.number) > 0) {
                bt_app_report_state_atci(BT_APP_REPORT_TYPE_MISSED_CALL, (uint32_t)event->caller_info.number);
            }
            break;
        default:
            break;
    }

}

void app_state_report_bt_cm_event_handler(uint32_t event_id, void *extra_data)
{
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (!remote_update) {
                break;
            }
            /* Print BT_CM update log. */
            APPS_LOG_MSGID_I("[APP_state] acl %d -> %d, profile:0x%x -> 0x%x, reason :0x%x", 5,
                             remote_update->pre_acl_state, remote_update->acl_state,
                             remote_update->pre_connected_service, remote_update->connected_service,
                             remote_update->reason);
        }
        break;

        default:
            break;
    }
}

bool app_event_state_report_activity_proc(
    ui_shell_activity_t *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        /* BT Sink event. */
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            bt_sink_app_event_handler((bt_sink_srv_event_t)event_id, extra_data);
            ret = false;
            break;
        }
        /* New BT_CM event. */
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            app_state_report_bt_cm_event_handler(event_id, extra_data);
            ret = false;
            break;
        }
        default:
            break;
    }
    return ret;
}
