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

#include "audio_source_control.h"
#include "stdio.h"
#include "syslog.h"
#include "audio_src_srv.h"
#include "string.h"
#include "assert.h"

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
#include "race_cmd_co_sys.h"
#endif
#if defined(MTK_AUDIO_TRANSMITTER_ENABLE)
#include "audio_transmitter_control.h"
#endif
#include "bt_sink_srv_ami.h"

log_create_module(AUDIO_SOURCE_CONTROL, PRINT_LEVEL_DEBUG);

#define AUDIO_SOURCE_CONTROL_USR_TABLE_NUM 6

#define TRANSMITTER_STA_NOT_INIT 0
#define TRANSMITTER_STA_IDLE     1
#define TRANSMITTER_STA_STARTED  2

typedef struct {
    audio_source_control_cfg_t usr_cfg;
    audio_src_srv_resource_manager_handle_t* resource_manager_handle;
    audio_source_control_cmd_dest_t cmd_src;
    #if defined(MTK_AUDIO_TRANSMITTER_ENABLE)
    audio_transmitter_id_t transmitter_id;
    uint8_t transmitter_state;
    #endif /* defined(MTK_AUDIO_TRANSMITTER_ENABLE) */
} audio_source_control_usr_context_t;

typedef struct {
    audio_source_control_usr_context_t usr_table[AUDIO_SOURCE_CONTROL_USR_TABLE_NUM];
} audio_source_control_context_t;

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)

#define AUDIO_SOURCE_CONTROL_INFO_ID_NONE                   0x00
#define AUDIO_SOURCE_CONTROL_INFO_ID_CMD                    0x01
#define AUDIO_SOURCE_CONTROL_INFO_ID_RESPONSE               0x02
#define AUDIO_SOURCE_CONTROL_INFO_ID_NOTIFY                 0x03

typedef struct {
    unsigned char                       usr_type;
} __attribute__((packed)) audio_source_control_co_sys_common_header_t;

typedef struct {
    audio_source_control_co_sys_common_header_t req_header;
    unsigned char                       cmd;
    unsigned char                       priority;
    unsigned char                       res_type;
} __attribute__((packed)) audio_source_control_co_sys_cmd_t;

typedef struct {
    audio_source_control_co_sys_common_header_t rsp_header;
    unsigned char                       rsp;
} __attribute__((packed)) audio_source_control_co_sys_response_t;

typedef struct {
    audio_source_control_co_sys_common_header_t notify_header;
    unsigned char                       notify_type;
    unsigned char                       notify;
} __attribute__((packed)) audio_source_control_co_sys_notify_t;

typedef struct {
    unsigned char                       info_type;
    union {
        audio_source_control_co_sys_cmd_t   req;
        audio_source_control_co_sys_notify_t    notify;
        audio_source_control_co_sys_response_t  rsp;
    } data;
} __attribute__((packed)) audio_source_control_co_sys_exchange_info_t;

#endif /* defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE) */

static audio_source_control_context_t s_ctx;

/*******************************************************************************
 * Dual-Chip init race cmd data handler
 *******************************************************************************/
#if AUDIO_SOURCE_CONTROL_TEST
extern void audio_source_control_test_init();
#endif /* AUDIO_SOURCE_CONTROL_TEST */

static void audio_source_control_judgement_callback_handler(struct _audio_src_srv_resource_manager_handle_t *current_handle, audio_src_srv_resource_manager_event_t event);

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
//#if defined(MTK_AUDIO_TRANSMITTER_ENABLE) depend on AIR_DUAL_CHIP

static void audio_source_control_send_notify(uint8_t usr, uint8_t type, uint8_t notify)
{
    audio_source_control_co_sys_exchange_info_t ex_info;
    ex_info.info_type = AUDIO_SOURCE_CONTROL_INFO_ID_NOTIFY;
    ex_info.data.notify.notify_header.usr_type = usr;
    ex_info.data.notify.notify_type = type;
    ex_info.data.notify.notify = notify;
    race_cosys_send_data(RACE_COSYS_MODULE_ID_AUDIO_SOURCE_CONTROL, false, (uint8_t *)&ex_info, sizeof(audio_source_control_co_sys_exchange_info_t));
}

static void audio_source_control_transmitter_callback(audio_transmitter_event_t event, void *data, void *user_data)
{
    audio_source_control_usr_context_t* usr_ctx = (audio_source_control_usr_context_t*)user_data;
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_transmitter_callback usr=%d, ev=%d", 2, usr_ctx->usr_cfg.usr, event);

    if (usr_ctx->cmd_src == AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL) {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_transmitter_callback to local usr=%d", 1, usr_ctx->usr_cfg.usr);
        if (usr_ctx->usr_cfg.request_notify != NULL) {
            usr_ctx->usr_cfg.request_notify(AUDIO_SOURCE_CONTROL_EVENT_TRANSMITTER, event, usr_ctx->usr_cfg.usr_data);
        } else {
            LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_transmitter_callback invalid callback", 0);
        }
    } else {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_transmitter_callback to remote usr=%d", 1, usr_ctx->usr_cfg.usr);
        audio_source_control_send_notify(usr_ctx->usr_cfg.usr, AUDIO_SOURCE_CONTROL_EVENT_TRANSMITTER, event);
    }
}

static bool audio_source_control_check_and_init_transmitter(audio_source_control_usr_context_t* usr_ctx) {
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "check_and_init_transmitter sta=%d", 1, usr_ctx->transmitter_state);
    if (usr_ctx->transmitter_state != TRANSMITTER_STA_NOT_INIT) {
        return true;
    }

    audio_transmitter_config_t config;

    config.scenario_config.reserved = 0;
    config.msg_handler = audio_source_control_transmitter_callback;
    config.user_data = (void *)usr_ctx;

    switch (usr_ctx->usr_cfg.usr) {
        case AUDIO_SOURCE_CONTROL_USR_LINE_IN:
            config.scenario_type = AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK;
            config.scenario_sub_id = AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_LINEIN_TO_I2S2;
            break;
        case AUDIO_SOURCE_CONTROL_USR_LINE_OUT:
            config.scenario_type = AUDIO_TRANSMITTER_WIRED_AUDIO;
            config.scenario_sub_id = AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE;
            break;
        case AUDIO_SOURCE_CONTROL_USR_AMA:
        case AUDIO_SOURCE_CONTROL_USR_GSOUND:
            config.scenario_type = AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK;
            config.scenario_sub_id = AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_ADC_TO_I2S0;
            break;
        default: {
            LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "check_and_init_transmitter no init param for usr=%d", 1, usr_ctx->usr_cfg.usr);
            return false;
        }
    }

    usr_ctx->transmitter_id = audio_transmitter_init(&config);

    if (usr_ctx->transmitter_id == -1) {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "check_and_init_transmitter fail to init transmitter for usr=%d", 1, usr_ctx->usr_cfg.usr);
        return false;
    }

    usr_ctx->transmitter_state = TRANSMITTER_STA_IDLE;
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "check_and_init_transmitter init transmitter for usr=%d, id=%d", 2,
            usr_ctx->usr_cfg.usr, usr_ctx->transmitter_id);
    return true;
}

static bool _check_and_gen_handle(audio_source_control_usr_context_t* usr_ctx, uint8_t res_type, uint8_t priority)
{
    if (usr_ctx->resource_manager_handle == NULL) {
        usr_ctx->resource_manager_handle = audio_src_srv_resource_manager_construct_handle(res_type, "audio_source_control_remote");
        if (usr_ctx->resource_manager_handle == NULL) {
            LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "_check_and_gen_handle get handle fail", 0);
            return false;
        }
        usr_ctx->resource_manager_handle->callback_func = audio_source_control_judgement_callback_handler;
        usr_ctx->resource_manager_handle->priority = priority;
    }
    return true;
}

static void audio_source_control_porcess_cmd(audio_source_control_usr_context_t* usr_ctx, audio_source_control_co_sys_cmd_t* cmd) {
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_porcess_cmd sta=%d, handle=0x%x", 2, 
                                        usr_ctx->transmitter_state, usr_ctx->resource_manager_handle);

    /* The flag will not be clear, it's means this usr must has only remote request. */
    usr_ctx->cmd_src = AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE;
    switch (cmd->cmd) {
        case AUDIO_SOURCE_CONTROL_CMD_REQUEST_RES:
            if (!_check_and_gen_handle(usr_ctx, cmd->res_type, cmd->priority)) {
                return;
            }
            audio_src_srv_resource_manager_take(usr_ctx->resource_manager_handle);
            break;
        case AUDIO_SOURCE_CONTROL_CMD_RELEASE_RES:
            if (usr_ctx->resource_manager_handle) {
                audio_src_srv_resource_manager_give(usr_ctx->resource_manager_handle);
            }
            break;
        case AUDIO_SOURCE_CONTROL_CMD_ADD_WAITTING_LIST:
            if (usr_ctx->resource_manager_handle) {
                audio_src_srv_resource_manager_add_waiting_list(usr_ctx->resource_manager_handle);
            }
            break;
        case AUDIO_SOURCE_CONTROL_CMD_DEL_WAITTING_LIST:
            if (usr_ctx->resource_manager_handle) {
                audio_src_srv_resource_manager_delete_waiting_list(usr_ctx->resource_manager_handle);
            }
            break;
        case AUDIO_SOURCE_CONTROL_CMD_START_TRANSMITTER: {
                if (!audio_source_control_check_and_init_transmitter(usr_ctx)) {
                    return;
                }
                audio_transmitter_status_t status = audio_transmitter_start(usr_ctx->transmitter_id);
                LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_transmitter_start sta=%d", 1, status);
                if (status != AUDIO_TRANSMITTER_STATUS_SUCCESS) {
                    audio_source_control_send_notify(usr_ctx->usr_cfg.usr, AUDIO_SOURCE_CONTROL_EVENT_CMD_FAIL, AUDIO_SOURCE_CONTROL_CMD_START_TRANSMITTER);
                }
            }
            break;
        case AUDIO_SOURCE_CONTROL_CMD_STOP_TRANSMITTER:
            if (usr_ctx->transmitter_state != TRANSMITTER_STA_NOT_INIT) {
                audio_transmitter_status_t status = audio_transmitter_stop(usr_ctx->transmitter_id);
                LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_transmitter_stop sta=%d", 1, status);
                if (status != AUDIO_TRANSMITTER_STATUS_SUCCESS) {
                    audio_source_control_send_notify(usr_ctx->usr_cfg.usr, AUDIO_SOURCE_CONTROL_EVENT_CMD_FAIL, AUDIO_SOURCE_CONTROL_CMD_STOP_TRANSMITTER);
                }
            } else {
                LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_transmitter_stop fail, due to state is not inited", 0);
                audio_source_control_send_notify(usr_ctx->usr_cfg.usr, AUDIO_SOURCE_CONTROL_EVENT_CMD_FAIL, AUDIO_SOURCE_CONTROL_CMD_STOP_TRANSMITTER);
            }
            break;
        case AUDIO_SOURCE_CONTROL_CMD_START_SIDETONE: {
                bt_sink_srv_am_result_t ami_result  = am_audio_side_tone_enable();
                LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "am_audio_side_tone_enable sta=%d", 1, ami_result);
                if (ami_result != AUD_EXECUTION_SUCCESS) {
                    audio_source_control_send_notify(usr_ctx->usr_cfg.usr, AUDIO_SOURCE_CONTROL_EVENT_CMD_FAIL, AUDIO_SOURCE_CONTROL_CMD_START_SIDETONE);
                }
            }
            break;
        case AUDIO_SOURCE_CONTROL_CMD_STOP_SIDETONE: {
                bt_sink_srv_am_result_t ami_result  = am_audio_side_tone_disable();
                LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "am_audio_side_tone_disable sta=%d", 1, ami_result);
                if (ami_result != AUD_EXECUTION_SUCCESS) {
                    audio_source_control_send_notify(usr_ctx->usr_cfg.usr, AUDIO_SOURCE_CONTROL_EVENT_CMD_FAIL, AUDIO_SOURCE_CONTROL_CMD_STOP_SIDETONE);
                }
            }
            break;
    }
}

static void audio_source_control_porcess_response(audio_source_control_usr_context_t* usr_ctx, audio_source_control_co_sys_response_t* response) {
    return;
}

static void audio_source_control_porcess_notify(audio_source_control_usr_context_t* usr_ctx, audio_source_control_co_sys_notify_t* notify) {
    if (usr_ctx->usr_cfg.request_notify != NULL) {
        usr_ctx->usr_cfg.request_notify(notify->notify_type, notify->notify, usr_ctx->usr_cfg.usr_data);
    } else {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_race_cosys_data_callback_handler, invalid callback for usr=%d", 1, usr_ctx->usr_cfg.usr);
    }
}

static void audio_source_control_race_cosys_data_callback_handler(bool from_irq, uint8_t *buff, uint32_t len)
{
    if ((buff == NULL) || (len == 0)) {
        LOG_MSGID_E(AUDIO_SOURCE_CONTROL, "audio_source_control_race_cosys_data_callback_handler, buffer or length error, buff : 0x%x, len : %d", 2, buff, len);
        return;
    }

    audio_source_control_co_sys_exchange_info_t *info = (audio_source_control_co_sys_exchange_info_t *)buff;
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_race_cosys_data_callback_handler, info type : 0x%02x", 1, info->info_type);

    if (info->data.req.req_header.usr_type == AUDIO_SOURCE_CONTROL_USR_ALL) {
        if (info->info_type == AUDIO_SOURCE_CONTROL_INFO_ID_NOTIFY) {
            audio_source_control_co_sys_notify_t* notify = &info->data.notify;
            LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_race_cosys_data_callback_handler, broadcast notify received", 0);
            LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_race_cosys_data_callback_handler, type=%d, ev=%d", 2,
                notify->notify_type, notify->notify);
            uint32_t idx = 0;
            for (idx = 0; idx < AUDIO_SOURCE_CONTROL_USR_TABLE_NUM; idx++) {
                if (s_ctx.usr_table[idx].usr_cfg.request_notify != NULL) {
                    s_ctx.usr_table[idx].usr_cfg.request_notify(notify->notify_type, notify->notify, s_ctx.usr_table[idx].usr_cfg.usr_data);
                }
            }
        }
        return;
    }

    audio_source_control_usr_context_t* usr_ctx = &s_ctx.usr_table[info->data.req.req_header.usr_type];
    usr_ctx->usr_cfg.usr = info->data.req.req_header.usr_type;
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_race_cosys_data_callback_handler, usr type=%d", 1, usr_ctx->usr_cfg.usr);
    switch (info->info_type) {
        case AUDIO_SOURCE_CONTROL_INFO_ID_CMD:
            audio_source_control_porcess_cmd(usr_ctx, &info->data.req);
            break;
        case AUDIO_SOURCE_CONTROL_INFO_ID_RESPONSE:
            audio_source_control_porcess_response(usr_ctx, &info->data.rsp);
            break;
        case AUDIO_SOURCE_CONTROL_INFO_ID_NOTIFY:
            audio_source_control_porcess_notify(usr_ctx, &info->data.notify);
            break;
    }
}
#else
static void audio_source_control_send_notify(uint8_t usr, uint8_t type, uint8_t notify)
{
    /* Do nothing */
}
#endif /* defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE) */

static void audio_source_control_side_tone_vendor_se_callback(vendor_se_event_t event, void *arg)
{
    uint32_t idx = 0;

    LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "vendor_se_callback, event=%d", 1, event);
    if (EVENT_SIDETONE_START != event && EVENT_SIDETONE_STOP!= event) {
        return;
    }

    for (idx = 0; idx < AUDIO_SOURCE_CONTROL_USR_TABLE_NUM; idx++) {
        if (s_ctx.usr_table[idx].usr_cfg.request_notify != NULL) {
            s_ctx.usr_table[idx].usr_cfg.request_notify(AUDIO_SOURCE_CONTROL_EVENT_VENDER_SE, event, s_ctx.usr_table[idx].usr_cfg.usr_data);
        }
    }

    audio_source_control_send_notify(AUDIO_SOURCE_CONTROL_USR_ALL, AUDIO_SOURCE_CONTROL_EVENT_VENDER_SE, event);
}

static void audio_source_control_side_tone_init()
{
    bt_sink_srv_am_result_t am_se_result = ami_register_vendor_se(ami_get_vendor_se_id(), audio_source_control_side_tone_vendor_se_callback);
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_side_tone_init, register vendor se callback result : 0x%02x", 1, am_se_result);
}

void audio_source_control_init()
{
    memset(&s_ctx, 0, sizeof(audio_source_control_context_t));
    audio_source_control_side_tone_init();
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
    bool ret = race_cosys_register_data_callback(RACE_COSYS_MODULE_ID_AUDIO_SOURCE_CONTROL, audio_source_control_race_cosys_data_callback_handler);
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_init, register cosys race handler result : %d", 1, ret);
#endif /* defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE) */
}

void audio_source_control_deinit()
{
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
    bool ret = race_cosys_register_data_callback(RACE_COSYS_MODULE_ID_AUDIO_SOURCE_CONTROL, NULL);
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_deinit, de-register cosys race handler result : %d", 1, ret);
#endif /* defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE) */
    return;
}

static void audio_source_control_judgement_callback_handler(struct _audio_src_srv_resource_manager_handle_t *current_handle, audio_src_srv_resource_manager_event_t event)
{
    uint32_t idx = 0;
    audio_source_control_usr_context_t* usr_ctx = NULL;

    /* find usr context */
    for (idx = 0; idx < AUDIO_SOURCE_CONTROL_USR_TABLE_NUM; idx++) {
        if (s_ctx.usr_table[idx].resource_manager_handle == current_handle) {
            usr_ctx = &s_ctx.usr_table[idx];
            break;
        }
    }

    if (idx == AUDIO_SOURCE_CONTROL_USR_TABLE_NUM) {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "judgement_callback_handler no usr match", 0);
        return;
    }

    if (usr_ctx->cmd_src == AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL) {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "judgement_callback_handler to local usr=%d", 1, idx);
        if (usr_ctx->usr_cfg.request_notify != NULL) {
            usr_ctx->usr_cfg.request_notify(AUDIO_SOURCE_CONTROL_EVENT_RES_CTRL, event, usr_ctx->usr_cfg.usr_data);
        } else {
            LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "judgement_callback_handler invalid callback", 0);
        }
    } else {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "judgement_callback_handler to remote usr=%d", 1, idx);
        audio_source_control_send_notify(idx, AUDIO_SOURCE_CONTROL_EVENT_RES_CTRL, event);
    }
}

void* audio_source_control_register(audio_source_control_cfg_t* cfg)
{
    audio_source_control_usr_context_t* usr_ctx = &s_ctx.usr_table[cfg->usr];
    memcpy(&usr_ctx->usr_cfg, cfg, sizeof(audio_source_control_cfg_t));
    usr_ctx->resource_manager_handle = audio_src_srv_resource_manager_construct_handle(cfg->res_type, (const char*)cfg->usr_name);
    if (usr_ctx->resource_manager_handle == NULL) {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_register get handle fail", 0);
        return NULL;
    }
    usr_ctx->resource_manager_handle->callback_func = audio_source_control_judgement_callback_handler;
    usr_ctx->resource_manager_handle->priority = cfg->req_priority;
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_register usr=%d, handle=0x%x", 2, cfg->usr, usr_ctx->resource_manager_handle);
    return usr_ctx;
}

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
static audio_source_control_result_t audio_source_control_cmd_remote(void* handle, audio_source_control_cmd_t cmd)
{
    audio_source_control_usr_context_t* usr_ctx = (audio_source_control_usr_context_t*) handle;
    audio_source_control_co_sys_exchange_info_t ex_info;

    ex_info.info_type = AUDIO_SOURCE_CONTROL_INFO_ID_CMD;
    ex_info.data.req.req_header.usr_type = usr_ctx->usr_cfg.usr;
    ex_info.data.req.cmd = cmd;
    ex_info.data.req.priority = usr_ctx->usr_cfg.req_priority;
    ex_info.data.req.res_type = usr_ctx->usr_cfg.res_type;
    race_cosys_send_data(RACE_COSYS_MODULE_ID_AUDIO_SOURCE_CONTROL, false, (uint8_t *)&ex_info, sizeof(audio_source_control_co_sys_exchange_info_t));

    return AUDIO_SOURCE_CONTROL_RESULT_OK;
}
#endif

static audio_source_control_result_t audio_source_control_cmd_local(void* handle, audio_source_control_cmd_t cmd)
{
    audio_source_control_usr_context_t* usr_ctx = (audio_source_control_usr_context_t*) handle;
    usr_ctx->cmd_src = AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL;

    switch (cmd) {
        case AUDIO_SOURCE_CONTROL_CMD_REQUEST_RES:
            audio_src_srv_resource_manager_take(usr_ctx->resource_manager_handle);
            break;
        case AUDIO_SOURCE_CONTROL_CMD_RELEASE_RES:
            audio_src_srv_resource_manager_give(usr_ctx->resource_manager_handle);
            break;
        case AUDIO_SOURCE_CONTROL_CMD_ADD_WAITTING_LIST:
            audio_src_srv_resource_manager_add_waiting_list(usr_ctx->resource_manager_handle);
            break;
        case AUDIO_SOURCE_CONTROL_CMD_DEL_WAITTING_LIST:
            audio_src_srv_resource_manager_delete_waiting_list(usr_ctx->resource_manager_handle);
            break;
        default: {
            LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_cmd_local, cmd=%d not processed", 1, cmd);
        }
    }

    return AUDIO_SOURCE_CONTROL_RESULT_OK;
}

audio_source_control_result_t audio_source_control_cmd(void *handle,
                    audio_source_control_cmd_t cmd,
                    audio_source_control_cmd_dest_t cmd_dest)
{
    audio_source_control_usr_context_t* usr_ctx = (audio_source_control_usr_context_t*) handle;
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL, "audio_source_control_cmd, USR=%d, cmd=%d, cmd_dest=%d", 3, usr_ctx->usr_cfg.usr, cmd, cmd_dest);
    if (cmd_dest == AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE) {
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
        return audio_source_control_cmd_remote(handle, cmd);
#endif
    } else if (cmd_dest == AUDIO_SOURCE_CONTROL_CMD_DEST_LOCAL) {
        return audio_source_control_cmd_local(handle, cmd);
    }

    return AUDIO_SOURCE_CONTROL_RESULT_OK;
}

