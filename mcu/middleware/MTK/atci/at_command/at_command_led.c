/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

// For Register AT command handler
// System head file

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"

#include "hal_gpio.h"
#include "hal_isink.h"
#include <stdlib.h>

//#ifdef MTK_BUILD_SMT_LOAD
#ifdef HAL_GPIO_MODULE_ENABLED
/*
 * sample code
*/


/*--- Function ---*/
atci_status_t atci_cmd_hdlr_led(atci_parse_cmd_param_t *parse_cmd);

/*
AT+ELED=<op>                |   "OK"
AT+ELED=?                   |   "+ELED=(0,1)","OK"
*/

#define     ELED_HELP       "AT+ELED=<led_id>,<mode>(mode0:off, mode1:on, mode2:blink)\r\n"
#define     ELED_INV_PARA   "+ELED:Invalid Para\r\n"
#define     ELED_OK         "+ELED:Succ\r\n"



#if defined(HAL_ISINK_MODULE_ENABLED) && defined(HAL_ISINK_FEATURE_ADVANCED_CONFIG)

inline void    atci_led_handle_isink_module(char *cmd_string, atci_response_t *response)
{
    uint32_t    led_id;
    uint32_t    opt_mode;
    char        *param = NULL;
    bool        set_hw_en = false;

    if(response == NULL){
        return;
    }
    /* Get param from cmd string */
    param = strchr(cmd_string, '=');
    param++;
    led_id  = atoi(param);

    param = strchr(param,',');
    param++;
    opt_mode = atoi(param);

    if(led_id > HAL_ISINK_CHANNEL_1){
        response->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
    } else {
        hal_isink_config_t  config;

        switch(opt_mode){
            case 0:{
                    hal_isink_stop(led_id);
                    hal_isink_deinit(led_id);
                    response->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                }break;
            case 1:{
                    config.mode = HAL_ISINK_MODE_PWM;
                    config.config.pwm_mode.blink_nums    = 0;
                    config.config.pwm_mode.hi_level_time = 8;
                    config.config.pwm_mode.lo_level_time = 2;
                    config.config.pwm_mode.idle_time     = 0;
                    response->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    set_hw_en = true;
                }break;
            case 2:{
                    config.mode = HAL_ISINK_MODE_PWM;
                    config.config.pwm_mode.blink_nums    = 0;
                    config.config.pwm_mode.hi_level_time = 500;
                    config.config.pwm_mode.lo_level_time = 500;
                    config.config.pwm_mode.idle_time     = 0;
                    response->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    set_hw_en = true;
                }break;
            default: {
                    response->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }break;
        }
        if(set_hw_en == true){
            hal_isink_stop(led_id);
            hal_isink_deinit(led_id);
            hal_isink_init(led_id);
            hal_isink_configure(led_id, &config);
            hal_isink_start(led_id);
        }
    }
}
#endif

inline void    atci_led_handle_gpio_module(char *cmd_string, atci_response_t *response)
{
    uint32_t  param_val;
    char     *param = NULL;

    if(response == NULL){
        return;
    }
    param = strtok(cmd_string, "AT+ELED=");
    param_val = atoi(param);
    if (param_val < 1 || param_val > 6) {
        response->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
        return;
    }
    /*   set work mode as GPIO    */
    hal_pinmux_set_function(HAL_GPIO_4, HAL_GPIO_4_GPIO4);
    hal_pinmux_set_function(HAL_GPIO_5, HAL_GPIO_5_GPIO5);
    hal_pinmux_set_function(HAL_GPIO_6, HAL_GPIO_6_GPIO6);
    /*   set dierection to be output  */
    hal_gpio_set_direction(HAL_GPIO_4, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_direction(HAL_GPIO_5, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_direction(HAL_GPIO_6, HAL_GPIO_DIRECTION_OUTPUT);

    if (1 == param_val) {
        hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_LOW);
    } else if(2 == param_val) {
        hal_gpio_set_output(HAL_GPIO_5, HAL_GPIO_DATA_LOW);
    } else if(3 == param_val) {
        hal_gpio_set_output(HAL_GPIO_6, HAL_GPIO_DATA_LOW);
    } else if(4 == param_val) {
        hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_HIGH);
    } else if(5 == param_val) {
        hal_gpio_set_output(HAL_GPIO_5, HAL_GPIO_DATA_HIGH);
    } else if(6 == param_val) {
        hal_gpio_set_output(HAL_GPIO_6, HAL_GPIO_DATA_HIGH);
    }
    response->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
}

inline void    _atci_led_handle_test(char *cmd_string, atci_response_t *response)
{
    uint32_t len;

    len = strlen(ELED_HELP);
    if(len > ATCI_UART_TX_FIFO_BUFFER_SIZE){
        len = ATCI_UART_TX_FIFO_BUFFER_SIZE;
    }
    strncpy((char*)response->response_buf, ELED_HELP, len);
    response->response_len   = strlen((char*)response->response_buf);
    response->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
}


/* AT command handler  */
atci_status_t atci_cmd_hdlr_led(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};
    atci_status_t   status = ATCI_STATUS_OK;

    resonse.response_flag = 0; /*    Command Execute Finish.  */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:{    /* rec: AT+ELED=?   */
                _atci_led_handle_test(parse_cmd->string_ptr, &resonse);
            }break;

        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+ELED=<op>  the handler need to parse the parameters  */
            if (strncmp(parse_cmd->string_ptr, "AT+ELED=", strlen("AT+ELED=")) == 0)
            {
            #if defined(HAL_ISINK_MODULE_ENABLED) && defined(HAL_ISINK_FEATURE_ADVANCED_CONFIG)
                atci_led_handle_isink_module(parse_cmd->string_ptr, &resonse);
            #else
                atci_led_handle_gpio_module(parse_cmd->string_ptr, &resonse);
            #endif
            }
            break;
        default :
            /* others are invalid command format */
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            break;
    }
    atci_send_response(&resonse);
    return status;
}

#endif
//#endif /* MTK_BUILD_SMT_LOAD */



