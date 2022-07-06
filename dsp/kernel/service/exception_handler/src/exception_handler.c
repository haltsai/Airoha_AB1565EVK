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

 /* Includes -----------------------------------------------------------------*/
#include "stdio.h"
#include "stdarg.h"
#include "memory_attribute.h"
#include "exception_handler.h"
#include "exception_portable.h"
#include "hal_gpt.h"
#include "hal_uart.h"
#include "hal_ccni.h"
#include "hal_hw_semaphore.h"
#include <xtensa/hal.h>
#include <xtensa/xtruntime.h>
#include "hal_uart.h"
#include "hal_ccni_config.h"
#ifdef HAL_CACHE_MODULE_ENABLED
#include "hal_cache.h"
#endif
#ifdef MTK_SWLA_ENABLE
#include "swla.h"
#endif

/* Private define ------------------------------------------------------------*/
#define EXCEPTION_STACK_WORDS                   (256)

#define EXCEPTION_CONFIGURATIONS_MAX            (6)

#define EXCEPTION_RESERVED_WORDS                (111)

#define EXCEPTION_PATTERN1                      (0xff00a5a5)
#define EXCEPTION_PATTERN2                      (0x5a5a00ff)

#define EXCEPTION_TRACE_DSP0

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
    int items;
    exception_config_type configs[EXCEPTION_CONFIGURATIONS_MAX];
} exception_config_t;

typedef struct
{
    uint32_t items;
    memory_region_type regions[EXCEPTION_CONFIGURATIONS_MAX];
} exception_user_regions_t;

typedef struct
{
    uint32_t count;
    uint32_t timestamp;
    uint32_t timestamp_32k;
    uint32_t reason;
    assert_expr_t *assert_expr;
} exception_info_t;

#if (EXCEPTION_AR_COUNT == 32)
/* Total 106 word, 424 Byte */
typedef struct
{
    /* General core registers */
    uint32_t ar0;
    uint32_t ar1;
    uint32_t ar2;
    uint32_t ar3;
    uint32_t ar4;
    uint32_t ar5;
    uint32_t ar6;
    uint32_t ar7;
    uint32_t ar8;
    uint32_t ar9;
    uint32_t ar10;
    uint32_t ar11;
    uint32_t ar12;
    uint32_t ar13;
    uint32_t ar14;
    uint32_t ar15;
    uint32_t ar16;
    uint32_t ar17;
    uint32_t ar18;
    uint32_t ar19;
    uint32_t ar20;
    uint32_t ar21;
    uint32_t ar22;
    uint32_t ar23;
    uint32_t ar24;
    uint32_t ar25;
    uint32_t ar26;
    uint32_t ar27;
    uint32_t ar28;
    uint32_t ar29;
    uint32_t ar30;
    uint32_t ar31;
    /* Window option special registers */
    uint32_t windowbase;
    uint32_t windowstart;
    /* Loop option special registers */
    uint32_t lbeg;
    uint32_t lend;
    uint32_t lcount;
    /* Shift amount special registers */
    uint32_t sar;
    /* Comparison special registers */
    uint32_t scompare;
    /* Exception and Interrupt option special registers */
    uint32_t exccause;
    uint32_t excvaddr;
    uint32_t pc;
    uint32_t epc1;
    uint32_t epc2;
    uint32_t epc3;
    uint32_t epc4;
    uint32_t epc5;
    uint32_t epc6;
    uint32_t epcnmi;
    uint32_t depc;
    uint32_t ps;
    uint32_t eps2;
    uint32_t eps3;
    uint32_t eps4;
    uint32_t eps5;
    uint32_t eps6;
    uint32_t epsnmi;
    uint32_t excsave1;
    uint32_t excsave2;
    uint32_t excsave3;
    uint32_t excsave4;
    uint32_t excsave5;
    uint32_t excsave6;
    uint32_t excsavenmi;
    uint32_t intenable;
    uint32_t interrupt;
    /* Bool option special registers */
    uint32_t br;
    /* Coprocessor option special registers */
    uint32_t cpenable;
    /* Debug option special registers */
    uint32_t debugcause;
    uint32_t ibreakenable;
    uint32_t ibreaka0;
    uint32_t ibreaka1;
    uint32_t dbreaka0;
    uint32_t dbreaka1;
    uint32_t dbreakc0;
    uint32_t dbreakc1;
    /* DSP engine special registers( 8 Byte aligned ) */
    uint64_t aep0;
    uint64_t aep1;
    uint64_t aep2;
    uint64_t aep3;
    uint64_t aep4;
    uint64_t aep5;
    uint64_t aep6;
    uint64_t aep7;
    uint64_t aeq0;
    uint64_t aeq1;
    uint64_t aeq2;
    uint64_t aeq3;
    uint32_t ae_ovf_sar;
    uint32_t ae_bithead;
    uint32_t ae_ts_fts_bu_bp;
    uint32_t ae_sd_no;
    uint32_t ae_cbegin0;
    uint32_t ae_cend0;
} exception_context_t;
#elif (EXCEPTION_AR_COUNT == 64)
/* Total 138 word, 552 Byte */
typedef struct
{
    /* General core registers */
    uint32_t ar0;
    uint32_t ar1;
    uint32_t ar2;
    uint32_t ar3;
    uint32_t ar4;
    uint32_t ar5;
    uint32_t ar6;
    uint32_t ar7;
    uint32_t ar8;
    uint32_t ar9;
    uint32_t ar10;
    uint32_t ar11;
    uint32_t ar12;
    uint32_t ar13;
    uint32_t ar14;
    uint32_t ar15;
    uint32_t ar16;
    uint32_t ar17;
    uint32_t ar18;
    uint32_t ar19;
    uint32_t ar20;
    uint32_t ar21;
    uint32_t ar22;
    uint32_t ar23;
    uint32_t ar24;
    uint32_t ar25;
    uint32_t ar26;
    uint32_t ar27;
    uint32_t ar28;
    uint32_t ar29;
    uint32_t ar30;
    uint32_t ar31;
    uint32_t ar32;
    uint32_t ar33;
    uint32_t ar34;
    uint32_t ar35;
    uint32_t ar36;
    uint32_t ar37;
    uint32_t ar38;
    uint32_t ar39;
    uint32_t ar40;
    uint32_t ar41;
    uint32_t ar42;
    uint32_t ar43;
    uint32_t ar44;
    uint32_t ar45;
    uint32_t ar46;
    uint32_t ar47;
    uint32_t ar48;
    uint32_t ar49;
    uint32_t ar50;
    uint32_t ar51;
    uint32_t ar52;
    uint32_t ar53;
    uint32_t ar54;
    uint32_t ar55;
    uint32_t ar56;
    uint32_t ar57;
    uint32_t ar58;
    uint32_t ar59;
    uint32_t ar60;
    uint32_t ar61;
    uint32_t ar62;
    uint32_t ar63;
    /* Window option special registers */
    uint32_t windowbase;
    uint32_t windowstart;
    /* Loop option special registers */
    uint32_t lbeg;
    uint32_t lend;
    uint32_t lcount;
    /* Shift amount special registers */
    uint32_t sar;
    /* Comparison special registers */
    uint32_t scompare;
    /* Exception and Interrupt option special registers */
    uint32_t exccause;
    uint32_t excvaddr;
    uint32_t pc;
    uint32_t epc1;
    uint32_t epc2;
    uint32_t epc3;
    uint32_t epc4;
    uint32_t epc5;
    uint32_t epc6;
    uint32_t epcnmi;
    uint32_t depc;
    uint32_t ps;
    uint32_t eps2;
    uint32_t eps3;
    uint32_t eps4;
    uint32_t eps5;
    uint32_t eps6;
    uint32_t epsnmi;
    uint32_t excsave1;
    uint32_t excsave2;
    uint32_t excsave3;
    uint32_t excsave4;
    uint32_t excsave5;
    uint32_t excsave6;
    uint32_t excsavenmi;
    uint32_t intenable;
    uint32_t interrupt;
    /* Bool option special registers */
    uint32_t br;
    /* Coprocessor option special registers */
    uint32_t cpenable;
    /* Debug option special registers */
    uint32_t debugcause;
    uint32_t ibreakenable;
    uint32_t ibreaka0;
    uint32_t ibreaka1;
    uint32_t dbreaka0;
    uint32_t dbreaka1;
    uint32_t dbreakc0;
    uint32_t dbreakc1;
    /* DSP engine special registers( 8 Byte aligned ) */
    uint64_t aep0;
    uint64_t aep1;
    uint64_t aep2;
    uint64_t aep3;
    uint64_t aep4;
    uint64_t aep5;
    uint64_t aep6;
    uint64_t aep7;
    uint64_t aeq0;
    uint64_t aeq1;
    uint64_t aeq2;
    uint64_t aeq3;
    uint32_t ae_ovf_sar;
    uint32_t ae_bithead;
    uint32_t ae_ts_fts_bu_bp;
    uint32_t ae_sd_no;
    uint32_t ae_cbegin0;
    uint32_t ae_cend0;
} exception_context_t;
#endif /* EXCEPTION_AR_COUNT */

/* 4 + 4 + 4 + 4 + 4 + 3*4 + 4*2 + 4*2 + 4*2 + 4*EXCEPTION_RESERVED_WORDS + 4 + 4 + 4 = 512B */
typedef struct
{
    uint32_t packet_head;
    uint32_t corenum;
    uint32_t count;
    uint32_t timestamp;
    /* These variables will be included in BCC verify */
    assert_expr_t* assert_info;
    /* These variables will be included in BCC verify */
    uint32_t heap_start_addr;
    uint32_t heap_size_guard_enable;
	/* These variables will not be included in BCC verify */
    uint32_t reserved2;
    /* These variables will be included in BCC verify */
    exception_context_t* context_regs;
    /* These variables will be included in BCC verify */
    uint32_t context_regs_num;
    /* These variables will be included in BCC verify */
    memory_region_type* static_regions;
    /* These variables will be included in BCC verify */
    uint32_t static_regions_items;
    /* These variables will be included in BCC verify */
    memory_region_type* dynamic_regions;
    /* These variables will be included in BCC verify */
    uint32_t dynamic_regions_items;
    uint32_t context_backup[EXCEPTION_RESERVED_WORDS];
	/* These variables will be included in BCC verify */
	uint32_t swla_start_addr;
	uint32_t swla_size;

    uint32_t data_checksum;
    uint32_t packet_size;
    uint32_t packet_tail;
} exception_sharemem_t;


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* exception handler's stack */
__attribute__ ((aligned (16))) static unsigned int exception_stack[EXCEPTION_STACK_WORDS] = {0};
unsigned int * const exception_stack_pointer = &exception_stack[EXCEPTION_STACK_WORDS - 4];

/* exception user configuration area */
static exception_config_t exception_config;
static exception_user_regions_t exception_user_regions;

/* assert information area */
static assert_expr_t assert_expr;
ATTR_LOG_STRING assert_string[] = "DSP%d assert : %s, file: %s, line: %d\r\n";

/* exception information area */
static exception_info_t exception_info;
exception_info_t *exception_info_pointer = &exception_info;

/* exception context area */
__attribute__ ((aligned (8))) static exception_context_t exception_context;
exception_context_t * const exception_context_pointer = &exception_context;

/* exception share memory area */
exception_sharemem_t * const exception_sharemem_pointer = (exception_sharemem_t *)EXCEPTION_SHARE_MEMORY_ADDRESS;

/* exception dump memory regions in region_init.c */
extern const memory_region_type memory_regions[];

/* except ab156x and 2822,exception_config_mode minidump bit,fulldump bit alawys true,but macro determines whether code is build*/
#if !defined(AB156X) && (PRODUCT_VERSION != 2822)
/* repeat 4 Byets for checking,wdt reset mode disable*/
volatile exception_config_mode_t exception_config_mode = {0xbfbfbfbf};
#else
/* ab156x and mt2822 minidump macro default defined,it means minidumop code is alawys been build,and if exception_config_mode
 * minidump bit is true,it will run minidump,otherwise it is just been build but not run */
/* repeat 4 Byets for checking */
volatile exception_config_mode_t exception_config_mode = {0};
#endif


/* Private functions ---------------------------------------------------------*/
/******************************************************************************/
/*            Exception's assert Functions                                    */
/******************************************************************************/
__attribute__((optimize ("-O0"))) void platform_assert(const char *expr, const char *file, int line)
{
    static uint32_t primask_backup_assert = 0;

    primask_backup_assert = XTOS_SET_INTLEVEL(4);
    assert_expr.is_valid = 1;
    assert_expr.expr = expr;
    assert_expr.file = file;
    assert_expr.line = line;
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;
    for (;;);

    /* Just to avoid compiler warnings. */
    ( void ) primask_backup_assert;
}

__attribute__((optimize ("-O0"))) void light_assert(const char *expr, const char *file, int line)
{
    static uint32_t primask_backup_assert = 0;

    primask_backup_assert = XTOS_SET_INTLEVEL(4);
    assert_expr.is_valid = 2;
    assert_expr.expr = expr;
    assert_expr.file = file;
    assert_expr.line = line;
    assert_expr.string = assert_string;
    *((volatile unsigned int *) 0xFFFFFFF1) = 1;
    for (;;);

    /* Just to avoid compiler warnings. */
    ( void ) primask_backup_assert;
}

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

/******************************************************************************/
/*            Exception's regitser callbacks Functions                        */
/******************************************************************************/
exception_status_t exception_register_callbacks(exception_config_type *cb)
{
    int i;

    if (exception_config.items >= EXCEPTION_CONFIGURATIONS_MAX)
    {
        return EXCEPTION_STATUS_ERROR;
    }
    else
    {
       /* check if it is already registered */
        for (i = 0; i < exception_config.items; i++)
        {
            if ( exception_config.configs[i].init_cb == cb->init_cb
                && exception_config.configs[i].dump_cb == cb->dump_cb)
            {
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
    if (exception_user_regions.items >= EXCEPTION_CONFIGURATIONS_MAX)
    {
        return EXCEPTION_STATUS_ERROR;
    }
    else
    {
        exception_user_regions.regions[exception_user_regions.items].region_name = region->region_name;
        exception_user_regions.regions[exception_user_regions.items].start_address = region->start_address;
        exception_user_regions.regions[exception_user_regions.items].end_address = region->end_address;
        exception_user_regions.regions[exception_user_regions.items].is_dumped = region->is_dumped;
        exception_user_regions.items++;
        return EXCEPTION_STATUS_OK;
    }
}

/******************************************************************************/
/*            Exception's Common Functions                                    */
/******************************************************************************/
void exception_init(void)
{
    int i;

    exception_core_status_update();

#ifdef MTK_SWLA_ENABLE
    /* add a record for exception to mark the end of the last thread */
    const uint8_t ucExceptionRec[5] = "excp";
    const uint32_t xExceptionRec = (uint32_t)(ucExceptionRec[0] | (ucExceptionRec[1] << 8) | (ucExceptionRec[2] << 16) | (ucExceptionRec[3] << 24));
    SLA_RamLogging(xExceptionRec);

#endif

    for (i = 0; ((i < exception_config.items) && (i < EXCEPTION_CONFIGURATIONS_MAX)); i++)
    {
        if (exception_config.configs[i].init_cb)
        {
            exception_config.configs[i].init_cb();
        }
    }
}

static void ReviseAddressRegsitersLocation(exception_context_t *pEC)
{
    uint32_t shift = 0;
    uint32_t i = 0;
    uint32_t buff[EXCEPTION_AR_COUNT] = {0};

    shift = pEC->windowbase * 4;
    for(i = 0; i < EXCEPTION_AR_COUNT; i++)
    {
        buff[(i + shift) % EXCEPTION_AR_COUNT] = *((uint32_t *)((uint32_t)pEC + i * 4));
    }
    for(i = 0; i < EXCEPTION_AR_COUNT; i++)
    {
        *((uint32_t *)((uint32_t)pEC + i * 4)) = buff[i];
    }
}

static void PrepareExceptionShareMemory(exception_sharemem_t *pESM, exception_info_t *pEI, exception_context_t *pEC,
                                        const memory_region_type *static_regions, exception_user_regions_t *pEUR)
{
    uint32_t i = 0;
    uint32_t check_sum = 0x0;

    pESM->packet_head = EXCEPTION_PATTERN1;
    pESM->corenum = EXCEPTION_CORENUM;

#ifdef MTK_SUPPORT_HEAP_DEBUG

    extern uint8_t ucHeap[];
	pESM->heap_start_addr =  (uint32_t)&ucHeap;
#else

	pESM->heap_start_addr = 0xdeadbeef;
#endif

	check_sum ^= pESM->heap_start_addr;

#if defined(MTK_HEAP_SIZE_GUARD_ENABLE)
	pESM->heap_size_guard_enable = TRUE;
#else
	pESM->heap_size_guard_enable = FALSE;
#endif

	check_sum ^=  pESM->heap_size_guard_enable;

#ifdef MTK_SWLA_ENABLE
	extern void SLA_get_region(uint32_t *pxBase, uint32_t *pxLen);
	SLA_get_region(&pESM->swla_start_addr,&pESM->swla_size);
#else
	pESM->swla_start_addr = 0xdeadbeef;
	pESM->swla_size = 0xdeadbeef;
#endif
	check_sum ^= pESM->swla_start_addr;
	check_sum ^= pESM->swla_size;

    /* prepare Exception Count */
    pESM->count = pEI->count;
    /* prepare Exception Time Stamp */
    pESM->timestamp = pEI->timestamp;

    /* prepare Assert Information */
    pESM->assert_info = pEI->assert_expr;
    check_sum ^= (uint32_t)(pEI->assert_expr);

    /* prepare Exception Context */
    pESM->context_regs = pEC;
    check_sum ^= (uint32_t)(pEC);
    pESM->context_regs_num = EXCEPTION_CONTEXT_WORDS;
    check_sum ^= EXCEPTION_CONTEXT_WORDS;

    /* prepare static memory regions */
    pESM->static_regions = (memory_region_type *)static_regions;
    check_sum ^= (uint32_t)(static_regions);
    pESM->static_regions_items = 0;
    for (i = 0;; i++)
    {
        if ( !(static_regions[i].region_name) ) {
            break;
        }
        pESM->static_regions_items += 1;
    }
    check_sum ^= pESM->static_regions_items;

    /* prepare dynamic memory regions */
    pESM->dynamic_regions = (memory_region_type *)(pEUR->regions);
    check_sum ^= (uint32_t)(pEUR->regions);
    pESM->dynamic_regions_items = pEUR->items;
    check_sum ^= pESM->dynamic_regions_items;

    /* prepare backup context for really double exception */
    if(pEI->count == 1)
    {
#if EXCEPTION_CONTEXT_WORDS < EXCEPTION_RESERVED_WORDS
        for(i = 0; i < EXCEPTION_CONTEXT_WORDS; i++)
        {
            *((uint32_t *)(&(pESM->context_backup)) + i) = *((uint32_t *)pEC + i);
            // *((uint32_t *)((uint32_t)(&(pESM->context_backup)) + i * 4)) = *((uint32_t *)((uint32_t)pEC + i * 4));
        }
#else /* EXCEPTION_CONTEXT_WORDS >= EXCEPTION_RESERVED_WORDS */
        for(i = 0; i < EXCEPTION_RESERVED_WORDS; i++)
        {
            *((uint32_t *)(&(pESM->context_backup)) + i) = *((uint32_t *)pEC + i);
            // *((uint32_t *)((uint32_t)(&(pESM->context_backup)) + i * 4)) = *((uint32_t *)((uint32_t)pEC + i * 4));
        }
#endif /* EXCEPTION_CONTEXT_WORDS < EXCEPTION_RESERVED_WORDS */
    }

    pESM->data_checksum = check_sum;
    pESM->packet_size = 512;
    pESM->packet_tail = EXCEPTION_PATTERN2;

    return;
}

#ifdef EXCEPTION_TRACE_DSP0
static uint8_t trace_offset = 0;

void exception_dsp0_trace_hook(void)
{
	*((volatile uint32_t *)(HW_SYSRAM_PRIVATE_MEMORY_DSP_EXCEPTION_INFO_START+ 4*trace_offset)) =
											((exception_info.count & 0xff) << 24) | (trace_offset << 16) | (trace_offset << 8) | (trace_offset << 0);
	trace_offset++;
}
#else

#define exception_dsp0_trace_hook()

#endif

/******************************************************************************/
/*            DSP Processor Exceptions Handlers                               */
/******************************************************************************/
void exception_dsp_fault_handler(void)
{

#ifdef EXCEPTION_TRACE_DSP0
	/* Init trace_offset */
	trace_offset = 0;
#endif

    /* Update Exception Count */
    exception_info.count += 1;

	/* Trace dsp exception flow */
	exception_dsp0_trace_hook();

	extern hal_hw_semaphore_status_t hw_semaphore_take(hal_hw_semaphore_id_t id);
    /* When occur force dump,hw sema is taken by cm4 */
    while(HAL_HW_SEMAPHORE_STATUS_OK != hw_semaphore_take(EXCEPTION_HW_SEMAPHORE));

	extern hal_hw_semaphore_status_t hw_semaphore_give(hal_hw_semaphore_id_t id);
	/* Give hw sema */
	hw_semaphore_give(EXCEPTION_HW_SEMAPHORE);

	/* Trace dsp exception flow */
	exception_dsp0_trace_hook();

    /* Trigger other Core's exception */
    exception_alert_other_cores();

	/* Trace dsp exception flow */
	exception_dsp0_trace_hook();

    /* Get current time stamp */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M , &(exception_info.timestamp));
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K , &(exception_info.timestamp_32k));

    /* Update Assert Information */
    exception_info.assert_expr = &assert_expr;

    /* Exception init */
    exception_init();

	/* Trace dsp exception flow */
	exception_dsp0_trace_hook();;

    /* ARx has shift in exception_context, so need to revise it */
    ReviseAddressRegsitersLocation(exception_context_pointer);

	/* Trace dsp exception flow */
	exception_dsp0_trace_hook();

    /* Prepare exception context for the master */
    PrepareExceptionShareMemory(exception_sharemem_pointer, exception_info_pointer, exception_context_pointer, memory_regions, &exception_user_regions);

	/* Trace dsp exception flow */
	exception_dsp0_trace_hook();

#ifdef HAL_CACHE_MODULE_ENABLED
    /* Flush Cache to ensure data coherence in Master's view */
    hal_cache_flush_all_cache_lines();
#endif

	/* Trace dsp exception flow */
	exception_dsp0_trace_hook();

    /* Check if the master has entered exception mode */
    uint32_t master_status = HAL_CCNI_EVENT_STATUS_IDLE;
    while(master_status != HAL_CCNI_EVENT_STATUS_BUSY)
    {
        hal_ccni_query_event_status(EXCEPTION_EVENT_FROM_MASTER, &master_status);
    }

	/* Trace dsp exception flow */
	exception_dsp0_trace_hook();

	/* Alert master that this slave's exception context is ready */
    hal_ccni_clear_event(EXCEPTION_EVENT_FROM_MASTER);

	/* Trace dsp exception flow */
	exception_dsp0_trace_hook();


    while(1);
}
