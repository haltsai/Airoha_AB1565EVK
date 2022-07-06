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
 
#ifndef __BT_SINK_SRV_HFP_H__
#define __BT_SINK_SRV_HFP_H__

#include "bt_sink_srv.h"
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
#include "bt_timer_external.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BT_SINK_SRV_HF_LINK_NUM 0x03
#define BT_SINK_SRV_HF_CMD_LENGTH 12
#define BT_SINK_SRV_HF_LONG_CMD_LENGTH 48
#define BT_SINK_SRV_HF_DIAL_CMD_LENGTH 48
#define BT_SINK_SRV_HF_VOLUME_MASK 0xF0

#define BT_SINK_SRV_TIMER_HF_WAIT_CALL_IND_DUR             (50)
#define BT_SINK_SRV_HF_SET_VOLUME_TIMER_DUR                (80)

#define BT_SINK_SRV_HF_SWITCH_LINK  0x80

typedef enum {
    BT_SINK_SRV_HF_CALL_STATE_IDLE              = 0,
    BT_SINK_SRV_HF_CALL_STATE_INCOMING          = BT_SINK_SRV_STATE_INCOMING,
    BT_SINK_SRV_HF_CALL_STATE_OUTGOING          = BT_SINK_SRV_STATE_OUTGOING,
    BT_SINK_SRV_HF_CALL_STATE_ACTIVE            = BT_SINK_SRV_STATE_ACTIVE,
    BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING      = BT_SINK_SRV_STATE_TWC_INCOMING,
    BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING      = BT_SINK_SRV_STATE_TWC_OUTGOING,
    BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE       = BT_SINK_SRV_STATE_HELD_ACTIVE,
    BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING    = BT_SINK_SRV_STATE_HELD_REMAINING,
    BT_SINK_SRV_HF_CALL_STATE_MULTIPARTY        = BT_SINK_SRV_STATE_MULTIPARTY
} bt_sink_srv_hf_call_state_t;

#define BT_SINK_SRV_HF_CALL_STATE_ALL (bt_sink_srv_hf_call_state_t) \
                                    (BT_SINK_SRV_HF_CALL_STATE_INCOMING| \
                                    BT_SINK_SRV_HF_CALL_STATE_OUTGOING| \
                                    BT_SINK_SRV_HF_CALL_STATE_ACTIVE| \
                                    BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING| \
                                    BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING| \
                                    BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE| \
                                    BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING| \
                                    BT_SINK_SRV_HF_CALL_STATE_MULTIPARTY)

//typedef enum {
#define BT_SINK_SRV_HF_CHLD_RELEASE_HELD_REJECT_WAITING  "AT+CHLD=0"
#define BT_SINK_SRV_HF_CHLD_RELEASE_ACTIVE_ACCEPT_OTHER  "AT+CHLD=1"
#define BT_SINK_SRV_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER     "AT+CHLD=2"
#define BT_SINK_SRV_HF_CHLD_ADD_HELD_TO_CONVERSATION     "AT+CHLD=3"
#define BT_SINK_SRV_HF_CHLD_EXPLICIT_CALL_TRANSFER       "AT+CHLD=4"
//} bt_sink_srv_hf_hold_action_t;

typedef enum
{   
    BT_HFP_CUSTOM_CMD_TYPE_NONE,
    BT_HFP_CUSTOM_CMD_TYPE_XAPL,
    BT_HFP_CUSTOM_CMD_TYPE_APLSIRI
} bt_sink_srv_hf_custom_cmd_type_t;

typedef struct {
    bt_sink_srv_hf_custom_cmd_type_t cmd_type;
    union {
        struct {
            uint8_t result;
        } aplsiri;
        struct {
            uint8_t feature;
        } xapl;
    } param;
} bt_sink_srv_hf_custom_cmd_result_t;

typedef struct
{
    bt_sink_srv_hf_custom_cmd_type_t type;
    uint8_t *command;
} bt_sink_srv_hf_custom_cmd_t;

typedef enum {
    BT_SINK_SRV_HF_FLAG_NONE            = 0x0000,
    BT_SINK_SRV_HF_FLAG_SCO_CREATED     = 0x0001,
    BT_SINK_SRV_HF_FLAG_SCO_ACTIVE      = 0x0002,
    BT_SINK_SRV_HF_FLAG_QUERY_NAME      = 0x0004,
    BT_SINK_SRV_HF_FLAG_USER_REJECT     = 0x0008,
    BT_SINK_SRV_HF_FLAG_QUERY_LIST      = 0x0010,
    BT_SINK_SRV_HF_FLAG_RINGING         = 0x0020,
    BT_SINK_SRV_HF_FLAG_TWC_RINGING     = 0x0040,
    BT_SINK_SRV_HF_FLAG_RECONNECT_SCO   = 0x0080,
    BT_SINK_SRV_HF_FLAG_DISCONNECT_HFP  = 0x0100,
#ifdef AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
    BT_SINK_SRV_HF_FLAG_HF_SWITCH       = 0x0200,
#endif
    BT_SINK_SRV_HF_FLAG_DISABLE_SNIFF   = 0x0400,
} bt_sink_srv_hf_flag_t;

typedef struct {
    uint32_t handle;
    bt_bd_addr_t address;
    bt_hfp_ag_feature_t ag_featues;
    bt_hfp_ag_hold_feature_t ag_chld_feature;
    bt_sink_srv_hf_call_state_t call_state;
    bt_sink_srv_hf_flag_t flag;
    bt_sink_srv_caller_information_t caller;
} bt_sink_srv_hf_link_context_t;

typedef struct {
    bool is_used;
    bt_sink_srv_hf_link_context_t link;
    void* device;
    uint8_t set_volume;
} bt_sink_srv_hf_context_t;

typedef struct {
    bt_hfp_audio_codec_type_t codec;
    bool connected;
} bt_sink_srv_hf_sco_state_change_t;

typedef struct {
    bt_bd_addr_t          bt_addr;
    uint8_t               volume;
} bt_sink_srv_speaker_volume_change_t;

#ifdef MTK_BT_CM_SUPPORT
typedef struct {
    uint8_t speaker_volume;
} bt_sink_srv_hf_stored_data_t;
#endif

bt_status_t bt_sink_srv_hf_get_init_params(bt_hfp_init_param_t *param);
bt_status_t bt_sink_srv_hf_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);
bt_status_t bt_sink_srv_hf_gap_callback(bt_msg_type_t msg, bt_status_t status, void *parameter);
bt_status_t bt_sink_srv_hf_action_handler(bt_sink_srv_action_t action, void *parameters);
void bt_sink_srv_hf_handle_setup_ind(bt_sink_srv_hf_context_t *link, bt_hfp_ciev_call_setup_state_t setup);
void bt_sink_srv_hf_handle_call_ind(bt_sink_srv_hf_context_t *link, bt_hfp_ciev_call_state_t call);
void bt_sink_srv_hf_handle_held_ind(bt_sink_srv_hf_context_t *link, bt_hfp_ciev_call_hold_state_t held);
void bt_sink_srv_hf_handle_call_info_ind(bt_sink_srv_hf_context_t *link, bt_hfp_call_list_ind_t *clcc);
void bt_sink_srv_hf_answer_call(bool accept);
void bt_sink_srv_hf_terminate_call(void);
void bt_sink_srv_hf_dial_number(char *number);
void bt_sink_srv_hf_dial_last(void);
void bt_sink_srv_hf_dial_missed(void);
void bt_sink_srv_hf_switch_audio_path(void);
void bt_sink_srv_hf_release_all_held_call(void);
void bt_sink_srv_hf_release_all_active_accept_others(void);
void bt_sink_srv_hf_hold_all_active_accept_others(void);
void bt_sink_srv_hf_release_special(uint8_t index);
void bt_sink_srv_hf_hold_special(uint8_t index);
void bt_sink_srv_hf_add_held_to_conversation(void);
void bt_sink_srv_hf_explicit_call_transfer(void);
void bt_sink_srv_hf_voice_recognition_activate(bool active);
void bt_sink_srv_hf_switch_audio_device(void);
void bt_sink_srv_hf_query_call_list(bt_bd_addr_t *address);
void bt_sink_srv_hf_send_dtmf(bt_sink_srv_send_dtmf_t *request);
void bt_sink_srv_hf_enable_apl_custom_commands(uint32_t handle, const bt_sink_srv_hf_custom_command_xapl_params_t *params);
void bt_sink_srv_hf_hold_device(bt_sink_srv_hf_context_t *device);
bt_sink_srv_hf_context_t *bt_sink_srv_hf_get_highlight_device(void);
void bt_sink_srv_hf_set_highlight_device(bt_sink_srv_hf_context_t *device);
bt_sink_srv_hf_context_t *bt_sink_srv_hf_get_context_by_flag(bt_sink_srv_hf_flag_t flag);
bt_sink_srv_hf_context_t *bt_sink_srv_hf_find_device_by_call_state(bt_sink_srv_hf_call_state_t call_state);
void bt_sink_srv_hf_call_state_change(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p,
                                      bt_sink_srv_hf_call_state_t previous_state,
                                      bt_sink_srv_hf_call_state_t new_state);
void bt_sink_srv_hf_mp_answer(bt_sink_srv_hf_context_t *current_device, bool accept);
void bt_sink_srv_hf_mp_swap(bt_sink_srv_hf_context_t *active_device, bt_sink_srv_hf_context_t *held_device);
void bt_sink_srv_hf_mp_state_change(bt_sink_srv_hf_context_t *device);
void bt_sink_srv_hf_mp_set_sco(bt_sink_srv_hf_context_t *highlight, bt_sink_srv_hf_context_t *device);
void bt_sink_srv_hf_mp_switch_audio(void);
bt_status_t bt_sink_srv_hf_set_audio_status(uint32_t handle, bt_hfp_audio_status_t status);
bt_status_t bt_sink_srv_hf_apl_report_battery(uint8_t battery_level);
void bt_sink_srv_hf_apl_siri(void);
void bt_sink_srv_hf_ncer_activate(bool active);
bool bt_sink_srv_hf_check_is_connected(bt_bd_addr_t* addr);
bool bt_sink_srv_hf_check_is_connected_by_context(bt_sink_srv_hf_context_t *context);
void bt_sink_srv_hf_set_hsp_flag(bool enable);
void bt_sink_srv_hf_reset_highlight_device(void);
void bt_sink_srv_hf_update_last_context(bt_sink_srv_hf_context_t *context, bool is_add);
bt_status_t bt_sink_srv_hf_get_speaker_volume(bt_bd_addr_t *address, uint32_t *volume);
bt_status_t bt_sink_srv_hf_xiaomi_custom(const char *parameter, uint32_t parameter_length);
bt_status_t bt_sink_srv_hf_mtk_custom(const char *parameter, uint32_t parameter_length);

bt_sink_srv_hf_context_t *bt_sink_srv_hf_alloc_free_context(bt_bd_addr_t *address);
bt_sink_srv_hf_context_t *bt_sink_srv_hf_get_context_by_address(bt_bd_addr_t *address);
uint32_t bt_sink_hf_get_connected_device_list(bt_bd_addr_t *addr_list);
uint32_t bt_sink_srv_hf_get_connected_sco_count(void);

bool bt_sink_srv_hf_get_nvdm_data(bt_bd_addr_t *bt_addr, void *data_p, uint32_t size);
bool bt_sink_srv_hf_set_nvdm_data(bt_bd_addr_t *bt_addr, void *data_p, uint32_t size);

bt_status_t bt_sink_srv_hf_cm_event_callback(bt_cm_event_t event_id, void *params, uint32_t params_len);

#define BT_SINK_SRV_HF_QUERY_CALL_LIST(handle) bt_hfp_send_command(handle, (uint8_t *)"AT+CLCC", (uint16_t)bt_sink_srv_strlen("AT+CLCC"))
#define BT_SINK_SRV_HF_DIAL_LAST_NUMBER(handle) bt_hfp_send_command(handle, (uint8_t *)"AT+BLDN", (uint16_t)bt_sink_srv_strlen("AT+BLDN"))
#define BT_SINK_SRV_HF_ANSWER_CALL(handle) bt_hfp_send_command(handle, (uint8_t *)"ATA", (uint16_t)bt_sink_srv_strlen("ATA"))
#define BT_SINK_SRV_HF_REJECT_CALL(handle) bt_hfp_send_command(handle, (uint8_t *)"AT+CHUP", (uint16_t)bt_sink_srv_strlen("AT+CHUP"))
#define BT_SINK_SRV_HF_TERMINATE_CALL(handle) bt_hfp_send_command(handle, (uint8_t *)"AT+CHUP", (uint16_t)bt_sink_srv_strlen("AT+CHUP"))
#define BT_SINK_SRV_HF_HOLD_CALL(handle, command) bt_hfp_send_command(handle, (uint8_t *)command, (uint16_t)bt_sink_srv_strlen(command))
#define BT_SINK_SRV_HF_SYNC_MIC_GAIN(handle, command) bt_hfp_send_command(handle, (uint8_t *)command, (uint16_t)bt_sink_srv_strlen(command))
#define BT_SINK_SRV_HF_SYNC_SPEAKER_GAIN(handle, command) bt_hfp_send_command(handle, (uint8_t *)command, (uint16_t)bt_sink_srv_strlen(command))
#define BT_SINK_SRV_HF_VOICE_RECOGNITION(handle, command) bt_hfp_send_command(handle, (uint8_t *)command, (uint16_t)bt_sink_srv_strlen(command))
#define BT_SINK_SRV_HF_DIAL_NUMBER(handle, command) bt_hfp_send_command(handle, (uint8_t *)command, (uint16_t)bt_sink_srv_strlen(command))
#define BT_SINK_SRV_HF_SEND_DTMF(handle, command) bt_hfp_send_command(handle, (uint8_t *)command, (uint16_t)bt_sink_srv_strlen(command))
#define BT_SINK_SRV_HF_ENABLE_CUSTOM_CMDS(handle, command) bt_hfp_send_command(handle, (uint8_t *)command, (uint16_t)bt_sink_srv_strlen(command))

#ifdef __cplusplus
}
#endif

#endif /* __BT_SINK_SRV_HFP_H__ */

