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
 
#ifndef _CPD_PARA_H_
#define _CPD_PARA_H_

#if 0
#ifdef WIN32
#define ALIGN(x) __declspec(align(x))
#else
#define ALIGN(x) __attribute__((aligned (x)))
#endif
#else
#include "dsp_utilities.h"
#endif
#include "dsp_gain_control.h"

#define MAX_AU_CPD_DELAY_SIZE 96*2*2 // *2(stereo)*2(96K sample rate)
#define MAX_BUF_SIZE    8
#define PACKET_BUF_SIZE 8
#define DSP_CPD_SCRATCH_MEMSIZE 1

#ifndef MTK_BT_HFP_SPE_ALG_V2

typedef struct
{
    U8  ENABLE;                /**< @Value 0x01 @Desc 1 */
    U8  REVISION;            /**< @Value 0x02 @Desc 1 */
    U16 CPD_AU_MODE;        /**< @Value 0x0000 @Desc B1: 0(Disable Audio INS, default), 1(Enable Audio INS) B0: 0(fullband, default), 1(multiband) */
    U16 CPD_AU_DELAY_SIZE;    /**< @Value 0x0060 @Desc 1 */
    S32 CPD_PACKET_MAX;                    /**< @Value 0x08000000 @Desc 1 */

    // AU_H(Multi-band compander or single-band compnader)
    S16 AUH_alpha_release1_44K;            /**< @Value 0x2000 @Desc 1 */
    S16 AUH_alpha_release2_44K;            /**< @Value 0x0666 @Desc 1 */
    S16 AUH_alpha_release3_44K;            /**< @Value 0x0CCD @Desc 1 */
    S16 AUH_alpha_attack_44K;            /**< @Value 0x2000 @Desc 1 */
    S16 AUH_alpha_release1_48K;            /**< @Value 0x1DFC @Desc 1 */
    S16 AUH_alpha_release2_48K;            /**< @Value 0x05D5 @Desc 1 */
    S16 AUH_alpha_release3_48K;            /**< @Value 0x0B95 @Desc 1 */
    S16 AUH_alpha_attack_48K;            /**< @Value 0x1DFC @Desc 1 */
    S16 AUH_alpha_release1_96K;            /**< @Value 0x1DFC @Desc 1 */
    S16 AUH_alpha_release2_96K;            /**< @Value 0x05D5 @Desc 1 */
    S16 AUH_alpha_release3_96K;            /**< @Value 0x0B95 @Desc 1 */
    S16 AUH_alpha_attack_96K;            /**< @Value 0x1DFC @Desc 1 */
    S32 AUH_CPD_THRD;                    /**< @Value 0x047F0000 @Desc 1 */
    S32 AUH_level_two;                    /**< @Value 0x00A40000 @Desc 1 */
    S32 AUH_level_three;                /**< @Value 0x00520000 @Desc 1 */
    S32 AUH_out_max;                    /**< @Value 0x06E60000 @Desc 1 */
    S16 AUH_period;                        /**< @Value 0x000A @Desc 1 */
    // AU_L(Multi-band compander)
    S16 AUL_alpha_release1_44K;            /**< @Value 0x028F @Desc 1 */
    S16 AUL_alpha_release2_44K;            /**< @Value 0x0666 @Desc 1 */
    S16 AUL_alpha_release3_44K;            /**< @Value 0x0CCD @Desc 1 */
    S16 AUL_alpha_attack_44K;            /**< @Value 0x399A @Desc 1 */
    S16 AUL_alpha_release1_48K;            /**< @Value 0x024D @Desc 1 */
    S16 AUL_alpha_release2_48K;            /**< @Value 0x0594 @Desc 1 */
    S16 AUL_alpha_release3_48K;            /**< @Value 0x0A9C @Desc 1 */
    S16 AUL_alpha_attack_48K;            /**< @Value 0x36C6 @Desc 1 */
    S32 AUL_CPD_THRD;                    /**< @Value 0x047F0000 @Desc 1 */
    S32 AUL_level_two;                    /**< @Value 0x00A40000 @Desc 1 */
    S32 AUL_level_three;                /**< @Value 0x00520000 @Desc 1 */
    S32 AUL_out_max;                    /**< @Value 0x04800000 @Desc 1 */
    S16 AUL_period;                        /**< @Value 0x000A @Desc 1 */
    // AU_F(Multi-band compander)
    S16 AUF_alpha_release1_44K;            /**< @Value 0x2000 @Desc 1 */
    S16 AUF_alpha_attack_44K;            /**< @Value 0x399A @Desc 1 */
    S16 AUF_alpha_release1_48K;            /**< @Value 0x1D28 @Desc 1 */
    S16 AUF_alpha_attack_48K;            /**< @Value 0x36F0 @Desc 1 */
    S32 AUF_CPD_THRD;                    /**< @Value 0x40000000 @Desc 1 */
    S32 AUF_out_max;                    /**< @Value 0x40000000 @Desc 1 */

    S32 AUX_INS_thrd_H;                    /**< @Value 0x000053E3 @Desc 1 */
    S32 AUX_INS_atten_H;                /**< @Value 0x00000CCD @Desc 1 */
    S16 AUX_INS_alpha_H;                /**< @Value 0x0200     @Desc 1 */

    S32 thrd;                            /**< @Value 0x10000000 @Desc 1 */
    S32 max_gain;                        /**< @Value 0x20000000 @Desc 1 */
    S32 band_sw;                        /**< @Value 0x00000000 @Desc 1 */
    S32 silence_th;                        /**< @Value 0x00050000 @Desc 1 */

    ALIGN(8) S32 HP_COFEB24_3rd_48K_00;    /**< @Value -8308203 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_01;    /**< @Value  8388607 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_02;    /**< @Value  8231420 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_03;    /**< @Value -8308203 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_04;    /**< @Value  8308203 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_05;    /**< @Value -8231420 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_06;    /**< @Value -8151489 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_07;    /**< @Value  4194304 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_08;    /**< @Value  4194304 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_09;    /**< @Value  4075745 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_10;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_11;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_12;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_13;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_14;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_15;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_16;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_17;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_18;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_19;    /**< @Value 0x00000000 @Desc 1 */

    ALIGN(8) S32 HP_COFEB24_4th_48K_00;    /**< @Value -8219113 @Desc 1 */
             S32 HP_COFEB24_4th_48K_01;    /**< @Value  8388607 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_02;    /**< @Value  8054930 @Desc 1 */
             S32 HP_COFEB24_4th_48K_03;    /**< @Value -8219113 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_04;    /**< @Value  8219113 @Desc 1 */
             S32 HP_COFEB24_4th_48K_05;    /**< @Value -8054930 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_06;    /**< @Value -8364874 @Desc 1 */
             S32 HP_COFEB24_4th_48K_07;    /**< @Value  8388607 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_08;    /**< @Value  8344025 @Desc 1 */
             S32 HP_COFEB24_4th_48K_09;    /**< @Value -8364874 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_10;    /**< @Value  8364874 @Desc 1 */
             S32 HP_COFEB24_4th_48K_11;    /**< @Value -8344025 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_12;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_48K_13;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_14;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_48K_15;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_16;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_48K_17;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_18;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_48K_19;    /**< @Value 0x00000000 @Desc 1 */

    ALIGN(8) S32 HP_COFEB24_3rd_44K_00;    /**< @Value -8300547 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_01;    /**< @Value  8388607 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_02;    /**< @Value  8216782 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_03;    /**< @Value -8300547 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_04;    /**< @Value  8300547 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_05;    /**< @Value -8216782 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_06;    /**< @Value -8130775 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_07;    /**< @Value  4194304 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_08;    /**< @Value  4194304 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_09;    /**< @Value  4065388 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_10;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_11;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_12;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_13;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_14;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_15;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_16;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_17;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_18;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_19;    /**< @Value 0x00000000 @Desc 1 */

    ALIGN(8) S32 HP_COFEB24_4th_44K_00;    /**< @Value -8204058 @Desc 1 */
             S32 HP_COFEB24_4th_44K_01;    /**< @Value  8388607 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_02;    /**< @Value  8025791 @Desc 1 */
             S32 HP_COFEB24_4th_44K_03;    /**< @Value -8204058 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_04;    /**< @Value  8204058 @Desc 1 */
             S32 HP_COFEB24_4th_44K_05;    /**< @Value -8025791 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_06;    /**< @Value -8362292 @Desc 1 */
             S32 HP_COFEB24_4th_44K_07;    /**< @Value  8388607 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_08;    /**< @Value  8339416 @Desc 1 */
             S32 HP_COFEB24_4th_44K_09;    /**< @Value -8362292 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_10;    /**< @Value  8362292 @Desc 1 */
             S32 HP_COFEB24_4th_44K_11;    /**< @Value -8339416 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_12;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_44K_13;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_14;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_44K_15;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_16;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_44K_17;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_18;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_44K_19; /**< @Value 0x00000000 @Desc 1 */

    S16 AUH_alpha_release1_88K;                /**< @Value 0x2000 @Desc 1 */
    S16 AUH_alpha_release2_88K;                /**< @Value 0x0666 @Desc 1 */
    S16 AUH_alpha_release3_88K;                /**< @Value 0x0CCD @Desc 1 */
    S16 AUH_alpha_attack_88K;                /**< @Value 0x2000 @Desc 1 */

    S32 AUX_INS_thrd_L;                        /**< @Value 0x000053E3 @Desc 1 */
    S32 AUX_INS_atten_L;                    /**< @Value 0x00000CCD @Desc 1 */
    S16 AUX_INS_alpha_L;                    /**< @Value 0x0200     @Desc 1 */

    S16 Offset_volume;                      /**< @Value 0x0000     @Desc 1 */

    U32 RESERVE_0;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_1;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_2;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_3;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_4;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_5;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_6;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_7;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_8;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_9;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_10;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_11;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_12;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_13;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_14;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_15;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_16;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_17;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_18;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_19;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_20;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_21;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_22;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_23;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_24;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_25;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_26;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_27;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_28;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_29;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_30;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_31;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_32;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_33;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_34;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_35;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_36;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_37;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_38;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_39;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_40;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_41;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_42;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_43;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_44;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_45;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_46;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_47;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_48;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_49;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_50;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_51;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_52;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_53;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_54;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_55;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_56;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_57;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_58;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_59;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_60;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_61;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_62;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_63;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_64;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_65;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_66;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_67;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_68;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_69;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_70;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_71;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_72;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_73;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_74;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_75;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_76;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_77;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_78;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_79;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_80;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_81;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_82;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_83;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_84;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_85;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_86;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_87;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_88;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_89;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_90;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_91;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_92;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_93;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_94;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_95;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_96;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_97;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_98;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_99;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_100;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_101;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_102;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_103;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_104;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_105;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_106;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_107;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_108;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_109;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_110;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_111;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_112;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_113;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_114;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_115;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_116;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_117;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_118;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_119;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_120;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_121;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_122;                        /**< @Value 0x00000000 @Desc 1 */
} PACKED CPD_AU_NVKEY_STATE;


typedef struct
{
    U8 ENABLE;                                /**< @Value 0x01 @Desc 1 */
    U8 REVISION;                            /**< @Value 0x01 @Desc 1 */
    U16 CPD_VO_DELAY_SIZE;                    /**< @Value 0x0028 @Desc 1 */
    S16 alpha_release;                        /**< @Value 0x7F1B @Desc 1 */
    S16 alpha_attack;                        /**< @Value 0x3B6A @Desc 1 */
    S32 CPD_THRD1;                            /**< @Value 87241523 @Desc 1 */
    S32 CPD_THRD2;                            /**< @Value 87241523 @Desc 1 */
    S32 CPD_IN2;                            /**< @Value 87241523 @Desc 1 */
    S32 out_max;                            /**< @Value 134217728 @Desc 1 */
    S32 CPD_PACKET_MAX;                        /**< @Value 134217728 @Desc 1 */
    S16 period;                                /**< @Value 0x0005 @Desc 1 */
    S16 INS_thrd;                            /**< @Value 0x000D @Desc 1 */
    S16 INS_atten;                            /**< @Value 0x2879 @Desc 1 */
    S16 INS_alpha_rx;                        /**< @Value 0x0CCD @Desc 1 */
    S16 INS_alpha_tx_vad;                    /**< @Value 0x70A4 @Desc 1 */
    S16 INS_alpha_tx_novad;                    /**< @Value 0x7C29 @Desc 1 */

    S16 Offset_volume;                        /**< @Value 0x0000     @Desc 1 */

    U16 RESERVE_0;                            /**< @Value 0x0000     @Desc 1 */
    U32 RESERVE_1;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_2;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_3;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_4;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_5;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_6;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_7;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_8;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_9;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_10;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_11;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_12;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_13;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_14;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_15;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_16;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_17;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_18;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_19;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_20;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_21;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_22;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_23;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_24;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_25;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_26;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_27;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_28;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_29;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_30;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_31;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_32;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_33;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_34;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_35;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_36;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_37;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_38;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_39;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_40;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_41;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_42;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_43;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_44;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_45;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_46;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_47;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_48;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_49;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_50;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_51;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_52;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_53;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_54;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_55;                            /**< @Value 0x00000000 @Desc 1 */
} PACKED CPD_VO_NVKEY_STATE;

//AU:
typedef struct AU_CPD_INSTANCE_s
{
    U32 MemoryCheck;
    S32 positive_gain;
    DSP_ALIGN8 CPD_AU_NVKEY_STATE NvKey;
    DSP_ALIGN8 U8 ScratchMemory[DSP_CPD_SCRATCH_MEMSIZE]; //Set at the end of structure for dynamic size
} AU_CPD_INSTANCE_t, *AU_CPD_INSTANCE_ptr;


//VO:
typedef struct VO_CPD_INSTANCE_s
{
    U32 MemoryCheck;
    U32 positive_gain; //only used for RX CPD
    DSP_ALIGN8 CPD_VO_NVKEY_STATE NvKey;
    DSP_ALIGN8 U8 ScratchMemory[DSP_CPD_SCRATCH_MEMSIZE]; //Set at the end of structure for dynamic size
} VO_CPD_INSTANCE_t, *VO_CPD_INSTANCE_ptr;

typedef struct {
    S16 TX_GAIN;
    S16 CPD_VAD;
    S16 avc_vol;
}ECNR_OUT;

#ifndef MTK_BT_A2DP_CPD_USE_PIC
int get_CPD_memsize (int mode);
void compander_AU_init (void *p_AU_STATE, void *p_AU_NVKEY, S32 recovery_gain, S32 band_sw);
void compander_VO_init (void *p_VO_STATE, void *p_VO_NVKEY, S32 mod);
void compander_AU_proc(void * p_AU_STATE, S32 *buf_L, S32 *buf_R, S32 frame_size, S16 Vol, S16 channel);
void compander_VO_proc(void *p_VO_STATE, S16 *buf, S32 frame_size, S16 Rx_Vol, ECNR_OUT *PAR);
int get_INS_memsize (void);
void INS_Init(void *p_INS_STATE, void *p_INS_NVKEY);
void INS_Prcs(void *p_INS_STATE, S32 *buf_L, S32 *buf_R, S32 frame_size, S16 channel);
#endif

#else /*MTK_BT_HFP_SPE_ALG_V2*/

typedef struct
{
    U8  ENABLE;                            /**< @Value 0x01 @Desc 1 */
    U8  REVISION;                        /**< @Value 0x02 @Desc 1 */
    U16 CPD_AU_MODE;                    /**< @Value 0x0000 @Desc B1: 0(Disable Audio INS, default), 1(Enable Audio INS) B0: 0(fullband, default), 1(multiband) */
    U16 CPD_AU_DELAY_SIZE;                /**< @Value 0x0060 @Desc 1 */
    S32 CPD_PACKET_MAX;                    /**< @Value 0x08000000 @Desc 1 */
                                
    // AU_H(Multi-band compander or single-band compnader)
    S16 AUH_alpha_release1_44K;            /**< @Value 0x2000 @Desc 1 */
    S16 AUH_alpha_release2_44K;            /**< @Value 0x0666 @Desc 1 */
    S16 AUH_alpha_release3_44K;            /**< @Value 0x0CCD @Desc 1 */
    S16 AUH_alpha_attack_44K;            /**< @Value 0x2000 @Desc 1 */
    S16 AUH_alpha_release1_48K;            /**< @Value 0x1DFC @Desc 1 */
    S16 AUH_alpha_release2_48K;            /**< @Value 0x05D5 @Desc 1 */
    S16 AUH_alpha_release3_48K;            /**< @Value 0x0B95 @Desc 1 */
    S16 AUH_alpha_attack_48K;            /**< @Value 0x1DFC @Desc 1 */
    S16 AUH_alpha_release1_96K;            /**< @Value 0x1DFC @Desc 1 */
    S16 AUH_alpha_release2_96K;            /**< @Value 0x05D5 @Desc 1 */
    S16 AUH_alpha_release3_96K;            /**< @Value 0x0B95 @Desc 1 */
    S16 AUH_alpha_attack_96K;            /**< @Value 0x1DFC @Desc 1 */
    S32 AUH_CPD_THRD;                    /**< @Value 0x047F0000 @Desc 1 */
    S32 AUH_level_two;                    /**< @Value 0x00A40000 @Desc 1 */
    S32 AUH_level_three;                /**< @Value 0x00520000 @Desc 1 */
    S32 AUH_out_max;                    /**< @Value 0x06E60000 @Desc 1 */
    S16 AUH_period;                        /**< @Value 0x000A @Desc 1 */
    // AU_L(Multi-band compander)                                
    S16 AUL_alpha_release1_44K;            /**< @Value 0x028F @Desc 1 */
    S16 AUL_alpha_release2_44K;            /**< @Value 0x0666 @Desc 1 */
    S16 AUL_alpha_release3_44K;            /**< @Value 0x0CCD @Desc 1 */
    S16 AUL_alpha_attack_44K;            /**< @Value 0x399A @Desc 1 */
    S16 AUL_alpha_release1_48K;            /**< @Value 0x024D @Desc 1 */
    S16 AUL_alpha_release2_48K;            /**< @Value 0x0594 @Desc 1 */
    S16 AUL_alpha_release3_48K;            /**< @Value 0x0A9C @Desc 1 */
    S16 AUL_alpha_attack_48K;            /**< @Value 0x36C6 @Desc 1 */
    S32 AUL_CPD_THRD;                    /**< @Value 0x047F0000 @Desc 1 */
    S32 AUL_level_two;                    /**< @Value 0x00A40000 @Desc 1 */
    S32 AUL_level_three;                /**< @Value 0x00520000 @Desc 1 */
    S32 AUL_out_max;                    /**< @Value 0x04800000 @Desc 1 */
    S16 AUL_period;                        /**< @Value 0x000A @Desc 1 */
    // AU_F(Multi-band compander)                            
    S16 AUF_alpha_release1_44K;            /**< @Value 0x2000 @Desc 1 */
    S16 AUF_alpha_attack_44K;            /**< @Value 0x399A @Desc 1 */
    S16 AUF_alpha_release1_48K;            /**< @Value 0x1D28 @Desc 1 */
    S16 AUF_alpha_attack_48K;            /**< @Value 0x36F0 @Desc 1 */
    S32 AUF_CPD_THRD;                    /**< @Value 0x40000000 @Desc 1 */
    S32 AUF_out_max;                    /**< @Value 0x40000000 @Desc 1 */    
    
    S32 AUX_INS_thrd_H;                    /**< @Value 0x000053E3 @Desc 1 */
    S32 AUX_INS_atten_H;                /**< @Value 0x00000CCD @Desc 1 */
    S16 AUX_INS_alpha_H;                /**< @Value 0x0200     @Desc 1 */
                                
    S32 thrd;                            /**< @Value 0x10000000 @Desc 1 */
    S32 max_gain;                        /**< @Value 0x20000000 @Desc 1 */
    S32 band_sw;                        /**< @Value 0x00000000 @Desc 1 */
    S32 silence_th;                        /**< @Value 0x00050000 @Desc 1 */
    
    ALIGN(8) S32 HP_COFEB24_3rd_48K_00;    /**< @Value -8308203 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_01;    /**< @Value  8388607 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_02;    /**< @Value  8231420 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_03;    /**< @Value -8308203 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_04;    /**< @Value  8308203 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_05;    /**< @Value -8231420 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_06;    /**< @Value -8151489 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_07;    /**< @Value  4194304 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_08;    /**< @Value  4194304 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_09;    /**< @Value  4075745 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_10;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_11;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_12;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_13;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_14;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_15;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_16;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_17;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_48K_18;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_48K_19;    /**< @Value 0x00000000 @Desc 1 */
                                
    ALIGN(8) S32 HP_COFEB24_4th_48K_00;    /**< @Value -8219113 @Desc 1 */
             S32 HP_COFEB24_4th_48K_01;    /**< @Value  8388607 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_02;    /**< @Value  8054930 @Desc 1 */
             S32 HP_COFEB24_4th_48K_03;    /**< @Value -8219113 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_04;    /**< @Value  8219113 @Desc 1 */
             S32 HP_COFEB24_4th_48K_05;    /**< @Value -8054930 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_06;    /**< @Value -8364874 @Desc 1 */
             S32 HP_COFEB24_4th_48K_07;    /**< @Value  8388607 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_08;    /**< @Value  8344025 @Desc 1 */
             S32 HP_COFEB24_4th_48K_09;    /**< @Value -8364874 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_10;    /**< @Value  8364874 @Desc 1 */
             S32 HP_COFEB24_4th_48K_11;    /**< @Value -8344025 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_12;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_48K_13;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_14;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_48K_15;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_16;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_48K_17;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_48K_18;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_48K_19;    /**< @Value 0x00000000 @Desc 1 */

    ALIGN(8) S32 HP_COFEB24_3rd_44K_00;    /**< @Value -8300547 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_01;    /**< @Value  8388607 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_02;    /**< @Value  8216782 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_03;    /**< @Value -8300547 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_04;    /**< @Value  8300547 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_05;    /**< @Value -8216782 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_06;    /**< @Value -8130775 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_07;    /**< @Value  4194304 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_08;    /**< @Value  4194304 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_09;    /**< @Value  4065388 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_10;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_11;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_12;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_13;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_14;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_15;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_16;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_17;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_3rd_44K_18;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_3rd_44K_19;    /**< @Value 0x00000000 @Desc 1 */

    ALIGN(8) S32 HP_COFEB24_4th_44K_00;    /**< @Value -8204058 @Desc 1 */
             S32 HP_COFEB24_4th_44K_01;    /**< @Value  8388607 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_02;    /**< @Value  8025791 @Desc 1 */
             S32 HP_COFEB24_4th_44K_03;    /**< @Value -8204058 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_04;    /**< @Value  8204058 @Desc 1 */
             S32 HP_COFEB24_4th_44K_05;    /**< @Value -8025791 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_06;    /**< @Value -8362292 @Desc 1 */
             S32 HP_COFEB24_4th_44K_07;    /**< @Value  8388607 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_08;    /**< @Value  8339416 @Desc 1 */
             S32 HP_COFEB24_4th_44K_09;    /**< @Value -8362292 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_10;    /**< @Value  8362292 @Desc 1 */
             S32 HP_COFEB24_4th_44K_11;    /**< @Value -8339416 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_12;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_44K_13;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_14;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_44K_15;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_16;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_44K_17;    /**< @Value 0x00000000 @Desc 1 */
    ALIGN(8) S32 HP_COFEB24_4th_44K_18;    /**< @Value 0x00000000 @Desc 1 */
             S32 HP_COFEB24_4th_44K_19; /**< @Value 0x00000000 @Desc 1 */
    
    S16 AUH_alpha_release1_88K;                /**< @Value 0x2000 @Desc 1 */
    S16 AUH_alpha_release2_88K;                /**< @Value 0x0666 @Desc 1 */
    S16 AUH_alpha_release3_88K;                /**< @Value 0x0CCD @Desc 1 */
    S16 AUH_alpha_attack_88K;                /**< @Value 0x2000 @Desc 1 */
    
    S32 AUX_INS_thrd_L;                        /**< @Value 0x000053E3 @Desc 1 */
    S32 AUX_INS_atten_L;                    /**< @Value 0x00000CCD @Desc 1 */
    S16 AUX_INS_alpha_L;                    /**< @Value 0x0200     @Desc 1 */
    
    S16 Offset_volume;                      /**< @Value 0x0000     @Desc 1 */
    
    U32 RESERVE_0;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_1;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_2;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_3;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_4;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_5;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_6;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_7;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_8;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_9;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_10;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_11;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_12;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_13;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_14;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_15;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_16;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_17;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_18;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_19;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_20;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_21;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_22;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_23;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_24;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_25;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_26;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_27;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_28;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_29;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_30;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_31;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_32;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_33;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_34;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_35;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_36;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_37;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_38;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_39;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_40;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_41;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_42;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_43;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_44;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_45;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_46;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_47;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_48;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_49;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_50;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_51;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_52;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_53;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_54;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_55;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_56;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_57;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_58;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_59;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_60;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_61;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_62;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_63;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_64;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_65;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_66;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_67;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_68;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_69;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_70;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_71;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_72;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_73;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_74;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_75;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_76;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_77;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_78;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_79;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_80;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_81;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_82;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_83;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_84;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_85;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_86;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_87;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_88;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_89;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_90;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_91;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_92;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_93;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_94;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_95;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_96;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_97;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_98;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_99;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_100;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_101;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_102;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_103;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_104;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_105;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_106;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_107;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_108;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_109;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_110;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_111;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_112;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_113;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_114;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_115;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_116;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_117;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_118;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_119;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_120;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_121;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_122;                        /**< @Value 0x00000000 @Desc 1 */
} PACKED CPD_AU_NVKEY_STATE;

typedef struct
{
    U8 ENABLE;                                /**< @Value 0x01 @Desc 1 */
    U8 REVISION;                            /**< @Value 0x01 @Desc 1 */
    U16 CPD_VO_DELAY_SIZE;                    /**< @Value 0x0028 @Desc 1 */
    S16 alpha_release;                        /**< @Value 0x7F1B @Desc 1 */
    S16 alpha_attack;                        /**< @Value 0x3B6A @Desc 1 */
    S32 CPD_THRD1;                            /**< @Value 87241523 @Desc 1 */
    S32 CPD_THRD2;                            /**< @Value 87241523 @Desc 1 */
    S32 CPD_IN2;                            /**< @Value 87241523 @Desc 1 */
    S32 out_max;                            /**< @Value 134217728 @Desc 1 */
    S32 CPD_PACKET_MAX;                        /**< @Value 134217728 @Desc 1 */    
    S16 period;                                /**< @Value 0x0005 @Desc 1 */
    S16 INS_thrd;                            /**< @Value 0x000D @Desc 1 */
    S16 INS_atten;                            /**< @Value 0x2879 @Desc 1 */
    S16 INS_alpha_rx;                        /**< @Value 0x0CCD @Desc 1 */
    S16 INS_alpha_tx_vad;                    /**< @Value 0x70A4 @Desc 1 */
    S16 INS_alpha_tx_novad;                    /**< @Value 0x7C29 @Desc 1 */
    
    S16 Offset_volume;                        /**< @Value 0x0000     @Desc 1 */
    
    U16 RESERVE_0;                            /**< @Value 0x0000     @Desc 1 */
    U32 RESERVE_1;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_2;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_3;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_4;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_5;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_6;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_7;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_8;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_9;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_10;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_11;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_12;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_13;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_14;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_15;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_16;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_17;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_18;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_19;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_20;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_21;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_22;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_23;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_24;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_25;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_26;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_27;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_28;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_29;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_30;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_31;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_32;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_33;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_34;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_35;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_36;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_37;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_38;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_39;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_40;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_41;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_42;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_43;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_44;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_45;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_46;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_47;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_48;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_49;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_50;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_51;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_52;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_53;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_54;                            /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_55;                            /**< @Value 0x00000000 @Desc 1 */
} PACKED CPD_VO_NVKEY_STATE;

//AU:
typedef struct AU_CPD_INSTANCE_s
{
    U32 MemoryCheck;
    S32 positive_gain;
    DSP_ALIGN8 CPD_AU_NVKEY_STATE NvKey;
    DSP_ALIGN8 U8 ScratchMemory[DSP_CPD_SCRATCH_MEMSIZE]; //Set at the end of structure for dynamic size
} AU_CPD_INSTANCE_t, *AU_CPD_INSTANCE_ptr;


//VO:
typedef struct VO_CPD_INSTANCE_s
{
    U32 MemoryCheck;
    U32 positive_gain; //only used for RX CPD
    DSP_ALIGN8 CPD_VO_NVKEY_STATE NvKey;
    DSP_ALIGN8 U8 ScratchMemory[DSP_CPD_SCRATCH_MEMSIZE]; //Set at the end of structure for dynamic size
} VO_CPD_INSTANCE_t, *VO_CPD_INSTANCE_ptr;

typedef struct {
    S16 TX_GAIN;
    S16 CPD_VAD;
    S16 avc_vol;
}ECNR_OUT;

#ifndef MTK_BT_A2DP_CPD_USE_PIC
void compander_AU_init(AU_CPD_INSTANCE_t *cpd_all, S32 recovery_gain);
void compander_AU_proc(AU_CPD_INSTANCE_t *cpd_all, S32 *buf_L, S32 *buf_R, S32 frame_size, S16 Vol, S16 channel);
void compander_VO_init(VO_CPD_INSTANCE_t *cpd_all, S32 mod);
void compander_VO_proc(VO_CPD_INSTANCE_t *cpd_all, S16 *buf, U16 frame_size, S16 Rx_Vol, ECNR_OUT *PAR);
#endif

#endif /*MTK_BT_HFP_SPE_ALG_V2*/




#endif /* _DSP_PARA_H_ */

