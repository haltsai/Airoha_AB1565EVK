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

#include "ui_shell_activity_stack.h"
#include "ui_shell_al_memory.h"
#include "ui_shell_al_log.h"
#include "ui_shell_activity_heap.h"

static ui_shell_activity_internal_t *s_acti_list = NULL;
static ui_shell_activity_internal_t *s_pre_proc_acti = NULL;
static bool s_already_started = false;

typedef struct _temp_saved_activity {
    ui_shell_activity_internal_t *acti;
    struct _temp_saved_activity *next;
} temp_saved_activity_t;

typedef struct _waiting_allowed_data {
    temp_saved_activity_t *waiting_acti_list;
    ui_shell_activity_internal_t *request_activity;
    uint32_t wait_request_id;
    struct _waiting_allowed_data *next;
} waiting_allowed_data_t;

static waiting_allowed_data_t *s_waiting_allowed_list = NULL;

void ui_shell_activity_stack_start(void)
{
    ui_shell_activity_internal_t *temp_acti = NULL;

    if (s_already_started) {
        UI_SHELL_LOG_MSGID_E("ui_shell_activity_stack_start have been called before", 0);
        return;
    }
    if (s_pre_proc_acti) {
        s_pre_proc_acti->proc_event_func(&s_pre_proc_acti->external_activity,
                EVENT_GROUP_UI_SHELL_SYSTEM,
                EVENT_ID_SHELL_SYSTEM_ON_CREATE,
                NULL, 0);
    }

    for (temp_acti = s_acti_list; temp_acti != NULL; temp_acti = temp_acti->next) {
        temp_acti->proc_event_func(&temp_acti->external_activity,
                EVENT_GROUP_UI_SHELL_SYSTEM,
                EVENT_ID_SHELL_SYSTEM_ON_CREATE,
                NULL, 0);
    }

    // Refresh the first idel activity
    if (s_acti_list) {
        s_acti_list->proc_event_func(&s_acti_list->external_activity,
                EVENT_GROUP_UI_SHELL_SYSTEM,
                EVENT_ID_SHELL_SYSTEM_ON_REFRESH,
                NULL, 0);
    }

    s_already_started = true;
}

bool ui_shell_activity_stack_add(ui_shell_activity_internal_t *activity, void *data, size_t data_len)
{
    bool ret = false;

    if (activity->activity_type == UI_SHELL_PRE_PROC_ACTIVITY) {
        if (s_pre_proc_acti) {
            UI_SHELL_LOG_MSGID_E("Cannot add 2 pre_proc_activities", 0);
        } else if (s_already_started) {
            UI_SHELL_LOG_MSGID_E("Must add pre_proc activity before start", 0);
        } else {
            s_pre_proc_acti = activity;
            ret = true;
        }
    } else { // UI_SHELL_IDLE_ACTIVITY and UI_SHELL_TRANSIENT_ACTIVITY
        ui_shell_activity_internal_t **p_acti = &s_acti_list;

        // temp_acti : which have higher priority than current activity.
        for (p_acti = &s_acti_list; *p_acti != NULL; p_acti = &(*p_acti)->next) {
            if ((*p_acti)->priority <= activity->priority) {
                // p_acti will point to the activity which have lower priority than current
                break;
            }
        }

        if (s_already_started) {
            if (*p_acti == s_acti_list && s_acti_list) { // This activity is next foreground activity
                // Pause last foreground activity
                s_acti_list->proc_event_func(&(s_acti_list->external_activity),
                        EVENT_GROUP_UI_SHELL_SYSTEM,
                        EVENT_ID_SHELL_SYSTEM_ON_PAUSE,
                        NULL, 0);
            }
            ret = activity->proc_event_func(&activity->external_activity,
                    EVENT_GROUP_UI_SHELL_SYSTEM,
                    EVENT_ID_SHELL_SYSTEM_ON_CREATE,
                    data, data_len);
            if (ret) {
                // If priority of *p_acti is lower than current, new activity is next foreground activity
                if (*p_acti == s_acti_list) {
                    activity->proc_event_func(&(activity->external_activity),
                        EVENT_GROUP_UI_SHELL_SYSTEM,
                        EVENT_ID_SHELL_SYSTEM_ON_REFRESH,
                        NULL, 0);
                }
            } else {
                UI_SHELL_LOG_MSGID_E("On create failed in activity = %x", 1, activity->proc_event_func);
                if (*p_acti == s_acti_list && s_acti_list) { // resume from pause status
                    s_acti_list->proc_event_func(&(s_acti_list->external_activity),
                        EVENT_GROUP_UI_SHELL_SYSTEM,
                        EVENT_ID_SHELL_SYSTEM_ON_RESUME,
                        NULL, 0);
                    s_acti_list->proc_event_func(&s_acti_list->external_activity,
                            EVENT_GROUP_UI_SHELL_SYSTEM,
                            EVENT_ID_SHELL_SYSTEM_ON_REFRESH,
                            NULL, 0);
                }
            }
        } else {
            // If have not started, the on create event will be called later
            ret = true;
        }

        // Add activity in list
        if (ret) {
            activity->next = (*p_acti);
            *p_acti = activity;
        }
    }
    if (!ret) {
        ui_shell_activity_heap_free_item(activity);
    }
    return ret;
}

bool ui_shell_activity_stack_remove(ui_shell_activity_internal_t *activity)
{
    bool ret = false;
    ui_shell_activity_internal_t **p_acti = &s_acti_list;
    ui_shell_activity_internal_t *temp_acti = s_acti_list;

    if (!activity || activity->activity_type != UI_SHELL_TRANSIENT_ACTIVITY) {
        UI_SHELL_LOG_MSGID_E("Cannot remove activity type is not UI_SHELL_TRANSIENT_ACTIVITY", 0);
        return ret;
    }

    // To avoid duplicate remove message for the same activity, check the activity is valid
    for (temp_acti = s_acti_list;
            temp_acti != NULL && temp_acti->activity_type == UI_SHELL_TRANSIENT_ACTIVITY;
            temp_acti = temp_acti->next) {
        if (temp_acti == activity) {
            if (activity == s_acti_list) {
                activity->proc_event_func(&(activity->external_activity),
                        EVENT_GROUP_UI_SHELL_SYSTEM,
                        EVENT_ID_SHELL_SYSTEM_ON_PAUSE,
                        NULL, 0);
            }

            ret = activity->proc_event_func(&(activity->external_activity),
                EVENT_GROUP_UI_SHELL_SYSTEM,
                EVENT_ID_SHELL_SYSTEM_ON_DESTROY,
                NULL, 0);
            break;
        }
    }

    if (ret) {
        for (p_acti = &s_acti_list; *p_acti != NULL; p_acti = &(*p_acti)->next) {
            if ((*p_acti)->started_by == activity) {
                (*p_acti)->started_by = NULL;
                UI_SHELL_LOG_MSGID_W("activity %x(%x)'s branch %x(%x) is detroied", 4,
                        (uint32_t)(*p_acti), (uint32_t)(*p_acti)->proc_event_func,
                        (uint32_t)activity, (uint32_t)activity->proc_event_func);
            }

            if (*p_acti == activity) {
                *p_acti = activity->next;
            }
        }

        if (activity->next == s_acti_list && s_acti_list) { //activity has been removed from list
            s_acti_list->proc_event_func(&(s_acti_list->external_activity),
                    EVENT_GROUP_UI_SHELL_SYSTEM,
                    EVENT_ID_SHELL_SYSTEM_ON_RESUME,
                    NULL, 0);
            s_acti_list->proc_event_func(&(s_acti_list->external_activity),
                    EVENT_GROUP_UI_SHELL_SYSTEM,
                    EVENT_ID_SHELL_SYSTEM_ON_REFRESH,
                    NULL, 0);
        }
        ui_shell_activity_heap_free_item(activity);
    } else {
        UI_SHELL_LOG_MSGID_I("Activity refused remove %x(%x)", 2, (uint32_t)activity,
                (uint32_t)activity->proc_event_func);
    }
    return ret;
}

void ui_shell_activity_stack_go_to_idle(void)
{
    ui_shell_activity_internal_t **p_acti = &s_acti_list;
    ui_shell_activity_internal_t *removed_list = NULL;
    ui_shell_activity_internal_t *temp_acti = s_acti_list;
    ui_shell_activity_internal_t *temp_removed = s_acti_list;
    bool ret = false;

    for (ret = true, p_acti = &s_acti_list;
            *p_acti != NULL && (*p_acti)->activity_type == UI_SHELL_TRANSIENT_ACTIVITY;
            p_acti = &(*p_acti)->next) {
        if ((*p_acti) == s_acti_list) {
            (*p_acti)->proc_event_func(&(*p_acti)->external_activity,
                    EVENT_GROUP_UI_SHELL_SYSTEM,
                    EVENT_ID_SHELL_SYSTEM_ON_PAUSE,
                    NULL, 0);
        }

        ret = (*p_acti)->proc_event_func(&(*p_acti)->external_activity,
                EVENT_GROUP_UI_SHELL_SYSTEM,
                EVENT_ID_SHELL_SYSTEM_ON_DESTROY,
                NULL, 0);
        if (!ret) {
            UI_SHELL_LOG_MSGID_I("temp_acti refused destroy command, pointer = %x", 1, (*p_acti)->proc_event_func);
            break;
        }
    }

    if (*p_acti != s_acti_list) { // Removed at least one activity
        removed_list = s_acti_list;
        s_acti_list = *p_acti;
        *p_acti = NULL; // Separate removed list from s_acti_list

        /* Remove the "started_by" pointer of removed activities in exist activities
         * Only useful when start activity with lower priority than starter.
         * */
        for (temp_acti = s_acti_list; temp_acti != NULL && temp_acti->activity_type == UI_SHELL_TRANSIENT_ACTIVITY; temp_acti = temp_acti->next) {
            for (temp_removed = removed_list; temp_removed != NULL; temp_removed = temp_removed->next) {
                if (temp_acti->started_by == temp_removed) {
                    UI_SHELL_LOG_MSGID_I("activity %x is started by removed activity : %x", 2,
                            temp_acti->proc_event_func, temp_removed->proc_event_func);
                    temp_acti->started_by = NULL;
                    break;
                }
            }
        }

        // Free removed activities
        for (temp_acti = removed_list; temp_acti != NULL;) {
            temp_removed = temp_acti;
            temp_acti = temp_acti->next;
            ui_shell_activity_heap_free_item(temp_removed); // free
            
        }

        // Fresh the first activity
        if (s_acti_list) {
            s_acti_list->proc_event_func(&(s_acti_list->external_activity),
                    EVENT_GROUP_UI_SHELL_SYSTEM,
                    EVENT_ID_SHELL_SYSTEM_ON_REFRESH,
                    NULL, 0);
        }
    }
}

void ui_shell_activity_stack_request_allowance(uint32_t request_id, ui_shell_activity_internal_t *requester)
{
    ui_shell_activity_internal_t *temp_acti = s_acti_list;
    temp_saved_activity_t *acti_need_saved = NULL;
    waiting_allowed_data_t *waiting_data = NULL;
    bool ret;

    for (; temp_acti != NULL; temp_acti = temp_acti->next) {
        ret = temp_acti->proc_event_func(&temp_acti->external_activity,
                EVENT_GROUP_UI_SHELL_SYSTEM,
                EVENT_ID_SHELL_SYSTEM_ON_REQUEST_ALLOWANCE, (void *)request_id, 0);
        if (!ret) {
            UI_SHELL_LOG_MSGID_I("Not allowed the request : %d", 1, request_id);
            if (!waiting_data) {
                waiting_data = ui_shell_al_zalloc(sizeof(waiting_allowed_data_t));
            }

            if (waiting_data) {
                acti_need_saved = ui_shell_al_zalloc(sizeof(temp_saved_activity_t));
                if (acti_need_saved) {
                    acti_need_saved->acti = temp_acti;
                    // Add at the first node
                    acti_need_saved->next = waiting_data->waiting_acti_list;
                    waiting_data->waiting_acti_list = acti_need_saved;
                } else {
                    UI_SHELL_LOG_MSGID_E("malloc acti_need_saved failed", 0);
                }
            } else {
                UI_SHELL_LOG_MSGID_E("malloc waiting_data failed", 0);
            }
        }
    }

    if (waiting_data) {
        waiting_data->request_activity = requester;
        waiting_data->wait_request_id = request_id;
        // Add at the first node
        waiting_data->next = s_waiting_allowed_list;
        s_waiting_allowed_list = waiting_data;
    } else {
        requester->proc_event_func(&requester->external_activity, EVENT_GROUP_UI_SHELL_SYSTEM,
                EVENT_ID_SHELL_SYSTEM_ON_ALLOW, (void *)request_id, 0);
    }
}

ui_shell_activity_internal_t *ui_shell_activity_stack_allow(ui_shell_activity_internal_t *target_acti, uint32_t request_id)
{
    waiting_allowed_data_t **p_wait_data = NULL;
    temp_saved_activity_t **p_temp_saved_acti = NULL;
    ui_shell_activity_internal_t *acti_need_notify = NULL;

    for (p_wait_data = &s_waiting_allowed_list; p_wait_data && *p_wait_data; ) {

        if (request_id == (*p_wait_data)->wait_request_id) {
            for (p_temp_saved_acti = &(*p_wait_data)->waiting_acti_list;
                    p_temp_saved_acti && *p_temp_saved_acti;
                    p_temp_saved_acti = &(*p_temp_saved_acti)->next) {

                if ((*p_temp_saved_acti)->acti == target_acti) {
                    temp_saved_activity_t *temp_activity_need_free = *p_temp_saved_acti;
                    *p_temp_saved_acti = (*p_temp_saved_acti)->next;
                    ui_shell_al_free(temp_activity_need_free);
                    break;
                }
            }

            if ((*p_wait_data)->waiting_acti_list == NULL) {
                acti_need_notify = (*p_wait_data)->request_activity;
                waiting_allowed_data_t *wait_data_need_free = *p_wait_data;
                *p_wait_data = (*p_wait_data)->next;
                ui_shell_al_free(wait_data_need_free);
            } else {
                p_wait_data = &(*p_wait_data)->next;
            }
            break;
        } else {
            p_wait_data = &(*p_wait_data)->next;
        }

    }

    return acti_need_notify;
}

void ui_shell_activity_stack_allowed(uint32_t request_id, ui_shell_activity_internal_t *requester)
{
    if (requester) {
        requester->proc_event_func(&requester->external_activity, EVENT_GROUP_UI_SHELL_SYSTEM,
                EVENT_ID_SHELL_SYSTEM_ON_ALLOW, (void *)request_id, 0);
    }
}

void ui_shell_activity_stack_clear(void)
{
    ui_shell_activity_internal_t *temp_acti = NULL;
    ui_shell_activity_stack_go_to_idle(); // Remove transient activity
    for (temp_acti = s_acti_list; temp_acti != NULL; temp_acti = s_acti_list) {
        temp_acti->proc_event_func(&temp_acti->external_activity,
                EVENT_GROUP_UI_SHELL_SYSTEM,
                EVENT_ID_SHELL_SYSTEM_ON_DESTROY,
                NULL, 0);
        s_acti_list = temp_acti->next;
        ui_shell_activity_heap_free_item(temp_acti);
    }

    if (s_pre_proc_acti) {
        s_pre_proc_acti->proc_event_func(&s_pre_proc_acti->external_activity,
                EVENT_GROUP_UI_SHELL_SYSTEM,
                EVENT_ID_SHELL_SYSTEM_ON_DESTROY,
                NULL, 0);
        ui_shell_activity_heap_free_item(s_pre_proc_acti);
        s_pre_proc_acti = NULL;
    }
}

bool ui_shell_activity_stack_traverse_stack(uint32_t event_group, uint32_t event_id, void *data, size_t data_len)
{
    bool ret = false;
    ui_shell_activity_internal_t *temp_acti = s_acti_list;

    if (s_pre_proc_acti) {
        ret = s_pre_proc_acti->proc_event_func(&s_pre_proc_acti->external_activity, event_group, event_id, data, data_len);
    }

    if (ret != true) {
        for (temp_acti = s_acti_list; temp_acti != NULL; temp_acti = temp_acti->next) {
            UI_SHELL_LOG_MSGID_D("traverse_stack: %x", 1, temp_acti);
            ret = temp_acti->proc_event_func(&temp_acti->external_activity, event_group, event_id, data, data_len);
            if (ret == true) { // true means the message is processed by the activity
                break;
            }
        }
    }
    UI_SHELL_LOG_MSGID_D("traverse_stack end", 0);

    return ret;
}

bool ui_shell_activity_stack_on_result(ui_shell_activity_internal_t *target_acti, void *data, size_t data_len)
{
    bool ret = false;
    ui_shell_activity_internal_t *temp_acti = s_acti_list;

    for (temp_acti = s_acti_list; temp_acti != NULL; temp_acti = temp_acti->next) {
        if (temp_acti == target_acti) {
            ret = target_acti->proc_event_func(&target_acti->external_activity,
                    EVENT_GROUP_UI_SHELL_SYSTEM,
                    EVENT_ID_SHELL_SYSTEM_ON_RESULT,
                    data, data_len);
            break;
        }
    }
    return ret;
}

void ui_shell_activity_stack_refresh_activity(ui_shell_activity_internal_t *target_acti)
{
    if (s_acti_list != target_acti) {
        UI_SHELL_LOG_MSGID_I("target activity is not the top activity, so ignore the refresh request", 0);
    } else {
        target_acti->proc_event_func(&target_acti->external_activity,
                EVENT_GROUP_UI_SHELL_SYSTEM,
                EVENT_ID_SHELL_SYSTEM_ON_REFRESH,
                NULL, 0);
    }
}
