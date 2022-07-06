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


#ifndef __RACE_EVENT_INTERNAL_H__
#define __RACE_EVENT_INTERNAL_H__

#include "race_cmd_feature.h"
#include "race_cmd.h"
#include "race_event.h"
#include "race_xport.h"


#define RACE_EVENT_REGISTER_ID_BASE       (1)
#define RACE_EVENT_REGISTER_MAX_NUM       (6)

#define RACE_EVENT_REGISTER_ID_MIN       (RACE_EVENT_REGISTER_ID_BASE)
#define RACE_EVENT_REGISTER_ID_MAX       (RACE_EVENT_REGISTER_ID_BASE + RACE_EVENT_REGISTER_MAX_NUM -1)

/* ~[BASE, BASE + MAX_NUM) */
#define RACE_EVENT_INVALID_REGISTER_ID    (0)


void race_event_register_notify(race_event_type_enum event_type, void *param);

RACE_ERRCODE race_send_event_notify_msg(race_event_type_enum event_type, void *param);

#ifdef RACE_FOTA_CMD_ENABLE
RACE_ERRCODE race_event_send_fota_start_event(bool is_dual_fota,
                                                       bool is_active_fota);

RACE_ERRCODE race_event_send_fota_cancelling_event(race_fota_stop_originator_enum originator,
                                                   race_fota_stop_reason_enum reason);

RACE_ERRCODE race_event_send_fota_cancel_event(bool result,
                                               race_fota_stop_originator_enum originator,
                                               race_fota_stop_reason_enum reason);

RACE_ERRCODE race_event_send_fota_need_reboot_event(uint8_t *address, uint32_t address_length);
#endif

#ifdef RACE_BT_CMD_ENABLE
RACE_ERRCODE race_event_send_bt_rho_result_event(bool result);
#endif

void race_event_notify_msg_process(race_general_msg_t *msg);

void race_event_init(void);

#ifdef RACE_FIND_ME_ENABLE
RACE_ERRCODE race_event_send_find_me_event(uint8_t is_blink, uint8_t is_tone);
#endif
#endif /* __RACE_EVENT_INTERNAL_H__ */

