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
 
#include "dsp_utilities.h"
#include "opus_ll_types.h"

/*
 *
 * Immediates
 *
 */
extern const S16 immediate_num[];
extern const S16 NR_immediate_num[];

/*
 *
 * Math
 *
 */
extern const S16 TABLE_SQRT[49];


/*
 *
 * EC
 *
 */
extern const S32 wb_r01[6];
extern const S16 sb_g10[30];
extern const S16 sb_h2ir[30];
extern const S16 sb_h2ir_v2[30];
extern const S32 hpf_100_coef[];
extern const S32 hpf_110_coef[];


/*
 *
 * NR
 *
 */
extern const S16 wb_score_bands[];
extern const S16 wb_sb_ptn[];
extern const S16 nb_score_bands[];
extern const S16 nb_sb_ptn[];
extern const S16 win_512_v3[256+16];
extern const S16 win_rx_512_v3[256+16];
extern const S16 win_rx_256_v3[128+8];


/*
 *
 * SBC Decoder
 *
 */
extern const S16 G_LEVEL[17];
extern const S16 S_OFFSET[17];
extern const S32 S_ARRAY[17];
extern const S32 SBC_cos_coef_4[12*4];
extern const S32 SBC_cos_coef_8[24*8];
extern const S32 SBC_proto_coef_4[4*11*5];
extern const S32 SBC_proto_coef_8[8*11*5];
extern const U16 SBC_crc_table[128];
extern const S16 SBC_OFFSET4[4][4];
extern const S16 SBC_OFFSET8[4][8];


/*
 *
 * mSBC Decoder
 *
 */
extern const S16 MSBC_ZERO_RESPONSE[30];


/*
 *
 * mSBC Encoder
 *
 */
extern const S32 gas32CoeffFor8SBs[80];
extern const S32 sbc_enc_as16Offset8[4][8];
extern const S32 NEWgas16AnalDCTcoeff8[128];
extern const S16 MASK_bitp[16];


/*
 *
 * PLC
 *
 */
extern const S16 rc_win_right_16s[16];
extern const S16 rc_win_left_16s[16];
extern const S32 zc_diff_size_tab[3];


/*
 *
 * Audio BIST
 *
 */
extern const S32 SINE_100HZ_96K[960];


/*
 *
 * Audio Celt decoder
 *
 */
extern const S16    LOG2_FRAC_TABLE[];            
extern const S16    C_tab_celt[];                 
extern const S16    exp2_table8[];                
extern const S16    tf_select_table[4][8] ;        
extern const S16    e_prob_model[2][42] ;         
extern const S16    band_allocation[] ;           
extern const S16    eMeans[]  ;                   
extern const S16    pred_coef[] ;                 
extern const S16    beta_coef[] ;                 
extern const S16    ordery_table[] ;              
extern const S16    bit_interleave_table[];       
extern const S16    bit_deinterleave_table[];     
extern const U32    CELT_PVQ_U_DATA[];            
extern const S16    small_energy_icdf[];         
extern const S16    eband5ms[] ;                  
extern const S16    trim_icdf[11] ;               
extern const S16    spread_icdf[] ;               
                                                          
//------------------------ frame 256  --------------------
extern const S16    eBands48000_256[] ;           
extern const S16    window128[] ;                 
extern const S16    allocVectors48000_256[] ;     
extern const S16    logN375[];                    
extern const S16    cache_index187[] ;            
extern const S16    cache_bits187[] ;             
extern const S16    cache_caps187[] ;             
extern const S16    mdct_twiddles256[] ;          
extern const S16    fft_bitrev128[] ;             
extern const S16    fft_bitrev64[] ;              
extern const kiss_twiddle_cpx    fft_twiddles48000_256[];
//------------------------ frame 256 end-------------------
//------------------------ frame 240 ----------------------
extern const S16    window120[] ;                 
extern const S16    logN400[] ;                   
extern const S16    cache_index50[] ;             
extern const S16    cache_bits50[] ;              
extern const S16     cache_caps50[] ;             
extern const kiss_twiddle_cpx    fft_twiddles48000_960[];
extern const S16   fft_bitrev480[] ;              
extern const S16    fft_bitrev240[] ;             
extern const S16    fft_bitrev120[] ;             
extern const S16    fft_bitrev60[] ;              
extern const S16    mdct_twiddles960[];           
//------------------------ frame 240 end -----------------
//------------------------ CELT 16K mode -----------------
extern const S16    window_16K_320[] ;                 
extern const S16    logN_16K_320[] ;                   
extern const S16    cache_index_16K_320[] ;             
extern const S16    cache_bits_16K_320[] ;              
extern const S16     cach_caps_16K_320[] ;             
extern const kiss_twiddle_cpx    fft_twiddles_16K_320;
extern const S16   fft_bitrev0_16K_320[] ;              
extern const S16    fft_bitrev1_16K_320[] ;             
extern const S16    fft_bitrev2_16K_320[] ;             
extern const S16    fft_bitrev3_16K_320[] ;              
extern const S16    mdct_tw_16K_320[];           
//------------------------ CELT 16K mode end --------------

/*
 *
 * AMR-NB decoder
 *
 */
 //------------- common tables -------------------
extern const S16 dgray[8];
//extern const S16 sqrt_table[49];	//replaced with TABLE_SQRT[49]
extern const S16 pow2_table[33];
extern const S16 ph_imp_mid[] ;
extern const S16 ph_imp_low[] ;
extern const S16 lsptable[65] ;
extern const S16 slope[64]  ;
extern const S16 log2table[33] ;
extern const S16 lsp_init_data[10] ;
extern const S16 table_IS[49] ;
extern const S16 startPos9[2*4*2] ;
extern const S16 gamma3[10] ;
extern const S16 gamma4[10] ;
extern const S16 inter_6[61] ;
extern const S16 cdown[7] ;
extern const S16 pdown[7] ;
extern const S16 pred[4] ;
extern const S16 a_3[3] ;
extern const S16 b_3[3] ;
//-------------- end of common tables ------------
//-------------- 5.9K mode tables ----------------
/*
extern const S16 table_gain_lowrates[64*4];
extern const S16 dico3_lsf_3[512*4] ;
extern const S16 dico2_lsf_3[512*3] ;
extern const S16 dico1_lsf_3[256*3];
extern const S16 pred_fac3[10]  ;
extern const S16 mean_lsf3[10] ;
*/
//-------------- end of 5.9K mode tables ---------
//-------------- 12.2K mode tables ----------------
extern const S16 mean_lsf[10] ;
extern const S16 dico1_lsf[128 * 4] ;
extern const S16 dico2_lsf[256 * 4] ;
//extern const S16 dico3_lsf[256 * 4] ;
extern const S16 dico5_lsf[64 * 4];
//extern const S16 dico4_lsf[256 * 4]  ;
extern const S16 qua_gain_pitch[16];
extern const S16 qua_gain_code[32*3];
extern const S16 pred_MR122[4] ;
extern const S16 gamma3_MR122[10] ;
extern const S16 gamma4_MR122[10] ;
//-------------- end of 12.2K mode tables ---------









































