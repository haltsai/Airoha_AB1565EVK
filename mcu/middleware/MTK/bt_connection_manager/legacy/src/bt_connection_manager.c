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


#include "bt_sink_srv.h"
#include "bt_connection_manager_internal.h"
#include "bt_gap_le.h"
#include "bt_connection_manager_utils.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_device_manager_config.h"
#include "bt_device_manager.h"
#include "hal_wdt.h"
#include "bt_sink_srv_music.h"
#include "hal.h"

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "bt_role_handover.h"
#endif

#ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
#include "bt_connection_manager_state_machine_ext.h"
#endif

#endif
#include "bt_os_layer_api.h"
#include "bt_device_manager_internal.h"
#include "bt_device_manager_db.h"
#include "bt_di.h"

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_sink_srv_cm_get_hfp_info=_default_bt_sink_srv_cm_get_hfp_info")
#pragma comment(linker, "/alternatename:_bt_sink_srv_cm_get_a2dp_sink_info=_default_bt_sink_srv_cm_get_a2dp_sink_info")
#pragma comment(linker, "/alternatename:_bt_sink_srv_cm_get_avrcp_info=_default_bt_sink_srv_cm_get_avrcp_info")
#pragma comment(linker, "/alternatename:_bt_sink_srv_cm_get_pbapc_info=_default_bt_sink_srv_cm_get_pbapc_info")
#pragma comment(linker, "/alternatename:_bt_sink_srv_cm_get_aws_info=_default_bt_sink_srv_cm_get_aws_info")
#pragma comment(linker, "/alternatename:_bt_sink_srv_cm_get_hsp_info=_default_bt_sink_srv_cm_get_hsp_info")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_sink_srv_cm_get_hf_info = default_bt_sink_srv_cm_get_hf_info
#pragma weak bt_sink_srv_cm_get_a2dp_sink_info = default_bt_sink_srv_cm_get_a2dp_sink_info
#pragma weak bt_sink_srv_cm_get_avrcp_info = default_bt_sink_srv_cm_get_avrcp_info
#pragma weak bt_sink_srv_cm_get_pbapc_info = default_bt_sink_srv_cm_get_pbapc_info
#pragma weak bt_sink_srv_cm_get_aws_info = default_bt_sink_srv_cm_get_aws_info
#pragma weak bt_sink_srv_cm_get_hsp_info = default_bt_sink_srv_cm_get_hsp_info
#else
#error "Unsupported Platform"
#endif

bool bt_sink_srv_cm_ls_enable_t = true;

static bool g_bt_cm_reload_n9_flag = false;
static bt_connection_manager_switch_mode_params_t g_n9_fw_reload_context;


static bool bt_cm_is_force_enable_dut = false;
static bool bt_cm_is_reload_n9 = false;
#ifdef MTK_AWS_MCE_ENABLE
static void bt_connection_manager_dut_mode_switch_role();
#endif
bool bt_connection_manager_get_dut_mode(void);
void bt_connection_manager_force_enable_dut_mode(bool dut);

extern void bt_gap_force_enable_dut(bool dut);



void* default_bt_sink_srv_cm_get_hf_info(uint8_t device_idx)
{
    return NULL;
}
void* default_bt_sink_srv_cm_get_a2dp_sink_info(uint8_t device_idx)
{
    return NULL;
}
void* default_bt_sink_srv_cm_get_avrcp_info(uint8_t device_idx)
{
    return NULL;
}
void* default_bt_sink_srv_cm_get_pbapc_info(uint8_t device_idx)
{
    return NULL;
}
void* default_bt_sink_srv_cm_get_aws_info(uint8_t device_idx)
{
    return NULL;
}

void* default_bt_sink_srv_cm_get_hsp_info(uint8_t device_idx)
{
    return NULL;
}

static bool bt_connection_manager_open_visibility_feature(void);
bt_sink_srv_cm_context_t g_sink_srv_cm_context;
bt_bd_addr_t g_sink_srv_cm_last_connected;
//static bt_sink_srv_cm_dev_info_t g_sink_srv_cm_dev_info[BT_SINK_SRV_CM_MAX_TRUSTED_DEV];

uint16_t BT_SINK_SRV_LINK_LOSS_RETRY_NUM_FOR_QA_TEST = 0xFFFF;
static uint8_t bt_sink_srv_role_switch_retry_times = 0xFF;
static bt_connection_manager_power_on_reconnect_allow_t g_bt_cm_power_reconnect_flag =
    BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_ACTIVE | BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_PASSIVE;
static bool g_bt_cm_power_on_open_discoverable = false;
static bool g_bt_cm_disconnect_all = false;
static uint8_t g_bt_cm_need_switch_slave = true;
static bool g_bt_cm_dut_enable = false;

static const uint8_t bt_sink_srv_connection_manager_search_di_pattern[] =
{
    BT_SDP_ATTRIBUTE_HEADER_8BIT(3),             /* Data Element Sequence, 9 bytes */

    BT_SDP_UUID_16BIT(BT_DI_SDP_SERVICE_CLASS_PNP_INFORMATION), /* The device Identification UUID in big-endian. */
    0x02, 0x00
};

static const uint8_t bt_sink_srv_connection_manager_search_di_attributes[] = {
    0x00, 0x64, /* 0x0064, max handle for attribute return */

    BT_SDP_ATTRIBUTE_HEADER_8BIT(9),

    BT_SDP_UINT_16BIT(BT_DI_SDP_ATTRIBUTE_VENDOR_ID),
    BT_SDP_UINT_16BIT(BT_DI_SDP_ATTRIBUTE_PRODUCT_ID),
    BT_SDP_UINT_16BIT(BT_DI_SDP_ATTRIBUTE_VERSION)
};


#define BT_SINK_SRV_CM_STATE_CHANGE(device, new_state)                                    \
{                                                                                     \
    bt_cmgr_report_id("[BT_CM][I] State Change, previous_state:%d, new_state:%d", 2, device->link_state, new_state); \
    device->link_state = new_state;                                    \
}

#if defined(__BT_SINK_SRV_DEBUG_INFO__ )
const static char *g_gap_event_string[] = {
    "BT_GAP_LINK_STATUS_UPDATED_IND", /**< This event indicates the the connection status changed, with #bt_gap_link_status_updated_ind_t payload. */
   // "BT_GAP_LINK_KEY_REQ_IND", /**< This event asks user for link key, with #bt_gap_link_key_notification_ind_t payload, user need fill the parameters with key information if the link key is found. */
    "BT_GAP_IO_CAPABILITY_REQ_IND", /**< This event asks user for io capability response, with #bt_gap_connection_handle_t payload, user need invoke #bt_gap_reply_io_capability_request() API to accept and reply the pairing request.
                                                                                        or invoke #bt_gap_reject_io_capability_request() API to reject the pairing request. */
    "BT_GAP_USER_CONFIRM_REQ_IND", /**< This event asks user for value confirm, with uint32_t payload, user need invoke #bt_gap_reply_user_confirm_request() API to replay it according to value of event parameters. */
    "BT_GAP_USER_PASSKEY_NOTIFICATION_IND", /**< This event notify the passkey value to the user, with uint32_t payload, user need display the passkey on the screen. */
    "BT_GAP_USER_PASSKEY_NOTIFICATION_END_IND", /**< This event notify the passkey enterd to the user, with NULL payload. user need cancel the passkey showing on the screen. */
    "BT_GAP_LINK_KEY_NOTIFICATION_IND", /**< This event notify user to update link key information, with #bt_gap_link_key_notification_ind_t payload. */
    "BT_GAP_USER_REPLY_PASSKEY_IND", /**< This event asks user for passkey, with NULL payload, user need invoke #bt_gap_reply_passkey_request() API to replay it according to value of event parameters. */
    "BT_GAP_SET_SCAN_MODE_CNF", /**< This event indicates the confirmation of setting scan mode with NULL payload. */
    "BT_GAP_INQUIRY_CNF", /**< This event indicates the confirmation of inquiry with NULL payload. */
    "BT_GAP_CANCEL_INQUIRY_CNF", /**< This event indicates the confirmation of cancel inquiry with NULL payload. */
    "BT_GAP_INQUIRY_IND", /**< This event indicates the result of inquiry with #bt_gap_inquiry_ind_t payload. */
    "BT_GAP_INQUIRY_COMPLETE_IND", /**< This event indicates the completement of inquiry with NULL payload. */
    "BT_GAP_SET_ROLE_CNF", /**< This event indicates the confirmation of #bt_gap_set_role() with NULL payload. */
    "BT_GAP_ROLE_CHANGED_IND", /**< This event indicates the connection role changed with #bt_gap_role_changed_ind_t payload. */
    "BT_GAP_READ_REMOTE_NAME_CNF", /**< This event indicates the confirmation of #bt_gap_read_remote_name_role() with NULL payload. */
    "BT_GAP_READ_REMOTE_NAME_COMPLETE_IND", /**< This event indicates the result of #bt_gap_read_remote_name_role() with #bt_gap_read_remote_name_complete_ind_t payload. */
    "BT_GAP_READ_CANCEL_REMOTE_NAME_CNF", /**< This event indicates the confirmation of #bt_gap_cancel_name_request() with NULL payload. */
    "BT_GAP_ENTER_TEST_MODE_CNF", /**< This event indicates the confirmation of #bt_gap_enter_test_mode() with NULL payload. */
    "BT_GAP_WRITE_INQUIRY_TX_PWR_CNF", /**< This event indicates the confirmation of #bt_gap_write_inquiry_tx() with NULL payload. */
    "BT_GAP_READ_INQUIRY_RESPONSE_TX_CNF", /**< This event indicates the confirmation of #bt_gap_read_inquiry_response_tx() with uint8_t payload. */
    "BT_GAP_READ_RSSI_CNF", /**< This event indicates the confirmation of #bt_gap_read_rssi() with #bt_gap_read_rssi_cnf_t payload. */
    "BT_GAP_GET_ROLE_CNF", /**< This event indicates the confirmation of #bt_gap_get_role() with bt_gap_get_role_cnf_t payload. */
    "BT_GAP_SET_EIR_CNF", /**< This event indicates the confirmation of #bt_gap_set_extended_inquiry_response() with NULL payload. */
    "BT_GAP_BONDING_START_IND", /**< This event indicates the start of bonding with NULL payload. */
    "BT_GAP_BONDING_COMPLETE_IND", /**< This event indicates the result of bonding with NULL payload. */
    "BT_GAP_WRITING_PAGE_SCAN_CNF", /**< This event indicates the confirmation of #bt_gap_write_page_scan_activity() with NULL payload. */
    "BT_GAP_WRITING_INQUIRY_SCAN_CNF",  /**< This event indicates the confirmation of #bt_gap_write_inquiry_scan_activity() with NULL payload. */
    "BT_GAP_WRITE_STORED_LINK_KEY_CNF", /**< This event indicates the confirmation of #bt_gap_write_stored_link_key, with #bt_gap_write_stored_link_key_cnf_t payload.. */
    "BT_GAP_DELETE_STORED_LINK_KEY_CNF", /**< This event indicates the confirmation of #bt_gap_delete_stored_link_key, with #bt_gap_delete_stored_link_key_cnf_t payload. */
    "BT_GAP_SNIFF_MODE_CHANGE_IND", /**< This event indicates link mode change, with #bt_gap_sniff_mode_changed_ind_t payload. */
    "BT_GAP_WRITE_PAGE_TIMEOUT_CNF",    /**< This event indicates the confirmation of #bt_gap_write_page_timeout() with NULL payload. */
    "BT_GAP_WRITE_SUPERVISION_TIMEOUT_CNF", /**< This event indicates the confirmation of #bt_gap_write_supervision_timeout() with #bt_gap_write_supervision_timeout_cnf_t payload. */
    "BT_GAP_CANCEL_CONNECTION_CNF", /**< This event indicates the confirmation of cancel connection with #bt_gap_cancel_connection_cnf_t payload. */
    "BT_GAP_READ_RAW_RSSI_CNF", /**< This event indicates the confirmation of #bt_gap_read_raw_rssi() with #bt_gap_read_rssi_cnf_t payload. */
    "BT_GAP_WRITE_INQUIRY_ACCESS_CODE_CNF", /**< This event indicates the confirmation of #bt_gap_write_inquiry_access_code() with NULL payload. */
    "BT_GAP_READ_REMOTE_VERSION_CNF",   /**< This event indicates the confirmation of #bt_gap_read_remote_version_information() with NULL payload. */
    "BT_GAP_READ_REMOTE_VERSION_COMPLETE_IND"   /**< This event indicates the result of #bt_gap_read_remote_version_information() with #bt_gap_read_remote_version_complete_ind_t payload. */
};

const static char *g_system_event_string[] = {
    "BT_POWER_ON_CNF",
    "BT_POWER_OFF_CNF",
    "BT_PANIC"
};
#endif /* defined(__BT_SINK_SRV_DEBUG_INFO__ ) */

static void bt_connection_manager_handle_power_on_cnf(void);
static void bt_connection_manager_connect_internal(bt_sink_srv_profile_connection_action_t *action_param);
static void bt_connection_manager_request_done(bt_status_t result, bt_sink_srv_cm_node_t *req);
static void bt_connection_manager_link_connected(bt_bd_addr_t *dev_addr, bt_gap_connection_handle_t handle, bt_gap_link_status_t link_status);
static bt_status_t bt_connection_manager_connect_action_handle(bt_bd_addr_t *addr);
static void bt_connection_manager_disconnect_action_handle(bt_bd_addr_t *param);
static void bt_sink_srv_cm_init(void);
static void bt_sink_srv_cm_deinit(void);
static void bt_connection_manager_power_on_action_handler(void);
//static uint32_t bt_connection_manager_get_all_connected_device(bt_bd_addr_t *addr_list);
static bt_status_t bt_connection_manager_power_on_reconnection();
static void bt_connection_manager_discoverable_action_handle(bool enable);

#ifdef MTK_AWS_MCE_ENABLE
static void bt_connection_manager_write_aws_key(void);
static void bt_connection_manager_partner_sync_address_to_agent(void);
static void bt_connection_manager_enable_aws_discoverable(bool enable);
static void bt_connection_manager_enable_aws_discoverable_by_device(bt_sink_srv_cm_remote_device_t *device_p, bool enable);
static void bt_connection_manager_aws_mce_packet_callback(bt_aws_mce_report_info_t *para);
static void bt_connection_manager_aws_sync_connnection_confirm(void);
#endif

static bool bt_connection_manager_is_valid_address(bt_bd_addr_t *address)
{
    static const bt_bd_addr_t empty_address = {0};
    return ((address != NULL) && (bt_connection_manager_memcmp(address, empty_address, 6) != 0));
}

static void bt_connection_manager_node_insert_head(bt_sink_srv_cm_node_t *head, bt_sink_srv_cm_node_t *node)
{
    bt_connection_manager_assert(head && node);
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

static void bt_connection_manager_node_insert_tail(bt_sink_srv_cm_node_t *head, bt_sink_srv_cm_node_t *node)
{
    bt_connection_manager_assert(head && node);
    node->next = head;
    node->prev = head->prev;
    head->prev->next = node;
    head->prev = node;
}

static bt_sink_srv_cm_node_t *bt_connection_manager_node_new(bt_sink_srv_cm_request_t *request)
{
    bt_sink_srv_cm_node_t *new_req = NULL;
    bt_connection_manager_assert(request);
    if (NULL != (new_req = bt_connection_manager_memory_alloc(sizeof(bt_sink_srv_cm_node_t)))) {
        bt_connection_manager_memset(new_req, 0, sizeof(bt_sink_srv_cm_node_t));
        if (NULL != (new_req->parameters = bt_connection_manager_memory_alloc(sizeof(bt_sink_srv_cm_request_t)))) {
            bt_connection_manager_memcpy(new_req->parameters, request, sizeof(bt_sink_srv_cm_request_t));
        }
    }
    return new_req;
}

static bt_sink_srv_cm_node_t *bt_connection_manager_node_get_next(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_cm_node_t *cur_req = g_sink_srv_cm_context.request_list.next;
    bt_sink_srv_cm_node_t *result = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (cur_req != &g_sink_srv_cm_context.request_list) {
        result = cur_req;
        cur_req->next->prev = cur_req->prev;
        cur_req->prev->next = cur_req->next;
        result->next = result;
        result->prev = result;
    }
    return result;
}

static void bt_connection_manager_node_delete(bt_sink_srv_cm_node_t *del_req)
{
    if (del_req) {
        if (del_req->parameters) {
            bt_connection_manager_memory_free(del_req->parameters);
        }
        bt_connection_manager_memory_free(del_req);
    }
}

static void bt_connection_manager_node_remove(bt_sink_srv_cm_node_t *node)
{
    bt_connection_manager_assert(node);
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static void bt_connection_manager_request_execute(bt_sink_srv_cm_node_t *req)
{
    if(NULL != req && NULL != req->parameters) {
        switch (req->parameters->type) {
            case BT_SINK_SRV_CM_REQ_TYPE_NORMAL:
            case BT_SINK_SRV_CM_REQ_TYPE_POWER_ON:
            case BT_SINK_SRV_CM_REQ_TYPE_LINK_LOST: {
                bt_connection_manager_connect_internal(&req->parameters->action_param);
            }
            break;

            default:
                break;
        }
    }
}

static void bt_connection_manager_request_delay_execute(void *params)
{
    bt_sink_srv_cm_node_t *req = (bt_sink_srv_cm_node_t *)params;

    if (req && g_sink_srv_cm_context.request_pending) {
        bt_connection_manager_request_execute(req);
    } else {
        bt_cmgr_report_id("[BT_CM][W] g_sink_srv_cm_context.request_pending is NULL ore params is NULL, Skip", 0);
    }
}

static bt_status_t bt_connection_manager_request_permit(bt_sink_srv_cm_request_t *request)
{
    bt_status_t result = BT_CONNECTION_MANAGER_STATUS_INVALID_PARAM;
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    bt_connection_manager_assert(request);
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, &request->action_param.address);

    switch (request->type) {
        case BT_SINK_SRV_CM_REQ_TYPE_NORMAL:
        case BT_SINK_SRV_CM_REQ_TYPE_POWER_ON:
        case BT_SINK_SRV_CM_REQ_TYPE_LINK_LOST:
            if (!bt_connection_manager_is_valid_address(&request->action_param.address)) {
                result = BT_CONNECTION_MANAGER_STATUS_FAIL;
            } else if (((BT_SINK_SRV_CM_MAX_DEVICE_NUMBER == g_sink_srv_cm_context.connected_dev_num) ||
                          (BT_AWS_MCE_ROLE_AGENT == aws_role && (BT_SINK_SRV_CM_MAX_DEVICE_NUMBER - 1) == g_sink_srv_cm_context.connected_dev_num)) &&
                          NULL == device_p) {
                result = BT_CONNECTION_MANAGER_STATUS_MAX_LINK;
            } else if (BT_SINK_SRV_CM_REQ_TYPE_NORMAL != request->type && NULL != device_p) {
                bt_sink_srv_profile_type_t req_profile = request->action_param.profile_connection_mask;
                bt_sink_srv_profile_type_t connected_profile = device_p->profile_connection_mask;
                req_profile = req_profile & (~connected_profile);
                if (BT_SINK_SRV_PROFILE_NONE == req_profile) {
                    result = BT_CONNECTION_MANAGER_STATUS_LINK_EXIST;
                } else {
                    request->action_param.profile_connection_mask = req_profile;
                    result = BT_STATUS_SUCCESS;
                }
            } else if (bt_connection_manager_power_get_state() != BT_CM_POWER_STATE_ON) {
                result = BT_CONNECTION_MANAGER_STATUS_INVALID_STATUS;
            } else {
                result = BT_STATUS_SUCCESS;
            }
            break;

        default:
            break;
    }
    bt_cmgr_report_id("[BT_CM][I] permit result:0x%x, type %d", 2, result, request->type);
    return result;
}

static void bt_connection_manager_pending_request_print(bt_sink_srv_cm_node_t *req)
{
    if(NULL != req && NULL != req->parameters) {
        uint8_t *addr = req->parameters->action_param.address;
        uint8_t profile_mask = req->parameters->action_param.profile_connection_mask;

        bt_cmgr_report_id("[BT_CM][I] request pending address: %02X:%02X:%02X:%02X:%02X:%02X, profile: 0x%02X", 7,
            addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], profile_mask);
    }else {
        bt_cmgr_report_id("[BT_CM][E] req or req->parameters is null", 0);
    }
}

static bt_status_t bt_connection_manager_request_run(void)
{
    bt_status_t result;

    if (g_sink_srv_cm_context.request_pending != NULL) {
        result = BT_CONNECTION_MANAGER_STATUS_PENDING;

        bt_cmgr_report_id("[BT_CM][W] request run but there have a pending", 0);
        bt_connection_manager_pending_request_print(g_sink_srv_cm_context.request_pending);
    } else {
        bt_sink_srv_cm_node_t *req = bt_connection_manager_node_get_next();

        if (req && req->parameters) {
            result = bt_connection_manager_request_permit(req->parameters);

            if (req->parameters->attampts > 0) {
                req->parameters->attampts--;
            }

            if (BT_STATUS_SUCCESS == result) {
                g_sink_srv_cm_context.request_pending = req;

                if (req->parameters->delay_time > 0) {
                    bt_cmgr_report_id("[BT_CM][I] request delay:%d, type:%d", 2, req->parameters->delay_time, req->parameters->type);
                    bt_connection_manager_timer_start(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY,
                                            req->parameters->delay_time,
                                            bt_connection_manager_request_delay_execute,
                                            (void *)req);
                } else {
                    bt_connection_manager_request_execute(req);
                }
            } else {
                req->parameters->attampts = 0;
                bt_connection_manager_request_done(result, req);
            }
        } else {
            result = BT_CONNECTION_MANAGER_STATUS_NO_REQUEST;
        }
    }
    return result;
}

static void bt_connection_manager_request_done(bt_status_t result, bt_sink_srv_cm_node_t *req)
{
    if (req && req->parameters) {
        bt_cmgr_report_id("[BT_CM][I] request done, result:%d, pending:0x%x, attampts:%d", 3,
                           result, g_sink_srv_cm_context.request_pending, req->parameters->attampts);

        bt_cmgr_report_id("[BT_CM][I] request done clear pending", 0);
        bt_connection_manager_pending_request_print(g_sink_srv_cm_context.request_pending);
        /* stop delay timer */
        bt_connection_manager_timer_stop(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY);

        g_sink_srv_cm_context.request_pending = NULL;
        if (BT_CONNECTION_MANAGER_STATUS_USER_CANCEL == result) {
            if (req->parameters->callback) {
                req->parameters->callback(result, req->parameters);
            }
            bt_connection_manager_node_delete(req);
        } else if (BT_STATUS_SUCCESS != result && req->parameters->attampts > 0) {
            //req->parameters->delay_time += BT_SINK_SRV_REQUEST_DELAY_TIME_INCREASE;
            bt_connection_manager_node_insert_tail(&g_sink_srv_cm_context.request_list, req);
            bt_connection_manager_request_run();
        } else if (BT_CONNECTION_MANAGER_STATUS_NEED_RETRY == result) {
            req->parameters->delay_time = BT_SINK_SRV_REQUEST_DELAY_TIME;
            bt_connection_manager_node_insert_head(&g_sink_srv_cm_context.request_list, req);
            bt_connection_manager_request_run();
        } else {
            if (req->parameters->callback) {
                req->parameters->callback(result, req->parameters);
            }
            bt_connection_manager_node_delete(req);
            bt_status_t ret = bt_connection_manager_request_run();
            if (BT_CONNECTION_MANAGER_STATUS_NO_REQUEST == ret && BT_STATUS_SUCCESS != result) {
                //bt_connection_manager_open_visibility_feature();
            }
        }
    }
}

static bt_sink_srv_cm_node_t *bt_connection_manager_request_find(bt_sink_srv_cm_request_t *request)
{
    bt_sink_srv_cm_node_t *find_node = NULL;

    if (request) {
        bt_sink_srv_cm_node_t *cur_node = g_sink_srv_cm_context.request_list.next;

        while (cur_node != &g_sink_srv_cm_context.request_list) {
            if (cur_node->parameters &&
                    0 == bt_connection_manager_memcmp(&cur_node->parameters->action_param.address, &request->action_param.address, sizeof(bt_bd_addr_t))) {
                find_node = cur_node;
                break;
            }
            cur_node = cur_node->next;
        }
    }

    bt_cmgr_report_id("[BT_CM][I] request find:0x%x", 1, find_node);

    return find_node;
}

static bt_status_t bt_connection_manager_request_new(bt_sink_srv_cm_request_t *new_req)
{
    bt_status_t result = bt_connection_manager_request_permit(new_req);

    if (BT_STATUS_SUCCESS == result && NULL == bt_connection_manager_request_find(new_req)) {
        bt_sink_srv_cm_node_t *new_node = bt_connection_manager_node_new(new_req);

        if (NULL != new_node) {
            bt_connection_manager_node_insert_tail(&g_sink_srv_cm_context.request_list, new_node);
        } else {
            bt_cmgr_report_id("[BT_CM][E] Allocate request context fail !!! ", 0);
            return BT_CONNECTION_MANAGER_STATUS_FAIL;
        }

        result = bt_connection_manager_request_run();

        bt_cmgr_report_id("[BT_CM][I] request new, result:%d, pending:0x%x", 2, result, g_sink_srv_cm_context.request_pending);

        if (BT_CONNECTION_MANAGER_STATUS_PENDING == result) {
            new_node->parameters->delay_time = BT_SINK_SRV_REQUEST_DELAY_TIME;
            result = BT_STATUS_SUCCESS;
        }
    } else {
        result = BT_CONNECTION_MANAGER_STATUS_REQUEST_EXIST;
    }
    return result;
}

static bt_status_t bt_connection_manager_request_cancel(bt_sink_srv_cm_req_type_t type, bt_bd_addr_t *addr)
{
    bt_sink_srv_cm_node_t *cur_node = g_sink_srv_cm_context.request_list.next;
    bt_sink_srv_cm_node_t *del_node = NULL;

    bt_cmgr_report_id("[BT_CM][I] Cancel connection req type : %d ", 1, type);
    while (NULL != cur_node && cur_node != &g_sink_srv_cm_context.request_list) {
        if (NULL != cur_node->parameters &&
            (type == BT_SINK_SRV_CM_REQ_TYPE_NONE || type == cur_node->parameters->type) &&
            (false == bt_connection_manager_is_valid_address(addr) ||
            !bt_connection_manager_memcmp(addr, &cur_node->parameters->action_param.address, sizeof(bt_bd_addr_t)))) {
            del_node = cur_node;
        }
        cur_node = cur_node->next;
        if (del_node != NULL) {
            bt_connection_manager_node_remove(del_node);
            bt_connection_manager_node_delete(del_node);
            del_node = NULL;
        }
    }
    cur_node = g_sink_srv_cm_context.request_pending;
    if (NULL != cur_node && NULL != cur_node->parameters &&
        (type == BT_SINK_SRV_CM_REQ_TYPE_NONE || type == cur_node->parameters->type) &&
        (false == bt_connection_manager_is_valid_address(addr) ||
         !bt_connection_manager_memcmp(addr, &cur_node->parameters->action_param.address, sizeof(bt_bd_addr_t)))) {
        bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR,
            (void *)&(cur_node->parameters->action_param.address));
        if (bt_connection_manager_timer_is_exist(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY)) {
            bt_connection_manager_timer_stop(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY);
        }
        if (NULL != device_p && BT_SINK_SRV_CM_STATE_CONNECTING == device_p->link_state
            && BT_STATUS_SUCCESS == bt_gap_cancel_connection((void *)&(cur_node->parameters->action_param.address))) {
            bt_cmgr_report_id("[BT_CM][I] Cancel connection .... ", 0);
            return BT_CONNECTION_MANAGER_STATUS_PENDING;
        }
        bt_connection_manager_request_done(BT_CONNECTION_MANAGER_STATUS_USER_CANCEL, cur_node);
    }
    return BT_STATUS_SUCCESS;
}

static uint8_t bt_connection_manager_request_find_node_num_by_type(bt_sink_srv_cm_req_type_t type)
{
    bt_sink_srv_cm_node_t *cur_node = g_sink_srv_cm_context.request_list.next;
    uint8_t node_num = 0;
    while (cur_node != &g_sink_srv_cm_context.request_list) {
        if (cur_node->parameters && type == cur_node->parameters->type) {
            node_num ++;
        }
        cur_node = cur_node->next;
    }
    return node_num;
}

static bool bt_connection_manager_open_visibility_feature(void)
{
    /* check visibility feature*/
    bool is_visibility = false;
    const bt_sink_srv_feature_visibility_params_t * visibility_params = bt_sink_srv_get_visibility_params();
    bt_cmgr_report_id("[BT_CM][I] visiable_flag %d", 1, visibility_params->power_on_be_visible_once);

    if (visibility_params->power_on_be_visible_once || true == g_bt_cm_power_on_open_discoverable) {
        g_bt_cm_power_on_open_discoverable = false;
        bt_connection_manager_discoverable_action_handle(true);
        is_visibility = true;
    } else {
        if (!bt_connection_manager_get_dut_mode()) {
            bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
        }
    }
    return is_visibility;
}

static bt_sink_srv_cm_remote_device_t *bt_connection_manager_alloc_device(bt_bd_addr_t *addr)
{
    if (!bt_connection_manager_is_valid_address(addr)) {
        bt_cmgr_report_id("[BT_CM][E] Invalid address. ", 0);
        return NULL;
    }

    bt_sink_srv_cm_remote_device_t *device = NULL;
    for (uint8_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        if (0 == bt_connection_manager_memcmp((void *)addr, (void *)&g_sink_srv_cm_context.devices[i].addr, sizeof(bt_bd_addr_t))) {
            device = &g_sink_srv_cm_context.devices[i];
            break;
        } else if (!bt_connection_manager_is_valid_address(&g_sink_srv_cm_context.devices[i].addr)) {
            device = &g_sink_srv_cm_context.devices[i];
        }
    }

    if (NULL != device) {
        bt_connection_manager_memcpy((void *)&device->addr, (void *) addr, sizeof(bt_bd_addr_t));
    }
    return device;
}

static void bt_connection_manager_free_device(bt_sink_srv_cm_remote_device_t *device_p)
{
    if (NULL != device_p) {
        bt_connection_manager_memset((void *)&device_p->addr, 0, sizeof(bt_bd_addr_t));
        device_p->handle = 0;
        device_p->link_state = BT_SINK_SRV_CM_STATE_DISCONNECTED;
        device_p->set_aws_state = BT_SINK_SRV_CM_SET_AWS_ENABLE_SUCCESS;
        device_p->aws_state = BT_AWS_MCE_AGENT_STATE_NONE;
        device_p->sniff_state = BT_GAP_LINK_SNIFF_TYPE_ACTIVE;
        device_p->flag = 0;
        device_p->profile_already_exist_mask = 0;
    }
}

extern void bt_avm_set_wide_band_scan_flag(bool enable);
static void bt_connection_manager_connect_internal(bt_sink_srv_profile_connection_action_t *action_param)
{
    bt_connection_manager_assert(action_param && "Null param");
    bt_cmgr_report_id("[BT_CM][I] Request Connect Address:%02x:%02x:%02x:%02x:%02x:%02x, mask:0x%x", 7,
                           action_param->address[5], action_param->address[4], action_param->address[3],
                           action_param->address[2], action_param->address[1], action_param->address[0], action_param->profile_connection_mask);
    if (!bt_connection_manager_is_valid_address(&action_param->address)) {
        return;
    }
    bt_connection_manager_mutex_lock();
    bt_sink_srv_cm_remote_device_t *device_p = bt_connection_manager_alloc_device(&action_param->address);
    if (device_p == NULL) {
        bt_cmgr_report_id("[BT_CM][E] Alloc device failed.", 0);
    } else {
        if (BT_SINK_SRV_CM_STATE_DISCONNECTED == device_p->link_state) {
        #ifdef MTK_AWS_MCE_ENABLE
        #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
            if(memcmp(&(action_param->address), bt_connection_manager_device_local_info_get_local_address(), sizeof(bt_bd_addr_t)) == 0) {
                bt_cmgr_report_id("[BT_CM][I] create aws special link", 0);
            } else {
                bt_connection_manager_state_machine_ext_action_handler(BT_SINK_SRV_ACTION_CONNECT, NULL);
            }
        #endif
            bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
            if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
                if (false == bt_connection_manager_get_dut_mode()) {
                    bt_avm_set_wide_band_scan_flag(true);
                }
            }
        #endif
        }
        if (BT_SINK_SRV_CM_STATE_DISCONNECTED == device_p->link_state || BT_SINK_SRV_CM_STATE_CONNECTED <= device_p->link_state) {
            if (BT_SINK_SRV_CM_STATE_DISCONNECTED == device_p->link_state) {
                BT_SINK_SRV_CM_STATE_CHANGE(device_p, BT_SINK_SRV_CM_STATE_CONNECTING);
            }
            action_param->profile_connection_mask = action_param->profile_connection_mask & ~(device_p->profile_connection_mask);
            bt_cmgr_report_id("[BT_CM][I] request connection profile mask:0x%x", 1, action_param->profile_connection_mask);
            bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PROFILE_CONNECT, (void *)action_param);
        } else {
            bt_cmgr_report_id("[BT_CM][E] Device state error:%d", 1, device_p->link_state);
        }
    }
    bt_connection_manager_mutex_unlock();
    return;
}

void bt_connection_manager_allow_power_on_reconnect(bt_connection_manager_power_on_reconnect_allow_t allow)
{
    bt_cmgr_report_id("[BT_CM][W] Role re-setup set power reconnection flag %d", 1, allow);
    g_bt_cm_power_reconnect_flag = allow;
}

void bt_connection_manager_connect_ext()
{
    bt_cmgr_report_id("[BT_CM][W] Role re-setup start power reconnection", 0);
    if (g_sink_srv_cm_context.scan_mode != BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY &&
        g_sink_srv_cm_context.scan_mode != BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE) {
        bt_connection_manager_power_on_reconnection();
    }
}

static bt_status_t bt_connection_manager_connect_action_handle(bt_bd_addr_t *addr)
{
    bt_sink_srv_cm_remote_device_t *device_p = NULL;
    bt_sink_srv_cm_request_t request;
    bt_sink_srv_profile_connection_action_t connect_action = {{0}};
    if (NULL != addr) {
        bt_connection_manager_memcpy((void *)&connect_action.address, addr, sizeof(bt_bd_addr_t));
    } else {
        if (BT_AWS_MCE_ROLE_AGENT == bt_connection_manager_device_local_info_get_aws_role()
            || BT_AWS_MCE_ROLE_NONE == bt_connection_manager_device_local_info_get_aws_role()) {
            bt_bd_addr_t* conn_addr = NULL;
            for (uint8_t i = 0; i < BT_SINK_SRV_CM_MAX_TRUSTED_DEV; i++) {
                conn_addr = bt_device_manager_remote_get_dev_by_seq_num(i + 1);
                if (NULL != conn_addr && true == bt_device_manager_is_paired((void*)conn_addr)) {
                    bt_cmgr_report_id("[BT_CM][I] App Connect addr:%02x:%02x:%02x:%02x:%02x:%02x", 6,
                                        (*conn_addr)[0],(*conn_addr)[1], (*conn_addr)[2],
                                        (*conn_addr)[3],(*conn_addr)[4], (*conn_addr)[5]);
                    bt_connection_manager_memcpy((void *)&connect_action.address, (void *)conn_addr, sizeof(bt_bd_addr_t));
                    break;
                }
            }
        } else {
            bt_connection_manager_memcpy((void *)&connect_action.address, (void *)&g_sink_srv_cm_last_connected, sizeof(bt_bd_addr_t));
        }
    }
    device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)connect_action.address);
    if (device_p != NULL) {
        bt_cmgr_report_id("[BT_CM][W]connection is pending", 0);
        return BT_CONNECTION_MANAGER_STATUS_REQUEST_EXIST;
    } else {
        if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & bt_connection_manager_device_local_info_get_aws_role()) {
            connect_action.profile_connection_mask = BT_SINK_SRV_PROFILE_AWS;
        } else {
            connect_action.profile_connection_mask = (BT_SINK_SRV_PROFILE_HFP | BT_SINK_SRV_PROFILE_A2DP_SINK);
        }
    }
    bt_connection_manager_memset((void *)&request, 0, sizeof(bt_sink_srv_cm_request_t));
    request.type = BT_SINK_SRV_CM_REQ_TYPE_NORMAL;
    request.attampts = 1;
    bt_connection_manager_memcpy((void *)&request.action_param, (void *)&connect_action, sizeof(connect_action));
    bt_connection_manager_request_new(&request);
    return BT_STATUS_SUCCESS;
}

bt_bd_addr_t *bt_sink_srv_cm_get_aws_connected_device(void)
{
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    bt_bd_addr_t* local_addr = bt_connection_manager_device_local_info_get_local_address();
    for (uint8_t i= 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
        bt_sink_srv_cm_remote_device_t *device_p = &(g_sink_srv_cm_context.devices[i]);
        if(BT_AWS_MCE_ROLE_PARTNER == aws_role  &&
            BT_SINK_SRV_CM_STATE_CONNECTED <= device_p->link_state &&
            bt_connection_manager_memcmp(local_addr, &(device_p->addr), sizeof(bt_bd_addr_t))) {
            return &(device_p->addr);
        } else if (BT_AWS_MCE_ROLE_AGENT == aws_role  &&
            BT_SINK_SRV_CM_STATE_CONNECTED <= device_p->link_state &&
            BT_AWS_MCE_AGENT_STATE_ATTACHED == device_p->aws_state) {
            bt_cmgr_report_id("[BT_CM][I] Find aws conncted device %p", 1, device_p);
            return &(device_p->addr);
        }
    }

    bt_cmgr_report_id("[BT_CM][I] Not find aws connected device ", 0);
    return NULL;
}

bt_sink_srv_cm_remote_device_t *bt_sink_srv_cm_find_device(bt_sink_srv_cm_find_by_type_t type, void *param)
{
    bt_sink_srv_cm_remote_device_t *dev_p = NULL;
    if(NULL == param) {
        bt_cmgr_report_id("[BT_CM][E] param is null", 0);
        return dev_p;
    }
    for (uint8_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        if (BT_SINK_SRV_CM_FIND_BY_HANDLE == type && g_sink_srv_cm_context.devices[i].handle == *((bt_gap_connection_handle_t *)param)) {
            dev_p = &g_sink_srv_cm_context.devices[i];
            break;
        } else if (BT_SINK_SRV_CM_FIND_BY_ADDR == type &&
                   bt_connection_manager_memcmp(param, &g_sink_srv_cm_context.devices[i].addr, sizeof(bt_bd_addr_t)) == 0) {
            dev_p = &g_sink_srv_cm_context.devices[i];
            break;
        } else if (BT_SINK_SRV_CM_FIND_BY_SET_AWS_STATE_TYPE == type &&
                   g_sink_srv_cm_context.devices[i].set_aws_state == *(bt_sink_srv_cm_set_aws_enable_t *)param) {
            dev_p = &g_sink_srv_cm_context.devices[i];
            break;
        }
    }

    if(dev_p) {
        bt_cmgr_report_id("[BT_CM][I] Handle:0x%x", 1, dev_p->handle);
    } else {
        bt_cmgr_report_id("[BT_CM][I] device not find", 0);
    }
    return dev_p;
}

bt_sink_srv_cm_remote_device_t *bt_sink_srv_cm_get_special_aws_device()
{
     bt_bd_addr_t* local_addr = bt_connection_manager_device_local_info_get_local_address();
     return bt_sink_srv_cm_find_device(
                           BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)local_addr);
}

bt_sink_srv_cm_remote_device_t *bt_sink_srv_cm_get_normal_aws_device()
{
    bt_bd_addr_t* local_addr = bt_connection_manager_device_local_info_get_local_address();
    bt_sink_srv_cm_remote_device_t *device_p = NULL;

    for (uint8_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        device_p = &g_sink_srv_cm_context.devices[i];
        if(bt_connection_manager_is_valid_address(&(device_p->addr)) &&
            bt_connection_manager_memcmp(local_addr, &(device_p->addr), sizeof(bt_bd_addr_t)) &&
            device_p->link_state >= BT_SINK_SRV_CM_STATE_CONNECTED) {
            return device_p;
        }
    }
    bt_cmgr_report_id("[BT_CM][I] Can not get normal aws device !!!", 0);
    return NULL;
}

static bt_status_t bt_connection_manager_disconnect_by_address(bt_bd_addr_t* addr)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)addr);
    bt_cmgr_report_id("[BT_CM][I] Disconnect, device_p:0x%x", 1, device_p);
    if (NULL == device_p || BT_SINK_SRV_CM_STATE_DISCONNECTED == device_p->link_state) {
        return ret;
    } else if (BT_SINK_SRV_CM_STATE_CONNECTING == device_p->link_state) {
        bt_cmgr_report_id("[BT_CM][I] Disconnect, cancel connecting device! ... ", 0);
    #ifdef MTK_AWS_MCE_ENABLE
        if (BT_AWS_MCE_ROLE_PARTNER == bt_connection_manager_device_local_info_get_aws_role()) {
            bt_avm_set_wide_band_scan_flag(false);
        }
    #endif
        return bt_connection_manager_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_NONE, addr);
    } else if (BT_SINK_SRV_CM_STATE_CONNECTED <= device_p->link_state) {
        BT_SINK_SRV_CM_STATE_CHANGE(device_p, BT_SINK_SRV_CM_STATE_DISCONNECTING);
    }
    ret = BT_CONNECTION_MANAGER_STATUS_PENDING;
    bt_sink_srv_profile_connection_action_t action_param = {{0}};
    action_param.profile_connection_mask = device_p->profile_connection_mask;
    bt_cmgr_report_id("[BT_CM][I] Disconnect,device_p->profile_connection_mask:0x%x", 1, device_p->profile_connection_mask);
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    bt_bd_addr_t* local_addr = bt_connection_manager_device_local_info_get_local_address();
    if (!bt_connection_manager_memcmp(local_addr, addr, sizeof(bt_bd_addr_t))) {
        if (BT_AWS_MCE_ROLE_AGENT == aws_role && (BT_AWS_MCE_AGENT_STATE_CONNECTABLE == device_p->aws_state ||
                BT_AWS_MCE_AGENT_STATE_ATTACHED == device_p->aws_state)) {
            /* If to do power off or reset then we need switch role t0 0x41 on special link before disconnect. */
            bt_connection_manager_enable_aws_discoverable(false);
            return ret;
        } else {
            return BT_STATUS_SUCCESS;
        }
    } else if (BT_AWS_MCE_ROLE_AGENT == aws_role) {
        /* Not to discsonnect aws profile with agent role. */
        bt_cmgr_report_id("[BT_CM][I] Unmask aws profile when do disconnect. ", 0);
        action_param.profile_connection_mask &= ~(BT_SINK_SRV_PROFILE_AWS);
    }
#endif
    if (0 == action_param.profile_connection_mask) {
        bt_status_t status = bt_gap_disconnect(device_p->handle);
        if (BT_STATUS_SUCCESS != status) {
            bt_cmgr_report_id("[BT_CM][E] Disconnect device:%p, fail status : 0x%X", 2, device_p, status);
            ret = BT_STATUS_SUCCESS;
        }
    } else {
        /*
        if (BT_CM_POWER_STATE_OFF_PENDING == bt_connection_manager_power_get_state()) {
            action_param.reason = BT_HCI_STATUS_REMOTE_TERMINATED_CONNECTION_DUE_TO_POWER_OFF;
        } else {
            action_param.reason = BT_HCI_STATUS_REMOTE_USER_TERMINATED_CONNECTION;
        }*/
        bt_connection_manager_memcpy(action_param.address, (void *)&device_p->addr, sizeof(bt_bd_addr_t));
        bt_cmgr_report_id("[BT_CM][I] Disconnect, device profile_type:0x%x", 1, action_param.profile_connection_mask);
        bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PROFILE_DISCONNECT, (void *)&action_param);
    }
    return ret;
}

bt_status_t bt_connection_manager_disconnect_all(bool force)
{
    bt_sink_srv_cm_remote_device_t *device_p = NULL;
    bt_sink_srv_cm_remote_device_t *saws_device = bt_sink_srv_cm_get_special_aws_device();
    bt_cmgr_report_id("[BT_CM][I] To do disconnect before bt power off force %d !!!", 1, force);
    bt_cm_power_state_t power_state = bt_connection_manager_power_get_state();
    g_bt_cm_disconnect_all = true;
    bt_connection_manager_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_NONE, NULL);
    for (uint8_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER + 1; ++i) {
        bt_status_t status;
        /* To disconnect special aws link at the end. */
        if (BT_SINK_SRV_CM_MAX_DEVICE_NUMBER != i) {
            device_p = &g_sink_srv_cm_context.devices[i];
            if (saws_device == device_p) {
                continue;
            }
        } else if(BT_CM_POWER_STATE_OFF_PENDING == power_state) {
            device_p = saws_device;
        } else {
            break;
        }
        if (NULL != device_p && bt_connection_manager_is_valid_address(&(device_p->addr))) {
            if (false == force && BT_STATUS_SUCCESS !=
                (status = bt_connection_manager_disconnect_by_address(&(device_p->addr)))) {
                bt_cmgr_report_id("[BT_CM][I] Disconnect link status : 0x%08X!!!", 1, status);
                return status;
            } else if (true == force && (device_p != saws_device) && BT_STATUS_SUCCESS ==
                (status = bt_gap_disconnect(device_p->handle))) {
                return BT_CONNECTION_MANAGER_STATUS_PENDING;
            }
        }
    }
    g_bt_cm_disconnect_all = false;
    return BT_STATUS_SUCCESS;
}

static void bt_connection_manager_disconnect_action_handle(bt_bd_addr_t *param)
{
    bt_connection_manager_mutex_lock();
    if (NULL != param) {
        bt_connection_manager_disconnect_by_address(param);
    } else {
        bt_connection_manager_disconnect_all(false);
    }
    bt_connection_manager_mutex_unlock();
}

static bt_sink_srv_cm_flags_t bt_connection_manager_get_flags(void)
{
    bt_cmgr_report_id("[BT_CM][I] Get flags:0x%x", 1, g_sink_srv_cm_context.flags);
    return g_sink_srv_cm_context.flags;
}

static void bt_connection_manager_set_flags(bt_sink_srv_cm_flags_t flags)
{
    g_sink_srv_cm_context.flags |= flags;
    bt_cmgr_report_id("[BT_CM][I] Sset flags:0x%x, new flags:0x%x", 2, flags, g_sink_srv_cm_context.flags);
    return;
}

static void bt_connection_manager_reset_flags(bt_sink_srv_cm_flags_t flags)
{
    g_sink_srv_cm_context.flags &=~ flags;
    bt_cmgr_report_id("[BT_CM][I] Reset flags:0x%x, new flags:0x%x", 2, flags, g_sink_srv_cm_context.flags);
    return;
}

static void bt_connection_manager_notify_timeout_handler(void *params)
{
    bt_sink_srv_cm_remote_device_t *device_p = (bt_sink_srv_cm_remote_device_t *)params;
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    bt_connection_manager_timer_stop(BT_SINK_SRV_TIMER_CM_PROFILE_NOTIFY);
    if (NULL != device_p) {
        bt_sink_srv_connection_information_t *event;
        bt_sink_srv_cm_node_t *pending_req = g_sink_srv_cm_context.request_pending;
        bt_cmgr_report_id("[BT_CM][I] Profile notify timeout", 0);
        bt_cmgr_report_id("[BT_CM][I] Address:%02x:%02x:%02x:%02x:%02x:%02x", 6,
               device_p->addr[5], device_p->addr[4], device_p->addr[3],
               device_p->addr[2], device_p->addr[1], device_p->addr[0]);
        if (!((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) &&
            device_p != bt_sink_srv_cm_get_special_aws_device()) {
            bt_device_manager_remote_set_seq_num(device_p->addr, 1);
        }

        if (NULL != pending_req && 0 == bt_connection_manager_memcmp(&device_p->addr, &pending_req->parameters->action_param.address,sizeof(bt_bd_addr_t))) {
            if (!(pending_req->parameters->action_param.profile_connection_mask & (~device_p->profile_connection_mask))) {
                bt_connection_manager_request_done(BT_STATUS_SUCCESS, pending_req);
            } else {
                bt_connection_manager_request_done(BT_CONNECTION_MANAGER_STATUS_NEED_RETRY, pending_req);
            }
        }

        if (NULL != (event = bt_connection_manager_memory_alloc(sizeof(*event)))) {
            bt_connection_manager_memcpy(&event->bt_addr, &device_p->addr, sizeof(device_p->addr));
            event->profile_type = device_p->profile_connection_mask;
            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE, event, sizeof(*event));
        #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
            bt_connection_manager_state_machine_ext_sink_event_callback(BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE, event, sizeof(*event));
        #endif
            bt_connection_manager_memory_free(event);
        }
    }
}

static void bt_connection_manager_profile_connection_state_notify(bt_bd_addr_t* address, bt_sink_srv_profile_type_t profile,
    bt_sink_srv_profile_connection_state_t state, bt_status_t reason)
{
    //bt_cmgr_report_id("[BT_CM] profile type:0x%x, connection state:0x%x", 2, profile, state);
    bt_sink_srv_profile_connection_state_update_t *event = NULL;
    if (NULL != (event = bt_connection_manager_memory_alloc(sizeof(*event)))) {
        bt_connection_manager_memcpy(&event->address, address, sizeof(*address));
        event->profile = profile;
        event->state = state;
        event->reason = reason;
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_PROFILE_CONNECTION_UPDATE, event, sizeof(*event));
    #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
        bt_connection_manager_state_machine_ext_sink_event_callback(BT_SINK_SRV_EVENT_PROFILE_CONNECTION_UPDATE, event, sizeof(*event));
    #endif
        bt_connection_manager_memory_free(event);
    }
    return;
}

void bt_sink_srv_cm_esco_state_notify(bt_bd_addr_t *addr, bt_sink_srv_sco_connection_state_t state)
{
    bt_cmgr_report_id("[BT_CM] eSCO notify:0x%08x, connection state:0x%x", 2, addr, state);
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)addr);
    bt_connection_manager_assert(device_p);

    switch (state) {
    case BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED: {
        bt_cmgr_report_id("[BT_CM] device_p->flag:0x%08x", 1, device_p->flag);
        device_p->flag |= BT_SINK_SRV_CM_REMOTE_FLAG_ESCO_CONNECTED;
    #ifdef MTK_AWS_MCE_ENABLE
        if (!(device_p->local_role & BT_ROLE_SLAVE))
    #else
        if (device_p->flag & BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING)
    #endif
        {
            if (bt_sink_srv_role_switch_retry_times && (true == g_bt_cm_need_switch_slave)) {
                device_p->flag |= BT_SINK_SRV_CM_REMOTE_FLAG_DISCON_ESCO;
                bt_sink_srv_send_action(BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH, NULL);
            }
        }
    }
    break;

    case  BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED: {
        device_p->flag &= (~BT_SINK_SRV_CM_REMOTE_FLAG_ESCO_CONNECTED);
    }
    break;

    default:
        bt_cmgr_report_id("[BT_CM] eSCO notify:0x%08x, connection state:0x%x", 2, addr, state);
    break;
    }
}

static void bt_connection_manager_profile_already_exist_notify_timeout_handler(void *params)
{
    bt_sink_srv_cm_remote_device_t *device_p = (bt_sink_srv_cm_remote_device_t *)params;
    if (NULL != device_p) {
        bt_cmgr_report_id("[BT_CM][I] acl already exist notify profile : 0x%x", 1, device_p->profile_already_exist_mask);
        bt_connection_manager_profile_connection_state_notify(&(device_p->addr),
            device_p->profile_already_exist_mask, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED,
            BT_HCI_STATUS_ACL_CONNECTION_ALREADY_EXISTS);
        device_p->profile_already_exist_mask = 0;
    }
}

static void bt_connection_manager_disconnect_timeout_handler(void *params)
{
    bt_sink_srv_cm_remote_device_t *device_p = (bt_sink_srv_cm_remote_device_t *)params;
    bt_cmgr_report_id("[BT_CM][I] Disconnect timer timeout", 0);
    bt_connection_manager_timer_stop(BT_SINK_SRV_CM_DISCONNECT_TIMER_ID);
    if (NULL != device_p) {
        bt_sink_srv_profile_connection_action_t action_param = {{0}};
        action_param.profile_connection_mask =  device_p->profile_connection_mask;
        bt_connection_manager_memcpy(action_param.address, (void *)&device_p->addr, sizeof(bt_bd_addr_t));
        bt_cmgr_report_id("[BT_CM][I] Disconnect, device profile_type:0x%x", 1, action_param.profile_connection_mask);
        bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PROFILE_DISCONNECT, (void *)&action_param);
    }
}

static void bt_connection_manager_notify_profile_status_change(bt_sink_srv_cm_remote_device_t *device_p, bt_sink_srv_profile_type_t request_profile,bt_sink_srv_profile_connection_state_t state)
{
    if ((device_p->profile_connection_mask & request_profile) == request_profile && state == BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED) {
        /*update device info if the all profile were connected with remote.*/
        bt_cmgr_report_id("[BT_CM][I] All profiles notify, 0x%x", 1, device_p->profile_connection_mask);
        bt_connection_manager_notify_timeout_handler(device_p);
    } else if (BT_SINK_SRV_PROFILE_NONE == device_p->profile_connection_mask ||
            device_p->profile_connection_mask == BT_SINK_SRV_PROFILE_AWS) {
        bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
        bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
        if (!(device_p->profile_connection_mask == BT_SINK_SRV_PROFILE_AWS && BT_AWS_MCE_ROLE_AGENT == aws_role &&
            bt_connection_manager_memcmp((const void *)device_p->addr, (const void *)local_addr, sizeof(bt_bd_addr_t)))) {
            bt_connection_manager_notify_timeout_handler(device_p);
        }
        if ((BT_SINK_SRV_CM_STATE_DISCONNECTING == device_p->link_state) &&
            (device_p != bt_sink_srv_cm_get_special_aws_device())) {
            bt_status_t status = bt_gap_disconnect(device_p->handle);
            if (BT_STATUS_SUCCESS != status) {
                bt_cmgr_report_id("[BT_CM][E] Disconnect device:%p, fail status : 0x%X", 2, device_p, status);
            }
        }
    } else {
        bt_connection_manager_timer_start(BT_SINK_SRV_TIMER_CM_PROFILE_NOTIFY,
                                BT_SINK_SRV_CM_PROFILE_NOTIFY,
                                bt_connection_manager_notify_timeout_handler,
                                device_p);
    }
}

void bt_sink_srv_cm_profile_status_notify(bt_bd_addr_t *addr, bt_sink_srv_profile_type_t profile, bt_sink_srv_profile_connection_state_t state, bt_status_t reason)
{
    bt_sink_srv_cm_remote_device_t *device_p = NULL;
   // bt_cmgr_report_id("[BT_CM][I] profile notify device:%02X-%02X-%02X-%02X-%02X-%02X", 6,
   //         (*addr)[0], (*addr)[1], (*addr)[2], (*addr)[3], (*addr)[4], (*addr)[5]);
    bt_cmgr_report_id("[BT_CM][I] profile notify, profile:0x%x, state:%d reason:0x%08X", 3, profile, state, reason);
    device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)addr);
    bt_cmgr_report_id("[BT_CM][I] profile notify, device_p:0x%x", 1, device_p);
    if (NULL == device_p) {
        return;
    }
    if ((BT_HCI_STATUS_ACL_CONNECTION_ALREADY_EXISTS == reason || BT_HCI_STATUS_VENDOR_REMOTE_CONNECTION_EXIST == reason) && BT_SINK_SRV_PROFILE_NONE != profile) {
        /* If Two same bd address device connect a smartphone, one side will report this status. */
        device_p->profile_already_exist_mask |= profile;
        bt_connection_manager_timer_start(BT_SINK_SRV_TIMER_CM_PROFILE_ALREADY_EXIST_NOTIFY,
            BT_SINK_SRV_CM_PROFILE_ALREADY_EXIST_TIMEOUT,
             bt_connection_manager_profile_already_exist_notify_timeout_handler, device_p);
    } else if (BT_SINK_SRV_PROFILE_NONE != profile) {
        bt_connection_manager_profile_connection_state_notify(addr, profile, state, reason);
    }
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    bt_sink_srv_profile_type_t previous = device_p->profile_connection_mask;
    bt_sink_srv_cm_node_t *pending_req = g_sink_srv_cm_context.request_pending;
    bt_sink_srv_profile_type_t request_profile = BT_SINK_SRV_PROFILE_NONE;
    if (NULL != pending_req && 0 == bt_connection_manager_memcmp(&device_p->addr, &pending_req->parameters->action_param.address,sizeof(bt_bd_addr_t))) {
        request_profile = pending_req->parameters->action_param.profile_connection_mask;
    }

    if (state == BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED) {
        device_p->profile_connection_mask |= profile;
        if (device_p->link_state == BT_SINK_SRV_CM_STATE_DISCONNECTING && BT_SINK_SRV_PROFILE_NONE != profile) {
            bt_connection_manager_timer_start(BT_SINK_SRV_CM_DISCONNECT_TIMER_ID, 1,
                bt_connection_manager_disconnect_timeout_handler, device_p);
            return;
        }
        if (BT_AWS_MCE_ROLE_NONE == aws_role && BT_SINK_SRV_PROFILE_NONE != profile) {
            bt_connection_manager_set_role(addr, BT_ROLE_SLAVE, false);
#ifdef MTK_AWS_MCE_ENABLE
        } else if (BT_AWS_MCE_ROLE_AGENT == aws_role) {
            bt_bd_addr_t* local_addr = bt_connection_manager_device_local_info_get_local_address();
            bt_cmgr_report_id("[BT_CM][I] profile notify, local role:0x%x, conn_mask:0x%x", 2, device_p->local_role, device_p->profile_connection_mask);
            bt_connection_manager_enable_aws_discoverable(true);
            if (BT_SINK_SRV_PROFILE_AWS == profile && (BT_SINK_SRV_FLAG_AWS_POWERING_ON & bt_connection_manager_get_flags()) &&
                !bt_connection_manager_memcmp((const void *)device_p->addr, (const void *)local_addr, sizeof(bt_bd_addr_t))) {
                /* Power on reconnect to sp after special link created.*/
                bt_cmgr_report_id("[BT_CM] Connect to sp after saws created when power on ...", 0);
                bt_connection_manager_reset_flags(BT_SINK_SRV_FLAG_AWS_POWERING_ON);
                /* agent need wait for saws link connected */
                bt_connection_manager_event_callback(BT_CONNECTION_MANAGER_EVENT_INIT_IND, NULL, 0);
                bt_connection_manager_power_on_action_handler();
            }
        } else if (((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) && (BT_SINK_SRV_PROFILE_NONE != profile)) {
            bt_cmgr_report_id("[BT_CM][I] partner role.", 0);
            bt_connection_manager_aws_sync_connnection_confirm();
            bt_connection_manager_partner_sync_address_to_agent();
#endif
        } else if (BT_SINK_SRV_PROFILE_NONE != profile) {
            bt_cmgr_report_id("[BT_CM][E] Unknown role:%x", 1, aws_role);
            bt_connection_manager_assert(0);
        }
    } else if (state == BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED) {
        if (BT_STATUS_UNSUPPORTED == reason) {
            if (NULL != pending_req && 0 == bt_connection_manager_memcmp(&device_p->addr, &pending_req->parameters->action_param.address,sizeof(bt_bd_addr_t))) {
                pending_req->parameters->action_param.profile_connection_mask &= (~profile);
                request_profile = pending_req->parameters->action_param.profile_connection_mask;
            }
        }
        device_p->profile_connection_mask &= (~profile);
        if (device_p->link_state == BT_SINK_SRV_CM_STATE_CONNECTING ||
            device_p->link_state == BT_SINK_SRV_CM_STATE_DISCONNECTED) {
            if (NULL != g_sink_srv_cm_context.request_pending
                    && !bt_connection_manager_memcmp(addr,
                    &g_sink_srv_cm_context.request_pending->parameters->action_param.address, sizeof(bt_bd_addr_t))) {
                bt_connection_manager_free_device(device_p);
                if (BT_STATUS_OUT_OF_MEMORY == reason) {
                    g_sink_srv_cm_context.request_pending->parameters->action_param.profile_connection_mask &= (~profile);
                    bt_cmgr_report_id("[BT_CM][I] connect profile new mask:%x", 1, g_sink_srv_cm_context.request_pending->parameters->action_param.profile_connection_mask);
                }
                bt_connection_manager_request_done(BT_CONNECTION_MANAGER_STATUS_FAIL, g_sink_srv_cm_context.request_pending);
            }
        }
        /* special patch: notify app connection fail due to connect fail not notify ACL link disconnect ind */
        if (NULL == bt_sink_srv_cm_get_normal_aws_device()) {
            bt_connection_manager_acl_link_ind_t event;
            event.is_connected = false;
            event.reason = reason;
            bt_connection_manager_memcpy(&event.address, addr, sizeof(bt_bd_addr_t));
            bt_connection_manager_event_callback(BT_CONNECTION_MANAGER_EVENT_ACL_LINK_STATE_IND, (void *)(&event), sizeof(event));
        }
    } else {
        bt_cmgr_report_id("[BT_CM][W] Not support now", 0);
        return;
    }
    if (device_p->profile_connection_mask != previous) {
        bt_cmgr_report_id("[BT_CM][I] Connection mask: 0x%x, previous: 0x%x", 2, device_p->profile_connection_mask, previous);
        bt_connection_manager_notify_profile_status_change(device_p, request_profile,state);
    }
}

extern uint32_t g_remote_cod;
static void bt_connection_manager_link_connected(bt_bd_addr_t *dev_addr, bt_gap_connection_handle_t handle, bt_gap_link_status_t link_status)
{
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    bt_sink_srv_cm_remote_device_t *device_p = bt_connection_manager_alloc_device((bt_bd_addr_t *)dev_addr);
    bt_cmgr_report_id("[BT_CM][I] Connected gap handle:0x%x", 1, handle);

    if (NULL == device_p || BT_CM_POWER_STATE_ON != bt_connection_manager_power_get_state()) {
        bt_cmgr_report_id("[BT_CM][E] No connection available", 0);
        bt_gap_disconnect(handle);
    } else if (BT_SINK_SRV_CM_STATE_CONNECTED > device_p->link_state && BT_GAP_LINK_STATUS_CONNECTED_0 <= link_status) {
        bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
        bt_sink_srv_cm_remote_device_t *other_link = bt_sink_srv_cm_get_normal_aws_device();

        BT_SINK_SRV_CM_STATE_CHANGE(device_p, BT_SINK_SRV_CM_STATE_CONNECTED);
        bt_connection_manager_timer_stop(BT_SINK_SRV_TIMER_CM_PROFILE_ALREADY_EXIST_NOTIFY);
        if (bt_connection_manager_memcmp(&(device_p->addr), local_addr, sizeof(bt_bd_addr_t))) {
            if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
                bt_cmgr_report_id("[BT_CM][I] agent attached !!! ", 0);
            #ifdef MTK_AWS_MCE_ENABLE
            #ifndef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
                bt_device_manager_aws_local_info_update();
            #endif
            #endif
            } else {
                bt_cmgr_report_id("[BT_CM][I] SP connected !!! ", 0);
            }
        } else {
            bt_cmgr_report_id("[BT_CM][I] Sepcial link connected !!! ", 0);
        }
        if(!((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) ||
            bt_connection_manager_memcmp(&(device_p->addr), local_addr, sizeof(bt_bd_addr_t))) {
            /* Partner also have a special link and it produce after normal aws link, but we can't connect it when CLD cmd. */
            bt_connection_manager_memcpy(&g_sink_srv_cm_last_connected, dev_addr, sizeof(bt_bd_addr_t));
        }

        device_p->handle = handle;
        g_sink_srv_cm_context.connected_dev_num++;

        if (true == g_bt_cm_disconnect_all) {
        #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
            if ((1 == bt_connection_manager_state_machine_ext_is_in_role_resetup()) &&
                (BT_STATUS_SUCCESS == bt_connection_manager_power_cancel_reset())) {
                g_bt_cm_disconnect_all = false;
                bt_connection_manager_state_machine_ext_cancel_role_resetup();
            } else
        #endif
            {
                bt_connection_manager_disconnect_all(false);
                return;
            }
        }
        if (NULL != other_link && BT_AWS_MCE_ROLE_AGENT == aws_role) {
            bt_connection_manager_disconnect_by_address(&(other_link->addr));
        }
        if(!((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) &&
            bt_connection_manager_memcmp(&(device_p->addr), local_addr, sizeof(bt_bd_addr_t)) && 
            (((bt_gap_config_t *)bt_sink_srv_gap_config())->cod != g_remote_cod) &&
            (false == g_bt_cm_dut_enable)) {
            bt_device_manager_db_remote_pnp_info_t device_id;
            if (BT_STATUS_SUCCESS != bt_device_manager_remote_find_pnp_info((void *)dev_addr, &device_id)) {
                bt_sdpc_service_request_t sdp_req = {
                   .address = (void *)&device_p->addr,
                   .pattern_length = sizeof(bt_sink_srv_connection_manager_search_di_pattern),
                   .search_pattern = bt_sink_srv_connection_manager_search_di_pattern,
                   .user_data = &device_p->addr,
                };
                bt_sdpc_search_service(&sdp_req);
            }
        }
        if ((g_sink_srv_cm_context.connected_dev_num == 2 && BT_AWS_MCE_ROLE_NONE != aws_role) ||
            (g_sink_srv_cm_context.connected_dev_num == 1 && BT_AWS_MCE_ROLE_NONE == aws_role)) {
        #ifdef MTK_AWS_MCE_ENABLE
            bt_gap_write_page_scan_activity(1024, 18);
        #else
            bt_gap_write_page_scan_activity(2048, 18);
        #endif
        }
        if (BT_AWS_MCE_ROLE_AGENT == aws_role) {
            if (g_sink_srv_cm_context.connected_dev_num == 2) {
                bt_sink_srv_state_set(BT_SINK_SRV_STATE_CONNECTED);
            }
        } else {
            if (g_sink_srv_cm_context.connected_dev_num == 1
        #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
                 && (BT_AWS_MCE_ROLE_PARTNER != aws_role)
        #endif
            ) {
                bt_sink_srv_state_set(BT_SINK_SRV_STATE_CONNECTED);
            }
        }
        if(g_bt_cm_power_on_open_discoverable){
            bt_connection_manager_discoverable_action_handle(true);
            g_bt_cm_power_on_open_discoverable = false;
        } else if (!bt_connection_manager_get_dut_mode()) {
            bt_connection_manager_discoverable_action_handle(false);
        }
        bt_gap_get_role_sync(device_p->handle, &device_p->local_role);
        device_p->aws_state = BT_AWS_MCE_AGENT_STATE_NONE;
    }

    if (BT_SINK_SRV_CM_STATE_ENCRYPTED != device_p->link_state && BT_GAP_LINK_STATUS_CONNECTED_0 < link_status) {
        bt_cmgr_report_id("[BT_CM][I] Link encrypted !!! ", 0);
        BT_SINK_SRV_CM_STATE_CHANGE(device_p, BT_SINK_SRV_CM_STATE_ENCRYPTED);
    #ifdef MTK_AWS_MCE_ENABLE
        //bt_connection_manager_enable_aws_discoverable(false);
    #endif
        bt_device_manager_db_remote_pnp_info_t device_id;
        if (BT_STATUS_SUCCESS != bt_device_manager_remote_find_pnp_info((void *)dev_addr, &device_id) &&
            (!((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role))) {
            bt_sdpc_service_request_t sdp_req = {
               .address = (void *)&device_p->addr,
               .pattern_length = sizeof(bt_sink_srv_connection_manager_search_di_pattern),
               .search_pattern = bt_sink_srv_connection_manager_search_di_pattern,
               .user_data = &device_p->addr,
            };
            bt_sdpc_search_service(&sdp_req);
        }
        bt_connection_manager_encryption_on_ind_t event;
        bt_connection_manager_memcpy((void *)(event.address), (void *)dev_addr, sizeof(bt_bd_addr_t));
        bt_sink_srv_event_callback(BT_CONNECTION_MANAGER_EVENT_ENCRYPTION_ON, (void *)(&event), sizeof(event));
    }
}

#if 0
static uint32_t bt_connection_manager_get_all_connected_device(bt_bd_addr_t *addr_list)
{
    uint32_t count = 0;
    if (NULL == addr_list) {
        return count;
    } else {
        bt_connection_manager_memset((void *)addr_list, 0, sizeof(bt_bd_addr_t) * BT_SINK_SRV_CM_MAX_DEVICE_NUMBER);
    }
    for (uint32_t i= 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
        bt_sink_srv_cm_remote_device_t *device_p = &(g_sink_srv_cm_context.devices[i]);
        if (BT_SINK_SRV_CM_STATE_CONNECTED <= device_p->link_state) {
            bt_connection_manager_memcpy(&addr_list[count], &g_sink_srv_cm_context.devices[i].addr, sizeof(bt_bd_addr_t));
            bt_cmgr_report_id("[BT_CM][I] Connected gap handle:0x%x", 1, g_sink_srv_cm_context.devices[i].handle);
            count++;
        }
    }

    bt_cmgr_report_id("[BT_CM][I] Connected count:%d", 1, count);
    return count;
}
#endif

uint32_t bt_sink_srv_cm_get_connected_device(bt_sink_srv_profile_type_t profile, bt_bd_addr_t *addr_list)
{
    uint32_t count = 0;
    bt_bd_addr_t* local_addr = bt_connection_manager_device_local_info_get_local_address();
    if (NULL == addr_list) {
        return count;
    } else {
        bt_connection_manager_memset((void *)addr_list, 0, sizeof(bt_bd_addr_t) * BT_SINK_SRV_CM_MAX_DEVICE_NUMBER);
    }
    for (uint32_t i= 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
        bt_sink_srv_cm_remote_device_t *device_p = &(g_sink_srv_cm_context.devices[i]);
        if (BT_SINK_SRV_CM_STATE_CONNECTED <= device_p->link_state &&
            (BT_SINK_SRV_PROFILE_NONE == profile || ((device_p->profile_connection_mask) & profile)) &&
            bt_connection_manager_memcmp(local_addr, &(device_p->addr), sizeof(bt_bd_addr_t))) {
            bt_connection_manager_memcpy(&addr_list[count], &g_sink_srv_cm_context.devices[i].addr, sizeof(bt_bd_addr_t));
            bt_cmgr_report_id("[BT_CM][I] Connected gap handle:0x%x", 1, g_sink_srv_cm_context.devices[i].handle);
            count++;
        }
    }

    bt_cmgr_report_id("[BT_CM][I] Connected count:%d", 1, count);
    return count;
}

uint32_t bt_sink_srv_cm_get_connected_device_list(bt_sink_srv_profile_type_t profile, bt_bd_addr_t *addr_list, uint32_t list_num)
{
    uint32_t count = 0;
    bt_bd_addr_t* local_addr = bt_connection_manager_device_local_info_get_local_address();
    if (NULL == addr_list) {
        return count;
    }
    bt_connection_manager_memset((void *)addr_list, 0, sizeof(bt_bd_addr_t) * list_num);
    for (uint32_t i= 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
        bt_sink_srv_cm_remote_device_t *device_p = &(g_sink_srv_cm_context.devices[i]);
        if ((BT_SINK_SRV_CM_STATE_CONNECTED <= device_p->link_state) &&
            (BT_SINK_SRV_PROFILE_NONE == profile || ((device_p->profile_connection_mask) & profile)) &&
            bt_connection_manager_memcmp(local_addr, &(device_p->addr), sizeof(bt_bd_addr_t))) {
            bt_connection_manager_memcpy(&addr_list[count], &(g_sink_srv_cm_context.devices[i].addr), sizeof(bt_bd_addr_t));
            bt_cmgr_report_id("[BT_CM][I] Connected gap handle:0x%x", 1, g_sink_srv_cm_context.devices[i].handle);
            if ((++count) == list_num) {
                break;
            }
        }
    }

    bt_cmgr_report_id("[BT_CM][I] Connected count:%d", 1, count);
    return count;
}

bool bt_sink_srv_cm_get_profile_data(bt_bd_addr_t *bt_addr, bt_sink_srv_profile_type_t profile, void *data_p)
{
    bool result = false;
    bt_device_manager_db_remote_profile_info_t *record = NULL;
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, bt_addr);

    if (NULL == (record = bt_connection_manager_memory_alloc(sizeof(bt_device_manager_db_remote_profile_info_t)))) {
        bt_cmgr_report_id("[BT_CM][I] Allocate record fail", 0);
        return result;
    }
    if (NULL != device_p && NULL != data_p &&
            BT_STATUS_SUCCESS == bt_device_manager_remote_find_profile_info(device_p->addr, record)) {
        switch (profile) {
#ifdef BT_SINK_SRV_HFP_STORAGE_SIZE
        case BT_SINK_SRV_PROFILE_HFP:
            bt_connection_manager_memcpy(data_p, record->hfp_info, BT_SINK_SRV_HFP_STORAGE_SIZE);
            result = true;
            break;
#endif
#ifdef BT_SINK_SRV_A2DP_STORAGE_SIZE
        case BT_SINK_SRV_PROFILE_A2DP_SINK:
            bt_connection_manager_memcpy(data_p, record->a2dp_info, BT_SINK_SRV_A2DP_STORAGE_SIZE);
            result = true;
            break;
#endif
#ifdef BT_SINK_SRV_AVRCP_STORAGE_SIZE
        case BT_SINK_SRV_PROFILE_AVRCP:
            bt_connection_manager_memcpy(data_p, record->avrcp_info, BT_SINK_SRV_AVRCP_STORAGE_SIZE);
            result = true;
            break;
#endif
#ifdef BT_SINK_SRV_PBAP_STORAGE_SIZE
        case BT_SINK_SRV_PROFILE_PBAPC:
            bt_connection_manager_memcpy(data_p, record->pbap_info, BT_SINK_SRV_PBAP_STORAGE_SIZE);
            result = true;
            break;
#endif
        default:
            break;
        }
    }
    bt_connection_manager_memory_free(record);
    return result;
}

bool bt_sink_srv_cm_set_profile_data(bt_bd_addr_t *bt_addr, bt_sink_srv_profile_type_t profile, void *data_p)
{
    bool result = false;
    bt_device_manager_db_remote_profile_info_t *record = NULL;
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, bt_addr);

    if (NULL == (record = bt_connection_manager_memory_alloc(sizeof(bt_device_manager_db_remote_profile_info_t)))) {
        bt_cmgr_report_id("[BT_CM][I] Allocate record fail", 0);
        return result;
    }
    if (NULL != device_p && NULL != data_p) {
        bt_device_manager_remote_find_profile_info(device_p->addr, record);
        switch (profile) {
#ifdef BT_SINK_SRV_HFP_STORAGE_SIZE
            case BT_SINK_SRV_PROFILE_HFP:
                bt_connection_manager_memcpy(record->hfp_info, data_p, BT_SINK_SRV_HFP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_A2DP_STORAGE_SIZE
            case BT_SINK_SRV_PROFILE_A2DP_SINK:
                bt_connection_manager_memcpy(record->a2dp_info, data_p, BT_SINK_SRV_A2DP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_AVRCP_STORAGE_SIZE
            case BT_SINK_SRV_PROFILE_AVRCP:
                bt_connection_manager_memcpy(record->avrcp_info, data_p, BT_SINK_SRV_AVRCP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_PBAP_STORAGE_SIZE
            case BT_SINK_SRV_PROFILE_PBAPC:
                bt_connection_manager_memcpy(record->pbap_info, data_p, BT_SINK_SRV_PBAP_STORAGE_SIZE);
                result = true;
                break;
#endif
            default:
                break;
        }
        bt_device_manager_remote_update_profile_info(device_p->addr, record);
    }
    bt_connection_manager_memory_free(record);
    return result;
}

void *bt_sink_srv_cm_get_profile_info(bt_bd_addr_t *address, bt_sink_srv_profile_type_t  profile)
{
    void *profile_info = NULL;
    for (uint8_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        if (bt_connection_manager_is_valid_address(&g_sink_srv_cm_context.devices[i].addr)
                && 0 == bt_connection_manager_memcmp((uint8_t *)&g_sink_srv_cm_context.devices[i].addr, (uint8_t *)address, sizeof(bt_bd_addr_t))) {
            switch (profile) {
                case BT_SINK_SRV_PROFILE_HFP:
                    profile_info = g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_HFP];
                break;
                case BT_SINK_SRV_PROFILE_A2DP_SINK:
                    profile_info = g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_A2DP_SINK];
                break;
                case BT_SINK_SRV_PROFILE_AVRCP:
                    profile_info = g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_AVRCP];
                break;
                case BT_SINK_SRV_PROFILE_PBAPC:
                    profile_info = g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_PBAPC];
                break;
                case BT_SINK_SRV_PROFILE_AWS:
                    profile_info = g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_AWS];
                break;
                case BT_SINK_SRV_PROFILE_HSP:
                    profile_info = g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_HSP];
                break;
                default:
                break;
            }
            break;
        }
    }
    return profile_info;
}

bt_bd_addr_t *bt_sink_srv_cm_get_address_by_profile_info(void *profile_info)
{
    if (NULL != profile_info) {
        uint8_t i, j;
        // search connected device
        for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
            if (bt_connection_manager_is_valid_address(&g_sink_srv_cm_context.devices[i].addr)) {
                // search profile
                for (j = 0; j < BT_SINK_SRV_CM_MAX_PROFILE_NUMBER; ++j) {
                    if (NULL != g_sink_srv_cm_context.devices[i].profile_info[j]
                            && profile_info == g_sink_srv_cm_context.devices[i].profile_info[j]) {
                        return &g_sink_srv_cm_context.devices[i].addr;
                    }
                }
            }
        }
    }
    return NULL;
}

uint32_t bt_sink_srv_cm_get_handle_by_profile_info(void *profile_info)
{
    if (NULL != profile_info) {
        uint8_t i, j;
        // search connected device
        for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
            if (bt_connection_manager_is_valid_address(&g_sink_srv_cm_context.devices[i].addr)) {
                // search profile
                for (j = 0; j < BT_SINK_SRV_CM_MAX_PROFILE_NUMBER; ++j) {
                    if (NULL != g_sink_srv_cm_context.devices[i].profile_info[j]
                        && profile_info == g_sink_srv_cm_context.devices[i].profile_info[j]) {
                        return (uint32_t)g_sink_srv_cm_context.devices[i].handle;
                    }
                }
            }
        }
    }
    return 0;
}

static void bt_connection_manager_discoverable_timeout(void *parameter)
{
   bt_cmgr_report_id("[BT_CM][I] Scan mode:%d, connected dev num:%d", 2,
         g_sink_srv_cm_context.scan_mode, g_sink_srv_cm_context.connected_dev_num);
    if (g_sink_srv_cm_context.scan_mode == BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE ||
                g_sink_srv_cm_context.scan_mode == BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY ) {
        /* 1, For agent role need 2 acl link.
                 2, For None role, controller not support 3 slave now, so we should disable page scan when 2 links had connected. */
        bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
        uint8_t stop_connectable_num = (BT_AWS_MCE_ROLE_AGENT == aws_role ? 2 : BT_SINK_SRV_CM_MAX_DEVICE_NUMBER);

        if (g_sink_srv_cm_context.connected_dev_num < stop_connectable_num) {
            bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
        } else {
            bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);
        }
    }
    return;
}

static void bt_connection_manager_power_on_callback(bt_status_t result, void *parameters)
{
     /* For agent role need 2 acl link. */
    bt_cmgr_report_id("[BT_CM][I] power_on_callback result:%d", 1, result);
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    uint8_t request_cancel_num = (BT_AWS_MCE_ROLE_AGENT == aws_role ? 2 : BT_SINK_SRV_CM_MAX_DEVICE_NUMBER);
    uint8_t stop_connectable_num = (BT_AWS_MCE_ROLE_AGENT == aws_role ? 2 : BT_SINK_SRV_CM_MAX_DEVICE_NUMBER);
    if (bt_connection_manager_request_find_node_num_by_type(BT_SINK_SRV_CM_REQ_TYPE_POWER_ON) == 0 &&
        g_sink_srv_cm_context.connected_dev_num < stop_connectable_num) {
        bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
    } else {
        bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);
    }
    if (g_sink_srv_cm_context.connected_dev_num >= request_cancel_num) {
        bt_connection_manager_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_POWER_ON, NULL);
    }
}

static bt_status_t bt_connection_manager_power_on_reconnection()
{
    const bt_connection_manager_features_config_t *config_p = bt_connection_manager_get_features_config();
    if (!(config_p->features & BT_CONNECTION_MANAGER_FEATURE_POWER_ON_RECONNECT)) {
        bt_cmgr_report_id("[BT_CM][W] Config not allowed power on reconnection", 0);
        return BT_STATUS_SUCCESS;
    }
    bt_sink_srv_cm_request_t request;
    bt_status_t result =BT_CONNECTION_MANAGER_STATUS_FAIL;
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    bt_bd_addr_t* conn_addr = NULL;
    const bt_sink_srv_feature_reconnect_params_t * params = bt_sink_srv_get_power_on_reconnect_params();

    bt_cmgr_report_id("[BT_CM][I] Power on reconnection params, attempts:%d", 1, params->attampts);
    if(params->attampts ==0) {
        return BT_STATUS_SUCCESS;
    }

    if (BT_AWS_MCE_ROLE_PARTNER == aws_role
        || BT_AWS_MCE_ROLE_CLINET == aws_role) {
        return BT_STATUS_FAIL;
    }

    bt_connection_manager_memset(&request, 0, sizeof(bt_sink_srv_cm_request_t));
    request.type = BT_SINK_SRV_CM_REQ_TYPE_POWER_ON;
    request.attampts = params->attampts;
    request.callback = bt_connection_manager_power_on_callback;
    //request.delay_time = BT_SINK_SRV_REQUEST_DELAY_TIME;
    request.delay_time = 10;
    request.action_param.profile_connection_mask = (bt_sink_srv_profile_type_t)
                 (BT_SINK_SRV_PROFILE_HFP | BT_SINK_SRV_PROFILE_A2DP_SINK);
    //Re-connect to the last two connected remote device.
    uint8_t connect_num = 0;
    if (BT_AWS_MCE_ROLE_AGENT == aws_role) {
        /*Reserve one connect num for special aws link. */
        /*Reserve one connect num for customer's nfc pairing. */
        connect_num += 2;
#ifdef MTK_IAP2_PROFILE_ENABLE
        request.action_param.profile_connection_mask |= BT_SINK_SRV_PROFILE_IAP2;
#endif
    } else if (BT_AWS_MCE_ROLE_NONE == aws_role) {
        //connect_num += 1; //GVA-4286, N9 limitation, reserver one link for page sacn.
        connect_num += (BT_SINK_SRV_CM_MAX_DEVICE_NUMBER - 1); // Only reconnect latest device.
#ifdef MTK_IAP2_PROFILE_ENABLE
		request.action_param.profile_connection_mask |= BT_SINK_SRV_PROFILE_IAP2;
#endif

    }
    for (uint8_t i = 0; i < BT_SINK_SRV_CM_MAX_TRUSTED_DEV; i++) {
        conn_addr = bt_device_manager_remote_get_dev_by_seq_num(i + 1);
        if (NULL != conn_addr && true == bt_device_manager_is_paired((void*)conn_addr)) {
            connect_num++;
            bt_connection_manager_memcpy(&request.action_param.address, conn_addr, sizeof(bt_bd_addr_t));
            bt_cmgr_report_id("[BT_CM][I] Reconnect addr:%02x:%02x:%02x:%02x:%02x:%02x", 6,
                                (*conn_addr)[5],(*conn_addr)[4], (*conn_addr)[3],
                                (*conn_addr)[2],(*conn_addr)[1], (*conn_addr)[0]);
            bt_cmgr_report_id("[BT_CM][I] Reconnect mask:0x%x", 1, request.action_param.profile_connection_mask);
            result = bt_connection_manager_request_new(&request);
        }
        if (BT_SINK_SRV_CM_MAX_DEVICE_NUMBER == connect_num) {
            break;
        }
    }
    return result;
}

static void bt_connection_manager_link_loss_reconnection(bt_bd_addr_t* address, bt_status_t status)
{
    const bt_connection_manager_features_config_t *config_p = bt_connection_manager_get_features_config();
    if (0 == (config_p->features & BT_CONNECTION_MANAGER_FEATURE_RECONNECT_ON_LINK_LOSS)) {
        bt_cmgr_report_id("[BT_CM][I] Not enable link loss reconnection, config:0x%x", 1, config_p->features);
        return;
    }

    const bt_sink_srv_feature_reconnect_params_t* config_params = bt_sink_srv_get_link_loss_reconnect_params();
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    bt_sink_srv_cm_request_t link_lost;
    bt_connection_manager_memset((void *)&link_lost, 0, sizeof(bt_sink_srv_cm_request_t));
    link_lost.type = BT_SINK_SRV_CM_REQ_TYPE_LINK_LOST;

    if (BT_SINK_SRV_LINK_LOSS_RETRY_NUM_FOR_QA_TEST > 0xFF) {
        link_lost.attampts = config_params->attampts;
    } else {
        link_lost.attampts = BT_SINK_SRV_LINK_LOSS_RETRY_NUM_FOR_QA_TEST;
    }

    link_lost.delay_time = BT_SINK_SRV_REQUEST_DELAY_TIME;

    if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
        link_lost.action_param.profile_connection_mask = BT_SINK_SRV_PROFILE_AWS;
    } else if (BT_AWS_MCE_ROLE_AGENT == aws_role) {
        bt_bd_addr_t* local_agent = bt_connection_manager_device_local_info_get_local_address();
        if (local_agent && (bt_connection_manager_memcmp(address, local_agent, sizeof(bt_bd_addr_t)) == 0)) {
            link_lost.action_param.profile_connection_mask = BT_SINK_SRV_PROFILE_AWS;
        } else {
#ifdef MTK_IAP2_PROFILE_ENABLE
            link_lost.action_param.profile_connection_mask =
                 (bt_sink_srv_profile_type_t)(BT_SINK_SRV_PROFILE_HFP | BT_SINK_SRV_PROFILE_A2DP_SINK|BT_SINK_SRV_PROFILE_IAP2);
#else
            link_lost.action_param.profile_connection_mask =
                 (bt_sink_srv_profile_type_t)(BT_SINK_SRV_PROFILE_HFP | BT_SINK_SRV_PROFILE_A2DP_SINK);
#endif
        }
    } else if (BT_AWS_MCE_ROLE_NONE == aws_role) {
#ifdef MTK_IAP2_PROFILE_ENABLE
        link_lost.action_param.profile_connection_mask =
            (bt_sink_srv_profile_type_t)(BT_SINK_SRV_PROFILE_HFP | BT_SINK_SRV_PROFILE_A2DP_SINK|BT_SINK_SRV_PROFILE_IAP2);
#else
        link_lost.action_param.profile_connection_mask =
            (bt_sink_srv_profile_type_t)(BT_SINK_SRV_PROFILE_HFP | BT_SINK_SRV_PROFILE_A2DP_SINK);
#endif
    } else {
        bt_cmgr_report_id("[BT_CM][E] Unexpected role:%x", 1, aws_role);
        bt_connection_manager_assert(0);
    }

    bt_cmgr_report_id("[BT_CM][I] Link lost reconnect mask:0x%x", 1, link_lost.action_param.profile_connection_mask);
    bt_connection_manager_memcpy(&link_lost.action_param.address, address, sizeof(bt_bd_addr_t));
    bt_connection_manager_request_new(&link_lost);
}

void bt_sink_srv_cm_init(void)
{
    bt_connection_manager_memset((void *)&g_sink_srv_cm_context, 0, sizeof(bt_sink_srv_cm_context_t));
    g_sink_srv_cm_context.request_list.next = &g_sink_srv_cm_context.request_list;
    g_sink_srv_cm_context.request_list.prev = &g_sink_srv_cm_context.request_list;
    for (uint32_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        // register profiles
        g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_HFP] = bt_sink_srv_cm_get_hf_info(i);
        g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_A2DP_SINK] = bt_sink_srv_cm_get_a2dp_sink_info(i);
        g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_AVRCP] = bt_sink_srv_cm_get_avrcp_info(i);
        g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_PBAPC] = bt_sink_srv_cm_get_pbapc_info(i);
        g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_AWS] = bt_sink_srv_cm_get_aws_info(i);
        g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_HSP] = bt_sink_srv_cm_get_hsp_info(i);
        g_sink_srv_cm_context.devices[i].local_role = BT_SINK_SRV_ROLE_UNKNOWN;
        g_sink_srv_cm_context.devices[i].profile_connection_mask = BT_SINK_SRV_PROFILE_NONE;
        g_sink_srv_cm_context.devices[i].device_state = BT_SINK_SRV_STATE_POWER_ON;
#ifdef MTK_AWS_MCE_ENABLE
        g_sink_srv_cm_context.devices[i].aws_state = BT_AWS_MCE_AGENT_STATE_NONE;
        g_sink_srv_cm_context.devices[i].set_aws_state = BT_SINK_SRV_CM_SET_AWS_ENABLE_SUCCESS;
#endif
    }
    bt_sink_srv_music_init();
    // init profiles
    bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PROFILE_INIT, NULL);
    bt_sink_srv_role_switch_retry_times = bt_sink_srv_get_role_switch_retry_times();
}

static void bt_sink_srv_cm_deinit(void)
{
    bt_connection_manager_timer_stop(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY);
    bt_connection_manager_timer_stop(BT_SINK_SRV_TIMER_CM_DISCOVERABLE);
    bt_connection_manager_timer_stop(BT_SINK_SRV_TIMER_CM_PROFILE_ALREADY_EXIST_NOTIFY);
    bt_connection_manager_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_NONE, NULL);
    bt_connection_manager_timer_stop(BT_SINK_SRV_CM_TERMINATE_AIR_PAIRING_TIMER_ID);
    g_sink_srv_cm_context.request_pending = NULL;
    bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PROFILE_DEINIT, NULL);
    for (uint32_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        // register profiles
        if (NULL != g_sink_srv_cm_context.devices[i].rho_sync_data) {
            bt_connection_manager_memory_free(g_sink_srv_cm_context.devices[i].rho_sync_data);
            g_sink_srv_cm_context.devices[i].rho_sync_data = NULL;
        }
    }
}

static void bt_sink_srv_cm_set_eir()
{
    bt_cmgr_report_id("[BT_CM][I] Set  eir", 0);
    const bt_sink_srv_eir_information_t* eir_info = bt_sink_srv_get_eir();
    const bt_gap_config_t * sink_gap_config = bt_gap_get_local_configuration();
    //const bt_gap_config_t * sink_gap_config = bt_sink_srv_gap_config();
    uint32_t name_length;
    uint8_t eir_data[56] = {0x11, 0x06};
    name_length = strlen(sink_gap_config->device_name);
    bt_connection_manager_memcpy((void *)(eir_data + 2), (void *) eir_info->uuid_128bit, 16);
    eir_data[18] = name_length + 1;
    eir_data[19] = 0x09;
    bt_connection_manager_memcpy((void *)(eir_data + 20), (void *) sink_gap_config->device_name, name_length);
    bt_gap_set_extended_inquiry_response(eir_data, 20 + name_length);
}

static void bt_connection_manager_power_on_create_aws_link()
{
    bt_sink_srv_cm_request_t request;
    bt_status_t result =BT_CONNECTION_MANAGER_STATUS_FAIL;
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    bt_bd_addr_t* conn_addr = NULL;

    bt_connection_manager_memset(&request, 0, sizeof(bt_sink_srv_cm_request_t));
    request.type = BT_SINK_SRV_CM_REQ_TYPE_POWER_ON;
    request.attampts = 0xFF;
    //request.delay_time = BT_SINK_SRV_REQUEST_DELAY_TIME;
    request.delay_time = 10;
    request.action_param.profile_connection_mask = BT_SINK_SRV_PROFILE_AWS;
    if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
        bt_connection_manager_reset_flags(BT_SINK_SRV_FLAG_AWS_POWERING_ON);
        conn_addr = bt_connection_manager_device_local_info_get_peer_aws_address();
        if (conn_addr) {
            bt_connection_manager_memcpy(&request.action_param.address, conn_addr, sizeof(bt_bd_addr_t));
            result = bt_connection_manager_request_new(&request);
        } else {
            bt_connection_manager_assert(0 && "[BT_CM][E] No agent addr.");
        }
    } else if (aws_role == BT_AWS_MCE_ROLE_AGENT) {
        conn_addr = bt_connection_manager_device_local_info_get_local_address();
        bt_connection_manager_memcpy(&request.action_param.address, conn_addr, sizeof(bt_bd_addr_t));
        result = bt_connection_manager_request_new(&request);
    } else {
        bt_connection_manager_assert(0 && "[BT_CM][I] Aws role error.");
    }

    bt_cmgr_report_id("[BT_CM][I] power on create aws link result : %d", 1, result);
}

static void bt_connection_manager_power_on_action_handler(void)
{
    const bt_connection_manager_features_config_t *config_p = bt_connection_manager_get_features_config();
    bool open_visibility = false;
    bt_cmgr_report_id("[BT_CM][I] Config feartures:0x%x", 1, config_p->features);

    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    if (config_p->features & BT_CONNECTION_MANAGER_FEATURE_VISIBILITY) {
        if (BT_AWS_MCE_ROLE_AGENT == aws_role) {
            open_visibility = bt_connection_manager_open_visibility_feature();
        } else if (BT_AWS_MCE_ROLE_NONE == aws_role) {
            bt_bd_addr_t* conn_addr = NULL;
            for (uint32_t i = 0; i < BT_SINK_SRV_CM_MAX_TRUSTED_DEV; i++) {
                conn_addr = bt_device_manager_remote_get_dev_by_seq_num(i + 1);
                if (NULL != conn_addr) {
                    bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);
                    break;
                }
                if (BT_SINK_SRV_CM_MAX_TRUSTED_DEV == (i + 1)) {
                    open_visibility = bt_connection_manager_open_visibility_feature();
                }
            }
        }
    }

    if (!(g_bt_cm_power_reconnect_flag & BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_ACTIVE)) {
        bt_cmgr_report_id("[BT_CM][W] Role re-setup forbit power reconnection", 0);
        return;
    }
    if (!open_visibility && (BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE != g_sink_srv_cm_context.scan_mode)) {
        bt_connection_manager_power_on_reconnection();
    }
}

static void bt_connection_manager_write_aws_page_scan(void)
{
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
        bt_gap_write_page_scan_activity(0x800|0x8000, 18);
    } else if (aws_role == BT_AWS_MCE_ROLE_AGENT) {
        bt_gap_write_page_scan_activity(0x60|0x8000, 18);
    }
}

static void bt_connection_manager_switch_mode_notify(void)
{
    bt_connection_manager_mode_changed_ind_t event;
    event.type = bt_firmware_type_get();
    event.role = bt_connection_manager_device_local_info_get_aws_role();
    bt_cmgr_report_id("[BT_CM][I] Mode changed notify, type:0x%x, AWS role:0x%x", 2, event.type, event.role);
    bt_connection_manager_event_callback(BT_CONNECTION_MANAGER_EVENT_MODE_CHANGED_IND, &event, sizeof(event));
}

static void bt_connection_manager_handle_power_on_cnf(void)
{
    bt_gap_default_sniff_params_t sniff_params = {
        .max_sniff_interval = 800,
        .min_sniff_interval = 800,
        .sniff_attempt = 4,
        .sniff_timeout = 1
    };
    bt_sink_srv_cm_init();
    //bt_sink_srv_state_set(BT_SINK_SRV_STATE_POWER_ON);
#ifdef MTK_AWS_MCE_ENABLE
    if(BT_STATUS_SUCCESS == bt_connection_manager_aws_air_pairing_power_on_check()) {
        return;
    }
#endif
    bt_sink_srv_cm_set_eir();
    bt_gap_set_default_sniff_parameters(&sniff_params);
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();

    if (bt_connection_manager_get_dut_mode()) {
        /* disable auto-connect */
#ifdef MTK_AWS_MCE_ENABLE
        bt_connection_manager_allow_power_on_reconnect(false);
        //bt_connection_manager_dut_mode_switch_role();
#endif
    }

    if (bt_cm_is_reload_n9) {
        bt_cm_is_reload_n9 = false;
    }

    if (g_bt_cm_reload_n9_flag) {
        g_bt_cm_reload_n9_flag = false;
        bt_connection_manager_switch_mode_notify();
    }
    if (BT_AWS_MCE_ROLE_NONE == aws_role) {
        bt_connection_manager_power_on_action_handler();
    } else {
#ifdef MTK_AWS_MCE_ENABLE
    #ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        bt_connection_manager_aws_rho_init();
    #endif
        bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_CM, bt_connection_manager_aws_mce_packet_callback);
        // read ls enable configure.
        if (BT_CONNECTION_MANAGER_AWS_LS_DISABLE == bt_connection_manager_device_local_info_get_aws_ls_enable()) {
            bt_sink_srv_cm_ls_enable_t = false;
        }
        bt_connection_manager_set_flags(BT_SINK_SRV_FLAG_AWS_POWERING_ON);
        if (BT_AWS_MCE_ROLE_PARTNER == aws_role) {
            aws_role |= BT_AWS_MCE_ROLE_CLINET;
        }
        bt_aws_mce_init_role(aws_role);
        if (BT_FIRMWARE_TYPE_SPEAKER == bt_firmware_type_get() &&
            ((BT_AWS_MCE_ROLE_AGENT | BT_AWS_MCE_ROLE_CLINET) & aws_role)) {
            bt_hci_send_vendor_cmd(0xFDCA, (uint8_t *)(bt_device_manager_aws_local_info_get_speaker_fixed_address()), sizeof(bt_bd_addr_t));
        }
        bt_connection_manager_write_aws_key();
        if (!bt_connection_manager_get_dut_mode()) {
        #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
            bt_connection_manager_state_ext_init();
            bt_connection_manager_state_machine_ext_power_onoff_notify(1);
        #endif
        }
#endif
    }
    if (BT_AWS_MCE_ROLE_AGENT != aws_role) {
        bt_connection_manager_event_callback(BT_CONNECTION_MANAGER_EVENT_INIT_IND, NULL, 0);
    }
}

#ifdef MTK_AWS_MCE_ENABLE
static void bt_connection_reset_aws_params(bt_connection_manager_switch_mode_params_t* params)
{
    bt_connection_manager_assert(params);
    bt_key_t zero_link_key = {0};
    bt_bd_addr_t zero_addr = {0};
    bt_cmgr_report_id("[BT_CM][I] Reset AWS params, type:0x%x", 1, params->type);
    switch (params->type) {
        case BT_FIRMWARE_TYPE_HAEDSET: {
            bt_device_manager_aws_local_info_store_key(&zero_link_key);
            bt_device_manager_aws_local_info_store_role(BT_AWS_MCE_ROLE_NONE);
            bt_device_manager_aws_local_info_store_peer_address(&zero_addr);
            bt_device_manager_aws_local_info_update();
        }
        break;

        case BT_FIRMWARE_TYPE_SPEAKER: {
            bt_device_manager_aws_local_info_store_key(&params->aws_key);
            if (BT_AWS_MCE_ROLE_AGENT ==  params->role) {
                bt_device_manager_aws_local_info_store_role(params->role);
                bt_device_manager_aws_local_info_store_peer_address(&zero_addr);
                bt_device_manager_aws_local_info_store_speaker_fixed_address(&(params->addr));
                bt_device_manager_aws_local_info_update();
            } else if (BT_AWS_MCE_ROLE_CLINET == params->role) {
                bt_device_manager_aws_local_info_store_role(params->role);
                bt_device_manager_aws_local_info_store_peer_address(&params->addr);
                bt_device_manager_aws_local_info_store_speaker_fixed_address(&(params->addr));
                bt_device_manager_aws_local_info_update();
            } else {
                bt_connection_manager_assert(0 && "Wrong AWS role!");
            }
        }
        break;

        case BT_FIRMWARE_TYPE_EARBUDS: {
            bt_device_manager_aws_local_info_store_key(&zero_link_key);
            if (BT_AWS_MCE_ROLE_AGENT == params->role) {
                bt_device_manager_aws_local_info_store_role(params->role);
                bt_device_manager_aws_local_info_store_peer_address(&zero_addr);
                bt_device_manager_aws_local_info_update();
            } else if (BT_AWS_MCE_ROLE_PARTNER == params->role) {
                bt_device_manager_aws_local_info_store_role(params->role);
                bt_device_manager_aws_local_info_store_peer_address(&params->addr);
                bt_device_manager_aws_local_info_update();
            } else {
                bt_connection_manager_assert(0 && "Wrong AWS role!");
            }
        }
        break;

        default:{
            bt_cmgr_report_id("[BT_CM][W] Invalid N9 type!", 0);
        }
        break;
    }
    return;
}
#endif

static void bt_connection_manager_handle_power_off_cnf(void)
{
    bt_sink_srv_cm_deinit();
    //bt_sink_srv_state_set(BT_SINK_SRV_STATE_NONE);
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    g_bt_cm_dut_enable = false;

    if ((g_sink_srv_cm_context.scan_mode == BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE ||
         g_sink_srv_cm_context.scan_mode == BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY
         ) && !((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role)) {
        bt_cmgr_report_id("[BT_CM][I] Power off cnf send visibility off to APP", 0);
        bt_sink_srv_visibility_state_update_t event;
        event.visibility_on = false;
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_VISIBILITY_STATE_UPDATE, (void *)(&event), sizeof(event));
    }

    if (bt_connection_manager_get_dut_mode()) {
#ifdef MTK_AWS_MCE_ENABLE
        bt_connection_manager_dut_mode_switch_role();
#endif
    }

    if (BT_AWS_MCE_ROLE_NONE != aws_role) {
#ifdef MTK_AWS_MCE_ENABLE
    #ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        bt_connection_manager_aws_rho_deinit();
    #endif
    #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
        bt_connection_manager_state_machine_ext_power_onoff_notify(0);
        bt_connection_manager_state_ext_deinit();
    #endif
        bt_aws_mce_report_deregister_callback(BT_AWS_MCE_REPORT_MODULE_CM);
        if (true == bt_connection_manager_aws_pair_pairing_is_ongoing()) {
            bt_bd_addr_t *aws_fixed_addr = bt_connection_manager_device_local_info_get_aws_fixed_address();
            bt_set_local_public_address((void *)aws_fixed_addr);
            bt_connection_manager_device_local_info_store_local_address(aws_fixed_addr);
            uint8_t peer_addr[6] = {0};
            bt_connection_manager_device_local_info_store_peer_aws_address((bt_bd_addr_t *)peer_addr);
        }

    /*#ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
        if (1 == bt_connection_manager_state_machine_ext_is_in_role_resetup()) {
            bt_connection_manager_handle_power_on_cnf();
        }
    #endif*/
#endif
    }
    if (g_bt_cm_reload_n9_flag) {
    #ifdef MTK_AWS_MCE_ENABLE
        bt_connection_reset_aws_params(&g_n9_fw_reload_context);
        bt_firmware_type_set(g_n9_fw_reload_context.type);
        bt_aws_mce_init_role(BT_AWS_MCE_ROLE_NONE);
    #endif /*MTK_AWS_MCE_ENABLE*/
        bt_sink_srv_send_action(BT_SINK_SRV_ACTION_BT_ACTIVE, NULL);
    }

    if (bt_cm_is_reload_n9) {
#ifdef MTK_AWS_MCE_ENABLE
        bt_firmware_type_set(BT_FIRMWARE_TYPE_HAEDSET);
        bt_aws_mce_init_role(BT_AWS_MCE_ROLE_NONE);
    #endif /*MTK_AWS_MCE_ENABLE*/
        bt_sink_srv_send_action(BT_SINK_SRV_ACTION_BT_ACTIVE, NULL);
    }
}

void bt_connection_manager_write_scan_enable_mode(bt_gap_scan_mode_t mode)
{
    bt_cmgr_report_id("[BT_CM][I] Scan mode, old: %x, new:%x", 2, g_sink_srv_cm_context.scan_mode, mode);

    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
        bt_cmgr_report_id("[BT_CM][E] Write scan enable on invalid role !!!", 0);
        return;
    }
    if (bt_connection_manager_get_flags() & BT_SINK_SRV_FLAG_SET_SCAN_MODE) {
        bt_cmgr_report_id("[BT_CM][W] Scan mode is setting ...", 0);
        return;
    }
    if (!(g_bt_cm_power_reconnect_flag & BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_PASSIVE)
        && false == bt_connection_manager_get_dut_mode()) {
        bt_cmgr_report_id("[BT_CM][W] Role re-setup mask page scan", 0);
        if (BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE == mode) {
            mode = BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY;
        } else if (BT_GAP_SCAN_MODE_CONNECTABLE_ONLY == mode) {
            mode = BT_GAP_SCAN_MODE_NOT_ACCESSIBLE;
        }
    }
    if (g_sink_srv_cm_context.scan_mode != mode) {
        const bt_connection_manager_features_config_t *config_p = bt_connection_manager_get_features_config();
        const bt_sink_srv_feature_visibility_params_t * visibility_params = bt_sink_srv_get_visibility_params();
        if (mode == BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE || mode == BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY) {
            if (!(config_p->features & BT_CONNECTION_MANAGER_FEATURE_VISIBILITY)) {
                bt_cmgr_report_id("[BT_CM][E] Config not allowed to set discoverable.", 0);
                return;
            } else if (NULL != visibility_params && 0xFFFFFFFF != visibility_params->visibility_duration) {
                bt_connection_manager_timer_start(BT_SINK_SRV_TIMER_CM_DISCOVERABLE, visibility_params->visibility_duration,
                    bt_connection_manager_discoverable_timeout, NULL);
            }
        } else if (bt_connection_manager_timer_is_exist(BT_SINK_SRV_TIMER_CM_DISCOVERABLE)) {
            bt_connection_manager_timer_stop(BT_SINK_SRV_TIMER_CM_DISCOVERABLE);
        }
        g_sink_srv_cm_context.scan_mode = mode;
        bt_connection_manager_set_flags(BT_SINK_SRV_FLAG_SET_SCAN_MODE);
        bt_gap_set_scan_mode(mode);
        bt_cmgr_report_id("[BT_CM][I] Scan enable:%x", 1, mode);
    }
}

void bt_connection_manager_set_page_scan(bool enable)
{
    if (true == enable) {
        if (BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY == g_sink_srv_cm_context.scan_mode) {
            bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);
        } else if (BT_GAP_SCAN_MODE_NOT_ACCESSIBLE == g_sink_srv_cm_context.scan_mode) {
            bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
        }
    } else if (false == enable) {
        if (BT_GAP_SCAN_MODE_CONNECTABLE_ONLY == g_sink_srv_cm_context.scan_mode) {
            bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);
        } else if (BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE == g_sink_srv_cm_context.scan_mode) {
            bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY);
        }
    }
}

void bt_connection_manager_scan_mode_update()
{
    if (BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY == g_sink_srv_cm_context.scan_mode ||
        BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE == g_sink_srv_cm_context.scan_mode) {
        bt_connection_manager_discoverable_action_handle(true);
    } else {
        bt_connection_manager_discoverable_action_handle(false);
    }
}


static void bt_sink_srv_cm_handle_link_disconnected(bt_status_t status, bt_gap_link_status_updated_ind_t *param)
{
    bt_cmgr_report_id("[BT_CM][I] detach single link, reason: 0x%x, handle = 0x%x", 2, status, param->handle);
    bt_sink_srv_cm_remote_device_t *disconn_device = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_HANDLE, &(param->handle));
    if (NULL == disconn_device) {
        bt_cmgr_report_id("[BT_CM][E] No connection available", 0);
        return;
    }
    bt_cmgr_report_id("[BT_CM][I] disconn_device->flag 0x%x, ",1 , disconn_device->flag);
    bool abnormal_disconnect = false;
    if (disconn_device->flag & BT_SINK_SRV_CM_REMOTE_FLAG_CONNECTION_ABNORMAL) {
        abnormal_disconnect = true;
    }
    bt_connection_manager_free_device(disconn_device);
    g_sink_srv_cm_context.connected_dev_num--;
    bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
    if (bt_connection_manager_memcmp(param->address, local_addr, sizeof(bt_bd_addr_t))) {
        bt_cmgr_report_id("[BT_CM][I] Normal aws link disconnected, connected num now : %d", 1, g_sink_srv_cm_context.connected_dev_num);
    } else {
        bt_cmgr_report_id("[BT_CM][I] Special aws link disconnected, connected num now : %d ", 1, g_sink_srv_cm_context.connected_dev_num);
    }
    if (NULL != g_sink_srv_cm_context.request_pending
            && !bt_connection_manager_memcmp(param->address, &g_sink_srv_cm_context.request_pending->parameters->action_param.address, sizeof(bt_bd_addr_t))) {
        if (bt_connection_manager_timer_is_exist(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY)) {
            bt_connection_manager_timer_stop(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY);
        }
        g_sink_srv_cm_context.request_pending->parameters->attampts = 0;
        bt_connection_manager_request_done(BT_CONNECTION_MANAGER_STATUS_FAIL, g_sink_srv_cm_context.request_pending);
    }
    if (BT_HCI_STATUS_PIN_OR_KEY_MISSING == status) {
        bt_connection_manager_memset((void *)&g_sink_srv_cm_last_connected, 0, sizeof(bt_bd_addr_t));
        bt_device_manager_remote_delete_info((bt_bd_addr_t *)(param->address), BT_DEVICE_MANAGER_REMOTE_INFO_MASK_PAIRED);
    }
    if (BT_CM_POWER_STATE_OFF_PENDING == bt_connection_manager_power_get_state()) {
        bt_connection_manager_power_off(false);
        return;
    }
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    /* Send a event to application indicate partner detached, after smatphone disconnected. */
    if (BT_AWS_MCE_ROLE_AGENT == aws_role && NULL == bt_sink_srv_cm_get_aws_connected_device()) {
        bt_sink_srv_event_param_t *event = bt_connection_manager_memory_alloc(sizeof(*event));
        if (event) {
            bt_cmgr_report_id("[BT_CM][I] Notify aws inactive to application", 0);
            bt_connection_manager_memcpy(&(event->aws_state_change.bt_addr), (void *)(param->address), sizeof(bt_bd_addr_t));
            event->aws_state_change.aws_state = BT_AWS_MCE_AGENT_STATE_INACTIVE;
            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MCE_STATE_CHANGE, (void *)event, sizeof(*event));
    #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
            bt_connection_manager_state_machine_ext_sink_event_callback(BT_SINK_SRV_EVENT_AWS_MCE_STATE_CHANGE, (void *)event, sizeof(*event));
    #endif
            bt_connection_manager_memory_free(event);
        } else {
            bt_cmgr_report_id("[BT_CM][E] Not enough heap memory.", 0);
        }
    }

    if ((g_sink_srv_cm_context.connected_dev_num <= 1 && BT_AWS_MCE_ROLE_NONE != aws_role) ||
        (g_sink_srv_cm_context.connected_dev_num == 0 && BT_AWS_MCE_ROLE_NONE == aws_role)) {
        /* notify ACL link disconnect reaon */
        bt_connection_manager_acl_link_ind_t event;
        event.is_connected = false;
        event.reason = status;
        bt_connection_manager_memcpy(&event.address, param->address, sizeof(bt_bd_addr_t));
        bt_connection_manager_event_callback(BT_CONNECTION_MANAGER_EVENT_ACL_LINK_STATE_IND, (void *)(&event), sizeof(event));

        bt_sink_srv_state_set(BT_SINK_SRV_STATE_POWER_ON);
#ifdef MTK_AWS_MCE_ENABLE
        bt_gap_write_page_scan_activity(1024, 36);
#else
        bt_gap_write_page_scan_activity(2048, 36);
#endif
    }

    if (true == g_bt_cm_disconnect_all)  {
        /*bt_status_t ret = bt_connection_manager_disconnect_all(false);
    #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
        if (ret == BT_STATUS_SUCCESS && (1 == bt_connection_manager_state_machine_ext_is_in_role_resetup())) {
            bt_connection_manager_handle_power_off_cnf();
        }
    #endif*/
        bt_status_t ret = bt_connection_manager_disconnect_all(false);
        if (ret != BT_STATUS_SUCCESS && g_bt_cm_disconnect_all) {
            bt_cmgr_report_id("[BT_CM][I] Disconnect all not complete, need continue wait", 0);
            return;
        } else {
            bt_cmgr_report_id("[BT_CM][I] Disconnect all complete", 0);
        }
    }

    bt_connection_manager_discoverable_action_handle(false);
    if (true == abnormal_disconnect || BT_SINK_SRV_CM_REASON_CONNECTION_TIMEOUT == status ||
        BT_SINK_SRV_CM_REASON_ROLE_SWITCH_PENDING == status || BT_SINK_SRV_CM_REASON_LMP_RESPONSE_TIMEOUT == status) {
        bt_connection_manager_link_loss_reconnection((bt_bd_addr_t* )param->address, status);
    }
#ifdef MTK_AWS_MCE_ENABLE
    bt_connection_manager_enable_aws_discoverable(true);
#endif
}

static void bt_sink_srv_cm_handle_link_connected(bt_status_t status, const bt_gap_link_status_updated_ind_t *param)
{
    if (BT_STATUS_SUCCESS != status) {
        bt_sink_srv_cm_remote_device_t *device_p
            = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)param->address);
        if (NULL != device_p && (BT_SINK_SRV_CM_STATE_CONNECTING == device_p->link_state)) {
            BT_SINK_SRV_CM_STATE_CHANGE(device_p, BT_SINK_SRV_CM_STATE_DISCONNECTED);
            bt_connection_manager_free_device(device_p);
            bt_cmgr_report_id("[BT_CM][W] Create link failed, %x", 1, status);
            bt_connection_manager_request_done(BT_CONNECTION_MANAGER_STATUS_FAIL, g_sink_srv_cm_context.request_pending);
        } else if (NULL != device_p) {
            device_p->flag |= BT_SINK_SRV_CM_REMOTE_FLAG_CONNECTION_ABNORMAL;
        }
    } else {
        uint8_t allow_connected_num = BT_SINK_SRV_CM_MAX_DEVICE_NUMBER - 1;
        bt_connection_manager_link_connected((bt_bd_addr_t*)(param->address), param->handle, param->link_status);
        if (g_sink_srv_cm_context.connected_dev_num >= allow_connected_num) {
            // If the connected device num expired the max support connecte number, we need clear the request list.
            bt_sink_srv_cm_node_t *cur_node = g_sink_srv_cm_context.request_list.next;
            bt_sink_srv_cm_node_t *del_node = NULL;
            while (NULL != cur_node && cur_node != &g_sink_srv_cm_context.request_list) {
                del_node = cur_node;
                cur_node = cur_node->next;
                bt_connection_manager_node_remove(del_node);
                bt_connection_manager_node_delete(del_node);
            }
            // If the connected device is as same as the pending request device, we can't to clear the pending, because the sp may can't cconnect sink profile,
            // we will reconnect the request profile.
            cur_node = g_sink_srv_cm_context.request_pending;
            if (NULL != cur_node && bt_connection_manager_memcmp(param->address, &cur_node->parameters->action_param.address, sizeof(bt_bd_addr_t))) {
                bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR,
                    (void *)&cur_node->parameters->action_param.address);

                if (bt_connection_manager_timer_is_exist(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY)) {
                    bt_connection_manager_timer_stop(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY);
                }
                if(NULL != device_p && BT_SINK_SRV_CM_STATE_CONNECTING == device_p->link_state) {
                    bt_gap_cancel_connection((void *)&(cur_node->parameters->action_param.address));
                }
                bt_connection_manager_request_done(BT_CONNECTION_MANAGER_STATUS_USER_CANCEL, cur_node);
            }
        }
        //bt_connection_manager_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_NORMAL, NULL);
        //bt_connection_manager_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_POWER_ON, NULL);
        //bt_connection_manager_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_LINK_LOST, NULL);
        // To avoid controller issue: if one link is paging, another link role switch will fail,
        // request done when all profiles are connected.
        //if (0 == bt_connection_manager_memcmp(param->address, &g_sink_srv_cm_context.request_pending->parameters->action_param.bt_addr, sizeof(bt_bd_addr_t))) {
        //    bt_connection_manager_request_done(BT_STATUS_SUCCESS, g_sink_srv_cm_context.request_pending);
        //}
        /*If the link is connected  due to link lost reconnect, and the reconnection is still running, we need finish this process. */
        /*if (NULL != g_sink_srv_cm_context.request_pending &&
            0 == bt_connection_manager_memcmp(param->address, &g_sink_srv_cm_context.request_pending->parameters->action_param.address, sizeof(bt_bd_addr_t))) {
            bt_connection_manager_timer_stop(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY);
            bt_connection_manager_request_done(BT_STATUS_SUCCESS, g_sink_srv_cm_context.request_pending);
        }*/
    }
}

bt_bd_addr_t *bt_sink_srv_cm_last_connected_device(void)
{
    return &g_sink_srv_cm_last_connected;
}

void bt_connection_manager_stop_switch_slave_retry(void)
{
    bt_cmgr_report_id("[BT_CM][W] Warning stop slave switch retry !!!", 0);
    bt_sink_srv_role_switch_retry_times = 0;
    g_bt_cm_need_switch_slave = false;
}

static void bt_connection_manager_switch_slave_timer_callback(void *params)
{
    bt_sink_srv_cm_remote_device_t *device_p = (bt_sink_srv_cm_remote_device_t *)params;
    if (NULL == device_p) {
        bt_cmgr_report_id("[BT_CM][E] Switch slave timer callback device is null", 0);
        return;
    }
    bt_connection_manager_set_role(&(device_p->addr), BT_ROLE_SLAVE, false);
}

static void bt_connection_manager_gap_role_event_handler(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_sink_srv_cm_remote_device_t *device_p = NULL;
    bt_cmgr_report_id("[BT_CM][I] Gap role event:0x%x, status:0x%x", 2, msg, status);
    if (BT_GAP_GET_ROLE_CNF == msg) {
        bt_gap_get_role_cnf_t *get_role = (bt_gap_get_role_cnf_t *)buffer;
        device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_HANDLE, &(get_role->handle));
        if (device_p) {
            bt_cmgr_report_id("[BT_CM][I] Get role cnf: 0x%x", 1, device_p->local_role);
            device_p->local_role = get_role->local_role;
        }
        return;
    } else if (BT_GAP_SET_ROLE_CNF == msg) {
        if (BT_STATUS_SUCCESS == status) {
    #ifdef MTK_AWS_MCE_ENABLE
            bt_sink_srv_cm_set_aws_enable_t set_aws_state = BT_SINK_SRV_CM_SET_AWS_ENABLE_PENDING_TRUE;
            device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_SET_AWS_STATE_TYPE, &set_aws_state);
            if (NULL == device_p) {
                set_aws_state = BT_SINK_SRV_CM_SET_AWS_ENABLE_PENDING_FALSE;
                device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_SET_AWS_STATE_TYPE, &set_aws_state);
            }
            if (NULL != device_p) {
                bt_connection_manager_enable_aws_discoverable(BT_SINK_SRV_CM_SET_AWS_ENABLE_PENDING_TRUE == device_p->set_aws_state ? true : false);
            }
    #endif
            return;
        } else {
            /* If status not success, role change event will not recevie, need clear peing flag. */
            for (uint8_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
                if (g_sink_srv_cm_context.devices[i].flag & BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING) {
                    device_p = &g_sink_srv_cm_context.devices[i];
                    break;
                }
            }
            if (NULL == device_p) {
                bt_cmgr_report_id("[BT_CM][E] Not find switching device !!!", 0);
                return;
            }
            device_p->flag &= ~BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING;
        }
    } else if (BT_GAP_ROLE_CHANGED_IND == msg) {
        bt_gap_role_changed_ind_t *role_change = (bt_gap_role_changed_ind_t *)buffer;
        if (NULL == role_change || NULL == (device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_HANDLE, &(role_change->handle)))) {
            bt_cmgr_report_id("[BT_CM][E] Not find device by handle !!!", 0);
            return;
        }
        bt_cmgr_report_id("[BT_CM][I] Changed:0x%x,local:0x%x", 2, role_change->local_role, device_p->local_role);
        bt_connection_manager_timer_stop(BT_SINK_SRV_CM_DELAY_TO_SWITCH_ROLE_TIMER_ID);
        device_p->flag &= ~BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING;
        device_p->local_role = role_change->local_role;
        if (BT_STATUS_SUCCESS == status) {
            bt_cmgr_report_id("[BT_CM][I]device_p flag:0x%x", 1, device_p->flag);
            bt_sink_srv_role_switch_retry_times = bt_sink_srv_get_role_switch_retry_times();
            if (device_p->flag & BT_SINK_SRV_CM_REMOTE_FLAG_DISCON_ESCO) {
                device_p->flag &= ~BT_SINK_SRV_CM_REMOTE_FLAG_DISCON_ESCO;
                bt_sink_srv_send_action(BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH, NULL);
            }
            /*Notify role changed ind to application.*/
            bt_sink_srv_gap_role_information_t *event;
            if (NULL != (event = bt_connection_manager_memory_alloc(sizeof(*event)))) {
                event->role = device_p->local_role;
                bt_connection_manager_memcpy(&event->address, (void *)&device_p->addr, sizeof(bt_bd_addr_t));
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_ROLE_CHANGED_IND, (void *)event, sizeof(*event));
                bt_connection_manager_memory_free(event);
            }
        #ifdef MTK_AWS_MCE_ENABLE
            if ((device_p->flag & BT_SINK_SRV_CM_REMOTE_FLAG_NEED_AWS_DISCOVERABLE) && (device_p->local_role & BT_ROLE_SLAVE)) {
                bt_connection_manager_enable_aws_discoverable(true);
            }
        #endif
            return;
        }
    }
    if (NULL != device_p && !(device_p->local_role & BT_ROLE_SLAVE) && true == g_bt_cm_need_switch_slave) {
        bt_cmgr_report_id("[BT_CM][I]Get flag: 0x%x", 1,device_p->flag);
        if (bt_sink_srv_role_switch_retry_times) {
            if (0xFF != bt_sink_srv_role_switch_retry_times) {
                bt_sink_srv_role_switch_retry_times--;
            }
            if (device_p->flag & BT_SINK_SRV_CM_REMOTE_FLAG_ESCO_CONNECTED) {
                device_p->flag |= BT_SINK_SRV_CM_REMOTE_FLAG_DISCON_ESCO;
                bt_sink_srv_send_action(BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH, NULL);
            }
            bt_connection_manager_timer_start(BT_SINK_SRV_CM_DELAY_TO_SWITCH_ROLE_TIMER_ID, 100,
                                            bt_connection_manager_switch_slave_timer_callback,
                                            (void *)device_p);
        }
        /*else {
            bt_connection_manager_assert(0 && "Switch role retry fail !!!");
        }*/
    }
}

void bt_connection_manager_set_role(bt_bd_addr_t *address_p, bt_role_t role, bool force)
{
    bt_sink_srv_cm_remote_device_t *device_p =
            bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)address_p);
    if (NULL == device_p) {
        bt_connection_manager_assert(0 && "Set role can't find the remote device !!!");
        return;
    }
    bt_role_t gap_role = (device_p->local_role & BT_SINK_SRV_ROLE_MASK_GAP);
    bt_cmgr_report_id("[BT_CM][I] Set role:%d, cur role:%d, link_state:%d, force:%d, flag:0x%02x", 5,
        role, gap_role, device_p->link_state, force, device_p->flag);
    if ((device_p->flag & BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING) || (gap_role == role) ||
            (device_p->link_state <= BT_SINK_SRV_CM_STATE_CONNECTED && false == force) || false == g_bt_cm_need_switch_slave) {
        return;
    }
    bt_status_t ret = bt_gap_set_role(device_p->handle, role);
    if (BT_STATUS_SUCCESS == ret) {
        device_p->flag |= BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING;
    } else {
        bt_cmgr_report_id("[BT_CM][E] Set role fail:0x%x", 1, ret);
    }
}

bt_role_t bt_connection_manager_get_gap_role(bt_bd_addr_t *address_p)
{
    bt_role_t gap_role = BT_SINK_SRV_ROLE_UNKNOWN;
    bt_sink_srv_cm_remote_device_t *device_p =
        bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)address_p);
    if (NULL != device_p) {
        gap_role = (device_p->local_role & BT_SINK_SRV_ROLE_MASK_GAP);
    }
    bt_cmgr_report_id("[BT_CM][I] Get gap role:0x%x", 1, gap_role);
    return gap_role;
}

bt_gap_connection_handle_t bt_sink_srv_cm_get_gap_handle(bt_bd_addr_t *address_p)
{
    bt_gap_connection_handle_t gap_handle = 0;
    bt_sink_srv_cm_remote_device_t *device_p =
        bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)address_p);
    if (NULL != device_p) {
        gap_handle =  device_p->handle;
    }
    bt_cmgr_report_id("[BT_CM][I] Get gap handle:0x%x", 1, gap_handle);
    return gap_handle;
}

bt_bd_addr_t *bt_connection_mananger_get_addr_by_gap_handle(bt_gap_connection_handle_t handle)
{
    bt_sink_srv_cm_remote_device_t *device_p =
            bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_HANDLE, &(handle));
    bt_cmgr_report_id("[BT_CM][I] Get addr by handle : 0x%x", 1, handle);
    if (NULL != device_p) {
        return &(device_p->addr);
    }
    return NULL;
}

static void bt_sink_srv_cm_handle_link_updata_ind(bt_status_t status, bt_gap_link_status_updated_ind_t *param)
{
    bt_cmgr_report_id("[BT_CM][I] Link staus:0x%x", 1, param->link_status);

    if (BT_GAP_LINK_STATUS_CONNECTED_0 <= param->link_status) {
        bt_sink_srv_cm_handle_link_connected(status, param);
    } else if (BT_GAP_LINK_STATUS_DISCONNECTED == param->link_status) {
        bt_sink_srv_cm_handle_link_disconnected(status, param);
    }
}

static void bt_connection_manager_discoverable_action_handle(bool enable)
{
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
        if (true == enable && NULL == bt_sink_srv_cm_get_aws_connected_device()) {
    #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
            bt_cmgr_report_id("[BT_CM][I] Discoverable with partner role, switch to agent firstly !!!", 0);
            g_bt_cm_power_on_open_discoverable = true;
            bt_connection_manager_state_machine_ext_trigger_role_change(BT_AWS_MCE_ROLE_AGENT);
        } else {
    #endif
            bt_cmgr_report_id("[BT_CM][I] Partner no need to exit discoverable mode", 0);
        }
        return;
    }
    #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
    else if (enable == true) {
        bt_connection_manager_state_machine_ext_stop_role_change();
    }
    #endif
#endif
    bt_connection_manager_mutex_lock();
    if (true == enable) {
        if (g_sink_srv_cm_context.connected_dev_num >= BT_SINK_SRV_CM_MAX_DEVICE_NUMBER) {
            bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY);
        } else {
            bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);
        }
    } else {
        if (g_sink_srv_cm_context.connected_dev_num < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER) {
            bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
        } else {
            bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);
        }
    }
    bt_connection_manager_mutex_unlock();
}

bt_status_t bt_sink_srv_cm_action_handler(bt_sink_srv_action_t action, void *param)
{
    bt_cmgr_report_id("[BT_CM][I] bt_sink_srv_cm_action_handler, action:%x", 1, action);
    bt_status_t status = BT_STATUS_SUCCESS;
#ifdef MTK_AWS_MCE_ENABLE
    if (true == bt_connection_manager_aws_pair_pairing_is_ongoing() && BT_SINK_SRV_ACTION_STOP_AIR_PAIRING != action) {
        bt_cmgr_report_id("[BT_CM][W] On air pairing state forbit action excute !!!", 0);
        return BT_CONNECTION_MANAGER_STATUS_FAIL;
    }
#endif
    if (bt_connection_manager_power_get_state() != BT_CM_POWER_STATE_ON &&
            !(BT_SINK_SRV_ACTION_RESET_TRUSTED_LIST == action ||
              BT_SINK_SRV_ACTION_SWITCH_MODE == action ||
              BT_SINK_SRV_ACTION_BT_STANDBY == action ||
              BT_SINK_SRV_ACTION_BT_ACTIVE == action ||
              BT_SINK_SRV_ACTION_CLASSIC_BT_STANDBY == action ||
              BT_SINK_SRV_ACTION_BT_RESET == action)) {
        bt_cmgr_report_id("[BT_CM][W] Not in power on state", 0);
        return BT_CONNECTION_MANAGER_STATUS_FAIL;
    }
    switch (action) {
        case BT_SINK_SRV_ACTION_CONNECT:
            status = bt_connection_manager_connect_action_handle((bt_bd_addr_t *)param);
            break;
        case BT_SINK_SRV_ACTION_DISCONNECT:
            bt_connection_manager_disconnect_action_handle((bt_bd_addr_t *)param);
            break;
        case BT_SINK_SRV_ACTION_DISCOVERABLE:
            bt_connection_manager_discoverable_action_handle(true);
            break;
        case BT_SINK_SRV_ACTION_EXIT_DISCOVERABLE:
            bt_connection_manager_discoverable_action_handle(false);
            break;
        case BT_SINK_SRV_ACTION_START_RECONNECT:
            status = bt_connection_manager_power_on_reconnection();
            break;
        case BT_SINK_SRV_ACTION_RESET_TRUSTED_LIST:
            status = bt_device_manager_unpair_all();
            break;
        case BT_SINK_SRV_ACTION_BT_STANDBY:
            status = bt_connection_manager_power_set_target_state(BT_CM_POWER_STATE_OFF);
            break;
        case BT_SINK_SRV_ACTION_BT_ACTIVE:
            status = bt_connection_manager_power_set_target_state(BT_CM_POWER_STATE_ON);
            break;
        case BT_SINK_SRV_ACTION_BT_RESET:
            status =  bt_connection_manager_power_reset(false);
            break;
        case BT_SINK_SRV_ACTION_CANCEL_CONNECT: {
           // if(NULL != param) {
                bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)param);
                if(NULL != device_p && device_p->link_state == BT_SINK_SRV_CM_STATE_CONNECTING) {
                    bt_gap_cancel_connection(param);
                } else {
                    bt_connection_manager_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_POWER_ON, NULL);
                    bt_connection_manager_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_LINK_LOST, NULL);
                }
           // }
        }
            break;
    #ifdef MTK_AWS_MCE_ENABLE
        case BT_SINK_SRV_ACTION_START_AIR_PAIRING:
            status = bt_connection_manager_aws_air_pairing_start(param);
        #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
            if (status == BT_STATUS_SUCCESS) {
                bt_connection_manager_state_machine_ext_stop_role_change();
            }
        #endif
            break;
        case BT_SINK_SRV_ACTION_STOP_AIR_PAIRING:
            status = bt_connection_manager_aws_air_pairing_cancel();
            break;
    #endif
    #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
        case BT_SINK_SRV_ACTION_NOTIFY_OUT_OF_CASE:
        case BT_SINK_SRV_ACTION_NOTIFY_IN_CASE:
            bt_connection_manager_state_machine_ext_action_handler(action, param);
            break;
    #endif
        case BT_SINK_SRV_ACTION_SWITCH_MODE: {
            bt_connection_manager_switch_mode_params_t *reload_param = (bt_connection_manager_switch_mode_params_t *)param;
            if (NULL == reload_param) {
                bt_cmgr_report_id("[BT_CM][I] N9 firmware reload param is null", 0);
                return BT_SINK_SRV_STATUS_INVALID_PARAM;
            }
            if (!g_bt_cm_reload_n9_flag) {
                g_bt_cm_reload_n9_flag = true;
                bt_connection_manager_memcpy((void *)&g_n9_fw_reload_context, reload_param, sizeof(bt_connection_manager_switch_mode_params_t));
                bt_sink_srv_send_action(BT_SINK_SRV_ACTION_BT_STANDBY, NULL);
            } else {
                bt_cmgr_report_id("[BT_CM][I] Mode switching.", 0);
                return BT_SINK_SRV_STATUS_INVALID_STATUS;
            }
        }
            break;
        case BT_SINK_SRV_ACTION_CLASSIC_BT_STANDBY: {
     #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
            //bt_connection_manager_state_machine_ext_power_onoff_notify(0);
            bt_connection_manager_state_ext_deinit();
     #endif
            bt_connection_manager_power_set_target_state(BT_CM_POWER_STATE_CLASSIC_BT_OFF);
        }
            break;

        case BT_SINK_SRV_ACTION_ROLE_CHANGE: {
#ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
            bt_aws_mce_role_t role = *(bt_aws_mce_role_t*)param;
            bt_cmgr_report_id("[BT_CM][I] app request switch role to 0x%x", 1, role);
            bt_connection_manager_state_machine_ext_trigger_role_change(role);
#endif
        }
        break;
        default:
            break;
    }

    return status;
}

static void bt_sink_srv_cm_handle_bt_panic(void)
{
#if (PRODUCT_VERSION == 2523 || PRODUCT_VERSION == 2533)
#ifdef HAL_WDT_MODULE_ENABLED
    hal_wdt_config_t wdt_config;
    wdt_config.mode = HAL_WDT_MODE_RESET;
    wdt_config.seconds = 1;
    hal_wdt_init(&wdt_config);
    hal_wdt_software_reset();
#endif
#endif
    return;
}

bt_status_t bt_sink_srv_cm_system_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
#if defined(__BT_SINK_SRV_DEBUG_INFO__ )
    if (msg >= BT_POWER_ON_CNF && msg <= BT_PANIC) {
        bt_cmgr_report("[BT_CM][I] SDK msg:%s", g_system_event_string[msg - BT_POWER_ON_CNF]);
    } else {
        bt_cmgr_report_id("[BT_CM][I] SDK msg:0x%x", 1, msg);
    }
#else
    bt_cmgr_report_id("[BT_CM][I] SDK msg:0x%x", 1, msg);
#endif /* defined(__BT_SINK_SRV_DEBUG_INFO__ ) */
    switch (msg) {
        case BT_POWER_ON_CNF: {
            bt_connection_manager_power_event_handle(msg, status, buffer);
            bt_connection_manager_handle_power_on_cnf();
        }
            break;
        case BT_POWER_OFF_CNF:
            bt_connection_manager_handle_power_off_cnf();
            bt_device_manager_db_flush_all(BT_DEVICE_MANAGER_DB_FLUSH_BLOCK);
            bt_connection_manager_power_event_handle(msg, status, buffer);
            break;
        case BT_PANIC:
           bt_sink_srv_cm_handle_bt_panic();
           break;
        case BT_DUT_MODE_ACTIVE_IND:
            bt_cmgr_report_id("[BT_CM][I] BT enter DUT mode", 0);
            bt_connection_manager_power_event_handle(msg, status, buffer);
           break;
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_connection_manager_sdp_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    bt_cmgr_report_id("[BT_CM][I] bt_connection_manager_sdp_callback, msg:0x%x", 1, msg);
    switch (msg) {
        case BT_SDPC_SEARCH_SERVICE_CNF: {
            bt_sdpc_service_cnf_t *service_result = (bt_sdpc_service_cnf_t *)buffer;
            bt_sink_srv_cm_remote_device_t *device_p = NULL;
            if (NULL != service_result) {
                device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)service_result->user_data);
            }
            bt_cmgr_report_id("[BT_CM][I] BT_SDPC_SEARCH_SERVICE_CNF", 0);
            if (NULL != service_result && NULL != device_p) {
                if (0 == service_result->handle_number) {
                    bt_device_manager_db_remote_pnp_info_t pnp_info = {
                        .vender_id = 0,
                        .product_id = 0
                    };
                    bt_device_manager_remote_update_pnp_info(device_p->addr, &pnp_info);
                    bt_cmgr_report_id("[BT_CM][I]bt_sdpc_search_no_result", 0);
                } else {
                    uint32_t query_service_handle = bt_sdp_get_32bit(service_result->handle_list);
                    bt_sdpc_attribute_request_t request;
                    request.address = (void *)&device_p->addr;
                    request.search_handle = query_service_handle;
                    request.attribute_pattern = bt_sink_srv_connection_manager_search_di_attributes;
                    request.pattern_length = sizeof(bt_sink_srv_connection_manager_search_di_attributes);
                    request.user_data = &device_p->addr;
                    bt_status_t return_result = bt_sdpc_search_attribute(&request);
                    bt_cmgr_report_id("[BT_CM][I]bt_sdpc_search_attribute-return_result: %d", 1, return_result);
                }
            }
            break;
        }
        case BT_SDPC_SEARCH_ATTRIBUTE_CNF: {
            bt_sdpc_attribute_cnf_t *attr_result = (bt_sdpc_attribute_cnf_t *)buffer;
            uint8_t *parse_result;
            uint16_t result_len;
            uint8_t *data;
            uint16_t data_len = 0;

            bt_cmgr_report_id("[BT_CM][I] BT_SDPC_SEARCH_ATTRIBUTE_CNF", 0);
            bt_device_manager_db_remote_pnp_info_t pnp_info = {
                .vender_id = 0,
                .product_id = 0
            };
            bt_sdpc_parse_attribute(&parse_result, &result_len, BT_DI_SDP_ATTRIBUTE_VENDOR_ID, 0, attr_result->length, attr_result->attribute_data);
            bt_sdpc_parse_next_value(&data, &data_len, parse_result, result_len);
            if (data_len) {
                pnp_info.vender_id = (data[0] << 8 | data[1]);
                bt_cmgr_report_id("[BT_CM][I] vender_id:0x%04x", 1, pnp_info.vender_id);
            }
            bt_sdpc_parse_attribute(&parse_result, &result_len, BT_DI_SDP_ATTRIBUTE_PRODUCT_ID, 0, attr_result->length, attr_result->attribute_data);
            bt_sdpc_parse_next_value(&data, &data_len, parse_result, result_len);
            if (data_len) {
                pnp_info.product_id= (data[0] << 8 | data[1]);
                bt_cmgr_report_id("[BT_CM][I] product_id:0x%04x", 1, pnp_info.product_id);
            }
            #ifdef MTK_AP_DEFAULT_TYPE
            bt_sdpc_parse_attribute(&parse_result, &result_len, BT_DI_SDP_ATTRIBUTE_VERSION, 0, attr_result->length, attr_result->attribute_data);
            bt_sdpc_parse_next_value(&data, &data_len, parse_result, result_len);
            if (data_len) {
                pnp_info.version = (data[0] << 8 | data[1]);
                bt_cmgr_report_id("[BT_CM][I] version:0x%04x", 1, pnp_info.version);
            }
            #endif
            bt_device_manager_remote_update_pnp_info((void *)(attr_result->user_data), &pnp_info);
            break;
        }
        case BT_SDPC_SEARCH_ERROR: {
            bt_cmgr_report_id("[BT_CM][I] BT_SDPC_SEARCH_ERROR", 0);
            break;
        }
        default:
            bt_cmgr_report_id("[BT_CM][I] Unexcepted msg:%x", 1, msg);
            break;
    }
    return result;
}

bt_status_t bt_sink_srv_cm_gap_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    bt_cmgr_report_id("[BT_CM][I] bt_sink_srv_cm_gap_callback, msg:0x%08x, status:0x%08x", 2, msg, status);

#if defined(__BT_SINK_SRV_DEBUG_INFO__)
    if (msg == BT_GAP_LINK_STATUS_UPDATED_IND ) {
        bt_cmgr_report("[BT_CM][I] SDK msg:%s", g_gap_event_string[0]);
    } else if (msg >= BT_GAP_IO_CAPABILITY_REQ_IND && msg <= BT_GAP_READ_REMOTE_VERSION_COMPLETE_IND) {
        bt_cmgr_report("[BT_CM][I] SDK msg:%s", g_gap_event_string[msg - BT_GAP_IO_CAPABILITY_REQ_IND + 1]);
    } else {
        bt_cmgr_report_id("[BT_CM][I] SDK msg:0x%x", 1, msg);
    }
#else
    bt_cmgr_report_id("[BT_CM][I] SDK msg:0x%x", 1, msg);
#endif /* __BT_SINK_SRV_DEBUG_INFO__ */

    switch (msg) {
        case BT_GAP_CANCEL_CONNECTION_CNF: {
            bt_cmgr_report_id("[BT_CM][I] Cancel connect confirm status 0x%x", 1, status);
            bt_gap_cancel_connection_cnf_t *cancel_addr = (bt_gap_cancel_connection_cnf_t *)buffer;
            bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
            if ((BT_AWS_MCE_ROLE_AGENT & aws_role) && BT_STATUS_SUCCESS != status) {
                break;
            } else if (BT_STATUS_SUCCESS == status || BT_HCI_STATUS_UNKNOWN_CONNECTION_IDENTIFIER == status) {
                bt_sink_srv_cm_remote_device_t *disconn_device =
                    bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)&(cancel_addr->address));
                if (NULL != disconn_device && disconn_device->link_state == BT_SINK_SRV_CM_STATE_CONNECTING) {
                    if (NULL != g_sink_srv_cm_context.request_pending
                            && !bt_connection_manager_memcmp(disconn_device->addr,
                            &g_sink_srv_cm_context.request_pending->parameters->action_param.address, sizeof(bt_bd_addr_t))) {
                        bt_connection_manager_request_done(BT_CONNECTION_MANAGER_STATUS_USER_CANCEL, g_sink_srv_cm_context.request_pending);
                    }
                    bt_connection_manager_free_device(disconn_device);
                }
                if (BT_CM_POWER_STATE_OFF_PENDING == bt_connection_manager_power_get_state()) {
                    bt_connection_manager_power_off(false);
                } else if (true == g_bt_cm_disconnect_all) {
                    bt_connection_manager_disconnect_all(false);
                    /*bt_status_t ret = bt_connection_manager_disconnect_all(false);
                #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
                    if (ret == BT_STATUS_SUCCESS && (1 == bt_connection_manager_state_machine_ext_is_in_role_resetup())) {
                        bt_connection_manager_handle_power_off_cnf();
                    }
                #endif*/
                }
            }
        }
            break;
        case BT_GAP_READ_REMOTE_NAME_COMPLETE_IND: {
            bt_gap_read_remote_name_complete_ind_t* p = (bt_gap_read_remote_name_complete_ind_t*)buffer;
            if (NULL == p || BT_STATUS_SUCCESS != status) {
                bt_cmgr_report_id("[BT_CM][W] read remote name complete ind buff cnf status : 0x%0x", 1, status);
                break;
            } else {
                bt_gap_connection_handle_t gap_handle = bt_sink_srv_cm_get_gap_handle((bt_bd_addr_t *)(p->address));
                status = bt_gap_read_remote_version_information(gap_handle);
                if (status != BT_STATUS_SUCCESS) {
                    bt_cmgr_report_id("[BT_CM][W] read remote version fail status 0x%x", 1, status);
                }
            }
        }
            break;
        case BT_GAP_READ_REMOTE_VERSION_COMPLETE_IND: {
            bt_gap_read_remote_version_complete_ind_t *complete_ind = (bt_gap_read_remote_version_complete_ind_t *)buffer;
            bt_device_manager_db_remote_version_info_t version_info;
            bt_bd_addr_t *remote_device = NULL;
            if (BT_STATUS_SUCCESS != status || NULL == complete_ind) {
                bt_cmgr_report_id("[BT_CM][W] read remote version complete ind fail status 0x%x complete_ind %p", 2, status, complete_ind);
                break;
            } else if (NULL == (remote_device = bt_connection_mananger_get_addr_by_gap_handle(complete_ind->handle))) {
                bt_cmgr_report_id("[BT_CM][E] read remote version can't find device by handle 0x%x status 0x%x", 2, complete_ind->handle, status);
                break;
            }
            version_info.version = complete_ind->version;
            version_info.manufacturer_id = complete_ind->manufacturer_id;
            version_info.subversion = complete_ind->subversion;
            bt_device_manager_remote_update_version_info(*remote_device, &version_info);
        }
            break;
        case BT_GAP_LINK_STATUS_UPDATED_IND:
            bt_sink_srv_cm_handle_link_updata_ind(status, (bt_gap_link_status_updated_ind_t *)buffer);
            break;
        case BT_GAP_SET_SCAN_MODE_CNF: {
        #ifdef MTK_AWS_MCE_ENABLE
            if(BT_STATUS_SUCCESS == bt_connection_manager_aws_air_pairing_event_handle(msg, status, buffer)) {
                break;
            }
        #endif
            bt_connection_manager_reset_flags(BT_SINK_SRV_FLAG_SET_SCAN_MODE);
            bt_sink_srv_visibility_state_update_t event;
            if (g_sink_srv_cm_context.scan_mode == BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE ||
                g_sink_srv_cm_context.scan_mode == BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY) {
                bt_cmgr_report_id("[BT_CM][I] Enter discoverable mode", 0);
                event.visibility_on = true;
            } else {
                event.visibility_on = false;
            }
            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_VISIBILITY_STATE_UPDATE, (void *)(&event), sizeof(event));
            if (BT_CM_POWER_STATE_OFF_PENDING == bt_connection_manager_power_get_state()) {
                bt_connection_manager_write_scan_enable_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);
            }
        }
            break;
        case BT_GAP_GET_ROLE_CNF:
        case BT_GAP_SET_ROLE_CNF:
        case BT_GAP_ROLE_CHANGED_IND:
            bt_connection_manager_gap_role_event_handler(msg, status, buffer);
            break;
        case BT_GAP_SNIFF_MODE_CHANGE_IND: {
            bt_gap_sniff_mode_changed_ind_t* ind = (bt_gap_sniff_mode_changed_ind_t *)buffer;
            bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_HANDLE, &(ind->handle));
            if (NULL == device_p) {
                bt_cmgr_report_id("[BT_CM][E] Can't find the device", 0);
                break;
            }
            device_p->sniff_state = ind->sniff_status;
        #ifdef MTK_AWS_MCE_ENABLE
            if (BT_GAP_LINK_SNIFF_TYPE_ACTIVE == device_p->sniff_state &&
                (device_p->flag & BT_SINK_SRV_CM_REMOTE_FLAG_NEED_AWS_DISCOVERABLE)) {
                bt_connection_manager_enable_aws_discoverable(true);
            }
        #ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            if (BT_GAP_LINK_SNIFF_TYPE_ACTIVE == device_p->sniff_state &&
                BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state() &&
                !(device_p->flag & BT_SINK_SRV_CM_REMOTE_FLAG_DISABLE_SNIFF_PENDING)) {
                bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_SINK_CM);
            }
        #endif
        #endif
            bt_connection_manager_sniff_mode_update_t event;
            bt_connection_manager_memcpy((void *)(event.address), (void *)(device_p->addr), sizeof(bt_bd_addr_t));
            event.sniff_mode = ind->sniff_status;
            bt_sink_srv_event_callback(BT_CONNECTION_MANAGER_EVENT_SNIFF_MODE_UPDATE, (void *)(&event), sizeof(event));
            break;
        }
        case BT_GAP_WRITE_LINK_POLICY_CNF: {
        #ifdef MTK_AWS_MCE_ENABLE
        #ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            bt_gap_write_link_policy_cnf_t *cnf = (bt_gap_write_link_policy_cnf_t *)buffer;
            bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_HANDLE, &(cnf->handle));
            if (NULL == device_p) {
                bt_cmgr_report_id("[BT_CM][E] Write link policy can't find the device", 0);
                break;
            }
            device_p->flag &= ~(BT_SINK_SRV_CM_REMOTE_FLAG_DISABLE_SNIFF_PENDING);
            if (BT_GAP_LINK_SNIFF_TYPE_ACTIVE == device_p->sniff_state &&
                BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state()) {
                bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_SINK_CM);
            }
        #endif
        #endif
            break;
        }
        case BT_GAP_INQUIRY_IND: {
        #ifdef MTK_AWS_MCE_ENABLE
            bt_connection_manager_aws_air_pairing_event_handle(msg, status, buffer);
        #endif
        }
        break;

        case BT_GAP_WRITE_STORED_LINK_KEY_CNF: {
            //bt_gap_write_stored_link_key_cnf_t *write_key_cnf = (bt_gap_write_stored_link_key_cnf_t *)buffer;
            if (BT_SINK_SRV_FLAG_AWS_POWERING_ON & bt_connection_manager_get_flags()) {
                bt_connection_manager_write_aws_page_scan();
            }
        }
            break;
        case BT_GAP_WRITING_PAGE_SCAN_CNF: {
            if (BT_SINK_SRV_FLAG_AWS_POWERING_ON & bt_connection_manager_get_flags()) {
                bt_connection_manager_power_on_create_aws_link();
            }
        }
            break;
        case BT_GAP_LE_CONNECT_IND: {
            if (status == BT_STATUS_SUCCESS) {
                bt_cmgr_report_id("[BT_CM][I] BLE connected", 0);
                g_sink_srv_cm_context.le_connected_dev_num++;
                if (g_sink_srv_cm_context.le_connected_dev_num == 1) {
                #ifdef MTK_AWS_MCE_ENABLE
                    bt_gap_write_page_scan_activity(1024, 18);
                #else
                    bt_gap_write_page_scan_activity(2048, 18);
                #endif
                }
            }
        }
            break;
        case BT_GAP_LE_DISCONNECT_IND: {
            bt_cmgr_report_id("[BT_CM][I] BLE disconnected", 0);
            g_sink_srv_cm_context.le_connected_dev_num--;
            if (g_sink_srv_cm_context.le_connected_dev_num == 0 &&
                g_sink_srv_cm_context.connected_dev_num == 0) {
            #ifdef MTK_AWS_MCE_ENABLE
                bt_gap_write_page_scan_activity(1024, 36);
            #else
                bt_gap_write_page_scan_activity(2048, 36);
            #endif
            }
        }
            break;
        case BT_GAP_READ_RSSI_CNF: {
            const bt_gap_read_rssi_cnf_t *standard_rssi = (bt_gap_read_rssi_cnf_t *)buffer;
            bt_cmgr_report_id("[BT_CM][I] rssi=%ddBm, handle %x ", 2, standard_rssi->rssi, standard_rssi->handle);
        }
            break;
        case BT_GAP_READ_RAW_RSSI_CNF: {
            const bt_gap_read_rssi_cnf_t *raw_rssi = (bt_gap_read_rssi_cnf_t *)buffer;
            bt_cmgr_report_id("[BT_CM][I] raw rssi=%ddBm, handle %x ", 2, raw_rssi->rssi, raw_rssi->handle);
        }
            break;
        case BT_GAP_SET_EIR_CNF: {
        #ifdef MTK_AWS_MCE_ENABLE
            if(BT_STATUS_SUCCESS == bt_connection_manager_aws_air_pairing_event_handle(msg, status, buffer)) {
                break;
			}
        #endif
            uint16_t page_timeout = bt_sink_srv_get_page_timeout_paramters();
            bt_gap_write_page_timeout(page_timeout);
        }
            break;
        case BT_GAP_ESCO_CONNECTION_PENDING_IND: {
            bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_HANDLE, &buffer);
            bt_cmgr_report_id("[BT_CM][I] BT_GAP_ESCO_CONNECTION_PENDING_IND status %d, device %p", 2, status, device_p);
            if (NULL != device_p) {
                device_p->flag |= BT_SINK_SRV_CM_REMOTE_FLAG_DISCON_ESCO;
            }
        }
            break;
        case BT_GAP_WRITE_INQUIRY_ACCESS_CODE_CNF: {
        #ifdef MTK_AWS_MCE_ENABLE
            bt_connection_manager_aws_air_pairing_event_handle(msg, status, buffer);
        #endif
        }
            break;
        case BT_GAP_ENCRYPTION_KEY_REFRESH_COMPLETE_IND: {
        #ifdef MTK_AWS_MCE_ENABLE
            for (uint32_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
                bt_sink_srv_cm_remote_device_t *device_p = &(g_sink_srv_cm_context.devices[i]);
                if (BT_AWS_MCE_AGENT_STATE_CONNECTABLE == device_p->aws_state) {
                    bt_connection_manager_enable_aws_discoverable(false);
                }
            }
        #endif
        }
            break;
        case BT_SDPC_SEARCH_SERVICE_CNF:
        case BT_SDPC_SEARCH_ATTRIBUTE_CNF:
        case BT_SDPC_SEARCH_ERROR: {
            result = bt_connection_manager_sdp_callback(msg, status, buffer);
        }
            break;
        case BT_GAP_ENTER_TEST_MODE_CNF:
            g_bt_cm_dut_enable = true;
            break;
        default:
            bt_cmgr_report_id("[BT_CM][I] Unexcepted msg:%x", 1, msg);
            break;
    }
    return result;
}

uint8_t bt_sink_srv_get_connected_devices_number(void)
{
    uint32_t count = 0;
    bt_bd_addr_t* local_addr = bt_connection_manager_device_local_info_get_local_address();
    for (uint32_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        bt_sink_srv_cm_remote_device_t *device_p = &(g_sink_srv_cm_context.devices[i]);
        if ((BT_SINK_SRV_CM_STATE_CONNECTED <= device_p->link_state) &&
            bt_connection_manager_memcmp(local_addr, &(device_p->addr), sizeof(bt_bd_addr_t))) {
            count++;
        }
    }
    bt_cmgr_report_id("[BT_CM][I] Connected count:%d", 1, count);
    return count;
}

void bt_sink_paird_list_changed(bt_device_manager_paired_event_t event, bt_bd_addr_ptr_t address)
{
    return;
}

bt_sink_srv_profile_type_t bt_sink_srv_cm_get_connected_profiles(bt_bd_addr_t *address)
{
    bt_sink_srv_profile_type_t connected_profiles = BT_SINK_SRV_PROFILE_NONE;
    if (NULL != address) {
        bt_sink_srv_cm_remote_device_t *device_p =
            bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)address);
        if (NULL != device_p) {
            connected_profiles =  device_p->profile_connection_mask;
        }
    }
    bt_cmgr_report_id("[BT_CM][I]Get connected profiles:0x%x", 1, connected_profiles);
    return connected_profiles;
}

void bt_connection_manager_atci_init(void);

void bt_connection_manager_init(bt_connection_manager_features_config_t *features)
{
    bt_cmgr_report_id("[BT_CM]bt_connection_manager_init", 0);
    // initialize ATCI
    bt_connection_manager_atci_init();
    bt_connection_manager_config_features(features);
    // initialize timer
    bt_connection_manager_timer_init();
    //bt_sink_srv_init_aws_peer_address();
    // init disconnect nofitication callback.
}

bt_status_t bt_connection_manager_exit_sniff_mode(bt_bd_addr_t *remote_addr)
{
    bt_status_t status;
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)remote_addr);
    if (NULL == device_p) {
        bt_cmgr_report_id("[BT_CM][I] Exit sniff mode fail, device not find", 0);
        return BT_CONNECTION_MANAGER_STATUS_INVALID_PARAM;
    }
    if (BT_GAP_LINK_SNIFF_TYPE_ACTIVE == device_p->sniff_state) {
        bt_cmgr_report_id("[BT_CM][I] Exit sniff mode fail, atively now", 0);
        return BT_CONNECTION_MANAGER_STATUS_STATE_ALREADY_EXIST;
    }
    status = bt_gap_exit_sniff_mode(device_p->handle);
    bt_cmgr_report_id("[BT_CM][E] Exit sniff mode fail status %x !!!", 1, status);
    return BT_STATUS_SUCCESS;
}

#ifdef MTK_AWS_MCE_ENABLE
static void bt_connection_manager_write_aws_key(void)
{
    bt_key_t* aws_key = bt_connection_manager_device_local_info_get_aws_key();
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    bt_bd_addr_t* aws_addr = NULL;
    bt_gap_stored_link_key_t key_list;
    bt_connection_manager_assert(aws_key);
    if((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
        aws_addr = bt_connection_manager_device_local_info_get_peer_aws_address();
    } else {
        aws_addr = bt_connection_manager_device_local_info_get_local_address();
    }
    bt_connection_manager_assert(aws_addr);
    bt_connection_manager_memcpy(&(key_list.address), aws_addr, sizeof(bt_bd_addr_t));
    bt_connection_manager_memcpy(&(key_list.key), aws_key, sizeof(bt_key_t));
    bt_gap_write_stored_link_key_param_t key_param = {
        .key_number = 1,
        .key_list = &key_list
    };
    bt_status_t ret = bt_gap_write_stored_link_key(&key_param);
    bt_cmgr_report_id("[BT_CM][I] Write aws key result:0x%x", 1, ret);
    if (BT_STATUS_SUCCESS != ret) {
        bt_connection_manager_assert(0);
    }
}

void bt_sink_srv_cm_ls_enable(bool enable)
{
    bt_sink_srv_cm_remote_device_t *aws_device = bt_sink_srv_cm_get_normal_aws_device();
    bt_sink_srv_cm_remote_device_t *saws_device = bt_sink_srv_cm_get_special_aws_device();
    bt_sink_srv_cm_ls_enable_t = enable;
    if(NULL != aws_device && BT_AWS_MCE_AGENT_STATE_CONNECTABLE == aws_device->aws_state && false == enable) {
        bt_connection_manager_enable_aws_discoverable_by_device(aws_device, false);
    } else if (NULL != aws_device && BT_AWS_MCE_AGENT_STATE_INACTIVE == aws_device->aws_state && true == enable) {
        bt_connection_manager_enable_aws_discoverable(true);
    } else if (NULL != saws_device && BT_AWS_MCE_AGENT_STATE_CONNECTABLE == saws_device->aws_state && false == enable) {
        bt_connection_manager_enable_aws_discoverable(false);
    } else if (NULL != saws_device && BT_AWS_MCE_AGENT_STATE_INACTIVE == saws_device->aws_state && true == enable) {
        bt_connection_manager_enable_aws_discoverable(true);
    }
}

static void bt_connection_manager_enable_aws_discoverable_by_device(
                   bt_sink_srv_cm_remote_device_t *device_p, bool enable)
{
    bt_aws_mce_agent_state_type_t aws_state = device_p->aws_state;
    bt_status_t ret = BT_STATUS_FAIL;
    bt_cmgr_report_id("[BT_CM][I] Enable aws discoverable:%d, state:0x%x", 2, enable, device_p->aws_state);
    if (enable) {
        if(BT_AWS_MCE_AGENT_STATE_NONE == aws_state || BT_AWS_MCE_AGENT_STATE_INACTIVE == aws_state) {
            bt_connection_manager_mutex_lock();
            uint32_t aws_handle = bt_sink_srv_aws_mce_get_handle(&(device_p->addr));
            ret = bt_aws_mce_set_state(aws_handle, BT_AWS_MCE_AGENT_STATE_CONNECTABLE);
            if (BT_STATUS_SUCCESS == ret) {
                device_p->flag |= BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING;
                device_p->set_aws_state = BT_SINK_SRV_CM_SET_AWS_ENABLE_SUCCESS;
                bt_cmgr_report_id("[BT_CM][I] Switch aws enable state .....  ", 0);
            } else {
                device_p->set_aws_state = BT_SINK_SRV_CM_SET_AWS_ENABLE_PENDING_TRUE;
                bt_cmgr_report_id("[BT_CM][W] AWS Set state enable fail:0x%x device %p aws_handle %08X", 3, ret, device_p, aws_handle);
            }
            bt_connection_manager_mutex_unlock();
        } else if (BT_AWS_MCE_AGENT_STATE_CONNECTABLE == aws_state){
            bt_cmgr_report_id("[BT_CM][W] AWS already discoverable.", 0);
        } else if (BT_AWS_MCE_AGENT_STATE_ATTACHED == aws_state){
            bt_cmgr_report_id("[BT_CM][W] AWS has attached, state:0x%x" , 1,aws_state);
        }
    } else {
        if (BT_AWS_MCE_AGENT_STATE_CONNECTABLE == aws_state || BT_AWS_MCE_AGENT_STATE_ATTACHED == aws_state) {
            bt_connection_manager_mutex_lock();
            uint32_t aws_handle = bt_sink_srv_aws_mce_get_handle(&(device_p->addr));
            ret = bt_aws_mce_set_state(aws_handle, BT_AWS_MCE_AGENT_STATE_INACTIVE);
            if (BT_STATUS_SUCCESS == ret) {
                device_p->flag |= BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING;
                device_p->set_aws_state = BT_SINK_SRV_CM_SET_AWS_ENABLE_SUCCESS;
                bt_cmgr_report_id("[BT_CM][I] Switch aws disable state .....  ", 0);
            } else {
                device_p->set_aws_state = BT_SINK_SRV_CM_SET_AWS_ENABLE_PENDING_FALSE;
                bt_cmgr_report_id("[BT_CM][W] AWS Set state disable fail:0x%x device %p aws_handle %08X", 3, ret, device_p, aws_handle);
            }
            bt_connection_manager_mutex_unlock();
        } else if (BT_AWS_MCE_AGENT_STATE_INACTIVE == aws_state || BT_AWS_MCE_AGENT_STATE_NONE == aws_state){
            bt_cmgr_report_id("[BT_CM][W] AWS already not discoverable.", 0);
        }
    }
}

static void bt_connection_manager_enable_aws_discoverable(bool enable)
{
    bt_sink_srv_cm_remote_device_t *normal_device = bt_sink_srv_cm_get_normal_aws_device();
    bt_sink_srv_cm_remote_device_t *saws_device = bt_sink_srv_cm_get_special_aws_device();
    if (bt_connection_manager_device_local_info_get_aws_role() != BT_AWS_MCE_ROLE_AGENT) {
        bt_cmgr_report_id("[BT_CM][E] Enable aws state but current role error !!!", 0);
        return;
    }
    if (!bt_sink_srv_cm_ls_enable_t && enable) {
        bt_cmgr_report_id("[BT_CM][W] LS enable had been forbidden !!!", 0);
        return;
    }
    if (enable && bt_connection_manager_get_dut_mode()) {
        bt_cmgr_report_id("[BT_CM][W] LS enable had been forbidden by dut mode !!!", 0);
        return;
    }
    bt_connection_manager_assert(NULL != saws_device);
    if ((saws_device->flag & BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING) ||
        (NULL != normal_device && (normal_device->flag & BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING))) {
        bt_cmgr_report_id("[BT_CM][E] Enable aws state but role is setting 1 !!!", 0);
        return;
    }
    if (false == enable) {
        bt_connection_manager_enable_aws_discoverable_by_device(saws_device, false);
        if (NULL != normal_device) {
            bt_connection_manager_enable_aws_discoverable_by_device(normal_device, false);
        }
    } else if (NULL == normal_device) {
        bt_connection_manager_enable_aws_discoverable_by_device(saws_device, true);
    } else {
        normal_device->flag |= BT_SINK_SRV_CM_REMOTE_FLAG_NEED_AWS_DISCOVERABLE;
        bt_cmgr_report_id("[BT_CM][I] Enable aws discoverable conn_dev_num:%d, link_state:%d, role:%d, sniff_state:%d, saws_state:0x%02X",
                5, g_sink_srv_cm_context.connected_dev_num, normal_device->link_state, normal_device->local_role,
                normal_device->sniff_state, saws_device->aws_state);
        if (g_sink_srv_cm_context.connected_dev_num > 2) {
            /* The connected sp maximum number should be 1 before enable LS on special link or sp link. */
            return;
        } else if (BT_GAP_LINK_SNIFF_TYPE_ACTIVE != normal_device->sniff_state) {
            /* The sp connection link sniff state should be active before enable LS on special link or sp link. */
            bt_status_t status;
            status = bt_gap_exit_sniff_mode(normal_device->handle);
            bt_cmgr_report_id("[BT_CM][E] Exit sniff mode fail status %x !!!", 1, status);
            return;
        } else if (BT_SINK_SRV_CM_STATE_ENCRYPTED > normal_device->link_state) {
            /* If the encryption had not complete and partner lost, we should enable LS on special link make parnter can connect again. */
            normal_device->flag &= (~BT_SINK_SRV_CM_REMOTE_FLAG_NEED_AWS_DISCOVERABLE);
            bt_connection_manager_enable_aws_discoverable_by_device(saws_device, true);
            return;
        } else if (saws_device->aws_state != BT_AWS_MCE_AGENT_STATE_INACTIVE && saws_device->aws_state != BT_AWS_MCE_AGENT_STATE_NONE) {
            /* Disable LS or Disconnect partner on special link before switch aws link to sp link. */
            bt_connection_manager_enable_aws_discoverable_by_device(saws_device, false);
            return;
        } else if (BT_SINK_SRV_ROLE_SLAVE != normal_device->local_role) {
            /* The sp connection link role should be slave before enable LS on SP link. */
            // if (!(normal_device->profile_connection_mask & BT_SINK_SRV_CM_PROFILE_ALL)) {
            //     bt_cmgr_report_id("[BT_CM][I] Enable aws discoverable wait profile connect ...", 0);
            // } else {
                bt_connection_manager_set_role(&(normal_device->addr), BT_SINK_SRV_ROLE_SLAVE, false);
            //}
            return;
        }
        normal_device->flag &= (~BT_SINK_SRV_CM_REMOTE_FLAG_NEED_AWS_DISCOVERABLE);
        bt_connection_manager_enable_aws_discoverable_by_device(normal_device, true);
    }
}

static void bt_connection_manager_send_aws_mce_packet(bt_sink_srv_cm_aws_sync_packet_t type, uint32_t data_length, void *data)
{
    bt_cmgr_report_id("[BT_CM][I] Send aws mce packet type:%d, data_length:%d, data:0x%x", 3, type, data_length, data);
    uint32_t report_length = sizeof(bt_aws_mce_report_info_t) + data_length + 1;
    bt_aws_mce_report_info_t *cm_report = bt_connection_manager_memory_alloc(report_length);
    if (NULL != cm_report) {
        uint8_t *data_payload = ((uint8_t *)cm_report) + sizeof(bt_aws_mce_report_info_t);

        bt_connection_manager_memset(cm_report, 0 , sizeof(report_length));
        cm_report->module_id = BT_AWS_MCE_REPORT_MODULE_CM;
        cm_report->param_len = report_length - sizeof(bt_aws_mce_report_info_t);
        cm_report->param = data_payload;
        data_payload[0] = type;
        if (data_length) {
            bt_connection_manager_memcpy(data_payload + 1, (void*)data, data_length);
        }
        bt_status_t status;
        if (BT_STATUS_SUCCESS != (status = bt_aws_mce_report_send_event(cm_report))) {
            bt_cmgr_report_id("[BT_CM][I] Send aws mce packet failed status 0x%x!!!", 1, status);
        }
        bt_connection_manager_memory_free(cm_report);
    } else {
        bt_cmgr_report_id("[BT_CM][E] Send aws mce packet failed can't allocat buffer !!!", 0);
    }
}

static void bt_connection_manager_partner_sync_address_to_agent(void)
{
    bt_bd_addr_t *agent_addr = bt_connection_manager_device_local_info_get_peer_aws_address();
    bt_sink_srv_cm_remote_device_t *device_p =
        bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)agent_addr);
    if (NULL != device_p) {
        bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
        bt_connection_manager_send_aws_mce_packet(BT_SINK_SRV_CM_AWS_SYNC_CLIENT_ADDR_TO_AGENT, sizeof(bt_bd_addr_t), local_addr);
    } else {
        bt_cmgr_report_id("[BT_CM][I] Sync address to agent failed due aws not attached !!!", 0);
    }
}

static void bt_connection_manager_aws_sync_connnection_confirm(void)
{
    bt_bd_addr_t *aws_addr = bt_sink_srv_cm_get_aws_connected_device();
    bt_cmgr_report_id("[BT_CM][I] Aws connection confirm ...", 0);
    if (NULL != aws_addr) {
        bt_connection_manager_send_aws_mce_packet(BT_SINK_SRV_CM_AWS_SYNC_CONNECTION_CONFIRM, 0, NULL);
    } else {
        bt_cmgr_report_id("[BT_CM][E] Sync aws connection confirm failed !!!", 0);
    }
}

bt_aws_mce_agent_state_type_t bt_sink_srv_cm_get_aws_link_state(void)
{
    bt_sink_srv_cm_remote_device_t *aws_device = bt_sink_srv_cm_get_normal_aws_device();
    bt_sink_srv_cm_remote_device_t *saws_device = bt_sink_srv_cm_get_special_aws_device();
    bt_sink_srv_cm_remote_device_t *device_p = aws_device != NULL ? aws_device : saws_device;
    if (NULL != device_p) {
        return device_p->aws_state;
    }
    return BT_AWS_MCE_AGENT_STATE_INACTIVE;
}

void bt_sink_srv_cm_aws_mce_state_notify(bt_bd_addr_t *addr, bt_aws_mce_agent_state_type_t state)
{
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)addr);
    if (NULL == device_p) {
        bt_cmgr_report_id("[BT_CM][E] Aws mce state notify device not find by addr", 0);
        return;
    }
#ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)bt_connection_manager_memory_alloc(sizeof(bt_sink_srv_aws_mce_state_changed_ind_t));
    if (event) {
        bt_connection_manager_memcpy(&(event->aws_state_change.bt_addr), (void *)addr, sizeof(bt_bd_addr_t));
        event->aws_state_change.aws_state = state;
        bt_connection_manager_state_machine_ext_sink_event_callback(BT_SINK_SRV_EVENT_AWS_MCE_STATE_CHANGE, (void *)event, sizeof(*event));
        bt_connection_manager_memory_free(event);
    }
#endif
    bt_sink_srv_cm_remote_device_t *saws_device = bt_sink_srv_cm_get_special_aws_device();
    bt_aws_mce_agent_state_type_t previous_state = device_p->aws_state;
    if (device_p == saws_device) {
        bt_cmgr_report_id("[BT_CM][I] Aws mce state changed:from 0x%02X to 0x%02X at special link", 2, device_p->aws_state, state);
    } else {
        bt_cmgr_report_id("[BT_CM][I] Aws mce state changed:from 0x%02X to 0x%02X at normal link", 2, device_p->aws_state, state);
    }
    device_p->flag &= ~BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING;
    device_p->aws_state = state;
    if (BT_AWS_MCE_AGENT_STATE_INACTIVE == state) {
        if (BT_CM_POWER_STATE_OFF_PENDING == bt_connection_manager_power_get_state()) {
            bt_cmgr_report_id("[BT_CM][I] Aws detached for disconnect", 0);
            bt_connection_manager_power_off(false);
        } else if (true == g_bt_cm_disconnect_all) {
            bt_connection_manager_disconnect_all(false);
        } else {
            /*If partner had detached then reconnect it. */
            bt_connection_manager_enable_aws_discoverable(true);
        }
        /*
        if (BT_SINK_SRV_CM_STATE_DISCONNECTING == device_p->link_state) {
            bt_cmgr_report_id("[BT_CM][I] Aws detached for disconnect", 0);
            bt_connection_manager_disconnect_by_address(&(device_p->addr));
        } else {
            bt_connection_manager_enable_aws_discoverable(true);
        }*/
    } else if (BT_AWS_MCE_AGENT_STATE_CONNECTABLE == state && device_p == saws_device) {
        /*If 0x51 received on special link after encryption complete. */
        bt_connection_manager_enable_aws_discoverable(true);
    } else if (BT_AWS_MCE_AGENT_STATE_ATTACHED == state && BT_AWS_MCE_AGENT_STATE_ATTACHED != previous_state) {
        bt_connection_manager_aws_sync_connnection_confirm();
        bt_device_manager_aws_local_info_update();
        if (BT_AWS_MCE_AGENT_STATE_NONE != previous_state && device_p != saws_device) {
            /*If normal aws link steup then sync paired info to partner. */
            bt_device_manager_remote_aws_sync_to_partner();
        }
    }
}

bt_connection_manager_aws_link_type_t bt_connection_manager_get_aws_link_type(void)
{
    bt_sink_srv_cm_remote_device_t *saws_device = bt_sink_srv_cm_get_special_aws_device();
    bt_sink_srv_cm_remote_device_t *normal_device = bt_sink_srv_cm_get_normal_aws_device();
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
        if (NULL != saws_device && BT_SINK_SRV_CM_STATE_CONNECTED <= saws_device->link_state) {
            return BT_CONNECTION_MANAGER_AWS_LINK_NORMAL;
        } else if (NULL != normal_device && BT_SINK_SRV_CM_STATE_CONNECTED <= normal_device->link_state) {
            return BT_CONNECTION_MANAGER_AWS_LINK_SPECIAL;
        }
    } else {
        if (NULL != saws_device && BT_AWS_MCE_AGENT_STATE_ATTACHED == saws_device->aws_state) {
            return BT_CONNECTION_MANAGER_AWS_LINK_SPECIAL;
        } else if (NULL != normal_device && BT_AWS_MCE_AGENT_STATE_ATTACHED == normal_device->aws_state) {
            return BT_CONNECTION_MANAGER_AWS_LINK_NORMAL;
        }
    }
    return BT_CONNECTION_MANAGER_AWS_LINK_NONE;
}

static void bt_connection_manager_aws_mce_packet_callback(bt_aws_mce_report_info_t *para)
{
    if (NULL == para) {
        bt_cmgr_report_id("[BT_CM][E] bt_connection_manager_aws_mce_packet_callback para is null !!!", 0);
        return;
    }
    bt_cmgr_report_id("[BT_CM][I] bt_connection_manager_aws_mce_packet_callback module_id:0x%x, is_sync:%d, sync_time:%d, param_len:%d!!!", 4,
        para->module_id, para->is_sync, para->sync_time, para->param_len);
    if (BT_AWS_MCE_REPORT_MODULE_CM != para->module_id) {
        bt_cmgr_report_id("[BT_CM][I] bt_connection_manager_aws_mce_packet_callback module is not CM!!!", 0);
        return;
    }
    bt_sink_srv_cm_aws_sync_packet_t event = ((uint8_t *)para->param)[0];
    bt_cmgr_report_id("[BT_CM][I] bt_connection_manager_aws_mce_packet_callback event %d!!!", 1, event);
    switch (event) {
    case BT_SINK_SRV_CM_AWS_SYNC_CLIENT_ADDR_TO_AGENT: {
        bt_bd_addr_t *client_addr = (bt_bd_addr_t *)(((uint8_t *)para->param) + 1);
        bt_connection_manager_device_local_info_store_peer_aws_address(client_addr);
    }
        break;
    case BT_SINK_SRV_CM_AWS_SYNC_CONNECTION_CONFIRM: {
        bt_cmgr_report_id("[BT_CM][I] Aws connection confirm received!!!", 0);
    #ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
        bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
        if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
            bt_sink_srv_state_set(BT_SINK_SRV_STATE_CONNECTED);
            bt_device_manager_aws_local_info_update();
            bt_connection_manager_state_machine_ext_agent_sync_notify();
        }
    #endif
    }
        break;
    default:
        bt_cmgr_report_id("[BT_CM][E] Aws packet event error", 0);
        break;
    }
}

#endif /* #ifdef MTK_AWS_MCE_ENABLE */


void bt_connection_manager_force_enable_dut_mode(bool dut)
{
    bt_cm_is_force_enable_dut = dut;
    bt_gap_force_enable_dut(dut);
    if (dut) {
#ifdef MTK_AWS_MCE_ENABLE
        bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

        if(BT_AWS_MCE_ROLE_CLINET == role) {
            bt_sink_srv_send_action(BT_SINK_SRV_ACTION_BT_STANDBY, NULL);
        } else {
            bt_connection_manager_state_ext_force_deinit();
#endif
            bt_connection_manager_power_reset(false);
#ifdef MTK_AWS_MCE_ENABLE
        }
#endif
    }
}

#ifdef MTK_AWS_MCE_ENABLE
void bt_connection_manager_exchange_bt_address()
{
    /*exchange local address and remote address*/

    bt_bd_addr_t local_addr = {0};
    bt_bd_addr_t empty_addr = {0};
    bt_bd_addr_t *remote_addr = bt_connection_manager_device_local_info_get_peer_aws_address();

    bt_connection_manager_memcpy((void *)&local_addr, (void *)bt_connection_manager_device_local_info_get_local_address(), sizeof(bt_bd_addr_t));
    if (memcmp(remote_addr, empty_addr, sizeof(bt_bd_addr_t))) {
        bt_connection_manager_device_local_info_store_local_address(remote_addr);
    } else {
        bt_connection_manager_assert(0 && "Peer address is null, please check the enviroment is setup right: each earbud must set local and peer address." );
    }

    bt_connection_manager_device_local_info_store_peer_aws_address(&local_addr);

    return;
}

static void bt_connection_manager_dut_mode_switch_role()
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_real_role();

    if (BT_AWS_MCE_ROLE_PARTNER == role) {
        /*switch local address and peer address*/
        bt_connection_manager_exchange_bt_address();
        if (role & BT_AWS_MCE_ROLE_AGENT) {
            /*switch role to partner*/
            bt_connection_manager_device_local_info_store_aws_role(BT_AWS_MCE_ROLE_PARTNER);
        } else {
            /*switch role to agent*/
            bt_connection_manager_device_local_info_store_aws_role(BT_AWS_MCE_ROLE_AGENT);
        }

        /*set BT address to BT stack*/
        bt_set_local_public_address((bt_bd_addr_ptr_t)bt_connection_manager_device_local_info_get_local_address());
    } else if(BT_AWS_MCE_ROLE_CLINET == role) {
        bt_cm_is_reload_n9 = true;
        bt_connection_manager_device_local_info_store_aws_role(BT_AWS_MCE_ROLE_NONE);
        bt_set_local_public_address((bt_bd_addr_ptr_t)bt_connection_manager_device_local_info_get_local_address());
    }

}
#endif

bool bt_connection_manager_get_dut_mode(void)
{
    return bt_cm_is_force_enable_dut;
}

bt_status_t bt_connection_manager_force_disconnect(bt_bd_addr_t *remote_dev)
{
    bt_sink_srv_cm_remote_device_t *device_p = NULL;
    uint8_t dev_index = 0;
    bt_status_t ret = BT_STATUS_FAIL;
    bt_cmgr_report_id("[BT_CM][I] To force disconnect the remote device", 0);
    if (NULL == remote_dev) {
        device_p = NULL;
        dev_index = 1;
    } else if (NULL == (device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)remote_dev))) {
        bt_cmgr_report_id("[BT_CM][E] Can't find the remote device", 0);
        return ret;
    }
    for (; dev_index < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER + 1; ++dev_index) {
        bt_bd_addr_t* local_addr = bt_connection_manager_device_local_info_get_local_address();
        if (dev_index > 0) {
            device_p = &g_sink_srv_cm_context.devices[dev_index - 1];
        }
        if (BT_SINK_SRV_CM_STATE_DISCONNECTED != device_p->link_state &&
            bt_connection_manager_memcmp(local_addr, &(device_p->addr), sizeof(bt_bd_addr_t))) {
            ret = bt_gap_disconnect_ext(device_p->handle, 2000);
            bt_cmgr_report_id("[BT_CM][I] Force ext disconnect result 0x%x", 1, ret);
        }
        if (dev_index == 0) {
            break;
        }
    }
    return ret;
}


