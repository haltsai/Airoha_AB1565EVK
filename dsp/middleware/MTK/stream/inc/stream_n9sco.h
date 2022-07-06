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
 
#ifndef _STREAM_N9SCO_H_
#define _STREAM_N9SCO_H_

/*!
 *@file   stream.h
 *@brief  defines the heap management of system
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
//#include "transform.h"

#include "config.h"
#include "types.h"
#include "source_inter.h"
#include "sink_inter.h"

#include "transform_inter.h"

#include "common.h"

#ifdef MTK_BT_HFP_FORWARDER_ENABLE
#define DL_TRIGGER_UL (1)
#include "bt_types.h"
#else
#define DL_TRIGGER_UL (1)
#endif

////////////////////////////////////////////////////////////////////////////////
// Type Defintions /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef enum {
    SCO_PKT_FREE,
    SCO_PKT_USED,
    SCO_PKT_LOST,
} sco_packet_state;

typedef enum {
    ENABLE_FORWARDER,
    DISABLE_FORWARDER,
} fowarder_ctrl;

typedef enum {
    RX_FORWARDER,
    TX_FORWARDER,
} fowarder_type;

typedef enum {
    SCO_PKT_2EV3 = 0x1,
    SCO_PKT_EV3  = 0x2,
    SCO_PKT_HV2  = 0x3,
    SCO_PKT_HV1  = 0x6,
    SCO_PKT_NULL = -1,
} sco_pkt_type;

typedef enum {
    SCO_PKT_2EV3_LEN = 60,
    SCO_PKT_EV3_LEN  = 30,
    SCO_PKT_HV2_LEN  = 20,
    SCO_PKT_HV1_LEN  = 10,
    SCO_PKT_NULL_LEN = -1,
} sco_pkt_len;


typedef struct N9Sco_Sink_Config_s
{
    BOOL isEnable;
    U16  Buffer_Frame_Num;
    U16  Process_Frame_Num;
    U16  Frame_Size;
    U16  Output_sample_rate;
    U16  N9_Ro_abnormal_cnt;
} N9Sco_Sink_config_t;

typedef struct N9Sco_Source_Config_s
{
    BOOL isEnable;
    U16  Buffer_Frame_Num;
    U16  Process_Frame_Num;
    U16  Frame_Size;
    U16  Input_sample_rate;
} N9Sco_Source_config_t;

typedef struct Stream_n9sco_Config_s
{
    N9Sco_Source_config_t N9Sco_source;
    N9Sco_Sink_config_t N9Sco_sink;
}Stream_n9sco_Config_t, *Stream_n9sco_Config_Ptr;

typedef struct Sco_Rx_InbandInfo_s
{
    struct InbandInf_s
    {
        U32 OFFSET                     : 16;
        U32 RXED                       : 1;
        U32 IS_MUTE                    : 1;
        U32 HEC_ERR                    : 1;
        U32 HEC_FORCE_OK               : 1;
        U32 CRC_ERR                    : 1;
        U32 SNR_ERR                    : 1;
        U32 _RSVD_0_                   : 10;
    } InbandInf;
    struct PICOCLK_s
    {
        U32 PICO_CLK                   : 28;
        U32 _RSVD_0_                   : 4;
    } field2;
    struct SNR0_3_s
    {
        U32 SNR0                       : 5;
        U32 _RSVD_0_                   : 3;
        U32 SNR1                       : 5;
        U32 _RSVD_1_                   : 3;
        U32 SNR2                       : 5;
        U32 _RSVD_2_                   : 3;
        U32 SNR3                       : 5;
        U32 _RSVD_3_                   : 3;
    } SNR0_3;
    struct SNR4_7_s
    {
        U32 SNR4                       : 5;
        U32 _RSVD_0_                   : 3;
        U32 SNR5                       : 5;
        U32 _RSVD_1_                   : 3;
        U32 SNR6                       : 5;
        U32 _RSVD_2_                   : 3;
        U32 SNR7                       : 5;
        U32 _RSVD_3_                   : 3;
    } SNR4_7;
    struct SNR8_9_s
    {
        U32 SNR8                       : 5;
        U32 _RSVD_0_                   : 3;
        U32 SNR9                       : 5;
        U32 _RSVD_1_                   : 19;
    } SNR8_9;
}Sco_Rx_InbandInfo_t, *Sco_Rx_InbandInfo_ptr;


////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
EXTERN VOID SinkInitN9Sco(SINK sink);
EXTERN VOID SourceInitN9Sco(SOURCE source);
EXTERN Stream_n9sco_Config_Ptr N9SCO_setting;
EXTERN VOID Source_N9Sco_Buffer_Init(SOURCE source);

#ifdef MTK_BT_HFP_FORWARDER_ENABLE
EXTERN hal_nvic_status_t Sco_Audio_Fwd_Ctrl(fowarder_ctrl forwarder_en, fowarder_type forwarder_type);
EXTERN void Forwarder_IRQ_init(BOOL isRx);
EXTERN VOID Sco_RX_IntrHandler(VOID);
U16 Get_RX_FWD_Pattern_Size(VOID);
U16 Get_TX_FWD_Pattern_Size(VOID);
VOID Get_RX_FWD_Pkt_Type(VOID);
EXTERN VOID Sco_TX_IntrHandler(VOID);
EXTERN VOID SCO_Tx_Forwarder_Buf_Init(SINK sink);
EXTERN VOID SCO_Rx_Intr_Ctrl(BOOL ctrl);
EXTERN VOID SCO_Tx_Intr_Ctrl(BOOL ctrl);
EXTERN VOID SCO_Rx_Buf_Ctrl(BOOL ctrl);
EXTERN VOID SCO_Tx_Buf_Ctrl(BOOL ctrl);
EXTERN U32 SCO_Rx_Status (void);
EXTERN U32 SCO_Tx_Status (void);
extern bool SCO_Rx_Check_Disconnect_Status(void);
extern void SCO_Rx_Reset_Disconnect_Status(void);
#ifdef AIR_HFP_SYNC_START_ENABLE
EXTERN bool SCO_Check_Sync_Start_Status(VOID);
EXTERN VOID SCO_Set_DSP_Ready_Status(VOID);
#endif
#ifdef AIR_HFP_SYNC_STOP_ENABLE
EXTERN bool SCO_Check_Sync_Stop_Status(VOID);
EXTERN VOID SCO_Reset_Sync_Stop_Status(VOID);
#endif
EXTERN U32 SCO_Rx_AncClk (void);
EXTERN U32 SCO_RX_FWD_IntrTime(void);
EXTERN VOID SCO_Rx_Intr_HW_Handler(VOID);
EXTERN VOID SCO_Tx_Intr_HW_Handler(VOID);
#ifdef AIR_SOFTWARE_GAIN_ENABLE
EXTERN void Call_UL_SW_Gain_Init(SINK sink);
EXTERN void Call_UL_SW_Gain_Set(int32_t target_gain);
EXTERN void Call_UL_SW_Gain_Mute_control(bool mute);
#endif
#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) && defined(AIR_FIXED_RATIO_SRC)
EXTERN void SCO_UL_Fix_Sample_Rate_Init(void);
EXTERN void SCO_UL_Fix_Sample_Rate_Deinit(void);
#endif
#endif


#endif /* _STREAM_N9SCO_H_ */

