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


/**
 * File: app_leakage_detection_utils.h
 *
 * Description: This file defines the common structure and functions of leakage detection app.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#ifndef __APP_LEAKAGE_DETECTION_UTILS_H__
#define __APP_LEAKAGE_DETECTION_UTILS_H__

#ifdef MTK_LEAKAGE_DETECTION_ENABLE

/**
 * The current design supports VP loop playback, but now the VP of leakage detection only plays once.
 * If APP_LEAKAGE_DETECTION_VP_REPEAT_INTERVAL is defined, the loop playback of VP is controlled by
 * this app. And the APP_LEAKAGE_DETECTION_VP_REPEAT_TIMES defines the loop times.
 * If APP_LEAKAGE_DETECTION_VP_REPEAT_INTERVAL is not defined, the loop playback of VP is controlled
 * by the VP module.
 */
#define APP_LEAKAGE_DETECTION_VP_REPEAT_TIMES 3
#if 0
#define APP_LEAKAGE_DETECTION_VP_REPEAT_INTERVAL 6000
#endif
#define APP_LEAKAGE_DETECTION_VP_PLAY_TIME (10 * 1000)
#define APP_LEAKAGE_DETECTION_VP_INDEX VP_INDEX_LEAKAGE_DETECTION

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "ui_shell_activity.h"
#include "apps_config_vp_index_list.h"


/**
 *  @brief This structure defines the leakage detection app's context
 */
typedef struct {
    int32_t vp_cnt;         /**<  The VP of leakage detection remaining playback times. */
    bool in_idle;           /**<  Indicates whether the VP of leakage detection can be played or not. */
    bool is_a2dp_playing;   /**<  Indicates whether the A2DP is streaming. */
    bool need_resume_a2dp;  /**<  Indicates whether the A2DP needs to resume after VP is finished. */
    bool seal_checking;     /**<  Indicates whether the leak detection is in progress. */
    bool vp_playing;        /**<  Indicates whether the VP of leakage detection is playing. */
} app_leakage_detection_context_t;

#endif /*MTK_LEAKAGE_DETECTION_ENABLE*/

#endif /*__APP_LEAKAGE_DETECTION_UTILS_H__*/
