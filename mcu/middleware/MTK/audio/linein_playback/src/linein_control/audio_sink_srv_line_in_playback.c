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

#include "audio_sink_srv_line_in.h"
#include "audio_sink_srv_line_in_internal.h"
#include "bt_sink_srv_ami.h"
#include "hal_audio.h"
#include "hal_audio_message_struct.h"
#include "FreeRTOS.h"
#include "bt_sink_srv_utils.h"

audio_sink_srv_line_in_pseudo_handle_t g_line_in_hd;
uint32_t g_line_in_gpt_codec_run_count_begin = 0;
uint32_t g_line_in_gpt_codec_run_count_end = 0;
#define UNUSED(x)  ((void)(x))

extern void audio_sink_srv_line_in_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param);
extern void audio_sink_srv_line_in_common_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void 
*param);

audio_src_srv_handle_t *audio_sink_srv_line_in_alloc_pseudo_handle(void);

void audio_sink_srv_line_in_free_pseudo_handle(audio_src_srv_handle_t *hd);

void audio_sink_srv_line_in_init(void)
{
    int8_t ori_aid = 0;
    audio_sink_srv_line_in_context_t *ctx = NULL;

    ctx = audio_sink_srv_line_in_get_context();
    ori_aid = ctx->line_in_aid;

    if (ctx->line_in_aid != AUDIO_SINK_SRV_INVALID_AID) {
        audio_src_srv_report("[sink][Line-IN]init id error: %d\n", 1, ctx->line_in_aid);
        bt_sink_srv_ami_audio_close(ctx->line_in_aid);
        ctx->line_in_aid = AUDIO_SINK_SRV_INVALID_AID;
    }

    ctx->line_in_aid = bt_sink_srv_ami_audio_open(AUD_MIDDLE, audio_sink_srv_line_in_common_ami_hdr);
    audio_src_srv_report("[sink][Line-IN]init-line_in_aid: %d, ori: %d", 2,
                       ctx->line_in_aid, ori_aid);
    UNUSED(ori_aid);
}

void audio_sink_srv_line_in_deinit(void)
{
    audio_sink_srv_line_in_context_t *ctx = NULL;
    ctx = audio_sink_srv_line_in_get_context();

    if (ctx->line_in_aid != AUDIO_SINK_SRV_INVALID_AID) {
        bt_sink_srv_ami_audio_close(ctx->line_in_aid);
        ctx->line_in_aid = AUDIO_SINK_SRV_INVALID_AID;
    }
    audio_src_srv_report("[sink][Line-IN]deinit-line_in_aid: %d, ori: %d", 1,
                       ctx->line_in_aid);
}

void audio_sink_srv_line_in_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param)
{
    audio_sink_srv_line_in_context_t *ctx = NULL;
    int32_t ret = 0;
    int32_t err_code = 0;
    audio_sink_srv_line_in_device_t *run_dev = NULL;

    ctx = audio_sink_srv_line_in_get_context();
    run_dev = audio_sink_srv_line_in_get_device();
    audio_src_srv_report("[sink][Line-IN]ami_hdr[s]-aid: %d, aud_id: %d, msg_id: %d, sub_msg: %d, 2nd: 0x%x", 5,
                           ctx->line_in_aid, aud_id, msg_id, sub_msg, sub_msg);
    if (ctx->line_in_aid == aud_id) {
         switch (msg_id) {
            case AUD_SELF_CMD_REQ: {
                if (AUD_CMD_FAILURE == sub_msg) {
                }
                break;
            }
            case AUD_SINK_OPEN_CODEC: {
                audio_sink_srv_line_in_state_machine_handle(run_dev, AUDIO_SINK_SRV_LINE_IN_EVT_PLAYING, NULL);
            }
         }
    }
    UNUSED(run_dev);
    UNUSED(err_code);
    UNUSED(ret);
#if 0
    //bt_sink_srv_assert(run_dev);
    if ((ctx->a2dp_aid == aud_id) &&
        (msg_id == AUD_A2DP_PROC_IND) &&
        (sub_msg == AUD_STREAM_EVENT_DATA_REQ ||
         sub_msg == AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW ||
         sub_msg == AUD_CODEC_MEDIA_AWS_CHECK_UNDERFLOW)) {
        // drop
        ;
    } else {
        bt_sink_srv_report("[sink][Line-IN]ami_hdr[s]-aid: %d, aud_id: %d, msg_id: %d, sub_msg: %d, 2nd: 0x%x", 5,
                           ctx->a2dp_aid, aud_id, msg_id, sub_msg, sub_msg);
    }
    if (ctx->a2dp_aid == aud_id) {
        switch (msg_id) {
            case AUD_SELF_CMD_REQ: {
                if (AUD_CMD_FAILURE == sub_msg) {
                }
                break;
            }

            case AUD_SINK_OPEN_CODEC: {
                if ((run_dev) && (run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC)) {
                    /* Save codec handle */
                    bt_sink_srv_memcpy(&(run_dev->med_handle), param, sizeof(bt_sink_srv_am_media_handle_t));

                    BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC);

                    bool aws_attached = false;
                    int32_t gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,
                        &g_a2dp_gpt_codec_run_count_end);

                    uint32_t cost_dur = (g_a2dp_gpt_codec_run_count_end - g_a2dp_gpt_codec_run_count_begin) * 1000 / 32768;;
                    bt_sink_srv_report("[sink][a2dp]codec done--end: %d, gpt_ret: %d, cost: %d", 3, g_a2dp_gpt_codec_run_count_end, gpt_ret, cost_dur);

                    /* Set codec open flag */
                    BT_SINK_SRV_SET_FLAG(run_dev->op, BT_SINK_SRV_MUSIC_OP_CODEC_OPEN);

                    // TODO: check aws set R/L channel
                    uint32_t aws_handle = 0;
                    uint8_t channel_num = HAL_AUDIO_STEREO;

                    if(run_dev->flag & BT_SINK_SRV_MUSIC_WAIT_SET_VOLUME) {
                        bt_sink_srv_ami_audio_set_volume(ctx->aws_aid, ctx->vol_lev, STREAM_OUT);
                        BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_WAIT_SET_VOLUME);
                    }

                    if(BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC ==
                        run_dev->handle->substate) {
                        if(run_dev->flag&BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC) {
                            BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC);
                        }

                        if(run_dev->flag&BT_SINK_SRV_MUSIC_FLAG_START_REQ_CNF) {
                            BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_START_REQ_CNF);
                        }

                        if(run_dev->flag&BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START) {
                            bt_sink_srv_report("[sink][Line-IN] need to response a2dp start", 0);
                            BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START);
                            bt_a2dp_start_streaming_response(run_dev->a2dp_hd, true);
                        }
                        bt_sink_srv_report("[sink][Line-IN] to clear codec", 0);
                        bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_CODEC_OPEN, NULL);
                        break;
                    }
                    if(BT_SINK_SRV_MUSIC_DEV_LEFT ==
                        bt_sink_app_get_device_role() && aws_attached) {
                        channel_num = HAL_AUDIO_STEREO_BOTH_L_CHANNEL;
                    } else if (BT_SINK_SRV_MUSIC_DEV_RIGHT ==
                        bt_sink_app_get_device_role() && aws_attached) {
                        channel_num = HAL_AUDIO_STEREO_BOTH_R_CHANNEL;
                    } else {
                        channel_num = HAL_AUDIO_STEREO;
                    }

                    bt_sink_srv_report("[sink][Line-IN]to set channel:%d", 1,channel_num);

                    /*set n pkt should before rsp to sp*/
#ifdef __BT_SINK_SRV_A2DP_AVM_DIRECT_SUPPORT__
                    uint32_t gap_hd = 0;
                    gap_hd = bt_sink_srv_cm_get_gap_handle(&(run_dev->dev_addr));
                    bt_sink_srv_assert(gap_hd);
                    if(run_dev->codec.type == BT_A2DP_CODEC_VENDOR) {
                        bt_avm_set_a2dp_notify_condition(gap_hd, BT_SINK_SRV_A2DP_N_PACKET_NOTIFY_VENDOR_CODEC);
                    } else {
                        bt_avm_set_a2dp_notify_condition(gap_hd, BT_SINK_SRV_A2DP_N_PACKET_NOTIFY);
                    }
                    BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_N_PKT_DONE);
#endif
                    BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_START_REQ_CNF);
                    BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC);
                    if(run_dev->flag&BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START)
                    {
                        BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START);
                        ret = bt_a2dp_start_streaming_response(run_dev->a2dp_hd, true);
                    } else {
                        ret = BT_STATUS_SUCCESS;
                    }
                    if (BT_STATUS_SUCCESS == ret) {

                        if(BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC != run_dev->handle->substate) {
                    #ifdef __BT_SINK_SRV_A2DP_V13_SUPPORT__
                         if (run_dev->codec.delay_report) {
                             int32_t de_ret = 0;
                             de_ret = bt_a2dp_set_delay_report(run_dev->a2dp_hd, BT_SINK_SRV_A2DP_DELAY);
                             bt_sink_srv_report("[sink][Line-IN]delay_report--ret: 0x%08x", 1, de_ret);
                         }

                         if(run_dev->codec.sec_type) {
                             bt_sink_srv_report("[sink][Line-IN]set content protection", 0);
                            bt_sink_srv_am_media_handle_t *med_handle = &(run_dev->med_handle);
                            med_handle->med_hd->set_content_protection(med_handle->med_hd,
                            true);
                         }
                    #endif
                        hal_audio_set_stream_out_channel_number(channel_num);
                        }

                        bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_CODEC_OPEN, NULL);
                    } else {
                        /* Error handle */
                        bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL, NULL);
                    }
                }
                break;
            }

            /* interrupt */
            case AUD_SUSPEND_BY_IND: {
                break;
            }

            case AUD_RESUME_IND: {
            }
            break;

            case AUD_A2DP_PROC_IND: {
                if (run_dev) {
                    switch (sub_msg) {
                        case AUD_A2DP_CODEC_RESTART:
                        case AUD_A2DP_AWS_UNDERFLOW: {
                            break;
                        }

                        case AUD_A2DP_LTCS_REPORT: {
                            /* for A2DP LTCS debug*/
                            uint32_t current_cnt = 0;
                            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &current_cnt);
                            if( (((current_cnt - g_Rdebug_a2dp_ltcs_last_time) * 1000)>>15) < 1000) {
                                bt_sink_srv_report("AUD_A2DP_LTCS_REPORT_error, LTCS too close. cur:%d, last:%d", 2,
                                                    current_cnt, g_Rdebug_a2dp_ltcs_last_time);
                                //bt_sink_srv_report("A2DP_LTCS_trigger_too_close", 0);
                                //configASSERT(0);
                                break;    /*Ignore this ltcs request.*/
                            }
                            g_Rdebug_a2dp_ltcs_last_time = current_cnt;
                            /*******************************************/
                            //to do calculate drift
                            bt_sink_srv_report("AUD_A2DP_LTCS_REPORT", 0);
                            audio_dsp_a2dp_ltcs_report_param_t *report_param = (audio_dsp_a2dp_ltcs_report_param_t *)param;

                            bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
                            bt_sink_srv_report("[sink][Line-IN]a2dp_hd:0x%08x", 1, (void *)(&(ctx->run_dev->a2dp_hd)));
                            bt_clock_t clk_base = {0};
                            uint32_t gap_hd = bt_sink_srv_cm_get_gap_handle(&(run_dev->dev_addr));;

                            uint32_t gpt_run_count_begin = 0;
                            uint32_t gpt_run_count_end = 0;
                            int32_t gpt_ret = 0;
                            uint32_t cost_dur = 0;
                            bt_sink_srv_assert(gap_hd);

                            bt_sink_srv_memcpy(&clk_base, &ctx->bt_clk, sizeof(bt_clock_t));

                            avm_direct_cal_pta(report_param->p_ltcs_asi_buf[63], &ctx->bt_clk, &clk_base);
                            ctx->us_asi += report_param->p_ltcs_asi_buf[63];
                            uint32_t asi_count = avm_direct_us_2_asi(ctx->freq,report_param->p_ltcs_asi_buf[63]);
                            ctx->ts += asi_count;
                            bt_sink_srv_report("[sink][Line-IN]asi_cur:0x%08x, asi_count:%d", 2, ctx->us_asi, asi_count);
                            //bt_sink_srv_memset(report_param->p_ltcs_min_gap_buf, 0, 64*sizeof(int32_t));

                            gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_run_count_begin);
                            int16_t drift_value = avm_direct_cal_drift(63, report_param->p_ltcs_asi_buf,
                                (int32_t *)report_param->p_ltcs_min_gap_buf);
                            gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_run_count_end);
                            cost_dur = (gpt_run_count_end - gpt_run_count_begin) * 1000 / 32768;
                            bt_sink_srv_report("[sink][Line-IN]cal_drift--ret: %d, cost: %d, gpt_ret: %d", 3, ret, cost_dur, gpt_ret);
                            bt_sink_srv_music_update_base_parameters_to_dsp(drift_value, ctx->bt_clk.nclk, ctx->ts, ctx->ts);
                            bt_clock_t lt_clk = {0};
                            bt_get_bt_clock(gap_hd, &lt_clk);
                            bt_sink_srv_report("[sink][Line-IN]send*****modify to 63 drift_value: %d, asi:0x%08x, nclk:0x%08x, nclk_intra:0x%04x, cur_nclk:0x%08x, cur_intra:0x%08x*****", 6, 
                            (int16_t)drift_value, ctx->ts, ctx->bt_clk.nclk,
                            ctx->bt_clk.nclk_intra, lt_clk.nclk, lt_clk.nclk_intra);
                            break;
                        }
                        case AUD_A2DP_DL_REINIT_REQUEST: {
                            bt_sink_srv_report("[sink][Line-IN] recv dsp reinitial sync request", 0);
                            BT_SINK_SRV_GPIO_SET_OUTPUT(HAL_GPIO_40, HAL_GPIO_DATA_HIGH);
                            if(!ctx->rho_flag){
                                bt_sink_srv_a2dp_reinitial_sync();
                            } else {
                                bt_sink_srv_am_id_t ami_ret = bt_sink_srv_ami_audio_set_volume(ctx->a2dp_aid, ctx->vol_lev, STREAM_OUT);
                                bt_sink_srv_report("[sink][Line-IN] during RHO, just to set volume, ami_ret:%d", 1, ami_ret);
                            }
                            BT_SINK_SRV_GPIO_SET_OUTPUT(HAL_GPIO_40, HAL_GPIO_DATA_LOW);
                            break;
                        }

                        case AUD_A2DP_ACTIVE_LATENCY_REQUEST: {
                            uint32_t *latency_val = (uint32_t *)param;
                            bt_sink_srv_report("[sink][Line-IN] recv dsp active latency request, latency:%d", 1, *latency_val);
                            if(!ctx->rho_flag){
                                bt_sink_srv_music_set_sink_latency(*latency_val, false);
                                bt_sink_srv_a2dp_reinitial_sync();
                            } else {
                                bt_sink_srv_am_id_t ami_ret = bt_sink_srv_ami_audio_set_volume(ctx->a2dp_aid, ctx->vol_lev, STREAM_OUT);
                                bt_sink_srv_report("[sink][Line-IN] during RHO, just to set volume, ami_ret:%d", 1, ami_ret);
                            }
                            break;
                        }

                        default:
                            break;
                    }
                } else {
                    bt_sink_srv_report("[sink][Line-IN]ami_hdr--empty run_dev, plase note!!!", 0);
                }
                break;
            }
            default:
                break;
        }
    }

    if (ctx->a2dp_aid == aud_id && msg_id == AUD_A2DP_PROC_IND &&
        (sub_msg == AUD_STREAM_EVENT_DATA_REQ ||
         sub_msg == AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW ||
         sub_msg == AUD_CODEC_MEDIA_AWS_CHECK_UNDERFLOW)) {
        // drop
        ;
    } else {
        bt_sink_srv_report("[sink][Line-IN]ami_hdr[e]-err_code: %d, ret: %d", 2, err_code, ret);
    }
#endif
}

audio_src_srv_handle_t *audio_sink_srv_line_in_alloc_pseudo_handle(void)
{
    audio_src_srv_handle_t *hd = NULL;
    if (!(g_line_in_hd.flag & AUDIO_SINK_SRV_LINE_IN_PSEUDO_FLAG_USEED)) {
        hd = g_line_in_hd.hd;
        AUDIO_SINK_SRV_SET_FLAG(g_line_in_hd.flag, AUDIO_SINK_SRV_LINE_IN_PSEUDO_FLAG_USEED);
        audio_sink_srv_line_in_fill_audio_src_callback(hd);
    }

    audio_src_srv_report("[sink][Line-IN]alloc_pseudo_handle--hd: 0x%x", 1, hd);

    bt_sink_srv_assert(hd);
    return hd;
}


void audio_sink_srv_line_in_free_pseudo_handle(audio_src_srv_handle_t *hd)
{
    int32_t ret = 0;
    if ((hd) &&
        (g_line_in_hd.flag & AUDIO_SINK_SRV_LINE_IN_PSEUDO_FLAG_USEED) &&
        (g_line_in_hd.hd == hd)) {
        AUDIO_SINK_SRV_REMOVE_FLAG(g_line_in_hd.flag, AUDIO_SINK_SRV_LINE_IN_PSEUDO_FLAG_USEED);
        ret = AUDIO_SINK_SRV_LINE_IN_ERR_SUCCESS_1ST;
    }

    audio_src_srv_report("[sink][Line-IN]free_pseudo_handle--hd: 0x%x, ret: %d", 2, hd, ret);
    UNUSED(ret);
}


audio_sink_srv_line_in_status_t audio_sink_srv_line_in_action_handler(audio_sink_srv_line_in_action_t action, void *param)
{
    audio_sink_srv_line_in_status_t ret = 0;
    bt_bd_addr_t *dev_addr = NULL;
    uint32_t hd = 0;
    int32_t err = 0;
    audio_sink_srv_line_in_device_t *line_in_dev = NULL;
    audio_sink_srv_line_in_context_t *cntx = NULL;

    audio_src_srv_report("[sink][Line-IN]process_line_in_action[s]-action: 0x%x\n", 1, action);
    cntx = audio_sink_srv_line_in_get_context();
    line_in_dev = audio_sink_srv_line_in_get_device();

    switch (action) {
        case AUDIO_SINK_SRV_LINE_IN_ACT_DEVICE_PLUG_IN: {
            audio_sink_srv_line_in_control_init();
            audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_READY, NULL);
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_ACT_DEVICE_PLUG_OUT: {
            audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_UNAVAILABLE, NULL);
            audio_sink_srv_line_in_control_deinit();
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_ACT_TRIGGER_START: {
            bt_sink_srv_mutex_lock();
            audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_START, NULL);
            bt_sink_srv_mutex_unlock();
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_ACT_TRIGGER_STOP: {
            bt_sink_srv_mutex_lock();
            audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_STOP, NULL);
            bt_sink_srv_mutex_unlock();
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_ACT_PAUSE: {
            audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_SUSPEND, NULL);

            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_ACT_VOLUME_UP: {

            audio_sink_srv_line_in_change_volume(VOLUME_UP, 1);
            break;
        }

        case AUDIO_SINK_SRV_LINE_IN_ACT_VOLUME_DOWN: {

            audio_sink_srv_line_in_change_volume(VOLUME_DOWN, 1);
            break;
        }

        default:
            break;
    }

    audio_src_srv_report("[sink][Line-IN]process_line_in_action[e]-ret: %d, err: %d", 2, ret, err);
    UNUSED(cntx);
    UNUSED(hd);
    UNUSED(dev_addr);
    UNUSED(ret);
    UNUSED(err);
    return AUDIO_SINK_SRV_LINE_IN_STATUS_SUCCESS;
}


static void audio_sink_srv_line_in_clear_codec(audio_sink_srv_line_in_device_t *line_in_dev)
{
    int32_t ret = 0;
    audio_sink_srv_line_in_context_t *ctx = NULL;


    ctx = audio_sink_srv_line_in_get_context();

    /* Clear recover flag */
    AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->flag, AUDIO_SINK_SRV_FLAG_LINE_IN_RECOVER);

    /* 1. DRV stop */
    if (line_in_dev->op & AUDIO_SINK_SRV_LINE_IN_OP_DRV_PLAY) {
        AUDIO_SINK_SRV_SET_FLAG(line_in_dev->flag, AUDIO_SINK_SRV_FLAG_WAIT_DRV_STOP);
        /* Sync DRV stop, or codec close must wait DRV stop done(cost time) */
        audio_sink_srv_line_in_drv_stop(line_in_dev);
    }

    /* 2. Codec close */
    if (line_in_dev->op & AUDIO_SINK_SRV_LINE_IN_OP_CODEC_OPEN) {
        if (line_in_dev->flag & AUDIO_SINK_SRV_FLAG_LINE_IN_RECOVER) {
            /* Skip codec close */
            ;
        } else {
            ret = bt_sink_srv_ami_audio_stop(ctx->line_in_aid);
            if (ret != AUD_EXECUTION_SUCCESS) {
                /* Failed close codec */
                audio_src_srv_report("[sink][a2dp]clear_codec(error)--ret: %d", 1, ret);
            } else {
                AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->op, AUDIO_SINK_SRV_LINE_IN_OP_CODEC_OPEN);
            }
        }
    }
#if 0
    /* 3. Transfer suspend SPK2 */

    /* 4. Clear flag */
    /* Clear wait start gpt timer */
    AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
    /* Clear wait start gpt timeout */
    AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_GPT_TIMEOUT);
    /* Clear recover flag */
    //BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);
#endif
}

void audio_sink_srv_line_in_drv_play(void *param)
{
    audio_sink_srv_line_in_device_t *line_in_dev = (audio_sink_srv_line_in_device_t *)param;
    audio_sink_srv_line_in_context_t *ctx = NULL;
    uint8_t dev_db = 0;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    int32_t ret = 0;
    uint32_t gpt_run_count_begin = 0;
    uint32_t gpt_run_count_end = 0;
    int32_t gpt_ret = 0;
    uint32_t cost_dur = 0;

    ctx = audio_sink_srv_line_in_get_context();

    if (ctx->run_dev == line_in_dev) {
        AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->flag, AUDIO_SINK_SRV_FLAG_WAIT_DRV_PLAY);
        med_hd = &(line_in_dev->med_handle);

        gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_run_count_begin);
        ret = med_hd->play(ctx->line_in_aid);
        gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_run_count_end);
        cost_dur = (gpt_run_count_end - gpt_run_count_begin) * 1000 / 32768;
        audio_src_srv_report("[sink][Line-IN]drv_play--ret: %d, cost: %d, gpt_ret: %d", 3, ret, cost_dur, gpt_ret);
        if (BT_CODEC_MEDIA_STATUS_OK == ret) {
            AUDIO_SINK_SRV_SET_FLAG(line_in_dev->op, AUDIO_SINK_SRV_LINE_IN_OP_DRV_PLAY);
            AUDIO_SINK_SRV_SET_FLAG(line_in_dev->flag, AUDIO_SINK_SRV_FLAG_LINE_IN_PLAYING);
            //bt_sink_srv_state_set(BT_SINK_SRV_STATE_STREAMING);
            //bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_PLAYING, NULL);
        } else {
            /* Error handle */
            audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_PREPARE_FAIL, NULL);
        }
    }
    UNUSED(dev_db);
    UNUSED(gpt_ret);
    UNUSED(cost_dur);
}


void audio_sink_srv_line_in_drv_stop(void *param)
{
    audio_sink_srv_line_in_device_t *line_in_dev = (audio_sink_srv_line_in_device_t *)param;
    audio_sink_srv_line_in_context_t *ctx = NULL;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    int32_t ret = 0;

    ctx = audio_sink_srv_line_in_get_context();

    if (ctx->run_dev == line_in_dev) {
        med_hd = &(line_in_dev->med_handle);
        AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->flag, AUDIO_SINK_SRV_FLAG_WAIT_DRV_STOP);
        ret = med_hd->stop(ctx->line_in_aid);
        audio_src_srv_report("[sink][Line-IN]drv_stop-ret: 0x%x", 1, ret);
        if (BT_CODEC_MEDIA_STATUS_OK == ret) {
            /* Remove DRV play flag */
            AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->op, AUDIO_SINK_SRV_LINE_IN_OP_DRV_PLAY);
            /* Remove play flag */
            AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->flag, AUDIO_SINK_SRV_FLAG_LINE_IN_PLAY);
            AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->flag, AUDIO_SINK_SRV_FLAG_LINE_IN_PLAYING);
            //bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
        } else {
            /* Error handle */
            audio_src_srv_report("[sink][Line-IN]drv_stop(error)--ret: %d", 1, ret);
        }
    }
}


int32_t audio_sink_srv_line_in_change_volume(uint8_t type, uint8_t in_out)
{
    int32_t ret = AUDIO_SINK_SRV_LINE_IN_ERR_SUCCESS_OK;
    bt_sink_srv_am_id_t ami_ret = 0;
    uint8_t vol = AUD_VOL_OUT_LEVEL0;
    audio_sink_srv_line_in_context_t *ctx = NULL;
    audio_sink_srv_line_in_device_t *line_in_dev = NULL;
    uint8_t dev_db = 0;


    ctx = audio_sink_srv_line_in_get_context();
    line_in_dev = audio_sink_srv_line_in_get_device();

    vol = ctx->vol_lev;
    /* volume up */
    if (VOLUME_UP == type) {
       if (vol < AUDIO_SINK_SRV_LINE_IN_MAX_VOL_LEV) {
           vol = vol + 1;
       } else {
           ret = AUDIO_SINK_SRV_LINE_IN_ERR_FAIL_1ST;
       }
    } else if (VOLUME_DOWN == type) {
        if (vol > AUDIO_SINK_SRV_LINE_IN_MIN_VOL_LEV) {
            vol = vol - 1;
        } else {
            ret = AUDIO_SINK_SRV_LINE_IN_ERR_FAIL_2ND;
        }
    }

    if ((vol != ctx->vol_lev) && line_in_dev) {
        ami_ret = bt_sink_srv_ami_audio_set_volume(ctx->line_in_aid, vol, STREAM_OUT);
        ctx->vol_lev = vol;
    }

    audio_src_srv_report("[sink][Line-IN]change_volume-ami_ret: %d, ret: %d, vol: %d", 3,
        ami_ret, ret, ctx->vol_lev);
    UNUSED(dev_db);
    UNUSED(ret);
    UNUSED(ami_ret);
    return ret;
}

/*****************************************************************************************/
void audio_sink_srv_line_in_play(audio_src_srv_handle_t *handle)
{
    audio_sink_srv_line_in_context_t *ctx = NULL;
    audio_sink_srv_line_in_device_t *line_in_dev = NULL;
    ctx = audio_sink_srv_line_in_get_context();
    line_in_dev = audio_sink_srv_line_in_get_device();

    bt_sink_srv_am_audio_capability_t aud_cap = {0};
    bt_sink_srv_am_result_t am_ret;
    int32_t ret = 0;
    uint8_t dev_db = 0;
    bt_sink_srv_assert(line_in_dev);

    AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->flag, AUDIO_SINK_SRV_FLAG_WAIT_LIST_SINK_PLAY);

    audio_src_srv_report("[sink][Line-IN]play(s)--hd: 0x%08x, type: %d, flag: 0x%08x, op: 0x%08x", 4,
        handle, handle->type, line_in_dev->flag, line_in_dev->op);

    if (line_in_dev->op & AUDIO_SINK_SRV_LINE_IN_OP_HF_INTERRUPT) {
        AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->op, AUDIO_SINK_SRV_LINE_IN_OP_HF_INTERRUPT);
        AUDIO_SINK_SRV_SET_FLAG(line_in_dev->op, AUDIO_SINK_SRV_LINE_IN_OP_PLAY_TRIGGER);
        line_in_dev->handle->substate = AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_CODEC;
        audio_src_srv_report("[sink][Line-IN]play(s)--HF int resume", 0);
    }
#if 0
    if (line_in_dev->flag & AUDIO_SINK_SRV_FLAG_LINE_IN_INTERRUPT) {
        AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->flag, AUDIO_SINK_SRV_FLAG_LINE_IN_INTERRUPT);
        audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_RESUME, NULL);
        return ;
    }

    if (line_in_dev->flag & AUDIO_SINK_SRV_FLAG_LINE_IN_RECOVER) {
        /* Update run device */
        //bt_sink_srv_music_update_run_device(line_in_dev);
        audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_CODEC_OPEN, NULL);
        audio_src_srv_report("[sink][Line-IN]play(recover)--flag: 0x%x", 1, line_in_dev->flag);
        return ;
    }
#endif
    /* Audio source accept play request */
    AUDIO_SINK_SRV_SET_FLAG(line_in_dev->flag, AUDIO_SINK_SRV_FLAG_WAIT_AMI_OPEN_CODEC);
    /* Update run device */
    //bt_sink_srv_music_update_run_device(line_in_dev);
    /* 1. Open A2DP codec */
    if(0){
        ctx->vol_lev = (dev_db & 0x0f);
    } else {
        /* use dedefault volume value and update it */
        ctx->vol_lev = AUDIO_SINK_SRV_LINE_IN_VOL_DEF_LEV;
    }

    audio_sink_srv_line_in_fill_am_aud_param(&aud_cap, &line_in_dev->codec.codec_cap);

    int32_t gpt_ret = 0;
    gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &g_line_in_gpt_codec_run_count_begin);
    audio_src_srv_report("[sink][Line-IN]drv_play--begin: %d, gpt_ret: %d", 2, g_line_in_gpt_codec_run_count_begin, gpt_ret);

    am_ret = bt_sink_srv_ami_audio_play(ctx->line_in_aid, &aud_cap);
    if (AUD_EXECUTION_SUCCESS != am_ret) {
        /* Exception: play fail */
        AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->flag, AUDIO_SINK_SRV_FLAG_WAIT_AMI_OPEN_CODEC);
        //bt_sink_srv_music_update_run_device(NULL);
        audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_PREPARE_FAIL, NULL);
    }

    AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->op, AUDIO_SINK_SRV_LINE_IN_OP_PLAY_TRIGGER);
	AUDIO_SINK_SRV_SET_FLAG(line_in_dev->op, AUDIO_SINK_SRV_LINE_IN_OP_CODEC_OPEN);//ToDO AM notify
    UNUSED(ret);
    UNUSED(gpt_ret);
}

void audio_sink_srv_line_in_stop(audio_src_srv_handle_t *handle)
{
    audio_sink_srv_line_in_device_t *ori_dev = NULL, *cur_dev = NULL;
    audio_sink_srv_line_in_context_t *ctx = NULL;
    audio_sink_srv_line_in_device_t *line_in_dev = NULL;
    ctx = audio_sink_srv_line_in_get_context();
    line_in_dev = audio_sink_srv_line_in_get_device();

    bt_sink_srv_assert(line_in_dev);

    ori_dev = ctx->run_dev;

    audio_src_srv_report("[sink][Line-IN]stop(s)--hd: 0x%x, type: %d, flag: 0x%x, op: 0x%x", 4,
        handle, handle->type, line_in_dev->flag, line_in_dev->op);



    /* Clear codec */
    audio_sink_srv_line_in_clear_codec(line_in_dev);

    /* Clear done */
    audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_CODEC_CLEAR, NULL);

    audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_READY, NULL);

    cur_dev = ctx->run_dev;
    UNUSED(cur_dev);
    UNUSED(ori_dev);
}


void audio_sink_srv_line_in_suspend(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd)
{
    audio_sink_srv_line_in_device_t *line_in_dev = NULL;
    audio_sink_srv_line_in_device_t *ori_dev = NULL, *cur_dev = NULL;
    int32_t ret = 0, err = 0;
    audio_sink_srv_line_in_context_t *ctx = audio_sink_srv_line_in_get_context();

    line_in_dev = audio_sink_srv_line_in_get_device();

    bt_sink_srv_assert(line_in_dev);

    //ori_dev = ctx->run_dev;

    audio_src_srv_report("[sink][Line-IN]suspend(s)--hd: 0x%x, type: %d, int: 0x%x, type: %d, flag: 0x%x, op: 0x%x", 6,
        handle, handle->type, int_hd, int_hd->type, line_in_dev->flag, line_in_dev->op);

    /* Clear codec */
    audio_sink_srv_line_in_clear_codec(line_in_dev);
    //AUDIO_SINK_SRV_REMOVE_FLAG(line_in_dev->flag, BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC);

    /* Clear done */
    if(line_in_dev->handle->substate != AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_CLEAR_CODEC) {
        audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_CODEC_CLEAR, NULL);
    }

    if ((handle->dev_id == int_hd->dev_id) &&
        (int_hd->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_HFP)) {
    } else if ((int_hd->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP)){
        err = AUDIO_SINK_SRV_LINE_IN_ERR_SUCCESS_2ND;
    } else {
        /* Add self in waiting list */
        audio_src_srv_add_waiting_list(handle);
        /* Set interrupt flag */
        AUDIO_SINK_SRV_SET_FLAG(line_in_dev->flag, AUDIO_SINK_SRV_FLAG_LINE_IN_INTERRUPT);
        /* Send pause cmd */
        err = AUDIO_SINK_SRV_LINE_IN_ERR_SUCCESS_3RD;
    }
/*
    cur_dev = ctx->run_dev;
    if(!(cur_dev->op & AUDIO_SINK_SRV_LINE_IN_OP_CODEC_OPEN)) {
        audio_src_srv_report("[sink][Line-IN]test haven`t open codec success, cur_dev:0x%08x", 1, cur_dev);
    }

    if (ori_dev == cur_dev && !(cur_dev->flag & AUDIO_SINK_SRV_FLAG_WAIT_AMI_OPEN_CODEC)) {
        //bt_sink_srv_music_update_run_device(NULL);
    }
*/
    audio_src_srv_report("[sink][Line-IN]suspend(e)--ret: %d, err: %d", 2, ret, err);
    UNUSED(ctx);
    UNUSED(cur_dev);
    UNUSED(ori_dev);
    UNUSED(ret);
    UNUSED(err);
}

void audio_sink_srv_line_in_reject(audio_src_srv_handle_t *handle)
{
    //bt_sink_srv_mutex_lock();
    audio_sink_srv_line_in_device_t *line_in_dev = NULL;
    audio_sink_srv_line_in_context_t *ctx = audio_sink_srv_line_in_get_context();
    line_in_dev = audio_sink_srv_line_in_get_device();
    const audio_src_srv_handle_t *rej_handle = audio_src_srv_get_runing_pseudo_device();

    bt_sink_srv_assert(line_in_dev);
    bt_sink_srv_assert(rej_handle);

    audio_src_srv_report("[sink][Line-IN]reject(s)--hd: 0x%x, type: %d, flag: 0x%x, op: 0x%x", 4,
        handle, handle->type, line_in_dev->flag, line_in_dev->op);

    if(rej_handle->dev_id == handle->dev_id) {
        audio_src_srv_report("[sink][Line-IN]Rejected by same device, add to waiting list", 0);
        AUDIO_SINK_SRV_SET_FLAG(line_in_dev->flag, AUDIO_SINK_SRV_FLAG_WAIT_LIST_SINK_PLAY);
        audio_src_srv_add_waiting_list(handle);
    } else {
    }
    audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_REJECT, NULL);
    UNUSED(ctx);
    /* Notify state machine reject reason */
    //bt_sink_srv_mutex_unlock();
}


void audio_sink_srv_line_in_exception(audio_src_srv_handle_t *handle, int32_t event, void *param)
{
}


void audio_sink_srv_line_in_create_handle(void)
{
    g_line_in_hd.hd = audio_src_srv_construct_handle(AUDIO_SRC_SRV_PSEUDO_DEVICE_LINE_IN);
}


void audio_sink_srv_line_in_destroy_handle(void)
{
    if(g_line_in_hd.hd != NULL) {
        audio_src_srv_destruct_handle(g_line_in_hd.hd);
        g_line_in_hd.hd = NULL;
    } else {
        audio_src_srv_report("[sink][Line-IN]audio_sink_srv_line_in_destroy_handle: linein handle is NULL, no need to destory", 0);
    }
}


int32_t audio_sink_srv_line_in_set_volume(uint8_t volume, audio_sink_srv_line_in_device_t *sp_dev)
{
    audio_sink_srv_line_in_context_t *cntx = audio_sink_srv_line_in_get_context();;
    audio_sink_srv_line_in_device_t *run_dev = audio_sink_srv_line_in_get_device();
    uint8_t new_vol = volume;/*bt_sink_srv_avrcp_get_volume_level(volume)*/
    uint8_t old_vol = cntx->vol_lev;
    int32_t ret = AUDIO_SINK_SRV_LINE_IN_ERR_SUCCESS_OK;
    bt_sink_srv_am_id_t ami_ret = 0;
    uint8_t dev_db = 0;

    audio_src_srv_report("[sink][Line-IN]set_volume[s]-new_vol: %d, old_vol: %d", 2, new_vol, old_vol);
    bt_sink_srv_assert(sp_dev&&"sp_dev is NULL why?");

    cntx->vol_lev = new_vol;

    if ((old_vol != new_vol) && run_dev && run_dev == sp_dev) {
        if(run_dev->op & AUDIO_SINK_SRV_LINE_IN_OP_CODEC_OPEN) {
            ami_ret = bt_sink_srv_ami_audio_set_volume(cntx->line_in_aid, new_vol, STREAM_OUT);
        } else {
            AUDIO_SINK_SRV_SET_FLAG(run_dev->flag, AUDIO_SINK_SRV_FLAG_WAIT_SET_VOLUME);
        }
    }

    audio_src_srv_report("[sink][Line-IN]set_volume-ami_ret: %d, ret: %d, vol: %d", 3,
        ami_ret, ret, cntx->vol_lev);
    UNUSED(dev_db);
    UNUSED(ami_ret);
    return ret;
}

