/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "bt_device_manager.h"

#include "bt_sink_srv.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_common.h"

#include "bt_connection_manager_internal.h"
#include "bt_sink_srv_call.h"
#include "bt_sink_srv_avrcp.h"
#include "bt_sink_srv_a2dp.h"
#include "bt_sink_srv_pbapc.h"
#include "bt_sink_srv_aws_mce.h"
#include "bt_sink_srv_aws_mce_a2dp.h"

#ifdef AIR_BT_CODEC_BLE_ENABLED
#include "bt_sink_srv_le.h"
#endif
#ifdef BT_SINK_DUAL_ANT_ENABLE
#include "bt_sink_srv_dual_ant.h"
#endif


bt_sink_srv_device_info_t* default_bt_sink_srv_is_idle_callback()
{
    return NULL;
}

#if _MSC_VER >= 1500
    #pragma comment(linker, "/alternatename:_bqb_avrcp_io_callback=_default_bqb_avrcp_io_callback")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
    #pragma weak bt_sink_srv_is_idle_callback = default_bt_sink_srv_is_idle_callback
#else
    #error "Unsupported Platform"
#endif

extern void bt_sink_srv_atci_init(void);
extern bt_status_t bt_sink_srv_iap2_action_handler(bt_sink_srv_action_t action, void *parameter);

 const static bt_sink_srv_hf_custom_command_xapl_params_t bt_sink_srv_default_apple_specific_params = {
    .vendor_infomation = "MTK-HB-0400",
    .features = BT_SINK_SRV_HF_CUSTOM_FEATURE_NONE
};

const bt_sink_srv_hf_custom_command_xapl_params_t *default_bt_sink_srv_get_hfp_custom_command_xapl_params(void)
{
    return &bt_sink_srv_default_apple_specific_params;
}

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_hfp_custom_command_xapl_params=_default_bt_sink_srv_get_hfp_custom_command_xapl_params")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_sink_srv_get_hfp_custom_command_xapl_params = default_bt_sink_srv_get_hfp_custom_command_xapl_params
#else
#error "Unsupported Platform"
#endif

#if defined (AIR_LE_AUDIO_ENABLE)
static bt_sink_srv_type_t bt_sink_srv_get_current_type();
#endif

const static bt_sink_srv_action_callback_table_t bt_sink_srv_action_callback_table[] = {
    {
        SINK_MODULE_MASK_COMMON | SINK_MODULE_MASK_HFP | SINK_MODULE_MASK_HSP,
        bt_sink_srv_call_action_handler
    },
    {
        SINK_MODULE_MASK_COMMON | SINK_MODULE_MASK_A2DP | SINK_MODULE_MASK_AVRCP,
        bt_sink_srv_music_a2dp_action_handler
    },
#ifdef MTK_AWS_MCE_ENABLE
    {
        SINK_MODULE_MASK_COMMON | SINK_MODULE_MASK_A2DP | SINK_MODULE_MASK_AVRCP,
        bt_sink_srv_music_aws_a2dp_action_handler
    },
#endif/*MTK_AWS_MCE_ENABLE*/
    {
        SINK_MODULE_MASK_COMMON | SINK_MODULE_MASK_AVRCP,
        bt_sink_srv_music_avrcp_action_handler
    },
    {
        SINK_MODULE_MASK_COMMON | SINK_MODULE_MASK_PBAPC,
        bt_sink_srv_pbapc_action_handler
    },
#ifdef MTK_AWS_MCE_ENABLE
#ifndef MTK_BT_CM_SUPPORT
    {
        SINK_MODULE_MASK_COMMON | SINK_MODULE_MASK_AWS_MCE,
        bt_sink_srv_aws_mce_action_handler
    },
#endif
#endif/*MTK_AWS_MCE_ENABLE*/
#ifdef MTK_IAP2_PROFILE_ENABLE
#ifndef MTK_IAP2_VIA_MUX_ENABLE
    {
        SINK_MODULE_MASK_COMMON,
        bt_sink_srv_iap2_action_handler
    },
#endif
#endif /* MTK_IAP2_PROFILE_ENABLE */
};

bt_status_t bt_sink_srv_send_action(bt_sink_srv_action_t action, void *parameters)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    uint32_t index;
    uint32_t action_module = (action & 0xF8F00000);

    bt_sink_srv_report_id("[Sink]bt_sink_srv_send_action, action:0x%x, module:0x%x", 2, action, action_module);

    //It will be removed when bt connection manager module has been restructured.
#ifndef MTK_BT_CM_SUPPORT
    if (BT_MODULE_CUSTOM_CONNECTION_MANAGER == action_module) {
        result = bt_sink_srv_cm_action_handler(action, parameters);
    }
#endif
    if (BT_MODULE_CUSTOM_SINK == action_module) {
        bt_sink_module_mask_t module_mask = SINK_MODULE_MASK_OFFSET(action);
        bt_sink_srv_report_id("[Sink]bt_sink_srv_send_action, module mask: 0x%x", 1, module_mask);

#if defined (AIR_LE_AUDIO_ENABLE)
        bt_sink_srv_type_t bt_type = BT_SINK_SRV_LE_AUDIO;
        bt_sink_srv_state_t state;
        bt_sink_srv_report_id("[LE_BT][Sink]bt_sink_srv_send_action,modlue_mask: 0x%x", 1, (SINK_MODULE_MASK_A2DP | SINK_MODULE_MASK_AVRCP | SINK_MODULE_MASK_HFP | SINK_MODULE_MASK_HSP));

        if (module_mask & (SINK_MODULE_MASK_A2DP | SINK_MODULE_MASK_AVRCP | SINK_MODULE_MASK_HFP | SINK_MODULE_MASK_HSP)) {
            bt_type = bt_sink_srv_get_current_type();
            if ((module_mask & (SINK_MODULE_MASK_HFP | SINK_MODULE_MASK_HSP)) && bt_type == BT_SINK_SRV_EDR) {
                state = le_sink_srv_get_state();
                if (state >= BT_SINK_SRV_STATE_INCOMING) {
                    bt_type = BT_SINK_SRV_LE_AUDIO;//for A2DP is playing but le call is comming or dialing
                }
            }

            bt_sink_srv_report_id("[LE_BT][Sink]bt_sink_srv_send_action,type:0x%x", 1, bt_type);

            if (bt_type == BT_SINK_SRV_LE_AUDIO) {
                bt_status_t ret = le_sink_srv_send_action(action, parameters);
               state = bt_sink_srv_get_state();
                if (state >= BT_SINK_SRV_STATE_INCOMING && (action >= BT_SINK_SRV_ACTION_3WAY_RELEASE_ALL_HELD && action <= 
                BT_SINK_SRV_ACTION_3WAY_EXPLICIT_CALL_TRANSFER)) {
                    ret = BT_STATUS_FAIL;
                }
                if (BT_STATUS_SUCCESS == ret) {
                    return ret;
                }
            }
        }
#endif

        for (index = 0; index < sizeof(bt_sink_srv_action_callback_table) / sizeof(bt_sink_srv_action_callback_table_t); index++) {
            if ((bt_sink_srv_action_callback_table[index].module & module_mask)
                && bt_sink_srv_action_callback_table[index].callback) {
                result = bt_sink_srv_action_callback_table[index].callback(action, parameters);
            }
        }
    }

    bt_sink_srv_report_id("[Sink]Action result:0x%x", 1, result);
    return result;
}

void bt_sink_srv_init(bt_sink_feature_config_t *features)
{
    bt_sink_srv_report_id("[Sink] bt_sink_srv_init", 0);

#ifdef MTK_AWS_MCE_ENABLE
    bt_sink_srv_register_aws_mce_report_callback();
#endif /*MTK_AWS_MCE_ENABLE*/

    //initialize register callback
    bt_sink_srv_register_callback_init();

    // initialize atci cmd
    bt_sink_srv_atci_init();

    bt_device_manager_init();

    bt_sink_srv_config_features(features);
#ifdef MTK_BT_CM_SUPPORT
    // initialize sink call (contains: hf && hsp)
    bt_sink_srv_call_init();
    // initialize sink music (contains: a2dp_sink && /* avrcp managed by sink music */)
    bt_sink_srv_music_init();
#endif
 
#ifdef BT_SINK_DUAL_ANT_ENABLE
    bt_sink_srv_dual_ant_init();
#endif
}


#if defined (AIR_LE_AUDIO_ENABLE)
static bt_sink_srv_type_t bt_sink_srv_get_current_type()
{
    bt_sink_srv_type_t bt_type = BT_SINK_SRV_LE_AUDIO;
    audio_src_srv_pseudo_device_t type = AUDIO_SRC_SRV_PSEUDO_DEVICE_BLE;
    //type = bt_sink_srv_ami_get_current_scenario();
    audio_src_srv_handle_t *device = audio_src_srv_get_runing_pseudo_device();
    if (device) {
        type = device->type;
    }

    bt_sink_srv_report_id("[LE_BT][Sink]bt_sink_srv_send_action,am_type:0x%x", 1, type);
#if 0
    if (type == NONE) {


		bt_type = BT_SINK_SRV_LE_AUDIO;


        bt_sink_srv_device_info_t *device_info = bt_sink_srv_is_idle_callback();
        if (device_info) {
            bt_type = device_info->type;
            bt_sink_srv_report_id("[LE_BT][Sink]bt_sink_srv_send_action,conn_type:0x%x", 1, device_info->type);
        }
    } else 
#endif


    if (type == AUDIO_SRC_SRV_PSEUDO_DEVICE_BLE) {
        bt_type = BT_SINK_SRV_LE_AUDIO;
    } else {
        bt_type = BT_SINK_SRV_EDR;
    }
    return bt_type;
}
#endif

uint8_t bt_sink_srv_query_device(bt_sink_srv_device_info_t *device_list)
{
    uint8_t link_num = 0;

#if 0
    bt_sink_srv_device_info_t *dev_info = bt_sink_srv_get_device_info();
    for (int i = 0; i < 5; i++) {
        bt_sink_srv_device_info_t *temp_info = dev_info + i;
        if (temp_info->mask & 0x08) {
            memcpy(device_list + i, dev_info, sizeof (bt_sink_srv_device_info_t));
            link_num++;
        }
    }
    LOG_MSGID_I(BT_APP, "[LE_BT]bt_sink_srv_is_idle_callback, link_num:%x", 1, link_num);
#endif
    return link_num;
}

