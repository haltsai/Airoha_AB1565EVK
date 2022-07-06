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

#include "bt_device_manager_internal.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_utils.h"

const static bt_device_manager_db_remote_pnp_info_t g_local_asi_device_id_list[] = {
    {0x2462, 0x00e0}, //GVA-11949, ASUS Chromebook
};

const static uint8_t g_local_asi_device_name_list[][BT_GAP_MAX_DEVICE_NAME_LENGTH + 1] = {
    "NW-ZX2",
};

bool bt_sink_srv_music_is_local_asi_device(void *bt_addr, bool *is_local_asi)
{
    uint8_t device_id_count = sizeof(g_local_asi_device_id_list)/sizeof(bt_device_manager_db_remote_pnp_info_t);
    uint8_t device_name_count = sizeof(g_local_asi_device_name_list)/(BT_GAP_MAX_DEVICE_NAME_LENGTH + 1);
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if(role == BT_AWS_MCE_ROLE_PARTNER || role == BT_AWS_MCE_ROLE_CLINET) return false;
    bt_device_manager_paired_device_complete_infomation_t remote_dev_info;
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    bt_device_manager_db_remote_pnp_info_t dev_id_p = {0,0};
    uint8_t *remote_addr = (uint8_t *)bt_addr;
    *is_local_asi = false;
    uint8_t i = 0;

    bt_device_manager_get_complete_paired_info((bt_bd_addr_ptr_t)(remote_addr),  &remote_dev_info);
    bt_device_manager_remote_find_pnp_info(remote_addr, &dev_id_p);

    if(run_dev) {
        uint8_t *run_addr = (uint8_t *)(&run_dev->dev_addr);
        bt_sink_srv_report_id("run_dev--0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", 6, run_addr[0], run_addr[1], run_addr[2], run_addr[3], run_addr[4], run_addr[5]);
        bt_sink_srv_report_id("N9_dev--0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", 6, remote_addr[0], remote_addr[1],remote_addr[2], remote_addr[3], remote_addr[4], remote_addr[5]);
    }

    for(i = 0; i < device_id_count; i++) {
        if(dev_id_p.product_id == g_local_asi_device_id_list[i].product_id
            &&dev_id_p.vender_id== g_local_asi_device_id_list[i].vender_id) {
            *is_local_asi = true;
            break;
        }
    }

    for(i = 0; i < device_name_count; i++) {
        if((strlen(remote_dev_info.name) == strlen((const char *)g_local_asi_device_name_list[i]))
            &&(bt_sink_srv_memcmp(g_local_asi_device_name_list[i], remote_dev_info.name, strlen((const char *)g_local_asi_device_name_list[i]))==0)) {
            *is_local_asi = true;
            break;
        }

    }

    bt_sink_srv_music_get_nvdm_data((bt_bd_addr_t *)remote_addr, BT_SINK_SRV_MUSIC_DATA_LOCAL_ASI_FLAG, (void *)is_local_asi);

    return true;
}

uint32_t bt_a2dp_get_avdtp_connection_delay(const bt_bd_addr_t *address)
{
    uint32_t ret_delay = 0;
    bt_device_manager_db_remote_pnp_info_t dev_id_p = {0,0};
    bt_device_manager_paired_device_complete_infomation_t remote_dev_info;

    bt_device_manager_remote_find_pnp_info((void *)address, &dev_id_p);
    bt_device_manager_get_complete_paired_info((bt_bd_addr_ptr_t)(address),  &remote_dev_info);

    if (dev_id_p.product_id == 0x0000 && dev_id_p.vender_id == 0x0000) {
        if ((strlen(remote_dev_info.name) == strlen("Nintendo Switch"))
            && (bt_sink_srv_memcmp("Nintendo Switch", remote_dev_info.name, strlen("Nintendo Switch")) == 0)) {
                ret_delay = 3000;
            }
    }
    bt_sink_srv_report_id("bt_a2dp_get_avdtp_connection_delay ret_delay:%d", 1, ret_delay);
    return ret_delay;
}
