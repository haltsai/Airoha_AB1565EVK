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


#include "race_cmd_feature.h"
#ifdef RACE_FOTA_CMD_ENABLE
#include "bt_hfp.h"
#include "bt_a2dp.h"
#include "bt_sink_srv.h"
#include "fota_multi_info_util.h"
#include "fota_flash.h"
#include "fota_multi_info.h"
#include "race_xport.h"
#include "race_util.h"
#include "race_bt.h"
#include "race_lpcomm_msg_struct.h"
#include "race_lpcomm_util.h"
#include "race_timer.h"
#include "race_lpcomm_packet.h"
#include "race_lpcomm_conn.h"
#include "race_cmd_fota.h"
#include "race_fota_util.h"
#include "race_cmd_bluetooth.h"
#include "race_fota.h"
#include "race_storage_access.h"
#include "race_storage_util.h"
#include "race_noti.h"
#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
#include "bt_role_handover.h"
#endif
#ifdef __FOTA_FOR_BISTO__
#include "fota_flash_for_gsound.h"
#endif
#ifdef RACE_CFU_ENABLE
#include "cfu.h"
#endif


typedef struct
{
    race_recipient_type_enum recipient_type;
    race_fota_stop_originator_enum originator;
    race_fota_stop_reason_enum reason;
}race_fota_stop_msg_data_struct;

#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
typedef struct
{
    uint8_t fota_dl_state;
}PACKED race_fota_rho_change_data_struct;
#endif


race_fota_cntx_struct g_race_fota_cntx;
race_fota_cntx_struct *g_race_fota_cntx_ptr;
uint16_t g_race_fota_spp_transmit_interval;
uint16_t g_race_fota_ble_transmit_interval;

#ifdef MTK_RACE_EVENT_ID_ENABLE
/* register_id is not included in g_race_fota_cntx because g_race_fota_cntx is reset frequently. */
int32_t g_race_fota_event_register_id;

static RACE_ERRCODE race_fota_race_event_cb(int32_t register_id, race_event_type_enum event_type, void *param, void *user_data);
#else
static RACE_ERRCODE race_fota_race_event_cb(race_event_type_enum event_type, void *param, void *user_data);
#endif

#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
/* allowed_callback is invoked after RHO_START event. */
bt_status_t race_fota_rho_srv_allowed_callback(const bt_bd_addr_t *addr);

bt_status_t race_fota_rho_srv_get_data_callback(const bt_bd_addr_t *addr, void * data);

uint8_t race_fota_rho_srv_get_length_callback(const bt_bd_addr_t *addr);

bt_status_t race_fota_rho_srv_update_callback(bt_role_handover_update_info_t *info);

void race_fota_rho_srv_status_callback(const bt_bd_addr_t *addr,
                                                bt_aws_mce_role_t role,
                                                bt_role_handover_event_t event,
                                                bt_status_t status);
#endif


RACE_ERRCODE race_fota_reset(void)
{
    if (g_race_fota_cntx_ptr)
    {
        memset(g_race_fota_cntx_ptr, 0, sizeof(race_fota_cntx_struct));
        g_race_fota_cntx_ptr->app_id_timer_id = RACE_TIMER_INVALID_TIMER_ID;
        g_race_fota_cntx_ptr->commit_timer_id = RACE_TIMER_INVALID_TIMER_ID;
#ifdef RACE_LPCOMM_ENABLE
        g_race_fota_cntx_ptr->ping_timer_id = RACE_TIMER_INVALID_TIMER_ID;
        g_race_fota_cntx_ptr->stop_timer_id = RACE_TIMER_INVALID_TIMER_ID;
#endif
        g_race_fota_cntx_ptr->fota_mode = RACE_FOTA_MODE_MAX;
        g_race_fota_cntx_ptr->dl_method = RACE_FOTA_DUAL_DEVICE_DL_METHOD_MAX;

        return RACE_ERRCODE_SUCCESS;
    }

    return RACE_ERRCODE_FAIL;
}


RACE_ERRCODE race_fota_init(void)
{
#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
    bt_role_handover_callbacks_t rho_srv_callbacks = {race_fota_rho_srv_allowed_callback,
                                                              race_fota_rho_srv_get_length_callback,
                                                              race_fota_rho_srv_get_data_callback,
                                                              race_fota_rho_srv_update_callback,
                                                              race_fota_rho_srv_status_callback};
#endif

    if (!g_race_fota_cntx_ptr)
    {
        g_race_fota_cntx_ptr = &g_race_fota_cntx;
    }

#ifdef MTK_RACE_EVENT_ID_ENABLE
    race_event_register(&g_race_fota_event_register_id, race_fota_race_event_cb, NULL);
#else
    race_event_register(race_fota_race_event_cb, NULL);
#endif

    g_race_fota_spp_transmit_interval = RACE_FOTA_DEFAULT_SPP_TRANSMIT_INTERVAL_IN_MS;
    g_race_fota_ble_transmit_interval = RACE_FOTA_DEFAULT_BLE_TRANSMIT_INTERVAL_IN_MS;

#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_FOTA, &rho_srv_callbacks);
#endif

    return race_fota_reset();
}


race_fota_cntx_struct *race_fota_cntx_get(void)
{
    return g_race_fota_cntx_ptr;
}


uint8_t race_fota_app_id_timer_id_get(void)
{
    if (g_race_fota_cntx_ptr)
    {
        if (race_timer_smart_is_enabled(g_race_fota_cntx_ptr->app_id_timer_id))
        {
            return g_race_fota_cntx_ptr->app_id_timer_id;
        }

        g_race_fota_cntx_ptr->app_id_timer_id = RACE_TIMER_INVALID_TIMER_ID;
    }

    return RACE_TIMER_INVALID_TIMER_ID;
}


RACE_ERRCODE race_fota_app_id_timer_id_set(uint8_t timer_id)
{
    if (g_race_fota_cntx_ptr)
    {
        g_race_fota_cntx_ptr->app_id_timer_id = timer_id;

        return RACE_ERRCODE_SUCCESS;
    }

    return RACE_ERRCODE_FAIL;
}


void race_fota_app_id_timer_expiration_hdl(uint8_t id, void *user_data)
{
    RACE_LOG_MSGID_I("id:%d user_data:%x app_id_timer_id:%d",3,
               id,
               user_data,
               g_race_fota_cntx_ptr ? g_race_fota_cntx_ptr->app_id_timer_id : RACE_TIMER_INVALID_TIMER_ID);

    if (!g_race_fota_cntx_ptr || id != g_race_fota_cntx_ptr->app_id_timer_id)
    {
        RACE_LOG_MSGID_W("Timer id not match. id:%d app_id_timer_id:%d",2,
                   id,
                   g_race_fota_cntx_ptr ? g_race_fota_cntx_ptr->app_id_timer_id : RACE_TIMER_INVALID_TIMER_ID);
        /* Must not reset timer id for it may be the previous timer which is not stopped properly. */
        race_timer_smart_stop(id, NULL);
        return;
    }

    /* SP lost */
    g_race_fota_cntx_ptr->sp_online = FALSE;

    race_timer_smart_stop(id, NULL);
    race_fota_app_id_timer_id_set(RACE_TIMER_INVALID_TIMER_ID);
    race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_AGENT,
                   RACE_FOTA_STOP_REASON_SP_LOST);
}


uint8_t race_fota_commit_timer_id_get(void)
{
    if (g_race_fota_cntx_ptr)
    {
        if (race_timer_smart_is_enabled(g_race_fota_cntx_ptr->commit_timer_id))
        {
            return g_race_fota_cntx_ptr->commit_timer_id;
        }

        g_race_fota_cntx_ptr->commit_timer_id = RACE_TIMER_INVALID_TIMER_ID;
    }

    return RACE_TIMER_INVALID_TIMER_ID;
}


RACE_ERRCODE race_fota_commit_timer_id_set(uint8_t timer_id)
{
    if (g_race_fota_cntx_ptr)
    {
        g_race_fota_cntx_ptr->commit_timer_id = timer_id;

        return RACE_ERRCODE_SUCCESS;
    }

    return RACE_ERRCODE_FAIL;
}


void race_fota_commit_timer_expiration_hdl(uint8_t id, void *user_data)
{
    race_fota_commit_timer_msg_struct *commit_timer_msg = (race_fota_commit_timer_msg_struct *)user_data;

    RACE_LOG_MSGID_I("id:%d user_data:%x commit_timer_id:%d",3,
               id,
               user_data,
               g_race_fota_cntx_ptr ? g_race_fota_cntx_ptr->commit_timer_id : RACE_TIMER_INVALID_TIMER_ID);

    if (!g_race_fota_cntx_ptr || id != g_race_fota_cntx_ptr->commit_timer_id)
    {
        RACE_LOG_MSGID_W("Timer id not match. id:%d commit_timer_id:%d",2,
                   id,
                   g_race_fota_cntx_ptr ? g_race_fota_cntx_ptr->commit_timer_id : RACE_TIMER_INVALID_TIMER_ID);
        /* Must not reset timer id for it may be the previous timer which is not stopped properly. */
        race_timer_smart_stop(id, NULL);
        return;
    }

    if (RACE_FOTA_DOWNLOAD_STATE_COMMITING == race_fota_dl_state_get())
    {
        if (commit_timer_msg &&
            RACE_EVENT_REMOTE_DEVICE_ADDRESS_LENGTH >= commit_timer_msg->address_length)
        {
            race_event_send_fota_need_reboot_event(commit_timer_msg->address, commit_timer_msg->address_length);
        }
        else
        {
            race_event_send_fota_need_reboot_event(NULL, 0);
        }
    }

    race_timer_smart_stop(id, NULL);
    if (user_data)
    {
        race_mem_free(user_data);
    }
    race_fota_commit_timer_id_set(RACE_TIMER_INVALID_TIMER_ID);
}


#ifdef RACE_LPCOMM_ENABLE
uint8_t race_fota_ping_timer_id_get(void)
{
    if (g_race_fota_cntx_ptr)
    {
        if (race_timer_smart_is_enabled(g_race_fota_cntx_ptr->ping_timer_id))
        {
            return g_race_fota_cntx_ptr->ping_timer_id;
        }

        g_race_fota_cntx_ptr->ping_timer_id = RACE_TIMER_INVALID_TIMER_ID;
    }

    return RACE_TIMER_INVALID_TIMER_ID;
}


RACE_ERRCODE race_fota_ping_timer_id_set(uint8_t timer_id)
{
    if (g_race_fota_cntx_ptr)
    {
        g_race_fota_cntx_ptr->ping_timer_id = timer_id;

        return RACE_ERRCODE_SUCCESS;
    }

    return RACE_ERRCODE_FAIL;
}


RACE_ERRCODE race_fota_send_ping_req_to_peer(void)
{
    uint8_t channel_id = 0;
    uint16_t process_id = race_gen_process_id();
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;

    RACE_LOG_MSGID_I("Try to send ping", 0);

    ret = race_fota_channel_id_get(&channel_id);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        return ret;
    }

    return race_lpcomm_packet_send_to_peer(NULL,
                                           0,
                                           RACE_LPCOMM_ROLE_AGENT,
                                           RACE_LPCOMM_PACKET_TYPE_COMMON_REQ,
                                           RACE_LPCOMM_COMMON_CMD_ID_FOTA_PING,
                                           RACE_APP_ID_FOTA,
                                           channel_id,
                                           process_id,
                                           RACE_LPCOMM_RETRY_MAX_TIME,
#ifdef RACE_AWS_ENABLE
                                           RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                           RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                           RACE_LPCOMM_DEFAULT_DEVICE_ID);
}


void race_fota_ping_timer_expiration_hdl(uint8_t id, void *user_data)
{
    race_lpcomm_role_enum role = race_fota_get_role();
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();

    RACE_LOG_MSGID_I("id:%d user_data:%x ping_timer_id:%d role:%d dl_state:%d",5,
               id,
               user_data,
               g_race_fota_cntx_ptr ? g_race_fota_cntx_ptr->ping_timer_id : RACE_TIMER_INVALID_TIMER_ID,
               role,
               fota_dl_state);

    if (!g_race_fota_cntx_ptr || id != g_race_fota_cntx_ptr->ping_timer_id)
    {
        RACE_LOG_MSGID_W("Timer id not match. id:%d ping_timer_id:%d",2,
                   id,
                   g_race_fota_cntx_ptr ? g_race_fota_cntx_ptr->ping_timer_id : RACE_TIMER_INVALID_TIMER_ID);
        /* Must not reset timer id for it may be the previous timer which is not stopped properly. */
        race_timer_smart_stop(id, NULL);
        return;
    }

    if (RACE_FOTA_DOWNLOAD_STATE_RHOING == fota_dl_state)
    {
        /* Do nothing if ping timer expires during RHO. */
        return;
    }

    if (RACE_LPCOMM_ROLE_AGENT == role)
    {
        /* Send ping req */
        if (race_fota_is_race_fota_running())
        {
            race_fota_send_ping_req_to_peer();
        }
        else
        {
            race_timer_smart_stop(id, NULL);
            race_fota_ping_timer_id_set(RACE_TIMER_INVALID_TIMER_ID);
        }
    }
    else if (RACE_LPCOMM_ROLE_PARTNER == role)
    {
        /* SP lost */
        g_race_fota_cntx_ptr->lpcomm_peer_online = FALSE;

        race_timer_smart_stop(id, NULL);
        race_fota_ping_timer_id_set(RACE_TIMER_INVALID_TIMER_ID);
        race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_PARTNER,
                       RACE_FOTA_STOP_REASON_AGENT_LOST);
    }
    else
    {
        RACE_LOG_MSGID_W("unexpected role:%d",1, role);
        race_timer_smart_stop(id, NULL);
        race_fota_ping_timer_id_set(RACE_TIMER_INVALID_TIMER_ID);
    }
}
#endif


uint8_t race_fota_stop_timer_id_get(void)
{
    if (g_race_fota_cntx_ptr)
    {
        if (race_timer_smart_is_enabled(g_race_fota_cntx_ptr->stop_timer_id))
        {
            return g_race_fota_cntx_ptr->stop_timer_id;
        }

        g_race_fota_cntx_ptr->stop_timer_id = RACE_TIMER_INVALID_TIMER_ID;
    }

    return RACE_TIMER_INVALID_TIMER_ID;
}


RACE_ERRCODE race_fota_stop_timer_id_set(uint8_t timer_id)
{
    if (g_race_fota_cntx_ptr)
    {
        g_race_fota_cntx_ptr->stop_timer_id = timer_id;

        return RACE_ERRCODE_SUCCESS;
    }

    return RACE_ERRCODE_FAIL;
}


void race_fota_stop_timer_expiration_hdl(uint8_t id, void *user_data)
{
#ifdef RACE_LPCOMM_ENABLE
    race_lpcomm_role_enum role = race_fota_get_role();
#endif
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

    RACE_LOG_MSGID_I("id:%d user_data:%x stop_timer_id:%d dl_state:%d",4,
               id,
               user_data,
               fota_cntx ? fota_cntx->stop_timer_id : 0xFF,
               fota_dl_state);

    if (!fota_cntx || id != fota_cntx->stop_timer_id)
    {
        RACE_LOG_MSGID_W("Timer id not match. id:%d stop_timer_id:%d",2,
                   id,
                   fota_cntx ? fota_cntx->stop_timer_id : RACE_TIMER_INVALID_TIMER_ID);
        /* Must not reset timer id for it may be the previous timer which is not stopped properly. */
        race_timer_smart_stop(id, NULL);
        return;
    }

    if (RACE_FOTA_DOWNLOAD_STATE_STOPPING == fota_dl_state)
    {
#ifdef RACE_LPCOMM_ENABLE
        if (RACE_LPCOMM_ROLE_PARTNER == role)
        {
            race_fota_stop_partner_agent_result_req_struct result_req = {0};

            /* No need to send result_rsp to Agent. */
            result_req.channel_id = 0;
            result_req.process_id = 0;
            result_req.trans_method = RACE_LPCOMM_TRANS_METHOD_NONE;
            result_req.originator = fota_cntx->originator;
            result_req.reason = fota_cntx->reason;
            result_req.result = RACE_ERRCODE_SUCCESS;

            race_fota_stop_partner_result_req_process(&result_req);
        }
        else if (RACE_LPCOMM_ROLE_AGENT == role)
#endif
        {
            race_fota_stop_agent_sp_stop_noti_struct sp_stop_noti = {0};

            sp_stop_noti.status = RACE_ERRCODE_SUCCESS;
            sp_stop_noti.recipient_type = fota_cntx->is_dual_fota ? RACE_RECIPIENT_TYPE_AGENT_PARTNER : RACE_RECIPIENT_TYPE_AGENT_ONLY;

            fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_5D;
            race_fota_stop_agent_sp_stop_noti_process(&sp_stop_noti);
        }
    }

    race_timer_smart_stop(id, NULL);
    race_fota_stop_timer_id_set(RACE_TIMER_INVALID_TIMER_ID);
}


void race_fota_set_fota_mode(race_fota_mode_enum fota_mode)
{
    if (g_race_fota_cntx_ptr)
    {
        g_race_fota_cntx_ptr->fota_mode = fota_mode;
    }
}


bool race_fota_is_active_mode(void)
{
    if (g_race_fota_cntx_ptr)
    {
        return RACE_FOTA_MODE_ACTIVE == g_race_fota_cntx_ptr->fota_mode;
    }

    return FALSE;
}


#ifdef RACE_FOTA_ADAPTIVE_MODE_ENABLE
bool race_fota_is_adaptive_mode(void)
{
    if (g_race_fota_cntx_ptr)
    {
        return RACE_FOTA_MODE_ADAPTIVE == g_race_fota_cntx_ptr->fota_mode;
    }

    return FALSE;
}
#endif


bool race_fota_is_dual_fota(void)
{
    if (g_race_fota_cntx_ptr)
    {
        return g_race_fota_cntx_ptr->is_dual_fota;
    }

    return FALSE;
}


race_fota_download_state_enum race_fota_dl_state_get(void)
{
    if (g_race_fota_cntx_ptr)
    {
        RACE_LOG_MSGID_I("dl_state:%d old_dl_state:%d",2,
                   g_race_fota_cntx_ptr->dl_state,
                   g_race_fota_cntx_ptr->old_dl_state);

        return g_race_fota_cntx_ptr->dl_state;
    }

    return RACE_FOTA_DOWNLOAD_STATE_MAX;
}


void race_fota_dl_state_set(race_fota_download_state_enum state)
{
    if (g_race_fota_cntx_ptr)
    {
        if (state == g_race_fota_cntx_ptr->dl_state)
        {
            return;
        }

        if (RACE_FOTA_DOWNLOAD_STATE_NONE == state ||
            RACE_FOTA_DOWNLOAD_STATE_MAX == state)
        {
            g_race_fota_cntx_ptr->old_dl_state = state;
            g_race_fota_cntx_ptr->dl_state = state;
        }
        else
        {
            g_race_fota_cntx_ptr->old_dl_state = g_race_fota_cntx_ptr->dl_state;
            g_race_fota_cntx_ptr->dl_state = state;
        }
    }
}


void race_fota_dl_state_rollback(void)
{
    if (g_race_fota_cntx_ptr)
    {
        RACE_LOG_MSGID_I("dl_state:%d old_dl_state:%d",2,
                   g_race_fota_cntx_ptr->dl_state,
                   g_race_fota_cntx_ptr->old_dl_state);
        g_race_fota_cntx_ptr->dl_state = g_race_fota_cntx_ptr->old_dl_state;
    }
}


void race_fota_sp_trans_method_set(race_fota_sp_trans_method_enum sp_trans_method)
{
    if (g_race_fota_cntx_ptr)
    {
        g_race_fota_cntx_ptr->sp_trans_method = sp_trans_method;
    }
}


race_fota_sp_trans_method_enum race_fota_sp_trans_method_get(void)
{
    if (g_race_fota_cntx_ptr)
    {
        if (!race_fota_is_race_fota_running())
        {
            g_race_fota_cntx_ptr->sp_trans_method = RACE_FOTA_SP_TRANS_METHOD_NONE;
        }

        return g_race_fota_cntx_ptr->sp_trans_method;
    }

    return RACE_FOTA_SP_TRANS_METHOD_MAX;
}


RACE_ERRCODE race_fota_set_sp_trans_method_by_channel_id(uint8_t channel_id)
{
    race_serial_port_type_enum port_type = race_get_port_type_by_channel_id(channel_id);

    if (RACE_SERIAL_PORT_TYPE_BLE == port_type)
    {
        race_fota_sp_trans_method_set(RACE_FOTA_SP_TRANS_METHOD_BLE);
    }
    else if (RACE_SERIAL_PORT_TYPE_SPP == port_type)
    {
        race_fota_sp_trans_method_set(RACE_FOTA_SP_TRANS_METHOD_SPP);
    }
#ifdef MTK_AIRUPDATE_ENABLE
    else if (RACE_SERIAL_PORT_TYPE_AIRUPDATE == port_type)
    {
        race_fota_sp_trans_method_set(RACE_FOTA_SP_TRANS_METHOD_AIRUPDATE);
    }
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
    else if (RACE_SERIAL_PORT_TYPE_IAP2 == port_type)
    {
        race_fota_sp_trans_method_set(RACE_FOTA_SP_TRANS_METHOD_IAP2);
    }
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
    else if (RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR == port_type)
    {
        race_fota_sp_trans_method_set(RACE_FOTA_SP_TRANS_METHOD_GATT_OVER_BREDR);
    }
#endif
    else if (RACE_SERIAL_PORT_TYPE_USB == port_type)
    {
        race_fota_sp_trans_method_set(RACE_FOTA_SP_TRANS_METHOD_USB);
    }
    else
    {
        return RACE_ERRCODE_NOT_SUPPORT;
    }

    return RACE_ERRCODE_SUCCESS;
}


RACE_ERRCODE race_fota_channel_id_get(uint8_t *channel_id)
{
    race_fota_sp_trans_method_enum sp_trans_method = race_fota_sp_trans_method_get();

    if (!channel_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    *channel_id = 0xFF;

    if (RACE_FOTA_SP_TRANS_METHOD_SPP == sp_trans_method)
    {
        *channel_id = race_get_channel_id_by_port_type(RACE_SERIAL_PORT_TYPE_SPP);
        return RACE_ERRCODE_SUCCESS;
    }
    else if (RACE_FOTA_SP_TRANS_METHOD_BLE == sp_trans_method)
    {
        *channel_id = race_get_channel_id_by_port_type(RACE_SERIAL_PORT_TYPE_BLE);
        return RACE_ERRCODE_SUCCESS;
    }
#ifdef MTK_AIRUPDATE_ENABLE
    else if (RACE_FOTA_SP_TRANS_METHOD_AIRUPDATE == sp_trans_method)
    {
        *channel_id = race_get_channel_id_by_port_type(RACE_SERIAL_PORT_TYPE_AIRUPDATE);
        return RACE_ERRCODE_SUCCESS;
    }
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
    else if (RACE_FOTA_SP_TRANS_METHOD_IAP2 == sp_trans_method)
    {
        *channel_id = race_get_channel_id_by_port_type(RACE_SERIAL_PORT_TYPE_IAP2);
        return RACE_ERRCODE_SUCCESS;
    }
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
    else if (RACE_FOTA_SP_TRANS_METHOD_GATT_OVER_BREDR == sp_trans_method)
    {
        *channel_id = race_get_channel_id_by_port_type(RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR);
        return RACE_ERRCODE_SUCCESS;
    }
#endif
    else if (RACE_FOTA_SP_TRANS_METHOD_USB == sp_trans_method)
    {
        *channel_id = race_get_channel_id_by_port_type(RACE_SERIAL_PORT_TYPE_USB);
        return RACE_ERRCODE_SUCCESS;
    }


    RACE_LOG_MSGID_E("Cannot get FOTA channel id.", 0);
    return RACE_ERRCODE_FAIL;
}


bool race_fota_is_race_fota_running(void)
{
    if (RACE_FOTA_DOWNLOAD_STATE_NONE != race_fota_dl_state_get() &&
        RACE_FOTA_DOWNLOAD_STATE_MAX != race_fota_dl_state_get()
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
        && RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT != race_fota_dl_state_get()
#endif
        )
    {
        return TRUE;
    }

    return FALSE;
}


bool race_fota_is_running(bool is_race_fota)
{
    if (is_race_fota)
    {
        return race_fota_is_race_fota_running();
    }
    else
    {
#ifdef __FOTA_FOR_BISTO__
        return fota_is_bisto_fota_running();
#else
        return FALSE;
#endif
    }
}


bool race_fota_is_cmd_allowed(race_fota_stop_reason_enum *reason, uint16_t cmd_id, uint8_t channel_id)
{
    race_serial_port_type_enum port_type = race_get_port_type_by_channel_id(channel_id);
    race_fota_sp_trans_method_enum sp_trans_method = race_fota_sp_trans_method_get();
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();
    bool same_trans_method = FALSE;

    if (reason)
    {
        *reason = RACE_FOTA_STOP_REASON_MAX;
    }

    if ((RACE_FOTA_SP_TRANS_METHOD_SPP == sp_trans_method &&
         RACE_SERIAL_PORT_TYPE_SPP == port_type) ||
        (RACE_FOTA_SP_TRANS_METHOD_BLE == sp_trans_method &&
         RACE_SERIAL_PORT_TYPE_BLE == port_type)
#ifdef MTK_AIRUPDATE_ENABLE
         || (RACE_FOTA_SP_TRANS_METHOD_AIRUPDATE == sp_trans_method &&
             RACE_SERIAL_PORT_TYPE_AIRUPDATE == port_type)
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
         || (RACE_FOTA_SP_TRANS_METHOD_IAP2 == sp_trans_method &&
             RACE_SERIAL_PORT_TYPE_IAP2 == port_type)
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
         || (RACE_FOTA_SP_TRANS_METHOD_GATT_OVER_BREDR == sp_trans_method &&
             RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR == port_type)
#endif
         || (RACE_FOTA_SP_TRANS_METHOD_USB == sp_trans_method &&
             RACE_SERIAL_PORT_TYPE_USB == port_type)
         )
    {
        same_trans_method = TRUE;
    }

#ifdef __FOTA_FOR_BISTO__
    if (fota_is_bisto_fota_running())
    {
        goto end;
    }
#endif

#ifdef RACE_CFU_ENABLE
    if (cfu_is_running())
    {
        goto end;
    }
#endif

/* Do not allow FOTA when it's about to RHO because non-FOTA-RHO will cause FOTA stop
  * anyway.
  */
#ifdef RACE_LPCOMM_ENABLE
    if (race_lpcomm_get_role_switch_enable()
#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
        || BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state()
#endif
        )
    {
        if (RACE_FOTA_START == cmd_id
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
            || RACE_FOTA_COMMIT == cmd_id
            || RACE_FOTA_DUAL_DEVICES_COMMIT == cmd_id
#endif
            )
        {
            RACE_LOG_MSGID_W("Do not allow FOTA for it's about to RHO. cmd_id:%d",1, cmd_id);
            goto end;
        }
    }
#endif

    /* Enhance: If it's the single FOTA(fota_cntx->is_dual_fota==FALSE), do not allow dual cmds. */

/* COMMIT is to reboot the devicer for FOTA upgrade. If allowed, there's a chance that only
  * one device is rebooted.
  */
#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
    if (BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state())
    {
        if (RACE_FOTA_COMMIT == cmd_id ||
            RACE_FOTA_DUAL_DEVICES_COMMIT == cmd_id)
        {
            RACE_LOG_MSGID_W("Do not allow FOTA COMMIT for it's RHOING.", 0);
            goto end;
        }
    }
#endif

    switch (fota_dl_state)
    {
        case RACE_FOTA_DOWNLOAD_STATE_NONE:
        case RACE_FOTA_DOWNLOAD_STATE_MAX:
        {
            if (RACE_FOTA_START == cmd_id ||
                RACE_FOTA_STOP == cmd_id ||
                RACE_FOTA_COMMIT == cmd_id ||
                RACE_FOTA_DUAL_DEVICES_COMMIT == cmd_id)
            {
                return TRUE;
            }
            break;
        }

        case RACE_FOTA_DOWNLOAD_STATE_STARTING:
        {
            if ((RACE_FOTA_START == cmd_id ||
                 RACE_FOTA_STOP == cmd_id) &&
                 same_trans_method)
            {
                return TRUE;
            }
            break;
        }

        case RACE_FOTA_DOWNLOAD_STATE_START:
        {
            if (same_trans_method)
            {
                return TRUE;
            }

            break;
        }

        case RACE_FOTA_DOWNLOAD_STATE_RHOING:
        {
            if ((
#ifdef RACE_FOTA_STOP_DURING_RHO_SUPPORT
                 RACE_FOTA_STOP == cmd_id ||
#endif
                 RACE_BLUETOOTH_DUAL_ROLE_SWITCH == cmd_id) &&
                same_trans_method)
            {
                return TRUE;
            }

           break;
        }

#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
        case RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT:
        {
            if (RACE_FOTA_START == cmd_id ||
                RACE_FOTA_STOP == cmd_id ||
                RACE_FOTA_COMMIT == cmd_id ||
                RACE_FOTA_DUAL_DEVICES_COMMIT == cmd_id)
            {
                return TRUE;
            }

           if ((RACE_FOTA_QUERY_STATE == cmd_id ||
                RACE_FOTA_DUAL_DEVICES_QUERY_STATE == cmd_id) &&
               same_trans_method)
            {
                return TRUE;
            }

            if (RACE_BLUETOOTH_GET_LE_LINK_STATUS == cmd_id &&
                same_trans_method)
            {
                /* New start */
                if (g_race_fota_cntx_ptr)
                {
                    g_race_fota_cntx_ptr->old_dl_state = RACE_FOTA_DOWNLOAD_STATE_STARTING;
                    g_race_fota_cntx_ptr->dl_state = RACE_FOTA_DOWNLOAD_STATE_START;
                }
                return TRUE;
            }

           break;
        }
#endif

        case RACE_FOTA_DOWNLOAD_STATE_STOPPING:
        {
            race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

            if (RACE_FOTA_STOP == cmd_id && same_trans_method)
            {
                return TRUE;
            }

            if (fota_cntx && reason)
            {
                *reason = fota_cntx->reason;
                RACE_LOG_MSGID_I("stop reason:%x", 1, fota_cntx->reason);
            }

            goto end;
        }

        case RACE_FOTA_DOWNLOAD_STATE_COMMITING:
        {
            if ((RACE_FOTA_COMMIT == cmd_id ||
                 RACE_FOTA_DUAL_DEVICES_COMMIT == cmd_id) &&
                same_trans_method)
            {
                return TRUE;
            }

            break;
        }

        default:
            break;
    }

end:
    if (reason && (*reason == RACE_FOTA_STOP_REASON_MAX))
    {
        *reason = RACE_FOTA_STOP_REASON_NOT_ALLOWED;
    }
    return FALSE;
}


static RACE_ERRCODE race_fota_stop_send_req_to_sp(race_fota_stop_reason_enum reason,
                                                    uint8_t channel_id)
{
    typedef struct
    {
        uint8_t sender;
        uint8_t recipient;
        uint8_t reason;
    } PACKED CMD;

    CMD *pCmd = NULL;
    int32_t ret = RACE_ERRCODE_FAIL;

    RACE_LOG_MSGID_I("reason:%d channel_id:%d",2, reason, channel_id);

    pCmd = RACE_ClaimPacketAppID(RACE_APP_ID_FOTA,
                                 RACE_TYPE_COMMAND,
                                 RACE_FOTA_STOP,
                                 sizeof(CMD),
                                 channel_id);
    if (pCmd)
    {
       pCmd->sender = 0;
       pCmd->recipient = 0x80; /* SP */
       pCmd->reason = reason;

       ret = race_flush_packet((uint8_t *)pCmd, channel_id);
       ret = RACE_STATUS_OK == ret ? RACE_ERRCODE_SUCCESS: RACE_ERRCODE_FAIL;
    }

    return ret;
}


/* Pre-process the cmd with RACE_APP_ID_FOTA
  * 1. For the below cases, do not process the cmd received. If the cmd received
  *     is not 5B or 5D, send RACE_FOTA_STOP back also (or send 5B with status of failure).
  * 1). race_fota is not allowed
  * 2). race_fota is allowed and race_fota is not running and the cmd received is not FOTA_START cmd
  *     such as RACE_FOTA_START, RACE_FOTA_COMMIT
  * 2. Reset app_id timer for the cases below
  * 1). race_fota is running
  *
  * Start app_id timer within the FOTA_START cmd hdl.
  */
RACE_ERRCODE race_fota_cmd_preprocess(uint16_t cmd_id,
                                              uint8_t cmd_type,
                                              uint8_t channel_id)
{
    uint8_t app_id_timer_id = race_fota_app_id_timer_id_get();
    race_fota_stop_reason_enum reason;
#ifdef RACE_LPCOMM_ENABLE
    race_lpcomm_role_enum role = race_fota_get_role();
#ifdef RACE_LPCOMM_SENDER_ROLE_ENABLE
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();

    if (RACE_FOTA_DOWNLOAD_STATE_NONE != fota_dl_state &&
        RACE_FOTA_DOWNLOAD_STATE_MAX != fota_dl_state &&
#if (defined RACE_RELAY_CMD_ENABLE) || (defined AIR_RACE_CO_SYS_ENABLE)
        (((!RACE_CHANNEL_ID_IS_RELAY_CMD_FLAG_SET(channel_id)) && RACE_LPCOMM_ROLE_AGENT != role) ||
         (RACE_CHANNEL_ID_IS_RELAY_CMD_FLAG_SET(channel_id) && RACE_LPCOMM_ROLE_PARTNER != role)))
#else
        RACE_LPCOMM_ROLE_AGENT != role)
#endif
#else
    if (RACE_LPCOMM_ROLE_AGENT != role)
#endif
    {
        /* This may happen due to RHO triggered by others. */
        RACE_LOG_MSGID_I("Drop race cmd for wrong role. role:%d channel_id:%x",2, role, channel_id);
        return RACE_ERRCODE_WRONG_STATE;
    }
#endif

    if (!race_fota_is_cmd_allowed(&reason, cmd_id, channel_id))
    {
        if (RACE_TYPE_COMMAND == cmd_type ||
            RACE_TYPE_COMMAND_WITHOUT_RSP == cmd_type)
        {
#if defined(RACE_LPCOMM_ENABLE) && defined(RACE_LPCOMM_SENDER_ROLE_ENABLE)
            if (RACE_LPCOMM_ROLE_PARTNER == role)
            {
                race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_PARTNER, RACE_FOTA_STOP_REASON_NOT_ALLOWED);
            }
            else
#endif
            {
                /* Send FOTA_STOP or send 5B with status of failure. */
                race_fota_stop_send_req_to_sp(reason, channel_id);
            }
        }

        return RACE_ERRCODE_NOT_ALLOWED;
    }

    if (RACE_TIMER_INVALID_TIMER_ID != app_id_timer_id)
    {
        race_timer_smart_reset(app_id_timer_id);
    }

    return RACE_ERRCODE_SUCCESS;
}


/* Disconnect HFP. Pause A2DP. */
RACE_ERRCODE race_fota_active_bt_preparation_revert(void)
{
    /* Do nothing. FOTA app will revert the bt preparation on receiving CONNECTED/DISCONNECTED
         * event and FOTA cancel event. */
        return RACE_ERRCODE_SUCCESS;
    }


/* Disconnect HFP. Pause A2DP. */
RACE_ERRCODE race_fota_active_bt_preparation(void)
{
    /* Do nothing. FOTA app will prepare bt on receiving CONNECTED/DISCONNECTED event and
         * FOTA start event. */
        return RACE_ERRCODE_SUCCESS;
    }


bool race_fota_is_stop_allowed(bool *execute_stop)
{
    if (execute_stop)
    {
        *execute_stop = FALSE;
    }

    if (
#ifdef __FOTA_FOR_BISTO__
        fota_is_bisto_fota_running() ||
#endif
        RACE_FOTA_DOWNLOAD_STATE_COMMITING == race_fota_dl_state_get()
#ifndef RACE_FOTA_STOP_DURING_RHO_SUPPORT
        || RACE_FOTA_DOWNLOAD_STATE_RHOING == race_fota_dl_state_get()
#endif
        )
    {
        return FALSE;
    }

    if (!(RACE_FOTA_DOWNLOAD_STATE_NONE == race_fota_dl_state_get() ||
          RACE_FOTA_DOWNLOAD_STATE_MAX == race_fota_dl_state_get() ||
          RACE_FOTA_DOWNLOAD_STATE_STOPPING == race_fota_dl_state_get()))
    {
        if (execute_stop)
        {
            *execute_stop = TRUE;
        }
    }

    return TRUE;
}


void race_fota_stop_agent_reset(void)
{
    void *user_data = NULL;

    race_storage_disable_fota_partition_accessibility();

    /* Reset fota status */
    fota_multi_info_sector_reset();
    if (race_fota_is_active_mode())
    {
        race_fota_active_bt_preparation_revert();
    }

    /* Stop app_id timer */
    race_timer_smart_stop(race_fota_app_id_timer_id_get(), NULL);
    race_timer_smart_stop(race_fota_commit_timer_id_get(), &user_data);
    if (user_data)
    {
        race_mem_free(user_data);
        user_data = NULL;
    }

#ifdef RACE_LPCOMM_ENABLE
    /* Stop ping timer */
    race_timer_smart_stop(race_fota_ping_timer_id_get(), NULL);
    race_timer_smart_stop(race_fota_stop_timer_id_get(), NULL);
#endif

    /* Reset fota cntx. */
    race_fota_reset();
}


static RACE_ERRCODE race_fota_stop_agent_revert(void)
{
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

    if (!fota_cntx ||
        RACE_FOTA_DOWNLOAD_STATE_STOPPING != fota_dl_state)
    {
        return RACE_ERRCODE_WRONG_STATE;
    }

    race_fota_dl_state_rollback();
    fota_cntx->originator = RACE_FOTA_STOP_ORIGINATOR_NONE;
    fota_cntx->reason = RACE_FOTA_STOP_REASON_MAX;
    race_timer_smart_stop(race_fota_stop_timer_id_get(), NULL);

    return RACE_ERRCODE_SUCCESS;
}


RACE_ERRCODE race_fota_stop_send_noti_to_sp(uint8_t status,
                                                     uint8_t recipient,
                                                     uint8_t channel_id,
                                                     bool noti_delay)
{
    race_fota_stop_noti_struct *noti = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;

    noti = (void *)RACE_ClaimPacketAppID(RACE_APP_ID_FOTA,
                                         RACE_TYPE_NOTIFICATION,
                                         RACE_FOTA_STOP,
                                         sizeof(race_fota_stop_noti_struct),
                                         channel_id);
    if (noti)
    {
        /* A3. Set the noti parameters with the cmd results.  */
        noti->status = status;
        noti->sender = 7;
        noti->recipient = recipient;

        /* A4. Send the noti. */
        ret = race_noti_send(noti, channel_id, noti_delay);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            /* A5. Free the noti if needed. */
            RACE_FreePacket(noti);
            noti = NULL;
        }
    }
    else
    {
        ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
    }

    return ret;
}


RACE_ERRCODE race_fota_stop_agent_sp_stop_req_process(race_fota_stop_agent_sp_stop_req_struct *sp_stop_req)
{
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();

    if (!fota_cntx || !sp_stop_req)
    {
        return RACE_ERRCODE_WRONG_STATE;
    }

    if (!race_fota_is_race_fota_running())
    {
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
        if (RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT == fota_dl_state)
        {
            race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_NONE);
        }
#endif

        return race_fota_stop_send_noti_to_sp(RACE_ERRCODE_SUCCESS,
                                              sp_stop_req->recipient_type,
                                              sp_stop_req->channel_id,
                                              TRUE);
    }

    if (!race_fota_is_stop_allowed(NULL))
    {
        return RACE_ERRCODE_NOT_ALLOWED;
    }

    if (!((fota_cntx->is_dual_fota && RACE_RECIPIENT_TYPE_AGENT_PARTNER == sp_stop_req->recipient_type) ||
          (!fota_cntx->is_dual_fota && RACE_RECIPIENT_TYPE_AGENT_ONLY == sp_stop_req->recipient_type)))
    {
        return RACE_ERRCODE_NOT_SUPPORT;
    }

    if (RACE_FOTA_DOWNLOAD_STATE_STARTING == fota_dl_state)
    {
        RACE_LOG_MSGID_I("Cache FOTA_STOP during STARTING and execute it after START is done.", 0);
        /* Execute FOTA_STOP after FOTA_START is done. */
        fota_cntx->fota_stop_required = TRUE;
        return RACE_ERRCODE_SUCCESS;
    }
#ifdef RACE_FOTA_STOP_DURING_RHO_SUPPORT
    else if (RACE_FOTA_DOWNLOAD_STATE_RHOING == fota_dl_state)
    {
        fota_cntx->fota_stop_required = TRUE;
        return RACE_ERRCODE_SUCCESS;
    }
#endif

    if (RACE_FOTA_DOWNLOAD_STATE_STOPPING == fota_dl_state)
    {
        /* If multiple RACE_FOTA_STOP 5A is sent, only respond one 5D.
                 * If the originator is not SP, do not respond 5D.
                 * AGENT_ONLY will not reach here actually. Keep to extend for delaying CANCELLED event.
                 */
        return RACE_ERRCODE_SUCCESS;
    }

    if (RACE_RECIPIENT_TYPE_AGENT_ONLY == sp_stop_req->recipient_type)
    {
        /* Agent only */
        fota_cntx->originator = RACE_FOTA_STOP_ORIGINATOR_SP;
        fota_cntx->reason = sp_stop_req->reason;
        race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_STOPPING);
        race_event_send_fota_cancelling_event(fota_cntx->originator,
                                              fota_cntx->reason);
        race_fota_stop_send_noti_to_sp(RACE_ERRCODE_SUCCESS,
                                       sp_stop_req->recipient_type,
                                       sp_stop_req->channel_id,
                                       TRUE);
        /* Even if 5D is not sent successfully, still stop FOTA. */
        race_event_send_fota_cancel_event(TRUE,
                                          fota_cntx->originator,
                                          fota_cntx->reason);
        race_fota_stop_agent_reset();
        ret = RACE_ERRCODE_SUCCESS;
    }
#ifdef RACE_LPCOMM_ENABLE
    else if (RACE_RECIPIENT_TYPE_AGENT_PARTNER == sp_stop_req->recipient_type)
    {
        /* Agent and Partner */
        fota_cntx->originator = RACE_FOTA_STOP_ORIGINATOR_SP;
        fota_cntx->reason = sp_stop_req->reason;
        race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_STOPPING);
        race_event_send_fota_cancelling_event(fota_cntx->originator,
                                              fota_cntx->reason);

        if (fota_cntx->lpcomm_peer_online)
        {
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            race_lpcomm_fota_stop_query_req_struct req = {fota_cntx->originator,
                                                          fota_cntx->reason};
#endif
            uint16_t process_id = race_gen_process_id();

#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            ret = race_lpcomm_packet_send_to_peer((uint8_t *)&req,
                                                  sizeof(req),
                                                  RACE_LPCOMM_ROLE_AGENT,
                                                  RACE_LPCOMM_PACKET_TYPE_COMMON_REQ,
                                                  RACE_LPCOMM_COMMON_CMD_ID_FOTA_STOP_QUERY,
                                                  RACE_APP_ID_FOTA,
                                                  sp_stop_req->channel_id,
                                                  process_id,
                                                  RACE_LPCOMM_RETRY_MAX_TIME,
#ifdef RACE_AWS_ENABLE
                                                  RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                                  RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                                  RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif
        }

        if (!fota_cntx->lpcomm_peer_online ||
            RACE_ERRCODE_SUCCESS != ret)
        {
            race_fota_stop_agent_partner_result_rsp_struct result_rsp = {0};

            result_rsp.channel_id = sp_stop_req->channel_id;
            result_rsp.status = RACE_ERRCODE_SUCCESS;
            result_rsp.result = RACE_ERRCODE_SUCCESS;
            result_rsp.originator = fota_cntx->originator;
            result_rsp.reason = fota_cntx->reason;

            fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_RESULT_RSP;
            ret = race_fota_stop_agent_result_rsp_process(&result_rsp);
        }
        else
        {
            /* Wait for STOP_QUERY RSP */
            fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_QUERY_RSP;
        }
    }
#endif
    else
    {
        /* Never reach here actually. Keep to extend for PARTNER_ONLY maybe. */
        ret = RACE_ERRCODE_NOT_SUPPORT;
    }

    return ret;
}


static RACE_ERRCODE race_fota_stop_agent_active_stop_process(race_fota_stop_agent_active_stop_struct *active_stop)
{
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();
    race_fota_stop_agent_sp_stop_noti_struct sp_stop_noti = {0};

    if (!fota_cntx || !active_stop)
    {
        return RACE_ERRCODE_WRONG_STATE;
    }

    sp_stop_noti.recipient_type = active_stop->recipient_type;

    if (!race_fota_is_race_fota_running())
    {
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
        if (RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT == fota_dl_state)
        {
            race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_NONE);
        }
#endif
        return RACE_ERRCODE_SUCCESS;
    }

    if (!race_fota_is_stop_allowed(NULL))
    {
        return RACE_ERRCODE_NOT_ALLOWED;
    }

    if (RACE_FOTA_STOP_REASON_UNEXPECTED_RHO_DONE == active_stop->reason ||
        RACE_FOTA_STOP_REASON_UNEXPECTED_RHO_ONGOING == active_stop->reason)
    {
        /* FOTA stop may be interrupted by RHO triggered by non-FOTA modules. In such case,
         * FOTA procedure is interrupted and it's hard or impossible to recover the previous FOTA
         * stop procedure. Therefore, reset FOTA directly no matter if it's STOPPING or not. Also,
         * it's simple and reliable to reset FOTA directly when it's other state, such as STARTING.
         * SP retry mechanism will discover the failure.
         */
        if (RACE_FOTA_DOWNLOAD_STATE_STOPPING != fota_dl_state)
        {
            fota_cntx->originator = active_stop->originator;
            fota_cntx->reason = active_stop->reason;
            race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_STOPPING);
            race_event_send_fota_cancelling_event(fota_cntx->originator, fota_cntx->reason);
        }
        race_event_send_fota_cancel_event(TRUE,
                                          fota_cntx->originator,
                                          fota_cntx->reason);
        race_fota_stop_agent_reset();
        return RACE_ERRCODE_SUCCESS;
    }

    if (RACE_FOTA_STOP_REASON_SP_LOST == active_stop->reason ||
        RACE_FOTA_STOP_REASON_BT_OFF == active_stop->reason)
    {
        /* SPP/BLE DISC event may be received after RHO_RESULT event. In such case,
         * fota_cntx->sp_online has not been set FALSE by DISC event.
         */
        RACE_LOG_MSGID_W("Set sp_online:%d to FALSE actively for reason:%d",2,
                   fota_cntx->sp_online, active_stop->reason);
        fota_cntx->sp_online = FALSE;
    }

#ifdef RACE_LPCOMM_ENABLE
    if (RACE_FOTA_STOP_REASON_PARTNER_LOST == active_stop->reason ||
        RACE_FOTA_STOP_REASON_BT_OFF == active_stop->reason)
    {
        RACE_LOG_MSGID_W("Set lpcomm_peer_online:%d to FALSE actively for reason:%d",2,
                   fota_cntx->lpcomm_peer_online, active_stop->reason);
        fota_cntx->lpcomm_peer_online = FALSE;
    }
#endif

    if (RACE_FOTA_DOWNLOAD_STATE_STARTING == fota_dl_state)
    {
        RACE_LOG_MSGID_I("Cache FOTA_STOP during STARTING and execute it after START is done.", 0);
        /* Execute FOTA_STOP after FOTA_START is done. */
        fota_cntx->fota_stop_required = TRUE;
        return RACE_ERRCODE_SUCCESS;
    }
#ifdef RACE_FOTA_STOP_DURING_RHO_SUPPORT
    else if (RACE_FOTA_DOWNLOAD_STATE_RHOING == fota_dl_state)
    {
        fota_cntx->fota_stop_required = TRUE;
        return RACE_ERRCODE_SUCCESS;
    }
#endif

    if (RACE_FOTA_DOWNLOAD_STATE_STOPPING == fota_dl_state)
    {
        if ((RACE_FOTA_STOP_REASON_SP_LOST == active_stop->reason ||
             RACE_FOTA_STOP_REASON_BT_OFF == active_stop->reason) &&
            (RACE_FOTA_STOP_STATUS_WAIT_FOR_5B == fota_cntx->stop_status ||
             RACE_FOTA_STOP_STATUS_WAIT_FOR_5D == fota_cntx->stop_status))
        {
            sp_stop_noti.status = RACE_ERRCODE_SUCCESS;
            return race_fota_stop_agent_sp_stop_noti_process(&sp_stop_noti);
        }
#ifdef RACE_LPCOMM_ENABLE
        else if ((RACE_FOTA_STOP_REASON_PARTNER_LOST == active_stop->reason ||
                  RACE_FOTA_STOP_REASON_BT_OFF == active_stop->reason) &&
                 (RACE_FOTA_STOP_STATUS_WAIT_FOR_QUERY_RSP == fota_cntx->stop_status ||
                  RACE_FOTA_STOP_STATUS_WAIT_FOR_RESULT_RSP == fota_cntx->stop_status))
        {
            /* Do nothing for llpcomm retry mechanism will handle this. */
        }
#endif
        return RACE_ERRCODE_SUCCESS;
    }

    if (RACE_RECIPIENT_TYPE_AGENT_ONLY == active_stop->recipient_type)
    {
        uint8_t timer_id = race_fota_stop_timer_id_get();

        fota_cntx->originator = active_stop->originator;
        fota_cntx->reason = active_stop->reason;
        race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_STOPPING);
        race_event_send_fota_cancelling_event(fota_cntx->originator,
                                              fota_cntx->reason);

        /* Send RACE_FOTA_STOP REQ(5A) to SP */
        if (fota_cntx->sp_online)
        {
            ret = race_fota_stop_send_req_to_sp(fota_cntx->reason,
                                                active_stop->channel_id);
        }

        if (!fota_cntx->sp_online ||
            RACE_ERRCODE_SUCCESS != ret)
        {
            /* Failed to send 5A. Stop FOTA immediately. */
            sp_stop_noti.status = RACE_ERRCODE_SUCCESS;
            fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_5D;
            return race_fota_stop_agent_sp_stop_noti_process(&sp_stop_noti);
        }
        else
        {
            /* Start timer and wait for 5B/5D from SP. */
            if (RACE_TIMER_INVALID_TIMER_ID == timer_id)
            {
                ret = race_timer_smart_start(&timer_id,
                                             RACE_FOTA_AGENT_STOP_TIMEOUT_IN_MS,
                                             race_fota_stop_timer_expiration_hdl,
                                             NULL);
                if (RACE_ERRCODE_SUCCESS == ret)
                {
                    race_fota_stop_timer_id_set(timer_id);
                }
            }
            else
            {
                /* There must be something wrong. Stop FOTA immediately. */
                ret = RACE_ERRCODE_FAIL;
            }

            if (RACE_ERRCODE_SUCCESS == ret)
            {
                fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_5B;
            }
            else
            {
                /* Failed to start the stop timer or something wrong happened.
                               * Stop FOTA immediately for 5D may not be received. */
                sp_stop_noti.status = RACE_ERRCODE_SUCCESS;
                fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_5D;
                return race_fota_stop_agent_sp_stop_noti_process(&sp_stop_noti);
            }
        }
    }
#ifdef RACE_LPCOMM_ENABLE
    else if (RACE_RECIPIENT_TYPE_AGENT_PARTNER == active_stop->recipient_type)
    {
        fota_cntx->originator = active_stop->originator;
        fota_cntx->reason = active_stop->reason;
        race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_STOPPING);
        race_event_send_fota_cancelling_event(fota_cntx->originator,
                                              fota_cntx->reason);

        if (fota_cntx->lpcomm_peer_online)
        {
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            race_lpcomm_fota_stop_query_req_struct req = {fota_cntx->originator,
                                                          fota_cntx->reason};
#endif
            uint16_t process_id = race_gen_process_id();

#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            ret = race_lpcomm_packet_send_to_peer((uint8_t *)&req,
                                                  sizeof(req),
                                                  RACE_LPCOMM_ROLE_AGENT,
                                                  RACE_LPCOMM_PACKET_TYPE_COMMON_REQ,
                                                  RACE_LPCOMM_COMMON_CMD_ID_FOTA_STOP_QUERY,
                                                  RACE_APP_ID_FOTA,
                                                  active_stop->channel_id,
                                                  process_id,
                                                  RACE_LPCOMM_RETRY_MAX_TIME,
#ifdef RACE_AWS_ENABLE
                                                  RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                                  RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                                  RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif
        }

        if (!fota_cntx->lpcomm_peer_online ||
            RACE_ERRCODE_SUCCESS != ret)
        {
            race_fota_stop_agent_partner_query_rsp_struct query_rsp = {0};

            query_rsp.channel_id = active_stop->channel_id;
            query_rsp.status = RACE_ERRCODE_SUCCESS;
            query_rsp.originator = fota_cntx->originator;
            query_rsp.reason = fota_cntx->reason;

            fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_QUERY_RSP;
            ret = race_fota_stop_agent_query_rsp_process(&query_rsp);
        }
        else
        {
            /* Wait for STOP_QUERY RSP */
            fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_QUERY_RSP;
        }
    }
#endif
    else
    {
        ret = RACE_ERRCODE_NOT_SUPPORT;
    }

    return ret;
}


RACE_ERRCODE race_fota_stop_agent_sp_stop_noti_process(race_fota_stop_agent_sp_stop_noti_struct *sp_stop_noti)
{
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();

    if (!fota_cntx || !sp_stop_noti ||
        RACE_FOTA_DOWNLOAD_STATE_STOPPING != fota_dl_state ||
        (RACE_FOTA_STOP_STATUS_WAIT_FOR_5B != fota_cntx->stop_status &&
         RACE_FOTA_STOP_STATUS_WAIT_FOR_5D != fota_cntx->stop_status))
    {
        RACE_LOG_MSGID_W("fota_cntx:%x is_dual_fota:%d fota_dl_state:%d stop_status:%d",4,
                   fota_cntx,
                   fota_cntx ? fota_cntx->is_dual_fota : FALSE,
                   fota_dl_state,
                   fota_cntx ? fota_cntx->stop_status : RACE_FOTA_STOP_STATUS_MAX);
        return RACE_ERRCODE_WRONG_STATE;
    }

    if (RACE_FOTA_STOP_ORIGINATOR_AGENT == fota_cntx->originator
#ifdef RACE_FOTA_CANCEL_FROM_PARTNER_ENABLE
        || RACE_FOTA_STOP_ORIGINATOR_PARTNER == fota_cntx->originator
#endif
    )
    {
        if (RACE_RECIPIENT_TYPE_AGENT_ONLY == sp_stop_noti->recipient_type)
        {
            if (RACE_ERRCODE_SUCCESS == sp_stop_noti->status)
            {
                race_event_send_fota_cancel_event(TRUE,
                                                  RACE_FOTA_STOP_ORIGINATOR_AGENT,
                                                  RACE_FOTA_STOP_REASON_CANCEL);
                race_fota_stop_agent_reset();
            }
            else
            {
                race_event_send_fota_cancel_event(FALSE,
                                                  fota_cntx->originator,
                                                  fota_cntx->reason);
                race_fota_stop_agent_revert();
            }
        }
#ifdef RACE_LPCOMM_ENABLE
        else if (RACE_RECIPIENT_TYPE_AGENT_PARTNER == sp_stop_noti->recipient_type)
        {
            race_fota_stop_agent_partner_result_rsp_struct result_rsp = {0};

            race_fota_channel_id_get(&result_rsp.channel_id);
            result_rsp.originator = fota_cntx->originator;
            result_rsp.reason = fota_cntx->reason;

            if (fota_cntx->lpcomm_peer_online)
            {
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
                race_lpcomm_fota_stop_result_req_struct req = {0};
#endif
                uint16_t process_id = race_gen_process_id();

#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
                req.result = sp_stop_noti->status;
                req.originator = fota_cntx->originator;
                req.reason = fota_cntx->reason;

                ret = race_lpcomm_packet_send_to_peer((uint8_t *)&req,
                                                      sizeof(req),
                                                      RACE_LPCOMM_ROLE_AGENT,
                                                      RACE_LPCOMM_PACKET_TYPE_COMMON_REQ,
                                                      RACE_LPCOMM_COMMON_CMD_ID_FOTA_STOP_RESULT,
                                                      RACE_APP_ID_FOTA,
                                                      result_rsp.channel_id,
                                                      process_id,
                                                      RACE_LPCOMM_RETRY_MAX_TIME,
#ifdef RACE_AWS_ENABLE
                                                      RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                                      RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                                      RACE_LPCOMM_DEFAULT_DEVICE_ID);

#else
                ret = RACE_ERRCODE_NOT_SUPPORT;
#endif
            }

            if (!fota_cntx->lpcomm_peer_online ||
                RACE_ERRCODE_SUCCESS != ret)
            {
                result_rsp.status = RACE_ERRCODE_SUCCESS;
                result_rsp.result = sp_stop_noti->status;

                fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_RESULT_RSP;
                ret = race_fota_stop_agent_result_rsp_process(&result_rsp);
            }
            else
            {
                fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_RESULT_RSP;
            }
        }
#endif
        else
        {
            ret = RACE_ERRCODE_NOT_SUPPORT;
        }
    }
    else
    {
        ret = RACE_ERRCODE_WRONG_STATE;
    }

    return ret;
}


RACE_ERRCODE race_fota_stop_agent_sp_stop_rsp_process(race_fota_stop_agent_sp_stop_rsp_struct *sp_stop_rsp)
{
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();

    if (!fota_cntx || !sp_stop_rsp ||
        RACE_FOTA_DOWNLOAD_STATE_STOPPING != fota_dl_state ||
        RACE_FOTA_STOP_STATUS_WAIT_FOR_5B != fota_cntx->stop_status)
    {
        RACE_LOG_MSGID_W("fota_cntx:%x sp_stop_rsp:%x fota_dl_state:%d stop_status:%d",4,
                   fota_cntx,
                   sp_stop_rsp,
                   fota_dl_state,
                   fota_cntx ? fota_cntx->stop_status : RACE_FOTA_STOP_STATUS_MAX);
        return RACE_ERRCODE_WRONG_STATE;
    }

    if (RACE_FOTA_STOP_ORIGINATOR_AGENT == fota_cntx->originator)
    {
        if (RACE_RECIPIENT_TYPE_AGENT_ONLY == sp_stop_rsp->recipient_type
#ifdef RACE_LPCOMM_ENABLE
            || RACE_RECIPIENT_TYPE_AGENT_PARTNER == sp_stop_rsp->recipient_type
#endif
            )
        {
            if (RACE_ERRCODE_SUCCESS == sp_stop_rsp->status)
            {
                fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_5D;
            }
            else
            {
                race_fota_stop_agent_sp_stop_noti_struct sp_stop_noti = {0};

                sp_stop_noti.recipient_type = sp_stop_rsp->recipient_type;
                sp_stop_noti.status = sp_stop_rsp->status;

                fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_5D;
                return race_fota_stop_agent_sp_stop_noti_process(&sp_stop_noti);
            }
        }
        else
        {
            ret = RACE_ERRCODE_NOT_SUPPORT;
        }
    }
    else
    {
        ret = RACE_ERRCODE_WRONG_STATE;
    }

    return ret;
}


#ifdef RACE_LPCOMM_ENABLE
void race_fota_stop_partner_reset()
{
    void *user_data = NULL;

    race_storage_disable_fota_partition_accessibility();

    /* Reset fota status */
    fota_multi_info_sector_reset();
    if (race_fota_is_active_mode())
    {
        race_fota_active_bt_preparation_revert();
    }

    race_timer_smart_stop(race_fota_commit_timer_id_get(), &user_data);
    if (user_data)
    {
        race_mem_free(user_data);
        user_data = NULL;
    }

#ifdef RACE_LPCOMM_ENABLE
    /* Stop ping timer */
    race_timer_smart_stop(race_fota_ping_timer_id_get(), NULL);
    race_timer_smart_stop(race_fota_stop_timer_id_get(), NULL);
#endif

    /* Reset fota cntx. */
    race_fota_reset();
}


RACE_ERRCODE race_fota_stop_partner_revert(void)
{
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

    if (!fota_cntx ||
        RACE_FOTA_DOWNLOAD_STATE_STOPPING != fota_dl_state)
    {
        return RACE_ERRCODE_WRONG_STATE;
    }

    race_fota_dl_state_rollback();
    fota_cntx->originator = RACE_FOTA_STOP_ORIGINATOR_NONE;
    fota_cntx->reason = RACE_FOTA_STOP_REASON_MAX;
    race_timer_smart_stop(race_fota_stop_timer_id_get(), NULL);

    return RACE_ERRCODE_SUCCESS;
}


RACE_ERRCODE race_fota_stop_agent_result_rsp_process(race_fota_stop_agent_partner_result_rsp_struct *result_rsp)
{
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();

    if (!fota_cntx || !result_rsp ||
        RACE_FOTA_DOWNLOAD_STATE_STOPPING != fota_dl_state ||
        RACE_FOTA_STOP_STATUS_WAIT_FOR_RESULT_RSP != fota_cntx->stop_status)
    {
        RACE_LOG_MSGID_W("fota_cntx:%x result_rsp:%x fota_dl_state:%d stop_status:%d",4,
                   fota_cntx,
                   result_rsp,
                   fota_dl_state,
                   fota_cntx ? fota_cntx->stop_status : RACE_FOTA_STOP_STATUS_MAX);

        return RACE_ERRCODE_WRONG_STATE;
    }

    if (RACE_FOTA_STOP_ORIGINATOR_SP == fota_cntx->originator)
    {
        RACE_ERRCODE status = RACE_ERRCODE_FAIL;

        if (RACE_ERRCODE_SUCCESS == result_rsp->status &&
            RACE_ERRCODE_SUCCESS == result_rsp->result)
        {
           status = RACE_ERRCODE_SUCCESS;
        }

        race_fota_stop_send_noti_to_sp(status,
                                       RACE_CMD_RECIPIENT_AGENT | RACE_CMD_RECIPIENT_PARTNER,
                                       result_rsp->channel_id,
                                       TRUE);

        if (RACE_ERRCODE_SUCCESS == result_rsp->status &&
            RACE_ERRCODE_SUCCESS == result_rsp->result)
        {
            /* Even if 5D is not sent successfully, still stop FOTA. */
            race_event_send_fota_cancel_event(TRUE,
                                              fota_cntx->originator,
                                              fota_cntx->reason);
            race_fota_stop_agent_reset();
        }
        else
        {
            race_event_send_fota_cancel_event(FALSE,
                                              fota_cntx->originator,
                                              fota_cntx->reason);
            race_fota_stop_agent_revert();
        }

        return RACE_ERRCODE_SUCCESS;
    }
    else if (RACE_FOTA_STOP_ORIGINATOR_AGENT == fota_cntx->originator
#ifdef RACE_FOTA_CANCEL_FROM_PARTNER_ENABLE
             || RACE_FOTA_STOP_ORIGINATOR_PARTNER == fota_cntx->originator
#endif
        )
    {
        if (RACE_ERRCODE_SUCCESS == result_rsp->status &&
            RACE_ERRCODE_SUCCESS == result_rsp->result)
        {
            race_event_send_fota_cancel_event(TRUE,
                                              RACE_FOTA_STOP_ORIGINATOR_AGENT,
                                              RACE_FOTA_STOP_REASON_CANCEL);
            race_fota_stop_agent_reset();
        }
        else
        {
            race_event_send_fota_cancel_event(FALSE,
                                              fota_cntx->originator,
                                              fota_cntx->reason);
            race_fota_stop_agent_revert();
        }

        return RACE_ERRCODE_SUCCESS;
    }
    else
    {
        return RACE_ERRCODE_NOT_SUPPORT;
    }
}


RACE_ERRCODE race_fota_stop_agent_query_rsp_process(race_fota_stop_agent_partner_query_rsp_struct *query_rsp)
{
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();
    race_fota_stop_agent_partner_result_rsp_struct result_rsp = {0};

    if (!fota_cntx || !query_rsp ||
        RACE_FOTA_DOWNLOAD_STATE_STOPPING != fota_dl_state ||
        RACE_FOTA_STOP_STATUS_WAIT_FOR_QUERY_RSP != fota_cntx->stop_status)
    {
        RACE_LOG_MSGID_W("fota_cntx:%x query_rsp:%x fota_dl_state:%d stop_status:%d",4,
                   fota_cntx,
                   query_rsp,
                   fota_dl_state,
                   fota_cntx ? fota_cntx->stop_status : RACE_FOTA_STOP_STATUS_MAX);
        return RACE_ERRCODE_WRONG_STATE;
    }

    result_rsp.channel_id = query_rsp->channel_id;
    result_rsp.originator = fota_cntx->originator;
    result_rsp.reason = fota_cntx->reason;

    if (RACE_ERRCODE_SUCCESS != query_rsp->status)
    {
        result_rsp.status = query_rsp->status;
        result_rsp.result = RACE_ERRCODE_FAIL;
        fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_RESULT_RSP;
        return race_fota_stop_agent_result_rsp_process(&result_rsp);
    }

    if (RACE_FOTA_STOP_ORIGINATOR_SP == fota_cntx->originator)
    {
        if (fota_cntx->lpcomm_peer_online)
        {
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            race_lpcomm_fota_stop_result_req_struct req = {0};
#endif
            uint16_t process_id = race_gen_process_id();

#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            req.result = RACE_ERRCODE_SUCCESS;
            req.originator = fota_cntx->originator;
            req.reason = fota_cntx->reason;

            ret = race_lpcomm_packet_send_to_peer((uint8_t *)&req,
                                                  sizeof(req),
                                                  RACE_LPCOMM_ROLE_AGENT,
                                                  RACE_LPCOMM_PACKET_TYPE_COMMON_REQ,
                                                  RACE_LPCOMM_COMMON_CMD_ID_FOTA_STOP_RESULT,
                                                  RACE_APP_ID_FOTA,
                                                  query_rsp->channel_id,
                                                  process_id,
                                                  RACE_LPCOMM_RETRY_MAX_TIME,
#ifdef RACE_AWS_ENABLE
                                                  RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                                  RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                                  RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif
        }

        if (!fota_cntx->lpcomm_peer_online ||
            RACE_ERRCODE_SUCCESS != ret)
        {
            result_rsp.status = RACE_ERRCODE_SUCCESS;
            result_rsp.result = RACE_ERRCODE_SUCCESS;
            fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_RESULT_RSP;
            ret = race_fota_stop_agent_result_rsp_process(&result_rsp);
        }
        else
        {
            fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_RESULT_RSP;
        }
    }
    else if (RACE_FOTA_STOP_ORIGINATOR_AGENT == fota_cntx->originator
#ifdef RACE_FOTA_CANCEL_FROM_PARTNER_ENABLE
             || RACE_FOTA_STOP_ORIGINATOR_PARTNER == fota_cntx->originator
#endif
        )
    {
        race_fota_stop_agent_sp_stop_noti_struct sp_stop_noti = {0};

        sp_stop_noti.recipient_type = RACE_RECIPIENT_TYPE_AGENT_PARTNER;

        if (fota_cntx->sp_online)
        {
            ret = race_fota_stop_send_req_to_sp(query_rsp->reason,
                                                query_rsp->channel_id);
        }

        if (!fota_cntx->sp_online ||
            RACE_ERRCODE_SUCCESS != ret)
        {
            sp_stop_noti.status = RACE_ERRCODE_SUCCESS;
            fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_5D;
            ret = race_fota_stop_agent_sp_stop_noti_process(&sp_stop_noti);
        }
        else
        {
            uint8_t timer_id = race_fota_stop_timer_id_get();

            /* Start timer and wait for 5B/5D from SP. */
            if (RACE_TIMER_INVALID_TIMER_ID == timer_id)
            {
                ret = race_timer_smart_start(&timer_id,
                                             RACE_FOTA_AGENT_STOP_TIMEOUT_IN_MS,
                                             race_fota_stop_timer_expiration_hdl,
                                             NULL);
                if (RACE_ERRCODE_SUCCESS == ret)
                {
                    race_fota_stop_timer_id_set(timer_id);
                }
            }
            else
            {
                /* There must be something wrong. Stop FOTA immediately. */
                ret = RACE_ERRCODE_FAIL;
            }

            if (RACE_ERRCODE_SUCCESS == ret)
            {
                fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_5B;
            }
            else
            {
                /* Failed to start the stop timer or something wrong happened.
                               * Stop FOTA immediately for 5D may not be received. */
                sp_stop_noti.status = RACE_ERRCODE_SUCCESS;

                fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_5D;
                return race_fota_stop_agent_sp_stop_noti_process(&sp_stop_noti);
            }
        }
    }
    else
    {
        ret = RACE_ERRCODE_NOT_SUPPORT;
    }

    return ret;
}


RACE_ERRCODE race_fota_stop_agent_query_req_process(race_fota_stop_agent_partner_query_req_struct *query_req)
{
#ifdef RACE_FOTA_CANCEL_FROM_PARTNER_ENABLE
    race_lpcomm_fota_stop_query_rsp_struct rsp = {0};
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();

    if (!fota_cntx || !query_req)
    {
        return RACE_ERRCODE_WRONG_STATE;
    }

    rsp.originator = query_req->originator;
    rsp.reason = query_req->reason;

    if (!race_fota_is_race_fota_running())
    {
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
        if (RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT == fota_dl_state)
        {
            race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_NONE);
        }
#endif

        ret = RACE_ERRCODE_SUCCESS;
        goto end;
    }

    if (!race_fota_is_stop_allowed(NULL))
    {
        ret =  RACE_ERRCODE_NOT_ALLOWED;
        goto end;
    }

    if (RACE_FOTA_DOWNLOAD_STATE_STOPPING != fota_dl_state)
    {
        race_fota_stop_agent_partner_query_rsp_struct query_rsp = {0};

        /* Do not care if RSP is sent successfully or not, because if Partner does not
                * receive RSP, it will still stop FOTA.
                */
        fota_cntx->originator = query_req->originator;
        fota_cntx->reason = query_req->reason;
        race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_STOPPING);
        race_event_send_fota_cancelling_event(fota_cntx->originator,
                                              fota_cntx->reason);

        query_rsp.originator = query_req->originator;
        query_rsp.reason = query_req->reason;
        query_rsp.status = RACE_ERRCODE_SUCCESS;
        query_rsp.channel_id = query_req->channel_id;

        fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_QUERY_RSP;
        ret = race_fota_stop_agent_query_rsp_process(&query_rsp);
    }
    else
    {
        /* FOTA STOP procedure in the Agent side starts before receiving Query REQ from Partner.
                * Let the on-going FOTA STOP procedure stop FOTA instead of starting a new one.
                */
        ret = RACE_ERRCODE_SUCCESS;
    }

end:
    rsp.status = ret;

    /* Do not care if RSP is sent successfully or not, because if Partner does not
         * receive RSP, it will still stop FOTA.
         */
    race_lpcomm_packet_send_to_peer((uint8_t *)&rsp,
                                    sizeof(race_lpcomm_fota_stop_query_rsp_struct),
                                    RACE_LPCOMM_ROLE_AGENT,
                                    RACE_LPCOMM_PACKET_TYPE_COMMON_RSP,
                                    RACE_LPCOMM_COMMON_CMD_ID_FOTA_STOP_QUERY,
                                    RACE_APP_ID_FOTA,
                                    query_req->channel_id,
                                    query_req->process_id,
                                    0,
                                    query_req->trans_method,
                                    RACE_LPCOMM_DEFAULT_DEVICE_ID);

    return ret;
#else
    return RACE_ERRCODE_NOT_SUPPORT;
#endif
}


RACE_ERRCODE race_fota_stop_partner_result_req_process(race_fota_stop_partner_agent_result_req_struct *result_req)
{
    race_lpcomm_fota_stop_result_rsp_struct rsp = {0};
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();

    if (!fota_cntx || !result_req)
    {
        return RACE_ERRCODE_WRONG_STATE;
    }

    if (!race_fota_is_race_fota_running())
    {
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
        if (RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT == fota_dl_state)
        {
            race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_NONE);
        }
#endif
        goto end;
    }

    if (!race_fota_is_stop_allowed(NULL))
    {
        ret = RACE_ERRCODE_NOT_ALLOWED;
        goto end;
    }

    rsp.result = result_req->result;
    rsp.originator = result_req->originator;
    rsp.reason = result_req->reason;

    if (RACE_FOTA_DOWNLOAD_STATE_STOPPING == fota_dl_state)
    {
        if (RACE_ERRCODE_SUCCESS == result_req->result)
        {
            race_event_send_fota_cancel_event(TRUE,
                                              fota_cntx->originator,
                                              fota_cntx->reason);
            race_fota_stop_partner_reset();
        }
        else
        {
            race_event_send_fota_cancel_event(FALSE,
                                              fota_cntx->originator,
                                              fota_cntx->reason);
            race_fota_stop_partner_revert();
        }
    }

end:
    rsp.status = ret;
    if (result_req->process_id)
    {
        race_lpcomm_packet_send_to_peer((uint8_t *)&rsp,
                                        sizeof(race_lpcomm_fota_stop_result_rsp_struct),
                                        RACE_LPCOMM_ROLE_PARTNER,
                                        RACE_LPCOMM_PACKET_TYPE_COMMON_RSP,
                                        RACE_LPCOMM_COMMON_CMD_ID_FOTA_STOP_RESULT,
                                        RACE_APP_ID_FOTA,
                                        result_req->channel_id,
                                        result_req->process_id,
                                        0,
                                        result_req->trans_method,
                                        RACE_LPCOMM_DEFAULT_DEVICE_ID);
    }
    return ret;
}


RACE_ERRCODE race_fota_stop_partner_query_req_process(race_fota_stop_partner_agent_query_req_struct *query_req)
{
    race_lpcomm_fota_stop_query_rsp_struct rsp = {0};
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    bool end_fota_stop_procedure = FALSE;
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();
    uint8_t timer_id = race_fota_stop_timer_id_get();
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();

    if (!fota_cntx || !query_req)
    {
        return RACE_ERRCODE_WRONG_STATE;
    }

    rsp.originator = query_req->originator;
    rsp.reason = query_req->reason;

    if (!race_fota_is_race_fota_running())
    {
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
        if (RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT == fota_dl_state)
        {
            race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_NONE);
        }
#endif

        ret = RACE_ERRCODE_SUCCESS;
        goto end;
    }

    if (!race_fota_is_stop_allowed(NULL))
    {
        ret =  RACE_ERRCODE_NOT_ALLOWED;
        goto end;
    }

    if (RACE_FOTA_DOWNLOAD_STATE_STOPPING != fota_dl_state)
    {
        /* Do not care if RSP is sent successfully or not, because if Agent does not
                * receive RSP, it will still stop FOTA.
                */
        fota_cntx->originator = query_req->originator;
        fota_cntx->reason = query_req->reason;
        race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_STOPPING);
        race_event_send_fota_cancelling_event(fota_cntx->originator,
                                              fota_cntx->reason);
        /* Process the first FOTA_STOP_QUERY_REQ */
        if (RACE_TIMER_INVALID_TIMER_ID != timer_id)
        {
            /* No matter currently which stop procedure is executing for there must be something wrong. */
            RACE_LOG_MSGID_E("unexpected! when dl_state is %d, stop_timer has been started!",1, timer_id);
            ret = RACE_ERRCODE_SUCCESS;
            end_fota_stop_procedure = TRUE;
            goto end;
        }

        ret = race_timer_smart_start(&timer_id,
                                     RACE_FOTA_PARTNER_STOP_TIMEOUT_IN_MS,
                                     race_fota_stop_timer_expiration_hdl,
                                     NULL);
        if (RACE_ERRCODE_SUCCESS == ret)
        {
            race_fota_stop_timer_id_set(timer_id);
            /* Partner does not use stop_status actually. This is to support Agent sends RESULT_REQ directly. */
            fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_RESULT_REQ;
        }
        else
        {
            /* No matter currently which stop procedure is executing for there must be something wrong
                        * which causes the failure of starting the stop timer.
                        */
            RACE_LOG_MSGID_W("Failed to start fota stop timer! Partner stops FOTA immeidately.", 0);
            ret = RACE_ERRCODE_SUCCESS;
            end_fota_stop_procedure = TRUE;
            goto end;
        }
    }
    else
    {
        /* Process the retried FOTA_STOP_QUERY_REQ */
        if (RACE_TIMER_INVALID_TIMER_ID != timer_id)
        {
            ret = race_timer_smart_reset(timer_id);
            if (RACE_ERRCODE_SUCCESS != ret)
            {
                /* No matter currently which stop procedure is executing for there must be something wrong
                                * which causes the failure of resetting the stop timer.
                                */
                RACE_LOG_MSGID_W("Failed to reset fota stop timer! Partner stops FOTA immeidately.", 0);
                ret = RACE_ERRCODE_SUCCESS;
                end_fota_stop_procedure = TRUE;
                goto end;
            }
        }
        else
        {
#ifdef RACE_FOTA_CANCEL_FROM_PARTNER_ENABLE
            /* If Partner cancels FOTA before receiving the STOP Query REQ from Agent, the state may be STOPPING with
             * the timer not started (The timer is supposed to be started on receiving the STOP Query RSP). Therefore,
             * when RACE_FOTA_CANCEL_FROM_PARTNER_ENABLE is enabled, do nothing.
             */
            ret = RACE_ERRCODE_SUCCESS;
#else

            RACE_LOG_MSGID_E("unexpected! when dl_state is %d, stop_timer is not started!",1, timer_id);
            ret = RACE_ERRCODE_SUCCESS;
            end_fota_stop_procedure = TRUE;
            goto end;
#endif
        }
    }

end:
    rsp.status = ret;

    /* Do not care if RSP is sent successfully or not, because if Agent does not
         * receive RSP, it will still stop FOTA.
         */
    race_lpcomm_packet_send_to_peer((uint8_t *)&rsp,
                                    sizeof(race_lpcomm_fota_stop_query_rsp_struct),
                                    RACE_LPCOMM_ROLE_PARTNER,
                                    RACE_LPCOMM_PACKET_TYPE_COMMON_RSP,
                                    RACE_LPCOMM_COMMON_CMD_ID_FOTA_STOP_QUERY,
                                    RACE_APP_ID_FOTA,
                                    query_req->channel_id,
                                    query_req->process_id,
                                    0,
                                    query_req->trans_method,
                                    RACE_LPCOMM_DEFAULT_DEVICE_ID);

    if (end_fota_stop_procedure)
    {
        race_fota_stop_partner_agent_result_req_struct result_req = {0};

        result_req.result = RACE_ERRCODE_SUCCESS;
        result_req.originator = query_req->originator;
        result_req.reason = query_req->reason;
        /* No need to send result_rsp to the Agent. */
        result_req.channel_id = 0;
        result_req.process_id = 0;
        result_req.trans_method = RACE_LPCOMM_TRANS_METHOD_NONE;

        ret = race_fota_stop_partner_result_req_process(&result_req);
    }

    return ret;
}


RACE_ERRCODE race_fota_stop_partner_query_rsp_process(race_fota_stop_partner_agent_query_rsp_struct *query_rsp)
{
#ifdef RACE_FOTA_CANCEL_FROM_PARTNER_ENABLE
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();
    race_fota_stop_partner_agent_result_req_struct result_req = {0};
    uint8_t timer_id = race_fota_stop_timer_id_get();

    if (!fota_cntx || !query_rsp ||
        RACE_FOTA_DOWNLOAD_STATE_STOPPING != fota_dl_state ||
        RACE_FOTA_STOP_STATUS_WAIT_FOR_QUERY_RSP != fota_cntx->stop_status)
    {
        RACE_LOG_MSGID_W("fota_cntx:%x query_rsp:%x fota_dl_state:%d stop_status:%d",4,
                         fota_cntx,
                         query_rsp,
                         fota_dl_state,
                         fota_cntx ? fota_cntx->stop_status : RACE_FOTA_STOP_STATUS_MAX);
        return RACE_ERRCODE_WRONG_STATE;
    }

    result_req.channel_id = query_rsp->channel_id;
    /* No need to send Result RSP to Agent. */
    result_req.process_id = 0;
    result_req.trans_method = RACE_LPCOMM_TRANS_METHOD_NONE;
    result_req.originator = fota_cntx->originator;
    result_req.reason = fota_cntx->reason;

    if (RACE_ERRCODE_SUCCESS != query_rsp->status)
    {
        result_req.result = RACE_ERRCODE_FAIL;
        return race_fota_stop_partner_result_req_process(&result_req);
    }

    if (RACE_TIMER_INVALID_TIMER_ID == timer_id)
    {
        ret = race_timer_smart_start(&timer_id,
                                     RACE_FOTA_PARTNER_STOP_TIMEOUT_IN_MS,
                                     race_fota_stop_timer_expiration_hdl,
                                     NULL);
        if (RACE_ERRCODE_SUCCESS == ret)
        {
            race_fota_stop_timer_id_set(timer_id);
            /* Partner does not use stop_status actually. This is to support Agent sends RESULT_REQ directly. */
            fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_RESULT_REQ;
        }
        else
        {
            /* No matter currently which stop procedure is executing for there must be something wrong
                        * which causes the failure of starting the stop timer.
                        */
            RACE_LOG_MSGID_W("Failed to start fota stop timer! Partner stops FOTA immeidately.", 0);
            result_req.result = RACE_ERRCODE_SUCCESS;
            return race_fota_stop_partner_result_req_process(&result_req);
        }
    }

    return ret;
#else
    return RACE_ERRCODE_NOT_SUPPORT;
#endif
}


void race_fota_stop_partner_quick_stop(race_fota_cntx_struct *fota_cntx,
                                                race_fota_download_state_enum fota_dl_state,
                                                race_fota_stop_originator_enum originator,
                                                race_fota_stop_reason_enum reason)
{
    if (RACE_FOTA_DOWNLOAD_STATE_STOPPING != fota_dl_state)
    {
        fota_cntx->originator = originator;
        fota_cntx->reason = reason;
        race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_STOPPING);
        race_event_send_fota_cancelling_event(fota_cntx->originator, fota_cntx->reason);
    }

    race_event_send_fota_cancel_event(TRUE,
                                      fota_cntx->originator,
                                      fota_cntx->reason);
    race_fota_stop_partner_reset();
}


static RACE_ERRCODE race_fota_stop_partner_active_stop_process(race_fota_stop_partner_active_stop_struct *active_stop)
{
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();
#ifdef RACE_FOTA_CANCEL_FROM_PARTNER_ENABLE
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
#endif

    if (!fota_cntx || !active_stop)
    {
        return RACE_ERRCODE_WRONG_STATE;
    }

    if (!race_fota_is_race_fota_running())
    {
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
        if (RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT == fota_dl_state)
        {
            race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_NONE);
        }
#endif

        return RACE_ERRCODE_SUCCESS;
    }

    if (!race_fota_is_stop_allowed(NULL))
    {
        return RACE_ERRCODE_NOT_ALLOWED;
    }

    if (RACE_FOTA_STOP_REASON_UNEXPECTED_RHO_DONE == active_stop->reason ||
        RACE_FOTA_STOP_REASON_UNEXPECTED_RHO_ONGOING == active_stop->reason)
    {
        /* FOTA stop may be interrupted by RHO triggered by non-FOTA modules. In such case,
         * FOTA procedure is interrupted and it's hard or impossible to recover the previous FOTA
         * stop procedure. Therefore, reset FOTA directly no matter if it's STOPPING or not. Also,
         * it's simple and reliable to reset FOTA directly when it's other state, such as STARTING.
         * Agent will also receive this failure reason and reset FOTA directly.
         */

        race_fota_stop_partner_quick_stop(fota_cntx,
                                          fota_dl_state,
                                          active_stop->originator,
                                          active_stop->reason);
        return RACE_ERRCODE_SUCCESS;
    }

    if (RACE_FOTA_STOP_REASON_AGENT_LOST == active_stop->reason ||
        RACE_FOTA_STOP_REASON_BT_OFF == active_stop->reason)
    {
        RACE_LOG_MSGID_W("Set lpcomm_peer_online:%d to FALSE actively for reason:%d",2,
                   fota_cntx->lpcomm_peer_online, active_stop->reason);
        fota_cntx->lpcomm_peer_online = FALSE;
    }

    if (RACE_FOTA_DOWNLOAD_STATE_STARTING == fota_dl_state)
    {
        RACE_LOG_MSGID_I("Cache FOTA_STOP during STARTING and execute it after START is done.", 0);
        /* Would not happen actually for there's no possible to stop FOTA during partner's STARTING state.
                * Add here to handle stop FOTA during STARTING state in case partner changes the start flow.
                */
        /* Execute FOTA_STOP after FOTA_START is done. */
        fota_cntx->fota_stop_required = TRUE;
        return RACE_ERRCODE_SUCCESS;
    }
#ifdef RACE_FOTA_STOP_DURING_RHO_SUPPORT
    else if (RACE_FOTA_DOWNLOAD_STATE_RHOING == fota_dl_state)
    {
        fota_cntx->fota_stop_required = TRUE;
        return RACE_ERRCODE_SUCCESS;
    }
#endif

#ifdef RACE_FOTA_CANCEL_FROM_PARTNER_ENABLE
    if (RACE_FOTA_DOWNLOAD_STATE_STOPPING == fota_dl_state)
    {
#ifdef RACE_LPCOMM_ENABLE
        if ((RACE_FOTA_STOP_REASON_AGENT_LOST == active_stop->reason ||
             RACE_FOTA_STOP_REASON_BT_OFF == active_stop->reason) &&
            (RACE_FOTA_STOP_STATUS_WAIT_FOR_RESULT_REQ == fota_cntx->stop_status))
        {
            race_fota_stop_partner_agent_result_req_struct result_req = {0};

            /* No need to send result_rsp to Agent. */
            result_req.channel_id = 0;
            result_req.process_id = 0;
            result_req.trans_method = RACE_LPCOMM_TRANS_METHOD_NONE;
            result_req.originator = fota_cntx->originator;
            result_req.reason = fota_cntx->reason;
            result_req.result = RACE_ERRCODE_SUCCESS;

            return race_fota_stop_partner_result_req_process(&result_req);
        }
        else if ((RACE_FOTA_STOP_REASON_AGENT_LOST == active_stop->reason ||
                  RACE_FOTA_STOP_REASON_BT_OFF == active_stop->reason) &&
                 (RACE_FOTA_STOP_STATUS_WAIT_FOR_QUERY_RSP == fota_cntx->stop_status))
        {
            /* Do nothing for lpcomm retry mechanism will handle this. */
        }
#endif
        return RACE_ERRCODE_SUCCESS;
    }

    fota_cntx->originator = active_stop->originator;
    fota_cntx->reason = active_stop->reason;
    race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_STOPPING);
    race_event_send_fota_cancelling_event(fota_cntx->originator,
                                          fota_cntx->reason);

    if (fota_cntx->lpcomm_peer_online)
    {
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
        race_lpcomm_fota_stop_query_req_struct req = {fota_cntx->originator,
                                                      fota_cntx->reason};
#endif
        uint16_t process_id = race_gen_process_id();

#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
        ret = race_lpcomm_packet_send_to_peer((uint8_t *)&req,
                                              sizeof(req),
                                              RACE_LPCOMM_ROLE_PARTNER,
                                              RACE_LPCOMM_PACKET_TYPE_COMMON_REQ,
                                              RACE_LPCOMM_COMMON_CMD_ID_FOTA_STOP_QUERY,
                                              RACE_APP_ID_FOTA,
                                              active_stop->channel_id,
                                              process_id,
                                              RACE_LPCOMM_RETRY_MAX_TIME,
#ifdef RACE_AWS_ENABLE
                                              RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                              RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                              RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
        ret = RACE_ERRCODE_NOT_SUPPORT;
#endif
    }

    if (!fota_cntx->lpcomm_peer_online ||
        RACE_ERRCODE_SUCCESS != ret)
    {
        race_fota_stop_partner_agent_result_req_struct result_req = {0};

        /* No need to send result_rsp to Agent. */
        result_req.channel_id = 0;
        result_req.process_id = 0;
        result_req.trans_method = RACE_LPCOMM_TRANS_METHOD_NONE;
        result_req.originator = fota_cntx->originator;
        result_req.reason = fota_cntx->reason;
        result_req.result = RACE_ERRCODE_SUCCESS;

        return race_fota_stop_partner_result_req_process(&result_req);
    }
    else
    {
        /* Wait for STOP_QUERY RSP */
        fota_cntx->stop_status = RACE_FOTA_STOP_STATUS_WAIT_FOR_QUERY_RSP;
    }

#else
    if (RACE_FOTA_DOWNLOAD_STATE_STOPPING == fota_dl_state)
    {
        /* Let the current FOTA_STOP procedure end FOTA Download. */
        /* Partner FOTA stop does not involve Agent or Sp. This must be Agent FOTA stop
         * being broken by RHO.
         */
        RACE_LOG_MSGID_W("Partner stop is triggered at STOPPING state ", 0);
    }

    /* FOTA_STOP triggered by Partner is not supported. So reset partner directly. */
    race_event_send_fota_cancel_event(TRUE,
                                      active_stop->originator,
                                      active_stop->reason);
    race_fota_stop_partner_reset();
#endif

    return RACE_ERRCODE_SUCCESS;
}
#endif /* #ifdef RACE_LPCOMM_ENABLE */


/* Send FOTA_STOP MSG to race task. */
RACE_ERRCODE race_fota_stop(race_fota_stop_originator_enum originator,
                                race_fota_stop_reason_enum reason)
{
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();
#endif
    race_general_msg_t msg_queue_item = {0};
    race_fota_stop_msg_data_struct *msg_data = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;


    RACE_LOG_MSGID_I("race_fota_stop originator:%d reason:%d",2, originator, reason);

    if (!g_race_fota_cntx_ptr)
    {
        return RACE_ERRCODE_SUCCESS;
    }

#ifndef RACE_FOTA_CANCEL_FROM_PARTNER_ENABLE
    if (RACE_FOTA_STOP_ORIGINATOR_PARTNER == originator)
    {
        return RACE_ERRCODE_NOT_SUPPORT;
    }
#endif

    if (!race_fota_is_race_fota_running())
    {
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
        if (RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT == fota_dl_state)
        {
            // TODO: Need mutex for it's running in multiple tasks.
            race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_NONE);
        }
#endif

        return RACE_ERRCODE_SUCCESS;
    }

    if (!race_fota_is_stop_allowed(NULL))
    {
        return RACE_ERRCODE_NOT_ALLOWED;
    }

    /* Send FOTA_STOP MSG to race cmd task. */
    msg_data = race_mem_alloc(sizeof(race_fota_stop_msg_data_struct));
    if (!msg_data)
    {
        return RACE_ERRCODE_NOT_ENOUGH_MEMORY;
    }

    msg_data->originator = originator;
    msg_data->reason = reason;
    msg_data->recipient_type = g_race_fota_cntx_ptr->is_dual_fota ? RACE_RECIPIENT_TYPE_AGENT_PARTNER : RACE_RECIPIENT_TYPE_AGENT_ONLY;
#ifdef RACE_FOTA_CANCEL_FROM_PARTNER_ENABLE
    if (RACE_FOTA_STOP_ORIGINATOR_PARTNER == originator)
    {
        msg_data->recipient_type = RACE_RECIPIENT_TYPE_AGENT_PARTNER;
    }
#endif

    msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_FOTA_STOP_IND;
    msg_queue_item.msg_data = (uint8_t*)msg_data;
    ret = race_send_msg(&msg_queue_item);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        RACE_LOG_MSGID_W("Failed to send msg:%d. ret:%d",2, MSG_ID_RACE_LOCAL_FOTA_STOP_IND, ret);
        race_mem_free((void *)msg_data);
    }

    return ret;
}


RACE_ERRCODE race_fota_stop_msg_process(race_general_msg_t *msg)
{
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;
    race_fota_stop_msg_data_struct *msg_data = NULL;
#ifdef RACE_LPCOMM_ENABLE
    race_lpcomm_role_enum role = race_fota_get_role();
#endif
    uint8_t channel_id = 0;

    if (!msg || !msg->msg_data || !g_race_fota_cntx_ptr
#ifdef RACE_LPCOMM_ENABLE
        || !(RACE_LPCOMM_ROLE_AGENT == role ||
             RACE_LPCOMM_ROLE_PARTNER == role)
#endif
          )
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    msg_data = (race_fota_stop_msg_data_struct *)msg->msg_data;

    RACE_LOG_MSGID_I("stop_status:%d curr_originator:%d curr_reason:%x",3,
               g_race_fota_cntx_ptr->stop_status,
               g_race_fota_cntx_ptr->originator,
               g_race_fota_cntx_ptr->reason);

    ret = race_fota_channel_id_get(&channel_id);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        return ret;
    }

#ifdef RACE_LPCOMM_ENABLE
    if (RACE_LPCOMM_ROLE_PARTNER == role)
    {
        /* Do not care if msg_data->originator is PARTNER or not. FOTA stop is only marked
                * during RHO and will be executed after RHO. If FOTA is stopped during RHO with
                * the originator of Agent by then. After RHO is done succesfully, the role is changed
                * to PARTNER. originator and role will not match. So ignore originator here.
                */
        race_fota_stop_partner_active_stop_struct partner_active_stop = {0};

        partner_active_stop.channel_id = channel_id;
        partner_active_stop.originator = msg_data->originator;
        partner_active_stop.reason = msg_data->reason;
        partner_active_stop.recipient_type = msg_data->recipient_type;

        return race_fota_stop_partner_active_stop_process(&partner_active_stop);
    }
    else
#endif
    {
        /* Do not care if msg_data->originator is Agent or not. FOTA stop is only marked
                * during RHO and will be executed after RHO. If FOTA is stopped during RHO with
                * the originator of Partner by then. After RHO is done succesfully, the role is changed
                * to Agent. originator and role will not match. So ignore originator here.
                */
        race_fota_stop_agent_active_stop_struct agent_active_stop = {0};

        agent_active_stop.channel_id = channel_id;
        agent_active_stop.originator = msg_data->originator;
        agent_active_stop.reason = msg_data->reason;
        agent_active_stop.recipient_type = msg_data->recipient_type;

        return race_fota_stop_agent_active_stop_process(&agent_active_stop);
    }

    return ret;
}


RACE_ERRCODE race_fota_cancel(void)
{
#ifdef RACE_LPCOMM_ENABLE
    race_lpcomm_role_enum role = race_fota_get_role();

    /* Cancel from the partner is not allowed. */
    if (RACE_LPCOMM_ROLE_AGENT != role)
    {
        return race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_PARTNER,
                              RACE_FOTA_STOP_REASON_CANCEL);
    }
    else
#endif
    {
        return race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_AGENT,
                              RACE_FOTA_STOP_REASON_CANCEL);
    }
}


RACE_ERRCODE race_fota_integrity_check(uint32_t signature_start_address,
                                             uint8_t storage_type,
                                             uint8_t *signature_generated,
                                             uint8_t signature_generated_size)
{
    unsigned char signature_in_fota_partition[RACE_STORAGE_SIGNATURE_MAX_SIZE] = {0};
    int32_t ret = RACE_ERRCODE_FAIL;

    if (!signature_generated || !signature_generated_size ||
        RACE_STORAGE_SIGNATURE_MAX_SIZE < signature_generated_size)
    {
        RACE_LOG_MSGID_E("signature_generated:%x signature_generated_size:%d",2, signature_generated, signature_generated_size);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    ret = fota_flash_read(signature_start_address,
                          signature_in_fota_partition,
                          signature_generated_size,
                          InternalFlash == storage_type);
    if (FOTA_ERRCODE_SUCCESS == ret)
    {
        if (strncmp((const char *)signature_in_fota_partition, (const char *)signature_generated, signature_generated_size) != 0)
        {
            ret = FOTA_ERRCODE_CHECK_INTEGRITY_FAIL;
        }
        else
        {
            ret = fota_dl_integrity_res_write(FOTA_DL_INTEGRITY_RES_VAL_PASS);
        }
    }

    ret = FOTA_ERRCODE_SUCCESS == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_CHECK_INTEGRITY_FAIL;
    return ret;
}


#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
/* allowed_callback is invoked after RHO_START event. */
bt_status_t race_fota_rho_srv_allowed_callback(const bt_bd_addr_t *addr)
{
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();

    if (!race_fota_is_race_fota_running())
    {
        return BT_STATUS_SUCCESS;
    }

    if (RACE_FOTA_DOWNLOAD_STATE_COMMITING == fota_dl_state)
    {
        /* If allow RHO when committing, there's a chance that only one device reboots. */
        RACE_LOG_MSGID_W("Do not allow RHO. It's FOTA COMMITING. The device is about to reboot", 0);
        return BT_STATUS_FAIL;
    }
    else if (race_lpcomm_get_role_switch_enable() &&
             RACE_FOTA_DOWNLOAD_STATE_RHOING == fota_dl_state)
    {
        RACE_LOG_MSGID_I("Allow RHO. It's FOTA RHO.", 0);
    }
    else
    {
        RACE_LOG_MSGID_I("Allow RHO. It's not FOTA RHO.", 0);
    }

    /* There're still things to do. */
    return BT_STATUS_PENDING;
}


bt_status_t race_fota_rho_srv_get_data_callback(const bt_bd_addr_t *addr, void * data)
{
#ifdef AIR_MULTI_POINT_ENABLE
    if (NULL == addr)
#endif
    {
        race_fota_rho_change_data_struct rho_change_data = {0};
        race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

        if (data && fota_cntx && race_fota_is_race_fota_running())
        {
            rho_change_data.fota_dl_state = race_fota_dl_state_get();
            memcpy(data, &rho_change_data, sizeof(race_fota_rho_change_data_struct));
        }
    }

    return BT_STATUS_SUCCESS;
}


uint8_t race_fota_rho_srv_get_length_callback(const bt_bd_addr_t *addr)
{
#ifdef AIR_MULTI_POINT_ENABLE
    if (NULL == addr)
#endif
    {
        race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

        if (fota_cntx && race_fota_is_race_fota_running())
        {
            return sizeof(race_fota_rho_change_data_struct);
        }
    }

    return 0;
}


bt_status_t race_fota_rho_srv_update_callback(bt_role_handover_update_info_t *info)
{
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

    if (fota_cntx && race_fota_is_race_fota_running())
    {
        if (info && BT_AWS_MCE_ROLE_PARTNER == info->role && info->data && info->length &&
            sizeof(race_fota_rho_change_data_struct) <= info->length)
        {
            race_fota_rho_change_data_struct *rho_change_data = (race_fota_rho_change_data_struct *)info->data;
            race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();

            RACE_LOG_MSGID_I("Agent fota_dl_state:%d Partner fota_dl_state:%d",2, rho_change_data->fota_dl_state, fota_dl_state);
            if (RACE_FOTA_DOWNLOAD_STATE_RHOING == fota_dl_state &&
                RACE_FOTA_DOWNLOAD_STATE_RHOING != rho_change_data->fota_dl_state)
            {
                /* This API is called in BT task. Mutex protection may be needed. */
                race_fota_dl_state_rollback();
            }
        }
    }

    return BT_STATUS_SUCCESS;
}

void race_fota_rho_srv_status_callback(const bt_bd_addr_t *addr,
                                                bt_aws_mce_role_t role,
                                                bt_role_handover_event_t event,
                                                bt_status_t status)
{
    /* Do nothing for FOTA listens to race RHO event. This is fine most of the time. However, when partner
         * receives RACE_EVENT_TYPE_BT_RHO_START, RHO may have already been done. If Partner need do
         * something before RHO, it need do it here when receiving BT_ROLE_HANDOVER_START_IND.
         * But all race APIs now do not support multiple tasks and mutex protection need be added.
         * Add mutexes may lead to the dead lock. It must be considered carefully.
         */
}
#endif


#ifdef MTK_RACE_EVENT_ID_ENABLE
static RACE_ERRCODE race_fota_race_event_cb(int32_t register_id, race_event_type_enum event_type, void *param, void *user_data)
#else
static RACE_ERRCODE race_fota_race_event_cb(race_event_type_enum event_type, void *param, void *user_data)
#endif
{
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();
    race_fota_sp_trans_method_enum sp_trans_method = race_fota_sp_trans_method_get();

#ifdef MTK_RACE_EVENT_ID_ENABLE
    RACE_LOG_MSGID_I("register_id:%d event_type:%d param:%x user_data:%x",4, register_id, event_type, param, user_data);

    if (!fota_cntx || register_id != g_race_fota_event_register_id)
    {
        RACE_LOG_MSGID_E("register_id does not match! register_id:%d, g_register_id:%d",2,
                   register_id,
                   g_race_fota_event_register_id);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }
#else
    RACE_LOG_MSGID_I("event_type:%d param:%x user_data:%x",3, event_type, param, user_data);

    if (!fota_cntx)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }
#endif

    switch (event_type)
    {
        case RACE_EVENT_TYPE_CONN_BLE_DISCONNECT:
        case RACE_EVENT_TYPE_CONN_SPP_DISCONNECT:
#ifdef MTK_AIRUPDATE_ENABLE
        case RACE_EVENT_TYPE_CONN_AIRUPDATE_DISCONNECT:
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
        case RACE_EVENT_TYPE_CONN_IAP2_DISCONNECT:
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
        case RACE_EVENT_TYPE_CONN_GATT_OVER_BREDR_DISCONNECT:
#endif
        case RACE_EVENT_TYPE_CONN_USB_DISCONNECT:
        {
            /* DISC event caused by RHO may be received when RHO has already been done for BT task's
             * priority is higher than race task's. If RHO fails, Agent receives it and do nothing and let FOTA
             * err be handled within RHO_RESULT event behind it. If RHO succeeds, Partner receives it and
             * nothing need be done.
             */
#if defined (RACE_LPCOMM_ENABLE)
            if (RACE_LPCOMM_ROLE_AGENT != race_fota_get_role())
            {
                /* RHO may be done before DISC event is processed by FOTA. Do nothing for
                 * FOTA will be stopped on receiving RHO_RESULT event.
                 */
                RACE_LOG_MSGID_W("Partner received DISC event.", 0);
            }
            else
#endif
            {
                race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();

                RACE_LOG_MSGID_I("fota_cntx:%x dl_state:%d trans_method:%d",3,
                           fota_cntx,
                           fota_dl_state,
                           sp_trans_method);
                if (fota_cntx && race_fota_is_race_fota_running())
                {
                    if ((RACE_FOTA_SP_TRANS_METHOD_SPP == sp_trans_method &&
                         RACE_EVENT_TYPE_CONN_SPP_DISCONNECT == event_type) ||
                        (RACE_FOTA_SP_TRANS_METHOD_BLE == sp_trans_method &&
                         RACE_EVENT_TYPE_CONN_BLE_DISCONNECT == event_type)
#ifdef MTK_AIRUPDATE_ENABLE
                        || (RACE_FOTA_SP_TRANS_METHOD_AIRUPDATE == sp_trans_method &&
                            RACE_EVENT_TYPE_CONN_AIRUPDATE_DISCONNECT == event_type)
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
                        || (RACE_FOTA_SP_TRANS_METHOD_IAP2 == sp_trans_method &&
                            RACE_EVENT_TYPE_CONN_IAP2_DISCONNECT == event_type)
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
                        || (RACE_FOTA_SP_TRANS_METHOD_GATT_OVER_BREDR == sp_trans_method &&
                            RACE_EVENT_TYPE_CONN_GATT_OVER_BREDR_DISCONNECT == event_type)
#endif
                        || (RACE_FOTA_SP_TRANS_METHOD_USB == sp_trans_method &&
                            RACE_EVENT_TYPE_CONN_USB_DISCONNECT == event_type)
                        )
                    {
                        fota_cntx->sp_online = FALSE;
#if defined (RACE_LPCOMM_ENABLE) && defined (RACE_AWS_ENABLE)
#ifndef RACE_RHO_WITHOUT_SMARTPHONE_DISCONNECT_ENABLE
                        if (race_lpcomm_get_role_switch_enable()
#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
                            || BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state()
#endif
                            )
                        {
                            if (race_lpcomm_get_role_switch_enable() &&
                                RACE_FOTA_DOWNLOAD_STATE_RHOING == fota_dl_state)
                            {
                                /* The disconnection is caused by FOTA RHO, Do nothing. Even if RHO API is not
                                                             * called, RHO timer will report RHO fail using RHO_RESULT event. Stop FOTA then.
                                                             */
                                RACE_LOG_MSGID_I("SPP/BLE Disc. It's FOTA RHO", 0);
                            }
                            else
                            {
                                /* Do nothing and stop FOTA when RHO finishes. */
                                RACE_LOG_MSGID_I("SPP/BLE Disc. It's not FOTA RHO", 0);
                            }
                        }
                        else
#endif
#endif
                        {
                            if (RACE_FOTA_DOWNLOAD_STATE_COMMITING == fota_dl_state)
                            {
                                /* Do nothing for it's caused by reboot procedure triggered by FOTA COMMIT. Even if
                                                             * the disconnection occurs before COMMIT CMD procedure finishes, it's OK to do
                                                             * nothing for it's not AWS detach and Agent will decide FOTA fail or reboot on
                                                             * receiving rsp from the partner.  */
                                // TODO: Handle SP disconnects SPP / BLE immediately on receiving 5B of COMMIT cmd
                                /* If SP disconnects SPP / BLE immediately on receiving 5B of COMMIT cmd, AWS may detach
                                                             * before Agent receives the commit RSP from the Partner. (5D of COMMIT may need in such case)
                                                             * Currently SP does not disconnect SPP / BLE on receiving 5B of COMMIT cmd.
                                                             */
                                 RACE_LOG_MSGID_I("SPP/BLE Disc. It's FOTA COMMITING", 0);
                            }
                            else
                            {
                                /* Link loss */
                                RACE_LOG_MSGID_I("SPP/BLE Disc. Stop FOTA for Link lost", 0);
                                race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_AGENT,
                                               RACE_FOTA_STOP_REASON_SP_LOST);
                            }
                        }
                    }
                    /* else FOTA should not care the DISC event of the trans method which it is not using.
                                       * Even if RHO may be able to executed when BLE is connected, RHO service needs
                                       * to notify FOTA to disconnect BLE before RHO if FOTA uses BLE connection.
                                       */
                }
            }
            break;
        }

        case RACE_EVENT_TYPE_CONN_BLE_CONNECT:
        case RACE_EVENT_TYPE_CONN_SPP_CONNECT:
#ifdef MTK_AIRUPDATE_ENABLE
        case RACE_EVENT_TYPE_CONN_AIRUPDATE_CONNECT:
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
        case RACE_EVENT_TYPE_CONN_IAP2_CONNECT:
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
        case RACE_EVENT_TYPE_CONN_GATT_OVER_BREDR_CONNECT:
#endif
        case RACE_EVENT_TYPE_CONN_USB_CONNECT:
        {
            if (fota_cntx && race_fota_is_race_fota_running())
            {
                if ((RACE_FOTA_SP_TRANS_METHOD_SPP == sp_trans_method &&
                     RACE_EVENT_TYPE_CONN_SPP_CONNECT == event_type) ||
                    (RACE_FOTA_SP_TRANS_METHOD_BLE == sp_trans_method &&
                     RACE_EVENT_TYPE_CONN_BLE_CONNECT == event_type)
#ifdef MTK_AIRUPDATE_ENABLE
                    || (RACE_FOTA_SP_TRANS_METHOD_AIRUPDATE == sp_trans_method &&
                        RACE_EVENT_TYPE_CONN_AIRUPDATE_CONNECT == event_type)
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
                    || (RACE_FOTA_SP_TRANS_METHOD_IAP2 == sp_trans_method &&
                        RACE_EVENT_TYPE_CONN_IAP2_CONNECT == event_type)
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
                    || (RACE_FOTA_SP_TRANS_METHOD_GATT_OVER_BREDR == sp_trans_method &&
                        RACE_EVENT_TYPE_CONN_GATT_OVER_BREDR_CONNECT == event_type)
#endif
                     || (RACE_FOTA_SP_TRANS_METHOD_USB == sp_trans_method &&
                         RACE_EVENT_TYPE_CONN_USB_CONNECT == event_type)
                     )
                {
                    /* Works only after RHO because only after SPP/BLE is connected, will fota be running
                                       * if it's starting FOTA. Set TRUE here to allow to continue FOTA without RACE_FOTA_START cmd.
                                       */
                    fota_cntx->sp_online = TRUE;
                }
            }
            break;
        }

#if defined(RACE_LPCOMM_ENABLE) && defined(RACE_AWS_ENABLE)
#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
        case RACE_EVENT_TYPE_BT_RHO_START:
        {
            /* No need to process this event. For Agent, it need do nothing. For Partner, RHO may have
                        * already been done. By then, REQs/RSPs received before RHO have already been processed.
                        */
            RACE_LOG_MSGID_I("role:%x rho_status:%d",2,
                       race_lpcomm_role_get(RACE_LPCOMM_TRANS_METHOD_AWS),
                       bt_role_handover_get_state());
            break;
        }

        /* Only Agent will receive RHO_PREPARE Event. */
        case RACE_EVENT_TYPE_BT_RHO_PREPARE:
        {
            race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();

            RACE_LOG_MSGID_I("fota_cntx:%x dl_state:%d trans_method:%d",3,
                       fota_cntx,
                       fota_dl_state,
                       sp_trans_method);

            /* No need to receive fake rsp because RHO may have already been done when receiving the fake rsp. */
            race_lpcomm_retry_cancel(RACE_APP_ID_FOTA, FALSE);

            if (fota_cntx && race_fota_is_race_fota_running())
            {
                if (RACE_FOTA_DOWNLOAD_STATE_RHOING == fota_dl_state)
                {
                    if (race_lpcomm_get_role_switch_enable())
                    {
                        /* RHO triggered by FOTA */
                        race_timer_smart_stop(race_fota_app_id_timer_id_get(), NULL);
                        race_fota_app_id_timer_id_set(RACE_TIMER_INVALID_TIMER_ID);
                    }
                    else
                    {
                        RACE_LOG_MSGID_W("UNEXPECTED_RHO for FOTA. fota_dl_state:%d",1, fota_dl_state);
                        /* FOTA will be stopped on receiving RHO_RESULT event */
                        race_fota_dl_state_rollback();
                    }
                }
                else if (RACE_FOTA_DOWNLOAD_STATE_COMMITING == fota_dl_state)
                {
                    RACE_LOG_MSGID_E("Err: RHO during FOTA commiting!", 0);
                }
                else
                {
                    /* FOTA will be stopped on receiving RHO_RESULT event */
                    RACE_LOG_MSGID_W("UNEXPECTED_RHO for FOTA. fota_dl_state:%d",1, fota_dl_state);
                }
            }
            bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_FOTA);
            break;
        }
#endif

        case RACE_EVENT_TYPE_BT_RHO_RESULT:
        {
            if (fota_cntx && race_fota_is_race_fota_running())
            {
                race_lpcomm_role_enum role = race_lpcomm_role_get(RACE_LPCOMM_TRANS_METHOD_AWS);
                race_event_rho_result_param_struct *rho_result = (race_event_rho_result_param_struct *)param;
                uint8_t timer_id = RACE_TIMER_INVALID_TIMER_ID;

                if (RACE_FOTA_DOWNLOAD_STATE_RHOING == race_fota_dl_state_get())
                {
                    race_fota_dl_state_rollback();
                    /* Reset Ping timer in case it expirated just after RHO. Ping timeout will not be
                                       * handled during RHO.
                                       */
                    timer_id = race_fota_ping_timer_id_get();
                    if (RACE_TIMER_INVALID_TIMER_ID != timer_id)
                    {
                        if (RACE_LPCOMM_ROLE_AGENT == role)
                        {
                            race_timer_smart_change_period(timer_id, RACE_FOTA_AGENT_PING_INTERVAL_IN_MS);
                        }
                        else
                        {
                            race_timer_smart_change_period(timer_id, RACE_FOTA_PARTNER_PING_INTERVAL_IN_MS);
                        }
                        race_timer_smart_reset(timer_id);
                    }

                    if (rho_result && !rho_result->result)
                    {
                        /* RHO triggered by FOTA failed */
                        /* Do nothing because SP may retry. Even if SP does not retry, Agent will
                                              * detect FOTA fail by app_id timer and Partner will detect FOTA fail by
                                              * ping timer
                                              */
                    }
#ifdef RACE_LPCOMM_SENDER_ROLE_ENABLE
                    else
                    {
                        /* RHO triggered by FOTA succeeded. */
                        RACE_LOG_MSGID_I("Update fota_role to %x after FOTA triggered RHO succeeded", 1, role);
                        fota_cntx->fota_role = role;
                    }
#endif

                    if (RACE_LPCOMM_ROLE_AGENT == role)
                    {
                        uint8_t timer_id = race_fota_app_id_timer_id_get();
                        RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;

                        if (RACE_TIMER_INVALID_TIMER_ID == timer_id)
                        {
                            ret = race_timer_smart_start(&timer_id,
                                                         RACE_FOTA_APP_ID_TIMEOUT_IN_MS,
                                                         race_fota_app_id_timer_expiration_hdl,
                                                         NULL);
                            if (RACE_ERRCODE_SUCCESS == ret)
                            {
                                race_fota_app_id_timer_id_set(timer_id);
                            }
                        }
                        else
                        {
                            RACE_LOG_MSGID_W("app_id timer is not stopped before RHO!", 0);
                            race_timer_smart_reset(timer_id);
                        }

                        if (RACE_ERRCODE_SUCCESS != ret
#ifdef RACE_FOTA_STOP_DURING_RHO_SUPPORT
                            || fota_cntx->fota_stop_required
#endif
                            )
                        {
#ifdef RACE_FOTA_STOP_DURING_RHO_SUPPORT
                            fota_cntx->fota_stop_required = FALSE;
#endif
                            /* Stop FOTA if fail to start app_id timer for app_id timer is important to
                                                      * detect if FOTA fails after RHO.
                                                      */
                            RACE_LOG_MSGID_W("Failed to start app_id timer after RHO!", 0);
                            return race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_AGENT,
                                                  RACE_FOTA_STOP_REASON_FAIL);
                        }
                    }
                    else
                    {
#ifdef RACE_FOTA_STOP_DURING_RHO_SUPPORT
                        if (fota_cntx->fota_stop_required)
                        {
                            fota_cntx->fota_stop_required = FALSE;
                            return race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_PARTNER,
                                                  RACE_FOTA_STOP_REASON_FAIL);
                        }
#endif
                    }
                }
                else
                {
                    if (!rho_result || rho_result->result)
                    {
                        if (RACE_LPCOMM_ROLE_AGENT == role)
                        {
                            /* RHO triggered by others */
                            race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_AGENT,
                                           RACE_FOTA_STOP_REASON_UNEXPECTED_RHO_DONE);
                        }
                        else if (RACE_LPCOMM_ROLE_PARTNER == role)
                        {
                            /* RHO triggered by others */
                            race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_PARTNER,
                                           RACE_FOTA_STOP_REASON_UNEXPECTED_RHO_DONE);
                        }
                    }
                    else
                    {
                        RACE_LOG_MSGID_W("Ignore the failed non-FOTA RHO!", 0);
                    }
                }
            }
            break;
        }
#endif

        default:
            break;
    }

    return RACE_ERRCODE_SUCCESS;
}


RACE_ERRCODE race_fota_get_transmit_interval(uint16_t *spp_transmit_interval, uint16_t *ble_transmit_interval)
{
    if (!spp_transmit_interval && !ble_transmit_interval)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (spp_transmit_interval)
    {
        *spp_transmit_interval = g_race_fota_spp_transmit_interval;
    }

    if (ble_transmit_interval)
    {
        *ble_transmit_interval = g_race_fota_ble_transmit_interval;
    }

    return RACE_ERRCODE_SUCCESS;
}


RACE_ERRCODE race_fota_set_transmit_interval(uint16_t spp_transmit_interval, uint16_t ble_transmit_interval)
{
    g_race_fota_spp_transmit_interval = spp_transmit_interval;
    g_race_fota_ble_transmit_interval = ble_transmit_interval;

    return RACE_ERRCODE_SUCCESS;
}


#ifdef RACE_FOTA_ACTIVE_MODE_KEEP_HFP
bool race_fota_is_hfp_active(void)
{
    bt_sink_srv_state_t srv_state = bt_sink_srv_get_state();
#if defined(RACE_AWS_ENABLE) && !defined(AIR_LE_AUDIO_ENABLE)
    race_lpcomm_role_enum role = race_lpcomm_role_get(RACE_LPCOMM_TRANS_METHOD_AWS);
#endif

#if defined(RACE_AWS_ENABLE) && !defined(AIR_LE_AUDIO_ENABLE)
    if (RACE_LPCOMM_ROLE_AGENT != role)
    {
        return FALSE;
    }
#endif

    if (BT_SINK_SRV_STATE_INCOMING == srv_state ||
        BT_SINK_SRV_STATE_OUTGOING == srv_state ||
        BT_SINK_SRV_STATE_ACTIVE == srv_state ||
        BT_SINK_SRV_STATE_TWC_INCOMING == srv_state ||
        BT_SINK_SRV_STATE_TWC_OUTGOING == srv_state ||
        BT_SINK_SRV_STATE_HELD_ACTIVE == srv_state ||
        BT_SINK_SRV_STATE_HELD_REMAINING == srv_state ||
        BT_SINK_SRV_STATE_MULTIPARTY == srv_state)
    {
        return TRUE;
    }

    return FALSE;
}
#endif


RACE_ERRCODE race_fota_is_fota_mode_allowed(race_fota_mode_enum fota_mode)
{
    if (RACE_FOTA_MODE_MAX <= fota_mode
#ifndef RACE_FOTA_ACTIVE_MODE_ENABLE
        || RACE_FOTA_MODE_ACTIVE == fota_mode
#endif
#ifndef RACE_FOTA_ADAPTIVE_MODE_ENABLE
        || RACE_FOTA_MODE_ADAPTIVE == fota_mode
#endif
        )
    {
        return RACE_ERRCODE_NOT_SUPPORT;
    }

#ifdef RACE_FOTA_ACTIVE_MODE_KEEP_HFP
    if (RACE_FOTA_MODE_ACTIVE == fota_mode &&
        race_fota_is_hfp_active())
    {
        return RACE_ERRCODE_REJECT_FOR_CALL_ONGOING;
    }
#endif

    if (g_race_fota_cntx_ptr &&
        (RACE_FOTA_MODE_MAX != g_race_fota_cntx_ptr->fota_mode &&
         fota_mode != g_race_fota_cntx_ptr->fota_mode))
    {
        return RACE_ERRCODE_CONFLICT;
    }

    return RACE_ERRCODE_SUCCESS;
}


RACE_ERRCODE race_fota_is_dl_method_allowed(race_fota_dual_device_dl_method_enum dl_method)
{
    if (RACE_FOTA_DUAL_DEVICE_DL_METHOD_MAX <= dl_method
#ifndef RACE_FOTA_DUAL_DEVICE_CONCURRENT_DOWNLOAD_ENABLE
        || RACE_FOTA_DUAL_DEVICE_DL_METHOD_CONCURRENT == dl_method
#endif
        )
    {
        return RACE_ERRCODE_NOT_SUPPORT;
    }

    if (g_race_fota_cntx_ptr &&
        (RACE_FOTA_DUAL_DEVICE_DL_METHOD_MAX != g_race_fota_cntx_ptr->dl_method &&
         dl_method != g_race_fota_cntx_ptr->dl_method))
    {
        return RACE_ERRCODE_CONFLICT;
    }

    return RACE_ERRCODE_SUCCESS;
}


RACE_ERRCODE race_fota_start_check_params(race_recipient_type_enum recipient_type,
                                                 race_fota_mode_enum fota_mode,
                                                 race_fota_dual_device_dl_method_enum dl_method)
{
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;

    if (RACE_RECIPIENT_TYPE_NONE == recipient_type ||
        RACE_RECIPIENT_TYPE_MAX <= recipient_type ||
        RACE_RECIPIENT_TYPE_PARTNER_ONLY == recipient_type)
    {
        ret = RACE_ERRCODE_NOT_SUPPORT;
    }

    if (RACE_ERRCODE_SUCCESS == ret)
    {
        ret = race_fota_is_fota_mode_allowed(fota_mode);
    }

    if (RACE_ERRCODE_SUCCESS == ret &&
        RACE_RECIPIENT_TYPE_AGENT_PARTNER == recipient_type)
    {
        ret = race_fota_is_dl_method_allowed(dl_method);
    }

    return ret;
}


#ifdef RACE_LPCOMM_ENABLE
race_lpcomm_role_enum race_fota_get_role(void)
{
#ifdef RACE_AWS_ENABLE
    return race_lpcomm_role_get(RACE_LPCOMM_TRANS_METHOD_AWS);
#else
#ifdef RACE_LPCOMM_SENDER_ROLE_ENABLE
    if (g_race_fota_cntx_ptr)
    {
        return g_race_fota_cntx_ptr->fota_role;
    }
#endif
    return RACE_LPCOMM_ROLE_NONE;
#endif
}
#endif
#endif /* RACE_FOTA_CMD_ENABLE */

