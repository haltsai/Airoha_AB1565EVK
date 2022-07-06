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
#ifndef _DSP_PARA_INS_H_
#define _DSP_PARA_INS_H_

#include "dsp_para_cpd.h"
#define DSP_INS_SCRATCH_MEMSIZE 1

/**
 * @brief Parameter for DSP INS algorithm
 * @KeyID 0xE110
 */
/*NvkeyDefine NVKEYID_DSP_ALG_PARA_INS*/
typedef struct
{
    U8  ENABLE;                            /**< @Value 0x01 @Desc 1 */
    U8  REVISION;                        /**< @Value 0x01 @Desc 1 */
    S16 AUX_INS_alpha_up;                /**< @Value 0x2000     @Desc 1 */
    S16 AUX_INS_alpha_down;                /**< @Value 0x0CCD     @Desc 1 */
    S16 AUX_INS_atten;                    /**< @Value 0x0000     @Desc 1 */
    S32 AUX_INS_thrd;                    /**< @Value 0x00000080 @Desc 1 */

    U32 RESERVE_0;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_1;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_2;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_3;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_4;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_5;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_6;                        /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_7;                        /**< @Value 0x00000000 @Desc 1 */
} PACKED INS_NVKEY_STATE;

typedef struct INS_INSTANCE_s
{
    U32 MemoryCheck;
    DSP_ALIGN8 INS_NVKEY_STATE NvKey;
    DSP_ALIGN8 U8 ScratchMemory[DSP_INS_SCRATCH_MEMSIZE]; //Set at the end of structure for dynamic size
} INS_INSTANCE_t, *INS_INSTANCE_ptr;

#endif /* _DSP_PARA_INS_H_ */
