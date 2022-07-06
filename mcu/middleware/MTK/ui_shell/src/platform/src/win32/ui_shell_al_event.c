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

//#include "stdafx.h"
#include <Windows.h>

#include "ui_shell_al_event.h"

Event_Group_Handle_t ui_shell_al_event_create(bool init_state)
{
    HANDLE handle;
    handle = CreateEvent(NULL,
                         true,
                         init_state == true ? true : false,
                         NULL);
    return (Event_Group_Handle_t)handle;
}

void ui_shell_al_event_delete(Event_Group_Handle_t ev_to_delete)
{
    CloseHandle((HANDLE)ev_to_delete);
    return;
}

uint16_t ui_shell_al_event_set(Event_Group_Handle_t ev_handle,
                        uint16_t   ev_bit_to_set)
{
    SetEvent((HANDLE)ev_handle);
    return 0;
}

uint16_t ui_shell_al_event_reset(Event_Group_Handle_t ev_handle,
                          uint16_t   ev_bit_to_reset)
{
    ResetEvent((HANDLE)ev_handle);
    return 0;
}

ui_shell_al_event_wait_result_t ui_shell_al_event_wait(Event_Group_Handle_t ev_handle,
                                     uint16_t ev_bit_to_wait,
                                     uint32_t xClearOnExit,
                                     uint32_t xWaitForAllBits,
                                     uint16_t xTicksToWait)
{
    DWORD d = WaitForSingleObject((HANDLE)ev_handle, xTicksToWait);
    if (d == WAIT_TIMEOUT)
    {
        return AL_EVENT_SIGNAL_TIMEOUT;
    }
    if (d == WAIT_OBJECT_0)
    {
        return AL_EVENT_SIGNAL_OBJECT_SET;
    }
    return AL_EVENT_SIGNAL_WAIT_FAILED;
}
