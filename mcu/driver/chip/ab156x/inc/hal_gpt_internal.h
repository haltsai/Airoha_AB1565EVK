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
 
#ifndef _HAL_GPT_INTERNAL_H_
#define _HAL_GPT_INTERNAL_H_

#include "hal_gpt.h"

#ifdef HAL_GPT_MODULE_ENABLED
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "hal_platform.h"

//#define AB155X_FPGA_13M_to_32K


#ifdef __cplusplus
extern "C"
{
#endif

//#define GPT_DEBUG_LOG

#define HAL_GPT_MS_PORT             HAL_GPT_4       /* configure HAL_GPT_4 as milliseconds timer */
#define HAL_GPT_US_PORT             HAL_GPT_2       /* configure HAL_GPT_2 as microseconds timer */
#define HAL_GPT_SW_MS_PORT          HAL_GPT_3       /* configure HAL_GPT_3 as software timer */
#define HAL_GPT_SW_US_PORT          HAL_GPT_1       /* configure HAL_GPT_1 as software timer */
//#define HAL_GPT_SW_MAGIC            0xdead0000      /* SW GPT magic handle number*/
//#define HAL_GPT_SW_HANDLE_MASK      0xffff          /* SW GPT handle mask section*/

#define HAL_GPT_SUPPORT_DELAY_CHANNEL   2           /* indicate the supported ms and us channel number*/    

#define HAL_GPT_SUPPORT_INT_CHANNEL     4           /* indicate the supported interrupt timer channle number*/  
#define HAL_GPT_SUPPORT_GPT_MAPPING     HAL_GPT_0,HAL_GPT_3,HAL_GPT_1,HAL_GPT_7   /* indicate the supported interrupt timer channle*/ 

#define HAL_GPT_CONTEXT_INDEX_ERROR     0xff        /* indicate the channel mapping error*/ 

/*define GPT SW timer type*/
#define HAL_GPI_SW_TYPE_NUMBER      0x02            /* indicate SW GPT type, now we have 1 sw ms timer and 1 sw us timer*/



#define SW_GPT_HANDLE_MAGIC_NUMBER 0xAAAA0000         /* SW GPT magic handle  number */
#define SW_GPT_HANDLE_TO_INDEX(handle) (handle & 0xFFFF)  /* Get SW GPT index accord to handle */



typedef enum {
    SW_GPT_MS_TYPE    = 0,
    SW_GPT_US_TYPE    = 1,
    SW_GPT_MAX_TYPE
} sw_gpt_type_t;


#define GPT_COUNT_CLEAR         0x1
#define GPT_COUNT_START         0x1
#define GPT_IRQ_ENABLE          0x1
#define GPT_IRQ_FLAG_STA        0x1
#define GPT_IRQ_FLAG_ACK        0x1
#define GPT_CLOCK_32KHZ         (0x10)
#define GPT_CLOCK_13MHZ         (0x00)
#define GPT_CLOCK_26M_SOURCE    (26000)

#define GPT_REG(_ADDRBASE_)     (*(volatile unsigned int *)(_ADDRBASE_))
#define GPT_PDN_CLRD2        (0xA2030B20)
#define GPT_PDN_COND2        (0xA2030B00)
#define GPT_PDN_MASK         (1<<17)
#define GPT_MODE_REPEAT_CTR    0x100
#define GPT_MODE_ONE_SHOT_CTR  0
#define GPT_MODE_KEEP_GO_CTR   0x200
#define GPT_MODE_FREE_RUN_CTR  0x300
#define GPT_CLOCK_GATE_CTR     0x10000
#define GPT_CLOCK_UNGATE_CTR   0
/***************************************************************/

//#define gpt_sw_delta(data1, data2)  (data1 >= data2) ? (data1 - data2) : (0xffffffff - (data2 - data1) + 1)

typedef enum {
    GPT_MODE_ONE_SHOT = 0,
    GPT_MODE_REPEAT   = 0x100,
    GPT_MODE_KEEP_GO  = 0x200,
    GPT_MODE_FREE_RUN = 0x300
} gpt_mode_type_t;

typedef enum {
    GPT_CLOCK_UNGATE  = 0,
    GPT_CLOCK_GATE    = 0x10000
} gpt_clock_gate_t;

typedef enum {
    GPT_DIVIDE_1  = 0,
    GPT_DIVIDE_2  = 1,
    GPT_DIVIDE_3  = 2,
    GPT_DIVIDE_4  = 3 ,
    GPT_DIVIDE_5  = 4,
    GPT_DIVIDE_6  = 5,
    GPT_DIVIDE_7  = 6,
    GPT_DIVIDE_8  = 7,
    GPT_DIVIDE_9  = 8,
    GPT_DIVIDE_10 = 9,
    GPT_DIVIDE_11 = 10,
    GPT_DIVIDE_12 = 11,
    GPT_DIVIDE_13 = 12,
    GPT_DIVIDE_16 = 13,
    GPT_DIVIDE_32 = 14,
    GPT_DIVIDE_64 = 15
} gpt_clock_divide_t;

typedef struct {
    hal_gpt_callback_t callback;
    void *user_data;
} gpt_callback_context_t;

typedef struct {
    hal_gpt_running_status_t running_status;
    gpt_callback_context_t   callback_context;
    bool                     has_initilized;
    bool                     is_gpt_locked_sleep;
} gpt_context_t;




/*SW GPT*********************************************************************/

typedef enum{
    SW_GPT_NOTE_STATUS_OF_FREE = 0, 
    SW_GPT_NOTE_STATUS_OF_ALLOC,
    SW_GPT_NOTE_STATUS_OF_RUNING,
}sw_gpt_note_status_t;

typedef struct{
    uint32_t               expire_count;
    sw_gpt_note_status_t   status;
    hal_gpt_callback_t callback;
    void *user_data;
    sw_gpt_type_t          timer_port_type;
}sw_gpt_note_t;

typedef struct{
sw_gpt_note_t sw_gpt_timer[HAL_SW_GPT_MAX_USERS];
uint32_t      used_timer_count;
}sw_gpt_add_note_t ;

/** *********marco about lock sleep when use 13M clock source***************/
#if 1
#ifdef HAL_SLEEP_MANAGER_ENABLED
#define GPT_GET_SLEEP_HANDLE(gpt_port) \
do{ \
    gpt_lock_sleep_handle[gpt_port] = hal_sleep_manager_set_sleep_handle(gpt_lock_sleep_name[gpt_port]); \
}while(0)

#define GPT_RELEASE_SLEEP_HANDLE(gpt_port) \
do{ \
    hal_sleep_manager_release_sleep_handle(gpt_lock_sleep_handle[gpt_port]); \
}while(0)

#define GPT_LOCK_SLEEP(gpt_port) \
do{ \
    hal_sleep_manager_lock_sleep(gpt_lock_sleep_handle[gpt_port]); \
}while(0)

#define GPT_UNLOCK_SLEEP(gpt_port) \
do{ \
    hal_sleep_manager_unlock_sleep(gpt_lock_sleep_handle[gpt_port]); \
}while(0)
#else
#define GPT_GET_SLEEP_HANDLE(gpt_port)
#define GPT_RELEASE_SLEEP_HANDLE(gpt_port)
#define GPT_LOCK_SLEEP(gpt_port)
#define GPT_UNLOCK_SLEEP(gpt_port)
#endif
#endif



/********* varible extern *************/
extern gpt_context_t             g_gpt_context[HAL_GPT_SUPPORT_INT_CHANNEL];
extern hal_gpt_running_status_t  gpt_running_state[HAL_GPT_SUPPORT_DELAY_CHANNEL];
/***********************/

/******** funtion extern **************/
void gpt_reset_default_timer(uint32_t gpt_port);
void gpt_nvic_register(void);
void gpt_delay_time(GPT_REGISTER_T *gpt, uint32_t count);
void  gpt_start_free_run_timer(GPT_REGISTER_T *gpt, uint32_t clock_source, uint32_t divide);
uint32_t gpt_convert_ms_to_32k_count(uint32_t ms);
uint32_t gpt_current_count(GPT_REGISTER_T *gpt);
void gpt_open_clock_source(void);
uint32_t gpt_save_and_mask_interrupt(GPT_REGISTER_T *gpt);
void gpt_restore_interrupt(GPT_REGISTER_T *gpt, uint32_t mask);

uint32_t gpt_clock_calibrate_us_to_count(uint32_t time_us);
uint32_t gpt_clock_calibrate_count_to_us(uint32_t count);
void gpt_get_lfosc_clock(void);
uint32_t gpt_get_context_index(hal_gpt_port_t gpt_port);


void sw_gpt_find_the_next_expire_time(uint32_t current_count,uint32_t *p_index, uint32_t *p_expire_count,sw_gpt_type_t sw_type);
void clear_gpt_and_restart_hw_gpt(uint32_t next_expired_count, hal_gpt_port_t port);
void sw_gpt_callback(sw_gpt_type_t *sw_type_port);
hal_gpt_status_t sw_gpt_timer_start(uint32_t handle, uint32_t timeout_time, hal_gpt_callback_t callback, void *user_data, sw_gpt_type_t sw_type);
hal_gpt_status_t sw_gpt_timer_stop(uint32_t handle,sw_gpt_type_t sw_type);
hal_gpt_status_t sw_gpt_get_remaining_time(uint32_t handle,uint32_t *remain_time,sw_gpt_type_t sw_type);
hal_gpt_status_t sw_gpt_start_timer_for_absolute_tick(uint32_t handle , hal_sw_gpt_absolute_parameter_t *absolute_parameter,sw_gpt_type_t sw_type);

#ifdef __cplusplus
}
#endif

#endif /*HAL_GPT_MODULE_ENABLED*/

#endif /*_HAL_GPT_INTERNAL_H_*/

