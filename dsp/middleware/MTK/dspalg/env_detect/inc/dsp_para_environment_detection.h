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
#ifndef _DSP_PARA_ENVIRONMENT_DETECTION_H_
#define _DSP_PARA_ENVIRONMENT_DETECTION_H_

#include "types.h"

#define DSP_ANC_ENVIRONMENT_DETECTION_LEVEL_MAXIMUM_NUMBER 4

typedef struct dsp_environment_detection_level_s
{
    S16 release_threshold;
    S16 attack_threshold;
    S16 attenuation_ff;
    S16 attenuation_fb;
} dsp_environment_detection_level_t;

/**
 * @brief Parameter for DSP noise gate algorithm
 * @KeyID 0xE302 NVKEY_DSP_PARA_ENVIRONMENT_DETECTION
 */

typedef struct stru_dsp_environment_detection_para_s
{
    //For algorithm
    U16 WB_NR_FAST_ALPHA;                   /**< @Value 0x747B @Desc 1 */
    U16 WB_NR_SLOW_ALPHA;                   /**< @Value 0x7C29 @Desc 1 */
    U16 WB_NR_NOISE_UPDATE_FAST;            /**< @Value 655    @Desc 1 */
    U16 WB_NR_NOISE_UPDATE_SLOW;            /**< @Value 328    @Desc 1 */
    U16 WB_NR_NOISE_UPDATE_ULTRASLOW;       /**< @Value 131    @Desc 1 */

    U16 WB_NR_RX_POW_MIN_BUF_PERIOD;        /**< @Value 0x000A @Desc 1 */
    U16 WB_NR_RX_VAD_THRD1;                 /**< @Value 0x0348 @Desc 1 */
    U16 WB_NR_RX_VAD_THRD_BANDS_0;          /**< @Value 0x0907 @Desc 1 */
    U16 WB_NR_RX_VAD_THRD_BANDS_1;          /**< @Value 0x0707 @Desc 1 */
    U16 WB_NR_RX_VAD_THRD_BANDS_2;          /**< @Value 0x0808 @Desc 1 */
    U16 WB_NR_RX_VAD_THRD_BANDS_3;          /**< @Value 0x0808 @Desc 1 */
    U16 WB_NR_RX_VAD_THRD_BANDS_4;          /**< @Value 0x0a07 @Desc 1 */
    U16 WB_NR_RX_NOISE_LAMDA;               /**< @Value 0x799a @Desc 1 */
    U16 WB_NR_RX_NOISE_FLOOR_MIN;           /**< @Value 0x8400 @Desc 1 */

    S16 NE_MIN_DB;                          /**< @Value 0xB800 @Desc 1 */
    U16 NE_ALPHA_DB;                        /**< @Value 0x028F @Desc 1 */
    U16 NE_VAD_THRA;                        /**< @Value 0x0015 @Desc 1 */
    U16 NE_STARTBIN;                        /**< @Value 0x0003 @Desc 1 */
    U16 NE_ENDBIN;                          /**< @Value 0x00DF @Desc 1 */

    U16 NE_ALPHA_DB_DOWN;                   /**< @Value 0x00A4 @Desc 1 */
    U16 RESERVE_2;                          /**< @Value 0x0000 @Desc 1 */
    U16 RESERVE_3;                          /**< @Value 0x0000 @Desc 1 */
    U16 RESERVE_4;                          /**< @Value 0x0000 @Desc 1 */
    U16 RESERVE_5;                          /**< @Value 0x0000 @Desc 1 */
    U16 RESERVE_6;                          /**< @Value 0x0000 @Desc 1 */
    U16 RESERVE_7;                          /**< @Value 0x0000 @Desc 1 */
    U16 RESERVE_8;                          /**< @Value 0x0000 @Desc 1 */
    U16 RESERVE_9;                          /**< @Value 0x0000 @Desc 1 */



    //For middleware
    U8  enable;                             /**< @Value     1  @Desc Set 1 to enable proceduret*/
    U8  print_result;                       /**< @Value     0  @Desc Set 1 to print detect result*/
    U16 level_numbers;                      /**< @Value     2  @Desc the number of level gate */
    U16 attack_count;                       /**< @Value   300  @Desc attack counter for detection, unit:15ms */
    U16 release_count;                      /**< @Value   150  @Desc release counter for detection, unit:15ms */
    U16 pause_attenuation;                  /**< @Value 0x0000 @Desc pause Environment detection attenuation */
    U16 _reserved_02;                       /**< @Value 0x0000 @Desc reserved */
    union {
        dsp_environment_detection_level_t dsp_environment_detection_levels[DSP_ANC_ENVIRONMENT_DETECTION_LEVEL_MAXIMUM_NUMBER];
        struct {
            S16 level_1_release_threshold;      /**< @Value    -12000   @Desc Set the release threshold of gate level 1, threshold = 256*db */
            S16 level_1_attack_threshold;       /**< @Value    -13500   @Desc Set the attack threshold of gate level 1, threshold = 256*db  */
            S16 level_1_attenuation_ff;         /**< @Value         0   @Desc Set FF attenuation of gate level 1. units: 0.01db             */
            S16 level_1_attenuation_fb;         /**< @Value     -2000   @Desc Set FB attenuation of gate level 1. units: 0.01db             */

            S16 level_2_release_threshold;      /**< @Value     -8000   @Desc Set the release threshold of gate level 2, threshold = 256*db */
            S16 level_2_attack_threshold;       /**< @Value     -8800   @Desc Set the attack threshold of gate level 2, threshold = 256*db  */
            S16 level_2_attenuation_ff;         /**< @Value         0   @Desc Set FF attenuation of gate level 2. units: 0.01db             */
            S16 level_2_attenuation_fb;         /**< @Value      -600   @Desc Set FB attenuation of gate level 2. units: 0.01db             */

            S16 level_3_release_threshold;      /**< @Value         0   @Desc Set the release threshold of gate level 3, threshold = 256*db */
            S16 level_3_attack_threshold;       /**< @Value         0   @Desc Set the attack threshold of gate level 3, threshold = 256*db  */
            S16 level_3_attenuation_ff;         /**< @Value         0   @Desc Set FF attenuation of gate level 3. units: 0.01db             */
            S16 level_3_attenuation_fb;         /**< @Value         0   @Desc Set FB attenuation of gate level 3. units: 0.01db             */

            S16 level_4_release_threshold;      /**< @Value         0   @Desc Set the release threshold of gate level 4, threshold = 256*db */
            S16 level_4_attack_threshold;       /**< @Value         0   @Desc Set the attack threshold of gate level 4, threshold = 256*db  */
            S16 level_4_attenuation_ff;         /**< @Value         0   @Desc Set FF attenuation of gate level 4. units: 0.01db             */
            S16 level_4_attenuation_fb;         /**< @Value         0   @Desc Set FB attenuation of gate level 4. units: 0.01db             */

        } environment_detection_level_struct;
    };
} PACKED DSP_PARA_ENVIRONMENT_DETECTION_STRU;

//   <Value>0x7B74297C8F02480183000A0048030709070708080808070A9A79008400B88F0215000300DF00A40000000000000000000000000000000000010002002C01960000000000000044CB000030F80000A0DD0000A8FD00000000000000000000000000000000</Value>


#endif /* _DSP_PARA_ENVIRONMENT_DETECTION_H_ */

/*
Please sync the following files
\dsp\middleware\MTK\dspalg\env_detect\inc\dsp_para_environment_detection.h
\mcu\middleware\MTK\audio\port\ab156x\inc\nvdm\dsp_para_environment_detection.h
*/

