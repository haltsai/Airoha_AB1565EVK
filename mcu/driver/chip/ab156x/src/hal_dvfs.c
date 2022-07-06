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

#include "hal_dvfs.h"

#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal.h"
#include "hal_dvfs_internal.h"
#include "hal_clock.h"
#ifdef  HAL_DVFS_DEBUG_ENABLE
#define dvfs_debug(_message,...) log_hal_info("[DVFS] "_message, ##__VA_ARGS__)
#else
#define dvfs_debug(_message,...)
#endif

#define RESET_DVFS_LEVEL(index)      (DVFS_LOCK_STATUS ? search_locked_lv(index) : domain->basic_opp_index)
#define DVFS_LOCK_STATUS              dvfs_frequency_Ctrl.lock_ctrl
static dvfs_opp_t *domain;
static dvfs_frequency_t dvfs_cur_backup=0 ;
hal_dvfs_status_t dvfs_target_frequency(dvfs_switch_t, dvfs_frequency_t);
ATTR_RWDATA_IN_TCM dvfs_lock_ctrl_t dvfs_frequency_Ctrl = { .lock_ctrl = 0};
ATTR_TEXT_IN_TCM dvfs_frequency_t search_locked_lv(dvfs_frequency_t index){
search :
    if(index <= domain->basic_opp_index) { return domain->basic_opp_index;}
    else if(frequency_Ctrl[index])  { return index ;}
    else if(--index)           { goto search  ;}
    else                       { return domain->basic_opp_index;}
}

dvfs_frequency_t hal_dvfs_lock_level(hal_dvfs_lock_parameter_t lock){
    uint32_t irq_mask = 0;
    hal_nvic_save_and_set_interrupt_mask(&irq_mask);
    if(lock == HAL_DVFS_LOCK && !dvfs_cur_backup){
        dvfs_cur_backup = domain->lock_index;
        domain->lock_index = HAL_DVFS_HIGH_SPEED_208M;
    }else if(lock == HAL_DVFS_UNLOCK){
        domain->lock_index = search_locked_lv(HAL_DVFS_HIGH_SPEED_208M);
        dvfs_cur_backup = 0;
        dvfs_target_frequency(NORMAL,domain->lock_index);
    }
    hal_nvic_restore_interrupt_mask(irq_mask);
    return domain->lock_index;
}

int dvfs_query_frequency(uint32_t freq, const dvfs_opp_t *dvfs_domain, hal_dvfs_freq_relation_t relation){
    uint32_t opp;
    uint8_t upper_bound = HAL_DVFS_HIGH_SPEED_208M, lower_bound = DVFS_LOW_VOLT_LV;
    for (opp = HAL_DVFS_HIGH_SPEED_208M ; opp >= DVFS_LOW_VOLT_LV; opp--){
        if (freq == dvfs_domain->frequency[opp]) {
            return opp;
        }else if(freq < dvfs_domain->frequency[opp]){
            upper_bound = opp;
        }else{
            lower_bound = opp;
            break;
        }
    }
    opp = relation == HAL_DVFS_FREQ_RELATION_H ? upper_bound : lower_bound;
    return  opp; //DVFS_MODE_NONE;
}

hal_dvfs_status_t dvfs_target_frequency(dvfs_switch_t type, dvfs_frequency_t next_opp_index){
    hal_dvfs_status_t ret = HAL_DVFS_STATUS_OK;
    ret = domain->switch_frequency(type, next_opp_index);
    log_hal_msgid_info("DVFS [%d] \r\n",1, hal_clock_get_freq_meter(_hf_fsys_ck, 100)/2);

    return ret;
}

hal_dvfs_status_t hal_dvfs_lock_control(dvfs_frequency_t freq,hal_dvfs_lock_parameter_t lock) {
    uint32_t xLinkRegAddr = (uint32_t)__builtin_return_address(0);

    log_hal_msgid_info("DVFS op:%d, idx%d:[%d khz], caller:[0x%x]\r\n", 4, lock, freq, domain->frequency[freq], xLinkRegAddr);

    if (lock == HAL_DVFS_LOCK ) {
        dvfs_target_frequency(LOCK,freq);
    }else if( lock == HAL_DVFS_UNLOCK ){
        dvfs_target_frequency(UNLOCK,freq);
    }
    return HAL_DVFS_STATUS_OK;
}

uint32_t hal_dvfs_get_cpu_frequency(void){
    if (!domain) {
        log_hal_msgid_info("[%s] non-init\r\n",1, __FUNCTION__);
        return 0;
    }
    return domain->frequency[domain -> cur_opp_index];
}

hal_dvfs_status_t hal_dvfs_get_cpu_frequency_list(const uint32_t **list, uint32_t *list_num){
    if (!domain) {
        log_hal_msgid_info("[%s] non-init\r\n",1, __FUNCTION__);
        return HAL_DVFS_STATUS_UNINITIALIZED;
    }
    *list = domain->frequency;
    *list_num = domain->opp_num;

    return HAL_DVFS_STATUS_OK;
}


hal_dvfs_status_t hal_dvfs_target_cpu_frequency(uint32_t target_freq, hal_dvfs_freq_relation_t relation) {
    dvfs_frequency_t next_idx = (unsigned int)dvfs_query_frequency(target_freq, domain, relation);
    return dvfs_target_frequency(NORMAL, next_idx);
}

hal_dvfs_status_t hal_dvfs_init(void){
    if (!domain) {
        domain = dvfs_domain_init();
        if (!domain) {
            log_hal_msgid_info("[%s] vcore initialized failed\r\n",1, __FUNCTION__);
            return HAL_DVFS_STATUS_ERROR;
        }
    }
    return HAL_DVFS_STATUS_OK;
}
void dvfs_debug_dump(void){}

#endif /* HAL_DVFS_MODULE_ENABLED */

