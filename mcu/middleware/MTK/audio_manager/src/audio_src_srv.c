/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include <string.h>
#include "stdint.h"
#include "audio_src_srv.h"
#include "audio_src_srv_internal.h"
#include "audio_log.h"


/* multi source handle */
audio_src_srv_handle_t g_audio_src_srv_handle[AUDIO_SRC_SRV_PSEUDO_DEVICE_NUM];


static void audio_src_srv_reset_handle(audio_src_srv_handle_t *handle)
{
    if (handle) {
        memset(handle, 0x00, sizeof(audio_src_srv_handle_t));
        handle->state = AUDIO_SRC_SRV_STATE_NONE;
        AUDIO_SRC_SRV_RESET_FLAG(handle, AUDIO_SRC_SRV_FLAG_USED);
    }
}


audio_src_srv_handle_t *audio_src_srv_construct_handle(audio_src_srv_pseudo_device_t type)
{
    int32_t i = 0;
    audio_src_srv_handle_t *hd = NULL;
    // mutex lock
    audio_src_srv_mutex_lock();
    for (i = 0; i < AUDIO_SRC_SRV_PSEUDO_DEVICE_NUM; ++i) {
        if (!(g_audio_src_srv_handle[i].flag & AUDIO_SRC_SRV_FLAG_USED)) {
            hd = &g_audio_src_srv_handle[i];
            //audio_src_srv_reset_handle(hd);
            hd->state = AUDIO_SRC_SRV_STATE_NONE;
            hd->substate = 0;
            hd->type = type;
            //hd->flag |= AUDIO_SRC_SRV_FLAG_USED;
            AUDIO_SRC_SRV_SET_FLAG(hd, AUDIO_SRC_SRV_FLAG_USED);
            break;
        }
    }
    // mutex unlock
    audio_src_srv_mutex_unlock();

    if (!hd) {
        // exception: no source handle, check
        audio_src_srv_report("[AudSrc]construct_handle--error, type(%d)\n", 1, type);
    } else {
        audio_src_srv_report("[AudSrc]construct_handle i(%d) type(%d)\n", 2, i, type);
    }

    return hd;
}


void audio_src_srv_destruct_handle(audio_src_srv_handle_t *handle)
{
    audio_src_srv_report("[AudSrc]destruct type(%d)\n", 1, handle->type);
    // mutex lock
    audio_src_srv_mutex_lock();
    audio_src_srv_context_t *ctx = audio_src_srv_get_ctx();
    if (ctx == NULL){
        audio_src_srv_reset_handle(handle);
    } else {
        if (ctx->running == handle) {
            audio_src_srv_process_psedev_event(handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
        }
        audio_src_srv_reset_handle(handle);
    }
    // mutex unlock
    audio_src_srv_mutex_unlock();
}


void audio_src_srv_add_waiting_list(audio_src_srv_handle_t *handle)
{
    // mutex lock
    audio_src_srv_mutex_lock();
    audio_src_srv_context_t *ctx = audio_src_srv_get_ctx();
    if (handle) {
        if ((ctx == NULL)? true : ((ctx->running == handle)? false : true)){
            audio_src_srv_report("[AudSrc]add_waiting_list--hd: 0x%x, type: %d, state: %d\n", 3, handle, handle->type, handle->state);
            AUDIO_SRC_SRV_SET_FLAG(handle, AUDIO_SRC_SRV_FLAG_WAITING);
        } else {
            audio_src_srv_report("[AudSrc]add_waiting_list(ignore)--hd = running: 0x%x, type: %d, state: %d\n", 3, handle, handle->type, handle->state);
        }
    } else {
        audio_src_srv_report("[AudSrc]add_waiting_list(err)\n", 0);
    }
    // mutex unlock
    audio_src_srv_mutex_unlock();
}


void audio_src_srv_del_waiting_list(audio_src_srv_handle_t *handle)
{
    // mutex lock
    audio_src_srv_mutex_lock();
    if (handle) {
        audio_src_srv_report("[AudSrc]del_waiting_list--hd: 0x%x, type: %d, state: %d\n", 3, handle, handle->type, handle->state);
        AUDIO_SRC_SRV_RESET_FLAG(handle, AUDIO_SRC_SRV_FLAG_WAITING);
    } else {
        audio_src_srv_report("[AudSrc]del_waiting_list(err)\n", 0);
    }
    // mutex unlock
    audio_src_srv_mutex_unlock();
}


void audio_src_srv_update_state(audio_src_srv_handle_t *handle, audio_src_srv_event_t evt_id)
{
    // mutex lock
    audio_src_srv_mutex_lock();
#if defined(MTK_AUDIO_MANAGER_DEBUG)
    audio_src_srv_report("[AudSrc]Change Request handle(0x%x) type(%d) msg_ID(%d)", 3, handle, handle->type, evt_id);
#endif
    if (handle) {
        audio_src_srv_process_psedev_event(handle, evt_id);
    } else {
        audio_src_srv_report("[AudSrc]update_state(err)\n", 0);
    }
    // mutex unlock
    audio_src_srv_mutex_unlock();
}

void audio_src_srv_set_substate(audio_src_srv_handle_t *handle, uint32_t substate)
{
    // mutex lock
    audio_src_srv_mutex_lock();
#if defined(MTK_AUDIO_MANAGER_DEBUG)
    audio_src_srv_report("[AudSrc]Set_substate  handle(0x%x) type(%d) msg_ID(%d)", 3, handle, handle->type, substate);
#endif
    if (handle) {
        handle->substate = substate;
    } else {
        audio_src_srv_report("[AudSrc]set_substate(err)\n", 0);
    }
    // mutex unlock
    audio_src_srv_mutex_unlock();
}

const audio_src_srv_handle_t *audio_src_srv_get_pseudo_device(void)
{
    return g_audio_src_srv_handle;
}

const audio_src_srv_handle_t *audio_src_srv_get_runing_pseudo_device(void)
{
    audio_src_srv_context_t *ctx = audio_src_srv_get_ctx();
    if (ctx == NULL)
        return NULL;
    else
    {
        return ctx->running;
    }
}

///////////////////////Static API /////////////////////////////////////////////////////
audio_src_srv_resource_manager_context_t g_audio_src_srv_resource_manager_ctx[AUDIO_SRC_SRV_RESOURCE_TYPE_MAX];

audio_src_srv_resource_manager_handle_t g_audio_src_srv_resource_manager_handle[AUDIO_SRC_SRV_RESOURCE_TYPE_MAX][AUDIO_SRC_SRV_RESOURCE_USR_MAX];

static audio_src_srv_resource_manager_context_t *audio_src_srv_resource_manger_get_ctx(audio_src_srv_resource_type_t resource_type)
{
    return &g_audio_src_srv_resource_manager_ctx[resource_type];
}


static void audio_src_srv_resource_manager_update_play_count(audio_src_srv_resource_manager_handle_t *handle)
{
    audio_src_srv_resource_manager_context_t *ctx = audio_src_srv_resource_manger_get_ctx(handle->resource_type);

    handle->play_count = ++ctx->play_count;
}

static void audio_src_srv_resource_manager_reset_handle(audio_src_srv_resource_manager_handle_t *handle)
{
    if (handle) {
        memset(handle, 0x00, sizeof(audio_src_srv_resource_manager_handle_t));
        handle->state = AUDIO_SRC_SRV_EVENT_NONE;
        AUDIO_SRC_SRV_RESET_FLAG(handle, AUDIO_SRC_SRV_FLAG_USED);
    }
}

static bool audio_src_srv_resource_manager_compare(audio_src_srv_resource_manager_handle_t *cur, audio_src_srv_resource_manager_handle_t *coming)
{
    if(coming->priority > cur->priority) {
        return true;
    } else if (coming->priority < cur->priority) {
        return false;
    } else {
        return (coming->play_count > cur->play_count) ? true : false;
    }
}

static audio_src_srv_resource_manager_handle_t *audio_src_srv_resource_manager_get_waiting_psedev(audio_src_srv_resource_type_t resource_type)
{
    audio_src_srv_resource_manager_handle_t *waiting_list = g_audio_src_srv_resource_manager_handle[resource_type];
    audio_src_srv_resource_manager_handle_t *psedev = NULL;
    int32_t i = 0;

    for (i = 0; i < AUDIO_SRC_SRV_RESOURCE_USR_MAX; ++i) {
        if (waiting_list[i].flag & AUDIO_SRC_SRV_FLAG_WAITING) {
            psedev = &waiting_list[i];
            break;
        }
    }

    if (psedev) {
        for (i += 1; i < AUDIO_SRC_SRV_RESOURCE_USR_MAX; ++i) {
            if (waiting_list[i].flag & AUDIO_SRC_SRV_FLAG_WAITING) {
                if(audio_src_srv_resource_manager_compare(psedev, &waiting_list[i])) {
                    psedev = &waiting_list[i];
                }
            }
        }
    }

    if(psedev == NULL) {
        audio_src_srv_report("[AudSrcManager]Get waiting list NULL.", 0);
    } else {
        audio_src_srv_report("[AudSrcManager]Get waiting list--hd: 0x%x, resource_type(%d), handle_name %d\n", 3, psedev, psedev->resource_type, psedev->handle_name);
    }
    return psedev;
}

static void audio_src_srv_resource_manager_update_running_psedev(audio_src_srv_resource_manager_handle_t *handle, audio_src_srv_resource_type_t resource_type)
{
    audio_src_srv_resource_manager_context_t *ctx = audio_src_srv_resource_manger_get_ctx(resource_type);

    if (ctx->running) {
        audio_src_srv_report("[AudSrcManager]update_running(cur)--hd: 0x%x, resource_type: %d, handle_name: %d\n", 3,
            ctx->running, ctx->running->resource_type, ctx->running->handle_name);
    }

    if (handle) {
        audio_src_srv_report("[AudSrcManager]update_running--hd: 0x%x, resource_type: %d, handle_name: %d\n", 3,
            handle, handle->resource_type, handle->handle_name);
    } else {
        audio_src_srv_report("[AudSrcManager]update_running--reset running\n",0);
    }

    ctx->running = handle;
}

static void audio_src_srv_resource_manager_update_state_broadcast(audio_src_srv_resource_manager_handle_t *handle, audio_src_srv_resource_manager_event_t state) {
    for(audio_src_srv_resource_type_t temp_resource_type = 0; temp_resource_type < AUDIO_SRC_SRV_RESOURCE_TYPE_MAX; temp_resource_type++) {
        /* The event will not notify to the user with the same resource type */
        if(temp_resource_type == handle->resource_type) {
            continue;
        }
        for (int32_t i = 0; i < AUDIO_SRC_SRV_RESOURCE_USR_MAX; ++i) {
            if ((g_audio_src_srv_resource_manager_handle[temp_resource_type][i].flag & AUDIO_SRC_SRV_FLAG_USED) && (g_audio_src_srv_resource_manager_handle[temp_resource_type][i].callback_func != NULL)) {
                audio_src_srv_resource_manager_handle_t *temp_handle = &g_audio_src_srv_resource_manager_handle[temp_resource_type][i];
                if(((uint32_t)temp_handle->callback_func) != ((uint32_t)handle->callback_func)) {
                    audio_src_srv_report("[AudSrcManager]audio_src_srv_resource_manager_update_state_broadcast--hd==>hd: 0x%x, 0x%x, resource_type: %d, handle_name: %d, state: %d\n", 5, handle, temp_handle, temp_handle->resource_type, temp_handle->handle_name, state);
                    temp_handle->callback_func(handle, state);
                } else {
                    audio_src_srv_report("[AudSrcManager]audio_src_srv_resource_manager_update_state_broadcast, no need broadcase due to the same callback func: --hd==>hd: 0x%x, 0x%x, resource_type: %d, handle_name: %d, state: %d\n", 5, handle, temp_handle, temp_handle->resource_type, temp_handle->handle_name, state);
                }
            }
        }
    }
}

static void audio_src_srv_resource_manager_update_state(audio_src_srv_resource_manager_handle_t *handle, audio_src_srv_resource_manager_event_t state) {
    audio_src_srv_report("[AudSrcManager]audio_src_srv_resource_manager_update_state--hd: 0x%x, resource_type: %d, handle_name: %d, state: %d\n", 4, handle, handle->resource_type, handle->handle_name, state);

    /* update handle state and callback to the corresponding callback_func */
    if((state != AUDIO_SRC_SRV_EVENT_TAKE_ALREADY) && (state != AUDIO_SRC_SRV_EVENT_GIVE_ERROR)) {
        handle->state = state;
    }

    /* The AUDIO_SRC_SRV_EVENT_TAKE_SUCCESS event should be broadcast at early stage */
    if(state == AUDIO_SRC_SRV_EVENT_TAKE_SUCCESS) {
        audio_src_srv_resource_manager_update_state_broadcast(handle, state);
        handle->callback_func(handle, state);
    } else if(state == AUDIO_SRC_SRV_EVENT_GIVE_SUCCESS){
        handle->callback_func(handle, state);
        audio_src_srv_resource_manager_update_state_broadcast(handle, state);
    } else {
        handle->callback_func(handle, state);
    }
}

static void audio_src_srv_resource_manager_interrupt_handle(audio_src_srv_resource_manager_handle_t *handle)
{
    audio_src_srv_resource_manager_context_t *ctx = audio_src_srv_resource_manger_get_ctx(handle->resource_type);
    bool preempt = false;

    audio_src_srv_report("[AudSrcManager]psedev_interrupt--cur~~hd: 0x%x, type: %d, handle_name: %d, pri: %d, cnt: %d, coming~~hd: 0x%x, type: %d, handle_name: %d, pri: %d, cnt: %d\n", 10,
        ctx->running, ctx->running->resource_type, ctx->running->handle_name, ctx->running->priority, ctx->running->play_count,
        handle, handle->resource_type, handle->handle_name, handle->priority, handle->play_count);

    if(audio_src_srv_resource_manager_compare(ctx->running, handle)) {
        preempt = true;
    }

    if (preempt) {
        /* accept and interrupt running pseudo device */
        /* push waiting list */
        audio_src_srv_resource_manager_add_waiting_list(handle);
        audio_src_srv_resource_manager_update_state(ctx->running, AUDIO_SRC_SRV_EVENT_SUSPEND);
    } else {
        /* reject play request */
        audio_src_srv_resource_manager_update_state(handle, AUDIO_SRC_SRV_EVENT_TAKE_REJECT);
    }
}

//////////////////////////////PUB API//////////////////////////
audio_src_srv_resource_manager_handle_t *audio_src_srv_resource_manager_construct_handle(audio_src_srv_resource_type_t resource_type, const char * handle_name) {
    int32_t i = 0;
    audio_src_srv_resource_manager_handle_t *hd = NULL;
    // mutex lock
    audio_src_srv_mutex_lock();
    for (i = 0; i < AUDIO_SRC_SRV_RESOURCE_USR_MAX; ++i) {
        if (!(g_audio_src_srv_resource_manager_handle[resource_type][i].flag & AUDIO_SRC_SRV_FLAG_USED)) {
            hd = &g_audio_src_srv_resource_manager_handle[resource_type][i];
            hd->resource_type = resource_type;
            hd->handle_name = handle_name;
            AUDIO_SRC_SRV_SET_FLAG(hd, AUDIO_SRC_SRV_FLAG_USED);
            break;
        }
    }
    // mutex unlock
    audio_src_srv_mutex_unlock();

    if (!hd) {
        // exception: no source handle, check
        // audio_src_srv_report("[AudSrcManager]construct_handle--error, resource_type(%d), handle_name: %d\n", 2, resource_type, handle_name);
        printf("[AudSrcManager]construct_handle--error, resource_type(%d), handle_name: %s\n", resource_type, handle_name);
    } else {
        // audio_src_srv_report("[AudSrcManager]construct_handle i(%d), hd: 0x%x, resource_type(%d), handle_name: %d\n", 4, i, hd, resource_type, handle_name);
        printf("[AudSrcManager]construct_handle i(%d), hd: 0x%x, resource_type(%d), handle_name: %s\n", (int)i, (unsigned int)hd, resource_type, handle_name);
    }

    return hd;
}

void audio_src_srv_resource_manager_destruct_handle(audio_src_srv_resource_manager_handle_t *handle) {
    // audio_src_srv_report("[AudSrcManager]destruct_handle hd: 0x%x, resource_type(%d), handle_name %d\n", 3, handle, handle->resource_type, handle->handle_name);
    printf("[AudSrcManager]destruct_handle hd: 0x%x, resource_type(%d), handle_name %s\n", (unsigned int)handle, handle->resource_type, handle->handle_name);
    // mutex lock
    audio_src_srv_mutex_lock();
    audio_src_srv_resource_manager_context_t *ctx = audio_src_srv_resource_manger_get_ctx(handle->resource_type);
    if (ctx == NULL){
        audio_src_srv_resource_manager_reset_handle(handle);
    } else {
        if (ctx->running == handle) {
            //Should never come here, user should give resource before destruct the handle
            audio_src_srv_resource_manager_update_state(handle, AUDIO_SRC_SRV_EVENT_NONE);
        }
        audio_src_srv_resource_manager_reset_handle(handle);
    }
    // mutex unlock
    audio_src_srv_mutex_unlock();
}

audio_src_srv_resource_manager_handle_t *audio_src_srv_resource_manager_get_current_running_handle(audio_src_srv_resource_type_t resource_type) {
    audio_src_srv_resource_manager_context_t *ctx = audio_src_srv_resource_manger_get_ctx(resource_type);
    return (ctx == NULL) ? NULL : ctx->running;
}

void audio_src_srv_resource_manager_add_waiting_list(audio_src_srv_resource_manager_handle_t *handle) {
    // mutex lock
    audio_src_srv_mutex_lock();
    audio_src_srv_resource_manager_context_t *ctx = audio_src_srv_resource_manger_get_ctx(handle->resource_type);
    if (handle) {
        if ((ctx == NULL) ? true : ((ctx->running == handle) ? false : true)){
            audio_src_srv_report("[AudSrcManager]add_waiting_list--hd: 0x%x, resource_type(%d), handle_name: %d, state: %d\n", 4, handle, handle->resource_type ,handle->handle_name, handle->state);
            AUDIO_SRC_SRV_SET_FLAG(handle, AUDIO_SRC_SRV_FLAG_WAITING);
        } else {
            audio_src_srv_report("[AudSrcManager]add_waiting_list(ignore)--hd = running: 0x%x, resource_type(%d), handle_name: %d, state: %d\n", 4, handle, handle->resource_type, handle->handle_name, handle->state);
        }
    } else {
        audio_src_srv_report("[AudSrcManager]add_waiting_list(err)\n", 0);
    }
    // mutex unlock
    audio_src_srv_mutex_unlock();
}

void audio_src_srv_resource_manager_delete_waiting_list(audio_src_srv_resource_manager_handle_t *handle) {
    // mutex lock
    audio_src_srv_mutex_lock();
    if (handle) {
        audio_src_srv_report("[AudSrcManager]del_waiting_list--hd: 0x%x, handle_name: %d, state: %d\n", 3, handle, handle->handle_name, handle->state);
        AUDIO_SRC_SRV_RESET_FLAG(handle, AUDIO_SRC_SRV_FLAG_WAITING);
    } else {
        audio_src_srv_report("[AudSrcManager]del_waiting_list(err)\n", 0);
    }
    // mutex unlock
    audio_src_srv_mutex_unlock();
}

void audio_src_srv_resource_manager_take(audio_src_srv_resource_manager_handle_t *handle) {
    audio_src_srv_report("[AudSrcManager]audio_src_srv_resource_manager_take--hd: 0x%x, resource_type: %d, priority: %d, handle_name: %d, state: %d\n", 5, handle, handle->resource_type, handle->priority, handle->handle_name, handle->state);
    // mutex lock
    audio_src_srv_mutex_lock();
    audio_src_srv_resource_manager_context_t *ctx = audio_src_srv_resource_manger_get_ctx(handle->resource_type);
    audio_src_srv_resource_manager_update_play_count(handle);
    if(ctx->running == NULL) {
        audio_src_srv_resource_manager_update_state(handle, AUDIO_SRC_SRV_EVENT_TAKE_SUCCESS);
        audio_src_srv_resource_manager_update_running_psedev(handle, handle->resource_type);
    } else {
        if(ctx->running != handle) {
            audio_src_srv_resource_manager_interrupt_handle(handle);
        } else {
            audio_src_srv_resource_manager_update_state(handle, AUDIO_SRC_SRV_EVENT_TAKE_ALREADY);
        }
    }
    // mutex unlock
    audio_src_srv_mutex_unlock();
}

void audio_src_srv_resource_manager_give(audio_src_srv_resource_manager_handle_t *handle) {
    audio_src_srv_report("[AudSrcManager]audio_src_srv_resource_manager_give--hd: 0x%x, resource_type: %d, priority: %d, handle_name: %d, state: %d\n", 5, handle, handle->resource_type, handle->priority, handle->handle_name, handle->state);
    // mutex lock
    audio_src_srv_mutex_lock();
    audio_src_srv_resource_manager_context_t *ctx = audio_src_srv_resource_manger_get_ctx(handle->resource_type);

    if ((ctx->running) && (ctx->running == handle) && ((handle->state == AUDIO_SRC_SRV_EVENT_TAKE_SUCCESS) || (handle->state == AUDIO_SRC_SRV_EVENT_SUSPEND))) {
        audio_src_srv_resource_manager_update_state(handle, AUDIO_SRC_SRV_EVENT_GIVE_SUCCESS);
        audio_src_srv_resource_manager_update_running_psedev(audio_src_srv_resource_manager_get_waiting_psedev(handle->resource_type), handle->resource_type);

        if(ctx->running) {
            audio_src_srv_resource_manager_update_state(ctx->running, AUDIO_SRC_SRV_EVENT_TAKE_SUCCESS);
            audio_src_srv_resource_manager_delete_waiting_list(ctx->running);
        }
    } else {
        if (handle == NULL){
            audio_src_srv_err("[AudSrcManager]audio_src_srv_resource_manager_give fail, handle == NULL !!", 0);
            configASSERT(handle);
        }
        audio_src_srv_resource_manager_update_state(handle, AUDIO_SRC_SRV_EVENT_GIVE_ERROR);
        audio_src_srv_report("[AudSrcManager]audio_src_srv_resource_manager_give fail--current_running: 0x%x, hd: 0x%x, resource_type: %d, handle_name: %d, state: %d\n", 5, ctx->running, handle, handle->resource_type, handle->handle_name, handle->state);
    }

    // mutex unlock
    audio_src_srv_mutex_unlock();
}
