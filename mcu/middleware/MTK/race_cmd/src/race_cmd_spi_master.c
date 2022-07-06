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
#include "race_cmd.h"
#include "race_lpcomm_trans.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_msg_struct.h"
#include "race_lpcomm_conn.h"
#include "race_noti.h"
#include "race_lpcomm_ps_noti.h"

#include "race_cmd_spi_master.h"
#include "hal.h"
#include "race_xport.h"
#include "memory_attribute.h"

#ifdef RACE_SPI_MASTER_CMD_ENABLE

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define RACE_SPI_MASTER_SEND_MAX_LEN    50
#define RACE_SPI_MASTER_RECEIVE_MAX_LEN 100

static SemaphoreHandle_t g_race_spi_master_semaphore = NULL;
static volatile uint32_t g_race_spi_master_err_flag = 0;
static ATTR_ZIDATA_IN_NONCACHED_RAM_16BYTE_ALIGN uint8_t send_buffer[RACE_SPI_MASTER_SEND_MAX_LEN];
static ATTR_ZIDATA_IN_NONCACHED_RAM_16BYTE_ALIGN uint8_t receive_buff[RACE_SPI_MASTER_RECEIVE_MAX_LEN];

void race_spi_master_callback (hal_spi_master_callback_event_t event,void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (HAL_SPI_MASTER_EVENT_SEND_FINISHED == event) {
        RACE_LOG_MSGID_I("Race SPI master send done",0);

    } else if (HAL_SPI_MASTER_EVENT_RECEIVE_FINISHED == event) {
        RACE_LOG_MSGID_I("Race SPI master receive done",0);
        g_race_spi_master_err_flag = 0;
        xSemaphoreGiveFromISR( g_race_spi_master_semaphore, &xHigherPriorityTaskWoken );
    } else if (HAL_SPI_MASTER_NO_BUSY_FUNCTION_ERROR == event) {
        // Error handler;
        g_race_spi_master_err_flag = 1;
         RACE_LOG_MSGID_I("Race SPI master race_spi_master_callback ERROR!!!",0);
    }
}


void* race_cmdhdl_spi_master_send_and_receive(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    uint32_t i;
    typedef struct// for import parameter
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t spi_master_port;
        hal_spi_master_config_t spi_config;
        hal_spi_master_advanced_config_t spi_advanced_config;
        hal_spi_master_non_single_config_t spi_non_single_config;
        hal_spi_master_chip_select_timing_t chip_select_timing;
        hal_spi_master_deassert_t deassert;
        uint32_t receive_length;
        uint32_t send_length;
        uint8_t send_data[0];
    } PACKED *PTR_THIS_RACE_CMD_STRU;
    PTR_THIS_RACE_CMD_STRU pThisCmd = (PTR_THIS_RACE_CMD_STRU)pCmdMsg;

    typedef struct {// for export parameter
        uint8_t  status;
        uint8_t receive_data[RACE_SPI_MASTER_RECEIVE_MAX_LEN];
    } PACKED RSP;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                        RACE_TYPE_RESPONSE,
                                        RACE_CMD_SPI_MASTER_SEND_AND_RECEIVE,
                                        pThisCmd->receive_length+1,
                                        channel_id);
    if((pThisCmd->receive_length>=RACE_SPI_MASTER_RECEIVE_MAX_LEN) || (pThisCmd->send_length>=RACE_SPI_MASTER_SEND_MAX_LEN)){
        RACE_LOG_MSGID_I("Race SPI master parameter check error,len too long!!",0);
        return (void*)pEvt;
    }
    g_race_spi_master_err_flag = 0;
    if (pEvt) {
           if(g_race_spi_master_semaphore == NULL)
                g_race_spi_master_semaphore = xSemaphoreCreateBinary();
           if( g_race_spi_master_semaphore == NULL )
                assert(0);
            hal_spi_send_and_receive_config_ex_no_busy_t spi_send_and_receive_config_ex_no_busy;
        RACE_LOG_MSGID_I("race_cmdhdl_spi_master_send_and_receive: spi master port:%d,clock freq:%d,slave port:%d,order:%d,pola:%d,phase:%d,rx len:%d,tx len:%d mode=%d\
        dummy_bits=%d command_bytes=%d byte_order=%d chip_polarity=%d get_tick=%d sample_select=%d setp_count=%d hold_count=%d idle_count=%d deassert=%d", 19,pThisCmd->spi_master_port,
                                                                                                          pThisCmd->spi_config.clock_frequency,
                                                                                                          pThisCmd->spi_config.slave_port,
                                                                                                          pThisCmd->spi_config.bit_order,
                                                                                                          pThisCmd->spi_config.polarity,
                                                                                                          pThisCmd->spi_config.phase,
                                                                                                          pThisCmd->receive_length,
                                                                                                          pThisCmd->send_length,
                                                                                                          pThisCmd->spi_non_single_config.mode,
                                                                                                          pThisCmd->spi_non_single_config.dummy_bits,
                                                                                                          pThisCmd->spi_non_single_config.command_bytes,
                                                                                                          pThisCmd->spi_advanced_config.byte_order,
                                                                                                          pThisCmd->spi_advanced_config.chip_polarity,
                                                                                                          pThisCmd->spi_advanced_config.get_tick,
                                                                                                          pThisCmd->spi_advanced_config.sample_select,
                                                                                                          pThisCmd->chip_select_timing.chip_select_setup_count,
                                                                                                          pThisCmd->chip_select_timing.chip_select_hold_count,
                                                                                                          pThisCmd->chip_select_timing.chip_select_idle_count,
                                                                                                          pThisCmd->deassert);

            spi_send_and_receive_config_ex_no_busy.spi_config.bit_order = pThisCmd->spi_config.bit_order;
            spi_send_and_receive_config_ex_no_busy.spi_config.clock_frequency = pThisCmd->spi_config.clock_frequency;
            spi_send_and_receive_config_ex_no_busy.spi_config.phase = pThisCmd->spi_config.phase;
            spi_send_and_receive_config_ex_no_busy.spi_config.polarity = pThisCmd->spi_config.polarity;
            spi_send_and_receive_config_ex_no_busy.spi_config.slave_port = pThisCmd->spi_config.slave_port;
            spi_send_and_receive_config_ex_no_busy.spi_advanced_config.byte_order = pThisCmd->spi_advanced_config.byte_order;
            spi_send_and_receive_config_ex_no_busy.spi_advanced_config.chip_polarity = pThisCmd->spi_advanced_config.chip_polarity;
            spi_send_and_receive_config_ex_no_busy.spi_advanced_config.get_tick = pThisCmd->spi_advanced_config.get_tick;
            spi_send_and_receive_config_ex_no_busy.spi_advanced_config.sample_select = pThisCmd->spi_advanced_config.sample_select;
            spi_send_and_receive_config_ex_no_busy.spi_non_single_config.mode = pThisCmd->spi_non_single_config.mode;
            spi_send_and_receive_config_ex_no_busy.spi_non_single_config.dummy_bits = pThisCmd->spi_non_single_config.dummy_bits;
            spi_send_and_receive_config_ex_no_busy.spi_non_single_config.command_bytes = pThisCmd->spi_non_single_config.command_bytes;
            spi_send_and_receive_config_ex_no_busy.chip_select_timing.chip_select_setup_count = pThisCmd->chip_select_timing.chip_select_setup_count;
            spi_send_and_receive_config_ex_no_busy.chip_select_timing.chip_select_hold_count = pThisCmd->chip_select_timing.chip_select_hold_count;
            spi_send_and_receive_config_ex_no_busy.chip_select_timing.chip_select_idle_count = pThisCmd->chip_select_timing.chip_select_idle_count;
            spi_send_and_receive_config_ex_no_busy.deassert = pThisCmd->deassert;
            for(i=0;i<pThisCmd->send_length;i++)
                send_buffer[i] = pThisCmd->send_data[i];

            spi_send_and_receive_config_ex_no_busy.spi_callback = race_spi_master_callback;
            spi_send_and_receive_config_ex_no_busy.spi_send_and_receive_config_ex.receive_buffer = receive_buff;
            spi_send_and_receive_config_ex_no_busy.spi_send_and_receive_config_ex.receive_length = pThisCmd->receive_length;
            spi_send_and_receive_config_ex_no_busy.spi_send_and_receive_config_ex.send_data = send_buffer;
            spi_send_and_receive_config_ex_no_busy.spi_send_and_receive_config_ex.send_length = pThisCmd->send_length;
            spi_send_and_receive_config_ex_no_busy.user_data = NULL;

            hal_spi_master_status_t status;
            // Send and receive data simultaneously.
            status = hal_spi_master_send_and_receive_dma_no_busy(pThisCmd->spi_master_port, &spi_send_and_receive_config_ex_no_busy);
            if (HAL_SPI_MASTER_STATUS_OK != status) {
            // Error handler;
                   pEvt->status = status;//
                   RACE_LOG_MSGID_I("Race SPI master send and receive error!!!status:%d",1,status);
                   return (void*)pEvt;
            }
             RACE_LOG_MSGID_I("Race SPI master waiting for send and receive done~~~",0);
            if(xSemaphoreTake( g_race_spi_master_semaphore, portMAX_DELAY) == pdTRUE)
            {
                if(g_race_spi_master_err_flag == 1)
                {
                   pEvt->status = 1;//
                    RACE_LOG_MSGID_I("Race SPI master send and receive fail!!!g_race_spi_master_err_flag = 1",0);
                    return (void*)pEvt;
                }
                for(i=0;i<pThisCmd->receive_length;i++)
                {
                    pEvt->receive_data[i]= receive_buff[i];
                    RACE_LOG_MSGID_I("Race SPI master receive data:0x%x",1,(uint8_t)receive_buff[i]);
                }
                pEvt->status = 0;
                RACE_LOG_MSGID_I("Race SPI master send and receive success",0);
            }
            else
            {
                pEvt->status = 1;//
                RACE_LOG_MSGID_I("Race SPI master send and receive fail!!!",0);
            }
    }
    return (void*)pEvt;
}


void* RACE_CmdHandler_spi_master_send_and_receive(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("pCmdMsg->hdr.id = %d", 1, (int)pCmdMsg->hdr.id);

    switch (pCmdMsg->hdr.id)
    {
        case RACE_CMD_SPI_MASTER_SEND_AND_RECEIVE: //0x5000
        {
            return race_cmdhdl_spi_master_send_and_receive((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);
        }
        break;

        default:
            break;
    }

    return NULL;
}

#endif
