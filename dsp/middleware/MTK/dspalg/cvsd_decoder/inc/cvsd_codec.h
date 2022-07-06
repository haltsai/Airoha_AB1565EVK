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

#ifndef CVSD_CODEC_H
#define CVSD_CODEC_H

#include <xtensa/tie/xt_core.h>
#include <xtensa/tie/xt_hifi2.h>
#include <xtensa/tie/xt_misc.h>
#include <xtensa/tie/xt_mul.h>
#include "types.h"
#include "cvsd_updn_sample.h"
// Encoder: 128 bytes -> 64 samples -> up-sample(8) -> 512 samples -> CVSD encoder -> 512 / 8 = 64 bytes
// Decoder: 64 bytes  -> CVSD decoder -> 64*8 samples -> down-sample(8) -> 64 sample -> 128 bytes
#define    cvsd_frame_samp_len    120 // 120*2 = 240 bytes, WB = 120 samples, NB = 60 samples

#define c_up_8k_ord            14
#define c_up_16k_32k_ord     3
#define c_dn_64k_ord        10 // 6
#define c_dn_32k_ord        10
#define c_dn_16k_ord        10

typedef struct {
    S32 acc;
    S32 delta;
    S32 step;
} cvsd_codec_information_t;

typedef struct {
ALIGN(8) S16 stack_8khz[c_up_8k_ord];
ALIGN(8) S16 stack_16khz[c_up_16k_32k_ord];
ALIGN(8) S16 stack_32khz[c_up_16k_32k_ord];
ae_p16x2s *up8k_pt, *up16k_pt, *up32k_pt;
} cvsd_src_up_sampling_information_t;

typedef struct {
ALIGN(8) S16 stack_64khz[c_dn_64k_ord];
ALIGN(8) S16 stack_32khz[c_dn_32k_ord];
ALIGN(8) S16 stack_16khz[c_dn_16k_ord];
ae_p16x2s *dn64k_pt, *dn32k_pt, *dn16k_pt;
} cvsd_src_dn_sampling_information_t;

typedef struct
{
    void                        *cvsd_enc_void, *cvsd_upsamp_void;
    U8                            cvsd_enc_buf[sizeof(cvsd_codec_information_t)];
    U8                            cvsd_upsamp_buf[sizeof(cvsd_src_up_sampling_information_t)];
    S16                            upsamp_tp_buf[cvsd_frame_samp_len*2], upsamp_ou_buf[cvsd_frame_samp_len*8];
    S16                            frm_len;
    cvsd_codec_information_t    cvsd_enc_info;
    ALIGN(8) dsp_updn_sample_interface_t cvsd_updn_sample;
    //cvsd_src_up_sampling_information_t  cvsd_src_up_sampling;
} CVSD_ENC_STATE, *CVSD_ENC_STATE_PTR;

typedef struct
{
    void                        *cvsd_dec_void, *cvsd_dnsamp_void;
    U8                            cvsd_dec_buf[sizeof(cvsd_codec_information_t)];
    U8                            cvsd_dnsamp_buf[sizeof(cvsd_src_dn_sampling_information_t)];
    S16                            upsamp_ou_buf[cvsd_frame_samp_len*8];
    S16                            frm_len;
    cvsd_codec_information_t    cvsd_dec_info;
    ALIGN(8) dsp_updn_sample_interface_t cvsd_updn_sample;
    //cvsd_src_up_sampling_information_t  cvsd_src_up_sampling;
} CVSD_DEC_STATE, *CVSD_DEC_STATE_PTR;


#define UPPER_BOUND(in,up) ((in) > (up) ? (up) : (in))
#define LOWER_BOUND(in,lo) ((in) < (lo) ? (lo) : (in))
#define BOUNDED(in,up,lo) ((in) > (up) ? (up) : (in) < (lo) ? (lo) : (in))

#define DELTA_MAX 0x140000    // (1280 << 10)
#define DELTA_MIN 0x2800    // (10 << 10)
#define MAX_Y     (0x01fffff)
#define MIN_Y     (-MAX_Y)

#define DOWN_LV1_SHIFT_BIT   6
#define DOWN_LV2_SHIFT_BIT   6
#define DOWN_LV3_SHIFT_BIT  11
#define UP_LV1_SHIFT_BIT    10
#define UP_LV2_SHIFT_BIT     5

#if 0
#include "dsp_rom_table.h"
#else
extern S16 cvsd_src_down_sampling_coef_1[c_dn_64k_ord];
extern S16 cvsd_src_down_sampling_coef_2[c_dn_32k_ord];
extern S16 cvsd_src_down_sampling_coef_3[c_dn_16k_ord];
extern S16 cvsd_src_up_sampling_coef_1[c_up_8k_ord];
extern S16 cvsd_src_up_sampling_coef_2[c_up_16k_32k_ord+1];
extern U8  cvsd_BitReverseTable16[16];
#endif

#ifndef MTK_BT_A2DP_CVSD_USE_PIC
extern void _CVSD_Encoder_Init(CVSD_ENC_STATE *para, U16 sel_wnb);
extern void _CVSD_Decoder_Init(CVSD_DEC_STATE *para, U16 sel_wnb);
extern S32 _CVSD_Encoder(CVSD_ENC_STATE *para, U16 *p_in_buf, U16 *p_ou_buf);
extern S32 _CVSD_Decoder(CVSD_DEC_STATE *para, U16 *p_in_buf, S16 *p_ou_buf);
#endif

S32 cvsd_codec_init (cvsd_codec_information_t *handle);
S32 cvsd_encode_init (void **handle_pointer, U8 *internal_buffer);
S32 cvsd_decode_init (void **handle_pointer, U8 *internal_buffer);
S32 cvsd_encode_process (void *raw_hdl, S16 *p_in_buf, U32 *p_in_byte_cnt, U16 *p_ou_buf, U32 *p_ou_byte_cnt);
S32 cvsd_decode_process (void *raw_hdl, U16 *p_in_buf, U32 *p_in_byte_cnt, S16 *p_ou_buf, U32 *p_ou_byte_cnt);

S32 cvsd_src_up_sampling_init (void **handle_pointer, U8 *internal_buffer);
S32 cvsd_src_dn_sampling_init (void **handle_pointer, U8 *internal_buffer);

void cvsd_src_up_sampling_mono_process_1 (S16 *p_in_buf, S16 *p_ou_buf, void *handle, U32 in_smpl_cnt);
void cvsd_src_up_sampling_mono_process_2 (S16 *p_in_buf, S16 *p_ou_buf, void *handle, U32 in_smpl_cnt);
void cvsd_src_up_sampling_mono_process_3 (S16 *p_in_buf, S16 *p_ou_buf, void *handle, U32 in_smpl_cnt);

void cvsd_src_dn_sampling_mono_process_1 (S16 *p_in_buf, S16 *p_ou_buf, void *handle, U32 in_smpl_cnt);
void cvsd_src_dn_sampling_mono_process_2 (S16 *p_in_buf, S16 *p_ou_buf, void *handle, U32 in_smpl_cnt);

S32 cvsd_src_up_sampling_process (void *handle, S16 *p_in_buf, S16 *p_tp_buf, S16 *p_ou_buf, U32 in_byte_cnt);
S32 cvsd_src_dn_sampling_process (void *handle, S16 *p_in_buf, S16 *p_ou_buf, U32 in_byte_cnt);

#endif /* CVSD_CODEC_H */

