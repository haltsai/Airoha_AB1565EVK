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
#include "hal_resource_assignment.h"
#include "hal_uart.h"
#include "hal_ccni.h"
#include "hal_ccni_config.h"
#include "hal_core_status.h"
#include "memory_attribute.h"
#include "exception_handler.h"
#include "exception_handler_dsp.h"

#ifdef HAL_WDT_MODULE_ENABLED
#include "hal_wdt.h"
#endif

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
#include "offline_dump.h"
#endif /* EXCEPTION_MEMDUMP_MODE */


/* Private define -----------------------------------------------------------*/
#define EXCEPTION_DSP_PATTERN1                          (0xff00a5a5)
#define EXCEPTION_DSP_PATTERN2                          (0x5a5a00ff)

#define EXCEPTION_CONTEXT_WORDS_DSP                     (138)
#define EXCEPTION_RESERVED_WORDS_DSP                    (111)

#define EXCEPTION_SHARE_MEMORY_ADDRESS_DSP0             (HW_SYSRAM_PRIVATE_MEMORY_EXCEPTION_START)

#define EXCEPTION_EVENT_TO_DSP0                         IRQGEN_CM4_TO_DSP0_EXCEPTION
#define EXCEPTION_EVENT_FROM_DSP0                       IRQGEN_DSP0_TO_CM4_EXCEPTION


/* Private typedef ----------------------------------------------------------*/
/* Define the linked list structure.  This is used to link free and allocated blocks in order
of their memory address. */
typedef struct A_BLOCK_LINK {
    struct A_BLOCK_LINK *pxNextFreeBlock;        /*<< The next free block in the list. */
    struct A_BLOCK_LINK *pxNextAllocatedBlock;   /*<< The next allocated block in the list. */
    size_t xBlockSize;                           /*<< The size of the free block. */
    uint32_t xMallocLinkRegAddr;
    uint32_t xFreeLinkRegAddr;
} dsp_BlockLink_t;

/* Define the heap pool header structure.  This is used to mange the heap pool. */
typedef struct A_MULTI_POOL_HEADER_LINK {
    struct A_BLOCK_LINK xStart;             /* 8B << The free block start. */
    struct A_BLOCK_LINK *pxEnd;             /* 4B << The end block of the pool. */
    size_t xFreeBytesRemaining;             /* 4B << The number of free bytes remaining, but says nothing about fragmentation. */
    size_t xMinimumEverFreeBytesRemaining;  /* 4B << The ever minimum free bytes remaining, but says nothing about fragmentation. */
    size_t xPoolAlignment;                  /* 4B << The pool alignment rule, each allocated memory block will be aligned with this rule. */
    struct A_BLOCK_LINK *pxFirstBlock;      /* 4B << Record first block of heap for heap walk */
    size_t xMagicPattern;                   /* 4B << The magic number for pool validity check */
} MultiPoolHeaderLink_t;

/* Total 138 word, 552 Byte */
typedef struct {
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
} exception_context_dsp_t;

/* 4 + 4 + 4 + 4 + 4 + 3*4 + 4*2 + 4*2 + 4*2 + 4*EXCEPTION_RESERVED_WORDS + 4 + 4 + 4 = 512B */
typedef struct {
    uint32_t packet_head;
    uint32_t corenum;
    uint32_t count;
    uint32_t timestamp;
    /* These variables will be included in BCC verify */
    assert_expr_t *assert_info;
    /* These variables will be included in BCC verify */
    uint32_t heap_start_addr;
    uint32_t heap_size_guard_enable;
    /* These variables will not be included in BCC verify */
    uint32_t reserved2;
    /* These variables will be included in BCC verify */
    exception_context_dsp_t *context_regs;
    /* These variables will be included in BCC verify */
    uint32_t context_regs_num;
    /* These variables will be included in BCC verify */
    memory_region_type *static_regions;
    /* These variables will be included in BCC verify */
    uint32_t static_regions_items;
    /* These variables will be included in BCC verify */
    memory_region_type *dynamic_regions;
    /* These variables will be included in BCC verify */
    uint32_t dynamic_regions_items;
	/* These variables will be included in BCC verify */
    uint32_t context_backup[EXCEPTION_RESERVED_WORDS_DSP];
    /* These variables will be included in BCC verify */
	uint32_t swla_start_addr;
	uint32_t swla_size;

    uint32_t data_checksum;
    uint32_t packet_size;
    uint32_t packet_tail;
} exception_sharemem_dsp_t;

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
typedef struct {
    uint32_t address;
    uint32_t size;
} exception_minidump_region_dsp_t;

typedef struct {
    uint32_t is_valid;
    const char *expr;
    const char *file;
    uint32_t line;
} exception_minidump_assert_dsp_t;

typedef struct {
    uint32_t length;
    char name[20];
    uint32_t reason;
    exception_minidump_assert_dsp_t assert_info;
    uint32_t context_size;
    exception_minidump_region_dsp_t regions[8];
    uint32_t data_checksum;
} exception_minidump_header_dsp_t;
#endif /* EXCEPTION_MEMDUMP_MODE */


/* Private macro ------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
uint32_t minidump_base_address_dsp = 0;
exception_minidump_header_dsp_t minidump_header_dsp = {0};
uint32_t slave0_minidump_region_size = 0;
#endif /* EXCEPTION_MEMDUMP_MODE */

extern volatile exception_config_mode_t exception_config_mode;

extern int32_t  offline_dump_region_write_with_race_header(exception_log_type_t log_type, uint32_t *curr_addr,  uint8_t *data, uint32_t size);

/* Private functions --------------------------------------------------------*/
/******************************************************************************/
/*            Exception's Common Functions                                    */
/******************************************************************************/
static exception_status_t exception_dsp_packet_checkout(int num, exception_sharemem_dsp_t *pESM)
{
    uint32_t i = 0;
    uint32_t check_sum = 0x0;
    exception_context_dsp_t *context_regs = NULL;

    /* Check packet head and tail */
    if ((pESM->packet_head != EXCEPTION_DSP_PATTERN1) ||
            (pESM->packet_tail != EXCEPTION_DSP_PATTERN2)) {
        return EXCEPTION_STATUS_ERROR;
    }

    /* Check core number */
    if (pESM->corenum != num) {
        return EXCEPTION_STATUS_ERROR;
    }

    /* Check variables (BCC verify) */
    check_sum ^= pESM->heap_start_addr;
    check_sum ^= pESM->heap_size_guard_enable;
    check_sum ^= pESM->swla_start_addr;
    check_sum ^= pESM->swla_size;
    check_sum ^= (uint32_t)(pESM->assert_info);
    check_sum ^= (uint32_t)(pESM->context_regs);
    check_sum ^= pESM->context_regs_num;
    check_sum ^= (uint32_t)(pESM->static_regions);
    check_sum ^= pESM->static_regions_items;
    check_sum ^= (uint32_t)(pESM->dynamic_regions);
    check_sum ^= pESM->dynamic_regions_items;
    if (check_sum != pESM->data_checksum) {
        return EXCEPTION_STATUS_ERROR;
    }

    /* CM4 need to transfer DSP0 address into CM4 view */
    context_regs = (exception_context_dsp_t *)hal_memview_dsp0_to_cm4((uint32_t)(pESM->context_regs));

    /* check backup registers */
#if EXCEPTION_CONTEXT_WORDS_DSP < EXCEPTION_RESERVED_WORDS_DSP
    for (i = 0; i < EXCEPTION_CONTEXT_WORDS_DSP; i++) {
        if (*(pESM->context_backup + i) != *((uint32_t *)context_regs + i)) {
            exception_printf("[Warning]context registers and backup registers are different\r\n");
            break;
        }
    }
#else /* EXCEPTION_CONTEXT_WORDS_DSP >= EXCEPTION_RESERVED_WORDS_DSP */
    for (i = 0; i < EXCEPTION_RESERVED_WORDS_DSP; i++) {
        if (*(pESM->context_backup + i) != *((uint32_t *)context_regs + i)) {
            exception_printf("[Warning]context registers and backup registers are different\r\n");
            break;
        }
    }
#endif /* EXCEPTION_CONTEXT_WORDS_DSP < EXCEPTION_RESERVED_WORDS_DSP */

    return EXCEPTION_STATUS_OK;
}

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
exception_status_t exception_query_slave_region_size(uint32_t core_number,uint32_t * size){
    exception_sharemem_dsp_t *pESM = (exception_sharemem_dsp_t *)EXCEPTION_SHARE_MEMORY_ADDRESS_DSP0 + 0x200 * core_number;
    uint32_t i = 0;
    uint32_t start_address = 0;
    uint32_t end_address = 0;

    if(EXCEPTION_STATUS_OK == exception_dsp_packet_checkout(core_number,pESM)){
        for (i = 0;i < pESM->static_regions_items;i++){

            if(pESM->static_regions[i].is_dumped == 1){
                start_address = (uint32_t)(pESM->static_regions[i].start_address);
                end_address = (uint32_t)(pESM->static_regions[i].end_address);
                if (end_address >= start_address){
                   *size += end_address - start_address;
                }
            }
        }

        for (i = 0;i< pESM->dynamic_regions_items;i++){

            if (pESM->dynamic_regions[i].is_dumped == 1){
                start_address = (uint32_t)(pESM->dynamic_regions[i].start_address);
                end_address = (uint32_t)(pESM->dynamic_regions[i].end_address);
                if (end_address >= start_address){
                    *size += end_address - start_address;
                }
            }
        }
    }else{
        *size = 0;
        return EXCEPTION_STATUS_ERROR;
    }

    *size += 4*1024;
    return EXCEPTION_STATUS_OK;
}
#endif

/******************************************************************************/
/*            Exception's context dump Functions                              */
/******************************************************************************/
static void exception_dsp_dump_context(int num, exception_sharemem_dsp_t *pESM)
{
    exception_context_dsp_t *context_regs = NULL;

    /* nodump option is not selected,it means EXCEPTION_MEMDUMP_TEXT or EXCEPTION_MEMDUMP_Binary or
     * minidump is selected.
     */
    if (!(exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_NODUMP)) {

        /* feed wdt to keep time for context dump */
        exception_feed_wdt();

        /* CM4 need to transfer DSP0 address into CM4 view */
        context_regs = (exception_context_dsp_t *)hal_memview_dsp0_to_cm4((uint32_t)(pESM->context_regs));
        if (NULL == context_regs) {
            exception_printf("[Warning]:dsp reg context is NULL!!!\r\n");
        }

        /* NMI interrupt */
        if (context_regs->epsnmi != 0x0) {
            /* change ps and epsnmi */
            context_regs->ps     = (context_regs->ps) ^ (context_regs->epsnmi);
            context_regs->epsnmi = (context_regs->ps) ^ (context_regs->epsnmi);
            context_regs->ps     = (context_regs->ps) ^ (context_regs->epsnmi);
        }

        /* Debug exception */
        if (context_regs->debugcause != 0x0) {
            /* clear PS.EXCM because PS.EXCM is 0 when Debug exception happens */
            context_regs->ps &= 0xffffffef;
        }
    }

    /* Context Dump */
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
    if ((exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) || (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_BINARY)) {
        /* feed wdt to keep time for context dump */
        exception_feed_wdt();
        if (NULL != context_regs) {
            /* General core registers */
            exception_printf("DSP%d Register dump begin:\r\n", pESM->corenum);
            exception_printf("ar0  = 0x%08x\r\n", context_regs->ar0);
            exception_printf("ar1  = 0x%08x\r\n", context_regs->ar1);
            exception_printf("ar2  = 0x%08x\r\n", context_regs->ar2);
            exception_printf("ar3  = 0x%08x\r\n", context_regs->ar3);
            exception_printf("ar4  = 0x%08x\r\n", context_regs->ar4);
            exception_printf("ar5  = 0x%08x\r\n", context_regs->ar5);
            exception_printf("ar6  = 0x%08x\r\n", context_regs->ar6);
            exception_printf("ar7  = 0x%08x\r\n", context_regs->ar7);
            exception_printf("ar8  = 0x%08x\r\n", context_regs->ar8);
            exception_printf("ar9  = 0x%08x\r\n", context_regs->ar9);
            exception_printf("ar10  = 0x%08x\r\n", context_regs->ar10);
            exception_printf("ar11  = 0x%08x\r\n", context_regs->ar11);
            exception_printf("ar12  = 0x%08x\r\n", context_regs->ar12);
            exception_printf("ar13  = 0x%08x\r\n", context_regs->ar13);
            exception_printf("ar14  = 0x%08x\r\n", context_regs->ar14);
            exception_printf("ar15  = 0x%08x\r\n", context_regs->ar15);
            exception_printf("ar16  = 0x%08x\r\n", context_regs->ar16);
            exception_printf("ar17  = 0x%08x\r\n", context_regs->ar17);
            exception_printf("ar18  = 0x%08x\r\n", context_regs->ar18);
            exception_printf("ar19  = 0x%08x\r\n", context_regs->ar19);
            exception_printf("ar20  = 0x%08x\r\n", context_regs->ar20);
            exception_printf("ar21  = 0x%08x\r\n", context_regs->ar21);
            exception_printf("ar22  = 0x%08x\r\n", context_regs->ar22);
            exception_printf("ar23  = 0x%08x\r\n", context_regs->ar23);
            exception_printf("ar24  = 0x%08x\r\n", context_regs->ar24);
            exception_printf("ar25  = 0x%08x\r\n", context_regs->ar25);
            exception_printf("ar26  = 0x%08x\r\n", context_regs->ar26);
            exception_printf("ar27  = 0x%08x\r\n", context_regs->ar27);
            exception_printf("ar28  = 0x%08x\r\n", context_regs->ar28);
            exception_printf("ar29  = 0x%08x\r\n", context_regs->ar29);
            exception_printf("ar30  = 0x%08x\r\n", context_regs->ar30);
            exception_printf("ar31  = 0x%08x\r\n", context_regs->ar31);
            exception_printf("ar32  = 0x%08x\r\n", context_regs->ar32);
            exception_printf("ar33  = 0x%08x\r\n", context_regs->ar33);
            exception_printf("ar34  = 0x%08x\r\n", context_regs->ar34);
            exception_printf("ar35  = 0x%08x\r\n", context_regs->ar35);
            exception_printf("ar36  = 0x%08x\r\n", context_regs->ar36);
            exception_printf("ar37  = 0x%08x\r\n", context_regs->ar37);
            exception_printf("ar38  = 0x%08x\r\n", context_regs->ar38);
            exception_printf("ar39  = 0x%08x\r\n", context_regs->ar39);
            exception_printf("ar40  = 0x%08x\r\n", context_regs->ar40);
            exception_printf("ar41  = 0x%08x\r\n", context_regs->ar41);
            exception_printf("ar42  = 0x%08x\r\n", context_regs->ar42);
            exception_printf("ar43  = 0x%08x\r\n", context_regs->ar43);
            exception_printf("ar44  = 0x%08x\r\n", context_regs->ar44);
            exception_printf("ar45  = 0x%08x\r\n", context_regs->ar45);
            exception_printf("ar46  = 0x%08x\r\n", context_regs->ar46);
            exception_printf("ar47  = 0x%08x\r\n", context_regs->ar47);
            exception_printf("ar48  = 0x%08x\r\n", context_regs->ar48);
            exception_printf("ar49  = 0x%08x\r\n", context_regs->ar49);
            exception_printf("ar50  = 0x%08x\r\n", context_regs->ar50);
            exception_printf("ar51  = 0x%08x\r\n", context_regs->ar51);
            exception_printf("ar52  = 0x%08x\r\n", context_regs->ar52);
            exception_printf("ar53  = 0x%08x\r\n", context_regs->ar53);
            exception_printf("ar54  = 0x%08x\r\n", context_regs->ar54);
            exception_printf("ar55  = 0x%08x\r\n", context_regs->ar55);
            exception_printf("ar56  = 0x%08x\r\n", context_regs->ar56);
            exception_printf("ar57  = 0x%08x\r\n", context_regs->ar57);
            exception_printf("ar58  = 0x%08x\r\n", context_regs->ar58);
            exception_printf("ar59  = 0x%08x\r\n", context_regs->ar59);
            exception_printf("ar60  = 0x%08x\r\n", context_regs->ar60);
            exception_printf("ar61  = 0x%08x\r\n", context_regs->ar61);
            exception_printf("ar62  = 0x%08x\r\n", context_regs->ar62);
            exception_printf("ar63  = 0x%08x\r\n", context_regs->ar63);
            /* Window option special registers */
            exception_printf("windowbase  = 0x%08x\r\n", context_regs->windowbase);
            exception_printf("windowstart  = 0x%08x\r\n", context_regs->windowstart);
            /* Loop option special registers */
            exception_printf("lbeg  = 0x%08x\r\n", context_regs->lbeg);
            exception_printf("lend  = 0x%08x\r\n", context_regs->lend);
            exception_printf("lcount  = 0x%08x\r\n", context_regs->lcount);
            /* Shift amount special registers */
            exception_printf("sar  = 0x%08x\r\n", context_regs->sar);
            /* Comparison special registers */
            exception_printf("scompare  = 0x%08x\r\n", context_regs->scompare);
            /* Exception and Interrupt option special registers */
            exception_printf("exccause  = 0x%08x\r\n", context_regs->exccause);
            exception_printf("excvaddr  = 0x%08x\r\n", context_regs->excvaddr);
            exception_printf("pc  = 0x%08x\r\n", context_regs->pc);
            exception_printf("epc1  = 0x%08x\r\n", context_regs->epc1);
            exception_printf("epc2  = 0x%08x\r\n", context_regs->epc2);
            exception_printf("epc3  = 0x%08x\r\n", context_regs->epc3);
            exception_printf("epc4  = 0x%08x\r\n", context_regs->epc4);
            exception_printf("epc5  = 0x%08x\r\n", context_regs->epc5);
            exception_printf("epc6  = 0x%08x\r\n", context_regs->epc6);
            exception_printf("epcnmi  = 0x%08x\r\n", context_regs->epcnmi);
            exception_printf("depc  = 0x%08x\r\n", context_regs->depc);
            exception_printf("ps  = 0x%08x\r\n", context_regs->ps);
            exception_printf("eps2  = 0x%08x\r\n", context_regs->eps2);
            exception_printf("eps3  = 0x%08x\r\n", context_regs->eps3);
            exception_printf("eps4  = 0x%08x\r\n", context_regs->eps4);
            exception_printf("eps5  = 0x%08x\r\n", context_regs->eps5);
            exception_printf("eps6  = 0x%08x\r\n", context_regs->eps6);
            exception_printf("epsnmi  = 0x%08x\r\n", context_regs->epsnmi);
            exception_printf("excsave1  = 0x%08x\r\n", context_regs->excsave1);
            exception_printf("excsave2  = 0x%08x\r\n", context_regs->excsave2);
            exception_printf("excsave3  = 0x%08x\r\n", context_regs->excsave3);
            exception_printf("excsave4  = 0x%08x\r\n", context_regs->excsave4);
            exception_printf("excsave5  = 0x%08x\r\n", context_regs->excsave5);
            exception_printf("excsave6  = 0x%08x\r\n", context_regs->excsave6);
            exception_printf("excsavenmi  = 0x%08x\r\n", context_regs->excsavenmi);
            exception_printf("intenable  = 0x%08x\r\n", context_regs->intenable);
            exception_printf("interrupt  = 0x%08x\r\n", context_regs->interrupt);
            /* Bool option special registers */
            exception_printf("br  = 0x%08x\r\n", context_regs->br);
            /* Coprocessor option special registers */
            exception_printf("cpenable  = 0x%08x\r\n", context_regs->cpenable);
            /* Debug option special registers */
            exception_printf("debugcause  = 0x%08x\r\n", context_regs->debugcause);
            exception_printf("ibreakenable  = 0x%08x\r\n", context_regs->ibreakenable);
            exception_printf("ibreaka0  = 0x%08x\r\n", context_regs->ibreaka0);
            exception_printf("ibreaka1  = 0x%08x\r\n", context_regs->ibreaka1);
            exception_printf("dbreaka0  = 0x%08x\r\n", context_regs->dbreaka0);
            exception_printf("dbreaka1  = 0x%08x\r\n", context_regs->dbreaka1);
            exception_printf("dbreakc0  = 0x%08x\r\n", context_regs->dbreakc0);
            exception_printf("dbreakc1  = 0x%08x\r\n", context_regs->dbreakc1);
            /* DSP engine special registers */
            exception_printf("aep0_L  = 0x%08x\r\n", (uint32_t)(context_regs->aep0 & 0xffffffff));
            exception_printf("aep0_H  = 0x%08x\r\n", (uint32_t)((context_regs->aep0 >> 32) & 0xffffffff));
            exception_printf("aep1_L  = 0x%08x\r\n", (uint32_t)(context_regs->aep1 & 0xffffffff));
            exception_printf("aep1_H  = 0x%08x\r\n", (uint32_t)((context_regs->aep1 >> 32) & 0xffffffff));
            exception_printf("aep2_L  = 0x%08x\r\n", (uint32_t)(context_regs->aep2 & 0xffffffff));
            exception_printf("aep2_H  = 0x%08x\r\n", (uint32_t)((context_regs->aep2 >> 32) & 0xffffffff));
            exception_printf("aep3_L  = 0x%08x\r\n", (uint32_t)(context_regs->aep3 & 0xffffffff));
            exception_printf("aep3_H  = 0x%08x\r\n", (uint32_t)((context_regs->aep3 >> 32) & 0xffffffff));
            exception_printf("aep4_L  = 0x%08x\r\n", (uint32_t)(context_regs->aep4 & 0xffffffff));
            exception_printf("aep4_H  = 0x%08x\r\n", (uint32_t)((context_regs->aep4 >> 32) & 0xffffffff));
            exception_printf("aep5_L  = 0x%08x\r\n", (uint32_t)(context_regs->aep5 & 0xffffffff));
            exception_printf("aep5_H  = 0x%08x\r\n", (uint32_t)((context_regs->aep5 >> 32) & 0xffffffff));
            exception_printf("aep6_L  = 0x%08x\r\n", (uint32_t)(context_regs->aep6 & 0xffffffff));
            exception_printf("aep6_H  = 0x%08x\r\n", (uint32_t)((context_regs->aep6 >> 32) & 0xffffffff));
            exception_printf("aep7_L  = 0x%08x\r\n", (uint32_t)(context_regs->aep7 & 0xffffffff));
            exception_printf("aep7_H  = 0x%08x\r\n", (uint32_t)((context_regs->aep7 >> 32) & 0xffffffff));
            exception_printf("aeq0_L  = 0x%08x\r\n", (uint32_t)(context_regs->aeq0 & 0xffffffff));
            exception_printf("aeq0_H  = 0x%08x\r\n", (uint32_t)((context_regs->aeq0 >> 32) & 0xffffffff));
            exception_printf("aeq1_L  = 0x%08x\r\n", (uint32_t)(context_regs->aeq1 & 0xffffffff));
            exception_printf("aeq1_H  = 0x%08x\r\n", (uint32_t)((context_regs->aeq1 >> 32) & 0xffffffff));
            exception_printf("aeq2_L  = 0x%08x\r\n", (uint32_t)(context_regs->aeq2 & 0xffffffff));
            exception_printf("aeq2_H  = 0x%08x\r\n", (uint32_t)((context_regs->aeq2 >> 32) & 0xffffffff));
            exception_printf("aeq3_L  = 0x%08x\r\n", (uint32_t)(context_regs->aeq3 & 0xffffffff));
            exception_printf("aeq3_H  = 0x%08x\r\n", (uint32_t)((context_regs->aeq3 >> 32) & 0xffffffff));
            exception_printf("ae_ovf_sar  = 0x%08x\r\n", context_regs->ae_ovf_sar);
            exception_printf("ae_bithead  = 0x%08x\r\n", context_regs->ae_bithead);
            exception_printf("ae_ts_fts_bu_bp  = 0x%08x\r\n", context_regs->ae_ts_fts_bu_bp);
            exception_printf("ae_sd_no  = 0x%08x\r\n", context_regs->ae_sd_no);
            exception_printf("ae_cbegin0  = 0x%08x\r\n", context_regs->ae_cbegin0);
            exception_printf("ae_cend0  = 0x%08x\r\n", context_regs->ae_cend0);
            exception_printf("DSP%d Register dump end:\r\n", pESM->corenum);
        }
    }

#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_MINIDUMP) {

        /* feed wdt to keep time for context dump */
        exception_feed_wdt();

        /* dump context */
        if ((minidump_base_address != 0) && (context_regs != NULL)) {

            uint8_t dsp_reg_str[32] = {0};
            uint32_t dsp_reg_value = 0;
            uint32_t i = 0;

            minidump_header_dsp.regions[0].address = *(uint32_t *)((uint32_t)context_regs + context_regs->windowbase * 4 * 4 + 4) - 16;
            minidump_header_dsp.regions[0].size = MINIDUMP_CURRENTSTACK_LENGTH_DSP0;
            if((0xdeadbeef != pESM->swla_start_addr) && (0xdeadbeef != pESM->swla_size)){

                uint32_t swla_log_buff[128] = {};
                snprintf((char *)swla_log_buff, 46, "####SWLA enabled[0x%08x,0x%08x]####\r\n", (unsigned int)pESM->swla_start_addr,(unsigned int)pESM->swla_size);
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)swla_log_buff, strlen((char *)swla_log_buff));
            }

            /* feed wdt to keep time for context dump */
            exception_feed_wdt();
            uint8_t dsp_fault_dump_str[] = {"DSP0 Fault Dump:\n\n"};
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_fault_dump_str, strlen((char *)dsp_fault_dump_str));


            /* DSP0 Register dump begin */
            uint8_t dsp_reg_dump_start[] = {"DSP0 Register dump begin:\n"};
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_dump_start, strlen((char *)dsp_reg_dump_start));

            /* ar0 - ar9*/
            for (i = 0; i < 10; i++) {
                dsp_reg_value = *((uint32_t *)context_regs + i) ;
                snprintf((char *)&dsp_reg_str[0], 17, "ar%d = 0x%08x", (unsigned int)i, (unsigned int)dsp_reg_value);
                dsp_reg_str[16] = '\n';
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, 17);
            }

            /* ar10-ar63 */
            for (i = 10; i < 64; i++) {
                dsp_reg_value =  *((uint32_t *)context_regs + i);
                snprintf((char *)&dsp_reg_str[0], 18, "ar%d = 0x%08x", (unsigned int)i, (unsigned int)dsp_reg_value);
                dsp_reg_str[17] = '\n';
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, 18);
            }

            /* windowbase */
            snprintf((char *)&dsp_reg_str[0], strlen("windowbase = 0x") + 8 + 1, "windowbase = 0x%08x", (unsigned int)context_regs->windowbase);
            dsp_reg_str[strlen("windowbase = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("windowbase = 0x") + 8 + 1);

            /* windowstart */
            snprintf((char *)&dsp_reg_str[0], strlen("windowstart = 0x") + 8 + 1, "windowstart = 0x%08x", (unsigned int)context_regs->windowstart);
            dsp_reg_str[strlen("windowstart = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("windowstart = 0x") + 8 + 1);

            /* lbeg */
            snprintf((char *)&dsp_reg_str[0], strlen("lbeg = 0x") + 8 + 1, "lbeg = 0x%08x", (unsigned int)context_regs->lbeg);
            dsp_reg_str[strlen("lbeg = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("lbeg = 0x") + 8 + 1);

            /* lend */
            snprintf((char *)&dsp_reg_str[0], strlen("lend = 0x") + 8 + 1, "lend = 0x%08x", (unsigned int)context_regs->lend);
            dsp_reg_str[strlen("lend = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("lend = 0x") + 8 + 1);

            /* lcount */
            snprintf((char *)&dsp_reg_str[0], strlen("lcount = 0x") + 8 + 1, "lcount = 0x%08x", (unsigned int)context_regs->lcount);
            dsp_reg_str[strlen("lcount = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("lcount = 0x") + 8 + 1);

            /* sar */
            snprintf((char *)&dsp_reg_str[0], strlen("sar = 0x") + 8 + 1, "sar = 0x%08x", (unsigned int)context_regs->sar);
            dsp_reg_str[strlen("sar = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("sar = 0x") + 8 + 1);

            /* scompare */
            snprintf((char *)&dsp_reg_str[0], strlen("scompare = 0x") + 8 + 1, "scompare = 0x%08x", (unsigned int)context_regs->scompare);
            dsp_reg_str[strlen("scompare = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("scompare = 0x") + 8 + 1);

            /* excause*/
            snprintf((char *)&dsp_reg_str[0], strlen("exccause = 0x") + 8 + 1, "exccause = 0x%08x", (unsigned int)context_regs->exccause);
            dsp_reg_str[strlen("exccause = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(0, &minidump_base_address, dsp_reg_str, strlen("exccause = 0x") + 8 + 1);

            /* excvaddr */
            snprintf((char *)&dsp_reg_str[0], strlen("excvaddr = 0x") + 8 + 1, "excvaddr = 0x%08x", (unsigned int)context_regs->excvaddr);
            dsp_reg_str[strlen("excvaddr = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("excvaddr = 0x") + 8 + 1);

            /* pc */
            snprintf((char *)&dsp_reg_str[0], strlen("pc = 0x") + 8 + 1, "pc = 0x%08x", (unsigned int)context_regs->pc);
            dsp_reg_str[strlen("pc = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("pc = 0x") + 8 + 1);

            /* epc1 - epc6 */
            for (i = 1; i < 7; i++) {
                dsp_reg_value = *((uint32_t *)&context_regs->pc + i);
                snprintf((char *)&dsp_reg_str[0], 18, "epc%d = 0x%08x", (unsigned int)i, (unsigned int)dsp_reg_value);
                dsp_reg_str[17] = '\n';
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, 18);
            }

            /* epcnmi */
            snprintf((char *)&dsp_reg_str[0], strlen("epcnmi = 0x") + 8 + 1, "epcnmi = 0x%08x", (unsigned int)context_regs->epcnmi);
            dsp_reg_str[strlen("epcnmi = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("epcnmi = 0x") + 8 + 1);

            /* depc */
            snprintf((char *)&dsp_reg_str[0], strlen("depc = 0x") + 8 + 1, "depc = 0x%08x", (unsigned int)context_regs->depc);
            dsp_reg_str[strlen("depc = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("depc = 0x") + 8 + 1);

            /* ps */
            snprintf((char *)&dsp_reg_str[0], strlen("ps = 0x") + 8 + 1, "ps = 0x%08x", (unsigned int)context_regs->ps);
            dsp_reg_str[strlen("ps = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("ps = 0x") + 8 + 1);

            /* eps2 - eps6 */
            for (i = 2; i < 7; i++) {
                dsp_reg_value = *((uint32_t *)&context_regs->ps + i - 1);
                snprintf((char *)&dsp_reg_str[0], 18, "eps%d = 0x%08x", (unsigned int)i, (unsigned int)dsp_reg_value);
                dsp_reg_str[17] = '\n';
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, 18);
            }

            /* epsnmi */;
            snprintf((char *)&dsp_reg_str[0], strlen("epsnmi = 0x") + 8 + 1, "epsnmi = 0x%08x", (unsigned int)context_regs->epsnmi);
            dsp_reg_str[19] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, 20);

            /* excsave1 - excsave6 */
            for (i = 1; i < 7; i++) {
                dsp_reg_value = *((uint32_t *)&context_regs->epsnmi + i);
                snprintf((char *)&dsp_reg_str[0], 22, "excsave%d = 0x%08x", (unsigned int)i, (unsigned int)dsp_reg_value);
                dsp_reg_str[21] = '\n';
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, 22);
            }

            /* excsavenmi */
            snprintf((char *)&dsp_reg_str[0], strlen("excsavenmi = 0x") + 8 + 1, "excsavenmi = 0x%08x", (unsigned int)context_regs->excsavenmi);
            dsp_reg_str[strlen("excsavenmi = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("excsavenmi = 0x") + 8 + 1);

            /* intenable */
            snprintf((char *)&dsp_reg_str[0], strlen("intenable = 0x") + 8 + 1, "intenable=0x%08x", (unsigned int)context_regs->intenable);
            dsp_reg_str[strlen("intenable=0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("intenable=0x") + 8 + 1);

            /* interrupt */
            snprintf((char *)&dsp_reg_str[0], strlen("interrupt = 0x") + 8 + 1, "interrupt = 0x%08x", (unsigned int)context_regs->interrupt);
            dsp_reg_str[strlen("interrupt = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("interrupt = 0x") + 8 + 1);

            /* br */
            snprintf((char *)&dsp_reg_str[0], strlen("br = 0x") + 8 + 1, "br = 0x%08x", (unsigned int)context_regs->br);
            dsp_reg_str[strlen("br = 0x") + 8 ] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("br = 0x") + 8 + 1);

            /* cpenable */
            snprintf((char *)&dsp_reg_str[0], strlen("cpenable = 0x") + 8 + 1, "cpenable = 0x%08x", (unsigned int)context_regs->cpenable);
            dsp_reg_str[strlen("cpenable = 0x") + 8 ] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("cpenable = 0x") + 8 + 1);

            /* debugcasue */
            snprintf((char *)&dsp_reg_str[0], strlen("debugcasue = 0x") + 8 + 1, "debugcasue = 0x%08x", (unsigned int)context_regs->debugcause);
            dsp_reg_str[strlen("debugcasue = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("debugcasue = 0x") + 8 + 1);

            /* ibreakenable */
            snprintf((char *)&dsp_reg_str[0], strlen("ibreakenable = 0x") + 8 + 1, "ibreakenable = 0x%08x", (unsigned int)context_regs->ibreakenable);
            dsp_reg_str[strlen("ibreakenable = 0x") + 8 ] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("ibreakenable = 0x") + 8 + 1);

            /* ibreaka0 */
            snprintf((char *)&dsp_reg_str[0], strlen("ibreaka0 = 0x") + 8 + 1, "ibreaka0 = 0x%08x", (unsigned int)context_regs->ibreaka0);
            dsp_reg_str[strlen("ibreaka0 = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("ibreaka0 = 0x") + 8 + 1);

            /* ibreaka1 */
            snprintf((char *)&dsp_reg_str[0], strlen("ibreaka1 = 0x") + 8 + 1, "ibreaka1 = 0x%08x", (unsigned int)context_regs->ibreaka1);
            dsp_reg_str[strlen("ibreaka1 = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("ibreaka1 = 0x") + 8 + 1);

            /* dbreaka0 */
            snprintf((char *)&dsp_reg_str[0], strlen("dbreaka0 = 0x") + 8 + 1, "dbreaka0 = 0x%08x", (unsigned int)context_regs->dbreaka0);
            dsp_reg_str[strlen("dbreaka0 = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("dbreaka0 = 0x") + 8 + 1);

            /* dbreaka1 */
            snprintf((char *)&dsp_reg_str[0], strlen("dbreaka1 = 0x") + 8 + 1, "dbreaka1 = 0x%08x", (unsigned int)context_regs->dbreaka1);
            dsp_reg_str[strlen("dbreaka1 = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("dbreaka1 = 0x") + 8 + 1);

            /* dbreakc0 */
            snprintf((char *)&dsp_reg_str[0], strlen("dbreakc0 = 0x") + 8 + 1, "dbreakc0 = 0x%08x", (unsigned int)context_regs->dbreakc0);
            dsp_reg_str[strlen("dbreakc0 = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("dbreakc0 = 0x") + 8 + 1);

            /* dbreakc1 */
            snprintf((char *)&dsp_reg_str[0], strlen("dbreakc1 = 0x") + 8 + 1, "dbreakc1 = 0x%08x", (unsigned int)context_regs->dbreakc1);
            dsp_reg_str[strlen("dbreakc1 = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("dbreakc1 = 0x") + 8 + 1);

            /* aep0_L - aep7_L */
            for (i = 0; i < 8; i++) {
                dsp_reg_value = (*(uint64_t *)&context_regs->aep0 + i) & 0xffffffff;
                snprintf((char *)&dsp_reg_str[0], 20, "aep%d_L = 0x%08x", (unsigned int)i, (unsigned int)dsp_reg_value);
                dsp_reg_str[19] = '\n';
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, 20);
                dsp_reg_value = (*((uint64_t *)&context_regs->aep0 + i) >> 32) & 0xffffffff;
                snprintf((char *)&dsp_reg_str[0], 20, "aep%d_H = 0x%08x", (unsigned int)i, (unsigned int)dsp_reg_value);
                dsp_reg_str[19] = '\n';
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, 20);
            }

            /* aeq0_L - aeq3_L */
            for (i = 0; i < 4; i++) {
                dsp_reg_value = (*(uint64_t *)&context_regs->aeq0 + i) & 0xffffffff;
                snprintf((char *)&dsp_reg_str[0], 20, "aeq%d_L = 0x%08x", (unsigned int)i, (unsigned int)dsp_reg_value);
                dsp_reg_str[19] = '\n';
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, 20);

                dsp_reg_value = (*((uint64_t *)&context_regs->aeq0 + i)  >> 32) & 0xffffffff;
                snprintf((char *)&dsp_reg_str[0], 20, "aeq%d_H = 0x%08x", (unsigned int)i, (unsigned int)dsp_reg_value);
                dsp_reg_str[19] = '\n';
                offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, 20);
            }

            /* ae_ovf_sar */
            snprintf((char *)&dsp_reg_str[0], strlen("ae_ovf_sar = 0x") + 8 + 1, "ae_ovf_sar = 0x%08x", (unsigned int)context_regs->ae_ovf_sar);
            dsp_reg_str[strlen("ae_ovf_sar = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("ae_ovf_sar = 0x") + 8 + 1);

            /* ae_bithead */
            snprintf((char *)&dsp_reg_str[0], strlen("ae_bithead = 0x") + 8 + 1, "ae_bithead = 0x%08x", (unsigned int)context_regs->ae_bithead);
            dsp_reg_str[strlen("ae_bithead = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(0, &minidump_base_address, dsp_reg_str, strlen("ae_bithead = 0x") + 8 + 1);

            /* ae_ts_fts_bu_bp */
            snprintf((char *)&dsp_reg_str[0], strlen("ae_ts_fts_bu_bp = 0x") + 8 + 1, "ae_ts_fts_bu_bp = 0x%08x", (unsigned int)context_regs->ae_ts_fts_bu_bp);
            dsp_reg_str[strlen("ae_ts_fts_bu_bp = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("ae_ts_fts_bu_bp = 0x") + 8 + 1);

            /* ae_sd_no */
            snprintf((char *)&dsp_reg_str[0], strlen("ae_sd_no = 0x") + 8 + 1, "ae_sd_no = 0x%08x", (unsigned int)context_regs->ae_sd_no);
            dsp_reg_str[strlen("ae_sd_no = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("ae_sd_no = 0x") + 8 + 1);

            /* ae_cbegin0 */
            snprintf((char *)&dsp_reg_str[0], strlen("ae_cbegin0 = 0x") + 8 + 1, "ae_cbegin0 = 0x%08x", (unsigned int)context_regs->ae_cbegin0);
            dsp_reg_str[strlen("ae_cbegin0 = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("ae_cbegin0 = 0x") + 8 + 1);

            /* ae_cend0 */
            snprintf((char *)&dsp_reg_str[0], strlen("ae_cend0 = 0x") + 8 + 1, "ae_cend0 = 0x%08x", (unsigned int)context_regs->ae_cend0);
            dsp_reg_str[strlen("ae_cend0 = 0x") + 8] = '\n';
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_str, strlen("ae_cend0 = 0x") + 8 + 1);

            uint8_t dsp_reg_dump_end[] = {"DSP0 Register dump end:\n"};
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_reg_dump_end, strlen((char *)dsp_reg_dump_end));
            minidump_header_dsp.context_size = EXCEPTION_CONTEXT_WORDS_DSP * 4;
        }
    }

#endif /* EXCEPTION_MEMDUMP_MODE */
}
static uint32_t dsp_region_size = 0;
/******************************************************************************/
/*            Exception's memory dump Functions                               */
/******************************************************************************/
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
static void exception_dsp_dump_region_info(uint32_t num, uint32_t items, memory_region_type *regions_base)
{
    uint32_t i = 0;
    unsigned int *current = 0;
    unsigned int *end = 0;
    memory_region_type *region = NULL;
    char *region_name = NULL;

    for (i = 0; i < items; i++) {
        /* CM4 need to transfer DSP0 address into CM4 view */
        region = (memory_region_type *)hal_memview_dsp0_to_cm4((uint32_t)(regions_base + i));
        region_name = (char *)hal_memview_dsp0_to_cm4((uint32_t)(region->region_name));

        if (region->is_dumped) {
            /* Make sure start address 4-byte align */
            current = (unsigned int *)((uint32_t)(region->start_address) & 0xfffffffc);
            end     = (unsigned int *)(region->end_address);
            dsp_region_size += (uint32_t)end - (uint32_t)current;
            if (current < end) {
                /* feed wdt to keep time for output data */
                exception_feed_wdt();
                /* CM4 need to print address in DSPx view */
                exception_printf("Region-%s: start_address = 0x%x, end_address = 0x%x\r\n", region_name, (unsigned int)current, (unsigned int)end);
            }
        }
    }
}
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT)
static void exception_dsp_dump_region_data_text(uint32_t num, uint32_t items, memory_region_type *regions_base)
{
    unsigned int i = 0;
    unsigned int j = 0;
    memory_region_type *region = NULL;
    unsigned int *current = 0;
    unsigned int *end = 0;
    unsigned int start_address = 0;

    for (i = 0; i < items; i++) {
        /* CM4 need to transfer DSP0 address into CM4 view */
        region = (memory_region_type *)hal_memview_dsp0_to_cm4((uint32_t)(regions_base + i));
        /* Make sure start address 4-byte align */
        current = (unsigned int *)(hal_memview_dsp0_to_cm4((uint32_t)(region->start_address)) & 0xfffffffc);
        start_address = (uint32_t)(region->start_address) & 0xfffffffc;
        end     = (unsigned int *)(hal_memview_dsp0_to_cm4((uint32_t)(region->end_address)));

        if (region->is_dumped) {
            for (j = 0; current < end; current += 4, j++) {
                if (*(current + 0) == 0 &&
                        *(current + 1) == 0 &&
                        *(current + 2) == 0 &&
                        *(current + 3) == 0) {
                    continue;
                }

                /* feed wdt to keep time for output data */
                exception_feed_wdt();
                /* CM4 need to print DSPx address */
                exception_printf("0x%08x: %08x %08x %08x %08x\r\n",
                                 (start_address + j * 0x10),
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
static void exception_dsp_dump_region_data_binary(uint32_t num, uint32_t items, memory_region_type *regions_base)
{
    unsigned int i = 0;
    unsigned int *current = 0;
    unsigned int *end = 0;
    memory_region_type *region = NULL;

    for (i = 0; i < items; i++) {
        /* CM4 need to transfer DSP0 address into CM4 view */
        region = (memory_region_type *)hal_memview_dsp0_to_cm4((uint32_t)(regions_base + i));
        /* Make sure start address 4-byte align */
        current = (unsigned int *)(hal_memview_dsp0_to_cm4((uint32_t)(region->start_address)) & 0xfffffffc);
        end     = (unsigned int *)(hal_memview_dsp0_to_cm4((uint32_t)(region->end_address)));

        if (region->is_dumped) {
            if (current < end) {
                /* feed wdt to keep time for output data */
                exception_feed_wdt();
                /* CM4 output dsp data */
                log_dump_exception_data((const uint8_t *)current, (uint32_t)end - (uint32_t)current);
            }
        }
    }
}
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
#if !((defined(AB156X) || (PRODUCT_VERSION == 2822)))
static exception_status_t exception_dsp_minidump_check_address(uint32_t num, uint32_t items, memory_region_type *regions_base, uint32_t address)
{
    uint32_t i = 0;
    uint32_t start, end;
    memory_region_type *region = NULL;

    for (i = 0; i < items ; i++) {
        /* CM4 need to transfer DSP0 address into CM4 view */
        region = (memory_region_type *)hal_memview_dsp0_to_cm4((uint32_t)(regions_base + i));

        start = (uint32_t)(region->start_address);
        end   = (uint32_t)(region->end_address);

        if ((address >= start) && (address <= end)) {
            return EXCEPTION_STATUS_OK;
        }
    }

    return EXCEPTION_STATUS_ERROR;
}
#endif

static void exception_dsp_dump_region_data_minidump(uint32_t num, uint32_t items, memory_region_type *regions_base)
{
#if ((defined(AB156X) || (PRODUCT_VERSION == 2822)))
    unsigned int i = 0;
    unsigned int *current = 0;
    unsigned int *end = 0;
    int region_size = 0;
    memory_region_type *region = NULL;
    uint8_t region_name[128] = {0};
    int32_t ret = 0;
    /* static regions */
    if (minidump_base_address_dsp != 0) {

        for (i = 0; i < items; i++) {

            /* CM4 need to transfer DSP0 address into CM4 view */
            region = (memory_region_type *)hal_memview_dsp0_to_cm4((uint32_t)(regions_base + i));
            /* Make sure start address 4-byte align */
            current = (unsigned int *)(hal_memview_dsp0_to_cm4((uint32_t)(region->start_address)) & 0xfffffffc);
            end	  = (unsigned int *)(hal_memview_dsp0_to_cm4((uint32_t)(region->end_address)));
            region_size = (int)end - (int)current;

            if (region->is_dumped) {
                if (current < end) {
                    /* remain dump size <=  region size */
                    if (slave0_minidump_region_size < (62 + strlen(region->region_name) + 1)){
                        region_size = 0;
                        slave0_minidump_region_size = 0;
                        snprintf((char *)region_name,50+strlen(region->region_name)+1,"[WARNING]:Region-%s,Actually Size is 0x%08x!!!\r\n",region->region_name,(unsigned int)region_size);
                        ret= offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, region_name, strlen((char *)region_name));
                        if (ret != true) {
                            minidump_base_address = 0;
                            return;
                        }
                        break;
                    }else if((slave0_minidump_region_size - (62 + strlen(region->region_name) + 1)) < region_size ){
                        region_size = slave0_minidump_region_size - (62 + strlen(region->region_name) + 1);
                        /* update end address */
                        region->end_address = (unsigned int *)((uint32_t)region->start_address + region_size);
                    }else{

                    }

                    snprintf((char *)region_name, 62 + strlen(region->region_name) + 1, "Region-%s: start_address = 0x%x, end_address = 0x%x\n",
                             region->region_name, (unsigned int)region->start_address, (unsigned int)region->end_address);

                    offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, region_name, strlen((char *)region_name));

                    /* dump region data */
                    offline_dump_region_write_with_race_header(EXCEPTION_BINARY, &minidump_base_address, (uint8_t *)current, (uint32_t)region_size);

                    /* update slave minidump remain size */
                    slave0_minidump_region_size -= (region_size + 62 + strlen(region->region_name) + 1);

                    /* feed wdt to keep time for output data */
                    exception_feed_wdt();

                     /* check is enough space to do dump */
                    if (slave0_minidump_region_size <= 0){

                        snprintf((char *)region_name,50+strlen(region->region_name)+1,"[WARNING]:Region-%s,Actually Size is 0x%08x!!!\r\n",region->region_name,(unsigned int)region_size);
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

    }
#else
    exception_status_t ret = EXCEPTION_STATUS_ERROR;
    /* static regions */
    if (minidump_base_address_dsp != 0) {
        /* DSP0 Regions Information: */
        offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"DSP0 Regions Information:\n\n", strlen("DSP0 Regions Information:\n\n"));

        uint8_t dsp_region_str[128] = {0};
        snprintf((char *)dsp_region_str, 65, "Region-sp: start_address = 0x%08x, end_address = 0x%08x\n",
                 (unsigned int)hal_memview_dsp0_to_cm4(minidump_header_dsp.regions[0].address),
                 (unsigned int)(hal_memview_dsp0_to_cm4(minidump_header_dsp.regions[0].address) + minidump_header_dsp.regions[0].size));

        offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, dsp_region_str, strlen((char *)dsp_region_str));

        /* dump current stacks */
        ret = exception_dsp_minidump_check_address(EXCEPTION_TEXT, items, regions_base, minidump_header_dsp.regions[0].address);
        if (ret == EXCEPTION_STATUS_OK) {
            offline_dump_region_write_with_race_header(EXCEPTION_BINARY,
                    &minidump_base_address,
                    (uint8_t *)(hal_memview_dsp0_to_cm4(minidump_header_dsp.regions[0].address)),
                    minidump_header_dsp.regions[0].size);
        } else {
            /* dsp stack is not a valid value */
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"DSP SP IS NOT A VALID VALUE\n", strlen("DSP SP IS NOT A VALID VALUE\n"));
        }


        /* DSP0 Regions Data: */
        offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"DSP0 Regions Data:\n\n", strlen("DSP0 Regions Data:\n\n"));

        /* DSP0 memory dump completed. */
        offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"DSP0 memory dump completed.\n\n", strlen("DSP0 memory dump completed.\n\n"));

    }
#endif
}
#endif

static void exception_dsp_dump_memory(int num, exception_sharemem_dsp_t *pESM)
{
    /* Memory Dump */
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
    if ((exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) || (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_BINARY)) {
        /* feed wdt to keep time for memory dump */
        exception_feed_wdt();

        /* Print Regions' information */
        exception_printf("DSP%d Regions Information:\r\n", num);
        /* static regions */
        exception_dsp_dump_region_info(num, pESM->static_regions_items, pESM->static_regions);
        /* dynamic regions */
        exception_dsp_dump_region_info(num, pESM->dynamic_regions_items, pESM->dynamic_regions);

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT)
        if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) {
            /* Print Regions' data */
            /* static regions */
            exception_dsp_dump_region_data_text(num, pESM->static_regions_items, pESM->static_regions);
            /* dynamic regions */
            exception_dsp_dump_region_data_text(num, pESM->dynamic_regions_items, pESM->dynamic_regions);
        }
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
        if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_BINARY) {
            /* Print Memory one by one regions */
            exception_printf("DSP%d Regions Data:\r\n", num);

            /* static regions */
            exception_dsp_dump_region_data_binary(num, pESM->static_regions_items, pESM->static_regions);
            /* dynamic regions */
            exception_dsp_dump_region_data_binary(num, pESM->dynamic_regions_items, pESM->dynamic_regions);
        }
#endif /* EXCEPTION_MEMDUMP_MODE */

        /* dump dsp end log */
        exception_printf("\r\nDSP%d memory dump completed.\r\n", num);
    }
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_MINIDUMP) {
        /* feed wdt to keep time for memory dump */
        exception_feed_wdt();

        /* DSP0 Regions Information: */
        offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"DSP0 Regions Information:\n\n", strlen("DSP0 Regions Information:\n\n"));

        exception_dsp_dump_region_data_minidump(num, pESM->static_regions_items, pESM->static_regions);

        #if (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565)
            exception_dsp_dump_region_data_minidump(num, pESM->dynamic_regions_items, pESM->dynamic_regions);
        #endif

        /* DSP0 Regions Data: */
        offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"DSP0 Regions Data:\n\n", strlen("DSP0 Regions Data:\n\n"));

        /* DSP0 memory dump completed. */
        offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"DSP0 memory dump completed.\n\n", strlen("DSP0 memory dump completed.\n\n"));
    }
#endif /* EXCEPTION_MEMDUMP_MODE */
}

/******************************************************************************/
/*            Exception's dump processor Functions                            */
/******************************************************************************/
exception_status_t exception_dsp_dump_preprocess(int num, exception_sharemem_dsp_t *pESM)
{
#if (EXCEPTION_MEMDUMP_MODE != EXCEPTION_MEMDUMP_NODUMP)
    assert_expr_t *dsp_assert_info = NULL;
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
    if ((exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) || (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_BINARY)) {
        exception_context_dsp_t *context_regs = NULL;

        /* feed wdt to keep time for preprocess */
        exception_feed_wdt();

        /* Check Exception Share Memory data */
        if (exception_dsp_packet_checkout(num, pESM) != EXCEPTION_STATUS_OK) {
            exception_printf("\r\nDSP%d data packet is dirty\r\n", num);
            return EXCEPTION_STATUS_ERROR;
        }

        /* dump dsp swla start address and size */
        if ((pESM->swla_size != 0xdeadbeef) && (pESM->swla_start_addr != 0xdeadbeef)){
            exception_printf("\r\n####SWLA enabled[0x%08x,0x%08x]####\r\n", pESM->swla_start_addr,pESM->swla_size);
        }

        /* dsp dump */
        exception_printf("\r\nDSP%d Fault Dump:\r\n", pESM->corenum);

        /* CM4 need to transfer DSP0 address into CM4 view */
        context_regs = (exception_context_dsp_t *)hal_memview_dsp0_to_cm4((uint32_t)(pESM->context_regs));
        dsp_assert_info  = (assert_expr_t *)hal_memview_dsp0_to_cm4((uint32_t)(pESM->assert_info));

        /* Check if there is a assert */
        if (dsp_assert_info->is_valid == 1) {
            /* CM4 need to transfer DSP0 address into CM4 view */
            exception_printf("DSP%d assert failed: %s, file: %s, line: %d\r\n",
                             pESM->corenum,
                             (const char *)hal_memview_dsp0_to_cm4((uint32_t)(dsp_assert_info->expr)),
                             (const char *)hal_memview_dsp0_to_cm4((uint32_t)(dsp_assert_info->file)),
                             dsp_assert_info->line);
        } else if (dsp_assert_info->is_valid == 2) {
            exception_msgid(EXCEPTION_CPU_ID_SLAVE,
                            dsp_assert_info->string,
                            4,
                            pESM->corenum,
                            dsp_assert_info->expr,
                            dsp_assert_info->file,
                            dsp_assert_info->line);
        }

        /* NMI interrupt */
        if (context_regs->epsnmi != 0x0) {
            exception_printf("\r\nIn NMI Fault Handler\r\n");
        }

        /* Debug exception */
        if (context_regs->debugcause != 0x0) {
            exception_printf("\r\nIn Debug Monitor Fault Handler\r\n");
        }

        /* Info Dump */
        exception_printf("Exception Count = 0x%08x\r\n", pESM->count);
        exception_printf("Exception Time = 0x%08x\r\n", pESM->timestamp);
        exception_printf("Exception Reason = 0x%08x\r\n", context_regs->exccause);
    }
#endif /* EXCEPTION_MEMDUMP_MODE */

#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_MINIDUMP)
    if (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_MINIDUMP) {
        /* feed wdt to keep time for preprocess */
        exception_feed_wdt();

        /* Check Exception Share Memory data */
        if ((exception_dsp_packet_checkout(num, pESM) != EXCEPTION_STATUS_OK) ||
                (minidump_base_address == 0)) {
            offline_dump_region_write_with_race_header(EXCEPTION_TEXT, &minidump_base_address, (uint8_t *)"DSP MEMORY IS DIRTY\n", strlen("DSP MEMORY IS DIRTY\n"));
            /* init dsp minidump base address */
            minidump_base_address_dsp = 0;
            return EXCEPTION_STATUS_ERROR;
        }

        minidump_base_address_dsp = minidump_base_address;
    }
#endif /* EXCEPTION_MEMDUMP_MODE */

    return EXCEPTION_STATUS_OK;
}

void exception_dsp_dump_postprocess(int num, exception_sharemem_dsp_t *pESM)
{
    (void)pESM;
}

/******************************************************************************/
/*            DSP0 Processor Exceptions Handlers                              */
/******************************************************************************/
void exception_alert_dsp0(void)
{
    hal_ccni_set_event(EXCEPTION_EVENT_TO_DSP0, NULL);
}

exception_slave_status_t exception_check_status_dsp0(void)
{
    uint32_t slave_status = HAL_CCNI_EVENT_STATUS_BUSY;

    if (HAL_CCNI_STATUS_OK == hal_ccni_query_event_status(EXCEPTION_EVENT_TO_DSP0, &slave_status)) {
        if (slave_status == HAL_CCNI_EVENT_STATUS_IDLE) {
            /* DSP0 is ready */
            return EXCEPTION_SLAVE_STATUS_READY;
        } else {
            /* DSP0 is not ready */
            return EXCEPTION_SLAVE_STATUS_IDLE;
        }
    }

    return EXCEPTION_SLAVE_STATUS_IDLE;
}

void exception_dsp_dump_heap_status(exception_sharemem_dsp_t *pESM)
{
#if (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_TEXT) || (EXCEPTION_MEMDUMP_MODE & EXCEPTION_MEMDUMP_BINARY)
    if ((exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_TEXT) || (exception_config_mode.exception_mode & EXCEPTION_MEMDUMP_BINARY)) {

        MultiPoolHeaderLink_t *pucHeapPoolBuffer = (MultiPoolHeaderLink_t *)pESM->heap_start_addr;
        if (NULL == pucHeapPoolBuffer){
            exception_printf("[Warning:]pucHeapPoolBuffer is NULL!!!\r\n");
            return;
        }

        pucHeapPoolBuffer = (MultiPoolHeaderLink_t *)(hal_memview_dsp0_to_cm4((uint32_t)pucHeapPoolBuffer));

        dsp_BlockLink_t *blk_iter = pucHeapPoolBuffer->pxFirstBlock;
        if (NULL == blk_iter){
            exception_printf("[Warning:]blk_iter is NULL!!!\r\n");
            return;
        }

        int32_t blk_size = 0;

        exception_printf("dsp heap block dump begin \r\n");

        while (blk_iter != pucHeapPoolBuffer->pxEnd) {
            /* DSP don't support unalign access, so must check the alignment before access. */
            if (((uint32_t)blk_iter & 0x3) != 0) {
                exception_printf("heap crash!!!!!");
                return;
            }
            /* #ifdef MTK_HEAP_SIZE_GUARD_ENABLE */
            if (pESM->heap_size_guard_enable == TRUE) {
#define GET_ACTUAL_BLOCK_SIZE(xBlockSize)  ((xBlockSize) & (uint32_t)0x00FFFFFF)	/* mask off top byte*/
                blk_size = GET_ACTUAL_BLOCK_SIZE(blk_iter->xBlockSize);
                exception_printf("block start = 0x%x,	 size = 0x%-8x\n",	(unsigned int)blk_iter, (unsigned int)blk_iter->xBlockSize);
            } else {
                blk_size = blk_iter->xBlockSize & ~0x80000000;
                exception_printf("block start = 0x%x,    pxNext=0x%-8x,    size = 0x%-8x,    lr = 0x%x\n",
                                 (unsigned int)blk_iter,
                                 (unsigned int)blk_iter->pxNextFreeBlock,
                                 (unsigned int)blk_iter->xBlockSize,
                                 (blk_iter->xBlockSize & 0x80000000) ? (unsigned int)(blk_iter->xMallocLinkRegAddr) : (unsigned int)(blk_iter->xFreeLinkRegAddr));
            } /* MTK_HEAP_SIZE_GUARD_ENABLE */
            blk_iter = (dsp_BlockLink_t *)((uint32_t)blk_iter + blk_size);

            if (((uint32_t)blk_iter > (uint32_t)pucHeapPoolBuffer->pxEnd) || ((uint32_t)blk_iter < (uint32_t)pucHeapPoolBuffer) || (blk_size <= 0)) {
                exception_printf("heap crash!!!!!\r\n");
                return;
            }
        }

        /* #ifdef MTK_HEAP_SIZE_GUARD_ENABLE */
        if (pESM->heap_size_guard_enable == TRUE) {

            exception_printf("block start = 0x%x,	  size = 0x%x", (unsigned int)blk_iter, (unsigned int)blk_iter->xBlockSize);
        } else {
            exception_printf("block start = 0x%x,    pxNext=0x%-8x,    size = 0x%-8x,    lr = 0x%x \n",
                             (unsigned int)blk_iter,
                             (unsigned int)blk_iter->pxNextFreeBlock,
                             (unsigned int)blk_iter->xBlockSize,
                             (blk_iter->xBlockSize & 0x80000000) ? (unsigned int)(blk_iter->xMallocLinkRegAddr) : (unsigned int)(blk_iter->xFreeLinkRegAddr));
        } /* MTK_HEAP_SIZE_GUARD_ENABLE */

        exception_printf("dsp reach blk_end\r\n");
    }
#endif
}



void exception_dump_dsp0(void)
{
    exception_sharemem_dsp_t *pESM = (exception_sharemem_dsp_t *)EXCEPTION_SHARE_MEMORY_ADDRESS_DSP0;

    /* dump exception time, fault type, etc */
    if (exception_dsp_dump_preprocess(0, pESM) != EXCEPTION_STATUS_OK) {
        exception_printf("data dirty or minidump same!!!\r\n");
        // return;
    }


    /* dump dsp context, such as core registers */
    exception_dsp_dump_context(0, pESM);

    /* dump dsp memory*/
    exception_dsp_dump_memory(0, pESM);

    /* heap_start_addr = 0xdeadbeef,dsp side does not define "MTK_SUPPORT_HEAP_DEBUG"
     * heap_size_guard_enable = FALSE,dsp side does not define "MTK_HEAP_SIZE_GUARD_ENABLE"
     */
    if ((pESM->heap_start_addr != 0xdeadbeef) || (pESM->heap_size_guard_enable != FALSE)) {
        /* dump dsp heap */
        exception_dsp_dump_heap_status(pESM);
    }

    /* finish the dump */
    exception_dsp_dump_postprocess(0, pESM);

}

void exception_forceddump_dsp0(void)
{
    if (hal_core_status_read(HAL_CORE_DSP0) > HAL_CORE_OFF) {
        exception_enable_wdt_interrupt();
    }
}
