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
#include "hal_captouch.h"
#ifdef HAL_CAPTOUCH_MODULE_ENABLED
#include "hal_captouch_internal.h"
#include "hal_nvic.h"
#include "hal_gpt.h"
#include "hal_clock.h"
#include "hal_rtc.h"
#include "hal_rtc_internal.h"
#include "nvkey_id_list.h"
#include "nvkey.h"

#if defined(AB1565)
#include "hal_pmu_charger_2565.h"
#include "assert.h"
#endif

#define UNUSED(p) ((void)(p))
#define captouch_rtc

hal_captouch_sdwu_nvdm_data sdwu_setting;
hal_captouch_eardetect_nvdm_data eardetect_setting ;
hal_captouch_autosuspend_timeout_nvdm_data autosuspend_timeout_data;

captouch_buffer_t captouch_buffer;
captouch_context_t captouch_context;
captouch_ear_detect_t captouch_ear_detect;
captouch_autosuspend_timeout_context_t autosuspend_timeout_context;
const uint8_t captouch_mapping_keydata[] = {KEYPAD_MAPPING};
CAPTOUCH_REGISTER_T *captouch = (CAPTOUCH_REGISTER_T *)(CAPTOUCH_BASE);
CAPTOUCH_REGISTERHIF_T *captouch_hif = ((CAPTOUCH_REGISTERHIF_T*)CAPTOUCH_HIFBASE);


// captouch temp event
captouch_event_temp_t captouch_event_temp;

TimerHandle_t captouch_fine_base_delay100ms_timer;
TimerHandle_t captouch_earcheck_timer;
TimerHandle_t captouch_earcheck_stop_baseK_timer;

uint32_t tune_delay_us, det_delay_us;
uint32_t captouch_close_autosuspend_timer[CAPTOUCH_CHANNEL_MAX];
uint32_t captouch_delay_timer[CAPTOUCH_CHANNEL_MAX];
uint32_t event_time_stamp[CAPTOUCH_CHANNEL_MAX];
uint32_t captouch_debounce_timer[CAPTOUCH_CHANNEL_MAX];
uint8_t noise_cnt_race[CAPTOUCH_CHANNEL_MAX] = {0, 0, 0, 0};

bool gfirstTouch_flag[CAPTOUCH_CHANNEL_MAX];
volatile bool gsend_flag[CAPTOUCH_CHANNEL_MAX];
bool lpwu_press = false;

void captouch_push_one_event_to_buffer(uint32_t channel,hal_captouch_key_state_t state,uint32_t time_stamp)
{

    if (captouch_context.has_initilized != true) {
        return;
    }

    captouch_buffer.data[captouch_buffer.write_index].state         = state;
    captouch_buffer.data[captouch_buffer.write_index].key_data       = channel;
    captouch_buffer.data[captouch_buffer.write_index].time_stamp    = time_stamp;
    captouch_buffer.write_index++;
    captouch_buffer.write_index &= (CAPTOUCH_BUFFER_SIZE - 1);

}

void captouch_pop_one_event_from_buffer(hal_captouch_event_t *key_event)
{

    key_event->state     = captouch_buffer.data[captouch_buffer.read_index].state;
    key_event->key_data  = captouch_buffer.data[captouch_buffer.read_index].key_data;
    key_event->time_stamp= captouch_buffer.data[captouch_buffer.read_index].time_stamp;
    captouch_buffer.read_index++;
    captouch_buffer.read_index &= (CAPTOUCH_BUFFER_SIZE - 1);

}

uint32_t captouch_get_buffer_left_size(void)
{
    if (captouch_buffer.write_index >= captouch_buffer.read_index ) {
        return (CAPTOUCH_BUFFER_SIZE - (captouch_buffer.write_index - captouch_buffer.read_index));
    } else {
        return (captouch_buffer.read_index - captouch_buffer.write_index);
    }
}

uint32_t captouch_get_buffer_data_size(void)
{
    return (CAPTOUCH_BUFFER_SIZE - captouch_get_buffer_left_size());
}

bool captouch_get_event_from_fifo(uint32_t *event,uint32_t *timestap)
{
    if (captouch_hif->TOUCH_HIF_CON0.CELLS.FIFO_OVERFLOW) {
        HAL_CAPTOUCH_LOG_PRINT("event overflow occured\r\n",0);
    }

    if (captouch_hif->TOUCH_HIF_CON1.CELLS.EVENT_PENDING == 1) {

        /*read timestamp*/
        *timestap = captouch_hif->TOUCH_HIF_CON2;

        /*read event type*/
        *event    = (uint32_t)captouch_hif->TOUCH_HIF_CON1.CELLS.EVENT_TYPE;

        /*pop event*/
        captouch_hif->TOUCH_HIF_CON1.CELLS.EVENT_POP = 1;

        return true;
    }
    else {
        return false;
    }

}

void captouch_interrupt_handler(hal_nvic_irq_t irq_number)
{
    uint32_t event;
    uint32_t time_stamp;
    hal_captouch_key_state_t state;
    hal_captouch_channel_t channel;

    while (captouch_hif->TOUCH_HIF_CON1.CELLS.EVENT_PENDING) {
        if (captouch_hif->TOUCH_HIF_CON0.CELLS.FIFO_OVERFLOW) {
            //log_hal_info("event overflow occured\r\n");
            HAL_CAPTOUCH_LOG_PRINT("captouch event overflow",0);
            captouch_hif->TOUCH_HIF_CON0.CELLS.HIF_SOFT_RST = 1;
            hal_gpt_delay_us(10);
            break;
        }
        /*read timestamp*/
        time_stamp = captouch_hif->TOUCH_HIF_CON2;

        /*read event type*/
        event    = (uint32_t)captouch_hif->TOUCH_HIF_CON1.CELLS.EVENT_TYPE;
        if (event <= 3) {
            state    = HAL_CAP_TOUCH_KEY_PRESS;
            channel = event;
        }
        else {
            state = HAL_CAP_TOUCH_KEY_RELEASE;
            channel = event -4 ;
        }
        if(state >1  || (!( (1<<channel) &(captouch_context.used_channel_map))) ){
            HAL_CAPTOUCH_LOG_PRINT("captouch wrong state  ",0 );
            captouch_hif->TOUCH_HIF_CON1.CELLS.EVENT_POP = 1;
            captouch_hif->TOUCH_HIF_CON0.CELLS.HIF_SOFT_RST = 1;
            hal_gpt_delay_us(10);
            break;
        }

        if(captouch_get_control_manual_state(HAL_CAPTOUCH_CHANNEL_0) && state == HAL_CAP_TOUCH_KEY_RELEASE){
                 captouch_findbase_after_ctrl_man();
            }

        HAL_CAPTOUCH_LOG_PRINT("captouch channel:%d, state:%d, time_stamp:0x%x",3 , channel, state, time_stamp );
        /*pop event*/
        captouch_hif->TOUCH_HIF_CON1.CELLS.EVENT_POP = 1;
        //captouch_push_one_event_to_buffer(channel,state,time_stamp);
        if (captouch_context.has_initilized == true)
            captouch_channel_debounce_check(channel, state, time_stamp);
    }

    if(captouch->TOUCH_LPWUFLAG.LPWUINT) {
        captouch->TOUCH_LPWUCLR.LPWUIN_CLR=1;
        while(captouch->TOUCH_LPWUFLAG.LPWUINT);
        #if( PRODUCT_VERSION == 2822 && defined RTC_CAPTOUCH_SUPPORTED )
        captouch->TOUCH_LPWUCLR.LPWUIN_CLR=0;
        #elif (defined RTC_CAPTOUCH_SUPPORTED)
        captouch->TOUCH_LPWUCLR.LPWUIN_CLR=0;
        rtc_internal_clear_wakeup_status();
        #endif
    }

    //if (captouch_context.has_initilized == true) {
       // captouch_call_user_callback();
    //}
}

void captouch_call_user_callback(void)
{
    hal_captouch_callback_context_t *context;

    context = &captouch_context.captouch_callback;

    if (captouch_context.has_initilized != true) {
        return;
    }

    context->callback(context->user_data);
}

#if (PRODUCT_VERSION == 2822)
void captouch_channel_debounce_check(hal_captouch_channel_t channel, hal_captouch_key_state_t state, uint32_t time_stamp)
{
    hal_gpt_status_t gpt_status;

    HAL_CAPTOUCH_LOG_PRINT("captouch debounce check channel:%d, state:%d, gsend_flag:%d, gfirstTouch_flag:%d", 4, channel, state, gsend_flag[channel], gfirstTouch_flag[channel]);
    if(gfirstTouch_flag[channel])
    {
        gfirstTouch_flag[channel] = FALSE;
        if (state == HAL_CAP_TOUCH_KEY_PRESS)
            gsend_flag[channel] = SEND_RELEASSE;
    }
    if(state == gsend_flag[channel])
    {
        HAL_CAPTOUCH_LOG_PRINT("captouch captouch_debounce_timer is used", 0);
        hal_gpt_sw_stop_timer_ms(captouch_debounce_timer[channel]);
        gpt_status = hal_gpt_sw_free_timer(captouch_debounce_timer[channel]);
        if(captouch_context.debounce_state[channel] == 0) noise_cnt_race[channel]++;
        HAL_CAPTOUCH_LOG_PRINT("captouch free gpt timer handle:0x%x, status:0x%x", 2, captouch_debounce_timer[channel], gpt_status);
    }
    else // timer not remain, debounce time out
    {
        gpt_status = hal_gpt_sw_get_timer(&captouch_debounce_timer[channel]);
        if(gpt_status != HAL_GPT_STATUS_OK)
        {
            HAL_CAPTOUCH_LOG_PRINT("captouch hal_gpt_sw_get_timer fail:%d", 1, gpt_status);
            return;
        }
        captouch_event_temp.captouch_event[channel].channel = channel;
        captouch_event_temp.captouch_event[channel].state = state;
        captouch_event_temp.captouch_event[channel].time_stamp = time_stamp;
        //start debounce timer
        if(state) //press event
        {
            gpt_status = hal_gpt_sw_start_timer_ms(captouch_debounce_timer[channel],\
                                                                                  captouch_context.swDebounceTime,\
                                                                                  captouch_key_event_handler,\
                                                                                  (void*)&captouch_event_temp.captouch_event[channel].channel);
        }
        else//release event
        {
            gpt_status = hal_gpt_sw_start_timer_ms(captouch_debounce_timer[channel],\
                                                                                  captouch_context.swDebounceTime_Rls,\
                                                                                  captouch_key_event_handler,\
                                                                                  (void*)&captouch_event_temp.captouch_event[channel].channel);
        }
        if(gpt_status != HAL_GPT_STATUS_OK)
        {
            HAL_CAPTOUCH_LOG_PRINT("captouch hal_gpt_sw_start_timer_ms fail:%d", 1, gpt_status);
            return;
        }
        HAL_CAPTOUCH_LOG_PRINT("captouch debounce gpt timer start handle:0x%x, status:0x%x", 2, captouch_debounce_timer[channel], gpt_status);
    }
}

void captouch_key_event_handler( void  *user_data)
{
    uint8_t channel;
    uint32_t isr_mask;
    hal_gpt_status_t gpt_status;
    channel =  *(uint8_t*)user_data;
    //captouch_event_temp_t *captouch_event_temp  = (captouch_event_temp_t *)user_data;
    hal_nvic_save_and_set_interrupt_mask(&isr_mask);

    captouch_push_one_event_to_buffer(captouch_event_temp.captouch_event[channel].channel,\
                                                                        captouch_event_temp.captouch_event[channel].state,\
                                                                        captouch_event_temp.captouch_event[channel].time_stamp);

    captouch_context.debounce_state[captouch_event_temp.captouch_event[channel].channel] =\
                                                                captouch_event_temp.captouch_event[channel].state*100;

    hal_nvic_restore_interrupt_mask(isr_mask);

    HAL_CAPTOUCH_LOG_PRINT("captouch send event to upper layer channel:%d  state:%d time_stamp:0x%x debounce_state:%d",\
                                                        4,\
                                                        captouch_event_temp.captouch_event[channel].channel,\
                                                        captouch_event_temp.captouch_event[channel].state,\
                                                        captouch_event_temp.captouch_event[channel].time_stamp,\
                                                        captouch_context.debounce_state[captouch_event_temp.captouch_event[channel].channel]);
    captouch_call_user_callback();
    gsend_flag[channel] = captouch_event_temp.captouch_event[channel].state;

    if(( autosuspend_timeout_context.ch_bitmap & (1<< channel)) && (autosuspend_timeout_context.time[channel]))
    {
        captouch_close_autosuspend_timeout(channel);
    }
    //hal_gpt_sw_stop_timer_ms(captouch_debounce_timer[channel]);
    gpt_status = hal_gpt_sw_free_timer(captouch_debounce_timer[channel]);
    HAL_CAPTOUCH_LOG_PRINT("captouch free gpt timer handle:0x%x, status:%d", 2, captouch_debounce_timer[channel], gpt_status);
}
void captouch_set_debounce_timer(uint8_t time_ms_p, uint8_t time_ms_r)
{
    captouch_context.swDebounceTime = time_ms_p;
    captouch_context.swDebounceTime_Rls = time_ms_r;
}
#else
void captouch_channel_debounce_check(hal_captouch_channel_t channel, hal_captouch_key_state_t state, uint32_t time_stamp)
{

    bool is_noise=false;
    hal_gpt_status_t gpt_status;
    uint32_t isr_mask;

    if(gfirstTouch_flag[channel] ){
        if( state == HAL_CAP_TOUCH_KEY_PRESS)
            gsend_flag[channel] = SEND_PRESS;
        else
            gsend_flag[channel] = SEND_RELEASSE;
        gfirstTouch_flag[channel] = false;
        HAL_CAPTOUCH_LOG_PRINT("captouch ch%d first state =%d ",2, channel,state);
	}

	HAL_CAPTOUCH_LOG_PRINT("captouch channel debounce check channel:%d, state:%d, time_stamp:0x%x gsend_flag=%d",4 , channel, state, time_stamp ,gsend_flag[channel]);
    if(state){
        event_time_stamp[channel] = time_stamp;
    }else{
        //release_time_stamp[channel] = time_stamp;
        is_noise = captouch_intr_bounce_check(channel,event_time_stamp, time_stamp);
    }

    if(!is_noise) //sw debounce 40ms
    {
        if(state) //press
        {
            HAL_CAPTOUCH_LOG_PRINT("captouch debounce - press intr - start debounce timer channel =%d", 1,channel);

            gpt_status = hal_gpt_sw_get_timer(&captouch_debounce_timer[channel]);

            if (gpt_status != HAL_GPT_STATUS_OK) {
                HAL_CAPTOUCH_LOG_PRINT("captouch hal_gpt_sw_get_timer fail:%d ",1 ,gpt_status );
            }

            captouch_event_temp.captouch_event[channel].channel    = channel;
            captouch_event_temp.captouch_event[channel].state      = state;
            captouch_event_temp.captouch_event[channel].time_stamp = time_stamp;
            //hal_gpt_sw_free_timer(captouch_debounce_timer[press_channel]);
            gpt_status =hal_gpt_sw_start_timer_ms(captouch_debounce_timer[channel],\
                                                  captouch_context.swDebounceTime,\
                                                  captouch_key_press_event_handler,\
                                                  (void*)&captouch_event_temp.captouch_event[channel].channel);

            if (gpt_status != HAL_GPT_STATUS_OK) {
                HAL_CAPTOUCH_LOG_PRINT("captouch hal_gpt_sw_start_timer_ms fail:%d ",1 ,gpt_status );
            }
        }
        else//release
        {
            if( !lpwu_press && gsend_flag[channel] == SEND_PRESS ){
               //gsend_flag[channel] =  SEND_RELEASSE;
                hal_gpt_sw_stop_timer_ms(captouch_debounce_timer[channel]);
                gpt_status = hal_gpt_sw_free_timer(captouch_debounce_timer[channel]);
                if(captouch_context.debounce_state[channel] == 0)noise_cnt_race[channel]++;
                HAL_CAPTOUCH_LOG_PRINT("captouch cant send release event and free gpt_status =%d", 1,gpt_status );
            }else{
                hal_nvic_save_and_set_interrupt_mask(&isr_mask);
                gsend_flag[channel] =  SEND_PRESS;
                captouch_push_one_event_to_buffer(channel,state,time_stamp);
                captouch_context.debounce_state[channel]= 0;
                hal_nvic_restore_interrupt_mask(isr_mask);
                if(lpwu_press) lpwu_press = false;
                HAL_CAPTOUCH_LOG_PRINT("captouch send release event to top channel =%d state= %d time_stamp =0x%x debounce =%d", 4 ,channel,state,time_stamp,captouch_context.debounce_state[channel]);
                captouch_call_user_callback();
                if( ( autosuspend_timeout_context.ch_bitmap & (1<<channel) ) && \
                    ( autosuspend_timeout_context.time[channel])           ){

                    gpt_status = hal_gpt_sw_stop_timer_ms(captouch_delay_timer[channel]);
                    gpt_status = hal_gpt_sw_free_timer(captouch_delay_timer[channel]);
                    //HAL_CAPTOUCH_LOG_PRINT("====== captouch release captouch_delay_timer delay =%d",1,gpt_status);

                    gpt_status = hal_gpt_sw_stop_timer_ms(captouch_close_autosuspend_timer[channel]);
                    gpt_status = hal_gpt_sw_free_timer(captouch_close_autosuspend_timer[channel]);
                    //HAL_CAPTOUCH_LOG_PRINT("====== captouch reset autosuspend handler delay",0);

                    hal_captouch_set_avg(channel,captouch_context.mavg_r[channel],captouch_context.avg_s[channel]);
                    captouch_set_autok_suspend(1<<channel,true);
                }
            }
        }
    }
    else
    {
        if ( gsend_flag[channel] == SEND_RELEASSE ){    //press has been sent so release must be send
            gsend_flag[channel] =  SEND_PRESS;
            hal_nvic_save_and_set_interrupt_mask(&isr_mask);
            captouch_push_one_event_to_buffer(channel,state,time_stamp);
            captouch_context.debounce_state[channel]= 0;
            hal_nvic_restore_interrupt_mask(isr_mask);
            HAL_CAPTOUCH_LOG_PRINT("captouch release intr noise  but press event has send ", 0);
            captouch_call_user_callback();
            noise_cnt_race[channel] = 0;
            if( ( autosuspend_timeout_context.ch_bitmap & (1<<channel) ) && \
                ( autosuspend_timeout_context.time[channel])           ){

                gpt_status = hal_gpt_sw_stop_timer_ms(captouch_delay_timer[channel]);
                gpt_status = hal_gpt_sw_free_timer(captouch_delay_timer[channel]);
                //HAL_CAPTOUCH_LOG_PRINT("====== captouch release captouch_delay_timer delay =%d",1,gpt_status);

                gpt_status = hal_gpt_sw_stop_timer_ms(captouch_close_autosuspend_timer[channel]);
                gpt_status = hal_gpt_sw_free_timer(captouch_close_autosuspend_timer[channel]);
                //HAL_CAPTOUCH_LOG_PRINT("====== captouch reset autosuspend handler delay",0);

                hal_captouch_set_avg(channel,captouch_context.mavg_r[channel],captouch_context.avg_s[channel]);
                captouch_set_autok_suspend(1<<channel,true);
            }
        }else{
            hal_gpt_sw_stop_timer_ms(captouch_debounce_timer[channel]);
            gpt_status = hal_gpt_sw_free_timer(captouch_debounce_timer[channel]);
            HAL_CAPTOUCH_LOG_PRINT("captouch debounce timer is active - release intr noise and free gpt_status =%d", 1,gpt_status);
            if(captouch_context.debounce_state[channel] == 0) noise_cnt_race[channel]++;
        }
    }

}
#endif

bool captouch_intr_bounce_check(hal_captouch_channel_t channel, uint32_t *pre_time_stamp,  uint32_t time_stamp )
{
    bool is_noise = true;
    int32_t temp;

    //temp = time_stamp - event_time_stamp[channel];

    temp = time_stamp - pre_time_stamp[channel];
    if(temp < 0)
        temp = (time_stamp + 0x3FFFFFF) - event_time_stamp[channel];
    if(temp*30/1000 > captouch_context.swDebounceTime) //temp*30/1000 > normal_setting->swDebounceTime
        is_noise = false;

    return is_noise;
}

void captouch_key_press_event_handler( void  *user_data)
{
    uint8_t press_channel;
    uint32_t isr_mask;
    press_channel =  *(uint8_t*)user_data;
    //captouch_event_temp_t *captouch_event_temp  = (captouch_event_temp_t *)user_data;
    hal_nvic_save_and_set_interrupt_mask(&isr_mask);

    if( gsend_flag[press_channel] == SEND_PRESS){

        gsend_flag[press_channel] = SEND_RELEASSE;

        captouch_push_one_event_to_buffer(captouch_event_temp.captouch_event[press_channel].channel,\
                                          captouch_event_temp.captouch_event[press_channel].state,\
                                          captouch_event_temp.captouch_event[press_channel].time_stamp);

        captouch_context.debounce_state[captouch_event_temp.captouch_event[press_channel].channel] =\
                                        captouch_event_temp.captouch_event[press_channel].state*100;

        hal_nvic_restore_interrupt_mask(isr_mask);

        HAL_CAPTOUCH_LOG_PRINT("captouch send press event to top channel = %d  state = %d time_stamp=%x debounce_state",\
                                  4,\
                                  captouch_event_temp.captouch_event[press_channel].channel,\
                                  captouch_event_temp.captouch_event[press_channel].state,\
                                  captouch_event_temp.captouch_event[press_channel].time_stamp,\
                                  captouch_context.debounce_state[captouch_event_temp.captouch_event[press_channel].channel] );

        captouch_call_user_callback();

        if( ( autosuspend_timeout_context.ch_bitmap & (1<< press_channel )) && \
            ( autosuspend_timeout_context.time[press_channel] )           ){
            captouch_close_autosuspend_timeout(press_channel);
        }
    }else{
        gsend_flag[press_channel] = SEND_PRESS;
        hal_nvic_restore_interrupt_mask(isr_mask);
    }
    hal_gpt_sw_free_timer(captouch_debounce_timer[press_channel]);
}

/*
void captouch_key_press_event_handler(TimerHandle_t xTimer)
{
    UNUSED(xTimer);
    hal_captouch_event_t	hal_captouch_event;
    if(noise_cnt != 0)
    {
        HAL_CAPTOUCH_LOG_PRINT("captouch noise_cnt =%d",1,noise_cnt);
        //noise_cnt--;
        noise_cnt --;
        return;
    }


    //hal_captouch_get_event(&hal_captouch_event);
    //HAL_CAPTOUCH_LOG_PRINT("captouch key press event handler channel:%d, state:%d",2, hal_captouch_event.channel, hal_captouch_event.state);
    //HAL_CAPTOUCH_LOG_PRINT("==== captouch send  press msg to app channel:%d, state:%d,====",2,hal_captouch_event.key_data,hal_captouch_event.state );
    //HAL_CAPTOUCH_LOG_PRINT("==== captouch send  press msg to app  ",0);
    //captouch_push_one_event_to_buffer(hal_captouch_event.key_data,hal_captouch_event.state,hal_captouch_event.time_stamp);
    captouch_protect_buffer();
    captouch_call_user_callback();
    captouch_unprotect_buffer();
}*/

void captouch_key_release_event_handler(TimerHandle_t channel_timer)
{
    hal_captouch_event_t	hal_captouch_event;


    hal_captouch_get_event(&hal_captouch_event);
    //HAL_CAPTOUCH_LOG_PRINT("captouch key release event handler channel:%d, state:%d",2, hal_captouch_event.channel, hal_captouch_event.state);

    if(hal_captouch_event.state== 1)
    {
        HAL_CAPTOUCH_LOG_PRINT("captouch send release msg fail channel:%d, state:%d",2, hal_captouch_event.key_data, hal_captouch_event.state);
        xTimerStopFromISR(channel_timer, 0);
        hal_captouch_get_event(&hal_captouch_event);
        if(captouch_context.debounce_state[hal_captouch_event.key_data] == 0) noise_cnt_race[hal_captouch_event.key_data]++;
    }
    else
    {
        //HAL_CAPTOUCH_LOG_PRINT("==== captouch send msg to app channel:%d, state:%d ",2,hal_captouch_event.key_data,hal_captouch_event.state );
        HAL_CAPTOUCH_LOG_PRINT("==== captouch send release msg to app channel:%d, state:%d ",2,hal_captouch_event.key_data,hal_captouch_event.state );
        captouch_push_one_event_to_buffer(hal_captouch_event.key_data,hal_captouch_event.state,hal_captouch_event.time_stamp);
        captouch_call_user_callback();
        captouch_context.debounce_state[hal_captouch_event.key_data] = hal_captouch_event.state*100;
    }

}
bool captouch_lpsd_handler(void)
{
    bool shutd_flag;

    shutd_flag = false;

    if(captouch->TOUCH_LPSDFLAG.LPSDINT) {
        shutd_flag = true;
        captouch->TOUCH_LPSDCLR.LPSDIN_CLR=1;
        while(captouch->TOUCH_LPSDFLAG.LPSDINT);
        captouch->TOUCH_LPSDCLR.LPSDIN_CLR =0;
    }
    return shutd_flag;
}


void captouch_register_nvic_callback(void)
{
    uint32_t mask;
    static bool register_flag;

    if (register_flag == false) {
        while (captouch_hif->TOUCH_HIF_CON1.CELLS.EVENT_PENDING){
            captouch_hif->TOUCH_HIF_CON1.CELLS.EVENT_POP = 1;
            hal_gpt_delay_us(1);
        }
        hal_nvic_save_and_set_interrupt_mask(&mask);
        hal_nvic_register_isr_handler(CAP_TOUCH_IRQn, captouch_interrupt_handler);
        hal_nvic_enable_irq(CAP_TOUCH_IRQn);
        hal_nvic_restore_interrupt_mask(mask);
        register_flag = true;
    }
}
void captouch_clk_control(bool is_clock_on)
{
    if (is_clock_on == true) {
         captouch->TOUCH_CON0.CON0 |= (1<<8);
    }
    else {
         captouch->TOUCH_CON0.CON0 &= ~(1<<8);
    }
}

void captouch_set_clk(hal_captouch_lowpower_type_t power_type,hal_captouch_clock_t clock)
{
    uint32_t tmp;
    tmp = captouch->TOUCH_CON1.CON1;
    if (power_type == HAL_CAPTOUCH_MODE_NORMAL) {
        tmp &= ~(0x7);
        tmp |= clock;
        captouch->TOUCH_CON1.CON1 = tmp;
    }
    else{
        tmp &= ~(0x70);
        tmp |= clock;
        captouch->TOUCH_CON1.CON1 = tmp;
    }
}

void captouch_set_mavg(hal_captouch_channel_t channel,uint8_t mavg)
{
    uint32_t tmp;
    tmp = captouch->TOUCH_CON3.CON3;
    tmp &= ~(0xf << (channel*4));
    tmp |= (mavg << (channel*4));
    captouch->TOUCH_CON3.CON3 = tmp;
}

void captouch_set_avg(hal_captouch_channel_t channel,uint8_t avg)
{
    uint32_t tmp;
    tmp = captouch->TOUCH_CON2.CON2;
    tmp &= ~(0x7 << (channel*3));
    tmp |= (avg << (channel*3));
    captouch->TOUCH_CON2.CON2 = tmp;
}


void captouch_set_fine_cap(hal_captouch_channel_t channel,int8_t fine_tune)
{
    uint32_t tmp;
    tmp = captouch->TOUCH_FINECAP.FINECAP;
    //tmp &= ~(0x7f << (channel*8));
    //tmp |= (fine_tune << (channel*8));
    //printf("captouch set before TOUCH_FINECAP.FINECAP =%x fine_tune =%d",tmp,fine_tune);
    captouch->TOUCH_FINECAP.FINECAP = BIT_FIELD_INSERT32(tmp,channel*8,7,fine_tune);
    //printf("captouch set TOUCH_FINECAP.FINECAP =%x",captouch->TOUCH_FINECAP.FINECAP);
    //captouch->TOUCH_FINECAP.FINECAP = tmp;
}

int8_t captouch_get_fine_cap(hal_captouch_channel_t channel)
{
    uint32_t tmp;
    uint16_t data=0;
    captouch_switch_debug_sel(channel);
    tmp = captouch->TOUCH_DBG1.DBG1;
    data = (tmp>>16) & 0x7f;
    return captouch_to16signed(7,data);


}

void captouch_set_coarse_cap(hal_captouch_channel_t channel, uint8_t coarse_tune)
{
    uint32_t tmp;
    tmp = captouch->TOUCH_CRSCFG.CRSCFG;
    tmp &= ~(0x7 << (channel*3));
    tmp |= (coarse_tune << (channel*3));
    captouch->TOUCH_CRSCFG.CRSCFG = tmp;
}

uint8_t captouch_get_coarse_cap(hal_captouch_channel_t channel)
{
    uint32_t tmp;
    uint8_t ret;
    tmp = captouch->TOUCH_CRSCFG.CRSCFG;
    ret  =(uint8_t)((tmp >> (channel*3)) & 0x7);

    return ret;
}

void captouch_set_threshold(hal_captouch_channel_t channel,int32_t high_thr, int32_t low_thr)
{
    switch (channel) {
        case HAL_CAPTOUCH_CHANNEL_0:
            captouch->TOUCH_THR0.THR = low_thr | (high_thr<<16);
        break;
        case HAL_CAPTOUCH_CHANNEL_1:
            captouch->TOUCH_THR1.THR = low_thr | (high_thr<<16);
        break;
        case HAL_CAPTOUCH_CHANNEL_2:
            captouch->TOUCH_THR2.THR = low_thr | (high_thr<<16);
        break;
        case HAL_CAPTOUCH_CHANNEL_3:
            captouch->TOUCH_THR3.THR = low_thr | (high_thr<<16);
        break;
        default:
        break;
    }

}

void captouch_set_nthreshold(hal_captouch_channel_t channel,int32_t high_thr, int32_t low_thr)
{
    switch (channel) {
        case HAL_CAPTOUCH_CHANNEL_0:
            captouch->TOUCH_NTHR0.THR = low_thr | (high_thr<<16);
        break;
        case HAL_CAPTOUCH_CHANNEL_1:
            captouch->TOUCH_NTHR1.THR = low_thr | (high_thr<<16);
        break;
        case HAL_CAPTOUCH_CHANNEL_2:
            captouch->TOUCH_NTHR2.THR = low_thr | (high_thr<<16);
        break;
         case HAL_CAPTOUCH_CHANNEL_3:
            captouch->TOUCH_NTHR3.THR = low_thr | (high_thr<<16);
        break;
        default:
        break;
    }

}

void captouch_set_dynamic_threshold(bool nthr_en,bool thr_en,int16_t rangeH, int16_t rangeL)
{
    uint32_t tmp;
    tmp = captouch->TOUCH_BASELINE.BASERANGE;
    tmp &= ~0xfffffff;

    if (nthr_en == true) {
        tmp |= (TOUCH_NTHRH_EN | TOUCH_NTHRL_EN);
    }
    else {
        tmp &= ~(TOUCH_NTHRH_EN | TOUCH_NTHRL_EN);
    }
    if (thr_en == true) {
        tmp |= (TOUCH_THRH_EN | TOUCH_THRL_EN);
    }
    else {
        tmp &= ~(TOUCH_THRH_EN | TOUCH_THRL_EN);
    }

    rangeH &= 0x1ff;
    rangeL &= 0x1ff;
    tmp |= (rangeL | (rangeH<<16));
    captouch->TOUCH_BASELINE.BASERANGE = tmp;
}

void captouch_set_control_manual(hal_captouch_channel_t channel,bool is_auto)
{
    uint32_t rdata;

    rdata = captouch->TOUCH_CON1.CON1;
    if (is_auto == true) {
        captouch->TOUCH_CON1.CON1 = BIT_FIELD_INSERT32(rdata,(channel+8),1,1);
    }
    else {
        captouch->TOUCH_CON1.CON1 = BIT_FIELD_INSERT32(rdata,(channel+8),1,0);
    }

}
bool captouch_get_control_manual_state(hal_captouch_channel_t channel)
{
    uint32_t rdata;
    rdata = captouch->TOUCH_CON1.CON1;
    return  BIT_FIELD_EXTRACT32(rdata,(channel+8),1);
}

void captouch_set_autok_suspend(uint8_t channel_bit_map, bool en)
{
    uint32_t rdata;

    rdata = captouch->TOUCH_CON0.CON0;
    if (en == true) {
        rdata |= (channel_bit_map<<24);
    }
    else {
        rdata &= ~(channel_bit_map<<24);
    }
    captouch->TOUCH_CON0.CON0 = rdata;
}

void captouch_set_autok_Nsuspend(uint8_t channel_bit_map, bool en)
{
    uint32_t rdata;

    rdata = captouch->TOUCH_CON0.CON0;
    if (en == true) {
        rdata |= (channel_bit_map<<28);
    }
    else {
        rdata &= ~(channel_bit_map<<28);
    }
    captouch->TOUCH_CON0.CON0 = rdata;
}

void captouch_channel_sense_control(uint8_t channel_bit_map, bool en)
{
    uint32_t temp;

    temp = captouch->TOUCH_CON0.CON0;
    if (en == true) {
        temp |= (channel_bit_map | (channel_bit_map<<16));
    }
    else {
        temp &= ~(channel_bit_map | (channel_bit_map<<16));
    }
    captouch->TOUCH_CON0.CON0 = temp;
    hal_gpt_delay_ms(1);
}

void captouch_channel_sensing_control(uint8_t channel_bit_map, bool en)
{
    uint32_t temp;
    temp = captouch->TOUCH_CON0.CON0;
    if (en == true) {
        temp |= (channel_bit_map);
    }
    else {
        temp &= ~(channel_bit_map);
    }
    captouch->TOUCH_CON0.CON0 = temp;
    hal_gpt_delay_ms(1);
}

void captouch_int_control(bool en)
{
    uint32_t temp;
    temp = captouch->TOUCH_CON0.CON0;

    if (en==true) {
        temp |= (TOUCH_INT_EN | TOUCH_WAKE_EN);
    }
    else {
        temp &= ~(TOUCH_INT_EN | TOUCH_WAKE_EN);
    }
    captouch->TOUCH_CON0.CON0 = temp;
}

void captouch_longpress_int_control(bool en)
{
    uint32_t temp;
    temp = captouch->TOUCH_LPWUCON.LPWUCON;

    if (en==true) {
        temp |= (TOUCH_LPWU_INT_EN | TOUCH_LPWU_WAKE_EN);
    }
    else {
        temp &= ~(TOUCH_LPWU_INT_EN | TOUCH_LPWU_WAKE_EN);
    }
    captouch->TOUCH_LPWUCON.LPWUCON = temp;

}

void captouch_channel_int_control(uint8_t channel_bit_map, bool en)
{
    uint32_t temp;
    temp = captouch->TOUCH_CHMASK.CHMASK;
    if (en == true) {
        temp |= ((channel_bit_map<<8) | (channel_bit_map<<12));
    }
    else {
        temp &= ~((channel_bit_map<<8) | (channel_bit_map<<12));
    }
    captouch->TOUCH_CHMASK.CHMASK = temp;
}

void captouch_wakeup_setting(uint8_t channel_bit_map, bool en)
{
    uint32_t temp;
    temp = captouch->TOUCH_CHMASK.CHMASK;
    if (en == true) {
        temp |= (channel_bit_map | (channel_bit_map<<4));
    }
    else {
        temp &= ~(channel_bit_map | (channel_bit_map<<4));
    }
    captouch->TOUCH_CHMASK.CHMASK = temp;
}

void captouch_longpress_channel_control(hal_captouch_longpress_type_t type, uint32_t count)
{
    uint32_t temp;

    if(type == HAL_CAPTOUCH_LONGPRESS_SHUTDOWN) {
        temp = captouch->TOUCH_LPSDCON.LPSDCON;
        temp |= TOUCH_LPSD_MASK;
        captouch->TOUCH_LPSDCON.LPSDCON = temp;
        captouch->TOUCH_LPSDTAR.LPSDTAR = count;
    }
    else {
        temp = captouch->TOUCH_LPWUCON.LPWUCON;
        temp |= TOUCH_LPWU_MASK;
        captouch->TOUCH_LPWUCON.LPWUCON = temp;
        captouch->TOUCH_LPWUTAR.LPWUTAR = count;
    }

}

void captouch_longpress_channel_select_control(hal_captouch_longpress_type_t type,uint8_t channel_bit_map,uint32_t count)
{
    uint32_t temp;

    if(type == HAL_CAPTOUCH_LONGPRESS_SHUTDOWN) {
        temp = captouch->TOUCH_LPSDCON.LPSDCON;
        temp |= channel_bit_map<<16;

        captouch->TOUCH_LPSDCON.LPSDCON = temp;
        captouch->TOUCH_LPSDTAR.LPSDTAR = count;

    }
    else {
        temp = captouch->TOUCH_LPWUCON.LPWUCON;
        temp |= TOUCH_LPWU_MASK;
        captouch->TOUCH_LPWUCON.LPWUCON = temp;
        captouch->TOUCH_LPWUTAR.LPWUTAR = count;
    }
    //HAL_CAPTOUCH_LOG_PRINT("captouch_longpress_channel_select_control = 0x%x",1,captouch->TOUCH_LPSDCON.LPSDCON);
}

void captouch_switch_debug_sel(hal_captouch_channel_t channel)
{
    uint32_t temp;
    temp = captouch->TOUCH_CON1.CON1;
    temp &= ~(0x3<<24);
    temp |= (channel<<24);
    captouch->TOUCH_CON1.CON1 = temp;
}

int16_t captouch_to16signed(uint16_t bit, int16_t data)
{
    data <<= (16-bit);
    data >>= (16-bit);

    return data;
}
void captouch_get_tune_state(hal_captouch_channel_t channel,hal_captouch_tune_data_t *tune_data)
{
    uint32_t temp;
    uint16_t data=0;

    captouch_switch_debug_sel(channel);
    tune_data->coarse_cap = captouch_get_coarse_cap(channel);//coares cap value

    temp = captouch->TOUCH_DBG1.DBG1;
    data = (temp>>16) & 0x7f;
    tune_data->fine_cap = captouch_to16signed(7,data);

    temp = captouch->TOUCH_DBG0.DBG0;
    data = (temp & 0xffff);
    tune_data->avg_adc = captouch_to16signed(9,data);

    temp = captouch->TOUCH_DBG2.DBG2;
    data = (temp & 0xffff);
    tune_data->vadc = captouch_to16signed(9,data);

    temp = captouch->TOUCH_FINECAP.FINECAP;
    tune_data->man  = temp & (0xff<<channel);

    temp = captouch->TOUCH_DBG1.DBG1;
    data =(temp & 0xffff);
    tune_data->mavg_dbg = captouch_to16signed(9,data);

    ///////

    if( tune_data->mavg_dbg >=32 || tune_data->mavg_dbg <=0 )
        tune_data->mavg_dbg = 0;

    tune_data->ear_detect_base = captouch_ear_detect.ear_detect_base[channel];

    tune_data->adc_debounce = captouch_context.debounce_state[channel];

    tune_data->ear_detect_data = tune_data->mavg_dbg + tune_data->fine_cap*114/4;

    tune_data->adc_debounce = captouch_context.debounce_state[channel];

    tune_data->noise_cnt_race = 0;

    if(captouch_context.debounce_state[channel] != 0)
    {
        tune_data->noise_cnt_race = noise_cnt_race[channel];
        HAL_CAPTOUCH_LOG_PRINT("captouch key_press_event noise count to race cnt:%d",1, noise_cnt_race[channel]);
        noise_cnt_race[channel] = 0;
    }
}
#if 0
bool captouch_sw_auto_tune(hal_captouch_channel_t channel, hal_captouch_tune_data_t *tune_data)
{
    uint16_t data;
    uint16_t temp;
    int16_t vadc_dbg;
    //int8_t cal_out;
    uint8_t coare_index;
    int16_t  find_cap[3];
    bool is_tune_ok;

    //captouch_tune_auto_control(channel,true);

    coare_index = 0;
    find_cap[0] = -64;
    find_cap[1] = 0;
    find_cap[2] = 63;

    is_tune_ok = false;
    while(1) {

        captouch_set_fine_cap(channel,find_cap[1]);
        captouch_set_coarse_cap(channel,coare_index);

        hal_gpt_delay_ms(2);

        temp = captouch->TOUCH_DBG2.DBG2;
        data =(temp & 0xffff);
        vadc_dbg = captouch_to16signed(9,data);

        // the adc value should be +-32
        if (vadc_dbg>=10) {             //if adc is bigger, the fine_cap should be bigger to decrease the adc value
            find_cap[0] =  find_cap[1]; //the lower limit should move to the current value
            find_cap[1] = (find_cap[2] + find_cap[1])/2;
        }
        else if(vadc_dbg<=(-20)) {      //if adc is smller, the fine_cap should be smller to increase the adc value
            find_cap[2] = find_cap[1];  //the upper limit should move to the current value
            find_cap[1] = (find_cap[1] + find_cap[0])/2;
        }
        else {
            is_tune_ok = true;
            break;
        }

        if ((find_cap[1]>35) || (find_cap[1]<=(-35))){
            ++coare_index;
            if (coare_index>=8) {
                is_tune_ok = false;
                break;
            }
            find_cap[0] = -64;
            find_cap[1] = 0;
            find_cap[2] = 63;
        }

    }

    captouch_get_tune_state(channel,tune_data);
    log_hal_info("captouch_sw_auto_tune ch=%d,fine_cap=%d, crs=%d, vadc_dbg=%d\r\n",channel,find_cap[1],coare_index,vadc_dbg);
    //captouch_tune_auto_control(channel,false);

    return is_tune_ok;

}
#endif
bool captouch_hw_auto_tune(hal_captouch_channel_t channel, hal_captouch_tune_data_t *tune_data )
{
    uint32_t time1,time2,duration,tune_delay;
    uint32_t temp;
    uint16_t data=0;
    int16_t cal_out, vadc_dbg,mavg_adc_dbg;
    bool is_tune_ok =true;

    captouch_switch_debug_sel(channel);

	temp = captouch->TOUCH_DBG1.DBG1;
    data = (temp>>16) & 0x7f;
    cal_out = captouch_to16signed(7,data);

    temp = captouch->TOUCH_DBG0.DBG0;
    data = (temp & 0xffff);
    vadc_dbg = captouch_to16signed(9,data);
    tune_delay = tune_delay_us;

    HAL_CAPTOUCH_LOG_PRINT("captouch auto coarse tune start ch  %d, coarse_cap %d, fine cap %d, adc %d ", 4, channel, tune_data->coarse_cap, cal_out, vadc_dbg);

    if( (cal_out >50 || vadc_dbg >= 40) && tune_data->coarse_cap<7) {
        //LOG_I(LOG_OS, "captouch_sw_auto_tune 0xdb >44[%d]  ", 1,cal_out);
        captouch_channel_sensing_control( 1<<channel , captouch_disable);
        captouch_set_coarse_cap(channel,++tune_data->coarse_cap);
        captouch_channel_sensing_control(1<<channel, captouch_enable);
        HAL_CAPTOUCH_LOG_PRINT("captouch auto coarse tune try set_coarse_cap %d", 1, tune_data->coarse_cap);
    }
    else if( (cal_out<=(-50)|| vadc_dbg <= (-40))&& tune_data->coarse_cap>0) {
        //LOG_I(LOG_OS, "captouch_sw_auto_tune 0xdb <44[%d]  ", 1,cal_out);
        captouch_channel_sensing_control(1<<channel, captouch_disable);
        captouch_set_coarse_cap(channel,--tune_data->coarse_cap);
        captouch_channel_sensing_control(1<<channel, captouch_enable);
        HAL_CAPTOUCH_LOG_PRINT("captouch auto coarse tune try set_coarse_cap %d", 1, tune_data->coarse_cap);
    }
    else {
        HAL_CAPTOUCH_LOG_PRINT("captouch auto coarse tune  first choose finecap base %d ", 1, cal_out);
        tune_delay = 0;
    }

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time1);

    while(1){

        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time2);
        hal_gpt_get_duration_count(time1, time2, &duration);

        if((duration>tune_delay)) {
            temp = captouch->TOUCH_DBG1.DBG1;
            data = (temp>>16) & 0x7f;
            cal_out = captouch_to16signed(7,data);

            temp = captouch->TOUCH_DBG0.DBG0;
            data = (temp & 0xffff);
            vadc_dbg = captouch_to16signed(9,data);
            mavg_adc_dbg = captouch_get_mavg_adc(channel);
            if(cal_out <50 && (cal_out>=(-50)) &&  vadc_dbg <=40 && vadc_dbg >=(-40)) {
                is_tune_ok = true;
                captouch_fine_cap_to_nvkey(channel,cal_out);
                HAL_CAPTOUCH_LOG_PRINT("captouch auto coarse tune pass coarse cap %d, fine_cap %d, adc %d", 3, tune_data->coarse_cap, cal_out, vadc_dbg);

                if( mavg_adc_dbg>0 && mavg_adc_dbg<32 ){
                    captouch_ear_detect.ear_detect_base[channel] = cal_out*114/4+mavg_adc_dbg;
                    HAL_CAPTOUCH_LOG_PRINT("captouch auto coarse tune ear detect base  =%d mavg_adc = %d ",2,\
                                                            captouch_ear_detect.ear_detect_base[channel],\
                                                            mavg_adc_dbg);
                }
                else{
                    captouch_ear_detect.ear_detect_base[channel] = cal_out*114/4;
                    HAL_CAPTOUCH_LOG_PRINT("captouch auto coarse tune ear detect base=%d mavg_adc = %d ",2,\
                                                                captouch_ear_detect.ear_detect_base[channel],\
                                                                mavg_adc_dbg);
                }

                captouch_eardetect_base_to_nvkey(channel,captouch_ear_detect.ear_detect_base[channel]);
            }
            else{
                is_tune_ok = false;
                HAL_CAPTOUCH_LOG_PRINT("captouch auto coarse tune fail coarse cap %d, fine_cap %d, adc %d", 3, tune_data->coarse_cap, cal_out, vadc_dbg);
            }

            break;
         }
    }
    captouch_coarsecap_to_nvkey(channel,tune_data->coarse_cap);
    HAL_CAPTOUCH_LOG_PRINT("captouch auto coarse tune end", 0);

    return is_tune_ok;
}

void captouch_get_tune_delay_time(uint8_t clk_k, uint8_t mavg_r)
{
    uint32_t i;

    tune_delay_us = 0;
    for(i=HAL_CAPTOUCH_CHANNEL_0; i<HAL_CAPTOUCH_CHANNEL_MAX; i++)
    {
        if(captouch_context.used_channel_map & (1<<i))
            tune_delay_us += (1000/clk_k*(1<<mavg_r))*20; //1/clk_k*1000*2^mavg_r*20
    }
    HAL_CAPTOUCH_LOG_PRINT("captouch_get_tune_delay_time delay %dus", 1, tune_delay_us);
}

void captouch_get_det_delay_time(hal_captouch_config_t *config, uint8_t clk_k)
{
    uint32_t i;

    det_delay_us = 0;
    for(i=HAL_CAPTOUCH_CHANNEL_0; i<HAL_CAPTOUCH_CHANNEL_MAX; i++)
    {
        if(captouch_context.used_channel_map & (1<<i))
            det_delay_us += (1000/clk_k*(1<<config->avg_s[i])); //1/clk_k*1000*2^avg_s
    }
    HAL_CAPTOUCH_LOG_PRINT("captouch_get_det_delay_time delay %dus", 1, det_delay_us);
}

void captouch_find_baseline(hal_captouch_channel_t channel)
{
    uint32_t temp;
    uint16_t data=0;
    int16_t vadc_dbg, cal_out;

    HAL_CAPTOUCH_LOG_PRINT("captouch fine tune start ch %d", 1, channel);
    captouch_switch_debug_sel(channel);

        temp = captouch->TOUCH_DBG0.DBG0;
        data = (temp & 0xffff);
        vadc_dbg = captouch_to16signed(9,data);
    cal_out = captouch_get_fine_cap(channel);

         if(vadc_dbg <=40 && vadc_dbg >=(-40)) {
        HAL_CAPTOUCH_LOG_PRINT("captouch fine tune pass vadc_dbg %d fine cap %d", 2, vadc_dbg, cal_out);
        captouch_fine_cap_to_nvkey(channel,(int8_t)cal_out);
         }
    else{
        HAL_CAPTOUCH_LOG_PRINT("captouch fine tune fail vadc_dbg %d fine cap %d", 2, vadc_dbg, cal_out);
    }
    HAL_CAPTOUCH_LOG_PRINT("captouch fine tune end", 0);
}

bool captouch_rtc_clk_control(bool is_en)
{
    #ifdef RTC_CAPTOUCH_SUPPORTED
    hal_rtc_status_t rtc_ret;

    if (is_en == true) {
        rtc_ret = hal_rtc_captouch_init();
    }
    else {
        rtc_ret = hal_rtc_captouch_deinit();
    }

    if (rtc_ret != HAL_RTC_STATUS_OK) {
        //log_hal_info("[Captouch] RTC init/deinit:%d fail!\r\n",is_en);
        return HAL_CAPTOUCH_STATUS_ERROR;
    }
    else {
        return HAL_CAPTOUCH_STATUS_OK;
    }
    #endif
    return HAL_CAPTOUCH_STATUS_OK;
}

bool captouch_rtc_lpm_control(hal_captouch_lowpower_type_t lowpower_type)
{
#ifdef RTC_CAPTOUCH_SUPPORTED
    hal_rtc_status_t rtc_ret;

    if (HAL_CAPTOUCH_MODE_LOWPOWER == lowpower_type) {
        rtc_ret = rtc_internal_captouch_lowpower(HAL_CAPTOUCH_MODE_LOWPOWER);
    }
    else {
        rtc_ret = rtc_internal_captouch_lowpower(HAL_CAPTOUCH_MODE_NORMAL);
    }

    if (rtc_ret != HAL_RTC_STATUS_OK) {
       HAL_CAPTOUCH_LOG_PRINT("lpm fail\r\n",0);
        return HAL_CAPTOUCH_STATUS_ERROR;
    }
    else {
        HAL_CAPTOUCH_LOG_PRINT("lpm sucess\r\n",0);
        return HAL_CAPTOUCH_STATUS_OK;
    }
#endif
}


void captouch_analog_init(void)
{
    uint32_t temp;
    temp = captouch->TOUCH_CON1.CON1;
    temp |= (TOUCH_BACK2BACK_EN | TOUCH_AUTO_DISABLE_CH | TOUCH_PER_CH_GATING_EN);
    captouch->TOUCH_CON1.CON1 = temp;
    captouch_set_clk(HAL_CAPTOUCH_MODE_NORMAL,HAL_CAPTOUCH_CLK_32K);

    temp = captouch->TOUCH_ANACFG1.ANACFG1;
    captouch->TOUCH_ANACFG1.ANACFG1 = temp | TOUCH_LDO_EN;
    hal_gpt_delay_us(50);
    captouch->TOUCH_ANACFG1.ANACFG1 = temp | TOUCH_LDO_EN | TOUCH_OP_EN;
    hal_gpt_delay_us(65);
    captouch->TOUCH_ANACFG1.ANACFG1 = temp | TOUCH_LDO_EN | TOUCH_OP_EN | TOUCH_ADC_EN;
    hal_gpt_delay_us(5);
    captouch->TOUCH_ANACFG0.ANACFG0 = (1<<16) | 0x2e;
}

void captouch_analog_deinit(void)
{
    uint32_t temp;
    temp = captouch->TOUCH_ANACFG1.ANACFG1;

    captouch->TOUCH_ANACFG0.ANACFG0 = (0<<16) | 0x2e;
    hal_gpt_delay_us(5);
    temp &= ~(TOUCH_ADC_EN);
    captouch->TOUCH_ANACFG1.ANACFG1= temp;
    hal_gpt_delay_us(5);
    temp &= ~(TOUCH_ADC_EN | TOUCH_OP_EN);
    captouch->TOUCH_ANACFG1.ANACFG1= temp;
    hal_gpt_delay_us(5);
    temp &= ~(TOUCH_ADC_EN | TOUCH_OP_EN | TOUCH_LDO_EN);
    captouch->TOUCH_ANACFG1.ANACFG1= temp;
}

void captouch_set_lpsd_chk_vbus(bool is_en)
{
	if(is_en)
		captouch->TOUCH_LPSDCON.LPSDCON |= (1 << 9);
	else
		captouch->TOUCH_LPSDCON.LPSDCON &= ~(1 << 9);
}

void captouch_set_vbus_flag(bool is_chgin)
{
	if(is_chgin)
		captouch->TOUCH_LPSDCON.LPSDCON |= (1 << 24);
	else
		captouch->TOUCH_LPSDCON.LPSDCON &= ~(1 << 24);
}

bool captouch_get_channel_trigger(hal_captouch_channel_t channel)
{
    uint32_t temp;
    int16_t touch_trig;

    captouch_switch_debug_sel(channel);
    temp = captouch->TOUCH_DBG0.DBG0;
    touch_trig = BIT_FIELD_EXTRACT32(temp,16,4);
    HAL_CAPTOUCH_LOG_PRINT("captouch captouch_event_check channel:%d, touch_trig:%x", 2, channel, touch_trig);
    return touch_trig & (1<<channel);

}

int8_t captouch_write_nvkey(hal_captouch_nvdm_id_t id ,hal_captouch_channel_t channel ,int16_t value ){


    hal_captouch_nvdm_data captouch_data;
    uint32_t length;
    nvkey_status_t nvkey_ret = NVKEY_STATUS_ERROR;
    length = sizeof(hal_captouch_nvdm_data);

    nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_CALI_4CHDATA, (uint8_t *)(&captouch_data), &length);
    if( nvkey_ret !=  NVKEY_STATUS_OK){
        HAL_CAPTOUCH_LOG_PRINT("captouch write_nvkey fail %d",1,nvkey_ret);
        return nvkey_ret;
    }
    switch (id) {
        case CAPTOUCH_COARSE_CAP_ID:
            captouch_data.coarse_cap[channel]= (uint8_t)value;
        break;
        case CAPTOUCH_FINE_CAP_ID:
            captouch_data.fine_cap[channel]= value;
        break;
        case CAPTOUCH_SW_TUNE_ID:
            captouch_data.swtune_en = (uint8_t)value;
            break;

        default:
        break;
    }

    nvkey_ret = nvkey_write_data(NVKEYID_CAPTOUCH_CALI_4CHDATA, (uint8_t *)(&captouch_data), length);
    return nvkey_ret;

}

uint8_t captouch_coarsecap_from_nvkey(uint8_t channel){

    hal_captouch_nvdm_data captouch_data;
    hal_captouch_tune_nvdm_data tune_data;
    uint32_t data_length,tune_data_length;
    nvkey_status_t nvkey_ret = NVKEY_STATUS_ERROR;
    tune_data_length = sizeof(hal_captouch_tune_nvdm_data);
    nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_TUNE_DATA, (uint8_t *)(&tune_data), &tune_data_length);
    if(nvkey_ret != NVKEY_STATUS_OK){
        data_length = sizeof(hal_captouch_nvdm_data);
        nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_CALI_4CHDATA, (uint8_t *)(&captouch_data), &data_length);
        if(nvkey_ret != NVKEY_STATUS_OK){
            HAL_CAPTOUCH_LOG_PRINT("captouch read coarse cap nvkey fail %d",1,nvkey_ret);
            return 0;
        }else{
            return captouch_data.coarse_cap[channel];
        }
    }else{
        return tune_data.coarseCapValue[channel];
    }

}

int8_t captouch_coarsecap_to_nvkey(uint8_t channel ,uint8_t value ){

    hal_captouch_nvdm_data captouch_data;
    hal_captouch_tune_nvdm_data tune_data;
    uint32_t data_length,tune_data_length;
    nvkey_status_t nvkey_ret = NVKEY_STATUS_ERROR;
    tune_data_length = sizeof(hal_captouch_tune_nvdm_data);
    nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_TUNE_DATA, (uint8_t *)(&tune_data), &tune_data_length);

    if(nvkey_ret != NVKEY_STATUS_OK){
        data_length = sizeof(hal_captouch_nvdm_data);
        nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_CALI_4CHDATA, (uint8_t *)(&captouch_data), &data_length);
        if(nvkey_ret == NVKEY_STATUS_OK){
            captouch_data.coarse_cap[channel]= value;
            nvkey_ret = nvkey_write_data(NVKEYID_CAPTOUCH_CALI_4CHDATA, (uint8_t *)(&captouch_data), data_length);
            HAL_CAPTOUCH_LOG_PRINT("captouch write coarse cap nvkey to fota coarse  %d ",1,value);
        }else{
            HAL_CAPTOUCH_LOG_PRINT("captouch no coarse cap nvkey ",0);
        }
    }else{
        tune_data.coarseCapValue[channel]  = value;
        nvkey_ret = nvkey_write_data(NVKEYID_CAPTOUCH_TUNE_DATA, (uint8_t *)(&tune_data), tune_data_length);
        HAL_CAPTOUCH_LOG_PRINT("captouch write coarse cap nvkey to nonfota coarse  %d ",1,value);
    }
    return nvkey_ret;
}

int8_t captouch_fine_cap_from_nvkey(uint8_t channel){


    hal_captouch_nvdm_data captouch_data;
    hal_captouch_tune_nvdm_data tune_data;
    uint32_t data_length,tune_data_length;
    nvkey_status_t nvkey_ret = NVKEY_STATUS_ERROR;
    tune_data_length = sizeof(hal_captouch_tune_nvdm_data);
    nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_TUNE_DATA, (uint8_t *)(&tune_data), &tune_data_length);
    if(nvkey_ret != NVKEY_STATUS_OK){
        data_length = sizeof(hal_captouch_nvdm_data);
        nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_CALI_4CHDATA, (uint8_t *)(&captouch_data), &data_length);
        if(nvkey_ret != NVKEY_STATUS_OK){
            HAL_CAPTOUCH_LOG_PRINT("captouch read coarse cap nvkey fail %d",1,nvkey_ret);
            return 0;
        }else{
            //printf("captouch read fine cap from fota  ");
            return captouch_data.fine_cap[channel];
        }
    }else{
        //printf("captouch read fine cap from non fota ");
        return tune_data.fineCap[channel];
    }

}


int8_t captouch_fine_cap_to_nvkey(uint8_t channel,int8_t value){

    hal_captouch_nvdm_data captouch_data;
    hal_captouch_tune_nvdm_data tune_data;
    uint32_t data_length,tune_data_length;
    nvkey_status_t nvkey_ret = NVKEY_STATUS_ERROR;
    tune_data_length = sizeof(hal_captouch_tune_nvdm_data);
    nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_TUNE_DATA, (uint8_t *)(&tune_data), &tune_data_length);

    if(nvkey_ret != NVKEY_STATUS_OK){
        data_length = sizeof(hal_captouch_nvdm_data);
        nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_CALI_4CHDATA, (uint8_t *)(&captouch_data), &data_length);
        if(nvkey_ret == NVKEY_STATUS_OK){
            captouch_data.fine_cap[channel]= value;
            nvkey_ret = nvkey_write_data(NVKEYID_CAPTOUCH_CALI_4CHDATA, (uint8_t *)(&captouch_data), data_length);
            HAL_CAPTOUCH_LOG_PRINT("captouch write fine cap nvkey to fota Fine %d ",1,value);
        }else{
            HAL_CAPTOUCH_LOG_PRINT("captouch no fine cap nvkey ",0);
        }
    }else{
        tune_data.fineCap[channel]  = value;
        nvkey_ret = nvkey_write_data(NVKEYID_CAPTOUCH_TUNE_DATA, (uint8_t *)(&tune_data), tune_data_length);
        HAL_CAPTOUCH_LOG_PRINT("captouch write fine cap nvkey to non fota Fine %d ",1,value);
    }
    return nvkey_ret;

}

uint8_t captouch_eardetect_enable(void){
    nvkey_status_t nvkey_ret = NVKEY_STATUS_ERROR;
    uint32_t length;
    length = sizeof(hal_captouch_eardetect_nvdm_data);
    nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_EARDETECT_DATA, (uint8_t *)(&eardetect_setting), &length);

    if( nvkey_ret !=  NVKEY_STATUS_OK ){

        return false;

    }else{
        if(!eardetect_setting.detect_ch)
            return false;
    }

    return true;

}




int16_t captouch_eardetect_base_from_nvkey(uint8_t channel)
{
    nvkey_status_t nvkey_ret = NVKEY_STATUS_ERROR;
    uint32_t length;
    int16_t mavg_adc;
    hal_captouch_tune_nvdm_data tune_nvdm_data;
    length = sizeof(hal_captouch_tune_nvdm_data);

    nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_TUNE_DATA, (uint8_t *)(&tune_nvdm_data), &length);

    //HAL_CAPTOUCH_LOG_PRINT("captouch sdwu  read_nvkey %d",1,nvkey_ret);
    //printf("captouch sdwu_en =%d sdwu_setting.sdtime =%d sdwu_setting.wutime=%d",sdwu_setting.sdwu_en,sdwu_setting.sdtime,sdwu_setting.wutime);
    if( nvkey_ret !=  NVKEY_STATUS_OK ){
        mavg_adc = captouch_get_mavg_adc(channel);
        if( mavg_adc>0 && mavg_adc<32 ){
             HAL_CAPTOUCH_LOG_PRINT("captouch earcheck base  =%d",1,captouch_get_fine_cap(channel)*114/4+mavg_adc);
             return captouch_get_fine_cap(channel)*114/4+mavg_adc;
        }else{
             HAL_CAPTOUCH_LOG_PRINT("captouch earcheck base =%d",1,captouch_get_fine_cap(channel)*114/4);
             return captouch_get_fine_cap(channel)*114/4;
        }

    }else{
        HAL_CAPTOUCH_LOG_PRINT("captouch earcheck base from nvkey =%d",1,tune_nvdm_data.earCheckBase[channel]);
        return tune_nvdm_data.earCheckBase[channel];
    }

}

void captouch_eardetect_base_to_nvkey(uint8_t channel,int16_t value){

    hal_captouch_tune_nvdm_data tune_nvdm_data;
    nvkey_status_t nvkey_ret = NVKEY_STATUS_ERROR;
    uint32_t length;
    length = sizeof(hal_captouch_tune_nvdm_data);
    nvkey_ret =nvkey_read_data(NVKEYID_CAPTOUCH_TUNE_DATA, (void *)&tune_nvdm_data, &length);
    if(nvkey_ret == NVKEY_STATUS_OK){
        tune_nvdm_data.earCheckBase[channel]= value;
        nvkey_ret = nvkey_write_data(NVKEYID_CAPTOUCH_TUNE_DATA, (uint8_t *)(&tune_nvdm_data), length);
        HAL_CAPTOUCH_LOG_PRINT("captouch write ear detect base[%d] to non fota nvkey_ret = %d ",2,tune_nvdm_data.earCheckBase[channel],nvkey_ret);
    }else{
        HAL_CAPTOUCH_LOG_PRINT("captouch write ear detect base nvkey fail =%d ",1,nvkey_ret);
    }

}
void captouch_findbase_after_ctrl_man(void )
{
    HAL_CAPTOUCH_LOG_PRINT("captouch disable manual mode in first release intr start", 0);
    captouch_set_control_manual(HAL_CAPTOUCH_CHANNEL_0, false);
    captouch_set_autok_suspend((1<<0), false);
    captouch_channel_sense_control((1<<0), captouch_disable);
    captouch_set_mavg(HAL_CAPTOUCH_CHANNEL_0, 8);
    captouch_channel_sense_control((1<<0), captouch_enable);

    xTimerStartFromISR(captouch_fine_base_delay100ms_timer, 0);
}

void captouch_fine_tune_delay_handler(TimerHandle_t xTimer)
{
    UNUSED(xTimer);

    captouch_channel_sense_control((1<<0), captouch_disable);
    captouch_set_mavg(HAL_CAPTOUCH_CHANNEL_0, captouch_context.mavg_r[HAL_CAPTOUCH_CHANNEL_0]);
    captouch_channel_sense_control((1<<0), captouch_enable);
    captouch_set_autok_suspend((1<<0), true);
    //captouch_fine_cap_to_nvkey(HAL_CAPTOUCH_CHANNEL_0, captouch_get_fine_cap(HAL_CAPTOUCH_CHANNEL_0));
    //HAL_CAPTOUCH_LOG_PRINT("captouch disable manual mode - fine base delay end channel[%d] write cfine = %d adc = %d", 3, HAL_CAPTOUCH_CHANNEL_0, captouch_get_fine_cap(HAL_CAPTOUCH_CHANNEL_0), captouch_get_avg_adc(HAL_CAPTOUCH_CHANNEL_0));
    HAL_CAPTOUCH_LOG_PRINT("captouch disable manual mode  in first release intr", 0);

    //lpsd config
    if(sdwu_setting.sdwu_en & (1<<HAL_CAPTOUCH_LONGPRESS_SHUTDOWN)) {
        if(autosuspend_timeout_data.lpsd_ch_bitmap){
                captouch_longpress_channel_select_control(HAL_CAPTOUCH_LONGPRESS_SHUTDOWN,\
                                                         autosuspend_timeout_data.lpsd_ch_bitmap,\
                                                         sdwu_setting.sdtime*LONGPRESS_TIME_S );
        }else{
            captouch_longpress_channel_control(HAL_CAPTOUCH_LONGPRESS_SHUTDOWN,sdwu_setting.sdtime*LONGPRESS_TIME_S);
        }
        //pmu_lpsd_rst_init(CAP_LPSD, PMU_ON);
        if(sdwu_setting.sdwu_en & (1<<HAL_CAPTOUCH_LONGPRESS_SHUTDOWN_VBUS)){
            captouch_set_vbus_flag(captouch_get_charge_in_status());
            captouch_set_lpsd_chk_vbus(true);
        }
    }
}

bool captouch_get_lpwu_int_flag(void)
{
	return captouch->TOUCH_LPWUFLAG.LPWUINT;
}

void captouch_protect_buffer(hal_captouch_channel_t channel ,bool *buffer_flag ,hal_captouch_send_type_t type ){
    uint32_t captouch_mask;
    hal_nvic_save_and_set_interrupt_mask(&captouch_mask);
    buffer_flag[channel]  = type;
    hal_nvic_restore_interrupt_mask(captouch_mask);
}

bool captouch_get_charge_in_status(void){

#if ( defined (MT2822) || defined(AB1568) )
     return pmu_get_chr_detect_value();
#elif ( defined(AB1565))
     return pmu_charger_is_plug();
#endif
    return 0;
}

bool captouch_is_feature_enable(void)
{

    hal_captouch_feature_nvdm_data captouch_feature_en;
    uint32_t length;
    nvkey_status_t nvkey_ret = NVKEY_STATUS_ERROR;
    length = sizeof(hal_captouch_feature_nvdm_data);

    nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_FEATURE, (uint8_t *)(&captouch_feature_en), &length);
    //HAL_CAPTOUCH_LOG_PRINT("captouch feature enable read_nvkey %d",1,nvkey_ret);
    if( nvkey_ret !=  NVKEY_STATUS_OK || !captouch_feature_en.IsCapTouchEnable ){
        HAL_CAPTOUCH_LOG_PRINT("captouch feature enable read_nvkey fail %d",1,nvkey_ret);
        return false;
    }
    return true;

}

uint8_t captouch_set_auto_tune_feature(uint8_t en)
{
    hal_captouch_nvdm_data captouch_data;
    uint32_t length;
    nvkey_status_t nvkey_ret = NVKEY_STATUS_ERROR;
    length = sizeof(hal_captouch_nvdm_data);

    captouch_write_nvkey(CAPTOUCH_SW_TUNE_ID, 0, en);
    nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_CALI_4CHDATA, (uint8_t *)(&captouch_data), &length);
    if( nvkey_ret !=  NVKEY_STATUS_OK){
        HAL_CAPTOUCH_LOG_PRINT("captouch read nvkey fail %d",1,nvkey_ret);
        return nvkey_ret;
    }

    return captouch_data.swtune_en;
}

uint8_t captouch_sdwusetting_from_NVKEY(void)
{

    nvkey_status_t nvkey_ret = NVKEY_STATUS_ERROR;
    uint32_t length;
    length = sizeof(hal_captouch_sdwu_nvdm_data);

    nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_SDWU_DATA, (uint8_t *)(&sdwu_setting), &length);
    //HAL_CAPTOUCH_LOG_PRINT("captouch sdwu  read_nvkey %d",1,nvkey_ret);
    if( nvkey_ret !=  NVKEY_STATUS_OK ){
        HAL_CAPTOUCH_LOG_PRINT("captouch sdwu  read_nvkey fail %d",1,nvkey_ret);
        return false;
    }else{
        return true;
    }
}


int16_t  captouch_get_avg_adc(hal_captouch_channel_t channel)
{
    uint32_t temp;
    uint16_t data=0;
    captouch_switch_debug_sel(channel);
    temp = captouch->TOUCH_DBG0.DBG0;
    data = BIT_FIELD_EXTRACT32(temp,0,16);
    return captouch_to16signed(9,data);
}

int16_t  captouch_get_mavg_adc(hal_captouch_channel_t channel)
{
    uint32_t temp;
    uint16_t data=0;
    captouch_switch_debug_sel(channel);
    temp = captouch->TOUCH_DBG1.DBG1;
    data = BIT_FIELD_EXTRACT32(temp,0,16);
    return captouch_to16signed(9,data);
}

void captouch_sw_ear_check(TimerHandle_t xTimer){

    UNUSED(xTimer);

    int16_t delt_cfine;
    int16_t mavg_adc;
    int16_t cifne_mavg;

    static uint16_t fine_cap_count = 0;
    static uint16_t outear_debounce_count = 0;

    //if(gMSG_HandlerRegisteredForSensor == NULL) return;

    mavg_adc = captouch_get_mavg_adc(captouch_ear_detect.detect_ch);
	if(mavg_adc <= 0 || mavg_adc >= 32 ){
        mavg_adc = 0;
    }

    cifne_mavg = captouch_get_fine_cap(captouch_ear_detect.detect_ch)*114/4 + mavg_adc ;
    delt_cfine = cifne_mavg  - captouch_ear_detect.ear_detect_base[captouch_ear_detect.detect_ch];

    /*
    HAL_CAPTOUCH_LOG_PRINT("captouch ear_detect_base = %d ear detect data = %d thr_high =%d thr_low =%d Delt fine =%d mavg_adc =%d MovAvg_n =%d  ",7,\
                            captouch_ear_detect.ear_detect_base[captouch_ear_detect.detect_ch],\
                            cifne_mavg ,\
                            captouch_context.highThre[captouch_ear_detect.detect_ch],\
                            captouch_context.lowThre[captouch_ear_detect.detect_ch],\
                            delt_cfine,\
                            mavg_adc  ,\
                            captouch_ear_detect.mavg_num);
   */

    if( delt_cfine >= captouch_context.highThre[captouch_ear_detect.detect_ch] && captouch_ear_detect.earinstatus == 0 ){

        captouch_ear_detect.earinstatus |= 1;
        HAL_CAPTOUCH_LOG_PRINT("===============  captouch  in ear =====================  earinstatus = %d ",1,captouch_ear_detect.earinstatus);
        captouch_push_one_event_to_buffer(captouch_ear_detect.detect_ch,HAL_CAP_TOUCH_KEY_PRESS,0);
        captouch_call_user_callback();
        if(captouch_ear_detect.baseKFrozeTime!=0){
            if(captouch_ear_detect.earBaseKFrozen == EAR_BAKSEK_FROZEN_START){
                //HAL_CAPTOUCH_LOG_PRINT("captouch baseK_Frozen close and earBaseKFrozen =%d ",1,captouch_ear_detect.earBaseKFrozen);
                xTimerStop(captouch_earcheck_stop_baseK_timer,0);
                captouch_ear_detect.earBaseKFrozen = EAR_BAKSEK_FROZEN_INIT;
            }
        }
        //MSG_MessageSendEx(gMSG_HandlerRegisteredForSensor, 0x100, NULL, captouch_ear_detect.earinstatus);

    }else if ( delt_cfine < ( captouch_context.lowThre[captouch_ear_detect.detect_ch]) && captouch_ear_detect.earinstatus == 1){
        HAL_CAPTOUCH_LOG_PRINT("captouch debounce count  =%d",1,outear_debounce_count);
        outear_debounce_count++;
        if(outear_debounce_count == 2){
            captouch_ear_detect.earinstatus &= 0;
            HAL_CAPTOUCH_LOG_PRINT("============== captouch out ear ====================  earinstatus =%d",1,captouch_ear_detect.earinstatus);
            captouch_push_one_event_to_buffer(captouch_ear_detect.detect_ch,HAL_CAP_TOUCH_KEY_RELEASE,0);
            captouch_call_user_callback();
            outear_debounce_count = 0;
        }
    }else if ( delt_cfine >= ( captouch_context.lowThre[captouch_ear_detect.detect_ch]  ) && (captouch_ear_detect.earinstatus == 0)&&\
                             (captouch_ear_detect.earBaseKFrozen == EAR_BAKSEK_FROZEN_INIT) && !captouch_ear_detect.stop_avg ){

        if(captouch_ear_detect.baseKFrozeTime!=0){
            //HAL_CAPTOUCH_LOG_PRINT("captouch baseK_Frozen open and earBaseKFrozen =%d ",1,captouch_ear_detect.earBaseKFrozen);
            captouch_ear_detect.earBaseKFrozen = EAR_BAKSEK_FROZEN_START;
            xTimerStart( captouch_earcheck_stop_baseK_timer,0 );
        }

    }else if (delt_cfine < ( captouch_context.lowThre[captouch_ear_detect.detect_ch]) ){

        outear_debounce_count = 0;

        if(captouch_ear_detect.baseKFrozeTime!=0 && captouch_ear_detect.earBaseKFrozen == EAR_BAKSEK_FROZEN_START){
            //HAL_CAPTOUCH_LOG_PRINT("captouch baseK_Frozen close and earBaseKFrozen =%d < lowThre ",1,captouch_ear_detect.earBaseKFrozen);
            captouch_ear_detect.earBaseKFrozen = EAR_BAKSEK_FROZEN_INIT;
            xTimerStop(captouch_earcheck_stop_baseK_timer,0);

        }
    }

    //cfine base moving average

    if( captouch_ear_detect.stop_avg ){

        if( fine_cap_count < captouch_ear_detect.mavg_num && (!captouch_ear_detect.earinstatus) ){
            captouch_ear_detect.fine_sum += cifne_mavg;
            fine_cap_count++;
        }else if (fine_cap_count == captouch_ear_detect.mavg_num ){
            captouch_ear_detect.ear_detect_base[captouch_ear_detect.detect_ch] = captouch_ear_detect.fine_sum / captouch_ear_detect.mavg_num;
            captouch_ear_detect.stop_avg = false;
        }
    }else{
         if(!captouch_ear_detect.earinstatus && (captouch_ear_detect.earBaseKFrozen != EAR_BAKSEK_FROZEN_START) ){
            captouch_ear_detect.fine_sum = captouch_ear_detect.fine_sum+delt_cfine;
            captouch_ear_detect.ear_detect_base[captouch_ear_detect.detect_ch] = captouch_round( (captouch_ear_detect.fine_sum),captouch_ear_detect.mavg_num );
            HAL_CAPTOUCH_LOG_PRINT("captouch  update ear check baseline= %d ",1,captouch_ear_detect.ear_detect_base[captouch_ear_detect.detect_ch]);
         }else {
            HAL_CAPTOUCH_LOG_PRINT("captouch  no update ear check baseline =%d ",1,captouch_ear_detect.ear_detect_base[captouch_ear_detect.detect_ch]);
         }
    }
}

void captouch_stop_earcheck_basek_timeout(TimerHandle_t xTimer){
    UNUSED(xTimer);
    captouch_ear_detect.earBaseKFrozen = EAR_BAKSEK_FROZEN_INIT;
    int16_t mavg_adc_new;
    int16_t cifne_mavg_new;
    mavg_adc_new = captouch_get_mavg_adc(captouch_ear_detect.detect_ch);

    if(mavg_adc_new <= 0 || mavg_adc_new >= 32 ){
        mavg_adc_new = 0;
    }

    cifne_mavg_new = captouch_get_fine_cap(captouch_ear_detect.detect_ch)*114/4 + mavg_adc_new ;

    captouch_ear_detect.fine_sum = cifne_mavg_new * captouch_ear_detect.mavg_num;

    captouch_ear_detect.ear_detect_base[captouch_ear_detect.detect_ch] =  cifne_mavg_new;

    HAL_CAPTOUCH_LOG_PRINT("captouch baseK_Frozen Close and earBaseKFrozen =%d new ear base =%d ",2,\
                                                                captouch_ear_detect.earBaseKFrozen,\
                                                                captouch_ear_detect.ear_detect_base[captouch_ear_detect.detect_ch]);
}


int16_t captouch_round(int32_t val1, int16_t val2){
    int32_t result = 0, tmp = 0;

    if (val2)
    {
        result = val1/val2;
        tmp = (10*val1/val2)%10;

        if (tmp > 4)
            result++;
        else if (tmp < -4)
            result--;
        //HAL_CAPTOUCH_LOG_PRINT("captouch_round fail, val1[%d], val2[%d], tmp[%d], result[%d]", 4, val1, val2, tmp, result);
    }
    else
    {
        HAL_CAPTOUCH_LOG_PRINT("captouch_round fail, val1[%d], val2[%d]", 2, val1, val2);
        assert(0);
    }

    return (int16_t)result;
}

bool captouch_autosuspend_timeout_enable(void)
{
    nvkey_status_t nvkey_ret = NVKEY_STATUS_ERROR;
    uint32_t length;
    length = sizeof(hal_captouch_autosuspend_timeout_nvdm_data);
    nvkey_ret = nvkey_read_data (NVKEYID_CAPTOUCH_AUTOSUSPEND_DATA, (uint8_t *)(&autosuspend_timeout_data), &length);

    if( nvkey_ret !=  NVKEY_STATUS_OK ){

        return false;

    }else{
        if(!autosuspend_timeout_data.ch_bitmap)
            return false;
    }
    return true;
}

void captouch_close_autosuspend_timeout(uint8_t channel){
    hal_gpt_status_t gpt_status;
    autosuspend_timeout_context.captouch_autosuspend_event[channel].channel=channel;
    gpt_status =hal_gpt_sw_get_timer(&captouch_close_autosuspend_timer[channel]);

    HAL_CAPTOUCH_LOG_PRINT("captouch close autosuspend_time out ch%d",1,channel);

    if (gpt_status != HAL_GPT_STATUS_OK) {
        HAL_CAPTOUCH_LOG_PRINT("captouch hal_gpt_sw_get close_autosuspend fail :%d ",1 ,gpt_status );
    }

    //hal_gpt_sw_free_timer(captouch_debounce_timer[press_channel]);
    gpt_status =hal_gpt_sw_start_timer_ms(captouch_close_autosuspend_timer[channel],\
                                          autosuspend_timeout_context.time[channel]*1000,\
                                          captouch_close_autosuspend_timeout_handler,\
                                          (void*)&autosuspend_timeout_context.captouch_autosuspend_event[channel].channel);

    if (gpt_status != HAL_GPT_STATUS_OK) {
      HAL_CAPTOUCH_LOG_PRINT("captouch hal_gpt_sw_start_close_autosuspend timer_ms fail:%d ",1 ,gpt_status );
    }

}

void captouch_close_autosuspend_timeout_handler( void *user_data){
    uint8_t press_chanel;
    hal_gpt_status_t gpt_status;

    press_chanel =  *(uint8_t*)user_data;
    if(captouch_get_channel_trigger(press_chanel)){
        hal_captouch_set_avg(press_chanel,5,captouch_context.avg_s[press_chanel]);
        captouch_set_autok_suspend(1<<press_chanel,false);
        //HAL_CAPTOUCH_LOG_PRINT("--------------  captouch close_autosuspend  channel[%d] -----------",1,press_chanel);

        gpt_status =hal_gpt_sw_get_timer(&captouch_delay_timer[press_chanel]);

        if (gpt_status != HAL_GPT_STATUS_OK) {
            HAL_CAPTOUCH_LOG_PRINT("captouch hal_gpt_sw_get_timer fail:%d ",1 ,gpt_status );
        }

        gpt_status =hal_gpt_sw_start_timer_ms(captouch_delay_timer[press_chanel],\
                                              200,\
                                              captouch_open_autosuspend_timeout_handler,\
                                             (void*)&autosuspend_timeout_context.captouch_autosuspend_event[press_chanel].channel);

        if (gpt_status != HAL_GPT_STATUS_OK) {
            HAL_CAPTOUCH_LOG_PRINT("captouch hal_gpt_sw_start_timer fail:%d ",1 ,gpt_status );
        }
    }

    gpt_status = hal_gpt_sw_free_timer(captouch_close_autosuspend_timer[press_chanel]);
    if(gpt_status != HAL_GPT_STATUS_OK){
         HAL_CAPTOUCH_LOG_PRINT("hal_gpt_sw_get close_autosuspend handler fail :%d ",1 ,gpt_status );
    }

}


void captouch_open_autosuspend_timeout_handler(void *user_data){

    hal_gpt_status_t gpt_status;
    int16_t avg_adc;
    uint8_t open_autosuspend_channel;

    open_autosuspend_channel =  *(uint8_t*)user_data;

    avg_adc = captouch_get_avg_adc(open_autosuspend_channel);

    HAL_CAPTOUCH_LOG_PRINT("captouch delay check to open_autosuspend  channel =%d",1,open_autosuspend_channel);

    if( (avg_adc < 40)  &&  (avg_adc> -40 ) ){
        hal_captouch_set_avg(open_autosuspend_channel,captouch_context.mavg_r[open_autosuspend_channel],captouch_context.avg_s[open_autosuspend_channel]);
        captouch_set_autok_suspend(1<<open_autosuspend_channel,true);
        hal_gpt_sw_free_timer(captouch_delay_timer[open_autosuspend_channel]);
    }else{
        gpt_status =hal_gpt_sw_start_timer_ms(captouch_delay_timer[open_autosuspend_channel],\
                                              200,\
                                              captouch_open_autosuspend_timeout_handler,\
                                              (void*)&autosuspend_timeout_context.captouch_autosuspend_event[open_autosuspend_channel].channel);
        if(gpt_status!= HAL_GPT_STATUS_OK){
             HAL_CAPTOUCH_LOG_PRINT(" captouch_open_captouch_delay_timer fail :%d ",1 ,gpt_status );
        }
    }

}

void captouch_init_parameter_print(bool capcon_state, bool lpwu_flag, bool isChargerIn, uint8_t ear_detect_ch, bool autosuspend_timeout_en)
{
    HAL_CAPTOUCH_LOG_PRINT("captouch init capcon_state:%d, lpwu_flag:%d, isChargerIn:%d, ear_detect_ch:%d, autosuspend_timeout_en:%d", 5,
                                                                                                            capcon_state, lpwu_flag, isChargerIn, ear_detect_ch, autosuspend_timeout_en);
}

void captouch_send_press_msg(void)
{
    hal_captouch_event_t hal_captouch_event;

    hal_captouch_event.key_data = 0;
    hal_captouch_event.state = HAL_CAP_TOUCH_KEY_PRESS;
    hal_captouch_event.time_stamp = 0;

    gsend_flag[hal_captouch_event.key_data] = SEND_RELEASSE;

    captouch_push_one_event_to_buffer(hal_captouch_event.key_data,\
                                      hal_captouch_event.state,\
                                      hal_captouch_event.time_stamp);

    captouch_context.debounce_state[hal_captouch_event.key_data] =\
                                    hal_captouch_event.state*100;
    HAL_CAPTOUCH_LOG_PRINT("captouch send lpwu press event to top channel = %d  state = %d debounce_state = %d",\
                              3,\
                              hal_captouch_event.key_data,\
                              hal_captouch_event.state,\
                              captouch_context.debounce_state[hal_captouch_event.key_data]);
    captouch_call_user_callback();

    if((autosuspend_timeout_context.ch_bitmap & (1<< hal_captouch_event.key_data)) && \
        (autosuspend_timeout_context.time[hal_captouch_event.key_data])){
        captouch_close_autosuspend_timeout(hal_captouch_event.key_data);
    }

    lpwu_press = true;
}


#if 0
VOID captouch_find_baseline(hal_captouch_channel_t channel)
{
    U16 temp;
    int16_t vadc_dbg;
    U32 time1;
    U32 time2;
    captouch_switch_debug_sel(channel);
    HAL_CAPTOUCH_LOG_PRINT("captouch_find_baseline channel %d ", 1,channel);

    time1 =DRV_TIMER_READ_COUNTER();
    HAL_CAPTOUCH_LOG_PRINT("captouch_find_baseline time1 %d ", 1,time1);
    while(1)
    {

        temp = captouch_get_adc_val();
        vadc_dbg  =captouch_to16signed(9,temp);
        //LOG_I(LOG_OS, "test1 0xdd[%d]  ", 1,DRV_3WIRE_Read(TOUCH_AVG_DBG));
        //LOG_I(LOG_OS, "test1 0xdd [%d]  ", 1,vadc_dbg);
        time2 =DRV_TIMER_READ_COUNTER();

         if(vadc_dbg <=40 && vadc_dbg >=(-40)) {
             HAL_CAPTOUCH_LOG_PRINT( "captouch_find_baseline time1 %d", 1,time2);
             break;
         }
         if((time2 - time1)>500000) {
             //LOG_I(LOG_OS, "captouch_find_baseline time1 [%d]  ", 1,time2);
             HAL_CAPTOUCH_LOG_PRINT("captouch_find_baseline fail", 0);
             break;
         }
    }

}
#endif


#endif //HAL_CAPTOUCH_MODULE_ENABLED

