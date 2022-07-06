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
#ifndef _DSP_PARA_PLC_H_
#define _DSP_PARA_PLC_H_

#include "types.h"

/**
 * @brief Parameter for DSP PLC algorithm
 * @KeyID 0xE140
 */

typedef struct stru_dsp_plc_para_s
{
	U8  ENABLE;                                        /**< @Value 0x01 @Desc 1 */
	U8  REVISION;                                      /**< @Value 0x01 @Desc 1 */
	U16 PLC_OFFSET;                                    /**< @Value 0x000E @Desc 1 */
	U16 PLC_OFFSET_mSBC;                               /**< @Value 0x001C @Desc 1 */
	U16 PLC_FramePartEn;                               /**< @Value 0x0001 @Desc 1 */
	U16 PLC_FramePartMethod;                           /**< @Value 0x0000 @Desc 1 */
	U16 PLC_CRCChkMethod;                              /**< @Value 0x0000 @Desc 1 */
	U16 PLC_MUTE_CONSECUTIVE_ERRS_DEFAULT;             /**< @Value 0x0008 @Desc 1 */
	U16 PLC_UNITY_GAIN_CONSECUTIVE_GOODFRM_DEFAULT;    /**< @Value 0x000E @Desc 1 */
	U16 PLC_MUTE_PE16_TH_DEFAULT;                      /**< @Value 0x000A @Desc 1 */
	U16 PLC_GAIN_UPDATE_PE16_TH_DEFAULT;               /**< @Value 0x000F @Desc 1 */
	U16 PLC_PE16_ADJ_DEFAULT;                          /**< @Value 0x0000 @Desc 1 */
	U16 PLC_MUTE_GAIN_RATE_FACTOR_ATTACK_DEFAULT;      /**< @Value 0x000A @Desc 1 */
	U16 PLC_MUTE_GAIN_RATE_FACTOR_RELEASE_DEFAULT;     /**< @Value 0x000A @Desc 1 */
	U16 PLC_WEIGHT_PKTLOSS_EN_DEFAULT;                 /**< @Value 0x0001 @Desc 1 */
	U16 PLC_WEIGHT_PKTLOSS_DEFAULT;                    /**< @Value 0x4000 @Desc 1 */
	U16 PLC_MIN_FRAME_EN;                              /**< @Value 0x0001 @Desc 1 */
	U16 PLC_MIN_FRAME_BER_METHOD;                      /**< @Value 0x0000 @Desc 1 */
	U16 PLC_SMART_EN;                                  /**< @Value 0x0001 @Desc 1 */
	U16 PLC_BER_GFRAME_EN;                             /**< @Value 0x0001 @Desc 1 */
	U16 PLC_CRC_GFRAME_EN;                             /**< @Value 0x0001 @Desc 1 */
	U16 PLC_BER_GFRAME_TH;                             /**< @Value 0x0001 @Desc 1 */
	U16 PLC_HEC_GFRAME_EN;                             /**< @Value 0x0001 @Desc 1 */
	U16 PLC_TONE_SBER_EN;                              /**< @Value 0x0001 @Desc 1 */
} PACKED DSP_PARA_PLC_STRU;


#endif /* _DSP_PARA_PLC_H_ */
