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
#include "FreeRTOSConfig.h"
#include "race_xport.h"
#include "race_lpcomm_msg_struct.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_recv.h"
#include "race_cmd_fota.h"
#include "race_cmd_storage.h"
#include "race_cmd_bluetooth.h"
#include "race_lpcomm_packet.h"
#include "race_lpcomm_retry.h"
#include "race_timer.h"
#include "race_noti.h"
#include "race_lpcomm_ps_noti.h"
#ifdef RACE_FOTA_CMD_ENABLE
#include "fota_multi_info_util.h"
#include "fota_multi_info.h"
#include "race_fota_util.h"
#include "race_fota.h"
#endif
#include "race_storage_access.h"

#ifdef RACE_FIND_ME_ENABLE
#include "race_cmd_find_me.h"
#endif
#ifdef RACE_CFU_ENABLE
#include "race_cmd_cfu.h"
#include "race_cfu.h"
#include "race_cfu_internal.h"
#include "race_cfu_handler.h"
#include "cfu_cmd_handler.h"
#include "cfu_conn_race.h"
#endif


#ifdef RACE_FOTA_CMD_ENABLE
RACE_ERRCODE race_lpcomma_fota_query_state_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_fota_get_version_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_fota_write_state_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_fota_new_transaction_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_fota_start_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_fota_query_partition_info_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_fota_commit_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_fota_active_fota_preparation_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_fota_stop_result_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_fota_stop_query_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_fota_stop_query_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_fota_ping_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

#ifdef RACE_FOTA_INTEGRITY_CHECK_ENHANCE_ENABLE
RACE_ERRCODE race_lpcomma_fota_check_integrity_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);
#endif

RACE_ERRCODE race_lpcomma_fota_sp_ping_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);
#endif /* RACE_FOTA_CMD_ENABLE */

#ifdef RACE_STORAGE_CMD_ENABLE
RACE_ERRCODE race_lpcomma_storage_get_partition_sha256_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_storage_get_4k_erased_status_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_storage_dual_devices_erase_partition_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_storage_lock_unlock_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);
#endif /* RACE_STORAGE_CMD_ENABLE */

#ifdef RACE_BT_CMD_ENABLE
RACE_ERRCODE race_lpcomma_bt_get_battery_level_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);

RACE_ERRCODE race_lpcomma_bt_role_switch_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);
#endif /* RACE_BT_CMD_ENABLE */

#ifdef RACE_FIND_ME_ENABLE
RACE_ERRCODE race_lpcomma_find_me_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);
#endif

#ifdef RACE_CFU_ENABLE
RACE_ERRCODE race_lpcommp_cfu_relay_packet_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id);
#endif


/* BEWARE: FAKE RSP may be received the structure of which is race_lpcomm_rsp_template_struct with the status of RACE_ERRCODE_FAIL.
  * Therefore, copy the results into noti, only when the status is RACE_ERRCODE_SUCCESS. Otherwise, invalid results are used.
  */
race_lpcomm_data_recv_hdl_struct g_lpcomm_agent_data_hdl_array[] = {
#ifdef RACE_FOTA_CMD_ENABLE
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_DUAL_DEVICES_QUERY_STATE, NULL, race_lpcomma_fota_query_state_rsp_hdl, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_GET_VERSION, NULL, race_lpcomma_fota_get_version_rsp_hdl, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_DUAL_DEVICES_WRITE_STATE, NULL, race_lpcomma_fota_write_state_rsp_hdl, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_DUAL_DEVICES_NEW_TRANSACTION, NULL, race_lpcomma_fota_new_transaction_rsp_hdl, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_START, NULL, race_lpcomma_fota_start_rsp_hdl, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_DUAL_DEVICES_QUERY_PARTITION_INFO, NULL, race_lpcomma_fota_query_partition_info_rsp_hdl, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_DUAL_DEVICES_COMMIT, NULL, race_lpcomma_fota_commit_rsp_hdl, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_ACTIVE_FOTA_PREPARATION, NULL, race_lpcomma_fota_active_fota_preparation_rsp_hdl, NULL},
#ifdef RACE_FOTA_INTEGRITY_CHECK_ENHANCE_ENABLE
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_CHECK_INTEGRITY, NULL, race_lpcomma_fota_check_integrity_rsp_hdl, NULL},
#endif
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_FOTA_PING, NULL, race_lpcomma_fota_sp_ping_rsp_hdl, NULL},
#endif /* RACE_FOTA_CMD_ENABLE */

#ifdef RACE_STORAGE_CMD_ENABLE
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_STORAGE_GET_PARTITION_SHA256, NULL, race_lpcomma_storage_get_partition_sha256_rsp_hdl, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_STORAGE_GET_4K_ERASED_STATUS, NULL, race_lpcomma_storage_get_4k_erased_status_rsp_hdl, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_STORAGE_LOCK_UNLOCK, NULL, race_lpcomma_storage_lock_unlock_rsp_hdl, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_STORAGE_DUAL_DEVICES_ERASE_PARTITION, NULL, race_lpcomma_storage_dual_devices_erase_partition_rsp_hdl, NULL},
#endif /* RACE_STORAGE_CMD_ENABLE */

#ifdef RACE_BT_CMD_ENABLE
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_BLUETOOTH_GET_BATTERY, NULL, race_lpcomma_bt_get_battery_level_rsp_hdl, NULL},
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_BLUETOOTH_DUAL_ROLE_SWITCH, NULL, race_lpcomma_bt_role_switch_rsp_hdl, NULL},
#endif /* RACE_BT_CMD_ENABLE */

#ifdef RACE_FOTA_CMD_ENABLE
    {RACE_LPCOMM_PACKET_CLASS_COMMON, RACE_LPCOMM_COMMON_CMD_ID_FOTA_STOP_RESULT, NULL, race_lpcomma_fota_stop_result_rsp_hdl, NULL},
    {RACE_LPCOMM_PACKET_CLASS_COMMON, RACE_LPCOMM_COMMON_CMD_ID_FOTA_STOP_QUERY, race_lpcomma_fota_stop_query_req_hdl, race_lpcomma_fota_stop_query_rsp_hdl, NULL},
    {RACE_LPCOMM_PACKET_CLASS_COMMON, RACE_LPCOMM_COMMON_CMD_ID_FOTA_PING, NULL, race_lpcomma_fota_ping_rsp_hdl, NULL},
#endif
#ifdef RACE_FIND_ME_ENABLE
     {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_CMD_FIND_ME, NULL, race_lpcomma_find_me_rsp_hdl, NULL},
#endif

#ifdef RACE_CFU_ENABLE
    {RACE_LPCOMM_PACKET_CLASS_RACE_CMD, RACE_CFU_RELAY_PACKET, NULL, race_lpcommp_cfu_relay_packet_rsp_hdl, NULL},
#endif

    {RACE_LPCOMM_PACKET_CLASS_NONE, RACE_INVALID_CMD_ID, NULL, NULL, NULL}
};


RACE_ERRCODE race_lpcomm_agent_data_recv_hdl(race_lpcomm_packet_struct *packet,
                                                        uint8_t device_id)
{
    int32_t i = 0;
    race_lpcomm_packet_type_enum packet_type = RACE_LPCOMM_PACKET_TYPE_NONE;
    race_lpcomm_data_recv_hdl_struct *data_recv_hdl_array = g_lpcomm_agent_data_hdl_array;

    RACE_LOG_MSGID_F("packet:%x, packet->process_id:%d, device_id:%d, data_recv_hdl_array:%x", 4,
               packet,
               packet ? packet->process_id : 0,
               device_id,
               data_recv_hdl_array);

    if (!packet || !packet->process_id || !data_recv_hdl_array)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    RACE_LOG_MSGID_I("packet_type:%x cmd_id:%x channel_id:%d process_id:%d trans_method:%d payload_len:%d",6,
                     packet->packet_type, packet->cmd_id, packet->channel_id, packet->process_id, packet->trans_method, packet->payload_len);
    packet_type = packet->packet_type;

    if (RACE_APP_ID_FOTA == packet->app_id &&
        ((RACE_LPCOMM_PACKET_TYPE_RACE_CMD_RSP == packet_type ||
          RACE_LPCOMM_PACKET_TYPE_COMMON_RSP == packet_type) &&
          !race_fota_is_race_fota_running()))
    {
        /* FOTA stop procedure does not remove the REQ from the retry list if it exists. Therefore, remove it here. */
        RACE_LOG_MSGID_W("Drop FOTA packet_type:%d cmd_id:%d because FOTA is not running right now.",2, packet_type, packet->cmd_id);
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
            else if ((RACE_LPCOMM_PACKET_TYPE_RACE_CMD_RSP == packet_type &&
                      RACE_LPCOMM_PACKET_CLASS_RACE_CMD == data_recv_hdl_array[i].packet_class) ||
                     (RACE_LPCOMM_PACKET_TYPE_COMMON_RSP == packet_type &&
                      RACE_LPCOMM_PACKET_CLASS_COMMON == data_recv_hdl_array[i].packet_class))
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
RACE_ERRCODE race_lpcomma_fota_query_state_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_query_state_rsp_struct *rsp = NULL;
    race_fota_dual_query_state_noti_struct *noti = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    bool noti_sent = FALSE;
    uint16_t agent_fota_state = 0xFFFF;
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_fota_query_state_rsp_struct *)packet->payload;

    noti = race_lpcomm_ps_noti_find(packet->process_id);
    if (noti)
    {
        agent_fota_state = noti->agent_fota_state;
        if (RACE_ERRCODE_SUCCESS == rsp->status)
        {
            noti->partner_fota_state = rsp->fota_state;
        }

        ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                           packet->process_id,
                                           packet->channel_id,
                                           rsp->status,
                                           RACE_LPCOMM_ROLE_PARTNER,
                                           FALSE);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            race_lpcomm_ps_noti_free(packet->process_id);
        }
    }
    else
    {
        fota_state_read(&agent_fota_state);
    }

    if (fota_cntx &&
        !fota_cntx->transfer_complete &&
        race_fota_is_race_fota_running() &&
        RACE_ERRCODE_SUCCESS == rsp->status &&
        0x0311 == rsp->fota_state &&
        0x0311 == agent_fota_state)
    {
        fota_cntx->transfer_complete = TRUE;
        /* Send the transfer complete event. */
        race_send_event_notify_msg(RACE_EVENT_TYPE_FOTA_TRANSFER_COMPLETE, NULL);
    }

// TODO: This does not work acutally when last time FOTA package is donwloaded and not committed.
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
    if (RACE_ERRCODE_SUCCESS == rsp->status &&
        0x0311 == agent_fota_state &&
        0x0311 == rsp->fota_state &&
        RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT != race_fota_dl_state_get())
    {
        RACE_LOG_MSGID_I("%x %x FOTA download complete. Wait for commit cmd to reboot.",2,
                   agent_fota_state,
                   rsp->fota_state);

        if (race_fota_is_active_mode())
        {
            race_fota_active_bt_preparation_revert();
        }

        race_fota_set_fota_mode(RACE_FOTA_MODE_MAX);
        race_storage_disable_fota_partition_accessibility();

        /* Stop app_id timer */
        race_timer_smart_stop(race_fota_app_id_timer_id_get(), NULL);
        race_fota_app_id_timer_id_set(RACE_TIMER_INVALID_TIMER_ID);

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


RACE_ERRCODE race_lpcomma_fota_get_version_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_get_version_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_fota_get_version_noti_struct *noti = NULL;
    uint16_t version_len = 0;

    RACE_LOG_MSGID_I("packet:%x",1, packet);

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_fota_get_version_rsp_struct *)packet->payload;
    if (RACE_ERRCODE_SUCCESS == rsp->status)
    {
        version_len = rsp->version_len;
    }
    noti = (void *)RACE_ClaimPacketAppID(packet->app_id,
                                         RACE_TYPE_NOTIFICATION,
                                         RACE_FOTA_GET_VERSION,
                                         sizeof(race_fota_get_version_noti_struct) + version_len,
                                         packet->channel_id);
    if (noti)
    {
        noti->status = rsp->status;
        noti->agent_or_partner = device_id;
        noti->version_len = version_len;
        if (RACE_ERRCODE_SUCCESS == rsp->status && noti->version_len)
        {
            memcpy(noti->version, rsp->version, noti->version_len);
        }

        ret = race_noti_send(noti, packet->channel_id, FALSE);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            RACE_FreePacket(noti);
        }
    }

    return ret;
}


RACE_ERRCODE race_lpcomma_fota_write_state_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_write_state_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    bool noti_sent = FALSE;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_fota_write_state_rsp_struct *)packet->payload;
    ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                       packet->process_id,
                                       packet->channel_id,
                                       rsp->status,
                                       RACE_LPCOMM_ROLE_PARTNER,
                                       FALSE);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        race_lpcomm_ps_noti_free(packet->process_id);
    }

    return ret;
}


RACE_ERRCODE race_lpcomma_fota_new_transaction_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_new_transaction_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    bool noti_sent = FALSE;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_fota_new_transaction_rsp_struct *)packet->payload;
    ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                       packet->process_id,
                                       packet->channel_id,
                                       rsp->status,
                                       RACE_LPCOMM_ROLE_PARTNER,
                                       FALSE);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        race_lpcomm_ps_noti_free(packet->process_id);
    }

    return ret;
}


RACE_ERRCODE race_lpcomma_fota_start_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_start_rsp_struct *rsp = NULL;
    RACE_ERRCODE noti_status = RACE_ERRCODE_FAIL, ret = RACE_ERRCODE_FAIL;
    bool noti_sent = FALSE;
    uint8_t timer_id = RACE_TIMER_INVALID_TIMER_ID;
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (!fota_cntx)
    {
        ret = RACE_ERRCODE_WRONG_STATE;
        goto exit;
    }

    rsp = (race_lpcomm_fota_start_rsp_struct *)packet->payload;
    if (RACE_ERRCODE_SUCCESS != rsp->status)
    {
        ret = rsp->status;
        goto exit;
    }

    ret = race_storage_enable_fota_partition_accessibility();
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        goto exit;
    }

    /* Start app_id timer */
    timer_id = race_fota_app_id_timer_id_get();
    if (RACE_TIMER_INVALID_TIMER_ID == timer_id)
    {
        ret = race_timer_smart_start(&timer_id,
                                            RACE_FOTA_APP_ID_TIMEOUT_IN_MS,
                                            race_fota_app_id_timer_expiration_hdl,
                                            NULL);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            goto exit;
        }

        race_fota_app_id_timer_id_set(timer_id);
    }

    /* Start ping if it has not been started. Starting ping only when the rsp from the partner is
         * success can handle the case below:
         * Agent FOTA is stopped and Partner FOTA is not. If next time, Partner rejects FOTA start
         * for it's not allowed according to its status, it will detect FOTA stop based on Ping eventually.
         */
    timer_id = race_fota_ping_timer_id_get();
    if (RACE_TIMER_INVALID_TIMER_ID == timer_id)
    {
        ret = race_timer_smart_start(&timer_id,
                                     RACE_FOTA_AGENT_PING_INTERVAL_IN_MS,
                                     race_fota_ping_timer_expiration_hdl,
                                     NULL);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            goto exit;
        }

        RACE_LOG_MSGID_I("start ping timer", 0);
        race_fota_ping_timer_id_set(timer_id);
    }

    if (RACE_ERRCODE_SUCCESS == ret)
    {
        ret = race_fota_send_ping_req_to_peer();
    }

exit:
    noti_status = ret;
    ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                       packet->process_id,
                                       packet->channel_id,
                                       noti_status,
                                       RACE_LPCOMM_ROLE_PARTNER,
                                       FALSE);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        race_lpcomm_ps_noti_free(packet->process_id);
        /* Failed to send 5D to SP.
                 * SP will retry or wait for Agent's app_id timer timeout. */
    }

    if (RACE_FOTA_DOWNLOAD_STATE_STARTING == race_fota_dl_state_get())
    {
        if (RACE_ERRCODE_SUCCESS == noti_status &&
            RACE_ERRCODE_SUCCESS == ret)
        {
            RACE_LOG_MSGID_I("Start FOTA for the first time. channel_id:%d",1, packet->channel_id);
            fota_cntx->lpcomm_peer_online = TRUE;
            race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_START);
            race_event_send_fota_start_event(fota_cntx->is_dual_fota, race_fota_is_active_mode());
            //race_send_event_notify_msg(RACE_EVENT_TYPE_FOTA_START, NULL);
        }
        else
        {
            race_fota_stop_agent_reset();
        }

        if (fota_cntx && fota_cntx->fota_stop_required)
        {
            RACE_LOG_MSGID_W("Execute the delayed FOTA_STOP during FOTA_START.", 0);
            fota_cntx->fota_stop_required = FALSE;
            race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_AGENT,
                           RACE_FOTA_STOP_REASON_CANCEL);
        }
    }
    /* else this is the retry cmd or the start after RHO, do nothing. Let app_id timer, ping
         * mechanism and AWS-IF-retry handle the potential errors. */

    return ret;
}


RACE_ERRCODE race_lpcomma_fota_query_partition_info_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_query_partition_info_rsp_struct *rsp = NULL;
    race_fota_dual_query_partition_info_noti_struct *noti = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    bool noti_sent = FALSE;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_fota_query_partition_info_rsp_struct *)packet->payload;
    noti = (race_fota_dual_query_partition_info_noti_struct *)race_lpcomm_ps_noti_find(packet->process_id);
    if (noti)
    {
        if (RACE_ERRCODE_SUCCESS == rsp->status)
        {
            noti->cli_storage_type = rsp->storage_type;
            noti->cli_partition_addr = rsp->partition_address;
            noti->cli_partition_len = rsp->partition_length;
        }

        ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                           packet->process_id,
                                           packet->channel_id,
                                           rsp->status,
                                           RACE_LPCOMM_ROLE_PARTNER,
                                           FALSE);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            race_lpcomm_ps_noti_free(packet->process_id);
        }
    }

    return ret;
}


RACE_ERRCODE race_lpcomma_fota_commit_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_commit_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;
    uint8_t timer_id = race_fota_commit_timer_id_get();

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_fota_commit_rsp_struct *)packet->payload;

    if (RACE_FOTA_DOWNLOAD_STATE_COMMITING == race_fota_dl_state_get())
    {
        if (RACE_ERRCODE_SUCCESS == rsp->status)
        {
            /* Integrity result check has been done before sending commit req to the partner. */
            ret = fota_upgrade_flag_set();
            if (RACE_ERRCODE_SUCCESS == ret)
            {
                /* Start the commit timer in order to reboot nearly at the same time with the agent. */
                RACE_LOG_MSGID_I("commit_timer_id:%d",1, timer_id);

                if (RACE_TIMER_INVALID_TIMER_ID == timer_id)
                {
                    ret = race_timer_smart_start(&timer_id,
                                                 RACE_TIMER_FOTA_COMMIT_DELAY_IN_MS,
                                                 race_fota_commit_timer_expiration_hdl,
                                                 NULL);

                    if (RACE_ERRCODE_SUCCESS == ret)
                    {
                        race_fota_commit_timer_id_set(timer_id);
                    }
                }
                else
                {
                    ret = race_timer_smart_reset(timer_id);
                }

                if (RACE_ERRCODE_SUCCESS != ret)
                {
                    /* Even if ret is not SUCCESS, reboot Agent because Partner will reboot for sure. */
                    race_event_send_fota_need_reboot_event(NULL, 0);
                    ret = RACE_ERRCODE_SUCCESS;
                }
            }
        }

        if (RACE_ERRCODE_SUCCESS != rsp->status ||
            RACE_ERRCODE_SUCCESS != ret)
        {
            RACE_LOG_MSGID_W("rsp->status:%d ret:%d",2, rsp->status, ret);
            fota_upgrade_flag_clear();

            if (RACE_TIMER_INVALID_TIMER_ID != timer_id)
            {
                void *user_data = NULL;

                race_timer_smart_stop(timer_id, &user_data);
                if (user_data)
                {
                    race_mem_free(user_data);
                    user_data = NULL;
                }
                race_fota_commit_timer_id_set(RACE_TIMER_INVALID_TIMER_ID);
            }
            race_fota_dl_state_rollback();
        }
    }

    return ret;
}


RACE_ERRCODE race_lpcomma_fota_active_fota_preparation_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_active_fota_preparation_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_fota_active_fota_preparation_noti_struct *noti = NULL;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_fota_active_fota_preparation_rsp_struct *)packet->payload;
    if (RACE_ERRCODE_SUCCESS == rsp->status)
    {
        /* Do nothing. Reserved for the future extension. */
    }
    noti = (void *)RACE_ClaimPacketAppID(packet->app_id,
                                         RACE_TYPE_NOTIFICATION,
                                         RACE_FOTA_ACTIVE_FOTA_PREPARATION,
                                         sizeof(race_fota_active_fota_preparation_noti_struct),
                                         packet->channel_id);
    if (noti)
    {
        noti->status = rsp->status;
        ret = race_noti_send(noti, packet->channel_id, FALSE);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            RACE_FreePacket(noti);
        }
    }

    return ret;
}


RACE_ERRCODE race_lpcomma_fota_stop_result_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_stop_result_rsp_struct *rsp = NULL;
    int8_t result = RACE_ERRCODE_SUCCESS;
    race_fota_stop_originator_enum originator = RACE_FOTA_STOP_ORIGINATOR_AGENT;
    race_fota_stop_reason_enum reason = RACE_FOTA_STOP_REASON_FAIL;
    race_fota_stop_agent_partner_result_rsp_struct result_rsp = {0};

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_fota_stop_result_rsp_struct *)packet->payload;

    if (RACE_ERRCODE_MAX_RETRY == rsp->status)
    {
        race_lpcomm_fake_rsp_struct *fake_rsp = (race_lpcomm_fake_rsp_struct *)packet->payload;
        race_lpcomm_fota_stop_result_req_struct *req = NULL;

        if (fake_rsp->req_len >= sizeof(race_lpcomm_fota_stop_result_req_struct))
        {
            req = (race_lpcomm_fota_stop_result_req_struct *)&(fake_rsp->req[0]);
            originator = req->originator;
            reason = req->reason;
            result = req->result;
        }

        rsp->status = RACE_ERRCODE_SUCCESS;
    }
    else
    {
        originator = rsp->originator;
        reason = rsp->reason;
        result = rsp->result;
    }

    result_rsp.channel_id = packet->channel_id;
    result_rsp.status = rsp->status;
    result_rsp.result = result;
    result_rsp.originator = originator;
    result_rsp.reason = reason;

    return race_fota_stop_agent_result_rsp_process(&result_rsp);
}


RACE_ERRCODE race_lpcomma_fota_stop_query_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_stop_query_rsp_struct *rsp = NULL;
    race_fota_stop_originator_enum originator = RACE_FOTA_STOP_ORIGINATOR_AGENT;
    race_fota_stop_reason_enum reason = RACE_FOTA_STOP_REASON_FAIL;
    race_fota_stop_agent_partner_query_rsp_struct query_rsp = {0};

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

    return race_fota_stop_agent_query_rsp_process(&query_rsp);
}


RACE_ERRCODE race_lpcomma_fota_ping_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_ping_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_fota_ping_rsp_struct *)packet->payload;

    if (RACE_ERRCODE_SUCCESS != rsp->status)
    {
        /* Stop ping timer */
        race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

        if (fota_cntx)
        {
            fota_cntx->lpcomm_peer_online = FALSE;
        }

        race_timer_smart_stop(race_fota_ping_timer_id_get(), NULL);
        race_fota_ping_timer_id_set(RACE_TIMER_INVALID_TIMER_ID);
        race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_AGENT,
                       RACE_FOTA_STOP_REASON_PARTNER_LOST);
    }

    return ret;
}


#ifdef RACE_FOTA_INTEGRITY_CHECK_ENHANCE_ENABLE
RACE_ERRCODE race_lpcomma_fota_check_integrity_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_check_integrity_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_fota_check_integrity_noti_struct *noti = NULL;
    uint8_t storage_type = 0xFF;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_fota_check_integrity_rsp_struct *)packet->payload;
    if (RACE_ERRCODE_MAX_RETRY == rsp->status)
    {
        race_lpcomm_fake_rsp_struct *fake_rsp = (race_lpcomm_fake_rsp_struct *)packet->payload;
        race_lpcomm_fota_check_integrity_req_struct *req = NULL;

        if (fake_rsp->req_len >= sizeof(race_lpcomm_fota_check_integrity_req_struct))
        {
            req = (race_lpcomm_fota_check_integrity_req_struct *)&(fake_rsp->req[0]);
            storage_type = req->storage_type;
        }
    }
    else
    {
        storage_type = rsp->storage_type;
    }

    noti = (void *)RACE_ClaimPacketAppID(packet->app_id,
                                         RACE_TYPE_NOTIFICATION,
                                         RACE_FOTA_CHECK_INTEGRITY,
                                         sizeof(race_fota_check_integrity_noti_struct) + sizeof(race_fota_check_integrity_recipient_param_struct),
                                         packet->channel_id);
    if (noti)
    {
        noti->status = rsp->status;
        noti->recipient_count = 1;
        noti->recipient_param[0].recipient = RACE_RECIPIENT_PARAM_RECIPIENT_PARTNER;
        noti->recipient_param[0].storage_type = storage_type;

        ret = race_noti_send(noti, packet->channel_id, FALSE);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            RACE_FreePacket(noti);
        }
    }

    return ret;
}
#endif


RACE_ERRCODE race_lpcomma_fota_sp_ping_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_fota_sp_ping_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_fota_ping_noti_struct *noti = NULL;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_fota_sp_ping_rsp_struct *)packet->payload;
    noti = (void *)RACE_ClaimPacketAppID(packet->app_id,
                                         RACE_TYPE_NOTIFICATION,
                                         RACE_FOTA_PING,
                                         sizeof(race_fota_ping_noti_struct) + sizeof(race_recipient_param_general_struct),
                                         packet->channel_id);
    if (noti)
    {
        noti->status = rsp->status;
        noti->recipient_count = 1;
        noti->recipient_param[0].recipient = RACE_RECIPIENT_PARAM_RECIPIENT_PARTNER;

        ret = race_noti_send(noti, packet->channel_id, FALSE);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            RACE_FreePacket(noti);
        }
    }

    return ret;
}


RACE_ERRCODE race_lpcomma_fota_stop_query_req_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
#ifdef RACE_FOTA_CANCEL_FROM_PARTNER_ENABLE
    race_lpcomm_fota_stop_query_req_struct *req = NULL;
    race_fota_stop_agent_partner_query_req_struct query_req = {0};

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

    return race_fota_stop_agent_query_req_process(&query_req);
#else
    return RACE_ERRCODE_NOT_SUPPORT;
#endif
}
#endif /* RACE_FOTA_CMD_ENABLE */


#ifdef RACE_STORAGE_CMD_ENABLE
RACE_ERRCODE race_lpcomma_storage_get_partition_sha256_rsp_hdl(race_lpcomm_packet_struct *packet,
                                                                              uint8_t device_id)
{
    race_lpcomm_storage_get_partition_sha256_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_storage_get_partition_sha256_noti_struct *noti = NULL;
    bool noti_sent = FALSE;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_storage_get_partition_sha256_rsp_struct *)packet->payload;

    noti = race_lpcomm_ps_noti_find(packet->process_id);
    if (noti)
    {
        if (RACE_ERRCODE_SUCCESS == rsp->status)
        {
            memcpy(noti->sha256, rsp->sha256, RACE_STORAGE_SHA256_SIZE);
        }

        ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                           packet->process_id,
                                           packet->channel_id,
                                           rsp->status,
                                           RACE_LPCOMM_ROLE_PARTNER,
                                           FALSE);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            race_lpcomm_ps_noti_free(packet->process_id);
        }
    }

    return ret;
}


RACE_ERRCODE race_lpcomma_storage_get_4k_erased_status_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_storage_get_4k_erased_status_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_storage_get_4k_erased_status_noti_struct *noti = NULL;
    bool noti_sent = FALSE;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_storage_get_4k_erased_status_rsp_struct *)packet->payload;

    noti = race_lpcomm_ps_noti_find(packet->process_id);
    if (noti)
    {
        if (RACE_ERRCODE_SUCCESS == rsp->status)
        {
            configASSERT(noti->erase_status_size == rsp->erase_status_size);
            memcpy(noti->erase_status, rsp->erase_status, noti->erase_status_size);
        }

        ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                           packet->process_id,
                                           packet->channel_id,
                                           rsp->status,
                                           RACE_LPCOMM_ROLE_PARTNER,
                                           FALSE);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            race_lpcomm_ps_noti_free(packet->process_id);
        }
    }

    return ret;
}


RACE_ERRCODE race_lpcomma_storage_dual_devices_erase_partition_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_storage_dual_devices_erase_partition_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_storage_dual_device_erase_partition_noti_struct *noti = NULL;
    bool noti_sent = FALSE;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_storage_dual_devices_erase_partition_rsp_struct *)packet->payload;

    noti = race_lpcomm_ps_noti_find(packet->process_id);
    if (noti)
    {
        ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                           packet->process_id,
                                           packet->channel_id,
                                           rsp->status,
                                           RACE_LPCOMM_ROLE_PARTNER,
                                           FALSE);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            race_lpcomm_ps_noti_free(packet->process_id);
        }
    }

    return ret;
}


RACE_ERRCODE race_lpcomma_storage_lock_unlock_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_storage_lock_unlock_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_storage_lock_unlock_noti_struct *noti = NULL;
    bool noti_sent = FALSE;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_storage_lock_unlock_rsp_struct *)packet->payload;

    noti = race_lpcomm_ps_noti_find(packet->process_id);
    if (noti)
    {
        ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                           packet->process_id,
                                           packet->channel_id,
                                           rsp->status,
                                           RACE_LPCOMM_ROLE_PARTNER,
                                           FALSE);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            race_lpcomm_ps_noti_free(packet->process_id);
        }
    }

    return ret;
}

#endif /* RACE_STORAGE_CMD_ENABLE */


#ifdef RACE_BT_CMD_ENABLE
RACE_ERRCODE race_lpcomma_bt_get_battery_level_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_bt_get_battery_level_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_bt_get_battery_level_noti_struct *noti = NULL;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    rsp = (race_lpcomm_bt_get_battery_level_rsp_struct *)packet->payload;

    noti = (void *)RACE_ClaimPacketAppID(packet->app_id,
                                         RACE_TYPE_NOTIFICATION,
                                         RACE_BLUETOOTH_GET_BATTERY,
                                         sizeof(race_bt_get_battery_level_noti_struct),
                                         packet->channel_id);
    if (noti)
    {
        noti->status = rsp->status;
        noti->agent_or_partner = device_id;
        if (RACE_ERRCODE_SUCCESS == noti->status)
        {
            noti->battery_level = rsp->battery_level;
        }

        ret = race_noti_send(noti, packet->channel_id, FALSE);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            RACE_FreePacket(noti);
        }
    }

    return ret;
}


RACE_ERRCODE race_lpcomma_bt_role_switch_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_lpcomm_bt_role_switch_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;
    bool noti_sent = FALSE;
    uint8_t timer_id = race_lpcomm_get_rho_timer_id();
    race_serial_port_type_enum cmd_port_type = RACE_SERIAL_PORT_TYPE_NONE;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    cmd_port_type = race_get_port_type_by_channel_id(packet->channel_id);
    rsp = (race_lpcomm_bt_role_switch_rsp_struct *)packet->payload;
    if (RACE_ERRCODE_SUCCESS == rsp->status)
    {
        if (RACE_TIMER_INVALID_TIMER_ID == timer_id)
        {
            ret = race_timer_smart_start(&timer_id,
                                         RACE_TIMER_ROLE_SWITCH_AGENT_TIMEOUT_IN_MS,
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

        rsp->status = ret;
    }

    ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                       packet->process_id,
                                       packet->channel_id,
                                       rsp->status,
                                       RACE_LPCOMM_ROLE_PARTNER,
                                       FALSE);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        race_lpcomm_ps_noti_free(packet->process_id);
    }

    if (!race_lpcomm_get_role_switch_enable())
    {
        if (RACE_ERRCODE_SUCCESS == rsp->status &&
            RACE_ERRCODE_SUCCESS == ret)
        {
            /* Agent sends RHO event on receiving the SPP/BLE DISC event if
             * RACE_ROLE_HANDOVER_SERVICE_ENABLE is not defined.
             */
            race_lpcomm_set_role_switch_enable(TRUE);
            race_lpcomm_set_role_switch_cmd_port_type(cmd_port_type);

#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
            /* Delay 100ms to avoid the last race cmd not being sent out successfully to the SP. */
            vTaskDelay(100);
            race_send_event_notify_msg(RACE_EVENT_TYPE_BT_NEED_RHO, NULL);
#endif
        }
        else
        {
            /* If the status of 5D is not success or fail to send 5D out, rollback the RHO modifications. */
            if (RACE_TIMER_INVALID_TIMER_ID != timer_id)
            {
                race_timer_smart_stop(timer_id, NULL);
                race_lpcomm_set_rho_timer_id(RACE_TIMER_INVALID_TIMER_ID);
            }

#ifdef RACE_FOTA_CMD_ENABLE
            if (RACE_FOTA_DOWNLOAD_STATE_RHOING == race_fota_dl_state_get())
            {
                race_fota_dl_state_rollback();
            }
#endif
        }
    }
    /* else this is the retry cmd, do nothing. Let RHO timer handle the potential errors. */

    return ret;
}
#ifdef RACE_FIND_ME_ENABLE
RACE_ERRCODE race_lpcomma_find_me_rsp_hdl(race_lpcomm_packet_struct *packet,
                                                                              uint8_t device_id)
{
    race_lpcomm_find_me_rsp_struct *rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_find_me_noti_struct *noti = NULL;
    bool noti_sent = FALSE;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }
    RACE_LOG_MSGID_I("[FIND_ME]find_me_rsp_hdl: id = %x \r\n",1, packet->process_id);

    rsp = (race_lpcomm_find_me_rsp_struct *)packet->payload;

    noti = race_lpcomm_ps_noti_find(packet->process_id);
    if (noti)
    {
    	RACE_LOG_MSGID_I("[FIND_ME]find_me_rsp_hdl:noti, status = %x, recipient = %x \r\n",2, rsp->status, noti->recipient);
        noti->status = rsp->status;
        ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                           packet->process_id,
                                           packet->channel_id,
                                           rsp->status,
                                           RACE_LPCOMM_ROLE_PARTNER,
                                           FALSE);
		RACE_LOG_MSGID_I("[FIND_ME]find_me_rsp_hdl:noti, ret = %x \r\n",1, ret);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            race_lpcomm_ps_noti_free(packet->process_id);
        }
    }

    return ret;
}
#endif /*RACE_FIND_ME_ENABLE*/
#endif /* RACE_BT_CMD_ENABLE */


#ifdef RACE_CFU_ENABLE
RACE_ERRCODE race_lpcommp_cfu_relay_packet_rsp_hdl(race_lpcomm_packet_struct *packet, uint8_t device_id)
{
    race_cfu_relay_packet_lpcomm_rsp_struct *lpcomm_rsp = NULL;
    race_cfu_relay_packet_response_struct *rsp = NULL, *update_content_rsp = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;

    if (!packet || !packet->process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    lpcomm_rsp = (race_cfu_relay_packet_lpcomm_rsp_struct *)packet->payload;

    if (RACE_ERRCODE_MAX_RETRY == lpcomm_rsp->status)
    {
        race_lpcomm_fake_rsp_struct *fake_rsp = (race_lpcomm_fake_rsp_struct *)packet->payload;

        if (fake_rsp->req_len >= sizeof(race_cfu_relay_packet_lpcomm_req_struct))
        {
            /* Reuse the buffer of fake_rsp->req (race_cfu_relay_packet_command_struct) as rsp. */
            rsp = (race_cfu_relay_packet_response_struct *)fake_rsp->req;
            rsp->packet_type = RACE_CFU_PACKET_TYPE_RSP;
        }
    }
    else
    {
        rsp = &(lpcomm_rsp->packet);
    }

    if (rsp)
    {
        RACE_LOG_MSGID_I("[CFU] partner_rsp: rsp_status=%d, data_len=%d, ", 2, lpcomm_rsp->status, rsp->data_len);
        if (RACE_ERRCODE_SUCCESS == lpcomm_rsp->status && rsp->data_len == (sizeof(cfu_content_aws_response_t) + 1))
        {
            update_content_rsp = race_mem_alloc(sizeof(race_cfu_packet_struct) + sizeof(cfu_content_response_t) + 1);
            if (update_content_rsp)
            {
                memcpy(update_content_rsp, rsp, sizeof(race_cfu_packet_struct));
                update_content_rsp->data_len = sizeof(cfu_content_response_t) + 1;
                update_content_rsp->data[0]  = rsp->data[0];
                cfu_race_convert_to_content_rsp((cfu_content_aws_response_t *)(rsp->data+1), (cfu_content_response_t *)(update_content_rsp->data+1));
                rsp = update_content_rsp;
            }
            else
            {
                RACE_LOG_MSGID_I("[CFU] partner_rsp memory malloc fail.", 0);
                lpcomm_rsp->status = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
            }
        }

        if (RACE_ERRCODE_SUCCESS != lpcomm_rsp->status)
        {
            /* Fail to get the rsp for the cmd. */
            rsp->data_len = 1;
            rsp->data[0] = 0; /* report_count */
        }

        race_cfu_handler_process_relay_packet_response((race_cfu_packet_struct *)rsp, FALSE);

        ret = RACE_ERRCODE_SUCCESS;
    }

    if (update_content_rsp)
    {
        race_mem_free(update_content_rsp);
    }

    return ret;
}
#endif /* RACE_CFU_ENABLE */


#endif /* RACE_LPCOMM_ENABLE */

