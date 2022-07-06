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
#ifdef RACE_AWS_ENABLE
#include "bt_sink_srv.h"
#include "race_lpcomm_aws.h"
#endif
#include "race_lpcomm_trans.h"
#include "race_lpcomm_packet.h"
#include "race_lpcomm_retry.h"
#include "race_xport.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_conn.h"
#include "race_timer.h"
#include "race_bt.h"
#ifdef RACE_COSYS_ENABLE
#include "race_cmd_co_sys.h"
#endif


#ifdef RACE_AWS_ENABLE
static void race_lpcomm_aws_header_init(uint8_t *packet, uint16_t payload_len);
static RACE_ERRCODE race_lpcomm_aws_packet_send(uint8_t *packet, uint8_t device_id);
#endif /* RACE_AWS_ENABLE */

#ifdef RACE_COSYS_ENABLE
static RACE_ERRCODE race_lpcomm_cosys_packet_send(uint8_t *packet, uint8_t device_id);
#endif


const race_lpcomm_trans_info_struct g_race_lpcomm_tm_array[] = {
#ifdef RACE_AWS_ENABLE
    {RACE_LPCOMM_TRANS_METHOD_AWS, RACE_LPCOMM_AWS_HEADR_LEN, race_lpcomm_aws_header_init, race_lpcomm_aws_packet_send, TRUE},
#endif /* RACE_AWS_ENABLE */
#ifdef RACE_COSYS_ENABLE
    {RACE_LPCOMM_TRANS_METHOD_COSYS, RACE_LPCOMM_COSYS_HEADR_LEN, NULL, race_lpcomm_cosys_packet_send, TRUE},
#endif /* RACE_COSYS_ENABLE */
    {RACE_LPCOMM_TRANS_METHOD_NONE, 0, NULL, FALSE},
};


race_lpcomm_trans_info_struct *race_lpcomm_trans_find_info(race_lpcomm_trans_method_enum method)
{
    int i = 0;

    while (RACE_LPCOMM_TRANS_METHOD_NONE != g_race_lpcomm_tm_array[i].method)
    {
        if (g_race_lpcomm_tm_array[i].method == method)
        {
            return (race_lpcomm_trans_info_struct *)&g_race_lpcomm_tm_array[i];
        }
        i++;
    }

    return NULL;
}


RACE_ERRCODE race_lpcomm_packet_send(uint8_t *packet, race_lpcomm_trans_method_enum method, uint8_t device_id)
{
    race_lpcomm_trans_info_struct *method_info = race_lpcomm_trans_find_info(method);

    RACE_LOG_MSGID_I("packet:%x, method:%d",2, packet, method);
    if (!packet || !method_info || !method_info->send_func)
    {
        return RACE_ERRCODE_FAIL;
    }

    return method_info->send_func(packet, device_id);
}


RACE_ERRCODE race_lpcomm_packet_send_to_peer(uint8_t *payload,
                                                         uint16_t payload_len,
                                                         uint8_t sender_role,
                                                         uint8_t packet_type,
                                                         uint16_t cmd_id,
                                                         uint8_t app_id,
                                                         uint8_t channel_id,
                                                         uint16_t process_id,
                                                         uint8_t max_retry_time,
                                                         race_lpcomm_trans_method_enum method,
                                                         uint8_t device_id)
{
    uint8_t *packet = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    race_lpcomm_trans_info_struct *method_info = race_lpcomm_trans_find_info(method);
#ifdef RACE_LPCOMM_RETRY_ENABLE
    race_lpcomm_retry_list_struct *retry_list_node = NULL;
#endif
    bool insert_into_list = FALSE;

    if (!method_info || !method_info->send_func ||
        (!payload && payload_len) ||
        (payload && !payload_len) ||
        RACE_LPCOMM_PACKET_TYPE_MAX <= packet_type ||
        RACE_APP_ID_MAX <= app_id ||
#ifdef RACE_LPCOMM_SENDER_ROLE_ENABLE
        RACE_LPCOMM_ROLE_NONE == sender_role ||
#endif
        RACE_LPCOMM_ROLE_MAX <= sender_role)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

#ifdef RACE_LPCOMM_RETRY_ENABLE
    if (method_info->retry_enable &&
        (max_retry_time ||
         RACE_LPCOMM_PACKET_TYPE_RACE_CMD_REQ == packet_type ||
         RACE_LPCOMM_PACKET_TYPE_COMMON_REQ == packet_type))
    {
            insert_into_list = TRUE;
    }
#endif

    /* 1. Create packet */
    packet = race_lpcomm_packet_create(payload,
                                       payload_len,
                                       sender_role,
                                       packet_type,
                                       cmd_id,
                                       app_id,
                                       channel_id,
                                       process_id,
                                       method);
    if (!packet)
    {
        ret = RACE_ERRCODE_FAIL;
        goto exit;
    }

    /* 2. Add into retry list if needed */
#ifdef RACE_LPCOMM_RETRY_ENABLE
    if (insert_into_list)
    {
        /* Comment init in race_init() and uncomment below to dynamically
                 * allocate retry context. */
        // race_lpcomm_retry_init();
        retry_list_node = race_lpcomm_retry_list_node_alloc();
        if (!retry_list_node)
        {
            ret = RACE_ERRCODE_FAIL;
            goto exit;
        }

        retry_list_node->data = packet;
        retry_list_node->method = method;
        retry_list_node->device_id = device_id;
        retry_list_node->max_retry_time = max_retry_time;
        ret = race_lpcomm_retry_list_insert(retry_list_node);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            goto exit;
        }
    }
#endif

    /* 3. Send the packet */
    ret = race_lpcomm_packet_send(packet, method, device_id);
    RACE_LOG_MSGID_I("race_lpcomm_packet_send ret:%x",1, ret);

#ifdef RACE_LPCOMM_RETRY_ENABLE
    /* 4. Start the retry timer if needed. */
    if (insert_into_list)
    {
        ret = race_timer_start(1000);
    }
#endif

exit:
    if (!insert_into_list || RACE_ERRCODE_SUCCESS != ret)
    {
#ifdef RACE_LPCOMM_RETRY_ENABLE
        if (retry_list_node)
        {
            race_lpcomm_retry_list_remove(retry_list_node);
            race_lpcomm_retry_list_node_free(retry_list_node);
        }
#endif

        /* 5. Free the packet if needed.
         * 1). Free every packet without retry
         * 2). Free the packet with retry only when ret != RACE_ERRCODE_SUCCESS
         * Only when the packet is inserted into the retry list successfully, will the noti and the packet be linked together.
         * Therefore, do not free the noti the process_id of which is the same as the one of the packet here because they
         * are not linked.
         */
        if (packet)
        {
            race_lpcomm_packet_free(packet);
        }
    }

    return ret;
}


/* If RACE_LPCOMM_MULTIPLE_LINK_ENABLE is defined, the SP should provide the device_id.
  *   If it's not defined, use RACE_LPCOMM_DEFAULT_DEVICE_ID.*/
RACE_ERRCODE race_lpcomm_send_race_cmd_req_to_peer(uint8_t *req,
                                                     uint16_t req_len,
                                                     uint8_t sender_role,
                                                     uint16_t cmd_id,
                                                     uint8_t app_id,
                                                     uint8_t channel_id,
                                                     uint16_t process_id,
                                                     race_lpcomm_trans_method_enum method,
                                                     uint8_t device_id)
{
    return race_lpcomm_packet_send_to_peer(req,
                                           req_len,
                                           sender_role,
                                           RACE_LPCOMM_PACKET_TYPE_RACE_CMD_REQ,
                                           cmd_id,
                                           app_id,
                                           channel_id,
                                           process_id,
                                           RACE_LPCOMM_RETRY_MAX_TIME,
                                           method,
                                           device_id);
}


RACE_ERRCODE race_lpcomm_send_race_cmd_rsp_to_peer(uint8_t *rsp,
                                              uint16_t rsp_len,
                                              uint8_t sender_role,
                                              uint16_t cmd_id,
                                              uint8_t app_id,
                                              uint8_t channel_id,
                                              uint16_t process_id,
                                              race_lpcomm_trans_method_enum method,
                                              uint8_t device_id)
{
    return race_lpcomm_packet_send_to_peer(rsp,
                                           rsp_len,
                                           sender_role,
                                           RACE_LPCOMM_PACKET_TYPE_RACE_CMD_RSP,
                                           cmd_id,
                                           app_id,
                                           channel_id,
                                           process_id,
                                           0,
                                           method,
                                           device_id);
}


/* If RACE_LPCOMM_MULTIPLE_LINK_ENABLE is defined, race_lpcomm_attach_proc() will gen a device_id.
  * If it's not defined, use RACE_LPCOMM_DEFAULT_DEVICE_ID.*/
RACE_ERRCODE race_lpcomm_send_noti_to_peer(uint8_t *noti,
                                                     uint16_t noti_len,
                                                     uint8_t sender_role,
                                                     uint16_t cmd_id,
                                                     uint8_t app_id,
                                                     uint8_t channel_id,
                                                     race_lpcomm_trans_method_enum method,
                                                     uint8_t device_id)

{
    uint16_t process_id = race_gen_process_id();

    return race_lpcomm_packet_send_to_peer(noti,
                                           noti_len,
                                           sender_role,
                                           RACE_LPCOMM_PACKET_TYPE_COMMON_NOTI,
                                           cmd_id,
                                           app_id,
                                           channel_id,
                                           process_id,
                                           0,
                                           method,
                                           device_id);
}


#ifdef RACE_AWS_ENABLE
static void race_lpcomm_aws_header_init(uint8_t *packet, uint16_t payload_len)
{
    bt_aws_mce_report_info_t *aws_packet = (bt_aws_mce_report_info_t *)packet;

    if (!aws_packet)
    {
        return;
    }

    memset(aws_packet, 0, RACE_LPCOMM_AWS_HEADR_LEN);
    aws_packet->module_id = BT_AWS_MCE_REPORT_MODULE_FOTA;
    aws_packet->param_len = payload_len;
    aws_packet->param = (void *)(packet + RACE_LPCOMM_AWS_HEADR_LEN);
}


static RACE_ERRCODE race_lpcomm_aws_packet_send(uint8_t *packet, uint8_t device_id)
{
    bt_aws_mce_report_info_t *aws_packet = (bt_aws_mce_report_info_t *)packet;
    int32_t ret = RACE_ERRCODE_FAIL;

    RACE_LOG_MSGID_I("packet:%x, device_id:%d",2, packet, device_id);

    if (!aws_packet)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (!race_lpcomm_is_attached(device_id))
    {
        return RACE_ERRCODE_CONNECTION_BROKEN;
    }

    ret = bt_aws_mce_report_send_event(aws_packet);

    RACE_LOG_MSGID_I("send aws data ret:%x",1, ret);
    ret = BT_STATUS_SUCCESS == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;

    return ret;
}
#endif /* RACE_AWS_ENABLE */


#ifdef RACE_COSYS_ENABLE
static RACE_ERRCODE race_lpcomm_cosys_packet_send(uint8_t *packet, uint8_t device_id)
{
    race_lpcomm_packet_struct *lpcomm_packet = (race_lpcomm_packet_struct *)packet;
    bool ret = FALSE;

    RACE_LOG_MSGID_I("packet:%x, device_id:%d",2, packet, device_id);

    if (!lpcomm_packet)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (!race_lpcomm_is_attached(device_id))
    {
        return RACE_ERRCODE_CONNECTION_BROKEN;
    }

    ret = race_cosys_send_data(RACE_COSYS_MODULE_ID_LPCOMM, FALSE, packet, RACE_LPCOMM_PACKET_HEADER_SIZE + lpcomm_packet->payload_len);

    RACE_LOG_MSGID_I("send cosys data ret:%x",1, ret);
    return ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
}
#endif /* RACE_COSYS_ENABLE */


#endif /* RACE_LPCOMM_ENABLE */

