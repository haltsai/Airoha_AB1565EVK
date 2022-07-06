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

#ifndef __NVDM_PORT_H__
#define __NVDM_PORT_H__

#ifdef MTK_NVDM_ENABLE

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#ifdef __EXT_BOOTLOADER__
#include "bl_common.h"
#include <string.h>
#endif

//#define NVDM_INTERNAL_DEBUG

#ifndef __EXT_BOOTLOADER__
void nvdm_log_info(const char *message, ...);
void nvdm_log_warning(const char *message, ...);
void nvdm_log_error(const char *message, ...);
void nvdm_log_msgid_info(const char *message, uint32_t arg_cnt, ...);
void nvdm_log_msgid_warning(const char *message, uint32_t arg_cnt, ...);
void nvdm_log_msgid_error(const char *message, uint32_t arg_cnt, ...);
#else /* __EXT_BOOTLOADER__ */

#ifndef NVDM_INTERNAL_DEBUG
/* Define an empty macro function to trigger the gc-section
 * and reduce the size of the NVDM driver.
 */
#define nvdm_log_info(message, ...)
#else /* NVDM_INTERNAL_DEBUG */
#define nvdm_log_info(message, ...)                       bl_print(LOG_INFO, message, ##__VA_ARGS__)
#endif /* NVDM_INTERNAL_DEBUG */

#define nvdm_log_warning(message, ...)                    bl_print(LOG_WARN, message, ##__VA_ARGS__)

#define nvdm_log_error(message, ...) \
            bl_print(LOG_ERROR, message, ##__VA_ARGS__); \
            assert(0)

#ifndef NVDM_INTERNAL_DEBUG
/* Define an empty macro function to trigger the gc-section
 * and reduce the size of the NVDM driver.
 */
#define nvdm_log_msgid_info(message, arg_cnt, ...)
#else /* NVDM_INTERNAL_DEBUG */
#define nvdm_log_msgid_info(message, arg_cnt, ...)        bl_print(LOG_INFO, message, ##__VA_ARGS__)
#endif /* NVDM_INTERNAL_DEBUG */

#define nvdm_log_msgid_warning(message, arg_cnt, ...)     bl_print(LOG_WARN, message, ##__VA_ARGS__)

#define nvdm_log_msgid_error(message, arg_cnt, ...) \
            bl_print(LOG_ERROR, message, ##__VA_ARGS__); \
            assert(0)
#endif /* __EXT_BOOTLOADER__ */

/* NVDM port functions declare */
void nvdm_port_flash_read(uint32_t address, uint8_t *buffer, uint32_t length);
void nvdm_port_flash_write(uint32_t address, const uint8_t *buffer, uint32_t length);
void nvdm_port_flash_erase(uint32_t address);
void *nvdm_port_malloc(uint32_t size);
void nvdm_port_free(void *pdata);
void nvdm_port_mutex_creat(void);
void nvdm_port_mutex_take(void);
void nvdm_port_mutex_give(void);

void nvdm_port_protect_mutex_create(void);
void nvdm_port_protect_mutex_take(void);
void nvdm_port_protect_mutex_give(void);

const char* nvdm_port_get_curr_task_name(void);
void nvdm_port_must_assert(void);

void nvdm_port_get_task_handler(void);
void nvdm_port_reset_task_handler(void);
bool nvdm_port_query_task_handler(void);

bool nvdm_port_send_queue(void);
uint32_t nvdm_port_get_data_item_config(uint32_t *max_data_item_size,
                                        uint32_t *max_group_name_size,
                                        uint32_t *max_data_item_name_size);
uint32_t nvdm_port_get_peb_config(uint32_t *peb_count);
uint32_t nvdm_port_get_peb_address(int32_t pnum, int32_t offset);
void nvdm_port_poweroff_time_set(void);
void nvdm_port_poweroff(uint32_t poweroff_time);


uint32_t nvdm_port_get_count(void);

/* The unit of time is microseconds. */
uint32_t nvdm_port_get_duration_time(uint32_t begin, uint32_t end);

#endif

#endif

