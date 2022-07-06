/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
  * File: ut_app_hid.c
  *
  * Description: This file could control hid connect and control etc.
  *
  */
#if 0
#include "bt_sdp.h"
#include "bt_type.h"
#include "bt_hid.h"
#include "bt_gap.h"
#include "bt_hfp.h"
#ifndef __TS_WIN32__
#include <FreeRTOS.h>
#include "timers.h"
#endif
#include "syslog.h"
#include "apps_debug.h"
#include "atci.h"
#include <stdlib.h>

#define UT_APP_CMP(_cmd) (strncmp((_cmd), cmd, strlen(_cmd)) == 0)


void copy_str_to_addr(uint8_t *addr, const char *str)
{
    unsigned int i, value;
    int using_long_format = 0;
    int using_hex_sign = 0;

    if (str[2] == ':' || str[2] == '-') {
        using_long_format = 1;
    }

    if (str[1] == 'x') {
        using_hex_sign = 2;
    }

    for (i=0; i<6; i++) {
        sscanf(str+using_hex_sign+i*(2+using_long_format), "%02x", &value);
        addr[5-i] = (uint8_t) value;
    }
}


static const uint8_t bt_hid_service_class_id_pattern[] = {
    BT_HID_SDP_SERVICE_CLASS_ID
};

static const uint8_t bt_hid_protocol_descriptor_list_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(BT_HID_CONTROL_CHANNEL)

};


static const uint8_t bt_hid_language_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_LANGUAGE
};


static const uint8_t bt_hid_addition_protocol_descriptor_list_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_ADDTIONAL_PROTOCOL_DESCRIPTOR(BT_HID_INTERRUPT_CHANNEL)

};

static const uint8_t bt_hid_service_name_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_SIZE_OF_SERVICE_NAME(4),
    'K', 'y', 'b', '\0'
};

static const uint8_t bt_hid_bt_profile_descriptor_list_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_BT_PROFILE_DESCRIPTOR
};

static const uint8_t bt_hid_parser_version_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_PARSERVERSION
};

static const uint8_t bt_hid_device_subclass_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_DEVICE_SUBCLASS
};

static const uint8_t bt_hid_device_country_code_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_COUNTRYCODE
};


static const uint8_t bt_hid_virtural_cable_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_VITRUALCABLE
};

static const uint8_t bt_hid_reconnection_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_RECONNECTION
};

static const uint8_t bt_hid_version_number_list_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_VERSION_NUMBER_LIST
};


#define HID_REPORT_DESCRIPTOR \
	0x05, 0x01, 0x09, 0x06,0xA1, \
        0x01, 0x15, 0x00, 0x25,0x01, \
        0x0b, 0x66, 0x00, 0x07, 0x00, \
	0x0b, 0xe2, 0x00, 0x0c, 0x00, \
	0x0b, 0x50, 0x00, 0x07, 0x00, \
	0x0b, 0xb4, 0x00, 0x0c, 0x00, \
	0x0b, 0xe9, 0x00, 0x0c, 0x00, \
	0x0b, 0xf1, 0x00, 0x07, 0x00, \
	0x0b, 0x58, 0x00, 0x07, 0x00, \
	0x0b, 0xcd, 0x00, 0x0c, 0x00, \
	0x0b, 0x89, 0x00, 0x0c, 0x00, \
	0x0b, 0x52, 0x00, 0x07, 0x00, \
	0x0b, 0x4f, 0x00, 0x07, 0x00, \
	0x0b, 0xb3, 0x00, 0x0c, 0x00, \
	0x0b, 0xea, 0x00, 0x0c, 0x00, \
	0x0b, 0x23, 0x02, 0x0c, 0x00, \
	0x0b, 0x51, 0x00, 0x07, 0x00, \
	0x0b, 0x40, 0x00, 0x0c, 0x00, \
	0x0b, 0x21, 0x02, 0x0c, 0x00, \
	0x95, 0x11, 0x75, 0x01, 0x81, \
	0x02, 0x95, 0x07, 0x75, 0x01, \
	0x81, 0x03, 0x15, 0x00, 0x25, \
	0x64, 0x0b, 0x20, 0x00, 0x06, \
	0x00, 0x95, 0x01, 0x75, 0x08, \
	0x81, 0x02, 0xc0
uint8_t g_o_tab[20][3] = {
    { 0x10, 0x00, 0x00},
    { 0x10, 0x01, 0x00},
    { 0x10, 0x02, 0x00},
    { 0x10, 0x00, 0x04},
    { 0x10, 0x04, 0x00},
    { 0x10, 0x08, 0x00},
    { 0x10, 0x10, 0x00},
    { 0x10, 0x80, 0x00},
    { 0x10, 0x80, 0x01},
    { 0x10, 0x80, 0x03},
};

uint8_t g_key_tab[18][4] = {
    {0x00, 0x00, 0x00, 0x64},
    {0x01, 0x00, 0x00, 0x64}, /*sleep*/
    {0x02, 0x00, 0x00, 0x64}, /*mute*/
    {0x04, 0x00, 0x00, 0x64}, /*lef*/
    {0x08, 0x00, 0x00, 0x64}, /*rewind*/

    {0x10, 0x00, 0x00, 0x64}, /*volume up*/
    {0x20, 0x00, 0x00, 0x64}, /*black*/
    {0x40, 0x00, 0x00, 0x64}, /*enter*/
    {0x80, 0x00, 0x00, 0x64}, /*play_pause*/

    {0x00, 0x01, 0x00, 0x64}, /*media select*/
    {0x00, 0x02, 0x00, 0x64}, /*up*/
    {0x00, 0x04, 0x00, 0x64}, /*right*/
    {0x00, 0x08, 0x00, 0x64}, /*fast*/
    {0x00, 0x10, 0x00, 0x64}, /*volume down*/

    {0x00, 0x20, 0x00, 0x64}, /*home*/
    {0x00, 0x40, 0x00, 0x64}, /*down*/
    {0x00, 0x80, 0x00, 0x64}, /*menu*/
    {0x00, 0x00, 0x01, 0x64}
};



static const uint8_t bt_hid_descriptor_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_HID_DESCRIPTOR_LIST(123),
    HID_REPORT_DESCRIPTOR
};

static const uint8_t bt_hid_langid_base_list_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_LANGID_BASE_ID_LIST
};

static const uint8_t bt_hid_boot_device_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_BOOTDEVICE
};

static const uint8_t bt_hid_attibute_list[] = {
    BT_HID_ATTRIBUTE_ID_LIST
};

static const uint8_t bt_hid_profile_pattern[] = {
    BT_HID_SDP_ATTRIBUTE_HID_VERSION
};

static const bt_sdps_attribute_t bt_hid_sdp_attributes_pattern[] = {
    /* Service Class ID List attribute */
    BT_HID_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(bt_hid_service_class_id_pattern),
    /* Protocol Descriptor List attribute */
    BT_HID_SDP_ATTRIBUTE_PROTOCOL_DESC_LIST(bt_hid_protocol_descriptor_list_pattern),
    /* Language Base ID List attribute */
    BT_HID_SDP_ATTRIBUTE_LANGUAGE_BASE_LIST(bt_hid_language_pattern),
    /*bluetooth protocol*/
    BT_HID_SDP_ATTRIBUTE_BT_PROFILE_DESC_LIST(bt_hid_bt_profile_descriptor_list_pattern),

    BT_HID_SDP_ATTRIBUTE_ID_LIST(bt_hid_attibute_list),
    /* Addational protocol*/
    BT_HID_SDP_ATTRIBUTE_ADDITIONAL_PROTOCOL_DESC_LIST(bt_hid_addition_protocol_descriptor_list_pattern),

    /* Serial Port Profile Service Name */
    BT_HID_SDP_ATTRIBUTE_SERVICE_NAME(bt_hid_service_name_pattern),

    BT_HID_SDP_ATTRIBUTE_VERSION_NUMBER(bt_hid_version_number_list_pattern),

    BT_HID_SDP_ATTRIBUTE_PARSER_VERSION(bt_hid_parser_version_pattern),

    BT_HID_SDP_ATTRIBUTE_DEVICE_SUB_CLASS(bt_hid_device_subclass_pattern),

    BT_HID_SDP_ATTRIBUTE_COUNTRY_CODE(bt_hid_device_country_code_pattern),

    BT_HID_SDP_ATTRIBUTE_VIRTUAL_CABLE(bt_hid_virtural_cable_pattern),

    BT_HID_SDP_ATTRIBUTE_RECONNECTION_INIITALIZATION(bt_hid_reconnection_pattern),
    BT_HID_SDP_ATTRIBUTE_BOOT_DEVICE(bt_hid_boot_device_pattern),

    BT_HID_SDP_ATTRIBUTE_HID_DESCRIPTOR(bt_hid_descriptor_pattern),

    BT_HID_SDP_ATTRIBUTE_LANGID_BASE_LIST(bt_hid_langid_base_list_pattern),

    BT_HID_SDP_ATTRIBUTE_HID_PROFILE(bt_hid_profile_pattern),
};


const bt_sdps_record_t bt_hid_sdp_record_1 = {
    .attribute_list_length = sizeof(bt_hid_sdp_attributes_pattern),
    .attribute_list = bt_hid_sdp_attributes_pattern,
};


#define MAX_DEV_NUM (2)
typedef struct {
    uint32_t handle;
    uint8_t cmd;
    uint8_t index;
#ifndef __TS_WIN32__
    TimerHandle_t timer_id;
    TimerHandle_t release_timer;

#endif
} ut_app_hid_cntx_t;

uint8_t g_flag, g_count, is_init, is_stop;
uint8_t g_hid_index = 0;
//extern uint32_t g_hfp_handle;
ut_app_hid_cntx_t hid_cntx[MAX_DEV_NUM];

void ut_app_create_hid_cntx(uint32_t handle)
{
    uint8_t i = 0;
    for (; i < MAX_DEV_NUM; i++) {
        if (!hid_cntx[i].handle) {
            hid_cntx[i].handle = handle;
            break;
        }
    }
}

void ut_app_clear_hid_cntx(uint32_t handle)
{
    uint8_t i = 0;
    for (; i < MAX_DEV_NUM; i++) {
        if (hid_cntx[i].handle == handle) {
            hid_cntx[i].handle = 0;
            break;
        }
    }
}

ut_app_hid_cntx_t *ut_app_hid_get_cntx(uint8_t index)
{
    return &hid_cntx[index];
}


void bt_hid_release_timeout_callback()
{
    uint8_t app_data[60];
    bt_hid_data_t value;
    bt_status_t status;

    value.packet = app_data;


#ifndef __TS_WIN32__
    TimerHandle_t timer = hid_cntx[0].release_timer;
    APPS_LOG_I("bt_hid_release_timeout_callback: timer = %x", 1, timer);
    if (timer) {
        xTimerStop(timer, 0);
        xTimerDelete(timer, 0);
        g_count = 0;
    }
#endif

    memcpy(value.packet, g_key_tab[0], 4);
    value.type = BT_HID_INPUT;
    value.packet_len = 4;
    status = bt_hid_send_data(hid_cntx[0].handle, &value);
    APPS_LOG_I("bt_hid_release_timeout_callback:status  = %d", 1, status);

    if  (hid_cntx[0].index == 17) {
        //status = bt_hfp_audio_transfer(g_hfp_handle, BT_HFP_AUDIO_TO_AG);
        APPS_LOG_I("bt_hid_release_timeout_callback : result = %d", 1, status);
    }
}


bt_status_t bt_hid_notify_data(uint8_t index, uint8_t count)
{
    bt_status_t status = BT_STATUS_FAIL;
    uint8_t j = 0;
    uint8_t app_data[60];
    bt_hid_data_t value;
    value.packet = app_data;

    APPS_LOG_I("bt_hid_notify_data: index:%x, count:%x", 2, index, count);

    for (j = 0; j < count; j++) {
        memcpy(value.packet, g_key_tab + index, 4);
        value.type = BT_HID_INPUT;
        value.packet_len = 4;
        APPS_LOG_I("hid_notify_data: i = %d,value = %x", 2, index, value.packet[0]);
        status = bt_hid_send_data(hid_cntx[0].handle, &value);
        APPS_LOG_I("hid_notify_data: send data: index = %d, status = %d", 2, index, status);
    }
#ifdef __TS_WIN32__
    if (status == BT_STATUS_SUCCESS) {
#else
APPS_LOG_I("hid_notify_data: timer_id = %x", 1, hid_cntx[0].timer_id);
if (status == BT_STATUS_SUCCESS && hid_cntx[0].timer_id == 0) {
        hid_cntx[0].release_timer = xTimerCreate("Hid Timer", 0xffff, pdFALSE, (void *)0,
                                    (TimerCallbackFunction_t)bt_hid_release_timeout_callback);
        APPS_LOG_I("start release timer: timer_id = %x", 1, hid_cntx[0].release_timer);

        xTimerChangePeriod(hid_cntx[0].release_timer,  200 / portTICK_PERIOD_MS, 0);
        xTimerReset(hid_cntx[0].release_timer, 0);
#endif
        hid_cntx[0].index = index;

    }
    return status;
}

#define BT_HID_TX_TIMEOUT    250

void bt_hid_timeout_callback()
{
    bt_status_t result = BT_STATUS_SUCCESS;
    uint8_t cmd = hid_cntx[0].cmd;
    g_count++;
#ifndef __TS_WIN32__
    TimerHandle_t timer = hid_cntx[0].timer_id;
#endif
    APPS_LOG_I("timeout_callback:cmd = %d, flag = %d, g_count = %d, is_stop = %d", 4, cmd, g_flag, g_count, is_stop);
    if (is_stop) {
        is_stop = 0;
    }
    if (g_count == g_flag) {
#ifndef __TS_WIN32__
        if (timer) {
            xTimerStop(timer, 0);
            xTimerDelete(timer, 0);
            g_count = 0;
        }
#endif
        APPS_LOG_I("long press release", 0);
        bt_hid_notify_data(0, 1);
#ifndef __TS_WIN32__
        hid_cntx[0].timer_id = 0;
#endif
        if  (cmd == 17) {
            //result = bt_hfp_audio_transfer(g_hfp_handle, BT_HFP_AUDIO_TO_AG);
            APPS_LOG_I("timeout_disconnect : result = %d", 1, result);
        }
    } else {
#ifndef __TS_WIN32__
        if (timer) {
            xTimerChangePeriod(hid_cntx[0].timer_id, BT_HID_TX_TIMEOUT / portTICK_PERIOD_MS, 0);
            xTimerReset(hid_cntx[0].timer_id, 0);
        }
#endif
        if (cmd) {
            bt_hid_notify_data(cmd, 1);
        }
    }
}

void bt_hid_start_timer()
{
#ifndef __TS_WIN32__
    hid_cntx[0].timer_id = xTimerCreate("Hid Timer", 0xffff, pdFALSE, (void *)0,
                                        (TimerCallbackFunction_t)bt_hid_timeout_callback);
    APPS_LOG_I("start timer: timer_id = %x", 1, hid_cntx[0].timer_id);

    xTimerChangePeriod(hid_cntx[0].timer_id,  BT_HID_TX_TIMEOUT / portTICK_PERIOD_MS, 0);
    xTimerReset(hid_cntx[0].timer_id, 0);
#else
    bt_hid_timeout_callback();
#endif
}


void bt_hid_long_press_set_timer(uint8_t cmd, uint32_t count)
{
    count = count * 1000;
    g_flag = count / 250;
    APPS_LOG_I("press_set_time: count = %d, g_flag = %d, cmd = %d", 3, count, g_flag, cmd);
    hid_cntx[0].cmd = cmd;
    bt_hid_start_timer();// timer_start();
    bt_hid_notify_data(cmd, 1);
}

void bt_hid_stop_lp()
{
#ifndef __TS_WIN32__
    TimerHandle_t timer = hid_cntx[0].timer_id;
    APPS_LOG_I("bt_hid_stop_lp:timer = %x, g_count = %d", 1, timer);

    if (timer) {
        is_stop = 1;
        xTimerStop(timer, 0);
        xTimerDelete(timer, 0);
        hid_cntx[0].timer_id = 0;
        g_count = 0;
    }
#endif
}

bt_bd_addr_t addr;
int8_t hfp_channel;
const char *cmd;

void bt_ut_app_clear_link_key(bt_bd_addr_t *addr);
bt_status_t bt_hid_io_callback(atci_parse_cmd_param_t *parse_cmd)
{
    cmd = parse_cmd->string_ptr + 7;
    printf("g_cmd:%s, inputs:%s", cmd, parse_cmd->string_ptr);
    bt_status_t status = BT_STATUS_FAIL;
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_OK};

    ut_app_hid_cntx_t *cntx = ut_app_hid_get_cntx(g_hid_index);
    is_stop = 0;
    bt_hid_stop_lp();
    APPS_LOG_I("hid_io_callback: , index = %d, handle = %x", 2, g_hid_index, cntx->handle);
    if (UT_APP_CMP("bt hid scan mode")) {
        const char cmd[240] = {0x17, 0x09,
                               0x41, 0x6d, 0x61, 0x7a, 0x6f,
                               0x6e, 0x20, 0x46, 0x69, 0x72,
                               0x65, 0x20, 0x54, 0x56, 0x20,
                               0x52, 0x65, 0x6d, 0x6f, 0x74,
                               0x65, 0x00,
                               0x02, 0x0a, 0x04, 0x09, 0xff,
                               0x31, 0x39, 0x34, 0x39, 0x30,
                               0x34, 0x30, 0x31
                              };
        bt_gap_set_extended_inquiry_response((uint8_t *)cmd, 240);
        bt_gap_write_page_scan_activity(0x66, 0x24);//(0xce, 0x24);
        bt_gap_write_inquiry_scan_activity(0x66, 0x24);//(0x19a, 0x24);
        bt_gap_set_scan_mode(3);
    } else if (UT_APP_CMP("bt hid disconnect")) {
        bt_hid_disconnect(hid_cntx[0].handle);
    } else if (UT_APP_CMP("bt hid sleep")) {
        bt_hid_notify_data(1, 1);
    } else if (UT_APP_CMP("bt hid mute")) {
        bt_hid_notify_data(2, 1);
    } else if (UT_APP_CMP("bt hid left")) {
        bt_hid_notify_data(3, 1);
    } else if (UT_APP_CMP("bt hid rewind")) {
        bt_hid_notify_data(4, 1);
    } else if (UT_APP_CMP("bt hid volume up")) {
        bt_hid_notify_data(5, 1);
    } else if (UT_APP_CMP("bt hid back")) {
        bt_hid_notify_data(6, 1);
    } else if (UT_APP_CMP("bt hid enter")) {
        bt_hid_notify_data(7, 1);
    } else if (UT_APP_CMP("bt hid play") || UT_APP_CMP("bt hid pause")) {
        bt_hid_notify_data(8, 1);
    } else if (UT_APP_CMP("bt hid media select")) {
        bt_hid_notify_data(9, 1);
    } else if (UT_APP_CMP("bt hid up")) {
        bt_hid_notify_data(10, 1);
    } else if (UT_APP_CMP("bt hid right")) {
        bt_hid_notify_data(11, 1);
    } else if (UT_APP_CMP("bt hid forward")) {
        bt_hid_notify_data(12, 1);
    } else if (UT_APP_CMP("bt hid volume down")) {
        bt_hid_notify_data(13, 1);
    } else if (UT_APP_CMP("bt hid home")) {
        printf("enter hid home");
        bt_hid_notify_data(14, 1);
    } else if (UT_APP_CMP("bt hid down")) {
        bt_hid_notify_data(15, 1);
    } else if (UT_APP_CMP("bt hid menu")) {
        bt_hid_notify_data(16, 1);
    } else if (UT_APP_CMP("bt hid search")) {
        //bt_status_t result;
        bt_hid_notify_data(17, 1);
        //result = bt_hfp_audio_transfer(g_hfp_handle, BT_HFP_AUDIO_TO_HF);
        // APPS_LOG_I("UT_HID", "result %d", 1, result);

    } else if (UT_APP_CMP("bt hid lp up")) {

        uint32_t count = atoi(cmd + 13);
        bt_hid_long_press_set_timer(10, count);

    } else if (UT_APP_CMP("bt hid lp down")) {
        uint8_t count = atoi(cmd + 15);
        APPS_LOG_I( "io_callback: long press down: count = %d", 1, count);
        bt_hid_long_press_set_timer(15, count);
    } else if (UT_APP_CMP("bt hid lp left")) {
        uint8_t count = atoi(cmd + 15);
        APPS_LOG_I( "io_callback:long press left count = %d", 1, count);
        bt_hid_long_press_set_timer(3, count);
        //bt_hid_notify_data(3, count);
    } else if (UT_APP_CMP("bt hid lp right")) {
        uint8_t count = atoi(cmd + 16);
        APPS_LOG_I( "io_callback:long press right count = %d", 1, count);
        bt_hid_long_press_set_timer(11, count);
    } else if (UT_APP_CMP("bt hid lp search")) {
        //bt_status_t result;
        uint8_t count = atoi(cmd + 17);
        APPS_LOG_I( "io_callback:long press right count = %d", 1, count);
        bt_hid_long_press_set_timer(17, count);
        /*if (g_hfp_handle) {
            result = bt_hfp_audio_transfer(g_hfp_handle, BT_HFP_AUDIO_TO_HF);
        }

        APPS_LOG_I("UT_HID", "result %d, handle = %x", result, g_hfp_handle);*/
    } else if (UT_APP_CMP("bt hid test")) {

        bt_hid_notify_data(16, 1);
        bt_hid_notify_data(7, 1);

    } else if (UT_APP_CMP("bt hid lp sleep")) {

        uint32_t count = atoi(cmd + 16);

        bt_hid_long_press_set_timer(1, count);

    } else if (UT_APP_CMP("bt hid lp mute")) {

        uint32_t count = atoi(cmd + 15);
        bt_hid_long_press_set_timer(2, count);

    } else if (UT_APP_CMP("bt hid lp rewind")) {

        uint32_t count = atoi(cmd + 17);

        bt_hid_long_press_set_timer(4, count);

    } else if (UT_APP_CMP("bt hid lp volume up")) {

        uint32_t count = atoi(cmd + 20);

        bt_hid_long_press_set_timer(5, count);
    } else if (UT_APP_CMP("bt hid lp back")) {

        uint32_t count = atoi(cmd + 15);

        bt_hid_long_press_set_timer(6, count);
    } else if (UT_APP_CMP("bt hid lp enter")) {

        uint32_t count = atoi(cmd + 16);

        bt_hid_long_press_set_timer(7, count);

    } else if (UT_APP_CMP("bt hid lp play")) {

        uint32_t count = atoi(cmd + 15);

        bt_hid_long_press_set_timer(8, count);
    } else if (UT_APP_CMP("bt hid lp media select")) {

        uint32_t count = atoi(cmd + 23);

        bt_hid_long_press_set_timer(9, count);

    } else if (UT_APP_CMP("bt hid lp forward")) {

        uint32_t count = atoi(cmd + 18);

        bt_hid_long_press_set_timer(12, count);

    } else if (UT_APP_CMP("bt hid lp volume down")) {

        uint32_t count = atoi(cmd + 22);

        bt_hid_long_press_set_timer(13, count);
    } else if (UT_APP_CMP("bt hid lp home")) {

        uint32_t count = atoi(cmd + 15);

        bt_hid_long_press_set_timer(14, count);
    } else if (UT_APP_CMP("bt hid lp menu")) {

        uint32_t count = atoi(cmd + 15);

        bt_hid_long_press_set_timer(16, count);
    } else if (UT_APP_CMP("bt hid connect")) {
        uint32_t handle;
        uint8_t total_len = strlen(cmd);
        uint8_t len = strlen("bt hid connect ");
        hfp_channel = atoi((cmd + len));
        APPS_LOG_I("bt hid connect hfp channel number: %d", 1, hfp_channel);
        if (total_len == 30) {
            copy_str_to_addr((uint8_t *)&addr, cmd + strlen("bt hid connect xx "));
        } else {
            copy_str_to_addr((uint8_t *)&addr, cmd + strlen("bt hid connect x "));
        }
        status = bt_hid_connect(&handle, (const bt_bd_addr_t *)&addr);
        if (status == BT_STATUS_SUCCESS || status == BT_STATUS_PENDING) {
            is_init = 1;
        }
        APPS_LOG_I("HID_CONNECT: status = %d", 1, status);
    }	else if (UT_APP_CMP("bt hid move")) {
        bt_hid_data_t data;
        uint8_t app_data[3];
        data.packet = app_data;
        data.packet[0] = 1;
        data.packet[1] = 0;
        data.type = 1;
        data.packet_len = 2;
        bt_hid_send_data(hid_cntx[0].handle, &data);
    }   else if (UT_APP_CMP("bt hid unplug")) {
        bt_bd_addr_t *addr = bt_hid_get_bd_addr_by_handle(hid_cntx[0].handle);
        bt_hid_send_hid_control(hid_cntx[0].handle, 5);
        //bt_ut_app_clear_link_key(addr);
        APPS_LOG_I("hid_unplug: status = %x", 1, addr);
    } else if (UT_APP_CMP("bt hid clear key")) {
        bt_bd_addr_t *addr = bt_hid_get_bd_addr_by_handle(hid_cntx[0].handle);
        //bt_ut_app_clear_link_key(addr);
        APPS_LOG_I("hid_clear_link_key: addr = %x", 1, addr);
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return BT_STATUS_SUCCESS;
}


bt_status_t bt_app_hid_event_callback(
    bt_msg_type_t msg,
    bt_status_t status,
    void *buff)
{
    APPS_LOG_I("msg = %x, status = %d", 2, msg, status);
    switch (msg) {
        case BT_HID_CONNECT_IND: {
            bt_status_t con_status;
            bt_hid_connect_ind_t  *ind = (bt_hid_connect_ind_t *)buff;

            con_status = bt_hid_connect_response(ind->handle, true);
            bt_gap_set_scan_mode(0);
            APPS_LOG_I("CONNECT_CNF: status = %d", 1, con_status);
            break;
        }
        case BT_HID_CONNECT_CNF: {
            bt_hid_connect_cnf_t *cnf = (bt_hid_connect_cnf_t *)buff;
            APPS_LOG_I("CONNECT_CNF: HID CONNECT SUCCESS", 0);

            hid_cntx[0].handle = cnf->handle;
            /*APPS_LOG_I("UT_HID", "(%s)CONNECT_CNF: is_init = %d, hfp = %x", __FUNCTION__, is_init, g_hfp_handle);
            if (is_init && !g_hfp_handle) {
                uint32_t handle;
                APPS_LOG_I("UT_HID", "(%s)hfp connect: hfp_channel = %d", __FUNCTION__, hfp_channel);
                status = bt_hfp_connect_ext(&handle, (bt_bd_addr_t *)&addr, hfp_channel);
                status = bt_hfp_connect(&handle, (bt_bd_addr_t *)&addr);
                APPS_LOG_I("UT_HID", "(%s)hfp connect: status = %d", __FUNCTION__, status);
            }*/
            is_init = 0;
            break;
        }
        case BT_HID_DISCONNECT_IND: {

            APPS_LOG_I("HID DISCONNECTED", 0);
            if (status == BT_STATUS_SUCCESS) {
                hid_cntx[0].handle = 0;
            }

            break;
        }
        case BT_HID_GET_REPORT_IND: {
            bt_hid_get_report_ind_t *ind = (bt_hid_get_report_ind_t *)buff;
            bt_hid_get_report_response_t report_rsp;

            memset(&report_rsp, 0x00, sizeof(bt_hid_get_report_response_t));
            report_rsp.report_len = 2;
            report_rsp.report_data[0] = 1;
            report_rsp.report_data[1] = 2;
            report_rsp.report_id = ind->report_id;
            report_rsp.type = ind->type;
            bt_hid_send_get_report_response((uint32_t)(ind->handle), &report_rsp);
            break;
        }
        case BT_HID_CONTROL_IND:
        {
            bt_hid_control_ind_t *ind = (bt_hid_control_ind_t *)buff;
            APPS_LOG_I( "HID_CONTROL : flag = %d", 1, ind->flag);
            if (ind->flag == 5) {
                //bt_ut_app_clear_link_key();
            }
            break;
        }
        default: {
            break;
        }

    }
    return BT_STATUS_SUCCESS;
}
#endif
