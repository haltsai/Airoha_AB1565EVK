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
#ifndef _DSP_PARA_CHAT_VOL_H_
#define _DSP_PARA_CHAT_VOL_H_

#include "types.h"

/**
 * @brief Parameter for GAME CHAT VOLUME
 * @KeyID 0x
 */

#ifdef WIN32
	#define PACKED
	#pragma pack(1)
#else
	// #define PACKED __attribute__((__packed__))
#endif

typedef struct stru_game_chat_vol_para_s
{
	S32 GC_LPF_COEF_0;   /**< @Value 0x00088791 @Desc 1 */
	S32 GC_LPF_COEF_1;   /**< @Value 0x0004BFCB @Desc 1 */
	S32 GC_LPF_COEF_2;   /**< @Value 0x00088790 @Desc 1 */
	S32 GC_LPF_COEF_3;   /**< @Value 0xFFB89FE8 @Desc 1 */
	S32 GC_LPF_COEF_4;   /**< @Value 0x00322925 @Desc 1 */
	S32 GC_LPF_COEF_5;   /**< @Value 0x0042C52E @Desc 1 */
	S32 GC_LPF_COEF_6;   /**< @Value 0xFFF3C75F @Desc 1 */
	S32 GC_LPF_COEF_7;   /**< @Value 0x0042C52D @Desc 1 */
	S32 GC_LPF_COEF_8;   /**< @Value 0xFFB80029 @Desc 1 */
	S32 GC_LPF_COEF_9;   /**< @Value 0x00643629 @Desc 1 */
	S32 GC_LPF_COEF_10;   /**< @Value 0x00400000 @Desc 1 */
	S32 GC_HPF_COEF_0;   /**< @Value 0x00747EC9 @Desc 1 */
	S32 GC_HPF_COEF_1;   /**< @Value 0xFF8B84CF @Desc 1 */
	S32 GC_HPF_COEF_2;   /**< @Value 0x00747EC8 @Desc 1 */
	S32 GC_HPF_COEF_3;   /**< @Value 0xFF8823A6 @Desc 1 */
	S32 GC_HPF_COEF_4;   /**< @Value 0x0070A471 @Desc 1 */
	S32 GC_HPF_COEF_5;   /**< @Value 0x007EEAD0 @Desc 1 */
	S32 GC_HPF_COEF_6;   /**< @Value 0xFF812916 @Desc 1 */
	S32 GC_HPF_COEF_7;   /**< @Value 0x007EEACF @Desc 1 */
	S32 GC_HPF_COEF_8;   /**< @Value 0xFF8173DF @Desc 1 */
	S32 GC_HPF_COEF_9;   /**< @Value 0x007D9185 @Desc 1 */
	S32 GC_HPF_COEF_10;   /**< @Value 0x00400000 @Desc 1 */
	S16 ALPHA_UP;					/**< @Value 0x1E00 @Desc 1 */
	S16 ALPHA_DN;					/**< @Value 0x0600 @Desc 1 */
	S16 MIN_DB;					/**< @Value 0xB000 @Desc 1 */
	U8	ENV_PERIOD;				/**< @Value 0x0A @Desc 1 */
	U8	ENV_SIZE;				/**< @Value 0x0C @Desc 1 */
	S32 RESERVE1;		 /**< @Value 0x00000000 @Desc 1 */
} PACKED DSP_PARA_GAME_CHAT_VOL_STRU;

#ifdef WIN32
	#pragma pack()
#endif

#endif /* _DSP_PARA_CHAT_VOL_H_ */
