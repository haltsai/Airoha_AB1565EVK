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


#include "hal.h"
#ifdef HAL_PMU_MODULE_ENABLED
#include "hal_pmu_ab2565_platform.h"
#include "hal_pmu_auxadc_2565.h"
#include "hal_pmu_charger_2565.h"
#include "hal_pmu_internal_2565.h"

#include "assert.h"
#include <string.h>

/*========[Auxadc basic setting]========*/
void pmu_auxadc_init(void) {
    pmu_set_register_value_2565(ADC_AVG0_ADDR, ADC_AVG0_MASK, ADC_AVG0_SHIFT, 0x3);
    pmu_set_register_value_2565(ADC_ONE_SHOT_START_ADDR, ADC_ONE_SHOT_START_MASK, ADC_ONE_SHOT_START_SHIFT, 1);//set ready bit
}


/*========[Auxadc basic function]========*/
uint32_t pmu_auxadc_get_channel_value(pmu_adc_channel_t Channel)
{
    uint16_t adc_val = 0xFFFF;
    uint16_t reg_406 = 0, adc_sta = 0, err_sta = 0;
    uint32_t time_rdy1_s, time_rdy1_e, time_rdy2_s, time_rdy2_e, time_busy_s, time_busy_e;

    if ((Channel >= PMU_AUX_MAX) || (Channel < PMU_AUX_VICHG)) {
        log_hal_msgid_error("pmu_auxadc_get_channel_value fail, channel[%d]", 1, Channel);
        assert(0);
    }

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time_rdy1_s);
    while(1)
    {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time_rdy1_e);
        adc_sta = pmu_get_register_value_2565(ADC_RDY_STS0_ADDR, ADC_RDY_STS0_MASK, ADC_RDY_STS0_SHIFT);
        if (adc_sta == 1)
        {
            break;
        }
        else if (time_rdy1_e - time_rdy1_s > 3000)
        {
            err_sta |= 1;
            break;
        }
    }

    reg_406 = pmu_get_register_value_2565(ADC_CON3, 0xFFFF, 0);
    reg_406 = pmu_get_register_value_2565(ADC_CON3, 0xFFFF, 0);
    if (reg_406 & 0xF81E)
    {
        log_hal_msgid_error("pmu_auxadc_get_channel_value fail, reg_406[0x%X]", 1, reg_406);
        reg_406 = pmu_get_register_value_2565(ADC_CON3, 0xFFFF, 0);
        log_hal_msgid_error("pmu_auxadc_get_channel_value error check, reg_406[0x%X]", 1, reg_406);
        assert(0);
    }
    reg_406 = ((1 << Channel) | 0x0001);//set ch & one shot start
    pmu_force_set_register_value_2565(ADC_CON3, reg_406);

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time_busy_s);
    while(1)
    {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time_busy_e);
        adc_sta = pmu_get_register_value_2565(ADC_RDY_STS0_ADDR, ADC_RDY_STS0_MASK, ADC_RDY_STS0_SHIFT);
        if (adc_sta == 0)
        {
            break;
        }
        else if (time_busy_e - time_busy_s > 200)
        {
            err_sta |= 2;
            break;
        }
    }

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time_rdy2_s);
    while(1)
    {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time_rdy2_e);
        adc_sta = pmu_get_register_value_2565(ADC_RDY_STS0_ADDR, ADC_RDY_STS0_MASK, ADC_RDY_STS0_SHIFT);
        if (adc_sta == 1)
        {
            break;
        }
        else if (time_rdy2_e - time_rdy2_s > 4000)
        {
            err_sta |= 4;
            break;
        }
    }

    if (err_sta > 0)
    {
        log_hal_msgid_warning("pmu_auxadc_get_channel_value timeout, err_sta[0x%X], rdy1[%d], busy[%d], rdy2[%d], reg_418[0x%X]", 5,
            err_sta, (time_rdy1_e - time_rdy1_s), (time_busy_e - time_busy_s), (time_rdy2_e - time_rdy2_s), pmu_get_register_value_2565(ADC_STATE0, 0xFFFF, 0));
    }

    switch(Channel) {
        case PMU_AUX_VICHG:
            adc_val = pmu_get_register_value_2565(ADC_VALUE0_ADDR, ADC_VALUE0_MASK, ADC_VALUE0_SHIFT);
            log_hal_msgid_info("pmu_auxadc_get_channel_value, time[%dus], VICHG[%d]", 2, (time_rdy2_e - time_rdy1_s), adc_val);
        break;

        case PMU_AUX_VBAT:
            adc_val = pmu_get_register_value_2565(ADC_VALUE1_ADDR, ADC_VALUE1_MASK, ADC_VALUE1_SHIFT);
            log_hal_msgid_info("pmu_auxadc_get_channel_value, time[%dus], VBAT[%d]", 2, (time_rdy2_e - time_rdy1_s), adc_val);
        break;

        case PMU_AUX_VCHG:
            adc_val = pmu_get_register_value_2565(ADC_VALUE2_ADDR, ADC_VALUE2_MASK, ADC_VALUE2_SHIFT);
            log_hal_msgid_info("pmu_auxadc_get_channel_value, time[%dus], VCHG[%d]", 2, (time_rdy2_e - time_rdy1_s), adc_val);
        break;

        case PMU_AUX_LDO_BUCK:
            adc_val = pmu_get_register_value_2565(ADC_VALUE3_ADDR, ADC_VALUE3_MASK, ADC_VALUE3_SHIFT);
            log_hal_msgid_info("pmu_auxadc_get_channel_value, time[%dus], LDO_BUCK[%d]", 2, (time_rdy2_e - time_rdy1_s), adc_val);
        break;

        case PMU_AUX_VBAT_CAL:
            adc_val = pmu_get_register_value_2565(ADC_VALUE4_ADDR, ADC_VALUE4_MASK, ADC_VALUE4_SHIFT);
            log_hal_msgid_info("pmu_auxadc_get_channel_value, time[%dus], VBAT_CAL[%d]", 2, (time_rdy2_e - time_rdy1_s), adc_val);
        break;

        case PMU_AUX_VIN:
            adc_val = pmu_get_register_value_2565(ADC_VALUE5_ADDR, ADC_VALUE5_MASK, ADC_VALUE5_SHIFT);
            log_hal_msgid_info("pmu_auxadc_get_channel_value, time[%dus], VIN[%d]", 2, (time_rdy2_e - time_rdy1_s), adc_val);
        break;
    }

    return adc_val;
}

#endif /* HAL_PMU_AUXADC_MODULE_ENABLED */
