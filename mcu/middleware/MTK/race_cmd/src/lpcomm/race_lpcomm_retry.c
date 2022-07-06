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
#ifdef RACE_LPCOMM_RETRY_ENABLE
#include "FreeRTOS.h"
#include "timers.h"
#include "bt_sink_srv.h"
#include "race_xport.h"
#include "race_util.h"
#include "race_lpcomm_retry.h"
#include "race_lpcomm_packet.h"
#include "race_lpcomm_msg_struct.h"
#include "race_timer.h"
#include "race_lpcomm_trans.h"
#include "race_lpcomm_ps_list.h"
#include "race_lpcomm_ps.h"
#include "race_lpcomm_ps_noti.h"
#include "race_util.h"
#ifdef RACE_FOTA_CMD_ENABLE
#include "race_fota.h"
#include "race_fota_util.h"
#endif

#define RACE_LPCOMM_RETRY_LIST_NODES_MAX_SIZE        (10)


#define RACE_LPCOMM_RETRY_GET_SUB_ABS(x,y)          ((x) > (y) ? ((x) - (y)) : ((y) - (x)))


typedef struct
{
    race_lpcomm_retry_list_struct retry_list_nodes[RACE_LPCOMM_RETRY_LIST_NODES_MAX_SIZE];
    race_lpcomm_retry_list_struct *retry_list;
}race_lpcomm_retry_context_struct;


static race_lpcomm_retry_context_struct *g_race_lpcomm_retry_cntx;


static RACE_ERRCODE race_lpcomm_retry_list_destory(void);

/**
* @brief      This function generates a fake response the status of which is RACE_ERRCODE_MAX_RETRY. And it also contains the original
* request content.
* @param[in] req_packet, a pointer points to the request packet.
* @param[in] device_id, the device identification.
*/
RACE_ERRCODE race_lpcomm_retry_fake_rsp(race_lpcomm_packet_struct *req_packet, uint8_t device_id);


bool race_lpcomm_retry_init(void)
{
    if (g_race_lpcomm_retry_cntx)
    {
        return TRUE;
    }

    g_race_lpcomm_retry_cntx = race_mem_alloc(sizeof(race_lpcomm_retry_context_struct));
    if (g_race_lpcomm_retry_cntx)
    {
        memset(g_race_lpcomm_retry_cntx, 0, sizeof(race_lpcomm_retry_context_struct));
        return TRUE;
    }

    return FALSE;
}


static void race_lpcomm_retry_reset(void)
{
    RACE_LOG_MSGID_I("race_lpcomm_retry_reset", 0);
    if (g_race_lpcomm_retry_cntx)
    {
        race_lpcomm_retry_list_destory();

        memset(g_race_lpcomm_retry_cntx, 0, sizeof(race_lpcomm_retry_context_struct));
    }
}


void race_lpcomm_retry_deinit(void)
{
    RACE_LOG_MSGID_I("race_lpcomm_retry_deinit", 0);

    race_lpcomm_retry_reset();
    if (g_race_lpcomm_retry_cntx)
    {
        race_mem_free(g_race_lpcomm_retry_cntx);
        g_race_lpcomm_retry_cntx = NULL;
    }
}


race_lpcomm_retry_list_struct *race_lpcomm_retry_list_node_alloc(void)
{
    uint32_t i = 0;

    if (!g_race_lpcomm_retry_cntx)
    {
        return NULL;
    }

    for (i = 0; i < RACE_LPCOMM_RETRY_LIST_NODES_MAX_SIZE; i++)
    {
        if (!g_race_lpcomm_retry_cntx->retry_list_nodes[i].is_used)
        {
            // RACE_LOG_MSGID_I("Allocate a retry_list_node idx:%d.", 1, i);
            memset(&(g_race_lpcomm_retry_cntx->retry_list_nodes[i]), 0, sizeof(race_lpcomm_retry_list_struct));
            g_race_lpcomm_retry_cntx->retry_list_nodes[i].is_used = TRUE;
            return &(g_race_lpcomm_retry_cntx->retry_list_nodes[i]);
        }
    }

    RACE_LOG_MSGID_E("Failed to allocate a retry_list_node.", 0);
    return NULL;
}


void race_lpcomm_retry_list_node_free(void *node)
{
    race_lpcomm_retry_list_struct *list_node = (race_lpcomm_retry_list_struct *)node;

    if (list_node)
    {
        list_node->is_used = FALSE;
    }
}


RACE_ERRCODE race_lpcomm_retry_list_insert(race_lpcomm_retry_list_struct *list_node)
{
    if (!g_race_lpcomm_retry_cntx)
    {
        return RACE_ERRCODE_FAIL;
    }

    if (list_node)
    {
        list_node->curr_time_in_ms = race_get_curr_time_in_ms();
    }

    return race_list_insert((race_template_list_struct **)&(g_race_lpcomm_retry_cntx->retry_list),
                            (race_template_list_struct *)list_node);
}


RACE_ERRCODE race_lpcomm_retry_list_remove(race_lpcomm_retry_list_struct *list_node)
{
    if (!g_race_lpcomm_retry_cntx)
    {
        return RACE_ERRCODE_FAIL;
    }

    return race_list_remove((race_template_list_struct **)&(g_race_lpcomm_retry_cntx->retry_list),
                            (race_template_list_struct *)list_node);
}


static RACE_ERRCODE race_lpcomm_retry_list_destory(void)
{
    race_lpcomm_retry_list_struct *list_node = NULL, *list_node_next = NULL;

    if (!g_race_lpcomm_retry_cntx)
    {
        return RACE_ERRCODE_SUCCESS;
    }

    list_node = g_race_lpcomm_retry_cntx->retry_list;

    while (list_node)
    {
        list_node_next = list_node->next;

        race_lpcomm_retry_drop_packet(list_node);

        list_node = list_node_next;
    }

    return RACE_ERRCODE_SUCCESS;
}


/* Drop the packet in the retry list. */
RACE_ERRCODE race_lpcomm_retry_drop_packet(race_lpcomm_retry_list_struct *list_node)
{
    race_lpcomm_packet_struct *lpcomm_packet = NULL;

    if (!list_node)
    {
        return RACE_ERRCODE_SUCCESS;
    }

    /* Only when is_used is true, is the list_node->data valid. */
    if (list_node->is_used)
    {
        lpcomm_packet = race_lpcomm_packet_header_get((uint8_t *)list_node->data,
                                                      list_node->method);
        if (lpcomm_packet)
        {
            RACE_LOG_MSGID_I("Drop packet. packet_type:%d cmd_id:%d process_id:%d chn_id:%d",4,
                             lpcomm_packet->packet_type,
                             lpcomm_packet->cmd_id,
                             lpcomm_packet->process_id,
                             lpcomm_packet->channel_id);


            /* Do not try to free the noti if the packet type is not REQ because only REQ will create noti. Otherwise, if non-REQ
            * is inserted into the retry list by mistake and its process_id is the same as one of the notifications created by
            * another REQ, the noti will be freed wrongly.
            */
#ifdef RACE_LPCOMM_SENDER_ROLE_ENABLE
            lpcomm_packet->packet_type = RACE_LPCOMM_PACKET_GET_PACKET_TYPE(lpcomm_packet->packet_type);
#endif
            if ((RACE_LPCOMM_PACKET_TYPE_RACE_CMD_REQ == lpcomm_packet->packet_type) ||
                (RACE_LPCOMM_PACKET_TYPE_COMMON_REQ == lpcomm_packet->packet_type))
            {
                race_lpcomm_ps_noti_free(lpcomm_packet->process_id);
            }
        }

        race_lpcomm_packet_free(list_node->data);
    }

    /* g_race_lpcomm_retry_cntx->retry_list will be set to NULL if the list is empty by race_lpcomm_retry_list_remove(). */
    race_lpcomm_retry_list_remove(list_node);
    race_lpcomm_retry_list_node_free(list_node);

    return RACE_ERRCODE_SUCCESS;
}


/* Cancel the AWS packets in the retry-list. */
RACE_ERRCODE race_lpcomm_retry_cancel(uint8_t app_id, bool gen_fake_rsp)
{
    race_lpcomm_retry_list_struct *list_node = NULL, *list_node_next = NULL;
    race_lpcomm_packet_struct *lpcomm_packet = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;

    RACE_LOG_MSGID_I("app_id:%d",1, app_id);

    if (!g_race_lpcomm_retry_cntx)
    {
        return RACE_ERRCODE_SUCCESS;
    }

    list_node = g_race_lpcomm_retry_cntx->retry_list;

    while (list_node)
    {
        /* list_node may be removed. To traverse the whole list, store the next list node here. */
        list_node_next = list_node->next;

        ret = RACE_ERRCODE_FAIL;
        if (list_node->is_used)
        {
            /* Only when is_used is true, is the list_node->data valid. */
            lpcomm_packet = race_lpcomm_packet_header_get((uint8_t *)list_node->data,
                                                          list_node->method);
            if (lpcomm_packet)
            {
                race_lpcomm_packet_type_enum packet_type = lpcomm_packet->packet_type;

#ifdef RACE_LPCOMM_SENDER_ROLE_ENABLE
                packet_type = RACE_LPCOMM_PACKET_GET_PACKET_TYPE(lpcomm_packet->packet_type);
#endif
                if (lpcomm_packet->app_id != app_id && RACE_APP_ID_ALL != app_id)
                {
                    /* Do not free the packet with a different app_id in the retry list. */
                    ret = RACE_ERRCODE_SUCCESS;

                }
                else if (gen_fake_rsp &&
                         ((RACE_LPCOMM_PACKET_TYPE_RACE_CMD_REQ == packet_type) ||
                          (RACE_LPCOMM_PACKET_TYPE_COMMON_REQ == packet_type)))
                {
                    ret = race_lpcomm_retry_fake_rsp(lpcomm_packet, list_node->device_id);
                }
            }
        }

        /* Only when the fake response is generated successfully, can the packet in the retry list be kept. In that case, the noti will be freed
        * by the corresponding response handler and the retry_list_node will be freed by the race_lpcomm_agent_data_recv_hdl(). Also free the
        * packets with list_node->is_used being false and lpcomm_packet being NULL.
        */
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            race_lpcomm_retry_drop_packet(list_node);
        }

        list_node = list_node_next;
    }

    return RACE_ERRCODE_SUCCESS;
}


race_lpcomm_retry_list_struct *race_lpcomm_retry_list_node_find_by_process_id(uint16_t process_id)
{
    race_lpcomm_retry_list_struct *list_node = NULL, *next_list_node = NULL;
    race_lpcomm_packet_struct *lpcomm_packet = NULL;

    if (!g_race_lpcomm_retry_cntx)
    {
        return NULL;
    }

    list_node = g_race_lpcomm_retry_cntx->retry_list;

    while (list_node)
    {
        next_list_node = list_node->next;

        if (list_node->is_used)
        {
        lpcomm_packet = race_lpcomm_packet_header_get((uint8_t *)list_node->data,
                                                      list_node->method);
        if (lpcomm_packet && (process_id == lpcomm_packet->process_id))
        {
            RACE_LOG_MSGID_I("Find the node. process_id:%d, lpcomm_package:%x",2, process_id, list_node->data);
            return list_node;
        }
        }
        else
        {
            RACE_LOG_MSGID_E("not used list_node is in the retry list!", 0);
            race_lpcomm_retry_list_remove(list_node);
            race_lpcomm_retry_list_node_free(list_node);
        }

        list_node = next_list_node;
    }

    RACE_LOG_MSGID_E("Not Found! process_id:%d",1, process_id);
    return NULL;
}


RACE_ERRCODE race_lpcomm_retry_fake_rsp(race_lpcomm_packet_struct *req_packet, uint8_t device_id)
{
    race_lpcomm_fake_rsp_struct *fake_rsp = NULL;
    race_lpcomm_packet_struct *fake_rsp_packet = NULL;
    race_general_msg_t msg_queue_item = {0};
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;

    RACE_LOG_MSGID_I("req_packet:%x device_id:%d",2, req_packet, device_id);

    if (!req_packet)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    fake_rsp_packet = race_mem_alloc(sizeof(race_lpcomm_packet_struct) + \
                                     sizeof(race_lpcomm_fake_rsp_struct) + \
                                     req_packet->payload_len);
    if (!fake_rsp_packet)
    {
        return RACE_ERRCODE_FAIL;
    }

    memcpy(fake_rsp_packet, req_packet, sizeof(race_lpcomm_packet_struct));

#ifndef RACE_LPCOMM_SENDER_ROLE_ENABLE
    if (RACE_LPCOMM_PACKET_TYPE_RACE_CMD_REQ == req_packet->packet_type)
    {
        fake_rsp_packet->packet_type = RACE_LPCOMM_PACKET_TYPE_RACE_CMD_RSP ;
    }
    else
    {
        fake_rsp_packet->packet_type = RACE_LPCOMM_PACKET_TYPE_COMMON_RSP;
    }
#else
    if (RACE_LPCOMM_PACKET_TYPE_RACE_CMD_REQ == RACE_LPCOMM_PACKET_GET_PACKET_TYPE(req_packet->packet_type))
    {
        RACE_LPCOMM_PACKET_SET_PACKET_TYPE(fake_rsp_packet->packet_type, RACE_LPCOMM_PACKET_TYPE_RACE_CMD_RSP);
    }
    else
    {
        RACE_LPCOMM_PACKET_SET_PACKET_TYPE(fake_rsp_packet->packet_type, RACE_LPCOMM_PACKET_TYPE_COMMON_RSP);
    }

    if (RACE_LPCOMM_ROLE_AGENT == RACE_LPCOMM_PACKET_GET_SENDER_ROLE(req_packet->packet_type))
    {
        RACE_LPCOMM_PACKET_SET_SENDER_ROLE(fake_rsp_packet->packet_type, RACE_LPCOMM_ROLE_PARTNER);
    }
    else
    {
        RACE_LPCOMM_PACKET_SET_SENDER_ROLE(fake_rsp_packet->packet_type, RACE_LPCOMM_ROLE_AGENT);
    }
#endif

    fake_rsp_packet->payload_len = sizeof(race_lpcomm_fake_rsp_struct)+ \
                                   req_packet->payload_len;
    fake_rsp = (race_lpcomm_fake_rsp_struct *)fake_rsp_packet->payload;
    fake_rsp->status = RACE_ERRCODE_MAX_RETRY;
    fake_rsp->req_len = req_packet->payload_len;
    if (req_packet->payload_len)
    {
        memcpy(fake_rsp->req, req_packet->payload, req_packet->payload_len);
    }

    msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_LPCOMM_DATA_RECV_IND;
    msg_queue_item.msg_data = (uint8_t *)fake_rsp_packet;
    msg_queue_item.dev_t = device_id;
    ret = race_send_msg(&msg_queue_item);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        race_mem_free(fake_rsp_packet);
    }

    return ret;
}


/* 1. Re-send the packet if timeout and it does not reach max retry-time.
  * 2. Send a fake response for the retry request if it reaches max retry-time.
  * 3. Free retry-list or ps-list resources if neccessary.
  *     (remove from list, free list node, free packet in the node;
  *       remove from list, free list node, free tmp_result in the node)
  *     3.1 Valid retry-list node(no packet, no related ps node, invalid ps node)
  *     3.2 Request reaches max retry-time.
  */
void race_lpcomm_retry_timer_expiration_process(bool *timer_in_use)
{
    race_lpcomm_retry_list_struct *list_node = NULL, *next_list_node = NULL;
    uint32_t curr_time_in_ms = race_get_curr_time_in_ms();
    uint32_t timer_interval = 0;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
#ifdef RACE_LPCOMM_DEBUG_ENABLE
    static int package_loss_count = 0;
#endif

    RACE_LOG_MSGID_I("race_lpcomm_retry_timer_expiration_process", 0);
    if (timer_in_use)
    {
        *timer_in_use = FALSE;
    }

    if (!g_race_lpcomm_retry_cntx)
    {
        return;
    }

    list_node = g_race_lpcomm_retry_cntx->retry_list;

    /* Traverse the whole packages sent list. */
    while (list_node)
    {
        uint8_t *packet = (uint8_t *)list_node->data;
        race_lpcomm_packet_struct *lpcomm_packet = NULL;

        /* list_node may be removed. To traverse the whole list, store the next list node here. */
        next_list_node = list_node->next;

        if (list_node->is_used)
        {
            lpcomm_packet = race_lpcomm_packet_header_get(packet, list_node->method);
        }

        /* Free invalid retry_list node */
        if (!list_node->is_used || !packet || !lpcomm_packet)
        {
            race_lpcomm_retry_drop_packet(list_node);
            list_node = next_list_node;
            continue;
        }

        /* Check if it reaches max retry time. */
        timer_interval = race_get_duration_in_ms(list_node->curr_time_in_ms, curr_time_in_ms);

        if ((RACE_LPCOMM_RETRY_TIMEOUT_IN_MS <= timer_interval) ||
            (RACE_TIMER_TIMEOUT_DELTA_IN_MS >= (RACE_LPCOMM_RETRY_TIMEOUT_IN_MS - timer_interval)))
        {
            if (list_node->max_retry_time > list_node->retry_count)
            {
                /* Send the package again */
                ret = race_lpcomm_packet_send(packet, list_node->method, list_node->device_id);
                if (RACE_ERRCODE_CONNECTION_BROKEN == ret)
                {
                    /* Do nothing and let retry mechamism and ping mechamism detect FOTA failure.
                                       * AWS may recover very soon, so don't be critical to AWS connection. Similar with
                                       * AWS detach event process.
                                       */
                    RACE_LOG_MSGID_W("CONNECTION_BROKEN", 0);
                }

                list_node->retry_count++;
                /* update curr_time_in_ms */
                list_node->curr_time_in_ms = curr_time_in_ms;
            }
            else
            {
                race_lpcomm_packet_type_enum packet_type = lpcomm_packet->packet_type;

#ifdef RACE_LPCOMM_SENDER_ROLE_ENABLE
                packet_type = RACE_LPCOMM_PACKET_GET_PACKET_TYPE(lpcomm_packet->packet_type);
#endif
                /* MAX retry time reaches. */
                RACE_LOG_MSGID_W("max retry time:%d reaches or connection broken ret:%d.",2,
                           list_node->max_retry_time,
                           ret);

                if ((RACE_LPCOMM_PACKET_TYPE_RACE_CMD_REQ == packet_type) ||
                    (RACE_LPCOMM_PACKET_TYPE_COMMON_REQ == packet_type))
                {
                    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

                    /* Gen fake response which contains status of MAX_RETRY and original REQ */
                    ret = race_lpcomm_retry_fake_rsp(lpcomm_packet, list_node->device_id);

#ifdef RACE_FOTA_CMD_ENABLE
                    if (race_fota_is_race_fota_running() && fota_cntx &&
                        RACE_APP_ID_FOTA == lpcomm_packet->app_id)
                    {
                        /* Partner losts for it does not respond. */
                        RACE_LOG_MSGID_I("Partner lost for reaching max retry time or connection broken.", 0);
                        fota_cntx->lpcomm_peer_online = FALSE;
                        race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_AGENT,
                                       RACE_FOTA_STOP_REASON_PARTNER_LOST);
                    }
#endif
                }
                else
                {
                    ret = RACE_ERRCODE_FAIL;
                }

                if (RACE_ERRCODE_SUCCESS != ret)
                {
                    race_lpcomm_retry_drop_packet(list_node);
                }

#ifdef RACE_LPCOMM_DEBUG_ENABLE
                package_loss_count++;
#endif
            }
        }

        list_node = next_list_node;
    }

    if (timer_in_use)
    {
        if (!g_race_lpcomm_retry_cntx->retry_list)
        {
            RACE_LOG_MSGID_I("No package to be sent, timer can be stopped.", 0);

            // race_lpcomm_retry_reset();
        }
        else
        {
            *timer_in_use = TRUE;
        }
    }

#ifdef RACE_LPCOMM_DEBUG_ENABLE
    RACE_LOG_MSGID_I("package_loss_count:%d",1, package_loss_count);
#endif
}
#endif /* RACE_LPCOMM_RETRY_ENABLE */

