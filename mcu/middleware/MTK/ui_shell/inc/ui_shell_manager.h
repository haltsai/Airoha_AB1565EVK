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

#ifndef __UI_SHELL_MANANGER_H__
#define __UI_SHELL_MANANGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ui_shell_activity.h"

/**
 * @addtogroup Ui_shell_Group UI Shell
 * @{
 */


/** @defgroup ui_shell_enum Enum
  * @{
  */

/** @brief This enum defines the return type of UI Shell APIs. */
typedef enum {
    UI_SHELL_STATUS_OUT_OF_RESOURCE = -3,    /**< Out of resources. */
    UI_SHELL_STATUS_INVALID_PARAMETER = -2,  /**< The parameter is not correct. */
    UI_SHELL_STATUS_INVALID_STATE = -1,      /**< Operation is not supported in the current state. For example, the UI shell may not have started. */
    UI_SHELL_STATUS_OK = 0,                /**< The operation was successful. */
} ui_shell_status_t;

/**
  * @}
  */

/** @brief start ui shell task
 *  @return UI_SHELL_STATUS_INVALID_STATE when the UI shell has started.
 *          UI_SHELL_STATUS_OUT_OF_RESOURCE if creating a task or malloc memory fails.
 */
ui_shell_status_t ui_shell_start(void);

/**
 * @brief Stop the UI shell task and destroy the data in the UI shell.
 * @return UI_SHELL_STATUS_INVALID_STATE if the UI shell has not started.
 */
ui_shell_status_t ui_shell_finish(void);

/**
 * @brief set the proc_event function of the pre-proc activity. The function should be called before the UI shell has started.
 * @param[in] proc_event is the proc event callback of the pre-proc activity.
 * @return UI_SHELL_STATUS_INVALID_STATE if the UI shell has started.
 *         UI_SHELL_STATUS_OUT_OF_RESOURCE if malloc memory failed.
 */
ui_shell_status_t ui_shell_set_pre_proc_func(ui_shell_proc_event_func_t proc_event);

/**
 * @brief start an activity
 * @param[in] self is the caller of the functions. The value could be NULL when idle activities are started by the system during the system initialization stage.
 * @param[in] proc_event is the proc event callback of the target activity that must be started.
 * @param[in] priority is the priority of the target activity to be started.
 * @param[in] extra_data is the data that is received with the ON_CREATE event by target activity.
 * @param[in] data_len is the data length of the extra data. If data_len > 0, the UI shell automatically frees up extra data.
 * @return UI_SHELL_STATUS_INVALID_STATE if the UI shell has not started and the priority is greater than ACTIVITY_PRIORITY_IDLE_TOP.
 *         UI_SHELL_STATUS_INVALID_PARAMETER if proc_event is NULL or the priority is out of range
 *         UI_SHELL_STATUS_OUT_OF_RESOURCE if it cannot create a new activity.
 */
ui_shell_status_t ui_shell_start_activity(ui_shell_activity_t *self,
        ui_shell_proc_event_func_t proc_event,
        ui_shell_activity_priority_t priority,
        void *extra_data,
        size_t data_len);

/**
 * @brief destroy an existing activity
 * @param[in] self calls the functions.
 * @param[in] target_activity is the pointer to the activity that must be destroyed.
 * @return UI_SHELL_STATUS_INVALID_STATE if the UI shell has not started.
 */
ui_shell_status_t ui_shell_finish_activity(ui_shell_activity_t *self,
        ui_shell_activity_t *target_activity);

/**
 * @brief destroy all transient activities; The idle top activity then becomes the top activity in the stack.
 * @param[in] self calls the functions.
 * @return UI_SHELL_STATUS_INVALID_STATE if the UI shell has not started.
 */
ui_shell_status_t ui_shell_back_to_idle(ui_shell_activity_t *self);

/**
 * @brief send data to an activity which starts the current activity.
 * @param[in] self calls the functions.
 * @param[in] data is the data that is sent to the target activity.
 * @param[in] data_len is the data length of the data.
 * @return UI_SHELL_STATUS_INVALID_STATE if the UI shell has not started.
 *         UI_SHELL_STATUS_INVALID_PARAMETER if the parameter is not correct (e.g., the starter activity of self activity is NULL).
 */
ui_shell_status_t ui_shell_set_result(ui_shell_activity_t *self,
        void *data, size_t data_len);

/**
 * @brief Request the UI shell to send an ON_REFRESH event to the target activity.
 * @param[in] self calls the functions.
 * @param[in] target is the activity that receives the ON_REFRESH event.
 *            If the target activity is not at the top of the stack, the ON_REFRESH event is not sent.
 * @return UI_SHELL_STATUS_INVALID_STATE if the UI shell has not started.
 */
ui_shell_status_t ui_shell_refresh_activity(ui_shell_activity_t *self,
        ui_shell_activity_t *target);

/**
 * @brief Send an allowance request to the UI shell. The UI shell sends it to all active activities.
 *        The activities can return true to immediately allow it, or use ui_shell_grant_allowance()
 *        to allow it later.
 * @param[in] self calls the functions.
 * @param[in] request_id is the id of the allowance request. It may be a enum defined in the APP layer.
 * @return UI_SHELL_STATUS_INVALID_STATE if the UI shell has not started.
 *         UI_SHELL_STATUS_INVALID_PARAMETER if self is NULL.
 */
ui_shell_status_t ui_shell_request_allowance(ui_shell_activity_t *self,
        uint32_t request_id);

/**
 * @brief If an activity temporarily did not allow the request when receive EVENT_ID_SHELL_SYSTEM_ON_GET_ALLOWN,
 *          it can call the function when it allows the request at a later time.
 * @param[in] self calls the functions.
 * @param[in] request_id is the id of the allowance request. It may be an enum defined in the APP layer.
 * @return UI_SHELL_STATUS_INVALID_STATE if the UI shell has not started.
 *         UI_SHELL_STATUS_INVALID_PARAMETER if self is NULL.
 */
ui_shell_status_t ui_shell_grant_allowance(ui_shell_activity_t *self,
        uint32_t request_id);

/**
 * @brief Send an event to the UI shell. The UI shell dispatches it to activities after a delay.
 * @param[in] from_isr must be set to true if it calls in ISR.
 * @param[in] priority is the priority of the event.
 * @param[in] event_group is the group value of the event.
 * @param[in] event_id is the id of the event.
 * @param[in] data is the extra data that is sent to activities with the event.
 * @param[in] data_len is the length of data. When data_len > 0, the UI automatically frees the data.
 * @param[in] special_free_extra_func is useful when data_len > 0. The UI shell calls it before it frees data.
 * @param[in] delay_ms is the delay time to dispatch the event to activities.
 * @return UI_SHELL_STATUS_INVALID_STATE if the UI shell has not started.
 *         UI_SHELL_STATUS_INVALID_PARAMETER if the priority is out of range.
 */
ui_shell_status_t ui_shell_send_event(bool from_isr,
        ui_shell_event_priority_t priority,
        uint32_t event_group,
        uint32_t event_id,
        void *data,
        size_t data_len,
        void (*special_free_extra_func)(void),
        uint32_t delay_ms);

/**
 * @brief Remove all unprocessed events which match the event group and the event id from the event list.
 * @param[in] event_group the event_group of events that must be removed.
 * @param[in] event_id the event_id of events that must be removed.
 * @return UI_SHELL_STATUS_INVALID_STATE if the UI shell has not started.
 */
ui_shell_status_t ui_shell_remove_event(uint32_t event_group,
        uint32_t event_id);

/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* __UI_SHELL_MANANGER_H__ */
