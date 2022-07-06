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

void battery_management_bc12_set_ibias_en_v12(uint8_t value) {
    bc12_set_register_value(PMU_BC12_IBIAS_EN_V12_ADDR, PMU_BC12_IBIAS_EN_V12_MASK, PMU_BC12_IBIAS_EN_V12_SHIFT, value);
}

void battery_management_bc12_set_cmp_en_v12(uint8_t value) {
    bc12_set_register_value(PMU_BC12_CMP_EN_V12_ADDR, PMU_BC12_CMP_EN_V12_MASK, PMU_BC12_CMP_EN_V12_SHIFT, value);
}

void battery_management_bc12_set_dcd_en_v12(uint8_t value) {
    bc12_set_register_value(PMU_BC12_DCD_EN_V12_ADDR, PMU_BC12_DCD_EN_V12_MASK, PMU_BC12_DCD_EN_V12_SHIFT, value);
}

void battery_management_bc12_set_ipdc_en_v12(uint8_t value) {
    bc12_set_register_value(PMU_BC12_IPDC_EN_V12_ADDR, PMU_BC12_IPDC_EN_V12_MASK, PMU_BC12_IPDC_EN_V12_SHIFT, value);
}

void battery_management_bc12_set_ipd_en_v12(uint8_t value) {
    bc12_set_register_value(PMU_BC12_IPD_EN_V12_ADDR, PMU_BC12_IPD_EN_V12_MASK, PMU_BC12_IPD_EN_V12_SHIFT, value);
}

void battery_management_bc12_set_ipd_half_en_v12(uint8_t value) {
    bc12_set_register_value(PMU_BC12_IPD_HALF_EN_V12_ADDR, PMU_BC12_IPD_HALF_EN_V12_MASK, PMU_BC12_IPD_HALF_EN_V12_SHIFT, value);
}

void battery_management_bc12_set_ipu_en_v12(uint8_t value) {
    bc12_set_register_value(PMU_BC12_IPU_EN_V12_ADDR, PMU_BC12_IPU_EN_V12_MASK, PMU_BC12_IPU_EN_V12_SHIFT, value);
}

void battery_management_bc12_set_vref_vth_en_v12(uint8_t value) {
    bc12_set_register_value(PMU_BC12_VREF_VTH_EN_V12_ADDR, PMU_BC12_VREF_VTH_EN_V12_MASK, PMU_BC12_VREF_VTH_EN_V12_SHIFT, value);
}

void battery_management_bc12_set_src_en_v12(uint8_t value) {
    bc12_set_register_value(PMU_BC12_SRC_EN_V12_ADDR, PMU_BC12_SRC_EN_V12_MASK, PMU_BC12_SRC_EN_V12_SHIFT, value);
}

void battery_management_bc12_set_ipu_test_en_v12(uint8_t value) {
    bc12_set_register_value(PMU_BC12_IPU_TEST_EN_V12_ADDR, PMU_BC12_IPU_TEST_EN_V12_MASK,PMU_BC12_IPU_TEST_EN_V12_SHIFT, value);
}

void battery_management_bc12_set_cs_trim_en_v12(uint8_t value) {
    bc12_set_register_value(PMU_BC12_CS_TRIM_V12_ADDR, PMU_BC12_CS_TRIM_V12_MASK, PMU_BC12_CS_TRIM_V12_SHIFT, value);
}

void battery_core_pmu_set_rg_aq_qi_bc12_cmp_out_v12(uint8_t value) {
    bc12_set_register_value(PMU_AQ_QI_BC12_CMP_OUT_V12_ADDR, PMU_AQ_QI_BC12_CMP_OUT_V12_MASK, PMU_AQ_QI_BC12_CMP_OUT_V12_SHIFT, value);
}

uint32_t battery_management_bc12_get_qi_bc12_cmp_out(void) {
    return bc12_get_register_value(PMU_AQ_QI_BC12_CMP_OUT_V12_ADDR, PMU_AQ_QI_BC12_CMP_OUT_V12_MASK, PMU_AQ_QI_BC12_CMP_OUT_V12_SHIFT);
}

void battery_core_pmu_set_bc12_0500_rg(short int value) {
    bc12_set_register_value(PMU2_ANA_CON0, 0x7FFF, 0, value);
}


