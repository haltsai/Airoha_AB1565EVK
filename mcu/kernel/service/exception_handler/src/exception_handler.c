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

/* Includes ------------------------------------------------------------------*/
#include "memory_attribute.h"
#include "exception_config.h"
#include "exception_handler.h"
#include "exception_portable.h"
#include "hal_dwt.h"
#include "hal_uart.h"
#include "hal_gpt.h"
#include "hal_pmu.h"

#ifdef MTK_NVDM_ENABLE
#include "nvkey.h"
#include "nvkey_id_list.h"
#endif

#ifdef HAL_WDT_MODULE_ENABLED
#include "hal_wdt.h"
#endif

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
#include "systemhang_tracer.h"
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */

#ifdef MTK_MEMORY_MONITOR_ENABLE
#include "memory_monitor.h"
#endif /* MTK_MEMORY_MONITOR_ENABLE */

#ifdef MTK_BOOTREASON_CHECK_ENABLE
#include "bootreason_check.h"
#endif /* MTK_BOOTREASON_CHECK_ENABLE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
#include "offline_dump.h"
#include "memory_map.h"
#endif /* EXCEPTION_MEMDUMP_MODE */

#ifdef MTK_SWLA_ENABLE
#include "swla.h"
#endif /* MTK_SWLA_ENABLE */

#ifdef HAL_ESC_MODULE_ENABLED
#include "hal_esc_internal.h"
#endif


/* Private define ------------------------------------------------------------*/
#if  defined ( __GNUC__ )
#ifndef __weak
#define __weak   __attribute__((weak))
#endif /* __weak */
#endif /* __GNUC__ */

#define EXCEPTION_CONFIGURATIONS_MAX                    6

#define EXCEPTION_STACK_WORDS                           768


/* Private typedef -----------------------------------------------------------*/
typedef enum {
    EXCEPTION_RESET     = 1,
    EXCEPTION_NMI       = 2,
    EXCEPTION_HARD_FAULT = 3,
    EXCEPTION_MEMMANAGE_FAULT = 4,
    EXCEPTION_BUS_FAULT = 5,
    EXCEPTION_USAGE_FAULT = 6,
    EXCEPTION_DEBUGMON_FAULT = 12,
} exception_type_t;

typedef struct {
    int items;
    exception_config_type configs[EXCEPTION_CONFIGURATIONS_MAX];
} exception_config_t;

typedef struct {
    uint32_t items;
    memory_region_type regions[EXCEPTION_CONFIGURATIONS_MAX];
} exception_user_regions_t;

typedef struct {
    uint32_t count;
    uint32_t timestamp;
    uint32_t timestamp_32k;
    uint32_t reason;
    assert_expr_t *assert_expr;
} exception_info_t;

enum { r0, r1, r2, r3, r12, lr, pc, psr,
       s0, s1, s2, s3, s4, s5, s6, s7,
       s8, s9, s10, s11, s12, s13, s14, s15,
       fpscr
     };

typedef struct {
    unsigned int r0;
    unsigned int r1;
    unsigned int r2;
    unsigned int r3;
    unsigned int r4;
    unsigned int r5;
    unsigned int r6;
    unsigned int r7;
    unsigned int r8;
    unsigned int r9;
    unsigned int r10;
    unsigned int r11;
    unsigned int r12;
    unsigned int sp;              /* after pop r0-r3, lr, pc, xpsr                   */
    unsigned int lr;              /* lr before exception                             */
    unsigned int pc;              /* pc before exception                             */
    unsigned int psr;             /* xpsr before exeption                            */
    unsigned int control;         /* nPRIV bit & FPCA bit meaningful, SPSEL bit = 0  */
    unsigned int exc_return;      /* current lr                                      */
    unsigned int msp;             /* msp                                             */
    unsigned int psp;             /* psp                                             */
    unsigned int basepri;         /* basepri                                         */
    unsigned int primask;         /* primask                                         */
    unsigned int faultmask;       /* faultmask                                       */
    unsigned int fpscr;
    unsigned int s0;
    unsigned int s1;
    unsigned int s2;
    unsigned int s3;
    unsigned int s4;
    unsigned int s5;
    unsigned int s6;
    unsigned int s7;
    unsigned int s8;
    unsigned int s9;
    unsigned int s10;
    unsigned int s11;
    unsigned int s12;
    unsigned int s13;
    unsigned int s14;
    unsigned int s15;
    unsigned int s16;
    unsigned int s17;
    unsigned int s18;
    unsigned int s19;
    unsigned int s20;
    unsigned int s21;
    unsigned int s22;
    unsigned int s23;
    unsigned int s24;
    unsigned int s25;
    unsigned int s26;
    unsigned int s27;
    unsigned int s28;
    unsigned int s29;
    unsigned int s30;
    unsigned int s31;
} exception_context_t;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* exception handler's stack */
static unsigned int exception_stack[EXCEPTION_STACK_WORDS] = {0};
__attribute__((__section__(".exception_rodata"))) unsigned int *exception_stack_pointer = &exception_stack[EXCEPTION_STACK_WORDS - 1];

/* exception user configuration area */
static exception_config_t exception_config = {0};

/* avoid other global value corrupt exception_user_regions */
ATTR_ZIDATA_IN_TCM static exception_user_regions_t exception_user_regions;

/* exception reboot configuration area */
static int reboot_flag = 0;

/* assert information area */
static assert_expr_t assert_expr = {0};
ATTR_LOG_STRING assert_string[] = "CM4 assert : %s, file: %s, line: %d\r\n";

/* exception information area */
static exception_info_t exception_info = {0};
exception_info_t *exception_info_pointer = &exception_info;

/* exception context area */
exception_context_t exception_context = {0};
__attribute__((__section__(".exception_rodata"))) exception_context_t *exception_context_pointer = &exception_context;

/* exception dump memory regions in region_init.c */
extern memory_region_type memory_regions[];

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
uint32_t minidump_base_address = 0;
uint32_t minidump_ori_address = 0;
int32_t minidump_total_size = 0;
int32_t minidump_region_total_size = 0;
exception_minidump_header_t minidump_header = {0};
exception_minidump_with_race_t exception_minidump_with_race;
extern int32_t  offline_dump_region_write_with_race_header(exception_log_type_t log_type, uint32_t *curr_addr,  uint8_t *data, uint32_t size);

#endif /* EXCEPTION_MEMDUMP_MODE */

static uint32_t primask_backup_assert = 0;

static uint32_t exception_wdt_mode = 0;

#ifdef MTK_BOOTREASON_CHECK_ENABLE
    static uint32_t ram_assert_flag = FALSE;
#endif /* MTK_BOOTREASON_CHECK_ENABLE */

static uint32_t double_assert_flag = FALSE;
static uint32_t double_assert_lr = FALSE;

/* except ab156x and 2822,exception_config_mode minidump bit,fulldump bit alawys true,but macro determines whether code is build*/
#if !defined(AB156X) && (PRODUCT_VERSION != 2822)
/* repeat 4 Byets for checking,wdt reset mode disable*/
ATTR_RWDATA_IN_TCM volatile exception_config_mode_t exception_config_mode = {0xbfbfbfbf};
#else
/* ab156x and mt2822 minidump macro default defined,it means minidumop code is alawys been build,and if exception_config_mode
 * minidump bit is true,it will run minidump,otherwise it is just been build but not run */
/* repeat 4 Byets for checking */
ATTR_RWDATA_IN_TCM volatile exception_config_mode_t exception_config_mode = {0};
#endif

#if (EXCEPTION_SLAVES_TOTAL > 0)
extern hal_hw_semaphore_status_t hw_semaphore_take(hal_hw_semaphore_id_t id);
extern hal_hw_semaphore_status_t hw_semaphore_give(hal_hw_semaphore_id_t id);
#endif

extern bool exception_triggered_flag;

/* Private functions ---------------------------------------------------------*/
/******************************************************************************/
/*            Exception's wdt timeout Functions                               */
/******************************************************************************/
void exception_feed_wdt(void)
{
#ifdef HAL_WDT_MODULE_ENABLED
    hal_wdt_feed(HAL_WDT_FEED_MAGIC);
#endif
}

void exception_enable_wdt_reset(void)
{
#ifdef HAL_WDT_MODULE_ENABLED
    hal_wdt_config_t wdt_config;
    wdt_config.mode = HAL_WDT_MODE_RESET;
    wdt_config.seconds = 30;
    hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
    hal_wdt_init(&wdt_config);
    hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
#endif
    exception_wdt_mode = EXCEPTION_WDT_RESET;
}

void exception_disable_wdt_reset(void)
{
#ifdef HAL_WDT_MODULE_ENABLED
    if (exception_wdt_mode == EXCEPTION_WDT_RESET) {
        hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
    }
#endif
}

void exception_enable_wdt_interrupt(void)
{
    if ((exception_wdt_mode != EXCEPTION_WDT_INTERRUPT) && (exception_info.reason != EXCEPTION_NMI)) {
#ifdef HAL_WDT_MODULE_ENABLED
        hal_wdt_config_t wdt_config;
        wdt_config.mode = HAL_WDT_MODE_INTERRUPT;
        wdt_config.seconds = 30;
        hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
        hal_wdt_init(&wdt_config);
        hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
#endif
        exception_wdt_mode = EXCEPTION_WDT_INTERRUPT;
    }
}

/******************************************************************************/
/*            Exception's assert Functions                                    */
/******************************************************************************/
void abort(void)
{
    static uint32_t primask_backup_abort = 0;

    primask_backup_abort = __get_PRIMASK();
    __disable_irq();

    SCB->CCR |=  SCB_CCR_UNALIGN_TRP_Msk;
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;
    for (;;);

    /* Just to avoid compiler warnings. */
    (void) primask_backup_abort;
}

extern  hal_core_status_t hal_core_status_read(hal_core_id_t id);
void platform_assert(const char *expr, const char *file, int line)
{
    if (HAL_CORE_EXCEPTION == hal_core_status_read(HAL_CORE_CM4)){
        exception_printf("[Warning]:cannot assert in exception flow!!!\r\n");
        return;
    }

    primask_backup_assert = __get_PRIMASK();
    __disable_irq();

#ifdef MTK_BOOTREASON_CHECK_ENABLE
    bootreason_set_flag_assert_reset();
#endif /* MTK_BOOTREASON_CHECK_ENABLE */

    SCB->CCR |=  SCB_CCR_UNALIGN_TRP_Msk;
    assert_expr.is_valid = 1;
    assert_expr.expr = expr;
    assert_expr.file = file;
    assert_expr.line = line;
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;

    while(1);

    /* Just to avoid compiler warnings. */
    (void) primask_backup_assert;
}

#if (PRODUCT_VERSION != 1552) && (PRODUCT_VERSION != 2552)
void light_assert(const char *expr, const char *file, int line)
{
    static uint32_t primask_backup_assert = 0;

    if (HAL_CORE_EXCEPTION == hal_core_status_read(HAL_CORE_CM4)){
        exception_printf("[Warning]:cannot assert in exception flow!!!\r\n");
        return;
    }

    primask_backup_assert = __get_PRIMASK();
    __disable_irq();

#ifdef MTK_BOOTREASON_CHECK_ENABLE
    bootreason_set_flag_assert_reset();
#endif /* MTK_BOOTREASON_CHECK_ENABLE */

    SCB->CCR |=  SCB_CCR_UNALIGN_TRP_Msk;
    assert_expr.is_valid = 2;
    assert_expr.expr = expr;
    assert_expr.file = file;
    assert_expr.line = line;
    assert_expr.string = assert_string;
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;
     while(1);


    /* Just to avoid compiler warnings. */
    (void) primask_backup_assert;
}
#endif /* (PRODUCT_VERSION != 1552) && (PRODUCT_VERSION != 2552) */

__attribute__((__section__(".exception_code"))) void __ram_assert(const char *expr, const char *file, int line)
{
	char exp[] = "RAM_Assert";

    if (HAL_CORE_EXCEPTION == hal_core_status_read(HAL_CORE_CM4)){
        double_assert_flag = TRUE;
        double_assert_lr = (uint32_t)__builtin_return_address(0);
        return;
    }

    primask_backup_assert = __get_PRIMASK();
    __disable_irq();

#ifdef MTK_BOOTREASON_CHECK_ENABLE
    ram_assert_flag = TRUE;
#endif /* MTK_BOOTREASON_CHECK_ENABLE */

    SCB->CCR |=  SCB_CCR_UNALIGN_TRP_Msk;
    assert_expr.is_valid = 1;
    assert_expr.expr = exp;
    assert_expr.file = file;
    assert_expr.line = line;
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;

    while(1);

    /* Just to avoid compiler warnings. */
    (void) primask_backup_assert;

}

#if defined (__CC_ARM) || defined (__ICCARM__)
void __aeabi_assert(const char *expr, const char *file, int line)
{
    platform_assert(expr, file, line);
}
#endif /* __CC_ARM */

void exception_get_assert_expr(const char **expr, const char **file, int *line)
{
    if (assert_expr.is_valid) {
        *expr = assert_expr.expr;
        *file = assert_expr.file;
        *line = assert_expr.line;
    } else {
        *expr = NULL;
        *file = NULL;
        *line = 0;
    }

}

void exception_print_assert_info(void)
{
    if (assert_expr.is_valid == 1) {
        exception_printf("CM4 assert failed: %s, file: %s, line: %d\r\n",
                         assert_expr.expr,
                         assert_expr.file,
                         (int)assert_expr.line);
    } else if (assert_expr.is_valid == 2) {
        exception_msgid(EXCEPTION_CPU_ID_MASTER,
                        assert_expr.string,
                        3,
                        assert_expr.expr,
                        assert_expr.file,
                        (int)assert_expr.line);
    }
}

/******************************************************************************/
/*            Exception's Reboot Functions                                    */
/******************************************************************************/
__weak void exception_reboot(void)
{
    /* It is defined as a weak function.
     * It needs to be implemented in project.
     */
#ifdef HAL_WDT_MODULE_ENABLED

#if 0
    /* Wait there for 3s to ensure uart output done */
    hal_wdt_config_t wdt_config;
    wdt_config.mode = HAL_WDT_MODE_RESET;
    wdt_config.seconds = 3;
    hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
    hal_wdt_init(&wdt_config);
    hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
#endif
    /* In exception flow,uart mode is polling,so it's no need to wait for 3s */
    hal_wdt_software_reset();
    while (1);
#endif
    return;
}

void exception_dump_config(int flag)
{
    reboot_flag = flag;
}

static int reboot_check(void)
{
    return reboot_flag;
}

/******************************************************************************/
/*            Exception's Dump Mode Functions                        */
/******************************************************************************/
int exception_dump_config_init(void)
{

#if (defined(AB156X) || (PRODUCT_VERSION == 2822)) && defined(MTK_NVDM_ENABLE)
    nvkey_status_t nvkey_status = NVKEY_STATUS_OK;

    uint32_t size = 4;
    log_hal_msgid_info("[Dump config]:0x%x\r\n", 1, (unsigned int)exception_config_mode.exception_mode);

    /* get user's exception dump configuartion */
    nvkey_status = nvkey_read_data(NVKEYID_EXCEPTION_DUMP_MODE, (uint8_t *)&exception_config_mode.exception_mode, &size);

    log_hal_msgid_info("exception_nodump:%d\r\n", 1, exception_config_mode.exception_mode_t.exception_nodump);
    log_hal_msgid_info("exception_fulldump_text:%d\r\n", 1, exception_config_mode.exception_mode_t.exception_fulldump_text);
    log_hal_msgid_info("exception_fulldump_binary:%d\r\n", 1, exception_config_mode.exception_mode_t.exception_fulldump_binary);
    log_hal_msgid_info("exception_minidump:%d\r\n", 1, exception_config_mode.exception_mode_t.exception_minidump);
    log_hal_msgid_info("reset_after_dump:%d\r\n", 1, exception_config_mode.exception_mode_t.reset_after_dump);
    log_hal_msgid_info("wdt_reset_mode:%d\r\n", 1, exception_config_mode.exception_mode_t.wdt_reset_mode);
    log_hal_msgid_info("mask_irq_check_assert:%d\r\n", 1, exception_config_mode.exception_mode_t.mask_irq_check_assert);

    log_hal_msgid_info("[Dump config]:0x%x\r\n", 1, (unsigned int)exception_config_mode.exception_mode);

    return nvkey_status;
#else
    return 0;
#endif
}

void exception_dump_config_check(void)
{
	uint8_t mode = 0;

	/* check mode */
	if (exception_config_mode.exception_mode == 0) {
		/* error status, exception_dump_mode should not be 0 */
		exception_config_mode.exception_mode_t.reset_after_dump = TRUE;
		exception_config_mode.exception_mode_t.exception_fulldump_binary = TRUE;
	}

	mode = (uint8_t)(exception_config_mode.exception_mode & 0xff);
	if ((mode != (uint8_t)((exception_config_mode.exception_mode >>  8) & 0xff)) ||
			(mode != (uint8_t)((exception_config_mode.exception_mode >> 16) & 0xff)) ||
			(mode != (uint8_t)((exception_config_mode.exception_mode >> 24) & 0xff))) {
		/* error status, exception_dump_mode should be a 4 Byets-repeated data*/
		exception_config_mode.exception_mode_t.reset_after_dump = TRUE;
		exception_config_mode.exception_mode_t.exception_fulldump_binary = TRUE;
	}
}


/******************************************************************************/
/*            Exception's regitser callbacks Functions                        */
/******************************************************************************/
exception_status_t exception_register_callbacks(exception_config_type *cb)
{
    int i;

    if (exception_config.items >= EXCEPTION_CONFIGURATIONS_MAX) {
        return EXCEPTION_STATUS_ERROR;
    } else {
        /* check if it is already registered */
        for (i = 0; i < exception_config.items; i++) {
            if (exception_config.configs[i].init_cb == cb->init_cb
                    && exception_config.configs[i].dump_cb == cb->dump_cb) {
                return EXCEPTION_STATUS_ERROR;
            }
        }
        exception_config.configs[exception_config.items].init_cb = cb->init_cb;
        exception_config.configs[exception_config.items].dump_cb = cb->dump_cb;
        exception_config.items++;
        return EXCEPTION_STATUS_OK;
    }
}

exception_status_t exception_register_regions(memory_region_type *region)
{
    if (exception_user_regions.items >= EXCEPTION_CONFIGURATIONS_MAX) {
        return EXCEPTION_STATUS_ERROR;
    } else {
        exception_user_regions.regions[exception_user_regions.items].region_name = region->region_name;
        exception_user_regions.regions[exception_user_regions.items].start_address = region->start_address;
        exception_user_regions.regions[exception_user_regions.items].end_address = region->end_address;
        exception_user_regions.regions[exception_user_regions.items].is_dumped = region->is_dumped;
        exception_user_regions.items++;
        return EXCEPTION_STATUS_OK;
    }
}

/******************************************************************************/
/*            Exception's minidump Functions                              */
/******************************************************************************/
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
void printf_dummy(const char *message, ...)
{
}

void msgid_dummy(uint8_t cpu_id, const char *message, uint32_t arg_cnt, ...)
{
    /* Just to avoid compiler warnings. */
    (void) cpu_id;
    (void) message;
    (void) arg_cnt;
}

#if !((defined(AB156X) || (PRODUCT_VERSION == 2822)))

static exception_status_t exception_minidump_check_address(uint32_t address)
{
    uint32_t i = 0;
    uint32_t start, end;

    for (i = 0; ; i++) {

        if (!memory_regions[i].region_name) {
            break;
        }

        start = (uint32_t)(memory_regions[i].start_address);
        end   = (uint32_t)(memory_regions[i].end_address);

        if ((address >= start) && (address <= end)) {
            return EXCEPTION_STATUS_OK;
        }

    }

    return EXCEPTION_STATUS_ERROR;
}
#endif

static exception_status_t exception_minidump_update_latest_index(uint32_t index)
{
    char flag = (char)(index & 0xff);

    if (index > 0xff) {
        return EXCEPTION_STATUS_ERROR;
    }

    hal_rtc_set_data(EXCEPTION_MINIDUMP_LATEST_INDEX_OFFSET,
                     (const char *)&flag,
                     1);

    return EXCEPTION_STATUS_OK;
}

exception_status_t exception_minidump_region_query_info(uint32_t index, uint32_t *address, uint32_t *length)
{
    uint32_t start_num, end_num = 0;

    if ((address == NULL) || (length == NULL) || (index == 0)) {
        return EXCEPTION_STATUS_ERROR;
    }

    if (true == offline_dump_region_query_seq_range(OFFLINE_REGION_MINI_DUMP, &start_num, &end_num)) {
        if ((end_num >= start_num) || (index <= (end_num - start_num + 1))) {
            if (true == offline_dump_region_query_by_seq(OFFLINE_REGION_MINI_DUMP, index + start_num - 1, address, length)) {
                return EXCEPTION_STATUS_OK;
            }
        }
    }

    return EXCEPTION_STATUS_ERROR;
}

exception_status_t exception_minidump_region_query_count(uint32_t *count)
{
    uint32_t start_num, end_num = 0;

    if (count == NULL) {
        return EXCEPTION_STATUS_ERROR;
    }

    if (true == offline_dump_region_query_seq_range(OFFLINE_REGION_MINI_DUMP, &start_num, &end_num)) {
        if (end_num >= start_num) {
            *count = end_num - start_num + 1;
            return EXCEPTION_STATUS_OK;
        }
    }

    return EXCEPTION_STATUS_ERROR;
}

exception_status_t exception_minidump_region_query_latest_index(uint32_t *index)
{
    uint32_t dump_count;
    uint8_t latest_index;

    /* get minidump total count */
    if (EXCEPTION_STATUS_OK != exception_minidump_region_query_count(&dump_count)) {
        *index = 0;
        return EXCEPTION_STATUS_ERROR;
    }

    /* get latest_index from rtc register */
    hal_rtc_get_data(EXCEPTION_MINIDUMP_LATEST_INDEX_OFFSET,
                     (char *)&latest_index,
                     1);

    if (latest_index > dump_count) {
        /* latest index and total dump count are not matched */
        *index = 0;
        return EXCEPTION_STATUS_ERROR;
    }

    if (latest_index != 0) {
        *index = (uint32_t)latest_index;
    } else {
        *index = dump_count;
    }

    return EXCEPTION_STATUS_OK;
}

exception_status_t exception_minidump_get_header_info(uint32_t address, uint8_t **header_address, uint32_t *size)
{
    if (address == 0) {
        return EXCEPTION_STATUS_ERROR;
    }

    *header_address = (uint8_t *)(address + sizeof(offline_dump_header_t));
    *size = MINIDUMP_HEADER_LENGTH;

    return EXCEPTION_STATUS_OK;
}

exception_status_t exception_minidump_get_assert_info(uint32_t address, char **file, uint32_t *line)
{
    exception_minidump_header_t *header = NULL;

    if (address == 0) {
        return EXCEPTION_STATUS_ERROR;
    }

    header = (exception_minidump_header_t *)(address + sizeof(offline_dump_header_t));
    *file = (char *)((header->assert_info).file);
    *line = (header->assert_info).line;

    /* Check if the file is in Flash region */
    if (((uint32_t)(*file) > (ROM_BASE + ROM_LENGTH)) || ((uint32_t)(*file) < ROM_BASE)) {
        return EXCEPTION_STATUS_ERROR;
    }

    return EXCEPTION_STATUS_OK;
}

exception_status_t exception_minidump_get_context_info(uint32_t address, uint8_t **context_address, uint32_t *size)
{
    if (address == 0) {
        return EXCEPTION_STATUS_ERROR;
    }

    *context_address = (uint8_t *)(address + sizeof(offline_dump_header_t) + MINIDUMP_HEADER_LENGTH);
    *size = sizeof(exception_context_t);

    return EXCEPTION_STATUS_OK;
}

exception_status_t exception_minidump_get_stack_info(uint32_t address, uint8_t **stack_address, uint32_t *size)
{
    exception_context_t *context = NULL;

    if (address == 0) {
        return EXCEPTION_STATUS_ERROR;
    }

    context = (exception_context_t *)(address + sizeof(offline_dump_header_t) + MINIDUMP_HEADER_LENGTH);
    if (context->sp == context->msp) {
        *stack_address = (uint8_t *)(address
                                     + sizeof(offline_dump_header_t)
                                     + MINIDUMP_HEADER_LENGTH
                                     + MINIDUMP_CONTEXT_LENGTH
                                     + MINIDUMP_PSPSTACK_LENGTH);
        *size = MINIDUMP_MSPSTACK_LENGTH;
    } else if (context->sp == context->psp) {
        *stack_address = (uint8_t *)(address
                                     + sizeof(offline_dump_header_t)
                                     + MINIDUMP_HEADER_LENGTH
                                     + MINIDUMP_CONTEXT_LENGTH);
        *size = MINIDUMP_PSPSTACK_LENGTH;
    } else {
        return EXCEPTION_STATUS_ERROR;
    }

    return EXCEPTION_STATUS_OK;
}

exception_status_t exception_minidump_check_not_duplicated(void)
{
    uint32_t dump_count = 0;
    uint32_t dump_addr, dump_len;
    char *dump_assert_file;
    uint32_t dump_assert_line;
    exception_minidump_header_t *dump_header;
    uint32_t dump_header_size;
    exception_context_t *dump_context;
    uint32_t dump_context_size;

    /* get minidump total count */
    if (EXCEPTION_STATUS_OK != exception_minidump_region_query_count(&dump_count)) {
        /* can not get minidump info, return ok for do this minidump */
        return EXCEPTION_STATUS_OK;
    }

    /* check existing minidump one by one */
    for (; dump_count > 0; dump_count--) {
        /* get minidump info */
        if (EXCEPTION_STATUS_OK != exception_minidump_region_query_info(dump_count,
                &dump_addr,
                &dump_len)) {
            /* can not get minidump info, return ok for do this minidump */
            return EXCEPTION_STATUS_OK;
        }

        /* get minidump header info */
        if (EXCEPTION_STATUS_OK != exception_minidump_get_header_info(dump_addr,
                (uint8_t **)(&dump_header),
                &dump_header_size)) {
            /* can not get minidump info, return ok for do this minidump */
            return EXCEPTION_STATUS_OK;
        }

        /* get minidump assert info */
        if (EXCEPTION_STATUS_OK != exception_minidump_get_assert_info(dump_addr,
                &dump_assert_file,
                &dump_assert_line)) {
            /* if exception is not assert, exception_minidump_get_assert_info() always return error */
            /* do nothing */
        }

        /* get minidump context info */
        if (EXCEPTION_STATUS_OK != exception_minidump_get_context_info(dump_addr,
                (uint8_t **)(&dump_context),
                &dump_context_size)) {
            /* can not get minidump info, return ok for do this minidump */
            return EXCEPTION_STATUS_OK;
        }

        /* TODO: add slaves' minidump check */

        /* check assert info */
        if ((exception_info.assert_expr->is_valid == true) &&
                (dump_assert_file == exception_info.assert_expr->file) &&
                (dump_assert_line == exception_info.assert_expr->line)) {
            /* same means this assert has been saved */
            /* change the latest index to this saved minidump's index */
            exception_minidump_update_latest_index(dump_count);
            return EXCEPTION_STATUS_ERROR;
        }

        /* check other exceptions' pc, lr and exception type*/
        if ((exception_info.assert_expr->is_valid != true) &&
                (dump_context->pc == exception_context.pc) &&
                (dump_context->lr == exception_context.lr) &&
                (dump_header->reason == exception_info.reason)) {
            /* same means this exception has been saved */
            /* change the latest index to this saved minidump's index */
            exception_minidump_update_latest_index(dump_count);
            return EXCEPTION_STATUS_ERROR;
        }
    }

    return EXCEPTION_STATUS_OK;
}
#endif /* EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
exception_status_t exception_query_cm4_region_size(int32_t * size)
{
    uint32_t i = 0;
    unsigned int * current = NULL;
    unsigned int * end = NULL;
    /* static regions */
    for (i = 0; ; i++) {
        if (!memory_regions[i].region_name) {
            break;
        }
        if (!memory_regions[i].is_dumped) {
            continue;
        }
        current = (unsigned int *)((uint32_t)(memory_regions[i].start_address) & 0xfffffffc);
        end     = (unsigned int *)(memory_regions[i].end_address);

        if((int32_t)end > (int32_t)current){
            *size += (int32_t)end - (int32_t)current;
        }
    }

    /* dynmic regions */
    for (i = 0; ; i++) {
        if (!exception_user_regions.regions[i].region_name) {
            break;
        }
        if (!exception_user_regions.regions[i].is_dumped) {
            continue;
        }
        current = (unsigned int *)((uint32_t)(exception_user_regions.regions[i].start_address) & 0xfffffffc);
        end     = (unsigned int *)(exception_user_regions.regions[i].end_address);

        if((int32_t)end > (int32_t)current){
            *size += (int32_t)end - (int32_t)current;
        }

    }

    /* each packet data need header,so here we need add header size,we reserve 4k */
    *size += 4*1024;

    return EXCEPTION_STATUS_OK;
}
#endif
/******************************************************************************/
/*            Exception's init Functions                                      */
/******************************************************************************/
__attribute__((__section__(".exception_code"))) void exception_init(void)
{

    /*enable unalign access,it will not trap when align access occurs */
    SCB->CCR &= ~SCB_CCR_UNALIGN_TRP_Msk;

#if (configUSE_FLASH_SUSPEND == 1)
    Flash_ReturnReady();
#endif

#if defined(HAL_ESC_MODULE_ENABLED) && defined(ESC_FLASH_ENABLE)
    esc_flash_return_ready();
#endif



#ifdef MTK_BOOTREASON_CHECK_ENABLE
    /* Set assert flag in rtc */
    if (TRUE == ram_assert_flag){
        bootreason_set_flag_assert_reset();
    }

    bootreason_set_flag_exception_reset();
#endif /* MTK_BOOTREASON_CHECK_ENABLE */

    /* enable wdt reset mode for prevent exception flow hang */
    exception_enable_wdt_reset();

#if defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O1) || defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O2)
    systemhang_exception_enter_trace();
#endif /* defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O1) || defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O2) */

    /* update core status for other module usage */
    exception_core_status_update();

    /* just for exception during dongle plug out */
    hal_gpt_delay_ms(500);

    /* enable exception log service */
    exception_log_service_init();

#ifdef MTK_BOOTREASON_CHECK_ENABLE
    /* check whther uart has received xoff */
    extern bool query_syslog_xoff(void);

    if (TRUE == query_syslog_xoff()) {
        bootreason_set_flag_xoff_reset();
        while (1);
    }
#endif

    /* show exception mode */
    exception_printf("exception_mode:0x%x\r\n", exception_config_mode.exception_mode);
    /* show pc */
    exception_printf("cm4 pc:0x%x\r\n", exception_context.pc);
    /* show lr */
    exception_printf("cm4 lr:0x%x\r\n", exception_context.lr);

#if (defined(AB156X) || (PRODUCT_VERSION == 2822))
    /* check exception dump configuration if is ok */
    exception_dump_config_check();
#endif

#ifdef MTK_SWLA_ENABLE
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
    if ((exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) || (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_BINARY)) {

        /* add a record for exception to mark the end of the last thread */
        const uint8_t ucExceptionRec[5] = "excp";
        const uint32_t xExceptionRec = (uint32_t)(ucExceptionRec[0] | (ucExceptionRec[1] << 8) | (ucExceptionRec[2] << 16) | (ucExceptionRec[3] << 24));
        SLA_RamLogging(xExceptionRec);
    }

    extern void SLA_MemoryCallbackInit(void);
    SLA_MemoryCallbackInit();

#endif /* MTK_SWLA_ENABLE */

#endif /* EXCEPTION_MEMDUMP_MODE */

    /* Update Exception Number */
    exception_info.count += 1;

#if (EXCEPTION_SLAVES_TOTAL > 0)


    uint32_t i = 0;
    /* Trigger other Core's exception */
    for (i = 0; i < EXCEPTION_SLAVES_TOTAL; i++) {
        if ((exceptionSlavesDump[i].is_dump != 0) &&
                (exceptionSlavesDump[i].slave_alert != NULL)) {
            exceptionSlavesDump[i].slave_alert();
        }
    }

    /* Give exception hw semaphore.
     * case 1:exception count is 1,dsp will not care about it.
     * case 2:exception count is 2(NMI),dsp will run only if cm4 give hw sema.
     * case 3(rare):exception count is 2(cm4 double exception),actually dsp has finished dump,
     * cm4 occurs exception in exception flow,cm4 will enter in double exception and because
     * dsp clear ccni,so cm4 will trigger force dump and it will make dsp dump again.it will dump 3 times
     */
    hw_semaphore_give(EXCEPTION_HW_SEMAPHORE);
#endif /* EXCEPTION_SLAVES_TOTAL > 0 */

    /* Get current time stamp */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M , &(exception_info.timestamp));
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K , &(exception_info.timestamp_32k));

#if defined(AB156X) || (PRODUCT_VERSION == 2822)
    if (exception_config_mode.exception_mode_t.reset_after_dump == TRUE) {
        exception_dump_config(DISABLE_WHILELOOP_MAGIC);
    }
#else

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP) || (EXCEPTION_MEMDUMP_MODE == EXCEPTION_MEMDUMP_NODUMP)
    exception_dump_config(DISABLE_WHILELOOP_MAGIC);
#endif /* EXCEPTION_MEMDUMP_MODE */

#endif
}

/******************************************************************************/
/*            Exception's context dump Functions                              */
/******************************************************************************/
static void exception_dump_context(uint32_t stack[])
{
    /* Context Dump */
    // exception_context.r0   = stack[r0];
    // exception_context.r1   = stack[r1];
    // exception_context.r2   = stack[r2];
    // exception_context.r3   = stack[r3];
    // exception_context.r12  = stack[r12];
    exception_context.sp   = ((uint32_t)stack) + 0x20;
    // exception_context.lr   = stack[lr];
    // exception_context.pc   = stack[pc];
    // exception_context.psr  = stack[psr];

    /* FPU context? */
    if ((exception_context.exc_return & 0x10) == 0) {
        exception_context.control |= 0x4; /* CONTROL.FPCA */
        exception_context.s0 = stack[s0];
        exception_context.s1 = stack[s1];
        exception_context.s2 = stack[s2];
        exception_context.s3 = stack[s3];
        exception_context.s4 = stack[s4];
        exception_context.s5 = stack[s5];
        exception_context.s6 = stack[s6];
        exception_context.s7 = stack[s7];
        exception_context.s8 = stack[s8];
        exception_context.s9 = stack[s9];
        exception_context.s10 = stack[s10];
        exception_context.s11 = stack[s11];
        exception_context.s12 = stack[s12];
        exception_context.s13 = stack[s13];
        exception_context.s14 = stack[s14];
        exception_context.s15 = stack[s15];
        exception_context.fpscr = stack[fpscr];
        exception_context.sp += 72; /* s0-s15, fpsr, reserved */
    }

    /* if CCR.STKALIGN=1, check PSR[9] to know if there is forced stack alignment */
    if ((SCB->CCR & SCB_CCR_STKALIGN_Msk) && (exception_context.psr & 0x200)) {
        exception_context.sp += 4;
    }

    /* update CONTROL.SPSEL and psp if returning to thread mode */
    if (exception_context.exc_return & 0x4) {
        exception_context.control |= 0x2; /* CONTROL.SPSel */
        exception_context.psp = exception_context.sp;
    } else { /* update msp if returning to handler mode */
        exception_context.msp = exception_context.sp;
    }

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
    if ((exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) || (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_BINARY)) {
        /* feed wdt to keep time for context dump */
        exception_feed_wdt();

        exception_printf("\r\nCM4 Register dump begin:\r\n");
        exception_printf("r0  = 0x%08x\r\n", exception_context.r0);
        exception_printf("r1  = 0x%08x\r\n", exception_context.r1);
        exception_printf("r2  = 0x%08x\r\n", exception_context.r2);
        exception_printf("r3  = 0x%08x\r\n", exception_context.r3);
        exception_printf("r4  = 0x%08x\r\n", exception_context.r4);
        exception_printf("r5  = 0x%08x\r\n", exception_context.r5);
        exception_printf("r6  = 0x%08x\r\n", exception_context.r6);
        exception_printf("r7  = 0x%08x\r\n", exception_context.r7);
        exception_printf("r8  = 0x%08x\r\n", exception_context.r8);
        exception_printf("r9  = 0x%08x\r\n", exception_context.r9);
        exception_printf("r10 = 0x%08x\r\n", exception_context.r10);
        exception_printf("r11 = 0x%08x\r\n", exception_context.r11);
        exception_printf("r12 = 0x%08x\r\n", exception_context.r12);
        exception_printf("lr  = 0x%08x\r\n", exception_context.lr);
        exception_printf("pc  = 0x%08x\r\n", exception_context.pc);
        exception_printf("psr = 0x%08x\r\n", exception_context.psr);
        exception_printf("EXC_RET = 0x%08x\r\n", exception_context.exc_return);

        /* FPU context? */
        if ((exception_context.exc_return & 0x10) == 0) {
            exception_printf("s0  = 0x%08x\r\n", exception_context.s0);
            exception_printf("s1  = 0x%08x\r\n", exception_context.s1);
            exception_printf("s2  = 0x%08x\r\n", exception_context.s2);
            exception_printf("s3  = 0x%08x\r\n", exception_context.s3);
            exception_printf("s4  = 0x%08x\r\n", exception_context.s4);
            exception_printf("s5  = 0x%08x\r\n", exception_context.s5);
            exception_printf("s6  = 0x%08x\r\n", exception_context.s6);
            exception_printf("s7  = 0x%08x\r\n", exception_context.s7);
            exception_printf("s8  = 0x%08x\r\n", exception_context.s8);
            exception_printf("s9  = 0x%08x\r\n", exception_context.s9);
            exception_printf("s10 = 0x%08x\r\n", exception_context.s10);
            exception_printf("s11 = 0x%08x\r\n", exception_context.s11);
            exception_printf("s12 = 0x%08x\r\n", exception_context.s12);
            exception_printf("s13 = 0x%08x\r\n", exception_context.s13);
            exception_printf("s14 = 0x%08x\r\n", exception_context.s14);
            exception_printf("s15 = 0x%08x\r\n", exception_context.s15);
            exception_printf("s16 = 0x%08x\r\n", exception_context.s16);
            exception_printf("s17 = 0x%08x\r\n", exception_context.s17);
            exception_printf("s18 = 0x%08x\r\n", exception_context.s18);
            exception_printf("s19 = 0x%08x\r\n", exception_context.s19);
            exception_printf("s20 = 0x%08x\r\n", exception_context.s20);
            exception_printf("s21 = 0x%08x\r\n", exception_context.s21);
            exception_printf("s22 = 0x%08x\r\n", exception_context.s22);
            exception_printf("s23 = 0x%08x\r\n", exception_context.s23);
            exception_printf("s24 = 0x%08x\r\n", exception_context.s24);
            exception_printf("s25 = 0x%08x\r\n", exception_context.s25);
            exception_printf("s26 = 0x%08x\r\n", exception_context.s26);
            exception_printf("s27 = 0x%08x\r\n", exception_context.s27);
            exception_printf("s28 = 0x%08x\r\n", exception_context.s28);
            exception_printf("s29 = 0x%08x\r\n", exception_context.s29);
            exception_printf("s30 = 0x%08x\r\n", exception_context.s30);
            exception_printf("s31 = 0x%08x\r\n", exception_context.s31);
            exception_printf("fpscr = 0x%08x\r\n", exception_context.fpscr);
        }

        exception_printf("CONTROL = 0x%08x\r\n", exception_context.control);
        exception_printf("MSP     = 0x%08x\r\n", exception_context.msp);
        exception_printf("PSP     = 0x%08x\r\n", exception_context.psp);
        exception_printf("sp      = 0x%08x\r\n", exception_context.sp);
        exception_printf("basepri = 0x%08x\r\n", exception_context.basepri);
        exception_printf("primask = 0x%08x\r\n", exception_context.primask);
        exception_printf("faultmask = 0x%08x\r\n", exception_context.faultmask);
        exception_printf("\r\nCM4 Register dump end:\r\n");
    }
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_MINIDUMP) {

        /* feed wdt to keep time for context dump */
        exception_feed_wdt();

        if (minidump_base_address != 0) {

            const char cm4_reg_dump_start[] = {"CM4 Register dump begin:\n"};
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_dump_start, strlen(cm4_reg_dump_start));

            uint32_t i = 0;
            uint32_t cm4_reg_value = 0;
            char cm4_reg_str[32] = {0};

            /* r0 - r9*/
            for (i = 0; i < 10; i++) {
                cm4_reg_value =  *((uint32_t *)&exception_context + i);
                snprintf(&cm4_reg_str[0], 16, "r%d = 0x%08x", (unsigned int)i, (unsigned int)cm4_reg_value);
                cm4_reg_str[15] = '\n';
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, 16);
            }

            /* r10 - r12 */
            for (i = 10; i < 13; i++) {
                cm4_reg_value =  *((uint32_t *)&exception_context + i);
                snprintf(&cm4_reg_str[0], 17, "r%d = 0x%08x", (unsigned int)i, (unsigned int)cm4_reg_value);
                cm4_reg_str[16] = '\n';
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, 17);
            }

            /* feed wdt to keep time for memory dump */
            exception_feed_wdt();

            /* lr */
            snprintf(&cm4_reg_str[0], strlen("lr = 0x") + 8 + 1, "lr = 0x%08x", exception_context.lr);
            cm4_reg_str[strlen("lr = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, strlen("lr = 0x") + 8 + 1);

            /* pc */
            snprintf(&cm4_reg_str[0], strlen("pc = 0x") + 8 + 1, "pc = 0x%08x", exception_context.pc);
            cm4_reg_str[strlen("pc = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, strlen("pc = 0x") + 8 + 1);

            /* psr */
            snprintf(&cm4_reg_str[0], strlen("psr = 0x") + 8 + 1, "psr = 0x%08x", exception_context.psr);
            cm4_reg_str[strlen("psr = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, strlen("psr = 0x") + 8 + 1);

            /* exc_return */
            snprintf(&cm4_reg_str[0], strlen("EXC_RET = 0x") + 8 + 1, "EXC_RET = 0x%08x", exception_context.exc_return);
            cm4_reg_str[strlen("EXC_RET = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, strlen("EXC_RET = 0x") + 8 + 1);

            /* feed wdt to keep time for memory dump */
            exception_feed_wdt();

            /* fpu used? */
            if ((exception_context.exc_return & 0x10) == 0) {
                for (i = 0; i < 10; i++) {
                    cm4_reg_value = *((uint32_t *)&exception_context.s0 + i) ;
                    snprintf(&cm4_reg_str[0], 16, "s%d = 0x%08x", (unsigned int)i, (unsigned int)cm4_reg_value);
                    cm4_reg_str[15] = '\n';
                    offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, 16);
                }
                for (i = 10; i < 32; i++) {
                    cm4_reg_value = *((uint32_t *)&exception_context.s0 + i);
                    snprintf(&cm4_reg_str[0], 17, "s%d = 0x%08x", (unsigned int)i, (unsigned int)cm4_reg_value);
                    cm4_reg_str[16] = '\n';
                    offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, 17);
                }

                /* fpscr */
                snprintf(&cm4_reg_str[0], strlen("fpscr = 0x") + 8 + 1, "fpscr = 0x%08x", exception_context.fpscr);
                cm4_reg_str[strlen("fpscr = 0x") + 8] = '\n';
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, strlen("fpscr = 0x") + 8 + 1);

            }

            /* feed wdt to keep time for memory dump */
            exception_feed_wdt();

            /* control */
            snprintf(&cm4_reg_str[0], strlen("CONTROL = 0x") + 8 + 1, "CONTROL = 0x%08x", exception_context.control);
            cm4_reg_str[strlen("CONTROL = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, strlen("CONTROL = 0x") + 8 + 1);

            /* msp */
            snprintf(&cm4_reg_str[0], strlen("MSP = 0x") + 8 + 1, "MSP = 0x%08x", exception_context.msp);
            cm4_reg_str[strlen("MSP = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, strlen("MSP = 0x") + 8 + 1);

            /* psp */
            snprintf(&cm4_reg_str[0], strlen("PSP = 0x") + 8 + 1, "PSP = 0x%08x", exception_context.psp);
            cm4_reg_str[strlen("PSP = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, strlen("PSP = 0x") + 8 + 1);

            /* sp */
            snprintf(&cm4_reg_str[0], strlen("sp = 0x") + 8 + 1, "sp = 0x%08x", exception_context.sp);
            cm4_reg_str[strlen("sp = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, strlen("sp = 0x") + 8 + 1);

            /* basepri */
            snprintf(&cm4_reg_str[0], strlen("basepri = 0x") + 8 + 1, "basepri = 0x%08x", exception_context.basepri);
            cm4_reg_str[strlen("basepri = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, strlen("basepri = 0x") + 8 + 1);

            /* primask */
            snprintf(&cm4_reg_str[0], strlen("primask = 0x") + 8 + 1, "primask = 0x%08x", exception_context.primask);
            cm4_reg_str[strlen("primask = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, strlen("primask = 0x") + 8 + 1);

            /* faultmask */
            snprintf(&cm4_reg_str[0], strlen("faultmask = 0x") + 8 + 1, "faultmask = 0x%08x", exception_context.faultmask);
            cm4_reg_str[strlen("faultmask = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_str, strlen("faultmask = 0x") + 8 + 1);

            /* feed wdt to keep time for memory dump */
            exception_feed_wdt();

            /* CM4 Register dump end */
            const uint8_t cm4_reg_dump_end[] = {"CM4 Register dump end:\n\n"};
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)cm4_reg_dump_end, strlen((char *)cm4_reg_dump_end));

            /* feed wdt to keep time for memory dump */
            exception_feed_wdt();
        }
    }
#endif /* EXCEPTION_MEMDUMP_MODE */
}

/******************************************************************************/
/*            Exception's memory dump Functions                               */
/******************************************************************************/
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
static void exception_dump_region_info(const memory_region_type *static_regions, exception_user_regions_t *user_regions)
{
    uint32_t i = 0;
    unsigned int *current, *end;

    /* static regions */
    for (i = 0; ; i++) {
        if (!static_regions[i].region_name) {
            break;
        }
        if (!static_regions[i].is_dumped) {
            continue;
        }
        current = (unsigned int *)((uint32_t)(static_regions[i].start_address) & 0xfffffffc);
        end     = (unsigned int *)(static_regions[i].end_address);
        if (current < end) {
            /* feed wdt to keep time for output data */
            exception_feed_wdt();
            /* output region's information */
            exception_printf("Region-%s: start_address = 0x%x, end_address = 0x%x\r\n", static_regions[i].region_name, (unsigned int)current, (unsigned int)end);
        }
    }

    /* dynamic regions */
    for (i = 0; ((i < user_regions->items) && (i < EXCEPTION_CONFIGURATIONS_MAX)); i++) {
        if ((user_regions->regions)[i].is_dumped) {
            current = (unsigned int *)((uint32_t)((user_regions->regions)[i].start_address) & 0xfffffffc);
            end     = (unsigned int *)((user_regions->regions)[i].end_address);
            if (current < end) {
                /* feed wdt to keep time for output data */
                exception_feed_wdt();
                /* output region's information */
                exception_printf("Region-%s: start_address = 0x%x, end_address = 0x%x\r\n", (user_regions->regions)[i].region_name, (unsigned int)current, (unsigned int)end);
            }
        }
    }
}
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT)
static void exception_dump_region_data_text(const memory_region_type *static_regions, exception_user_regions_t *user_regions)
{
    uint32_t i = 0;
    unsigned int *current, *end;

    /* static regions */
    for (i = 0; ; i++) {
        if (!static_regions[i].region_name) {
            break;
        }
        if (!static_regions[i].is_dumped) {
            continue;
        }
        current = (unsigned int *)((uint32_t)(static_regions[i].start_address) & 0xfffffffc);
        end     = (unsigned int *)(static_regions[i].end_address);
        for (; current < end; current += 4) {
            if (*(current + 0) == 0 &&
                    *(current + 1) == 0 &&
                    *(current + 2) == 0 &&
                    *(current + 3) == 0) {
                continue;
            }
            /* feed wdt to keep time for output data */
            exception_feed_wdt();
            /* output data */
            exception_printf("0x%08x: %08x %08x %08x %08x\r\n",
                             (unsigned int)current,
                             *(current + 0),
                             *(current + 1),
                             *(current + 2),
                             *(current + 3));
        }
    }

    /* dynamic regions */
    for (i = 0; ((i < user_regions->items) && (i < EXCEPTION_CONFIGURATIONS_MAX)); i++) {
        if ((user_regions->regions)[i].is_dumped) {
            current = (unsigned int *)((uint32_t)((user_regions->regions)[i].start_address) & 0xfffffffc);
            end     = (unsigned int *)((user_regions->regions)[i].end_address);
            for (; current < end; current += 4) {
                if (*(current + 0) == 0 &&
                        *(current + 1) == 0 &&
                        *(current + 2) == 0 &&
                        *(current + 3) == 0) {
                    continue;
                }
                /* feed wdt to keep time for output data */
                exception_feed_wdt();
                /* output data */
                exception_printf("0x%08x: %08x %08x %08x %08x\r\n",
                                 (unsigned int)current,
                                 *(current + 0),
                                 *(current + 1),
                                 *(current + 2),
                                 *(current + 3));
            }
        }
    }
}
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
static void exception_dump_region_data_binary(const memory_region_type *static_regions, exception_user_regions_t *user_regions)
{
    uint32_t i = 0;
    unsigned int *current, *end;

    /* static regions */
    for (i = 0; ; i++) {
        if (!static_regions[i].region_name) {
            break;
        }
        if (!static_regions[i].is_dumped) {
            continue;
        }
        current = (unsigned int *)((uint32_t)(static_regions[i].start_address) & 0xfffffffc);
        end     = (unsigned int *)(static_regions[i].end_address);
        if (current < end) {
            /* feed wdt to keep time for output data */
            exception_feed_wdt();
            /* output data */
            log_dump_exception_data((const uint8_t *)current, (uint32_t)end - (uint32_t)current);
        }
    }

    /* dynamic regions */
    for (i = 0; ((i < user_regions->items) && (i < EXCEPTION_CONFIGURATIONS_MAX)); i++) {
        if ((user_regions->regions)[i].is_dumped) {
            current = (unsigned int *)((uint32_t)((user_regions->regions)[i].start_address) & 0xfffffffc);
            end     = (unsigned int *)((user_regions->regions)[i].end_address);
            if (current < end) {
                /* feed wdt to keep time for output data */
                exception_feed_wdt();
                /* output data */
                log_dump_exception_data((const uint8_t *)current, (uint32_t)end - (uint32_t)current);
            }
        }
    }
}
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
extern exception_status_t exception_query_slave_region_size(uint32_t core_number,int32_t * size);
extern uint32_t slave0_minidump_region_size;
static void exception_dump_region_data_minidump(const memory_region_type *static_regions, exception_user_regions_t *user_regions)
{
#if ((defined(AB156X) || (PRODUCT_VERSION == 2822)))
    int32_t region_size = 0;
    uint32_t *current, *end;
    bool ret = false;
    uint32_t i = 0;
    uint8_t region_name[128];
    int32_t size = 0;
    /* dsp side dump region size */
    int32_t slave_total_region_size = 0;
    int32_t cm4_total_region_size = 0;
    int32_t cm4_minidump_region_size = 0;

    /* static regions */
    if (minidump_base_address != 0) {
        /* get dsp side dump region size */
        exception_query_slave_region_size(0,&slave_total_region_size);
        /* get cm4 side dump region size */
        exception_query_cm4_region_size(&cm4_total_region_size);

        /* indicate dsp occurs exception */
        if (TRUE == exception_triggered_flag){
            /* dsp occurs exception,cm4 is trigged */
            if (minidump_region_total_size >= slave_total_region_size){
                slave0_minidump_region_size = slave_total_region_size;
                cm4_minidump_region_size = minidump_region_total_size - slave_total_region_size;

            }else{
                slave0_minidump_region_size = minidump_region_total_size;
                cm4_minidump_region_size = 0;
            }

        }else{
            /* cm4 occurs exception,dsp is trigged */
            if (minidump_region_total_size >= cm4_total_region_size){
                cm4_minidump_region_size = cm4_total_region_size;
                slave0_minidump_region_size = minidump_region_total_size -cm4_total_region_size;
            }else{
                cm4_minidump_region_size = minidump_region_total_size;
                slave0_minidump_region_size = 0;
            }
        }

        /* CM4 Regions Information */
        ret = offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"CM4 Regions Information:\n", strlen("CM4 Regions Information:\n"));
        if (ret != true) {
            minidump_base_address = 0;
            return;
        }

        /* overlist region list */
        for (i = 0;; i++) {

            /* region is NULL */
            if (!static_regions[i].region_name) {
                break;
            }

            /* is need to dump */
            if (!static_regions[i].is_dumped) {
                continue;
            }

            /* get start address,end address and size */
            current = (uint32_t *)(((uint32_t)(static_regions[i].start_address)) & 0xfffffffc);
            end 	= (uint32_t *)static_regions[i].end_address;
            region_size = (uint32_t)end - (uint32_t)current;

            /* remain dump size <=  region size */
            if (cm4_minidump_region_size < (62 + strlen(static_regions[i].region_name) + 1)){
                region_size = 0;
                cm4_minidump_region_size = 0;
                snprintf((char *)region_name,50+strlen(static_regions[i].region_name)+1,"[WARNING]:Region-%s,Actually Size is 0x%08x!!!\r\n",static_regions[i].region_name,(unsigned int)region_size);
                ret= offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, region_name, strlen((char *)region_name));
                if (ret != true) {
                    minidump_base_address = 0;
                    return;
                }
                break;
            }else if((cm4_minidump_region_size - (62 + strlen(static_regions[i].region_name) + 1)) < region_size ){
                region_size = cm4_minidump_region_size - (62 + strlen(static_regions[i].region_name) + 1);
                /* update end address */
                end = (uint32_t *)((uint32_t)current + region_size);
            }else{

            }

            if (region_size == 0) {
                continue;
            }

            snprintf((char *)region_name, 62 + strlen(static_regions[i].region_name) + 1, "Region-%s: start_address = 0x%08x, end_address = 0x%08x\n",
                     static_regions[i].region_name, (unsigned int)current, (unsigned int)end);

            /* write region header */
            ret = offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, region_name, strlen((char *)region_name));
            if (ret == false) {
                minidump_base_address = 0;
                return;
            }

            /* clear region name array */
            memset(region_name, 0, 128);

            /* feed wdt to keep time for output data */
            exception_feed_wdt();

            /* write region data */
            size = offline_dump_region_write_with_race_header(EXCEPTION_BINARY, &minidump_base_address, (uint8_t *)current, region_size);
            if (size == false) {
                minidump_base_address = 0;
                return;
            }

            /* update remain region size */
            cm4_minidump_region_size -= (region_size + 62 + strlen(static_regions[i].region_name) + 1);

             /* check is enough space to do dump */
            if (cm4_minidump_region_size <= 0){
                snprintf((char *)region_name,50+strlen(static_regions[i].region_name)+1,"[WARNING]:Region-%s,Actually Size is 0x%08x!!!\r\n",static_regions[i].region_name,(unsigned int)region_size);
                ret= offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, region_name, strlen((char *)region_name));

                if (ret != true) {
                    minidump_base_address = 0;
                    return;
                }
                break;
            }

        }

        /* dynamic regions */
        for (i = 0; ((i < user_regions->items) && (i < EXCEPTION_CONFIGURATIONS_MAX)); i++) {

            /* check is enough space to do dump */
            if (cm4_minidump_region_size == 0){
                region_size = 0;
                snprintf((char *)region_name,50+strlen(user_regions->regions[i].region_name)+1,"[WARNING]:Region-%s,Actually Size is 0x%08x!!!\r\n",user_regions->regions[i].region_name,(unsigned int)region_size);
                ret= offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, region_name, strlen((char *)region_name));
                if (ret != true) {
                    minidump_base_address = 0;
                    return;
                }
                break;
            }


            /* need dump ? */
            if ((user_regions->regions)[i].is_dumped) {
                current = (uint32_t *)(((uint32_t)(user_regions->regions[i].start_address)) & 0xfffffffc);
                end 	= (uint32_t *)user_regions->regions[i].end_address;
                region_size = (uint32_t)end - (uint32_t)current;

                /* remain dump size <=  region size */
                if (cm4_minidump_region_size < (62 + strlen(user_regions->regions[i].region_name) + 1)){
                    region_size = 0;
                    cm4_minidump_region_size = 0;
                    snprintf((char *)region_name,50+strlen(user_regions->regions[i].region_name)+1,"[WARNING]:Region-%s,Actually Size is 0x%08x!!!\r\n",user_regions->regions[i].region_name,(unsigned int)region_size);
                    ret= offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, region_name, strlen((char *)region_name));
                    if (ret != true) {
                        minidump_base_address = 0;
                        return;
                    }
                    break;
                }else if((cm4_minidump_region_size - (62 + strlen(user_regions->regions[i].region_name) + 1)) < region_size ){
                    region_size = cm4_minidump_region_size - (62 + strlen(user_regions->regions[i].region_name) + 1);
                    /* update end address */
                    end = (uint32_t*)((uint32_t)current + region_size);
                }else{

                }

                if (region_size == 0) {
                    continue;
                }

                snprintf((char *)region_name, 62 + strlen(user_regions->regions[i].region_name) + 1, "Region-%s: start_address = 0x%x, end_address = 0x%x\n",
                         user_regions->regions[i].region_name, (unsigned int )current, (unsigned int )end);

            ret = offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, region_name, strlen((char *)region_name));
            if (ret == false) {
                minidump_base_address = 0;
                return;
            }
            memset(region_name, 0, 128);
            /* feed wdt to keep time for output data */
            exception_feed_wdt();

            /* Write binary data frist and then write region information */
            size = offline_dump_region_write_with_race_header(EXCEPTION_BINARY, &minidump_base_address, (uint8_t *)current, region_size);
            if (size == false) {
                minidump_base_address = 0;
                return;
            }

            /* update remain region size */
            cm4_minidump_region_size -= (region_size + 62 + strlen(user_regions->regions[i].region_name) + 1);

            /* remain dump size <=  region size */
            if (cm4_minidump_region_size <= 0){
                region_size = 0;
                cm4_minidump_region_size = 0;
                snprintf((char *)region_name,50+strlen(user_regions->regions[i].region_name)+1,"[WARNING]:Region-%s,Actually Size is 0x%08x!!!\r\n",user_regions->regions[i].region_name,(unsigned int)region_size);
                ret= offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, region_name, strlen((char *)region_name));
                if (ret != true) {
                    minidump_base_address = 0;
                    return;
                }
                break;
            }
          }
        }
    }

    if (minidump_base_address != 0) {
        /* CM4 Regions Data: */
        ret = offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"CM4 Regions Data:\n", strlen("CM4 Regions Data:\n"));
        if (ret == false) {
            minidump_base_address = 0;
            return;
        }

        /* memory dump completed. */
        ret = offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"memory dump completed.\n", strlen("memory dump completed.\n"));
        if (ret == false) {
            minidump_base_address = 0;
            return;
        }
    }

#else
    exception_status_t ret = EXCEPTION_STATUS_ERROR;
    uint8_t region_str[128] = {0};
    /* static regions */
    if (minidump_base_address != 0) {
        /* dump PSP stacks */
        ret = exception_minidump_check_address(exception_context.psp - 100);
        if (ret == EXCEPTION_STATUS_OK) {
            /* feed wdt to keep time for output data */
            exception_feed_wdt();

            /* CM4 Regions Information */
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"CM4 Regions Information:\n", strlen("CM4 Regions Information:\n"));

            snprintf((char *)region_str, 65, "Region-psp: start_address = 0x%08x, end_address = 0x%08x", (exception_context.psp - 100),
                     (exception_context.psp - 100 + MINIDUMP_PSPSTACK_LENGTH));

            region_str[64] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, region_str, 65);

            /* output data */
            offline_dump_region_write_with_race_header(EXCEPTION_BINARY, &minidump_base_address, (uint8_t *)(exception_context.psp - 100), MINIDUMP_PSPSTACK_LENGTH);

        } else {
            /* PSP is not a valid value */
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"PSP IS NOY A VALIAD VALUE\n", strlen("PSP IS NOY A VALIAD VALUE\n"));
        }

        /* dump MSP stacks */
        ret = exception_minidump_check_address(exception_context.msp - 100);
        if (ret == EXCEPTION_STATUS_OK) {
            /* feed wdt to keep time for output data */
            exception_feed_wdt();

            snprintf((char *)region_str, 65, "Region-msp: start_address = 0x%08x, end_address = 0x%08x", (exception_context.msp - 100),
                     (exception_context.msp - 100 + MINIDUMP_MSPSTACK_LENGTH));

            region_str[64] = '\n';

            offline_dump_region_write_with_race_header(EXCEPTION_TEXT,
                    &minidump_base_address,
                    region_str,
                    65);

            /* output data */
            offline_dump_region_write_with_race_header(EXCEPTION_BINARY,
                    &minidump_base_address,
                    (uint8_t *)(exception_context.msp - 100),
                    MINIDUMP_MSPSTACK_LENGTH);


            /* CM4 actlr region */
            uint8_t cm_region_actlr_str[] = {"Region-actlr: start_address = 0xe000e008, end_address = 0xe000e00c\n"};
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, cm_region_actlr_str, strlen((char *)cm_region_actlr_str));
            offline_dump_region_write_with_race_header(EXCEPTION_BINARY, &minidump_base_address, (uint8_t *)0xe000e008, 4);

            /* Region-scb */
            uint8_t cm_region_scb_str[] = {"Region-scb: start_address = 0xe000ed00, end_address = 0xe000ed40\n"};
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, cm_region_scb_str, strlen((char *)cm_region_scb_str));
            offline_dump_region_write_with_race_header(EXCEPTION_BINARY, &minidump_base_address, (uint8_t *)0xe000ed00, 4 * 16);

            /* feed wdt to keep time for context dump */
            exception_feed_wdt();

            /* CM4 Regions Data: */
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"CM4 Regions Data:\n\n", strlen("CM4 Regions Data:\n\n"));

            /* CM4 memory dump completed. */
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"CM4 memory dump completed.\n\n", strlen("CM4 memory dump completed.\n\n"));
        } else {
            /* MSP is not a valid value*/
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"MSP IS NOY A VALIAD VALUE\n", strlen("MSP IS NOY A VALIAD VALUE\n"));
        }
    }
#endif
}

#endif /* EXCEPTION_MEMDUMP_MODE */

static void exception_dump_memory(void)
{
    /* Memory Dump */
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
    if ((exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) || (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_BINARY)) {

        /* feed wdt to keep time for memory dump */
        exception_feed_wdt();

        /* Print Regions' information */
        exception_printf("CM4 Regions Information:\r\n");
        exception_dump_region_info(memory_regions, &exception_user_regions);


#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT)
        if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT)
            /* Print Regions' data */
        {
            exception_dump_region_data_text(memory_regions, &exception_user_regions);
        }
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
        if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_BINARY) {
            /* Print Memory one by one regions */
            exception_printf("CM4 Regions Data:\r\n");
            /* Print Regions' data */
            exception_dump_region_data_binary(memory_regions, &exception_user_regions);
        }
#endif /* EXCEPTION_MEMDUMP_MODE */

        /* dump cm4 end log */
        exception_printf("\r\nmemory dump completed.\r\n");
    }
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)

    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_MINIDUMP) {

        /* feed wdt to keep time for memory dump */
        exception_feed_wdt();

        /* dump memory data into flash */
        exception_dump_region_data_minidump(memory_regions, &exception_user_regions);
    }
#endif /* EXCEPTION_MEMDUMP_MODE */
}

/******************************************************************************/
/*            Exception's slaves dump Functions                               */
/******************************************************************************/
#if (EXCEPTION_SLAVES_TOTAL > 0)
void exception_dump_slaves(void)
{
    uint32_t slave_current = 0;
    uint32_t slave_finish = 0;
    uint32_t gpt_start_count, gpt_current_count, gpt_duration_count;

    /* zero exceptionSlaveStatus */
    for (slave_current = 0; slave_current < EXCEPTION_SLAVES_TOTAL; slave_current++) {
        exceptionSlaveStatus[slave_current] = EXCEPTION_SLAVE_STATUS_IDLE;
    }
    /* check which core is ready */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
    while (slave_finish <  EXCEPTION_SLAVES_TOTAL) {
        /* feed wdt to keep time for query slave status*/
        exception_feed_wdt();
        /* check if any slave can do dump */
        for (slave_current = 0; slave_current < EXCEPTION_SLAVES_TOTAL; slave_current++) {
            if (exceptionSlaveStatus[slave_current] == EXCEPTION_SLAVE_STATUS_IDLE) {
                if ((exceptionSlavesDump[slave_current].is_dump != 0) &&
                        (exceptionSlavesDump[slave_current].slave_checkstatus != NULL) &&
                        (exceptionSlavesDump[slave_current].slave_dump != NULL)) {
                    if (exceptionSlavesDump[slave_current].slave_checkstatus() == EXCEPTION_SLAVE_STATUS_READY) {
                        /* Slave is ready */
                        /* feed wdt to keep time for slave dump */
                        exception_feed_wdt();
                        /* do slave dump */
                        exceptionSlavesDump[slave_current].slave_dump();
                        exceptionSlaveStatus[slave_current] = EXCEPTION_SLAVE_STATUS_FINISH;
                        slave_finish += 1;
                        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
                    } else {
                        /* Slave is not ready */
                        exceptionSlaveStatus[slave_current] = EXCEPTION_SLAVE_STATUS_IDLE;
                    }
                } else {
                    /* Slave configuration is not ok */
                    exceptionSlaveStatus[slave_current] = EXCEPTION_SLAVE_STATUS_ERROR;
                    slave_finish += 1;
                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
                }
            }
        }
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
        hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
        /* Check if timeout */
        if (EXCEPTION_SLAVES_TIMEOUT < gpt_duration_count) {
            for (slave_current = 0; slave_current < EXCEPTION_SLAVES_TOTAL; slave_current++) {
                /* Print Slaves' status */
                if ((exceptionSlavesDump[slave_current].is_dump != 0) &&
                        (exceptionSlavesDump[slave_current].slave_checkstatus != NULL) &&
                        (exceptionSlavesDump[slave_current].slave_dump != NULL)) {
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
                    exception_printf("[Warning]%s:%d\r\n", exceptionSlavesDump[slave_current].slave_name,
                                     exceptionSlaveStatus[slave_current]);
#elif (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
#endif /* EXCEPTION_MEMDUMP_MODE */
                }

                /* Do Forced-dump */
                if ((exceptionSlavesDump[slave_current].is_dump != 0) &&
                        (exceptionSlavesDump[slave_current].slave_checkstatus != NULL) &&
                        (exceptionSlavesDump[slave_current].slave_forceddump != NULL) &&
                        (exceptionSlaveStatus[slave_current] != EXCEPTION_SLAVE_STATUS_FINISH)) {
                    /* feed wdt to keep time for slave forced dump */
                    exception_feed_wdt();
                    /* Slave needs to be forced to do dump */
                    exceptionSlavesDump[slave_current].slave_forceddump();

                    /* Take Exception HW semaphore */
                    while (HAL_HW_SEMAPHORE_STATUS_OK != hw_semaphore_take(EXCEPTION_HW_SEMAPHORE));
                }
            }
            slave_finish = EXCEPTION_SLAVES_TOTAL;
        }
    }
}
#endif /* EXCEPTION_SLAVES_TOTAL */

/******************************************************************************/
/*            Exception's print fault type Functions                          */
/******************************************************************************/
static void printUsageErrorMsg(uint32_t CFSRValue)
{
    exception_printf("Usage fault: ");
    CFSRValue >>= 16; /* right shift to lsb */
    if ((CFSRValue & (1 << 9)) != 0) {
        exception_printf("Divide by zero\r\n");
    }
    if ((CFSRValue & (1 << 8)) != 0) {
        exception_printf("Unaligned access\r\n");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        exception_printf("Coprocessor error\r\n");
    }
    if ((CFSRValue & (1 << 2)) != 0) {
        exception_printf("Invalid EXC_RETURN\r\n");
    }
    if ((CFSRValue & (1 << 1)) != 0) {
        exception_printf("Invalid state\r\n");
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        exception_printf("Undefined instruction\r\n");
    }

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_MINIDUMP) {

        char *exception_log_str = "Usage fault:";
        offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));

        CFSRValue >>= 16; /* right shift to lsb */
        if ((CFSRValue & (1 << 9)) != 0) {
            exception_log_str = "Divide by zero\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));
        }
        if ((CFSRValue & (1 << 8)) != 0) {
            exception_log_str = "Unaligned access\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));
        }
        if ((CFSRValue & (1 << 3)) != 0) {
            exception_log_str = "Coprocessor error\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));

        }
        if ((CFSRValue & (1 << 2)) != 0) {

            exception_log_str = "Invalid EXC_RETURN\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));

        }
        if ((CFSRValue & (1 << 1)) != 0) {
            exception_log_str = "Invalid state\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));
        }
        if ((CFSRValue & (1 << 0)) != 0) {
            exception_log_str = "Undefined instruction\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));
        }
    }
#endif
}

static void printMemoryManagementErrorMsg(uint32_t CFSRValue)
{
    exception_printf("Memory Management fault: ");
    CFSRValue &= 0x000000FF; /* mask mem faults */
    if ((CFSRValue & (1 << 5)) != 0) {
        exception_printf("A MemManage fault occurred during FP lazy state preservation\r\n");
    }
    if ((CFSRValue & (1 << 4)) != 0) {
        exception_printf("A derived MemManage fault occurred on exception entry\r\n");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        exception_printf("A derived MemManage fault occurred on exception return\r\n");
    }
    if ((CFSRValue & (1 << 1)) != 0) { /* Need to check valid bit (bit 7 of CFSR)? */
        exception_printf("Data access violation @0x%08x\r\n", (unsigned int)SCB->MMFAR);
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        exception_printf("MPU or Execute Never (XN) default memory map access violation\r\n");
    }
    if ((CFSRValue & (1 << 7)) != 0) { /* To review: remove this if redundant */
        exception_printf("SCB->MMFAR = 0x%08x\r\n", (unsigned int)SCB->MMFAR);
    }


#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_MINIDUMP) {

        char exception_info_buffer[128] = {};
        char *exception_log_str = "Memory Management fault: ";

        offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));


        CFSRValue &= 0x000000FF; /* mask mem faults */
        if ((CFSRValue & (1 << 5)) != 0) {

            exception_log_str = "A MemManage fault occurred during FP lazy state preservation\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));
        }
        if ((CFSRValue & (1 << 4)) != 0) {

            exception_log_str = "A derived MemManage fault occurred on exception entry\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));
        }
        if ((CFSRValue & (1 << 3)) != 0) {
            exception_log_str = "A derived MemManage fault occurred on exception return\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));
        }
        if ((CFSRValue & (1 << 1)) != 0) { /* Need to check valid bit (bit 7 of CFSR)? */
            sprintf(exception_info_buffer, "Data access violation @0x%08x\r\n", (unsigned int)SCB->MMFAR);
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_info_buffer, strlen(exception_info_buffer));
        }
        if ((CFSRValue & (1 << 0)) != 0) {
            exception_log_str = "MPU or Execute Never (XN) default memory map access violation\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));
        }
        if ((CFSRValue & (1 << 7)) != 0) { /* To review: remove this if redundant */
            sprintf(exception_info_buffer, "SCB->MMFAR = 0x%08x\r\n", (unsigned int)SCB->MMFAR);
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_info_buffer, strlen(exception_info_buffer));
        }
    }
#endif
}

static void printBusFaultErrorMsg(uint32_t CFSRValue)
{
    exception_printf("Bus fault: ");
    CFSRValue &= 0x0000FF00; /* mask bus faults */
    CFSRValue >>= 8;
    if ((CFSRValue & (1 << 5)) != 0) {
        exception_printf("A bus fault occurred during FP lazy state preservation\r\n");
    }
    if ((CFSRValue & (1 << 4)) != 0) {
        exception_printf("A derived bus fault has occurred on exception entry\r\n");
    }
    if ((CFSRValue & (1 << 3)) != 0) {
        exception_printf("A derived bus fault has occurred on exception return\r\n");
    }
    if ((CFSRValue & (1 << 2)) != 0) {
        exception_printf("Imprecise data access error has occurred\r\n");
    }
    if ((CFSRValue & (1 << 1)) != 0) { /* Need to check valid bit (bit 7 of CFSR)? */
        exception_printf("A precise data access error has occurred @x%08x\r\n", (unsigned int)SCB->BFAR);
    }
    if ((CFSRValue & (1 << 0)) != 0) {
        exception_printf("A bus fault on an instruction prefetch has occurred\r\n");
    }
    if ((CFSRValue & (1 << 7)) != 0) { /* To review: remove this if redundant */
        exception_printf("SCB->BFAR = 0x%08x\r\n", (unsigned int)SCB->BFAR);
    }


#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_MINIDUMP) {

        char exception_info_buffer[128] = {};
        char *exception_log_str = "Bus fault: ";
        offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));
        CFSRValue &= 0x0000FF00; /* mask bus faults */
        CFSRValue >>= 8;
        if ((CFSRValue & (1 << 5)) != 0) {
            exception_log_str = "A bus fault occurred during FP lazy state preservation\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));
        }
        if ((CFSRValue & (1 << 4)) != 0) {
            exception_log_str = "A derived bus fault has occurred on exception entry\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));
        }
        if ((CFSRValue & (1 << 3)) != 0) {
            exception_log_str = "A derived bus fault has occurred on exception return\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));
        }
        if ((CFSRValue & (1 << 2)) != 0) {
            exception_log_str = "Imprecise data access error has occurred\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));
        }
        if ((CFSRValue & (1 << 1)) != 0) { /* Need to check valid bit (bit 7 of CFSR)? */
            sprintf(exception_info_buffer, "A precise data access error has occurred @x%08x\r\n", (unsigned int)SCB->BFAR);
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));
        }
        if ((CFSRValue & (1 << 0)) != 0) {
            exception_log_str = "A bus fault on an instruction prefetch has occurred\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_log_str, strlen(exception_log_str));

        }
        if ((CFSRValue & (1 << 7)) != 0) { /* To review: remove this if redundant */
            sprintf(exception_info_buffer, "SCB->BFAR = 0x%08x\r\n", (unsigned int)SCB->BFAR);
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_info_buffer, strlen(exception_info_buffer));
        }
    }
#endif /* EXCEPTION_MEMDUMP_MODE */
}
/******************************************************************************/
/*            Exception's dump processor Functions                            */
/******************************************************************************/
void exception_dump_preprocess(uint32_t fault_type)
{
#ifdef HAL_DWT_MODULE_ENABLED
    uint32_t dwt_function3 = DWT->FUNCTION3;
    uint32_t stack_end = DWT->COMP3;
    uint32_t is_match;
#endif
    exception_info.reason = fault_type;
    exception_info.assert_expr = &assert_expr;

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
    if ((exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) || (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_BINARY)) {

        /* Genie start message */
        exception_printf("<<<<<<<< LOG START LOG START LOG START LOG START LOG START <<<<<<<<\r\n");
    }
#endif
    /* feed wdt to keep time for preprocess */
    exception_feed_wdt();

    exception_printf("\r\nCM4 Fault Dump:\r\n");
    exception_print_assert_info();
    exception_printf("Exception Count = 0x%08x\r\n", (unsigned int)exception_info.count);
    exception_printf("Exception Time = 0x%08x\r\n", (unsigned int)exception_info.timestamp_32k);

    switch (exception_info.reason) {
        case EXCEPTION_NMI:
            exception_printf("\r\nIn NMI Fault Handler\r\n");
            break;
        case EXCEPTION_HARD_FAULT:
            exception_printf("\r\nIn Hard Fault Handler\r\n");
            break;
        case EXCEPTION_MEMMANAGE_FAULT:
            exception_printf("\r\nIn MemManage Fault Handler\r\n");
            break;
        case EXCEPTION_BUS_FAULT:
            exception_printf("\r\nIn Bus Fault Handler\r\n");
            break;
        case EXCEPTION_USAGE_FAULT:
            exception_printf("\r\nIn Usage Fault Handler\r\n");
            break;
        case EXCEPTION_DEBUGMON_FAULT:
            exception_printf("\r\nIn Debug Monitor Fault Handler\r\n");
#ifdef HAL_DWT_MODULE_ENABLED
            /* is task stack overflow? */
            is_match = (dwt_function3 & DWT_FUNCTION_MATCHED_Msk) >> DWT_FUNCTION_MATCHED_Pos;
            exception_printf("Task stack overflow:%c, stack end:0x%x \r\n", ((is_match) ? 'Y' : 'N'), (unsigned int)stack_end);
#endif /* HAL_DWT_MODULE_ENABLED */
            break;
        default:
            exception_printf("\r\nIn Unknow Fault Handler\r\n");
            break;
    }

    exception_printf("SCB->HFSR = 0x%08x\r\n", (unsigned int)SCB->HFSR);
    exception_printf("SCB->CFSR = 0x%08x\r\n", (unsigned int)SCB->CFSR);
    if ((SCB->HFSR & (1 << 30)) != 0) {
        exception_printf("Forced Hard Fault\r\n");
    }
    if ((SCB->CFSR & 0xFFFF0000) != 0) {
        printUsageErrorMsg(SCB->CFSR);
    }
    if ((SCB->CFSR & 0x0000FF00) != 0) {
        printBusFaultErrorMsg(SCB->CFSR);
    }
    if ((SCB->CFSR & 0x000000FF) != 0) {
        printMemoryManagementErrorMsg(SCB->CFSR);
    }

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)

    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_MINIDUMP) {

        /* feed wdt to keep time for reset the latest index*/
        exception_feed_wdt();
        /* reset the latest index */
        exception_minidump_update_latest_index(0x0);
        /* check if there has been same minidump on the flash */
        if (exception_minidump_check_not_duplicated() == EXCEPTION_STATUS_OK) {

            /* there is not the same minidump on the flash, do this minidump */
            /* clean FOTA status to prevent FOTA upgrade after reboot */
            exception_minidump_fota_status_clean();

            /* feed wdt to keep time for offline dump init*/
            exception_feed_wdt();

            /* minidump region init */
            offline_dump_region_init();

            /* feed wdt to keep time for offline dump alloc */
            exception_feed_wdt();

            /*  Get the minidump region address */
            if (offline_dump_region_alloc(OFFLINE_REGION_MINI_DUMP, &minidump_base_address) != true) {
                minidump_base_address = 0;
            }
            minidump_ori_address = minidump_base_address;
        } else {

            /* there is the same minidump on the flash, drop this dump */
            minidump_base_address = 0;
        }

        /* get minidump total size */
		if( true != offline_dump_region_query_cell_size(OFFLINE_REGION_MINI_DUMP, (uint32_t *)&minidump_total_size)){
			minidump_total_size = 0;
			minidump_base_address = 0;
		}

        /* get minidump dump region total size,include cm4 and dsp */
        minidump_region_total_size = minidump_total_size >= MINIDUMP_FIXED_STRING_SIZE ?
                                    (minidump_total_size - MINIDUMP_FIXED_STRING_SIZE):0;

        exception_printf("minidump_base_address = 0x%x,minidump_total_size= 0x%x\r\n",minidump_base_address,minidump_total_size);

        if (minidump_base_address != 0) {
            /* dump swla information to flash */
            #ifdef MTK_SWLA_ENABLE
                uint32_t pxBase = 0;
                uint32_t pxLen = 0;
                extern void SLA_get_region(uint32_t *pxBase, uint32_t *pxLen);
                SLA_get_region(&pxBase,&pxLen);
                char swla_log_buff[128] = {};
                snprintf((char *)swla_log_buff, 46, "####SWLA enabled[0x%08x,0x%08x]####\r\n", (unsigned int)pxBase,(unsigned int)pxLen);
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)swla_log_buff, strlen((char *)swla_log_buff));
            #endif

            /* feed wdt to keep time for preprocess */
            exception_feed_wdt();

            char  *log_str = "<<<<<<<< LOG START LOG START LOG START LOG START LOG START <<<<<<<<\r\n";

            /* Genie start message */
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)log_str, strlen(log_str));

            log_str = "\r\nCM4 Fault Dump:\r\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)log_str, strlen(log_str));

#if !((defined(AB156X) || (PRODUCT_VERSION == 2822)))
            /* build time string */
            extern char build_date_time_str[];
            uint8_t build_time_str[64] = {0};
            snprintf((char *)build_time_str, (22 + strlen(build_date_time_str) + 1), "build_date_time_str = %s", build_date_time_str);
            build_time_str[22 + strlen(build_date_time_str)] = '\n';
            build_time_str[22 + strlen(build_date_time_str) + 1] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, build_time_str, (22 + strlen(build_date_time_str) + 1 + 1));
#endif
            /* TODO:assert expression size */
            //exception_print_assert_info();

            char exception_context_buffer[128] = {};
            snprintf((char *)exception_context_buffer, 22 + 8 + 1, "Exception Count = 0x%08x\r\n", (unsigned int)exception_info.count);
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_context_buffer, strlen((char *)exception_context_buffer));
            memset(exception_context_buffer, 0, 128);

            snprintf((char *)exception_context_buffer, 21 + 8 + 1, "Exception Time = 0x%08x\r\n", (unsigned int)exception_info.timestamp_32k);
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_context_buffer, strlen((char *)exception_context_buffer));
            memset(exception_context_buffer, 0, 128);

            switch (exception_info.reason) {
                case EXCEPTION_NMI:
                    log_str = "\r\nIn NMI Fault Handler\r\n";
                    offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)log_str, strlen(log_str));
                    break;
                case EXCEPTION_HARD_FAULT:
                    log_str = "\r\nIn Hard Fault Handler\r\n";
                    offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)log_str, strlen(log_str));
                    break;
                case EXCEPTION_MEMMANAGE_FAULT:
                    log_str = "\r\nIn MemManage Fault Handler\r\n";
                    offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)log_str, strlen(log_str));
                    break;
                case EXCEPTION_BUS_FAULT:
                    log_str = "\r\nIn Bus Fault Handler\r\n";
                    offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)log_str, strlen(log_str));
                    break;
                case EXCEPTION_USAGE_FAULT:
                    log_str = "\r\nIn Usage Fault Handler\r\n";
                    offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)log_str, strlen(log_str));
                    break;
                case EXCEPTION_DEBUGMON_FAULT:
                    log_str = "\r\nIn Debug Monitor Fault Handler\r\n";
                    offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)log_str, strlen(log_str));
#ifdef HAL_DWT_MODULE_ENABLED
                    /* is task stack overflow? */
                    is_match = (dwt_function3 & DWT_FUNCTION_MATCHED_Msk) >> DWT_FUNCTION_MATCHED_Pos;
                    sprintf((char *)exception_context_buffer, "Task stack overflow:%c, stack end:0x%x \r\n", ((is_match) ? 'Y' : 'N'), (unsigned int)stack_end);
                    offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_context_buffer, strlen((char *)exception_context_buffer));
                    memset(exception_context_buffer, 0, 128);
#endif /* HAL_DWT_MODULE_ENABLED */
                    break;
                default:
                    log_str = "\r\nIn Unknow Fault Handler\r\n";
                    offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)log_str, strlen(log_str));
                    break;
            }

            snprintf((char *)exception_context_buffer, 16 + 8 + 1, "SCB->HFSR = 0x%08x\r\n", (unsigned int)SCB->HFSR);
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_context_buffer, strlen((char *)exception_context_buffer));
            memset(exception_context_buffer, 0, 128);

            snprintf((char *)exception_context_buffer, 16 + 8 + 1, "SCB->CFSR = 0x%08x\r\n", (unsigned int)SCB->CFSR);
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)exception_context_buffer, strlen((char *)exception_context_buffer));
            memset(exception_context_buffer, 0, 128);

            if ((SCB->HFSR & (1 << 30)) != 0) {
                log_str = "Forced Hard Fault\r\n";
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)log_str, strlen(log_str));
            }
            if ((SCB->CFSR & 0xFFFF0000) != 0) {
                printUsageErrorMsg(SCB->CFSR);
            }
            if ((SCB->CFSR & 0x0000FF00) != 0) {
                printBusFaultErrorMsg(SCB->CFSR);
            }
            if ((SCB->CFSR & 0x000000FF) != 0) {
                printMemoryManagementErrorMsg(SCB->CFSR);
            }
        }
    }
#endif /* EXCEPTION_MEMDUMP_MODE */
}

void exception_dump_postprocess(void)
{
    uint32_t i = 0;

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
    if ((exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) || (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_BINARY)) {
#ifdef MTK_SWLA_ENABLE
        extern void SLA_MemoryCallbackInit(void);
#endif

        for (i = 0; i < exception_config.items; i++) {
#ifdef MTK_SWLA_ENABLE
            if (exception_config.configs[i].init_cb && (exception_config.configs[i].init_cb != SLA_MemoryCallbackInit)) {
#else
            if (exception_config.configs[i].init_cb) {
#endif

                /* feed wdt to keep time for init callback */
                exception_feed_wdt();
                /* run init callback */
                exception_config.configs[i].init_cb();
            }
        }
    }
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_MINIDUMP) {
        uint32_t dump_count = 0;

        /* feed wdt to keep time for postprocess */
        exception_feed_wdt();

        /* write mini dump header */
        if (minidump_base_address != 0) {
            exception_minidump_header_update();

            /* feed wdt to keep time for minidump header */
            exception_feed_wdt();

            char * minidump_end_str = "<<<<<<<< LOG END LOG END LOG END LOG END LOG END <<<<<<<<\n";
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT,
                    &minidump_base_address,
                    (uint8_t *)minidump_end_str,
                    strlen(minidump_end_str));

        }
        /* feed wdt to keep time for minidump ending */
        exception_feed_wdt();

        if ((minidump_base_address != minidump_ori_address) || (minidump_base_address != 0)) {
            /* update status to minidump control block */
            if (offline_dump_region_write_end(OFFLINE_REGION_MINI_DUMP, minidump_total_size) == true) {
                /* update latest index */
                exception_minidump_region_query_count(&dump_count);
                exception_minidump_update_latest_index(dump_count);

                /* log service post-process, such as save log to flash */
                exception_log_service_post_process();
            }
        }

    }
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
    if ((exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) || (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_BINARY)) {
        /* Genie complete message */
        exception_printf("<<<<<<<< LOG END LOG END LOG END LOG END LOG END <<<<<<<<\r\n");
    }
#endif

}

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/
void exception_cm4_fault_handler(uint32_t stack[], uint32_t fault_type)
{

    /* dump exception time, fault type, etc */
    exception_dump_preprocess(fault_type);

    /* dump the context when the exception happens */
    exception_dump_context(stack);

    /* dump the memory */
    exception_dump_memory();

#if (EXCEPTION_SLAVES_TOTAL > 0)
    /* do the slave dump */
    exception_dump_slaves();
#endif /* EXCEPTION_SLAVES_TOTAL */

    /* finish the dump */
    exception_dump_postprocess();

    /* check if reboot */
    if (reboot_check() == DISABLE_WHILELOOP_MAGIC) {
        exception_reboot();
    } else {

        /* disable wdt reset mode for entering while loop */
        /* maybe wdt has been changed to interrupt mode for triggering the NMI interrupt */
        exception_disable_wdt_reset();

#if (defined(AB156X) || (PRODUCT_VERSION == 2822))
        /* it will enable cap touch lpsd and power key */
        pmu_enable_lpsd();
#endif

        extern void syslog_port_disable_flowcontrol(void);
        syslog_port_disable_flowcontrol();
        while (1);

    }


}

/******************************************************************************/
/*                   Toolchain Dependent Part                                 */
/******************************************************************************/
#if defined (__CC_ARM)

#define __EXHDLR_ATTR__ __asm ATTR_TEXT_IN_RAM

/**
  * @brief  This function is the common part of exception handlers.
  * @param  r3 holds EXC_RETURN value
  * @retval None
  */
__EXHDLR_ATTR__ void CommonFault_Handler(void)
{
    extern exception_context_pointer
    extern exception_stack_pointer

    PRESERVE8

    mrs r2, primask               /* move primask to r2           */
    cpsid i                       /* disable irq                  */
    ldr r3, = exception_stack_pointer
              ldr r3, [r3]                  /* r3 := exception_stack_pointer       */
              ldr r0, = exception_context_pointer
                        ldr r0, [r0]                  /* r0 := exception_context_pointer      */
                        add r0, r0, #16               /* point to context.r4          */
                        stmia r0!, {r4 - r11}         /* store r4-r11                 */
                        mov r5, r12                   /* r5 := EXC_RETURN             */
                        add r0, r0, #20               /* point to context.control     */
                        mrs r1, control               /* move CONTROL to r1           */
                        str r1, [r0], #4              /* store CONTROL                */
                        str r5, [r0], #4              /* store EXC_RETURN             */
                        mrs r4, msp                   /* r4 := MSP                    */
                        str r4, [r0], #4              /* store MSP                    */
                        mrs r1, psp                   /* move PSP to r1               */
                        str r1, [r0], #4              /* store PSP                    */
                        mrs r1, basepri               /* move basepri to r1           */
                        str r1, [r0], #4              /* store basepri                */
                        mov r1, r2                    /* move primask to r1           */
                        str r1, [r0], #4              /* store primask                */
                        mrs r1, faultmask             /* move faultmask to r1         */
                        str r1, [r0]                  /* store faultmask              */
                        tst r5, #0x10                 /* FPU context?                 */
                        itt eq
                        addeq r0, r0, #68             /* point to contex.s16          */
                        vstmeq r0, {s16 - s31}        /* store s16-s31                */
                        cmp r3, #0                    /* if (!exception_stack_pointer)       */
                        it ne
                        movne sp, r3                  /* update msp                   */
                        push {lr}
                        bl __cpp(exception_init)
                        pop  {lr}
                        tst r5, #4                    /* thread or handler mode?      */
                        ite eq
                        moveq r0, r4
                        mrsne r0, psp
                        bx lr
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void HardFault_Handler(void)
{
    PRESERVE8

    mov r12, lr
    bl __cpp(CommonFault_Handler)
    mov r1, #3
    bl __cpp(exception_cm4_fault_handler)
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void MemManage_Handler(void)
{
    PRESERVE8

    mov r12, lr
    bl __cpp(CommonFault_Handler)
    mov r1, #4
    bl __cpp(exception_cm4_fault_handler)
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void BusFault_Handler(void)
{
    PRESERVE8

    mov r12, lr
    bl __cpp(CommonFault_Handler)
    mov r1, #5
    bl __cpp(exception_cm4_fault_handler)
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void UsageFault_Handler(void)
{
    PRESERVE8

    mov r12, lr
    bl __cpp(CommonFault_Handler)
    mov r1, #6
    bl __cpp(exception_cm4_fault_handler)
}

#ifdef HAL_DWT_MODULE_ENABLED
__EXHDLR_ATTR__  void DebugMon_Handler(void)
{
    PRESERVE8

    mov r12, lr
    bl __cpp(CommonFault_Handler)
    mov r1, #12
    bl __cpp(exception_cm4_fault_handler)
}
#endif /* HAL_DWT_MODULE_ENABLED */

#endif /* __CC_ARM */

#if defined (__ICCARM__)

#define __EXHDLR_ATTR__  __stackless

ATTR_TEXT_IN_RAM void CommonFault_Handler(void);
ATTR_TEXT_IN_RAM void HardFault_Handler(void);
ATTR_TEXT_IN_RAM void MemManage_Handler(void);
ATTR_TEXT_IN_RAM void BusFault_Handler(void);
ATTR_TEXT_IN_RAM void UsageFault_Handler(void);
ATTR_TEXT_IN_RAM void DebugMon_Handler(void);

/**
  * @brief  This function is the common part of exception handlers.
  * @param  r3 holds EXC_RETURN value
  * @retval None
  */
__EXHDLR_ATTR__ void CommonFault_Handler(void)
{
    __asm volatile
    (
        "mrs r2, primask               \n"     /* move primask to r2          */
        "cpsid i                       \n"     /* disable irq                 */
        "mov r3, %0                    \n"     /* r3 := exception_stack_pointer      */
        "mov r0, %1                    \n"     /* r0 := exception_context_pointer     */
        "add r0, r0, #16               \n"     /* point to context.r4         */
        "stmia r0!, {r4-r11}           \n"     /* store r4-r11                */
        "mov r5, r12                   \n"     /* r5 := EXC_RETURN            */
        "add r0, r0, #20               \n"     /* point to context.control    */
        "mrs r1, control               \n"     /* move CONTROL to r1          */
        "str r1, [r0], #4              \n"     /* store CONTROL               */
        "str r5, [r0], #4              \n"     /* store EXC_RETURN            */
        "mrs r4, msp                   \n"     /* r4 := MSP                   */
        "str r4, [r0], #4              \n"     /* store MSP                   */
        "mrs r1, psp                   \n"     /* move PSP to r1              */
        "str r1, [r0], #4              \n"     /* store PSP                   */
        "mrs r1, basepri               \n"     /* move basepri to r1          */
        "str r1, [r0], #4              \n"     /* store basepri               */
        "mov r1, r2                    \n"     /* move primask to r1          */
        "str r1, [r0], #4              \n"     /* store primask               */
        "mrs r1, faultmask             \n"     /* move faultmask to r1        */
        "str r1, [r0]                  \n"     /* store faultmask             */
        "tst r5, #0x10                 \n"     /* FPU context?                */
        "itt eq                        \n"
        "addeq r0, r0, #68             \n"     /* point to contex.s16         */
        "vstmeq r0, {s16-s31}          \n"     /* store s16-s31               */
        "cmp r3, #0                    \n"     /* if (!exception_stack_pointer)      */
        "it ne                         \n"
        "movne sp, r3                  \n"     /* update msp                  */
        "push {lr}                     \n"
        "bl exception_init             \n"
        "pop {lr}                      \n"
        "tst r5, #4                    \n"     /* thread or handler mode?     */
        "ite eq                        \n"
        "moveq r0, r4                  \n"
        "mrsne r0, psp                 \n"
        "bx lr                         \n"
        ::"r"(exception_stack_pointer), "r"(exception_context_pointer)
    );
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void HardFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "mov r1, #3                    \n"
        "bl exception_cm4_fault_handler\n"
    );
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void MemManage_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "mov r1, #4                    \n"
        "bl exception_cm4_fault_handler\n"
    );
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void BusFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "mov r1, #5                    \n"
        "bl exception_cm4_fault_handler\n"
    );
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
__EXHDLR_ATTR__ void UsageFault_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "mov r1, #6                    \n"
        "bl exception_cm4_fault_handler\n"
    );
}

#ifdef HAL_DWT_MODULE_ENABLED
__EXHDLR_ATTR__  void DebugMon_Handler(void)
{
    __asm volatile
    (
        "mov r12, lr                   \n"
        "bl CommonFault_Handler        \n"
        "mov r1, #12                   \n"
        "bl exception_cm4_fault_handler\n"
    );
}
#endif /* HAL_DWT_MODULE_ENABLED */

#endif /* __ICCARM__ */
