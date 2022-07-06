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
#ifdef RACE_LPCOMM_ENABLE
#include "race_lpcomm_ps_noti.h"
#include "race_lpcomm_ps_list.h"
#include "race_noti.h"
#include "race_util.h"
#include "race_lpcomm_ps.h"


typedef struct
{
    int8_t status;
}PACKED race_lpcomm_noti_template_struct;


/* This API should be called with race_lpcomm_ps_noti_try_send(), since race_lpcomm_ps_noti_try_send() 
  * will help to free the resources when necessary. */
RACE_ERRCODE race_lpcomm_ps_noti_create(void **noti,
                                                 uint16_t *process_id,
                                                 uint16_t cmd_id,
                                                 uint8_t app_id,
                                                 bool dual_cmd,                                                 
                                                 uint16_t noti_len,
                                                 uint8_t channel_id)
{
    race_send_pkt_t *send_pkt = NULL;
    process_status_info *process_status = NULL;

    if (!noti || *noti || !noti_len ||
        !process_id || *process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    /* 1. Create Noti */
    *noti = RACE_ClaimPacketAppID(app_id,
                                  RACE_TYPE_NOTIFICATION,
                                  cmd_id,
                                  noti_len,
                                  channel_id);
    
    if (!(*noti))
    {
        return RACE_ERRCODE_FAIL;
    }

    /* Start address of the real noti packet */
    send_pkt = race_pointer_cnv_pkt_to_send_pkt(*noti);

    /* 2. Store the const parameters in noti. */
    // No paraemter to store for this cmd

    /* 3. Create process_status and attach noti with it. */
    *process_id = race_gen_process_id();
    process_status = race_lpcomm_process_status_create(*process_id,
                                                       dual_cmd,
                                                       send_pkt);
    if (!process_status)
    {
        *process_id = 0;
        RACE_FreePacket(*noti);
        *noti = NULL;

        return RACE_ERRCODE_FAIL;
    }

    return RACE_ERRCODE_SUCCESS;
}


void *race_lpcomm_ps_noti_find(uint16_t process_id)
{
    process_status_info *process_status = race_lpcomm_process_status_info_find(process_id);

    if (process_status)
    {
        if (!process_status->tmp_result)
        {
            RACE_LOG_MSGID_E("Node in ps_list without tmp_result!", 0);
            race_lpcomm_process_status_free_by_id(process_id);
            return NULL;
        }

        return race_pointer_cnv_send_pkt_to_pkt((race_send_pkt_t *)process_status->tmp_result);
    }

    return NULL;
}


/* If the return value is RACE_ERRCODE_SUCCESS, no need to call race_lpcomm_ps_noti_free(). 
  * If send immediately will make 5D before 5B, delay to send.
  */
RACE_ERRCODE race_lpcomm_ps_noti_try_send(bool *noti_sent,
                                                    uint16_t process_id,
                                                    uint8_t channel_id,
                                                    uint8_t status,
                                                    race_lpcomm_role_enum role,
                                                    bool noti_delay)
{
    int32_t ret = RACE_ERRCODE_FAIL;
    process_status_info *process_status = race_lpcomm_process_status_info_find(process_id);

    if (!noti_sent || !process_status)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    *noti_sent = FALSE;

    if (!process_status->tmp_result)
    {
        RACE_LOG_MSGID_E("Node in ps_list without tmp_result!", 0);
        return RACE_ERRCODE_FAIL;
    }

    /* 7. Update process_status */
    ret = race_lpcomm_process_status_info_update(process_status,
                                                 channel_id,
                                                 status,
                                                 role);
    if (RACE_ERRCODE_SUCCESS == ret)
    {
        if (race_lpcomm_process_status_info_finish_check(process_status))
        {
            race_lpcomm_noti_template_struct *noti = NULL;

            noti = race_pointer_cnv_send_pkt_to_pkt((race_send_pkt_t *)process_status->tmp_result);

            noti->status = process_status->status;
            ret = race_noti_send(noti,
                                 process_status->channel_id,
                                 noti_delay);
            if (RACE_ERRCODE_SUCCESS == ret)
            {
                *noti_sent = TRUE;
            }
        }
    }

    if (RACE_ERRCODE_SUCCESS == ret && (*noti_sent))
    {
        if (*noti_sent)
        {
            RACE_LOG_MSGID_I("Noti is sent. process_id:%d",1, process_id);
        }
        /* 8. Free noti & process_status if needed. If ret is not SUCCESS, race_lpcomm_ps_noti_free()
         *      must be called to free noti & process_status.
         */
        race_lpcomm_process_status_free_by_id(process_id);
    }

    return ret;
}


void race_lpcomm_ps_noti_free(uint16_t process_id)
{
    process_status_info *process_status = race_lpcomm_process_status_info_find(process_id);

    if (process_status)
    {
        if (process_status->tmp_result)
        {
            race_mem_free(process_status->tmp_result);
        }

        race_lpcomm_process_status_free_by_id(process_id);
    }
}

#endif /* RACE_LPCOMM_ENABLE */

