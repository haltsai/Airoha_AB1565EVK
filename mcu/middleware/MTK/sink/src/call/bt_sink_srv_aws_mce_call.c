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

#include "bt_sink_srv.h"
#include "bt_sink_srv_utils.h"
#include "bt_aws_mce_srv.h"
#include "bt_connection_manager_internal.h"
#include "bt_device_manager.h"
#include "bt_sink_srv_hf.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_sink_srv_aws_mce_call.h"
#include "bt_sink_srv_call_pseudo_dev.h"
#include "bt_sink_srv_call_pseudo_dev_mgr.h"
#ifdef MTK_BT_HSP_ENABLE
#include "bt_sink_srv_hsp.h"
#endif
#include "bt_aws_mce_report.h"
#ifdef AIR_LE_AUDIO_ENABLE
#include "bt_sink_srv_le_cap.h"
#include "bt_sink_srv_le_cap_stream.h"
#endif

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
extern void bt_sink_srv_call_role_handover_init(void);
#endif

#define BT_SINK_SRV_AWS_MCE_CALL_CONNECTION_NUM 4

bt_sink_srv_aws_mce_call_context_t bt_sink_srv_mce_call_context[BT_SINK_SRV_AWS_MCE_CALL_CONNECTION_NUM];

void* bt_sink_srv_aws_mce_get_call_module(uint8_t idx)
{
    return &bt_sink_srv_mce_call_context[idx];
}

void bt_sink_srv_aws_mce_call_reset_context_by_dev(void *dev)
{
    for(uint8_t i = 0; i < BT_SINK_SRV_CALL_PSD_NUM; i++) {
        if (bt_sink_srv_mce_call_context[i].device == dev) {
            bt_sink_srv_report_id("[CALL][AWS_MCE]Reset context[%d].dev = 0x%0x", 2, i, dev);
            bt_sink_srv_mce_call_context[i].aws_handle = 0;
            bt_sink_srv_mce_call_context[i].call_info.call_state = BT_SINK_SRV_AWS_MCE_CALL_STATE_IDLE;
            bt_sink_srv_mce_call_context[i].call_info.is_ring = 0;
            bt_sink_srv_mce_call_context[i].call_info.sco_state = BT_SINK_SRV_AWS_MCE_SCO_STATE_DISCONNECTED;
            bt_sink_srv_mce_call_context[i].call_info.volume = 0;
            bt_sink_srv_mce_call_context[i].device = NULL;
            break;
        }
    }
}

bt_sink_srv_aws_mce_call_context_t *bt_sink_srv_aws_mce_call_get_free_context(void)
{
    uint8_t i = 0;
    bt_sink_srv_aws_mce_call_context_t *context = NULL;

    for (; i < BT_SINK_SRV_AWS_MCE_CALL_CONNECTION_NUM; i++)
    {
        if (bt_sink_srv_mce_call_context[i].aws_handle == 0)
        {
            context = &bt_sink_srv_mce_call_context[i];
            break;
        }
    }

    bt_sink_srv_report_id("[CALL][AWS_MCE]get_free_context context:%d", 1, i);
    return context;
}

bt_sink_srv_aws_mce_call_context_t *bt_sink_srv_aws_mce_call_get_context_by_handle(uint32_t aws_handle)
{
    uint8_t i = 0;
    bt_sink_srv_aws_mce_call_context_t *context = NULL;

    for (; i < BT_SINK_SRV_AWS_MCE_CALL_CONNECTION_NUM; i++)
    {
        if (bt_sink_srv_mce_call_context[i].aws_handle == aws_handle)
        {
            context = &bt_sink_srv_mce_call_context[i];
            break;
        }
    }

    bt_sink_srv_report_id("[CALL][AWS_MCE]get_context_by_handle handle:0x%x, context:%d", 2, aws_handle, i);
    return context;
}

bt_sink_srv_aws_mce_call_context_t *bt_sink_srv_aws_mce_call_get_context_by_address(const bt_bd_addr_t *address)
{
    uint8_t i = 0;
    bt_sink_srv_aws_mce_call_context_t *context = NULL;

    for (; i < BT_SINK_SRV_AWS_MCE_CALL_CONNECTION_NUM; i++)
    {
        if ((bt_sink_srv_mce_call_context[i].aws_handle != 0) &&
            (bt_sink_srv_memcmp(bt_aws_mce_get_bd_addr_by_handle(bt_sink_srv_mce_call_context[i].aws_handle), address, sizeof(bt_bd_addr_t)) == 0))
        {
            context = &bt_sink_srv_mce_call_context[i];
            break;
        }
    }

    bt_sink_srv_report_id("[CALL][AWS_MCE]get_context_by_address address:0x%x:%x:%x:%x:%x:%x, context:%d", 7,
        (*address)[0], (*address)[1], (*address)[2], (*address)[3], (*address)[4], (*address)[5], i);
    return context;
}

bt_sink_srv_aws_mce_call_context_t *bt_sink_srv_aws_mce_call_get_context_by_dev(void *dev)
{
    uint8_t i = 0;
    bt_sink_srv_aws_mce_call_context_t *aws_call_context = NULL;

    for(i = 0; i < BT_SINK_SRV_CALL_PSD_NUM; i++) {
        if (bt_sink_srv_mce_call_context[i].device == dev) {
            aws_call_context = &bt_sink_srv_mce_call_context[i];
            break;
        }
    }

    bt_sink_srv_report_id("[CALL][AWS_MCE]Get context, context[%d] = 0x%0x", 2, i, aws_call_context);
    return aws_call_context;
}

bt_sink_srv_aws_mce_call_context_t *bt_sink_srv_aws_mce_call_get_context_by_sco_state(bt_sink_srv_aws_mce_sco_state state)
{
    bt_sink_srv_aws_mce_call_context_t *context = NULL;

    for (uint8_t i = 0; i < BT_SINK_SRV_CALL_PSD_NUM; i++) {
        if (bt_sink_srv_mce_call_context[i].call_info.sco_state == state) {
            context = &bt_sink_srv_mce_call_context[i];
        }
    }

    return context;
}

uint32_t bt_sink_srv_aws_mce_call_get_speaker_volume(bt_sink_srv_aws_mce_call_context_t *context)
{
    if (context == NULL) {
        return 0xFFFFFFFF;
    } else {
        return (uint32_t)context->call_info.volume;
    }
}

static void bt_sink_srv_aws_mce_call_sco_state_change_notify(uint32_t aws_handle, bt_sink_srv_sco_connection_state_t state, uint8_t codec)
{
    bt_sink_srv_report_id("[CALL][AWS_MCE]codec:0x%x, sco state:%d", 2, codec, state);

    /* Using stack due to heap fragment */
    bt_sink_srv_sco_state_update_t update = {{0}};
    bt_bd_addr_t *address = (bt_bd_addr_t *)bt_aws_mce_get_bd_addr_by_handle(aws_handle);

    update.state = state;
    update.codec = codec;
    bt_sink_srv_memcpy(&update.address, address, sizeof(bt_bd_addr_t));
    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE, &update, sizeof(bt_sink_srv_sco_state_update_t));
}

static void bt_sink_srv_aws_mce_call_volume_change_notify(bt_bd_addr_t * address, uint8_t volume)
{
    bt_sink_srv_report_id("[CALL][AWS_MCE]Volume Change:%d", 1, volume);
    bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));

    if (NULL != event) {
        bt_sink_srv_memcpy((void *)&event->call_volume.address, (void *)address, sizeof(bt_bd_addr_t));
        event->call_volume.current_volume = volume;
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_SPEAKER_VOLUME_CHANGE, event, sizeof(*event));
        bt_sink_srv_memory_free(event);
    }
}

bt_sink_srv_aws_mce_call_state_t bt_sink_srv_aws_mce_call_transfer_hf_call_state(bt_sink_srv_state_t hf_call)
{
    bt_sink_srv_aws_mce_call_state_t aws_call = BT_SINK_SRV_AWS_MCE_CALL_STATE_IDLE;

    switch (hf_call) {
            case BT_SINK_SRV_STATE_INCOMING: {
                aws_call = BT_SINK_SRV_AWS_MCE_CALL_STATE_INCOMING;
            }
            break;

            case  BT_SINK_SRV_STATE_OUTGOING: {
                 aws_call = BT_SINK_SRV_AWS_MCE_CALL_STATE_OUTGOING;
            }
            break;

            case BT_SINK_SRV_STATE_ACTIVE: {
                 aws_call = BT_SINK_SRV_AWS_MCE_CALL_STATE_ACTIVE;
            }
            break;

            case BT_SINK_SRV_STATE_TWC_INCOMING: {
                 aws_call = BT_SINK_SRV_AWS_MCE_CALL_STATE_TWC_INCOMING;
            }
            break;

            case BT_SINK_SRV_STATE_TWC_OUTGOING: {
                 aws_call = BT_SINK_SRV_AWS_MCE_CALL_STATE_TWC_OUTGOING;
            }
            break;

            case BT_SINK_SRV_STATE_HELD_ACTIVE: {
                 aws_call = BT_SINK_SRV_AWS_MCE_CALL_STATE_HELD_ACTIVE;
            }
            break;

            case BT_SINK_SRV_STATE_HELD_REMAINING: {
                 aws_call = BT_SINK_SRV_AWS_MCE_CALL_STATE_HELD_REMAINING;
            }
            break;

            case BT_SINK_SRV_STATE_MULTIPARTY: {
                 aws_call = BT_SINK_SRV_AWS_MCE_CALL_STATE_MULTIPARTY;
            }
            break;

            default: {
                bt_sink_srv_report_id("[CALL][AWS_MCE]exception hf call state: 0x%x", 1, hf_call);
            }
            break;
        }
    return aws_call;
}

bt_sink_srv_hf_call_state_t bt_sink_srv_aws_mce_call_transfer_aws_call_state(bt_sink_srv_aws_mce_call_state_t state)
{
    bt_sink_srv_hf_call_state_t hf_call = BT_SINK_SRV_HF_CALL_STATE_IDLE;

    switch (state) {
            case BT_SINK_SRV_AWS_MCE_CALL_STATE_IDLE: {
                hf_call = BT_SINK_SRV_HF_CALL_STATE_IDLE;
            }
            break;

            case BT_SINK_SRV_AWS_MCE_CALL_STATE_INCOMING: {
                hf_call = BT_SINK_SRV_STATE_INCOMING;
            }
            break;

            case  BT_SINK_SRV_AWS_MCE_CALL_STATE_OUTGOING: {
                 hf_call = BT_SINK_SRV_STATE_OUTGOING;
            }
            break;

            case BT_SINK_SRV_AWS_MCE_CALL_STATE_ACTIVE: {
                 hf_call = BT_SINK_SRV_STATE_ACTIVE;
            }
            break;

            case BT_SINK_SRV_AWS_MCE_CALL_STATE_TWC_INCOMING: {
                 hf_call = BT_SINK_SRV_STATE_TWC_INCOMING;
            }
            break;

            case BT_SINK_SRV_AWS_MCE_CALL_STATE_TWC_OUTGOING: {
                 hf_call = BT_SINK_SRV_STATE_TWC_OUTGOING;
            }
            break;

            case BT_SINK_SRV_AWS_MCE_CALL_STATE_HELD_ACTIVE: {
                 hf_call = BT_SINK_SRV_STATE_HELD_ACTIVE;
            }
            break;

            case BT_SINK_SRV_AWS_MCE_CALL_STATE_HELD_REMAINING: {
                 hf_call = BT_SINK_SRV_STATE_HELD_REMAINING;
            }
            break;

            case BT_SINK_SRV_AWS_MCE_CALL_STATE_MULTIPARTY: {
                 hf_call = BT_SINK_SRV_STATE_MULTIPARTY;
            }
            break;

            default: {
                bt_sink_srv_report_id("[CALL][AWS_MCE]exception aws call state: 0x%x", 1, state);
            }
            break;
        }

    return hf_call;
}

#ifdef MTK_BT_SPEAKER_ENABLE
void bt_sink_srv_aws_mce_call_info_retry_handler(uint32_t timer_id, uint32_t user_data)
{
    bt_aws_mce_agent_state_type_t state = BT_AWS_MCE_AGENT_STATE_NONE;
    bt_sink_srv_aws_mce_call_context_t *context = (bt_sink_srv_aws_mce_call_context_t *)user_data;

    if (context == NULL) {
        return;
    }

    state = bt_sink_srv_aws_mce_get_aws_state_by_handle(context->aws_handle);
    bt_sink_srv_report_id("[CALL][AWS_MCE]info retry handler, retry counter:%d", 1, context->info_retry_counter);

    if ((state == BT_AWS_MCE_AGENT_STATE_CONNECTABLE) &&
        (bt_aws_mce_srv_get_mode() == BT_AWS_MCE_SRV_MODE_BROADCAST)) {
        bt_aws_mce_information_t info = {
            .type = BT_AWS_MCE_INFORMATION_SCO,
            .data_length = sizeof(bt_sink_srv_aws_mce_call_info_t),
            .data = (uint8_t *)&context->call_info
        };

        context->info_retry_counter--;
        bt_aws_mce_send_information(context->aws_handle, (const bt_aws_mce_information_t *)&info, true);
    } else {
        context->info_retry_counter = 0;
    }

    if (context->info_retry_counter > 0) {
        bt_timer_ext_start(BT_SINK_SRV_CALL_INFO_RETRY_TIMER_ID,
                           user_data,
                           BT_SINK_SRV_AWS_MCE_CALL_INFO_RETRY_TIMEOUT,
                           bt_sink_srv_aws_mce_call_info_retry_handler);
    }
}
#endif

void bt_sink_srv_aws_mce_call_send_call_info(bt_bd_addr_t *remote_address, bt_sink_srv_aws_mce_call_update_info_t* call_info)
{
    bt_sink_srv_assert(call_info);
    bt_sink_srv_report_id("[CALL][AWS_MCE]update call info, mask:0x%02x", 1, call_info->mask);
    bt_sink_srv_aws_mce_call_context_t *aws_call_cntx = bt_sink_srv_aws_mce_call_get_context_by_address((const bt_bd_addr_t *)remote_address);

    if (aws_call_cntx) {
        bt_aws_mce_agent_state_type_t state = bt_sink_srv_aws_mce_get_aws_state_by_handle(aws_call_cntx->aws_handle);

        if (call_info->mask & BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_STATE) {
    #if defined (AIR_LE_AUDIO_ENABLE) && defined (AIR_LE_AUDIO_CIS_ENABLE)
            if ((BT_SINK_SRV_AWS_MCE_CALL_STATE_IDLE == call_info->data.call_state && BT_SINK_SRV_AWS_MCE_CALL_STATE_IDLE != aws_call_cntx->call_info.call_state) ||
                (BT_SINK_SRV_AWS_MCE_CALL_STATE_IDLE != call_info->data.call_state && BT_SINK_SRV_AWS_MCE_CALL_STATE_IDLE == aws_call_cntx->call_info.call_state)) {
                bt_handle_t le_handle = bt_sink_srv_cap_get_link_handle(0xFF);
                if (BT_HANDLE_INVALID != le_handle) {
                    bt_sink_srv_cap_update_connection_interval(le_handle, 0x30);
                }
            }
    #endif
            aws_call_cntx->call_info.call_state = call_info->data.call_state;
        }
    #ifndef MTK_BT_SPEAKER_ENABLE
        if (call_info->mask & BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_SCO_STATUS) {
            aws_call_cntx->call_info.sco_state = call_info->data.sco_state;
        }
    #endif /*MTK_BT_SPEAKER_ENABLE*/
        if (call_info->mask & BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_VOLUME) {
            aws_call_cntx->call_info.volume = call_info->data.volume;
        }
        if (call_info->mask & BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_RING_IND) {
            aws_call_cntx->call_info.is_ring = call_info->data.is_ring;
        }

        if ((state & BT_AWS_MCE_AGENT_STATE_ATTACHED) == BT_AWS_MCE_AGENT_STATE_ATTACHED
    #ifdef MTK_BT_SPEAKER_ENABLE
            || (state == BT_AWS_MCE_AGENT_STATE_CONNECTABLE && BT_AWS_MCE_SRV_MODE_BROADCAST == bt_aws_mce_srv_get_mode())
    #endif /*MTK_BT_SPEAKER_ENABLE*/
        ) {
            bt_aws_mce_information_t send_sco_info = {
                .type = BT_AWS_MCE_INFORMATION_SCO,
                .data_length = sizeof(bt_sink_srv_aws_mce_call_info_t),
                .data = (uint8_t*)&aws_call_cntx->call_info
            };
            bt_sink_srv_report_id("[CALL][AWS_MCE]call info:%d,%d,%d,%d", 4,
                    aws_call_cntx->call_info.call_state,
                    aws_call_cntx->call_info.sco_state,
                    aws_call_cntx->call_info.volume,
                    aws_call_cntx->call_info.is_ring);
            bt_aws_mce_send_information(aws_call_cntx->aws_handle, (const bt_aws_mce_information_t *)&send_sco_info, true);
        }

    #ifdef MTK_BT_SPEAKER_ENABLE
        if ((state == BT_AWS_MCE_AGENT_STATE_CONNECTABLE) &&
            (BT_AWS_MCE_SRV_MODE_BROADCAST == bt_aws_mce_srv_get_mode())) {
            /* Set retry counter on broadcast mode. */
            aws_call_cntx->info_retry_counter = BT_SINK_SRV_AWS_MCE_CALL_INFO_RETRY_COUNT;

            /* Start timer to retry. */
            if (bt_timer_ext_find(BT_SINK_SRV_CALL_INFO_RETRY_TIMER_ID) == NULL) {
                bt_timer_ext_start(BT_SINK_SRV_CALL_INFO_RETRY_TIMER_ID,
                                   (uint32_t)aws_call_cntx,
                                   BT_SINK_SRV_AWS_MCE_CALL_INFO_RETRY_TIMEOUT,
                                   bt_sink_srv_aws_mce_call_info_retry_handler);
            }
        }
    #endif
    } else {
        bt_sink_srv_report_id("[CALL][AWS_MCE]Can't find context, addr:%02x:%02x:%02x:%02x:%02x:%02x", 6,
                        (*remote_address)[5], (*remote_address)[4],(*remote_address)[3],
                        (*remote_address)[2], (*remote_address)[1],(*remote_address)[0]);
    }
}

static void bt_sink_srv_aws_mce_call_parse_call_info(uint32_t handle, uint8_t *data,uint16_t length)
{
    bt_sink_srv_assert(length >= BT_SINK_SRV_AWS_MCE_CALL_INFO_LENGTH);
    bt_sink_srv_aws_mce_call_info_t *call_info = (bt_sink_srv_aws_mce_call_info_t *)data;
    bt_sink_srv_aws_mce_call_context_t *call_context = bt_sink_srv_aws_mce_call_get_context_by_handle(handle);

    if (call_context) {
        bt_sink_srv_report_id("[CALL][AWS_MCE]Call volume, prev:0x%x, new:0x%x", 2, call_context->call_info.volume, call_info->volume);

        if (call_context->call_info.volume != call_info->volume) {
            call_context->call_info.volume = call_info->volume;
            bt_bd_addr_t *address_p = (bt_bd_addr_t *)bt_aws_mce_get_bd_addr_by_handle(call_context->aws_handle);
            bt_sink_srv_aws_mce_call_volume_change_notify(address_p, call_context->call_info.volume);
            bt_sink_srv_call_psd_set_speaker_volume(call_context->device, (bt_sink_srv_call_audio_volume_t)call_context->call_info.volume);
        #ifdef MTK_AUDIO_SYNC_ENABLE
            if (((call_info->sco_state == BT_SINK_SRV_AWS_MCE_SCO_STATE_CONNECTED) &&
                (call_context->call_info.sco_state == BT_SINK_SRV_AWS_MCE_SCO_STATE_DISCONNECTED)) ||
                (bt_sink_srv_call_psd_is_playing_codec(call_context->device))) {
                bt_sink_srv_call_psd_set_speaker_start_volume(call_context->device, call_context->call_info.volume);
            }
        #endif
        }

        if (call_context->call_info.call_state != call_info->call_state) {
            bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
            bt_sink_srv_report_id("[CALL][AWS_MCE]Call state change, prev:0x%x, new:0x%x", 2, call_context->call_info.call_state, call_info->call_state);
            if (call_info->call_state == BT_SINK_SRV_AWS_MCE_CALL_STATE_IDLE) {
                /* add for partner BLe advertising feature, to start BLE adv. */
                if (role == BT_AWS_MCE_ROLE_CLINET || role == BT_AWS_MCE_ROLE_PARTNER) {
                    /* change adv type */
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_LE_START_CONNECTABLE_ADV, NULL, 0);
                    bt_sink_srv_report_id("[CALL][AWS_MCE]start BLE CONN-ADV", 0);
                }

                bt_sink_srv_call_psd_state_event_notify(call_context->device, BT_SINK_SRV_CALL_EVENT_CALL_END_IND, NULL);
            } else if (call_context->call_info.call_state == BT_SINK_SRV_AWS_MCE_CALL_STATE_IDLE) {
                /* add for partner BLe advertising feature, to disconnect BLE connection and start nonconn-adv.*/
                if (role == BT_AWS_MCE_ROLE_CLINET || role == BT_AWS_MCE_ROLE_PARTNER) {
                    /* disconnect BLE*/
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_LE_DISCONNECT, NULL, 0);
                    bt_sink_srv_report_id("[CALL][AWS_MCE]disconnect BLE", 0);
                }
            #if defined (AIR_LE_AUDIO_ENABLE) && defined (AIR_LE_AUDIO_CIS_ENABLE)
                bt_handle_t le_handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established();
                if (le_handle != BT_HANDLE_INVALID) {
                    bool resume = (BLE_MCS_MEDIA_STATE_PLAYING == bt_le_audio_sink_media_get_state(le_handle, BLE_MCP_GMCS_INDEX) ? true : false);
                    bt_sink_srv_cap_stream_release_autonomously(le_handle, 0xFF, resume, 0);
                }
            #endif
                bt_sink_srv_call_psd_state_event_notify(call_context->device, BT_SINK_SRV_CALL_EVENT_CALL_START_IND, NULL);

            #ifdef MTK_BT_CM_SUPPORT
                /* IoT case: using one SCO on two call indicators, active SCO again. */
                bt_sink_srv_report_id("[CALL][AWS_MCE]Call start, sco_state:0x%x", 1, call_context->call_info.sco_state);

                if ((!bt_sink_srv_call_psd_is_playing(call_context->device)) &&
                    (call_context->call_info.sco_state != BT_SINK_SRV_AWS_MCE_SCO_STATE_DISCONNECTED) &&
                    (bt_sink_srv_call_psd_get_codec_type(call_context->device) != BT_HFP_CODEC_TYPE_NONE)) {
                    bt_sink_srv_call_psd_state_event_notify(call_context->device, BT_SINK_SRV_CALL_EVENT_SCO_ACTIVATED, NULL);
                }
            #endif
            }
            call_context->call_info.call_state = call_info->call_state;
            bt_sink_srv_hf_call_state_t hf_call_state = bt_sink_srv_aws_mce_call_transfer_aws_call_state(call_info->call_state);
            if (BT_SINK_SRV_HF_CALL_STATE_IDLE == hf_call_state) {
                bt_sink_srv_state_set(BT_SINK_SRV_STATE_CONNECTED);
            } else {
                bt_sink_srv_state_set(hf_call_state);
            }
        }

        //Only saved the sco state for N9 used after RHO.
        call_context->call_info.sco_state = call_info->sco_state;

        if (call_info->is_ring && call_context->call_info.call_state <= BT_SINK_SRV_AWS_MCE_CALL_STATE_INCOMING) {
            call_context->call_info.is_ring = call_info->is_ring;
            bt_sink_srv_call_psd_state_event_notify(call_context->device, BT_SINK_SRV_CALL_EVENT_RING_IND, NULL);
        }

        if (call_info->is_ring == 0 && call_context->call_info.is_ring == 1) {
            call_context->call_info.is_ring = 0;
            /*Audio path default output to AG, it should stop ringtone when call is active.*/
            if (call_info->call_state == BT_SINK_SRV_AWS_MCE_CALL_STATE_ACTIVE) {
                bt_sink_srv_call_psd_state_event_notify(call_context->device, BT_SINK_SRV_CALL_EVENT_STOP_RING, NULL);
            }
        }
    }
}

static uint8_t bt_sink_srv_aws_mce_call_get_parameter_length(bt_sink_srv_action_t action, void *param)
{
    switch (action)
    {
        case BT_SINK_SRV_ACTION_HF_ECNR_ACTIVATE:
        case BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE:
        {
            return sizeof(bool);
        }

        case BT_SINK_SRV_ACTION_3WAY_RELEASE_SPECIAL:
        case BT_SINK_SRV_ACTION_3WAY_HOLD_SPECIAL:
        case BT_SINK_SRV_ACTION_CALL_SET_VOLUME:
        case BT_SINK_SRV_ACTION_REPORT_BATTERY:
        {
            return sizeof(uint8_t);
        }

        case BT_SINK_SRV_ACTION_QUERY_CALL_LIST:
        {
            return sizeof(bt_bd_addr_t);
        }

        case BT_SINK_SRV_ACTION_DTMF:
        {
            return sizeof(bt_sink_srv_send_dtmf_t);
        }

        case BT_SINK_SRV_ACTION_DIAL_NUMBER:
        {
            return (uint8_t)bt_sink_srv_strlen((char *)param);
        }

        default:
        {
            return 0;
        }
    }
}

static void bt_sink_srv_aws_mce_call_receive_action(uint8_t *data, uint16_t data_len)
{
    bt_sink_srv_assert((data != NULL) && (data_len >= sizeof(bt_sink_srv_action_t)));
    bt_sink_srv_aws_mce_call_action_t *action = (bt_sink_srv_aws_mce_call_action_t *)data;
    bt_sink_srv_send_action(action->action, action->action_param);
}

static void bt_sink_srv_aws_mce_call_dispatch_call_packet(
    bt_sink_srv_aws_mce_call_packet_type_t type, uint8_t *data, uint16_t length)
{
    bt_sink_srv_report_id("[CALL][AWS_MCE]type:0x%x", 1, type);

    switch (type) {
        case BT_SINK_SRV_AWS_MCE_CALL_PACKET_ACTION: {
            bt_sink_srv_aws_mce_call_receive_action(data, length);
        }
        break;

        default:{
            bt_sink_srv_report_id("[CALL][AWS_MCE]Excepted type: 0x%x", 1, type);
        }
        break;
    }
}

static void bt_sink_srv_aws_mce_call_send_action(bt_sink_srv_action_t action, void *param)
{
    bt_sink_srv_aws_mce_call_context_t *call_cntx = NULL;
    uint8_t param_length = bt_sink_srv_aws_mce_call_get_parameter_length(action, param);
    bt_sink_srv_aws_mce_call_action_t *buf = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_aws_mce_call_action_t) + param_length);
    bt_sink_srv_report_id("[CALL][AWS_MCE]send action, action:0x%04x buffer:0x%x", 2, action, buf);

    for (uint8_t i = 0; i < BT_SINK_SRV_AWS_MCE_CALL_CONNECTION_NUM; ++i) {
        if (bt_sink_srv_mce_call_context[i].aws_handle != BT_AWS_MCE_INVALID_HANDLE) {
            bt_bd_addr_t *addr = (bt_bd_addr_t *)bt_aws_mce_get_bd_addr_by_handle(bt_sink_srv_mce_call_context[i].aws_handle);
            bt_sink_srv_assert(addr);
            bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
            if (0 != bt_sink_srv_memcmp((const void *)addr, (const void *)local_addr, sizeof(bt_bd_addr_t))) {
                call_cntx = &bt_sink_srv_mce_call_context[i];
                bt_sink_srv_report_id("[CALL][AWS_MCE]send action, context:0x%x handle:0x%x", 2, call_cntx, call_cntx->aws_handle);
                break;
            }
        }
    }

    if (call_cntx && buf) {
        bt_aws_mce_report_info_t app_report = {0};

        buf->action = action;
        buf->packet_type = BT_SINK_SRV_AWS_MCE_CALL_PACKET_ACTION;

        if (param_length > 0) {
            bt_sink_srv_memcpy(buf->action_param, param, param_length);
        }

        app_report.module_id = BT_AWS_MCE_REPORT_MODULE_SINK_CALL;
        app_report.param = (void*)buf;
        app_report.param_len = sizeof(bt_sink_srv_aws_mce_call_action_t) + param_length;
        bt_aws_mce_report_send_event(&app_report);
    }

    if (buf != NULL) {
        bt_sink_srv_memory_free(buf);
    }
}

void bt_sink_srv_aws_mce_call_pseudo_dev_callback(
    bt_sink_srv_call_pseudo_dev_event_t event_id, void *device, void *params)
{
    bt_sink_srv_assert(device);
    bt_sink_srv_report_id("[CALL][AWS_MCE]PSD_CB, event_id:0x%x, device:0x%x, params:0x%x", 3, event_id, device, params);

    switch(event_id) {
        case BT_SINK_SRV_CALL_PSD_EVENT_GET_CALL_STATE: {
            bt_sink_srv_aws_mce_call_context_t *call_context = NULL;
            call_context = bt_sink_srv_aws_mce_call_get_context_by_dev(device);
            if (call_context) {
                bt_sink_srv_hf_call_state_t *call_state = (bt_sink_srv_hf_call_state_t *)params;
                *call_state = bt_sink_srv_aws_mce_call_transfer_aws_call_state(call_context->call_info.call_state);
            }
        }
        break;

        case BT_SINK_SRV_CALL_PSD_EVENT_GET_SCO_STATE: {
            bt_sink_srv_aws_mce_call_context_t *call_context = NULL;
            call_context = bt_sink_srv_aws_mce_call_get_context_by_dev(device);
            if (call_context) {
                bt_sink_srv_sco_connection_state_t *sco_state = (bt_sink_srv_sco_connection_state_t *)params;
                if (bt_sink_srv_call_psd_get_codec_type(device) != BT_HFP_CODEC_TYPE_NONE) {
                    *sco_state = BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED;
                } else {
                    *sco_state = BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED;
                }
            }
        }
        break;

        case BT_SINK_SRV_CALL_PSD_EVENT_SUSPEND: {
            bt_sink_srv_aws_mce_call_context_t *call_context = NULL;
            call_context = bt_sink_srv_aws_mce_call_get_context_by_dev(device);
            if (call_context) {
                if (bt_sink_srv_call_psd_get_codec_type(device) != BT_HFP_CODEC_TYPE_NONE) {
                    call_context->flag |= BT_SINK_SRV_AWS_MCE_CALL_FLAG_SUSPEND;
                } else {
                    bt_sink_srv_call_psd_state_event_notify(device, BT_SINK_SRV_CALL_EVENT_SUSPEND_REQ, NULL);
                }
            }
        }
        break;

        case BT_SINK_SRV_CALL_PSD_EVENT_IS_SCO_ACTIVATED: {
            bool *is_activated = (bool *)params;
            bt_sink_srv_aws_mce_call_context_t *call_context = bt_sink_srv_aws_mce_call_get_context_by_dev(device);
            if (call_context != NULL) {
                if (bt_sink_srv_call_psd_get_codec_type(device) != BT_HFP_CODEC_TYPE_NONE) {
                    *is_activated = true;
                } else {
                    *is_activated = false;
                }
            }
        }
        break;

        case BT_SINK_SRV_CALL_PSD_EVENT_DEINIT: {
            bt_sink_srv_aws_mce_call_reset_context_by_dev(device);
            bt_sink_srv_call_psd_free_device(device);
        }
        break;

        default:
        break;
    }
}

bt_status_t  bt_sink_srv_aws_mce_call_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{

    bt_status_t result = BT_STATUS_SUCCESS;
    bt_sink_srv_aws_mce_call_context_t* aws_call_cntx = NULL;
    bt_sink_srv_report_id("[CALL][AWS_MCE]common callback:%x, %x", 2, msg, status);
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    switch (msg) {
        case BT_AWS_MCE_CONNECTED: {
            bt_aws_mce_connected_t *conn = (bt_aws_mce_connected_t *)buffer;
            if (status == BT_STATUS_SUCCESS) {
                //alloc aws call context.
                aws_call_cntx = bt_sink_srv_aws_mce_call_get_free_context();
                if (aws_call_cntx) {
                    aws_call_cntx->aws_handle = conn->handle;
                    bt_sink_srv_report_id("[CALL][AWS_MCE]common_callback BT_AWS_MCE_CONNECTED, handle:0x%x", 1, aws_call_cntx->aws_handle);
                    if (role == BT_AWS_MCE_ROLE_CLINET || role == BT_AWS_MCE_ROLE_PARTNER) {
                        bt_bd_addr_t* local_addr = bt_connection_manager_device_local_info_get_local_address();
                        if (0 != bt_sink_srv_memcmp(conn->address, local_addr, sizeof(bt_bd_addr_t))) {
                            //alloc aws call pseduo device
                            aws_call_cntx->device = bt_sink_srv_call_psd_alloc_device(conn->address, bt_sink_srv_aws_mce_call_pseudo_dev_callback);
                            bt_sink_srv_call_psd_state_event_notify(aws_call_cntx->device, BT_SINK_SRV_CALL_EVENT_CONNECT_LINK_REQ_IND, NULL);
                            bt_sink_srv_call_psd_state_event_notify(aws_call_cntx->device, BT_SINK_SRV_CALL_EVENT_LINK_CONNECTED, NULL);
                        }
                    } else {
                        bt_sink_srv_report_id("[CALL][AWS_MCE]Agent role!", 0);
                    }
                }else {
                    bt_sink_srv_report_id("[CALL][AWS_MCE]Can't find aws call context.", 0);
                }
            }
        }
        break;

        case  BT_AWS_MCE_DISCONNECTED: {
            bt_aws_mce_disconnected_t *disc_ind = (bt_aws_mce_disconnected_t *)buffer;
            aws_call_cntx = bt_sink_srv_aws_mce_call_get_context_by_handle(disc_ind->handle);
            if (aws_call_cntx && aws_call_cntx->aws_handle != BT_AWS_MCE_INVALID_HANDLE) {
                aws_call_cntx->aws_handle = BT_AWS_MCE_INVALID_HANDLE;
                if (role == BT_AWS_MCE_ROLE_CLINET || role == BT_AWS_MCE_ROLE_PARTNER) {
                    bt_bd_addr_t* local_addr = bt_connection_manager_device_local_info_get_local_address();
                    bt_bd_addr_t *cur_addr = (bt_bd_addr_t *)bt_aws_mce_get_bd_addr_by_handle(disc_ind->handle);
                    if (0 != bt_sink_srv_memcmp(cur_addr, local_addr, sizeof(bt_bd_addr_t))) {
                        bt_sink_srv_call_psd_state_event_notify(aws_call_cntx->device, BT_SINK_SRV_CALL_EVENT_LINK_DISCONNECTED, NULL);
                        /* Reset call state */
                        if ((bt_sink_srv_get_state() >= BT_SINK_SRV_STATE_INCOMING) &&
                            (bt_sink_srv_get_state() <= BT_SINK_SRV_STATE_MULTIPARTY)) {
                            bt_sink_srv_state_set(BT_SINK_SRV_STATE_NONE);
                        }
                    }
                } else if(role == BT_AWS_MCE_ROLE_AGENT) {
                    bt_sink_srv_memset((void *)&aws_call_cntx->call_info, 0, sizeof(bt_sink_srv_aws_mce_call_info_t));
                }
            } else {
                bt_sink_srv_report_id("[CALL][AWS_MCE]Can't find aws call context.", 0);
            }
        }
        break;

        case  BT_AWS_MCE_STATE_CHANGED_IND: {
            bt_aws_mce_state_change_ind_t *state_change = (bt_aws_mce_state_change_ind_t *)buffer;
            if (role == BT_AWS_MCE_ROLE_AGENT) {
                aws_call_cntx = bt_sink_srv_aws_mce_call_get_context_by_handle(state_change->handle);
                if (aws_call_cntx) {
                    if ((state_change->state & BT_AWS_MCE_AGENT_STATE_ATTACHED) == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
                        bt_bd_addr_t *addr = (bt_bd_addr_t *)bt_aws_mce_get_bd_addr_by_handle(aws_call_cntx->aws_handle);
                        bt_sink_srv_assert(addr);
                        if(bt_sink_srv_hf_check_is_connected(addr)
                          #ifdef MTK_BT_HSP_ENABLE
                            || bt_sink_srv_hsp_check_is_connected(addr)
                          #endif
                           ) {
                            bt_sink_srv_report_id("[CALL][AWS_MCE]Agent send call info:%d,%d,%d,%d", 4,
                            aws_call_cntx->call_info.call_state,
                            aws_call_cntx->call_info.sco_state,
                            aws_call_cntx->call_info.volume,
                            aws_call_cntx->call_info.is_ring);
                            bt_aws_mce_information_t send_sco_info = {
                             .type = BT_AWS_MCE_INFORMATION_SCO,
                             .data_length = sizeof(bt_sink_srv_aws_mce_call_info_t),
                             .data = (uint8_t*)&aws_call_cntx->call_info
                            };
                            bt_aws_mce_send_information(aws_call_cntx->aws_handle, (const bt_aws_mce_information_t *)&send_sco_info, true);
                        }
                    } else if (state_change->state == BT_AWS_MCE_AGENT_STATE_NONE) {
                        //RHO case, if hfp is not connected, aws call context need alloc a audio_src.
                        bt_bd_addr_t *rem_addr = (bt_bd_addr_t *)bt_aws_mce_get_bd_addr_by_handle(aws_call_cntx->aws_handle);
                        bt_sink_srv_assert(rem_addr);
                        if(!bt_sink_srv_hf_check_is_connected(rem_addr)
                          #ifdef MTK_BT_HSP_ENABLE
                            && !bt_sink_srv_hsp_check_is_connected(rem_addr)
                          #endif
                            ){
                            bt_bd_addr_t* agent_addr = bt_connection_manager_device_local_info_get_local_address();
                            bt_sink_srv_assert(agent_addr);
                            aws_call_cntx->device = bt_sink_srv_call_psd_alloc_device(agent_addr, bt_sink_srv_aws_mce_call_pseudo_dev_callback);
                            bt_sink_srv_assert(aws_call_cntx->device);
                            bt_sink_srv_call_psd_state_event_notify(aws_call_cntx->device, BT_SINK_SRV_CALL_EVENT_CONNECT_LINK_REQ_IND, NULL);
                            bt_sink_srv_call_psd_state_event_notify(aws_call_cntx->device, BT_SINK_SRV_CALL_EVENT_LINK_CONNECTED, NULL);
                        }
                    }
                } else {
                    bt_sink_srv_report_id("[CALL][AWS_MCE]Can't find context, handle:0x%08x", 1, state_change->handle);
                }
            }
        }
        break;

        case BT_AWS_MCE_CALL_AUDIO_CONNECTED: {
            bt_aws_mce_call_audio_connected_t *sco_conn_ind = (bt_aws_mce_call_audio_connected_t *)buffer;
            aws_call_cntx = bt_sink_srv_aws_mce_call_get_context_by_handle(sco_conn_ind->handle);
            if (aws_call_cntx) {
                bt_sink_srv_call_audio_codec_type_t call_codec = sco_conn_ind->sco_type - 1;
                bt_sink_srv_aws_mce_call_sco_state_change_notify(sco_conn_ind->handle, BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED, call_codec);
                //For aws call, connected means activated.
                bt_sink_srv_call_psd_set_codec_type(aws_call_cntx->device, call_codec);
                bt_sink_srv_call_psd_state_event_notify(aws_call_cntx->device, BT_SINK_SRV_CALL_EVENT_SCO_ACTIVATED, NULL);
            } else {
                bt_sink_srv_report_id("[CALL][AWS_MCE]Can't find aws call context.", 0);
            }
        }
        break;

        case BT_AWS_MCE_CALL_AUDIO_DISCONNECTED: {
            bt_aws_mce_call_audio_disconnected_t *sco_disc_ind = (bt_aws_mce_call_audio_disconnected_t *)buffer;
            aws_call_cntx = bt_sink_srv_aws_mce_call_get_context_by_handle(sco_disc_ind->handle);
            if (aws_call_cntx) {
                bt_sink_srv_aws_mce_call_sco_state_change_notify(sco_disc_ind->handle, BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED, 0);
                bt_sink_srv_call_psd_state_event_notify(aws_call_cntx->device, BT_SINK_SRV_CALL_EVENT_SCO_DISCONNECTED, NULL);
                bt_sink_srv_call_psd_set_codec_type(aws_call_cntx->device, BT_HFP_CODEC_TYPE_NONE);
                if ((aws_call_cntx->flag & BT_SINK_SRV_AWS_MCE_CALL_FLAG_SUSPEND) != 0) {
                    aws_call_cntx->flag &= ~BT_SINK_SRV_AWS_MCE_CALL_FLAG_SUSPEND;
                    bt_sink_srv_call_psd_state_event_notify(aws_call_cntx->device, BT_SINK_SRV_CALL_EVENT_SUSPEND_REQ, NULL);
                }
            } else {
                bt_sink_srv_report_id("[CALL][AWS_MCE]Can't find aws call context.", 0);
            }
        }
        break;

        case BT_AWS_MCE_INFOMATION_PACKET_IND: {
            bt_aws_mce_information_ind_t *call_data_ind = (bt_aws_mce_information_ind_t *)buffer;
            if (((role == BT_AWS_MCE_ROLE_PARTNER) || (role == BT_AWS_MCE_ROLE_CLINET)) &&
                (call_data_ind->packet.type == BT_AWS_MCE_INFORMATION_SCO)) {
                bt_sink_srv_aws_mce_call_parse_call_info(call_data_ind->handle, call_data_ind->packet.data, call_data_ind->packet.data_length);
            }
        }
        break;

        default: {
            bt_sink_srv_report_id("[CALL][AWS_MCE]common callback,exception event: %x", 1, msg);
        }
        break;
    }

    return result;
}
#ifdef AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
extern uint8_t bt_avm_esco_stream_control(bool value);
static void bt_sink_srv_aws_mce_call_hf_switch(bool value)
{
    bt_sink_srv_report_id("[CALL][AWS_MCE] hf switch value: %d", 1, value);
    uint8_t i = 0;
    uint8_t avm_status = 0;
    bt_sink_srv_aws_mce_call_context_t *context = NULL;
    for(i=0;i<BT_SINK_SRV_AWS_MCE_CALL_CONNECTION_NUM;i++) {
        if(bt_sink_srv_mce_call_context[i].device != NULL) {
            context = &bt_sink_srv_mce_call_context[i];
            break;
        }
    }
    if(context == NULL) {
        bt_sink_srv_report_id("[CALL][AWS_MCE] hf switch aws mce call context = NULL",0);
        return;
    }
    if(value) {
        bt_sink_srv_call_psd_state_event_notify(context->device,BT_SINK_SRV_CALL_EVENT_HF_SWITCH_START,NULL);
    } else {
        bt_sink_srv_call_psd_state_event_notify(context->device,BT_SINK_SRV_CALL_EVENT_HF_SWITCH_STOP,NULL);
    }
    avm_status = bt_avm_esco_stream_control(value);
    bt_sink_srv_report_id("[CALL][AWS_MCE] avm esco stream control status = %d", 1,avm_status);
}
#endif

void bt_sink_srv_aws_mce_call_app_report_callback(bt_aws_mce_report_info_t *para)
{
    uint8_t *local_para = (uint8_t *)para->param;
    bt_sink_srv_aws_mce_call_packet_type_t packet_type = local_para[0];
    switch(packet_type) {
        case BT_SINK_SRV_AWS_MCE_CALL_PACKET_ACTION: {
    bt_sink_srv_aws_mce_call_action_t *action = (bt_sink_srv_aws_mce_call_action_t *)(para->param);
    bt_sink_srv_aws_mce_call_dispatch_call_packet(action->packet_type, (uint8_t *)para->param, para->param_len);
        }
        break;
#ifdef AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
        case BT_SINK_SRV_AWS_MCE_CALL_HF_SWITCH: {
            bt_sink_srv_aws_mce_call_hf_switch_t *switch_para = (bt_sink_srv_aws_mce_call_hf_switch_t *)(para->param);
            bt_sink_srv_aws_mce_call_hf_switch(switch_para->switch_value);
        }
        break;
#endif
        default:
          bt_sink_srv_report_id("[CALL][AWS_MCE] app report callback,exception packet_type: %x", 1, packet_type);
          break;
    }
}

bt_status_t bt_sink_srv_aws_mce_call_action_handler(bt_sink_srv_action_t action, void *param)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    switch (action) {
        case BT_SINK_SRV_ACTION_PROFILE_INIT: {
        #ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            bt_sink_srv_call_role_handover_init();
        #endif /*SUPPORT_ROLE_HANDOVER_SERVICE*/
            bt_sink_srv_memset(bt_sink_srv_mce_call_context, 0, sizeof(bt_sink_srv_mce_call_context));
            break;
        }

        case BT_SINK_SRV_ACTION_PROFILE_DEINIT: {
            break;
        }

        default: {
            break;
        }
    }

    if (role == BT_AWS_MCE_ROLE_PARTNER) {
        if (BT_SINK_MODULE_HFP_ACTION == (action & 0xFFFFF000)) {
            bt_sink_srv_aws_mce_call_send_action(action, param);
        }
    }

    return BT_STATUS_SUCCESS;
}
#ifdef AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
bt_status_t bt_sink_srv_aws_mce_call_sync_hf_switch(bool value)
{
    bt_aws_mce_report_info_t app_report = {0};
    bt_sink_srv_aws_mce_call_hf_switch_t local_switch;
    local_switch.packet_type = BT_SINK_SRV_AWS_MCE_CALL_HF_SWITCH;
    local_switch.switch_value = value;

    app_report.module_id = BT_AWS_MCE_REPORT_MODULE_SINK_CALL;
    app_report.param = &local_switch;
    app_report.param_len = sizeof(bt_sink_srv_aws_mce_call_hf_switch_t);
    return bt_aws_mce_report_send_event(&app_report);
}
#endif

#ifdef BT_SINK_SRV_AWS_MCE_CALL_IN_MULTIPOINT
void bt_sink_srv_aws_mce_call_update_agent(void)
{
    bt_bd_addr_t *new_agent_address = bt_device_manager_aws_local_info_get_peer_address();
    bt_bd_addr_t *new_partner_address = bt_device_manager_aws_local_info_get_local_address();

    /* Destory in-active link context. */
    for (uint32_t i = 0; i < BT_SINK_SRV_AWS_MCE_CALL_CONNECTION_NUM; i++) {
        if (bt_sink_srv_mce_call_context[i].aws_handle != BT_AWS_MCE_INVALID_HANDLE) {
            bt_bd_addr_t *address = (bt_bd_addr_t *)bt_aws_mce_get_bd_addr_by_handle(
                bt_sink_srv_mce_call_context[i].aws_handle);

            if (address != NULL) {
                if (bt_sink_srv_memcmp(address, new_agent_address, sizeof(bt_bd_addr_t)) == 0) {
                    bt_sink_srv_report_id("[CALL][AWS_MCE]update agent, context[%d] is active link", 1, i);
                    continue;
                } else if (bt_sink_srv_memcmp(address, new_partner_address, sizeof(bt_bd_addr_t)) == 0) {
                    bt_sink_srv_report_id("[CALL][AWS_MCE]update agent, context[%d] is special link", 1, i);
                    continue;
                } else {
                    /* Destory in-active link context. */
                }
            }

            bt_sink_srv_report_id("[CALL][AWS_MCE]update agent, context[%d] is in-active link, destory", 1, i);
            bt_sink_srv_memset(&bt_sink_srv_mce_call_context[i], 0, sizeof(bt_sink_srv_aws_mce_call_context_t));
        }
    }
}

void bt_sink_srv_aws_mce_call_update_partner(void)
{
    bt_bd_addr_t connected_address[BT_SINK_SRV_AWS_MCE_CALL_CONNECTION_NUM] = {{0}};

    /* Create in-active link context. */
    uint32_t connected_number = bt_cm_get_connected_devices(
        ~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS),
        connected_address,
        BT_SINK_SRV_AWS_MCE_CALL_CONNECTION_NUM);

    for (uint32_t i = 0; i < connected_number; i++) {
        bt_sink_srv_aws_mce_call_context_t *context = bt_sink_srv_aws_mce_call_get_context_by_address((const bt_bd_addr_t *)&connected_address[i]);
        if (context == NULL) {
            bt_sink_srv_aws_mce_call_context_t *context = bt_sink_srv_aws_mce_call_get_free_context();
            bt_sink_srv_assert((context != NULL) && "Can not alloc a Sink AWS MCE CALL context!");

            context->aws_handle = bt_aws_mce_query_handle_by_address(
                BT_MODULE_AWS_MCE,
                (const bt_bd_addr_t *)&connected_address[i]);
        } else {
            if (context->device != NULL) {
                bt_sink_srv_report_id("[CALL][AWS_MCE]update partner, context 0x%x's device is not NULL(0x%x), need to be freed", 2,
                    context, context->device);
                context->device = NULL;
            }
        }
    }
}
#endif /* BT_SINK_SRV_AWS_MCE_CALL_IN_MULTIPOINT */
