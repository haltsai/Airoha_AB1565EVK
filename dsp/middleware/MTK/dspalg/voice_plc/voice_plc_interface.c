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
#include "dsp_feature_interface.h"
#include "dsp_utilities.h"
#include "dsp_buffer.h"
#include "dsp_share_memory.h"
#include "voice_plc_interface.h"
#include "plcpitch_sim.h"
#include "config.h"
#include "dsp_rom_table.h"
#include "audio_nvdm_common.h"
#include "voice_plc_interface.h"
#include "dsp_dump.h"
#include "string.h"

#ifdef MTK_PLC_USE_PIC
#include "plc_portable.h"
#endif

/**
 *
 *  Definition
 *
 */
#define MAX_PLC_PKT_SIZE                (360)
#define MAX_PLC_OFFSET_SIZE             (60)
#define MIN_PLC_PKT_SIZE                (30)
#define MSBC_DECODED_FRAME_SIZE         (120)


/**
 *
 *  Type Definition
 *
 */

typedef enum SNR_USED_IN_LAST_FRAME_e
{
    PLC_WITHOUT_SNR,
    PLC_NEARLY_SNR,
    PLC_WORST_SNR,
    PLC_MEAN_SNR,
} SNR_USED_IN_LAST_FRAME_t;


typedef enum PLC_MODEM_DATA_TYPE_e
{
    PLC_GFSK = 0,
    PLC_EDR2M,
    PLC_EDR3M
} PLC_MODEM_DATA_TYPE_t;


#if (PLC_VERIFICATION)
typedef struct PLC_IN_BUF_s
{
    U16 wo;
    U16 ro;
    U16 count;
    BOOL enable;
    U16 threshold;
} PLC_IN_BUF_t;
#endif

typedef struct PLC_PKT_INFO_s
{
    U16 PktSize;
    U16 PktType;
    U16 CodecType;
} PLC_PKT_INFO_t;

typedef struct PLC_PARA_STRU_s
{
    PLC_PKT_INFO_t PktInfo;
} PLC_PARA_STRU_t;


/**
 *
 *  External Symbols
 *
 */
extern S16 crc_fail_bad_frame;


/**
 *
 *  Buffer & Control
 *
 */
DSP_ALIGN8 PLC_PARA_STRU_t gPlcCtrl;
DSP_ALIGN8 VOICE_PLC_SCRATCH_PTR_t PlcCtrlPtr;

#if (PLC_USE_FAKE_PARA)
DSP_ALIGN8 VOICE_RX_PKT_STRU_t PlcFakeInfo;
#endif




/**
 *
 * Function Prototype
 *
 */
bool stream_function_plc_initialize (void *para);
bool stream_function_plc_process (void *para);
VOID Voice_PLC_PacketBasedProcess (VOICE_RX_INBAND_INFO_t* para);
VOID Voice_PLC_CollectPlcInfo(U16 PktSize, U16 PktType, U16 CodecType);
VOID Voice_PLC_PktType2PLCInfo(VOID);
VOID Voice_PLC_CalculateFrameSize(VOID);
VOID Voice_PLC_UpdatePacketInfo(U16 CodecType, U16 PktSize, U16 PktType);
VOID Voice_PLC_UpdateInbandInfo(VOICE_RX_INBAND_INFO_t* Ptr, U32 Lengh, U8 PktIdx);

#if (PLC_VERIFICATION)
VOID DSP_PLC_InitializeInputStreamCtrl(VOID);
U32 DSP_PLC_UsbDataInput(U8* getBuf, S32 getLen);
#endif
#ifdef AIR_VOICE_PLC_ENABLE

#if (PLC_USE_FAKE_PARA)
void plcpitch_sim_msbc_init(PLC_state *s)
{
    s->PLC_MUTE_CONSECUTIVE_ERRS_DEFAULT = 8;
    s->PLC_UNITY_GAIN_CONSECUTIVE_GOODFRM_DEFAULT = 14;
    s->PLC_MUTE_PE16_TH_DEFAULT = 10;
    s->PLC_GAIN_UPDATE_PE16_TH_DEFAULT = 15;
    s->PLC_PE16_ADJ_DEFAULT = 0;
    s->PLC_MUTE_GAIN_RATE_FACTOR_ATTACK_DEFAULT = 10;
    s->PLC_MUTE_GAIN_RATE_FACTOR_RELEASE_DEFAULT = 10;
    s->PLC_WEIGHT_PKTLOSS_EN_DEFAULT = 1;
    s->PLC_WEIGHT_PKTLOSS_DEFAULT = 16384;
    s->PLC_MIN_FRAME_EN = 1;     //1;                                                                                        // 1: divide to mini frame for GFSK: 30, EDR 2M: 60, EDR 3M: 90
    s->PLC_MIN_FRAME_BER_METHOD = 0;                                                              // 2;   % 0: CRC, 1: near SNR, 2: worst SNR, 3: mean SNR using last no SNR period frame
    s->PLC_SMART_EN = 1; //1;                                                                                                        // 1: adaptive plc type & gain control parameter
    s->PLC_BER_GFRAME_EN = 1;                                                                                   // 1: include BER to good frame for bad frame
    s->PLC_CRC_GFRAME_EN = 1;                                                                                  // 1: CRC to good frame for eSCO
    s->PLC_BER_GFRAME_TH = 1;                                                                                   // when plc_ber_gframe_en set, this value to index good/bad frame, fix point
    s->PLC_HEC_GFRAME_EN =1;                                                                                   // HEC error, but CVSD still decoder, change to good frame by SNR or CRC
    s->PLC_TONE_SBER_EN = 1;                                                                                       // when smart, single tone will enable strict BER condition
    s->FRAME_AMP_TH = 0x0020;               // voice too small, fixed point
    s->FRAME_UNVOICED_TH = 0x02cc;                       // unvoice zero corss number threshold > 3.5k=26/30(frame), fixed point from parameter
    s->ZC_IIR_DIFF_TH = 0x0016;                                     // zero corss number & iir difference number threshold, fixed point from parameter
    s->ZC_DIFF_TH = 34;              // for continue similar zc number, fixed point from parameter
    s->ZC_DIFF_NUM_TH = 1;                                                           // for torelence continue similar zc number, fixed point from parameter
    s->FBER_MIIR_TH = 0x1999;            // middle average threshold, fixed point from parameter
    s->FBER_SLIIR_TH = 0x051e;           // long & short average difference threshold, fixed point from parameter
    s->FBER_MLIIR_TH = 0x051e;           // long & middle average difference threshold, fixed point from parameter
    s->FS_16K = 1;                                                                                                   // 0: CVSD, 1: mSBC
}


void plcpitch_para_msbc_init(PLC_state *s)
{
    s->FS_16K = 1;                                                                                                   // 0: CVSD, 1: mSBC
}

#endif


/**
 * stream_function_plc_initialize
 *
 * This function is used to init memory space for voice PLC
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
bool stream_function_plc_initialize (void *para)
{
    PlcCtrlPtr = stream_function_get_working_buffer(para);
    #if 0
    Voice_PLC_CollectPlcInfo(gPlcCtrl.PktInfo.PktSize,
                             gPlcCtrl.PktInfo.PktType,
                             gPlcCtrl.PktInfo.CodecType);
    #else
    Voice_PLC_CollectPlcInfo(60,
                             0,
                             mSBC);  // 155x only support packet type 2EV3
    #endif


    Voice_PLC_PktType2PLCInfo();
    Voice_PLC_CalculateFrameSize();

    if (PlcCtrlPtr->CodecType == mSBC)
    {
        #if (PLC_USE_FAKE_PARA)
        plcpitch_sim_msbc_init(&PlcCtrlPtr->PLC_INSTANCE);
        nvkey_read_full_key(NVKEY_DSP_PARA_PLC,
                          &PlcCtrlPtr->PLC_INSTANCE.plc_paras,
                          sizeof(DSP_PARA_PLC_STRU));
        #if 1
        PlcCtrlPtr->PLC_INSTANCE.PLC_MUTE_CONSECUTIVE_ERRS_DEFAULT               = 8;
        PlcCtrlPtr->PLC_INSTANCE.PLC_UNITY_GAIN_CONSECUTIVE_GOODFRM_DEFAULT      = 14;
        PlcCtrlPtr->PLC_INSTANCE.PLC_MUTE_CONSECUTIVE_ERRS_DEFAULT               = 10;
        PlcCtrlPtr->PLC_INSTANCE.PLC_UNITY_GAIN_CONSECUTIVE_GOODFRM_DEFAULT      = 15;
        PlcCtrlPtr->PLC_INSTANCE.PLC_PE16_ADJ_DEFAULT                            = 0;
        PlcCtrlPtr->PLC_INSTANCE.PLC_MUTE_GAIN_RATE_FACTOR_ATTACK_DEFAULT        = 10;
        PlcCtrlPtr->PLC_INSTANCE.PLC_MUTE_GAIN_RATE_FACTOR_RELEASE_DEFAULT       = 10;
        PlcCtrlPtr->PLC_INSTANCE.PLC_WEIGHT_PKTLOSS_DEFAULT                      = 16384;
        PlcCtrlPtr->PLC_INSTANCE.PLC_WEIGHT_PKTLOSS_EN_DEFAULT                   = 1;
        PlcCtrlPtr->PLC_INSTANCE.PLC_MIN_FRAME_EN                                = 1;
        PlcCtrlPtr->PLC_INSTANCE.PLC_MIN_FRAME_BER_METHOD                        = 0;
        PlcCtrlPtr->PLC_INSTANCE.PLC_SMART_EN                                    = 1;
        PlcCtrlPtr->PLC_INSTANCE.PLC_BER_GFRAME_EN                               = 1;
        PlcCtrlPtr->PLC_INSTANCE.PLC_CRC_GFRAME_EN                               = 1;
        PlcCtrlPtr->PLC_INSTANCE.PLC_BER_GFRAME_TH                               = 1;
        PlcCtrlPtr->PLC_INSTANCE.PLC_HEC_GFRAME_EN                               = 1;
        PlcCtrlPtr->PLC_INSTANCE.PLC_TONE_SBER_EN                                = 1;
        PlcCtrlPtr->PLC_INSTANCE.FS_16K                                          = 1;
        PlcCtrlPtr->PLC_INSTANCE.FRAME_AMP_TH                                    = 32;
        PlcCtrlPtr->PLC_INSTANCE.FRAME_UNVOICED_TH                               = 716;
        PlcCtrlPtr->PLC_INSTANCE.ZC_IIR_DIFF_TH                                  = 22;
        PlcCtrlPtr->PLC_INSTANCE.ZC_DIFF_TH                                      = 34;
        PlcCtrlPtr->PLC_INSTANCE.ZC_DIFF_NUM_TH                                  = 1;
        PlcCtrlPtr->PLC_INSTANCE.FBER_MIIR_TH                                    = 6553;
        PlcCtrlPtr->PLC_INSTANCE.FBER_SLIIR_TH                                   = 1310;
        PlcCtrlPtr->PLC_INSTANCE.FBER_MLIIR_TH                                   = 1310;
        #endif

        plcpitch_para_init(&PlcCtrlPtr->PLC_INSTANCE);
        #endif
        PlcCtrlPtr->PLC_INSTANCE.FS_16K = 1;
    }
    else if (PlcCtrlPtr->CodecType == CVSD)
    {
        #if (PLC_USE_FAKE_PARA)
        //plcpitch_sim_init(&PlcCtrlPtr->PLC_INSTANCE);
        nvkey_read_full_key(NVKEY_DSP_PARA_PLC,
                          &PlcCtrlPtr->PLC_INSTANCE.plc_paras,
                          sizeof(DSP_PARA_PLC_STRU));
        plcpitch_para_init(&PlcCtrlPtr->PLC_INSTANCE);
        #endif
        PlcCtrlPtr->PLC_INSTANCE.FS_16K = 0;
    }

    SMART_PITCH_PLC_INI(&PlcCtrlPtr->PLC_INSTANCE, PlcCtrlPtr->ModemDataType);


    return FALSE;
}


/**
 * stream_function_plc_process
 *
 * This function handles main process for voice PLC
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_plc_process (void *para)
{
    U16 PLC_BufOffset,i;
    U16 PLC_FrameSize;
    VOICE_RX_PKT_STRU_PTR_t pktInfoPtr;

    S16* InBuf = stream_function_get_1st_inout_buffer(para);
    U16 Process_Times = stream_function_get_output_size(para)/(PlcCtrlPtr->PktSize*2);
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)InBuf, (U32)stream_function_get_output_size(para), VOICE_RX_PLC_IN);
#endif

    #if (PLC_USE_FAKE_PARA)
    pktInfoPtr = &PlcFakeInfo;
    #else
    pktInfoPtr = &((DSP_ENTRY_PARA_PTR)para)->pkt_info.PlcInfo;
    #endif
    Process_Times = (PlcCtrlPtr->CodecType == mSBC)
        ? stream_function_get_output_size(para)/(PlcCtrlPtr->PktSize*4)
           : stream_function_get_output_size(para)/(PlcCtrlPtr->PktSize*2);

    for (i = 0 ; i < Process_Times ; i++)
    {
        switch (PlcCtrlPtr->CodecType)
        {
            case CVSD:

                if ((PlcCtrlPtr->PLC_INSTANCE.plc_paras.ENABLE)
                 && (PlcCtrlPtr->PktSize >= MIN_PLC_PKT_SIZE))
                {
                    PLC_BufOffset = PlcCtrlPtr->PLC_INSTANCE.plc_paras.PLC_OFFSET;
                }
                else
                {
                    PLC_BufOffset = 0;
                }

                PLC_FrameSize = PlcCtrlPtr->PktSize;
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
                LOG_AUDIO_DUMP((U8*)(&PLC_FrameSize), (U32)sizeof(U16), _RESERVED);
#endif
                memcpy(&PlcCtrlPtr->PLC_BUF[PLC_BufOffset],
                       &InBuf[i*PLC_FrameSize],
                       PLC_FrameSize*2);
                if (PlcCtrlPtr->PLC_INSTANCE.plc_paras.ENABLE)
                {
                    Voice_PLC_PacketBasedProcess(&(pktInfoPtr->InbandInfo[i]));
                }

                memcpy(&InBuf[i*PLC_FrameSize],
                       &PlcCtrlPtr->PLC_BUF[0],
                       PLC_FrameSize*2);

                memcpy(&PlcCtrlPtr->PLC_BUF[0],
                       &PlcCtrlPtr->PLC_BUF[PLC_FrameSize],
                       PLC_BufOffset*2);

                break;

            case mSBC:
                if (PlcCtrlPtr->PLC_INSTANCE.plc_paras.ENABLE)
                {
                    PLC_BufOffset = PlcCtrlPtr->PLC_INSTANCE.plc_paras.PLC_OFFSET_mSBC;
                }
                else
                {
                    PLC_BufOffset = 0;
                }
                if (PLC_BufOffset != 0x1c)
                    {
                        DSP_MW_LOG_I("plc offset :%d", 1,PLC_BufOffset);
                    }
                PLC_FrameSize = MSBC_DECODED_FRAME_SIZE;
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
                LOG_AUDIO_DUMP((U8*)(&PLC_FrameSize), (U32)sizeof(U16), _RESERVED);
#endif
                memcpy(&PlcCtrlPtr->PLC_BUF[PLC_BufOffset],
                       &InBuf[i*PLC_FrameSize],
                       PLC_FrameSize*2);

                if (PlcCtrlPtr->PLC_INSTANCE.plc_paras.ENABLE)
                {
                    Voice_PLC_PacketBasedProcess(&(pktInfoPtr->InbandInfo[i]));
                }

                memcpy(&InBuf[i*PLC_FrameSize],
                       &PlcCtrlPtr->PLC_BUF[0],
                       PlcCtrlPtr->ActualFrameSize*2);

                memcpy(&PlcCtrlPtr->PLC_BUF[0],
                       &PlcCtrlPtr->PLC_BUF[PLC_FrameSize],
                       PLC_BufOffset*2);

                break;

            default:
                return TRUE;
        }
    }
    for (i = 1 ; i < stream_function_get_device_channel_number(para) ; i++)
    {
        memcpy(stream_function_get_inout_buffer(para, i+1),
               stream_function_get_inout_buffer(para, 1),
               stream_function_get_output_size(para));
    }
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)InBuf, (U32)stream_function_get_output_size(para), VOICE_RX_PLC_OUT);
#endif

    return FALSE;
}




/**
 * Voice_PLC_PacketBasedProcess
 *
 * This function handles main process for frame based voice PLC
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 VOID Voice_PLC_PacketBasedProcess (VOICE_RX_INBAND_INFO_t* para)
{
    //DSP_MW_LOG_I("[PLC] RxEd:%d, HEC:%d, CRC:%d, ", 3, para->RxEd, para->HecErr, para->CrcErr);
    //U8 InfoBuf[240];
    U16 PKT_Status          = (U16)(para->RxEd)^1;
#ifndef MTK_BT_HFP_FORWARDER_ENABLE
    U16 HEC_Status          = 0;
    U16 CRC_Status          = 0;
#else
    U16 HEC_Status          = (U16)(para->HecErr);
    U16 CRC_Status          = (U16)(para->CrcErr);
#endif
    U16 SNR_Report;
    //U16 PLC_process_counter = PlcCtrlPtr->PktSize/PlcCtrlPtr->SnrReportFrameSize;
    U16 PLC_DUMP_BUF[11];

    //LOG_AUDIO_DUMP(&InfoBuf[0], 240, VOICE_RX_PLC_INFO);
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)&PKT_Status, (U32)sizeof(U8), VOICE_RX_PLC_INFO);
#endif

    if (PKT_Status == 1)
    {
        SNR_Report = 0;
        HEC_Status = 1;
        CRC_Status = 1;
    }
    else if (HEC_Status == 1)
    {
        CRC_Status = 1;
    }

    SNR_Report = 0;

    PlcCtrlPtr->ActualFrameSize = MSBC_DECODED_FRAME_SIZE;

    /* Temporarily use for legacy dump parser */
        PLC_DUMP_BUF[0]  = 0xACE0;
        PLC_DUMP_BUF[1]  = PlcCtrlPtr->ActualFrameSize + 9;
        PLC_DUMP_BUF[2]  = PlcCtrlPtr->ActualFrameSize;
        PLC_DUMP_BUF[3]  = (PKT_Status|HEC_Status);
        PLC_DUMP_BUF[4]  = (PlcCtrlPtr->ModemDataType);
        PLC_DUMP_BUF[5]  = (PlcCtrlPtr->ScoType);
        PLC_DUMP_BUF[6]  = (CRC_Status);
        PLC_DUMP_BUF[7]  = (SNR_Report);
        PLC_DUMP_BUF[8]  = (0); // PLC_Ind_Read
        PLC_DUMP_BUF[9]  = (PKT_Status);
        PLC_DUMP_BUF[10] = (0); //Retransmit_Status

    /*for(int i = 0; i < 240; i++)
    {
       InfoBuf[i] = (1 - (PKT_Status|HEC_Status|CRC_Status));
    }*/

    //LOG_AUDIO_DUMP(&InfoBuf[0], 240, VOICE_RX_PLC_INFO);
    //LOG_AUDIO_DUMP(&PlcCtrlPtr->PLC_BUF, 240, VOICE_RX_PLC_IN);
    SMART_PITCH_PLC_PROCESS(PlcCtrlPtr->PLC_BUF,
                            (1 - (PKT_Status|HEC_Status|CRC_Status)),
                            CRC_Status,
                            SNR_Report,
                            PlcCtrlPtr->ModemDataType,
                            PlcCtrlPtr->ScoType,
                            PlcCtrlPtr->ActualFrameSize,
                            &PlcCtrlPtr->PLC_INSTANCE);
    //LOG_AUDIO_DUMP(&PlcCtrlPtr->PLC_BUF, 240, VOICE_RX_PLC_OUT);


}


/**
 * Voice_PLC_CollectPlcInfo
 *
 * This function collect PLC informations
 *
 *
 *
 */
VOID Voice_PLC_CollectPlcInfo(U16 PktSize, U16 PktType, U16 CodecType)
{
    PlcCtrlPtr->PktSize     = PktSize;
    PlcCtrlPtr->PktType     = PktType;
    PlcCtrlPtr->CodecType     = CodecType;
}


/**
 * Voice_PLC_PktType2PLCInfo
 *
 * This function transform packet type to PLC parameters
 *
 *
 *
 */
VOID Voice_PLC_PktType2PLCInfo(VOID)
{
    #if 1 // only support 2EV3 codec type
    PlcCtrlPtr->SnrReportFrameSize = 60;
    PlcCtrlPtr->ModemDataType = PLC_EDR2M;
    PlcCtrlPtr->ScoType = eSCO;
    #else

    switch (PlcCtrlPtr->PktType)
    {
        case BT_PKT_TYPE_HV1:
            PlcCtrlPtr->SnrReportFrameSize = 10;
            PlcCtrlPtr->ModemDataType = PLC_GFSK;
            PlcCtrlPtr->ScoType = SCO;
            break;

        case BT_PKT_TYPE_HV2:
            PlcCtrlPtr->SnrReportFrameSize = 20;
            PlcCtrlPtr->ModemDataType = PLC_GFSK;
            PlcCtrlPtr->ScoType = SCO;
            break;

        case BT_PKT_TYPE_HV3:
            PlcCtrlPtr->SnrReportFrameSize = 30;
            PlcCtrlPtr->ModemDataType = PLC_GFSK;
            PlcCtrlPtr->ScoType = SCO;
            break;

        case BT_PKT_TYPE_EV3:
            PlcCtrlPtr->SnrReportFrameSize = 30;
            PlcCtrlPtr->ModemDataType = PLC_GFSK;
            PlcCtrlPtr->ScoType = eSCO;
            break;

        case BT_PKT_TYPE_EV4:
            PlcCtrlPtr->SnrReportFrameSize = 20;
            PlcCtrlPtr->ModemDataType = PLC_GFSK;
            PlcCtrlPtr->ScoType = eSCO;
            break;

        case BT_PKT_TYPE_EV5:
            PlcCtrlPtr->SnrReportFrameSize = 30;
            PlcCtrlPtr->ModemDataType = PLC_GFSK;
            PlcCtrlPtr->ScoType = eSCO;
            break;

        case BT_PKT_TYPE_2EV3:
            PlcCtrlPtr->SnrReportFrameSize = 60;
            PlcCtrlPtr->ModemDataType = PLC_EDR2M;
            PlcCtrlPtr->ScoType = eSCO;
            break;

        case BT_PKT_TYPE_2EV5:
            PlcCtrlPtr->SnrReportFrameSize = 60;
            PlcCtrlPtr->ModemDataType = PLC_EDR2M;
            PlcCtrlPtr->ScoType = eSCO;
            break;

        case BT_PKT_TYPE_3EV3:
            PlcCtrlPtr->SnrReportFrameSize = 90;
            PlcCtrlPtr->ModemDataType = PLC_EDR3M;
            PlcCtrlPtr->ScoType = eSCO;
            break;

        case BT_PKT_TYPE_3EV5:
            PlcCtrlPtr->SnrReportFrameSize = 90;
            PlcCtrlPtr->ModemDataType = PLC_EDR3M;
            PlcCtrlPtr->ScoType = eSCO;
            break;

        default:
            OS_ASSERT(FALSE);
    }
    #endif
}


/**
 * Voice_PLC_CalculateFrameSize
 *
 * This function extract actual initial frame size used by PLC
 *
 *
 *
 */
VOID Voice_PLC_CalculateFrameSize(VOID)
{
    if (PlcCtrlPtr->CodecType == CVSD)
    {
        PlcCtrlPtr->ActualFrameSize = PlcCtrlPtr->SnrReportFrameSize;
    }
    else if (PlcCtrlPtr->CodecType == mSBC)
    {
        PlcCtrlPtr->ActualFrameSize = MSBC_DECODED_FRAME_SIZE;
    }
}


/**
 * Voice_PLC_UpdatePacketInfo
 *
 * This function updates packet information to PLC
 *
 *
 *
 */
VOID Voice_PLC_UpdatePacketInfo(U16 CodecType, U16 PktSize, U16 PktType)
{
    gPlcCtrl.PktInfo.PktSize = PktSize;
    gPlcCtrl.PktInfo.PktType = PktType;
    gPlcCtrl.PktInfo.CodecType = CodecType;
}










#endif
/**
 * Voice_PLC_CheckAndFillZeroResponse
 *
 * This function fills zero mSBC response if needed
 *
 *
 *
 */
VOID Voice_PLC_CheckAndFillZeroResponse(S16* DataPtr,U8 codec_type)
{
    if (codec_type == mSBC)
    {
        #ifdef MTK_BT_A2DP_SBC_ENABLE
        memcpy(DataPtr, MSBC_ZERO_RESPONSE, 60);
        #endif
    }
    else
    {
        #ifdef MTK_BT_HFP_FORWARDER_ENABLE
        memset(DataPtr, 0 , 120); //CVSD zero response
        #else
        memset(DataPtr, 0x55 , 60); //CVSD zero response
        #endif
    }
}

/**
 * Voice_PLC_UpdateInbandInfo
 *
 * This function updates inband information to PLC
 *
 *
 *
 */
VOID Voice_PLC_UpdateInbandInfo(VOICE_RX_INBAND_INFO_t* Ptr, U32 Lengh, U8 PktIdx)
{
    memcpy(&(PlcFakeInfo.InbandInfo[PktIdx]), Ptr, Lengh);
}

/**
 * Voice_PLC_CheckInfo
 *
 *
 * @return TRUE if the packet is valid, FALSE otherwise.
 */
BOOL Voice_PLC_CheckInfoValid(VOICE_RX_SINGLE_PKT_STRU_PTR_t InfoPtr)
{
    U16 PKT_Status              = (U16)(InfoPtr->InbandInfo.RxEd);
    U16 HEC_Status              = (U16)(InfoPtr->InbandInfo.HecErr)^1;
    U16 CRC_Status              = (U16)(InfoPtr->InbandInfo.CrcErr)^1;

    //if((PKT_Status && HEC_Status && CRC_Status)==FALSE)
        //DSP_MW_LOG_I("[Voice_PLC_CheckInfoValid] info_ptr RxEd:%d, CRC:%d, HEC:%d, %d", 4, PKT_Status, HEC_Status, CRC_Status, PKT_Status && HEC_Status && CRC_Status);

    return (PKT_Status && HEC_Status && CRC_Status);
}

/**
 * Voice_PLC_CleanInfo
 *
 * This function is used to clean up inband information
 *
 *
 */
VOID Voice_PLC_CleanInfo(VOICE_RX_SINGLE_PKT_STRU_PTR_t info)
{
    info->InbandInfo.RxEd = 0;
    info->InbandInfo.HecErr = 1;
    info->InbandInfo.CrcErr = 1;
}

