/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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

#include "types.h"  

#ifndef __DSP_NVKEY_VAD_PARA_H__
#define __DSP_NVKEY_VAD_PARA_H__

#define ENDIAN_RVRS(A)                  (A)

typedef struct {
S16 en_vad;                /**< @Value 0x0000 @Desc 1 */
S16 gamma;                 /**< @Value 0x0000 @Desc 1 */
S16 diff_fac;              /**< @Value 0x0000 @Desc 1 */
S16 diff_facb;             /**< @Value 0x0000 @Desc 1 */
S16 vad_fac;               /**< @Value 0x0000 @Desc 1 */
S16 vad_ph_cc;             /**< @Value 0x0000 @Desc 1 */
S16 vad_ph_init;           /**< @Value 0x0000 @Desc 1 */
S16 vad_ph_range;          /**< @Value 0x0000 @Desc 1 */
S16 vad_corr_th;           /**< @Value 0x0000 @Desc 1 */
S16 vad_pitch_th;          /**< @Value 0x0000 @Desc 1 */
S16 win_sm_1st;            /**< @Value 0x0000 @Desc 1 */
S16 sil_thr   ;            /**< @Value 0x0000 @Desc 1 */
S16 sil_thr2  ;            /**< @Value 0x0000 @Desc 1 */
S16 fr_thr    ;            /**< @Value 0x0000 @Desc 1 */
S16 fr_thr2   ;            /**< @Value 0x0000 @Desc 1 */
S16 rec_thr   ;            /**< @Value 0x0000 @Desc 1 */
S16 rec_thr2  ;            /**< @Value 0x0000 @Desc 1 */
S16 end_thr   ;            /**< @Value 0x0000 @Desc 1 */
S16 gar_thr   ;            /**< @Value 0x0000 @Desc 1 */
S16 rel_thr   ;            /**< @Value 0x0000 @Desc 1 */
S16 ss_thr    ;            /**< @Value 0x0000 @Desc 1 */
S16 ms_thr    ;            /**< @Value 0x0000 @Desc 1 */
S16 ss_rel_thr;            /**< @Value 0x0000 @Desc 1 */
S16 ss_g_thr  ;            /**< @Value 0x0000 @Desc 1 */
S16 fa_end_thr;            /**< @Value 0x0000 @Desc 1 */
S16 fa_beg_thr;            /**< @Value 0x0000 @Desc 1 */
S16 short_thr ;            /**< @Value 0x0000 @Desc 1 */
S16 rel_st4_thr;           /**< @Value 0x0000 @Desc 1 */
S16 rel_oth_thr;           /**< @Value 0x0000 @Desc 1 */
S16 dur_thr   ;            /**< @Value 0x0000 @Desc 1 */
S16 max_dur_thr;           /**< @Value 0x0000 @Desc 1 */
S16 ms_s_thr  ;            /**< @Value 0x0000 @Desc 1 */
S16 vad_leave_th;          /**< @Value 0x0000 @Desc 1 */
S16 vad_eng_thr;           /**< @Value 0x0000 @Desc 1 */
U16 RESERVE_A ;            /**< @Value 0x0000 @Desc 1 */
U16 RESERVE_B ;            /**< @Value 0x0000 @Desc 1 */
U16 RESERVE_C ;            /**< @Value 0x0000 @Desc 1 */
U16 RESERVE_D ;            /**< @Value 0x0000 @Desc 1 */
} DSP_NVKEY_VAD_PARA; 

#endif

