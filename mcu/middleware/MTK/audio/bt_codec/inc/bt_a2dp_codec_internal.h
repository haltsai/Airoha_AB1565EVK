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
 
#ifndef __BT_A2DP_CODEC_INTERNAL_H__
#define __BT_A2DP_CODEC_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include <string.h>
#include "bt_codec.h"
#include "hal_audio.h"

#ifndef HAL_AUDIO_MODULE_ENABLED
#error "please turn on audio feature option on hal_feature_config.h"
#endif

#if defined(MTK_AVM_DIRECT)
    #include "hal_audio_cm4_dsp_message.h"
    #include "hal_audio_internal.h"
#else
    #include "hal_audio_internal_service.h"
    #include "hal_audio_internal_afe.h"
    #include "hal_audio_fw_sherif.h"
    #include "hal_audio_fw_interface.h"

    #include "bt_hfp_codec_internal.h"
#endif


#ifdef MTK_BT_A2DP_SOURCE_SUPPORT
#include "sbc_codec.h"
#endif /*MTK_BT_A2DP_SOURCE_SUPPORT*/

#include "hal_gpt.h"


log_create_module(BT_A2DP_CODEC, PRINT_LEVEL_INFO);
#if 0
#define TASK_LOG_E(fmt,arg...)      LOG_E(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg)
#define TASK_LOG_W(fmt,arg...)      /* LOG_W(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg) */
#define TASK_LOG_I(fmt,arg...)      LOG_I(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg)
#define TASK_LOG_CTRL(fmt,arg...)   LOG_I(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg)
#define LISR_LOG_E(fmt,arg...)      LOG_E(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg)
#define LISR_LOG_W(fmt,arg...)      /* LOG_W(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg) */
#define LISR_LOG_I(fmt,arg...)      LOG_I(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg)
#define LISR_LOG_CTRL(fmt,arg...)   /* LOG_I(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg) */
#else
#define TASK_LOG_MSGID_E(fmt,arg...)      LOG_MSGID_E(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg)
#define TASK_LOG_MSGID_W(fmt,arg...)      /* LOG_MSGID_W(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg) */
#define TASK_LOG_MSGID_I(fmt,arg...)      LOG_MSGID_I(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg)
#define TASK_LOG_MSGID_CTRL(fmt,arg...)   LOG_MSGID_I(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg)
#define LISR_LOG_MSGID_E(fmt,arg...)      LOG_MSGID_E(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg)
#define LISR_LOG_MSGID_W(fmt,arg...)      /* LOG_MSGID_W(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg) */
#define LISR_LOG_MSGID_I(fmt,arg...)      LOG_MSGID_I(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg)
#define LISR_LOG_MSGID_CTRL(fmt,arg...)   /* LOG_MSGID_I(BT_A2DP_CODEC, "[A2DP CODEC]: "fmt,##arg) */
#endif

#ifdef MTK_BT_A2DP_AAC_ENABLE
#define     AAC_ERROR_FRAME_THRESHOLD   0xFFFFFFFF
#define     AAC_FILL_SILENCE_TRHESHOLD  0x800
#endif /*MTK_BT_A2DP_AAC_ENABLE*/

#define AWS_CLOCK_SKEW_MAX_SAMPLE_COUNT (48)            /* 1 ms for 48kHz*/
#define AWS_CLOCK_SKEW_RING_BUFFER_SAMPLE_COUNT (1440)  /* 30 ms for 48kHz */
#define AWS_CLOCK_SKEW_PREBUFFER_MILLISECOND_COUNT (5)  /* Pre-buffer 5 ms for clock skew adjustment */

#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
#define AWS_SRC_PHASE_BITS  11
#define AWS_SRC_TOTAL_VALUE (1 << AWS_SRC_PHASE_BITS)
#define AWS_SRC_PHASE_MASK  (AWS_SRC_TOTAL_VALUE - 1)
#define AWS_SRC_INTERPOLATION(val_lo, val_hi, phase) (((val_hi) * (phase) + (val_lo) * (AWS_SRC_TOTAL_VALUE - (phase))) >> AWS_SRC_PHASE_BITS)
#define AWS_ADD_POINT_BUF_SIZE 48 /*by sample*/

typedef struct {
    int16_t previous_sample_L;
    int16_t previous_sample_R;
} bt_codec_aws_src_handle_t;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */

typedef struct {
    bt_media_handle_t     handle;
    bt_codec_a2dp_audio_t codec_info;
    uint32_t              sample_rate;
    ring_buffer_information_t ring_info;
    uint16_t              channel_number;
    uint32_t              frame_count;

#if defined(MTK_BT_A2DP_AAC_ENABLE) && !defined(MTK_AVM_DIRECT)
    uint32_t                    error_count;
    uint8_t                     aac_silence_pattern[SILENCE_TOTAL_LENGTH];
#endif /*MTK_BT_A2DP_AAC_ENABLE*/
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    bool                  aws_flag;
    bool                  aws_init_sync_flag;
    bool                  aws_internal_flag;
    uint8_t              *aws_working_buffer;
    int32_t               aws_working_buf_size;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
#ifdef MTK_BT_A2DP_SOURCE_SUPPORT
    sbc_codec_media_handle_t        *sbc_encode_handle;
    sbc_codec_initial_parameter_t   initial_parameter;
#endif /*MTK_BT_A2DP_SOURCE_SUPPORT*/

#if defined(MTK_AVM_DIRECT)
    bool                  is_cp;
    bool                  is_aws;
	bool                  is_alc_en;
    bool                  is_lm_en; //latency monitor
    uint32_t              time_stamp;
    uint32_t              ts_ratio;
    uint32_t              start_asi;
    uint32_t              start_bt_clk;
    uint32_t              start_bt_intra_clk;
#endif

} bt_a2dp_audio_internal_handle_t;


#define ADDR(FIELD)     ((uint16_t volatile *) (FIELD##_ADDR  ))
#define MASK(FIELD)     (FIELD##_MASK)
#define SHIFT(FIELD)    (FIELD##_SHIFT)

#define SET_DSP_VALUE(FIELD, VALUE) \
    { uint16_t volatile *addr = ADDR(FIELD); \
    *addr = ((((uint16_t)(VALUE) << SHIFT(FIELD)) \
    & MASK(FIELD)) | (*addr & ~MASK(FIELD))); }

#define GET_DSP_VALUE(FIELD) (( *ADDR(FIELD)&(MASK(FIELD))) >> SHIFT(FIELD))

#define RG_SBC_PARSER_EN_ADDR        DSP_SBC_DEC_CTRL
#define RG_SBC_PARSER_EN_MASK        0x0001
#define RG_SBC_PARSER_EN_SHIFT       0

#define RG_SBC_DEC_FSM_ADDR          DSP_SBC_DEC_CTRL
#define RG_SBC_DEC_FSM_MASK          0xF000
#define RG_SBC_DEC_FSM_SHIFT         12

#define RG_SBC_DEC_STATUS_ADDR       DSP_SBC_DEC_STATUS
#define RG_SBC_DEC_STATUS_MASK       0xF000
#define RG_SBC_DEC_STATUS_SHIFT      12

#define RG_SBC_PAR_STATUS_ADDR       DSP_SBC_DEC_STATUS
#define RG_SBC_PAR_STATUS_MASK       0x0F00
#define RG_SBC_PAR_STATUS_SHIFT      8

#define RG_SBC_BS_R_MIRROR_ADDR      DSP_SBC_DEC_DM_BS_DSP_R_PTR
#define RG_SBC_BS_R_MIRROR_SHIFT     12
#define RG_SBC_BS_R_MIRROR_MASK      (0x1 << RG_SBC_BS_R_MIRROR_SHIFT)
#define RG_SBC_BS_R_MIRROR_CLEAR     (~RG_SBC_BS_R_MIRROR_MASK)

#define RG_SBC_BS_W_MIRROR_ADDR      DSP_SBC_DEC_DM_BS_MCU_W_PTR
#define RG_SBC_BS_W_MIRROR_SHIFT     12
#define RG_SBC_BS_W_MIRROR_MASK      (0x1 << RG_SBC_BS_W_MIRROR_SHIFT)
#define RG_SBC_BS_W_MIRROR_CLEAR     (~RG_SBC_BS_W_MIRROR_MASK)

/** @brief sbc decoder MCU-DSP state define*/
#define DSP_SBC_STATE_IDLE         0x0
#define DSP_SBC_STATE_START        0x1
#define DSP_SBC_STATE_RUNNING      0xF
#define DSP_SBC_STATE_FLUSH        0xE
#define DSP_SBC_STATE_STOP_1       0xC
#define DSP_SBC_STATE_STOP_2       0x8

/** @brief sbc decoder status define*/
#define DSP_SBC_DEC_NORMAL                 0x0
#define DSP_SBC_DEC_SYNC_ERR               0xF
#define DSP_SBC_DEC_CRC_ERR                0xE
#define DSP_SBC_DEC_BITPOOL_ERR            0xD
#define DSP_SBC_DEC_BS_UNDERFLOW           0x1
#define DSP_SBC_DEC_SILENCE_OUT_OF_BOUND   0x2

/** @brief sbc parser status define*/
#define DSP_SBC_PAR_NORMAL                              0x0
#define DSP_SBC_PAR_MAGIC_WORD_ERR                      0xE
#define DSP_SBC_PAR_INVALID_NON_FRAGMENTED_PAYLOAD      0xD
#define DSP_SBC_PAR_START_PAYLOAD_ERR                   0xC
#define DSP_SBC_PAR_INVALID_FRAGMENT_INDEX_1            0xB
#define DSP_SBC_PAR_INVALID_FRAGMENT_INDEX_2            0xA
#define DSP_SBC_PAR_INVALID_FRAME_NUMBER                0x9


#ifdef MTK_BT_A2DP_AAC_ENABLE
/** @brief aac decoder register define*/
#define DSP_AAC_DEC_FSM             DSP_AAC_MAIN_CONTROL

/** @brief aac decoder MCU-DSP state define*/
#define DSP_AAC_STATE_IDLE          0x0
#define DSP_AAC_STATE_START         0x6
#define DSP_AAC_STATE_PLAYING       0x2
#define DSP_AAC_STATE_STOP          0xA   //10: fast stop
#define DSP_AAC_STATE_END           0xE   //14: normal stop

/** @brief aac decoder dsp parameter define*/
#define DSP_AAC_PAGE_NUM            0x5

/** @brief aac decoder error report define*/
#define DSP_AAC_REPORT_NONE                 0x0
#define DSP_AAC_REPORT_BUFFER_NOT_ENOUGH    0x1111
#define DSP_AAC_REPORT_UNDERFLOW            0x2222

#endif //MTK_BT_A2DP_AAC_SUPPORT


//--------------------------------------------
// Open Parameters for AVM
//--------------------------------------------

#ifdef __cplusplus
}
#endif

#endif  /*__BT_A2DP_CODEC_INTERNAL_H__*/
