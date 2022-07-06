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

#include <string.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "hal_log.h"
#include "hal_gpt.h"

#include "hal_keypad_table.h"
#include "airo_key_event.h"
#include "airo_key_event_internal.h"

airo_key_event_context_t airo_key_event_context;
airo_key_event_state_t   airo_key_event_state;
#ifdef MTK_GSENSOR_KEY_ENABLE
airo_key_gsensor_state_t airo_gsensor_state;
#endif

airo_key_event_config_t  airo_key_config[AIRO_KEY_SUPPORT_NUMBER] = AIRO_KEY_MAPPING;

#define NO_ACTION       0xffffffff
#define DUMMY           0xdeadbeef

typedef struct {
    uint8_t             id;                 // state id for debug
    uint32_t            state;              // internal state machine
    uint32_t            event;              // if no need send event, should be 0xffffffff--NO_ACTION
    uint32_t            number_check;       // if no need check,should be 0xffffffff--NO_ACTION
    uint32_t            max_check;          // click or long level max value,if no should be NO_ACTION
    uint32_t            continue_no_ack;    // if want continue without any action, should be 0x1, otherwise,should be 0xffffffff--NO_ACTION
    uint32_t            support;            // if 0xffffffff, support none
    uint32_t            timer;              // if no need start time,should be 0xffffffff--NO_ACTION
} key_next_state_t;

typedef struct {
    key_next_state_t release;               /*id: 0*/
    key_next_state_t idle;                  /*id: 1*/

    key_next_state_t press_1;               /*id: 2*/
    key_next_state_t release_1_temp;        /*id: 3*/
    key_next_state_t short_click_1;         /*id: 4*/
    key_next_state_t short_click_1_dummy;   /*id: 5*/

    key_next_state_t release_1;             /*id: 6*/
    key_next_state_t short_click_2;         /*id: 7*/

    key_next_state_t press_2;               /*id: 8*/
    key_next_state_t release_2_temp;        /*id: 9*/
    key_next_state_t double_click_1;        /*id: 10*/
    key_next_state_t double_click_1_dummy;  /*id: 11*/

    key_next_state_t release_2;             /*id: 12*/
    key_next_state_t double_click_2;        /*id: 13*/

    key_next_state_t slong;                 /*id: 14*/
    key_next_state_t slong_repeat;          /*id: 15*/
    key_next_state_t dlong;                 /*id: 16*/
    key_next_state_t dlong_repeat;          /*id: 17*/

    key_next_state_t press_3;               /*id: 18*/
    key_next_state_t release_3_temp;        /*id: 19*/
    key_next_state_t triple_click_1;        /*id: 20*/
    key_next_state_t triple_click_1_dummy;  /*id: 21*/
    key_next_state_t release_3;             /*id: 22*/

    key_next_state_t long_1_temp;           /*id: 23*/
    key_next_state_t long_1;                /*id: 24*/
    key_next_state_t long_2_temp;           /*id: 25*/
    key_next_state_t long_2;                /*id: 26*/
    key_next_state_t long_3_temp;           /*id: 27*/
    key_next_state_t long_3;                /*id: 28*/


    key_next_state_t long_repeat_temp;      /*id: 29*/
    key_next_state_t long_repeat;           /*id: 30*/
    key_next_state_t invalid;               /*id: 31*/
    key_next_state_t dummy_click;   /*id: 32*/
    key_next_state_t dummy_press;   /*id: 33*/
    key_next_state_t dummy_release;   /*id: 34*/
} airo_key_state_confg_t;

#define KEY_CHECK_CLICK(x)      ((x<<8) + SUPPORT_MULTIPLE_CLICK)
#define KEY_CHECK_LONG(x)       ((x<<8) + SUPPORT_LONG_LEVEL)

airo_key_event_config_t p_key1;

#define AIRO_KEY_NULL       0


const airo_key_state_confg_t key_next_state = {
                           /*id*//*state*/                 /*send event*/     /*click number check*//*max_check*//*continue_no_ack*//*support*/         /*timer*/
    /*release*/             {0, key_state_release,          AIRO_KEY_RELEASE,       NO_ACTION,          NO_ACTION,  1,          SUPPORT_PRESS_RELEASE,  NO_ACTION},
    /*idlle*/               {1, key_state_idle,             NO_ACTION,              NO_ACTION,          NO_ACTION,  NO_ACTION,  NO_ACTION,              NO_ACTION},

    /*press_1*/             {2, key_state_press_1,          AIRO_KEY_PRESS,         NO_ACTION,          NO_ACTION,  NO_ACTION,  SUPPORT_PRESS_RELEASE,  TIME_LONG1},
    /*release_1_temp*/      {3, key_state_release_1_temp,   AIRO_KEY_RELEASE,       KEY_CHECK_CLICK(1), NO_ACTION,  NO_ACTION,  SUPPORT_PRESS_RELEASE,  NO_ACTION},
    /*short_click_1*/       {4, key_state_short_click_1,    AIRO_KEY_SHORT_CLICK,   NO_ACTION,          1,          1,  SUPPORT_MULTIPLE_CLICK, NO_ACTION},
    /*short_click_1_dummy*/ {5, key_state_short_click_1,    NO_ACTION,              NO_ACTION,          NO_ACTION,  NO_ACTION,  NO_ACTION,              TIME_SILENCE},

    /*release_1*/           {6, key_state_release_1,        NO_ACTION,              NO_ACTION,          NO_ACTION,  NO_ACTION,  NO_ACTION,              TIME_RELEASE},
    /*short_click_2*/       {7, key_state_short_click_2,    AIRO_KEY_SHORT_CLICK,   NO_ACTION,          1,          1,          SUPPORT_MULTIPLE_CLICK, NO_ACTION},


    /*press_2*/             {8, key_state_press_2,          AIRO_KEY_PRESS,         NO_ACTION,          NO_ACTION,  NO_ACTION,  SUPPORT_PRESS_RELEASE,  TIME_SLONG},
    /*release_2_temp*/      {9, key_state_release_2_temp,   AIRO_KEY_RELEASE,       KEY_CHECK_CLICK(2), NO_ACTION,  NO_ACTION,  SUPPORT_PRESS_RELEASE,  NO_ACTION},
    /*double_click_1*/      {10,key_state_double_click_1,   AIRO_KEY_DOUBLE_CLICK,  NO_ACTION,          2,          1,  SUPPORT_MULTIPLE_CLICK, NO_ACTION},
    /*double_click_1_dummy*/{11,key_state_double_click_1,   NO_ACTION,              NO_ACTION,          NO_ACTION,  NO_ACTION,  NO_ACTION,              TIME_SILENCE},

    /*release_2*/           {12,key_state_release_2,        NO_ACTION,              NO_ACTION,          NO_ACTION,  NO_ACTION,  NO_ACTION,              TIME_RELEASE},
    /*double_click_2*/      {13,key_state_double_click_2,   AIRO_KEY_DOUBLE_CLICK,  NO_ACTION,          2,          1,          SUPPORT_MULTIPLE_CLICK, NO_ACTION},

    /*slong*/               {14,key_state_slong,            AIRO_KEY_SLONG,         NO_ACTION,          NO_ACTION,  NO_ACTION,  SUPPORT_SLONG,          TIME_REPEAT},
    /*slong_repeat*/        {15,key_state_slong_repeat,     AIRO_KEY_REPEAT,        NO_ACTION,          NO_ACTION,  NO_ACTION,  SUPPORT_SLONG_REPEAT,   TIME_REPEAT},

    /*dlong*/               {16,key_state_dlong,            AIRO_KEY_DLONG,         NO_ACTION,          NO_ACTION,  NO_ACTION,  SUPPORT_SLONG,          TIME_REPEAT},
    /*dlong_repeat*/        {17,key_state_dlong_repeat,     AIRO_KEY_REPEAT,        NO_ACTION,          NO_ACTION,  NO_ACTION,  SUPPORT_SLONG_REPEAT,   TIME_REPEAT},
    /*press_3*/             {18,key_state_press_3,          AIRO_KEY_PRESS,         NO_ACTION,          NO_ACTION,  NO_ACTION,  SUPPORT_PRESS_RELEASE,  TIME_PRESS},
    /*release_3_temp*/      {19,key_state_release_3_temp,   AIRO_KEY_RELEASE,       KEY_CHECK_CLICK(3), NO_ACTION,  NO_ACTION,  SUPPORT_PRESS_RELEASE,  NO_ACTION},
    /*triple_click_1*/      {20,key_state_triple_click_1,   AIRO_KEY_TRIPLE_CLICK,  NO_ACTION,          3,          1,  SUPPORT_MULTIPLE_CLICK, NO_ACTION},
    /*triple_click_1_dumy*/ {21,key_state_triple_click_1,   NO_ACTION,              NO_ACTION,          NO_ACTION,  NO_ACTION,  NO_ACTION,              TIME_SILENCE},
    /*release_3*/           {22,key_state_release_3,        NO_ACTION,              NO_ACTION,          NO_ACTION,  NO_ACTION,  NO_ACTION,              NO_ACTION},
    /*long_1_temp*/         {23,key_state_long_1_temp,      AIRO_KEY_LONG_PRESS_1,  KEY_CHECK_LONG(1),  1,          NO_ACTION,  SUPPORT_LONG_LEVEL,     NO_ACTION},
    /*long_1*/              {24,key_state_long_1,           NO_ACTION,              NO_ACTION,          NO_ACTION,  NO_ACTION,  NO_ACTION,              TIME_LONG2},
    /*long_2_temp*/         {25,key_state_long_2_temp,      AIRO_KEY_LONG_PRESS_2,  KEY_CHECK_LONG(2),  2,          NO_ACTION,  SUPPORT_LONG_LEVEL,     NO_ACTION},
    /*long_2*/              {26,key_state_long_2,           NO_ACTION,              NO_ACTION,          NO_ACTION,  NO_ACTION,  NO_ACTION,              TIME_LONG3},
    /*long_3_temp*/         {27,key_state_long_3_temp,      AIRO_KEY_LONG_PRESS_3,  KEY_CHECK_LONG(3),  3,          NO_ACTION,  SUPPORT_LONG_LEVEL,     NO_ACTION},
//if long level >3, column timer shoud be next long press time
    /*long_3*/              {28,key_state_long_3,           NO_ACTION,              NO_ACTION,          NO_ACTION,  NO_ACTION,  NO_ACTION,              NO_ACTION},
    /*long_repeat_temp*/    {29,key_state_long_repeat_temp, NO_ACTION,              NO_ACTION,          NO_ACTION,  NO_ACTION,  NO_ACTION,              TIME_REPEAT},
    /*long_repeat*/         {30,key_state_long_repeat,      AIRO_KEY_REPEAT,        NO_ACTION,          NO_ACTION,  1,          SUPPORT_LONG_REPEAT,    NO_ACTION},
    /*invalid*/             {31,key_state_invalid,          AIRO_KEY_INVALID,       NO_ACTION,          NO_ACTION,  NO_ACTION,  NO_ACTION,              NO_ACTION},
    /*dummy_click*/         {32,key_state_dummy_click,      NO_ACTION,              NO_ACTION,          NO_ACTION,  NO_ACTION,  NO_ACTION,              TIME_SILENCE},
    /*dummy_press*/         {33,key_state_dummy_press,      AIRO_KEY_PRESS,         NO_ACTION,          NO_ACTION,  NO_ACTION,  SUPPORT_PRESS_RELEASE,  NO_ACTION},
    /*dummy_release*/       {34,key_state_dummy_release,    AIRO_KEY_RELEASE,       NO_ACTION,          NO_ACTION,  1        ,  SUPPORT_PRESS_RELEASE,  NO_ACTION},
};

// row: current state, column: next state
const uint32_t key_state_machine_table[key_state_max][KEY_ACTION_MAX] = {
                                    /*0:KEY_ACTION_PRESS*/         /*1:KEY_action_keep press*/                  /*2:KEY_ACTION_RELEASE*/                        /*3:KEY_action_keep release*/            /*4:reach click max:Yes*/                 /*5:reach click max:No*/             /*6:KEY_ACTION_CONTINUE_NO_ACTION*/
    /*0:key_state_idle*/           {(uint32_t) &key_next_state.press_1, AIRO_KEY_NULL,                          (uint32_t) &key_next_state.release,                                          AIRO_KEY_NULL,                           AIRO_KEY_NULL,                             AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*1:key_state_press_1*/        {AIRO_KEY_NULL,                      (uint32_t) &key_next_state.long_1_temp, (uint32_t) &key_next_state.release_1_temp,      AIRO_KEY_NULL,                           AIRO_KEY_NULL,                             AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*2:key_state_release_1_temp*/ {AIRO_KEY_NULL,                      AIRO_KEY_NULL,                          AIRO_KEY_NULL,                                  AIRO_KEY_NULL,                          (uint32_t) &key_next_state.short_click_1,   (uint32_t) &key_next_state.release_1, AIRO_KEY_NULL},
    /*3:key_state_short_click_1*/  {DUMMY,                              DUMMY,                                  (uint32_t) &key_next_state.dummy_click,         (uint32_t) &key_next_state.idle,         AIRO_KEY_NULL,                             AIRO_KEY_NULL,                        (uint32_t) &key_next_state.dummy_click},
    /*4:key_state_short_click_2*/  {AIRO_KEY_NULL,                      AIRO_KEY_NULL,                          AIRO_KEY_NULL,                                  AIRO_KEY_NULL,                           AIRO_KEY_NULL,                             AIRO_KEY_NULL,                       (uint32_t) &key_next_state.idle},
    /*5:key_state_release_1*/      {(uint32_t) &key_next_state.press_2, AIRO_KEY_NULL,                          AIRO_KEY_NULL,                                  (uint32_t) &key_next_state.short_click_2,AIRO_KEY_NULL,                             AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*6:key_state_press_2*/        {AIRO_KEY_NULL,                      (uint32_t) &key_next_state.slong,       (uint32_t) &key_next_state.release_2_temp,      AIRO_KEY_NULL,                           AIRO_KEY_NULL,                             AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*7:key_state_slong*/          {AIRO_KEY_NULL,                      (uint32_t) &key_next_state.slong_repeat,(uint32_t) &key_next_state.release,             AIRO_KEY_NULL,                           AIRO_KEY_NULL,                             AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*8:key_state_slong_repeat*/   {DUMMY,                              (uint32_t) &key_next_state.slong_repeat,(uint32_t) &key_next_state.release,             AIRO_KEY_NULL,                           AIRO_KEY_NULL,                             AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*9:key_state_release_2_temp*/ {AIRO_KEY_NULL,                      AIRO_KEY_NULL,                          AIRO_KEY_NULL,                                  AIRO_KEY_NULL,                          (uint32_t) &key_next_state.double_click_1,  (uint32_t) &key_next_state.release_2, AIRO_KEY_NULL},
    /*10:key_state_double_click_1*/{DUMMY,                              DUMMY,                                  (uint32_t) &key_next_state.dummy_click,(uint32_t) &key_next_state.idle,         AIRO_KEY_NULL,                             AIRO_KEY_NULL,                        (uint32_t) &key_next_state.dummy_click},
    /*11:key_state_double_click_2*/{AIRO_KEY_NULL,                      AIRO_KEY_NULL,                          AIRO_KEY_NULL,                                  AIRO_KEY_NULL,                           AIRO_KEY_NULL,                             AIRO_KEY_NULL,                        (uint32_t) &key_next_state.idle},
    /*12:key_state_release_2*/     {(uint32_t) &key_next_state.press_3, AIRO_KEY_NULL,                          AIRO_KEY_NULL,                                  (uint32_t) &key_next_state.double_click_2,AIRO_KEY_NULL,                            AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*13:key_state_press_3*/       {AIRO_KEY_NULL,                      (uint32_t) &key_next_state.dlong,     (uint32_t) &key_next_state.release_3_temp,      AIRO_KEY_NULL,                           AIRO_KEY_NULL,                             AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*14:key_state_dlong*/         {AIRO_KEY_NULL,                      (uint32_t) &key_next_state.dlong_repeat,(uint32_t) &key_next_state.release,             AIRO_KEY_NULL,                           AIRO_KEY_NULL,                             AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*15:key_state_dlong_repeat*/  {DUMMY,                              (uint32_t) &key_next_state.dlong_repeat,(uint32_t) &key_next_state.release,             AIRO_KEY_NULL,                           AIRO_KEY_NULL,                             AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*16:key_state_invalid*/       {DUMMY,                              DUMMY,                                  (uint32_t) &key_next_state.release,             AIRO_KEY_NULL,                           AIRO_KEY_NULL,                             AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*17:key_state_triple_click_1*/{DUMMY,                              DUMMY,                                  (uint32_t) &key_next_state.dummy_click,(uint32_t) &key_next_state.idle,         AIRO_KEY_NULL,                             AIRO_KEY_NULL,                        (uint32_t) &key_next_state.dummy_click},
//because of just support 3 click, so KEY_STATE_triple_click_2 as all AIRO_KEY_NULL!!!
    /*18:key_state_triple_click_2*/{AIRO_KEY_NULL,                      AIRO_KEY_NULL,                          AIRO_KEY_NULL,                                  AIRO_KEY_NULL,                          AIRO_KEY_NULL,                              AIRO_KEY_NULL,                        (uint32_t) &key_next_state.idle},
    /*19:key_state_release_3_temp*/{AIRO_KEY_NULL,                      AIRO_KEY_NULL,                          AIRO_KEY_NULL,                                  AIRO_KEY_NULL,                          (uint32_t) &key_next_state.triple_click_1,  (uint32_t) &key_next_state.release_3, AIRO_KEY_NULL},
    /*20:key_state_release*/       {AIRO_KEY_NULL,                      AIRO_KEY_NULL,                          AIRO_KEY_NULL,                                  AIRO_KEY_NULL,                          AIRO_KEY_NULL,                              AIRO_KEY_NULL,                        (uint32_t) &key_next_state.idle},
//if multiple click >3, the column 0 shoubld be next press_4, the column 3 should be triple_click_2.
    /*21:key_state_release_3*/     {AIRO_KEY_NULL,                      AIRO_KEY_NULL,                          (uint32_t) &key_next_state.release,             AIRO_KEY_NULL,                          AIRO_KEY_NULL,                              AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*22:key_state_long_1_temp*/   {AIRO_KEY_NULL,                      AIRO_KEY_NULL,                          (uint32_t) &key_next_state.release,             AIRO_KEY_NULL,                          (uint32_t) &key_next_state.long_repeat_temp,(uint32_t) &key_next_state.long_1,    AIRO_KEY_NULL},
    /*23:key_state_long_1*/        {AIRO_KEY_NULL,                      (uint32_t) &key_next_state.long_2_temp, (uint32_t) &key_next_state.release,             AIRO_KEY_NULL,                          AIRO_KEY_NULL,                              AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*24:key_state_long_2_temp*/   {AIRO_KEY_NULL,                      AIRO_KEY_NULL,                          (uint32_t) &key_next_state.release,             AIRO_KEY_NULL,                          (uint32_t) &key_next_state.long_repeat_temp,(uint32_t) &key_next_state.long_2,    AIRO_KEY_NULL},
    /*25:key_state_long_2*/        {AIRO_KEY_NULL,                      (uint32_t) &key_next_state.long_3_temp, (uint32_t) &key_next_state.release,             AIRO_KEY_NULL,                          AIRO_KEY_NULL,                              AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*26:key_state_long_3_temp*/   {AIRO_KEY_NULL,                      AIRO_KEY_NULL,                          (uint32_t) &key_next_state.release,             AIRO_KEY_NULL,                          (uint32_t) &key_next_state.long_repeat_temp,AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*27:key_state_long_3*/        {AIRO_KEY_NULL,                      AIRO_KEY_NULL,                          (uint32_t) &key_next_state.release,             AIRO_KEY_NULL,                          AIRO_KEY_NULL,                              AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*28:key_state_long_repeat*/   {AIRO_KEY_NULL,                      AIRO_KEY_NULL,                          (uint32_t) &key_next_state.release,             AIRO_KEY_NULL,                          AIRO_KEY_NULL,                              AIRO_KEY_NULL,                        (uint32_t) &key_next_state.long_repeat_temp},
    /*29:key_state_long_repeat_temp*/{AIRO_KEY_NULL,                    (uint32_t) &key_next_state.long_repeat, (uint32_t) &key_next_state.release,             (uint32_t) &key_next_state.long_repeat, AIRO_KEY_NULL,                              AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*30:key_state_dummy_click*/   {(uint32_t) &key_next_state.dummy_press,AIRO_KEY_NULL,                       (uint32_t) AIRO_KEY_NULL,                       (uint32_t) &key_next_state.idle,        AIRO_KEY_NULL,                              AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*31:key_state_dummy_press*/   {AIRO_KEY_NULL,                      DUMMY,                                  (uint32_t) &key_next_state.dummy_release,       AIRO_KEY_NULL,                          AIRO_KEY_NULL,                              AIRO_KEY_NULL,                        AIRO_KEY_NULL},
    /*32:key_state_dummy_release*/ {(uint32_t) &key_next_state.dummy_press,AIRO_KEY_NULL,                       AIRO_KEY_NULL,                                  AIRO_KEY_NULL,                          AIRO_KEY_NULL,                              AIRO_KEY_NULL,                        (uint32_t) &key_next_state.dummy_click},
};


uint8_t airo_key_get_key_name(uint8_t index)
{
    return airo_key_event_state.key_data[index];
}

void airo_key_start_timer(uint8_t index, uint32_t time)
{
    hal_gpt_status_t ret_state;

    if (airo_key_event_state.is_timer_started[index] == false) {
        ret_state = hal_gpt_sw_get_timer(&airo_key_event_state.timer_handle[index]);

        if (ret_state != HAL_GPT_STATUS_OK) {
            LOG_MSGID_I(common, "[airo_key][timer]get timer handle error,ret = %d, handle = 0x%x,index=%d\r\n",3, \
                         (unsigned int)ret_state, \
                         (unsigned int)airo_key_event_state.timer_handle[index], index);
        } else {
#ifdef DEBUG_AIRO_TIMER_LOG
            LOG_MSGID_I(common, "[airo_key][timer]allocate timer handle=0x%x,index=%d\r\n",2, airo_key_event_state.timer_handle[index], index);
#endif
        }

        /*start timer*/
        airo_key_event_state.is_timer_started[index] = true;

        ret_state = hal_gpt_sw_start_timer_ms(airo_key_event_state.timer_handle[index], \
                                              time, \
                                              (hal_gpt_callback_t)airo_key_multiple_press_process, \
                                              (void *)(&airo_key_event_state.position[index]));
        if (ret_state != HAL_GPT_STATUS_OK) {
            LOG_MSGID_I(common, "[airo_key][timer]start timer handle=0x%x error,ret = %d, state=%d,index=%d\r\n",4, \
                         airo_key_event_state.timer_handle[index], ret_state, airo_key_event_state.airo_state[index], index);
        } else {
#ifdef DEBUG_AIRO_TIMER_LOG
            LOG_MSGID_I(common, "[airo_key][timer]start time handle=0x%x,index=%d,time=%d\r\n",3, airo_key_event_state.timer_handle[index], index, time);
#endif
        }
    }

}

void airo_key_stop_timer(uint8_t index)
{
    hal_gpt_status_t ret_state;

    if (airo_key_event_state.is_timer_started[index] == true) {
        ret_state = hal_gpt_sw_stop_timer_ms(airo_key_event_state.timer_handle[index]);
        if (ret_state != HAL_GPT_STATUS_OK) {
            LOG_MSGID_I(common, "[airo_key][timer]stop timer handle error,ret = %d, handle = 0x%x, index=%d\r\n",3, \
                         (int)ret_state, \
                         (int)airo_key_event_state.timer_handle[index], index);
        } else {
#ifdef DEBUG_AIRO_TIMER_LOG
            LOG_MSGID_I(common, "[airo_key][timer]stop time handle=0x%x,index=%d\r\n",2, airo_key_event_state.timer_handle[index], index);
#endif
        }

        /*free timer*/
        ret_state = hal_gpt_sw_free_timer(airo_key_event_state.timer_handle[index]);
        if (ret_state != HAL_GPT_STATUS_OK) {
            LOG_MSGID_I(common, "[airo_key][timer]free timer handle error,ret = %d, handle = 0x%x\r\n",2, \
                         (int)ret_state, \
                         (int)airo_key_event_state.timer_handle[index]);
        } else {
#ifdef DEBUG_AIRO_TIMER_LOG
            LOG_MSGID_I(common, "[airo_key][timer]free time handle=0x%x,index=%d\r\n",2, airo_key_event_state.timer_handle[index], index);
#endif
        }
        airo_key_event_state.is_timer_started[index] = false;
    }
}


uint8_t airo_key_name_align_check(uint8_t index, uint8_t key_name)
{
    uint8_t i;

    for (i = 0; i < AIRO_KEY_SUPPORT_NUMBER; i++) {
        if (airo_key_config[i].key_data == key_name) {
#ifdef DEBUG_AIRO_FLOW_LOG
            LOG_MSGID_I(common, "i=%d,config key_data=%d, key_name=%d\r\n",3, i, airo_key_config[i].key_data, key_name);
#endif
            return i;
        }
    }

    LOG_MSGID_I(common, "[airo_key][err] ept tool key name is not align with config key name\r\n", 0);
    LOG_MSGID_I(common, "[airo_key][err] please project owner sync with airo_key driver owner what happened\r\n", 0);
    LOG_MSGID_I(common, "[airo_key][err] ept key name=%d(0x%x), index=%d\r\n", 3, key_name, key_name, index);
    assert(0);
    return 0xFF;
}

void airo_key_multiple_press_process(uint8_t *key_type)
{
    uint8_t index;
    uint8_t key_name;
    uint8_t config_index;
    index = *key_type;

#ifdef DEBUG_AIRO_PERFORMANCE_LOG
    uint32_t exe_time[2];
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,  &exe_time[0]);
#endif

#ifdef DEBUG_AIRO_BASIC_FLOW_LOG
    LOG_MSGID_I(common, "[airo_key][timer]enter timer process, index=%d, previous_action=%d\r\n",2, index, airo_key_event_state.previous_action[index]);
#endif

    /*get key symbol name*/
    key_name = airo_key_get_key_name(index);
    config_index = airo_key_name_align_check(index, key_name);
    if (config_index == 0xFF) {
        return;
    }
    if (airo_key_event_state.is_timer_started[index] == true) {
        hal_gpt_sw_free_timer(airo_key_event_state.timer_handle[index]); // timer has be stopped, only free
        airo_key_event_state.is_timer_started[index] = false;
    }

    if (airo_key_event_state.previous_action[index] == KEY_ACTION_PRESS) {
        airo_key_state_machine(index, config_index, key_name, KEY_ACTION_KEEP_PRESS);
    } else if (airo_key_event_state.previous_action[index] == KEY_ACTION_RELEASE) {
        airo_key_state_machine(index, config_index, key_name, KEY_ACTION_KEEP_RELEASE);
    } else {
        LOG_MSGID_I(common, "[airo_key][timer]airo_key_multiple_press_process error! index=%d, previous=%d\r\n",2, index, airo_key_event_state.previous_action[index]);
        assert(0);
        return;
    }
#ifdef DEBUG_AIRO_FLOW_LOG
    LOG_MSGID_I(common, "[airo_key][timer]exit timer proces\r\n", 0);
#endif

#ifdef DEBUG_AIRO_PERFORMANCE_LOG
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,  &exe_time[1]);
    LOG_MSGID_I(common, "[airo_key][timer] exe_time=%d\r\n",1, exe_time[1] - exe_time[0]);
#endif

}

uint8_t airo_key_find_index(airo_key_mapping_event_t *key_event, airo_key_source_type_t type)
{
    uint8_t index;

    /*check powerkey or keypad index number*/

    index = 0xff;
    switch (type) {

#ifdef MTK_KEYPAD_ENABLE
        case AIRO_KEY_KEYPAD: {
            index = key_event->key_data;
            airo_key_event_state.key_data[index] = keypad_custom_translate_keydata(index);
        }
        break;
#endif

#ifdef AIRO_KEY_FEATRURE_POWERKEY
        case AIRO_KEY_POWERKEY: {
            index = AIRO_KEY_KEYPAD_NUMBER;
            airo_key_event_state.key_data[index] = key_event->key_data;
        }
        break;
#endif


#ifdef HAL_CAPTOUCH_MODULE_ENABLED
        case AIRO_KEY_CAPTOUCH: {
            index = AIRO_KEY_KEYPAD_NUMBER + AIRO_KEY_POWERKEY_NUMBER + key_event->key_data;
            hal_captouch_translate_channel_to_symbol(key_event->key_data, &airo_key_event_state.key_data[index]);
        }
        break;
#endif

#ifdef MTK_EINT_KEY_ENABLE
        case AIRO_KEY_EINT_KEY: {
            index =  key_event->key_data - BSP_EINT_KEY_DATA0 + AIRO_KEY_KEYPAD_NUMBER + AIRO_KEY_POWERKEY_NUMBER + AIRO_KEY_CAPTOUCH_NUMBER;
            airo_key_event_state.key_data[index] = key_event->key_data;

        }
        break;
#endif
#ifdef AIR_PSENSOR_KEY_ENABLE
        case AIRO_KEY_PSENSOR: {
            index =  AIRO_EINT_KEY_NUMBER + AIRO_KEY_KEYPAD_NUMBER + AIRO_KEY_POWERKEY_NUMBER + AIRO_KEY_CAPTOUCH_NUMBER;
            airo_key_event_state.key_data[index] = key_event->key_data;

        }
        break;
#endif
#ifdef AIR_BSP_INEAR_ENABLE
        case AIRO_KEY_INEAR_TOUCH: {
            index =  key_event->key_data - DEVICE_KEY_0 + AIRO_PSENSOR_KEY_NUMBER + AIRO_EINT_KEY_NUMBER + AIRO_KEY_KEYPAD_NUMBER + AIRO_KEY_POWERKEY_NUMBER + AIRO_KEY_CAPTOUCH_NUMBER;
            airo_key_event_state.key_data[index] = key_event->key_data;
        }
        break;
#endif
        default: {
            LOG_MSGID_I(common, "[airo_key][find] oder the index error, data=%d,type=%d\r\n",2, key_event->key_data, type);
            assert(0);
            return 0xFF;
        }

    }

    return index;


}

void airo_key_callbak(airo_key_event_t event, uint8_t key_name)
{
#ifdef DEBUG_AIRO_FLOW_LOG
    LOG_MSGID_I(common, "send event: event=%d,key_name=%d\r\n",2,event,key_name);
#endif
    airo_key_event_context.callback(event, key_name, airo_key_event_context.user_data);
}

void airo_key_callbak_ex(airo_key_event_t event, uint8_t key_name, uint8_t index)
{
    static uint32_t airo_key_last_event[AIRO_KEY_SUPPORT_NUMBER];

    /* This is a special case, when the last event is AIRO_KEY_LONG_PRESS_x(x=1/2/3 ...) or AIRO_KEY_SLONG or AIRO_KEY_DLONG
     * and the current event is AIRO_KEY_RELEASE, an extra event will be send to up layer before current event.
     */
    if (event == AIRO_KEY_RELEASE) {
        if ((airo_key_last_event[index] & AIRO_KEY_TYPE_MASK) == AIRO_KEY_LONG_PRESS_TYPE_MASK) {
                airo_key_callbak((airo_key_last_event[index] & (~AIRO_KEY_TYPE_MASK)) + AIRO_KEY_LONG_PRESS_RELEASE_TYPE_MASK, key_name);
        } else if (airo_key_last_event[index] == AIRO_KEY_SLONG) {
            airo_key_callbak(AIRO_KEY_SLONG_RELEASE, key_name);
        } else if (airo_key_last_event[index] == AIRO_KEY_DLONG) {
            airo_key_callbak(AIRO_KEY_DLONG_RELEASE, key_name);
        }
    }
    airo_key_last_event[index] = event;
    airo_key_callbak(event, key_name);
}

void airo_key_state_machine(uint8_t index, uint8_t config_index, uint8_t key_name, key_action_type_t action)
{
    volatile key_next_state_t *p_key_note;
    volatile uint32_t *p_time;
    volatile uint8_t  *p_support;
    volatile uint32_t  number_click;
    volatile uint32_t  temp[3];

#ifdef DEBUG_AIRO_FLOW_LOG
    LOG_MSGID_I(common, "[airo_key][state] index=%d,key_name=%d,pre_int_act=%d, action=%d\r\n",4, airo_key_event_state.previous_action[index], index, key_name, action);
#endif

    p_key_note = (key_next_state_t *)key_state_machine_table[airo_key_event_state.airo_state[index]][action];

    if (p_key_note == AIRO_KEY_NULL) {
        LOG_MSGID_I(common, "[airo_key][state] p_key_note = AIRO_KEY_NULL\r\n", 0);
        assert(0);
        return;
    }

    if (p_key_note == (void *)DUMMY) {
        return;
    }

#ifdef DEBUG_AIRO_BASIC_FLOW_LOG
    LOG_MSGID_I(common, "[airo_key][state] current: row=%d,col=%d, state id=%d\r\n",3, airo_key_event_state.airo_state[index], action, p_key_note->id);
#endif

    airo_key_event_state.airo_state[index] = p_key_note->state;

#ifdef DEBUG_AIRO_BASIC_FLOW_LOG
    LOG_MSGID_I(common, "[airo_key][state] next: row = %d\r\n",1, airo_key_event_state.airo_state[index]);
#endif
    p_support  = (uint8_t *)(((uint32_t)&airo_key_config[config_index].support) + p_key_note->support);
    p_time     = (uint32_t *)(((uint32_t)&airo_key_config[config_index].time)  + p_key_note->timer * 4);

#ifdef DEBUG_AIRO_STATE_LOG
    LOG_MSGID_I(common, "[airo_key][state] p_key_note->event=%x,*p_support=%x\r\n",2, p_key_note->event, *p_support);
#endif


    if ((p_key_note->event != NO_ACTION) && (p_key_note->support != NO_ACTION) && (*p_support != 0)) {
#ifdef DEBUG_AIRO_STATE_LOG
        LOG_MSGID_I(common, "[airo_key][state] p_key_note->max_check=%x\r\n",1, p_key_note->max_check);
#endif
        if (p_key_note->max_check != NO_ACTION) {
            if (p_key_note->max_check <= *p_support) {
                airo_key_callbak_ex(p_key_note->event, key_name, index);
            }
        } else {
            airo_key_callbak_ex(p_key_note->event, key_name, index);
        }
    } else if (p_key_note->event == AIRO_KEY_INVALID) {
        airo_key_callbak_ex(p_key_note->event, key_name, index);
    }

    /*If the key is configured to only support hardware press and release event, the timer do not need to start up. */
    if ((airo_key_config[config_index].support.multiple_click != 0) || (airo_key_config[config_index].support.long_level != 0)) {
        if (p_key_note->timer != NO_ACTION) {
            airo_key_stop_timer(index);
            airo_key_start_timer(index, *p_time);
        } else {
            airo_key_stop_timer(index);
        }
    }

    if (p_key_note->number_check != NO_ACTION) {
        number_click = p_key_note->number_check;
        temp[0] = number_click >> 8;
        temp[1] = number_click & 0xff;
        temp[2] = *((uint8_t *)(((uint32_t)&airo_key_config[config_index].support) + temp[1]));

        if (temp[0] >= temp[2]) {

#ifdef DEBUG_AIRO_FLOW_LOG
            LOG_MSGID_I(common, "[airo_key][state] enter KEY_ACTION_CLICK_NUMBER_YES\r\n", 0);
#endif

            airo_key_state_machine(index, config_index, key_name, KEY_ACTION_CLICK_NUMBER_YES);
        } else {

#ifdef DEBUG_AIRO_FLOW_LOG
            LOG_MSGID_I(common, "[airo_key][state] enter KEY_ACTION_CLICK_NUMBER_NO\r\n", 0);
#endif

            airo_key_state_machine(index, config_index, key_name, KEY_ACTION_CLICK_NUMBER_NO);
        }
    }

    if (p_key_note->continue_no_ack != NO_ACTION) {

#ifdef DEBUG_AIRO_FLOW_LOG
        LOG_MSGID_I(common, "[airo_key][state] enter NO_ACTION =%d\r\n",1, p_key_note->continue_no_ack);
#endif

        airo_key_state_machine(index, config_index, key_name, KEY_ACTION_CONTINUE_NO_ACTION);

#ifdef DEBUG_AIRO_FLOW_LOG
        LOG_MSGID_I(common, "[airo_key][state] exit NO_ACTION\r\n", 0);
#endif
    }


}

#ifdef MTK_GSENSOR_KEY_ENABLE
void gsensor_timeout(void *user_data)
{
    if (airo_gsensor_state.is_silence == true) {
        airo_gsensor_state.is_silence = false;
        airo_gsensor_state.key_count  = 0;
        return;
    }

    airo_key_callbak(airo_gsensor_state.key_count, airo_gsensor_state.key_data);
    airo_gsensor_state.key_count = 0;
}

void airo_key_gsensor_process(airo_key_mapping_event_t *key_event)
{
    //log_hal_info("state=%d airo_gsensor_state.key_count=%d\r\n",key_event->state,airo_gsensor_state.key_count);
    if ((key_event->state == AIRO_KEY_DRIVEN_SINGLE_TAP) || (key_event->state == AIRO_KEY_DRIVEN_DOUBLE_TAP)) {

        hal_gpt_sw_stop_timer_ms(airo_gsensor_state.timer_handle);

        airo_gsensor_state.key_data = key_event->key_data;

        //check silence
        if (airo_gsensor_state.is_silence == true) {
            hal_gpt_sw_start_timer_ms(airo_gsensor_state.timer_handle, \
                                      airo_key_gsensor_config.t_silence, \
                                      (hal_gpt_callback_t)gsensor_timeout, \
                                      &airo_gsensor_state.timer_handle);

            return;
        }

        //check multiple click
        if (airo_key_gsensor_config.multiple_click == 0) {
            LOG_MSGID_I(common, "[airo_key][gsensor] multiple_click can not be 0\r\n", 0);
            assert(0);
            return;
        }

        // click process
        airo_gsensor_state.key_count++;
        LOG_MSGID_I(common, "[airo_key][gsensor] short count=%d\r\n",1, airo_gsensor_state.key_count);
        if (airo_gsensor_state.key_count >= airo_key_gsensor_config.multiple_click) {

            if (airo_key_gsensor_config.t_silence != 0) {
                airo_gsensor_state.is_silence = true;
                hal_gpt_sw_start_timer_ms(airo_gsensor_state.timer_handle, \
                                          airo_key_gsensor_config.t_silence, \
                                          (hal_gpt_callback_t)gsensor_timeout, \
                                          &airo_gsensor_state.timer_handle);
            }

            airo_key_callbak(airo_gsensor_state.key_count, airo_gsensor_state.key_data);
            airo_gsensor_state.key_count = 0;

            return;
        }

        //timer start
        //log_hal_info("start interval time=%d",airo_key_gsensor_config.t_interval);
        hal_gpt_sw_start_timer_ms(airo_gsensor_state.timer_handle, \
                                  airo_key_gsensor_config.t_interval, \
                                  (hal_gpt_callback_t)gsensor_timeout, \
                                  &airo_gsensor_state.timer_handle);
    }

}

#endif
void airo_key_process_key(airo_key_mapping_event_t *key_event, airo_key_source_type_t type)
{
    uint8_t  index;
    uint8_t  key_name;
    uint8_t  config_index;
    key_action_type_t action_type = KEY_ACTION_PRESS;

#ifdef DEBUG_AIRO_PR_TIME_LOG
    uint32_t event_time[3];
#endif

#ifdef DEBUG_AIRO_PERFORMANCE_LOG
    uint32_t exe_time[2];
#endif


    if (airo_key_event_state.has_initilized == false) {
        LOG_MSGID_I(common, "[airo_key][process] sct key initilization has been not finished yet\r\n", 0);
        return;
    }

#ifdef DEBUG_AIRO_PERFORMANCE_LOG
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,  &exe_time[0]);
#endif

#ifdef MTK_GSENSOR_KEY_ENABLE
    if (type == AIRO_KEY_GSENSOR) {
        LOG_MSGID_I(common, "enter airo_key_process_key", 0);
        airo_key_gsensor_process(key_event);
        return;
    }
#endif

    // find key index in the position table
    index = airo_key_find_index(key_event, type);
    if (index == 0xFF) {
        return;
    }
    airo_key_event_state.position[index] = index;

    /*get key symbol name*/
    key_name = airo_key_get_key_name(index);

    config_index = airo_key_name_align_check(index, key_name);
    if (config_index == 0xFF) {
        return;
    }

    if (key_event->state == AIRO_KEY_DRIVEN_PRESS) {

        action_type = KEY_ACTION_PRESS;

#ifdef DEBUG_AIRO_PR_TIME_LOG
        event_time[0] = airo_key_event_state.time_stamp[index];
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &airo_key_event_state.time_stamp[index]);
        hal_gpt_get_duration_count(event_time[0], airo_key_event_state.time_stamp[index], &event_time[2]);

        LOG_MSGID_I(common, "[airo_key][process] --[release to press]<-- time=%d\r\n",1, event_time[2] / 1000);
#endif
    } else if (key_event->state == AIRO_KEY_DRIVEN_RELEASE) {

        action_type = KEY_ACTION_RELEASE;

#ifdef DEBUG_AIRO_PR_TIME_LOG
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &event_time[1]);
        hal_gpt_get_duration_count(airo_key_event_state.time_stamp[index], event_time[1], &event_time[2]);
        LOG_MSGID_I(common, "[airo_key][process] -->[press to release]<-- time=%d\r\n",1, event_time[2] / 1000);
#endif
    } else {
        LOG_MSGID_I(common, "[airo_key][process key state input error,key_event->state = %d\r\n",1, key_event->state);
        assert(0);
        return;
    }

    //stop timer first before airo_key_event_state.previous_action[index] get new action.
    //otherwise,if timer expired after previous_action updated, the state machine will lead to release(row:18) state but keep release next state(col:3).
    airo_key_stop_timer(index);
    airo_key_event_state.previous_action[index] = action_type;

#ifdef DEBUG_AIRO_BASIC_FLOW_LOG
    LOG_MSGID_I(common, "[airo_key][process] enter state machine, index=%d,conifg_index=%d, key_name=%d,action=%d\r\n",4, index, config_index, key_name, action_type);
#endif

    airo_key_state_machine(index, config_index, key_name, action_type);

#ifdef DEBUG_AIRO_BASIC_FLOW_LOG
    LOG_MSGID_I(common, "[airo_key][process] exit airo_key_process_key\r\n", 0);
#endif

#ifdef DEBUG_AIRO_PERFORMANCE_LOG
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,  &exe_time[1]);
    LOG_MSGID_I(common, "[airo_key][process] exe_time=%d us\r\n",1, exe_time[1] - exe_time[0]);
#endif
}

