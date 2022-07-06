/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
 
#include "hal_dvfs_internal.h"

#ifdef HAL_DVFS_MODULE_ENABLED
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "hal.h"
#include "hal_gpt.h"
#include "hal_clock.h"
#include "hal_pmu.h"
#include "hal_dvfs_internal.h"
#include "hal_clock_internal.h"
#include "hal_nvic_internal.h"
#ifdef  HAL_DVFS_DEBUG_ENABLE
#define dvfs_debug(_message,...) log_hal_info("[DVFS] "_message, ##__VA_ARGS__)
#else
#define dvfs_debug(_message,...)
#endif

#define VCORE_DYNAMIC_SCALING(PTR_CUR,PTR_NEXT)     if(PTR_CUR!= PTR_NEXT) {pmu_lock_vcore(PMU_NORMAL,PTR_NEXT->vcore,PMU_LOCK);\
                                                                            pmu_lock_vcore(PMU_NORMAL,PTR_CUR->vcore,PMU_UNLOCK);}
#define PLATFORM_26M_PROTECTION_MASK                0xFFFF
#define DVFS_100_MS                                 100000

hal_dvfs_status_t dvfs_vcore_switch_frequency( dvfs_switch_t type, unsigned int next_opp);
hal_dvfs_status_t dvfs_vcore_switch_voltage( dvfs_switch_t type,unsigned int c_voltage, unsigned int n_voltage);
extern dvfs_lock_ctrl_t dvfs_frequency_Ctrl;
typedef enum {
    DVFS_VCORE_MODE_LOW_SPEED = 26000,
    DVFS_VCORE_MODE_HELF_SPEED = 52000,
    DVFS_VCORE_MODE_FULL_SPEED = 104000,
    DVFS_VCORE_MODE_HIGH_SPEED = 208000
}dvfs_vcore_mode_t;
//------------[DVFS Variable Declaration]

static const uint32_t dvfs_vcore_frequency[HAL_DVFS_MAX_SPEED] = { DVFS_VCORE_MODE_LOW_SPEED,
                                                               DVFS_VCORE_MODE_HELF_SPEED,
                                                               DVFS_VCORE_MODE_FULL_SPEED,
                                                               DVFS_VCORE_MODE_HIGH_SPEED };


typedef struct {
    pmu_power_vcore_voltage_t vcore;

}dvfs_vsram_vcore_mapping_t;


ATTR_RWDATA_IN_TCM static dvfs_vsram_vcore_mapping_t pmu_mapping_tbl_[HAL_DVFS_MAX_SPEED] = {
    {.vcore = HAL_DVFS_VCORE_TEST_LOWV },
    {.vcore = HAL_DVFS_VCORE_LOWV },
    {.vcore = PMIC_VCORE_0P8_V  },
    {.vcore = PMIC_VCORE_0P9_V  },
};

ATTR_RWDATA_IN_TCM static dvfs_opp_t domain={
    .opp_num = HAL_DVFS_MAX_SPEED, //max number about can choise voltage
    .module_num =1,
    .cur_opp_index = HAL_DVFS_HIGH_SPEED_208M,
    .switch_voltage = NULL,//function about switch voltage
    .switch_frequency = dvfs_vcore_switch_frequency,// function about switch frequency
    .frequency = dvfs_vcore_frequency,//set all frequency
    .dvfs_switch_interval = 0 ,
#ifdef MTK_SYSTEM_CLOCK_104M
    .basic_opp_index = HAL_DVFS_FULL_SPEED_104M,
#elif defined(MTK_SYSTEM_CLOCK_208M)
    .basic_opp_index = HAL_DVFS_HIGH_SPEED_208M,
#elif defined(MTK_SYSTEM_CLOCK_52M)
    .basic_opp_index = DVFS_LOW_VOLT_LV,
#endif
    .lock_index = DVFS_TEST_VOLT_LV,
};

//------------[DVFS basic setting api]
hal_dvfs_status_t  dvfs_vcore_switch_voltage(dvfs_switch_t type, unsigned int c_voltage, unsigned int n_voltage){
    dvfs_vsram_vcore_mapping_t *pmu_current = &pmu_mapping_tbl_[c_voltage];
    dvfs_vsram_vcore_mapping_t *pmu_next = &pmu_mapping_tbl_[n_voltage];
    *CKSYS_CLK_CFG_0 &= ~(PLATFORM_26M_PROTECTION_MASK);
    *CKSYS_CLK_FORCE_ON_0 = 0x101;
    *CKSYS_CLK_UPDATE_0 |= 0x100;
    *CKSYS_CLK_UPDATE_0 |= 0x1;
    while(*CKSYS_CLK_UPDATE_0 & PLATFORM_26M_PROTECTION_MASK);
    while(*CKSYS_CLK_UPDATE_STATUS_0 & PLATFORM_26M_PROTECTION_MASK);
    *CKSYS_CLK_FORCE_ON_0 = 0;
#ifdef HAL_PMU_MODULE_ENABLED
    VCORE_DYNAMIC_SCALING(pmu_current,pmu_next);
#endif

    return HAL_DVFS_STATUS_OK;
}

dvfs_opp_t *dvfs_domain_init(){
    uint8_t clk_sys_clk_sel = clock_mux_cur_sel(CLK_DSP_SEL);
#ifdef MTK_DVFS_LOWV_OP73V
    pmu_mapping_tbl_[HAL_DVFS_HALF_SPEED_52M_W_LDSP].vcore = M_RSV_B ? PMIC_VCORE_0P7_V : PMIC_VCORE_0P73_V);
#endif
    switch(clk_sys_clk_sel){
    case DVFS_DSP_CLOCK_83M:
        pmu_lock_vcore(PMU_NORMAL,pmu_mapping_tbl_[HAL_DVFS_HALF_SPEED_52M_W_LDSP].vcore,PMU_LOCK);
        domain.cur_opp_index = HAL_DVFS_HALF_SPEED_52M_W_LDSP;
        break;
    case DVFS_DSP_CLOCK_104M:
        pmu_lock_vcore(PMU_NORMAL,pmu_mapping_tbl_[HAL_DVFS_HALF_SPEED_52M].vcore,PMU_LOCK);
        domain.cur_opp_index = HAL_DVFS_HALF_SPEED_52M;
        break;
    case DVFS_DSP_CLOCK_208M:
        pmu_lock_vcore(PMU_NORMAL,PMIC_VCORE_0P8_V,PMU_LOCK);
        domain.cur_opp_index = HAL_DVFS_FULL_SPEED_104M;
        break;
    case DVFS_DSP_CLOCK_416M:
        pmu_lock_vcore(PMU_NORMAL,PMIC_VCORE_0P9_V,PMU_LOCK);
        domain.cur_opp_index = HAL_DVFS_HIGH_SPEED_208M;
        break;
    default :
        return NULL;
    }
    return &domain;
}

ATTR_TEXT_IN_TCM hal_dvfs_status_t dvfs_vcore_switch_frequency( dvfs_switch_t type, unsigned int next_opp)
{

    uint32_t irq_mask=0,dvfs_cur_ticks = 0 ;
    unsigned int pmu_unlock_lv;
    hal_dvfs_status_t result = HAL_DVFS_STATUS_OK;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &dvfs_cur_ticks);
    if(!domain.dvfs_switch_interval){
        domain.dvfs_switch_interval = dvfs_cur_ticks;
    }else if((dvfs_cur_ticks - domain.dvfs_switch_interval) < DVFS_100_MS){
        log_hal_msgid_info("DVFS INTERVAL TOO SHORT [%d] \r\n",1,(dvfs_cur_ticks - domain.dvfs_switch_interval));
    }

    domain.dvfs_switch_interval = dvfs_cur_ticks;
    pmu_get_lock_status(0);
    hal_nvic_save_and_set_interrupt_mask_special(&irq_mask);

    pmu_unlock_lv = domain.cur_opp_index;

    if(type == UNLOCK ){
        if(frequency_Ctrl[next_opp] && --frequency_Ctrl[next_opp])     { goto FREQ_SWICH_END; }
        else if(next_opp == domain.lock_index)                         { domain.lock_index = search_locked_lv(next_opp);
                                                                         next_opp = domain.lock_index;}
        else                                                           { goto FREQ_SWICH_END; }

    }else if(type == LOCK){
        if(!frequency_Ctrl[next_opp]++ && domain.lock_index < next_opp){ domain.lock_index = next_opp ;}
        else                                                           { goto FREQ_SWICH_END;}

    }else if(domain.lock_index > next_opp){ //NORMAL
        goto FREQ_SWICH_END;
    }
#ifdef HAL_ESC_MODULE_ENABLED
    esc_mask_channel();
#endif
    if(next_opp != domain.cur_opp_index){
        dvfs_vcore_switch_voltage(type,pmu_unlock_lv,next_opp);
    }
    switch (next_opp) {
        case HAL_DVFS_HIGH_SPEED_208M:
        case HAL_DVFS_FULL_SPEED_104M:
        case HAL_DVFS_HALF_SPEED_52M_W_LDSP:
            CKSYS_CLK_DIV_REG.CLK_OSC1_D5_EN = 1;
            CKSYS_CLK_DIV_REG.CLK_OSC1_D2_EN = 1;
            dvfs_switch_clock_freq(domain.cur_opp_index,next_opp);
            domain.cur_opp_index = next_opp;
            break;
        case HAL_DVFS_HALF_SPEED_52M:
            CKSYS_CLK_DIV_REG.CLK_OSC1_D5_EN = 1;
            CKSYS_CLK_DIV_REG.CLK_OSC1_D2_EN = 1;
            dvfs_switch_clock_freq(domain.cur_opp_index,next_opp);
            domain.cur_opp_index = next_opp;
            break;
        default:
            assert(0);
    }
#ifdef HAL_ESC_MODULE_ENABLED
    esc_unmask_channel();
#endif
    SystemCoreClockUpdate();

FREQ_SWICH_END:
    hal_nvic_restore_interrupt_mask_special(irq_mask);
    pmu_get_lock_status(1);
    return result;
}

#endif /* HAL_DVFS_MODULE_ENABLED */

