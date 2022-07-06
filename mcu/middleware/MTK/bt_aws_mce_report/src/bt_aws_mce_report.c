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

#if defined(MTK_AWS_MCE_ENABLE)
#include "bt_callback_manager.h"
#include "bt_debug.h"
#include "FreeRTOS.h"
#include "bt_connection_manager.h"
#include "bt_aws_mce_report_internal.h"
#include "bt_sink_srv_common.h"
#include "bt_aws_mce_srv.h"
log_create_module(bt_aws_mce_report, PRINT_LEVEL_INFO);
#ifdef MTK_BT_SPEAKER_ENABLE
//#define BT_AWS_MCE_REPORT_SPEAKER_ENABLE_INTERNAL
#endif

//#define BT_AWS_MCE_REPORT_TEST
#ifdef MTK_BT_SPEAKER_ENABLE
//#define BT_AWS_MCE_REPORT_SPEAKER_ENABLE_INTERNAL
#endif

static bt_aws_mce_report_callback_table_t bt_aws_mce_report_callback_table[BT_AWS_MCE_REPORT_MODULE_MAX] = {{0}};
#ifdef MTK_MUX_AWS_MCE_ENABLE
static bt_aws_mce_report_mux_callback_t  bt_aws_mce_report_callback;
#endif

static bt_aws_mce_report_context_t bt_aws_mce_report_cntx = {0};
static bool bt_aws_mce_report_init_flag = false;

#ifdef BT_AWS_MCE_REPORT_TEST
#include "hal_gpio.h"
#define BT_AWS_MCE_REPORT_MODULE_TEST         (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 13) 

static hal_gpio_data_t gpio_data = 0;
void bt_aws_mce_report_test_by_gpio_init(hal_gpio_pin_t port_num);

#endif

static bt_status_t bt_aws_mce_report_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff);


static void *bt_aws_mce_report_memset(void *p, int32_t value, uint32_t size)
{
    if (p == NULL) {
        return NULL;
    } else {
        return memset(p, value, size);
    }
}

static void *bt_aws_mce_report_alloc(uint16_t size)
{
    void *p = (void *)pvPortMalloc(size);
    bt_aws_mce_report_memset(p, 0, size);
    return ((p == NULL) ? NULL : p);
}

static void bt_aws_mce_report_free(void *p)
{
    if (p) {
        vPortFree(p);
        p = NULL;
    }
}

void *bt_aws_mce_report_memcpy(void *dest, const void *src, uint32_t size)
{
    if (src == NULL || dest == NULL) {
        return NULL;
    } else {
        return memcpy(dest, src, size);
    }
}

int32_t bt_aws_mce_report_memcmp(const void *dest, const void *src, uint32_t count)
{
    if (src == NULL || dest == NULL) {
        return -1;
    } else {
        return memcmp(dest, src, count);
    }
}



void bt_aws_mce_report_init(void)
{
#ifdef BT_AWS_MCE_REPORT_TEST
    bt_aws_mce_report_test_by_gpio_init(HAL_GPIO_13);
    gpio_data = HAL_GPIO_DATA_LOW;
#endif
    //memset(&bt_aws_mce_report_cntx, 0, sizeof(bt_aws_mce_report_context_t));
    if(bt_aws_mce_report_init_flag == false) {
        LOG_MSGID_I(bt_aws_mce_report, "bt_aws_mce_report_init", 0);
        bt_callback_manager_register_callback(bt_callback_type_app_event,
                                          (uint32_t)(MODULE_MASK_AWS_MCE),
                                          (void *)bt_aws_mce_report_event_callback);
        bt_aws_mce_report_init_flag  = true;
    }
}

#ifdef MTK_MUX_AWS_MCE_ENABLE
bt_status_t bt_aws_mce_report_register_mux_callback(bt_aws_mce_report_mux_callback_t callback)
{
    LOG_MSGID_I(bt_aws_mce_report, "register_event_callback",0);
    bt_aws_mce_report_init();

    if (!callback) {
        LOG_MSGID_E(bt_aws_mce_report, "rx_callback is NULL", 0);
        return BT_STATUS_FAIL;
    }

    bt_aws_mce_report_callback = callback;
    return BT_STATUS_SUCCESS;

}
#endif

bt_status_t bt_aws_mce_report_register_callback(bt_aws_mce_report_module_id_t  module_id, bt_aws_mce_report_callback_t callback)
{
    int32_t ret = BT_STATUS_FAIL;
    bt_aws_mce_report_init();

    if (!callback) {
        LOG_MSGID_E(bt_aws_mce_report, "callback is NULL", 0);
        return BT_STATUS_FAIL;
    }

    if(module_id >= BT_AWS_MCE_REPORT_MODULE_CM && 
       module_id < (BT_AWS_MCE_REPORT_MODULE_MAX + BT_AWS_MCE_REPORT_MODULE_CM)){
        uint8_t index = module_id - BT_AWS_MCE_REPORT_MODULE_START;
        bt_aws_mce_report_callback_table[index].callback = callback;
        bt_aws_mce_report_callback_table[index].module_id = module_id;
        ret = BT_STATUS_SUCCESS;
    }

    LOG_MSGID_I(bt_aws_mce_report, "bt_aws_mce_report_register_callback, 0x%0x, status: %d ", 2, module_id, ret);
    return ret;
}


bt_status_t bt_aws_mce_report_deregister_callback(bt_aws_mce_report_module_id_t  module_id)
{
    int32_t ret = BT_STATUS_FAIL;
    if(module_id >= BT_AWS_MCE_REPORT_MODULE_CM && 
       module_id < (BT_AWS_MCE_REPORT_MODULE_MAX + BT_AWS_MCE_REPORT_MODULE_CM)){
        uint8_t index = module_id - BT_AWS_MCE_REPORT_MODULE_START;
        if(bt_aws_mce_report_callback_table[index].callback){
            bt_aws_mce_report_callback_table[index].callback = NULL;
            bt_aws_mce_report_callback_table[index].module_id = 0;
            ret = BT_STATUS_SUCCESS;
        }
    }
    LOG_MSGID_I(bt_aws_mce_report, "bt_aws_mce_report_deregister_callback, 0x%0x, status: %d ", 2, module_id, ret);
    return ret;
}

bt_status_t bt_aws_mce_report_notify_user_event(bt_aws_mce_report_info_t *info)
{
    int32_t ret = BT_STATUS_FAIL;
    
#ifdef BT_AWS_MCE_REPORT_TEST
    if(info->module_id == BT_AWS_MCE_REPORT_MODULE_TEST){
        gpio_data = gpio_data == HAL_GPIO_DATA_HIGH ? HAL_GPIO_DATA_LOW : HAL_GPIO_DATA_HIGH;
        hal_gpio_set_output(HAL_GPIO_13,gpio_data);
        LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report]Test signl received,gpio_data:%d module:0x%0x", 2,gpio_data, info->module_id);
        return BT_STATUS_SUCCESS;
    }
#endif
    if(info->module_id >= BT_AWS_MCE_REPORT_MODULE_CM && 
       info->module_id < (BT_AWS_MCE_REPORT_MODULE_MAX + BT_AWS_MCE_REPORT_MODULE_CM)){
        uint8_t index = info->module_id - BT_AWS_MCE_REPORT_MODULE_START;
        if(bt_aws_mce_report_callback_table[index].module_id == info->module_id && 
            bt_aws_mce_report_callback_table[index].callback){
            bt_aws_mce_report_callback_table[index].callback(info);
            ret = BT_STATUS_SUCCESS;
        }
    }
    LOG_MSGID_I(bt_aws_mce_report, "bt_aws_mce_report_notify_user_event, 0x%0x, status: %d ", 2, info->module_id, ret);
    return ret;

}

#ifdef BT_AWS_MCE_REPORT_TEST
void bt_aws_mce_report_test_by_gpio_init(hal_gpio_pin_t port_num)
{
    hal_gpio_init(port_num);
    hal_pinmux_set_function(port_num, 0);
    hal_gpio_set_direction(port_num, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(port_num, HAL_GPIO_DATA_LOW);
}
#endif

bt_status_t bt_aws_mce_report_send(bt_aws_mce_report_info_t *info, bool urgent, bool is_sync)
{
    uint32_t aws_handle = 0;
    bt_clock_t play_clock = {0};
    if(info == NULL){
        return BT_STATUS_FAIL;
    }

#ifdef BT_AWS_MCE_REPORT_TEST
    if(info->module_id == BT_AWS_MCE_REPORT_MODULE_TEST){
        gpio_data = gpio_data == HAL_GPIO_DATA_HIGH ? HAL_GPIO_DATA_LOW : HAL_GPIO_DATA_HIGH;
        hal_gpio_set_output(HAL_GPIO_13,gpio_data);
        LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report]send event test signl, gpio_data: %x, module:0x%0x", 2,gpio_data, info->module_id);
    }
#endif

    if(is_sync){
        bt_sink_srv_bt_clock_addition(&play_clock, 0, info->sync_time* 1000);
    }
    bt_status_t status;
    uint32_t len;

    if(bt_aws_mce_report_cntx.role == BT_AWS_MCE_ROLE_CLINET) {
        LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report]client can not send event, module:0x%0x", 1, info->module_id);
        return BT_STATUS_FAIL;
    }

    /* check aws link handle */
    if (bt_aws_mce_report_cntx.aws_handle) {
        aws_handle = bt_aws_mce_report_cntx.aws_handle;
    } else {
        LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report] aws link is not exsit, aws handle is NULL", 0);
        return BT_STATUS_FAIL;
    }

    bt_aws_mce_information_t information;
    information.type = BT_AWS_MCE_INFORMATION_REPORT_APP;
    information.data_length = info->param_len;
    information.data = (uint8_t*)info->param;

#ifdef BT_AWS_MCE_REPORT_SPEAKER_ENABLE_INTERNAL
    if(bt_aws_mce_srv_get_mode() == BT_AWS_MCE_SRV_MODE_BROADCAST){
        bt_aws_mce_report_old_header_t old_header;
        uint8_t old_module_id = bt_aws_mce_report_get_mapped_module_id(BT_AWS_MCE_REPORT_MODULE_ID_MAP_TYPE_NEW_TO_OLD,info->module_id);
        if(BT_AWS_MCE_REPORT_INVALID_MODULE_ID == old_module_id){
            LOG_MSGID_W(bt_aws_mce_report, "[aws_mce_report]can not find mapped module_id, new_module:0x%0x, old_module: 0x%x", 2, 
                        info->module_id, old_module_id);
            old_module_id = info->module_id;
        }

        old_header.module_id = old_module_id;
        old_header.data_len = info->param_len;
        /* send to aws_mce profile */
        status = bt_aws_mce_send_information_with_extra_header(aws_handle, (const bt_aws_mce_information_t *)&information, urgent, &old_header, sizeof(old_header));
        LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report]send speaker event, new_module:0x%0x, old_module: 0x%x, aws_handle: 0x%x", 3, info->module_id, old_module_id, aws_handle);
        return status;
    }
#endif

    if(is_sync) {
        len = info->param_len + sizeof(bt_aws_mce_report_sync_payload_header_t);

        bt_aws_mce_report_sync_header_t aws_sync_header;
        bt_aws_mce_report_memset(&aws_sync_header, 0x00, sizeof(aws_sync_header));
        aws_sync_header.common_header.module_id = info->module_id;
        aws_sync_header.common_header.payload_length = len;
        aws_sync_header.sync_header.is_sync = true;
        aws_sync_header.sync_header.len = info->param_len;
        aws_sync_header.sync_header.nclk = play_clock.nclk;
        aws_sync_header.sync_header.nclk_intra = play_clock.nclk_intra;
        /* check data length */
        if (len > BT_AWS_MCE_PACKET_PAYLOAD_MAX_DATA_LENGTH) {
            LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report] Data len(%d) exceed the max(%d)", 2, len, BT_AWS_MCE_PACKET_PAYLOAD_MAX_DATA_LENGTH);
            return BT_STATUS_FAIL;
        }
        /* send to aws_mce profile */
        status = bt_aws_mce_send_information_with_extra_header(aws_handle, (const bt_aws_mce_information_t *)&information, urgent, (uint8_t *)&aws_sync_header, sizeof(aws_sync_header));
        LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report]send sync event, module:0x%0x, sync_time: %d, aws_handle: 0x%x", 3, info->module_id, info->sync_time, aws_handle);
    } else {
        len =  info->param_len + sizeof(bt_aws_mce_report_payload_header_t);
        bt_aws_mce_report_none_sync_header_t aws_none_sync_header;
        bt_aws_mce_report_memset(&aws_none_sync_header, 0x00, sizeof(aws_none_sync_header));
        aws_none_sync_header.common_header.module_id = info->module_id;
        aws_none_sync_header.common_header.payload_length = len;
        aws_none_sync_header.none_sync_header.is_sync = false;
        aws_none_sync_header.none_sync_header.len = info->param_len;
        /* check data length */
        if (len > BT_AWS_MCE_PACKET_PAYLOAD_MAX_DATA_LENGTH) {
            LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report] Data len(%d) exceed the max(%d)", 2, len, BT_AWS_MCE_PACKET_PAYLOAD_MAX_DATA_LENGTH);
            return BT_STATUS_FAIL;
        }
        /* send to aws_mce profile */
        status = bt_aws_mce_send_information_with_extra_header(aws_handle, (const bt_aws_mce_information_t *)&information, urgent, (uint8_t *)&aws_none_sync_header, sizeof(aws_none_sync_header));
        LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report]send event, module:0x%0x, aws_handle: 0x%x", 2, info->module_id, aws_handle);
    }
    return status;
}

uint32_t bt_aws_mce_report_mux_send(uint8_t *packet, uint32_t len, bool urgent)
{
    bt_status_t result = BT_STATUS_FAIL;
    uint32_t aws_handle = 0;

    if (NULL == packet) {
        LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report]mux send: Null data.", 0);
        return 0;
    }

    if (len > BT_AWS_MCE_MUX_PAYLOAD_MAX_DATA_LENGTH) {
        LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report] mux data len(%d) exceed the max(%d)", 2, len, BT_AWS_MCE_MUX_PAYLOAD_MAX_DATA_LENGTH);
        return 0;
    }

    if(bt_aws_mce_report_cntx.role == BT_AWS_MCE_ROLE_CLINET) {
        LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report]client can not send event", 0);
        return 0;
    }

    if (bt_aws_mce_report_cntx.aws_handle) {
        aws_handle = bt_aws_mce_report_cntx.aws_handle;
    } else {
        LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report] aws link is not exsit, aws handle is NULL", 0);
        return 0;
    }

    LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report] Get aws handle:%x", 1, aws_handle);

    bt_aws_mce_information_t information;
    information.type = BT_AWS_MCE_INFORMATION_REPORT_APP;
    information.data_length = len;
    information.data = (uint8_t*)packet;
    result = bt_aws_mce_send_information(aws_handle, (const bt_aws_mce_information_t *)&information, urgent);
    if(result != BT_STATUS_SUCCESS) {
        return 0;
    } else {
        return len;
    }
}

bt_status_t bt_aws_mce_report_send_event(bt_aws_mce_report_info_t *info)
{
    bt_status_t status;
    status = bt_aws_mce_report_send(info, false, false);
    return status;
}

bt_status_t bt_aws_mce_report_send_sync_event(bt_aws_mce_report_info_t *info)
{
    bt_status_t status;
    status = bt_aws_mce_report_send(info, true, true);
    return status;
}

bt_status_t bt_aws_mce_report_send_urgent_event(bt_aws_mce_report_info_t *info)
{
    bt_status_t status;
    status = bt_aws_mce_report_send(info, true, false);
    return status;
}

bt_status_t bt_aws_mce_report_notify_callback(bt_aws_mce_information_t *buffer)
{
    //bt_aws_mce_information_ind_t *data_ind = (bt_aws_mce_information_ind_t *)buffer;
#ifdef BT_AWS_MCE_REPORT_SPEAKER_ENABLE_INTERNAL
        if(bt_aws_mce_srv_get_mode() == BT_AWS_MCE_SRV_MODE_BROADCAST){
            bt_aws_mce_report_old_packet_t* old_packet = (bt_aws_mce_report_old_packet_t*)buffer->data;
            bt_aws_mce_report_info_t info;
            info.is_sync = false;
            info.sync_time = 0;
            info.param_len = old_packet->data_len;
            info.param = old_packet->data;
            LOG_MSGID_I(bt_aws_mce_report,"[aws_mce_report]bt_aws_mce_report_notify_callback: info_type:%x, info_padding:%x, info_data_length:%x, old_module_id = 0x%0x", 
            4, buffer->type, buffer->padding, buffer->data_length, old_packet->module_id);
            if (old_packet->module_id >= BT_AWS_MCE_REPORT_MODULE_START && old_packet->module_id <= BT_AWS_MCE_REPORT_MODULE_START + BT_AWS_MCE_REPORT_MODULE_MAX) {
                info.module_id = old_packet->module_id;
            } else if (old_packet->module_id < BT_AWS_MCE_REPORT_MODULE_START) {
                info.module_id = bt_aws_mce_report_get_mapped_module_id(BT_AWS_MCE_REPORT_MODULE_ID_MAP_TYPE_OLD_TO_NEW, old_packet->module_id);
                LOG_MSGID_I(bt_aws_mce_report,"[aws_mce_report]bt_aws_mce_report_notify_callback: old_module_id = 0x%0x, new_module_id = 0x%x",
                    2, old_packet->module_id, info.module_id);
                if(BT_AWS_MCE_REPORT_INVALID_MODULE_ID == info.module_id) {
                    LOG_MSGID_E(bt_aws_mce_report,"[aws_mce_report]bt_aws_mce_report_notify_callback error, can not find mapped module_id: 0x%0x", 1,old_packet->module_id);
                    return BT_STATUS_FAIL;
                }
            }
            return bt_aws_mce_report_notify_user_event(&info);
        }
#endif

    bt_aws_mce_report_packet_t *packet = (bt_aws_mce_report_packet_t *)(buffer->data);
    bool is_sync = (bool)( packet->payload[0]);
    bt_aws_mce_report_info_t info;
    bt_status_t status;

    if (is_sync) {
        int32_t duration;
        bt_clock_t sync_clk;
        bt_aws_mce_report_sync_payload_t *sync_payload = (bt_aws_mce_report_sync_payload_t *) & (packet->payload[0]);

        info.is_sync = true;
        info.module_id = packet->header.module_id;
        info.param_len = sync_payload->payload_header.len;
        info.param = bt_aws_mce_report_alloc( info.param_len);
        bt_aws_mce_report_memcpy(info.param, sync_payload->param, info.param_len);
        sync_clk.nclk = sync_payload->payload_header.nclk;
        sync_clk.nclk_intra = sync_payload->payload_header.nclk_intra;
        bt_sink_srv_bt_clock_get_duration(&sync_clk, 0, &duration);
        info.sync_time = (uint32_t)(duration / 1000);
        LOG_MSGID_I(bt_aws_mce_report,"[aws_mce_report]bt_aws_mce_report_notify_callback, duration :%d, nclk:0x%0x, intra: 0x%0x", 3,
                    duration, sync_clk.nclk, sync_clk.nclk_intra);
    } else {
        bt_aws_mce_report_payload_t *payload = (bt_aws_mce_report_payload_t *) & (packet->payload[0]);
        info.is_sync = false;
        info.module_id = packet->header.module_id;
        info.param_len = payload->payload_header.len;
        info.param = bt_aws_mce_report_alloc( info.param_len);
        bt_aws_mce_report_memcpy(info.param, payload->param, info.param_len);
        info.sync_time = 0;
    }
    status = bt_aws_mce_report_notify_user_event(&info);
    bt_aws_mce_report_free( info.param);
    return status;
}

static void bt_aws_mce_report_state_change_ind_handler(bt_status_t status, void *buff)
{
    bt_aws_mce_state_change_ind_t *ind = (bt_aws_mce_state_change_ind_t *)buff;
    //bt_aws_mce_report_cntx.aws_state = ind->state;
    bt_aws_mce_report_cntx.role = bt_connection_manager_device_local_info_get_aws_role();
    LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report] state_change_ind_handler, state:0x%0x, aws hdl:0x%x , context hdl:0x%x, role:0x%0x",
        4,
        ind->state,
        ind->handle,
        bt_aws_mce_report_cntx.aws_handle,
        bt_aws_mce_report_cntx.role);

#ifdef MTK_BT_SPEAKER_ENABLE
       if (bt_aws_mce_report_cntx.role == BT_AWS_MCE_ROLE_AGENT && bt_aws_mce_srv_get_mode() == BT_AWS_MCE_SRV_MODE_BROADCAST) {
           if ((ind->state & 0xF0) >= BT_AWS_MCE_AGENT_STATE_CONNECTABLE) {
               bt_aws_mce_report_cntx.aws_handle = ind->handle;
               bt_aws_mce_report_cntx.aws_state = BT_AWS_MCE_AGENT_STATE_CONNECTABLE;
           } else if ((ind->state & 0xF0) == BT_AWS_MCE_AGENT_STATE_INACTIVE && bt_aws_mce_report_cntx.aws_handle == ind->handle) {
               bt_aws_mce_report_cntx.aws_handle = 0;
               bt_aws_mce_report_cntx.aws_state = BT_AWS_MCE_AGENT_STATE_INACTIVE;
           }
           return;
       }
#endif
//RHO case: agent->partner
    if (bt_aws_mce_report_cntx.aws_state == BT_AWS_MCE_AGENT_STATE_ATTACHED && (ind->state & 0xF0) == BT_AWS_MCE_AGENT_STATE_NONE) {
        bt_aws_mce_report_cntx.aws_state = BT_AWS_MCE_AGENT_STATE_NONE;
        bt_aws_mce_report_cntx.aws_handle = ind->handle;
        return;
    }
//RHO case:partner->agent
    if (bt_aws_mce_report_cntx.aws_state == BT_AWS_MCE_AGENT_STATE_NONE && (ind->state & 0xF0) == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
        bt_aws_mce_report_cntx.aws_state = BT_AWS_MCE_AGENT_STATE_ATTACHED;
        bt_aws_mce_report_cntx.aws_handle = ind->handle;
        return;
    }

    if (bt_aws_mce_report_cntx.role == BT_AWS_MCE_ROLE_AGENT) {
        if ((ind->state & 0xF0) == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
            bt_aws_mce_report_cntx.aws_handle = ind->handle;
            } else if ((ind->state & 0xF0) != BT_AWS_MCE_AGENT_STATE_ATTACHED && bt_aws_mce_report_cntx.aws_handle == ind->handle) {
            bt_aws_mce_report_cntx.aws_handle = 0;
            bt_aws_mce_report_memset(&(bt_aws_mce_report_cntx.remote_addr), 0, sizeof(bt_bd_addr_t));
      } else {
      }
      bt_aws_mce_report_cntx.aws_state = (ind->state & 0xF0);
    }

}

static bt_status_t bt_aws_mce_report_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report] event callback, msg 0x%x, status 0x%x", 2, msg, status);

    switch (msg) {
        case BT_AWS_MCE_CONNECTED: {
            if (status != BT_STATUS_SUCCESS) {
                 return BT_STATUS_SUCCESS;
            }
            bt_aws_mce_connected_t *connected = (bt_aws_mce_connected_t *)buff;
            bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
            bt_aws_mce_report_cntx.role = bt_connection_manager_device_local_info_get_aws_role();
            if((bt_aws_mce_report_cntx.role == BT_AWS_MCE_ROLE_CLINET || bt_aws_mce_report_cntx.role == BT_AWS_MCE_ROLE_PARTNER) &&
              (bt_aws_mce_report_memcmp(local_addr, connected->address, sizeof(bt_bd_addr_t)) != 0)) {
                bt_aws_mce_report_cntx.aws_handle = connected->handle;
                bt_aws_mce_report_cntx.aws_state = BT_AWS_MCE_AGENT_STATE_NONE;
                bt_aws_mce_report_memcpy((void *)(&(bt_aws_mce_report_cntx.remote_addr)), (void *)(connected->address), sizeof(bt_bd_addr_t));
                LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report] aws link is connected, aws handle: 0x%0x,", 1, connected->handle);
            }
            break;
        }
        case BT_AWS_MCE_DISCONNECTED: {
            bt_aws_mce_disconnected_t *ind = (bt_aws_mce_disconnected_t *)buff;
            if (ind->handle == bt_aws_mce_report_cntx.aws_handle) {
                bt_aws_mce_report_cntx.aws_handle = 0;
                bt_aws_mce_report_cntx.aws_state = BT_AWS_MCE_AGENT_STATE_NONE;
                bt_aws_mce_report_memset(&(bt_aws_mce_report_cntx.remote_addr), 0, sizeof(bt_bd_addr_t));
                LOG_MSGID_I(bt_aws_mce_report, "[aws_mce_report] aws link is disconnected, aws handle: 0x%0x,", 1, ind->handle);
            }
            break;
        }
        case BT_AWS_MCE_INFOMATION_PACKET_IND: {
            bt_aws_mce_information_ind_t *if_packet = (bt_aws_mce_information_ind_t *)buff;
            bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
            if(role & 0x0C){
                return BT_STATUS_FAIL;
            }
            if (if_packet->packet.type == BT_AWS_MCE_INFORMATION_REPORT_APP) {
                bt_aws_mce_report_notify_callback(&(if_packet->packet));
#ifdef MTK_MUX_AWS_MCE_ENABLE
                bt_aws_mce_report_callback(msg, if_packet->packet.data, if_packet->packet.data_length);
#endif
            }
            break;
        }
        case BT_AWS_MCE_STATE_CHANGED_IND: {
            bt_aws_mce_state_change_ind_t *state_change = (bt_aws_mce_state_change_ind_t *)buff;
            bt_aws_mce_report_state_change_ind_handler(status, state_change);
            break;
        }
        default:
            break;
    }
    return BT_STATUS_SUCCESS;

}


#endif /*#if defined(MTK_AWS_MCE_ENABLE) */

