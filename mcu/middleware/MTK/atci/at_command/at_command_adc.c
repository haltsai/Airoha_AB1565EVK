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
#include <stdlib.h>
#include "hal_gpio.h"


#if (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 7686) ||(PRODUCT_VERSION == 1565)

#ifdef HAL_ADC_MODULE_ENABLED
#include "hal_adc.h"
#include "syslog.h"
log_create_module(at_cmd_adc, PRINT_LEVEL_INFO);
/*
 * sample code
*/

#if(PRODUCT_VERSION == 7686)
static uint16_t adc_sample_to_voltage(uint32_t source_code)
{
    uint16_t voltage = (source_code * 1400) / 4095;
    return voltage;
}
#endif
/*--- Function ---*/
atci_status_t atci_cmd_hdlr_auxadc(atci_parse_cmd_param_t *parse_cmd);

/*
AT+EWDT=<op>                |   "OK"
AT+EWDT=?                   |   "+EAUXADC=(1)","OK"


*/
/* AT command handler  */
atci_status_t atci_cmd_hdlr_auxadc(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}};
#if(PRODUCT_VERSION == 2822)||(PRODUCT_VERSION == 1565)
    char *param = NULL;
    uint32_t param_val[2] = {0};
    uint32_t  adc_k_data = 0;
    uint32_t i = 0;
#endif
#if(PRODUCT_VERSION == 7686)
    char param_val;
#endif
    unsigned int  adc_data = 0;
    unsigned int adc_voltage = 0;

    response.response_flag = 0; /*    Command Execute Finish.  */
#ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    response.cmd_id = parse_cmd->cmd_id;
#endif

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    /* rec: AT+EAUXADC=?   */
            strncpy((char *)response.response_buf, "+EAUXADC:1, measure voltage of CH2.",strlen("+EAUXADC:1, measure voltage of CH2."));
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+EAUXADC=<op>  the handler need to parse the parameters  */
#if (PRODUCT_VERSION == 2822)||(PRODUCT_VERSION == 1565)
            param = strtok(parse_cmd->string_ptr, "=,\n\r");
            while (NULL != (param = strtok(NULL, ",\n\r"))) {
                param_val[i++] = atoi(param);

            }
            hal_pinmux_set_function(HAL_GPIO_6, HAL_GPIO_6_AUXADC6);
            hal_pinmux_set_function(HAL_GPIO_7, HAL_GPIO_7_AUXADC5);
            hal_pinmux_set_function(HAL_GPIO_8, HAL_GPIO_8_AUXADC4);
            hal_pinmux_set_function(HAL_GPIO_9, HAL_GPIO_9_AUXADC3);
            hal_pinmux_set_function(HAL_GPIO_10, HAL_GPIO_10_AUXADC2);
            hal_pinmux_set_function(HAL_GPIO_11, HAL_GPIO_11_AUXADC1);
            hal_pinmux_set_function(HAL_GPIO_12, HAL_GPIO_12_AUXADC0);
 
            hal_adc_init();

            for (i = 0; i < param_val[1]; i++) {
                LOG_MSGID_I(at_cmd_adc,"adc_data = 0x%x\r\n",1, adc_data);
                switch (param_val[0]) {
                    case 0:
                        hal_adc_get_data_polling(HAL_ADC_CHANNEL_0, (uint32_t *)&adc_data);
                        hal_adc_get_calibraton_data(adc_data, &adc_k_data);
                        hal_adc_get_calibraton_voltage(adc_data, (uint32_t *)&adc_voltage);
                        break;
                    case 1:
                        hal_adc_get_data_polling(HAL_ADC_CHANNEL_1, (uint32_t *)&adc_data);
                        hal_adc_get_calibraton_data(adc_data, &adc_k_data);
                        hal_adc_get_calibraton_voltage(adc_data, (uint32_t *)&adc_voltage);
                        break;
                    case 2:
                        hal_adc_get_data_polling(HAL_ADC_CHANNEL_2, (uint32_t *)&adc_data);
                        hal_adc_get_calibraton_data(adc_data, &adc_k_data);
                        hal_adc_get_calibraton_voltage(adc_data,(uint32_t *) &adc_voltage);
                        break;
                    case 3:
                        hal_adc_get_data_polling(HAL_ADC_CHANNEL_3, (uint32_t *)&adc_data);
                        hal_adc_get_calibraton_data(adc_data, &adc_k_data);
                        hal_adc_get_calibraton_voltage(adc_data, (uint32_t *)&adc_voltage);
                        break;
                    case 4:
                        hal_adc_get_data_polling(HAL_ADC_CHANNEL_4, (uint32_t *)&adc_data);
                        hal_adc_get_calibraton_data(adc_data, &adc_k_data);
                        hal_adc_get_calibraton_voltage(adc_data,(uint32_t *) &adc_voltage);
                        break;
                    case 5:
                        hal_adc_get_data_polling(HAL_ADC_CHANNEL_5, (uint32_t *)&adc_data);
                        hal_adc_get_calibraton_data(adc_data, &adc_k_data);
                        hal_adc_get_calibraton_voltage(adc_data,(uint32_t *) &adc_voltage);
                        break;
                    case 6:
                        hal_adc_get_data_polling(HAL_ADC_CHANNEL_6, (uint32_t *)&adc_data);
                        hal_adc_get_calibraton_data(adc_data, &adc_k_data);
                        hal_adc_get_calibraton_voltage(adc_data, (uint32_t *)&adc_voltage);
                        break;
                    default:  //strcpy((char *)response.response_buf, "ERROR Command.\r\n");
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        response.response_len = strlen((char *)response.response_buf);
                        atci_send_response(&response);
                        hal_adc_deinit();
                        return ATCI_STATUS_OK;
                        break;
                }


                //LOG_MSGID_I(at_cmd_adc,"adc_channel = %d, adc_data = 0x%04x, adc_k_data = 0x%04x,adc_voltage = %dmV\r\n",4, (int) param_val[0],(unsigned int)adc_data,(unsigned int) adc_k_data, (unsigned int)adc_voltage);
                hal_pinmux_set_function(HAL_GPIO_7, HAL_GPIO_7_UART2_TXD);
                hal_pinmux_set_function(HAL_GPIO_12, HAL_GPIO_12_UART2_RXD);

                snprintf((char *)response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,"+EAUXADC:adc_channel = %d adc_data = 0x%04x, adc_k_data = 0x%04x,adc_voltage = %dmV",(int) param_val[0],(unsigned int) adc_data, (unsigned int)adc_k_data,(unsigned int) adc_voltage);

                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

                response.response_len = strlen((char *)response.response_buf);

                atci_send_response(&response);

                vTaskDelay(100);

            }
            hal_adc_deinit();
#endif
#if(PRODUCT_VERSION == 7686)

                if (parse_cmd->parse_pos < parse_cmd->string_len) {
                   param_val = *(parse_cmd->string_ptr + parse_cmd->parse_pos);
                //hal_pinmux_set_function(HAL_GPIO_32, 5);
                hal_adc_init();
                switch(param_val) {
                        case '0': hal_adc_get_data_polling(HAL_ADC_CHANNEL_0, (uint32_t *)&adc_data);
                       //adc_raw_data = g_adc_raw_data[HAL_ADC_CHANNEL_0];
                        break;
                        case '1': hal_adc_get_data_polling(HAL_ADC_CHANNEL_1, (uint32_t *)&adc_data);
                       //adc_raw_data = g_adc_raw_data[HAL_ADC_CHANNEL_1];
                        break;
                        case '2': hal_adc_get_data_polling(HAL_ADC_CHANNEL_2, (uint32_t *)&adc_data);
                      //adc_raw_data = g_adc_raw_data[HAL_ADC_CHANNEL_2];
                        break;
                        case '3': hal_adc_get_data_polling(HAL_ADC_CHANNEL_3, (uint32_t *)&adc_data);
                     //adc_raw_data = g_adc_raw_data[HAL_ADC_CHANNEL_3];
                       break;
                       default:  //strcpy((char *)response.response_buf, "ERROR Command.\r\n");
                       response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                       response.response_len = strlen((char *)response.response_buf);
                       atci_send_response(&response);
                       hal_adc_deinit();
                       return ATCI_STATUS_OK;
                       break;
                             }
                    hal_adc_deinit();

                    adc_voltage = adc_sample_to_voltage(adc_data);
                    if ((adc_voltage > 800) && (adc_voltage < 1000)) {
                        sprintf((char *)response.response_buf, "+EAUXADC:adc_channel = %c, adc_data = 0x%04x, adc_voltage = %dmV", param_val,adc_data, adc_voltage);
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    } else {
                        sprintf((char *)response.response_buf, "+EAUXADC:adc_channel = %c, adc_data = 0x%04x, adc_voltage = %dmV", param_val,adc_data, adc_voltage);
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                    response.response_len = strlen((char *)response.response_buf);
                    atci_send_response(&response);
                }
                    else {
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    response.response_len = 0;
                    atci_send_response(&response);
                         }
#endif     
          break;
          default :
            /* others are invalid command format */
            strncpy((char *)response.response_buf, "ERROR Command.\r\n",strlen("ERROR Command.\r\n"));
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

#endif
#else
atci_status_t atci_cmd_hdlr_auxadc(atci_parse_cmd_param_t *parse_cmd)
{
    parse_cmd = parse_cmd;
    return ATCI_STATUS_OK;
}
#endif

