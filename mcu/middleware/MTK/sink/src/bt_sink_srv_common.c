/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "bt_avm.h"
#include "avm_direct.h"
#include "bt_callback_manager.h"
#include "bt_sink_srv_ami.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_common.h"
#include "bt_connection_manager_internal.h"
#include "bt_sink_srv_call.h"
#include "bt_sink_srv_a2dp.h"
#include "bt_sink_srv_avrcp.h"
#include "bt_sink_srv_pbapc.h"
#include "bt_sink_srv_hf.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_call.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#endif /*MTK_AWS_MCE_ENABLE*/

#ifdef MTK_AWS_MCE_ENABLE
extern bt_status_t bt_sink_srv_aws_mce_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);
extern void bt_sink_srv_aws_mce_a2dp_app_report_callback(bt_aws_mce_report_info_t *para);
extern void bt_sink_srv_aws_mce_call_app_report_callback(bt_aws_mce_report_info_t *para);
#endif /*MTK_AWS_MCE_ENABLE*/
extern bt_status_t bt_sink_srv_call_set_mute(bt_sink_srv_mute_t type, bool mute);

bt_sink_feature_config_t bt_sink_srv_features_config;

void bt_sink_srv_config_features(bt_sink_feature_config_t *features)
{
    bt_sink_srv_memcpy(&bt_sink_srv_features_config, features, sizeof(bt_sink_feature_config_t));
}

const bt_sink_feature_config_t *bt_sink_srv_get_features_config(void)
{
    return &bt_sink_srv_features_config;
}

bt_status_t bt_sink_srv_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    uint32_t moduel = msg & 0xFF000000;
    switch (moduel) {
    #ifndef MTK_BT_CM_SUPPORT
        case BT_MODULE_SYSTEM:
            result = bt_sink_srv_cm_system_callback(msg, status, buffer);
            break;
    #endif
        case BT_MODULE_GAP:
        case BT_MODULE_SDP:
    #ifndef MTK_BT_CM_SUPPORT
            result = bt_sink_srv_cm_gap_callback(msg, status, buffer);
    #endif
            result = bt_sink_srv_hf_gap_callback(msg, status, buffer);
            result = bt_sink_srv_a2dp_common_callback(msg, status, buffer);
            break;

        case BT_MODULE_HFP:
        case BT_MODULE_HSP:
            result = bt_sink_srv_call_common_callback(msg, status, buffer);
            break;

        case BT_MODULE_A2DP:
            result = bt_sink_srv_a2dp_common_callback(msg, status, buffer);
         break;
         
        case BT_MODULE_AVM:
            result = bt_sink_srv_a2dp_common_callback(msg, status, buffer);
    #ifdef MTK_AWS_MCE_ENABLE
            result = bt_sink_srv_aws_mce_common_callback(msg, status, buffer);
    #endif
            break;

        case BT_MODULE_AVRCP:
            result = bt_sink_srv_avrcp_common_callback(msg, status, buffer);
            break;

        case BT_MODULE_PBAPC:
            result = bt_sink_srv_pbapc_common_callback(msg, status, buffer);
            break;
        
    #ifdef MTK_AWS_MCE_ENABLE
        case BT_MODULE_AWS_MCE:
            result = bt_sink_srv_aws_mce_common_callback(msg, status, buffer);
            break;
    #endif

        case BT_MODULE_MM:
            result = bt_sink_srv_a2dp_common_callback(msg, status, buffer);
            break;
#ifdef MTK_BT_HID_ENABLE
        case BT_MODULE_HID:
            bt_sink_srv_report_id("[SINK][COMMON]HID event", 0);
            result = bt_app_hid_event_callback(msg, status, buffer);
            break;
#endif
        default:
            bt_sink_srv_report_id("[SINK][COMMON]Unknown Bluetooth MSG:0x%x, status:0x%x", 2, msg, status);
            break;
    }
    return result;
}


bt_status_t bt_sink_srv_set_clock_offset_ptr_to_dsp(const bt_bd_addr_t *address)
{
    bt_sink_srv_assert(address && "Err: address NULL");
    uint32_t handle = bt_sink_srv_cm_get_gap_handle((bt_bd_addr_t *)address);
    if (handle == 0) {
        bt_sink_srv_report_id("[SINK][COMMON]GAP handle is NULL.", 0);
        //bt_sink_srv_assert(0 && "Err: handle NULL");
        return BT_SINK_SRV_STATUS_INVALID_PARAM;
    }

    const void *clk_offset_buf = bt_avm_get_clock_offset_address(handle);
    if (clk_offset_buf == NULL) {
        bt_sink_srv_report_id("[SINK][COMMON]Get clock offset ptr buff is NULL.", 0);
        //bt_sink_srv_assert(0 && "Err: clock_buff ptr NULL");
        return BT_SINK_SRV_STATUS_FAIL;
    }

    bt_sink_srv_ami_set_bt_inf_address((bt_sink_srv_am_bt_audio_param_t)clk_offset_buf);
    bt_sink_srv_report_id("[SINK][COMMON]Set clock offset ptr, handle:0x%08x, buf ptr:0x%08x", 2, 
                        handle, clk_offset_buf);
    return BT_STATUS_SUCCESS;
}


#ifdef MTK_AWS_MCE_ENABLE
bt_status_t bt_sink_srv_bt_clock_addition(bt_clock_t *target_clk, bt_clock_t *base_clk, uint32_t duration)
{
    
    bt_bd_addr_t *device_p = bt_sink_srv_cm_get_aws_connected_device();
    bt_status_t ret =BT_STATUS_FAIL;
    if(device_p && target_clk) {
        bt_clock_t dur_clock;
        dur_clock.nclk = (duration/1250)<<2;
        dur_clock.nclk_intra = duration%1250;
        if(!base_clk) {
            uint32_t connection_handle = bt_sink_srv_cm_get_gap_handle(device_p);
            bt_clock_t cur_clock = {0};
            bt_get_bt_clock(connection_handle, &(cur_clock));
#ifdef MTK_INITIAL_SYNC_BY_SAMPLE_INDEX_SUPPORT
            avm_direct_bt_clock_add_duration(target_clk, &cur_clock, &dur_clock, BT_ROLE_MASTER);
#else
            avm_direct_bt_clock_add_duration(target_clk, &cur_clock, &dur_clock, BT_ROLE_SLAVE);
#endif
            bt_sink_srv_report_id("[Sink] cur_nclk:0x%08x, cur_intra:0x%08x, duration:%d", 3, 
                cur_clock.nclk, cur_clock.nclk_intra, duration);
            bt_sink_srv_report_id("[Sink] target_nclk:0x%08x, target_intra:0x%08x", 2, 
                target_clk->nclk, target_clk->nclk_intra);
        } else {
#ifdef MTK_INITIAL_SYNC_BY_SAMPLE_INDEX_SUPPORT
            avm_direct_bt_clock_add_duration(target_clk, base_clk, &dur_clock, BT_ROLE_MASTER);
#else
            avm_direct_bt_clock_add_duration(target_clk, base_clk, &dur_clock, BT_ROLE_SLAVE);
#endif
        }
        ret = BT_STATUS_SUCCESS;
    }

    return ret;
}

bt_status_t bt_sink_srv_bt_clock_get_duration(bt_clock_t *target_clk, bt_clock_t 
    *base_clk, int32_t *duration)
{
    bt_sink_srv_mutex_lock();
    bt_bd_addr_t *device_p = bt_sink_srv_cm_get_aws_connected_device();
    bt_status_t ret = BT_STATUS_FAIL;
    if(device_p && target_clk && duration) {
        if(!base_clk) {
            uint32_t connection_handle = bt_sink_srv_cm_get_gap_handle(device_p);
            bt_clock_t cur_clock = {0};
            bt_get_bt_clock(connection_handle, &(cur_clock));
            *duration = ((int)(target_clk->nclk - cur_clock.nclk)*625>>1) + 
                (int16_t)(target_clk->nclk_intra - cur_clock.nclk_intra);
            bt_sink_srv_report_id("[Sink] cur_nclk:0x%08x, cur_intra:0x%08x, duration:%d", 3, 
                cur_clock.nclk, cur_clock.nclk_intra, *duration);
            bt_sink_srv_report_id("[Sink] target_nclk:0x%08x, target_intra:0x%08x", 2, 
                target_clk->nclk, target_clk->nclk_intra);
        } else {
            *duration = ((int)(target_clk->nclk - base_clk->nclk)*625>>1) + 
                (int16_t)(target_clk->nclk_intra - base_clk->nclk_intra);
        }
        ret = BT_STATUS_SUCCESS;
    }
    bt_sink_srv_mutex_unlock();

    return ret;
}

bt_status_t bt_sink_srv_convert_bt_clock_2_gpt_count(const bt_clock_t *bt_clock, uint32_t *gpt_count)
{
    bt_sink_srv_mutex_lock();
    bt_bd_addr_t *device_p = bt_sink_srv_cm_get_aws_connected_device();
    bt_status_t ret = BT_STATUS_FAIL;
    if(device_p && bt_clock && gpt_count) {
        uint32_t connection_handle = bt_sink_srv_cm_get_gap_handle(device_p);
        bt_clock_t cur_clock = {0};
        extern bt_status_t bt_get_bt_clock_with_gpt(uint32_t gap_handle, bt_clock_t *bt_clock, uint32_t *gpt_count);
        bt_get_bt_clock_with_gpt(connection_handle, &(cur_clock), gpt_count);
        if (cur_clock.nclk == 0 && cur_clock.nclk_intra == 0) {
        	  bt_sink_srv_mutex_unlock();
		        return ret;
		//bt_get_bt_clock_with_gpt(connection_handle, &(cur_clock), gpt_count);
        }
        cur_clock.nclk = cur_clock.nclk&0xfffffffc;
        uint32_t duration = 0;
#ifndef MTK_INITIAL_SYNC_BY_SAMPLE_INDEX_SUPPORT
        cur_clock.nclk_intra   = (cur_clock.nclk_intra + 625) % 1250;
#endif
        uint32_t nclk = bt_clock->nclk - cur_clock.nclk;
        if(bt_clock->nclk_intra < cur_clock.nclk_intra) {
            duration = (((bt_clock->nclk - cur_clock.nclk - 4)&0x0fffffff)>>2)*1250 + 
                (bt_clock->nclk_intra + 1250 - cur_clock.nclk_intra);
        } else {
            duration = (((bt_clock->nclk-cur_clock.nclk) & 0x0fffffff) >> 2)*1250 + bt_clock->nclk_intra-cur_clock.nclk_intra;
        }
        *gpt_count = (*gpt_count) + duration;
        bt_sink_srv_report_id("[Sink] ****** cur_nclk:0x%08x, cur_intra:0x%08x, target_nclk:0x%08x, target_intra:0x%08x, duration:%d", 5,
            cur_clock.nclk, cur_clock.nclk_intra, bt_clock->nclk, bt_clock->nclk_intra, duration);
        if(!(nclk&0x80000000)) {
            ret = BT_STATUS_SUCCESS;
        }
    }
    bt_sink_srv_mutex_unlock();

    return ret;

}

#endif /*MTK_AWS_MCE_ENABLE*/

void bt_sink_srv_register_callback_init(void)
{
    bt_callback_manager_register_callback(bt_callback_type_app_event,
                                      (uint32_t)(MODULE_MASK_GAP | MODULE_MASK_SYSTEM | MODULE_MASK_HFP |
                                              MODULE_MASK_HSP | MODULE_MASK_AVRCP | MODULE_MASK_A2DP |
                                              MODULE_MASK_PBAPC | MODULE_MASK_SPP | MODULE_MASK_AWS_MCE |
                                              MODULE_MASK_MM | MODULE_MASK_AVM | MODULE_MASK_SDP
                                              | MODULE_MASK_HID),
                                      (void *)bt_sink_srv_common_callback);
    bt_callback_manager_register_callback(bt_callback_type_hfp_get_init_params,
                                      0,
                                      (void *)bt_sink_srv_hf_get_init_params);
    bt_callback_manager_register_callback(bt_callback_type_a2dp_get_init_params,
                                      0,
                                      (void *)bt_sink_srv_a2dp_get_init_params);
}

#ifdef MTK_AWS_MCE_ENABLE
void bt_sink_srv_register_aws_mce_report_callback(void)
{
    bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_SINK_MUSIC, bt_sink_srv_aws_mce_a2dp_app_report_callback);
    bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_SINK_CALL, bt_sink_srv_aws_mce_call_app_report_callback);
}
#endif /*MTK_AWS_MCE_ENABLE*/

uint32_t bt_sink_srv_get_volume(bt_bd_addr_t *bd_addr, bt_sink_srv_volume_type_t type)
{
    uint32_t volume = 0xffffffff;
    if(type == BT_SINK_SRV_VOLUME_HFP) {
        bt_sink_srv_hf_get_speaker_volume(bd_addr, &volume);
    } else if(type == BT_SINK_SRV_VOLUME_A2DP) {
        bt_sink_srv_a2dp_get_volume(bd_addr, &volume);
    }
    
    return volume;
}

uint32_t bt_sink_srv_get_device_state(const bt_bd_addr_t *device_address, bt_sink_srv_device_state_t *state_list, uint32_t list_number)
{
    bt_bd_addr_t null_address = {0};

    uint32_t address_number = 0;
    bt_bd_addr_t address_list[BT_SINK_SRV_MAX_DEVICE_NUM] = {{0}};

    if (device_address != NULL) {
        address_number = 1;
        bt_sink_srv_memcpy(&address_list[0], device_address, sizeof(bt_bd_addr_t));
    } else {
    #ifdef MTK_BT_CM_SUPPORT
        address_number = bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS),
            address_list, BT_SINK_SRV_MAX_DEVICE_NUM);
    #endif
    }

    for (uint32_t i = 0; i < address_number; i++) {
        if (bt_sink_srv_memcmp(&address_list[i], null_address, sizeof(bt_bd_addr_t)) != 0) {
            bt_sink_srv_device_state_t device_state = {{0}};

            bt_sink_srv_memcpy(&device_state.address, &address_list[i], sizeof(bt_bd_addr_t));
            device_state.music_state = bt_sink_srv_music_get_music_state(&device_state.address);
            bt_sink_srv_call_get_device_state(&device_state);

            bt_sink_srv_report_id("[Sink][Common]get device state, address:0x%x-%x-%x-%x-%x-%x", 6,
                device_state.address[0], device_state.address[1], device_state.address[2],
                device_state.address[3], device_state.address[4], device_state.address[5]);

            if (i < list_number) {
                bt_sink_srv_memcpy(&state_list[i], &device_state, sizeof(bt_sink_srv_device_state_t));
                bt_sink_srv_report_id("[Sink][Common]get device state, music:0x%x call:0x%x sco:0x%x", 3,
                    device_state.music_state, device_state.call_state, device_state.sco_state);
            }
        }
    }
    
    return (address_number < list_number) ? address_number : list_number;
}

bt_status_t bt_sink_srv_set_mute(bt_sink_srv_mute_t type, bool mute)
{
    bt_status_t status = BT_STATUS_SUCCESS;

    switch (type) {
        case BT_SINK_SRV_MUTE_MICROPHONE: {
            status = bt_sink_srv_call_set_mute(type, mute);
            break;
        }

        case BT_SINK_SRV_MUTE_SPEAKER: {
            const audio_src_srv_handle_t *device = audio_src_srv_get_runing_pseudo_device();
            if (device != NULL) {
                bt_sink_srv_report_id("[Sink]set mute, mute 0x%x speaker", 1, device->type);
                if ((device->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP) ||
                    (device->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP)) {
                    status = bt_sink_srv_music_set_mute(mute);
                } else if ((device->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_HFP) ||
                    (device->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_HFP)) {
                    status = bt_sink_srv_call_set_mute(type, mute);
                } else {
                    /* Add more cases here. */
                }
            }
            break;
        }

        default: {
            break;
        }
    }

    bt_sink_srv_report_id("[Sink]set mute, type:%x mute:%d status:0x%x", 3, type, mute, status);
    return status;
}
#ifdef MTK_AUDIO_SYNC_ENABLE
#define BT_SINK_SRV_MAX_SYNC_MODULE_NUMBER 4
#define BT_SINK_SRV_MAX_SYNC_DATA_LENGTH   32

static bt_sink_srv_sync_callback_t sync_callback[BT_SINK_SRV_MAX_SYNC_MODULE_NUMBER];
static volatile uint8_t bt_sink_sync_stop_counter = 0;

typedef struct {
    uint8_t type;
    uint8_t length;
    uint16_t reserved;
    uint8_t data[BT_SINK_SRV_MAX_SYNC_DATA_LENGTH];
} bt_sink_srv_sync_parameter_t;

bt_status_t bt_sink_srv_register_sync_callback(bt_sink_srv_sync_feature_t feature_type, bt_sink_srv_sync_feature_callback_t callback)
{
    bt_status_t ret = BT_STATUS_FAIL;
    uint32_t i = 0;

    for(i = 0; i<BT_SINK_SRV_MAX_SYNC_MODULE_NUMBER; i++) {
        if(sync_callback[i].type == feature_type) return ret;
    }

    for(i = 0; i<BT_SINK_SRV_MAX_SYNC_MODULE_NUMBER; i++) {
        if(sync_callback[i].type == 0) {
            sync_callback[i].type = feature_type;
            sync_callback[i].sync_callback = callback;
            ret = BT_STATUS_SUCCESS;
            break;
        }
    }

    bt_sink_srv_report_id("regiester_sync_callback, feature_type:0x%02x, ret:0x%08x", 2, feature_type, ret);
    return ret;
}


bt_status_t bt_sink_srv_deregister_sync_callback(bt_sink_srv_sync_feature_t feature_type)
{
    bt_status_t ret = BT_STATUS_FAIL;
    uint32_t i = 0;

    for(i = 0; i<BT_SINK_SRV_MAX_SYNC_MODULE_NUMBER; i++) {
        if(sync_callback[i].type == feature_type) {
            sync_callback[i].type = 0;
            sync_callback[i].sync_callback = NULL;
            ret = BT_STATUS_SUCCESS;
            break;
        }
    }

    bt_sink_srv_report_id("deregiester_sync_callback, feature_type:0x%02x, ret:0x%08x", 2, feature_type, ret);
    return ret;
}

static void bt_sink_srv_sync_gpt_timeout(uint32_t timer_id, uint32_t data)
{
    bt_sink_srv_report_id("[ERROR] sync_gpt_timeout, guard timer type:0x%x", 1, data);

    bt_sink_srv_set_sync_state(false);
    /* only care A2DP sync stop due to need release pseudo device & codec */
    if (BT_SINK_SRV_MUSIC_STOP_TYPE == data) {
        uint32_t i = 0;
        bt_sink_srv_sync_feature_callback_t sync_data_callback = NULL;
        for(i = 0; i < BT_SINK_SRV_MAX_SYNC_MODULE_NUMBER; i++) {
            if(sync_callback[i].type == BT_SINK_SRV_MUSIC_STOP_TYPE) {
                sync_data_callback = sync_callback[i].sync_callback;
                break;
            }
        }
        if (sync_data_callback) {
            bt_sink_srv_sync_callback_data_t app_sync_data;
            app_sync_data.type = BT_SINK_SRV_MUSIC_STOP_TYPE;
            app_sync_data.length = 0;
            app_sync_data.data = NULL;
            app_sync_data.gpt_count = 0;
            sync_data_callback(BT_SINK_SRV_SYNC_TIMEOUT, &app_sync_data);
        }
    }
}

extern uint8_t bt_avm_request_sync_gpt(void *parameter, uint32_t duration, uint32_t timeout_duration);

bt_status_t bt_sink_srv_request_sync_gpt(bt_sink_srv_get_sync_data_parameter_t *sync_parameters)
{
    uint8_t avm_ret = 0xFF;
    bt_sink_srv_sync_parameter_t sync_data;
    sync_data.type = sync_parameters->type;
    sync_data.length = sync_parameters->length;
    sync_data.reserved = 0;
    bt_sink_srv_assert(sync_data.length <= BT_SINK_SRV_MAX_SYNC_DATA_LENGTH && "data is too long");
    bt_sink_srv_memcpy(sync_data.data, sync_parameters->data, sync_data.length);

    bt_role_handover_state_t rho_state = bt_role_handover_get_state();
    bt_status_t ret = BT_STATUS_FAIL;
    if (BT_ROLE_HANDOVER_STATE_IDLE == rho_state) {
        avm_ret = bt_avm_request_sync_gpt((void *)&sync_data, sync_parameters->duration, sync_parameters->timeout_duration);
        if (0x00 == avm_ret) {
            bt_sink_srv_set_sync_state(true);
            /* duration + guard time */
            uint32_t timeout = sync_parameters->timeout_duration/1000 + 500;
            bt_timer_ext_start(BT_SINK_SRV_AUDIO_SYNC_STOP_GUARD_TIMER_ID, sync_data.type, timeout, bt_sink_srv_sync_gpt_timeout);
            ret = BT_STATUS_SUCCESS;
        }
    }

    bt_sink_srv_report_id("request_sync_gpt, type:0x%02x, length:0x%02x, duration:0x%08x, timeout:0x%08x, rho_state:0x%x, avm_ret:0x%x", 6, sync_data.type, sync_data.length,
                                                                                    sync_parameters->duration, sync_parameters->timeout_duration, rho_state, avm_ret);
    return ret;
}

void bt_sink_srv_audio_sync_callback(void *para, uint32_t event, uint32_t gpt_count)
{
    bt_sink_srv_sync_parameter_t *sync_data = (bt_sink_srv_sync_parameter_t *)para;
    bt_sink_srv_sync_feature_callback_t sync_data_callback = NULL;
    bt_sink_srv_set_sync_state(false);
    uint32_t i = 0;
    for(i = 0; i < BT_SINK_SRV_MAX_SYNC_MODULE_NUMBER; i++) {
        if(sync_callback[i].type == sync_data->type) {
            sync_data_callback = sync_callback[i].sync_callback;
            break;
        }
    }
    bt_timer_ext_t *guard_timer = bt_timer_ext_find(BT_SINK_SRV_AUDIO_SYNC_STOP_GUARD_TIMER_ID);
    if (guard_timer) {
        if (guard_timer->data != sync_data->type) {
            bt_sink_srv_report_id("[ERROR] sync_callback, type:0x%02x, guard timer type:0x%x", 2, sync_data->type, guard_timer->data);
        }
        bt_timer_ext_stop(BT_SINK_SRV_AUDIO_SYNC_STOP_GUARD_TIMER_ID);
    }
    if(sync_data_callback) {
        bt_sink_srv_sync_callback_data_t app_sync_data;
        app_sync_data.type = sync_data->type;
        app_sync_data.length = sync_data->length;
        app_sync_data.data = sync_data->data;
        app_sync_data.gpt_count = gpt_count;
        sync_data_callback(event, &app_sync_data);
    } else {
        bt_sink_srv_assert(0 && "callback not found");
    }

    bt_sink_srv_report_id("sync_callback, type:0x%02x, length:0x%02x, event:0x%02x, gpt_count:0x%08x, sync_data_callback:0x%08x", 5, sync_data->type,
                                                                                      sync_data->length, event, gpt_count, sync_data_callback);
}

void bt_sink_srv_set_sync_state(bool is_running)
{
    if(is_running) {
        bt_sink_sync_stop_counter++;
    } else {
        if (bt_sink_sync_stop_counter > 0) {
            bt_sink_sync_stop_counter--;
        }
    }
    bt_sink_srv_report_id("bt_sink_srv_set_sync_state:0x%x, counter: 0x%x", 2, is_running, bt_sink_sync_stop_counter);
}

uint8_t bt_sink_srv_get_sync_counter(void)
{
    bt_sink_srv_report_id("bt_sink_srv_get_sync_counter, counter:0x%x", 1, bt_sink_sync_stop_counter);
    return bt_sink_sync_stop_counter;
}


#endif


#if 0
bt_sink_srv_device_info_t g_bt_sink_srv_device_info[3 + 2];
void bt_sink_srv_update_device_info(uint32_t handle, bt_sink_srv_type_t type, uint8_t state)
{
    uint8_t i= 0;
    bt_sink_srv_report_id("[LE_BT][Sink]update_info: handle:0x%x, type:0x%x, state:0x%x", 3, handle, type, state);
    for (; i < 6; i++) {

        if (state == BT_SINK_SRV_STATE_CONNECTED) {
            bt_sink_srv_report_id("[LE_BT][Sink]conn:mask:0x%x, handle:0x%x", 2, g_bt_sink_srv_device_info[i].mask, g_bt_sink_srv_device_info[i].handle);

            if (!g_bt_sink_srv_device_info[i].mask) {
                 g_bt_sink_srv_device_info[i].mask = (0x08 | (i + 1));
                 g_bt_sink_srv_device_info[i].handle = handle;
                 g_bt_sink_srv_device_info[i].type = type;
                 break;
            } else if (g_bt_sink_srv_device_info[i].handle == handle) {
                if (g_bt_sink_srv_device_info[i].type == type) {
                    bt_sink_srv_report_id("[LE_BT][Sink]update_info: connect is exist",0);
                    break;
                }
            }
        } else if (state == BT_SINK_SRV_STATE_POWER_ON) {
            bt_sink_srv_report_id("[LE_BT][Sink]disconn: check mask:0x%x, handle:0x%x", 2, g_bt_sink_srv_device_info[i].mask, g_bt_sink_srv_device_info[i].handle);
            if(g_bt_sink_srv_device_info[i].mask && handle == g_bt_sink_srv_device_info[i].handle) {
                g_bt_sink_srv_device_info[i].mask = 0;
                break;
            }
        } else if (state == BT_SINK_SRV_STATE_STREAMING) {
            bt_sink_srv_report_id("[LE_BT][Sink]operation: check mask:0x%x, handle:0x%x", 2, g_bt_sink_srv_device_info[i].mask, g_bt_sink_srv_device_info[i].handle);
            if(g_bt_sink_srv_device_info[i].mask && handle == g_bt_sink_srv_device_info[i].handle) {
                g_bt_sink_srv_device_info[i].mask |= 0x81;
                break;
            }
       }
    }
    bt_sink_srv_report_id("[LE_BT][Sink]update_info, ret: i:%x, handle: 0x%x, mask: %x", 2, i, handle);
}

bt_sink_srv_device_info_t * bt_sink_srv_get_device_info()
{
    return &g_bt_sink_srv_device_info;
}
#endif


