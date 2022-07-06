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
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OifAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */
#if defined(MTK_MUX_AWS_MCE_ENABLE)

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "hal_platform.h"
#include "hal_nvic.h"

#include "mux.h"
#include "mux_port_device.h"
#include "assert.h"
#include "mux_port.h"
#include "bt_aws_mce.h"
#include "mux_aws_mce.h"

#define AWS_MCE_PORT_INDEX_TO_MUX_PORT(port_index) (port_index+MUX_AWS_MCE_BEGIN)

mux_irq_handler_t g_mux_aws_mce_callback;
virtual_read_write_point_t g_mux_aws_mce_r_w_point[MUX_AWS_MCE_END - MUX_AWS_MCE_BEGIN + 1];

log_create_module(mux_aws_mce, PRINT_LEVEL_INFO);

static void port_mux_aws_mce_set_rx_hw_wptr_internal_use(uint8_t port_index, uint32_t move_bytes);
static void port_mux_aws_mce_set_tx_hw_rptr_internal_use(uint8_t port_index, uint32_t move_bytes);


mux_aws_mce_user_table_t g_mux_aws_mce_user_table[] = {
    {MUX_AWS_MCE_USER_CM, "mux_aws_mce_cm"},
    {MUX_AWS_MCE_USER_SINK_MUSIC, "mux_aws_mce_sink_music"},
    { MUX_AWS_MCE_USER_SINK_CALL, "mux_aws_mce_call"},     /**< The module type of sink call. */
    { MUX_AWS_MCE_USER_BT_AIR, "mux_aws_mce_ble_air"},       /**< The module type of BT air. */
    { MUX_AWS_MCE_USER_FOTA, "mux_aws_mce_fota"},          /**< The module type of FOTA. */
    { MUX_AWS_MCE_USER_TEST, "mux_aws_mce_test"},            /**< The module type for test. */
    { MUX_AWS_MCE_USER_RELAY_CMD, "mux_aws_mce_relay_cmd"},       /**< The module type of race relay cmd. */
    { MUX_AWS_MCE_USER_HFP_AVC, "mux_aws_mce_hfp_avc"},    /**< The module type of hfp avc. */
    { MUX_AWS_MCE_USER_GSOUND, "mux_aws_mce_gsound"},    /**< The module type of gsound. */
    { MUX_AWS_MCE_USER_DM, "mux_aws_mce_dm"},    /**< The module type of bt device manager. */
    { MUX_AWS_MCE_USER_VP, "mux_aws_mce_vp"},                                      /**< The module type of voice prompts. */
    { MUX_AWS_MCE_USER_LED, "mux_aws_mce_led"},                            /**< The module type of LED. */
    { MUX_AWS_MCE_USER_PEQ, "mux_aws_mce_peq"},                           /**< The module type of PEQ. */
    { MUX_AWS_MCE_USER_BATTERY , "mux_aws_mce_battery"},                      /**< The module type of battery. */
    { MUX_AWS_MCE_USER_APP_ACTION, "mux_aws_mce_app_action"},                    /**< The module type of app action. */
    { MUX_AWS_MCE_USER_ANC , "mux_aws_mce_anc"},                          /**< The module type of anc. */
    { MUX_AWS_MCE_USER_BLE_APP, "mux_aws_mce_ble_app"},                           /**< The module type of ble app. */
    { MUX_AWS_MCE_USER_FAST_PAIR, "mux_aws_mce_fast_pair"},                            /**< The module type of fast pair. */
    { MUX_AWS_MCE_USER_SMCHARGER, "mux_aws_mce_smartcharge"},                          /**< The module type of smart charger. */
    { MUX_AWS_MCE_USER_IN_EAR, "mux_aws_mce_in_ear"},
    { MUX_AWS_MCE_USER_AP, "mux_aws_mce_ap"}
};

static bool mux_aws_mce_check_user_id(mux_aws_mce_user_id_t aws_user_id)
{
    bool is_mux_aws = false;
    uint8_t i;
    for (i = 0; i < (sizeof(g_mux_aws_mce_user_table) / sizeof(mux_aws_mce_user_table_t)); i++) {
        if (g_mux_aws_mce_user_table[i].aws_mce_user_id == aws_user_id) {
            is_mux_aws = true;
            break;
        }
    }
    return is_mux_aws;
}

void mux_aws_mce_send_data(uint8_t port_index, uint32_t addr, uint32_t len, volatile uint32_t *sending_point)
{
    bool urgent = false;
    uint8_t port;
    uint32_t send_len;
    port = port_index + MUX_AWS_MCE_BEGIN;

    if (len == 0 || (port < MUX_AWS_MCE_BEGIN) || (port > MUX_AWS_MCE_END)) {
        return;
    }

    mux_driver_debug_for_check(&g_mux_aws_mce_r_w_point[port_index]);
    /*Must be  set the value of tx_sending_read_point firstly!!!!!must before call if send!!!!!*/
    *sending_point = addr + len;
    urgent = (port == MUX_AWS_MCE_URGENT) ?  true : false;
    send_len = bt_aws_mce_report_mux_send((uint8_t *)addr, len, urgent);
    port_mux_aws_mce_set_tx_hw_rptr_internal_use(port_index, send_len);

    LOG_MSGID_I(mux_aws_mce, "mux_aws_mce_send_data,send_len:%d, urgent:%d, addr:0x%0x", 3, send_len, urgent, addr);

}

static void mux_aws_mce_callback(bt_msg_type_t event, void *parameter, uint32_t len)
{
    uint8_t port_index = 0;
    bool is_mux_aws;
    virtual_read_write_point_t *p;
    uint32_t next_available_block_len, next_free_block_len, per_cpu_irq_mask, read_len;
    mux_aws_mce_header_t *header = (mux_aws_mce_header_t *)(parameter);

    p = &g_mux_aws_mce_r_w_point[port_index];
    LOG_MSGID_I(mux_aws_mce, "mux_aws_mce_callback, event:0x%0x, para:0x%0x, len:%d", 3, event, parameter, len);


    if (mux_aws_mce_check_user_id(header->user_id) == false) {
        LOG_MSGID_I(mux_aws_mce, "is not user of mux_aws_mce, user_id:0x%0x", 1, header->user_id);
        return;
    }
    mux_driver_debug_for_check(&g_mux_aws_mce_r_w_point[port_index]);

    switch (event) {
    case BT_AWS_MCE_INFOMATION_PACKET_IND: {
        next_free_block_len = mux_common_device_get_buf_next_free_block_len(p->rx_buff_start, p->rx_buff_read_point, p->rx_buff_write_point, p->rx_buff_end, p->rx_buff_available_len);
        LOG_MSGID_I(mux_aws_mce, "mux_aws_mce_callback, next_free_block_len:%d data_len:%d\r\n", 2, next_free_block_len, len);
        if (next_free_block_len >= len) {
            memcpy((void *)(p->rx_buff_write_point), (uint8_t *)parameter, len);
        } else {
            if (p->rx_buff_len - p->rx_buff_read_point < len - next_free_block_len) {
                return;
            }

            memcpy((void *)(p->rx_buff_write_point), (uint8_t *)parameter, next_free_block_len);
            memcpy((void *)(p->rx_buff_start), (uint8_t *)parameter + next_free_block_len, len - next_free_block_len);
        }
        port_mux_aws_mce_set_rx_hw_wptr_internal_use(port_index, len);
        g_mux_aws_mce_callback(AWS_MCE_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_READY_TO_READ, parameter);
        break;
    }
    default:
        break;
    }
}

void mux_aws_mce_package(mux_handle_t handle, mux_buffer_t buffers[], uint32_t buffers_counter, mux_buffer_t *head, mux_buffer_t *tail, void *user_data)
{
    uint32_t i, user_id,  total_size;
    mux_aws_mce_header_t *header = (mux_aws_mce_header_t *)(head->p_buf);
    const char *user_name  = NULL;

    total_size = 0;
    for (i = 0; i < buffers_counter; i++) {
        total_size += buffers[i].buf_size;
    }

    mux_query_user_name(handle, &user_name);
    LOG_MSGID_I(mux_aws_mce, "mux_aws_mce_package, handle:0x%0x, user_name:0x%0x", 2, handle, user_name);

    for (i = 0; i < (sizeof(g_mux_aws_mce_user_table) / sizeof(mux_aws_mce_user_table_t )); i++ ) {
        if (strcmp(user_name, g_mux_aws_mce_user_table[i].aws_mce_user_name) == 0) {
            user_id = g_mux_aws_mce_user_table[i].aws_mce_user_id;
            break;
        }
    }

    header->user_id = user_id;
    header->len = total_size;
    head->buf_size = sizeof(mux_aws_mce_header_t);

    tail->p_buf = NULL;
    tail->buf_size = 0;
    LOG_MSGID_I(mux_aws_mce, "mux_aws_mce_package, len:%d, user_id:0x%0x, handle:0x%0x", 3, total_size, user_id, handle);

}

void mux_aws_mce_urgent_package(mux_handle_t handle, mux_buffer_t buffers[], uint32_t buffers_counter, mux_buffer_t *head, mux_buffer_t *tail, void *user_data)
{
    uint32_t i, total_size, user_id, delay_time;
    mux_aws_mce_urgent_header_t *header = (mux_aws_mce_urgent_header_t *)(head->p_buf);
    const char *user_name = NULL;

    total_size = 0;
    for (i = 0; i < buffers_counter; i++) {
        total_size += buffers[i].buf_size;
    }
    mux_query_user_name(handle, &user_name);
    for (i = 0; i < (sizeof(g_mux_aws_mce_user_table) / sizeof(mux_aws_mce_user_table_t )); i++ ) {
        if (strcmp(user_name, g_mux_aws_mce_user_table[i].aws_mce_user_name) == 0) {
            user_id = g_mux_aws_mce_user_table[i].aws_mce_user_id;
            break;
        }
    }

    header->user_id = user_id;
    header->len = total_size;
    delay_time = *(uint32_t *)(buffers[0].p_buf);
    head->buf_size = sizeof(mux_aws_mce_urgent_header_t);
    LOG_MSGID_I(mux_aws_mce, "mux_aws_mce_urgent_package, buf_size:%d, total_size:%d, delay_time:%d, handle:0x%0x", 4, head->buf_size, total_size, delay_time, handle);

    bt_sink_srv_bt_clock_addition(&(header->clk), 0, delay_time * 1000);

    tail->p_buf = NULL;
    tail->buf_size = 0;


}



void mux_aws_mce_unpackage(mux_handle_t *handle, mux_buffer_t buffers[], uint32_t buffers_counter, uint32_t *consume_len, uint32_t *package_len, void *user_data)
{
    uint32_t i, user_id, data_len;
    mux_aws_mce_header_t header;
    const char user_name[50];
    uint32_t total_size = 0;

    *package_len = 0;
    *consume_len = 0;


    for (i = 0; i < buffers_counter; i++) {
        total_size += buffers[i].buf_size;
    }
    LOG_MSGID_I(mux_aws_mce, "mux_aws_mce_unpackage, total_size:%d", 1, total_size);

    if (total_size < sizeof(mux_aws_mce_header_t)) {
        return;
    }

    // The header structure may cross the ring buffer's boundary.
    if (buffers[0].buf_size >= sizeof(mux_aws_mce_header_t)) {
        memcpy(&header, buffers[0].p_buf, sizeof(mux_aws_mce_header_t));
    } else {
        memcpy(&header, buffers[0].p_buf, buffers[0].buf_size);
        memcpy(((uint8_t *)&header) + buffers[0].buf_size, buffers[1].p_buf, sizeof(mux_aws_mce_header_t) - buffers[0].buf_size);
    }

    if (total_size < sizeof(mux_aws_mce_header_t) + header.len) {
        return;
    }

    user_id = header.user_id;
    data_len = header.len;

    *consume_len = sizeof(mux_aws_mce_header_t);
    *package_len = data_len;
    for (i = 0; i < (sizeof(g_mux_aws_mce_user_table) / sizeof(mux_aws_mce_user_table_t )); i++ ) {
        if (g_mux_aws_mce_user_table[i].aws_mce_user_id == user_id) {
            strcpy(user_name, g_mux_aws_mce_user_table[i].aws_mce_user_name);
            mux_query_user_handle(MUX_AWS_MCE, user_name, handle);
            LOG_MSGID_I(mux_aws_mce, "mux_aws_mce_unpackage, user_name:%s, handle:0x%0x", 2, user_name, *handle);
            break;
        }
    }

    LOG_MSGID_I(mux_aws_mce, "mux_aws_mce_unpackage, consume_len:%d, package_len:%d, handle:0x%0x", 3, *consume_len, *package_len, *handle);

    return;
}

void mux_aws_mce_urgent_unpackage(mux_handle_t *handle, mux_buffer_t buffers[], uint32_t buffers_counter, uint32_t *consume_len, uint32_t *package_len, void *user_data)
{

    uint32_t i, total_size, user_id, data_len, delay_us, delay_ms;
    uint32_t *sync_time;
    mux_aws_mce_urgent_header_t header;
    const char user_name[50];
    bool para_wrap = false;
    uint8_t wrap_num = 0;

    *package_len = 0;
    *consume_len = 0;


    for (i = 0; i < buffers_counter; i++) {
        total_size += buffers[i].buf_size;
    }
    LOG_MSGID_I(mux_aws_mce, "mux_aws_mce_urgent_unpackage, total_size:%d", 1, total_size);

    if (total_size < sizeof(mux_aws_mce_urgent_header_t)) {
        return;
    }

    // The header structure may cross the ring buffer's boundary.
    if (buffers[0].buf_size >= (sizeof(mux_aws_mce_urgent_header_t) + sizeof(uint32_t))) {
        memcpy(&header, buffers[0].p_buf, sizeof(mux_aws_mce_urgent_header_t));
        sync_time = buffers[0].p_buf + sizeof(mux_aws_mce_urgent_header_t);
    } else if (buffers[0].buf_size > sizeof(mux_aws_mce_urgent_header_t)) {
        memcpy(&header, buffers[0].p_buf, sizeof(mux_aws_mce_urgent_header_t));
        para_wrap = true;
        wrap_num = buffers[0].buf_size - sizeof(mux_aws_mce_urgent_header_t);
    } else {
        memcpy(&header, buffers[0].p_buf, buffers[0].buf_size);
        memcpy(((uint8_t *)&header) + buffers[0].buf_size, buffers[1].p_buf, sizeof(mux_aws_mce_urgent_header_t) - buffers[0].buf_size);
        sync_time = buffers[1].p_buf + sizeof(mux_aws_mce_urgent_header_t) - buffers[0].buf_size;
    }

    if (total_size < sizeof(mux_aws_mce_urgent_header_t) + header.len) {
        return;
    }
    user_id = header.user_id;
    data_len = header.len;
    bt_sink_srv_bt_clock_get_duration(&(header.clk), 0, &delay_us);
    delay_ms = (uint32_t)(delay_us / 1000);
    if (!para_wrap) {
        *sync_time = delay_ms;
    } else {
        memcpy(buffers[0].p_buf + sizeof(mux_aws_mce_urgent_header_t), (uint8_t *)(&delay_ms), wrap_num);
        delay_ms = delay_ms << (wrap_num * 8);
        memcpy(buffers[1].p_buf, (uint8_t *)(&delay_ms), sizeof(uint32_t) - wrap_num);
    }
    *consume_len =  sizeof(mux_aws_mce_urgent_header_t);
    *package_len = data_len;
    for (i = 0; i < (sizeof(g_mux_aws_mce_user_table) / sizeof(mux_aws_mce_user_table_t )); i++ ) {
        if (g_mux_aws_mce_user_table[i].aws_mce_user_id == user_id) {
            strcpy(user_name, g_mux_aws_mce_user_table[i].aws_mce_user_name);
            mux_query_user_handle(MUX_AWS_MCE, user_name, handle);
            LOG_MSGID_I(mux_aws_mce, "mux_aws_mce_urgent_unpackage, handle:0x%0x", 1,  *handle);
            break;
        }
    }
    LOG_MSGID_I(mux_aws_mce, "mux_aws_mce_urgent_unpackage, sync_time:%d,consume_len:%d, package_len:%d, handle:0x%0x", 4, sync_time, consume_len, package_len, handle);

    return;
}


mux_status_t port_mux_aws_mce_init()
{
    uint8_t port_index = 0;
    mux_status_t status;
    mux_port_setting_t setting, u_setting;
    mux_protocol_t mux_aws_mce_pro_callback, mux_aws_mce_urgent_pro_callback;

    bt_aws_mce_report_init();
    bt_aws_mce_report_register_mux_callback(mux_aws_mce_callback);

    setting.tx_buffer_size = 512;
    setting.rx_buffer_size = 512;
    mux_aws_mce_pro_callback.tx_protocol_callback = mux_aws_mce_package;
    mux_aws_mce_pro_callback.rx_protocol_callback = mux_aws_mce_unpackage;
    status = mux_init(MUX_AWS_MCE, &setting, &mux_aws_mce_pro_callback);
    LOG_MSGID_I(mux_aws_mce, "port_mux_aws_mce_init, status:%d", 1, status);
#if 0
    u_setting.tx_buffer_size = 512;
    u_setting.rx_buffer_size = 512;
    mux_aws_mce_urgent_pro_callback.tx_protocol_callback = mux_aws_mce_urgent_package;
    mux_aws_mce_urgent_pro_callback.rx_protocol_callback = mux_aws_mce_urgent_unpackage;
    status = mux_init(MUX_AWS_MCE_URGENT, &u_setting, &mux_aws_mce_urgent_pro_callback);
    //LOG_MSGID_I(mux_aws_mce, "port_mux_aws_mce_init, status:%d", 1, status);
#endif
    return status;

}

mux_status_t port_mux_aws_mce_normal_init(uint8_t port_index, mux_port_config_t *p_setting, mux_irq_handler_t irq_handler)
{
    uint8_t port;
    serial_port_status_t serial_port_status;
    serial_port_open_para_t serial_port_if_config;
    port = port_index + MUX_AWS_MCE_BEGIN;
    if ((port < MUX_AWS_MCE_BEGIN) || (port  > MUX_AWS_MCE_END) ) {
        return MUX_STATUS_ERROR_PARAMETER;
    } else {
        g_mux_aws_mce_callback = irq_handler;
        mux_common_device_r_w_point_init(&g_mux_aws_mce_r_w_point[port_index], p_setting);
    }

    return MUX_STATUS_OK;
}

mux_status_t port_mux_aws_mce_deinit(uint8_t port_index)
{
    serial_port_status_t status;

    mux_driver_debug_for_check(&g_mux_aws_mce_r_w_point[port_index]);

    //status = serial_port_close(serial_port_if_handle[port_index]);
    status = SERIAL_PORT_STATUS_OK;
    if (status != SERIAL_PORT_STATUS_OK) {
        return MUX_STATUS_ERROR_DEINIT_FAIL;
    }
    return MUX_STATUS_OK;
}

void port_mux_aws_mce_exception_init(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
}

void port_mux_aws_mce_exception_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    //TODO: need if replace
    //maybe exception if disconnect ???
    //if_mux_dump_data(port_index, buffer, size);
}

bool port_mux_aws_mce_buf_is_full(uint8_t port_index, bool is_rx)
{
    return mux_common_device_buf_is_full(&g_mux_aws_mce_r_w_point[port_index], is_rx);
}

uint32_t port_mux_aws_mce_get_hw_rptr(uint8_t port_index, bool is_rx)
{
    return mux_common_device_get_hw_rptr(&g_mux_aws_mce_r_w_point[port_index], is_rx);
}

uint32_t port_mux_aws_mce_get_hw_wptr(uint8_t port_index, bool is_rx)
{
    return mux_common_device_get_hw_wptr(&g_mux_aws_mce_r_w_point[port_index], is_rx);
}

void port_mux_aws_mce_set_rx_hw_rptr(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_rx_hw_rptr(&g_mux_aws_mce_r_w_point[port_index], move_bytes);
}

static void port_mux_aws_mce_set_rx_hw_wptr_internal_use(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_rx_hw_wptr_internal_use(&g_mux_aws_mce_r_w_point[port_index], move_bytes);
}

static void port_mux_aws_mce_set_tx_hw_rptr_internal_use(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_tx_hw_rptr_internal_use(&g_mux_aws_mce_r_w_point[port_index], move_bytes);
}

void port_mux_aws_mce_set_tx_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    virtual_read_write_point_t *p = &g_mux_aws_mce_r_w_point[port_index];
    uint32_t per_cpu_irq_mask;
    uint32_t send_addr, send_len;
    mux_driver_debug_for_check(p);

    //port_mux_local_cpu_enter_critical(&per_cpu_irq_mask);

    mux_common_device_set_tx_hw_wptr(p, move_bytes);
    LOG_MSGID_I(mux_aws_mce, "aws_mce_set_tx_hw_wptr, tx_start: 0x%0x, write_point: 0x%0x, move_bytes:%d, tx_end:0x%0x,available_len:%d", 5,
                p->tx_buff_start, p->tx_buff_write_point, move_bytes, p->tx_buff_end, p->tx_buff_available_len);

    send_len = mux_common_device_get_buf_next_available_block_len(p->tx_buff_start, p->tx_buff_read_point, p->tx_buff_write_point, p->tx_buff_end, p->tx_buff_available_len);
    send_addr = p->tx_buff_read_point;
    LOG_MSGID_I(mux_aws_mce, "aws_mce_set_tx_hw_wptr, send_addr: 0x%0x, send_len:%d", 2, send_addr, send_len);

    if ((p->tx_send_is_running == MUX_DEVICE_HW_RUNNING) || (send_len == 0) ) { //HW is running, no need to do send now, when hw transfer done, will restart on the IRQ handle
        //port_mux_local_cpu_exit_critical(per_cpu_irq_mask);
        return;
    } else {
        p->tx_send_is_running = MUX_DEVICE_HW_RUNNING;
        p->tx_sending_read_point = p->rx_buff_read_point + send_len;
    }

    //port_mux_local_cpu_exit_critical(per_cpu_irq_mask);

    //TODO: need if replace

    if (send_len < move_bytes) {
        uint8_t *pFullPacket;
        uint32_t send_addr_2, send_len_2;

        pFullPacket = (uint8_t *)port_mux_malloc(move_bytes);
        memcpy(pFullPacket, send_addr, send_len);
        send_len_2 = move_bytes - send_len;
        send_addr_2 = p->tx_buff_start;
        LOG_MSGID_I(mux_aws_mce, "aws_mce_set_tx_hw_wptr, send_addr_2: 0x%0x, send_len_2:%d", 2, send_addr_2, send_len_2);
        memcpy(pFullPacket + send_len, (void *)send_addr_2, move_bytes - send_len);
        mux_aws_mce_send_data(port_index, (uint32_t)pFullPacket, move_bytes, &p->tx_sending_read_point);
        p->tx_sending_read_point = send_addr_2 + send_len_2;
        port_mux_free(pFullPacket);
    } else {
        mux_aws_mce_send_data(port_index, send_addr, send_len, &p->tx_sending_read_point);
    }
    p->tx_send_is_running = MUX_DEVICE_HW_IDLE;
    if (p->tx_buff_available_len == 0) {
        LOG_MSGID_I(mux_aws_mce, "aws_mce_set_tx_hw_wptr, send out OK and empty", 0);
    } else {
        LOG_MSGID_I(mux_aws_mce, "aws_mce_set_tx_hw_wptr, send out OK and non-empty tx_buff_available_len=%d", 1, p->tx_buff_available_len);
    }
}

void port_mux_aws_phase1_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
    return;
}

void port_mux_aws_phase2_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
    return;
}

mux_status_t port_mux_aws_control(uint8_t port_index, mux_ctrl_cmd_t command, mux_ctrl_para_t *para)
{
    virtual_read_write_point_t *p = &g_mux_aws_mce_r_w_point[port_index];
    switch (command) {
    case MUX_CMD_GET_VIRTUAL_TX_AVAIL_LEN: {
        *(uint32_t *)para = p->tx_buff_available_len;
        return MUX_STATUS_OK;
    } break;

    case MUX_CMD_CLEAN_TX_VIRUTUAL: {
        port_mux_aws_mce_set_tx_hw_rptr_internal_use(port_index, p->tx_buff_available_len);
        return MUX_STATUS_OK;
    } break;

    default: break;
    }
    return MUX_STATUS_ERROR;
}

port_mux_device_ops_t g_port_mux_aws_mce_ops = {
#ifdef MTK_CPU_NUMBER_0
    port_mux_aws_mce_normal_init,
    port_mux_aws_mce_deinit,
    port_mux_aws_mce_exception_init,
    port_mux_aws_mce_exception_send,
    port_mux_aws_mce_buf_is_full,
#endif
//    port_mux_aws_mce_hook,
    port_mux_aws_mce_get_hw_rptr,
    port_mux_aws_mce_set_rx_hw_rptr,
    port_mux_aws_mce_get_hw_wptr,
    port_mux_aws_mce_set_tx_hw_wptr,
    port_mux_aws_phase1_send,
    port_mux_aws_phase2_send,
    port_mux_aws_control,
    NULL,
    NULL,
};

#endif
