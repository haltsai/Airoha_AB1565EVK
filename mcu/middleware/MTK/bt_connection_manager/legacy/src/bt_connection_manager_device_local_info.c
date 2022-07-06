/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

 
#include "bt_type.h"
#include "bt_aws_mce.h"
#include "bt_connection_manager.h"
#include "bt_device_manager_internal.h"

bt_bd_addr_t* bt_connection_manager_device_local_info_get_aws_fixed_address(void)
{
#ifdef MTK_AWS_MCE_ENABLE
    return bt_device_manager_aws_local_info_get_fixed_address();
#else
    return NULL;
#endif
}

void bt_connection_manager_device_local_info_store_local_address(bt_bd_addr_t *addr)
{
    bt_device_manager_store_local_address_internal(addr);
}

bt_bd_addr_t* bt_connection_manager_device_local_info_get_local_address(void)
{
    return bt_device_manager_get_local_address();
}

void bt_connection_manager_device_local_info_store_peer_aws_address(bt_bd_addr_t *addr)
{
#ifdef MTK_AWS_MCE_ENABLE
    bt_device_manager_aws_local_info_store_peer_address(addr);
#endif
}

bt_bd_addr_t* bt_connection_manager_device_local_info_get_peer_aws_address(void)
{
#ifdef MTK_AWS_MCE_ENABLE
    return bt_device_manager_aws_local_info_get_peer_address();
#else
    return NULL;
#endif
}

void bt_connection_manager_device_local_info_store_aws_key(bt_key_t* aws_key)
{
#ifdef MTK_AWS_MCE_ENABLE
    bt_device_manager_aws_local_info_store_key(aws_key);
#endif
}

bt_key_t* bt_connection_manager_device_local_info_get_aws_key(void)
{
#ifdef MTK_AWS_MCE_ENABLE
    return bt_device_manager_aws_local_info_get_key();
#else
    return NULL;
#endif
}

void bt_connection_manager_device_local_info_store_aws_role(bt_aws_mce_role_t aws_role)
{
#ifdef MTK_AWS_MCE_ENABLE
    bt_device_manager_aws_local_info_store_role(aws_role);
#endif
}

bt_aws_mce_role_t bt_connection_manager_device_local_info_get_aws_role(void)
{
#ifdef MTK_AWS_MCE_ENABLE
    return bt_device_manager_aws_local_info_get_role();
#else
    return BT_AWS_MCE_ROLE_NONE;
#endif
}

void bt_connection_manager_device_local_info_store_aws_ls_enable(bt_connection_manager_aws_ls_enable_t ls_enable)
{
#ifdef MTK_AWS_MCE_ENABLE
    bt_device_manager_aws_local_info_store_ls_enable(ls_enable);
#endif
}

bt_connection_manager_aws_ls_enable_t bt_connection_manager_device_local_info_get_aws_ls_enable()
{
#ifdef MTK_AWS_MCE_ENABLE
    return bt_device_manager_aws_local_info_get_ls_enable();
#else
    return BT_CONNECTION_MANAGER_AWS_LS_DISABLE;
#endif
}

void bt_connection_manager_device_local_info_init()
{
    bt_device_manager_init();
}


