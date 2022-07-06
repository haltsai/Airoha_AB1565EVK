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
#include "hal_feature_config.h"
#ifdef HAL_PMU_MODULE_ENABLED
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "at_command.h"
#include "hal.h"
#include "hal_pmu.h"
#include "FreeRTOS.h"
#if ((PRODUCT_VERSION == 2822) || defined(AB1568))
#include"hal_pmu_internal_2568.h"
#include "hal_pmu_auxadc_2568.h"
#include "hal_pmu_ab2568_platform.h"
#endif

#if (defined(AB1565))
#include"hal_pmu_internal_2565.h"
#include "hal_pmu_auxadc_2565.h"
#endif

#if ((PRODUCT_VERSION == 1552) || defined(AM255X))
#include "hal_pmu_auxadc.h"
#include "hal_pmu_mt6388_platform.h"
#endif

#define STRNCPY_PMU(dest, source) strncpy(dest, source, strlen(source)+1);
//--
#if ((PRODUCT_VERSION == 1552) || defined(AM255X) || (PRODUCT_VERSION == 2822) || defined(AB1565)|| defined(AB1568))

void pmu_pk_callback(void) {
    log_hal_msgid_info("[pmu_pk_callback]\r\n",0);
}
#endif
//--
/*--- Function ---*/
atci_status_t atci_cmd_hdlr_pmu(atci_parse_cmd_param_t *parse_cmd);
static int htoi(char s[]);
/*
 AT+EPMUREG=<op>                |   "OK"
 AT+EPMUREG=?                |   "+EPMUREG=(0,1)","OK"


 */
// AT command handler
atci_status_t atci_cmd_hdlr_pmu(atci_parse_cmd_param_t *parse_cmd) {
    atci_response_t response = { { 0 } };
    uint16_t read_value = 0;
    int input_addr = 0;
    int input_value = 0;
    char *end_pos = NULL;
    response.response_flag = 0; // Command Execute Finish.
#ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
            response.cmd_id = parse_cmd->cmd_id;
#endif
    atci_response_heavy_data_t dumpall_response;
    uint8_t str_size = strlen(parse_cmd->string_ptr); /*0d/0a*/
    dumpall_response.response_buf = pvPortMalloc(str_size+1);
    if(dumpall_response.response_buf==NULL){
        LOG_MSGID_I(common,"Command is null",0);
        return ATCI_STATUS_OK;
    }
    char *str  = NULL;
    str =(char *)dumpall_response.response_buf;
    switch (parse_cmd->mode) {
    case ATCI_CMD_MODE_TESTING:    // rec: AT+EPMUREG=?
        STRNCPY_PMU((char *) response.response_buf, "+EPMUREG=(0,1)\r\nOK\r\n");

        response.response_len = strlen((const char *) response.response_buf);
        atci_send_response(&response);
        break;
    case ATCI_CMD_MODE_EXECUTION: // rec: AT+EPMUREG=<op>  the handler need to parse the parameters

        if (strncmp(parse_cmd->string_ptr, "AT+EPMUREG=0,", strlen("AT+EPMUREG=0,")) == 0) {
            /*command: AT+EPMUREG=0,1234*/
            end_pos = strchr(parse_cmd->string_ptr, ',');
            end_pos++;
            input_addr = htoi(end_pos);
            end_pos = NULL;

            /* read data of input register address */

#if ((PRODUCT_VERSION == 1552) || defined(AM255X))
            log_hal_msgid_info("[PMU] read register address:0x%x", 1, input_addr);
            read_value = pmu_get_register_value_2byte_mt6388(input_addr, 0xFFFF, 0);
#else
            read_value = pmu_get_register_value(input_addr, 0xFFFF, 0);
#endif
            snprintf((char *) response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "+EPMUREG:0x%x,0x%x\r\n", input_addr, read_value);
            /* ATCI will help append "OK" at the end of response buffer  */
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = strlen((const char *) response.response_buf);
        } else if (strncmp(parse_cmd->string_ptr, "AT+EPMUREG=1,", strlen("AT+EPMUREG=1,")) == 0) {
            /*command: AT+EPMUREG=1,1234,456*/
            char *mid_pos = NULL;
            mid_pos = strchr(parse_cmd->string_ptr, ',');
            mid_pos++;
            end_pos = strchr(mid_pos, ',');
            if((strlen(mid_pos) - strlen(end_pos))<str_size){
            memcpy(str, mid_pos, strlen(mid_pos) - strlen(end_pos));
            input_addr = htoi(mid_pos);
            end_pos++;
            input_value = htoi(end_pos);
            mid_pos = NULL;
            end_pos = NULL;

            /* write input data to input register address*/
#if ((PRODUCT_VERSION == 1552) || defined(AM255X))
            log_hal_msgid_info("register address:0x%x, set register value:0x%x\r\n", 2, input_addr, input_value);
            pmu_set_register_value_2byte_mt6388(input_addr, 0xFFFF, 0, input_value);
#else
            pmu_set_register_value(input_addr, 0xFFFF, 0, input_value);
#endif
            }else{
                LOG_MSGID_I(common,"command lengh error",0);
            }
                /* ATCI will help append "OK" at the end of response buffer */
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = strlen((const char *) response.response_buf);

        }

#if (PRODUCT_VERSION == 3335)
        else if (strncmp(parse_cmd->string_ptr, "AT+EPMUREG=VCORE,", strlen("AT+EPMUREG=VCORE,")) == 0) {
            /*command: AT+EPMUREG=VCORE,7 (7 => 0.7V, 8 => 0.8V)*/
            end_pos = strchr(parse_cmd->string_ptr, ',');
            end_pos++;
            input_value = atoi(end_pos);
            end_pos = NULL;
            printf("Set vcore voltage =0.%d\r\n", input_value);
            /* Set VCORE voltage */
            switch (input_value)
            {
                case 6:
                    pmu_set_vcore_voltage(PMU_NORMAL, PMU_VCORE_VOL_0P65V);
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    response.response_len = strlen((const char *) response.response_buf);
                    break;
                case 7:
                    pmu_set_vcore_voltage(PMU_NORMAL, PMU_VCORE_VOL_0P7V);
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    response.response_len = strlen((const char *) response.response_buf);
                    break;
                case 8:
                    pmu_set_vcore_voltage(PMU_NORMAL, PMU_VCORE_VOL_0P825V);
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    response.response_len = strlen((const char *) response.response_buf);
                    break;
                default:
                    STRNCPY_PMU((char *) response.response_buf, "ERROR\r\n");
                    response.response_len = strlen((const char *) response.response_buf);
                    printf("Wrong input\r\n");
                    atci_send_response(&response);
                    break;
            }
        } 
#endif

#if ((PRODUCT_VERSION == 1552) || defined(AM255X)|| (PRODUCT_VERSION == 2822))
        else if (strncmp(parse_cmd->string_ptr, "AT+EPMUREG=PWR,", strlen("AT+EPMUREG=PWR,")) == 0) {
            end_pos = strchr(parse_cmd->string_ptr, ',');
            end_pos++;
            input_addr = htoi(end_pos);
            end_pos = NULL;
            if (input_addr == 0) {
                pmu_power_off_sequence(PMU_PWROFF);
            } else {
                pmu_power_off_sequence(PMU_RTC);
            }
            /* ATCI will help append "OK" at the end of response buffer  */
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = strlen((const char *) response.response_buf);
        } else if (strncmp(parse_cmd->string_ptr, "AT+EPMUREG=AUX,", strlen("AT+EPMUREG=AUX,")) == 0) {
            /*command: AT+EPMUREG=0,1234*/
            end_pos = strchr(parse_cmd->string_ptr, ',');
            end_pos++;
            input_addr = htoi(end_pos);
            end_pos = NULL;
            log_hal_msgid_info("Input channel : %d\r\n", 1, (int)input_addr);
            uint32_t temp=pmu_auxadc_get_channel_value(input_addr);
            log_hal_msgid_info("Index :%d : %lx", 2, (int)input_addr,temp);
            snprintf((char *) response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "[Channel%d = 0x%x]\r\n", input_addr, (unsigned int)temp);
            /* ATCI will help append "OK" at the end of response buffer  */
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = strlen((const char *) response.response_buf);
        } else if (strncmp(parse_cmd->string_ptr, "AT+EPMUREG=PWRKEY,", strlen("AT+EPMUREG=PWRKEY,")) == 0) {
            /*command: AT+EPMUREG=0,1234*/
            char *mid_pos = NULL;
            int input_case = 0;
            int input_value = 0;
            mid_pos = strchr(parse_cmd->string_ptr, ',');
            mid_pos++;
            end_pos = strchr(mid_pos, ',');
            if((strlen(mid_pos) - strlen(end_pos))<str_size){
            memcpy(str, mid_pos, strlen(mid_pos) - strlen(end_pos));
            input_case = htoi(mid_pos);
            end_pos++;
            input_value = htoi(end_pos);
            mid_pos = NULL;
            end_pos = NULL;
            log_hal_msgid_info("input_case:0x%x, input_value:0x%x\r\n",2, input_case, input_value);
            switch (input_case){
                case 0:
                    pmu_pwrkey_enable(input_value);
                    break;
                case 1:
                    pmu_pwrkey_duration_time(input_value);
                    break;
                case 2:
                    pmu_long_press_shutdown_function_sel(input_value);
                    break;
            }
            }else{
                LOG_MSGID_I(common,"command lengh error",0);
            }
            /* ATCI will help append "OK" at the end of response buffer  */
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = strlen((const char *) response.response_buf);
        }
#if (PRODUCT_VERSION == 2822)
        else if (strncmp(parse_cmd->string_ptr, "AT+EPMUREG=LOCK,", strlen("AT+EPMUREG=LOCK,")) == 0) {
            char *mid_pos = NULL;
            int input_domain = 0;
            int input_value = 0;
            mid_pos = strchr(parse_cmd->string_ptr, ',');
            mid_pos++;
            end_pos = strchr(mid_pos, ',');
            if((strlen(mid_pos) - strlen(end_pos))<str_size){
            memcpy(str, mid_pos, strlen(mid_pos) - strlen(end_pos));
            input_domain = htoi(mid_pos);
            end_pos++;
            input_value = htoi(end_pos);
            mid_pos = NULL;
            end_pos = NULL;
            log_hal_msgid_info("input_domain :%d input_value :%d", 2, (int)input_domain,(int)input_value);
            switch (input_domain) {
                case 0:
                    pmu_lock_vcore(PMU_DVS,input_value,PMU_UNLOCK);
                break;
                case 1:
                    pmu_lock_vcore(PMU_DVS,input_value,PMU_LOCK);
                break;
                case 2:
                    pmu_select_vcore_voltage(PMU_NORMAL,input_value);
                break;
            }
            }else{
                LOG_MSGID_I(common,"command lengh error",0);
            }
            /* ATCI will help append "OK" at the end of response buffer  */
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = strlen((const char *) response.response_buf);
        }else if (strncmp(parse_cmd->string_ptr, "AT+EPMUREG=ACTMODE,", strlen("AT+EPMUREG=ACTMODE,")) == 0) {
            char *mid_pos = NULL;
            int input_domain = 0;
            int input_value = 0;
            mid_pos = strchr(parse_cmd->string_ptr, ',');
            mid_pos++;
            end_pos = strchr(mid_pos, ',');
            if((strlen(mid_pos) - strlen(end_pos))<str_size){
            memcpy(str, mid_pos, strlen(mid_pos) - strlen(end_pos));
            input_domain = htoi(mid_pos);
            end_pos++;
            input_value = htoi(end_pos);
            mid_pos = NULL;
            end_pos = NULL;
            log_hal_msgid_info("input_domain :%d input_value :%d", 2, (int)input_domain,(int)input_value);

            switch (input_domain) {
                    case PMU_BUCK_VCORE:
                        if(input_value) {
                            pmu_switch_control_mode(PMU_BUCK_VCORE,PMU_HW_MODE);
                        } else {
                            pmu_switch_control_mode(PMU_BUCK_VCORE,PMU_SW_MODE);
                            pmu_enable_sw_lp_mode(PMU_BUCK_VCORE,PMU_SW_MODE);
                        }
                    break;
                    case PMU_BUCK_VIO18:
                        if(input_value) {
                            pmu_switch_control_mode(PMU_BUCK_VIO18,PMU_HW_MODE);
                        } else {
                            pmu_switch_control_mode(PMU_BUCK_VIO18,PMU_SW_MODE);
                            pmu_enable_sw_lp_mode(PMU_BUCK_VIO18,PMU_SW_MODE);
                        }
                    break;
                    case PMU_BUCK_VRF:
                        if(input_value) {
                            pmu_switch_control_mode(PMU_BUCK_VRF,PMU_HW_MODE);
                        } else {
                            pmu_switch_control_mode(PMU_BUCK_VRF,PMU_SW_MODE);
                            pmu_enable_sw_lp_mode(PMU_BUCK_VRF,PMU_SW_MODE);
                        }
                    break;
                    case PMU_BUCK_VAUD18:
                        if(input_value) {
                            pmu_switch_control_mode(PMU_BUCK_VAUD18,PMU_HW_MODE);
                        } else {
                            pmu_switch_control_mode(PMU_BUCK_VAUD18,PMU_SW_MODE);
                            pmu_enable_sw_lp_mode(PMU_BUCK_VAUD18,PMU_SW_MODE);
                        }
                    break;
                    case PMU_LDO_VA18:
                        if(input_value) {
                            pmu_switch_control_mode(PMU_LDO_VA18,PMU_HW_MODE);
                        } else {
                            pmu_switch_control_mode(PMU_LDO_VA18,PMU_SW_MODE);
                            pmu_enable_sw_lp_mode(PMU_LDO_VA18,PMU_SW_MODE);
                        }
                    break;
            }
            }else{
                LOG_MSGID_I(common,"command lengh error",0);
            }
            /* ATCI will help append "OK" at the end of response buffer  */
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = strlen((const char *) response.response_buf);
        }else if (strncmp(parse_cmd->string_ptr, "AT+EPMUREG=DR,", strlen("AT+EPMUREG=DR,")) == 0) {
            end_pos = strchr(parse_cmd->string_ptr, ',');
            end_pos++;
            input_addr = htoi(end_pos);
            end_pos = NULL;
            /* read data of input register address */
            read_value = pmu_get_register_value_ddie(input_addr, 0xFFFF, 0);
            snprintf((char *) response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "+EPMUREG:0x%x,0x%x\r\n", input_addr, read_value);
            /* ATCI will help append "OK" at the end of response buffer  */
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = strlen((const char *) response.response_buf);
        }else if (strncmp(parse_cmd->string_ptr, "AT+EPMUREG=DW,", strlen("AT+EPMUREG=DW,")) == 0) {
                /*command: AT+EPMUREG=1,1234,456*/
                char *mid_pos = NULL;
                mid_pos = strchr(parse_cmd->string_ptr, ',');
                mid_pos++;
                end_pos = strchr(mid_pos, ',');
                if((strlen(mid_pos) - strlen(end_pos))<str_size){
                memcpy(str, mid_pos, strlen(mid_pos) - strlen(end_pos));
                input_addr = htoi(mid_pos);
                end_pos++;
                input_value = htoi(end_pos);
                mid_pos = NULL;
                end_pos = NULL;
                /* write input data to input register address*/
                pmu_set_register_value_ddie(input_addr, 0xFFFF, 0, input_value);
                /* ATCI will help append "OK" at the end of response buffer */
                }else{
                LOG_MSGID_I(common,"command lengh error",0);
                }
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                response.response_len = strlen((const char *) response.response_buf);
        }
#endif
        else if (strncmp(parse_cmd->string_ptr, "AT+EPMUREG=DEBUG,", strlen("AT+EPMUREG=DEBUG,")) == 0) {
            end_pos = strchr(parse_cmd->string_ptr, ',');
            end_pos++;
            input_addr = htoi(end_pos);
                end_pos = NULL;
                switch(input_addr) {
                    case 0:
#if ((PRODUCT_VERSION == 1552) || defined(AM255X))
                    log_hal_msgid_info("===Interrupt setting===\r\n",0);
                    log_hal_msgid_info("PMU INT MASK : COM0 :%x\r\n",1,pmu_get_register_value_mt6388(PMU_INT_MASK_CON0, 0xffff, 0));
                    log_hal_msgid_info("PMU INT MASK : COM1 :%x\r\n",1,pmu_get_register_value_mt6388(PMU_INT_MASK_CON1, 0xffff, 0));
                    log_hal_msgid_info("PMU INT MASK : COM2 :%x\r\n",1,pmu_get_register_value_mt6388(PMU_INT_MASK_CON2, 0xffff, 0));
                    log_hal_msgid_info("PMU INT MASK : COM3 :%x\r\n",1,pmu_get_register_value_mt6388(PMU_INT_MASK_CON3, 0xffff, 0));

                    log_hal_msgid_info("PMU INT status : COM0 :%x\r\n",1,pmu_get_register_value_mt6388(PMU_INT_STATUS0, 0xffff, 0));
                    log_hal_msgid_info("PMU INT status : COM1 :%x\r\n",1,pmu_get_register_value_mt6388(PMU_INT_STATUS1, 0xffff, 0));
                    log_hal_msgid_info("PMU INT status : COM2 :%x\r\n",1,pmu_get_register_value_mt6388(PMU_INT_STATUS2, 0xffff, 0));
                    log_hal_msgid_info("PMU INT status : COM3 :%x\r\n",1,pmu_get_register_value_mt6388(PMU_INT_STATUS3, 0xffff, 0));

                    log_hal_msgid_info("PMU INT enable : COM0 :%x\r\n",1,pmu_get_register_value_mt6388(PMU_INT_CON0, 0xffff, 0));
                    log_hal_msgid_info("PMU INT enable : COM1 :%x\r\n",1,pmu_get_register_value_mt6388(PMU_INT_CON1, 0xffff, 0));
                    log_hal_msgid_info("PMU INT enable : COM2 :%x\r\n",1,pmu_get_register_value_mt6388(PMU_INT_CON2, 0xffff, 0));
                    log_hal_msgid_info("PMU INT enable : COM3 :%x\r\n",1,pmu_get_register_value_mt6388(PMU_INT_CON3, 0xffff, 0));
#endif
#if (PRODUCT_VERSION == 2822)
                    log_hal_msgid_info("===Interrupt setting===\r\n",0);
                    log_hal_msgid_info("PMU INT MASK : COM0 :%x\r\n",1,pmu_get_register_value(PMU_INT_MASK_CON0, 0xffff, 0));
                    log_hal_msgid_info("PMU INT MASK : COM1 :%x\r\n",1,pmu_get_register_value(PMU_INT_MASK_CON1, 0xffff, 0));
                    log_hal_msgid_info("PMU INT MASK : COM2 :%x\r\n",1,pmu_get_register_value(PMU_INT_MASK_CON2, 0xffff, 0));
                    log_hal_msgid_info("PMU INT MASK : COM3 :%x\r\n",1,pmu_get_register_value(PMU_INT_MASK_CON3, 0xffff, 0));
                    log_hal_msgid_info("PMU INT status : COM0 :%x\r\n",1,pmu_get_register_value(PMU_INT_STATUS0, 0xffff, 0));
                    log_hal_msgid_info("PMU INT status : COM1 :%x\r\n",1,pmu_get_register_value(PMU_INT_STATUS1, 0xffff, 0));
                    log_hal_msgid_info("PMU INT status : COM2 :%x\r\n",1,pmu_get_register_value(PMU_INT_STATUS2, 0xffff, 0));
                    log_hal_msgid_info("PMU INT status : COM3 :%x\r\n",1,pmu_get_register_value(PMU_INT_STATUS3, 0xffff, 0));
                    log_hal_msgid_info("PMU INT enable : COM0 :%x\r\n",1,pmu_get_register_value(PMU_INT_CON0, 0xffff, 0));
                    log_hal_msgid_info("PMU INT enable : COM1 :%x\r\n",1,pmu_get_register_value(PMU_INT_CON1, 0xffff, 0));
                    log_hal_msgid_info("PMU INT enable : COM2 :%x\r\n",1,pmu_get_register_value(PMU_INT_CON2, 0xffff, 0));
                    log_hal_msgid_info("PMU INT enable : COM3 :%x\r\n",1,pmu_get_register_value(PMU_INT_CON3, 0xffff, 0));
#endif
                    break;

                    case 1:
                        pmu_register_callback(RG_INT_PWRKEY, pmu_pk_callback, NULL);
                    break;
                }
            /* ATCI will help append "OK" at the end of response buffer  */
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = strlen((const char *) response.response_buf);
        }
#endif
        else {
            /*invalide parameter, return "ERROR"*/
            STRNCPY_PMU((char *) response.response_buf, "ERROR\r\n");
            response.response_len = strlen((const char *) response.response_buf);
        }
        atci_send_response(&response);

        break;
    default:
        /* others are invalid command format */
        STRNCPY_PMU((char *) response.response_buf, "ERROR\r\n");
        response.response_len = strlen((const char *) response.response_buf);
        atci_send_response(&response);
        break;
    }
    vPortFree(dumpall_response.response_buf);
    return ATCI_STATUS_OK;
}

// change the data with hex type in string to data with dec type
int htoi(char s[]) {
    int i;
    int n = 0;
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        i = 2;
    } else {
        i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z'); ++i) {
        if (tolower((int)s[i]) > '9') {
            n = 16 * n + (10 + tolower((int )s[i]) - 'a');
        } else {
            n = 16 * n + (tolower((int)s[i]) - '0');
        }
    }
    return n;
}

#endif

