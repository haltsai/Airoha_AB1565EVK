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

#include "audio_source_control.h"
#include "audio_source_control_va.h"
#include "syslog.h"
#include "assert.h"

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) && defined(MTK_AUDIO_TRANSMITTER_ENABLE)
#include "audio_transmitter_control.h"
#endif /* defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) && defined(MTK_AUDIO_TRANSMITTER_ENABLE) */

#define AUDIO_SOURCE_CONTROL_VA_INVALID_TRANSMITTER_ID     (-1)

#define AUDIO_SOURCE_CONTROL_VA_COUNT                       2

log_create_module(AUDIO_SOURCE_CONTROL_VA, PRINT_LEVEL_DEBUG);

typedef struct {
    unsigned char               handle_id;
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) && defined(MTK_AUDIO_TRANSMITTER_ENABLE)
    audio_transmitter_id_t      transmitter_id;
#endif /* defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) && defined(MTK_AUDIO_TRANSMITTER_ENABLE) */
} audio_source_control_va_t;

audio_source_control_va_t g_audio_source_control_va_table[AUDIO_SOURCE_CONTROL_VA_COUNT] = {
    {
        0x00,
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) && defined(MTK_AUDIO_TRANSMITTER_ENABLE)
        AUDIO_SOURCE_CONTROL_VA_INVALID_TRANSMITTER_ID,
#endif
    },
    {
        0x00,
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) && defined(MTK_AUDIO_TRANSMITTER_ENABLE)
        AUDIO_SOURCE_CONTROL_VA_INVALID_TRANSMITTER_ID,
#endif
    }
};

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) && defined(MTK_AUDIO_TRANSMITTER_ENABLE)
static bool audio_source_control_va_check_control_exist(audio_source_control_va_t *control)
{
    unsigned char index = 0;
    for (index = 0; index < AUDIO_SOURCE_CONTROL_VA_COUNT; index ++) {
        if ((g_audio_source_control_va_table[index].handle_id != 0x00)
                && ((&(g_audio_source_control_va_table[index])) == control)) {
            return true;
        }
    }
    return false;
}

static void audio_source_control_va_transmitter_msg_handler(audio_transmitter_event_t event, void *data, void *user_data)
{
    audio_source_control_va_t *control = (audio_source_control_va_t *)user_data;

    if (control == NULL) {
        LOG_MSGID_E(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_transmitter_msg_handler, control is NULL", 0);
        return;
    }

    if (audio_source_control_va_check_control_exist(control) == false) {
        LOG_MSGID_E(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_transmitter_msg_handler, the control not exist with ID : %d", 1, control->handle_id);
        return;
    }

    LOG_MSGID_I(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_transmitter_msg_handler, transmitter callback event : %d, handle ID : %d", 2, event, control->handle_id);

    audio_source_control_request_result_t result = AUDIO_SOURCE_CONTROL_REQUEST_RESULT_NONE;
    switch (event) {
        case AUDIO_TRANSMITTER_EVENT_START_SUCCESS: {
            result = AUDIO_SOURCE_CONTROL_REQUEST_RESULT_REQUEST_SUCCEED;
        }
        break;
        case AUDIO_TRANSMITTER_EVENT_START_FAIL:
        case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_RECORDER:
        case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_A2DP:
        case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_HFP: {
            result = AUDIO_SOURCE_CONTROL_REQUEST_RESULT_REQUEST_FAILED;
        }
        break;
        case AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS: {
            result = AUDIO_SOURCE_CONTROL_REQUEST_RESULT_RELEASE_SUCCEED;
        }
        break;
        default:
        return;
    }

    audio_source_control_send_result(control->handle_id, true, result);
}

static bool audio_source_control_va_init_transmitter(audio_source_control_va_t *control)
{
    audio_transmitter_config_t config;

    config.scenario_type = AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK;
    config.scenario_sub_id = AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_ADC_TO_I2S0;
    config.scenario_config.reserved = 0;
    config.msg_handler = audio_source_control_va_transmitter_msg_handler;
    config.user_data = (void *)control;

    control->transmitter_id = audio_transmitter_init(&config);

    if (control->transmitter_id == AUDIO_SOURCE_CONTROL_VA_INVALID_TRANSMITTER_ID) {
        LOG_MSGID_E(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_init_transmitter, failed to init transmitter", 0);
        return false;
    }

    LOG_MSGID_I(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_init_transmitter, succeed to init transmitter with ID : %d for handle ID : %d",
                    2, control->transmitter_id, control->handle_id);

    return true;
}
#endif /* defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) && defined(MTK_AUDIO_TRANSMITTER_ENABLE) */

static audio_source_control_va_t *audio_source_control_va_get_control_by_id(unsigned char id)
{
    unsigned char index = 0;
    for (index = 0; index < AUDIO_SOURCE_CONTROL_VA_COUNT; index ++) {
        if (g_audio_source_control_va_table[index].handle_id == id) {
            return &(g_audio_source_control_va_table[index]);
        }
    }
    return NULL;
}

static audio_source_control_va_t *audio_source_control_va_get_empty_control()
{
    unsigned char index = 0;
    for (index = 0; index < AUDIO_SOURCE_CONTROL_VA_COUNT; index ++) {
        if (g_audio_source_control_va_table[index].handle_id == 0x00) {
            return &(g_audio_source_control_va_table[index]);
        }
    }
    return NULL;
}

void audio_source_control_va_init(unsigned char handle_id)
{

    audio_source_control_va_t *control = NULL;

    control = audio_source_control_va_get_control_by_id(handle_id);
    if (control == NULL) {
        control = audio_source_control_va_get_empty_control();
        if (control == NULL) {
            LOG_MSGID_I(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_init, all VA control has been used, cannot init control with ID : %d", 1, handle_id);
            return;
        }
    } else {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_init, already inited with handle ID : %d", 1, handle_id);
        return;
    }

    control->handle_id = handle_id;

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) && defined(MTK_AUDIO_TRANSMITTER_ENABLE)
    audio_source_control_va_init_transmitter(control);
#endif
}

bool audio_source_control_va_need_take_resource_first(unsigned char handle_id)
{
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_need_take_resource_first, VA always need take resource firstly : 0x%02x", 1, handle_id);
    return true;
}

void audio_source_control_va_handler(unsigned char handle_id, audio_source_control_request_result_t request_result)
{
    audio_source_control_va_t *control = audio_source_control_va_get_control_by_id(handle_id);
    if (control == NULL) {
        LOG_MSGID_E(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_handler, control is NULL (failed to find with ID : 0x%02x)", 1, handle_id);
        return;
    }

    LOG_MSGID_E(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_handler, execute handle ID : 0x%02x, request_result : 0x%02x", 2, handle_id, request_result);

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
    if (request_result == AUDIO_SOURCE_CONTROL_REQUEST_RESULT_REQUEST_SUCCEED) {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_handler, start transmitter with ID : 0x%02x", 1, control->transmitter_id);
        audio_transmitter_status_t status = audio_transmitter_start(control->transmitter_id);
        if (status != AUDIO_TRANSMITTER_STATUS_SUCCESS) {
            LOG_MSGID_E(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_handler, start transmitter failed", 0);
            audio_source_control_send_result(handle_id, true, AUDIO_SOURCE_CONTROL_REQUEST_RESULT_REQUEST_FAILED);
        }
    } else if (request_result == AUDIO_SOURCE_CONTROL_REQUEST_RESULT_RELEASE_SUCCEED) {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_handler, stop transmitter with ID : 0x%02x", 1, control->transmitter_id);
        audio_transmitter_status_t status = audio_transmitter_stop(control->transmitter_id);
        if (status != AUDIO_TRANSMITTER_STATUS_SUCCESS) {
            LOG_MSGID_E(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_handler, stop transmitter failed", 0);
            audio_source_control_send_result(handle_id, true, AUDIO_SOURCE_CONTROL_REQUEST_RESULT_RELEASE_FAILED);
        }
    } else {
        audio_source_control_send_result(handle_id, true, request_result);
    }
#else
    LOG_MSGID_E(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_handler, defined AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE, but not define MTK_AUDIO_TRANSMITTER_ENABLE", 0);
    assert(false);
#endif /* MTK_AUDIO_TRANSMITTER_ENABLE */
#else

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_handler, Slave mode, do nothing", 0);
    assert(false);
#else
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_handler, Single chip send result directly", 0);
    audio_source_control_send_result(handle_id, false, request_result);
#endif
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE */
}

audio_source_control_checker_result_t audio_source_control_va_resource_checker(unsigned char handle_id)
{
#if defined (AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined (AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)

#if defined (AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
    /* Dual Chip Master Role */
    if ((handle_id == AUDIO_SOURCE_CONTROL_ID_VA_AMA)
            || (handle_id == AUDIO_SOURCE_CONTROL_ID_VA_GSOUND)) {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_resource_checker, Master role and MIC exist, return 1", 0);
        return AUDIO_SOURCE_CONTROL_CHECKER_RESULT_EXIST;
    }
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE */

#if defined (AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
    /* Dual Chip Slave Role */
    if ((handle_id == AUDIO_SOURCE_CONTROL_ID_VA_AMA)
            || (handle_id == AUDIO_SOURCE_CONTROL_ID_VA_GSOUND)) {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_resource_checker, Slave role and MIC DO not exist, return 0", 0);
        return AUDIO_SOURCE_CONTROL_CHECKER_RESULT_NON_EXIST;
    }
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */

#else
    /* Single Chip Project */
    if ((handle_id == AUDIO_SOURCE_CONTROL_ID_VA_AMA)
            || (handle_id == AUDIO_SOURCE_CONTROL_ID_VA_GSOUND)) {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_resource_checker, Single chip mode, return 1", 0);
        return AUDIO_SOURCE_CONTROL_CHECKER_RESULT_EXIST;
    }
#endif

    LOG_MSGID_I(AUDIO_SOURCE_CONTROL_VA, "audio_source_control_va_resource_checker, Unknown state, return -1", 0);
    return AUDIO_SOURCE_CONTROL_CHECKER_RESULT_INVALID;
}


