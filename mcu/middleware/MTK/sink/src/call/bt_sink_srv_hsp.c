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

#include "bt_sink_srv_hf.h"
#include "bt_sink_srv_hsp.h"
#include "bt_sink_srv_call_pseudo_dev.h"
#include "bt_sink_srv_utils.h"
#include "bt_connection_manager_internal.h"
#include "bt_hsp.h"
#include "bt_sink_srv_aws_mce_call.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_sink_srv_common.h"
#include "bt_device_manager_internal.h"

bt_sink_srv_hsp_context_t g_sink_srv_hsp_context[BT_SINK_SRV_HSP_LINK_NUM];
static bt_sink_srv_hsp_context_t *g_sink_srv_hsp_hightlight_p = NULL;

static bool bt_sink_srv_hsp_get_nvdm_data(bt_bd_addr_t *bt_addr, void *data_p, uint32_t size);
static bool bt_sink_srv_hsp_set_nvdm_data(bt_bd_addr_t *bt_addr, void *data_p, uint32_t size);


void bt_sink_srv_hsp_send_aws_call_info(bt_bd_addr_t *remote_address, bt_sink_srv_aws_mce_call_update_info_t* call_info)
{
#ifdef __MTK_AWS_MCE_ENABLE__
    bt_sink_srv_aws_mce_call_send_call_info(remote_address, call_info);
#endif
}

void bt_sink_srv_hsp_set_hfp_flag(bool enable)
{
    bt_sink_srv_report_id("[CALL][HSP] Enable HFP:%d", 1, enable);
    //bt_hfp_enable_service_record(enable);
}

void* bt_sink_srv_cm_get_hsp_info(uint8_t device_idx)
{
    return (void*)&g_sink_srv_hsp_context[device_idx];
}

void bt_sink_srv_hsp_reset_by_device(void* dev)
{
    for(uint8_t i = 0; i < BT_SINK_SRV_HSP_LINK_NUM; i++) {
        if (g_sink_srv_hsp_context[i].device == dev) {
            g_sink_srv_hsp_context[i].handle = BT_HSP_INVALID_HANDLE;
            g_sink_srv_hsp_context[i].device = NULL;
            g_sink_srv_hsp_context[i].is_used = false;
            bt_sink_srv_memset(&g_sink_srv_hsp_context[i].address, 0x00, sizeof(bt_bd_addr_t));
            break;
        }
    }
    return;
}

bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_get_context_by_dev(void *dev)
{
    bt_sink_srv_hsp_context_t *hsp_context = NULL;
    uint8_t i = 0; 
    for(i = 0; i < BT_SINK_SRV_HSP_LINK_NUM; i++) {
        if (g_sink_srv_hsp_context[i].device == dev) {
            hsp_context = &g_sink_srv_hsp_context[i];
            break;
        }
    }
    bt_sink_srv_report_id("[CALL][HSP]Get context, context[%d] = 0x%0x", 2, i, hsp_context);
    return hsp_context;
}


bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_get_connected_device(void)
{
    uint8_t i = 0;
    for(i = 0; i < BT_SINK_SRV_HSP_LINK_NUM; i++)
    {
        if (bt_sink_srv_call_psd_is_ready(g_sink_srv_hsp_context[i].device)) {
            return &g_sink_srv_hsp_context[i];
        }
    }
    return NULL;
}

bool bt_sink_srv_hsp_check_is_connected(bt_bd_addr_t* addr)
{
    bool result = false;
    bt_sink_srv_hsp_context_t *hsp_context = bt_sink_srv_hsp_get_context_by_address(addr);
    if (hsp_context) {
       result = bt_sink_srv_call_psd_is_ready(hsp_context->device);
    }
    bt_sink_srv_report_id("[CALL][HSP]is connected:%d", 1, result);
    return result;
}

bool bt_sink_srv_hsp_check_is_connected_by_context(bt_sink_srv_hsp_context_t *context)
{
    bool result = false;
    if (context && context->handle && context->device) {
        result = bt_sink_srv_call_psd_is_ready(context->device);
    }
    bt_sink_srv_report_id("[CALL][HSP] context is connected:%d", 1, result);
    return result;
}

bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_get_device_by_handle(uint32_t handle)
{
    uint8_t i = 0;
    for(i = 0; i < BT_SINK_SRV_HSP_LINK_NUM; i++)
    {
        if (g_sink_srv_hsp_context[i].handle == handle) {
            return &g_sink_srv_hsp_context[i];
        }
    }
    return NULL;
}

static void bt_sink_srv_hsp_volume_change_notify(bt_bd_addr_t *address, bt_sink_srv_call_audio_volume_t local_volume)
{
    bt_sink_srv_report_id("[CALL][HSP] volume:%d", 1, local_volume);
    bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));;
    
    if (NULL != event) {
        bt_sink_srv_memcpy((void *)&event->call_volume.address, (void *)address, sizeof(bt_bd_addr_t));
        event->call_volume.current_volume = (uint8_t)local_volume;
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_SPEAKER_VOLUME_CHANGE, event, sizeof(*event));
        bt_sink_srv_memory_free(event);
    }
    bt_sink_srv_aws_mce_call_update_info_t call_info = {0,{0}};
    call_info.mask = BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_VOLUME;
    call_info.data.volume = (uint8_t)local_volume;
    bt_sink_srv_hsp_send_aws_call_info(address, &call_info);
    return;
}

bt_status_t bt_sink_srv_hsp_set_audio_status(uint32_t handle, bt_hsp_audio_status_t status)
{
    bt_sink_srv_report_id("[CALL][HSP] set audio, handle:0x%x, status:0x%x", 2, handle, status);

#if defined(MTK_BT_CM_SUPPORT) && defined(AIR_MULTI_POINT_ENABLE)
    bt_bd_addr_t activated_address = {0};
    bt_sink_srv_hsp_context_t *context = bt_sink_srv_hsp_get_device_by_handle(handle);

    bt_cm_get_connected_devices(
        BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), &activated_address, 1);

    if (context != NULL) {
        if (status == BT_HSP_AUDIO_STATUS_ACTIVE) {
            bt_cm_connect_t connect_request = {{0}};

            connect_request.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
            bt_sink_srv_memcpy(connect_request.address, context->address, sizeof(bt_bd_addr_t));

            bt_status_t connect_status = bt_cm_connect(&connect_request);
            bt_sink_srv_report_id("[CALL][HSP]set audio, switch AWS link status:0x%x", 1, connect_status);
        }
        if ((bt_cm_is_disconnecting_aws_device(context->address)) ||
            (bt_sink_srv_memcmp(activated_address, context->address, sizeof(bt_bd_addr_t)) != 0)) {
            status |= BT_SINK_SRV_HSP_SWITCH_LINK;
        }
    }
#endif

    return bt_hsp_set_audio_status(handle, status);
}

bt_status_t bt_sink_srv_hsp_setup_audio_connection(uint32_t handle, bool connect)
{
    bt_status_t result = bt_hsp_send_command(handle, (const char*)"AT+CKPD=200");
    bt_sink_srv_report_id("[CALL][HSP]set audio, handle:0x%x, connect:%d, result:0x%x", 3, handle, connect, result);
    return result;
}

void bt_sink_srv_hsp_set_highlight_device(bt_sink_srv_hsp_context_t *device)
{
    bt_sink_srv_report_id("[CALL][HSP]highlight:0x%x, new_highlight:0x%x", 2, g_sink_srv_hsp_hightlight_p, device);
    if(g_sink_srv_hsp_hightlight_p != NULL) {
        bt_sink_srv_report_id("[CALL][HSP]highlight esco state = %02x", 1, g_sink_srv_hsp_hightlight_p->sco_state);
    }
    if(g_sink_srv_hsp_hightlight_p == NULL) {
        g_sink_srv_hsp_hightlight_p = device;
    } else if((g_sink_srv_hsp_hightlight_p->sco_state == BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED) &&
                        (g_sink_srv_hsp_hightlight_p != device)) {
    g_sink_srv_hsp_hightlight_p = device;
    }
}

static void bt_sink_srv_hsp_reset_highlight_device(bt_sink_srv_hsp_context_t *device)
{
    bt_sink_srv_hsp_context_t *hightlight = g_sink_srv_hsp_hightlight_p;
    if (hightlight == device) {
         bt_sink_srv_hsp_context_t *conn_dev = bt_sink_srv_hsp_get_connected_device();
         if (conn_dev != device) {
             g_sink_srv_hsp_hightlight_p = conn_dev;
         }
    }
    bt_sink_srv_report_id("[CALL][HSP]reset highlight:0x%x, new_highlight:0x%x", 2, hightlight, g_sink_srv_hsp_hightlight_p);
}

bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_get_highlight_device(void)
{
    bt_sink_srv_report_id("[CALL][HSP]Get highlight:0x%x", 1, g_sink_srv_hsp_hightlight_p);
    return g_sink_srv_hsp_hightlight_p;
}

bool bt_sink_srv_hsp_volume_change_handler(bt_sink_srv_call_audio_volume_act_t vol_act, bool min_max)
{
    bool result = false;
    bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_context_p = bt_sink_srv_hsp_get_highlight_device();

    if (NULL != bt_sink_srv_hsp_context_p) {
        bt_sink_srv_call_audio_volume_t local_volume = bt_sink_srv_call_psd_get_speaker_volume(bt_sink_srv_hsp_context_p->device);
        if (BT_SINK_SRV_CALL_AUDIO_VOL_ACT_UP == vol_act) {
            if (min_max) {
                local_volume = BT_SINK_SRV_CALL_AUDIO_MAX_VOLUME;
            } else {
                if (BT_SINK_SRV_CALL_AUDIO_MAX_VOLUME > local_volume) {
                    local_volume++;
                }
            }
        } else if (BT_SINK_SRV_CALL_AUDIO_VOL_ACT_DOWN == vol_act) {
            if (min_max) {
                local_volume = BT_SINK_SRV_CALL_AUDIO_MIN_VOLUME;
            } else {
                if (BT_SINK_SRV_CALL_AUDIO_MIN_VOLUME < local_volume) {
                    local_volume--;
                }
            }
        }
        
        bt_sink_srv_call_psd_set_speaker_volume(bt_sink_srv_hsp_context_p->device, local_volume);

        bt_bd_addr_t *address_p = &bt_sink_srv_hsp_context_p->address;
        bt_sink_srv_hsp_volume_change_notify(address_p, local_volume);
        bt_sink_srv_hf_stored_data_t stored_data;
        stored_data.speaker_volume = (uint8_t)local_volume | BT_SINK_SRV_HF_VOLUME_MASK;
        bt_sink_srv_hsp_set_nvdm_data(address_p, &stored_data, sizeof(stored_data));
        char hsp_cmd[BT_SINK_SRV_HSP_CMD_LENGTH];
        bt_sink_srv_call_audio_volume_t bt_vol = bt_sink_srv_call_audio_volume_local_to_bt(local_volume);
        snprintf(hsp_cmd, BT_SINK_SRV_HSP_CMD_LENGTH, "AT+VGS=%d", (uint8_t)bt_vol);
        bt_hsp_send_command(bt_sink_srv_hsp_context_p->handle, hsp_cmd);
    }
    return result;
}

void bt_sink_srv_hsp_pseudo_dev_callback(
    bt_sink_srv_call_pseudo_dev_event_t event_id, void *device, void *params)
{
    bt_sink_srv_report_id("[CALL][HSP]PSD_CB, event_id:0x%x, device:0x%x, params:0x%x", 3, event_id, device, params);
    bt_sink_srv_assert(device);
    switch(event_id) {
        case BT_SINK_SRV_CALL_PSD_EVENT_DEINIT: {
            bt_sink_srv_hsp_reset_by_device(device);
            bt_sink_srv_call_psd_free_device(device);
            bt_sink_srv_hsp_set_hfp_flag(true);
        }
        break;

        case BT_SINK_SRV_CALL_PSD_EVENT_GET_SCO_STATE: {
            bt_sink_srv_hsp_context_t *hsp_ctx = bt_sink_srv_hsp_get_context_by_dev(device);
            if (hsp_ctx) {
                bt_sink_srv_sco_connection_state_t *sco_state = (bt_sink_srv_sco_connection_state_t *)params;
                *sco_state = hsp_ctx->sco_state;
            }
        }
        break;

        default:
        break;
    }
}

bt_status_t bt_sink_srv_hsp_connect(bt_bd_addr_t *address)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_p= bt_sink_srv_hsp_alloc_free_context(address);
    if (NULL != bt_sink_srv_hsp_p) {
        status = bt_hsp_connect(&bt_sink_srv_hsp_p->handle, (const bt_bd_addr_t *)address);
        if (status == BT_STATUS_SUCCESS){
            bt_sink_srv_hsp_p->is_used = true;
            bt_sink_srv_memcpy(&bt_sink_srv_hsp_p->address, address, sizeof(bt_bd_addr_t));
            bt_sink_srv_hsp_set_hfp_flag(false);
            bt_sink_srv_hsp_p->device = bt_sink_srv_call_psd_alloc_device(address, bt_sink_srv_hsp_pseudo_dev_callback);
            bt_sink_srv_assert(bt_sink_srv_hsp_p->device);
            bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hsp_p->device, BT_SINK_SRV_CALL_EVENT_CONNECT_LINK_REQ, NULL);
            bt_sink_srv_cm_profile_status_notify(address, BT_SINK_SRV_PROFILE_HSP, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTING, BT_STATUS_SUCCESS);
        } else {
            bt_sink_srv_report_id("[CALL][HSP] Connect hsp connection failed:0x%x", 1, status);
        }
    }
    return status;
}

bt_status_t bt_sink_srv_hsp_disconnect(bt_bd_addr_t *address)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_p= bt_sink_srv_hsp_get_context_by_address(address);
    if (bt_sink_srv_hsp_p && bt_sink_srv_hsp_p->handle) {
        status = bt_hsp_disconnect(bt_sink_srv_hsp_p->handle);
        if (status == BT_STATUS_SUCCESS){
            bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hsp_p->device, BT_SINK_SRV_CALL_EVENT_DISCONNECT_LINK_REQ, NULL);
        } else {
            bt_sink_srv_report_id("[CALL][HSP] Disconnect hsp connection failed:0x%x", 1, status);
        }
    }
    return status;
}

void bt_sink_srv_hsp_button_press(void)
{
    bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_p = bt_sink_srv_hsp_get_highlight_device();
    if (bt_sink_srv_hsp_p) {
        bt_status_t status = bt_hsp_send_command(bt_sink_srv_hsp_p->handle, (const char*)"AT+CKPD=200");
        if (status != BT_STATUS_SUCCESS){
            bt_sink_srv_report_id("[CALL][HSP] Send press event failed:0x%x", 1, status);
        }
    }
}

static bool bt_sink_srv_hsp_get_nvdm_data(bt_bd_addr_t *bt_addr, void *data_p, uint32_t size)
{
    bool result = false;
#ifdef BT_SINK_SRV_HFP_STORAGE_SIZE
    // Warnning: Due to the task stack limite, record should not increase more than 100 bytes
    bt_device_manager_db_remote_profile_info_t record = {{0}};
    if (NULL != bt_addr && NULL != data_p &&
            BT_STATUS_SUCCESS == bt_device_manager_remote_find_profile_info((*bt_addr), &record)) {
        bt_sink_srv_memcpy(data_p, record.hfp_info, size);
        result = true;
    }
#endif
    return result;
}

static bool bt_sink_srv_hsp_set_nvdm_data(bt_bd_addr_t *bt_addr, void *data_p, uint32_t size)
{
    bool result = false;
#ifdef BT_SINK_SRV_HFP_STORAGE_SIZE
    // Warnning: Due to the task stack limite, record should not increase more than 100 bytes
    bt_device_manager_db_remote_profile_info_t record = {{0}};
    if (NULL != bt_addr && NULL != data_p) {
        bt_device_manager_remote_find_profile_info((*bt_addr), &record);
        bt_sink_srv_memcpy(record.hfp_info, data_p, size);
        result = bt_device_manager_remote_update_profile_info((*bt_addr), &record);
    }
#endif
    return result;
}

static void bt_sink_srv_hsp_sco_state_change_notify(bt_sink_srv_hsp_context_t *hsp_context, bt_sink_srv_sco_connection_state_t state)
{
    bt_sink_srv_report_id("[CALL][HSP]sco state:%d", 1, state);

    /* Using stack due to heap fragment */
    bt_sink_srv_sco_state_update_t update = {{0}};
    bt_bd_addr_t *address_p = &hsp_context->address;

    /* Notify to cm the esco state */
#ifndef MTK_BT_CM_SUPPORT
    bt_sink_srv_cm_esco_state_notify(address_p, state);
#endif
    hsp_context->sco_state = state;

    update.state = state;
    update.codec = BT_HFP_CODEC_TYPE_CVSD;
    bt_sink_srv_memcpy(&update.address, address_p, sizeof(bt_bd_addr_t));
    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE, &update, sizeof(bt_sink_srv_sco_state_update_t));
}

static void bt_sink_srv_hsp_ring_ind_notify(bt_sink_srv_hsp_context_t *hsp_context)
{
    bt_sink_srv_report_id("[CALL][HSP]ring ind.", 0);
    bt_bd_addr_t *address_p = &hsp_context->address;
    bt_sink_srv_event_param_t *event = NULL;
    event = bt_sink_srv_memory_alloc(sizeof(*event));
    bt_sink_srv_memcpy(&event->ring_ind.address, address_p, sizeof(*address_p));
    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_RING_IND, event, sizeof(*event));
    bt_sink_srv_memory_free(event);
    return;
}

static void bt_sink_srv_hsp_init_speaker_volume(bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_context_p)
{
    bt_sink_srv_hf_stored_data_t stored_data;
    bt_bd_addr_t *address_p = &bt_sink_srv_hsp_context_p->address;
    //reuse hfp call volume.
    if (bt_sink_srv_hsp_get_nvdm_data(address_p, &stored_data, sizeof(stored_data))) {
        bt_sink_srv_report_id("[CALL][HSP]Real NVDM volume is 0x%02x", 1, stored_data.speaker_volume);
        if (((stored_data.speaker_volume & ~BT_SINK_SRV_HF_VOLUME_MASK) > BT_SINK_SRV_CALL_AUDIO_MAX_VOLUME) ||
            ((stored_data.speaker_volume & BT_SINK_SRV_HF_VOLUME_MASK) != BT_SINK_SRV_HF_VOLUME_MASK)) {
            bt_sink_srv_report_id("[CALL][HSP]Get volume invalid, set default volume.", 0);
            stored_data.speaker_volume = BT_SINK_SRV_CALL_AUDIO_DEFAULT_VOLUME | BT_SINK_SRV_HF_VOLUME_MASK;
            bt_sink_srv_hsp_set_nvdm_data(address_p, &stored_data, sizeof(stored_data));
        }
    } else {
        bt_sink_srv_report_id("[CALL][HSP]Get volume failed, set default volume.", 0);
        stored_data.speaker_volume = BT_SINK_SRV_CALL_AUDIO_DEFAULT_VOLUME | BT_SINK_SRV_HF_VOLUME_MASK;
        bt_sink_srv_hsp_set_nvdm_data(address_p, &stored_data, sizeof(stored_data));
    }
    bt_sink_srv_call_psd_init_speaker_volume(bt_sink_srv_hsp_context_p->device, stored_data.speaker_volume & ~BT_SINK_SRV_HF_VOLUME_MASK);
    return;
}

static void bt_sink_srv_hsp_sync_speaker_gain_ind_handler(bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_context_p, uint8_t volume)
{
    bt_sink_srv_call_audio_volume_t local_volume = bt_sink_srv_call_audio_volume_bt_to_local(volume);
    bt_sink_srv_call_psd_set_speaker_volume(bt_sink_srv_hsp_context_p->device, local_volume);
    bt_bd_addr_t *address_p = &bt_sink_srv_hsp_context_p->address;
    bt_sink_srv_hf_stored_data_t stored_data;
    stored_data.speaker_volume = (uint8_t)local_volume | BT_SINK_SRV_HF_VOLUME_MASK;
    bt_sink_srv_hsp_set_nvdm_data(address_p, &stored_data, sizeof(stored_data));
    bt_sink_srv_hsp_volume_change_notify(address_p, local_volume);
    return;
}

#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
static void bt_sink_srv_hsp_bt_timeout_callback(uint32_t timer_id, uint32_t data)
{
    bt_sink_srv_report_id("[CALL][HSP]timeout callback, timer id:0x%08x", 1, timer_id);
    switch (timer_id) {
        case BT_SINK_SRV_TIMER_ID_HS_WAIT_RING_IND: {
            bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_p = (bt_sink_srv_hsp_context_t *)data;
            bt_sink_srv_assert(bt_sink_srv_hsp_p);
            if (bt_sink_srv_hsp_p->sco_state == BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED) {
                bt_sink_srv_state_t state = bt_sink_srv_get_state();
                if (state == BT_SINK_SRV_STATE_INCOMING) {
                    bt_sink_srv_state_set(BT_SINK_SRV_STATE_CONNECTED);
                    if (bt_sink_srv_hsp_p->flag & BT_SINK_SRV_HSP_FLAG_RINGING) {
                       bt_sink_srv_hsp_p->flag &= ~BT_SINK_SRV_HSP_FLAG_RINGING;
                    }
                }
                bt_bd_addr_t *address_p = &bt_sink_srv_hsp_p->address;
                bt_sink_srv_aws_mce_call_update_info_t call_info;
                call_info.mask = (BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_VOLUME |
                                  BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_STATE |
                                  BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_RING_IND);
                call_info.data.call_state = BT_SINK_SRV_AWS_MCE_CALL_STATE_IDLE;
                call_info.data.volume = (uint8_t)bt_sink_srv_call_psd_get_speaker_volume(bt_sink_srv_hsp_p->device);
                call_info.data.is_ring = 0;
                bt_sink_srv_hsp_send_aws_call_info(address_p, &call_info); 
                bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hsp_p->device, BT_SINK_SRV_CALL_EVENT_CALL_END_IND, NULL);
            }
        }
        break;

        default: {
            bt_sink_srv_report_id("[CALL][HSP] exception timer id:0x%08x", 1, timer_id);
        }
        break;
    }
}
#endif


bt_status_t bt_sink_srv_hsp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_bd_addr_t *address_p = NULL;
    bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_p = NULL;
    bt_sink_srv_report_id("[CALL][HSP] SDK msg:0x%x", 1, msg);
    switch (msg) {

        case BT_HSP_CONNECT_IND: {
            bt_hsp_connect_ind_t *message = (bt_hsp_connect_ind_t *)buffer;
            bt_sink_srv_hsp_p = bt_sink_srv_hsp_alloc_free_context(message->address);
            bt_sink_srv_report_id("[CALL][HSP]CONNECT IND, result:0x%x", 1, status);
            if (NULL != bt_sink_srv_hsp_p) {
                bt_sink_srv_hsp_p->is_used = true;
                bt_sink_srv_memcpy(&bt_sink_srv_hsp_p->address, message->address, sizeof(bt_bd_addr_t));
                bt_sink_srv_hsp_p->handle = message->handle;
                bt_sink_srv_hsp_set_hfp_flag(false);
                bt_sink_srv_hsp_p->device = bt_sink_srv_call_psd_alloc_device(message->address, bt_sink_srv_hsp_pseudo_dev_callback);
                bt_sink_srv_assert(bt_sink_srv_hsp_p->device);
                bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hsp_p->device, BT_SINK_SRV_CALL_EVENT_CONNECT_LINK_REQ_IND, NULL);
                bt_status_t ret = bt_hsp_connect_response(message->handle, true);
                if (ret == BT_STATUS_SUCCESS) {
                    bt_sink_srv_report_id("[CALL][HSP]Accept hsp connection success.", 0);
                } else {
                    bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hsp_p->device, BT_SINK_SRV_CALL_EVENT_LINK_DISCONNECTED, NULL);
                    bt_sink_srv_report_id("[CALL][HSP]Accept hsp connection failed:0x%x", 1, status);
                }
            } else {
                bt_sink_srv_report_id("[CALL][HSP]Can't find the context, addr:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", 6,
                  (*message->address)[5], (*message->address)[4], (*message->address)[3],
                  (*message->address)[2], (*message->address)[1], (*message->address)[0]);
            }
        }
        break;

        case BT_HSP_CONNECT_CNF: {
            bt_hsp_connect_cnf_t *message = (bt_hsp_connect_cnf_t *)buffer;
            bt_sink_srv_report_id("[CALL][HSP]CONNECT CNF, handle:0x%x, status:0x%x", 2, message->handle, status);
            bt_sink_srv_hsp_p = bt_sink_srv_hsp_get_device_by_handle(message->handle);
            if (NULL != bt_sink_srv_hsp_p) {
                address_p = &bt_sink_srv_hsp_p->address;
                if (BT_STATUS_SUCCESS == status) {
                    bt_sink_srv_hsp_set_highlight_device(bt_sink_srv_hsp_p);
                    //Init speaker volume and sync to the remote device.
                    bt_sink_srv_hsp_init_speaker_volume(bt_sink_srv_hsp_p);
                    //Run pesudo state machine
                    bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hsp_p->device, BT_SINK_SRV_CALL_EVENT_LINK_CONNECTED, NULL);
                    //Update hfp call info to partner.
                    bt_sink_srv_aws_mce_call_update_info_t call_info = {0, {0}};
                    call_info.mask = (BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_VOLUME |
                                      BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_RING_IND);
                    call_info.data.volume = bt_sink_srv_call_psd_get_speaker_volume(bt_sink_srv_hsp_p->device);
                    call_info.data.is_ring = 0;
                    bt_sink_srv_hsp_send_aws_call_info(address_p, &call_info);
                    //Notify to Sink cm
                    bt_sink_srv_cm_profile_status_notify(address_p, BT_SINK_SRV_PROFILE_HSP, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED, status); 
                } else {
                    //Notify to Sink cm
                    bt_sink_srv_cm_profile_status_notify(address_p, BT_SINK_SRV_PROFILE_HSP, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED, status);
                    //Run pesudo state machine
                    bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hsp_p->device, BT_SINK_SRV_CALL_EVENT_LINK_DISCONNECTED, NULL);
                }
            }
        }
        break;

        case BT_HSP_DISCONNECTED_IND: {
            bt_hsp_disconnected_ind_t *message = (bt_hsp_disconnected_ind_t *)buffer;
            bt_sink_srv_report_id("[CALL][HSP]DISCONNECTED_IND, handle:0x%x", 1, message->handle);
            bt_sink_srv_hsp_p = bt_sink_srv_hsp_get_device_by_handle(message->handle);
            if (bt_sink_srv_hsp_p) {
                bt_timer_ext_t* timer_ext = bt_timer_ext_find(BT_SINK_SRV_TIMER_ID_HS_WAIT_RING_IND);
                if (timer_ext) {
                    bt_timer_ext_stop(BT_SINK_SRV_TIMER_ID_HS_WAIT_RING_IND);
                }
                bt_bd_addr_t *addr = &bt_sink_srv_hsp_p->address;
                bt_sink_srv_cm_profile_status_notify(addr, BT_SINK_SRV_PROFILE_HSP, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED, status);
                bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hsp_p->device, BT_SINK_SRV_CALL_EVENT_LINK_DISCONNECTED, NULL);
                bt_sink_srv_hsp_reset_highlight_device(bt_sink_srv_hsp_p);
                if((bt_sink_srv_hsp_p->flag & BT_SINK_SRV_HSP_FLAG_RINGING) ||
                            (bt_sink_srv_hsp_p->sco_state == BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED)) {
                bt_sink_srv_state_set(BT_SINK_SRV_STATE_CONNECTED);
                }
                if (bt_sink_srv_hsp_p->flag&BT_SINK_SRV_HSP_FLAG_RINGING) {
                    bt_sink_srv_hsp_p->flag &= ~BT_SINK_SRV_HSP_FLAG_RINGING;
                }
            }
        }
        break;
        
        case BT_HSP_AUDIO_CONNECTED_IND: {
            bt_hsp_audio_connected_ind_t *message = (bt_hsp_audio_connected_ind_t *)buffer;
            bt_sink_srv_hsp_p = bt_sink_srv_hsp_get_device_by_handle(message->handle);
            if (NULL != bt_sink_srv_hsp_p) {
                bt_timer_ext_t* timer_ext = bt_timer_ext_find(BT_SINK_SRV_TIMER_ID_HS_WAIT_RING_IND);
                if (timer_ext) {
                    bt_timer_ext_stop(BT_SINK_SRV_TIMER_ID_HS_WAIT_RING_IND);
                }
                bt_sink_srv_hsp_set_highlight_device(bt_sink_srv_hsp_p);
                bt_sink_srv_hsp_sco_state_change_notify(bt_sink_srv_hsp_p, BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED);
                bt_sink_srv_call_psd_set_codec_type(bt_sink_srv_hsp_p->device, BT_HFP_CODEC_TYPE_CVSD);
                if (bt_sink_srv_hsp_p == bt_sink_srv_hsp_get_highlight_device()) {
                    bt_bd_addr_t *addr = &bt_sink_srv_hsp_p->address;
                    bt_sink_srv_state_set(BT_SINK_SRV_STATE_ACTIVE);
                    if (bt_sink_srv_hsp_p->flag & BT_SINK_SRV_HSP_FLAG_RINGING) {
                        bt_sink_srv_hsp_p->flag &= ~BT_SINK_SRV_HSP_FLAG_RINGING;
                    }
                    bt_sink_srv_aws_mce_call_update_info_t call_info;
                    bt_sink_srv_memset((void *)&call_info, 0, sizeof(bt_sink_srv_aws_mce_call_update_info_t));
                    call_info.mask = BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_ALL;
                    call_info.data.call_state = BT_SINK_SRV_AWS_MCE_CALL_STATE_ACTIVE;
                    call_info.data.volume = (uint8_t)bt_sink_srv_call_psd_get_speaker_volume(bt_sink_srv_hsp_p->device);
                    call_info.data.sco_state = BT_SINK_SRV_AWS_MCE_SCO_STATE_CONNECTED;
                    call_info.data.is_ring = 0;
                    bt_sink_srv_hsp_send_aws_call_info(addr, &call_info); 
                    bt_sink_srv_hsp_set_audio_status(message->handle, BT_HSP_AUDIO_STATUS_ACTIVE);
                #if defined(MTK_BT_CM_SUPPORT) && defined(AIR_MULTI_POINT_ENABLE)
                    bt_gap_connection_handle_t handle = 0;
                    bt_gap_link_policy_setting_t setting = {BT_GAP_LINK_POLICY_DISABLE};
                    for (uint32_t i = 0; i < BT_SINK_SRV_HSP_LINK_NUM; i++) {
                        handle = bt_cm_get_gap_handle(g_sink_srv_hsp_context[i].address);
                        if (handle != 0) {
                            bt_gap_write_link_policy(handle, &setting);
                        }
                    }
                #endif
                } else {
                    bt_sink_srv_report_id("[CALL][HSP]Not the hilglight device, disconnect sco, 0x%x", 1, bt_sink_srv_hsp_p);
                    bt_sink_srv_hsp_setup_audio_connection(bt_sink_srv_hsp_p->handle, false);
                }
            }
        }
        break;

        case BT_HSP_AUDIO_DISCONNECTED_IND: {
            bt_hsp_audio_disconnected_ind_t *message = (bt_hsp_audio_disconnected_ind_t *)buffer;
            bt_sink_srv_hsp_p = bt_sink_srv_hsp_get_device_by_handle(message->handle);
            if (bt_sink_srv_hsp_p) {
                bt_sink_srv_call_psd_set_codec_type(bt_sink_srv_hsp_p->device, BT_HFP_CODEC_TYPE_NONE);
                bt_sink_srv_hsp_sco_state_change_notify(bt_sink_srv_hsp_p, BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED);
                /*For hsp, sco disconnected means call end.*/
                if(bt_sink_srv_hsp_p == bt_sink_srv_hsp_get_highlight_device()) {
                bt_sink_srv_state_set(BT_SINK_SRV_STATE_CONNECTED);
                }
                if (bt_sink_srv_hsp_p->flag & BT_SINK_SRV_HSP_FLAG_RINGING) {
                    bt_sink_srv_hsp_p->flag &= ~BT_SINK_SRV_HSP_FLAG_RINGING;
                }
                bt_sink_srv_aws_mce_call_update_info_t call_info;
                bt_sink_srv_memset((void *)&call_info, 0, sizeof(bt_sink_srv_aws_mce_call_update_info_t));
                call_info.mask = BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_ALL;
                call_info.data.call_state = BT_SINK_SRV_AWS_MCE_CALL_STATE_IDLE;
                call_info.data.volume = (uint8_t)bt_sink_srv_call_psd_get_speaker_volume(bt_sink_srv_hsp_p->device);
                call_info.data.sco_state = BT_SINK_SRV_AWS_MCE_SCO_STATE_DISCONNECTED;
                call_info.data.is_ring = 0;
                address_p = &bt_sink_srv_hsp_p->address;
                bt_sink_srv_hsp_send_aws_call_info(address_p, &call_info); 
                bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hsp_p->device, BT_SINK_SRV_CALL_EVENT_SCO_DISCONNECTED, NULL);
            #if defined(MTK_BT_CM_SUPPORT) && defined(AIR_MULTI_POINT_ENABLE)
                bt_gap_connection_handle_t handle = 0;
                bt_gap_link_policy_setting_t setting = {BT_GAP_LINK_POLICY_ENABLE};
                for (uint32_t i = 0; i < BT_SINK_SRV_HSP_LINK_NUM; i++) {
                    handle = bt_cm_get_gap_handle(g_sink_srv_hsp_context[i].address);
                    if (handle != 0) {
                        bt_gap_write_link_policy(handle, &setting);
                    }
                }
            #endif
            }
        }
        break;
        
        case BT_HSP_RING_IND: {
            bt_hsp_ring_ind_t *ringtone = (bt_hsp_ring_ind_t *)buffer;
            bt_sink_srv_hsp_p = bt_sink_srv_hsp_get_device_by_handle(ringtone->handle);
            if (bt_sink_srv_hsp_p) {
                #ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
                bt_timer_ext_status_t result = BT_TIMER_EXT_STATUS_FAIL;
                bt_timer_ext_t* timer_ext = bt_timer_ext_find(BT_SINK_SRV_TIMER_ID_HS_WAIT_RING_IND);
                if (timer_ext) {
                    bt_timer_ext_stop(BT_SINK_SRV_TIMER_ID_HS_WAIT_RING_IND);
                }
                result = bt_timer_ext_start(
                            BT_SINK_SRV_TIMER_ID_HS_WAIT_RING_IND, 
                            (uint32_t)bt_sink_srv_hsp_p, 
                            BT_SINK_SRV_TIMER_DURATION_HS_WAIT_RING_IND, 
                            bt_sink_srv_hsp_bt_timeout_callback);
                bt_sink_srv_report_id("[CALL][HSP]Time result: 0x%x", 1, result);
                #endif /*MTK_BT_TIMER_EXTERNAL_ENABLE*/
                if (BT_TIMER_EXT_STATUS_SUCCESS == result) {
                    bt_bd_addr_t *address_p = &bt_sink_srv_hsp_p->address;
                    bt_sink_srv_state_set(BT_SINK_SRV_STATE_INCOMING);
                    bt_sink_srv_aws_mce_call_update_info_t call_info;
                    call_info.mask = (BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_STATE |
                                      BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_VOLUME);
                    call_info.data.call_state = BT_SINK_SRV_AWS_MCE_CALL_STATE_INCOMING;
                    call_info.data.volume = (uint8_t)bt_sink_srv_call_psd_get_speaker_volume(bt_sink_srv_hsp_p->device);
                #ifdef BT_SINK_ENABLE_CALL_LOCAL_RINGTONE
                    call_info.mask |= BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_RING_IND;
                    call_info.data.is_ring = 1;
                    bt_sink_srv_hsp_send_aws_call_info(address_p, &call_info);
                    bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hsp_p->device, BT_SINK_SRV_CALL_EVENT_RING_IND, NULL);
                #else/*BT_SINK_ENABLE_CALL_LOCAL_RINGTONE*/
                    if (!(bt_sink_srv_hsp_p->flag & BT_SINK_SRV_HSP_FLAG_RINGING)) {
                        bt_sink_srv_hsp_p->flag |= BT_SINK_SRV_HSP_FLAG_RINGING;
                        bt_sink_srv_hsp_send_aws_call_info(address_p, &call_info);
                        bt_sink_srv_hsp_ring_ind_notify(bt_sink_srv_hsp_p);
                        bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hsp_p->device, BT_SINK_SRV_CALL_EVENT_CALL_START_IND, NULL);
                    }
                #endif/*BT_SINK_ENABLE_CALL_LOCAL_RINGTONE*/
                }
            } else {
                bt_sink_srv_report_id("[CALL][HSP]Can't find the context:0x%x", 1, ringtone->handle);
            }
        }
        break;

        case BT_HSP_VOLUME_SYNC_SPEAKER_GAIN_IND: {
            bt_hsp_volume_sync_speaker_gain_ind_t *message = (bt_hsp_volume_sync_speaker_gain_ind_t *)buffer;
            bt_sink_srv_report_id("[CALL][HSP]volume:%d", 1, message->volume);
            bt_sink_srv_hsp_p = bt_sink_srv_hsp_get_device_by_handle(message->handle);
            if (bt_sink_srv_hsp_p) {
                bt_sink_srv_hsp_sync_speaker_gain_ind_handler(bt_sink_srv_hsp_p, message->volume);
            }
        }
        break;

        case BT_HSP_ACTION_CMD_CNF: {
            //bt_hsp_action_cmd_cnf_t *action_cnf = (bt_hsp_action_cmd_cnf_t *)buffer;
        }
        break;

        case BT_HSP_AUDIO_STATUS_CHANGE_IND: {
            bt_hsp_audio_status_change_ind_t *message = (bt_hsp_audio_status_change_ind_t *)buffer;

            bt_sink_srv_report_id("[CALL][HSP]audio changed, handle:0x%x, state:0x%x", 2, message->handle, message->status);

            bt_sink_srv_hsp_p = bt_sink_srv_hsp_get_device_by_handle(message->handle);

            if (NULL != bt_sink_srv_hsp_p) {
                if (BT_HSP_AUDIO_STATUS_ACTIVE == message->status) {
                    bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hsp_p->device, BT_SINK_SRV_CALL_EVENT_SCO_ACTIVATED, NULL);
                } else {
                    bt_sink_srv_report_id("[CALL][HSP]sco inactived", 0);
                }
            }
        }
        break;

        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_sink_srv_hsp_action_handler(bt_sink_srv_action_t action, void *parameters)
{
    bt_status_t result = BT_STATUS_SUCCESS;

    //bt_sink_srv_report_id("[CALL][HSP] Action:%x", 1, action);

    switch (action) {
#ifndef MTK_BT_CM_SUPPORT
        case BT_SINK_SRV_ACTION_PROFILE_INIT: {
            for(uint8_t i = 0; i < BT_SINK_SRV_HSP_LINK_NUM ; i++){
                bt_sink_srv_memset(g_sink_srv_hsp_context, 0, sizeof(g_sink_srv_hsp_context));
            }
            bt_sink_srv_hsp_set_highlight_device(NULL);
        }
        break;

        case BT_SINK_SRV_ACTION_PROFILE_DEINIT: {
            for(uint8_t i = 0; i < BT_SINK_SRV_HSP_LINK_NUM ; i++){
                bt_sink_srv_memset(g_sink_srv_hsp_context, 0, sizeof(g_sink_srv_hsp_context));
            }
            bt_sink_srv_hsp_set_highlight_device(NULL);
        }
        break;

        case BT_SINK_SRV_ACTION_PROFILE_CONNECT: {
            bt_sink_srv_profile_connection_action_t *action_param = (bt_sink_srv_profile_connection_action_t *)parameters;
            if (action_param->profile_connection_mask & BT_SINK_SRV_PROFILE_HSP) {
                bt_sink_srv_hsp_connect(&(action_param->address));
            }
        }
        break;

        case BT_SINK_SRV_ACTION_PROFILE_DISCONNECT: {
            bt_sink_srv_profile_connection_action_t *action_param = (bt_sink_srv_profile_connection_action_t *)parameters;
            if (action_param->profile_connection_mask & BT_SINK_SRV_PROFILE_HSP) {
               bt_sink_srv_hsp_disconnect(&(action_param->address));
            }
        }
        break;
#endif

        case BT_SINK_SRV_ACTION_ANSWER: {
            bt_sink_srv_hsp_button_press();
        }
        break;

        case BT_SINK_SRV_ACTION_HANG_UP: {
            bt_sink_srv_hsp_button_press();
        }
        break;

        case BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH: {
            bt_sink_srv_hsp_button_press();
        }
        break;
        
        case BT_SINK_SRV_ACTION_DIAL_LAST: {
            bt_sink_srv_hsp_button_press();
        }
        break;
        
        case BT_SINK_SRV_ACTION_CALL_VOLUME_UP: {
            bt_sink_srv_hsp_volume_change_handler(BT_SINK_SRV_CALL_AUDIO_VOL_ACT_UP, false);
        }
        break;

        case BT_SINK_SRV_ACTION_CALL_VOLUME_DOWN: {
            bt_sink_srv_hsp_volume_change_handler(BT_SINK_SRV_CALL_AUDIO_VOL_ACT_DOWN, false);
        }
        break;

        default: {
            bt_sink_srv_report_id("[CALL][HSP]Unexcepted action:0x%x", 1, action);
        }
        break;
    }
    return result;
}

bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_alloc_free_context(bt_bd_addr_t *address)
{
    uint8_t idx = 0;

    for (; idx < BT_SINK_SRV_HSP_LINK_NUM; idx++) {
        if (false == g_sink_srv_hsp_context[idx].is_used) {
            return &g_sink_srv_hsp_context[idx];
        }
    }
    bt_sink_srv_report_id("[CALL][HSP] no free hsp context to use", 0);
    return NULL;
}

bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_get_context_by_address(bt_bd_addr_t *address)
{
    if (address == NULL) {
        return NULL;
    }

    for (uint32_t idx = 0; idx < BT_SINK_SRV_HSP_LINK_NUM; idx++) {
        if (g_sink_srv_hsp_context[idx].is_used &&
            (!bt_sink_srv_memcmp(&g_sink_srv_hsp_context[idx].address, address, sizeof(bt_bd_addr_t)))){
            return &g_sink_srv_hsp_context[idx];
        }
    }

    bt_sink_srv_report_id("[CALL][HSP] not found hsp context", 0);
    return NULL;
}

#ifdef MTK_BT_CM_SUPPORT
bt_status_t  bt_sink_srv_hsp_cm_callback_handler(bt_cm_profile_service_handle_t type, void *data)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    uint8_t *address = NULL;
    bt_sink_srv_report_id("[CALL][HSP] cm_callback_handler type:0x%02x", 1, type);
    switch (type) {
    case BT_CM_PROFILE_SERVICE_HANDLE_POWER_ON:
        bt_sink_srv_call_psd_init();
        for(uint8_t i = 0; i < BT_SINK_SRV_HSP_LINK_NUM ; i++){
            bt_sink_srv_memset(g_sink_srv_hsp_context, 0, sizeof(g_sink_srv_hsp_context));
        }
        bt_sink_srv_hsp_set_highlight_device(NULL);
        bt_sink_srv_aws_mce_call_action_handler(BT_SINK_SRV_ACTION_PROFILE_INIT, NULL);
        break;
    case BT_CM_PROFILE_SERVICE_HANDLE_POWER_OFF:
        bt_sink_srv_call_psd_deinit();
        for(uint8_t i = 0; i < BT_SINK_SRV_HSP_LINK_NUM ; i++){
            bt_sink_srv_memset(g_sink_srv_hsp_context, 0, sizeof(g_sink_srv_hsp_context));
        }
        bt_sink_srv_hsp_set_highlight_device(NULL);
        bt_sink_srv_aws_mce_call_action_handler(BT_SINK_SRV_ACTION_PROFILE_DEINIT, NULL);
        break;
    case BT_CM_PROFILE_SERVICE_HANDLE_CONNECT:
        bt_sink_srv_mutex_lock();
        address = (uint8_t *)data;
        bt_sink_srv_assert(address);
        bt_sink_srv_report_id("[CALL][HSP] connect HSP addr :0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", 6, address[0],address[1],address[2],
            address[3],address[4],address[5]);
        bt_sink_srv_hsp_connect((bt_bd_addr_t *)address);
        bt_sink_srv_mutex_unlock();
        break;
    case BT_CM_PROFILE_SERVICE_HANDLE_DISCONNECT:
        bt_sink_srv_mutex_lock();
        address = (uint8_t *)data;
        bt_sink_srv_assert(address);
        bt_sink_srv_report_id("[CALL][HF] disconnect HFP addr :0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", 6, address[0],address[1],address[2],
            address[3],address[4],address[5]);
        bt_sink_srv_hsp_disconnect((bt_bd_addr_t *)address);
        bt_sink_srv_mutex_unlock();
        break;
    default:
        break;
    }
    return status;
}
#endif
