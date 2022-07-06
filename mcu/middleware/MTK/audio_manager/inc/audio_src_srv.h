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

#ifndef __AUDIO_SRC_SRV_H__
#define __AUDIO_SRC_SRV_H__

#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This macro defines the mak of state.
 */
#define AUDIO_SRC_SRV_STATE_MASK  (0xFF000000)

/**
 * @brief This structure defines multi-source state.
 */
typedef enum {
    AUDIO_SRC_SRV_STATE_OFF          = 0<<24,
    AUDIO_SRC_SRV_STATE_NONE         = 1<<24,
    AUDIO_SRC_SRV_STATE_READY        = 2<<24,
    AUDIO_SRC_SRV_STATE_PREPARE_PLAY = 3<<24,
    AUDIO_SRC_SRV_STATE_PLAYING      = 4<<24,
    AUDIO_SRC_SRV_STATE_PREPARE_STOP = 5<<24,
} audio_src_srv_state_t;

/**
 * @brief This structure defines pseudo device type.
 */
typedef enum {
    AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP = 0,
    AUDIO_SRC_SRV_PSEUDO_DEVICE_HFP,
    AUDIO_SRC_SRV_PSEUDO_DEVICE_MP3,
    AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP,
    AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_HFP,
    AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_MP3,
    AUDIO_SRC_SRV_PSEUDO_DEVICE_VP,  //Not use.
    AUDIO_SRC_SRV_PSEUDO_DEVICE_LINE_IN,
    AUDIO_SRC_SRV_PSEUDO_DEVICE_USB_AUDIO,
    AUDIO_SRC_SRV_PSEUDO_DEVICE_BLE,
    AUDIO_SRC_SRV_PSEUDO_DEVICE_ULL_A2DP,
} audio_src_srv_pseudo_device_t;

/**
 * @brief This macro defines multi-source priority base.
 */
#define AUDIO_SRC_SRV_FLAG_USED                     (1 << 0)    /* Pseudo device handle used */
#define AUDIO_SRC_SRV_FLAG_WAITING                  (1 << 1)    /* Pseudo device in waiting list */

#define AUDIO_SRC_SRV_PSEUDO_DEVICE_NUM             (3 /* Sink A2DP */ + 2 /* Sink HFP */ + 3 /* A2DP_AWS */ + 1 /* HFP_AWS */ + 1 /* Sink ULL A2DP */ + 2 /* BLE Music*/ + 2 /* BLE Call*/ + 1 /* BLE Broadcast*/  + 1 /* line-in */ + 1 /* USB audio */ + 4 /* Reserve */)


/**
 * @brief This structure defines multi-source priority.
 */
typedef enum {
    AUDIO_SRC_SRV_PRIORITY_LOW = 0,
    AUDIO_SRC_SRV_PRIORITY_NORMAL = 2,
    AUDIO_SRC_SRV_PRIORITY_ABOVE_NORMAL = 3,
    AUDIO_SRC_SRV_PRIORITY_MIDDLE = 4,
    AUDIO_SRC_SRV_PRIORITY_HIGH = 6,
} audio_src_srv_priority_t;


/**
 * @brief This structure defines multi-source event ID.
 */
typedef enum {
    AUDIO_SRC_SRV_EVT_UNAVAILABLE,
    AUDIO_SRC_SRV_EVT_READY,
    AUDIO_SRC_SRV_EVT_PREPARE_PLAY,
    AUDIO_SRC_SRV_EVT_PREPARE_STOP,
    AUDIO_SRC_SRV_EVT_PLAYING,
} audio_src_srv_event_t;


/**
 * @brief This structure defines multi-source handle.
 */
typedef struct _audio_src_srv_handle_t {
    audio_src_srv_state_t state;
    uint32_t substate;
    audio_src_srv_pseudo_device_t type;
    uint32_t play_count;
    uint16_t flag;

    /* source should write follownig members */
    uint8_t priority;
    uint64_t dev_id;        /* Bluetooth address */
    void (*play)(struct _audio_src_srv_handle_t *handle);
    void (*stop)(struct _audio_src_srv_handle_t *handle);
    void (*suspend)(struct _audio_src_srv_handle_t *handle, struct _audio_src_srv_handle_t *int_hd);
    void (*reject)(struct _audio_src_srv_handle_t *handle);
    void (*exception_handle)(struct _audio_src_srv_handle_t *handle, int32_t event, void *param);
} audio_src_srv_handle_t;

/**
 * @brief     This function is to register audio pseudo device service handler.
 *
 * @param[in] type      audio_src_srv_pseudo_device_t
 * @return     Handler pointer.
 * @sa         #audio_src_srv_construct_handle()
 */
audio_src_srv_handle_t *audio_src_srv_construct_handle(audio_src_srv_pseudo_device_t type);

/**
 * @brief     This function is to deregister audio pseudo device service handler.
 *
 * @param[in] handle      Handler pointer.
 * @return     void
 * @sa         #audio_src_srv_destruct_handle()
 */
void audio_src_srv_destruct_handle(audio_src_srv_handle_t *handle);

/**
 * @brief     This function is to send event request to audio pseudo device service.
 *
 * @param[in] handle      Handler pointer.
 * @param[in] evt_id       Audio pseudo device event.
 * @return     void
 * @sa         #audio_src_srv_update_state()
 */
void audio_src_srv_update_state(audio_src_srv_handle_t *handle, audio_src_srv_event_t evt_id);

void audio_src_srv_set_substate(audio_src_srv_handle_t *handle, uint32_t substate);

const audio_src_srv_handle_t *audio_src_srv_get_pseudo_device(void);
const audio_src_srv_handle_t *audio_src_srv_get_runing_pseudo_device(void);

void audio_src_srv_add_waiting_list(audio_src_srv_handle_t *handle);

void audio_src_srv_del_waiting_list(audio_src_srv_handle_t *handle);

bool audio_src_srv_psedev_compare(audio_src_srv_handle_t *cur, audio_src_srv_handle_t *coming);

void audio_src_srv_mutex_lock(void);

void audio_src_srv_mutex_unlock(void);

////////////////////////////////////////////////////////////
#include "audio_src_srv_resource_manager_config.h"

typedef enum {
    AUDIO_SRC_SRV_EVENT_NONE         = 0x10,
    AUDIO_SRC_SRV_EVENT_TAKE_ALREADY = 0x11,
    AUDIO_SRC_SRV_EVENT_GIVE_ERROR   = 0x12,//no need give or give already
    AUDIO_SRC_SRV_EVENT_TAKE_SUCCESS = 0,
    AUDIO_SRC_SRV_EVENT_TAKE_REJECT  = 1,
    AUDIO_SRC_SRV_EVENT_GIVE_SUCCESS = 2,
    AUDIO_SRC_SRV_EVENT_SUSPEND      = 3
} audio_src_srv_resource_manager_event_t;

typedef struct _audio_src_srv_resource_manager_handle_t {
    /* Internally used */
    uint32_t play_count;
    uint16_t flag;
    audio_src_srv_resource_manager_event_t state;

    /* User should write follownig members */
    audio_src_srv_resource_type_t resource_type;
    const char *handle_name;
    uint8_t priority;
    void (*callback_func)(struct _audio_src_srv_resource_manager_handle_t *current_handle, audio_src_srv_resource_manager_event_t event);
} audio_src_srv_resource_manager_handle_t;


audio_src_srv_resource_manager_handle_t *audio_src_srv_resource_manager_construct_handle(audio_src_srv_resource_type_t resource_type, const char *handle_name);

void audio_src_srv_resource_manager_destruct_handle(audio_src_srv_resource_manager_handle_t *handle);

audio_src_srv_resource_manager_handle_t *audio_src_srv_resource_manager_get_current_running_handle(audio_src_srv_resource_type_t resource_type);

void audio_src_srv_resource_manager_add_waiting_list(audio_src_srv_resource_manager_handle_t *handle);

void audio_src_srv_resource_manager_delete_waiting_list(audio_src_srv_resource_manager_handle_t *handle);

void audio_src_srv_resource_manager_take(audio_src_srv_resource_manager_handle_t *handle);

void audio_src_srv_resource_manager_give(audio_src_srv_resource_manager_handle_t *handle);


#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_SRC_SRV_H__ */

