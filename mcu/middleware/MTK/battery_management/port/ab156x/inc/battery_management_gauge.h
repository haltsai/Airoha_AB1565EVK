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

#ifndef __BATTERY_MANAGEMENT_GUAGE_H__
#define __BATTERY_MANAGEMENT_GUAGE_H__


#define INVALID_INDEX 0xFF

typedef struct
{
	signed short currentVoltagePercent;
}battery_gauge_ctrl_stru;

typedef struct
{
	unsigned short int initVolt;
	unsigned short int finalVolt;
	unsigned int rBattery;
}battery_fuel_gauge_voltage_stru;

enum
{
	FUEL_GUAGE_ON_DATA_INIT_SUCCESS,
	FUEL_GUAGE_ON_DATA_INIT_FAIL,
	FUEL_GUAGE_OFF,
};
/*==========[Battery Management:Linear Gague]==========*/
void battery_calibration_gauge_tolerance(void);
void battery_switch_calibration_state(uint8_t ste);
void battery_set_calibration_time(void);
unsigned char battery_gauge_get_refernece_index(signed short vBatSnsValue);
uint32_t battery_core_gauge_function(signed short vBatSnsValue);
float battery_core_gauge_indexCalc(unsigned char index);
uint32_t battery_get_gauge_percent(void);
uint32_t battery_get_gauge_percent_level(void);
void battery_smooth_charging_soc(uint32_t soc_value, uint32_t batsns);
void battery_smooth_discharger_soc(uint32_t soc_value,uint32_t batsns);
uint32_t get_smooth_soc(void);
uint32_t get_smooth_vbat(void);
void battery_linear_gauge_task(void * pvParameters);
void battery_gauge_init(void);

#endif // __BATTERY_MANAGEMENT_GUAGE_H__
