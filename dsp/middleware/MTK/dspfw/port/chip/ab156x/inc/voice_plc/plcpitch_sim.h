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
 
#ifndef _plcpitch_sim_h
#define _plcpitch_sim_h

#define PACKED __attribute__((packed))

#define XTENSA
//typedef  short S16;
//typedef   int   S32;
#include "types.h"
#include "dsp_utilities.h"


#define USE_TEN_DIVIDER


//#define DEBUG				// define for debug dump data
//#define PLC_MAXLIKE			// define for max like plc, otherwise pitch PLC be run

#ifdef DEBUG
extern FILE *sys_ptr1;
extern FILE *sys_ptr2;
extern FILE *sys_ptr3;
#endif

/* meth operation */
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))









// pitch period copy time










// parameter declaraction
//extern int PLC_MUTE_CONSECUTIVE_ERRS_DEFAULT;    //%15;   %12;   % 20140515 modify
//extern int PLC_UNITY_GAIN_CONSECUTIVE_GOODFRM_DEFAULT;   //%10;
//extern int PLC_MUTE_PE16_TH_DEFAULT;   //%15;   %13;   % 20140515 modify
//extern int PLC_GAIN_UPDATE_PE16_TH_DEFAULT;   //%10;               %2;
//extern int PLC_PE16_ADJ_DEFAULT;
//extern int PLC_MUTE_GAIN_RATE_FACTOR_ATTACK_DEFAULT;  //%5;   %10;
//extern int PLC_MUTE_GAIN_RATE_FACTOR_RELEASE_DEFAULT; //%5;
//extern int PLC_WEIGHT_PKTLOSS_EN_DEFAULT;
//extern int PLC_WEIGHT_PKTLOSS_DEFAULT;
//extern int PLC_MIN_FRAME_EN;						// 1: divide to mini frame for GFSK: 30, EDR 2M: 60, EDR 3M: 90
//extern int PLC_MIN_FRAME_BER_METHOD;				// 2;   % 0: CRC, 1: near SNR, 2: worst SNR, 3: mean SNR using last no SNR period frame
//extern int PLC_SMART_EN;							// 1: adaptive plc type & gain control parameter
//extern int PLC_BER_GFRAME_EN;						// 1: include BER to good frame for bad frame
//extern int PLC_CRC_GFRAME_EN;						// 1: CRC to good frame for eSCO
//extern int PLC_BER_GFRAME_TH;						// when plc_ber_gframe_en set, this value to index good/bad frame, fix point
//extern int PLC_HEC_GFRAME_EN;						// HEC error, but CVSD still decoder, change to good frame by SNR or CRC
//extern int PLC_TONE_SBER_EN;						// when smart, single tone will enable strict BER condition
//extern int FRAME_AMP_TH;               // voice too small, fixed point
//extern int FRAME_UNVOICED_TH;		// unvoice zero corss number threshold > 3.5k=26/30(frame), fixed point from parameter
//extern int ZC_IIR_DIFF_TH;			// zero corss number & iir difference number threshold, fixed point from parameter
//extern int ZC_DIFF_TH;              // for continue similar zc number, fixed point from parameter
//extern int ZC_DIFF_NUM_TH;				// for torelence continue similar zc number, fixed point from parameter
//extern int FBER_MIIR_TH;            // middle average threshold, fixed point from parameter
//extern int FBER_SLIIR_TH;           // long & short average difference threshold, fixed point from parameter
//extern int FBER_MLIIR_TH;           // long & middle average difference threshold, fixed point from parameter
//extern int FS_16K;



#define x_buf x_replace
#define x_replace_tail  pitchbuff
#define x_replace_ovlp  x_frame_ovlp





#ifdef XTENSA
#ifndef XTENSA_H
#define XTENSA_H

#include <xtensa/tie/xt_hifi2.h>
#include <xtensa/tie/xt_misc.h>

#endif
#else
#endif

//-------------------------------------------------

#define  fber_liir_coe_exp   8
#define  fber_miir_coe_exp   6                             // bad frame middle average iir coefficient 2^m, m max is 9, because 31 bit use 5.17=22 bit, 31-22=9 bit for iir
#define  fber_siir_coe_exp   4                              // bad frame short average iir coefficient 2^m, m max is 9, because 31 bit use 5.17=22 bit, 31-22=9 bit for iir
#define  s_bit             4   	         				// right shift bit parameter by define for correlation sum not overflow


#define X_FRAME_OVLP_SIZE       16
#define X_REPLACE_TAIL_OVER_SIZE 16
#define X_REPLACE_OVLP_SIZE     16
#define MAX_FRAME_SIZE         540
#define MAX_DELAY_IN_SIZE  16

#if 0
#ifdef XTENSA
#define XT_ALIGN4 __attribute__((aligned(4)))
#else
#define XT_ALIGN4 __declspec(align(8))
#endif

#define DSP_ALIGN4 XT_ALIGN4
#endif

#ifdef XTENSA
#define _AE_Q32S_  (ae_q32s*)
#else
#define _AE_Q32S_

#endif


#ifdef XTENSA
#define _AE_P16X2S_  (ae_p16x2s*)
#else
#define _AE_P16X2S_

#endif
//data struction
typedef struct stru_dsp_plc_para_s {
 // part 1 yoyo's system use
  U8 ENABLE;
  U8 REVISION;
  //U16 PLC_ENABLE;
  U16 PLC_OFFSET;
  U16 PLC_OFFSET_mSBC;
  U16 PLC_FramePartEn;
  U16 PLC_FramePartMethod;
  U16 PLC_CRCChkMethod;

  //part 2 ,plc internal
  U16 PLC_MUTE_CONSECUTIVE_ERRS_DEFAULT;    //%15;   %12;   % 20140515 modify
  U16 PLC_UNITY_GAIN_CONSECUTIVE_GOODFRM_DEFAULT;   //%10;
  U16 PLC_MUTE_PE16_TH_DEFAULT;   //%15;   %13;   % 20140515 modify
  U16 PLC_GAIN_UPDATE_PE16_TH_DEFAULT;   //%10;               %2;
  U16 PLC_PE16_ADJ_DEFAULT;
  U16 PLC_MUTE_GAIN_RATE_FACTOR_ATTACK_DEFAULT;  //%5;   %10;
  U16 PLC_MUTE_GAIN_RATE_FACTOR_RELEASE_DEFAULT; //%5;
  U16 PLC_WEIGHT_PKTLOSS_EN_DEFAULT;
  U16 PLC_WEIGHT_PKTLOSS_DEFAULT;
  U16 PLC_MIN_FRAME_EN;						// 1: divide to mini frame for GFSK: 30, EDR 2M: 60, EDR 3M: 90
  U16 PLC_MIN_FRAME_BER_METHOD;				// 2;   % 0: CRC, 1: near SNR, 2: worst SNR, 3: mean SNR using last no SNR period frame
  U16 PLC_SMART_EN;							// 1: adaptive plc type & gain control parameter
  U16 PLC_BER_GFRAME_EN;						// 1: include BER to good frame for bad frame
  U16 PLC_CRC_GFRAME_EN;						// 1: CRC to good frame for eSCO
  U16 PLC_BER_GFRAME_TH;						// when plc_ber_gframe_en set, this value to index good/bad frame, fix point
  U16 PLC_HEC_GFRAME_EN;						// HEC error, but CVSD still decoder, change to good frame by SNR or CRC
  U16 PLC_TONE_SBER_EN;						// when smart, single tone will enable strict BER condition


}PACKED  DSP_PARA_PLC_STRU;


typedef struct {
	// pitch period copy time
	//int PLC_MIN_FRAME_BER_METHOD ;//16bit
	//int FS_16K;
	int	ovlp_size;
	int delay_in ;
	int convergence_time;		// 80 for 1k tone
	int dec_delay;				// 16 for 1k tone
	int period_extend;   //0:timing invariant pitch copy,  1: timing varying pitch copy
	int pitch_min;
	int pitch_max ;
	int pitch_diff;                       // 240-80=160
	int ovlp_max ;
   int copy_max_time;                                      // pitch period copy time
   int historlen;           // 240*2+60= 540
   int corrlen;											// 40*2 sample of 16kbps
   int corrbuflen;                         // 40*2 + 240 = 320
   int	PLC_FRAME_ERR_FLAGS;

   int weight_pktloss_en;						// weight for packet loss enable
   int weight_pktloss;					// continue packet loss weight
   int	PLC_FRAME_OUT_GAIN;
   int first_time;
   int poffset;
   int f_cnt;
   int zc_diff_size;
   int sec_time;
   int pitchblen;
   int pitch_buf_start;
   int pitch_buf_end;
   int	erasecnt;


	int fmsnr_size;    // SNR period frame sizse base on modulation type
	int	weight_pktloss_acc;          // max value is 32768, unsignedddddddddddddddddddddddddddd
	int	weight_pktloss_acc_d;        // max value is 32768, unsigned
   int	weight_pktloss_en_d;
   long fber_liir;
   long fber_siir;
   long fber_miir;
   long frame_zc_iir;
   int pitch;



    // plc gain control declaration
    int flag_length;
	int PLC_MUTE_CONSECUTIVE_ERRS ;
	int PLC_UNITY_GAIN_CONSECUTIVE_GOODFRM;
    int PLC_MUTE_PE16_TH ;
	int PLC_GAIN_UPDATE_PE16_TH;
	int PLC_PE16_ADJ;
   int PLC_MUTE_GAIN_RATE_FACTOR_ATTACK;
   int PLC_MUTE_GAIN_RATE_FACTOR_RELEASE;

   int fber_liir_coe;				// bad frame long average iir coefficient
   int fber_miir_coe;				// bad frame middle average iir coefficient
   int fber_siir_coe;				// bad frame short average iir coefficient
   long fber_th;					// absolution threshold for ber, fix point
   int zc_qexp ;
   int frame_zc_iir_coe_exp;		// zero corss number iir coefficient 2^m
   int frame_zc_iir_coe;			// zero corss number iir coefficient




//----------------------------- user parameter  -------
  int PLC_MUTE_CONSECUTIVE_ERRS_DEFAULT;    //%15;   %12;   % 20140515 modify
  int PLC_UNITY_GAIN_CONSECUTIVE_GOODFRM_DEFAULT;   //%10;
  int PLC_MUTE_PE16_TH_DEFAULT;   //%15;   %13;   % 20140515 modify
  int PLC_GAIN_UPDATE_PE16_TH_DEFAULT;   //%10;               %2;
  int PLC_PE16_ADJ_DEFAULT;
  int PLC_MUTE_GAIN_RATE_FACTOR_ATTACK_DEFAULT;  //%5;   %10;
  int PLC_MUTE_GAIN_RATE_FACTOR_RELEASE_DEFAULT; //%5;
  int PLC_WEIGHT_PKTLOSS_EN_DEFAULT;
  int PLC_WEIGHT_PKTLOSS_DEFAULT;
  int PLC_MIN_FRAME_EN;						// 1: divide to mini frame for GFSK: 30, EDR 2M: 60, EDR 3M: 90
  int PLC_MIN_FRAME_BER_METHOD;				// 2;   % 0: CRC, 1: near SNR, 2: worst SNR, 3: mean SNR using last no SNR period frame
  int PLC_SMART_EN;							// 1: adaptive plc type & gain control parameter
  int PLC_BER_GFRAME_EN;						// 1: include BER to good frame for bad frame
  int PLC_CRC_GFRAME_EN;						// 1: CRC to good frame for eSCO
  int PLC_BER_GFRAME_TH;						// when plc_ber_gframe_en set, this value to index good/bad frame, fix point
  int PLC_HEC_GFRAME_EN;						// HEC error, but CVSD still decoder, change to good frame by SNR or CRC
  int PLC_TONE_SBER_EN;						// when smart, single tone will enable strict BER condition
  int FRAME_AMP_TH;               // voice too small, fixed point
  int FRAME_UNVOICED_TH;		// unvoice zero corss number threshold > 3.5k=26/30(frame), fixed point from parameter
  int ZC_IIR_DIFF_TH;			// zero corss number & iir difference number threshold, fixed point from parameter
  int ZC_DIFF_TH;              // for continue similar zc number, fixed point from parameter
  int ZC_DIFF_NUM_TH;				// for torelence continue similar zc number, fixed point from parameter
  int FBER_MIIR_TH;            // middle average threshold, fixed point from parameter
  int FBER_SLIIR_TH;           // long & short average difference threshold, fixed point from parameter
  int FBER_MLIIR_TH;           // long & middle average difference threshold, fixed point from parameter
  int FS_16K;  //external setting

//--------------------------------------------------
   DSP_ALIGN8 short plc_tmp[32];
   DSP_ALIGN8 long x_replace_tail_ovlp[X_REPLACE_TAIL_OVER_SIZE];
   DSP_ALIGN8 long x_replace_ovlp[X_REPLACE_OVLP_SIZE];
   DSP_ALIGN8 short rc_win_left[16];
   DSP_ALIGN8 short rc_win_right[16];
   DSP_ALIGN8 short pitchbuff[MAX_FRAME_SIZE];			// 16 bits
   DSP_ALIGN8 short lastq[60];				           // 16 bits
   DSP_ALIGN8 short x_buf[MAX_FRAME_SIZE];				// 16 bits
   //opt it! long x_frame_ovlp[X_FRAME_OVLP_SIZE];
   DSP_ALIGN8 int  frame_zc_buffer[20];
   DSP_ALIGN8 short fespeechout[MAX_FRAME_SIZE];	//local @cvsd_a2plc  ,maybe can be to optimize
   // opt it!  XT_ALIGN4 short x_replace_tail[MAX_FRAME_SIZE];	//local @cvsd_a2plc  ,maybe can be to optimize
   //opt it! XT_ALIGN4 short x_replace[MAX_FRAME_SIZE];	   //local @cvsd_a2plc  ,maybe can be to optimize
   DSP_PARA_PLC_STRU  plc_paras;
}PLC_state;




extern const S16 rc_win_right_16s[16];
extern const S16 rc_win_left_16s[16];
extern const S32 zc_diff_size_tab[3] ;




#ifndef USE_TEN_DIVIDER
void my_div(double av, double bv, int quantize_en, double &c, int &sc);
double quant_floor(double in, int wlength, int wdepth, int en);
double quant_round(double in, int wlength, int wdepth, int en);
#endif


int Q_ubitlength(int in, int outlength);
void plc_gainctrl_ini(PLC_state *s);
void plc_gainctrl(int fisgood ,short *x_frame,int frame_size ,PLC_state *s);
void plc_framstus(int fisgood_i, int fcrc, long fber, int ber_mliir_change, int ber_miir_change, int ber_sliir_change,
				  int unvoice_detect, int frame_small, int *ber_change, int *fisgood,int eSCO,int frame_size ,PLC_state *s);
long snr2ber_8k(int mod_type, int snr_8k);
void plc_stactrl(long fber, int frame_size, int f_number, int zc_diff_size,
				 int *ber_miir_change, int *ber_sliir_change, int *ber_mliir_change, int *unvoice_detect, int *frame_small,short *x_frame,PLC_state *s ) ;
void find_pitch_ini();
int find_pitch(short *pitchbuff, int corrbuflen, int corrlen, int pitch_max, int pitch_diff, int pitch_buf_end);
void getfespeech(short *pitchbuff, int pitchblen, int pitch_buf_start, int sz , short *out,PLC_state *s);



void plcpitch_sim_init(PLC_state *s);
void plcpitch_para_init(PLC_state *s);

void plc_stactrl_ini(int mod_type,PLC_state *s);

#ifdef PLC_MAXLIKE
void cvsd_a3plc_ini();
void cvsd_a3plc(int fisgood, int fcrc, int fsnr);
#else
void cvsd_a2plc_ini(int mod_type,PLC_state *s);
void SMART_PITCH_PLC_PROCESS(short *x_frame ,int fisgood, int fcrc, int fsnr,int mod_type,int eSCO,int frame_size, PLC_state *s);
#endif

void SMART_PITCH_PLC_INI(PLC_state *s,int mod_type) ;

#endif
