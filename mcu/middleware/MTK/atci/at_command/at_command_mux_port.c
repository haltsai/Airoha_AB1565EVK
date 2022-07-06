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

#ifdef MTK_MUX_ENABLE

/* Includes ------------------------------------------------------------------*/
#include "at_command.h"
#include "mux.h"
#include "toi.h"
#include "syslog.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include <ctype.h>
#include "serial_port_assignment.h"
#include <stdlib.h>

#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#endif

#if  defined ( __GNUC__ )
#ifndef __weak
#define __weak   __attribute__((weak))
#endif /* __weak */
#endif /* __GNUC__ */

/* Private macro -------------------------------------------------------------*/
#define AT_COMMAND_MUX_PORT_HELP \
"AT+MPORT - show or modify mux port assignment and switch.\r\n\
usage:\r\n\
    MUX port show current setting:  AT+MPORT?\r\n\
    MUX port show nvdm setting:     AT+MPORT=0\r\n\
    MUX port assign singal-user:    AT+MPORT=1,<owner_name>,<device_id>\r\n\
    MUX port show nvdm setting:     AT+MPORT=2,<device_id>,<on/off>\r\n\
    MUX port assign multi-user:     AT+MPORT=3,<device_1>,<owner_name...>,<device_2>,<owner_name...>\r\n\0"

log_create_module(atci_mux_port, PRINT_LEVEL_INFO);

#define LOGE(fmt,arg...)   LOG_E(atci_mux_port, "[MUX_atci_port]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atci_mux_port, "[MUX_atci_port]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atci_mux_port ,"[MUX_atci_port]"fmt,##arg)

#define LOGMSGIDE(fmt,cnt,arg...)   LOG_MSGID_E(atci_mux_port ,"[MUX_atci_port]"fmt,cnt,##arg)
#define LOGMSGIDW(fmt,cnt,arg...)   LOG_MSGID_W(atci_mux_port ,"[MUX_atci_port]"fmt,cnt,##arg)
#define LOGMSGIDI(fmt,cnt,arg...)   LOG_MSGID_I(atci_mux_port ,"[MUX_atci_port]"fmt,cnt,##arg)


#define is_0_to_f(_c)   ((_c >= '0' && _c <= '9') || (_c >= 'a' && _c <= 'f'))
#define ascii_to_number(_c)   ((_c >= '0' && _c <= '9') ? (_c - '0') : (_c - 'a' + 10))
#define number_to_ascii(_c)   ((_c <= 9) ? (_c + '0') : (_c - 10 + 'a'))

/* Private typedef -----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Extern functions ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
* @brief      Translate integer to string.
* @param[in]  number: The integer need to tranlate.
* @param[out] buffer: The buffer used to store the string translated from integer.
* @return     bit count of the string.
*/
static uint32_t digital_to_string(uint32_t number, char *buffer)
{
    uint32_t i, bits, temp[10], divider;

    bits = 0;
    divider = number;
    do {
        temp[bits] = divider % 10;
        divider /= 10;
        bits++;
    } while (divider);

    for (i = 0; i < bits; i++) {
        buffer[i] = temp[bits - i - 1] + 0x30;
    }

    return bits;
}

/**
* @brief      Parse device common parameters string imported by ATCI input.
* @param[in]  para: parameters string imported by ATCI input.
* @param[out] owner_name: ower name of port service user after parsing.
* @param[out] dev_id: device index assigned to port service user after parsing.
* @return     whether parse of parameter string is successfully.
*/
static bool parse_command(char *para, char *owner_name, mux_port_t *dev_id)
{
    uint8_t type;
    uint32_t temp;
    char *p_owner_name, *p_device_id, *p_bt_address, *p_curr_pos;

    /* skip AT+MPORT=2 */
    p_owner_name = p_curr_pos = para;
    p_curr_pos = strchr(p_owner_name, ',');
    if (p_curr_pos == NULL) {
        return false;
    }

    /* skip <owner_name> */
    p_owner_name = ++p_curr_pos;
    p_curr_pos = strchr(p_owner_name, ',');
    if (p_curr_pos == NULL) {
        return false;
    }
    *p_curr_pos = '\0';

    /* skip <device_id> */
    p_device_id = ++p_curr_pos;
    p_curr_pos = strchr(p_device_id, ',');
    if (p_curr_pos != NULL) {
        *p_curr_pos = '\0';
        p_bt_address = ++p_curr_pos;
        p_curr_pos = p_bt_address;
    } else {
        p_bt_address = NULL;
        p_curr_pos = p_device_id;
    }

    /* Replace end of parameter string to '\0' */
    while (*p_curr_pos && (*p_curr_pos != '\n') && (*p_curr_pos != '\r')) {
        p_curr_pos++;
    }
    *p_curr_pos = '\0';

    /* extract owner name from parameters string */
    if (strlen(p_owner_name) >= MAX_USER_NAME) {
        return false;
    }
    strncpy(owner_name, p_owner_name,strlen(p_owner_name));

    /* extract device index from parameters string */
    temp = toi(p_device_id, &type);
    if (!(type == TOI_DEC && temp <= MUX_USB_END)) {
        return false;
    }
    *dev_id = (mux_port_t)temp;

    return true;
}

/**
* @brief      Read and format default port setting of all MUX port devices.
* @param[out] response_buf: Buffer to store all formatted data.
* @return     None.
*/
static void at_mux_port_show_running_setting(char *response_buf)
{
    uint32_t i, j, bits;
    char temp[10];
    uint32_t user_count;
    mux_status_t status, f_status;
    mux_port_assign_t mux_port_assign[10];

    f_status = MUX_STATUS_ERROR;
    for(i=MUX_UART_BEGIN; i<MUX_PORT_END; i++) {
        user_count = 0;
        memset((uint8_t*)mux_port_assign, 0 ,sizeof(mux_port_assign_t) * 10);

        status = mux_query_port_user_number(i, &user_count);
        if(status != MUX_STATUS_OK) {
            LOGMSGIDI("status = %d mux port %d user number %d ", 3, status, i, user_count);
            continue;
        }

        status = mux_query_port_user_name(i, (mux_port_assign_t*)&mux_port_assign);
        LOGMSGIDI("mux query user name status = %d", 1, status);

        if((user_count != 0) && (status == MUX_STATUS_OK)) {
            for(j=0; j<user_count; j++) {
                strncat(response_buf, "+MPORT: \0",strlen("+MPORT: \0"));
                strncat(response_buf, mux_port_assign[j].name, strlen(mux_port_assign[j].name));
                strncat(response_buf, " = \0", strlen(" = \0"));
                bits = digital_to_string(i, temp);
                temp[bits] = '\0';
                strncat(response_buf, temp, strlen(temp));
                strncat(response_buf, "\r\n\0", strlen("\r\n\0"));
            }
            f_status = MUX_STATUS_OK;
        }
    }

    if (f_status != MUX_STATUS_OK) {
        strncat(response_buf, " mux running setting none\r\n\0",strlen(" mux running setting none\r\n\0"));
        return;
    }
}

/**
* @brief      Read and format default port setting of all MUX port devices.
* @param[out] response_buf: Buffer to store all formatted data.
* @return     None.
*/
static void at_mux_port_show_nvdm_setting(char *response_buf)
{
    uint32_t i, j, bits;
    char temp[10];
    uint32_t user_count;
    mux_status_t status, f_status;
    mux_port_assign_t mux_port_assign[10];

    f_status = MUX_STATUS_ERROR;
    for(i=MUX_UART_BEGIN; i<MUX_PORT_END; i++) {

        user_count = 0;
        memset((uint8_t*)mux_port_assign, 0 ,sizeof(mux_port_assign_t) * 10);

        status = mux_query_port_user_number_form_nvdm(i, &user_count);
        LOGMSGIDI("status = %d mux port %d user number %d ", 3, status, i, user_count);
        if(status != MUX_STATUS_OK) {
            continue;
        }

        status = mux_query_port_user_name_from_nvdm(i, (mux_port_assign_t*)&mux_port_assign);
        LOGMSGIDI("mux query user name status = %d", 1, status);

        if((user_count != 0) && (status == MUX_STATUS_OK)) {
            for(j=0; j<user_count; j++) {
                strncat(response_buf, "+MPORT: \0", strlen("+MPORT: \0"));
                strncat(response_buf, mux_port_assign[j].name, strlen(mux_port_assign[j].name));
                strncat(response_buf, " = \0", strlen(" = \0"));
                bits = digital_to_string(i, temp);
                temp[bits] = '\0';
                strncat(response_buf, temp,strlen(temp));
                strncat(response_buf, "\r\n\0", strlen("\r\n\0"));
            }
            f_status = MUX_STATUS_OK;
        }
    }

    if (f_status != MUX_STATUS_OK) {
        strncat(response_buf, " mux none\r\n ", strlen(" mux none\r\n "));
        return;
    }
}

mux_status_t mux_user_port_write_config(const char *user_name, mux_port_t device)
{
#ifdef MTK_NVDM_ENABLE
    mux_status_t status;
    mux_port_buffer_t query_port_buffer;
    mux_port_buffer_t query_syslog_port_buffer;

    if ((user_name == NULL ) || (device >= MUX_PORT_END)) {
        LOGMSGIDI("[ERROR_PARAMETER]: please check user_name or device", 0);
        return MUX_STATUS_ERROR_PARAMETER;
    }

    memset(&query_port_buffer, 0 ,sizeof(mux_port_buffer_t));
    memset(&query_syslog_port_buffer, 0 ,sizeof(mux_port_buffer_t));

    if((strncmp(user_name, "atci", 4) == 0) || (strncmp(user_name, "ATCI", 4) == 0)) {
        query_syslog_port_buffer.count = 0;
        query_port_buffer.count = 0;
        /* query nvdm atci port count */
        if (MUX_STATUS_OK != mux_query_port_numbers_from_nvdm("ATCI", (mux_port_buffer_t*)&query_port_buffer.count) ){
            LOGMSGIDI("[ERROR_NVDM_NOT_FOUND]: cannot query atci user in NVDM", 0);
            return MUX_STATUS_ERROR_NVDM_NOT_FOUND;
        }
        /* query nvdm syslog port count */
        if (MUX_STATUS_OK != mux_query_port_numbers_from_nvdm("SYSLOG", (mux_port_buffer_t*)&query_syslog_port_buffer.count)) {
            LOGMSGIDI("[ERROR_NVDM_NOT_FOUND]: cannot query syslog user in NVDM", 0);
            return MUX_STATUS_ERROR_NVDM_NOT_FOUND;
        }

        //LOGMSGIDI("syslog port count = %d, syslog port = %d", 2, query_syslog_port_buffer.count, query_syslog_port_buffer.buf[0]);
        //LOGMSGIDI("atci port count = %d, atci port = %d, set = %d", 3, query_port_buffer.count, query_port_buffer.buf[0], device);

        if(query_syslog_port_buffer.count > 1) {
            LOGMSGIDI("syslog port in NVDM must < 2", 0);
            return MUX_STATUS_ERROR;
        }

        if(query_port_buffer.count > 1) {
            LOGMSGIDI("atci port in NVDM must < 2", 0);
            return MUX_STATUS_ERROR;
        }

        if(device == query_syslog_port_buffer.buf[0]) {
            LOGMSGIDI("atci setting port can't be syslog port", 0);
            return MUX_STATUS_ERROR;
        }

        status = mux_close_delete_from_nvdm(query_port_buffer.buf[0], "ATCI");
        if(status != MUX_STATUS_OK) {
            LOGMSGIDI("mux port delete ATCI from NVDM: error", 0);
            return MUX_STATUS_ERROR;
        }
        LOGMSGIDI("mux port delete ATCI from NVDM: OK", 0);

        status = mux_open_save_to_nvdm(device, "ATCI");
        if(status != MUX_STATUS_OK) {
            LOGMSGIDI("mux port open ATCI save to NVDM: error ", 0);
            return MUX_STATUS_ERROR;
        }
        LOGMSGIDI("mux port open ATCI save to NVDM: OK ", 0);

    }

    return MUX_STATUS_OK;
#else
    return MUX_STATUS_ERROR_NVDM_NOT_FOUND;
#endif /* MTK_NVDM_ENABLE */
}

/**
* @brief      Parse device common parameters string imported by ATCI input.
* @param[in]  para: parameters string imported by ATCI input.
* @param[in]  separator: Specified separation character.
* @param[out] cmd_str: Receive an array of substrings.
* @param[out] number: The number of strings to be split.
* @return     whether parse of parameter string is successfully.
*/
static bool parse_command_split(char *para, const char *separator, char **cmd_str,uint32_t *number)
{
      char *pNext;
      int count = 0;
      if (para == NULL || strlen(para) == 0)
         return false;
      if (separator == NULL || strlen(separator) == 0)
         return false;
      pNext = strtok(para,separator);
      while(pNext != NULL) {
           *cmd_str++ = pNext;
           ++count;
          pNext = strtok(NULL,separator);
     }
     *number = count;
    return true;
}

void delete_special_char(char* str,char chr)
{
    char *p;
 	for(p = str; *p != '\0'; p++)
 		if(*p != chr)
            *str++ = *p;
 	*str = '\0';
}

mux_status_t at_mux_delete_port_setting_from_nvdm(mux_port_t port)
{
#ifdef MTK_NVDM_ENABLE
    char group_name[6];
    char user_name[16];
    nvdm_status_t status;

    if (port >= MUX_PORT_END) {
        return MUX_STATUS_ERROR_PARAMETER;
    }
    strncpy(group_name, "mux", sizeof(group_name));
    snprintf(user_name, 16, "%s%d%s","_port_", port, "_cfg");

    status = nvdm_delete_data_item(group_name, user_name);

    if (status != NVDM_STATUS_OK) {
        return MUX_STATUS_ERROR;
    }

    return MUX_STATUS_OK;
#else
    return MUX_STATUS_ERROR_NVDM_NOT_FOUND;
#endif
}

#if 0
static void at_mux_delete_all_users_of_port(mux_port_t port)
{
    uint32_t i, j;
    uint32_t user_count;
    mux_status_t status;
    mux_port_assign_t mux_port_assign[10];

    for(i=MUX_UART_BEGIN; i<MUX_UART_END; i++) {
        user_count = 0;
        memset((uint8_t*)mux_port_assign, 0 ,sizeof(mux_port_assign_t) * 10);

        status = mux_query_port_user_number_form_nvdm(i, &user_count);
        LOGMSGIDI("[delete_all_users]status = %d mux_port %d user number %d ", 3, status, i, user_count);
        if(status != MUX_STATUS_OK) {
            continue;
        }

        status = mux_query_port_user_name_from_nvdm(i, (mux_port_assign_t*)&mux_port_assign);
        LOGMSGIDI("[delete_all_users]mux query user name status = %d", 1, status);

        if((user_count != 0) && (status == MUX_STATUS_OK)) {
            for(j=0; j<user_count; j++) {
                if (MUX_STATUS_OK != mux_close_delete_from_nvdm(port,mux_port_assign[j].name)){
                    LOGMSGIDI("[delete_all_users]mux_close_delete_from_nvdm fail port = %d", 1, i);
                } else{
                    LOGMSGIDI("[delete_all_users]mux_close_delete_from_nvdm success port = %d", 1, i);
                }
            }
        }
    }

}
#endif

static void at_mux_delete_all_port_users(void)
{
    uint32_t i, j;
    uint32_t user_count;
    mux_status_t status;
    mux_port_assign_t mux_port_assign[10];

    for(i=MUX_UART_BEGIN; i<MUX_USB_END; i++) {
        user_count = 0;
        memset((uint8_t*)mux_port_assign, 0 ,sizeof(mux_port_assign_t) * 10);

        status = mux_query_port_user_number_form_nvdm(i, &user_count);
        LOGMSGIDI("[delete_all_users]status = %d mux_port %d user number %d ", 3, status, i, user_count);
        if(status != MUX_STATUS_OK) {
            continue;
        }

        status = mux_query_port_user_name_from_nvdm(i, (mux_port_assign_t*)&mux_port_assign);
        LOGMSGIDI("[delete_all_users]mux query user name status = %d", 1, status);

        if((user_count != 0) && (status == MUX_STATUS_OK)) {
            for(j=0; j<user_count; j++) {
                if (MUX_STATUS_OK != mux_close_delete_from_nvdm(i,mux_port_assign[j].name)){
                    LOGMSGIDI("[delete_all_users]mux_close_delete_from_nvdm fail port = %d", 1, i);
                } else{
                    LOGMSGIDI("[delete_all_users]mux_close_delete_from_nvdm success port = %d", 1, i);
                }
            }
        }
    }

}

static mux_status_t at_mux_port_config_port_user_in_nvdm(char *cmd_str[], uint32_t number)
{
#ifdef MTK_NVDM_ENABLE

    uint32_t i;
    mux_port_t port=0;

    at_mux_delete_all_port_users();

    for(i = 1; i < number; i++){
        if ((strncmp(cmd_str[i],"0",1)==0)||(strncmp(cmd_str[i],"1",1)==0)||(strncmp(cmd_str[i],"2",1)==0)||(strncmp(cmd_str[i],"4",1)==0)||(strncmp(cmd_str[i],"5",1)==0)){
            //at_mux_delete_all_users_of_port(atoi(cmd_str[i]));
            port = (mux_port_t)atoi(cmd_str[i]);
            //LOGMSGIDI("port = %d,i=%d",2,port,i);
        }else {
            delete_special_char(cmd_str[i],'\r');
            delete_special_char(cmd_str[i],'\n');
            //LOGMSGIDI("cmd_str[%d] = %s",2,i,cmd_str[i]);
            mux_open_save_to_nvdm(port, cmd_str[i]);
        }
    }
    for(i = MUX_UART_BEGIN; i < MUX_UART_END; i++){
        if( MUX_STATUS_OK != at_mux_delete_port_setting_from_nvdm((mux_port_t)i)){
            LOGMSGIDI("at_mux_delete_port_setting_from_nvdm failed port=%d\r\n", 1,i);
       }
    }
    return MUX_STATUS_OK;
#else
    return MUX_STATUS_ERROR_NVDM_NOT_FOUND;
#endif
}


/**
* @brief      AT command handler function for port service.
* @param[in]  parse_cmd: command parameters imported by ATCI module.
* @return     Execution result of port service AT command.
*/
atci_status_t atci_cmd_hdlr_mux_port(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}};
    char owner_name[MAX_USER_NAME];
    mux_port_t dev_id;
    bool result;
    char cmd[256] = {0};
    uint8_t  i = 0;

    char   *ptr = NULL;
    uint32_t para[2]={0};
    char   *cmd_str;
    cmd_str = (char *)parse_cmd->string_ptr;

    response.response_flag = 0; // Command Execute Finish.
    #ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    response.cmd_id = parse_cmd->cmd_id;
    #endif

    strncpy(cmd, (char *)parse_cmd->string_ptr, sizeof(cmd)-1);
    for (i = 0; i < strlen((char *)parse_cmd->string_ptr); i++) {
        cmd[i] = (char)toupper((unsigned char)cmd[i]);
    }

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            LOGMSGIDI("atci_cmd_hdlr_mux_port query command usage\r\n", 0);
            strncpy((char *)(response.response_buf), AT_COMMAND_MUX_PORT_HELP, strlen(AT_COMMAND_MUX_PORT_HELP));
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            response.response_len = strlen((char *)(response.response_buf));
            break;

        case ATCI_CMD_MODE_READ:
            LOGMSGIDI("atci_cmd_hdlr_mux_port query current status\r\n", 0);
            at_mux_port_show_running_setting((char *)(response.response_buf));
            response.response_len = strlen((char *)response.response_buf);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;

        case ATCI_CMD_MODE_EXECUTION:
            /* Show default port for specific port service user. */
            if (strncmp((char *)cmd, "AT+MPORT=0", 10) == 0) {
                LOGMSGIDI("atci_cmd_hdlr_mux_port read port config\r\n", 0);
                at_mux_port_show_nvdm_setting((char *)(response.response_buf));
                response.response_len = strlen((char *)response.response_buf);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            }
            /* Modify default port for specific port service user. */
            else if (strstr((char *)cmd, "AT+MPORT=1,") != NULL) {
                LOGMSGIDI("atci_cmd_hdlr_mux_port write port config\r\n", 0);
                result = parse_command(parse_cmd->string_ptr, owner_name, &dev_id);
                if (result == false) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
#ifdef MTK_RACE_SYSLOG_NO_BOUNDLE
                if(mux_user_port_write_config(owner_name, dev_id) != MUX_STATUS_OK) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                } else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
#else
                mux_user_port_write_config(owner_name, dev_id);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
#endif
            }
            /* Switch port for specific port service user. */
            /* AT+MPORT=2,<port_number>,<on/off>
                <port_number>, MUX_PORT_T.
                <on/off> 1:OFF, 0 ON.
            */
            else if (strncmp(cmd_str, "AT+MPORT=2,", strlen("AT+MPORT=2,")) == 0){
                LOGMSGIDI("atci_cmd_hdlr_mux_port ignore data of a port\r\n", 0);
                ptr = strtok(parse_cmd->string_ptr,",");
                for(i=0; i<2; i++) {
                    ptr = strtok(NULL, ",");
                    if(ptr != NULL){
                        para[i] = atoi(ptr);
                    }
                }
                if(para[1] == 1){
                    mux_control(para[0],MUX_CMD_DISCONNECT,NULL);
                    mux_control(para[0],MUX_CMD_CLEAN,NULL);
                }else if(para[1] == 0){
                    mux_control(para[0],MUX_CMD_CLEAN,NULL);
                    mux_control(para[0],MUX_CMD_CONNECT,NULL);

                }

                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&response);
                return ATCI_STATUS_OK;

            } else if (strstr((char *)cmd, "AT+MPORT=3,") != NULL){
                LOGMSGIDI("atci_cmd_hdlr_mux_port config port setting in nvdm\r\n", 0);
                uint32_t total_str_num;
                char *revbuf[20];
                result = parse_command_split(parse_cmd->string_ptr, "," ,revbuf, &total_str_num);
                if (result == false) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                // for(uint32_t i = 0; i<total_str_num; i++){
                //     LOGMSGIDI("revbuf = %s",1,revbuf[i]);
                // }
                if (MUX_STATUS_OK != at_mux_port_config_port_user_in_nvdm(revbuf, total_str_num)) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

            } else {
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            break;

        default:
            strncpy((char *)(response.response_buf), "ERROR\r\n\0", strlen("ERROR\r\n\0"));
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)(response.response_buf));
            break;
    }

    atci_send_response(&response);

    return ATCI_STATUS_OK;
}

#endif  /*MTK_MUX_ENABLE*/

