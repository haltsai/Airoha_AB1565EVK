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
#include "audio_loopback_test_interface.h"
#include "types.h"

/**
 *
 *  External Symbols
 *
 */


/**
 *
 *  Buffer & Control
 *
 */
static AUDIO_LOOPBACK_TEST_CTRL_t AudioLoopbackCtrl;

// N9-DSP share information buffer (32 bytes)
typedef struct {
    uint32_t start_addr;       // start address of N9-DSP share buffer
    uint32_t read_offset;      // read pointer of N9-DSP share buffer
    uint32_t write_offset;     // write pointer of N9-DSP share buffer
    uint32_t next;             // next read position in buf for DSP
    uint32_t sampling_rate;    // for AWS clock skew
    uint16_t length;           // total length of N9-DSP share buffer
    uint8_t  bBufferIsFull;    // buffer full flag, when N9 find there is no free buffer for putting a packet,
                               // set this flag = 1, DSP will reset this flag when data be taken by DSP
    uint8_t  notify_count;     // notify count
    int32_t  drift_comp_val;   // long term drift compensation value
    uint32_t anchor_clk;       // long term drift anchor clk
    uint32_t asi_base;
    uint32_t asi_cur;
} n9_dsp_share_info_t;


VOID audio_loopback_test_inform_result_addr (uint32_t addr)
{
    n9_dsp_share_info_t *result_addr = (n9_dsp_share_info_t *)addr;
    AudioLoopbackCtrl.ThdN = &result_addr->start_addr;
    AudioLoopbackCtrl.SigPower = &result_addr->read_offset;
    AudioLoopbackCtrl.TotalPower = &result_addr->write_offset;
    AudioLoopbackCtrl.lib_process_flag = &result_addr->next;
}

bool Audio_Loopback_Test_Init (void *para)
{

    AudioLoopbackCtrl.SineIndex = 0;
    AudioLoopbackCtrl.BistIndex = 0;

    AudioLoopbackCtrl.BufPtr = (S32 *)stream_function_get_working_buffer(para);//DSP_GetFuncMemoryInstancePtr(para);

    return FALSE;
}


bool Audio_Loopback_Test_Process (void *para){
    S32* BufL   = (S32*)stream_function_get_1st_inout_buffer(para);//DSP_GetFuncStream1Ptr(para);
    //S32* BufR   = (S32*)stream_function_get_2nd_inout_buffer(para);//DSP_GetFuncStream2Ptr(para);
    //S32* Buf3   = (S32*)stream_function_get_3rd_inout_buffer(para);//DSP_GetFuncStream3Ptr(para);
    //S32* Buf4   = (S32*)stream_function_get_4th_inout_buffer(para);//DSP_GetFuncStream4Ptr(para);
    S32* Buf;
    U16 FrameSize = (U16)stream_function_get_output_size(para);//DSP_GetFuncStreamSize(para);
    U16 FrameSample = (U16)FrameSize/sizeof(U32);
    U16 i;

    Buf = BufL;

    for (i = 0 ; i < FrameSample ; i++)
    {
        AudioLoopbackCtrl.BufPtr[AudioLoopbackCtrl.BistIndex] = ((S32)Buf[i]);
        AudioLoopbackCtrl.BistIndex++;

        if (AudioLoopbackCtrl.BistIndex == AUDIO_LOOPBACK_TEST_FRAME_SIZE)
        {
            *AudioLoopbackCtrl.ThdN = calc_thd_n(AudioLoopbackCtrl.BufPtr, (int *)AudioLoopbackCtrl.SigPower, (int *)AudioLoopbackCtrl.TotalPower);
            (*AudioLoopbackCtrl.lib_process_flag) ++;  
            AudioLoopbackCtrl.BistIndex = 0;
            HAL_AUDIO_LOG_INFO("[AUDIO LOOPBACK TEST] ThdN=%d, SigPower=%d, TotalPower=%d, ThdN/32=%d, SigPower/32=%d, TotalPower/32=%d, lib_process_flag=%d", 7,(S32)*AudioLoopbackCtrl.ThdN,(S32)*AudioLoopbackCtrl.SigPower,(S32)*AudioLoopbackCtrl.TotalPower,(S32)*AudioLoopbackCtrl.ThdN/32,(S32)*AudioLoopbackCtrl.SigPower/32,(S32)*AudioLoopbackCtrl.TotalPower/32,(S32)*AudioLoopbackCtrl.lib_process_flag);
        }
    }
    stream_function_modify_output_resolution(para, RESOLUTION_32BIT);
    return FALSE;
}

