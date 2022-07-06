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

#include "string.h"
#include "syslog.h"
#include "pka_porting_layer.h"


log_create_module(PKA_DM2L, PRINT_LEVEL_INFO);
log_create_module(PKA_LC, PRINT_LEVEL_INFO);
log_create_module(PKA_COMMON, PRINT_LEVEL_INFO);



ATTR_RODATA_IN_TCM log_control_block_t* pka_log_tab[] = {
    &log_control_block_PKA_DM2L,
    &log_control_block_PKA_LC,
    &log_control_block_PKA_COMMON,
};
COMPILE_TIME_ASSERT((sizeof(pka_log_tab)/sizeof(pka_log_tab[0]))==LOG_NUM,log_table_length_mismatch);



void bt_pka_log_msgid_i(pka_log_type_t type , const char *message, uint32_t arg_cnt, ...)
{

#if !defined (MTK_DEBUG_LEVEL_NONE)
    va_list ap;

    va_start(ap, arg_cnt);
    log_print_msgid(pka_log_tab[type], PRINT_LEVEL_INFO, message, arg_cnt, ap);
    va_end(ap);
#endif
}

void bt_pka_log_msgid_d(pka_log_type_t type, const char *message, uint32_t arg_cnt, ...)
{
#if !defined (MTK_DEBUG_LEVEL_NONE)
    va_list ap;

    va_start(ap, arg_cnt);
    log_print_msgid(pka_log_tab[type], PRINT_LEVEL_DEBUG, message, arg_cnt, ap);
    va_end(ap);
#endif
}

void bt_pka_log_msgid_w(pka_log_type_t type, const char *message, uint32_t arg_cnt, ...)
{
#if !defined (MTK_DEBUG_LEVEL_NONE)
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(pka_log_tab[type], PRINT_LEVEL_WARNING, message, arg_cnt, ap);
    va_end(ap);
#endif
}

void bt_pka_log_msgid_e(pka_log_type_t type, const char *message, uint32_t arg_cnt, ...)
{
#if !defined (MTK_DEBUG_LEVEL_NONE)
    va_list ap;

    va_start(ap, arg_cnt);
    log_print_msgid(pka_log_tab[type], PRINT_LEVEL_ERROR, message, arg_cnt, ap);
    va_end(ap);
#endif
}

void bt_pka_dump_msgid_i(pka_log_type_t type , const void *data, uint32_t length, const char *message, ...)
{

#if !defined (MTK_DEBUG_LEVEL_NONE)
    va_list ap;
    va_start(ap, message);
    vdump_module_buffer(pka_log_tab[type], "a", 0, PRINT_LEVEL_INFO, data, length, message, ap);
    va_end(ap);
#endif
}
