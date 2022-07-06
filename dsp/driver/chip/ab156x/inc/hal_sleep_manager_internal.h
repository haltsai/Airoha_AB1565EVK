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
 
#ifndef __HAL_SLEEP_MANAGER_INTERNAL_H__
#define __HAL_SLEEP_MANAGER_INTERNAL_H__
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_platform.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include <stdio.h>
#include <string.h>
#include "hal_log.h"
#include "memory_attribute.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_spm.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "hal_core_status.h"

//#define SLEEP_MANAGEMENT_DEBUG_ENABLE
//#define SLEEP_MANAGEMENT_DEBUG_SLEEP_WAKEUP_LOG_ENABLE

typedef enum {
    UNLOCK_SLEEP    = 0,
    LOCK_SLEEP      = 1
} sleep_management_lock_sleep_t;

typedef enum {
    SLEEP_MANAGEMENT_UNINITIALIZED  = 0,
    SLEEP_MANAGEMENT_INITIALIZED    = 1
} sleep_management_init_status_t;

/* Lock Sleep Handle ID : 0~31 */
#define  SLEEP_LOCK_HANDLE_MAX          32
#define  SLEEP_LOCK_HANDLE_USER_MAX     (SLEEP_LOCK_HANDLE_MAX-SLEEP_LOCK_USER_START_ID)
#define  SLEEP_HANDLE_NAME_LEN          11

typedef struct {
    uint32_t    lock_sleep_request;
    uint32_t    user_handle_resoure;
    uint8_t     user_handle_count;
    uint8_t     lock_sleep_request_count[SLEEP_LOCK_HANDLE_MAX];
    uint8_t     user_handle_name[SLEEP_LOCK_HANDLE_USER_MAX][SLEEP_HANDLE_NAME_LEN];
} sleep_management_handle_t;

typedef void(* sleep_management_suspend_callback_t)(void *data);
typedef void(* sleep_management_resume_callback_t)(void *data);

typedef struct {
    sleep_management_suspend_callback_t func;
    void *data;
    bool init_status;
} sleep_management_suspend_callback_func_t;

typedef struct {
    sleep_management_resume_callback_t func;
    void *data;
    bool init_status;
} sleep_management_resume_callback_func_t;

typedef struct {
    uint32_t wakeup_source;
    uint32_t abort_sleep;
    uint32_t pshut_off;
    uint32_t bootvector_backup;
    uint32_t bootvector_address;
} sleep_management_dsp_status_t;
extern sleep_management_dsp_status_t sleep_management_dsp_status;

ATTR_TEXT_IN_IRAM void sleep_management_enter_deep_sleep(void);

void sleep_management_register_suspend_callback(sleep_management_backup_restore_module_t module , sleep_management_suspend_callback_t callback , void *data);
void sleep_management_register_resume_callback(sleep_management_backup_restore_module_t module , sleep_management_resume_callback_t callback , void *data);
void sleep_management_suspend_callback(void);
void sleep_management_resume_callback(void);
uint8_t sleep_management_get_lock_handle(const char *handle_name);
void sleep_management_release_lock_handle(uint8_t handle_index);
void sleep_management_lock_sleep(sleep_management_lock_sleep_t lock, uint8_t handle_index);
bool sleep_management_check_sleep_locks(void);
bool sleep_management_check_handle_status(uint8_t handle_index);
uint32_t sleep_management_get_lock_sleep_request_info(void);
uint32_t sleep_management_get_lock_sleep_handle_list(void);
void sleep_management_dump_sleep_handel_name(uint32_t handle_index);
//void sleep_management_dump_debug_log(sleep_management_debug_log_index_t log_index);

#ifdef SLEEP_MANAGEMENT_DEBUG_ENABLE
void sleep_management_debug_lock_sleep_timelog(sleep_management_lock_sleep_t lock, uint8_t handle_index);
void sleep_management_debug_backup_restore_fun_timelog(uint32_t type, uint32_t order, uint32_t callback);
void sleep_management_debug_dump_lock_sleep_time(void);
void sleep_management_debug_reset_lock_sleep_time(void);
void sleep_management_debug_dump_backup_restore_time(void);
void sleep_management_dump_wakeup_source(uint32_t wakeup_source, uint32_t eint_status);
void sleep_management_dump_eint_wakeup_source(uint32_t eint_num);
#endif

#endif
#endif

