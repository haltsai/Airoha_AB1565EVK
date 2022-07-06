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

#ifndef _LONG_TERM_CLOCK_SKEW_H_
#define _LONG_TERM_CLOCK_SKEW_H_

#include "types.h"

typedef enum {
    LTCS_TYPE_SBC = 0,
    LTCS_TYPE_AAC,
    LTCS_TYPE_IOS_AAC,
    LTCS_TYPE_ANDROID_AAC,
    LTCS_TYPE_IOS_SBC,
    LTCS_TYPE_VENDOR,
    LTCS_TYPE_DSP_CNT,
    LTCS_TYPE_OTHERS,
} ltcs_bt_type_t;

typedef struct {
	int btclk;
	int seqno;
	int timestamp;
	ltcs_bt_type_t a2dp_audio_type;
} ltcs_header_type_t, *ltcs_header_type_ptr_t;

typedef struct {
	int packet_index;
	int cycle_index;
	int gap_min;
	int drift_val; // in ppm
	int sample_rate; // hz
	U32* min_gap_buf;
	U32* asi_buf;
	bool init;
	bool notify;
	int ts_ratio;
	U32 sink_latency;
	uint32_t prev_base_asi;
	uint32_t base_asi;
    uint32_t local_asi;
	int actual_drift_value;
	int asi_threshold;
	int timestamp_anchor;
	int seqno_anchor;
	int flag_2048;
} ltcs_ctrl_type_t;

typedef struct {
    int32_t drift_comp_val; // long term drift compensation value
    uint32_t anchor_clk;    // long term drift anchor clk
    uint32_t asi_base;      // 1st time is play_en asi, later is anchor asi
    uint32_t asi_cur;   // asi base for current play
} ltcs_anchor_info_t;

extern void lt_clk_skew_reset_info (void);
extern void lt_clk_skew_set_sample_rate (U32 sample_rate);
extern void lt_clk_skew_set_min_gap_buf (U32* buf);
extern void lt_clk_skew_set_asi_buf (U32* buf);
extern void lt_clk_skew_update_actual_dsp_drift (void);
extern int  lt_clk_skew_get_dsp_drift_value (void);
extern void lt_clk_skew_set_drift_value (S32 drift_val);
extern void lt_clk_skew_set_ts_ratio (S32 ratio);
extern void lt_clk_skew_set_sink_latency (U32 sink_latency);
extern void lt_clk_skew_update_base_asi (uint32_t base_asi, uint32_t curr_asi);
extern void lt_clk_skew_set_asi_threshold (int threshold);
extern int  lt_clk_skew_get_asi_threshold (int sample_rate);
extern void lt_clk_skew_gather_pkt_info (int btclk, int timestamp, int seqno, ltcs_bt_type_t a2dp_audio_type);
extern void lt_clk_skew_gather_regression_info (void);
extern void lt_clk_skew_notify_cm4 (void);
extern void lt_clk_skew_update_anchor_clock (int btclk);
extern void lt_clk_skew_accumulate_local_asi(S32 val);
#ifdef MTK_PCDC_TIMEOUT_ENABLE
extern U32 lt_clk_skew_get_local_asi(VOID);
extern int lt_clk_skew_timeout_check(int asi_threshold, int cur_asi, int sample_rate);
#endif


#endif /* _LONG_TERM_CLOCK_SKEW_H_ */
