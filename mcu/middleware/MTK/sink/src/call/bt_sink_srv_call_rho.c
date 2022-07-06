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

#include "bt_gap.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_call.h"
#include "bt_sink_srv_hf.h"
#include "bt_sink_srv_hsp.h"
#include "bt_sink_srv_aws_mce_call.h"
#include "bt_connection_manager_internal.h"
#include "bt_sink_srv_call_pseudo_dev.h"
#include "bt_sink_srv_call_pseudo_dev_mgr.h"

#ifdef MTK_BT_HSP_ENABLE
#include "bt_sink_srv_hsp.h"
#endif /*MTK_BT_HSP_ENABLE*/

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "bt_role_handover.h"
#endif /*SUPPORT_ROLE_HANDOVER_SERVICE*/

#define BT_SINK_SRV_CALL_RHO_FLAG_HIGHLIGHT         (0x01U)
#define BT_SINK_SRV_CALL_RHO_FLAG_WAITING_LIST      (0x02U)
#define BT_SINK_SRV_CALL_RHO_FLAG_MSBC_CODEC        (0x04U)
#define BT_SINK_SRV_CALL_RHO_FLAG_LAST_DEVICE       (0x08U)
typedef uint8_t bt_sink_srv_call_rho_flag_t;

BT_PACKED(
typedef struct {
    /*HFP used*/
    bt_hfp_ag_feature_t ag_featues;
    bt_hfp_ag_hold_feature_t ag_chld_feature;
    bt_sink_srv_hf_call_state_t call_state;
    bt_sink_srv_hf_flag_t flag;
    /*HSP used*/
    bt_sink_srv_sco_connection_state_t sco_state;
#ifdef AIR_MULTI_POINT_ENABLE
    uint8_t speaker_volume;
    bt_sink_srv_call_rho_flag_t rho_flag;
#endif /* AIR_MULTI_POINT_ENABLE */
}) bt_sink_srv_call_rho_content_t;

extern bt_sink_srv_hf_context_t g_sink_srv_hf_context[BT_SINK_SRV_HF_LINK_NUM];
extern bt_sink_srv_hsp_context_t g_sink_srv_hsp_context[BT_SINK_SRV_HSP_LINK_NUM];
extern bt_sink_srv_call_pseudo_dev_t bt_sink_srv_call_pseudo_dev[BT_SINK_SRV_CALL_PSD_NUM];
extern bt_sink_srv_hf_context_t *g_sink_srv_hf_last_device_p;
extern bt_sink_srv_hf_context_t *g_sink_srv_hf_hightlight_p;

extern void bt_sink_srv_hf_set_hsp_flag(bool enable);
extern void bt_sink_srv_aws_mce_call_pseudo_dev_callback(bt_sink_srv_call_pseudo_dev_event_t event_id, void *device, void *params);
extern void bt_sink_srv_hf_reset_highlight_device(void);
extern bt_sink_srv_hf_context_t *bt_sink_srv_hf_get_highlight_device(void);
extern void bt_sink_srv_hf_set_highlight_device(bt_sink_srv_hf_context_t *device);
extern void bt_sink_srv_hf_pseudo_dev_callback(bt_sink_srv_call_pseudo_dev_event_t event_id, void *device, void *params);
extern void bt_sink_srv_call_psd_add_waiting_list(bt_sink_srv_call_pseudo_dev_t *device);
extern void bt_sink_srv_call_psd_del_waiting_list(bt_sink_srv_call_pseudo_dev_t *device);

#ifdef MTK_BT_HSP_ENABLE
extern void bt_sink_srv_hsp_pseudo_dev_callback(bt_sink_srv_call_pseudo_dev_event_t event_id, void *device, void *params);
extern bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_get_highlight_device(void);
extern void bt_sink_srv_hsp_set_highlight_device(bt_sink_srv_hsp_context_t *device);
#endif /*MTK_BT_HSP_ENABLE*/
bt_sink_srv_call_rho_content_t g_bt_sink_srv_call_rho_content = {0};

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
static bt_status_t bt_sink_srv_call_role_handover_is_allowed(const bt_bd_addr_t* addr);
static uint8_t bt_sink_srv_call_role_handover_get_length(const bt_bd_addr_t* addr);
static bt_status_t bt_sink_srv_call_role_handover_get_data(const bt_bd_addr_t* addr, void *data);
static bt_status_t bt_sink_srv_call_role_handover_update(bt_role_handover_update_info_t *info);
static void bt_sink_srv_call_role_handover_status_callback(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status);

bt_role_handover_callbacks_t bt_sink_srv_call_call_role_cb = {
    .allowed_cb = bt_sink_srv_call_role_handover_is_allowed,
    .get_len_cb = bt_sink_srv_call_role_handover_get_length,
    .get_data_cb = bt_sink_srv_call_role_handover_get_data,
    .update_cb = bt_sink_srv_call_role_handover_update,
    .status_cb = bt_sink_srv_call_role_handover_status_callback
};

void bt_sink_srv_call_role_handover_init(void)
{
    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_SINK_CALL, &bt_sink_srv_call_call_role_cb);
}

bt_status_t bt_sink_srv_call_role_handover_is_allowed(const bt_bd_addr_t* addr)
{
    bt_status_t result = BT_STATUS_SUCCESS;

    if (!bt_sink_srv_call_psd_is_all_in_steady_state()) {
        result = BT_STATUS_FAIL;
    }

    bt_sink_srv_report_id("[CALL][AWS_MCE][RHO]is allowed, result:0x%x", 1, result);
    return result;
}

uint8_t bt_sink_srv_call_role_handover_get_length(const bt_bd_addr_t* addr)
{
    uint8_t length = 0;
    bt_sink_srv_hf_context_t *hfp_context = bt_sink_srv_hf_get_context_by_address((bt_bd_addr_t*)addr);
#ifdef MTK_BT_HSP_ENABLE
    bt_sink_srv_hsp_context_t *hsp_context = bt_sink_srv_hsp_get_context_by_address((bt_bd_addr_t*)addr);
#endif /*MTK_BT_HSP_ENABLE*/

    if ((hfp_context && bt_sink_srv_call_psd_is_ready(hfp_context->device))
        #ifdef MTK_BT_HSP_ENABLE
        || (hsp_context && bt_sink_srv_call_psd_is_ready(hsp_context->device))
        #endif /*MTK_BT_HSP_ENABLE*/
        ) {
        length = (uint8_t)(sizeof(bt_sink_srv_call_rho_content_t) & 0x000000FF);
    }

    bt_sink_srv_report_id("[CALL][AWS_MCE][RHO]get length, length:%d", 1, length);
    return length;
}

bt_status_t bt_sink_srv_call_role_handover_get_data(const bt_bd_addr_t* addr, void *data)
{
    bt_sink_srv_hf_context_t *hf_context = bt_sink_srv_hf_get_context_by_address((bt_bd_addr_t* )addr);
#ifdef MTK_BT_HSP_ENABLE
    bt_sink_srv_hsp_context_t *hs_context = bt_sink_srv_hsp_get_context_by_address((bt_bd_addr_t* )addr);
#endif /*MTK_BT_HSP_ENABLE*/

    if (hf_context) {
        g_bt_sink_srv_call_rho_content.ag_featues = hf_context->link.ag_featues;
        g_bt_sink_srv_call_rho_content.ag_chld_feature = hf_context->link.ag_chld_feature;
        g_bt_sink_srv_call_rho_content.call_state = hf_context->link.call_state;
        g_bt_sink_srv_call_rho_content.flag = hf_context->link.flag;
    #ifdef AIR_MULTI_POINT_ENABLE
        bt_sink_srv_call_pseudo_dev_t *device = (bt_sink_srv_call_pseudo_dev_t *)hf_context->device;
        g_bt_sink_srv_call_rho_content.speaker_volume = bt_sink_srv_call_psd_get_speaker_volume(hf_context->device);
        if (hf_context == bt_sink_srv_hf_get_highlight_device()) {
            g_bt_sink_srv_call_rho_content.rho_flag |= BT_SINK_SRV_CALL_RHO_FLAG_HIGHLIGHT;
        } else {
            g_bt_sink_srv_call_rho_content.rho_flag &= ~BT_SINK_SRV_CALL_RHO_FLAG_HIGHLIGHT;
        }
        if ((device->flag & BT_SINK_SRV_CALL_PSD_FLAG_ADD_WAIT_LIST) != 0) {
            g_bt_sink_srv_call_rho_content.rho_flag |= BT_SINK_SRV_CALL_RHO_FLAG_WAITING_LIST;
        } else {
            g_bt_sink_srv_call_rho_content.rho_flag &= ~BT_SINK_SRV_CALL_RHO_FLAG_WAITING_LIST;
        }
        if ((hf_context->link.flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) != 0) {
            if (bt_sink_srv_call_psd_get_codec_type(hf_context->device) == BT_HFP_CODEC_TYPE_MSBC) {
                g_bt_sink_srv_call_rho_content.rho_flag |= BT_SINK_SRV_CALL_RHO_FLAG_MSBC_CODEC;
            } else {
                g_bt_sink_srv_call_rho_content.rho_flag &= ~BT_SINK_SRV_CALL_RHO_FLAG_MSBC_CODEC;
            }
        }
        if (hf_context == g_sink_srv_hf_last_device_p) {
            g_bt_sink_srv_call_rho_content.rho_flag |= BT_SINK_SRV_CALL_RHO_FLAG_LAST_DEVICE;
        } else {
            g_bt_sink_srv_call_rho_content.rho_flag &= ~BT_SINK_SRV_CALL_RHO_FLAG_LAST_DEVICE;
        }
    #endif /* AIR_MULTI_POINT_ENABLE */
        bt_sink_srv_report_id("[CALL][AWS_MCE][RHO]get data, HF data(0x%x, 0x%x, 0x%x, 0x%x)", 4,
            g_bt_sink_srv_call_rho_content.ag_featues, g_bt_sink_srv_call_rho_content.ag_chld_feature,
            g_bt_sink_srv_call_rho_content.call_state, g_bt_sink_srv_call_rho_content.flag);
    }

#ifdef MTK_BT_HSP_ENABLE
    if (hs_context) {
        g_bt_sink_srv_call_rho_content.sco_state = hs_context->sco_state;
    #ifdef AIR_MULTI_POINT_ENABLE
        g_bt_sink_srv_call_rho_content.speaker_volume = bt_sink_srv_call_psd_get_speaker_volume(hs_context->device);
        if (hs_context == bt_sink_srv_hsp_get_highlight_device()) {
            g_bt_sink_srv_call_rho_content.rho_flag |= BT_SINK_SRV_CALL_RHO_FLAG_HIGHLIGHT;
        } else {
            g_bt_sink_srv_call_rho_content.rho_flag &= ~BT_SINK_SRV_CALL_RHO_FLAG_HIGHLIGHT;
        }
    #endif /* AIR_MULTI_POINT_ENABLE */
        bt_sink_srv_report_id("[CALL][AWS_MCE][RHO]get data, HSP data(0x%x)", 1, g_bt_sink_srv_call_rho_content.sco_state);
    }
#endif /*MTK_BT_HSP_ENABLE*/

    bt_sink_srv_memcpy(data, (void *)&g_bt_sink_srv_call_rho_content, sizeof(bt_sink_srv_call_rho_content_t));
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_sink_srv_call_role_handover_update(bt_role_handover_update_info_t *info)
{
    bt_sink_srv_assert(info != NULL);
    bt_sink_srv_aws_mce_call_context_t *aws_context = bt_sink_srv_aws_mce_call_get_context_by_address(info->addr);

    if (info->is_active) {
        bt_sink_srv_assert((aws_context != NULL) && "AWS connection not found in active link");
    }

    if (info->role == BT_AWS_MCE_ROLE_PARTNER) {
        /* Previous Partner, create hfp context and destory aws call context. */
        bt_sink_srv_report_id("[CALL][AWS_MCE][RHO]update, Partner->Agent.", 0);
        bt_sink_srv_report_id("[CALL][AWS_MCE][RHO]update, hfp_handle:0x%x hsp_handle:0x%x", 2,
            info->profile_info->hfp_handle, info->profile_info->hsp_handle);

        bt_sink_srv_assert(info->profile_info != NULL);
        bt_sink_srv_call_rho_content_t *rho_context = (bt_sink_srv_call_rho_content_t *)info->data;

        if (info->profile_info->hfp_handle != 0) {
            /* 1. Update hfp context. */
            bt_sink_srv_hf_context_t *hf_context = bt_sink_srv_hf_alloc_free_context((bt_bd_addr_t *)info->addr);
            bt_sink_srv_assert(hf_context);

            /* 1.1. Update device. */
            if ((aws_context != NULL) && (aws_context->device != NULL)) {
                hf_context->device = aws_context->device;
                bt_sink_srv_call_psd_set_device_id(hf_context->device, (bt_bd_addr_t *)info->addr);
                bt_sink_srv_call_psd_reset_user_callback(hf_context->device, bt_sink_srv_hf_pseudo_dev_callback);
            #ifdef AIR_MULTI_POINT_ENABLE
                bt_sink_srv_call_psd_set_speaker_volume(hf_context->device, rho_context->speaker_volume);
            #endif /* AIR_MULTI_POINT_ENABLE */
            } else {
                hf_context->device = bt_sink_srv_call_psd_alloc_device((bt_bd_addr_t *)info->addr, bt_sink_srv_hf_pseudo_dev_callback);
                bt_sink_srv_assert(hf_context->device);
                /* Switch connected state if not in active link. */
                bt_sink_srv_call_psd_state_event_notify(hf_context->device, BT_SINK_SRV_CALL_EVENT_CONNECT_LINK_REQ_IND, NULL);
                bt_sink_srv_call_psd_state_event_notify(hf_context->device, BT_SINK_SRV_CALL_EVENT_LINK_CONNECTED, NULL);
            #ifdef AIR_MULTI_POINT_ENABLE
                bt_sink_srv_call_psd_set_speaker_volume(hf_context->device, rho_context->speaker_volume);
            #endif /* AIR_MULTI_POINT_ENABLE */
            }

            if ((aws_context != NULL) && (aws_context->device != NULL)) {
                if (((rho_context->flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE) != 0)
                    && (bt_sink_srv_call_psd_get_codec_type(aws_context->device) == BT_HFP_CODEC_TYPE_NONE)) {
                    bt_sink_srv_report_id("[CALL][AWS_MCE][RHO]update, eSCO state not match!", 0);
                    rho_context->flag &= ~BT_SINK_SRV_HF_FLAG_SCO_ACTIVE;
                }
            }

            #ifdef AIR_MULTI_POINT_ENABLE
            if ((rho_context->rho_flag & BT_SINK_SRV_CALL_RHO_FLAG_WAITING_LIST) != 0) {
                bt_sink_srv_call_psd_add_waiting_list(hf_context->device);
            }
            if ((rho_context->flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) != 0) {
                if ((rho_context->rho_flag & BT_SINK_SRV_CALL_RHO_FLAG_MSBC_CODEC) != 0) {
                    bt_sink_srv_call_psd_set_codec_type(hf_context->device, BT_HFP_CODEC_TYPE_MSBC);
                } else {
                    bt_sink_srv_call_psd_set_codec_type(hf_context->device, BT_HFP_CODEC_TYPE_CVSD);
                }
            }
            #endif /* AIR_MULTI_POINT_ENABLE */

            /* 1.2. Update link context. */
            hf_context->is_used = true;
            hf_context->link.handle = info->profile_info->hfp_handle;
            hf_context->link.ag_featues = rho_context->ag_featues;
            hf_context->link.ag_chld_feature = rho_context->ag_chld_feature;
            hf_context->link.call_state = rho_context->call_state;
            hf_context->link.flag = rho_context->flag;
            bt_sink_srv_memcpy(&(hf_context->link.address), info->addr, sizeof(bt_bd_addr_t));
            bt_sink_srv_report_id("[CALL][AWS_MCE][RHO]update, HF data(0x%x, 0x%x, 0x%x, 0x%x)", 4,
                hf_context->link.ag_featues, hf_context->link.ag_chld_feature,
                hf_context->link.call_state, hf_context->link.flag);

        #ifdef AIR_MULTI_POINT_ENABLE
            if ((rho_context->rho_flag & BT_SINK_SRV_CALL_RHO_FLAG_HIGHLIGHT) != 0) {
                g_sink_srv_hf_hightlight_p = hf_context;
            }
            if ((rho_context->rho_flag & BT_SINK_SRV_CALL_RHO_FLAG_LAST_DEVICE) != 0) {
                bt_sink_srv_hf_update_last_context(hf_context, true);
            }
        #else
            bt_sink_srv_hf_set_highlight_device(hf_context);
            bt_sink_srv_hf_update_last_context(hf_context, true);
        #endif /* AIR_MULTI_POINT_ENABLE */

            /* 1.3. Store volume to the nvdm. */
            bt_sink_srv_hf_stored_data_t stored_data = {0};
            stored_data.speaker_volume = (uint8_t)bt_sink_srv_call_psd_get_speaker_volume(hf_context->device) | BT_SINK_SRV_HF_VOLUME_MASK;
            bt_sink_srv_hf_set_nvdm_data((bt_bd_addr_t *)info->addr, &stored_data, sizeof(stored_data));
        } else if (info->profile_info->hsp_handle != 0) {
        #ifdef MTK_BT_HSP_ENABLE
            /* 2. Update hsp context. */
            bt_sink_srv_hsp_context_t *hsp_context = bt_sink_srv_hsp_alloc_free_context((bt_bd_addr_t *)info->addr);
            bt_sink_srv_assert(hsp_context);

            /* 2.1. Update device. */
            if ((aws_context != NULL) && (aws_context->device != NULL)) {
                hsp_context->device = aws_context->device;
                bt_sink_srv_call_psd_set_device_id(hsp_context->device, (bt_bd_addr_t *)info->addr);
                bt_sink_srv_call_psd_reset_user_callback(hsp_context->device, bt_sink_srv_hsp_pseudo_dev_callback);
            #ifdef AIR_MULTI_POINT_ENABLE
                bt_sink_srv_call_psd_set_speaker_volume(hsp_context->device, rho_context->speaker_volume);
            #endif /* AIR_MULTI_POINT_ENABLE */
            } else {
                hsp_context->device = bt_sink_srv_call_psd_alloc_device((bt_bd_addr_t *)info->addr, bt_sink_srv_hsp_pseudo_dev_callback);
                bt_sink_srv_assert(hsp_context->device);
                /* Switch connected state if not in active link. */
                bt_sink_srv_call_psd_state_event_notify(hsp_context->device, BT_SINK_SRV_CALL_EVENT_CONNECT_LINK_REQ_IND, NULL);
                bt_sink_srv_call_psd_state_event_notify(hsp_context->device, BT_SINK_SRV_CALL_EVENT_LINK_CONNECTED, NULL);
            #ifdef AIR_MULTI_POINT_ENABLE
                bt_sink_srv_call_psd_set_speaker_volume(hsp_context->device, rho_context->speaker_volume);
            #endif /* AIR_MULTI_POINT_ENABLE */
            }
            
            #ifdef AIR_MULTI_POINT_ENABLE
            if (rho_context->sco_state == BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED) {
                bt_sink_srv_call_psd_set_codec_type(hsp_context->device, BT_HFP_CODEC_TYPE_CVSD);
            }
            #endif /* AIR_MULTI_POINT_ENABLE */

            /* 2.2. Update link context. */
            hsp_context->is_used = true;
            hsp_context->handle = info->profile_info->hsp_handle;
            hsp_context->sco_state = rho_context->sco_state;
            bt_sink_srv_memcpy(&(hsp_context->address), info->addr, sizeof(bt_bd_addr_t));
            bt_sink_srv_report_id("[CALL][AWS_MCE][RHO]update, HSP data(0x%x)", 1, hsp_context->sco_state);

        #ifdef AIR_MULTI_POINT_ENABLE
            if ((rho_context->rho_flag & BT_SINK_SRV_CALL_RHO_FLAG_HIGHLIGHT) != 0) {
                bt_sink_srv_hsp_set_highlight_device(hsp_context);
            }
        #else
            bt_sink_srv_hsp_set_highlight_device(hsp_context);
        #endif /* AIR_MULTI_POINT_ENABLE */

            /* 2.3. Store volume to the nvdm. */
            bt_sink_srv_hf_stored_data_t stored_data = {0};
            stored_data.speaker_volume = (uint8_t)bt_sink_srv_call_psd_get_speaker_volume(hsp_context->device) | BT_SINK_SRV_HF_VOLUME_MASK;
            bt_sink_srv_hf_set_nvdm_data((bt_bd_addr_t *)info->addr, &stored_data, sizeof(stored_data));
        #endif /* MTK_BT_HSP_ENABLE */
        } else {
            bt_sink_srv_assert((info->data == NULL) && "HFP or HSP connection not found");
        }

        /* 3. Delete aws context pseudo device. */
        if (aws_context != NULL) {
            aws_context->device = NULL;
        }
    } else if (info->role == BT_AWS_MCE_ROLE_AGENT) {
        /* Previous agent, create aws call context and destory hfp context. */
        bt_sink_srv_report_id("[CALL][AWS_MCE][RHO]update, Agent->Partner.", 0);

        void *current_device = NULL;
        bt_bd_addr_t *agent_addr = bt_connection_manager_device_local_info_get_local_address();
        bt_sink_srv_hf_context_t *hf_context = bt_sink_srv_hf_get_context_by_address((bt_bd_addr_t *)info->addr);
    #ifdef MTK_BT_HSP_ENABLE
        bt_sink_srv_hsp_context_t *hsp_context = bt_sink_srv_hsp_get_context_by_address((bt_bd_addr_t *)info->addr);
    #ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
        bt_timer_ext_t* timer_ext = bt_timer_ext_find(BT_SINK_SRV_TIMER_ID_HS_WAIT_RING_IND);

        if (timer_ext) {
            bt_sink_srv_report_id("[CALL][AWS_MCE][RHO]Stop hsp timer when become patner.", 0);
            bt_timer_ext_stop(BT_SINK_SRV_TIMER_ID_HS_WAIT_RING_IND);
        }
    #endif/* MTK_BT_TIMER_EXTERNAL_ENABLE */
    #endif/* MTK_BT_HSP_ENABLE */

        /* 1. Update aws call context. */
        /* Only hfp has call state, so we can get call state from hfp always even if hfp is not connected. */
        if (hf_context) {
            aws_context->call_info.call_state = bt_sink_srv_aws_mce_call_transfer_hf_call_state(hf_context->link.call_state);
        } else {
            aws_context->call_info.call_state = BT_SINK_SRV_AWS_MCE_CALL_STATE_IDLE;
        }

        if(hf_context && hf_context->device) {
            current_device = hf_context->device;
    #ifdef MTK_BT_HSP_ENABLE
        } else if (hsp_context && hsp_context->device) {
            current_device = hsp_context->device;
    #endif /*MTK_BT_HSP_ENABLE*/
        } else {
            bt_sink_srv_report_id("[CALL][AWS_MCE][RHO]update, HFP or HSP connection not found.", 0);
        }

        if (current_device) {
            aws_context->device = current_device;
            bt_sink_srv_call_psd_set_device_id(aws_context->device, agent_addr);
            bt_sink_srv_call_psd_reset_user_callback(aws_context->device, bt_sink_srv_aws_mce_call_pseudo_dev_callback);

            aws_context->call_info.volume = (uint8_t)bt_sink_srv_call_psd_get_speaker_volume(current_device);
            bt_sink_srv_report_id("[CALL][AWS_MCE][RHO]update, Call info(0x%x, 0x%x, 0x%x, 0x%x)", 4,
                aws_context->call_info.call_state, aws_context->call_info.sco_state,aws_context->call_info.volume, aws_context->call_info.is_ring);
        } else {
            /* RHO case, if hfp or hsp is not connected, aws call context need alloc a audio_src. */
            aws_context->device = bt_sink_srv_call_psd_alloc_device(agent_addr, bt_sink_srv_aws_mce_call_pseudo_dev_callback);
            bt_sink_srv_assert(aws_context->device);

            bt_sink_srv_call_psd_state_event_notify(aws_context->device, BT_SINK_SRV_CALL_EVENT_CONNECT_LINK_REQ_IND, NULL);
            bt_sink_srv_call_psd_state_event_notify(aws_context->device, BT_SINK_SRV_CALL_EVENT_LINK_CONNECTED, NULL);
            bt_sink_srv_call_psd_set_speaker_volume(aws_context->device, (bt_sink_srv_call_audio_volume_t)aws_context->call_info.volume);
        }

        /* 2. Delete unused pseudo device. */
        uint32_t i = 0;
        for (i = 0; i < BT_SINK_SRV_CALL_PSD_NUM; i++) {
            bt_sink_srv_call_pseudo_dev_t *dev = &bt_sink_srv_call_pseudo_dev[i];
            if ((dev->user_cb != NULL) &&
                (dev->user_cb != bt_sink_srv_aws_mce_call_pseudo_dev_callback)) {
                dev->audio_src->dev_id = 0;
                bt_sink_srv_call_psd_del_waiting_list(dev);
                bt_sink_srv_call_psd_free_device(dev);
            }
        }

        /* 3. Delete HFP context. */
        bt_sink_srv_hf_reset_highlight_device();
        bt_sink_srv_hf_update_last_context(NULL, true);
        bt_sink_srv_memset(g_sink_srv_hf_context, 0, sizeof(g_sink_srv_hf_context));

        /* 4. Delete HSP context. */
    #ifdef MTK_BT_HSP_ENABLE
        bt_sink_srv_hsp_set_highlight_device(NULL);
        bt_sink_srv_memset(g_sink_srv_hsp_context, 0, sizeof(g_sink_srv_hsp_context));
    #endif
    } else {
        bt_sink_srv_assert(0 && "Wrong AWS role!");
    }

    return BT_STATUS_SUCCESS;
}

void bt_sink_srv_call_role_handover_status_callback(
    const bt_bd_addr_t *addr, bt_aws_mce_role_t role,
    bt_role_handover_event_t event, bt_status_t status)
{
    bt_sink_srv_report_id("[CALL][AWS_MCE][RHO]status callback, role:0x%x, event:0x%x, status:0x%x", 3,
                         role, event, status);
    switch (event) {
        case BT_ROLE_HANDOVER_START_IND:
        {

        }
        break;

        case BT_ROLE_HANDOVER_PREPARE_REQ_IND:
        {

        }
        break;

        case BT_ROLE_HANDOVER_COMPLETE_IND:
        {
        #ifdef BT_SINK_SRV_AWS_MCE_CALL_IN_MULTIPOINT
            if (status == BT_STATUS_SUCCESS) {
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    bt_sink_srv_aws_mce_call_update_agent();
                } else if (role == BT_AWS_MCE_ROLE_PARTNER) {
                    uint32_t i = 0;

                    /* Update AWS MCE CALL context. */
                    bt_sink_srv_aws_mce_call_update_partner();

                    /* Check if unused pseudo device exist, then destory. */
                    for (i = 0; i < BT_SINK_SRV_CALL_PSD_NUM; i++) {
                        bt_sink_srv_call_pseudo_dev_t *dev = &bt_sink_srv_call_pseudo_dev[i];
                        if ((dev->user_cb != NULL) &&
                            (dev->user_cb == bt_sink_srv_aws_mce_call_pseudo_dev_callback)) {
                            dev->audio_src->dev_id = 0;
                            bt_sink_srv_call_psd_del_waiting_list(dev);
                            bt_sink_srv_call_psd_free_device(dev);
                        }
                    }

                    /* Disable Page Scan when 2 SCO connected in new Agent side. */
                #if defined(MTK_BT_CM_SUPPORT) && defined(AIR_MULTI_POINT_ENABLE)
                    if (bt_sink_srv_hf_get_connected_sco_count() > 1) {
                        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_DISABLE);
                    }
                #endif
                } else {
                    bt_sink_srv_assert(0 && "Wrong AWS role!");
                }
            }
        #endif /* BT_SINK_SRV_AWS_MCE_CALL_IN_MULTIPOINT */
        }
        break;

        default:
        break;
    }
    return;
}
#endif /*SUPPORT_ROLE_HANDOVER_SERVICE*/
