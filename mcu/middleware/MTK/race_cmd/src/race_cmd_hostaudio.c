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
#ifdef RACE_HOSTAUDIO_CMD_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "hal.h"
#if defined(HAL_AUDIO_MODULE_ENABLED)
#include "memory_attribute.h"
#include "bt_sink_srv_ami.h"
#endif
#include "race_cmd.h"
#include "race_cmd_hostaudio.h"
#include "race_xport.h"
#include "race_noti.h"
#include "race_cmd_dsprealtime.h"
#include "bt_connection_manager_internal.h"
#include "bt_sink_srv_music.h"

#ifdef AIR_MCSYNC_SHARE_ENABLE
#include "race_cmd_share_mode.h"
#endif

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
#include "race_cmd_co_sys.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MTK_PROMPT_SOUND_ENABLE
extern bool app_voice_prompt_setLang(uint8_t lang_index, bool need_sync);
extern uint8_t app_voice_prompt_getLang();
extern uint16_t app_voice_prompt_getLangCount();
extern bool app_voice_prompt_getSupportLang(uint16_t *buffer);

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_app_voice_prompt_setLang = _default_app_voice_prompt_setLang")
#pragma comment(linker, "/alternatename:_app_voice_prompt_getLang = _default_app_voice_prompt_getLang")
#pragma comment(linker, "/alternatename:_app_voice_prompt_getLangCount = _default_app_voice_prompt_getLangCount")
#pragma comment(linker, "/alternatename:_app_voice_prompt_getSupportLang = _default_app_voice_prompt_getSupportLang")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak app_voice_prompt_setLang = default_app_voice_prompt_setLang
#pragma weak app_voice_prompt_getLang = default_app_voice_prompt_getLang
#pragma weak app_voice_prompt_getLangCount = default_app_voice_prompt_getLangCount
#pragma weak app_voice_prompt_getSupportLang = default_app_voice_prompt_getSupportLang
#else
#error "Unsupported Platform"
#endif
#endif/* MTK_PROMPT_SOUND_ENABLE */

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
extern uint8_t g_mmi_anc_race_cosys_ch_id;
#endif

#ifndef UNUSED
#define UNUSED(x)  ((void)(x))
#endif

//////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifdef MTK_PROMPT_SOUND_ENABLE
bool default_app_voice_prompt_setLang(uint8_t lang_index, bool need_sync)
{
    return false;
}
uint8_t default_app_voice_prompt_getLang()
{
    return 0xFF;
}
uint16_t default_app_voice_prompt_getLangCount()
{
    return 0;
}
bool default_app_voice_prompt_getSupportLang(uint16_t *buffer)
{
    return false;
}
#endif

void race_mmi_set_peq_group_id(uint8_t peq_group_id, uint8_t *status, am_feature_type_t audio_path_id)
{
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
    RACE_LOG_MSGID_I("race_mmi_set_peq_group_id peq_groud_id:%d\n",1, peq_group_id);

    if (status) {
        uint32_t ret;
        bt_clock_t target_bt_clk = {0};
        uint8_t setting_mode = PEQ_DIRECT;
        bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
#ifdef MTK_AWS_MCE_ENABLE
        race_dsprt_peq_get_target_bt_clk(role, &setting_mode, &target_bt_clk);
#endif
        if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
            uint8_t enable = 1;
            uint8_t sound_mode = peq_group_id;
            if (peq_group_id == 0) {
                enable = 0;
                sound_mode = PEQ_SOUND_MODE_UNASSIGNED;
            }
            ret = race_dsprt_peq_change_mode_data(0, setting_mode, target_bt_clk.nclk, enable, sound_mode, audio_path_id);
        } else {
            ret = 1;
            RACE_LOG_MSGID_W("race_mmi_set_peq_group_id role: 0x%x error\n",1,role);
        }

        if (ret == 0) {
            *status = (uint8_t)RACE_ERRCODE_SUCCESS;
        } else {
            *status = (uint8_t)RACE_ERRCODE_FAIL;
        }
    }
#else
    UNUSED(peq_group_id);
    if (status) {
        *status = (uint8_t)RACE_ERRCODE_FAIL;
    }
#endif
}

static void race_mmi_get_peq_group_id(uint8_t *peq_group_id, uint8_t *status, bt_sink_srv_am_type_t type)
{
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
    uint8_t peq_info[4];
    if (peq_group_id && status) {
        if (aud_peq_get_sound_mode(type, (uint8_t *)peq_info) == 0) {
            *peq_group_id = (peq_info[0] == 0) ? 0 : peq_info[1];
            *status = RACE_ERRCODE_SUCCESS;
        } else {
            *peq_group_id = 0;
            *status = RACE_ERRCODE_FAIL;
        }
    }
#else
    UNUSED(peq_group_id);
    if (status) {
        *status = (uint8_t)RACE_ERRCODE_FAIL;
    }
#endif
}

void race_mmi_get_anc_status(uint8_t *anc_status, uint8_t *status)
{
#ifdef MTK_ANC_ENABLE
#ifndef MTK_ANC_V2
    uint8_t anc_enable;
    uint32_t runtime_info;
    int16_t *anc_runtime_gain;
    if (anc_status && status) {
        anc_get_status(&anc_enable, &runtime_info, NULL);
        if (anc_enable > 0) {
            *anc_status = (uint8_t)(runtime_info & ANC_FILTER_TYPE_MASK);
        } else {
            *anc_status = 0;
        }
        anc_runtime_gain = (int16_t *)(anc_status + 1);
        *anc_runtime_gain = anc_get_runtime_gain(1);
        *status = RACE_ERRCODE_SUCCESS;
    }
#else
    uint8_t anc_enable;
    audio_anc_control_filter_id_t anc_current_filter_id;
    int16_t *anc_runtime_gain;
    int16_t anc_cur_runtime_gain;
    if (anc_status && status) {
        audio_anc_control_get_status(&anc_enable, &anc_current_filter_id, NULL, NULL, NULL, NULL);
        audio_anc_control_get_runtime_gain(&anc_cur_runtime_gain, AUDIO_ANC_CONTROL_TYPE_HYBRID);
        if (anc_enable > 0) {
            *anc_status = anc_current_filter_id;
        } else {
            *anc_status = 0;
        }
        anc_runtime_gain  = (int16_t *)(anc_status + 1);
        *anc_runtime_gain = anc_cur_runtime_gain;
        *status = RACE_ERRCODE_SUCCESS;
    }
#endif
#else
    UNUSED(anc_status);
    if (status) {
        *status = (uint8_t)RACE_ERRCODE_FAIL;
    }
#endif
}

static void race_mmi_get_vp_group_id(uint8_t *peq_group_id, uint8_t *status)
{
#ifdef MTK_PROMPT_SOUND_ENABLE
    if (peq_group_id && status) {
        *peq_group_id = app_voice_prompt_getLang();
        if(*peq_group_id != 0xFF){
            *status = (uint8_t)RACE_ERRCODE_SUCCESS;
        }else{
            *status = (uint8_t)RACE_ERRCODE_FAIL;
        }
    }
#else
    UNUSED(peq_group_id);
    if (status) {
        *status = (uint8_t)RACE_ERRCODE_FAIL;
    }
#endif
}

static void race_mmi_set_vp_group_id(uint8_t peq_group_id, uint8_t *status)
{
#ifdef MTK_PROMPT_SOUND_ENABLE
    if (status) {
        if(app_voice_prompt_setLang(peq_group_id, true)){
            *status = (uint8_t)RACE_ERRCODE_SUCCESS;
        }else{
            *status = (uint8_t)RACE_ERRCODE_FAIL;
        }
    }
#else
    UNUSED(peq_group_id);
    if (status) {
        *status = (uint8_t)RACE_ERRCODE_FAIL;
    }
#endif
}

static void race_mmi_get_vp_lang_group_id(uint8_t *peq_group_id, uint8_t *status)
{
#ifdef MTK_PROMPT_SOUND_ENABLE
    uint16_t lang_cnt = app_voice_prompt_getLangCount();

    if (peq_group_id && status) {
        uint16_t *pBuf = NULL;

        if(lang_cnt != 0){
            pBuf = race_mem_alloc(lang_cnt * sizeof(uint16_t));
            app_voice_prompt_getSupportLang(pBuf);

            *peq_group_id = lang_cnt;
            if(pBuf){
                memcpy(peq_group_id+1, pBuf, sizeof(uint16_t)*lang_cnt);
            }else{
                memset(peq_group_id+1, 0, sizeof(uint16_t)*lang_cnt);
            }
            race_mem_free(pBuf);
            *status = (uint8_t)RACE_ERRCODE_SUCCESS;
        }else{
            *peq_group_id = 0;
            *status = (uint8_t)RACE_ERRCODE_FAIL;
        }

    }
#else
    UNUSED(peq_group_id);
    if (status) {
        *status = (uint8_t)RACE_ERRCODE_FAIL;
    }
#endif
}

void race_mmi_get_passthru_gain(uint8_t *gain, uint8_t *status)
{
#ifdef MTK_ANC_ENABLE
#ifndef MTK_ANC_V2
    int16_t *runtime_gain = (int16_t *)gain;
    if (runtime_gain && status) {
        *runtime_gain = anc_get_runtime_gain(0);
        *status = RACE_ERRCODE_SUCCESS;
    }
#else
    int16_t *runtime_gain = (int16_t *)gain;
    if (runtime_gain && status) {
        audio_anc_control_get_runtime_gain(runtime_gain, AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF);
        *status = RACE_ERRCODE_SUCCESS;
    }
#endif
#else
    UNUSED(gain);
    if (status) {
        *status = (uint8_t)RACE_ERRCODE_FAIL;
    }
#endif
}

static void race_mmi_get_game_mode(uint8_t *game_mode, uint8_t *status)
{
    uint32_t count = 0;
    bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
    count = bt_sink_srv_cm_get_connected_device((~BT_SINK_SRV_PROFILE_AWS), addr_list);
    RACE_LOG_MSGID_I("[race_mmi_get_game_mode] connected_dev_num:%d \n", 1, count);

    if (game_mode && status) {
        if (count != 0) {
            *game_mode = bt_sink_srv_music_get_mode((bt_bd_addr_t *)addr_list[0]);
            RACE_LOG_MSGID_I("[race_mmi_get_game_mode] get mode:%d \n", 1, *game_mode);
        }
        if(*game_mode != BT_SINK_SRV_MUSIC_DATA_NOT_FOUND){
            *status = (uint8_t)RACE_ERRCODE_SUCCESS;
        }else{
            *status = (uint8_t)RACE_ERRCODE_FAIL;
        }
    }

}

/**
 * Add for line-in feature
 */
#ifdef APPS_LINE_IN_SUPPORT

line_in_app_callback_t line_in_callback = {0};
uint8_t line_in_app_comm_channel_id = 0xFF;

void race_cmd_hostaudio_set_app_line_in_callback(line_in_app_callback_t *callback)
{
    if (callback != NULL) {
        line_in_callback.pull_request = callback->pull_request;
        line_in_callback.push_response = callback->push_response;
        line_in_callback.control_request = callback->control_request;
    }
}

void race_cmd_hostaudio_notify_app_audio_path_change(uint8_t new_audio_path)
{
    if (new_audio_path != AUDIO_PATH_LINE_IN && new_audio_path != AUDIO_PATH_BT) {
        RACE_LOG_MSGID_E("[race_cmd_hostaudio_notify_app_audio_path_change] unknown path to notify %d", 1, new_audio_path);
        return;
    }
    if (line_in_app_comm_channel_id == 0xFF) {
        RACE_LOG_MSGID_E("[race_cmd_hostaudio_notify_app_audio_path_change] unknown channel ID to notify %d", 1, line_in_app_comm_channel_id);
        return;
    }

    typedef struct
    {
        race_mmi_module_t module;
        uint8_t audio_path;
    }PACKED RACE_MMI_SET_ENUM_EVT_STRU;

    RACE_MMI_SET_ENUM_EVT_STRU *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_COMMAND,
                                                        (uint16_t)RACE_MMI_SET_ENUM,
                                                        (uint16_t)sizeof(RACE_MMI_SET_ENUM_EVT_STRU),
                                                        line_in_app_comm_channel_id);

    if (pEvt == NULL) {
        RACE_LOG_MSGID_E("[race_cmd_hostaudio_notify_app_audio_path_change] allocate memory failed for send event %d", 0);
        return;
    }
    pEvt->module = RACE_MMI_MODULE_AUDIO_PATH;
    pEvt->audio_path = new_audio_path;

    uint32_t port = race_get_port_handle_by_channel_id(line_in_app_comm_channel_id);

    race_send_pkt_t *send_ptr = (void*)race_pointer_cnv_pkt_to_send_pkt(pEvt);

    race_port_send_data(port, (uint8_t*)&send_ptr->race_data, send_ptr->length);

    race_mem_free(send_ptr);
}

static void race_mmi_set_new_audio_path(uint8_t new_path, uint8_t *status)
{
    if (new_path != AUDIO_PATH_LINE_IN && new_path != AUDIO_PATH_BT) {
        *status = RACE_ERRCODE_FAIL;
        RACE_LOG_MSGID_E("[race_mmi_set_new_audio_path] unknown path to set %d", 1, new_path);
        return;
    }
    if (status == NULL) {
        RACE_LOG_MSGID_E("[race_mmi_set_new_audio_path] status is NULL", 0);
        return;
    }

    if (line_in_callback.control_request == NULL) {
        *status = RACE_ERRCODE_FAIL;
        RACE_LOG_MSGID_E("[race_mmi_set_new_audio_path] the control_request callback is NULL", 0);
        return;
    }

    line_in_callback.control_request(new_path, status);
}

static void race_mmi_get_audio_path(uint8_t *current_path, uint8_t *status)
{
    if (current_path == NULL || status == NULL) {
        RACE_LOG_MSGID_E("[race_mmi_get_audio_path] the current is NULL", 0);
        return;
    }
    if (line_in_callback.pull_request == NULL) {
        RACE_LOG_MSGID_E("[race_mmi_get_audio_path] the pull_request callback is NULL", 0);
        *status = RACE_ERRCODE_FAIL;
        return;
    }
    line_in_callback.pull_request(current_path);
    *status = RACE_ERRCODE_SUCCESS;
}

static void race_mmi_audio_path_push_response(uint8_t result)
{
    if (line_in_callback.push_response == NULL) {
        RACE_LOG_MSGID_E("[race_mmi_get_audio_path] the push_response callback is NULL", 0);
        return;
    }
    line_in_callback.push_response(result);
}

#endif /* APPS_LINE_IN_SUPPORT */


#ifdef AIR_MCSYNC_SHARE_ENABLE
#define SM_TAG "[RACE share mode] "

static race_share_mode_config s_share_mode_config = {NULL, NULL};
static uint8_t share_mode_channel_id = 0xff;

void race_share_mode_register(race_share_mode_config *config)
{
    s_share_mode_config.set_callback = config->set_callback;
    s_share_mode_config.get_callback = config->get_callback;
}


void race_share_mode_notify_state_change(race_share_mode_state_type new_sta) {
    typedef struct
    {
        race_mmi_module_t module;
        uint8_t sta;
        uint8_t new_state;
    }PACKED RACE_MMI_SET_ENUM_EVT_STRU;

    RACE_MMI_SET_ENUM_EVT_STRU *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_COMMAND,
                                                        (uint16_t)RACE_MMI_GET_ENUM,
                                                        (uint16_t)sizeof(RACE_MMI_SET_ENUM_EVT_STRU),
                                                        share_mode_channel_id);

    if (pEvt == NULL) {
        RACE_LOG_MSGID_E(SM_TAG"allocate memory failed for send event", 0);
        return;
    }
    pEvt->module = RACE_MMI_MODULE_SHARE_MODE;
    pEvt->sta = RACE_ERRCODE_SUCCESS;
    pEvt->new_state = new_sta;

    uint32_t port = race_get_port_handle_by_channel_id(share_mode_channel_id);
    race_send_pkt_t *send_ptr = (void*)race_pointer_cnv_pkt_to_send_pkt(pEvt);
    race_port_send_data(port, (uint8_t*)&send_ptr->race_data, send_ptr->length);
    race_mem_free(send_ptr);
}


static void race_mmi_set_share_mode(uint8_t enable, uint8_t *status) {
    if (s_share_mode_config.set_callback != NULL) {
        *status = s_share_mode_config.set_callback(enable);
    } else {
        *status = RACE_ERRCODE_FAIL;
    }
}


static void race_mmi_get_share_mode_sta(uint8_t *share_mode_sta, uint8_t *status) {
    if (s_share_mode_config.get_callback != NULL) {
        *status = RACE_ERRCODE_SUCCESS;
        *share_mode_sta = s_share_mode_config.get_callback();
    } else {
        *status = RACE_ERRCODE_FAIL;
    }
}
#endif

/**
 * RACE_MMI_SET_ENUM_HDR
 *
 * RACE_MMI_SET_ENUM_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_MMI_SET_ENUM_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        race_mmi_module_t module;
        uint8_t parameters[1];
    }PACKED RACE_MMI_SET_ENUM_CMD_STRU;

    typedef struct
    {
        race_mmi_module_t module;
        uint8_t status;
    }PACKED RACE_MMI_SET_ENUM_EVT_STRU;

    RACE_MMI_SET_ENUM_CMD_STRU *pCmd = (RACE_MMI_SET_ENUM_CMD_STRU *)pCmdMsg;
    RACE_MMI_SET_ENUM_EVT_STRU* pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_MMI_SET_ENUM, (uint16_t)sizeof(RACE_MMI_SET_ENUM_EVT_STRU), channel_id);

    RACE_LOG_MSGID_I("RACE_MMI_SET_ENUM_HDR() enter, channel_id = %x, module = %d\r\n", 2, channel_id, pCmd->module);

    if (pEvt != NULL)
    {
        pEvt->module = pCmd->module;
        pEvt->status = 1;
        #if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
        race_cmd_hostaudio_cosys_relay_set_cmd(pCmd->module, pCmd->parameters[0]);
        #endif
        switch (pCmd->module)
        {
            case RACE_MMI_MODULE_PEQ_GROUP_ID:
                race_mmi_set_peq_group_id(pCmd->parameters[0], &pEvt->status, AM_A2DP_PEQ);
                break;
            case RACE_MMI_MODULE_LINEIN_PEQ_GROUP_ID:
                race_mmi_set_peq_group_id(pCmd->parameters[0], &pEvt->status, AM_LINEIN_PEQ);
                break;
            case RACE_MMI_MODULE_VP_SET:
                race_mmi_set_vp_group_id(pCmd->parameters[0], &pEvt->status);
                break;
            /**
             * Add for line-in feature
             */
            #ifdef APPS_LINE_IN_SUPPORT
            case RACE_MMI_MODULE_AUDIO_PATH:
                if (pCmd->Hdr.type == RACE_TYPE_COMMAND) {
                    race_mmi_set_new_audio_path(pCmd->parameters[0], &pEvt->status);
                } else if (pCmd->Hdr.type == RACE_TYPE_RESPONSE) {
                    race_mmi_audio_path_push_response(pCmd->parameters[0]);
                    pEvt->status = 0;
                } else {
                    RACE_LOG_MSGID_I("RACE_MMI_SET_ENUM_HDR() enter, unknown type : %x", 1, pCmd->Hdr.type);
                }
                break;
            #endif /* APPS_LINE_IN_SUPPORT */

            #ifdef AIR_MCSYNC_SHARE_ENABLE
            case RACE_MMI_MODULE_SHARE_MODE:
                race_mmi_set_share_mode(pCmd->parameters[0], &pEvt->status);
                break;
            #endif
            default:
                break;
        }
    }

    return pEvt;
}

/**
 * RACE_MMI_GET_ENUM_HDR
 *
 * RACE_MMI_GET_ENUM_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_MMI_GET_ENUM_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    RACE_MMI_GET_ENUM_CMD_STRU *pCmd = (RACE_MMI_GET_ENUM_CMD_STRU *)pCmdMsg;
    RACE_MMI_GET_ENUM_EVT_STRU *pEvt = NULL;

    RACE_LOG_MSGID_I("RACE_MMI_GET_ENUM_HDR() enter, channel_id = %x, module = %d\r\n", 2, channel_id, pCmd->module);

    switch (pCmd->module)
    {
        case RACE_MMI_MODULE_PEQ_GROUP_ID: {
            pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_MMI_GET_ENUM, (uint16_t)sizeof(RACE_MMI_GET_ENUM_EVT_STRU), channel_id);
            if (pEvt) {
                pEvt->module = pCmd->module;
                race_mmi_get_peq_group_id(&pEvt->data[0], &pEvt->status, A2DP);
            }
            break;
        }

        case RACE_MMI_MODULE_LINEIN_PEQ_GROUP_ID: {
            pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_MMI_GET_ENUM, (uint16_t)sizeof(RACE_MMI_GET_ENUM_EVT_STRU), channel_id);
            if (pEvt) {
                pEvt->module = pCmd->module;
                race_mmi_get_peq_group_id(&pEvt->data[0], &pEvt->status, LINE_IN);
            }
            break;
        }

        case RACE_MMI_MODULE_ANC_STATUS: {
        #ifdef MTK_ANC_ENABLE
            pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_MMI_GET_ENUM, (uint16_t)sizeof(RACE_MMI_GET_ENUM_EVT_STRU) + sizeof(uint16_t), channel_id);
            if (pEvt) {
                pEvt->module = pCmd->module;
                race_mmi_get_anc_status(&pEvt->data[0], &pEvt->status);
            }
        #else
        #if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
            bool ret = FALSE;
            ret = race_cosys_send_data(RACE_COSYS_MODULE_ID_ANC_PASSTHROUGH, FALSE, (uint8_t *)pCmdMsg, pCmdMsg->hdr.length + 4);
            RACE_LOG_MSGID_I("[RACE][HOST AUDIO][COSYS]send cosys data ret %d, type(0x%x) msg_length:%d id:%d\n", 4, ret, pCmdMsg->hdr.type, pCmdMsg->hdr.length, pCmdMsg->hdr.id);
            g_mmi_anc_race_cosys_ch_id = channel_id;
        #endif
        #endif
            break;
        }

        case RACE_MMI_MODULE_VP_LANGUAGE:{
            uint16_t lang_cnt = 0;
#ifdef MTK_PROMPT_SOUND_ENABLE
            lang_cnt = app_voice_prompt_getLangCount();
#endif
            pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_MMI_GET_ENUM, (uint16_t)sizeof(RACE_MMI_GET_ENUM_EVT_STRU) + sizeof(uint16_t)*lang_cnt, channel_id);
            if (pEvt) {
                pEvt->module = pCmd->module;
                race_mmi_get_vp_lang_group_id(&pEvt->data[0], &pEvt->status);
            }
            break;
        }

        case RACE_MMI_MODULE_VP_GET:
            pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_MMI_GET_ENUM, (uint16_t)sizeof(RACE_MMI_GET_ENUM_EVT_STRU), channel_id);
            if (pEvt) {
                pEvt->module = pCmd->module;
                race_mmi_get_vp_group_id(&pEvt->data[0], &pEvt->status);
            }
            break;

        case RACE_MMI_MODULE_PASSTHRU_GAIN: {
        #ifdef MTK_ANC_ENABLE
            pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_MMI_GET_ENUM, (uint16_t)sizeof(RACE_MMI_GET_ENUM_EVT_STRU) + sizeof(uint8_t), channel_id);
            if (pEvt) {
                pEvt->module = pCmd->module;
                race_mmi_get_passthru_gain(&pEvt->data[0], &pEvt->status);
            }
        #else
            #if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
            bool ret = FALSE;
            ret = race_cosys_send_data(RACE_COSYS_MODULE_ID_ANC_PASSTHROUGH, FALSE, (uint8_t *)pCmdMsg, pCmdMsg->hdr.length + 4);
            RACE_LOG_MSGID_I("[RACE][HOST AUDIO][COSYS]send cosys data ret %d, type(0x%x) msg_length:%d id:%d\n", 4, ret, pCmdMsg->hdr.type, pCmdMsg->hdr.length, pCmdMsg->hdr.id);
            g_mmi_anc_race_cosys_ch_id = channel_id;
            #endif
        #endif
            break;
        }

        case RACE_MMI_MODULE_GAME_MODE:
            pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_MMI_GET_ENUM, (uint16_t)sizeof(RACE_MMI_GET_ENUM_EVT_STRU), channel_id);
            if (pEvt) {
                pEvt->module = pCmd->module;
                race_mmi_get_game_mode(&pEvt->data[0], &pEvt->status);
            }
            break;

        /**
         * Add for line-in feature
        */
        #ifdef APPS_LINE_IN_SUPPORT
        case RACE_MMI_MODULE_AUDIO_PATH:
            pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_MMI_GET_ENUM, (uint16_t)sizeof(RACE_MMI_GET_ENUM_EVT_STRU), channel_id);
            if (pEvt!= NULL) {
                pEvt->module = pCmd->module;
                race_mmi_get_audio_path(&pEvt->data[0], &pEvt->status);
            }
            break;
        #endif /* APPS_LINE_IN_SUPPORT */

        #ifdef AIR_MCSYNC_SHARE_ENABLE
        case RACE_MMI_MODULE_SHARE_MODE:
            pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_MMI_GET_ENUM, (uint16_t)sizeof(RACE_MMI_GET_ENUM_EVT_STRU), channel_id);
            if (pEvt!= NULL) {
                pEvt->module = pCmd->module;
                race_mmi_get_share_mode_sta(&pEvt->data[0], &pEvt->status);
            }
            break;
        #endif
        default:
            break;
    }

    return pEvt;
}

/**
 * RACE_HOSTAUDIO_PEQ_SAVE_STATUS_HDR
 *
 * RACE_HOSTAUDIO_PEQ_SAVE_STATUS_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_HOSTAUDIO_PEQ_SAVE_STATUS_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t audio_path_id;
        uint16_t peq_nvkey_id;
    }PACKED RACE_HOSTAUDIO_PEQ_SAVE_STATUS_CMD_STRU;

    typedef struct
    {
        uint8_t Status;
    }PACKED RACE_HOSTAUDIO_PEQ_SAVE_STATUS_EVT_STRU;

    RACE_HOSTAUDIO_PEQ_SAVE_STATUS_CMD_STRU *pCmd = (RACE_HOSTAUDIO_PEQ_SAVE_STATUS_CMD_STRU *)pCmdMsg;

    RACE_LOG_MSGID_I("RACE_HOSTAUDIO_SAVE_STATUS_HDR() enter, channel_id = %x\r\n",1, channel_id);

    RACE_HOSTAUDIO_PEQ_SAVE_STATUS_EVT_STRU* pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_HOSTAUDIO_PEQ_SAVE_STATUS, (uint16_t)sizeof(RACE_HOSTAUDIO_PEQ_SAVE_STATUS_EVT_STRU), channel_id);
    bt_sink_srv_am_result_t am_status;

    //printf("0x%02x 0x%02x \n", pCmd->audio_path_id, pCmd->peq_nvkey_id);

    am_feature_type_t audio_path_id = 0;
    if (pCmd->audio_path_id == AUDIO_PATH_BT) {
        audio_path_id = AM_A2DP_PEQ;
    } else if (pCmd->audio_path_id == AUDIO_PATH_LINE_IN) {
        audio_path_id = AM_LINEIN_PEQ;
    } else {
        RACE_LOG_MSGID_E("Un-supported scenario: %d\n",1,pCmd->audio_path_id);
    }

    if (pEvt != NULL)
    {
        bt_sink_srv_am_feature_t feature_param;
        memset(&feature_param, 0, sizeof(bt_sink_srv_am_feature_t));
        feature_param.type_mask                             = audio_path_id;
        feature_param.feature_param.peq_param.enable        = 1;
        feature_param.feature_param.peq_param.sound_mode    = PEQ_SOUND_MODE_SAVE;
        feature_param.feature_param.peq_param.u2ParamSize   = pCmd->peq_nvkey_id;
        am_status = am_audio_set_feature(FEATURE_NO_NEED_ID, &feature_param);
        pEvt->Status = am_status != AUD_EXECUTION_SUCCESS ? RACE_ERRCODE_FAIL : RACE_ERRCODE_SUCCESS;
    }

    return pEvt;
#else
    UNUSED(pCmdMsg);
    UNUSED(channel_id);
    return NULL;
#endif
}

void* RACE_CmdHandler_HOSTAUDIO(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id)
{
    void* ptr = NULL;

    RACE_LOG_MSGID_I("RACE_CmdHandler_HOSTAUDIO, type[0x%X], race_id[0x%X], channel_id[%d]", 3,
        pRaceHeaderCmd->hdr.type, pRaceHeaderCmd->hdr.id, channel_id);

/**
 * Add for line-in feature
 */
#ifdef APPS_LINE_IN_SUPPORT
    line_in_app_comm_channel_id = channel_id;
#endif /* APPS_LINE_IN_SUPPORT */

#ifdef AIR_MCSYNC_SHARE_ENABLE
    share_mode_channel_id = channel_id;
#endif

    switch (pRaceHeaderCmd->hdr.id)
    {
        case RACE_MMI_SET_ENUM:
            ptr = RACE_MMI_SET_ENUM_HDR(pRaceHeaderCmd, channel_id);
            break;
        case RACE_MMI_GET_ENUM:
            ptr = RACE_MMI_GET_ENUM_HDR(pRaceHeaderCmd, channel_id);
            break;
        case RACE_HOSTAUDIO_PEQ_SAVE_STATUS:
            ptr = RACE_HOSTAUDIO_PEQ_SAVE_STATUS_HDR(pRaceHeaderCmd, channel_id);
            break;
        default:
            break;
    }

    return ptr;
}

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
bool race_cmd_hostaudio_cosys_relay_set_cmd(race_mmi_module_t module, uint8_t parameters)
{
    bool is_relay = false;
    switch (module){
        case RACE_MMI_MODULE_PEQ_GROUP_ID:
        case RACE_MMI_MODULE_LINEIN_PEQ_GROUP_ID:
            {
            RACE_MMI_COSYS_RELAY_SET_ENUM_CMD_STRU relay_cmd;
            relay_cmd.module = module;
            relay_cmd.parameters[0] = parameters;
            bool ret = FALSE;
            ret = race_cosys_send_data(RACE_COSYS_MODULE_ID_HOST_AUDIO_SET, FALSE, (uint8_t *)&relay_cmd, sizeof(RACE_MMI_COSYS_RELAY_SET_ENUM_CMD_STRU));
            RACE_LOG_MSGID_I("[RACE][HOST AUDIO][COSYS]send cosys set MMI ret %d, module:%d, para:%d \n", 3, ret, relay_cmd.module, relay_cmd.parameters[0]);
            is_relay = true;
            }
            break;
        default:
            break;
    }

    return is_relay;
}
void race_cmd_hostaudio_cosys_relay_set_cmd_callback(bool is_critical, uint8_t *buff, uint32_t len)
{
    RACE_MMI_COSYS_RELAY_SET_ENUM_CMD_STRU *set_ptr = (RACE_MMI_COSYS_RELAY_SET_ENUM_CMD_STRU *)buff;
    uint8_t status = RACE_ERRCODE_FAIL;
    switch (set_ptr->module)
    {
        case RACE_MMI_MODULE_PEQ_GROUP_ID:
            race_mmi_set_peq_group_id(set_ptr->parameters[0], &status, AM_A2DP_PEQ);
            break;
        case RACE_MMI_MODULE_LINEIN_PEQ_GROUP_ID:
            race_mmi_set_peq_group_id(set_ptr->parameters[0], &status, AM_LINEIN_PEQ);
            break;
        default:
            RACE_LOG_MSGID_E("[RACE][HOST AUDIO][COSYS]send cosys set MMI callback error: %d,  \n", 1, set_ptr->module);
            break;
    }
    RACE_LOG_MSGID_I("[RACE][HOST AUDIO][COSYS]cosys set MMI callback module:%d, status:%d, \n", 2, set_ptr->module, status);

}

#endif

#endif /* RACE_HOSTAUDIO_CMD_ENABLE */

