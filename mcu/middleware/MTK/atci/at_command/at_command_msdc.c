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
//#include "atci.h"

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#define MTK_BUILD_HQA_LOAD
#ifdef HAL_SD_MODULE_ENABLED
#if defined(MTK_BUILD_SMT_LOAD) || defined(MTK_BUILD_HQA_LOAD)
#include "hal_sd.h"
#include "hal_msdc.h"
#include "hal_log.h"
#include "stdlib.h"
#include "memory_attribute.h"
#include "task_def.h"

/*
 * sample code
*/
#ifdef MT2523
#define MSDC0_MSDC_IOCON_REG_ADDR (0xA0020014)
#define MSDC1_MSDC_IOCON_REG_ADDR (0xA0030014)
#else

#endif
#define LOGMSGIDE(fmt,cnt,arg...)   LOG_MSGID_E(atcmd, "ATCMD: "fmt,cnt,##arg)
#define LOGMSGIDW(fmt,cnt,arg...)   LOG_MSGID_W(atcmd, "ATCMD: "fmt,cnt,##arg)
#define LOGMSGIDI(fmt,cnt,arg...)   LOG_MSGID_I(atcmd ,"ATCMD: "fmt,cnt,##arg)

#define TEST_BLOCK_NUMBER  (2)
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint32_t msdc_buf[128 * TEST_BLOCK_NUMBER];  /*512byte * TEST_BLOCK_NUMBER = 64K bytes*/

hal_sd_config_t sd_cfg = { HAL_SD_BUS_WIDTH_1,
                           52000,
						   0
                         };

static volatile bool is_read_write_going = 0;
static volatile bool is_write;
static volatile uint32_t msdc_number;
static volatile bool is_stop_done;
static bool read_write_OK = false;

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_msdc(atci_parse_cmd_param_t *parse_cmd);

/*
AT+EMSDC= <0/1>,<clk_src(0,1,2)>,<clk_fre(MHZ)>,<bit(1,4)>,<driving(0,1,2,3)>

*/
extern bool msdc_card_is_present(msdc_port_t msdc_port);

void atci_msdc_modify_io_parameter(hal_sd_port_t sd_port, uint32_t io_config)
{
#ifdef MT2523
    if (HAL_SD_PORT_0 == sd_port) {
        *(volatile uint32_t *)MSDC0_MSDC_IOCON_REG_ADDR = ((*(volatile uint32_t *)MSDC0_MSDC_IOCON_REG_ADDR) & (~ MSDC_IOCON_ODCCFG0_MASK)) |
                (io_config & MSDC_IOCON_ODCCFG0_MASK);
    } else if (HAL_SD_PORT_1 == sd_port) {
        *(volatile uint32_t *)MSDC1_MSDC_IOCON_REG_ADDR = ((*(volatile uint32_t *)MSDC1_MSDC_IOCON_REG_ADDR) & (~ MSDC_IOCON_ODCCFG0_MASK)) |
                (io_config & MSDC_IOCON_ODCCFG0_MASK);
    }
#else

#endif
}

void atci_msdc_get_io_parameter(hal_sd_port_t sd_port, uint32_t *io_config)
{
#ifdef MT2523
    if (HAL_SD_PORT_0 == sd_port) {
        *io_config = (*(volatile uint32_t *)MSDC0_MSDC_IOCON_REG_ADDR) & MSDC_IOCON_ODCCFG0_MASK;
    } else if (HAL_SD_PORT_1 == sd_port) {
        *io_config = (*(volatile uint32_t *)MSDC1_MSDC_IOCON_REG_ADDR) & MSDC_IOCON_ODCCFG0_MASK;
    }
#else

#endif
}

// AT command handler
atci_status_t atci_cmd_hdlr_msdc(atci_parse_cmd_param_t *parse_cmd)
{

    atci_response_t resonse;
    char *param = NULL;
    uint32_t  param_val[10] = {0, 0,0, 0, 0};
    int i = 0;
    uint32_t msdc_iocon_driving = 0;
    uint32_t clock = 0;
    char str[10];
	uint32_t clk_src = 0;
	uint32_t block_start = 0;
    LOGMSGIDI("atci_cmd_hdlr_msdc \r\n", 0);

    memset(&resonse, 0, sizeof(atci_response_t));

    #ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    resonse.cmd_id = parse_cmd->cmd_id;
    #endif

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            strcpy((char *)resonse.response_buf, "+EMSDC:(0~7),<p1>,<p2>,<p3>");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+EMSDC=<op>  the handler need to parse the parameters

            param = strtok(parse_cmd->string_ptr, "=");
            while (NULL != (param = strtok(NULL, ","))) {
                param_val[i++] = atoi(param);
            }
			
            log_hal_msgid_info("atci_cmd_hdlr_msdc P0= %d, P1=%d, P2=%d, P3=%d ,P4=%d \r\n", 5,param_val[0], param_val[1], param_val[2], param_val[3],param_val[4]);
			
			hal_pinmux_set_function(HAL_GPIO_14, 3);
			hal_pinmux_set_function(HAL_GPIO_15, 3);
			hal_pinmux_set_function(HAL_GPIO_16, 3);

			hal_gpio_set_pupd_register(HAL_GPIO_14, 0, 1, 1);
			hal_gpio_set_pupd_register(HAL_GPIO_15, 0, 1, 1);
			hal_gpio_set_pupd_register(HAL_GPIO_16, 0, 1, 1);
			hal_gpio_set_pupd_register(HAL_GPIO_17, 0, 1, 1);
			hal_gpio_set_pupd_register(HAL_GPIO_18, 0, 1, 1);
					
			msdc_iocon_driving = param_val[4];
			
			hal_gpio_set_driving_current(HAL_GPIO_14,msdc_iocon_driving);
			hal_gpio_set_driving_current(HAL_GPIO_15,msdc_iocon_driving);
			hal_gpio_set_driving_current(HAL_GPIO_16,msdc_iocon_driving);
			hal_gpio_set_driving_current(HAL_GPIO_17,msdc_iocon_driving);
			hal_gpio_set_driving_current(HAL_GPIO_18,msdc_iocon_driving);
			hal_gpio_set_driving_current(HAL_GPIO_19,msdc_iocon_driving);
			
			sd_cfg.clk_src = param_val[1];
			sd_cfg.bus_width = param_val[3]-2;
			switch(sd_cfg.clk_src){
				case 0:
					hal_pinmux_set_function(HAL_GPIO_17, 3);
					hal_pinmux_set_function(HAL_GPIO_18, 3);
					hal_pinmux_set_function(HAL_GPIO_19, 3);
					hal_gpio_set_pupd_register(HAL_GPIO_19, 1, 1, 1);					
				break;
				case 1:
					hal_pinmux_set_function(HAL_GPIO_17, 7);
					hal_gpio_set_pupd_register(HAL_GPIO_17, 1, 1, 1);
					hal_pinmux_set_function(HAL_GPIO_18, 3);
					hal_pinmux_set_function(HAL_GPIO_19, 3);
					sd_cfg.bus_width = 1;
					
				break;
				case 2:
					hal_pinmux_set_function(HAL_GPIO_17, 3);
					hal_pinmux_set_function(HAL_GPIO_18, 7);
					hal_gpio_set_pupd_register(HAL_GPIO_18, 1, 1, 1);
					hal_pinmux_set_function(HAL_GPIO_19, 3);
					sd_cfg.bus_width = 1;
				break;
				default:break;
			}
				
				sd_cfg.clock = param_val[2]*1000;
				
				if(0 != hal_sd_init(HAL_SD_PORT_0,&sd_cfg)){
					log_hal_msgid_info("hal_sd_init failed\r\n",0);
					while(1);
				}
				
				while(1){
					if(block_start >= 10000) {
						block_start = 0;
						vTaskDelay(1);
					}
					/*write data*/
					for (i = 0; i < 128; i++) {
						msdc_buf[i] = (i << 24) | (i << 16) | (i << 8) | i;
					}
					if (0 <= hal_sd_write_blocks(HAL_SD_PORT_0, msdc_buf, block_start, 1)) {
						memset(msdc_buf, 0, 128 * sizeof(unsigned int));
						/*read & compare*/
						if (0 <= hal_sd_read_blocks(HAL_SD_PORT_0, msdc_buf, block_start, 1)) {
							for (i = 0; i < 128; i++) {
								if (msdc_buf[i] != ((i << 24) | (i << 16) | (i << 8) | i)) {
									log_hal_msgid_info("test failed\r\n",0);
									resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
									//resonse.response_flag |= ATCI_RESPONSE_FLAG_URC_FORMAT;
									atci_send_response(&resonse);
									return ATCI_STATUS_OK;
								}
							}
							log_hal_msgid_info("block number %d test success\r\n",1,block_start);
							block_start++;
							
							resonse.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
							//resonse.response_flag |= ATCI_RESPONSE_FLAG_URC_FORMAT;
							atci_send_response(&resonse);
						}
					}								
            } 
            break;
        default :
            // others are invalid command format
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            //resonse.response_flag |= ATCI_RESPONSE_FLAG_URC_FORMAT;
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}

#endif
#endif

