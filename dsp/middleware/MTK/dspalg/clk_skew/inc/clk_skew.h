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

#ifndef _CLK_SKEW_COMPENSATION_H_
#define _CLK_SKEW_COMPENSATION_H_

#include "dsp_feature_interface.h"
#include "dsp_utilities.h"
#include "common.h"
#include "skew_ctrl.h"
#include "hal_audio_afe_define.h"
#ifdef MTK_BT_HFP_SPE_ALG_V2
#include "stream_n9sco.h"
#endif

//#define DSP_CLK_SKEW_DEBUG_LOG
#define DSP_CLK_SKEW_MEMSIZE        (((sizeof(skew_ctrl_t)) + 3) / 4) * 4 // 4-byte alignment
#define DSP_RCDC_MAX_SAMPLE_DEVIATION  1
#define DSP_CLK_SKEW_TEMP_BUF_SIZE  (4096 + DSP_RCDC_MAX_SAMPLE_DEVIATION * 4)
#define BT_CLOCK_NUMBER_OF_BITS 28
#define BT_CLOCK_MAX_TICKS (((U32)1<<BT_CLOCK_NUMBER_OF_BITS)-1)
#define BTCLK_INVALID_CLK (0xFFFFFFFF)
#define BTINTRA_INVALID_CLK (0xFFFF)

typedef enum {
    CLK_SKEW_DL,
    CLK_SKEW_UL,
} CLK_SKEW_DIRECTION_TYPE_t;

typedef enum CLK_SKEW_DL_TYPE_e {
    CLK_SKEW_DL_A2DP = 0,
    CLK_SKEW_DL_ESCO = 1,
    CLK_SKEW_DL_BLE_MUSIC = 2,
} CLK_SKEW_DL_TYPE_t;

typedef enum CLK_SKEW_FS_enum {
    CLK_SKEW_FS_8K              = 8000,
    CLK_SKEW_FS_11_025K         = 11025,
    CLK_SKEW_FS_12K             = 12000,
    CLK_SKEW_FS_16K             = 16000,
    CLK_SKEW_FS_22_05K          = 22050,
    CLK_SKEW_FS_24K             = 24000,
    CLK_SKEW_FS_32K             = 32000,
    CLK_SKEW_FS_44_1K           = 44100,
    CLK_SKEW_FS_48K             = 48000,
    CLK_SKEW_FS_64K             = 64000,
    CLK_SKEW_FS_88_2K           = 88200,
    CLK_SKEW_FS_96K             = 96000,
    CLK_SKEW_FS_192K            = 192000,
} CLK_SKEW_FS_t;

typedef struct dsp_clock_skew_ctrl_s
{
    S16 AccumulatedSamples;
    S16 CompensatedSamples;
    S16 CompensatedPolarity;
    U16 IntrDownCnt;
    U16 BytesPerSample;
    U16 FrameSize;
    BOOL Initialized;
    S16 DbgAccBytes;
    U16 InterruptHandleCnt;
    U16 UL_Prev_Polarity;
    S16 UL_Pol_Change_Samples;
#ifdef ENABLE_HWSRC_CLKSKEW
    BOOL ClkSkewMode;
#endif
} DSP_CLOCK_SKEW_CTRL_t;

typedef struct rcdc_clk_info_t_s
{
    uint32_t  audio_clk;
    uint32_t  native_clk;
} RCDC_BT_CLK_INFO_t;

typedef struct rcdc_clk_offset_info_s
{
    U16 handle;
    U32 bt_clk_offset;
    U16 bt_intra_slot_offset;
} PACKED RCDC_CLK_OFFSET_INFO_t;

typedef struct dsp_clock_skew_rcdc_ctrl_s
{
    U32 n9_clk_prev;
    U32 n9_clk_next;
    U32 aud_clk_prev;
    U32 aud_clk_next;
    U32 bt_clk_offset_prev;
    U32 bt_clk_offset_next;
    U16 bt_intra_slot_offset_prev;
    U16 bt_intra_slot_offset_next;
    S32 prephase;
    BOOL first_offset_flag;
    BOOL initial_offset_flag;
#ifdef ENABLE_HWSRC_CLKSKEW
    U32 frame_cnt;
#endif
} DSP_CLOCK_SKEW_RCDC_CTRL_t;

typedef struct dsp_clock_skew_pcdc_ctrl_s
{
    int asi_cnt;
    int deviation_in_ppm;
    int debug_bytes;
} DSP_CLOCK_SKEW_PCDC_CTRL_t;

extern CLK_SKEW_FS_t clk_skew_fs_converter(stream_samplerate_t fs_in);
#ifdef MTK_BT_HFP_SPE_ALG_V2
VOID Clock_Skew_Offset_Update(BT_CLOCK_OFFSET_SCENARIO type);
#else
VOID Clock_Skew_Offset_Update(void);
#endif

#ifdef AIR_BT_CODEC_BLE_ENABLED
VOID Clock_Skew_Offset_Update_BLE(SOURCE source,BT_CLOCK_OFFSET_SCENARIO type);
#endif
#ifdef ENABLE_HWSRC_CLKSKEW
VOID Clock_Skew_UL_Offset_Update(BT_CLOCK_OFFSET_SCENARIO type);
#endif

VOID clk_skew_inform_ul_leads_samples (U32 samples);
VOID clk_skew_inform_ul_lags_samples (U32 samples);
S16 clk_skew_get_ul_comp_bytes (VOID);
S16 clk_skew_check_ul_status (VOID);
VOID clk_skew_finish_ul_compensation (VOID);
void clk_skew_get_ul_accumulated_bytes(void);
VOID clk_skew_inform_dl_leads_samples (U32 samples);
VOID clk_skew_inform_dl_lags_samples (U32 samples);
S16 clk_skew_get_dl_comp_bytes (VOID);
S16 clk_skew_check_dl_status (VOID);
VOID clk_skew_finish_dl_compensation (VOID);

EXTERN VOID Clock_Skew_UL_Para_Init();
EXTERN VOID Clock_Skew_DL_Para_Init();
EXTERN bool stream_function_clock_skew_uplink_initialize (void *para);
EXTERN bool stream_function_clock_skew_downlink_initialize (void *para);
EXTERN bool stream_function_clock_skew_uplink_process (void *para);
EXTERN bool stream_function_clock_skew_downlink_process (void *para);
EXTERN bool stream_function_clock_skew_hfp_downlink_process (void *para);
#ifdef AIR_BT_CODEC_BLE_ENABLED
EXTERN bool stream_function_clock_skew_ble_music_downlink_process (void *para);
#endif
EXTERN S16 clk_skew_get_ul_comp_samples (VOID);
#ifdef ENABLE_HWSRC_CLKSKEW
EXTERN void ClkSkewMode_Selection(hal_ccni_message_t msg, hal_ccni_message_t *ack);
void clk_skew_get_dl_accumulated_bytes_from_ul(U32 fs_in, U32 fs_out);
EXTERN void pcdc_asi_threshold_counter(U32 sample_size, int fs);
EXTERN void pcdc_asi_count_add(U32 add_amount);
#endif

extern int pcdc_asi_threshold;
extern unsigned int ClkSkewLagsNo;
extern unsigned int ClkSkewLeadsNo;
extern RCDC_CLK_OFFSET_INFO_t * rcdc_clk_offset_info_ptr;
extern RCDC_BT_CLK_INFO_t * rcdc_clk_info_ptr;
extern bool fgULRCDCPolling;
extern bool fgUL1stRCDC;
extern DSP_CLOCK_SKEW_CTRL_t ClkSkewDLCtrl;
extern DSP_CLOCK_SKEW_CTRL_t ClkSkewULCtrl;
extern DSP_CLOCK_SKEW_RCDC_CTRL_t ClkSkewRCDCCtrl;
extern DSP_CLOCK_SKEW_PCDC_CTRL_t ClkSkewPCDCCtrl;
#ifdef ENABLE_HWSRC_CLKSKEW
extern bool ClkSkewMode_g;
extern bool ClkSkewMode_isModify_g;
extern DSP_CLOCK_SKEW_RCDC_CTRL_t ClkSkewULRCDCCtrl;
#endif


#endif /* _CLK_SKEW_COMPENSATION_H_ */
