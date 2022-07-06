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

#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_ami.h"
#include "audio_src_srv.h"
#include "bt_sink_srv_music_rho.h"
#include "bt_sink_srv_a2dp.h"
#include "bt_sink_srv_avrcp.h"
#include "bt_connection_manager.h"
#include "bt_timer_external.h"
#include "bt_device_manager_internal.h"
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "bt_role_handover.h"
static bt_status_t bt_sink_srv_music_role_handover_service_allowed(const bt_bd_addr_t *addr);
static uint8_t bt_sink_srv_music_role_handover_service_get_length(const bt_bd_addr_t *addr);
static bt_status_t bt_sink_srv_music_role_handover_service_get_data(const bt_bd_addr_t *addr,
    void *data);
static bt_status_t bt_sink_srv_music_role_handover_service_update(bt_role_handover_update_info_t *info);
static void bt_sink_srv_music_role_handover_service_status_callback(const bt_bd_addr_t *addr,
    bt_aws_mce_role_t role, bt_role_handover_event_t event,
    bt_status_t status);
#endif

#define BT_SINK_SRV_MUSIC_RHO_SWAP(a,b) {typeof(a) temp = a; a=b; b = temp;}

bool bt_sink_srv_music_role_handover_is_allowed(const bt_bd_addr_t *addr)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    bt_sink_srv_music_device_t *sp_dev = ctx->run_dev ? ctx->run_dev:bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_SP, NULL);
    bool ret = true;
    uint32_t connect_bit = 0;
    uint32_t sp_flag = 0;
    uint32_t sp_op = 0;
    
#ifdef  AIR_MULTI_POINT_ENABLE
    bt_sink_srv_music_device_t *device_list = ctx->sink_dev;
    uint32_t dev_index = 0;
    for(dev_index = 0; dev_index < BT_SINK_SRV_MUISC_DEV_COUNT; dev_index++) {
        sp_dev = &(device_list[dev_index]);
        if(((sp_dev->conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT)
            || (sp_dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT))
            && (sp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_AVRCP_CONN_RESULT)) {
            ret = false;

            sp_flag = sp_dev->flag;
            sp_op = sp_dev->op;
            connect_bit = sp_dev->conn_bit;
            break;
        }
    }

    if(ret && run_dev
        && (!(run_dev->op&BT_SINK_SRV_MUSIC_OP_DRV_PLAY))) {
        ret = false;
    }
#else
    if(sp_dev) {
        if (sp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_AVRCP_CONN_RESULT) {
            ret = false;
        }else if(ctx->run_dev && ((!(sp_dev->op&BT_SINK_SRV_MUSIC_OP_DRV_PLAY)) || (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE != sp_dev->handle->substate))) {
            ret = false;
        }
        sp_flag = sp_dev->flag;
        sp_op = sp_dev->op;
        connect_bit = sp_dev->conn_bit;
    }
#endif
    if(ret) {
#ifdef MTK_AUDIO_SYNC_ENABLE
        extern uint8_t bt_sink_srv_get_sync_counter(void);
        if (0 != bt_sink_srv_get_sync_counter()) {
            ret = false;
        } else {
            ctx->rho_flag = 1;
        }
#else
        ctx->rho_flag = 1;
#endif
    }

    bt_sink_srv_report_id("[music_rho]ret:%d, flag: 0x%08x, op:0x%08x, conn_bit: 0x%08x, sp_dev:0x%08x, run_dev:0x%08x", 6,
        ret, sp_flag, sp_op, connect_bit, sp_dev, run_dev);
    return ret;
}

uint8_t bt_sink_srv_music_role_handover_get_length(const bt_bd_addr_t* addr)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *sp_dev = ctx->run_dev ? ctx->run_dev:bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_SP, NULL);
    uint8_t ctx_len = sizeof(bt_sink_srv_music_change_context_t);
    if(!sp_dev) {
        ctx_len = 0;
    }
#ifdef  AIR_MULTI_POINT_ENABLE
    else {
        if(!addr) {
            ctx_len = 0;
        }
    }
#endif
#ifndef SUPPORT_ROLE_HANDOVER_SERVICE
    ctx->rho_flag = 1;
#endif
    bt_sink_srv_report_id("[music_rho]rho_get_length return len is %d", 1, ctx_len);
    return ctx_len;
}

void bt_sink_srv_music_role_handover_get_data(const bt_bd_addr_t* addr, uint8_t *data)
{
    bt_sink_srv_report_id("[music_rho] rho get data", 0);

    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *sp_dev = ctx->run_dev ? ctx->run_dev:bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_SP, NULL);
    bt_sink_srv_music_change_context_t *change_ctx = (bt_sink_srv_music_change_context_t *)data;
    if(sp_dev && change_ctx) {

#ifdef  AIR_MULTI_POINT_ENABLE
        if(!addr) {
            if(ctx->run_dev) {
                bt_sink_srv_music_common_exchange_context_t *common_ctx = (bt_sink_srv_music_common_exchange_context_t *)data;
                bt_sink_srv_memcpy(&(common_ctx->playing_addr),&ctx->run_dev->dev_addr, sizeof(bt_bd_addr_t));
            }
            return;
        }
        sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, (void *)addr);
        if(!sp_dev) sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_AVRCP, (void *)addr);
        if(!sp_dev)  {
            BT_SINK_SRV_SET_FLAG(change_ctx->rho_temp_flag, BT_SINK_SRV_MUSIC_RHO_EMPTY_DEVICE_FLAG);
            return;
        }
#endif

        change_ctx->conn_bit = sp_dev->conn_bit;
        change_ctx->flag = sp_dev->flag;
        change_ctx->a2dp_status = sp_dev->a2dp_status;
        change_ctx->avrcp_status = sp_dev->avrcp_status;
        change_ctx->last_play_pause_action = sp_dev->last_play_pause_action;
        change_ctx->rho_temp_flag = 0;
        change_ctx->avrcp_volume = sp_dev->avrcp_volume;
#ifdef  AIR_MULTI_POINT_ENABLE
        bt_sink_srv_memcpy(&(change_ctx->codec_cap), &(sp_dev->codec), sizeof(bt_a2dp_codec_capability_t));
#endif
        bt_device_manager_db_remote_pnp_info_t device_id;
        if (BT_STATUS_SUCCESS == bt_device_manager_remote_find_pnp_info(sp_dev->dev_addr, &device_id)) {
            bt_sink_srv_memcpy(&change_ctx->device_id, &device_id, sizeof(bt_device_manager_db_remote_pnp_info_t));
        }
        if(sp_dev->handle->flag & AUDIO_SRC_SRV_FLAG_WAITING) {
            BT_SINK_SRV_SET_FLAG(change_ctx->rho_temp_flag, BT_SINK_SRV_MUSIC_RHO_WAITING_LIST_FLAG);
        }
        if(sp_dev->op&BT_SINK_SRV_MUSIC_START_PLAY_ON_NEW_AGENT_FLAG) {
            bt_sink_srv_report_id("[music_rho] BT_SINK_SRV_MUSIC_START_PLAY_ON_NEW_AGENT_FLAG on.", 0);
            BT_SINK_SRV_REMOVE_FLAG(sp_dev->op, BT_SINK_SRV_MUSIC_START_PLAY_ON_NEW_AGENT_FLAG);
            BT_SINK_SRV_SET_FLAG(change_ctx->rho_temp_flag, BT_SINK_SRV_MUSIC_RHO_START_ON_NEW_AGENT);
        }
        change_ctx->op = sp_dev->op;
        bt_sink_srv_music_stored_data_t dev_db;

        if (bt_sink_srv_music_get_a2dp_nvdm_data(&(sp_dev->dev_addr), &dev_db, sizeof(dev_db))) {
            if ((dev_db.music_volume & 0xff00) == BT_SINK_SRV_A2DP_MAGIC_CODE) {
                /* use storge volume value */
                change_ctx->vol = (dev_db.music_volume & 0x00ff);
            } else {
                change_ctx->vol = BT_SINK_SRV_A2DP_INVALID_VOLUME;
            }
        }
        change_ctx->vol_rsp_flag = sp_dev->volume_change_status;
    }
}

static void bt_sink_srv_music_role_handover_update_sp_profile_handle(bt_sink_srv_music_device_t *sp_dev, bt_aws_mce_role_handover_profile_t *profile_data)
{
    sp_dev->a2dp_hd = profile_data->a2dp_handle;
    sp_dev->avrcp_hd = profile_data->avrcp_handle;
    sp_dev->avrcp_browse_hd = profile_data->avrcp_handle;//Temp solution
}

static void bt_sink_srv_music_role_handover_update_sp_context(bt_sink_srv_music_device_t *sp_dev, bt_bd_addr_t *sp_addr, bt_sink_srv_music_change_context_t *change_ctx)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;

    sp_dev->handle->type = AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP;
    if(run_dev && sp_dev == run_dev && (!(run_dev->op&BT_SINK_SRV_MUSIC_OP_DRV_PLAY))) {
        BT_SINK_SRV_SET_FLAG(sp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_RHO_HAPPEN_DURING_STARTING_PLAY);
        bt_sink_srv_report_id("[music_rho]RHO happen during partner starting play", 0);
    } else {
        sp_dev->flag = change_ctx->flag;
        sp_dev->op = change_ctx->op;
    }
    sp_dev->conn_bit = change_ctx->conn_bit;
    sp_dev->volume_change_status = change_ctx->vol_rsp_flag;
    //bt_sink_srv_avrcp_set_vol_rsp_flag(change_ctx->vol_rsp_flag);
    
    bt_sink_srv_report_id("[music_rho] dev:0x%08x", 1,
        sp_dev);
    bt_sink_srv_memcpy(&sp_dev->dev_addr, sp_addr, sizeof(bt_bd_addr_t));
    sp_dev->aws_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
    sp_dev->handle->dev_id = bt_sink_srv_music_convert_btaddr_to_devid(sp_addr);
    sp_dev->a2dp_status = change_ctx->a2dp_status;
    sp_dev->avrcp_status = change_ctx->avrcp_status;
    sp_dev->last_play_pause_action = change_ctx->last_play_pause_action;
    sp_dev->avrcp_volume = change_ctx->avrcp_volume;
#ifdef  AIR_MULTI_POINT_ENABLE
    bt_sink_srv_memcpy(&(sp_dev->codec), &(change_ctx->codec_cap), sizeof(bt_a2dp_codec_capability_t));
#endif
    bt_device_manager_remote_update_pnp_info((void*)sp_addr, &change_ctx->device_id);
    if(change_ctx->rho_temp_flag&BT_SINK_SRV_MUSIC_RHO_WAITING_LIST_FLAG) {
        audio_src_srv_add_waiting_list(sp_dev->handle);
    }
    if(change_ctx->vol != BT_SINK_SRV_A2DP_INVALID_VOLUME) {
        if(run_dev == sp_dev) {
            ctx->vol_lev = change_ctx->vol;
        }
        bt_sink_srv_music_set_nvdm_data(&(sp_dev->dev_addr),  BT_SINK_SRV_MUSIC_DATA_VOLUME, &change_ctx->vol);
    }
    if(change_ctx->rho_temp_flag&BT_SINK_SRV_MUSIC_RHO_START_ON_NEW_AGENT) {
        bt_sink_srv_music_state_machine_handle(sp_dev, BT_A2DP_START_STREAMING_IND, NULL);
        bt_sink_srv_report_id("[music_rho] start music on new agent", 0);
    }
    if((sp_dev->conn_bit&BT_SINK_SRV_MUSIC_A2DP_CONN_BIT) && !(sp_dev->conn_bit&BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
        bt_timer_ext_start(BT_SINK_SRV_AVRCP_CONNECTION_TIMER_ID, (uint32_t)sp_dev,
            BT_SINK_SRV_AVRCP_CONNECTION_TIMER_DUR, bt_sink_srv_a2dp_initial_avrcp_timer);
    }

}

//extern audio_src_srv_handle_t *g_aws_mce_a2dp_pse_hd;

void bt_sink_srv_music_role_handover(const bt_bd_addr_t* addr, bt_aws_mce_role_handover_profile_t *profile_data, uint8_t *data, bool is_active)
{
    //bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS,
        (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);

    if(is_active) {
        BT_SINK_SRV_MUSIC_RHO_SWAP(ctx->a2dp_aid, ctx->aws_aid);
    }
    ctx->rho_flag = 0;
    bt_sink_srv_memset(&(ctx->a2dp_eir), 0, sizeof(bt_sink_srv_aws_mce_a2dp_base_eir_t));

    if(profile_data) {
        bt_sink_srv_music_change_context_t *change_ctx = (bt_sink_srv_music_change_context_t *)data;
        bt_sink_srv_assert(aws_a2dp_dev);
        bt_bd_addr_t *sp_addr = (bt_bd_addr_t *)addr;
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        sp_addr = (bt_bd_addr_t *)bt_gap_get_remote_address(profile_data->gap_handle);
#endif

        if(!change_ctx) {
            aws_a2dp_dev->aws_hd = bt_sink_srv_music_get_aws_handle_by_addr(sp_addr);
            aws_a2dp_dev->handle->dev_id = bt_sink_srv_music_convert_btaddr_to_devid(sp_addr);
            bt_sink_srv_memcpy(&(aws_a2dp_dev->dev_addr), sp_addr, sizeof(bt_bd_addr_t));
            bt_sink_srv_report_id("[music_rho] no a2dp&avrcp connection, RHO CLIENT-->AGENT success.", 0);
            return;
        }

        //to alloc sp_aws_dev
        bt_sink_srv_music_device_t *sp_aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
        bt_sink_srv_assert(sp_aws_dev);

        sp_aws_dev->aws_hd = bt_sink_srv_music_get_aws_handle_by_addr(sp_addr);
        bt_sink_srv_memcpy(&(sp_aws_dev->dev_addr), sp_addr, sizeof(bt_bd_addr_t));
        /* Init pse handle */
        sp_aws_dev->handle = bt_sink_srv_a2dp_alloc_pseudo_handle();
        sp_aws_dev->handle->type = AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP;
        bt_sink_srv_music_fill_audio_src_callback((audio_src_srv_handle_t *)sp_aws_dev->handle);
        sp_aws_dev->handle->priority = AUDIO_SRC_SRV_PRIORITY_NORMAL;
        //dev->gap_role = bt_sink_srv_cm_get_gap_role(&(dev->dev_addr));
        sp_aws_dev->handle->dev_id = bt_sink_srv_music_convert_btaddr_to_devid(sp_addr);
#ifdef  AIR_MULTI_POINT_ENABLE
        if(is_active)
#endif
        {
        BT_SINK_SRV_SET_FLAG(sp_aws_dev->conn_bit, BT_SINK_SRV_MUSIC_AWS_CONN_BIT);
        bt_sink_srv_music_state_machine_handle(sp_aws_dev, BT_SINK_SRV_MUSIC_EVT_READY, NULL);
        }
#if 0
        //to alloc agent_aw_dev
        bt_bd_addr_t *old_agent_addr = bt_connection_manager_device_local_info_get_local_address();
        bt_sink_srv_music_device_t *agent_aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
        bt_sink_srv_assert(old_agent_addr);
        bt_sink_srv_assert(agent_aws_dev);
        agent_aws_dev->aws_hd = bt_sink_srv_music_get_aws_handle_by_addr(old_agent_addr);
        bt_sink_srv_memcpy(&(agent_aws_dev->dev_addr), old_agent_addr, sizeof(bt_bd_addr_t));
        /* Init pse handle */
        agent_aws_dev->handle = bt_sink_srv_a2dp_alloc_pseudo_handle();
        agent_aws_dev->handle->type = AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP;
        bt_sink_srv_music_fill_audio_src_callback((audio_src_srv_handle_t *)agent_aws_dev->handle);
        agent_aws_dev->handle->priority = AUDIO_SRC_SRV_PRIORITY_NORMAL;
        //dev->gap_role = bt_sink_srv_cm_get_gap_role(&(dev->dev_addr));
        agent_aws_dev->handle->dev_id = bt_sink_srv_music_convert_btaddr_to_devid(old_agent_addr);
        bt_sink_srv_report_id("[music_rho]sp_aws_dev handle : 0x%08x, agent_aws_dev handle: 0x%08x", 2,
        sp_aws_dev->aws_hd, agent_aws_dev->aws_hd);
#endif
        bt_sink_srv_assert(change_ctx);
        bt_sink_srv_music_device_t *sp_dev = aws_a2dp_dev;
        if(change_ctx->rho_temp_flag & BT_SINK_SRV_MUSIC_RHO_EMPTY_DEVICE_FLAG) {
            bt_sink_srv_report_id("[music_rho] RHO CLIENT-->AGENT success, it`s empty device", 0);
            if(is_active) {
                bt_sink_srv_a2dp_free_pseudo_handle(aws_a2dp_dev->handle);
                aws_a2dp_dev->handle = NULL;
                bt_sink_srv_music_reset_device(aws_a2dp_dev);
            }
            return;
        }

#ifdef  AIR_MULTI_POINT_ENABLE
        if(!is_active){
            sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
            bt_sink_srv_assert(sp_dev);
            
            bt_sink_srv_memcpy(&(sp_dev->dev_addr), sp_addr, sizeof(bt_bd_addr_t));
            /* Init pse handle */
            sp_dev->handle = bt_sink_srv_a2dp_alloc_pseudo_handle();
            sp_dev->handle->type = AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP;
            bt_sink_srv_music_fill_audio_src_callback((audio_src_srv_handle_t *)sp_dev->handle);
            sp_dev->handle->priority = AUDIO_SRC_SRV_PRIORITY_NORMAL;
            //dev->gap_role = bt_sink_srv_cm_get_gap_role(&(dev->dev_addr));
            sp_dev->handle->dev_id = bt_sink_srv_music_convert_btaddr_to_devid(sp_addr);
            bt_sink_srv_music_state_machine_handle(sp_dev, BT_SINK_SRV_MUSIC_EVT_READY, NULL);
        }
#endif

        bt_sink_srv_music_role_handover_update_sp_profile_handle(sp_dev, profile_data);
        bt_sink_srv_music_role_handover_update_sp_context(sp_dev,sp_addr, change_ctx);

        bt_sink_srv_report_id("[music_rho] RHO CLIENT-->AGENT success, conn_bit:0x%08x, flag:0x%08x, rho_flag:0x%08x", 3, change_ctx->conn_bit, change_ctx->flag,
                                                                                                                                        change_ctx->rho_temp_flag);
    } else {

        bt_sink_srv_music_device_t *sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_SP,
            NULL);
        bt_bd_addr_t *sp_addr = (bt_bd_addr_t *)addr;
        bt_sink_srv_assert(aws_a2dp_dev);
        if(!sp_dev) {
            bt_sink_srv_memcpy(&(aws_a2dp_dev->dev_addr), bt_connection_manager_device_local_info_get_local_address(), sizeof(bt_bd_addr_t));
            aws_a2dp_dev->handle->dev_id = bt_sink_srv_music_convert_btaddr_to_devid(&(aws_a2dp_dev->dev_addr));
            aws_a2dp_dev->aws_hd = bt_sink_srv_music_get_aws_handle_by_addr(sp_addr);
            bt_sink_srv_report_id("[music_rho] no a2dp&avrcp connection, RHO AGENT-->CLIENT success.", 0);
            return;
        }
        while(aws_a2dp_dev) {
            bt_sink_srv_report_id("[music_rho] to free mce dev, aws_a2dp_dev:0x%08x", 1, aws_a2dp_dev);
            bt_sink_srv_a2dp_free_pseudo_handle(aws_a2dp_dev->handle);
            aws_a2dp_dev->handle = NULL;
            bt_sink_srv_music_reset_device(aws_a2dp_dev);
            aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_BY_LINK, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
        }

        bt_sink_srv_assert(sp_dev);

        if(ctx->run_dev) sp_dev = ctx->run_dev;
        bt_timer_ext_stop(BT_SINK_SRV_AVRCP_CONNECTION_TIMER_ID);
        sp_dev->handle->type = AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP;
        sp_dev->conn_bit = BT_SINK_SRV_MUSIC_AWS_CONN_BIT;
        sp_dev->a2dp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        sp_dev->avrcp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        sp_dev->volume_change_status = false;
        //to du replace handle
        bt_sink_srv_memcpy(&sp_dev->dev_addr, bt_connection_manager_device_local_info_get_local_address(), sizeof(bt_bd_addr_t));
        sp_dev->handle->dev_id = bt_sink_srv_music_convert_btaddr_to_devid(&sp_dev->dev_addr);
        uint8_t *addr_point = (uint8_t *)(&sp_dev->dev_addr);
        bt_sink_srv_report_id("[music_rho]addr:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", 6,
            addr_point[0],addr_point[1], addr_point[2], addr_point[3], addr_point[4],
            addr_point[5]);
        sp_dev->aws_hd = bt_sink_srv_music_get_aws_handle_by_addr(sp_addr);
        if(sp_dev->handle->flag & AUDIO_SRC_SRV_FLAG_WAITING) {
            bt_sink_srv_report_id("[music_rho]RHO happen, pseduo device is in waiting list", 0);
            audio_src_srv_del_waiting_list(sp_dev->handle);
        }

#ifdef  AIR_MULTI_POINT_ENABLE
        sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_SP,
                    NULL);

        while(sp_dev) {
            bt_sink_srv_report_id("[music_rho] to free mce dev, aws_a2dp_dev:0x%08x, aws_hd:0x%08x", 2, sp_dev, sp_dev->aws_hd);

            if(sp_dev->handle->flag & AUDIO_SRC_SRV_FLAG_WAITING) {
                bt_sink_srv_report_id("[music_rho]RHO happen, pseduo device is in waiting list", 0);
                audio_src_srv_del_waiting_list(sp_dev->handle);
            }

            bt_sink_srv_a2dp_free_pseudo_handle(sp_dev->handle);
            sp_dev->handle = NULL;
            bt_sink_srv_music_reset_device(sp_dev);
            sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_SP, NULL);
        }
#endif
        bt_sink_srv_report_id("[music_rho] RHO AGENT-->CLIENT success", 0);
    }
}


/*
bt_status_t
bt_sink_srv_music_role_handover_service_register_callbacks(bt_role_handover_module_type_t
type, bt_role_handover_callbacks_t callbacks)
{

}
*/
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
bt_status_t bt_sink_srv_music_role_handover_service_init(void)
{

    bt_role_handover_callbacks_t handover_callbacks = {
        bt_sink_srv_music_role_handover_service_allowed,
        bt_sink_srv_music_role_handover_service_get_length,
        bt_sink_srv_music_role_handover_service_get_data,
        bt_sink_srv_music_role_handover_service_update,
        bt_sink_srv_music_role_handover_service_status_callback,
    };

    bt_status_t ret = bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_SINK_MUSIC, &handover_callbacks);
    bt_sink_srv_report_id("[music_rho]init ret:0x%08x", 1, ret);
    return ret;
}

static bt_status_t bt_sink_srv_music_role_handover_service_allowed(const bt_bd_addr_t *addr)
{
    bool result = bt_sink_srv_music_role_handover_is_allowed(addr);

    if (result) {
        bt_timer_ext_t* avrcp_timer = bt_timer_ext_find(BT_SINK_SRV_AVRCP_CONNECTION_TIMER_ID);
        if(avrcp_timer) {
            return BT_STATUS_PENDING;
        }
    }
    return result ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

static uint8_t bt_sink_srv_music_role_handover_service_get_length(const bt_bd_addr_t *addr)
{
    return bt_sink_srv_music_role_handover_get_length(addr);
}

static bt_status_t bt_sink_srv_music_role_handover_service_get_data(const bt_bd_addr_t
*addr, void *data)
{
    bt_sink_srv_music_role_handover_get_data(addr, data);
    return BT_STATUS_SUCCESS;
}

static bt_status_t bt_sink_srv_music_role_handover_service_update(bt_role_handover_update_info_t *info)
{
    bt_sink_srv_music_role_handover((const bt_bd_addr_t *)(info->addr),
        (bt_aws_mce_role_handover_profile_t *)(info->profile_info), (uint8_t *)(info->data), info->is_active);
    return BT_STATUS_SUCCESS;
}

static void bt_sink_srv_music_role_handover_service_status_callback(const bt_bd_addr_t
    *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    bt_sink_srv_report_id("[music_rho]role:0x%02x, event:0x%02x, status:0x%08x", 3, role, event, status);
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    switch (event) {
        case BT_ROLE_HANDOVER_START_IND:
            ctx->rho_flag = 1;
            break;

        case BT_ROLE_HANDOVER_PREPARE_REQ_IND:{
            //stop timer
            bt_timer_ext_t *avrcp_timer = bt_timer_ext_find(BT_SINK_SRV_AVRCP_CONNECTION_TIMER_ID);
            if(avrcp_timer) {
                bt_timer_ext_stop(BT_SINK_SRV_AVRCP_CONNECTION_TIMER_ID);
            }
            //notify
            bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_SINK_MUSIC);
            break;
        }
        case BT_ROLE_HANDOVER_COMPLETE_IND: {
            ctx->rho_flag = 0;
            /* if partner->agent no a2dp device, then update aws handle */
            if (BT_AWS_MCE_ROLE_PARTNER == role
                && (NULL == bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_SP, NULL))) {
                bt_sink_srv_report_id("[music_rho] there is no a2dp connection, just update aws sink device!", 0);
                /* 1. Destory all old aws link context. */
                bt_sink_srv_music_device_t *aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
                while(aws_a2dp_dev) {
                    bt_sink_srv_report_id("[music_rho] old partner free mce dev, aws_a2dp_dev:0x%08x", 1, aws_a2dp_dev);
                    bt_sink_srv_a2dp_free_pseudo_handle(aws_a2dp_dev->handle);
                    aws_a2dp_dev->handle = NULL;
                    bt_sink_srv_music_reset_device(aws_a2dp_dev);
                    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_BY_LINK, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
                }

                /* 2. Create aws context to SP. */
                bt_bd_addr_t connected_address[3] = {{0}};
                uint32_t connected_number = bt_cm_get_connected_devices(
                    ~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), connected_address, 3);

                for (uint32_t i = 0; i < connected_number; i++) {
                    bt_sink_srv_music_device_t *dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, NULL);
                    bt_sink_srv_assert(dev && "Alloc free sink device fail!");

                    dev->aws_hd = bt_aws_mce_query_handle_by_address(BT_MODULE_AWS_MCE, (const bt_bd_addr_t *)&connected_address[i]);
                    bt_sink_srv_report_id("[music_rho] init aws sink device, aws_hd:0x%x", 1, dev->aws_hd);
                    bt_sink_srv_assert(dev->aws_hd && "aws_hd is 0!");
                    bt_sink_srv_memcpy(&(dev->dev_addr), &(connected_address[i]), sizeof(bt_bd_addr_t));
                    /* Init pse handle */
                    dev->handle = bt_sink_srv_a2dp_alloc_pseudo_handle();
                    bt_sink_srv_music_fill_audio_src_callback(dev->handle);
                    dev->handle->type = AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP;
                    dev->handle->priority = AUDIO_SRC_SRV_PRIORITY_NORMAL;
                    dev->handle->dev_id = bt_sink_srv_music_convert_btaddr_to_devid(&(dev->dev_addr));

                    BT_SINK_SRV_SET_FLAG(dev->conn_bit, BT_SINK_SRV_MUSIC_AWS_CONN_BIT);
                    BT_SINK_SRV_SET_FLAG(dev->op , BT_SINK_SRV_MUSIC_NO_STREAMING_STATE_UPDATE_FLAG);
                    bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_READY, NULL);
                }
            }
            break;
        }
        default:
            break;
    }
}
#endif
#endif
