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

#define __CHRDET_RG_DUMP__x

extern battery_managerment_control_info_t bm_ctrl_info;

void bc12_set_register_value(uint32_t address, short int mask, short int shift, short int value) {
    pmu_set_register_value_internal(address, mask, shift, value);
}

uint32_t bc12_get_register_value(uint32_t address, short int mask, short int shift) {
    return pmu_get_register_value_internal(address, mask, shift);
}


void battery_management_bc12_init(void) {
    battery_management_bc12_set_ibias_en_v12(1);
    hal_gpt_delay_ms(10);
    battery_management_bc12_set_src_en_v12(0);
    battery_management_bc12_set_vref_vth_en_v12(0);
    battery_management_bc12_set_cmp_en_v12(0);
    battery_management_bc12_set_ipu_en_v12(0);
    battery_management_bc12_set_ipd_en_v12(0);
    battery_management_bc12_set_ipd_half_en_v12(0);
}
/*data contact detect*/
uint8_t battery_management_bc12_dcd(void) {
    uint8_t value = 0xFF;
    battery_management_bc12_set_src_en_v12(0);
    battery_management_bc12_set_vref_vth_en_v12(1);
    battery_management_bc12_set_cmp_en_v12(2);
    battery_management_bc12_set_ipu_en_v12(2);
    battery_management_bc12_set_ipd_en_v12(0);
    battery_management_bc12_set_dcd_en_v12(1);
    battery_management_bc12_set_ipd_half_en_v12(0);

    hal_gpt_delay_ms(10);
    value = battery_management_bc12_get_qi_bc12_cmp_out();
    hal_gpt_delay_ms(10);
    return value;
}

uint8_t battery_management_bc12_primary_detction(void) {
    uint8_t compareResult = 0xFF;
    battery_management_bc12_set_src_en_v12(2);
    battery_management_bc12_set_vref_vth_en_v12(0);
    battery_management_bc12_set_cmp_en_v12(1);
    battery_management_bc12_set_ipu_en_v12(0);
    battery_management_bc12_set_ipd_en_v12(1);
    battery_management_bc12_set_ipd_half_en_v12(0);
    hal_gpt_delay_ms(40);
    compareResult = battery_management_bc12_get_qi_bc12_cmp_out();
    hal_gpt_delay_ms(20);
    return compareResult;
}

uint8_t battery_management_bc12_check_dp(void) {
    uint8_t compareResult = 0xFF;
    battery_management_bc12_set_src_en_v12(0);
    battery_management_bc12_set_vref_vth_en_v12(0);
    battery_management_bc12_set_cmp_en_v12(2);
    battery_management_bc12_set_ipu_en_v12(0);
    battery_management_bc12_set_ipd_en_v12(2);
    battery_management_bc12_set_ipd_half_en_v12(0);
    hal_gpt_delay_ms(10);
    compareResult = battery_management_bc12_get_qi_bc12_cmp_out();
    hal_gpt_delay_ms(10);
    return compareResult;
}

uint8_t battery_management_bc12_check_dm(void) {
    uint8_t compareResult = 0xFF;
    battery_management_bc12_set_src_en_v12(0);
   // battery_management_bc12_set_vref_vth_en_v12(0);
    battery_management_bc12_set_cmp_en_v12(1);
    battery_management_bc12_set_ipu_en_v12(0);
    battery_management_bc12_set_ipd_en_v12(1);
    battery_management_bc12_set_ipd_half_en_v12(0);
    hal_gpt_delay_ms(10);
    compareResult = battery_management_bc12_get_qi_bc12_cmp_out();
    hal_gpt_delay_ms(10);
    return compareResult;
}

uint8_t battery_management_bc12_secondary_detection(void) {
    uint8_t compareResult = 0xFF;
    battery_management_bc12_set_src_en_v12(1);
    battery_management_bc12_set_vref_vth_en_v12(0);
    battery_management_bc12_set_cmp_en_v12(2);
    battery_management_bc12_set_ipu_en_v12(0);
    battery_management_bc12_set_ipd_en_v12(2);
    battery_management_bc12_set_ipd_half_en_v12(0);
    hal_gpt_delay_ms(40);
    compareResult = battery_management_bc12_get_qi_bc12_cmp_out();
    hal_gpt_delay_ms(20);
    return compareResult;
}

uint8_t battery_management_bc12_check_floating(void) {
    uint8_t compareResult = 0xFF;
    battery_management_bc12_set_src_en_v12(0);
    battery_management_bc12_set_vref_vth_en_v12(0);
    battery_management_bc12_set_cmp_en_v12(1);
    battery_management_bc12_set_ipu_en_v12(0);
    battery_management_bc12_set_ipd_en_v12(1);
    battery_management_bc12_set_ipd_half_en_v12(1);
    hal_gpt_delay_ms(10);
    compareResult = battery_management_bc12_get_qi_bc12_cmp_out();
    hal_gpt_delay_ms(10);
    return compareResult;
}

uint8_t battery_management_bc12_check_DCP(void) {
    uint8_t compareResult = 0xFF;
    battery_management_bc12_set_src_en_v12(0);
    battery_management_bc12_set_vref_vth_en_v12(0);
    battery_management_bc12_set_cmp_en_v12(1);
    battery_management_bc12_set_ipu_en_v12(0);
    battery_management_bc12_set_ipd_en_v12(2);
    battery_management_bc12_set_ipd_half_en_v12(0);

    hal_gpt_delay_ms(10);
    battery_management_bc12_set_ipd_en_v12(0);
    hal_gpt_delay_us(200);
    compareResult = battery_management_bc12_get_qi_bc12_cmp_out();
    return compareResult;
}
uint8_t battery_management_get_charger_type_bc12(void) {
    battery_management_bc12_init();
    if (battery_management_bc12_dcd() == 0) {                       //SDP/CDP/DCP/S charger
        if (battery_management_bc12_primary_detction() == 0) {      //SDP
            battery_management_bc12_end();
            LOG_MSGID_I(battery_management, "BC12 SDP Charger\r\n", 0);
            return SDP_CHARGER;
        } else {                                                    //CDP/DCP/S charger
            if(battery_management_bc12_secondary_detection()==0){   //CDP
                battery_management_bc12_end();
                LOG_MSGID_I(battery_management, "BC12 CDP Charger\r\n", 0);
                return CDP_CHARGER;
            }else{
                 if(battery_management_bc12_check_DCP()==0){          //DCP
                     battery_management_bc12_end();
                     LOG_MSGID_I(battery_management, "BC12 DCP Charger\r\n", 0);
                     return DCP_CHARGER;
                 }else{                                             //S charger
                     battery_management_bc12_end();
                     LOG_MSGID_I(battery_management, "BC12 SS Charger\r\n", 0);
                     return SS_TABLET_CHARGER;
                 }
            }
        }
    } else {                                                        //apple /non-standard/ DP&DM floating charger
        uint8_t dp_out, dm_out;
        dp_out = battery_management_bc12_check_dp();
        dm_out = battery_management_bc12_check_dm();
        if((dp_out==0)&&(dm_out==0)){
            if(battery_management_bc12_check_floating()==0){                 //DP&DM floating charger
                battery_management_bc12_end();
                LOG_MSGID_I(battery_management, "BC12 DP&DM Floating\r\n", 0);
                return DP_DM_FLOATING;
            }else{                                                      //non-standard charger
                battery_management_bc12_end();
                LOG_MSGID_I(battery_management, "BC12 NON-STD charger\r\n", 0);
                return NON_STD_CHARGER;
            }
        }else if((dp_out==0)&&(dm_out==1)){                          //apple 5V 1A charger
                battery_management_bc12_end();
                LOG_MSGID_I(battery_management, "BC12 IPHONE_5V_1A Charger\r\n", 0);
                return IPHONE_5V_1A_CHARGER;
        }else{                                                      //apple ipad2/ipad4 charger
            battery_management_bc12_end();
            LOG_MSGID_I(battery_management, "BC12 IPAD2_IPAD4 Charger\r\n", 0);
            return IPAD2_IPAD4_CHARGER;
        }
    }
}

void battery_management_charger_type_setting(uint8_t port) {
    switch (bm_ctrl_info.chargerType)
    {
        case SDP_CHARGER:
            battery_set_icl_curent_level(ICL_ITH_400mA);
            break;
        case CDP_CHARGER:
            battery_set_icl_curent_level(ICL_ITH_600mA);
            break;
        case DCP_CHARGER:
            battery_set_icl_curent_level(ICL_ITH_800mA);
            break;
        default:
            battery_set_icl_curent_level(ICL_ITH_400mA);
            break;
    }
    LOG_MSGID_I(battery_management, "Get ICL Current After setting:%d\r\n",1, battery_get_icl_curent_level());
}

void battery_management_bc12_action_cdp_port(void) //note
{
    hal_gpt_delay_ms(100);
    battery_management_bc12_init();
    hal_gpt_delay_ms(100);
    battery_management_bc12_dcd();
    hal_gpt_delay_ms(100);
    battery_management_bc12_primary_detction();
}
void battery_management_bc12_end(void) {
    battery_management_bc12_set_ibias_en_v12(0);
    battery_management_bc12_set_src_en_v12(0);
    battery_management_bc12_set_vref_vth_en_v12(0);
    battery_management_bc12_set_cmp_en_v12(0);
    battery_management_bc12_set_ipu_en_v12(0);
    battery_management_bc12_set_ipd_en_v12(0);
    battery_management_bc12_set_ipd_half_en_v12(0);
}

void battery_management_bc12_debug(uint8_t port) {
    uint8_t chargerCurrent = 0xFF;
    chargerCurrent = battery_get_icl_curent_level();
    switch (bm_ctrl_info.chargerType)
    {
        case SDP_CHARGER:
            LOG_MSGID_I(battery_management, "SDP Charger ICL Current:%d\r\n",1, chargerCurrent);
            break;
        case CDP_CHARGER:
            LOG_MSGID_I(battery_management, "CDP Charger ICL Current:%d\r\n",1, chargerCurrent);
            break;
        case DCP_CHARGER:
            LOG_MSGID_I(battery_management, "DCP Charger ICL Current:%d\r\n",1, chargerCurrent);
            break;
        default:
            LOG_MSGID_I(battery_management, "Unknow Charger ICL Current:%d\r\n",1, chargerCurrent);
            break;
    }
}
