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

/**
 * File: bt_power_on_config.c
 *
 * Description: The file is to config the bt mode when system power on.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include "FreeRTOS.h"
#include "task.h"

#include "task_def.h"
#include "syslog.h"
#include "bt_power_on_config.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "serial_port.h"
#include "hal_sleep_manager.h"
#include "at_command_bt.h"

static bt_power_on_type_t g_power_on_type = BT_POWER_ON_NORMAL;  /* BT power on type, the default mode is normal mode. */
static uint8_t g_relay_port_number = 0;  /* The relay port number of BT relay. */
extern bool bt_driver_enter_relay_mode(uint8_t port);
extern bool bt_driver_enter_dut_mode(void);
extern atci_bt_relay_callbacks atci_bt_relay_cb;
extern void bt_driver_relay_register_callbacks(void *callback);

void bt_power_on_set_config_type(bt_power_on_type_t power_on_type)
{
    g_power_on_type = power_on_type;
}

bt_power_on_type_t bt_power_on_get_config_type(void)
{
    return g_power_on_type;
}

uint8_t bt_power_on_get_relay_port_number(void)
{
    return g_relay_port_number;
}

void bt_controler_test_task(void *param)
{
    if (BT_POWER_ON_RELAY == bt_power_on_get_config_type()) {
        /* The BT power on mode is configured as BT_POWER_ON_RELAY. */
        uint8_t bt_sleep_manager_handle = hal_sleep_manager_set_sleep_handle("bt");
        hal_sleep_manager_lock_sleep(bt_sleep_manager_handle);
        bt_driver_relay_register_callbacks((void *)&atci_bt_relay_cb);
        bool result = bt_driver_enter_relay_mode(bt_power_on_get_relay_port_number());
        LOG_MSGID_I(common, "[BT]to enable relay mode over, port:%d\r\n", 1, bt_power_on_get_relay_port_number());
        if (result == true) {
            /* BT enter relay mode successed. */
            LOG_MSGID_I(common, "[BT]enable relay mode success!!!\n", 0);
        } else {
            /* BT enter relay mode failed. */
            LOG_MSGID_I(common, "[BT]enable relay mode fail!!!\n", 0);
        }
    } else if (BT_POWER_ON_DUT == bt_power_on_get_config_type()) {
        /* The BT power on mode is configured as BT_POWER_ON_DUT. */
        bool result = bt_driver_enter_dut_mode();
        if (result == true) {
            /* BT enter dut mode successed. */
            LOG_MSGID_I(common, "[BT]enable dut mode success!!!\n", 0);
        } else {
            /* BT enter dut mode failed. */
            LOG_MSGID_I(common, "[BT]enable dut mode fail!!!\n", 0);
        }
    }
    vTaskDelete(NULL);
}


void bt_power_on_config_init(void)
{
    bool dut_config = false;
    bt_power_on_relay_config_t relay_config = {
        .relay_enable = false,
        .port_number = 0,
    };
    uint32_t relay_size = sizeof(bt_power_on_relay_config_t);
    uint32_t dut_size = sizeof(bool);
    nvkey_status_t ret = nvkey_read_data(NVKEYID_BT_RELAY_ENABLE, (uint8_t *)(&relay_config), &relay_size);
    LOG_MSGID_I(common, "ret:%d, relay_enable:%d, port_number:%d\r\n", 3, ret, relay_config.relay_enable, relay_config.port_number);
    ret = nvkey_read_data(NVKEYID_BT_DUT_ENABLE, (uint8_t *)(&dut_config), &dut_size);
    LOG_MSGID_I(common, "ret:%d, dut_config:%d\r\n", 2, ret, dut_config);
    if (relay_config.relay_enable) {
        /* Get BT power on mode from NVDM, set BT power on mode as BT_POWER_ON_RELAY. */
        bt_power_on_set_config_type(BT_POWER_ON_RELAY);
        g_relay_port_number = relay_config.port_number;
    } else if (dut_config) {
        /* Get BT power on mode from NVDM, set BT power on mode as BT_POWER_ON_DUT. */
        bt_power_on_set_config_type(BT_POWER_ON_DUT);
    } else {
        /* Get noting from NVDM for BT power on config, set BT power on mode as default BT_POWER_ON_NORMAL. */
        bt_power_on_set_config_type(BT_POWER_ON_NORMAL);
    }
}


