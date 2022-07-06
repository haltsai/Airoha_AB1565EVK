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

//---Battery parameter---
#define INITIAL_BAT 3000 //mA
#define SW_SHUT_DOWN 3200 //mA
#define FULL_BATTERY 4200 //mA
#define BATTERY_CAPACITY_OFFSET 50 //mA
#define CHARGER_REGULAR_TIME 120 // second
#define CHARGER_TOLERANCE_TIME 13 //minute

#define GAUGE_TOLERANCE_PERCENT 95
#define BATTERY_VOLTAGE_REFERENCE_POINTS 9
#define MAX_BATTERY_VOLTAGE_OVERLAY 5500
#define EOC_RECHARGER_VOLTAGE 4100
#define RECHARGER_VOLTAGE RECHARGER_MARK_100

#define EXTEND_TIME ITERM_TIME_SETTING_DISABLE
#define BJT_SAFETY_TIME BATTERRY_BJT_SAFETY_TIME_9H
#define CHARGER_MULTIPLE_DEFAULT_LEVEL ICC_MULTIPLE_ITH_1C
#define BATTERY_CHECK_BATTERY_VOLTAGE

/* The first times in EOC state ,
 * It will be waits for one minute to execute EOC behavior.
 * BATTERY_EOC_CHECK_TIME unit is second.
 * EOC_CHECK_ON set BATTERY_OPERATE_ON is feature on.
 * */
#define BATTERY_EOC_CHECK_TIME 60
#define EOC_CHECK_ON BATTERY_OPERATE_ON

/* Disable HW-JEITA WARM and COOOL stage detect.
 */
//#define DISABLE_WARM_COOL

/*
 *PMIC_AWAKE_TO_CHARGER ,This feature is for low power charger.
 *When the feature on, CM4 can enter sleep in charging*/
//#define PMIC_AWAKE_TO_CHARGER


/* Customized settings for customers to use
 * Used in init setting.
 * Default off , this api just for test and development*/
//#define BATTERY_CUSTOMER_SETTING

/* This is for BC1.2 feature
 */
#ifdef MTK_SMART_CHARGER_ENABLE

#else
	#define BATTERY_FEATURE_BC1P2
#endif

#ifdef BATTERY_CUSTOMER_SETTING
typedef struct {
    uint32_t addr;
    uint32_t value;
} battery_customer_setting_t;
#endif

#define CHARGER_CALIBRATION
#ifdef CHARGER_CALIBRATION
/*For BC1.2 specification, suggest not less than 500ms,if less than 500ms does not meet the specifications*/
#define CALIBRATION_TIME 1000 //Millisecond
#endif
