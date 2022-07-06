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

#ifndef __BT_FAST_PAIR_UTILITY_H__
#define __BT_FAST_PAIR_UTILITY_H__
#include "bt_type.h"

#define BT_FAST_PAIR_DEBUG

#ifdef BT_FAST_PAIR_DEBUG
void bt_fast_pair_log_msgid_i(const char *msg, uint32_t arg_cnt, ...);
void bt_fast_pair_log_msgid_w(const char *msg, uint32_t arg_cnt, ...);
void bt_fast_pair_log_msgid_e(const char *msg, uint32_t arg_cnt, ...);
void bt_fast_pair_log_msgid_d(const char *msg, uint32_t arg_cnt, ...);

/**
 * Define BT log with msg id.
 */
#define BT_FAST_PAIR_LOG_I(msg, arg_cnt, ...) bt_fast_pair_log_msgid_i(msg, arg_cnt, ## __VA_ARGS__)
#define BT_FAST_PAIR_LOG_W(msg, arg_cnt, ...) bt_fast_pair_log_msgid_w(msg, arg_cnt, ## __VA_ARGS__)
#define BT_FAST_PAIR_LOG_E(msg, arg_cnt, ...) bt_fast_pair_log_msgid_e(msg, arg_cnt, ## __VA_ARGS__)
#define BT_FAST_PAIR_LOG_D(msg, arg_cnt, ...) bt_fast_pair_log_msgid_d(msg, arg_cnt, ## __VA_ARGS__)
#else
#define BT_FAST_PAIR_LOG_I(...)
#define BT_FAST_PAIR_LOG_W(...)
#define BT_FAST_PAIR_LOG_E(...)
#define BT_FAST_PAIR_LOG_D(...)
#endif

#define BT_FAST_PAIR_SPP_EVENT_CONNECTED        (0x01)
#define BT_FAST_PAIR_SPP_EVENT_DISCONNECTED     (0x02)
#define BT_FAST_PAIR_SPP_EVENT_DATA_IND         (0x03)
#define BT_FAST_PAIR_SPP_EVENT_BT_POWER_ON      (0x04)
#define BT_FAST_PAIR_SPP_EVENT_BT_POWER_OFF     (0x05)
typedef uint8_t bt_fast_pair_spp_event_t;

void                bt_fast_pair_spp_init(void);
void                bt_fast_pair_spp_event_handle_cb(bt_fast_pair_spp_event_t evt, bt_status_t status, bt_bd_addr_t *addr, uint8_t* data, uint16_t data_length);
bt_status_t         bt_fast_pair_spp_data_send(bt_bd_addr_t* addr, uint8_t *data, uint16_t data_length);
bt_status_t         bt_fast_pair_spp_disconnect(bt_bd_addr_t* addr);

typedef enum {
    BT_FAST_PAIR_NVKEY_STA_SUCCESS = 0x0,
    BT_FAST_PAIR_NVKEY_STA_FAIL = 0x1,
    BT_FAST_PAIR_NVKEY_STA_NOT_FOUND = 0x2
} bt_fast_pair_nvkey_sta_t;

typedef enum {
    BT_FAST_PAIR_NVDM_STA_SUCCESS = 0x0,
    BT_FAST_PAIR_NVDM_STA_FAIL = 0x1,
    BT_FAST_PAIR_NVDM_STA_NOT_FOUND = 0x2
} bt_fast_pair_nvdm_sta_t;

#define NVKEYID_SPOT_COUNTER 0xF316
#define NVKEYID_SPOT_EPHEMERAL_KEY 0xF315
#define NVID_APP_FPS_SPOT_LAST_EID 0xF320

typedef void (*spot_timer_callback)(void *timer_handle);

bt_fast_pair_nvdm_sta_t bt_fast_pair_nvdm_read(const char *group_name, const char *item_name, uint8_t *buf, uint32_t size);
bt_fast_pair_nvkey_sta_t bt_fast_pair_nvkey_read(uint16_t id, uint8_t *buf, uint32_t* size);
bt_fast_pair_nvkey_sta_t bt_fast_pair_nvkey_write(uint16_t id, uint8_t *data, uint32_t size);

void *bt_fast_pair_spot_sys_timer_create(char* timer_name, bool repeat, uint32_t period_ms,
                                         void* usr_data, spot_timer_callback cb);
bool bt_fast_pair_spot_sys_timer_start(void *timer_handle);
bool bt_fast_pair_spot_sys_timer_stop(void *timer_handle);
bool bt_fast_pair_spot_sys_timer_delete(void *timer_handle);

uint32_t bt_fast_pair_spot_sys_run_time_second(void);
void bt_fast_pair_hex_dump(const char *msg, unsigned char *buf, unsigned int buf_len);

typedef void (*bt_fast_pair_task_function)(void *param);

typedef struct {
    bt_fast_pair_task_function      function;
    char                            name[10];
    unsigned int                    stack_depth;
    void                           *parameter;
    bool                            is_high_priority;
} bt_fast_pair_task_t;

void        bt_fast_pair_task_create(bt_fast_pair_task_t *task);
void        bt_fast_pair_task_destroy();
uint32_t    bt_fast_pair_task_get_running_ms();

#endif /* __BT_FAST_PAIR_UTILITY_H__  */

