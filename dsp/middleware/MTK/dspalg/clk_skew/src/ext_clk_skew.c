/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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

#include "hal_nvic.h"
#include "types.h"
#include "clk_skew.h"
#include "ext_clk_skew.h"
#include "long_term_clk_skew.h"

static U32 isr_bt_clk_prev,isr_bt_clk_next,cnt;
static U16 isr_bt_phase_prev,isr_bt_phase_next;
static S32 isr_drift = 0,isr_drift_acc = 0;
static S32 ul_cp_samples,dl_cp_samples;
ATTR_TEXT_IN_IRAM_LEVEL_2 void clk_skew_isr_time_initial(void)
{
    isr_bt_clk_next = BTCLK_INVALID_CLK;
    isr_bt_phase_next = BTINTRA_INVALID_CLK;
    isr_bt_clk_prev = BTCLK_INVALID_CLK;
    isr_bt_phase_prev = BTINTRA_INVALID_CLK;
    ul_cp_samples = 0;
    dl_cp_samples = 0;
    isr_drift = 0;
    cnt = 0;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void clk_skew_isr_time_update(U32 bt_clk_next,U16 bt_phase_next)
{          
    isr_bt_clk_next = bt_clk_next;
    isr_bt_phase_next = bt_phase_next;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void clk_skew_isr_time_accumulated_drift(CLK_SKEW_FS_t fs)
{
    S64 cnt_prev,cnt_next;
    
    if(isr_bt_clk_prev != BTCLK_INVALID_CLK){
        cnt_prev = isr_bt_clk_prev*8125 + isr_bt_phase_prev*13;
        cnt_next = isr_bt_clk_next*8125 + isr_bt_phase_next*13;
        isr_drift += ((S32)AFE_DL_Interval_Cnt - ((S32)cnt_next - (S32)cnt_prev))*((S32)fs);
        cnt++;
        if(cnt == clk_skew_isr_observe_cnt){
            cnt = 0;
            isr_drift_acc = isr_drift;
        }
        
        if(isr_drift_acc > 26000000){
            ul_cp_samples++;
            dl_cp_samples++;
            DSP_MW_LOG_I("[CLK_SKEW_ISR]acc++ isr_drift:%d, ul_cp:%d,dl_cp:%d",3,isr_drift_acc,ul_cp_samples,dl_cp_samples);
            isr_drift_acc -= 26000000;            
        }else if(isr_drift_acc < -26000000){
            ul_cp_samples--;
            dl_cp_samples--;
            DSP_MW_LOG_I("[CLK_SKEW_ISR]acc-- isr_drift:%d, ul_cp:%d,dl_cp:%d",3,isr_drift_acc,ul_cp_samples,dl_cp_samples);
            isr_drift_acc += 26000000;            
        }
        
    }
    isr_bt_clk_prev = isr_bt_clk_next;
    isr_bt_phase_prev = isr_bt_phase_next;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 S32 clk_skew_isr_time_get_dl_cp_samples(void)
{
    return dl_cp_samples;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void clk_skew_isr_time_set_dl_cp_samples(S32 cp_samples)
{
    dl_cp_samples = cp_samples;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 S32 clk_skew_isr_time_get_ul_cp_samples(void)
{
    return ul_cp_samples;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void clk_skew_isr_time_set_ul_cp_samples(S32 cp_samples)
{
    ul_cp_samples = cp_samples;
}
