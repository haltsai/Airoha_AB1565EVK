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

/**
 * File: apps_config_vp_manager.h
 *
 * Description: This file defines the interface of apps_config_vp_manager.c.
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef __APPS_CONFIG_VP_MANAGER_H__
#define __APPS_CONFIG_VP_MANAGER_H__
#include "app_voice_prompt.h"

/********************************************************
 * External Function
 *
 ********************************************************/

uint16_t apps_config_set_vp_by_gpt(uint32_t vp_index, uint32_t gpt_count, app_vp_prio_t level, bool cleanup, app_vp_play_callback_t callback);

uint16_t apps_config_set_vp_preempt(uint32_t vp_index, bool need_sync, uint32_t sync_delay_time,
        app_vp_prio_t level, app_vp_play_callback_t callback);

/**
* @brief      This function requires to play a VP, there will occur preemption if the VP priority is higher than the current playing VP.
* @param[in]  tone_idx, the VP index which requires playing, refer to apps_config_vp_index_list.h.
* @param[in]  need_sync, true means that the VP needs to be synchronized to partner. Only can be true on agent.
* @param[in]  delay_time, after the time, agent and partner play the VP at the same time.
* @param[in]  level, the priority of the VP.
* @param[in]  cleanup, if true, will empty all queueing VP and reject any new VP from now on. This flag should be used only when system power off!!!
* @param[in]  callback, feedback VP play result.
* @return     If the operation is completed successfully, the return value is an ID(should > 0) which use in stop procedure, otherwise 0 means fail.
*/
uint16_t apps_config_set_vp(uint32_t vp_index, bool need_sync, uint32_t sync_delay_time, app_vp_prio_t level, bool cleanup, app_vp_play_callback_t callback);

/**
* @brief      This function requires to stop a VP by index, no matter the VP is playing or waiting to play.
* @param[in]  id, the id returned by apps_config_set_vp.
* @param[in]  need_sync, if the stopping requirement needs to be synchronized to partner. Only can be true on agent.
* @param[in]  delay_time, after the time, agent and partner stop at the same time.
* @return     If the operation is completed successfully, return true, otherwise return false.
*/
bool apps_config_stop_vp(uint16_t id, bool need_sync, uint32_t sync_delay_time);

/**
* @brief      This function requires to play a ringtone or voice broadcast.
* @param[in]  tone_idx is ringtone or voice broadcast index which requires playing, refer to apps_config_vp_index_list.h.
* @param[in]  need_sync, true means that ringtone or voice broadcast needs to be synchronized to partner. Only can be true on agent.
* @param[in]  delay_time, after the time, agent and partner play the ringtone or voice broadcast at the same time.
* @param[in]  level, the priority of the ringtone or voice broadcast.
* @param[in]  ringtone, if the playing requirement is a ringtone, otherwise is a voice broadcast.
* @param[in]  repeat, the ringtone or voice broadcast needs to be repeated or not.
* @param[in]  callback, feedback play result.
* @return     If the operation is completed successfully, the return value id an ID which must be larger than 0.
*/
uint16_t apps_config_set_voice(uint32_t vp_index, bool need_sync, uint32_t sync_delay_time, app_vp_prio_t level, bool ringtone, bool repeat, app_vp_play_callback_t callback);

/**
* @brief      This function requires to stop a ringtone or voice broadcast.
* @param[in]  need_sync, if the stopping requirement needs to be synchronized to partner. Only can be true on agent.
* @param[in]  delay_time, after the time, agent and partner will stop at the same time.
* @param[in]  ringtone, stop ringtone if true, otherwise stop voice broadcast.
* @return     If the operation is completed successfully, return true, otherwise return false.
*/
bool apps_config_stop_voice(bool need_sync, uint32_t sync_delay_time,  bool ringtone);

/**
* @brief      This function requires to set VP language.
* @param[in]  lang_index, an index in language support array.
* @param[in]  need_sync, it means that the language setting needs to be synchronized to partner.
* @return     If the operation is completed successfully, return true, otherwise return false.
*/
bool apps_config_set_vp_lang(uint8_t lang_index, bool need_sync);

#endif /* __APPS_CONFIG_VP_MANAGER_H__ */
