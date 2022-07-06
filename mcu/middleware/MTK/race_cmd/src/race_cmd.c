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


#include "race_cmd_feature.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hal_uart.h"
#include "syslog.h"
#include "atci_adapter.h"
#include "atci_main.h"
#include "race_cmd.h"
#include "race_xport.h"
#include "race_cmd_nvdm.h"
#include "race_cmd_bluetooth.h"
#include "race_cmd_ctrl_baseband.h"
#include "race_cmd_dsprealtime.h"
#include "race_cmd_fota.h"
#include "race_cmd_storage.h"
#include "race_cmd_ctrl_baseband.h"
#include "race_cmd_hostaudio.h"
#include "race_cmd_informational.h"
#include "race_cmd_bootreason.h"
#include "race_cmd_syslog.h"
#include "race_cmd_offline_log.h"
#include "race_cmd_online_log.h"
#include "race_util.h"
#include "race_cmd_captouch.h"
#include "race_cmd_register.h"
#include "race_cmd_rofs.h"
#include "race_cmd_system_power.h"
#include "race_cmd_system.h"
#include "race_cmd_version_code.h"
#include "race_cmd_i2c_master.h"
#include "race_cmd_spi_master.h"
#ifdef MTK_AUDIO_LOOPBACK_TEST_ENABLE
#include "race_cmd_audio_loopback_test.h"
#endif
//#include "at_command.h"
#include "memory_attribute.h"
#include <string.h>
#include <stdio.h>
#include "race_fota_util.h"
#ifdef RACE_FIND_ME_ENABLE
#include "race_cmd_find_me.h"
#endif
#if defined(RACE_RELAY_CMD_ENABLE) || defined(RACE_DUMMY_RELAY_CMD_ENABLE)
#include "race_cmd_relay_cmd.h"
#endif

#ifdef RACE_BT_CMD_ENABLE
#include "race_cmd_fcd_cmd.h"
#endif
#include "race_cmd_key_cmd.h"

#ifdef AIR_RACE_SCRIPT_ENABLE
#include "race_cmd_script.h"
#endif

#ifdef AIR_RACE_CO_SYS_ENABLE
#include "race_cmd_co_sys.h"
#endif

#ifdef RACE_CFU_ENABLE
#include "race_cmd_cfu.h"
#endif

#ifdef AIR_LE_AUDIO_ENABLE
#include "race_cmd_le_audio.h"
#endif

/* This range is reserved for customer use. */
#define RACE_ID_CUSTOM_BEGIN 0x0000
#define RACE_ID_CUSTOM_END 0x01FF

#define RACE_ID_FOTA_BEGIN 0x1C00
#define RACE_ID_FOTA_END 0x1C1F

#define RACE_ID_FLASH_BEGIN 0x702
#define RACE_ID_FLASH_END 0x70F

#define RACE_ID_NVKEY_BEGIN 0x0A00
#define RACE_ID_NVKEY_END 0x0AFF

#define RACE_ID_BLUETOOTH_BEGIN 0x0CC0
#define RACE_ID_BLUETOOTH_END 0x0CDF

#define RACE_ID_INFORMATIONAL_BEGIN 0x0300
#define RACE_ID_INFORMATIONAL_END   0x0301

#define RACE_ID_STORAGE_BEGIN  0x0400
#define RACE_ID_STORAGE_END  0x0433

#define RACE_ID_DSPREALTIME_BEGIN   (RACE_DSPREALTIME_BEGIN_ID)
#define RACE_ID_DSPREALTIME_END     (RACE_DSPREALTIME_END_ID)

#define RACE_ID_CTRL_BASEBAND_BEGIN 0x20C
#define RACE_ID_CTRL_BASEBAND_END 0x20D

#define RACE_ID_HOSTAUDIO_BEGIN 0x0900
#define RACE_ID_HOSTAUDIO_END 0x09FF

#define RACE_ID_AUDIO_LOOPBACK_TEST_BEGIN 0x1000
#define RACE_ID_AUDIO_LOOPBACK_TEST_END 0x100F

#define RACE_ID_SCRIPT_BEGIN 0x1120
#define RACE_ID_SCRIPT_END 0x112F

#define RACE_ID_CAPTOUCH_BEGIN 0x1600
#define RACE_ID_CAPTOUCH_END 0x167f

#define RACE_ID_RG_RW_BEGIN 0x1680
#define RACE_ID_RG_RW_END 0x16ff

#define RACE_ID_2WIRE_RG_RW_BEGIN 0x0210
#define RACE_ID_2WIRE_RG_RW_END 0x0211

#define RACE_ID_FIND_ME_BEGIN 0X2C00
#define RACE_ID_FIND_ME_END 0x2C01

#define RACE_ID_BOOTREASON_BEGIN 0x1E00
#define RACE_ID_BOOTREASON_END 0x1E02

#define RACE_ID_OFFLINE_LOG_BEGIN 0x1E03
#define RACE_ID_OFFLINE_LOG_END 0x1E07

//offline and online share 0x1E07 for assert
#define RACE_ID_ONLINE_LOG_BEGIN 0x1E08
#define RACE_ID_ONLINE_LOG_END 0x1E0E

#define RACE_ID_SYSLOG_START 0x0F14
#define RACE_ID_SYSLOG_END 0x0F20

#define RACE_ID_VERSION_CODE_BEGIN 0x0305
#define RACE_ID_VERSION_CODE_END 0x0307

#define RACE_ID_SPI_MASTER_BEGIN 0x5000
#define RACE_ID_SPI_MASTER_END 0x5000

#define RACE_ID_I2C_MASTER_BEGIN 0x5008
#define RACE_ID_I2C_MASTER_END 0x5008




#define RACE_ID_RELAY_BEGIN 0X0D00
#define RACE_ID_RELAY_END 0x0D01
#define RACE_ID_ROFS_BEGIN 0x1D00
#define RACE_ID_ROFS_END 0x1D0F

#define RACE_ID_RSSI_START 0x1700
#define RACE_ID_RSSI_END 0x1700

#define RACE_ID_FCD_START 0x1F00
#define RACE_ID_FCD_END 0x1F3F

#define RACE_ID_KEY_START 0x1100
#define RACE_ID_KEY_END 0x1101

#define RACE_ID_COSYS_START 0x110F
#define RACE_ID_COSYS_END 0x110F

#define RACE_ID_SYS_PWR_START 0x1110
#define RACE_ID_SYS_PWR_END 0x111F

#define RACE_ID_SYSTEM_BEGIN 0x0200
#define RACE_ID_SYSTEM_END 0x0201

#define RACE_ID_SLEEP_CONTROL_BEGIN 0x0220
#define RACE_ID_SLEEP_CONTROL_END 0x0221

#define RACE_ID_CHARGER_BEGIN 0x0240
#define RACE_ID_CHARGER_END 0x024F

#define RACE_ID_REG_I2C_CONTROL_BEGIN 0x020E
#define RACE_ID_REG_I2C_CONTROL_END 0x020F

#define RACE_ID_CFU_BEGIN 0x2E00
#define RACE_ID_CFU_END 0x2E0F

#define RACE_ID_LE_AUDIO_BEGIN 0x2200
#define RACE_ID_LE_AUDIO_END 0x22FF

const RACE_HANDLER race_handlers[] = {
#ifdef RACE_NVDM_CMD_ENABLE
    {RACE_ID_NVKEY_BEGIN, RACE_ID_NVKEY_END, RACE_CmdHandler_NVDM},
#endif
    {RACE_ID_INFORMATIONAL_BEGIN, RACE_ID_INFORMATIONAL_END, RACE_CmdHandler_INFORMATION},

#ifdef RACE_FOTA_CMD_ENABLE
    {RACE_ID_FOTA_BEGIN, RACE_ID_FOTA_END, RACE_CmdHandler_FOTA},
#endif

#ifdef RACE_BT_CMD_ENABLE
    {RACE_ID_BLUETOOTH_BEGIN, RACE_ID_BLUETOOTH_END, RACE_CmdHandler_BLUETOOTH},
#endif

#ifdef RACE_STORAGE_CMD_ENABLE
    {RACE_ID_STORAGE_BEGIN, RACE_ID_STORAGE_END, race_cmdhdl_storage},
#endif

#ifdef RACE_DSP_REALTIME_CMD_ENABLE
    {RACE_ID_DSPREALTIME_BEGIN, RACE_ID_DSPREALTIME_END, RACE_CmdHandler_DSPREALTIME},
#endif

#ifdef RACE_CTRL_BASEBAND_CMD_ENABLE
    {RACE_ID_CTRL_BASEBAND_BEGIN, RACE_ID_CTRL_BASEBAND_END, RACE_CmdHandler_CTRL_BASEBAND},
#endif

#ifdef RACE_HOSTAUDIO_CMD_ENABLE
    {RACE_ID_HOSTAUDIO_BEGIN, RACE_ID_HOSTAUDIO_END, RACE_CmdHandler_HOSTAUDIO},
#endif

#ifdef RACE_CAPTOUCH_CMD_ENABLE
    {RACE_ID_CAPTOUCH_BEGIN, RACE_ID_CAPTOUCH_END, RACE_CmdHandler_captouch},
#endif

#ifdef RACE_RG_READ_WRITE_ENABLE
    {RACE_ID_RG_RW_BEGIN, RACE_ID_RG_RW_END, RACE_CmdHandler_RG_read_write},
    {RACE_ID_2WIRE_RG_RW_BEGIN, RACE_ID_2WIRE_RG_RW_END, RACE_CmdHandler_2wire_RG_read_write},
#endif

#ifdef RACE_FIND_ME_ENABLE
    {RACE_ID_FIND_ME_BEGIN, RACE_ID_FIND_ME_END, RACE_CmdHandler_FIND_ME},
#endif

#ifdef RACE_BOOTREASON_CMD_ENABLE
    {RACE_ID_BOOTREASON_BEGIN, RACE_ID_BOOTREASON_END, RACE_CmdHandler_bootreason},
#endif

#ifdef RACE_OFFLINE_LOG_CMD_ENABLE
    {RACE_ID_OFFLINE_LOG_BEGIN, RACE_ID_OFFLINE_LOG_END, RACE_CmdHandler_offline_log},
#endif

#ifdef RACE_SYSLOG_CMD_ENABLE
    {RACE_ID_SYSLOG_START, RACE_ID_SYSLOG_END, RACE_CmdHandler_syslog},
#endif

#ifdef RACE_VERSION_CODE_CMD_ENABLE
        {RACE_ID_VERSION_CODE_BEGIN, RACE_ID_VERSION_CODE_END, RACE_CmdHandler_version_code},
#endif

#ifdef RACE_I2C_MASTER_CMD_ENABLE
            {RACE_ID_I2C_MASTER_BEGIN, RACE_ID_I2C_MASTER_END, RACE_CmdHandler_i2c_master_send_and_receive},
#endif
#ifdef RACE_SPI_MASTER_CMD_ENABLE
            {RACE_ID_SPI_MASTER_BEGIN, RACE_ID_SPI_MASTER_END, RACE_CmdHandler_spi_master_send_and_receive},
#endif


#ifdef RACE_ONLINE_LOG_CMD_ENABLE
    {RACE_ID_ONLINE_LOG_BEGIN, RACE_ID_ONLINE_LOG_END, RACE_CmdHandler_online_log},
#else
#ifdef MTK_MUX_ENABLE
    {RACE_ID_ONLINE_LOG_BEGIN, RACE_ID_ONLINE_LOG_END, RACE_CmdHandler_online_log_2},
#endif
#endif

#if defined(RACE_RELAY_CMD_ENABLE) || defined(RACE_DUMMY_RELAY_CMD_ENABLE)
    {RACE_ID_RELAY_BEGIN, RACE_ID_RELAY_END, RACE_CmdHandler_RELAY_RACE_CMD},
#endif
#ifdef RACE_ROFS_CMD_ENABLE
    {RACE_ID_ROFS_BEGIN, RACE_ID_ROFS_END, RACE_CmdHandlerROFS},
#endif
#ifdef RACE_BT_CMD_ENABLE
    {RACE_ID_RSSI_START, RACE_ID_RSSI_END, RACE_CmdHandler_GET_RSSI},
    {RACE_ID_FCD_START, RACE_ID_FCD_END, RACE_CmdHandler_FCD},
#endif
    {RACE_ID_KEY_START, RACE_ID_KEY_END, RACE_CmdHandler_KEY},
#ifdef AIR_RACE_CO_SYS_ENABLE
    {RACE_ID_COSYS_START, RACE_ID_COSYS_END, RACE_CmdHandler_co_sys},
#endif
    {RACE_ID_SYS_PWR_START, RACE_ID_SYS_PWR_END, RACE_CmdHandler_SYS_PWR},
    {RACE_ID_SYSTEM_BEGIN, RACE_ID_SYSTEM_END, RACE_CmdHandler_System},
    {RACE_ID_SLEEP_CONTROL_BEGIN, RACE_ID_SLEEP_CONTROL_END, RACE_CmdHandler_System},
    {RACE_ID_REG_I2C_CONTROL_BEGIN, RACE_ID_REG_I2C_CONTROL_END, RACE_CmdHandler_System},
    {RACE_ID_CHARGER_BEGIN, RACE_ID_CHARGER_END, RACE_CmdHandler_System},
#ifdef MTK_AUDIO_LOOPBACK_TEST_ENABLE
    {RACE_ID_AUDIO_LOOPBACK_TEST_BEGIN, RACE_ID_AUDIO_LOOPBACK_TEST_END, RACE_CmdHandler_audio_loopback_test},
#endif
#ifdef AIR_RACE_SCRIPT_ENABLE
    {RACE_ID_SCRIPT_BEGIN, RACE_ID_SCRIPT_END, RACE_CmdHandler_Script},
#endif
#ifdef RACE_CFU_ENABLE
    {RACE_ID_CFU_BEGIN, RACE_ID_CFU_END, RACE_CmdHandler_Cfu},
#endif

#ifdef AIR_LE_AUDIO_ENABLE
    {RACE_ID_LE_AUDIO_BEGIN, RACE_ID_LE_AUDIO_END, RACE_CmdHandler_LE_AUDIO},
#endif
};


/*******************************************************************************/
/*                      Global Variables                                      */
/*******************************************************************************/
//static Handler app_race_handler = NULL;
static uint32_t g_race_registered_table_number;
static RACE_HANDLER g_race_cm4_general_hdlr_tables[RACE_MAX_GNENERAL_TABLE_NUM];

void *RACE_ClaimPacket(uint8_t race_type, uint16_t race_id, uint16_t dat_len, uint8_t channel_id)
{
    race_send_pkt_t *pPacket = NULL;
    pPacket = (race_send_pkt_t *)race_mem_alloc(sizeof(race_send_pkt_t) + dat_len);
    RACE_LOG_MSGID_I("RACE_ClaimPacket, race_type[0x%X], race_id[0x%X], dat_len[%d], channel_id[%d], pPacket[0x%X]", 5,
               race_type, race_id, dat_len, channel_id, pPacket);

    if (pPacket != NULL) {
        pPacket->channel_id = channel_id;
        pPacket->length = sizeof(RACE_COMMON_HDR_STRU) + dat_len;
        pPacket->offset = 6;//OS_OFFSET_OF(RACE_IPC_STRU, payload);
        pPacket->reserve = 0xCC;

        pPacket->race_data.hdr.pktId.value = 0x05;
        pPacket->race_data.hdr.type = race_type;
        pPacket->race_data.hdr.length = sizeof(uint16_t) + dat_len;
        pPacket->race_data.hdr.id = race_id;

        return pPacket->race_data.payload;
    } else {
        return NULL;
    }
}

void *RACE_ClaimPacketAppID(uint8_t app_id, uint8_t race_type, uint16_t race_id, uint16_t dat_len, uint8_t channel_id)
{
    race_send_pkt_t *pPacket = NULL;
    pPacket = (race_send_pkt_t *)race_mem_alloc(sizeof(race_send_pkt_t) + dat_len);
    RACE_LOG_MSGID_I("RACE_ClaimPacketAppID, race_type[0x%X], race_id[0x%X], dat_len[%d], channel_id[%d], pPacket[0x%X]", 5,
               race_type, race_id, dat_len, channel_id, pPacket);

    if (pPacket != NULL) {
        pPacket->channel_id = channel_id;
        pPacket->length = sizeof(RACE_COMMON_HDR_STRU) + dat_len;
        pPacket->offset = 6;//OS_OFFSET_OF(RACE_IPC_STRU, payload);
        pPacket->reserve = 0xCC;

        pPacket->race_data.hdr.pktId.value = ((app_id << 4) | 0x05);
        pPacket->race_data.hdr.type = race_type;
        pPacket->race_data.hdr.length = sizeof(uint16_t) + dat_len;
        pPacket->race_data.hdr.id = race_id;

        return pPacket->race_data.payload;
    } else {
        return NULL;
    }
}


/* Input the pointer returned by RACE_ClaimPacket() or RACE_ClaimPacketAppID() */
void RACE_FreePacket(void *data)
{
    race_send_pkt_t *send_pkt = NULL;

    /* Convert payload pointer to the pointer points to the begining of the whole package. */
    send_pkt = race_pointer_cnv_pkt_to_send_pkt(data);

    RACE_LOG_MSGID_I("RACE_FreePacket, send_pkt[0x%X]", 1, send_pkt);
    race_mem_free(send_pkt);
}

race_status_t RACE_Register_Handler(RACE_HANDLER *pHandler)
{
    if(g_race_registered_table_number == RACE_MAX_GNENERAL_TABLE_NUM)
        return RACE_STATUS_REGISTRATION_FAILURE;
    else if(!pHandler)
        return RACE_STATUS_ERROR;

    memcpy(&g_race_cm4_general_hdlr_tables[g_race_registered_table_number], pHandler, sizeof(RACE_HANDLER));
    g_race_registered_table_number++;

    return RACE_STATUS_OK;
}

#if (RACE_DEBUG_PRINT_ENABLE)
void race_dump(uint8_t *data, race_debug_type_enum type)
{
    uint16_t length = (data[3]<<8) + data[2] + 4;
    if (length > 25)
        length = 25;

    switch(type)
    {
        case RACE_DBG_CMD:
            LOG_HEXDUMP_I(race, "CMD", data, length);
        break;

        case RACE_DBG_EVT:
            LOG_HEXDUMP_I(race, "EVT", data, length);
        break;

        case RACE_DBG_EVT_APP:
            LOG_HEXDUMP_I(race, "EVT_APP", data, length);
        break;

        case RACE_DBG_IF_RELAY:
            LOG_HEXDUMP_I(race, "IF_RELAY", data, length);
        break;

        case RACE_DBG_FLUSH:
            LOG_HEXDUMP_I(race, "FLUSH", data, length);
        break;

        default:
        break;
    }
}
#endif


void *RACE_CmdHandler(race_pkt_t *pMsg, uint8_t channel_id)
{
    uint32_t i;

    void *ptr = NULL;

    if (!pMsg) {
        return NULL;
    }
    RACE_LOG_MSGID_I("RACE_CmdHandler, type[0x%X], id[0x%X], app_id[%d]", 3, pMsg->hdr.type, pMsg->hdr.id, pMsg->hdr.pktId.field.app_id);

#if (RACE_DEBUG_PRINT_ENABLE)
    race_dump((uint8_t *)pMsg, RACE_DBG_CMD);
#endif

    switch (pMsg->hdr.type) {
        case RACE_TYPE_RESPONSE:
        case RACE_TYPE_NOTIFICATION:
            if (race_get_port_type_by_channel_id(channel_id) == RACE_SERIAL_PORT_TYPE_UART)
                break;
        case RACE_TYPE_COMMAND:
        case RACE_TYPE_COMMAND_WITHOUT_RSP: {
            for (i = 0; i < sizeof(race_handlers) / sizeof(RACE_HANDLER); i++) {
                if (pMsg->hdr.id >= race_handlers[i].id_start && pMsg->hdr.id <= race_handlers[i].id_end) {
#ifdef RACE_FOTA_CMD_ENABLE
                    if (RACE_APP_ID_FOTA == pMsg->hdr.pktId.field.app_id) {
                        RACE_ERRCODE ret = race_fota_cmd_preprocess(pMsg->hdr.id,
                                           pMsg->hdr.type,
                                           channel_id);
                        if (RACE_ERRCODE_SUCCESS != ret) {
                            break;
                        }
                    }
#endif /* RACE_FOTA_CMD_ENABLE */

                    ptr = race_handlers[i].handler(pMsg, pMsg->hdr.length, channel_id);

                    if (ptr) {
                        ptr = (void *)race_pointer_cnv_pkt_to_send_pkt(ptr);
#if (RACE_DEBUG_PRINT_ENABLE)
                        race_pkt_t      *pret;
                        race_send_pkt_t *psend;
                        psend = (race_send_pkt_t *)ptr;
                        pret = &psend->race_data;
                        race_dump((uint8_t *)pret, RACE_DBG_EVT);
#endif
                    }
                    else if (pMsg->hdr.type == RACE_TYPE_COMMAND) {
                        RACE_LOG_MSGID_W("RACE_CmdHandler, evt ptr null, id[0x%x]", 1, pMsg->hdr.id);
                    }
                    break;
                }
            }
            if (i == sizeof(race_handlers) / sizeof(RACE_HANDLER)) { //not found
                for(i = 0; i < g_race_registered_table_number; i++) {
                    if (pMsg->hdr.id >= g_race_cm4_general_hdlr_tables[i].id_start && pMsg->hdr.id <= g_race_cm4_general_hdlr_tables[i].id_end) {

                        ptr = g_race_cm4_general_hdlr_tables[i].handler(pMsg, pMsg->hdr.length, channel_id);

                        if (ptr) {
                            ptr = (void *)race_pointer_cnv_pkt_to_send_pkt(ptr);
#if (RACE_DEBUG_PRINT_ENABLE)
                            race_pkt_t      *pret;
                            race_send_pkt_t *psend;
                            psend = (race_send_pkt_t *)ptr;
                            pret = &psend->race_data;
                            race_dump((uint8_t *)pret, RACE_DBG_EVT_APP);
#endif
                        }
                        break;
                    }
                }

                if (i == g_race_registered_table_number) {
                    RACE_LOG_MSGID_E("RACE_CmdHandler, handler not found, id[0x%x]", 1, pMsg->hdr.id);
                }
            }
#if 0
            if (i == sizeof(race_handlers) / sizeof(RACE_HANDLER)) { //not found
                if (app_race_handler) {
                    RACE_Send2Handler(pMsg, app_race_handler);
                } else {
                    PTR_RACE_PAYLOAD_STRU payload;
                    uint8_t *cptr = RACE_ClaimPacket(RACE_TYPE_RESPONSE, pRaceHeaderCmd->id, sizeof(uint8_t), pMsg->channel_id);
                    cptr[0] = RACE_ERRCODE_NOT_SUPPORT;
                    //PTR_RACE_IPC_STRU packet;
                    payload = OS_CONTAINER_OF(cptr, RACE_PAYLOAD_STRU, param);
                    ptr = (void *)OS_CONTAINER_OF(payload, RACE_IPC_STRU, payload);
                }
            }
#endif
            break;
        }

        default: {
            break;
        }
    }

    // We should free the memory.
    //OSMEM_Put(pMsg);
    return ptr;
}

bool race_cmd_is_to_remote(race_pkt_t *pMsg)
{
    bool ret = false;

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
    if (pMsg->hdr.id == 0x2c82 || pMsg->hdr.id == 0x2c83) {
        if ((pMsg->payload[0] == 0x00) || (pMsg->payload[0] == 0x01)) {
            RACE_LOG_MSGID_I("race_cosys slave APP, id[0x%x], module[%d]", 2, pMsg->hdr.id, pMsg->payload[0]);
            ret = true;
        }
    }
#endif

    return ret;
}
