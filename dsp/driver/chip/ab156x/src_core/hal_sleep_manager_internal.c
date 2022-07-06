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
#include "hal_clock_platform_mt2811.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"

extern void sleep_exit_prepare();
#ifdef HAL_SLEEP_MANAGER_ENABLED

#ifndef __UBL__
#include "assert.h"
#else
//#define assert(expr) log_hal_error("assert\r\n")
#endif

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
#define log_debug(_message,...) printf(_message, ##__VA_ARGS__)
#else
#define log_debug(_message,...)
#endif

#define SKIP_UART_BACKUP_RESTORE 0
#define SPM_ABORT_TEST_EN 0

static sleep_management_handle_t sleep_management_handle = {
    .lock_sleep_request = 0,
    .user_handle_resoure = 0,
    .user_handle_count = 0
};

#define DSPCFG_BOOT_VECTOR                         ((volatile uint32_t*)(0xA21F0010))
#define DSPCFG_BOOT_VECTOR_SELECT                  ((volatile uint32_t*)(0xA21F0014))
#define DSP_DEEP_SLEEP                             ((volatile uint32_t*)(0xA21F001C))

static sleep_management_suspend_callback_func_t    suspend_callback_func_table      [SLEEP_BACKUP_RESTORE_MODULE_MAX];
static sleep_management_resume_callback_func_t     resume_callback_func_table       [SLEEP_BACKUP_RESTORE_MODULE_MAX];
static sleep_management_suspend_callback_func_t    suspend_user_callback_func_table [SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX];
static sleep_management_resume_callback_func_t     resume_user_callback_func_table  [SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX];
static uint32_t suspend_user_register_count = 0, resume_user_register_count = 0;
sleep_management_dsp_status_t sleep_management_dsp_status = {0,0,0,0,(uint32_t)DSPCFG_BOOT_VECTOR};
XtosCoreState dsp_core_state_backup;

//#define CACHE_TEST

#ifdef CACHE_TEST
ATTR_ZIDATA_IN_DRAM uint32_t temp0,temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8;
ATTR_ZIDATA_IN_DRAM uint32_t cacheattr0,cacheattr1;
ATTR_ZIDATA_IN_DRAM uint32_t icacheway0,icacheway1 ,dcacheway0,dcacheway1;
ATTR_ZIDATA_IN_DRAM uint32_t set_region_result,  core_save_result;
#endif

__attribute__((optimize ("-O0"))) ATTR_TEXT_IN_IRAM void sleep_management_enter_deep_sleep(void)
{
    SPM_SET_DSP0_SW_STATE(0x0);
    /* Peripheral driver backup callback function */
    sleep_management_suspend_callback();
#ifdef HAL_CACHE_MODULE_ENABLED
    extern void cache_status_save(void);
    ////cache_status_save();
#endif /* HAL_CACHE_MODULE_ENABLED */

    SPM_SET_DSP0_SW_STATE(0x1);

    *DSPCFG_BOOT_VECTOR_SELECT = 1;

    sleep_management_dsp_status.bootvector_backup = *DSPCFG_BOOT_VECTOR;
    sleep_management_dsp_status.bootvector_address = (uint32_t)DSPCFG_BOOT_VECTOR;

    *DSP_DEEP_SLEEP = 1;
    __asm__ __volatile__ (
                            " movi   a2, 0x00108400             \n"
                            " wsr    a2, MEMCTL                 \n");

    sleep_management_dsp_status.pshut_off = _xtos_core_save(XTOS_KEEPON_MEM, &dsp_core_state_backup, (void *) 0);

    SPM_SET_DSP0_SW_STATE(0x2);
    if(*DSP_DEEP_SLEEP == 1)
    {
        if((*SPM_DSP_WAKEUP_SOURCE_STA&0x10000) == 0) {
        SPM_SET_DSP0_SW_STATE(0x9);
        //abort case before waiti
        sleep_management_dsp_status.abort_sleep = 1;
        *DSP_DEEP_SLEEP = 0;
        }else {
            *((volatile uint32_t *)0xA2110CBC) = 0; //Clear cpu power off flag

            SPM_SET_DSP0_SW_STATE(0x3);
            __asm__ __volatile__ (  " movi   a2, 1f                     \n"
                                    " s32i   a2, %0, 0                  \n"
                                    " isync                             \n"
                                    " waiti 4                           \n"
                                    " 1:                                \n"
                                    " isync                             \n"
                                    " nop                               \n"
                                    " rsil  a2, 15                      \n"
                                    " movi  a2 , 0                      \n"
                                    " movi  a3 , 0xA21F001C             \n"
                                    " s32i  a2 , a3 , 0                 \n"
                                    " movi  a2 , sleep_management_dsp_status \n"
                                    " l32i  a2 , a2 , 8                     \n"
                                    " movi  a3 , dsp_core_state_backup  \n"
                                    " call0   _xtos_core_restore_nw     \n"
                                    ::"a" (sleep_management_dsp_status.bootvector_address));
        }
    }

    if(sleep_management_dsp_status.abort_sleep != 0) {
           __asm__ __volatile__ (   "rsr     a2 , PS\n"
                                    "movi    a3 , 0x40000\n"
                                    "or      a3, a3, a2\n"
                                    "wsr      a3 , PS\n"
                                   );
    }

    __asm__ __volatile__ (      " movi   a2 , 0x00108400            \n"
                                " wsr    a2 , MEMCTL                \n"
                                " isync                             \n");

    SPM_SET_DSP0_SW_STATE(0x6);

    /* Get wakeup source */
    sleep_management_dsp_status.wakeup_source = *SPM_DSP_WAKEUP_SOURCE_STA;

    *DSPCFG_BOOT_VECTOR = sleep_management_dsp_status.bootvector_backup;

#ifdef HAL_CACHE_MODULE_ENABLED
    extern void cache_status_restore(void);
    //cache_status_restore();
#endif /* HAL_CACHE_MODULE_ENABLED */

    /* Peripheral driver restore callback function */
    sleep_management_resume_callback();
    SPM_SET_DSP0_SW_STATE(0x7);

    #if SPM_ABORT_TEST_EN
    hal_nvic_enable_irq(GPT_IRQn);
    #endif

    if(sleep_management_dsp_status.abort_sleep != 0){
        //log_hal_msgid_warning("abort : %d", 1, sleep_management_dsp_status.abort_sleep);
        sleep_management_dsp_status.abort_sleep = 0;
        SPM_SET_DSP0_SW_STATE(0x8);
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
            //log_hal_msgid_error("[Sleep Management]register suspend callback function overflow\r\n", 0);
            //assert(0);
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
            //log_hal_msgid_error("[Sleep Management]register resume callback function overflow\r\n", 0);
            //assert(0);
        }
    } else {
        resume_callback_func_table[module].func = callback;
        resume_callback_func_table[module].data = data;
        resume_callback_func_table[module].init_status = SLEEP_MANAGEMENT_INITIALIZED;
    }
}

ATTR_TEXT_IN_IRAM void sleep_management_suspend_callback()
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

ATTR_TEXT_IN_IRAM void sleep_management_resume_callback()
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

    hal_nvic_save_and_set_interrupt_mask(&mask);
    for (index = 0 ; index < SLEEP_LOCK_HANDLE_USER_MAX; index++) {
        if (((sleep_management_handle.user_handle_resoure >> index) & 0x01) == 0) {
            sleep_management_handle.user_handle_resoure |= (1 << index);
            sleep_management_handle.user_handle_count++;

            memset(&sleep_management_handle.user_handle_name[index][0], 0, SLEEP_HANDLE_NAME_LEN);
            name_len = strlen(handle_name);

            if (name_len >= SLEEP_HANDLE_NAME_LEN) {
                name_len = SLEEP_HANDLE_NAME_LEN - 1;
            }
            memcpy(&sleep_management_handle.user_handle_name[index][0], handle_name, name_len);

            /* Check handle name */
            if (name_len == 0) {
                //log_hal_msgid_error("[Sleep Management]sleep handle name error\r\n", 0);
                //assert(0);
            }
            for (i = 0; i < name_len; i++) {
                if ((sleep_management_handle.user_handle_name[index][i] <= 0x20) || (sleep_management_handle.user_handle_name[index][i] >= 0x7E)) {
                    //log_hal_msgid_error("[Sleep Management]sleep handle name error\r\n", 0);
                    //assert(0);
                }
            }
            break;
        }
    }
    hal_nvic_restore_interrupt_mask(mask);
    //log_hal_msgid_info("[Sleep Management]sleep handle name : %s\r\n", 1, &sleep_management_handle.user_handle_name[index][0]);
    if (index >= SLEEP_LOCK_HANDLE_USER_MAX) {
        //log_hal_msgid_error("[Sleep Management]cannot get sleep handle\r\n", 0);
        //assert(0);
        return (SLEEP_LOCK_INVALID_ID);
    }

    index += SLEEP_LOCK_USER_START_ID;

    return (index);
}

void sleep_management_release_lock_handle(uint8_t handle_index)
{
    uint32_t mask;
    /*  check handle index range */
    if ((handle_index >= SLEEP_LOCK_HANDLE_MAX) || (handle_index < SLEEP_LOCK_USER_START_ID)) {
        //log_hal_msgid_error("[Sleep Management]sleep handle index error\r\n", 0);
        return;
    }

    handle_index -= SLEEP_LOCK_USER_START_ID;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (((sleep_management_handle.user_handle_resoure >> handle_index) & 0x01) == 1) {
        sleep_management_handle.user_handle_count--;
        sleep_management_handle.user_handle_resoure &= ~(1 << handle_index);
        memset(&sleep_management_handle.user_handle_name[handle_index][0], 0, SLEEP_HANDLE_NAME_LEN);
    } else {
        //log_hal_msgid_warning("[Sleep Management]sleep handle already release \r\n", 0);
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void sleep_management_lock_sleep(sleep_management_lock_sleep_t lock, uint8_t handle_index)
{
    uint32_t mask;

    if (handle_index >= SLEEP_LOCK_HANDLE_MAX) {
        //log_hal_msgid_error("[Sleep Management]sleep handle index error\r\n", 0);
        return;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    /*for some speical handle, because DSP0 and DSP1 DCM not auto, need sw control
    If there are some cross core memory access or DMA memory access, need both lock sleep and DCM.*/

#ifdef HAL_SLEEP_MANAGER_LOCK_SLEEP_CROSS_CORE
    if (lock == LOCK_SLEEP) {
        if( (SLEEP_LOCK_DSP0_CROSS_CORE_MEMORY_ACCESS == handle_index) || (SLEEP_LOCK_DSP0_CROSS_CORE_PRELOADER == handle_index) ||
            (SLEEP_LOCK_DMA == handle_index) ) {
            clock_dsp_dcm_disable(CLK_DSP0);
        }
    } else {
        if( (SLEEP_LOCK_DSP0_CROSS_CORE_MEMORY_ACCESS == handle_index) || (SLEEP_LOCK_DSP0_CROSS_CORE_PRELOADER == handle_index) ||
            (SLEEP_LOCK_DMA == handle_index)){
            clock_dsp_dcm_enable(CLK_DSP0);
        }
    }
#endif
    if (lock == LOCK_SLEEP) {
        /* Lock sleep request */
        sleep_management_handle.lock_sleep_request_count[handle_index]++;
        sleep_management_handle.lock_sleep_request |= (1 << handle_index);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
        sleep_management_debug_lock_sleep_timelog(lock, handle_index);
#endif
        if (sleep_management_handle.lock_sleep_request_count[handle_index] == 0xFF) {
            if (handle_index < SLEEP_LOCK_USER_START_ID) {
                //log_hal_msgid_warning("[Sleep Management]sleep handle=%d,lock sleep count full \r\n", 1, handle_index);
            } else {
                //log_hal_msgid_warning("[Sleep Management]sleep handle=%d,%s,lock sleep count full \r\n", 2, handle_index, (char *)&sleep_management_handle.user_handle_name[(handle_index - SLEEP_LOCK_USER_START_ID)]);
            }
        }
    } else {
        /* Unlock sleep request */
        if (sleep_management_handle.lock_sleep_request_count[handle_index] > 0) {
            sleep_management_handle.lock_sleep_request_count[handle_index]--;
            if (sleep_management_handle.lock_sleep_request_count[handle_index] == 0) {
                sleep_management_handle.lock_sleep_request &= ~(1 << handle_index);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
                sleep_management_debug_lock_sleep_timelog(lock, handle_index);
#endif
            }
        } else {
            if (handle_index < SLEEP_LOCK_USER_START_ID) {
                //log_hal_msgid_warning("[Sleep Management]sleep handle=%d,lock sleep has already released \r\n", 1, handle_index);
            } else {
                //log_hal_msgid_warning("[Sleep Management]sleep handle=%d,%s,lock sleep has already released \r\n", 2, handle_index, (char *)&sleep_management_handle.user_handle_name[(handle_index - SLEEP_LOCK_USER_START_ID)]);
            }
        }
    }
    hal_nvic_restore_interrupt_mask(mask);
}

bool sleep_management_check_sleep_locks(void)
{
    uint32_t mask;
    bool lock;
    hal_nvic_save_and_set_interrupt_mask(&mask);

    if (sleep_management_handle.lock_sleep_request == 0) {
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
        //log_hal_msgid_error("[Sleep Management]sleep handle index error\r\n", 0);
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

void sleep_management_dump_sleep_handle_name(uint8_t handle_index)
{
    switch (handle_index) {
        case 0  : log_hal_msgid_info("sleep handle=%d,DMA\r\n", 1, (int)handle_index);         break;
        case 1  : log_hal_msgid_info("sleep handle=%d,SFC\r\n", 1, (int)handle_index);         break;
        case 2  : log_hal_msgid_info("sleep handle=%d,I2S0\r\n", 1, (int)handle_index);        break;
        case 3  : log_hal_msgid_info("sleep handle=%d,I2S1\r\n", 1, (int)handle_index);        break;
        case 4  : log_hal_msgid_info("sleep handle=%d,I2S2\r\n", 1, (int)handle_index);        break;
        case 5  : log_hal_msgid_info("sleep handle=%d,I2S3\r\n", 1, (int)handle_index);        break;
        case 6  : log_hal_msgid_info("sleep handle=%d,SPI_MST0\r\n", 1, (int)handle_index);    break;
        case 7  : log_hal_msgid_info("sleep handle=%d,SPI_MST1\r\n", 1, (int)handle_index);    break;
        case 8  : log_hal_msgid_info("sleep handle=%d,SPI_SLV\r\n", 1, (int)handle_index);     break;
        case 9  : log_hal_msgid_info("sleep handle=%d,UART0\r\n", 1, (int)handle_index);       break;
        case 10 : log_hal_msgid_info("sleep handle=%d,UART1\r\n", 1, (int)handle_index);       break;
        case 11 : log_hal_msgid_info("sleep handle=%d,I2C0\r\n", 1, (int)handle_index);        break;
        case 12 : log_hal_msgid_info("sleep handle=%d,I2C1\r\n", 1, (int)handle_index);        break;
    }
    return;
}

uint32_t sleep_management_get_lock_sleep_handle_list(void)
{
    uint8_t i;
    uint32_t lock_sleep_request = sleep_management_handle.lock_sleep_request;

    log_hal_msgid_info("\r\n[Sleep Management]lock sleep handle list : \r\n", 0);
    for (i = 0; i < SLEEP_LOCK_HANDLE_MAX; i++) {
        if (lock_sleep_request & (1 << i)) {
            if (i < SLEEP_LOCK_USER_START_ID) {
                sleep_management_dump_sleep_handle_name(i);
            } else {
                log_hal_msgid_info("sleep handle=%d,%s\r\n", 2, i, (char *)&sleep_management_handle.user_handle_name[i - SLEEP_LOCK_USER_START_ID][0]);
            }
        }
    }
    return lock_sleep_request;
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
    uint8_t i;
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
    uint8_t i;
    if (sleep_management_dsp_status.abort_sleep != 0) {
        log_hal_msgid_info("[Sleep]Abort Deep Sleep:%d\r\n", 1, sleep_management_dsp_status.abort_sleep);
        sleep_management_dsp_status.abort_sleep = 0;
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
                case 11 : log_hal_msgid_info("[Sleep Management]WAKEUP_SOURCE : NONE\r\n", 0);         break;
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
