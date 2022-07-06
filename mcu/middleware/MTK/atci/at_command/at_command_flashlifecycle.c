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

// For Register AT command handler
// System head file
#include "hal_feature_config.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "at_command.h"
#include "syslog.h"
#include "hal_gpt.h"

#if !defined(MTK_REG_AT_COMMAND_DISABLE) && defined(HAL_FLASH_MODULE_ENABLED)

//#if defined(MTK_NVDM_ENABLE)
//#include "memory_map.h"
//#endif

#if defined(MTK_FATFS_ON_SERIAL_NOR_FLASH)
#include "hal_flash_opt_gen.h"
#endif

#if defined(MTK_FATFS_ON_SERIAL_NOR_FLASH)
#if defined(__NOR_FDM5__)
// verno(2B) + LBI (2B) + Block type (1B)  + EC (3B)
typedef struct {
    uint16_t version;
    uint16_t LA_BLK_ID;
    uint32_t blockinfo;
} BlockHeader;
#else
//   block type(1B) + EC (3B)
typedef struct {
    uint32_t blockinfo;
} BlockHeader;
#endif
#endif

#if defined(MTK_NVDM_ENABLE)
typedef enum {
    PEB_STATUS_VIRGIN = 0xFF,   /* the block status is undefined, it maybe has erased or not erased completely */
    PEB_STATUS_EMPTY = 0xFE,    /* the block has valid PEB header, no valid data */
    PEB_STATUS_ACTIVING = 0xFC, /* the block is frist being filled with data item */
    PEB_STATUS_TRANSFERING = 0xF8,  /* the block is being transfered data from reclaimed block */
    PEB_STATUS_TRANSFERED = 0xF0,   /* the block has being transfered data completely */
    PEB_STATUS_ACTIVED = 0xE0,  /* the block has valid PEB header and data */
    PEB_STATUS_RECLAIMING = 0xC0,   /* the block is very dirty, and is being reclaimed */
    PEB_STATUS_ERASING = 0x80,  /* the block is being erased */
} peb_status_t;

typedef struct {
    uint32_t magic;         /* erase header magic number */
    uint32_t erase_count;   /* erase count of this block */
    peb_status_t status;    /* status of PEB */
    uint8_t peb_reserved;   /* block is reserved and not used */
    uint8_t version;        /* version of NVDM */
    uint8_t reserved;       /* reserved byte */
} peb_header_t;

#endif

log_create_module(atci_sflash, PRINT_LEVEL_INFO);

#if 0
#define LOGE(fmt,arg...)   LOG_E(atci_sflash, "[REG]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atci_sflash, "[REG]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atci_sflash ,"[REG]"fmt,##arg)
#else
#define LOGE(fmt,arg...)   printf("[SFLASH]"fmt,##arg)
#define LOGW(fmt,arg...)   printf("[SFLASH]"fmt,##arg)
#define LOGI(fmt,arg...)   printf("[SFLASH]"fmt,##arg)

#define LOGMSGIDI(fmt,cnt,arg...)   LOG_MSGID_I(atci_sflash ,"[SFLASH]"fmt,cnt,##arg)
#define LOGMSGIDW(fmt,cnt,arg...)   LOG_MSGID_W(atci_sflash, "[SFLASH]"fmt,cnt,##arg)
#define LOGMSGIDE(fmt,cnt,arg...)   LOG_MSGID_E(atci_sflash ,"[SFLASH]"fmt,cnt,##arg)
#endif

#if defined(MTK_NVDM_ENABLE)
#define NVDM_PORT_PEB_SIZE (4096)
#define NVDM_PORT_REGION_SIZE (64*1024)   //(ROM_NVDM_LENGTH)

extern void peb_read_header(int32_t pnum, peb_header_t *peb_header);
#define NVDM_BLK_CNT    (NVDM_PORT_REGION_SIZE/NVDM_PORT_PEB_SIZE)
#define NVDM_RSV_BLK_CNT    (128/4)    //  128KB/4KB
uint32_t nvdm_EC_info_table_s[NVDM_RSV_BLK_CNT];
uint32_t nvdm_EC_info_table_e[NVDM_RSV_BLK_CNT];

uint32_t ec_nvdm_total_count;     //total count
uint32_t ec_nvdm_avg_cycles_init; //cycles at start
uint32_t ec_nvdm_avg_cycles_inc;  //increate at end
#endif

#if defined(MTK_FATFS_ON_SERIAL_NOR_FLASH)
#define FS_BLK_CNT   128       //(FS_LENGTH/FS_NOR_BLOCK_SIZE)
uint32_t FS_EC_info_table_s[FS_BLK_CNT];
uint32_t FS_EC_info_table_e[FS_BLK_CNT];


uint32_t ec_sfc_total_count;     //total count
uint32_t ec_sfc_avg_cycles_init; //cycles at start
uint32_t ec_sfc_avg_cycles_inc;  //increate at end
#endif

uint32_t sflash_EC_start;    //start or end flag
uint32_t ec_start_time;      //test start time
uint32_t ec_end_time;        //test end time
uint32_t ec_test_duration;   //test duration

void dump_ec(uint32_t flag)
{
    uint32_t i;

#if defined(MTK_NVDM_ENABLE)
    if (flag == 0) {
        LOGMSGIDI("NVDM EC dump from power on: \r\n" , 0);
        for (i=0;i<NVDM_BLK_CNT;i=i+8) {
            LOGMSGIDI("%8d, %8d, %8d, %8d, %8d, %8d, %8d, %8d \r\n",8,
                    nvdm_EC_info_table_e[i+0],nvdm_EC_info_table_e[i+1],
                    nvdm_EC_info_table_e[i+2],nvdm_EC_info_table_e[i+3],
                    nvdm_EC_info_table_e[i+4],nvdm_EC_info_table_e[i+5],
                    nvdm_EC_info_table_e[i+6],nvdm_EC_info_table_e[i+7]);
        }
        LOGMSGIDI("\r\n NVDM EC dump from power on Finished. \r\n" ,0);
    } else {
        LOGMSGIDI("NVDM EC dump start : \r\n", 0);
        for (i=0;i<NVDM_BLK_CNT;i=i+8) {
            LOGMSGIDI("%8d, %8d, %8d, %8d, %8d, %8d, %8d, %8d \r\n", 8,
                    nvdm_EC_info_table_s[i+0],nvdm_EC_info_table_s[i+1],
                    nvdm_EC_info_table_s[i+2],nvdm_EC_info_table_s[i+3],
                    nvdm_EC_info_table_s[i+4],nvdm_EC_info_table_s[i+5],
                    nvdm_EC_info_table_s[i+6],nvdm_EC_info_table_s[i+7]);
        }

        LOGMSGIDI("\r\n NVDM EC dump start finished \r\n" , 0);

        LOGMSGIDI("NVDM EC dump end: \r\n", 0);
        for (i=0;i<NVDM_BLK_CNT;i=i+8) {
            LOGMSGIDI("%8d, %8d, %8d, %8d, %8d, %8d, %8d, %8d \r\n", 8,
                    nvdm_EC_info_table_e[i+0],nvdm_EC_info_table_e[i+1],
                    nvdm_EC_info_table_e[i+2],nvdm_EC_info_table_e[i+3],
                    nvdm_EC_info_table_e[i+4],nvdm_EC_info_table_e[i+5],
                    nvdm_EC_info_table_e[i+6],nvdm_EC_info_table_e[i+7]);
        }
        LOGMSGIDI("\r\n NVDM EC dump end finished \r\n", 0);
    }
#endif

    #if defined(MTK_FATFS_ON_SERIAL_NOR_FLASH)
    if (flag == 0) {
        LOGMSGIDI("FS EC dump from power on: \r\n", 0);
        for (i=0;i<FS_BLK_CNT;i=i+8) {
            LOGMSGIDI("%8d, %8d, %8d, %8d, %8d, %8d, %8d, %8d \r\n", 8,
                    FS_EC_info_table_e[i+0],FS_EC_info_table_e[i+1],
                    FS_EC_info_table_e[i+2],FS_EC_info_table_e[i+3],
                    FS_EC_info_table_e[i+4],FS_EC_info_table_e[i+5],
                    FS_EC_info_table_e[i+6],FS_EC_info_table_e[i+7]);
        }
        LOGMSGIDI("\r\n FS EC dump from power on finished \r\n", 0);
    } else {
        LOGMSGIDI("FS EC dump start: \r\n", 0);
        for (i=0;i<FS_BLK_CNT;i=i+8) {
            LOGMSGIDI("%8d, %8d, %8d, %8d, %8d, %8d, %8d, %8d \r\n", 8,
                    FS_EC_info_table_s[i+0],FS_EC_info_table_s[i+1],
                    FS_EC_info_table_s[i+2],FS_EC_info_table_s[i+3],
                    FS_EC_info_table_s[i+4],FS_EC_info_table_s[i+5],
                    FS_EC_info_table_s[i+6],FS_EC_info_table_s[i+7]);
        }
        LOGMSGIDI("\r\n FS EC dump start finished \r\n", 0);

        LOGMSGIDI("FS EC dump end: \r\n", 0);
        for (i=0;i<FS_BLK_CNT;i=i+8) {
            LOGMSGIDI("%8d, %8d, %8d, %8d, %8d, %8d, %8d, %8d \r\n", 8,
                    FS_EC_info_table_e[i+0],FS_EC_info_table_e[i+1],
                    FS_EC_info_table_e[i+2],FS_EC_info_table_e[i+3],
                    FS_EC_info_table_e[i+4],FS_EC_info_table_e[i+5],
                    FS_EC_info_table_e[i+6],FS_EC_info_table_e[i+7]);
        }
        LOGMSGIDI("\r\n FS EC dump end finished \r\n", 0);
    }
    #endif
}

void show_result(uint32_t flag, uint32_t duration)
{
    uint32_t i;
    uint32_t total_ec;
    #if defined(MTK_NVDM_ENABLE)
    uint32_t nv_max_ec = 0;
    #endif
    #if defined(MTK_FATFS_ON_SERIAL_NOR_FLASH)
    uint32_t fs_max_ec = 0;
    #endif

    if (flag == 0) {
        // it is result from frist power on
        LOGMSGIDI("Test from first power on:  \r\n", 0);
        #if defined(MTK_NVDM_ENABLE)
        total_ec = 0;
        for (i=0;i<NVDM_BLK_CNT;i++) {
            total_ec += nvdm_EC_info_table_e[i];
            
            if (nv_max_ec < nvdm_EC_info_table_e[i]) {
                nv_max_ec = nvdm_EC_info_table_e[i];
            }
        }
        LOGMSGIDI("NVDM Total erase count: 0x%x \r\n", 1, total_ec);
        #endif
        #if defined(MTK_FATFS_ON_SERIAL_NOR_FLASH)
        total_ec = 0;
        for (i=0;i<FS_BLK_CNT;i++) {
            total_ec += FS_EC_info_table_e[i];
            
            if (fs_max_ec < FS_EC_info_table_e[i]) {
                fs_max_ec = FS_EC_info_table_e[i];
            }
        }
        LOGMSGIDI("FS Total erase count: 0x%x \r\n", 1, total_ec);
        #endif
    } else {
        //test result in duration
        LOGMSGIDI("Test duration: %d second\r\n", 1, duration/1000);
        #if defined(MTK_NVDM_ENABLE)
        total_ec = 0;
        for (i=0;i<NVDM_BLK_CNT;i++) {
            if (nvdm_EC_info_table_s[i] != 0) {
                total_ec += (nvdm_EC_info_table_e[i] - nvdm_EC_info_table_s[i]);
            } else {
                total_ec += nvdm_EC_info_table_e[i];
            }
            if (nv_max_ec < nvdm_EC_info_table_e[i]) {
                nv_max_ec = nvdm_EC_info_table_e[i];
            }
        }
        LOGMSGIDI("NVDM Total erase count: 0x%x \r\n", 1, total_ec);
        LOGMSGIDI("NVDM MAX count: 0x%x \r\n", 1, nv_max_ec);
        #endif
        #if defined(MTK_FATFS_ON_SERIAL_NOR_FLASH)
        total_ec = 0;
        for (i=0;i<FS_BLK_CNT;i++) {
            if (FS_EC_info_table_s[i] != 0) {
                total_ec += (FS_EC_info_table_e[i] - FS_EC_info_table_s[i]);
            } else {
                total_ec += FS_EC_info_table_e[i];
            }
            if (fs_max_ec < FS_EC_info_table_e[i]) {
                fs_max_ec = FS_EC_info_table_e[i];
            }
        }

        LOGMSGIDI("FS Total erase count: 0x%x \r\n", 1, total_ec);
        LOGMSGIDI("FS MAX count: 0x%x \r\n", 1, fs_max_ec);
        #endif
    }

    {
        //test finished, reset default value
        sflash_EC_start = 0;
        ec_start_time = 0;
        ec_end_time = 0;
        ec_test_duration = 0;

        #if defined(MTK_NVDM_ENABLE)
        for (i=0;i<sizeof(NVDM_BLK_CNT);i++) {
            nvdm_EC_info_table_s[i] = 0;
            nvdm_EC_info_table_e[i] = 0;
        }
        #endif
        #if defined(MTK_FATFS_ON_SERIAL_NOR_FLASH)
        LOGMSGIDI("FS Total erase count: 0x%x \r\n", 1, total_ec);
        for (i=0;i<sizeof(FS_BLK_CNT);i++) {
            FS_EC_info_table_s[i] = 0;  //reset value
            FS_EC_info_table_e[i] = 0;
        }
        #endif
    }
}

// AT command handler
atci_status_t atci_cmd_hdlr_Sflash_lifecycle(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}};
    char cmd[256] = {0};
    uint8_t  i = 0;

    strncpy(cmd, (char *)parse_cmd->string_ptr, sizeof(cmd)-1);
    for (i = 0; i < strlen((char *)parse_cmd->string_ptr); i++) {
        cmd[i] = (char)toupper((unsigned char)cmd[i]);
    }

    LOGMSGIDI("tci_cmd_hdlr_Sflash_lifecycle \r\n", 0);

    response.response_flag = 0; // Command Execute Finish.
    #ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    response.cmd_id = parse_cmd->cmd_id;
    #endif

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+EREG=?
            strncpy((char*)response.response_buf, "+SFLASH=(0,1)\r\nOK\r\n", strlen("+SFLASH=(0,1)\r\nOK\r\n"));
            response.response_len = strlen((char*)response.response_buf);
            atci_send_response(&response);
            break;
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+EREG=<op>  the handler need to parse the parameters
            if (strncmp(cmd, "AT+SFLASH=EC_STA", strlen("AT+SFLASH=EC_STA")) == 0)
            {
                uint32_t pnum;
                /*command: AT+SFLASH=EC_STA : Start count*/
                sflash_EC_start = 1;   //EC start
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &ec_start_time);

#if defined(MTK_NVDM_ENABLE)
                {
                    peb_header_t peb_header;
                    for(pnum = 0; pnum < NVDM_BLK_CNT; pnum++) {
                        peb_read_header(pnum, &peb_header);
                        nvdm_EC_info_table_s[pnum] = peb_header.erase_count;
                        if (nvdm_EC_info_table_s[pnum] == 0xFFFFFFFF) {
                            nvdm_EC_info_table_s[pnum] = 0;
                        }
                    }
                }
#endif

#if defined(MTK_FATFS_ON_SERIAL_NOR_FLASH)
                {
                    BlockHeader header;
                    uint32_t address;
                    for(pnum = 0; pnum < FS_BLK_CNT; pnum++) {
                        address = ((FS_BASE + pnum * FS_NOR_BLOCK_SIZE) | SFC_GENERIC_FLASH_BANK_MASK);
                        #if defined(__NOR_FDM5__)
                        //refer to FDM5.0 spec:  
                        // verno(2B) + LBI (2B) + Block type (1B)  + EC (3B)
                        header.blockinfo = (*(volatile uint32_t *)(address + 4));
                        FS_EC_info_table_s[pnum] = (header.blockinfo >> 8);
                        #else
                        //   block type(1B) + EC (3B)
                        header.blockinfo = ((*(volatile uint32_t *)(address)) & 0xFFFFFF);
                        FS_EC_info_table_s[pnum] = header.blockinfo;
                        #endif
                        if (FS_EC_info_table_s[pnum]  == 0xFFFFFFFF || FS_EC_info_table_s[pnum] == 0xFFFFFF) {
                            FS_EC_info_table_s[pnum] = 0;
                        }
                    }
                }
#endif
                dump_ec(sflash_EC_start);
                
                /* ATCI will help append "OK" at the end of resonse buffer */
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                response.response_len = 0;
            }
            else if (strncmp(cmd, "AT+SFLASH=EC_END", strlen("AT+SFLASH=EC_END")) == 0)
            {
                /*command: AT+SFLASH=EC_END : end count */
                uint32_t pnum = 0;

                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &ec_end_time);

            #if defined(MTK_NVDM_ENABLE)
                {
                    peb_header_t peb_header;
                    for(pnum = 0; pnum < NVDM_BLK_CNT; pnum++) {
                        peb_read_header(pnum, &peb_header);
                        nvdm_EC_info_table_e[pnum] = peb_header.erase_count;
                        if (nvdm_EC_info_table_e[pnum] == 0xFFFFFFFF) {
                            nvdm_EC_info_table_e[pnum] = 0;
                        }
                    }
                }
            #endif

            #if defined(MTK_FATFS_ON_SERIAL_NOR_FLASH)
                {
                    BlockHeader header;
                    uint32_t pnum = 0;
                    uint32_t address;

                    for(pnum = 0; pnum < FS_BLK_CNT; pnum++) {
                        address = ((FS_BASE + pnum * FS_NOR_BLOCK_SIZE) | SFC_GENERIC_FLASH_BANK_MASK);;
                        #if defined(__NOR_FDM5__)
                        //refer to FDM5.0 spec:  
                        // verno(2B) + LBI (2B) + Block type (1B)  + EC (3B)
                        header.blockinfo = (*(volatile uint32_t *)(address + 4));
                        FS_EC_info_table_s[pnum] = (header.blockinfo >> 8);
                        #else
                        //   block type(1B) + EC (3B)
                        header.blockinfo = ((*(volatile uint32_t *)(address)) & 0xFFFFFF);
                        //LOGMSGIDI(" 000addr = 0x%x , 0x%x \r\n", 2, address, header.blockinfo);
                        FS_EC_info_table_e[pnum] = header.blockinfo;
                        #endif
                        if (FS_EC_info_table_e[pnum]  == 0xFFFFFFFF || FS_EC_info_table_e[pnum] == 0xFFFFFF) {
                            FS_EC_info_table_e[pnum] = 0;
                        }
                    }
                }
            #endif

                if (sflash_EC_start == 0) {
                    // it is result from frist power on
                    dump_ec(sflash_EC_start);
                    show_result(sflash_EC_start, 0);
                } else {
                    hal_gpt_get_duration_count(ec_start_time, ec_end_time,&ec_test_duration);
                    dump_ec(sflash_EC_start);
                    // it is a result of (s -e) duration
                    show_result(sflash_EC_start, ec_test_duration);
                }

                /* ATCI will help append "OK" at the end of resonse buffer    */
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                response.response_len = 0;
            }
            else
            {
                /*invalide parameter, return "ERROR"*/
                strncpy((char*)response.response_buf, "ERROR\r\n", strlen("ERROR\r\n"));
                response.response_len = strlen((char*)response.response_buf);
            };

            atci_send_response(&response);

            break;

        default :
            /* others are invalid command format */
            strncpy((char*)response.response_buf, "ERROR\r\n", strlen("ERROR\r\n"));
            response.response_len = strlen((char*)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}


#endif

