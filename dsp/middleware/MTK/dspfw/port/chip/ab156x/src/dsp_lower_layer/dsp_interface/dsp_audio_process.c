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
#include "dsp_audio_process.h"
#include "dsp_feature_interface.h"
#include "dsp_callback.h"
#include "dsp_drv_dfe.h"
#include "sfr_au_src.h"
#include "dsp_buffer.h"
#include "dsp_memory.h"
#include "dsp_dump.h"
#include "stream_audio_driver.h"
#include <string.h>


#ifdef MTK_HWSRC_IN_STREAM
//modify for asrc
#include "hal_clock_internal.h"

typedef BOOL (*SRC_HANDLE_ENTRY) (VOID* , VOID* , VOID* , U32);
STATIC SRC_HANDLE_ENTRY SRC_Handle_Entry;
#endif
/******************************************************************************
 * CONSTANT DEFINITIONS
 ******************************************************************************/
#define CALLBACK_SRC_FLOAT_PROTECTION 5
#define ProcessSize 512
#define MAX_CHANNELS 2

/******************************************************************************
 * Function Declaration
 ******************************************************************************/
#ifdef AIR_HFP_DNN_PATH_ENABLE
void gain_change_detect(U8 mic_map_table);
#endif


/******************************************************************************
 * Variables
 ******************************************************************************/



/**
 * @brief Copies the values of num sample from 16-bit resulation data
 * to 24-bit resulation data.
 *
 * @param des     Pointer to the destination array where the content is to be copied.
 * @param src     Pointer to the source of data to be copied.
 * @param sample  Number of sample to copy.
 */
VOID DSP_Converter_16Bit_to_24bit(
          S32* des,
          S16* src,
          U32  sample)
{
        U32 i = 0;

        if(des == NULL || src == NULL)
            return;
#if 0
        des+=(sample-1);
        src+=(sample-1);

        for(i=0;i<sample;i++)
        {
            *des = *src;
            *des<<=8;
            des--;
            src--;
        }
#else

        ae_p16x2s*  ae_src = (ae_p16x2s*)(src + sample);
        ae_p24x2s*  ae_des = (ae_p24x2s*)(des + sample);
        ae_p24x2s p_tmp = AE_CVTP24A16(0);

        for(i=0;i<sample;i+=2)
        {
            AE_LP16X2F_IU (p_tmp,ae_src,-4);
            AE_SP24X2S_IU (p_tmp,ae_des,-8);

        }
#endif
}

/**
 * @brief Copies the values of num sample from 16-bit resulation data
 * to 32-bit resulation data.
 *
 * @param des     Pointer to the destination array where the content is to be copied.
 * @param src     Pointer to the source of data to be copied.
 * @param sample  Number of sample to copy.
 */
VOID dsp_converter_16bit_to_32bit(
          S32* des,
          S16* src,
          U32  sample)
{
        U32 i;

        if(des == NULL || src == NULL)
            return;

        ae_p16x2s*  ae_src = (ae_p16x2s*)(src + sample);
        ae_p24x2s*  ae_des = (ae_p24x2s*)(des + sample);
        ae_p24x2s   p_tmp = AE_CVTP24A16(0);

        for(i=0;i<sample;i+=2)
        {
            AE_LP16X2F_IU (p_tmp,ae_src,-4);
            AE_SP24X2F_IU (p_tmp,ae_des,-8);

        }
}

/**
 * @brief Copies the values of num sample from 32-bit resulation data
 * to 16-bit resulation data.
 *
 * @param des     Pointer to the destination array where the content is to be copied.
 * @param src     Pointer to the source of data to be copied.
 * @param sample  Number of sample to copy.
 */
VOID dsp_converter_32bit_to_16bit(
          S16* des,
          S32* src,
          U32  sample)
{
        U32 i;

        if(des == NULL || src == NULL)
            return;

        ae_p24x2s*  ae_src = (ae_p24x2s*)src;
        ae_p16x2s*  ae_des = (ae_p16x2s*)des;
        ae_p24x2s   p_tmp = AE_CVTP24A16(0);

        ae_src--;
        ae_des--;
        for(i=0;i<sample;i+=2)
        {
            AE_LP24X2F_IU (p_tmp,ae_src,8);
            AE_SP16X2F_IU (p_tmp,ae_des,4);
        }
}

/**
 * @brief Gain Adjust 16-bit data.
 *
 * @param src     Pointer to the source of data to be adjusted.
 * @param sample  Number of sample to copy.
 * @param gain    Adjust Gain .
 */
VOID DSP_GainAdjust_16bit(
          S16* src,
          U32  sample,
          S16  gain)
{
    U32 i;
    S32 temp;
    //TEMP!!
    if (gain != VAL_1_IN_Q15FORMAT)
    {
        for(i=0 ; i<sample ; i++)
        {
            temp = (S32)*(src+i)*gain/VAL_1_IN_Q15FORMAT;
            *(src+i) = (S16)temp;
        }
    }
}

VOID DSP_GainAdjust_32bit(
          S32* src,
          U32  sample,
          S16  gain)
{
    S64 temp;
    U32 i;
    //TEMP!!
    if (gain != VAL_1_IN_Q15FORMAT)
    {
        for(i=0 ; i<sample ; i++)
        {
            temp = (S64)*(src+i)*gain/VAL_1_IN_Q15FORMAT;
            *(src+i) = (S32)temp;
        }
    }
}

VOID DSP_D2I_BufferCopy_16bit(U16* DestBuf,
                              U16* SrcBuf1,
                              U16* SrcBuf2,
                              U16  SampleSize)
{
#if 1
    int i;

    for (i = 0; i < SampleSize; i++)
    {
        DestBuf[i * 2    ] = SrcBuf1[i];
        DestBuf[i * 2 + 1] = SrcBuf2[i];
    }

#else
    ae_p16x2s*  ae_src1    = (ae_p16x2s*)(SrcBuf1 - 2);
    ae_p16x2s*  ae_src2    = (ae_p16x2s*)(SrcBuf2 - 2);
    ae_p16x2s*  ae_DestBuf = (ae_p16x2s*)(DestBuf - 2);


    int i;
    ae_p24x2s   p_tmp1;
    ae_p24x2s   p_tmp2;
    ae_p24x2s   p_tmp3;
    ae_p24x2s   p_tmp4;

    for (i = 0; i < SampleSize/2; i++)
    {
        AE_LP16X2F_IU (p_tmp1, ae_src1, 4);
        AE_LP16X2F_IU (p_tmp2, ae_src2, 4);

        p_tmp3 = AE_SELP24_HH (p_tmp1, p_tmp2);
        p_tmp4 = AE_SELP24_LL (p_tmp1, p_tmp2);

        AE_SP16X2F_IU (p_tmp3, ae_DestBuf, 4);
        AE_SP16X2F_IU (p_tmp4, ae_DestBuf, 4);
    }
#endif
}

ATTR_TEXT_IN_IRAM_LEVEL_2 VOID DSP_I2D_BufferCopy_16bit(U16* SrcBuf,
                              U16* DestBuf1,
                              U16* DestBuf2,
                              U16  SampleSize)
{
    int i;
    if(!(DestBuf2 == NULL))
    {
       for (i = 0; i < SampleSize; i++)
       {
           DestBuf1[i] = SrcBuf[i * 2];
           DestBuf2[i] = SrcBuf[i * 2 + 1];
       }
    }
    else
    {
       for (i = 0; i < SampleSize; i++)
       {
           DestBuf1[i] = SrcBuf[i * 2];
       }
    }
}

ATTR_TEXT_IN_IRAM_LEVEL_2 VOID DSP_I2D_BufferCopy_16bit_mute(U16* SrcBuf,
                              U16* DestBuf1,
                              U16* DestBuf2,
                              U16  SampleSize,
                              BOOL muteflag)
{
    int i;
    if(muteflag == TRUE)
    {
       if(!(DestBuf2 == NULL)){
          for (i = 0; i < SampleSize; i++){
              DestBuf1[i] = 0;
              DestBuf2[i] = 0;
          }
       }else{
          for (i = 0; i < SampleSize; i++){
              DestBuf1[i] = 0;
          }
       }
    }else{
       if(!(DestBuf2 == NULL)){
          for (i = 0; i < SampleSize; i++){
              DestBuf1[i] = SrcBuf[i * 2];
              DestBuf2[i] = SrcBuf[i * 2 + 1];
          }
       }else{
          for (i = 0; i < SampleSize; i++){
              DestBuf1[i] = SrcBuf[i * 2];
          }
       }
    }
}

ATTR_TEXT_IN_IRAM VOID DSP_D2I_BufferCopy_32bit(U32* DestBuf,
                              U32* SrcBuf1,
                              U32* SrcBuf2,
                              U32  SampleSize)
{
#if 1
    U32 i;
    for (i = 0; i < SampleSize; i++)
    {
        DestBuf[i * 2    ] = SrcBuf1[i];
        DestBuf[i * 2 + 1] = SrcBuf2[i];
    }

#else // need 16 byte align
    ae_p24x2s*  ae_src1    = (ae_p24x2s*)(SrcBuf1 - 2);
    ae_p24x2s*  ae_src2    = (ae_p24x2s*)(SrcBuf2 - 2);
    ae_p24x2s*  ae_DestBuf = (ae_p24x2s*)(DestBuf - 2);


    int i;
    ae_p24x2s   p_tmp1;
    ae_p24x2s   p_tmp2;
    ae_p24x2s   p_tmp3;
    ae_p24x2s   p_tmp4;

    for (i = 0; i < SampleSize/2; i++)
    {
        AE_LP24X2F_IU (p_tmp1, ae_src1, 8);
        AE_LP24X2F_IU (p_tmp2, ae_src2, 8);

        p_tmp3 = AE_SELP24_HH (p_tmp1, p_tmp2);
        p_tmp4 = AE_SELP24_LL (p_tmp1, p_tmp2);

        AE_SP24X2F_IU (p_tmp3, ae_DestBuf, 8);
        AE_SP24X2F_IU (p_tmp4, ae_DestBuf, 8);
    }
    DestBuf[SampleSize * 2 - 2] = SrcBuf1[SampleSize -1];
    DestBuf[SampleSize * 2 - 1] = SrcBuf2[SampleSize -1];
#endif
}

VOID DSP_I2D_BufferCopy_32bit(U32* SrcBuf,
                                     U32* DestBuf1,
                                     U32* DestBuf2,
                                     U16  SampleSize)
{
    int i;
    for (i = 0; i < SampleSize; i++)
    {
        DestBuf1[i] = SrcBuf[i * 2];
        DestBuf2[i] = SrcBuf[i * 2 + 1];
    }
}

VOID DSP_I2D_BufferCopy_32bit_mute(U32* SrcBuf,
                                     U32* DestBuf1,
                                     U32* DestBuf2,
                                     U16  SampleSize,
                                     BOOL muteflag)
{
    int i;
    if(muteflag == TRUE)
    {
       if(!(DestBuf2 == NULL)){
          for (i = 0; i < SampleSize; i++){
              DestBuf1[i] = 0;
              DestBuf2[i] = 0;
          }
       }else{
          for (i = 0; i < SampleSize; i++){
              DestBuf1[i] = 0;
          }
       }
    }else{
       if(!(DestBuf2 == NULL)){
          for (i = 0; i < SampleSize; i++){
              DestBuf1[i] = SrcBuf[i * 2];
              DestBuf2[i] = SrcBuf[i * 2 + 1];
          }
       }else{
          for (i = 0; i < SampleSize; i++){
              DestBuf1[i] = SrcBuf[i * 2];
          }
       }
    }
}

VOID DSP_Fade_Process(Audio_Fade_Ctrl_Ptr fade_ctrl_ptr, U8 *src_addr, U32 length)
{
    U32 remainLength = length;
    U32 processLength;
    while(remainLength!=0)
    {
        if (fade_ctrl_ptr->Target_Level == fade_ctrl_ptr->Current_Level)
        {
            processLength = remainLength;
        }
        else
        {
            if (fade_ctrl_ptr->Resolution==RESOLUTION_16BIT)
            {
                processLength = sizeof(S16);
            }
            else
            {
                processLength = sizeof(S32);
            }
        }

        if (fade_ctrl_ptr->Resolution==RESOLUTION_16BIT)
        {
            DSP_GainAdjust_16bit((S16*)src_addr, processLength/sizeof(S16), fade_ctrl_ptr->Current_Level);
        }
        else
        {
            DSP_GainAdjust_32bit((S32*)src_addr, processLength/sizeof(S32), fade_ctrl_ptr->Current_Level);
        }

        if(fade_ctrl_ptr->Step>0)
        {
            if ((fade_ctrl_ptr->Current_Level+fade_ctrl_ptr->Step)<fade_ctrl_ptr->Target_Level)
            {
                fade_ctrl_ptr->Current_Level+=fade_ctrl_ptr->Step;
            }
            else
            {
                fade_ctrl_ptr->Current_Level = fade_ctrl_ptr->Target_Level;
            }
        }
        else
        {
            if ((fade_ctrl_ptr->Current_Level+fade_ctrl_ptr->Step)>fade_ctrl_ptr->Target_Level)
            {
                fade_ctrl_ptr->Current_Level+=fade_ctrl_ptr->Step;
            }
            else
            {
                fade_ctrl_ptr->Current_Level = fade_ctrl_ptr->Target_Level;
            }
        }
        src_addr += processLength;
        remainLength -= processLength;
    }
}

/**
 * stream_pcm_copy_process
 *
 * Copy callback in_ptr to out_ptr
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
bool stream_pcm_copy_process(void *para)
{
    U32 i, in_ChNum;
    in_ChNum = 0;


    stream_codec_modify_output_size(para, stream_codec_get_input_size(para));
    stream_codec_modify_output_samplingrate(para, stream_codec_get_input_samplingrate(para));


    for (i=0 ; i<stream_codec_get_output_channel_number(para) ; i++)
    {
        if (i != in_ChNum) {
            memcpy(stream_codec_get_output_buffer(para, i+1),
                stream_codec_get_input_buffer(para, in_ChNum+1),
                stream_codec_get_output_size(para));
        }
        in_ChNum++;
        in_ChNum %= stream_codec_get_input_channel_number(para);
    }

    if(stream_codec_get_input_resolution(para) != stream_codec_get_output_resolution(para))
    {
        if(stream_codec_get_input_resolution(para)==RESOLUTION_16BIT)
        {
            for (i=0 ; i<stream_codec_get_output_channel_number(para) ; i++)
            {
                dsp_converter_16bit_to_32bit((S32*)stream_codec_get_output_buffer(para, i+1),
                                             (S16*)stream_codec_get_output_buffer(para, i+1),
                                             stream_codec_get_output_size(para)/sizeof(S16));
            }
            stream_codec_modify_output_size(para, stream_codec_get_output_size(para)*2);
        }
        else
        {
            for (i=0 ; i<stream_codec_get_output_channel_number(para) ; i++)
            {
                dsp_converter_32bit_to_16bit((S16*)stream_codec_get_output_buffer(para, i+1),
                                             (S32*)stream_codec_get_output_buffer(para, i+1),
                                             stream_codec_get_output_size(para)/sizeof(S32));
            }
            stream_codec_modify_output_size(para, stream_codec_get_output_size(para)/2);
        }
    }
    stream_codec_modify_resolution(para, stream_codec_get_output_resolution(para));
    return 0;
}
bool stream_function_size_converter_initialize(void *para)
{
    //Target IP init process
    UNUSED(para);
    return 0;
}



bool stream_function_size_converter_process(void *para)
{
    //S16* BufL = (S16*)stream_function_get_1st_inout_buffer(para);
    //S16* BufR = (S16*)stream_function_get_2nd_inout_buffer(para);
	U16 FrameSize = stream_function_get_output_size(para);

    U16 ProcessTimes,i;
    bool status = FALSE;
    if(FrameSize == 0){
        return FALSE;
    }
    else if ((FrameSize % ProcessSize) != 0){
        //printf("Warning:Process size not a factor of codec out frame size");
    }
    ProcessTimes = (FrameSize /ProcessSize);
    for (i = 0 ; i< ProcessTimes ; i++)
    {
    	//  ((status =  Target_IP_process(stream_codec_get_workingbuffer(para), (U8*)InBuf + i*Target IP init, (U8*)OutBuf + i*240, 0)))
        /*bool Target_IP_process (InstancePtr ptr, S16* buffer_L_Channel,  S16* buffer_R_Channel, S16 frame_size ) */
        //if((status =  Target_IP_process(stream_codec_get_workingbuffer(para), (U8*)BufL + i*ProcessSize, (U8*)BufL + i*ProcessSize, ProcessSize)))
            //{break;}
    }
    return status;
}



BOOL UART2AudioInit(VOID* para)
{
    memset(stream_codec_get_1st_input_buffer(para) , 0, stream_codec_get_input_size(para) );
    memset(stream_codec_get_1st_output_buffer(para), 0, stream_codec_get_output_size(para));
    if(stream_codec_get_output_channel_number(para) >1)
        memset(stream_codec_get_2nd_output_buffer(para), 0, stream_codec_get_output_size(para));
    stream_codec_modify_output_samplingrate(para, FS_RATE_48K);
    return 0;
}

BOOL UART2AudioCodec(VOID* para)
{
    U16 i = 0;
    U16 y = 0;
    for (i = 0;i < stream_codec_get_input_size(para) ;)
    {
        ((U8*)stream_codec_get_1st_output_buffer(para))[y+0] = ((U8*)stream_codec_get_1st_input_buffer(para))[i+0];
        ((U8*)stream_codec_get_1st_output_buffer(para))[y+2] = ((U8*)stream_codec_get_1st_input_buffer(para))[i+1];
        ((U8*)stream_codec_get_1st_output_buffer(para))[y+3] = ((U8*)stream_codec_get_1st_input_buffer(para))[i+2];

        ((U8*)stream_codec_get_2nd_output_buffer(para))[y+0] = ((U8*)stream_codec_get_1st_input_buffer(para))[i+3];
        ((U8*)stream_codec_get_2nd_output_buffer(para))[y+2] = ((U8*)stream_codec_get_1st_input_buffer(para))[i+4];
        ((U8*)stream_codec_get_2nd_output_buffer(para))[y+3] = ((U8*)stream_codec_get_1st_input_buffer(para))[i+5];

        y = y + 4;
        i = i + 6;
    }

    stream_codec_modify_output_size(para, stream_codec_get_input_size(para)*2/3);
    return 0;
}
BOOL UART2AudioCodec_16bit(VOID* para)
{
    U16 i = 0;
    U16 y = 0;
    for (i = 0;i < stream_codec_get_input_size(para) ;)
    {
        ((U8*)stream_codec_get_1st_output_buffer(para))[y+0] = ((U8*)stream_codec_get_1st_input_buffer(para))[i+0];
        ((U8*)stream_codec_get_1st_output_buffer(para))[y+1] = ((U8*)stream_codec_get_1st_input_buffer(para))[i+1];

        ((U8*)stream_codec_get_2nd_output_buffer(para))[y+0] = ((U8*)stream_codec_get_1st_input_buffer(para))[i+2];
        ((U8*)stream_codec_get_2nd_output_buffer(para))[y+1] = ((U8*)stream_codec_get_1st_input_buffer(para))[i+3];

        y = y + 2;
        i = i + 4;
    }
    stream_codec_modify_output_size(para, stream_codec_get_input_size(para)/2);
    return 0;
}
BOOL Audio2DataStreamCodec(VOID* para)
{

    U16 i = 0;
    U16 y = 0;
    for (i = 0;i < stream_codec_get_input_size(para) ;)
    {
        ((U8*)stream_codec_get_1st_output_buffer(para))[y+0] = ((U8*)stream_codec_get_1st_input_buffer(para))[i+0];
        ((U8*)stream_codec_get_1st_output_buffer(para))[y+1] = ((U8*)stream_codec_get_1st_input_buffer(para))[i+2];
        ((U8*)stream_codec_get_1st_output_buffer(para))[y+2] = ((U8*)stream_codec_get_1st_input_buffer(para))[i+3];

        ((U8*)stream_codec_get_1st_output_buffer(para))[y+3] = ((U8*)stream_codec_get_2nd_input_buffer(para))[i+0];
        ((U8*)stream_codec_get_1st_output_buffer(para))[y+4] = ((U8*)stream_codec_get_2nd_input_buffer(para))[i+2];
        ((U8*)stream_codec_get_1st_output_buffer(para))[y+5] = ((U8*)stream_codec_get_2nd_input_buffer(para))[i+3];

        y = y + 6;
        i = i + 4;
    }

    return 0;
}

#ifdef MTK_HWSRC_IN_STREAM
extern bool hal_audio_src_set_start(afe_src_configuration_t *configuration, hal_audio_memory_sync_selection_t sync_select, hal_audio_control_status_t control);
extern bool hal_audio_src_configuration(afe_src_configuration_t *configuration, hal_audio_control_status_t control);

void DSP_Set_ASRC_Configuration_Parameters(VOID* para){
    DSP_MW_LOG_I("Set_ASRC_Configuration_Parameters\n", 0);
    DSP_SRC_FEATURE_PTR  src_feature_ptr;
    DSP_STREAMING_PARA_PTR stream_ptr;
    S32 nch = stream_function_get_channel_number(para);
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    src_feature_ptr = stream_function_get_working_buffer(para);
    afe_src_configuration_t src_configuration;



    memset(&src_configuration,0,sizeof(src_configuration));

    src_configuration.id = AFE_SRC_1;
    src_configuration.input_buffer.addr = hal_memview_dsp0_to_infrasys((uint32_t)src_feature_ptr->inSRC_mem_ptr);
    src_configuration.input_buffer.offset = 0;
    src_configuration.input_buffer.rate = DSP_FsChange2SRCInRate((stream_samplerate_t)(U8)stream_function_get_samplingrate(para));
    src_configuration.input_buffer.size = src_feature_ptr->inSRC_mem_size;
    src_configuration.is_mono = (stream_function_get_channel_number(para)<2);
    src_configuration.mode = AFE_SRC_NO_TRACKING;
    src_configuration.output_buffer.addr = hal_memview_dsp0_to_infrasys((uint32_t)src_feature_ptr->outSRC_mem_ptr);
    src_configuration.output_buffer.offset = stream_ptr->callback.Src.out_frame_size*nch+64;
    src_configuration.output_buffer.rate = DSP_FsChange2SRCOutRate((stream_samplerate_t)(U8)stream_ptr->callback.Src.out_sampling_rate);
    src_configuration.output_buffer.size = src_feature_ptr->outSRC_mem_size;
    src_configuration.ul_mode = false;
    if(DSP_RsChange2SRCOutRs(stream_ptr->callback.Src.in_resolution)==4){
        src_configuration.input_buffer.format = AFE_PCM_FORMAT_S32_LE;
        src_configuration.output_buffer.format  = AFE_PCM_FORMAT_S32_LE;
    }else{
        src_configuration.input_buffer.format =  AFE_PCM_FORMAT_S16_LE;
        src_configuration.output_buffer.format =  AFE_PCM_FORMAT_S16_LE;
    }

    hal_audio_src_configuration(&src_configuration, HAL_AUDIO_CONTROL_ON);
    DSP_MW_LOG_I("+ ASRC_EN %d\n", 1,(AFE_GET_REG(ASM_GEN_CONF + AFE_MEM_ASRC_1)&ASM_GEN_CONF_ASRC_BUSY_MASK));

    hal_audio_src_set_start(&src_configuration, HAL_AUDIO_MEMORY_SYNC_NONE, HAL_AUDIO_CONTROL_ON);
    DSP_MW_LOG_I("- ASRC_EN %d\n", 1,(AFE_GET_REG(ASM_GEN_CONF + AFE_MEM_ASRC_1)&ASM_GEN_CONF_ASRC_BUSY_MASK));

    DSP_MW_LOG_I("Set_ASRC src_feature_ptr->inSRC_mem_ptr 0x%x\n", 1,src_feature_ptr->inSRC_mem_ptr);
    DSP_MW_LOG_I("Set_ASRC ASM_IBUF_SADR 0x%x\n", 1,AFE_GET_REG(ASM_IBUF_SADR));
    DSP_MW_LOG_I("Set_ASRC src_feature_ptr->inSRC_mem_size %d\n", 1,src_feature_ptr->inSRC_mem_size);
    DSP_MW_LOG_I("Set_ASRC ASM_IBUF_SIZE %d\n", 1,AFE_GET_REG(ASM_IBUF_SIZE));
    DSP_MW_LOG_I("Set_ASRC src_feature_ptr->outSRC_mem_ptr 0x%x\n", 1,src_feature_ptr->outSRC_mem_ptr);
    DSP_MW_LOG_I("Set_ASRC ASM_OBUF_SADR 0x%x\n", 1,AFE_GET_REG(ASM_OBUF_SADR));
    DSP_MW_LOG_I("Set_ASRC src_feature_ptr->outSRC_mem_size %d\n", 1,src_feature_ptr->outSRC_mem_size);
    DSP_MW_LOG_I("Set_ASRC ASM_OBUF_SIZE %d\n", 1,AFE_GET_REG(ASM_OBUF_SIZE));

    DSP_MW_LOG_I("transform asrc in rate:%d, out rate:%d\r\n", 2, src_configuration.input_buffer.rate, src_configuration.output_buffer.rate);
    DSP_MW_LOG_I("input_buffer.format %d\n", 1,src_configuration.input_buffer.format);
    DSP_MW_LOG_I("output_buffer.format %d\n", 1,src_configuration.output_buffer.format);
    DSP_MW_LOG_I("is_mono %d\n", 1,src_configuration.is_mono);
    DSP_MW_LOG_I("DSP_GetFuncStreamSamplingRate(para) %d\n", 1,stream_function_get_samplingrate(para));
    DSP_MW_LOG_I("ASRC_OUTPUT_OFFSET %d\n", 1,AFE_READ(ASM_CH01_OBUF_WRPNT)-AFE_GET_REG(ASM_OBUF_SADR));

    src_feature_ptr->inSRC_wo = 0;//init
    AFE_WRITE(ASM_CH01_IBUF_WRPNT, src_feature_ptr->inSRC_wo + AFE_READ(ASM_IBUF_SADR));
    src_feature_ptr->outSRC_ro = 0;//init
    AFE_WRITE(ASM_CH01_OBUF_RDPNT, src_feature_ptr->outSRC_ro + AFE_GET_REG(ASM_OBUF_SADR));
    DSP_MW_LOG_I("ASRC inwptr %d outrptr %d\n", 2,AFE_READ(ASM_CH01_IBUF_WRPNT)-AFE_GET_REG(ASM_IBUF_SADR),
    AFE_READ(ASM_CH01_OBUF_RDPNT)-AFE_GET_REG(ASM_OBUF_SADR));
}

BOOL DSP_Callback_ASRC_Handle(VOID* para, VOID* buf_ptr1, VOID* buf_ptr2, U32 in_size)
{
    //printf("DSP_Callback_ASRC_Handle\n");
    U32 inWriteOffset;
    U32 outReadOffset;
    DSP_SRC_FEATURE_PTR  src_feature_ptr;
    DSP_STREAMING_PARA_PTR stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    src_feature_ptr = stream_function_get_working_buffer(para);
    S32 bytes_per_sample = DSP_RsChange2SRCOutRs(stream_ptr->callback.Src.in_resolution);
    U32 in_num= ProcessSize;
    U32 outsize;
    void *pv_alloc_ptr;
    S32 fs_in = DSP_FsChange2SRCInRate(stream_function_get_samplingrate(para));
    S32 fs_out = DSP_FsChange2SRCOutRate(stream_ptr->callback.Src.out_sampling_rate);
    S32 nch = stream_function_get_channel_number(para);
    S32 insize_chunk = ProcessSize;
    S32 ui_size, ui_alignment, ui_type;
    S32 a, n_output;
    //HWSRC
    uint32_t hw_current_read_idx = 0;
    U16 copy_size,copy_offset,unwrap_size;
    U32 writeOffset = 0;
    U32 ReadOffset  = 0;
    UNUSED(ReadOffset);
    UNUSED(writeOffset);
    UNUSED(unwrap_size);
    UNUSED(copy_offset);
    UNUSED(copy_size);
    UNUSED(hw_current_read_idx);
    UNUSED(n_output);
    UNUSED(a);
    UNUSED(ui_type);
    UNUSED(ui_alignment);
    UNUSED(insize_chunk);
    UNUSED(fs_out);
    UNUSED(fs_in);
    UNUSED(pv_alloc_ptr);
    UNUSED(outsize);
    UNUSED(in_num);
    UNUSED(outReadOffset);
    UNUSED(ui_size);


    if(src_feature_ptr->InitDone !=1){
        //if (!(AFE_GET_REG(ASM_GEN_CONF + AFE_MEM_ASRC_1)&ASM_GEN_CONF_ASRC_EN_MASK)) {
            DSP_Set_ASRC_Configuration_Parameters(para);
        //}
        src_feature_ptr->InitDone =1;
    }


    src_feature_ptr->inSRC_ro = AFE_READ(ASM_CH01_IBUF_RDPNT)- AFE_READ(ASM_IBUF_SADR);
    src_feature_ptr->inSRC_wo = AFE_READ(ASM_CH01_IBUF_WRPNT)- AFE_READ(ASM_IBUF_SADR);
    src_feature_ptr->inSRC_count = ((src_feature_ptr->inSRC_wo +src_feature_ptr->inSRC_mem_size )
                                    -src_feature_ptr->inSRC_ro)
                                    %src_feature_ptr->inSRC_mem_size;
    //printf("1 src_feature_ptr->inSRC_count %d src_feature_ptr->inSRC_ro %d src_feature_ptr->inSRC_wo %d in_size %d\n",src_feature_ptr->inSRC_count,src_feature_ptr->inSRC_ro,src_feature_ptr->inSRC_wo,in_size);
    if (src_feature_ptr->inSRC_count+(in_size*nch) <= src_feature_ptr->inSRC_mem_size)
    {   //stream_ptr->callback.Src.outSRC_Full = FALSE;
        //printf("nch %d\n",nch);
        if(nch < MAX_CHANNELS){
            //memcpy(src_feature_ptr->SRC_INPUT_BUF, buf_ptr1, in_size);
            DSP_D2C_BufferCopy(src_feature_ptr->inSRC_wo+src_feature_ptr->inSRC_mem_ptr,
            buf_ptr1,
            in_size,
            src_feature_ptr->inSRC_mem_ptr,
            src_feature_ptr->inSRC_mem_size);
        }
        else if(nch==MAX_CHANNELS){

            if (bytes_per_sample == 4){

                U8* DestCBufEnd     = (U8*)((U8*)src_feature_ptr->inSRC_mem_ptr + src_feature_ptr->inSRC_mem_size);
                U16 UnwrapSize      = (U8*)DestCBufEnd - (U8*)(src_feature_ptr->inSRC_wo+src_feature_ptr->inSRC_mem_ptr); /* Remove + 1 to sync more common usage */
                S32 WrapSize        = (in_size*nch) - UnwrapSize;

                configASSERT((DestCBufEnd >= (U8*)(src_feature_ptr->inSRC_wo+src_feature_ptr->inSRC_mem_ptr)) && ((src_feature_ptr->inSRC_wo+src_feature_ptr->inSRC_mem_ptr) >= src_feature_ptr->inSRC_mem_ptr));

                if (WrapSize > 0)
                {
                    DSP_D2I_BufferCopy_32bit((U32*)(src_feature_ptr->inSRC_wo+src_feature_ptr->inSRC_mem_ptr),
                                        (U32*)buf_ptr1,
                                        (U32*)buf_ptr2,
                                        (UnwrapSize/nch)>>2);

                        DSP_D2I_BufferCopy_32bit((U32*)src_feature_ptr->inSRC_mem_ptr,
                        (U32*)((U8*)buf_ptr1 + (UnwrapSize>>1)),
                        (U32*)((U8*)buf_ptr2 + (UnwrapSize>>1)),
                        (WrapSize/nch)>>2);
                }
                else
                {
                    //memcpy(DestBuf, SrcBuf, CopySize);
                    DSP_D2I_BufferCopy_32bit((U32*)(src_feature_ptr->inSRC_wo+src_feature_ptr->inSRC_mem_ptr),
                    (U32*)buf_ptr1,
                    (U32*)buf_ptr2,
                    in_size>>2);
                }

            }else{

                U8* DestCBufEnd     = (U8*)((U8*)src_feature_ptr->inSRC_mem_ptr + src_feature_ptr->inSRC_mem_size);
                U16 UnwrapSize      = (U8*)DestCBufEnd - (U8*)(src_feature_ptr->inSRC_wo+src_feature_ptr->inSRC_mem_ptr); /* Remove + 1 to sync more common usage */
                S32 WrapSize        = (in_size*nch) - UnwrapSize;

                configASSERT((DestCBufEnd >= (U8*)(src_feature_ptr->inSRC_wo+src_feature_ptr->inSRC_mem_ptr)) && ((src_feature_ptr->inSRC_wo+src_feature_ptr->inSRC_mem_ptr) >= src_feature_ptr->inSRC_mem_ptr));

                if (WrapSize > 0)
                {
                    DSP_D2I_BufferCopy_16bit((U16*)(src_feature_ptr->inSRC_wo+src_feature_ptr->inSRC_mem_ptr),
                                        (U16*)buf_ptr1,
                                        (U16*)buf_ptr2,
                                        (UnwrapSize/nch)>>1);

                    //memcpy(DestCBufStart, (U8*)SrcBuf + UnwrapSize, WrapSize);
                        DSP_D2I_BufferCopy_16bit((U16*)src_feature_ptr->inSRC_mem_ptr,
                        (U16*)((U8*)buf_ptr1 + (UnwrapSize>>1)),
                        (U16*)((U8*)buf_ptr2 + (UnwrapSize>>1)),
                        (WrapSize/nch)>>1);
                }
                else
                {
                    DSP_D2I_BufferCopy_16bit((U16*)(src_feature_ptr->inSRC_wo+src_feature_ptr->inSRC_mem_ptr),
                    (U16*)buf_ptr1,
                    (U16*)buf_ptr2,
                    in_size>>1);
                }
            }

            //if (in_size != 0)
            //    printf("sbc index :%d",SbcDecCtrl.SBC_decode_frame_index );
        }


        src_feature_ptr->inSRC_wo = AFE_READ(ASM_CH01_IBUF_WRPNT)- AFE_READ(ASM_IBUF_SADR);
        inWriteOffset = src_feature_ptr->inSRC_wo;
        //printf("src_feature_ptr->inSRC_wo %d (in_size*nch) %d bytes_per_sample %d\n",src_feature_ptr->inSRC_wo,(in_size*nch),bytes_per_sample);
        src_feature_ptr->inSRC_wo += (in_size*nch);
        src_feature_ptr->inSRC_wo %= src_feature_ptr->inSRC_mem_size;
        AFE_WRITE(ASM_CH01_IBUF_WRPNT, src_feature_ptr->inSRC_wo + AFE_READ(ASM_IBUF_SADR));
        //printf("ASM_CH01_IBUF_WRPNT %d\n",src_feature_ptr->inSRC_wo);

    }
    else
    {
        // need to handle for callback
        //stream_ptr->callback.Src.outSRC_Full = TRUE;
        //printf("inSRC_Full %d,outSRC_Full %d\n",stream_ptr->callback.Src.inSRC_Full,stream_ptr->callback.Src.outSRC_Full);//modify for asrc in stream
    }

    src_feature_ptr->inSRC_ro = AFE_READ(ASM_CH01_IBUF_RDPNT)- AFE_READ(ASM_IBUF_SADR);
    src_feature_ptr->inSRC_wo = AFE_READ(ASM_CH01_IBUF_WRPNT)- AFE_READ(ASM_IBUF_SADR);
    //src_feature_ptr->inSRC_mem_size = AFE_READ(ASM_IBUF_SIZE);
    src_feature_ptr->inSRC_count = ((src_feature_ptr->inSRC_wo +src_feature_ptr->inSRC_mem_size  )
                                   -src_feature_ptr->inSRC_ro)
                                    %src_feature_ptr->inSRC_mem_size;

    //printf("2 src_feature_ptr->inSRC_count %d src_feature_ptr->inSRC_ro %d src_feature_ptr->inSRC_wo %d in_size %d\n",src_feature_ptr->inSRC_count,src_feature_ptr->inSRC_ro,src_feature_ptr->inSRC_wo,in_size);
    /****************Write to Circular Buffer**********************/
    do{
        src_feature_ptr->inSRC_ro = AFE_READ(ASM_CH01_IBUF_RDPNT)- AFE_READ(ASM_IBUF_SADR);
        src_feature_ptr->inSRC_wo = AFE_READ(ASM_CH01_IBUF_WRPNT)- AFE_READ(ASM_IBUF_SADR);
        src_feature_ptr->inSRC_count = ((src_feature_ptr->inSRC_wo +src_feature_ptr->inSRC_mem_size  )
                               -src_feature_ptr->inSRC_ro)
                                %src_feature_ptr->inSRC_mem_size;

        src_feature_ptr->outSRC_ro = AFE_READ(ASM_CH01_OBUF_RDPNT)- AFE_READ(ASM_OBUF_SADR);
        src_feature_ptr->outSRC_wo = AFE_READ(ASM_CH01_OBUF_WRPNT)-AFE_GET_REG(ASM_OBUF_SADR);
        src_feature_ptr->outSRC_count = (src_feature_ptr->outSRC_wo > src_feature_ptr->outSRC_ro) ?
                                        src_feature_ptr->outSRC_wo - src_feature_ptr->outSRC_ro :
                                        src_feature_ptr->outSRC_wo + src_feature_ptr->outSRC_mem_size - src_feature_ptr->outSRC_ro;
        //printf("outSRC_wo %d,outSRC_ro %d outSRC_count %d out_frame_size %d nch %d inSRC_extract_length %d\n",src_feature_ptr->outSRC_wo,src_feature_ptr->outSRC_ro,
        //src_feature_ptr->outSRC_count,stream_ptr->callback.Src.out_frame_size,nch,src_feature_ptr->inSRC_extract_length);
    }while((src_feature_ptr->outSRC_count <= (stream_ptr->callback.Src.out_frame_size * nch))&& (src_feature_ptr->inSRC_count > (nch*src_feature_ptr->inSRC_extract_length)));


    if (src_feature_ptr->outSRC_count > stream_ptr->callback.Src.out_frame_size * nch)//(stream_ptr->callback.Src.out_frame_size * nch)
    {

        if(nch < MAX_CHANNELS){

            DSP_C2D_BufferCopy(buf_ptr1,
                                src_feature_ptr->outSRC_ro+src_feature_ptr->outSRC_mem_ptr,
                                stream_ptr->callback.Src.out_frame_size * nch,
                                src_feature_ptr->outSRC_mem_ptr,
                                src_feature_ptr->outSRC_mem_size);

        }
        else if(nch==MAX_CHANNELS){
                if (bytes_per_sample == 4){

                    U8* SrcCBufEnd      =  (U8*)((U8*)src_feature_ptr->outSRC_mem_ptr +  src_feature_ptr->outSRC_mem_size);
                    U16 UnwrapSize      = (U8*)SrcCBufEnd - (U8*)(src_feature_ptr->outSRC_ro+src_feature_ptr->outSRC_mem_ptr); /* Remove + 1 to sync more common usage */
                    S32 WrapSize        = (stream_ptr->callback.Src.out_frame_size * nch) - UnwrapSize;
                    configASSERT((SrcCBufEnd >= (U8*)(src_feature_ptr->outSRC_ro+src_feature_ptr->outSRC_mem_ptr)) && ((src_feature_ptr->outSRC_ro+src_feature_ptr->outSRC_mem_ptr) >= src_feature_ptr->outSRC_mem_ptr));

                    if (WrapSize > 0)
                    {
                        DSP_I2D_BufferCopy_32bit((U32* )(src_feature_ptr->outSRC_ro+src_feature_ptr->outSRC_mem_ptr),
                                            (U32* )buf_ptr1,
                                            (U32* )buf_ptr2,
                                            (UnwrapSize/nch)>>2);

                        DSP_I2D_BufferCopy_32bit((U32* )src_feature_ptr->outSRC_mem_ptr,
                                            (U32* )((U8*)buf_ptr1 + (UnwrapSize>>1)),
                                            (U32* )((U8*)buf_ptr2 + (UnwrapSize>>1)),
                                            (WrapSize/nch)>>2);

                    }
                    else
                    {

                        DSP_I2D_BufferCopy_32bit((U32* )(src_feature_ptr->outSRC_ro+src_feature_ptr->outSRC_mem_ptr),
                                            (U32* )buf_ptr1,
                                            (U32* )buf_ptr2,
                                            (stream_ptr->callback.Src.out_frame_size)>>2);
                    }

                }else{

                    U8* SrcCBufEnd      =  (U8*)((U8*)src_feature_ptr->outSRC_mem_ptr +  src_feature_ptr->outSRC_mem_size);
                    U16 UnwrapSize      = (U8*)SrcCBufEnd - (U8*)(src_feature_ptr->outSRC_ro+src_feature_ptr->outSRC_mem_ptr); /* Remove + 1 to sync more common usage */
                    S32 WrapSize        = (stream_ptr->callback.Src.out_frame_size * nch) - UnwrapSize;
                    configASSERT((SrcCBufEnd >= (U8*)(src_feature_ptr->outSRC_ro+src_feature_ptr->outSRC_mem_ptr)) && ((src_feature_ptr->outSRC_ro+src_feature_ptr->outSRC_mem_ptr) >= src_feature_ptr->outSRC_mem_ptr));

                    if (WrapSize > 0)
                    {

                        DSP_I2D_BufferCopy_16bit((U16*)(src_feature_ptr->outSRC_ro+src_feature_ptr->outSRC_mem_ptr),
                                            (U16*)buf_ptr1,
                                            (U16*)buf_ptr2,
                                            (UnwrapSize/nch)>>1);

                        DSP_I2D_BufferCopy_16bit((U16*)src_feature_ptr->outSRC_mem_ptr,
                                            (U16*)((U8*)buf_ptr1 + (UnwrapSize>>1)),
                                            (U16*)((U8*)buf_ptr2 + (UnwrapSize>>1)),
                                            (WrapSize/nch)>>1);

                    }
                    else
                    {
                        DSP_I2D_BufferCopy_16bit((U16*)(src_feature_ptr->outSRC_ro+src_feature_ptr->outSRC_mem_ptr),
                                            (U16*)buf_ptr1,
                                            (U16*)buf_ptr2,
                                            (stream_ptr->callback.Src.out_frame_size)>>1);
                    }
                }
        }

        src_feature_ptr->outSRC_ro = AFE_READ(ASM_CH01_OBUF_RDPNT)- AFE_READ(ASM_OBUF_SADR);
        src_feature_ptr->outSRC_ro += (stream_ptr->callback.Src.out_frame_size * nch);
        src_feature_ptr->outSRC_ro %= src_feature_ptr->outSRC_mem_size;
        AFE_WRITE(ASM_CH01_OBUF_RDPNT, src_feature_ptr->outSRC_ro + AFE_GET_REG(ASM_OBUF_SADR));

        //printf("1 outSRC_wo %d,outSRC_ro %d outSRC_count %d out_frame_size %d nch %d\n ",src_feature_ptr->outSRC_wo,src_feature_ptr->outSRC_ro,src_feature_ptr->outSRC_count,stream_ptr->callback.Src.out_frame_size,nch);

    }
    else
    {
        // need to handle for callback
        //printf("0 outSRC_wo %d,outSRC_ro %d outSRC_count %d out_frame_size %d nch %d\n ",src_feature_ptr->outSRC_wo,src_feature_ptr->outSRC_ro,src_feature_ptr->outSRC_count,stream_ptr->callback.Src.out_frame_size,nch);
        return FALSE;
    }


    return TRUE;
}
#endif

BOOL DSP_Callback_SRC_Handle(VOID* para, VOID* buf_ptr1, VOID* buf_ptr2, U32 in_size)
{
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    U32 inWriteOffset;
    U32 outReadOffset;
    DSP_SRC_FEATURE_PTR  src_feature_ptr;
    DSP_STREAMING_PARA_PTR stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    src_feature_ptr = stream_function_get_working_buffer(para);

    while (DSP_GetSRCStatus(src_feature_ptr->src_ptr) && (stream_ptr->streamingStatus == STREAMING_ACTIVE))
    {
       // DSP_LOG_WarningPrint(DSP_WARNING_WAIT_SRC_DONE, 0);
       // osTaskSignalWait(src_feature_ptr->task_id);
       // osTaskTaskingRequest();
    }
    if (src_feature_ptr->inSRC_count+in_size <= src_feature_ptr->inSRC_mem_size)
    {
        DSP_D2C_BufferCopy((VOID*)(DSP_GetSRCIn1BufPtr(src_feature_ptr->src_ptr)+src_feature_ptr->inSRC_wo),
                           buf_ptr1,
                           in_size,
                           DSP_GetSRCIn1BufPtr(src_feature_ptr->src_ptr),
                           DSP_GetSRCInBufSize(src_feature_ptr->src_ptr));

        if (src_feature_ptr->channel_num >= 2)
        {
            DSP_D2C_BufferCopy((VOID*)(DSP_GetSRCIn2BufPtr(src_feature_ptr->src_ptr)+src_feature_ptr->inSRC_wo),
                               buf_ptr2,
                               in_size,
                               DSP_GetSRCIn2BufPtr(src_feature_ptr->src_ptr),
                               DSP_GetSRCInBufSize(src_feature_ptr->src_ptr));
        }

        inWriteOffset = src_feature_ptr->inSRC_wo;
        src_feature_ptr->inSRC_wo += in_size;
        src_feature_ptr->inSRC_wo %= src_feature_ptr->inSRC_mem_size;
    }
    else
    {
        // need to handle for callback
    }


    src_feature_ptr->inSRC_ro = DSP_GetSRCInReadOffset(src_feature_ptr->src_ptr);
    src_feature_ptr->inSRC_count = ((src_feature_ptr->inSRC_wo + src_feature_ptr->inSRC_mem_size)
                                    -src_feature_ptr->inSRC_ro)
                                   %src_feature_ptr->inSRC_mem_size;

    if(OFFSET_OVERFLOW_CHK(inWriteOffset, src_feature_ptr->inSRC_wo, src_feature_ptr->inSRC_ro)||
      (src_feature_ptr->inSRC_count <= src_feature_ptr->inSRC_extract_length+CALLBACK_SRC_FLOAT_PROTECTION) )
    {
        return FALSE;
    }



    #if 1
    // Read ping-pong buffer
    outReadOffset = (DSP_GetSRCOutWriteOffset(src_feature_ptr->src_ptr) +
                     DSP_GetSRCOutBufSize(src_feature_ptr->src_ptr) - DSP_GetSRCOutFrameSize(src_feature_ptr->src_ptr))
                     % DSP_GetSRCOutBufSize(src_feature_ptr->src_ptr);
    DSP_SetSRCTrigger(src_feature_ptr->src_ptr);
    #else
    // Wait for SRC process
    outReadOffset = DSP_GetSRCOutWriteOffset(src_feature_ptr->src_ptr);
    DSP_SetSRCTrigger(src_feature_ptr->src_ptr);
    while (DSP_GetSRCStatus(src_feature_ptr->src_ptr));
    #endif

    DSP_C2D_BufferCopy(buf_ptr1,
                       (VOID*)(DSP_GetSRCOut1BufPtr(src_feature_ptr->src_ptr) + outReadOffset),
                       DSP_GetSRCOutFrameSize(src_feature_ptr->src_ptr),
                       DSP_GetSRCOut1BufPtr(src_feature_ptr->src_ptr),
                       DSP_GetSRCOutBufSize(src_feature_ptr->src_ptr));
    if (src_feature_ptr->channel_num >= 2)
    {
        DSP_C2D_BufferCopy(buf_ptr2,
                           (VOID*)(DSP_GetSRCOut2BufPtr(src_feature_ptr->src_ptr) + outReadOffset),
                           DSP_GetSRCOutFrameSize(src_feature_ptr->src_ptr),
                           DSP_GetSRCOut2BufPtr(src_feature_ptr->src_ptr),
                           DSP_GetSRCOutBufSize(src_feature_ptr->src_ptr));
    }

    src_feature_ptr->outSRC_ro = outReadOffset;
    src_feature_ptr->outSRC_wo = DSP_GetSRCOutWriteOffset(src_feature_ptr->src_ptr) ;
#else
    UNUSED(para);
    UNUSED(buf_ptr1);
    UNUSED(buf_ptr2);
    UNUSED(in_size);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
    return TRUE;
}

VOID DSP_SRC_CBufWrite(DSP_SRC_FEATURE_PTR src_feature_ptr, VOID* buf_ptr1, VOID* buf_ptr2, U32 in_size)
{
    DSP_D2C_BufferCopy((VOID*)(src_feature_ptr->buf_mem_ptr+src_feature_ptr->buf_wo),
                       buf_ptr1,
                       in_size,
                       (VOID*)src_feature_ptr->buf_mem_ptr,
                       src_feature_ptr->buf_mem_size);

    if (src_feature_ptr->channel_num>1)
    {
        DSP_D2C_BufferCopy((VOID*)(src_feature_ptr->buf_mem_ptr+src_feature_ptr->buf_mem_size+src_feature_ptr->buf_wo),
                           buf_ptr2,
                           in_size,
                           (VOID*)(src_feature_ptr->buf_mem_ptr+src_feature_ptr->buf_mem_size),
                           src_feature_ptr->buf_mem_size);
    }

    src_feature_ptr->buf_wo += in_size;
    src_feature_ptr->buf_wo %= src_feature_ptr->buf_mem_size;
    src_feature_ptr->buf_count += in_size;
}

VOID DSP_SRC_CBufRead(DSP_SRC_FEATURE_PTR src_feature_ptr, VOID* buf_ptr1, VOID* buf_ptr2, U32 out_size)
{

    DSP_C2D_BufferCopy(buf_ptr1,
                       (VOID*)(src_feature_ptr->buf_mem_ptr+src_feature_ptr->buf_ro),
                       out_size,
                       (VOID*)src_feature_ptr->buf_mem_ptr,
                       src_feature_ptr->buf_mem_size);

    if (src_feature_ptr->channel_num>1)
    {
        DSP_C2D_BufferCopy(buf_ptr2,
                       (VOID*)(src_feature_ptr->buf_mem_ptr+src_feature_ptr->buf_mem_size+src_feature_ptr->buf_ro),
                       out_size,
                       (VOID*)(src_feature_ptr->buf_mem_ptr+src_feature_ptr->buf_mem_size),
                       src_feature_ptr->buf_mem_size);
    }

    src_feature_ptr->buf_ro += out_size;
    src_feature_ptr->buf_ro %= src_feature_ptr->buf_mem_size;
    src_feature_ptr->buf_count -= out_size;
}

uint8_t hwsrc_sleep_manager_handle;

#ifdef MTK_HWSRC_IN_STREAM
bool stream_function_src_initialize(void *para)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    DSP_SRC_FEATURE_PTR src_feature_ptr;
    DSP_DRV_SRC_VDM_INIT_STRU src_setting;
    U32 ch_num;
    U8* mem_ptr;
    U16 srcInFrameSize;

    src_feature_ptr = stream_function_get_working_buffer(para);
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    ch_num = stream_function_get_channel_number(para);
    srcInFrameSize = stream_ptr->callback.EntryPara.out_malloc_size;

    src_feature_ptr->inSRC_mem_size     = srcInFrameSize*DSP_CALLBACK_SRC_IN_FRAME;
    src_feature_ptr->outSRC_mem_size    = stream_ptr->callback.Src.out_frame_size*DSP_CALLBACK_SRC_OUT_FRAME;
    src_feature_ptr->buf_mem_size       = srcInFrameSize*DSP_CALLBACK_SRC_BUF_FRAME;
    DSP_MW_LOG_I("stream_function_src_initialize \n", 0);

#ifdef MTK_HWSRC_IN_STREAM
    mem_ptr = (U8*)((U32)src_feature_ptr + DSP_OFFSET_OF(DSP_SRC_FEATURE, mem_begin));
    mem_ptr = (U8*)((U32)(mem_ptr + 15) & ~15);  //modify for asrc 8-byte align
    src_feature_ptr->inSRC_mem_ptr = mem_ptr;
    mem_ptr += src_feature_ptr->inSRC_mem_size*ch_num;
    mem_ptr = (U8*)((U32)(mem_ptr + 15) & ~15);  //modify for asrc 8-byte align
    src_feature_ptr->outSRC_mem_ptr = mem_ptr;
    mem_ptr += src_feature_ptr->outSRC_mem_size*ch_num;
    mem_ptr = (U8*)((U32)(mem_ptr + 15) & ~15);  //modify for asrc 8-byte align
    src_feature_ptr->buf_mem_ptr = mem_ptr;
#else
    mem_ptr = (U8*)((U32)src_feature_ptr + DSP_OFFSET_OF(DSP_SRC_FEATURE, mem_begin));
    src_feature_ptr->inSRC_mem_ptr = mem_ptr;
    mem_ptr += src_feature_ptr->inSRC_mem_size*ch_num;
    src_feature_ptr->outSRC_mem_ptr = mem_ptr;
    mem_ptr += src_feature_ptr->outSRC_mem_size*ch_num;
    src_feature_ptr->buf_mem_ptr = mem_ptr;
#endif

    memset(src_feature_ptr->inSRC_mem_ptr,    0, (src_feature_ptr->inSRC_mem_size +
                                                  src_feature_ptr->outSRC_mem_size +
                                                  src_feature_ptr->buf_mem_size)*ch_num);
#ifdef MTK_HWSRC_IN_STREAM
    src_feature_ptr->outSRC_mem_size = src_feature_ptr->outSRC_mem_size * 2;
    src_feature_ptr->inSRC_mem_size = src_feature_ptr->inSRC_mem_size * 2;

    if (AFE_GET_REG(ASM_GEN_CONF + AFE_MEM_ASRC_1)&ASM_GEN_CONF_ASRC_BUSY_MASK) {
        DSP_MW_LOG_E("DSP_SRC_Init() error: asrc[%d] is running\r\n", 1, AFE_MEM_ASRC_1);
    }
    if ((AFE_GET_REG(ASM_GEN_CONF + AFE_MEM_ASRC_1)&ASM_GEN_CONF_ASRC_EN_MASK)) {

        afe_src_configuration_t src_configuration;
        src_configuration.id = AFE_SRC_1;
        hal_audio_src_set_start(&src_configuration, HAL_AUDIO_MEMORY_SYNC_NONE, HAL_AUDIO_CONTROL_OFF);
        hal_audio_src_configuration(&src_configuration, HAL_AUDIO_CONTROL_OFF);
    }
    hwsrc_sleep_manager_handle = hal_sleep_manager_set_sleep_handle("hwsrc");
    hal_sleep_manager_lock_sleep(hwsrc_sleep_manager_handle);
    DSP_MW_LOG_I("hal_sleep_manager_lock_sleep\n", 0);
    //dvfs_lock_control("DVFS",DVFS_156M_SPEED,DVFS_LOCK);
    //if(src_feature_ptr->src_ptr == SRC_DEF){
    //    src_feature_ptr->src_ptr = ASRC_1;
    //    printf("SRC_DEF\n");
    //}else if(src_feature_ptr->src_ptr == ASRC_1){
    //    afe_set_asrc_enable(false, AFE_MEM_ASRC_1,NULL);
    //    afe_mem_asrc_enable(AFE_MEM_ASRC_1, false);
    //    printf("ASRC_1\n");
    //}
#endif

    stream_ptr->callback.Src.inSRC_Full = FALSE;
    stream_ptr->callback.Src.outSRC_Full= FALSE;

    src_feature_ptr->in_sampling_rate =  stream_function_get_samplingrate(para);
    src_feature_ptr->channel_num      =  stream_function_get_channel_number(para);
    src_feature_ptr->task_id          =  (TaskHandle_t)stream_function_get_task(para);
    /*Configure SRC*/
    src_setting.src_ptr        = stream_ptr->callback.Src.src_ptr;
    src_setting.mode           = SRC_OVDM;
    src_setting.task_id        = src_feature_ptr->task_id ;
    src_setting.radma_buf_addr = (U8*)(src_feature_ptr->inSRC_mem_ptr);
    src_setting.radma_buf_size = src_feature_ptr->inSRC_mem_size;
    src_setting.radma_THD      = srcInFrameSize;
    src_setting.wadma_buf_addr = (U8*)(src_feature_ptr->outSRC_mem_ptr);
    src_setting.wadma_buf_size = src_feature_ptr->outSRC_mem_size;
    src_setting.wadma_THD      = stream_ptr->callback.Src.out_frame_size;
    src_setting.fs_in          = DSP_FsChange2SRCInRate(src_feature_ptr->in_sampling_rate);
    src_setting.fs_out         = DSP_FsChange2SRCOutRate(stream_ptr->callback.Src.out_sampling_rate);
#ifdef MTK_HWSRC_IN_STREAM
    src_setting.Res_In         = DSP_RsChange2SRCOutRs(stream_ptr->callback.Src.in_resolution);
    src_setting.Res_Out        = DSP_RsChange2SRCOutRs(stream_ptr->callback.Src.out_resolution);
#else
    src_setting.Res_In         = stream_ptr->callback.Src.in_resolution;
    src_setting.Res_Out        = stream_ptr->callback.Src.out_resolution;
#endif

    src_setting.channel_num    = ch_num;


    //stream_ptr->callback.Src.src_ptr = DSP_DRV_SRC_VDM_INIT(&src_setting);
#ifdef MTK_HWSRC_IN_STREAM
    stream_ptr->callback.Src.src_ptr =(SRC_PTR_s)ASRC_1;
#endif

    stream_ptr->callback.EntryPara.src_out_sampling_rate = stream_ptr->callback.Src.out_sampling_rate;

    src_feature_ptr->src_ptr = stream_ptr->callback.Src.src_ptr;
    src_feature_ptr->inSRC_extract_length = (stream_ptr->callback.Src.out_frame_size * src_feature_ptr->in_sampling_rate)
                                            / stream_ptr->callback.Src.out_sampling_rate;
    src_feature_ptr->in_max_size = 0;

    src_feature_ptr->inSRC_ro = 0;// DSP_GetSRCInReadOffset(src_feature_ptr->src_ptr);
    src_feature_ptr->inSRC_wo = 0;//(src_feature_ptr->inSRC_ro + src_feature_ptr->inSRC_mem_size/2)%src_feature_ptr->inSRC_mem_size;
    src_feature_ptr->inSRC_count = ((src_feature_ptr->inSRC_wo + src_feature_ptr->inSRC_mem_size)
                                    -src_feature_ptr->inSRC_ro)
                                   %src_feature_ptr->inSRC_mem_size;
    DSP_MW_LOG_I("src_feature_ptr->inSRC_mem_size %d \n", 1,src_feature_ptr->inSRC_mem_size);

    src_feature_ptr->outSRC_ro = 0;
    src_feature_ptr->outSRC_wo = 0;

    src_feature_ptr->buf_ro = 0;
    src_feature_ptr->buf_wo = 0;//stream_ptr->callback.Src.out_frame_size;
#ifndef MTK_HWSRC_IN_STREAM
    src_feature_ptr->buf_count = ((src_feature_ptr->buf_wo + src_feature_ptr->buf_mem_size)
                                  - src_feature_ptr->buf_ro)
                                 % src_feature_ptr->buf_mem_size;
#endif
#ifdef MTK_HWSRC_IN_STREAM
    DSP_MW_LOG_I("MTK_HWSRC_IN_STREAM\n", 0);
    //if (!(AFE_GET_REG(ASM_GEN_CONF + AFE_MEM_ASRC_1)&ASM_GEN_CONF_ASRC_EN_MASK)) {
    //    DSP_Set_ASRC_Configuration_Parameters(para);
    //}
    src_feature_ptr->InitDone =0;

    //if(src_feature_ptr->src_ptr == ASRC_1){
    //    clock_dsp_dcm_disable(CLK_DSP0);
    //    DSP_Set_ASRC_Configuration_Parameters(para);
    //    printf("2 ASRC_1\n");
    //}
    SRC_Handle_Entry = DSP_Callback_ASRC_Handle;//DSP_Callback_SRC_Handle;
#endif

    return FALSE;

}



bool stream_function_src_process(void *para)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    DSP_SRC_FEATURE_PTR src_feature_ptr;
    U32 in_size, out_size = 0;
    U32 ch_num;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    src_feature_ptr = stream_function_get_working_buffer(para);
    ch_num = stream_function_get_channel_number(para);
    UNUSED(in_size);
    if (src_feature_ptr->src_ptr!= NULL)
    {
        if((src_feature_ptr->in_sampling_rate != stream_function_get_samplingrate(para)) ||
           (stream_ptr->callback.Src.in_resolution != stream_function_get_output_resolution(para)))
        {
            stream_ptr->callback.Src.in_resolution = stream_function_get_output_resolution(para);
            stream_feature_reinitialize(para);
            DSP_MW_LOG_I("rate %d != %d\r\n", 2,src_feature_ptr->in_sampling_rate,stream_function_get_samplingrate(para));
            DSP_MW_LOG_I("in_resolution %d != %d\r\n", 2,stream_ptr->callback.Src.in_resolution,stream_function_get_output_resolution(para));
        }
        else
        {

            if (src_feature_ptr->in_max_size < stream_function_get_output_size(para))
                src_feature_ptr->in_max_size = stream_function_get_output_size(para);
            #if 0
            //Out Ring-Buffer
            if (stream_ptr->callback.Src.outSRC_Full == FALSE)
            {
                in_size = stream_function_get_output_size(para);
                do
                {
                    /*SRC triger and prco*/
                    if(DSP_Callback_SRC_Handle(src_feature_ptr,
                                               stream_function_get_1st_inout_buffer(para),
                                               stream_function_get_2nd_inout_buffer(para),
                                               in_size))
                    {
                        DSP_SRC_CBufWrite(src_feature_ptr,
                                          stream_function_get_1st_inout_buffer(para),
                                          stream_function_get_2nd_inout_buffer(para),
                                          stream_ptr->callback.Src.out_frame_size);
                    }
                    in_size = 0;
                }while((src_feature_ptr->inSRC_count > 2*src_feature_ptr->inSRC_extract_length) &&
                       (src_feature_ptr->buf_mem_size-src_feature_ptr->buf_count >= stream_ptr->callback.Src.out_frame_size));
            }
            stream_ptr->callback.Src.inSRC_Full = ((src_feature_ptr->inSRC_mem_size - src_feature_ptr->inSRC_count)<src_feature_ptr->in_max_size);


            if (src_feature_ptr->buf_count >= stream_ptr->callback.Src.out_frame_size)
            {
                DSP_SRC_CBufRead(src_feature_ptr,
                                 stream_function_get_1st_inout_buffer(para),
                                 stream_function_get_2nd_inout_buffer(para),
                                 stream_ptr->callback.Src.out_frame_size);

                out_size = stream_ptr->callback.Src.out_frame_size;
            }
            stream_ptr->callback.Src.outSRC_Full = ((src_feature_ptr->buf_mem_size - src_feature_ptr->buf_count)<stream_ptr->callback.Src.out_frame_size);
            stream_ptr->callback.EntryPara.src_out_sampling_rate = stream_ptr->callback.Src.out_sampling_rate;
            #else
            //In Ring-Buffer
            if (src_feature_ptr->in_max_size < stream_function_get_output_size(para))
                src_feature_ptr->in_max_size = stream_function_get_output_size(para);
#ifdef MTK_HWSRC_IN_STREAM
                if(SRC_Handle_Entry(para,
                     stream_function_get_1st_inout_buffer(para),
                     stream_function_get_2nd_inout_buffer(para),
                     stream_function_get_output_size(para)))
                {
                    out_size = stream_ptr->callback.Src.out_frame_size;

                }

            stream_ptr->callback.Src.inSRC_Full = (src_feature_ptr->inSRC_count > (8*src_feature_ptr->inSRC_extract_length));
            stream_ptr->callback.Src.outSRC_Full = (src_feature_ptr->outSRC_count > (2 * stream_ptr->callback.Src.out_frame_size *ch_num));
            stream_ptr->callback.EntryPara.src_out_sampling_rate = stream_ptr->callback.Src.out_sampling_rate;
#else

            DSP_SRC_CBufWrite(src_feature_ptr,
                              stream_function_get_1st_inout_buffer(para),
                              stream_function_get_2nd_inout_buffer(para),
                              stream_function_get_output_size(para));

            do
            {
                in_size =MIN(MIN(src_feature_ptr->buf_count,
                                 src_feature_ptr->inSRC_mem_size - src_feature_ptr->inSRC_count-CALLBACK_SRC_FLOAT_PROTECTION),
                                 stream_ptr->callback.EntryPara.out_malloc_size);
                DSP_SRC_CBufRead(src_feature_ptr,
                                 stream_function_get_1st_inout_buffer(para),
                                 stream_function_get_2nd_inout_buffer(para),
                                 in_size);
                /*SRC triger and prco*/
                if(DSP_Callback_SRC_Handle(para,
                                           stream_function_get_1st_inout_buffer(para),
                                           stream_function_get_2nd_inout_buffer(para),
                                           in_size))
                {
                    out_size = stream_ptr->callback.Src.out_frame_size;
                    in_size = 0;
                    break;
                }
            }while(in_size > 0);

            stream_ptr->callback.Src.inSRC_Full = ((src_feature_ptr->buf_mem_size - src_feature_ptr->buf_count) < src_feature_ptr->in_max_size);
            stream_ptr->callback.Src.outSRC_Full = (src_feature_ptr->inSRC_count > 2*src_feature_ptr->inSRC_extract_length);

            stream_ptr->callback.EntryPara.src_out_sampling_rate = stream_ptr->callback.Src.out_sampling_rate;
            #endif
#endif//MTK_HWSRC_IN_STREAM

            stream_function_modify_output_resolution(para, stream_ptr->callback.Src.out_resolution);
        }
    }
    else
    {
        out_size = stream_ptr->callback.EntryPara.codec_out_size;
        stream_ptr->callback.EntryPara.src_out_sampling_rate = stream_ptr->callback.EntryPara.codec_out_sampling_rate;
    }

    stream_ptr->callback.EntryPara.src_out_size = out_size;
#ifdef MTK_HWSRC_IN_STREAM
    return FALSE;
#else
    return (stream_ptr->callback.EntryPara.src_out_size == 0)
               ? TRUE
               : FALSE;
#endif

}
#endif

VOID DSP_Callback_SRC_Config(DSP_STREAMING_PARA_PTR stream, stream_feature_type_ptr_t feature_type_ptr, U32 feature_entry_num)
{
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    stream->callback.EntryPara.with_src    = feature_entry_num;
    stream->callback.Src.out_sampling_rate = (*(feature_type_ptr)&0xFF00)>>8;
    stream->callback.Src.out_resolution    = (*(feature_type_ptr)&0x00010000)>>16;
    stream->callback.Src.in_resolution     =  (*(feature_type_ptr)&0x00020000)>>17;
    stream->callback.Src.out_frame_size    = (*(feature_type_ptr)&0xFFFC0000)>>18;

    if (stream->callback.Src.out_frame_size == 0 && stream->callback.Src.out_sampling_rate==0)
    {
        stream->callback.Src.out_resolution = (stream->sink->param.audio.AfeBlkControl.u4asrcflag)
                                               ? Audio_setting->resolution.SRCInRes
                                               : Audio_setting->resolution.AudioOutRes;
        stream->callback.Src.in_resolution = Audio_setting->resolution.AudioInRes;

        stream->callback.Src.out_frame_size = Audio_setting->Audio_sink.Frame_Size;
        stream->callback.Src.out_sampling_rate = AudioSinkSamplingRate_Get();
    }

    *(feature_type_ptr) &= ~0xFFFFFF00;
#else
    UNUSED(stream);
    UNUSED(feature_type_ptr);
    UNUSED(feature_entry_num);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
}


ATTR_TEXT_IN_IRAM_LEVEL_1 BOOL DSP_Callback_SRC_Triger_Chk(DSP_CALLBACK_PTR callback_ptr)
{
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    #if 0
    return ((callback_ptr->EntryPara.with_src != FALSE) &&
            (callback_ptr->Src.src_ptr != NULL) &&
            (callback_ptr->Src.buf_count >= 2*callback_ptr->Src.out_frame_size))
            ? FALSE
            : TRUE;
    #else

    if ((callback_ptr->EntryPara.with_src != FALSE) &&
        (callback_ptr->Src.src_ptr != NULL) &&
        (((callback_ptr->Src.outSRC_Full==TRUE)||(callback_ptr->Src.inSRC_Full==TRUE)) &&
          (callback_ptr->EntryPara.number.field.process_sequence < callback_ptr->EntryPara.with_src)))        /*SRCin buf full*/
    {
        callback_ptr->EntryPara.in_size = 0;
        callback_ptr->EntryPara.codec_out_size = 0;
        callback_ptr->EntryPara.resolution.process_res = callback_ptr->EntryPara.resolution.feature_res;
        return FALSE;
    }
    else
    {
        return TRUE;
    }
    #endif
#else
    UNUSED(callback_ptr);
    return TRUE;
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
}

U32 DSP_calculate_shift_bit(U32 value)
{
    U32 i;
    for(i=0 ; i<32 ; i++) {
        if (value == 0) {
            break;
        }
        value = value>>1;
    }
    return (i>0) ? i-1 : 0;
}

U32 dsp_count_bit(U32 value)
{
    U32 count = 0;
    while (value) {
        count++;
        value &= (value-1) ;
    }
    return count;
}

#define DSP_APPLY_SW_GAIN_CONST_TABLE
const uint32_t dsp_apply_sw_gain_table[] = {
    0,      // 0db 0
    29204,  //-1db 0.8912509381337455299531086810783
    26028,  //-2db 0.79432823472428150206591828283639
    23197,  //-3db 0.70794578438413791080221494218931
    20675,  //-4db 0.63095734448019324943436013662234
    18426,  //-5db 0.56234132519034908039495103977648
    0,      // 0db
};

VOID dsp_apply_sw_gain_16bit(void *ptr, U32 sample, S32 gain_times_of_db)
{
    S16 *buf16_ptr;
    S32 buf;
    U32 i;
    U32 absolute_gain;
    S32 multiplier = 0, times_6db, remain_multiply;

    absolute_gain = (gain_times_of_db>=0) ? gain_times_of_db : -gain_times_of_db;

    buf16_ptr = ptr;

    times_6db = (absolute_gain/6);
    if (gain_times_of_db==0) {
        return;
    } else if (gain_times_of_db>0) {
        if (absolute_gain%6) {
            times_6db++;
        }
        remain_multiply = (times_6db)*6 - absolute_gain;
    } else {
        remain_multiply = absolute_gain - times_6db*6;
    }
#ifdef DSP_APPLY_SW_GAIN_CONST_TABLE
    multiplier = dsp_apply_sw_gain_table[remain_multiply];
#else
    if (remain_multiply==1) {
        multiplier = 29204;//-1db 0.8912509381337455299531086810783
    } else if (remain_multiply==2) {
        multiplier = 26028;//-2db 0.79432823472428150206591828283639
    } else if (remain_multiply==3) {
        multiplier = 23197;//-3db 0.70794578438413791080221494218931
    } else if (remain_multiply==4) {
        multiplier = 20675;//-4db 0.63095734448019324943436013662234
    } else if (remain_multiply==5) {
        multiplier = 18426;//-5db 0.56234132519034908039495103977648
    }
#endif

    for(i = 0 ; i < sample ; i++) {

        buf = (S32)buf16_ptr[i];

        if (gain_times_of_db>=1) {
            buf = buf << times_6db;
        } else {
            buf = buf >> times_6db;
        }

        if (multiplier) {
            buf = (((S32)buf*multiplier)>>15); // *(multiplier/32768)
        }

        //overflow check
        if ((buf16_ptr[i]*buf) < 0) {
            if (buf16_ptr[i]<0) {
                buf = (S32)((S16)0x8000);
            } else {
                buf = 0x7FFF;
            }
        } else if (buf > (S32)0x7FFF) {
            buf = 0x7FFF;
        } else if (buf < (S32)((S16)0x8000)) {
            buf = (S32)((S16)0x8000);
        }
        buf16_ptr[i] = (S16)buf;
    }
}


VOID dsp_apply_sw_gain_32bit(void *ptr, U32 sample, S32 gain_times_of_db)
{
    S32 *buf32_ptr;
    S64 buf;
    U32 i;
    U32 absolute_gain;
    S32 multiplier = 0, times_6db, remain_multiply;

    absolute_gain = (gain_times_of_db>=0) ? gain_times_of_db : -gain_times_of_db;

    times_6db = (absolute_gain/6);
    if (gain_times_of_db==0) {
        return;
    } else if (gain_times_of_db>0) {
        if (absolute_gain%6) {
            times_6db++;
        }
        remain_multiply = (times_6db)*6 - absolute_gain;
    } else {
        remain_multiply = absolute_gain - times_6db*6;
    }

#ifdef DSP_APPLY_SW_GAIN_CONST_TABLE
    multiplier = dsp_apply_sw_gain_table[remain_multiply];
#else
    if (remain_multiply==1) {
        multiplier = 1913946815;//-1db 0.8912509381337455299531086810783
    } else if (remain_multiply==2) {
        multiplier = 1705806895;//-2db 0.79432823472428150206591828283639
    } else if (remain_multiply==3) {
        multiplier = 1520301995;//-3db 0.70794578438413791080221494218931
    } else if (remain_multiply==4) {
        multiplier = 1354970579;//-4db 0.63095734448019324943436013662234
    } else if (remain_multiply==5) {
        multiplier = 1207618800;//-5db 0.56234132519034908039495103977648
    }
#endif

    buf32_ptr = ptr;
    for(i = 0 ; i < sample ; i++) {

        buf = (S64)buf32_ptr[i];

        if (gain_times_of_db>=1) {
            buf = buf << times_6db;
        } else {
            buf = buf >> times_6db;
        }

        if (multiplier) {
        #ifdef DSP_APPLY_SW_GAIN_CONST_TABLE
            buf = (((S64)buf*multiplier)>>15); // *(multiplier/32768)
        #else
            buf = (((S64)buf*multiplier)>>31); // *(multiplier/2147483648)
        #endif
        }

        //overflow check
        if ((buf32_ptr[i]*buf) < 0) {
            if (buf32_ptr[i]<0) {
                buf = (S64)((S32)0x80000000);
            } else {
                buf = 0x7FFFFFFF;
            }
        } else if (buf > (S64)0x7FFFFFFF) {
            buf = 0x7FFFFFFF;
        } else if (buf < (S64)((S32)0x80000000)) {
            buf = (S64)((S32)0x80000000);
        }
        buf32_ptr[i] = (S32)buf;
    }
}


typedef void (*sw_gain_entry) (void *ptr, U32 sample, S32 gain_times_of_db);

bool stream_function_gain_initialize(void *para)
{
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    DSP_STREAMING_PARA_PTR stream_ptr;
    U8* mic_map_table;
    uint32_t i, channel_number;
    U8 mic_mapping_table_base;
    mic_mapping_table_base = 0;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    mic_map_table = stream_function_get_working_buffer(para);
    channel_number = stream_function_get_channel_number(para);

#ifdef MTK_SPECIAL_FUNCTIONS_ENABLE
    DSP_FEATURE_TABLE_PTR feature_table_ptr = stream_ptr->callback.FeatureTablePtr;
    stream_feature_type_t mic_fature_type = FUNC_END;
    if (feature_table_ptr) {
        while(feature_table_ptr->FeatureType != FUNC_END) {
            if ((feature_table_ptr->FeatureType>=FUNC_FUNCTION_A)&&(feature_table_ptr->FeatureType<=FUNC_FUNCTION_H)) {
                mic_fature_type = (stream_feature_type_t)feature_table_ptr->FeatureType;
                break;
            }
            feature_table_ptr++;
        }
    }
    if (mic_fature_type != FUNC_END) {
        mic_mapping_table_base = INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_BASE;
        DSP_MW_LOG_W("[SW GAIN]DSP Mic Functions type:0x%x", 1, mic_fature_type);
    }
#endif

    #ifdef ENABLE_2A2D_TEST
    uint32_t device[]= {stream_ptr->source->param.audio.audio_device,stream_ptr->source->param.audio.audio_device1,stream_ptr->source->param.audio.audio_device2,stream_ptr->source->param.audio.audio_device3,
                        stream_ptr->source->param.audio.audio_device4,stream_ptr->source->param.audio.audio_device5,stream_ptr->source->param.audio.audio_device6,stream_ptr->source->param.audio.audio_device7};
    uint32_t interface[]={stream_ptr->source->param.audio.audio_interface,stream_ptr->source->param.audio.audio_interface1,stream_ptr->source->param.audio.audio_interface2,stream_ptr->source->param.audio.audio_interface3,
                        stream_ptr->source->param.audio.audio_interface4,stream_ptr->source->param.audio.audio_interface5,stream_ptr->source->param.audio.audio_interface6,stream_ptr->source->param.audio.audio_interface7};
    #else
    uint32_t device[]={stream_ptr->source->param.audio.audio_device,0,0,0,0,0,0,0};
    uint32_t interface[]={stream_ptr->source->param.audio.audio_interface,0,0,0,0,0,0,0};
    #endif

    if ((stream_ptr->source->type == SOURCE_TYPE_AUDIO) || (stream_ptr->source->type == SOURCE_TYPE_AUDIO2) ||
#if defined(MTK_MULTI_MIC_STREAM_ENABLE) || defined(MTK_ANC_SURROUND_MONITOR_ENABLE) || defined(AIR_WIRED_AUDIO_ENABLE)
        ((stream_ptr->source->type >= SOURCE_TYPE_SUBAUDIO_MIN) && (stream_ptr->source->type <= SOURCE_TYPE_SUBAUDIO_MAX)))
#else
        (0))
#endif
    {
        if (stream_ptr->source->param.audio.echo_reference) {
            channel_number--;
            mic_map_table[channel_number] = INPUT_DIGITAL_GAIN_FOR_ECHO_PATH;
#ifdef MTK_SPECIAL_FUNCTIONS_ENABLE
            if (mic_mapping_table_base) {
                mic_map_table[channel_number] = INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_ECHO;
            }
#endif
            DSP_MW_LOG_I("[SW GAIN] mic_map_table echo[%d]=%d",2, channel_number,mic_map_table[channel_number]);
        }
    }

    for (i=1 ; i<=channel_number ; i++){
        if(device[i-1]!=(uint32_t)NULL && interface[i-1] !=(uint32_t)NULL)
        {
            if((device[i-1] == HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L) || (device[i-1] == HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_L) ){
                if(interface[i-1]==HAL_AUDIO_INTERFACE_1){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_MIC0_L;
                }
                else if(interface[i-1]==HAL_AUDIO_INTERFACE_2){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_MIC1_L;
                }
                else if(interface[i-1]==HAL_AUDIO_INTERFACE_3){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_MIC2_L;
                }else{
                    DSP_MW_LOG_W("[SW GAIN] mic L error ch=%d,if=%d",2, i-1,interface[i-1]);
                }

            } else if((device[i-1] == HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R) || (device[i-1] == HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_R) ){
                if(interface[i-1]==HAL_AUDIO_INTERFACE_1){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_MIC0_R;
                }
                else if(interface[i-1]==HAL_AUDIO_INTERFACE_2){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_MIC1_R;
                }
                else if(interface[i-1]==HAL_AUDIO_INTERFACE_3){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_MIC2_R;
                }else{
                    DSP_MW_LOG_W("[SW GAIN] mic R error ch=%d,if=%d",2, i-1,interface[i-1]);
                }
            } else if((device[i-1] == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER) ||
                      (device[i-1] == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE)){

                if(interface[i-1]==HAL_AUDIO_INTERFACE_1){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_I2S0_L;
                }
                else if(interface[i-1]==HAL_AUDIO_INTERFACE_2){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_I2S1_L;
                }
                else if(interface[i-1]==HAL_AUDIO_INTERFACE_3){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_I2S2_L;
                }else{
                    DSP_MW_LOG_W("[SW GAIN] I2S error ch=%d,if=%d",2, i-1,interface[i-1]);
                }
            } else if((device[i-1] == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L)){
                if(interface[i-1]==HAL_AUDIO_INTERFACE_1){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_I2S0_L;
                }
                else if(interface[i-1]==HAL_AUDIO_INTERFACE_2){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_I2S1_L;
                }
                else if(interface[i-1]==HAL_AUDIO_INTERFACE_3){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_I2S2_L;
                }else{
                    DSP_MW_LOG_W("[SW GAIN] I2S error ch=%d,if=%d",2, i-1,interface[i-1]);
                }
            } else if((device[i-1] == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R)){
                if(interface[i-1]==HAL_AUDIO_INTERFACE_1){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_I2S0_R;
                }
                else if(interface[i-1]==HAL_AUDIO_INTERFACE_2){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_I2S1_R;
                }
                else if(interface[i-1]==HAL_AUDIO_INTERFACE_3){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_I2S2_R;
                }else{
                    DSP_MW_LOG_W("[SW GAIN] I2S error ch=%d,if=%d",2, i-1,interface[i-1]);
                }
            } else if((device[i-1] == HAL_AUDIO_CONTROL_DEVICE_LINE_IN_L)){
                if(interface[i-1]==HAL_AUDIO_INTERFACE_1){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_LINEIN_L;
                }else{
                    DSP_MW_LOG_W("[SW GAIN] LINE_IN_L error ch=%d,if=%d",2, i-1,interface[i-1]);
                }

            } else if((device[i-1] == HAL_AUDIO_CONTROL_DEVICE_LINE_IN_R)){
                if(interface[i-1]==HAL_AUDIO_INTERFACE_1){
                    mic_map_table[i-1] = INPUT_DIGITAL_GAIN_FOR_LINEIN_R;
                }else{
                    DSP_MW_LOG_W("[SW GAIN] LINE_IN_R error ch=%d,if=%d",2, i-1,interface[i-1]);
                }
            } else {

            }
#ifdef MTK_SPECIAL_FUNCTIONS_ENABLE
            mic_map_table[i-1] += mic_mapping_table_base;//To separate mic scenario and special function gain
#endif
        }else{
            DSP_MW_LOG_I("[SW GAIN] get null device interface %d", 1, i-1);
        }
        DSP_MW_LOG_I("[SW GAIN] ch_num:%d, mic_map_table[%d]=%d, device:0x%x ", 4, channel_number,i-1,mic_map_table[i-1], device[i-1]);
    }

#else
    UNUSED(para);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
    return 0;
}

#ifdef AIR_HFP_DNN_PATH_ENABLE
int32_t digital_gain_times_of_db_last_8;
int32_t digital_gain_times_of_db_last_9;
void gain_change_detect(U8 mic_map_table)
{
    if(mic_map_table == 8) {
        if(digital_gain_times_of_db_last_8 != afe_audio_get_input_digital_gain(mic_map_table)) {
            DSP_MW_LOG_I("[stream_function_gain_process] mic_map_table[%d] gain update to 0x%x", 2, mic_map_table, afe_audio_get_input_digital_gain(mic_map_table));
        }
        digital_gain_times_of_db_last_8 = afe_audio_get_input_digital_gain(mic_map_table);
    } else if (mic_map_table == 9) {
        if(digital_gain_times_of_db_last_9 != afe_audio_get_input_digital_gain(mic_map_table)) {
            DSP_MW_LOG_I("[stream_function_gain_process] mic_map_table[%d] gain update to 0x%x", 2, mic_map_table, afe_audio_get_input_digital_gain(mic_map_table));
        }
        digital_gain_times_of_db_last_9 = afe_audio_get_input_digital_gain(mic_map_table);
    }
}
#endif
bool stream_function_gain_process(void *para)
{
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    uint32_t i, channel_number;
    int32_t digital_gain_times_of_db;
    DSP_STREAMING_PARA_PTR stream_ptr;
    sw_gain_entry gain_entry;
    uint32_t sample;
    U8* mic_map_table;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    mic_map_table = stream_function_get_working_buffer(para);
    channel_number = stream_function_get_channel_number(para);

    if (stream_function_get_output_resolution(para) == RESOLUTION_16BIT) {
        gain_entry = dsp_apply_sw_gain_16bit;
        sample = stream_function_get_output_size(para)/sizeof(S16);
    } else {
        gain_entry = dsp_apply_sw_gain_32bit;
        sample = stream_function_get_output_size(para)/sizeof(S32);
    }

    for (i=1 ; i<=channel_number ; i++)
    {

        digital_gain_times_of_db = (int32_t)((S16)afe_audio_get_input_digital_gain(mic_map_table[i-1])/100);
#ifdef AIR_HFP_DNN_PATH_ENABLE
        if(i==1) {
            gain_change_detect(mic_map_table[i-1]);
        }
#endif
        gain_entry(stream_function_get_inout_buffer(para, i),
                   sample,
                   digital_gain_times_of_db);
        //printf("gain %d addr:0x%x",i,stream_function_get_inout_buffer(para, i));
        /*if(i==1){
        LOG_AUDIO_DUMP((U8*)stream_function_get_inout_buffer(para, i), (U32)stream_function_get_output_size(para), VOICE_TX_MIC_0);
        }else if (i == 2){
        LOG_AUDIO_DUMP((U8*)stream_function_get_inout_buffer(para, i), (U32)stream_function_get_output_size(para), VOICE_TX_MIC_1);
        }else if (i == 3){
        LOG_AUDIO_DUMP((U8*)stream_function_get_inout_buffer(para, i), (U32)stream_function_get_output_size(para), VOICE_TX_MIC_2);
        }else if (i == 4){
        LOG_AUDIO_DUMP((U8*)stream_function_get_inout_buffer(para, i), (U32)stream_function_get_output_size(para), VOICE_TX_MIC_3);
        }*/

         /*if(i==1){
            LOG_AUDIO_DUMP((U8*)stream_function_get_inout_buffer(para, i), (U32)stream_function_get_output_size(para), SOURCE_IN1);
            }else if (i == 2){
            LOG_AUDIO_DUMP((U8*)stream_function_get_inout_buffer(para, i), (U32)stream_function_get_output_size(para), SOURCE_IN2);
            }else if (i == 3){
            LOG_AUDIO_DUMP((U8*)stream_function_get_inout_buffer(para, i), (U32)stream_function_get_output_size(para), SOURCE_IN3);
            }else if (i == 4){
            LOG_AUDIO_DUMP((U8*)stream_function_get_inout_buffer(para, i), (U32)stream_function_get_output_size(para), SOURCE_IN4);
         }*/
    }

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)(stream_function_get_inout_buffer(para, 1)), (U32)(stream_function_get_output_size(para)), SOURCE_IN3);
    if(channel_number > 1) {
        LOG_AUDIO_DUMP((U8*)(stream_function_get_inout_buffer(para, 2)), (U32)(stream_function_get_output_size(para)), SOURCE_IN4);
    }
#endif
    //DSP_MW_LOG_I("SOURCE_IN3 0x%x SOURCE_IN4 0x%x",2,stream_function_get_inout_buffer(para, 1),stream_function_get_inout_buffer(para, 2));

#else
    UNUSED(para);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */

    return 0;
}


#ifdef MTK_SPECIAL_FUNCTIONS_ENABLE
/**
 * Fixed gain table for mic functions
 * Gain setting depend on input sequence
 * unit:db
 */
const int32_t dsp_fixed_sw_gain_table[]= {
    0,//1st input channel
    0,//2nd input channel
    0,//3rd input channel
    0,//4th input channel
};

bool stream_function_fixed_gain_initialize(void *para)
{
    UNUSED(para);
    return false;
}

bool stream_function_fixed_gain_process(void *para)
{
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    uint32_t i, channel_number;
    DSP_STREAMING_PARA_PTR stream_ptr;
    sw_gain_entry gain_entry;
    uint32_t sample;
    U8* mic_map_table;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    mic_map_table = stream_function_get_working_buffer(para);
    channel_number = stream_function_get_channel_number(para);

    if (stream_function_get_output_resolution(para) == RESOLUTION_16BIT) {
        gain_entry = dsp_apply_sw_gain_16bit;
        sample = stream_function_get_output_size(para)/sizeof(S16);
    } else {
        gain_entry = dsp_apply_sw_gain_32bit;
        sample = stream_function_get_output_size(para)/sizeof(S32);
    }

    for (i=1 ; i<=channel_number ; i++)
    {
        gain_entry(stream_function_get_inout_buffer(para, i),
                   sample,
                   dsp_fixed_sw_gain_table[i-1]);
    }
#endif /* AIR_AUDIO_HARDWARE_ENABLE */

    return false;
}

#endif

