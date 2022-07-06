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

#include "config.h"

#include "sfr_au_dfe.h"
#include "dsp_drv_dfe.h"
#include "stream_audio_setting.h"
#include "stream_audio_driver.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "hal_clock_internal.h"

/******************************************************************************
 * Function Declaration
 ******************************************************************************/
AU_DFE_UPDOWN_RATIO DSP_UpDownRate2HwDfe(stream_feature_convert_samplerate_t rate);
VOID DSP_AD_IN_INIT(VOID);
VOID DSP_AD_IN_END(VOID);
VOID DSP_DA_OUT_INIT(U8 Channel);
VOID DSP_DA_OUT_RST(U8 Channel);
VOID DSP_DA_OUT_EN(U8 Channel);
VOID DSP_DA_OUT_END(VOID);


/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/


/**
 * DSP_DRV_DFE
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_DRV_DFE(VOID)
{
/*     return; */
}


/**

 *DSP_DRV_iDFE_DEC3_INIT
 *
 *  DFE_INPUT_SEL : Input selection  0:codec, 1:I2S MS, 2:I2S SL, 3:SPDIF, 4: mini DSP
 *  Fs_in              : Input sampling rate
 *  Fs_out            : Output sampling rate
 *  Channelnum    : Total channel used (only useful if input is AD)
 *  BitRes            : Bit resolution (Shall be 16 bit based or 24 bit based)
 * @Author : Machi <MachiWu@airoha.com.tw>
 */


VOID DSP_DRV_iDFE_DEC3_INIT(stream_feature_convert_samplerate_t DWN_RATIO ,U32 ChannelNum, stream_resolution_t BitRes)
{

/*     DFE_CTL1_t AudioDfeReset;
    DFE_CTL0_t AudioDfeEN;

    AudioDfeReset.reg = AUDIO_DFE.CTL1.reg;
    AudioDfeReset.field.RST_AU_IDFE_DEC3 = 1;
    AudioDfeReset.field.RST_AU_IDFE_INTF = 1;
    AudioDfeReset.field.SET_AU_IDFE_DEC3_ACT_CH = (ChannelNum-1);
    AUDIO_DFE.CTL1.reg = AudioDfeReset.reg;

    AUDIO_DFE.IN_SET0.field.DEC3_BIT_RES = BitRes;
    AUDIO_DFE.IN_SET0.field.DEC3_DWN_RATIO = DSP_UpDownRate2HwDfe(DWN_RATIO);

    AudioDfeEN.reg = AUDIO_DFE.CTL0.reg;
    AudioDfeEN.field.EN_AU_IDFE_DEC3 = 1;
    AudioDfeEN.field.EN_AU_IDFE_INTF = 1;
    AudioDfeEN.field.SET_AU_IDFE_DEC3_ACT_CH = (ChannelNum-1);
    AUDIO_DFE.CTL0.reg = AudioDfeEN.reg;
    return; */
    UNUSED(DWN_RATIO);
    UNUSED(ChannelNum);
    UNUSED(BitRes);
}
VOID DSP_DRV_iDFE_DEC3_END(VOID)
{
/*     AUDIO_DFE.CTL1.field.RST_AU_IDFE_DEC3 = 1;
    AUDIO_DFE.CTL0.field.EN_AU_IDFE_DEC3 = 0; */
}


AU_IDFE_WADMA_PTR DSP_DRV_WADMA_CH_GET (ADMA_CH_NUM_s Channel_sel)
{
/*     AU_IDFE_WADMA_PTR Channel_ctrl=NULL;

        switch (Channel_sel)
        {
            case ADMA_CH0:
            case ADMA_CH1:
            case ADMA_CH2:
            case ADMA_CH3:
            case ADMA_CH4:
                Channel_ctrl = (AU_IDFE_WADMA_PTR)(((U8*)&AU_IDFE_CH0_WADMA) + (Channel_sel<<8));
                break;
            case SRCA_CH0:
                Channel_ctrl = (AU_IDFE_WADMA_PTR)((U8*)&AU_SRC_A_CH0_WADMA);
                break;
            case SRCA_CH1:
                Channel_ctrl = (AU_IDFE_WADMA_PTR)((U8*)&AU_SRC_A_CH1_WADMA);
                break;
            case SRCB_CH0:
                Channel_ctrl = (AU_IDFE_WADMA_PTR)((U8*)&AU_SRC_B_CH0_WADMA);
                break;
            case SRCB_CH1:
                Channel_ctrl = (AU_IDFE_WADMA_PTR)((U8*)&AU_SRC_B_CH1_WADMA);
                break;
            default:
                //configASSERT(0);//Unknown Channel sel;
                break;
        }
    return Channel_ctrl; */
    UNUSED(Channel_sel);
    return 0;
}



/**

 *DSP_DRV_WADMA_INIT
 *
 *  Channel_sel    :  0~4 channel (channel4 is reserved for  AEC path)
 *  Buf_addr         :  Buffer address
 *  Buf_size          : Buffer size
 *  THD_size         : Threshold of interrupt triggered
 * @Author : Machi <MachiWu@airoha.com.tw>
 */




VOID DSP_DRV_WADMA_INIT(ADMA_CH_NUM_s Channel_sel,U8* Buf_addr, U32 Buf_size, U32 THD_size)
{
/*     AU_IDFE_WADMA_PTR Channel_ctrl;

    if ((Channel_ctrl = DSP_DRV_WADMA_CH_GET (Channel_sel)))
    {
        (*Channel_ctrl).CTL.field.ENABLE = 0;
        (*Channel_ctrl).CTL.field.DATA_DIR = 0;
        (*Channel_ctrl).SET.field.BUF_SIZE = Buf_size>>2;
        (*Channel_ctrl).SET.field.THD_SIZE = THD_size>>2;
        (*Channel_ctrl).INIT.field.ADDR = (U32)Buf_addr;
        DSP_DRV_WADMA_CTRL(Channel_sel, TRUE);
    } */
    UNUSED(Channel_sel);
    UNUSED(Buf_addr);
    UNUSED(Buf_size);
    UNUSED(THD_size);
}

VOID DSP_DRV_WADMA_END(U32 Channel_sel)
{
/*     AU_IDFE_WADMA_s* Channel_ctrl;// = (AU_IDFE_WADMA_s*)(((U8*)&AU_IDFE_CH0_WADMA) + (Channel_sel<<8));

    if ((Channel_ctrl = DSP_DRV_WADMA_CH_GET (Channel_sel)))
    {
        (*Channel_ctrl).CTL.field.SW_RESET = 1;
        (*Channel_ctrl).CTL.field.THD_INTR_MASK = 1;
        (*Channel_ctrl).CTL.field.ERR_INTR_MASK = 1;
        (*Channel_ctrl).CTL.field.ENABLE = 0;
    } */
    UNUSED(Channel_sel);
}

VOID DSP_DRV_WADMA_CTRL(ADMA_CH_NUM_s Channel_sel, BOOL IsEnabled)
{
/*     AU_IDFE_WADMA_s* Channel_ctrl;// = (AU_IDFE_WADMA_s*)(((U8*)&AU_IDFE_CH0_WADMA) + (Channel_sel<<8));

    if ((Channel_ctrl = DSP_DRV_WADMA_CH_GET (Channel_sel)))
    {
        (*Channel_ctrl).CTL.field.ERR_INTR_CLR = 1;
        (*Channel_ctrl).CTL.field.THD_INTR_CLR = 1;

        (*Channel_ctrl).CTL.field.SW_RESET = 1;
        (*Channel_ctrl).CTL.field.FLUSH_DATA = 0;

        (*Channel_ctrl).CTL.field.THD_INTR_MASK = IsEnabled^1;
        (*Channel_ctrl).CTL.field.ERR_INTR_MASK = 1;
        (*Channel_ctrl).CTL.field.ENABLE = IsEnabled;
    } */
    UNUSED(Channel_sel);
    UNUSED(IsEnabled);
}

VOID DSP_DRV_SOURCE_WADMA(SOURCE source, BOOL IsEnabled)
{

/*     audio_channel audio_ch;
    ADMA_CH_NUM_s ch0;
    audio_ch = (audio_channel)(source->param.audio.channel_sel);
    ch0 = (audio_ch==AUDIO_CHANNEL_B) ? ADMA_CH1 : ADMA_CH0;

    switch (audio_ch)
    {
        case AUDIO_CHANNEL_A:
        case AUDIO_CHANNEL_B:
            DSP_DRV_WADMA_CTRL(ch0, IsEnabled);
            break;
        case AUDIO_CHANNEL_A_AND_B:
            DSP_DRV_WADMA_CTRL(ADMA_CH0, IsEnabled);
            DSP_DRV_WADMA_CTRL(ADMA_CH1, IsEnabled);

            break;
        default:
            break;
    } */
    UNUSED(source);
    UNUSED(IsEnabled);
}

/**

 *DSP_DRV_oDFE_INT4_INIT
 *
 *  DFE_OUTPUT_ID   : Output selection 5:codec, 6:I2S MS, 7:I2S SL, 8:SPDIF, 9: mini DSP
 *  Fs_in                   : Input sampling rate
 *  Fs_out                 : Output sampling rate
 *  Channelnum         : Total channel used (only useful if input is AD)
 *  SRC_ADMA_SEL     : Data MUX of INT4. 1:from ADMA, 0:from AU_SRC
 *  Cic_bypass           : 0: not to bypass CIC, 1: bypass CIC
 * @Author : Machi <MachiWu@airoha.com.tw>
 */




VOID DSP_DRV_oDFE_INT4_INIT(stream_feature_convert_samplerate_t UPS_RATIO, U32 ChannelNum, SRC_ADMA_enum_s SRC_ADMA_SEL, CIC_SEL_enum_s CIC_bypass , stream_resolution_t BitRes)
{
/*     DSP_DRV_oDFE_INT4_RST(ChannelNum);

    AUDIO_DFE.OUT_SET0.field.INT4_IN_DATA_MUX    = SRC_ADMA_SEL&1;
    AUDIO_DFE.OUT_SET0.field.BYPASS_INT4_CC1_FIL  = CIC_bypass&1;
    AUDIO_DFE.OUT_SET0.field.INT4_BIT_RES = BitRes;
    AUDIO_DFE.OUT_SET0.field.INT4_UPS_RATIO = DSP_UpDownRate2HwDfe(UPS_RATIO);

    DSP_DRV_oDFE_INT4_EN(ChannelNum); */
    UNUSED(UPS_RATIO);
    UNUSED(ChannelNum);
    UNUSED(SRC_ADMA_SEL);
    UNUSED(CIC_bypass);
    UNUSED(BitRes);
}
VOID DSP_DRV_oDFE_INT4_RST(U32 ChannelNum)
{
/*     DFE_CTL1_t AudioDfeReset;

    DSP_DRV_EnableAudioDfeClock();
    AudioDfeReset.reg = AUDIO_DFE.CTL1.reg;
    AudioDfeReset.field.RST_AU_ODFE_INT4 = 1;
    AudioDfeReset.field.RST_AU_ODFE_INTF = 1;
    AudioDfeReset.field.SET_AU_ODFE_INT4_ACT_CH = (ChannelNum-1);
    AUDIO_DFE.CTL1.reg = AudioDfeReset.reg; */
    UNUSED(ChannelNum);
}


VOID DSP_DRV_oDFE_INT4_EN(U32 ChannelNum)
{
/*     DFE_CTL0_t AudioDfeEN;
    AudioDfeEN.reg = AUDIO_DFE.CTL0.reg;
    AudioDfeEN.field.EN_AU_ODFE_INT4 = 1;
    AudioDfeEN.field.EN_AU_ODFE_INTF = 1;
    AudioDfeEN.field.SET_AU_ODFE_INT4_ACT_CH = (ChannelNum-1);
    AUDIO_DFE.CTL0.reg = AudioDfeEN.reg; */
    UNUSED(ChannelNum);
}

VOID DSP_DRV_oDFE_INT4_END(VOID)
{
/*     AUDIO_DFE.CTL1.field.RST_AU_ODFE_INT4 = 0;
    AUDIO_DFE.CTL0.field.EN_AU_ODFE_INT4 = 0; */
}


/**

 *DSP_DRV_RADMA_INIT
 *
 *  Channel_sel    :  0 : INT4_0, 1 : INT4_1, 2 : INT6, 3 : DBG_BUFFER_of_Au_oDFE, 4 : DBG_BUFFER_of_Au_oDFE
 *  Buf_addr         :  Buffer address
 *  Buf_size          : Buffer size
 *  THD_size         : Threshold of interrupt triggered
 * @Author : Machi <MachiWu@airoha.com.tw>
 */

AU_ODFE_RADMA_PTR DSP_DRV_RADMA_CH_GET (ADMA_CH_NUM_s Channel_sel)
{
/*     AU_ODFE_RADMA_PTR Channel_ctrl=NULL;
    switch (Channel_sel)
    {
        case ADMA_CH0:
        case ADMA_CH1:
        case ADMA_CH2:
        case ADMA_CH3:
        case ADMA_CH4:
            Channel_ctrl = (AU_ODFE_RADMA_s*)(((U8*)&AU_ODFE_CH0_RADMA) + (Channel_sel<<8));
            break;
        case SRCA_CH0:
            Channel_ctrl = (AU_ODFE_RADMA_s*)((U8*)&AU_SRC_A_CH0_RADMA);
            break;
        case SRCA_CH1:
            Channel_ctrl = (AU_ODFE_RADMA_s*)((U8*)&AU_SRC_A_CH1_RADMA);
            break;
        case SRCB_CH0:
            Channel_ctrl = (AU_ODFE_RADMA_s*)((U8*)&AU_SRC_B_CH0_RADMA);
            break;
        case SRCB_CH1:
            Channel_ctrl = (AU_ODFE_RADMA_s*)((U8*)&AU_SRC_B_CH1_RADMA);
            break;
        default:
            //configASSERT(0);//Unknown Channel sel;
            break;
    }
    return Channel_ctrl; */
    UNUSED(Channel_sel);
    return 0;
}

VOID DSP_DRV_RADMA_INIT (ADMA_CH_NUM_s Channel_sel,U8* Buf_addr, U32 Buf_size, U32 THD_size)//Not enable yet
{
/*     AU_ODFE_RADMA_PTR Channel_ctrl;
    if ((Channel_ctrl = DSP_DRV_RADMA_CH_GET(Channel_sel)))
    {
        (*Channel_ctrl).CTL.field.ENABLE = 0;
        (*Channel_ctrl).CTL.field.DATA_DIR = 1;
        (*Channel_ctrl).SET.field.BUF_SIZE = Buf_size>>2;//DW based
        (*Channel_ctrl).SET.field.THD_SIZE = THD_size>>2;//DW based
        (*Channel_ctrl).INIT.field.ADDR = (U32)Buf_addr;
        DSP_DRV_RADMA_CTRL(Channel_sel, TRUE);
    } */
    UNUSED(Channel_sel);
    UNUSED(Buf_addr);
    UNUSED(Buf_size);
    UNUSED(THD_size);
}

VOID DSP_DRV_RADMA_END(ADMA_CH_NUM_s Channel_sel)
{
/*     AU_ODFE_RADMA_PTR Channel_ctrl;
    if ((Channel_ctrl = DSP_DRV_RADMA_CH_GET(Channel_sel)))
    {
        (*Channel_ctrl).CTL.field.SW_RESET = 1;
        (*Channel_ctrl).CTL.field.THD_INTR_MASK = 1;
        (*Channel_ctrl).CTL.field.ERR_INTR_MASK = 1;
        (*Channel_ctrl).CTL.field.ENABLE = 0;
    } */
    UNUSED(Channel_sel);
}

VOID DSP_DRV_RADMA_CTRL(ADMA_CH_NUM_s Channel_sel, BOOL IsEnabled)
{
/*     AU_ODFE_RADMA_PTR Channel_ctrl;
    if ((Channel_ctrl = DSP_DRV_RADMA_CH_GET(Channel_sel)))
    {
        Channel_ctrl->CTL.field.THD_INTR_CLR = 1;
        Channel_ctrl->CTL.field.ERR_INTR_CLR = 1;
        Channel_ctrl->CTL.field.SW_RESET = 1;
        Channel_ctrl->CTL.field.FLUSH_DATA = 0;

        Channel_ctrl->CTL.field.THD_INTR_MASK = IsEnabled^1;
        Channel_ctrl->CTL.field.ERR_INTR_MASK = 1;
        Channel_ctrl->CTL.field.ENABLE = IsEnabled;
    } */
    UNUSED(Channel_sel);
    UNUSED(IsEnabled);
}


VOID DSP_DRV_SINK_RADMA(SINK sink, BOOL IsEnabled)
{
/*     audio_channel audio_ch;
    ADMA_CH_NUM_s CH0, CH1;

    audio_ch = (audio_channel)(sink->param.audio.channel_sel);
    CH0 = (Audio_setting->Audio_sink.SRC_Out_Enable) ? SRCA_CH0 : ADMA_CH0;
    CH1 = (Audio_setting->Audio_sink.SRC_Out_Enable) ? SRCA_CH1 : ADMA_CH1;

    switch (audio_ch)
    {
        case AUDIO_CHANNEL_A:
            DSP_DRV_RADMA_CTRL(CH0, IsEnabled);
            break;
        case AUDIO_CHANNEL_A_AND_B:
            DSP_DRV_RADMA_CTRL(CH0, IsEnabled);
        case AUDIO_CHANNEL_B:
            DSP_DRV_RADMA_CTRL(CH1, IsEnabled);
            break;
        case AUDIO_CHANNEL_VP:
            DSP_DRV_RADMA_CTRL(ADMA_CH2, IsEnabled);
            break;
        default:
            break;
    } */
    UNUSED(sink);
    UNUSED(IsEnabled);
}


/**

 *DSP_DRV_iDFE_DEC9_INIT
 *  Digital front end for AEC path
 *
 *  AEC_SOURCE   :  0 : From Au_DnFilt, 1 : From Au_oDFE, other : Mute
 *  Fs_in              : Input sampling rate  (output rate is fixed as 16) enum :AEC_FS_IN_enum
 * @Author : Machi <MachiWu@airoha.com.tw>
 */



VOID DSP_DRV_iDFE_DEC9_INIT (AEC_SOURCE_SEL_enum_s AEC_Source,AEC_FS_IN_enum_s Fs_in , stream_resolution_t BitRes)
{

/*     AUDIO_DFE.IN_SET0.field.DEC9_BIT_RES = BitRes; //supposed to use 16 bit based

    AUDIO_DFE.CTL1.field.RST_AU_IDFE_DEC9 = 1;
    AUDIO_DFE.IN_SET0.field.EC_PATH_DATA_SEL = AEC_Source;
    AUDIO_DFE.IN_SET0.field.DEC9_DWN_RATIO = Fs_in;
    AUDIO_DFE.CTL0.field.EN_AU_IDFE_DEC9 = 1;
 */
    UNUSED(AEC_Source);
    UNUSED(Fs_in);
    UNUSED(BitRes);
}

VOID DSP_DRV_iDFE_DEC9_END(VOID)
{
/*     AUDIO_DFE.CTL1.field.RST_AU_IDFE_DEC9 = 1;
    AUDIO_DFE.CTL0.field.EN_AU_IDFE_DEC9 = 0; */
}


/**

 *DSP_DRV_SIDETONE_INIT
 *  SFR for Select Sidetone path
 *  Gain_Tune       :  Gain tune dB
 *  TONE_SEL       : 0 : From CH0 of DEC3, 1 : From CH1 of DEC3, 2 : From CH2 of DEC3, 3 : From CH3 of DEC3
 * @Author : Machi <MachiWu@airoha.com.tw>
 */


VOID DSP_DRV_SIDETONE_INIT (U32 Gain_Tune, ADMA_CH_NUM_s TONE_SEL)
{
/*     AUDIO_DFE.CTL2.field.AU_ODFE_SIDETONE_GAIN = Gain_Tune;
    AUDIO_DFE.IN_SET0.field.SIDETONE_SEL = TONE_SEL; */
    UNUSED(Gain_Tune);
    UNUSED(TONE_SEL);
}
VOID DSP_DRV_SIDETONE_END(VOID)
{
/*     AUDIO_DFE.CTL2.field.AU_ODFE_SIDETONE_GAIN = 0; */
}

/**

 *DSP_DRV_oDFE_INT6_INIT
 *  Output DFE for VP/RT path
 *
 *  DFE_OUTPUT_ID   : Output selection 5:codec, 6:I2S MS, 7:I2S SL, 8:SPDIF, 9: mini DSP
 *  Fs_out                 : Output sampling rate  enum: VPRT_OUT_RATE_enum
 *  CIC_bypass           : Bypass CIC filter
 * @Author : Machi <MachiWu@airoha.com.tw>
 */





 VOID DSP_DRV_oDFE_INT6_INIT(stream_feature_convert_samplerate_t UPS_RATIO, CIC_SEL_enum_s CIC_bypass, stream_resolution_t BitRes)//
{

/*     DFE_CTL1_t AudioDfeReset;
    AudioDfeReset.reg = AUDIO_DFE.CTL1.reg;
    AudioDfeReset.field.RST_AU_ODFE_INT6 = 1;
    AUDIO_DFE.CTL1.field.RST_AU_ODFE_INTF       = 1;
    AUDIO_DFE.CTL1.reg = AudioDfeReset.reg;


    AUDIO_DFE.OUT_SET0.field.BYPASS_INT6_CC1_FIL  = CIC_bypass&1;
    AUDIO_DFE.OUT_SET0.field.INT6_BIT_RES = BitRes;

    AUDIO_DFE.OUT_SET0.field.INT6_UPS_RATIO = DSP_UpDownRate2HwDfe(UPS_RATIO);
    AUDIO_DFE.CTL0.field.EN_AU_ODFE_INTF = 1;//Shall consider DAVT    TODO
    AUDIO_DFE.CTL0.field.EN_AU_ODFE_INT6 = 1; */
    UNUSED(UPS_RATIO);
    UNUSED(CIC_bypass);
    UNUSED(BitRes);
}
VOID DSP_DRV_oDFE_INT6_END(VOID)
{
/*     AUDIO_DFE.CTL1.field.RST_AU_ODFE_INT6 = 0;
    AUDIO_DFE.CTL0.field.EN_AU_ODFE_INT6 = 0; */
}


/**

 *DSP_DRV_SRC_A_INIT
 *  SRC_A init function with DMA setup
 *
 *  Fs_in              : Input sampling rate
 *  Fs_out            : Output sampling rate
 *  Buf1_addr       : SRC input buffer 1
 *  Buf2_addr       : SRC input buffer 2
 *  BUF_size        : Buffer size for SRC input buffer
 *  THD_size        : Threshold size of SRC interrupt interval
 * @Author : Machi <MachiWu@airoha.com.tw>
 */

VOID DSP_DRV_SRC_RATE_SW_DEF (SRC_PTR_s src_ptr)
{
/*     U32 fs_in, fs_out;
    U64 integer, fractional;

    if ((src_ptr->SRC_SET.field.INPUT_SAMPLE_RATE >= SRC_IN_SW_DEF )||
        (src_ptr->SRC_SET.field.OUTPUT_SAMPLE_RATE >= SRC_OUT_SW_DEF))
    {
        fs_in = DSP_SRCInRateChange2Value(src_ptr->SRC_SET.field.INPUT_SAMPLE_RATE);
        fs_out = DSP_SRCOutRateChange2Value(src_ptr->SRC_SET.field.OUTPUT_SAMPLE_RATE);
        integer = (U64)(32*fs_in/fs_out);
        fractional = (U64)((32*fs_in)-(integer*fs_out))*33554432/fs_out;

        src_ptr->SRC_INTE.field.INTEGER = (U32)integer;
        src_ptr->SRC_FRAC.field.FRACTIONAL = (U32)fractional;
    } */
    UNUSED(src_ptr);
}


VOID DSP_DRV_SRC_A_INIT (AU_SRC_FS_IN Fs_in, AU_SRC_FS_OUT Fs_out ,stream_resolution_t Res_in, stream_resolution_t Res_out)
{
    // U32 BitRes;
    // SRC_A.SRC_SET.field.ROOT_CLK                = 1;
    // SRC_A.SRC_SET.field.SRC_GATED_CLK           = 0; /* 0:24M, 1:12M, 2:6M, 3:3M, 4:2.4M, 5:1.5M, 6:0.75M, 7:OFF */

    // SRC_A.SRC_CTL0.field.ENABLE                 = 0; /* SRC A Disable */
    // AU_SRC_A_CH0_RADMA.CTL.field.SW_RESET       = 1;
    // AU_SRC_A_CH1_RADMA.CTL.field.SW_RESET       = 1;
    // SRC_A.SRC_CTL1.field.RESET                  = 1; /* SRC A Reset */

    // /* Mode Config */
    // BitRes = Res_in*2 + Res_out;
    // SRC_A.SRC_SET.field.BIT_RESOLUTION          = BitRes;
    // SRC_A.SRC_SET.field.INPUT_SAMPLE_RATE       = Fs_in;
    // SRC_A.SRC_SET.field.OUTPUT_SAMPLE_RATE      = Fs_out;
    // DSP_DRV_SRC_RATE_SW_DEF((SRC_PTR_s)&SRC_A);
    // SRC_A.SRC_SET.field.DRIVING_MODE            = SRC_CDM;    /* Continuous Driving Mode */
    // SRC_A.SRC_SET.field.CHANNEL_SELECTION       = SRC_TWO_CH; /* SRC 2 Channel */
    // SRC_A.SRC_SET.field.INPUT_INTR_SEL          = 0; /* Input Interrupt Selection as H/W Auto */

    // SRC_A.SRC_CTL0.field.ENABLE                 = 1; /* SRC A Enable */
    UNUSED(Fs_in);
    UNUSED(Fs_out);
    UNUSED(Res_in);
    UNUSED(Res_out);
}

VOID DSP_DRV_SRC_INIT (SRC_PTR_s VOLATILE src_ptr, AU_SRC_DRIVING_MODE mode, AU_SRC_CH ch, AU_SRC_FS_IN Fs_in, AU_SRC_FS_OUT Fs_out ,stream_resolution_t Res_in, stream_resolution_t Res_out)
{
    // U32 BitRes;
     // /*Controlled by SRC_A CSR*/
    // SRC_A.SRC_SET.field.ROOT_CLK                = 1;       /* Enable Root Clock */
    // SRC_A.SRC_SET.field.SRC_GATED_CLK            = 0;       /* 0:24M, 1:12M, 2:6M, 3:3M, 4:2.4M, 5:1.5M, 6:0.75M, 7:OFF */

    // src_ptr->SRC_CTL0.field.ENABLE              = 0;       /* SRC Enable */
    // src_ptr->SRC_CTL1.field.RESET               = 1;       /* SRC  Reset */

    // /* Mode Config */
    // BitRes = Res_in*2 + Res_out;
    // src_ptr->SRC_SET.field.BIT_RESOLUTION       = BitRes;
    // src_ptr->SRC_SET.field.INPUT_SAMPLE_RATE    = Fs_in;
    // src_ptr->SRC_SET.field.OUTPUT_SAMPLE_RATE   = Fs_out;
    // DSP_DRV_SRC_RATE_SW_DEF(src_ptr);
    // src_ptr->SRC_SET.field.DRIVING_MODE         = mode;    /* Driving Mode */
    // src_ptr->SRC_SET.field.CHANNEL_SELECTION    = ch;      /* SRC Channel */
    // src_ptr->SRC_SET.field.INPUT_INTR_SEL       = 0;       /* Input Interrupt Selection as H/W Auto */
    // src_ptr->SRC_CTL0.field.VECTOR_INTR_MASK    = 1;

    //Pre-trigger to stagger offset
    // src_ptr->SRC_CTL0.field.ENABLE              = 1;       /* SRC Enable */
    UNUSED(src_ptr);
    UNUSED(mode);
    UNUSED(ch);
    UNUSED(Fs_in);
    UNUSED(Fs_out);
    UNUSED(Res_in);
    UNUSED(Res_out);
}


VOID DSP_DRV_SRC_A_END(VOID)
{
/*     SRC_A.SRC_CTL0.field.ENABLE                     = 0;
    SRC_A.SRC_CTL1.field.RESET                      = 1; */
}

extern bool hal_audio_src_set_start(afe_src_configuration_t *configuration, hal_audio_memory_sync_selection_t sync_select, hal_audio_control_status_t control);
extern bool hal_audio_src_configuration(afe_src_configuration_t *configuration, hal_audio_control_status_t control);
extern uint8_t hwsrc_sleep_manager_handle;

VOID DSP_DRV_SRC_END(SRC_PTR_s src_ptr)
{
#ifdef MTK_HWSRC_IN_STREAM
        //modify for asrc
        if(src_ptr != NULL){
            if (AFE_GET_REG(ASM_GEN_CONF + AFE_MEM_ASRC_1)&ASM_GEN_CONF_ASRC_BUSY_MASK) {
                DSP_MW_LOG_E("DSP_DRV_SRC_END() error: asrc[%d] is running\r\n", 1, AFE_MEM_ASRC_1);
            }
        //}
        //else{
            if ((AFE_GET_REG(ASM_GEN_CONF + AFE_MEM_ASRC_1)&ASM_GEN_CONF_ASRC_EN_MASK))
            {
                DSP_MW_LOG_I("DSP_DRV_SRC_END\n", 0);
                DSP_MW_LOG_I("Disable ASRC\n", 0);

                afe_src_configuration_t src_configuration;
                src_configuration.id = AFE_SRC_1;
                hal_audio_src_set_start(&src_configuration, HAL_AUDIO_MEMORY_SYNC_NONE, HAL_AUDIO_CONTROL_OFF);
                hal_audio_src_configuration(&src_configuration, HAL_AUDIO_CONTROL_OFF);
                DSP_MW_LOG_I("hal_sleep_manager_unlock_sleep\n", 0);
                hal_sleep_manager_unlock_sleep(hwsrc_sleep_manager_handle);
                hal_sleep_manager_release_sleep_handle(hwsrc_sleep_manager_handle);
            }
        }

        //}

#endif

/*     AU_ODFE_RADMA_s *ch0_radma_ptr, *ch1_radma_ptr;
    AU_IDFE_WADMA_s *ch0_wadma_ptr, *ch1_wadma_ptr;

    if (src_ptr==NULL)
        return;
    src_ptr->SRC_CTL1.field.RESET                   = 1;
    src_ptr->SRC_CTL0.reg                           = 0;
    src_ptr->SRC_SET.reg                            = 0;
    src_ptr->SRC_CMP.reg                            = 0;
    src_ptr->SRC_INTE.reg                           = 0;

    ch0_radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_ptr + 0x0100);
    ch1_radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_ptr + 0x0200);
    ch0_wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_ptr + 0x0300);
    ch1_wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_ptr + 0x0400);

    ch0_radma_ptr->CTL.field.SW_RESET               = 1;
    ch1_radma_ptr->CTL.field.SW_RESET               = 1;
    ch0_radma_ptr->CTL.field.ENABLE                 = 0;
    ch1_radma_ptr->CTL.field.ENABLE                 = 0;
    ch0_wadma_ptr->CTL.field.SW_RESET               = 1;
    ch1_wadma_ptr->CTL.field.SW_RESET               = 1;
    ch0_wadma_ptr->CTL.field.ENABLE                 = 0;
    ch1_wadma_ptr->CTL.field.ENABLE                 = 0;

    ch0_radma_ptr->SET.reg                          = 0;
    ch1_radma_ptr->SET.reg                          = 0;
    ch0_wadma_ptr->SET.reg                          = 0;
    ch1_wadma_ptr->SET.reg                          = 0;
    ch0_radma_ptr->INIT.reg                         = 0;
    ch1_radma_ptr->INIT.reg                         = 0;
    ch0_wadma_ptr->INIT.reg                         = 0;
    ch1_wadma_ptr->INIT.reg                         = 0; */
    UNUSED(src_ptr);
}

/**

 *DSP_DRV_I2S_MS_INIT
 * I2S0 init function
 *
 *  Fs_rate                   : I2S sampling rate
 *  DATA_WORD_LEN     : Data word length
 *  TX_WORD_LEN         : TX bit resolution
 *  RX_WORD_LEN         : RX bit resolution
 * @Author : Machi <MachiWu@airoha.com.tw>
 */

VOID DSP_DRV_GROUP_CTL(U8 group_sel, U8 mode)
{
 /*    switch(group_sel)
    {
        case GROUP_SEL_00:
            IOMUX.GRP.field.grp_sel_00 = mode;
            break;
        case GROUP_SEL_01:
            IOMUX.GRP.field.grp_sel_01 = mode;
            break;
        case GROUP_SEL_1:
            IOMUX.GRP.field.grp_sel_1 = mode;
            break;
        case GROUP_SEL_20:
            IOMUX.GRP.field.grp_sel_20 = mode;
            break;
        case GROUP_SEL_21:
            IOMUX.GRP.field.grp_sel_21 = mode;
            break;
        case GROUP_SEL_3:
            IOMUX.GRP.field.grp_sel_3 = mode;
            break;
        case GROUP_SEL_4:
            IOMUX.GRP.field.grp_sel_40 = mode;
            break;
        default:
            break;
    } */
    UNUSED(group_sel);
    UNUSED(mode);
}
VOID DSP_DRV_I2S_MS_INIT(I2S_FS_RATE_enum_s FS_rate,I2S_WORD_LEN_enum_s DATA_WORD_LEN,I2S_WORD_LEN_enum_s TX_WORD_LEN,I2S_WORD_LEN_enum_s RX_WORD_LEN ,AU_I2S_MODE INOUT)
{

/*     SYSTEM.CLKEN.field.audio_mdsp_rst_n  = 1;
    SYSTEM.CLKEN.field.i2s_clk_en  = 1;
    SYSTEM.MCLK.field.mclk_gen_en  = 1;
    SYSTEM.MCLK.field.mclk_int_sel = 1;

    DSP_DRV_GROUP_CTL(Audio_setting->Audio_interface.I2S_Master_Group, GPIO_MUX_I2S_MASTER);

    I2S.CTL1.field.RST_I2S0_MOD = 1;                                // Reset I2S
    // Parameters
    // Rate of MS
    I2S.SET0.field.SET_I2S_M_FS                                           = FS_rate;     //set fs, 0:48k, 1:96k mode
    // WordLength of MS
    I2S.SET0.field.I2S_DATA_WORD_LEN                = DATA_WORD_LEN;     //i2s data word length, 0:16bit 1:24bit 2:32bit 3:32bit
    I2S.SET0.field.AURX_I2S_WORD_LEN                = RX_WORD_LEN;     //i2s -> autx word length, 0:16bit 1:24bit 2:32bit 3:32bit
    I2S.SET0.field.I2S_AUTX_WORD_LEN                = TX_WORD_LEN;     //aurx-> i2s word length,  0:16bit 1:24bit 2:32bit 3:32bit

    // Tx/Rx/TRx of MS
    I2S.SET0.field.I2S_TR_MODE_CTL                                        = INOUT;     //i2s mode. 0:tx mode, 1:rx, 2:tx+rx, 3:disable.
    I2S.SET0.field.I2S_BIT_FORMAT                                         = 2;     //0:RJ, 1:LJ, 2:I2s, 3:disable
    I2S.CTL0.field.EN_I2S0_MOD                                             = 1;     //enable I2S MS*/
    UNUSED(FS_rate);
    UNUSED(DATA_WORD_LEN);
    UNUSED(TX_WORD_LEN);
    UNUSED(RX_WORD_LEN);
    UNUSED(INOUT);
}
VOID DSP_DRV_I2S_MS_END (VOID)
{
/*     I2S.CTL1.field.RST_I2S0_MOD = 1;
    I2S.CTL0.field.EN_I2S0_MOD  = 0; */
}

/**

 *DSP_DRV_I2S_SL_INIT
 * I2S0 init function
 *
 *  Fs_rate                   : I2S sampling rate
 *  DATA_WORD_LEN     : Data word length
 *  TX_WORD_LEN         : TX bit resolution
 *  RX_WORD_LEN         : RX bit resolution
 * @Author : Machi <MachiWu@airoha.com.tw>
 */





VOID DSP_DRV_I2S_SL_INIT(I2S_WORD_LEN_enum_s DATA_WORD_LEN,I2S_WORD_LEN_enum_s TX_WORD_LEN,I2S_WORD_LEN_enum_s RX_WORD_LEN,AU_I2S_MODE INOUT)
{
/*     SYSTEM.CLKEN.field.audio_mdsp_rst_n  = 1;
    SYSTEM.CLKEN.field.i2s_clk_en  = 1;
    SYSTEM.MCLK.field.mclk_gen_en  = 1;
    SYSTEM.MCLK.field.mclk_int_sel = 1;

    DSP_DRV_GROUP_CTL(Audio_setting->Audio_interface.I2S_Slave_Group, GPIO_MUX_I2S_SLAVE);

    I2S.CTL1.field.RST_I2S1_MOD = 1;                                // Reset I2S
    // Parameters
    // WordLength of SL
    I2S.SET1.field.I2S_DATA_WORD_LEN                = DATA_WORD_LEN;     //i2s data word length, 0:16bit 1:24bit 2:32bit 3:32bit
    I2S.SET1.field.AURX_I2S_WORD_LEN                = RX_WORD_LEN;     //i2s -> autx word length, 0:16bit 1:24bit 2:32bit 3:32bit
    I2S.SET1.field.I2S_AUTX_WORD_LEN                = TX_WORD_LEN;     //aurx-> i2s word length,  0:16bit 1:24bit 2:32bit 3:32bit

    // Tx/Rx/TRx of MS
    I2S.SET1.field.I2S_TR_MODE_CTL                  = INOUT;     //i2s mode. 0:tx mode, 1:rx, 2:tx+rx, 3:disable.
    I2S.SET1.field.I2S_BIT_FORMAT                   = 2;         //0:RJ, 1:LJ, 2:I2s, 3:disable
    I2S.CTL0.field.EN_I2S1_MOD                      = 1;         //enable I2S MS */
    UNUSED(DATA_WORD_LEN);
    UNUSED(TX_WORD_LEN);
    UNUSED(RX_WORD_LEN);
    UNUSED(INOUT);
}
VOID DSP_DRV_I2S_SL_END (VOID)
{
    /* I2S.CTL1.field.RST_I2S1_MOD = 1;
    I2S.CTL0.field.EN_I2S1_MOD  = 0; */
}

VOID DSP_DRV_SPDIF_TX_GPIO_CTL(U32 gpio)
{
/*     GROUP_SEL group_sel;
    group_sel = (gpio>=4)
                  ? (gpio-4)/4
                  : 0;

    DSP_DRV_GROUP_CTL(group_sel, GPIO_MUX_BY_SIG_SEL);
    IOMUX.SigSel[gpio].value = SIG_SEL_SPDIF_TXO; */
    UNUSED(gpio);
}

VOID DSP_DRV_SPDIF_TX_INIT(SPDIF_RELATED_enum_s TX_SAMPLE_RATE)
{
/*     #if 1
    SYSTEM.CLKEN.field.spdif_clk_en                 = 1;

    DSP_DRV_SPDIF_TX_GPIO_CTL(Audio_setting->Audio_interface.SPDIF_TX_GPIO);

    #if ASIC_BUILD
    SPDIF.TIMING.field.FAST_CLK_FREQ                = FAST_CLK_FOR_ASIC_144M;//1:144M for ASIC, 3: 72M for FPGA
    #else
    SPDIF.TIMING.field.FAST_CLK_FREQ                = FAST_CLK_FOR_FPGA_72M;
    #endif

    SPDIF.TIMING.field.TX_SAMPLE_RATE               = TX_SAMPLE_RATE;          //Tx Sampling rate; 0:192k, 1:96k, 2:48k, 3:44.1k, 4:32k
    SPDIF.TX_CH_STAT0.reg                           = 0x21000004; //spdif channel status setting (by Taylor)
    SPDIF.TX_CH_STAT1.reg                           = 0x0000000B; //spdif channel status setting (by Taylor)

    SPDIF.TX_CTRL.field.START                       = 1;
    #endif */
    UNUSED(TX_SAMPLE_RATE);
}

VOID DSP_DRV_SPDIF_TX_END(VOID)
{
/*     SPDIF.TX_CTRL.field.START           = 0;
    if (SPDIF.RX_CTRL.field.START==0)
        SYSTEM.CLKEN.field.spdif_clk_en = 0; */
}

VOID DSP_DRV_SPDIF_RX_INIT(VOID)//(SPDIF_RELATED_enum_s PRECISION_OF_DELAT)
{
/*     SYSTEM.CLKSEL.field.dig_pll_src_clk_sel         = 1;
    SYSTEM.CLKEN.field.spdif_clk_en                 = 1;

    IOMUX.DB_UART.field.spdif_rx_sel                = Audio_setting->Audio_interface.SPDIF_RX_GPIO;

    #if ASIC_BUILD
    SPDIF.TIMING.field.FAST_CLK_FREQ                = FAST_CLK_FOR_ASIC_144M;
    #else
    SPDIF.TIMING.field.FAST_CLK_FREQ                = FAST_CLK_FOR_FPGA_72M;
    #endif
    SPDIF.RX_TIMEOUT.field.CYCLE_CNT                = 750;

    SPDIF.RX_ADJ.field.DELTA_PREC                   = PRECISION_OF_DELTA_0125;          //spdif Precision of delta, 0:0.5 1:0.25, 2:0.125 cycle.
    SPDIF.RX_ADJ.field.TOLERANCE_WIDTH              = WIDER_TOLERANCE_05UI_to_35UI;       //spdif accept toggle width, 0:0.5UI~3.5UI, 1:0.25UI~3.75UI

    SPDIF.RX_INTR_MASK.field.SAMPLE_RATE            = 1;
    SPDIF.RX_INTR_MASK.field.SIDE_BAND              = 1;
    SPDIF.RX_INTR_MASK.field.TIME_OUT               = 1;

    SPDIF.RX_CTRL.field.START                       = 1;          //1:start to rx, 0:stop at frame boundary */
}

VOID DSP_DRV_SPDIF_RX_END(VOID)
{
/*     SPDIF.RX_INTR_MASK.field.SAMPLE_RATE= 0;
    SPDIF.RX_CTRL.field.START           = 0;
    if (SPDIF.TX_CTRL.field.START==0)
        SYSTEM.CLKEN.field.spdif_clk_en = 0; */
}



VOID DSP_AD_IN_INIT(VOID)
{
/*
    DSP_DRV_ResetDec1Filter(DEC1_CH0_CH1);
    AUDIO_CODEC.DWN_FIL_SET0.field.CH0_DEC1_IN_SRC_SEL = 0;
    AUDIO_CODEC.DWN_FIL_SET0.field.CH1_DEC1_IN_SRC_SEL = 0;
    DSP_DRV_EnableDec1Filter(DEC1_CH0_CH1);
    */
}


VOID DSP_AD_IN_END(VOID)
{
/*
    DSP_DRV_ResetInt2Filter(INT2_CH0_CH1);
    AUDIO_CODEC.CTL0.field.EN_DEC1_DWN_FIL        = 0;
    */
}


VOID DSP_DA_OUT_INIT(U8 Channel)
{
/*
    DSP_DA_OUT_RST(Channel);

    //AUDIO_CODEC.DWN_FIL_SET0.field.DEC1_FIL_DIG_GAIN = 2; // Doug Debug Dec1 filter gain setting
    #if (FPGA)
    AUDIO_CODEC.UPS_FIL_DSM.field.AU_DAC_TEST_OUT_SEL     = 1;
    #endif


    DSP_DA_OUT_EN(Channel);
    */
    UNUSED(Channel);
}


VOID DSP_DA_OUT_RST(U8 Channel)
{
    //DSP_DRV_ResetInt2Filter(Channel-1);
    UNUSED(Channel);
}


VOID DSP_DA_OUT_EN(U8 Channel)
{
    //DSP_DRV_EnableInt2Filter(Channel-1);
    //AUDIO_CODEC.CTL0.field.EN_AU_DAC_DSM = 1;
    UNUSED(Channel);
}


VOID DSP_DA_OUT_END(VOID)
{
    //DSP_DRV_DisableInt2Filter();
    //AUDIO_CODEC.CTL0.field.EN_AU_DAC_DSM = 0;
}


AU_DFE_UPDOWN_RATIO DSP_UpDownRate2HwDfe(stream_feature_convert_samplerate_t rate)
{
/*
    AU_DFE_UPDOWN_RATIO hw_rate;
    switch(rate)
    {
        case UPSAMPLE_BY1:
            hw_rate = AU_DEF_UP_BY_1;
            break;
        case UPSAMPLE_BY2:
            hw_rate = AU_DEF_UP_BY_2;
            break;
        case UPSAMPLE_BY3:
            hw_rate = AU_DEF_UP_BY_3;
            break;
        case UPSAMPLE_BY4:
            hw_rate = AU_DEF_UP_BY_4;
            break;
        case UPSAMPLE_BY6:
            hw_rate = AU_DEF_UP_BY_6;
            break;
        case UPSAMPLE_BY12:
            hw_rate = AU_DEF_UP_BY_12;
            break;
        case DOWNSAMPLE_BY1:
            hw_rate = AU_DEF_DOWN_BY_1;
            break;
        case DOWNSAMPLE_BY2:
            hw_rate = AU_DEF_DOWN_BY_2;
            break;
        case DOWNSAMPLE_BY3:
            hw_rate = AU_DEF_DOWN_BY_3;
            break;
        case DOWNSAMPLE_BY4:
            hw_rate = AU_DEF_DOWN_BY_4;
            break;
        case DOWNSAMPLE_BY6:
            hw_rate = AU_DEF_DOWN_BY_6;
            break;
        case DOWNSAMPLE_BY12:
            hw_rate = AU_DEF_DOWN_BY_12;
            break;
        default:
            hw_rate = AU_DEF_DOWN_BY_1;
            break;
    }
    return hw_rate;
    */
    UNUSED(rate);
    return 0;
}


U8 DSP_UpDownRate2Value(stream_feature_convert_samplerate_t rate)
{
/*
    U8 value;
    switch(rate)
    {
        case UPSAMPLE_BY1:
        case DOWNSAMPLE_BY1:
            value = 1;
            break;
        case UPSAMPLE_BY2:
        case DOWNSAMPLE_BY2:
            value = 2;
            break;
        case UPSAMPLE_BY3:
        case DOWNSAMPLE_BY3:
            value = 3;
            break;
        case UPSAMPLE_BY4:
        case DOWNSAMPLE_BY4:
            value = 4;
            break;
        case UPSAMPLE_BY6:
        case DOWNSAMPLE_BY6:
            value = 6;
            break;
        case UPSAMPLE_BY12:
        case DOWNSAMPLE_BY12:
            value = 12;
            break;
        default:
            value = 1;
            break;
    }
    return value;
    */
    UNUSED(rate);
    return 1;
}

stream_feature_convert_samplerate_t DSP_UpValue2Rate(U8 value)
{
/*
    stream_feature_convert_samplerate_t rate;
    switch(value)
    {
        case 1:
            rate = UPSAMPLE_BY1;
            break;
        case 2:
            rate = UPSAMPLE_BY2;
            break;
        case 3:
            rate = UPSAMPLE_BY3;
            break;
        case 4:
            rate = UPSAMPLE_BY4;
            break;
        case 6:
            rate = UPSAMPLE_BY6;
            break;
        case 12:
            rate = UPSAMPLE_BY12;
            break;
        default:
            rate = UPSAMPLE_BY1;
            break;
    }
    return rate;
    */
    UNUSED(value);
    return 0;
}
stream_feature_convert_samplerate_t DSP_DownValue2Rate(U8 value)
{
/*
    stream_feature_convert_samplerate_t rate;
    switch(value)
    {
        case 1:
            rate = DOWNSAMPLE_BY1;
            break;
        case 2:
            rate = DOWNSAMPLE_BY2;
            break;
        case 3:
            rate = DOWNSAMPLE_BY3;
            break;
        case 4:
            rate = DOWNSAMPLE_BY4;
            break;
        case 6:
            rate = DOWNSAMPLE_BY6;
            break;
        case 12:
            rate = DOWNSAMPLE_BY12;
            break;
        default:
            rate = DOWNSAMPLE_BY1;
            break;
    }
    return rate;
    */
    UNUSED(value);
    return 0;
}

AU_SRC_FS_IN DSP_FsChange2SRCInRate(stream_samplerate_t fs_in)
{

    AU_SRC_FS_IN src_in_rate;
    switch (fs_in)
    {
        case FS_RATE_8K:
            src_in_rate = SRC_IN_RATE_8K;
            break;
        case FS_RATE_16K:
            src_in_rate = SRC_IN_RATE_16K;
            break;
        case FS_RATE_24K:
            src_in_rate = SRC_IN_RATE_24K;
            break;
        case FS_RATE_32K:
            src_in_rate = SRC_IN_RATE_32K;
            break;
        case FS_RATE_44_1K:
            src_in_rate = SRC_IN_RATE_44_1K;
            break;
        case FS_RATE_48K:
            src_in_rate = SRC_IN_RATE_48K;
            break;
        case FS_RATE_88_2K:
            src_in_rate = SRC_IN_RATE_88_2K;
            break;
        case FS_RATE_96K:
            src_in_rate = SRC_IN_RATE_96K;
            break;
        case FS_RATE_192K:
            src_in_rate = SRC_IN_RATE_192K;
            break;
        default:
            src_in_rate = SRC_IN_SW_DEF;
            break;
    }
    return src_in_rate;
}

AU_SRC_SR_OUT DSP_RsChange2SRCOutRs(stream_resolution_t rs_out)

{

    AU_SRC_SR_OUT src_out_rs;
    switch (rs_out)
    {
        case RESOLUTION_16BIT:
            src_out_rs = SRC_OUT_RS_16BIT;
            break;
        case RESOLUTION_32BIT:
            src_out_rs = SRC_OUT_RS_32BIT;
            break;
        default:
            src_out_rs = SRC_OUT_RS_DEF;
            break;
    }
    return src_out_rs;

}

AU_SRC_FS_OUT DSP_FsChange2SRCOutRate(stream_samplerate_t fs_out)
{

    AU_SRC_FS_OUT src_out_rate;
    switch (fs_out)
    {
        case FS_RATE_8K:
            src_out_rate = SRC_OUT_RATE_8K;
            break;
        case FS_RATE_16K:
            src_out_rate = SRC_OUT_RATE_16K;
            break;
        case FS_RATE_32K:
            src_out_rate = SRC_OUT_SW_DEF_32K;
            break;
        case FS_RATE_44_1K:
            src_out_rate = SRC_OUT_SW_DEF_44_1K;
            break;
        case FS_RATE_48K:
            src_out_rate = SRC_OUT_RATE_48K;
            break;
        case FS_RATE_88_2K:
            src_out_rate = SRC_OUT_SW_DEF_88_2K;
            break;
        case FS_RATE_96K:
            src_out_rate = SRC_OUT_RATE_96K;
            break;
        case FS_RATE_192K:
            src_out_rate = SRC_OUT_RATE_192K;
            break;
        default:
            src_out_rate = SRC_OUT_SW_DEF;
            break;
    }
    return src_out_rate;

}

U32 DSP_FsChange2Value(stream_samplerate_t fs_in)
{
/*
    U32 value;
    switch (fs_in)
    {
        case FS_RATE_8K:
            value = 8000;
            break;
        case FS_RATE_16K:
            value = 16000;
            break;
        case FS_RATE_24K:
            value = 24000;
            break;
        case FS_RATE_32K:
            value = 32000;
            break;
        case FS_RATE_44_1K:
            value = 44100;
            break;
        case FS_RATE_48K:
            value = 48000;
            break;
        case FS_RATE_88_2K:
            value = 88200;
            break;
        case FS_RATE_96K:
            value = 96000;
            break;
        case FS_RATE_192K:
            value = 192000;
            break;
        default:
            value = 96000;
            break;
    }
    return value;
    */
    UNUSED(fs_in);
    return 0;
}

U32 DSP_SRCInRateChange2Value(AU_SRC_FS_IN src_in_rate)
{
/*
    U32 fs_in;
    switch (src_in_rate)
    {
        case SRC_IN_RATE_8K:
            fs_in = 8000;
            break;
        case SRC_IN_RATE_11_025K:
            fs_in = 11025;
            break;
        case SRC_IN_RATE_12K:
            fs_in = 12000;
            break;
        case SRC_IN_RATE_16K:
            fs_in = 16000;
            break;
        case SRC_IN_RATE_22_05K:
            fs_in = 22050;
            break;
        case SRC_IN_RATE_24K:
            fs_in = 24000;
            break;
        case SRC_IN_RATE_32K:
            fs_in = 32000;
            break;
        case SRC_IN_RATE_44_1K:
            fs_in = 44100;
            break;
        case SRC_IN_RATE_48K:
            fs_in = 48000;
            break;
        case SRC_IN_RATE_64K:
            fs_in = 64000;
            break;
        case SRC_IN_RATE_88_2K:
            fs_in = 88200;
            break;
        default:
        case SRC_IN_RATE_96K:
            fs_in = 96000;
            break;
        case SRC_IN_RATE_192K:
            fs_in = 192000;
            break;
    }
    return fs_in;
    */
    UNUSED(src_in_rate);
    return 0;
}

U32 DSP_SRCOutRateChange2Value(AU_SRC_FS_OUT src_out_rate)
{
/*
    U32 fs_out;
    switch (src_out_rate)
    {
        case SRC_OUT_RATE_8K:
            fs_out = 8000;
            break;
        case SRC_OUT_RATE_16K:
            fs_out = 16000;
            break;
        case SRC_OUT_SW_DEF_32K:
            fs_out = 32000;
            break;
        case SRC_OUT_SW_DEF_44_1K:
            fs_out = 44100;
            break;
        case SRC_OUT_RATE_48K:
            fs_out = 48000;
            break;
        case SRC_OUT_SW_DEF_64K:
            fs_out = 64000;
            break;
        case SRC_OUT_SW_DEF_88_2K:
            fs_out = 88200;
            break;
        case SRC_OUT_RATE_96K:
            fs_out = 96000;
            break;
        case SRC_OUT_RATE_192K:
            fs_out = 192000;
            break;

        case SRC_OUT_SW_DEF:
        case SRC_OUT_MAX:
        default:
            configASSERT(FALSE);
            break;
    }
    return fs_out;
    */
    UNUSED(src_out_rate);
    return 0;
}

SPDIF_RELATED_enum_s DSP_ChangeFs2SpdifRate(stream_samplerate_t fs)
{
/*
    SPDIF_RELATED_enum_s spdif_rate;
    switch (fs)
    {
        case FS_RATE_32K:
            spdif_rate = SPDIF_SAMPLE_RATE_32k;
            break;
        case FS_RATE_44_1K:
            spdif_rate = SPDIF_SAMPLE_RATE_44k;
            break;
        case FS_RATE_48K:
            spdif_rate = SPDIF_SAMPLE_RATE_48k;
            break;
        case FS_RATE_96K:
            spdif_rate = SPDIF_SAMPLE_RATE_96k;
            break;
        case FS_RATE_192K:
            spdif_rate = SPDIF_SAMPLE_RATE_192k;
            break;
        default:
            spdif_rate = SPDIF_SAMPLE_RATE_96k;
            break;
    }
    return spdif_rate;
    */
    UNUSED(fs);
    return 0;
}

U32 DSP_ChangeSpdifRate2Value(SPDIF_RELATED_enum_s spdif_rate)
{
/*
    U32 fs_value;
    switch (spdif_rate)
    {
        case SPDIF_SAMPLE_RATE_32k:
            fs_value = 32000;
            break;
        case SPDIF_SAMPLE_RATE_44k:
            fs_value = 44100;
            break;
        case SPDIF_SAMPLE_RATE_48k:
            fs_value = 48000;
            break;
        case SPDIF_SAMPLE_RATE_96k:
            fs_value = 96000;
            break;
        case SPDIF_SAMPLE_RATE_192k:
            fs_value = 1922000;
            break;
        default:
            fs_value = 0;
            break;
    }
    return fs_value;
    */
    UNUSED(spdif_rate);
    return 0;
}




AU_SRC_FS_IN DSP_GetSRCInRate(SRC_PTR_s src_ptr)
{
    //return src_ptr->SRC_SET.field.INPUT_SAMPLE_RATE;
    UNUSED(src_ptr);
    return 0;
}

AU_SRC_FS_OUT DSP_GetSRCOutRate(SRC_PTR_s src_ptr)
{
    //return src_ptr->SRC_SET.field.OUTPUT_SAMPLE_RATE;
    UNUSED(src_ptr);
    return 0;
}

VOID* DSP_GetSRCIn1BufPtr(SRC_PTR_s src_ptr)
{
/*
    AU_ODFE_RADMA_s *radma_ptr;
    radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_ptr + 0x0100);

    return (VOID*)radma_ptr->INIT.field.ADDR;
    */
    UNUSED(src_ptr);
    return 0;
}
VOID* DSP_GetSRCIn1NextPtr(SRC_PTR_s src_ptr)
{
/*
    AU_ODFE_RADMA_s *radma_ptr;
    radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_ptr + 0x0100);

    return (VOID*)radma_ptr->NEXT.field.ADDR;
    */
    UNUSED(src_ptr);
    return 0;
}
VOID* DSP_GetSRCIn2BufPtr(SRC_PTR_s src_ptr)
{
/*
    AU_ODFE_RADMA_s *radma_ptr;
    radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_ptr + 0x0200);

    return (VOID*)radma_ptr->INIT.field.ADDR;
    */
    UNUSED(src_ptr);
    return 0;
}
VOID* DSP_GetSRCIn2NextPtr(SRC_PTR_s src_ptr)
{
/*
    AU_ODFE_RADMA_s *radma_ptr;
    radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_ptr + 0x0200);

    return (VOID*)radma_ptr->NEXT.field.ADDR;
    */
    UNUSED(src_ptr);
    return 0;
}
VOID* DSP_GetSRCInBufPtr(SRC_PTR_s src_ptr, U32 CH)
{
/*
    AU_ODFE_RADMA_s *radma_ptr;
    radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_ptr + 0x0100 + CH*0x0100);

    return (VOID*)radma_ptr->INIT.field.ADDR;
    */
    UNUSED(src_ptr);
    UNUSED(CH);
    return 0;
}
VOID* DSP_GetSRCInNextPtr(SRC_PTR_s src_ptr, U32 CH)
{
/*
    AU_ODFE_RADMA_s *radma_ptr;
    radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_ptr + 0x0100 + CH*0x0100);

    return (VOID*)radma_ptr->NEXT.field.ADDR;
    */
    UNUSED(src_ptr);
    UNUSED(CH);
    return 0;
}

U32 DSP_GetSRCInReadOffset(SRC_PTR_s src_ptr)
{
/*
    AU_ODFE_RADMA_s *radma_ptr;
    radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_ptr + 0x0100);

    return (radma_ptr->NEXT.field.ADDR - radma_ptr->INIT.field.ADDR);
    */
    UNUSED(src_ptr);
    return 0;
}


U32 DSP_GetSRCInBufSize(SRC_PTR_s src_ptr)
{
/*
    AU_ODFE_RADMA_s *radma_ptr;
    radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_ptr + 0x0100);

    return (radma_ptr->SET.field.BUF_SIZE<<2);
    */
    UNUSED(src_ptr);
    return 0;
}
U32 DSP_GetSRCInFrameSize(SRC_PTR_s src_ptr)
{
/*
    AU_ODFE_RADMA_s *radma_ptr;
    radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_ptr + 0x0100);

    return (radma_ptr->SET.field.THD_SIZE<<2);
    */
    UNUSED(src_ptr);
    return 0;
}

VOID* DSP_GetSRCOut1BufPtr(SRC_PTR_s src_ptr)
{
/*
    AU_IDFE_WADMA_s *wadma_ptr;
    wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_ptr + 0x0300);

    return (VOID*)wadma_ptr->INIT.field.ADDR;
    */
    UNUSED(src_ptr);
    return 0;
}
VOID* DSP_GetSRCOut1NextPtr(SRC_PTR_s src_ptr)
{
/*
    AU_IDFE_WADMA_s *wadma_ptr;
    wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_ptr + 0x0300);

    return (VOID*)wadma_ptr->NEXT.field.ADDR;
    */
    UNUSED(src_ptr);
    return 0;
}

VOID* DSP_GetSRCOut2BufPtr(SRC_PTR_s src_ptr)
{
/*
    AU_IDFE_WADMA_s *wadma_ptr;
    wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_ptr + 0x0400);

    return (VOID*)wadma_ptr->INIT.field.ADDR;
    */
    UNUSED(src_ptr);
    return 0;
}
VOID* DSP_GetSRCOut2NextPtr(SRC_PTR_s src_ptr)
{
/*
    AU_IDFE_WADMA_s *wadma_ptr;
    wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_ptr + 0x0400);

    return (VOID*)wadma_ptr->NEXT.field.ADDR;
    */
    UNUSED(src_ptr);
    return 0;
}

VOID* DSP_GetSRCOutBufPtr(SRC_PTR_s src_ptr, U32 CH)
{
/*
    AU_IDFE_WADMA_s *wadma_ptr;
    wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_ptr + 0x0300 + CH*0x0100);

    return (VOID*)wadma_ptr->INIT.field.ADDR;
    */
    UNUSED(src_ptr);
    UNUSED(CH);
    return 0;
}
VOID* DSP_GetSRCOutNextPtr(SRC_PTR_s src_ptr, U32 CH)
{
/*
    AU_IDFE_WADMA_s *wadma_ptr;
    wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_ptr + 0x0300 + CH*0x0100);

    return (VOID*)wadma_ptr->NEXT.field.ADDR;
    */
    UNUSED(src_ptr);
    UNUSED(CH);
    return 0;
}

U32 DSP_GetSRCOutWriteOffset(SRC_PTR_s src_ptr)
{
/*
    AU_IDFE_WADMA_s *wadma_ptr;
    wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_ptr + 0x0300);

    return (wadma_ptr->NEXT.field.ADDR - wadma_ptr->INIT.field.ADDR);
    */
    UNUSED(src_ptr);
    return 0;
}

U32 DSP_GetSRCOutBufSize(SRC_PTR_s src_ptr)
{
/*
    AU_IDFE_WADMA_s *wadma_ptr;
    wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_ptr + 0x0300);

    return (wadma_ptr->SET.field.BUF_SIZE<<2);
    */
    UNUSED(src_ptr);
    return 1;//avoid divide by 0
}
U32 DSP_GetSRCOutFrameSize(SRC_PTR_s src_ptr)
{
/*
    AU_IDFE_WADMA_s *wadma_ptr;
    wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_ptr + 0x0300);

    return (wadma_ptr->SET.field.THD_SIZE<<2);
    */
    UNUSED(src_ptr);
    return 0;
}

VOID DSP_SetSRCTrigger(SRC_PTR_s VOLATILE src_ptr)
{
     //src_ptr->SRC_CTL0.field.ENABLE = 1;
     UNUSED(src_ptr);
}

BOOL DSP_GetSRCStatus(SRC_PTR_s VOLATILE src_ptr)
{
    //return src_ptr->SRC_CTL0.field.ENABLE;//TRUE:processing, FALSE:done
    UNUSED(src_ptr);
    return 0;
}

SRC_PTR_s DSP_GET_SRC_SFR(AU_HW_SRC_SEL sel)
{
/*
    SRC_PTR_s src_ptr;
    switch(sel)
    {
        case AU_HW_SRC_A:
            src_ptr = (SRC_PTR_s)&SRC_A;
            break;
        case AU_HW_SRC_B:
            src_ptr = (SRC_PTR_s)&SRC_B;
            break;
        default:
            src_ptr = NULL;
            break;
    }
    return src_ptr;
    */
    UNUSED(sel);
    return 0;
}



VOID DSP_SetSRCCompensation(SRC_PTR_s src_ptr, U32 sign, U32 value)
{
/*
     src_ptr->SRC_CMP.field.SRC_CMP_VAL             = value;
     src_ptr->SRC_CMP.field.SRC_CMP_SGN             = sign;
     src_ptr->SRC_CMP.field.UPDATE_SRC_CMP          = 1;
     */
    UNUSED(src_ptr);
    UNUSED(sign);
    UNUSED(value);
}

VOID DSP_SetADCompensation(U32 sign, U32 value)
{
/*
    AUDIO_CODEC.DEC1_INT_CMP.field.INT_CMP_VALUE    = value;
    AUDIO_CODEC.DEC1_INT_CMP.field.INT_CMP_SIGN     = sign;
    AUDIO_CODEC.DEC1_INT_CMP.field.UPDATE_CMP_VALUE = 1;
    */
    UNUSED(sign);
    UNUSED(value);
}

VOID* DSP_GetAudioInNextPtr(ADMA_CH_NUM_s Channel_sel)
{
/*
    AU_IDFE_WADMA_s *wadma_ptr;

    wadma_ptr = DSP_DRV_WADMA_CH_GET(Channel_sel);
    return (VOID*)wadma_ptr->NEXT.field.ADDR;
    */
    UNUSED(Channel_sel);
    return 0;
}

VOID* DSP_GetAudioOutNextPtr(ADMA_CH_NUM_s Channel_sel)
{
/*
    AU_ODFE_RADMA_s *radma_ptr;

    radma_ptr = DSP_DRV_RADMA_CH_GET(Channel_sel);
    return (VOID*)radma_ptr->NEXT.field.ADDR;
    */
    UNUSED(Channel_sel);
    return 0;
}



SRC_PTR_s DSP_DRV_SRC_VDM_INIT(DSP_DRV_SRC_VDM_INIT_STRU_PTR src_setting)
{
/*
    ADMA_CH_NUM_s ch0, ch1;
    AU_SRC_CH src_ch_sel;
    AU_ODFE_RADMA_s *ch0_radma_ptr, *ch1_radma_ptr;
    AU_IDFE_WADMA_s *ch0_wadma_ptr, *ch1_wadma_ptr;
    src_ch_sel = (src_setting->channel_num>1) ? SRC_TWO_CH : SRC_CH0_ONLY;


    if (src_setting->src_ptr == &SRC_A)
    {
        ch0     = SRCA_CH0;
        ch1     = SRCA_CH1;
    }
    else if (src_setting->src_ptr == &SRC_B)
    {
        ch0     = SRCB_CH0;
        ch1     = SRCB_CH1;
    }
    else if ((!SRC_B.SRC_CTL0.field.ENABLE) &&
        (!AU_SRC_B_CH0_WADMA.CTL.field.ENABLE) &&
        (!AU_SRC_B_CH1_WADMA.CTL.field.ENABLE))
    {
        src_setting->src_ptr = (SRC_PTR_s)&SRC_B;
        ch0     = SRCB_CH0;
        ch1     = SRCB_CH1;
    }
    else if ((!SRC_A.SRC_CTL0.field.ENABLE) &&
             (!AU_SRC_A_CH0_WADMA.CTL.field.ENABLE) &&
             (!AU_SRC_A_CH1_WADMA.CTL.field.ENABLE))
    {
        src_setting->src_ptr = (SRC_PTR_s)&SRC_A;
        ch0     = SRCA_CH0;
        ch1     = SRCA_CH1;
    }
    else
    {
        configASSERT(FALSE);
    }

    while(src_setting->src_ptr->SRC_CTL0.field.ENABLE);

    ch0_radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_setting->src_ptr + 0x0100);
    ch1_radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_setting->src_ptr + 0x0200);
    ch0_wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_setting->src_ptr + 0x0300);
    ch1_wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_setting->src_ptr + 0x0400);

    DSP_DRV_RADMA_INIT(ch0, src_setting->radma_buf_addr, src_setting->radma_buf_size, src_setting->radma_THD);
    ch0_radma_ptr->CTL.field.THD_INTR_MASK = 1;
    ch0_radma_ptr->CTL.field.ERR_INTR_MASK = 1;
    ch0_radma_ptr->CTL.field.ENABLE        = 1;
    if (src_setting->channel_num>1)
    {
        DSP_DRV_RADMA_INIT(ch1, src_setting->radma_buf_addr + src_setting->radma_buf_size, src_setting->radma_buf_size, src_setting->radma_THD);
        ch1_radma_ptr->CTL.field.THD_INTR_MASK = 1;
        ch1_radma_ptr->CTL.field.ERR_INTR_MASK = 1;
        ch1_radma_ptr->CTL.field.ENABLE        = 1;
    }
    DSP_DRV_WADMA_INIT(ch0, src_setting->wadma_buf_addr, src_setting->wadma_buf_size, src_setting->wadma_THD);
    ch0_wadma_ptr->CTL.field.THD_INTR_MASK = 1;
    ch0_wadma_ptr->CTL.field.ERR_INTR_MASK = 1;
    ch0_wadma_ptr->CTL.field.ENABLE        = 1;
    if (src_setting->channel_num>1)
    {
        DSP_DRV_WADMA_INIT(ch1, src_setting->wadma_buf_addr + src_setting->wadma_buf_size, src_setting->wadma_buf_size, src_setting->wadma_THD);
        ch1_wadma_ptr->CTL.field.THD_INTR_MASK = 1;
        ch1_wadma_ptr->CTL.field.ERR_INTR_MASK = 1;
        ch1_wadma_ptr->CTL.field.ENABLE        = 1;
    }

    src_setting->src_ptr->SRC_CTL0.field.VECTOR_MODE_LENGTH = (src_setting->mode == SRC_IVDM)
                                                                ?src_setting->radma_THD>>2
                                                                :src_setting->wadma_THD>>2;
    DSP_DRV_SRC_INIT(src_setting->src_ptr, src_setting->mode, src_ch_sel, src_setting->fs_in, src_setting->fs_out, src_setting->Res_In, src_setting->Res_Out);
    while(src_setting->src_ptr->SRC_CTL0.field.ENABLE);

    return src_setting->src_ptr;
    */
    UNUSED(src_setting);
    return 0;
}

VOID DSP_DRV_SINK_SRC_VDM_PreTrigger(SINK sink, STREAM_SRC_VDM_PTR_t VOLATILE src_vdm)
{
/*
    U32 targetQ;
    U32 targetADDR;
    U32 thdSize;
    AU_ODFE_RADMA_PTR VOLATILE src_radma_ptr;
    AU_IDFE_WADMA_PTR VOLATILE src_wadma_ptr;

    src_radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_vdm->src_ptr + 0x0100);
    src_wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_vdm->src_ptr + 0x0300);

    thdSize = (src_wadma_ptr->SET.field.THD_SIZE<<2);
    targetQ = ((sink->streamBuffer.BufferInfo.length/thdSize)/2);



    //AU_ODFE_RADMA_PTR VOLATILE oDFE_radma_ptr;
    //oDFE_radma_ptr = (sink->param.audio.channel_sel == AUDIO_CHANNEL_VP)
    //                   ? &AU_ODFE_CH2_RADMA
    //                   :(sink->param.audio.channel_sel == AUDIO_CHANNEL_B)
    //                       ? &AU_ODFE_CH1_RADMA
    //                       : &AU_ODFE_CH0_RADMA;


    #if 1
    do
    {
        #if 0
        targetADDR = ((targetQ*thd_size) + (oDFE_radma_ptr->NEXT.field.ADDR - oDFE_radma_ptr->INIT.field.ADDR))%sink->streamBuffer.BufferInfo.length
                     + oDFE_radma_ptr->INIT.field.ADDR;
        #else
        targetADDR = (targetQ*thdSize)%sink->streamBuffer.BufferInfo.length
                     + src_wadma_ptr->INIT.field.ADDR;
        #endif
        src_vdm->src_ptr->SRC_CTL0.field.ENABLE = 1;
        while(src_vdm->src_ptr->SRC_CTL0.field.ENABLE);
    }while((src_wadma_ptr->NEXT.field.ADDR - targetADDR)>thdSize);
    #endif

    Sink_Audio_BufferInfo_Rst(sink, src_radma_ptr->NEXT.field.ADDR-src_radma_ptr->INIT.field.ADDR);
*/
    UNUSED(sink);
    UNUSED(src_vdm);
}

VOID DSP_DRV_SOURCE_SRC_VDM_PreTrigger(SOURCE source, STREAM_SRC_VDM_PTR_t VOLATILE src_vdm)
{
/*
    U32 VOLATILE targetQ;
    U32 VOLATILE targetADDR;
    U32 VOLATILE thdSize;
    AU_ODFE_RADMA_PTR VOLATILE src_radma_ptr;
    AU_IDFE_WADMA_PTR VOLATILE src_wadma_ptr;

    src_radma_ptr = (AU_ODFE_RADMA_s *)((U32)src_vdm->src_ptr + 0x0100);
    src_wadma_ptr = (AU_IDFE_WADMA_s *)((U32)src_vdm->src_ptr + 0x0300);

    thdSize = (src_radma_ptr->SET.field.THD_SIZE<<2);
    targetQ = ((source->streamBuffer.BufferInfo.length/thdSize)/2);

    #if 1
    do
    {
        targetADDR = (targetQ*thdSize)%source->streamBuffer.BufferInfo.length
                     + src_radma_ptr->INIT.field.ADDR;
        src_vdm->src_ptr->SRC_CTL0.field.ENABLE = 1;
        while(src_vdm->src_ptr->SRC_CTL0.field.ENABLE);
    }while((src_radma_ptr->NEXT.field.ADDR - targetADDR)>thdSize);
    #endif

   Source_Audio_BufferInfo_Rst(source, src_wadma_ptr->NEXT.field.ADDR-src_wadma_ptr->INIT.field.ADDR);
*/
    UNUSED(source);
    UNUSED(src_vdm);
}

/**
 *
 *
 *
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_DRV_SelectOdfeClk (AU_ODFE_CLK_GATE_t Clk)
{
    //AUDIO_DFE.CTL2.field.AU_ODFE_GATED_CLK = Clk;

    // ODFE_CLK_24MHZ for audio in/out
    UNUSED(Clk);
}


/**
 *
 *
 *
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_DRV_SelectIdfeClk (AU_IDFE_CLK_GATE_t Clk)
{
    //AUDIO_DFE.CTL2.field.AU_IDFE_GATED_CLK = Clk;

    // IDFE_CLK_24MHZ for audio in/out
    UNUSED(Clk);
}


/**
 *
 *
 *
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_DRV_EnableOdfeClock (VOID)
{
/*
    if (AUDIO_DFE.CTL2.field.ODFE_CLK_EN == 0)
    {
        AUDIO_DFE.CTL2.field.ODFE_CLK_EN = 1;
    }
    */
}


/**
 *
 *
 *
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_DRV_DisableOdfeClock (VOID)
{
    //AUDIO_DFE.CTL2.field.ODFE_CLK_EN = 0;
}


/**
 *
 *
 *
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_DRV_EnableIdfeClock (VOID)
{
/*
    if (AUDIO_DFE.CTL2.field.IDFE_CLK_EN == 0)
    {
        AUDIO_DFE.CTL2.field.IDFE_CLK_EN = 1;
    }
    */
}


/**
 *
 *
 *
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_DRV_DisableIdfeClock (VOID)
{
    //AUDIO_DFE.CTL2.field.IDFE_CLK_EN = 0;
}


/**
 *
 *
 *
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_DRV_ClearReadAdmaIsrStatus(VOID)
{
    //AU_ODFE_CH0_RADMA.CTL.field.THD_INTR_CLR = 1;
    //AU_ODFE_CH1_RADMA.CTL.field.THD_INTR_CLR = 1;
}


/**
 *
 *
 *
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */

VOID DSP_DRV_ClearSrcIsrStatus(VOID)
{
    //AU_SRC_A_CH0_RADMA.CTL.field.THD_INTR_CLR = 1;
    //AU_SRC_A_CH1_RADMA.CTL.field.THD_INTR_CLR = 1;
}


/**
 *
 *
 *
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */

VOID DSP_DRV_ClearVpIsrStatus(VOID)
{
    //AU_ODFE_CH2_RADMA.CTL.field.THD_INTR_CLR    = 1;
    //AU_ODFE_CH2_RADMA.CTL.field.ERR_INTR_CLR    = 1;
}


/**
 *
 *
 *
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */

VOID DSP_DRV_ClearWriteAdmaIsrStatus(VOID)
{
   // AU_IDFE_CH0_WADMA.CTL.field.THD_INTR_CLR = 1;
   // AU_IDFE_CH1_WADMA.CTL.field.THD_INTR_CLR = 1;
}


