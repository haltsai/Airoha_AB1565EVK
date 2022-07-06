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
 


#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"

#include "syslog.h"

#define LOGMSGIDE(fmt,arg...)   LOG_MSGID_E(atcmd ,"ATCMD: "fmt,##arg)
#define LOGMSGIDW(fmt,arg...)   LOG_MSGID_W(atcmd ,"ATCMD: "fmt,##arg)
#define LOGMSGIDI(fmt,arg...)   LOG_MSGID_I(atcmd ,"ATCMD: "fmt,##arg)
#if defined(HAL_SECURITY_MODULE_ENABLED) && defined(MTK_SECURITY_AT_COMMAND_ENABLE)

#if (PRODUCT_VERSION == 2822)
#include "hal_security_internal.h"
#include "hal_security.h"
#endif

#define STRNCPY_SECURITY(dest, source) strncpy(dest, source, strlen(source)+1)
atci_status_t atci_cmd_hdlr_security(atci_parse_cmd_param_t *parse_cmd);


atci_status_t atci_cmd_hdlr_security(atci_parse_cmd_param_t *parse_cmd)
{
#if (PRODUCT_VERSION == 2822)
    atci_response_t response = {{0}};
    char *param     = NULL;
    char *RW        = NULL;
    char *SecLvl    = NULL;
    char *varType   = NULL;
    char *item      = NULL;

    #ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    response.cmd_id = parse_cmd->cmd_id;
    #endif

    response.response_flag = 0; /*    Command Execute Finish.  */
    
	switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            STRNCPY_SECURITY((char *)response.response_buf, "+ECRYPTO=(\"<AES, Key Length, Mode>\")\r\nOK\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
	        break;
        case ATCI_CMD_MODE_EXECUTION:
		    param = strtok(parse_cmd->string_ptr, "=");
            LOGMSGIDI("\n\r param=%s\n\r", 1, param);
            RW = strtok(NULL, ",");  
            LOGMSGIDI("\n\r TYPE=%s\n\r", 1, RW);
            SecLvl  = strtok(NULL, ",");
            varType = strtok(NULL, ",");
            item    = strtok(NULL, ",");
            if ((RW == NULL)|| (SecLvl == NULL) || (varType == NULL) || (item == NULL))
            {
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                STRNCPY_SECURITY((char *)response.response_buf, "miss info. Format:read,0,0,<NAME>\r\n");
  
            } else {
                if (strcmp(RW, "read") == 0) {
                    if(strstr(item, "Enable_Secure_Boot_Check") != NULL ) {
                        uint32_t enable = 0;
                        hal_security_sbc_enable(&enable);
                        snprintf((char *)response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "Secure Boot Enable = %u\r\n", enable);
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    } else if (strstr(item, "Disable_USBDL_By_Auto_Detect") != NULL ) {
                        uint32_t disable = 0;
                        security_usbdl_auto_detect_is_disable(&disable);
                        snprintf((char *)response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "USBDL Auto-Detect Disable = %u\r\n", disable);
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    } else {
                        snprintf((char *)response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "ERROR: name not recognized = (%s)\r\n", item);
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                } else {
                    snprintf((char *)response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "ERROR: Wrong <op> = %s\r\n", RW);
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            }
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
	        break;
        default :
            /* others are invalid command format */
            STRNCPY_SECURITY((char *)response.response_buf, "AT+SEC error\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
#endif
    return ATCI_STATUS_OK;
}
#endif