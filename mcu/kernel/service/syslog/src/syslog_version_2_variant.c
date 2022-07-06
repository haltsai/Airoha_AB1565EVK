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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "syslog.h"
#include "mux.h"
#include "mux_port_common.h"
#include "serial_port_assignment.h"
#include "hal_uart.h"

/*********port layer*****************/
#define PORT_SYSLOG_UNUSED(a) (void)a

#define NO_INLINE __attribute__((noinline))

/* MSGID log string start address define */
#define PORT_SYSLOG_MSG_ADDR_OFFSET 0x06000000

/* syslog length limitation */
#define PORT_SYSLOG_MAX_ONE_LOG_SIZE (256 + 64)
#define PORT_SYSLOG_MAX_ONE_PLAIN_LOG_SIZE (256)

/* module and filter define */
#define PORT_SYSLOG_MODULE_FILTER_TOTAL_NUMBER 200
#define PORT_SYSLOG_MODULE_FILTER_STATUS_SIZE (MTK_MAX_CPU_NUMBER * 2 + PORT_SYSLOG_MODULE_FILTER_TOTAL_NUMBER)
#define PORT_SYSLOG_MAX_MODULE_FILTER_STATUS_SIZE (PORT_SYSLOG_MAX_CPU_NUMBER * 2 + PORT_SYSLOG_MODULE_FILTER_TOTAL_NUMBER)

/* max drop log buffer size define */
#define MAX_DROP_BUFFER_SIZE                    38

/* syslog status define */
#define SYSLOG_INIT_NONE  0x00
#define SYSLOG_EARLY_INIT 0x01
#define SYSLOG_FULL_INIT  0x02
#define SYSLOG_EXCEPTION_INIT  0x03

/* RACE PROTOCOL */
#define RACE_TYPE_CMD_WITH_RESPONCE             0x5A
#define RACE_TYPE_RESPONCE                      0x5B
#define RACE_TYPE_CMD_WITHOUT_RESPONCE          0x5C
#define RACE_TYPE_NOTIFICATION                  0x5D

#define RACE_PROTOCAL_SYSLOG_BEGIN              (0x0F00 | 0x10)
#define RACE_PROTOCAL_STRING_LOG                (0x0F00 | 0x10)
#define RACE_PROTOCAL_MSGID_LOG                 (0x0F00 | 0x11)
#define RACE_PROTOCAL_EXCEPTION_STRING_LOG      (0x0F00 | 0x12)
#define RACE_PROTOCAL_EXCEPTION_BINARY_LOG      (0x0F00 | 0x13)
#define RACE_PROTOCAL_ACTIVE_ASSERT             (0x0F00 | 0x14)
#define RACE_PROTOCAL_QUERY_VERSION_BUILDTIME   (0x0F00 | 0x15)
#define RACE_PROTOCAL_GET_LOG_FILTER_INFO       (0x0F00 | 0x16)
#define RACE_PROTOCAL_SET_LOG_FILTER_INFO       (0x0F00 | 0x17)
#define RACE_PROTOCAL_SAVE_LOG_SETTING          (0x0F00 | 0x18)
#define RACE_PROTOCAL_QUERY_CPU_FILTER_INFO     (0x0F00 | 0x19)
#define RACE_PROTOCAL_SET_CPU_FILTER_INFO       (0x0F00 | 0x20)
#define RACE_PROTOCAL_EXCEPTION_MSGID           (0x0F00 | 0x1A)
#define RACE_PROTOCAL_TLV_LOG                   (0x0F00 | 0x00)
#define RACE_PROTOCAL_SYSLOG_END                (0x0F00 | 0x20)

/* RACE USER PROTOCOL ID */
#define RACE_PROTOCAL_GPS_BEGIN                 (0x0F91)
#define RACE_PROTOCAL_GPS_END                   (0x0F91)
#define RACE_PROTOCAL_ATCI_BEGIN                (0x0F92)
#define RACE_PROTOCAL_ATCI_END                  (0x0F92)

/* For self debug */
log_create_module(SYSLOG, PRINT_LEVEL_INFO);
#define SYSLOG_I(fmt, arg...)               LOG_I(SYSLOG, fmt, ##arg)
#define SYSLOG_E(fmt, arg...)               LOG_E(SYSLOG, fmt, ##arg)
#define SYSLOG_MSGID_I(fmt, cnt, arg...)    LOG_MSGID_I(SYSLOG, fmt, cnt, ##arg)
#define SYSLOG_MSGID_E(fmt, cnt, arg...)    LOG_MSGID_E(SYSLOG, fmt, cnt, ##arg)

/* SET MODULE FILTER END FLAG (SILM ASSERT API)*/
__attribute__((__section__(".log_filter_end"))) __attribute__((used)) static const char fiter_end_flag[] = "[MODULE_FILTER_END]";

static uint32_t port_syslog_get_current_timestamp(void)
{
    uint32_t count = 0;
    uint64_t count64 = 0;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count);
    count64 = ((uint64_t)count) * 1000 / 32768;

    return (unsigned int)count64;
}

static void port_syslog_tx_hook(uint32_t id, uint8_t *p_data, uint32_t timestamp)
{
    if ((id == RACE_PROTOCAL_STRING_LOG) || (id == RACE_PROTOCAL_MSGID_LOG)) {
        p_data[2] = timestamp & 0xFF;
        p_data[3] = (timestamp >> 8) & 0xFF;
        p_data[4] = (timestamp >> 16) & 0xFF;
        p_data[5] = (timestamp >> 24) & 0xFF;
    }
}

mux_port_t parse_syslog_port(syslog_port_type_t port_type, uint8_t port_index)
{
    if (port_type == SYSLOG_PORT_TYPE_UART) {
        return MUX_UART_BEGIN + port_index;
    } else if (port_type == SYSLOG_PORT_TYPE_USB) {
        return MUX_USB_BEGIN + port_index;
    }
    return CONFIG_SYSLOG_RUNNING_STAGE_PORT;
}

#define DROP_PROMPT_LOG ">>> log drop "
#define DROP_PROMPT_LOG_FIX_SIZE        (sizeof(DROP_PROMPT_LOG) - 1) /* substract \0 */

static uint32_t uint_to_str(uint32_t number, uint8_t *str_array)
{
    uint32_t i, value, bits;
    uint8_t byte_array[16];

    bits = 0;
    value = number;
    do {
        byte_array[bits++] = value % 10;
        value /= 10;
    } while (value);

    for (i = 0; i < bits; i++) {
        str_array[i] = byte_array[bits - i - 1] + '0';
    }

    return bits;
}

uint32_t port_syslog_drop_prompt_length(uint32_t drop_log_count)
{
    uint32_t drop_bit;

    drop_bit = 0;
    while (drop_log_count / 10) {
        drop_log_count /= 10;
        drop_bit++;
    }
    if (drop_log_count) {
        drop_bit++;
    }

#ifdef MTK_DEBUG_PLAIN_LOG_ENABLE
    return DROP_PROMPT_LOG_FIX_SIZE + drop_bit + 2; /* string + drop + /r/n */
#else
    return 12 + DROP_PROMPT_LOG_FIX_SIZE + drop_bit + 2; /* race header + string + drop + /r/n */
#endif
}

void port_syslog_drop_prompt(uint32_t timestamp, uint32_t drop_log_size, uint32_t drop_log_count, uint8_t *drop_prompt)
{
    uint32_t bits;
    uint8_t array[16];
    uint8_t *curr_drop_prompt;
    curr_drop_prompt = drop_prompt;

#ifndef MTK_DEBUG_PLAIN_LOG_ENABLE
    uint32_t i;
#endif

    /* race header */
#ifndef MTK_DEBUG_PLAIN_LOG_ENABLE
    i = 0;
    array[i++] = 0x05;
    array[i++] = RACE_TYPE_NOTIFICATION;
    array[i++] = (drop_log_size - 4) & 0xFF;
    array[i++] = ((drop_log_size - 4) >> 8) & 0xFF;
    array[i++] = RACE_PROTOCAL_STRING_LOG & 0xFF;
    array[i++] = (RACE_PROTOCAL_STRING_LOG >> 8) & 0xFF;
    array[i++] = GET_CURRENT_CPU_ID();
    array[i++] = 0x00;
    array[i++] = timestamp & 0xFF;
    array[i++] = (timestamp >> 8) & 0xFF;
    array[i++] = (timestamp >> 16) & 0xFF;
    array[i++] = (timestamp >> 24) & 0xFF;
    memcpy(curr_drop_prompt, array, i);
    curr_drop_prompt += i;
#endif

    /* drop log prefix */
    memcpy(curr_drop_prompt, DROP_PROMPT_LOG, DROP_PROMPT_LOG_FIX_SIZE);
    curr_drop_prompt += DROP_PROMPT_LOG_FIX_SIZE;

    /* drop log count */
    bits = uint_to_str(drop_log_count, array);
    memcpy(curr_drop_prompt, array, bits);
    curr_drop_prompt += bits;

    /* add \r\n */
    memcpy(curr_drop_prompt, "\r\n", 2);
}

#if !defined(MTK_DEBUG_LEVEL_PRINTF)

/* share syslog variable define */
#define PORT_SYSLOG_MAX_CPU_NUMBER 16
/*
 * Global share variable shared by all CPUs, make sure every member keep align access.
 * [cpu_module_filter_status]:
 *      cpu1_id                   1 byte
 *      cpu1_module_number        1 byte
 *      cpu2_id                   1 byte
 *      cpu2_module_number        1 byte
 *        ..                        ..
 *      cpuN_id                   1 byte
 *      cpuN_module_number        1 byte
 *      valid_cpu1_module1_log_switch   1 byte (high 4 bit)
 *      valid_cpu1_module1_print_level  1 byte (low 4 bit)
 *        ..                              ..
 *      valid_cpu1_moduleM_log_switch   1 byte (high 4 bit)
 *      valid_cpu1_moduleM_print_level  1 byte (low 4 bit)
 *      valid_cpu2_module1_log_switch   1 byte (high 4 bit)
 *      valid_cpu2_module1_print_level  1 byte (low 4 bit)
 *        ..                              ..
 *      valid_cpu2_moduleM_log_switch   1 byte (high 4 bit)
 *      valid_cpu2_moduleM_print_level  1 byte (low 4 bit)
 * [cpu_module_filter]:
 *      per-cpu needs to do address transfer to CM4 view for access by CM4 side.
 */
typedef struct {
    mux_handle_t handle;
    uint32_t init_phase;
    uint32_t sequence[2];
    uint32_t drop_count[2];
    uint8_t cpu_log_switch[PORT_SYSLOG_MAX_CPU_NUMBER];
    uint8_t cpu_log_print_level[PORT_SYSLOG_MAX_CPU_NUMBER];
    uint8_t cpu_module_filter_status[PORT_SYSLOG_MAX_MODULE_FILTER_STATUS_SIZE]; /* log filter setting read from NVDM to let per-cpu do initialization with it's log filter array. */
    void *cpu_module_filter[PORT_SYSLOG_MAX_CPU_NUMBER]; /* Init to per-cpu's log filter array when per-cpu call log_set_filter() in it's init phase. */
} syslog_share_variable_t;

#ifdef MTK_SINGLE_CPU_ENV
ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN static volatile syslog_share_variable_t syslog_share_variable;
static volatile syslog_share_variable_t *g_syslog_share_variable = &syslog_share_variable;
#else
#include "hal_resource_assignment.h"
#define PORT_SYSLOG_SHARE_VARIABLE_BEGIN HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_VAR_START
static volatile syslog_share_variable_t *g_syslog_share_variable = (volatile syslog_share_variable_t *)(PORT_SYSLOG_SHARE_VARIABLE_BEGIN); /* 344byte */
#endif /* MTK_SINGLE_CPU_ENV */

/* log filters collection */
#if defined(__GNUC__)
extern uint8_t _log_filter_start[];
extern uint8_t _log_filter_end[];

#define LOG_FILTER_START _log_filter_start
#define LOG_FILTER_END _log_filter_end
#endif /* __GNUC__ */

// #define port_syslog_memory_remap_to_primary_cpu(cpu_id, address) address
/* Memory remap across CPUs (other CPU remap to primary CPU) */
static uint32_t port_syslog_memory_remap_to_primary_cpu(uint32_t cpu_id, uint32_t address)
{
#ifdef MTK_SINGLE_CPU_ENV
    return address;
#else
    if (cpu_id == 1) {
        return hal_memview_dsp0_to_cm4(address);
    } else {
        return address;
    }
#endif
}

/* SDK version and build time */
#ifdef MTK_CPU_NUMBER_0
extern char sw_verno_str[];
#else /* MTK_CPU_NUMBER_0 */
#include "memory_attribute.h"
#define DUMMY_SDK_VERSION_STRING "DUMMY_SDK_VERSION"
static char sw_verno_str[] = DUMMY_SDK_VERSION_STRING;
ATTR_LOG_VERSION log_bin_sw_verno_str[] = DUMMY_SDK_VERSION_STRING;
#endif /* MTK_CPU_NUMBER_0 */
extern char build_date_time_str[];

#ifdef MTK_SINGLE_CPU_ENV
#define PORT_SYSLOG_SDK_VERSION_BEGIN sw_verno_str
#define PORT_SYSLOG_SDK_VERSION_LENGTH strlen(sw_verno_str)
#define PORT_SYSLOG_BUILD_TIME_BEGIN build_date_time_str
#define PORT_SYSLOG_BUILD_TIME_LENGTH strlen(build_date_time_str)
static void port_syslog_build_time_sdk_version_copy(uint32_t cpu_id)
{}
#else /* MTK_SINGLE_CPU_ENV */
#define PORT_SYSLOG_SDK_VERSION_BEGIN HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_VERSION_VAR_START
#define PORT_SYSLOG_SDK_VERSION_LENGTH 48
#define PORT_SYSLOG_BUILD_TIME_BEGIN HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_BUILD_TIME_VAR_START
#define PORT_SYSLOG_BUILD_TIME_LENGTH 48
static void port_syslog_build_time_sdk_version_copy(uint32_t cpu_id)
{
    strncpy((char *)(PORT_SYSLOG_SDK_VERSION_BEGIN + cpu_id * PORT_SYSLOG_SDK_VERSION_LENGTH), sw_verno_str, PORT_SYSLOG_SDK_VERSION_LENGTH);
    strncpy((char *)(PORT_SYSLOG_BUILD_TIME_BEGIN + cpu_id * PORT_SYSLOG_BUILD_TIME_LENGTH), build_date_time_str, PORT_SYSLOG_BUILD_TIME_LENGTH);
}
#endif /* MTK_SINGLE_CPU_ENV */

/* memory malloc and free */
#ifdef MTK_CPU_NUMBER_0

#include "serial_port_assignment.h"

static volatile bool g_primary_cpu_syslog_is_initialized = false;
static volatile bool g_exception_is_happen = false;

static void *port_syslog_malloc(uint32_t size)
{
    return malloc(size);
}

static void port_syslog_free(void *pdata)
{
    free(pdata);
}

/* syslog setting read/write to non-volatile memory */
#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#include "nvdm_id_list.h"
uint32_t port_syslog_read_setting(char *name, uint8_t *p_setting, uint32_t size)
{
    uint32_t max_size;
    nvdm_status_t status;

    if (name == NULL) {
        return false;
    }

    max_size = size;
    if (strcmp(name, "cpu_filter") == 0) {
        status = nvdm_read_data_item(NVDM_GROUP_SYSLOG, NVDM_SYSLOG_ITEM_CPU_FILTER, p_setting, &max_size);
    } else if (strcmp(name, "module_filter") == 0) {
        status = nvdm_read_data_item(NVDM_GROUP_SYSLOG, NVDM_SYSLOG_ITEM_MODULE_FILTER, p_setting, &max_size);
    } else {
        status = NVDM_STATUS_ERROR;
    }

    if (status != NVDM_STATUS_OK) {
        return false;
    }

    return true;
}

bool port_syslog_save_setting(char *name, uint8_t *p_setting, uint32_t size)
{
    nvdm_status_t status;

    if (name == NULL) {
        return false;
    }

    if (strcmp(name, "cpu_filter") == 0) {
        status = nvdm_write_data_item(NVDM_GROUP_SYSLOG, NVDM_SYSLOG_ITEM_CPU_FILTER, NVDM_DATA_ITEM_TYPE_RAW_DATA, p_setting, size);
    } else if (strcmp(name, "module_filter") == 0) {
        status = nvdm_write_data_item(NVDM_GROUP_SYSLOG, NVDM_SYSLOG_ITEM_MODULE_FILTER, NVDM_DATA_ITEM_TYPE_RAW_DATA, p_setting, size);
    } else {
        status = NVDM_STATUS_ERROR;
    }

    if (status != NVDM_STATUS_OK) {
        return false;
    }

    return true;
}

bool port_syslog_save_is_available(void)
{
    return true;
}

#else /* MTK_NVDM_ENABLE */

uint32_t port_syslog_read_setting(char *name, uint8_t *p_setting, uint32_t size)
{
    PORT_SYSLOG_UNUSED(name);
    PORT_SYSLOG_UNUSED(p_setting);
    PORT_SYSLOG_UNUSED(size);

    return false;
}

bool port_syslog_save_setting(char *name, uint8_t *p_setting, uint32_t size)
{
    PORT_SYSLOG_UNUSED(name);
    PORT_SYSLOG_UNUSED(p_setting);
    PORT_SYSLOG_UNUSED(size);

    return false;
}

bool port_syslog_save_is_available(void)
{
    return false;
}
#endif /* MTK_NVDM_ENABLE */

#include "hal_uart.h"
#include "hal_uart_internal.h"

static void pc_tool_command_handler(mux_handle_t handle, mux_event_t event, uint32_t data_len, void *user_data);

mux_port_t      g_syslog_port;
mux_handle_t    g_exception_handle;
print_level_t   g_cpu_level[2];

static mux_buffer_t *g_mux_buffer;
static uint32_t g_mux_buffer_offset, g_mux_buffer_idx, g_mux_buffer_counter, g_mux_buffer_left_data_len, g_mux_buffer_total_length;

mux_port_t query_syslog_port(void)
{
    if ((g_syslog_share_variable->init_phase == SYSLOG_INIT_NONE) || (g_primary_cpu_syslog_is_initialized == false)) {
        return MUX_UART_0;
    } else if (g_syslog_share_variable->init_phase == SYSLOG_EARLY_INIT) {
        return CONFIG_SYSLOG_INIT_STAGE_PORT;
    }

    return g_syslog_port;
}

extern UART_REGISTER_T *const g_uart_regbase[];
bool query_syslog_xoff(void)
{
    hal_uart_port_t log_port;

    log_port = query_syslog_port();

    if (log_port >= HAL_UART_MAX) {
        return false;
    }

    if ((g_uart_regbase[log_port]->MCR_UNION.MCR & UART_MCR_XOFF_STATUS_MASK) != 0) {
        return true;
    }

    return false;
}

void syslog_port_disable_flowcontrol(void)
{
    hal_uart_port_t log_port;

    log_port = query_syslog_port();

    if (log_port < HAL_UART_MAX) {
        hal_uart_disable_flowcontrol(log_port);
    }
}

static void port_syslog_early_init(void)
{
    hal_uart_config_t uart_config;

    uart_config.baudrate = CONFIG_SYSLOG_BAUDRATE;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    hal_uart_init(CONFIG_SYSLOG_INIT_STAGE_PORT, &uart_config);
#ifndef MTK_DEBUG_PLAIN_LOG_ENABLE
    hal_uart_set_software_flowcontrol(CONFIG_SYSLOG_INIT_STAGE_PORT, 0x11, 0x13, 0x77);
#endif
}

static void multi_buffer_fetch_init(mux_buffer_t buffers[], uint32_t buffers_counter)
{
    uint32_t i;
    g_mux_buffer = buffers;
    g_mux_buffer_offset = 0;
    g_mux_buffer_idx = 0;
    g_mux_buffer_counter = buffers_counter;
    g_mux_buffer_left_data_len = 0;
    for (i = 0; i < buffers_counter; i++) {
        g_mux_buffer_left_data_len += buffers[i].buf_size;
    }
    g_mux_buffer_total_length = g_mux_buffer_left_data_len;
}

static bool protocol_header_fetch(uint8_t *out_buf, uint32_t out_len)
{
    uint32_t i;

    if (g_mux_buffer_idx >= g_mux_buffer_counter) {
        return false;
    }

    if (g_mux_buffer_left_data_len < out_len) {
        return false;
    }

    for (i = 0; i < out_len; i++, g_mux_buffer_left_data_len--, g_mux_buffer_offset++) {
        if (g_mux_buffer_offset >= g_mux_buffer[g_mux_buffer_idx].buf_size) {
            g_mux_buffer_idx++;
            g_mux_buffer_offset = 0;
            if (g_mux_buffer_idx >= g_mux_buffer_counter) {
                assert(0);
            }
        }
        *(out_buf + i) = *(g_mux_buffer[g_mux_buffer_idx].p_buf + g_mux_buffer_offset);
    }
    return true;
}


static void race_tx_protocol_callback(mux_handle_t handle, const mux_buffer_t payload[], uint32_t buffers_counter, mux_buffer_t *head, mux_buffer_t *tail, void *user_data)
{
    mux_status_t status;
    const char *user_name = NULL;
    uint32_t race_id = 0, total_size = 0;
    uint32_t i;

    status = mux_query_user_name(handle, &user_name);
    if ((status == MUX_STATUS_OK) && (user_name != NULL)) {
        if (strcmp(user_name, "SYSLOG") == 0) {
            head->p_buf = NULL;
            tail->p_buf = NULL;
            head->buf_size = 0;
            tail->buf_size = 0;
            return;
        } else if (strcmp(user_name, "RACE_CMD") == 0) {
            head->p_buf = NULL;
            tail->p_buf = NULL;
            head->buf_size = 0;
            tail->buf_size = 0;
            return;
        } else if (strcmp(user_name, "HCI_CMD") == 0) {
            head->p_buf = NULL;
            tail->p_buf = NULL;
            head->buf_size = 0;
            tail->buf_size = 0;
            return;
        } else if (strcmp(user_name, "ATCI") == 0) {
            race_id = RACE_PROTOCAL_ATCI_BEGIN;
        } else if (strcmp(user_name, "PAIR_RACE") == 0) {
            race_id = RACE_PROTOCAL_GPS_BEGIN;
        }
    }

    total_size = 0;
    for (i = 0; i < buffers_counter; i++) {
        total_size += payload[i].buf_size;
    }

    /* Insert the race header here */
    head->p_buf[0] = 0x05;
    head->p_buf[1] = RACE_TYPE_NOTIFICATION;
    head->p_buf[2] = (uint8_t)((total_size + 2) & 0xFF);
    head->p_buf[3] = (uint8_t)(((total_size + 2) >> 8) & 0xFF);
    head->p_buf[4] = (uint8_t)(race_id & 0xFF);
    head->p_buf[5] = (uint8_t)((race_id >> 8) & 0xFF);
    head->buf_size = 6;

    tail->p_buf = NULL;
    tail->buf_size = 0;
}


static void race_rx_protocol_callback(mux_handle_t *handle, mux_buffer_t buffers[], uint32_t buffers_counter, uint32_t *consume_len, uint32_t *package_len, void *user_data)
{
    uint8_t race_channel, race_type;
    uint16_t race_length, race_id;
    mux_handle_t p_handle;

    race_length = 0;
    *package_len = 0;
    *consume_len = 0;

    multi_buffer_fetch_init(buffers, buffers_counter);

    if (protocol_header_fetch(&race_channel, 1) == false) {
        *package_len = 0;
        *consume_len = g_mux_buffer_total_length - g_mux_buffer_left_data_len;
        return;
    }

    if (race_channel == 0x05) { //RACE PACKAGE
        if (protocol_header_fetch(&race_type, 1) == false) {
            *package_len = 0;
            *consume_len = g_mux_buffer_total_length - g_mux_buffer_left_data_len;
            return;
        }
        if ((race_type < RACE_TYPE_CMD_WITH_RESPONCE) || (race_type > RACE_TYPE_NOTIFICATION)) {
            *package_len = 0;
            *consume_len = g_mux_buffer_total_length - g_mux_buffer_left_data_len;
            return;
        }

        if (protocol_header_fetch((uint8_t *)&race_length, 2) == false) {
            *package_len = 0;
            *consume_len = g_mux_buffer_total_length - g_mux_buffer_left_data_len;
            return;
        }

        if (protocol_header_fetch((uint8_t *)&race_id, 2) == false) {
            *package_len = 0;
            *consume_len = g_mux_buffer_total_length - g_mux_buffer_left_data_len;
            return;
        }

        *handle = 0;
        if ((race_id >= RACE_PROTOCAL_ATCI_BEGIN) && (race_id <= RACE_PROTOCAL_ATCI_END)) {
            if (mux_query_user_handle(g_syslog_port, "ATCI", &p_handle) == MUX_STATUS_OK) {
                /* ATCI race command */
                *handle = p_handle;
                /* RACE cmd format: 05 5D length(2 byte) 0f92 at+test=? */
                *package_len = race_length - 2;
                *consume_len = 6;
            } else {
                *package_len = 0;
                *consume_len = race_length + 4;
                return;
            }
        } else if ((race_id >= RACE_PROTOCAL_SYSLOG_BEGIN) && (race_id <= RACE_PROTOCAL_SYSLOG_END)) {
            if (mux_query_user_handle(g_syslog_port, "SYSLOG", &p_handle) == MUX_STATUS_OK) {
                /* SYSLOG race command */
                *handle = p_handle;
                *package_len = race_length + 6 - 2 - 4;   //pc handle think the first byte is race ID
                *consume_len = 4;
            } else {
                *package_len = 0;
                *consume_len = race_length + 4;
                return;
            }
        } else if ((race_id >= RACE_PROTOCAL_GPS_BEGIN) && (race_id <= RACE_PROTOCAL_GPS_END)) {
            if (mux_query_user_handle(g_syslog_port, "PAIR_RACE", &p_handle) == MUX_STATUS_OK) {
                /* PAIR race command */
                *handle = p_handle;
                *package_len = race_length - 2;
                *consume_len = 6;
            } else {
                *package_len = 0;
                *consume_len = race_length + 4;
                return;
            }
        } else {
            if (mux_query_user_handle(g_syslog_port, "RACE_CMD", &p_handle) == MUX_STATUS_OK) {
                /* race command */
                *handle = p_handle;
                *package_len = race_length + 6 - 2;
                *consume_len = 0;
            } else {
                *package_len = 0;
                *consume_len = race_length + 4;
                return;
            }
        }
    } else if (race_channel == 0x01) { //CMD  bt_hci_packet_cmd_t
        uint16_t reserved_data;
        uint8_t total_length;
        if (protocol_header_fetch((uint8_t *)&reserved_data, 2) == false) {
            *package_len = 0;
            *consume_len = 0;
            return;
        }
        if (protocol_header_fetch((uint8_t *)&total_length, 1) == false) {
            *package_len = 0;
            *consume_len = 0;
            return;
        }
        *handle = 0;
        if (mux_query_user_handle(g_syslog_port, "HCI_CMD", &p_handle) == MUX_STATUS_OK) {
            /* race command */
            *handle = p_handle;
            *package_len = total_length + 4;
            *consume_len = 0;
        } else {
            *package_len = 0;
            *consume_len = g_mux_buffer_total_length - g_mux_buffer_left_data_len;
        }
    } else if (race_channel == 0x02) { //ACL  bt_hci_packet_acl_t
        uint16_t reserved_data, total_length;
        if (protocol_header_fetch((uint8_t *)&reserved_data, 2) == false) {
            *package_len = 0;
            *consume_len = 0;
            return;
        }
        if (protocol_header_fetch((uint8_t *)&total_length, 2) == false) {
            *package_len = 0;
            *consume_len = 0;
            return;
        }
        *handle = 0;
        if (mux_query_user_handle(g_syslog_port, "HCI_CMD", &p_handle) == MUX_STATUS_OK) {
            /* race command */
            *handle = p_handle;
            *package_len = total_length + 5;
            *consume_len = 0;
        } else {
            *package_len = 0;
            *consume_len = g_mux_buffer_total_length - g_mux_buffer_left_data_len;
        }
    } else if (race_channel == 0x04) { //EVT  bt_hci_packet_evt_t
        uint8_t reserved_data, total_length;
        if (protocol_header_fetch((uint8_t *)&reserved_data, 1) == false) {
            *package_len = 0;
            *consume_len = 0;
            return;
        }
        if (protocol_header_fetch((uint8_t *)&total_length, 1) == false) {
            *package_len = 0;
            *consume_len = 0;
            return;
        }
        *handle = 0;
        if (mux_query_user_handle(g_syslog_port, "HCI_CMD", &p_handle) == MUX_STATUS_OK) {
            /* race command */
            *handle = p_handle;
            *package_len = total_length + 3;
            *consume_len = 0;
        } else {
            *package_len = 0;
            *consume_len = g_mux_buffer_total_length - g_mux_buffer_left_data_len;
        }
    } else {
        *package_len = 0;
        *consume_len = g_mux_buffer_total_length - g_mux_buffer_left_data_len;
        return;
    }
}

static bool syslog_one_wire_active = false;

#if defined(MTK_NVDM_ENABLE) && !defined(AG3335)
#include "nvkey.h"
#include "nvkey_id_list.h"
bool log_port_form_nvkey_init(void)
{
    uint8_t  syslog_nvkey_data[4];
    uint32_t nvkey_item_size;
    uint8_t  syslog_port, cpu_level;
    nvkey_status_t nvkey_status;
    uint8_t  onewire_syslog_nvkey_data[5];
    uint32_t onewire_nvkey_item_size;

    nvkey_item_size = sizeof(syslog_nvkey_data);
    onewire_nvkey_item_size = sizeof(onewire_syslog_nvkey_data);

    memset(syslog_nvkey_data, 0, nvkey_item_size);
    memset(onewire_syslog_nvkey_data, 0, onewire_nvkey_item_size);

    nvkey_status = nvkey_read_data(NVKEYID_LOG_SETTING, (uint8_t *)(&syslog_nvkey_data), &nvkey_item_size);
    if ((nvkey_status != NVKEY_STATUS_OK) || (nvkey_item_size != 4) || (syslog_nvkey_data[3] != 0x5A)) {
        SYSLOG_MSGID_E("Logging: Read NVKEY status=%d", 1, nvkey_status);
        return false;
    }

    SYSLOG_MSGID_I("Logging: Read NVKEY, status=%d [0]=0x%02x [1]=0x%02x [2]=0x%02x [3]=0x%02x", 5,
                nvkey_status, syslog_nvkey_data[0], syslog_nvkey_data[1], syslog_nvkey_data[2], syslog_nvkey_data[3]);

    nvkey_status = nvkey_read_data(NVKEYID_1WIRE_LOGGING_SETTING, (uint8_t *)(&onewire_syslog_nvkey_data), &onewire_nvkey_item_size);
    if ((nvkey_status != NVKEY_STATUS_OK) || (onewire_nvkey_item_size != 5) || (onewire_syslog_nvkey_data[3] != 0x5A)) {
        SYSLOG_MSGID_I("Logging 1Wire: Read NVKEY status=%d", 1, nvkey_status);
    }

    SYSLOG_MSGID_I("Logging 1Wire: Read NVKEY, status=%d [0]=0x%02x [1]=0x%02x [2]=0x%02x [3]=0x%02x [4]=0x%02x", 6,
                nvkey_status, onewire_syslog_nvkey_data[0], onewire_syslog_nvkey_data[1]
                , onewire_syslog_nvkey_data[2], onewire_syslog_nvkey_data[3]
                , onewire_syslog_nvkey_data[4]);

#if 1
    if (onewire_syslog_nvkey_data[4] != 0) {
        syslog_one_wire_active = true;
        for (int i = 0; i < 4; i++) {
            syslog_nvkey_data[i] = onewire_syslog_nvkey_data[i];
        }
        hal_pinmux_set_function(HAL_GPIO_5, HAL_GPIO_5_GPIO5);
        hal_gpio_set_direction(HAL_GPIO_5 , HAL_GPIO_DIRECTION_INPUT);
        hal_pinmux_set_function(HAL_GPIO_4, HAL_GPIO_4_UART1_TXD);
#if defined(AB1568)
        hal_pinmux_set_function(HAL_GPIO_6 , HAL_GPIO_6_GPIO6);
        hal_gpio_set_direction(HAL_GPIO_6 , HAL_GPIO_DIRECTION_OUTPUT);
        hal_gpio_set_output(HAL_GPIO_6, HAL_GPIO_DATA_LOW);
#endif
    }
#endif
    syslog_port = syslog_nvkey_data[0] & 0x7F;
    cpu_level   = syslog_nvkey_data[1] & 0x0F;

    if ((syslog_nvkey_data[0] & 0x80) != 0) {
        ;
    }

    g_cpu_level[0] = (cpu_level >> 0) & 0x03;
    g_cpu_level[1] = (cpu_level >> 2) & 0x03;

    switch (syslog_port) {
        case 0x00: {
            g_syslog_port = MUX_UART_0;
            SYSLOG_MSGID_I("Logging: Read NVKEY Logging port: UART_0", 0);
        }
        break;

        case 0x01: {
            g_syslog_port = MUX_UART_1;
            SYSLOG_MSGID_I("Logging: Read NVKEY Logging port: UART_1", 0);
        }
        break;

        case 0x02: {
            g_syslog_port = MUX_UART_2;
            SYSLOG_MSGID_I("Logging: Read NVKEY Logging port: UART_2", 0);
        }
        break;

        case 0x03: {
            g_syslog_port = MUX_USB_COM_1;
            SYSLOG_MSGID_I("Logging: Read NVKEY Logging port: USB_1", 0);
        }
        break;

        case 0x04: {
            g_syslog_port = MUX_USB_COM_2;
            SYSLOG_MSGID_I("Logging: Read NVKEY Logging port: USB_2", 0);
        }
        break;

        case 0x05: {
            g_syslog_port = MUX_AIRAPP_0;
            SYSLOG_MSGID_I("Logging: Read NVKEY Logging port: AIR_SPP", 0);
        }
        break;

        default:
            break;
    }
    return true;
}
#endif /* MTK_NVDM_ENABLE && !AG3335 */

#ifdef AIR_SYSLOG_BUFFER_EXPAND_ENABLE
#if defined AIR_SYSLOG_BUFFER_SIZE_16K_ENABLE
#define AIR_SYSLOG_TX_BUFFER_SIZE       (16 * 1024)
#define AIR_SYSLOG_RX_BUFFER_SIZE       (1024)
#elif defined AIR_SYSLOG_BUFFER_SIZE_12K_ENABLE
#define AIR_SYSLOG_TX_BUFFER_SIZE       (12 * 1024)
#define AIR_SYSLOG_RX_BUFFER_SIZE       (1024)
#else
#define AIR_SYSLOG_TX_BUFFER_SIZE       (8 * 1024)
#define AIR_SYSLOG_RX_BUFFER_SIZE       (1024)
#endif
#else /* AIR_SYSLOG_BUFFER_EXPAND_ENABLE */
#define AIR_SYSLOG_TX_BUFFER_SIZE       (8 * 1024)
#define AIR_SYSLOG_RX_BUFFER_SIZE       (1024)
#endif /* !AIR_SYSLOG_BUFFER_EXPAND_ENABLE */

bool log_port_init(void)
{
    bool nvkey_status = false;
    mux_status_t status;
    mux_port_t syslog_port;
    mux_port_setting_t syslog_setting;
    mux_protocol_t protocol_callback;
    mux_port_buffer_t query_port_buffer;
    /* default syslog port setting */
    syslog_setting.tx_buffer_size = AIR_SYSLOG_TX_BUFFER_SIZE;
    syslog_setting.rx_buffer_size = AIR_SYSLOG_RX_BUFFER_SIZE;
    syslog_setting.dev_setting.uart.uart_config.baudrate    = CONFIG_SYSLOG_BAUDRATE;
    syslog_setting.dev_setting.uart.uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    syslog_setting.dev_setting.uart.uart_config.stop_bit    = HAL_UART_STOP_BIT_1;
    syslog_setting.dev_setting.uart.uart_config.parity      = HAL_UART_PARITY_NONE;
    syslog_setting.dev_setting.uart.flowcontrol_type        = MUX_UART_SW_FLOWCONTROL;

#if defined(MTK_NVDM_ENABLE) && !defined(AG3335)
    nvkey_status = log_port_form_nvkey_init();
#endif /* MTK_NVDM_ENABLE && !AG3335 */

    if ((nvkey_status == true) && (g_syslog_port == MUX_AIRAPP_0)) {
        SYSLOG_MSGID_I("Logging: nvkey = MUX_AIRAPP_0", 0);
        status = mux_query_port_numbers_from_nvdm("SYSLOG", (mux_port_buffer_t *)&query_port_buffer);
        if ((status == MUX_STATUS_OK) && (query_port_buffer.count == 1)) {
            SYSLOG_MSGID_I("Logging: query syslog port from nvdm ok, port=%d", 1, query_port_buffer.buf[0]);
            if (query_port_buffer.buf[0] != MUX_AIRAPP_0) {
                mux_close_delete_from_nvdm(query_port_buffer.buf[0], "SYSLOG");
            }
        }
        syslog_port = g_syslog_port;
    } else {
        query_port_buffer.count = 0;
        status = mux_query_user_port_numbers_from_nvdm("SYSLOG", (uint32_t *) & (query_port_buffer.count));
        SYSLOG_MSGID_I("Logging: query syslog port from nvdm, status:%d quantity:%d", 2, status, query_port_buffer.count);
        if ((status == MUX_STATUS_OK) && (query_port_buffer.count > 1)) {
            SYSLOG_MSGID_I("Logging: syslog port must <= 1", 0);
            assert(0); // syslog port must <= 1
        }
        status = mux_query_port_numbers_from_nvdm("SYSLOG", (mux_port_buffer_t *)&query_port_buffer);
        if ((status == MUX_STATUS_OK) && (query_port_buffer.count == 1)) {
            SYSLOG_MSGID_I("Logging: query syslog port from nvdm ok, port=%d", 1, query_port_buffer.buf[0]);
            if (syslog_one_wire_active == true) {
                if (query_port_buffer.buf[0] != g_syslog_port) {
                    mux_close_delete_from_nvdm(query_port_buffer.buf[0], "SYSLOG");
                }
                syslog_port = g_syslog_port;
                SYSLOG_MSGID_I("Logging: one-wire is avtive, use 1-wire port=%d", 1, syslog_port);
            } else {
                syslog_port = query_port_buffer.buf[0];
                SYSLOG_MSGID_I("Logging: one-wire is invalid, use nvdm port=%d", 1, syslog_port);
            }
        } else {
            syslog_port = g_syslog_port;
            SYSLOG_MSGID_I("Logging: query syslog port from nvdm fail, use default port=%d", 1, syslog_port);
        }
    }
    /*g_syslog_port might be used before/during logging init,so it need update on time*/
    g_syslog_port = syslog_port;

    if (syslog_port == MUX_AIRAPP_0) {
        protocol_callback.rx_protocol_callback = NULL;
        protocol_callback.tx_protocol_callback = NULL;
        protocol_callback.user_data = NULL;
    } else {
        protocol_callback.rx_protocol_callback = race_rx_protocol_callback; //race_rx_no_packed_callback;
        protocol_callback.tx_protocol_callback = race_tx_protocol_callback; //race_tx_no_packed_callback;
        protocol_callback.user_data = NULL;
    }

    SYSLOG_MSGID_I("Logging: begin mux_init syslog port:%d tx_buf_size:%d rx_buf_size:%d", 3,
                syslog_port, syslog_setting.tx_buffer_size, syslog_setting.rx_buffer_size);


    if (syslog_port < MUX_UART_END) {
        uart_disable_irq(syslog_port);
    }
    status = mux_init(syslog_port, &syslog_setting, &protocol_callback);

    if ((syslog_port != MUX_AIRAPP_0) && (syslog_one_wire_active == false)) {
        mux_open_save_to_nvdm(syslog_port, "SYSLOG");
    }

    g_syslog_port = syslog_port;

    return true;
}


static void port_syslog_post_init(void)
{
    uint32_t port_num;
    mux_status_t status;

#if 0
    mux_port_buffer_t query_port_buffer;

    query_port_buffer.count = 0;

    status = mux_query_port_numbers_from_nvdm("SYSLOG", (mux_port_buffer_t *)&query_port_buffer);
    if ((query_port_buffer.count > 1) || (status != MUX_STATUS_OK)) {
        assert(0);  //logging port number must be one
    }

    if (query_port_buffer.count != 0) {
        port_num = query_port_buffer.buf[0];
    } else {
        port_num = g_syslog_port;
    }
#endif

    port_num = g_syslog_port;

    if (port_num == MUX_AIRAPP_0) {
        status = mux_open(port_num, "SYSLOG", (mux_handle_t *)&g_syslog_share_variable->handle, NULL, NULL);
        if (status != MUX_STATUS_OK) {
            assert(0);
        }
        g_exception_handle = 0x55AAFF00;
    } else {
        status = mux_open(port_num, "SYSLOG", (mux_handle_t *)&g_syslog_share_variable->handle, pc_tool_command_handler, NULL);
        if (status != MUX_STATUS_OK) {
            assert(0);
        }
        g_exception_handle = g_syslog_share_variable->handle;
    }
}

#endif /* MTK_CPU_NUMBER_0 */

static uint32_t port_syslog_send(uint32_t id, bool drop_flag, const uint8_t **p_data, uint32_t *p_len)
{
    mux_status_t status;
    mux_buffer_t mux_buffers[6];
    uint8_t race_header[6];
    uint32_t i, total_size;
    uint32_t counter;
    uint32_t cpu_id;

    total_size = 0;
    for (i = 0; p_data[i] != NULL; i++) {
        total_size += p_len[i];
        mux_buffers[i + 1].p_buf = (uint8_t *)p_data[i];
        mux_buffers[i + 1].buf_size = p_len[i];
    }

    mux_buffers[i + 1].p_buf = NULL;
    counter = i + 1;

    /* Insert the race header here */
    race_header[0] = 0x05;
    race_header[1] = RACE_TYPE_NOTIFICATION;
    race_header[2] = (uint8_t)((total_size + 2) & 0xFF);
    race_header[3] = (uint8_t)(((total_size + 2) >> 8) & 0xFF);
    race_header[4] = (uint8_t)(id & 0xFF);
    race_header[5] = (uint8_t)((id >> 8) & 0xFF);

    mux_buffers[0].p_buf = race_header;
    mux_buffers[0].buf_size = 6;

#ifdef MTK_CPU_NUMBER_0
    if (g_syslog_share_variable->init_phase == SYSLOG_EARLY_INIT) {
#ifndef MTK_DEBUG_PLAIN_LOG_ENABLE
        hal_uart_send_polling(CONFIG_SYSLOG_INIT_STAGE_PORT, race_header, 6);
#endif
        /* send the payload */
        for (i = 0; p_data[i] != NULL; i++) {
            hal_uart_send_polling(CONFIG_SYSLOG_INIT_STAGE_PORT, p_data[i], p_len[i]);
        }
        return total_size;
    } else if (g_syslog_share_variable->init_phase == SYSLOG_EXCEPTION_INIT) {
        mux_exception_send(g_syslog_share_variable->handle, mux_buffers, counter);
        return total_size;
    }
#endif

    cpu_id = GET_CURRENT_CPU_ID();

#if 1 /* This feature have risk, need review */
    if (g_syslog_share_variable->drop_count[cpu_id] != 0) {
        uint32_t tx_size = 0, tx_free_size = 0;
        uint32_t drop_package_len = 0;
        uint8_t  drop_package_buffer[MAX_DROP_BUFFER_SIZE];
        mux_buffer_t drop_buf[1];
        drop_package_len = port_syslog_drop_prompt_length(g_syslog_share_variable->drop_count[cpu_id]);
        tx_size = mux_buffers[0].buf_size + total_size + drop_package_len;
        mux_control(g_syslog_port, MUX_CMD_GET_TX_AVAIL, (mux_ctrl_para_t *)&tx_free_size);
        if (tx_free_size >= tx_size) {
            memset(drop_package_buffer, 0, sizeof(drop_package_buffer));
            port_syslog_drop_prompt(port_syslog_get_current_timestamp(), drop_package_len, g_syslog_share_variable->drop_count[cpu_id], (uint8_t *)drop_package_buffer);
            drop_buf[0].p_buf = drop_package_buffer;
            drop_buf[0].buf_size = drop_package_len;
            mux_tx(g_syslog_share_variable->handle, drop_buf, 1, &tx_size);
            g_syslog_share_variable->drop_count[cpu_id] = 0;
        } else {
            g_syslog_share_variable->drop_count[cpu_id] += 1;
            /* The last piece of log is a DSP update and the tx buffer is full at this time,
                the log will no longer be printed, so this operation push data out if need */
            if ((g_syslog_port == MUX_USB_COM_1) || (g_syslog_port == MUX_USB_COM_2)) {
                mux_control(g_syslog_share_variable->handle, MUX_CMD_CLEAN, NULL);
            }
            return 0;
        }
    }
#endif

    do {
        status = mux_tx(g_syslog_share_variable->handle, mux_buffers, counter, &total_size);
    } while ((drop_flag == false) && (total_size == 0));

    if (status == MUX_STATUS_OK) {
        total_size = total_size - mux_buffers[0].buf_size;
    } else {
        g_syslog_share_variable->drop_count[cpu_id] += 1;
        total_size = 0;
    }
    return total_size;
}

#define MAX_PC_TX_DATA_SIZE                     256
#define MAX_PC_RX_DATA_SIZE                     128

typedef struct {
    uint8_t cpu_id;
    uint8_t module_number;
} module_filter_info_t;

#define SYSLOG_FILTER_VALID_MARK 0x80

NO_INLINE volatile uint8_t *find_start_of_cpu_log_filters(uint32_t cpu_id)
{
    uint32_t offset, i;
    module_filter_info_t *p_filter_info;

    offset = 2 * MTK_MAX_CPU_NUMBER;
    p_filter_info = (module_filter_info_t *)(g_syslog_share_variable->cpu_module_filter_status);
    for (i = 0; i < MTK_MAX_CPU_NUMBER; i++) {
        if ((p_filter_info[i].cpu_id & SYSLOG_FILTER_VALID_MARK) == 0) {
            break;
        }
        if ((p_filter_info[i].cpu_id & (~SYSLOG_FILTER_VALID_MARK)) == cpu_id) {
            break;
        }
        offset += p_filter_info[i].module_number;
    }

    return &(g_syslog_share_variable->cpu_module_filter_status[offset]);
}

NO_INLINE volatile void *qurey_cpu_module_filter_pointer(uint32_t cpu_id)
{
    return (log_control_block_t *)(g_syslog_share_variable->cpu_module_filter[cpu_id]);
}

NO_INLINE bool log_set_all_module_filter_off(log_switch_t log_switch)
{
    volatile uint8_t *p_filters_status;
    log_control_block_t *filters;
    module_filter_info_t *p_filter_info;
    uint32_t module_num, i, cpu_id;

    //only handle CM4
    if (log_switch > DEBUG_LOG_OFF) {
        return false;
    }

    cpu_id = GET_CURRENT_CPU_ID();

    for (cpu_id = 0; cpu_id < MTK_MAX_CPU_NUMBER; cpu_id++) {
        filters = (log_control_block_t *)g_syslog_share_variable->cpu_module_filter[cpu_id];
        if (filters == NULL) {
            return false;
        }
        p_filter_info = (module_filter_info_t *)g_syslog_share_variable->cpu_module_filter_status;
        if ((p_filter_info[cpu_id].cpu_id & SYSLOG_FILTER_VALID_MARK) == 0) {
            return false;
        }

        module_num = p_filter_info[cpu_id].module_number;
        p_filters_status = find_start_of_cpu_log_filters(cpu_id);
        for (i = 0; i < module_num; i++) {
            p_filters_status[i] = ((log_switch << 4) | (p_filters_status[i]  & 0x0F));
        }
    }

    return true;
}
#ifdef MTK_CPU_NUMBER_0

#include "exception_handler.h"

#define CMD_CPU_NAME_MAX_LENGTRH        16
#define CMD_SDK_VERSION_MAX_LENGTRH     48
#define CMD_BUILD_TIME_MAX_LENGTRH      48

#define PC_COMMAND_ERROR_CODE_OK        0x00
#define PC_COMMAND_ERROR_CODE_FAIL      0x01

static volatile uint32_t g_curr_filter_save_type;
static volatile bool g_filter_save_is_need = false;

static uint32_t port_syslog_receive(uint8_t *p_buf, uint32_t size)
{
    mux_buffer_t buffer;
    uint32_t len;
    buffer.p_buf = p_buf;
    buffer.buf_size = size;
    mux_rx(g_syslog_share_variable->handle, &buffer, &len);
    return len;
}

static void filter_config_load(void)
{
    uint32_t i, size;
    char *syslog_filter_buf;

    syslog_filter_buf = (char *)port_syslog_malloc(PORT_SYSLOG_MODULE_FILTER_STATUS_SIZE);
    assert(syslog_filter_buf != NULL);

    /* this part need each CPU to parse itself */
    size = PORT_SYSLOG_MODULE_FILTER_STATUS_SIZE;
    if (port_syslog_read_setting("module_filter", (uint8_t *)syslog_filter_buf, size) == true) {
        memcpy((uint8_t *)g_syslog_share_variable->cpu_module_filter_status, syslog_filter_buf, size);
    }

    size = 2 * MTK_MAX_CPU_NUMBER;
    if (port_syslog_read_setting("cpu_filter", (uint8_t *)syslog_filter_buf, size) == true) {
        for (i = 0; i < MTK_MAX_CPU_NUMBER; i++) {
            g_syslog_share_variable->cpu_log_switch[i] = syslog_filter_buf[i * 2];
            g_syslog_share_variable->cpu_log_print_level[i] = syslog_filter_buf[i * 2 + 1];
        }
    } else {
        for (i = 0; i < MTK_MAX_CPU_NUMBER; i++) {
            g_syslog_share_variable->cpu_log_switch[i] = DEBUG_LOG_ON;
            g_syslog_share_variable->cpu_log_print_level[i] = PRINT_LEVEL_DEBUG;
        }
    }

    port_syslog_free(syslog_filter_buf);
}

static bool filter_cpu_config_save(void)
{
    uint32_t i;
    char syslog_filter_buf[2 * MTK_MAX_CPU_NUMBER];

    for (i = 0; i < MTK_MAX_CPU_NUMBER; i++) {
        syslog_filter_buf[i * 2] = g_syslog_share_variable->cpu_log_switch[i];
        syslog_filter_buf[i * 2 + 1] = g_syslog_share_variable->cpu_log_print_level[i];
    }

    return port_syslog_save_setting("cpu_filter", (uint8_t *)syslog_filter_buf, sizeof(syslog_filter_buf));
}

static bool filter_module_config_save(void)
{
    return port_syslog_save_setting("module_filter", (uint8_t *)g_syslog_share_variable->cpu_module_filter_status, PORT_SYSLOG_MODULE_FILTER_STATUS_SIZE);
}

/* Below APIs is used also by AT command */
bool log_get_cpu_filter_number(uint32_t *p_cpu_number)
{
    if (p_cpu_number == NULL) {
        return false;
    }

    *p_cpu_number = MTK_MAX_CPU_NUMBER;

    return true;
}

bool log_get_cpu_filter_config(uint32_t cpu_id, log_switch_t *p_log_switch, print_level_t *p_print_level)
{
    if ((cpu_id >= MTK_MAX_CPU_NUMBER) || (p_log_switch == NULL) || (p_print_level == NULL)) {
        return false;
    }

    *p_log_switch = g_syslog_share_variable->cpu_log_switch[cpu_id];
    *p_print_level = g_syslog_share_variable->cpu_log_print_level[cpu_id];

    return true;
}

bool log_get_module_filter_number(uint32_t cpu_id, uint32_t *p_module_number)
{
    log_control_block_t *filters;
    module_filter_info_t *p_filter_info;

    if ((cpu_id >= MTK_MAX_CPU_NUMBER) || (p_module_number == NULL)) {
        return false;
    }

    /* This filter has been remap already in log_set_filter() */
    filters = (log_control_block_t *)(g_syslog_share_variable->cpu_module_filter[cpu_id]);
    if (filters == NULL) {
        return false;
    }
    p_filter_info = (module_filter_info_t *)g_syslog_share_variable->cpu_module_filter_status;
    if ((p_filter_info[cpu_id].cpu_id & SYSLOG_FILTER_VALID_MARK) == 0) {
        return false;
    }
    *p_module_number = p_filter_info[cpu_id].module_number;

    return true;
}

bool log_get_module_filter_config(uint32_t cpu_id, uint32_t module_id, const char **p_module_name, log_switch_t *p_log_switch, print_level_t *p_print_level)
{
    volatile uint8_t *p_filters_status;
    log_control_block_t *filters;
    module_filter_info_t *p_filter_info;

    if ((cpu_id >= MTK_MAX_CPU_NUMBER) || (p_log_switch == NULL) || (p_print_level == NULL)) {
        return false;
    }

    p_filters_status = find_start_of_cpu_log_filters(cpu_id);
    filters = (log_control_block_t *)(g_syslog_share_variable->cpu_module_filter[cpu_id]);
    if (filters == NULL) {
        return false;
    }
    p_filter_info = (module_filter_info_t *)g_syslog_share_variable->cpu_module_filter_status;
    if ((p_filter_info[cpu_id].cpu_id & SYSLOG_FILTER_VALID_MARK) == 0) {
        return false;
    }

    if (module_id >= p_filter_info[cpu_id].module_number) {
        return false;
    }

    if (p_module_name != NULL) {
        *p_module_name = filters[module_id].module_name;
    }
    *p_log_switch = p_filters_status[module_id] >> 4;
    *p_print_level = p_filters_status[module_id] & 0x0F;

    return true;
}

bool log_set_cpu_filter_config(uint32_t cpu_id, log_switch_t log_switch, print_level_t print_level)
{
    if ((cpu_id >= MTK_MAX_CPU_NUMBER) ||
        (log_switch > DEBUG_LOG_OFF) || (print_level > PRINT_LEVEL_ERROR)) {
        return false;
    }

    g_syslog_share_variable->cpu_log_switch[cpu_id] = log_switch;
    g_syslog_share_variable->cpu_log_print_level[cpu_id] = print_level;

    return true;
}

bool log_set_module_filter_config(uint32_t cpu_id, char *module_name, log_switch_t log_switch, print_level_t print_level)
{
    uint32_t module_id, size_1, size_2;
    volatile uint8_t *p_filters_status;
    log_control_block_t *filters;
    module_filter_info_t *p_filter_info;

    if ((cpu_id >= MTK_MAX_CPU_NUMBER) ||
        (module_name == NULL) ||
        (log_switch > DEBUG_LOG_OFF) || (print_level > PRINT_LEVEL_ERROR)) {
        return false;
    }

    filters = (log_control_block_t *)g_syslog_share_variable->cpu_module_filter[cpu_id];
    if (filters == NULL) {
        return false;
    }
    p_filter_info = (module_filter_info_t *)g_syslog_share_variable->cpu_module_filter_status;
    if ((p_filter_info[cpu_id].cpu_id & SYSLOG_FILTER_VALID_MARK) == 0) {
        return false;
    }

    size_1 = strlen(module_name);
    for (module_id = 0; module_id < p_filter_info[cpu_id].module_number; module_id++) {
        size_2 = strlen(filters[module_id].module_name);
        if (size_1 != size_2) {
            continue;
        }
        if (!strcmp(filters[module_id].module_name, module_name)) {
            break;
        }
    }
    if (module_id >= p_filter_info[cpu_id].module_number) {
        return false;
    }

    p_filters_status = find_start_of_cpu_log_filters(cpu_id);
    p_filters_status[module_id] = (log_switch << 4) | print_level;

    return true;
}

void log_trigger_save_filter(void)
{
    g_curr_filter_save_type = 0xFF;
    g_filter_save_is_need = true;
}

bool log_query_save_filter(void)
{
    return !g_filter_save_is_need;
}

/* Below API should be called by idle or deamon task */
void log_save_filter(void)
{
    uint8_t status = 0x0;
    uint8_t *p_data[2];
    uint8_t respond_buffer[16];
    uint32_t total_size;

    if (g_filter_save_is_need == false) {
        return;
    }

    /* Save log filter setting to NVDM */
    if (g_curr_filter_save_type & 0x0F) {
        if (filter_cpu_config_save() != true) {
            status |= PC_COMMAND_ERROR_CODE_FAIL;
            SYSLOG_MSGID_E("filter_cpu_config_save fail", 0);
        }
    }

    if (g_curr_filter_save_type & 0xF0) {
        if (filter_module_config_save() != true) {
            status |= PC_COMMAND_ERROR_CODE_FAIL;
            SYSLOG_MSGID_E("filter_module_config_save fail", 0);
        }
    }

    respond_buffer[0] = status;
    total_size = 1;

    p_data[0] = respond_buffer;
    p_data[1] = NULL;
    port_syslog_send(RACE_PROTOCAL_SAVE_LOG_SETTING, false, (const uint8_t **)p_data, &total_size);

    /* Reset the save flag. */
    g_filter_save_is_need = false;
}
#ifdef SYSTEM_DAEMON_TASK_ENABLE
#include "system_daemon.h"
#endif
volatile bool g_log_resp_push = false;
static void pc_tool_command_handler(mux_handle_t handle, mux_event_t event, uint32_t data_len, void *user_data)
{
    uint32_t i, filter_number;
    uint16_t race_id;
    uint8_t cpu_id, filter_info, error_code;
    uint8_t *p_data[2];
    uint8_t *pc_command_response_buf;
    int32_t pc_command_response_len[2];
    uint8_t rx_data_buf[MAX_PC_RX_DATA_SIZE];
    uint8_t *pc_received_data_buf;
    bool rx_mallco_flag = false;
    log_switch_t log_switch;
    print_level_t print_level;
    log_control_block_t *filters;

    /* syslog handle pc tool command only support MUX_EVENT_READY_TO_READ */
    if (event != MUX_EVENT_READY_TO_READ) {
        return;
    }
    /* for too long pc command */
    if (data_len > MAX_PC_RX_DATA_SIZE) {
        /* error pc command data length too long */
        LOG_MSGID_E(common, "pc tool command data length too long", 1, data_len);
        pc_received_data_buf = port_syslog_malloc(data_len);
        if (pc_received_data_buf == NULL) {
            assert(0);
        }
        rx_mallco_flag = true;
        memset(pc_received_data_buf, 0, data_len);
    } else {
        /* normal data length use stack */
        pc_received_data_buf = &rx_data_buf[0];
        memset(pc_received_data_buf, 0, MAX_PC_RX_DATA_SIZE);
    }
    /* read back all data from mux rx buffer */
    if (port_syslog_receive(pc_received_data_buf, data_len) != data_len) {
        if (rx_mallco_flag == true) {
            port_syslog_free(pc_received_data_buf);
        }
        return;
    }
    /* malloc response data buffer */
    pc_command_response_buf = port_syslog_malloc(MAX_PC_TX_DATA_SIZE);
    if (pc_command_response_buf == NULL) {
        assert(0);
    }
    /* clean heap buffer for response */
    memset(pc_command_response_buf, 0, MAX_PC_TX_DATA_SIZE);

    /* parsing parameters */
    cpu_id  = pc_received_data_buf[2];
    race_id = (pc_received_data_buf[1] << 8) | pc_received_data_buf[0];

    SYSLOG_MSGID_I("Logging:  PC logging tool command id = 0x%04x", 1, race_id);

    switch (race_id) {
        case RACE_PROTOCAL_ACTIVE_ASSERT: { //0x0F14
            platform_assert("Asserted by PC logging tool", __FILE__, __LINE__);
        }
        break;

        case RACE_PROTOCAL_QUERY_VERSION_BUILDTIME: { //0x0F15
            if (cpu_id >= MTK_MAX_CPU_NUMBER) {
                goto error;
            }
            pc_command_response_buf[0] = cpu_id;
            strncpy((char *)&pc_command_response_buf[1 + CMD_CPU_NAME_MAX_LENGTRH], (char *)(PORT_SYSLOG_SDK_VERSION_BEGIN + cpu_id * PORT_SYSLOG_SDK_VERSION_LENGTH), PORT_SYSLOG_SDK_VERSION_LENGTH);
            strncpy((char *)&pc_command_response_buf[1 + CMD_CPU_NAME_MAX_LENGTRH + CMD_SDK_VERSION_MAX_LENGTRH], (char *)(PORT_SYSLOG_BUILD_TIME_BEGIN + cpu_id * PORT_SYSLOG_BUILD_TIME_LENGTH), PORT_SYSLOG_BUILD_TIME_LENGTH);
            pc_command_response_len[0] = 1 + CMD_CPU_NAME_MAX_LENGTRH + CMD_SDK_VERSION_MAX_LENGTRH + CMD_BUILD_TIME_MAX_LENGTRH;
        }
        break;

        case RACE_PROTOCAL_GET_LOG_FILTER_INFO: { //0x0F16
            if (cpu_id >= MTK_MAX_CPU_NUMBER) {
                goto error;
            }
            pc_command_response_buf[0] = cpu_id;
            log_get_module_filter_number(cpu_id, &filter_number);
            if (filter_number == 0) {
                goto error;
            }
            for (i = 0; i < filter_number / 2; i++) {
                log_get_module_filter_config(cpu_id, i * 2, NULL, &log_switch, &print_level);
                pc_command_response_buf[i + 1] = (log_switch << 3) | print_level;
                log_get_module_filter_config(cpu_id, i * 2 + 1, NULL, &log_switch, &print_level);
                pc_command_response_buf[i + 1] |= ((log_switch << 3) | print_level) << 4;
            }
            if (filter_number % 2) {
                log_get_module_filter_config(cpu_id, i * 2, NULL, &log_switch, &print_level);
                pc_command_response_buf[i + 1] = (log_switch << 3) | print_level;
            }
            pc_command_response_len[0] = 1 + filter_number / 2 + filter_number % 2;
        }
        break;

        case RACE_PROTOCAL_SET_LOG_FILTER_INFO: { //0x0F17
            if (cpu_id >= MTK_MAX_CPU_NUMBER) {
                goto error;
            }
            error_code = PC_COMMAND_ERROR_CODE_OK;
            log_get_module_filter_number(cpu_id, &filter_number);
            if (filter_number == 0) {
                goto error;
            }
            filters = (log_control_block_t *)(g_syslog_share_variable->cpu_module_filter[cpu_id]);
            for (i = 0; i < filter_number / 2 ; i++) {
                filter_info = pc_received_data_buf[3 + i];
                log_switch = (filter_info >> 3) & 0x01;
                print_level = filter_info & 0x03;
                log_set_module_filter_config(cpu_id, (char *)(filters[i * 2].module_name), log_switch, print_level);
                log_switch = (filter_info >> 7) & 0x01;
                print_level = (filter_info >> 4) & 0x03;
                log_set_module_filter_config(cpu_id, (char *)(filters[i * 2 + 1].module_name), log_switch, print_level);
            }
            if (filter_number % 2) {
                filter_info = pc_received_data_buf[3 + filter_number / 2];
                log_switch = (filter_info >> 3) & 0x01;
                print_level = filter_info & 0x03;
                log_set_module_filter_config(cpu_id, (char *)(filters[i * 2].module_name), log_switch, print_level);
            }
            pc_command_response_buf[0] = error_code;
            pc_command_response_len[0] = 1;
        }
        break;

        case RACE_PROTOCAL_QUERY_CPU_FILTER_INFO: { //0x0F19
            if (cpu_id >= MTK_MAX_CPU_NUMBER) {
                goto error;
            }
            if (log_get_cpu_filter_config(cpu_id, &log_switch, &print_level) == true) {
                pc_command_response_buf[0] = PC_COMMAND_ERROR_CODE_OK;
                pc_command_response_buf[1] = cpu_id;
                pc_command_response_buf[2] = (log_switch << 3) | print_level;
                pc_command_response_len[0] = 3;
            } else {
                pc_command_response_buf[0] = PC_COMMAND_ERROR_CODE_FAIL;
                pc_command_response_len[0] = 1;
            }
        }
        break;

        case RACE_PROTOCAL_SET_CPU_FILTER_INFO: { //0x0F20
            if (cpu_id >= MTK_MAX_CPU_NUMBER) {
                goto error;
            }
            filter_info = pc_received_data_buf[3];
            log_switch = (filter_info >> 3) & 0x01;
            print_level = filter_info & 0x03;
            if (log_set_cpu_filter_config(cpu_id, log_switch, print_level) == true) {
                pc_command_response_buf[0] = PC_COMMAND_ERROR_CODE_OK;
                pc_command_response_len[0] = 1;
            } else {
                pc_command_response_buf[0] = PC_COMMAND_ERROR_CODE_FAIL;
                pc_command_response_len[0] = 1;
            }
        }
        break;

        case RACE_PROTOCAL_SAVE_LOG_SETTING: {
            log_trigger_save_filter();
            goto error;
        }
        break;

        default:
            goto error;
            break;
    }

    p_data[0] = pc_command_response_buf;
    p_data[1] = NULL;
    pc_command_response_len[1] = 0;
    port_syslog_send(race_id, false, (const uint8_t **)p_data, (uint32_t *)&pc_command_response_len);
    if (!g_log_resp_push) {
        g_log_resp_push = true;
#ifdef SYSTEM_DAEMON_TASK_ENABLE
        system_daemon_send_message(SYSTEM_DAEMON_ID_LOGGING_TO_FLASH, NULL);
#endif
    }
error:
    if (rx_mallco_flag == true) {
        port_syslog_free(pc_received_data_buf);
    }
    port_syslog_free(pc_command_response_buf);
}

static volatile uint32_t g_exception_string_log_count = 0;
static volatile uint32_t g_exception_binary_log_count = 0;
void exception_syslog_callback(void)
{
    /* port handle init before log full init done */
    if ((g_syslog_share_variable->init_phase == SYSLOG_INIT_NONE) || (g_primary_cpu_syslog_is_initialized == false)) {
        g_exception_handle = 0x55AAFF00;
    } else if (g_syslog_share_variable->init_phase == SYSLOG_EARLY_INIT) {
        g_exception_handle = 0x55AAFF00 | CONFIG_SYSLOG_INIT_STAGE_PORT;
    }

    /* Reset status of syslog to make other CPU's log to be bypass. */
    g_syslog_share_variable->init_phase = SYSLOG_EXCEPTION_INIT;
    g_syslog_share_variable->handle = g_exception_handle;
    g_exception_string_log_count = 0;
    g_exception_binary_log_count = 0;

    mux_exception_init(g_syslog_share_variable->handle);

    g_exception_is_happen = true;
}

/* This API will bypass all log APIs including exception APIs. */
void log_global_turn_off(void)
{
    /* Reset status of syslog to make other CPU's log to be bypass. */
    g_syslog_share_variable->init_phase = SYSLOG_INIT_NONE;
}

int log_print_exception_log(const char *message, ...)
{
    uint8_t *p_data[3];
    uint32_t p_len[2];
    va_list list;
    int32_t log_size;
    char frame_header[PORT_SYSLOG_MAX_ONE_PLAIN_LOG_SIZE];

    if (g_exception_is_happen == false) {
        return 0;
    }
    va_start(list, message);

    log_size = vsnprintf(frame_header, sizeof(frame_header), message, list);
    if (log_size < 0) {
        va_end(list);
        return 0;
    }
    if ((uint32_t)log_size >= sizeof(frame_header)) {
        log_size = sizeof(frame_header) - 1;
    }

    va_end(list);

#ifndef MTK_DEBUG_PLAIN_LOG_ENABLE
    p_data[0] = (uint8_t *)&g_exception_string_log_count;
    p_len[0] = 4;
    p_data[1] = (uint8_t *)frame_header;
    p_len[1] = log_size;
    p_data[2] = NULL;
#else
    p_data[0] = (uint8_t *)frame_header;
    p_len[0] = log_size;
    p_data[1] = NULL;
#endif
    port_syslog_send(RACE_PROTOCAL_EXCEPTION_STRING_LOG, false, (const uint8_t **)p_data, p_len);

    g_exception_string_log_count++;

    return log_size;
}

#define MAX_EXCEPTION_DATA_LENGTH 1024

void log_dump_exception_data(const uint8_t *data, uint32_t size)
{
    uint8_t *p_data[3];
    uint32_t p_len[2];
    uint8_t *curr_data;
    uint32_t curr_size;

    if (g_exception_is_happen == false) {
        return;
    }

    curr_data = (uint8_t *)data;
    while (size > 0) {
        if (size >= MAX_EXCEPTION_DATA_LENGTH) {
            curr_size = MAX_EXCEPTION_DATA_LENGTH;
            size -= MAX_EXCEPTION_DATA_LENGTH;
        } else {
            curr_size = size;
            size = 0;
        }
#ifndef MTK_DEBUG_PLAIN_LOG_ENABLE
        p_data[0] = (uint8_t *)&g_exception_binary_log_count;
        p_len[0] = 4;
        p_data[1] = curr_data;
        p_len[1] = curr_size;
        p_data[2] = NULL;
#else
        p_data[0] = curr_data;
        p_len[0] = curr_size;
        p_data[1] = NULL;
#endif
        port_syslog_send(RACE_PROTOCAL_EXCEPTION_BINARY_LOG, false, (const uint8_t **)p_data, p_len);
        curr_data += curr_size;
        g_exception_binary_log_count++;
    }
}


#define MAX_EXCEPTION_MSG_ID_PARAMETER_NUMBER 20
#define MAX_EXCEPTION_TLV_DUMP_DATA_SIZE      2048
#define MAX_EXCEPTION_USER_DUMP_BUFFER_COUNT  5

NO_INLINE static int32_t exception_binary_log_handler(log_type_t type,
                                                      uint8_t cpu_id,
                                                      const char *message,
                                                      const void **p_data,
                                                      uint32_t *p_length)
{
    uint8_t *p_tx_data[MAX_EXCEPTION_USER_DUMP_BUFFER_COUNT + 3];
    uint32_t p_tx_len[MAX_EXCEPTION_USER_DUMP_BUFFER_COUNT + 2];
    uint8_t tlv_header[12];
    uint32_t i, payload_size, buffer_count, offset;
    uint32_t res_len;

    /* Calculate total size of payload */
    payload_size = 0;
    buffer_count = 0;
    for (i = 0; p_data[i] != NULL; i++) {
        buffer_count++;
        if (buffer_count > MAX_EXCEPTION_USER_DUMP_BUFFER_COUNT) {
            return 0;
        }
        p_tx_data[i + 1] = (uint8_t *)p_data[i];
        p_tx_len[i + 1] = p_length[i];
        payload_size += p_length[i];
    }
    p_tx_data[i + 1] = NULL;

    /* Limit the max size of dump data when calling LOG_TLVDUMP_*() */
    if (g_syslog_share_variable->init_phase == SYSLOG_FULL_INIT) {
        if (payload_size > MAX_EXCEPTION_TLV_DUMP_DATA_SIZE) {
            payload_size = MAX_EXCEPTION_TLV_DUMP_DATA_SIZE;
        }
    }

    /* Initialize the TLV header */
    tlv_header[0] = cpu_id;
    tlv_header[1] = 0;
    tlv_header[2] = 0;
    tlv_header[3] = 0;
    tlv_header[4] = 0;
    tlv_header[5] = 0;
    offset = (uint32_t)message - PORT_SYSLOG_MSG_ADDR_OFFSET;
    tlv_header[6] = offset & 0xff;
    tlv_header[7] = (offset >> 8) & 0xff;
    tlv_header[8] = (offset >> 16) & 0xff;
    tlv_header[9] = (offset >> 24) & 0xff;
    /* syslog header */
    p_tx_data[0] = (uint8_t *)tlv_header;
    p_tx_len[0]  = 10;
    res_len = port_syslog_send(RACE_PROTOCAL_TLV_LOG | type, true, (const uint8_t **)p_tx_data, p_tx_len);

    return res_len;
}

void log_print_exception_msgid_log(uint8_t cpu_id, const char *message, uint32_t arg_cnt, ...)
{
    va_list list;
    uint32_t i, buffer[MAX_EXCEPTION_MSG_ID_PARAMETER_NUMBER];
    void *p_data[2];
    uint32_t p_length[2];

    va_start(list, arg_cnt);

    if (arg_cnt > MAX_EXCEPTION_MSG_ID_PARAMETER_NUMBER) {
        va_end(list);
        return;
    }

    for (i = 0; i < arg_cnt; i++) {
        buffer[i] = va_arg(list, uint32_t);
    }

    p_data[0]   = buffer;
    p_length[0] = arg_cnt * sizeof(uint32_t);
    p_data[1]   = NULL;
    p_length[1] = 0;

    exception_binary_log_handler(LOG_TYPE_EXCEPTION_MSGID_LOG, cpu_id, message, (const void **)p_data, p_length);

    va_end(list);
}


bool log_uart_init(hal_uart_port_t port, hal_uart_baudrate_t baudrate)
{
    exception_config_type exception_config;

    PORT_SYSLOG_UNUSED(port);
    PORT_SYSLOG_UNUSED(baudrate);

    if (g_primary_cpu_syslog_is_initialized == true) {
        return false;
    }

    /* Initialize the global share variables */
    memset((syslog_share_variable_t *)g_syslog_share_variable, 0, sizeof(syslog_share_variable_t));

    port_syslog_early_init();

    g_syslog_share_variable->init_phase = SYSLOG_EARLY_INIT;

    exception_config.init_cb = NULL;
    exception_config.dump_cb = NULL;
    exception_register_callbacks(&exception_config);
    g_exception_is_happen = false;

    g_primary_cpu_syslog_is_initialized = true;

    return true;
}


bool log_init(syslog_port_type_t port_type, uint8_t port_index)
{
    if ((g_primary_cpu_syslog_is_initialized == false) ||
        (g_syslog_share_variable->init_phase != SYSLOG_EARLY_INIT)) {
        return false;
    }

    if (port_type >= SYSLOG_PORT_TYPE_MAX) {
        return false;
    }

    /* log port init */
    g_syslog_port = parse_syslog_port(port_type, port_index);
    log_port_init();

    /* load logging filters setting */
    filter_config_load();

    /* logging device post initialization */
    port_syslog_post_init();

    /* update syslog init phase flag */
    g_syslog_share_variable->init_phase = SYSLOG_FULL_INIT;

    return true;
}

#endif /* MTK_CPU_NUMBER_0 */

#if !defined(MTK_DEBUG_LEVEL_NONE)

bool log_set_filter(void)
{
    uint32_t i, cpu_id, offset;
    uint32_t runtime_filter_number;
    module_filter_info_t *p_filter_info;
    volatile uint8_t *p_filters;
    log_control_block_t *entries;

    if (g_syslog_share_variable->init_phase != SYSLOG_FULL_INIT) {
        return false;
    }

    cpu_id = GET_CURRENT_CPU_ID();

    port_syslog_build_time_sdk_version_copy(cpu_id);

    /* Calculate the number of runtime module filter */
    runtime_filter_number = (LOG_FILTER_END - LOG_FILTER_START) / sizeof(log_control_block_t);
    /* As this variable need to be by accessed by master CPU, so maybe need to do remap */
    entries = (log_control_block_t *)LOG_FILTER_START;
    g_syslog_share_variable->cpu_module_filter[cpu_id] = (void *)port_syslog_memory_remap_to_primary_cpu(cpu_id, (uint32_t)entries);

    p_filter_info = (module_filter_info_t *)g_syslog_share_variable->cpu_module_filter_status;

    for (i = 0; i < MTK_MAX_CPU_NUMBER; i++) {
        if (!(p_filter_info[i].cpu_id & SYSLOG_FILTER_VALID_MARK)) {
            break;
        }
        if ((p_filter_info[i].cpu_id & (~SYSLOG_FILTER_VALID_MARK)) == cpu_id) {
            if (p_filter_info[i].module_number != runtime_filter_number) {
                assert(0);
                return false;
            }
            return true;
        }
    }
    offset = i;

    p_filters = find_start_of_cpu_log_filters(cpu_id);
    for (i = 0; i < runtime_filter_number; i++) {
        p_filters[i] = ((uint8_t)(entries[i].log_switch) << 4) | (uint8_t)(entries[i].print_level);
    }
    p_filter_info[offset].cpu_id = cpu_id | SYSLOG_FILTER_VALID_MARK;
    p_filter_info[offset].module_number = runtime_filter_number;

    return true;
}

void filter_config_print_switch(void *handle, log_switch_t log_switch)
{
    uint32_t index, cpu_id;
    volatile uint8_t *p_filters;

    cpu_id = GET_CURRENT_CPU_ID();
    if (g_syslog_share_variable->cpu_module_filter[cpu_id] == NULL) {
        return;
    }

    index = ((uint32_t)handle - (uint32_t)LOG_FILTER_START) / sizeof(log_control_block_t);
    p_filters = find_start_of_cpu_log_filters(cpu_id);

    p_filters[index] &= 0x0F;
    p_filters[index] |= log_switch << 4;
}

void filter_config_print_level(void *handle, print_level_t log_level)
{
    uint32_t index, cpu_id;
    volatile uint8_t *p_filters;

    cpu_id = GET_CURRENT_CPU_ID();
    if (g_syslog_share_variable->cpu_module_filter[cpu_id] == NULL) {
        return;
    }

    index = ((uint32_t)handle - (uint32_t)LOG_FILTER_START) / sizeof(log_control_block_t);
    p_filters = find_start_of_cpu_log_filters(cpu_id);

    p_filters[index] &= 0xF0;
    p_filters[index] |= log_level;
}

static bool filter_runtime_check(const void *is_module_control, log_control_block_t *context, print_level_t level)
{
    uint32_t offset;
    volatile uint8_t *p_filters;

    /* If current CPU debug level is turn off, bypass the log. */
    if (g_syslog_share_variable->cpu_log_switch[GET_CURRENT_CPU_ID()] == DEBUG_LOG_OFF) {
        return false;
    }

    /* Check the address range to detect the seperate build log filter */
    if (((uint32_t)context >= (uint32_t)LOG_FILTER_END) || ((uint32_t)context < (uint32_t)LOG_FILTER_START)) {
        if ((is_module_control != NULL) &&
            ((context->log_switch == DEBUG_LOG_OFF) ||
             (level < g_syslog_share_variable->cpu_log_print_level[GET_CURRENT_CPU_ID()]) ||
             (level < context->print_level))) {
            return false;
        }

        return true;
    }

    /* For LOG_*()/LOG_MSGID_*()/LOG_TLVDUMP_*(), ignore it if
     * 1. If module's debug level is turn off or
     * 2. If current log level is lower than current CPU's debug level.
     * 3. If current log level is lower than module's debug level.
     */
    offset = ((uint32_t)context - (uint32_t)(g_syslog_share_variable->cpu_module_filter[GET_CURRENT_CPU_ID()])) / sizeof(log_control_block_t);
    p_filters = find_start_of_cpu_log_filters(GET_CURRENT_CPU_ID());
    if ((is_module_control != NULL) &&
        (((p_filters[offset] >> 4) == DEBUG_LOG_OFF) ||
         (level < g_syslog_share_variable->cpu_log_print_level[GET_CURRENT_CPU_ID()]) ||
         (level < (p_filters[offset] & 0x0F)))) {
        return false;
    }

    return true;
}

static const char *print_level_table[] = {"debug", "info", "warning", "error"};

#define change_level_to_string(level) \
  ((level) - PRINT_LEVEL_DEBUG <= PRINT_LEVEL_ERROR) ? print_level_table[level] : "debug"

log_create_module(common, PRINT_LEVEL_INFO);
log_create_module(MPLOG, PRINT_LEVEL_WARNING);
log_create_module(printf, PRINT_LEVEL_INFO);

NO_INLINE static bool check_log_control(const void *is_module_control, log_control_block_t *context, print_level_t level)
{
    /* check whether syslog is initialized. */
    if (g_syslog_share_variable->init_phase == SYSLOG_INIT_NONE) {
        return false;
    }

    /* check whether syslog is exception mode. */
    if (g_syslog_share_variable->init_phase == SYSLOG_EXCEPTION_INIT) {
        return false;
    }

    /* double check whether syslog is exception mode. */
    if (hal_core_status_read(HAL_CORE_CM4) == HAL_CORE_EXCEPTION) {
        return false;
    }

    /* check whether CPU control when not full initialize. */
    if (g_syslog_share_variable->init_phase == SYSLOG_EARLY_INIT) {
        if (GET_CURRENT_CPU_ID() != 0) {
            return false;
        }
    }

    /* check whether debug level control when full initialize */
    if (g_syslog_share_variable->init_phase == SYSLOG_FULL_INIT) {
        if (filter_runtime_check(is_module_control, context, level) == false) {
            return false;
        }
    }

    return true;
}

#ifdef MTK_SYSLOG_SUB_FEATURE_STRING_LOG_SUPPORT
NO_INLINE static void string_log_handler(void *handle,
                                         const char *func,
                                         int line,
                                         print_level_t level,
                                         const char *message,
                                         va_list list,
                                         const void *data,
                                         int data_len)
{
    uint8_t *p_data[4];
    uint32_t p_len[3];
    uint8_t text_dump_data;
    char *p_frame_header;
    char frame_header[PORT_SYSLOG_MAX_ONE_LOG_SIZE];
    int32_t step_size, log_size, max_size;
    log_control_block_t *context = (log_control_block_t *)handle;
    uint32_t per_cpu_irq_mask;

    /* check log control */
    if (check_log_control(func, context, level) == false) {
        return;
    }

#ifdef MTK_DEBUG_PLAIN_LOG_ENABLE
    max_size = sizeof(frame_header) - 2; /* reserved for \r\n */
#else
    max_size = sizeof(frame_header) - 6; /* reserved for TLV header */
#endif

    if (data) {
        max_size -= 1;
    }

    /* Format the log header and calculate the size */
#ifdef MTK_DEBUG_PLAIN_LOG_ENABLE
    p_frame_header = frame_header;
#else
    p_frame_header = &frame_header[6];
#endif
    if ((func) && (strcmp(context->module_name, "printf") != 0)) {
        log_size = snprintf(p_frame_header, max_size,
                            "[M:%s C:%s F:%s L:%d]: ",
                            context->module_name,
                            change_level_to_string(level),
                            func,
                            line);
        if (log_size < 0) {
            return;
        }
        if (log_size >= max_size) {
            log_size = max_size - 1;
        }
    } else {
        log_size = 0;
    }

    /* Format the log string/arguments and calculate the size */
    max_size -= log_size;
    p_frame_header += log_size;
    step_size = vsnprintf(p_frame_header, max_size, message, list);
    if (step_size < 0) {
        return;
    }
    if (step_size >= max_size) {
        step_size = max_size - 1;
    }
    log_size += step_size;

#ifndef MTK_DEBUG_PLAIN_LOG_ENABLE
    hal_nvic_save_and_set_interrupt_mask(&per_cpu_irq_mask);
    /* fill CPU ID / reserve byte / timestamp */
    frame_header[0] = GET_CURRENT_CPU_ID();
    frame_header[1] = g_syslog_share_variable->sequence[0]++;
    frame_header[2] = 0;
    frame_header[3] = 0;
    frame_header[4] = 0;
    frame_header[5] = 0;
    hal_nvic_restore_interrupt_mask(per_cpu_irq_mask);
    port_syslog_tx_hook(RACE_PROTOCAL_STRING_LOG, (uint8_t *)frame_header, port_syslog_get_current_timestamp());
#else
    /* add \r\n in the end */
    frame_header[log_size] = '\r';
    frame_header[log_size + 1] = '\n';
#endif

    p_data[0] = (uint8_t *)frame_header;
    p_data[1] = NULL;
#ifndef MTK_DEBUG_PLAIN_LOG_ENABLE
    p_len[0] = log_size + 6;
#else
    p_len[0] = log_size + 2;
#endif

    if (data) {
        text_dump_data = 0x00;
        p_data[1] = &text_dump_data;
        p_len[1] = 1;
        p_data[2] = (uint8_t *)data;
        p_len[2] = data_len;
        p_data[3] = NULL;
    }

    port_syslog_send(RACE_PROTOCAL_STRING_LOG, true, (const uint8_t **)p_data, p_len);
}

void vprint_module_log(void *handle,
                       const char *func,
                       int line,
                       print_level_t level,
                       const char *message,
                       va_list list)
{
    string_log_handler(handle, func, line, level, message, list, NULL, 0);
}

/* For LOG_*() */
ATTR_TEXT_IN_SYSRAM void print_module_log(void *handle,
                                          const char *func,
                                          int line,
                                          print_level_t level,
                                          const char *message, ...)
{
    va_list ap;

    if (g_syslog_share_variable->cpu_log_switch[GET_CURRENT_CPU_ID()] == DEBUG_LOG_OFF) {
        return;
    }

    va_start(ap, message);
    vprint_module_log(handle, func, line, level, message, ap);
    va_end(ap);
}

/* For printf() */
int __wrap_printf(const char *format, ...)
{
    va_list ap;
    /* module fun line level format ap */
    va_start(ap, format);

#ifdef MTK_DEBUG_PLAIN_LOG_ENABLE
    vprint_module_log(&LOG_CONTROL_BLOCK_SYMBOL(printf), NULL, 0, 0, format, ap);
#else
    const char *fun = __FUNCTION__;
    vprint_module_log(&LOG_CONTROL_BLOCK_SYMBOL(printf), fun, __LINE__, PRINT_LEVEL_INFO, format, ap);
#endif

    va_end(ap);

    return 0;
}

void vdump_module_buffer(void *handle,
                         const char *func,
                         int line,
                         print_level_t level,
                         const void *data,
                         int length,
                         const char *message,
                         va_list list)
{
    string_log_handler(handle, func, line, level, message, list, data, length);
}

/* For LOG_HEXDUMP_*() */
ATTR_TEXT_IN_SYSRAM void dump_module_buffer(void *handle,
                                            const char *func,
                                            int line,
                                            print_level_t level,
                                            const void *data,
                                            int length,
                                            const char *message, ...)
{
    va_list ap;

    if (g_syslog_share_variable->cpu_log_switch[GET_CURRENT_CPU_ID()] == DEBUG_LOG_OFF) {
        return;
    }

    va_start(ap, message);
    vdump_module_buffer(handle, func, line, level, data, length, message, ap);
    va_end(ap);
}
#else /* MTK_SYSLOG_SUB_FEATURE_STRING_LOG_SUPPORT */
void vprint_module_log(void *handle,
                       const char *func,
                       int line,
                       print_level_t level,
                       const char *message,
                       va_list list)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(func);
    PORT_SYSLOG_UNUSED(line);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(message);
    PORT_SYSLOG_UNUSED(list);
}
void print_module_log(void *handle,
                      const char *func,
                      int line,
                      print_level_t level,
                      const char *message, ...)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(func);
    PORT_SYSLOG_UNUSED(line);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(message);
}

int __wrap_printf(const char *format, ...)
{
    PORT_SYSLOG_UNUSED(format);

    return 0;
}
void vdump_module_buffer(void *handle,
                         const char *func,
                         int line,
                         print_level_t level,
                         const void *data,
                         int length,
                         const char *message,
                         va_list list)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(func);
    PORT_SYSLOG_UNUSED(line);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(data);
    PORT_SYSLOG_UNUSED(length);
    PORT_SYSLOG_UNUSED(message);
    PORT_SYSLOG_UNUSED(list);
}
void dump_module_buffer(void *handle,
                        const char *func,
                        int line,
                        print_level_t level,
                        const void *data,
                        int length,
                        const char *message, ...)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(func);
    PORT_SYSLOG_UNUSED(line);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(data);
    PORT_SYSLOG_UNUSED(length);
    PORT_SYSLOG_UNUSED(message);
}
#endif /* MTK_SYSLOG_SUB_FEATURE_STRING_LOG_SUPPORT */

#ifdef MTK_SYSLOG_SUB_FEATURE_BINARY_LOG_SUPPORT

#define MAX_TLV_DUMP_DATA_SIZE 2048

#define MAX_USER_DUMP_BUFFER_COUNT 5

NO_INLINE static int32_t binary_log_handler(void *handle,
                                            print_level_t level,
                                            log_type_t type,
                                            const char *message,
                                            const void **p_data,
                                            uint32_t *p_length)
{
    uint8_t *p_tx_data[MAX_USER_DUMP_BUFFER_COUNT + 3];
    uint32_t p_tx_len[MAX_USER_DUMP_BUFFER_COUNT + 2];
    uint8_t tlv_header[12];
    uint32_t i, payload_size, buffer_count, offset;
    log_control_block_t *context = (log_control_block_t *)handle;
    uint32_t per_cpu_irq_mask;

#ifdef MTK_DEBUG_PLAIN_LOG_ENABLE
    return 0;
#endif

    /* check log control */
    if (check_log_control(p_data, context, level) == false) {
        return 0;
    }

    /* Calculate total size of payload */
    payload_size = 0;
    buffer_count = 0;
    for (i = 0; p_data[i] != NULL; i++) {
        buffer_count++;
        if (buffer_count > MAX_USER_DUMP_BUFFER_COUNT) {
            return 0;
        }
        p_tx_data[i + 1] = (uint8_t *)p_data[i];
        p_tx_len[i + 1] = p_length[i];
        payload_size += p_length[i];
    }
    p_tx_data[i + 1] = NULL;

    /* Limit the max size of dump data when calling LOG_TLVDUMP_*() */
    if (g_syslog_share_variable->init_phase == SYSLOG_FULL_INIT) {
        if (payload_size > MAX_TLV_DUMP_DATA_SIZE) {
            return 0;
        }
    }

    /* Initialize the TLV header */
    if (type == LOG_TYPE_MSG_ID_LOG) {
        hal_nvic_save_and_set_interrupt_mask(&per_cpu_irq_mask);
        tlv_header[0] = GET_CURRENT_CPU_ID();
        tlv_header[1] = g_syslog_share_variable->sequence[0]++;
        tlv_header[2] = 0;
        tlv_header[3] = 0;
        tlv_header[4] = 0;
        tlv_header[5] = 0;
        hal_nvic_restore_interrupt_mask(per_cpu_irq_mask);
        port_syslog_tx_hook(RACE_PROTOCAL_STRING_LOG, (uint8_t *)tlv_header, port_syslog_get_current_timestamp());
        offset = (uint32_t)message - PORT_SYSLOG_MSG_ADDR_OFFSET;
        tlv_header[6] = offset & 0xff;
        tlv_header[7] = (offset >> 8) & 0xff;
        tlv_header[8] = (offset >> 16) & 0xff;
        tlv_header[9] = (offset >> 24) & 0xff;
        /* syslog header */
        p_tx_data[0] = (uint8_t *)tlv_header;
        p_tx_len[0] = 10;
        return port_syslog_send(RACE_PROTOCAL_MSGID_LOG, true, (const uint8_t **)p_tx_data, p_tx_len);
    } else if ((type > LOG_TYPE_COMMON_LOG_END) && (type < LOG_TYPE_SPECIAL_LOG_END)) {
        tlv_header[0] = 0;
        p_tx_data[0] = (uint8_t *)tlv_header;
        p_tx_len[0]  = 0;
        return port_syslog_send(RACE_PROTOCAL_TLV_LOG | type, true, (const uint8_t **)p_tx_data, p_tx_len);
    } else {
        return 0;//other case
    }

    return 0;
}

#define MAX_SYSLOG_MSG_ID_PARAMETER_NUMBER 20

void log_print_msgid(void *handle,
                     print_level_t level,
                     const char *message,
                     uint32_t arg_cnt,
                     va_list list)
{
    uint32_t i, buffer[MAX_SYSLOG_MSG_ID_PARAMETER_NUMBER];
    void *p_data[2];
    uint32_t p_length[2];

#ifdef MTK_DEBUG_PLAIN_LOG_ENABLE
    const char *fun = " ";
    vprint_module_log(handle, fun, 0, level, message, list);
    return;
#endif

    if (arg_cnt > MAX_SYSLOG_MSG_ID_PARAMETER_NUMBER) {
        return;
    }

    for (i = 0; i < arg_cnt; i++) {
        buffer[i] = va_arg(list, uint32_t);
    }

    p_data[0] = buffer;
    p_data[1] = NULL;
    p_length[0] = arg_cnt * sizeof(uint32_t);
    p_length[1] = 0;
    binary_log_handler(handle, level, LOG_TYPE_MSG_ID_LOG, message, (const void **)p_data, p_length);
}

/* For LOG_MSGID_*() */
ATTR_TEXT_IN_SYSRAM void print_module_msgid_log(void *handle,
                                                print_level_t level,
                                                const char *message,
                                                uint32_t arg_cnt, ...)
{
    va_list list;

    if (g_syslog_share_variable->cpu_log_switch[GET_CURRENT_CPU_ID()] == DEBUG_LOG_OFF) {
        return;
    }

    va_start(list, arg_cnt);
    log_print_msgid(handle, level, message, arg_cnt, list);
    va_end(list);
}

/* For LOG_TLVDUMP_*() */
ATTR_TEXT_IN_SYSRAM uint32_t dump_module_tlv_buffer(void *handle,
                                                    print_level_t level,
                                                    log_type_t type,
                                                    const void **p_data,
                                                    uint32_t *p_length)
{

    if (g_syslog_share_variable->cpu_log_switch[GET_CURRENT_CPU_ID()] == DEBUG_LOG_OFF) {
        return 0;
    }

    return binary_log_handler(handle, level, type, NULL, p_data, p_length);
}
#else /* MTK_SYSLOG_SUB_FEATURE_BINARY_LOG_SUPPORT */
void log_print_msgid(void *handle,
                     print_level_t level,
                     const char *message,
                     uint32_t arg_cnt,
                     va_list list)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(message);
    PORT_SYSLOG_UNUSED(arg_cnt);
    PORT_SYSLOG_UNUSED(list);
}
void print_module_msgid_log(void *handle,
                            print_level_t level,
                            const char *message,
                            uint32_t arg_cnt, ...)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(message);
    PORT_SYSLOG_UNUSED(arg_cnt);
}

uint32_t dump_module_tlv_buffer(void *handle,
                                print_level_t level,
                                log_type_t type,
                                const void **p_data,
                                uint32_t *p_length)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(type);
    PORT_SYSLOG_UNUSED(p_data);
    PORT_SYSLOG_UNUSED(p_length);

    return 0;
}

#endif /* MTK_SYSLOG_SUB_FEATURE_BINARY_LOG_SUPPORT */

#else /* !MTK_DEBUG_LEVEL_NONE */

bool log_set_filter(void)
{
    return false;
}

void filter_config_print_switch(void *handle, log_switch_t log_switch)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(log_switch);
}

void filter_config_print_level(void *handle, print_level_t log_level)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(log_level);
}

void vprint_module_log(void *handle,
                       const char *func,
                       int line,
                       print_level_t level,
                       const char *message,
                       va_list list)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(func);
    PORT_SYSLOG_UNUSED(line);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(message);
    PORT_SYSLOG_UNUSED(list);
}

void print_module_log(void *handle,
                      const char *func,
                      int line,
                      print_level_t level,
                      const char *message, ...)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(func);
    PORT_SYSLOG_UNUSED(line);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(message);
}

int __wrap_printf(const char *format, ...)
{
    PORT_SYSLOG_UNUSED(format);

    return 0;
}

void vdump_module_buffer(void *handle,
                         const char *func,
                         int line,
                         print_level_t level,
                         const void *data,
                         int length,
                         const char *message,
                         va_list list)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(func);
    PORT_SYSLOG_UNUSED(line);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(data);
    PORT_SYSLOG_UNUSED(length);
    PORT_SYSLOG_UNUSED(message);
    PORT_SYSLOG_UNUSED(list);
}

void dump_module_buffer(void *handle,
                        const char *func,
                        int line,
                        print_level_t level,
                        const void *data,
                        int length,
                        const char *message, ...)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(func);
    PORT_SYSLOG_UNUSED(line);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(data);
    PORT_SYSLOG_UNUSED(length);
    PORT_SYSLOG_UNUSED(message);
}

void log_print_msgid(void *handle,
                     print_level_t level,
                     const char *message,
                     uint32_t arg_cnt,
                     va_list list)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(message);
    PORT_SYSLOG_UNUSED(arg_cnt);
    PORT_SYSLOG_UNUSED(list);
}

void print_module_msgid_log(void *handle,
                            print_level_t level,
                            const char *message,
                            uint32_t arg_cnt, ...)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(message);
    PORT_SYSLOG_UNUSED(arg_cnt);
}

uint32_t dump_module_tlv_buffer(void *handle,
                                print_level_t level,
                                log_type_t type,
                                const void **p_data,
                                uint32_t *p_length)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(type);
    PORT_SYSLOG_UNUSED(p_data);
    PORT_SYSLOG_UNUSED(p_length);

    return 0;
}

#endif /* !MTK_DEBUG_LEVEL_NONE */

#else /* !MTK_DEBUG_LEVEL_PRINTF */

typedef struct {
    uint8_t port_index;
} syslog_share_variable_t;

#ifdef MTK_SINGLE_CPU_ENV
ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN static volatile syslog_share_variable_t syslog_share_variable;
static volatile syslog_share_variable_t *g_syslog_share_variable = &syslog_share_variable;
#else
#include "hal_resource_assignment.h"
#define PORT_SYSLOG_SHARE_VARIABLE_BEGIN HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_VAR_START
static volatile syslog_share_variable_t *g_syslog_share_variable = (volatile syslog_share_variable_t *)(PORT_SYSLOG_SHARE_VARIABLE_BEGIN);
#endif /* MTK_SINGLE_CPU_ENV */

log_create_module(common, PRINT_LEVEL_INFO);
log_create_module(MPLOG, PRINT_LEVEL_WARNING);
log_create_module(printf, PRINT_LEVEL_INFO);

#ifdef MTK_CPU_NUMBER_0

#include "serial_port_assignment.h"
#include "exception_handler.h"

static volatile bool g_primary_cpu_syslog_is_initialized = false;
static volatile bool g_exception_is_happen = false;

mux_port_t query_syslog_port(void)
{
    return CONFIG_SYSLOG_INIT_STAGE_PORT;
}

bool query_syslog_xoff(void)
{
    return false;
}

void syslog_port_disable_flowcontrol(void)
{}

void exception_syslog_callback(void)
{
    g_exception_is_happen = true;
}

/* This API will bypass all log APIs including exception APIs. */
void log_global_turn_off(void)
{}

int log_print_exception_log(const char *message, ...)
{
    va_list list;
    int32_t log_size;
    char frame_header[PORT_SYSLOG_MAX_ONE_PLAIN_LOG_SIZE];

    if (g_exception_is_happen == false) {
        return 0;
    }

    va_start(list, message);

    log_size = vsnprintf(frame_header, sizeof(frame_header), message, list);
    if (log_size < 0) {
        va_end(list);
        return 0;
    }
    if ((uint32_t)log_size >= sizeof(frame_header)) {
        log_size = sizeof(frame_header) - 1;
    }

    va_end(list);

    hal_uart_send_polling(g_syslog_share_variable->port_index, (uint8_t *)frame_header, log_size);

    return log_size;
}

/* Internal API for exception data dump. */
void log_dump_exception_data(const uint8_t *data, uint32_t size)
{
    if (g_exception_is_happen == false) {
        return;
    }

    hal_uart_send_polling(g_syslog_share_variable->port_index, data, size);
}

NO_INLINE static int32_t exception_binary_log_handler(log_type_t type,
                                                      uint8_t cpu_id,
                                                      const char *message,
                                                      const void **p_data,
                                                      uint32_t *p_length)
{
    PORT_SYSLOG_UNUSED(type);
    PORT_SYSLOG_UNUSED(cpu_id);
    PORT_SYSLOG_UNUSED(message);
    PORT_SYSLOG_UNUSED(p_data);
    PORT_SYSLOG_UNUSED(p_length);
    return 0;
}

void log_print_exception_msgid_log(uint8_t cpu_id, const char *message, uint32_t arg_cnt, ...)
{
    PORT_SYSLOG_UNUSED(cpu_id);
    PORT_SYSLOG_UNUSED(message);
    PORT_SYSLOG_UNUSED(arg_cnt);
}

bool log_uart_init(hal_uart_port_t port, hal_uart_baudrate_t baudrate)
{
    hal_uart_config_t uart_config;
    exception_config_type exception_config;

    PORT_SYSLOG_UNUSED(port);
    PORT_SYSLOG_UNUSED(baudrate);

    if (g_primary_cpu_syslog_is_initialized == true) {
        return false;
    }

    uart_config.baudrate = CONFIG_SYSLOG_BAUDRATE;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    hal_uart_init(CONFIG_SYSLOG_INIT_STAGE_PORT, &uart_config);

    exception_config.init_cb = NULL;
    exception_config.dump_cb = NULL;
    exception_register_callbacks(&exception_config);
    g_exception_is_happen = false;

    g_syslog_share_variable->port_index = CONFIG_SYSLOG_INIT_STAGE_PORT;

    g_primary_cpu_syslog_is_initialized = true;

    return true;
}

bool log_init(syslog_port_type_t port_type, uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_type);
    PORT_SYSLOG_UNUSED(port_index);

    return false;
}

void log_save_filter(void)
{}

#endif /* MTK_CPU_NUMBER_0 */

bool log_set_filter(void)
{
    return false;
}

void filter_config_print_switch(void *handle, log_switch_t log_switch)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(log_switch);
}

void filter_config_print_level(void *handle, print_level_t log_level)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(log_level);
}

void vprint_module_log(void *handle,
                       const char *func,
                       int line,
                       print_level_t level,
                       const char *message,
                       va_list list)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(func);
    PORT_SYSLOG_UNUSED(line);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(message);
    PORT_SYSLOG_UNUSED(list);
}

void print_module_log(void *handle,
                      const char *func,
                      int line,
                      print_level_t level,
                      const char *message, ...)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(func);
    PORT_SYSLOG_UNUSED(line);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(message);
}

int __wrap_printf(const char *format, ...)
{
    va_list ap;
    int32_t log_size;
    uint32_t irq_mask;
    char frame_header[PORT_SYSLOG_MAX_ONE_PLAIN_LOG_SIZE];

    va_start(ap, format);
    log_size = vsnprintf(frame_header, sizeof(frame_header), format, ap);
    if (log_size < 0) {
        return -1;
    }
    if ((uint32_t)log_size >= (sizeof(frame_header))) {
        log_size = sizeof(frame_header) - 1;
    }
    va_end(ap);

    hal_nvic_save_and_set_interrupt_mask(&irq_mask);
#ifndef MTK_SINGLE_CPU_ENV
    while (hal_hw_semaphore_take(HW_SEMAPHORE_SYSLOG) != HAL_HW_SEMAPHORE_STATUS_OK);
#endif
    uart_send_polling(g_syslog_share_variable->port_index, (uint8_t *)frame_header, log_size);
#ifndef MTK_SINGLE_CPU_ENV
    while (hal_hw_semaphore_give(HW_SEMAPHORE_SYSLOG) != HAL_HW_SEMAPHORE_STATUS_OK);
#endif
    hal_nvic_restore_interrupt_mask(irq_mask);

    return log_size;
}

void vdump_module_buffer(void *handle,
                         const char *func,
                         int line,
                         print_level_t level,
                         const void *data,
                         int length,
                         const char *message,
                         va_list list)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(func);
    PORT_SYSLOG_UNUSED(line);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(data);
    PORT_SYSLOG_UNUSED(length);
    PORT_SYSLOG_UNUSED(message);
    PORT_SYSLOG_UNUSED(list);
}

void dump_module_buffer(void *handle,
                        const char *func,
                        int line,
                        print_level_t level,
                        const void *data,
                        int length,
                        const char *message, ...)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(func);
    PORT_SYSLOG_UNUSED(line);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(data);
    PORT_SYSLOG_UNUSED(length);
    PORT_SYSLOG_UNUSED(message);
}

void log_print_msgid(void *handle,
                     print_level_t level,
                     const char *message,
                     uint32_t arg_cnt,
                     va_list list)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(message);
    PORT_SYSLOG_UNUSED(arg_cnt);
    PORT_SYSLOG_UNUSED(list);
}


void print_module_msgid_log(void *handle,
                            print_level_t level,
                            const char *message,
                            uint32_t arg_cnt, ...)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(message);
    PORT_SYSLOG_UNUSED(arg_cnt);
}

uint32_t dump_module_tlv_buffer(void *handle,
                                print_level_t level,
                                log_type_t type,
                                const void **p_data,
                                uint32_t *p_length)
{
    PORT_SYSLOG_UNUSED(handle);
    PORT_SYSLOG_UNUSED(level);
    PORT_SYSLOG_UNUSED(type);
    PORT_SYSLOG_UNUSED(p_data);
    PORT_SYSLOG_UNUSED(p_length);

    return 0;
}

#endif /* !MTK_DEBUG_LEVEL_PRINTF */

