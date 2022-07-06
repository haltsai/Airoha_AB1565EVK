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
#ifndef _DSP_PARA_AEC_NR_H_
#define _DSP_PARA_AEC_NR_H_

#include "types.h"

/**
 * @brief Parameter for DSP AEC/NR algorithm
 * @KeyID 0xE150
 */

#ifdef WIN32
	#pragma pack(1)
#endif

#ifdef MTK_INEAR_ENHANCEMENT
typedef struct stru_dsp_aec_nr_para_s
{
	U8  ENABLE;                                        	/**< @Value 0x01 @Desc 1 */
	U8  REVISION;                                      	/**< @Value 0x01 @Desc 1 */
	U16 AEC_NR_EN;		                               	/**< @Value 0x0003 @Desc b0: EC/NR switch, b1-2: 0(1-MIC), 1(1-MIC PD), 2(2-MIC), b3: 0(headset),1(speaker), b13: 0(disable Rx HPF), 1(enable Rx HPF), b15: 0(disable flash read) ,1(enable flash read)*/
	U16 AEC_CNG_GAIN_M;                                	/**< @Value 0x3333 @Desc 1 */
	U16 AEC_ref_pow_min;                               	/**< @Value 0x0001 @Desc 1 */
	U16 AEC_ECHO_TAIL_LENGTH;                          	/**< @Value 0x0006 @Desc 1 */
	U16 AEC_EC_RESIST;																	/**< @Value 0x0000 @Desc 1 */
	U16 AEC_MU_FAC;																			/**< @Value 0x399A @Desc 1 */
	U16 AEC_MU_MIN;																			/**< @Value 0x1000 @Desc 1 */
	U16 AEC_NORM_CAP1;																	/**< @Value 0x1FD7 @Desc 1 */
	U16 AEC_NORM_CAP2;																	/**< @Value 0x3000 @Desc 1 */
	U16 AEC_PF_MIN;																			/**< @Value 0x0400 @Desc 1 */
	S32 AEC_block_percent;															/**< @Value 0x00155555 @Desc 1 */
	U16 AEC_DT_boost;																		/**< @Value 0x0001 @Desc 1 */
	U16 AEC_PF_order;																		/**< @Value 0x0003 @Desc 1 */
	U16 AEC_DT_ratio_thrd;															/**< @Value 0x4000 @Desc 1 */
	U16 AEC_norm_tap;																		/**< @Value 0x0018 @Desc 1 */
	U16 AEC_DT_length;																	/**< @Value 0x0006 @Desc 1 */
	U16 MULT_AFTER_EC;																	/**< @Value 0x1000 @Desc 1 */
	S16 CH1_REF_GAIN;																		/**< @Value 0x0021 @Desc 1 */
	S16 CH2_REF_GAIN;																		/**< @Value 0x0021 @Desc 1 */
	S16 CH3_REF_GAIN;																		/**< @Value 0x0852 @Desc 1 */
	S16 CH4_REF_GAIN;																		/**< @Value 0x299A @Desc 1 */
	S16 CH1_REF2_GAIN;																	/**< @Value 0x0148 @Desc 1 */
	S16 CH2_REF2_GAIN;																	/**< @Value 0x028F @Desc 1 */
	S16 CH3_REF2_GAIN;																	/**< @Value 0x028F @Desc 1 */
	S16 CH4_REF2_GAIN;																	/**< @Value 0x028F @Desc 1 */

	U16 AVC_ALPHA;																			/**< @Value 0x799A @Desc 1 */
	U16 AVC_THRD;																				/**< @Value 0x0037 @Desc 1 */
	U16 AVC_VOL_MAX;  																	/**< @Value 0x7FFF @Desc 1 */
	U16 DSP_EC_SW;																			/**< @Value 0x0000 @Desc 1 */

  U16 WB_NR_TX_POW_MIN_BUF_PERIOD;										/**< @Value 0x000B @Desc 1 */
  U16 WB_NR_TX_NOISE_GAIN_LIMITER;   									/**< @Value 0x0FEE @Desc 1 */
  U16 WB_NR_TX_VAD_THRD1;            									/**< @Value 0x0348 @Desc 1 */
  U16 WB_NR_TX_VAD_THRD_BANDS_0;    									/**< @Value 0x0B0B @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_1;    									/**< @Value 0x0B08 @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_2;    									/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_3;    									/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_4;    									/**< @Value 0x0A07 @Desc 1 */
  U16 WB_NR_TX_OVER_SUPPRESS_FAC;    									/**< @Value 0x219A @Desc 1 */
  U16 WB_NR_TX_SPEECH_RETAIN_FAC;    									/**< @Value 0x599A @Desc 1 */
  U16 WB_NR_TX_NOISE_LAMDA;          									/**< @Value 0x799A @Desc 1 */
  U16 WB_NR_TX_NOISE_FLOOR_MIN;      									/**< @Value 0x8400 @Desc 1 */
  U16 WB_NR_FAST_ALPHA;		          									/**< @Value 0x747B @Desc 1 */
  U16 WB_NR_SLOW_ALPHA;	            									/**< @Value 0x7C29 @Desc 1 */
  U16 WB_NR_NOISE_UPDATE_FAST;       									/**< @Value 6554	 @Desc 1 */
  U16 WB_NR_NOISE_UPDATE_SLOW;       									/**< @Value 983		 @Desc 1 */
  U16 WB_NR_NOISE_UPDATE_ULTRASLOW; 									/**< @Value 131		 @Desc 1 */
  U16 WB_NR_TX_EMPH_COEF_0;												/**< @Value 0xC287 @Desc 1 */
	U16 WB_NR_TX_EMPH_COEF_1;											/**< @Value 0x15CC @Desc 1 */

  U16 NB_NR_RX_POW_MIN_BUF_PERIOD;  									/**< @Value 0x000D @Desc 1 */
  U16 NB_NR_RX_NOISE_GAIN_LIMITER;  									/**< @Value 0x1EB8 @Desc 1 */
  U16 NB_NR_RX_VAD_THRD1;           									/**< @Value 0x0348 @Desc 1 */
  U16 NB_NR_RX_VAD_THRD_BANDS_0;   										/**< @Value 0x0b0b @Desc 1 */
	U16 NB_NR_RX_VAD_THRD_BANDS_1;   										/**< @Value 0x0b08 @Desc 1 */
	U16 NB_NR_RX_VAD_THRD_BANDS_2;   										/**< @Value 0x0808 @Desc 1 */
	U16 NB_NR_RX_VAD_THRD_BANDS_3;   										/**< @Value 0x0807 @Desc 1 */
  U16 NB_NR_RX_OVER_SUPPRESS_FAC;   									/**< @Value 0x2666 @Desc 1 */
  U16 NB_NR_RX_SPEECH_RETAIN_FAC;   									/**< @Value 0x6000 @Desc 1 */
  U16 NB_NR_RX_NOISE_LAMDA;         									/**< @Value 0x799a @Desc 1 */
  U16 NB_NR_RX_NOISE_FLOOR_MIN;     									/**< @Value 0xae00 @Desc 1 */
  U16 NB_NR_RX_EMPH_COEF_0;														/**< @Value 0xC8C0 @Desc 1 */
	U16 NB_NR_RX_EMPH_COEF_1;														/**< @Value 0x24AF @Desc 1 */

	U16 WB_NR_RX_POW_MIN_BUF_PERIOD;										/**< @Value 0x000b @Desc 1 */
	U16 WB_NR_RX_NOISE_GAIN_LIMITER;										/**< @Value 0x1eb8 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD1;															/**< @Value 0x0348 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_0;											/**< @Value 0x0b0b @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_1;											/**< @Value 0x0b08 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_2;											/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_3;											/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_4;											/**< @Value 0x0a07 @Desc 1 */
	U16 WB_NR_RX_OVER_SUPPRESS_FAC;											/**< @Value 0x2800 @Desc 1 */
	U16 WB_NR_RX_SPEECH_RETAIN_FAC;											/**< @Value 0x6000 @Desc 1 */
	U16 WB_NR_RX_NOISE_LAMDA;														/**< @Value 0x799a @Desc 1 */
	U16 WB_NR_RX_NOISE_FLOOR_MIN;												/**< @Value 0xae00 @Desc 1 */
	U16 WB_NR_RX_EMPH_COEF_0;														/**< @Value 0xCA16 @Desc 1 */
	U16 WB_NR_RX_EMPH_COEF_1;														/**< @Value 0x0B29 @Desc 1 */

	U16 PD_NR_TX_OPT;																		/**< @Value      0 @Desc 1 */
	U16 PD_PEAK_BUF_SIZE;																/**< @Value      0 @Desc 1 */
	U16 PD_PITCH_VAD_THRD;															/**< @Value  20480 @Desc 1 */
	U16 PD_PITCH_FAC;																		/**< @Value  31949 @Desc 1 */
	U16 PD_PEAK_RATIO_FAC;															/**< @Value  30032 @Desc 1 */
	U16 PD_TRANSIENT_THRD;															/**< @Value 0x0046 @Desc 1 */
	U16 PD_TRANSIENT_AUG;																/**< @Value   2048 @Desc 1 */
	U16 PD_PITCH_AUG;																		/**< @Value    328 @Desc 1 */
	U16 PD_TRANSIENT_THRD2;															/**< @Value 0x0028 @Desc 1 */
	U16 PD_RESERVE2;																		/**< @Value      0 @Desc 1 */

	U16 M2_MICDIST;																			/**< @Value 0x0000 @Desc 1 */
	U16 M2_WIND_THRD;																		/**< @Value 0x1000 @Desc 1 */
	U16 M2_WIND_TRANS;																	/**< @Value 0x0640 @Desc 1 */
	U16 M2_WIND_BLOCK;																	/**< @Value 0x1000 @Desc 1 */
	U16 M2_FILTER_GRD;																	/**< @Value 0x0002 @Desc 1 */
	U16 M2_DISTANCE_FAC;																/**< @Value 0x6119 @Desc 1 */
	U16 M2_VAD_THRD_1;																	/**< @Value 0x1062 @Desc 1 */
	U16 M2_VAD_THRD_2;																	/**< @Value 0x0C4A @Desc 1 */
	U16 M2_VAD_THRD_3;																	/**< @Value 0x0852 @Desc 1 */
	U16 M2_VAD_THRD_4;																	/**< @Value 0x03D7 @Desc 1 */
	U16 M2_VAD_THRD_12;																	/**< @Value 0x1333 @Desc 1 */
	U16 M2_VAD_THRD_22;																	/**< @Value 0x0E14 @Desc 1 */
	U16 M2_VAD_THRD_32;																	/**< @Value 0x0666 @Desc 1 */
	U16 M2_VAD_THRD_42;																	/**< @Value 0x028F @Desc 1 */
	U16 M2_VAD_THRD_13;																	/**< @Value 0x2A3D @Desc 1 */
	U16 M2_VAD_THRD_23;																	/**< @Value 0x15C3 @Desc 1 */
	U16 M2_VAD_THRD_33;																	/**< @Value 0x0B85 @Desc 1 */
	U16 M2_VAD_THRD_43;																	/**< @Value 0x051F @Desc 1 */
	U16 M2_NORM_FAC1;																		/**< @Value 0x0000 @Desc 1 */
	U16 M2_PHASE_COMB;																	/**< @Value 0x5C02 @Desc 1 */
	U16 M2_PHASE_GAIN;																	/**< @Value 0x4000 @Desc 1 */
	U16 M2_NE_DURATION;																	/**< @Value 0x0038 @Desc 1 */

	U16 M2_BEAM1_NORM_LIMIT;														/**< @Value 0x6000 @Desc 1 */
	U16 M2_OVER_SUPPRESS_FAC2;													/**< @Value 0x2000 @Desc 1 */
	S16 M2_BAND1_ALPHA;																	/**< @Value 0x7800 @Desc 1 */
	S16 M2_BAND2_ALPHA;																	/**< @Value 0x7D71 @Desc 1 */
	S16 M2_BAND3_ALPHA;																	/**< @Value 0x7F80 @Desc 1 */
	S16 M2_BANDH_ALPHA;																	/**< @Value 0x7F5C @Desc 1 */
	
	S16 OFF_PITCH_ALPHA;																/**< @Value 0x0106 @Desc 1 */
	S16 WIND_DETECT_THRD;																/**< @Value 0x0014 @Desc 1 */
	S16 PD_WIND_FREQ_RANGE;															/**< @Value 0x0011 @Desc 1 */
	S16 PD_WIND_LOW_ATTEN;															/**< @Value 0x65AD @Desc 1 */
	S16 PD_NONSTATIONARY_ALPHA_1;												/**< @Value 0x0106 @Desc 1 */
	S16 PD_NONSTATIONARY_ALPHA_2;												/**< @Value 0x028F @Desc 1 */
	
	S16 M2_PD_HIGH_ATTEN_GOAL;													/**< @Value	13045		@Desc 1 */
	S32 M2_PD_SEVERE_NOISE_THRD;												/**< @Value 8388352 @Desc 1 */
  S16 NR_HIGH_ATTEN_DIVIDE;  													/**< @Value 109  		@Desc 1 */
  
  U16 WB_VOICE_TX_GAIN;																/**< @Value 0x0800 @Desc 1 */
  U16 NB_VOICE_TX_GAIN;																/**< @Value 0x0800 @Desc 1 */

	S32 VOICE_TX_HP_COEF_0;								              /**< @Value 6731724  @Desc 1 */
	S32 VOICE_TX_HP_COEF_1;								              /**< @Value -6725950 @Desc 1 */
	S32 VOICE_TX_HP_COEF_2;								              /**< @Value 6731723  @Desc 1 */
	S32 VOICE_TX_HP_COEF_3;								              /**< @Value -6986246 @Desc 1 */
	S32 VOICE_TX_HP_COEF_4;								              /**< @Value 5970844  @Desc 1 */
	S32 VOICE_TX_HP_COEF_5;								              /**< @Value 8329426  @Desc 1 */
	S32 VOICE_TX_HP_COEF_6;								              /**< @Value -8293150 @Desc 1 */
	S32 VOICE_TX_HP_COEF_7;								              /**< @Value 8329425  @Desc 1 */
	S32 VOICE_TX_HP_COEF_8;								              /**< @Value -8072154 @Desc 1 */
	S32 VOICE_TX_HP_COEF_9;								              /**< @Value 7972216  @Desc 1 */
	S32 VOICE_TX_HP_COEF_10;							              /**< @Value 4194304  @Desc 1 */

	S32 VOICE_WB_RX_HP_COEF_0;	                        /**< @Value 7798431  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_1;	                        /**< @Value -7798015 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_2;	                        /**< @Value 7798430  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_3;	                        /**< @Value -8032569 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_4;	                        /**< @Value 7703874  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_5;	                        /**< @Value 8347007  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_6;	                        /**< @Value -8344739 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_7;	                        /**< @Value 8347006  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_8;	                        /**< @Value -8328435 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_9;	                        /**< @Value 8282082  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_10;	                        /**< @Value 4194304  @Desc 1 */	
	
	S32 VOICE_NB_RX_HP_COEF_0;	                        /**< @Value 7398364  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_1;	                        /**< @Value -7396782 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_2;	                        /**< @Value 7398363  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_3;	                        /**< @Value -7679575 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_4;	                        /**< @Value 7075462  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_5;	                        /**< @Value 8373828  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_6;	                        /**< @Value -8364724 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_7;	                        /**< @Value 8373827  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_8;	                        /**< @Value -8255389 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_9;	                        /**< @Value 8177081  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_10;	                        /**< @Value 4194304  @Desc 1 */

	U16 AEC_Corel_Ratio1_Sensitive;											/**< @Value 0x0100	   @Desc 1 */
	S16 PD_VOICE_IN_WIND;                               /**< @Value 0x7FFF	   @Desc 1 */  
	S16 PD_VOICE_AMP_LEVEL;                             /**< @Value 0x4000	   @Desc 1 */  
	S16 M2_HF_NSG2_OVER;	                              /**< @Value 0x0000	   @Desc 1 */
	S16 M2_DF_RATIO;																		/**< @Value 0x6CCD	   @Desc 1 */
	S16 M2_POW1_ALPHA;																	/**< @Value 0x0CCD	   @Desc 1 */
	S16 M2_POW1_UP_GAIN;																/**< @Value 0x0028	   @Desc 1 */
	S16 M2_POW1_DOWN_GAIN;															/**< @Value 0x00A4	   @Desc 1 */
	S16 M2_POW1_THRD1;																	/**< @Value 0x30A4	   @Desc 1 */
	S16 M2_POW1_THRD2;																	/**< @Value 0x1333	   @Desc 1 */
	U16 M2_VAD3IN4;																			/**< @Value 0x0000	   @Desc 1 */
	S16 M2_POW1_VAD_RATIO;															/**< @Value 0x4000	   @Desc 1 */
	U16 M2_DEEMPH_COEF_1;																/**< @Value 0x5A3D	   @Desc 1 */
	U16 M2_DEEMPH_COEF_2;																/**< @Value 0x6536	   @Desc 1 */
	U16 M2_DEEMPH_COEF_3;																/**< @Value 0xD3A9	   @Desc 1 */
	S16 M2_BAND2_GAIN;																	/**< @Value 0x2D3F	   @Desc 1 */
	S16 M2_BAND3_GAIN;																	/**< @Value 0x4000	   @Desc 1 */
	S16 M2_BANDH_GAIN;																	/**< @Value 0x2000	   @Desc 1 */
	U16 AEC_Corel_Ratio4_Sensitive; 										/**< @Value 0x0100     @Desc 1 */
	S16 AEC_Ch4_Percentage;															/**< @Value 0x0000     @Desc 1 */
	U16 RXIN_TXREF_DELAY;																/**< @Value 0x0004	   @Desc 1 */
	U16 MIC_EC_DELAY	;																	/**< @Value 0x0014	   @Desc 1 */
	U16 AEC_REF_GAIN_AUTO;															/**< @Value 0x0000     @Desc 1 */
	
	S16 M2_REF_MIC_CH1_GAIN;														/**< @Value 0x2000     @Desc 1 */
	S16 M2_REF_MIC_CH2_GAIN;														/**< @Value 0x2000     @Desc 1 */
	S16 M2_REF_MIC_CH3_GAIN;														/**< @Value 0x2000     @Desc 1 */
	S16 M2_REF_MIC_CH4_GAIN;														/**< @Value 0x2000     @Desc 1 */
	
	S16 Mel_scale_band_num; 														/**< @Value 0x0000     @Desc 1 */
	S16 Harmonics_thrd;																	/**< @Value 0x0005     @Desc 1 */
	
	S16 AEC_tap_length;			/**< @Value 0x0000     @Desc 1 */
	
	//Inear parameter
	S16 IE_main_type;                    /**< @Value 2     @Desc 1 */
    S16 IE_main_alpha;                   /**< @Value 31130 @Desc 1 */
    S16 IE_main_alpha_1;                 /**< @Value 1637  @Desc 1 */
    S16 IE_main_gain_limiter;            /**< @Value 583   @Desc 1 */
                                        
    S16 IE_inear_type;                   /**< @Value 2     @Desc 1 */
    S16 IE_inear_alpha;                  /**< @Value 31130 @Desc 1 */
    S16 IE_inear_alpha_1;                /**< @Value 1637  @Desc 1 */
    S16 IE_inear_gain_limiter;		     /**< @Value 583   @Desc 1 */
    
	S16 IE_ENG_VAD_THR;					/**< @Value 0x0f33 @Desc 1 */	
    S16 IE_ENG_VAD_HANG;				/**< @Value 0x000a @Desc 1 */
    S16 IE_ENG_POW_INIT;				/**< @Value -9216 @Desc 1 */
    S16 IE_slow_up_alpha;				/**< @Value 0x0021 @Desc 1 */
    S16 IE_slow_dn_alpha;				/**< @Value 0x4000 @Desc 1 */
    S16 IE_fast_up_alpha;				/**< @Value 0x4000 @Desc 1 */
    S16 IE_fast_dn_alpha;				/**< @Value 0x028f @Desc 1 */
    S16 IE_end_inear_wind;			    /**< @Value     83 @Desc for 2+1 nr */
	
	S32 IE_msc_alpha;					/**< @Value 429496832 @Desc 1 */
    S32 IE_msc_sm_up_alpha;				/**< @Value 6442496 @Desc 1 */
    S32 IE_msc_sm_dn_alpha;				/**< @Value 21474816 @Desc 1 */
    S32 IE_msc_bias;					/**< @Value 0x00040000 @Desc 1 */
    S32 IE_MSC_WIND_THR2;				/**< @Value 142606336 @Desc 1 */
    S32 IE_MSC_WIND_THR;				/**< @Value 167772160 @Desc 1 */
                                       
    S16 IE_mid_eq;                       /**< @Value 0x1000 @Desc 1 */
    S16 IE_end_inear_est;                /**< @Value 0x0030 @Desc 1 */
    S16 IE_mid_eq_st;                    /**< @Value 0x000f @Desc 1 */
    S16 IE_inEar_nor_gain0;              /**< @Value 11627 @Desc 1 */
    S16 IE_inEar_nor_gain1;              /**< @Value 11627 @Desc 1 */
	S16 IE_ff_mel_filter;					/**< @Value 0x0002 @Desc 0: disable melfilter, 1: enable melfilter 26 bands, 2: enable melfilter 64 bands */
    S16 IE_fb_mel_filter;					/**< @Value 0x0000 @Desc 0: disable melfilter, 1: enable melfilter 26 bands, 2: enable melfilter 64 bands */
    S16 IE_en_pre_emp;						/**< @Value 0x0066 @Desc bit0: enable pre_emp, bit1: enable low band noise est, bit2: enable low band phase compensation, bit3: low band phase inverse, bit4: zero phase de-pop, bit5: use ast_eq to enhance ff noise est, bit6: enable pitch enhancement, bit7: when fe_vad_on no enhance low bin noise, bit8: fb enable mel smooth, bit9: enable mic status check, bit14: calibration fb mic, bit15: calibration ff mic*/
	S16	IE_vad_credit_thr;					/**< @Value 0x0800 @Desc 1 */
	S16 IE_padding2;						/**< @Value 0x0000 @Desc 1 */	
	S16 IE_main_alpha_peak_up;             /**< @Value 0x747b @Desc 1 */
    S16 IE_main_alpha_peak_dn;             /**< @Value 0x7c29 @Desc 1 */
    S16 IE_main_alpha_valley_dn;           /**< @Value 0x747b @Desc 1 */
    S16 IE_main_alpha_valley_up;           /**< @Value 0x7c29 @Desc 1 */
    S16 IE_main_pow_diff_thr;              /**< @Value 0x0348 @Desc 1 */
    S16 IE_main_pow_credit;                /**< @Value 0x03e8 @Desc 1 */
    S16 IE_main_alpha_sm_fast;             /**< @Value 0x4ccd @Desc 1 */
    S16 IE_main_alpha_sm_slow;             /**< @Value 0x6ccd @Desc 1 */
    S16 IE_main_noise_update_thr0;         /**< @Value 0x0a80 @Desc 1 */
    S16 IE_main_noise_update_thr1;         /**< @Value 0x0780 @Desc 1 */
    S16 IE_main_noise_update_thr2;         /**< @Value 0x0300 @Desc 1 */
    S16 IE_main_padding;                   /**< @Value 0x0000 @Desc 1 */
   
    S32 IE_main_noise_alpha_fast;          /**< @Value 0x66666600 @Desc 1 */
    S32 IE_main_noise_alpha_slow;          /**< @Value 0x7c28f600 @Desc 1 */
    S32 IE_main_noise_alpha_ultra_slow;    /**< @Value 0x7f7cee00 @Desc 1 */
	S32 IE_main_over_est;					/**< @Value 0x19999a00 @Desc 1 */
	                                        
    S16 IE_inear_alpha_peak_up;            /**< @Value 0x747b @Desc 1 */
    S16 IE_inear_alpha_peak_dn;            /**< @Value 0x7c29 @Desc 1 */
    S16 IE_inear_alpha_valley_dn;          /**< @Value 0x747b @Desc 1 */
    S16 IE_inear_alpha_valley_up;          /**< @Value 0x7c29 @Desc 1 */
    S16 IE_inear_pow_diff_thr;             /**< @Value 0x0348 @Desc 1 */
    S16 IE_inear_pow_credit;               /**< @Value 0x03e8 @Desc 1 */
    S16 IE_inear_alpha_sm_fast;            /**< @Value 0x4ccd @Desc 1 */
    S16 IE_inear_alpha_sm_slow;            /**< @Value 0x6ccd @Desc 1 */
    S16 IE_inear_noise_update_thr0;        /**< @Value 0x0a80 @Desc 1 */
    S16 IE_inear_noise_update_thr1;        /**< @Value 0x0780 @Desc 1 */
    S16 IE_inear_noise_update_thr2;        /**< @Value 0x0300 @Desc 1 */
    S16 IE_inear_padding;                  /**< @Value 0x0000 @Desc 1 */
    
    S32 IE_inear_noise_alpha_fast;         /**< @Value 0x66666600 @Desc 1 */
    S32 IE_inear_noise_alpha_slow;         /**< @Value 0x7c28f600 @Desc 1 */
    S32 IE_inear_noise_alpha_ultra_slow;   /**< @Value 0x7f7cee00 @Desc 1 */
	S32 IE_inear_over_est;					/**< @Value 0x19999a00 @Desc 1 */
	
	S32 IE_main_nest_alpha;				    /**< @Value 1717986816 @Desc 1 */
    S32 IE_main_nest_alpha_1;				/**< @Value 429496576 @Desc 1 */
    S32 IE_main_nest_decay;				    /**< @Value 2147483392 @Desc 1 */
    S32 IE_main_nest_over_est;				/**< @Value 429496832 @Desc 1 */
    
	S32 IE_inear_nest_alpha;				/**< @Value 1717986816 @Desc 1 */
    S32 IE_inear_nest_alpha_1;				/**< @Value  429496576 @Desc 1 */
    S32 IE_inear_nest_decay;				/**< @Value 2147483392 @Desc 1 */
    S32 IE_inear_nest_over_est;			    /**< @Value  429496832 @Desc 1 */

    S16	IE_de_pop_gain_limiter;			    /**< @Value    3277 @Desc 1 */ 
    S16	IE_fe_vad_thr;			            /**< @Value 0x0000 @Desc 1 */ 
	
	S16 IE_PF_MIN;					/**< @Value 0x0400 @Desc 1 */
	S16 IE_DT_ratio_thrd;			/**< @Value 0x4000 @Desc 1 */
	S16 IE_DT_length;				/**< @Value 0x0016 @Desc 1 */
	S16 IE_FB_MIC_ATTEN;				/**< @Value 0x4000 @Desc 1 */
	S16 IE_MID_WIND_ATTEN;				/**< @Value 0x7FFF @Desc 1 */   
   
    S16 IE_h_1st_max        ;       /**< @Value 0x199A @Desc 1 */
    S16 IE_wind_frozon_han  ;       /**< @Value     30 @Desc 1 */
	S16 IE_inEear_input_gain;       /**< @Value 4096   @Desc 1 */	
	S32 IE_MSC_frozon_eng	;       /**< @Value 0x0147AE00 @Desc 1 */
	S32 IE_pitch_thr		;       /**< @Value 4096000 @Desc 1 */
    S32 IE_pitch_thr2		;       /**< @Value 2293760 @Desc 1 */
    S16 IE_pitch_idx_st		;       /**< @Value 0x0028 @Desc 1 */
    S16 IE_pitch_idx_end	;       /**< @Value 0x00C8 @Desc 1 */
    S16 IE_max_dis			;       /**< @Value 0x0007 @Desc 1 */
    S16 IE_h_max			;       /**< @Value 0x3333 @Desc 1 */
    S16 IE_h_min			;       /**< @Value 0x0500 @Desc 1 */

	//FB MIC reference Gain
	S16 IE_CH1_REF_GAIN;				/**< @Value 0x0148 @Desc 1 */
	S16 IE_CH2_REF_GAIN;				/**< @Value 0x028F @Desc 1 */
	S16 IE_CH3_REF_GAIN;				/**< @Value 0x028F @Desc 1 */
	S16 IE_CH4_REF_GAIN;				/**< @Value 0x028F @Desc 1 */
	//FB MIC HP COEF
	S32 VOICE_IE_HP_COEF_0;								/**< @Value 0x0076FE9F  @Desc 1 */
	S32 VOICE_IE_HP_COEF_1;								/**< @Value 0xFF890301  @Desc 1 */
	S32 VOICE_IE_HP_COEF_2;								/**< @Value 0x0076FE9E  @Desc 1 */
	S32 VOICE_IE_HP_COEF_3;								/**< @Value 0xFF856EC7  @Desc 1 */
	S32 VOICE_IE_HP_COEF_4;								/**< @Value 0x00758D42  @Desc 1 */
	S32 VOICE_IE_HP_COEF_5;								/**< @Value 0x007F5D7F  @Desc 1 */
	S32 VOICE_IE_HP_COEF_6;								/**< @Value 0xFF80AB5D  @Desc 1 */
	S32 VOICE_IE_HP_COEF_7;								/**< @Value 0x007F5D7E  @Desc 1 */
	S32 VOICE_IE_HP_COEF_8;								/**< @Value 0xFF80EB0D  @Desc 1 */
	S32 VOICE_IE_HP_COEF_9;								/**< @Value 0x007E5FE2  @Desc 1 */
	S32 VOICE_IE_HP_COEF_10;							/**< @Value 0x00400000	@Desc 1 */

	S32 IE_main_nest_over_est1;			/**< @Value 1717986816	@Desc 1 */
	S32 IE_inear_nest_over_est1;		/**< @Value 858993408	@Desc 1 */	
    S16 IE_pitch_bin_end    ;			/**< @Value 0x0050		@Desc 1 */
    S16 IE_pitch_enhance_end;			/**< @Value 0x0078		@Desc 1 */
    S16 IE_mix_w0;                      /**< @Value 0x7FFF		@Desc 1 */    
    S16 IE_mix_w1;                      /**< @Value 0x4000		@Desc 1 */
    S16 IE_mix_w2;                      /**< @Value 0x7333		@Desc 1 */
    S16 IE_Dummy1;                      /**< @Value 0x0000		@Desc 1 */
    
    S32 IE_main_nest_decay1;			/**< @Value 2147483392 @Desc 1 */
    S32 IE_inear_nest_decay1;			/**< @Value 2147483392 @Desc 1 */

	//TX REF VAD
	S16 AEC_ENG_VAD_THR;                /**< @Value 0x0a00		@Desc 1 */    
    S16 AEC_ENG_VAD_HANG;               /**< @Value 0x0005		@Desc 1 */
    S16 AEC_ENG_POW_INIT;               /**< @Value 0x0400		@Desc 1 */
    S16 AEC_slow_up_alpha;              /**< @Value 0x0021		@Desc 1 */
	S16 AEC_slow_dn_alpha;              /**< @Value 0x4000		@Desc 1 */
    S16 AEC_fast_up_alpha;              /**< @Value 0x6666		@Desc 1 */
    S16 AEC_fast_dn_alpha;              /**< @Value 0x0666		@Desc 1 */
	S16 IE_Dummy2;                      /**< @Value 0x0000		@Desc 1 */
   
    S32 IE_MSC_VAD_THR;                 /**< @Value  402653184 @Desc 1 */
    S16 IE_h_sub_max;                   /**< @Value      13107 @Desc 1 */
    S16 IE_h_1st_sub_max;               /**< @Value       6554 @Desc 1 */
                                       
    S16 IE_MIC_RATIO_THR;               /**< @Value       8192 @Desc 1 */
    S16 IE_BAND_RATIO_THR;              /**< @Value       2867 @Desc 1 */
    S16 IE_low_st;                      /**< @Value          3 @Desc 1 */
    S16 IE_high_st;                     /**< @Value         16 @Desc 1 */
    S16 IE_mic_ratio_alpha;             /**< @Value        164 @Desc 1 */
    S16 IE_band_ratio_alpha;            /**< @Value        164 @Desc 1 */
      
   //Reserve
	U32 RESERVE_1;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_2;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_3;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_4;																			/**< @Value 0x00000000 @Desc 1 */	
	U32 RESERVE_5;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_6;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_7;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_8;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_9;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_10;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_11;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_12;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_13;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_14;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_15;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_16;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_17;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_18;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_19;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_20;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_21;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_22;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_23;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_24;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_25;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_26;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_27;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_28;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_29;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_30;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_31;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_32;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_33;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_34;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_35;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_36;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_37;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_38;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_39;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_40;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_41;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_42;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_43;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_44;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_45;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_46;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_47;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_48;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_49;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_50;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_51;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_52;																		  /**< @Value 0x00000000 @Desc 1 */	
} PACKED DSP_PARA_AEC_NR_STRU;

#elif defined(MTK_DUALMIC_INEAR)

typedef struct stru_dsp_aec_nr_para_s
{
	U8  ENABLE;                                        	/**< @Value 0x01 @Desc 1 */
	U8  REVISION;                                      	/**< @Value 0x01 @Desc 1 */
	U16 AEC_NR_EN;		                               		/**< @Value 0x0015 @Desc b0: EC/NR switch, b1-2: 0(1-MIC), 1(1-MIC PD), 2(2-MIC), b3: 0(headset),1(speaker), b4: 0(inear off),1(inear on), b13: 0(disable Rx HPF), 1(enable Rx HPF), b15: 0(disable flash read) ,1(enable flash read)*/
	U16 AEC_CNG_GAIN_M;                                	/**< @Value 0x3333 @Desc 1 */
	U16 AEC_ref_pow_min;                               	/**< @Value 0x0001 @Desc 1 */
	U16 AEC_ECHO_TAIL_LENGTH;                          	/**< @Value 0x0006 @Desc 1 */
	U16 AEC_EC_RESIST;																	/**< @Value 0x0000 @Desc 1 */
	U16 AEC_MU_FAC;																			/**< @Value 0x399A @Desc 1 */
	U16 AEC_MU_MIN;																			/**< @Value 0x1000 @Desc 1 */
	U16 AEC_NORM_CAP1;																	/**< @Value 0x1FD7 @Desc 1 */
	U16 AEC_NORM_CAP2;																	/**< @Value 0x3000 @Desc 1 */
	U16 AEC_PF_MIN;																			/**< @Value 0x4000 @Desc 1 */
	S32 AEC_block_percent;															/**< @Value 0x00155555 @Desc 1 */
	U16 AEC_DT_boost;																		/**< @Value 0x0001 @Desc 1 */
	U16 AEC_PF_order;																		/**< @Value 0x0003 @Desc 1 */
	U16 AEC_DT_ratio_thrd;															/**< @Value 0x4000 @Desc 1 */
	U16 AEC_norm_tap;																		/**< @Value 0x0018 @Desc 1 */
	U16 AEC_DT_length;																	/**< @Value 0x0006 @Desc 1 */
	U16 MULT_AFTER_EC;																	/**< @Value 0x1000 @Desc 1 */
	S16 CH1_REF_GAIN;																		/**< @Value 0x0021 @Desc 1 */
	S16 CH2_REF_GAIN;																		/**< @Value 0x0021 @Desc 1 */
	S16 CH3_REF_GAIN;																		/**< @Value 0x0852 @Desc 1 */
	S16 CH4_REF_GAIN;																		/**< @Value 0x299A @Desc 1 */
	S16 CH1_REF2_GAIN;																	/**< @Value 0x0148 @Desc 1 */
	S16 CH2_REF2_GAIN;																	/**< @Value 0x028F @Desc 1 */
	S16 CH3_REF2_GAIN;																	/**< @Value 0x028F @Desc 1 */
	S16 CH4_REF2_GAIN;																	/**< @Value 0x028F @Desc 1 */

	U16 AVC_ALPHA;																			/**< @Value 0x799A @Desc 1 */
	U16 AVC_THRD;																				/**< @Value 0x0037 @Desc 1 */
	U16 AVC_VOL_MAX;  																	/**< @Value 0x7FFF @Desc 1 */
	U16 DSP_EC_SW;																			/**< @Value 0x0000 @Desc 1 */

  U16 WB_NR_TX_POW_MIN_BUF_PERIOD;										/**< @Value 0x000B @Desc 1 */
  U16 WB_NR_TX_NOISE_GAIN_LIMITER;   									/**< @Value 4078 @Desc 1 */
  U16 WB_NR_TX_VAD_THRD1;            									/**< @Value 0x0348 @Desc 1 */
  U16 WB_NR_TX_VAD_THRD_BANDS_0;    									/**< @Value 0x0B0B @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_1;    									/**< @Value 0x0B08 @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_2;    									/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_3;    									/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_4;    									/**< @Value 0x0A07 @Desc 1 */
  U16 WB_NR_TX_OVER_SUPPRESS_FAC;    									/**< @Value 0x219A @Desc 1 */
  U16 WB_NR_TX_SPEECH_RETAIN_FAC;    									/**< @Value 0x599A @Desc 1 */
  U16 WB_NR_TX_NOISE_LAMDA;          									/**< @Value 0x799A @Desc 1 */
  U16 WB_NR_TX_NOISE_FLOOR_MIN;      									/**< @Value 0x8400 @Desc 1 */
  U16 WB_NR_FAST_ALPHA;		          									/**< @Value 0x747B @Desc 1 */
  U16 WB_NR_SLOW_ALPHA;	            									/**< @Value 0x7C29 @Desc 1 */
  U16 WB_NR_NOISE_UPDATE_FAST;       									/**< @Value 6554	 @Desc 1 */
  U16 WB_NR_NOISE_UPDATE_SLOW;       									/**< @Value 983		 @Desc 1 */
  U16 WB_NR_NOISE_UPDATE_ULTRASLOW; 									/**< @Value 131		 @Desc 1 */
  U16 WB_NR_TX_EMPH_COEF_0;												/**< @Value 0xAE82 @Desc 1 */
	U16 WB_NR_TX_EMPH_COEF_1;											/**< @Value 0x3DD9 @Desc 1 */

  U16 NB_NR_RX_POW_MIN_BUF_PERIOD;  									/**< @Value 0x000D @Desc 1 */
  U16 NB_NR_RX_NOISE_GAIN_LIMITER;  									/**< @Value 0x1EB8 @Desc 1 */
  U16 NB_NR_RX_VAD_THRD1;           									/**< @Value 0x0348 @Desc 1 */
  U16 NB_NR_RX_VAD_THRD_BANDS_0;   										/**< @Value 0x0b0b @Desc 1 */
	U16 NB_NR_RX_VAD_THRD_BANDS_1;   										/**< @Value 0x0b08 @Desc 1 */
	U16 NB_NR_RX_VAD_THRD_BANDS_2;   										/**< @Value 0x0808 @Desc 1 */
	U16 NB_NR_RX_VAD_THRD_BANDS_3;   										/**< @Value 0x0807 @Desc 1 */
  U16 NB_NR_RX_OVER_SUPPRESS_FAC;   									/**< @Value 0x2666 @Desc 1 */
  U16 NB_NR_RX_SPEECH_RETAIN_FAC;   									/**< @Value 0x6000 @Desc 1 */
  U16 NB_NR_RX_NOISE_LAMDA;         									/**< @Value 0x799a @Desc 1 */
  U16 NB_NR_RX_NOISE_FLOOR_MIN;     									/**< @Value 0xae00 @Desc 1 */
  U16 NB_NR_RX_EMPH_COEF_0;														/**< @Value 0xC8C0 @Desc 1 */
	U16 NB_NR_RX_EMPH_COEF_1;														/**< @Value 0x24AF @Desc 1 */

	U16 WB_NR_RX_POW_MIN_BUF_PERIOD;										/**< @Value 0x000b @Desc 1 */
	U16 WB_NR_RX_NOISE_GAIN_LIMITER;										/**< @Value 0x1eb8 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD1;															/**< @Value 0x0348 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_0;											/**< @Value 0x0b0b @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_1;											/**< @Value 0x0b08 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_2;											/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_3;											/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_4;											/**< @Value 0x0a07 @Desc 1 */
	U16 WB_NR_RX_OVER_SUPPRESS_FAC;											/**< @Value 0x2800 @Desc 1 */
	U16 WB_NR_RX_SPEECH_RETAIN_FAC;											/**< @Value 0x6000 @Desc 1 */
	U16 WB_NR_RX_NOISE_LAMDA;														/**< @Value 0x799a @Desc 1 */
	U16 WB_NR_RX_NOISE_FLOOR_MIN;												/**< @Value 0xae00 @Desc 1 */
	U16 WB_NR_RX_EMPH_COEF_0;														/**< @Value 0xCA16 @Desc 1 */
	U16 WB_NR_RX_EMPH_COEF_1;														/**< @Value 0x0B29 @Desc 1 */

	U16 PD_NR_TX_OPT;																		/**< @Value      0 @Desc 1 */
	U16 PD_PEAK_BUF_SIZE;																/**< @Value      0 @Desc 1 */
	U16 PD_PITCH_VAD_THRD;															/**< @Value  20480 @Desc 1 */
	U16 PD_PITCH_FAC;																		/**< @Value  31949 @Desc 1 */
	U16 PD_PEAK_RATIO_FAC;															/**< @Value  30032 @Desc 1 */
	U16 PD_TRANSIENT_THRD;															/**< @Value 0x0046 @Desc 1 */
	U16 PD_TRANSIENT_AUG;																/**< @Value   2048 @Desc 1 */
	U16 PD_PITCH_AUG;																		/**< @Value    328 @Desc 1 */
	U16 PD_TRANSIENT_THRD2;															/**< @Value 0x0028 @Desc 1 */
	U16 PD_RESERVE2;																		/**< @Value      0 @Desc 1 */

	U16 M2_MICDIST;																			/**< @Value 0x0000 @Desc 1 */
	U16 M2_WIND_THRD;																		/**< @Value 0x1000 @Desc 1 */
	U16 M2_WIND_TRANS;																	/**< @Value 0x0640 @Desc 1 */
	U16 M2_WIND_BLOCK;																	/**< @Value 0x1000 @Desc 1 */
	U16 M2_FILTER_GRD;																	/**< @Value 0x0002 @Desc 1 */
	U16 M2_DISTANCE_FAC;																/**< @Value 0x6119 @Desc 1 */
	U16 M2_VAD_THRD_1;																	/**< @Value 0x1062 @Desc 1 */
	U16 M2_VAD_THRD_2;																	/**< @Value 0x0C4A @Desc 1 */
	U16 M2_VAD_THRD_3;																	/**< @Value 0x0852 @Desc 1 */
	U16 M2_VAD_THRD_4;																	/**< @Value 0x03D7 @Desc 1 */
	U16 M2_VAD_THRD_12;																	/**< @Value 0x1333 @Desc 1 */
	U16 M2_VAD_THRD_22;																	/**< @Value 0x0E14 @Desc 1 */
	U16 M2_VAD_THRD_32;																	/**< @Value 0x0666 @Desc 1 */
	U16 M2_VAD_THRD_42;																	/**< @Value 0x028F @Desc 1 */
	U16 M2_VAD_THRD_13;																	/**< @Value 0x2A3D @Desc 1 */
	U16 M2_VAD_THRD_23;																	/**< @Value 0x15C3 @Desc 1 */
	U16 M2_VAD_THRD_33;																	/**< @Value 0x0B85 @Desc 1 */
	U16 M2_VAD_THRD_43;																	/**< @Value 0x051F @Desc 1 */
	U16 M2_NORM_FAC1;																		/**< @Value 0x0000 @Desc 1 */
	U16 M2_PHASE_COMB;																	/**< @Value 0x5C02 @Desc 1 */
	U16 M2_PHASE_GAIN;																	/**< @Value 0x4000 @Desc 1 */
	U16 M2_NE_DURATION;																	/**< @Value 0x0038 @Desc 1 */

	U16 M2_BEAM1_NORM_LIMIT;														/**< @Value 0x6000 @Desc 1 */
	U16 M2_OVER_SUPPRESS_FAC2;													/**< @Value 0x2000 @Desc 1 */
	S16 M2_BAND1_ALPHA;																	/**< @Value 0x7800 @Desc 1 */
	S16 M2_BAND2_ALPHA;																	/**< @Value 0x7D71 @Desc 1 */
	S16 M2_BAND3_ALPHA;																	/**< @Value 0x7F80 @Desc 1 */
	S16 M2_BANDH_ALPHA;																	/**< @Value 0x7F5C @Desc 1 */
	
	S16 OFF_PITCH_ALPHA;																/**< @Value 0x0106 @Desc 1 */
	S16 WIND_DETECT_THRD;																/**< @Value 0x0014 @Desc 1 */
	S16 PD_WIND_FREQ_RANGE;															/**< @Value 0x0011 @Desc 1 */
	S16 PD_WIND_LOW_ATTEN;															/**< @Value 0x65AD @Desc 1 */
	S16 PD_NONSTATIONARY_ALPHA_1;												/**< @Value 0x0106 @Desc 1 */
	S16 PD_NONSTATIONARY_ALPHA_2;												/**< @Value 0x028F @Desc 1 */
	
	S16 M2_PD_HIGH_ATTEN_GOAL;													/**< @Value	13045		@Desc 1 */
	S32 M2_PD_SEVERE_NOISE_THRD;												/**< @Value 8388352 @Desc 1 */
  S16 NR_HIGH_ATTEN_DIVIDE;  													/**< @Value 109  		@Desc 1 */
  
  U16 WB_VOICE_TX_GAIN;																/**< @Value 0x0800 @Desc 1 */
  U16 NB_VOICE_TX_GAIN;																/**< @Value 0x0800 @Desc 1 */

	S32 VOICE_TX_HP_COEF_0;								              /**< @Value 6731724  @Desc 1 */
	S32 VOICE_TX_HP_COEF_1;								              /**< @Value -6725950 @Desc 1 */
	S32 VOICE_TX_HP_COEF_2;								              /**< @Value 6731723  @Desc 1 */
	S32 VOICE_TX_HP_COEF_3;								              /**< @Value -6986246 @Desc 1 */
	S32 VOICE_TX_HP_COEF_4;								              /**< @Value 5970844  @Desc 1 */
	S32 VOICE_TX_HP_COEF_5;								              /**< @Value 8329426  @Desc 1 */
	S32 VOICE_TX_HP_COEF_6;								              /**< @Value -8293150 @Desc 1 */
	S32 VOICE_TX_HP_COEF_7;								              /**< @Value 8329425  @Desc 1 */
	S32 VOICE_TX_HP_COEF_8;								              /**< @Value -8072154 @Desc 1 */
	S32 VOICE_TX_HP_COEF_9;								              /**< @Value 7972216  @Desc 1 */
	S32 VOICE_TX_HP_COEF_10;							              /**< @Value 4194304  @Desc 1 */

	S32 VOICE_WB_RX_HP_COEF_0;	                        /**< @Value 7798431  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_1;	                        /**< @Value -7798015 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_2;	                        /**< @Value 7798430  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_3;	                        /**< @Value -8032569 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_4;	                        /**< @Value 7703874  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_5;	                        /**< @Value 8347007  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_6;	                        /**< @Value -8344739 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_7;	                        /**< @Value 8347006  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_8;	                        /**< @Value -8328435 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_9;	                        /**< @Value 8282082  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_10;	                        /**< @Value 4194304  @Desc 1 */	
	
	S32 VOICE_NB_RX_HP_COEF_0;	                        /**< @Value 7398364  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_1;	                        /**< @Value -7396782 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_2;	                        /**< @Value 7398363  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_3;	                        /**< @Value -7679575 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_4;	                        /**< @Value 7075462  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_5;	                        /**< @Value 8373828  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_6;	                        /**< @Value -8364724 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_7;	                        /**< @Value 8373827  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_8;	                        /**< @Value -8255389 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_9;	                        /**< @Value 8177081  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_10;	                        /**< @Value 4194304  @Desc 1 */

	U16 AEC_Corel_Ratio1_Sensitive;											/**< @Value 0x0100	   @Desc 1 */
	S16 PD_VOICE_IN_WIND;                               /**< @Value 0x7FFF	   @Desc 1 */  
	S16 PD_VOICE_AMP_LEVEL;                             /**< @Value 0x4000	   @Desc 1 */  
	S16 M2_HF_NSG2_OVER;	                              /**< @Value 0x0000	   @Desc 1 */
	S16 M2_DF_RATIO;																		/**< @Value 0x6CCD	   @Desc 1 */
	S16 M2_POW1_ALPHA;																	/**< @Value 0x0CCD	   @Desc 1 */
	S16 M2_POW1_UP_GAIN;																/**< @Value 0x0028	   @Desc 1 */
	S16 M2_POW1_DOWN_GAIN;															/**< @Value 0x00A4	   @Desc 1 */
	S16 M2_POW1_THRD1;																	/**< @Value 0x30A4	   @Desc 1 */
	S16 M2_POW1_THRD2;																	/**< @Value 0x1333	   @Desc 1 */
	U16 M2_VAD3IN4;																			/**< @Value 0x0000	   @Desc 1 */
	S16 M2_POW1_VAD_RATIO;															/**< @Value 0x4000	   @Desc 1 */
	U16 M2_DEEMPH_COEF_1;																/**< @Value 0x5A3D	   @Desc 1 */
	U16 M2_DEEMPH_COEF_2;																/**< @Value 0x6536	   @Desc 1 */
	U16 M2_DEEMPH_COEF_3;																/**< @Value 0xD3A9	   @Desc 1 */
	S16 M2_BAND2_GAIN;																	/**< @Value 0x2D3F	   @Desc 1 */
	S16 M2_BAND3_GAIN;																	/**< @Value 0x4000	   @Desc 1 */
	S16 M2_BANDH_GAIN;																	/**< @Value 0x2000	   @Desc 1 */
	U16 AEC_Corel_Ratio4_Sensitive; 										/**< @Value 0x0100     @Desc 1 */
	S16 AEC_Ch4_Percentage;															/**< @Value 0x0000     @Desc 1 */
	U16 RXIN_TXREF_DELAY;																/**< @Value 0x0005	   @Desc 1 */
	U16 MIC_EC_DELAY	;																	/**< @Value 0x003C	   @Desc 1 */
	U16 AEC_REF_GAIN_AUTO;															/**< @Value 0x0000     @Desc 1 */
	
	S16 M2_REF_MIC_CH1_GAIN;														/**< @Value 0x2000     @Desc 1 */
	S16 M2_REF_MIC_CH2_GAIN;														/**< @Value 0x2000     @Desc 1 */
	S16 M2_REF_MIC_CH3_GAIN;														/**< @Value 0x2000     @Desc 1 */
	S16 M2_REF_MIC_CH4_GAIN;														/**< @Value 0x2000     @Desc 1 */
	
	S16 Mel_scale_band_num; 														/**< @Value 0x0000     @Desc 1 */
	S16 Harmonics_thrd;																	/**< @Value 0x0000     @Desc 1 */
	
	S16 AEC_tap_length;					/**< @Value 0x755F     @Desc 1 */
	
	//Inear parameter
	S16 IE_main_type;                    /**< @Value 0x0002 @Desc 1 */
    S16 IE_main_alpha;                   /**< @Value 0x7D71 @Desc 1 */
    S16 IE_main_alpha_1;                 /**< @Value 0x028E @Desc 1 */
    S16 IE_main_gain_limiter;            /**< @Value 0x0519 @Desc 1 */
                                        
    S16 IE_inear_type;                   /**< @Value 0x0002 @Desc 1 */
    S16 IE_inear_alpha;                  /**< @Value 0x028E @Desc 1 */
    S16 IE_inear_alpha_1;                /**< @Value 0x0665 @Desc 1 */
    S16 IE_inear_gain_limiter;		     /**< @Value 3276   @Desc 1 wenchen */
    
	S16 IE_ENG_VAD_THR;					/**< @Value 0x0C00 @Desc 1 wenchen */	
    S16 IE_ENG_VAD_HANG;				/**< @Value 0x0009 @Desc 1 wenchen */
    S16 IE_ENG_POW_INIT;				/**< @Value 0xe200 @Desc 1 wenchen */
    S16 IE_slow_up_alpha;				/**< @Value 0x0021 @Desc 1 wenchen */
    S16 IE_slow_dn_alpha;				/**< @Value 0x4000 @Desc 1 wenchen */
    S16 IE_fast_up_alpha;				/**< @Value 0x4000 @Desc 1 wenchen */
    S16 IE_fast_dn_alpha;				/**< @Value 0x028f @Desc 1 wenchen */
    S16 IE_end_inear_wind;				/**< @Value 0x0054 @Desc 1 wenchen */
	
	S32 IE_msc_alpha;					/**< @Value 0x40000000 @Desc 1 wenchen */
    S32 IE_msc_sm_up_alpha;				/**< @Value 0x00624E00 @Desc 1 */
    S32 IE_msc_sm_dn_alpha;				/**< @Value 0x6CCCCCCD @Desc 1 wenchen */
    S32 IE_msc_bias;					/**< @Value 0x00040000 @Desc 1 */
    S32 IE_MSC_WIND_THR2;				/**< @Value 0x00200000 @Desc 1 wenchen */
    S32 IE_MSC_WIND_THR;				/**< @Value 0x00100000 @Desc 1 wenchen */
                                       
    S16 IE_mid_eq;                       /**< @Value 0x1000 @Desc 1 wenchen */
    S16 IE_end_inear_est;                /**< @Value 0x0030 @Desc 1 wenchen */
    S16 IE_mid_eq_st;                    /**< @Value 0x000f @Desc 1 wenchen */
    S16 IE_inEar_nor_gain0;              /**< @Value 11627 @Desc 1 */
    S16 IE_inEar_nor_gain1;              /**< @Value 11627 @Desc 1 */
	S16 IE_ff_mel_filter;					/**< @Value 0x0002 @Desc 0: disable melfilter, 1: enable melfilter 26 bands, 2: enable melfilter 64 bands */
    S16 IE_fb_mel_filter;					/**< @Value 0x0000 @Desc 0: disable melfilter, 1: enable melfilter 26 bands, 2: enable melfilter 64 bands */
    S16 IE_en_pre_emp;						/**< @Value 0x06e2 @Desc wenchen bit1: enable low band noise est, bit2: enable low band phase compensation, bit3: low band phase inverse, bit5:use ast eq, bit6:pitch enhancement, bit7: fevad_on switch,  bit9: fail safe, bit10: inear_VAD used high band suppression, bit11: degrade detection, bit14: calibration fb mic, bit15: calibration ff mic */
	S16	IE_vad_credit_thr;					/**< @Value 0x0800 @Desc 1 */
	S16 IE_padding2;						/**< @Value 0x0000 @Desc 1 */	
	S16 IE_main_alpha_peak_up;             /**< @Value 0x747b @Desc 1 */
    S16 IE_main_alpha_peak_dn;             /**< @Value 0x7c29 @Desc 1 */
    S16 IE_main_alpha_valley_dn;           /**< @Value 0x747b @Desc 1 */
    S16 IE_main_alpha_valley_up;           /**< @Value 0x7c29 @Desc 1 */
    S16 IE_main_pow_diff_thr;              /**< @Value 0x0348 @Desc 1 */
    S16 IE_main_pow_credit;                /**< @Value 0x03e8 @Desc 1 */
    S16 IE_main_alpha_sm_fast;             /**< @Value 0x4ccd @Desc 1 */
    S16 IE_main_alpha_sm_slow;             /**< @Value 0x6ccd @Desc 1 */
    S16 IE_main_noise_update_thr0;         /**< @Value 0x0a80 @Desc 1 */
    S16 IE_main_noise_update_thr1;         /**< @Value 0x0780 @Desc 1 */
    S16 IE_main_noise_update_thr2;         /**< @Value 0x0300 @Desc 1 */
    S16 IE_main_padding;                   /**< @Value 0x0000 @Desc 1 */
   
    S32 IE_main_noise_alpha_fast;          /**< @Value 0x66666600 @Desc 1 */
    S32 IE_main_noise_alpha_slow;          /**< @Value 0x7c28f600 @Desc 1 */
    S32 IE_main_noise_alpha_ultra_slow;    /**< @Value 0x7f7cee00 @Desc 1 */
	S32 IE_main_over_est;					/**< @Value 0x19999a00 @Desc 1 */
	                                        
    S16 IE_inear_alpha_peak_up;            /**< @Value 0x747b @Desc 1 */
    S16 IE_inear_alpha_peak_dn;            /**< @Value 0x7c29 @Desc 1 */
    S16 IE_inear_alpha_valley_dn;          /**< @Value 0x747b @Desc 1 */
    S16 IE_inear_alpha_valley_up;          /**< @Value 0x7c29 @Desc 1 */
    S16 IE_inear_pow_diff_thr;             /**< @Value 0x0348 @Desc 1 */
    S16 IE_inear_pow_credit;               /**< @Value 0x03e8 @Desc 1 */
    S16 IE_inear_alpha_sm_fast;            /**< @Value 0x4ccd @Desc 1 */
    S16 IE_inear_alpha_sm_slow;            /**< @Value 0x6ccd @Desc 1 */
    S16 IE_inear_noise_update_thr0;        /**< @Value 0x0a80 @Desc 1 */
    S16 IE_inear_noise_update_thr1;        /**< @Value 0x0780 @Desc 1 */
    S16 IE_inear_noise_update_thr2;        /**< @Value 0x0300 @Desc 1 */
    S16 IE_inear_padding;                  /**< @Value 0x0000 @Desc 1 */
    
    S32 IE_inear_noise_alpha_fast;         /**< @Value 0x66666600 @Desc 1 */
    S32 IE_inear_noise_alpha_slow;         /**< @Value 0x7c28f600 @Desc 1 */
    S32 IE_inear_noise_alpha_ultra_slow;   /**< @Value 0x7f7cee00 @Desc 1 */
	S32 IE_inear_over_est;				   /**< @Value 0x06660000 @Desc 1  wenchen */
	
	S32 IE_main_nest_alpha;				    /**< @Value 1503238656 @Desc 1 */
    S32 IE_main_nest_alpha_1;				/**< @Value 644244736 @Desc 1 */
    S32 IE_main_nest_decay;				    /**< @Value 2040109568 @Desc 1 */
    S32 IE_main_nest_over_est;				/**< @Value 429496832 @Desc 1 */    
	S32 IE_inear_nest_alpha;				/**< @Value 0x66660000 @Desc 1 wenchen */
    S32 IE_inear_nest_alpha_1;				/**< @Value 429496576 @Desc 1 */
    S32 IE_inear_nest_decay;				/**< @Value 0x73330000 @Desc 1 wenchen */
    S32 IE_inear_nest_over_est;			    /**< @Value 0x199a0000 @Desc 1 wenchen */
	
    S16	IE_de_pop_gain_limiter;			    /**< @Value    3277 @Desc 1 */ 
    S16	IE_fe_vad_thr;			            /**< @Value 0x0000 @Desc 1 */ 
	
	S16 IE_PF_MIN;					/**< @Value 0x0400 @Desc 1 */
	S16 IE_DT_ratio_thrd;			/**< @Value 0x4000 @Desc 1 */
	S16 IE_DT_length;				/**< @Value 0x0016 @Desc 1 */
	S16 IE_FB_MIC_ATTEN;				/**< @Value 0x4000 @Desc 1 */
	S16 IE_MID_WIND_ATTEN;				/**< @Value 0x7FFF @Desc 1 */   
   
    S16 IE_h_1st_max        ;       /**< @Value 0x199A @Desc 1 */
    S16 IE_wind_frozon_han  ;       /**< @Value 0x000A @Desc 1 */
	S16 IE_inEear_input_gain;       /**< @Value 2048   @Desc 1 */	
	S32 IE_MSC_frozon_eng	;       /**< @Value 0x00000078 @Desc 1 wenchen */
	S32 IE_pitch_thr		;       /**< @Value 0x0007A120 @Desc 1 */
    S32 IE_pitch_thr2		;       /**< @Value 0x00026480 @Desc 1 */
    S16 IE_pitch_idx_st		;       /**< @Value 0x0028 @Desc 1 */
    S16 IE_pitch_idx_end	;       /**< @Value 0x00C8 @Desc 1 */
    S16 IE_max_dis			;       /**< @Value 0x000A @Desc 1 */
    S16 IE_h_max			;       /**< @Value 0x3333 @Desc 1 */
    S16 IE_h_min			;       /**< @Value 0x0500 @Desc 1 */

	//FB MIC reference Gain
	S16 IE_CH1_REF_GAIN;				/**< @Value 0x0148 @Desc 1 */
	S16 IE_CH2_REF_GAIN;				/**< @Value 0x028F @Desc 1 */
	S16 IE_CH3_REF_GAIN;				/**< @Value 0x028F @Desc 1 */
	S16 IE_CH4_REF_GAIN;				/**< @Value 0x028F @Desc 1 */
	//FB MIC HP COEF
	S32 VOICE_IE_HP_COEF_0;								/**< @Value 0x0076FE9F  @Desc 1 */
	S32 VOICE_IE_HP_COEF_1;								/**< @Value 0xFF890301  @Desc 1 */
	S32 VOICE_IE_HP_COEF_2;								/**< @Value 0x0076FE9E  @Desc 1 */
	S32 VOICE_IE_HP_COEF_3;								/**< @Value 0xFF856EC7  @Desc 1 */
	S32 VOICE_IE_HP_COEF_4;								/**< @Value 0x00758D42  @Desc 1 */
	S32 VOICE_IE_HP_COEF_5;								/**< @Value 0x007F5D7F  @Desc 1 */
	S32 VOICE_IE_HP_COEF_6;								/**< @Value 0xFF80AB5D  @Desc 1 */
	S32 VOICE_IE_HP_COEF_7;								/**< @Value 0x007F5D7E  @Desc 1 */
	S32 VOICE_IE_HP_COEF_8;								/**< @Value 0xFF80EB0D  @Desc 1 */
	S32 VOICE_IE_HP_COEF_9;								/**< @Value 0x007E5FE2  @Desc 1 */
	S32 VOICE_IE_HP_COEF_10;							/**< @Value 0x00400000	@Desc 1 */

	S32 IE_main_nest_over_est1;			/**< @Value 1717986816	@Desc 1 */
	S32 IE_inear_nest_over_est1;		/**< @Value 858993408	@Desc 1 */	
    S16 IE_pitch_bin_end    ;			/**< @Value 0x0050		@Desc 1 */
    S16 IE_pitch_enhance_end;			/**< @Value 0x0078		@Desc 1 */
    S16 IE_mix_w0;                      /**< @Value 0x7FFF		@Desc 1 */    
    S16 IE_mix_w1;                      /**< @Value 0x4000		@Desc 1 */
    S16 IE_mix_w2;                      /**< @Value 0x7333		@Desc 1 */
    S16 IE_Dummy1;                      /**< @Value 0x0000		@Desc 1 */
   
    S32 IE_main_nest_decay1;			/**< @Value 2147483392 @Desc 1 */
    S32 IE_inear_nest_decay1;			/**< @Value 2147483392 @Desc 1 */

   //TX REF VAD
	S16 AEC_ENG_VAD_THR;                  /**< @Value 0x0a00		@Desc 1 */    
    S16 AEC_ENG_VAD_HANG;                  /**< @Value 0x0005		@Desc 1 */
    S16 AEC_ENG_POW_INIT;                  /**< @Value 0x0400		@Desc 1 */
    S16 AEC_slow_up_alpha;                  /**< @Value 0x0021		@Desc 1 */
	S16 AEC_slow_dn_alpha;                  /**< @Value 0x4000		@Desc 1 */
    S16 AEC_fast_up_alpha;                  /**< @Value 0x6666		@Desc 1 */
    S16 AEC_fast_dn_alpha;                  /**< @Value 0x0666		@Desc 1 */
	S16 IE_Dummy2;                      /**< @Value 0x0000		@Desc 1 */
   
    S32 IE_MSC_VAD_THR;                 /**< @Value  402653184 @Desc 1 */
    S16 IE_h_sub_max;                   /**< @Value      13107 @Desc 1 */
    S16 IE_h_1st_sub_max;               /**< @Value       6554 @Desc 1 */
                                       
    S16 IE_MIC_RATIO_THR;               /**< @Value       8192 @Desc 1 */
    S16 IE_BAND_RATIO_THR;              /**< @Value       1229 @Desc 1 wenchen */
    S16 IE_low_st;                      /**< @Value          1 @Desc 1 wenchen */
    S16 IE_high_st;                     /**< @Value         16 @Desc 1 wenchen */
    S16 IE_mic_ratio_alpha;             /**< @Value         80 @Desc 1 */
    S16 IE_band_ratio_alpha;            /**< @Value        164 @Desc 1 wenchen */

	S16 SBEC_PF_order_12;               /**< @Value    0x0028 @Desc high-byte = 0(sbec_sw off), !=0(sbec_sw on) */
	S16 SBEC_PF_order_34;               /**< @Value    0x08ff @Desc 1 */
	S16 IE_SBEC_PF_order_12;            /**< @Value    0x2828 @Desc 1 */
	S16 IE_SBEC_PF_order_34;            /**< @Value    0x28ff @Desc 1 */
                                                              
	S16 SBEC_DT_ratio_thrd_12;          /**< @Value    0x1e32 @Desc 1 */
	S16 SBEC_DT_ratio_thrd_34;          /**< @Value    0x3232 @Desc 1 */
	S16 IE_SBEC_DT_ratio_thrd_12;       /**< @Value    0x3c32 @Desc 1 */
	S16 IE_SBEC_DT_ratio_thrd_34;       /**< @Value    0x3232 @Desc 1 */
                                                              
	S16 SBEC_PF_MIN_12;                 /**< @Value    0x5A40 @Desc 1 */
	S16 SBEC_PF_MIN_34;                 /**< @Value    0x405A @Desc 1 */
	S16 IE_SBEC_PF_MIN_12;              /**< @Value    0x5A5A @Desc 1 */
	S16 IE_SBEC_PF_MIN_34;              /**< @Value    0x5A5A @Desc 1 */
	S16 SBEC_noise_paste_gain;		    /**< @Value     32767 @Desc 1 */

	//Reserve
	U16 RESERVE_1;																			/**< @Value     0x0000 @Desc 1 */
	U32 RESERVE_2;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_3;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_4;																			/**< @Value 0x00000000 @Desc 1 */	
	U32 RESERVE_5;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_6;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_7;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_8;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_9;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_10;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_11;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_12;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_13;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_14;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_15;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_16;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_17;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_18;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_19;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_20;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_21;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_22;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_23;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_24;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_25;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_26;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_27;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_28;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_29;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_30;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_31;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_32;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_33;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_34;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_35;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_36;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_37;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_38;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_39;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_40;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_41;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_42;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_43;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_44;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_45;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_46;																		  /**< @Value 0x00000000 @Desc 1 */
										
} PACKED DSP_PARA_AEC_NR_STRU;

#elif defined(MTK_3RD_PARTY_NR)

typedef struct stru_dsp_aec_nr_para_s
{
	U8  ENABLE;                                        	/**< @Value 0x01 @Desc 1 */
	U8  REVISION;                                      	/**< @Value 0x01 @Desc 1 */
	U16 AEC_NR_EN;		                               		/**< @Value 0x0115 @Desc b0: EC/NR switch, b1-2: 0(1-MIC), 1(1-MIC PD), 2(2-MIC), b3: 0(headset),1(speaker), b4: 0(inear off),1(inear on), b8: NC output for ext NR, b13: 0(disable Rx HPF), 1(enable Rx HPF), b15: 0(disable flash read) ,1(enable flash read)*/
	U16 AEC_CNG_GAIN_M;                                	/**< @Value 0x3333 @Desc 1 */
	U16 AEC_ref_pow_min;                               	/**< @Value 0x0001 @Desc 1 */
	U16 AEC_ECHO_TAIL_LENGTH;                          	/**< @Value 0x0006 @Desc 1 */
	U16 AEC_EC_RESIST;																	/**< @Value 0x0000 @Desc 1 */
	U16 AEC_MU_FAC;																			/**< @Value 0x399A @Desc 1 */
	U16 AEC_MU_MIN;																			/**< @Value 0x1000 @Desc 1 */
	U16 AEC_NORM_CAP1;																	/**< @Value 0x1FD7 @Desc 1 */
	U16 AEC_NORM_CAP2;																	/**< @Value 0x3000 @Desc 1 */
	U16 AEC_PF_MIN;																			/**< @Value 0x0400 @Desc 1 */
	S32 AEC_block_percent;															/**< @Value 0x00155555 @Desc 1 */
	U16 AEC_DT_boost;																		/**< @Value 0x0001 @Desc 1 */
	U16 AEC_PF_order;																		/**< @Value 0x0003 @Desc 1 */
	U16 AEC_DT_ratio_thrd;															/**< @Value 0x4000 @Desc 1 */
	U16 AEC_norm_tap;																		/**< @Value 0x0018 @Desc 1 */
	U16 AEC_DT_length;																	/**< @Value 0x0006 @Desc 1 */
	U16 MULT_AFTER_EC;																	/**< @Value 0x1000 @Desc 1 */
	S16 CH1_REF_GAIN;																		/**< @Value 0x00A0 @Desc 1 */
	S16 CH2_REF_GAIN;																		/**< @Value 0x00A0 @Desc 1 */
	S16 CH3_REF_GAIN;																		/**< @Value 0x00A0 @Desc 1 */
	S16 CH4_REF_GAIN;																		/**< @Value 0x00A0 @Desc 1 */
	S16 CH1_REF2_GAIN;																	/**< @Value 0x00A0 @Desc 1 */
	S16 CH2_REF2_GAIN;																	/**< @Value 0x00A0 @Desc 1 */
	S16 CH3_REF2_GAIN;																	/**< @Value 0x00A0 @Desc 1 */
	S16 CH4_REF2_GAIN;																	/**< @Value 0x00A0 @Desc 1 */

	U16 AVC_ALPHA;																			/**< @Value 0x799A @Desc 1 */
	U16 AVC_THRD;																				/**< @Value 0x0037 @Desc 1 */
	U16 AVC_VOL_MAX;  																	/**< @Value 0x7FFF @Desc 1 */
	U16 DSP_EC_SW;																			/**< @Value 0x0000 @Desc 1 */

  U16 WB_NR_TX_POW_MIN_BUF_PERIOD;										/**< @Value 0x000B @Desc 1 */
  U16 WB_NR_TX_NOISE_GAIN_LIMITER;   									/**< @Value 0x0FEE @Desc 1 */
  U16 WB_NR_TX_VAD_THRD1;            									/**< @Value 0x0348 @Desc 1 */
  U16 WB_NR_TX_VAD_THRD_BANDS_0;    									/**< @Value 0x0B0B @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_1;    									/**< @Value 0x0B08 @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_2;    									/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_3;    									/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_4;    									/**< @Value 0x0A07 @Desc 1 */
  U16 WB_NR_TX_OVER_SUPPRESS_FAC;    									/**< @Value 0x219A @Desc 1 */
  U16 WB_NR_TX_SPEECH_RETAIN_FAC;    									/**< @Value 0x599A @Desc 1 */
  U16 WB_NR_TX_NOISE_LAMDA;          									/**< @Value 0x799A @Desc 1 */
  U16 WB_NR_TX_NOISE_FLOOR_MIN;      									/**< @Value 0x8400 @Desc 1 */
  U16 WB_NR_FAST_ALPHA;		          									/**< @Value 0x747B @Desc 1 */
  U16 WB_NR_SLOW_ALPHA;	            									/**< @Value 0x7C29 @Desc 1 */
  U16 WB_NR_NOISE_UPDATE_FAST;       									/**< @Value 6554	 @Desc 1 */
  U16 WB_NR_NOISE_UPDATE_SLOW;       									/**< @Value 983		 @Desc 1 */
  U16 WB_NR_NOISE_UPDATE_ULTRASLOW; 									/**< @Value 131		 @Desc 1 */
  U16 WB_NR_TX_EMPH_COEF_0;														/**< @Value 0xAE82 @Desc 1 */
	U16 WB_NR_TX_EMPH_COEF_1;														/**< @Value 0x3DD9 @Desc 1 */

  U16 NB_NR_RX_POW_MIN_BUF_PERIOD;  									/**< @Value 0x000D @Desc 1 */
  U16 NB_NR_RX_NOISE_GAIN_LIMITER;  									/**< @Value 0x1EB8 @Desc 1 */
  U16 NB_NR_RX_VAD_THRD1;           									/**< @Value 0x0348 @Desc 1 */
  U16 NB_NR_RX_VAD_THRD_BANDS_0;   										/**< @Value 0x0b0b @Desc 1 */
	U16 NB_NR_RX_VAD_THRD_BANDS_1;   										/**< @Value 0x0b08 @Desc 1 */
	U16 NB_NR_RX_VAD_THRD_BANDS_2;   										/**< @Value 0x0808 @Desc 1 */
	U16 NB_NR_RX_VAD_THRD_BANDS_3;   										/**< @Value 0x0807 @Desc 1 */
  U16 NB_NR_RX_OVER_SUPPRESS_FAC;   									/**< @Value 0x2666 @Desc 1 */
  U16 NB_NR_RX_SPEECH_RETAIN_FAC;   									/**< @Value 0x6000 @Desc 1 */
  U16 NB_NR_RX_NOISE_LAMDA;         									/**< @Value 0x799a @Desc 1 */
  U16 NB_NR_RX_NOISE_FLOOR_MIN;     									/**< @Value 0xae00 @Desc 1 */
  U16 NB_NR_RX_EMPH_COEF_0;														/**< @Value 0xC8C0 @Desc 1 */
	U16 NB_NR_RX_EMPH_COEF_1;														/**< @Value 0x24AF @Desc 1 */

	U16 WB_NR_RX_POW_MIN_BUF_PERIOD;										/**< @Value 0x000b @Desc 1 */
	U16 WB_NR_RX_NOISE_GAIN_LIMITER;										/**< @Value 0x1eb8 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD1;															/**< @Value 0x0348 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_0;											/**< @Value 0x0b0b @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_1;											/**< @Value 0x0b08 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_2;											/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_3;											/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_4;											/**< @Value 0x0a07 @Desc 1 */
	U16 WB_NR_RX_OVER_SUPPRESS_FAC;											/**< @Value 0x2800 @Desc 1 */
	U16 WB_NR_RX_SPEECH_RETAIN_FAC;											/**< @Value 0x6000 @Desc 1 */
	U16 WB_NR_RX_NOISE_LAMDA;														/**< @Value 0x799a @Desc 1 */
	U16 WB_NR_RX_NOISE_FLOOR_MIN;												/**< @Value 0xae00 @Desc 1 */
	U16 WB_NR_RX_EMPH_COEF_0;														/**< @Value 0xCA16 @Desc 1 */
	U16 WB_NR_RX_EMPH_COEF_1;														/**< @Value 0x0B29 @Desc 1 */

	U16 PD_NR_TX_OPT;																		/**< @Value      0 @Desc 1 */
	U16 PD_PEAK_BUF_SIZE;																/**< @Value      0 @Desc 1 */
	U16 PD_PITCH_VAD_THRD;															/**< @Value  20480 @Desc 1 */
	U16 PD_PITCH_FAC;																		/**< @Value  31949 @Desc 1 */
	U16 PD_PEAK_RATIO_FAC;															/**< @Value  30032 @Desc 1 */
	U16 PD_TRANSIENT_THRD;															/**< @Value 0x0046 @Desc 1 */
	U16 PD_TRANSIENT_AUG;																/**< @Value   2048 @Desc 1 */
	U16 PD_PITCH_AUG;																		/**< @Value    328 @Desc 1 */
	U16 PD_TRANSIENT_THRD2;															/**< @Value 0x0028 @Desc 1 */
	U16 PD_RESERVE2;																		/**< @Value      0 @Desc 1 */

	U16 M2_MICDIST;																			/**< @Value 0x0000 @Desc 1 */
	U16 M2_WIND_THRD;																		/**< @Value 0x1000 @Desc 1 */
	U16 M2_WIND_TRANS;																	/**< @Value 0x0640 @Desc 1 */
	U16 M2_WIND_BLOCK;																	/**< @Value 0x1000 @Desc 1 */
	U16 M2_FILTER_GRD;																	/**< @Value 0x0002 @Desc 1 */
	U16 M2_DISTANCE_FAC;																/**< @Value 0x6119 @Desc 1 */
	U16 M2_VAD_THRD_1;																	/**< @Value 0x1062 @Desc 1 */
	U16 M2_VAD_THRD_2;																	/**< @Value 0x0C4A @Desc 1 */
	U16 M2_VAD_THRD_3;																	/**< @Value 0x0852 @Desc 1 */
	U16 M2_VAD_THRD_4;																	/**< @Value 0x03D7 @Desc 1 */
	U16 M2_VAD_THRD_12;																	/**< @Value 0x1333 @Desc 1 */
	U16 M2_VAD_THRD_22;																	/**< @Value 0x0E14 @Desc 1 */
	U16 M2_VAD_THRD_32;																	/**< @Value 0x0666 @Desc 1 */
	U16 M2_VAD_THRD_42;																	/**< @Value 0x028F @Desc 1 */
	U16 M2_VAD_THRD_13;																	/**< @Value 0x2A3D @Desc 1 */
	U16 M2_VAD_THRD_23;																	/**< @Value 0x15C3 @Desc 1 */
	U16 M2_VAD_THRD_33;																	/**< @Value 0x0B85 @Desc 1 */
	U16 M2_VAD_THRD_43;																	/**< @Value 0x051F @Desc 1 */
	U16 M2_NORM_FAC1;																		/**< @Value 0x0000 @Desc 1 */
	U16 M2_PHASE_COMB;																	/**< @Value 0x5C02 @Desc 1 */
	U16 M2_PHASE_GAIN;																	/**< @Value 0x4000 @Desc 1 */
	U16 M2_NE_DURATION;																	/**< @Value 0x0038 @Desc 1 */

	U16 M2_BEAM1_NORM_LIMIT;														/**< @Value 0x6000 @Desc 1 */
	U16 M2_OVER_SUPPRESS_FAC2;													/**< @Value 0x2000 @Desc 1 */
	S16 M2_BAND1_ALPHA;																	/**< @Value 0x7800 @Desc 1 */
	S16 M2_BAND2_ALPHA;																	/**< @Value 0x7D71 @Desc 1 */
	S16 M2_BAND3_ALPHA;																	/**< @Value 0x7F80 @Desc 1 */
	S16 M2_BANDH_ALPHA;																	/**< @Value 0x7F5C @Desc 1 */
	
	S16 OFF_PITCH_ALPHA;																/**< @Value 0x0106 @Desc 1 */
	S16 WIND_DETECT_THRD;																/**< @Value 0x0014 @Desc 1 */
	S16 PD_WIND_FREQ_RANGE;															/**< @Value 0x0011 @Desc 1 */
	S16 PD_WIND_LOW_ATTEN;															/**< @Value 0x65AD @Desc 1 */
	S16 PD_NONSTATIONARY_ALPHA_1;												/**< @Value 0x0106 @Desc 1 */
	S16 PD_NONSTATIONARY_ALPHA_2;												/**< @Value 0x028F @Desc 1 */
	
	S16 M2_PD_HIGH_ATTEN_GOAL;													/**< @Value	13045		@Desc 1 */
	S32 M2_PD_SEVERE_NOISE_THRD;												/**< @Value 8388352 @Desc 1 */
  S16 NR_HIGH_ATTEN_DIVIDE;  													/**< @Value 109  		@Desc 1 */
  
  U16 WB_VOICE_TX_GAIN;																/**< @Value 0x0800 @Desc 1 */
  U16 NB_VOICE_TX_GAIN;																/**< @Value 0x0800 @Desc 1 */

	S32 VOICE_TX_HP_COEF_0;								              /**< @Value 6731724  @Desc 1 */
	S32 VOICE_TX_HP_COEF_1;								              /**< @Value -6725950 @Desc 1 */
	S32 VOICE_TX_HP_COEF_2;								              /**< @Value 6731723  @Desc 1 */
	S32 VOICE_TX_HP_COEF_3;								              /**< @Value -6986246 @Desc 1 */
	S32 VOICE_TX_HP_COEF_4;								              /**< @Value 5970844  @Desc 1 */
	S32 VOICE_TX_HP_COEF_5;								              /**< @Value 8329426  @Desc 1 */
	S32 VOICE_TX_HP_COEF_6;								              /**< @Value -8293150 @Desc 1 */
	S32 VOICE_TX_HP_COEF_7;								              /**< @Value 8329425  @Desc 1 */
	S32 VOICE_TX_HP_COEF_8;								              /**< @Value -8072154 @Desc 1 */
	S32 VOICE_TX_HP_COEF_9;								              /**< @Value 7972216  @Desc 1 */
	S32 VOICE_TX_HP_COEF_10;							              /**< @Value 4194304  @Desc 1 */

	S32 VOICE_WB_RX_HP_COEF_0;	                        /**< @Value 7798431  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_1;	                        /**< @Value -7798015 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_2;	                        /**< @Value 7798430  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_3;	                        /**< @Value -8032569 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_4;	                        /**< @Value 7703874  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_5;	                        /**< @Value 8347007  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_6;	                        /**< @Value -8344739 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_7;	                        /**< @Value 8347006  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_8;	                        /**< @Value -8328435 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_9;	                        /**< @Value 8282082  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_10;	                        /**< @Value 4194304  @Desc 1 */	
	
	S32 VOICE_NB_RX_HP_COEF_0;	                        /**< @Value 7398364  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_1;	                        /**< @Value -7396782 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_2;	                        /**< @Value 7398363  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_3;	                        /**< @Value -7679575 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_4;	                        /**< @Value 7075462  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_5;	                        /**< @Value 8373828  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_6;	                        /**< @Value -8364724 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_7;	                        /**< @Value 8373827  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_8;	                        /**< @Value -8255389 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_9;	                        /**< @Value 8177081  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_10;	                        /**< @Value 4194304  @Desc 1 */

	U16 AEC_Corel_Ratio1_Sensitive;											/**< @Value 0x0100	   @Desc 1 */
	S16 PD_VOICE_IN_WIND;                               /**< @Value 0x7FFF	   @Desc 1 */  
	S16 PD_VOICE_AMP_LEVEL;                             /**< @Value 0x4000	   @Desc 1 */  
	S16 M2_HF_NSG2_OVER;	                              /**< @Value 0x0000	   @Desc 1 */
	S16 M2_DF_RATIO;																		/**< @Value 0x6CCD	   @Desc 1 */
	S16 M2_POW1_ALPHA;																	/**< @Value 0x0CCD	   @Desc 1 */
	S16 M2_POW1_UP_GAIN;																/**< @Value 0x0028	   @Desc 1 */
	S16 M2_POW1_DOWN_GAIN;															/**< @Value 0x00A4	   @Desc 1 */
	S16 M2_POW1_THRD1;																	/**< @Value 0x30A4	   @Desc 1 */
	S16 M2_POW1_THRD2;																	/**< @Value 0x1333	   @Desc 1 */
	U16 M2_VAD3IN4;																			/**< @Value 0x0000	   @Desc 1 */
	S16 M2_POW1_VAD_RATIO;															/**< @Value 0x4000	   @Desc 1 */
	U16 M2_DEEMPH_COEF_1;																/**< @Value 0x5A3D	   @Desc 1 */
	U16 M2_DEEMPH_COEF_2;																/**< @Value 0x6536	   @Desc 1 */
	U16 M2_DEEMPH_COEF_3;																/**< @Value 0xD3A9	   @Desc 1 */
	S16 M2_BAND2_GAIN;																	/**< @Value 0x2D3F	   @Desc 1 */
	S16 M2_BAND3_GAIN;																	/**< @Value 0x4000	   @Desc 1 */
	S16 M2_BANDH_GAIN;																	/**< @Value 0x2000	   @Desc 1 */
	U16 AEC_Corel_Ratio4_Sensitive; 										/**< @Value 0x0100     @Desc 1 */
	S16 AEC_Ch4_Percentage;															/**< @Value 0x0000     @Desc 1 */
	U16 RXIN_TXREF_DELAY;																/**< @Value 0x0004	   @Desc 1 */
	U16 MIC_EC_DELAY	;																	/**< @Value 0x0010	   @Desc 1 */
	U16 AEC_REF_GAIN_AUTO;															/**< @Value 0x0000     @Desc 1 */
	
	S16 M2_REF_MIC_CH1_GAIN;														/**< @Value 0x2000     @Desc 1 */
	S16 M2_REF_MIC_CH2_GAIN;														/**< @Value 0x2000     @Desc 1 */
	S16 M2_REF_MIC_CH3_GAIN;														/**< @Value 0x2000     @Desc 1 */
	S16 M2_REF_MIC_CH4_GAIN;														/**< @Value 0x2000     @Desc 1 */
	
	S16 Mel_scale_band_num; 														/**< @Value 0x0000     @Desc 1 */
	S16 Harmonics_thrd;																	/**< @Value 0x0005     @Desc 1 */
	
	S16 AEC_tap_length;			/**< @Value 0x0000     @Desc 1 */
	
	//Inear parameter
	S16 IE_main_type;                    /**< @Value 0x0002 @Desc 1 */
    S16 IE_main_alpha;                   /**< @Value 0x799a @Desc 1 */
    S16 IE_main_alpha_1;                 /**< @Value 0x0665 @Desc 1 */
    S16 IE_Reserve1;            /**< @Value 0x0000 @Desc 1 */
                                        
    S16 IE_inear_type;                   /**< @Value 0x0002 @Desc 1 */
    S16 IE_inear_alpha;                  /**< @Value 0x799a @Desc 1 */
    S16 IE_inear_alpha_1;                /**< @Value 0x0665 @Desc 1 */
    S16 IE_inear_gain_limiter;		         /**< @Value 0x0ccd @Desc 1 */
    
	S16 IE_ENG_VAD_THR;					/**< @Value 0x1000 @Desc 1 */	
    S16 IE_ENG_VAD_HANG;					/**< @Value 0x000a @Desc 1 */
    S16 IE_ENG_POW_INIT;					/**< @Value 0xdc00 @Desc 1 */
    S16 IE_slow_up_alpha;				/**< @Value 0x0021 @Desc 1 */
    S16 IE_slow_dn_alpha;				/**< @Value 0x4000 @Desc 1 */
    S16 IE_fast_up_alpha;				/**< @Value 0x7fff @Desc 1 */
    S16 IE_fast_dn_alpha;				/**< @Value 0x028f @Desc 1 */
    S16 IE_wd_padding;						/**< @Value 0x0000 @Desc 1 */
	
	S32 IE_msc_alpha;					/**< @Value 0x028F5C00 @Desc 1 */
    S32 IE_msc_sm_up_alpha;				/**< @Value 0x00A3D700 @Desc 1 */
    S32 IE_msc_sm_dn_alpha;				/**< @Value 0x00C49C00 @Desc 1 */
    S32 IE_msc_bias;						/**< @Value 0x00040000 @Desc 1 */
    S32 IE_MSC_WIND_THR2;				/**< @Value 0x07800000 @Desc 1 */
    S32 IE_MSC_WIND_THR;					/**< @Value 0x09000000 @Desc 1 */
                                       
    S16 IE_mid_eq;                       /**< @Value 0x1000 @Desc 1 */
    S16 IE_end_inear_est;                /**< @Value 0x0030 @Desc 1 */
    S16 IE_mid_eq_st;                    /**< @Value 0x000f @Desc 1 */
    S16 IE_inEar_nor_gain0;              /**< @Value 0x7fff @Desc 1 */
    S16 IE_inEar_nor_gain1;              /**< @Value 0x7fff @Desc 1 */
	S16 IE_ff_mel_filter;					/**< @Value 0x0000 @Desc 1 */
    S16 IE_fb_mel_filter;					/**< @Value 0x0000 @Desc 1 */
    S16 IE_en_pre_emp;						/**< @Value 0x003f @Desc bit0: enable pre_emp, bit1: enable low band noise est, bit2: enable low band phase compensation, bit3: low band phase inverse, bit4: zero phase de-pop, bit5: use ast_eq to enhance ff noise est*/
	S16	IE_vad_credit_thr;					/**< @Value 0x0800 @Desc 1 */
	S16 IE_padding2;						/**< @Value 0x0000 @Desc 1 */	
	S16 IE_main_alpha_peak_up;             /**< @Value 0x747b @Desc 1 */
    S16 IE_main_alpha_peak_dn;             /**< @Value 0x7c29 @Desc 1 */
    S16 IE_main_alpha_valley_dn;           /**< @Value 0x747b @Desc 1 */
    S16 IE_main_alpha_valley_up;           /**< @Value 0x7c29 @Desc 1 */
    S16 IE_main_pow_diff_thr;              /**< @Value 0x0348 @Desc 1 */
    S16 IE_main_pow_credit;                /**< @Value 0x03e8 @Desc 1 */
    S16 IE_main_alpha_sm_fast;             /**< @Value 0x4ccd @Desc 1 */
    S16 IE_main_alpha_sm_slow;             /**< @Value 0x6ccd @Desc 1 */
    S16 IE_main_noise_update_thr0;         /**< @Value 0x0a80 @Desc 1 */
    S16 IE_main_noise_update_thr1;         /**< @Value 0x0780 @Desc 1 */
    S16 IE_main_noise_update_thr2;         /**< @Value 0x0300 @Desc 1 */
    S16 IE_main_padding;                   /**< @Value 0x0000 @Desc 1 */
   
    S32 IE_main_noise_alpha_fast;          /**< @Value 0x66666600 @Desc 1 */
    S32 IE_main_noise_alpha_slow;          /**< @Value 0x7c28f600 @Desc 1 */
    S32 IE_main_noise_alpha_ultra_slow;    /**< @Value 0x7f7cee00 @Desc 1 */
	S32 IE_main_over_est;					/**< @Value 0x19999a00 @Desc 1 */
	                                        
    S16 IE_inear_alpha_peak_up;            /**< @Value 0x747b @Desc 1 */
    S16 IE_inear_alpha_peak_dn;            /**< @Value 0x7c29 @Desc 1 */
    S16 IE_inear_alpha_valley_dn;          /**< @Value 0x747b @Desc 1 */
    S16 IE_inear_alpha_valley_up;          /**< @Value 0x7c29 @Desc 1 */
    S16 IE_inear_pow_diff_thr;             /**< @Value 0x0348 @Desc 1 */
    S16 IE_inear_pow_credit;               /**< @Value 0x03e8 @Desc 1 */
    S16 IE_inear_alpha_sm_fast;            /**< @Value 0x4ccd @Desc 1 */
    S16 IE_inear_alpha_sm_slow;            /**< @Value 0x6ccd @Desc 1 */
    S16 IE_inear_noise_update_thr0;        /**< @Value 0x0a80 @Desc 1 */
    S16 IE_inear_noise_update_thr1;        /**< @Value 0x0780 @Desc 1 */
    S16 IE_inear_noise_update_thr2;        /**< @Value 0x0300 @Desc 1 */
    S16 IE_inear_padding;                  /**< @Value 0x0000 @Desc 1 */
    
    S32 IE_inear_noise_alpha_fast;         /**< @Value 0x66666600 @Desc 1 */
    S32 IE_inear_noise_alpha_slow;         /**< @Value 0x7c28f600 @Desc 1 */
    S32 IE_inear_noise_alpha_ultra_slow;   /**< @Value 0x7f7cee00 @Desc 1 */
	S32 IE_inear_over_est;					/**< @Value 0x19999a00 @Desc 1 */
	S32 IE_main_nest_alpha;				/**< @Value 0x66666600 @Desc 1 */
    S32 IE_main_nest_alpha_1;				/**< @Value 0x19999900 @Desc 1 */
    S32 IE_main_nest_decay;				/**< @Value 0x7fffff00 @Desc 1 */
    S32 IE_main_nest_over_est;				/**< @Value 0x19999a00 @Desc 1 */
	S32 IE_inear_nest_alpha;				/**< @Value 0x66666600 @Desc 1 */
    S32 IE_inear_nest_alpha_1;				/**< @Value 0x19999900 @Desc 1 */
    S32 IE_inear_nest_decay;				/**< @Value 0x7fffff00 @Desc 1 */
    S32 IE_inear_nest_over_est;			/**< @Value 0x19999a00 @Desc 1 */

    S16	IE_de_pop_gain_limiter;			    /**< @Value    3277 @Desc 1 */ 
    S16	IE_fe_vad_thr;			            /**< @Value 0x0000 @Desc 1 */ 
	
	S16 IE_PF_MIN;					/**< @Value 0x0400 @Desc 1 */
	S16 IE_DT_ratio_thrd;			/**< @Value 0x4000 @Desc 1 */
	S16 IE_DT_length;				/**< @Value 0x0016 @Desc 1 */
	S16 IE_FB_MIC_ATTEN;				/**< @Value 0x4000 @Desc 1 */
	S16 IE_MID_WIND_ATTEN;				/**< @Value 0x7FFF @Desc 1 */   
   
    S16 IE_h_1st_max        ;       /**< @Value 0x199A @Desc 1 */
    S16 IE_wind_frozon_han  ;       /**< @Value 0x000A @Desc 1 */
	S16 IE_inEear_input_gain;       /**< @Value 4096   @Desc 1 */	
	S32 IE_MSC_frozon_eng	;       /**< @Value 0x00000078 @Desc 1 wenchen */
	S32 IE_pitch_thr		;       /**< @Value 0x0007A120 @Desc 1 */
    S32 IE_pitch_thr2		;       /**< @Value 0x00026480 @Desc 1 */
    S16 IE_pitch_idx_st		;       /**< @Value 0x0028 @Desc 1 */
    S16 IE_pitch_idx_end	;       /**< @Value 0x00C8 @Desc 1 */
    S16 IE_max_dis			;       /**< @Value 0x000A @Desc 1 */
    S16 IE_h_max			;       /**< @Value 0x3333 @Desc 1 */
    S16 IE_h_min			;       /**< @Value 0x0500 @Desc 1 */

	//FB MIC reference Gain
	S16 IE_CH1_REF_GAIN;				/**< @Value 0x0148 @Desc 1 */
	S16 IE_CH2_REF_GAIN;				/**< @Value 0x028F @Desc 1 */
	S16 IE_CH3_REF_GAIN;				/**< @Value 0x028F @Desc 1 */
	S16 IE_CH4_REF_GAIN;				/**< @Value 0x028F @Desc 1 */
	//FB MIC HP COEF
	S32 VOICE_IE_HP_COEF_0;								/**< @Value 0x0076FE9F  @Desc 1 */
	S32 VOICE_IE_HP_COEF_1;								/**< @Value 0xFF890301  @Desc 1 */
	S32 VOICE_IE_HP_COEF_2;								/**< @Value 0x0076FE9E  @Desc 1 */
	S32 VOICE_IE_HP_COEF_3;								/**< @Value 0xFF856EC7  @Desc 1 */
	S32 VOICE_IE_HP_COEF_4;								/**< @Value 0x00758D42  @Desc 1 */
	S32 VOICE_IE_HP_COEF_5;								/**< @Value 0x007F5D7F  @Desc 1 */
	S32 VOICE_IE_HP_COEF_6;								/**< @Value 0xFF80AB5D  @Desc 1 */
	S32 VOICE_IE_HP_COEF_7;								/**< @Value 0x007F5D7E  @Desc 1 */
	S32 VOICE_IE_HP_COEF_8;								/**< @Value 0xFF80EB0D  @Desc 1 */
	S32 VOICE_IE_HP_COEF_9;								/**< @Value 0x007E5FE2  @Desc 1 */
	S32 VOICE_IE_HP_COEF_10;							/**< @Value 0x00400000	@Desc 1 */

	S32 IE_main_nest_over_est1;			/**< @Value 1717986816	@Desc 1 */
	S32 IE_inear_nest_over_est1;		/**< @Value 858993408	@Desc 1 */	
    S16 IE_pitch_bin_end    ;			/**< @Value 0x0050		@Desc 1 */
    S16 IE_pitch_enhance_end;			/**< @Value 0x0078		@Desc 1 */
    S16 IE_mix_w0;                      /**< @Value 0x7FFF		@Desc 1 */    
    S16 IE_mix_w1;                      /**< @Value 0x4000		@Desc 1 */
    S16 IE_mix_w2;                      /**< @Value 0x7333		@Desc 1 */
    S16 IE_Dummy1;                      /**< @Value 0x0000		@Desc 1 */
   
    S32 IE_main_nest_decay1;			/**< @Value 2147483392 @Desc 1 */
    S32 IE_inear_nest_decay1;			/**< @Value 2147483392 @Desc 1 */

   //TX REF VAD
	S16 AEC_ENG_VAD_THR;                  /**< @Value 0x0a00		@Desc 1 */    
    S16 AEC_ENG_VAD_HANG;                  /**< @Value 0x0005		@Desc 1 */
    S16 AEC_ENG_POW_INIT;                  /**< @Value 0x0400		@Desc 1 */
    S16 AEC_slow_up_alpha;                  /**< @Value 0x0021		@Desc 1 */
	S16 AEC_slow_dn_alpha;                  /**< @Value 0x4000		@Desc 1 */
    S16 AEC_fast_up_alpha;                  /**< @Value 0x6666		@Desc 1 */
    S16 AEC_fast_dn_alpha;                  /**< @Value 0x0666		@Desc 1 */
	S16 IE_Dummy2;                      /**< @Value 0x0000		@Desc 1 */
   
    S32 IE_MSC_VAD_THR;                 /**< @Value  402653184 @Desc 1 */
    S16 IE_h_sub_max;                   /**< @Value      13107 @Desc 1 */
    S16 IE_h_1st_sub_max;               /**< @Value       6554 @Desc 1 */
                                       
    S16 IE_MIC_RATIO_THR;               /**< @Value       8192 @Desc 1 */
    S16 IE_BAND_RATIO_THR;              /**< @Value       2867 @Desc 1 */
    S16 IE_low_st;                      /**< @Value          3 @Desc 1 */
    S16 IE_high_st;                     /**< @Value         16 @Desc 1 */
    S16 IE_mic_ratio_alpha;             /**< @Value        164 @Desc 1 */
    S16 IE_band_ratio_alpha;            /**< @Value        164 @Desc 1 */

	S16 SBEC_PF_order_12;               /**< @Value    0x0028 @Desc high-byte = 0(sbec_sw off), !=0(sbec_sw on) */
	S16 SBEC_PF_order_34;               /**< @Value    0x08ff @Desc 1 */
	S16 IE_SBEC_PF_order_12;            /**< @Value    0x2828 @Desc 1 */
	S16 IE_SBEC_PF_order_34;            /**< @Value    0x28ff @Desc 1 */
                                                              
	S16 SBEC_DT_ratio_thrd_12;          /**< @Value    0x1e32 @Desc 1 */
	S16 SBEC_DT_ratio_thrd_34;          /**< @Value    0x3232 @Desc 1 */
	S16 IE_SBEC_DT_ratio_thrd_12;       /**< @Value    0x3c32 @Desc 1 */
	S16 IE_SBEC_DT_ratio_thrd_34;       /**< @Value    0x3232 @Desc 1 */
                                                              
	S16 SBEC_PF_MIN_12;                 /**< @Value    0x5A40 @Desc 1 */
	S16 SBEC_PF_MIN_34;                 /**< @Value    0x405A @Desc 1 */
	S16 IE_SBEC_PF_MIN_12;              /**< @Value    0x5A5A @Desc 1 */
	S16 IE_SBEC_PF_MIN_34;              /**< @Value    0x5A5A @Desc 1 */
	S16 SBEC_noise_paste_gain;		    /**< @Value     32767 @Desc 1 */
	S16 vad_end_bin;                    /**< @Value        48 @Desc 1 */        
	
	S16 IE_post_type;					/**< @Value         0 @Desc 1 */
	S16 IE_post_alpha;					/**< @Value         0 @Desc 1 */
	S16 IE_post_alpha_1;				/**< @Value         0 @Desc 1 */
	S16 IE_post_gain_limiter;			/**< @Value         0 @Desc 1 */
	S16 IE_post_mel_filter;				/**< @Value         0 @Desc 1 */
	S16 IE_post_abs_thr_order;			/**< @Value         0 @Desc 1 */
	
	S16 ov_st_bin;						/**< @Value         0 @Desc 1 */
    S16 nc_sig_nor;						/**< @Value    0x1998 @Desc 1 */
	S16 NC_MU_MIN;						/**< @Value         8 @Desc 1 */
	S16	IE_wind_pow_thd;				/**< @Value       164 @Desc 1 */
	U16 M2_lp_coef_attu;			/**< @Value       0x0 @Desc 1 */
	U16 M2_lp_thrd;				/**< @Value       0x10 @Desc 1 */
	S16 M2_lp_norm_thrd;		/**< @Value       0x0 @Desc 1 */
	S16 low_noise_thrd;					/**< @Value         0 @Desc low_noise_detection=1 @ (low_noise_thrd!=0), low_noise_detection=0 @ (low_noise_thrd==0) */
	S16 low_noise_slow_alpha;			/**< @Value       163 @Desc 1 */
	S16 low_noise_fast_alpha;			/**< @Value      1638 @Desc 1 */

   //Reserve
	U32 RESERVE_1;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_2;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_3;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_4;																			/**< @Value 0x00000000 @Desc 1 */	
	U32 RESERVE_5;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_6;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_7;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_8;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_9;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_10;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_11;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_12;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_13;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_14;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_15;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_16;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_17;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_18;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_19;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_20;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_21;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_22;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_23;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_24;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_25;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_26;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_27;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_28;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_29;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_30;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_31;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_32;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_33;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_34;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_35;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_36;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_37;																		  /**< @Value 0x00000000 @Desc 1 */																	  /**< @Value 0x00000000 @Desc 1 */
} PACKED DSP_PARA_AEC_NR_STRU;

#else

typedef struct stru_dsp_aec_nr_para_s
{
	U8  ENABLE;                                        	/**< @Value 0x01 @Desc 1 */
	U8  REVISION;                                      	/**< @Value 0x01 @Desc 1 */
	U16 AEC_NR_EN;		                               		/**< @Value 0x0003 @Desc b0: EC/NR switch, b1-2: 0(1-MIC), 1(1-MIC PD), 2(2-MIC), b3: 0(headset),1(speaker), b13: 0(disable Rx HPF), 1(enable Rx HPF), b15: 0(disable flash read) ,1(enable flash read)*/
	U16 AEC_CNG_GAIN_M;                                	/**< @Value 0x3333 @Desc 1 */
	U16 AEC_ref_pow_min;                               	/**< @Value 0x0001 @Desc 1 */
	U16 AEC_ECHO_TAIL_LENGTH;                          	/**< @Value 0x0006 @Desc 1 */
	U16 AEC_EC_RESIST;																	/**< @Value 0x0000 @Desc 1 */
	U16 AEC_MU_FAC;																			/**< @Value 0x399A @Desc 1 */
	U16 AEC_MU_MIN;																			/**< @Value 0x1000 @Desc 1 */
	U16 AEC_NORM_CAP1;																	/**< @Value 0x1FD7 @Desc 1 */
	U16 AEC_NORM_CAP2;																	/**< @Value 0x3000 @Desc 1 */
	U16 AEC_PF_MIN;																			/**< @Value 0x0400 @Desc 1 */
	S32 AEC_block_percent;															/**< @Value 0x00155555 @Desc 1 */
	U16 AEC_DT_boost;																		/**< @Value 0x0001 @Desc 1 */
	U16 AEC_PF_order;																		/**< @Value 0x0003 @Desc 1 */
	U16 AEC_DT_ratio_thrd;															/**< @Value 0x4000 @Desc 1 */
	U16 AEC_norm_tap;																		/**< @Value 0x0018 @Desc 1 */
	U16 AEC_DT_length;																	/**< @Value 0x0006 @Desc 1 */
	U16 MULT_AFTER_EC;																	/**< @Value 0x1000 @Desc 1 */
	S16 CH1_REF_GAIN;																		/**< @Value 0x0021 @Desc 1 */
	S16 CH2_REF_GAIN;																		/**< @Value 0x0021 @Desc 1 */
	S16 CH3_REF_GAIN;																		/**< @Value 0x0852 @Desc 1 */
	S16 CH4_REF_GAIN;																		/**< @Value 0x299A @Desc 1 */
	S16 CH1_REF2_GAIN;																	/**< @Value 0x0148 @Desc 1 */
	S16 CH2_REF2_GAIN;																	/**< @Value 0x028F @Desc 1 */
	S16 CH3_REF2_GAIN;																	/**< @Value 0x028F @Desc 1 */
	S16 CH4_REF2_GAIN;																	/**< @Value 0x028F @Desc 1 */

	U16 AVC_ALPHA;																			/**< @Value 0x799A @Desc 1 */
	U16 AVC_THRD;																				/**< @Value 0x0037 @Desc 1 */
	U16 AVC_VOL_MAX;  																	/**< @Value 0x7FFF @Desc 1 */
	U16 DSP_EC_SW;																			/**< @Value 0x0000 @Desc 1 */

  U16 WB_NR_TX_POW_MIN_BUF_PERIOD;										/**< @Value 0x000B @Desc 1 */
  U16 WB_NR_TX_NOISE_GAIN_LIMITER;   									/**< @Value 0x0FEE @Desc 1 */
  U16 WB_NR_TX_VAD_THRD1;            									/**< @Value 0x0348 @Desc 1 */
  U16 WB_NR_TX_VAD_THRD_BANDS_0;    									/**< @Value 0x0B0B @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_1;    									/**< @Value 0x0B08 @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_2;    									/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_3;    									/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_TX_VAD_THRD_BANDS_4;    									/**< @Value 0x0A07 @Desc 1 */
  U16 WB_NR_TX_OVER_SUPPRESS_FAC;    									/**< @Value 0x219A @Desc 1 */
  U16 WB_NR_TX_SPEECH_RETAIN_FAC;    									/**< @Value 0x599A @Desc 1 */
  U16 WB_NR_TX_NOISE_LAMDA;          									/**< @Value 0x799A @Desc 1 */
  U16 WB_NR_TX_NOISE_FLOOR_MIN;      									/**< @Value 0x8400 @Desc 1 */
  U16 WB_NR_FAST_ALPHA;		          									/**< @Value 0x747B @Desc 1 */
  U16 WB_NR_SLOW_ALPHA;	            									/**< @Value 0x7C29 @Desc 1 */
  U16 WB_NR_NOISE_UPDATE_FAST;       									/**< @Value 6554	 @Desc 1 */
  U16 WB_NR_NOISE_UPDATE_SLOW;       									/**< @Value 983		 @Desc 1 */
  U16 WB_NR_NOISE_UPDATE_ULTRASLOW; 									/**< @Value 131		 @Desc 1 */
  U16 WB_NR_TX_EMPH_COEF_0;														/**< @Value 0xAE82 @Desc 1 */
	U16 WB_NR_TX_EMPH_COEF_1;														/**< @Value 0x3DD9 @Desc 1 */

  U16 NB_NR_RX_POW_MIN_BUF_PERIOD;  									/**< @Value 0x000D @Desc 1 */
  U16 NB_NR_RX_NOISE_GAIN_LIMITER;  									/**< @Value 0x1EB8 @Desc 1 */
  U16 NB_NR_RX_VAD_THRD1;           									/**< @Value 0x0348 @Desc 1 */
  U16 NB_NR_RX_VAD_THRD_BANDS_0;   										/**< @Value 0x0b0b @Desc 1 */
	U16 NB_NR_RX_VAD_THRD_BANDS_1;   										/**< @Value 0x0b08 @Desc 1 */
	U16 NB_NR_RX_VAD_THRD_BANDS_2;   										/**< @Value 0x0808 @Desc 1 */
	U16 NB_NR_RX_VAD_THRD_BANDS_3;   										/**< @Value 0x0807 @Desc 1 */
  U16 NB_NR_RX_OVER_SUPPRESS_FAC;   									/**< @Value 0x2666 @Desc 1 */
  U16 NB_NR_RX_SPEECH_RETAIN_FAC;   									/**< @Value 0x6000 @Desc 1 */
  U16 NB_NR_RX_NOISE_LAMDA;         									/**< @Value 0x799a @Desc 1 */
  U16 NB_NR_RX_NOISE_FLOOR_MIN;     									/**< @Value 0xae00 @Desc 1 */
  U16 NB_NR_RX_EMPH_COEF_0;														/**< @Value 0xC8C0 @Desc 1 */
	U16 NB_NR_RX_EMPH_COEF_1;														/**< @Value 0x24AF @Desc 1 */

	U16 WB_NR_RX_POW_MIN_BUF_PERIOD;										/**< @Value 0x000b @Desc 1 */
	U16 WB_NR_RX_NOISE_GAIN_LIMITER;										/**< @Value 0x1eb8 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD1;															/**< @Value 0x0348 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_0;											/**< @Value 0x0b0b @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_1;											/**< @Value 0x0b08 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_2;											/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_3;											/**< @Value 0x0808 @Desc 1 */
	U16 WB_NR_RX_VAD_THRD_BANDS_4;											/**< @Value 0x0a07 @Desc 1 */
	U16 WB_NR_RX_OVER_SUPPRESS_FAC;											/**< @Value 0x2800 @Desc 1 */
	U16 WB_NR_RX_SPEECH_RETAIN_FAC;											/**< @Value 0x6000 @Desc 1 */
	U16 WB_NR_RX_NOISE_LAMDA;														/**< @Value 0x799a @Desc 1 */
	U16 WB_NR_RX_NOISE_FLOOR_MIN;												/**< @Value 0xae00 @Desc 1 */
	U16 WB_NR_RX_EMPH_COEF_0;														/**< @Value 0xCA16 @Desc 1 */
	U16 WB_NR_RX_EMPH_COEF_1;														/**< @Value 0x0B29 @Desc 1 */

	U16 PD_NR_TX_OPT;																		/**< @Value      0 @Desc 1 */
	U16 PD_PEAK_BUF_SIZE;																/**< @Value      0 @Desc 1 */
	U16 PD_PITCH_VAD_THRD;															/**< @Value  20480 @Desc 1 */
	U16 PD_PITCH_FAC;																		/**< @Value  31949 @Desc 1 */
	U16 PD_PEAK_RATIO_FAC;															/**< @Value  30032 @Desc 1 */
	U16 PD_TRANSIENT_THRD;															/**< @Value 0x0046 @Desc 1 */
	U16 PD_TRANSIENT_AUG;																/**< @Value   2048 @Desc 1 */
	U16 PD_PITCH_AUG;																		/**< @Value    328 @Desc 1 */
	U16 PD_TRANSIENT_THRD2;															/**< @Value 0x0028 @Desc 1 */
	U16 PD_RESERVE2;																		/**< @Value      0 @Desc 1 */

	U16 M2_MICDIST;																			/**< @Value 0x0000 @Desc 1 */
	U16 M2_WIND_THRD;																		/**< @Value 0x1000 @Desc 1 */
	U16 M2_WIND_TRANS;																	/**< @Value 0x0640 @Desc 1 */
	U16 M2_WIND_BLOCK;																	/**< @Value 0x1000 @Desc 1 */
	U16 M2_FILTER_GRD;																	/**< @Value 0x0002 @Desc 1 */
	U16 M2_DISTANCE_FAC;																/**< @Value 0x6119 @Desc 1 */
	U16 M2_VAD_THRD_1;																	/**< @Value 0x1062 @Desc 1 */
	U16 M2_VAD_THRD_2;																	/**< @Value 0x0C4A @Desc 1 */
	U16 M2_VAD_THRD_3;																	/**< @Value 0x0852 @Desc 1 */
	U16 M2_VAD_THRD_4;																	/**< @Value 0x03D7 @Desc 1 */
	U16 M2_VAD_THRD_12;																	/**< @Value 0x1333 @Desc 1 */
	U16 M2_VAD_THRD_22;																	/**< @Value 0x0E14 @Desc 1 */
	U16 M2_VAD_THRD_32;																	/**< @Value 0x0666 @Desc 1 */
	U16 M2_VAD_THRD_42;																	/**< @Value 0x028F @Desc 1 */
	U16 M2_VAD_THRD_13;																	/**< @Value 0x2A3D @Desc 1 */
	U16 M2_VAD_THRD_23;																	/**< @Value 0x15C3 @Desc 1 */
	U16 M2_VAD_THRD_33;																	/**< @Value 0x0B85 @Desc 1 */
	U16 M2_VAD_THRD_43;																	/**< @Value 0x051F @Desc 1 */
	U16 M2_NORM_FAC1;																		/**< @Value 0x0000 @Desc 1 */
	U16 M2_PHASE_COMB;																	/**< @Value 0x5C02 @Desc 1 */
	U16 M2_PHASE_GAIN;																	/**< @Value 0x4000 @Desc 1 */
	U16 M2_NE_DURATION;																	/**< @Value 0x0038 @Desc 1 */

	U16 M2_BEAM1_NORM_LIMIT;														/**< @Value 0x6000 @Desc 1 */
	U16 M2_OVER_SUPPRESS_FAC2;													/**< @Value 0x2000 @Desc 1 */
	S16 M2_BAND1_ALPHA;																	/**< @Value 0x7800 @Desc 1 */
	S16 M2_BAND2_ALPHA;																	/**< @Value 0x7D71 @Desc 1 */
	S16 M2_BAND3_ALPHA;																	/**< @Value 0x7F80 @Desc 1 */
	S16 M2_BANDH_ALPHA;																	/**< @Value 0x7F5C @Desc 1 */
	
	S16 OFF_PITCH_ALPHA;																/**< @Value 0x0106 @Desc 1 */
	S16 WIND_DETECT_THRD;																/**< @Value 0x0014 @Desc 1 */
	S16 PD_WIND_FREQ_RANGE;															/**< @Value 0x0011 @Desc 1 */
	S16 PD_WIND_LOW_ATTEN;															/**< @Value 0x65AD @Desc 1 */
	S16 PD_NONSTATIONARY_ALPHA_1;												/**< @Value 0x0106 @Desc 1 */
	S16 PD_NONSTATIONARY_ALPHA_2;												/**< @Value 0x028F @Desc 1 */
	
	S16 M2_PD_HIGH_ATTEN_GOAL;													/**< @Value	13045		@Desc 1 */
	S32 M2_PD_SEVERE_NOISE_THRD;												/**< @Value 8388352 @Desc 1 */
  S16 NR_HIGH_ATTEN_DIVIDE;  													/**< @Value 109  		@Desc 1 */
  
  U16 WB_VOICE_TX_GAIN;																/**< @Value 0x0800 @Desc 1 */
  U16 NB_VOICE_TX_GAIN;																/**< @Value 0x0800 @Desc 1 */

	S32 VOICE_TX_HP_COEF_0;								              /**< @Value 6731724  @Desc 1 */
	S32 VOICE_TX_HP_COEF_1;								              /**< @Value -6725950 @Desc 1 */
	S32 VOICE_TX_HP_COEF_2;								              /**< @Value 6731723  @Desc 1 */
	S32 VOICE_TX_HP_COEF_3;								              /**< @Value -6986246 @Desc 1 */
	S32 VOICE_TX_HP_COEF_4;								              /**< @Value 5970844  @Desc 1 */
	S32 VOICE_TX_HP_COEF_5;								              /**< @Value 8329426  @Desc 1 */
	S32 VOICE_TX_HP_COEF_6;								              /**< @Value -8293150 @Desc 1 */
	S32 VOICE_TX_HP_COEF_7;								              /**< @Value 8329425  @Desc 1 */
	S32 VOICE_TX_HP_COEF_8;								              /**< @Value -8072154 @Desc 1 */
	S32 VOICE_TX_HP_COEF_9;								              /**< @Value 7972216  @Desc 1 */
	S32 VOICE_TX_HP_COEF_10;							              /**< @Value 4194304  @Desc 1 */

	S32 VOICE_WB_RX_HP_COEF_0;	                        /**< @Value 7798431  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_1;	                        /**< @Value -7798015 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_2;	                        /**< @Value 7798430  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_3;	                        /**< @Value -8032569 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_4;	                        /**< @Value 7703874  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_5;	                        /**< @Value 8347007  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_6;	                        /**< @Value -8344739 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_7;	                        /**< @Value 8347006  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_8;	                        /**< @Value -8328435 @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_9;	                        /**< @Value 8282082  @Desc 1 */
	S32 VOICE_WB_RX_HP_COEF_10;	                        /**< @Value 4194304  @Desc 1 */	
	
	S32 VOICE_NB_RX_HP_COEF_0;	                        /**< @Value 7398364  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_1;	                        /**< @Value -7396782 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_2;	                        /**< @Value 7398363  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_3;	                        /**< @Value -7679575 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_4;	                        /**< @Value 7075462  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_5;	                        /**< @Value 8373828  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_6;	                        /**< @Value -8364724 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_7;	                        /**< @Value 8373827  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_8;	                        /**< @Value -8255389 @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_9;	                        /**< @Value 8177081  @Desc 1 */
	S32 VOICE_NB_RX_HP_COEF_10;	                        /**< @Value 4194304  @Desc 1 */

	U16 AEC_Corel_Ratio1_Sensitive;											/**< @Value 0x0100	   @Desc 1 */
	S16 PD_VOICE_IN_WIND;                               /**< @Value 0x7FFF	   @Desc 1 */  
	S16 PD_VOICE_AMP_LEVEL;                             /**< @Value 0x4000	   @Desc 1 */  
	S16 M2_HF_NSG2_OVER;	                              /**< @Value 0x0000	   @Desc 1 */
	S16 M2_DF_RATIO;																		/**< @Value 0x6CCD	   @Desc 1 */
	S16 M2_POW1_ALPHA;																	/**< @Value 0x0CCD	   @Desc 1 */
	S16 M2_POW1_UP_GAIN;																/**< @Value 0x0028	   @Desc 1 */
	S16 M2_POW1_DOWN_GAIN;															/**< @Value 0x00A4	   @Desc 1 */
	S16 M2_POW1_THRD1;																	/**< @Value 0x30A4	   @Desc 1 */
	S16 M2_POW1_THRD2;																	/**< @Value 0x1333	   @Desc 1 */
	U16 M2_VAD3IN4;																			/**< @Value 0x0000	   @Desc 1 */
	S16 M2_POW1_VAD_RATIO;															/**< @Value 0x4000	   @Desc 1 */
	U16 M2_DEEMPH_COEF_1;																/**< @Value 0x5A3D	   @Desc 1 */
	U16 M2_DEEMPH_COEF_2;																/**< @Value 0x6536	   @Desc 1 */
	U16 M2_DEEMPH_COEF_3;																/**< @Value 0xD3A9	   @Desc 1 */
	S16 M2_BAND2_GAIN;																	/**< @Value 0x2D3F	   @Desc 1 */
	S16 M2_BAND3_GAIN;																	/**< @Value 0x4000	   @Desc 1 */
	S16 M2_BANDH_GAIN;																	/**< @Value 0x2000	   @Desc 1 */
	U16 AEC_Corel_Ratio4_Sensitive; 										/**< @Value 0x0100     @Desc 1 */
	S16 AEC_Ch4_Percentage;															/**< @Value 0x0000     @Desc 1 */
	U16 RXIN_TXREF_DELAY;																/**< @Value 0x0004	   @Desc 1 */
	U16 MIC_EC_DELAY	;																	/**< @Value 0x0014	   @Desc 1 */
	U16 AEC_REF_GAIN_AUTO;															/**< @Value 0x0000     @Desc 1 */
	
	S16 M2_REF_MIC_CH1_GAIN;														/**< @Value 0x2000     @Desc 1 */
	S16 M2_REF_MIC_CH2_GAIN;														/**< @Value 0x2000     @Desc 1 */
	S16 M2_REF_MIC_CH3_GAIN;														/**< @Value 0x2000     @Desc 1 */
	S16 M2_REF_MIC_CH4_GAIN;														/**< @Value 0x2000     @Desc 1 */
	
	S16 Mel_scale_band_num; 														/**< @Value 0x0002     @Desc 1 */
	S16 Harmonics_thrd;																	/**< @Value 0x0005     @Desc 1 */
	
	S16 AEC_tap_length;			/**< @Value 0x755F     @Desc 1 */
	
	//Inear parameter
	S16 IE_main_type;                    /**< @Value 0x0002 @Desc 1 */
    S16 IE_main_alpha;                   /**< @Value 0x799a @Desc 1 */
    S16 IE_main_alpha_1;                 /**< @Value 0x0665 @Desc 1 */
    S16 IE_Reserve1;            /**< @Value 0x0000 @Desc 1 */
                                        
    S16 IE_inear_type;                   /**< @Value 0x0002 @Desc 1 */
    S16 IE_inear_alpha;                  /**< @Value 0x799a @Desc 1 */
    S16 IE_inear_alpha_1;                /**< @Value 0x0665 @Desc 1 */
    S16 IE_inear_gain_limiter;		         /**< @Value 0x0ccd @Desc 1 */
    
	S16 IE_ENG_VAD_THR;					/**< @Value 0x1000 @Desc 1 */	
    S16 IE_ENG_VAD_HANG;					/**< @Value 0x000a @Desc 1 */
    S16 IE_ENG_POW_INIT;					/**< @Value 0xdc00 @Desc 1 */
    S16 IE_slow_up_alpha;				/**< @Value 0x0021 @Desc 1 */
    S16 IE_slow_dn_alpha;				/**< @Value 0x4000 @Desc 1 */
    S16 IE_fast_up_alpha;				/**< @Value 0x7fff @Desc 1 */
    S16 IE_fast_dn_alpha;				/**< @Value 0x028f @Desc 1 */
    S16 IE_wd_padding;						/**< @Value 0x0000 @Desc 1 */
	
	S32 IE_msc_alpha;					/**< @Value 0x028F5C00 @Desc 1 */
    S32 IE_msc_sm_up_alpha;				/**< @Value 0x00A3D700 @Desc 1 */
    S32 IE_msc_sm_dn_alpha;				/**< @Value 0x00C49C00 @Desc 1 */
    S32 IE_msc_bias;						/**< @Value 0x00040000 @Desc 1 */
    S32 IE_MSC_WIND_THR2;				/**< @Value 0x07800000 @Desc 1 */
    S32 IE_MSC_WIND_THR;					/**< @Value 0x09000000 @Desc 1 */
                                       
    S16 IE_mid_eq;                       /**< @Value 0x1000 @Desc 1 */
    S16 IE_end_inear_est;                /**< @Value 0x0030 @Desc 1 */
    S16 IE_mid_eq_st;                    /**< @Value 0x000f @Desc 1 */
    S16 IE_inEar_nor_gain0;              /**< @Value 0x7fff @Desc 1 */
    S16 IE_inEar_nor_gain1;              /**< @Value 0x7fff @Desc 1 */
	S16 IE_ff_mel_filter;					/**< @Value 0x0000 @Desc 1 */
    S16 IE_fb_mel_filter;					/**< @Value 0x0000 @Desc 1 */
    S16 IE_en_pre_emp;						/**< @Value 0x003f @Desc bit0: enable pre_emp, bit1: enable low band noise est, bit2: enable low band phase compensation, bit3: low band phase inverse, bit4: zero phase de-pop, bit5: use ast_eq to enhance ff noise est*/
	S16	IE_vad_credit_thr;					/**< @Value 0x0800 @Desc 1 */
	S16 IE_padding2;						/**< @Value 0x0000 @Desc 1 */	
	S16 IE_main_alpha_peak_up;             /**< @Value 0x747b @Desc 1 */
    S16 IE_main_alpha_peak_dn;             /**< @Value 0x7c29 @Desc 1 */
    S16 IE_main_alpha_valley_dn;           /**< @Value 0x747b @Desc 1 */
    S16 IE_main_alpha_valley_up;           /**< @Value 0x7c29 @Desc 1 */
    S16 IE_main_pow_diff_thr;              /**< @Value 0x0348 @Desc 1 */
    S16 IE_main_pow_credit;                /**< @Value 0x03e8 @Desc 1 */
    S16 IE_main_alpha_sm_fast;             /**< @Value 0x4ccd @Desc 1 */
    S16 IE_main_alpha_sm_slow;             /**< @Value 0x6ccd @Desc 1 */
    S16 IE_main_noise_update_thr0;         /**< @Value 0x0a80 @Desc 1 */
    S16 IE_main_noise_update_thr1;         /**< @Value 0x0780 @Desc 1 */
    S16 IE_main_noise_update_thr2;         /**< @Value 0x0300 @Desc 1 */
    S16 IE_main_padding;                   /**< @Value 0x0000 @Desc 1 */
   
    S32 IE_main_noise_alpha_fast;          /**< @Value 0x66666600 @Desc 1 */
    S32 IE_main_noise_alpha_slow;          /**< @Value 0x7c28f600 @Desc 1 */
    S32 IE_main_noise_alpha_ultra_slow;    /**< @Value 0x7f7cee00 @Desc 1 */
	S32 IE_main_over_est;					/**< @Value 0x19999a00 @Desc 1 */
	                                        
    S16 IE_inear_alpha_peak_up;            /**< @Value 0x747b @Desc 1 */
    S16 IE_inear_alpha_peak_dn;            /**< @Value 0x7c29 @Desc 1 */
    S16 IE_inear_alpha_valley_dn;          /**< @Value 0x747b @Desc 1 */
    S16 IE_inear_alpha_valley_up;          /**< @Value 0x7c29 @Desc 1 */
    S16 IE_inear_pow_diff_thr;             /**< @Value 0x0348 @Desc 1 */
    S16 IE_inear_pow_credit;               /**< @Value 0x03e8 @Desc 1 */
    S16 IE_inear_alpha_sm_fast;            /**< @Value 0x4ccd @Desc 1 */
    S16 IE_inear_alpha_sm_slow;            /**< @Value 0x6ccd @Desc 1 */
    S16 IE_inear_noise_update_thr0;        /**< @Value 0x0a80 @Desc 1 */
    S16 IE_inear_noise_update_thr1;        /**< @Value 0x0780 @Desc 1 */
    S16 IE_inear_noise_update_thr2;        /**< @Value 0x0300 @Desc 1 */
    S16 IE_inear_padding;                  /**< @Value 0x0000 @Desc 1 */
    
    S32 IE_inear_noise_alpha_fast;         /**< @Value 0x66666600 @Desc 1 */
    S32 IE_inear_noise_alpha_slow;         /**< @Value 0x7c28f600 @Desc 1 */
    S32 IE_inear_noise_alpha_ultra_slow;   /**< @Value 0x7f7cee00 @Desc 1 */
	S32 IE_inear_over_est;					/**< @Value 0x19999a00 @Desc 1 */
	S32 IE_main_nest_alpha;				/**< @Value 0x66666600 @Desc 1 */
    S32 IE_main_nest_alpha_1;				/**< @Value 0x19999900 @Desc 1 */
    S32 IE_main_nest_decay;				/**< @Value 0x7fffff00 @Desc 1 */
    S32 IE_main_nest_over_est;				/**< @Value 0x19999a00 @Desc 1 */
	S32 IE_inear_nest_alpha;				/**< @Value 0x66666600 @Desc 1 */
    S32 IE_inear_nest_alpha_1;				/**< @Value 0x19999900 @Desc 1 */
    S32 IE_inear_nest_decay;				/**< @Value 0x7fffff00 @Desc 1 */
    S32 IE_inear_nest_over_est;			/**< @Value 0x19999a00 @Desc 1 */

    S16	IE_de_pop_gain_limiter;			    /**< @Value    3277 @Desc 1 */ 
    S16	IE_fe_vad_thr;			            /**< @Value 0x0000 @Desc 1 */ 
	
	S16 IE_PF_MIN;					/**< @Value 0x0400 @Desc 1 */
	S16 IE_DT_ratio_thrd;			/**< @Value 0x4000 @Desc 1 */
	S16 IE_DT_length;				/**< @Value 0x0016 @Desc 1 */
	S16 IE_FB_MIC_ATTEN;				/**< @Value 0x4000 @Desc 1 */
	S16 IE_MID_WIND_ATTEN;				/**< @Value 0x7FFF @Desc 1 */   
   
    S16 IE_h_1st_max        ;       /**< @Value 0x199A @Desc 1 */
    S16 IE_wind_frozon_han  ;       /**< @Value 0x000A @Desc 1 */
	S16 IE_inEear_input_gain;       /**< @Value 4096   @Desc 1 */	
	S32 IE_MSC_frozon_eng	;       /**< @Value 0x00000078 @Desc 1 wenchen */
	S32 IE_pitch_thr		;       /**< @Value 0x0007A120 @Desc 1 */
    S32 IE_pitch_thr2		;       /**< @Value 0x00026480 @Desc 1 */
    S16 IE_pitch_idx_st		;       /**< @Value 0x0028 @Desc 1 */
    S16 IE_pitch_idx_end	;       /**< @Value 0x00C8 @Desc 1 */
    S16 IE_max_dis			;       /**< @Value 0x000A @Desc 1 */
    S16 IE_h_max			;       /**< @Value 0x3333 @Desc 1 */
    S16 IE_h_min			;       /**< @Value 0x0500 @Desc 1 */

	//FB MIC reference Gain
	S16 IE_CH1_REF_GAIN;				/**< @Value 0x0148 @Desc 1 */
	S16 IE_CH2_REF_GAIN;				/**< @Value 0x028F @Desc 1 */
	S16 IE_CH3_REF_GAIN;				/**< @Value 0x028F @Desc 1 */
	S16 IE_CH4_REF_GAIN;				/**< @Value 0x028F @Desc 1 */
	//FB MIC HP COEF
	S32 VOICE_IE_HP_COEF_0;								/**< @Value 0x0076FE9F  @Desc 1 */
	S32 VOICE_IE_HP_COEF_1;								/**< @Value 0xFF890301  @Desc 1 */
	S32 VOICE_IE_HP_COEF_2;								/**< @Value 0x0076FE9E  @Desc 1 */
	S32 VOICE_IE_HP_COEF_3;								/**< @Value 0xFF856EC7  @Desc 1 */
	S32 VOICE_IE_HP_COEF_4;								/**< @Value 0x00758D42  @Desc 1 */
	S32 VOICE_IE_HP_COEF_5;								/**< @Value 0x007F5D7F  @Desc 1 */
	S32 VOICE_IE_HP_COEF_6;								/**< @Value 0xFF80AB5D  @Desc 1 */
	S32 VOICE_IE_HP_COEF_7;								/**< @Value 0x007F5D7E  @Desc 1 */
	S32 VOICE_IE_HP_COEF_8;								/**< @Value 0xFF80EB0D  @Desc 1 */
	S32 VOICE_IE_HP_COEF_9;								/**< @Value 0x007E5FE2  @Desc 1 */
	S32 VOICE_IE_HP_COEF_10;							/**< @Value 0x00400000	@Desc 1 */

	S32 IE_main_nest_over_est1;			/**< @Value 1717986816	@Desc 1 */
	S32 IE_inear_nest_over_est1;		/**< @Value 858993408	@Desc 1 */	
    S16 IE_pitch_bin_end    ;			/**< @Value 0x0050		@Desc 1 */
    S16 IE_pitch_enhance_end;			/**< @Value 0x0078		@Desc 1 */
    S16 IE_mix_w0;                      /**< @Value 0x7FFF		@Desc 1 */    
    S16 IE_mix_w1;                      /**< @Value 0x4000		@Desc 1 */
    S16 IE_mix_w2;                      /**< @Value 0x7333		@Desc 1 */
    S16 IE_Dummy1;                      /**< @Value 0x0000		@Desc 1 */
   
    S32 IE_main_nest_decay1;			/**< @Value 2147483392 @Desc 1 */
    S32 IE_inear_nest_decay1;			/**< @Value 2147483392 @Desc 1 */

   //TX REF VAD
	S16 AEC_ENG_VAD_THR;                  /**< @Value 0x0a00		@Desc 1 */    
    S16 AEC_ENG_VAD_HANG;                  /**< @Value 0x0005		@Desc 1 */
    S16 AEC_ENG_POW_INIT;                  /**< @Value 0x0400		@Desc 1 */
    S16 AEC_slow_up_alpha;                  /**< @Value 0x0021		@Desc 1 */
	S16 AEC_slow_dn_alpha;                  /**< @Value 0x4000		@Desc 1 */
    S16 AEC_fast_up_alpha;                  /**< @Value 0x6666		@Desc 1 */
    S16 AEC_fast_dn_alpha;                  /**< @Value 0x0666		@Desc 1 */
	S16 IE_Dummy2;                      /**< @Value 0x0000		@Desc 1 */
   
    S32 IE_MSC_VAD_THR;                 /**< @Value  402653184 @Desc 1 */
    S16 IE_h_sub_max;                   /**< @Value      13107 @Desc 1 */
    S16 IE_h_1st_sub_max;               /**< @Value       6554 @Desc 1 */
                                       
    S16 IE_MIC_RATIO_THR;               /**< @Value       8192 @Desc 1 */
    S16 IE_BAND_RATIO_THR;              /**< @Value       2867 @Desc 1 */
    S16 IE_low_st;                      /**< @Value          3 @Desc 1 */
    S16 IE_high_st;                     /**< @Value         16 @Desc 1 */
    S16 IE_mic_ratio_alpha;             /**< @Value        164 @Desc 1 */
    S16 IE_band_ratio_alpha;            /**< @Value        164 @Desc 1 */

	//Reserve
	U32 RESERVE_1;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_2;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_3;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_4;																			/**< @Value 0x00000000 @Desc 1 */	
	U32 RESERVE_5;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_6;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_7;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_8;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_9;																			/**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_10;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_11;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_12;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_13;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_14;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_15;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_16;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_17;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_18;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_19;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_20;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_21;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_22;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_23;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_24;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_25;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_26;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_27;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_28;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_29;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_30;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_31;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_32;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_33;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_34;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_35;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_36;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_37;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_38;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_39;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_40;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_41;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_42;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_43;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_44;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_45;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_46;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_47;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_48;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_49;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_50;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_51;																		  /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_52;																		  /**< @Value 0x00000000 @Desc 1 */
										
} PACKED DSP_PARA_AEC_NR_STRU;

#endif

#ifdef WIN32
	#pragma pack()
#endif

#endif /* _DSP_PARA_AEC_NR_H_ */
