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

/**
 * File: nvdm_config_factory_reset.h
 *
 * Description: This file defines the interface of nvdm_config_factory_reset.c.
 *
 */

#ifndef __NVDM_CONFIG_FACTORY_RST_H__
#define __NVDM_CONFIG_FACTORY_RST_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MTK_NVDM_ENABLE

#define FACTORY_RESET_FLAG      (0x55)    /* Flag write in NVDM to indicate that factory reset is requested, all NVDM items except for those in FACTORY_RST_RESERVED_NVDM_ITEM_LIST will be deleted. */
#define FACTORY_RESET_LINK_KEY  (0x56)    /* Flag write in NVDM to indicate that link key reset is requested, only link related NVkey items will be deleted. */

/*
 * This macro defines table of NVDM items that will be reserved after factory reset. Items not in the table will be deleted.
 * 1. The user should put the NVDM items which need to be reserved after factory reset here with group name and item name.
 * 2. Those reserved NVDM items should be reviewed carefully before put them here.
 * 3. See mcu/middleware/MTK/nvdm/inc/nvkey_id_list.h and mcu/middleware/MTK/nvdm/inc/nvdm_id_list.h for more nvdm item details.
 */
#define FACTORY_RST_RESERVED_NVDM_ITEM_LIST \
{\
    /* NVDM internal use. */ \
    {NVDM_INTERNAL_USE_GROUP, NVDM_USE_SETTING},\
    /* BT relative. */        \
    {NVDM_GROUP_BT, NVDM_BT_ITEM_LOCAL_ADDR},\
    {NVDM_GROUP_BT_APP, NVDM_BT_APP_ITEM_FIX_ADDR},\
    {NVDM_GROUP_BT_APP, NVDM_BT_APP_ITEM_NAME},\
    {NVDM_GROUP_CONTEXT_INFO, NVDM_CONTEXT_INFO_ITEM_INDEX},\
    {NVDM_GROUP_CONTEXT_INFO, NVDM_CONTEXT_INFO_ITEM_INFO_0},\
    {NVDM_GROUP_CONTEXT_INFO, NVDM_CONTEXT_INFO_ITEM_INFO_1},\
    {NVDM_GROUP_CONTEXT_INFO, NVDM_CONTEXT_INFO_ITEM_INFO_2},\
    {NVDM_GROUP_HCI_SWITCH, NVDM_HCI_SWITCH_ITEM_SWITCH_ITEM},\
    {NVDM_GROUP_PORT_SETTING, NVDM_PORT_SETTING_ITEM_PORT_ASSIGN},\
    {NVDM_GROUP_PORT_SETTING, NVDM_PORT_SETTING_ITEM_PORT_CONFIG},\
    {NVDM_INTERNAL_USE_GROUP, NVDM_USE_SETTING},\
};

#define FACTORY_RST_RESERVED_NVKEY_ITEM_LIST \
{\
    NVKEYID_NVDM_VERSION,\
    NVKEYID_PRODUCT_CATEGORY,\
    NVKEYID_BT_AIR_SPP_UUID_DATA,\
    NVKEYID_BT_ULL_PEER_ADDRESS,\
    NVKEYID_BT_LE_AUDIO_CSIS_DATA,\
    NVKEYID_BT_LE_AUDIO_AICS_GAIN_SETTING_PROPERTIES,\
    NVKEYID_BT_LE_AUDIO_METADATA,\
    NVKEYID_MP_CAL_CHG_CONFIG,\
    NVKEYID_BATTERY_MANAGEMENT_BASIC,\
    NVKEYID_MP_CAL_INT_CHG_DAC_CONFIG,\
    NVKEYID_MP_CAL_INT_CHG_TRICKLE_CURRENT_CONFIG,\
    NVKEYID_MP_CAL_INT_CHG_CC1_CURRENT_CONFIG,\
    NVKEYID_MP_CAL_INT_CHG_CC2_CURRENT_CONFIG,\
    NVKEYID_MP_CAL_CV_STOP_CURRENT_ADC,\
    NVKEYID_MP_CAL_SYS_LDO,\
    NVKEYID_MP_CAL_OCP,\
    NVKEYID_MP_CAL_JEITA,\
    NVKEYID_MP_CAL_JEITA_WARM,\
    NVKEYID_MP_CAL_JEITA_COOL,\
    NVKEYID_MP_CAL_CHG_ADC_CONFIG,\
    NVKEYID_MP_CAL_BUCK_MV_CONFIG,\
    NVKEYID_MP_CAL_BUCK_MV_STB_CONFIG,\
    NVKEYID_MP_CAL_BUCK_MV_FREQ,\
    NVKEYID_MP_CAL_BUCK_LV_CONFIG,\
    NVKEYID_MP_CAL_BUCK_LV_LPM,\
    NVKEYID_MP_CAL_BUCK_LV_FREQ,\
    NVKEYID_MP_CAL_LDO_VDD33_REG_CONFIG,\
    NVKEYID_MP_CAL_LDO_VDD33_REG_RET_CONFIG,\
    NVKEYID_MP_CAL_LDO_VDD33_RET_CONFIG,\
    NVKEYID_MP_CAL_LDO_VRF_REG_CONFIG,\
    NVKEYID_MP_CAL_LDO_VRF_REG_RET_CONFIG,\
    NVKEYID_MP_CAL_LDO_VRF_RET_CONFIG,\
    NVKEYID_MP_CAL_VDIG18_CONFIG,\
    NVKEYID_MP_CAL_HPBG_CONFIG,\
    NVKEYID_MP_CAL_LPBG_CONFIG,\
    NVKEYID_MP_CAL_VBAT_ADC_CALIBRATION_TABLE,\
    NVKEYID_MP_CAL_VBAT_VOLTAGE_CONFIG,\
    NVKEYID_BATTERY_MANAGEMENT_CHR,\
    NVKEYID_BT_TX_POWER_GC,\
    NVKEYID_MP_CAL_XO_26M_CRTSTAL_TRIM,\
    NVKEYID_BR_DPC_VALUE_TABLE_MP_K,\
    NVKEYID_EDR_DPC_VALUE_TABLE_MP_K,\
    NVKEYID_BR_TX_GAIN_TABLE_MP_K,\
    NVKEYID_EDR_TX_GAIN_TABLE_MP_K,\
    NVKEYID_NTC_NTC_PARA_CONFIG1,\
    NVKEYID_NTC_NTC_PARA_CONFIG2,\
    NVKEYID_NTC_NTC_PARA_TOOL_TEMP_TABLE,\
    NVKEYID_NTC_NTC_PARA_TOOL_RESISTOR_TABLE,\
    NVKEYID_NTC_NTC_PARA_TEMP_TABLE,\
    NVKEYID_NTC_NTC_PARA_RARIO_TABLE,\
    NVKEYID_RF_MISC_PWR_CTL,\
    NVKEYID_LM_INFO_LMP_FEATURES_PAGE0,\
    NVKEYID_LM_INFO_LMP_FEATURES_PAGE1,\
    NVKEYID_LM_INFO_LMP_FEATURES_PAGE2,\
    NVKEYID_BT_BLE_BLE_ADV_NAME,\
    NVKEYID_USB_SETTING_0,\
    NVKEYID_AIRO_KEY_NUMS,\
    NVKEYID_AIRO_KEY_1,\
    NVKEYID_AIRO_KEY_2,\
    NVKEYID_AIRO_KEY_3,\
    NVKEYID_AIRO_KEY_4,\
    NVKEYID_AIRO_KEY_5,\
    NVKEYID_AIRO_KEY_6,\
    NVKEYID_AIRO_KEY_7,\
    NVKEYID_AIRO_KEY_8,\
    NVKEYID_AIRO_KEY_9,\
    NVKEYID_AIRO_KEY_A,\
    NVKEYID_AIRO_KEY_B,\
    NVKEYID_AIRO_KEY_C,\
    NVKEYID_AIRO_KEY_D,\
    NVKEYID_AIRO_KEY_E,\
    NVKEYID_AIRO_KEY_F,\
    NVKEYID_CAPTOUCH_CALI_DATA,\
    NVKEYID_CAPTOUCH_CALI_4CHDATA,\
    NVKEYID_CAPTOUCH_SDWU_DATA,\
    NVKEYID_CAPTOUCH_FEATURE,\
    NVKEYID_CAPTOUCH_EARDETECT_DATA,\
    NVKEYID_CAPTOUCH_FORCETOUCH_DATA,\
    NVKEYID_CAPTOUCH_TUNE_DATA,\
    NVKEYID_CAPTOUCH_AUTOSUSPEND_DATA,\
    NVKEYID_DSP_FW_PARA_DATADUMP,\
    NVKEYID_BT_MUSIC_LATENCY_SETTING,\
    NVKEYID_DSP_FW_PARA_VOLUME_PARAMETERS,\
    NVKEYID_DSP_FW_PARA_VOW_PARAMETERS,\
    NVKEYID_DSP_PARA_RESERVED,\
    NVKEYID_DSP_FW_PARA_VOLUME_PARAMETERS_2,\
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_COMMON,\
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_A2DP,\
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_LINE,\
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_SCO,\
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_VC,\
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_VP,\
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_RT,\
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_AT,\
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_SCO_NB,\
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_USB_AUDIO,\
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_MEDIAPLAYER,\
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_A2DP,\
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_LINE,\
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_SCO,\
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_VC,\
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_VP,\
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_RT,\
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_AT,\
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_SCO_NB,\
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_USB_AUDIO,\
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_MEDIAPLAYER,\
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_A2DP,\
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_LINE,\
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_SCO,\
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_VC,\
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_VP,\
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_RT,\
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_AT,\
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_DETACH_MIC,\
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_A2DP,\
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_LINE,\
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_SCO,\
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_VC,\
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_VP,\
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_RT,\
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_AT,\
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_SCO_NB,\
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_DETACH_MIC,\
    NVKEYID_DSP_FW_PARA_DIN_GP_TABLE_SCO,\
    NVKEYID_DSP_FW_PARA_DIN_GP_TABLE_SCO_NB,\
    NVKEYID_DSP_FW_PARA_DIN_GP_TABLE_AT,\
    NVKEYID_DSP_FW_PARA_DIN_GP_TABLE_SCO_MULTI_MIC,\
    NVKEYID_DSP_FW_PARA_DIN_GP_TABLE_A2DPSOURCE,\
    NVKEYID_DSP_FW_PARA_DIN_GP_TABLE_SYSTEM_SOFTWARE,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_L,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_R,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_NB_L,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_NB_R,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_AT_L,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_AT_R,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_MULTI_MIC,\
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_WWE,\
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_MIC_FUNC,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_MIC2,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_MIC3,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_MIC4,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_MIC5,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_NB_MIC2,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_NB_MIC3,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_NB_MIC4,\
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_NB_MIC5,\
    NVKEYID_DSP_ALG_PARA_DRC,\
    NVKEYID_DSP_ALG_PARA_WB_TX_VO_CPD,\
    NVKEYID_DSP_ALG_PARA_NB_TX_VO_CPD,\
    NVKEYID_DSP_ALG_PARA_WB_RX_VO_CPD,\
    NVKEYID_DSP_ALG_PARA_NB_RX_VO_CPD,\
    NVKEYID_DSP_ALG_PARA_VP_CPD,\
    NVKEYID_DSP_ALG_PARA_A2DP_AU_CPD,\
    NVKEYID_DSP_ALG_PARA_LINE_AU_CPD,\
    NVKEYID_DSP_ALG_PARA_POSITIVE_GAIN,\
    NVKEYID_DSP_ALG_PARA_INS,\
    NVKEYID_DSP_ALG_PARA_EFFECT,\
    NVKEYID_DSP_ALG_PARA_VC,\
    NVKEYID_DSP_ALG_PARA_PLC,\
    NVKEYID_DSP_ALG_PARA_AEC_NR,\
    NVKEYID_DSP_ALG_PARA_WB_RX_EQ,\
    NVKEYID_DSP_ALG_PARA_WB_TX_EQ,\
    NVKEYID_DSP_ALG_PARA_NB_RX_EQ,\
    NVKEYID_DSP_ALG_PARA_NB_TX_EQ,\
    NVKEYID_DSP_ALG_PARA_WB_RX_EQ_2ND,\
    NVKEYID_DSP_ALG_PARA_NB_RX_EQ_2ND,\
    NVKEYID_DSP_ALG_PARA_INEAR_EQ,\
    NVKEYID_DSP_ALG_PARA_AST_EQ,\
    NVKEYID_DSP_PARA_WB_TX_FIR_EQ,\
    NVKEYID_DSP_PARA_NB_TX_FIR_EQ,\
    NVKEY_DSP_PARA_VAD_COMMON       ,\
    NVKEY_DSP_PARA_VAD_1MIC_V_MODE  ,\
    NVKEY_DSP_PARA_VAD_1MIC_C_MODE  ,\
    NVKEY_DSP_PARA_VAD_2MIC_V_MODE  ,\
    NVKEY_DSP_PARA_VAD_2MIC_C_MODE  ,\
    NVKEYID_DSP_PARA_ANC_L_FILTER1,\
    NVKEYID_DSP_PARA_ANC_L_FILTER2,\
    NVKEYID_DSP_PARA_ANC_L_FILTER3,\
    NVKEYID_DSP_PARA_ANC_L_FILTER4,\
    NVKEYID_DSP_PARA_ANC_R_FILTER1,\
    NVKEYID_DSP_PARA_ANC_R_FILTER2,\
    NVKEYID_DSP_PARA_ANC_R_FILTER3,\
    NVKEYID_DSP_PARA_ANC_R_FILTER4,\
    NVKEYID_DSP_PARA_ANC_L_FILTER5,\
    NVKEYID_DSP_PARA_ANC_R_FILTER5,\
    NVKEYID_DSP_PARA_ANC_SW_GAIN  ,\
    NVKEYID_DSP_PARA_ANC,\
    NVKEYID_DSP_PARA_ANC_MISC_PARA,\
    NVKEYID_DSP_PARA_PASS_THRU,\
    NVKEYID_DSP_PARA_LEAKAGE_COMPENSATION,\
    NVKEYID_DSP_PARA_VOICE_WB_RX_AGC,\
    NVKEYID_DSP_PARA_VOICE_NB_RX_AGC,\
    NVKEYID_DSP_PARA_VOICE_TX_AGC,\
    NVKEYID_DSP_PARA_AEC_NR_BOOMIC,\
    NVKEYID_DSP_PARA_NB_TX_EQ_BOOMIC,\
    NVKEYID_DSP_PARA_WB_TX_EQ_BOOMIC,\
    NVKEYID_DSP_PARA_WB_TX_VO_CPD_BOOMIC,\
    NVKEYID_DSP_PARA_NB_TX_FIR_EQ_BOOMIC,\
    NVKEYID_DSP_PARA_WB_TX_FIR_EQ_BOOMIC,\
    NVKEYID_DSP_PARA_SIDETONE_FIR_COEFFICIENT,\
    NVKEYID_DSP_PARA_ANC_L_FILTER6,\
    NVKEYID_DSP_PARA_ANC_R_FILTER6,\
    NVKEYID_DSP_PARA_ANC_L_FILTER7,\
    NVKEYID_DSP_PARA_ANC_R_FILTER7,\
    NVKEYID_DSP_PARA_ANC_L_FILTER_1,\
    NVKEYID_DSP_PARA_ANC_R_FILTER_1,\
    NVKEYID_DSP_PARA_ANC_L_FILTER_2,\
    NVKEYID_DSP_PARA_ANC_R_FILTER_2,\
    NVKEYID_DSP_PARA_ANC_L_FILTER_3,\
    NVKEYID_DSP_PARA_ANC_R_FILTER_3,\
    NVKEYID_DSP_PARA_ANC_L_FILTER_4,\
    NVKEYID_DSP_PARA_ANC_R_FILTER_4,\
    NVKEYID_DSP_PARA_ANC_PATH_SETTING,\
    NVKEYID_DSP_PARA_PASSTHRU_L_FILTER_1,\
    NVKEYID_DSP_PARA_PASSTHRU_R_FILTER_1,\
    NVKEYID_DSP_PARA_PASSTHRU_L_FILTER_2,\
    NVKEYID_DSP_PARA_PASSTHRU_R_FILTER_2,\
    NVKEYID_DSP_PARA_PASSTHRU_L_FILTER_3,\
    NVKEYID_DSP_PARA_PASSTHRU_R_FILTER_3,\
    NVKEYID_DSP_PARA_ANC_CURRENT_STATUS,\
    NVKEYID_DSP_PARA_ANC_SETTING,\
    NVKEYID_DSP_PARA_PASSTHRU_SETTING,\
    NVKEYID_DSP_PARA_ANC_POWER_DETECT,\
    NVKEYID_DSP_PARA_PASSTHRU_POWER_DETECT,\
    NVKEYID_DSP_PARA_ANC_CALIBRATE_GAIN,\
    NVKEYID_DSP_PARA_ANC_DEFAULT_STATUS,\
    0xE1E7,\
    0xE1E8,\
    0xE1E9,\
    0xE1EA,\
    0xE1EB,\
    0xE1EC,\
    0xE1ED,\
    0xE1EE,\
    0xE1EF,\
    NVKEYID_DSP_PARA_ANC_MULTI_FEATURE,\
    NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_PRE_PEQ_1,\
    NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_PRE_PEQ_2,\
    NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_PRE_PEQ_3,\
    NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_PRE_PEQ_4,\
    NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_PRE_PEQ_TABLE,\
    NVKEYID_DSP_A2DP_PEQ_0,\
    NVKEYID_DSP_A2DP_PEQ_1,\
    NVKEYID_DSP_A2DP_PEQ_2,\
    NVKEYID_DSP_A2DP_PEQ_3,\
    NVKEYID_DSP_A2DP_PEQ_4,\
    NVKEYID_DSP_A2DP_PEQ_5,\
    NVKEYID_DSP_A2DP_PEQ_6,\
    NVKEYID_DSP_A2DP_PEQ_7,\
    NVKEYID_DSP_A2DP_PEQ_8,\
    NVKEYID_DSP_A2DP_PEQ_9,\
    NVKEYID_DSP_A2DP_PEQ_10,\
    NVKEYID_DSP_A2DP_PEQ_11,\
    NVKEYID_DSP_A2DP_PEQ_12,\
    NVKEYID_DSP_A2DP_PEQ_13,\
    NVKEYID_DSP_A2DP_PEQ_14,\
    NVKEYID_DSP_A2DP_PEQ_15,\
    NVKEYID_DSP_A2DP_PEQ_16,\
    NVKEYID_DSP_A2DP_PEQ_17,\
    NVKEYID_DSP_A2DP_PEQ_18,\
    NVKEYID_DSP_A2DP_PEQ_19,\
    NVKEYID_DSP_A2DP_PEQ_20,\
    NVKEYID_DSP_A2DP_PEQ_21,\
    NVKEYID_DSP_A2DP_PEQ_22,\
    NVKEYID_DSP_A2DP_PEQ_23,\
    NVKEYID_DSP_A2DP_PEQ_24,\
    NVKEYID_DSP_A2DP_PEQ_25,\
    NVKEYID_DSP_A2DP_PEQ_26,\
    NVKEYID_DSP_A2DP_PEQ_27,\
    NVKEYID_LOG_SETTING,\
    NVKEYID_EXCEPTION_DUMP_MODE,\
    NVKEYID_APP_DEVICE_NAME_DEFAULT,\
    NVKEYID_APP_VP_LANG,\
    NVKEYID_PEQ_AUDIO_MISC_PARA,\
    NVKEYID_PEQ_AUDIO_PATH_TABLE,\
    NVKEYID_PEQ_GROUP_TO_PARA_TABLE1,\
    NVKEYID_PEQ_GROUP_TO_PARA_TABLE2,\
    NVKEYID_PEQ_GROUP_TO_PARA_TABLE3,\
    NVKEYID_PEQ_GROUP_TO_PARA_TABLE4,\
    NVKEYID_PEQ_GROUP_TO_PARA_TABLE5,\
    NVKEYID_PEQ_GROUP_TO_PARA_TABLE6,\
    NVKEYID_APP_VOLUME_SCO_SETTING,\
    NVKEYID_APP_VOLUME_A2DP_SETTING,\
    NVKEYID_APP_VOLUME_MP3_SETTING,\
    NVKEYID_APP_VOLUME_SCOMIC_SETTING,\
    NVKEYID_APP_VOLUME_VPRT_SETTING,\
    NVKEYID_APP_VOLUME_LINEIN_SETTING,\
    NVKEYID_APP_VOLUME_USB_AUDIO_SETTING,\
    NVKEYID_APP_VOLUME_MEDIAPLAYER_SETTING,\
    NVKEYID_APP_VOLUME_A2DPSOURCE_SETTING,\
    NVKEYID_APP_VOLUME_USB_AUDIO_SW_SETTING,\
    NVKEYID_APP_VOLUME_USB_VOICE_SW_SETTING,\
    NVKEYID_PEQ_PARAMETER1,\
    NVKEYID_PEQ_PARAMETER2,\
    NVKEYID_PEQ_PARAMETER3,\
    NVKEYID_PEQ_PARAMETER4,\
    NVKEYID_PEQ_PARAMETER5,\
    NVKEYID_PEQ_PARAMETER6,\
    NVKEYID_PEQ_PARAMETER7,\
    NVKEYID_PEQ_PARAMETER8,\
    NVKEYID_PEQ_PARAMETER9,\
    NVKEYID_PEQ_PARAMETER10,\
    NVKEYID_PEQ_PARAMETER11,\
    NVKEYID_PEQ_PARAMETER12,\
    NVKEYID_PEQ_PARAMETER13,\
    NVKEYID_PEQ_PARAMETER14,\
    NVKEYID_PEQ_PARAMETER15,\
    NVKEYID_PEQ_PARAMETER16,\
    NVKEYID_PEQ_PARAMETER17,\
    NVKEYID_PEQ_PARAMETER18,\
    NVKEYID_PEQ_PARAMETER19,\
    NVKEYID_PEQ_PARAMETER20,\
    NVKEYID_PEQ_PARAMETER21,\
    NVKEYID_PEQ_PARAMETER22,\
    NVKEYID_PEQ_PARAMETER23,\
    NVKEYID_PEQ_PARAMETER24,\
    NVKEYID_PEQ_PARAMETER25,\
    NVKEYID_PEQ_PARAMETER26,\
    NVKEYID_PEQ_PARAMETER27,\
    NVKEYID_PEQ_PARAMETER28,\
    NVKEYID_APP_LED_PATTERN,\
    NVKEYID_AUDIO_DVFS_CLK_SETTING,\
    NVKEYID_APP_AUDIO_HW_IO_CONFIGURE,\
    NVKEYID_APP_VP_LANGUAGE_INDEX_0,\
    NVKEYID_APP_VP_LANGUAGE_INDEX_1,\
    NVKEYID_PROFILE_AWS_MCE_CONFIG,\
    NVKEYID_APP_AUDIO_CHANNEL,\
    NVKEYID_BT_MUSIC_AAC_ENABLE,\
    NVKEYID_BT_DEFAULT_TX_POWER,\
    NVKEYID_A2DP_3M_CONFIG,\
    NVKEYID_VA_CONFIG_INFO,\
    NVKEYID_BT_TX_PWR_BDR_GAIN_TABLE,\
    NVKEYID_BT_TX_PWR_EDR_GAIN_TABLE,\
    NVKEYID_XIAOAI_MI_PHONE_ADDR,\
    NVKEYID_XIAOAI_KEY_ADDR_MAPPING,\
    NVKEYID_XIAOAI_KEY_BROADCAST_COUNT,\
    NVKEYID_BT_DEFAULT_TX_POWER_EXTEND,\
    NVKEYID_APP_FAST_PAIR_PROTECTED,\
    NVKEYID_APP_FAST_PAIR_CONFIGURE,\
    NVKEYID_APP_GSOUND_INFO,\
    NVKEYID_BT_WB_SCAN_CALIB_PARAM,\
    NVKEYID_BT_MODEL_NAME,\
    NVKEYID_BT_AAC_CONFIG,\
    NVKEYID_BT_SBC_CONFIG,\
    NVKEY_DSP_PARA_WIND_DETECT,\
    NVKEY_DSP_PARA_USR_UNAWARE,\
    NVKEY_DSP_PARA_ENVIRONMENT_DETECTION,\
    NVKEY_DSP_PARA_ADAPTIVE_FF,\
    NVKEY_DSP_PARA_ADAPTIVE_FF_KEEP_FILTER,\
    NVKEYID_DSP_PARA_AFC,\
    NVKEYID_DSP_PARA_LD_NR_MISC,\
    NVKEYID_DSP_PARA_LD_NR_PARAMETER1,\
    NVKEYID_DSP_PARA_LD_NR_PARAMETER2,\
    NVKEYID_DSP_PARA_AT_AGC,\
    NVKEYID_DSP_PARA_AT_AGC_DRC,\
    NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_MISC,\
    NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_SPEACIL_PEQ,\
    NVKEYID_DSP_PARA_SILENCE_DETECTION,\
};


/**
* @brief      This function is to check which reset flag is set and do NVDM re-organization.
*/
void factory_rst_reserved_nvdm_item_list_check(void);


#endif

#ifdef __cplusplus
}
#endif

#endif /* __NVDM_CONFIG_FACTRST_H__ */
