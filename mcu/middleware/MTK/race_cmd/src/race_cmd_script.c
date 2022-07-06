/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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
#include "race_cmd.h"
#include "race_xport.h"
#include "race_event.h"
#include "race_cmd_script.h"
#include "race_event_internal.h"
#include "hal_audio_internal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mux.h"
#include "mux_pseudo.h"
#include "atci_main.h"
#include "bt_power_on_config.h"
#include "at_command_bt.h"
#include "bt_connection_manager.h"

#define RACE_SCRIPT_TASK_STACK   (375)


typedef struct {
    uint16_t length;
    uint8_t cmd[0];
} race_script_cmd_t;

typedef enum {
    RACE_SCRIPT_STATUS_NONE,
    RACE_SCRIPT_STATUS_STORE,
    RACE_SCRIPT_STATUS_WAIT_START,
    RACE_SCRIPT_STATUS_RUNNING,
    RACE_SCRIPT_STATUS_PENDING,
} race_script_status_t;

typedef struct {
    uint8_t *cmd_buff;
    uint32_t buff_len;
    uint32_t buff_used;
    uint16_t cmd_num;
    uint16_t index_running;
    race_script_cmd_t *next_cmd;
    race_script_status_t status;
} race_script_context_t;

static race_script_context_t race_script_context = {0};
QueueHandle_t g_race_script_queue = NULL;
TaskHandle_t g_race_script_task_handle = NULL;

extern bool bt_driver_enter_relay_mode(uint8_t port);
extern void bt_driver_relay_register_callbacks(void *callback);
extern atci_bt_relay_callbacks atci_bt_relay_cb;


static RACE_ERRCODE race_script_pseudo_port_init()
{
    RACE_ERRCODE ret_race = RACE_ERRCODE_FAIL;
    atci_status_t ret_at = ATCI_STATUS_ERROR;
    bool bt_ret = false;

    /* Init mux in race, and open race. */
    ret_race = race_pseudo_port_init();

    /* Open AT. */
    ret_at = atci_mux_port_reinit(MUX_PORT_PSEUDO, true, false);

    /* Open hci after BT disconnect all and standby. */
#if 0
    if (bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_ALL, NULL, 0) > 0) {
        bt_cm_connect_t param = {0};
        bt_bd_addr_t addr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        memcpy(param.address, addr, sizeof(bt_bd_addr_t));
        param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
        bt_cm_disconnect(&param);
        RACE_LOG_MSGID_I("race_script_pseudo_port_init, BT disconnecting ...", 0);
        vTaskDelay(3000);
    }
#endif

#if 1
    bt_cm_power_standby(false);
    if (bt_cm_power_get_state() > BT_CM_POWER_STATE_OFF) {
        RACE_LOG_MSGID_I("race_script_pseudo_port_init, BT standby waiting ...", 0);
        vTaskDelay(1000);
    }

    bt_driver_relay_register_callbacks((void *)&atci_bt_relay_cb);
    bt_ret = bt_driver_enter_relay_mode(MUX_PORT_PSEUDO);
    bt_power_on_set_config_type(BT_POWER_ON_RELAY);
#else
    bt_ret = true;
#endif

    RACE_LOG_MSGID_I("race_script_pseudo_port_init, status race %d at %d bt %d", 3, ret_race, ret_at, bt_ret);
    if (ret_race == RACE_ERRCODE_SUCCESS && ret_at == ATCI_STATUS_OK && bt_ret) {
        return RACE_ERRCODE_SUCCESS;
    } else {
        return RACE_ERRCODE_FAIL;
    }
}

static void race_script_store_cmd_buff_init()
{
    n9_dsp_share_info_t *share_buff = hal_audio_query_bt_audio_dl_share_info();
    if (share_buff && share_buff->start_addr) {
        race_script_context.cmd_buff = (uint8_t *)share_buff->start_addr;
        race_script_context.buff_len = share_buff->length;
        memset(race_script_context.cmd_buff, 0, race_script_context.buff_len);
    }
}

static RACE_ERRCODE race_script_store_cmd(uint8_t *cmd, uint16_t length)
{
    if (race_script_context.cmd_buff == NULL) {
        race_script_store_cmd_buff_init();
        race_script_context.status = RACE_SCRIPT_STATUS_STORE;
    }

    if (race_script_context.status != RACE_SCRIPT_STATUS_STORE) {
        RACE_LOG_MSGID_E("race_script_store_cmd, status error %d", 1, race_script_context.status);
        return RACE_ERRCODE_FAIL;
    }

    RACE_LOG_MSGID_I("race_script_store_cmd, total %d, used %d, cmd len %d, current cmd num %d", 4,
            race_script_context.buff_len, race_script_context.buff_used, length, race_script_context.cmd_num);

    if ((race_script_context.buff_len - race_script_context.buff_used) < (length + 2)) {
        return RACE_ERRCODE_NOT_ENOUGH_MEMORY;
    }

    race_script_cmd_t *p_store = (race_script_cmd_t *)(race_script_context.cmd_buff + race_script_context.buff_used);
    p_store->length = length;
    memcpy(p_store->cmd, cmd, length);

    race_script_context.buff_used += (length + 2);
    race_script_context.cmd_num ++;

    return RACE_ERRCODE_SUCCESS;
}

static RACE_ERRCODE race_script_start_test()
{
    if (race_script_context.status != RACE_SCRIPT_STATUS_WAIT_START) {
        RACE_LOG_MSGID_E("race_script_start_test, status error %d", 1, race_script_context.status);
        return RACE_ERRCODE_FAIL;
    }

    if (race_script_context.cmd_buff == NULL || race_script_context.buff_used == 0 || race_script_context.cmd_num == 0) {
        RACE_LOG_MSGID_E("race_script_start_test, no cmd stored", 0);
        return RACE_ERRCODE_FAIL;
    }

    if (race_script_pseudo_port_init() != RACE_ERRCODE_SUCCESS) {
        RACE_LOG_MSGID_E("race_script_start_test, port init fail", 0);
        return RACE_ERRCODE_FAIL;
    }

    if (xTaskCreate(race_script_task, "race_script", RACE_SCRIPT_TASK_STACK, NULL ,7, &g_race_script_task_handle) != pdPASS) {
        RACE_LOG_MSGID_E("race_script_start_test task create fail", 0);
        return RACE_ERRCODE_NOT_ENOUGH_MEMORY;
    }

    RACE_LOG_MSGID_I("race_script_start_test, starting", 0);
    race_script_context.index_running = 0;
    race_script_context.next_cmd = (race_script_cmd_t *)(race_script_context.cmd_buff);
    race_script_context.status = RACE_SCRIPT_STATUS_RUNNING;
    race_script_send_msg(RACE_SCRIPT_EVENT_SEND, NULL);
    return RACE_ERRCODE_SUCCESS;
}

void race_script_remote_disconnected()
{
    if (race_script_context.status == RACE_SCRIPT_STATUS_WAIT_START) {
        RACE_LOG_MSGID_I("race_script_remote_disconnected, start test", 0);
        race_script_start_test();
    }
}

void* RACE_CmdHandler_Script(ptr_race_pkt_t pCmdMsg, uint16_t Length, uint8_t channel_id)
{
    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    RSP *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE, pCmdMsg->hdr.id, sizeof(RSP), channel_id);
    if (pEvt == NULL) {
        RACE_LOG_MSGID_E("response alloc fail", 0);
        return NULL;
    }
    pEvt->status = 0xff;
    RACE_ERRCODE err = RACE_ERRCODE_FAIL;
    switch (pCmdMsg->hdr.id)
    {
        case RACE_SCRIPT_STORE_CMD:
            err = race_script_store_cmd(pCmdMsg->payload, pCmdMsg->hdr.length - 2);
            break;
        case RACE_SCRIPT_START_TEST:
            RACE_LOG_MSGID_I("race_script RACE_SCRIPT_START_TEST, status %d", 1, race_script_context.status);
            if (race_script_context.status == RACE_SCRIPT_STATUS_STORE) {
                race_script_context.status = RACE_SCRIPT_STATUS_WAIT_START;
                if (bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_ALL, NULL, 0) == 0) {
                    err = race_script_start_test();
                } else {
                    RACE_LOG_MSGID_I("race_script RACE_SCRIPT_START_TEST, wait disconnect", 0);
                    err = RACE_ERRCODE_SUCCESS;
                }
            }
            break;
        case RACE_SCRIPT_SLEEP: {
            uint32_t time_ms;
            memcpy(&time_ms, pCmdMsg->payload, sizeof(uint32_t));
            RACE_LOG_MSGID_I("race_script RACE_SCRIPT_SLEEP, status %d, time %d ms", 2, race_script_context.status, time_ms);
            if (race_script_context.status == RACE_SCRIPT_STATUS_RUNNING) {
                race_script_send_msg(RACE_SCRIPT_EVENT_SLEEP, (void *)time_ms);
                err = RACE_ERRCODE_SUCCESS;
            }
            break;
        }
        case RACE_SCRIPT_WAIT:
            RACE_LOG_MSGID_I("race_script RACE_SCRIPT_WAIT, status %d", 1, race_script_context.status);
            if (race_script_context.status == RACE_SCRIPT_STATUS_RUNNING) {
                race_script_context.status = RACE_SCRIPT_STATUS_PENDING;
                err = RACE_ERRCODE_SUCCESS;
            }
            break;
        default:
            RACE_LOG_MSGID_W("race_script unknown race cmd [0x%X]", 1, pCmdMsg->hdr.id);
            break;
    }

    if (err == RACE_ERRCODE_SUCCESS) {
        pEvt->status = 0x00;
    }
    return pEvt;
}

void race_script_key_notify()
{
    if (race_script_context.status == RACE_SCRIPT_STATUS_PENDING) {
        RACE_LOG_MSGID_I("race_script_key_notify", 0);
        race_script_context.status = RACE_SCRIPT_STATUS_RUNNING;
        race_script_send_msg(RACE_SCRIPT_EVENT_SEND, NULL);
    }
}

void race_script_pseudo_read_data(uint32_t port_index, pseudo_port_read_data_t *data)
{
    data->buffer = race_script_context.next_cmd->cmd;
    data->ret_size = (uint32_t)(race_script_context.next_cmd->length);
}

void race_script_pseudo_write_rsp(uint32_t port_index, uint32_t length, uint32_t data_ptr)
{
    race_script_rsp_data_t *data = NULL;

    if (length > 0 && data_ptr) {

        /* TODO: malloc data buffer and copy data in.
        data = pvMalloc(sizeof(race_script_rsp_data_t) + length);
        if (data) {
            data->len = length;
            data->buff = data + 1;
            memcpy(data->buff, data_ptr, length);
        */

        data = pvPortMalloc(sizeof(race_script_rsp_data_t) + 0);
        if(data) {
            data->len = length;
            data->buff = NULL;
        }

        race_script_send_msg(RACE_SCRIPT_EVENT_RSP, data);
    }
}


static void race_script_send_cmd()
{
    race_script_cmd_t *p_cmd = NULL;
    uint8_t *p_next = NULL;

    if (race_script_context.status != RACE_SCRIPT_STATUS_RUNNING) {
        RACE_LOG_MSGID_W("race_script_send_cmd, status error %d", 1, race_script_context.status);
        return;
    }

    if (race_script_context.index_running >= race_script_context.cmd_num) {
        RACE_LOG_MSGID_I("race_script_send_cmd, no more cmd", 0);
        race_script_send_msg(RACE_SCRIPT_EVENT_END, NULL);
        return;
    }

    p_cmd = race_script_context.next_cmd;

    mux_pseudo_callback(PSEUDO_EVENT_READY_TO_READ, 0);
    LOG_HEXDUMP_I(race, "script", p_cmd->cmd, p_cmd->length);
    race_script_context.index_running ++;
    RACE_LOG_MSGID_I("race_script_send_cmd, cur index %d", 1, race_script_context.index_running);
    p_next = (uint8_t *)(race_script_context.next_cmd) + p_cmd->length + 2;
    race_script_context.next_cmd = (race_script_cmd_t *)(p_next);

    //for test, should mark.
    //vTaskDelay(500);
    //race_script_send_msg(RACE_SCRIPT_EVENT_SEND, NULL);
}

static void race_script_handle_rsp(race_script_rsp_data_t *rsp)
{
    if(rsp) {
        RACE_LOG_MSGID_I("race_script_handle_rsp len %d", 1, rsp->len);
        /* TODO: check and print response. */
        vPortFree(rsp);
        race_script_send_msg(RACE_SCRIPT_EVENT_SEND, NULL);
    }
}

static void race_script_test_end()
{
    RACE_LOG_MSGID_I("race_script_test_end", 0);
    vQueueDelete(g_race_script_queue);
    memset(&race_script_context, 0, sizeof(race_script_context_t));
    vTaskDelete(NULL);
}


void race_script_task(void *arg)
{
    race_script_msg_t msg;
    RACE_LOG_MSGID_I("race_script_task, enter", 0);
    g_race_script_queue = xQueueCreate(10, sizeof(race_script_msg_t));

    if (g_race_script_queue == NULL) {
        RACE_LOG_MSGID_E("race_script_task: g_race_script_queue create fail", 0);
        return;
    }

    while (1) {
        /* Wait msg, blocking. */
        if (xQueueReceive((QueueHandle_t)g_race_script_queue, &msg, portMAX_DELAY) == pdFALSE) {
            continue;
        }
        RACE_LOG_MSGID_I("race_script_task, recv msg type %d", 1, msg.type);

        switch (msg.type) {
            case RACE_SCRIPT_EVENT_SEND:
                race_script_send_cmd();
                break;
            case RACE_SCRIPT_EVENT_RSP: {
                race_script_rsp_data_t *rsp = (race_script_rsp_data_t *)msg.data;
                race_script_handle_rsp(rsp);
                break;
            }
            case RACE_SCRIPT_EVENT_SLEEP:
                vTaskDelay((uint32_t)(msg.data));
                break;
            case RACE_SCRIPT_EVENT_END:
                race_script_test_end();
                break;
            default:
                RACE_LOG_MSGID_W("race_script_task, wrong type msg", 0);
                break;
        }

    }
}


void race_script_send_msg(race_script_event_t type, void *data)
{
    race_script_msg_t msg;

    msg.type = type;
    msg.data = data;

    if (xQueueSend((QueueHandle_t)g_race_script_queue, &msg, 0) == pdFALSE) {
        RACE_LOG_MSGID_E("race_script_send_msg, fail", 0);
    }
}
