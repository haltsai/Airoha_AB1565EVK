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
 
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_hf.h"
#include "bt_connection_manager_internal.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_sink_srv_call_pseudo_dev.h"
#include "bt_sink_srv_call_pseudo_dev_mgr.h"

extern bt_sink_srv_hf_context_t g_sink_srv_hf_context[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];

bt_sink_srv_hf_context_t *g_sink_srv_hf_hightlight_p;
bt_status_t bt_sink_srv_hf_set_audio_status(uint32_t handle, bt_hfp_audio_status_t status);

static void bt_sink_srv_hf_state_set_and_sync(bt_sink_srv_state_t state)
{
#ifdef MTK_BT_CM_SUPPORT
    /* For CM restructure, only reset Sink state in call. */
    bt_sink_srv_state_t origin_state = bt_sink_srv_get_state();
    if (state <= BT_SINK_SRV_STATE_CONNECTED) {
        if ((origin_state >= BT_SINK_SRV_STATE_INCOMING) && (origin_state <= BT_SINK_SRV_STATE_MULTIPARTY)) {
            bt_sink_srv_state_set(BT_SINK_SRV_STATE_NONE);
        }
    } else {
        bt_sink_srv_state_set(state);
    }
#else
    bt_sink_srv_state_set(state);
#endif
}

void bt_sink_srv_hf_reset_highlight_device(void)
{
    bt_sink_srv_report_id("[CALL][HF][MP]Reset highlight.", 0);
    g_sink_srv_hf_hightlight_p = NULL;
}

void bt_sink_srv_hf_set_highlight_device(bt_sink_srv_hf_context_t *device)
{
    bt_sink_srv_hf_context_t *hilight = g_sink_srv_hf_hightlight_p;

    bt_sink_srv_report_id("[CALL][HF][MP]set highlight, hilight_hd:0x%x, device_hd:0x%x", 2,
                       (hilight == NULL ? 0 : hilight->link.handle),
                       (device == NULL ? 0 : device->link.handle));

    g_sink_srv_hf_hightlight_p = device;
    bt_sink_srv_hf_mp_set_sco(hilight, device);
    bt_sink_srv_report_id("[CALL][HF][MP]set done, hilight:0x%x", 1,
                       (g_sink_srv_hf_hightlight_p == NULL ? NULL : g_sink_srv_hf_hightlight_p));
}

bt_sink_srv_hf_context_t *bt_sink_srv_hf_get_highlight_device(void)
{
    bt_sink_srv_report_id("[CALL][HF][MP]get highlight:0x%x, handle:0x%x", 2,
        g_sink_srv_hf_hightlight_p, (g_sink_srv_hf_hightlight_p == NULL ? 0 : g_sink_srv_hf_hightlight_p->link.handle));
    return g_sink_srv_hf_hightlight_p;
}

void bt_sink_srv_hf_mp_state_change(bt_sink_srv_hf_context_t *device)
{
    if (device) {
        bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = NULL;

        if (NULL == bt_sink_srv_hf_get_highlight_device()) {
            bt_sink_srv_hf_set_highlight_device(device);
        }

        switch (device->link.call_state) {
            case BT_SINK_SRV_HF_CALL_STATE_IDLE: {
                bt_sink_srv_hf_context_p =
                    bt_sink_srv_hf_find_device_by_call_state(BT_SINK_SRV_HF_CALL_STATE_ALL);

                if (NULL != bt_sink_srv_hf_context_p) {
                    if (BT_SINK_SRV_HF_CALL_STATE_IDLE != bt_sink_srv_hf_context_p->link.call_state) {

                        bt_sink_srv_hf_set_highlight_device(bt_sink_srv_hf_context_p);

                        if (bt_sink_srv_hf_context_p->link.call_state & BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING) {
                            BT_SINK_SRV_HF_HOLD_CALL(bt_sink_srv_hf_context_p->link.handle, BT_SINK_SRV_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER);
                        }
                        bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)bt_sink_srv_hf_context_p->link.call_state);
                        break;
                    }
                } else {
                    for (uint32_t i = 0; i < BT_SINK_SRV_HF_LINK_NUM; i++) {
                        if ((device != &g_sink_srv_hf_context[i]) &&
                            (g_sink_srv_hf_context[i].link.flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED)) {
                            bt_sink_srv_hf_context_p = &g_sink_srv_hf_context[i];
                            break;
                        }
                    }
                    bt_sink_srv_hf_set_highlight_device(bt_sink_srv_hf_context_p);
                    if (device->link.handle) {
                        bt_sink_srv_hf_state_set_and_sync(BT_SINK_SRV_STATE_CONNECTED);
                    } else {
                        bt_sink_srv_report_id("[CALL][HF][MP]Already disconnected.", 0);
                    #ifdef MTK_BT_CM_SUPPORT
                        /* For CM restructure, reset Sink state after disconnected. */
                        bt_sink_srv_hf_state_set_and_sync(BT_SINK_SRV_STATE_NONE);
                    #endif
                    }
                }
            }
            break;

            case BT_SINK_SRV_HF_CALL_STATE_INCOMING: {
                bt_sink_srv_hf_context_p =
                    bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                            (BT_SINK_SRV_HF_CALL_STATE_OUTGOING | BT_SINK_SRV_HF_CALL_STATE_ACTIVE |
                             BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING | BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING |
                             BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE | BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING |
                             BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING));

                if (NULL != bt_sink_srv_hf_context_p) {
                    bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING);
                } else {
                    bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)device->link.call_state);
                }
            }
            break;

            case BT_SINK_SRV_HF_CALL_STATE_OUTGOING: {
                bt_sink_srv_hf_context_p =
                    bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                            (BT_SINK_SRV_HF_CALL_STATE_INCOMING | BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING));

                if (NULL != bt_sink_srv_hf_context_p) {
                    bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING);
                } else {
                    bt_sink_srv_hf_context_p =
                        bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                                (BT_SINK_SRV_HF_CALL_STATE_ACTIVE | BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING |
                                 BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE | BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING |
                                 BT_SINK_SRV_HF_CALL_STATE_MULTIPARTY));
                    if (NULL != bt_sink_srv_hf_context_p) {
                        bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING);
                    #if 0
                        if (bt_sink_srv_hf_context_p->link.call_state == BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING) {
                            bt_sink_srv_hf_set_highlight_device(device);
                        }
                    #endif
                    } else {
                        bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)device->link.call_state);
                    }
                }
            }
            break;

            case BT_SINK_SRV_HF_CALL_STATE_ACTIVE: {
                bt_sink_srv_hf_context_p =
                    bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                            (BT_SINK_SRV_HF_CALL_STATE_INCOMING | BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING));

                if (NULL != bt_sink_srv_hf_context_p) {
                    bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING);
                } else {
                    bt_sink_srv_hf_context_p =
                        bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                                (BT_SINK_SRV_HF_CALL_STATE_OUTGOING | BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING));
                    if (NULL != bt_sink_srv_hf_context_p) {
                        bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING);
                    } else {
                        bt_sink_srv_hf_context_p =
                            bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                                    (BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING));
                        if (NULL != bt_sink_srv_hf_context_p) {
                            bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE);
                            if (bt_sink_srv_hf_context_p == bt_sink_srv_hf_get_highlight_device()) {
                                /*Need switch the higlitht devcie to from held device to active device.*/
                                bt_sink_srv_report_id("[CALL][HF][MP]Switch higlitht device to call.", 0);
                                //bt_sink_srv_hf_set_highlight_device(device);
                            }
                        } else {
                            // when call state change from hold to active, should active SCO
                            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_highlight_device();
                            if (device == bt_sink_srv_hf_context_p) {
                                if ((device->link.flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) &&
                                        (!(device->link.flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE)) &&
                                        (bt_sink_srv_hf_get_context_by_flag(BT_SINK_SRV_HF_FLAG_SCO_CREATED | BT_SINK_SRV_HF_FLAG_SCO_ACTIVE) == NULL)) {
                                    device->link.flag |= BT_SINK_SRV_HF_FLAG_SCO_ACTIVE;
                                    bt_sink_srv_hf_set_audio_status(device->link.handle, BT_HFP_AUDIO_STATUS_ACTIVE);
                                }
                            } else {
                                if ((bt_sink_srv_hf_context_p != device) &&
                                    (bt_sink_srv_hf_context_p->link.call_state == BT_SINK_SRV_HF_CALL_STATE_IDLE)) {
                                    bt_sink_srv_hf_set_highlight_device(device);
                                }
                            }
                            bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)device->link.call_state);
                        }
                    }
                }
            }
            break;

            case BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING: {
                bt_sink_srv_hf_context_p =
                    bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                            (BT_SINK_SRV_HF_CALL_STATE_INCOMING | BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING));

                if (NULL != bt_sink_srv_hf_context_p) {
                    bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING);
                } else {
                    bt_sink_srv_hf_context_p =
                        bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                                (BT_SINK_SRV_HF_CALL_STATE_OUTGOING | BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING));
                    if (NULL != bt_sink_srv_hf_context_p) {
                        bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING);
                    } else {
                        bt_sink_srv_hf_context_p =
                            bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                                    (BT_SINK_SRV_HF_CALL_STATE_ACTIVE));
                        if (NULL != bt_sink_srv_hf_context_p) {
                            bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE);
                        } else {
                            for (uint32_t i = 0; i < BT_SINK_SRV_HF_LINK_NUM; i++) {
                                bt_sink_srv_hf_context_p = &g_sink_srv_hf_context[i];
                                if ((bt_sink_srv_hf_context_p != device) &&
                                    ((bt_sink_srv_hf_context_p->link.call_state != BT_SINK_SRV_HF_CALL_STATE_IDLE) ||
                                     (bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED))) {
                                    bt_sink_srv_hf_set_highlight_device(bt_sink_srv_hf_context_p);
                                }
                            }
                            bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)device->link.call_state);
                        }
                    }
                }
            }
            break;

            default: {
                bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)device->link.call_state);
            }
            break;
        }
    }
}

void bt_sink_srv_hf_mp_answer(bt_sink_srv_hf_context_t *current_device, bool accept)
{
    bt_sink_srv_hf_context_t *highlight = bt_sink_srv_hf_get_highlight_device();

    bt_sink_srv_report_id("[CALL][HF][MP] hiligh_hd:0x%x, curr_hd:0x%x", 2, highlight->link.handle, current_device->link.handle);

    if (current_device != highlight) {
        if (accept && (highlight->link.call_state & BT_SINK_SRV_HF_CALL_STATE_ACTIVE)) {
            BT_SINK_SRV_HF_HOLD_CALL(highlight->link.handle, BT_SINK_SRV_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER);
        } else if(accept && (highlight->link.call_state & BT_SINK_SRV_HF_CALL_STATE_OUTGOING)) {
            BT_SINK_SRV_HF_TERMINATE_CALL(highlight->link.handle);
        }

        if (accept) {
            bt_sink_srv_hf_set_highlight_device(current_device);
            BT_SINK_SRV_HF_ANSWER_CALL(current_device->link.handle);
        } else {
            current_device->link.flag |= BT_SINK_SRV_HF_FLAG_USER_REJECT;
            BT_SINK_SRV_HF_REJECT_CALL(current_device->link.handle);
        }
    }
}

void bt_sink_srv_hf_mp_swap(bt_sink_srv_hf_context_t *active_device, bt_sink_srv_hf_context_t *held_device)
{
    bt_sink_srv_report_id("[CALL][HF][MP]active:0x%x, held:0x%x", 2, active_device, held_device);
    if (active_device && held_device) {
        bt_sink_srv_report_id("[CALL][HF][MP] active handle:0x%x, held handle:0x%x", 2, active_device->link.handle, held_device->link.handle);
        if (active_device->link.call_state & BT_SINK_SRV_HF_CALL_STATE_ACTIVE) {
            BT_SINK_SRV_HF_HOLD_CALL(active_device->link.handle, BT_SINK_SRV_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER);
        }
        
        if (held_device->link.call_state & BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING) {
            BT_SINK_SRV_HF_HOLD_CALL(held_device->link.handle, BT_SINK_SRV_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER);
        }
        bt_sink_srv_hf_set_highlight_device(held_device);
    }
}

void bt_sink_srv_hf_mp_switch_audio(void)
{
    bt_bd_addr_t addr_list[BT_SINK_SRV_HF_LINK_NUM];
    uint32_t count = bt_sink_hf_get_connected_device_list(addr_list);
    uint16_t i;

    for (i = 0; i < count; i++) {
        bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = (bt_sink_srv_hf_context_t *)bt_sink_srv_hf_get_context_by_address(&addr_list[i]);

        if (NULL != bt_sink_srv_hf_context_p) {
            if (bt_sink_srv_hf_context_p != bt_sink_srv_hf_get_highlight_device()) {
                bt_sink_srv_hf_set_highlight_device(bt_sink_srv_hf_context_p);
                break;
            }
        }
    }
}

bt_status_t bt_sink_srv_hf_set_audio_status(uint32_t handle, bt_hfp_audio_status_t status)
{
    bt_sink_srv_report_id("[CALL][HF][MP]set audio, handle:0x%x status:0x%x", 2, handle, status);

#if defined(MTK_BT_CM_SUPPORT) && defined(AIR_MULTI_POINT_ENABLE)
    bt_bd_addr_t activated_address = {0};
    bt_bd_addr_t *activating_address = bt_hfp_get_bd_addr_by_handle(handle);

    bt_cm_get_connected_devices(
        BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), &activated_address, 1);

    if ((activating_address != NULL) && (status == BT_HFP_AUDIO_STATUS_ACTIVE)) {
        bt_cm_connect_t conn_req = {{0}};

        bt_sink_srv_memcpy(&conn_req.address, activating_address, sizeof(bt_bd_addr_t));
        conn_req.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);

        bt_status_t connect_status = bt_cm_connect(&conn_req);
        bt_sink_srv_report_id("[CALL][HF][MP]set audio, switch AWS link status:0x%x", 1, connect_status);
    }

    if ((activating_address != NULL)
        || (bt_cm_is_disconnecting_aws_device(*activating_address))
        || (bt_sink_srv_memcmp(&activated_address, activating_address, sizeof(bt_bd_addr_t)) != 0)) {
        status |= BT_SINK_SRV_HF_SWITCH_LINK;
    }
#endif

    return bt_hfp_set_audio_status(handle, status);
}

void bt_sink_srv_hf_mp_set_sco(bt_sink_srv_hf_context_t *highlight, bt_sink_srv_hf_context_t *device)
{
    if (NULL != device) {
        bt_sink_srv_hf_link_context_t *link_cntx = &device->link;
        if ( NULL != highlight) {
            bt_sink_srv_hf_link_context_t *hilit_cntx = &highlight->link;
            bt_sink_srv_report_id("[CALL][HF][MP] hilight_hd:0x%x, device_hd:0x%x, hilight_flag:0x%x, device_flag:0x%x", 4,
                               hilit_cntx->handle, link_cntx->handle, hilit_cntx->flag, link_cntx->flag);

            if (device == highlight) {
                if ((link_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) &&
                        (!(link_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE))) {
                    bt_sink_srv_call_psd_state_event_notify(device->device, BT_SINK_SRV_CALL_EVENT_SCO_ACTIVATE_REQ, NULL);
                }
            } else {
                bt_sink_srv_report_id("[CALL][HF][MP] hilight:0x%x, device:0x%x", 2, highlight->device, device->device);
                if ((hilit_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) &&
                        (hilit_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE)) {
                    bt_sink_srv_call_psd_state_event_notify(highlight->device, BT_SINK_SRV_CALL_EVENT_SCO_DEACTIVATE_REQ, NULL);
                } else if ((link_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) &&
                           (!(link_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE))) {
                    //link_cntx->flag |= BT_SINK_SRV_HF_FLAG_SCO_ACTIVE;
                    //bt_sink_srv_hf_set_audio_status(link_cntx->handle, BT_HFP_AUDIO_STATUS_ACTIVE);
                    //bt_sink_srv_call_psd_state_event_notify(device->device, BT_SINK_SRV_CALL_EVENT_SCO_ACTIVATE_REQ, NULL);
                }
            }
        }else {
            if ((link_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) &&
                        (!(link_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE))) {
                //link_cntx->flag |= BT_SINK_SRV_HF_FLAG_SCO_ACTIVE;
                //bt_sink_srv_hf_set_audio_status(link_cntx->handle, BT_HFP_AUDIO_STATUS_ACTIVE);
                bt_sink_srv_call_psd_state_event_notify(device->device, BT_SINK_SRV_CALL_EVENT_SCO_ACTIVATE_REQ, NULL);
            }
        }
    }
}

