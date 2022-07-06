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
#ifndef _DSP_PARA_LD_H_
#define _DSP_PARA_LD_H_

#include "types.h"


#ifdef WIN32
    #pragma pack(1)
#endif
#define PACKED __attribute__((packed))
//#define AT_DRAM0_const __attribute__ ((section (".dram0.rodata")))


typedef struct stru_dsp_ld_para_s
{
    unsigned char ENABLE;                           /**< @Value 0x01  @Desc 1 */
    unsigned char REVISION;                         /**< @Value 0x01  @Desc 1 */
    signed short IE_DT_length;                      /**< @Value 0x0016  @Desc 1 */
    signed short ld_thrd;                           /**< @Value   4915  @Desc 1 */
    unsigned short AEC_MU_FAC;                      /**< @Value 0x399A  @Desc 1 */
    //TX REF VAD
    signed short AEC_ENG_VAD_THR;                   /**< @Value 0x0a00	 @Desc 1 */
    signed short AEC_ENG_VAD_HANG;                  /**< @Value 0x0005	 @Desc 1 */
    signed short AEC_ENG_POW_INIT;                  /**< @Value 0x0400	 @Desc 1 */
    signed short AEC_slow_up_alpha;                 /**< @Value 0x0021	 @Desc 1 */
    signed short AEC_slow_dn_alpha;                 /**< @Value 0x4000	 @Desc 1 */
    signed short AEC_fast_up_alpha;                 /**< @Value 0x6666	 @Desc 1 */
    signed short AEC_fast_dn_alpha;                 /**< @Value 0x0666	 @Desc 1 */

    unsigned short RXIN_TXREF_DELAY;                /**< @Value 0x0005  @Desc 1 */
    unsigned short MIC_EC_DELAY;                    /**< @Value 0x003C	 @Desc 1 */
    unsigned short AEC_MU_MIN;                      /**< @Value 0x1000  @Desc 1 */

    signed short DIGITAL_GAIN;                      /**< @Value 0x2000  @Desc 1 */
    unsigned short Dummy;                           /**< @Value 0x0000  @Desc 1 */
    unsigned int RESERVE_1;                         /**< @Value 0x00000000 @Desc 1 */
    unsigned int RESERVE_2;                         /**< @Value 0x00000000 @Desc 1 */
    unsigned int RESERVE_3;                         /**< @Value 0x00000000 @Desc 1 */

} PACKED DSP_PARA_LD_STRU;

//extern AT_DRAM0_const const DSP_PARA_LD_STRU LD_DspParaInit;
int get_aec_nr_memsize(void);


void LeakageDetect_Prcs(S16 *MICB, S16 *REF, S16 *NR, S32 *ld_iwxe1_power, S32 *ld_aecref1_power, S16 *fe_vad_cntr);

void LeakageDetect_Init(int NB_mode, S16 *bufin, void *p_ecnr_mem_ext, void *p_ld_NvKey);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* _DSP_PARA_LD_H_ */
