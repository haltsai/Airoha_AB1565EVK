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


#ifndef __NVDM_ID_LIST_H__
#define __NVDM_ID_LIST_H__


#define NVDM_GROUP_APP_DYNAMIC_FEATURE                      "features"
#define NVDM_APP_DYNAMIC_FEATURE_ITEM_AUTO_RHO              "AUTORHO"

#define NVDM_GROUP_APP                                      "app"
#define NVDM_APP_SIDETONE_VALUE                             "sidetone_value"

#define NVDM_GROUP_BT                                       "BT"
#define NVDM_BT_ITEM_LOCAL_ADDR                             "local_addr"
#define NVDM_BT_ITEM_MUSIC_VOLUME                           "music_volume"


#define NVDM_GROUP_BT_APP                                   "BT_APP"
#define NVDM_BT_APP_ITEM_BLE_RANDOM_ADDRESS                 "address"
#define NVDM_BT_APP_ITEM_BLE_LOCAL_IRK                      "irk"
#define NVDM_BT_APP_ITEM_FIX_ADDR                           "fix_addr"
#define NVDM_BT_APP_ITEM_NAME                               "name"
#define NVDM_BT_APP_ITEM_BLE_TILE_ADDR                      "tile_addr"


#define NVDM_GROUP_BT_DM                                    "BT_DM"
#define NVDM_BT_DM_ITEM_BONDED_INFO_PREFIX                  "bonded_info_%02x"


#define NVDM_GROUP_BLE_SRV                                  "BLE_SRV"
#define NVDM_BLE_SRV_ITEM_RANDOM_ADDRESS                    "address"

#define NVDM_GROUP_COMMON                                   "common"
#define NVDM_COMMON_ITEM_AUDIO_PATH                         "audio_path"


#define NVDM_GROUP_CONTEXT_INFO                             "context_info"
#define NVDM_CONTEXT_INFO_ITEM_INDEX                        "index"
#define NVDM_CONTEXT_INFO_ITEM_INFO_0                       "info_0"
#define NVDM_CONTEXT_INFO_ITEM_INFO_1                       "info_1"
#define NVDM_CONTEXT_INFO_ITEM_INFO_2                       "info_2"


#define NVDM_GROUP_FACTORY_RESET                            "factrst"
#define NVDM_FACTORY_RESET_ITEM_FACTORY_RESET_FLAG          "factrst_flag"


#define NVDM_GROUP_FAST_PAIR_APP                            "fast_pair"
#define NVDM_GROUP_FAST_PAIR_ACCOUNT_KEY                    "account_key_list"
#define NVDM_FAST_PAIR_PERSONALIZED_NAME                    "personalized_name"


#define NVDM_GROUP_FOTA                                     "fota"
#define NVDM_FOTA_ITEM_TRIGGER_MARK                         "fota_mark"
#define NVDM_FOTA_ITEM_VERSION                              "ver"


#define NVDM_GROUP_GSOUND                                   "GSOUND"
#define NVDM_GSOUND_ITEM_INIT_STATE                         "init_state"


#define NVDM_GROUP_GSOUND_DATA                              "gsound_data"
#define NVDM_GSOUND_DATA_ITEM_DATA                          "data"

#define NVDM_GROUP_DHSS                                     "DHSS"
#define NVDM_DHSS_ITEM_PAIRED_LE_ADDR                       "paired_le_addr"

#define NVDM_GROUP_NAME_SPEAKER                             "speaker"
#define NVDM_SPEAKER_ITEM_FIRMWARE_TYPE                     "firmware_type"


#define NVDM_GROUP_PORT_SETTING                             "port_service"
#define NVDM_PORT_SETTING_ITEM_PORT_ASSIGN                  "port_assign"
#define NVDM_PORT_SETTING_ITEM_PORT_CONFIG                  "port_config"


#define NVDM_GROUP_SYSLOG                                   "SYSLOG"
#define NVDM_SYSLOG_ITEM_CPU_FILTER                         "cpu_filter"
#define NVDM_SYSLOG_ITEM_MODULE_FILTER                      "module_filter"
#define NVDM_SYSLOG_ITEM_DUMP                               "dump"


#define NVDM_GROUP_MMI_APP                                  "mmi_app"
#define NVDM_MULTI_ADV_LE_ADDR                              "multi_adv_le_addr"


#define NVDM_INTERNAL_USE_GROUP                             "nvdm"
#define NVDM_USE_SETTING                                    "user_setting"

#ifdef MTK_VA_MODEL_MANAGER_ENABLE
#define NVDM_VA_MODEL_MANAGER_GROUP                         "va_model_manager"
#define NVDM_VA_MODEL_MANAGER_ITEM                          "backup_item"
#endif /* MTK_VA_MODEL_MANAGER_ENABLE */

#define NVDM_GROUP_HCI_SWITCH                               "hci_log_switch_group"
#define NVDM_HCI_SWITCH_ITEM_SWITCH_ITEM                    "hci_log_switch_item"

#endif /* __NVDM_ID_LIST_H__ */