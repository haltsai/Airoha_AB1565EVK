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


#ifndef __RACE_FOTA_UTIL_H__
#define __RACE_FOTA_UTIL_H__


#include "race_cmd_feature.h"
#ifdef RACE_FOTA_CMD_ENABLE
#include "bt_connection_manager.h"
#include "race_cmd.h"
#include "race_event_internal.h"
#include "race_lpcomm_retry.h"
#include "race_util.h"
#include "race_cmd_fota.h"
#include "race_lpcomm_util.h"
#include "race_event.h"


////////////////////////////////////////////////////////////////////////////////
// DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RACE_FOTA_START_GET_FOTA_MODE(p)  ((p) & 0x0F)
#define RACE_FOTA_START_GET_DL_METHOD(p)  (((p) & 0xF0) >> 4)


////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RACE_FOTA_APP_ID_TIMEOUT_IN_MS (RACE_FOTA_SP_MAX_RETRY_TIME * RACE_FOTA_SP_RETRY_INTERVAL + 1000)  /* (SPP/BLE max retry interval + 1)s */
#define RACE_FOTA_PING_INTERVAL_IN_MS  (RACE_FOTA_SP_MAX_RETRY_TIME * ((1 + RACE_LPCOMM_RETRY_MAX_TIME) * RACE_LPCOMM_RETRY_TIMEOUT_IN_MS + 1000))
#define RACE_FOTA_AGENT_PING_INTERVAL_IN_MS  RACE_FOTA_PING_INTERVAL_IN_MS
#define RACE_FOTA_PARTNER_PING_INTERVAL_IN_MS  (RACE_FOTA_AGENT_PING_INTERVAL_IN_MS + ((1 + RACE_LPCOMM_RETRY_MAX_TIME) * RACE_LPCOMM_RETRY_TIMEOUT_IN_MS))
#define RACE_FOTA_AGENT_STOP_TIMEOUT_IN_MS (3000)
#define RACE_FOTA_PARTNER_STOP_TIMEOUT_IN_MS (RACE_FOTA_AGENT_STOP_TIMEOUT_IN_MS + RACE_LPCOMM_RETRY_MAX_TIME * RACE_LPCOMM_RETRY_TIMEOUT_IN_MS)


////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef enum
{
    RACE_FOTA_DOWNLOAD_STATE_NONE,

    RACE_FOTA_DOWNLOAD_STATE_STARTING,
    RACE_FOTA_DOWNLOAD_STATE_START,
    RACE_FOTA_DOWNLOAD_STATE_STOPPING,
    RACE_FOTA_DOWNLOAD_STATE_RHOING,
    /* This state should not be trust. For example, other module erases the last 4K of FOTA partition and
         * destory the downloaded fota package, but FOTA may have no way to know to reset the dl_state.
         */
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
    RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT,
#endif
    RACE_FOTA_DOWNLOAD_STATE_COMMITING,

    RACE_FOTA_DOWNLOAD_STATE_MAX = 0xFF
}race_fota_download_state_enum;

typedef enum
{
    RACE_FOTA_SP_TRANS_METHOD_NONE,

    RACE_FOTA_SP_TRANS_METHOD_SPP,
    RACE_FOTA_SP_TRANS_METHOD_BLE,
    RACE_FOTA_SP_TRANS_METHOD_AIRUPDATE,
    RACE_FOTA_SP_TRANS_METHOD_IAP2,
    RACE_FOTA_SP_TRANS_METHOD_GATT_OVER_BREDR,
    RACE_FOTA_SP_TRANS_METHOD_USB,

    RACE_FOTA_SP_TRANS_METHOD_MAX
}race_fota_sp_trans_method_enum;

typedef enum
{
    RACE_FOTA_STOP_STATUS_NONE,

    /* Agent status */
    RACE_FOTA_STOP_STATUS_WAIT_FOR_QUERY_RSP,
    RACE_FOTA_STOP_STATUS_WAIT_FOR_RESULT_RSP,
    RACE_FOTA_STOP_STATUS_WAIT_FOR_5B,
    RACE_FOTA_STOP_STATUS_WAIT_FOR_5D,

    /* Partner status */
    RACE_FOTA_STOP_STATUS_WAIT_FOR_RESULT_REQ,

    RACE_FOTA_STOP_STATUS_MAX = 0xFF
}race_fota_stop_status_enum;


typedef enum
{
    RACE_FOTA_ACTIVE_MODE_ACTION_NONE = 0,

    RACE_FOTA_ACTIVE_MODE_ACTION_HFP_DISCONNECT = 0x01,
    RACE_FOTA_ACTIVE_MODE_ACTION_A2DP_PAUSE = 0x02,
    RACE_FOTA_ACTIVE_MODE_ACTION_A2DP_DISCONNECT = 0x04,
    RACE_FOTA_ACTIVE_MODE_ACTION_DSP_SUSPEND = 0x80,

    RACE_FOTA_ACTIVE_MODE_ACTION_MAX = 0xFFFF
}race_fota_active_mode_action_enum;


typedef enum
{
    RACE_FOTA_DUAL_DEVICE_DL_METHOD_NONE = 0,

    RACE_FOTA_DUAL_DEVICE_DL_METHOD_RHO = RACE_FOTA_DUAL_DEVICE_DL_METHOD_NONE,
    RACE_FOTA_DUAL_DEVICE_DL_METHOD_CONCURRENT,

    RACE_FOTA_DUAL_DEVICE_DL_METHOD_MAX    /* Must not exceed 0x0F */
}race_fota_dual_device_dl_method_enum;


typedef enum
{
    RACE_FOTA_MODE_BACKGROUND = 0,
    RACE_FOTA_MODE_ACTIVE,
    RACE_FOTA_MODE_ADAPTIVE,

    RACE_FOTA_MODE_MAX    /* Must not exceed 0x0F */
}race_fota_mode_enum;


typedef struct
{
    bool is_dual_fota;
    race_fota_mode_enum fota_mode;
    race_fota_dual_device_dl_method_enum dl_method;
    race_fota_sp_trans_method_enum sp_trans_method;
    race_fota_download_state_enum old_dl_state;
    race_fota_download_state_enum dl_state;
    uint8_t app_id_timer_id;
    uint8_t commit_timer_id;
    uint8_t stop_timer_id;
#ifdef RACE_LPCOMM_ENABLE
    uint8_t ping_timer_id;
    bool lpcomm_peer_online;
#endif
    bool sp_online;
    race_fota_stop_status_enum stop_status;
    race_fota_stop_originator_enum originator;
    race_fota_stop_reason_enum reason;
    bool fota_stop_required;
    bool transfer_complete;
#ifdef RACE_LPCOMM_SENDER_ROLE_ENABLE
    race_lpcomm_role_enum fota_role; /* The device that receives the fota start cmd from SP directly is the Agent. */
#endif
}race_fota_cntx_struct;

typedef struct
{
    uint8_t sender;
    race_recipient_type_enum recipient_type;
    uint8_t reason;
    uint8_t channel_id;
}race_fota_stop_agent_sp_stop_req_struct;

typedef struct
{
    uint8_t status;
    race_recipient_type_enum recipient_type;
}race_fota_stop_agent_sp_stop_rsp_struct;

typedef struct
{
    uint8_t status;
    race_recipient_type_enum recipient_type;
}race_fota_stop_agent_sp_stop_noti_struct;

typedef struct
{
    int8_t status;
    uint8_t channel_id;
    race_fota_stop_originator_enum originator;
    race_fota_stop_reason_enum reason;
}race_fota_stop_agent_partner_query_rsp_struct;

typedef race_fota_stop_agent_partner_query_rsp_struct race_fota_stop_partner_agent_query_rsp_struct;

typedef struct
{
    int8_t status;  /* Partner result */
    int8_t result;  /* The same result in STOP_RESULT REQ.  */
    uint8_t channel_id;
    race_fota_stop_originator_enum originator;
    race_fota_stop_reason_enum reason;
}race_fota_stop_agent_partner_result_rsp_struct;

typedef struct
{
    uint8_t channel_id;
    uint16_t process_id;
    uint8_t trans_method;
    race_fota_stop_originator_enum originator;
    race_fota_stop_reason_enum reason;
}race_fota_stop_partner_agent_query_req_struct;

typedef race_fota_stop_partner_agent_query_req_struct race_fota_stop_agent_partner_query_req_struct;

typedef struct
{
    bool result;
    uint8_t channel_id;
    uint16_t process_id;
    uint8_t trans_method;
    race_fota_stop_originator_enum originator;
    race_fota_stop_reason_enum reason;
}race_fota_stop_partner_agent_result_req_struct;


typedef struct
{
    uint8_t channel_id;
    race_recipient_type_enum recipient_type;
    race_fota_stop_originator_enum originator;
    race_fota_stop_reason_enum reason;
}race_fota_stop_partner_active_stop_struct;

typedef struct
{
    uint8_t channel_id;
    race_recipient_type_enum recipient_type;
    race_fota_stop_originator_enum originator;
    race_fota_stop_reason_enum reason;
}race_fota_stop_agent_active_stop_struct;


typedef struct
{
    uint8_t address[RACE_EVENT_REMOTE_DEVICE_ADDRESS_LENGTH];
    uint8_t address_length;
}race_fota_commit_timer_msg_struct;

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
RACE_ERRCODE race_fota_stop(race_fota_stop_originator_enum originator,
                                race_fota_stop_reason_enum reason);

bool race_fota_is_stop_allowed(bool *execute_stop);

RACE_ERRCODE race_fota_stop_agent_sp_stop_noti_process(race_fota_stop_agent_sp_stop_noti_struct *sp_stop_noti);

RACE_ERRCODE race_fota_stop_agent_sp_stop_req_process(race_fota_stop_agent_sp_stop_req_struct *sp_stop_req);

RACE_ERRCODE race_fota_stop_agent_sp_stop_rsp_process(race_fota_stop_agent_sp_stop_rsp_struct *sp_stop_rsp);

RACE_ERRCODE race_fota_stop_agent_result_rsp_process(race_fota_stop_agent_partner_result_rsp_struct *result_rsp);

RACE_ERRCODE race_fota_stop_agent_query_rsp_process(race_fota_stop_agent_partner_query_rsp_struct *query_rsp);

RACE_ERRCODE race_fota_stop_agent_query_req_process(race_fota_stop_agent_partner_query_req_struct *query_req);

RACE_ERRCODE race_fota_stop_partner_result_req_process(race_fota_stop_partner_agent_result_req_struct *result_req);

RACE_ERRCODE race_fota_stop_partner_query_req_process(race_fota_stop_partner_agent_query_req_struct *query_req);

RACE_ERRCODE race_fota_stop_partner_query_rsp_process(race_fota_stop_partner_agent_query_rsp_struct *query_rsp);

RACE_ERRCODE race_fota_stop_msg_process(race_general_msg_t *msg);

void race_fota_stop_agent_reset(void);

race_fota_cntx_struct *race_fota_cntx_get(void);

uint8_t race_fota_app_id_timer_id_get(void);

RACE_ERRCODE race_fota_app_id_timer_id_set(uint8_t timer_id);

void race_fota_app_id_timer_expiration_hdl(uint8_t id, void *user_data);

uint8_t race_fota_commit_timer_id_get(void);

RACE_ERRCODE race_fota_commit_timer_id_set(uint8_t timer_id);

void race_fota_commit_timer_expiration_hdl(uint8_t id, void *user_data);

#ifdef RACE_LPCOMM_ENABLE
uint8_t race_fota_ping_timer_id_get(void);

RACE_ERRCODE race_fota_ping_timer_id_set(uint8_t timer_id);

RACE_ERRCODE race_fota_send_ping_req_to_peer(void);

void race_fota_ping_timer_expiration_hdl(uint8_t id, void *user_data);

void race_fota_stop_partner_reset();
#endif

uint8_t race_fota_stop_timer_id_get(void);

RACE_ERRCODE race_fota_stop_timer_id_set(uint8_t timer_id);

void race_fota_stop_timer_expiration_hdl(uint8_t id, void *user_data);

void race_fota_set_fota_mode(race_fota_mode_enum fota_mode);

#ifdef RACE_FOTA_ADAPTIVE_MODE_ENABLE
bool race_fota_is_adaptive_mode(void);
#endif

/* The return value is valid only when race fota is running. */
bool race_fota_is_dual_fota(void);

race_fota_download_state_enum race_fota_dl_state_get(void);

void race_fota_dl_state_set(race_fota_download_state_enum state);

void race_fota_dl_state_rollback(void);

void race_fota_sp_trans_method_set(race_fota_sp_trans_method_enum sp_trans_method);

race_fota_sp_trans_method_enum race_fota_sp_trans_method_get(void);

RACE_ERRCODE race_fota_set_sp_trans_method_by_channel_id(uint8_t channel_id);

/* Return the channel_id fota is used based on sp_trans_method. */
RACE_ERRCODE race_fota_channel_id_get(uint8_t *channel_id);

bool race_fota_is_cmd_allowed(race_fota_stop_reason_enum *reason, uint16_t cmd_id, uint8_t channel_id);

RACE_ERRCODE race_fota_cmd_preprocess(uint16_t cmd_id,
                                              uint8_t cmd_type,
                                              uint8_t channel_id);

RACE_ERRCODE race_fota_active_bt_preparation_revert(void);

RACE_ERRCODE race_fota_active_bt_preparation(void);

RACE_ERRCODE race_fota_init(void);

RACE_ERRCODE race_fota_integrity_check(uint32_t signature_start_address,
                                             uint8_t storage_type,
                                             uint8_t *signature_generated,
                                             uint8_t signature_generated_size);

bool race_fota_is_race_fota_running(void);

RACE_ERRCODE race_fota_get_transmit_interval(uint16_t *spp_transmit_interval, uint16_t *ble_transmit_interval);

RACE_ERRCODE race_fota_start_check_params(race_recipient_type_enum recipient_type,
                                                 race_fota_mode_enum fota_mode,
                                                 race_fota_dual_device_dl_method_enum dl_method);
#ifdef RACE_LPCOMM_ENABLE
race_lpcomm_role_enum race_fota_get_role(void);
#endif

#endif /* RACE_FOTA_CMD_ENABLE */

#endif /* __RACE_FOTA_UTIL_H__ */

