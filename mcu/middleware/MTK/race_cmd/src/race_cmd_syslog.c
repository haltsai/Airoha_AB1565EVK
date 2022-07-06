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
#include "race_cmd.h"
#include "race_event.h"
#include "race_cmd_syslog.h"

#define RACE_SYSLOG_UNUSED(a) (void)a

#if !defined(MTK_DEBUG_PLAIN_LOG_ENABLE) && defined(MTK_MUX_ENABLE) && !defined(MTK_DEBUG_LEVEL_PRINTF)
#include "hal_resource_assignment.h"
#include "memory_attribute.h"
#include "mux_port_common.h"
#include "syslog.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define HW_VERSION_LENGTH  16
#define SDK_VERSION_LENGTH 48
#define BUILD_TIME_LENGTH  48

#define COMMAND_ERROR_CODE_OK        0x00
#define COMMAND_ERROR_CODE_FAIL      0x01


#define PORT_SYSLOG_SDK_VERSION_BEGIN HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_VERSION_VAR_START
#define PORT_SYSLOG_BUILD_TIME_BEGIN  HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_BUILD_TIME_VAR_START

////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern bool log_get_module_filter_number(uint32_t cpu_id, uint32_t *p_module_number);
extern bool log_get_module_filter_config(uint32_t cpu_id, uint32_t module_id, const char **p_module_name, log_switch_t *p_log_switch, print_level_t *p_print_level);
extern void log_trigger_save_filter(void);
extern void log_save_filter(void);
extern bool log_query_save_filter(void);
extern void *qurey_cpu_module_filter_pointer(uint32_t cpu_id);

static void* RACE_QUERY_VERSION_BUILDTIME_HDR(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t channel_id)
{
    uint8_t cpu_id;
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
		uint8_t cpu_id;
    } PACKED *PTR_THIS_RACE_CMD_STRU;

    typedef struct {
        uint8_t cpu_id;
        char hw_version[HW_VERSION_LENGTH];
        char sw_version[SDK_VERSION_LENGTH];
        char build_time[BUILD_TIME_LENGTH];
    } PACKED THIS_RACE_EVT_STRU, *PTR_THIS_RACE_EVT_STRU;

	PTR_THIS_RACE_CMD_STRU pCmd = (PTR_THIS_RACE_CMD_STRU)pCmdMsg;

    cpu_id = pCmd->cpu_id;

	if(cpu_id >= MTK_MAX_CPU_NUMBER) {
		return NULL;
	}

	PTR_THIS_RACE_EVT_STRU pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                                        RACE_TYPE_NOTIFICATION,
                                                        pCmdMsg->id,
                                                        sizeof(THIS_RACE_EVT_STRU),
                                                        channel_id);

    if (pEvt != NULL) {
        pEvt->cpu_id = cpu_id;
		memset(pEvt->hw_version, 0, HW_VERSION_LENGTH);
        memset(pEvt->sw_version, 0, SDK_VERSION_LENGTH);
        memset(pEvt->build_time, 0, BUILD_TIME_LENGTH);
		// strncpy(pEvt->hw_version, &hw_verno_str[0], HW_VERSION_LENGTH);
        strncpy(pEvt->sw_version, (char *)(PORT_SYSLOG_SDK_VERSION_BEGIN + cpu_id * SDK_VERSION_LENGTH), SDK_VERSION_LENGTH);
        strncpy(pEvt->build_time, (char *)(PORT_SYSLOG_BUILD_TIME_BEGIN + cpu_id * BUILD_TIME_LENGTH), BUILD_TIME_LENGTH);
    }

    return (void*)pEvt;
}

static void* RACE_QUERY_LOG_FILTER_INFO_HDR(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t channel_id)
{
    uint8_t cpu_id;
	uint32_t i, res_len, filter_number;
    log_switch_t  log_switch;
    print_level_t print_level;

    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
		uint8_t cpu_id;
    } PACKED *PTR_THIS_RACE_CMD_STRU;

	PTR_THIS_RACE_CMD_STRU pCmd = (PTR_THIS_RACE_CMD_STRU)pCmdMsg;

    cpu_id = pCmd->cpu_id;

	if(cpu_id >= MTK_MAX_CPU_NUMBER) {
		return NULL;
	}

	log_get_module_filter_number(cpu_id, &filter_number);
	if (filter_number == 0) {
		return NULL;
	}

	res_len = 1 + filter_number / 2 + filter_number % 2;
	uint8_t *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                        RACE_TYPE_NOTIFICATION,
                                        pCmdMsg->id,
                                        res_len,
                                        channel_id);

    if (pEvt) {
        pEvt[0] = cpu_id; //cpu id
		for (i = 0; i < filter_number / 2; i++) {
			log_get_module_filter_config(cpu_id, i * 2, NULL, &log_switch, &print_level);
			pEvt[i+1] = (log_switch << 3) | print_level;
			log_get_module_filter_config(cpu_id, i * 2 + 1, NULL, &log_switch, &print_level);
			pEvt[i+1] |= ((log_switch << 3) | print_level) << 4;
		}
		if (filter_number % 2) {
			log_get_module_filter_config(cpu_id, i * 2, NULL, &log_switch, &print_level);
			pEvt[i+1] = (log_switch << 3) | print_level;
		}
    }

    return (void*)pEvt;
}

static void* RACE_SET_LOG_FILTER_INFO_HDR(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t channel_id)
{
    uint8_t cpu_id;
	uint32_t i, filter_number;
    log_switch_t  log_switch;
    print_level_t print_level;
	uint8_t  filter_info;
    log_control_block_t *filters;

    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
		uint8_t cpu_id;
		uint8_t Data[0];
    } PACKED *PTR_THIS_RACE_CMD_STRU;

	PTR_THIS_RACE_CMD_STRU pCmd = (PTR_THIS_RACE_CMD_STRU)pCmdMsg;

    cpu_id = pCmd->cpu_id;

	if(cpu_id >= MTK_MAX_CPU_NUMBER) {
		return NULL;
	}

	uint8_t *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                        RACE_TYPE_NOTIFICATION,
                                        pCmdMsg->id,
                                        1,
                                        channel_id);

	/* response pointer */
	if(pEvt != NULL) {
        log_get_module_filter_number(cpu_id, &filter_number);
        if (filter_number == 0) {
            pEvt[0] = COMMAND_ERROR_CODE_FAIL;
            return (void*)pEvt;
        }
        filters = qurey_cpu_module_filter_pointer(cpu_id);
        for (i = 0; i < filter_number/2 ; i++) {
            filter_info = pCmd->Data[i];
            log_switch = (filter_info >> 3) & 0x01;
            print_level = filter_info & 0x03;
            log_set_module_filter_config(cpu_id, (char *)(filters[i * 2].module_name), log_switch, print_level);
            log_switch = (filter_info >> 7) & 0x01;
            print_level = (filter_info >> 4) & 0x03;
            log_set_module_filter_config(cpu_id, (char *)(filters[i * 2 + 1].module_name), log_switch, print_level);
        }
        if (filter_number % 2) {
            filter_info = pCmd->Data[filter_number/2];
            log_switch = (filter_info >> 3) & 0x01;
            print_level = filter_info & 0x03;
            log_set_module_filter_config(cpu_id, (char *)(filters[i * 2].module_name), log_switch, print_level);
        }

        /* response */
        pEvt[0] = COMMAND_ERROR_CODE_OK;
    }

    return (void*)pEvt;
}

static void* RACE_SAVE_LOG_FILTER_HDR(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t channel_id)
{
	/* alloc response buffer */
	uint8_t *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                        RACE_TYPE_NOTIFICATION,
                                        pCmdMsg->id,
                                        1,
                                        channel_id);

    if (pEvt != NULL) {
        /* set save flag */
        log_trigger_save_filter();
        /* check status */
        pEvt[0] = COMMAND_ERROR_CODE_OK;
    }

    return (void*)pEvt;
}

static void* RACE_QUERY_CPU_FILTER_INFO_HDR(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t channel_id)
{
	uint8_t cpu_id;
	log_switch_t log_switch;
	print_level_t print_level;
	typedef struct
	{
		RACE_COMMON_HDR_STRU Hdr;
		uint8_t cpu_id;
	} PACKED *PTR_THIS_RACE_CMD_STRU;

    typedef struct
    {
        uint8_t  status;
        uint8_t  cpu_id;
        uint8_t  filter_info;
    } PACKED *PTR_THIS_RACE_EVT_STRU;

	PTR_THIS_RACE_CMD_STRU pCmd = (PTR_THIS_RACE_CMD_STRU)pCmdMsg;

    cpu_id = pCmd->cpu_id;

	if(cpu_id >= MTK_MAX_CPU_NUMBER) {
		return NULL;
	}

	PTR_THIS_RACE_EVT_STRU pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                                            RACE_TYPE_NOTIFICATION,
                                                            pCmdMsg->id,
                                                            3,
                                                            channel_id);

    if (pEvt != NULL) {
        if (log_get_cpu_filter_config(cpu_id, &log_switch, &print_level) == true) {
            pEvt->status = COMMAND_ERROR_CODE_OK;
            pEvt->cpu_id = cpu_id;
            pEvt->filter_info = (log_switch << 3) | print_level;
        } else {
            pEvt->status = COMMAND_ERROR_CODE_FAIL;
        }
    }

    return (void*)pEvt;
}

static void* RACE_SET_CPU_FILTER_INFO_HDR(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint8_t channel_id)
{
	uint8_t cpu_id;
	log_switch_t log_switch;
	print_level_t print_level;
	uint8_t filter_info;
	typedef struct
	{
		RACE_COMMON_HDR_STRU Hdr;
		uint8_t cpu_id;
		uint8_t filter_info;
    } PACKED *PTR_THIS_RACE_CMD_STRU;

	PTR_THIS_RACE_CMD_STRU pCmd = (PTR_THIS_RACE_CMD_STRU)pCmdMsg;

    cpu_id = pCmd->cpu_id;

	if(cpu_id >= MTK_MAX_CPU_NUMBER) {
		return NULL;
	}

	/* alloc response buffer */
	uint8_t *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                            RACE_TYPE_NOTIFICATION,
                                            pCmdMsg->id,
                                            1,
                                            channel_id);

    if (pEvt != NULL) {
        filter_info = pCmd->filter_info;
        log_switch = (filter_info >> 3) & 0x01;
        print_level = filter_info & 0x03;

        if (log_set_cpu_filter_config(cpu_id, log_switch, print_level) == true) {
            pEvt[0] = COMMAND_ERROR_CODE_OK;
        } else {
            pEvt[0] = COMMAND_ERROR_CODE_FAIL;
        }
    }

    return (void*)pEvt;
}

void* RACE_CmdHandler_syslog(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
	RACE_SYSLOG_UNUSED(length);
    void* ptr = NULL;

    switch (pCmdMsg->hdr.id)
    {
        case RACE_SYSLOG_ACTIVE_ASSERT: { //0x0F14
            // platform_assert("Asserted by PC logging tool", __FILE__, __LINE__);
            extern void light_assert(const char *expr, const char *file, int line);
            ATTR_LOG_STRING exp[] = "Asserted by logging RACE command"; \
            ATTR_LOG_STRING file[] = __FILE__; \
            light_assert(exp, file, __LINE__);
            ptr = NULL;
		} break;

		case RACE_SYSLOG_QUERY_VERSION_BUILDTIME: { //0x0F15
			ptr = RACE_QUERY_VERSION_BUILDTIME_HDR((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), channel_id);
		} break;

		case RACE_SYSLOG_QUERY_LOG_FILTER_INFO: { //0x0F16
			ptr = RACE_QUERY_LOG_FILTER_INFO_HDR((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), channel_id);
		} break;

		case RACE_SYSLOG_SET_LOG_FILTER_INFO: { //0x0F17
			ptr = RACE_SET_LOG_FILTER_INFO_HDR((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), channel_id);
		} break;

		case RACE_SYSLOG_SAVE_LOG_SETTING: { //0x0F18
			ptr = RACE_SAVE_LOG_FILTER_HDR((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), channel_id);
		} break;

		case RACE_SYSLOG_QUERY_CPU_FILTER_INFO: { //0x0F19
			ptr = RACE_QUERY_CPU_FILTER_INFO_HDR((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), channel_id);
		} break;

		case RACE_SYSLOG_SET_CPU_FILTER_INFO: { //0x0F20
			ptr = RACE_SET_CPU_FILTER_INFO_HDR((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), channel_id);
		} break;

		default:
            break;
    }

    return ptr;
}
#else
void* RACE_CmdHandler_syslog(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
	RACE_SYSLOG_UNUSED(pCmdMsg);
	RACE_SYSLOG_UNUSED(length);
	RACE_SYSLOG_UNUSED(channel_id);
    return NULL;
}

#endif /* MTK_DEBUG_PLAIN_LOG_ENABLE */
