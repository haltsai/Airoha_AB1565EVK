/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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

#include "ui_shell_activity_heap.h"
#include "FreeRTOS.h"
#include <string.h>

typedef struct {
    ui_shell_activity_internal_t activity;
    uint16_t index_in_array;
} ui_shell_activity_heap_item_t;

static ui_shell_activity_heap_item_t s_ui_shell_activity_heap_array[MTK_UI_SHELL_MAX_ACTIVITY_COUNT];

ui_shell_activity_internal_t *ui_shell_activity_heap_get_preproc_item(void)
{
    memset(&s_ui_shell_activity_heap_array[0], 0, sizeof(ui_shell_activity_heap_item_t));
    return &s_ui_shell_activity_heap_array[0].activity;
}

ui_shell_activity_internal_t *ui_shell_activity_heap_get_free_item(void)
{
    uint16_t i;
    /* s_ui_shell_activity_heap_array[0] is used for preproc activity */
    for (i = MTK_UI_SHELL_MAX_ACTIVITY_COUNT - 1; i > 0; i--) {
        if (s_ui_shell_activity_heap_array[i].index_in_array == 0) {
            memset(&s_ui_shell_activity_heap_array[i], 0, sizeof(ui_shell_activity_internal_t));
            s_ui_shell_activity_heap_array[i].index_in_array = i;
            return &s_ui_shell_activity_heap_array[i].activity;
        }
    }
    configASSERT(0);
    return NULL;
}

void ui_shell_activity_heap_free_item(ui_shell_activity_internal_t *activity)
{
    ui_shell_activity_heap_item_t *activity_item = (ui_shell_activity_heap_item_t *)activity;
    configASSERT(activity
            && activity_item->index_in_array < MTK_UI_SHELL_MAX_ACTIVITY_COUNT
            && activity_item == &s_ui_shell_activity_heap_array[activity_item->index_in_array]);
    activity_item->index_in_array = 0;
}