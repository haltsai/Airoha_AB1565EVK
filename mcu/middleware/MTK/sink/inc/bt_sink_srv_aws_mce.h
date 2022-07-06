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
 
#ifndef __BT_SINK_SRV_AWS_MCE_H__
#define __BT_SINK_SRV_AWS_MCE_H__

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_sink_srv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_SINK_SRV_AWS_MCE_NVDM_GROUP "BT_SINK"
#define BT_SINK_SRV_AWS_MCE_NVDM_NAME  "AWS_ROLE"

#define BT_SINK_SRV_AWS_MCE_CONNECTION_NUM 4

#if defined(SUPPORT_ROLE_HANDOVER_SERVICE) && defined(AIR_MULTI_POINT_ENABLE)
#define BT_SINK_SRV_AWS_MCE_IN_MULTIPOINT
#endif /* SUPPORT_ROLE_HANDOVER_SERVICE && AIR_MULTI_POINT_ENABLE */

#define BT_SINK_SRV_AWS_MCE_MODULE_MUSIC     0
#define BT_SINK_SRV_AWS_MCE_MODULE_CALL      1
#define BT_SINK_SRV_AWS_MCE_MODULE_INDEX_NUM 2
typedef uint8_t bt_sink_srv_aws_mce_module_index_t; /**<The module index. */

#define BT_SINK_SRV_AWS_MCE_IS_SPECIAL_LINK(local_address, address) \
    (bt_sink_srv_memcmp(local_address, address, sizeof(bt_bd_addr_t)) == 0)

typedef bt_status_t (*aws_mce_callback_func_t)(bt_msg_type_t msg, bt_status_t status, void *buffer);

typedef struct
{
    aws_mce_callback_func_t aws_callback;
}bt_sink_srv_aws_mce_cb_t;

typedef struct {
    uint32_t aws_handle;
    bt_sink_srv_profile_connection_state_t conn_state;
    bt_aws_mce_agent_state_type_t state;
    void* module_info[BT_SINK_SRV_AWS_MCE_MODULE_INDEX_NUM];
} bt_sink_srv_aws_mce_context_t;

void bt_sink_srv_aws_mce_init(void);
void bt_sink_srv_aws_mce_deinit(void);
uint32_t bt_sink_srv_aws_mce_get_handle(bt_bd_addr_t *bt_addr);
bt_bd_addr_t* bt_sink_srv_aws_mce_get_address(uint32_t aws_handle);
void* bt_sink_srv_aws_mce_get_module_info_by_handle(uint32_t aws_handle, bt_sink_srv_aws_mce_module_index_t module);
bt_bd_addr_t * bt_sink_srv_aws_mce_get_module_address(void* module_info);
uint32_t bt_sink_srv_aws_mce_get_module_gap_handle(void* module_info);
bt_status_t bt_sink_srv_aws_mce_action_handler(bt_sink_srv_action_t action, void *param);
void* bt_sink_srv_aws_mce_get_module_info_by_handle(uint32_t aws_handle, bt_sink_srv_aws_mce_module_index_t module);
void* bt_sink_srv_aws_mce_get_module_info(bt_bd_addr_t *address, bt_sink_srv_aws_mce_module_index_t module);
bt_aws_mce_agent_state_type_t bt_sink_srv_aws_mce_get_aws_state_by_handle(uint32_t aws_handle);

#ifdef __cplusplus
}
#endif

#endif /*MTK_AWS_MCE_ENABLE*/

#endif /* __BT_SINK_SRV_AWS_MCE_H__ */

