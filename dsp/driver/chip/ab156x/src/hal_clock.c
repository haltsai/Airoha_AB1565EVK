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

#include "hal_clock_internal.h"
#include "hal_gpt.h"
#ifdef HAL_CLOCK_MODULE_ENABLED

void hal_clock_dcm_enable(hal_clk_dcm_id dcm_id){
    switch(dcm_id){
    case clk_dsp_dcm :
        *DSP0_SLOW_CON0 = 0x1;
        BUS_DCM_CON_REG.RG_BUS_DCM_EN |= 0x4;
        break;
    default :
        return;
    }

    hal_gpt_delay_us(1);
}

void hal_clock_dcm_disable(hal_clk_dcm_id dcm_id){
    switch(dcm_id){

    case clk_dsp_dcm :
        BUS_DCM_CON_REG.RG_BUS_DCM_EN &= 0xB;
        break;
    default :
        return;
    }

    hal_gpt_delay_us(1);
}
hal_clock_status_t hal_clock_disable(hal_clock_cg_id clock_id){
	hal_clock_status_t ret = HAL_CLOCK_STATUS_OK;
    volatile uint32_t *set_addr = NULL;
    uint32_t bit_idx = clock_id & 0x1f;

    if (clock_id <= HAL_CLOCK_CG_AUXADC) {
        set_addr = XO_PDN_PD_SET0;
    } else if (clock_id <= HAL_CLOCK_CG_GPTIMER) {
        set_addr = XO_PDN_AO_SET0;
    } else if (clock_id <= HAL_CLOCK_CG_DEBUGSYS) {
        set_addr = XO_PDN_TOP_SET0;
    }else if (clock_id <= HAL_CLOCK_CG_USB_DMA) {
        set_addr = PDN_PD_SET0;
    }else if(clock_id == HAL_CLOCK_CG_CMSYS_ROM){
        set_addr = PDN_TOP_SET0;
    }else {
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
    }

    *(set_addr) |= (0x1 << bit_idx);
    return ret;
}
hal_clock_status_t hal_clock_enable(hal_clock_cg_id clock_id){
	hal_clock_status_t ret = HAL_CLOCK_STATUS_OK;

    volatile uint32_t *clr_addr = NULL;
    uint32_t bit_idx = clock_id & (0x1f);

    if (clock_id <= HAL_CLOCK_CG_AUXADC) {
        clr_addr = XO_PDN_PD_CLRD0;
    } else if (clock_id <= HAL_CLOCK_CG_GPTIMER) {
        clr_addr = XO_PDN_AO_CLRD0;
    } else if (clock_id <= HAL_CLOCK_CG_DEBUGSYS) {
        clr_addr = XO_PDN_TOP_CLRD0;
    }else if (clock_id <= HAL_CLOCK_CG_USB_DMA) {
        clr_addr = PDN_PD_CLRD0;
    }else if(clock_id == HAL_CLOCK_CG_CMSYS_ROM){
        clr_addr = PDN_TOP_CLRD0;
    }else {
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
    }
    *(clr_addr) |= (0x1 << bit_idx);

    return ret;
}

#endif /* HAL_CLOCK_MODULE_ENABLED */



