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

#ifndef __RACE_LPCOMM_UTIL__
#define __RACE_LPCOMM_UTIL__


#include "race_cmd_feature.h"
#ifdef RACE_LPCOMM_ENABLE
#include "race_lpcomm_trans.h"
#include "race_util.h"
#include "race_xport.h"


#define RACE_LPCOMM_MAX_CONN_NUM  (1)


/* role will be used in the high 4 bits of packet_type. Therefore, it must not exceed 0x0F. */
typedef enum
{
    RACE_LPCOMM_ROLE_NONE = 0,
    RACE_LPCOMM_ROLE_AGENT,
    RACE_LPCOMM_ROLE_PARTNER,

    RACE_LPCOMM_ROLE_MAX = 0x0F /**< Must not exceed 0x0F */
}race_lpcomm_role_enum;


race_lpcomm_role_enum race_lpcomm_role_get(race_lpcomm_trans_method_enum trans_method);

void race_lpcomm_set_role_switch_enable(bool enable);

bool race_lpcomm_get_role_switch_enable(void);

void race_lpcomm_set_role_switch_cmd_port_type(race_serial_port_type_enum cmd_port_type);

race_serial_port_type_enum race_lpcomm_get_role_switch_cmd_port_type(void);

void race_lpcomm_set_rho_timer_id(uint8_t timer_id);

/* It may need be protected by the mutex */
uint8_t race_lpcomm_get_rho_timer_id(void);

void race_lpcomm_rho_timer_expiration_hdl(uint8_t id, void *user_data);

#endif /* RACE_LPCOMM_ENABLE */
#endif /* __RACE_LPCOMM_UTIL__ */

