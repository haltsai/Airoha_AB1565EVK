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

#ifndef __APP_LINE_IN_UTLS_H__
#define __APP_LINE_IN_UTLS_H__

#include "app_line_in_idle_activity.h"
#include "apps_events_event_group.h"
#include "ui_shell_manager.h"
#include "ui_shell_activity.h"
#include "apps_events_event_group.h"
#include "apps_events_key_event.h"
#include "apps_events_interaction_event.h"
#include "apps_config_event_list.h"
#include "race_cmd_hostaudio.h"
#include "audio_sink_srv_line_in.h"
#include "bt_sink_srv.h"
#include "apps_debug.h"
#include "nvdm.h"
#include "nvdm_id_list.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "hal_gpio.h"
#include "hal_eint.h"
#include "hal_nvic.h"
#include "stdlib.h"
#include "hal_pinmux_define.h"
#include "semphr.h"
#include "apps_race_cmd_event.h"
#include "audio_transmitter_control.h"

#define LINE_IN_LOG_I(msg, ...)     APPS_LOG_MSGID_I("[LINE_IN_MMI]"msg, ##__VA_ARGS__)
#define LINE_IN_LOG_E(msg, ...)     APPS_LOG_MSGID_E("[LINE_IN_MMI]"msg, ##__VA_ARGS__)
#define LINE_IN_LOG_D(msg, ...)     APPS_LOG_MSGID_D("[LINE_IN_MMI]"msg, ##__VA_ARGS__)

/**
 * The port used to detect the line-in plugin or plugout.
 * For high value means this is no line-in.
 * For low value means line-in already exist.
 */
extern const char BSP_LINE_IN_DET_PIN;
extern const unsigned char BSP_LINE_IN_EINT;
extern const char BSP_LINE_IN_SWITCH_PIN;
#define LINE_IN_DETECT_PORT             BSP_LINE_IN_DET_PIN
#define LINE_IN_DETECT_EINT_NUM         BSP_LINE_IN_EINT
#define LINE_IN_DETECT_EINT_FUNCTION    HAL_GPIO_13_EINT13

#ifdef LINE_IN_DET_WITH_HIGH_LEVEL
#define LINE_IN_ALREADY_PLUG_IN         HAL_GPIO_DATA_HIGH
#define LINE_IN_ALREADY_PLUG_OUT        HAL_GPIO_DATA_LOW
#else
#define LINE_IN_ALREADY_PLUG_IN         HAL_GPIO_DATA_LOW
#define LINE_IN_ALREADY_PLUG_OUT        HAL_GPIO_DATA_HIGH
#endif

#define AUDIO_PATH_ANALOG_SW_PORT       BSP_LINE_IN_SWITCH_PIN
#ifdef LINE_IN_ENABLE_WITH_LOW_LEVEL
#define AUDIO_PATH_ANALOG_MIC_IN        HAL_GPIO_DATA_HIGH
#define AUDIO_PATH_ANALOG_LINE_IN       HAL_GPIO_DATA_LOW
#else
#define AUDIO_PATH_ANALOG_MIC_IN        HAL_GPIO_DATA_LOW
#define AUDIO_PATH_ANALOG_LINE_IN       HAL_GPIO_DATA_HIGH
#endif

#define LINE_IN_SCENARIO_TYPE_COMMON 1
#define LINE_IN_SCENARIO_TYPE_MIX 0

/**
* @brief      This function is used to strigger the start or stop line in function.
* @param[in]  path, the destination audio path.
* @return     None.
*/
void app_line_in_switch(app_audio_path_t path);

/**
* @brief      This function is used to start or stop line in function immedialy.
* @param[in]  path, the destination audio path.
* @return     None.
*/
void app_line_in_start_stop(app_audio_path_t audio_path);

/**
* @brief      This function is used to register callback to audio manager.
* @return     None.
*/
void app_line_in_am_init(void);

/**
* @brief      This function is used to handle the event which come from audio manager.
* @param[in]  event, the AM event.
* @return     None.
*/
void app_line_in_handle_am_event(vendor_se_event_t event);

/**
* @brief      This function is used to handle the event which come from audio transmitter.
* @param[in]  event, the transmitter event.
* @return     None.
*/
void app_line_in_transmitter_event(uint16_t event);

/**
* @brief      This function is used to handle the bt sink event
* @param[in]  event_id, current event id.
* @param[in]  extra_data, the data of event.
* @param[in]  data_len, the data length.
* @return     None.
*/
void app_line_in_handle_sink_event(uint32_t event_id, void *extra_data, size_t data_len);

/**
* @brief      This function is used to init the line in mix.
* @return     None.
*/
void app_line_in_mix_init(void);

/**
* @brief      This function is used to start or stop the line in mix.
* @param[in]  path, if the input path is LINE_IN means start, BT means stop.
* @return     None.
*/
void app_line_in_mix_start_stop(app_audio_path_t path);

/**
* @brief      This function is used to init line out.
* @return     None.
*/
void app_line_out_init(void);

/**
* @brief      This function is used to start or stop the line out.
* @param[in]  path, if the input path is LINE_IN means start, BT means stop.
* @return     None.
*/
void app_line_out_start_stop(app_audio_path_t path);

/**
* @brief      This function is used to get the current volume.
* @return     current volume.
*/
uint8_t app_line_in_get_volume(void);

/**
* @brief      This function is used to line in's volume.
* @param[in]  volue, the destination volume.
* @return     None.
*/
void app_line_in_set_volume(uint8_t volume);

void app_line_in_out_audio_source_ctrl_init();

void app_line_out_request_source();

void app_line_in_request_source();

void app_line_out_release_source();

void app_line_in_release_source();

void app_line_in_audio_source_event(uint32_t event);

void app_line_out_set_mute_unmute();

void app_line_out_set_unmute();

#endif

