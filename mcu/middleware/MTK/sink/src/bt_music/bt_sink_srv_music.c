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
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_avrcp.h"
#include "bt_sink_srv_ami.h"
#include "bt_sink_srv_a2dp.h"
#include "bt_sink_srv_aws_mce_a2dp.h"
#include "bt_sink_srv_state_notify.h"
#include "audio_src_srv.h"
#include "math.h"
#include "bt_avm.h"
#ifdef MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE
#include "hal_audio_message_struct.h"
#include "hal_audio_internal.h"
#endif
#include "avm_direct.h"
#include "bt_sink_srv_music_rho.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "bt_connection_manager_device_local_info.h"
#include "bt_device_manager_internal.h"
#ifdef BT_SINK_DUAL_ANT_ENABLE
#include "bt_sink_srv_dual_ant.h"
#endif

uint8_t g_alc_enable = 1;    /* for A2DP ALC */
bt_sink_srv_music_context_t g_bt_sink_srv_cntx;
extern hal_audio_status_t hal_audio_write_audio_drift_val(int32_t val);
extern hal_audio_status_t hal_audio_write_audio_anchor_clk(uint32_t val);
extern hal_audio_status_t hal_audio_write_audio_asi_base(uint32_t val);
extern hal_audio_status_t hal_audio_write_audio_asi_cur(uint32_t val);
extern bt_sink_srv_music_device_t *bt_sink_srv_avrcp_get_device(void *param, bt_sink_srv_action_t action);
#ifdef MTK_BT_CM_SUPPORT
void bt_sink_srv_music_init(void)
{
    g_alc_enable = 1;
    bt_cm_profile_service_register(BT_CM_PROFILE_SERVICE_A2DP_SINK, (bt_cm_profile_service_handle_callback_t)bt_sink_srv_a2dp_cm_callback_handler);
    bt_cm_profile_service_register(BT_CM_PROFILE_SERVICE_AVRCP, (bt_cm_profile_service_handle_callback_t)bt_sink_srv_avrcp_cm_callback_handler);

    /* Construct A2DP pseudo handle */
    bt_sink_srv_a2dp_create_pse_handle();
    /* Construct AWS A2DP pseudo handle */
#if defined(__BT_AWS_MCE_A2DP_SUPPORT__)
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    //bt_sink_srv_aws_mce_a2dp_create_pse_handle();
    bt_sink_srv_music_role_handover_service_init();
#endif
#endif
#ifdef MTK_BT_SPEAKER_ENABLE
    bt_status_t ret = bt_cm_register_event_callback(&bt_sink_srv_a2dp_mode_changed_handle_callback);
    bt_sink_srv_report_id("[sink][a2dp]register mode changed callback, ret: %d", 1, ret);
#endif
}

void bt_sink_srv_music_init_context(void)
{
    int32_t i = 0;
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    if (ctx->a2dp_aid != 0) {
        bt_sink_srv_report_id("[sink][music] sink_music a2dp_aid was init :%d", 1, ctx->a2dp_aid);
        return;
    }

    bt_sink_srv_memset(ctx, 0x00, sizeof(bt_sink_srv_music_context_t));
    ctx->state = AUDIO_SRC_SRV_STATE_NONE;

    for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
        // Init state and target state
        ctx->sink_dev[i].state = AUDIO_SRC_SRV_STATE_NONE;
        ctx->sink_dev[i].target_state = AUDIO_SRC_SRV_STATE_NONE;

        /* Init invalid a2dp & avrcp & aws handle */
        ctx->sink_dev[i].a2dp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        ctx->sink_dev[i].avrcp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        ctx->sink_dev[i].role = BT_A2DP_INVALID_ROLE;
#if defined(__BT_AWS_SUPPORT__) || defined(__BT_AWS_MCE_A2DP_SUPPORT__)
        ctx->sink_dev[i].aws_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
#endif
#if defined(__BT_AWS_SUPPORT__)
        ctx->sink_dev[i].aws_role = BT_AWS_ROLE_NONE;
#endif
    }
    ctx->a2dp_aid = BT_SINK_SRV_INVALID_AID;
    ctx->aws_aid = BT_SINK_SRV_INVALID_AID;
    ctx->vol_lev = AUD_VOL_OUT_LEVEL4;
    ctx->init_sink_latency = BT_SINK_SRV_A2DP_DEFAULT_SINK_LATENCY;
    /* default A2DP interrupt ULL flag are configured as true */
    bt_sink_srv_music_set_interrupt_flag(BT_SINK_SRV_CNTX_FLAG_A2DP_INTER_ULL_BY_AVRCP, false);
    bt_sink_srv_report_id("[sink][music]init-EVT_START: 0x%x", 1, BT_SINK_SRV_MUSIC_EVT_START);
}
#else
void bt_sink_srv_music_init(void)
{
    int32_t i = 0;
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    g_alc_enable = 1;
    if (ctx->a2dp_aid != 0) {
        return;
    }

    bt_sink_srv_memset(ctx, 0x00, sizeof(bt_sink_srv_music_context_t));
    ctx->state = AUDIO_SRC_SRV_STATE_NONE;

    for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
        // Init state and target state
        ctx->sink_dev[i].state = AUDIO_SRC_SRV_STATE_NONE;
        ctx->sink_dev[i].target_state = AUDIO_SRC_SRV_STATE_NONE;

        /* Init invalid a2dp & avrcp & aws handle */
        ctx->sink_dev[i].a2dp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        ctx->sink_dev[i].avrcp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        ctx->sink_dev[i].role = BT_A2DP_INVALID_ROLE;
    #if defined(__BT_AWS_SUPPORT__) || defined(__BT_AWS_MCE_A2DP_SUPPORT__)
        ctx->sink_dev[i].aws_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
    #endif
    #if defined(__BT_AWS_SUPPORT__)
        ctx->sink_dev[i].aws_role = BT_AWS_ROLE_NONE;
    #endif
    }
    ctx->a2dp_aid = BT_SINK_SRV_INVALID_AID;
    ctx->aws_aid = BT_SINK_SRV_INVALID_AID;
    ctx->vol_lev = AUD_VOL_OUT_LEVEL4;
    ctx->init_sink_latency = BT_SINK_SRV_A2DP_DEFAULT_SINK_LATENCY;

    /* default A2DP interrupt ULL flag are configured as true */
    bt_sink_srv_music_set_interrupt_flag(BT_SINK_SRV_CNTX_FLAG_A2DP_INTER_ULL_BY_AVRCP, true);

    /* Construct A2DP pseudo handle */
    bt_sink_srv_a2dp_create_pse_handle();
    /* Construct AWS A2DP pseudo handle */
#if defined(__BT_AWS_MCE_A2DP_SUPPORT__)
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    //bt_sink_srv_aws_mce_a2dp_create_pse_handle();
    bt_sink_srv_music_role_handover_service_init();
#endif
#endif
    bt_sink_srv_report_id("[sink][music]init-EVT_START: 0x%x", 1, BT_SINK_SRV_MUSIC_EVT_START);
}
#endif

bt_sink_srv_music_context_t *bt_sink_srv_music_get_context(void)
{
    return &g_bt_sink_srv_cntx;
}

bt_sink_srv_music_device_t *bt_sink_srv_music_get_device(bt_sink_srv_music_device_type_t type, const void *param)
{
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_bd_addr_t *dev_addr = NULL;
    int32_t i = 0;
    uint32_t *p_hd = NULL;
    #if defined(__BT_AWS_SUPPORT__) || defined(__BT_AWS_MCE_A2DP_SUPPORT__)
    audio_src_srv_pseudo_device_t pse_type = AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP;
    #endif
    audio_src_srv_handle_t *pse_hd = NULL;
    bool aws_exist = false;

    ctx = bt_sink_srv_music_get_context();

    switch (type) {
        case BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD: {
            p_hd = (uint32_t *)param;

            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if (ctx->sink_dev[i].a2dp_hd != BT_SINK_SRV_MUSIC_INVALID_HD
                        && ctx->sink_dev[i].a2dp_hd == *p_hd) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD: {
            p_hd = (uint32_t *)param;

            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if (ctx->sink_dev[i].avrcp_hd != BT_SINK_SRV_MUSIC_INVALID_HD
                        && ctx->sink_dev[i].avrcp_hd == *p_hd) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP: {
            dev_addr = (bt_bd_addr_t *)param;

            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].a2dp_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                    (memcmp(dev_addr, &(ctx->sink_dev[i].dev_addr), sizeof(bt_bd_addr_t)) == 0)) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_ADDR_AVRCP: {
            dev_addr = (bt_bd_addr_t *)param;

            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].avrcp_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                    (memcmp(dev_addr, &(ctx->sink_dev[i].dev_addr), sizeof(bt_bd_addr_t)) == 0)) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

    #ifdef __BT_AWS_SUPPORT__
        case BT_SINK_SRV_MUSIC_DEVICE_ADDR_AWS_MP3: {
            dev_addr = (bt_bd_addr_t *)param;

            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].aws_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                    ((ctx->sink_dev[i].mp3->audio_src) && (ctx->sink_dev[i].mp3->audio_src->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_MP3)) &&
                    (memcmp(dev_addr, &(ctx->sink_dev[i].dev_addr), sizeof(bt_bd_addr_t)) == 0)) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }
    #endif
        case BT_SINK_SRV_MUSIC_DEVICE_UNUSED: {
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].a2dp_hd == BT_SINK_SRV_MUSIC_INVALID_HD)
                    && (ctx->sink_dev[i].avrcp_hd == BT_SINK_SRV_MUSIC_INVALID_HD)
                    && (ctx->run_dev != &(ctx->sink_dev[i]))
            #ifdef __BT_AWS_MCE_A2DP_SUPPORT__
                    && (ctx->sink_dev[i].aws_hd == BT_SINK_SRV_MUSIC_INVALID_HD)
            #endif
            #ifdef MTK_AUDIO_MP3_ENABLED
                    && (ctx->sink_dev[i].mp3 == NULL)
            #endif
                ) {
                    dev = &(ctx->sink_dev[i]);
                    ctx->latest_dev = dev;
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_USED: {
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if (ctx->sink_dev[i].a2dp_hd != BT_SINK_SRV_MUSIC_INVALID_HD) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_LATEST: {
            dev = ctx->latest_dev;
            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_INTERRUPT: {
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if (ctx->sink_dev[i].flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_FOCUS: {
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if (ctx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) {
                    aws_exist = true;
                    break;
                }
            }

            /* find SP dev information */
            if (aws_exist) {
                for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                    if ((ctx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT) &&
                        (ctx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
                        dev = &(ctx->sink_dev[i]);
                        break;
                    }
                }
            } else {
                /* if there are no streaming dev, the latest conneced dev will be
                the focus dev */
                if (ctx->focus_dev) {
                    dev = ctx->focus_dev;
                } else {
                    dev = ctx->latest_dev;
                }
            }
            break;
        }
    #if defined(__BT_AWS_SUPPORT__) || defined(__BT_AWS_MCE_A2DP_SUPPORT__)
        case BT_SINK_SRV_MUSIC_DEVICE_AWS_A2DP_HD: {
            p_hd = (uint32_t *)param;

            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].aws_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                    ((ctx->sink_dev[i].handle) && (ctx->sink_dev[i].handle->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP)) &&
                    (ctx->sink_dev[i].aws_hd == *p_hd)) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

    #ifdef MTK_AUDIO_MP3_ENABLED
        case BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD: {
            p_hd = (uint32_t *)param;

            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].aws_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                    ((ctx->sink_dev[i].mp3->audio_src) && (ctx->sink_dev[i].mp3->audio_src->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_MP3)) &&
                    (ctx->sink_dev[i].aws_hd == *p_hd)) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }
    #endif

        case BT_SINK_SRV_MUSIC_DEVICE_AWS: {
            uint32_t tmp = (uint32_t)param;
            pse_type = (audio_src_srv_pseudo_device_t)tmp;
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].aws_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                    (ctx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) &&
                    ((ctx->sink_dev[i].handle) && (ctx->sink_dev[i].handle->type == pse_type))) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_AWS_BY_LINK: {
            uint32_t tmp = (uint32_t)param;
            pse_type = (audio_src_srv_pseudo_device_t)tmp;
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].aws_hd != BT_SINK_SRV_MUSIC_INVALID_HD) &&
                    ((ctx->sink_dev[i].handle) && (ctx->sink_dev[i].handle->type == pse_type))) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

            break;
        }
    #endif /* defined(__BT_AWS_SUPPORT__) || defined(__BT_AWS_MCE_A2DP_SUPPORT__) */

        case BT_SINK_SRV_MUSIC_DEVICE_PSE_HD: {
            pse_hd = (audio_src_srv_handle_t *) param;
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].handle) &&
                    (ctx->sink_dev[i].handle == pse_hd)) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }
            break;
        }
#ifdef MTK_AUDIO_MP3_ENABLED
        case BT_SINK_SRV_MUSIC_DEVICE_LOCAL: {
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                //bt_sink_srv_report_id("[sink][music]get local %d %x", 2, i, &(ctx->sink_dev[i]));
                if (ctx->sink_dev[i].mp3) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }
            break;
        }
#endif
        case BT_SINK_SRV_MUSIC_DEVICE_SP: {
            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if ((ctx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT)
                || (ctx->sink_dev[i].conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_DEVICE_AVRCP_BROWSE_HD: {
            p_hd = (uint32_t *)param;

            for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
                if (ctx->sink_dev[i].avrcp_browse_hd != BT_SINK_SRV_MUSIC_INVALID_HD
                        && ctx->sink_dev[i].avrcp_browse_hd == *p_hd) {
                    dev = &(ctx->sink_dev[i]);
                    break;
                }
            }

        }
        default:
            break;
    }

    bt_sink_srv_report_id("[sink][music]get_dev-dev: 0x%x, type: %d, param: 0x%x\n", 3,
                       dev, type, param);

    return dev;
}


void bt_sink_srv_music_reset_device(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_music_context_t *ctx = NULL;

    bt_sink_srv_assert(dev);
    bt_sink_srv_memset(dev, 0x00, sizeof(bt_sink_srv_music_device_t));
    dev->state = AUDIO_SRC_SRV_STATE_NONE;
    dev->target_state = AUDIO_SRC_SRV_STATE_NONE;
    dev->role = BT_A2DP_INVALID_ROLE;
#ifdef __BT_AWS_SUPPORT__
    dev->aws_role = BT_AWS_ROLE_NONE;
#endif

    /* Update device */
    ctx = bt_sink_srv_music_get_context();

    ctx->latest_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_USED, NULL);
    bt_sink_srv_music_set_focus_device(ctx->latest_dev);
}


void bt_sink_srv_music_set_focus_device(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_music_context_t *ctx = NULL;

    ctx = bt_sink_srv_music_get_context();

    ctx->focus_dev = dev;
}


void bt_sink_srv_music_update_run_device(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    bt_sink_srv_report_id("[sink][music]update_run_device--run: 0x%08x", 1, dev);
    ctx->run_dev = dev;

    if (dev) {
        bt_sink_srv_music_set_focus_device(dev);
    }
}


void bt_sink_srv_music_drv_play(void *param)
{
    bt_sink_srv_music_device_t *dev = (bt_sink_srv_music_device_t *)param;
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP == dev->handle->type) {
        bt_sink_srv_a2dp_drv_play(param);
    }
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
    else if (AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP == dev->handle->type) {
        bt_sink_srv_aws_mce_a2dp_drv_play(param);
    }
#endif
    else {
        bt_sink_srv_report_id("[sink][music]drv_play(error)--type: %d", 1, dev->handle->type);
    }
}


void bt_sink_srv_music_drv_stop(bt_sink_srv_music_device_t *dev, uint8_t aid)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    int32_t ret = 0;

#ifdef MTK_BT_SPEAKER_ENABLE
    if(BT_SINK_SRV_MUSIC_MODE_BROADCAST == bt_sink_srv_music_get_spk_mode()) {
        bt_timer_ext_stop(BT_SINK_SRV_A2DP_SEND_PLAY_IND_TIMER_ID);
    }
#endif

    if (ctx->run_dev == dev) {
        med_hd = &(dev->med_handle);
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP);
        ret = med_hd->stop(aid);
        bt_sink_srv_report_id("[sink][a2dp]drv_stop-ret: 0x%x", 1, ret);
#ifdef BT_SINK_DUAL_ANT_ENABLE
        bt_sink_srv_dual_ant_data_t notify;
        notify.type = BT_SINK_DUAL_ANT_TYPE_MUSIC;
        notify.music_info.a2dp_state = false;
        bt_sink_srv_dual_ant_notify(false, &notify);
#endif
        if (BT_CODEC_MEDIA_STATUS_OK == ret) {
            /* Remove DRV play flag */
            BT_SINK_SRV_REMOVE_FLAG(dev->op, BT_SINK_SRV_MUSIC_OP_DRV_PLAY);
            /* Remove play flag */
            BT_SINK_SRV_REMOVE_FLAG(dev->op, BT_SINK_SRV_MUSIC_REINIT_ON_PARTNER_LATER_JOIN_FLAG);
        } else {
            /* Error handle */
            bt_sink_srv_report_id("[sink][a2dp]drv_stop(error)--ret: %d", 1, ret);
        }
    }
}

void bt_sink_srv_music_clear_codec(bt_sink_srv_music_device_t *dev,uint8_t aid)
{
    int32_t ret = 0;
    
#if defined(__BT_AWS_SUPPORT__) || defined(__BT_AWS_MCE_A2DP_SUPPORT__)
    bt_sink_srv_aws_mce_a2dp_reset_n_flag();
#endif
    if (dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY) {
        BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP);
        /* Sync DRV stop, or codec close must wait DRV stop done(cost time) */
        bt_sink_srv_music_drv_stop(dev, aid);
    }
    if(!(dev->flag&BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC) // for agent
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
        && !(bt_sink_srv_music_get_context()->a2dp_eir.flag & BT_SINK_SRV_AWS_MCE_A2DP_REINIT_FLAG) // for partner
#endif
        && (BT_SINK_SRV_STATE_STREAMING == bt_sink_srv_get_state())) {
        bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
    }

    /* 2. Codec close */
    if (dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN) {
        ret = bt_sink_srv_ami_audio_stop(aid);
        if (ret != AUD_EXECUTION_SUCCESS) {
            /* Failed close codec */
            bt_sink_srv_report_id("[sink][a2dp]clear_codec(error)--ret: %d", 1, ret);
        } else {
            BT_SINK_SRV_REMOVE_FLAG(dev->op, BT_SINK_SRV_MUSIC_OP_CODEC_OPEN);
        }
    }

    /* 3. Transfer suspend SPK2 */

    /* 4. Clear flag */
    /* Clear wait start gpt timer */
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
    /* Clear wait start gpt timeout */
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_GPT_TIMEOUT);
}

void bt_sink_srv_music_stop(bt_sink_srv_music_device_t *dev, uint8_t aid)
{
    bt_sink_srv_assert(dev);
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    uint32_t gap_hd = bt_sink_srv_cm_get_gap_handle(&(dev->dev_addr));
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    bt_sink_srv_music_clear_codec(dev, aid);
#ifdef MTK_INITIAL_SYNC_BY_SAMPLE_INDEX_SUPPORT
    bt_sink_srv_a2dp_stop_timer(BT_SINK_SRV_STA_TIMER_ID);
    bt_sink_srv_music_clear_media_packet_list();
#endif
    bt_sink_srv_music_cancel_audio_play_en(dev);

    if (!(dev->flag&BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC)) {
        bt_sink_srv_music_update_run_device(NULL);
    }

    if(role == BT_AWS_MCE_ROLE_NONE || role == BT_AWS_MCE_ROLE_AGENT) {
        if(gap_hd) {
            bt_sink_srv_music_set_music_enable(gap_hd, BT_AVM_ROLE_AWS_MCE_AGENT, false);
        }
        uint32_t default_time_len = bt_gap_get_default_sniff_time_length();
        bt_gap_reset_sniff_timer(default_time_len);
        bt_sink_srv_report_id("[sink][a2dp]to set sniff timer to default:%d", 1, default_time_len);
    }else {
#if defined(__BT_AWS_SUPPORT__) || defined(__BT_AWS_MCE_A2DP_SUPPORT__)
        if(dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) {
            bt_sink_srv_music_set_music_enable(dev->aws_hd, BT_AVM_ROLE_AWS_MCE_PARTNER, false);
            ctx->ratio = BT_SINK_SRV_MUSIC_RATIO_MAGIC_CODE;
        }
#endif
        if(!(dev->flag&BT_SINK_SRV_MUSIC_FLAG_WAITING_START)) {
            BT_SINK_SRV_REMOVE_FLAG(dev->op, BT_SINK_SRV_MUSIC_OP_PLAY_IND);
        }
    }

    /* Clear done */
    bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_CODEC_CLEAR, NULL);
    avm_direct_clear_drift_parameters();

    if ((!(dev->conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT)) && (!(dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT))) {
        bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_UNAVAILABLE, NULL);
    } else {
        bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_READY, NULL);
    }
    if ((dev->flag & BT_SINK_SRV_MUSIC_FLAG_ADD_WAITING_LIST) && (NULL != audio_src_srv_get_runing_pseudo_device())) {
        bt_sink_srv_a2dp_add_waitinglist(dev->handle);
    }
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_ADD_WAITING_LIST);

    if (dev->handle && !(dev->conn_bit) &&
        dev->avrcp_hd == BT_SINK_SRV_MUSIC_INVALID_HD) {
        bt_sink_srv_a2dp_free_pseudo_handle(dev->handle);
        dev->handle = NULL;
        bt_sink_srv_music_reset_device(dev);
        if(ctx->context_flag & BT_SINK_SRV_CNTX_FLAG_POWER_OFF) {
            bt_sink_srv_ami_audio_close(aid);
            ctx->a2dp_aid = BT_SINK_SRV_INVALID_AID;
            ctx->aws_aid = BT_SINK_SRV_INVALID_AID;
            BT_SINK_SRV_REMOVE_FLAG(ctx->context_flag, BT_SINK_SRV_CNTX_FLAG_POWER_OFF);
        }
    }

    if(dev->flag & BT_SINK_SRV_MUSIC_NORMAL_STOP_FLAG) {
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_NORMAL_STOP_FLAG);
        if(((dev->flag & BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC) && dev->handle &&(role == BT_AWS_MCE_ROLE_NONE || role == BT_AWS_MCE_ROLE_AGENT))
            || (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAITING_START)) {
            BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAITING_START);
            bt_sink_srv_music_state_machine_handle(dev, BT_A2DP_START_STREAMING_IND, NULL);
        }
#ifdef FEATURE_NEW_MULTIPOINT_TRIAL
        else {
            if ((dev->a2dp_hd != BT_SINK_SRV_MUSIC_INVALID_HD)
                && (dev->conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT)) {
                bt_a2dp_suspend_streaming_response(dev->a2dp_hd, true);
            }
            bt_sink_srv_music_device_t* other_streaming_dev = bt_sink_srv_music_get_other_streaming_devices(dev);
            if(other_streaming_dev) {
                bt_sink_srv_music_state_machine_handle(other_streaming_dev, BT_A2DP_START_STREAMING_IND, NULL);
            }
        }
#endif
    } else {
        if (NULL == audio_src_srv_get_runing_pseudo_device()) {
            bt_sink_srv_report_id("[Sink][music] dev: 0x%x, a2dp_status: 0x%x", 2, dev, dev->a2dp_status);
            if ((BT_SINK_SRV_A2DP_STATUS_STREAMING == dev->a2dp_status) && dev->handle && (role == BT_AWS_MCE_ROLE_NONE || role == BT_AWS_MCE_ROLE_AGENT)) {
                bt_sink_srv_music_state_machine_handle(dev, BT_A2DP_START_STREAMING_IND, NULL);
            }
        }
    }

    bt_sink_srv_report_id("[Sink][music] music_stop",0);
}


void bt_sink_srv_music_fill_audio_src_callback(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_assert(handle);
    handle->play = bt_sink_srv_music_play_handle;
    handle->stop = bt_sink_srv_music_stop_handle;
    handle->suspend = bt_sink_srv_music_suspend_handle;
    handle->reject = bt_sink_srv_music_reject_handle;
    handle->exception_handle = bt_sink_srv_music_exception_handle;
}


int32_t bt_sink_srv_music_start_gpt_timer(hal_gpt_callback_t callback, void *user_data, uint32_t duration)
{
    hal_gpt_port_t gpt_port = HAL_GPT_MAX_PORT;
    hal_gpt_status_t gpt_ret = HAL_GPT_STATUS_ERROR_PORT_USED;

    if (hal_gpt_init(HAL_GPT_1) == HAL_GPT_STATUS_OK) {
        gpt_port = HAL_GPT_1;
    } else if (hal_gpt_init(HAL_GPT_2) == HAL_GPT_STATUS_OK) {
        gpt_port = HAL_GPT_2;
    } else {
        ;
    }

    if (gpt_port != HAL_GPT_MAX_PORT) {
        hal_gpt_register_callback(gpt_port,
            callback, user_data);
        //sink_loc_play_nclk_intra += duration;
        gpt_ret = hal_gpt_start_timer_us(gpt_port, duration - 2000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
    } else {
        bt_sink_srv_report_id("[sink][music]start_gpt_timer(error)--dur: %d", 1, duration);
    }

    bt_sink_srv_report_id("[sink][music]start_gpt_timer--ret: %d", 1, gpt_ret);

    return gpt_port;
}

void bt_sink_srv_music_fill_am_aud_param(bt_sink_srv_am_audio_capability_t  *aud_cap,
                                bt_a2dp_codec_capability_t *a2dp_cap, bt_a2dp_role_t role, uint16_t a2dp_mtu)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    memset(aud_cap, 0x00, sizeof(bt_sink_srv_am_audio_capability_t));
    aud_cap->type = A2DP;
    aud_cap->codec.a2dp_format.a2dp_codec.role = role;
    memcpy(&(aud_cap->codec.a2dp_format.a2dp_codec.codec_cap), a2dp_cap, sizeof(bt_a2dp_codec_capability_t));
    aud_cap->audio_stream_out.audio_device = BT_SINK_SRV_A2DP_OUTPUT_DEVICE;
    aud_cap->audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t)(ctx->vol_lev);
    ctx->last_volume = ctx->vol_lev;
    bt_sink_srv_report_id("agent open codec init vol is %d", 1, aud_cap->audio_stream_out.audio_volume);
    aud_cap->audio_stream_out.audio_mute = false;
    aud_cap->codec.a2dp_format.a2dp_codec.a2dp_mtu = a2dp_mtu;
}


void BT_A2DP_MAKE_SBC_CODEC(bt_a2dp_codec_capability_t *codec,
                            bt_a2dp_role_t role,
                            uint8_t min_bit_pool, uint8_t max_bit_pool,
                            uint8_t block_length, uint8_t subband_num,
                            uint8_t alloc_method, uint8_t sample_rate,
                            uint8_t channel_mode)
{
    do {
        codec->type = BT_A2DP_CODEC_SBC;
        codec->sep_type = role;
        codec->length = sizeof(bt_a2dp_sbc_codec_t);
        codec->codec.sbc.channel_mode = (channel_mode & 0x0F);
        codec->codec.sbc.sample_freq = (sample_rate & 0x0F);
        codec->codec.sbc.alloc_method = (alloc_method & 0x03);
        codec->codec.sbc.subbands = (subband_num & 0x03);
        codec->codec.sbc.block_len = (block_length & 0x0F);
        codec->codec.sbc.min_bitpool = (min_bit_pool & 0xFF);
        codec->codec.sbc.max_bitpool = (max_bit_pool & 0xFF);
    #ifdef __BT_SINK_SRV_A2DP_V13_SUPPORT__
        codec->delay_report = true;
        codec->sec_type = BT_A2DP_CP_SCMS_T;
    #endif
    } while (0);
}


void BT_A2DP_MAKE_AAC_CODEC(bt_a2dp_codec_capability_t *codec,
                            bt_a2dp_role_t role, bool vbr, uint8_t object_type,
                            uint8_t channels, uint16_t sample_rate,
                            uint32_t bit_rate)
{
    do {
        codec->type = BT_A2DP_CODEC_AAC;
        codec->sep_type = role;
        codec->length = sizeof(bt_a2dp_aac_codec_t);
        codec->codec.aac.object_type = object_type;
        codec->codec.aac.freq_h = ((sample_rate >> 4) & 0xFF);
        codec->codec.aac.reserved = 0x0;
        codec->codec.aac.channels = channels;
        codec->codec.aac.freq_l = (sample_rate & 0x0F);
        codec->codec.aac.br_h = ((bit_rate >> 16) & 0x7F);
        codec->codec.aac.vbr = vbr;
        codec->codec.aac.br_m = ((bit_rate >> 8) & 0xFF);
        codec->codec.aac.br_l = (bit_rate & 0xFF);
    #ifdef __BT_SINK_SRV_A2DP_V13_SUPPORT__
        codec->delay_report = true;
        codec->sec_type = BT_A2DP_CP_SCMS_T;
    #endif
    } while (0);
}

void BT_A2DP_MAKE_VENDOR_CODEC(bt_a2dp_codec_capability_t *codec,
                            bt_a2dp_role_t role, uint32_t vendor_id, uint16_t
                            codec_id, uint8_t sample_frequency, uint8_t
                            channel_mode)
{
    bt_a2dp_vendor_codec_t *vendor = &codec->codec.vendor;
    do {
        codec->type = BT_A2DP_CODEC_VENDOR;
        codec->sep_type = role;
        codec->length = 8;
        vendor->vendor_id = vendor_id;
        vendor->codec_id = codec_id;
        vendor->value[0] = 0x3f&sample_frequency;
        vendor->value[1] = 0x07&channel_mode;
    #ifdef __BT_SINK_SRV_A2DP_V13_SUPPORT__
        codec->delay_report = true;
        codec->sec_type = BT_A2DP_CP_SCMS_T;
    #endif
    } while (0);
}

#ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
void BT_A2DP_MAKE_VENDOR_CODEC_2(bt_a2dp_codec_capability_t *codec,
                            bt_a2dp_role_t role, uint32_t vendor_id, uint16_t
                            codec_id, uint8_t sample_frequency, uint8_t
                            mode)
{
    bt_a2dp_vendor_codec_t *vendor = &codec->codec.vendor;
    do {
        codec->type = BT_A2DP_CODEC_VENDOR;
        codec->sep_type = role;
        codec->length = 9;
        vendor->vendor_id = vendor_id;
        vendor->codec_id = codec_id;
        vendor->value[0] = 0x3f&sample_frequency;
        vendor->value[1] = mode;
        vendor->value[2] = 0x00;
    #ifdef __BT_SINK_SRV_A2DP_V13_SUPPORT__
        codec->delay_report = true;
        codec->sec_type = BT_A2DP_CP_SCMS_T;
    #endif
    } while (0);
}
#endif

void BT_A2DP_CONVERT_SBC_CODEC(bt_codec_capability_t *dst_codec,
                               bt_a2dp_codec_capability_t *src_codec)
{
    dst_codec->type = BT_A2DP_CODEC_SBC;
    dst_codec->codec.sbc.min_bit_pool = src_codec->codec.sbc.min_bitpool;
    dst_codec->codec.sbc.max_bit_pool = src_codec->codec.sbc.max_bitpool;
    dst_codec->codec.sbc.block_length = src_codec->codec.sbc.block_len;
    dst_codec->codec.sbc.subband_num = src_codec->codec.sbc.subbands;
    dst_codec->codec.sbc.alloc_method = src_codec->codec.sbc.alloc_method;
    dst_codec->codec.sbc.sample_rate = src_codec->codec.sbc.sample_freq;
    dst_codec->codec.sbc.channel_mode = src_codec->codec.sbc.channel_mode;
    bt_sink_srv_report_id("[sink][am][med]CONVERT_SBC--min_pool: %d, max_pool: %d, b_len: %d, sub_num: %d, all_met: %d, samp_rate: %d, ch_m: %d", 7,
                       dst_codec->codec.sbc.min_bit_pool, dst_codec->codec.sbc.max_bit_pool,
                       dst_codec->codec.sbc.block_length, dst_codec->codec.sbc.subband_num,
                       dst_codec->codec.sbc.alloc_method, dst_codec->codec.sbc.sample_rate,
                       dst_codec->codec.sbc.channel_mode);
}


void BT_A2DP_CONVERT_AAC_CODEC(bt_codec_capability_t *dst_codec,
                               bt_a2dp_codec_capability_t *src_codec)
{
    dst_codec->type = BT_A2DP_CODEC_AAC;
    dst_codec->codec.aac.vbr = src_codec->codec.aac.vbr;
    dst_codec->codec.aac.object_type = src_codec->codec.aac.object_type;
    dst_codec->codec.aac.channels = src_codec->codec.aac.channels;
    dst_codec->codec.aac.sample_rate = (src_codec->codec.aac.freq_h << 4) | (src_codec->codec.aac.freq_l);
    dst_codec->codec.aac.bit_rate = (src_codec->codec.aac.br_h << 16) | (src_codec->codec.aac.br_m << 8) | (src_codec->codec.aac.br_l);
    bt_sink_srv_report_id("[sink][am][med]CONVERT_AAC--vbr: %d, object_type: %d, channels: %d, sample_rate: %d, bit_rate: %d", 5,
                       dst_codec->codec.aac.vbr, dst_codec->codec.aac.object_type,
                       dst_codec->codec.aac.channels, dst_codec->codec.aac.sample_rate,
                       dst_codec->codec.aac.bit_rate);
}


void BT_A2DP_CONVERT_VENDOR_CODEC(bt_codec_capability_t *dst_codec,
                               bt_a2dp_codec_capability_t *src_codec)
{
    dst_codec->type = BT_A2DP_CODEC_VENDOR;
    if (src_codec->codec.vendor.codec_id == 0x00AA)
    {
        dst_codec->codec.vendor.channels = (src_codec->codec.vendor.value[1] & BT_A2DP_VENDOR_CODEC_CHANNEL_MODE_MASK);
        dst_codec->codec.vendor.sample_rate = (src_codec->codec.vendor.value[0] & BT_A2DP_VENDOR_CODEC_SAMPLE_RATE_MASK);
    }
	#ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
    else
    {
        dst_codec->codec.vendor.channels = 0x01;
        dst_codec->codec.vendor.sample_rate = (src_codec->codec.vendor.value[0] & BT_A2DP_VENDOR_2_CODEC_SAMPLE_RATE_MASK);
    }
	#endif
    dst_codec->codec.vendor.codec_id = src_codec->codec.vendor.codec_id;

    //dst_codec->codec.vendor.bit_rate = (src_codec->codec.vendor.br_h << 16) | (src_codec->codec.vendor.br_m << 8) | (src_codec->codec.vendor.br_l);
    bt_sink_srv_report_id("[sink][am][med]CONVERT_vendor-- vendor id: %d, codec id: %d, channels: %d, sample_rate: %d", 4,
                       dst_codec->codec.vendor.vendor_id,dst_codec->codec.vendor.codec_id,
                       dst_codec->codec.vendor.channels, dst_codec->codec.vendor.sample_rate
                       );
}


uint64_t bt_sink_srv_music_convert_btaddr_to_devid(bt_bd_addr_t *bd_addr)
{
    uint64_t dev_id = 0;
    uint32_t hdev = 0, ldev = 0;
    int32_t i = 0;
    uint8_t addr[16] = {0};

    bt_sink_srv_assert(bd_addr);
    bt_sink_srv_memcpy(addr, bd_addr, sizeof(bt_bd_addr_t));
    for (i = 0; i < BT_BD_ADDR_LEN; ++i) {
        dev_id = ((dev_id << 8) | (addr[i]));
    }

    hdev = (dev_id >> 32 & 0xffffffff);
    ldev = (dev_id & 0xffffffff);

    bt_sink_srv_report_id("[music]convert_btaddr_to_devid--hdev: 0x%x, ldev: 0x%x", 2, hdev, ldev);

    return dev_id;
}


uint8_t bt_sink_srv_get_vol_local2bt(uint8_t vol, uint8_t local_level, uint8_t bt_level)
{
    return (uint8_t)(floor(vol * bt_level / local_level + 0.5));
}


uint8_t bt_sink_srv_get_vol_bt2local(uint8_t vol, uint8_t local_level, uint8_t bt_level)
{
    return (uint8_t)(floor(vol * local_level / bt_level + 0.5));
}


bt_sink_srv_audio_rtp_header_t *bt_sink_srv_music_get_media_info(uint32_t *end_addr, void *start_addr, uint32_t pre_sn, bool is_agent)
{
    bt_sink_srv_audio_pcb_type_t *pcb_data = (bt_sink_srv_audio_pcb_type_t *)start_addr;
    *end_addr = (uint32_t)((uint8_t *)start_addr + pcb_data->size + sizeof(bt_sink_srv_audio_pcb_type_t));
    bt_sink_srv_audio_rtp_header_t * rtp_hd = (bt_sink_srv_audio_rtp_header_t *)
        ((uint8_t *)start_addr + sizeof(bt_sink_srv_audio_pcb_type_t));
    /*1 means this packet is write OK on agent/partner, 2 is always ok in agent, and maybe lost packet on partner*/
    if((pcb_data->state != BT_SINK_SRV_PCB_STATE_COMPLETE)
        && (pcb_data->state != BT_SINK_SRV_PCB_STATE_INCOMPLETE)) {
        rtp_hd = NULL;
    }
    if((pcb_data->state == BT_SINK_SRV_PCB_STATE_INCOMPLETE) && (!is_agent)) {
        if((pre_sn != BT_SINK_SRV_INVALID_SEQUENCE_NUMBER) && (rtp_hd->seqno != (pre_sn + 1))) {
            rtp_hd = NULL;
        }
    }
    bt_sink_srv_report_id("[music]****start_addr: 0x%08x, end_addr: 0x%08x, pcb_state:%d******", 3,
        start_addr, *end_addr, pcb_data->state);
    return rtp_hd;
}


void bt_sink_srv_music_cancel_audio_play_en(bt_sink_srv_music_device_t *a2dp_dev)
{
    bt_sink_srv_assert(a2dp_dev);
    uint32_t gap_hd = bt_sink_srv_cm_get_gap_handle(&(a2dp_dev->dev_addr));
    bt_timer_ext_t* timer_ret = bt_timer_ext_find(BT_SINK_SRV_SET_AUDIO_PLAY_EN_TIMER);
    if(gap_hd && timer_ret) {
        bt_timer_ext_stop(BT_SINK_SRV_SET_AUDIO_PLAY_EN_TIMER);
        avm_direct_cancel_audio_play_en(gap_hd);
    } else {
        bt_sink_srv_report_id("[music] gap_hd is 0 or audio play en timer due", 0);
    }
}

static void bt_sink_srv_music_audio_play_en_timer_handler(uint32_t timer_id, uint32_t data)
{
    bt_sink_srv_report_id("[music]Audio play en timer due", 0);
}

void bt_sink_srv_music_set_audio_play_en_timer(bt_clock_t *cur_clock, bt_clock_t *play_clock)
{
    uint32_t dur = (((play_clock->nclk-cur_clock->nclk)*625>>1) + (play_clock->nclk_intra - cur_clock->nclk_intra))/1000 + 1;

    bt_timer_ext_start(BT_SINK_SRV_SET_AUDIO_PLAY_EN_TIMER, 0,
        dur, bt_sink_srv_music_audio_play_en_timer_handler);
    bt_sink_srv_report_id("[music]play nclkdur:%d", 1, dur);

}


void bt_sink_srv_music_update_base_parameters_to_dsp(int16_t drift_value, uint32_t nclk, uint32_t asi_base, uint32_t asi_cur)
{
    hal_audio_write_audio_drift_val(drift_value);
    hal_audio_write_audio_anchor_clk(nclk);
    hal_audio_write_audio_asi_cur(asi_cur);
    hal_audio_write_audio_asi_base(asi_base);
    bt_sink_srv_report_id("[music]updte parameters, drift:%d, pta-nclk:0x%08x, asi_base:0x%08x, asi_cur:0x%08x", 4, drift_value, nclk, asi_base, asi_cur);
}

bt_status_t bt_sink_srv_music_set_sink_latency(uint32_t latency_value, bool is_initial_value)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_status_t ret = avm_direct_set_sink_latency(latency_value);
    if(ret == BT_STATUS_SUCCESS) {
        bt_sink_srv_ami_set_a2dp_sink_latency(latency_value);
        if(is_initial_value) {
            ctx->init_sink_latency = latency_value;
        }
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

uint32_t bt_sink_srv_music_get_sink_latency(void)
{
    uint32_t sink_latency = avm_direct_get_sink_latency();
    return sink_latency;
}

bt_avrcp_operation_id_t bt_sink_srv_music_get_play_pause_action(bt_sink_srv_music_device_t *dev)
{
    bt_avrcp_operation_id_t action_id = 0;
    if(dev->last_play_pause_action == BT_AVRCP_OPERATION_ID_PLAY) {
        action_id = BT_AVRCP_OPERATION_ID_PAUSE;
    } else if (dev->last_play_pause_action == BT_AVRCP_OPERATION_ID_PAUSE) {
        action_id = BT_AVRCP_OPERATION_ID_PLAY;
    } else if (dev->avrcp_status == BT_AVRCP_STATUS_PLAY_PLAYING) {
        action_id = BT_AVRCP_OPERATION_ID_PAUSE;
    } else if (dev->avrcp_status == BT_AVRCP_STATUS_PLAY_PAUSED
        || dev->avrcp_status == BT_AVRCP_STATUS_PLAY_STOPPED) {
        action_id = BT_AVRCP_OPERATION_ID_PLAY;
    } else if (dev->avrcp_status == BT_SINK_SRV_MUSIC_AVRCP_INVALID_STATUS && dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_STREAMING) {
        action_id = BT_AVRCP_OPERATION_ID_PAUSE;
    } else if (dev->avrcp_status == BT_SINK_SRV_MUSIC_AVRCP_INVALID_STATUS && dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_SUSPEND) {
        action_id = BT_AVRCP_OPERATION_ID_PLAY;
    } else if ((dev->avrcp_status == BT_AVRCP_STATUS_PLAY_FWD_SEEK || dev->avrcp_status == BT_AVRCP_STATUS_PLAY_REV_SEEK)
        && dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_STREAMING) {
        action_id = BT_AVRCP_OPERATION_ID_PAUSE;
    } else if ((dev->avrcp_status == BT_AVRCP_STATUS_PLAY_FWD_SEEK || dev->avrcp_status == BT_AVRCP_STATUS_PLAY_REV_SEEK)
        && dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_SUSPEND) {
        action_id = BT_AVRCP_OPERATION_ID_PLAY;
    }
    return action_id;
}


void bt_sink_srv_music_decide_play_pause_action(bt_sink_srv_music_device_t *dev)
{
    bt_avrcp_operation_id_t action_id = 0;
    bt_status_t ret = BT_STATUS_SUCCESS;
    uint16_t conn_bit = 0;

    if(dev && (dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
        conn_bit = dev->conn_bit;
        action_id = bt_sink_srv_music_get_play_pause_action(dev);

        if(action_id) {
            ret = bt_sink_srv_avrcp_send_play_pause_command(dev, action_id);
            if(ret == BT_STATUS_SUCCESS) {
                dev->last_wear_action = BT_SINK_SRV_INVALID_LAST_PLAY_PAUSE_ACTION;
                dev->last_play_pause_action = action_id;
            }
        } else {
            bt_sink_srv_assert(0 && "action_id 0");
        }
    }
    bt_sink_srv_report_id("[music]bt_sink_srv_music_decide_play_pause_action, dev:0x%08x, conn_bit:0x%04x, action_id:0x%02x, ret:0x%08x",
        4, dev, conn_bit, action_id, ret);
}

void bt_sink_srv_music_avrcp_status_change_notify(bt_bd_addr_t *remote_addr, bt_avrcp_status_t avrcp_status)
{
    bt_sink_srv_event_param_t *params = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_event_param_t));
    bt_sink_srv_assert(params);
    bt_sink_srv_memcpy(&params->avrcp_status_change.address, remote_addr, sizeof(bt_bd_addr_t));
    params->avrcp_status_change.avrcp_status= avrcp_status;

    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AVRCP_STATUS_CHANGE, params, sizeof(bt_sink_srv_event_param_t));
    bt_sink_srv_memory_free(params);
}

void bt_sink_srv_music_update_audio_buffer(uint16_t buffer_size)
{
    bt_avm_share_buffer_info_t audio_buf = {0};
    n9_dsp_share_info_t *a2dp_share_info = hal_audio_query_bt_audio_dl_share_info();
    //a2dp_share_info->length = buffer_size;
    audio_buf.a2dp_address = (uint32_t)a2dp_share_info;
    audio_buf.sco_dl_address = (uint32_t)hal_audio_query_bt_voice_dl_share_info();
    audio_buf.sco_up_address = (uint32_t)hal_audio_query_bt_voice_ul_share_info();
    audio_buf.clock_mapping_address = (uint32_t)hal_audio_query_rcdc_share_info();
    bt_status_t ret = bt_avm_set_share_buffer(&audio_buf);
    bt_sink_srv_report_id("update audio buffer, buffer_size:0x%04x, ret:%d", 2, buffer_size, ret);
}

#ifdef BT_SINK_SRV_MUSIC_ADD_GPIO
void bt_sink_srv_music_init_gpio(hal_gpio_pin_t port_num)
{
    hal_gpio_init(port_num);
    hal_pinmux_set_function(port_num, 0);
    hal_gpio_set_direction(port_num, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(port_num, HAL_GPIO_DATA_LOW);
}
#endif

bt_status_t bt_sink_srv_music_set_mute(bool is_mute)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_am_result_t ret = 0;
    uint8_t aid = 0;
    if(role == BT_AWS_MCE_ROLE_NONE || role == BT_AWS_MCE_ROLE_AGENT) {
        aid = ctx->a2dp_aid;
    } else {
        aid = ctx->aws_aid;
    }
    ret = bt_sink_srv_ami_audio_set_mute(aid, is_mute, STREAM_OUT);
    bt_sink_srv_report_id("[music]bt_sink_srv_music_set_mute, run_dev:0x%08x, aid:0x%02x, mute:%d, ret:0x%08x",
        4, ctx->run_dev, aid, is_mute, ret);

    return BT_STATUS_SUCCESS;
}

bt_sink_srv_music_mode_t bt_sink_srv_music_get_mode(bt_bd_addr_t *remote_addr)
{
    bt_sink_srv_music_mode_t music_mode = BT_SINK_SRV_MUSIC_DATA_NOT_FOUND;
    bt_sink_srv_music_get_nvdm_data(remote_addr, BT_SINK_SRV_MUSIC_DATA_MODE, &music_mode);
    return music_mode;
}

bt_status_t bt_sink_srv_music_set_mode(bt_bd_addr_t *remote_addr, bt_sink_srv_music_mode_t mode)
{
    return bt_sink_srv_music_set_nvdm_data(remote_addr, BT_SINK_SRV_MUSIC_DATA_MODE, &mode);
}

#ifdef MTK_BT_SPEAKER_ENABLE
static void bt_sink_srv_music_get_speaker_volume_data(uint8_t *volume_data)
{
    uint16_t vol_client = 0;
    if(bt_connection_manager_device_local_info_get_local_music_volume(&vol_client) && ((vol_client & 0xff00) == BT_SINK_SRV_A2DP_MAGIC_CODE)) {
        *volume_data = (vol_client & 0x00ff);
    } else {
        *volume_data = bt_sink_srv_ami_get_a2dp_default_volume_level();
        vol_client = (BT_SINK_SRV_A2DP_MAGIC_CODE | (*volume_data));
        bt_connection_manager_device_local_info_set_local_music_volume(&vol_client);
    }
}
#endif

bool bt_sink_srv_music_get_a2dp_nvdm_data(bt_bd_addr_t *bt_addr, void *data_p, uint32_t size)
{
    bool result = false;
#ifdef BT_SINK_SRV_A2DP_STORAGE_SIZE
    // Warnning: Due to the task stack limite, record should not increase more than 100 bytes
    bt_device_manager_db_remote_profile_info_t record;
    bt_sink_srv_memset(&record, 0, sizeof(bt_device_manager_db_remote_profile_info_t));
    bt_sink_srv_assert(size <= BT_SINK_SRV_A2DP_STORAGE_SIZE);
    if (NULL != bt_addr && NULL != data_p &&
            BT_STATUS_SUCCESS == bt_device_manager_remote_find_profile_info((*bt_addr), &record)) {
        bt_sink_srv_memcpy(data_p, record.a2dp_info, size);
        result = true;
    }
#endif
    return result;
}

bool bt_sink_srv_music_set_a2dp_nvdm_data(bt_bd_addr_t *bt_addr, void *data_p, uint32_t size)
{
    bool result = false;
#ifdef BT_SINK_SRV_A2DP_STORAGE_SIZE
    // Warnning: Due to the task stack limite, record should not increase more than 100 bytes
    bt_device_manager_db_remote_profile_info_t record;
    bt_sink_srv_memset(&record, 0, sizeof(bt_device_manager_db_remote_profile_info_t));
    bt_sink_srv_assert(size <= BT_SINK_SRV_A2DP_STORAGE_SIZE);
    if (NULL != bt_addr && NULL != data_p) {
        bt_device_manager_remote_find_profile_info((*bt_addr), &record);
        bt_sink_srv_memcpy(record.a2dp_info, data_p, size);
        result = bt_device_manager_remote_update_profile_info((*bt_addr), &record);
    }
#endif
    return result;
}


bt_status_t bt_sink_srv_music_get_nvdm_data(bt_bd_addr_t *remote_addr, bt_sink_srv_music_data_type_t data_type, void *data)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_sink_srv_music_stored_data_t dev_db;
    bt_sink_srv_music_device_t *dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_AVRCP, remote_addr);
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_sink_srv_memset(&dev_db, 0, sizeof(bt_sink_srv_music_stored_data_t));
    if (role == BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_NONE) {
        bt_sink_srv_music_get_a2dp_nvdm_data(remote_addr, &dev_db, sizeof(dev_db));
        switch (data_type) {
            case BT_SINK_SRV_MUSIC_DATA_VOLUME:
            {
                uint8_t *volume_data = (uint8_t *)data;
                bt_sink_srv_report_id("[sink][music]music_vol:0x%04x", 1, dev_db.music_volume);
                if ((dev_db.music_volume& 0xff00) == BT_SINK_SRV_A2DP_MAGIC_CODE) {
                    /* use storge volume value */
                    *volume_data = (dev_db.music_volume& 0x00ff);
                } else {
                    /* use dedefault volume value and update it */
                    if(dev && dev->volume_change_status) {
                        *volume_data = bt_sink_srv_get_default_volume_level(true);
                    } else {
                        *volume_data = bt_sink_srv_get_default_volume_level(false);
                    }
                    dev_db.music_volume= (BT_SINK_SRV_A2DP_MAGIC_CODE | (*volume_data));
                    bt_sink_srv_music_set_a2dp_nvdm_data(remote_addr, &dev_db, sizeof(dev_db));
                }
                bt_sink_srv_report_id("[sink][music]get_nvdm_data, vol_data:%d", 1, *volume_data);
                break;
            }
            case BT_SINK_SRV_MUSIC_DATA_MODE:
            {
                uint8_t *mode_data = (uint8_t *)data;
                if ((dev_db.music_mode & 0xf0) == BT_SINK_SRV_MUSIC_MODE_MAGIC_NUMBER) {
                    /* use storge volume value */
                    *mode_data = (dev_db.music_mode& 0x0f);
                } else {
                    /* use dedefault volume value and update it */
                    *mode_data = BT_SINK_SRV_MUSIC_NORMAL_MODE;
                    dev_db.music_mode= (BT_SINK_SRV_MUSIC_MODE_MAGIC_NUMBER | (*mode_data));
                    bt_sink_srv_music_set_a2dp_nvdm_data(remote_addr, &dev_db, sizeof(dev_db));
                }
                bt_sink_srv_report_id("[sink][music]get_nvdm_data, mode_data:%d", 1, *mode_data);
                break;
            }
            case BT_SINK_SRV_MUSIC_DATA_LOCAL_ASI_FLAG:
            {
                uint8_t *local_asi_flag_data = (uint8_t *)data;
                *local_asi_flag_data = dev_db.local_asi_flag;
                bt_sink_srv_report_id("[sink][music]get_nvdm_data, local_asi_flag:%d", 1, *local_asi_flag_data);
                break;
            }
            default:
                break;
        }
    }
#ifdef MTK_BT_SPEAKER_ENABLE
    else if(role == BT_AWS_MCE_ROLE_CLINET && data_type == BT_SINK_SRV_MUSIC_DATA_VOLUME) {
        bt_sink_srv_music_get_speaker_volume_data((uint8_t *)data);
    }
#endif
    else {
        ret = BT_STATUS_FAIL;
    }
    bt_sink_srv_report_id("[sink][music]get_nvdm_data, data_type:%d, bt_addr:0x%08x, ret:%d, role:%d",
        4, data_type, (uint32_t)*remote_addr, ret, role);
    return ret;
}

bt_status_t bt_sink_srv_music_set_nvdm_data(bt_bd_addr_t *remote_addr, bt_sink_srv_music_data_type_t data_type, void *data)
{
    bt_sink_srv_music_stored_data_t dev_db;
    bt_sink_srv_memset(&dev_db, 0, sizeof(bt_sink_srv_music_stored_data_t));
    bt_status_t ret = BT_STATUS_SUCCESS;
    bool get_data_ret = bt_sink_srv_music_get_a2dp_nvdm_data(remote_addr, &dev_db, sizeof(dev_db));

    switch (data_type) {
        case BT_SINK_SRV_MUSIC_DATA_VOLUME:
        {
            uint8_t *vol_data = (uint8_t *)data;
            dev_db.music_volume = (*vol_data) | BT_SINK_SRV_A2DP_MAGIC_CODE;
            bt_sink_srv_report_id("[sink][music]set_nvdm_data, vol_data:%d", 1, *vol_data);
            break;
        }
        case BT_SINK_SRV_MUSIC_DATA_MODE:
        {
            uint8_t *mode_data = (uint8_t *)data;
            dev_db.music_mode = (*mode_data) | BT_SINK_SRV_MUSIC_MODE_MAGIC_NUMBER;
            bt_sink_srv_report_id("[sink][music]set_nvdm_data, mode_data:%d", 1, *mode_data);
            break;
        }
        case BT_SINK_SRV_MUSIC_DATA_LOCAL_ASI_FLAG:
        {
            uint8_t *local_asi_flag_data = (uint8_t *)data;
            dev_db.local_asi_flag = *local_asi_flag_data;
            bt_sink_srv_report_id("[sink][music]set_nvdm_data, local_asi_flag:%d", 1, *local_asi_flag_data);
            break;
        }
    }
    bt_sink_srv_music_set_a2dp_nvdm_data(remote_addr, &dev_db, sizeof(dev_db));
    bt_sink_srv_report_id("[sink][music]set_nvdm_data, data_type:%d, bt_addr:0x%08x, ret:%d, get_data_ret:%d", 4, data_type, (uint32_t)(*remote_addr), ret, get_data_ret);
    return ret;
}

uint32_t bt_sink_srv_get_latency_by_mode(bt_sink_srv_music_mode_t music_mode, bool is_vendor_codec, bool is_extend, bool is_speaker)
{
    uint32_t music_latency_size = sizeof(bt_sink_srv_music_latency_t);
    uint32_t latency_val = 0;
    bt_sink_srv_music_latency_t music_latency = {
        .reserved = 0,
        .normal_latency = 140,
        .normal_latency_extend = 400,
        .game_latency = 100,
        .game_latency_extend = 100,
        .vendor_latency = 240,
        .vendor_latency_extend = 240,
        .vendor_game_latency = 240,
        .vendor_game_latency_extend = 240,
        .speaker_latency = 190,
        .speaker_latency = 400,
    };

    nvkey_status_t ret = nvkey_read_data(NVKEYID_BT_MUSIC_LATENCY_SETTING, (uint8_t *)(&music_latency), &music_latency_size);
    if(!is_speaker) {
        if(music_mode != BT_SINK_SRV_MUSIC_GAME_MODE){
            if (!is_vendor_codec && !is_extend) {
                latency_val = music_latency.normal_latency;
            } else if (!is_vendor_codec && is_extend) {
                latency_val = music_latency.normal_latency_extend;
            } else if(is_vendor_codec && !is_extend) {
                latency_val = music_latency.vendor_latency;
            } else if(is_vendor_codec && is_extend) {
                latency_val = music_latency.vendor_latency_extend;
            }
        } else {
            if (!is_vendor_codec && !is_extend) {
                latency_val = music_latency.game_latency;
            } else if (!is_vendor_codec && is_extend) {
                latency_val = music_latency.game_latency_extend;
            } else if(is_vendor_codec && !is_extend) {
                latency_val = music_latency.vendor_game_latency;
            } else if(is_vendor_codec && is_extend) {
                latency_val = music_latency.vendor_game_latency_extend;
            }
        }
    } else {
        if (!is_extend) {
            latency_val = music_latency.speaker_latency;
        } else {
            latency_val = music_latency.speaker_latency_extend;
        }
    }
    latency_val = latency_val*1000;
    bt_sink_srv_report_id("[sink][music]get_music_latency, is_vendor_codec:%d, is_extend:%d, latency_val:%d, ret:%d", 4,
        is_vendor_codec, is_extend, latency_val, ret);
    return latency_val;

}

uint32_t bt_sink_srv_get_latency(bt_bd_addr_t *remote_addr, bool is_vendor_codec, bool is_extend, bool is_speaker)
{
    bt_sink_srv_assert(remote_addr);
    bt_sink_srv_music_mode_t music_mode = BT_SINK_SRV_MUSIC_NORMAL_MODE;
    music_mode = bt_sink_srv_music_get_mode(remote_addr);
    uint32_t music_latency_size = sizeof(bt_sink_srv_music_latency_t);
    uint32_t latency_val = 0;
    bt_sink_srv_music_latency_t music_latency = {
        .reserved = 0,
        .normal_latency = 140,
        .normal_latency_extend = 400,
        .game_latency = 100,
        .game_latency_extend = 100,
        .vendor_latency = 240,
        .vendor_latency_extend = 240,
        .vendor_game_latency = 240,
        .vendor_game_latency_extend = 240,
        .speaker_latency = 190,
        .speaker_latency = 400,
    };

    nvkey_status_t ret = nvkey_read_data(NVKEYID_BT_MUSIC_LATENCY_SETTING, (uint8_t *)(&music_latency), &music_latency_size);
    if(!is_speaker) {
        if(music_mode != BT_SINK_SRV_MUSIC_GAME_MODE){
            if (!is_vendor_codec && !is_extend) {
                latency_val = music_latency.normal_latency;
            } else if (!is_vendor_codec && is_extend) {
                latency_val = music_latency.normal_latency_extend;
            } else if(is_vendor_codec && !is_extend) {
                latency_val = music_latency.vendor_latency;
            } else if(is_vendor_codec && is_extend) {
                latency_val = music_latency.vendor_latency_extend;
            }
        } else {
            if (!is_vendor_codec && !is_extend) {
                latency_val = music_latency.game_latency;
            } else if (!is_vendor_codec && is_extend) {
                latency_val = music_latency.game_latency_extend;
            } else if(is_vendor_codec && !is_extend) {
                latency_val = music_latency.vendor_game_latency;
            } else if(is_vendor_codec && is_extend) {
                latency_val = music_latency.vendor_game_latency_extend;
            }
        }
    } else {
        if (!is_extend) {
            latency_val = music_latency.speaker_latency;
        } else {
            latency_val = music_latency.speaker_latency_extend;
        }
    }
    latency_val = latency_val*1000;
    bt_sink_srv_report_id("[sink][music]get_music_latency, bt_addr:0x%08x, is_vendor_codec:%d, is_extend:%d, latency_val:%d, ret:%d", 5,
        (uint32_t)(*remote_addr), is_vendor_codec, is_extend, latency_val, ret);
    return latency_val;
}

uint8_t default_bt_sink_srv_get_default_volume_level(bool support_absolute_volume)
{
    uint8_t default_vol = bt_sink_srv_ami_get_a2dp_default_volume_level();
    if(!support_absolute_volume) {
        default_vol = bt_sink_srv_ami_get_a2dp_max_volume_level();
    }
    bt_sink_srv_report_id("[sink][music]default_bt_sink_srv_get_default_volume_level, absolute_volume:%d, default_vol:%d",
        2, support_absolute_volume, default_vol);
    return default_vol;
}

#ifdef MTK_INITIAL_SYNC_BY_SAMPLE_INDEX_SUPPORT
bt_status_t bt_sink_srv_music_fill_recevied_media_data(bt_sink_srv_music_data_info_t *media_data)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_status_t ret = BT_STATUS_FAIL;
    if(ctx->packet_count < BT_SINK_SRV_MAX_OPTIMIZE_MEDIA_PACKET_COUNT) {
        bt_sink_srv_memcpy(&ctx->media_data_list[ctx->packet_count], media_data, sizeof(bt_sink_srv_music_data_info_t));
        ctx->packet_count++;
        ret = BT_STATUS_SUCCESS;
    }
    bt_sink_srv_report_id("[sink][a2dp]ret:0x%08x, asi:0x%04x, rec_nclk:0x%08x, count:%d", 4,
        ret,ctx->media_data_list[ctx->packet_count-1].asi, ctx->media_data_list[ctx->packet_count-1].clock.nclk, ctx->packet_count);
    return ret;
}

void bt_sink_srv_music_clear_media_packet_list()
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    ctx->packet_count = 0;
}

void bt_sink_srv_music_trigger_play(uint32_t gap_handle, bt_sink_srv_music_device_t *dev, bt_clock_t *pta, uint32_t asi_base, uint32_t asi)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_media_handle_t *med_hd = dev->med_handle.med_hd;

    int16_t drift_value = avm_direct_get_drift();
    bt_sink_srv_music_update_base_parameters_to_dsp(drift_value, pta->nclk, asi_base, asi);
    bt_status_t ret = bt_avm_set_audio_tracking_time(gap_handle, BT_AVM_TYPE_A2DP, pta);

    bool is_speaker = ((BT_SINK_SRV_MUSIC_MODE_BROADCAST == bt_sink_srv_music_get_spk_mode()) ? true : false);
    bool vendor_codec = ((dev->codec.type == BT_A2DP_CODEC_VENDOR) ? true : false);
    bt_sink_srv_music_mode_t music_mode = BT_SINK_SRV_MUSIC_NORMAL_MODE;

    if(role == BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_NONE) {
        music_mode = bt_sink_srv_music_get_mode(&(dev->dev_addr));
    }
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
    else {
        bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
        music_mode = ((ctx->a2dp_eir.flag & BT_SINK_SRV_AWS_MCE_A2DP_ALC_FLAG) ? BT_SINK_SRV_MUSIC_GAME_MODE : BT_SINK_SRV_MUSIC_NORMAL_MODE);
    }
#endif
    uint32_t extend_latency = bt_sink_srv_get_latency_by_mode(music_mode, vendor_codec, true, is_speaker);
    uint32_t normal_latency = bt_sink_srv_music_get_sink_latency();

    med_hd->set_gaming_mode_flag(med_hd, ((music_mode == BT_SINK_SRV_MUSIC_GAME_MODE) ? true : false));
    med_hd->set_alc_mode_flag(med_hd, ((normal_latency == extend_latency) ? true : false));
    med_hd->set_start_time_stamp(med_hd, asi);
    med_hd->set_start_asi(med_hd, asi);
    uint32_t nclk_intra = pta->nclk_intra;
    med_hd->set_start_bt_clk(med_hd, pta->nclk, nclk_intra);

    BT_SINK_SRV_GPIO_SET_OUTPUT(HAL_GPIO_38, HAL_GPIO_DATA_HIGH);
    bt_sink_srv_music_drv_play(dev);
    BT_SINK_SRV_GPIO_SET_OUTPUT(HAL_GPIO_38, HAL_GPIO_DATA_LOW);
    bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_PLAYING, NULL);
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_N_PKT_DONE);
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC);
    bt_avm_a2dp_media_info_t play_info;
    play_info.gap_handle = gap_handle;
    play_info.asi = asi;
    play_info.clock.nclk = pta->nclk;
    play_info.clock.nclk_intra = pta->nclk_intra;
    bt_status_t set_pta_ret = bt_avm_play_a2dp(&play_info);
    bt_sink_srv_music_clear_media_packet_list();
    bt_sink_srv_report_id("[sink][a2dp]bt_sink_srv_music_trigger_play, ret:0x%08x", 1, ret);
    bt_sink_srv_assert(set_pta_ret == BT_STATUS_SUCCESS && "set play fail");

}
#endif

void bt_sink_srv_music_reject_a2dp_1M(void)
{
    extern void bt_avm_reject_a2dp_1M(void);
    bt_avm_reject_a2dp_1M();
}

void bt_sink_srv_music_set_max_bit_pool(uint32_t max_bp)
{
    extern bt_a2dp_codec_capability_t g_bt_sink_srv_a2dp_codec_list[];
    bt_a2dp_codec_capability_t *sbc_codec = &g_bt_sink_srv_a2dp_codec_list[0];
    sbc_codec->codec.sbc.max_bitpool = (max_bp & 0xFF);
}

void bt_sink_srv_music_set_music_enable(uint32_t handle, bt_avm_role_t role, bool enable)
{
    bt_status_t status = bt_avm_set_music_enable(handle, role, enable);
    while (status == BT_STATUS_OUT_OF_MEMORY) {
        vTaskDelay(5);
        status = bt_avm_set_music_enable(handle, role, enable);
    }
}

void bt_sink_srv_get_module_data_list_by_connection_info(uint8_t *data_count, void *data_list,bt_sink_srv_profile_type_t module, bt_sink_srv_profile_type_t
profile_connection_select)
{
    bt_device_manager_paired_infomation_t *paired_list = (bt_device_manager_paired_infomation_t *)bt_sink_srv_memory_alloc(BT_SINK_SRV_CM_MAX_TRUSTED_DEV*sizeof(bt_device_manager_paired_infomation_t));
    uint32_t dev_count = BT_SINK_SRV_CM_MAX_TRUSTED_DEV;
    bt_sink_srv_assert(paired_list);
    *data_count = 0;
    int i = 0;
    bt_device_manager_get_paired_list(paired_list, &dev_count);
    for(i = 0; i < dev_count; i++) {
        bt_sink_srv_profile_type_t connection_info = bt_sink_srv_cm_get_connected_profiles(&(paired_list[i].address));
        switch (module) {
            case BT_SINK_SRV_PROFILE_A2DP_SINK: {
                bt_sink_srv_music_stored_data_t dev_db = {0,0,0};
                bt_sink_srv_music_get_a2dp_nvdm_data(&(paired_list[i].address), &dev_db, sizeof(dev_db));
                uint8_t *temp_addr = (uint8_t *)(&paired_list[i].address);
                bt_sink_srv_music_data_list *temp_data_list = (bt_sink_srv_music_data_list *)data_list;
                bt_sink_srv_report_id("[sink][music]get_data, addr[%d]--0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x, count:%d", 8, i,
                    temp_addr[0], temp_addr[1], temp_addr[2], temp_addr[3], temp_addr[4], temp_addr[5], dev_db.play_order_count);
                bt_sink_srv_report_id("[sink][music]bt_sink_srv_get_module_data_list_by_connection_info, play_order_count:%d, connect_info:%d", 2,
                    dev_db.play_order_count, connection_info);
                if((temp_addr[0] == 0) && (temp_addr[1] == 0) && (temp_addr[2] == 0) && (temp_addr[3] == 0) && (temp_addr[4] == 0)) {
                    assert(0 && "unexpected addr");
                }
                if(((connection_info&profile_connection_select) == profile_connection_select) && (dev_db.play_order_count > 0)) {
                    bt_sink_srv_memcpy(&(temp_data_list[*data_count].dev_db), &dev_db, sizeof(bt_sink_srv_music_stored_data_t));
                    bt_sink_srv_memcpy(&(temp_data_list[*data_count].remote_addr), &(paired_list[i].address), sizeof(bt_bd_addr_t));
                    (*data_count)++;
                }
                break;
            }
            case BT_SINK_SRV_PROFILE_HFP:
            default:
                break;
        }
    }

    bt_sink_srv_memory_free(paired_list);
    bt_sink_srv_report_id("[sink][music]bt_sink_srv_get_module_data_list_by_connection_info, dev_count:%d, data_count:%d", 2, dev_count, *data_count);
}

void bt_sink_srv_order_for_list(void *data_list, bt_sink_srv_profile_type_t module_type, uint8_t data_count)
{
    uint8_t i = 0;
    uint8_t j = 0;
    bt_sink_srv_assert(data_list && data_count && "please check input parameters");
    for(i = data_count; i > 0; i--) {
        for(j = 0; j < i - 1; j++) {
            switch (module_type) {
                case BT_SINK_SRV_PROFILE_A2DP_SINK: {
                    bt_sink_srv_music_data_list *played_data_list = (bt_sink_srv_music_data_list *)data_list;
                    if(played_data_list[j].dev_db.play_order_count < played_data_list[j + 1].dev_db.play_order_count) {
                        bt_sink_srv_music_data_list temp_data;
                        bt_sink_srv_memcpy(&temp_data, &played_data_list[j], sizeof(bt_sink_srv_music_data_list));
                        bt_sink_srv_memcpy(&played_data_list[j], &played_data_list[j + 1], sizeof(bt_sink_srv_music_data_list));
                        bt_sink_srv_memcpy(&played_data_list[j + 1], &temp_data, sizeof(bt_sink_srv_music_data_list));
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}

bt_sink_srv_music_device_list_t *bt_sink_srv_music_get_played_device_list(bool is_connected)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_data_list data_list[BT_SINK_SRV_CM_MAX_TRUSTED_DEV];
    uint8_t played_dev_count = 0;
    uint8_t i = 0;
    bt_sink_srv_profile_type_t connected_situation = 0;

    if(is_connected) {
        connected_situation = BT_SINK_SRV_PROFILE_A2DP_SINK|BT_SINK_SRV_PROFILE_AVRCP;
    }

    bt_sink_srv_get_module_data_list_by_connection_info(&played_dev_count, data_list,
        BT_SINK_SRV_PROFILE_A2DP_SINK, connected_situation);
    if(played_dev_count) {
        bt_sink_srv_order_for_list(data_list, BT_SINK_SRV_PROFILE_A2DP_SINK, played_dev_count);

        for(i = 0; i < played_dev_count; i++) {
            bt_sink_srv_report_id("addr[%d]--0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x, dev_count:%d\r\n", 8, i,
                data_list[i].remote_addr[0], data_list[i].remote_addr[1],
                data_list[i].remote_addr[2], data_list[i].remote_addr[3],
                data_list[i].remote_addr[4], data_list[i].remote_addr[5], data_list[i].dev_db.play_order_count);
            bt_sink_srv_memcpy(&ctx->played_connected_dev.device_list[i], &data_list[i].remote_addr, sizeof(bt_bd_addr_t));
        }
    }
    ctx->played_connected_dev.number = played_dev_count;
    bt_sink_srv_report_id("[sink][music]bt_sink_srv_music_get_played_device_list, dev_count:%d", 1, played_dev_count);
    return &ctx->played_connected_dev;
}

void bt_sink_srv_update_last_device(bt_bd_addr_t *remote_addr, bt_sink_srv_profile_type_t module_type)
{
    bt_device_manager_paired_infomation_t *paired_list = (bt_device_manager_paired_infomation_t *)bt_sink_srv_memory_alloc(BT_SINK_SRV_CM_MAX_TRUSTED_DEV * sizeof(bt_device_manager_paired_infomation_t));
    uint32_t dev_count = BT_SINK_SRV_CM_MAX_TRUSTED_DEV;
    bt_sink_srv_assert(paired_list);
    void *newest_data = NULL;
    int i = 0;
    bt_device_manager_get_paired_list(paired_list, &dev_count);
    if(module_type == BT_SINK_SRV_PROFILE_A2DP_SINK) {
        newest_data = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_music_data_list));
        bt_sink_srv_memset(newest_data, 0, sizeof(bt_sink_srv_music_data_list));
    }
    for(i = 0; i < dev_count; i++) {
        if(module_type == BT_SINK_SRV_PROFILE_A2DP_SINK) {
            bt_sink_srv_music_stored_data_t dev_db;
            bt_sink_srv_music_data_list *temp_data = (bt_sink_srv_music_data_list *)newest_data;
            bt_sink_srv_music_get_a2dp_nvdm_data(&paired_list[i].address, &dev_db, sizeof(dev_db));
            uint8_t *temp_addr = (uint8_t *)(&paired_list[i].address);
            bt_sink_srv_report_id("[sink][music]addr[%d]--0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x, play_order_count:%d", 8, i,
                temp_addr[0], temp_addr[1], temp_addr[2], temp_addr[3], temp_addr[4], temp_addr[5], dev_db.play_order_count);
            if((temp_addr[0] == 0) && (temp_addr[1] == 0) && (temp_addr[2] == 0) && (temp_addr[3] == 0) && (temp_addr[4] == 0)) {
                assert(0 && "update unexpected addr");
            }
            if(temp_data->dev_db.play_order_count < dev_db.play_order_count) {
                bt_sink_srv_memcpy(&(temp_data->remote_addr), &paired_list[i].address, sizeof(bt_bd_addr_t));
                bt_sink_srv_memcpy(&(temp_data->dev_db), &dev_db, sizeof(bt_sink_srv_music_stored_data_t));
            }
        }
    }

    if(module_type == BT_SINK_SRV_PROFILE_A2DP_SINK) {
        bt_sink_srv_music_data_list *temp_data = (bt_sink_srv_music_data_list *)newest_data;
        uint8_t *temp_addr = (uint8_t *)(&temp_data->remote_addr);
        if(bt_sink_srv_memcmp(&temp_data->remote_addr, remote_addr, sizeof(bt_bd_addr_t)) != 0) {
            bt_sink_srv_music_stored_data_t dev_db;
            bt_sink_srv_music_get_a2dp_nvdm_data(remote_addr, &dev_db, sizeof(dev_db));
            dev_db.play_order_count = temp_data->dev_db.play_order_count + 1;
            bt_sink_srv_music_set_a2dp_nvdm_data(remote_addr, &dev_db, sizeof(dev_db));
        }
        bt_sink_srv_report_id("[sink][music]bt_sink_srv_update_last_device, addr--0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:, max_count:%d", 7,
            temp_addr[0], temp_addr[1], temp_addr[2], temp_addr[3], temp_addr[4], temp_addr[5], temp_data->dev_db.play_order_count);
        temp_addr = (uint8_t *)remote_addr;
        bt_sink_srv_report_id("[sink][music]remote_addr--0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", 6,
            temp_addr[0], temp_addr[1], temp_addr[2], temp_addr[3], temp_addr[4], temp_addr[5]);
        bt_sink_srv_memory_free(newest_data);
    }
    bt_sink_srv_memory_free(paired_list);
}

uint32_t bt_sink_srv_music_find_free_timer(uint32_t start_timer_id, uint32_t end_timer_id)
{
    uint32_t i = 0;
    uint32_t timer_ret = 0;
    for(i = start_timer_id; i <= end_timer_id; i++) {
        if(!bt_timer_ext_find(i)) {
            timer_ret = i;
            break;
        }
    }
    bt_sink_srv_report_id("[sink][music] find timer id:0x%08x", 1, timer_ret);
    return timer_ret;
}

void bt_sink_srv_music_lock_dvfs()
{
#ifdef HAL_DVFS_416M_SOURCE
    hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M,HAL_DVFS_LOCK);
#elif defined(HAL_DVFS_312M_SOURCE)
    hal_dvfs_lock_control(DVFS_156M_SPEED, HAL_DVFS_LOCK);
#endif
}

void bt_sink_srv_music_unlock_dvfs()
{
#ifdef HAL_DVFS_416M_SOURCE
    hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M,HAL_DVFS_UNLOCK);
#elif defined(HAL_DVFS_312M_SOURCE)
    hal_dvfs_lock_control(DVFS_156M_SPEED, HAL_DVFS_UNLOCK);
#endif
}

void bt_sink_srv_music_set_ALC_enable(uint8_t is_enable)
{
    bt_sink_srv_report_id("[sink][music] bt_sink_srv_music_set_ALC_enable:0x%x", 1, is_enable);
    g_alc_enable = is_enable;
}

uint8_t bt_sink_srv_music_get_ALC_enable(void)
{
    bt_sink_srv_report_id("[sink][music] bt_sink_srv_music_get_ALC_enable:0x%x", 1, g_alc_enable);
    return g_alc_enable;
}

#ifdef ENABLE_HWSRC_CLKSKEW
#define BT_SINK_SRV_MUSIC_MAPPING_VALUE     (61)

int16_t bt_sink_srv_music_drift_mapping(int16_t origin_drift, int16_t mapping_value)
{

    int16_t new_drift = ((origin_drift + (mapping_value>>1))/mapping_value)*mapping_value;
    if(new_drift > 915) {
        new_drift -= mapping_value;
    } else if(new_drift < (-915)) {
        new_drift += mapping_value;
    }
    bt_sink_srv_report_id("[sink][music] new_drift:%d, origin_drift:%d, mapping_value:%d", 3, new_drift, origin_drift, mapping_value);

    return new_drift;
}
#endif

extern volatile uint32_t g_Rdebug_a2dp_ltcs_last_time;    /* for A2DP LTCS debug */

void bt_sink_srv_music_handle_long_time_drift(audio_dsp_a2dp_ltcs_report_param_t *report_param)
{
    /* for A2DP LTCS debug*/
    bt_sink_srv_mutex_lock();
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if((role != BT_AWS_MCE_ROLE_NONE) && (role != BT_AWS_MCE_ROLE_AGENT)) {
        bt_sink_srv_mutex_unlock();
        return;
    }
    uint32_t start_cnt = g_Rdebug_a2dp_ltcs_last_time;
    uint32_t current_cnt = 0;
    uint32_t duration = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, (uint32_t *)(&g_Rdebug_a2dp_ltcs_last_time));
    hal_gpt_get_duration_count(start_cnt, g_Rdebug_a2dp_ltcs_last_time, &duration);
    if((((duration * 1000)>>15) < 1000) || ((role == BT_AWS_MCE_ROLE_CLINET) || (role == BT_AWS_MCE_ROLE_PARTNER))) {
        bt_sink_srv_report_id("AUD_A2DP_LTCS_REPORT_error, LTCS too close. cur:%d, last:%d, role:0x%02x", 3,
                            current_cnt, g_Rdebug_a2dp_ltcs_last_time, role);
        bt_sink_srv_mutex_unlock();
        //platform_assert("A2DP_LTCS_trigger_too_close",__FILE__,__LINE__);
        return;    /*Ignore this ltcs request.*/
    }
    /*******************************************/
    //to do calculate drift
    bt_sink_srv_report_id("AUD_A2DP_LTCS_REPORT", 0);

    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    if(!run_dev) {
        bt_sink_srv_mutex_unlock();
        return;
    }
    bt_sink_srv_report_id("[sink][a2dp]a2dp_hd:0x%08x", 1, (void *)(&(ctx->run_dev->a2dp_hd)));
    bt_clock_t clk_base = {0};
    uint32_t gap_hd = bt_sink_srv_cm_get_gap_handle(&(run_dev->dev_addr));;

    uint32_t gpt_run_count_begin = 0;
    uint32_t gpt_run_count_end = 0;
    int32_t gpt_ret = 0;
    uint32_t cost_dur = 0;
    bt_sink_srv_assert(gap_hd);

    bt_sink_srv_memcpy(&clk_base, &ctx->bt_clk, sizeof(bt_clock_t));

    avm_direct_cal_pta(report_param->p_ltcs_asi_buf[63], &ctx->bt_clk, &clk_base, BT_ROLE_MASTER);
    ctx->us_asi += report_param->p_ltcs_asi_buf[63];
    uint32_t asi_count = avm_direct_us_2_asi(ctx->freq,report_param->p_ltcs_asi_buf[63]);
    ctx->ts += asi_count;
    ctx->ts &= 0x03ffffff;
    bt_sink_srv_report_id("[sink][a2dp]asi_cur:0x%08x, asi_count:%d", 2, ctx->us_asi, asi_count);
    //bt_sink_srv_memset(report_param->p_ltcs_min_gap_buf, 0, 64*sizeof(int32_t));

    gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_run_count_begin);
    int16_t drift_value = avm_direct_cal_drift(63, report_param->p_ltcs_asi_buf, 
        (int32_t *)report_param->p_ltcs_min_gap_buf);

#ifdef ENABLE_HWSRC_CLKSKEW
    drift_value = bt_sink_srv_music_drift_mapping(drift_value, BT_SINK_SRV_MUSIC_MAPPING_VALUE);
    avm_direct_set_drift(drift_value);
#endif

    gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_run_count_end);
    cost_dur = (gpt_run_count_end - gpt_run_count_begin) * 1000 / 32768;
    bt_sink_srv_report_id("[sink][a2dp]cal_drift--cost: %d, gpt_ret: %d", 3, cost_dur, gpt_ret);
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
    bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_UPDATE_BASE, (void *)(&(ctx->run_dev->a2dp_hd)));
#endif
#ifdef MTK_BT_SPEAKER_ENABLE

    ctx->update_base_count = 0;
    if(BT_SINK_SRV_MUSIC_MODE_BROADCAST == bt_sink_srv_music_get_spk_mode()) {
        ctx->update_base_count++;
        bt_timer_ext_start(BT_SINK_SRV_A2DP_SEND_BASE_IND_TIMER_ID, 0, 
            BT_SINK_SRV_A2DP_SEND_BASE_IND_TIMER_DUR, bt_sink_srv_a2dp_update_base_timer);
    }
#endif
    bt_sink_srv_music_update_base_parameters_to_dsp(drift_value, ctx->bt_clk.nclk, ctx->ts, ctx->ts);
    bt_clock_t lt_clk = {0};
    bt_get_bt_clock(gap_hd, &lt_clk);
    bt_sink_srv_report_id("[sink][a2dp]send*****modify to 63 drift_value: %d, asi:0x%08x, nclk:0x%08x, nclk_intra:0x%04x, cur_nclk:0x%08x, cur_intra:0x%08x*****", 6, 
    (int16_t)drift_value, ctx->ts, ctx->bt_clk.nclk, 
    ctx->bt_clk.nclk_intra, lt_clk.nclk, lt_clk.nclk_intra);

    bt_sink_srv_mutex_unlock();
}

#ifdef FEATURE_NEW_MULTIPOINT_TRIAL
bt_sink_srv_music_device_t *bt_sink_srv_music_get_other_streaming_devices(bt_sink_srv_music_device_t* current_streaming_dev)
{
    bt_sink_srv_music_context_t* ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *streaming_dev = NULL;

    uint8_t count = 0;
    for (int i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
        if (ctx->sink_dev[i].a2dp_status == BT_SINK_SRV_A2DP_STATUS_STREAMING) {
            count++;
            if(current_streaming_dev == &(ctx->sink_dev[i])){
                bt_sink_srv_report_id("[Sink][music] run dev index = %d,addr:%02x:%02x:%02x:%02x:%02x:%02x", 7,i,
                    ctx->sink_dev[i].dev_addr[5],ctx->sink_dev[i].dev_addr[4], ctx->sink_dev[i].dev_addr[3],
                    ctx->sink_dev[i].dev_addr[2],ctx->sink_dev[i].dev_addr[1], ctx->sink_dev[i].dev_addr[0]);
            }else{
                streaming_dev = &(ctx->sink_dev[i]);
                bt_sink_srv_report_id("[Sink][music] index = %d,addr:%02x:%02x:%02x:%02x:%02x:%02x", 7,i,
                    ctx->sink_dev[i].dev_addr[5],ctx->sink_dev[i].dev_addr[4], ctx->sink_dev[i].dev_addr[3],
                    ctx->sink_dev[i].dev_addr[2],ctx->sink_dev[i].dev_addr[1], ctx->sink_dev[i].dev_addr[0]);
            }
        }

    }

    bt_sink_srv_report_id("[Sink][music] streaming device num = %d, streaming_dev:0x%08x",2,count, streaming_dev);
    return streaming_dev;
}
#endif

#ifdef MTK_AUDIO_SYNC_ENABLE
void bt_sink_srv_music_volume_sync_callback(bt_sink_srv_sync_status_t status, bt_sink_srv_sync_callback_data_t *sync_data)
{
    bt_sink_srv_mutex_lock();
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_sink_srv_music_context_t* ctx = bt_sink_srv_music_get_context();
    uint8_t aid = (role == BT_AWS_MCE_ROLE_AGENT ? ctx->a2dp_aid : ctx->aws_aid);
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    uint32_t op_flag = 0xffffffff;

    if(status == BT_SINK_SRV_SYNC_SUCCESS && run_dev && (run_dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY)) {
        bt_sink_srv_am_audio_sync_capability_t sync_capability = {0};
        bt_sink_srv_audio_sync_music_data_t *music_data = (bt_sink_srv_audio_sync_music_data_t *)sync_data->data;

        sync_capability.sync_scenario_type = MCU2DSP_SYNC_REQUEST_A2DP;
        sync_capability.sync_action_type = MCU2DSP_SYNC_REQUEST_SET_VOLUME;
        sync_capability.target_gpt_cnt = sync_data->gpt_count;
        sync_capability.vol_out.vol_level = music_data->volume;
        sync_capability.vol_out.channel = MCU2DSP_SYNC_VOLUME_CHANNEL_DUAL;
        op_flag = run_dev->op;

        bt_sink_srv_ami_audio_request_sync(aid, &sync_capability);
        if(music_data->volume_type) {
            bt_sink_srv_avrcp_volume_notification(run_dev->avrcp_hd, music_data->volume, music_data->volume_type);
        }
        if(role == BT_AWS_MCE_ROLE_PARTNER) {
            ctx->vol_lev = music_data->volume;
        }
    }
    bt_sink_srv_report_id("[Sink][music] music_volume_sync_callback, run_dev:0x%08x, op_flag:0x%08x, status:0x%02x",3,run_dev, op_flag, status);
    bt_sink_srv_mutex_unlock();
}

uint32_t test_duration = 200000;

void bt_sink_srv_music_set_duration(uint32_t dur)
{
    test_duration = dur;

    bt_sink_srv_report_id("[Sink][music] set dur:0x%08x", 1, test_duration);
}

#ifdef AIR_A2DP_SYNC_STOP_ENABLE
bool g_a2dp_for_stop = false;
void bt_sink_srv_music_set_force_stop(bool force_stop)
{
    bt_sink_srv_report_id("[Sink][music] bt_sink_srv_music_set_force_stop:0x%x", 1, force_stop);
    g_a2dp_for_stop = force_stop;
}

bool bt_sink_srv_music_get_force_stop(void)
{
    bt_sink_srv_report_id("[Sink][music] bt_sink_srv_music_get_force_stop:0x%x", 1, g_a2dp_for_stop);
    return g_a2dp_for_stop;
}

bt_status_t bt_sink_srv_music_request_sync_stop()
{
    bt_sink_srv_music_context_t* ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    bt_sink_srv_get_sync_data_parameter_t sync_data;
    sync_data.type = BT_SINK_SRV_MUSIC_STOP_TYPE;
    sync_data.length = 0;
    sync_data.duration = test_duration;
    sync_data.timeout_duration = 0xffffffff;
    sync_data.data = NULL;

    bt_status_t ret = bt_sink_srv_request_sync_gpt(&sync_data);
    bt_sink_srv_report_id("[Sink][music] request_sync_stop, ret:0x%08x, dur:0x%08x", 2, ret, test_duration);
    return ret;
}

void bt_sink_srv_music_stop_sync_callback(bt_sink_srv_sync_status_t status, bt_sink_srv_sync_callback_data_t *sync_data)
{
    bt_sink_srv_mutex_lock();
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_sink_srv_music_context_t* ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    uint8_t aid = (role == BT_AWS_MCE_ROLE_AGENT ? ctx->a2dp_aid : ctx->aws_aid);

    if(run_dev && (run_dev->state == AUDIO_SRC_SRV_STATE_PLAYING)) {
        if(status == BT_SINK_SRV_SYNC_SUCCESS) {
            bt_sink_srv_am_audio_sync_capability_t sync_capability = {0};
            bt_sink_srv_audio_sync_music_data_t *music_data = (bt_sink_srv_audio_sync_music_data_t *)sync_data->data;

            sync_capability.sync_scenario_type = MCU2DSP_SYNC_REQUEST_A2DP;
            sync_capability.sync_action_type = MCU2DSP_SYNC_REQUEST_STOP;
            sync_capability.target_gpt_cnt = sync_data->gpt_count;
            bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_SUSPEND_IND, NULL);

            bt_sink_srv_ami_audio_request_sync(aid, &sync_capability);
        } else {
            bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_SUSPEND_IND, NULL);
            bt_sink_srv_music_stop(run_dev, aid);
        }
    }

    bt_sink_srv_report_id("[Sink][music] music_stop_sync_callback, status:0x%02x",1,status);
    bt_sink_srv_mutex_unlock();
}

void bt_sink_srv_music_sync_stop_done_handler(hal_audio_event_t event, void *data)
{
    bt_sink_srv_report_id("[Sink][music] music_stop_done_callback, event:0x%02x, enter", 1, event);
    bt_sink_srv_mutex_lock();
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_sink_srv_music_context_t* ctx = bt_sink_srv_music_get_context();
    uint8_t aid = ((BT_AWS_MCE_ROLE_NONE == role) || (BT_AWS_MCE_ROLE_AGENT == role)) ? ctx->a2dp_aid:ctx->aws_aid;
    if(event == HAL_AUDIO_EVENT_END) {
        bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
        if (run_dev) {
            if((!(run_dev->conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT)) && (role == BT_AWS_MCE_ROLE_NONE || role == BT_AWS_MCE_ROLE_AGENT)) {
                bt_sink_srv_cm_profile_status_notify(&run_dev->dev_addr, BT_SINK_SRV_PROFILE_A2DP_SINK, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED, BT_STATUS_SUCCESS);
            }
            bt_sink_srv_music_stop(ctx->run_dev, aid);
        }
    }
    bt_sink_srv_report_id("[Sink][music] music_stop_done_callback, event:0x%02x, run_dev:0x%x", 2, event, ctx->run_dev);
    bt_sink_srv_mutex_unlock();
}

#endif
#endif

void bt_sink_srv_music_get_waiting_list_devices(uint32_t *device_list, uint32_t *device_count)
{
    bt_sink_srv_music_context_t* ctx = bt_sink_srv_music_get_context();
    uint32_t count = 0;

    for (int i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
        if (ctx->sink_dev[i].handle && ctx->sink_dev[i].handle->flag&AUDIO_SRC_SRV_FLAG_WAITING) {
            device_list[count] = (uint32_t)(&(ctx->sink_dev[i]));
            count++;
            bt_sink_srv_report_id("[Sink][music] waiting list dev index = %d,addr:%02x:%02x:%02x:%02x:%02x:%02x", 7,i,
                ctx->sink_dev[i].dev_addr[5],ctx->sink_dev[i].dev_addr[4], ctx->sink_dev[i].dev_addr[3],
                ctx->sink_dev[i].dev_addr[2],ctx->sink_dev[i].dev_addr[1], ctx->sink_dev[i].dev_addr[0]);
        }
    }

    *device_count = count;
}

void bt_sink_srv_music_device_waiting_list_operation(uint32_t *device_list, uint32_t device_count, bool is_add)
{
    bt_sink_srv_music_device_t *device_ptr = NULL;
    uint32_t i = 0;
    for(i = 0; i < device_count; i++) {
        device_ptr = (bt_sink_srv_music_device_t *)(device_list[i]);
        if(is_add) {
            audio_src_srv_add_waiting_list(device_ptr->handle);
        } else {
            audio_src_srv_del_waiting_list(device_ptr->handle);
        }
    }
}


bt_sink_srv_music_device_t *bt_sink_srv_music_get_must_play_flag_dev(void)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *dev = NULL;
    uint32_t i = 0;

    for (i = 0; i < BT_SINK_SRV_MUISC_DEV_COUNT; ++i) {
        if (ctx->sink_dev[i].avrcp_flag & BT_SINK_SRV_AVRCP_MUST_PLAY_RING_TONE_FLAG) {
            dev = &(ctx->sink_dev[i]);
            break;
        }
    }

    return dev;
}


bt_status_t bt_sink_srv_set_must_play_tone_flag(bt_bd_addr_t *sp_addr, bt_sink_srv_notification_voice_t type, bool is_open)
{
    bt_sink_srv_music_context_t* ctx = bt_sink_srv_music_get_context();
    bt_status_t ret = BT_STATUS_FAIL;
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
    if(sp_addr) {
        a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, (void *)sp_addr);
        if(a2dp_dev) {
            if(is_open) {
                BT_SINK_SRV_SET_FLAG(a2dp_dev->avrcp_flag,BT_SINK_SRV_AVRCP_MUST_PLAY_RING_TONE_FLAG);
            } else {
                BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->avrcp_flag, BT_SINK_SRV_AVRCP_MUST_PLAY_RING_TONE_FLAG);
            }

            ret = BT_STATUS_SUCCESS;
        } 
    } else {
        if(is_open) {
            BT_SINK_SRV_SET_FLAG(ctx->context_flag,BT_SINK_SRV_CNTX_FLAG_MUST_PLAY_RING_TONE_FLAG);
        } else {
            bt_sink_srv_music_device_t *dev = bt_sink_srv_music_get_must_play_flag_dev();
            if(dev) {
                BT_SINK_SRV_REMOVE_FLAG(dev->avrcp_flag, BT_SINK_SRV_AVRCP_MUST_PLAY_RING_TONE_FLAG);
            }
            BT_SINK_SRV_REMOVE_FLAG(ctx->context_flag, BT_SINK_SRV_CNTX_FLAG_MUST_PLAY_RING_TONE_FLAG);
        }
        
        ret = BT_STATUS_SUCCESS;

    }

    bt_sink_srv_report_id("[Sink][music] set play tone flag : 0x%02x, ret:0x%08x, dev:0x%08x",3,is_open, ret, a2dp_dev);

    return ret;
}


#ifdef AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
bt_status_t bt_sink_srv_music_stop_local_music(void)
{
    bt_sink_srv_mutex_lock();
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    bt_status_t ret = BT_STATUS_FAIL;

    BT_SINK_SRV_SET_FLAG(ctx->context_flag, BT_SINK_SRV_CNTX_FLAG_SWITCH_AUDIO_OFF);

    if(run_dev) {
        ctx->resume_dev = run_dev;
        uint32_t device_list[BT_SINK_SRV_MUISC_DEV_COUNT];
        uint32_t device_count = 0;
        bt_sink_srv_music_get_waiting_list_devices(ctx->waiting_list, &ctx->waiting_list_count);
        bt_sink_srv_music_device_waiting_list_operation(ctx->waiting_list, ctx->waiting_list_count, false);

        bt_sink_srv_music_state_machine_handle(run_dev, BT_A2DP_SUSPEND_STREAMING_IND, NULL);
        ret = BT_STATUS_SUCCESS;
    } else {
        bt_sink_srv_report_id("[a2dp]It is not on playing or during prepare play", 0);
        ret = BT_STATUS_SUCCESS;
    }
    bt_sink_srv_report_id("[a2dp]To stop local music, run_dev:0x%08x", 1, run_dev);
    bt_sink_srv_mutex_unlock();

    return ret;
}

bt_status_t bt_sink_srv_music_resume_local_music(void)
{
    bt_sink_srv_mutex_lock();
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *resume_dev = ctx->resume_dev;
    bt_status_t ret = BT_STATUS_FAIL;
    uint32_t op_flag = 0;

    BT_SINK_SRV_REMOVE_FLAG(ctx->context_flag, BT_SINK_SRV_CNTX_FLAG_SWITCH_AUDIO_OFF);

    if(resume_dev) {
        op_flag = resume_dev->op;
        if(resume_dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_STREAMING && (resume_dev->conn_bit&BT_SINK_SRV_MUSIC_A2DP_CONN_BIT)) {
            bt_sink_srv_music_state_machine_handle(ctx->resume_dev, BT_A2DP_START_STREAMING_IND, NULL);
        }
        bt_sink_srv_music_device_waiting_list_operation(ctx->waiting_list, ctx->waiting_list_count, true);
        ctx->waiting_list_count = 0;
        ret = BT_STATUS_SUCCESS;
        ctx->resume_dev = NULL;
    }
    bt_sink_srv_report_id("[a2dp]To resume local music, run_dev:0x%08x, op:0x%08x", 2, resume_dev, op_flag);
    bt_sink_srv_mutex_unlock();

    return ret;
}

bt_status_t bt_sink_srv_music_audio_switch(bool switch_on)
{
    bt_status_t ret = BT_STATUS_FAIL;
    if(switch_on) {
        ret = bt_sink_srv_music_resume_local_music();
    } else {
        ret = bt_sink_srv_music_stop_local_music();
    }

    return ret;
}
#endif

uint8_t bt_sink_srv_get_last_music_volume()
{
    bt_sink_srv_music_context_t* ctx = bt_sink_srv_music_get_context();
    return ctx->last_volume;
}

bt_status_t bt_sink_srv_set_local_volume(uint8_t volume)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_sink_srv_music_context_t* ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    bt_status_t ret = BT_STATUS_FAIL;
    uint8_t aid = (role==BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_NONE) ? ctx->a2dp_aid : ctx->aws_aid;

    if(run_dev && (run_dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN)) {
        bt_sink_srv_ami_audio_set_volume(aid, volume, STREAM_OUT);
        ctx->last_volume = volume;
        ret = BT_STATUS_SUCCESS;
    }

    bt_sink_srv_report_id("[Sink][music] set local volume : 0x%02x, ret:0x%08x, dev:0x%08x",3,volume, ret, run_dev);

    return ret;
}

bt_sink_srv_music_playback_state_t bt_sink_srv_get_music_state()
{
    bt_sink_srv_music_device_t *dev = bt_sink_srv_avrcp_get_device(NULL, BT_SINK_SRV_ACTION_PLAY_PAUSE);
    bt_sink_srv_music_playback_state_t state = BT_SINK_SRV_MUSIC_PLAYBACK_STATE_NONE;
    bt_avrcp_operation_id_t action_id = 0;
    if (dev) {
        action_id = bt_sink_srv_music_get_play_pause_action(dev);
        if (action_id == BT_AVRCP_OPERATION_ID_PLAY) {
            state = BT_SINK_SRV_MUSIC_PLAYBACK_STATE_STOPPED;
        } else if (action_id == BT_AVRCP_OPERATION_ID_PAUSE) {
            state = BT_SINK_SRV_MUSIC_PLAYBACK_STATE_PLAYING;
        }
    }

    return state;
}

bt_sink_srv_state_t bt_sink_srv_music_get_music_state(bt_bd_addr_t *addr)
{
    bt_sink_srv_music_context_t* ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    bt_sink_srv_state_t state = BT_SINK_SRV_STATE_NONE;

    
    bt_sink_srv_music_device_t *dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_AVRCP, (void *)addr);
    if(!dev) {
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, (void *)addr);
    }

    if(dev && run_dev && run_dev == dev) {
        state = BT_SINK_SRV_STATE_STREAMING;
    } else {
        state = BT_SINK_SRV_STATE_CONNECTED;
    }

    return state;
}

void bt_sink_srv_music_set_interrupt_flag(uint32_t flagMask, bool val)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    if (BT_SINK_SRV_CNTX_FLAG_A2DP_INTER_ULL_BY_AVRCP != flagMask) {
        assert(0 && "unexpect music interrupt flag");
    }
    if (val) {
        BT_SINK_SRV_SET_FLAG(ctx->context_flag,flagMask);
    }
    else {
        BT_SINK_SRV_REMOVE_FLAG(ctx->context_flag,flagMask);
    }
    bt_sink_srv_report_id("[Sink][music] set a2dp interrupt flag: context_flag:0x%x, 0x%x, val:0x%08x",
        3, ctx->context_flag, flagMask, val);
}

bool bt_sink_srv_music_get_ratio_status(uint32_t *addr, uint32_t *asi)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    if(ctx->ratio == BT_SINK_SRV_MUSIC_RATIO_MAGIC_CODE){
        return false;
    }
    *addr = ctx->ratio;
    *asi = ctx->ts;
    bt_sink_srv_report_id("[a2dp] controller get ratio:0x%08x, asi:0x%08x", 2, ctx->ratio, ctx->ts);
    return true;
}

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_default_volume_level=_default_bt_sink_srv_get_default_volume_level")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_sink_srv_get_default_volume_level = default_bt_sink_srv_get_default_volume_level
#else
#error "Unsupported Platform"
#endif


