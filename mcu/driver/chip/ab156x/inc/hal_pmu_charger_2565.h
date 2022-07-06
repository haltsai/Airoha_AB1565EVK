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


#ifndef __HAL_PMU_CHARGER_H__
#define __HAL_PMU_CHARGER_H__
#ifdef HAL_PMU_MODULE_ENABLED

#define DIGITAL_THERMAL_FUNCTION 1
#define HW_JEITA_HOT_STAGE 0xF
#define HW_JEITA_WARM_STAGE 0xE
#define HW_JEITA_NORMAL_STAGE 0xC
#define HW_JEITA_COOL_STAGE 0x8
#define HW_JEITA_COLD_STAGE 0
#define HW_JEITA_ERRIR_STATE 0x4

typedef struct pmu_chg_info
{
    uint16_t cc1_thrd_volt;
    uint16_t cc1_curr;
    uint16_t cc2_thrd_volt;
    uint16_t cc2_curr;
    uint16_t full_bat_volt;
    uint16_t rechg_volt;
}PMU_CHG_INFO;

#define NTC_CHARGER_CON2_TRICKLE        1
#define NTC_CHARGER_CON2_CC1            2
#define NTC_CHARGER_CON2_CC2            3
#define NTC_CHARGER_CON2_CV             5
#define NTC_CHARGER_CON2_COMPL          6
#define NTC_CHARGER_CON2_RECHG          7

enum JEITA_CHARGER_STATUS
{
    JEITA_DISABLE_CHARGER = 0,
    JEITA_ENABLE_CHARGER = 1,
    JEITA_NORMAL_TO_WARM = 2,
    JEITA_NORMAL_TO_COOL = 3,
    JEITA_WARM_TO_NORMAL = 4,
    JEITA_COOL_TO_NORMAL = 5
};

typedef struct
{
    uint8_t enable_warm : 1;             //JEITA WARM
    uint8_t enable_cold : 1;             //JEITA COOL
    uint8_t enable_force_chg_off : 1;    //Any state to cold/hot
    uint8_t enable_limit_current : 1;    //Normal to Cool/Warm
    uint8_t disable_force_chg_off : 1;   //Cold Hot to any state
    uint8_t disable_limit_current : 1;   //Cool/Warm to Normal
    uint8_t power_off : 1;
    uint8_t reserved1 : 1;
}NTC_OP;

/*
 *  BC1.2 address */

#define PMU_REG_BASE_AB155X (0xA2070000)

// PMU Group
#define PMU2_ANA_CON0   (PMU_REG_BASE_AB155X+0x0500)
#define PMU2_ANA_CON1   (PMU_REG_BASE_AB155X+0x0504)
#define PMU2_ELR_0      (PMU_REG_BASE_AB155X+0x0508)
#define PMU2_ANA_RO     (PMU_REG_BASE_AB155X+0x0510)

//Control RG

#define PMU_BC12_IBIAS_EN_V12_ADDR                  PMU2_ANA_CON0
#define PMU_BC12_IBIAS_EN_V12_MASK                  0x1
#define PMU_BC12_IBIAS_EN_V12_SHIFT                 0

#define PMU_BC12_CMP_EN_V12_ADDR                    PMU2_ANA_CON0
#define PMU_BC12_CMP_EN_V12_MASK                    0x3
#define PMU_BC12_CMP_EN_V12_SHIFT                   1

#define PMU_BC12_DCD_EN_V12_ADDR                          PMU2_ANA_CON0
#define PMU_BC12_DCD_EN_V12_MASK                          0x1
#define PMU_BC12_DCD_EN_V12_SHIFT                         3

#define PMU_BC12_IPDC_EN_V12_ADDR                          PMU2_ANA_CON0
#define PMU_BC12_IPDC_EN_V12_MASK                          0x3
#define PMU_BC12_IPDC_EN_V12_SHIFT                         4

#define PMU_BC12_IPD_EN_V12_ADDR                          PMU2_ANA_CON0
#define PMU_BC12_IPD_EN_V12_MASK                          0x3
#define PMU_BC12_IPD_EN_V12_SHIFT                         6

#define PMU_BC12_IPD_HALF_EN_V12_ADDR                       PMU2_ANA_CON0
#define PMU_BC12_IPD_HALF_EN_V12_MASK                       0x1
#define PMU_BC12_IPD_HALF_EN_V12_SHIFT                      8

#define PMU_BC12_IPU_EN_V12_ADDR                          PMU2_ANA_CON0
#define PMU_BC12_IPU_EN_V12_MASK                          0x3
#define PMU_BC12_IPU_EN_V12_SHIFT                         9

#define PMU_BC12_VREF_VTH_EN_V12_ADDR                       PMU2_ANA_CON0
#define PMU_BC12_VREF_VTH_EN_V12_MASK                       0x3
#define PMU_BC12_VREF_VTH_EN_V12_SHIFT                      11

#define PMU_BC12_SRC_EN_V12_ADDR                          PMU2_ANA_CON0
#define PMU_BC12_SRC_EN_V12_MASK                          0x3
#define PMU_BC12_SRC_EN_V12_SHIFT                         13

#define PMU_BC12_IPU_TEST_EN_V12_ADDR                       PMU2_ANA_CON1
#define PMU_BC12_IPU_TEST_EN_V12_MASK                       0x1
#define PMU_BC12_IPU_TEST_EN_V12_SHIFT                      0

#define PMU_BC12_CS_TRIM_V12_ADDR                          PMU2_ELR_0
#define PMU_BC12_CS_TRIM_V12_MASK                          0x3F
#define PMU_BC12_CS_TRIM_V12_SHIFT                         0

#define PMU_AQ_QI_BC12_CMP_OUT_V12_ADDR                       PMU2_ANA_RO
#define PMU_AQ_QI_BC12_CMP_OUT_V12_MASK                       0x1
#define PMU_AQ_QI_BC12_CMP_OUT_V12_SHIFT                      0


/**************2565*****************/
typedef uint8_t CHG_STATE;
#define CHG_IDLE              ((CHG_STATE)0)
#define CHG_TRICKLE           ((CHG_STATE)1)
#define CHG_CC1               ((CHG_STATE)2)
#define CHG_CC2               ((CHG_STATE)3)
#define CHG_CV_INIT           ((CHG_STATE)4)
#define CHG_CV                ((CHG_STATE)5)
#define CHG_COMPL             ((CHG_STATE)6)
#define CHG_RECHG             ((CHG_STATE)7)

typedef uint8_t CHG_INTR;
#define CHG_INTR_NONE         ((CHG_INTR)0)
#define CHG_INTR_IN           ((CHG_INTR)1)
#define CHG_INTR_OUT          ((CHG_INTR)2)
#define CHG_INTR_COMPL        ((CHG_INTR)4)
#define CHG_INTR_RECHG        ((CHG_INTR)8)

typedef struct
{
    pmu_callback_t callback1; //chg in callback
    void *user_data1;
    pmu_callback_t callback2; //chg out callback
    void *user_data2;
    pmu_callback_t callback3; //chg compl callback
    void *user_data3;
    pmu_callback_t callback4; //chg rechg callback
    void *user_data4;
} pmu_charger_config_t;

typedef enum {
    PMU_CHG_OUT_DEB_0MS,
    PMU_CHG_OUT_DEB_10MS,
    PMU_CHG_OUT_DEB_20MS,
    PMU_CHG_OUT_DEB_40MS,
    PMU_CHG_IN_DEB_0MS = 0,
    PMU_CHG_IN_DEB_16MS,
    PMU_CHG_IN_DEB_40MS,
    PMU_CHG_IN_DEB_128MS,
} pmu_chg_deb_time_t;

typedef enum {
    PMU_CHG_IN_INT_FLAG,
    PMU_CHG_IN_INT_EN,
    PMU_CHG_OUT_INT_FLAG,
    PMU_CHG_OUT_INT_EN,
    PMU_CHG_COMPLETE_INT_FLAG,
    PMU_CHG_COMPLETE_INT_EN,
    PMU_CHG_RECHG_INT_FLAG,
    PMU_CHG_RECHG_INT_EN,
} pmu_chg_int_mask_t;

typedef enum {
    PMU_CHG_CC_JEITA_NORM,
    PMU_CHG_CC_JEITA_WARM,
    PMU_CHG_CC_JEITA_COOL,
    PMU_CHG_CC_BAT_REPORT,
    PMU_CHG_CC_MAX,
} pmu_chg_cc_state_t;

typedef struct
{
    uint16_t state;
    uint16_t norm_cc1_rsel;
    uint16_t warm_cc1_rsel;
    uint16_t cool_cc1_rsel;
    uint16_t vbat_cc1_rsel;
    uint16_t norm_cc2_rsel;
    uint16_t warm_cc2_rsel;
    uint16_t cool_cc2_rsel;
    uint16_t vbat_cc2_rsel;
} pmu_chg_cc_rsel_t;

typedef enum {
    PMU_CHG_CC1,
    PMU_CHG_CC2,
} pmu_cc_type_t;

extern pmu_chg_cc_rsel_t pmu_chg_cc_config;
extern PMU_CHG_INFO pmu_chg_info;

extern pmu_operate_status_t pmu_charger_cc_config(pmu_cc_type_t type, pmu_chg_cc_state_t state);
extern uint32_t pmu_chg_vchg_to_volt(uint32_t adc);
extern void pmu_charger_handler(uint16_t chg_flag);
extern bool pmu_charger_is_plug(void);
extern bool pmu_charger_is_compl(void);
extern uint16_t pmu_charger_get_chg_state(void);
extern bool pmu_charger_callback_init(void);
extern void pmu_eoc_ctrl (pmu_power_operate_t oper);
extern pmu_operate_status_t pmu_chg_deb_time(pmu_chg_deb_time_t out_deb_time, pmu_chg_deb_time_t in_deb_time);
extern void pmu_charger_pfm_config(uint8_t op);

extern void pmu_charger_int_in(void);
extern void pmu_charger_int_out(void);
extern void pmu_charger_int_compl(void);
extern void pmu_charger_int_rechg(void);

extern void ntc_change_state(enum JEITA_CHARGER_STATUS JEITA_OP);

extern void pmu_bat_init(void);
extern uint8_t pmu_bat_adc_to_perc(uint32_t adcval);
extern uint16_t pmu_bat_adc_to_k_perc(uint32_t adc);
extern uint32_t pmu_bat_volt_to_adc(uint32_t volt);
extern uint32_t pmu_bat_adc_to_volt(uint32_t adcval);
extern void pmu_bat_3v3_proc(void);
extern uint16_t pmu_bat_get_pure_adc(void);

/*==========[Basic function]==========*/
void pmu_charger_init_2565(uint16_t precc_cur,uint16_t cv_termination);
/*==========[charger]==========*/
uint8_t pmu_enable_charger_2565(uint8_t isEnableCharging);

/*==========[BC 1.2 behavior]==========*/

uint8_t pmu_get_bc12_charger_type_2565(void);
void pmu_bc12_init(void);


#endif /* HAL_PMU_MODULE_ENABLED */
#endif

