/* Copyright Statement:
 *
 * (C) 2018  Airoha Technology Corp. All rights reserved.
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
#ifndef __HAL_PMU_CALI_H__
#define __HAL_PMU_CALI_H__
#ifdef HAL_PMU_MODULE_ENABLED

//#include "config.h"
//#include "log.h"

#define DEFAULT_TAG 0
#define MP_K        1
#define FT_TOOL_K   2
#define FT_FW_K     3

#define VBAT_CHECK_POINT_NUM   18

#define OTP_BASE_ADDR                 (0x0100)
#define OTP_VBAT_ADDR                 (OTP_BASE_ADDR+0x0000)
#define OTP_BUCK_MV_ADDR              (OTP_BASE_ADDR+0x000D)
#define OTP_BUCK_MV_FREQ_ADDR         (OTP_BASE_ADDR+0x0014)
#define OTP_BUCK_MV_RET_ADDR          (OTP_BASE_ADDR+0x0019)
#define OTP_BUCK_LV_ADDR              (OTP_BASE_ADDR+0x0020)
#define OTP_BUCK_LV_FREQ_ADDR         (OTP_BASE_ADDR+0x0027)
#define OTP_LDO_VDIG18_ADDR           (OTP_BASE_ADDR+0x002C)
#define OTP_LDO_VRF_REG_ADDR          (OTP_BASE_ADDR+0x0033)
#define OTP_LDO_VRF_RET_ADDR          (OTP_BASE_ADDR+0x003A)
#define OTP_LDO_VDD33_REG_ADDR        (OTP_BASE_ADDR+0x0041)
#define OTP_LDO_VDD33_RET_ADDR        (OTP_BASE_ADDR+0x0048)
#define OTP_HPBG_ADDR                 (OTP_BASE_ADDR+0x004F)
#define OTP_LPBG_ADDR                 (OTP_BASE_ADDR+0x0056)
#define OTP_CHG_DAC_ADDR              (OTP_BASE_ADDR+0x005D)
#define OTP_VSYS_LDO_ADDR             (OTP_BASE_ADDR+0x0064)
#define OTP_OCP_ADDR                  (OTP_BASE_ADDR+0x006B)
#define OTP_CHG_4V2_CURRENT_ADDR      (OTP_BASE_ADDR+0x006E)
#define OTP_CHG_4V35_CURRENT_ADDR     (OTP_BASE_ADDR+0x007C)
#define OTP_VICHG_ADC_VALUE_ADDR      (OTP_BASE_ADDR+0x008A)
#define OTP_ADIE_VERSION_ADDR         (OTP_BASE_ADDR+0x0091)
#define OTP_ADIE_KGD_ADDR             (OTP_BASE_ADDR+0x0093)


#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif

typedef enum {
    bat_4p2v,
    bat_4p35v,
    bat_4p05v,
    bat_4p10v,
    bat_4p15v,
    bat_4p25v,
    bat_4p3v,
    bat_4p4v,
    bat_4p45v,
    bat_4p5v,
    bat_max,
} bat_volt_sel_t;

typedef struct vbat_slope_t
{
    uint16_t volt1;
    uint16_t adc1;
    uint16_t volt2;
    uint16_t adc2;
    uint8_t voltage_sel;
    uint8_t two_step_sel;
}VBAT_SLOPE_CONFIG;

typedef struct dac_slope_t
{
    uint16_t volt1;
    uint16_t dac1;
    uint16_t volt2;
    uint16_t dac2;
}DAC_SLOPE_CONFIG;

typedef struct curr_slope_t
{
    uint16_t curr1;
    uint16_t val1;
    uint16_t curr2;
    uint16_t val2;
    uint16_t chg_cc_curr;
}CURR_SLOPE_CONFIG;

typedef struct chg_cc_curr_t
{
    uint16_t cc1_curr;
    uint16_t cc2_curr;
}CHG_CC_CURR_CONFIG;

typedef struct vichg_adc_t
{
    uint16_t ADC_4_35V;
    uint16_t ADC_4_2V;
    uint16_t CV_stop_current_percent;
}VICHG_ADC_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2000
  */
/*NvkeyDefine NVKEYID_MP_CAL_CHG_CONFIG*/
typedef struct chg_config_t
{
    uint8_t intext_chg_config;
    uint8_t bat_volt_sel;
    uint8_t two_step_sel;
    uint8_t kflag;
}CHG_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x200C for AB1565
  */
/*NvkeyDefine NVKEYID_MP_CAL_CHG_ADC_CONFIG*/
typedef struct chg_adc_config_t
{
    uint16_t cc1_threshold_voltage;
    uint16_t cc1_threshold_ADC;
    uint16_t cc1_thd_voltage_offset;
    uint16_t cc1_thd_ADC_offset;
    uint16_t cc2_threshold_voltage;
    uint16_t cc2_threshold_ADC;
    uint16_t cc2_offset_voltage;
    uint16_t cc2_offset_ADC;
    uint16_t cv_threshold_voltage;
    uint16_t cv_threshold_ADC;
    uint16_t cv_thd_voltage_offset;
    uint16_t cv_thd_ADC_offset;
    uint16_t full_bat_voltage;
    uint16_t full_bat_ADC;
    uint16_t full_bat_voltage_offset;
    uint16_t full_bat_ADC_offset;
    uint16_t recharge_voltage;
    uint16_t recharge_ADC;
    uint16_t recharge_voltage_offset;
    uint16_t recharge_ADC_offset;
    uint16_t full_bat_voltage_2;       //new
    uint16_t full_bat_ADC_2;           //new
    uint16_t full_bat_voltage_2_offset;//new
    uint16_t full_bat_ADC_2_offset;    //new
}CHG_ADC_CONFIG;


/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2002
  */
/*NvkeyDefine NVKEYID_MP_CAL_INT_CHG_DAC_CONFIG*/
typedef struct int_chg_dac_config
{
    uint16_t tricklecurrentDAC;
    uint16_t tricklecurrentDACoffset;
    uint16_t cc1currentDAC;
    uint16_t cc1currentDACoffset;
    uint16_t cc2currentDAC;
    uint16_t cc2currentDACoffset;
    uint16_t cvDAC;
    uint16_t cvDACoffset;
}INT_CHG_DAC_CONFIG;

typedef struct volt_adc_t
{
    uint16_t volt;
    uint16_t adc;
}PACKED VOLTAGE_ADC_STRU;

typedef struct cc_curr_t
{
    uint16_t current;
    uint16_t rchg_sel;
}PACKED CCCURRENT_STRU;
/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2003
  */
/*NvkeyDefine NVKEYID_MP_CAL_INT_CHG_TRICKLE_CURRENT_CONFIG*/
typedef struct int_chg_trickle_current_config
{
    uint8_t cal_cnt;
    uint8_t select;
    CCCURRENT_STRU data[10];
}INT_CHG_TRICKLE_CURRENT_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2004
  */
/*NvkeyDefine NVKEYID_MP_CAL_INT_CHG_CC1_CURRENT_CONFIG*/
typedef struct int_chg_cc1_current_config
{
    uint8_t cal_cnt;
    uint8_t select;
    CCCURRENT_STRU data[10];
}INT_CHG_CC1_CURRENT_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2005
  */
/*NvkeyDefine NVKEYID_MP_CAL_INT_CHG_CC2_CURRENT_CONFIG*/
typedef struct int_chg_cc2_current_config
{
    uint8_t cal_cnt;
    uint8_t select;
    CCCURRENT_STRU data[10];
}INT_CHG_CC2_CURRENT_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2006
  */
/*NvkeyDefine NVKEYID_MP_CAL_CV_STOP_CURRENT_ADC*/
typedef struct chg_cv_stop_current_adc
{
    uint16_t CV_stop_current_percent;
    uint16_t CV_stop_current_ADC;
}CHG_CV_STOP_CURRENT_ADC;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2007
  */
/*NvkeyDefine NVKEYID_MP_CAL_SYS_LDO*/
typedef struct chg_sys_ldo
{
    uint16_t SYSLDO_output_voltage;
    uint16_t CHG_LDO_SEL;
}CHG_SYS_LDO;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2008
  */
/*NvkeyDefine NVKEYID_MP_CAL_OCP*/
typedef struct chg_ocp
{
    uint16_t SW_OC_LMT;
    uint16_t I_LIM_TRIM;
}CHG_OCP;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2009
  */
/*NvkeyDefine NVKEYID_MP_CAL_JEITA*/
typedef struct jeita
{
    uint16_t Cool_State_Current_Percent;
    uint16_t Cool_State_DAC_decrease;
    uint16_t Warm_State_Current_Percent;
    uint16_t Warm_State_DAC_decrease;
}JEITA;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x200A
  */
/*NvkeyDefine NVKEYID_MP_CAL_JEITA_WARM*/
typedef struct jeita_warm
{
    uint16_t JEITA_warm_cc2_threshold_voltage;
    uint16_t JEITA_warm_cc2_threshold_ADC;
    uint16_t JEITA_warm_cv_threshold_voltage;
    uint16_t JEITA_warm_cv_threshold_ADC;
    uint16_t JEITA_warm_full_bat_voltage;
    uint16_t JEITA_warm_full_bat_ADC;
    uint16_t JEITA_warm_recharge_voltage;
    uint16_t JEITA_warm_recharge_ADC;
    uint16_t JEITA_warm_full_bat_voltage_2;
    uint16_t JEITA_warm_full_bat_ADC_2;
    uint16_t JEITA_warm_cc1_current_DAC;
    uint16_t JEITA_warm_cc2_current_DAC;
    uint16_t JEITA_warm_cv_DAC;
    uint16_t JEITA_warm_cc1_current_1;
    uint16_t JEITA_warm_rchg_sel_cc1_1;
    uint16_t JEITA_warm_cc2_current_1;
    uint16_t JEITA_warm_rchg_sel_cc2_1;
    uint16_t JEITA_warm_CV_stop_current_ADC;
}JEITA_WARM;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x200B
  */
/*NvkeyDefine NVKEYID_MP_CAL_JEITA_COOL*/
typedef struct jeita_cool
{
    uint16_t JEITA_cool_cc2_threshold_voltage;
    uint16_t JEITA_cool_cc2_threshold_ADC;
    uint16_t JEITA_cool_cv_threshold_voltage;
    uint16_t JEITA_cool_cv_threshold_ADC;
    uint16_t JEITA_cool_full_bat_voltage;
    uint16_t JEITA_cool_full_bat_ADC;
    uint16_t JEITA_cool_recharge_voltage;
    uint16_t JEITA_cool_recharge_ADC;
    uint16_t JEITA_cool_full_bat_voltage_2;
    uint16_t JEITA_cool_full_bat_ADC_2;
    uint16_t JEITA_cool_cc1_current_DAC;
    uint16_t JEITA_cool_cc2_current_DAC;
    uint16_t JEITA_cool_cv_DAC;
    uint16_t JEITA_cool_cc1_current_1;
    uint16_t JEITA_cool_rchg_sel_cc1_1;
    uint16_t JEITA_cool_cc2_current_1;
    uint16_t JEITA_cool_rchg_sel_cc2_1;
    uint16_t JEITA_cool_CV_stop_current_ADC;
}JEITA_COOL;

typedef struct buck_val_stru
{
    uint16_t volt;
    uint8_t value;
    uint16_t adc;
}PACKED BUCK_VAL_STRU;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2010
  */
/*NvkeyDefine NVKEYID_MP_CAL_BUCK_MV_CONFIG*/
typedef struct buck_mv_config
{
    uint8_t kflag;
    uint8_t sloep_value1;
    uint16_t adc1;
    uint8_t sloep_value2;
    uint16_t adc2;
    uint8_t cal_cnt;
    uint8_t volselect;
    BUCK_VAL_STRU data[8];
}PACKED BUCK_MV_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2011
  */
/*NvkeyDefine NVKEYID_MP_CAL_BUCK_MV_STB_CONFIG*/
typedef struct buck_mv_stb_config
{
    uint8_t kflag;
    uint8_t sloep_value1;
    uint16_t adc1;
    uint8_t sloep_value2;
    uint16_t adc2;
    uint8_t cal_cnt;
    uint8_t volselect;
    BUCK_VAL_STRU data[8];
}PACKED BUCK_MV_STB_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2012
  */
/*NvkeyDefine NVKEYID_MP_CAL_BUCK_MV_FREQ*/
typedef struct buck_mv_freq
{
    uint8_t kflag;
    uint8_t BUCK_FREQ_MV;
    uint8_t OSC_FREQK_MV;
}PACKED BUCK_MV_FREQ;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2013
  */
/*NvkeyDefine NVKEYID_MP_CAL_BUCK_LV_CONFIG*/
typedef struct buck_lv_config
{
    uint8_t kflag;
    uint8_t sloep_value1;
    uint16_t adc1;
    uint8_t sloep_value2;
    uint16_t adc2;
    uint8_t cal_cnt;
    uint8_t volselect;
    BUCK_VAL_STRU data[8];
}PACKED BUCK_LV_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2014
  */
/*NvkeyDefine NVKEYID_MP_CAL_BUCK_LV_LPM*/
typedef struct buck_lv_lpm
{
    uint8_t kflag;
    uint8_t sloep_value1;
    uint16_t adc1;
    uint8_t sloep_value2;
    uint16_t adc2;
    uint8_t cal_cnt;
    uint8_t volselect;
    BUCK_VAL_STRU data[8];
}PACKED BUCK_LV_LPM;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2015
  */
/*NvkeyDefine NVKEYID_MP_CAL_BUCK_LV_FREQ*/
typedef struct buck_lv_freq
{
    uint8_t kflag;
    uint8_t BUCK_FREQ_LV;
    uint8_t OSC_FREQK_LV;
}PACKED BUCK_LV_FREQ;

typedef struct ldo_val_stru
{
    uint16_t volt;
    uint8_t value;
    uint16_t adc;
}PACKED LDO_VAL_STRU;
/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2016
  */
/*NvkeyDefine NVKEYID_MP_CAL_LDO_VDD33_REG_CONFIG*/
typedef struct ldo_vdd33_reg_config
{
    uint8_t kflag;
    uint8_t sloep_value1;
    uint16_t adc1;
    uint8_t sloep_value2;
    uint16_t adc2;
    uint8_t cal_cnt;
    uint8_t volselect;
    LDO_VAL_STRU data[8];
}PACKED LDO_VDD33_REG_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2017
  */
/*NvkeyDefine NVKEYID_MP_CAL_LDO_VDD33_REG_RET_CONFIG*/
typedef struct ldo_vdd33_reg_ret_config
{
    uint8_t kflag;
    uint8_t sloep_value1;
    uint16_t adc1;
    uint8_t sloep_value2;
    uint16_t adc2;
    uint8_t cal_cnt;
    uint8_t volselect;
    LDO_VAL_STRU data[8];
}PACKED LDO_VDD33_REG_RET_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2018
  */
/*NvkeyDefine NVKEYID_MP_CAL_LDO_VDD33_RET_CONFIG*/
typedef struct ldo_vdd33_ret_config
{
    uint8_t kflag;
    uint8_t sloep_value1;
    uint16_t adc1;
    uint8_t sloep_value2;
    uint16_t adc2;
    uint8_t cal_cnt;
    uint8_t volselect;
    LDO_VAL_STRU data[8];
}PACKED LDO_VDD33_RET_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2019
  */
/*NvkeyDefine NVKEYID_MP_CAL_LDO_VRF_REG_CONFIG*/
typedef struct ldo_vrf_reg_config
{
    uint8_t kflag;
    uint8_t sloep_value1;
    uint16_t adc1;
    uint8_t sloep_value2;
    uint16_t adc2;
    uint8_t cal_cnt;
    uint8_t volselect;
    LDO_VAL_STRU data[8];
}PACKED LDO_VRF_REG_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x201A
  */
/*NvkeyDefine NVKEYID_MP_CAL_LDO_VRF_REG_RET_CONFIG*/
typedef struct ldo_vrf_reg_ret_config
{
    uint8_t kflag;
    uint8_t sloep_value1;
    uint16_t adc1;
    uint8_t sloep_value2;
    uint16_t adc2;
    uint8_t cal_cnt;
    uint8_t volselect;
    LDO_VAL_STRU data[8];
}PACKED LDO_VRF_REG_RET_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x201B
  */
/*NvkeyDefine NVKEYID_MP_CAL_LDO_VRF_RET_CONFIG*/
typedef struct ldo_vrf_ret_config
{
    uint8_t kflag;
    uint8_t sloep_value1;
    uint16_t adc1;
    uint8_t sloep_value2;
    uint16_t adc2;
    uint8_t cal_cnt;
    uint8_t volselect;
    LDO_VAL_STRU data[8];
}PACKED LDO_VRF_RET_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x201C
  */
/*NvkeyDefine NVKEYID_MP_CAL_VDIG18_CONFIG*/
typedef struct ldo_vdig18_config
{
    uint8_t kflag;
    uint8_t sloep_value1;
    uint16_t adc1;
    uint8_t sloep_value2;
    uint16_t adc2;
    uint8_t cal_cnt;
    uint8_t volselect;
    LDO_VAL_STRU data[8];
}PACKED LDO_VDIG18_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x201D
  */
/*NvkeyDefine NVKEYID_MP_CAL_HPBG_CONFIG*/
typedef struct hpbg_config
{
    uint8_t kflag;
    uint8_t sloep_value1;
    uint16_t adc1;
    uint8_t sloep_value2;
    uint16_t adc2;
    uint8_t cal_cnt;
    uint8_t volselect;
    LDO_VAL_STRU data[2];
}PACKED HPBG_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x201E
  */
/*NvkeyDefine NVKEYID_MP_CAL_LPBG_CONFIG*/
typedef struct lpbg_config
{
    uint8_t kflag;
    uint8_t sloep_value1;
    uint16_t adc1;
    uint8_t sloep_value2;
    uint16_t adc2;
    uint8_t cal_cnt;
    uint8_t volselect;
    LDO_VAL_STRU data[2];
}PACKED LPBG_CONFIG;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2020
  */
/*NvkeyDefine NVKEYID_MP_CAL_VBAT_ADC_CALIBRATION_TABLE*/
typedef struct vbatadc_calibrationtable
{
    uint8_t kflag;
    uint8_t cal_cnt;
    VOLTAGE_ADC_STRU data[10];
}PACKED VBAT_ADC_CALIBRATION_TABLE;

typedef struct check_point_adc
{
    uint16_t check_point;
    uint16_t adc;
}CHECK_POINT_ADC;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2021
  */
/*NvkeyDefine NVKEYID_MP_CAL_VBAT_VOLTAGE_CONFIG*/
typedef struct vbat_voltage_config
{
    uint8_t kflag;
    uint16_t initial_bat;
    uint16_t initial_bat_adc;
    uint16_t low_bat;
    uint16_t low_bat_adc;
    uint16_t shutdown_bat;
    uint16_t shutdown_bat_adc;
    CHECK_POINT_ADC data[VBAT_CHECK_POINT_NUM];
}PACKED VBAT_VOLTAGE_CONFIG;

/**********************************************modify later*************************************/
/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2045
  */
/*NvkeyDefine NVKEYID_MP_CAL_XO_24M_CRTSTAL_TRIM*/
typedef struct xo24m_crystal_trim
{
    uint8_t crtstal_trim_spec;
    uint16_t cap_val;
}PACKED XO24M_CRYSTAL_TRIM;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2046
  */
/*NvkeyDefine NVKEYID_MP_CAL_XO_32K_CRTSTAL_TRIM*/
typedef struct x32kcrystal_trim
{
    uint8_t crtstal_trim_spec;
    uint8_t cap_val;
}PACKED XO32k_CRYSTAL_TRIM;

/**
  *  @brief Use brief, otherwise the index won't have a brief explanation.
  *  @KeyID 0x2047
  */
/*NvkeyDefine NVKEYID_MP_CAL_GC_OFFSET_CONFIG*/
typedef struct gc_offset_config
{
    uint8_t ft_kflag;
    int8_t GC_Offset;
}PACKED GC_OFFSET_CONFIG;

/****************************** OTP stru ******************************/
typedef struct volt_adc
{
    uint16_t volt;
    uint16_t adc;
}PACKED VOL_ADC;

typedef struct opt_vbat_config
{
    uint8_t ft_kflag;
    VOL_ADC data[3];
}PACKED OTP_VBAT_CONFIG;

typedef struct volt_sel
{
    uint16_t volt;
    uint8_t sel;
}PACKED VOL_SEL;

typedef struct otp_buck_mv_config
{
    uint8_t ft_kflag;
    VOL_SEL data[2];
}PACKED OTP_BUCK_MV_CONFIG;

typedef struct otp_buck_mv_freq_config
{
    uint8_t ft_kflag;
    uint16_t Frequency;
    uint8_t Freq_Trim;
    uint8_t Freqk_Trim;
}PACKED OTP_BUCK_MV_FREQ_CONFIG;

typedef struct otp_buck_mv_ret_config
{
    uint8_t ft_kflag;
    VOL_SEL data[2];
}PACKED OTP_BUCK_MV_RET_CONFIG;

typedef struct otp_buck_lv_config
{
    uint8_t ft_kflag;
    VOL_SEL data[2];
}PACKED OTP_BUCK_LV_CONFIG;

typedef struct otp_buck_lv_freq_config
{
    uint8_t ft_kflag;
    uint16_t Frequency;
    uint8_t Freq_Trim;
    uint8_t Freqk_Trim;
}PACKED OTP_BUCK_LV_FREQ_CONFIG;

typedef struct otp_ldo_vdig18_config
{
    uint8_t ft_kflag;
    VOL_SEL data[2];
}PACKED OTP_LDO_VDIG18_CONFIG;

typedef struct otp_ldo_vrf_reg_config
{
    uint8_t ft_kflag;
    VOL_SEL data[2];
}PACKED OTP_LDO_VRF_REG_CONFIG;

typedef struct otp_ldo_vrf_ret_config
{
    uint8_t ft_kflag;
    VOL_SEL data[2];
}PACKED OTP_LDO_VRF_RET_CONFIG;

typedef struct otp_ldo_vdd33_reg_config
{
    uint8_t ft_kflag;
    VOL_SEL data[2];
}PACKED OTP_LDO_VDD33_REG_CONFIG;

typedef struct otp_ldo_vdd33_ret_config
{
    uint8_t ft_kflag;
    VOL_SEL data[2];
}PACKED OTP_LDO_VDD33_RET_CONFIG;

typedef struct otp_hpbg_config
{
    uint8_t ft_kflag;
    VOL_SEL data[2];
}PACKED OTP_HPBG_CONFIG;

typedef struct otp_lpbg_config
{
    uint8_t ft_kflag;
    VOL_SEL data[2];
}PACKED OTP_LPBG_CONFIG;

typedef struct otp_chg_dac_config
{
    uint8_t ft_kflag;
    uint16_t DAC_4_35V;
    uint16_t DAC_4_2V;
    uint16_t DAC_4_05V;
}PACKED OTP_CHG_DAC_CONFIG;

typedef struct otp_vsys_ldo_config
{
    uint8_t ft_kflag;
    VOL_SEL data[2];
}PACKED OTP_VSYS_LDO_CONFIG;

typedef struct otp_ocp_config
{
    uint8_t ft_kflag;
    uint8_t load_switch_OCP_trim;
    uint8_t VSYSLDO_OCP_trim;
}PACKED OTP_OCP_CONFIG;

typedef struct current_value
{
    uint16_t dedicated_current;
    uint8_t estimated_measured_value;
}PACKED CURRENT_VALUE;

typedef struct otp_chg_4v2_current_config
{
    uint8_t ft_kflag;
    CURRENT_VALUE current_value[4];
}PACKED OTP_CHG_4V2_CURRENT_CONFIG;

typedef struct otp_chg_4v35_current_config
{
    uint8_t ft_kflag;
    CURRENT_VALUE current_value[4];
}PACKED OTP_CHG_4V35_CURRENT_CONFIG;

typedef struct otp_vichg_adc_config
{
    uint8_t ft_kflag;
    uint16_t ADC_4_35V;
    uint16_t ADC_4_2V;
    uint16_t ADC_4_05V;
}PACKED OTP_VICHG_ADC_CONFIG;

/******************************************To be remove****************************************/
typedef struct otp_aio_config
{
    uint8_t ft_kflag;
    uint8_t Gpio_sel;
    uint8_t Count;
    VOL_ADC data[5];
}PACKED OTP_AIO_CONFIG;

typedef struct otp_temp_adc_config
{
    uint8_t ft_kflag;
    uint8_t Offset_of_Adc_Table;
    uint8_t rsv;
}PACKED OTP_TEMP_ADC_CONFIG;

typedef struct otp_gc_offset_config
{
    uint8_t ft_kflag;
    int8_t GC_Offset;
}PACKED OTP_GC_OFFSET_CONFIG;


extern uint16_t slope_calc(uint16_t volt1, uint16_t adc1, uint16_t volt2, uint16_t adc2, uint16_t volt_val);
extern void otp_calibration(void);
extern void mpk_init(void);
extern pmu_status_t hal_pmu_get_nvkey(uint16_t id, uint8_t *ptr);
extern void* get_nvkey_data(uint16_t nvkey_id);

#endif
#endif/* __HAL_PMU_CALI_H__ */
