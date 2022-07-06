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

/*==========[Battery parameter]==========*/
#define INITIAL_BAT 3000 //mA
#define SW_SHUT_DOWN 3300 //mA
#define FULL_BATTERY 4200 //mA
#define BATTERY_CAPACITY_OFFSET 50 //mA
#define MAX_BATTERY_VOLTAGE_OVERLAY 5500
#define EXTEND_TIME ITERM_TIME_SETTING_DISABLE
/*==========[Battery parameter: linear gauge]==========*/
#define GAUGE_TOLERANCE_PERCENT 95
#define CHARGER_TOLERANCE_TIME 13 //minute

/*==========[Charger parameter]==========*/
#ifdef AB1568
#define BATTERY_PRECC_CURRENT pmu_fastcc_chrcur_5mA
#endif
#define BATTERY_ITERM_ITH pmu_iterm_chrcur_5mA
#define CHARGER_REGULAR_TIME 120 // second
#define BATTERY_VOLTAGE_REFERENCE_POINTS 9
#define EOC_RECHARGER_VOLTAGE 4100
#define RECHARGER_VOLTAGE RECHARGER_MARK_100

#ifdef MTK_SMART_CHARGER_ENABLE
#define BATTERY_MANAGER_DEFAULT_CHARGER_OPTION 1 /*If smart charger case enable need choose option 1*/
#else
#ifdef MTK_BATTERY_EOC_ENTER_RTC
#define BATTERY_MANAGER_DEFAULT_CHARGER_OPTION 4
#else
#define BATTERY_MANAGER_DEFAULT_CHARGER_OPTION 1
#endif
#endif

#define CV_ITERM 50
#define BATTERY_S0_VOLTAGE 4     /*index : 4 means 3000V ; 0:2.4V, 1:2.55V, 2:2.7V, 3:2.85V, 4:3.0V, 5:3.15V, 6:3.3V, 7:3.45V */
#define BATTERY_S0_CHR_CUR pmu_fastcc_chrcur_100mA
#define BATTERY_S1_VOLTAGE 3700
#define BATTERY_S1_CHR_CUR pmu_fastcc_chrcur_100mA
#define BATTERY_S2_VOLTAGE 3900
#define BATTERY_S2_CHR_CUR pmu_fastcc_chrcur_100mA
#ifdef AB1568
#define BATTERY_ITERM_CURRENT_IRQ pmu_iterm_chrcur_50mA
#endif
/*==========[Charger parameter : JEITA]==========*/
#define HW_JEITA_HOT_THRESHOLD  593   /*45 celsius */
#define HW_JEITA_WARM_THRESHOLD 635   /*42 celsius */
#define HW_JEITA_COOL_THRESHOLD 1270  /* 3 celsius */
#define HW_JEITA_COLD_THRESHOLD 1316  /* 0 celsius */

#define BATTERY_COOL_CC RG_ICC_JC_20
#define BATTERY_COOL_CV 4200
#define BATTERY_WARM_CC RG_ICC_JC_100
#define BATTERY_WARM_CV 4050
/*==========[Feature Option]==========*/
#define BATTERY_MESSAGE_QUEUE_TASK FEATURE_OFF
#define BATTERY_MANAGER_JEITA_ENABLE FEATURE_ON

/* The first times in EOC state ,
 * It will be waits for one minute to execute EOC behavior.
 * BATTERY_EOC_CHECK_TIME unit is second.
 * EOC_CHECK_ON set BATTERY_OPERATE_ON is feature on.
 * */
#define BATTERY_EOC_CHECK_TIME 60
#define EOC_CHECK_ON BATTERY_OPERATE_ON
/*
 * It will check battery voltage when boot up
 * When the battery voltage is lower than the set voltage,
 * it will enter RTC mode
 * */
#define BATTERY_CHECK_BATTERY_VOLTAGE
/* Disable HW-JEITA WARM and COOOL stage detect.
 **/
//#define DISABLE_WARM_COOL

/*
 *PMIC_AWAKE_TO_CHARGER ,This feature is for low power charger.
 *When the feature on, CM4 can enter sleep in charging*/
//#define PMIC_AWAKE_TO_CHARGER

/* This is for BC1.2 feature
 */
#ifdef MTK_SMART_CHARGER_ENABLE

#else
/*BC1.2 feature */
#if !(defined MTK_BATTERY_BC1P2_DISABLE)
#define BATTERY_FEATURE_BC1P2
#endif

#endif


/* Avoid unexpected problems caused by irq shake,
 * Confirm HW status after irq specified time, and send notification if there is any problem*/
#define CHARGER_CALIBRATION
#ifdef CHARGER_CALIBRATION
#define CALIBRATION_TIME 1000 //Millisecond
#endif

/*Avoid VBUS shaking, ensure the Irq data is consistent this time*/
#define BATTERY_AVOID_SHAKING

/*Without NTC */
//#define BATTERY_NTC_LESS

/*When Use external charger IC ,Disable internal chager ,need to enable*/
//#define BATTERY_DISABLE_CHARGER

/*When NVDM feature enable,but don't want to use the value from NVDM.
 *And, use battery management customer file to set value.
 *Enable MTK_BATTERY_MANAGEMENT_STATIC_INIT_ENABLE that will use customer file value to setting*/
//#define MTK_BATTERY_MANAGEMENT_STATIC_INIT_ENABLE

/*Set the lowest showed SOC*/
#define MTK_BATTERY_ULTRA_LOW_BAT 1

/* Customized settings for customers to use
 * Used in init setting.
 * Default off , this api just for test and development*/
//#define BATTERY_CUSTOMER_SETTING
#define MTK_BATTERY_CHARGER_STEP

/*==========Linear gauge==========*/
#define BATTERY_LINEAR_GAUGE
#define DECREASE_CC_OBTAIN_VABT
#define LINEAR_GAUGE_SIZE 15 /*Define linear gauge array size*/
#define LINEAR_REGULAR_GAUGE_TIME 10000
#define BATTERY_STABLE_CHARGING_VOLTAGE 90
#define BATTERY_REMOVE_IR_SOC_MARGIN 3
//#define BATTERY_DECREASE_RESISTANCE
#ifdef BATTERY_CUSTOMER_SETTING
typedef struct {
    uint32_t addr;
    uint32_t value;
} battery_customer_setting_t;
#endif
