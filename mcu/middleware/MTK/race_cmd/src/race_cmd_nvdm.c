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
#ifdef RACE_NVDM_CMD_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "serial_port.h"
#include "hal.h"
#include "nvdm.h"
#include "nvdm_internal.h"
#include "atci.h"
#include "atci_main.h"
#include "race_xport.h"
#include "race_cmd_nvdm.h"
#include "race_event_internal.h"
#include "race_util.h"
#include "serial_port_assignment.h"
#include "bt_system.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "bt_connection_manager.h"
#include "bt_connection_manager_internal.h"
#include "bt_callback_manager.h"
#include "hal_sleep_manager.h"
#include "at_command_bt.h"
#include "bt_sink_srv_ami.h"

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RACE_GROUP_NAME "AB15"
#define RACE_ITEM_NAME_SIZE 2

////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
enum
{
    RACE_SWITCH_TO_AT,
    RACE_SWITCH_TO_RELAY,
    RACE_SWITCH_TO_LOG,
    RACE_SWITCH_TO_HCI,
};

typedef struct
{
    RACE_COMMON_HDR_STRU Hdr;
    uint16_t NVKEY_ID;
    uint8_t WriteData[0];
}PACKED RACE_NVKEY_WRITEFULLKEY_RESP_NVID_CMD_STRU;

typedef struct
{
    uint8_t return_code;
    uint16_t NVKEY_ID;
}PACKED RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU;


typedef struct
{
    RACE_COMMON_HDR_STRU Hdr;
    uint16_t NVKEY_ID;
    uint8_t WriteData[0];
}PACKED RACE_NVKEY_WRITEFULLKEY_CMD_STRU;

typedef struct
{
    uint8_t return_code;
}PACKED RACE_NVKEY_WRITEFULLKEY_EVT_STRU;


#ifdef AIR_RACE_CO_SYS_ENABLE

#include "race_cmd_co_sys.h"

typedef struct {
    uint16_t nvkey_id;
    uint16_t data_len;
    uint8_t data[1024];
} sync_nvkey_t;


static uint8_t s_channel_id;
static volatile uint8_t s_co_sys_write_status = 0xFF;

/* 0 - 0x0A01
 * 1 - 0x0A0D
 */
static sync_nvkey_t s_data_for_sync[2];
static RACE_NVKEY_WRITEFULLKEY_EVT_STRU* s_pEvt_without_id_rsp = NULL;
static RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU* s_pEvt_with_id_rsp = NULL;


void race_cosys_nvkey_data_sync(bool is_critical, uint8_t *buff, uint32_t len)
{
    is_critical = is_critical;

    uint16_t event_id = ((RACE_COMMON_HDR_STRU*)(buff))->id;
    uint32_t data_len = ((RACE_COMMON_HDR_STRU*)(buff))->length;
    RACE_LOG_MSGID_I("race_cosys_nvkey_data_sync recieve event id: 0x%X, len: %d", 2, event_id, data_len);

    switch(event_id){
        case RACE_NVKEY_WRITEFULLKEY:{
                if(data_len == 1){
                    RACE_NVKEY_WRITEFULLKEY_EVT_STRU* pCmd = (RACE_NVKEY_WRITEFULLKEY_EVT_STRU*)(buff + sizeof(RACE_COMMON_HDR_STRU));
                    s_co_sys_write_status = pCmd->return_code;
                    RACE_LOG_MSGID_I("race_cosys_nvkey_data_sync recieve response: %d", 1, s_co_sys_write_status);

                    s_pEvt_without_id_rsp->return_code = s_co_sys_write_status;
                    if(s_co_sys_write_status != 0x0){
                        RACE_LOG_MSGID_E("cosys write failed with status %u.", 1, s_co_sys_write_status);
                        s_co_sys_write_status = 0xFF;
                        race_flush_packet((uint8_t*)s_pEvt_without_id_rsp, s_channel_id);
                        s_pEvt_without_id_rsp = NULL;
                        return;
                    }
                    s_co_sys_write_status = 0xFF;
                    nvkey_write_data(s_data_for_sync[0].nvkey_id, s_data_for_sync[0].data, s_data_for_sync[0].data_len);
                    race_flush_packet((uint8_t*)s_pEvt_without_id_rsp, s_channel_id);
                    s_pEvt_without_id_rsp = NULL;
                    RACE_LOG_MSGID_I("cosys write nvkey success", 0);
                } else {
                    RACE_NVKEY_WRITEFULLKEY_CMD_STRU* pCmd = (RACE_NVKEY_WRITEFULLKEY_CMD_STRU*)buff;
                    data_len = pCmd->Hdr.length - sizeof(pCmd->Hdr.id) - sizeof(pCmd->NVKEY_ID);
                    nvkey_status_t status = nvkey_write_data(pCmd->NVKEY_ID, pCmd->WriteData, data_len);
                    pCmd->Hdr.length = 1;
                    if(status != NVKEY_STATUS_OK){
                        ((RACE_NVKEY_WRITEFULLKEY_EVT_STRU*)((uint8_t*)pCmd + sizeof(RACE_COMMON_HDR_STRU)))->return_code = 1;
                        RACE_LOG_MSGID_E("race_cosys_nvkey_data_sync fail %d", 1, status);
                    } else {
                        RACE_LOG_MSGID_I("race_cosys_nvkey_data_sync success 0x%4X", 1, pCmd->NVKEY_ID);
                        ((RACE_NVKEY_WRITEFULLKEY_EVT_STRU*)((uint8_t*)pCmd + sizeof(RACE_COMMON_HDR_STRU)))->return_code = 0;
                    }
                    bool send_resp_succ = race_cosys_send_data(RACE_COSYS_MODULE_ID_NVKEY, false,
                                            (uint8_t*)(pCmd),
                                            sizeof(RACE_COMMON_HDR_STRU) + sizeof(RACE_NVKEY_WRITEFULLKEY_EVT_STRU));
                    if(send_resp_succ != true){
                        RACE_LOG_MSGID_E("race_cosys_send_data fail.", 0);
                    }
                }
            }
            break;
        case RACE_NVKEY_WRITEFULLKEY_RESP_NVID:{
                if(data_len == 3){
                    RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU* pCmd = (RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU*)(buff + sizeof(RACE_COMMON_HDR_STRU));
                    s_co_sys_write_status = pCmd->return_code;
                    RACE_LOG_MSGID_I("race_cosys_nvkey_data_sync recieve response(0x%X): %d", 2, pCmd->NVKEY_ID, s_co_sys_write_status);

                    s_pEvt_with_id_rsp->return_code = s_co_sys_write_status;
                    s_pEvt_with_id_rsp->NVKEY_ID = pCmd->NVKEY_ID;
                    if(s_co_sys_write_status != 0x0){
                        RACE_LOG_MSGID_E("cosys write nvkey 0x%X failed with status %u.", 2, pCmd->NVKEY_ID, s_co_sys_write_status);
                        s_co_sys_write_status = 0xFF;
                        race_flush_packet((uint8_t*)s_pEvt_with_id_rsp, s_channel_id);
                        s_pEvt_with_id_rsp = NULL;
                        return;
                    }
                    s_co_sys_write_status = 0xFF;
                    nvkey_write_data(s_data_for_sync[1].nvkey_id, s_data_for_sync[1].data, s_data_for_sync[1].data_len);
                    race_flush_packet((uint8_t*)s_pEvt_with_id_rsp, s_channel_id);
                    RACE_LOG_MSGID_I("cosys write nvkey 0x%X success", 1, pCmd->NVKEY_ID);
                    s_pEvt_with_id_rsp = NULL;
                } else {
                    RACE_NVKEY_WRITEFULLKEY_RESP_NVID_CMD_STRU* pCmd = (RACE_NVKEY_WRITEFULLKEY_RESP_NVID_CMD_STRU*)buff;
                    data_len = pCmd->Hdr.length - sizeof(pCmd->Hdr.id) - sizeof(pCmd->NVKEY_ID);
                    nvkey_status_t status = nvkey_write_data(pCmd->NVKEY_ID, pCmd->WriteData, data_len);
                    pCmd->Hdr.length = 3;
                    ((RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU*)((uint8_t*)pCmd + sizeof(RACE_COMMON_HDR_STRU)))->NVKEY_ID = pCmd->NVKEY_ID;
                    if(status != NVKEY_STATUS_OK){
                        ((RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU*)((uint8_t*)pCmd + sizeof(RACE_COMMON_HDR_STRU)))->return_code = 1;
                        RACE_LOG_MSGID_E("race_cosys_nvkey_data_sync fail %d", 1, status);
                    } else {
                        ((RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU*)((uint8_t*)pCmd + sizeof(RACE_COMMON_HDR_STRU)))->return_code = 0;
                        RACE_LOG_MSGID_I("race_cosys_nvkey_data_sync success 0x%04X", 1, pCmd->NVKEY_ID);
                    }
                    bool send_resp_succ = race_cosys_send_data(RACE_COSYS_MODULE_ID_NVKEY, false,
                                            (uint8_t*)(pCmd),
                                            sizeof(RACE_COMMON_HDR_STRU) + sizeof(RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU));
                    if(send_resp_succ != true){
                        RACE_LOG_MSGID_E("race_cosys_send_data fail.", 0);
                    }
                }
            }
            break;
    }
}

void race_cosys_nvkey_init(void)
{
    race_cosys_register_data_callback(RACE_COSYS_MODULE_ID_NVKEY, race_cosys_nvkey_data_sync);
}


const uint16_t nvkey_sync_range[][2] = {
    /* nvkey range */
    { 0xF233, 0xF239 },     /* NVKEYID_PEQ_AUDIO_PATH_TABLE */
    { 0xF260, 0xF27F },     /* NVKEYID_PEQ_PARAMETER */
    { 0xEF00, 0xEF03 },     /* NVKEYID_UI_PEQ */
    { 0xEF10, 0xEF13 },     /* NVKEYID_UI_PEQ */
    { 0xFFFF, 0xFFFF },
};
uint32_t nvkey_need_sync(uint16_t nvkey)
{
    uint32_t idx, idx_max = sizeof(nvkey_sync_range)/(sizeof(uint16_t)*2);
    uint16_t l, r;
    for(idx = 0; idx < idx_max; idx++){
        l = nvkey_sync_range[idx][0];
        r = nvkey_sync_range[idx][1];
        if((l<= nvkey) && (nvkey <= r)){
            return 1;
        }
    }
    return 0;
}
#endif


////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void* RACE_NVKEY_READFULLKEY_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("RACE_NVKEY_READFULLKEY_HDR, channel_id[%d]", 1, channel_id);

    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint16_t NVKEY_ID;
        uint16_t length_of_read_bytes;
    }PACKED RACE_NVKEY_READFULLKEY_CMD_STRU;

    typedef struct
    {
        uint16_t length_of_read_bytes;
        uint8_t read_data[0];
    }PACKED RACE_NVKEY_READFULLKEY_EVT_STRU;

    RACE_NVKEY_READFULLKEY_CMD_STRU* pCmd = (RACE_NVKEY_READFULLKEY_CMD_STRU*)pCmdMsg;
    RACE_NVKEY_READFULLKEY_EVT_STRU* pEvt = NULL;
    uint32_t size = 0;
    nvkey_status_t status = NVKEY_STATUS_ERROR;

    status = nvkey_data_item_length(pCmd->NVKEY_ID, &size);
    if ((status != NVKEY_STATUS_OK) || (size == 0))
    {
        RACE_LOG_MSGID_E("nvkey_data_item_length fial, nvkey_id[0x%x], status[%d], nvkey_length[%d]", 3, pCmd->NVKEY_ID, status, size);
    }
    else
    {
        pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_NVKEY_READFULLKEY,
                                                                 (uint16_t)(sizeof(RACE_NVKEY_READFULLKEY_EVT_STRU) + size), channel_id);
        if (pEvt)
        {
            status = nvkey_read_data(pCmd->NVKEY_ID, (uint8_t*)pEvt->read_data, &size);
            if (status == NVKEY_STATUS_ERROR)
            {
                RACE_FreePacket(pEvt);
                RACE_LOG_MSGID_E("nvkey_read_data fail, nvkey_id[0x%x]", 1, pCmd->NVKEY_ID);
            }
            else
            {
                pEvt->length_of_read_bytes = size;
                return pEvt;
            }
        }
        else
        {
            RACE_LOG_MSGID_E("RACE_ClaimPacket fail, nvkey_id[0x%x]", 1, pCmd->NVKEY_ID);
        }
    }

    pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_NVKEY_READFULLKEY,
        (uint16_t)(sizeof(RACE_NVKEY_READFULLKEY_EVT_STRU) + 1), channel_id);

    if (pEvt != NULL)
    {
        pEvt->length_of_read_bytes = 0;
        *((uint8_t *)pEvt->read_data) = 0;
    }
    else
    {
        RACE_LOG_MSGID_E("pEvt race_mem_alloc fail", 0);
        pEvt = NULL;
    }

    return pEvt;
}

void* RACE_NVKEY_READFULLKEY_RESP_NVID_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("RACE_NVKEY_READFULLKEY_RESP_NVID_HDR, channel_id[%d]", 1, channel_id);

    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint16_t NVKEY_ID;
        uint16_t length_of_read_bytes;
    }PACKED RACE_NVKEY_READFULLKEY_RESP_NVID_CMD_STRU;

    typedef struct
    {
        int8_t ReturnCode;
        uint16_t NVKEY_ID;
        uint16_t length_of_read_bytes;
        uint8_t ReadData[0];
    }PACKED RACE_NVKEY_READFULLKEY_RESP_NVID_EVT_STRU;

    uint32_t        size;
    nvdm_status_t status;
    RACE_NVKEY_READFULLKEY_RESP_NVID_EVT_STRU* pEvt = NULL;

    RACE_NVKEY_READFULLKEY_RESP_NVID_CMD_STRU* pCmd = (RACE_NVKEY_READFULLKEY_RESP_NVID_CMD_STRU*)pCmdMsg;
    status = nvkey_data_item_length(pCmd->NVKEY_ID, &size);
    if (status == NVDM_STATUS_OK)
    {
        pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_NVKEY_READFULLKEY_RESP_NVID,
                                                                 (uint16_t)(sizeof(RACE_NVKEY_READFULLKEY_RESP_NVID_EVT_STRU) + size), channel_id);
        if (pEvt != NULL)
        {
            status = nvkey_read_data(pCmd->NVKEY_ID, (uint8_t*)pEvt->ReadData, &size);
            if (status == NVDM_STATUS_OK)
            {
                pEvt->length_of_read_bytes = size;
                pEvt->ReturnCode = 0;
                pEvt->NVKEY_ID = pCmd->NVKEY_ID;

                /*if(size > 1024)
                {
                    race_send_pkt_t* pSndPkt;
                    uint32_t port_handle, ret_size;
                    uint8_t *ptr;

                    pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pEvt);
                    port_handle = race_get_port_handle_by_channel_id(channel_id);
                    ret_size = race_port_send_data(port_handle, (uint8_t*)&pSndPkt->race_data, pSndPkt->length);
                    size = pSndPkt->length;
                    size -= ret_size;
                    ptr = (uint8_t*)&pSndPkt->race_data;
                    ptr += ret_size;
                    while(size > 0)
                    {
                        ret_size = race_port_send_data(port_handle, ptr, size);
                        size -= ret_size;
                        ptr += ret_size;
                    }
                    race_mem_free(pSndPkt);
                    return NULL;
                }*/
            }
            else
            {
                RACE_LOG_MSGID_E("nvkey_read_data fail, status[%d]", 1, status);
                pEvt->length_of_read_bytes = 0;
                pEvt->ReturnCode = -1;
                pEvt->NVKEY_ID = pCmd->NVKEY_ID;
            }
        }
    }
    /*else if (status == NVDM_STATUS_ITEM_NOT_FOUND)
    {
        pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_NVKEY_READFULLKEY_RESP_NVID,
                                                                 (uint16_t)(sizeof(RACE_NVKEY_READFULLKEY_RESP_NVID_EVT_STRU) + 1), channel_id);
        if (pEvt != NULL)
        {
            pEvt->length_of_read_bytes = 0;
            pEvt->ReturnCode = 0;
            pEvt->NVKEY_ID = pCmd->NVKEY_ID;
            *((uint8_t *)pEvt->ReadData) = 0;
        }
        RACE_LOG_MSGID_E("nvkey_data_item_length, NVDM_STATUS_ITEM_NOT_FOUND add 1 byte zero, status [%d]", 1, status);
    }*/
    else
    {
        size = 0;
        pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_NVKEY_READFULLKEY_RESP_NVID,
                                                                 (uint16_t)(sizeof(RACE_NVKEY_READFULLKEY_RESP_NVID_EVT_STRU) + size), channel_id);
        if (pEvt != NULL){
            pEvt->length_of_read_bytes = 0;
            pEvt->ReturnCode = (int8_t)status;
            pEvt->NVKEY_ID = pCmd->NVKEY_ID;
            RACE_LOG_MSGID_W("nvkey_data_item_length, status[%d]", 1, status);
        } else {
            RACE_LOG_MSGID_E("Race Response malloc fail!", 0);
        }
    }

    return pEvt;
}

void* RACE_NVKEY_WRITEFULLKEY_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("RACE_NVKEY_WRITEFULLKEY_HDR, channel_id[%d]", 1, channel_id);

    uint32_t nvdm_length = 0, size = 0, i;
    nvdm_status_t status = NVDM_STATUS_ERROR;

    RACE_NVKEY_WRITEFULLKEY_CMD_STRU* pCmd = (RACE_NVKEY_WRITEFULLKEY_CMD_STRU*)pCmdMsg;
#ifdef AIR_RACE_CO_SYS_ENABLE
    RACE_NVKEY_WRITEFULLKEY_EVT_STRU* pEvt;
    if(nvkey_need_sync(pCmd->NVKEY_ID)){
        if(s_pEvt_without_id_rsp == NULL){
            s_pEvt_without_id_rsp = (RACE_NVKEY_WRITEFULLKEY_EVT_STRU*)RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
            (uint16_t)RACE_NVKEY_WRITEFULLKEY, (uint16_t)(sizeof(RACE_NVKEY_WRITEFULLKEY_EVT_STRU)), channel_id);
        }
        pEvt = s_pEvt_without_id_rsp;
    } else {
        pEvt = (RACE_NVKEY_WRITEFULLKEY_EVT_STRU*)RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
            (uint16_t)RACE_NVKEY_WRITEFULLKEY, (uint16_t)(sizeof(RACE_NVKEY_WRITEFULLKEY_EVT_STRU)), channel_id);
    }
#else
    RACE_NVKEY_WRITEFULLKEY_EVT_STRU* pEvt = (RACE_NVKEY_WRITEFULLKEY_EVT_STRU*)RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
        (uint16_t)RACE_NVKEY_WRITEFULLKEY, (uint16_t)(sizeof(RACE_NVKEY_WRITEFULLKEY_EVT_STRU)), channel_id);
#endif

    if (pEvt != NULL)
    {
        nvdm_length = pCmd->Hdr.length - sizeof(pCmd->Hdr.id) - sizeof(pCmd->NVKEY_ID);
        if (nvdm_length > NVDM_DATA_LENGTH)
        {
            nvdm_length = NVDM_DATA_LENGTH;
            RACE_LOG_MSGID_E("write nvdm_length fail, size[%d]", 1, nvdm_length);
        }

#ifdef AIR_RACE_CO_SYS_ENABLE
        if(nvkey_need_sync(pCmd->NVKEY_ID)){
            s_channel_id = channel_id;
            s_data_for_sync[0].nvkey_id = pCmd->NVKEY_ID;
            s_data_for_sync[0].data_len = nvdm_length;
            memcpy(s_data_for_sync[0].data, pCmd->WriteData, nvdm_length);
            bool send_data_succ = race_cosys_send_data(RACE_COSYS_MODULE_ID_NVKEY, false, (uint8_t*)(pCmd), pCmd->Hdr.length + sizeof(RACE_COMMON_HDR_STRU) - 2);
            if(send_data_succ != true){
                RACE_LOG_MSGID_E("race_cosys_send_data fail at RACE_NVKEY_WRITEFULLKEY_RESP_NVID_HDR.", 0);
                pEvt->return_code = 1;
                return pEvt;
            } else {
                return NULL;
            }
        }
#endif
        status = nvkey_write_data(pCmd->NVKEY_ID, pCmd->WriteData, nvdm_length);
        if (status == NVDM_STATUS_OK)
        {
            uint8_t* pData = NULL;
            status = nvkey_data_item_length(pCmd->NVKEY_ID, &size);
            if (status != NVDM_STATUS_OK)
            {
                RACE_LOG_MSGID_E("get NVDM length fail, status[%d]", 1, status);
                pEvt->return_code = status;
                return pEvt;
            }

            pData = (uint8_t*)race_mem_alloc(size);
            if (pData == NULL)
            {
                RACE_LOG_MSGID_E("race_mem_alloc fail", 0);
                pEvt->return_code = NVDM_STATUS_ERROR;
                return pEvt;
            }
            status = nvkey_read_data(pCmd->NVKEY_ID, (uint8_t*)pData, &size);

            if (status == NVDM_STATUS_OK)
            {
                for (i = 0; i < size; i++)
                {
                    if (pData[i] != pCmd->WriteData[i])
                    {
                        RACE_LOG_MSGID_E("nvdm compare fail, nvkey_id[0x%X], write_data[0x%X], read_data[0x%X]",
                            3, pCmd->NVKEY_ID, pCmd->WriteData[i], pData[i]);
                        status = RACE_STATUS_ERROR;
                    }
                }
                pEvt->return_code = status;
            }
            else
            {
                RACE_LOG_MSGID_E("read nvdm fail, nvdm_status[%d]", 1, status);
                pEvt->return_code = status;
            }
            race_mem_free(pData);

            #ifdef SUPPORT_PEQ_NVKEY_UPDATE
            aud_peq_chk_nvkey(pCmd->NVKEY_ID);
            #endif
        }
        else
        {
            RACE_LOG_MSGID_E("write nvdm fail, nvdm_status[%d]", 1, status);
            pEvt->return_code = status;
        }
    }

    return pEvt;
}

void* RACE_NVKEY_WRITEFULLKEY_RESP_NVID_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("RACE_NVKEY_WRITEFULLKEY_RESP_NVID_HDR, channel_id[%d]", 1, channel_id);

    uint32_t nvdm_length;
    nvdm_status_t status;
    uint32_t size;
    uint8_t* pData;
    uint32_t i;

    RACE_NVKEY_WRITEFULLKEY_RESP_NVID_CMD_STRU* pCmd = (RACE_NVKEY_WRITEFULLKEY_RESP_NVID_CMD_STRU*)pCmdMsg;
#ifdef AIR_RACE_CO_SYS_ENABLE
    RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU* pEvt;
    if(nvkey_need_sync(pCmd->NVKEY_ID)){
        if(s_pEvt_with_id_rsp == NULL){
            s_pEvt_with_id_rsp = (RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU*)RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                (uint16_t)RACE_NVKEY_WRITEFULLKEY_RESP_NVID,
                                                                (uint16_t)(sizeof(RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU)),
                                                                channel_id);
        }
        pEvt = s_pEvt_with_id_rsp;
    } else {
        pEvt = (RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU*)RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                (uint16_t)RACE_NVKEY_WRITEFULLKEY_RESP_NVID,
                (uint16_t)(sizeof(RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU)),
                channel_id);
    }
#else
    RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU* pEvt = (RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU*)RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                            (uint16_t)RACE_NVKEY_WRITEFULLKEY_RESP_NVID,
                                                            (uint16_t)(sizeof(RACE_NVKEY_WRITEFULLKEY_RESP_NVID_EVT_STRU)),
                                                            channel_id);
#endif

    if (pEvt != NULL)
    {
        pEvt->NVKEY_ID = pCmd->NVKEY_ID;
        pEvt->return_code = RACE_ERRCODE_FAIL;
        nvdm_length = pCmd->Hdr.length - sizeof(pCmd->Hdr.id) - sizeof(pCmd->NVKEY_ID);

        if (nvdm_length > NVDM_DATA_LENGTH)
            nvdm_length = NVDM_DATA_LENGTH;

        RACE_LOG_MSGID_I("nvdm_length[%ld]", 1, nvdm_length);

#ifdef AIR_RACE_CO_SYS_ENABLE
        if(nvkey_need_sync(pCmd->NVKEY_ID)){
            s_channel_id = channel_id;
            s_data_for_sync[1].nvkey_id = pCmd->NVKEY_ID;
            s_data_for_sync[1].data_len = nvdm_length;
            memcpy(s_data_for_sync[1].data, pCmd->WriteData, nvdm_length);
            bool send_data_succ = race_cosys_send_data(RACE_COSYS_MODULE_ID_NVKEY, false, (uint8_t*)(pCmd), pCmd->Hdr.length + sizeof(RACE_COMMON_HDR_STRU) - 2);
            if(send_data_succ != true){
                RACE_LOG_MSGID_E("race_cosys_send_data fail at RACE_NVKEY_WRITEFULLKEY_RESP_NVID_HDR.", 0);
                pEvt->return_code = 1;
                return pEvt;
            } else {
                return NULL;
            }
        }
#endif
        status = nvkey_write_data(pCmd->NVKEY_ID, pCmd->WriteData, nvdm_length);
        if (status == NVDM_STATUS_OK)
        {
            RACE_LOG_MSGID_I("NVDM Write Done, Read NVDM and compare Data", 0);

            status = nvkey_data_item_length(pCmd->NVKEY_ID, &size);
            if (status != NVDM_STATUS_OK)
            {
                RACE_LOG_MSGID_E("get NVDM length fail, status[%d]", 1, status);
                pEvt->return_code = status;
                return pEvt;
            }
            pData = (uint8_t*)race_mem_alloc(size);
            status = nvkey_read_data(pCmd->NVKEY_ID, (uint8_t*)pData, &size);

            if (status == NVDM_STATUS_OK)
            {
                //compare_buffer(pData, pCmd->WriteData, size);
                for (i = 0; i < size; i++)
                {
                    //RACE_LOG_MSGID_E("nvdm data are : [0x%x] [0x%x]", 2, pData[i], pCmd->WriteData[i]);
                    if (pData[i] != pCmd->WriteData[i])
                    {
                        RACE_LOG_MSGID_E("nvdm data different, nvdm[0x%X], [0x%X] != [0x%X]", 3, pCmd->NVKEY_ID, pData[i], pCmd->WriteData[i]);
                        status = NVDM_STATUS_ERROR;
                        break;
                    }
                }
                if (status == NVDM_STATUS_ERROR)
                    pEvt->return_code = RACE_ERRCODE_FAIL;
                else
                    pEvt->return_code = RACE_ERRCODE_SUCCESS;
            }
            race_mem_free(pData);

            #ifdef SUPPORT_PEQ_NVKEY_UPDATE
            aud_peq_chk_nvkey(pCmd->NVKEY_ID);
            #endif
        }
    }

    return pEvt;
}

#if 0
void* RACE_NVKEY_NEXT_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("RACE_NVKEY_NEXT_HDR() enter, channel_id[0x%X]", 1, channel_id);

    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint16_t NVKEY_ID;
        uint16_t length_of_read_bytes;
    }PACKED RACE_NVKEY_READFULLKEY_CMD_STRU;

    typedef struct
    {
        uint16_t length_of_read_bytes;
        uint8_t ReadData[0];
    }PACKED RACE_NVKEY_READFULLKEY_EVT_STRU;

    uint32_t        size;


    RACE_NVKEY_READFULLKEY_CMD_STRU* pCmd = (RACE_NVKEY_READFULLKEY_CMD_STRU*)pCmdMsg;


    nvkey_data_item_length(pCmd->NVKEY_ID, &size);

    RACE_NVKEY_READFULLKEY_EVT_STRU* pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_NVKEY_READFULLKEY,
                                                             (uint16_t)(sizeof(RACE_NVKEY_READFULLKEY_EVT_STRU) + size), channel_id);


    if (pEvt != NULL)
    {
        nvdm_status_t status;
        uint32_t nvdm_length = pCmd->length_of_read_bytes;

        #if defined (MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
            if (nvdm_length > SERIAL_PORT_RECEIVE_BUFFER_SIZE)
                nvdm_length = SERIAL_PORT_RECEIVE_BUFFER_SIZE;
        #else
            if (nvdm_length > 1024)
                nvdm_length = 1024;
        #endif

        RACE_LOG_MSGID_I("pCmd->NVKEY_ID = %ld, nvdm_length = %ld \r\n",2, pCmd->NVKEY_ID, nvdm_length);

        pEvt->length_of_read_bytes = nvdm_length;

        size = nvdm_length;
        status = nvkey_read_data(pCmd->NVKEY_ID, (const uint8_t*)pEvt->ReadData, &size);
        pEvt->length_of_read_bytes = size;
        if (status != NVDM_STATUS_OK)
        {
            RACE_LOG_MSGID_E("nvkey_read_data fail, status = %d \r\n",1, status);
        }
    }

    return pEvt;
}
#endif

/**
 * RACE_NVKEY_RECLAIM_HDR
 *
 * RACE NVKEY RECLAIM Handler
 *
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_NVKEY_RECLAIM_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("RACE_NVKEY_RECLAIM_HDR, channel_id[%d]",1, channel_id);

    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint16_t minimum_space;
    }PACKED RACE_NVDM_RECLAIM_CMD_STRU;

    typedef struct
    {
        uint8_t Status;
    }PACKED RACE_NVDM_RECLAIM_EVT_STRU;

    RACE_NVDM_RECLAIM_CMD_STRU* pCmd = (RACE_NVDM_RECLAIM_CMD_STRU*)pCmdMsg;
    RACE_NVDM_RECLAIM_EVT_STRU* pEvt = NULL;

    pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_NVKEY_RECLAIM,
                            (uint16_t)(sizeof(RACE_NVDM_RECLAIM_EVT_STRU)), channel_id);
    if (pEvt != NULL)
    {
        uint32_t total_avail_space, curr_used_space;
        if( (nvdm_query_space_info(&total_avail_space, &curr_used_space) == NVDM_STATUS_OK)
            && (total_avail_space - curr_used_space > (uint32_t)pCmd->minimum_space) )
        {
            pEvt->Status = 1; //follow old proj, return 1 when success.
        }
        else
        {
            pEvt->Status = 0;
        }
    }

    return pEvt;
}

extern uint32_t g_max_group_name_size;
extern uint32_t g_max_data_item_name_size;
void* RACE_NVDM_GETALL_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        uint8_t Status;
        uint16_t NumNvdm;
        uint16_t NvdmData[0];
    }PACKED RACE_NVDM_GETALL_EVT_STRU;

    char *group_name = NULL, *item_name = NULL;
    RACE_NVDM_GETALL_EVT_STRU* pEvt = NULL;
    nvdm_status_t ret;
    uint32_t data_item_count, tmp_data;
    uint16_t length_NVDM_data, i;
    race_status_t status = RACE_STATUS_OK;

    RACE_LOG_MSGID_I("RACE_NVDM_GETALL_HDR, channel_id[%d]", 1, channel_id);

    group_name = (char *)race_mem_alloc(g_max_group_name_size + 1);
    item_name = (char *)race_mem_alloc(g_max_data_item_name_size + 1);

    if ((group_name == NULL) || (item_name == NULL))
    {
        RACE_LOG_MSGID_E("group item, race_mem_alloc fail", 0);
        goto RETURN_ERRCODE_2;
    }

    ret = nvdm_query_begin();
    if (ret != NVDM_STATUS_OK)
    {
        RACE_LOG_MSGID_E("nvdm_query_begin fail, ret[%d]", 1, ret);
        goto RETURN_ERRCODE_2;
    }

    while(1)
    {
        ret = nvdm_query_next_group_name(group_name);
        if (ret != NVDM_STATUS_OK)
        {
            RACE_LOG_MSGID_E("nvdm_query_next_group_name fail, ret[%d]", 1, ret);
            goto RETURN_ERRCODE_1;
        }
        if (!strcmp(group_name, RACE_GROUP_NAME))
        {
            break;
        }
    }

    nvdm_query_data_item_count(group_name, (uint32_t *)&data_item_count);
    length_NVDM_data = data_item_count * RACE_ITEM_NAME_SIZE;

    pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_NVDM_GETALL,
                            (uint16_t)(sizeof(RACE_NVDM_GETALL_EVT_STRU) + length_NVDM_data), channel_id);
    if (pEvt != NULL)
    {
        pEvt->NumNvdm = (uint16_t)data_item_count;

        RACE_LOG_MSGID_I("data_item_count[%d], length_NVDM_data[%d]", 2, data_item_count, length_NVDM_data);

        for (i = 0; i < data_item_count; i++)
        {
            ret = nvdm_query_next_data_item_name(item_name);
            if (ret == NVDM_STATUS_OK)
            {
                sscanf(item_name, "%x", (unsigned int *)&tmp_data);
                pEvt->NvdmData[i] = (uint16_t)tmp_data;
                //RACE_LOG_MSGID_I("data[%X]", 1, tmp_data);
            }
            else
            {
                RACE_LOG_MSGID_E("nvdm_query_next_data_item_name fail, i[%d], ret[%d]", 2, i, ret);
                pEvt->NvdmData[i] = 0xFFFF;
                status = RACE_STATUS_ERROR;
            }
            //RACE_LOG_I("i[%d], item name[%s], tmp_data[0x%X]", i, item_name, tmp_data);
        }
        pEvt->Status = status;

        ret = nvdm_query_end();
        if (ret != NVDM_STATUS_OK)
        {
            RACE_LOG_MSGID_E("nvdm_query_end fail, ret[%d]", 1, ret);
        }
        if (group_name != NULL){
            race_mem_free(group_name);
        }
        if (item_name != NULL){
            race_mem_free(item_name);
        }

        return pEvt;
    }

    RETURN_ERRCODE_1:
    ret = nvdm_query_end();
    if (ret != NVDM_STATUS_OK)
    {
        RACE_LOG_MSGID_E("nvdm_query_end fail, ret[%d]", 1, ret);
    }

    RETURN_ERRCODE_2:
    if (group_name != NULL){
        race_mem_free(group_name);
    }
    if (item_name != NULL){
        race_mem_free(item_name);
    }

    pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_NVDM_GETALL,
        (uint16_t)(sizeof(RACE_NVDM_GETALL_EVT_STRU) + 1), channel_id);

    if (pEvt != NULL)
    {
        pEvt->Status = RACE_ERRCODE_SUCCESS;
        pEvt->NumNvdm = 0;
        *((uint8_t *)pEvt->NvdmData) = 0;
    }
    else
    {
        RACE_LOG_MSGID_E("pEvt race_mem_alloc fail", 0);
        pEvt = NULL;
    }

    return pEvt;
}

#if ((PRODUCT_VERSION == 1552) || defined(AM255X))
extern bool bt_driver_enter_relay_mode(uint8_t port);
extern void bt_driver_relay_register_callbacks(void *callback);
extern atci_bt_relay_callbacks atci_bt_relay_cb;
#if _MSC_VER >= 1500
    #pragma comment(linker, "/alternatename:_bt_driver_enter_relay_mode=_bt_driver_enter_relay_mode_ext")
    #pragma comment(linker, "/alternatename:_bt_driver_relay_register_callbacks=_bt_driver_relay_register_callbacks_ext")

#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION)
    #pragma weak bt_driver_enter_relay_mode = bt_driver_enter_relay_mode_ext
    #pragma weak bt_driver_relay_register_callbacks = bt_driver_relay_register_callbacks_ext
#else
    #error "Unsupported Platform"
#endif

bool bt_driver_enter_relay_mode_ext(uint8_t port)
{
    return false;
}

void bt_driver_relay_register_callbacks_ext(void *callback)
{
    return;
}

static void RACE_NVDM_ENTER_RELAY_WITH_TX_CONFIG()
{
    uint32_t tx_config_size = sizeof(bt_config_tx_power_ext1_t);
    bt_config_tx_power_ext1_t tx_cfg = {
        .bdr_init_tx_power_level = 7,
        .reserved = 0,
        .fixed_tx_power_enable = 0,
        .fixed_tx_power_level = 0,
        .le_init_tx_power_level = 7,
        .bt_max_tx_power_level = 7,
        .bdr_tx_power_level_offset = 1,
        .bdr_fine_tx_power_level_offset = 0,
        .edr_tx_power_level_offset = 1,
        .edr_fine_tx_power_level_offset = 0,
        .ble_tx_power_level_offset = 1,
        .ble_fine_tx_power_level_offset = 0,
        .edr_init_tx_power_level = 7
    };
    nvkey_status_t nvkey_ret = nvkey_read_data(NVKEYID_BT_DEFAULT_TX_POWER_EXTEND, (uint8_t *)(&tx_cfg), &tx_config_size);
    RACE_LOG_MSGID_I("read nvkey of BT default tx power extend, ret[%d]", 1, nvkey_ret);
    bt_status_t status = bt_config_tx_power_level_ext1(&tx_cfg);
    RACE_LOG_MSGID_I("status[0x%08x], bdr_init_tx[%d], bt_max_tx[%d], le_init_tx[%d], fixed_tx_enable[%d], fixed_tx[%d], bdr_offset[%d], bdr_fine_offset[%d], edr_offset[%d], edr_fine_offset[%d], ble_offset[%d], ble_fine_offset[%d], edr_init_tx[%d]",
    13, status, tx_cfg.bdr_init_tx_power_level, tx_cfg.bt_max_tx_power_level,
    tx_cfg.le_init_tx_power_level,tx_cfg.fixed_tx_power_enable, tx_cfg.fixed_tx_power_level,
    tx_cfg.bdr_tx_power_level_offset, tx_cfg.bdr_fine_tx_power_level_offset,
    tx_cfg.edr_tx_power_level_offset, tx_cfg.edr_fine_tx_power_level_offset,
    tx_cfg.ble_tx_power_level_offset, tx_cfg.ble_fine_tx_power_level_offset,
    tx_cfg.edr_init_tx_power_level);
    uint8_t bt_sleep_manager_handle = hal_sleep_manager_set_sleep_handle("bt");
    hal_sleep_manager_lock_sleep(bt_sleep_manager_handle);
    bt_driver_relay_register_callbacks((void *)&atci_bt_relay_cb);
    bool ret = bt_driver_enter_relay_mode(g_race_uart_port);
    RACE_LOG_MSGID_I("enter relay mode result[%d], port[%d]",2, ret, g_race_uart_port);
}

static bt_status_t RACE_NVDM_BT_GAP_EVENT_CALLBACK(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    switch (msg) {
        case BT_POWER_ON_CNF: {
            RACE_LOG_MSGID_I("BT_POWER_ON_CNF", 0);
        #ifdef MTK_BT_CM_SUPPORT
            bt_cm_power_standby(false);
        #else
            bt_sink_srv_send_action(BT_SINK_SRV_ACTION_BT_STANDBY, NULL);
        #endif
        }
        break;

        case BT_POWER_OFF_CNF: {
            RACE_LOG_MSGID_I("BT_POWER_OFF_CNF", 0);
            RACE_NVDM_ENTER_RELAY_WITH_TX_CONFIG();
        }
        break;
    }
    return BT_STATUS_SUCCESS;
}
#endif

void* RACE_SWITCH_FUNC_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("RACE_SWITCH_FUNC_HDR, channel_id[%d]", 1, channel_id);

    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t SwitchFunc;

    }PACKED RACE_SWITCH_FUNC_CMD_STRU;
#ifndef MTK_MUX_ENABLE
    typedef struct
    {
        uint8_t return_code;
    }PACKED RACE_SWITCH_FUNC_EVT_STRU;
#endif
    RACE_SWITCH_FUNC_CMD_STRU* pCmd = (RACE_SWITCH_FUNC_CMD_STRU*)pCmdMsg;

    if (pCmd != NULL)
    {
        switch (pCmd->SwitchFunc)
        {
            case RACE_SWITCH_TO_AT :
            {
#if defined(MTK_MUX_ENABLE)
                RACE_LOG_MSGID_E("RACE_SWITCH_FUNC_HDR, no support MUX",0);
                return NULL;
#else
                RACE_SWITCH_FUNC_EVT_STRU* pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_SWITCH_FUNC, (uint16_t)(sizeof(RACE_SWITCH_FUNC_EVT_STRU)), channel_id);

                if (pEvt != NULL)
                {
                    pEvt->return_code = RACE_STATUS_OK;
                    race_flush_packet((uint8_t *)pEvt, channel_id);
                }
                race_status_t sta = race_uart_deinit();
                if (sta != RACE_STATUS_OK)
                {}
                RACE_LOG_MSGID_I("RACE switch to AT, result[%d], port[%d], default port[%d]", 3, sta, g_race_uart_port, g_race_uart_port_default);

                if (g_race_uart_port == g_race_uart_port_default)
                {
                    atci_status_t sta = atci_deinit_keep_table(g_atci_uart_port_default);
                    if (sta != ATCI_STATUS_OK)
                    {}

                #if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
                    serial_port_dev_t port;
                    port = g_race_uart_port_default;
                    atci_init(port);
                #else
                    atci_init(g_race_uart_port_default);
                #endif

                }
                else
                {
                #if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
                    serial_port_dev_t port;
                    serial_port_setting_uart_t uart_setting;

                    if(serial_port_config_read_dev_number("atci", &port) != SERIAL_PORT_STATUS_OK)
                    {
                        port = CONFIG_ATCI_PORT;
                        serial_port_config_write_dev_number("atci", port);
                        RACE_LOG_MSGID_I("serial_port_config_write_dev_number setting uart1", 0);
                        uart_setting.baudrate = HAL_UART_BAUDRATE_921600;
                        serial_port_config_write_dev_setting(port, (serial_port_dev_setting_t *)&uart_setting);
                    }
                    atci_init(port);
                #else
                    atci_init(CONFIG_ATCI_PORT);
                #endif
                #if defined(MTK_RACE_CMD_ENABLE) && defined(MTK_PORT_SERVICE_ENABLE)
                    serial_port_dev_t race_port;
                    serial_port_setting_uart_t race_uart_setting;

                    if(serial_port_config_read_dev_number("race", &race_port) != SERIAL_PORT_STATUS_OK)
                    {
                        race_port = CONFIG_RACE_PORT;
                        serial_port_config_write_dev_number("race", race_port);
                        RACE_LOG_MSGID_I("serial_port_config_write_dev_number setting uart1", 0);
                        race_uart_setting.baudrate = HAL_UART_BAUDRATE_921600;
                        serial_port_config_write_dev_setting(race_port, (serial_port_dev_setting_t *)&race_uart_setting);
                    }
                    race_serial_port_uart_init(race_port);
                #else
                    race_serial_port_uart_init(CONFIG_RACE_PORT);
                #endif
                }
#endif
            }
            break;

            case RACE_SWITCH_TO_LOG :
            {

            }
            break;

            case RACE_SWITCH_TO_HCI :
            {

            }
            case RACE_SWITCH_TO_RELAY :
            {
#if ((PRODUCT_VERSION == 1552) || defined(AM255X))
                RACE_LOG_MSGID_I("RACE_SWITCH_TO_RELAY", 0);
                RACE_SWITCH_FUNC_EVT_STRU* pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_SWITCH_FUNC, (uint16_t)(sizeof(RACE_SWITCH_FUNC_EVT_STRU)), channel_id);

                if (pEvt != NULL)
                {
                    pEvt->return_code = RACE_STATUS_OK;
                    race_flush_packet((uint8_t *)pEvt, channel_id);
                }

                race_status_t sta = race_uart_deinit();
                bt_cm_power_state_t power_state = bt_connection_manager_power_get_state();
                if(sta != RACE_STATUS_OK) {
                    RACE_LOG_MSGID_E("Deinit race fail", 0);
                }
                if(BT_CM_POWER_STATE_ON == power_state || BT_CM_POWER_STATE_ON_PENDING == power_state
                    || BT_CM_POWER_STATE_OFF_PENDING == power_state) {
                    bt_gap_ata_callback_register((void *)RACE_NVDM_BT_GAP_EVENT_CALLBACK);
                    if(BT_CM_POWER_STATE_ON == power_state) {
                #ifdef MTK_BT_CM_SUPPORT
                        bt_cm_power_standby(false);
                #else
                        bt_sink_srv_send_action(BT_SINK_SRV_ACTION_BT_STANDBY, NULL);
                #endif
                    }
                } else {
                    RACE_NVDM_ENTER_RELAY_WITH_TX_CONFIG();
                }
#endif
            }
            break;

            default:
            {
                while(1);
            }
            break;
        }
    }
    return NULL;
}

/**
 * race_reload_nvdm_to_ram
 *
 * Notify module to reload data from NDVM to RAM
 *
 * @p_cmd_msg : pointer of ptr_race_pkt_t
 *
 */
static void* race_reload_nvdm_to_ram(ptr_race_pkt_t p_cmd_msg, uint8_t channel_id)
{
    typedef struct {
        uint8_t status;
    } PACKED race_response_t;

    typedef struct {
        RACE_COMMON_HDR_STRU Hdr;
        uint16_t nvkey_id;
    } PACKED race_cmd_t;

    race_response_t *p_response = RACE_ClaimPacket(RACE_TYPE_RESPONSE, RACE_RELOAD_NVKEY_TO_RAM, sizeof(race_response_t), channel_id);
    if (p_response == NULL) {
        RACE_LOG_MSGID_E("response alloc fail", 0);
        return NULL;
    }

    race_cmd_t* cmd = (race_cmd_t*)p_cmd_msg;

    uint16_t *p_nvkey_id = (uint16_t *)race_mem_alloc(sizeof(uint16_t));
    if (p_nvkey_id) {
        *p_nvkey_id = cmd->nvkey_id;
        p_response->status = race_send_event_notify_msg(RACE_EVENT_RELOAD_NVKEY_TO_RAM, (void *)p_nvkey_id);
    } else {
        RACE_LOG_MSGID_E("Cannot malloc p_nvkey_id", 0);
        p_response->status = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
    }

    return p_response;
}

void* RACE_CmdHandler_NVDM(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id)
{
    void* ptr = NULL;

    RACE_LOG_MSGID_I("RACE_CmdHandler_NVDM, type[0x%X], race_id[0x%X], channel_id[%d]", 3,
        pRaceHeaderCmd->hdr.type, pRaceHeaderCmd->hdr.id, channel_id);

    switch (pRaceHeaderCmd->hdr.id)
    {
        case RACE_NVKEY_READFULLKEY :
        {
            ptr = RACE_NVKEY_READFULLKEY_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_NVKEY_WRITEFULLKEY :
        {
            ptr = RACE_NVKEY_WRITEFULLKEY_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_NVKEY_NEXT :
        {
            //ptr = RACE_NVKEY_NEXT_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_NVKEY_RECLAIM :
        {
            ptr = RACE_NVKEY_RECLAIM_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_NVKEY_READFULLKEY_RESP_NVID :
        {
            ptr = RACE_NVKEY_READFULLKEY_RESP_NVID_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_NVKEY_WRITEFULLKEY_RESP_NVID :
        {
            ptr = RACE_NVKEY_WRITEFULLKEY_RESP_NVID_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_NVDM_GETALL :
        {
            ptr = RACE_NVDM_GETALL_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_SWITCH_FUNC :
        {
            ptr = RACE_SWITCH_FUNC_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_RELOAD_NVKEY_TO_RAM :
        {
            ptr = race_reload_nvdm_to_ram(pRaceHeaderCmd, channel_id);
        }
        break;

        default:
        {
        }
        break;
    }

    return ptr;
}


#endif /* RACE_NVDM_CMD_ENABLE */

