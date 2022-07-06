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
#ifndef _DSP_PARA_WIND_H_
#define _DSP_PARA_WIND_H_

#include "types.h"

/**
 * @brief Parameter for DSP Wind detection algorithm
 * @KeyID 0xE300
 */

typedef struct stru_dsp_wind_para_s
{
    //For algorithm
    U8 ENABLE;                      /**< @Value   0x01 @Desc 1 */
    U8 REVISION;                    /**< @Value   0x01 @Desc 1 */
    S16 _reserved0;                 /**< @Value      0 @Desc 0 */

    S16 option;                     /**< @Value      1 @Desc 0 for regular det mode.; 1 for continuous det mode.*/
    S16 w_thr0;                     /**< @Value  20000 @Desc 0.7; smooth wind first thr. (0 ~ 32767)        */
    S16 w_thr1;                     /**< @Value   9000 @Desc 0.25; smooth wind sec thr.  (0 ~ 32767)        */
    S16 wind_sm_len;                /**< @Value     30 @Desc smooth len for wind. (1 ~ 30)                  */
    S16 egy_sm_len;                 /**< @Value     30 @Desc smooth len for energy. (1 ~ 30)                */
    S16 vad_leave;                  /**< @Value     80 @Desc how many fr does RNN predict for each decision */
    S32 c_thr0;                     /**< @Value  16000000 @Desc smooth energy first thr                        */
    S32 c_thr1;                     /**< @Value   8500 @Desc smooth energy sec thr                          */
    S32 c_thr2;                     /**< @Value   6500 @Desc smooth energy thrd thr                         */


    //For middleware
    U16 _reserved;                  /**< @Value     0  @Desc Set 1 to print detect result*/
    S16 attenuation;                /**< @Value -3000  @Desc attenuation value of 0.01db*/
    U16 attack_count;               /**< @Value    30  @Desc attack counter for wind detection, unit:15ms */
    U16 release_count;              /**< @Value  2000  @Desc release counter for non-wind detection, unit:15ms */

} PACKED DSP_PARA_WIND_STRU;


#endif /* _DSP_PARA_WIND_H_ */
