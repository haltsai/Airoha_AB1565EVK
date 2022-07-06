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

#ifndef _PEQ_H_
#define _PEQ_H_

#include "dsp_feature_interface.h"
#include "dsp_utilities.h"
#include "common.h"

#define DSP_PEQ_MEMSIZE             ((DSP_PEQ_INSTANCE_MEMSIZE * 2) + DSP_PEQ_OVERLAP_BUFSIZE)
#define DSP_PEQ_INSTANCE_MEMSIZE    ((sizeof(PEQ_ST) + 6) & (~0x3))
#define DSP_PEQ_OVERLAP_BUF_SAMPLES (2048)
#define DSP_PEQ_OVERLAP_BUFSIZE     (DSP_PEQ_OVERLAP_BUF_SAMPLES << 2)
#define PEQ_SWITCH_STEP_PERCENTAGE  (1)
#define PEQ_SWITCH_TOTAL_PERCENTAGE (100)
#ifdef MTK_GAMING_MODE_HEADSET
#define PEQ_OVERLAP_FRAME_SIZE      (8)
#else
#define PEQ_OVERLAP_FRAME_SIZE      (8)
#endif
#define FW_MAX_ELEMENT      (4)
#define FW_MAX_BANDS        (10)
#define MAX_BANDS           (15)

#ifdef MTK_DEQ_ENABLE
#define DEQ_MAX_DELAY_SAMPLES       (4)
#define DSP_DEQ_MEMSIZE             ((DSP_PEQ_INSTANCE_MEMSIZE * 2) + DSP_PEQ_OVERLAP_BUFSIZE + (DEQ_MAX_DELAY_SAMPLES << 2))
#endif

#define DSP_PEQ_SYNC_WITH_BT_CLOCK_PTS

#define DSP_BT_LOG_D(_message, arg_cnt, ...)   LOG_W(MPLOG,_message, ##__VA_ARGS__)

typedef struct
{
    S16 RUN_N;
    S32 bq1X[MAX_BANDS];
    S32 bq2X[MAX_BANDS];
    S32 bq3X[MAX_BANDS];
    S32 aq2X[MAX_BANDS];
    S32 aq3X[MAX_BANDS];

    S32 output_gain ;

    //L
    S32 Xstate1_l[MAX_BANDS];
    S32 Xstate2_l[MAX_BANDS] ;
    S32 Ystate1_l[MAX_BANDS] ;
    S32 Ystate2_l[MAX_BANDS] ;

    //R
    S32 Xstate1_r[MAX_BANDS] ;
    S32 Xstate2_r[MAX_BANDS] ;
    S32 Ystate1_r[MAX_BANDS] ;
    S32 Ystate2_r[MAX_BANDS] ;
}PEQ_ST;

typedef struct dsp_peq_param_s
{
    U16 elementID;
    U16 numOfParameter;
    S16 peq_inter_param[5*FW_MAX_BANDS*2+2+1];
} PACKED DSP_PEQ_PARAM_t;

typedef struct dsp_peq_nvkey_s
{
    U16 numOfElement;
    U16 peqAlgorithmVer;
    DSP_PEQ_PARAM_t peq_element_param[FW_MAX_ELEMENT];
} PACKED DSP_PEQ_NVKEY_t;

typedef struct dsp_peq_sub_ctrl_s
{
    PEQ_ST *peq_instance;
    U8 status;
    S16 overlap_progress;
} DSP_PEQ_SUB_CTRL_t;

typedef struct dsp_peq_ctrl_s
{
    DSP_PEQ_NVKEY_t peq_nvkey_param;
    S16             *p_peq_inter_param;
    U16             peq_nvkey_id;
    U8              need_update;
    U8              sample_rate;
    BOOL            enable;
    U8              phase_id;
    U8              proc_ch_mask;

    U8              trigger_drc;
    U32             target_timestamp;

    DSP_PEQ_SUB_CTRL_t sub_ctrl[2];
    S32             *p_overlap_buffer;
    S32             peq_mode;   //0: with DRC, 1: without DRC
    U8              audio_path;
} DSP_PEQ_CTRL_t;

typedef struct dsp_peq_sync_ctrl_s
{
    U8              started;
    U8              use_seqno;
    U32             prepare_bt_clk;
    U32             anchor_bt_clk;
    U32             anchor_timestamp;
    S32             asi_cnt;
    U32             current_timestamp;
} DSP_PEQ_SYNC_CTRL_t;

typedef struct
{
    U16 elementID;
    U16 numOfParameter;
    U16 parameter[1];
} PEQ_ELEMENT_STRU;

typedef struct
{
    U16 numOfElement;
    U16 peqAlgorithmVer;
    PEQ_ELEMENT_STRU peqElement[1];
} PEQ_PARAMETER_STRU;

enum  //elementID
{
    PEQ_32K = 0x00, //sync with tool, but no use
    PEQ_44_1K,
    PEQ_48K,
    PEQ_16K,    //sync with tool, but no use
    PEQ_8K,     //sync with tool, but no use
    PEQ_88_2K,
    PEQ_96K,
    PEQ_24K,
};

typedef enum
{
    PEQ_STATUS_OFF = 0,
    PEQ_STATUS_ON,
    PEQ_STATUS_FADE_OUT,
    PEQ_STATUS_FADE_IN,
} PEQ_STATUS_t;

EXTERN bool stream_function_peq_initialize (void *para);
EXTERN bool stream_function_instant_peq_process (void *para);
EXTERN bool stream_function_peq_process (void *para);
EXTERN bool stream_function_peq2_initialize (void *para);
EXTERN bool stream_function_peq2_process (void *para);
EXTERN bool stream_function_peq3_initialize (void *para);
EXTERN bool stream_function_peq3_process (void *para);
EXTERN bool stream_function_peq4_initialize (void *para);
EXTERN bool stream_function_peq4_process (void *para);
#ifdef MTK_DEQ_ENABLE
EXTERN bool stream_function_deq_initialize (void *para);
EXTERN bool stream_function_deq_process (void *para);
EXTERN bool stream_function_deq_mute_initialize (void *para);
EXTERN bool stream_function_deq_mute_process (void *para);
EXTERN void dsp_deq_set_param (hal_ccni_message_t msg, hal_ccni_message_t *ack);
#endif
EXTERN U8 peq_get_trigger_drc(U8 phase_id,U8 type);
EXTERN void PEQ_Reset_Info(void);
EXTERN void PEQ_Set_Param(hal_ccni_message_t msg, hal_ccni_message_t *ack, BOOL BypassTimestamp);
EXTERN void PEQ_Update_Info(U32 anchor_bt_clk, U32 anchor_timestamp);


#endif /* _PEQ_H_ */
