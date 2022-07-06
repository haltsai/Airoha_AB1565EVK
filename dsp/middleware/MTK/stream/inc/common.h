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

#ifndef __COMMON_H__
#define __COMMON_H__

/*!
 *@file   common.h
 *@brief  defines common definition  of streaming
 *
 @verbatim
 @endverbatim
 */

#include "types.h"
#include "dsp_task.h"
#include "transform_.h"
#include "source_.h"
#include "dsp_drv_dfe.h"
#include "hal_audio_afe_define.h"
#include "hal_audio_afe_control.h"
#include "audio_afe_common.h"
#include "dsp_temp.h"

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define BUFFER_INFO_CH_NUM      4

////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef enum {
    BUFFER_TYPE_CIRCULAR_BUFFER     =  (U8)(0UL),
    BUFFER_TYPE_QUEUE               =  (U8)(1UL),
    BUFFER_TYPE_DIRECT_BUFFER       =  (U8)(2UL),
    BUFFER_TYPE_INTERLEAVED_BUFFER  =  (U8)(3UL),
    BUFFER_TYPE_OTHER               =  (U8)(4UL),
} BUFFER_TYPE;


typedef struct {
    U8*    startaddr[BUFFER_INFO_CH_NUM];
    U32    ReadOffset;
    U32    WriteOffset;
    U16    channelSelected;
    U32    length;
    BOOL   bBufferIsFull;
} BUFFER_INFO,*BUFFER_INFO_PTR;

typedef union {
    U32 next;
    struct{
    U16 blk_size;
    U16 blk_num;
    }block_info;
} SUB_INFO;

typedef struct {
    U32 startaddr;     // start address of N9-DSP share buffer
    U32 ReadOffset;    // read pointer of N9-DSP share buffer
    U32 WriteOffset;   // write pointer of N9-DSP share buffer
    SUB_INFO sub_info;          // next read position in buf for DSP
    U32 sample_rate;   // sample rate for clock skew counting
    U16 length;        // total length of N9-DSP share buffer
    U8  bBufferIsFull; // buffer full flag, when N9 find there is no free buffer for putting a
                       // packet, set this flag = 1, DSP will reset this flag when data be taken by DSP
    U8  notify_count;  // notify count
    S32 drift_comp_val;// long term drift compensation value
    U32 anchor_clk;    // long term drift anchor clk
    U32 asi_base;      // asi base for anchor
    U32 asi_current;   // asi base for current play
} SHARE_BUFFER_INFO,*SHARE_BUFFER_INFO_PTR;

#define BT_AWS_MCE_ROLE_NONE                0x00          /**< No Role. */
#define BT_AWS_MCE_ROLE_CLINET              0x10          /**< Client Role. */
#define BT_AWS_MCE_ROLE_PARTNER             0x20          /**< Partner Role. */
#define BT_AWS_MCE_ROLE_AGENT               0x40          /**< Agent Role. */

#define INFO_STATUS_USED                    0x01
#define INFO_STATUS_FREE                    0x10

typedef struct {
    U8  aws_role;
    U8  info_status;
    U16 cur_seqn;
    U32 cur_asi;
} AUDIO_SYNC_INFO;

typedef struct {
U32 StartAddr;       // start address of share buffer
U16 ReadIndex;  // read pointer of share buffer  : DSP monitor
U16 WriteIndex; // write pointer of share buffer : Controller monitor
U32 SampleRate; // sample rate for clock skew counting
U16 MemBlkSize; // block length for each frame
U16 MemBlkNum;  // number of block for frame usage
U32 DbgInfoAddr; // start address of controller packet address table
U16 FrameSampleNum;  // DSP notify audio
U16 codec_type;      // Codec information
U16 codec_config;    // Codec config information
U16 NotifyCount;  // notify count of DSP notify controller not to sleep
U32 ForwarderAddr; // forwarder buffer address, also used to report ASI compromise in A2DP
U32 SinkLatency; // a2dp sink latency
U32 reserved[1]; // reserved
} AVM_SHARE_BUF_INFO,*AVM_SHARE_BUF_INFO_PTR;


typedef struct {
    U8*  addr;
    U32  length;
} MEMBLK;

typedef enum {
    BUFFER_INFO_TYPE = 0,
    SHARE_BUFFER_INFO_TYPE,
    AVM_SHARE_BUF_INFO_TYPE,
    STREAM_BUFFER_TYPE_MAX = 0xffffffff
} STREAM_BUFFER_TYPE;

typedef union {
    BUFFER_INFO         BufferInfo;

    SHARE_BUFFER_INFO   ShareBufferInfo;

    AVM_SHARE_BUF_INFO  AVMBufferInfo;

} STREAM_BUFFER;

typedef struct{
    U16 cid;
    U16 connhandle;
    U32 id_ext;
    SOURCE rfcomm;
    VOID* handler;
    U8 is_rfcomm;
}L2CAP_SOURCE_PARAMETER;

typedef struct {
    U8 session_idx;
    U8 dlci;
    U8 rx_credit;
    U8 isFC;
    U32 id_ext;
    VOID* handler;
    BOOL(*MoreSpaceCB)(SOURCE);
}RFCOMM_SOURCE_PARAMETER;

typedef struct {
    U16 dcid;
    U16 connhandle;
    VOID* handler;
    U32 id_ext;
    U16 mtu;
    U16 cid;
} L2CAP_SINK_PARAMETER;

typedef struct {
    U8 session_idx;
    U8 dlci;
    U8 tx_credit;
    U8 isFC;
    VOID* handler;
    U16 mfs;
    BOOL creditFail;
    BOOL L2Fail;
} RFCOMM_SINK_PARAMETER;

typedef struct{
    struct SCO_PARA_CTRL_s
    {
    U32 codec_type             :2;  // mSBC,CVSD,transparent
    U32 data_length            :16;
    U32 RESERVE                :14; // reserved
    } field;
    U8* sbc_codec_param_ptr;
    S16 mSBC_frame_cnt;
}SCO_PARAMETER;

typedef struct{
    U16 process_data_length;
    U16 frame_length;
#ifdef MTK_BT_HFP_FORWARDER_ENABLE
    AVM_SHARE_BUF_INFO_PTR share_info_base_addr;
#else
    SHARE_BUFFER_INFO_PTR share_info_base_addr;
#endif
    BOOL IsFirstIRQ;
    BOOL dl_enable_ul;
    U16 write_offset_advance;
    U32 ul_play_gpt;
#ifdef MTK_BT_HFP_FORWARDER_ENABLE
    bool rx_forwarder_en;
    bool tx_forwarder_en;
#endif
#ifdef DEBUG_HFP_PLK_LOST
    U32 lost_pkt_num;
    U32 forwarder_pkt_num;
#endif
    BOOL dl_reinit;
    BOOL ul_reinit;
}N9SCO_PARAMETER;

#ifdef AIR_BT_CODEC_BLE_ENABLED
typedef struct{
    U16 process_number; /* the process package number */
    U16 frame_length; /* payload size */
    SHARE_BUFFER_INFO_PTR share_info_base_addr;
    BOOL IsFirstIRQ;
    BOOL ul_only;
    BOOL dl_only;
    BOOL skip_after_drop;
    BOOL pkt_lost_state;
    U8  dl_afe_skip_time;
    U16 write_offset_advance;
    U32 ul_play_gpt; /* record the time to trigger UL start up */
    U16 dual_cis_status;
    U8  dual_cis_buffer_offset;
    U8  dual_cis_sub_channel_sel;
    SHARE_BUFFER_INFO_PTR sub_share_info_base_addr;
    U32 context_type;
    U32 seq_num; /* total sequence number of received frame */
    U32 predict_timestamp;
    U32 frame_interval;   /* unit: us   (7500 = 7.5ms) */
    U32 ret_window_len; /* unit :us */
    U16 iso_interval; /* unit :us */
    U8  ft;
    U8  seq_miss_cnt;
    U8  frame_per_iso;
    U8  predict_frame_counter;
    U32 underrun_recovery_size;
}N9BLE_PARAMETER;
#endif


/**
 *  @brief This structure defines the sink CM4 record information.
 */
typedef struct{
    SHARE_BUFFER_INFO_PTR share_info_base_addr;
    U16 process_data_length;
    U16 frame_length;
    U32 frames_per_message;
    DSP_ENC_BITRATE_t bitrate;
    bool interleave;
}CM4_RECORD_PARAMETER;

typedef struct{
    U8 *addr;
    U32 length;
}REGION_PARAMETER;

typedef struct{
    U8  channel_sel;
    U16 frame_size;
    STREAM_SRC_VDM_PTR_t src;
    U8  channel_num;
    TRANSFORM transform;
}DSP_PARAMETER;

typedef struct{
    U8  channel_sel;
    U16 frame_size;
    STREAM_SRC_VDM_PTR_t src;
    U8  channel_num;
    U8  HW_type;
    VOID* handler;
    afe_pcm_format_t format;
    uint32_t rate;                    /* rate in Hz       */
    uint32_t src_rate;
    uint32_t count;                    /* trigger periods  */
    float period;                /* delay in ms      */
    uint32_t buffer_size;            /* buffer size      */
    uint32_t format_bytes;
    uint32_t sram_empty_fill_size;
    afe_block_t AfeBlkControl;     // for interleaved
    afe_stream_channel_t connect_channel_type;
    bool is_pcm_ul1_open;
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
    uint16_t                                Frame_Size;
    uint8_t                                 Buffer_Frame_Num;
#endif
    hal_audio_device_t               audio_device;
#ifdef ENABLE_2A2D_TEST
    hal_audio_device_t               audio_device1;
    hal_audio_device_t               audio_device2;
    hal_audio_device_t               audio_device3;
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    hal_audio_device_t               audio_device4;
    hal_audio_device_t               audio_device5;
    hal_audio_device_t               audio_device6;
    hal_audio_device_t               audio_device7;
#endif
#endif
    hal_audio_channel_selection_t    stream_channel;
    hal_audio_memory_t                      memory;
    hal_audio_interface_t                   audio_interface;
#ifdef ENABLE_2A2D_TEST
    hal_audio_interface_t                   audio_interface1;
    hal_audio_interface_t                   audio_interface2;
    hal_audio_interface_t                   audio_interface3;
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    hal_audio_interface_t                   audio_interface4;
    hal_audio_interface_t                   audio_interface5;
    hal_audio_interface_t                   audio_interface6;
    hal_audio_interface_t                   audio_interface7;
#endif
#endif
    audio_pcm_ops_p                         ops;
    uint32_t                                misc_parms;
    bool     hw_gain;
    bool     echo_reference;
    bool     irq_exist;
    U8       afe_wait_play_en_cnt;
    bool     aws_sync_request;
    uint32_t aws_sync_time;
    uint16_t pop_noise_pkt_num;
    bool     mute_flag;

    bool with_sink_src;
    bool     linein_scenario_flag;
    bool     lineout_scenario_flag;
    bool     is_memory_start;
#ifdef HAL_AUDIO_ENABLE_PATH_MEM_DEVICE
    //modify for ab1568
    //for hal_audio_set_path
    hal_audio_path_parameter_t path_handle;
    //for hal_audio_set_memory
    hal_audio_memory_parameter_t mem_handle;
    //for hal_audio_set_device
    hal_audio_device_parameter_t device_handle;
#ifdef ENABLE_2A2D_TEST
hal_audio_device_parameter_t device_handle1;
hal_audio_device_parameter_t device_handle2;
hal_audio_device_parameter_t device_handle3;
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
hal_audio_device_parameter_t device_handle4;
hal_audio_device_parameter_t device_handle5;
hal_audio_device_parameter_t device_handle6;
hal_audio_device_parameter_t device_handle7;
#endif
#endif
#endif
    uint8_t sw_channels;
    bool                     with_upwdown_sampler; /*if need up or down sampler flag*/
    uint32_t                 audio_path_input_rate; /**< for audio_path_input_rate */
    uint32_t                 audio_path_output_rate; /**< for audio_path_output_rate */
#ifdef AIR_HFP_DNN_PATH_ENABLE
    bool enable_ul_dnn;
#endif
    uint32_t scenario_id;
    uint32_t scenario_sub_id;
    bool     drop_redundant_data_at_first_time;
    bool     enable_clk_skew;
}AUDIO_PARAMETER;

typedef struct
{
    U8 tone;
    U8 CycleNum;
    S16 volume_start;
    S16 volume_step;
    S16 volume_max;
}RT_INFO_CTL_STRU;

typedef union VPRT_PARA_CTRL_u
{
    RT_INFO_CTL_STRU RTInfo;
    U32 Pattern_Length;
} VPRT_PARA_CTRL_t;

typedef struct{
    U8  mode;
    U16 PatternNum;
    VPRT_PARA_CTRL_t para;
}VPRT_PARAMETER;


typedef struct{
    U8  channel_sel;
    U8  sampling_rate;
    U16 frame_size;
    U8  resolution;
}USBCLASS_PARAMETER;

typedef struct{
    U32 offset;
    U8* ptr;
}AUDIO_QUEUE_PARAMETER;

typedef struct
{
    U32 readOffset;
    U32 totalFrameCnt;
    U32 dropFrameCnt;
    U8 samplingFreq;
    U16 inSize;
}A2DP_PARAMETER;

typedef struct{
    VOID (*entry)(U8* ptr, U32 length);
    U32 mem_size;
    VOID* handler;
    int8_t user_count;
}AUDIO_VIRTUAL_PARAMETER;

typedef struct
{
    U32 max_output_length;
    U32 remain_len;
    U8 temp4copy;
    U8 memory_type;
    VOID* handler;
}MEMORY_PARAMETER;

/* N9 A2DP parameter structure */
/**
 *  @brief Define for A2DP codec type.
 */
#define BT_A2DP_CODEC_SBC      (0)           /**< SBC codec. */
#define BT_A2DP_CODEC_AAC      (2)           /**< AAC codec. */
#define BT_A2DP_CODEC_AIRO_CELT (0x65)        /**< Airoha CELT codec. */
#define BT_A2DP_CODEC_VENDOR   (0xFF)           /**< VENDOR codec. */
#define BT_A2DP_CODEC_VENDOR_2 (0xA5)           /**< VENDOR codec. */
#define BT_HFP_CODEC_CVSD  (1)           /**< SBC codec. */
#define BT_HFP_CODEC_mSBC  (2)           /**< AAC codec. */

#ifdef AIR_BT_CODEC_BLE_ENABLED
#define BT_BLE_CODEC_LC3   (1)           /**< LC3 codec. */

#define CODEC_AUDIO_MODE    0x0         /* For music play */
#define CODEC_VOICE_MODE    0x1         /* For phone call */

#define SINGLE_CIS_MODE 0x0
#define DUAL_CIS_MODE   0x1

#define SINGLE_CIS_MODE 0x0
#define DUAL_CIS_MODE   0x1

typedef enum {
    DUAL_CIS_DISABLED      = 0,
    DUAL_CIS_WAITING_MAIN  = 1,
    DUAL_CIS_WAITING_SUB   = 2,
    DUAL_CIS_BOTH_ENABLED  = 3,
} dual_cis_status;

typedef enum {
    SUB_CIS_CHANNEL_L      = 0,
    SUB_CIS_CHANNEL_R      = 1,
} dual_cis_channel_status;

typedef enum {
    BLE_CONTEXT_PROHIBITED       = 0x0000,
    BLE_CONTEXT_UNSPECIFIED      = 0x0001,
    BLE_CONTEXT_CONVERSATIONAL   = 0x0002,
    BLE_CONTEXT_MEDIA            = 0x0004,
    BLE_CONTEXT_GAME             = 0x0008,
    BLE_CONTEXT_INSTRUCTIONAL    = 0x0010,
    BLE_CONTEXT_VOICE_ASSISTANTS = 0x0020,
    BLE_CONTEXT_LIVE             = 0x0040,
    BLE_CONTEXT_SOUND_EFFECTS    = 0x0080,
    BLE_CONTEXT_NOTIFICATIONS    = 0x0100,
    BLE_CONTEXT_RINGTONE         = 0x0200,
    BLE_CONTEXT_ALERTS           = 0x0400,
    BLE_CONTEXT_EMERGENCY_ALARM  = 0x0800,
} bt_ble_context_type_t;

#endif

typedef U8 bt_a2dp_codec_type_t;    /**< The type of A2DP codec. */
typedef U8 bt_hfp_codec_type_t;    /**< The type of HFP codec  */ /**< 1: CVSD, 2: mSBC */
#ifdef AIR_BT_CODEC_BLE_ENABLED
typedef U8 bt_ble_codec_type_t;    /**< The type of BLE codec  */ /**< 1: LC3 */
#endif

/**
 *  @brief Define for A2DP role type.
 */

#define BT_A2DP_SOURCE          (0)    /**< SRC role. */
#define BT_A2DP_SINK            (1)    /**< SNK role. */
#define BT_A2DP_SOURCE_AND_SINK (2)    /**< Both roles for a single device (series case). */
#define BT_A2DP_INVALID_ROLE    (0xFF) /**< Invalid role. */
typedef U8 bt_a2dp_role_t;        /**< The type of A2DP role. */

/**
 *  @brief This structure defines the SBC codec details.
 */
typedef struct {
    U8 min_bit_pool;       /**< The minimum bit pool. */
    U8 max_bit_pool;       /**< The maximum bit pool. */
    U8 block_length;       /**< b0: 16, b1: 12, b2: 8, b3: 4. */
    U8 subband_num;        /**< b0: 8, b1: 4. */
    U8 alloc_method;       /**< b0: loudness, b1: SNR. */
    U8 sample_rate;        /**< b0: 48000, b1: 44100, b2: 32000, b3: 16000. */
    U8 channel_mode;       /**< b0: joint stereo, b1: stereo, b2: dual channel, b3: mono. */
} bt_codec_sbc_t;

/**
 *  @brief This structure defines the AAC codec details.
 */
typedef struct {
    BOOL vbr;              /**< Indicates if VBR is supported or not. */
    U8 object_type;        /**< b4: M4-scalable, b5: M4-LTP, b6: M4-LC, b7: M2-LC. */
    U8 channels;           /**< b0: 2, b1: 1. */
    U16 sample_rate;       /**< b0~b11: 96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000. */
    U32 bit_rate;          /**< Constant/peak bits per second in 23 bit UiMsbf. A value of 0 indicates that the bit rate is unknown. */
} bt_codec_aac_t;

typedef struct {
    uint16_t codec_id;
    uint8_t channels;           /**< b0: 2, b1: 1. */
    uint8_t sample_rate;       /**< b0~b11: 96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000. */
    uint32_t vendor_id;          /**< Constant/peak bits per second in 23 bit UiMsbf. A value of 0 indicates that the bit rate is unknown. */
} bt_codec_vendor_t;



/**
 *  @brief This union defines the A2DP codec.
 */
typedef union {
    bt_codec_sbc_t sbc;    /**< SBC codec. */
    bt_codec_aac_t aac;    /**< AAC codec. */
    bt_codec_vendor_t vend;  /**< vend codec. */
} bt_codec_t;

/**
 *  @brief This structure defines the A2DP codec capability.
 */
typedef struct {
    bt_a2dp_codec_type_t type;  /**< Codec type. */
    bt_codec_t codec;           /**< Codec information. */
} bt_codec_capability_t;

/** @brief This structure defines the A2DP codec. */
typedef struct {
    bt_codec_capability_t codec_cap;    /**< The capabilities of Bluetooth codec */
    bt_a2dp_role_t             role;    /**< The Bluetooth codec roles */
} bt_codec_a2dp_audio_t;

// typedef struct {
    // bt_a2dp_codec_type_t type;  /**< Codec type. */
    // bt_codec_t codec;           /**< Codec information. */
// } bt_codec_capability_t;

// /** @brief This structure defines the A2DP codec. */
// typedef struct {
    // bt_codec_capability_t codec_cap;    /**< The capabilities of Bluetooth codec */
    // bt_a2dp_role_t             role;    /**< The Bluetooth codec roles */
// } bt_codec_a2dp_audio_t;
typedef struct {
    U16  a2dp_report_cnt;    /**< The capabilities of Bluetooth codec */
    U16  a2dp_accumulate_cnt;    /**< The Bluetooth codec roles */
    U32* p_a2dp_bitrate_report;
} bt_a2dp_bitrate_report;

#define MAX_IOS_SBC_LOST_PTK    (10)
typedef struct _bt_a2dp_lost_ptk_t{
    U32 u4SeqNo;
    U32 u4NumFrameToPad;
    U32 u4NumFramePadded;
    BOOL verified_flag;
} bt_a2dp_lost_ptk_t;

#if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
typedef struct {
    U32 FirstAnchor;
    U32 MinAfeSinkSize;
    U32 AccumulatedSamples;
    U8  CurrentSeqNo;
    U8  CntIdx;
    U8  DebugSeqNo;
    BOOL Initialized;
    U32 AfeAnchor;
    U32 AfeAnchorIntra;
    U32 MuteCnt;
    U8 BypassDrop;
    U8 SeqNotMatchCnt;
} AIROHA_GAMING_CTRL, *AIROHA_GAMING_CTRL_PTR;

typedef struct {
    U8  SeqNo;
    U8  FrameDataBegin[1];
} AIROHA_GAMING_FRAME_TYPE, *AIROHA_GAMING_FRAME_PTR;

typedef struct {
    U8  FrameNo;
    U8  FrameSize;
    U16 CheckSum;
    U32 NewestAnchor;
    U8  FrameBegin[1];
} AIROHA_GAMING_BUFFER_TYPE, *AIROHA_GAMING_BUFFER_PTR;
#endif

typedef struct
{
    bt_codec_a2dp_audio_t codec_info;
    BOOL cp_exist;
    BOOL mce_flag;/* 1: AWS MCE mode */
    BOOL fragment_flag;
    BOOL ios_aac_flag;
    U32  share_info_base_addr;
    U32  pkt_lost_report_state;
    U32  predict_timestamp;
    U32  timestamp_ratio;
    U32  predict_asi;
    U32  current_packet_size;
    U32  current_frame_size;
    U32  current_bitstream_size;
    U32  current_seq_num;
    U32  current_timestamp;
    U32  readOffset;/* frame offset, also store AFE buffer report in avm mode  */
    U32  totalFrameCnt;/* if fragmented, the number of remaining fragments, including current fragment.
                          if non-fragmented, the number of frames contained in this packet. */
    U32  dropFrameCnt;
    U32  current_asi;
    U32  *asi_buf;
    U32  *min_gap_buf;
    U32  prev_seq_num;
    U16  DspReportStartId;
    bt_a2dp_bitrate_report  a2dp_bitrate_report;
    U32  sink_latency;
    U16  buffer_empty_cnt;
    BOOL ios_sbc_flag;
    BOOL latency_monitor;
    BOOL alc_monitor;
    BOOL asi_sync_en;
    BOOL resend_reinit_req;
    U16 pkt_loss_seq_no;
    U16 padding_count;
    U16 u2OverPadFrameCnt;
    U32  *a2dp_lostnum_report; // U32 lostnum U32 current_ts
    volatile AUDIO_SYNC_INFO *sync_info;
    U32  *pcdc_info_buf;
#if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
    AIROHA_GAMING_BUFFER_PTR pkt_ptr;
    U8 FrameNo;
    U16 PktNewestSeqNo;
    U16 PktCnt;
#endif
}N9_A2DP_PARAMETER;

/* N9 A2DP parameter structure */

/** @brief Define audio sampling rate. */
typedef enum {
    AUDIO_SAMPLING_RATE_8KHZ      = 0, /**< 8000Hz  */
    AUDIO_SAMPLING_RATE_11_025KHZ = 1, /**< 11025Hz */
    AUDIO_SAMPLING_RATE_12KHZ     = 2, /**< 12000Hz */
    AUDIO_SAMPLING_RATE_16KHZ     = 3, /**< 16000Hz */
    AUDIO_SAMPLING_RATE_22_05KHZ  = 4, /**< 22050Hz */
    AUDIO_SAMPLING_RATE_24KHZ     = 5, /**< 24000Hz */
    AUDIO_SAMPLING_RATE_32KHZ     = 6, /**< 32000Hz */
    AUDIO_SAMPLING_RATE_44_1KHZ   = 7, /**< 44100Hz */
    AUDIO_SAMPLING_RATE_48KHZ     = 8, /**< 48000Hz */
    AUDIO_SAMPLING_RATE_96KHZ     = 9  /**< 96000Hz */
} audio_sampling_rate_t;

/** @brief Define the number of bits per second (bps) to stream audio data. */
typedef enum {
    AUDIO_BITS_PER_SAMPLING_16    = 0, /**< 16 bps */
    AUDIO_BITS_PER_SAMPLING_24    = 1  /**< 24 bps */
} audio_bits_per_sample_t;

/** @brief audio channel number define */
typedef enum {
    AUDIO_MONO                  = 0, /**< A single channel.  */
    AUDIO_STEREO                = 1, /**< Two channels. */
    AUDIO_STEREO_BOTH_L_CHANNEL = 2, /**< Two channels, but only output L channel. That is (L, R) -> (L, L). */
    AUDIO_STEREO_BOTH_R_CHANNEL = 3, /**< Two channels, but only output R channel. That is (L, R) -> (R, R). */
    AUDIO_STEREO_BOTH_L_R_SWAP  = 4  /**< Two channels, L and R channels are swapped. That is (L, R) -> (R, L). */
} audio_channel_number_t;

typedef struct
{
    SINK attSINK;
    U16 attTxHandle;
    U8 linkIdx;
}AIRAPP_SINK_PARAMETER;

/** @brief audio channel number define */

typedef enum {
    AUDIO_INOUT_ENABLE           = 0,
    AUDIO_OUTPUT_ONLY            = 1,
    AUDIO_INPUT_ONLY             = 2,
    AUDIO_INOUT_MUTE             = 3,
} audio_inout_mute_ctrl_t;


/**
 *  @brief This structure defines the CM4 playback share information.
 */
typedef struct {
    U32 share_info_base_addr;
    hal_audio_channel_number_t  channel_number;
    U8  bit_type;
    U8  sampling_rate;
    U8  codec_type;
    U8  data_local_address_index;
    U8  data_local_stream_mode;
} cm4_playback_share_info_t;


/**
 *  @brief This structure defines the extracted CM4 playback share information.
 */
typedef struct {
    audio_bits_per_sample_t  bit_type;
    U32 sampling_rate;
    audio_channel_number_t channel_number;
    U8  codec_type;
    U8  source_channels;
    U32 share_info_base_addr;
} cm4_playback_info_t;


/**
 *  @brief This structure defines the source CM4 playback information.
 */
typedef struct
{
    cm4_playback_info_t info;
    U32  current_frame_size;
    U32  remain_bs_size;
    U8   data_request;
    U8   data_refill_request;
} CM4_PLAYBACK_PARAMETER;

typedef struct {
    DSP_ENC_BITRATE_t bitrate;
    U8   first_packet;
    U8   payload_count; // frame count
} data_transl_param_t;

typedef enum {
    AUDIO_DSP_CODEC_TYPE_CVSD = 0,
    AUDIO_DSP_CODEC_TYPE_MSBC,
    AUDIO_DSP_CODEC_TYPE_DNN, // for DNN loopback
    AUDIO_DSP_CODEC_TYPE_LC3,

    AUDIO_DSP_CODEC_TYPE_PCM = 0x100,
    AUDIO_DSP_CODEC_TYPE_SBC,
    AUDIO_DSP_CODEC_TYPE_MP3,
    AUDIO_DSP_CODEC_TYPE_AAC,
    AUDIO_DSP_CODEC_TYPE_VENDOR,
    AUDIO_DSP_CODEC_TYPE_OPUS,
    AUDIO_DSP_CODEC_TYPE_ANC_LC, //for leakage compensation
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
    AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF,
#else
    AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_SZ,
    AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_PZ,
    AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_PZ_FIR,
#endif
} audio_dsp_codec_type_t;//modify for opus

typedef struct {
    uint32_t sample_rate;
    uint32_t channel_mode;
    hal_audio_pcm_format_t format;
} audio_codec_pcm_t;

typedef struct {
    uint8_t min_bit_pool;       /**< The minimum bit pool. */
    uint8_t max_bit_pool;       /**< The maximum bit pool. */
    uint8_t block_length;       /**< b0: 16, b1: 12, b2: 8, b3: 4. */
    uint8_t subband_num;        /**< b0: 8, b1: 4. */
    uint8_t alloc_method;       /**< b0: loudness, b1: SNR. */
    uint8_t sample_rate;        /**< b0: 48000, b1: 44100, b2: 32000, b3: 16000. */
    uint8_t channel_mode;       /**< b0: joint stereo, b1: stereo, b2: dual channel, b3: mono. */
} audio_codec_sbc_t;

typedef struct {
    uint32_t sample_rate;        /**< b0: 48000, b1: 44100, b2: 32000, b3: 16000. */
    uint32_t bit_rate;           /**< b0: 16kbps, b1:32kbps, b2:64kbps. */
    uint32_t channel_mode;       /**< b0: joint stereo, b1: stereo, b2: dual channel, b3: mono. */
} audio_codec_opus_t;

typedef struct {
    uint32_t sample_rate;        /**< b0: 48000, b1: 44100, b2: 32000, b3: 16000. */
    uint32_t bit_rate;           /**< uint:bps. */
    uint32_t channel_mode;       /**< b0: joint stereo, b1: stereo, b2: dual channel, b3: mono. */
    uint32_t frame_interval;     /**< uint:us. */
    uint32_t frame_size;         /**< uint:Byte. */
} audio_codec_lc3_t;

typedef union {
    audio_codec_pcm_t pcm;
    audio_codec_sbc_t sbc;
    audio_codec_opus_t opus;
    audio_codec_lc3_t lc3;
} audio_codec_param_t;

typedef struct{
    /* last timestemp */
    uint32_t last_timestamp;
    /* actual total time */
    uint32_t act_time;
    /* actual times */
    uint32_t act_count;
    /* actual total diff */
    int32_t act_diff;
    /* average diff */
    float ave_diff;
    /* compensatory samples */
    int32_t compen_samples;
} usb_in_clk_skew, bt_out_clk_skew;

typedef struct{
    /* timer handler */
    uint32_t handle;
    /* timer period */
    uint32_t period;
    /* timer callback */
    hal_gpt_callback_t callback;
    /* timer timestemp */
    uint32_t timestamp;
} timer_param;

typedef struct{
    /* codec type */
    audio_dsp_codec_type_t codec_type;
    /* codec parameters */
    audio_codec_param_t codec_param;
    /* receive data period, uint: us */
    uint32_t data_period;
    /* receive data timestemp */
    uint32_t data_timestamp;
    /* timer parameters */
    timer_param timer;
    /* clock skew */
    usb_in_clk_skew clk_skew;
    /* the frames has been processed */
    uint32_t process_frames;
    /* fetch flag */
    uint32_t fetch_flag;
    /* application handle */
    void *handle;
} usb_in_parameter;

typedef struct{
    /* codec type */
    audio_dsp_codec_type_t codec_type;
    /* codec parameters */
    audio_codec_param_t codec_param;
    /* send data period, uint: us */
    uint32_t data_period;
    /* send data timestemp */
    uint32_t data_timestamp;
    /* event status */
    uint32_t event_status;
    /* event timestemp */
    uint32_t event_timestamp;
    /* event fail count */
    uint32_t event_failcount;
    /* timer parameters */
    timer_param timer;
    /* clock skew */
    bt_out_clk_skew clk_skew;
} bt_out_parameter;

typedef union{
    usb_in_parameter usb_in_param;
    bt_out_parameter bt_out_param;
} usb_in_broadcast_param_t;

typedef struct{
    /* codec type */
    audio_dsp_codec_type_t codec_type;
    /* codec parameters */
    audio_codec_param_t codec_param;
    /* receive data period, uint: us */
    uint32_t data_period;
    /* receive data timestemp */
    uint32_t data_timestamp;
    /* event status */
    uint32_t event_status;
    /* event timestemp */
    uint32_t event_timestamp;
    /* event fail count */
    uint32_t event_failcount;
    /* timer parameters */
    timer_param timer;
    /* the frames has been processed */
    uint32_t process_frames;
    /* the frames needed to be dropped */
    uint32_t drop_frames;
    /* fetch flag */
    uint32_t fetch_flag;
    /* application handle */
    void *handle;
} bt_in_parameter;

typedef struct{
    /* codec type */
    audio_dsp_codec_type_t codec_type;
    /* codec parameters */
    audio_codec_param_t codec_param;
    /* send data period, uint: us */
    uint32_t data_period;
    /* send data timestemp */
    uint32_t data_timestamp;
    /* timer parameters */
    timer_param timer;
} usb_out_parameter;

typedef union{
    bt_in_parameter     bt_in_param;
    usb_out_parameter   usb_out_param;
} usb_out_broadcast_param_t;

typedef struct{
    U8   seq_num;
    U8   frame_size;
    U16  checksum;
    U8   process_frame_num;
    bool ul_process_done;
    U8   dl_irq_cnt;
} game_headset_voice_param_t;

typedef struct{
    usb_in_parameter usb_in_param;
    bool is_use_afe_dl3;
    bool is_dummy_data;
    bool is_afe_irq_comming;
} usb_in_local_param_t;

typedef struct {
    audio_dsp_codec_type_t codec_type;
    audio_codec_param_t codec_param;
    uint32_t period;
    int32_t gain_default_L;
    int32_t gain_default_R;
} gaming_mode_param_t;

typedef gaming_mode_param_t wired_audio_param_t;

typedef union {
    uint32_t reserved;
    /* For scenario specific struct define */
    data_transl_param_t data_transl_param;
    usb_in_broadcast_param_t usb_in_broadcast_param;
    usb_out_broadcast_param_t usb_out_broadcast_param;
    game_headset_voice_param_t voice_param;
    usb_in_local_param_t usb_in_local_param;
    wired_audio_param_t wired_audio_param;
} audio_transmitter_scenario_param_t;

typedef struct {
    U8   scenario_type;
    U8   scenario_sub_id;
    U16  current_notification_index;
    U16  data_notification_frequency; /* frequency of notice CM4 */
    uint16_t max_payload_size; /* max allowed size in a block */
    U16  seq_num;
    U16  frame_size; /* current actually frame size */
    bool is_assembling; /* indicate whether the package has been generated done */
    bool is_customize; /* indicate whether user want to assemble the package itself */
    SHARE_BUFFER_INFO_PTR share_info_base_addr;
    audio_transmitter_scenario_param_t scenario_param;
} AUDIO_TRANSMITTER_PARAMETER;

typedef union {
    uint32_t reserved;
    /* For scenario specific struct define */
    usb_in_broadcast_param_t    usb_in_broadcast_param;
    usb_out_broadcast_param_t   usb_out_broadcast_param;
} bt_common_scenario_param_t;

typedef struct {
    U8   scenario_type;
    U8   scenario_sub_id;
    U16  current_notification_index;
    U16  data_notification_frequency; /* frequency of notice BT controller */
    uint16_t max_payload_size; /* max allowed size in a block */
    U16  seq_num;
    U16  frame_size; /* current actually frame size */
    uint32_t status;
    STREAM_BUFFER_TYPE share_info_type;
    STREAM_BUFFER *share_info_base_addr;
    // SHARE_BUFFER_INFO_PTR share_info_base_addr;
    bt_common_scenario_param_t scenario_param;
} BT_COMMON_PARAMETER;

typedef union {
    L2CAP_SOURCE_PARAMETER  l2cap;
    RFCOMM_SOURCE_PARAMETER rfcomm;
    SCO_PARAMETER           sco;
    N9SCO_PARAMETER         n9sco;
#ifdef AIR_BT_CODEC_BLE_ENABLED
    N9BLE_PARAMETER         n9ble;
#endif
    REGION_PARAMETER        region;
    DSP_PARAMETER           dsp;
    AUDIO_PARAMETER         audio;
    VPRT_PARAMETER          VPRT;
    USBCLASS_PARAMETER      USB;
    AUDIO_QUEUE_PARAMETER   audioQ;
    A2DP_PARAMETER          a2dp;
    MEMORY_PARAMETER        memory;
    N9_A2DP_PARAMETER       n9_a2dp;
    CM4_PLAYBACK_PARAMETER  cm4_playback;
    AUDIO_TRANSMITTER_PARAMETER data_dl;
    BT_COMMON_PARAMETER     bt_dl;
} SOURCE_PARAMETER;

typedef union {
    L2CAP_SINK_PARAMETER    l2cap;
    SCO_PARAMETER           sco;
    N9SCO_PARAMETER         n9sco;
#ifdef AIR_BT_CODEC_BLE_ENABLED
    N9BLE_PARAMETER         n9ble;
#endif
    RFCOMM_SINK_PARAMETER   rfcomm;
    REGION_PARAMETER        region;
    DSP_PARAMETER           dsp;
    AUDIO_PARAMETER         audio;
    USBCLASS_PARAMETER      USB;
    AUDIO_QUEUE_PARAMETER   audioQ;
    AUDIO_VIRTUAL_PARAMETER virtual_para;
    AIRAPP_SINK_PARAMETER   airapp;
    MEMORY_PARAMETER        memory;
    CM4_RECORD_PARAMETER    cm4_record;
    AUDIO_TRANSMITTER_PARAMETER data_ul;
    BT_COMMON_PARAMETER     bt_ul;
} SINK_PARAMETER;


typedef struct
{
    U8 type;
    U8 addr[6];
} BDADDR;
typedef struct { U32 lap; U8 uap; U16 nap; } BD_ADDR;

/** @brief Mark highest bit for the valid of scenario type. */
#define HAL_AUDIO_STREAM_IN_SCENARIO_MARK 0x8000

/** @brief Define the audio stream in scenario type. */
typedef enum {
    HAL_AUDIO_STREAM_IN_SCENARIO_HFP = 0,  /**<  stream in scenario for HFP. */
    HAL_AUDIO_STREAM_IN_SCENARIO_LINE_IN,  /**<  stream in scenario for LINE IN. */
    HAL_AUDIO_STREAM_IN_SCENARIO_BLE_CALL, /**<  stream in scenario for BLE CALL. */
    HAL_AUDIO_STREAM_IN_SCENARIO_MAX,      /**<  stream in scenario max. */
    HAL_AUDIO_STREAM_IN_SCENARIO_INVALID,  /**<  stream in scenario invalid. */
} hal_audio_stream_in_scenario_t;

/* Open message member parameter structure */
typedef enum {
    STREAM_IN_AFE  = 0,
    STREAM_IN_HFP,
    STREAM_IN_BLE,
    STREAM_IN_A2DP,
    STREAM_IN_PLAYBACK,
    STREAM_IN_VP,
    STREAM_IN_VP_DUMMY_SOURCE,
    STREAM_IN_GSENSOR,
    STREAM_IN_AUDIO_TRANSMITTER,
    STREAM_IN_BT_COMMON,
    STREAM_IN_DUMMY = 0xFFFFFFFF,
} mcu2dsp_stream_in_selection;

typedef enum {
    STREAM_OUT_AFE  = 0,
    STREAM_OUT_HFP,
    STREAM_OUT_BLE,
    STREAM_OUT_RECORD,
    STREAM_OUT_VIRTUAL,
    STREAM_OUT_AUDIO_TRANSMITTER,
    STREAM_OUT_BT_COMMON,
    STREAM_OUT_DUMMY = 0xFFFFFFFF,
} mcu2dsp_stream_out_selection;



typedef struct {
    mcu2dsp_stream_in_selection     stream_in;
    mcu2dsp_stream_out_selection    stream_out;
    U32                      *Feature;
}  mcu2dsp_param_t, *mcu2dsp_param_p;

typedef struct {
    hal_audio_device_t               audio_device;
    hal_audio_device_t               audio_device1;
    hal_audio_device_t               audio_device2;
    hal_audio_device_t               audio_device3;
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    hal_audio_device_t               audio_device4;
    hal_audio_device_t               audio_device5;
    hal_audio_device_t               audio_device6;
    hal_audio_device_t               audio_device7;
#endif
    hal_audio_channel_selection_t    stream_channel;
    hal_audio_memory_t                      memory;
    hal_audio_interface_t                   audio_interface;
#ifdef ENABLE_2A2D_TEST
    hal_audio_interface_t                   audio_interface1;
    hal_audio_interface_t                   audio_interface2;
    hal_audio_interface_t                   audio_interface3;
#endif
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
#ifdef ENABLE_2A2D_TEST
    hal_audio_interface_t                   audio_interface4;
    hal_audio_interface_t                   audio_interface5;
    hal_audio_interface_t                   audio_interface6;
    hal_audio_interface_t                   audio_interface7;
#endif
    /*uplink adc mode:
        0x0: ACC_10k,
        0x1: ACC_20k,
        0x2: DCC,
    */
    uint8_t                                 ul_adc_mode[8];
    /*amic type
        0x0: MEMS,
        0x1: ECM Differential,
        0x2: ECM Single,
    */
    uint8_t                                 amic_type[8];
    /*downlink dac mode
        0x0: Class_G,
        0x1: Class_AB,
        0x2: Class_D,
    */
    uint8_t                                 dl_dac_mode;
    /*  uplink:
        0x0: Normal_Mode,
        0x1: High_Performance,
        0x2: Low_Power_mode,
        0x3: Ultra_Low_Power_mode,
        0x4: Super_Ultra_Low_Power_mode,
        downlink:
        0x0: Class_G,
        0x1: Class_AB,
        0x2: Class_D,
        */
    //uint8_t                                 performance;/**hal_audio_analog_mdoe_t*/

    /*bias voltage,support 5 bia voltage
        0x0: 1.8V,
        0x1: 1.85V,
        0x2: 1.9V,
        0x3: 2.0V,
        0x4: 2.1V,
        0x5: 2.2V,
        0x6: 2.4V,
        0x7: 2.55V */
    uint8_t                                 bias_voltage[5];/**hal_audio_bias_voltage_t*/

    /*bias enable
        bit mask to enable Bias
        8'b 00000001: Bias 0,
        8'b 00000010: Bias 1,
        8'b 00000100: Bias 2,
        8'b 00001000: Bias 3,
        8'b 00010000: Bias 4,*/
    uint8_t                                 bias_select;/**hal_audio_bias_selection_t*/
    /*external bias enable (reserve)*/
    uint8_t                                 with_external_bias;/**< for with_external_bias */
    /*bias lowpower enable (reserve)*/
    uint8_t                                 with_bias_lowpower;/**< for with_bias_lowpower */
    /*define if bias1_2 config with LDO0 (reserve)*/
    bool                                    bias1_2_with_LDO0;/**< for bias1_2_with_LDO0 */
    /*DMIC select (reserve???)*/
    uint8_t                                 dmic_selection[8];/**< hal_audio_dmic_selection_t */
    /*iir filter (reserve)*/
    uint8_t                                 iir_filter[3];/**hal_audio_ul_iir_t*/
    /*I2S format (reserve)*/
    uint8_t                                 i2s_format;/**< hal_audio_i2s_format_t */
    /*I2S slave TDM (reserve)*/
    uint8_t                                 i2S_Slave_TDM;
    /*I2S word length(reserve)*/
    uint8_t                                 i2s_word_length;/**< hal_audio_i2s_word_length_t */
    /*I2S low jitter config
        false, DCXO
        true,  APLL*/
    bool                                    is_low_jitter;
#endif

    afe_pcm_format_t                        format;
    uint32_t                                misc_parms;
    uint32_t                                sampling_rate;
    uint32_t                                stream_out_sampling_rate;
    uint16_t                                frame_size;
    uint8_t                                 frame_number;
    float                                   irq_period;
    uint8_t                                 sw_channels;
    bool                                    hw_gain;
#if defined(LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA) || defined(HAL_AUDIO_ENABLE_PATH_MEM_DEVICE)
    hal_audio_analog_mdoe_t                 adc_mode;
    hal_audio_performance_mode_t            performance;
#endif
#ifdef ENABLE_HWSRC_CLKSKEW
    clkskew_mode_t                          clkskew_mode;
#endif
#ifdef AIR_HFP_DNN_PATH_ENABLE
    bool                                    enable_ul_dnn;
#endif
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    /*DMIC clock(reserve)*/
    uint8_t                                 dmic_clock_rate[3];/**afe_dmic_clock_rate_t*/
#endif
    bool                                    with_upwdown_sampler; /*if need up or down sampler flag*/
    uint32_t                                audio_path_input_rate; /**< for audio_path_input_rate */
    uint32_t                                audio_path_output_rate; /**< for audio_path_output_rate */
} au_afe_open_param_t,*au_afe_open_param_p;

typedef struct
{
    U8 enable;
} dsp_audio_plc_ctrl_t, *dsp_audio_plc_ctrl_p;

typedef struct {
    bt_codec_a2dp_audio_t codec_info;       /**< Codec cap. */
#ifdef MTK_BT_HFP_FORWARDER_ENABLE
    AVM_SHARE_BUF_INFO_PTR p_share_info;                       /**< Codec information. */
#else
    SHARE_BUFFER_INFO_PTR p_share_info;                       /**< Codec information. */
#endif
    uint32_t              *p_asi_buf;
    uint32_t              *p_min_gap_buf;
    uint32_t              *p_current_bit_rate;
    uint32_t              sink_latency;
    uint32_t              bt_inf_address;
#ifdef MTK_BT_HFP_FORWARDER_ENABLE
    uint32_t              p_afe_buf_report;// AVM only
#else
    uint32_t              clk_info_address;
#endif
    uint32_t              *p_lostnum_report;
    AUDIO_SYNC_INFO       *p_audio_sync_info;
    uint32_t              *p_pcdc_anchor_info_buf;
#ifdef MTK_GAMING_MODE_HEADSET
    uint32_t              *ull_clk_info_address;
#endif
#ifdef MTK_AUDIO_PLC_ENABLE
    dsp_audio_plc_ctrl_t  audio_plc;
#endif
} bt_a2dp_open_param_t, *bt_a2dp_open_param_p;

typedef struct {
    bt_hfp_codec_type_t   codec_type;         /**< Codec type. */
#ifdef MTK_BT_HFP_FORWARDER_ENABLE
    AVM_SHARE_BUF_INFO_PTR p_share_info;                       /**< Codec information. */
#else
    SHARE_BUFFER_INFO_PTR p_share_info;                       /**< Codec information. */
#endif
    uint32_t              bt_inf_address;
    uint32_t              clk_info_address;
    uint32_t              p_air_dump_buf;
    uint32_t               p_rx_audio_forwarder_buf;
    uint32_t               p_tx_audio_forwarder_buf;
} bt_hfp_open_param_t, *bt_hfp_open_param_p;

#ifdef AIR_BT_CODEC_BLE_ENABLED

typedef enum {
    CHANNEL_MODE_DL_ONLY,
    CHANNEL_MODE_DL_UL_BOTH,
    CHANNEL_MODE_UL_ONLY,
    CHANNEL_MODE_NUM
} channel_mode_t;

typedef struct {
    bt_ble_codec_type_t   codec_type;         /**< Codec type. */
    uint8_t  channel_num;
    uint8_t  channel_mode;
    uint16_t frame_payload_length;
    uint32_t frame_ms;
    uint32_t sampling_frequency;
    uint32_t context_type;
    uint8_t  dual_cis_mode;
    uint8_t  decode_mode;
    SHARE_BUFFER_INFO_PTR p_share_info;                       /**< Codec information. */
    SHARE_BUFFER_INFO_PTR p_sub_share_info;                   /**< Codec information for second CIS. */
} bt_ble_open_param_t, *bt_ble_open_param_p;

#endif


typedef struct {
    uint32_t codec_type;
    audio_codec_param_t codec_param;
    uint32_t period;
    uint32_t channel_enable;
    int32_t gain_default_L;
    int32_t gain_default_R;
    uint8_t *share_buffer_channel_1;
    uint8_t *share_buffer_channel_2;
} ble_audio_dongle_param_t;

typedef union {
    uint32_t reserved;
    /* For scenario specific struct define */
    gaming_mode_param_t gaming_mode_param;
    wired_audio_param_t wired_audio_param;
    ble_audio_dongle_param_t ble_audio_dongle_param;
} audio_transmitter_scenario_open_param_t;

typedef struct {
    U8 scenario_type;
    U8 scenario_sub_id;
    U16  data_notification_frequency;
    uint16_t max_payload_size;
    SHARE_BUFFER_INFO_PTR p_share_info;
    audio_transmitter_scenario_open_param_t scenario_param;
} audio_transmitter_open_param_t, *audio_transmitter_open_param_p;

typedef struct {
    SHARE_BUFFER_INFO_PTR p_share_info;
    uint32_t frames_per_message;
    DSP_ENC_BITRATE_t bitrate;
    bool interleave;
} cm4_record_open_param_t, *cm4_record_open_param_p;

typedef union {
    uint32_t reserved;
    /* For scenario specific struct define */
    gaming_mode_param_t gaming_mode_param;
    ble_audio_dongle_param_t ble_audio_dongle_param;
} bt_common_scenario_open_param_t;

typedef struct {
    U8   scenario_type;
    U8   scenario_sub_id;
    U16  data_notification_frequency;
    uint16_t max_payload_size;
    STREAM_BUFFER_TYPE share_info_type;
    STREAM_BUFFER *p_share_info;
    bt_common_scenario_open_param_t scenario_param;
} bt_common_open_param_t, *bt_common_open_param_p;

typedef union {
    au_afe_open_param_t         afe;
    bt_hfp_open_param_t         hfp;
#ifdef AIR_BT_CODEC_BLE_ENABLED
    bt_ble_open_param_t         ble;
#endif
    bt_a2dp_open_param_t        a2dp;
    cm4_playback_share_info_t   playback;
    audio_transmitter_open_param_t data_dl;
    bt_common_open_param_t      bt_dl;
} mcu2dsp_open_stream_in_param_t, *mcu2dsp_open_stream_in_param_p;

typedef union {
    au_afe_open_param_t         afe;
    bt_hfp_open_param_t         hfp;
#ifdef AIR_BT_CODEC_BLE_ENABLED
    bt_ble_open_param_t         ble;
#endif
    cm4_record_open_param_t     record;
    audio_transmitter_open_param_t data_ul;
    bt_common_open_param_t      bt_ul;
} mcu2dsp_open_stream_out_param_t, *mcu2dsp_open_stream_out_param_p;

/* Open message parameter structure */
typedef struct {
    mcu2dsp_param_t                 param;
    mcu2dsp_open_stream_in_param_t  stream_in_param;
    mcu2dsp_open_stream_out_param_t stream_out_param;
} mcu2dsp_open_param_t, *mcu2dsp_open_param_p;


/* Start message member parameter structure */
typedef struct {
    uint32_t                    start_time_stamp;
    uint32_t                    time_stamp_ratio;
    uint32_t                    start_asi;
    uint32_t                    start_bt_clk;
    uint32_t                    start_bt_intra_clk;
    bool                        content_protection_exist;
    bool                        alc_monitor;
    bool                        latency_monitor_enable;
} audio_dsp_a2dp_dl_start_param_t, *audio_dsp_a2dp_dl_start_param_p;

typedef struct {
    bool                        mce_flag;
    bool                        aws_sync_request;
    uint32_t                    aws_sync_time;
} audio_dsp_afe_start_param_t, *audio_dsp_afe_start_param_p;

typedef union {
    audio_dsp_a2dp_dl_start_param_t a2dp;
    audio_dsp_afe_start_param_t     afe;
} mcu2dsp_start_stream_in_param_t, *mcu2dsp_start_stream_in_param_p;

typedef union {
    audio_dsp_afe_start_param_t     afe;
} mcu2dsp_start_stream_out_param_t, *mcu2dsp_start_stream_out_param_p;

/* Start message parameter structure */
typedef struct {
    mcu2dsp_param_t                     param;
    mcu2dsp_start_stream_in_param_t     stream_in_param;
    mcu2dsp_start_stream_out_param_t    stream_out_param;
} mcu2dsp_start_param_t, *mcu2dsp_start_param_p;

/* SideTone message parameter structure */
typedef afe_sidetone_param_t mcu2dsp_sidetone_param_t;
typedef afe_sidetone_param_p mcu2dsp_sidetone_param_p;

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
#define PEQ_DIRECT      (0)
#define PEQ_SYNC        (1)
typedef struct {
    uint8_t         *nvkey_addr;
    uint16_t        peq_nvkey_id;
    uint8_t         drc_enable;
    uint8_t         setting_mode;
    uint32_t        target_bt_clk;
    uint8_t         phase_id;
    uint8_t         drc_force_disable;
} mcu2dsp_peq_param_t, *mcu2dsp_peq_param_p;
#endif

//--------------------------------------------
// Temp audio data
//--------------------------------------------
#ifdef HAL_AUDIO_ANC_ENABLE
typedef struct {
    au_afe_open_param_t    adc_setting;
    au_afe_open_param_t    dac_setting;
} mcu2dsp_open_adda_param_t, *mcu2dsp_open_adda_param_p;
#endif

typedef struct {
    uint32_t sequence_number;
    uint32_t bt_clock;
} audio_dsp_a2dp_dl_play_en_param_t, *audio_dsp_a2dp_dl_play_en_param_p;

typedef struct {
    audio_dsp_a2dp_dl_play_en_param_t play_en;
    bool is_play_en_ready;
    bool is_a2dp_started;
} audio_dsp_ull_start_ctrl_param_t;

#endif
