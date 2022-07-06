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


#ifndef __RACE_UTIL_H__
#define __RACE_UTIL_H__


#include "race_cmd_feature.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "race_cmd.h"
#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs.h"
#endif


#ifndef UNUSED
#define UNUSED(n)
#endif

/* RECIPIENT parameter */
#define RACE_CMD_RECIPIENT_AGENT (0x01)
#define RACE_CMD_RECIPIENT_PARTNER (0x02)

/* Recipient_count format - RECIPIENT parameter */
#define RACE_RECIPIENT_PARAM_RECIPIENT_AGENT (0x00)
#define RACE_RECIPIENT_PARAM_RECIPIENT_PARTNER (0x01)

#define RACE_DVFS_HANDLE  ("race")

typedef enum
{
    RACE_RECIPIENT_TYPE_NONE,

    RACE_RECIPIENT_TYPE_AGENT_ONLY,
    RACE_RECIPIENT_TYPE_PARTNER_ONLY,
    RACE_RECIPIENT_TYPE_AGENT_PARTNER,

    RACE_RECIPIENT_TYPE_MAX
}race_recipient_type_enum;


typedef enum
{
    RACE_STORAGE_PARTITION_ID_FOTA = 0,
    RACE_STORAGE_PARTITION_ID_FS = 1
}race_storage_partition_id_enum;


typedef enum
{
    RACE_DEVICE_ROLE_LEFT = 0,
    RACE_DEVICE_ROLE_RIGHT,

    RACE_DEVICE_ROLE_MAX = 0xFF
}race_device_role_enum;


typedef enum
{
    RACE_APP_ID_NONE,
    RACE_APP_ID_FOTA,

    RACE_APP_ID_ALL = 0x0F,
    RACE_APP_ID_MAX = RACE_APP_ID_ALL
}race_app_id_enum;


typedef struct race_template_list
{
    struct race_template_list *next;
}race_template_list_struct;


typedef struct
{
    uint8_t recipient;
}PACKED race_recipient_param_general_struct;


typedef void (*race_list_free_func)(void *data);


RACE_ERRCODE race_list_insert(race_template_list_struct **list, race_template_list_struct *list_node);

RACE_ERRCODE race_list_remove(race_template_list_struct **list, race_template_list_struct *list_node);

RACE_ERRCODE race_list_destory(race_template_list_struct **list,
                                   race_list_free_func free_func);

SemaphoreHandle_t race_mutex_create(void);

void race_mutex_free(SemaphoreHandle_t mutex);

RACE_ERRCODE race_mutex_lock(SemaphoreHandle_t mutex);

RACE_ERRCODE race_mutex_unlock(SemaphoreHandle_t mutex);

uint16_t race_gen_process_id(void);

RACE_ERRCODE race_query_partition_info(uint8_t *storage_type,
                                            uint32_t *address,
                                            uint32_t *length,
                                            uint8_t partition_id);

uint32_t race_get_curr_time_in_ms(void);

uint32_t race_get_duration_in_ms(uint32_t start_time, uint32_t end_time);

race_device_role_enum race_get_device_role(void);

uint8_t race_get_battery_level(void);

void *race_pointer_cnv_send_pkt_to_pkt(race_send_pkt_t* pPacket);

race_send_pkt_t *race_pointer_cnv_pkt_to_send_pkt(void* pNoti);

race_recipient_type_enum race_recipient_type_convt(uint8_t recipient);


/* if there's only uint8_t recipient in the recipient parameters, use sizeof(race_recipient_param_general_struct)
  * for recipient_param_size.
  */
RACE_ERRCODE race_recipient_param_parse(uint8_t recipient_count,
                                                uint8_t *recipient_param,
                                                uint8_t recipient_param_size,
                                                uint8_t **agent_recipient_param,
                                                uint8_t **partner_recipient_param);

#endif /* __RACE_UTIL_H__ */

