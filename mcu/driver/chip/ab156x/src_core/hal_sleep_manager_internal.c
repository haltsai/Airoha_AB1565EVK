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

#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include <stdio.h>
#include <string.h>
#include "hal_log.h"
#include "memory_attribute.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"

#ifndef __UBL__
#include "assert.h"
#else
#define assert(expr) log_hal_msgid_error("assert\r\n", 0)
#endif

#include "hal_emi_internal.h"
#include "hal_cache_internal.h"
#include "hal_flash_sf.h"
#include "hal_clock_internal.h"
#include "hal_gpt.h"
#include "hal_mpu_internal.h"
#include "hal_clock.h"

#ifndef log_debug
#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
#define log_debug(_message,...) printf(_message, ##__VA_ARGS__)
#else
#define log_debug(_message,...)
#endif
#endif

#define SKIP_UART_BACKUP_RESTORE 0

log_create_module(SLEEP_MANAGER, PRINT_LEVEL_INFO);
#define SLEEP_MANAGER_LOG_I(fmt,cnt,arg...)          LOG_I(SLEEP_MANAGER,fmt,cnt,##arg)
#define SLEEP_MANAGER_LOG_W(fmt,cnt,arg...)          LOG_W(SLEEP_MANAGER,fmt,cnt,##arg)
#define SLEEP_MANAGER_LOG_E(fmt,cnt,arg...)          LOG_E(SLEEP_MANAGER,fmt,cnt,##arg)
#define SLEEP_MANAGER_MSGID_I(fmt,cnt,arg...)        LOG_MSGID_I(SLEEP_MANAGER,fmt,cnt,##arg)
#define SLEEP_MANAGER_MSGID_W(fmt,cnt,arg...)        LOG_MSGID_W(SLEEP_MANAGER,fmt,cnt,##arg)
#define SLEEP_MANAGER_MSGID_E(fmt,cnt,arg...)        LOG_MSGID_E(SLEEP_MANAGER,fmt,cnt,##arg)

static sleep_management_handle_t sleep_management_handle = {
    .lock_sleep_request = 0,
    .user_handle_resoure = 0,
    .user_handle_count = 0
};

static sleep_management_suspend_callback_func_t    suspend_callback_func_table      [SLEEP_BACKUP_RESTORE_MODULE_MAX];
static sleep_management_resume_callback_func_t     resume_callback_func_table       [SLEEP_BACKUP_RESTORE_MODULE_MAX];
static sleep_management_suspend_callback_func_t    suspend_user_callback_func_table [SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX];
static sleep_management_resume_callback_func_t     resume_user_callback_func_table  [SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX];
static uint32_t suspend_user_register_count = 0, resume_user_register_count = 0;

ATTR_ZIDATA_IN_TCM volatile uint32_t wakeup_source_status;
ATTR_ZIDATA_IN_TCM volatile uint32_t Vector0_backup, temp_reg;
ATTR_ZIDATA_IN_TCM volatile uint32_t origin_msp_bak_reg, origin_psp_bak_reg, backup_return_address;
ATTR_ZIDATA_IN_TCM volatile CPU_CORE_BAKEUP_REG_T  cpu_core_reg ;
ATTR_ZIDATA_IN_TCM volatile nvic_sleep_backup_register_t nvic_backup_register;
ATTR_ZIDATA_IN_TCM volatile FPU_BAKEUP_REG_T  fpu_reg;
ATTR_ZIDATA_IN_TCM volatile CMSYS_CFG_BAKEUP_REG_T  cmsys_cfg_reg;
ATTR_ZIDATA_IN_TCM volatile CMSYS_CFG_EXT_BAKEUP_REG_T cmsys_cfg_ext_reg;
ATTR_ZIDATA_IN_TCM volatile CM4_SYS_CTRL_BAKEUP_REG_T  cm4_sys_ctrl_reg;
sleep_management_status_t sleep_management_status = {0,0,0};

#if     defined (__GNUC__)      //GCC disable compiler optimize
__attribute__((optimize("O0")))
#elif   defined (__ICCARM__)    //IAR disable compiler optimize
#pragma optimize=none
#elif   defined (__CC_ARM)      //MDK disable compiler optimize
#pragma push
#pragma diag_suppress 1267
#pragma O0
#endif
ATTR_TEXT_IN_TCM void sleep_management_enter_deep_sleep(hal_sleep_mode_t mode)
{
    static uint32_t abort_rg;
#if defined (__CC_ARM)
    /* Backup function return address(R14) */
    __asm volatile("mov backup_return_address,__return_address() \n");
#endif

    /* Set CM4 SLEEPDEEP bits */
    *CM4_SYSTEM_CONTROL = *CM4_SYSTEM_CONTROL | 0x4;

    SPM_SET_CM4_SW_STATE(0x3);

    /* Set Boot Slave */
    MCU_CFG_PRI->CMCFG_BOOT_FROM_SLV = 0x1;

    /* Peripheral driver backup callback function */
    sleep_management_suspend_callback();

    SPM_SET_CM4_SW_STATE(0x4);

    /* Backup CMSYS register */
    deep_sleep_cmsys_backup();

    /* Enable SPM IRQ and clear pending bits */
    hal_nvic_enable_irq(SPM_IRQn);
    hal_nvic_clear_pending_irq(SPM_IRQn);

    /* General register backup */
    __CPU_STACK_POINT_BACKUP(origin_psp_bak_reg, origin_msp_bak_reg);

    /* Backup BootVector0 Stack Address */
    Vector0_backup = MCU_CFG_PRI->CMCFG_BOOT_VECTOR0;   //boot vector 0(boot slave stack point)

    /* Backup MSP Address */
#if (defined (__GNUC__) || defined (__ICCARM__))
    __asm volatile("push {r0-r12, lr}");
    __asm volatile("mov %0, sp" : "=r"(temp_reg));
#elif defined (__CC_ARM)
    __PUSH_CPU_REG();
    __BACKUP_SP(temp_reg);
#endif
    MCU_CFG_PRI->CMCFG_BOOT_VECTOR0 = temp_reg;  //VECTOR0 write MSP Address

    if ((*SPM_CM4_WAKEUP_SOURCE_STA&0x10000) == 0) {
        sleep_management_status.abort_sleep = 1;
        SPM_SET_CM4_SW_STATE(0x9);
    } else {
        /*
        *Need to pay special attention to two points,
        1. Cache tag memory and data memory (TCM) can be maintained during core in and out of sleep.
           But for safety, flush all cache line is required before entering sleep
        2, The cache controller needs to re-set the register value immediately after exiting sleep,
           so that it can correctly fetch instructions from the relevant address.
        */
        *((volatile uint32_t *)0xE0180004)  = 0x13;       // flush all cache line
        *((volatile uint32_t *)0xE0180000)  = 0x20C;      // disable cache controller

        SPM_SET_CM4_SW_STATE(0x5);
#ifdef MTK_LOCK_S1
        *SPM_SPM_STATE_CONTROL_0 |= 0x1;
#endif
        /* Enter Deep Sleep */
        temp_reg = (uint32_t)&MCU_CFG_PRI->CMCFG_BOOT_VECTOR1;  //CMCFG_BOOT_VECTOR1 Address
        __ENTER_DEEP_SLEEP(temp_reg);

        /* CMSYS Peripheral : make virtual space available */
        *((volatile uint32_t *)0xE0181000) = 0x10000023;
        *((volatile uint32_t *)0xE0181004) = 0x0;
        /*SYSRAM remap */
        *((volatile uint32_t *)0xE0181008) = 0x14200015;
        *((volatile uint32_t *)0xE018100C) = 0x04200000;

        *((volatile uint32_t *)0xE0190000)  = g_cache_entry[0].cache_entry_n.w;      //region 0 start
        *((volatile uint32_t *)0xE0190040)  = g_cache_entry[0].cache_end_entry_n.w;  //region 0 end
        *((volatile uint32_t *)0xE0190004)  = g_cache_entry[1].cache_entry_n.w;      //region 1 start
        *((volatile uint32_t *)0xE0190044)  = g_cache_entry[1].cache_end_entry_n.w;  //region 1 end
        *((volatile uint32_t *)0xE0180004)  = 0x1;                   // enable cache
        *((volatile uint32_t *)0xE018002C)  = g_cache_region_en;     // enable cache region
        *((volatile uint32_t *)0xE0180000)  = 0x20D;                 // enable cache controller
        /* POP CPU Reg R0-R12 */
        __POP_CPU_REG();

        SPM_SET_CM4_SW_STATE(0x6);

        //Check Abort case     // CM4_ABORT_FLAG           = 0xA2110C80
        abort_rg = *((volatile uint32_t *)0xA2110C80);
        if((abort_rg & 0x100) == 0x100){
            *((volatile uint32_t *)0xA2110C80) = abort_rg & 0xFFFFFEFF; /* Clear Abort flag */
            sleep_management_status.abort_sleep = 2;
            *SPM_PCM_SW_INT_CLEAR = 1;
            *SPM_PCM_SW_INT_CLEAR = 0;
            SPM_SET_CM4_SW_STATE(0xA);
        }
    }
    hal_gpt_delay_us(50);

    /* Get wakeup source */
    sleep_management_status.wakeup_source = *SPM_CM4_WAKEUP_SOURCE_STA;

    /* Restore MSP */
    temp_reg = (uint32_t)&origin_msp_bak_reg;
    __MSP_RESTORE(temp_reg);

    /* Switch stack point to PSP */
    __SWITCH_TO_PSP_STACK_POINT();

    /* Restore PSP */
    temp_reg = (uint32_t)&origin_psp_bak_reg;
    __PSP_RESTORE(temp_reg);

    /* Restore Core register - CONTROL */
    temp_reg = (uint32_t)&cpu_core_reg.CONTROL;
    __CPU_CORE_CONTROL_REG_RESTORE(temp_reg);

    /* Restore boot Vector */
    MCU_CFG_PRI->CMCFG_BOOT_FROM_SLV = 0x0;
    MCU_CFG_PRI->CMCFG_BOOT_VECTOR0 = Vector0_backup ;

    /* Restore CMSYS register */
    deep_sleep_cmsys_restore();

    /* Clear CM4 Deep Sleep bits */
    *CM4_SYSTEM_CONTROL = *CM4_SYSTEM_CONTROL & (~0x4);

    /* Disable SPM IRQ and clear pending bits */
    *SPM_PCM_SW_INT_CLEAR = 1;
    *SPM_PCM_SW_INT_CLEAR = 0;
    hal_nvic_disable_irq(SPM_IRQn);
    hal_nvic_clear_pending_irq(SPM_IRQn);

    SPM_SET_CM4_SW_STATE(0x7);

    /* Peripheral driver restore callback function */
    sleep_management_resume_callback();

    //log_hal_msgid_info("source:0x%x", 1, sleep_management_status.wakeup_source);
    SPM_SET_CM4_SW_STATE(0x8);

#if defined (__CC_ARM)
    __RESTORE_LR(backup_return_address);
#endif
}
#if     defined (__GNUC__)
#elif   defined (__CC_ARM)
#pragma pop
#endif

ATTR_TEXT_IN_TCM inline void deep_sleep_cmsys_backup(void)
{
    uint32_t i;
    /* backup CPU core registers */
    temp_reg = (unsigned int)&cpu_core_reg;
    __CPU_CORE_REG_BACKUP(temp_reg);

    /* NVIC backup */
    nvic_backup_register.nvic_iser = NVIC->ISER[0];
    nvic_backup_register.nvic_iser1 = NVIC->ISER[1];
    for (i = 0; i < SAVE_PRIORITY_GROUP; i++) {
        nvic_backup_register.nvic_ip[i] = NVIC->IP[i];
        /* Disable All NVIC IRQ before enter sleep */
        hal_nvic_disable_irq(i);
    }

    /* cache backcp */
#ifdef HAL_CACHE_MODULE_ENABLED
    //cache_status_save();
#endif

    /* mpu backcp */
#ifdef HAL_MPU_MODULE_ENABLED
    mpu_status_save();
#endif

    /* cmsys config backup */
    cmsys_cfg_reg.STCALIB = CMSYS_CFG->STCALIB;
    cmsys_cfg_reg.AHB_BUFFERALBE = CMSYS_CFG->AHB_BUFFERALBE;
    cmsys_cfg_reg.AHB_FIFO_TH = CMSYS_CFG->AHB_FIFO_TH;
    cmsys_cfg_reg.INT_ACTIVE_HL0 = CMSYS_CFG->INT_ACTIVE_HL0;
    cmsys_cfg_reg.INT_ACTIVE_HL1 = CMSYS_CFG->INT_ACTIVE_HL1;
    cmsys_cfg_reg.DCM_CTRL_REG = CMSYS_CFG->DCM_CTRL_REG;

    cmsys_cfg_ext_reg.CG_EN = CMSYS_CFG_EXT->CG_EN;
    cmsys_cfg_ext_reg.DCM_EN = CMSYS_CFG_EXT->DCM_EN;

    /* fpu backup */
    fpu_reg.FPCCR = FPU->FPCCR;
    fpu_reg.FPCAR = FPU->FPCAR;

    /* CM4 system control registers backup */
    cm4_sys_ctrl_reg.ACTLR = SCnSCB->ACTLR;
    cm4_sys_ctrl_reg.VTOR = SCB->VTOR;
    cm4_sys_ctrl_reg.SCR = SCB->SCR;
    cm4_sys_ctrl_reg.CCR = SCB->CCR;

    cm4_sys_ctrl_reg.SHP[0] = SCB->SHP[0]; /* MemMange */
    cm4_sys_ctrl_reg.SHP[1] = SCB->SHP[1]; /* BusFault */
    cm4_sys_ctrl_reg.SHP[2] = SCB->SHP[2]; /* UsageFault */
    cm4_sys_ctrl_reg.SHP[7] = SCB->SHP[7]; /* SVCall */
    cm4_sys_ctrl_reg.SHP[8] = SCB->SHP[8]; /* DebugMonitor */
    cm4_sys_ctrl_reg.SHP[10] = SCB->SHP[10]; /* PendSV */
    cm4_sys_ctrl_reg.SHP[11] = SCB->SHP[11]; /* SysTick */

    cm4_sys_ctrl_reg.SHCSR = SCB->SHCSR;
    cm4_sys_ctrl_reg.CPACR = SCB->CPACR;

    /* CM4 DWT register backup */
    cm4_sys_ctrl_reg.DHCSR = CoreDebug->DHCSR;
    cm4_sys_ctrl_reg.DEMCR = CoreDebug->DEMCR;
    cm4_sys_ctrl_reg.COMP0 = DWT->COMP0;
    cm4_sys_ctrl_reg.MASK0 = DWT->MASK0;
    cm4_sys_ctrl_reg.FUNCTION0 = DWT->FUNCTION0;
    cm4_sys_ctrl_reg.COMP1 = DWT->COMP1;
    cm4_sys_ctrl_reg.MASK1 = DWT->MASK1;
    cm4_sys_ctrl_reg.FUNCTION1 = DWT->FUNCTION1;
    cm4_sys_ctrl_reg.COMP2 = DWT->COMP2;
    cm4_sys_ctrl_reg.MASK2 = DWT->MASK2;
    cm4_sys_ctrl_reg.FUNCTION2 = DWT->FUNCTION2;
    cm4_sys_ctrl_reg.COMP3 = DWT->COMP3;
    cm4_sys_ctrl_reg.MASK3 = DWT->MASK3;
    cm4_sys_ctrl_reg.FUNCTION3 = DWT->FUNCTION3;
}

ATTR_TEXT_IN_TCM inline void deep_sleep_cmsys_restore(void)
{
    uint32_t i;

    /* CM4 system control registers restore */
    SCnSCB->ACTLR = cm4_sys_ctrl_reg.ACTLR;
    SCB->VTOR = cm4_sys_ctrl_reg.VTOR;
    SCB->SCR = cm4_sys_ctrl_reg.SCR;
    SCB->CCR = cm4_sys_ctrl_reg.CCR;
    SCB->SHP[0] = cm4_sys_ctrl_reg.SHP[0]; /* MemMange */
    SCB->SHP[1] = cm4_sys_ctrl_reg.SHP[1]; /* BusFault */
    SCB->SHP[2] = cm4_sys_ctrl_reg.SHP[2]; /* UsageFault */
    SCB->SHP[7] = cm4_sys_ctrl_reg.SHP[7]; /* SVCall */
    SCB->SHP[8] = cm4_sys_ctrl_reg.SHP[8]; /* DebugMonitor */
    SCB->SHP[10] = cm4_sys_ctrl_reg.SHP[10]; /* PendSV */
    SCB->SHP[11] = cm4_sys_ctrl_reg.SHP[11]; /* SysTick */
    SCB->SHCSR = cm4_sys_ctrl_reg.SHCSR;
    SCB->CPACR = cm4_sys_ctrl_reg.CPACR;

    /* CM4 DWT register backup */
    CoreDebug->DHCSR = cm4_sys_ctrl_reg.DHCSR;
    CoreDebug->DEMCR = cm4_sys_ctrl_reg.DEMCR;
    DWT->COMP0 = cm4_sys_ctrl_reg.COMP0;
    DWT->MASK0 = cm4_sys_ctrl_reg.MASK0;
    DWT->FUNCTION0 = cm4_sys_ctrl_reg.FUNCTION0;
    DWT->COMP1 = cm4_sys_ctrl_reg.COMP1;
    DWT->MASK1 = cm4_sys_ctrl_reg.MASK1;
    DWT->FUNCTION1 = cm4_sys_ctrl_reg.FUNCTION1;
    DWT->COMP2 = cm4_sys_ctrl_reg.COMP2;
    DWT->MASK2 = cm4_sys_ctrl_reg.MASK2;
    DWT->FUNCTION2 = cm4_sys_ctrl_reg.FUNCTION2;
    DWT->COMP3 = cm4_sys_ctrl_reg.COMP3;
    DWT->MASK3 = cm4_sys_ctrl_reg.MASK3;
    DWT->FUNCTION3 = cm4_sys_ctrl_reg.FUNCTION3;

    /* fpu restore */
    FPU->FPCCR = fpu_reg.FPCCR;
    FPU->FPCAR = fpu_reg.FPCAR;

    /* cmsys config restore */
    CMSYS_CFG->STCALIB = cmsys_cfg_reg.STCALIB;
    CMSYS_CFG->AHB_BUFFERALBE = cmsys_cfg_reg.AHB_BUFFERALBE;
    CMSYS_CFG->AHB_FIFO_TH = cmsys_cfg_reg.AHB_FIFO_TH;
    CMSYS_CFG->INT_ACTIVE_HL0 = cmsys_cfg_reg.INT_ACTIVE_HL0;
    CMSYS_CFG->INT_ACTIVE_HL1 = cmsys_cfg_reg.INT_ACTIVE_HL1;
    CMSYS_CFG->DCM_CTRL_REG = cmsys_cfg_reg.DCM_CTRL_REG;

    CMSYS_CFG_EXT->CG_EN = cmsys_cfg_ext_reg.CG_EN;
    CMSYS_CFG_EXT->DCM_EN = cmsys_cfg_ext_reg.DCM_EN;

    /* mpu restore */
#ifdef HAL_MPU_MODULE_ENABLED
    mpu_status_restore();
#endif

    /* cache restore */
#ifdef HAL_CACHE_MODULE_ENABLED
    //cache_status_restore();
#endif

    /* restore CPU core registers */
    temp_reg = (unsigned int)&cpu_core_reg;
    __CPU_CORE_REG_RESTORE(temp_reg);

    /* NVIC restore */
    NVIC->ISER[0] = nvic_backup_register.nvic_iser;
    NVIC->ISER[1] = nvic_backup_register.nvic_iser1;
    for (i = 0; i < SAVE_PRIORITY_GROUP; i++) {
        NVIC->IP[i] = nvic_backup_register.nvic_ip[i];
    }
}

void sleep_management_register_suspend_callback(sleep_management_backup_restore_module_t module , sleep_management_suspend_callback_t callback , void *data)
{
    if (module == SLEEP_BACKUP_RESTORE_USER) {
        if (suspend_user_register_count < SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX) {
            suspend_user_callback_func_table[suspend_user_register_count].func        = callback;
            suspend_user_callback_func_table[suspend_user_register_count].data        = data;
            suspend_user_callback_func_table[suspend_user_register_count].init_status = SLEEP_MANAGEMENT_INITIALIZED;
            suspend_user_register_count++;
        } else {
            log_hal_msgid_error("[Sleep Management]register suspend callback function overflow\r\n", 0);
            assert(0);
        }
    } else {
        suspend_callback_func_table[module].func        = callback;
        suspend_callback_func_table[module].data        = data;
        suspend_callback_func_table[module].init_status = SLEEP_MANAGEMENT_INITIALIZED;
    }
}

void sleep_management_register_resume_callback(sleep_management_backup_restore_module_t module , sleep_management_resume_callback_t callback , void *data)
{
    if (module == SLEEP_BACKUP_RESTORE_USER) {
        if (resume_user_register_count < SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX) {
            resume_user_callback_func_table[resume_user_register_count].func        = callback;
            resume_user_callback_func_table[resume_user_register_count].data        = data;
            resume_user_callback_func_table[resume_user_register_count].init_status = SLEEP_MANAGEMENT_INITIALIZED;
            resume_user_register_count++;
        } else {
            log_hal_msgid_error("[Sleep Management]register resume callback function overflow\r\n", 0);
            assert(0);
        }
    } else {
        resume_callback_func_table[module].func = callback;
        resume_callback_func_table[module].data = data;
        resume_callback_func_table[module].init_status = SLEEP_MANAGEMENT_INITIALIZED;
    }
}

ATTR_TEXT_IN_TCM void sleep_management_perisys_suspend_callback()
{
    static uint32_t i;
    for (i = 0; i < SLEEP_BACKUP_RESTORE_PERISYS_MAX; i++) {
        if (suspend_callback_func_table[i].init_status == SLEEP_MANAGEMENT_INITIALIZED) {

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(0, 0, i);
#endif
            suspend_callback_func_table[i].func(suspend_callback_func_table[i].data);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(0, 1, i);
#endif
        }
    }
}

ATTR_TEXT_IN_TCM void sleep_management_perisys_resume_callback()
{
    static uint32_t i;
    for (i = 0; i < SLEEP_BACKUP_RESTORE_PERISYS_MAX; i++) {
        if (resume_callback_func_table[i].init_status == SLEEP_MANAGEMENT_INITIALIZED) {
#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(1, 0, i);
#endif

            resume_callback_func_table[i].func(resume_callback_func_table[i].data);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
        sleep_management_debug_backup_restore_fun_timelog(1, 1, i);
#endif
        }
    }
}

ATTR_TEXT_IN_TCM void sleep_management_suspend_callback()
{
    static uint32_t i;

    for (i = 0; i < SLEEP_BACKUP_RESTORE_MODULE_MAX; i++) {
        if (suspend_callback_func_table[i].init_status == SLEEP_MANAGEMENT_INITIALIZED) {

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(0, 0, i);
#endif

        #if SKIP_UART_BACKUP_RESTORE == 1
            if(i != SLEEP_BACKUP_RESTORE_UART){
                suspend_callback_func_table[i].func(suspend_callback_func_table[i].data);
            }
        #else
            suspend_callback_func_table[i].func(suspend_callback_func_table[i].data);
        #endif

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(0, 1, i);
#endif
        }
    }

    for (i = 0; i < SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX; i++) {
        if (suspend_user_callback_func_table[i].init_status == SLEEP_MANAGEMENT_INITIALIZED) {

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(1, 0, i + SLEEP_BACKUP_RESTORE_MODULE_MAX);
#endif

            suspend_user_callback_func_table[i].func(suspend_user_callback_func_table[i].data);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(1, 1, i + SLEEP_BACKUP_RESTORE_MODULE_MAX);
#endif
        }
    }
}


ATTR_TEXT_IN_TCM void sleep_management_resume_callback()
{
    static uint32_t i;

    for (i = 0; i < SLEEP_BACKUP_RESTORE_MODULE_MAX; i++) {
        if (resume_callback_func_table[i].init_status == SLEEP_MANAGEMENT_INITIALIZED) {

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(1, 0, i);
#endif

        #if SKIP_UART_BACKUP_RESTORE == 1
            if(i != SLEEP_BACKUP_RESTORE_UART){
                resume_callback_func_table[i].func(resume_callback_func_table[i].data);
            }
        #else
            resume_callback_func_table[i].func(resume_callback_func_table[i].data);
        #endif

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(1, 1, i);
#endif
        }
    }

    for (i = 0; i < SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX; i++) {
        if (resume_user_callback_func_table[i].init_status == SLEEP_MANAGEMENT_INITIALIZED) {

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(1, 0, i + SLEEP_BACKUP_RESTORE_MODULE_MAX);
#endif

            resume_user_callback_func_table[i].func(resume_user_callback_func_table[i].data);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_backup_restore_fun_timelog(1, 1, i + SLEEP_BACKUP_RESTORE_MODULE_MAX);
#endif
        }
    }
}

uint8_t sleep_management_get_lock_handle(const char *handle_name)
{
    uint8_t index = 0, i;
    uint32_t mask, name_len;

    for (index = 0 ; index < SLEEP_LOCK_HANDLE_USER_MAX; index++) {
        hal_nvic_save_and_set_interrupt_mask(&mask);

        if (((sleep_management_handle.user_handle_resoure >> index) & 0x01) == 0) {

            sleep_management_handle.user_handle_resoure |= (1 << index);
            sleep_management_handle.user_handle_count++;

            hal_nvic_restore_interrupt_mask(mask);

            memset(&sleep_management_handle.user_handle_name[index][0], 0, SLEEP_HANDLE_NAME_LEN);
            name_len = strlen(handle_name);

            if (name_len >= SLEEP_HANDLE_NAME_LEN) {
                name_len = SLEEP_HANDLE_NAME_LEN - 1;
            }
            memcpy(&sleep_management_handle.user_handle_name[index][0], handle_name, name_len);

            /* Check handle name */
            if (name_len == 0) {
                log_hal_msgid_error("[Sleep Management]sleep handle name error\r\n", 0);
                assert(0);
            }
            for (i = 0; i < name_len; i++) {
                if ((sleep_management_handle.user_handle_name[index][i] <= 0x20) || (sleep_management_handle.user_handle_name[index][i] >= 0x7E)) {
                    log_hal_msgid_error("[Sleep Management]sleep handle name error\r\n", 0);
                    assert(0);
                }
            }
            break;
        }else {
            hal_nvic_restore_interrupt_mask(mask);
        }
    }

    //log_hal_msgid_info("[Sleep Management]sleep handle name : %s\r\n", 1, &sleep_management_handle.user_handle_name[index][0]);
    if (index >= SLEEP_LOCK_HANDLE_USER_MAX) {
        log_hal_msgid_error("[Sleep Management]cannot get sleep handle\r\n", 0);
        assert(0);
        return (SLEEP_LOCK_INVALID_ID);
    }

    index += SLEEP_LOCK_USER_START_ID;

    return (index);
}

void sleep_management_release_lock_handle(uint8_t handle_index)
{
    uint32_t mask;
    uint64_t shift;

    /*  check handle index range */
    if ((handle_index >= SLEEP_LOCK_HANDLE_MAX) || (handle_index < SLEEP_LOCK_USER_START_ID)) {
        log_hal_msgid_error("[Sleep Management]sleep handle index error\r\n", 0);
        return;
    }

    handle_index -= SLEEP_LOCK_USER_START_ID;

    if (handle_index >= SLEEP_LOCK_HANDLE_USER_MAX)
    {
        log_hal_msgid_error("[Sleep Management]sleep handle index error\r\n", 0);
        return;
    }

    shift = (uint64_t)handle_index;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (((sleep_management_handle.user_handle_resoure >> shift) & 0x01) == 1) {
        sleep_management_handle.user_handle_count--;
        sleep_management_handle.user_handle_resoure &= ~(1 << shift);
        memset(&sleep_management_handle.user_handle_name[handle_index][0], 0, SLEEP_HANDLE_NAME_LEN);
        hal_nvic_restore_interrupt_mask(mask);
    } else {
        hal_nvic_restore_interrupt_mask(mask);
        log_hal_msgid_warning("[Sleep Management]sleep handle already release \r\n", 0);
    }
}

ATTR_TEXT_IN_TCM void sleep_management_lock_sleep(sleep_management_lock_sleep_t lock, uint8_t handle_index)
{
    uint32_t mask;
    uint64_t shift = 1;

    if (handle_index >= SLEEP_LOCK_HANDLE_MAX) {
        log_hal_msgid_error("[Sleep Management]sleep handle index error\r\n", 0);
        return;
    }

    if (lock == LOCK_SLEEP) {
        /* Lock sleep request */
            hal_nvic_save_and_set_interrupt_mask(&mask);
            if(sleep_management_handle.lock_sleep_request_count[handle_index] < 0xFF){
                sleep_management_handle.lock_sleep_request_count[handle_index]++;
                shift <<= handle_index;
                sleep_management_handle.lock_sleep_request |= shift;
            }
            hal_nvic_restore_interrupt_mask(mask);
    #ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
        sleep_management_debug_lock_sleep_timelog(lock, handle_index);
    #endif
        if (sleep_management_handle.lock_sleep_request_count[handle_index] == 0xFF) {
            if (handle_index < SLEEP_LOCK_USER_START_ID) {
                log_hal_msgid_warning("[Sleep Management]sleep handle=%d,lock sleep count full \r\n", 1, handle_index);
            } else {
                log_hal_msgid_warning("[Sleep Management]sleep handle=%d,lock sleep count full \r\n", 1, handle_index);
            }
        }
    } else {
        /* Unlock sleep request */
        hal_nvic_save_and_set_interrupt_mask(&mask);
        if (sleep_management_handle.lock_sleep_request_count[handle_index] > 0) {
            sleep_management_handle.lock_sleep_request_count[handle_index]--;
            if (sleep_management_handle.lock_sleep_request_count[handle_index] == 0) {
                shift <<= handle_index;
                sleep_management_handle.lock_sleep_request &= ~shift;
            }
            hal_nvic_restore_interrupt_mask(mask);
        } else {
            hal_nvic_restore_interrupt_mask(mask);
            if (handle_index < SLEEP_LOCK_USER_START_ID) {
                log_hal_msgid_warning("[Sleep Management]sleep handle=%d,lock sleep has already released \r\n", 1, handle_index);
                return;
            } else {
                log_hal_msgid_warning("[Sleep Management]sleep handle=%d,%s,lock sleep has already released \r\n", 2, handle_index, (char *)&sleep_management_handle.user_handle_name[(handle_index - SLEEP_LOCK_USER_START_ID)]);
                return;
            }
        }

        #ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            sleep_management_debug_lock_sleep_timelog(lock, handle_index);
        #endif
    }
}

bool sleep_management_check_perisys_sleep_locks(void)
{
    uint32_t mask;
    uint32_t perisys_handle_mask = 1;
    bool lock;

    perisys_handle_mask <<= SLEEP_LOCK_PERISYS_MAX;
    perisys_handle_mask -= 1;

    hal_nvic_save_and_set_interrupt_mask(&mask);

    if ( (sleep_management_handle.lock_sleep_request & perisys_handle_mask) == 0) {
        lock = false;
    } else {
        lock = true;
    }
    hal_nvic_restore_interrupt_mask(mask);
    return lock;
}

bool sleep_management_check_sleep_locks(void)
{
    uint32_t mask;
    bool lock;

    hal_nvic_save_and_set_interrupt_mask(&mask);

    if ((sleep_management_handle.lock_sleep_request) == 0) {
        lock = false;
    } else {
        lock = true;
    }
    hal_nvic_restore_interrupt_mask(mask);
    return lock;
}

bool sleep_management_check_handle_status(uint8_t handle_index)
{
    /*  check handle index range */
    if ((handle_index >= SLEEP_LOCK_HANDLE_MAX)) {
        log_hal_msgid_error("[Sleep Management]sleep handle index error\r\n", 0);
        return false;
    }

    if (((sleep_management_handle.lock_sleep_request >> (handle_index)) & 0x01) == 1) {
        return true;
    } else {
        return false;
    }
}

uint32_t sleep_management_get_lock_sleep_request_info(void)
{
    return sleep_management_handle.lock_sleep_request;
}

uint32_t sleep_management_get_lock_sleep_handle_list(void)
{
    uint8_t i;
    uint64_t lock_sleep_request = sleep_management_handle.lock_sleep_request;
    uint64_t temp = 1;

    log_hal_msgid_info("\r\n[Sleep Management]lock sleep handle list : \r\n", 0);
    for (i = 0; i < SLEEP_LOCK_HANDLE_MAX; i++) {
        if (lock_sleep_request & (temp << i)) {
            if (i < SLEEP_LOCK_USER_START_ID) {
                sleep_management_dump_sleep_handle_name(i);
            } else {
                SLEEP_MANAGER_MSGID_I("index of sleep handle = %d\r\n", 1, i);
                SLEEP_MANAGER_LOG_I("sleep handle = %s\r\n", sleep_management_handle.user_handle_name[i - SLEEP_LOCK_USER_START_ID]);
            }
        }
    }
    return lock_sleep_request;
}

void sleep_management_low_power_init_setting(void)
{
    unsigned int system_info;

    /* set SRCLKEN PAD (to PMIC) PD = 0 */
    *((volatile uint32_t *)(0xA20E0030)) = (*((volatile uint32_t *)(0xA20E0030)) & 0xFFFFFFFE);

    /* cm4_dcm_en */
    *((volatile uint32_t *)(0xE0100040)) = 0x107;

    /* Bonding */
    system_info = *((volatile uint32_t *)(0xA2010040)); /* SYSTEM_INFOD */
    if(!((system_info >> 6) & 0x1)) {
        /* bond_pkg_type_0 == 0 */
        *((volatile uint32_t *)(0xA20D0050)) = 0x2;     /* IO_CFG_1_PU_CFG0_CLR */
        *((volatile uint32_t *)(0xA20D0034)) = 0x2;     /* IO_CFG_1_PD_CFG0_SET */
    }

    if(!((system_info >> 7) & 0x1)) {
        /* bond_pkg_type_1 == 0 */
        *((volatile uint32_t *)(0xA20D0050)) = 0x1;     /* IO_CFG_1_PU_CFG0_CLR */
        *((volatile uint32_t *)(0xA20D0034)) = 0x1;     /* IO_CFG_1_PD_CFG0_SET */
    }

    if(((system_info >> 8) & 0x1)) {
        /* bond_psram_sip == 1 */
        *((volatile uint32_t *)(0xA20D0050)) = 0x4;     /* IO_CFG_1_PU_CFG0_CLR */
        *((volatile uint32_t *)(0xA20D0034)) = 0x4;     /* IO_CFG_1_PD_CFG0_SET */
    }

    if(((system_info >> 9) & 0x1)) {
        /* bond_sf_sip == 1 */
        *((volatile uint32_t *)(0xA20D0050)) = 0x8;     /* IO_CFG_1_PU_CFG0_CLR */
        *((volatile uint32_t *)(0xA20D0034)) = 0x8;     /* IO_CFG_1_PD_CFG0_SET */
    }

    /* LP Setting for DEBUG signal */
    /* DEBUGMON */
    *((volatile uint32_t *)(0xA2010060)) = 0xD;         /* TOP_DEBUG */
    /* XPLL */
    *((volatile uint32_t *)(0xA2050400)) = 0x0;         /* XPLL_DBG_PROB */
    /* BBPLL */
    *((volatile uint32_t *)(0xA2040420)) = 0x10001;     /* SYS_ABIST_MON_CON0 */
    *((volatile uint32_t *)(0xA2040428)) = 0x0;         /* SYS_ABIST_MON_CON2 */
    /* DCXO */
    *((volatile uint32_t *)(0xA2060100)) = 0x0;         /* DCXO_DEBUG0 */
    /* CMSYS */
    *((volatile uint32_t *)(0xE0100070)) = 0x3F;        /* CM_DEBUG_SELECT */
    /* INFRA AO/PD */
    *((volatile uint32_t *)(0xA2290010)) = 0xFFFF;      /* INFRA_CFG_DBGMON0 */
    /* DSPSYS0/1 */
    *((volatile uint32_t *)(0xA2010070)) = 0x3F3F;      /* DSPSYS_DEBUG_MON_SELECT */

    /* Disable PMIC_INT D2D PD*/
    *((volatile uint32_t*)(0xA20E0038)) = 0x10;

    /* Audio Global Bias disable */
    *((volatile uint32_t*)(0xA2070224)) |= 0x10;
}

void sleep_management_dump_sleep_handle_name(uint32_t handle_index)
{
    switch (handle_index) {
        case 0  : log_hal_msgid_info("sleep handle=%d,ESC_AESOTF\r\n"      , 1, (int)handle_index);   break;
        case 1  : log_hal_msgid_info("sleep handle=%d,CRYPTO\r\n"      , 1, (int)handle_index);   break;
        case 2  : log_hal_msgid_info("sleep handle=%d,TRNG\r\n"     , 1, (int)handle_index);   break;
        case 3  : log_hal_msgid_info("sleep handle=%d,ESC\r\n"     , 1, (int)handle_index);   break;
        case 4  : log_hal_msgid_info("sleep handle=%d,SPI_MST1\r\n"     , 1, (int)handle_index);   break;
        case 5  : log_hal_msgid_info("sleep handle=%d,SPI_MST2\r\n"     , 1, (int)handle_index);   break;
        case 6  : log_hal_msgid_info("sleep handle=%d,SPI_SLV\r\n" , 1, (int)handle_index);   break;
        case 7  : log_hal_msgid_info("sleep handle=%d,UART1\r\n" , 1, (int)handle_index);   break;
        case 8  : log_hal_msgid_info("sleep handle=%d,UART2\r\n" , 1, (int)handle_index);   break;
        case 9  : log_hal_msgid_info("sleep handle=%d,I2C1\r\n"  , 1, (int)handle_index);   break;
        case 10 : log_hal_msgid_info("sleep handle=%d,I2C2\r\n"    , 1, (int)handle_index);   break;
        case 11 : log_hal_msgid_info("sleep handle=%d,AUXADC\r\n"    , 1, (int)handle_index);   break;
        case 12 : log_hal_msgid_info("sleep handle=%d,SLOW_DMA1\r\n"    , 1, (int)handle_index);   break;
        case 13 : log_hal_msgid_info("sleep handle=%d,DCXO_CFG\r\n"     , 1, (int)handle_index);   break;
        case 14 : log_hal_msgid_info("sleep handle=%d,PERISYS_MAX\r\n"     , 1, (int)handle_index);   break;
        case 15 : log_hal_msgid_info("sleep handle=%d,BATTERY_MANAGEMENT\r\n"     , 1, (int)handle_index);   break;
        case 16 : log_hal_msgid_info("sleep handle=%d,DMA\r\n"     , 1, (int)handle_index);   break;
        case 17 : log_hal_msgid_info("sleep handle=%d,BATTERY\r\n"     , 1, (int)handle_index);   break;
        case 18 : log_hal_msgid_info("sleep handle=%d,UART0\r\n"     , 1, (int)handle_index);   break;
        case 19 : log_hal_msgid_info("sleep handle=%d,SLOW_DMA0\r\n"     , 1, (int)handle_index);   break;
        case 20 : log_hal_msgid_info("sleep handle=%d,I2C0\r\n"     , 1, (int)handle_index);   break;
        case 21 : log_hal_msgid_info("sleep handle=%d,SPI_MST0\r\n"     , 1, (int)handle_index);   break;
        case 22 : log_hal_msgid_info("sleep handle=%d,MSDC\r\n"     , 1, (int)handle_index);   break;
        case 23 : log_hal_msgid_info("sleep handle=%d,BT_CONTROLLER\r\n"     , 1, (int)handle_index);   break;
        case 24 : log_hal_msgid_info("sleep handle=%d,USB\r\n"     , 1, (int)handle_index);   break;
        case 25 : log_hal_msgid_info("sleep handle=%d,CHARGER_CASE\r\n"     , 1, (int)handle_index);   break;
        case 26 : log_hal_msgid_info("sleep handle=%d,BT_CONTROLLER_A2DP\r\n"     , 1, (int)handle_index);   break;
    }
    return;
}

void sleep_management_dump_debug_log(sleep_management_debug_log_index_t log_index)
{
    static uint32_t dump_index = 0;

    if ((log_index == SLEEP_MANAGEMENT_DEBUG_LOG_DUMP) && (dump_index != 0)) {
        if (((dump_index >> SLEEP_MANAGEMENT_DEBUG_LOG_OWNERSHIP_FAIL) & 0x01) == 0x01) {
            log_hal_msgid_error("connsys_get_ownership fail\r\n", 0);
        }
        dump_index = 0;
    } else {
        if (log_index < SLEEP_MANAGEMENT_DEBUG_LOG_MAX) {
            dump_index |= (1 << log_index);
        }
    }
}

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
#include "hal_gpt.h"
#define CLOCK_SOURCE_32K_FREQ   32.768
uint32_t sleep_handle_total_lock_sleep_time[SLEEP_LOCK_HANDLE_MAX];
uint32_t sleep_handle_total_lock_sleep_count[SLEEP_LOCK_HANDLE_MAX];
uint32_t sleep_backup_fun_time[SLEEP_BACKUP_RESTORE_MODULE_MAX + SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX];
uint32_t sleep_restore_fun_time[SLEEP_BACKUP_RESTORE_MODULE_MAX + SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX];

void sleep_management_debug_lock_sleep_timelog(sleep_management_lock_sleep_t lock, uint8_t handle_index)
{
    static uint32_t lock_sleep_time[SLEEP_LOCK_HANDLE_MAX], unlock_sleep_time;

    if (lock == LOCK_SLEEP) {
        if (sleep_management_handle.lock_sleep_request_count[handle_index] == 1) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &lock_sleep_time[handle_index]);
        }
        sleep_handle_total_lock_sleep_count[handle_index]++;
    } else {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &unlock_sleep_time);

        if (unlock_sleep_time >= lock_sleep_time[handle_index]) {
            sleep_handle_total_lock_sleep_time[handle_index] += unlock_sleep_time - lock_sleep_time[handle_index];
        } else {
            sleep_handle_total_lock_sleep_time[handle_index] += unlock_sleep_time + (0xFFFFFFFF - lock_sleep_time[handle_index]);
        }
    }
}

void sleep_management_debug_backup_restore_fun_timelog(uint32_t type, uint32_t order, uint32_t callback)
{
    static  uint32_t enter, exit;

    if (order == 0) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &enter);
    } else {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &exit);
        if (exit >= enter) {
            exit = exit - enter;
        } else {
            exit = exit + (0xFFFFFFFF - enter);
        }
        if (type == 0) {
            sleep_backup_fun_time[callback] = exit;
        } else {
            sleep_restore_fun_time[callback] = exit;
        }
    }

}

void sleep_management_debug_dump_lock_sleep_time(void)
{
    uint32_t i;
    float lock_time;

    log_hal_msgid_info("\r\ndump lock sleep time : \r\n", 0);
    for (i = 0; i < SLEEP_LOCK_HANDLE_MAX; i++) {
        if (sleep_handle_total_lock_sleep_count[i] > 0) {
            if (i < SLEEP_LOCK_USER_START_ID) {
                sleep_management_dump_sleep_handle_name(i);
            } else {
                log_hal_msgid_info("sleep handle=%d,%s\r\n", 2, (int)i, (char *)&sleep_management_handle.user_handle_name[i - SLEEP_LOCK_USER_START_ID][0]);
            }
            log_hal_msgid_info("count : %d\r\n"             , 1, (int)sleep_handle_total_lock_sleep_count[i]);

            lock_time = ((float)sleep_handle_total_lock_sleep_time[i]);
            lock_time = (lock_time) / CLOCK_SOURCE_32K_FREQ;
            if (lock_time < 1) {
                log_hal_msgid_info("total lock time : %d us,%d\r\n"   , 2, (int)lock_time * 1000, (int)sleep_handle_total_lock_sleep_time[i]);
            } else if (lock_time >= 1) {
                log_hal_msgid_info("total lock time : %d ms,%d\r\n"   , 2, (int)lock_time, (int)sleep_handle_total_lock_sleep_time[i]);
            }
            log_hal_msgid_info("\r\n", 0);
        }
    }
}

void sleep_management_debug_reset_lock_sleep_time(void)
{
    uint32_t i;

    for (i = 0; i < SLEEP_LOCK_HANDLE_MAX; i++) {
        sleep_handle_total_lock_sleep_time[i] = 0;
        sleep_handle_total_lock_sleep_count[i] = 0;
    }
}

void sleep_management_debug_dump_backup_restore_time(void)
{
    uint32_t i;
    float time;

    log_hal_msgid_info("\r\ndump backup restore function execute time : \r\n", 0);
    for (i = 0; i < SLEEP_BACKUP_RESTORE_MODULE_MAX; i++) {
        time = (((float)sleep_backup_fun_time[i] * 1000) / CLOCK_SOURCE_32K_FREQ);
        log_hal_msgid_info("backup fun[%d]  : %d us,%d\r\n", 3, (int)i, (int)time , (int)sleep_backup_fun_time[i]);

        time = (((float)sleep_restore_fun_time[i] * 1000) / CLOCK_SOURCE_32K_FREQ);
        log_hal_msgid_info("restore fun[%d] : %d us,%d\r\n", 3, (int)i, (int)time , (int)sleep_restore_fun_time[i]);
    }
}

void sleep_management_dump_wakeup_source(uint32_t wakeup_source, uint32_t eint_status)
{
    int i;
    if (sleep_management_status.abort_sleep != 0) {
        log_hal_msgid_warning("[Sleep]Abort Deep Sleep\r\n", 0);
        sleep_management_status.abort_sleep = 0;
    }

    log_hal_msgid_info("===============Wakeup from Deep Sleep===============\r\n", 0);
    log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE = 0x%x\r\n", 1, (int)wakeup_source);
    for(i = 0;i < 13;i++){
        if(((wakeup_source >> i)&0x01) == 0){
            switch(i){
                case 0  : log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE : GPT\r\n", 0);          break;
                case 1  : sleep_management_dump_eint_wakeup_source(eint_status);        break;
                case 2  : log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE : IRQGEN\r\n", 0);       break;
                case 3  : log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE : OST\r\n", 0);          break;
                case 4  : log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE : AUDIO\r\n", 0);        break;
                case 5  : log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE : I2S\r\n", 0);          break;
                case 6  : log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE : I2S_DMA\r\n", 0);      break;
                case 7  : log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE : CAP_TOUCH\r\n", 0);    break;
                case 8  : log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE : ANC\r\n", 0);          break;
                case 9  : log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE : SPI_SLAVE\r\n", 0);    break;
                case 10 : log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE : RGU\r\n", 0);          break;
                case 11 : log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE : JTAG\r\n", 0);         break;
                case 12 : log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE : DSP_DMA\r\n", 0);      break;
                default : break;
            }
        }
    }
}

void sleep_management_dump_eint_wakeup_source(uint32_t eint_num)
{
    uint32_t i;

    log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE : EINT = 0x%x\r\n", 1, (unsigned int)eint_num);

    for (i = 0; i < HAL_EINT_NUMBER_MAX; i++) {
        if (((eint_num >> i) & 0x01) == 0x01) {
            if (i <= 22) {
                log_hal_msgid_info("[Sleep Management]EINT%d\r\n", 1, (unsigned int)i);
            }
            switch (i) {
                case 23 : log_hal_msgid_info("[Sleep Management]EINT_UART_0_RX\r\n", 0);           break;
                case 24 : log_hal_msgid_info("[Sleep Management]EINT_UART_1_RX\r\n", 0);           break;
                case 25 : log_hal_msgid_info("[Sleep Management]EINT_UART_2_RX\r\n", 0);           break;
                case 26 : log_hal_msgid_info("[Sleep Management]EINT_RTC_EVENT\r\n", 0);           break;
                case 27 : log_hal_msgid_info("[Sleep Management]EINT_PDN_USB\r\n", 0);             break;
                case 28 : log_hal_msgid_info("[Sleep Management]EINT_PMU_IRQ\r\n", 0);             break;
                case 29 : log_hal_msgid_info("[Sleep Management]EINT_IR_WAK_AP\r\n", 0);           break;
                case 30 : log_hal_msgid_info("[Sleep Management]EINT_CONN2AP_WDT_IRQ\r\n", 0);     break;
                case 31 : log_hal_msgid_info("[Sleep Management]EINT_CONN2AP_WAKEUP_OUT\r\n", 0);  break;
            }
        }
    }
}
#endif

#endif
