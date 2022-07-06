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
#include "hal_pmu_ab2568_platform.h"
#include "hal_pmu_auxadc_2568.h"
#include "hal_pmu_internal_2568.h"
#include "hal_nvic_internal.h"
#include <string.h>

#define AUXADC_CH0_BASE 0
#define AUXADC_CH2_BASE 20
#define AUXADC_CH3_BASE 30
#define AUXADC_CH4_BASE 40

#define AUXADC_CHECK_TIMES 25
#define AUXADC_CHECK_MAX_TIMES 100

uint8_t re_request_flag =0;
uint8_t auxadc_status_index=0;

auxadc_efuse_data_stru gAuxadcEfuseInfo; /*data : auxadc efuse struct*/

/*========[Auxadc basic setting]========*/
void pmu_auxadc_init(void) {
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_CON0, PMU_AUXADC_CK_AON_MASK, PMU_AUXADC_CK_AON_SHIFT, 0); //setting of dynamic CLK Managerment
    pmu_set_register_value_ab2568(PMU_STRUP_CON6, PMU_RG_STRUP_AUXADC_RSTB_SW_MASK, PMU_RG_STRUP_AUXADC_RSTB_SW_SHIFT, 1);//auxadc not being reset on sleep mode
    pmu_set_register_value_ab2568(PMU_STRUP_CON6, PMU_RG_STRUP_AUXADC_RSTB_SEL_MASK, PMU_RG_STRUP_AUXADC_RSTB_SEL_SHIFT, 1);
    //BAT DETECT
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_LBAT3, PMU_AUXADC_LBAT_EN_MAX_MASK, PMU_AUXADC_LBAT_EN_MAX_SHIFT, 1);
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_LBAT4, PMU_AUXADC_LBAT_EN_MIN_MASK, PMU_AUXADC_LBAT_EN_MIN_SHIFT, 1);
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_LBAT2_3, PMU_AUXADC_LBAT2_EN_MAX_MASK, PMU_AUXADC_LBAT2_EN_MAX_SHIFT, 1);
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_LBAT2_4, PMU_AUXADC_LBAT2_EN_MIN_MASK, PMU_AUXADC_LBAT2_EN_MIN_SHIFT, 1);
    //THR DETECT
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_THR3, PMU_AUXADC_THR_EN_MAX_MASK, PMU_AUXADC_THR_EN_MAX_SHIFT, 1);
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_THR4, PMU_AUXADC_THR_EN_MIN_MASK, PMU_AUXADC_THR_EN_MIN_SHIFT, 1);
}

void pmu_thermal_parameter_init_2568(void) {
    memset(&gAuxadcEfuseInfo, 0, sizeof(auxadc_efuse_data_stru));
    pmu_efuse_enable_reading();
    gAuxadcEfuseInfo.byte0 = pmu_core_efuse_get_efuse_data_by_address(106);
    gAuxadcEfuseInfo.byte1 = pmu_core_efuse_get_efuse_data_by_address(108);
    gAuxadcEfuseInfo.byte2 = pmu_core_efuse_get_efuse_data_by_address(8);
    pmu_efuse_disable_reading();
    uint8_t temp=pmu_get_register_value_ddie((uint32_t)&gAuxadcEfuseInfo.byte2, 0x1F, 6);
    pmu_set_register_value_ab2568(PMU_VREF_Interface_ELR_2, PMU_RG_VCORE_VSLEEP_V10_TRIM_MASK, PMU_RG_VCORE_VSLEEP_V10_TRIM_SHIFT, temp); /*Low power setting*/
    gAuxadcEfuseInfo.isInit = 1;
}

/*========[Auxadc basic function]========*/

uint32_t pmu_auxadc_get_channel_value(pmu_adc_channel_t Channel)
{
    uint32_t adc_result=-1;
    pmu_enable_power(PMU_LDO_VA18, PMU_ON);
    if (Channel >= PMU_AUX_MAX) {
        log_hal_msgid_error("[PMIC]Error auxadc chnnel\r\n", 0);
        return (0xFFFFFFFF);    //Channel error
    }
    switch(Channel) {
        case PMU_AUX_PN_ZCV:
            adc_result = pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC20,PMU_AUXADC_ADC_OUT_PWRON_PCHR_MASK,PMU_AUXADC_ADC_OUT_PWRON_PCHR_SHIFT);
            adc_result =(adc_result*1800*3)/32768;
        break;

        case PMU_AUX_WK_ZCV:
            adc_result = pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC22,PMU_AUXADC_ADC_OUT_WAKEUP_PCHR_MASK,PMU_AUXADC_ADC_OUT_WAKEUP_PCHR_SHIFT);
            adc_result =(adc_result*1800*3)/32768;
        break;

        case PMU_AUX_BATSNS:
            if(pmu_auxadc_channel_request(0)!=PMU_STATUS_SUCCESS){
               log_hal_msgid_error("[PMIC]auxadc chnnel erro %d\r\n", 1,PMU_AUX_BATSNS);
            }
            adc_result = pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC25,PMU_AUXADC_ADC_OUT_CH0_BY_AP_MASK,PMU_AUXADC_ADC_OUT_CH0_BY_AP_SHIFT);
            adc_result =((float)adc_result*1800*3)/32768;
        break;
        case PMU_AUX_LBAT:
            if(pmu_auxadc_channel_request(0)!=PMU_STATUS_SUCCESS){
               log_hal_msgid_error("[PMIC]auxadc chnnel erro %d\r\n", 1,PMU_AUX_LBAT);
            }
            adc_result = pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC25,PMU_AUXADC_ADC_OUT_LBAT2_MASK,PMU_AUXADC_ADC_OUT_LBAT2_SHIFT);
            adc_result =((float)adc_result*1800*3)/32768;
        break;
        case PMU_AUX_LBAT2_VBATLOW:
            if(pmu_auxadc_channel_request(0)!=PMU_STATUS_SUCCESS){
               log_hal_msgid_error("[PMIC]auxadc chnnel erro %d\r\n", 1,PMU_AUX_LBAT2_VBATLOW);
            }
            adc_result = pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC25,PMU_AUXADC_ADC_OUT_CH0_BY_AP_MASK,PMU_AUXADC_ADC_OUT_CH0_BY_AP_SHIFT);
            adc_result =((float)adc_result*1800*3)/32768;
        break;
        case PMU_AUX_BAT_RECHARGER:
            if(pmu_auxadc_channel_request(2)!=PMU_STATUS_SUCCESS){
               log_hal_msgid_error("[PMIC]auxadc chnnel erro %d\r\n", 1,PMU_AUX_BAT_RECHARGER);
            }
            adc_result = pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC25,PMU_AUXADC_ADC_OUT_VBATSNS_DET_MASK,PMU_AUXADC_ADC_OUT_VBATSNS_DET_SHIFT);
            adc_result = (adc_result*1800*8)/4096;
        break;
        case PMU_AUX_VBUS:
            if(pmu_get_chr_detect_value()==0){
            pmu_set_register_value_ab2568(PMU_CORE_CORE_AO_CTRL_0, PMU_RG_BUS_SNS_LOGIC_SEL_MASK, PMU_RG_BUS_SNS_LOGIC_SEL_SHIFT, 0);	
            }
            if(pmu_auxadc_channel_request(2)!=PMU_STATUS_SUCCESS){
               log_hal_msgid_error("[PMIC]auxadc chnnel erro %d\r\n", 1,PMU_AUX_VBUS);
            }
            adc_result = pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC2, PMU_AUXADC_ADC_OUT_CH2_MASK, PMU_AUXADC_ADC_OUT_CH2_SHIFT);
            adc_result = (adc_result * 1800 * 8) / 4096;
            pmu_set_register_value_ab2568(PMU_CORE_CORE_AO_CTRL_0, PMU_RG_BUS_SNS_LOGIC_SEL_MASK, PMU_RG_BUS_SNS_LOGIC_SEL_SHIFT, 1);
        break;
        case PMU_AUX_CHR_THM:
            pmu_set_register_value_ab2568(PMU_AUXADC_AD_RQST0, PMU_AUXADC_RQST_CH3_MASK, PMU_AUXADC_RQST_CH3_SHIFT, 1);
            do {
            }while(pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC3,PMU_AUXADC_ADC_RDY_CH3_MASK,PMU_AUXADC_ADC_RDY_CH3_SHIFT)!=1);
            adc_result = pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC3,PMU_AUXADC_ADC_OUT_CH3_MASK,PMU_AUXADC_ADC_OUT_CH3_SHIFT);
            adc_result = (adc_result*1800)/4096;
        break;

        case PMU_AUX_HW_JEITA:
            adc_result = pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC35,PMU_AUXADC_ADC_OUT_JEITA_MASK,PMU_AUXADC_ADC_OUT_JEITA_SHIFT);
            adc_result = (adc_result*1800)/4096;
        break;
        case PMU_AUX_PMIC_AP:
            if (pmu_auxadc_channel_request(4) != PMU_STATUS_SUCCESS) {
                log_hal_msgid_error("[PMIC]auxadc chnnel erro %d\r\n", 1,PMU_AUX_PMIC_AP);
            }
            adc_result = pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC4,PMU_AUXADC_ADC_OUT_CH4_MASK,PMU_AUXADC_ADC_OUT_CH4_SHIFT);
            adc_result = (adc_result*1800)/4096;
        break;
        case PMU_AUX_PMIC_AUTO:
            if (pmu_auxadc_channel_request(4) != PMU_STATUS_SUCCESS) {
                log_hal_msgid_error("[PMIC]auxadc chnnel erro %d\r\n", 1,PMU_AUX_PMIC_AUTO);
            }
            adc_result = pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC4,PMU_AUXADC_ADC_OUT_THR_HW_MASK,PMU_AUXADC_ADC_OUT_THR_HW_SHIFT);
            adc_result = (adc_result*1800)/4096;
        break;
    }
    pmu_enable_power(PMU_LDO_VA18, PMU_OFF);
    if (re_request_flag) {
        log_hal_msgid_info("[PMIC]re_request happen", 0);
        re_request_flag = 0;
    }
    log_hal_msgid_info("[PMIC] auxadc_status :%d  Ch:%d Val:%d", 3,auxadc_status_index,Channel,adc_result);
    return adc_result;
}

int8_t pmu_auxadc_channel_request(uint8_t channel) {
    pmu_status_t request_state = -1;
    uint8_t channel0_status = 0;      //check times
	uint8_t channel2_status = 0;      //check times
    uint8_t channel3_status = 0;      //check times
    uint8_t channel4_status = 0;      //check times
    switch (channel) {
    case 0:
        pmu_set_register_value_ab2568(PMU_AUXADC_AD_RQST0, PMU_AUXADC_RQST_CH0_MASK, PMU_AUXADC_RQST_CH0_SHIFT, 1);
        do {
            if (channel0_status > AUXADC_CHECK_TIMES) {  //Re-request
                pmu_set_register_value_ab2568(PMU_AUXADC_AD_RQST0, PMU_AUXADC_RQST_CH0_MASK, PMU_AUXADC_RQST_CH0_SHIFT,1);
                re_request_flag = 1;
            }
            if (channel0_status > AUXADC_CHECK_MAX_TIMES) { //Request fail
                request_state = PMU_STATUS_ERROR;
                return request_state;
            }
            channel0_status++;
            auxadc_status_index = AUXADC_CH0_BASE + channel0_status;
        } while (pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC25, PMU_AUXADC_ADC_RDY_CH0_BY_AP_MASK,PMU_AUXADC_ADC_RDY_CH0_BY_AP_SHIFT) != 1);
        request_state = PMU_STATUS_SUCCESS;
        break;

    case 2:
        pmu_set_register_value_ab2568(PMU_AUXADC_AD_RQST0, PMU_AUXADC_RQST_CH2_MASK, PMU_AUXADC_RQST_CH2_SHIFT, 1);
        do {
            if (channel2_status > AUXADC_CHECK_TIMES) {  //Re-request
                pmu_set_register_value_ab2568(PMU_AUXADC_AD_RQST0, PMU_AUXADC_RQST_CH2_MASK, PMU_AUXADC_RQST_CH2_SHIFT,1);
                re_request_flag = 1;
            }
            if (channel2_status > AUXADC_CHECK_MAX_TIMES) { //Request fail
                request_state = PMU_STATUS_ERROR;
                return request_state;
            }
            channel2_status++;
            auxadc_status_index = AUXADC_CH2_BASE + channel2_status;
        } while (pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC2, PMU_AUXADC_ADC_RDY_CH2_MASK,PMU_AUXADC_ADC_RDY_CH2_SHIFT) != 1);
        request_state = PMU_STATUS_SUCCESS;
        break;

    case 3:
        pmu_set_register_value_ab2568(PMU_AUXADC_AD_RQST0, PMU_AUXADC_RQST_CH3_MASK, PMU_AUXADC_RQST_CH3_SHIFT, 1);
        do {
            if (channel3_status > AUXADC_CHECK_TIMES) {  //Re-request
                pmu_set_register_value_ab2568(PMU_AUXADC_AD_RQST0, PMU_AUXADC_RQST_CH3_MASK, PMU_AUXADC_RQST_CH3_SHIFT,1);
                re_request_flag = 1;
            }
            if (channel3_status > AUXADC_CHECK_MAX_TIMES) { //Request fail
                request_state = PMU_STATUS_ERROR;
                return request_state;
            }
            channel3_status++;
            auxadc_status_index = AUXADC_CH3_BASE + channel3_status;
        } while (pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC3, PMU_AUXADC_ADC_RDY_CH3_MASK,PMU_AUXADC_ADC_RDY_CH3_SHIFT) != 1);
        request_state = PMU_STATUS_SUCCESS;
        break;
    case 4:
        pmu_set_register_value_ab2568(PMU_AUXADC_AD_RQST0, PMU_AUXADC_RQST_CH4_MASK, PMU_AUXADC_RQST_CH4_SHIFT, 1);
        do {
            if (channel4_status > AUXADC_CHECK_TIMES) {  //Re-request
                pmu_set_register_value_ab2568(PMU_AUXADC_AD_RQST0, PMU_AUXADC_RQST_CH4_MASK, PMU_AUXADC_RQST_CH4_SHIFT,1);
                re_request_flag = 1;
            }
            if (channel4_status > AUXADC_CHECK_MAX_TIMES) { //Request fail
                request_state = PMU_STATUS_ERROR;
                return request_state;
            }
            channel4_status++;
            auxadc_status_index = AUXADC_CH4_BASE + channel4_status;
        } while (pmu_get_register_value_ab2568(PMU_AUXADC_AD_ADC4, PMU_AUXADC_ADC_RDY_CH4_MASK,PMU_AUXADC_ADC_RDY_CH4_SHIFT) != 1);
        request_state = PMU_STATUS_SUCCESS;
        break;
    }
    return request_state;
}

/*========[Auxadc Algorithm]========*/
uint32_t pmu_auxadc_voltage_transfer(int16_t value) {
    uint32_t miniVoltage = -1;
    miniVoltage = (value * 1800) / 4096;
    return miniVoltage;
}

/*
Tcurrent = DEGC / 2 + (Ycurr - VBE_T)*1e-3 / {-[0.0016754 + (1 - 2 * O_SLOPE_SIGN)*O_SLOPE / 100000]}
Note: O_SLOPE is signed integer.
*/
uint32_t pmu_auxadc_calculate_current_temperature(int16_t slope_value, int32_t yBet, int32_t yCurr, int8_t degcValue) {
    int32_t PmicTemp;
    int32_t tempValue = 0;
    int32_t tempValue2 = 0;
    int32_t temperature = 0;
    int32_t tempValue3 = 0;
    int32_t tempValue4 = 0;
    tempValue3 = (1 - 2 * pmu_auxadc_get_slope_sign_value()) * ((float) slope_value) / 100000;
    tempValue = (-1) * ((16754/10000000) + tempValue3);
    tempValue2 = ((float) (yCurr - yBet)) * ((float) (0.001));
    tempValue4 = tempValue2 / tempValue;
    temperature = ((float) degcValue) / 2 + tempValue4;
    PmicTemp = temperature;
    return PmicTemp;
}

int16_t pmu_auxadc_get_slope_value(void) {
    int16_t slope = 0;
    if (pmu_auxadc_get_slope_sign_value() == 1) {
        slope = ((int16_t) pmu_auxadc_get_efuse_slope_value()) * (-1);
    } else {
        slope = ((int16_t) pmu_auxadc_get_efuse_slope_value()) * (1);
    }
    return slope;
}

int8_t pmu_core_auxadc_get_degc_value(void) {
    int8_t degc = 0;
    if (pmu_auxadc_get_degc_cali() < 38 || pmu_auxadc_get_degc_cali() > 60) {
        degc = 53;
    } else {
        degc = pmu_auxadc_get_degc_cali();
    }
    return degc;
}

uint32_t pmu_get_auxadc_pmic_temperature_2568(void) {
    uint32_t adc_Out = 0;
    uint32_t vts_value = 0;
    if (!gAuxadcEfuseInfo.isInit) {
        log_hal_msgid_info("Efuse Data Is Not Init\r\n",0);
    }

    if (!pmu_auxadc_get_adc_cali_en()) {
        log_hal_msgid_info("TempValue in Efuse is Not Calibration\r\n",0);
    }
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_CON12, PMU_RG_VBUF_BYP_MASK, PMU_RG_VBUF_BYP_SHIFT, 0);
    adc_Out = pmu_auxadc_get_channel_value(PMU_AUX_PMIC_AP);
    vts_value = pmu_auxadc_voltage_transfer((int16_t) pmu_auxadc_get_vts());
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_CON12, PMU_RG_VBUF_BYP_MASK, PMU_RG_VBUF_BYP_SHIFT, 1);
    return pmu_auxadc_calculate_current_temperature(pmu_auxadc_get_slope_value(), (int32_t) vts_value, (int32_t) adc_Out,pmu_core_auxadc_get_degc_value());
}

bool pmu_core_efuse_rd_trigger(void) {
    uint8_t rgTrigValue = 0xFF;
    rgTrigValue = pmu_get_register_value_ab2568(PMU_OTP_CON8, PMU_RG_OTP_RD_TRIG_MASK, PMU_RG_OTP_RD_TRIG_SHIFT);
    if (rgTrigValue == 0xFF) {
        log_hal_msgid_info("Efuse RD Trig Failed\r\n", 0);
        return false;
    }
    if (rgTrigValue == 0) {
        pmu_set_register_value_ab2568(PMU_OTP_CON8, PMU_RG_OTP_RD_TRIG_MASK, PMU_RG_OTP_RD_TRIG_SHIFT, 1);
        return true;
    } else if (rgTrigValue == 1) {
        pmu_set_register_value_ab2568(PMU_OTP_CON8, PMU_RG_OTP_RD_TRIG_MASK, PMU_RG_OTP_RD_TRIG_SHIFT, 0);
        return true;
    } else {
        return false;
    }
}

uint16_t pmu_core_efuse_get_efuse_data_by_address(uint16_t efuseAddress) {
    pmu_set_register_value_ab2568(PMU_OTP_CON0, PMU_RG_OTP_PA_MASK, PMU_RG_OTP_PA_SHIFT, efuseAddress);
    if (!pmu_core_efuse_rd_trigger())
        return 0;
    hal_gpt_delay_us(300);
    while (pmu_get_register_value_ab2568(PMU_OTP_CON13, PMU_RG_OTP_RD_BUSY_MASK, PMU_RG_OTP_RD_BUSY_SHIFT)) {
        log_hal_msgid_info("Efuse Data Not Ready\r\n",0);
    };
    return pmu_get_register_value_ab2568(PMU_OTP_CON12, PMU_RG_OTP_DOUT_SW_MASK, PMU_RG_OTP_DOUT_SW_SHIFT);
}

/*========[Auxadc info for Algorithm]========*/
uint8_t pmu_auxadc_get_slope_sign_value(void) {
    return ((gAuxadcEfuseInfo.byte1 & AUXADC_EFUSE_O_SLOPE_SIGNED_IN_B1) >> 10);
}

uint8_t pmu_auxadc_get_efuse_slope_value(void) {
    return ((gAuxadcEfuseInfo.byte1 & AUXADC_EFUSE_O_SLOPE_IN_B1) >> 4);
}

uint8_t pmu_auxadc_get_id(void) {
    return ((gAuxadcEfuseInfo.byte1 & AUXADC_EFUSE_ID_IN_B1) >> 11);
}

uint16_t pmu_auxadc_get_vts(void) {
    return ((gAuxadcEfuseInfo.byte0 & AUXADC_EFUSE_MASK_OVTS_IN_BO) >> 7) + ((gAuxadcEfuseInfo.byte1 & AUXADC_EFUSE_MASK_OVTS_IN_B1) << 9);
}

uint8_t pmu_auxadc_get_degc_cali(void) {
    return (gAuxadcEfuseInfo.byte0 & AUXADC_EFUSE_MASK_DEGC_CALI_IN_B0);
}

uint8_t pmu_auxadc_get_adc_cali_en(void) {
    return ((gAuxadcEfuseInfo.byte0 & AUXADC_EFUSE_MASK_ADC_CALI_EN_IN_B0) >> 6);
}

uint8_t pmu_core_auxadc_check_id(void) {
    return ((gAuxadcEfuseInfo.byte1 & AUXADC_EFUSE_ID_IN_B1) >> 11);
}


#endif /* HAL_PMU_AUXADC_MODULE_ENABLED */
