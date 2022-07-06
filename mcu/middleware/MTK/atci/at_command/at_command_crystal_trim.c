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

// For Register AT command handler
// System head file

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#include "syslog.h"
#include <stdlib.h>
#include "hal_gpio.h"

#ifdef MTK_MUX_ENABLE
#include "mux.h"
#endif

#define ATCI_CMD_FOR_FACTORY_TEST
#define TRIM_TIME_MAX(x)  ((x)*1000)

#if defined(ATCI_CMD_FOR_FACTORY_TEST) && defined(HAL_UART_MODULE_ENABLED)

#include "hal_gpt.h"
#include "hal_uart.h"
#include "hal_uart_internal.h"
#include "hal_wdt.h"
#include "hal_platform.h"

/*Function Declare*/
#if (PRODUCT_VERSION == 1552) || (PRODUCT_VERSION == 2552)
extern hal_uart_status_t   hal_uart_ext_get_uart_config(hal_uart_port_t uart_port, hal_uart_config_t  *config);
#elif (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565) || (PRODUCT_VERSION == 1568)
extern hal_uart_status_t   hal_uart_ext_get_uart_config(hal_uart_port_t uart_port, hal_uart_config_t  *uart_config, hal_uart_dma_config_t *dma_config, uint32_t *callback);
#endif
extern bool                hal_uart_ext_is_dma_mode(hal_uart_port_t uart_port);
extern hal_uart_status_t   hal_uart_ext_set_baudrate(hal_uart_port_t uart_port, uint32_t baudrate);


atci_status_t              atci_cmd_hdlr_crystal_trim(atci_parse_cmd_param_t *parse_cmd);

/* Private variable declare  */
static  volatile    bool    flg_timeout   = false;
static  uint8_t             chr_pattern[] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};

/* AT command handler  */
#if (PRODUCT_VERSION == 1552) || (PRODUCT_VERSION == 2552)
void    cystal_trim_timer_callback(void *args)
{
    flg_timeout = true;
}

bool    send_pattern_char(uint8_t port, uint32_t baudrate, uint32_t timeout_s)
{
    hal_uart_config_t  config;
    uint32_t           handle;
    bool               wdt_en = false;
    LOG_MSGID_I(common, "155x Port:%d,Baudrate:%d,timeout:%dms\r\n",3, (int)port, (int)baudrate, (int)timeout_s);

#ifdef MTK_SMART_CHARGER_1WIRE_ENABLE
#if (PRODUCT_VERSION == 1552)
    hal_pinmux_set_function(1, 0);
    hal_gpio_set_direction(1, 1);
    hal_gpio_set_output(1, 0);

    if( port == HAL_UART_1) {
        /*set GPIO as UART1 mode*/
        hal_pinmux_set_function(HAL_GPIO_10, HAL_GPIO_10_UART1_TXD);
        hal_gpio_set_direction(HAL_GPIO_10, HAL_GPIO_DIRECTION_OUTPUT);
        hal_gpio_disable_pull(HAL_GPIO_10);

        /*set uart rx to input pull down*/
        hal_pinmux_set_function(HAL_GPIO_9, HAL_GPIO_9_UART1_RXD);
        hal_gpio_set_direction(HAL_GPIO_9, HAL_GPIO_DIRECTION_INPUT);
        hal_gpio_pull_up(HAL_GPIO_9);
    }
#endif
#endif

    if (HAL_GPT_STATUS_OK != hal_gpt_sw_get_timer(&handle) ) {
        return false;
    }
    if(HAL_GPT_STATUS_OK !=hal_gpt_sw_start_timer_ms(handle, timeout_s/*+10ms*/, cystal_trim_timer_callback, NULL)){
        return false;
    }
    //hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start);
    flg_timeout = false;
    if(hal_wdt_get_enable_status() == true) {
        hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
        wdt_en = true;
    }

    if(hal_uart_ext_get_uart_config(port, &config) != HAL_UART_STATUS_OK){
        config.baudrate = HAL_UART_BAUDRATE_38400;
        config.parity   = HAL_UART_PARITY_NONE;
        config.stop_bit = HAL_UART_STOP_BIT_1;
        config.word_length = HAL_UART_WORD_LENGTH_8;
        LOG_MSGID_I(common,"uart %d not initialized!", 1, port);
        hal_uart_init(port, &config);
        hal_uart_ext_set_baudrate(port, baudrate);
        while(flg_timeout == false){
            hal_uart_send_polling(port, (const uint8_t *)chr_pattern, sizeof(chr_pattern));
        }
        hal_uart_deinit(port);
    }else {
        LOG_MSGID_I(common,"uart %d initialized!", 1, port);
        if(hal_uart_ext_is_dma_mode(port) == true){
            LOG_MSGID_I(common,"uart %d in dma mode!", 1, port);
            /*Set a signal frequency,Baud rate is twice the frequency*/
            hal_uart_ext_set_baudrate(port, baudrate);

            while(flg_timeout == false){
                    hal_uart_send_dma(port, (const uint8_t *)chr_pattern, sizeof(chr_pattern));
            }
        } else {
            LOG_MSGID_I(common,"uart %d in fifo mode!", 1, port);
            hal_uart_ext_set_baudrate(port, baudrate);
            while(flg_timeout == false){
                hal_uart_send_polling(port, chr_pattern, sizeof(chr_pattern));
            }
        }

        /*Set a signal frequency,Baud rate is twice the frequency*/
        hal_uart_set_baudrate(port, config.baudrate);
    }
    //hal_gpt_sw_stop_timer_ms(handle);
    if(HAL_GPT_STATUS_OK != hal_gpt_sw_free_timer(handle)){
    }
    if(wdt_en == true) {
        hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
    }

#ifdef MTK_SMART_CHARGER_1WIRE_ENABLE
#if (PRODUCT_VERSION == 1552)
    hal_pinmux_set_function(1, 5);
    hal_gpio_pull_up(1);

    if( port == HAL_UART_1) {
        /*set GPIO as UART1 mode*/

        /*set uart tx to disable pull*/
        hal_pinmux_set_function(HAL_GPIO_10, HAL_GPIO_10_GPIO10);
        hal_gpio_set_direction(HAL_GPIO_10, HAL_GPIO_DIRECTION_INPUT);
        hal_gpio_disable_pull(HAL_GPIO_10);

        /*set uart rx to input pull down*/
        hal_pinmux_set_function(HAL_GPIO_9, HAL_GPIO_9_GPIO9);
        hal_gpio_set_direction(HAL_GPIO_9, HAL_GPIO_DIRECTION_INPUT);
        hal_gpio_pull_down(HAL_GPIO_9);
    }
#endif
#endif
    return true;
}
#elif (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565) || (PRODUCT_VERSION == 1568)
extern void mux_restore_callback();  //reset mux software pointer
bool  send_pattern_char_156x(uint8_t port, uint32_t baudrate, uint32_t timeout_s)
{
    hal_uart_config_t  orig_uart_config;
    hal_uart_dma_config_t orig_dma_config;
    uint32_t orig_uart_callback;

    uint32_t start_time, end_time, during_time;
    flg_timeout = false;
    LOG_MSGID_I(common, "156x Port:%d,Baudrate:%d,timeout:%dms\r\n",3, (int)port, (int)baudrate, (int)timeout_s);

    if(hal_uart_ext_get_uart_config(port, &orig_uart_config,&orig_dma_config, &orig_uart_callback) != HAL_UART_STATUS_OK){
        orig_uart_config.baudrate = HAL_UART_BAUDRATE_38400;
        orig_uart_config.parity   = HAL_UART_PARITY_NONE;
        orig_uart_config.stop_bit = HAL_UART_STOP_BIT_1;
        orig_uart_config.word_length = HAL_UART_WORD_LENGTH_8;
        LOG_MSGID_I(common,"156x uart %d not initialized!", 1, port);
        hal_uart_init(port, &orig_uart_config);
        hal_uart_ext_set_baudrate(port, baudrate);
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start_time);
        while(flg_timeout == false){
            hal_uart_send_polling(port, (const uint8_t *)chr_pattern, sizeof(chr_pattern));
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&end_time);
            hal_gpt_get_duration_count(start_time, end_time, &during_time);
            if(during_time > TRIM_TIME_MAX(timeout_s)){
                flg_timeout = true;
            }
        }
        hal_uart_deinit(port);
    }else {
        LOG_MSGID_I(common,"156x uart %d initialized!", 1, port);
        __disable_irq();
        if(hal_uart_ext_is_dma_mode(port) == true){
            LOG_MSGID_I(common,"156x uart %d in dma mode!", 1, port);
            /*Set a signal frequency,Baud rate is twice the frequency*/
            mux_control((mux_port_t)port,MUX_CMD_CLEAN,NULL);
            hal_uart_ext_set_baudrate(port, baudrate);
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start_time);
            while(flg_timeout == false){
                    hal_uart_send_dma(port, (const uint8_t *)chr_pattern, sizeof(chr_pattern));
                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&end_time);
                    hal_gpt_get_duration_count(start_time, end_time, &during_time);
                    if(during_time > TRIM_TIME_MAX(timeout_s)){
                        flg_timeout = true;
                    }
            }
        } else {
            LOG_MSGID_I(common,"156x uart %d in fifo mode!", 1, port);
            hal_uart_ext_set_baudrate(port, baudrate);
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start_time);
            while(flg_timeout == false){
                hal_uart_send_polling(port, (const uint8_t *)chr_pattern, sizeof(chr_pattern));
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&end_time);
                hal_gpt_get_duration_count(start_time, end_time, &during_time);
                if(during_time > TRIM_TIME_MAX(timeout_s)){
                    flg_timeout = true;
                }
            }
        }
        /*Set a signal frequency,Baud rate is twice the frequency*/
        #if 0
        hal_uart_set_baudrate(port, config.baudrate);
        mux_control((mux_port_t)port,MUX_CMD_CLEAN,NULL);
        #else
        mux_control((mux_port_t)port,MUX_CMD_CLEAN,NULL);
        hal_uart_deinit(port);
        hal_uart_init(port,&orig_uart_config);
        hal_uart_set_dma(port, &orig_dma_config);
        hal_uart_register_callback(port,(hal_uart_callback_t)orig_uart_callback,(void*)(intptr_t)port);
        hal_uart_set_software_flowcontrol(port,0x11,0x13,0x77);
        mux_restore_callback(port);
        #endif
        __enable_irq();

    }
    LOG_MSGID_I(common,"start_time=%d end_time=%d,during_time=%d", 3, start_time,end_time,during_time);

    return true;
}
#endif


static void delete_special_char(char* str,char chr)
{
    char *p;
 	for(p = str; *p != '\0'; p++)
 		if(*p != chr)
            *str++ = *p;
 	*str = '\0';
}

/*
    Instructions for use at_command_crystal_trim.c
    AT command: AT+TRIM=CRYSTAL,<uart_port>,<output freqency>,<running time>

    <uart_port>:       Uart port of the desired output frequency,  ex,0,1,2.
    <output freqency>: Specific output frequency,  ex,31250  32.25khz signal freqency.
    <running time>:    Signal output time in milliseconds

    Example 1:
    1)  INPUT: AT+TRIM=?\0d\0a
    2)  RESPONSE: +TRIM
                  OK
    Example 2:
    1)  INPUT: AT+TRIM=CRYSTAL,1,31250,10000\0d\0a
    2)  RESPONSE: UART port 1 will Continuous output 0x55 using 62500 baudrate which is 01010101 in ASCII that look like Square wave.The duration of the entire signal output process is 10000 ms.
*/
atci_status_t atci_cmd_hdlr_crystal_trim(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}};
    //char            *param = NULL;
    //char            param_val;
    uint32_t        para[4],i;
    char            *ptr = NULL;
    //bool            arg_valid = false;
    char            *cmd_str;


    LOG_MSGID_I(common, "atci_cmd_hdlr_crystal_trim \r\n", 0);
    cmd_str = (char *)parse_cmd->string_ptr;
    response.response_flag = 0; /*    Command Execute Finish.  */
    #ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    response.cmd_id = parse_cmd->cmd_id;
    #endif

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    /* rec: AT+TRIM=?   */
            strncpy((char *)response.response_buf, "+TRIM\r\nOK\r\n",strlen("+TRIM\r\nOK\r\n"));
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
        /*AT+TRIM=CRYSTAL,<uart_port>,<baudrate>,<running time>*/
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+TRIM=<op>  the handler need to parse the parameters  */
            if (strncmp(cmd_str, "AT+TRIM=CRYSTAL,", strlen("AT+TRIM=CRYSTAL,")) == 0)
            {
                ptr = strtok(parse_cmd->string_ptr,",");
                for(i=0; i<3; i++) {
                    ptr = strtok(NULL, ",");
                    delete_special_char(ptr,'\r');
                    delete_special_char(ptr,'\n');

                    para[i] = atoi(ptr);

                }
                if(para[0] >= HAL_UART_MAX){
                    goto error;
                }

                strncpy((char *)response.response_buf, "Trim start.\r\n",strlen("Trim start.\r\n"));
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                response.response_len = strlen((char *)response.response_buf);
                atci_send_response(&response);
#if(PRODUCT_VERSION == 1552) || (PRODUCT_VERSION == 2552)
                bool status = send_pattern_char(para[0],para[1],para[2]);
#elif (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565) || (PRODUCT_VERSION == 1568)
                bool status = send_pattern_char_156x(para[0],para[1],para[2]);
#endif

                if(status == true){
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }

                strncpy((char *)response.response_buf, "Trim end.\r\n",strlen("Trim end.\r\n"));
                response.response_len = strlen((char *)response.response_buf);
                atci_send_response(&response);
                return ATCI_STATUS_OK;
            }
error:
            strncpy((char *)response.response_buf, "Invalid Command.\r\n",strlen("Invalid Command.\r\n"));
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
        default :
            /* others are invalid command format */
            strncpy((char *)response.response_buf, "ERROR Command.\r\n",strlen("Invalid Command.\r\n"));
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}
#endif
