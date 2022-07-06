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

#ifndef __UI_SHELL_AL_EVENT_H__
#define __UI_SHELL_AL_EVENT_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum {

    AL_EVENT_SIGNAL_TIMEOUT,
    AL_EVENT_SIGNAL_OBJECT_SET,
    AL_EVENT_SIGNAL_WAIT_FAILED,

} ui_shell_al_event_wait_result_t;

typedef void * Event_Group_Handle_t;

/************************************************************************
 * Create a event to handle
 * 
 * For 7687     : EventGroupHandle_t xEventGroupCreate( void )
 *
 * For Win32    : HANDLE CreateEvent(LPSECURITY_ATTRIBUTES lpEventAttributes,   // Security attributes, normally is NULL
                                     BOOL bManualReset,                         // Reset method, 1: manual, 0: auto
                                     BOOL bInitialState,                        // Init state 1: available, 0: unavailable
                                     LPCTSTR lpName);                           // The object name
 *
 * init_state   : ONLY for Win32, means the default signal is available or NOT
 *
 ************************************************************************/
Event_Group_Handle_t ui_shell_al_event_create(bool init_state);


/************************************************************************
 * Delete a event handler
 * 
 * For 7687     : void vEventGroupDelete(EventGroupHandle_t xEventGroup)
 *
 * For Win32    : Maybe CloseHandle
 *
 * ev_to_delete : Should be the return value of alevent_create
 *
 ************************************************************************/
void ui_shell_al_event_delete(Event_Group_Handle_t ev_to_delete);

/************************************************************************
 * Set the signal to be available
 * 
 * For 7687     : EventBits_t xEventGroupSetBits(EventGroupHandle_t xEventGroup,
                                                 const EventBits_t uxBitsToSet )
 *
 * For Win32    : SetEvent(HANDLE);         The handle should be the return value of alevent_create
 *
 * ev_handle    : Should be the return value of alevent_create
 *
 * ev_bit_to_set: ONLY for 7687, to set which bit is available
 *
 ************************************************************************/
uint16_t ui_shell_al_event_set(Event_Group_Handle_t   ev_handle,
                         uint16_t   ev_bit_to_set);

/************************************************************************
 * Set the signal to be unavailable
 *
 * For 7687     : EventBits_t xEventGroupClearBits( EventGroupHandle_t xEventGroup,
 *                                                  const EventBits_t uxBitsToClear )
 *
 * For Win32    : ResetEvent(HANDLE)
 *
 * ev_handle    : Should be the return value of alevent_create
 *
 * ev_bit_to_rest: ONLY for 7687, to set which bit is unavailable
 *
 ************************************************************************/
uint16_t ui_shell_al_event_reset(Event_Group_Handle_t  ev_handle,
                           uint16_t   ev_bit_to_reset);

/************************************************************************
 * Wait the signal
 *
 * For 7687     : EventBits_t xEventGroupWaitBits(const EventGroupHandle_t xEventGroup,
 *                                                const EventBits_t uxBitsToWaitFor,
 *                                                const BaseType_t xClearOnExit,
 *                                                const BaseType_t xWaitForAllBits,
 *                                                TickType_t xTicksToWait )
 *
 * For Win32    : DWORD WaitForSignalObject(HANDLE hObject,         // The object from create
 *                                          DWORD dwMilliseconds);  // How long to wait the object
 *
 * ev_handle    : Should be the return value of alevent_create
 *
 * ev_bit_to_wait:  ONLY for 7687, which bit th wait, should be equal to set and rest
 *
 * d_mil_seconds : How long to wait the signal
 *
 ************************************************************************/
ui_shell_al_event_wait_result_t ui_shell_al_event_wait(Event_Group_Handle_t ev_handle,
                                     uint16_t    ev_bit_to_wait,
                                     uint32_t    xClearOnExit,
                                     uint32_t    xWaitForAllBits,
                                     uint16_t    xTicksToWait);




#endif /* __UI_SHELL_AL_EVENT_H__ */
