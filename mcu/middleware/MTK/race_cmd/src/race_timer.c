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


#include "race_cmd_feature.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "race_xport.h"
#include "race_timer.h"
#include "race_util.h"
#include "race_lpcomm_retry.h"
#include "race_lpcomm_partner.h"


/* smart timer: app_id, ping, rho+spp_need_disc/commit_id */
#define RACE_TIMER_LIST_MAX_NUM  (5)
#define RACE_TIMER_BASE_TIMER_ID  (1)


static TimerHandle_t g_race_timer;

race_timer_list_struct g_race_timer_list[RACE_TIMER_LIST_MAX_NUM];


static void race_timer_hdlr(TimerHandle_t timer_id)
{
    race_general_msg_t msg_queue_item = {0};

    /*
         * Send msg to race cmd task only.
         */
    msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_TIMER_EXPIRATION_IND;
    race_send_msg(&msg_queue_item);
}


RACE_ERRCODE race_timer_start(uint32_t delay_msec)
{
    RACE_LOG_MSGID_I("period: %d ms",1, delay_msec);
    if (!g_race_timer)
    {
        g_race_timer = xTimerCreate( "race_timer",
                                    (delay_msec / portTICK_PERIOD_MS),
                                    pdTRUE, /* Repeat timer */
                                    NULL,
                                    race_timer_hdlr);
    }

    if (!g_race_timer)
    {
        return RACE_ERRCODE_FAIL;
    }

    if (xTimerIsTimerActive(g_race_timer) == pdFALSE)
    {
        xTimerStart(g_race_timer, 0);
    }

    return RACE_ERRCODE_SUCCESS;
}


static bool race_timer_stop(void)
{
    RACE_LOG_MSGID_I("race_timer_stop", 0);
    if (!g_race_timer)
    {
        return TRUE;
    }

    if (pdPASS == xTimerDelete(g_race_timer, 0))
    {
        g_race_timer = NULL;
        return TRUE;
    }

    return FALSE;
}


RACE_ERRCODE race_timer_smart_start(uint8_t *id,
                                         uint32_t timeout,
                                         race_timer_expiration_hdl hdl,
                                         void *user_data)
{
    int32_t i = 0;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;

    if (!id || !hdl)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    *id = RACE_TIMER_INVALID_TIMER_ID;

    for (i = 0; i < RACE_TIMER_LIST_MAX_NUM; i++)
    {
        if (FALSE == g_race_timer_list[i].is_used)
        {
            if (g_race_timer_list[i].user_data)
            {
                RACE_LOG_MSGID_W("user_data is not NULL for unused timer id:%d",1, i);
            }
            memset(&g_race_timer_list[i], 0, sizeof(race_timer_list_struct));
            g_race_timer_list[i].is_used = TRUE;
            g_race_timer_list[i].timestamp = race_get_curr_time_in_ms();
            g_race_timer_list[i].timeout = timeout;
            g_race_timer_list[i].hdl = hdl;
            g_race_timer_list[i].user_data = user_data;

            ret = race_timer_start(1000);
            if (RACE_ERRCODE_SUCCESS != ret)
            {
                g_race_timer_list[i].is_used = FALSE;
            }
            else
            {
                *id = i + RACE_TIMER_BASE_TIMER_ID;
            }

            break;
        }
    }

    if (RACE_ERRCODE_SUCCESS != ret)
    {
        RACE_LOG_MSGID_E("Failed to start timer. i:%d",1, i);
    }

    return ret;
}


RACE_ERRCODE race_timer_smart_reset(uint8_t id)
{
    id -= RACE_TIMER_BASE_TIMER_ID;

    if (id < RACE_TIMER_LIST_MAX_NUM &&
        g_race_timer_list[id].is_used)
    {
        g_race_timer_list[id].timestamp = race_get_curr_time_in_ms();
    }

    return RACE_ERRCODE_SUCCESS;
}


bool race_timer_smart_is_enabled(uint8_t id)
{
    id -= RACE_TIMER_BASE_TIMER_ID;

    if (id < RACE_TIMER_LIST_MAX_NUM &&
        g_race_timer_list[id].is_used)
    {
        return TRUE;
    }

    return FALSE;
}


RACE_ERRCODE race_timer_smart_change_period(uint8_t id,
                                                      uint32_t timeout)
{
    id -= RACE_TIMER_BASE_TIMER_ID;

    if (id < RACE_TIMER_LIST_MAX_NUM &&
        g_race_timer_list[id].is_used)
    {
        g_race_timer_list[id].timeout = timeout;

        return RACE_ERRCODE_SUCCESS;
    }

    return RACE_ERRCODE_FAIL;
}


void race_timer_smart_stop(uint8_t id, void **user_data)
{
    id -= RACE_TIMER_BASE_TIMER_ID;

    if (id < RACE_TIMER_LIST_MAX_NUM)
    {
        if (user_data)
        {
            *user_data = g_race_timer_list[id].user_data;
        }
        g_race_timer_list[id].user_data = NULL;
        g_race_timer_list[id].is_used = FALSE;
    }
}

void race_timer_expiration_msg_process(void)
{
#ifdef RACE_LPCOMM_RETRY_ENABLE
    bool timer_in_use = FALSE;
#endif
    bool stop_timer = TRUE;
    uint8_t i = 0;

    RACE_LOG_MSGID_I("race_timer_expiration_msg_process", 0);
#ifdef RACE_LPCOMM_RETRY_ENABLE
    race_lpcomm_retry_timer_expiration_process(&timer_in_use);
    if (timer_in_use)
    {
        stop_timer = FALSE;
    }
#endif

    for (i = 0; i < RACE_TIMER_LIST_MAX_NUM; i++)
    {
        if (g_race_timer_list[i].is_used &&
            g_race_timer_list[i].hdl)
        {
            uint32_t curr_timestamp = race_get_curr_time_in_ms();
            uint32_t timer_interval = race_get_duration_in_ms(g_race_timer_list[i].timestamp, curr_timestamp);

            if (g_race_timer_list[i].timeout <= timer_interval ||
                (RACE_TIMER_TIMEOUT_DELTA_IN_MS >= (g_race_timer_list[i].timeout - timer_interval)))
            {
                /* Timeout */
                /* Repeat timer */
                g_race_timer_list[i].timestamp = race_get_curr_time_in_ms();
                g_race_timer_list[i].hdl(i + RACE_TIMER_BASE_TIMER_ID, g_race_timer_list[i].user_data);
            }

            RACE_LOG_MSGID_I("i:%d is_used:%d",2, i, g_race_timer_list[i].is_used);

            if (g_race_timer_list[i].is_used)
            {
                stop_timer = FALSE;
            }
        }
        else if (g_race_timer_list[i].is_used &&
                 !g_race_timer_list[i].hdl)
        {
            RACE_LOG_MSGID_W("i:%d has no hdl. user_data:%x",2, i, g_race_timer_list[i].user_data);
            /* Convert to timer id from index. */
            i += RACE_TIMER_BASE_TIMER_ID;
            race_timer_smart_stop(i, NULL);
        }
    }

    if (stop_timer)
    {
        race_timer_stop();
    }
}

