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

#include "hal_clock.h"

#if !defined(MTK_CLOCK_AT_COMMAND_DISABLE) && defined(HAL_CLOCK_MODULE_ENABLED)

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "at_command.h"
#include <string.h>
#include "hal_clock_internal.h"

#include "syslog.h"
#define LOGE(fmt,arg...)   LOG_E(atcmd, "[clock]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atcmd, "[clock]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atcmd ,"[clock]"fmt,##arg)

#if (PRODUCT_VERSION == 2822 || defined(HAL_DCXO_LPM_ENABLED))
#include "hal_gpt.h"

#define DCXO_CAPID_EFUSE_REG            (*((dcxo_capid_efuse_t*)0xA2060018))
typedef struct{
    uint16_t DCXO_CAPID_EFUSE_SEL;
    uint16_t DCXO_CAPID_EFUSE;
}__attribute__((packed)) dcxo_capid_efuse_t ;

extern void dcxo_lp_mode(dcxo_mode_t mode);
static dcxo_mode_t dcxo_cur_mode = DCXO_NORMAL_MODE;
void dcxo_check_mode_periodically(){
    dcxo_mode_t dcxo_next_mode = dcxo_cur_mode ^ 1;
    dcxo_lp_mode(dcxo_next_mode);
    dcxo_cur_mode = dcxo_next_mode;
}


void dcxo_tune_fpm_capid(uint32_t target_capid){
    if(DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE < target_capid){
        DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE_SEL = 0x1;
        DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE++;
    }else if(DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE > target_capid){
        DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE_SEL = 0x1;
        DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE--;
    }else{
        hal_gpt_stop_timer(HAL_GPT_7);
        hal_gpt_deinit(HAL_GPT_7);
    }
}
#endif /*(PRODUCT_VERSION == 2822)*/

typedef enum{
    AT_CMD_METER =0,
    AT_CMD_MUX,
    AT_CMD_CG,
    AT_CMD_TEST,
}at_cmd_clock;

#define LOGMSGIDE(fmt,cnt,arg...)   LOG_MSGID_E(atcmd, "[clock]"fmt,cnt,##arg)
#define LOGMSGIDW(fmt,cnt,arg...)   LOG_MSGID_W(atcmd, "[clock]"fmt,cnt,##arg)
#define LOGMSGIDI(fmt,cnt,arg...)   LOG_MSGID_I(atcmd ,"[clock]"fmt,cnt,##arg)

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_clock(atci_parse_cmd_param_t *parse_cmd);

// AT command handler
atci_status_t atci_cmd_hdlr_clock(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response;
    LOGMSGIDI("atci_cmd_hdlr_clock\r\n", 0);
    memset(&response, 0, sizeof(response));
#ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    response.cmd_id = parse_cmd->cmd_id;
#endif
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            /* AT+CLOCK=? */
            snprintf((char *)response.response_buf, sizeof(response.response_buf), "AT+CLOCK=?\r\nTest %s\r\n", "OK");
            break;

        case ATCI_CMD_MODE_READ:
            /* AT+CLOCK? */
#if (PRODUCT_VERSION != 3335)
            clock_dump_info();
#endif
            snprintf((char *)response.response_buf, sizeof(response.response_buf), "AT+CLOCK=?\r\nTest %s\r\n", "OK");
            break;

        default :
            /* others are invalid command format */
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

            char *param = NULL;
            char *param_0 =NULL;
            char *param_1 =NULL;

            char *pch = strtok(parse_cmd->string_ptr,"=");
            if(!strcmp(pch,"AT+CLOCK")){
                while((pch = (strtok(NULL,",")))){
                    if(!param){
                        param = pch;
                    }else if(!param_0){
                        param_0 = pch;
                    }else if(!param_1){
                        param_1 = pch;
                    }else{
                        break;
                    }
                }
            uint8_t func = strtol(param,NULL,10);
            switch(func){
            case AT_CMD_METER: //"AT+CLOCK = 1,1000,14"
#ifdef HAL_CLOCK_METER_ENABLE
                if(param_0 && param_1){
                    uint32_t winset = strtol(param_0,NULL,10);
                    uint32_t clk_src = strtol(param_1,NULL,10);
                    int freq = hal_clock_get_freq_meter(clk_src,winset);
                    snprintf((char *)response.response_buf,sizeof(response.response_buf),"freq meter [%d] \r\n",freq);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
#endif
                break;

            case AT_CMD_MUX: //4
                if(param_0 && param_1){
                    uint32_t mux_id = strtol(param_0,NULL,10);
                    uint32_t mux_sel = strtol(param_1,NULL,10);
                    if(clock_mux_sel(mux_id,mux_sel)== HAL_CLOCK_STATUS_OK){
                        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
                }
                break;
            case AT_CMD_CG:
                if(param_0 && param_1){
                    hal_clock_cg_id clk_cg_id = strtol(param_0,NULL,10);
                    int result = 0;
                    uint32_t on_off = strtol(param_1,NULL,10);
                    if(on_off){
                        if((result = hal_clock_enable(clk_cg_id))!= HAL_CLOCK_STATUS_OK){
                            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                            snprintf((char *)response.response_buf,sizeof(response.response_buf),"linkage_mux : %d",result);
                        }
                    }else{
                        if((result = hal_clock_disable(clk_cg_id))!= HAL_CLOCK_STATUS_OK){
                            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                            snprintf((char *)response.response_buf,sizeof(response.response_buf),"linkage_mux : %d",result);
                        }
                    }
                    snprintf((char *)response.response_buf,sizeof(response.response_buf),"clock_cg_id : %d (%s)",clk_cg_id,on_off?"on":"off");
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                break;

            case AT_CMD_TEST:
#if (PRODUCT_VERSION == 3335) /*(PRODUCT_VERSION == 3335)*/
                if(param_0 && param_1){
                    int value =  strtol(param_1,NULL,10);
                    if(!strcmp(param_0,"dcm_ctrl")){
                        value &= 0x1;
                        BUS_DCM_CON_REG.RG_BUS_CLKSLOW_EN =value;
                        BUS_DCM_CON_REG.RG_BUS_CLKOFF_EN = value;
                        snprintf((char *)response.response_buf,sizeof(response.response_buf),"dcm ctrl : %s\r\n",value ? "on":"off");
                    }else if(!strcmp(param_0,"osc_cali")){
                        hal_clock_set_osc_freq(value);
                        snprintf((char *)response.response_buf,sizeof(response.response_buf),"osc cali : %d\r\n",value);
                    }
                }
#elif (PRODUCT_VERSION == 2822 || defined(HAL_DCXO_LPM_ENABLED))/*(PRODUCT_VERSION == 2822)*/

               if(param_0 && param_1){
                    if(!strcmp(param_0,"dcxo_mode")){
                        dcxo_mode_t mode =  (dcxo_mode_t)strtol(param_1,NULL,10);
                        dcxo_lp_mode(mode);
                        snprintf((char *)response.response_buf,sizeof(response.response_buf),"dcxo mode : %s\r\n",mode ? "Normal":"LowPwr");
                    }else if(!strcmp(param_0,"periodical")){
                        if(hal_gpt_init(HAL_GPT_7)== HAL_GPT_STATUS_OK){
                            uint32_t interval = strtol(param_1,NULL,10);
                            hal_gpt_register_callback(HAL_GPT_7,dcxo_check_mode_periodically,NULL);
                            hal_gpt_start_timer_ms(HAL_GPT_7,interval,HAL_GPT_TIMER_TYPE_REPEAT);
                            snprintf((char *)response.response_buf,sizeof(response.response_buf),"periodical : on\r\n");
                        }else {
                            hal_gpt_stop_timer(HAL_GPT_7);
                            hal_gpt_deinit(HAL_GPT_7);
                            snprintf((char *)response.response_buf,sizeof(response.response_buf),"periodical : off\r\n");
                        }
                    }else if(!strcmp(param_0,"step")){
                        if(hal_gpt_init(HAL_GPT_7)== HAL_GPT_STATUS_OK){
                            uint32_t target = strtol(param_1,NULL,10);
                            hal_gpt_register_callback(HAL_GPT_7,dcxo_tune_fpm_capid,target);
                            hal_gpt_start_timer_us(HAL_GPT_7,30,HAL_GPT_TIMER_TYPE_REPEAT);
                            snprintf((char *)response.response_buf,sizeof(response.response_buf),"tune capid start ..\r\n");
                        }
                    }
                }
#endif
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                break;

            }
        }

            /* others are invalid command format */

    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

#endif /* !MTK_CLOCK_AT_COMMAND_DISABLE && HAL_CLOCK_MODULE_ENABLED */

