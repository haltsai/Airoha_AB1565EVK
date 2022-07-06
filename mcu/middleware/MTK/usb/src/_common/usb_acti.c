/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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

#include "usb_resource.h"

#include "atci.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include <stdio.h>
#include <string.h>

#include "usb_custom.h"
#include "usb_resource.h"

/* Syslog create module for usb_detect_host.c */
#include "syslog.h"
log_create_module_variant(USB_ATCI, DEBUG_LOG_ON, PRINT_LEVEL_INFO);

static atci_status_t usb_at_string_handler(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t usb_at_string_reset_handler(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t usb_at_string_save_handler(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t usb_at_basic_info_handler(atci_parse_cmd_param_t *parse_cmd);

/* tricky reference function */
void USB_String_Reset(void);

static atci_cmd_hdlr_item_t g_usb_atci_cmd[] = {
    {
        .command_head = "AT+USBSTR",
        .command_hdlr = usb_at_string_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+USBSTR_RESET",
        .command_hdlr = usb_at_string_reset_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+USBSTR_SAVE",
        .command_hdlr = usb_at_string_save_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+USB_BASICINFO",
        .command_hdlr = usb_at_basic_info_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
};

void usb_atci_init(void) {
    atci_register_handler(g_usb_atci_cmd, sizeof(g_usb_atci_cmd) / sizeof(atci_cmd_hdlr_item_t));
}

static atci_status_t usb_at_string_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {
        .response_buf = {0},
        .response_len = 0,
        .response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR
    };

    usb_string_usage_t usage;
    uint8_t id;
    char str[31];
    int status;

    char *s = NULL;

    LOG_MSGID_I(USB_ATCI, "usb_at_string_handler mode = %d", 1, parse_cmd->mode);
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            s = parse_cmd->string_ptr + parse_cmd->name_len + 1;

            status = sscanf(s, "%x,%x,%[^\n\r]", (unsigned int *)&usage, (unsigned int *)&id, str);
            if(status > 0) {
                USB_String_Create(usage, id, str);

                snprintf((char *)response.response_buf,
                         ATCI_UART_TX_FIFO_BUFFER_SIZE,
                         "usage:x%02x,id:x%02x,len:x%02x,%31s\r\n",
                         usage, id, strlen(str), str
                );
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            }
            else {
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            break;
        }
        default:
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t usb_at_string_reset_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {
        .response_buf = {0},
        .response_len = 0,
        .response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR
    };

    LOG_MSGID_I(USB_ATCI, "usb_at_string_reset_handler mode = %d", 1, parse_cmd->mode);
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_ACTIVE: {
            USB_String_Reset();
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        }
        default:
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            break;
    }
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t usb_at_string_save_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {
        .response_buf = {0},
        .response_len = 0,
        .response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR
    };

    LOG_MSGID_I(USB_ATCI, "usb_at_string_save_handler mode = %d", 1, parse_cmd->mode);
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_ACTIVE: {
            USB_String_Nvkey_Write();
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        }
        default:
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            break;
    }
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

/**
 * TODO: move to right place
 */
#define USB_NVKEY_ADDR_BASICINFO                0x3B10
#define USB_BASICINFO_NVKEY_SIZE                18
#define USB_NVKEY_BASICINFO_OFFSET_USE_CLASS    0
#define USB_NVKEY_BASICINFO_OFFSET_USE_PRODUCT  1
#define USB_NVKEY_BASICINFO_OFFSET_USE_POWER    2
#define USB_NVKEY_BASICINFO_OFFSET_CLASS_INFO   3
#define USB_NVKEY_BASICINFO_OFFSET_PRODUCT_INFO 8
#define USB_NVKEY_BASICINFO_OFFSET_POWER_INFO   14

static atci_status_t usb_at_basic_info_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {
        .response_buf = {0},
        .response_len = 0,
        .response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR
    };

    int status;

    char *s = NULL;
    const char *format_s = "%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x";
    uint32_t u32data[13];

    uint8_t basic_info[USB_BASICINFO_NVKEY_SIZE];
    uint8_t *use_class                      = (uint8_t*)(basic_info + USB_NVKEY_BASICINFO_OFFSET_USE_CLASS);
    uint8_t *use_product                    = (uint8_t*)(basic_info + USB_NVKEY_BASICINFO_OFFSET_USE_PRODUCT);
    uint8_t *use_power                      = (uint8_t*)(basic_info + USB_NVKEY_BASICINFO_OFFSET_USE_POWER);
    usb_custom_class_info_t   *class_info   = (usb_custom_class_info_t  *)(basic_info + USB_NVKEY_BASICINFO_OFFSET_CLASS_INFO);
    usb_custom_product_info_t *product_info = (usb_custom_product_info_t*)(basic_info + USB_NVKEY_BASICINFO_OFFSET_PRODUCT_INFO);
    usb_custom_power_info_t   *power_info   = (usb_custom_power_info_t  *)(basic_info + USB_NVKEY_BASICINFO_OFFSET_POWER_INFO);

    LOG_MSGID_I(USB_ATCI, "usb_at_basic_info_handler mode = %d", 1, parse_cmd->mode);
    switch (parse_cmd->mode) {
#if !defined(AG3335)
        case ATCI_CMD_MODE_EXECUTION: {
            s = parse_cmd->string_ptr + parse_cmd->name_len + 1;

            status = sscanf(s, format_s,
                &u32data[0],  &u32data[1],  &u32data[2],  &u32data[3], &u32data[4],
                &u32data[5],  &u32data[6],  &u32data[7],  &u32data[8], &u32data[9],
                &u32data[10], &u32data[11], &u32data[12], &u32data[13]
            );

            *use_class                  = u32data[0];
            *use_product                = u32data[1];
            *use_power                  = u32data[2];
            class_info->bcdUSB          = u32data[3];
            class_info->class           = u32data[4];
            class_info->sub_class       = u32data[5];
            class_info->protocol        = u32data[6];
            product_info->vender_id     = u32data[7];
            product_info->product_id    = u32data[8];
            product_info->bcd_version   = u32data[9];
            power_info->self_power      = u32data[10];
            power_info->remote_wakeup   = u32data[11];
            power_info->maxpower        = u32data[12];

            /* Debug use log */
            /*
            for (int i = 0; i < USB_BASICINFO_NVKEY_SIZE; i++) {
                LOG_MSGID_I(USB_ATCI, "usb_at_basic_info_handler basic_info[%d]=%02x", 2, i, basic_info[i]);
            }
            */

            LOG_MSGID_I(USB_ATCI, "usb_at_basic_info_handler status = %d", 1, status);
            if(status > 0) {
                status = nvkey_write_data(USB_NVKEY_ADDR_BASICINFO, basic_info, USB_BASICINFO_NVKEY_SIZE);

                if (status) {
                    snprintf((char *)response.response_buf,
                            ATCI_UART_TX_FIFO_BUFFER_SIZE,
                            "write nvkey failed, status:%d\r\n",
                            status
                    );
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }

                snprintf((char *)response.response_buf,
                         ATCI_UART_TX_FIFO_BUFFER_SIZE,
                         "use_class:x%02x,use_product:x%02x,use_power:x%02x\r\n",
                         (unsigned int)*use_class, (unsigned int)*use_product, (unsigned int)*use_power
                );
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            }
            else {
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            break;
        }
#endif
        /*
        case ATCI_CMD_MODE_READ: {
            u32data[0]  = *use_class;
            u32data[1]  = *use_product;
            u32data[2]  = *use_power;
            u32data[3]  = class_info->bcdUSB;
            u32data[4]  = class_info->class;
            u32data[5]  = class_info->sub_class;
            u32data[6]  = class_info->protocol;
            u32data[7]  = product_info->vender_id;
            u32data[8]  = product_info->product_id;
            u32data[9]  = product_info->bcd_version;
            u32data[10] = power_info->self_power;
            u32data[11] = power_info->remote_wakeup;
            u32data[12] = power_info->maxpower;
            snprintf((char *)response.response_buf,
                     1024,
                     format_s,
                     u32data[0],  u32data[1],  u32data[2],  u32data[3], u32data[4],
                     u32data[5],  u32data[6],  u32data[7],  u32data[8], u32data[9],
                     u32data[10], u32data[11], u32data[12], u32data[13]
            );
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        }
        */
        default:
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            break;
    }
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}
