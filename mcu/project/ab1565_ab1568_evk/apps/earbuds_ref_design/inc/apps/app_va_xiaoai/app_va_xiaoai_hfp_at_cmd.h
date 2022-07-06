
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

/**
 * File: app_va_xiaoai_hfp_at_cmd.h
 *
 * Description: This file defines the interface of app_va_xiaoai_hfp_at_cmd.c.
 *
 */

#ifndef __APP_VA_XIAOAI_HFP_AT_CMD_H__
#define __APP_VA_XIAOAI_HFP_AT_CMD_H__

#ifdef MTK_VA_XIAOAI_ENABLE

#include "stdbool.h"
#include "stdint.h"
#include "bt_type.h"

bool app_va_xiaoai_hfp_miui_basic_atcmd();

void app_va_xiaoai_hfp_miui_more_atcmd_report_feature();
void app_va_xiaoai_hfp_miui_more_atcmd_report_all_status();

// Use type=0x00 for single_state_report
void app_va_xiaoai_hfp_miui_more_atcmd_report_anc();
void app_va_xiaoai_hfp_miui_more_atcmd_report_key();
void app_va_xiaoai_hfp_miui_more_atcmd_report_voice();
void app_va_xiaoai_hfp_miui_more_atcmd_report_eq();
void app_va_xiaoai_hfp_miui_more_atcmd_report_game_mode();
void app_va_xiaoai_hfp_miui_more_atcmd_report_anti_lost();

void xiaoai_va_xiaoai_hfp_at_cmd_register(bool enable);


#endif /* MTK_XIAOAI_ENABLED */

#endif /* __APP_VA_XIAOAI_HFP_AT_CMD_H__ */

