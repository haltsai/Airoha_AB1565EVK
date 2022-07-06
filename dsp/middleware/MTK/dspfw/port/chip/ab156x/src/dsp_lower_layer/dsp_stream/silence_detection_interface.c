/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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

#include "silence_detection_interface.h"
#include "hal_audio_volume.h"
#include "audio_nvdm_common.h"

#define abs32(x) ( (x >= 0) ? x : (-x) )
extern afe_volume_digital_control_t afe_digital_gain[AFE_HW_DIGITAL_GAIN_NUM];
static SD_INSTANCE_t SD_Parameter;
static U8 isInitial = 0;
static BOOL NvKeyfromCM4 = FALSE;
static BOOL IniDataUpdate = FALSE;
static S32 HW_Gain = 100;

U32 Get_AutoPowerOff_Time(void)
{
    return SD_Parameter.NvKey.AutoPowerOff_Time;
}

U32 Get_NLE_Time(void)
{
    return SD_Parameter.NvKey.NLE_Time;
}

U32 Get_ExtAmpOff_Time(void)
{
    return SD_Parameter.NvKey.ExtAmpOff_Time;
}

U8 Sink_Audio_SilenceDetection_Get_InitCnt(void)
{
    return isInitial;
}

void Sink_Audio_SilenceDetection_Load_Nvkey(void *nvkey)
{
    DSP_MW_LOG_I("[SD]load nvkey",0);
    memcpy(&(SD_Parameter.NvKey), nvkey, sizeof(SD_NVKEY_STATE));
    NvKeyfromCM4 = TRUE;
}

void Sink_Audio_SilenceDetection_TH_Update(S32 hw_gain)
{
    S32 Gain = 0;
    if(hw_gain != HW_Gain){
        HW_Gain = hw_gain;
        Gain = HW_Gain/100;
        SD_Parameter.AutoPowerOff_TH_16 = (S32)afe_calculate_digital_gain_index((SD_Parameter.NvKey.AutoPowerOff_TH_dB)*100 - HW_Gain,0X7FFF);//16bits
        SD_Parameter.NLE_TH_16 = (S32)afe_calculate_digital_gain_index((SD_Parameter.NvKey.NLE_TH_dB)*100 - HW_Gain,0X7FFF);//16bits
        SD_Parameter.ExtAmpOff_TH_16 = (S32)afe_calculate_digital_gain_index((SD_Parameter.NvKey.ExtAmpOff_TH_dB)*100 - HW_Gain,0X7FFF);//16bits

        SD_Parameter.AutoPowerOff_TH_32 = (S32)afe_calculate_digital_gain_index((SD_Parameter.NvKey.AutoPowerOff_TH_dB)*100 - HW_Gain,0X7FFFFFFF);//32bits
        SD_Parameter.NLE_TH_32 = (S32)afe_calculate_digital_gain_index((SD_Parameter.NvKey.NLE_TH_dB)*100 - HW_Gain,0X7FFFFFFF);//32bits
        SD_Parameter.ExtAmpOff_TH_32 = (S32)afe_calculate_digital_gain_index((SD_Parameter.NvKey.ExtAmpOff_TH_dB)*100 - HW_Gain,0X7FFFFFFF);//32bits
        DSP_MW_LOG_I("[SD]TH Update, HW Gain:%ddB",1,Gain);
        DSP_MW_LOG_I("[SD]APO TH:%ddB,TH:%d(16Bits),TH:%d(32Bits)",3,SD_Parameter.NvKey.AutoPowerOff_TH_dB - Gain,SD_Parameter.AutoPowerOff_TH_16,SD_Parameter.AutoPowerOff_TH_32);
        DSP_MW_LOG_I("[SD]NLE TH:%ddB,TH:%d(16Bits),TH:%d(32Bits)",3,SD_Parameter.NvKey.NLE_TH_dB - Gain,SD_Parameter.NLE_TH_16,SD_Parameter.NLE_TH_32);
        DSP_MW_LOG_I("[SD]EAO TH:%ddB,TH:%d(16Bits),TH:%d(32Bits)",3,SD_Parameter.NvKey.ExtAmpOff_TH_dB - Gain,SD_Parameter.ExtAmpOff_TH_16,SD_Parameter.ExtAmpOff_TH_32);
    }
}
        
        
void Sink_Audio_SilenceDetection_Register(SINK sink)
{
    FuncPtr APO_FunPtr = SD_Parameter.APO_FunPtr;
    FuncPtr NLE_FunPtr = SD_Parameter.NLE_FunPtr;
    FuncPtr EAO_FunPtr = SD_Parameter.EAO_FunPtr;

    if(NvKeyfromCM4){
        SD_Parameter.RegisteredSink[sink->type] = TRUE;
    }
    
    if((!isInitial) && NvKeyfromCM4){
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&(SD_Parameter.StartCnt));

        SD_Parameter.APO_MaxValue_16 = 0;
        SD_Parameter.NLE_MaxValue_16 = 0;
        SD_Parameter.EAO_MaxValue_16 = 0;
        SD_Parameter.APO_MaxValue_32 = 0;
        SD_Parameter.NLE_MaxValue_32 = 0;
        SD_Parameter.EAO_MaxValue_32 = 0;
        
        SD_Parameter.SampleCntAccumulate = 0;

        if(NvKeyfromCM4 && (!IniDataUpdate)){
            Sink_Audio_SilenceDetection_TH_Update(afe_digital_gain[AFE_HW_DIGITAL_GAIN1].index);
            IniDataUpdate = TRUE;
        }
        
        if(SD_Parameter.APO_SilenceFlag != TRUE){
            SD_Parameter.APO_SilenceFlag = TRUE;
            if(SD_Parameter.NvKey.APO_isEnable){
                APO_FunPtr(SD_Parameter.APO_SilenceFlag);
            }
        }

        if(SD_Parameter.NLE_SilenceFlag != TRUE){
            SD_Parameter.NLE_SilenceFlag = TRUE;
            if(SD_Parameter.NvKey.NLE_isEnable){
                NLE_FunPtr(SD_Parameter.NLE_SilenceFlag);
            }
        }

        if(SD_Parameter.EAO_SilenceFlag != TRUE){
            SD_Parameter.EAO_SilenceFlag = TRUE;
            if(SD_Parameter.NvKey.EAO_isEnable){
                EAO_FunPtr(SD_Parameter.EAO_SilenceFlag);  
            }
        }
        
    }
    if(NvKeyfromCM4 && IniDataUpdate){
        isInitial++;
    }
    DSP_MW_LOG_I("[SD]Silence Detection Init SinkType:0x%x Cnt:%d",2,sink->type,isInitial);
}

void Sink_Audio_SilenceDetection_Unregister(SINK sink)
{
    FuncPtr APO_FunPtr = SD_Parameter.APO_FunPtr;
    FuncPtr NLE_FunPtr = SD_Parameter.NLE_FunPtr;
    FuncPtr EAO_FunPtr = SD_Parameter.EAO_FunPtr;
    
    SD_Parameter.RegisteredSink[sink->type] = FALSE;
    isInitial--;
    
    if(!isInitial){   
        
        if(SD_Parameter.APO_SilenceFlag != TRUE){
            SD_Parameter.APO_SilenceFlag = TRUE;
            if(SD_Parameter.NvKey.APO_isEnable){
                APO_FunPtr(SD_Parameter.APO_SilenceFlag);
            }
        }

        if(SD_Parameter.NLE_SilenceFlag != TRUE){
            SD_Parameter.NLE_SilenceFlag = TRUE;
            if(SD_Parameter.NvKey.NLE_isEnable){
                NLE_FunPtr(SD_Parameter.NLE_SilenceFlag);
            }
        }

        if(SD_Parameter.EAO_SilenceFlag != TRUE){
            SD_Parameter.EAO_SilenceFlag = TRUE;
            if(SD_Parameter.NvKey.EAO_isEnable){
                EAO_FunPtr(SD_Parameter.EAO_SilenceFlag);  
            }
        }
    }
    
    DSP_MW_LOG_I("[SD]Silence Detection Close SinkType:0x%x Cnt:%d",2,sink->type,isInitial);
}

void Sink_Audio_SilenceDetection_Init(VOID *APO_FunPtr,VOID *NLE_FunPtr,VOID *EAO_FunPtr)
{
    U32 i = 0;
    for(i = 0;i < SINK_TYPE_MAX;i++){
        SD_Parameter.RegisteredSink[i] = FALSE;
    }
    SD_Parameter.APO_FunPtr = APO_FunPtr;
    SD_Parameter.NLE_FunPtr = NLE_FunPtr;
    SD_Parameter.EAO_FunPtr = EAO_FunPtr;

    SD_Parameter.APO_MaxValue_16 = 0;
    SD_Parameter.NLE_MaxValue_16 = 0;
    SD_Parameter.EAO_MaxValue_16 = 0;
    SD_Parameter.APO_MaxValue_32 = 0;
    SD_Parameter.NLE_MaxValue_32 = 0;
    SD_Parameter.EAO_MaxValue_32 = 0;

    SD_Parameter.StartCnt = 0XFFFFFFFF;
    SD_Parameter.Mutex = FALSE;

    if(NvKeyfromCM4){
        Sink_Audio_SilenceDetection_TH_Update(afe_digital_gain[AFE_HW_DIGITAL_GAIN1].index);
        IniDataUpdate = TRUE;
    }

    DSP_MW_LOG_I("[SD]APO_EN:%d, Time:%ds",2,SD_Parameter.NvKey.APO_isEnable,SD_Parameter.NvKey.AutoPowerOff_Time);
    DSP_MW_LOG_I("[SD]NLE_EN:%d, Time:%ds",2,SD_Parameter.NvKey.NLE_isEnable,SD_Parameter.NvKey.NLE_Time);
    DSP_MW_LOG_I("[SD]EAO_EN:%d, Time:%ds",2,SD_Parameter.NvKey.EAO_isEnable,SD_Parameter.NvKey.ExtAmpOff_Time);
    
    SD_Parameter.DetectTime_s = Silence_DetectTime_s;
    SD_Parameter.SampleCntAccumulate = 0;

    SD_Parameter.APO_SilenceFlag = FALSE;
    SD_Parameter.NLE_SilenceFlag = FALSE;
    SD_Parameter.EAO_SilenceFlag = FALSE;
    
}

void Sink_Audio_SilenceDetection(VOID* SrcBuf, U16 CopySize, SINK sink)
{
    FuncPtr APO_FunPtr = SD_Parameter.APO_FunPtr;
    FuncPtr NLE_FunPtr = SD_Parameter.NLE_FunPtr;
    FuncPtr EAO_FunPtr = SD_Parameter.EAO_FunPtr;
    S32 AutoPowerOff_TH;
    S32 ExtAmpOff_TH;
    S32 NLE_TH;
    U32 format = sink->param.audio.format_bytes;
    U32 SampleCnt = CopySize/format;    
    S32 *APO_Max;
    S32 *NLE_Max;
    S32 *EAO_Max;
    U32 NowCnt;
    S16 *Buf_16;
    S32 *Buf_32;
    U32 i = 0;
    
    if((SD_Parameter.NvKey.NLE_isEnable || SD_Parameter.NvKey.APO_isEnable || SD_Parameter.NvKey.NLE_isEnable) && SD_Parameter.RegisteredSink[sink->type]){

        Sink_Audio_SilenceDetection_TH_Update(afe_digital_gain[AFE_HW_DIGITAL_GAIN1].index);

        if(format == 2){//16bits  
            Buf_16 = (S16*)SrcBuf;
            APO_Max = &(SD_Parameter.APO_MaxValue_16);
            NLE_Max = &(SD_Parameter.NLE_MaxValue_16);
            EAO_Max = &(SD_Parameter.EAO_MaxValue_16);
            AutoPowerOff_TH = SD_Parameter.AutoPowerOff_TH_16;
            ExtAmpOff_TH = SD_Parameter.ExtAmpOff_TH_16;
            NLE_TH = SD_Parameter.NLE_TH_16;
        }else if(format == 4){//32bits
            Buf_32 = (S32*)SrcBuf;
            APO_Max = &(SD_Parameter.APO_MaxValue_32);
            NLE_Max = &(SD_Parameter.NLE_MaxValue_32);
            EAO_Max = &(SD_Parameter.EAO_MaxValue_32);
            AutoPowerOff_TH = SD_Parameter.AutoPowerOff_TH_32;
            ExtAmpOff_TH = SD_Parameter.ExtAmpOff_TH_32;
            NLE_TH = SD_Parameter.NLE_TH_32;
        }
        
        if (format == 2) {//16bits      
            for (i = 0; i < SampleCnt; i++) {
                
                if(sink->type != SINK_TYPE_VP_AUDIO && (*(Buf_16+i)) > *APO_Max){
                    *APO_Max = (*(Buf_16+i));
                }
    
                if((*(Buf_16+i)) > *NLE_Max){
                    *NLE_Max = (*(Buf_16+i));
                }

                if((*(Buf_16+i)) > *EAO_Max){
                    *EAO_Max = (*(Buf_16+i));
                }

                if(SD_Parameter.NvKey.EAO_isEnable && ((*(Buf_16+i)) >= ExtAmpOff_TH) && SD_Parameter.EAO_SilenceFlag == TRUE){
                    SD_Parameter.EAO_SilenceFlag = FALSE;
                    EAO_FunPtr(SD_Parameter.EAO_SilenceFlag);
                }
            }
        } else if (format == 4) {//32bits
            for (i = 0; i < SampleCnt; i++) {
                
                if(sink->type != SINK_TYPE_VP_AUDIO && (*(Buf_32+i)) > *APO_Max){
                    *APO_Max = (*(Buf_32+i));
                }
    
                if((*(Buf_32+i)) > *NLE_Max){
                    *NLE_Max = (*(Buf_32+i));
                }

                if((*(Buf_32+i)) > *EAO_Max){
                    *EAO_Max = (*(Buf_32+i));
                }

                if(SD_Parameter.NvKey.EAO_isEnable && ((*(Buf_32+i)) >= ExtAmpOff_TH) && SD_Parameter.EAO_SilenceFlag == TRUE){
                    SD_Parameter.EAO_SilenceFlag = FALSE;
                    EAO_FunPtr(SD_Parameter.EAO_SilenceFlag);
                }
            }
        }
        if(SD_Parameter.Mutex == FALSE){
            SD_Parameter.Mutex = TRUE;
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&NowCnt);
            if((NowCnt > SD_Parameter.StartCnt)&&((NowCnt - SD_Parameter.StartCnt) >= (SD_Parameter.DetectTime_s*1000000))){
                U32 TempCnt;
                DSP_MW_LOG_I("[TEST] $%d$ $%d$ $%d$ $%d$ $%d$ $%d$",6,NowCnt,SD_Parameter.StartCnt,SD_Parameter.DetectTime_s*1000000,TempCnt,TempCnt - (NowCnt - SD_Parameter.StartCnt - SD_Parameter.DetectTime_s*1000000),sink->type);
                SD_Parameter.StartCnt = NowCnt;
                DSP_MW_LOG_I("[SD]APO_Max:%d|0x%x, NLE_Max:%d|0x%x, EAO_Max:%d|0x%x",6,*APO_Max,*APO_Max,*NLE_Max,*NLE_Max,*EAO_Max,*EAO_Max);

                if(SD_Parameter.NvKey.APO_isEnable){
                    if((SD_Parameter.APO_MaxValue_32 < SD_Parameter.AutoPowerOff_TH_32) && (SD_Parameter.APO_MaxValue_16 < SD_Parameter.AutoPowerOff_TH_16) && (SD_Parameter.APO_SilenceFlag == FALSE)){
                        SD_Parameter.APO_SilenceFlag = TRUE;
                        APO_FunPtr(SD_Parameter.APO_SilenceFlag);
                    }else if(((SD_Parameter.APO_MaxValue_32 >= SD_Parameter.AutoPowerOff_TH_32) || (SD_Parameter.APO_MaxValue_16 >= SD_Parameter.AutoPowerOff_TH_16)) && SD_Parameter.APO_SilenceFlag == TRUE){
                        SD_Parameter.APO_SilenceFlag = FALSE;
                        APO_FunPtr(SD_Parameter.APO_SilenceFlag);
                    }
                }

                if(SD_Parameter.NvKey.NLE_isEnable){//Reserved for NLE
                    if((SD_Parameter.NLE_MaxValue_32 < SD_Parameter.NLE_TH_32) && (SD_Parameter.NLE_MaxValue_16 < SD_Parameter.NLE_TH_16)){
                        SD_Parameter.NLE_SilenceFlag = TRUE;
                        NLE_FunPtr(SD_Parameter.NLE_SilenceFlag);
                    }
                }

                if(SD_Parameter.NvKey.EAO_isEnable){
                    if((SD_Parameter.EAO_MaxValue_32 < SD_Parameter.ExtAmpOff_TH_32) && (SD_Parameter.EAO_MaxValue_16 < SD_Parameter.ExtAmpOff_TH_16)){
                        SD_Parameter.EAO_SilenceFlag = TRUE;
                        EAO_FunPtr(SD_Parameter.EAO_SilenceFlag);
                    }           
                }

                SD_Parameter.APO_MaxValue_16 = 0;
                SD_Parameter.NLE_MaxValue_16 = 0;
                SD_Parameter.EAO_MaxValue_16 = 0;
                SD_Parameter.APO_MaxValue_32 = 0;
                SD_Parameter.NLE_MaxValue_32 = 0;
                SD_Parameter.EAO_MaxValue_32 = 0;
            }
            SD_Parameter.Mutex = FALSE;
        }


    }

    return;
}
