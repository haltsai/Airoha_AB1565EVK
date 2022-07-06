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

#include "dsp_nvkey_vad_para.h"  

#define ENDIAN_RVRS(A)	(A)

DSP_NVKEY_VAD_PARA NvKey_1M_VAD =
{                                              
    ENDIAN_RVRS(0x0001),       // S16 en_vad;                 
    ENDIAN_RVRS(0x0005),       // S16 gamma;                  
    ENDIAN_RVRS(0x00E6),       // S16 diff_fac;               
    ENDIAN_RVRS(0x00DC),       // S16 diff_facb;              
    ENDIAN_RVRS(0x000F),       // S16 vad_fac;                
    ENDIAN_RVRS(0x0015),       // S16 vad_ph_cc;              
    ENDIAN_RVRS(0x0006),       // S16 vad_ph_init;            
    ENDIAN_RVRS(0x007F),       // S16 vad_ph_range;           
    ENDIAN_RVRS(0x4A3D),       // S16 vad_corr_th;            
    ENDIAN_RVRS(0x0A3D),       // S16 vad_pitch_th;           
    ENDIAN_RVRS(0x0010),       // S16 win_sm_1st;             
    ENDIAN_RVRS(0x1F40),       // S16 sil_thr   ;             
    ENDIAN_RVRS(0x0064),       // S16 sil_thr2  ;             
    ENDIAN_RVRS(0x0021),       // S16 fr_thr    ;             
    ENDIAN_RVRS(0x0001),       // S16 fr_thr2   ;             
    ENDIAN_RVRS(0x11F8),       // S16 rec_thr   ;             
    ENDIAN_RVRS(0x2328),       // S16 rec_thr2  ;             
    ENDIAN_RVRS(0x1004),       // S16 end_thr   ;             
    ENDIAN_RVRS(0x0AF0),       // S16 gar_thr   ;             
    ENDIAN_RVRS(0x06D6),       // S16 rel_thr   ;             
    ENDIAN_RVRS(0x1B58),       // S16 ss_thr    ;             
    ENDIAN_RVRS(0x1518),       // S16 ms_thr    ;             
    ENDIAN_RVRS(0x1388),       // S16 ss_rel_thr;             
    ENDIAN_RVRS(0x5BCC),       // S16 ss_g_thr  ;             
    ENDIAN_RVRS(0x1964),       // S16 fa_end_thr;             
    ENDIAN_RVRS(0x1004),       // S16 fa_beg_thr;             
    ENDIAN_RVRS(0x07D0),       // S16 short_thr ;             
    ENDIAN_RVRS(0x0320),       // S16 rel_st4_thr;            
    ENDIAN_RVRS(0x170C),       // S16 rel_oth_thr;            
    ENDIAN_RVRS(0x0032),       // S16 dur_thr   ;             
    ENDIAN_RVRS(0x0064),       // S16 max_dur_thr;            
    ENDIAN_RVRS(0x1838),       // S16 ms_s_thr  ;             
    ENDIAN_RVRS(0x000F),       // S16 vad_leave_th;           
    ENDIAN_RVRS(0x030D),       // S16 vad_eng_thr;            
    ENDIAN_RVRS(0x0000),       // U16 RESERVE_A ;             
    ENDIAN_RVRS(0x0000),       // U16 RESERVE_B ;             
    ENDIAN_RVRS(0x0000),       // U16 RESERVE_C ;             
    ENDIAN_RVRS(0x0000)        // U16 RESERVE_D ;     
}; 

DSP_NVKEY_VAD_PARA NvKey_1M_CON =
{                                              
    ENDIAN_RVRS(0x0000),       // S16 en_vad;                 
    ENDIAN_RVRS(0x0005),       // S16 gamma;                  
    ENDIAN_RVRS(0x00E6),       // S16 diff_fac;               
    ENDIAN_RVRS(0x00DC),       // S16 diff_facb;              
    ENDIAN_RVRS(0x0032),       // S16 vad_fac;                
    ENDIAN_RVRS(0x0015),       // S16 vad_ph_cc;              
    ENDIAN_RVRS(0x0006),       // S16 vad_ph_init;            
    ENDIAN_RVRS(0x007F),       // S16 vad_ph_range;           
    ENDIAN_RVRS(0x4000),       // S16 vad_corr_th;            
    ENDIAN_RVRS(0x0A3D),       // S16 vad_pitch_th;           
    ENDIAN_RVRS(0x0010),       // S16 win_sm_1st;             
    ENDIAN_RVRS(0x2EE0),       // S16 sil_thr   ;             
    ENDIAN_RVRS(0x0514),       // S16 sil_thr2  ;             
    ENDIAN_RVRS(0x0021),       // S16 fr_thr    ;
	#ifdef MTK_WWE_ALEXA_ENABLE
	ENDIAN_RVRS(0x0002),       // S16 fr_thr2   ;             
    ENDIAN_RVRS(0x1B58),       // S16 rec_thr   ; 
	#else             
    ENDIAN_RVRS(0x0006),       // S16 fr_thr2   ;             
    ENDIAN_RVRS(0x0FA0),       // S16 rec_thr   ;
	#endif             
    ENDIAN_RVRS(0x2134),       // S16 rec_thr2  ;             
    ENDIAN_RVRS(0x1194),       // S16 end_thr   ;             
    ENDIAN_RVRS(0x0E74),       // S16 gar_thr   ;             
    ENDIAN_RVRS(0x07D0),       // S16 rel_thr   ;             
    ENDIAN_RVRS(0x1900),       // S16 ss_thr    ;             
    ENDIAN_RVRS(0x125C),       // S16 ms_thr    ;             
    ENDIAN_RVRS(0x1388),       // S16 ss_rel_thr;             
    ENDIAN_RVRS(0x2904),       // S16 ss_g_thr  ;             
    ENDIAN_RVRS(0x0FA0),       // S16 fa_end_thr;             
    ENDIAN_RVRS(0x1004),       // S16 fa_beg_thr;             
    ENDIAN_RVRS(0x0834),       // S16 short_thr ;             
    ENDIAN_RVRS(0x03E8),       // S16 rel_st4_thr;            
    ENDIAN_RVRS(0x157C),       // S16 rel_oth_thr;            
    ENDIAN_RVRS(0x002A),       // S16 dur_thr   ;             
    ENDIAN_RVRS(0x005A),       // S16 max_dur_thr;            
    ENDIAN_RVRS(0x1B58),       // S16 ms_s_thr  ;             
    ENDIAN_RVRS(0x0032),       // S16 vad_leave_th;  
	#ifdef MTK_WWE_ALEXA_ENABLE
	ENDIAN_RVRS(0x0186),       // S16 vad_eng_thr;
	#else         
    ENDIAN_RVRS(0x030D),       // S16 vad_eng_thr; 
	#endif           
    ENDIAN_RVRS(0x0000),       // U16 RESERVE_A ;             
    ENDIAN_RVRS(0x0000),       // U16 RESERVE_B ;             
    ENDIAN_RVRS(0x0000),       // U16 RESERVE_C ;             
    ENDIAN_RVRS(0x0000)        // U16 RESERVE_D ;     
}; 

DSP_NVKEY_VAD_PARA NvKey_2M_VAD =
{                                              
       ENDIAN_RVRS(0x0001),       // S16 en_vad;                 
    ENDIAN_RVRS(0x0001),       // S16 gamma;                  
    ENDIAN_RVRS(0x0076),       // S16 diff_fac;               
    ENDIAN_RVRS(0x0076),       // S16 diff_facb;              
    ENDIAN_RVRS(0x0010),       // S16 vad_fac;                
    ENDIAN_RVRS(0x0015),       // S16 vad_ph_cc;              
    ENDIAN_RVRS(0x0006),       // S16 vad_ph_init;            
    ENDIAN_RVRS(0x0040),       // S16 vad_ph_range;           
    ENDIAN_RVRS(0x2CCD),       // S16 vad_corr_th;            
    ENDIAN_RVRS(0x0A3D),       // S16 vad_pitch_th;           
    ENDIAN_RVRS(0x0010),       // S16 win_sm_1st;             
    ENDIAN_RVRS(0x2EE0),       // S16 sil_thr   ;             
    ENDIAN_RVRS(0x06A4),       // S16 sil_thr2  ;             
    ENDIAN_RVRS(0x0021),       // S16 fr_thr    ;             
    ENDIAN_RVRS(0x000D),       // S16 fr_thr2   ;             
    ENDIAN_RVRS(0x10CC),       // S16 rec_thr   ;             
    ENDIAN_RVRS(0x3A98),       // S16 rec_thr2  ;             
    ENDIAN_RVRS(0x1964),       // S16 end_thr   ;             
    ENDIAN_RVRS(0x1964),       // S16 gar_thr   ;             
    ENDIAN_RVRS(0x1194),       // S16 rel_thr   ;             
    ENDIAN_RVRS(0x1644),       // S16 ss_thr    ;             
    ENDIAN_RVRS(0x12C0),       // S16 ms_thr    ;             
    ENDIAN_RVRS(0x0001),       // S16 ss_rel_thr;             
    ENDIAN_RVRS(0x0001),       // S16 ss_g_thr  ;             
    ENDIAN_RVRS(0x1964),       // S16 fa_end_thr;             
    ENDIAN_RVRS(0x1004),       // S16 fa_beg_thr;             
    ENDIAN_RVRS(0x07D0),       // S16 short_thr ;             
    ENDIAN_RVRS(0x012C),       // S16 rel_st4_thr;            
    ENDIAN_RVRS(0x170C),       // S16 rel_oth_thr;            
    ENDIAN_RVRS(0x0032),       // S16 dur_thr   ;             
    ENDIAN_RVRS(0x0064),       // S16 max_dur_thr;            
    ENDIAN_RVRS(0x1F40),       // S16 ms_s_thr  ;             
    ENDIAN_RVRS(0x0032),       // S16 vad_leave_th;           
    ENDIAN_RVRS(0x030D),       // S16 vad_eng_thr;            
    ENDIAN_RVRS(0x0000),       // U16 RESERVE_A ;             
    ENDIAN_RVRS(0x0000),       // U16 RESERVE_B ;             
    ENDIAN_RVRS(0x0000),       // U16 RESERVE_C ;             
    ENDIAN_RVRS(0x0000)        // U16 RESERVE_D ;                  
}; 

DSP_NVKEY_VAD_PARA NvKey_2M_CON =
{                                              
    ENDIAN_RVRS(0x0001),       // S16 en_vad;                 
    ENDIAN_RVRS(0x0001),       // S16 gamma;                  
    ENDIAN_RVRS(0x0076),       // S16 diff_fac;               
    ENDIAN_RVRS(0x0076),       // S16 diff_facb;              
    ENDIAN_RVRS(0x0018),       // S16 vad_fac;                
    ENDIAN_RVRS(0x001E),       // S16 vad_ph_cc;              
    ENDIAN_RVRS(0x0006),       // S16 vad_ph_init;            
    ENDIAN_RVRS(0x0040),       // S16 vad_ph_range;           
    ENDIAN_RVRS(0x399A),       // S16 vad_corr_th;            
    ENDIAN_RVRS(0x7333),       // S16 vad_pitch_th;           
    ENDIAN_RVRS(0x0010),       // S16 win_sm_1st;             
    ENDIAN_RVRS(0x2EE0),       // S16 sil_thr   ;             
    ENDIAN_RVRS(0x07D0),       // S16 sil_thr2  ;             
    ENDIAN_RVRS(0x0019),       // S16 fr_thr    ;             
    ENDIAN_RVRS(0x000D),       // S16 fr_thr2   ;             
    ENDIAN_RVRS(0x10CC),       // S16 rec_thr   ;             
    ENDIAN_RVRS(0x3A98),       // S16 rec_thr2  ;             
    ENDIAN_RVRS(0x1964),       // S16 end_thr   ;             
    ENDIAN_RVRS(0x17D4),       // S16 gar_thr   ;             
    ENDIAN_RVRS(0x1B58),       // S16 rel_thr   ;             
    ENDIAN_RVRS(0x2134),       // S16 ss_thr    ;             
    ENDIAN_RVRS(0x0FA0),       // S16 ms_thr    ;             
    ENDIAN_RVRS(0x0001),       // S16 ss_rel_thr;             
    ENDIAN_RVRS(0x0001),       // S16 ss_g_thr  ;             
    ENDIAN_RVRS(0x0FA0),       // S16 fa_end_thr;             
    ENDIAN_RVRS(0x1004),       // S16 fa_beg_thr;             
    ENDIAN_RVRS(0x0834),       // S16 short_thr ;             
    ENDIAN_RVRS(0x03E8),       // S16 rel_st4_thr;            
    ENDIAN_RVRS(0x157C),       // S16 rel_oth_thr;            
    ENDIAN_RVRS(0x0032),       // S16 dur_thr   ;             
    ENDIAN_RVRS(0x005A),       // S16 max_dur_thr;            
    ENDIAN_RVRS(0x1B58),       // S16 ms_s_thr  ;             
    ENDIAN_RVRS(0x0032),       // S16 vad_leave_th;           
    ENDIAN_RVRS(0x030D),       // S16 vad_eng_thr;            
    ENDIAN_RVRS(0x0000),       // U16 RESERVE_A ;             
    ENDIAN_RVRS(0x0000),       // U16 RESERVE_B ;             
    ENDIAN_RVRS(0x0000),       // U16 RESERVE_C ;             
    ENDIAN_RVRS(0x0000)        // U16 RESERVE_D ;            
}; 

