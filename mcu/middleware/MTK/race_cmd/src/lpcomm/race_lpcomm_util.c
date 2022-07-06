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


#include "race_cmd_feature.h"
#ifdef RACE_LPCOMM_ENABLE
#include "race_lpcomm_util.h"
#include "race_lpcomm_trans.h"
#include "race_lpcomm_aws.h"
#include "race_timer.h"
#include "race_event_internal.h"


typedef struct
{
    race_serial_port_type_enum cmd_port_type;
    bool role_switch_enable;
    uint8_t rho_timer_id;
} race_lpcomm_role_switch_info_struct;


race_lpcomm_role_switch_info_struct g_race_lpcomm_role_switch_info;


race_lpcomm_role_enum race_lpcomm_role_get(race_lpcomm_trans_method_enum trans_method)
{
    switch (trans_method)
    {
#ifdef RACE_AWS_ENABLE
        case RACE_LPCOMM_TRANS_METHOD_AWS:
        {
            return race_aws_get_aws_mce_role();
       }
#endif
        default:
            break;

    }

    return RACE_LPCOMM_ROLE_MAX;
}


/* It may need be protected by the mutex */
void race_lpcomm_set_role_switch_enable(bool enable)
{
    g_race_lpcomm_role_switch_info.role_switch_enable = enable;
}


/* It may need be protected by the mutex */
bool race_lpcomm_get_role_switch_enable(void)
{
    return g_race_lpcomm_role_switch_info.role_switch_enable;
}


void race_lpcomm_set_rho_timer_id(uint8_t timer_id)
{
    g_race_lpcomm_role_switch_info.rho_timer_id = timer_id;
}


/* It may need be protected by the mutex */
uint8_t race_lpcomm_get_rho_timer_id(void)
{
    if (race_timer_smart_is_enabled(g_race_lpcomm_role_switch_info.rho_timer_id))
    {
        return g_race_lpcomm_role_switch_info.rho_timer_id;
    }

    g_race_lpcomm_role_switch_info.rho_timer_id = RACE_TIMER_INVALID_TIMER_ID;

    return RACE_TIMER_INVALID_TIMER_ID;
}


void race_lpcomm_rho_timer_expiration_hdl(uint8_t id, void *user_data)
{
    RACE_LOG_MSGID_I("race_lpcomm_rho_timer_expiration_hdl", 0);

    if (id != g_race_lpcomm_role_switch_info.rho_timer_id)
    {
        RACE_LOG_MSGID_W("Timer id not match. id:%d rho_timer_id:%d",2,
                   id, g_race_lpcomm_role_switch_info.rho_timer_id);
        /* Must not reset timer id for it may be the previous timer which is not stopped properly. */
        race_timer_smart_stop(id, NULL);
        return;
    }

    if (race_lpcomm_get_role_switch_enable())
    {
        race_lpcomm_set_role_switch_enable(FALSE);
        /* RHO Fail */
        RACE_LOG_MSGID_E("RHO result:0", 0);
        race_event_send_bt_rho_result_event(FALSE);
        //race_lpcomm_set_role_switch_cmd_port_type(RACE_SERIAL_PORT_TYPE_NONE);
    }

    race_timer_smart_stop(id, NULL);
    g_race_lpcomm_role_switch_info.rho_timer_id = RACE_TIMER_INVALID_TIMER_ID;
}


void race_lpcomm_set_role_switch_cmd_port_type(race_serial_port_type_enum cmd_port_type)
{
    g_race_lpcomm_role_switch_info.cmd_port_type = cmd_port_type;
}


race_serial_port_type_enum race_lpcomm_get_role_switch_cmd_port_type(void)
{
    return g_race_lpcomm_role_switch_info.cmd_port_type;
}

#endif /* RACE_LPCOMM_ENABLE */

