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
#include <string.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "race_xport.h"
#include "race_cmd.h"
#include "race_event.h"
#include "race_cmd_online_log.h"
#include "syslog.h"

#ifdef MTK_MUX_ENABLE

#include "mux.h"
#include "mux_port_common.h"

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

#define CMD_END_FLAG_CLEAR          0x0F
#define CMD_END_FLAG_SET            0xF0

#define CMD_INDEX_RESET_FLAG_SET    0x00
#define CMD_INDEX_RESET_FLAG_CLEAR  0xFF

#define SDK_VERSION_LENGTH 48
#define BUILD_TIME_LENGTH  48
#define HW_VERSION_LENGTH  16
#define AIRLINE_LOG_INTERVAL_TIME  (60)
#define RESPONSE_BUFFER_MAX_LEN    (512)

#define RACE_AIRLOG_UNUSED(a) (void)a

#if !defined(MTK_DEBUG_LEVEL_NONE) && !defined(MTK_DEBUG_LEVEL_PRINTF)

//extern online_log_share_variable_t *g_online_log_share_variable;
extern volatile uint32_t *online_log_flag;
extern volatile uint32_t *g_syslog_send_packet_ok;
extern uint32_t need_send_msg();
extern void online_log_rx_handler_post(uint8_t *pbuf);
extern bool log_set_all_module_filter_off(log_switch_t log_switch);
extern uint8_t* online_log_rx_handler(uint8_t port_index, uint8_t* data, uint32_t length, uint32_t *user_len);

//////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static TimerHandle_t airlog_rtos_timer = NULL;

//////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
extern char build_date_time_str[];
extern char sw_verno_str[];
extern char hw_verno_str[];
extern print_level_t g_cpu_level[2];

extern bool log_set_all_module_filter_off(log_switch_t log_switch);
extern bool log_get_module_filter_number(uint32_t cpu_id, uint32_t *p_module_number);
extern void *qurey_cpu_module_filter_pointer(uint32_t cpu_id);
extern uint8_t *find_start_of_cpu_log_filters(uint32_t cpu_id);


static void airlog_os_layer_rtos_timer_os_expire(TimerHandle_t timer)
{
    mux_ctrl_para_t need_tx_length;

    RACE_AIRLOG_UNUSED(timer);
    need_tx_length.mux_get_tx_avail.ret_size = 0;
    /* get tx buffer avail length */
    mux_control(MUX_AIRAPP_0, MUX_CMD_GET_VIRTUAL_TX_AVAIL_LEN, &need_tx_length);

    if(need_tx_length.mux_get_tx_avail.ret_size > 0) {
        mux_control(MUX_AIRAPP_0, MUX_TX_BUFFER_SEND, NULL);
    }
}

static void airlog_os_layer_init_timer(void)
{
    if(airlog_rtos_timer == NULL) {
        airlog_rtos_timer = xTimerCreate("airlog online timer", AIRLINE_LOG_INTERVAL_TIME, pdTRUE, NULL, airlog_os_layer_rtos_timer_os_expire);
        configASSERT(airlog_rtos_timer != NULL);
        xTimerStart(airlog_rtos_timer, 0);
    } else {
        xTimerReset(airlog_rtos_timer, 0);
    }
}

static uint32_t airlog_os_layer_is_timer_active(void)
{
    configASSERT(airlog_rtos_timer != NULL);

    if (xTimerIsTimerActive(airlog_rtos_timer) != pdFALSE) {
        return 1;
    } else {
        return 0;
    }
}

static void airlog_os_layer_stop_timer(void)
{
    if (airlog_rtos_timer != NULL) {
        if (airlog_os_layer_is_timer_active() == 1) {
            configASSERT(pdFAIL != xTimerStop(airlog_rtos_timer, 0));
        }
    }
}

void* RACE_BUILD_VERSION_HDR(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        uint8_t cpu_id;
        char hw_version[HW_VERSION_LENGTH];
        char sw_version[SDK_VERSION_LENGTH];
        char build_time[BUILD_TIME_LENGTH];
    } PACKED RSP;

    uint32_t copy_len;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                        RACE_TYPE_RESPONSE,
                                        pCmdMsg->id,
                                        sizeof(RSP),
                                        channel_id);

    if (pEvt) {
        pEvt->cpu_id = GET_CURRENT_CPU_ID();
        memset(pEvt->hw_version, 0, HW_VERSION_LENGTH);
        memset(pEvt->sw_version, 0, SDK_VERSION_LENGTH);
        memset(pEvt->build_time, 0, BUILD_TIME_LENGTH);

        copy_len = strlen(hw_verno_str);
        if (copy_len > HW_VERSION_LENGTH) {
            copy_len = HW_VERSION_LENGTH;
        }
        memcpy(pEvt->hw_version, &hw_verno_str[0], copy_len);

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
    } else {
        return NULL;
    }

    return (void*)pEvt;
}

void* RACE_AIRLOG_START_HDR(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t start;
    } PACKED *PTR_THIS_RACE_CMD_STRU;

    typedef struct
    {
        uint8_t Status;
    } PACKED *PTR_THIS_RACE_EVT_STRU;

    PTR_THIS_RACE_CMD_STRU pThisCmd = (PTR_THIS_RACE_CMD_STRU)pCmdMsg;
    if (pThisCmd->start == 0x01) {
        log_set_cpu_filter_config(0, DEBUG_LOG_ON, PRINT_LEVEL_WARNING);
        log_set_cpu_filter_config(1, DEBUG_LOG_ON, PRINT_LEVEL_WARNING);
        mux_control(MUX_AIRAPP_0, MUX_CMD_CONNECT, NULL);
        //start timer
        airlog_os_layer_init_timer();
    } else {
        //stop timer
        airlog_os_layer_stop_timer();
        mux_control(MUX_AIRAPP_0, MUX_CMD_DISCONNECT, NULL);
        log_set_cpu_filter_config(0, DEBUG_LOG_ON, PRINT_LEVEL_INFO);
        log_set_cpu_filter_config(1, DEBUG_LOG_ON, PRINT_LEVEL_INFO);
    }

    PTR_THIS_RACE_EVT_STRU pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                        RACE_TYPE_RESPONSE,
                                        pCmdMsg->id,
                                        1,
                                        channel_id);

    if (pEvt != NULL)
    {
        pEvt->Status = RACE_ERRCODE_SUCCESS;
    }

    return pEvt;
}

/* for ab156x Architecture with race cmd */
void* RACE_MODULE_QUERY_HDR(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t channel_id)
{
	uint32_t res_len, name_len, cpu_id, filter_number = 0;
    volatile uint8_t *p_filters_status;
    static uint8_t curr_module_id = 0;
    log_control_block_t *filters;
    uint8_t *p_data, *p_idenx;

    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t RESET_FLAG;
    } PACKED *PTR_THIS_RACE_CMD_STRU;

    PTR_THIS_RACE_CMD_STRU pThisCmd = (PTR_THIS_RACE_CMD_STRU)pCmdMsg;

    if (pThisCmd->Hdr.length != 3) {// 05 5A 03 00 0A 1E 【00/FF】
        return NULL;
    }

    /* reset flag & clear current id */
    if (pThisCmd->RESET_FLAG == CMD_INDEX_RESET_FLAG_SET) {  //CMD_INDEX_RESET_FLAG_SET
        RACE_LOG_MSGID_I("RACE_MODULE_QUERY_HDR. online mode, set all module filter off!!!", 0);
        log_set_all_module_filter_off(DEBUG_LOG_OFF);
        curr_module_id = 0;
    } else if (pThisCmd->RESET_FLAG != CMD_INDEX_RESET_FLAG_CLEAR) {
        return NULL;
    }

	cpu_id = GET_CURRENT_CPU_ID();

	log_get_module_filter_number(cpu_id, &filter_number);
    if (filter_number == 0) {
        return NULL;
    }

    filters = qurey_cpu_module_filter_pointer(cpu_id);
    p_filters_status = find_start_of_cpu_log_filters(cpu_id);

    p_data = (uint8_t *)malloc(RESPONSE_BUFFER_MAX_LEN);
    if (p_data == NULL) {
        return NULL;
    }
    p_idenx = p_data;

    res_len = 0;
    while (curr_module_id < filter_number) {
        name_len = strlen(filters[curr_module_id].module_name);
        res_len  += name_len + 4; //total data len
        if(res_len > RESPONSE_BUFFER_MAX_LEN) {
            res_len -= (name_len + 4);
            break;
        }
        *p_data++ = cpu_id;             /* cpu id */
        *p_data++ = curr_module_id;     /* module id */
        *p_data++ = name_len;           /* module name length */
        strncpy((char *)p_data, filters[curr_module_id].module_name, name_len); /* module name */
        p_data += name_len;
        *p_data++ = p_filters_status[curr_module_id];   /* module log switch<<4 | module log print level */
        curr_module_id += 1;
    }

    //uint8_t *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE, pCmdMsg->id, res_len + 2 , channel_id);
    uint8_t *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                            RACE_TYPE_RESPONSE,
                                            pCmdMsg->id,
                                            res_len + 2,
                                            channel_id);
    if (pEvt != NULL) {
        p_data = p_idenx;
        if(curr_module_id == filter_number){//END
            pEvt[0] = RACE_ERRCODE_SUCCESS;
            pEvt[1] = CMD_END_FLAG_SET;
            memcpy(&pEvt[2], p_data, res_len);
            curr_module_id = 0;
        } else {//NOT END
            pEvt[0] = RACE_ERRCODE_SUCCESS;
            pEvt[1] = CMD_END_FLAG_CLEAR;
            memcpy(&pEvt[2], p_data, res_len);
        }
    }

    free(p_data);

    return (void*)pEvt;
}

void* RACE_MODULE_SET_HDR(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t channel_id)
{
    uint32_t filter_number;
    volatile uint8_t *p_filters_status;
    log_control_block_t *filters;
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t CPU_ID;
        uint8_t MODULE_ID;
        uint8_t MODULE_INFO;
    } PACKED *PTR_THIS_RACE_CMD_STRU;
    PTR_THIS_RACE_CMD_STRU pThisCmd = (PTR_THIS_RACE_CMD_STRU)pCmdMsg;

    if (pThisCmd->Hdr.length != 5) { //05 5A 05 00 0B 1E 【00 00 00】 cpu id / filter id / filter info
        return NULL;
    }

    if (pThisCmd->CPU_ID >= MTK_MAX_CPU_NUMBER) {
        return NULL;
    }

	log_get_module_filter_number(pThisCmd->CPU_ID, &filter_number);
	if (filter_number == 0) {
		return NULL;
	}

    filters = qurey_cpu_module_filter_pointer(pThisCmd->CPU_ID);
    if (filters == NULL) {
        return NULL;
    }

    if ((pThisCmd->MODULE_ID >= filter_number) ||
            ((pThisCmd->MODULE_INFO >> 4)    > DEBUG_LOG_OFF) ||
            ((pThisCmd->MODULE_INFO & 0x0F)  > PRINT_LEVEL_ERROR)) {
        return NULL;
    }

    p_filters_status = find_start_of_cpu_log_filters(pThisCmd->CPU_ID);
    p_filters_status[pThisCmd->MODULE_ID] = pThisCmd->MODULE_INFO;

    uint8_t *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                            RACE_TYPE_RESPONSE,
                                            pCmdMsg->id,
                                            1,
                                            channel_id);
    if (pEvt != NULL) {
        pEvt[0] = RACE_ERRCODE_SUCCESS;
    }

    return (void*)pEvt;
}

void* RACE_CPU_FILTER_INFO_QUERY_HDR(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t channel_id)
{
    uint32_t cpu_id;
	log_switch_t log_switch;
	print_level_t print_level;
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t cpu_id;
    } PACKED THIS_RACE_CMD_STRU;

    typedef struct
    {
        uint8_t  status;
        uint8_t  cpu_id;
        uint8_t  filter_info;
    } PACKED THIS_RACE_EVT_STRU;

    THIS_RACE_CMD_STRU *pThisCmd = (THIS_RACE_CMD_STRU *)pCmdMsg;

	cpu_id = pThisCmd->cpu_id; //GET_CURRENT_CPU_ID();

    if(cpu_id >= MTK_MAX_CPU_NUMBER) {
		return NULL;
    }

	THIS_RACE_EVT_STRU *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id, RACE_TYPE_RESPONSE, pCmdMsg->id, 3, channel_id);

    if (pEvt != NULL) {
        if (log_get_cpu_filter_config(cpu_id, &log_switch, &print_level) == true) {
            pEvt->status = RACE_ERRCODE_SUCCESS;
            pEvt->cpu_id = cpu_id;
            pEvt->filter_info = (log_switch << 4) | print_level;
        } else {
            pEvt->status = RACE_ERRCODE_FAIL;
        }
    }

    return (void*)pEvt;
}

void* RACE_CPU_FILTER_INFO_SET_HDR(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t channel_id)
{
    uint32_t cpu_id;
	log_switch_t log_switch;
	print_level_t print_level;
	uint8_t filter_info;
	typedef struct
	{
		RACE_COMMON_HDR_STRU Hdr;
		uint8_t cpu_id;
		uint8_t filter_info;
    } PACKED THIS_RACE_DATA_RAM_CMD_HDR_STRU;
	THIS_RACE_DATA_RAM_CMD_HDR_STRU *pCmd = (THIS_RACE_DATA_RAM_CMD_HDR_STRU*)pCmdMsg;

	cpu_id = pCmd->cpu_id; //GET_CURRENT_CPU_ID();

	/* dsp id is 1 */
	if(cpu_id >= MTK_MAX_CPU_NUMBER) {
		return NULL;
	}

	/* alloc response buffer */
	uint8_t *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id, RACE_TYPE_RESPONSE, pCmdMsg->id, 1 , channel_id);

	filter_info = pCmd->filter_info;
	log_switch = (filter_info >> 4) & 0x01;
	print_level = filter_info & 0x03;

    if (pEvt != NULL) {
        if (log_set_cpu_filter_config(cpu_id, log_switch, print_level) == true) {
            pEvt[0] = RACE_ERRCODE_SUCCESS;
        } else {
            pEvt[0] = RACE_ERRCODE_FAIL;
        }
    }

    return (void*)pEvt;
}

void* RACE_SAVE_LOG_FILTER_HDR(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t channel_id)
{
	/* alloc response buffer */
	uint8_t *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id, RACE_TYPE_RESPONSE, pCmdMsg->id, 1 , channel_id);

	/* set save flag */
	log_trigger_save_filter();

	/* check status */
    if (pEvt != NULL) {
        pEvt[0] = RACE_ERRCODE_SUCCESS;
    }

    return (void*)pEvt;
}

void* RACE_CmdHandler_online_log_2(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    RACE_AIRLOG_UNUSED(length);
    void* ptr = NULL;

    switch (pCmdMsg->hdr.id)
    {
        case RACE_ID_BUILD_VERSION_INFO_GET: //0x1E08
        {
            ptr = RACE_BUILD_VERSION_HDR((PTR_RACE_COMMON_HDR_STRU)pCmdMsg, channel_id);
        }
        break;

        case RACE_ID_ONLINE_LOG_OVER_BT_START: //0x1E09
        {
            ptr = RACE_AIRLOG_START_HDR((PTR_RACE_COMMON_HDR_STRU)pCmdMsg, channel_id);
        }
        break;

        case RACE_ID_ONLINE_LOG_MODULE_SETTING_QUERY: //0x1E0A
        {
            ptr = RACE_MODULE_QUERY_HDR((PTR_RACE_COMMON_HDR_STRU)pCmdMsg, channel_id);
        }
        break;

        case RACE_ID_ONLINE_LOG_MODULE_SETTING_SET: //0x1E0B
        {
            ptr = RACE_MODULE_SET_HDR((PTR_RACE_COMMON_HDR_STRU)pCmdMsg, channel_id);
        }
        break;

        case RACE_ID_AIRLOG_CPU_FILTER_INFO_QUERY:{ //0x1E0C
            ptr = RACE_CPU_FILTER_INFO_QUERY_HDR((PTR_RACE_COMMON_HDR_STRU)pCmdMsg, channel_id);
        }
        break;

        case RACE_ID_AIRLOG_CPU_FILTER_INFO_SET:{ //0x1E0D
            ptr = RACE_CPU_FILTER_INFO_SET_HDR((PTR_RACE_COMMON_HDR_STRU)pCmdMsg, channel_id);
        }
        break;

        case RACE_ID_AIRLOG_SAVE_LOG_FILTER:{ //0x1E0E
            ptr = RACE_SAVE_LOG_FILTER_HDR((PTR_RACE_COMMON_HDR_STRU)pCmdMsg, channel_id);
        }
        break;

        case RACE_ID_ONLINE_ASSERT:
        {
            assert(0);
        }
        break;

        default:
            break;
    }

    return ptr;
}

#else /* MTK_DEBUG_LEVEL_NONE */

void* RACE_CmdHandler_online_log_2(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    RACE_AIRLOG_UNUSED(pCmdMsg);
    RACE_AIRLOG_UNUSED(length);
    RACE_AIRLOG_UNUSED(channel_id);
    return NULL;
}

#endif /* !MTK_DEBUG_LEVEL_NONE */
#endif /* MTK_MUX_ENABLE */


