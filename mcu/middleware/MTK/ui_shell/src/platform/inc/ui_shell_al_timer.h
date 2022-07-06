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

#ifndef __UI_SHELL_AL_TIMER_H__
#define __UI_SHELL_AL_TIMER_H__

#include <stdint.h>
#include <stdbool.h>

/************************************************************************
 * While the timer is timeout, this method will be called
 ************************************************************************/
typedef void (*ui_shell_al_timer_handle_method)(void *);

typedef void * Timer_Handle_t;

/************************************************************************
 * Method to create a timer
 *
 * For 7687     : TimerHandle_t xTimerCreate(const char *pcTimerName,
 *                                           const TickType_t xTimerPeriod,
 *                                           const UBaseType_t uxAutoReload,
 *                                           void * const pvTimerID,
 *                                           TimerCallbackFunction_t pxCallbackFunction )
 *
 * For Win32    : Maybe it's just the SetTimer()
 *
 * timer_name   : ONLY for 7687, for debug
 *
 * timer_ms  :
 *
 * pv_timer_id  : ONLY for 7687, for timer id
 *
 * auto_reload  : Means auto reset the timer, maybe auto run after timeout
 *
 * handler      : Timeout handle method
 *
 ************************************************************************/
Timer_Handle_t ui_shell_al_timer_create(char *timer_name,
                               uint32_t timer_ms,
                               uint32_t auto_reload,
                               void *pv_timer_id,
                               ui_shell_al_timer_handle_method hanlder);


/************************************************************************
 * Method to start the timer
 *
 * For 7687     : BaseType_t xTimerStart( TimerHandle_t xTimer, TickType_t xTicksToWait ) 
 *
 * For Win32    : Maybe begin to dispatch the message
 *
 * timer_handler: ONLY for 7687, the return value from ui_shell_al_timer_create
 *
 * timer_to_wait: How long the timer to wait
 *
 ************************************************************************/
int32_t ui_shell_al_timer_start(Timer_Handle_t timer_handler,
                          uint16_t  time_to_wait);

/************************************************************************
 * Method to stop the timer
 *
 * For 7687     : BaseType_t xTimerStop( TimerHandle_t xTimer, TickType_t xTicksToWait ) 
 *
 * For Win32    : Maybe just stop to dispatch the message
 *
 * timer_handler    : ONLY for 7687, the return value of ui_shell_al_timer_create
 *
 * time_to_wait     : Maybe no meanings
 *
 ************************************************************************/
int32_t ui_shell_al_timer_stop(Timer_Handle_t timer_handler,
                         uint16_t    time_to_wait);

/************************************************************************
 * Method to delete the timer
 *
 * For 7687     : BaseType_t xTimerDelete( TimerHandle_t xTimer, TickType_t xTicksToWait ) 
 *
 * For Win32    : Maybe no meanings
 *
 * timer_handler    : ONLY for 7687, the return value of ui_shell_al_timer_create
 *
 * timer_to_wait    : Maybe no meanings
 *
 ************************************************************************/
int32_t ui_shell_al_timer_delete(Timer_Handle_t  timer_handler,
                           uint16_t  timer_to_wait);


uint32_t ui_shell_al_get_current_ms();


bool ui_shell_al_is_time_larger(uint32_t a, uint32_t b);

#endif /* __UI_SHELL_AL_TIMER_H__ */
