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

#ifdef HAL_GPT_MODULE_ENABLED
#include "hal_gpt_internal.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_log.h"
#include "memory_attribute.h"
//#include "hal_clock.h"
//#include "hal_clock_internal.h"
#include <assert.h>
#include "hal_clock.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#include "hal_sleep_manager_platform.h"
#include "hal_rtc.h"

ATTR_ZIDATA_IN_TCM gpt_context_t g_gpt_context[HAL_GPT_SUPPORT_INT_CHANNEL];
ATTR_ZIDATA_IN_TCM hal_gpt_running_status_t gpt_running_state[HAL_GPT_SUPPORT_DELAY_CHANNEL];
const hal_gpt_port_t        gpt_context_mapping[HAL_GPT_SUPPORT_INT_CHANNEL] = {HAL_GPT_SUPPORT_GPT_MAPPING};
extern const char *gpt_lock_sleep_name[HAL_GPT_MAX_PORT];
extern uint8_t gpt_lock_sleep_handle[HAL_GPT_MAX_PORT];


uint32_t gpt_get_context_index(hal_gpt_port_t gpt_port)
{
    uint32_t i;
    
    for(i=0;i<HAL_GPT_SUPPORT_INT_CHANNEL;i++) {
        if(gpt_port == gpt_context_mapping[i]) {
            return i;
        }
    }
    return HAL_GPT_CONTEXT_INDEX_ERROR;
}
ATTR_TEXT_IN_TCM uint32_t gpt_current_count(GPT_REGISTER_T *gpt)
{
    return gpt->GPT_COUNT;

}

ATTR_TEXT_IN_TCM uint32_t gpt_convert_ms_to_32k_count(uint32_t ms)
{
    uint32_t tick_per_second = 32768;

    hal_rtc_get_f32k_frequency(&tick_per_second);
    return (uint32_t)(ms * (tick_per_second / 1000.0f));
}


ATTR_TEXT_IN_TCM void gpt_open_clock_source(void)
{

    if (hal_clock_is_enabled(HAL_CLOCK_CG_GPTIMER) == false) {
        hal_clock_enable(HAL_CLOCK_CG_GPTIMER);
    }
}


ATTR_TEXT_IN_TCM void  gpt_start_free_run_timer(GPT_REGISTER_T *gpt, uint32_t clock_source, uint32_t divide)
{
    gpt_open_clock_source();

    gpt->GPT_CLK = clock_source | divide;

    /* set to free run mode, open clock source and start counter */
    gpt->GPT_CON_UNION.GPT_CON = (uint32_t)GPT_CLOCK_UNGATE | (uint32_t)GPT_MODE_FREE_RUN | GPT_COUNT_START;

}

ATTR_TEXT_IN_TCM void gpt_delay_time(GPT_REGISTER_T *gpt, const uint32_t count)
{
    uint32_t current = 0;
    uint32_t  temp0 = 0;
    uint32_t  temp1 = 0;
    current = gpt_current_count(gpt);

    while (temp1 <= count){
        temp0 = gpt_current_count(gpt);

        if (temp0 > current) {
            temp1 = temp0 - current;
         } else {
            temp1= (0xffffffff - (current - temp0)) + 1;
        }
    }
}



void gpt_reset_default_timer(uint32_t gpt_port)
{
    GPT(gpt_port)->GPT_IRQ_EN = 0;                       /* disable interrupt */
    GPT(gpt_port)->GPT_CON_UNION.GPT_CON = 0;            /* disable timer     */
    GPT(gpt_port)->GPT_CLR = GPT_COUNT_CLEAR;            /* clear counter value */

    while (GPTGLB->GPT_CLRSTA & (1<<gpt_port));
    GPT(gpt_port)->GPT_CLK     = GPT_CLOCK_32KHZ;  /* default 32Mhz, divide 1 */
    GPT(gpt_port)->GPT_IRQ_ACK = GPT_IRQ_FLAG_ACK; /* clear interrupt status */
    GPT(gpt_port)->GPT_COMPARE = 0xffffffff;       /* set max threshold value */

}



uint32_t gpt_save_and_mask_interrupt(GPT_REGISTER_T *gpt)
{
    volatile uint32_t mask;

    mask = gpt->GPT_IRQ_EN;

    gpt->GPT_IRQ_EN  &= ~GPT_IRQ_ENABLE;

    return mask;
}

void gpt_restore_interrupt(GPT_REGISTER_T *gpt, uint32_t mask)
{
    gpt->GPT_IRQ_EN = mask;
}


void gpt_interrupt_handler(hal_nvic_irq_t irq_number)
{
    uint32_t i,index;
    volatile uint32_t mask;
    volatile uint32_t state;
    volatile uint32_t enable;

    irq_number = irq_number;

    for (i = 0; i < HAL_GPT_MAX_PORT; i++) {
        index = gpt_get_context_index(i);
        if (index == HAL_GPT_CONTEXT_INDEX_ERROR) {
            continue;
        }
        
        /* get gpt irq status */
        state  = GPT(i)->GPT_IRQ_STA;
        enable = GPT(i)->GPT_IRQ_EN;

        mask = gpt_save_and_mask_interrupt(GPT(i));

        if ((state & GPT_IRQ_FLAG_STA) && (enable & GPT_IRQ_ENABLE)) {
            /* clear interrupt status */
            GPT(i)->GPT_IRQ_ACK = GPT_IRQ_FLAG_ACK;
#ifdef GPT_DEBUG_LOG
            if (i != HAL_GPT_SW_MS_PORT) {
                //    log_hal_info("[GPT%d]GPT_IRQ_STA = 0x%x\r\n", (int)i, (int)GPT(i)->GPT_IRQ_STA);
            }
#endif

            if (g_gpt_context[index].callback_context.callback != NULL) {
                g_gpt_context[index].callback_context.callback(g_gpt_context[index].callback_context.user_data);
            }

        }
        gpt_restore_interrupt(GPT(i), mask);

    }
}



void gpt_nvic_register(void)
{
    static bool is_init = false;

    if (is_init == false) {
        hal_nvic_disable_irq(GPT_IRQn);
        hal_nvic_register_isr_handler(GPT_IRQn, gpt_interrupt_handler);
        hal_nvic_enable_irq(GPT_IRQn);
        is_init = true;

    }
}

/**************** software timer for intenal api  for multi-User************************/
extern sw_gpt_add_note_t g_sw_gpt_notes;
extern uint32_t g_sw_gpt_running_note[];
bool sw_gpt_us_locksleep_status=false;

ATTR_TEXT_IN_TCM void sw_gpt_find_the_next_expire_time(uint32_t current_count,uint32_t *p_index, uint32_t *p_expire_count,sw_gpt_type_t sw_type)
{
    uint32_t i,temp_count;
    uint32_t the_minimum_count = 0xFFFFFFFF,the_minimum_note = HAL_SW_GPT_MAX_USERS;
    for(i=0;i<HAL_SW_GPT_MAX_USERS;i++){
        if((g_sw_gpt_notes.sw_gpt_timer[i].status == SW_GPT_NOTE_STATUS_OF_RUNING) && (g_sw_gpt_notes.sw_gpt_timer[i].timer_port_type == sw_type)){
            temp_count = g_sw_gpt_notes.sw_gpt_timer[i].expire_count - current_count;
            if(temp_count<0x7FFFFFFF){
                if(the_minimum_count>temp_count){
                    the_minimum_count = temp_count;
                    the_minimum_note = i;
                }
            }else{
                the_minimum_count = 0;
                the_minimum_note = i;
                break;
            }
        }
    }
    *p_index = the_minimum_note;
    *p_expire_count= the_minimum_count;
    return;
}

void sw_gpt_callback(sw_gpt_type_t *sw_gpt_type_port)
{
    uint32_t i,mask,current_count=0;
    uint32_t next_index,next_count;
    hal_gpt_callback_t temp_callback;
    void * temp_user_data;
    hal_gpt_port_t gpt_port=HAL_GPT_MAX_PORT;
    sw_gpt_type_t sw_gpt_type=*sw_gpt_type_port;
    g_sw_gpt_running_note[sw_gpt_type] = HAL_SW_GPT_MAX_USERS;    /*assigned a global variable to check if any irq are coming in after running callback  */
    uint32_t expired_flag;
Reget_current_count:
    if(sw_gpt_type == SW_GPT_MS_TYPE ){
        gpt_port = HAL_GPT_SW_MS_PORT;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&current_count);
    }
    if(sw_gpt_type == SW_GPT_US_TYPE){
        gpt_port = HAL_GPT_SW_US_PORT;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&current_count);
    }
    expired_flag=false;
    for(i=0;i<HAL_SW_GPT_MAX_USERS;i++){
        /*Why disable IRQ??? To prevent the higher priority irq from rushing in when switch the timer status,because if the irq function call 
         get sw gpt handle may get current timer handle ,which will lead current timer callback is assigned a NULL. when irq return ,the
         system will crash. */
         hal_nvic_save_and_set_interrupt_mask(&mask);//re-start again
        if((g_sw_gpt_notes.sw_gpt_timer[i].status == SW_GPT_NOTE_STATUS_OF_RUNING)      && 
                (g_sw_gpt_notes.sw_gpt_timer[i].timer_port_type == sw_gpt_type)    &&
                (current_count-g_sw_gpt_notes.sw_gpt_timer[i].expire_count <0x7FFFFFFF) ){
                g_sw_gpt_notes.sw_gpt_timer[i].status = SW_GPT_NOTE_STATUS_OF_ALLOC;
                temp_callback = g_sw_gpt_notes.sw_gpt_timer[i].callback;
                temp_user_data = g_sw_gpt_notes.sw_gpt_timer[i].user_data;
                expired_flag = true;
                hal_nvic_restore_interrupt_mask(mask);
                /*Why use local variable??? use local variable will ensure the callback function is run when the higher priority irq rush in. */
                temp_callback(temp_user_data);
           }else
                hal_nvic_restore_interrupt_mask(mask);
    }
    if(expired_flag == true)
    goto Reget_current_count;
    hal_nvic_save_and_set_interrupt_mask(&mask);//re-start again
    /*Why judge whether == HAL_SW_GPT_MAX_USERS ????? because enable irq above,if the higher pirorty irq function start a timer and the gpt-running-note
    is modified . When the interrupt returns , there is no need to find next-index to start timer   */
    if(g_sw_gpt_running_note[sw_gpt_type] == HAL_SW_GPT_MAX_USERS){
        sw_gpt_find_the_next_expire_time(current_count,&next_index,&next_count,sw_gpt_type);
        if(next_index == HAL_SW_GPT_MAX_USERS){//means all notes have been expired
            if(sw_gpt_type==SW_GPT_US_TYPE){
               sw_gpt_us_locksleep_status=false;
               GPT_UNLOCK_SLEEP(gpt_port);
            }
            hal_nvic_restore_interrupt_mask(mask);
            return ;
        }
        g_sw_gpt_running_note[sw_gpt_type] = next_index; // update the running index
        clear_gpt_and_restart_hw_gpt(next_count,gpt_port);
    }
    hal_nvic_restore_interrupt_mask(mask);
}


ATTR_TEXT_IN_TCM void clear_gpt_and_restart_hw_gpt(uint32_t next_expired_count, hal_gpt_port_t port)
{   
    uint32_t mask;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    gpt_open_clock_source();
    GPT(port)->GPT_CON_UNION.GPT_CON &= ~(GPT_CLOCK_GATE);  /*enable the gpt clock*/
    GPT(port)->GPT_IRQ_EN  = 0;               /* disable interrupt */
    GPT(port)->GPT_CON_UNION.GPT_CON_CELLS.EN &= ~GPT_COUNT_START;         /* stop timer */
    /*note:default clock source is 13M,clear GPT_CLR will take two cycles of 13M,if clock source is 32K,it will
       take two cycles of 32K,so we need to switch clock to 13M to save more time.
     */
    GPT(port)->GPT_CLK= GPT_CLOCK_13MHZ | (uint32_t) GPT_DIVIDE_1; 
    GPT(port)->GPT_COMPARE= next_expired_count;
    while(GPTGLB->GPT_WCOMPSTA & (1 << port));/* need to wait for finishing write the compare value to compare reg  */
    GPT(port)->GPT_IRQ_ACK   = GPT_IRQ_FLAG_ACK; /* clear the irq status*/
    GPT(port)->GPT_CLR       = GPT_COUNT_CLEAR;  /*clear the gpt count reg value*/
    while (GPTGLB->GPT_CLRSTA & (1<< port));
    /* set  the clock source to 32K.*/
   
    if(port == HAL_GPT_SW_MS_PORT){
        GPT(port)->GPT_CLK= GPT_CLOCK_32KHZ | (uint32_t)GPT_DIVIDE_1;
     }
    if(port == HAL_GPT_SW_US_PORT){
         GPT(port)->GPT_CLK= GPT_CLOCK_13MHZ | (uint32_t)GPT_DIVIDE_13;
     }
    
    GPT(port)->GPT_CON_UNION.GPT_CON &= ~(3<<8);
    GPT(port)->GPT_CON_UNION.GPT_CON_CELLS.EN |= GPT_COUNT_START;     
    GPTGLB->GPT_IRQMSK0    &= ~(1<<port);  //CM4 IRQ enable
    GPTGLB->GPT_WAKEUPMSK0 &= ~(1<<port);  //CM4 wakeup enable
    GPT(port)->GPT_IRQ_EN = GPT_IRQ_ENABLE;
    
    
    
    hal_nvic_restore_interrupt_mask(mask);
    return;
}


#define SW_GPT_Max_Start_Time_ms 65535999
#define SW_GPT_Max_Start_Time_us 2147483647

hal_gpt_status_t sw_gpt_timer_start(uint32_t handle, uint32_t timeout_time, hal_gpt_callback_t callback, void *user_data, sw_gpt_type_t sw_type)
{ 
    uint32_t index = SW_GPT_HANDLE_TO_INDEX(handle);
    uint32_t current_count=0,expire_count,timeout_tick=0,mask;
    uint32_t next_index,next_count;
    hal_gpt_port_t gpt_port = HAL_GPT_MAX_PORT;
    if(callback == NULL){
      return HAL_GPT_STATUS_INVALID_PARAMETER;
    }
    if((handle & SW_GPT_HANDLE_MAGIC_NUMBER)!= SW_GPT_HANDLE_MAGIC_NUMBER ){
      return HAL_GPT_STATUS_INVALID_PARAMETER;
    }  
    /* According to the adder principle, when the count value is less than 0x7FFFFFFF,Interval time = expiration time - start time ,
     don't worry about the timeline expiration reset*/
    if(timeout_time > SW_GPT_Max_Start_Time_ms && sw_type == SW_GPT_MS_TYPE){
        return HAL_GPT_STATUS_ERROR_START_TOO_LONG;
    }
    if(timeout_time > SW_GPT_Max_Start_Time_us && sw_type == SW_GPT_US_TYPE ){
        return HAL_GPT_STATUS_ERROR_START_TOO_LONG; 
    }

    /* Why no need disable IRQ???? no race condition??? Because they all use local variables, there are no atomic operations*/
    if(sw_type == SW_GPT_MS_TYPE){  
       gpt_port = HAL_GPT_SW_MS_PORT;
       hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&current_count);
       timeout_tick = gpt_convert_ms_to_32k_count(timeout_time);
    }
    if(sw_type == SW_GPT_US_TYPE){
       gpt_port = HAL_GPT_SW_US_PORT;
       if(sw_gpt_us_locksleep_status==false){
       GPT_LOCK_SLEEP(gpt_port);
       sw_gpt_us_locksleep_status=true;
       }
       hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&current_count);
       timeout_tick = timeout_time;
    }
    expire_count = current_count+timeout_tick;
    
    /* step1: check this notes_gpt_timer status whether is SW_GPT_NOTE_STATUS_OF_ALLOC*/
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if(g_sw_gpt_notes.sw_gpt_timer[index].status != SW_GPT_NOTE_STATUS_OF_ALLOC){
      hal_nvic_restore_interrupt_mask(mask);
      return HAL_GPT_STATUS_ERROR_RESTART_ERROR;
    }
    /*step2: push the note to global variables  */
    g_sw_gpt_notes.sw_gpt_timer[index].status=SW_GPT_NOTE_STATUS_OF_RUNING;
    g_sw_gpt_notes.sw_gpt_timer[index].expire_count = expire_count;
    g_sw_gpt_notes.sw_gpt_timer[index].callback = callback;
    g_sw_gpt_notes.sw_gpt_timer[index].user_data = user_data;
    g_sw_gpt_notes.sw_gpt_timer[index].timer_port_type = sw_type;
    /* step3: find the next expire timer  and re-start the gpt*/
    sw_gpt_find_the_next_expire_time(current_count,&next_index,&next_count,sw_type);
    if(next_index == HAL_SW_GPT_MAX_USERS){ 
        assert(0);
    }
     /* means the next timer's expire count  is less than the running timer's expire count
     so should re-start the next timer */
    if(next_index != g_sw_gpt_running_note[sw_type]){
        g_sw_gpt_running_note[sw_type] = next_index;
        clear_gpt_and_restart_hw_gpt(next_count,gpt_port); // restart the gpt.
    }
    hal_nvic_restore_interrupt_mask(mask);
    return HAL_GPT_STATUS_OK;
}

hal_gpt_status_t sw_gpt_timer_stop(uint32_t handle,sw_gpt_type_t sw_type)
{
    uint32_t index = SW_GPT_HANDLE_TO_INDEX(handle);
    uint32_t mask;
    uint32_t current_count =0, next_index ,next_count;
    hal_gpt_status_t ret_status;
    hal_gpt_port_t gpt_port= HAL_GPT_MAX_PORT;
    
    if((handle & SW_GPT_HANDLE_MAGIC_NUMBER)!= SW_GPT_HANDLE_MAGIC_NUMBER ){
      return HAL_GPT_STATUS_INVALID_PARAMETER;
    }  

    if(sw_type == SW_GPT_MS_TYPE ){
        gpt_port = HAL_GPT_SW_MS_PORT;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&current_count);
    }
    if(sw_type == SW_GPT_US_TYPE){
        gpt_port = HAL_GPT_SW_US_PORT;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&current_count);
    }
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if(g_sw_gpt_notes.sw_gpt_timer[index].status == SW_GPT_NOTE_STATUS_OF_RUNING){
        g_sw_gpt_notes.sw_gpt_timer[index].status = SW_GPT_NOTE_STATUS_OF_ALLOC;       
       /* find the next index and culcalate the next_count that is count between curent count and  the lastest expire count,
       if  the next timer's expire count is less than the running timer's expire count so should re-start the next timer*/
       sw_gpt_find_the_next_expire_time(current_count,&next_index,&next_count,sw_type);
       if(next_index == HAL_SW_GPT_MAX_USERS){//mean all timer have been expired,and don't re-start gpt.
           if(sw_type==SW_GPT_US_TYPE){
                sw_gpt_us_locksleep_status=false;
                GPT_UNLOCK_SLEEP(gpt_port);
           }
           g_sw_gpt_running_note[sw_type]=HAL_SW_GPT_MAX_USERS;
           hal_nvic_restore_interrupt_mask(mask);
           return HAL_GPT_STATUS_OK;
       }
       if(g_sw_gpt_running_note[sw_type] != next_index){
           /*restart the gpt*/
           clear_gpt_and_restart_hw_gpt(next_count,gpt_port);
           g_sw_gpt_running_note[sw_type] = next_index;
       }
       ret_status = HAL_GPT_STATUS_OK;
       hal_nvic_restore_interrupt_mask(mask);
    }else{
        ret_status =HAL_GPT_STATUS_ERROR;
        hal_nvic_restore_interrupt_mask(mask);
    }
    return ret_status;
}

hal_gpt_status_t sw_gpt_get_remaining_time(uint32_t handle,uint32_t *remain_time,sw_gpt_type_t sw_type)
{
    uint32_t index = SW_GPT_HANDLE_TO_INDEX(handle);
    uint32_t mask;
    uint32_t current_count = 0;
    uint32_t remain_count,time_s,time_ms,remain_time_ms,remain_time_us,expire_count;
   
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (g_sw_gpt_notes.sw_gpt_timer[index].status != SW_GPT_NOTE_STATUS_OF_RUNING) {
        hal_nvic_restore_interrupt_mask(mask);
        return HAL_GPT_STATUS_ERROR;
    }
    expire_count = g_sw_gpt_notes.sw_gpt_timer[index].expire_count;
    hal_nvic_restore_interrupt_mask(mask);

    if (sw_type == SW_GPT_MS_TYPE) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&current_count);
        remain_count=expire_count-current_count;
        /* current handle have not expired*/
        if(remain_count < 0x7FFFFFFF){
            time_s = remain_count / 32768;
            time_ms = ((remain_count % 32768) * 1000 + 16384) / 32768;
            remain_time_ms = time_s * 1000 + time_ms;
            *remain_time =remain_time_ms;
            return HAL_GPT_STATUS_OK;
        }
    }
    if(sw_type == SW_GPT_US_TYPE) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&current_count);
        remain_count=expire_count-current_count;
        /* current handle have not expired*/
        if(remain_count < 0x7FFFFFFF){ 
            remain_time_us =remain_count;
            *remain_time = remain_time_us;
            return HAL_GPT_STATUS_OK;
        }
    }
    *remain_time = 0;
     return HAL_GPT_STATUS_OK;
}

hal_gpt_status_t sw_gpt_start_timer_for_absolute_tick(uint32_t handle , hal_sw_gpt_absolute_parameter_t *absolute_parameter,sw_gpt_type_t sw_type)
{
    uint32_t index = SW_GPT_HANDLE_TO_INDEX(handle);
    hal_sw_gpt_absolute_parameter_t *p_temp=absolute_parameter;
    uint32_t current_count = 0,mask;
    uint32_t next_index,next_count;
    hal_gpt_port_t gpt_port = HAL_GPT_MAX_PORT;
    if(p_temp->callback == NULL){
      return HAL_GPT_STATUS_INVALID_PARAMETER;
    }
    if((handle & SW_GPT_HANDLE_MAGIC_NUMBER)!= SW_GPT_HANDLE_MAGIC_NUMBER ){
      return HAL_GPT_STATUS_INVALID_PARAMETER;
    }  
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if(sw_type == SW_GPT_MS_TYPE){  
       gpt_port = HAL_GPT_SW_MS_PORT;
       hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&current_count);
    }
    if(sw_type == SW_GPT_US_TYPE){
       gpt_port = HAL_GPT_SW_US_PORT;
       if(sw_gpt_us_locksleep_status==false){
       GPT_LOCK_SLEEP(gpt_port);
       sw_gpt_us_locksleep_status=true;
       }
       hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&current_count);
    }
    /*use the  passed parameter by User to catch the absolute_time that has expired within a time of resignation*/
    if(p_temp->absolute_time_count-current_count+ p_temp->maxdelay_time_count >0x7FFFFFFF)
    {
        hal_nvic_restore_interrupt_mask(mask);
        return HAL_GPT_STATUS_ERROR;
    }
    hal_nvic_restore_interrupt_mask(mask);
      /* step1: check this notes_gpt_timer status whether is SW_GPT_NOTE_STATUS_OF_ALLOC*/
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if(g_sw_gpt_notes.sw_gpt_timer[index].status != SW_GPT_NOTE_STATUS_OF_ALLOC){
      hal_nvic_restore_interrupt_mask(mask);
      return HAL_GPT_STATUS_ERROR_RESTART_ERROR;
    }
    /*step2: push the note to global variables  */
    g_sw_gpt_notes.sw_gpt_timer[index].status=SW_GPT_NOTE_STATUS_OF_RUNING;
    g_sw_gpt_notes.sw_gpt_timer[index].expire_count = p_temp->absolute_time_count;
    g_sw_gpt_notes.sw_gpt_timer[index].callback = p_temp->callback;
    g_sw_gpt_notes.sw_gpt_timer[index].user_data = p_temp->user_data;
    g_sw_gpt_notes.sw_gpt_timer[index].timer_port_type = sw_type;
    /* step3: find the next expire timer  and re-start the gpt*/
    sw_gpt_find_the_next_expire_time(current_count,&next_index,&next_count,sw_type);
    if(next_index == HAL_SW_GPT_MAX_USERS){ 
        assert(0);
    }
     /* means the next timer's expire count  is less than the running timer's expire count
     so should re-start the next timer */
    if(next_index != g_sw_gpt_running_note[sw_type]){
        g_sw_gpt_running_note[sw_type] = next_index;
        clear_gpt_and_restart_hw_gpt(next_count,gpt_port); // restart the gpt.
    }
    hal_nvic_restore_interrupt_mask(mask);
    return HAL_GPT_STATUS_OK;

}

#endif /* HAL_GPT_MODULE_ENABLED */


