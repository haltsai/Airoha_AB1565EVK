/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

#include "audio_src_srv.h"
#include "audio_sink_srv_line_in.h"
#include "audio_sink_srv_line_in_internal.h"

#define UNUSED(x)  ((void)(x))

/* Function declare */

static void audio_sink_srv_line_in_transfer_state(audio_src_srv_state_t state);

static void audio_sink_srv_line_in_update_dev_state(audio_sink_srv_line_in_device_t *dev, audio_sink_srv_line_in_state_t state);

static void audio_sink_srv_line_in_set_dev_transient_state(audio_sink_srv_line_in_device_t *dev, audio_sink_srv_line_in_transient_state_t substate);

static void audio_sink_srv_line_in_state_none_handle(audio_sink_srv_line_in_device_t *dev, audio_sink_srv_line_in_event_t evt_id, void *param);

static void audio_sink_srv_line_in_state_ready_handle(audio_sink_srv_line_in_device_t *dev, audio_sink_srv_line_in_event_t evt_id, void *param);

static void audio_sink_srv_line_in_state_playing_handle(audio_sink_srv_line_in_device_t *dev, audio_sink_srv_line_in_event_t evt_id, void *param);




static void audio_sink_srv_line_in_transfer_state(audio_sink_srv_line_in_state_t state)
{
    audio_sink_srv_line_in_context_t *ctx = audio_sink_srv_line_in_get_context();

    audio_src_srv_report("[Line-IN]transfer_state--ori: 0x%08x, cur: 0x%08x", 2, ctx->state, state);
    ctx->state = state;
}


static void audio_sink_srv_line_in_update_dev_state(audio_sink_srv_line_in_device_t *dev, audio_sink_srv_line_in_state_t state)
{
    audio_src_srv_report("[Line-IN]update_dev_state-type: %d, ori: 0x%08x, cur: 0x%08x", 3, dev->handle->type, dev->handle->state, state);
    dev->state = state;
}


static void audio_sink_srv_line_in_set_dev_transient_state(audio_sink_srv_line_in_device_t *dev, audio_sink_srv_line_in_transient_state_t substate)
{
    audio_src_srv_report("[Line-IN]set_dev_transient_state--type: %d, ori: %d, cur: %d", 3, dev->handle->type, dev->handle->substate, substate);
    //dev->substate = substate;
    audio_src_srv_set_substate(dev->handle, substate);
}


static void audio_sink_srv_line_in_state_none_handle(audio_sink_srv_line_in_device_t *dev, audio_sink_srv_line_in_event_t evt_id, void *param)
{
    switch (evt_id) {
        /* SDK event */
        /* Sink Line-IN event */
        case AUDIO_SINK_SRV_LINE_IN_EVT_READY: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate) {
                /* Reset transient state */
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE);
                /* Update seudo device state */
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update music state */
                audio_sink_srv_line_in_transfer_state(AUDIO_SRC_SRV_STATE_READY);
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


static void audio_sink_srv_line_in_state_ready_handle(audio_sink_srv_line_in_device_t *dev, audio_sink_srv_line_in_event_t evt_id, void *param)
{
    switch (evt_id) {
        /* SDK event */
/*
        case AUDIO_SINK_SRV_LINE_IN_EVT_START_IND:
        case AUDIO_SINK_SRV_LINE_IN_EVT_START_STREAMING_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_CODEC);
                // Query audio source play
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
            }
            break;
        }
*/
        case AUDIO_SINK_SRV_LINE_IN_EVT_SUSPEND: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_BUFFER == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            } else if(AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_CODEC == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_CLEAR_CODEC);
            }
            break;
        }

        /* Sink music event */
        case AUDIO_SINK_SRV_LINE_IN_EVT_REJECT: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_CODEC == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE);
            }
            audio_src_srv_add_waiting_list(dev->handle);
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_CODEC_OPEN: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_CODEC == dev->handle->substate
                 || AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_BUFFER);
            } else if(AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_CLEAR_CODEC ==
                dev->handle->substate) {
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_PREPARE_FAIL: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_CODEC == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            } else if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_BUFFER == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_RESUME: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate) {
            }
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_PLAYING: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE);
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_PLAYING);
                audio_sink_srv_line_in_transfer_state(AUDIO_SRC_SRV_STATE_PLAYING);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PLAYING);
            }

            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_READY: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_WAIT_CONN == dev->handle->substate ) {
                /* Reset transient state */
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE);
                /* Update seudo device state */
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate) {
                /* AWS connection */
                /* Update seudo device state */
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else {
                /* Exception */
            }
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_UNAVAILABLE: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate ) {
                /* Update pseudo device state */
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_NONE);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            } else {
                /* Exception */
            }
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_CODEC_CLEAR: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_CLEAR_CODEC == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE);
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                audio_sink_srv_line_in_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_BUFFER == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE);
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                audio_sink_srv_line_in_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if(AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_CODEC ==
                dev->handle->substate){
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_CLEAR_CODEC);
            }
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_START: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate) {
/*
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_CODEC);
                // Query audio source play
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
                // Update pseudo device state
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_PREPARE_PLAY);
*/
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE);
                // Query audio source play
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
                // Update pseudo device state
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_PLAYING);
            }
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_STOP: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate) {
                //audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        default:
            break;
    }
}


static void audio_sink_srv_line_in_state_playing_handle(audio_sink_srv_line_in_device_t *dev, audio_sink_srv_line_in_event_t evt_id, void *param)
{
    audio_sink_srv_line_in_context_t *ctx = audio_sink_srv_line_in_get_context();

    switch (evt_id) {
        /* SDK event */
/*
        case AUDIO_SINK_SRV_LINE_IN_EVT_SUSPEND_STREAMING_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_DISCONNECT :
        case AUDIO_SINK_SRV_LINE_IN_EVT_DISCONNECT_CNF : {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            } else {
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            }
            break;
        }
        case AUDIO_SINK_SRV_LINE_IN_EVT_START_STREAMING_IND: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_CODEC);
                // Query audio source play
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
            }
            break;
        }
*/
        /* Sink music event */
        case AUDIO_SINK_SRV_LINE_IN_EVT_PLAYING: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE);
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_PLAYING);
                audio_sink_srv_line_in_transfer_state(AUDIO_SRC_SRV_STATE_PLAYING);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PLAYING);
            }

            break;
        }
        case AUDIO_SINK_SRV_LINE_IN_EVT_CODEC_CLEAR: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate) {
                /* Interrupt case */
                audio_sink_srv_line_in_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_CLEAR_CODEC == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE);
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Nature */
                audio_sink_srv_line_in_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            }
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_RECOVER: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate) {
                /* Recover flow equal waiting list + stop */
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_CLEAR_CODEC);
                /* Priority +1 */
                ++dev->handle->priority;
                /* Push waiting list */
                audio_src_srv_add_waiting_list(dev->handle);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_SUSPEND: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate) {
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_READY: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_WAIT_CONN == dev->handle->substate ) {
                /* Reset transient state */
                audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE);
                /* Update seudo device state */
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate){
                /* AWS connection */
                /* Update seudo device state */
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else {
                /* Exception */
            }
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_UNAVAILABLE: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate ) {
                /* Update pseudo device state */
                audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_NONE);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            } else {
                /* Exception */
            }
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_START: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate) {
                //audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_CODEC);
                // Query audio source play
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
            }
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_EVT_STOP: {
            if (AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE == dev->handle->substate) {
                //audio_sink_srv_line_in_set_dev_transient_state(dev, AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        default:
            break;
    }
    UNUSED(ctx);
}


void audio_sink_srv_line_in_state_machine_handle(audio_sink_srv_line_in_device_t *dev, uint32_t evt_id, void *param)
{
    if(!(dev->handle)) {
        audio_src_srv_report("[Line-IN]Error, handle NULL", 0);
        return;
    }
    if(dev->state == 0){
        /* Update pseudo device state */
        audio_sink_srv_line_in_update_dev_state(dev, AUDIO_SRC_SRV_STATE_NONE);
        audio_src_srv_report("[Line-IN]Change state from OFF to NONE", 0); //ToDO
    }
    audio_src_srv_report("[Line-IN]state_machine_handle--state: 0x%08x, type: %d, evt: 0x%x, dev--state: 0x%08x, substate: %d", 5,
        dev->state, dev->handle->type, evt_id, dev->handle->state, dev->handle->substate);

    switch (dev->state) {
        case AUDIO_SRC_SRV_STATE_NONE: {
            audio_sink_srv_line_in_state_none_handle(dev, evt_id, param);
            break;
        }

        case AUDIO_SRC_SRV_STATE_READY: {
            audio_sink_srv_line_in_state_ready_handle(dev, evt_id, param);
            break;
        }

        case AUDIO_SRC_SRV_STATE_PLAYING:
        {
            audio_sink_srv_line_in_state_playing_handle(dev, evt_id, param);
            break;
        }
    }
}

