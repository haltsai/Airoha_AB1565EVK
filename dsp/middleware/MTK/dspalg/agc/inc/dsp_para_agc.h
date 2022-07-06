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
#ifndef _DSP_PARA_AGC_H_
#define _DSP_PARA_AGC_H_

#include "dsp_utilities.h"

#define C_Max_PacketBuf 32

/**
 * @brief Parameter for DSP AGC algorithm
 * @KeyID 0xE190 WB_RX_AGC
 * @KeyID 0xE191 NB_RX_AGC
 * @KeyID 0xE192 TX_AGC
 */
typedef struct
{
	U8 ENABLE;							    /**< @Value 0x00       @Desc 1 */
	U8 REVISION;						    /**< @Value 0x01       @Desc 1 */
	U16 agc_mode;                           /**< @Value 0x0003     @Desc 
	                                                                B[1] = 0(slowgain_en disable),  1(slowgain_en enable)
							                                        B[0] = 0(vad_en disable),       1(vad_en enable)      */
	S16 period;							    /**< @Value 0x0001     @Desc 1 */	
	S16 reserve_s;                          /**< @Value 0x0000     @Desc 1 */	
	S32 target_level;                       /**< @Value 0x016C310E @Desc 1 */
	S32 gain_max;                           /**< @Value 0x141857EA @Desc 1 */  
	S32 gain_min;                           /**< @Value 0x05A9DF7B @Desc 1 */  	
	S16 gain_alpha;                         /**< @Value 0x0010     @Desc 1 */
	S16 alpha_attack;					    /**< @Value 0x0DAD     @Desc 1 */
	S16 alpha_decay;					    /**< @Value 0x0DAD     @Desc 1 */	
	S16 vad_debounce;                       /**< @Value 0x0010     @Desc 1 */
	S32	adj_pos_inc;						/**< @Value 0x400A9D00 @Desc 1 */
	S32	adj_neg_inc;						/**< @Value 0x3FF56500 @Desc 1 */
	S32 min_level;						    /**< @Value 0x0147AE14 @Desc 1 */
	S32 bot_level;						    /**< @Value 0x01044915 @Desc 1 */		
	
	U32 RESERVE_0;						    /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_1;						    /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_2;						    /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_3;						    /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_4;						    /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_5;						    /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_6;						    /**< @Value 0x00000000 @Desc 1 */
	U32 RESERVE_7;						    /**< @Value 0x00000000 @Desc 1 */
} PACKED AGC_VO_NVKEY;

typedef struct {        
	S16 ENABLE; 
	S16 period;	
	S32 target_level;       
	S32 gain_max;           
	S32 gain_min;    
	S16 gain_alpha;
	S16 alpha_attack;		
	S16 alpha_decay;
	S16 vad_debound_thr; 
	S32	adj_pos_inc;					
	S32	adj_neg_inc;					
	S32 min_level;			
	S32 bot_level;	
	S16 slowgain_en, vad_en;
	S16	packet_num;
	S16	vad_o, vad_st, vad_debound_cnt;    
	S32 agc_max_buf;
	S32	magnitude;    
	S32 agc_packet_buf[C_Max_PacketBuf];
	S32 gain_o, agc_o, agc_o_last;
    S32 adjust_gain;
    S32 attackdecay_gain;
    S16 agc_rate_cntr;  
    S16 agc_packet_cntr;		
	S16 block_size;
	S16	one_over_blocksize;
} AGC_VO_STATE;

typedef struct AGC_VO_INSTANCE_s
{
  DSP_ALIGN8 AGC_VO_NVKEY NvKey;
  DSP_ALIGN8 AGC_VO_STATE Scratch_Memory;  
} AGC_VO_INSTANCE_t;

extern int get_agc_memsize(void);
extern void AGC_VO_Init(void *p_agc_mem_ext, AGC_VO_NVKEY *agc_nvkey, S16 mod);
extern void AGC_Proc(void *p_agc_mem_ext, S16 *buf, S16 vad_in, S16 frame_size);

#endif /* _DSP_PARA_AGC_H_ */

