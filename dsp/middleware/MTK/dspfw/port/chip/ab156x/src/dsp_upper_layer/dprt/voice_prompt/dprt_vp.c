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

#include "dprt_vp.h"





/******************************************************************************
 * Function Declaration
 ******************************************************************************/
BOOL            AMRInit                     (VOID* para);
BOOL            AMRCodec                    (VOID* para);


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

#if 0
VP_CTL_t * VPCtl;

////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * VP_Setup
 *
 * Setup VoicePrompt
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID VP_Setup(VOID)
{
    VP_MemInit();

	/* Start VP decode until buffer full */
    while(VPCtl->STATUS.count < (VP_BUFFER_SIZE - VP_GENERATE_SIZE))
    {
        VP_PatternDecode();
    }

    VP_SettingCSR(TRUE);

    OS_INTR_RegisterHdlr(OS_INTR_ID_LEVEL3, OS_LV3_INTR_Hdlr);
    OS_LV3_INTR_RegisterHandler(OS_LV3_INTR_ID_ODFE_VP, VP_IsrHandler, DPR_TASK_ID);
}

/**
 * VP_Process
 *
 * Active Entry for VoicePrompt background process
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 *
 * @Return : processing status
 */
BOOL VP_Process(VOID)
{
    if (VPCtl->PARA.RemainingLength)
    {
        osTaskWaitSignal( (U32*)&VPCtl->STATUS.IntrSignal, 1 , 0 );

        if (VPCtl->STATUS.count < (VP_BUFFER_SIZE - VP_GENERATE_SIZE))
        {
            VP_PatternDecode();
        }
    }
    else if (VPCtl->STATUS.PaddingCount < 2)
    {
        if (VPCtl->STATUS.count < (VP_BUFFER_SIZE - VP_GENERATE_SIZE))
        {
            VP_ZeroPadding();
            VPCtl->STATUS.PaddingCount++;
        }
    }
    /*ZeroPadding done*/
    else if (VPCtl->STATUS.count < VP_ODFE_BUFFER_SIZE)
    {
        return FALSE;
    }
    return TRUE;
}

/**
 * VP_DeInit
 *
 * DeInit VoicePrompt
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID VP_DeInit(VOID)
{
    VP_SettingCSR(FALSE);
    OS_LV3_INTR_CancelHandler(OS_LV3_INTR_ID_ODFE_VP);
}

/**
 * VP_MemInit
 *
 * Initialize Memory of VoicePrompt.
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID VP_MemInit(VOID)
{
    VPCtl = (VP_CTL_t *)DSPMEM_tmalloc(DPR_TASK_ID, (SIZE)sizeof(VP_CTL_t));
    VPCtl->STATUS.count          = 0;
    VPCtl->STATUS.wo             = 0;
    VPCtl->STATUS.ro             = 0;
    VPCtl->STATUS.PaddingCount   = 0;
    VPCtl->STATUS.IntrSignal     = 0;
    memset((U16*)&VPCtl->BUF.ODFE_BUF[0], 0, sizeof(VPCtl->BUF.ODFE_BUF));
    memset((U16*)&VPCtl->BUF.OUT_BUF[0], 0, sizeof(VPCtl->BUF.OUT_BUF));
    memset((U16*)&VPCtl->BUF.STREAM_BUF[0], 0, sizeof(VPCtl->BUF.STREAM_BUF));
    memset((U16*)&VPCtl->BUF.DECODE_BUF[0], 0, sizeof(VPCtl->BUF.DECODE_BUF));

    VPCtl->PARA.AmrMode          = 0xFFFF;
    VPCtl->PARA.AmrStreamSize    = 1;
    VPCtl->PARA.RemainingLength  = 0xFFFF;
}

/**
 * VP_SettingCSR
 *
 * Setting CSR of VoicePrompt.
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID VP_SettingCSR(BOOL ctrl)
{
    OS_PS ps = OS_ENTER_CRITICAL();
    #if 1
    AU_ODFE_CH2_RADMA.CTL.field.SW_RESET        = 1;
    AU_ODFE_CH2_RADMA.CTL.field.THD_INTR_MASK   = ctrl^1;
    AU_ODFE_CH2_RADMA.CTL.field.ERR_INTR_MASK   = 1;
    AU_ODFE_CH2_RADMA.CTL.field.DATA_DIR        = 1;
    AU_ODFE_CH2_RADMA.CTL.field.FLUSH_DATA      = 0;
    AU_ODFE_CH2_RADMA.CTL.field.THD_INTR_CLR    = 1;
    AU_ODFE_CH2_RADMA.CTL.field.ERR_INTR_CLR    = 1;
    AU_ODFE_CH2_RADMA.SET.field.BUF_SIZE        = VP_ODFE_BUFFER_SIZE;
    AU_ODFE_CH2_RADMA.SET.field.THD_SIZE        = VP_ODFE_BUFFER_SIZE>>1;
    AU_ODFE_CH2_RADMA.INIT.field.ADDR           = (U32)VPCtl->BUF.ODFE_BUF;
    AU_ODFE_CH2_RADMA.CTL.field.ENABLE          = ctrl;
    #else
    DSP_DRV_RADMA_INIT (ADMA_CH2, VPCtl->BUF.OUT_BUF, RT_ODFE_BUFFER_SIZE<<1, RT_ODFE_BUFFER_SIZE);//Not enable yet
    #endif

    AUDIO_DFE.CTL1.field.RST_AU_ODFE_INT6       = 1;
    AUDIO_DFE.CTL1.field.RST_AU_ODFE_INTF       = 1;
    AUDIO_DFE.OUT_SET0.field.BYPASS_INT6_CC1_FIL= 0;
    AUDIO_DFE.OUT_SET0.field.INT6_UPS_RATIO     = 4;
    AUDIO_DFE.OUT_SET0.field.INT6_BIT_RES       = 0;
    AUDIO_DFE.CTL0.field.EN_AU_ODFE_INT6        = ctrl;
    AUDIO_DFE.CTL0.field.EN_AU_ODFE_INTF        = ctrl;

    CODEC_CTL0_t audio_codec_ctl0;
    audio_codec_ctl0.reg = AUDIO_CODEC.CTL0.reg;
    audio_codec_ctl0.field.EN_INT2_UPS_FIL      = ctrl;
    audio_codec_ctl0.field.SET_INT2_FIL_ACT_CH  = ctrl;
    audio_codec_ctl0.field.EN_AU_DAC_DSM        = ctrl;
    DSP_DRV_ResetInt2Filter(INT2_CH0_CH1);
    AUDIO_CODEC.CTL0.reg = audio_codec_ctl0.reg;
    OS_EXIT_CRITICAL(ps); 
}



/**
 * VP_PatternDecode
 *
 * Voice Prompt main decoding state.
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID VP_PatternDecode(VOID)
{
    S16 * decodeBufPtr;
    decodeBufPtr = (S16 *)(&VPCtl->BUF.OUT_BUF[VPCtl->STATUS.wo]);


    OS_PS ps = OS_ENTER_CRITICAL();
    VPCtl->STATUS.wo    = (VPCtl->STATUS.wo+VP_GENERATE_SIZE)%VP_BUFFER_SIZE;
    VPCtl->STATUS.count =  VPCtl->STATUS.count+VP_GENERATE_SIZE;
    OS_EXIT_CRITICAL(ps); 
    VPCtl->PARA.RemainingLength -= VPCtl->PARA.AmrStreamSize;

}

/**
 * VP_ZeroPadding
 *
 * It only padded zero into the internal buffer.
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID VP_ZeroPadding(VOID)
{
    memset((U16*)&VPCtl->BUF.OUT_BUF[VPCtl->STATUS.wo],
              0,
              VP_GENERATE_SIZE*2);
    OS_PS ps = OS_ENTER_CRITICAL();
    VPCtl->STATUS.wo    = (VPCtl->STATUS.wo+VP_GENERATE_SIZE)%VP_BUFFER_SIZE;
    VPCtl->STATUS.count =  VPCtl->STATUS.count+VP_GENERATE_SIZE;
    OS_EXIT_CRITICAL(ps); 
}


/**
 * VP_IsrHandler
 *
 * VoicePrompt Interrupt Handler.
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID VP_IsrHandler(VOID)
{
    U16 InterruptStatus;
    InterruptStatus = (AU_ODFE_CH2_RADMA.STAT.field.INTR_TOKEN)^1;
    #if 0
    VPCtl->STATUS.IntrSignal = 1;
    if (VPCtl->STATUS.count<VP_ODFE_BUFFER_SIZE)
    {
        memset((U16*)&VPCtl->BUF.ODFE_BUF[InterruptStatus*VP_ODFE_BUFFER_SIZE],
                  0,
                  VP_ODFE_BUFFER_SIZE*2);
    }
    else
    {
        memcpy((U16*)&VPCtl->BUF.ODFE_BUF[InterruptStatus*VP_ODFE_BUFFER_SIZE],
                  (U16*)&VPCtl->BUF.OUT_BUF[VPCtl->STATUS.ro],
                  VP_ODFE_BUFFER_SIZE*2);
        VPCtl->STATUS.ro    = (VPCtl->STATUS.ro+VP_ODFE_BUFFER_SIZE)%VP_BUFFER_SIZE;
        VPCtl->STATUS.count =  VPCtl->STATUS.count-VP_ODFE_BUFFER_SIZE;
    }
    #else
    PromptCtl->STATUS.IntrSignal = 1;
    #endif
    AU_ODFE_CH2_RADMA.CTL.field.THD_INTR_CLR    = 1;
    AU_ODFE_CH2_RADMA.CTL.field.ERR_INTR_CLR    = 1;
}
#endif

/**
 * AMRInit
 *
 * Initialize Parameter of AMRPatternDecode.
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
BOOL AMRInit(VOID* para)
{
#if 0
    //RINGTONE_CTL_PTR_t  ringtone_ctl_ptr = (RINGTONE_CTL_PTR_t)stream_function_get_working_buffer(para);
    memset((U8*)ringtone_ctl_ptr->GENERATION_BUF,
                   0,
                   RT_GENERATE_SIZE*2);
#endif
    UNUSED(para);

    return 0;
}

/**
 * AMRCodec
 *
 * Decode AMR Pattern.
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
BOOL AMRCodec(VOID* para)
{
    //RINGTONE_CTL_PTR_t ringtone_ctl_ptr = (RINGTONE_CTL_PTR_t)stream_function_get_working_buffer(para);
    U8 *pattern_ptr = stream_codec_get_1st_input_buffer(para);
    U8 *decoderOut_ptr = stream_codec_get_1st_output_buffer(para);


    memcpy(decoderOut_ptr,
           pattern_ptr,
           stream_codec_get_output_size(para)*2);

    return 0;
}


