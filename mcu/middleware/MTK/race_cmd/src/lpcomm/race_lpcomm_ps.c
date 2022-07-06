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
#include "race_lpcomm_ps.h"
#include "race_lpcomm_ps_list.h"


#define RACE_LPCOMM_DUAL_PROCESS_LOCAL_DONE   (0x01)
#define RACE_LPCOMM_DUAL_PROCESS_PEER_DONE   (0x02)


RACE_ERRCODE race_lpcomm_process_status_free_by_id(uint16_t process_id)
{
    process_status_info_list *list_node = NULL;
    
    if (!process_id)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    list_node = race_lpcomm_ps_list_node_find_by_process_id(process_id);
    if (list_node)
    {
        race_lpcomm_ps_list_remove(list_node);
        race_lpcomm_ps_node_free(list_node);
    }

    return RACE_ERRCODE_SUCCESS;
}


/* Create a process_status and link the tmp_result to the process_status. */
process_status_info *race_lpcomm_process_status_create(uint16_t process_id,
                                                                  bool dual_cmd,
                                                                  void *tmp_result)
{
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    process_status_info_list *list_node = NULL;

    list_node = race_lpcomm_ps_list_node_find_by_process_id(process_id);
    if (list_node)
    {
        RACE_LOG_MSGID_E("process_id:%d has been allocated.",1, process_id);
        return NULL;
    }

    list_node = race_lpcomm_ps_node_alloc(process_id);
    if (!list_node)
    {
        return NULL;
    }
    
    list_node->process_status.dual_cmd = dual_cmd;
    list_node->process_status.tmp_result = tmp_result;
    ret = race_lpcomm_ps_list_insert(list_node);
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        race_lpcomm_ps_node_free(list_node);
        return NULL;
    }

    return &(list_node->process_status);
}


process_status_info *race_lpcomm_process_status_info_find(uint16_t process_id)
{
    process_status_info_list *list_node = NULL;

    if (!process_id)
    {
        return NULL;
    }

    list_node = race_lpcomm_ps_list_node_find_by_process_id(process_id);
    if (!list_node)
    {
        return NULL;
    }

    return &(list_node->process_status);
}


RACE_ERRCODE race_lpcomm_process_status_info_update(process_status_info *process_status,
                                                                 uint8_t channel_id,
                                                                 int8_t status,
                                                                 race_lpcomm_role_enum role)
{
    if (!process_status ||
        ((RACE_LPCOMM_DUAL_PROCESS_LOCAL_DONE & process_status->process_status) &&
        RACE_LPCOMM_ROLE_AGENT == role) ||
        ((RACE_LPCOMM_DUAL_PROCESS_PEER_DONE & process_status->process_status) &&
        RACE_LPCOMM_ROLE_PARTNER == role) ||
        (RACE_LPCOMM_ROLE_AGENT != role && RACE_LPCOMM_ROLE_PARTNER != role) ||
        (process_status->channel_id && channel_id &&
         process_status->channel_id != channel_id))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if ((RACE_LPCOMM_DUAL_PROCESS_LOCAL_DONE & process_status->process_status) &&
        (RACE_LPCOMM_DUAL_PROCESS_PEER_DONE & process_status->process_status))
    {
        RACE_LOG_MSGID_W("Both results has been updated.", 0);
        return RACE_ERRCODE_FAIL;
    }

    if (!(RACE_LPCOMM_DUAL_PROCESS_LOCAL_DONE & process_status->process_status) &&
        !(RACE_LPCOMM_DUAL_PROCESS_PEER_DONE & process_status->process_status))
    {
        RACE_LOG_MSGID_I("First result. role:%d status:%x",2, 
                   role, status);
        process_status->channel_id = channel_id;
        process_status->status = status;
    }
    else
    {
        RACE_LOG_MSGID_I("Second result. role:%d 1st_status:%x 2nd_status:%x",3,
                   role, process_status->status, status);
        if (RACE_ERRCODE_SUCCESS == process_status->status)
        {
            process_status->status = status;
        }
    }

    if (RACE_LPCOMM_ROLE_AGENT == role)
    {
        process_status->process_status |= RACE_LPCOMM_DUAL_PROCESS_LOCAL_DONE;
    }
    else
    {
        process_status->process_status |= RACE_LPCOMM_DUAL_PROCESS_PEER_DONE;
    }

    return RACE_ERRCODE_SUCCESS;
}


bool race_lpcomm_process_status_info_finish_check(process_status_info *process_status)
{
    if (process_status && 
        ((!process_status->dual_cmd &&
          ((RACE_LPCOMM_DUAL_PROCESS_LOCAL_DONE & process_status->process_status) ||
           (RACE_LPCOMM_DUAL_PROCESS_PEER_DONE & process_status->process_status))) ||
         (process_status->dual_cmd &&
          ((RACE_LPCOMM_DUAL_PROCESS_LOCAL_DONE & process_status->process_status) &&
           (RACE_LPCOMM_DUAL_PROCESS_PEER_DONE & process_status->process_status)))))
    {
        return TRUE;
    }

    return FALSE;
}

#endif /* RACE_LPCOMM_ENABLE */

