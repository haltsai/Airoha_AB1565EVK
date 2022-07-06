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

#ifndef _STREAM_N9BLE_H_
#define _STREAM_N9BLE_H_

/*!
 *@file   stream.h
 *@brief  defines the heap management of system
 *
 @verbatim
         Author : CharlesSu   <CharlesSu@airoha.com.tw>
                  ChiaHoHu    <ChiaHoHu@airoha.com.tw>
                  HungChiaLin <HungChiaLin@airoha.com.tw>
 @endverbatim
 */

#include "config.h"
#include "types.h"
#include "source_inter.h"
#include "sink_inter.h"

#include "source.h"
#include "sink.h"
//#include "transform.h"

#include "config.h"
#include "types.h"
#include "source_inter.h"
#include "sink_inter.h"

#include "transform_inter.h"

#include "common.h"



////////////////////////////////////////////////////////////////////////////////
// Type Defintions /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


typedef struct N9Ble_Sink_Config_s {
    U16  N9_Ro_abnormal_cnt;
    U16  Buffer_Frame_Num;
    U16  Process_Frame_Num;
    U16  Frame_Size;
//    U16  Output_sample_rate;
//    BOOL isEnable;
} N9Ble_Sink_config_t;

typedef struct N9Ble_Source_Config_s {
    U16  Buffer_Frame_Num;
    U16  Process_Frame_Num;
    U16  Frame_Size;
    U16  Input_sample_rate;
//    BOOL isEnable;
} N9Ble_Source_config_t;

typedef enum {
    BLE_PKT_FREE,
    BLE_PKT_USED,
    BLE_PKT_LOST,
} ble_packet_state;

typedef struct Stream_n9ble_Config_s {
    N9Ble_Source_config_t N9Ble_source;
    N9Ble_Sink_config_t   N9Ble_sink;
} Stream_n9ble_Config_t, *Stream_n9ble_Config_Ptr;

typedef struct  {
    stream_samplerate_t fs_in;
    stream_samplerate_t fs_out;
    stream_samplerate_t fs_tmp1;
    stream_samplerate_t fs_tmp2;
    uint8_t swb_cnt;
} N9ble_Swsrc_Config_t, *N9ble_Swsrc_Config_Ptr;



////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
EXTERN VOID SinkInitN9Ble(SINK sink);
EXTERN VOID SourceInitN9Ble(SOURCE source);
EXTERN Stream_n9ble_Config_Ptr N9BLE_setting;
EXTERN bool ble_query_rx_packet_lost_status(uint32_t index);
ATTR_TEXT_IN_IRAM_LEVEL_2 VOID N9Ble_update_readoffset_share_information(SOURCE source, U32 ReadOffset);
ATTR_TEXT_IN_IRAM_LEVEL_2 VOID N9Ble_update_writeoffset_share_information(SINK sink, U32 WriteOffset);
EXTERN VOID N9Ble_SourceUpdateLocalReadOffset(SOURCE source, U8 num);
#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) && defined(AIR_FIXED_RATIO_SRC)
EXTERN void N9Ble_UL_Fix_Sample_Rate_Init(void);
EXTERN void N9Ble_UL_Fix_Sample_Rate_Deinit(void);
#endif

#if defined(AIR_BLE_SWB_ENABLE) && defined(AIR_FIXED_RATIO_SRC)
EXTERN void N9Ble_DL_SWB_Sample_Rate_Init(void);
EXTERN void N9Ble_DL_SWB_Sample_Rate_Deinit(void);
EXTERN void N9Ble_UL_SWB_Sample_Rate_Init(void);
EXTERN void N9Ble_UL_SWB_Sample_Rate_Deinit(void);
#endif


#ifdef AIR_SOFTWARE_GAIN_ENABLE
#include "sw_gain_interface.h"
EXTERN sw_gain_port_t *g_call_sw_gain_port;
EXTERN volatile int32_t g_call_target_gain;
EXTERN void Call_UL_SW_Gain_Mute_control(bool mute);
EXTERN void Call_UL_SW_Gain_Set(int32_t target_gain);
EXTERN void Call_UL_SW_Gain_Deinit(void);
#endif

#endif /* _STREAM_N9SCO_H_ */

