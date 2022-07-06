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

#include "battery_management_core.h"
extern auxadc_efuse_data_stru gAuxadcEfuseInfo;

void battery_efuse_enable_reading(void) {
    //Get Original MatchKey
    gAuxadcEfuseInfo.matchKey = pmu_get_register_value_2byte_mt6388(PMU_RG_OTP_RD_PKEY_ADDR, PMU_RG_OTP_RD_PKEY_MASK, PMU_RG_OTP_RD_PKEY_SHIFT);
    // RG_EFUSE_CK_PDN
    pmu_set_register_value_2byte_mt6388(PMU_RG_EFUSE_CK_PDN_ADDR, PMU_RG_EFUSE_CK_PDN_MASK, PMU_RG_EFUSE_CK_PDN_SHIFT, 0);
    //RG_EFUSE_CK_PDN_HWEN
    pmu_set_register_value_2byte_mt6388(PMU_RG_EFUSE_CK_PDN_HWEN_ADDR, PMU_RG_EFUSE_CK_PDN_HWEN_MASK, PMU_RG_EFUSE_CK_PDN_HWEN_SHIFT, 0);
    // OTP set Match Key
    pmu_set_register_value_2byte_mt6388(PMU_RG_OTP_RD_PKEY_ADDR, PMU_RG_OTP_RD_PKEY_MASK, PMU_RG_OTP_RD_PKEY_SHIFT, 0x0289);
    /*Set SW trigger read mode
     * Set HW Mode ;
     * 0: non SW trigger read mode
     * 1: SW trigger read mode*/
    pmu_set_register_value_2byte_mt6388(PMU_RG_OTP_RD_SW_ADDR, PMU_RG_OTP_RD_SW_MASK, PMU_RG_OTP_RD_SW_SHIFT, 1);
}

void battery_efuse_disable_reading(void) {
    // RG_EFUSE_CK_PDN
    pmu_set_register_value_2byte_mt6388(PMU_RG_EFUSE_CK_PDN_ADDR, PMU_RG_EFUSE_CK_PDN_MASK, PMU_RG_EFUSE_CK_PDN_SHIFT, 1);
    //RG_EFUSE_CK_PDN_HWEN
    pmu_set_register_value_2byte_mt6388(PMU_RG_EFUSE_CK_PDN_HWEN_ADDR, PMU_RG_EFUSE_CK_PDN_HWEN_MASK, PMU_RG_EFUSE_CK_PDN_HWEN_SHIFT, 1);
    // OTP set Original Match Key
    pmu_set_register_value_2byte_mt6388(PMU_RG_OTP_RD_PKEY_ADDR, PMU_RG_OTP_RD_PKEY_MASK, PMU_RG_OTP_RD_PKEY_SHIFT, gAuxadcEfuseInfo.matchKey);
    /*Set SW trigger read mode
     * Set HW Mode ;
     * 0: non SW trigger read mode
     * 1: SW trigger read mode*/
    pmu_set_register_value_2byte_mt6388(PMU_RG_OTP_RD_SW_ADDR, PMU_RG_OTP_RD_SW_MASK, PMU_RG_OTP_RD_SW_SHIFT, 0);
}

bool battery_core_efuse_rd_trigger(void) {
    uint8_t rgTrigValue = 0xFF;
    rgTrigValue = pmu_get_register_value_2byte_mt6388(PMU_RG_OTP_RD_TRIG_ADDR, PMU_RG_OTP_RD_TRIG_MASK, PMU_RG_OTP_RD_TRIG_SHIFT);
    if (rgTrigValue == 0xFF) {
        LOG_MSGID_I(battery_management, "Efuse RD Trig Failed\r\n", 0);
        return false;
    }
    if (rgTrigValue == 0) {
        pmu_set_register_value_2byte_mt6388(PMU_RG_OTP_RD_TRIG_ADDR, PMU_RG_OTP_RD_TRIG_MASK, PMU_RG_OTP_RD_TRIG_SHIFT, 1);
        return true;
    } else if (rgTrigValue == 1) {
        pmu_set_register_value_2byte_mt6388(PMU_RG_OTP_RD_TRIG_ADDR, PMU_RG_OTP_RD_TRIG_MASK, PMU_RG_OTP_RD_TRIG_SHIFT, 0);
        return true;
    } else {
        return false;
    }
}

uint16_t battery_core_efuse_get_efuse_data_by_address(uint16_t efuseAddress) {
    pmu_set_register_value_2byte_mt6388(PMU_RG_OTP_PA_ADDR, PMU_RG_OTP_PA_MASK, PMU_RG_OTP_PA_SHIFT, efuseAddress);
    if (!battery_core_efuse_rd_trigger())
        return 0;
    hal_gpt_delay_us(300);
    while (pmu_get_register_value_2byte_mt6388(PMU_RG_OTP_RD_BUSY_ADDR, PMU_RG_OTP_RD_BUSY_MASK, PMU_RG_OTP_RD_BUSY_SHIFT)) {
        LOG_MSGID_I(battery_management, "Efuse Data Not Ready\r\n", 0);
    };
    hal_gpt_delay_us(100);
    return pmu_get_register_value_2byte_mt6388(PMU_RG_OTP_DOUT_SW_ADDR, PMU_RG_OTP_DOUT_SW_MASK, PMU_RG_OTP_DOUT_SW_SHIFT);
}
