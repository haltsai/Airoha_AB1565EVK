/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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
 

#include "bt_aws_mce_le_association.h"
#include "bt_aws_mce_le_association_internal.h"
#include "bt_gap_le.h"
#include "bt_hci.h"
#include "bt_gattc.h"
#include "bt_gatts.h"
#include "syslog.h"

#define BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_IDLE 0x0
#define BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_AGENT_ADDRESS 0x1
#define BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_KEY 0x2
#define BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_CLIENT_ADDRESS 0x3
#define BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_AUDIO_LAT 0x4
#define BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_VOICE_LAT 0x5
#define BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_NUMBER 0x6
typedef uint8_t bt_aws_mce_le_association_client_state_t;
typedef struct {
    bt_aws_mce_le_association_state_t state;
    bt_aws_mce_le_association_client_state_t sub_state;
    bt_handle_t handle;
    bt_aws_mce_le_association_pair_mode_t mode;
    bt_aws_mce_le_association_service_t service;
    bt_aws_mce_le_association_client_info_t client;
    bt_aws_mce_le_association_agent_pair_cnf_t pair_cnf;

} bt_aws_mce_le_association_client_context_t;

static bt_aws_mce_le_association_client_context_t g_bt_aws_mce_le_assocation_client_context;

static bt_aws_mce_le_association_client_context_t *bt_aws_mce_le_association_client_get_context(void)
{
    return &g_bt_aws_mce_le_assocation_client_context;
}

static bool bt_aws_mce_le_association_check_char_type(bt_aws_mce_le_association_char_type_t type)
{
    if ((type == BT_AWS_MCE_LE_ASSOCIATION_CHAR_AGENT_ADDR) 
        || (type == BT_AWS_MCE_LE_ASSOCIATION_CHAR_SECRET_KEY)
        || (type == BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_ADDR)
        || (type == BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_AUDIO_LATENCY)
        || (type == BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_VOICE_LATENCY)
        || (type == BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_NO )
        || (type == BT_AWS_MCE_LE_ASSOCIATION_CHAR_CUSTOM_READ_DATA)
        || (type == BT_AWS_MCE_LE_ASSOCIATION_CHAR_CUSTOM_WRITE_DATA)) {

            return true;

    } else {

        return false;

    }

        
}

static bt_aws_mce_le_association_char_t *bt_aws_mce_le_association_get_char_by_type(bt_aws_mce_le_association_service_t *service, bt_aws_mce_le_association_char_type_t type)
{
    
    if (service == NULL) {
       return NULL;
    }

    uint32_t i; 
    for (i = 0; i < service->char_count; i++) {
        if (service->chara[i].type == type) {
            return &service->chara[i];
        }
    }

    return NULL;
    
}

static void bt_aws_mce_le_association_client_handle_error_and_notify_user(bt_aws_mce_le_association_client_context_t *client_cntx)
{
    memset(client_cntx, 0, sizeof(bt_aws_mce_le_association_client_context_t));

    bt_aws_mce_le_association_event_callback(BT_AWS_MCE_LE_ASSOCIATION_EVENT_AGENT_PAIR_CNF, BT_STATUS_FAIL, NULL, 0);


}

static bt_status_t bt_aws_mce_le_association_client_idle_handler(bt_aws_mce_le_association_client_context_t *client_cntx, bt_aws_mce_le_association_client_state_t next_state)
{
    bt_status_t ret = BT_STATUS_FAIL;

    if (next_state == BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_AGENT_ADDRESS) {

        bt_aws_mce_le_association_char_t *chara = bt_aws_mce_le_association_get_char_by_type(&client_cntx->service, BT_AWS_MCE_LE_ASSOCIATION_CHAR_AGENT_ADDR);

        if (chara == NULL) {
            /* Back to idle */
        } else {

            BT_GATTC_NEW_READ_CHARC_REQ(req, chara->value_handle);

            ret = bt_gattc_read_charc(client_cntx->handle, &req);

            if (ret == BT_STATUS_SUCCESS || ret == BT_STATUS_PENDING) {

                client_cntx->sub_state = BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_AGENT_ADDRESS;
                return BT_STATUS_SUCCESS;

            } else {
                /* Back to idle */

            }

        }

        /* Handle to idle */

    } else {
        /* Could not run this case */
    }
    return ret;
}


static bt_status_t bt_aws_mce_le_association_client_agent_addr_handler(bt_aws_mce_le_association_client_context_t *client_cntx, bt_aws_mce_le_association_client_state_t next_state)
{
     bt_status_t ret = BT_STATUS_FAIL;
     if (next_state == BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_KEY) {

        bt_aws_mce_le_association_char_t *chara = bt_aws_mce_le_association_get_char_by_type(&client_cntx->service, BT_AWS_MCE_LE_ASSOCIATION_CHAR_SECRET_KEY);

        if (chara == NULL) {
            /* Back to idle */
        } else {


            BT_GATTC_NEW_READ_CHARC_REQ(req, chara->value_handle);

            ret = bt_gattc_read_charc(client_cntx->handle, &req);

            if (ret == BT_STATUS_SUCCESS || ret == BT_STATUS_PENDING) {

                client_cntx->sub_state = BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_KEY;
                return BT_STATUS_SUCCESS;

            } else {
                /* Back to idle */

            }

        }

        /* Handle to idle */

    } else {
        /* Could not run this case */
    }

    return ret;
   
}

static bt_status_t bt_aws_mce_le_association_client_get_key_handler(bt_aws_mce_le_association_client_context_t *client_cntx, bt_aws_mce_le_association_client_state_t next_state)
{
    bt_status_t ret = BT_STATUS_FAIL;
     if (next_state == BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_CLIENT_ADDRESS) {

        bt_aws_mce_le_association_char_t *chara = bt_aws_mce_le_association_get_char_by_type(&client_cntx->service, BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_ADDR);

        if (chara == NULL) {
            /* Back to idle */
        } else {

            if (client_cntx->mode == BT_AWS_MCE_LE_ASSOCIATION_PAIR_MODE_NORMAL) {
                /* Pair ind should not write client addr */

            } else {

                uint8_t buff[25] = {0};

                BT_GATTC_NEW_WRITE_CHARC_REQ(req, buff, chara->value_handle,   &client_cntx->client.address, sizeof(bt_bd_addr_t))

                ret = bt_gattc_write_charc(client_cntx->handle, &req);

                if (ret == BT_STATUS_SUCCESS || ret == BT_STATUS_PENDING) {

                    client_cntx->sub_state = BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_CLIENT_ADDRESS;
                    return BT_STATUS_SUCCESS;

                } else {
                    /* Back to idle */
                    
                }
            }

        }

        /* Handle to idle */

    } else if (next_state == BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_AUDIO_LAT) {
        bt_aws_mce_le_association_char_t *chara = bt_aws_mce_le_association_get_char_by_type(&client_cntx->service, BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_AUDIO_LATENCY);

        if (chara == NULL) {
            /* Back to idle */
        } else {

            if (client_cntx->mode == BT_AWS_MCE_LE_ASSOCIATION_PAIR_MODE_NUMBERED) {
                /* Pair ind should not write audio latency */

            } else {

                uint8_t buff[25] = {0};

                BT_GATTC_NEW_WRITE_CHARC_REQ(req, buff, chara->value_handle, &client_cntx->client.audio_latency, sizeof(uint16_t));

                ret = bt_gattc_write_charc(client_cntx->handle, &req);


                if (ret == BT_STATUS_SUCCESS || ret == BT_STATUS_PENDING) {

                    client_cntx->sub_state = BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_AUDIO_LAT;
                    return BT_STATUS_SUCCESS;

                } else {
                    /* Back to idle */
                    
                }
            }

        }


    } else {
        /* Could not run this case */
    }

    return ret;
 

}


static bt_status_t bt_aws_mce_le_association_client_write_client_addr_handler(bt_aws_mce_le_association_client_context_t *client_cntx, bt_aws_mce_le_association_client_state_t next_state)
{

    bt_status_t ret = BT_STATUS_FAIL;

    if (next_state == BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_AUDIO_LAT) {

        bt_aws_mce_le_association_char_t *chara = bt_aws_mce_le_association_get_char_by_type(&client_cntx->service, BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_AUDIO_LATENCY);

        if (chara == NULL) {
            /* Back to idle */
        } else {

            if (client_cntx->mode == BT_AWS_MCE_LE_ASSOCIATION_EVENT_CLIENT_PAIR_IND) {
                /* Wrongly status */

            } else {

                uint8_t buff[25] = {0};

                BT_GATTC_NEW_WRITE_CHARC_REQ(req, buff, chara->value_handle, &client_cntx->client.audio_latency, sizeof(uint16_t));

                ret = bt_gattc_write_charc(client_cntx->handle, &req);


                if (ret == BT_STATUS_SUCCESS || ret == BT_STATUS_PENDING) {

                    client_cntx->sub_state = BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_AUDIO_LAT;
                    return BT_STATUS_SUCCESS;

                } else {
                    /* Back to idle */
                    
                }
            }

        }

        /* Handle to idle */

    } else {
        /* Could not run this case */
    }

    return ret;
 
}

static bt_status_t bt_aws_mce_le_association_client_write_audio_lat_handler(bt_aws_mce_le_association_client_context_t *client_cntx, bt_aws_mce_le_association_client_state_t next_state)
{
    bt_status_t ret = BT_STATUS_FAIL;
    if (next_state == BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_VOICE_LAT) {

    bt_aws_mce_le_association_char_t *chara = bt_aws_mce_le_association_get_char_by_type(&client_cntx->service, BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_VOICE_LATENCY);

    if (chara == NULL) {
        /* Back to idle */
    } else {

        uint8_t buff[25] = {0};

        BT_GATTC_NEW_WRITE_CHARC_REQ(req, buff, chara->value_handle, &client_cntx->client.voice_latency, sizeof(uint16_t));

        ret = bt_gattc_write_charc(client_cntx->handle, &req);


        if (ret == BT_STATUS_SUCCESS || ret == BT_STATUS_PENDING) {

            client_cntx->sub_state = BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_VOICE_LAT;

            return BT_STATUS_SUCCESS;

        } else {
            /* Back to idle */
            
        }

    }

    /* Handle to idle */

    } else {
    /* Could not run this case */
    }

    return ret;
 

}

static bt_status_t bt_aws_mce_le_association_client_write_voice_lat_handler(bt_aws_mce_le_association_client_context_t *client_cntx, bt_aws_mce_le_association_client_state_t next_state)
{
    bt_status_t ret = BT_STATUS_FAIL;
    if (next_state == BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_NUMBER) {

        bt_aws_mce_le_association_char_t *chara = bt_aws_mce_le_association_get_char_by_type(&client_cntx->service, BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_NO);

        if (chara == NULL) {
            /* Back to idle */
        } else {

            if (client_cntx->mode == BT_AWS_MCE_LE_ASSOCIATION_PAIR_MODE_NORMAL) {
                /* Don't need get number for normal mode */

            } else {


                BT_GATTC_NEW_READ_CHARC_REQ(req, chara->value_handle);

                ret = bt_gattc_read_charc(client_cntx->handle, &req);

                if (ret == BT_STATUS_SUCCESS || ret == BT_STATUS_PENDING) {

                    client_cntx->sub_state = BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_NUMBER;
                    return BT_STATUS_SUCCESS;

                } else {
                    /* Back to idle */
                    
                }
            }

        }

    /* Handle to idle */

    } else {
    /* Could not run this case */
    }

    return ret;
 

}

static bt_status_t bt_aws_mce_le_association_client_get_number_handler(bt_aws_mce_le_association_client_context_t *client_cntx, bt_aws_mce_le_association_client_state_t next_state)
{
    /* Currently this status is no useful */
    return BT_STATUS_SUCCESS;


}

bt_status_t bt_aws_mce_le_association_client_pairing_state_handler(bt_aws_mce_le_association_client_state_t next_state)
{
    bt_aws_mce_le_association_client_context_t *client_cntx = bt_aws_mce_le_association_client_get_context();
    bt_status_t ret = BT_STATUS_FAIL;
    
    LOG_MSGID_I(common, "main state(%d), state(%d), new_state(%d)",3, client_cntx->state, client_cntx->sub_state, next_state);

    if (client_cntx->state == BT_AWS_MCE_LE_ASSOCIATION_STATE_IDLE) {
        return BT_STATUS_FAIL;
    }
    switch (client_cntx->sub_state) {
        case BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_IDLE:
        {
            ret = bt_aws_mce_le_association_client_idle_handler(client_cntx, next_state);
            break;
        }
        case BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_AGENT_ADDRESS:
        {
            ret = bt_aws_mce_le_association_client_agent_addr_handler(client_cntx, next_state);
            break;
        }
        case BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_KEY:
        {

            ret = bt_aws_mce_le_association_client_get_key_handler(client_cntx, next_state);
            break;
        }
        case BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_CLIENT_ADDRESS:
        {
            ret = bt_aws_mce_le_association_client_write_client_addr_handler(client_cntx, next_state);
            break;
        }
        case BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_AUDIO_LAT:
        {
            ret = bt_aws_mce_le_association_client_write_audio_lat_handler(client_cntx, next_state);
            break;
        }
        case BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_VOICE_LAT:
        {
            ret = bt_aws_mce_le_association_client_write_voice_lat_handler(client_cntx, next_state);
            break;
        }
        case BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_NUMBER:
        {
            ret = bt_aws_mce_le_association_client_get_number_handler(client_cntx, next_state);
            break;
        }
        default: {
            break;
        }
    }

    LOG_MSGID_I(common, "handler result(%d)",1, ret);
    return ret;
    
}



bt_status_t bt_aws_mce_le_association_client_start_pairing(bt_aws_mce_le_association_pair_agent_req_t *req)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    uint32_t i;


    if (req == NULL || req->handle == BT_HANDLE_INVALID
        || req->service == NULL || req->service->char_count == 0
        || req->client == NULL) {
        return BT_STATUS_LE_ASSOCIATION_PARAMETER_ERR;
    }

    bt_aws_mce_le_association_client_context_t *client_cntx = bt_aws_mce_le_association_client_get_context();


    /* Evaluate service */

    for (i = 0; i < req->service->char_count; i++) {

        if (bt_aws_mce_le_association_check_char_type(req->service->chara[i].type) == false) {
            return BT_STATUS_LE_ASSOCIATION_PARAMETER_ERR;
        }

        memcpy(&client_cntx->service.chara[i], &req->service->chara[i], sizeof(bt_aws_mce_le_association_char_t));

    }

    client_cntx->handle = req->handle;
    client_cntx->mode = req->mode;
    client_cntx->service.char_count = req->service->char_count;
    client_cntx->state = BT_AWS_MCE_LE_ASSOCIATION_STATE_PAIRING;
    memcpy(&client_cntx->client, req->client, sizeof(bt_aws_mce_le_association_client_info_t));

    client_cntx->sub_state = BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_IDLE;
    ret = bt_aws_mce_le_association_client_pairing_state_handler(BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_AGENT_ADDRESS);

    return ret;

}


#if 0
bt_status_t bt_aws_mce_le_association_client_pair_agent(bt_aws_mce_le_association_pair_mode_t mode, bt_aws_mce_le_association_client_info_t *client)
{

    bt_status_t ret;

    do {

        if (client == NULL) {

            ret = BT_STATUS_LE_ASSOCIATION_PARAMETER_ERR;
            break;

        }


    } while(0);



}
#endif

bt_status_t bt_aws_mce_le_association_client_event_handler(bt_msg_type_t msg, bt_status_t status, void *buffer)
{

    LOG_MSGID_I(common, "association_event_handler msg(%d), status(%x)",2, msg, status);
    bt_aws_mce_le_association_client_context_t *client_cntx = bt_aws_mce_le_association_client_get_context();
    bt_status_t ret = BT_STATUS_SUCCESS;
    switch (msg) {
        case BT_GAP_LE_DISCONNECT_IND:
        {

            bt_hci_evt_disconnect_complete_t *disconn = (bt_hci_evt_disconnect_complete_t *) buffer;


            if (disconn->connection_handle == client_cntx->handle 
                && client_cntx->mode == BT_AWS_MCE_LE_ASSOCIATION_STATE_PAIRING) {
                /* Enter IDLE mode and notify user */
                ret = BT_STATUS_FAIL;
            }

            break;

        }
        case BT_GATTC_READ_CHARC:
        {

            LOG_MSGID_I(common, "association_event_handler(read_charac) :state(%x), substate:(%x)",2, client_cntx->state, client_cntx->sub_state);
            if (client_cntx->state == BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_IDLE) {
                /* Back to idle */
                break;
            }

            bt_gattc_read_rsp_t *read_char = (bt_gattc_read_rsp_t *)buffer;
            uint16_t length = read_char->length - 1;

            if (read_char->connection_handle != client_cntx->handle) {
                break;
            }

            if (status != BT_STATUS_SUCCESS) {
                /* Error handle */
                ret = BT_STATUS_FAIL;
                break;
            }

            if (client_cntx->state == BT_AWS_MCE_LE_ASSOCIATION_STATE_CUSTOM_INTERACTION)
            {
                uint8_t temp_data[BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN] = {0};
                bt_aws_mce_le_association_read_custom_data_cnf_t cnf;
                cnf.handle = client_cntx->handle;
                if (length >= BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN)
                {
                    length = BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN;
                }
                memcpy(temp_data, read_char->att_rsp->attribute_value, length);
                cnf.info.data = temp_data;
                LOG_MSGID_I(common, "association_event_handler: read_custom_data: lenth (%d), data[%x-%x-%x-%x]",5, length, cnf.info.data[0],
                    cnf.info.data[1], cnf.info.data[2], cnf.info.data[3]);
                client_cntx->state = BT_AWS_MCE_LE_ASSOCIATION_STATE_IDLE;
                bt_aws_mce_le_association_event_callback(BT_AWS_MCE_LE_ASSOCIATION_EVENT_READ_CUSTOM_DATA_CNF, BT_STATUS_SUCCESS, &cnf, sizeof(bt_aws_mce_le_association_read_custom_data_cnf_t));
                break;
            }

            switch (client_cntx->sub_state)
            {
                case BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_AGENT_ADDRESS:
                {
                    if (length != sizeof(bt_bd_addr_t)) {
                        /* Error handle */
                        ret = BT_STATUS_FAIL;
                        break;
                    }
                    memcpy(&client_cntx->pair_cnf.info.address, read_char->att_rsp->attribute_value, length);
                    /* Enter next stage */
                    ret = bt_aws_mce_le_association_client_pairing_state_handler(BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_KEY);

                    break;

                }
                case BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_KEY:
                {
                    if (length != 16) {
                        /* Error handle */
                        ret = BT_STATUS_FAIL;
                        break;
                    }
                    
                    memcpy(client_cntx->pair_cnf.info.secret_key, read_char->att_rsp->attribute_value, length);
                    /* Enter next stage */
                    if (client_cntx->mode == BT_AWS_MCE_LE_ASSOCIATION_PAIR_MODE_NORMAL) {

                        ret = bt_aws_mce_le_association_client_pairing_state_handler(BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_AUDIO_LAT);
                    } else {

                        ret = bt_aws_mce_le_association_client_pairing_state_handler(BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_CLIENT_ADDRESS);

                    }
                    break;

                }
                case BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_NUMBER:
                {
                    if (length != sizeof(uint16_t)) {
                        /* Error handle */
                        ret = BT_STATUS_FAIL;
                        break;
                    }

                    memcpy(&client_cntx->pair_cnf.number, read_char->att_rsp->attribute_value, length);
                    /* Need enter IDLE before notify application. */

                    client_cntx->state = BT_AWS_MCE_LE_ASSOCIATION_STATE_IDLE;
                    client_cntx->sub_state = BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_IDLE;

                    client_cntx->pair_cnf.handle = client_cntx->handle;

                    bt_aws_mce_le_association_event_callback(BT_AWS_MCE_LE_ASSOCIATION_EVENT_AGENT_PAIR_CNF, BT_STATUS_SUCCESS, &client_cntx->pair_cnf, sizeof(bt_aws_mce_le_association_agent_pair_cnf_t));
                    break;

                }
                default:
                {
                    /* No handle */
                    break;
                }
            }

            break;
        }
        case BT_GATTC_WRITE_CHARC:
        {

            LOG_MSGID_I(common, "association_write(%d), sub_state(%d)",2, client_cntx->state, client_cntx->sub_state);
            if (client_cntx->state == BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_IDLE) {
                /* Back to idle */
                break;
            }

            bt_gattc_write_rsp_t *write_rsp = (bt_gattc_write_rsp_t *)buffer;

            if (write_rsp->connection_handle != client_cntx->handle) {
                break;
            }

            if (status != BT_STATUS_SUCCESS) {
                ret = BT_STATUS_FAIL;
                /* Error handle */
            }
            if (client_cntx->state == BT_AWS_MCE_LE_ASSOCIATION_STATE_CUSTOM_INTERACTION)
            {
                bt_aws_mce_le_association_write_custom_data_cnf_t data_cnf;
                data_cnf.handle = client_cntx->handle;
                client_cntx->state = BT_AWS_MCE_LE_ASSOCIATION_STATE_IDLE;

                bt_aws_mce_le_association_event_callback(BT_AWS_MCE_LE_ASSOCIATION_EVENT_WRITE_CUSTOM_DATA_CNF, BT_STATUS_SUCCESS, &data_cnf, sizeof(bt_aws_mce_le_association_write_custom_data_cnf_t));
                break;
            }

            switch (client_cntx->sub_state)
            {
                case BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_CLIENT_ADDRESS:
                {

                    /* Enter next stage */
                    ret = bt_aws_mce_le_association_client_pairing_state_handler(BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_AUDIO_LAT);
                    break;
                }
                case BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_AUDIO_LAT:
                {
                    /* Enter next stage */
                    ret = bt_aws_mce_le_association_client_pairing_state_handler(BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_VOICE_LAT);
                    break;

                }
                case BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_WRITE_VOICE_LAT:
                {

                    if (client_cntx->mode == BT_AWS_MCE_LE_ASSOCIATION_PAIR_MODE_NUMBERED) {
                        /* Enter next stage */
                        ret = bt_aws_mce_le_association_client_pairing_state_handler(BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_GET_NUMBER);

                    } else {
                        /* Number always be zero for normal mode. */
                                    
                        client_cntx->state = BT_AWS_MCE_LE_ASSOCIATION_STATE_IDLE;
                        client_cntx->sub_state = BT_AWS_MCE_LE_ASSOCIATION_CLIENT_STATE_IDLE;
                        client_cntx->pair_cnf.number = 0;
                        client_cntx->pair_cnf.handle = client_cntx->handle;

                        bt_aws_mce_le_association_event_callback(BT_AWS_MCE_LE_ASSOCIATION_EVENT_AGENT_PAIR_CNF, BT_STATUS_SUCCESS, &client_cntx->pair_cnf,  sizeof(bt_aws_mce_le_association_agent_pair_cnf_t));
                    }

                    break;
                }
                default:
                    break;
            }
        }
    }

    if (ret != BT_STATUS_SUCCESS) {

        bt_aws_mce_le_association_client_handle_error_and_notify_user(client_cntx);

    }

    return ret;

}


bt_status_t bt_aws_mce_le_association_client_check_adv_data(void *buffer, uint16_t buffer_len, bt_firmware_type_t mode)
{
    //uint32_t i;
    bt_status_t ret = BT_STATUS_FAIL;
    uint8_t length, type;
    uint8_t *buff = (uint8_t *)buffer;
    uint8_t *buffer_end = (uint8_t *)buffer + buffer_len;
    char adv_data[] = "AWS 1.0";
    LOG_HEXDUMP_I(common, "adv raw", buff, buffer_len);
    if (buff && (buff[0] !=0)) {
        while ((buff < buffer_end) && (buff[0] != 0)) {
            length = *buff++;
            type = *buff;
            if (mode  == BT_FIRMWARE_TYPE_SPEAKER || mode == BT_AWS_MCE_SRV_MODE_BROADCAST) {
                mode = 0x01;//BT_FIRMWARE_TYPE_SPEAKER
            }
            if (type == 0xFF) {
                if (memcmp(buff + 1, adv_data, sizeof(adv_data) - 1) == 0 && (*(buff + sizeof(adv_data)) == mode)) {
                    ret = BT_STATUS_SUCCESS;
                    break;
                }

            }
            buff += length - 1;
        }

    }
    return ret;

}

bt_status_t bt_aws_mce_le_association_read_custom_data(bt_handle_t handle)
{
    bt_status_t ret = BT_STATUS_FAIL;
    bt_aws_mce_le_association_client_context_t *client_cntx = bt_aws_mce_le_association_client_get_context();
    if (client_cntx == NULL || handle != client_cntx->handle) {
        return ret;
    }
    bt_aws_mce_le_association_char_t *chara = bt_aws_mce_le_association_get_char_by_type(&client_cntx->service, BT_AWS_MCE_LE_ASSOCIATION_CHAR_CUSTOM_READ_DATA);
    
    if (chara == NULL) {
        /* Back to idle */
    } else {
        BT_GATTC_NEW_READ_CHARC_REQ(req, chara->value_handle);
        ret = bt_gattc_read_charc(client_cntx->handle, &req);
        if (ret == BT_STATUS_SUCCESS || ret == BT_STATUS_PENDING) {
            client_cntx->state = BT_AWS_MCE_LE_ASSOCIATION_STATE_CUSTOM_INTERACTION;
            return BT_STATUS_SUCCESS;
        } else {
            /* Back to idle */
        
        }
    }
    return ret;   
}

bt_status_t bt_aws_mce_le_association_write_custom_data(bt_handle_t handle,  uint8_t *data, uint8_t len)
{
    bt_status_t ret = BT_STATUS_FAIL;
    if (len > BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN) {
        ret = BT_STATUS_LE_ASSOCIATION_PARAMETER_ERR;
        return ret;
    }
    bt_aws_mce_le_association_client_context_t *client_cntx = bt_aws_mce_le_association_client_get_context();
    LOG_MSGID_I(common, "[]scan len %x, type %x",2, client_cntx, len);
    if (client_cntx == NULL || handle != client_cntx->handle) {
        return ret;
    }

    bt_aws_mce_le_association_char_t *chara = bt_aws_mce_le_association_get_char_by_type(&client_cntx->service, BT_AWS_MCE_LE_ASSOCIATION_CHAR_CUSTOM_WRITE_DATA);
    if (chara == NULL) {
        return ret;
    }

    uint8_t buff[25] = {0};

    BT_GATTC_NEW_WRITE_CHARC_REQ(req, buff, chara->value_handle, data, len);

    ret = bt_gattc_write_charc(client_cntx->handle, &req);


    if (ret == BT_STATUS_SUCCESS || ret == BT_STATUS_PENDING) {
        client_cntx->state = BT_AWS_MCE_LE_ASSOCIATION_STATE_CUSTOM_INTERACTION;
        return BT_STATUS_SUCCESS;
    } else {
        /* Back to idle */
    }
    return ret;
}

