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

#include "types.h"
#include "hal_nvic.h"
#include "long_term_clk_skew.h"
#include "hal_audio_afe_define.h"
#include "dsp_utilities.h"
#include "dsp_audio_msg.h"
#include "memory_attribute.h"
#include <stdio.h>
#include <string.h>
#include "clk_skew.h"
//#define LTCS_UT

#define LTCS_CYCLE_NO           64
#define LTCS_PREDIC_CYCLE_NO    63

#define LTCS_OBSERVE_CYCLE      32
#define LTCS_ASI_OBSERVE_CYCLE  128


ltcs_header_type_t ltcs_anchor, ltcs_previous, ltcs_receive;
ltcs_ctrl_type_t ltcs_ctrl;

#ifdef LTCS_UT
DSP_ALIGN4 int asi_buf[64];
DSP_ALIGN4 int min_gap_buf[64];
#endif

void lt_clk_skew_reset_info (void);
U32 lt_clk_skew_calculate_audio_sample_count (ltcs_header_type_ptr_t receive_pkt);
S64 lt_clk_skew_calculate_ideal_play_time (ltcs_header_type_ptr_t receive_pkt);
S32 lt_clk_skew_calculate_gap (ltcs_header_type_ptr_t receive_pkt);
U32 lt_clk_skew_update_min_gap (ltcs_header_type_ptr_t receive_pkt);
void lt_clk_skew_update_anchor (ltcs_header_type_ptr_t receive_pkt);
void lt_clk_skew_write_val_to_buf (U32* buf, U32 val, U32 idx);
static void lt_clk_skew_send_data_update(void);
#ifdef MTK_PCDC_TIMEOUT_ENABLE
void lt_clk_skew_send_timeout_request(void);
int lt_clk_skew_timeout_check(int asi_threshold, int cur_asi, int sample_rate);
#endif

void lt_clk_skew_reset_info (void)
{
    ltcs_ctrl.packet_index = 0;
    ltcs_ctrl.cycle_index = 0;
    ltcs_ctrl.gap_min = 0x7FFFFFFF;
    ltcs_ctrl.drift_val = 0;
    ltcs_ctrl.notify = FALSE;
    ltcs_ctrl.init = TRUE;
    ltcs_ctrl.actual_drift_value = 0;

    #ifdef LTCS_UT
    ltcs_ctrl.sample_rate = 44100;
    ltcs_ctrl.asi_buf = asi_buf;
    ltcs_ctrl.min_gap_buf = min_gap_buf;
    #endif
}

void lt_clk_skew_set_sample_rate (U32 sample_rate)
{
    ltcs_ctrl.sample_rate = sample_rate;
}

void lt_clk_skew_set_min_gap_buf (U32* buf)
{
    #ifndef LTCS_UT
    ltcs_ctrl.min_gap_buf = buf;
    #endif
}

void lt_clk_skew_set_asi_buf (U32* buf)
{
    #ifndef LTCS_UT
    ltcs_ctrl.asi_buf = buf;
    #endif
}

void lt_clk_skew_set_sink_latency (U32 sink_latency)
{
    ltcs_ctrl.sink_latency = sink_latency;
}


void lt_clk_skew_update_actual_dsp_drift (void)
{
    ltcs_ctrl.actual_drift_value = ltcs_ctrl.drift_val;
    DSP_MW_LOG_I("[LTCS] DSP current drift value:%d \r\n", 1, ltcs_ctrl.actual_drift_value);
}


int lt_clk_skew_get_dsp_drift_value (void)
{
    return ltcs_ctrl.actual_drift_value;
}


ATTR_TEXT_IN_IRAM_LEVEL_2 void lt_clk_skew_set_drift_value (S32 drift_val)
{
    if (ltcs_ctrl.drift_val != drift_val) {
         ltcs_ctrl.drift_val = drift_val;
         DSP_MW_LOG_I("[LTCS][DSP] Estimated drift value:%d \r\n", 1, drift_val);
    }
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void lt_clk_skew_set_ts_ratio (S32 ratio)
{
    if ((ltcs_ctrl.ts_ratio != 0) && (ratio != 0)) {

        if (ltcs_ctrl.ts_ratio != ratio) {
            ltcs_ctrl.ts_ratio = ratio;
            DSP_MW_LOG_I("[LTCS] TS ratio:%d \r\n", 1, ratio);
        }
    }
    else {
        ltcs_ctrl.ts_ratio = 1;
    }
}


ATTR_TEXT_IN_IRAM_LEVEL_2 U32 lt_clk_skew_calculate_audio_sample_count (ltcs_header_type_ptr_t receive_pkt)
{
    int diff_seq_no = 0;

    switch (receive_pkt->a2dp_audio_type) {
        case LTCS_TYPE_AAC:
        case LTCS_TYPE_IOS_AAC:
        case LTCS_TYPE_ANDROID_AAC:
            diff_seq_no
                = (receive_pkt->seqno >= ltcs_anchor.seqno)
                    ? (receive_pkt->seqno - ltcs_anchor.seqno)
                        : (receive_pkt->seqno + 65536 - ltcs_anchor.seqno); /* seq no wrap */
            return 1024 * (U32)diff_seq_no;
        case LTCS_TYPE_SBC:
            return (receive_pkt->timestamp - ltcs_anchor.timestamp) / (ltcs_ctrl.ts_ratio);
        case LTCS_TYPE_IOS_SBC:
            diff_seq_no
                = (receive_pkt->seqno >= ltcs_anchor.seqno)
                    ? (receive_pkt->seqno - ltcs_anchor.seqno)
                        : (receive_pkt->seqno + 65536 - ltcs_anchor.seqno); /* seq no wrap */
            return diff_seq_no * receive_pkt->timestamp * 128; /*receive_pkt->timestamp: num of frames in a packet*/
        case LTCS_TYPE_VENDOR:
            return (receive_pkt->timestamp - ltcs_anchor.timestamp) / (ltcs_ctrl.ts_ratio);
        case LTCS_TYPE_DSP_CNT:
            return (receive_pkt->timestamp - ltcs_anchor.timestamp);
        case LTCS_TYPE_OTHERS:
        default:
            return 0;
    }
}
ATTR_TEXT_IN_IRAM_LEVEL_2 S32 lt_clk_skew_convert_asi_to_seq ( int asi_diff, int seqno)
{
    int result_seq_no = 0;

    switch (ltcs_receive.a2dp_audio_type) {
        case LTCS_TYPE_AAC:
        case LTCS_TYPE_IOS_AAC:
        case LTCS_TYPE_ANDROID_AAC:
        	result_seq_no = asi_diff / (1024) + seqno;
          break;
        case LTCS_TYPE_SBC:
        	return result_seq_no = 0x7FFFFFFF;
          break;
        case LTCS_TYPE_IOS_SBC:
          result_seq_no = asi_diff / (ltcs_receive.timestamp * 128) + seqno;/*receive_pkt->timestamp: num of frames in a packet*/
          break;  
        case LTCS_TYPE_VENDOR:
        case LTCS_TYPE_DSP_CNT:
        case LTCS_TYPE_OTHERS:
        default:
        	return result_seq_no = 0x7FFFFFFF;
          break;
            
            

    }   
    DSP_MW_LOG_I("[LTCS] Pre_seqno is %d, after is %d ", 2,seqno  ,result_seq_no );
    
    if (result_seq_no >= 65536)
      result_seq_no = result_seq_no - 65536;
    if (result_seq_no < 0)    
      result_seq_no = result_seq_no + 65536;
    return result_seq_no;	  
}
ATTR_TEXT_IN_IRAM_LEVEL_2 S32 lt_clk_skew_convert_asi_to_ts ( int asi_diff, int timestamp)
{
    int result_ts = 0;
    switch (ltcs_receive.a2dp_audio_type) {
        case LTCS_TYPE_AAC:
        case LTCS_TYPE_IOS_AAC:
        case LTCS_TYPE_ANDROID_AAC:
        	return result_ts = 0x7FFFFFFF;
          break;
        case LTCS_TYPE_SBC:
        	result_ts = timestamp + asi_diff * ltcs_ctrl.ts_ratio ;
          break;
        case LTCS_TYPE_IOS_SBC:
          return result_ts = 0x7FFFFFFF;
          break;
        case LTCS_TYPE_VENDOR:
         	result_ts = timestamp + asi_diff * ltcs_ctrl.ts_ratio ;
          break;
        case LTCS_TYPE_DSP_CNT:
            result_ts = timestamp + asi_diff;
          break;
        case LTCS_TYPE_OTHERS:
        default:
        	return result_ts = 0x7FFFFFFF;
          break;
            
            

    }   
    DSP_MW_LOG_I("[LTCS] Pre_ts is %d, after is %d ", 2,timestamp  ,result_ts );

    return result_ts;	  
}

S64 lt_clk_skew_calculate_ideal_play_time (ltcs_header_type_ptr_t receive_pkt)
{
    U32 sample_count = lt_clk_skew_calculate_audio_sample_count(receive_pkt);
    S64 play_time = 0;

    if (sample_count) {
        play_time = (S64)(((U64)sample_count * 1000000) / ltcs_ctrl.sample_rate);
        return (((S64)ltcs_anchor.btclk * 625) / 2 + play_time + (S64)((play_time * (S64)ltcs_ctrl.drift_val) / 1000000));
    }
    else { /* can not support this a2dp_audio_type */
        return ((S64)receive_pkt->btclk * 625 / 2);
    }
}

S32 lt_clk_skew_calculate_gap (ltcs_header_type_ptr_t receive_pkt)
{
    S64 ideal_play_time = lt_clk_skew_calculate_ideal_play_time(receive_pkt);
    S64 receive_btc;

    if ((ltcs_anchor.btclk > (int)(1<<27)) && (receive_pkt->btclk < (int)(1<<27))) { /* btc wrap */
        receive_btc = ((S64)receive_pkt->btclk + (S64)(1<<28)) * 625 / 2;
    }
    else {
        receive_btc = ((S64)receive_pkt->btclk) * 625 / 2;
    }

    return (S32)(receive_btc - ideal_play_time);
}

ATTR_TEXT_IN_IRAM_LEVEL_2 U32 lt_clk_skew_update_min_gap (ltcs_header_type_ptr_t receive_pkt)
{
    S32 current_gap = lt_clk_skew_calculate_gap(receive_pkt);

    ltcs_ctrl.gap_min
        = (current_gap < ltcs_ctrl.gap_min)
            ? current_gap
                : ltcs_ctrl.gap_min;

    return ltcs_ctrl.gap_min;
}

void lt_clk_skew_update_anchor (ltcs_header_type_ptr_t receive_pkt)
{
    //if (ltcs_anchor.btclk != receive_pkt.btclk) {
    if (1) {
        memcpy(&ltcs_anchor.seqno, &receive_pkt->seqno, sizeof(ltcs_header_type_t)-sizeof(int));
    }
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void lt_clk_skew_update_anchor_clock (int btclk)
{
    int ideal_btclk;

    if (btclk == 0) {
        return;
    }

    ideal_btclk = btclk - ltcs_ctrl.sink_latency * 2 / 625; // us convert to bt half slot

    if (ltcs_anchor.btclk != ideal_btclk) {
        ltcs_anchor.btclk = ideal_btclk;
        DSP_MW_LOG_I("[LTCS] New Anchor: 0x%x \r\n", 1, ideal_btclk);
    }
}

void lt_clk_skew_set_asi_threshold (int threshold)
{
    ltcs_ctrl.asi_threshold = threshold;
}

int lt_clk_skew_get_asi_threshold (int sample_rate)
{
	if(ltcs_ctrl.asi_threshold == 0x7FFFFFFF)
    {
         return ltcs_ctrl.asi_threshold;
	}
	else
    {
         //DSP_MW_LOG_I("[LTCS][DSP] asi thd:%d, fs:%d %d", 3, ltcs_ctrl.asi_threshold, sample_rate, ltcs_ctrl.sample_rate);
         return (S32)((S64)ltcs_ctrl.asi_threshold*sample_rate/ltcs_ctrl.sample_rate);
    }
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void lt_clk_skew_update_base_asi (uint32_t base_asi, uint32_t curr_asi)
{
    if (ltcs_ctrl.init == TRUE)
    {
        ltcs_ctrl.base_asi = curr_asi;
        ltcs_ctrl.prev_base_asi = base_asi;
        ltcs_ctrl.flag_2048 = 0;
        DSP_MW_LOG_I("[LTCS][DSP] PlayEn base ASI: 0x%x, curr ASI: 0x%x \r\n", 2, base_asi, curr_asi);

        lt_clk_skew_update_actual_dsp_drift();
        #ifdef MTK_BT_HFP_SPE_ALG_V2
        ltcs_ctrl.init = FALSE;
        #endif
    }

    if (ltcs_ctrl.prev_base_asi != base_asi) {
        lt_clk_skew_set_asi_threshold((base_asi - ltcs_ctrl.base_asi)&0x3FFFFFF);
        DSP_MW_LOG_I("[LTCS][DSP] base ASI: 0x%x, curr ASI: 0x%x, last_base ASI:0x%x, threshold:0x%x", 4, base_asi, curr_asi, ltcs_ctrl.base_asi, (base_asi - ltcs_ctrl.base_asi)&0x3FFFFFF);

        #ifndef MTK_BT_HFP_SPE_ALG_V2
        ltcs_anchor.seqno = lt_clk_skew_convert_asi_to_seq (base_asi - ltcs_ctrl.base_asi , ltcs_anchor.seqno);
        ltcs_anchor.timestamp = lt_clk_skew_convert_asi_to_ts (base_asi - ltcs_ctrl.base_asi , ltcs_anchor.timestamp);
        #endif

        ltcs_ctrl.base_asi = base_asi;
        ltcs_ctrl.prev_base_asi = ltcs_ctrl.base_asi;

        // kevin add
        ltcs_ctrl.flag_2048 = 1; // a sign of update anchor
        //ltcs_anchor.timestamp = base_asi;
        #ifndef MTK_BT_HFP_SPE_ALG_V2
        DSP_MW_LOG_I("[LTCS] New ltcs_anchor.seq_no :0x%x,New ltcs_anchor.timestamp:%x\r\n", 2,  ltcs_anchor.seqno, ltcs_anchor.timestamp);
        #endif
        DSP_MW_LOG_I("[LTCS][DSP] New Base ASI:0x%x, ASI samples:0x%x\r\n", 2, base_asi, lt_clk_skew_get_asi_threshold(ltcs_ctrl.sample_rate));
    }
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void lt_clk_skew_gather_pkt_info (int btclk, int timestamp, int seqno, ltcs_bt_type_t a2dp_audio_type)
{
    uint32_t mask;

    hal_nvic_save_and_set_interrupt_mask(&mask); // packet info gathering should be atomic

    if (btclk != ltcs_receive.btclk) {
        ltcs_receive.btclk = btclk;
        ltcs_receive.timestamp = (a2dp_audio_type == LTCS_TYPE_DSP_CNT) ? (int)ltcs_ctrl.local_asi : timestamp;
        ltcs_receive.seqno = seqno;
        ltcs_receive.a2dp_audio_type = a2dp_audio_type;
    }

    hal_nvic_restore_interrupt_mask(mask);
}

void lt_clk_skew_write_val_to_buf (U32* buf, U32 val, U32 idx)
{
    buf[idx] = val;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void lt_clk_skew_gather_regression_info (void)
{
    S32 min_gap = 0;
    U32 asi = 0;
    U32 sample_count = 0;
    U16 i = 0;
    U16 observe_thd;
    if (ltcs_receive.btclk == ltcs_previous.btclk) {
        return; // do not use same packet more than once
    }
    if (ltcs_ctrl.flag_2048 == 1)
    {
    	if( ltcs_anchor.timestamp <= ltcs_receive.timestamp)
    	{
    		ltcs_ctrl.flag_2048 = 0;
    		ltcs_ctrl.cycle_index = 0;
    		ltcs_ctrl.packet_index = 0;
    	}
        else if(ltcs_anchor.seqno <= ltcs_receive.seqno)
    	{
    		ltcs_ctrl.flag_2048 = 0;
    		ltcs_ctrl.cycle_index = 0;
    		ltcs_ctrl.packet_index = 0;
    		
    	}
    	else
    		return;
    }	 
    uint32_t mask;

    hal_nvic_save_and_set_interrupt_mask(&mask); // packet info gathering should be atomic

    if (ltcs_ctrl.init == TRUE) {

        DSP_MW_LOG_I("[LTCS] First Packet BTC: 0x%x \r\n", 1, ltcs_receive.btclk);

        lt_clk_skew_update_anchor(&ltcs_receive);
        ltcs_ctrl.local_asi = ltcs_receive.timestamp;

        ltcs_ctrl.init = FALSE;
    }

    min_gap = lt_clk_skew_update_min_gap(&ltcs_receive); // in us
    sample_count = lt_clk_skew_calculate_audio_sample_count(&ltcs_receive);

    ltcs_ctrl.packet_index++;
    observe_thd = (ltcs_receive.a2dp_audio_type == LTCS_TYPE_DSP_CNT)? LTCS_ASI_OBSERVE_CYCLE : LTCS_OBSERVE_CYCLE;

    if (ltcs_ctrl.packet_index == observe_thd) {

        // write asi
        asi = (U32)(((U64)sample_count * 1000000) / ltcs_ctrl.sample_rate); // in us
        #ifdef LTCS_UT
        lt_clk_skew_write_val_to_buf(asi_buf, asi, ltcs_ctrl.cycle_index);
        #else
        lt_clk_skew_write_val_to_buf(ltcs_ctrl.asi_buf, asi, ltcs_ctrl.cycle_index);
        #endif

        // write min gap
        #ifdef LTCS_UT
        lt_clk_skew_write_val_to_buf(min_gap_buf, min_gap, ltcs_ctrl.cycle_index);
        #else
        lt_clk_skew_write_val_to_buf(ltcs_ctrl.min_gap_buf, min_gap, ltcs_ctrl.cycle_index);
        #endif

        //printf("[LTCS] min GAP:%x \r\n", min_gap);

        ltcs_ctrl.gap_min = 0x7FFFFFFF;
        ltcs_ctrl.packet_index = 0;
        ltcs_ctrl.cycle_index++;

        if (ltcs_ctrl.cycle_index == LTCS_PREDIC_CYCLE_NO) {

            switch (ltcs_receive.a2dp_audio_type) {
                case LTCS_TYPE_AAC:
                case LTCS_TYPE_IOS_AAC:
                case LTCS_TYPE_ANDROID_AAC:
                    for (i = 0 ; i < (LTCS_CYCLE_NO - LTCS_PREDIC_CYCLE_NO) ; i++) {
                        sample_count += 1024 * observe_thd; // AAC only
                        asi = (U32)(((U64)sample_count * 1000000) / ltcs_ctrl.sample_rate); // in us
                        pcdc_asi_threshold = sample_count;
                        DSP_MW_LOG_I("[LTCS][sourcesize] sample_count: %d, asi:%d", 2, sample_count, asi);
                        ltcs_ctrl.seqno_anchor = ltcs_receive.seqno + observe_thd;
                        if(ltcs_ctrl.seqno_anchor >= 65536)
                        	 ltcs_ctrl.seqno_anchor = ltcs_ctrl.seqno_anchor - 65536;
                        lt_clk_skew_write_val_to_buf(ltcs_ctrl.asi_buf, asi, ltcs_ctrl.cycle_index + i);
                        lt_clk_skew_write_val_to_buf(ltcs_ctrl.min_gap_buf, min_gap, ltcs_ctrl.cycle_index + i);
                    }
                    break;
                case LTCS_TYPE_SBC:
                    for (i = 0 ; i < (LTCS_CYCLE_NO - LTCS_PREDIC_CYCLE_NO) ; i++) {
                        sample_count += (ltcs_receive.timestamp - ltcs_previous.timestamp) / (ltcs_ctrl.ts_ratio) * observe_thd; //
                        ltcs_ctrl.timestamp_anchor =  ltcs_receive.timestamp + (ltcs_receive.timestamp - ltcs_previous.timestamp) * observe_thd;
                        asi = (U32)(((U64)sample_count * 1000000) / ltcs_ctrl.sample_rate); // in us
                        pcdc_asi_threshold = sample_count;
                        DSP_MW_LOG_I("[LTCS][sourcesize] sample_count: %d, asi:%d, ltcs_ctrl.timestamp_anchor:%d", 2, sample_count, asi, ltcs_ctrl.timestamp_anchor);
                        lt_clk_skew_write_val_to_buf(ltcs_ctrl.asi_buf, asi, ltcs_ctrl.cycle_index + i);
                        lt_clk_skew_write_val_to_buf(ltcs_ctrl.min_gap_buf, min_gap, ltcs_ctrl.cycle_index + i);
                    }
                    break;
                case LTCS_TYPE_IOS_SBC:
                    for (i = 0 ; i < (LTCS_CYCLE_NO - LTCS_PREDIC_CYCLE_NO) ; i++) {
                        sample_count += ltcs_receive.timestamp * 128 * observe_thd; // IOS SBC only
                        asi = (U32)(((U64)sample_count * 1000000) / ltcs_ctrl.sample_rate); // in us
                        pcdc_asi_threshold = sample_count;
                        DSP_MW_LOG_I("[LTCS][sourcesize] sample_count: %d, asi:%d", 2, sample_count, asi);
                        ltcs_ctrl.seqno_anchor = ltcs_receive.seqno + observe_thd;
                        if(ltcs_ctrl.seqno_anchor >= 65536)
                        	 ltcs_ctrl.seqno_anchor = ltcs_ctrl.seqno_anchor - 65536;
                        lt_clk_skew_write_val_to_buf(ltcs_ctrl.asi_buf, asi, ltcs_ctrl.cycle_index + i);
                        lt_clk_skew_write_val_to_buf(ltcs_ctrl.min_gap_buf, min_gap, ltcs_ctrl.cycle_index + i);
                    }
                    break;
                case LTCS_TYPE_VENDOR:
                    for (i = 0 ; i < (LTCS_CYCLE_NO - LTCS_PREDIC_CYCLE_NO) ; i++) {
                        sample_count += (ltcs_receive.timestamp - ltcs_previous.timestamp) / (ltcs_ctrl.ts_ratio) * observe_thd; //
                        ltcs_ctrl.timestamp_anchor =  ltcs_receive.timestamp + (ltcs_receive.timestamp - ltcs_previous.timestamp) * observe_thd;
                        pcdc_asi_threshold = sample_count;
                        DSP_MW_LOG_I("[LTCS][sourcesize] sample_count: %d, asi:%d, ltcs_ctrl.timestamp_anchor:%d", 3, sample_count, asi, ltcs_ctrl.timestamp_anchor);
                        asi = (U32)(((U64)sample_count * 1000000) / ltcs_ctrl.sample_rate); // in us
                        lt_clk_skew_write_val_to_buf(ltcs_ctrl.asi_buf, asi, ltcs_ctrl.cycle_index + i);
                        lt_clk_skew_write_val_to_buf(ltcs_ctrl.min_gap_buf, min_gap, ltcs_ctrl.cycle_index + i);
                      //  printf("Kevin: TS_ratio is %d",ltcs_ctrl.ts_ratio);
                    }
                    break;
                case LTCS_TYPE_DSP_CNT:
                    for (i = 0 ; i < (LTCS_CYCLE_NO - LTCS_PREDIC_CYCLE_NO) ; i++) {
                        sample_count += (ltcs_receive.timestamp - ltcs_previous.timestamp) * observe_thd;
                        ltcs_ctrl.timestamp_anchor =  ltcs_receive.timestamp + (ltcs_receive.timestamp - ltcs_previous.timestamp) * observe_thd;
                        pcdc_asi_threshold = sample_count;
                        asi = (U32)(((U64)sample_count * 1000000) / ltcs_ctrl.sample_rate); // in us
                        lt_clk_skew_write_val_to_buf(ltcs_ctrl.asi_buf, asi, ltcs_ctrl.cycle_index + i);
                        lt_clk_skew_write_val_to_buf(ltcs_ctrl.min_gap_buf, min_gap, ltcs_ctrl.cycle_index + i);
                      //  printf("Kevin: TS_ratio is %d",ltcs_ctrl.ts_ratio);
                    }
                    break;
                case LTCS_TYPE_OTHERS:
                default:
                    break;
            }

            ltcs_ctrl.notify = TRUE; // pre-notify cm4
        }

        if (ltcs_ctrl.cycle_index == LTCS_CYCLE_NO) {


            #if 0
            // notify cm4
            ltcs_ctrl.notify = TRUE;
            #endif
            ltcs_ctrl.cycle_index = 0;
        }
    }

    ltcs_previous.btclk = ltcs_receive.btclk;
    ltcs_previous.timestamp = ltcs_receive.timestamp;

    hal_nvic_restore_interrupt_mask(mask);
}

static void lt_clk_skew_send_data_update(void)
{
    hal_ccni_message_t msg;
    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
    msg.ccni_message[0] = MSG_DSP2MCU_BT_AUDIO_DL_LTCS_DATA_REPORT << 16;
    aud_msg_tx_handler(msg, 0, FALSE);
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void lt_clk_skew_notify_cm4 (void)
{
    if (ltcs_ctrl.notify) {

        ltcs_ctrl.notify = FALSE;

        lt_clk_skew_send_data_update();
    }
}
void lt_clk_skew_accumulate_local_asi(S32 val)
{
    ltcs_ctrl.local_asi += val;
}

U32 lt_clk_skew_get_local_asi(VOID)
{
    return ltcs_ctrl.local_asi;
}

#ifdef MTK_PCDC_TIMEOUT_ENABLE
void lt_clk_skew_send_timeout_request(void)
{
    hal_ccni_message_t msg;
    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
    msg.ccni_message[0] = MSG_DSP2MCU_BT_AUDIO_DL_LTCS_TIMEOUT_REQUEST << 16;
    aud_msg_tx_handler(msg, 0, FALSE);
    DSP_MW_LOG_I("[LTCS] DSP Trigger send CCNI ID: 0x%x", 1, MSG_DSP2MCU_BT_AUDIO_DL_LTCS_TIMEOUT_REQUEST);
}

int lt_clk_skew_timeout_check(int asi_threshold, int cur_asi, int sample_rate)
{
    if(asi_threshold!=0x7fffffff){
        int asi_threshold_fixfs = (S32)((S64)asi_threshold*sample_rate/ltcs_ctrl.sample_rate);
        if((cur_asi > (asi_threshold_fixfs + 10*sample_rate))) {
             lt_clk_skew_send_timeout_request();
             DSP_MW_LOG_I("[LTCS] DSP Trigger Re-sync, asi_cnt:%d, asi_threshold:%d, fs:%d, fs:%d\r\n", 2, cur_asi, asi_threshold_fixfs, sample_rate, ltcs_ctrl.sample_rate);
             return (asi_threshold_fixfs + 10*sample_rate);
        }
        return 0;
    } else {
      return 0;
    }
}
#endif

