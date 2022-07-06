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
#include "fota_multi_info.h"
#include "race_xport.h"
#include "race_lpcomm_aws.h"
#include "race_lpcomm_ps_list.h"


#define race_lpcomm_ps_LIST_NODES_MAX_SIZE 10


typedef struct
{
    process_status_info_list list_nodes[race_lpcomm_ps_LIST_NODES_MAX_SIZE];
    process_status_info_list *list;
}race_lpcomm_ps_list_context_struct;


static race_lpcomm_ps_list_context_struct *g_race_lpcomm_ps_list_cntx;


bool race_lpcomm_ps_list_init(void)
{
    if (g_race_lpcomm_ps_list_cntx)
    {
        return TRUE;
    }

    g_race_lpcomm_ps_list_cntx = race_mem_alloc(sizeof(race_lpcomm_ps_list_context_struct));
    if (g_race_lpcomm_ps_list_cntx)
    {
        memset(g_race_lpcomm_ps_list_cntx, 0, sizeof(race_lpcomm_ps_list_context_struct));
        return TRUE;
    }

    return FALSE;
}


void race_lpcomm_ps_list_deinit(void)
{
    if (g_race_lpcomm_ps_list_cntx)
    {
        race_lpcomm_ps_list_destory();

        race_mem_free(g_race_lpcomm_ps_list_cntx);
        g_race_lpcomm_ps_list_cntx = NULL;
    }
}


process_status_info_list *race_lpcomm_ps_node_alloc(uint16_t process_id)
{
    uint32_t i = 0;

    if (!g_race_lpcomm_ps_list_cntx || !process_id)
    {
        return NULL;
    }

    for (i = 0; i < race_lpcomm_ps_LIST_NODES_MAX_SIZE; i++)
    {
        if (!g_race_lpcomm_ps_list_cntx->list_nodes[i].is_used)
        {
            // RACE_LOG_MSGID_I("Allocate a list_node idx:%d.", 1, i);
            memset(&(g_race_lpcomm_ps_list_cntx->list_nodes[i]), 0, sizeof(process_status_info_list));
            g_race_lpcomm_ps_list_cntx->list_nodes[i].is_used = TRUE;
            g_race_lpcomm_ps_list_cntx->list_nodes[i].process_id = process_id;
            return &(g_race_lpcomm_ps_list_cntx->list_nodes[i]);
        }
    }

    RACE_LOG_MSGID_E("Failed to allocate a list_node.", 0);
    return NULL;
}


void race_lpcomm_ps_node_free(void *node)
{
    process_status_info_list *list_node = (process_status_info_list *)node;

    if (list_node)
    {
        list_node->is_used = FALSE;
    }
}


RACE_ERRCODE race_lpcomm_ps_list_insert(process_status_info_list *list_node)
{
    return race_list_insert((race_template_list_struct **)&(g_race_lpcomm_ps_list_cntx->list),
                            (race_template_list_struct *)list_node);
}


RACE_ERRCODE race_lpcomm_ps_list_remove(process_status_info_list *list_node)
{
    return race_list_remove((race_template_list_struct **)&(g_race_lpcomm_ps_list_cntx->list),
                            (race_template_list_struct *)list_node);
}


RACE_ERRCODE race_lpcomm_ps_list_destory(void)
{
    return race_list_destory((race_template_list_struct **)&(g_race_lpcomm_ps_list_cntx->list),
                             race_lpcomm_ps_node_free);
}


process_status_info_list *race_lpcomm_ps_list_node_find_by_process_id(uint16_t process_id)
{
    process_status_info_list *list_node = g_race_lpcomm_ps_list_cntx->list;

    if (!list_node || !process_id)
    {
        return NULL;
    }

    while (list_node)
    {
        if (list_node->process_id == process_id && list_node->is_used)
        {
            return list_node;
        }

        list_node = list_node->next;
    }

    RACE_LOG_MSGID_E("Not Found! process_id:%d",1, process_id);
    return NULL;
}


#endif /* RACE_LPCOMM_ENABLE*/

