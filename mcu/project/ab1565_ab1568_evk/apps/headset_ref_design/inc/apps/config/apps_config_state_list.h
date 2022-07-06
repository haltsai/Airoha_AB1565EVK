/* Copyright Statement:
 *
 * (C) 2018  Airoha Technology Corp. All rights reserved.
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

#ifndef __APPS_CONFIG_STATE_LIST_H__
#define __APPS_CONFIG_STATE_LIST_H__

typedef enum {
    APP_BT_OFF,                            //0
    APP_DISCONNECTED,
    APP_CONNECTABLE,
    APP_CONNECTED,
    APP_HFP_INCOMING,
    APP_HFP_OUTGOING,                   //5
    APP_HFP_CALLACTIVE,
    APP_HFP_CALLACTIVE_WITHOUT_SCO,
    APP_HFP_TWC_INCOMING,
    APP_HFP_TWC_OUTGOING,
    APP_HFP_MULTITPART_CALL,                    //10
    //APP_FAKEON,
    //APP_FAKEOFF,
    //APP_DETACHING_LINK,
    //APP_TEST_MODE,
    //APP_FM,                             //15
    APP_WIRED_MUSIC_PLAY = 16,
    //APP_UPDATING,
    //APP_UPDATED_OK,
    //APP_UPDATED_FAIL,
    //APP_VOICE_PROMPT_LANG_SELECT,       //20
    //APP_TWS_PAIRING,
    //APP_INQUIRY,
    //APP_MP_STATE,
    APP_A2DP_PLAYING = 24,
    APP_STATE_HELD_ACTIVE,                //25
    APP_STATE_FIND_ME,
    /**
     * @brief Add for VA to ignore some key event when recording and thinking.
     */
    APP_STATE_VA,
    APP_ULTRA_LOW_LATENCY_PLAYING,
    APP_LE_AUDIO_BIS_PLAYING,
    APP_STATE_RESERVED5,                  //30
    APP_STATE_RESERVED6,
    APP_STATE_RESERVED7,
    APP_TOTAL_STATE_NO
} apps_config_state_t;

#endif /* __APPS_CONFIG_STATE_LIST_H__ */
