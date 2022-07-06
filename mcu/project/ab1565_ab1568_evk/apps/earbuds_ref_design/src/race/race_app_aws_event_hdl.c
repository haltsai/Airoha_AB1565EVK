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
 * File: race_app_aws_event_hdl.c
 *
 * Description: This file processes the AWS MCE related connection manager events and the AWS MCE report.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#include "race_cmd_feature.h"
#ifdef RACE_AWS_ENABLE
#include "syslog.h"
#include "bt_sink_srv.h"
#include "bt_connection_manager.h"
#include "bt_device_manager.h"
#ifdef RACE_FOTA_BLE_ENABLE
#include "bt_gap_le.h"
#endif
#include "race_xport.h"
#include "race_lpcomm_aws.h"
#include "race_lpcomm_packet.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_trans.h"
#include "race_lpcomm_conn.h"
#include "race_event.h"
#include "race_fota_util.h"
#include "bt_app_common.h"
#include "race_app_aws_event_hdl.h"


log_create_module(race_app_aws, PRINT_LEVEL_INFO);

#ifndef MTK_DEBUG_LEVEL_NONE
#define RACE_APP_AWS_LOG_E(fmt,arg...)   LOG_E(race_app_aws, fmt,##arg)
#define RACE_APP_AWS_LOG_W(fmt,arg...)   LOG_W(race_app_aws, fmt,##arg)
#define RACE_APP_AWS_LOG_I(fmt,arg...)   LOG_I(race_app_aws, fmt,##arg)
#define RACE_APP_AWS_LOG_D(fmt,arg...)

#define RACE_APP_AWS_LOG_MSGID_E(fmt,arg...)   LOG_MSGID_E(race_app_aws, fmt,##arg)
#define RACE_APP_AWS_LOG_MSGID_W(fmt,arg...)   LOG_MSGID_W(race_app_aws, fmt,##arg)
#define RACE_APP_AWS_LOG_MSGID_I(fmt,arg...)   LOG_MSGID_I(race_app_aws, fmt,##arg)
#define RACE_APP_AWS_LOG_MSGID_D(fmt,arg...)
#else
#define RACE_APP_AWS_LOG_E(fmt,arg...)
#define RACE_APP_AWS_LOG_W(fmt,arg...)
#define RACE_APP_AWS_LOG_I(fmt,arg...)

#define RACE_APP_AWS_LOG_MSGID_E(fmt,arg...)
#define RACE_APP_AWS_LOG_MSGID_W(fmt,arg...)
#define RACE_APP_AWS_LOG_MSGID_I(fmt,arg...)
#define RACE_APP_AWS_LOG_MSGID_D(fmt,arg...)
#endif


bool g_race_app_aws_is_connected; /* This variable records if the AWS MCE connection is connected or not. */


void race_app_aws_connected_hdl(race_lpcomm_role_enum device_role)
{
    uint8_t device_id = 0;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;

    if (g_race_app_aws_is_connected) {
        return;
    }

    g_race_app_aws_is_connected = TRUE;

    ret = race_lpcomm_attach_proc(&device_id,
                                  NULL,
                                  0,
                                  device_role,
                                  RACE_LPCOMM_TRANS_METHOD_AWS);

    RACE_APP_AWS_LOG_MSGID_I("ret:%d, device_id:%d", 2, ret, device_id);
}


void race_app_aws_disconnected_hdl(race_lpcomm_role_enum device_role)
{
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;

    if (g_race_app_aws_is_connected) {
        g_race_app_aws_is_connected = FALSE;
    }

    ret = race_lpcomm_deattach_proc(NULL,
                                    0,
                                    device_role,
                                    RACE_LPCOMM_TRANS_METHOD_AWS);
    RACE_APP_AWS_LOG_MSGID_I("ret:%d", 1, ret);
}


void race_app_aws_mce_report_handler(bt_aws_mce_report_info_t *app_report)
{
    race_lpcomm_packet_struct *packet = NULL;
    race_general_msg_t msg_queue_item = {0};
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#ifdef RACE_LPCOMM_MULTIPLE_LINK_ENABLE
    race_lpcomm_conn_info_struct *conn_info = NULL;
#endif

    RACE_APP_AWS_LOG_MSGID_I("role:%x app_report:%x data_len:%d, module:%d", 4,
                             role,
                             app_report,
                             app_report ? app_report->param_len : -1,
                             app_report ? app_report->module_id : 0xFF);

    if (!app_report || BT_AWS_MCE_REPORT_MODULE_FOTA != app_report->module_id ||
        app_report->param_len < sizeof(race_lpcomm_packet_struct) ||
        (BT_AWS_MCE_ROLE_AGENT != role && BT_AWS_MCE_ROLE_PARTNER != role)) {
        return;
    }

#ifdef RACE_LPCOMM_MULTIPLE_LINK_ENABLE
    if (BT_AWS_MCE_ROLE_AGENT == role) {
        conn_info = race_lpcomm_find_conn_info(NULL,
                                               0,
                                               RACE_LPCOMM_ROLE_PARTNER,
                                               RACE_LPCOMM_TRANS_METHOD_AWS);

    } else {
        conn_info = race_lpcomm_find_conn_info(NULL,
                                               0,
                                               RACE_LPCOMM_ROLE_AGENT,
                                               RACE_LPCOMM_TRANS_METHOD_AWS);
    }

    if (!conn_info) {
        RACE_APP_AWS_LOG_MSGID_W("Cannot find corresponding device_id.", 0);
        return;
    }

    msg_queue_item.dev_t = conn_info->device_id;
#else
    msg_queue_item.dev_t = RACE_LPCOMM_DEFAULT_DEVICE_ID;
#endif

    packet = pvPortMalloc(app_report->param_len);
    if (!packet) {
        RACE_APP_AWS_LOG_MSGID_W("Not enought memory.", 0);
        return;
    }

    memcpy(packet, app_report->param, app_report->param_len);

    msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_LPCOMM_DATA_RECV_IND;
    msg_queue_item.msg_data = (uint8_t *)packet;
    /* Send a message to notify the race task that there are AWS data received. */
    ret = race_send_msg(&msg_queue_item);
    if (RACE_ERRCODE_SUCCESS != ret) {
        RACE_APP_AWS_LOG_MSGID_W("Data loss. packet_type:%x process_id:%d, cmd_id:%d, channel_id:%d", 4,
                                 packet->packet_type, packet->process_id, packet->cmd_id, packet->channel_id);
        vPortFree(packet);
    }
}


bt_status_t race_app_aws_cm_event_handler(bt_cm_event_t event_id, void *parameters)
{
    race_lpcomm_role_enum device_role = RACE_LPCOMM_ROLE_NONE;
    bt_aws_mce_role_t bt_device_role = bt_device_manager_aws_local_info_get_role();

    if (BT_AWS_MCE_ROLE_AGENT == bt_device_role) {
        device_role = RACE_LPCOMM_ROLE_AGENT;
    } else if (BT_AWS_MCE_ROLE_PARTNER == bt_device_role) {
        device_role = RACE_LPCOMM_ROLE_PARTNER;
    }

    RACE_APP_AWS_LOG_MSGID_I("race_app_aws_cm_event_handler event_id:%x param:%x device_role:%d", 3, event_id, parameters, device_role);
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            /* Handle the event that updates the remote information. */
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)parameters;

            if (remote_update) {
                RACE_APP_AWS_LOG_MSGID_I("profile:0x%x -> 0x%x, reason :0x%x", 3,
                                         remote_update->pre_connected_service, remote_update->connected_service,
                                         remote_update->reason);

                if (!(BT_CM_PROFILE_SERVICE_MASK((uint8_t)BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service) &&
                    (BT_CM_PROFILE_SERVICE_MASK((uint8_t)BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    RACE_APP_AWS_LOG_MSGID_I("AWS connection is set up. addr:%x %x %x %x %x %x", 6,
                                             remote_update->address[0], remote_update->address[1],
                                             remote_update->address[2], remote_update->address[3],
                                             remote_update->address[4], remote_update->address[5]);
                    race_app_aws_connected_hdl(device_role);

                } else if ((BT_CM_PROFILE_SERVICE_MASK((uint8_t)BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service) &&
                           !(BT_CM_PROFILE_SERVICE_MASK((uint8_t)BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    RACE_APP_AWS_LOG_MSGID_I("AWS connection is disconnected. addr:%x %x %x %x %x %x", 6,
                                             remote_update->address[0], remote_update->address[1],
                                             remote_update->address[2], remote_update->address[3],
                                             remote_update->address[4], remote_update->address[5]);
                    race_app_aws_disconnected_hdl(device_role);
                }
            }

            break;
        }

        default: {
            break;
        }
    }

    return BT_STATUS_SUCCESS;
}

#endif /* RACE_AWS_ENABLE */

