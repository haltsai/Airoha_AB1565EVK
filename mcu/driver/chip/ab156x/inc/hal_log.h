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
 
#ifndef __HAL_LOG_H__
#define __HAL_LOG_H__

/**
 * @filel_log.h.
 *
 * The debug printing macros that were used by HAL module is defined in this
 * header file.
 *
 * Feature option: MTK_HAL_NO_LOG_ENABLE
 *
 * This feature option turns the LOG system integration off in HAL module.
 * It was added to allow bootloader, which has no operating system in it, to
 * be able to use HAL layer code.
 */


#include "hal_feature_config.h"

#if !defined(MTK_HAL_PLAIN_LOG_ENABLE) && !defined(MTK_HAL_NO_LOG_ENABLE)
#include "syslog.h"
#else
#include "stdio.h"
#endif /* !defined(MTK_HAL_PLAIN_LOG_ENABLE) */

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(MTK_HAL_PLAIN_LOG_ENABLE) && !defined(MTK_HAL_NO_LOG_ENABLE)

#define log_hal_error(_message,...)                LOG_E(hal,_message,##__VA_ARGS__)
#define log_hal_warning(_message,...)              LOG_W(hal,_message,##__VA_ARGS__)
#define log_hal_info(_message,...)                 LOG_I(hal,_message,##__VA_ARGS__)
#define log_hal_dump(_message, _data, _len, ...)

#define log_hal_msgid_error(_message,arg_cnt,...)                LOG_MSGID_E(hal,_message,arg_cnt,##__VA_ARGS__)
#define log_hal_msgid_warning(_message,arg_cnt,...)              LOG_MSGID_W(hal,_message,arg_cnt,##__VA_ARGS__)
#define log_hal_msgid_info(_message,arg_cnt,...)                 LOG_MSGID_I(hal,_message,arg_cnt,##__VA_ARGS__)
#define log_hal_msgid_dump(_message,arg_cnt, _data, _len, ...)
#else
#define log_hal_error(_message,...)
#define log_hal_warning(_message,...)
#define log_hal_info(_message,...)
#define log_hal_dump(_message, _data, _len, ...)

#define log_hal_msgid_error(_message,arg_cnt,...)
#define log_hal_msgid_warning(_message,arg_cnt,...)
#define log_hal_msgid_info(_message,arg_cnt,...)
#define log_hal_msgid_dump(_message,arg_cnt, _data, _len, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif

