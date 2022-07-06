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

#include <stdio.h>
#include <stdlib.h>
#include "hal_feature_config.h"
#include "hal.h"
//#define DEBUG_MONITOR ((volatile uint32_t*) 0xA2010008)
#ifdef HAL_DCXO_MODULE_ENABLED
#include "hal_clock_internal.h"
#define __EOSC_32K__
unsigned int dcxo_capid = 0;
void set_capid()
{
    if(dcxo_capid){
        DCXO_PCON_REG.DCXO_CAPID_EFUSE = dcxo_capid;
        DCXO_PCON_REG.DCXO_CAPID_EFUSE_SEL = 0x1;
        log_hal_msgid_info("Load CAPID done, CAPID RG = 0x%x\r\n",1, dcxo_capid);
    }
}

uint32_t get_capid(){
    return dcxo_capid;
}

ATTR_TEXT_IN_TCM void dcxo_lp_mode(dcxo_mode_t mode){
    if(mode == DCXO_LP_MODE){
        *DCXO_CTUNE_LPM = DCXO_PCON_REG.DCXO_CAPID_EFUSE;
        DCXO_CORE_ISEL_LPM_REG.ISEL_LPM = *DCXO_CORE_ISEL;
    }
    DCXO_PCON_REG.EN_26M_FPM = mode;
    DCXO_PCON_REG.BT_26M_EN = mode;
}

void hal_dcxo_init(void)
{

    set_capid();
    DCXO_PCON_REG.GSM_DCXO_CTL_EN=0x1;
    DCXO_PCON_REG.EXT_DCXO_CTL_EN = 0x1;
#ifdef __EOSC_32K__
    DCXO_PCON_REG.DCXO_PWR_EN_TD = 0x1;
    DCXO_PCON_REG.DCXO_EN_TD=0x1;
    DCXO_PCON_REG.DCXO_BUF_EN_TD=0x52;
#else //32kless
    DCXO_PCON_REG.DCXO32K_EN = 0x1;
    DCXO_PCON_REG.DCXO_PWR_EN_TD = 0x1;
    DCXO_PCON_REG.DCXO_EN_TD = 0x1;
    DCXO_PCON_REG.DCXO_BUF_EN_TD=0x14;

#endif
    DCXO_PCON_REG.DCXO_ISO_EN_TD=0x6;
    DCXO_PCON_REG.DCXO_SLEEP_TD=0xA;
}

#endif /*HAL_DCXO_MODULE_ENABLED*/
