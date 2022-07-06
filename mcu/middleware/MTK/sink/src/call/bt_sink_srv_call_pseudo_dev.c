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

#include "bt_sink_srv_call_pseudo_dev_mgr.h"
#include "bt_connection_manager_internal.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_aws_mce_call.h"
#ifdef BT_SINK_DUAL_ANT_ENABLE
#include "bt_sink_srv_dual_ant.h"
#endif
bt_sink_srv_call_pseudo_dev_t bt_sink_srv_call_pseudo_dev[BT_SINK_SRV_CALL_PSD_NUM];

static bool bt_sink_srv_call_psd_init_flag = false;
static bt_sink_srv_call_pseudo_dev_t *bt_sink_srv_call_psd_get_dev_by_audio_id(bt_sink_srv_call_audio_id_t audio_id)
{
    bt_sink_srv_call_pseudo_dev_t * dev = NULL;
    uint8_t i = 0;
    for (i = 0; i < BT_SINK_SRV_CALL_PSD_NUM; ++i) {
        if (bt_sink_srv_call_pseudo_dev[i].audio_id == audio_id) {
            dev = &bt_sink_srv_call_pseudo_dev[i];
            break;
        }  
    }
    bt_sink_srv_report_id("[CALL][PSD] get dev, dev[%d] = 0x%0x", 2, i, dev);
    return dev;
}

bool bt_sink_srv_call_psd_is_all_in_steady_state(void)
{
    bool result = true;
    for (uint8_t i = 0; i < BT_SINK_SRV_CALL_PSD_NUM; ++i) {
        if (!bt_sink_srv_call_psd_is_steady(&bt_sink_srv_call_pseudo_dev[i])) {
            result = false;
            break;
        }
    }
    // For connecting state, CM will not allowe to start rho.
    //bt_sink_srv_report_id("[CALL][PSD]Is steady state:%d", 1, result);
    return result;
}

void bt_sink_srv_call_psd_am_callback(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *parm)
{
    bt_sink_srv_report_id("[CALL][PSD]AM_CB, aud_id:%x, msg_id:%x, sub_msg:%x", 3, aud_id, msg_id, sub_msg);
    bt_sink_srv_mutex_lock();
    if (msg_id == AUD_SELF_CMD_REQ) {
        //find out the device by audio id.
        bt_sink_srv_call_pseudo_dev_t* dev = bt_sink_srv_call_psd_get_dev_by_audio_id(aud_id);
        //bt_sink_srv_assert(dev);
        if (NULL == dev) {
            bt_sink_srv_report_id("[CALL][PSD]Can't find the device!", 0);
            bt_sink_srv_mutex_unlock();
            return;
        }
        
        if (sub_msg == AUD_HFP_PLAY_OK) {
            bt_sink_srv_assert(dev->audio_src);
            bt_sink_srv_call_audio_side_tone_enable();
            bt_bd_addr_t address = {0};
            bt_sink_srv_call_psd_convert_devid_to_btaddr(dev->audio_src->dev_id, &address);
            uint32_t gap_handle = (uint32_t) bt_sink_srv_cm_get_gap_handle(&address);
            bt_sink_srv_report_id("[CALL][PSD]get handle: 0x%0x", 1, gap_handle);
            bt_sink_srv_sco_connection_state_t sco_state = BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED;
            dev->user_cb(BT_SINK_SRV_CALL_PSD_EVENT_GET_SCO_STATE, (void *)dev, (void *)&sco_state);
            if (sco_state == BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED && dev->next_state == BT_SINK_SRV_CALL_PSD_NEXT_STATE_INIT) {
                bt_sink_srv_call_audio_init_play(gap_handle);
            } else {
                bt_sink_srv_report_id("[CALL][PSD]No need init sync, sco state:0x%x, dev->next state:0x%x", 2, sco_state, dev->next_state);
            }
            bt_sink_srv_call_psd_state_event_notify(dev, BT_SINK_SRV_CALL_EVENT_PLAY_CODEC_IND, NULL);
#ifdef MTK_AWS_MCE_ENABLE
    #ifdef BT_SINK_DUAL_ANT_ENABLE
            if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
                    bt_sink_srv_dual_ant_data_t notify;
                    notify.type = BT_SINK_DUAL_ANT_TYPE_CALL;
                    notify.call_info.esco_state = true;
                    bt_sink_srv_dual_ant_notify(false, &notify);
            }
    #endif
#else
    #ifdef BT_SINK_DUAL_ANT_ENABLE
            bt_sink_srv_dual_ant_data_t notify;
            notify.type = BT_SINK_DUAL_ANT_TYPE_CALL;
            notify.call_info.esco_state = true;
            bt_sink_srv_dual_ant_notify(false, &notify);
    #endif
#endif
        } else if (sub_msg == AUD_CMD_COMPLETE) {
            bt_sink_srv_assert(dev->audio_src);
            bt_sink_srv_report_id("[CALL][PSD]state:0x%x, sub state:0x%x", 2, dev->audio_src->state, dev->audio_src->substate);
            if (dev->audio_src->substate == BT_SINK_SRV_CALL_PSD_SUB_STATE_CODEC_STARTING) {
                bt_sink_srv_call_psd_state_event_notify(dev, BT_SINK_SRV_CALL_EVENT_PLAY_CODEC_IND, NULL);
#ifdef MTK_AWS_MCE_ENABLE
    #ifdef BT_SINK_DUAL_ANT_ENABLE
            if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
                    bt_sink_srv_dual_ant_data_t notify;
                    notify.type = BT_SINK_DUAL_ANT_TYPE_CALL;
                    notify.call_info.esco_state = true;
                    bt_sink_srv_dual_ant_notify(false, &notify);
            }
    #endif
#else
    #ifdef BT_SINK_DUAL_ANT_ENABLE
            bt_sink_srv_dual_ant_data_t notify;
            notify.type = BT_SINK_DUAL_ANT_TYPE_CALL;
            notify.call_info.esco_state = true;
            bt_sink_srv_dual_ant_notify(false, &notify);
    #endif
#endif
            } else if (dev->audio_src->substate == BT_SINK_SRV_CALL_PSD_SUB_STATE_CODEC_STOPPING){
                bt_sink_srv_call_psd_state_event_notify(dev, BT_SINK_SRV_CALL_EVENT_STOP_CODEC_IND, NULL);
#ifdef MTK_AWS_MCE_ENABLE
    #ifdef BT_SINK_DUAL_ANT_ENABLE
                if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
                        bt_sink_srv_dual_ant_data_t notify;
                        notify.type = BT_SINK_DUAL_ANT_TYPE_CALL;
                        notify.call_info.esco_state = false;
                        bt_sink_srv_dual_ant_notify(false, &notify);
                }
    #endif
#else
    #ifdef BT_SINK_DUAL_ANT_ENABLE
                bt_sink_srv_dual_ant_data_t notify;
                notify.type = BT_SINK_DUAL_ANT_TYPE_CALL;
                notify.call_info.esco_state = false;
                bt_sink_srv_dual_ant_notify(false, &notify);
    #endif
#endif
            }
        }
    }
    bt_sink_srv_mutex_unlock();
}

void bt_sink_srv_call_psd_reset_device(bt_sink_srv_call_pseudo_dev_t* dev)
{
    bt_sink_srv_report_id("[CALL][PSD]Reset dev:0x%x", 1, dev);
    bt_sink_srv_assert(dev);
    dev->audio_id = BT_SINK_SRV_CALL_AUDIO_INVALID_ID;
    dev->audio_type = BT_SINK_SRV_CALL_AUDIO_NONE;
    dev->codec = BT_HFP_CODEC_TYPE_NONE;
    dev->next_state = BT_SINK_SRV_CALL_PSD_NEXT_STATE_INIT;
    dev->spk_vol = 0;
    dev->flag = BT_SINK_SRV_CALL_PSD_FLAG_NONE;
    dev->user_cb = NULL;
}

void bt_sink_srv_call_psd_init(void)
{
    uint8_t i = 0;
    bt_sink_srv_report_id("[CALL][PSD]Init device.  init_flag = %d", 1, bt_sink_srv_call_psd_init_flag);
    if (false == bt_sink_srv_call_psd_init_flag) {
        bt_sink_srv_call_psd_init_flag = true;
        for (i = 0; i < BT_SINK_SRV_CALL_PSD_NUM; ++i) {
            bt_sink_srv_call_pseudo_dev_t *dev = &bt_sink_srv_call_pseudo_dev[i];
            if (dev->audio_src != NULL) {
                bt_sink_srv_report_id("[CALL][PSD]Init device, skip dev 0x%x", 1, dev);
                continue;
            }
            bt_sink_srv_memset((void*)dev, 0, sizeof(bt_sink_srv_call_pseudo_dev_t));
            bt_sink_srv_call_psd_alloc_audio_src(dev);
            bt_sink_srv_call_psd_reset_device(dev);
        }
    }
}

void bt_sink_srv_call_psd_deinit(void)
{
    uint8_t i = 0;
    bt_sink_srv_report_id("[CALL][PSD]Deinit device.  init_flag = %d", 1, bt_sink_srv_call_psd_init_flag);
    if (bt_sink_srv_call_psd_init_flag) {
        bt_sink_srv_call_psd_init_flag = false;
        for (i = 0; i < BT_SINK_SRV_CALL_PSD_NUM; ++i) {
            if (bt_sink_srv_call_pseudo_dev[i].audio_id != BT_SINK_SRV_CALL_AUDIO_INVALID_ID) {
                bt_sink_srv_report_id("[CALL][PSD]Set next state, origin:0x%x->new:0x%x", 2,
                    bt_sink_srv_call_pseudo_dev[i].next_state, BT_SINK_SRV_CALL_PSD_NEXT_STATE_POWER_OFF);
                bt_sink_srv_call_pseudo_dev[i].next_state = BT_SINK_SRV_CALL_PSD_NEXT_STATE_POWER_OFF;
                continue;
            }
            bt_sink_srv_call_psd_free_audio_src(&bt_sink_srv_call_pseudo_dev[i]);
        #if 0
            //error handle if audio id is not close in some case.
            if (bt_sink_srv_call_pseudo_dev[i].audio_id != BT_SINK_SRV_CALL_AUDIO_INVALID_ID) {
                bt_sink_srv_call_audio_codec_close(bt_sink_srv_call_pseudo_dev[i].audio_id);
            }
        #endif
            bt_sink_srv_call_psd_reset_device(&bt_sink_srv_call_pseudo_dev[i]);
        }
    }
}

void* bt_sink_srv_call_psd_alloc_device(bt_bd_addr_t *addr, bt_sink_srv_call_pseudo_dev_callback callback)
{
    uint8_t i = 0;
    bt_sink_srv_call_pseudo_dev_t *dev = NULL;
    bt_sink_srv_assert(callback);
    bt_sink_srv_assert(addr);
    uint64_t dev_id = bt_sink_srv_call_psd_convert_btaddr_to_devid(addr);
    for (i = 0; i < BT_SINK_SRV_CALL_PSD_NUM; ++i) {
        if (callback == bt_sink_srv_call_pseudo_dev[i].user_cb && dev_id == bt_sink_srv_call_pseudo_dev[i].audio_src->dev_id) {
            dev = &bt_sink_srv_call_pseudo_dev[i];
            bt_sink_srv_report_id("[CALL][PSD]Alloc dev, already exist.", 0);
            break;
        } else if (bt_sink_srv_call_pseudo_dev[i].user_cb == NULL && 0 == bt_sink_srv_call_pseudo_dev[i].audio_src->dev_id) {
            dev = &bt_sink_srv_call_pseudo_dev[i];
            bt_sink_srv_call_psd_set_device_id((void*)dev, addr);
            dev->user_cb = callback;
            //open audio id.
            dev->audio_id = bt_sink_srv_call_audio_codec_open(bt_sink_srv_call_psd_am_callback);
            break;
        }
    }

    bt_sink_srv_report_id("[CALL][PSD]Alloc dev, i:%d, dev:0x%x", 2, i, dev);
    return (void*)dev;
}

void bt_sink_srv_call_psd_free_device(void *device)
{
    bt_sink_srv_report_id("[CALL][PSD]Free dev:0x%x", 1, device);
    bt_sink_srv_assert(device);
    bt_sink_srv_call_pseudo_dev_t *dev = (bt_sink_srv_call_pseudo_dev_t *)device;    
    bt_sink_srv_assert(dev->audio_src);
      
    //close audio id.
    bt_sink_srv_call_audio_codec_close(dev->audio_id);

    //reset dev.
    bt_sink_srv_call_psd_reset_device(dev);
}

void bt_sink_srv_call_psd_set_mic_volume(void *device, bt_sink_srv_call_audio_volume_t volume)
{
    bt_sink_srv_assert(device);
    bt_sink_srv_call_pseudo_dev_t *dev = (bt_sink_srv_call_pseudo_dev_t *)device;
    bt_sink_srv_report_id("[CALL][PSD]Set mic volume, device:0x%x, prev:%d, next:%d", 3, device, dev->mic_vol, volume);

    if (dev->mic_vol != volume) {
        bt_sink_srv_report_id("[CALL][PSD]Mic volume change, volume:%d", 1, volume);
        dev->mic_vol = volume;
    }
}

void bt_sink_srv_call_psd_set_speaker_volume(void *device, bt_sink_srv_call_audio_volume_t volume)
{
    bt_sink_srv_assert(device);
    bt_sink_srv_call_pseudo_dev_t *dev = (bt_sink_srv_call_pseudo_dev_t *)device;
    bt_sink_srv_report_id("[CALL][PSD]Set volume, device:0x%x, prev:%d, next:%d", 3, device, dev->spk_vol, volume);
    if (dev->spk_vol != volume) {
        bt_sink_srv_report_id("[CALL][PSD]Volume Change, volume:%d, audio_id:%d, audio_type:%d", 3,  
                             volume, dev->audio_id, dev->audio_type);
        dev->spk_vol = volume;
        if ((dev->audio_type != BT_SINK_SRV_CALL_AUDIO_NONE) && (bt_sink_srv_call_psd_is_playing(dev))) {
            bt_sink_srv_call_audio_set_out_volume(dev->audio_id, volume);
        }
    }
}

void bt_sink_srv_call_psd_set_speaker_start_volume(void *device, bt_sink_srv_call_audio_volume_t volume)
{
#ifdef MTK_AUDIO_SYNC_ENABLE
    bt_sink_srv_call_pseudo_dev_t *dev = (bt_sink_srv_call_pseudo_dev_t *)device;
    bt_sink_srv_report_id("[CALL][PSD]Set start volume, device:0x%x, volume:%d", 2, device, volume);

    if ((dev != NULL) && (dev->audio_type != BT_SINK_SRV_CALL_AUDIO_NONE)) {
        bt_sink_srv_call_audio_set_out_start_volume(dev->audio_id, volume);
    }
#endif
}

bt_sink_srv_call_audio_volume_t bt_sink_srv_call_psd_get_mic_volume(void *device)
{
    bt_sink_srv_assert(device);
    bt_sink_srv_call_pseudo_dev_t *dev = (bt_sink_srv_call_pseudo_dev_t *)device;
    bt_sink_srv_report_id("[CALL][PSD]Get mic volume, device:0x%x, volume:%d", 2, device, dev->mic_vol);
    return dev->mic_vol;
}

bt_sink_srv_call_audio_volume_t bt_sink_srv_call_psd_get_speaker_volume(void *device)
{
    bt_sink_srv_assert(device);
    bt_sink_srv_call_pseudo_dev_t *dev = (bt_sink_srv_call_pseudo_dev_t *)device;
    bt_sink_srv_report_id("[CALL][PSD]Get volume, device:0x%x, volume:%d", 2, device, dev->spk_vol);
    return dev->spk_vol;
}

void bt_sink_srv_call_psd_init_speaker_volume(void *device, bt_sink_srv_call_audio_volume_t volume)
{
    bt_sink_srv_assert(device);
    bt_sink_srv_report_id("[CALL][PSD]Init volume, device:0x%x, volume:%d", 2, device, volume);
    bt_sink_srv_call_pseudo_dev_t *dev = (bt_sink_srv_call_pseudo_dev_t *)device;
    dev->spk_vol = volume;
}

void bt_sink_srv_call_psd_reset_user_callback(void *device, bt_sink_srv_call_pseudo_dev_callback callback)
{
    bt_sink_srv_assert(device);
    bt_sink_srv_call_pseudo_dev_t *dev = (bt_sink_srv_call_pseudo_dev_t *)device;
    bt_sink_srv_report_id("[CALL][PSD]Set user_cb, device:0x%x, prev:0x%x, new:0x%x", 3, dev, dev->user_cb, callback);
    dev->user_cb = callback;
}

void bt_sink_srv_call_psd_set_codec_type(void *device, bt_sink_srv_call_audio_codec_type_t codec)
{
    bt_sink_srv_assert(device);
    bt_sink_srv_call_pseudo_dev_t *dev = (bt_sink_srv_call_pseudo_dev_t *)device;
    bt_sink_srv_report_id("[CALL][PSD]Set codec, device:0x%x, codec:0x%x", 2, device, codec);
    dev->codec = codec;
}

bt_sink_srv_call_audio_codec_type_t bt_sink_srv_call_psd_get_codec_type(void *device)
{
    bt_sink_srv_assert(device);
    bt_sink_srv_call_pseudo_dev_t *dev = (bt_sink_srv_call_pseudo_dev_t *)device;
    bt_sink_srv_report_id("[CALL][PSD]Get codec, device:0x%x, codec:0x%x", 2, device, dev->codec);
    return dev->codec;
}

void bt_sink_srv_call_psd_state_event_notify (void *device, bt_sink_srv_call_state_event_t event, void* data)
{
    bt_sink_srv_mutex_lock();
    bt_sink_srv_call_psd_state_machine((bt_sink_srv_call_pseudo_dev_t*)device, event, data);
    bt_sink_srv_mutex_unlock();
}

