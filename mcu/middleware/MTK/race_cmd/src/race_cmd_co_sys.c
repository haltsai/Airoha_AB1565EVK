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
#include "race_cmd.h"
#include "race_xport.h"
#include "race_util.h"
#include "race_cmd_co_sys.h"
#include "mux.h"
#include "semphr.h"
#include "timers.h"

/***************************************************************/
/*                   Defines                                   */
/***************************************************************/
#define RACE_COSYS_ID_DATA    0x110F

typedef struct {
        uint8_t is_critical;
        uint8_t module_id;
        uint8_t data[0];
} PACKED race_cosys_payload_t;

race_cosys_data_callback_t g_race_cosys_callback_table[RACE_COSYS_MODULE_ID_NUM] = {0};
TimerHandle_t race_cosys_tx_timer = NULL;
static bool race_cosys_tx_timer_alive = false;

extern uint32_t race_port_send_data_imp(uint32_t port_handle, uint8_t* buf, uint32_t buf_len);
extern void race_start_sleep_lock_timer();

bool race_cosys_register_data_callback(race_cosys_module_id_t module_id, race_cosys_data_callback_t callback)
{
    if (g_race_cosys_callback_table[module_id] == NULL) {
        RACE_LOG_MSGID_I("race_cosys callback register module %d, callback[0x%X]", 2, module_id, callback);
        g_race_cosys_callback_table[module_id] = callback;
        return true;
    }

    return false;
}

bool race_cosys_send_data(race_cosys_module_id_t module_id, bool is_critical, uint8_t *buff, uint32_t len)
{
    race_status_t ret = RACE_STATUS_ERROR;
    race_send_pkt_t* pData = NULL;
    uint32_t port_handle= 0;
    uint8_t channel_id;

    RACE_LOG_MSGID_I("race_cosys_send_data module %d, is_critical %d, len %d", 3, module_id, is_critical, len);

    if (buff == NULL || len == 0) {
        RACE_LOG_MSGID_W("race_cosys_send_data empty", 0);
        return false;
    }

    if (HAL_NVIC_QUERY_EXCEPTION_NUMBER != 0 && is_critical) {
        RACE_LOG_MSGID_E("race_cosys_send_data, cannot send critical data in IRQ", 0);
        return false;
    }

    race_cosys_payload_t *payload = (race_cosys_payload_t *)RACE_ClaimPacket(RACE_TYPE_COMMAND_WITHOUT_RSP, RACE_COSYS_ID_DATA,
            sizeof(race_cosys_payload_t) + len, race_get_channel_id_by_port_type(RACE_SERIAL_PORT_TYPE_COSYS));

    if (payload == NULL) {
        RACE_LOG_MSGID_E("race_cosys_send_data claim data fail", 0);
        return false;
    }

    payload->is_critical = (uint8_t)is_critical;
    payload->module_id = (uint8_t)module_id;
    memcpy(payload->data, buff, len);
    channel_id = race_get_channel_id_by_port_type(RACE_SERIAL_PORT_TYPE_COSYS);

    if (!is_critical) {
        ret = race_flush_packet((void *)payload, channel_id);
    } else {
        pData = race_pointer_cnv_pkt_to_send_pkt((void *)payload);
        port_handle = race_get_port_handle_by_channel_id(channel_id);
        if (race_port_send_data_imp(port_handle, (uint8_t*)&pData->race_data, pData->length) == pData->length) {
            ret = RACE_STATUS_OK;
        }
        race_mem_free(pData);
    }

    if (ret == RACE_STATUS_OK) {
        return true;
    } else {
        return false;
    }
}

static void race_cosys_dispatch_data(race_cosys_payload_t *payload, uint32_t len)
{
    race_cosys_module_id_t module_id;

    if (payload == NULL || len == 0) {
        RACE_LOG_MSGID_W("race_cosys_dispatch_data empty data", 0);
        return;
    }

    module_id = payload->module_id;
    RACE_LOG_MSGID_I("race_cosys_dispatch_data module %d, is_critical %d, len %d", 3, module_id, payload->is_critical, len);

#if (RACE_DEBUG_PRINT_ENABLE)
    race_dump(payload->data, RACE_DBG_EVT);
#endif

    if (g_race_cosys_callback_table[module_id]) {
        g_race_cosys_callback_table[module_id](payload->is_critical, payload->data, len);
    } else {
        RACE_LOG_MSGID_I("race_cosys_dispatch_data, callback not found", 0);
    }
}

void* RACE_CmdHandler_co_sys(ptr_race_pkt_t pCmdMsg, uint16_t Length, uint8_t channel_id)
{
    switch (pCmdMsg->hdr.id)
    {
        case RACE_COSYS_ID_DATA: {
            RACE_LOG_MSGID_I("race_cosys data from channel %d, length %d", 2, channel_id, Length);
            race_cosys_payload_t *payload = (race_cosys_payload_t *)pCmdMsg->payload;
            race_cosys_dispatch_data(payload, pCmdMsg->hdr.length - 4);
            break;
        }
        default:
            RACE_LOG_MSGID_E("race_cosys unknown ID[0x%X]", 1, pCmdMsg->hdr.id);
            break;
    }

    return NULL;
}


void race_cosys_rx_irq(mux_handle_t handle, uint32_t data_len)
{
    mux_status_t ret = MUX_STATUS_ERROR;
    uint32_t recv_len = 0;
    mux_buffer_t mux_buffer;
    ptr_race_pkt_t race_ptr = NULL;
    uint8_t *buff = race_mem_alloc(data_len);

    if (buff == NULL) {
        RACE_LOG_MSGID_E("race_cosys rx irq, buff alloc fail", 0);
        return;
    }

    race_start_sleep_lock_timer();

    mux_buffer.p_buf = buff;
    mux_buffer.buf_size = data_len;
    ret = mux_rx(handle, &mux_buffer, &recv_len);
    if (MUX_STATUS_OK != ret) {
        RACE_LOG_MSGID_E("race_cosys rx irq, rx fail:%d", 1, ret);
        return;
    }

    RACE_LOG_MSGID_I("race_cosys rx irq, data_len:%d, recv_len %d", 2, data_len, recv_len);
    race_ptr = (ptr_race_pkt_t)buff;
    RACE_CmdHandler_co_sys(race_ptr, recv_len, 0xFF);
    race_mem_free(buff);
}

static void race_cosys_tx_timer_callback(TimerHandle_t xtimer)
{
    race_cosys_tx_timer_alive = false;
    RACE_LOG_MSGID_I("race_cosys_tx_timer_callback", 0);
}

void race_cosys_tx_timer_init()
{
    if(race_cosys_tx_timer == NULL)
    {
        race_cosys_tx_timer = xTimerCreate("race_cosys_timer", (9 * 1000 / portTICK_PERIOD_MS), pdFALSE, NULL, race_cosys_tx_timer_callback);
        if(race_cosys_tx_timer == NULL)
        {
            RACE_LOG_MSGID_I("race_cosys_tx_timer create fail", 0);
            return;
        }
        RACE_LOG_MSGID_I("race_cosys_tx_timer init done", 0);
    }
}

void race_cosys_tx_timer_start(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t ret;

    if(!race_cosys_tx_timer_alive) {
        race_cosys_tx_timer_alive = true;
        if (HAL_NVIC_QUERY_EXCEPTION_NUMBER != 0) {
            ret = xTimerStartFromISR(race_cosys_tx_timer, &xHigherPriorityTaskWoken);
        } else {
            ret = xTimerStart(race_cosys_tx_timer, 0);
        }
        RACE_LOG_MSGID_I("race_cosys_tx_timer_start, start ret %d", 1, ret);
    } else {
        if (HAL_NVIC_QUERY_EXCEPTION_NUMBER != 0) {
            ret = xTimerResetFromISR(race_cosys_tx_timer, &xHigherPriorityTaskWoken);
        } else {
            ret = xTimerReset(race_cosys_tx_timer, 0);
        }
        RACE_LOG_MSGID_I("race_cosys_tx_timer_start, restart ret %d", 1, ret);
    }

    if(xHigherPriorityTaskWoken == pdTRUE){
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

}

bool race_cosys_tx_timer_is_alive()
{
    return race_cosys_tx_timer_alive;
}
