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

#ifndef __AIRO_KEY_EVENT_INTERNAL_H__
#define __AIRO_KEY_EVENT_INTERNAL_H__

#ifdef MTK_KEYPAD_ENABLE
#include "airo_keypad.h"
#include "keypad_custom.h"
#endif

#ifdef MTK_EINT_KEY_ENABLE
#include "bsp_eint_key_custom.h"
#include "airo_eint_key.h"
#endif

#ifdef HAL_CAPTOUCH_MODULE_ENABLED
#include "airo_captouch.h"
#endif

#ifdef MTK_GSENSOR_KEY_ENABLE
#include "airo_key_gsensor.h"
#endif

#include "airo_key_config.h"
#include "airo_key_event.h"

#ifdef AIRO_KEY_FEATRURE_POWERKEY
#include "airo_powerkey.h"
#endif

#ifdef AIR_PSENSOR_KEY_ENABLE
#include "airo_key_psensor.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// column
typedef enum {
    KEY_ACTION_PRESS,
    KEY_ACTION_KEEP_PRESS,
    KEY_ACTION_RELEASE,
    KEY_ACTION_KEEP_RELEASE,
    KEY_ACTION_CLICK_NUMBER_YES,
    KEY_ACTION_CLICK_NUMBER_NO,
    KEY_ACTION_CONTINUE_NO_ACTION,
    KEY_ACTION_MAX,
} key_action_type_t;

//row
typedef enum {
    key_state_idle,             /*0*/
    key_state_press_1,          /*1*/
    key_state_release_1_temp,   /*2*/
    key_state_short_click_1,    /*3*/
    key_state_short_click_2,    /*4*/
    key_state_release_1,        /*5*/
    key_state_press_2,          /*6*/
    key_state_slong,            /*7*/
    key_state_slong_repeat,     /*8*/
    key_state_release_2_temp,   /*9*/
    key_state_double_click_1,   /*10*/
    key_state_double_click_2,   /*11*/
    key_state_release_2,        /*12*/
    key_state_press_3,          /*13*/
    key_state_dlong,            /*14*/
    key_state_dlong_repeat,     /*15*/
    key_state_invalid,          /*16*/
    key_state_triple_click_1,   /*17*/
    key_state_triple_click_2,   /*18*/
    key_state_release_3_temp,   /*19*/
    key_state_release,          /*20*/
    key_state_release_3,        /*21*/
    key_state_long_1_temp,      /*22*/
    key_state_long_1,           /*23*/
    key_state_long_2_temp,      /*24*/
    key_state_long_2,           /*25*/
    key_state_long_3_temp,      /*26*/
    key_state_long_3,           /*27*/
    key_state_long_repeat,      /*28*/
    key_state_long_repeat_temp, /*29*/
    key_state_dummy_click,      /*30*/
    key_state_dummy_press,      /*31*/
    key_state_dummy_release,    /*32*/
    key_state_max               /*33*/
} key_state_type_t;


typedef enum {
    SUPPORT_PRESS_RELEASE   = 0,
    SUPPORT_SLONG           = 1,
    SUPPORT_SLONG_REPEAT    = 2,
    SUPPORT_LONG_LEVEL      = 3,
    SUPPORT_LONG_REPEAT     = 4,
    SUPPORT_MULTIPLE_CLICK  = 5
} airo_support_event_type_t;

typedef enum {
    TIME_PRESS      = 0,
    TIME_RELEASE    = 1,
    TIME_SILENCE    = 2,
    TIME_REPEAT     = 3,
    TIME_LONG1      = 4,
    TIME_LONG2      = 5,
    TIME_LONG3      = 6,
    TIME_SLONG      = 7
} airo_support_time_type_t;



typedef enum {
    AIRO_KEY_DRIVEN_RELEASE        = 0,
    AIRO_KEY_DRIVEN_PRESS          = 1,
    AIRO_KEY_DRIVEN_LONG_PRESS     = 2,
    AIRO_KEY_DRIVEN_REPEAT         = 3,
    AIRO_KEY_DRIVEN_PMU_LONG_PRESS = 4,
    AIRO_KEY_DRIVEN_SINGLE_TAP     = 5,
    AIRO_KEY_DRIVEN_DOUBLE_TAP     = 6,
} airo_key_driven_t;

typedef struct {
    key_state_type_t  airo_state[AIRO_KEY_STATE_SIZE];
    airo_key_driven_t  key_state[AIRO_KEY_STATE_SIZE];
    uint32_t          time_stamp[AIRO_KEY_STATE_SIZE];
    uint32_t          timer_handle[AIRO_KEY_STATE_SIZE];
    bool              is_timer_started[AIRO_KEY_STATE_SIZE];
    uint32_t          press_time;
    uint8_t           position[AIRO_KEY_STATE_SIZE];
    uint8_t           key_data[AIRO_KEY_STATE_SIZE];
    bool              has_initilized;
    key_action_type_t previous_action[AIRO_KEY_STATE_SIZE];
} airo_key_event_state_t;

typedef struct {
    uint8_t     key_count;
    uint8_t     key_data;
    uint32_t    timer_handle;
    bool        is_timer_start;
    bool        is_silence;
} airo_key_gsensor_state_t;

typedef enum {
    AIRO_GET_KEY_DATA_KEYPAD     = 1,
    AIRO_GET_KEY_DATA_POWERKEY   = 2,
    AIRO_GET_KEY_DATA_EINT       = 3,
    AIRO_CALL_CALLBACK           = 4,
    AIRO_REPEAT_CALLBACK         = 5,
    AIRO_GET_KEY_DATA_CAPTOUCH   = 6
} airo_key_msg_type_t;


typedef enum {
    AIRO_KEY_KEYPAD              = 0,
    AIRO_KEY_EINT_KEY            = 1,
    AIRO_KEY_CAPTOUCH            = 2,
    AIRO_KEY_POWERKEY            = 3,
    AIRO_KEY_GSENSOR             = 4,
    AIRO_KEY_PSENSOR             = 5,
	AIRO_KEY_INEAR_TOUCH		 = 6
} airo_key_source_type_t;


typedef struct {
    char *src_mod;
    airo_key_msg_type_t msg_id;
    uint8_t key_index;
} airo_key_msg_t;


typedef struct {
    airo_key_callback_t callback;
    void *user_data;
} airo_key_event_context_t;



typedef struct {
    airo_key_driven_t   state;
    uint32_t   key_data;
    uint32_t   time_stamp;
} airo_key_mapping_event_t;


/********* varible extern *************/
extern airo_key_event_context_t airo_key_event_context;
extern airo_key_event_state_t   airo_key_event_state;
extern airo_key_event_time_t    airo_key_event_time;
extern airo_key_event_config_t airo_key_config[AIRO_KEY_SUPPORT_NUMBER];
#ifdef MTK_GSENSOR_KEY_ENABLE
extern airo_key_gsensor_state_t airo_gsensor_state;
#endif

/******** funtion extern **************/
void airo_key_multiple_press_process(uint8_t *key_type);
void airo_key_process_key(airo_key_mapping_event_t *key_event, airo_key_source_type_t type);
void airo_key_state_machine(uint8_t index, uint8_t config_index, uint8_t key_name, key_action_type_t action);

#ifdef __cplusplus
}
#endif

#endif /* __AIRO_KEY_EVENT_INTERNAL_H__ */

