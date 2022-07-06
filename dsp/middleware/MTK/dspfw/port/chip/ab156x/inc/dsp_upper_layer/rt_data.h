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
 
#ifndef _RT_DATA_H_
#define _RT_DATA_H_

#include "types.h"
#include "config.h"

#define RINGTONE_ENABLE

#define RINGTONE_LIST					\
		RTONEDEF(SHORT_1_NOTE_1)		\
		RTONEDEF(SHORT_1_NOTE_2)		\
		RTONEDEF(SHORT_1_NOTE_3)		\
		RTONEDEF(SHORT_1_NOTE_4)		\
		RTONEDEF(SHORT_2_NOTE)			\
		RTONEDEF(MEDIAN_1_NOTE)			\
		RTONEDEF(LONG_1_NOTE_HIGH)		\
		RTONEDEF(LONG_1_NOTE_LOW)		\
		RTONEDEF(VERYLONG_1_NOTE_HIGH)	\
		RTONEDEF(VERYLONG_1_NOTE_LOW)	\
		RTONEDEF(RISING_2_NOTE)			\
		RTONEDEF(RISING_4_NOTE)			\
		RTONEDEF(FALLING_2_NOTE)		\
		RTONEDEF(FALLING_4_NOTE)		\
		RTONEDEF(LONG_RING_A)			\
		RTONEDEF(LONG_RING_B)			\
		RTONEDEF(LONG_RING_C)			\
		RTONEDEF(USER_DEFINED_A)		\

#define RTONEDEF(name)	name,

enum
{
	RINGTONE_LIST

	/*config tool can extend user-defined data here*/

	TOTAL_BEEP_PATTERNS,
};

#define TOTAL_BUILTIN_RINGTONE_LENGTH 500

typedef struct
{
	U16 offset;
	U16 dataLength;
} RINGTONE_RECORD_TYPE;

typedef struct driver_ringtone_data_st
{
	U32 totalRingtoneNo;
	U32 DummyAlignGap[3];
	RINGTONE_RECORD_TYPE ringtoneRecord[TOTAL_BEEP_PATTERNS];
	U8 ringtoneData[TOTAL_BUILTIN_RINGTONE_LENGTH];
} DRIVER_RINGTONE_DATA_TYPE, *DRIVER_RINGTONE_DATA_TYPE_PTR;

EXTERN U32 ALIGN(4) CODE gDriver_ringtone_data_init;

#endif /* _RT_DATA_H_ */

