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
#include "audio_plc.h"
#include "config.h"
#include "audio_plc_interface.h"
//#include "DSP_Dump.h"
#include "dsp_dump.h"

#include "dsp_callback.h"

#include "common.h"
#include "dsp_para_plc_nvstruc.h"

#define QCONST16(x,bits) ((short)(.5+(x)*(((int)1)<<(bits))))
#define FRAME_LEN 512


#define SBC_FRAME_LEN 128 //128sample
#define AAC_FRAME_LEN 1024
#define LDAC_FRAME_LEN_IN882_96HZ 256


#define DUMP_AUDIO_PLC 0
#define AUDIO_PLC_MIPS_E 0


DSP_PARA_AUDIO_PLC_STRU plc_nvkey = {
	0x1,					// U8  ENABLE
	0x1,					// U8  REVISION
	0x0003,					// U16 option: b[1] = 1(2-channel     ), 0(1-channel      )
							//             b[0] = 1(Enable PRE_EMP), 0(Disable PRE_EMP)
	FRAME_LEN,				// S16 framesize
	3,						// S16 loss_count, default = 5
	965,					// S16 pitch_lag_max, 720, 965
	500,					// S16 pitch_lag_min, 100, 500
	QCONST16(0.8f, 15),		// S16 fade, 0.95
	1024,					// S16 max_period must be 2000 or 1024, max_period must be larger than pitch_lag_max or pitch_lag_min.
	0x0000,					// U16 reserve_2
	0x0000,					// U16 reserve_3
	0x0000,					// U16 reserve_4
	0x0000,					// U16 reserve_5
	0x0000,					// U16 reserve_6
	0x0000 					// U16 reserve_7
};


static DSP_AUDIO_PLC_CTRL_t g_audio_plc_ctrl;


U8 DSP_GetFuncStreamCodecType(VOID* para)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    DSP_FEATURE_TABLE_PTR  FeatureTablePtr;

    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    FeatureTablePtr = (DSP_FEATURE_TABLE_PTR)(stream_ptr->callback.FeatureTablePtr);
    return FeatureTablePtr->FeatureType;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 U16 DSP_GetStreamCodecOutSize(VOID* para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->codec_out_size;
}


bool Audio_PLC_Init (VOID* para);

bool Audio_PLC_Process (VOID* para);

bool Audio_PLC_Init (VOID* para){

    DSP_STREAMING_PARA_PTR stream_ptr;
    AUDIO_PLC_SCRATCH_PTR_t audio_plc_feature_ptr;
    U32 ch_num = stream_function_get_channel_number(para);//DSP_GetFuncStreamChannelNum(para);
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    audio_plc_feature_ptr = stream_function_get_working_buffer(para);//DSP_GetFuncMemoryInstancePtr(para);
    audio_plc_feature_ptr->init_done = 0;
    UNUSED(ch_num);
    DSP_MW_LOG_I("Audio_PLC_Init\r\n",0);

    return FALSE;
}

bool Audio_PLC_Process (VOID* para){
    DSP_STREAMING_PARA_PTR stream_ptr;
    AUDIO_PLC_SCRATCH_PTR_t audio_plc_feature_ptr;
    U32 ch_num = stream_function_get_channel_number(para);//DSP_GetFuncStreamChannelNum(para);
    void *Buf_L;
    void *Buf_R;
    U16 streamsize;
    U16 streamlength;
    U32 plcProcOutLen;
    stream_samplerate_t sameplerate;

    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    audio_plc_feature_ptr = stream_function_get_working_buffer(para);//DSP_GetFuncMemoryInstancePtr(para);
    Buf_L = stream_function_get_1st_inout_buffer(para);//DSP_GetFuncStream1Ptr(para);
    Buf_R = stream_function_get_2nd_inout_buffer(para);//DSP_GetFuncStream2Ptr(para);
    streamlength = DSP_GetStreamCodecOutSize(para);
    streamsize = streamlength/sizeof(U32);
    sameplerate = stream_function_get_samplingrate(para);
    if(audio_plc_feature_ptr->init_done == 0 && g_audio_plc_ctrl.enable == 1){
        plc_nvkey.option  = (ch_num>1) ? 3:1;	// 3(stereo), 1(mono)

        if(streamsize==SBC_FRAME_LEN || streamsize==AAC_FRAME_LEN || streamsize==LDAC_FRAME_LEN_IN882_96HZ){
            audio_plc_feature_ptr->codec_frame_size = streamsize;
            plc_nvkey.framesize = streamsize;
        }else{

            platform_assert("Audio PLC not support framesize.",__FILE__,__LINE__);
        }
        if(sameplerate>FS_RATE_48K){
            plc_nvkey.pitch_lag_max = 965*2;
            plc_nvkey.pitch_lag_min = 500*2;
            plc_nvkey.max_period  = 2000;
        }
        if ( (plc_nvkey.pitch_lag_max > plc_nvkey.max_period) || ((plc_nvkey.pitch_lag_min > plc_nvkey.max_period)) ) {
            DSP_MW_LOG_I("max_period must be larger than pitch_lag_max or pitch_lag_min.\n",0);
        }
        if(MAX_PLC_MEM_SIZE >= get_plc_memsize(ch_num,(int) plc_nvkey.max_period)&& ch_num <= 2){
            audio_plc_feature_ptr->init_done = 1;
            PLC_Init(audio_plc_feature_ptr->p_plc_mem_ext, &plc_nvkey);
            DSP_MW_LOG_I("Audio PLC init get memsize %d framesize %d\r\n", 2,get_plc_memsize(ch_num, plc_nvkey.max_period),audio_plc_feature_ptr->codec_frame_size);
        }else{
            DSP_MW_LOG_E("Audio PLC error:memory not enough %d < %d\r\n", 2, MAX_PLC_MEM_SIZE,get_plc_memsize(ch_num, plc_nvkey.max_period));
        }
    }

    if(streamsize == audio_plc_feature_ptr->codec_frame_size && audio_plc_feature_ptr->init_done == 1)
    {

        audio_plc_feature_ptr->pre_bfi= audio_plc_feature_ptr->bfi;
        if(stream_codec_get_mute_flag(para) == TRUE && g_audio_plc_ctrl.enable == 1){//DSP_GetCodecMuteFlag(para) == TRUE
            audio_plc_feature_ptr->bfi = 1;//bed frame
            //DSP_MW_LOG_I("Audio PLC bad frame\r\n", 0);
        }
        else{
            audio_plc_feature_ptr->bfi = 0;//good frame
            //DSP_MW_LOG_E("Audio PLC good frame\r\n", 0);
        }

        if(audio_plc_feature_ptr->pre_bfi==1 && audio_plc_feature_ptr->bfi == 0){
            audio_plc_feature_ptr->bad_to_good_flag = 1;//for decodec muted to good frame.
        }
        if(audio_plc_feature_ptr->p_plc_mem_ext !=NULL){

            //DSP_MW_LOG_E("Audio PLC bfi %d streamsize %d\r\n", 2,audio_plc_feature_ptr->bfi,streamsize*sizeof(U32));


            //PLC_Proc(audio_plc_feature_ptr->p_plc_mem_ext, Buf_L, Buf_R, audio_plc_feature_ptr->bfi);
            if(audio_plc_feature_ptr->bad_to_good_flag ==1){//for decodec muted to good frame.



                if(streamsize == SBC_FRAME_LEN)
                {
                    plcProcOutLen = PLC_Proc_128(audio_plc_feature_ptr->p_plc_mem_ext, Buf_L, Buf_R, audio_plc_feature_ptr->bad_to_good_flag);
                }
                else if(streamsize == AAC_FRAME_LEN)
                {
                    plcProcOutLen = PLC_Proc_512(audio_plc_feature_ptr->p_plc_mem_ext, Buf_L, Buf_R, audio_plc_feature_ptr->bad_to_good_flag);

                }
                else if(streamsize == LDAC_FRAME_LEN_IN882_96HZ)
                {
                    plcProcOutLen = PLC_Proc_256(audio_plc_feature_ptr->p_plc_mem_ext, Buf_L, Buf_R, audio_plc_feature_ptr->bad_to_good_flag);
                }
                else{
                    platform_assert("Audio PLC not support codec type.",__FILE__,__LINE__);
                }
                audio_plc_feature_ptr->bad_to_good_flag = 0;


            }else{

                if(DSP_GetFuncStreamCodecType(para) == CODEC_DECODER_SBC)
                {
                    plcProcOutLen = PLC_Proc_128(audio_plc_feature_ptr->p_plc_mem_ext, Buf_L, Buf_R, audio_plc_feature_ptr->bfi);
                }
                else if(DSP_GetFuncStreamCodecType(para) == CODEC_DECODER_AAC)
                {
                    plcProcOutLen = PLC_Proc_512(audio_plc_feature_ptr->p_plc_mem_ext, Buf_L, Buf_R, audio_plc_feature_ptr->bfi);

                }
                else if(streamsize == LDAC_FRAME_LEN_IN882_96HZ)
                {
                    plcProcOutLen = PLC_Proc_256(audio_plc_feature_ptr->p_plc_mem_ext, Buf_L, Buf_R, audio_plc_feature_ptr->bfi);
                }
                else{
                    platform_assert("Audio PLC not support codec type.",__FILE__,__LINE__);
                }


            }


            stream_function_modify_output_size(para,(plcProcOutLen*sizeof(U32)));
#if (DUMP_AUDIO_PLC)
            U8  tempbuffer[AAC_FRAME_LEN];
            U32 *src_buf_L = (U32 *)Buf_L;
            for (int i=0; i<plcProcOutLen; i++)
            {
                tempbuffer[i] = (U16)(src_buf_L[i] >> 24);
            }

            LOG_AUDIO_DUMP((U8*)tempbuffer, (plcProcOutLen*sizeof(U8)), AUDIO_SOURCE_IN_L);
            U8  tempbuffer_L[AAC_FRAME_LEN];
            if(audio_plc_feature_ptr->bfi == 1 || audio_plc_feature_ptr->bad_to_good_flag){

                for (int i=0; i<plcProcOutLen; i++)
                {
                    tempbuffer_L[i] = 127;
                }

            }else{

                for (int i=0; i<plcProcOutLen; i++)
                {
                    tempbuffer_L[i] = 0;
                }

            }

            LOG_AUDIO_DUMP((U8*)tempbuffer_L, plcProcOutLen, AUDIO_SOURCE_IN_R);
#endif
        }else{
            DSP_MW_LOG_I("Audio PLC get null p_plc_mem_ext\r\n", 0);
        }


    }else{

        //do nothing
        DSP_MW_LOG_I("Audio PLC get streamsize %d\r\n",1, streamsize);
    }


    return FALSE;
}

void Audio_PLC_ctrl (dsp_audio_plc_ctrl_t audio_plc_ctrl){

    g_audio_plc_ctrl.enable = audio_plc_ctrl.enable;
    DSP_MW_LOG_E("Audio_PLC_ctrl enable %d\r\n", 1,audio_plc_ctrl.enable);
}

U16 Audio_PLC_Get_OutputSize (void){
    U16 outputsize;
    outputsize = g_audio_plc_ctrl.outputsize;
    return outputsize;
}

void Audio_PLC_Modify_OutputSize (U16 outputsize){

    g_audio_plc_ctrl.outputsize = outputsize;

}




