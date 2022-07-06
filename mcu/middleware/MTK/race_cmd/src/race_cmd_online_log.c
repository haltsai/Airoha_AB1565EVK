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

#ifdef RACE_ONLINE_LOG_CMD_ENABLE
#include <string.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "race_cmd.h"
#include "race_event.h"
#include "race_cmd_online_log.h"
#include "race_lpcomm_trans.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_msg_struct.h"
#include "race_lpcomm_conn.h"
#include "race_noti.h"
#include "race_lpcomm_ps_noti.h"
#include "syslog.h"

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

extern char build_date_time_str[];
extern char sw_verno_str[];
extern char hw_verno_str[];

#define SDK_VERSION_LENGTH 48
#define BUILD_TIME_LENGTH  32
#define HW_VERSION_LENGTH  16
#define ONLINE_LOG_INTERVAL_TIME  (60)

//extern online_log_share_variable_t *g_online_log_share_variable;
extern volatile uint32_t *online_log_flag;
extern volatile uint32_t *g_syslog_send_packet_ok;
extern uint32_t need_send_msg();
extern void online_log_rx_handler_post(uint8_t *pbuf);
extern bool log_set_all_module_filter_off(log_switch_t log_switch);
extern uint8_t* online_log_rx_handler(uint8_t port_index, uint8_t* data, uint32_t length, uint32_t *user_len);

static uint32_t spp_connect;
static uint32_t spp_log_flag;
static uint32_t time_delay_ms;

typedef void (*online_Log_os_layer_timer_expired_t)(void);

static TimerHandle_t online_log_rtos_timer = NULL;

static void online_log_os_layer_rtos_timer_os_expire(TimerHandle_t timer)
{
    if (need_send_msg()) {
        race_general_msg_t msg_queue_item;
        msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_SEND_LOG_IND;
        msg_queue_item.dev_t = SERIAL_PORT_DEV_BT_SPP;
        msg_queue_item.msg_data = NULL;
        race_send_msg(&msg_queue_item);
    }
}

static void online_log_os_layer_init_timer(void)
{
    time_delay_ms = ONLINE_LOG_INTERVAL_TIME;
    if(online_log_rtos_timer == NULL) {
        online_log_rtos_timer = xTimerCreate("online log timer", time_delay_ms, pdTRUE, NULL, online_log_os_layer_rtos_timer_os_expire);
        configASSERT(online_log_rtos_timer != NULL);
        xTimerStart(online_log_rtos_timer, 0);
    } else {
        xTimerReset(online_log_rtos_timer, 0);
    }
}

static uint32_t online_log_os_layer_is_timer_active(void)
{
    configASSERT(online_log_rtos_timer != NULL);

    if (xTimerIsTimerActive(online_log_rtos_timer) != pdFALSE) {
        return 1;
    } else {
        return 0;
    }
}

static void online_log_os_layer_stop_timer(void)
{
    configASSERT(online_log_rtos_timer != NULL);
    if (online_log_os_layer_is_timer_active() == 1) {
        configASSERT(pdFAIL != xTimerStop(online_log_rtos_timer, 0));
    }
}

void online_log_os_layer_deinit_timer(void)
{
    if (online_log_rtos_timer != NULL) {
        xTimerDelete(online_log_rtos_timer, 0);
        online_log_rtos_timer = NULL;
    }
}

static RACE_ERRCODE race_online_log_race_event_cb(race_event_type_enum event_type, void *param, void *user_data)
{
    switch (event_type)
    {
        case RACE_EVENT_TYPE_CONN_SPP_CONNECT:
            spp_connect = 1;
            /*  apk will reconnect and need a log start cmd again
            if (spp_log_flag && (*online_log_flag == 0)) {
                *online_log_flag = 1;
            }
            */
            break;
        case RACE_EVENT_TYPE_CONN_SPP_DISCONNECT:
            if (*online_log_flag == 1) {
                online_log_os_layer_stop_timer();
                *online_log_flag = 0;
            }
            break;
        default:
            break;
    }
    return RACE_ERRCODE_SUCCESS;
}

RACE_ERRCODE race_online_log_init(void)
{
#ifdef MTK_RACE_EVENT_ID_ENABLE
    int32_t register_id;

    return race_event_register(&register_id, race_online_log_race_event_cb, NULL);
#else
    return race_event_register(race_online_log_race_event_cb, NULL);
#endif
}


void* race_cmdhdl_online_load_info_get(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct {
        uint8_t status;
        char sw_version[SDK_VERSION_LENGTH];
        char build_time[BUILD_TIME_LENGTH];
        char hw_version[HW_VERSION_LENGTH];
    } PACKED RSP;

    uint32_t min_seq, max_seq;
    uint32_t start_addr, total_length, copy_len;

    (void)min_seq;
    (void)max_seq;
    (void)start_addr;
    (void)total_length;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_ID_BUILD_VERSION_INFO_GET,
                                      sizeof(RSP),
                                      channel_id);
    if (pEvt) {
        pEvt->status = 0;   //0:succuss   1: failed
        memset(pEvt->sw_version, 0, SDK_VERSION_LENGTH);
        memset(pEvt->build_time, 0, BUILD_TIME_LENGTH);
        memset(pEvt->hw_version, 0, HW_VERSION_LENGTH);

        copy_len = strlen(sw_verno_str);
        if (copy_len > SDK_VERSION_LENGTH) {
            copy_len = SDK_VERSION_LENGTH;
        }
        memcpy(pEvt->sw_version, &sw_verno_str[0], copy_len);

        copy_len = strlen(build_date_time_str);
        if (copy_len > BUILD_TIME_LENGTH) {
            copy_len = BUILD_TIME_LENGTH;
        }
        memcpy(pEvt->build_time, &build_date_time_str[0], copy_len);

        copy_len = strlen(hw_verno_str);
        if (copy_len > HW_VERSION_LENGTH) {
            copy_len = HW_VERSION_LENGTH;
        }
        memcpy(pEvt->hw_version, &hw_verno_str[0], copy_len);
    }

    return (void*)pEvt;
}

void* race_cmdhdl_online_reply_status(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t rsp_status, uint8_t channel_id, uint16_t raceid)
{
    typedef struct {
        uint8_t status;
    } PACKED RSP;

    uint32_t start_addr, total_length;

    (void)start_addr;
    (void)total_length;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      raceid,
                                      sizeof(RSP),
                                      channel_id);
    if (pEvt) {
        pEvt->status = rsp_status;   //0:succuss   1: failed
    }

    return (void*)pEvt;
}


void* race_cmdhdl_online_reply_data(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t channel_id, uint16_t length, uint16_t race_id, uint8_t *data)
{
/*
    typedef struct {
       uint8_t status;
       uint8_t *data;
    }PACKED DATA_RSP;
*/
    uint8_t *pdata;

    pdata = (uint8_t *)RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      race_id,
                                      length+1,
                                      channel_id);
    if(pdata == NULL){
        return NULL;
    }
    pdata[0] = 0;   //status

    memcpy(&pdata[1], data, length);
    online_log_rx_handler_post(data);
    return (void*)pdata;
}

void* RACE_CmdHandler_online_log(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    static uint32_t g_race_event_registe = 0;
    //RACE_LOG_MSGID_I("RACE_CmdHandler_online_log, pCmdMsg->hdr.id[0x%X]", 1, (int)pCmdMsg->hdr.id);

    switch (pCmdMsg->hdr.id)
    {
        case RACE_ID_BUILD_VERSION_INFO_GET:
            return race_cmdhdl_online_load_info_get((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);
            break;
        case RACE_ID_ONLINE_LOG_OVER_BT_START:
        if (g_race_event_registe == 0) {
            RACE_ERRCODE status;
            status = race_online_log_init();
            if (RACE_ERRCODE_SUCCESS == status) {
                g_race_event_registe = 1;
            }
        }

        if(pCmdMsg->payload[0] == 1) {
            //APK log off
            *online_log_flag = 1;
            *g_syslog_send_packet_ok = 1;
            spp_log_flag = 1;
            //start timer
            log_set_cpu_filter_config(0,DEBUG_LOG_ON,PRINT_LEVEL_WARNING);
            log_set_cpu_filter_config(1,DEBUG_LOG_ON,PRINT_LEVEL_WARNING);
            online_log_os_layer_init_timer();

        } else if(pCmdMsg->payload[0] == 0) {
            //APK log on
            *online_log_flag = 0;  //log off
            spp_log_flag = 0;
            //stop timer
            online_log_os_layer_stop_timer();
            log_set_cpu_filter_config(0,DEBUG_LOG_ON,PRINT_LEVEL_INFO);
            log_set_cpu_filter_config(1,DEBUG_LOG_ON,PRINT_LEVEL_INFO);
        } else {
            //invalid data
            spp_log_flag = 0;
        }
            return race_cmdhdl_online_reply_status((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), 0, channel_id, pCmdMsg->hdr.id);
        break;
        case RACE_ID_ONLINE_LOG_MODULE_SETTING_QUERY:
            {
                uint8_t *userdata;
                uint32_t len, cmd_len;

                cmd_len = length - 2;
                log_set_all_module_filter_off(DEBUG_LOG_OFF);
                userdata = online_log_rx_handler(0, &(pCmdMsg->payload[0]), cmd_len, &len);
                if ( userdata != NULL) {
                    return race_cmdhdl_online_reply_data((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), channel_id, len, pCmdMsg->hdr.id, userdata);
                } else {
                    return race_cmdhdl_online_reply_status((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), 1, channel_id, pCmdMsg->hdr.id);
                }
            }
            break;

        case RACE_ID_ONLINE_LOG_MODULE_SETTING_SET:
            {
                uint8_t *userdata;
                uint32_t len, cmd_len;

                cmd_len = length - 2;
                userdata = online_log_rx_handler(0, &(pCmdMsg->payload[0]), cmd_len, &len);

                if ( userdata != NULL) {
                    return race_cmdhdl_online_reply_data((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), channel_id, len, pCmdMsg->hdr.id, userdata);
                } else {
                    return race_cmdhdl_online_reply_status((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), 1, channel_id, pCmdMsg->hdr.id);
                }
            }
            break;

        case RACE_ID_ONLINE_ASSERT:
            //should be enable mini dump
            //#ifdef MTK_MINIDUMP_ENABLE
            assert(0);
            //#endif
            break;

        default:
            break;
    }

    return NULL;
}

#endif /* RACE_ONLINE_LOG_CMD_ENABLE */

