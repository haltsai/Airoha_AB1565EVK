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
#ifndef _SMP_PARA_H_
#define _SMP_PARA_H_

#include "types.h"


#define MAX_DELAY_SIZE 96*2
#define MAX_BUF_SIZE 8
#define PACKET_BUF_SIZE 8
#define DSP_ALIGN8 ALIGN(8)

typedef struct
{
    S16 LP_COFEB16_00;  /**< @Value 0x1FCF @Desc 1 */
    S16 LP_COEFB16_01;  /**< @Value 0x2752 @Desc 1 */
    S16 LP_COFEB16_02;  /**< @Value 0x4306 @Desc 1 */
    S16 LP_COEFB16_03;  /**< @Value 0xCD63 @Desc 1 */
    S16 LP_COFEB16_04;  /**< @Value 0x2752 @Desc 1 */
    S16 LP_COEFB16_05;  /**< @Value 0x2752 @Desc 1 */
    S16 LP_COFEB16_06;  /**< @Value 0x08F8 @Desc 1 */
    S16 LP_COEFB16_07;  /**< @Value 0x2752 @Desc 1 */
    S16 LP_COFEB16_08;  /**< @Value 0x2839 @Desc 1 */
    S16 LP_COEFB16_09;  /**< @Value 0xA49F @Desc 1 */
    S16 LP_COFEB16_10;  /**< @Value 0x00CE @Desc 1 */
    S16 LP_COEFB16_11;  /**< @Value 0x2752 @Desc 1 */
    S16 LP_COFEB16_12;  /**< @Value 0x19C3 @Desc 1 */
    S16 LP_COEFB16_13;  /**< @Value 0x893E @Desc 1 */
    S16 LP_COFEB16_14;  /**< @Value 0x2752 @Desc 1 */
    S16 LP_Reserved;
} PACKED SMP_VO_NVKEY, dsp_updn_coef_t;


typedef struct {
    S32 Xstate32[2];    //For IIR filte  X[n-1]¡BX[n-2] state temporary
    S32 Ystate32[6];    //For IIR filter Y1[n-1]¡BY1[n-2]¡BY2[n-1]¡BY2[n-2]¡BY3[n-1]¡BY3[n-2] state temporary
} SMP_VO_STATE, dsp_updn_sate_t;


typedef struct dsp_updn_sample_interface_s
{
    dsp_updn_coef_t     NvKey;
    dsp_updn_sate_t     State;
    S16                 buf[240];
} VO_SMP_INSTANCE_t, dsp_updn_sample_interface_t, *dsp_updn_sample_interface_p;


extern int16_t dsp_updown_sample_by_2_coef[16] ;

void updn_sampling_by2_Init(dsp_updn_sample_interface_p smp_all);
S16  updn_sampling_by2_Proc(dsp_updn_sample_interface_p smp_all, S16 *InBuf, S16 *OuBuf, S16 len, S16 mode, S16 enable);

#endif /* _SMP_PARA_H_ */

