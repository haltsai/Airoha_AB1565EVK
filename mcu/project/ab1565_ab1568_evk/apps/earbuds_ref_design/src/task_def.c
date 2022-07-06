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
 * File: task_def.c
 *
 * Description: This file implements the functions defined in the task_def.h file.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include "FreeRTOS.h"
#include "task.h"

#include "task_def.h"
#include "syslog.h"

/* Dsp0 nvdm include. */
#include "audio_nvdm_common.h"
#include "audio_nvdm_coef_default.h"

#define ATCI_DEMO
#define BT_SINK_DEMO

#define SLEEP_MANAGER_LOW_POWER_CONFIG

#include "bt_platform.h"
#ifdef BT_SINK_DEMO
#include "bt_sink_srv_am_task.h"
#endif /* BT_SINK_DEMO */

#ifdef ATCI_DEMO
#include "atci.h"

#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
#include "serial_port.h"
#endif
#ifdef MTK_AUDIO_MP3_ENABLED
#include "audio_player_main.h"
#endif

#include "serial_port_assignment.h"
#ifdef MTK_RACE_CMD_ENABLE
#include "race_xport.h"
#endif
#include "bt_type.h"
#include "bt_init.h"
#include "hal_audio.h"
#include "bt_power_on_config.h"
void atci_def_task(void *param)
{
    while (1) {
        atci_processing();
    }
}
#endif

#if defined GSOUND_LIBRARY_ENABLE && defined GSOUND_TARGET_TASK_ENABLE
#include "gsound_port_interface.h"
#endif

/****************************************************************************
 * Types.
 ****************************************************************************/

typedef struct tasks_list_ {
    TaskFunction_t      pvTaskCode;
    char                *pcName;
    uint16_t            usStackDepth;
    void                *pvParameters;
    UBaseType_t         uxPriority;
} tasks_list_t;

/****************************************************************************
 * Forward Declarations.
 ****************************************************************************/
extern void *AUDIO_SRC_SRV_AM_TASK;
extern void bt_task(void *arg);

#ifdef MTK_AMA_ENABLE
extern void ama_port_task_main(void *arg);
extern void ama_core_task_handler(void *arg);
#endif
extern void app_ui_realtime_task(void *arg);

static const tasks_list_t tasks_list[] = {
    { app_ui_realtime_task, UI_REALTIME_TASK_NAME,   UI_REALTIME_TASK_STACKSIZE,  NULL,  UI_REALTIME_TASK_PRIO },
    { bt_task,      BT_TASK_NAME,   BT_TASK_STACKSIZE,  NULL,   BT_TASK_PRIORITY },
    { atci_def_task, ATCI_TASK_NAME, ATCI_TASK_STACKSIZE, NULL,   ATCI_TASK_PRIO },
#ifdef MTK_RACE_CMD_ENABLE
    { race_task,   RACE_TASK_NAME,   RACE_TASK_STACKSIZE,   NULL,   RACE_TASK_PRIO },
#endif
#ifdef BT_SINK_DEMO
    { am_task_main, AM_TASK_NAME,   AM_TASK_STACKSIZE,  NULL,   AM_TASK_PRIO },
#ifdef MTK_MP3_TASK_DEDICATE
    // { mp3_codec_task_main, MP3_CODEC_TASK_NAME,  MP3_CODEC_TASK_STACKSIZE, NULL,   MP3_CODEC_TASK_PRIO},
#endif
#endif
    { bt_controler_test_task, CONTROLER_TEST_NAME,  CONTROLER_TEST_STACKSIZE, NULL,   CONTROLER_TEST_PRIORITY},
#if defined GSOUND_LIBRARY_ENABLE && defined GSOUND_TARGET_TASK_ENABLE
    { gsound_port_task_main, GSOUND_TARGET_TASK_NAME, GSOUND_TARGET_TASK_STACK_POOL, NULL, GSOUND_TARGET_TASK_PRIORITY},
#endif

#ifdef MTK_AMA_ENABLE
    {ama_port_task_main, AMA_TARGET_TASK_NAME, AMA_TARGET_TASK_STACK_SIZE, NULL, AMA_TARGET_TASK_PRIORITY},
    {ama_core_task_handler, AMA_CORE_TASK_NAME, AMA_CORE_TASK_STACK_SIZE, NULL, AMA_CORE_TASK_PRIORITY},
#endif
};

#define tasks_list_count  (sizeof(tasks_list) / sizeof(tasks_list_t))

static TaskHandle_t     task_handles[tasks_list_count];

/****************************************************************************
 * Private Functions.
 ****************************************************************************/

/****************************************************************************
 * Public API.
 ****************************************************************************/
extern size_t xPortGetFreeHeapSize(void);


void task_def_init(void)
{
#ifdef ATCI_DEMO
#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)

    serial_port_dev_t port;
    serial_port_setting_uart_t uart_setting;

    if (serial_port_config_read_dev_number("atci", &port) != SERIAL_PORT_STATUS_OK) {
        port = CONFIG_ATCI_PORT;
        serial_port_config_write_dev_number("atci", port);
        LOG_MSGID_W(common, "serial_port_config_write_dev_number setting uart1", 0);
        uart_setting.baudrate = CONFIG_ATCI_BAUDRATE;
        serial_port_config_write_dev_setting(port, (serial_port_dev_setting_t *)&uart_setting);
    }
    LOG_MSGID_W(common, "task_def_init, port = %d", 1, port);
    if (BT_POWER_ON_RELAY != bt_power_on_get_config_type() || port != bt_power_on_get_relay_port_number()) {
        LOG_MSGID_I(common, "atci will init", 0);

        atci_init(port);
    } else {
        LOG_MSGID_W(common, "bt_power_on_get_config_type() = %d, bt_power_on_get_relay_port_number() = %d", 2,
                    bt_power_on_get_config_type(), bt_power_on_get_relay_port_number());
    }
#else
    atci_init(CONFIG_ATCI_PORT);
#endif
#ifdef SLEEP_MANAGER_LOW_POWER_CONFIG
    extern void bt_atci_init(void);

    bt_atci_init();
#endif
#endif

    bt_sink_init();
    hal_audio_init();

    audio_dsp0_nvdm_init();     /* Write the default nvdm data to flash memory. */
}

void task_def_create(void)
{
    uint16_t            i;
    BaseType_t          x;
    const tasks_list_t  *t;

    for (i = 0; i < tasks_list_count; i++) {
        t = &tasks_list[i];
        if (BT_POWER_ON_RELAY == bt_power_on_get_config_type()) {
            if ((memcmp(t->pcName, BT_TASK_NAME, 5) == 0)) {
                continue;
            }
        } else {
            if (memcmp(t->pcName, CONTROLER_TEST_NAME, 5) == 0) {
                continue;
            }
        }

        LOG_I(common, "xCreate task %s, pri %d", t->pcName, (int)t->uxPriority);

        x = xTaskCreate(t->pvTaskCode,
                        t->pcName,
                        t->usStackDepth,
                        t->pvParameters,
                        t->uxPriority,
                        &task_handles[i]);
        if (t->pvTaskCode == am_task_main) {
            AUDIO_SRC_SRV_AM_TASK = task_handles[i];
        }

        if (x != pdPASS) {
            LOG_MSGID_E(common, ": failed", 0);
        } else {
            LOG_MSGID_I(common, ": succeeded", 0);
        }
    }
    LOG_MSGID_I(common, "Free Heap size:%d bytes", 1, xPortGetFreeHeapSize());
}

void task_def_dump_stack_water_mark(void)
{
    uint16_t i;

    for (i = 0; i < tasks_list_count; i++) {
        if (task_handles[i] != NULL) {
            LOG_I(common, "task name:%s, water_mark:%d",
                  tasks_list[i].pcName,
                  uxTaskGetStackHighWaterMark(task_handles[i]));
        }
    }
}

