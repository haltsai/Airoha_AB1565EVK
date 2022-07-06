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
 
#include "bt_a2dp.h"
#include "bt_sink_srv_music.h"
#include "audio_src_srv.h"
#include "bt_sink_srv_utils.h"

/* Function declare */

static void bt_sink_srv_music_transfer_state(audio_src_srv_state_t state);

static void bt_sink_srv_music_update_dev_state(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_state_t state);

static void bt_sink_srv_music_set_dev_transient_state(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_transient_state_t substate);

static void bt_sink_srv_music_state_none_handle(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_event_t evt_id, void *param);

static void bt_sink_srv_music_state_ready_handle(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_event_t evt_id, void *param);

static void bt_sink_srv_music_state_playing_handle(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_event_t evt_id, void *param);




static void bt_sink_srv_music_transfer_state(bt_sink_srv_music_state_t state)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    bt_sink_srv_report_id("[Music]transfer_state--ori: 0x%08x, cur: 0x%08x", 2, ctx->state, state);
    ctx->state = state;
}


static void bt_sink_srv_music_update_dev_state(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_state_t state)
{
    bt_sink_srv_report_id("[Music]update_dev_state-type: %d, ori: 0x%08x, cur: 0x%08x", 3, dev->handle->type, dev->handle->state, state);
    dev->state = state;
}


static void bt_sink_srv_music_set_dev_transient_state(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_transient_state_t substate)
{
    bt_sink_srv_report_id("[Music]set_dev_transient_state--type: %d, ori: %d, cur: %d", 3, dev->handle->type, dev->handle->substate, substate);
    //dev->substate = substate;
    audio_src_srv_set_substate(dev->handle, substate);
}


static void bt_sink_srv_music_state_none_handle(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_event_t evt_id, void *param)
{
    switch (evt_id) {
        /* SDK event */
        case BT_A2DP_CONNECT_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN);
            } else {
                /* Error */
            }
            break;
        }

        case BT_A2DP_CONNECT_CNF: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
            } else {
                /* Exception */
            }
            break;
        }

    #ifdef __BT_AWS_SUPPORT__
        case BT_AWS_CONNECT_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN);
            } else {
                /* Error */
            }
            break;
        }

        case BT_AWS_CONNECT_CNF: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
            } else {
                /* Exception */
            }
            break;
        }
    #endif
        /* Sink music event */
        case BT_SINK_SRV_MUSIC_EVT_READY: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN == dev->handle->substate) {
                /* Reset transient state */
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                /* Update seudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update music state */
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                /* AWS connection */
                /* Update seudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update music state */
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else {
                /* Exception */
            }
            break;
        }

        default:
            break;
    }
}


static void bt_sink_srv_music_state_ready_handle(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_event_t evt_id, void *param)
{
    switch (evt_id) {
        /* SDK event */
        case BT_SINK_SRV_MUSIC_EVT_START_IND:
        case BT_A2DP_START_STREAMING_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC);
                /* Query audio source play */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC == dev->handle->substate){
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC);
            }
            break;
        }
        case BT_SINK_SRV_MUSIC_EVT_PREPARE_CODEC: {
            bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC);
            break;
        }
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
        case BT_AWS_MCE_DISCONNECTED :
#endif
        case BT_A2DP_DISCONNECT_IND : 
        case BT_A2DP_DISCONNECT_CNF : {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
            }
            break;
        }
        case BT_SINK_SRV_MUSIC_EVT_SUSPEND_IND: 
        case BT_A2DP_SUSPEND_STREAMING_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            } else if(BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC == 
                dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
            }
            break;
        }

    #ifdef __BT_AWS_SUPPORT__
        case BT_AWS_DISCONNECT_IND : {
            audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            break;
        }
    #endif
        /* Sink music event */
        case BT_SINK_SRV_MUSIC_EVT_REJECT: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_CODEC_OPEN: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC == dev->handle->substate
                 || BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER);
            } else if(BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC == 
                dev->handle->substate) {
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_RESUME: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_PLAYING: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);                
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_PLAYING);
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_PLAYING);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PLAYING);
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_READY: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN == dev->handle->substate ) {
                /* Reset transient state */
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                /* Update seudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                /* AWS connection */
                /* Update seudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else {
                /* Exception */
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_UNAVAILABLE: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate ) {
                /* Update pseudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_NONE);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            } else {
                /* Exception */
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_CODEC_CLEAR: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if(BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC == 
                dev->handle->substate){
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_AVRCP_PLAYING: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC);
                /* Query audio source play */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_AVRCP_PAUSED: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

    #ifdef __BT_AWS_MCE_A2DP_SUPPORT__
        case BT_SINK_SRV_MUISC_EVT_AWS_MCE_A2DP_DIS: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }
    #endif /* __BT_AWS_MCE_A2DP_SUPPORT__ */

        default:
            break;
    }
}


static void bt_sink_srv_music_state_playing_handle(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_event_t evt_id, void *param)
{
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
#endif

    switch (evt_id) {
        /* SDK event */
        case BT_A2DP_SUSPEND_STREAMING_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_A2DP_DISCONNECT_IND :
        case BT_A2DP_DISCONNECT_CNF : {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            } else if(BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC == dev->handle->substate) {
                ;
            }else {
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            }
            break;
        }

        case BT_A2DP_START_STREAMING_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC);
                /* Query audio source play */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
            }
            break;
        }
    #ifdef __BT_AWS_SUPPORT__
        case BT_AWS_DISCONNECT_IND : {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_AWS_DISCONNECT_CNF: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }
    #endif /* __BT_AWS_SUPPORT__ */

    #ifdef __BT_AWS_MCE_A2DP_SUPPORT__
        case BT_AWS_MCE_DISCONNECTED : {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_AWS_MCE_CONNECTED: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }
    #endif /* __BT_AWS_MCE_A2DP_SUPPORT__ */

        /* Sink music event */
        case BT_SINK_SRV_MUSIC_EVT_CODEC_CLEAR: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                /* Interrupt case */
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);                
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Nature */
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);                
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Nature */
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_RECOVER: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                /* Recover flow equal waiting list + stop */
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                /* Priority +1 */
                ++dev->handle->priority;
                /* Push waiting list */
                audio_src_srv_add_waiting_list(dev->handle);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_SUSPEND_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_READY: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN == dev->handle->substate ) {
                /* Reset transient state */
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                /* Update seudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate){
                /* AWS connection */
                /* Update seudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else {
                /* Exception */
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_UNAVAILABLE: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate ) {
                /* Update pseudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_NONE);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            } else {
                /* Exception */
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_AVRCP_PLAYING: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC);
                /* Query audio source play */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_AVRCP_PAUSED: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }
        case BT_SINK_SRV_MUSIC_EVT_PREPARE_CLEAR: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
            }
            break;
        }

    #ifdef __BT_AWS_MCE_A2DP_SUPPORT__
        case BT_SINK_SRV_MUISC_EVT_AWS_MCE_A2DP_DIS: {
            if ((BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) &&
                (dev == ctx->run_dev)) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }
    #endif /* __BT_AWS_MCE_A2DP_SUPPORT__ */

        default:
            break;
    }
}


void bt_sink_srv_music_state_machine_handle(bt_sink_srv_music_device_t *dev, uint32_t evt_id, void *param)
{
    if(!(dev->handle)) {
        bt_sink_srv_report_id("[Music]Error, handle NULL", 0);
        return;
    }
    bt_sink_srv_report_id("[Music]state_machine_handle--state: 0x%08x, type: %d, evt: 0x%x, dev--state: 0x%08x, substate: %d", 5,
        dev->state, dev->handle->type, evt_id, dev->handle->state, dev->handle->substate);

    switch (dev->state) {
        case AUDIO_SRC_SRV_STATE_NONE: {
            bt_sink_srv_music_state_none_handle(dev, evt_id, param);
            break;
        }

        case AUDIO_SRC_SRV_STATE_READY: {
            bt_sink_srv_music_state_ready_handle(dev, evt_id, param);
            break;
        }

        case AUDIO_SRC_SRV_STATE_PLAYING:
        {
            bt_sink_srv_music_state_playing_handle(dev, evt_id, param);
            break;
        }
    }
}

