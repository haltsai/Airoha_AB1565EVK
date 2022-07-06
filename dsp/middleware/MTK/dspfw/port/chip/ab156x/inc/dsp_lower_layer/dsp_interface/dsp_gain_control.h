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

#ifndef _DSP_GAIN_CONTROL_H_
#define _DSP_GAIN_CONTROL_H_

#include "types.h"
#include "dsp_utilities.h"
#include "dsp_control.h"
#include "hal_ccni.h"
#define MAX_TOTAL_NO_OF_OUT_IDX (101)
#define MAX_TOTAL_NO_OF_IN_IDX 	(16)

typedef struct DSP_OUT_GAIN_TABLE_CTRL_s
{
	U16 TotalIndex;
	U16 OutGainIndex[MAX_TOTAL_NO_OF_OUT_IDX];
} PACKED DSP_OUT_GAIN_TABLE_CTRL_t;

typedef struct DSP_ANALOG_OUT_GAIN_TABLE_CTRL_s
{
	U16 TotalIndex;
	U32 GainIndex[MAX_TOTAL_NO_OF_OUT_IDX];
} PACKED DSP_ANALOG_OUT_GAIN_TABLE_CTRL_t;

typedef struct DSP_DIGITAL_IN_GAIN_TABLE_CTRL_s
{
	U16 TotalIndex;
	U16 GainIndex[MAX_TOTAL_NO_OF_IN_IDX];
} PACKED DSP_DIGITAL_IN_GAIN_TABLE_CTRL_t;

typedef struct DSP_ANALOG_IN_GAIN_TABLE_CTRL_s
{
	U16 TotalIndex;
	U16 GainIndex[MAX_TOTAL_NO_OF_IN_IDX];
} PACKED DSP_ANALOG_IN_GAIN_TABLE_CTRL_t;
typedef union DSP_GAIN_DIG_OUT_CTRL_u
{
	struct DSP_GAIN_DIG_OUT_CTRL_s
	{
		S16 A2DP;
	    S16 LINE;
	    S16 SCO;
	    S16 VC;
	    S16 VP;
	    S16 RT;
	} Field;

	S16 Reg[AUDIO_GAIN_MAX_COMPONENT];
} DSP_GAIN_DIG_OUT_CTRL_t;

typedef union DSP_GAIN_ANA_IN_CTRL_u
{
	struct DSP_GAIN_ANA_IN_CTRL_s
	{
		S16 LineGain_L;
	    S16 LineGain_R;
	    S16 MicGain_L;
	    S16 MicGain_R;
	    S16 AncMicGain_L;
	    S16 AncMicGain_R;
	    S16 VadMicGain;
	} Field;

	S16 Reg[AU_AFE_IN_COMPONENT_NO];
} DSP_GAIN_ANA_IN_CTRL_t;


typedef union DSP_GAIN_DIG_IN_CTRL_u
{
	struct DSP_GAIN_DIG_IN_CTRL_s
	{
		S16 Gain;
	} Field;

	S16 Reg[AU_DFE_IN_COMPONENT_NO];
} DSP_GAIN_DIG_IN_CTRL_t;


typedef union DSP_GAIN_ANA_OUT_CTRL_u
{
	struct DSP_GAIN_ANA_OUT_CTRL_s
	{
		S32 EarGain;
	    S32 DepopGain;
	} Field;

	S32 Reg[AU_AFE_OUT_COMPONENT_NO];
} DSP_GAIN_ANA_OUT_CTRL_t;


typedef struct DSP_IN_GAIN_PERCENTAGE_CTRL_s
{
	S16 Percentage;
	BOOL Enable;
} PACKED DSP_IN_GAIN_PERCENTAGE_CTRL_t;


typedef struct DSP_STATIC_GAIN_TABLE_s
{
	DSP_GAIN_DIG_IN_CTRL_t DigitalIn;
	DSP_GAIN_ANA_IN_CTRL_t AnalogIn;
} PACKED DSP_STATIC_GAIN_TABLE_t;


typedef struct DSP_STATIC_PERCENTAGE_GAIN_TABLE_s
{
	DSP_DIGITAL_IN_GAIN_TABLE_CTRL_t 	DInPercentage;
	DSP_ANALOG_IN_GAIN_TABLE_CTRL_t 	AInPercentageL;
	DSP_ANALOG_IN_GAIN_TABLE_CTRL_t 	AInPercentageR;
	DSP_IN_GAIN_PERCENTAGE_CTRL_t 		InputPercentCtrl;
	DSP_ANALOG_OUT_GAIN_TABLE_CTRL_t 	AOutPercentage;
} PACKED DSP_STATIC_PERCENTAGE_GAIN_TABLE_t;


typedef struct DSP_STATIC_GAIN_CTONROL_s
{
	DSP_STATIC_PERCENTAGE_GAIN_TABLE_t* TablePtr;
	S16 GainPercentage;
} DSP_STATIC_GAIN_CTONROL_t;


typedef struct DSP_GAIN_CTRL_s
{
	DSP_ALIGN4 DSP_GAIN_DIG_OUT_CTRL_t DigitalOut;
	DSP_ALIGN4 S16 AnalogOut;
	#if 0
	DSP_ALIGN4 S16 DigitalInGain;
	DSP_ALIGN4 DSP_GAIN_ANA_OUT_CTRL_t AnalogOut;
	DSP_ALIGN4 DSP_GAIN_ANA_IN_CTRL_t AnalogIn;
	#else
	DSP_ALIGN4 DSP_STATIC_GAIN_TABLE_t GainTable;
	#endif
	DSP_ALIGN4 DSP_STATIC_GAIN_CTONROL_t StaticControl;
} DSP_GAIN_CTRL_t;
EXTERN VOID DSP_GC_Init (VOID);
EXTERN S16 DSP_GC_ConvertPercentageToIdx (S16 Percentage, S16 TotalIndex);
EXTERN S16 DSP_GC_GetDigitalGainIndexFromComponent(AUDIO_GAIN_COMPONENT_t Component, S16 TotalIndex);
EXTERN S16 DSP_GC_GetDigitalOutLevel (AUDIO_GAIN_COMPONENT_t Component);
EXTERN VOID DSP_GC_ResetAnalogOutGain (VOID);
EXTERN VOID DSP_GC_SetAnalogOutOnSequence (VOID);
EXTERN VOID DSP_GC_SetAnalogOutOffSequence (VOID);
EXTERN VOID DSP_GC_SetOutputVolume(hal_ccni_message_t msg, hal_ccni_message_t *ack);
EXTERN VOID DSP_GC_SetInputVolume(hal_ccni_message_t msg, hal_ccni_message_t *ack);
EXTERN VOID DSP_GC_MuteOutput(hal_ccni_message_t msg, hal_ccni_message_t *ack);
EXTERN VOID DSP_GC_MuteInput(hal_ccni_message_t msg, hal_ccni_message_t *ack);
EXTERN void dsp_gc_set_anc_volume(hal_ccni_message_t msg, hal_ccni_message_t *ack);
void DSP_GC_SetGainParameters(hal_ccni_message_t msg, hal_ccni_message_t *ack);
void DSP_GC_POWER_OFF_DAC_IMMEDIATELY(void);
#endif /* _DSP_GAIN_CONTROL_H_ */

