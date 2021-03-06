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


#ifndef __NVKEY_ID_LIST_H__
#define __NVKEY_ID_LIST_H__

#if ((PRODUCT_VERSION == 1552) || (PRODUCT_VERSION == 2822) || defined(AM255X) || defined(AB156X) || defined(MT2822))
// The valid NVKEY range is from 0x0001 to 0xFFFF.

typedef enum
{
    NVKEYID_NVDM_VERSION                               = 0x1001,
    NVKEYID_PRODUCT_CATEGORY                           = 0x1002,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_01              = 0x1800,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_02              = 0x1801,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_03              = 0x1802,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_04              = 0x1803,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_05              = 0x1804,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_06              = 0x1805,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_07              = 0x1806,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_08              = 0x1807,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_09              = 0x1808,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_10              = 0x1809,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_11              = 0x180A,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_12              = 0x180B,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_13              = 0x180C,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_14              = 0x180D,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_15              = 0x180E,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_ID_MAX             = 0x180F,
    NVKEYID_BT_HOST_LINK_KEY_RECORD_SEQUENCE           = 0x1810,
    NVKEYID_BT_AIR_SPP_UUID_INDEX                      = 0x1813,
    NVKEYID_BT_AIR_SPP_UUID_DATA                       = 0x1814,
    NVKEYID_BT_ULL_PEER_ADDRESS                        = 0x1815,
    NVKEYID_BT_LE_AUDIO_CSIS_DATA                      = 0x1900,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_VCS_VOLUME                     = 0x1901,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_VOCS_DATA                      = 0x1902,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_AICS_GAIN_SETTING_PROPERTIES   = 0x1903,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_AICS_GAIN_SETTING              = 0x1904,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_SINK_LOCATION                  = 0x1905,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_SOURCE_LOCATION                = 0x1906,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_DESCRIPTION_STRING_1           = 0x1907,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_DESCRIPTION_STRING_2           = 0x1908,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_DESCRIPTION_STRING_3           = 0x1909,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_DESCRIPTION_STRING_4           = 0x190A,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_DESCRIPTION_STRING_5           = 0x190B,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_DESCRIPTION_STRING_6           = 0x190C,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_DESCRIPTION_STRING_7           = 0x190D,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_DESCRIPTION_STRING_8           = 0x190E,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_DESCRIPTION_STRING_9           = 0x190F,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_DESCRIPTION_STRING_10          = 0x1910,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_METADATA                       = 0x1911,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_ENABLE                         = 0x1912,    /* LE AUDIO NVKEY */
    NVKEYID_BT_LE_AUDIO_TEST_MODE                      = 0x1913,    /* LE AUDIO NVKEY */
    NVKEYID_MP_CAL_CHG_CONFIG                          = 0x2000,
    NVKEYID_BATTERY_MANAGEMENT_BASIC                   = 0x2001,
    NVKEYID_MP_CAL_INT_CHG_DAC_CONFIG                  = 0x2002,
    NVKEYID_MP_CAL_INT_CHG_TRICKLE_CURRENT_CONFIG      = 0x2003,
    NVKEYID_MP_CAL_INT_CHG_CC1_CURRENT_CONFIG          = 0x2004,
    NVKEYID_MP_CAL_INT_CHG_CC2_CURRENT_CONFIG          = 0x2005,
    NVKEYID_MP_CAL_CV_STOP_CURRENT_ADC                 = 0x2006,
    NVKEYID_MP_CAL_SYS_LDO                             = 0x2007,
    NVKEYID_MP_CAL_OCP                                 = 0x2008,
    NVKEYID_MP_CAL_JEITA                               = 0x2009,
    NVKEYID_MP_CAL_JEITA_WARM                          = 0x200A,
    NVKEYID_MP_CAL_JEITA_COOL                          = 0x200B,
    NVKEYID_MP_CAL_CHG_ADC_CONFIG                      = 0x200C,
    NVKEYID_MP_CAL_BUCK_MV_CONFIG                      = 0x2010,
    NVKEYID_MP_CAL_BUCK_MV_STB_CONFIG                  = 0x2011,
    NVKEYID_MP_CAL_BUCK_MV_FREQ                        = 0x2012,
    NVKEYID_MP_CAL_BUCK_LV_CONFIG                      = 0x2013,
    NVKEYID_MP_CAL_BUCK_LV_LPM                         = 0x2014,
    NVKEYID_MP_CAL_BUCK_LV_FREQ                        = 0x2015,
    NVKEYID_MP_CAL_LDO_VDD33_REG_CONFIG                = 0x2016,
    NVKEYID_MP_CAL_LDO_VDD33_REG_RET_CONFIG            = 0x2017,
    NVKEYID_MP_CAL_LDO_VDD33_RET_CONFIG                = 0x2018,
    NVKEYID_MP_CAL_LDO_VRF_REG_CONFIG                  = 0x2019,
    NVKEYID_MP_CAL_LDO_VRF_REG_RET_CONFIG              = 0x201A,
    NVKEYID_MP_CAL_LDO_VRF_RET_CONFIG                  = 0x201B,
    NVKEYID_MP_CAL_VDIG18_CONFIG                       = 0x201C,
    NVKEYID_MP_CAL_HPBG_CONFIG                         = 0x201D,
    NVKEYID_MP_CAL_LPBG_CONFIG                         = 0x201E,
    NVKEYID_MP_CAL_VBAT_ADC_CALIBRATION_TABLE          = 0x2020,
    NVKEYID_MP_CAL_VBAT_VOLTAGE_CONFIG                 = 0x2021,
    NVKEYID_BATTERY_MANAGEMENT_CHR                     = 0x2022,
    NVKEYID_BT_TX_POWER_GC                             = 0x2040,
    NVKEYID_MP_CAL_XO_26M_CRTSTAL_TRIM                 = 0x2045,
    NVKEYID_BR_DPC_VALUE_TABLE_MP_K                    = 0x2048,
    NVKEYID_EDR_DPC_VALUE_TABLE_MP_K                   = 0x2049,
    NVKEYID_BR_TX_GAIN_TABLE_MP_K                      = 0x204C,
    NVKEYID_EDR_TX_GAIN_TABLE_MP_K                     = 0x204D,
    NVKEYID_BT_PEC_CONFIG                              = 0x204E,
    NVKEYID_NTC_NTC_PARA_CONFIG1                       = 0x2100,
    NVKEYID_NTC_NTC_PARA_CONFIG2                       = 0x2101,
    NVKEYID_NTC_NTC_PARA_TOOL_TEMP_TABLE               = 0x2102,
    NVKEYID_NTC_NTC_PARA_TOOL_RESISTOR_TABLE           = 0x2103,
    NVKEYID_NTC_NTC_PARA_TEMP_TABLE                    = 0x2104,
    NVKEYID_NTC_NTC_PARA_RARIO_TABLE                   = 0x2105,
    NVKEYID_RF_MISC_PWR_CTL                            = 0x3720,
    NVKEYID_AWS_MISC_CTL                               = 0x3742,
    NVKEYID_LM_INFO_LMP_FEATURES_PAGE0                 = 0x3821,
    NVKEYID_LM_INFO_LMP_FEATURES_PAGE1                 = 0x3822,
    NVKEYID_LM_INFO_LMP_FEATURES_PAGE2                 = 0x3823,
    NVKEYID_BT_BLE_BLE_ADV_NAME                        = 0x3901,
    NVKEYID_USB_SETTING_0                              = 0x3B00,
    NVKEYID_USB_SETTING_1                              = 0x3B01,
    NVKEYID_USB_SETTING_2                              = 0x3B02,
    NVKEYID_USB_SETTING_3                              = 0x3B03,
    NVKEYID_USB_SETTING_4                              = 0x3B04,
    NVKEYID_AIRO_KEY_NUMS                              = 0xD000,
    NVKEYID_AIRO_KEY_1                                 = 0xD001,
    NVKEYID_AIRO_KEY_2                                 = 0xD002,
    NVKEYID_AIRO_KEY_3                                 = 0xD003,
    NVKEYID_AIRO_KEY_4                                 = 0xD004,
    NVKEYID_AIRO_KEY_5                                 = 0xD005,
    NVKEYID_AIRO_KEY_6                                 = 0xD006,
    NVKEYID_AIRO_KEY_7                                 = 0xD007,
    NVKEYID_AIRO_KEY_8                                 = 0xD008,
    NVKEYID_AIRO_KEY_9                                 = 0xD009,
    NVKEYID_AIRO_KEY_A                                 = 0xD00A,
    NVKEYID_AIRO_KEY_B                                 = 0xD00B,
    NVKEYID_AIRO_KEY_C                                 = 0xD00C,
    NVKEYID_AIRO_KEY_D                                 = 0xD00D,
    NVKEYID_AIRO_KEY_E                                 = 0xD00E,
    NVKEYID_AIRO_KEY_F                                 = 0xD00F,
    NVKEYID_CAPTOUCH_CALI_DATA                         = 0xD100,
    NVKEYID_CAPTOUCH_CALI_4CHDATA                      = 0xD101,
    NVKEYID_CAPTOUCH_SDWU_DATA                         = 0xD102,
    NVKEYID_CAPTOUCH_FEATURE                           = 0xD103,
    NVKEYID_CAPTOUCH_EARDETECT_DATA                    = 0xD104,
    NVKEYID_CAPTOUCH_FORCETOUCH_DATA                   = 0xD105,
    NVKEYID_CAPTOUCH_TUNE_DATA                         = 0xD106,
    NVKEYID_CAPTOUCH_AUTOSUSPEND_DATA                  = 0xD107,
    NVKEYID_DSP_FW_PARA_DATADUMP                       = 0xE001,
    NVKEYID_BT_MUSIC_LATENCY_SETTING                   = 0xE004,
    NVKEYID_DSP_FW_PARA_VOLUME_PARAMETERS              = 0xE005,
    NVKEYID_DSP_FW_PARA_VOW_PARAMETERS                 = 0xE006,
    NVKEYID_DSP_PARA_RESERVED                          = 0xE007,
    NVKEYID_DSP_FW_PARA_VOLUME_PARAMETERS_2            = 0xE008,
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_COMMON        = 0xE00A,
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_A2DP         = 0xE010,
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_LINE         = 0xE011,
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_SCO          = 0xE012,
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_VC           = 0xE013,
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_VP           = 0xE014,
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_RT           = 0xE015,
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_AT           = 0xE016,
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_SCO_NB       = 0xE017,
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_USB_AUDIO    = 0xE018,
    NVKEYID_DSP_FW_PARA_DIGITAL_GAINTABLE_MEDIAPLAYER  = 0xE019,
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_A2DP          = 0xE020,
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_LINE          = 0xE021,
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_SCO           = 0xE022,
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_VC            = 0xE023,
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_VP            = 0xE024,
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_RT            = 0xE025,
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_AT            = 0xE026,
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_SCO_NB        = 0xE027,
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_USB_AUDIO     = 0xE028,
    NVKEYID_DSP_FW_PARA_ANALOG_GAINTABLE_MEDIAPLAYER   = 0xE029,
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_A2DP             = 0xE030,
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_LINE             = 0xE031,
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_SCO              = 0xE032,
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_VC               = 0xE033,
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_VP               = 0xE034,
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_RT               = 0xE035,
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_AT               = 0xE036,
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_DETACH_MIC       = 0xE037,
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_A2DP             = 0xE040,
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_LINE             = 0xE041,
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_SCO              = 0xE042,
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_VC               = 0xE043,
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_VP               = 0xE044,
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_RT               = 0xE045,
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_AT               = 0xE046,
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_SCO_NB           = 0xE047,
    NVKEYID_DSP_FW_PARA_AIN_GAINTABLE_DETACH_MIC       = 0xE048,
    NVKEYID_DSP_FW_PARA_DIN_GP_TABLE_SCO               = 0xE050,
    NVKEYID_DSP_FW_PARA_DIN_GP_TABLE_SCO_NB            = 0xE051,
    NVKEYID_DSP_FW_PARA_DIN_GP_TABLE_AT                = 0xE052,
    NVKEYID_DSP_FW_PARA_DIN_GP_TABLE_SCO_MULTI_MIC     = 0xE056,
    NVKEYID_DSP_FW_PARA_DIN_GP_TABLE_A2DPSOURCE        = 0xE057,
    NVKEYID_DSP_FW_PARA_DIN_GP_TABLE_SYSTEM_SOFTWARE   = 0xE058,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_L             = 0xE060,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_R             = 0xE061,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_NB_L          = 0xE062,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_NB_R          = 0xE063,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_AT_L              = 0xE064,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_AT_R              = 0xE065,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_MULTI_MIC     = 0xE066,
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_WWE              = 0xE067,
    NVKEYID_DSP_FW_PARA_DIN_GAINTABLE_MIC_FUNC         = 0xE068,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_MIC2          = 0xE069,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_MIC3          = 0xE070,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_MIC4          = 0xE071,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_MIC5          = 0xE072,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_NB_MIC2       = 0xE073,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_NB_MIC3       = 0xE074,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_NB_MIC4       = 0xE075,
    NVKEYID_DSP_FW_PARA_AIN_GP_TABLE_SCO_NB_MIC5       = 0xE076,
    NVKEYID_DSP_ALG_PARA_DRC                           = 0xE100,
    NVKEYID_DSP_ALG_PARA_WB_TX_VO_CPD                  = 0xE101,
    NVKEYID_DSP_ALG_PARA_NB_TX_VO_CPD                  = 0xE102,
    NVKEYID_DSP_ALG_PARA_WB_RX_VO_CPD                  = 0xE103,
    NVKEYID_DSP_ALG_PARA_NB_RX_VO_CPD                  = 0xE104,
    NVKEYID_DSP_ALG_PARA_VP_CPD                        = 0xE105,
    NVKEYID_DSP_ALG_PARA_A2DP_AU_CPD                   = 0xE106,
    NVKEYID_DSP_ALG_PARA_LINE_AU_CPD                   = 0xE107,
    NVKEYID_DSP_ALG_PARA_POSITIVE_GAIN                 = 0xE109,
    NVKEYID_DSP_ALG_PARA_INS                           = 0xE110,
    NVKEYID_DSP_ALG_PARA_EFFECT                        = 0xE120,
    NVKEYID_DSP_ALG_PARA_VC                            = 0xE130,
    NVKEYID_DSP_ALG_PARA_PLC                           = 0xE140,
    NVKEYID_DSP_ALG_PARA_AEC_NR                        = 0xE150,
    NVKEYID_DSP_ALG_PARA_WB_RX_EQ                      = 0xE161,
    NVKEYID_DSP_ALG_PARA_WB_TX_EQ                      = 0xE162,
    NVKEYID_DSP_ALG_PARA_NB_RX_EQ                      = 0xE163,
    NVKEYID_DSP_ALG_PARA_NB_TX_EQ                      = 0xE164,
    NVKEYID_DSP_ALG_PARA_WB_RX_EQ_2ND                  = 0xE165,
    NVKEYID_DSP_ALG_PARA_NB_RX_EQ_2ND                  = 0xE166,
    NVKEYID_DSP_ALG_PARA_INEAR_EQ                      = 0xE167,
    NVKEYID_DSP_ALG_PARA_AST_EQ                        = 0xE168,
    NVKEYID_DSP_PARA_WB_TX_FIR_EQ                      = 0xE169,
    NVKEYID_DSP_PARA_NB_TX_FIR_EQ                      = 0xE16A,
    // NVKEYID_DSP_ALG_PARA_AT_MDSP                    = 0xE170,
    NVKEYID_DSP_PARA_ANC_L_FILTER1                     = 0xE180,
    NVKEYID_DSP_PARA_ANC_L_FILTER2                     = 0xE181,
    NVKEYID_DSP_PARA_ANC_L_FILTER3                     = 0xE182,
    NVKEYID_DSP_PARA_ANC_L_FILTER4                     = 0xE183,
    NVKEYID_DSP_PARA_ANC_R_FILTER1                     = 0xE184,
    NVKEYID_DSP_PARA_ANC_R_FILTER2                     = 0xE185,
    NVKEYID_DSP_PARA_ANC_R_FILTER3                     = 0xE186,
    NVKEYID_DSP_PARA_ANC_R_FILTER4                     = 0xE187,
    NVKEYID_DSP_PARA_ANC_L_FILTER5                     = 0xE188,
    NVKEYID_DSP_PARA_ANC_R_FILTER5                     = 0xE189,
    NVKEYID_DSP_PARA_ANC_SW_GAIN                       = 0xE18A,
    NVKEYID_DSP_PARA_ANC                               = 0xE18B,
    NVKEYID_DSP_PARA_ANC_MISC_PARA                     = 0xE18C,
    NVKEYID_DSP_PARA_PASS_THRU                         = 0xE18D,
    NVKEYID_DSP_PARA_LEAKAGE_COMPENSATION              = 0xE18E,
    NVKEYID_DSP_PARA_VOICE_WB_RX_AGC                   = 0xE190,
    NVKEYID_DSP_PARA_VOICE_NB_RX_AGC                   = 0xE191,
    NVKEYID_DSP_PARA_VOICE_TX_AGC                      = 0xE192,
    NVKEYID_DSP_PARA_AEC_NR_BOOMIC                       = 0xE193,
    NVKEYID_DSP_PARA_NB_TX_EQ_BOOMIC                     = 0xE194,
    NVKEYID_DSP_PARA_WB_TX_EQ_BOOMIC                     = 0xE195,
    NVKEYID_DSP_PARA_WB_TX_VO_CPD_BOOMIC                 = 0xE196,
    NVKEYID_DSP_PARA_WB_TX_FIR_EQ_BOOMIC               = 0xE197,
    NVKEYID_DSP_PARA_NB_TX_FIR_EQ_BOOMIC               = 0xE198,
    NVKEYID_DSP_PARA_SIDETONE_FIR_COEFFICIENT          = 0xE1A0,
    NVKEYID_DSP_PARA_ANC_L_FILTER6                     = 0xE1B8,
    NVKEYID_DSP_PARA_ANC_R_FILTER6                     = 0xE1B9,
    NVKEYID_DSP_PARA_ANC_L_FILTER7                     = 0xE1BA,
    NVKEYID_DSP_PARA_ANC_R_FILTER7                     = 0xE1BB,
    NVKEYID_DSP_PARA_ANC_L_FILTER_1                    = 0xE1C0,
    NVKEYID_DSP_PARA_ANC_R_FILTER_1                    = 0xE1C1,
    NVKEYID_DSP_PARA_ANC_L_FILTER_2                    = 0xE1C2,
    NVKEYID_DSP_PARA_ANC_R_FILTER_2                    = 0xE1C3,
    NVKEYID_DSP_PARA_ANC_L_FILTER_3                    = 0xE1C4,
    NVKEYID_DSP_PARA_ANC_R_FILTER_3                    = 0xE1C5,
    NVKEYID_DSP_PARA_ANC_L_FILTER_4                    = 0xE1C6,
    NVKEYID_DSP_PARA_ANC_R_FILTER_4                    = 0xE1C7,
    NVKEYID_DSP_PARA_ANC_PATH_SETTING                  = 0xE1CA,
    NVKEYID_DSP_PARA_PASSTHRU_L_FILTER_1               = 0xE1D0,
    NVKEYID_DSP_PARA_PASSTHRU_R_FILTER_1               = 0xE1D1,
    NVKEYID_DSP_PARA_PASSTHRU_L_FILTER_2               = 0xE1D2,
    NVKEYID_DSP_PARA_PASSTHRU_R_FILTER_2               = 0xE1D3,
    NVKEYID_DSP_PARA_PASSTHRU_L_FILTER_3               = 0xE1D4,
    NVKEYID_DSP_PARA_PASSTHRU_R_FILTER_3               = 0xE1D5,
    NVKEYID_DSP_PARA_ANC_CURRENT_STATUS                = 0xE1E0,
    NVKEYID_DSP_PARA_ANC_SETTING                       = 0xE1E1,
    NVKEYID_DSP_PARA_PASSTHRU_SETTING                  = 0xE1E2,
    NVKEYID_DSP_PARA_ANC_POWER_DETECT                  = 0xE1E3,
    NVKEYID_DSP_PARA_PASSTHRU_POWER_DETECT             = 0xE1E4,
    NVKEYID_DSP_PARA_ANC_CALIBRATE_GAIN                = 0xE1E5,
    NVKEYID_DSP_PARA_ANC_DEFAULT_STATUS                = 0xE1E6,
    NVKEYID_DSP_PARA_ANC_MULTI_FEATURE                 = 0xE1F0,
    NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_PRE_PEQ_1    = 0xE230,
    NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_PRE_PEQ_2    = 0xE231,
    NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_PRE_PEQ_3    = 0xE232,
    NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_PRE_PEQ_4    = 0xE233,
    NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_PRE_PEQ_TABLE= 0xE390,
    NVKEYID_DSP_A2DP_PEQ_0                             = 0xEE05,
    NVKEYID_DSP_A2DP_PEQ_1                             = 0xEE06,
    NVKEYID_DSP_A2DP_PEQ_2                             = 0xEE07,
    NVKEYID_DSP_A2DP_PEQ_3                             = 0xEE08,
    NVKEYID_DSP_A2DP_PEQ_4                             = 0xEE09,
    NVKEYID_DSP_A2DP_PEQ_5                             = 0xEE0A,
    NVKEYID_DSP_A2DP_PEQ_6                             = 0xEE0B,
    NVKEYID_DSP_A2DP_PEQ_7                             = 0xEE0C,
    NVKEYID_DSP_A2DP_PEQ_8                             = 0xEE0D,
    NVKEYID_DSP_A2DP_PEQ_9                             = 0xEE0E,
    NVKEYID_DSP_A2DP_PEQ_10                            = 0xEE0F,
    NVKEYID_DSP_A2DP_PEQ_11                            = 0xEE13,
    NVKEYID_DSP_A2DP_PEQ_12                            = 0xEE14,
    NVKEYID_DSP_A2DP_PEQ_13                            = 0xEE15,
    NVKEYID_DSP_A2DP_PEQ_14                            = 0xEE16,
    NVKEYID_DSP_A2DP_PEQ_15                            = 0xEE17,
    NVKEYID_DSP_A2DP_PEQ_16                            = 0xEE18,
    NVKEYID_DSP_A2DP_PEQ_17                            = 0xEE19,
    NVKEYID_DSP_A2DP_PEQ_18                            = 0xEE1A,
    NVKEYID_DSP_A2DP_PEQ_19                            = 0xEE1B,
    NVKEYID_DSP_A2DP_PEQ_20                            = 0xEE1C,
    NVKEYID_DSP_A2DP_PEQ_21                            = 0xEE1D,
    NVKEYID_DSP_A2DP_PEQ_22                            = 0xEE1E,
    NVKEYID_DSP_A2DP_PEQ_23                            = 0xEE1F,
    NVKEYID_DSP_A2DP_PEQ_24                            = 0xEE21,
    NVKEYID_DSP_A2DP_PEQ_25                            = 0xEE22,
    NVKEYID_DSP_A2DP_PEQ_26                            = 0xEE23,
    NVKEYID_DSP_A2DP_PEQ_27                            = 0xEE24,
    NVKEYID_PEQ_UI_DATA_01                             = 0xEF00,
    NVKEYID_PEQ_UI_DATA_02                             = 0xEF01,
    NVKEYID_PEQ_UI_DATA_03                             = 0xEF02,
    NVKEYID_PEQ_UI_DATA_04                             = 0xEF03,
    NVKEYID_PEQ_EXTENDED_UI_DATA_01                    = 0xEF10,
    NVKEYID_PEQ_EXTENDED_UI_DATA_02                    = 0xEF11,
    NVKEYID_PEQ_EXTENDED_UI_DATA_03                    = 0xEF12,
    NVKEYID_PEQ_EXTENDED_UI_DATA_04                    = 0xEF13,
    NVKEYID_LOG_SETTING                                = 0xF007,
    NVKEYID_1WIRE_LOGGING_SETTING                      = 0xF023,
#if defined(AB156X) || (PRODUCT_VERSION == 2822)
    NVKEYID_EXCEPTION_DUMP_MODE                        = 0xF080,
#endif
    NVKEYID_APP_DEVICE_NAME_DEFAULT                    = 0xF202,
    NVKEYID_APP_DEVICE_NAME_USER_DEFINED               = 0xF203,
    NVKEYID_APP_VP_LANG                                = 0xF230,
    NVKEYID_PEQ_AUDIO_MISC_PARA                        = 0xF232,
    NVKEYID_PEQ_AUDIO_PATH_TABLE                       = 0xF233,
    NVKEYID_PEQ_GROUP_TO_PARA_TABLE1                   = 0xF234,
    NVKEYID_PEQ_GROUP_TO_PARA_TABLE2                   = 0xF235,
    NVKEYID_PEQ_GROUP_TO_PARA_TABLE3                   = 0xF236,
    NVKEYID_PEQ_GROUP_TO_PARA_TABLE4                   = 0xF237,
    NVKEYID_PEQ_GROUP_TO_PARA_TABLE5                   = 0xF238,
    NVKEYID_PEQ_GROUP_TO_PARA_TABLE6                   = 0xF239,
    NVKEYID_APP_VOLUME_SCO_SETTING                     = 0xF23A,
    NVKEYID_APP_VOLUME_A2DP_SETTING                    = 0xF23B,
    NVKEYID_APP_VOLUME_MP3_SETTING                     = 0xF23C,
    NVKEYID_APP_VOLUME_SCOMIC_SETTING                  = 0xF23D,
    NVKEYID_APP_VOLUME_VPRT_SETTING                    = 0xF23E,
    NVKEYID_APP_VOLUME_LINEIN_SETTING                  = 0xF23F,
    NVKEYID_APP_VOLUME_USB_AUDIO_SETTING               = 0xF240,
    NVKEYID_APP_VOLUME_MEDIAPLAYER_SETTING             = 0xF241,
    NVKEYID_APP_VOLUME_A2DPSOURCE_SETTING              = 0xF243,
    NVKEYID_APP_VOLUME_USB_AUDIO_SW_SETTING            = 0xF244,
    NVKEYID_APP_VOLUME_USB_VOICE_SW_SETTING            = 0xF245,
    // AiroThru Sound Level Setting                    = 0xF250,
    NVKEYID_PEQ_PARAMETER1                             = 0xF260,
    NVKEYID_PEQ_PARAMETER2                             = 0xF261,
    NVKEYID_PEQ_PARAMETER3                             = 0xF262,
    NVKEYID_PEQ_PARAMETER4                             = 0xF263,
    NVKEYID_PEQ_PARAMETER5                             = 0xF264,
    NVKEYID_PEQ_PARAMETER6                             = 0xF265,
    NVKEYID_PEQ_PARAMETER7                             = 0xF266,
    NVKEYID_PEQ_PARAMETER8                             = 0xF267,
    NVKEYID_PEQ_PARAMETER9                             = 0xF268,
    NVKEYID_PEQ_PARAMETER10                            = 0xF269,
    NVKEYID_PEQ_PARAMETER11                            = 0xF26A,
    NVKEYID_PEQ_PARAMETER12                            = 0xF26B,
    NVKEYID_PEQ_PARAMETER13                            = 0xF26C,
    NVKEYID_PEQ_PARAMETER14                            = 0xF26D,
    NVKEYID_PEQ_PARAMETER15                            = 0xF26E,
    NVKEYID_PEQ_PARAMETER16                            = 0xF26F,
    NVKEYID_PEQ_PARAMETER17                            = 0xF270,
    NVKEYID_PEQ_PARAMETER18                            = 0xF271,
    NVKEYID_PEQ_PARAMETER19                            = 0xF272,
    NVKEYID_PEQ_PARAMETER20                            = 0xF273,
    NVKEYID_PEQ_PARAMETER21                            = 0xF274,
    NVKEYID_PEQ_PARAMETER22                            = 0xF275,
    NVKEYID_PEQ_PARAMETER23                            = 0xF276,
    NVKEYID_PEQ_PARAMETER24                            = 0xF277,
    NVKEYID_PEQ_PARAMETER25                            = 0xF278,
    NVKEYID_PEQ_PARAMETER26                            = 0xF279,
    NVKEYID_PEQ_PARAMETER27                            = 0xF27A,
    NVKEYID_PEQ_PARAMETER28                            = 0xF27B,
    NVKEYID_PEQ_PARAMETER29                            = 0xF27C,
    NVKEYID_PEQ_PARAMETER30                            = 0xF27D,
    NVKEYID_PEQ_PARAMETER31                            = 0xF27E,
    NVKEYID_PEQ_PARAMETER32                            = 0xF27F,
    NVKEYID_APP_LED_PATTERN                            = 0xF280,
    NVKEYID_AUDIO_DVFS_CLK_SETTING                     = 0xF283,
    NVKEYID_APP_AUDIO_HW_IO_CONFIGURE                  = 0xF28F,
    NVKEYID_APP_VP_LANGUAGE_INDEX_0                    = 0xF292,
    NVKEYID_APP_VP_LANGUAGE_INDEX_1                    = 0xF293,
    NVKEYID_PROFILE_AWS_MCE_CONFIG                     = 0xF2B0,
    NVKEYID_APP_AUDIO_CHANNEL                          = 0xF2B5,
    NVKEYID_ENABLE_MP_TEST_MODE                        = 0xF2C4,
    NVKEYID_APP_VP_SEL_PARA                            = 0xF2DA,
    NVKEYID_APP_FAST_PAIR                              = 0xF2E0,
    NVKEYID_BT_MUSIC_AAC_ENABLE                        = 0xF2E1,
    NVKEYID_BT_RELAY_ENABLE                            = 0xF2E2,
    NVKEYID_BT_DUT_ENABLE                              = 0xF2E3,
    NVKEYID_BT_DEFAULT_TX_POWER                        = 0xF2E4,
    NVKEYID_SYSTEM_MODE_SETTING                        = 0xF2E5,
    NVKEYID_A2DP_3M_CONFIG                             = 0xF2E6,
    NVKEYID_CONFIGURABLE_KEY_ACTION_TABLE              = 0xF2E7,
    NVKEYID_VA_DRIVER_VARIATION                        = 0xF2E8,
    NVKEYID_VA_CONFIG_INFO                             = 0xF2E9,
    NVKEYID_VA_SWITH                                   = 0xF2EA,
    NVKEYID_BT_TX_PWR_BDR_GAIN_TABLE                   = 0xF2EB,
    NVKEYID_BT_TX_PWR_EDR_GAIN_TABLE                   = 0xF2EC,
    NVKEYID_XIAOWEI_PROPER_VALUE                       = 0xF2ED,
    NVKEYID_XIAOAI_MI_PHONE_ADDR                       = 0xF2EE,
    NVKEYID_PEER_CONFIGURABLE_KEY_ACTION_TABLE         = 0xF2EF,
    NVKEYID_XIAOAI_KEY_ADDR_MAPPING                    = 0xF2F0,
    NVKEYID_XIAOAI_KEY_VIDPID                          = 0xF2F1,
    NVKEYID_XIAOAI_KEY_BROADCAST_COUNT                 = 0xF2F2,
    NVKEYID_XIAOAI_KEY_CONFIG_INFO                     = 0xF2F3,
    NVKEYID_BT_DEFAULT_TX_POWER_EXTEND                 = 0xF300,
    NVKEYID_APP_FAST_PAIR_PROTECTED                    = 0xF301,
    NVKEYID_APP_FAST_PAIR_CONFIGURE                    = 0xF302,
    NVKEYID_APP_GSOUND_INFO                            = 0xF303,
    NVKEYID_APP_EASY_PAIR                              = 0xF304,
    NVKEYID_APP_LE_AUDIO_CONNECTED_FLAG                = 0xF305,
    NVKEYID_APP_IN_EAR_MUSIC_ABILITY                   = 0xF306,
    NVKEYID_APP_RACE_CMD_TOUCH_KEY_ENABLE              = 0xF307,
    NVKEYID_APP_POWER_SAVING_CFG                       = 0xF308,
    NVKEYID_APP_IN_EAR_HFP_ABILITY                     = 0xF309,
    NVKEYID_APP_ULL_CONFIG                             = 0xF30A,
    NVKEYID_BT_WB_SCAN_CALIB_PARAM                     = 0xF310,
    NVKEYID_APP_MULTI_POINT_ENABLE                     = 0xF311,
    NVKEYID_APP_CASE_BATTERY_INFO                      = 0xF312,
    NVKEYID_APP_LINE_IN_SCENARIO_TYPE                  = 0xF313,
    NVKEYID_APP_USB_AUDIO_SCENARIO_TYPE                = 0xF314,
    NVKEYID_SPOT_EIK                                   = 0xF315,
    NVKEYID_SPOT_EID_COUNTER                           = 0xF316,
    NVKEYID_APP_ANC_GAIN_CONTROL_STATUS                = 0xF317,
    NVKEYID_APP_USB_LOGGING_MODE                       = 0xF319,
    NVKEYID_APP_XBOX_VOL_INFO                          = 0xF31B,
    NVKEYID_APP_ADVANCE_PT                             = 0xF31C,
    NVKEYID_APP_ULL_MISC                               = 0xF31D,
    NVKEYID_BT_MODEL_NAME                              = 0xF500,
    NVKEYID_BT_AAC_CONFIG                              = 0xF501,
    NVKEYID_BT_SBC_CONFIG                              = 0xF502,
    NVKEYID_DSP_PARA_AFC                               = 0xF504,
    NVKEYID_DSP_PARA_LD_NR_MISC                        = 0xF505,
    NVKEYID_DSP_PARA_LD_NR_PARAMETER1                  = 0xF506,
    NVKEYID_DSP_PARA_LD_NR_PARAMETER2                  = 0xF507,
    NVKEYID_DSP_PARA_AT_AGC                            = 0xF508,
    NVKEYID_DSP_PARA_AT_AGC_DRC                        = 0xF509,
    NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_MISC         = 0xF50A,
    NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_SPEACIL_PEQ  = 0xF50B,
    NVKEYID_DSP_PARA_GAME_CHAT_VOLUME_SMART_BALANCE    = 0xF50C,
    NVKEYID_DSP_PARA_SILENCE_DETECTION                   = 0xF50D,
} NVKey_ID_list;


/* To avoid compatibility problem, define the old name to the new name. */
#define NVKEYID_INTERNAL_CHIP_NAME                      NVKEYID_PRODUCT_CATEGORY

#endif /* #if (PRODUCT_VERSION == 1552) */

#endif /* __NVKEY_ID_LIST_H__ */
