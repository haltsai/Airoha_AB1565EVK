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
 
#include "hal_gpt.h"
#include "hal_nvic_internal.h"
#ifdef HAL_GPT_MODULE_ENABLED
#include "hal_gpt_internal.h"
#include "hal_log.h"
#include "memory_attribute.h"
#include "hal_nvic_internal.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#include "hal_sleep_manager_platform.h"
const char *gpt_lock_sleep_name[HAL_GPT_MAX_PORT] = {"GPT0", "GPT1", "GPT2", "GPT3", "GPT4", "GPT5", "GPT6", "GPT7"};
uint8_t gpt_lock_sleep_handle[HAL_GPT_MAX_PORT];

static bool hal_gpt_is_port_valid(hal_gpt_port_t gpt_port)
{
    // /*make sure this port just for sw gpt*/
    // if ((gpt_sw_context.is_sw_gpt == true) && ((gpt_port == HAL_GPT_SW_MS_PORT)||(gpt_port==HAL_GPT_SW_US_PORT))) {
    //     return true;
    // }
    /*default gpt0 & gpt7 reserve for CM4 user*/
    if ((gpt_port < HAL_GPT_MAX_PORT) && ((gpt_port == HAL_GPT_0)|| (gpt_port == HAL_GPT_7))) {
        return true;
    }

    return false;
}

hal_gpt_status_t hal_gpt_init(hal_gpt_port_t gpt_port)
{
    uint32_t index;

    index = gpt_get_context_index(gpt_port);
    
    if ((hal_gpt_is_port_valid(gpt_port) != true) || (index == HAL_GPT_CONTEXT_INDEX_ERROR)) {
        return HAL_GPT_STATUS_ERROR_PORT;
    }

    if ((g_gpt_context[index].running_status == HAL_GPT_RUNNING) || (g_gpt_context[index].has_initilized == true)) {
        return HAL_GPT_STATUS_ERROR;
    }

    /*set structure to 0 */
    memset(&g_gpt_context[index], 0, sizeof(gpt_context_t));

    /*enable pdn power, open clock source*/
    gpt_open_clock_source();
    GPT(gpt_port)->GPT_CON_UNION.GPT_CON &= ~(GPT_CLOCK_GATE);

    GPTGLB->GPT_IRQMSK0    &= ~(1<<gpt_port);  //CM4 IRQ enable
    GPTGLB->GPT_WAKEUPMSK0 &= ~(1<<gpt_port);  //CM4 wakeup enable

    /*disable interrutp*/
    GPT(gpt_port)->GPT_IRQ_EN  &= ~GPT_IRQ_ENABLE;

    GPT_GET_SLEEP_HANDLE(gpt_port);
#ifdef GPT_DEBUG_LOG
    log_hal_msgid_info("[GPT%d] init OK\r\n",1, (int)gpt_port);
#endif

    /*set flag respect this port has initlized */
    g_gpt_context[index].has_initilized = true;

    return HAL_GPT_STATUS_OK;
}

hal_gpt_status_t hal_gpt_deinit(hal_gpt_port_t gpt_port)
{
    uint32_t index;

    index = gpt_get_context_index(gpt_port);
    
    if ((hal_gpt_is_port_valid(gpt_port) != true) || (index == HAL_GPT_CONTEXT_INDEX_ERROR)) {
        return HAL_GPT_STATUS_ERROR_PORT;
    }

    if (g_gpt_context[index].running_status == HAL_GPT_RUNNING) {

        return HAL_GPT_STATUS_ERROR;
    }

    /* set structure to 0 */
    memset(&g_gpt_context[index], 0, sizeof(gpt_context_t));

    /* set flag indicate this port has deinitlized */
    g_gpt_context[index].has_initilized = false;

    GPTGLB->GPT_IRQMSK0 |= (1<<gpt_port);

    gpt_reset_default_timer((uint32_t)gpt_port);

    GPT_RELEASE_SLEEP_HANDLE(gpt_port);
#ifdef GPT_DEBUG_LOG
    log_hal_msgid_info("[GPT%d] deinit OK\r\n",1, (int)gpt_port);
#endif

    return HAL_GPT_STATUS_OK;
}


ATTR_TEXT_IN_TCM hal_gpt_status_t hal_gpt_get_free_run_count(hal_gpt_clock_source_t clock_source, uint32_t *count)
{
    /* millisecond free run timer */
    if (clock_source == HAL_GPT_CLOCK_SOURCE_32K) {
        if (gpt_running_state[0] != HAL_GPT_RUNNING) {

            /* set clock source to 32khz, and start timer */
            #ifdef FPGA_ENV
                gpt_start_free_run_timer(GPT(HAL_GPT_US_PORT), GPT_CLOCK_13MHZ, GPT_DIVIDE_13);
            #else
                gpt_start_free_run_timer(GPT(HAL_GPT_MS_PORT), GPT_CLOCK_32KHZ, GPT_DIVIDE_1);
            #endif

            gpt_running_state[0] = HAL_GPT_RUNNING;
        }
        
        #ifdef FPGA_ENV
            *count = gpt_current_count(GPT(HAL_GPT_US_PORT)) / 30;
        #else
            *count = gpt_current_count(GPT(HAL_GPT_MS_PORT));
        #endif

    } /* microsecond free rum timer */
    else if (clock_source == HAL_GPT_CLOCK_SOURCE_1M) {
        if (gpt_running_state[1] != HAL_GPT_RUNNING) {

            /* set clcok source to 1mhz, and start timer */
            gpt_start_free_run_timer(GPT(HAL_GPT_US_PORT), GPT_CLOCK_13MHZ, GPT_DIVIDE_13);

            gpt_running_state[1] = HAL_GPT_RUNNING;

        }
        *count = gpt_current_count(GPT(HAL_GPT_US_PORT));
    } else {
        return HAL_GPT_STATUS_INVALID_PARAMETER;
    }

    return HAL_GPT_STATUS_OK;
}

ATTR_TEXT_IN_TCM hal_gpt_status_t hal_gpt_get_duration_count(uint32_t start_count, uint32_t end_count, uint32_t *duration_count)
{
    if (duration_count == NULL) {
        return HAL_GPT_STATUS_INVALID_PARAMETER;
    }

    if (end_count > start_count) {
        *duration_count = end_count - start_count;
    } else {
        *duration_count = (0xffffffff - (start_count - end_count)) + 1;
    }
    return HAL_GPT_STATUS_OK;
}

hal_gpt_status_t hal_gpt_get_running_status(hal_gpt_port_t gpt_port, hal_gpt_running_status_t *running_status)
{
    uint32_t index;

    index = gpt_get_context_index(gpt_port);
    
    if (index != HAL_GPT_CONTEXT_INDEX_ERROR) {
        *running_status = g_gpt_context[index].running_status;
        return HAL_GPT_STATUS_OK;
    }

    if (gpt_port == HAL_GPT_MS_PORT) {
        *running_status = gpt_running_state[0];
        return HAL_GPT_STATUS_OK;
    }

    if (gpt_port == HAL_GPT_US_PORT) {
        *running_status = gpt_running_state[1];
        return HAL_GPT_STATUS_OK;
    }

    return HAL_GPT_STATUS_ERROR_PORT;
    
}

hal_gpt_status_t hal_gpt_register_callback(hal_gpt_port_t    gpt_port,
        hal_gpt_callback_t   callback,
        void                *user_data)
{

    uint32_t index;

    index = gpt_get_context_index(gpt_port);
    
    if ((hal_gpt_is_port_valid(gpt_port) != true) || (index == HAL_GPT_CONTEXT_INDEX_ERROR)) {
        return HAL_GPT_STATUS_ERROR_PORT;
    }

    if ((g_gpt_context[index].running_status == HAL_GPT_RUNNING) ||
            (g_gpt_context[index].has_initilized != true)) {

        return HAL_GPT_STATUS_ERROR;
    }

    if (callback == NULL) {
        return HAL_GPT_STATUS_INVALID_PARAMETER;
    }

#ifdef GPT_DEBUG_LOG
    log_hal_msgid_info("[GPT%d] register callback:0x%.8x\r\n",2, (unsigned int)gpt_port, (unsigned int)callback);
#endif
    g_gpt_context[index].callback_context.callback  = callback;
    g_gpt_context[index].callback_context.user_data = user_data;

    gpt_nvic_register();

    return HAL_GPT_STATUS_OK;
}

hal_gpt_status_t hal_gpt_start_timer_ms(hal_gpt_port_t gpt_port, uint32_t timeout_time_ms, hal_gpt_timer_type_t timer_type)
{

    uint32_t mask;
    uint32_t index;

    index = gpt_get_context_index(gpt_port);
    
    if ((hal_gpt_is_port_valid(gpt_port) != true) || (index == HAL_GPT_CONTEXT_INDEX_ERROR)) {
        return HAL_GPT_STATUS_ERROR_PORT;
    }

    if ((g_gpt_context[index].running_status == HAL_GPT_RUNNING)
            || (g_gpt_context[index].has_initilized != true)) {

        return HAL_GPT_STATUS_ERROR_PORT_USED;
    }

    if (timeout_time_ms > HAL_GPT_MAXIMUM_MS_TIMER_TIME) {
        return HAL_GPT_STATUS_INVALID_PARAMETER;
    }
#ifdef GPT_DEBUG_LOG
    if (gpt_port !=  HAL_GPT_SW_MS_PORT) {
        log_hal_msgid_info("[GPT%d]hal_gpt_start_timer_ms, time=%d ms,type=%d\r\n",3, (int)gpt_port, (int)timeout_time_ms, (int)timer_type);
    }
#endif

    hal_nvic_save_and_set_interrupt_mask(&mask);

    GPT(gpt_port)->GPT_IRQ_EN  = 0;               /* disable interrupt */
    GPT(gpt_port)->GPT_CON_UNION.GPT_CON_CELLS.EN &= ~GPT_COUNT_START;            /* stop timer */

    /* set to 32K clock and 1 division,clear counter */
#ifdef FPGA_ENV
    GPT(gpt_port)->GPT_CLK       = GPT_CLOCK_13MHZ | (uint32_t)GPT_DIVIDE_13;
    GPT(gpt_port)->GPT_COMPARE   = (timeout_time_ms * 1000);
#else

    /*note:default clock source is 13M,clear GPT_CLR will take two cycles of 13M,if clock source is 32K,it will
     *take two cycles of 32K,so we need to switch clock to 13M to save more time.
     */
    GPT(gpt_port)->GPT_CLK= GPT_CLOCK_13MHZ | (uint32_t)GPT_DIVIDE_1;
    GPT(gpt_port)->GPT_COMPARE= gpt_convert_ms_to_32k_count(timeout_time_ms);
    while(GPTGLB->GPT_WCOMPSTA & (1 <<gpt_port));
#endif

    GPT(gpt_port)->GPT_IRQ_ACK   = GPT_IRQ_FLAG_ACK;
    GPT(gpt_port)->GPT_CLR       = GPT_COUNT_CLEAR;
    while (GPTGLB->GPT_CLRSTA & (1<<gpt_port));

    /* set  the clock source to 32K.*/
    GPT(gpt_port)->GPT_CLK= GPT_CLOCK_32KHZ | (uint32_t)GPT_DIVIDE_1;

    GPT(gpt_port)->GPT_CON_UNION.GPT_CON = 0;

    /* set to mode, open clock source and start counter */
    if (timer_type) {
        GPT(gpt_port)->GPT_CON_UNION.GPT_CON |= (uint32_t)(GPT_MODE_REPEAT | GPT_COUNT_START);
    } else {
        GPT(gpt_port)->GPT_CON_UNION.GPT_CON |= (uint32_t)(GPT_MODE_ONE_SHOT | GPT_COUNT_START);
    }

    GPT(gpt_port)->GPT_IRQ_EN = GPT_IRQ_ENABLE;
    g_gpt_context[index].running_status = HAL_GPT_RUNNING;
    hal_nvic_restore_interrupt_mask(mask);


    return HAL_GPT_STATUS_OK;
}


ATTR_TEXT_IN_TCM hal_gpt_status_t hal_gpt_delay_ms(uint32_t ms)
{
#ifdef FPGA_ENV
    hal_gpt_delay_us(ms * 1000);
#else
    /* if free run timer is not open, open it */
    if (gpt_running_state[0] != HAL_GPT_RUNNING) {

        /* set clcok source to 1mhz, and start timer */
        gpt_start_free_run_timer(GPT(HAL_GPT_MS_PORT), GPT_CLOCK_32KHZ, GPT_DIVIDE_1);
        gpt_running_state[0] = HAL_GPT_RUNNING;
    }

    gpt_delay_time(GPT(HAL_GPT_MS_PORT), gpt_convert_ms_to_32k_count(ms));
#endif

    return HAL_GPT_STATUS_OK;
}


#ifdef HAL_GPT_FEATURE_US_TIMER
hal_gpt_status_t hal_gpt_start_timer_us(hal_gpt_port_t gpt_port, uint32_t timeout_time_us, hal_gpt_timer_type_t timer_type)
{
    uint32_t mask;
    uint32_t index;

    index = gpt_get_context_index(gpt_port);
    
    if ((hal_gpt_is_port_valid(gpt_port) != true) || (index == HAL_GPT_CONTEXT_INDEX_ERROR)) {
        //log_hal_error("Invalid port: %d. Only port 0 or 1 works as timer.", gpt_port);
        return HAL_GPT_STATUS_ERROR_PORT;
    }

    if ((g_gpt_context[index].running_status == HAL_GPT_RUNNING)
            || (g_gpt_context[index].has_initilized != true)) {

        return HAL_GPT_STATUS_ERROR_PORT_USED;
    }

#ifdef GPT_DEBUG_LOG
    if (gpt_port !=  HAL_GPT_SW_MS_PORT) {
        log_hal_msgid_info("[GPT%d]hal_gpt_start_timer_us, time=%d us,type=%d\r\n",3, (int)gpt_port, (int)timeout_time_us, (int)timer_type);
    }
#endif

    GPT_LOCK_SLEEP(gpt_port);
    g_gpt_context[index].is_gpt_locked_sleep = true;

    hal_nvic_save_and_set_interrupt_mask(&mask);

    GPT(gpt_port)->GPT_IRQ_EN  = 0;               /* disable interrupt */
    GPT(gpt_port)->GPT_CON_UNION.GPT_CON_CELLS.EN &= ~GPT_COUNT_START;            /* stop timer */

    //set to 13MHz clock and 13 division, clear counter,1 us per tick
    GPT(gpt_port)->GPT_CLK       = GPT_CLOCK_13MHZ | (uint32_t)GPT_DIVIDE_13;
    GPT(gpt_port)->GPT_COMPARE   = timeout_time_us;
    GPT(gpt_port)->GPT_IRQ_ACK   = GPT_IRQ_FLAG_ACK;
    GPT(gpt_port)->GPT_CLR       = GPT_COUNT_CLEAR;
    while (GPTGLB->GPT_CLRSTA & (1<<gpt_port));

    GPT(gpt_port)->GPT_CON_UNION.GPT_CON = 0;
    /* set to mode, open clock source and start counter */
    if (timer_type) {
        GPT(gpt_port)->GPT_CON_UNION.GPT_CON |= (uint32_t)(GPT_MODE_REPEAT | GPT_COUNT_START);
    } else {
        GPT(gpt_port)->GPT_CON_UNION.GPT_CON |= (uint32_t)(GPT_MODE_ONE_SHOT | GPT_COUNT_START);
    }
	
    GPT(gpt_port)->GPT_IRQ_EN = GPT_IRQ_ENABLE;
	g_gpt_context[index].running_status = HAL_GPT_RUNNING;

    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPT_STATUS_OK;
}


ATTR_TEXT_IN_TCM hal_gpt_status_t hal_gpt_delay_us(uint32_t us)
{
    /* if free run timer is not open, open it */
    if (gpt_running_state[1] != HAL_GPT_RUNNING) {

        /* set clcok source to 1mhz, and start timer */
        gpt_start_free_run_timer(GPT(HAL_GPT_US_PORT), GPT_CLOCK_13MHZ, GPT_DIVIDE_13);

        gpt_running_state[1] = HAL_GPT_RUNNING;
    }
    gpt_delay_time(GPT(HAL_GPT_US_PORT), us);

    return HAL_GPT_STATUS_OK;
}
#endif /* HAL_GPT_FEATURE_US_TIMER */


hal_gpt_status_t hal_gpt_stop_timer(hal_gpt_port_t gpt_port)
{
    uint32_t mask;
    uint32_t index;

    index = gpt_get_context_index(gpt_port);
    
    if ((hal_gpt_is_port_valid(gpt_port) != true) || (index == HAL_GPT_CONTEXT_INDEX_ERROR)) {
        return HAL_GPT_STATUS_ERROR_PORT;
    }

#ifdef GPT_DEBUG_LOG
    if (gpt_port !=  HAL_GPT_SW_MS_PORT) {
        log_hal_msgid_info("[GPT%d]hal_gpt_stop_timer\r\n",1, (int)gpt_port);
    }
#endif

    hal_nvic_save_and_set_interrupt_mask(&mask);

    /*diable interrupt*/
    GPT(gpt_port)->GPT_IRQ_EN &= ~GPT_IRQ_ENABLE;

    /* stop timer */
    GPT(gpt_port)->GPT_CON_UNION.GPT_CON_CELLS.EN &= ~GPT_COUNT_START;
    GPT(gpt_port)->GPT_IRQ_ACK = GPT_IRQ_FLAG_ACK;

    g_gpt_context[index].running_status = HAL_GPT_STOPPED;
    hal_nvic_restore_interrupt_mask(mask);

    if (g_gpt_context[index].is_gpt_locked_sleep == true) {
        GPT_UNLOCK_SLEEP(gpt_port);
        g_gpt_context[index].is_gpt_locked_sleep = false;
    }
    return HAL_GPT_STATUS_OK;
}

/**************** software timer API for  multi-User *************************/
volatile sw_gpt_add_note_t g_sw_gpt_notes;
uint32_t g_sw_gpt_running_note[SW_GPT_MAX_TYPE] = {HAL_SW_GPT_MAX_USERS,HAL_SW_GPT_MAX_USERS};
volatile bool sw_gpt_first_start=false;



hal_gpt_status_t hal_gpt_sw_get_timer(uint32_t *handle)
{
    uint32_t i,mask;
    uint32_t gpt_ms_index , gpt_us_index ;
    static sw_gpt_type_t sw_type_port[HAL_GPI_SW_TYPE_NUMBER];
    sw_type_port[0] = SW_GPT_MS_TYPE;
    sw_type_port[1] = SW_GPT_US_TYPE;
    if (handle == NULL){
         return HAL_GPT_STATUS_INVALID_PARAMETER;
    }
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (g_sw_gpt_notes.used_timer_count >= HAL_SW_GPT_MAX_USERS) {
         hal_nvic_restore_interrupt_mask(mask);
         return HAL_GPT_STATUS_ERROR_PORT_USE_FULL;
    }
    for(i=0;i<HAL_SW_GPT_MAX_USERS;i++)
    {
        if(g_sw_gpt_notes.sw_gpt_timer[i].status == SW_GPT_NOTE_STATUS_OF_FREE)  //get a free_status timer.
        {
           *handle = i | SW_GPT_HANDLE_MAGIC_NUMBER;
           g_sw_gpt_notes.sw_gpt_timer[i].status = SW_GPT_NOTE_STATUS_OF_ALLOC;
           g_sw_gpt_notes.sw_gpt_timer[i].expire_count = 0x0;
           g_sw_gpt_notes.sw_gpt_timer[i].callback = NULL;
           g_sw_gpt_notes.sw_gpt_timer[i].user_data = NULL;
           break;
        }
    }
    g_sw_gpt_notes.used_timer_count++;
    hal_nvic_restore_interrupt_mask(mask);
    if(sw_gpt_first_start==false){
    GPT_GET_SLEEP_HANDLE(HAL_GPT_SW_US_PORT);
    }
    hal_nvic_save_and_set_interrupt_mask(&mask);
    /*judge if the sw_gpt is first start, there will assign the hw-gpt-callback  sw_gpt_callback  and
       register the GPT_IRQn irq_handler */
    if(sw_gpt_first_start==false){
       sw_gpt_first_start= true;
       gpt_ms_index = gpt_get_context_index(HAL_GPT_SW_MS_PORT);
       g_gpt_context[gpt_ms_index].callback_context.callback  = (hal_gpt_callback_t)sw_gpt_callback;
       g_gpt_context[gpt_ms_index].callback_context.user_data = (void*)(&sw_type_port[0]);
       gpt_us_index = gpt_get_context_index(HAL_GPT_SW_US_PORT);
       g_gpt_context[gpt_us_index].callback_context.callback  = (hal_gpt_callback_t)sw_gpt_callback;
       g_gpt_context[gpt_us_index].callback_context.user_data = (void*)(&sw_type_port[1]);
       gpt_nvic_register();
    }
    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPT_STATUS_OK;
}

hal_gpt_status_t hal_gpt_sw_free_timer(uint32_t handle)
{
    uint32_t index;
    uint32_t mask;
    if ((handle & SW_GPT_HANDLE_MAGIC_NUMBER) != SW_GPT_HANDLE_MAGIC_NUMBER) {
        return HAL_GPT_STATUS_INVALID_PARAMETER;
    } 
    index= SW_GPT_HANDLE_TO_INDEX(handle);
    if (g_sw_gpt_notes.sw_gpt_timer[index].status != SW_GPT_NOTE_STATUS_OF_ALLOC) {
        return HAL_GPT_STATUS_ERROR;
    }
    hal_nvic_save_and_set_interrupt_mask(&mask);
    g_sw_gpt_notes.sw_gpt_timer[index].status = SW_GPT_NOTE_STATUS_OF_FREE;
    g_sw_gpt_notes.used_timer_count--;
    hal_nvic_restore_interrupt_mask(mask);
    return HAL_GPT_STATUS_OK;
}

hal_gpt_status_t hal_gpt_sw_start_timer_ms(uint32_t handle, uint32_t timeout_time_ms, hal_gpt_callback_t callback, void *user_data)
{
    return sw_gpt_timer_start(handle, timeout_time_ms, callback,user_data, SW_GPT_MS_TYPE);

}
hal_gpt_status_t hal_gpt_sw_stop_timer_ms(uint32_t handle)
{
    return sw_gpt_timer_stop(handle,SW_GPT_MS_TYPE);
}

hal_gpt_status_t hal_gpt_sw_get_remaining_time_ms(uint32_t handle, uint32_t *remaing_time)
{
    return sw_gpt_get_remaining_time(handle,remaing_time,SW_GPT_MS_TYPE);
}


hal_gpt_status_t hal_gpt_sw_start_timer_us(uint32_t handle, uint32_t timeout_time_us, hal_gpt_callback_t callback, void *user_data)
{
    return sw_gpt_timer_start(handle, timeout_time_us, callback,user_data, SW_GPT_US_TYPE);

}

hal_gpt_status_t hal_gpt_sw_stop_timer_us(uint32_t handle)
{
    return sw_gpt_timer_stop(handle,SW_GPT_US_TYPE);
}

hal_gpt_status_t hal_gpt_sw_get_remaining_time_us(uint32_t handle, uint32_t *remaing_time)
{
    return sw_gpt_get_remaining_time(handle,remaing_time,SW_GPT_US_TYPE);
}

#ifdef HAL_GPT_SW_FEATURE_ABSOLUTE_COUNT
hal_gpt_status_t hal_gpt_sw_start_timer_for_absolute_tick_32K(uint32_t handle , hal_sw_gpt_absolute_parameter_t *absolute_parameter)
{
    return sw_gpt_start_timer_for_absolute_tick(handle,absolute_parameter,SW_GPT_MS_TYPE);
}
hal_gpt_status_t hal_gpt_sw_start_timer_for_absolute_tick_1M(uint32_t handle , hal_sw_gpt_absolute_parameter_t *absolute_parameter)
{
    return sw_gpt_start_timer_for_absolute_tick(handle,absolute_parameter,SW_GPT_US_TYPE);
}
#endif //HAL_GPT_SW_FEATURE_ABSOLUTE_COUNT

#endif //HAL_GPT_MODULE_ENABLED


