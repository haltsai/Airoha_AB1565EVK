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
#ifdef RACE_LPCOMM_ENABLE
#include "race_xport.h"
#include "race_lpcomm_msg_struct.h"
#include "race_lpcomm_recv.h"
#include "race_cmd_fota.h"
#include "race_cmd_storage.h"
#include "race_cmd_bluetooth.h"
#include "race_lpcomm_packet.h"
#include "race_timer.h"
#include "race_lpcomm_util.h"
#include "race_storage_access.h"
#ifdef RACE_FOTA_CMD_ENABLE
#include "fota_multi_info.h"
#include "fota_multi_info_util.h"
#include "race_fota_util.h"
#include "race_fota.h"
#include "race_event.h"
#endif

#ifdef RACE_FIND_ME_ENABLE
#include "race_cmd_find_me.h"
#endif

#ifdef RACE_CFU_ENABLE
#include "race_cfu.h"
#include "race_cmd_cfu.h"
#include "race_cfu_internal.h"
#include "race_cfu_handler.h"
#endif


#ifdef RACE_FOTA_INTEGRITY_CHECK_ENHANCE_ENABLE
typedef struct
{
    uint8_t app_id;
    uint8_t channel_id;
    uint16_t process_id;
    uint16_t cmd_id;
    uint32_t signature_start_address;
    uint8_t storage_type;
    uint8_t trans_method;
    uint8_t device_id;
}race_lpcommp_fota_integrity_check_info_struct;
#endif


#ifdef RACE_FOTA_SMART_INTEGRITY_CHECK
bool g_lpcommp_fota_is_integritcy_check_on_going;
#endif


#ifdef RACE_FOTA_CMD_ENABLE
RACE_ERRCODE race_lpcommp_fota_query_state_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_fota_get_version_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_fota_write_state_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_fota_new_transaction_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_fota_start_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_fota_query_partition_info_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_fota_commit_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_fota_active_fota_preparation_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_fota_stop_result_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_fota_stop_query_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_fota_stop_query_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_fota_ping_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

#ifdef RACE_FOTA_INTEGRITY_CHECK_ENHANCE_ENABLE
RACE_ERRCODE race_lpcommp_fota_check_integrity_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);
#endif

RACE_ERRCODE race_lpcommp_fota_sp_ping_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);
#endif /* RACE_FOTA_CMD_ENABLE */

#ifdef RACE_STORAGE_CMD_ENABLE
RACE_ERRCODE race_lpcommp_storage_get_partition_sha256_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_storage_get_4k_erased_status_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_storage_dual_devices_erase_partition_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_storage_lock_unlock_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);
#endif /* RACE_STORAGE_CMD_ENABLE */

#ifdef RACE_BT_CMD_ENABLE
RACE_ERRCODE race_lpcommp_bt_get_battery_level_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcommp_bt_role_switch_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);
#endif /* RACE_BT_CMD_ENABLE */

#ifdef RACE_FIND_ME_ENABLE
RACE_ERRCODE race_lpcommp_find_me_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);
#endif

#ifdef RACE_CFU_ENABLE
RACE_ERRCODE race_lpcommp_cfu_relay_packet_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);
#endif


/* BEWARE: FAKE RSP may be received the structure of which is race_lpcomm_rsp_template_struct with the status of RACE_ERRCODE_FAIL.
  * Therefore, copy the results into noti, only when the status is RACE_ERRCODE_SUCCESS. Otherwise, invalid results are used.
  */
race_lpcomm_data_recv_hdl_struct g_lpcomm_partner_data_recv_hdl_array[] = {
#ifdef RACE_FOTA_CMD_ENABLE
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_DUAL_DEVICES_QUERY_STATE, race_lpcommp_fota_query_state_req_hdl, NULL, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_GET_VERSION, race_lpcommp_fota_get_version_req_hdl, NULL, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_DUAL_DEVICES_WRITE_STATE, race_lpcommp_fota_write_state_req_hdl, NULL, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_DUAL_DEVICES_NEW_TRANSACTION, race_lpcommp_fota_new_transaction_req_hdl, NULL, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_START, race_lpcommp_fota_start_req_hdl, NULL, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_DUAL_DEVICES_QUERY_PARTITION_INFO, race_lpcommp_fota_query_partition_info_req_hdl, NULL, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_DUAL_DEVICES_COMMIT, race_lpcommp_fota_commit_req_hdl, NULL, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_ACTIVE_FOTA_PREPARATION, race_lpcommp_fota_active_fota_preparation_req_hdl, NULL, NULL},
#ifdef RACE_FOTA_INTEGRITY_CHECK_ENHANCE_ENABLE
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_CHECK_INTEGRITY, race_lpcommp_fota_check_integrity_req_hdl, NULL, NULL},
#endif
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_PING, race_lpcommp_fota_sp_ping_req_hdl, NULL, NULL},
#endif /* RACE_FOTA_CMD_ENABLE */

#ifdef RACE_STORAGE_CMD_ENABLE
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_STORAGE_GET_PARTITION_SHA256, race_lpcommp_storage_get_partition_sha256_req_hdl, NULL, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_STORAGE_GET_4K_ERASED_STATUS, race_lpcommp_storage_get_4k_erased_status_req_hdl, NULL, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_STORAGE_DUAL_DEVICES_ERASE_PARTITION, race_lpcommp_storage_dual_devices_erase_partition_req_hdl, NULL, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_STORAGE_LOCK_UNLOCK, race_lpcommp_storage_lock_unlock_req_hdl, NULL, NULL},
#endif /* RACE_STORAGE_CMD_ENABLE */

#ifdef RACE_BT_CMD_ENABLE
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_BLUETOOTH_GET_BATTERY, race_lpcommp_bt_get_battery_level_req_hdl, NULL, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_BLUETOOTH_DUAL_ROLE_SWITCH, race_lpcommp_bt_role_switch_req_hdl, NULL, NULL},
#endif /* RACE_BT_CMD_ENABLE */

#ifdef RACE_FOTA_CMD_ENABLE
    {RACE_LPCOMM_PACKET_CLASS_COMMON, RACE_LPCOMM_COMMON_CMD_ID_FOTA_STOP_RESULT, race_lpcommp_fota_stop_result_req_hdl, NULL, NULL},
    {RACE_LPCOMM_PACKET_CLASS_COMMON, RACE_LPCOMM_COMMON_CMD_ID_FOTA_STOP_QUERY, race_lpcommp_fota_stop_query_req_hdl, race_lpcommp_fota_stop_query_rsp_hdl, NULL},
    {RACE_LPCOMM_PACKET_CLASS_COMMON, RACE_LPCOMM_COMMON_CMD_ID_FOTA_PING, race_lpcommp_fota_ping_req_hdl, NULL, NULL},
#endif
#ifdef RACE_FIND_ME_ENABLE
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_CMD_FIND_ME, race_lpcommp_find_me_req_hdl, NULL, NULL},
#endif

#ifdef RACE_CFU_ENABLE
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_CFU_RELAY_PACKET, race_lpcommp_cfu_relay_packet_req_hdl, NULL, NULL},
#endif

    {RACE_LPCOMM_PACKET_CLASS_NONE, RACE_INVALID_CMD_ID, NULL, NULL, NULL}
};


/* TODO: Partner does not check app_id. If Partner triggers FOTA, it just resets itself without telling Agent about
  * it. If Agent cancels FOTA during RHO, it changes to new Partner and stops FOTA after RHO. The new Agent
  * does not know Partner has stopped FOTA. At this time, RHO cmd with FOTA app_id is received. New Agent
  * will execute RHO and new Partner does not reject it though FOTA has been stopped.
  * The FOTA will be stopped by ping mechanism at last.
  */
RACE_ERRCODE race_lpcomm_partner_data_recv_hdl(race_lpcomm_packet_struct *packet,
                                                          uint8_t device_id)
{
    int32_t i = 0;
    race_lpcomm_packet_type_enum packet_type = RACE_LPCOMM_PACKET_TYPE_NONE;
    race_lpcomm_data_recv_hdl_struct *data_recv_hdl_array = g_lpcomm_partner_data_recv_hdl_array;

    RACE_LOG_MSGID_F("packet:%x, packet->process_id:%d, device_id:%d", 3,
               packet,
               packet ? packet->process_id : 0,
               device_id);

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    RACE_LOG_MSGID_I("packet_type:%x cmd_id:%x channel_id:%d process_id:%d trans_method:%d payload_len:%d",6,
               packet->packet_type, packet->cmd_id, packet->channel_id, packet->process_id, packet->trans_method, packet->payload_len);
    packet_type = packet->packet_type;

    if (RACE_APP_ID_FOTA == packet->app_id &&
        (((RACE_LPCOMM_PACKET_TYPE_RACE_CMD_RSP == packet_type ||
           RACE_LPCOMM_PACKET_TYPE_COMMON_RSP == packet_type) &&
          !race_fota_is_race_fota_running())
         /*|| ((RACE_LPCOMM_PACKET_TYPE_RACE_CMD_REQ == packet_type ||
              RACE_LPCOMM_PACKET_TYPE_COMMON_REQ == packet_type) &&
             !race_fota_is_cmd_allowed(NULL, packet->cmd_id, packet->channel_id))*/
         ))
    {
        /* FOTA stop procedure does not remove the REQ from the retry list if it exists. Therefore, remove it here. */
        RACE_LOG_MSGID_W("Drop FOTA packet_type:%d cmd_id:%d channel_id:%d because it is not allowed.",3, packet_type, packet->cmd_id, packet->channel_id);
        race_lpcomm_retry_list_struct *list_node = race_lpcomm_retry_list_node_find_by_process_id(packet->process_id);
        if (list_node)
        {
            race_lpcomm_retry_drop_packet(list_node);
        }
        return RACE_ERRCODE_WRONG_STATE;
    }

    while (RACE_INVALID_CMD_ID != data_recv_hdl_array[i].cmd_id)
    {
        if (packet->cmd_id == data_recv_hdl_array[i].cmd_id)
        {
            if (((RACE_LPCOMM_PACKET_TYPE_RACE_CMD_REQ == packet_type &&
                  RACE_LPCOMM_PACKET_CLASS_RACE_CMD == data_recv_hdl_array[i].packet_class) ||
                 (RACE_LPCOMM_PACKET_TYPE_COMMON_REQ == packet_type &&
                  RACE_LPCOMM_PACKET_CLASS_COMMON == data_recv_hdl_array[i].packet_class)) &&
                data_recv_hdl_array[i].req_hdl)
            {
                return data_recv_hdl_array[i].req_hdl(packet, device_id);
            }
            else if (((RACE_LPCOMM_PACKET_TYPE_RACE_CMD_RSP == packet_type &&
                       RACE_LPCOMM_PACKET_CLASS_RACE_CMD == data_recv_hdl_array[i].packet_class) ||
                      (RACE_LPCOMM_PACKET_TYPE_COMMON_RSP == packet_type &&
                       RACE_LPCOMM_PACKET_CLASS_COMMON == data_recv_hdl_array[i].packet_class)) &&
                     data_recv_hdl_array[i].rsp_hdl)
            {
#ifdef RACE_LPCOMM_RETRY_ENABLE
                /* Remove req from the retry-list */
                race_lpcomm_retry_list_struct *list_node = race_lpcomm_retry_list_node_find_by_process_id(packet->process_id);
                if (list_node)
                {
                    race_lpcomm_packet_free(list_node->data);
                    race_lpcomm_retry_list_remove(list_node);
                    race_lpcomm_retry_list_node_free(list_node);

                    /* Free process_status in rsp_hdl if needed. */
                    if (data_recv_hdl_array[i].rsp_hdl)
                    {
                        return data_recv_hdl_array[i].rsp_hdl(packet, device_id);
                    }
                    else
                    {
                        configASSERT(0);
                    }
                }
                else
                {
                    RACE_LOG_MSGID_I("Drop the RSP for there's no REQ in the retry_list.", 0);
                }
#else
                if (data_recv_hdl_array[i].rsp_hdl)
                {
                    /* Free process_status in rsp_hdl if needed. */
                    return data_recv_hdl_array[i].rsp_hdl(packet, device_id);
                }
                else
                {
                    configASSERT(0);
                }
#endif /* RACE_LPCOMM_RETRY_ENABLE */
            }
            else if (RACE_LPCOMM_PACKET_CLASS_COMMON == data_recv_hdl_array[i].packet_class &&
                     RACE_LPCOMM_PACKET_TYPE_COMMON_NOTI == packet_type &&
                     data_recv_hdl_array[i].noti_hdl)
            {
                return data_recv_hdl_array[i].noti_hdl(packet, device_id);
            }

            return RACE_ERRCODE_FAIL;
        }

        i++;
    }

    return RACE_ERRCODE_FAIL;
}


#ifdef RACE_FOTA_CMD_ENABLE
RACE_ERRCODE race_lpcommp_fota_query_state_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    int32_t ret = RACE_ERRCODE_FAIL;
    uint16_t fota_state = 0xFFFF;
    race_lpcomm_fota_dual_device_query_state_req_struct *req = NULL;
    race_lpcomm_fota_query_state_rsp_struct rsp = {0};
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    req = (race_lpcomm_fota_dual_device_query_state_req_struct *)packet->payload;
    ret = fota_state_read(&fota_state);
    ret = FOTA_ERRCODE_SUCCESS == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;

    rsp.status = ret;
    if (RACE_ERRCODE_SUCCESS == ret)
    {
        rsp.fota_state = fota_state;
    }

    ret = race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                                sizeof(race_lpcomm_fota_query_state_rsp_struct),
                                                RACE_LPCOMM_ROLE_PARTNER,
                                                packet->cmd_id,
                                                packet->app_id,
                                                packet->channel_id,
                                                packet->process_id,
                                                packet->trans_method,
                                                device_id);

    if (fota_cntx &&
        !fota_cntx->transfer_complete &&
        race_fota_is_race_fota_running() &&
        RACE_ERRCODE_SUCCESS == rsp.status &&
        RACE_ERRCODE_SUCCESS == ret &&
        0x0311 == rsp.fota_state &&
        0x0311 == req->agent_fota_state)
    {
        fota_cntx->transfer_complete = TRUE;
        /* Send the transfer complete event. */
        race_send_event_notify_msg(RACE_EVENT_TYPE_FOTA_TRANSFER_COMPLETE, NULL);
    }

// TODO: This does not work acutally when last time FOTA package is donwloaded and not committed.
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
    if (RACE_ERRCODE_SUCCESS == rsp.status &&
        RACE_ERRCODE_SUCCESS == ret &&
        0x0311 == rsp.fota_state &&
        0x0311 == req->agent_fota_state &&
        RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT != race_fota_dl_state_get())
    {
        RACE_LOG_MSGID_I("%x %x FOTA download complete. Wait for commit cmd to reboot.",2,
                   req->agent_fota_state,
                   rsp.fota_state);

        if (race_fota_is_active_mode())
        {
            race_fota_active_bt_preparation_revert();
        }
        race_fota_set_fota_mode(RACE_FOTA_MODE_MAX);
        race_storage_disable_fota_partition_accessibility();

#ifdef RACE_LPCOMM_ENABLE
        /* Stop ping timer */
        race_timer_smart_stop(race_fota_ping_timer_id_get(), NULL);
        race_fota_ping_timer_id_set(RACE_TIMER_INVALID_TIMER_ID);
#endif
        race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT);
    }
#endif

    return ret;
}


RACE_ERRCODE race_lpcommp_fota_get_version_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    int32_t ret = RACE_ERRCODE_FAIL;
    race_lpcomm_fota_get_version_rsp_struct rsp = {0};

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    ret = fota_version_get(rsp.version, FOTA_VERSION_MAX_SIZE, FOTA_VERSION_TYPE_STORED);
    rsp.status = FOTA_ERRCODE_SUCCESS == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
    if (RACE_ERRCODE_SUCCESS == rsp.status)
    {
        rsp.version_len = strlen((const char *)rsp.version);
    }

    ret = race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                       sizeof(race_lpcomm_fota_get_version_rsp_struct),
                                       RACE_LPCOMM_ROLE_PARTNER,
                                       packet->cmd_id,
                                       packet->app_id,
                                       packet->channel_id,
                                       packet->process_id,
                                       packet->trans_method,
                                       device_id);

    return ret;
}


RACE_ERRCODE race_lpcommp_fota_write_state_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_write_state_req_struct *req = NULL;
    race_lpcomm_fota_write_state_rsp_struct rsp = {0};

    if (!packet || !packet->process_id ||
        packet->payload_len < sizeof(race_lpcomm_fota_write_state_req_struct))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    req = (race_lpcomm_fota_write_state_req_struct *)packet->payload;
    rsp.status = fota_state_write(req->fota_state);

    return race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                        sizeof(race_lpcomm_fota_write_state_rsp_struct),
                                        RACE_LPCOMM_ROLE_PARTNER,
                                        packet->cmd_id,
                                        packet->app_id,
                                        packet->channel_id,
                                        packet->process_id,
                                        packet->trans_method,
                                        device_id);
}


RACE_ERRCODE race_lpcommp_fota_new_transaction_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_new_transaction_rsp_struct rsp = {0};

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp.status = fota_multi_info_sector_reset();

    return race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                        sizeof(race_lpcomm_fota_new_transaction_rsp_struct),
                                        RACE_LPCOMM_ROLE_PARTNER,
                                        packet->cmd_id,
                                        packet->app_id,
                                        packet->channel_id,
                                        packet->process_id,
                                        packet->trans_method,
                                        device_id);
}


RACE_ERRCODE race_lpcommp_fota_start_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_start_req_struct *req = NULL;
    race_lpcomm_fota_start_rsp_struct rsp = {0};
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;
    uint8_t timer_id = race_fota_ping_timer_id_get();
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();
    race_fota_mode_enum fota_mode = RACE_FOTA_MODE_MAX;
    race_fota_dual_device_dl_method_enum dl_method = RACE_FOTA_DUAL_DEVICE_DL_METHOD_MAX;

    if (!packet || !packet->process_id ||
        packet->payload_len < sizeof(race_lpcomm_fota_start_req_struct))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    req = (race_lpcomm_fota_start_req_struct *)packet->payload;
    fota_mode = RACE_FOTA_START_GET_FOTA_MODE(req->fota_mode);
    dl_method = RACE_FOTA_START_GET_DL_METHOD(req->fota_mode);

    /* Check the start cmd parameters. */
    ret = race_fota_start_check_params(RACE_RECIPIENT_TYPE_AGENT_PARTNER, fota_mode, dl_method);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        RACE_LOG_MSGID_W("Fail to start FOTA for parameter errors. ret:%d",1, ret);

#ifdef RACE_FOTA_ACTIVE_MODE_KEEP_HFP
        if (RACE_ERRCODE_REJECT_FOR_CALL_ONGOING == ret &&
            race_fota_is_race_fota_running() &&
            race_fota_is_active_mode())
        {
            RACE_ERRCODE ret_val = race_fota_cancel();

            RACE_LOG_MSGID_I("race_fota_cancel() ret:%d",1, ret_val);
        }
#endif
    }

    if (RACE_ERRCODE_SUCCESS != ret)
    {
        rsp.status = ret;
    }
    else if (!race_fota_is_cmd_allowed(NULL, RACE_FOTA_START, packet->channel_id) || !fota_cntx)
    {
        /* Status of Agent and Partner may not sync. Wait for Partner is ready to start FOTA. */
        rsp.status = RACE_ERRCODE_NOT_ALLOWED;
    }
    else
    {
        if (RACE_FOTA_DOWNLOAD_STATE_NONE == fota_dl_state ||
            RACE_FOTA_DOWNLOAD_STATE_MAX == fota_dl_state
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
            || RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT == fota_dl_state

#endif
            )
        {
            race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_STARTING);
            fota_cntx->transfer_complete = FALSE;
            race_fota_set_fota_mode(fota_mode);
            fota_cntx->dl_method = dl_method;
        }

        ret = race_storage_enable_fota_partition_accessibility();

        if (RACE_ERRCODE_SUCCESS == ret)
        {
            /* Start ping if it has not been started. */
            if (RACE_TIMER_INVALID_TIMER_ID == timer_id)
            {
                ret = race_timer_smart_start(&timer_id,
                                             RACE_FOTA_PARTNER_PING_INTERVAL_IN_MS,
                                             race_fota_ping_timer_expiration_hdl,
                                             NULL);
                if (RACE_ERRCODE_SUCCESS == ret)
                {
                    RACE_LOG_MSGID_I("start ping timer", 0);
                    race_fota_ping_timer_id_set(timer_id);
                }
            }
        }

        rsp.status = ret;
    }

    ret = race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                                sizeof(race_lpcomm_fota_start_rsp_struct),
                                                RACE_LPCOMM_ROLE_PARTNER,
                                                packet->cmd_id,
                                                packet->app_id,
                                                packet->channel_id,
                                                packet->process_id,
                                                packet->trans_method,
                                                device_id);

    if ((RACE_FOTA_DOWNLOAD_STATE_NONE == fota_dl_state ||
         RACE_FOTA_DOWNLOAD_STATE_MAX == fota_dl_state
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
         || RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT == fota_dl_state
#endif
         ) &&
        RACE_FOTA_DOWNLOAD_STATE_STARTING == race_fota_dl_state_get())
    {
        if (RACE_ERRCODE_SUCCESS == rsp.status &&
            RACE_ERRCODE_SUCCESS == ret)
        {
            RACE_LOG_MSGID_I("Start FOTA for the first time. channel_id:%d",1, packet->channel_id);

            race_fota_set_sp_trans_method_by_channel_id(packet->channel_id);
            fota_cntx->lpcomm_peer_online = TRUE;
            fota_cntx->is_dual_fota = TRUE;
#ifdef RACE_LPCOMM_SENDER_ROLE_ENABLE
            fota_cntx->fota_role = RACE_LPCOMM_ROLE_PARTNER;
#endif
            race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_START);
            race_event_send_fota_start_event(fota_cntx->is_dual_fota, race_fota_is_active_mode());
            //race_send_event_notify_msg(RACE_EVENT_TYPE_FOTA_START, NULL);
        }
        else
        {
            race_fota_stop_partner_reset();
        }

        if (fota_cntx && fota_cntx->fota_stop_required)
        {
            /* Would not happen actually for there's no possible to stop FOTA during partner's STARTING state.
                        * Add here to handle stop FOTA during STARTING state in case partner changes the start flow.
                        */
            RACE_LOG_MSGID_W("Execute the delayed FOTA_STOP during FOTA_START.", 0);
            fota_cntx->fota_stop_required = FALSE;
            race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_PARTNER,
                           RACE_FOTA_STOP_REASON_CANCEL);
        }
    }
    /* else this is the retry cmd or the start after RHO, do nothing. Let ping timer handle the potential errors. */

    if (RACE_ERRCODE_SUCCESS == ret && RACE_ERRCODE_SUCCESS != rsp.status)
    {
        ret = rsp.status;
    }

    return ret;
}


RACE_ERRCODE race_lpcommp_fota_query_partition_info_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_query_partition_info_req_struct *req = NULL;
    race_lpcomm_fota_query_partition_info_rsp_struct rsp = {0};

    if (!packet || !packet->process_id ||
        packet->payload_len < sizeof(race_lpcomm_fota_query_partition_info_req_struct))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    req = (race_lpcomm_fota_query_partition_info_req_struct *)packet->payload;
    rsp.status = race_query_partition_info(&(rsp.storage_type),
                                           &(rsp.partition_address),
                                           &(rsp.partition_length),
                                           req->partition_id);
    return race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                        sizeof(race_lpcomm_fota_query_partition_info_rsp_struct),
                                        RACE_LPCOMM_ROLE_PARTNER,
                                        packet->cmd_id,
                                        packet->app_id,
                                        packet->channel_id,
                                        packet->process_id,
                                        packet->trans_method,
                                        device_id);
}


RACE_ERRCODE race_lpcommp_fota_commit_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_commit_rsp_struct rsp = {0};
    race_lpcomm_fota_commit_req_struct *req = NULL;
    int32_t ret = RACE_ERRCODE_SUCCESS;
    uint8_t integrity_res = 0xFF;
    uint8_t timer_id = race_fota_commit_timer_id_get();
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();
    race_fota_commit_timer_msg_struct *commit_timer_msg = NULL;

    if (!packet || !packet->process_id ||
        packet->payload_len < sizeof(race_lpcomm_fota_commit_req_struct))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    req = (race_lpcomm_fota_commit_req_struct *)packet->payload;

    if (RACE_EVENT_REMOTE_DEVICE_ADDRESS_LENGTH < req->address_length)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (RACE_FOTA_DOWNLOAD_STATE_COMMITING != fota_dl_state)
    {
        /* Check integrity result and set FOTA upgrade flag. */
        ret = fota_dl_integrity_res_read(&integrity_res);
        if (FOTA_ERRCODE_SUCCESS == ret &&
            FOTA_DL_INTEGRITY_RES_VAL_PASS == integrity_res)
        {
            ret = fota_upgrade_flag_set();
            ret = FOTA_ERRCODE_SUCCESS == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
        }
        else
        {
            ret = RACE_ERRCODE_FAIL;
        }

        if (RACE_ERRCODE_SUCCESS == ret)
        {
            /* Start the commit timer to wait for the retry(retry interval: 2s)  req in case rsp is lost. */
            /* Start commit timer */
            RACE_LOG_MSGID_I("commit_timer_id:%d",1, timer_id);
            if (RACE_TIMER_INVALID_TIMER_ID != timer_id)
            {
                RACE_LOG_MSGID_W("unexpected! when dl_state is:%d, commit_timer has been started!",1, timer_id);
                ret = race_timer_smart_reset(timer_id);
                if (RACE_ERRCODE_SUCCESS != ret)
                {
                    race_timer_smart_stop(timer_id, (void **)&commit_timer_msg);
                    if (commit_timer_msg)
                    {
                        race_mem_free(commit_timer_msg);
                        commit_timer_msg = NULL;
                    }
                    timer_id = RACE_TIMER_INVALID_TIMER_ID;
                    race_fota_commit_timer_id_set(timer_id);
                    ret = RACE_ERRCODE_SUCCESS;
                }
            }

            if (RACE_TIMER_INVALID_TIMER_ID == timer_id)
            {
                commit_timer_msg = race_mem_alloc(sizeof(race_fota_commit_timer_msg_struct));

                if (commit_timer_msg)
                {
                    memset(commit_timer_msg, 0, sizeof(race_fota_commit_timer_msg_struct));
                    commit_timer_msg->address_length = req->address_length;
                    if (commit_timer_msg->address_length)
                    {
                        memcpy(commit_timer_msg->address, req->address, commit_timer_msg->address_length);
                    }
                }

                ret = race_timer_smart_start(&timer_id,
                                             RACE_TIMER_FOTA_COMMIT_DELAY_IN_MS,
                                             race_fota_commit_timer_expiration_hdl,
                                             commit_timer_msg);

                if (RACE_ERRCODE_SUCCESS == ret)
                {
                    race_fota_commit_timer_id_set(timer_id);
                }
                else
                {
                    race_mem_free(commit_timer_msg);
                    commit_timer_msg = NULL;
                }
            }
        }
    }
    else
    {
        /* Partner has processed COMMIT REQ successfully previously. */
        if (RACE_TIMER_INVALID_TIMER_ID == timer_id)
        {
            /* Do not start commit timer for commit timer has been timeout. */
            RACE_LOG_MSGID_W("commit_timer_id is valid. It maybe has expired.", 0);
        }
        else
        {
            ret = race_timer_smart_reset(timer_id);
            if (RACE_ERRCODE_SUCCESS != ret)
            {
                /* Even if ret is not SUCCESS, reboot Partner because RSP with status of SUCCESS
                                * has sent to Agent before. */
                race_event_send_fota_need_reboot_event(req->address, req->address_length);
                ret = RACE_ERRCODE_SUCCESS;
            }
        }
    }

    rsp.status = ret;
    /* Agent will reboot only if it receives partner's rsp with status of RACE_ERRCODE_SUCCESS */
    ret = race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                                 sizeof(race_lpcomm_fota_commit_rsp_struct),
                                                 RACE_LPCOMM_ROLE_PARTNER,
                                                 packet->cmd_id,
                                                 packet->app_id,
                                                 packet->channel_id,
                                                 packet->process_id,
                                                 packet->trans_method,
                                                 device_id);

    if (RACE_FOTA_DOWNLOAD_STATE_COMMITING != fota_dl_state)
    {
        if (RACE_ERRCODE_SUCCESS != rsp.status ||
            RACE_ERRCODE_SUCCESS != ret)
        {
            RACE_LOG_MSGID_W("rsp.status:%d ret:%d",2, rsp.status, ret);
            fota_upgrade_flag_clear();
            if (RACE_TIMER_INVALID_TIMER_ID != timer_id)
            {
                race_timer_smart_stop(timer_id, (void **)&commit_timer_msg);
                if (commit_timer_msg)
                {
                    race_mem_free(commit_timer_msg);
                    commit_timer_msg = NULL;
                }
                race_fota_commit_timer_id_set(RACE_TIMER_INVALID_TIMER_ID);
            }
        }
        else
        {
            race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_COMMITING);
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
            race_fota_set_sp_trans_method_by_channel_id(packet->channel_id);
#endif
        }
    }
    /* else partner has sent rsp with status of RACE_ERRCODE_SUCCESS previously. So it
         * will not revert commit even if it fails to send rsp for Agent rarely does not receive the previous RSP.
         */

    return ret;
}


RACE_ERRCODE race_lpcommp_fota_active_fota_preparation_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    int32_t ret = RACE_ERRCODE_FAIL;
    race_lpcomm_fota_active_fota_preparation_rsp_struct rsp = {0};

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    /* Do nothing. Reserved for the future extension. */
    rsp.status = RACE_ERRCODE_SUCCESS;
    race_fota_set_fota_mode(RACE_FOTA_MODE_ACTIVE);

    ret = race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                       sizeof(race_lpcomm_fota_active_fota_preparation_rsp_struct),
                                       RACE_LPCOMM_ROLE_PARTNER,
                                       packet->cmd_id,
                                       packet->app_id,
                                       packet->channel_id,
                                       packet->process_id,
                                       packet->trans_method,
                                       device_id);
    return ret;
}


RACE_ERRCODE race_lpcommp_fota_stop_result_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_stop_result_req_struct *req = NULL;
    race_fota_stop_partner_agent_result_req_struct result_req = {0};

    if (!packet || !packet->process_id ||
        packet->payload_len < sizeof(race_lpcomm_fota_stop_result_req_struct))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    req = (race_lpcomm_fota_stop_result_req_struct *)packet->payload;

    result_req.channel_id = packet->channel_id;
    result_req.process_id = packet->process_id;
    result_req.trans_method = packet->trans_method;
    result_req.originator = req->originator;
    result_req.reason = req->originator;
    result_req.result = req->result;

    return race_fota_stop_partner_result_req_process(&result_req);
}


RACE_ERRCODE race_lpcommp_fota_stop_query_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_stop_query_req_struct *req = NULL;
    race_fota_stop_partner_agent_query_req_struct query_req = {0};

    if (!packet || !packet->process_id ||
        packet->payload_len < sizeof(race_lpcomm_fota_stop_query_req_struct))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    req = (race_lpcomm_fota_stop_query_req_struct *)packet->payload;

    query_req.channel_id = packet->channel_id;
    query_req.originator = req->originator;
    query_req.reason = req->reason;
    query_req.process_id = packet->process_id;
    query_req.trans_method = packet->trans_method;

    return race_fota_stop_partner_query_req_process(&query_req);
}


RACE_ERRCODE race_lpcommp_fota_stop_query_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
#ifdef RACE_FOTA_CANCEL_FROM_PARTNER_ENABLE
    race_lpcomm_fota_stop_query_rsp_struct *rsp = NULL;
    race_fota_stop_originator_enum originator = RACE_FOTA_STOP_ORIGINATOR_PARTNER;
    race_fota_stop_reason_enum reason = RACE_FOTA_STOP_REASON_FAIL;
    race_fota_stop_partner_agent_query_rsp_struct query_rsp = {0};

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (RACE_FOTA_DOWNLOAD_STATE_STOPPING != race_fota_dl_state_get())
    {
        RACE_LOG_MSGID_W("Wrong state: %d",1, race_fota_dl_state_get());
        return RACE_ERRCODE_FAIL;
    }

    rsp = (race_lpcomm_fota_stop_query_rsp_struct *)packet->payload;

    if (RACE_ERRCODE_MAX_RETRY == rsp->status)
    {
        race_lpcomm_fake_rsp_struct *fake_rsp = (race_lpcomm_fake_rsp_struct *)packet->payload;
        race_lpcomm_fota_stop_query_req_struct *req = NULL;

        if (fake_rsp->req_len >= sizeof(race_lpcomm_fota_stop_query_req_struct))
        {
            req = (race_lpcomm_fota_stop_query_req_struct *)&(fake_rsp->req[0]);
            originator = req->originator;
            reason = req->reason;
        }

        rsp->status = RACE_ERRCODE_SUCCESS;
    }
    else
    {
        originator = rsp->originator;
        reason = rsp->reason;
    }

    RACE_LOG_MSGID_I("rsp->status:%d originator:%d reason:%d",3,
           rsp->status,
           originator,
           reason);

    query_rsp.channel_id = packet->channel_id;
    query_rsp.originator = originator;
    query_rsp.reason = reason;
    query_rsp.status = rsp->status;

    return race_fota_stop_partner_query_rsp_process(&query_rsp);
#else
    return RACE_ERRCODE_NOT_SUPPORT;
#endif
}


RACE_ERRCODE race_lpcommp_fota_ping_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_ping_rsp_struct rsp = {0};

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp.status = RACE_ERRCODE_SUCCESS;

    race_timer_smart_reset(race_fota_ping_timer_id_get());

    return race_lpcomm_packet_send_to_peer((uint8_t *)&rsp,
                                           sizeof(race_lpcomm_fota_ping_rsp_struct),
                                           RACE_LPCOMM_ROLE_PARTNER,
                                           RACE_LPCOMM_PACKET_TYPE_COMMON_RSP,
                                           packet->cmd_id,
                                           packet->app_id,
                                           packet->channel_id,
                                           packet->process_id,
                                           0,
                                           packet->trans_method,
                                           device_id);
}


#ifdef RACE_FOTA_INTEGRITY_CHECK_ENHANCE_ENABLE
RACE_ERRCODE race_lpcommp_fota_integrity_check_sha256_generate_cb(uint8_t status,
                                                                       uint32_t data_start_address,
                                                                       uint32_t data_length,
                                                                       uint8_t storage_type,
                                                                       unsigned char sha256_generated[RACE_STORAGE_SHA256_SIZE],
                                                                       void *user_data)
{
    uint32_t ret = RACE_ERRCODE_FAIL;
    race_lpcommp_fota_integrity_check_info_struct *cb_user_data = (race_lpcommp_fota_integrity_check_info_struct *)user_data;
    race_lpcomm_fota_check_integrity_rsp_struct rsp = {0};

    RACE_LOG_MSGID_I("status:%d data_start_address:%x data_length:%d storage_type:%d user_data:%x",5,
               status,
               data_start_address,
               data_length,
               storage_type,
               user_data);

    if (!cb_user_data)
    {
        RACE_LOG_MSGID_E("Empty user_data!", 0);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

#ifdef RACE_FOTA_SMART_INTEGRITY_CHECK
    g_lpcommp_fota_is_integritcy_check_on_going = FALSE;
#endif

    if (RACE_ERRCODE_SUCCESS == status)
    {
        status = race_fota_integrity_check(cb_user_data->signature_start_address,
                                           cb_user_data->storage_type,
                                           (uint8_t *)(&sha256_generated[0]),
                                           RACE_STORAGE_SHA256_SIZE);
    }
    rsp.status = status;
    rsp.storage_type = cb_user_data->storage_type;

    ret = race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                               sizeof(race_lpcomm_fota_check_integrity_rsp_struct),
                                               RACE_LPCOMM_ROLE_PARTNER,
                                               cb_user_data->cmd_id,
                                               cb_user_data->app_id,
                                               cb_user_data->channel_id,
                                               cb_user_data->process_id,
                                               cb_user_data->trans_method,
                                               cb_user_data->device_id);

    race_mem_free(cb_user_data);
    return ret;
}


RACE_ERRCODE race_lpcommp_fota_check_integrity_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_check_integrity_req_struct *req = NULL;
    race_lpcomm_fota_check_integrity_rsp_struct rsp = {0};
    int32_t ret = RACE_ERRCODE_FAIL;
    fota_integrity_check_type_enum integrity_check_type = FOTA_INTEGRITY_CHECK_TYPE_MAX;
    uint32_t signature_start_address = 0, data_start_address = 0;
    uint32_t data_length = 0;
    FotaStorageType storage_type = Invalid;
    unsigned char sha256_generated[RACE_STORAGE_SHA256_SIZE] = {0};
    race_lpcommp_fota_integrity_check_info_struct *user_data = NULL;

    if (!packet || !packet->process_id ||
        packet->payload_len < sizeof(race_lpcomm_fota_check_integrity_req_struct))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

#ifdef RACE_FOTA_SMART_INTEGRITY_CHECK
    if(g_lpcommp_fota_is_integritcy_check_on_going)
    {
        RACE_LOG_MSGID_W("Drop the packet. cmd_id:%x channel_id:%d because previous req is on-going.", 2, packet->cmd_id, packet->channel_id);
        return RACE_ERRCODE_SUCCESS;
    }
#endif

    req = (race_lpcomm_fota_check_integrity_req_struct *)packet->payload;
    rsp.storage_type = req->storage_type;

    ret = fota_get_integrity_check_info(&integrity_check_type,
                                        &signature_start_address,
                                        &data_start_address,
                                        &data_length,
                                        &storage_type);
    if (FOTA_ERRCODE_SUCCESS == ret)
    {
        RACE_LOG_MSGID_I("integrity_check data_start_address:%x data_length:%d storage_type:%d",3,
                   data_start_address,
                   data_length,
                   storage_type);
        if (req->storage_type == storage_type)
        {
            switch (integrity_check_type)
            {
                case FOTA_INTEGRITY_CHECK_TYPE_SHA256:
                {
                    /* A2. Create the noti(to Smart Phone). */
                    user_data = race_mem_alloc(sizeof(race_lpcommp_fota_integrity_check_info_struct));
                    if (user_data)
                    {
                        user_data->app_id = packet->app_id;
                        user_data->channel_id = packet->channel_id;
                        user_data->cmd_id = packet->cmd_id;
                        user_data->signature_start_address = signature_start_address;
                        user_data->storage_type = storage_type;
                        user_data->process_id = packet->process_id;
                        user_data->trans_method = packet->trans_method;
                        user_data->device_id = device_id;
                        ret = race_storage_nb_sha256_generate(data_start_address,
                                                              data_length,
                                                              storage_type,
                                                              sha256_generated,
                                                              race_lpcommp_fota_integrity_check_sha256_generate_cb,
                                                              (void *)user_data);
                        RACE_LOG_MSGID_I("race_storage_nb_sha256_generate ret:%d",1, ret);
                    }
                    else
                    {
                        ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
                        goto exit;
                    }

                    if (RACE_ERRCODE_MORE_OPERATION != ret)
                    {
                        /*  Get sha256 done. */
                        if (RACE_ERRCODE_SUCCESS == ret)
                        {
                            ret = race_lpcommp_fota_integrity_check_sha256_generate_cb(RACE_ERRCODE_SUCCESS,
                                                                                       data_start_address,
                                                                                       data_length,
                                                                                       storage_type,
                                                                                       sha256_generated,
                                                                                       (void *)user_data);
                            /* cb will free user_data and send RSP no matter what the return value is. */
                            user_data = NULL;
                            return ret;
                        }
                    }
                    else
                    {
                        /* race_storage_nb_sha256_generate() has made sure cb will always be invoked even if there're errors
                                              * during the process.
                                              */
#ifdef RACE_FOTA_SMART_INTEGRITY_CHECK
                        g_lpcommp_fota_is_integritcy_check_on_going = TRUE;
#endif
                        /* Integrity check is not finished. race_lpcommp_fota_integrity_check_sha256_generate_cb will send the rsp to Agent. */
                        return RACE_ERRCODE_SUCCESS;
                    }

                    break;
                }

                case FOTA_INTEGRITY_CHECK_TYPE_CRC32:
                case FOTA_INTEGRITY_CHECK_TYPE_SHA256_RSA:
                {
                    RACE_LOG_MSGID_E("integrity_check_type:%d not implemented.",1, integrity_check_type);
                    ret = RACE_ERRCODE_NOT_SUPPORT;
                    break;
                }

                default:
                {
                    ret = RACE_ERRCODE_NOT_SUPPORT;
                    break;
                }
            }
        }
        else
        {
            ret = RACE_ERRCODE_CONFLICT;
        }
    }
    else
    {
        ret = RACE_ERRCODE_FAIL;
    }

    rsp.status = ret;

exit:
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        if (user_data)
        {
            race_mem_free(user_data);
            user_data = NULL;
        }
    }

    return race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                                 sizeof(race_lpcomm_fota_check_integrity_rsp_struct),
                                                 RACE_LPCOMM_ROLE_PARTNER,
                                                 packet->cmd_id,
                                                 packet->app_id,
                                                 packet->channel_id,
                                                 packet->process_id,
                                                 packet->trans_method,
                                                 device_id);
}
#endif


RACE_ERRCODE race_lpcommp_fota_sp_ping_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_sp_ping_rsp_struct rsp = {0};

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (race_fota_is_race_fota_running())
    {
        rsp.status = RACE_ERRCODE_SUCCESS;
    }
    else
    {
        rsp.status = RACE_ERRCODE_FAIL;
    }

    return race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                                 sizeof(race_lpcomm_fota_sp_ping_rsp_struct),
                                                 RACE_LPCOMM_ROLE_PARTNER,
                                                 packet->cmd_id,
                                                 packet->app_id,
                                                 packet->channel_id,
                                                 packet->process_id,
                                                 packet->trans_method,
                                                 device_id);
}

#endif /* RACE_FOTA_CMD_ENABLE */


#ifdef RACE_STORAGE_CMD_ENABLE
RACE_ERRCODE race_lpcommp_storage_get_partition_sha256_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    int32_t ret = RACE_ERRCODE_FAIL;
    race_lpcomm_storage_get_partition_sha256_req_struct *req = NULL;
    race_lpcomm_storage_get_partition_sha256_rsp_struct rsp = {0};

    if (!packet || !packet->process_id ||
        packet->payload_len < sizeof(race_lpcomm_storage_get_partition_sha256_req_struct))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    req = (race_lpcomm_storage_get_partition_sha256_req_struct *)packet->payload;
    rsp.status = race_storage_sha256_generate(rsp.sha256,
                                               req->partition_address,
                                               req->partition_length,
                                               req->storage_type);
    ret = race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                       sizeof(race_lpcomm_storage_get_partition_sha256_rsp_struct),
                                       RACE_LPCOMM_ROLE_PARTNER,
                                       packet->cmd_id,
                                       packet->app_id,
                                       packet->channel_id,
                                       packet->process_id,
                                       packet->trans_method,
                                       device_id);

    return ret;
}


RACE_ERRCODE race_lpcommp_storage_get_4k_erased_status_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    int32_t ret = RACE_ERRCODE_FAIL;
    race_lpcomm_storage_get_4k_erase_status_req_struct *req = NULL;
    race_lpcomm_storage_get_4k_erased_status_rsp_struct rsp = {0};
    race_lpcomm_storage_get_4k_erased_status_rsp_struct *rsp_ptr = &rsp, *rsp_alloc_ptr = NULL;
    uint16_t erase_status_size = 0;

    if (!packet || !packet->process_id ||
        packet->payload_len < sizeof(race_lpcomm_storage_get_4k_erase_status_req_struct))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    req = (race_lpcomm_storage_get_4k_erase_status_req_struct *)packet->payload;
    ret = race_storage_get_partition_erase_status(NULL,
                                                  &erase_status_size,
                                                  req->partition_address,
                                                  req->partition_length,
                                                  req->storage_type);
    if (RACE_ERRCODE_SUCCESS == ret)
    {
        rsp_alloc_ptr = (race_lpcomm_storage_get_4k_erased_status_rsp_struct *)race_mem_alloc(sizeof(race_lpcomm_storage_get_4k_erased_status_rsp_struct) + erase_status_size);
        if (!rsp_alloc_ptr)
        {
            ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
        }
        else
        {
            memset(rsp_alloc_ptr, 0, sizeof(race_lpcomm_storage_get_4k_erased_status_rsp_struct) + erase_status_size);
            rsp_alloc_ptr->erase_status_size = erase_status_size;
            ret = race_storage_get_partition_erase_status(rsp_alloc_ptr->erase_status,
                                                          &(rsp_alloc_ptr->erase_status_size),
                                                          req->partition_address,
                                                          req->partition_length,
                                                          req->storage_type);
        }
    }

    if (RACE_ERRCODE_SUCCESS == ret)
    {
        rsp_ptr = rsp_alloc_ptr;
    }
    else
    {
        rsp_ptr->erase_status_size = 0;
    }

    rsp_ptr->status = ret;
    ret = race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)rsp_ptr,
                                       sizeof(race_lpcomm_storage_get_4k_erased_status_rsp_struct) + rsp_ptr->erase_status_size,
                                       RACE_LPCOMM_ROLE_PARTNER,
                                       packet->cmd_id,
                                       packet->app_id,
                                       packet->channel_id,
                                       packet->process_id,
                                       packet->trans_method,
                                       device_id);
    if (rsp_alloc_ptr)
    {
        race_mem_free(rsp_alloc_ptr);
    }

    return ret;
}


RACE_ERRCODE race_lpcommp_storage_dual_erase_partition_req_cb(uint8_t status,
                                                              uint8_t storage_type,
                                                              uint32_t partition_length,
                                                              uint32_t partition_address,
                                                              uint8_t app_id,
                                                              uint8_t channel_id,
                                                              bool noti_delay,
                                                              void *user_data)
{
    race_lpcomm_storage_dual_devices_erase_partition_rsp_struct rsp = {0};
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_lpcomm_packet_struct *req_packet = user_data;

    RACE_LOG_MSGID_I("status:%d storage_type:%d",2, status, storage_type);

    if (!req_packet)
    {
        RACE_LOG_MSGID_E("user_data used as process_id is not valid!", 0);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp.status = status;
    /* payload_len is reused as device_id. */
    ret = race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                       sizeof(race_lpcomm_storage_dual_devices_erase_partition_rsp_struct),
                                       RACE_LPCOMM_ROLE_PARTNER,
                                       req_packet->cmd_id,
                                       app_id,
                                       req_packet->channel_id,
                                       req_packet->process_id,
                                       req_packet->trans_method,
                                       (uint8_t)req_packet->payload_len);

    race_mem_free(req_packet);
    return ret;
}


RACE_ERRCODE race_lpcommp_storage_dual_devices_erase_partition_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    uint8_t ret = RACE_ERRCODE_FAIL;
    race_lpcomm_storage_dual_device_erase_partition_req_struct *req = NULL;
    race_lpcomm_storage_dual_devices_erase_partition_rsp_struct rsp = {0};
    race_lpcomm_packet_struct *req_packet = NULL;


    RACE_LOG_MSGID_I("payload_len:%d",1 , packet ? packet->payload_len : 0);
    if (!packet || !packet->process_id ||
        packet->payload_len < sizeof(race_lpcomm_storage_dual_device_erase_partition_req_struct))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    req = (race_lpcomm_storage_dual_device_erase_partition_req_struct *)packet->payload;

    RACE_LOG_MSGID_I("storage_type:%d length:%d address:%d",3,
               req->storage_type,
               req->partition_length,
               req->partition_address);

    /* Execute the cmd */
    ret = race_storage_is_addr_accessible(req->storage_type,
                                          req->partition_address,
                                          req->partition_length);
    if (RACE_ERRCODE_SUCCESS == ret)
    {
        req_packet = (race_lpcomm_packet_struct *)race_mem_alloc(sizeof(race_lpcomm_packet_struct));
        if (req_packet)
        {
            memcpy(req_packet, packet, sizeof(race_lpcomm_packet_struct));
            req_packet->payload_len = device_id; /* Reused as device_id. */
        	ret = race_storage_erase_partition(req->storage_type,
        									   req->partition_length,
        									   req->partition_address,
        									   packet->app_id,
        									   packet->channel_id,
        									   FALSE,
        									   race_lpcommp_storage_dual_erase_partition_req_cb,
        									   req_packet);
        }
    }

    RACE_LOG_MSGID_I("ret:%d",1, ret);

    if (RACE_ERRCODE_MORE_OPERATION == ret)
    {
        /* race_lpcommp_storage_dual_erase_partition_req_cb will be called after erase is done or
                 * any error occurs latter.
                 */
		ret = RACE_ERRCODE_SUCCESS;
	}
    else
    {
        rsp.status = ret;
        ret = race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                           sizeof(race_lpcomm_storage_dual_devices_erase_partition_rsp_struct),
                                           RACE_LPCOMM_ROLE_PARTNER,
                                           packet->cmd_id,
                                           packet->app_id,
                                           packet->channel_id,
                                           packet->process_id,
                                           packet->trans_method,
                                           device_id);
        if (req_packet)
        {
            race_mem_free(req_packet);
        }
    }

    return ret;
}


RACE_ERRCODE race_lpcommp_storage_lock_unlock_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    uint8_t ret = RACE_ERRCODE_FAIL;
    //race_lpcomm_storage_lock_unlock_req_struct *req = NULL;
    race_lpcomm_storage_lock_unlock_rsp_struct rsp = {0};

    if (!packet || !packet->process_id ||
        packet->payload_len < sizeof(race_lpcomm_storage_lock_unlock_req_struct))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    // req = (race_lpcomm_storage_lock_unlock_req_struct *)packet->payload;
    /*Do nothing  */

    rsp.status = RACE_ERRCODE_SUCCESS;

    ret = race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                       sizeof(race_lpcomm_storage_lock_unlock_rsp_struct),
                                       RACE_LPCOMM_ROLE_PARTNER,
                                       packet->cmd_id,
                                       packet->app_id,
                                       packet->channel_id,
                                       packet->process_id,
                                       packet->trans_method,
                                       device_id);

    return ret;
}
#endif /* RACE_STORAGE_CMD_ENABLE */


#ifdef RACE_BT_CMD_ENABLE
RACE_ERRCODE race_lpcommp_bt_get_battery_level_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    int32_t ret = RACE_ERRCODE_FAIL;
    race_lpcomm_bt_get_battery_level_rsp_struct rsp = {0};

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp.status = RACE_ERRCODE_SUCCESS;
    rsp.battery_level = race_get_battery_level();

    ret = race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                       sizeof(race_lpcomm_bt_get_battery_level_rsp_struct),
                                       RACE_LPCOMM_ROLE_PARTNER,
                                       packet->cmd_id,
                                       packet->app_id,
                                       packet->channel_id,
                                       packet->process_id,
                                       packet->trans_method,
                                       device_id);

    return ret;
}


RACE_ERRCODE race_lpcommp_bt_role_switch_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_bt_role_switch_rsp_struct rsp = {0};
    race_serial_port_type_enum cmd_port_type = RACE_SERIAL_PORT_TYPE_NONE;
    uint8_t timer_id = race_lpcomm_get_rho_timer_id();
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    cmd_port_type = race_get_port_type_by_channel_id(packet->channel_id);

    if (RACE_TIMER_INVALID_TIMER_ID == timer_id)
    {
        ret = race_timer_smart_start(&timer_id,
                                     RACE_TIMER_ROLE_SWITCH_PARTNER_TIMEOUT_IN_MS,
                                     race_lpcomm_rho_timer_expiration_hdl,
                                     NULL);
        if (RACE_ERRCODE_SUCCESS == ret)
        {
            race_lpcomm_set_rho_timer_id(timer_id);
        }
    }
    else
    {
        race_timer_smart_reset(timer_id);
    }

    if (RACE_ERRCODE_SUCCESS == ret &&
        !race_lpcomm_get_role_switch_enable())
    {
        race_send_event_notify_msg(RACE_EVENT_TYPE_BT_NEED_RHO, NULL);
        race_lpcomm_set_role_switch_enable(TRUE);
    }

    race_lpcomm_set_role_switch_cmd_port_type(cmd_port_type);
    rsp.status = ret;

    if (RACE_ERRCODE_SUCCESS == rsp.status)
    {
#ifdef RACE_FOTA_CMD_ENABLE
        if (race_fota_is_race_fota_running())
        {
            race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_RHOING);
        }
#endif
    }

    /* Agent would retry. So do not care if the rsp is sent successfully. If not, rho timer will handle it. */
    return race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                                 sizeof(race_lpcomm_bt_role_switch_rsp_struct),
                                                 RACE_LPCOMM_ROLE_PARTNER,
                                                 packet->cmd_id,
                                                 packet->app_id,
                                                 packet->channel_id,
                                                 packet->process_id,
                                                 packet->trans_method,
                                                 device_id);
}

#endif /* RACE_BT_CMD_ENABLE */

#ifdef RACE_FIND_ME_ENABLE
RACE_ERRCODE race_lpcommp_find_me_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    int32_t ret = RACE_ERRCODE_FAIL;
    race_lpcomm_find_me_req_struct *req = NULL;
    race_lpcomm_find_me_rsp_struct rsp = {0};


    if (!packet || !packet->process_id ||
        packet->payload_len < sizeof(race_lpcomm_find_me_req_struct))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }
    RACE_LOG_MSGID_I("[FIND_ME]find_me_req_hdl: id = %x \r\n",1, packet->process_id);

    req = (race_lpcomm_find_me_req_struct *)packet->payload;
	if (req)
    ret = race_event_send_find_me_event(req->is_blink, req->is_tone);
	rsp.status = ret;
	RACE_LOG_MSGID_I("[FIND_ME]find_me_req_hdl: ret = %x \r\n",1, rsp.status);
    ret = race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)&rsp,
                                       sizeof(race_lpcomm_storage_get_partition_sha256_rsp_struct),
                                       RACE_LPCOMM_ROLE_PARTNER,
                                       packet->cmd_id,
                                       packet->app_id,
                                       packet->channel_id,
                                       packet->process_id,
                                       packet->trans_method,
                                       device_id);



    return ret;
}
#endif /*RACE_FIND_ME_ENABLE*/


#ifdef RACE_CFU_ENABLE
RACE_ERRCODE race_lpcommp_cfu_relay_packet_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_cfu_relay_packet_lpcomm_req_struct *req = NULL;
    race_cfu_receive_callback recv_cb = race_cfu_get_recv_cb();
    race_cfu_handler_relay_packet_rsp_info_struct *rsp_info = race_cfu_handler_get_relay_packet_rsp_info();
    race_cfu_udpate_mode_enum update_mode = race_cfu_get_update_mode();

    if (!packet || !packet->process_id ||
        packet->payload_len < sizeof(race_cfu_relay_packet_lpcomm_req_struct))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    req = (race_cfu_relay_packet_lpcomm_req_struct *)packet->payload;

    /* Return a fail rsp to agent directly if update_mode is not dual_device. */
    if (RACE_CFU_UPDATE_MODE_DUAL_DEVICE != update_mode)
    {
        U32 lpcomm_rsp_len = sizeof(race_cfu_relay_packet_lpcomm_rsp_struct) + 1;
        race_cfu_relay_packet_lpcomm_rsp_struct *lpcomm_rsp = NULL;

        lpcomm_rsp = race_mem_alloc(lpcomm_rsp_len);
        if (lpcomm_rsp)
        {
            lpcomm_rsp->status = RACE_ERRCODE_FAIL;
            lpcomm_rsp->packet.packet_type = RACE_CFU_PACKET_TYPE_RSP;
            lpcomm_rsp->packet.packet_id = req->packet_id;
            lpcomm_rsp->packet.update_mode = update_mode;
            lpcomm_rsp->packet.data_len = 1;
            lpcomm_rsp->packet.data[0] = 0; /* report_count */
            race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)lpcomm_rsp,
                                                  lpcomm_rsp_len,
                                                  RACE_LPCOMM_ROLE_PARTNER,
                                                  packet->cmd_id,
                                                  packet->app_id,
                                                  packet->channel_id,
                                                  packet->process_id,
                                                  packet->trans_method,
                                                  device_id);
            race_mem_free(lpcomm_rsp);
        }
        else
        {
            RACE_LOG_MSGID_W("[CFU][RACE] No enough memory for lpcomm_rsp. lpcomm_rsp_len:%d", 1, lpcomm_rsp_len);
        }

        race_cfu_handler_reset_relay_packet_rsp_info();
#ifdef RACE_CFU_HANDLER_CLEAR_CACHED_INFO_TIMER_ENABLE
        race_cfu_handler_stop_clear_cached_info_timer();
#endif
        return RACE_ERRCODE_CONFLICT;
    }

    if (recv_cb && rsp_info)
    {
#ifdef RACE_CFU_HANDLER_CLEAR_CACHED_INFO_TIMER_ENABLE
        U8 timer_id = race_cfu_handler_get_clear_cached_info_timer_id();
#endif

        race_cfu_handler_reset_relay_packet_rsp_info();

        memcpy(&(rsp_info->packet), packet, sizeof(race_lpcomm_packet_struct));
        rsp_info->packet.payload_len = 0;
        rsp_info->device_id = device_id;
        rsp_info->packet_id = req->packet_id;

#ifdef RACE_CFU_HANDLER_CLEAR_CACHED_INFO_TIMER_ENABLE
        if (RACE_TIMER_INVALID_TIMER_ID == timer_id)
        {
            race_timer_smart_start(&timer_id,
                                   RACE_TIMER_CFU_CLEAR_CACHED_INFO_DELAY_IN_MS,
                                   race_cfu_handler_clear_cached_info_timer_hdl,
                                   NULL);
            race_cfu_handler_set_clear_cached_info_timer_id(timer_id);
        }
        else
        {
            race_timer_smart_reset(timer_id);
        }
#endif
        /* Notify the data to the local CFU device module. */
        recv_cb((race_cfu_packet_struct *)req);

        return RACE_ERRCODE_SUCCESS;
    }
    /* else do nothing. Agent will retry. */

    return RACE_ERRCODE_WRONG_STATE;
}
#endif /* RACE_CFU_ENABLE */

#endif /* RACE_LPCOMM_ENABLE */

