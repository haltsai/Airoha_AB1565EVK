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

#ifndef __UI_SHELL_ACITIVITY_H__
#define __UI_SHELL_ACITIVITY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 *@addtogroup Ui_shell_Group UI Shell
 *@{
 * This section introduces the ui_shell interface APIs including terms and acronyms, supported features, software architecture, details on how to use this interface, ui_shell function groups, enums, structures and functions.
 *
 * @section ui_shell_Terms_Chapter Terms and acronyms
 *
 * |        Terms         |           Details                |
 * |----------------------|----------------------------------|
 * |\b Activity           | A structure that must be implemented by the AP layer to process events. |
 * |\b Pre-proc Activity  | A type of activity that is unique in a project. It pre-processes all events. |
 * |\b Idle Activity      | A type of activity that is normally created when the system starts. |
 * |\b Transient Activity | A type of activity that is created when running. |
 */


/** @defgroup ui_shell_enum Enum
  * @{
  */

/** @brief
 * This enum defines the type of activities.
 */
typedef enum {
    UI_SHELL_PRE_PROC_ACTIVITY = 0,         /**< Defines the activity type as pre-proc. */
    UI_SHELL_IDLE_ACTIVITY,                 /**< Defines the activity type as idle. */
    UI_SHELL_TRANSIENT_ACTIVITY,            /**< Defines the activity type as transient. */
} ui_shell_activity_type_t;

/** @brief
 * This enum defines the priority of activities. When an event is processed by the activties stack, an activity with a higher priority
 * will process the event before other activities with a lower priority.
 */
typedef enum {
    ACTIVITY_PRIORITY_IDLE_BACKGROUND = 0,  /**< The priority is idle background; Only idle activities use it. */
    ACTIVITY_PRIORITY_IDLE_TOP,             /**< The priority is idle top; Only one idle activity in complete project uses it.*/
    ACTIVITY_PRIORITY_LOWEST,               /**< The priority is lowest; Only transient activities use it. */
    ACTIVITY_PRIORITY_LOW,                  /**< The priority is low; Only transient activities use it. */
    ACTIVITY_PRIORITY_MIDDLE,               /**< The priority is middle; Only transient activities use it. */
    ACTIVITY_PRIORITY_HIGH,                 /**< The priority is high; Only transient activities use it. */
    ACTIVITY_PRIORITY_HIGHEST,              /**< The priority is highest; Only transient activities use it. */
} ui_shell_activity_priority_t;

/** @brief
 * This enum defines the event group number.
 */
typedef enum {
    EVENT_GROUP_UI_SHELL_SYSTEM = 0,        /**< ui_shell system group for UI shell internal use. */
    EVENT_GROUP_UI_SHELL_APP_BASE,          /**< The application can define some event group numbers which must not be smaller than the base value. */
} ui_shell_event_group_t;

/** @brief
 * This enum defines the event ids of the UI shell group.
 */
enum {
    EVENT_ID_SHELL_SYSTEM_ON_CREATE,            /**< Event id of an activity that is created and will be added.*/
    EVENT_ID_SHELL_SYSTEM_ON_DESTROY,           /**< Event id of an activity that is being destroyed. The activity is removed from the activities stack and destroyed. */
    EVENT_ID_SHELL_SYSTEM_ON_RESUME,            /**< Event id of an activity that is resuming. */
    EVENT_ID_SHELL_SYSTEM_ON_PAUSE,             /**< Event id of an activity that is pausing. */
    EVENT_ID_SHELL_SYSTEM_ON_REFRESH,           /**< Event id of an activity that is refreshing. */
    EVENT_ID_SHELL_SYSTEM_ON_RESULT,            /**< Event id of an activity that is receiving a result. */
    EVENT_ID_SHELL_SYSTEM_ON_REQUEST_ALLOWANCE, /**< Event id of an activity that is receiving an allowance request. */
    EVENT_ID_SHELL_SYSTEM_ON_ALLOW,             /**< Event id of an allowance request is allowed. */
};

/** @brief
 * This enum defines the priority of events. An event with higher priority is processed before other events with a lower priority.
 */
typedef enum {
    EVENT_PRIORITY_LOWEST = 0,              /**< The priority is lowest. */
    EVENT_PRIORITY_LOW,                     /**< The priority is low. */
    EVENT_PRIORITY_MIDDLE,                  /**< The priority is in the middle. */
    EVENT_PRIORITY_HIGH,                    /**< The priority is high. */
    EVENT_PRIORITY_HIGNEST,                 /**< The priority is highest. */
    EVENT_PRIORITY_UI_SHELL_SYSTEM,         /**< The priority is the UI shell system internal. It is only used by the UI shell. */
} ui_shell_event_priority_t;

/**
  * @}
  */

/** @defgroup ui_shell_struct Structures
  * @{
  */

/** @brief This type defines the activity structure. */
typedef struct _ui_shell_activity ui_shell_activity_t;

/**
  * @}
  */

/** @defgroup ui_shell_typedef Typedef
  * @{
  */

/** @brief  This defines the callback function prototype. Each activity should have its own event proc
 *  function. The proc_event_func is called if the corresponding activity receives an event.
 *  @param [in] self: The pointer to the activity self.
 *  @param [in] event_group: Event group value.
 *  @param [in] event_id: Event id value.
 *  @param [in] extra_data: Extra data of the event.
 *  @param [in] data_len: Extra data length of the event.
 *  @return     For EVENT_GROUP_UI_SHELL_SYSTEM events, the return value is useful when an activity processes EVENT_ID_SHELL_SYSTEM_ON_CREATE or EVENT_ID_SHELL_SYSTEM_ON_DESTROY. If it returns false, the stack will not change.\n
 *              For other event groups, if it returns false, the event is sent to the next activity in the activities stack. \n
 */
typedef bool (*ui_shell_proc_event_func_t)(
        ui_shell_activity_t *self,
        uint32_t event_group,
        uint32_t event_id,
        void *extra_data,
        size_t data_len);

/**
  * @}
  */

/** @defgroup ui_shell_struct Structures
  * @{
  */

/** @brief This structure defines the activity structure. The activity structure is malloced by the UI shell. */
struct _ui_shell_activity {
    void *local_context;                        /**< The local context of the activity. The user can assign the pointer of a self-defined variable to local_context. */
};

/**
  * @}
  */

/**
  * @}
  */
#ifdef __cplusplus
}
#endif

#endif /* __UI_SHELL_ACITIVITY_H__ */
