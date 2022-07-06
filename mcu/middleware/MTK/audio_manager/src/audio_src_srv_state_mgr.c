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


#include "stdint.h"
#include "audio_src_srv.h"
#include "audio_src_srv_internal.h"
#include "audio_log.h"
#include "hal_nvic.h"

static audio_src_srv_context_t g_audio_src_srv_ctx;

extern audio_src_srv_handle_t g_audio_src_srv_handle[AUDIO_SRC_SRV_PSEUDO_DEVICE_NUM];

xSemaphoreHandle g_xSemaphore_am_state = NULL;
uint8_t          g_xSemaphore_cnt      = 0;
void*    AUDIO_SRC_SRV_CURRENT_TASK = NULL;
extern void *pxCurrentTCB;

// function declare
static bool audio_src_srv_check_psedev_param(audio_src_srv_handle_t *handle);

static uint32_t audio_src_srv_gen_play_count(void);

static void audio_src_srv_transfer_state(audio_src_srv_state_t state);

static void audio_src_srv_update_running_psedev(audio_src_srv_handle_t *handle);

static void audio_src_srv_psedev_interrupt_handle(audio_src_srv_handle_t *handle);

static audio_src_srv_handle_t *audio_src_srv_get_waiting_psedev(void);

static void audio_src_srv_state_none_handle(audio_src_srv_handle_t *handle, audio_src_srv_message_t msg_id, void *param);

static void audio_src_srv_state_ready_handle(audio_src_srv_handle_t *handle, audio_src_srv_message_t msg_id, void *param);

static void audio_src_srv_state_prepare_play_handle(audio_src_srv_handle_t *handle, audio_src_srv_message_t msg_id, void *param);

static void audio_src_srv_state_prepare_stop_handle(audio_src_srv_handle_t *handle, audio_src_srv_message_t msg_id, void *param);

static void audio_src_srv_state_playing_handle(audio_src_srv_handle_t *handle, audio_src_srv_message_t msg_id, void *param);

static void default_audio_src_srv_mutex_lock(void);

static void default_audio_src_srv_mutex_unlock(void);

/**< Weak pointer */
#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:audio_src_srv_psedev_compare=default_audio_src_srv_psedev_compare")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION) || defined(__CC_ARM)
#pragma weak audio_src_srv_psedev_compare = default_audio_src_srv_psedev_compare
#else
#error "Unsupported Platform"
#endif

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:audio_src_srv_mutex_lock=audio_src_srv_mutex_lock")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION) || defined(__CC_ARM)
#pragma weak audio_src_srv_mutex_lock = default_audio_src_srv_mutex_lock
#else
#error "Unsupported Platform"
#endif

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:audio_src_srv_mutex_lock=audio_src_srv_mutex_lock")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION) || defined(__CC_ARM)
#pragma weak audio_src_srv_mutex_unlock = default_audio_src_srv_mutex_unlock
#else
#error "Unsupported Platform"
#endif



static bool audio_src_srv_check_psedev_param(audio_src_srv_handle_t *handle)
{
    if (handle->stop && handle->play && handle->suspend && handle->reject && handle->exception_handle) {
        ;
    } else {
        audio_src_srv_report("[AudSrc]check_param(err)--empty func ptr\n", 0);
        return false;
    }

    return true;
}


static uint32_t audio_src_srv_gen_play_count(void)
{
    audio_src_srv_context_t *ctx = audio_src_srv_get_ctx();

    return ctx->play_count++;
}


static bool default_audio_src_srv_psedev_compare(audio_src_srv_handle_t *cur, audio_src_srv_handle_t *coming)
{
    return (cur->play_count > coming->play_count) ? true : false;
}

static void default_audio_src_srv_mutex_lock(void)
{
    bool is_current_task = false;
    uint32_t savedmask;
    if(g_xSemaphore_am_state == NULL){
        g_xSemaphore_am_state =  xSemaphoreCreateMutex();
    }
    hal_nvic_save_and_set_interrupt_mask(&savedmask);
    if(AUDIO_SRC_SRV_CURRENT_TASK == pxCurrentTCB){
        is_current_task = true;
    } else {
        is_current_task = false;
    }
    hal_nvic_restore_interrupt_mask(savedmask);
    if(is_current_task){
        g_xSemaphore_cnt ++;
    } else {
        xSemaphoreTake(g_xSemaphore_am_state, portMAX_DELAY);
        AUDIO_SRC_SRV_CURRENT_TASK = pxCurrentTCB;
    }
    audio_src_srv_report("Take am_state ++ (%d)", 1, g_xSemaphore_cnt);
}


static void default_audio_src_srv_mutex_unlock(void)
{
    bool is_current_task = false;
    uint32_t savedmask;
    hal_nvic_save_and_set_interrupt_mask(&savedmask);
    if(AUDIO_SRC_SRV_CURRENT_TASK == pxCurrentTCB){
        is_current_task = true;
    } else {
        is_current_task = false;
    }
    hal_nvic_restore_interrupt_mask(savedmask);
    if(is_current_task){
        if(g_xSemaphore_cnt == 0){
            audio_src_srv_report("Give am_state --", 0);
            AUDIO_SRC_SRV_CURRENT_TASK = NULL;
            xSemaphoreGive(g_xSemaphore_am_state);
        } else {
            g_xSemaphore_cnt --;
            audio_src_srv_report("Give am_state --(%d)", 1, g_xSemaphore_cnt);
        }
    } else {
        audio_src_srv_report("Give am_state false, not current task.", 0);
    }
}

static void audio_src_srv_transfer_state(audio_src_srv_state_t state)
{
    audio_src_srv_context_t *ctx = audio_src_srv_get_ctx();

    audio_src_srv_report("[AudSrc]transfer_state--ori: 0x%x, cur: 0x%x\n", 2, ctx->state, state);
    ctx->state = state;
}


static void audio_src_srv_update_running_psedev(audio_src_srv_handle_t *handle)
{
    audio_src_srv_context_t *ctx = audio_src_srv_get_ctx();

    if (handle) {
        audio_src_srv_report("[AudSrc]update_running--hd: 0x%x, type: %d, state: 0x%x\n", 3,
            handle, handle->type, handle->state);
    } else {
        audio_src_srv_report("[AudSrc]update_running--reset running\n",0);
    }

    if (ctx->running) {
        audio_src_srv_report("[AudSrc]update_running(cur)--hd: 0x%x, type: %d, state: 0x%x\n", 3,
            ctx->running, ctx->running->type, ctx->running->state);
    }

    ctx->running = handle;
}

static void audio_src_srv_psedev_interrupt_handle(audio_src_srv_handle_t *handle)
{
    audio_src_srv_context_t *ctx = audio_src_srv_get_ctx();
    bool preempt = false;

    audio_src_srv_report("[AudSrc]psedev_interrupt--cur~~hd: 0x%x, type: %d, state: 0x%x, pri: %d, cnt: %d, coming~~hd: 0x%x, type: %d, state: 0x%x, pri: %d, cnt: %d\n", 10,
        ctx->running, ctx->running->type, ctx->running->state, ctx->running->priority, ctx->running->play_count,
        handle, handle->type, handle->state, handle->priority, handle->play_count);

    if ((handle->priority > ctx->running->priority)
        || ((handle->priority == ctx->running->priority) && audio_src_srv_psedev_compare(handle, ctx->running))) {
        preempt = true;
    }

    if (preempt){
        /* accept and interrupt running pseudo device */
        /* push waiting list */
        audio_src_srv_add_waiting_list(handle);
        audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_PREPARE_PLAY);

        /* change psedev state with PREPARE_STOP */
        audio_src_srv_update_psedev_state(ctx->running, AUDIO_SRC_SRV_STATE_PREPARE_STOP);
        audio_src_srv_transfer_state(AUDIO_SRC_SRV_STATE_PREPARE_STOP);

        /* notify interrupt */
#if defined(MTK_AUDIO_MANAGER_DEBUG)
        audio_src_srv_report("[AudSrc]psedev_interrupt handle(0x%x) type(%d). suspend", 2, ctx->running, ctx->running->type);
#endif
        ctx->running->suspend(ctx->running, handle);
    } else {
            /* reject play request */
#if defined(MTK_AUDIO_MANAGER_DEBUG)
        audio_src_srv_report("[AudSrc]psedev_interrupt handle(0x%x) type(%d). reject", 2, handle, handle->type);
#endif
        handle->reject(handle);
    }
}


static audio_src_srv_handle_t *audio_src_srv_get_waiting_psedev(void)
{
    audio_src_srv_handle_t *waiting_list = g_audio_src_srv_handle;
    audio_src_srv_handle_t *psedev = NULL;
    int32_t i = 0;

    for (i = 0; i < AUDIO_SRC_SRV_PSEUDO_DEVICE_NUM; ++i) {
        if (waiting_list[i].flag & AUDIO_SRC_SRV_FLAG_WAITING) {
            psedev = &waiting_list[i];
            break;
        }
    }

    if (psedev) {
        for (i += 1; i < AUDIO_SRC_SRV_PSEUDO_DEVICE_NUM; ++i) {
            if (waiting_list[i].flag & AUDIO_SRC_SRV_FLAG_WAITING) {
                if (waiting_list[i].priority > psedev->priority) {
                    psedev = &waiting_list[i];
                } else if (waiting_list[i].priority == psedev->priority) {
                    if (audio_src_srv_psedev_compare(psedev, &waiting_list[i])) {
                        ;
                    } else {
                        psedev = &waiting_list[i];
                    }
                }
            }
        }
    }

    if(psedev == NULL){
        audio_src_srv_report("[AudSrc]Get waiting list NULL.", 0);
    }else{
        audio_src_srv_report("[AudSrc]Get waiting list handle(0x%x), type(%d).", 2, psedev, psedev->type);
    }
    return psedev;
}


static void audio_src_srv_state_none_handle(audio_src_srv_handle_t *handle, audio_src_srv_message_t msg_id, void *param)
{
    switch (msg_id) {
        case AUDIO_SRC_SRV_MSG_READY: {
            audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_READY);
            audio_src_srv_transfer_state(AUDIO_SRC_SRV_STATE_READY);
            break;
        }

        default:
            break;
    }
}


static void audio_src_srv_state_ready_handle(audio_src_srv_handle_t *handle, audio_src_srv_message_t msg_id, void *param)
{
    audio_src_srv_context_t *ctx = audio_src_srv_get_ctx();
    int32_t i = 0;

    switch (msg_id) {
        case AUDIO_SRC_SRV_MSG_UNAVAILABLE: {
            audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_NONE);

            for (i = 0; i < AUDIO_SRC_SRV_PSEUDO_DEVICE_NUM; ++i) {
                if ((g_audio_src_srv_handle[i].state & AUDIO_SRC_SRV_STATE_MASK) != AUDIO_SRC_SRV_STATE_NONE) {
                    break;
                }
            }

            if (i >= AUDIO_SRC_SRV_PSEUDO_DEVICE_NUM) {
                audio_src_srv_transfer_state(AUDIO_SRC_SRV_STATE_NONE);
                //audio_src_srv_update_running_psedev(NULL);
            }
            break;
        }

        case AUDIO_SRC_SRV_MSG_READY: {
            if ((ctx->running) && (ctx->running == handle)) {
                if ((ctx->running->state & AUDIO_SRC_SRV_STATE_MASK) == AUDIO_SRC_SRV_STATE_PREPARE_STOP) {
                    audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_READY);
                    audio_src_srv_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                    audio_src_srv_update_running_psedev(audio_src_srv_get_waiting_psedev());
                    /* Wake up waiting pseudo device */
                    if (ctx->running) {
                        /* Remove handle from waiting list */
                        audio_src_srv_del_waiting_list(ctx->running);
                        /* Update pseudo device transient state */
                        audio_src_srv_update_psedev_state(ctx->running, AUDIO_SRC_SRV_STATE_PREPARE_PLAY);
                        audio_src_srv_transfer_state(AUDIO_SRC_SRV_STATE_PREPARE_PLAY);
#if defined(MTK_AUDIO_MANAGER_DEBUG)
                        audio_src_srv_report("[AudSrc]state_ready handle(0x%x) type(%d). play", 2, ctx->running, ctx->running->type);
#endif
                        ctx->running->play(ctx->running);
                    }
                } else {
                    audio_src_srv_report("[AudSrc]state_ready(err_ready)--type: %d, state: 0x%x\n", 2, ctx->running->type, ctx->running->state);
                }
            } else {
                audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_READY);
            }
            break;
        }

        case AUDIO_SRC_SRV_MSG_PLAY: {
            if (ctx->running) {
                if ((ctx->running->state & AUDIO_SRC_SRV_STATE_MASK) == AUDIO_SRC_SRV_STATE_PREPARE_PLAY) {
                    /* interrupt case, cur_state in PREPARE_PLAY */
                    audio_src_srv_psedev_interrupt_handle(handle);
                } else if ((ctx->running->state & AUDIO_SRC_SRV_STATE_MASK) == AUDIO_SRC_SRV_STATE_PREPARE_STOP) {
                    /* consider waiting list item priority?? */
                    /* push waiting list & wait next schedule */
                    audio_src_srv_add_waiting_list(handle);
                } else {
                    audio_src_srv_report("[AudSrc]state_ready(err_play)--type: %d, state: 0x%x\n", 2, ctx->running->type, ctx->running->state);
                }
                audio_src_srv_report("[AudSrc]state_ready(msg_play)--hd: 0x%08x, type: %d, state: 0x%x\n", 3, 
                    ctx->running, ctx->running->type, ctx->running->state);
            } else {
                /* update pseudo device transient state */
                audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_PREPARE_PLAY);
                audio_src_srv_transfer_state(AUDIO_SRC_SRV_STATE_PREPARE_PLAY);
                /* update running pseudo device handle */
                audio_src_srv_update_running_psedev(handle);
                /* accept pseudo device play request */
#if defined(MTK_AUDIO_MANAGER_DEBUG)
                audio_src_srv_report("[AudSrc]state_ready handle(0x%x) type(%d). play handle", 2, handle, handle->type);
#endif
                handle->play(handle);
            }
            break;
        }

        default:
            break;
    }
}


static void audio_src_srv_state_prepare_play_handle(audio_src_srv_handle_t *handle, audio_src_srv_message_t msg_id, void *param)
{
    audio_src_srv_context_t *ctx = audio_src_srv_get_ctx();

    switch (msg_id) {
        case AUDIO_SRC_SRV_MSG_STOP: {
            if (ctx->running == handle) {
                if ((ctx->running->state & AUDIO_SRC_SRV_STATE_MASK) == AUDIO_SRC_SRV_STATE_PREPARE_PLAY) {
                    audio_src_srv_update_psedev_state(ctx->running, AUDIO_SRC_SRV_STATE_PREPARE_STOP);
                    audio_src_srv_transfer_state(AUDIO_SRC_SRV_STATE_PREPARE_STOP);
#if defined(MTK_AUDIO_MANAGER_DEBUG)
                    audio_src_srv_report("[AudSrc]state_prepare_play handle(0x%x) type(%d). stop", 2, ctx->running, ctx->running->type);
#endif
                    ctx->running->stop(ctx->running);
                } else {
                    audio_src_srv_report("[AudSrc]state_ready(err_stop)--type: %d, state: 0x%x\n", 2, ctx->running->type, ctx->running->state);
                }
            } else {
                /* check item in waiting list ?? */
                audio_src_srv_del_waiting_list(handle);
                audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_PREPARE_STOP);
#if defined(MTK_AUDIO_MANAGER_DEBUG)
                    audio_src_srv_report("[AudSrc]state_prepare_play handle(0x%x) type(%d). stop handle", 2, handle, handle->type);
#endif
                handle->stop(handle);
            }
            break;
        }

        case AUDIO_SRC_SRV_MSG_PLAYING: {
            if (ctx->running == handle) {
                /* update pseudo device transient state */
                audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_PLAYING);
                audio_src_srv_transfer_state(AUDIO_SRC_SRV_STATE_PLAYING);
            } else {
                audio_src_srv_report("[AudSrc]state_ready(err_playing)--hd: 0x%x, type: %d, state: 0x%x, coming~~hd: 0x%x, type: %d, state: 0x%x\n", 6,
                    ctx->running, ctx->running->type, ctx->running->state,
                    handle, handle->type, handle->state);
            }
            break;
        }

        case AUDIO_SRC_SRV_MSG_READY: {
            if ((ctx->running) && (ctx->running == handle)) {
                audio_src_srv_report("[AudSrc]state_prepare_play(err_ready)--type: %d, state: 0x%x\n", 2, ctx->running->type, ctx->running->state);
            } else {
                audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_READY);
            }
            break;
        }

        case AUDIO_SRC_SRV_MSG_PLAY: {
            if (ctx->running) {
                if ((ctx->running->state & AUDIO_SRC_SRV_STATE_MASK) == AUDIO_SRC_SRV_STATE_PREPARE_PLAY) {
                    /* interrupt case, cur_state in PREPARE_PLAY */
                    audio_src_srv_psedev_interrupt_handle(handle);
                } else {
                    audio_src_srv_report("[AudSrc]state_prepare_play(err_play)--type: %d, state: 0x%x\n", 2, ctx->running->type, ctx->running->state);
                }
                audio_src_srv_report("[AudSrc]state_prepare_play(msg_play)--hd: 0x%08x, type: %d, state: 0x%x\n", 3,
                    ctx->running, ctx->running->type, ctx->running->state);
            }
            break;
        }

        case AUDIO_SRC_SRV_MSG_UNAVAILABLE: {
            if (ctx->running == handle) {
                /* the same pseudo device */
                if (ctx->running->state == AUDIO_SRC_SRV_STATE_PREPARE_PLAY) {
                    audio_src_srv_update_psedev_state(ctx->running, AUDIO_SRC_SRV_STATE_NONE);
                    audio_src_srv_transfer_state(AUDIO_SRC_SRV_STATE_NONE);
#if defined(MTK_AUDIO_MANAGER_DEBUG)
                    audio_src_srv_report("[AudSrc]state_prepare_play handle(0x%x) type(%d). stop", 2, ctx->running, ctx->running->type);
#endif
                    ctx->running->stop(ctx->running);
                } else {
                    audio_src_srv_report("[AudSrc]state_prepare_play(err2)--type: %d, state: 0x%x\n", 2, ctx->running->type, ctx->running->state);
                }
            } else {
                audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_NONE);
            }
            break;
        }
        default:
            break;
    }
}


static void audio_src_srv_state_prepare_stop_handle(audio_src_srv_handle_t *handle, audio_src_srv_message_t msg_id, void *param)
{
    audio_src_srv_context_t *ctx = audio_src_srv_get_ctx();

    switch (msg_id) {
        case AUDIO_SRC_SRV_MSG_READY: {
            if ((ctx->running) && (ctx->running == handle)) {
                if ((ctx->running->state & AUDIO_SRC_SRV_STATE_MASK) == AUDIO_SRC_SRV_STATE_PREPARE_STOP) {
                    audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_READY);
                    audio_src_srv_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                    audio_src_srv_update_running_psedev(audio_src_srv_get_waiting_psedev());
                    /* Wake up waiting pseudo device */
                    if (ctx->running) {
                        /* Remove handle from waiting list */
                        audio_src_srv_del_waiting_list(ctx->running);
                        /* update pseudo device transient state */
                        audio_src_srv_update_psedev_state(ctx->running, AUDIO_SRC_SRV_STATE_PREPARE_PLAY);
                        audio_src_srv_transfer_state(AUDIO_SRC_SRV_STATE_PREPARE_PLAY);
#if defined(MTK_AUDIO_MANAGER_DEBUG)
                        audio_src_srv_report("[AudSrc]state_prepare_stop handle(0x%x) type(%d). play", 2, ctx->running, ctx->running->type);
#endif
                        ctx->running->play(ctx->running);
                    }
                } else {
                    audio_src_srv_report("[AudSrc]state_prepare_stop(err_ready)--type: %d, state: 0x%x\n", 2, ctx->running->type, ctx->running->state);
                }
            } else {
                audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_READY);
            }
            break;
        }
        case AUDIO_SRC_SRV_MSG_STOP: {
            if ((ctx->running) && (ctx->running == handle)) {
#if defined(MTK_AUDIO_MANAGER_DEBUG)
                    audio_src_srv_report("[AudSrc]state_prepare_stop handle(0x%x) type(%d). stop", 2, ctx->running, ctx->running->type);
#endif
                ctx->running->stop(ctx->running);    /*Try to suspend A2DP*/
            } else {
                audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_PREPARE_STOP);
                audio_src_srv_handle_t *waiting_handle = NULL;
                waiting_handle = audio_src_srv_get_waiting_psedev();
                if((waiting_handle != NULL) && (waiting_handle == handle)){
                    audio_src_srv_del_waiting_list(waiting_handle);
                }
            }
            break;
        }
        case AUDIO_SRC_SRV_MSG_PLAY: {
            if ((ctx->running) && (ctx->running == handle)) {
#if defined(MTK_AUDIO_MANAGER_DEBUG)
                audio_src_srv_report("[AudSrc]state_prepare_stop handle(0x%x) type(%d). stop", 2, ctx->running, ctx->running->type);
#endif
                audio_src_srv_report("[AudSrc]state_prepare_stop handle: unexpected msg_id!", 0);
            } else {
                /* push waiting list & wait next schedule */
                audio_src_srv_add_waiting_list(handle);
            }
            break;
        }
        case AUDIO_SRC_SRV_MSG_UNAVAILABLE: {
            if (ctx->running == handle) {
                /* the same pseudo device */
                if (ctx->running->state == AUDIO_SRC_SRV_STATE_PREPARE_STOP) {
                    audio_src_srv_update_psedev_state(ctx->running, AUDIO_SRC_SRV_STATE_NONE);
                    audio_src_srv_transfer_state(AUDIO_SRC_SRV_STATE_NONE);
                } else {
                    audio_src_srv_report("[AudSrc]state_prepare_stop(err2)--type: %d, state: 0x%x\n", 2, ctx->running->type, ctx->running->state);
                }
            } else {
                audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_NONE);
            }
            break;
        }
        default:
            audio_src_srv_report("[AudSrc]state_prepare_stop handle: unexpected msg_id!", 0);
            break;
    }
}


static void audio_src_srv_state_playing_handle(audio_src_srv_handle_t *handle, audio_src_srv_message_t msg_id, void *param)
{
    audio_src_srv_context_t *ctx = audio_src_srv_get_ctx();

    switch (msg_id) {
        case AUDIO_SRC_SRV_MSG_PLAY: {
            if ((ctx->running->state & AUDIO_SRC_SRV_STATE_MASK) == AUDIO_SRC_SRV_STATE_PLAYING) {
                audio_src_srv_psedev_interrupt_handle(handle);
            } else if ((ctx->running->state & AUDIO_SRC_SRV_STATE_MASK) == AUDIO_SRC_SRV_STATE_PREPARE_STOP) {
                /* push waiting list & wait next schedule */
                audio_src_srv_add_waiting_list(handle);
                audio_src_srv_report("[AudSrc]state_playing(wait)--hd: 0x%08x, type: %d, state: 0x%x\n", 3,
                    ctx->running, ctx->running->type, ctx->running->state);
            } else {
                audio_src_srv_report("[AudSrc]state_playing(err1)--type: %d, state: 0x%x\n", 2, ctx->running->type, ctx->running->state);
            }
            break;
        }

        case AUDIO_SRC_SRV_MSG_UNAVAILABLE: {
            if (ctx->running == handle) {
                /* the same pseudo device */
                if (ctx->running->state == AUDIO_SRC_SRV_STATE_PLAYING) {
                    audio_src_srv_update_psedev_state(ctx->running, AUDIO_SRC_SRV_STATE_PREPARE_STOP);
                    audio_src_srv_transfer_state(AUDIO_SRC_SRV_STATE_PREPARE_STOP);
#if defined(MTK_AUDIO_MANAGER_DEBUG)
                    audio_src_srv_report("[AudSrc]state_playing handle(0x%x) type(%d). stop", 2, ctx->running, ctx->running->type);
#endif
                    ctx->running->stop(ctx->running);
                } else {
                    audio_src_srv_report("[AudSrc]state_playing(err2)--type: %d, state: 0x%x\n", 2, ctx->running->type, ctx->running->state);
                }
            } else {
                audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_NONE);
            }
            break;
        }

        case AUDIO_SRC_SRV_MSG_STOP: {
            if (ctx->running == handle) {
                /* the same pseudo device */
                if (ctx->running->state == AUDIO_SRC_SRV_STATE_PLAYING) {
                    audio_src_srv_update_psedev_state(ctx->running, AUDIO_SRC_SRV_STATE_PREPARE_STOP);
                    audio_src_srv_transfer_state(AUDIO_SRC_SRV_STATE_PREPARE_STOP);
#if defined(MTK_AUDIO_MANAGER_DEBUG)
                    audio_src_srv_report("[AudSrc]state_playing handle(0x%x) type(%d). stop", 2, ctx->running, ctx->running->type);
#endif
                    ctx->running->stop(ctx->running);
                } else {
                    audio_src_srv_report("[AudSrc]state_playing(err2)--type: %d, state: 0x%x\n", 2, ctx->running->type, ctx->running->state);
                }
            } else {
                /* check item in waiting list ?? */
                audio_src_srv_del_waiting_list(handle);
                audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_PREPARE_STOP);
#if defined(MTK_AUDIO_MANAGER_DEBUG)
                audio_src_srv_report("[AudSrc]state_playing handle(0x%x) type(%d). stop handle", 2, handle, handle->type);
#endif
                handle->stop(handle);
            }
            break;
        }

        case AUDIO_SRC_SRV_MSG_PLAYING: {
            break;
        }

        case AUDIO_SRC_SRV_MSG_READY: {
            if ((ctx->running) && (ctx->running == handle)) {
                audio_src_srv_report("[AudSrc]state_playing(err_ready)--type: %d, state: 0x%x\n", 2, ctx->running->type, ctx->running->state);
            } else {
                audio_src_srv_update_psedev_state(handle, AUDIO_SRC_SRV_STATE_READY);
            }
            break;
        }

        default:
            break;
    }
}


void audio_src_srv_update_psedev_state(audio_src_srv_handle_t *handle, audio_src_srv_state_t state)
{
    audio_src_srv_report("[AudSrc]update_psedev_state--type: %d, ori: 0x%x, cur: 0x%x\n", 3, handle->type, handle->state, state);
    handle->state = state;
}


void audio_src_srv_state_machine_handle(audio_src_srv_handle_t *handle, audio_src_srv_message_t msg_id, void *param)
{
    audio_src_srv_context_t *ctx = audio_src_srv_get_ctx();

    audio_src_srv_report("[AudSrc]state_machine_handle--state: 0x%x, msg: %d, hd: 0x%08x, type: %d, dev_state: 0x%x\n", 5,
        ctx->state, msg_id, handle, handle->type, handle->state);

    if (!audio_src_srv_check_psedev_param(handle)) {
        return ;
    }

    switch (ctx->state & AUDIO_SRC_SRV_STATE_MASK) {
        // Add off state because it is the default state. 
        case AUDIO_SRC_SRV_STATE_OFF:
        case AUDIO_SRC_SRV_STATE_NONE: {
            audio_src_srv_state_none_handle(handle, msg_id, param);
            break;
        }

        case AUDIO_SRC_SRV_STATE_READY: {
            audio_src_srv_state_ready_handle(handle, msg_id, param);
            break;
        }

        case AUDIO_SRC_SRV_STATE_PREPARE_PLAY: {
            audio_src_srv_state_prepare_play_handle(handle, msg_id, param);
            break;
        }

        case AUDIO_SRC_SRV_STATE_PREPARE_STOP: {
            audio_src_srv_state_prepare_stop_handle(handle, msg_id, param);
            break;
        }

        case AUDIO_SRC_SRV_STATE_PLAYING:
        {
            audio_src_srv_state_playing_handle(handle, msg_id, param);
            break;
        }
    }
}


audio_src_srv_context_t *audio_src_srv_get_ctx(void)
{
    return &g_audio_src_srv_ctx;
}


void audio_src_srv_update_psedev_play_count(audio_src_srv_handle_t *handle)
{
    handle->play_count = audio_src_srv_gen_play_count();
}


void audio_src_srv_process_psedev_event(audio_src_srv_handle_t *handle, audio_src_srv_event_t evt_id)
{
    audio_src_srv_message_t msg_id = AUDIO_SRC_SRV_MSG_START;
    void *param = NULL;

    switch (evt_id) {
        case AUDIO_SRC_SRV_EVT_UNAVAILABLE: {
            msg_id = AUDIO_SRC_SRV_MSG_UNAVAILABLE;
            break;
        }

        case AUDIO_SRC_SRV_EVT_READY: {
            msg_id = AUDIO_SRC_SRV_MSG_READY;
            break;
        }

        case AUDIO_SRC_SRV_EVT_PLAYING: {
            msg_id = AUDIO_SRC_SRV_MSG_PLAYING;
            break;
        }

        case AUDIO_SRC_SRV_EVT_PREPARE_PLAY: {
            if (handle && handle->state == AUDIO_SRC_SRV_STATE_READY) {
                msg_id = AUDIO_SRC_SRV_MSG_PLAY;
                /* generate play count-->timestamp */
                audio_src_srv_update_psedev_play_count(handle);
            }
            break;
        }

        case AUDIO_SRC_SRV_EVT_PREPARE_STOP: {
            msg_id = AUDIO_SRC_SRV_MSG_STOP;
            break;
        }

        default:
            break;
    }

    if (msg_id > AUDIO_SRC_SRV_MSG_START) {
        //audio_src_srv_update_psedev_state(handle, state);
        audio_src_srv_state_machine_handle(handle, msg_id, param);
    } else {
        if(handle) {
            audio_src_srv_report("[AudSrc]update_state(err)--type: %d, msg_id: %d, evt_id: %d\n", 3, handle->type, msg_id, evt_id);
        } else {
            audio_src_srv_report("[AudSrc]update_state(err)--handle is NULL\n", 0);
        }
    }
}

