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
 
#ifndef _STREAM_N9_A2DP_H_
#define _STREAM_N9_A2DP_H_

/*!
 *@file   stream_n9_a2dp.h
 *@brief  defines the setting of n9 a2dp stream
 *
 @verbatim
 @endverbatim
 */


#include "config.h"
#include "types.h"
#include "source_inter.h"
#include "sink_inter.h"

#include "source.h"
#include "sink.h"
#include "stream.h"
#include "transform_inter.h"
#include "dsp_callback.h"

#include "stream_audio_hardware.h"
#include "stream_audio_setting.h"
#include "stream_audio_driver.h"

#include "hal_hw_semaphore.h"
#include "hal_resource_assignment.h"
#include "dsp_audio_msg_define.h"

#include "syslog.h"

#ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
#include "vendor_decoder_proting.h"
#endif

#include "long_term_clk_skew.h"


#define STRM_A2DP_USE_MSGID_SEND_LOG
#ifdef STRM_A2DP_USE_MSGID_SEND_LOG
#define STRM_A2DP_LOG_E(_message, arg_cnt, ...)  LOG_MSGID_E(strm_a2dp,_message, arg_cnt, ##__VA_ARGS__)
#define STRM_A2DP_LOG_W(_message, arg_cnt, ...)  LOG_MSGID_W(strm_a2dp,_message, arg_cnt, ##__VA_ARGS__)
#define STRM_A2DP_LOG_I(_message, arg_cnt, ...)  LOG_MSGID_I(strm_a2dp,_message, arg_cnt, ##__VA_ARGS__)
#define STRM_A2DP_LOG_D(_message, arg_cnt, ...)  LOG_MSGID_D(strm_a2dp,_message, arg_cnt, ##__VA_ARGS__)
#else
#define STRM_A2DP_LOG_E(_message, arg_cnt, ...)  LOG_E(strm_a2dp,_message, ##__VA_ARGS__)
#define STRM_A2DP_LOG_W(_message, arg_cnt, ...)  LOG_W(strm_a2dp,_message, ##__VA_ARGS__)
#define STRM_A2DP_LOG_I(_message, arg_cnt, ...)  LOG_I(strm_a2dp,_message, ##__VA_ARGS__)
#define STRM_A2DP_LOG_D(_message, arg_cnt, ...)  LOG_D(strm_a2dp,_message, ##__VA_ARGS__)
#endif

// #define N9_A2DP_UT

// #define HW_SEMAPHORE_DSP_CIRCULAR_BUFFER  HW_DSP_CIRCULAR_BUFFER

enum
{
    SBC_MONO_CHANNEL,
    SBC_DUAL_CHANNEL,
    SBC_STEREO_CHANNEL,
    SBC_JOINT_STEREO_CHANNEL
};

enum
{
    PCB_STATE_FREE,
    PCB_STATE_USED,
    PCB_STATE_LOST,
    PCB_STATE_FEC,
    PCB_STATE_SKIP,
    PCB_STATE_DECODED
};

enum
{
    RETURN_PASS,
    RETURN_FAIL,
};

enum
{
    PL_STATE_NONE,
    PL_STATE_REPORTPREVSEQN,
    PL_STATE_REPORTNEXTSEQN,
};

#define LATENCY_MONITOR_THD 140 // UNIT : ms
#define ADATIVE_LATENCY_CTRL (1) // 0 : disable 1 : enable
#ifdef MTK_BT_A2DP_VENDOR_1_ENABLE
#define AUDIO_ASI_CNT (1) // 0 : disable 1 : enable  for heaset usage
#else
#define AUDIO_ASI_CNT (0) // 0 : disable 1 : enable  for heaset usage	
#endif
#if  ADATIVE_LATENCY_CTRL
#define ADATIVE_LATENCY (400000) // UNIT : us , Modified latency regardess of Idle buffer size
#endif
#define REINIT_FLAG (0x7FFF)


////////////////////////////////////////////////////////////////////////////////
// Type Defintions /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 *  @brief This structure defines PCB from N9.
 */
typedef struct
{
    U8 state;      //0:free, 1:used, 2:lost, 3:FEC, 4:skip, 5:decoded
    U8  data_size;          //parameters size for FEC in payload tail
    U16 size;               //size in byte, including rtpheader and payload
} PACKED bt_codec_a2dp_pcb_type_t;
#define VEND_FRAME_BUF_SIZE (340)

typedef struct
{
    U32 frame_asi;      //Frame ASI.
    U16 frame_size;     //Frame size in byte.
    U16 frame_pkt_seqn; //frame origin belonging packet sequence number.
    U8  pcb_state;      //0:free, 1:used, 2:lost, 3:FEC, 4:skip, 5:decoded
    U8  reserved[3];    //Reserved
} PACKED bt_codec_a2dp_hdr_type_t,*bt_codec_a2dp_hdr_type_ptr;


/**
 *  @brief This structure defines RTP header from N9.
 */
typedef struct
{
    U8  version;   //version, 0x80 in general
    U8  type;      //type of media payload
    U16 seqno;     //sequence number
    U32 timestamp; //timestamp in audio sample
    U32 ssrc;      //ssrc, 0 in general
} PACKED bt_codec_a2dp_rtpheader_t;

/**
 *  @brief This structure defines CP from N9.
 */
typedef struct
{
    U8  content_protect; //content protect
} PACKED bt_codec_a2dp_cp_type_t;

/**
 *  @brief This structure defines payload header.
 */
typedef struct
{
    U8 numOfFrames:4; //if fragmented = 0, number of frames contained in this packet
                      //if fragmented = 1, number of remaining fragments (including this)
    U8 reserved   :1; //reserved
    U8 starting   :1; //1:starting packet of a fragmented SBC frame, 0:otherwise
    U8 last       :1; //1:last packet of a fragmented SBC frame, 0:otherwise
    U8 fragmented :1; //1:SBC frame is fragmented, 0:otherwise
} PACKED bt_codec_a2dp_sbc_pl_header_t;

/**
 *  @brief This structure defines sbc type2.
 */
typedef union
{
    U8 sbc_byte2;
    struct
    {
        U8 SUBBANDS:1;
        U8 ALLOCATION_METHOD:1;
        U8 CHANNEL_MODE:2;
        U8 BLOCKS:2;
        U8 SAMPLING_FREQ:2;
    } PACKED bit_alloc;
} PACKED bt_codec_a2dp_sbc_byte2_t;

/**
 *  @brief This structure defines frame header.
 */
typedef struct
{
    U8  SyncWord; //0x9C
    bt_codec_a2dp_sbc_byte2_t Byte1;
    U8  Bitpool;
    U8  RSVD[10];
} PACKED bt_codec_a2dp_sbc_frame_header_t;


typedef struct
{
    U8 numOfFrames:4; //if fragmented = 0, number of frames contained in this packet
                      //if fragmented = 1, number of remaining fragments (including this)
    U8 RFA        :1; //reserved
    U8 L_Bit      :1; //1:last packet of a fragmented SBC frame, 0:otherwise
    U8 S_Bit      :1; //1:starting packet of a fragmented SBC frame, 0:otherwise
    U8 F_Bit      :1; //1:vendor frame is fragmented, 0:otherwise
} PACKED bt_codec_a2dp_vend_pl_header_t;


typedef struct
{
    U8 latency    :2; //latency
    U8 numOfFrames:4; //if fragmented = 0, number of frames contained in this packet
    U8 RFA        :2; //reserved
} PACKED bt_codec_a2dp_vend_2_pl_header_t;


typedef struct
{
    U8 vend_byte1;
    U8 vend_byte2;
    U8 vend_byte3;
} PACKED bt_codec_a2dp_vend_frame_header_t;

typedef union
{
    U32 vend_header;
    struct
    {
        U8 vend_byte1;
        U8 vend_byte2;
        U8 vend_byte3;
        U8 vend_byte4;
    } PACKED byte_alloc;
} PACKED bt_codec_a2dp_vend_2_frame_header_t;


// /**
 // *  @brief This structure defines payload.
 // */
// typedef struct
// {
    // bt_codec_a2dp_sbc_pl_header_t    payloadInfo;
    // bt_codec_a2dp_sbc_frame_header_t frameHeader; //In each frame
    // U8 framedata[1];
// } bt_codec_a2dp_sbc_frame_t;

// /**
 // *  @brief This structure defines media packet with cp.
 // */
// typedef struct
// {
    // bt_codec_a2dp_pcb_type_t    pcb;
    // bt_codec_a2dp_rtpheader_t   rtpheader;
    // bt_codec_a2dp_cp_type_t     cp;
    // bt_codec_a2dp_sbc_frame_t   sbcframe;
// } bt_codec_a2dp_sbc_media_pkt_w_cp_t;

// /**
 // *  @brief This structure defines media packet without cp.
 // */
// typedef struct
// {
    // bt_codec_a2dp_pcb_type_t    pcb;
    // bt_codec_a2dp_rtpheader_t   rtpheader;
    // bt_codec_a2dp_sbc_frame_t   sbcframe;
// } bt_codec_a2dp_sbc_media_pkt_wo_cp_t;
uint32_t a2dp_get_samplingrate(SOURCE source);
uint32_t a2dp_get_channel(SOURCE source);
EXTERN void Au_DL_send_reinit_request(DSP_REINIT_CAUSE reinit_msg);
EXTERN VOID SourceInit_N9_a2dp(SOURCE source);
#ifdef MTK_GAMING_MODE_HEADSET
#include "bt_types.h"
EXTERN VOID a2dp_ull_get_proper_play_en(BTCLK CurrCLK, U8 CurrSeqNo, BTCLK* ProperCLK, U8* ProperSeqNo);
EXTERN VOID a2dp_ull_set_init_seq_no(U8 SeqNo);
EXTERN VOID a2dp_ull_manually_update_seq_no(U8 SeqNo);
EXTERN bool a2dp_ull_drop_expired_pkt(SOURCE source);
#endif

#ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
typedef struct _vend_bc_extern_buf_t{
    SOURCE a2dp_source;
    U8 bc_temp_buf[340];
    U8 bc_temp_read_buf[340];
    BOOL read_drop_flag;
    vend_bc_param_t param;
    vend_bc_handle_t* p_handle;
} vend_bc_extern_buf_t;

typedef struct vend_bc_extern_buf_v2_s{
    SOURCE a2dp_source;
    vend_bc_param_t param;
    vend_bc_handle_t* p_handle;
} vend_bc_extern_buf_v2_t;

#endif

typedef struct _local_asi_compare_info_t{
    U16 localSEQN;
    U16 relaySEQN;
    U32 localASI;
    U32 relayASI;
    S32 comp_val;
}local_asi_compare_info_t;

#endif /* _STREAM_N9_A2DP_H_ */

