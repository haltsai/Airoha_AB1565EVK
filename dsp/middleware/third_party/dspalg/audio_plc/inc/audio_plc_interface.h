/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

#ifndef AUDIO_PLC_INTERFACE_H
#define AUDIO_PLC_INTERFACE_H

#include "types.h"
//#include "DSP_Utilities.h"
#include "dsp_utilities.h"
#include "common.h"

#define MAX_PLC_MEM_SIZE                (40716)//(29472)//(29464)//get from get_plc_memsize()
//#define BUF_MEM_SIZE                    2560//3072//128 samples *  5 queues * 4 bytes

#define AUDIO_PLC_MEMSIZE  (sizeof(AUDIO_PLC_SCRATCH_t))
typedef struct AUDIO_PLC_PARA_s
{
    U32                 bfi;
    U32                 pre_bfi;
    U32                 bad_to_good_flag;
    U16                 in_max_size;
    U16                 codec_frame_size;
    //----------plc_option setting----------------------//
    //plc_option.b[0] = 1(Enable PRE_EMP), 0(Disable PRE_EMP)
    //plc_option.b[1] = 1(2-channel), 0(1-channel)
    //plc_option = 0 -> b[1] = 0, b[0] = 0
    //plc_option = 1 -> b[1] = 0, b[0] = 1
    //plc_option = 2 -> b[1] = 1, b[0] = 0
    //plc_option = 3 -> b[1] = 1, b[0] = 1
    U8                  init_done;
    //U16                 buf_wo;
    //U16                 buf_ro;
    //U16                 buf_count;
    //U16                 buf_mem_size;
    DSP_ALIGN8 S8       p_plc_mem_ext[MAX_PLC_MEM_SIZE];
    //DSP_ALIGN8 S8       buf_L[BUF_MEM_SIZE];
    //DSP_ALIGN8 S8       buf_R[BUF_MEM_SIZE];
} AUDIO_PLC_SCRATCH_t, *AUDIO_PLC_SCRATCH_PTR_t;

typedef struct dsp_audio_plc_ctrl_s
{
    U8              enable;
    U16             outputsize;
} DSP_AUDIO_PLC_CTRL_t;


EXTERN bool Audio_PLC_Init (VOID* para);

EXTERN bool Audio_PLC_Process (VOID* para);

EXTERN void Audio_PLC_ctrl (dsp_audio_plc_ctrl_t audio_plc_ctrl);

EXTERN U16 Audio_PLC_Get_OutputSize (void);

EXTERN void Audio_PLC_Modify_OutputSize (U16 outputsize);


#endif /* AUDIO_PLC_INTERFACE_H */
