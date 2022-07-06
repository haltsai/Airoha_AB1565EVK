/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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
 * File: app_power_save_utils.h
 *
 * Description: This file defines the interface of app_power_save_utils.c.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */

#ifndef __APP_POWER_SAVE_UTILS__
#define __APP_POWER_SAVE_UTILS__

#ifdef __cplusplus
extern "C" {
#endif

#include "ui_shell_activity.h"
#include "bt_sink_srv.h"

/**
 *  @brief This enum defines the states of Power Saving APP.
 */
typedef enum {
    POWER_SAVING_STATE_IDLE = 0,            /**<  Idle state. */
    POWER_SAVING_STATE_WAITING_TIMEOUT,     /**<  Waiting timeout state, wait timeout after power_saving mode take effect. */
    POWER_SAVING_STATE_TIMEOUT,             /**<  Timeout state, do power saving action after power_saving mode timeout. */
} app_power_saving_state_t;

/**
 *  @brief This enum defines the BT states of Power Saving APP.
 */
typedef enum {
    APP_POWER_SAVING_BT_OFF = 0,            /**<  BT off state, maybe do power saving action (SYSTEM_OFF). */
    APP_POWER_SAVING_BT_DISCONNECTED,       /**<  BT on but no EDR connection, maybe do power saving action (SYSTEM OFF or BT OFF). */
    APP_POWER_SAVING_BT_CONNECTED,          /**<  BT on and connected, no need to do power saving. */
} app_power_saving_bt_state_t;

/**
 *  @brief This enum defines the power saving type.
 */
typedef enum {
    APP_POWER_SAVING_TYPE_NO_CONNECTION = 0,    /**<  Device is not connect BT. */
    APP_POWER_SAVING_TYPE_AUDIO_SILENCE,        /**<  Device audio is silence. */
} app_power_saving_type_t;

#define APP_POWER_SAVING_ENABLED 0x0001
#define APP_POWER_SAVING_DISABLED 0X000
/**
 *  @brief This structure define the power saving configuration.
 */
typedef struct {
    uint16_t power_saving_enable;           /**<  The power saving enable flag. */
    uint16_t timeout;                       /**<  The timeout of auto power off. */
    uint16_t silence_detect_enable;         /**<  The flag of auto power off when silence detected. */
    uint16_t silence_detect_timeout;        /**<  The timeout of auto power off when silence detect. */
} app_power_saving_cfg;

/**
 *  @brief This structure defines the context of Power Saving APP.
 */
typedef struct {
    app_power_saving_state_t app_state;                 /**<  The state of Power Saving APP. */
    app_power_saving_bt_state_t bt_sink_srv_state;      /**<  BT state. */
    app_power_saving_type_t waiting_type;           /**<  The waiting type is no connection or silence detect. */
} app_power_saving_context_t;

/**
 *  @brief This enum defines the action of Power Saving APP.
 */
typedef enum {
    APP_POWER_SAVING_TARGET_MODE_NORMAL = 0,    /**<  None power saving action. */
    APP_POWER_SAVING_TARGET_MODE_BT_OFF,        /**<  Power saving action: BT OFF. */
    APP_POWER_SAVING_TARGET_MODE_SYSTEM_OFF,    /**<  Power saving action: SYSTEM OFF. */
} app_power_saving_target_mode_t;

/**
 *  @brief This enum defines the event of Power Saving group.
 */
typedef enum {
    APP_POWER_SAVING_EVENT_TIMEOUT,             /**<  Timeout event for app_power_saving_waiting_timeout_activity. */
    APP_POWER_SAVING_EVENT_REFRESH_TIME,        /**<  Refresh timer event. */
    APP_POWER_SAVING_EVENT_DISABLE_BT,          /**<  Disable BT after receiving the event. */
    APP_POWER_SAVING_EVENT_POWER_OFF_SYSTEM,    /**<  Power off system after receiving the event. */
    APP_POWER_SAVING_EVENT_NOTIFY_CHANGE,       /**<  Notify change event. */
} app_power_saving_event_t;

#define APPS_MIN_TIMEOUT_OF_SLEEP_AFTER_NO_CONNECTEION   (20 * 1000) /* The minimal waiting time (seconds) before sleep */

/**
 *  @brief This typedef defines the function pointer used to update power saving target_mode.
 */
typedef app_power_saving_target_mode_t (*get_power_saving_target_mode_func_t)(void);

/**
* @brief      This function is used to add new power saving target mode function. (internal use in Power Saving APP)
* @param[in]  callback_func, new power saving target mode function.
* @return     If return true, add new power saving target mode function successfully.
*/
bool app_power_save_utils_add_new_callback_func(get_power_saving_target_mode_func_t callback_func);

/**
* @brief      This function is used to get target_mode (current power saving action).
* @param[in]  self, current activity (be used to get context of Power Saving APP).
* @return     Return current power saving target mode.
*/
app_power_saving_target_mode_t app_power_save_utils_get_target_mode(ui_shell_activity_t *self, app_power_saving_type_t *type);

/**
* @brief      This function is used to send APP_POWER_SAVING_EVENT_NOTIFY_CHANGE event.
* @param[in]  from_isr, whether called from ISR.
* @param[in]  callback_func, only used to print log.
* @return     If return true, send event successfully.
*/
bool app_power_save_utils_notify_mode_changed(bool from_isr, get_power_saving_target_mode_func_t callback_func);

/**
* @brief      This function is used to register new power_saving_target_mode function for external APP.
* @param[in]  callback_func, new power saving target mode function.
* @return     If return true, add new power saving target mode function successfully.
*/
bool app_power_save_utils_register_get_mode_callback(get_power_saving_target_mode_func_t callback_func);

/**
* @brief      This function is used to handle BT event and update BT state of Power Saving APP.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, need to do action.
*/
bool app_power_save_utils_update_bt_state(ui_shell_activity_t *self,
                                            uint32_t event_id, void *extra_data, size_t data_len);

/**
* @brief      This function is used to refresh the waiting time.
*/
void app_power_save_utils_refresh_waiting_time(void);

/**
* @brief      This function is used to get the enable state of power saving.
* @return     If return true, the power saving is enabled.
*/
bool app_power_save_utils_is_enabled(void);

/**
* @brief      This function is used to get the timeout of power saving.
* @param[in]  type, the type of power saving waiting.
* @return     The timeout of power saving (ms).
*/
uint32_t app_power_save_utils_get_timeout(app_power_saving_type_t type);

/**
* @brief      This function is used to get the power saving configuration.
* @return     The pointer of configuration
*/
app_power_saving_cfg* app_power_saving_get_cfg(void);

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) && defined(AIR_SILENCE_DETECTION_ENABLE)
/**
* @brief      This function is notify the slave silence detect state change.
*/
void app_power_save_utils_slave_silence_detect(bool silence);
#endif

/**
* @brief      This function is used to init power saving utils.
* @return     None.
*/
void app_power_save_utils_init(void);

/**
* @brief      This function is used to set the configuration of power saving.
* @param[in]  config, the configuration of power saving.
* @return     None.
*/
int32_t app_power_save_utils_set_cfg(app_power_saving_cfg *config);

/**
* @brief      This function is used to notify app that the power saving configuration updated.
* @return     None.
*/
void app_power_save_utils_cfg_updated_notify(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_POWER_SAVE_UTILS__ */
