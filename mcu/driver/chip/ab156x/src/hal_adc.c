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

#include "hal_adc.h"

#ifdef HAL_ADC_MODULE_ENABLED

#include "hal_clock.h"
#include "hal_log.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
volatile static uint32_t s_macro_con2;
volatile static uint32_t s_ana_en_con;

void adc_backup_all_register(void);
void adc_restore_all_register(void);
#endif

//#define ADC_DEBUG_LOG
#ifdef ADC_DEBUG_LOG
#define ADC_DEBUG_INF(fmt,...) printf(fmt,##__VA_ARGS__)
#else
#define ADC_DEBUG_INF(fmt,...)
#endif


#ifdef HAL_ADC_CALIBRATION_ENABLE
double adc_a , adc_b;
void adc_data_calibrate(hal_adc_channel_t channel, uint32_t input_data, uint32_t *output_data);
#endif

uint32_t g_adc_raw_data[HAL_ADC_CHANNEL_MAX] = {0};
volatile uint32_t adc_resource_control;

hal_adc_status_t hal_adc_init(void)
{
    uint32_t int_mask;
#ifdef HAL_ADC_CALIBRATION_ENABLE
    uint16_t adc_GE_data=0,adc_OE_data=0;
    uint32_t effuse_data=0;
    effuse_data = *(volatile uint32_t *)(0xA20A0404);
    if(0==effuse_data )
    {
        ADC_DEBUG_INF("the effuse no avilable value ");
    }
    else
    {
        effuse_data = effuse_data&0x7FFFF;
        adc_GE_data = effuse_data&0x3FF;
        adc_OE_data = (effuse_data>>10)&0x1FF;
        ADC_DEBUG_INF("the adc_effuse_ge_data = [%d]  and  the adc_effuse_oe_data = [%d]\r\n",adc_GE_data,adc_OE_data);
        adc_a = (double)(adc_GE_data -256)/(double)4096;
        adc_b = (double)(adc_OE_data -128)/(double)4096;
        ADC_DEBUG_INF(" adc_a = %lf  adc_b = %lf \r\n", adc_a,adc_b);
    }
#endif/* HAL_ADC_CALIBRATION_ENABLE */
#ifdef HAL_SLEEP_MANAGER_ENABLED
    sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_AUXADC, (sleep_management_suspend_callback_t)adc_backup_all_register, NULL);
    sleep_management_register_resume_callback(SLEEP_BACKUP_RESTORE_AUXADC, (sleep_management_resume_callback_t)adc_restore_all_register, NULL);
#endif /*HAL_SLEEP_MANAGER_ENABLED*/

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    hal_nvic_save_and_set_interrupt_mask(&int_mask);
    adc_resource_control++;
    /* Check module status */
    if (adc_resource_control > 1) {
        
        /* Restore the previous status of interrupt */
        hal_nvic_restore_interrupt_mask(int_mask);
        return HAL_ADC_STATUS_OK;
    }


    /* Enable VA28_LDO */
    ADC->MACRO_CON2 |= MACRO_CON2_RG_AUXADC_LDO_EN_MASK;

    /* Enable AUXADC */
    //ADC->ANA_EN_CON |= ANA_EN_CON_AUXADC_EN_MASK;
    ADC->ANA_EN_CON = 0x101;
    /*Enable clock: *(volatile uint32_t *)(0xA2030b20) |= 0x200000; */
    if (HAL_CLOCK_STATUS_OK != hal_clock_enable(HAL_CLOCK_CG_AUXADC)) {
        //log_hal_msgid_error("\r\n [ADC] Clock enable failed!", 0);
        hal_nvic_restore_interrupt_mask(int_mask);
        return HAL_ADC_STATUS_ERROR;
    }

    /* Software reset ADC */
    ADC->AUXADC_CON3_UNION.AUXADC_CON3_CELLS.SOFT_RST = AUXADC_CON3_SOFT_RST_MASK;
    ADC->AUXADC_CON3_UNION.AUXADC_CON3_CELLS.SOFT_RST = 0;

    /* Reference voltage selection */
    *(uint32_t *)(0xA2070400) |= 0x100; 
    hal_nvic_restore_interrupt_mask(int_mask);
    return HAL_ADC_STATUS_OK;
}

hal_adc_status_t hal_adc_deinit(void)
{
    uint32_t int_mask;

    hal_nvic_save_and_set_interrupt_mask(&int_mask);
    adc_resource_control--; 
    
    if(adc_resource_control ==0){

    /* Disable clock: *(volatile uint32_t *)(0xA2030b10) |= 0x200000; */
        if (HAL_CLOCK_STATUS_OK != hal_clock_disable(HAL_CLOCK_CG_AUXADC)) {
        //log_hal_msgid_error("\r\n [ADC] Clock disable failed!", 0);
        hal_nvic_restore_interrupt_mask(int_mask);
        return HAL_ADC_STATUS_ERROR;
    }

    /* Disable AUXADC */
    ADC->ANA_EN_CON = 0;

    /* Disable VA28_LDO */
    ADC->MACRO_CON2 = 0;
    
    hal_nvic_restore_interrupt_mask(int_mask);
    return HAL_ADC_STATUS_OK;
    }else{
        hal_nvic_restore_interrupt_mask(int_mask);
        return HAL_ADC_STATUS_OK;
    }
}
volatile uint32_t adc_get_data_resource[HAL_ADC_CHANNEL_MAX];
hal_adc_status_t hal_adc_get_data_polling(hal_adc_channel_t channel, uint32_t *data)
{

    uint32_t times = 0;
    uint32_t temp_data = 0;
    uint32_t int_mask;

    /* Channel is invalid */
    if (channel >= HAL_ADC_CHANNEL_MAX) {
        log_hal_msgid_error("\r\n [ADC] Invalid channel: %d.",1, channel);
        return HAL_ADC_STATUS_ERROR_CHANNEL;
    }

    /* Parameter check */
    if (data == NULL) {
        log_hal_msgid_error("\r\n [ADC] Invalid parameter.", 0);
        return HAL_ADC_STATUS_INVALID_PARAMETER;
    }
    
    hal_nvic_save_and_set_interrupt_mask(&int_mask);
    /* changed by mtk60731 on 2020.2.10: from DE descrption,1568 adc gpio default as DGPIO,so,need to switch it to
      * AGPIO,otherwise adc will not work.
      */
   
    if(adc_get_data_resource[channel] == 0){
    *((volatile uint32_t *)(0xA20C0048)) |= 1 << (12 - channel);
    }
    adc_get_data_resource[channel]++; 
    hal_nvic_restore_interrupt_mask(int_mask);

    for (times = 0; times < 8; times++) {
        

        hal_nvic_save_and_set_interrupt_mask(&int_mask);
        /* Disable the corresponding region */
        ADC->AUXADC_CON1 = 0;
        ADC->AUXADC_CON1 = (1 << (uint16_t)channel);

        /* Wait until the module status is idle */
        while (ADC->AUXADC_CON3_UNION.AUXADC_CON3_CELLS.ADC_STAT & AUXADC_CON3_ADC_STA_MASK);

        /* Retrieve data for corresponding channel */
        switch (channel) {
            case HAL_ADC_CHANNEL_0:
                temp_data += ADC->AUXADC_DATA0;
                break;
            case HAL_ADC_CHANNEL_1:
                temp_data += ADC->AUXADC_DATA1;
                break;
            case HAL_ADC_CHANNEL_2:
                temp_data += ADC->AUXADC_DATA2;
                break;
            case HAL_ADC_CHANNEL_3:
                temp_data += ADC->AUXADC_DATA3;
                break;
            case HAL_ADC_CHANNEL_4:
                temp_data += ADC->AUXADC_DATA4;
                break;
            case HAL_ADC_CHANNEL_5:
                temp_data += ADC->AUXADC_DATA5;
                break;
            case HAL_ADC_CHANNEL_6:
                temp_data += ADC->AUXADC_DATA6;
                break;

            default:
                /* Should not run here */
                break;
        }
        hal_nvic_restore_interrupt_mask(int_mask);

    }

    hal_nvic_save_and_set_interrupt_mask(&int_mask);
    adc_get_data_resource[channel]--; 
    if(adc_get_data_resource[channel] == 0){
    /* Switch GPIO to digital mode after sample done */
    *((volatile uint32_t *)(0xA20C0044)) |= 1 << (12 - channel);
    } 
    hal_nvic_restore_interrupt_mask(int_mask);
    
    g_adc_raw_data[channel] = temp_data >> 3;

#ifdef ADC_CALIBRATION_ENABLE
    adc_data_calibrate(channel, temp_data >> 3, data);
#else
    *data = (temp_data >> 3);
#endif

    return HAL_ADC_STATUS_OK;
}
#ifdef HAL_ADC_SUPPORT_AVERAGE_ENABLE
hal_adc_status_t hal_adc_get_average_data(hal_adc_channel_t channel, hal_adc_average_t average_num, uint32_t *data)
{
    /* Channel is invalid */
    if (channel >= HAL_ADC_CHANNEL_MAX) {
        log_hal_msgid_error("\r\n [ADC] Invalid channel: %d.", 1, channel);
        return HAL_ADC_STATUS_ERROR_CHANNEL;
    }

    /* Parameter check */
    if (data == NULL) {
        log_hal_msgid_error("\r\n [ADC] Invalid parameter.", 0);
        return HAL_ADC_STATUS_INVALID_PARAMETER;
    }

    if (average_num >= HAL_ADC_AVERAGE_MAX) {
        log_hal_msgid_error("\r\n [ADC] Invalid parameter.", 0);
        return HAL_ADC_STATUS_INVALID_PARAMETER;
    }

    /* changed by mtk60731 on 2020.2.10: from DE descrption,1568 adc gpio default as DGPIO,so,need to switch it to
      * AGPIO,otherwise adc will not work.
      */
    *((volatile uint32_t *)(0xA20C0048)) |= 1 << (12 - channel);

    /* Disable the corresponding channel */
    ADC->AUXADC_CON1 = 0;

    /* Enable auto average */
    ADC->AUXADC_CON4 |= (1 << 8);

    /* Set average number */
    ADC->AUXADC_AVG_NUM = average_num;

    /* Start auxadc */
    ADC->AUXADC_CON1 = (1 << (uint16_t)channel);

    /* Wait until the module status is ready */
    while (ADC->AUXADC_AVG_READY == 0);

    /* Retrieve data for corresponding channel */
    *data = ADC->AUXADC_AVG_DATA >> 3;

    *((volatile uint32_t *)(0xA20C0044)) |= 1 << (12 - channel);

    /* change by suchuan on 2020.02.20:from DE descrption,when data ready flag set */
    /* Disable auto average */
    //ADC->AUXADC_CON4 &= ~(1 << 8);
    return HAL_ADC_STATUS_OK;
}
#endif /*HAL_ADC_SUPPORT_AVERAGE_ENABLE */

#ifdef HAL_SLEEP_MANAGER_ENABLED
void adc_backup_all_register(void)
{
    s_macro_con2 = ADC->MACRO_CON2;
    s_ana_en_con = ADC->ANA_EN_CON;

}

void adc_restore_all_register(void)
{
    ADC->MACRO_CON2 = s_macro_con2;
    ADC->ANA_EN_CON = s_ana_en_con;
}
#endif

#ifdef ADC_CALIBRATION_ENABLE

/* return false: No calibration for adc
 * return true:  Get the gain error and offset error code from efuse
 */
bool adc_get_gain_offset_error(uint32_t *gain_error, uint32_t *offset_error)
{
    uint32_t adc_efuse_data;

    /* Get factory efuse value */
    adc_efuse_data = *(volatile uint32_t *)(0xA20A0240);
    /* adc_efuse_data0 is 0 indicates that no need to calibrate */
    if (0 == adc_efuse_data) {
        return false;
    } else {
        *offset_error = ((adc_efuse_data >> 16) & 0xFFF);
        *gain_error = (uint32_t)((adc_efuse_data & 0x1FFF) * (1.0 - ((1.0 - (adc_efuse_data >> 31)) * 0.071429)) - 0.071429 * (1.0 - (adc_efuse_data >> 31)));
        return true;
    }
}

void adc_data_calibrate(hal_adc_channel_t channel, uint32_t input_data, uint32_t *output_data)
{
    uint32_t ADC_GE, ADC_OE;
    double adc_a, adc_b;
    double adc_voltage;

    /* Calibrate the raw code if the calibation is enabled */
    if (false == adc_get_gain_offset_error(&ADC_GE, &ADC_OE)) {
        *output_data = input_data;
        return;
    } else {
        /*
        * ADC_GE = A*4096 + 4096
        * ADC_OE = B*4096 + 2048
        */
        adc_a = (double)((int32_t)ADC_GE - 4096) / 4096;
        adc_b = (double)((int32_t)ADC_OE - 2048) / 4096;

        /*
        * adc_voltage = [(adc_data /4096 - B) * 1.4 / (1+A)]
        */

        adc_voltage = ((double)input_data / 4096 - adc_b) * (double)1.4 / (1 + adc_a);

        /*
        * (output_data/4096)*1.4 =  adc_voltage
        */

        *output_data = (uint32_t)((adc_voltage / (double)1.4) * 4096 + (double)0.5);

        if (*output_data > 4095) {
            *output_data = 4095;
        }
    }
    //printf("\r\nraw_code = 0x%04x, trimmed_code = 0x%04x, APB_AUXADC0[12:0] = %d, APB_AUXADC0[27:16] = %d, ADC_GE = %f, ADC_OE = %f", input_data, *output_data, ADC_GE, ADC_OE, adc_a, adc_b);
}
#endif /* AUXADC_CALIBRATION_ENABLE */

#ifdef HAL_ADC_CALIBRATION_ENABLE  
hal_adc_status_t hal_adc_get_calibraton_data(uint32_t raw_data, uint32_t  *cal_data)
{
    if (NULL == cal_data) {
        return HAL_ADC_STATUS_INVALID_PARAMETER;
    }
    

    
    *cal_data = (((double)raw_data / (double)4096 - adc_b )* (double)1.4 /(1+ adc_a ))* 2925;
    return HAL_ADC_STATUS_OK;
}

hal_adc_status_t hal_adc_get_calibraton_voltage(uint32_t raw_data, uint32_t *cal_voltage)
{
    if (NULL == cal_voltage) {
        return HAL_ADC_STATUS_INVALID_PARAMETER;
    }

    /* unit is mv */
    *cal_voltage = (((double)raw_data / (double)4096 - adc_b )* (double)1.4 /(1+ adc_a ))* 1000;
    return HAL_ADC_STATUS_OK;
}
  
#endif /* HAL_ADC_CALIBRATION_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* HAL_ADC_MODULE_ENABLED */

