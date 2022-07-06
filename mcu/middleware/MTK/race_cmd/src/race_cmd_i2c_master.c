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

#include "race_cmd_i2c_master.h"
#include "hal.h"
#include "race_xport.h"
#include "memory_attribute.h"

#ifdef RACE_I2C_MASTER_CMD_ENABLE
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
#define RACE_I2C_MASTER_SEND_MAX_LEN    50
#define RACE_I2C_MASTER_RECEIVE_MAX_LEN 50
static SemaphoreHandle_t g_race_i2c_master_semaphore = NULL;
static volatile uint32_t g_race_i2c_master_err_flag = 0;

static ATTR_ZIDATA_IN_NONCACHED_RAM_16BYTE_ALIGN uint8_t send_buffer[RACE_I2C_MASTER_SEND_MAX_LEN];
static ATTR_ZIDATA_IN_NONCACHED_RAM_16BYTE_ALIGN uint8_t receive_buff[RACE_I2C_MASTER_RECEIVE_MAX_LEN];


void race_i2c_master_callback (uint8_t slave_address,hal_i2c_callback_event_t event,void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (HAL_I2C_EVENT_SUCCESS == event) {
        RACE_LOG_MSGID_I("Race I2C master send done",0);
        g_race_i2c_master_err_flag = 0;

    } else if (HAL_I2C_EVENT_NACK_ERROR == event) {
         RACE_LOG_MSGID_I("Race I2C master HAL_I2C_EVENT_NACK_ERROR ERROR!!!",0);
         g_race_i2c_master_err_flag = 1;
    } else if (HAL_I2C_EVENT_TIMEOUT_ERROR == event) {
         RACE_LOG_MSGID_I("Race I2C master HAL_I2C_EVENT_TIMEOUT_ERROR ERROR!!!",0);
         g_race_i2c_master_err_flag = 1;
    } else if (HAL_I2C_EVENT_ACK_ERROR == event) {
        // Error handler;
         RACE_LOG_MSGID_I("Race I2C master HAL_I2C_EVENT_ACK_ERROR ERROR!!!",0);
        g_race_i2c_master_err_flag = 1;
    }
    xSemaphoreGiveFromISR( g_race_i2c_master_semaphore, &xHigherPriorityTaskWoken );
}


void* race_cmdhdl_i2c_master_send_and_receive(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    uint32_t i;
    typedef struct// for import parameter
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t i2c_master_port;
        hal_i2c_config_t i2c_config;
        uint8_t slave_address;                     /**<  The slave device address. */
        uint32_t receive_packet_length;            /**<  The receive packet length. */
        uint32_t receive_bytes_in_one_packet;      /**<  The number of bytes in one packet. */
        uint32_t send_packet_length;               /**<  The send packet length. */
        uint32_t send_bytes_in_one_packet;         /**<  The number of bytes in one packet. */
        uint8_t send_data[0];
    } PACKED *PTR_THIS_RACE_CMD_STRU;
    PTR_THIS_RACE_CMD_STRU pThisCmd = (PTR_THIS_RACE_CMD_STRU)pCmdMsg;

    typedef struct {// for export parameter
        uint8_t  status;
        uint8_t receive_data[RACE_I2C_MASTER_RECEIVE_MAX_LEN];
    } PACKED RSP;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                        RACE_TYPE_RESPONSE,
                                        RACE_CMD_I2C_MASTER_SEND_AND_RECEIVE,
                                        pThisCmd->receive_bytes_in_one_packet * pThisCmd->receive_packet_length+1,
                                        channel_id);
    if((pThisCmd->receive_bytes_in_one_packet * pThisCmd->receive_packet_length >= RACE_I2C_MASTER_RECEIVE_MAX_LEN) || 
            (pThisCmd->send_bytes_in_one_packet * pThisCmd->send_packet_length >= RACE_I2C_MASTER_SEND_MAX_LEN)){
        RACE_LOG_MSGID_I("Race I2C master parameter check error,len too long!!",0);
        return (void*)pEvt;
    }
    g_race_i2c_master_err_flag = 0;
    if (pEvt) {
           if(g_race_i2c_master_semaphore == NULL)
                g_race_i2c_master_semaphore = xSemaphoreCreateBinary();
           if( g_race_i2c_master_semaphore == NULL )
                assert(0);
            hal_i2c_send_to_receive_config_ex_no_busy_t i2c_send_to_receive_config_no_busy_ex;
        RACE_LOG_MSGID_I("race_cmdhdl_i2c_master_send_and_receive: i2c master port:%d,clock freq:%d,slave addr:%d,Rx packet len:%d,Rx %d bytes in one packet,Tx packet len:%d,Tx %d bytes in one packet",7,
                                                                                                          pThisCmd->i2c_master_port,
                                                                                                          pThisCmd->i2c_config.frequency,
                                                                                                          pThisCmd->slave_address,
                                                                                                          pThisCmd->receive_packet_length,
                                                                                                          pThisCmd->receive_bytes_in_one_packet,
                                                                                                          pThisCmd->send_packet_length,
                                                                                                          pThisCmd->send_bytes_in_one_packet);

            i2c_send_to_receive_config_no_busy_ex.i2c_config.frequency = pThisCmd->i2c_config.frequency;
            i2c_send_to_receive_config_no_busy_ex.i2c_send_to_receive_config_ex.slave_address = pThisCmd->slave_address;
            i2c_send_to_receive_config_no_busy_ex.i2c_send_to_receive_config_ex.receive_packet_length = pThisCmd->receive_packet_length;
            i2c_send_to_receive_config_no_busy_ex.i2c_send_to_receive_config_ex.receive_bytes_in_one_packet = pThisCmd->receive_bytes_in_one_packet;
            i2c_send_to_receive_config_no_busy_ex.i2c_send_to_receive_config_ex.send_packet_length = pThisCmd->send_packet_length;
            i2c_send_to_receive_config_no_busy_ex.i2c_send_to_receive_config_ex.send_bytes_in_one_packet = pThisCmd->send_bytes_in_one_packet;

           i2c_send_to_receive_config_no_busy_ex.i2c_send_to_receive_config_ex.receive_buffer = receive_buff; 
            i2c_send_to_receive_config_no_busy_ex.i2c_send_to_receive_config_ex.send_data = send_buffer;
            i2c_send_to_receive_config_no_busy_ex.i2c_callback = race_i2c_master_callback;
            i2c_send_to_receive_config_no_busy_ex.user_data = NULL;
            for(i=0;i<pThisCmd->send_bytes_in_one_packet * pThisCmd->send_packet_length;i++)
                send_buffer[i] = pThisCmd->send_data[i];

            hal_i2c_status_t status;
            // Send and receive data simultaneously.
            status = hal_i2c_master_send_to_receive_dma_ex_none_blocking(pThisCmd->i2c_master_port, &i2c_send_to_receive_config_no_busy_ex);
            if (HAL_I2C_STATUS_OK != status) {
            // Error handler;
                   pEvt->status = status;// 
                   RACE_LOG_MSGID_I("Race I2C master send and receive error!!!status:%d",1,status);
                   return (void*)pEvt; 
            }
             RACE_LOG_MSGID_I("Race I2C master waiting for send and receive done~~~",0);
            if(xSemaphoreTake( g_race_i2c_master_semaphore, portMAX_DELAY) == pdTRUE)
            {
                if(g_race_i2c_master_err_flag == 1)
                {
                    pEvt->status = 1;// 
                    RACE_LOG_MSGID_I("Race I2C master send and receive fail!!!g_race_i2c_master_err_flag = 1",0);
                    return (void*)pEvt;
                    
                }
                for(i=0;i<pThisCmd->receive_bytes_in_one_packet * pThisCmd->receive_packet_length;i++)
                {
                    pEvt->receive_data[i]= receive_buff[i];
                    RACE_LOG_MSGID_I("Race I2C master receive data:0x%x",1,(uint8_t)receive_buff[i]);
                }
                pEvt->status = 0;
                RACE_LOG_MSGID_I("Race I2C master send and receive success",0);
            }
            else
            {
                pEvt->status = 1;// 
                RACE_LOG_MSGID_I("Race I2C master send and receive fail!!!",0);
            }
    }
    return (void*)pEvt;
}




void* RACE_CmdHandler_i2c_master_send_and_receive(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("pCmdMsg->hdr.id = %d", 1, (int)pCmdMsg->hdr.id);

    switch (pCmdMsg->hdr.id)
    {
        case RACE_CMD_I2C_MASTER_SEND_AND_RECEIVE: //0x5008
        {
            return race_cmdhdl_i2c_master_send_and_receive((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);
        }
        break;

        default:
            break;
    }

    return NULL;
}

#endif

