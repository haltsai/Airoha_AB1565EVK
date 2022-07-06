/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __EXCEPTION_HANDLER__
#define __EXCEPTION_HANDLER__

#if (PRODUCT_VERSION == 1552) || (PRODUCT_VERSION == 2552) || (PRODUCT_VERSION == 3335) || (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565)
/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "stdio.h"
#include "stdarg.h"
#include <stdint.h>
#include <string.h>
#include "syslog.h"
#if !defined(__UBL__)
#include "exception_config.h"
#endif

/* Public define -------------------------------------------------------------*/
#define DISABLE_MEMDUMP_MAGIC                           0xdeadbeef
#define DISABLE_WHILELOOP_MAGIC                         0xdeadaaaa

#define EXCEPTION_WDT_INTERRUPT                         0xdeadbbbb
#define EXCEPTION_WDT_RESET                             0xdeadcccc

#define EXCEPTION_MEMDUMP_NODUMP                        0x01
#define EXCEPTION_MEMDUMP_TEXT                          0x02
#define EXCEPTION_MEMDUMP_BINARY                        0x04
#define EXCEPTION_MEMDUMP_MINIDUMP                      0x08
#define MASK_IRQ_TOO_LONG_ASSERT                        0x10
#define EXCEPTION_DUMP_DISABLE_WHILELOOP                0x20
#define EXCEPTION_DUMMY_BIT 							0x80

/* FULLDUMP(N),MINIDUMP(N) */
#if !defined(MTK_FULLDUMP_ENABLE) && !defined(MTK_MINIDUMP_ENABLE)
#define EXCEPTION_MEMDUMP_MODE                          (EXCEPTION_MEMDUMP_NODUMP)
/* FULLDUMP(Y),MINIDUMP(N) */
#elif defined(MTK_FULLDUMP_ENABLE) && !defined(MTK_MINIDUMP_ENABLE)
#define EXCEPTION_MEMDUMP_MODE                          (EXCEPTION_MEMDUMP_DEFAULT)
/* FULLDUMP(N),MINIDUMP(Y) */
#elif !defined(MTK_FULLDUMP_ENABLE) && defined(MTK_MINIDUMP_ENABLE)
#define EXCEPTION_MEMDUMP_MODE                          (EXCEPTION_MEMDUMP_MINIDUMP)

#else
#define EXCEPTION_MEMDUMP_MODE                          (EXCEPTION_MEMDUMP_DEFAULT | EXCEPTION_MEMDUMP_MINIDUMP)
#endif /* !define(MTK_FULLDUMP_ENABLE) && !define(MTK_MINIDUMP_ENABLE) */

#if (EXCEPTION_MEMDUMP_MODE == EXCEPTION_MEMDUMP_MINIDUMP)
extern void printf_dummy(const char *message, ...);
extern void msgid_dummy(uint8_t cpu_id, const char *message, uint32_t arg_cnt, ...);
#define platform_printf                                 printf_dummy
#define exception_printf                                printf_dummy
#define exception_msgid                                 msgid_dummy

#elif (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT)
#define platform_printf                                 printf
#define exception_printf                                EXCEPTION_PRINT_DEFAULT
#define exception_msgid                                 EXCEPTION_MSGID_DEFAULT

#elif (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
#define platform_printf                                 printf
#define exception_printf                                EXCEPTION_PRINT_DEFAULT
#define exception_msgid                                 EXCEPTION_MSGID_DEFAULT

#else
#define platform_printf                                 printf
#define exception_printf                                printf
#define exception_msgid                                 EXCEPTION_MSGID_DEFAULT
#endif /* EXCEPTION_MEMDUMP_MODE */


/* Public typedef ------------------------------------------------------------*/
typedef enum
{
    EXCEPTION_STATUS_ERROR = 0,
    EXCEPTION_STATUS_OK = 1
} exception_status_t;

typedef enum
{
    EXCEPTION_TEXT = 0,
    EXCEPTION_BINARY = 1
} exception_log_type_t;


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
    unsigned int  *end_address;
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
		uint32_t exception_nodump	: 1;
		uint32_t exception_fulldump_text : 1;
		uint32_t exception_fulldump_binary : 1;
		uint32_t exception_minidump : 1;
		uint32_t mask_irq_check_assert : 1;
		uint32_t reset_after_dump : 1;
		uint32_t wdt_reset_mode :1 ;
		uint32_t exception_dummy_dump_mode : 1;
		uint32_t magic_number : 24;
	}exception_mode_t;
}exception_config_mode_t;

typedef struct{
	uint8_t race_header[10];
	uint32_t race_text_index;
	uint32_t race_binary_index;
}exception_minidump_with_race_t;


#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
typedef struct
{
    uint32_t address;
    uint32_t size;
} exception_minidump_region_t;

typedef struct
{
    uint32_t is_match;
    uint32_t overflow_address;
} exception_minidump_overflow_t;

typedef struct
{
    uint32_t is_valid;
    const char *expr;
    const char *file;
    uint32_t line;
} exception_minidump_assert_t;

typedef struct
{
    uint32_t core_num;
    uint32_t core_size[1 + EXCEPTION_SLAVES_TOTAL];
    uint32_t length;
    char name[20];
    uint32_t reason;
    exception_minidump_assert_t assert_info;
    exception_minidump_overflow_t overflow_info;
    uint32_t context_size;
    exception_minidump_region_t regions[20];
    uint32_t data_checksum;
} exception_minidump_header_t;
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_SLAVES_TOTAL > 0)
typedef enum
{
    EXCEPTION_SLAVE_STATUS_ERROR = -1,
    EXCEPTION_SLAVE_STATUS_IDLE = 0,
    EXCEPTION_SLAVE_STATUS_READY = 1,
    EXCEPTION_SLAVE_STATUS_FINISH = 2
} exception_slave_status_t;

typedef void (*f_exception_slave_alert_callback_t)(void);
typedef exception_slave_status_t (*f_exception_slave_checkstatus_callback_t)(void);
typedef void (*f_exception_slave_dump_callback_t)(void);
typedef void (*f_exception_slave_forceddump_callback_t)(void);
typedef struct
{
    const char* slave_name;
    f_exception_slave_alert_callback_t slave_alert;
    f_exception_slave_checkstatus_callback_t slave_checkstatus;
    f_exception_slave_dump_callback_t slave_dump;
    f_exception_slave_forceddump_callback_t slave_forceddump;
    unsigned int is_dump;
} exception_slaves_dump_t;
#endif /* EXCEPTION_SLAVES_TOTAL > 0 */


/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
extern uint32_t minidump_base_address;
extern exception_minidump_header_t minidump_header;
#endif /* EXCEPTION_MEMDUMP_MODE */


/* Public functions ----------------------------------------------------------*/
void exception_feed_wdt(void);
void exception_enable_wdt_reset(void);
void exception_enable_wdt_interrupt(void);
void exception_get_assert_expr(const char **expr, const char **file, int *line);
void platform_assert(const char *expr, const char *file, int line);

#if (PRODUCT_VERSION == 1552) || (PRODUCT_VERSION == 2552)
#define light_assert platform_assert
#else
void light_assert(const char *expr, const char *file, int line);
void __ram_assert(const char *expr, const char *file, int line);
#define ram_assert(__e) ((__e)?(void)0:__ram_assert(NULL,__FILE__,__LINE__))
#endif

void exception_dump_config(int flag);
void exception_reboot(void);
exception_status_t exception_register_callbacks(exception_config_type *cb);
exception_status_t exception_register_regions(memory_region_type *region);
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
exception_status_t exception_minidump_region_query_info(uint32_t index, uint32_t *address, uint32_t *length);
exception_status_t exception_minidump_region_query_count(uint32_t *count);
exception_status_t exception_minidump_region_query_latest_index(uint32_t *index);
exception_status_t exception_minidump_get_header_info(uint32_t address, uint8_t **header_address, uint32_t *size);
exception_status_t exception_minidump_get_assert_info(uint32_t address, char **file, uint32_t *line);
exception_status_t exception_minidump_get_context_info(uint32_t address, uint8_t **context_address, uint32_t *size);
exception_status_t exception_minidump_get_stack_info(uint32_t address, uint8_t **stack_address, uint32_t *size);
exception_status_t exception_minidump_check_not_duplicated(void);
#endif /* EXCEPTION_MEMDUMP_MODE */
int exception_dump_config_init(void);
#else
#define light_assert platform_assert
void platform_assert(const char *expr, const char *file, int line);

#include <stdbool.h>

#if (PRODUCT_VERSION == 2523 || PRODUCT_VERSION == 2533)
#include "mt2523.h"
#include "hal_flash_mtd.h"
#define configUSE_FLASH_SUSPEND 1
#endif

#if (PRODUCT_VERSION == 2625)
#include "mt2625.h"
#include "hal_flash_mtd.h"
#define configUSE_FLASH_SUSPEND 1
#endif

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)
#include "mt7687.h"
#include "flash_sfc.h"
#define configUSE_FLASH_SUSPEND 1
#endif

#if (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7682)
#include "mt7686.h"
#include "flash_sfc.h"
#define configUSE_FLASH_SUSPEND 1
#endif

#if (PRODUCT_VERSION == 5932)
#include "mt7686.h"
#define configUSE_FLASH_SUSPEND 0
#endif

#if (PRODUCT_VERSION == 7698)
#include "aw7698.h"
#include "flash_sfc.h"
#define configUSE_FLASH_SUSPEND 1
#endif

#if defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE)

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)
#include "flash_map.h"
#include "xflash_map.h"
#define CRASH_CONTEXT_FLASH_BASE     CRASH_CONTEXT_BASE
#define CRASH_CONTEXT_RESERVED_SIZE  CRASH_CONTEXT_LENGTH

#define CRASH_CONTEXT_EXT_FLASH_BASE     CRASH_CONTEXT_EXT_BASE
#define CRASH_CONTEXT_EXT_RESERVED_SIZE  CRASH_CONTEXT_EXT_LENGTH

#endif

#if (PRODUCT_VERSION == 2523)
#include "memory_map.h"
#define CRASH_CONTEXT_FLASH_BASE     (CRASH_CONTEXT_BASE - BL_BASE)
#define CRASH_CONTEXT_RESERVED_SIZE  CRASH_CONTEXT_LENGTH
#endif

#if (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7698)
extern uint32_t minidump_base_address;

bool exception_minidump_check_address(uint32_t address);
bool exception_minidump_region_query_info(uint32_t index, uint32_t *address, uint32_t *length);
bool exception_minidump_region_query_count(uint32_t *count);
#endif

void exception_get_assert_expr(const char **expr, const char **file, int *line);

#endif /* MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE */

#if defined(USE_KIPRINTF_AS_PRINTF)
extern int KiPrintf (const char *format, ...);
#define platform_printf KiPrintf
#elif defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE) && (PRODUCT_VERSION == 7686)
extern void printf_dummy(const char *message, ...);
#define platform_printf printf_dummy
#elif defined(MTK_SAVE_LOG_AND_CONTEXT_DUMP_ENABLE) && (PRODUCT_VERSION == 7698)
extern void printf_dummy(const char *message, ...);
#define platform_printf printf_dummy
#else
#define platform_printf printf
#endif

#define DISABLE_MEMDUMP_MAGIC 0xdeadbeef
#define DISABLE_WHILELOOP_MAGIC 0xdeadaaaa

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

bool exception_register_callbacks(exception_config_type *cb);
void exception_dump_config(int flag);
void exception_reboot_config(bool auto_reboot);
void exception_reboot(void);
void exception_get_assert_expr(const char **expr, const char **file, int *line);

#endif

#endif // #ifndef __EXCEPTION_HANDLER__
