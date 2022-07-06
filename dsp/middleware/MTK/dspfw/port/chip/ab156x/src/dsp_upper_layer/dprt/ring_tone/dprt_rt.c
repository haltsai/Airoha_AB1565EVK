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
#include "dsp_drv_dfe.h"
#include "dsp_memory.h"
#include "dsp_audio_process.h"

#include "dprt_rt.h"
#include "Stream_audio_setting.h"


/******************************************************************************
 * Function Declaration
 ******************************************************************************/
STATIC INLINE VOID  RingTone_Parameter      (VOID* para, RINGTONE_CTL_PTR_t rt_codecMemPtr);


/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/


/******************************************************************************
 * Type Definitions
 ******************************************************************************/



/******************************************************************************
 * Constants
 ******************************************************************************/


/******************************************************************************
 * Variables
 ******************************************************************************/



////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * RingTone_Parameter
 *
 * Setting CSR of RingTone.
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
STATIC INLINE VOID RingTone_Parameter(VOID* para, RINGTONE_CTL_PTR_t rt_codecMemPtr)
{
    rt_codecMemPtr->GENERATION_SIZE = stream_codec_get_output_size(para);

    RINGTONE.RT_CTRL.field.SOFT_RST             = 0;
    RINGTONE.RT_CTRL.field.SEL_8K16K            = 0;
    RINGTONE.RT_CTRL.field.BURST_ENABLE         = 0;
    RINGTONE.RT_SAMPLE_SIZE.reg                 = (Audio_setting->resolution.AudioOutRes == RESOLUTION_32BIT)
                                                    ? rt_codecMemPtr->GENERATION_SIZE>>2
                                                    : rt_codecMemPtr->GENERATION_SIZE>>1;
    RINGTONE.RT_START_ADDR.reg                  = (Audio_setting->resolution.AudioOutRes == RESOLUTION_32BIT)
                                                    ? (U32)rt_codecMemPtr->GENERATION_BUF
                                                    : (U32)stream_codec_get_1st_output_buffer(para);
}

/**
 * RingToneInit
 *
 * Initialize Parameter of RingTone.
 *
 * @Author : MachiWu, BrianChen
 */
BOOL RingToneInit(VOID* para)
{

    RINGTONE_CTL_PTR_t  ringtone_ctl_ptr = (RINGTONE_CTL_PTR_t)stream_codec_get_workingbuffer(para);

    RingTone_Parameter(para,  ringtone_ctl_ptr);


    ringtone_ctl_ptr->PARA.PreTone      = 0;
    ringtone_ctl_ptr->PARA.Tone         = 0;
    ringtone_ctl_ptr->PARA.Volume       = 0;

    stream_codec_modify_output_samplingrate(para, FS_RATE_16K);
    return 0;
}

/**
 * RingToneCodec
 *
 * Generate Tone.
 *
 * @Author : MachiWu, BrianChen
 */
BOOL RingToneCodec(VOID* para)
{
    RINGTONE_CTL_PTR_t ringtone_ctl_ptr = (RINGTONE_CTL_PTR_t)stream_codec_get_workingbuffer(para);
    RT_INFO_PTR_t INFOPtr = (RT_INFO_PTR_t)stream_codec_get_1st_input_buffer(para);
    U32 i;

    if (ringtone_ctl_ptr->PARA.PreTone != INFOPtr->Tone)
    {
        RINGTONE.RT_CTRL.field.TONE_SEL = INFOPtr->Tone;
        ringtone_ctl_ptr->PARA.PreTone = INFOPtr->Tone;
        RINGTONE.RT_START_TRIG.field.START_TRIG     = 1;
    }
    else if (Audio_setting->resolution.AudioOutRes != RESOLUTION_32BIT)
    {
        RINGTONE.RT_START_TRIG.field.START_TRIG     = 1;
    }

    while (RINGTONE.RT_START_TRIG.field.START_TRIG == 1)
    {
        portYIELD();
    }

    ringtone_ctl_ptr->PARA.Volume = INFOPtr->Volume;

    if (Audio_setting->resolution.AudioOutRes == RESOLUTION_32BIT)
    {
        DSP_Converter_16Bit_to_24bit(stream_codec_get_1st_output_buffer(para),
                                     ringtone_ctl_ptr->GENERATION_BUF,
                                     stream_codec_get_output_size(para)>>2);
        RINGTONE.RT_START_TRIG.field.START_TRIG     = 1;
    }

    // Copy output generated to "para->out_ptr[ ]"
    for (i=2 ; i<=stream_codec_get_output_channel_number(para) ; i++)
    {
        memcpy(stream_codec_get_output_buffer(para, i),
               stream_codec_get_1st_output_buffer(para),
               stream_codec_get_output_size(para));
    }
    return 0;
}

