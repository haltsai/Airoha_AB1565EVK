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

#ifndef __EXCEPTION_HANDLER__
#define __EXCEPTION_HANDLER__


/* Includes -----------------------------------------------------------------*/
#include "stdio.h"
#include "hal_resource_assignment.h"
#include "exception_config.h"


/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef enum
{
    EXCEPTION_STATUS_ERROR = 0,
    EXCEPTION_STATUS_OK = 1
} exception_status_t;

typedef struct
{
    uint32_t is_valid;
    const char *expr;
    const char *file;
    uint32_t line;
    const char *string;
} assert_expr_t;

typedef struct
{
    char *region_name;
    unsigned int *start_address;
    unsigned int *end_address;
    unsigned int is_dumped;
} memory_region_type;

typedef void (*f_exception_callback_t)(void);

typedef struct
{
    f_exception_callback_t init_cb;
    f_exception_callback_t dump_cb;
} exception_config_type;

/* exception dump configuration area */
typedef union{
    uint32_t exception_mode;
    struct {
        uint32_t exception_nodump          : 1;
        uint32_t exception_fulldump_text   : 1;
        uint32_t exception_fulldump_binary : 1;
        uint32_t exception_minidump        : 1;
        uint32_t mask_irq_check_assert     : 1;
        uint32_t reset_after_dump          : 1;
        uint32_t wdt_reset_mode            : 1;
        uint32_t exception_dummy_dump_mode : 1;
        uint32_t magic_number              : 24;
    }exception_mode_t;
}exception_config_mode_t;

/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
exception_status_t exception_register_callbacks(exception_config_type *cb);
exception_status_t exception_register_regions(memory_region_type *region);
void exception_get_assert_expr(const char **expr, const char **file, int *line);
void platform_assert(const char *expr, const char *file, int line);
void light_assert(const char *expr, const char *file, int line);


#endif /*__EXCEPTION_HANDLER__*/
