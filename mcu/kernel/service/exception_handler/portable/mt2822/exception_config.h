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

#ifndef __EXCEPTION_CONFIG__
#define __EXCEPTION_CONFIG__

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "syslog.h"
#include "syslog_internal.h"


/* Public define -------------------------------------------------------------*/
#define EXCEPTION_MEMDUMP_DEFAULT                       (EXCEPTION_MEMDUMP_BINARY)

#define configUSE_FLASH_SUSPEND                         1

#define EXCEPTION_SLAVES_TOTAL                          1
#define EXCEPTION_SLAVES_TIMEOUT                        10000000

#define MINIDUMP_FIXED_STRING_SIZE                     (20 * 1024)

#define MINIDUMP_ADDRESS_OFFSET_CM4                     (0)
#define MINIDUMP_HEADER_LENGTH                          (512)
#define MINIDUMP_CONTEXT_LENGTH                         (512)
#define MINIDUMP_MSPSTACK_LENGTH                        (2 * 1024)
#define MINIDUMP_PSPSTACK_LENGTH                        (3 * 1024)
#define MINIDUMP_DATA_SIZE_CM4                          (6144)

#define EXCEPTION_MINIDUMP_LATEST_INDEX_OFFSET          0x4

#define EXCEPTION_CPU_ID_MASTER                         0
#define EXCEPTION_CPU_ID_SLAVE                          1

extern int log_print_exception_log(const char *message, ...);
extern void log_dump_exception_data(const uint8_t *data, uint32_t size);
#define EXCEPTION_PRINT_DEFAULT                         log_print_exception_log
#define EXCEPTION_MSGID_DEFAULT                         LOG_EXCEPTION_MSGID


/* Public typedef ------------------------------------------------------------*/
/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/


#endif /* #ifndef __EXCEPTION_CONFIG__ */
