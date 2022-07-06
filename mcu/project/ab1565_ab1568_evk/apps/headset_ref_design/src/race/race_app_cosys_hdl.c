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
 * File: race_app_cosys_event_hdl.c
 *
 * Description: This file process the data from COSYS.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#include "race_cmd_feature.h"
#ifdef RACE_COSYS_ENABLE
#include "syslog.h"
#include "race_lpcomm_packet.h"
//#include "race_lpcomm_util.h"
#include "race_cmd_co_sys.h"
#include "race_lpcomm_conn.h"
//#include "race_event.h"
//#include "race_fota_util.h"


log_create_module(race_app_cosys, PRINT_LEVEL_INFO);

#ifndef MTK_DEBUG_LEVEL_NONE
#define RACE_APP_COSYS_LOG_E(fmt,arg...)   LOG_E(race_app_cosys, fmt,##arg)
#define RACE_APP_COSYS_LOG_W(fmt,arg...)   LOG_W(race_app_cosys, fmt,##arg)
#define RACE_APP_COSYS_LOG_I(fmt,arg...)   LOG_I(race_app_cosys, fmt,##arg)
#define RACE_APP_COSYS_LOG_D(fmt,arg...)

#define RACE_APP_COSYS_LOG_MSGID_E(fmt,arg...)   LOG_MSGID_E(race_app_cosys, fmt,##arg)
#define RACE_APP_COSYS_LOG_MSGID_W(fmt,arg...)   LOG_MSGID_W(race_app_cosys, fmt,##arg)
#define RACE_APP_COSYS_LOG_MSGID_I(fmt,arg...)   LOG_MSGID_I(race_app_cosys, fmt,##arg)
#define RACE_APP_COSYS_LOG_MSGID_D(fmt,arg...)
#else
#define RACE_APP_COSYS_LOG_E(fmt,arg...)
#define RACE_APP_COSYS_LOG_W(fmt,arg...)
#define RACE_APP_COSYS_LOG_I(fmt,arg...)

#define RACE_APP_COSYS_LOG_MSGID_E(fmt,arg...)
#define RACE_APP_COSYS_LOG_MSGID_W(fmt,arg...)
#define RACE_APP_COSYS_LOG_MSGID_I(fmt,arg...)
#define RACE_APP_COSYS_LOG_MSGID_D(fmt,arg...)
#endif


static void race_app_cosys_data_callback(bool from_irq, uint8_t *buff, uint32_t len)
{
    race_lpcomm_packet_struct *packet = NULL;
    race_general_msg_t msg_queue_item = {0};
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;

    RACE_APP_COSYS_LOG_MSGID_I("buff:%x len:%d", 2, buff, len);

    if (!buff || !len || len < sizeof(race_lpcomm_packet_struct))
    {
        return;
    }

    /* No need to consider of multiple link. */
    msg_queue_item.dev_t = RACE_LPCOMM_DEFAULT_DEVICE_ID;
    packet = pvPortMalloc(len);
    if (!packet) {
        RACE_APP_COSYS_LOG_MSGID_W("Not enought memory.", 0);
        return;
    }

    memcpy(packet, buff, len);

    msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_LPCOMM_DATA_RECV_IND;
    msg_queue_item.msg_data = (uint8_t *)packet;
    /* Send a message to notify the race task that there are COSYS data received. */
    ret = race_send_msg(&msg_queue_item);
    if (RACE_ERRCODE_SUCCESS != ret) {
        RACE_APP_COSYS_LOG_MSGID_W("Data loss. packet_type:%x process_id:%d, cmd_id:%d, channel_id:%d", 4,
                                 packet->packet_type, packet->process_id, packet->cmd_id, packet->channel_id);
        vPortFree(packet);
    }
}


RACE_ERRCODE race_app_cosys_init(void)
{
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    bool ret_val = race_cosys_register_data_callback(RACE_COSYS_MODULE_ID_LPCOMM, race_app_cosys_data_callback);
    uint8_t device_id = 0;

    if (ret_val)
    {
        ret = race_lpcomm_attach_proc(&device_id,
                                      NULL,
                                      0,
                                      RACE_LPCOMM_ROLE_NONE,
                                      RACE_LPCOMM_TRANS_METHOD_COSYS);
    }

    RACE_APP_COSYS_LOG_MSGID_I("ret:%d, device_id:%d", 2, ret, device_id);

    return ret;
}
#endif /* RACE_COSYS_ENABLE */

