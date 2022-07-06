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

#include <string.h>
#include "dsp_feature_interface.h"
#include "ch_select_interface.h"
#include "hal_platform.h"

static CH_SEL_CTRL_t ch_sel_ctrl = {CH_SEL_STEREO};
static CH_SEL_CTRL_t ch_sel_ctrl_hfp = {CH_SEL_STEREO};
static CH_SEL_CTRL_t ch_sel_ctrl_vp  = {CH_SEL_STEREO};

/**
 *
 * Function Prototype
 *
 */
bool stream_function_channel_selector_initialize_a2dp (void *para);
bool stream_function_channel_selector_initialize_hfp (void *para);
bool stream_function_channel_selector_initialize (void *para, CH_SEL_SCENARIO_TYPE type);
bool stream_function_channel_selector_process_a2dp (void *para);
bool stream_function_channel_selector_process_hfp (void *para);
bool stream_function_channel_selector_process (void *para, CH_SEL_SCENARIO_TYPE type);

/**
 * stream_function_channel_selector_initialize
 *
 * This function is used to init the channel mode for channel selection for a2dp
 *
 * @para : Default parameter of callback function, scenerio_type
 *
 */
bool stream_function_channel_selector_initialize_a2dp (void *para)
{
     stream_function_channel_selector_initialize(para, CH_SEL_A2DP);
     return FALSE;
}

/**
 * stream_function_channel_selector_initialize
 *
 * This function is used to init the channel mode for channel selection for hfp
 *
 * @para : Default parameter of callback function, scenerio_type
 *
 */
bool stream_function_channel_selector_initialize_hfp (void *para)
{
     stream_function_channel_selector_initialize(para, CH_SEL_HFP);
     return FALSE;
}

/**
 * stream_function_channel_selector_initialize
 *
 * This function is used to init the channel mode for channel selection for vp
 *
 * @para : Default parameter of callback function, scenerio_type
 *
 */
bool stream_function_channel_selector_initialize_vp (void *para)
{
     stream_function_channel_selector_initialize(para, CH_SEL_VP);
     return FALSE;
}

/**
 * stream_function_channel_selector_initialize
 *
 * This function is used to init the channel mode for channel selection
 *
 *
 * @para : Default parameter of callback function, scenerio_type
 *
 */
bool stream_function_channel_selector_initialize (void *para, CH_SEL_SCENARIO_TYPE type)
{
    UNUSED(para);

    if(type == CH_SEL_A2DP) {
        if(ch_sel_ctrl.ch_mode >= CH_SEL_NUM) {
            ch_sel_ctrl.ch_mode = CH_SEL_STEREO;
            DSP_MW_LOG_W("[A2DP] Ch_Select_Init ch mode %d -> %d\n", 2, ch_sel_ctrl.ch_mode, CH_SEL_STEREO);
        } else {
            DSP_MW_LOG_I("[A2DP] Ch_Select_Init, ch mode = %d\n", 1, ch_sel_ctrl.ch_mode);
        }
    } else if (type == CH_SEL_HFP) {
        if(ch_sel_ctrl_hfp.ch_mode >= CH_SEL_NUM) {
            ch_sel_ctrl_hfp.ch_mode = CH_SEL_STEREO;
            DSP_MW_LOG_W("[HFP] Ch_Select_Init ch mode %d -> %d\n", 2, ch_sel_ctrl_hfp.ch_mode, CH_SEL_BOTH_L);
        } else {
            //DSP_MW_LOG_I("[HFP] Ch_Select_Init, ch mode = %d\n", 1, ch_sel_ctrl_hfp.ch_mode);
        }
    } else if (type == CH_SEL_VP) {
        if(ch_sel_ctrl_vp.ch_mode >= CH_SEL_NUM) {
            ch_sel_ctrl_vp.ch_mode = CH_SEL_STEREO;
            DSP_MW_LOG_W("[VP] Ch_Select_Init ch mode %d -> %d\n", 2, ch_sel_ctrl_vp.ch_mode, CH_SEL_STEREO);
        } else {
            DSP_MW_LOG_I("[VP] Ch_Select_Init, ch mode = %d\n", 1, ch_sel_ctrl_vp.ch_mode);
        }
    }

    return FALSE;
}

/**
 * stream_function_channel_selector_process_a2dp
 *
 * Channel Select main process for a2dp
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_channel_selector_process_a2dp (void *para)
{
    stream_function_channel_selector_process(para, CH_SEL_A2DP);
    return FALSE;
}


/**
 * stream_function_channel_selector_process_hfp
 *
 * Channel Select main process for hfp
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_channel_selector_process_hfp (void *para)
{
    stream_function_channel_selector_process(para, CH_SEL_HFP);
    return FALSE;
}

/**
 * stream_function_channel_selector_process_vp
 *
 * Channel Select main process for vp
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_channel_selector_process_vp (void *para)
{
    stream_function_channel_selector_process(para, CH_SEL_VP);
    return FALSE;
}

/**
 * stream_function_channel_selector_process
 *
 * Channel Select main process
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_channel_selector_process (void *para, CH_SEL_SCENARIO_TYPE type)
{
    S16 Channels = (S16)stream_function_get_channel_number(para);

    if(type == CH_SEL_A2DP)
    {
        if(ch_sel_ctrl.ch_mode == CH_SEL_MONO)
        {
            U16 FrameSize = stream_function_get_output_size(para);
            U8 proc_resolution = (U8)stream_function_get_output_resolution(para);
            S32 *BufL = (S32*)stream_function_get_1st_inout_buffer(para);
            S32 *BufR = (S32*)stream_function_get_2nd_inout_buffer(para);
            U32 i, samples;
            if ((BufL != NULL) && (BufR != NULL)) {
                if(proc_resolution == RESOLUTION_16BIT)
                {
                    S16 *Buf1 = (S16*)BufL;
                    S16 *Buf2 = (S16*)BufR;
                    samples = FrameSize >> 1;
                    for(i = 0; i < samples; i++)
                    {
                        *Buf1 = (((*Buf1) >> 1) + ((*Buf2) >> 1));
                        Buf1++;
                        Buf2++;
                    }
                } else {
                    samples = FrameSize >> 2;
                    for(i = 0; i < samples; i++)
                    {
                        *BufL = (((*BufL) >> 1) + ((*BufR) >> 1));
                        BufL++;
                        BufR++;
                    }
                }
                memcpy(stream_function_get_2nd_inout_buffer(para), stream_function_get_1st_inout_buffer(para), FrameSize);
            }
        } else if (ch_sel_ctrl.ch_mode == CH_SEL_BOTH_L) {
            U16 FrameSize = stream_function_get_output_size(para);
            if (stream_function_get_2nd_inout_buffer(para) != NULL)
            {
                memcpy(stream_function_get_2nd_inout_buffer(para),stream_function_get_1st_inout_buffer(para),FrameSize);
            }
        } else if (ch_sel_ctrl.ch_mode == CH_SEL_BOTH_R) {
            U16 FrameSize = stream_function_get_output_size(para);
            if (stream_function_get_2nd_inout_buffer(para) != NULL)
            {
                memcpy(stream_function_get_1st_inout_buffer(para),stream_function_get_2nd_inout_buffer(para),FrameSize);
            }
        }
     } else if((type == CH_SEL_HFP) && (Channels >= 3)) {
         if(ch_sel_ctrl_hfp.ch_mode == CH_SEL_BOTH_R) {
             U16 FrameSize = stream_function_get_output_size(para);
             if (stream_function_get_2nd_inout_buffer(para) != NULL) {
                 memcpy(stream_function_get_1st_inout_buffer(para),stream_function_get_2nd_inout_buffer(para),FrameSize);
             }
             //DSP_MW_LOG_I("[HFP][ch select] CH_SEL_BOTH_R\n", 0);
         } else if(ch_sel_ctrl_hfp.ch_mode == CH_SEL_BOTH_L){
             U16 FrameSize = stream_function_get_output_size(para);
             if (stream_function_get_2nd_inout_buffer(para) != NULL) {
                 memcpy(stream_function_get_2nd_inout_buffer(para),stream_function_get_1st_inout_buffer(para),FrameSize);
             }
             //DSP_MW_LOG_I("[HFP][ch select] CH_SEL_BOTH_L\n", 0);
#ifdef ENABLE_2A2D_TEST
         } else if(ch_sel_ctrl_hfp.ch_mode == CH_SEL_BOTH_Ref_Mic3){
             U16 FrameSize = stream_function_get_output_size(para);
             if (stream_function_get_3rd_inout_buffer(para) != NULL) {
                 memcpy(stream_function_get_1st_inout_buffer(para),stream_function_get_3rd_inout_buffer(para),FrameSize);
             }
             //DSP_MW_LOG_I("[HFP][ch select] CH_SEL_BOTH_Ref_Mic3\n", 0);
#ifdef MTK_BT_HFP_SPE_ALG_V2
         } else if(ch_sel_ctrl_hfp.ch_mode == CH_SEL_BOTH_Ref_Mic4){
             U16 FrameSize = stream_function_get_output_size(para);
             if (stream_function_get_4th_inout_buffer(para) != NULL) {
                 memcpy(stream_function_get_1st_inout_buffer(para),stream_function_get_4th_inout_buffer(para),FrameSize);
             }
             DSP_MW_LOG_I("[HFP][ch select] CH_SEL_BOTH_Ref_Mic4\n", 0);
         } else if(ch_sel_ctrl_hfp.ch_mode == CH_SEL_BOTH_Ref_Mic5){
             U16 FrameSize = stream_function_get_output_size(para);
             if (stream_function_get_5th_inout_buffer(para) != NULL) {
                 memcpy(stream_function_get_1st_inout_buffer(para),stream_function_get_5th_inout_buffer(para),FrameSize);
             }
             DSP_MW_LOG_I("[HFP][ch select] CH_SEL_BOTH_Ref_Mic5\n", 0);
         } else if(ch_sel_ctrl_hfp.ch_mode == CH_SEL_BOTH_Ref_Mic6){
             U16 FrameSize = stream_function_get_output_size(para);
             if (stream_function_get_6th_inout_buffer(para) != NULL) {
                 memcpy(stream_function_get_1st_inout_buffer(para),stream_function_get_6th_inout_buffer(para),FrameSize);
             }
             DSP_MW_LOG_I("[HFP][ch select] CH_SEL_BOTH_Ref_Mic6\n", 0);
#endif /*MTK_BT_HFP_SPE_ALG_V2*/
#endif
         } else if(ch_sel_ctrl_hfp.ch_mode == CH_SEL_STEREO){
             //DSP_MW_LOG_I("[HFP][ch select] CH_SEL_STEREO\n", 0);
         }
    } else if(type == CH_SEL_VP) {
        switch(ch_sel_ctrl_vp.ch_mode){
            case CH_SEL_MONO:{
                U16 FrameSize = stream_function_get_output_size(para);
                U8 proc_resolution = (U8)stream_function_get_output_resolution(para);
                S32 *BufL = (S32*)stream_function_get_1st_inout_buffer(para);
                S32 *BufR = (S32*)stream_function_get_2nd_inout_buffer(para);
                U32 i, samples;
                if ((BufL != NULL) && (BufR != NULL)) {
                    if(proc_resolution == RESOLUTION_16BIT)
                    {
                        S16 *Buf1 = (S16*)BufL;
                        S16 *Buf2 = (S16*)BufR;
                        samples = FrameSize >> 1;
                        for(i = 0; i < samples; i++)
                        {
                            *Buf1 = (((*Buf1) >> 1) + ((*Buf2) >> 1));
                            Buf1++;
                            Buf2++;
                        }
                    } else {
                        samples = FrameSize >> 2;
                        for(i = 0; i < samples; i++)
                        {
                            *BufL = (((*BufL) >> 1) + ((*BufR) >> 1));
                            BufL++;
                            BufR++;
                        }
                    }
                    memcpy(stream_function_get_2nd_inout_buffer(para), stream_function_get_1st_inout_buffer(para), FrameSize);
                }
                break;
            }
            case CH_SEL_BOTH_L:{
                U16 FrameSize = stream_function_get_output_size(para);
                if (stream_function_get_2nd_inout_buffer(para) != NULL)
                {
                    memcpy(stream_function_get_2nd_inout_buffer(para),stream_function_get_1st_inout_buffer(para),FrameSize);
                }
                break;
            }
            case CH_SEL_BOTH_R:{
                U16 FrameSize = stream_function_get_output_size(para);
                if (stream_function_get_2nd_inout_buffer(para) != NULL)
                {
                    memcpy(stream_function_get_1st_inout_buffer(para),stream_function_get_2nd_inout_buffer(para),FrameSize);
                }
                break;
            }
            default:
                break;
        }
    }

    return FALSE;
}


/**
 * Ch_Select_Set_Param
 *
 * Set the channel mode for channel selection
 *
 *
 * @para : Default parameter of callback function
 *
 */
void Ch_Select_Set_Param (hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);
    CH_SEL_MODE ch_mode;
    uint16_t scenario;
    scenario = (msg.ccni_message[0] & 0x00ff);

    if(scenario == CH_SEL_A2DP)
    {
        switch(msg.ccni_message[1])
        {
            case AUDIO_DSP_CHANNEL_SELECTION_STEREO:
                ch_mode = CH_SEL_STEREO;
                break;
            case AUDIO_DSP_CHANNEL_SELECTION_MONO:
                ch_mode = CH_SEL_MONO;
                break;
            case AUDIO_DSP_CHANNEL_SELECTION_BOTH_L:
                ch_mode = CH_SEL_BOTH_L;
                break;
            case AUDIO_DSP_CHANNEL_SELECTION_BOTH_R:
                ch_mode = CH_SEL_BOTH_R;
                break;
            default:
                ch_mode = CH_SEL_STEREO;
                break;
        }
        if(ch_sel_ctrl.ch_mode != ch_mode)
        {
            DSP_MW_LOG_I("[A2DP][ch select] mode changes: %d -> %d\n", 2, ch_sel_ctrl.ch_mode, ch_mode);
            ch_sel_ctrl.ch_mode = ch_mode;
        }
    }else if(scenario == CH_SEL_HFP)
    {
       switch(msg.ccni_message[1])
       {
            case AUDIO_DSP_CHANNEL_SELECTION_STEREO:
                ch_mode = CH_SEL_STEREO;
                break;
            case AUDIO_DSP_CHANNEL_SELECTION_BOTH_R:
                ch_mode = CH_SEL_BOTH_R;
                break;
            case AUDIO_DSP_CHANNEL_SELECTION_BOTH_L:
                ch_mode = CH_SEL_BOTH_L;
                break;
#ifdef ENABLE_2A2D_TEST
            case AUDIO_DSP_CHANNEL_SELECTION_REF_MIC_3:
                ch_mode = CH_SEL_BOTH_Ref_Mic3;
                break;
#ifdef MTK_BT_HFP_SPE_ALG_V2
            case AUDIO_DSP_CHANNEL_SELECTION_REF_MIC_4:
                ch_mode = CH_SEL_BOTH_Ref_Mic4;
                break;
            case AUDIO_DSP_CHANNEL_SELECTION_REF_MIC_5:
                ch_mode = CH_SEL_BOTH_Ref_Mic5;
                break;
            case AUDIO_DSP_CHANNEL_SELECTION_REF_MIC_6:
                ch_mode = CH_SEL_BOTH_Ref_Mic6;
                break;
#endif /*MTK_BT_HFP_SPE_ALG_V2*/
#endif
            default:
                ch_mode = CH_SEL_STEREO;
                break;
       }
        if(ch_sel_ctrl_hfp.ch_mode != ch_mode)
        {
            DSP_MW_LOG_I("[HFP][ch select] mode changes: %d -> %d\n", 2, ch_sel_ctrl_hfp.ch_mode, ch_mode);
            ch_sel_ctrl_hfp.ch_mode = ch_mode;
        }
    }else if(scenario == CH_SEL_VP){
        switch(msg.ccni_message[1])
        {
            case AUDIO_DSP_CHANNEL_SELECTION_STEREO:
                ch_mode = CH_SEL_STEREO;
                break;
            case AUDIO_DSP_CHANNEL_SELECTION_MONO:
                ch_mode = CH_SEL_MONO;
                break;
            case AUDIO_DSP_CHANNEL_SELECTION_BOTH_L:
                ch_mode = CH_SEL_BOTH_L;
                break;
            case AUDIO_DSP_CHANNEL_SELECTION_BOTH_R:
                ch_mode = CH_SEL_BOTH_R;
                break;
            default:
                ch_mode = CH_SEL_STEREO;
                break;
        }
        if(ch_sel_ctrl_vp.ch_mode != ch_mode)
        {
            DSP_MW_LOG_I("[VP][ch select] mode changes: %d -> %d\n", 2, ch_sel_ctrl_vp.ch_mode, ch_mode);
            ch_sel_ctrl_vp.ch_mode = ch_mode;
        }
    } else {
       DSP_MW_LOG_I("[??][ch select] no this kind of scenario: %d \n", 1, scenario);
    }
}


/**
 * Ch_Select_Get_Param
 *
 * Get the channel mode for channel selection
 *
 *
 * @scenario : scneario of channel select, CH_SEL_A2DP for A2DP, CH_SEL_HFP for eSCO
 *
 */
CH_SEL_MODE Ch_Select_Get_Param (CH_SEL_SCENARIO_TYPE scenario)
{
    if (scenario == CH_SEL_A2DP) {
        return ch_sel_ctrl.ch_mode;
    } else if (scenario == CH_SEL_HFP) {
        return ch_sel_ctrl_hfp.ch_mode;
    } else if (scenario == CH_SEL_VP) {
        return ch_sel_ctrl_vp.ch_mode;
    } else {
        DSP_MW_LOG_I("[ch select] unknown scenario for channel get: %d \n", 1, scenario);
        return CH_SEL_STEREO;
    }
}


