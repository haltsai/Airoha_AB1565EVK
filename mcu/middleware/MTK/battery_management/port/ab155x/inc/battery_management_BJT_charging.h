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

#ifndef __BATTERY_MANAGEMENT_BJT_CHARGING_H__
#define __BATTERY_MANAGEMENT_BJT_CHARGING_H__
enum
{
    TSTEP_ICC_MULTIPLE_TIME_0uS         = 0,
    TSTEP_ICC_MULTIPLE_TIME_16uS        = 1,
    TSTEP_ICC_MULTIPLE_TIME_128uS       = 2,
    TSTEP_ICC_MULTIPLE_TIME_256uS       = 3,
};

enum{
    RG_TSTEP_ICC_BJT_CURRENT_LEVEL_p5A    = 0,
    RG_TSTEP_ICC_BJT_CURRENT_LEVEL_p75A   = 1,
    RG_TSTEP_ICC_BJT_CURRENT_LEVEL_1A     = 2,
    RG_TSTEP_ICC_BJT_CURRENT_LEVEL_1p25A  = 3,
    RG_TSTEP_ICC_BJT_CURRENT_LEVEL_1p5A   = 4,
    RG_TSTEP_ICC_BJT_CURRENT_LEVEL_1p75A  = 5,
    RG_TSTEP_ICC_BJT_CURRENT_LEVEL_2A     = 6,
    RG_TSTEP_ICC_BJT_CURRENT_LEVEL_2AA     = 7,
};

typedef enum {
    BATTERRY_BJT_SAFETY_TIME_3H = 1,
    BATTERRY_BJT_SAFETY_TIME_4H5 = 2,
    BATTERRY_BJT_SAFETY_TIME_9H = 3,
} battery_managerment_bjt_safety_time_t;
void battery_enable_bjt(void);
void battery_bjt_charging_setting(void);
void battery_bjt_disable_charging(void);
void battery_bjt_check_ovp(void);
bool battery_bjt_enable_charging(uint8_t value);
#endif // __BATTERY_MANAGEMENT_BJT_CHARGING_H__
