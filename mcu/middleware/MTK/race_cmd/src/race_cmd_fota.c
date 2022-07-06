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
#ifdef RACE_FOTA_CMD_ENABLE
#include "stdio.h"
#include "fota_platform.h"
#include "fota_util.h"
#include "fota_flash.h"
#include "fota_multi_info.h"
#include "fota_multi_info_util.h"
#include "race_lpcomm_aws.h"
#include "race_util.h"
#include "race_xport.h"
#include "race_cmd_fota.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_trans.h"
#include "race_lpcomm_conn.h"
#include "race_lpcomm_msg_struct.h"
#include "race_fota_util.h"
#include "race_noti.h"
#include "race_bt.h"
#include "race_lpcomm_ps_noti.h"
#include "race_storage_access.h"
#include "race_storage_util.h"
#include "race_timer.h"
#include "race_lpcomm_packet.h"
#include "race_fota.h"


////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    uint8_t app_id;
    uint8_t channel_id;
    uint32_t signature_start_address;
    uint8_t storage_type;
#ifdef RACE_FOTA_INTEGRITY_CHECK_ENHANCE_ENABLE
    race_fota_check_integrity_noti_struct *noti;
#endif
}race_fota_integrity_check_info_struct;


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void* RACE_CmdHandler_FOTA_commit(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(pCmdMsg);
    UNUSED(length);

    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    uint8_t integrity_res = 0xFF;
    int32_t ret = RACE_ERRCODE_FAIL;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_FOTA_COMMIT,
                                      sizeof(RSP),
                                      channel_id);

    if (pEvt)
    {
        ret = fota_dl_integrity_res_read(&integrity_res);
        if (FOTA_ERRCODE_SUCCESS == ret &&
            FOTA_DL_INTEGRITY_RES_VAL_PASS == integrity_res)
        {
            ret = fota_upgrade_flag_set();
            if (FOTA_ERRCODE_SUCCESS == ret)
            {
                /* If failed, FOTA upgrade will be triggered in the other cause of reboot.
                               * No error handle here.
                               */
                race_event_send_fota_need_reboot_event(NULL, 0);
            }
        }
        else
        {
            ret = FOTA_ERRCODE_FAIL;
        }

        if (FOTA_ERRCODE_SUCCESS == ret)
        {
            race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_COMMITING);
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
            race_fota_set_sp_trans_method_by_channel_id(channel_id);
#endif
        }

        pEvt->status = FOTA_ERRCODE_SUCCESS == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
    }

    return pEvt;
}


/* RACE_FOTA_STOP cmd does not interrupt on-going RHO or FOTA_reboot */
void* RACE_CmdHandler_FOTA_stop(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);

    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t sender;
        uint8_t recipient;
        uint8_t reason;
    } PACKED CMD;

    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_FOTA_STOP,
                                      sizeof(RSP),
                                      channel_id);
    race_recipient_type_enum recipient_type = race_recipient_type_convt(pCmd->recipient);
    int32_t ret = RACE_ERRCODE_SUCCESS;

    RACE_LOG_MSGID_I("sender:%x recipient:%x reason:%x",3,
               pCmd->sender,
               pCmd->recipient,
               pCmd->reason);

    if (pEvt != NULL)
    {
        race_fota_stop_agent_sp_stop_req_struct sp_req = {0};

        if (RACE_FOTA_STOP_REASON_SP_LOST <= pCmd->reason)
        {
            RACE_LOG_MSGID_W("wrong reason from SP:%x",1, pCmd->reason);
            pCmd->reason = RACE_FOTA_STOP_REASON_CANCEL;
        }

        sp_req.channel_id = channel_id;
        sp_req.recipient_type = recipient_type;
        sp_req.sender = pCmd->sender;
        sp_req.reason = pCmd->reason;

        ret = race_fota_stop_agent_sp_stop_req_process(&sp_req);

        pEvt->status = ret;
    }

    return pEvt;
}


void* RACE_CmdHandler_FOTA_query_state(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(pCmdMsg);
    UNUSED(length);

    typedef struct
    {
        uint8_t status;
        uint16_t fota_state;
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_FOTA_QUERY_STATE,
                                      sizeof(RSP),
                                      channel_id);

    if (pEvt != NULL)
    {
        ret = fota_state_read(&(pEvt->fota_state));
        ret = FOTA_ERRCODE_SUCCESS == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
        pEvt->status = ret;
    }

    return pEvt;
}


void* RACE_CmdHandler_FOTA_write_state(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(pCmdMsg);
    UNUSED(length);

    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint16_t fota_state;
    } PACKED CMD;

    typedef struct
    {
        uint8_t status;
        uint16_t fota_state;
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_FOTA_WRITE_STATE,
                                      sizeof(RSP),
                                      channel_id);

    if (pEvt != NULL)
    {
        pEvt->fota_state = pCmd->fota_state;
        ret = fota_state_write(pEvt->fota_state);
        pEvt->status = FOTA_ERRCODE_SUCCESS == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
    }

    return pEvt;
}


void* RACE_CmdHandler_FOTA_query_partition_info(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);

    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t partition_id;
    } PACKED CMD;

    typedef struct
    {
        uint8_t status;
        uint8_t partition_id;
        uint8_t storage_type;
        uint32_t partition_address;
        uint32_t partition_length;
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD * pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = NULL;

    pEvt = RACE_ClaimPacketAppID(pCmd->cmdhdr.pktId.field.app_id,
                                 RACE_TYPE_RESPONSE,
                                 RACE_FOTA_QUERY_PARTITION_INFO,
                                 sizeof(RSP),
                                 channel_id);
    if (pEvt != NULL)
    {
        pEvt->partition_id = (race_storage_partition_id_enum)pCmd->partition_id;

        switch (pEvt->partition_id)
        {
            case RACE_STORAGE_PARTITION_ID_FOTA:
            {
               ret = fota_flash_get_fota_partition_info(&(pEvt->storage_type),
                                                                  &(pEvt->partition_address),
                                                                  &(pEvt->partition_length));
                pEvt->status = FOTA_ERRCODE_SUCCESS == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
                /* Reduce the last 4K which is used to store history states or upgrade info. */
                pEvt->partition_length -= 0x1000;
                break;
            }

            case RACE_STORAGE_PARTITION_ID_FS:
            {
                /* 1530 feature only which merges filesystem sector and fota sector as a new extended fota sector. */
                pEvt->status = (uint8_t)RACE_ERRCODE_NOT_SUPPORT;
                break;
            }

            default:
            {
                pEvt->status = (uint8_t)RACE_ERRCODE_NOT_SUPPORT;
                break;
            }
        }
    }

    return pEvt;
}


#ifndef RACE_FOTA_INTEGRITY_CHECK_ENHANCE_ENABLE
RACE_ERRCODE race_fota_integrity_check_sha256_generate_cb(uint8_t status,
                                                                       uint32_t data_start_address,
                                                                       uint32_t data_length,
                                                                       uint8_t storage_type,
                                                                       unsigned char sha256_generated[RACE_STORAGE_SHA256_SIZE],
                                                                       void *user_data)
{
    typedef struct
    {
        uint8_t status;
        uint8_t storage_type;
    } PACKED RSP;

    uint32_t ret = RACE_ERRCODE_FAIL;
    race_fota_integrity_check_info_struct *cb_user_data = (race_fota_integrity_check_info_struct *)user_data;
    RSP *pEvt = NULL;

    RACE_LOG_MSGID_I("status:%d data_start_address:%x data_length:%d storage_type:%d user_data:%x",5,
               status,
               data_start_address,
               data_length,
               storage_type,
               user_data);

    if (!cb_user_data)
    {
        RACE_LOG_MSGID_E("Empty user_data!", 0);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    pEvt = RACE_ClaimPacketAppID(cb_user_data->app_id,
                                 RACE_TYPE_RESPONSE,
                                 RACE_FOTA_CHECK_INTEGRITY,
                                 sizeof(RSP),
                                 cb_user_data->channel_id);

    if (pEvt)
    {
        if (RACE_ERRCODE_SUCCESS == status)
        {
            ret = race_fota_integrity_check(cb_user_data->signature_start_address,
                                            cb_user_data->storage_type,
                                            (uint8_t *)(&sha256_generated[0]),
                                            RACE_STORAGE_SHA256_SIZE);
        }

        pEvt->status = ret;
        pEvt->storage_type = cb_user_data->storage_type;
        ret = race_flush_packet((uint8_t *)pEvt, cb_user_data->channel_id);
        ret = RACE_STATUS_OK == ret ? RACE_ERRCODE_SUCCESS: RACE_ERRCODE_FAIL;
    }
    else
    {
        ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
    }

    race_mem_free(cb_user_data);
    return ret;
}


void* RACE_CmdHandler_FOTA_check_integrity(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);

    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t storage_type;
    } PACKED CMD;

    typedef struct
    {
        uint8_t status;
        uint8_t storage_type;
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD * pCmd = (CMD *)pCmdMsg;

    RSP * pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                       RACE_TYPE_RESPONSE,
                                       RACE_FOTA_CHECK_INTEGRITY,
                                       sizeof(RSP),
                                       channel_id);

    if (pEvt != NULL)
    {
        fota_integrity_check_type_enum integrity_check_type = FOTA_INTEGRITY_CHECK_TYPE_MAX;
        uint32_t signature_start_address = 0, data_start_address = 0;
        uint32_t data_length = 0;
        FotaStorageType storage_type = Invalid;
        unsigned char sha256_generated[RACE_STORAGE_SHA256_SIZE] = {0};

        pEvt->storage_type = pCmd->storage_type;
        ret = fota_get_integrity_check_info(&integrity_check_type,
                                            &signature_start_address,
                                            &data_start_address,
                                            &data_length,
                                            &storage_type);
        if (FOTA_ERRCODE_SUCCESS == ret)
        {
            RACE_LOG_MSGID_I("integrity_check data_start_address:%x data_length:%d storage_type:%d",3,
                       data_start_address,
                       data_length,
                       storage_type);
            if (pCmd->storage_type == storage_type)
            {
                switch (integrity_check_type)
                {
                    case FOTA_INTEGRITY_CHECK_TYPE_SHA256:
                    {
                        race_fota_integrity_check_info_struct *user_data = NULL;

                        user_data = race_mem_alloc(sizeof(race_fota_integrity_check_info_struct));
                        if (user_data)
                        {
                            user_data->app_id = pCmdMsg->pktId.field.app_id;
                            user_data->channel_id = channel_id;
                            user_data->signature_start_address = signature_start_address;
                            user_data->storage_type = storage_type;
                            ret = race_storage_nb_sha256_generate(data_start_address,
                                                                  data_length,
                                                                  storage_type,
                                                                  sha256_generated,
                                                                  race_fota_integrity_check_sha256_generate_cb,
                                                                  (void *)user_data);
                            RACE_LOG_MSGID_I("race_storage_nb_sha256_generate ret:%d",1, ret);
                        }
                        else
                        {
                            ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
                        }

                        if (RACE_ERRCODE_MORE_OPERATION != ret)
                        {
                            race_mem_free(user_data);
                            user_data = NULL;

                            if (RACE_ERRCODE_SUCCESS == ret)
                            {
                                ret = race_fota_integrity_check(signature_start_address,
                                                                storage_type,
                                                                (uint8_t *)(&sha256_generated[0]),
                                                                RACE_STORAGE_SHA256_SIZE);
                            }
                        }
                        else
        {
                            ret = RACE_ERRCODE_SUCCESS;
                            /* Send the 5B at race_fota_integrity_check_sha256_generate_cb() */
                            RACE_FreePacket(pEvt);
                            return NULL;
                        }

                break;
                    }

                    case FOTA_INTEGRITY_CHECK_TYPE_CRC32:
                    case FOTA_INTEGRITY_CHECK_TYPE_SHA256_RSA:
                    {
                        RACE_LOG_MSGID_E("integrity_check_type:%d not implemented.",1, integrity_check_type);
                        ret = RACE_ERRCODE_NOT_SUPPORT;
                break;
                    }

            default:
                    {
                ret = RACE_ERRCODE_NOT_SUPPORT;
                break;
        }
                }
            }
            else
            {
                ret = RACE_ERRCODE_CONFLICT;
            }
        }
        else
        {
            ret = RACE_ERRCODE_FAIL;
        }

        pEvt->status = ret;
    }

    return pEvt;
}

#else

RACE_ERRCODE race_fota_integrity_check_sha256_generate_cb(uint8_t status,
                                                                       uint32_t data_start_address,
                                                                       uint32_t data_length,
                                                                       uint8_t storage_type,
                                                                       unsigned char sha256_generated[RACE_STORAGE_SHA256_SIZE],
                                                                       void *user_data)
{
    uint32_t ret = RACE_ERRCODE_FAIL;
    race_fota_integrity_check_info_struct *cb_user_data = (race_fota_integrity_check_info_struct *)user_data;

    RACE_LOG_MSGID_I("status:%d data_start_address:%x data_length:%d storage_type:%d user_data:%x",5,
               status,
               data_start_address,
               data_length,
               storage_type,
               user_data);

    if (!cb_user_data)
    {
        RACE_LOG_MSGID_E("Empty user_data!", 0);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

        if (RACE_ERRCODE_SUCCESS == status)
        {
        status = race_fota_integrity_check(cb_user_data->signature_start_address,
                                            cb_user_data->storage_type,
                                            (uint8_t *)(&sha256_generated[0]),
                                            RACE_STORAGE_SHA256_SIZE);
        }

    cb_user_data->noti->status = status;
    ret = race_noti_send(cb_user_data->noti, cb_user_data->channel_id, TRUE);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        RACE_FreePacket(cb_user_data->noti);
    }

    race_mem_free(cb_user_data);
    return ret;
}


void* RACE_CmdHandler_FOTA_check_integrity(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);

    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t recipient_count;
        race_fota_check_integrity_recipient_param_struct recipient_param[0];
    } PACKED CMD;

    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD * pCmd = (CMD *)pCmdMsg;

    RSP * pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                       RACE_TYPE_RESPONSE,
                                       RACE_FOTA_CHECK_INTEGRITY,
                                       sizeof(RSP),
                                       channel_id);
    race_fota_check_integrity_recipient_param_struct *agent_recipient_param = NULL;
    race_fota_check_integrity_recipient_param_struct *partner_recipient_param = NULL;
    race_fota_check_integrity_noti_struct *noti = NULL;
    race_fota_integrity_check_info_struct *user_data = NULL;

    if (pEvt != NULL)
    {
        ret = race_recipient_param_parse(pCmd->recipient_count,
                                         (uint8_t *)pCmd->recipient_param,
                                         sizeof(race_fota_check_integrity_recipient_param_struct),
                                         (uint8_t **)&agent_recipient_param,
                                         (uint8_t **)&partner_recipient_param);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            goto exit;
        }

        if (agent_recipient_param && !partner_recipient_param)
        {
            /* Agent only */
        fota_integrity_check_type_enum integrity_check_type = FOTA_INTEGRITY_CHECK_TYPE_MAX;
        uint32_t signature_start_address = 0, data_start_address = 0;
        uint32_t data_length = 0;
        FotaStorageType storage_type = Invalid;
        unsigned char sha256_generated[RACE_STORAGE_SHA256_SIZE] = {0};

        ret = fota_get_integrity_check_info(&integrity_check_type,
                                            &signature_start_address,
                                            &data_start_address,
                                            &data_length,
                                            &storage_type);
        if (FOTA_ERRCODE_SUCCESS == ret)
        {
            RACE_LOG_MSGID_I("integrity_check data_start_address:%x data_length:%d storage_type:%d",3,
                       data_start_address,
                       data_length,
                       storage_type);
                if (agent_recipient_param->storage_type == storage_type)
            {
                switch (integrity_check_type)
                {
                    case FOTA_INTEGRITY_CHECK_TYPE_SHA256:
                    {
                            /* A2. Create the noti(to Smart Phone). */
                            noti = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                                         RACE_TYPE_NOTIFICATION,
                                                         RACE_FOTA_CHECK_INTEGRITY,
                                                         sizeof(race_fota_check_integrity_noti_struct) + sizeof(race_fota_check_integrity_recipient_param_struct),
                                                         channel_id);
                            if (noti)
                            {
                                /* A3. Set the noti parameters with the cmd results.  */
                                noti->recipient_count = pCmd->recipient_count;
                                memcpy(noti->recipient_param, pCmd->recipient_param, sizeof(race_fota_check_integrity_recipient_param_struct));
                            }
                            else
                            {
                                ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
                                goto exit;
                            }

                        user_data = race_mem_alloc(sizeof(race_fota_integrity_check_info_struct));
                        if (user_data)
                        {
                            user_data->app_id = pCmdMsg->pktId.field.app_id;
                            user_data->channel_id = channel_id;
                            user_data->signature_start_address = signature_start_address;
                            user_data->storage_type = storage_type;
                                user_data->noti = noti;
                            ret = race_storage_nb_sha256_generate(data_start_address,
                                                                  data_length,
                                                                  storage_type,
                                                                  sha256_generated,
                                                                  race_fota_integrity_check_sha256_generate_cb,
                                                                  (void *)user_data);
                            RACE_LOG_MSGID_I("race_storage_nb_sha256_generate ret:%d",1, ret);
                        }
                        else
                        {
                            ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
                                goto exit;
                        }

                        if (RACE_ERRCODE_MORE_OPERATION != ret)
                        {
                                /*  Get sha256 done. */
                            if (RACE_ERRCODE_SUCCESS == ret)
                            {
                                    ret = race_fota_integrity_check_sha256_generate_cb(RACE_ERRCODE_SUCCESS,
                                                                                 data_start_address,
                                                                                 data_length,
                                                                storage_type,
                                                                                 sha256_generated,
                                                                                 (void *)user_data);
                                    /* cb will free user_data and noti and send 5D no matter what the return value is. */
                                    user_data = NULL;
                                    noti = NULL;
                            }
                        }
                        else
        {
                            ret = RACE_ERRCODE_SUCCESS;
                        }

                break;
                    }

                    case FOTA_INTEGRITY_CHECK_TYPE_CRC32:
                    case FOTA_INTEGRITY_CHECK_TYPE_SHA256_RSA:
                    {
                        RACE_LOG_MSGID_E("integrity_check_type:%d not implemented.",1, integrity_check_type);
                        ret = RACE_ERRCODE_NOT_SUPPORT;
                break;
                    }

            default:
                    {
                ret = RACE_ERRCODE_NOT_SUPPORT;
                break;
        }
                }
            }
            else
            {
                ret = RACE_ERRCODE_CONFLICT;
            }
        }
        else
        {
            ret = RACE_ERRCODE_FAIL;
        }
        }
        else if (!agent_recipient_param && partner_recipient_param)
        {
            /* Partner only */
#ifndef RACE_LPCOMM_ENABLE
            ret = RACE_ERRCODE_NOT_SUPPORT;
#else
            race_lpcomm_fota_check_integrity_req_struct req = {0};
            uint16_t process_id = race_gen_process_id();

            /* 5. Send the req to the peer */
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            req.storage_type = partner_recipient_param->storage_type;
            ret = race_lpcomm_send_race_cmd_req_to_peer((uint8_t *)&req,
                                                        sizeof(race_lpcomm_fota_check_integrity_req_struct),
                                                        RACE_LPCOMM_ROLE_AGENT,
                                                        RACE_FOTA_CHECK_INTEGRITY,
                                                        pCmdMsg->pktId.field.app_id,
                                                        channel_id,
                                                        process_id,
#ifdef RACE_AWS_ENABLE
                                                        RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                                        RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                                        RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif
#endif /* RACE_LPCOMM_ENABLE */
        }
        else
        {
            ret = RACE_ERRCODE_NOT_SUPPORT;
        }
     }

exit:
    if (pEvt)
    {
        pEvt->status = ret;
    }

    if (RACE_ERRCODE_SUCCESS != ret)
    {
        if (noti)
        {
            RACE_FreePacket(noti);
            noti = NULL;
        }

        if (user_data)
        {
            race_mem_free(user_data);
        }
    }

    return pEvt;
}
#endif


void* RACE_CmdHandler_FOTA_dual_devices_new_transaction(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);

    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_FOTA_DUAL_DEVICES_NEW_TRANSACTION,
                                      sizeof(RSP),
                                      channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;
#ifdef RACE_LPCOMM_ENABLE
    race_fota_dual_start_transaction_noti_struct *noti = NULL;
    /* process_id must be initialized to 0. */
    uint16_t process_id = 0;
    bool noti_sent = FALSE;
#endif

    if (pEvt != NULL)
    {
#ifndef RACE_LPCOMM_ENABLE
        ret = RACE_ERRCODE_NOT_SUPPORT;
#else
        /* 1. Create Noti */
        ret = race_lpcomm_ps_noti_create((void **)&noti,
                                         &process_id,
                                         RACE_FOTA_DUAL_DEVICES_NEW_TRANSACTION,
                                         pCmdMsg->pktId.field.app_id,
                                         TRUE,
                                         sizeof(race_fota_dual_start_transaction_noti_struct),
                                         channel_id);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            // 5B fail
            ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
            goto exit;
        }

        /* 2. Store the const parameters in noti. */
        // No paraemter to store for this cmd

        /* 3. Send the req to the peer */
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
        ret = race_lpcomm_send_race_cmd_req_to_peer(NULL,
                                           0,
                                           RACE_LPCOMM_ROLE_AGENT,
                                           RACE_FOTA_DUAL_DEVICES_NEW_TRANSACTION,
                                           pCmdMsg->pktId.field.app_id,
                                           channel_id,
                                           process_id,
#ifdef RACE_AWS_ENABLE
                                           RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                           RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                           RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
        ret = RACE_ERRCODE_NOT_SUPPORT;
#endif
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            // 5B fail & free noti
            goto exit;
        }

        /* 5. Execute the race cmd */
        ret = fota_multi_info_sector_reset();

        /* 6. Update noti with race cmd execution result for the local device. */
        // Nothing to do for this cmd

        /* 7. Update noti status and try to send noti
                 *     (Noti will only be sent when both results are obtained.) */
        ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                           process_id,
                                           channel_id,
                                           FOTA_ERRCODE_SUCCESS == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL,
                                           RACE_LPCOMM_ROLE_AGENT,
                                           TRUE);
exit:
#endif /* RACE_LPCOMM_ENABLE */

        pEvt->status = ret;
        if (RACE_ERRCODE_SUCCESS != ret)
        {
#ifdef RACE_LPCOMM_ENABLE
            /* 8. Free noti if needed */
            /* process_id must be initialized to 0 when defining. */
            race_lpcomm_ps_noti_free(process_id);
#endif
        }
    }

    return pEvt;
}


void* RACE_CmdHandler_FOTA_start(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);

    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t recipient;
        uint8_t fota_mode; /* 0: background; 1: active; 2: adaptive */
    } PACKED CMD;

    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_FOTA_START,
                                      sizeof(RSP),
                                      channel_id);
    int32_t ret = RACE_ERRCODE_NOT_SUPPORT;
    race_recipient_type_enum recipient_type = race_recipient_type_convt(pCmd->recipient);
    race_fota_mode_enum fota_mode = RACE_FOTA_START_GET_FOTA_MODE(pCmd->fota_mode);
    race_fota_dual_device_dl_method_enum dl_method = RACE_FOTA_START_GET_DL_METHOD(pCmd->fota_mode);
    race_fota_download_state_enum fota_dl_state = race_fota_dl_state_get();
    race_fota_start_noti_struct *noti = NULL;
#ifdef RACE_LPCOMM_ENABLE
    /* process_id must be initialized to 0. */
    uint16_t process_id = 0;
    bool noti_sent = FALSE;
#endif
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

    if (pEvt != NULL)
    {
        /* Check the start cmd parameters. */
        ret = race_fota_start_check_params(recipient_type, fota_mode, dl_method);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            pEvt->status = ret;
            RACE_LOG_MSGID_W("Fail to start FOTA for parameter errors. ret:%d",1, ret);

#ifdef RACE_FOTA_ACTIVE_MODE_KEEP_HFP
            if (RACE_ERRCODE_REJECT_FOR_CALL_ONGOING == ret &&
                race_fota_is_race_fota_running() &&
                race_fota_is_active_mode())
            {
                ret = race_fota_cancel();
                RACE_LOG_MSGID_I("race_fota_cancel() ret:%d",1, ret);
            }
#endif
            return pEvt;
        }

        if (fota_cntx)
        {
            RACE_LOG_MSGID_I("fota_dl_state:%d channel_id:%d fota_cntx:%x",3, fota_dl_state, channel_id, fota_cntx);

#ifdef RACE_LPCOMM_SENDER_ROLE_ENABLE
            if (RACE_LPCOMM_ROLE_PARTNER == fota_cntx->fota_role)
            {
                pEvt->status = RACE_ERRCODE_NOT_ALLOWED;
                RACE_LOG_MSGID_W("FOTA has been started and its fota_role is partner.",0);
                return pEvt;
            }
#endif

            /* 5B/5D may be lost and SP will retry. This may cause RACE_FOTA_START be sent
                        * when FOTA is started.
                        * Even after RHO is done, the state is still RACE_FOTA_DOWNLOAD_STATE_RHOING.
                        */
            if (RACE_FOTA_DOWNLOAD_STATE_NONE == fota_dl_state ||
                RACE_FOTA_DOWNLOAD_STATE_MAX == fota_dl_state
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
                || RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT == fota_dl_state
#endif
                )
            {
                /* FOTA start for the first time or after RHO. */
                race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_STARTING);
                fota_cntx->sp_online = TRUE;
                fota_cntx->transfer_complete = FALSE;
                race_fota_set_sp_trans_method_by_channel_id(channel_id);
                race_fota_set_fota_mode(fota_mode);
                fota_cntx->dl_method = dl_method;
#ifdef RACE_LPCOMM_SENDER_ROLE_ENABLE
                fota_cntx->fota_role = RACE_LPCOMM_ROLE_AGENT;
#endif
            }

            if (RACE_RECIPIENT_TYPE_AGENT_ONLY == recipient_type)
            {
                uint8_t timer_id = race_fota_app_id_timer_id_get();

                ret = race_storage_enable_fota_partition_accessibility();

                /* Start app_id timer */
                if (RACE_ERRCODE_SUCCESS == ret)
                {
                    if (RACE_TIMER_INVALID_TIMER_ID == timer_id)
                    {
                        ret = race_timer_smart_start(&timer_id,
                                                     RACE_FOTA_APP_ID_TIMEOUT_IN_MS,
                                                     race_fota_app_id_timer_expiration_hdl,
                                                     NULL);

                        if (RACE_ERRCODE_SUCCESS == ret)
                        {
                            race_fota_app_id_timer_id_set(timer_id);
                        }
                    }

                    if (RACE_ERRCODE_SUCCESS == ret)
                    {
                        noti = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                                     RACE_TYPE_NOTIFICATION,
                                                     RACE_FOTA_START,
                                                     sizeof(race_fota_start_noti_struct),
                                                     channel_id);
                        if (noti)
                        {
                            noti->status = RACE_ERRCODE_SUCCESS;
                            noti->recipient = pCmd->recipient;
                            noti->fota_mode = pCmd->fota_mode;

                            /* Send the noti. */
                            ret = race_noti_send(noti, channel_id, TRUE);
                            if (RACE_ERRCODE_SUCCESS != ret)
                            {
                                /* Free the noti if needed. */
                                RACE_FreePacket(noti);
                                noti = NULL;
                            }
                        }
                        else
                        {
                            ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
                        }
                    }
                }

                if (RACE_FOTA_DOWNLOAD_STATE_STARTING == race_fota_dl_state_get())
                {
                    if (RACE_ERRCODE_SUCCESS == ret)
                    {
                        RACE_LOG_MSGID_I("Start FOTA for the first time.", 0);
                        fota_cntx->is_dual_fota = FALSE;
                        race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_START);
                        race_event_send_fota_start_event(fota_cntx->is_dual_fota, race_fota_is_active_mode());
                        //race_send_event_notify_msg(RACE_EVENT_TYPE_FOTA_START, NULL);
                    }
                    else
                    {
                        race_fota_stop_agent_reset();
                    }
                }
                /* else this is the retry cmd, do nothing. Let app_id timer handle the potential errors. */
            }
            else if (RACE_RECIPIENT_TYPE_AGENT_PARTNER == recipient_type)
            {
#ifndef RACE_LPCOMM_ENABLE
                ret = RACE_ERRCODE_NOT_SUPPORT;
#else
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
                race_lpcomm_fota_start_req_struct req = {0};
#endif

                fota_cntx->is_dual_fota = TRUE;

                /* 1. Create Noti */
                ret = race_lpcomm_ps_noti_create((void **)&noti,
                                                 &process_id,
                                                 RACE_FOTA_START,
                                                 pCmdMsg->pktId.field.app_id,
                                                 TRUE,
                                                 sizeof(race_fota_start_noti_struct),
                                                 channel_id);
                if (RACE_ERRCODE_SUCCESS != ret)
                {
                    // 5B fail
                    ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
                    goto exit;
                }

                /* 2. Store the const parameters in noti. */
                noti->recipient = pCmd->recipient;
                noti->fota_mode = pCmd->fota_mode;

                /* 3. Send the req to the peer */
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
                req.fota_mode = pCmd->fota_mode;
                ret = race_lpcomm_send_race_cmd_req_to_peer((uint8_t *)&req,
                                                            sizeof(race_lpcomm_fota_start_req_struct),
                                                            RACE_LPCOMM_ROLE_AGENT,
                                                            RACE_FOTA_START,
                                                            pCmdMsg->pktId.field.app_id,
                                                            channel_id,
                                                            process_id,
#ifdef RACE_AWS_ENABLE
                                                            RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                                            RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                                            RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
                ret = RACE_ERRCODE_NOT_SUPPORT;
#endif
                if (RACE_ERRCODE_SUCCESS != ret)
                {
                    // 5B fail & free noti
                    goto exit;
                }

                /* 5. Execute the race cmd */
                // Nothing to do here

                /* 6. Update noti with race cmd execution result for the local device. */
                // Nothing to do for this cmd

                /* 7. Update noti status and try to send noti
                                *     (Noti will only be sent when both results are obtained.) */
                ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                                   process_id,
                                                   channel_id,
                                                   RACE_ERRCODE_SUCCESS,
                                                   RACE_LPCOMM_ROLE_AGENT,
                                                   TRUE);

exit:
                if (RACE_ERRCODE_SUCCESS != ret)
                {
                    /* 8. Free noti if needed */
                    /* process_id must be initialized to 0 when defining. */
                    race_lpcomm_ps_noti_free(process_id);
                }

                if ((RACE_FOTA_DOWNLOAD_STATE_NONE == fota_dl_state ||
                     RACE_FOTA_DOWNLOAD_STATE_MAX == fota_dl_state
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
                     || RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT == fota_dl_state
#endif
                     ) &&
                    RACE_FOTA_DOWNLOAD_STATE_STARTING == race_fota_dl_state_get())
                {
                    if (RACE_ERRCODE_SUCCESS != ret)
                    {
                        /* Reset only if it is the first start not the retry. */
                        race_fota_stop_agent_reset();
                    }
                }
                /* else this is the retry cmd or the start after RHO, do nothing. Let app_id timer, ping
                                * mechanism and AWS-IF-retry handle the potential errors. */
#endif /* RACE_LPCOMM_ENABLE */
            }
        }
        else
        {
            ret = RACE_ERRCODE_NOT_INITIALIZED;
        }

        pEvt->status = ret;
    }

    return pEvt;
}


void* RACE_CmdHandler_FOTA_get_version(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t agent_or_partner;
    }PACKED CMD;

    typedef struct
    {
        uint8_t status;
    }PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_FOTA_GET_VERSION,
                                      sizeof(RSP),
                                      channel_id);
    if (pEvt)
    {
        if (!pCmd->agent_or_partner)
        {
            /* Agent */
            race_fota_get_version_noti_struct *noti = NULL;
            uint8_t version[FOTA_VERSION_MAX_SIZE] = {0};

            /* A1. Execute the cmd. */
            ret = fota_version_get(version, FOTA_VERSION_MAX_SIZE, FOTA_VERSION_TYPE_STORED);
            if (FOTA_ERRCODE_SUCCESS == ret)
            {
                /* A2. Create the noti. */
                noti = (void *)RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                                     RACE_TYPE_NOTIFICATION,
                                                     RACE_FOTA_GET_VERSION,
                                                     sizeof(race_fota_get_version_noti_struct) + strlen((const char *)version),
                                                     channel_id);

                if (noti)
                {
                    /* A3. Set the noti parameters with the cmd results.  */
                    noti->status = RACE_ERRCODE_SUCCESS;
                    noti->agent_or_partner = pCmd->agent_or_partner;
                    noti->version_len = strlen((const char *)version);

                    if (noti->version_len)
                    {
                        memcpy(noti->version, version, noti->version_len);
                    }

                    /* A4. Send the noti. */
                    ret = race_noti_send(noti, channel_id, TRUE);
                    if (RACE_ERRCODE_SUCCESS != ret)
                    {
                        /* A5. Free the noti if needed. */
                        RACE_FreePacket(noti);
                        noti = NULL;
                    }
                }
                else
                {
                    ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
                }
            }
        }
        else
        {
            /* Client */
#ifdef RACE_LPCOMM_ENABLE
            uint16_t process_id = race_gen_process_id();
            /* Send the req to the peer */
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            /* C1. Sent the req to the partner and the noti will be created within the Agent's req_hdl.
                        *       ps_noti is not used because there's no parameter needed to be stored for the final noti.
                        */
            ret = race_lpcomm_send_race_cmd_req_to_peer(NULL,
                                               0,
                                               RACE_LPCOMM_ROLE_AGENT,
                                               RACE_FOTA_GET_VERSION,
                                               pCmdMsg->pktId.field.app_id,
                                               channel_id,
                                               process_id,
#ifdef RACE_AWS_ENABLE
                                               RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                               RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                               RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif /* RACE_AWS_ENABLE */

#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif /* RACE_LPCOMM_ENABLE */
        }

        pEvt->status = ret;
    }

    return pEvt;
}


void* RACE_CmdHandler_FOTA_dual_devices_commit(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_FOTA_DUAL_DEVICES_COMMIT,
                                      sizeof(RSP),
                                      channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;

    if (pEvt)
    {
#ifdef RACE_LPCOMM_ENABLE
        uint8_t integrity_res = 0xFF;

        /* Add cmd into race_lpcomm_retry_no_process_status if it
                 * does not use ps list.
                 */
        ret = fota_dl_integrity_res_read(&integrity_res);
        if (FOTA_ERRCODE_SUCCESS == ret &&
            FOTA_DL_INTEGRITY_RES_VAL_PASS == integrity_res)
        {
            uint16_t process_id = race_gen_process_id();
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            race_lpcomm_fota_commit_req_struct req;
            bt_bd_addr_t *address = race_get_bt_connection_addr(channel_id);

            memset(&req, 0, sizeof(race_lpcomm_fota_commit_req_struct));
            if (address)
            {
                req.address_length = RACE_EVENT_REMOTE_DEVICE_ADDRESS_LENGTH > sizeof(bt_bd_addr_t) ? sizeof(bt_bd_addr_t) : RACE_EVENT_REMOTE_DEVICE_ADDRESS_LENGTH;
                memcpy(req.address, address, req.address_length);
            }

            /* Send the req to the peer */
            ret = race_lpcomm_send_race_cmd_req_to_peer((uint8_t *)&req,
                                                        sizeof(race_lpcomm_fota_commit_req_struct),
                                                        RACE_LPCOMM_ROLE_AGENT,
                                                        RACE_FOTA_DUAL_DEVICES_COMMIT,
                                                        pCmdMsg->pktId.field.app_id,
                                                        channel_id,
                                                        process_id,
#ifdef RACE_AWS_ENABLE
                                                        RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                                        RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                                        RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif
            if (RACE_ERRCODE_SUCCESS == ret)
            {
                race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_COMMITING);
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
                race_fota_set_sp_trans_method_by_channel_id(channel_id);
#endif
            }
        }
        else
        {
            ret = RACE_ERRCODE_FAIL;
        }
#else
        ret = RACE_ERRCODE_NOT_SUPPORT;
#endif /* RACE_LPCOMM_ENABLE */

        pEvt->status = ret;
    }

    return pEvt;
}


void* RACE_CmdHandler_FOTA_dual_device_query_state(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);

    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_FOTA_DUAL_DEVICES_QUERY_STATE,
                                      sizeof(RSP),
                                      channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;
#ifdef RACE_LPCOMM_ENABLE
    race_fota_dual_query_state_noti_struct *noti = NULL;
    /* process_id must be initialized to 0. */
    uint16_t process_id = 0;
    bool noti_sent = FALSE;
#endif

    if (pEvt != NULL)
    {
#ifndef RACE_LPCOMM_ENABLE
        ret = RACE_ERRCODE_NOT_SUPPORT;
#else
        uint16_t fota_state = 0xFFFF;
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
        race_lpcomm_fota_dual_device_query_state_req_struct req = {0};
#endif
        /* 1. Execute the race cmd */
        ret = fota_state_read(&fota_state);
        ret = FOTA_ERRCODE_SUCCESS == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;

        if (RACE_ERRCODE_SUCCESS != ret)
        {
            // 5B fail
            goto exit;
        }

        /* 2. Create Noti */
        ret = race_lpcomm_ps_noti_create((void **)&noti,
                                         &process_id,
                                         RACE_FOTA_DUAL_DEVICES_QUERY_STATE,
                                         pCmdMsg->pktId.field.app_id,
                                         TRUE,
                                         sizeof(race_fota_dual_query_state_noti_struct),
                                         channel_id);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            // 5B fail
            ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
            goto exit;
        }

        /* 3. Store the const parameters in noti. */
        // No paraemter to store for this cmd

        /* 4. Update noti with race cmd execution result for the local device. */
        noti->agent_fota_state = fota_state;

        /* 5. Send the req to the peer */
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
        req.agent_fota_state = fota_state;
        ret = race_lpcomm_send_race_cmd_req_to_peer((uint8_t *)&req,
                                                    sizeof(race_lpcomm_fota_dual_device_query_state_req_struct),
                                                    RACE_LPCOMM_ROLE_AGENT,
                                                    RACE_FOTA_DUAL_DEVICES_QUERY_STATE,
                                                    pCmdMsg->pktId.field.app_id,
                                                    channel_id,
                                                    process_id,
#ifdef RACE_AWS_ENABLE
                                                    RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                                    RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                                    RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
        ret = RACE_ERRCODE_NOT_SUPPORT;
#endif
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            // 5B fail & free noti
            goto exit;
        }

        /* 6. Update noti status and try to send noti
                 *     (Noti will only be sent when both results are obtained.) */
        ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                           process_id,
                                           channel_id,
                                           ret,
                                           RACE_LPCOMM_ROLE_AGENT,
                                           TRUE);
exit:
#endif /* RACE_LPCOMM_ENABLE */

        pEvt->status = ret;
        if (RACE_ERRCODE_SUCCESS != ret)
        {
#ifdef RACE_LPCOMM_ENABLE
            /* 7. Free noti if needed */
            /* process_id must be initialized to 0 when defining. */
            race_lpcomm_ps_noti_free(process_id);
#endif
        }
    }

    return pEvt;
}


void* RACE_CmdHandler_FOTA_new_transaction(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);

    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_FOTA_NEW_TRANSACTION,
                                      sizeof(RSP),
                                      channel_id);
    if (pEvt != NULL)
    {
        ret = fota_multi_info_sector_reset();
        pEvt->status = FOTA_ERRCODE_SUCCESS == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
    }

    return pEvt;
}


void* RACE_CmdHandler_FOTA_dual_device_write_state(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);

#ifdef RACE_LPCOMM_ENABLE
    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint16_t agent_fota_state;
        uint16_t client_fota_state;
    } PACKED CMD;
#endif

    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

#ifdef RACE_LPCOMM_ENABLE
    CMD *pCmd = (CMD *)pCmdMsg;
#endif
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_FOTA_DUAL_DEVICES_WRITE_STATE,
                                      sizeof(RSP),
                                      channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;
#ifdef RACE_LPCOMM_ENABLE
    race_fota_dual_write_state_noti_struct *noti = NULL;
    /* process_id must be initialized to 0. */
    uint16_t process_id = 0;
    bool noti_sent = FALSE;
#endif

    if (pEvt != NULL)
    {
#ifndef RACE_LPCOMM_ENABLE
        ret = RACE_ERRCODE_NOT_SUPPORT;
#else
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
        race_lpcomm_fota_write_state_req_struct req = {0};
#endif

        /* 1. Create Noti */
        ret = race_lpcomm_ps_noti_create((void **)&noti,
                                         &process_id,
                                         RACE_FOTA_DUAL_DEVICES_WRITE_STATE,
                                         pCmdMsg->pktId.field.app_id,
                                         TRUE,
                                         sizeof(race_fota_dual_write_state_noti_struct),
                                         channel_id);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            // 5B fail
            ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
            goto exit;
        }

        /* 2. Store the const parameters in noti. */
        noti->agent_fota_state = pCmd->agent_fota_state;
        noti->client_fota_state = pCmd->client_fota_state;

        /* 3. Send the req to the peer */
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
        req.fota_state = pCmd->client_fota_state;

        ret = race_lpcomm_send_race_cmd_req_to_peer((uint8_t *)&req,
                                           sizeof(race_lpcomm_fota_write_state_req_struct),
                                           RACE_LPCOMM_ROLE_AGENT,
                                           RACE_FOTA_DUAL_DEVICES_WRITE_STATE,
                                           pCmdMsg->pktId.field.app_id,
                                           channel_id,
                                           process_id,
#ifdef RACE_AWS_ENABLE
                                           RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                           RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                           RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
        ret = RACE_ERRCODE_NOT_SUPPORT;
#endif
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            // 5B fail & free noti
            goto exit;
        }

        /* 5. Execute the race cmd */
        ret = fota_state_write(pCmd->agent_fota_state);

        /* 6. Update noti with race cmd execution result for the local device. */
        // Nothing to do for this cmd

        /* 7. Update noti status and try to send noti
                 *     (Noti will only be sent when both results are obtained.) */
        ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                           process_id,
                                           channel_id,
                                           FOTA_ERRCODE_SUCCESS == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL,
                                           RACE_LPCOMM_ROLE_AGENT,
                                           TRUE);
exit:
#endif /* RACE_LPCOMM_ENABLE */

        pEvt->status = ret;
        if (RACE_ERRCODE_SUCCESS != ret)
        {
#ifdef RACE_LPCOMM_ENABLE
            /* 8. Free noti if needed */
            /* process_id must be initialized to 0 when defining. */
            race_lpcomm_ps_noti_free(process_id);
#endif
        }
    }

    return pEvt;
}


void* RACE_CmdHandler_FOTA_dual_devices_query_partition_info(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);

#ifdef RACE_LPCOMM_ENABLE
    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t partition_id;
    } PACKED CMD;
#endif

    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

#ifdef RACE_LPCOMM_ENABLE
    CMD *pCmd = (CMD *)pCmdMsg;
#endif
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_FOTA_DUAL_DEVICES_QUERY_PARTITION_INFO,
                                      sizeof(RSP),
                                      channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;
#ifdef RACE_LPCOMM_ENABLE
    race_fota_dual_query_partition_info_noti_struct *noti = NULL;
    /* process_id must be initialized to 0. */
    uint16_t process_id = 0;
    bool noti_sent = FALSE;
#endif

    if (pEvt != NULL)
    {
#ifndef RACE_LPCOMM_ENABLE
        ret = RACE_ERRCODE_NOT_SUPPORT;
#else
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
        race_lpcomm_fota_query_partition_info_req_struct req = {0};
#endif

        /* 1. Create Noti */
        ret = race_lpcomm_ps_noti_create((void **)&noti,
                                         &process_id,
                                         RACE_FOTA_DUAL_DEVICES_QUERY_PARTITION_INFO,
                                         pCmdMsg->pktId.field.app_id,
                                         TRUE,
                                         sizeof(race_fota_dual_query_partition_info_noti_struct),
                                         channel_id);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            // 5B fail
            ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
            goto exit;
        }

        /* 2. Store the const parameters in noti. */
        noti->partition_id = pCmd->partition_id;

        /* 3. Send the req to the peer */
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
        req.partition_id = pCmd->partition_id;

        ret = race_lpcomm_send_race_cmd_req_to_peer((uint8_t *)&req,
                                           sizeof(race_lpcomm_fota_query_partition_info_req_struct),
                                           RACE_LPCOMM_ROLE_AGENT,
                                           RACE_FOTA_DUAL_DEVICES_QUERY_PARTITION_INFO,
                                           pCmdMsg->pktId.field.app_id,
                                           channel_id,
                                           process_id,
#ifdef RACE_AWS_ENABLE
                                           RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                           RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                           RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
        ret = RACE_ERRCODE_NOT_SUPPORT;
#endif
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            // 5B fail & free noti
            goto exit;
        }

        /* 5. Execute the race cmd */
        ret = race_query_partition_info(&(noti->agent_storage_type),
                                        &(noti->agent_partition_addr),
                                        &(noti->agent_partition_len),
                                        pCmd->partition_id);

        /* 6. Update noti with race cmd execution result for the local device. */
        // Has been upated above

        /* 7. Update noti status and try to send noti
                 *     (Noti will only be sent when both results are obtained.) */
        ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                           process_id,
                                           channel_id,
                                           ret,
                                           RACE_LPCOMM_ROLE_AGENT,
                                           TRUE);
exit:
#endif /* RACE_LPCOMM_ENABLE */

        pEvt->status = ret;
        if (RACE_ERRCODE_SUCCESS != ret)
        {
#ifdef RACE_LPCOMM_ENABLE
            /* 8. Free noti if needed */
            /* process_id must be initialized to 0 when defining. */
            race_lpcomm_ps_noti_free(process_id);
#endif
        }
    }

    return pEvt;
}


/* Phase-out cmd */
void* RACE_CmdHandler_FOTA_active_fota_preparation(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t agent_or_partner;
    }PACKED CMD;

    typedef struct
    {
        uint8_t status;
    }PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_FOTA_ACTIVE_FOTA_PREPARATION,
                                      sizeof(RSP),
                                      channel_id);
    configASSERT(pCmd);
    if (pEvt)
    {
        if (!pCmd->agent_or_partner)
        {
            /* Agent */
            bt_bd_addr_t *sp_addr = race_bt_get_sp_bd_addr();

            if (!sp_addr &&
                RACE_SERIAL_PORT_TYPE_BLE == race_get_port_type_by_channel_id(channel_id))
            {
                /* BLE FOTA does not support BLE only. */
                ret = RACE_ERRCODE_NOT_ALLOWED;
            }
            else
            {
#ifdef RACE_BT_CMD_ENABLE
                race_fota_active_fota_preparation_noti_struct *noti = NULL;

                /* A1. Execute the cmd. */
                ret = race_fota_active_bt_preparation();
                if (RACE_ERRCODE_SUCCESS == ret)
                {
                    RACE_LOG_MSGID_I("Create Noti", 0);
                    /* Send Noti with status of FOTA_ERRCODE_SUCCESS. */
                    /* A2. Create the noti. */
                    noti = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                                 RACE_TYPE_NOTIFICATION,
                                                 RACE_FOTA_ACTIVE_FOTA_PREPARATION,
                                                 sizeof(race_fota_active_fota_preparation_noti_struct),
                                                 channel_id);
                    if (noti)
                    {
                        /* A3. Set the noti parameters with the cmd results.  */
                        noti->status = RACE_ERRCODE_SUCCESS;
                        noti->agent_or_partner = pCmd->agent_or_partner;

                        /* A4. Send the noti. */
                        ret = race_noti_send(noti, channel_id, TRUE);
                        if (RACE_ERRCODE_SUCCESS != ret)
                        {
                            /* A5. Free the noti if needed. */
                            RACE_FreePacket(noti);
                            noti = NULL;
                        }
                    }
                    else
                    {
                        ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
                    }
                }
#else
                ret = RACE_ERRCODE_SUCCESS;
#endif
            }

            if (RACE_ERRCODE_SUCCESS == ret)
            {
                race_fota_set_fota_mode(RACE_FOTA_MODE_ACTIVE);
            }
        }
        else
        {
            /* Client */
#ifdef RACE_LPCOMM_ENABLE
            uint16_t process_id = race_gen_process_id();
            /* Send the req to the peer */
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            /* C1. Sent the req to the partner and the noti will be created within the Agent's req_hdl.
                        *       ps_noti is not used because there's no parameter needed to be stored for the final noti.
                        */
            ret = race_lpcomm_send_race_cmd_req_to_peer(NULL,
                                               0,
                                               RACE_LPCOMM_ROLE_AGENT,
                                               RACE_FOTA_ACTIVE_FOTA_PREPARATION,
                                               pCmdMsg->pktId.field.app_id,
                                               channel_id,
                                               process_id,
#ifdef RACE_AWS_ENABLE
                                               RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                               RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                               RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif /* RACE_AWS_ENABLE */

#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif /* RACE_LPCOMM_ENABLE */
        }

        pEvt->status = ret;
    }

    return pEvt;
}


void* RACE_CmdHandler_FOTA_ping(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);

    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t recipient_count;
        race_recipient_param_general_struct recipient_param[0];
    } PACKED CMD;

    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD * pCmd = (CMD *)pCmdMsg;

    RSP * pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                       RACE_TYPE_RESPONSE,
                                       RACE_FOTA_PING,
                                       sizeof(RSP),
                                       channel_id);
    race_recipient_param_general_struct *agent_recipient_param = NULL;
    race_recipient_param_general_struct *partner_recipient_param = NULL;
    race_fota_ping_noti_struct *noti = NULL;

    if (pEvt != NULL)
    {
        ret = race_recipient_param_parse(pCmd->recipient_count,
                                         (uint8_t *)pCmd->recipient_param,
                                         sizeof(race_recipient_param_general_struct),
                                         (uint8_t **)&agent_recipient_param,
                                         (uint8_t **)&partner_recipient_param);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            goto exit;
        }

        if (agent_recipient_param && !partner_recipient_param)
        {
            /* Agent only */
            /* A2. Create the noti(to Smart Phone). */
            noti = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                         RACE_TYPE_NOTIFICATION,
                                         RACE_FOTA_PING,
                                         sizeof(race_fota_ping_noti_struct) + sizeof(race_recipient_param_general_struct),
                                         channel_id);
            if (noti)
            {
                /* A3. Set the noti parameters with the cmd results.  */
                noti->recipient_count = pCmd->recipient_count;
                memcpy(noti->recipient_param, pCmd->recipient_param, sizeof(race_recipient_param_general_struct));
                noti->status = RACE_ERRCODE_SUCCESS;

                /* A4. Send the noti. */
                ret = race_noti_send(noti, channel_id, TRUE);
                if (RACE_ERRCODE_SUCCESS != ret)
                {
                    /* A5. Free the noti if needed. */
                    RACE_FreePacket(noti);
                    noti = NULL;
                }
            }
            else
            {
                ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
            }
        }
        else if (!agent_recipient_param && partner_recipient_param)
        {
            /* Partner only */
#ifndef RACE_LPCOMM_ENABLE
            ret = RACE_ERRCODE_NOT_SUPPORT;
#else
            uint16_t process_id = race_gen_process_id();

            /* 5. Send the req to the peer */
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            ret = race_lpcomm_send_race_cmd_req_to_peer(NULL,
                                                        0,
                                                        RACE_LPCOMM_ROLE_AGENT,
                                                        RACE_FOTA_PING,
                                                        pCmdMsg->pktId.field.app_id,
                                                        channel_id,
                                                        process_id,
#ifdef RACE_AWS_ENABLE
                                                        RACE_LPCOMM_TRANS_METHOD_AWS,
#else
                                                        RACE_LPCOMM_TRANS_METHOD_COSYS,
#endif
                                                        RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif
#endif /* RACE_LPCOMM_ENABLE */
        }
        else
        {
            ret = RACE_ERRCODE_NOT_SUPPORT;
        }
     }

exit:
    if (pEvt)
    {
        pEvt->status = ret;
    }

    return pEvt;
}


void* RACE_CmdHandler_FOTA_query_transmit_interval(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);

    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t recipient_count;
        race_recipient_param_general_struct recipient_param[0];
    } PACKED CMD;

    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD * pCmd = (CMD *)pCmdMsg;

    RSP * pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                       RACE_TYPE_RESPONSE,
                                       RACE_FOTA_QUERY_TRANSMIT_INTERVAL,
                                       sizeof(RSP),
                                       channel_id);
    race_recipient_param_general_struct *agent_recipient_param = NULL;
    race_recipient_param_general_struct *partner_recipient_param = NULL;
    race_fota_query_transmit_interval_noti_struct *noti = NULL;

    if (pEvt != NULL)
    {
        ret = race_recipient_param_parse(pCmd->recipient_count,
                                         (uint8_t *)pCmd->recipient_param,
                                         sizeof(race_recipient_param_general_struct),
                                         (uint8_t **)&agent_recipient_param,
                                         (uint8_t **)&partner_recipient_param);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            goto exit;
        }

        if (agent_recipient_param && !partner_recipient_param)
        {
            /* Agent only */
            race_fota_sp_trans_method_enum sp_trans_method = race_fota_sp_trans_method_get();
            uint16_t spp_transmit_interval = 0, ble_transmit_interval = 0;

            ret = race_fota_get_transmit_interval(&spp_transmit_interval, &ble_transmit_interval);
            if (RACE_ERRCODE_SUCCESS == ret)
            {
                /* A2. Create the noti(to Smart Phone). */
                noti = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                             RACE_TYPE_NOTIFICATION,
                                             RACE_FOTA_QUERY_TRANSMIT_INTERVAL,
                                             sizeof(race_fota_query_transmit_interval_noti_struct) + sizeof(race_fota_query_transmit_interval_recipient_param_struct),
                                             channel_id);
                if (noti)
                {
                    noti->status = RACE_ERRCODE_SUCCESS;
                    noti->recipient_count = pCmd->recipient_count;
                    noti->recipient_param[0].recipient = pCmd->recipient_param[0].recipient;

                    /* A3. Set the noti parameters with the cmd results.  */
                    if (RACE_FOTA_SP_TRANS_METHOD_SPP == sp_trans_method)
                    {
                        noti->recipient_param[0].transmit_interval = spp_transmit_interval;
                    }
                    else if (RACE_FOTA_SP_TRANS_METHOD_BLE == sp_trans_method)
                    {
                        noti->recipient_param[0].transmit_interval = ble_transmit_interval;
                    }
                    else
                    {
                        noti->status = RACE_ERRCODE_NOT_SUPPORT;
                    }
                }
                else
                {
                    ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
                    goto exit;
                }

                /* A4. Send the noti. */
                ret = race_noti_send(noti, channel_id, TRUE);
                if (RACE_ERRCODE_SUCCESS != ret)
                {
                    /* A5. Free the noti if needed. */
                    RACE_FreePacket(noti);
                    noti = NULL;
                }
            }
        }
        else
        {
            ret = RACE_ERRCODE_NOT_SUPPORT;
        }
     }

exit:
    if (pEvt)
    {
        pEvt->status = ret;
    }

    return pEvt;
}


void* RACE_EvtHandler_FOTA_stop_rsp_noti_hdl(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);

    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t status;
    } PACKED RSP_NOTI;

    RSP_NOTI* rsp_noti = (RSP_NOTI*)pCmdMsg;
    race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

    if (!pCmdMsg || !fota_cntx)
    {
        return NULL;
    }

    if (RACE_TYPE_RESPONSE == pCmdMsg->type)
    {
        race_fota_stop_agent_sp_stop_rsp_struct sp_stop_rsp = {0};

        sp_stop_rsp.recipient_type = fota_cntx->is_dual_fota ? RACE_RECIPIENT_TYPE_AGENT_PARTNER : RACE_RECIPIENT_TYPE_AGENT_ONLY;
        sp_stop_rsp.status = rsp_noti->status;
        race_fota_stop_agent_sp_stop_rsp_process(&sp_stop_rsp);
    }
    else if (RACE_TYPE_NOTIFICATION == pCmdMsg->type)
    {
        race_fota_stop_agent_sp_stop_noti_struct sp_stop_noti = {0};

        sp_stop_noti.recipient_type = fota_cntx->is_dual_fota ? RACE_RECIPIENT_TYPE_AGENT_PARTNER : RACE_RECIPIENT_TYPE_AGENT_ONLY;
        sp_stop_noti.status = rsp_noti->status;
        race_fota_stop_agent_sp_stop_noti_process(&sp_stop_noti);
    }

    return NULL;
}


void* RACE_CmdHandler_FOTA(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);


    if (pCmdMsg->hdr.type == RACE_TYPE_COMMAND ||
        pCmdMsg->hdr.type == RACE_TYPE_COMMAND_WITHOUT_RSP)
    {
        switch (pCmdMsg->hdr.id)
        {
            case RACE_FOTA_QUERY_PARTITION_INFO:
                return RACE_CmdHandler_FOTA_query_partition_info((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_CHECK_INTEGRITY:
                return RACE_CmdHandler_FOTA_check_integrity((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_COMMIT:
                return RACE_CmdHandler_FOTA_commit((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_STOP:
                return RACE_CmdHandler_FOTA_stop((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_QUERY_STATE:
                return RACE_CmdHandler_FOTA_query_state((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_WRITE_STATE:
                return RACE_CmdHandler_FOTA_write_state((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_GET_VERSION:
                return RACE_CmdHandler_FOTA_get_version((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_NEW_TRANSACTION:
                return RACE_CmdHandler_FOTA_new_transaction((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_DUAL_DEVICES_NEW_TRANSACTION:
                return RACE_CmdHandler_FOTA_dual_devices_new_transaction((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_START:
                return RACE_CmdHandler_FOTA_start((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_DUAL_DEVICES_QUERY_STATE:
                return RACE_CmdHandler_FOTA_dual_device_query_state((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_DUAL_DEVICES_WRITE_STATE:
                return RACE_CmdHandler_FOTA_dual_device_write_state((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_DUAL_DEVICES_QUERY_PARTITION_INFO:
                return RACE_CmdHandler_FOTA_dual_devices_query_partition_info((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_DUAL_DEVICES_COMMIT:
                return RACE_CmdHandler_FOTA_dual_devices_commit((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_ACTIVE_FOTA_PREPARATION:
                return RACE_CmdHandler_FOTA_active_fota_preparation((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_PING:
                return RACE_CmdHandler_FOTA_ping((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            case RACE_FOTA_QUERY_TRANSMIT_INTERVAL:
                return RACE_CmdHandler_FOTA_query_transmit_interval((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            default:
                break;
        }
    }
    else
    {
        switch (pCmdMsg->hdr.id)
        {
            case RACE_FOTA_STOP:
                return RACE_EvtHandler_FOTA_stop_rsp_noti_hdl((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            default:
                break;
        }
    }

    return NULL;
}

#endif /* RACE_FOTA_CMD_ENABLE */

