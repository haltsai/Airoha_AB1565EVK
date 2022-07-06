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
#ifdef RACE_STORAGE_CMD_ENABLE
#include "race_lpcomm_trans.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_msg_struct.h"
#include "race_cmd_storage.h"
#include "race_lpcomm_conn.h"
#include "race_storage_access.h"
#include "race_storage_util.h"
#include "race_noti.h"
#include "race_lpcomm_ps_noti.h"
#include "memory_map.h"


////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void *race_cmdhdl_storage_write_byte(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t storage_type;
        uint16_t data_length;
        uint32_t storage_address;
        uint8_t data[1];
    } PACKED CMD;

    typedef struct {
        uint8_t status;
        uint8_t storage_type;
        uint16_t data_lenght;
        uint32_t storage_address;
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_STORAGE_WRITE_BYTE,
                                      sizeof(RSP),
                                      channel_id);
    if (pEvt) {
        ret = race_storage_is_addr_accessible(pCmd->storage_type, pCmd->storage_address, pCmd->data_length);
        if (RACE_ERRCODE_SUCCESS == ret) {
            ret = race_storage_write(pCmd->storage_address,
                                     pCmd->data,
                                     pCmd->data_length,
                                     pCmd->storage_type);
        }

        pEvt->status = ret;
    }

    return pEvt;
}

void *race_cmdhdl_storage_read_page(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t storage_type;
        uint8_t number_of_page;
        uint32_t storage_address;
    } PACKED CMD;

    typedef struct {
        uint8_t status;
        uint8_t storage_type;
        uint8_t number_of_the_rest_page;
        uint8_t crc;
        uint32_t storage_address;
        uint8_t data[256];
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_STORAGE_READ_PAGE,
                                      sizeof(RSP),
                                      channel_id);

    if (pEvt) {
        pEvt->storage_type = pCmd->storage_type;
        pEvt->number_of_the_rest_page = 0;
        pEvt->storage_address = pCmd->storage_address;

        ret = race_storage_read(pCmd->storage_address,
                                pEvt->data,
                                256,
                                pCmd->storage_type);

        pEvt->status = ret;
    }

    return pEvt;
}

#if 0
void *race_cmdhdl_storage_write_page(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{

    typedef struct {
        uint8_t crc;
        uint32_t storage_addr;
        uint8_t data[256];
    } PACKED race_storage_page_info_struct;

    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t storage_type;
        uint8_t page_count;
        race_storage_page_info_struct page_info[0]; /* array size: page_count */
    } PACKED CMD;

    typedef struct {
        uint8_t status;
        uint8_t storage_type;
        uint8_t completed_page_count;
        uint32_t storage_address[0];    /* array size: completed_page_count */
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = NULL;

    if (pCmd) {
        int i = 0, completed_page_count = 0;
        uint32_t storage_address[4] = {0};
        uint32_t *storage_address_ptr = storage_address;
        bool free_storage_address = FALSE;

        if (pCmd->page_count > 4) {
            storage_address_ptr = race_mem_alloc(pCmd->page_count * sizeof(uint32_t));
            if (storage_address_ptr) {
                memset(storage_address_ptr, 0, pCmd->page_count * sizeof(uint32_t));
                free_storage_address = TRUE;
            } else {
                storage_address_ptr = storage_address;
            }
        }

        if (storage_address_ptr) {
            for (i = 0; i < pCmd->page_count; i++) {
                ret = race_storage_is_addr_accessible(pCmd->storage_type,
                                                      pCmd->page_info[i].storage_addr,
                                                      256);
                if (RACE_ERRCODE_SUCCESS == ret) {
                    ret = race_storage_write(pCmd->page_info[i].storage_addr,
                                             pCmd->page_info[i].data,
                                             256,
                                             pCmd->storage_type);

                    if (RACE_ERRCODE_SUCCESS == ret) {
                        uint8_t crc8 = 0;
                        ret = race_storage_crc8_generate(&crc8,
                                                         pCmd->page_info[i].storage_addr,
                                                         256,
                                                         pCmd->storage_type);
                        if (RACE_ERRCODE_SUCCESS == ret && crc8 == pCmd->page_info[i].crc) {
                            storage_address_ptr[completed_page_count] = pCmd->page_info[i].storage_addr;
                            completed_page_count++;
                        } else {
                            ret = RACE_ERRCODE_CONFLICT;
                            RACE_LOG_MSGID_I("crc8 not match! i:%d crc_trans:%x crc_calc:%x", 3, i, pCmd->page_info[i].crc, crc8);
                        }
                    }
                }
            }
        }

        pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                     RACE_TYPE_RESPONSE,
                                     RACE_STORAGE_WRITE_PAGE,
                                     sizeof(RSP) + completed_page_count * sizeof(uint32_t),
                                     channel_id);
        if (pEvt) {
            pEvt->storage_type = pCmd->storage_type;
            pEvt->completed_page_count = completed_page_count;

            if (!storage_address_ptr || 0 == completed_page_count) {
                pEvt->status = RACE_ERRCODE_FAIL;
            } else {
                memcpy(pEvt->storage_address, storage_address_ptr, completed_page_count * sizeof(uint32_t));
                pEvt->status = RACE_ERRCODE_SUCCESS;
            }
        }

        if (free_storage_address && storage_address_ptr) {
            race_mem_free(storage_address_ptr);
        }
    }


    return pEvt;
}
#else

bool race_storage_data_check(PTR_RACE_COMMON_HDR_STRU pCmdMsg)
{
    typedef struct {
        uint8_t crc;
        uint32_t storage_addr;
        uint8_t data[256];
    } PACKED race_storage_page_info_struct;

    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t storage_type;
        uint8_t page_count;
        race_storage_page_info_struct page_info[0]; /* array size: page_count */
    } PACKED CMD;

    bool crc_pass = true;
    CMD *pCmd = (CMD *)pCmdMsg;
    uint8_t crc8 = 0;
    int32_t ret = RACE_ERRCODE_FAIL;
    int i;

    for (i = 0; i < pCmd->page_count; i++) {
        ret = race_storage_crc8_generate(&crc8,
                    (uint32_t)pCmd->page_info[i].data,
                    256,
                    pCmd->storage_type);

        if ((RACE_ERRCODE_SUCCESS != ret) ||
            (crc8 != pCmd->page_info[i].crc)
        ) {
            RACE_LOG_MSGID_I("race_storage_data_check: crc8( addr: 0x%08X ) not match! i:%d crc_trans:%x crc_calc:%x", 4,
                pCmd->page_info[i].storage_addr, i,
                pCmd->page_info[i].crc, crc8);
            crc_pass = false;
        }
    }
    return crc_pass;
}

void *race_cmdhdl_storage_write_page(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{

    typedef struct {
        uint8_t crc;
        uint32_t storage_addr;
        uint8_t data[256];
    } PACKED race_storage_page_info_struct;

    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t storage_type;
        uint8_t page_count;
        race_storage_page_info_struct page_info[0]; /* array size: page_count */
    } PACKED CMD;

    typedef struct {
        uint8_t status;
        uint8_t storage_type;
        uint8_t completed_page_count;
        uint32_t storage_address[0];    /* array size: completed_page_count */
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL, ret_val = RACE_ERRCODE_SUCCESS;
    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = NULL;
    uint32_t i = 0, idx = 0;
    uint8_t crc8 = 0;
    uint32_t storage_address[4] = {0};

    if (pCmd) {
        pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                     RACE_TYPE_RESPONSE,
                                     RACE_STORAGE_WRITE_PAGE,
                                     sizeof(RSP) + pCmd->page_count * sizeof(uint32_t),
                                     channel_id);
        if (!pEvt) {
            return NULL;
        }

        pEvt->completed_page_count = 0;

        for (i = 0; i < pCmd->page_count; i++) {
            storage_address[i] = pCmd->page_info[i].storage_addr;
            ret = race_storage_crc8_generate(&crc8,
                                             (uint32_t)pCmd->page_info[i].data,
                                             256,
                                             pCmd->storage_type);

            if (RACE_ERRCODE_SUCCESS != ret || crc8 != pCmd->page_info[i].crc) {
                ret_val = RACE_ERRCODE_CONFLICT;
                RACE_LOG_MSGID_I("crc8( addr: 0x%08X ) not match! i:%d crc_trans:%x crc_calc:%x", 4,
                    pCmd->page_info[i].storage_addr, i,
                    pCmd->page_info[i].crc, crc8);
            }
        }

        memcpy(pEvt->storage_address, &storage_address, pCmd->page_count * sizeof(uint32_t));
        if(ret_val == RACE_ERRCODE_CONFLICT){
            for(i = 0; i < pCmd->page_count; i++){
                RACE_LOG_MSGID_E("RACE_STORAGE_WRITE_PAGE Para[%d]: CRC8: 0x%02X, ADDR: 0x%08X", 3,
                    i, pCmd->page_info[i].crc, pCmd->page_info[i].storage_addr);
                for(idx = 0; idx < 256; idx += 16){
                    RACE_LOG_MSGID_E("RACE_STORAGE_WRITE_PAGE Data[%d]: 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X", 17,
                    i,
                    pCmd->page_info[i].data[idx], pCmd->page_info[i].data[idx+1], pCmd->page_info[i].data[idx+2], pCmd->page_info[i].data[idx+3],
                    pCmd->page_info[i].data[idx+4], pCmd->page_info[i].data[idx+5], pCmd->page_info[i].data[idx+6], pCmd->page_info[i].data[idx+7],
                    pCmd->page_info[i].data[idx+8], pCmd->page_info[i].data[idx+9], pCmd->page_info[i].data[idx+10], pCmd->page_info[i].data[idx+11],
                    pCmd->page_info[i].data[idx+12], pCmd->page_info[i].data[idx+13], pCmd->page_info[i].data[idx+14], pCmd->page_info[i].data[idx+15]
                    );
                }
            }
        }

        if (RACE_ERRCODE_SUCCESS == ret_val) {
            for (i = 0; i < pCmd->page_count; i++) {
                ret = race_storage_is_addr_accessible(pCmd->storage_type,
                                                      pCmd->page_info[i].storage_addr,
                                                      256);
                if (RACE_ERRCODE_SUCCESS == ret) {
                    ret = race_storage_write(pCmd->page_info[i].storage_addr,
                                             pCmd->page_info[i].data,
                                             256,
                                             pCmd->storage_type);

                    if (RACE_ERRCODE_SUCCESS == ret) {
                        pEvt->completed_page_count++;
                    } else {
                        ret_val = RACE_ERRCODE_FAIL;
                        break;
                    }
                }
            }
        }

        pEvt->storage_type = pCmd->storage_type;
        if (0 == pCmd->page_count) {
            ret_val = RACE_ERRCODE_PARAMETER_ERROR;
        }
        pEvt->status = ret_val;
    }

    return pEvt;
}
#endif


/* Send the Noti */
RACE_ERRCODE race_storage_erase_partition_cmd_cb(uint8_t status,
        uint8_t storage_type,
        uint32_t partition_length,
        uint32_t partition_address,
        uint8_t app_id,
        uint8_t channel_id,
        bool noti_delay,
        void *user_data)
{
    race_storage_erase_partition_noti_struct *noti = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;

    noti = RACE_ClaimPacketAppID(app_id,
                                 RACE_TYPE_NOTIFICATION,
                                 RACE_STORAGE_ERASE_PARTITION,
                                 sizeof(race_storage_erase_partition_noti_struct),
                                 channel_id);

    if (noti) {
        noti->status = status;
        noti->storage_type = storage_type;
        noti->partition_length = partition_length;
        noti->partition_address = partition_address;

        ret = race_noti_send((void *)noti,
                             channel_id,
                             noti_delay);
        if (RACE_ERRCODE_SUCCESS != ret) {
            RACE_LOG_MSGID_W("Lost a RACE_STORAGE_ERASE_PARTITION noti, ret:%d", 1, ret);
            RACE_FreePacket((void *)noti);
        }
    } else {
        ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
        RACE_LOG_MSGID_W("Lost a RACE_STORAGE_ERASE_PARTITION noti, ret:%d", 1, ret);
    }

    return ret;
}


void *race_cmdhdl_storage_erase_partition(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t storage_type;
        uint32_t partition_length;
        uint32_t partition_address;
    } PACKED CMD;

    typedef struct {
        uint8_t status;
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_STORAGE_ERASE_PARTITION,
                                      sizeof(RSP),
                                      channel_id);
    if (pEvt) {
        ret = race_storage_is_addr_accessible(pCmd->storage_type,
                                              pCmd->partition_address,
                                              pCmd->partition_length);
        if (RACE_ERRCODE_SUCCESS == ret) {
            /* Execute the cmd */
            ret = race_storage_erase_partition(pCmd->storage_type,
                                               pCmd->partition_length,
                                               pCmd->partition_address,
                                               pCmdMsg->pktId.field.app_id,
                                               channel_id,
                                               FALSE,
                                               race_storage_erase_partition_cmd_cb,
                                               NULL);
            if (RACE_ERRCODE_SUCCESS == ret) {
                ret = race_storage_erase_partition_cmd_cb(ret,
                        pCmd->storage_type,
                        pCmd->partition_length,
                        pCmd->partition_address,
                        pCmdMsg->pktId.field.app_id,
                        channel_id,
                        TRUE,
                        NULL);
            } else if (RACE_ERRCODE_MORE_OPERATION == ret) {
                ret = RACE_ERRCODE_SUCCESS;
            }
        }

        pEvt->status = ret;
    }

    return pEvt;
}


#ifdef RACE_LPCOMM_ENABLE
RACE_ERRCODE race_storage_dual_erase_partition_cmd_cb(uint8_t status,
        uint8_t storage_type,
        uint32_t partition_length,
        uint32_t partition_address,
        uint8_t app_id,
        uint8_t channel_id,
        bool noti_delay,
        void *user_data)
{
    race_storage_dual_device_erase_partition_noti_struct *noti = NULL;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    uint16_t process_id = 0;
    bool noti_sent = FALSE;

    if (!user_data) {
        RACE_LOG_MSGID_E("user_data used as process_id is not valid!", 0);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    process_id = (uint16_t)((uint32_t)user_data);
    noti = race_lpcomm_ps_noti_find(process_id);
    if (noti) {
        ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                           process_id,
                                           channel_id,
                                           status,
                                           RACE_LPCOMM_ROLE_AGENT,
                                           noti_delay);

        if (RACE_ERRCODE_SUCCESS != ret) {
            race_lpcomm_ps_noti_free(process_id);
        }
    }

    return ret;
}
#endif


void *race_cmdhdl_storage_dual_devices_erase_partition(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
#ifdef RACE_LPCOMM_ENABLE
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t  agent_storage_type;
        uint32_t agent_length;
        uint32_t agent_storage_address;
        uint8_t  partner_storage_type;
        uint32_t partner_length;
        uint32_t partner_storage_address;
    } PACKED CMD;
#endif

    typedef struct {
        uint8_t status;
    } PACKED RSP;

    uint8_t ret = RACE_ERRCODE_FAIL;
#ifdef RACE_LPCOMM_ENABLE
    CMD *pCmd = (CMD *)pCmdMsg;
#endif
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_STORAGE_DUAL_DEVICES_ERASE_PARTITION,
                                      sizeof(RSP),
                                      channel_id);
#ifdef RACE_LPCOMM_ENABLE
    race_storage_dual_device_erase_partition_noti_struct *noti = NULL;
    /* process_id must be initialized to 0. */
    uint16_t process_id = 0;
    bool noti_sent = FALSE;
#endif

    if (pEvt != NULL) {
#ifndef RACE_LPCOMM_ENABLE
        ret = RACE_ERRCODE_NOT_SUPPORT;
#else
        ret = race_storage_is_addr_accessible(pCmd->agent_storage_type,
                                              pCmd->agent_storage_address,
                                              pCmd->agent_length);

        if (RACE_ERRCODE_SUCCESS == ret) {
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
        race_lpcomm_storage_dual_device_erase_partition_req_struct req = {0};
#endif

        /* 1. Create Noti */
        ret = race_lpcomm_ps_noti_create((void **)&noti,
                                         &process_id,
                                         RACE_STORAGE_DUAL_DEVICES_ERASE_PARTITION,
                                         pCmdMsg->pktId.field.app_id,
                                         TRUE,
                                         sizeof(race_storage_dual_device_erase_partition_noti_struct),
                                         channel_id);
        if (RACE_ERRCODE_SUCCESS != ret) {
            // 5B fail
            ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
            goto exit;
        }

        /* 2. Store the const parameters in noti. */
        noti->agent_storage_type = pCmd->agent_storage_type;
        noti->agent_length = pCmd->agent_length;
        noti->agent_storage_address = pCmd->agent_storage_address;
        noti->partner_storage_type = pCmd->partner_storage_type;
        noti->partner_length = pCmd->partner_length;
        noti->partner_storage_address = pCmd->partner_storage_address;

        /* 3. Send the req to the peer */
        if (pCmd->partner_length) {
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            req.storage_type = pCmd->partner_storage_type;
            req.partition_address = pCmd->partner_storage_address;
            req.partition_length = pCmd->partner_length;

            ret = race_lpcomm_send_race_cmd_req_to_peer((uint8_t *)&req,
                    sizeof(race_lpcomm_storage_dual_device_erase_partition_req_struct),
                    RACE_LPCOMM_ROLE_AGENT,
                    RACE_STORAGE_DUAL_DEVICES_ERASE_PARTITION,
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
        } else {
            ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                               process_id,
                                               channel_id,
                                               RACE_ERRCODE_SUCCESS,
                                               RACE_LPCOMM_ROLE_PARTNER,
                                               TRUE);
        }

        if (RACE_ERRCODE_SUCCESS != ret) {
            // 5B fail & free noti
            goto exit;
        }

        /* 5. Execute the race cmd */
        ret = race_storage_erase_partition(pCmd->agent_storage_type,
                                           pCmd->agent_length,
                                           pCmd->agent_storage_address,
                                           pCmdMsg->pktId.field.app_id,
                                           channel_id,
                                           FALSE,
                                           race_storage_dual_erase_partition_cmd_cb,
                                           (void *)((uint32_t)process_id));
        if (RACE_ERRCODE_SUCCESS == ret) {
            /* 6. Update noti with race cmd execution result for the local device. */
            /* 7. Update noti status and try to send noti
                 *	  (Noti will only be sent when both results are obtained.) */
            ret = race_storage_dual_erase_partition_cmd_cb(ret,
                    pCmd->agent_storage_type,
                    pCmd->agent_length,
                    pCmd->agent_storage_address,
                    pCmdMsg->pktId.field.app_id,
                    channel_id,
                    TRUE,
                    (void *)((uint32_t)process_id));
            /* If RACE_ERRCODE_SUCCESS == ret, noti need not be freed.
                           * If not, noti has been freed by race_storage_dual_erase_partition_cmd_cb;
                           */
            noti = NULL;
        } else if (RACE_ERRCODE_MORE_OPERATION == ret) {
            ret = RACE_ERRCODE_SUCCESS;
        }
        }
exit:
#endif /* RACE_LPCOMM_ENABLE */

        pEvt->status = ret;
        if (RACE_ERRCODE_SUCCESS != ret) {
#ifdef RACE_LPCOMM_ENABLE
            /* 8. Free noti if needed */
            /* process_id must be initialized to 0 when defining. */
            if (noti) {
                race_lpcomm_ps_noti_free(process_id);
            }
#endif
        }
    }

    return pEvt;
}


void *race_cmdhdl_storage_lock_unlock(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t storage_type;
        uint8_t agent_or_partner;
        uint8_t lock_or_unlock;
    } PACKED CMD;

    typedef struct {
        uint8_t status;
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_STORAGE_LOCK_UNLOCK,
                                      sizeof(RSP),
                                      channel_id);

    if (pEvt) {
        if (!pCmd->agent_or_partner) {
            /* Agent */
            race_storage_lock_unlock_noti_struct *noti = NULL;

            /* A1. Execute the cmd. */
            ret = RACE_ERRCODE_SUCCESS;

            if (RACE_ERRCODE_SUCCESS == ret) {
                /* A2. Create the noti. */
                noti = (void *)RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                                     RACE_TYPE_NOTIFICATION,
                                                     RACE_STORAGE_LOCK_UNLOCK,
                                                     sizeof(race_storage_lock_unlock_noti_struct),
                                                     channel_id);
                if (noti) {
                    /* A3. Set the noti parameters with the cmd results.  */
                    noti->status = RACE_ERRCODE_SUCCESS;
                    noti->storage_type = pCmd->storage_type;
                    noti->agent_or_partner = pCmd->agent_or_partner;
                    noti->lock_or_unlock = pCmd->lock_or_unlock;

                    /* A4. Send the noti. */
                    ret = race_noti_send(noti, channel_id, TRUE);

                    if (RACE_ERRCODE_SUCCESS != ret) {
                        /* A5. Free the noti if needed. */
                        RACE_FreePacket(noti);
                        noti = NULL;
                    }
                } else {
                    ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
                }
            }
        } else {
            /* Client */
#ifdef RACE_LPCOMM_ENABLE
            race_storage_lock_unlock_noti_struct *noti = NULL;
            uint16_t process_id = 0;
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            race_lpcomm_storage_lock_unlock_req_struct req = {0};
#endif

            /* C1. Create the noti */
            ret = race_lpcomm_ps_noti_create((void **)&noti,
                                             &process_id,
                                             RACE_STORAGE_LOCK_UNLOCK,
                                             pCmdMsg->pktId.field.app_id,
                                             FALSE,
                                             sizeof(race_storage_lock_unlock_noti_struct),
                                             channel_id);
            if (RACE_ERRCODE_SUCCESS == ret) {
                /* C2. Set const parameters for the noti */
                noti->storage_type = pCmd->storage_type;
                noti->agent_or_partner = pCmd->agent_or_partner;
                noti->lock_or_unlock = pCmd->lock_or_unlock;

                /* C3. Send the req to the peer */
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
                req.storage_type = pCmd->storage_type;
                req.lock_or_unlock = pCmd->lock_or_unlock;

                ret = race_lpcomm_send_race_cmd_req_to_peer((uint8_t *)&req,
                        sizeof(race_lpcomm_storage_lock_unlock_req_struct),
                        RACE_LPCOMM_ROLE_AGENT,
                        RACE_STORAGE_LOCK_UNLOCK,
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

                /* C3. Send the req to the peer */
                if (RACE_ERRCODE_SUCCESS != ret) {
                    /* C4. Free the noti if needed */
                    race_lpcomm_ps_noti_free(process_id);
                    noti = NULL;
                }
            }
#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif /* RACE_LPCOMM_ENABLE */
        }

        pEvt->status = ret;
    }

    return pEvt;
}


void *race_cmdhdl_storage_get_partition_sha256(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t storage_type;
        uint8_t agent_or_partner;
        uint32_t partition_address;
        uint32_t partition_length;
    } PACKED CMD;

    typedef struct {
        uint8_t status;
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_STORAGE_GET_PARTITION_SHA256,
                                      sizeof(RSP),
                                      channel_id);
    if (pEvt) {
        if (!pCmd->agent_or_partner) {
            /* Agent */
            race_storage_get_partition_sha256_noti_struct *noti = NULL;
            unsigned char sha256[RACE_STORAGE_SHA256_SIZE] = {0};

            /* A1. Execute the cmd. */
            ret = race_storage_sha256_generate(sha256,
                                               pCmd->partition_address,
                                               pCmd->partition_length,
                                               pCmd->storage_type);
            if (RACE_ERRCODE_SUCCESS == ret) {
                /* A2. Create the noti(to Smart Phone). */
                noti = (void *)RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                                     RACE_TYPE_NOTIFICATION,
                                                     RACE_STORAGE_GET_PARTITION_SHA256,
                                                     sizeof(race_storage_get_partition_sha256_noti_struct),
                                                     channel_id);
                if (noti) {
                    /* A3. Set the noti parameters with the cmd results.  */
                    noti->status = RACE_ERRCODE_SUCCESS;
                    noti->agent_or_partner = pCmd->agent_or_partner;
                    noti->storage_type = pCmd->storage_type;
                    noti->partition_address = pCmd->partition_address;
                    noti->partition_length = pCmd->partition_length;
                    memcpy(noti->sha256, sha256, RACE_STORAGE_SHA256_SIZE);

                    /* A4. Send the noti. */
                    ret = race_noti_send(noti, channel_id, TRUE);
                    if (RACE_ERRCODE_SUCCESS != ret) {
                        /* A5. Free the noti if needed. */
                        RACE_FreePacket(noti);
                        noti = NULL;
                    }
                } else {
                    ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
                }
            }
        } else {
            /* Client */
#ifdef RACE_LPCOMM_ENABLE
            race_storage_get_partition_sha256_noti_struct *noti = NULL;
            uint16_t process_id = 0;
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            race_lpcomm_storage_get_partition_sha256_req_struct req = {0};
#endif

            /* C1. Create the noti (To partner)*/
            ret = race_lpcomm_ps_noti_create((void **)&noti,
                                             &process_id,
                                             RACE_STORAGE_GET_PARTITION_SHA256,
                                             pCmdMsg->pktId.field.app_id,
                                             FALSE,
                                             sizeof(race_storage_get_partition_sha256_noti_struct),
                                             channel_id);
            if (RACE_ERRCODE_SUCCESS == ret) {
                /* C2. Store the const parameters in noti. */
                noti->storage_type = pCmd->storage_type;
                noti->agent_or_partner = pCmd->agent_or_partner;
                noti->partition_address = pCmd->partition_address;
                noti->partition_length = pCmd->partition_length;

                /* C3. Send the req to the peer */
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
                req.storage_type = pCmd->storage_type;
                req.partition_address = pCmd->partition_address;
                req.partition_length = pCmd->partition_length;

                ret = race_lpcomm_send_race_cmd_req_to_peer((uint8_t *)&req,
                        sizeof(race_lpcomm_storage_get_partition_sha256_req_struct),
                        RACE_LPCOMM_ROLE_AGENT,
                        RACE_STORAGE_GET_PARTITION_SHA256,
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

                /* C3. Send the req to the peer */
                if (RACE_ERRCODE_SUCCESS != ret) {
                    /* C4. Free the noti if needed */
                    race_lpcomm_ps_noti_free(process_id);
                    noti = NULL;
                }
            }
#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif /* RACE_LPCOMM_ENABLE */
        }

        pEvt->status = ret;
    }

    return pEvt;
}


void *race_cmdhdl_storage_get_4k_erased_status(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t storage_type;
        uint8_t agent_or_partner;
        uint32_t partition_address;
        uint32_t partition_length;
    } PACKED CMD;

    typedef struct {
        uint8_t status;
    } PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_STORAGE_GET_4K_ERASED_STATUS,
                                      sizeof(RSP),
                                      channel_id);
    uint16_t erase_status_size = 0;


    if (pEvt) {
        if (!pCmd->agent_or_partner) {
            /* Agent */
            race_storage_get_4k_erased_status_noti_struct *noti = NULL;

            /* A1. Execute the cmd. */
            /* Get erase_status size. */
            ret = race_storage_get_partition_erase_status(NULL,
                    &erase_status_size,
                    pCmd->partition_address,
                    pCmd->partition_length,
                    pCmd->storage_type);
            if (RACE_ERRCODE_SUCCESS == ret) {
                /* A2. Create the noti. */
                noti = (void *)RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                                     RACE_TYPE_NOTIFICATION,
                                                     RACE_STORAGE_GET_4K_ERASED_STATUS,
                                                     sizeof(race_storage_get_4k_erased_status_noti_struct) + erase_status_size,
                                                     channel_id);
                if (noti) {
                    /* A3. Set the noti parameters with the cmd results.  */
                    noti->erase_status_size = erase_status_size;
                    ret = race_storage_get_partition_erase_status(noti->erase_status,
                            &noti->erase_status_size,
                            pCmd->partition_address,
                            pCmd->partition_length,
                            pCmd->storage_type);
                    if (RACE_ERRCODE_SUCCESS == ret) {
                        noti->status = ret;
                        noti->storage_type = pCmd->storage_type;
                        noti->agent_or_partner = pCmd->agent_or_partner;
                        noti->partition_address = pCmd->partition_address;
                        noti->partition_length = pCmd->partition_length;
                        /* A4. Send the noti. */
                        ret = race_noti_send(noti, channel_id, TRUE);
                    }

                    if (RACE_ERRCODE_SUCCESS != ret) {
                        /* A5. Free the noti if needed. */
                        RACE_FreePacket(noti);
                        noti = NULL;
                    }
                } else {
                    ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
                }
            }
        } else {
            /* Client */
#ifdef RACE_LPCOMM_ENABLE
            race_storage_get_4k_erased_status_noti_struct *noti = NULL;
            uint16_t process_id = 0;
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
            race_lpcomm_storage_get_4k_erase_status_req_struct req = {0};
#endif

            /* C1. Create the noti */
            /* Get erase_status size. */
            ret = race_storage_get_partition_erase_status(NULL,
                    &erase_status_size,
                    pCmd->partition_address,
                    pCmd->partition_length,
                    pCmd->storage_type);
            if (RACE_ERRCODE_SUCCESS == ret) {
                ret = race_lpcomm_ps_noti_create((void **)&noti,
                                                 &process_id,
                                                 RACE_STORAGE_GET_4K_ERASED_STATUS,
                                                 pCmdMsg->pktId.field.app_id,
                                                 FALSE,
                                                 sizeof(race_storage_get_4k_erased_status_noti_struct) + erase_status_size,
                                                 channel_id);
                if (RACE_ERRCODE_SUCCESS == ret) {
                    /* C2. Store the const parameters in noti. */
                    noti->storage_type = pCmd->storage_type;
                    /* Check this size with the size in rsp in req_hdl. */
                    noti->erase_status_size = erase_status_size;
                    noti->agent_or_partner = pCmd->agent_or_partner;
                    noti->partition_address = pCmd->partition_address;
                    noti->partition_length = pCmd->partition_length;

                    /* C3. Send the req to the peer */
#if defined(RACE_AWS_ENABLE) || defined(RACE_COSYS_ENABLE)
                    req.storage_type = pCmd->storage_type;
                    req.partition_address = pCmd->partition_address;
                    req.partition_length = pCmd->partition_length;

                    ret = race_lpcomm_send_race_cmd_req_to_peer((uint8_t *)&req,
                            sizeof(race_lpcomm_storage_get_4k_erase_status_req_struct),
                            RACE_LPCOMM_ROLE_AGENT,
                            RACE_STORAGE_GET_4K_ERASED_STATUS,
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

                    /* C3. Send the req to the peer */
                    if (RACE_ERRCODE_SUCCESS != ret) {
                        /* C4. Free the noti if needed */
                        race_lpcomm_ps_noti_free(process_id);
                        noti = NULL;
                    }
                }
            }
#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif /* RACE_LPCOMM_ENABLE */
        }

        pEvt->status = ret;
    }

    return pEvt;
}


void *race_cmdhdl_storage(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("race_cmdhdl_storage, pCmdMsg->hdr.id[0x%X]", 1, (int)pCmdMsg->hdr.id);

    switch (pCmdMsg->hdr.id) {
        case RACE_STORAGE_WRITE_BYTE:
            return race_cmdhdl_storage_write_byte((PTR_RACE_COMMON_HDR_STRU) & (pCmdMsg->hdr), length, channel_id);

        case RACE_STORAGE_WRITE_PAGE:
            return race_cmdhdl_storage_write_page((PTR_RACE_COMMON_HDR_STRU) & (pCmdMsg->hdr), length, channel_id);

        case RACE_STORAGE_READ_PAGE:
            return race_cmdhdl_storage_read_page((PTR_RACE_COMMON_HDR_STRU) & (pCmdMsg->hdr), length, channel_id);

        case RACE_STORAGE_ERASE_PARTITION:
            return race_cmdhdl_storage_erase_partition((PTR_RACE_COMMON_HDR_STRU) & (pCmdMsg->hdr), length, channel_id);

        case RACE_STORAGE_LOCK_UNLOCK:
            return race_cmdhdl_storage_lock_unlock((PTR_RACE_COMMON_HDR_STRU) & (pCmdMsg->hdr), length, channel_id);

        case RACE_STORAGE_GET_PARTITION_SHA256:
            return race_cmdhdl_storage_get_partition_sha256((PTR_RACE_COMMON_HDR_STRU) & (pCmdMsg->hdr), length, channel_id);

        case RACE_STORAGE_DUAL_DEVICES_ERASE_PARTITION:
            return race_cmdhdl_storage_dual_devices_erase_partition((PTR_RACE_COMMON_HDR_STRU) & (pCmdMsg->hdr), length, channel_id);

        case RACE_STORAGE_GET_4K_ERASED_STATUS:
            return race_cmdhdl_storage_get_4k_erased_status((PTR_RACE_COMMON_HDR_STRU) & (pCmdMsg->hdr), length, channel_id);

        default:
            break;
    }

    return NULL;
}

#endif /* RACE_STORAGE_CMD_ENABLE */

