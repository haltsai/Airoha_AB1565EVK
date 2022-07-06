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


/**
 * File: bt_customer_config.c
 *
 * Description: This file defines interface and Macros for bt_customer_config.c
 *
 */

#ifndef __BT_CUSTOMER_CONFIG_H__
#define __BT_CUSTOMER_CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "bt_gap.h"
#include "bt_connection_manager.h"
#include "bt_sink_srv.h"
#include "bt_system.h"

#define BT_GAP_LE_MAX_DEVICE_NAME_LENGTH        (34)    /**< The maximum length of the device name, including null-ending char.*/
#define BT_A2DP_DISABLE_3M_MTU_SIZE             (672)   /**< The a2dp mtu size if 3M is disable.*/

/**
 * @brief    This function is used to get the bt connection configuration.
 * @return   The pointer of bt_cm_config_t.
 */
const bt_cm_config_t *bt_customer_config_get_cm_config(void);

/**
 * @brief    This function is used to quickly get the bt connection configuration.
 * @return   The pointer of bt_cm_config_t.
 */
const bt_cm_config_t *bt_customer_config_app_get_cm_config(void);

/**
 * @brief    This function is used to get the GAP configuration.
 * @return   The pointer of bt_gap_config_t.
 */
const bt_gap_config_t *bt_customer_config_get_gap_config(void);

/**
 * @brief    This function is used to get the BLE name.
 * @param[out] ble_name  The buffer of BLE name, it will be filled in the fuction.
 * @return   None.
 */
void bt_customer_config_get_ble_device_name(char ble_name[BT_GAP_LE_MAX_DEVICE_NAME_LENGTH]);

/**
 * @brief    This function is used to get the bt sink features.
 * @return   The pointer of bt_sink_feature_config_t.
 */
bt_sink_feature_config_t *bt_customer_config_get_bt_sink_features(void);

/**
 * @brief    This function is used to get parameters for HFP initializing.
 * @param[out] param  The buffer of HFP paramters, it will be changed in the fuction.
 * @return   BT_STATUS_SUCCESS or BT_STATUS_FAIL.
 */
bt_status_t bt_customer_config_hf_get_init_params(bt_hfp_init_param_t *param);

/**
 * @brief    This function is used to get BT initializing configuration.
 * @return   The BT initializing configuration mask, refer to bt_system.h.
 */
bt_init_feature_mask_t bt_customer_config_get_feature_mask_configuration(void);

/**
 * @brief    This function is used to set the HFP's audio codec.
 * @return   None.
 */
void update_hfp_audio_codec(bt_hfp_audio_codec_type_t new_type);

/**
 * @brief    This function is used to get the HFP's audio codec.
 * @return   The current audio codec.
 */
bt_hfp_audio_codec_type_t get_hfp_audio_codec();

#ifdef __cplusplus
}
#endif

#endif /* __BT_APP_COMMON_H__ */

