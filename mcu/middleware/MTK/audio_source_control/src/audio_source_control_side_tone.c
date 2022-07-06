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

#include "audio_source_control_side_tone.h"
#include "syslog.h"
#include "bt_sink_srv_ami.h"

log_create_module(AUDIO_SOURCE_CONTROL_SIDE_TONE, PRINT_LEVEL_DEBUG);

#define AUDIO_SOURCE_CONTROL_SIDE_TONE_CONTROL_TABLE_NUM        4

typedef struct {
    bool                            ctl_is_used;
    unsigned char                   ctl_handle_id;
    am_vendor_se_id_t               ctl_side_tone_id;
} __attribute__((packed)) audio_source_control_side_tone_control_t;

audio_source_control_side_tone_control_t side_tone_control_table[AUDIO_SOURCE_CONTROL_SIDE_TONE_CONTROL_TABLE_NUM];

audio_source_control_side_tone_control_t *audio_source_control_side_tone_get_empty_control()
{
    unsigned char index = 0;
    for (index = 0; index < AUDIO_SOURCE_CONTROL_SIDE_TONE_CONTROL_TABLE_NUM; index ++) {
        if (side_tone_control_table[index].ctl_is_used == false) {
            return &(side_tone_control_table[index]);
        }
    }
    return NULL;
}

void audio_source_control_side_tone_notify_result(audio_source_control_request_result_t result)
{
    unsigned char index = 0;
    bool to_remote = false;
    for (index = 0; index < AUDIO_SOURCE_CONTROL_SIDE_TONE_CONTROL_TABLE_NUM; index ++) {
        if ((side_tone_control_table[index].ctl_is_used == true) && (side_tone_control_table[index].ctl_handle_id != AUDIO_SOURCE_CONTROL_ID_VA_NONE)) {
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
            to_remote = true;
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE */
            audio_source_control_send_result(side_tone_control_table[index].ctl_handle_id, to_remote, result);
        }
    }
}

void audio_source_control_side_tone_vendor_se_callback(vendor_se_event_t event, void *arg)
{
    if (event == EVENT_SIDETONE_STOP) {
        audio_source_control_side_tone_notify_result(AUDIO_SOURCE_CONTROL_REQUEST_RESULT_RELEASE_SUCCEED);
    }

    if (event == EVENT_SIDETONE_START) {
        audio_source_control_side_tone_notify_result(AUDIO_SOURCE_CONTROL_REQUEST_RESULT_REQUEST_SUCCEED);
    }
}

void audio_source_control_side_tone_init(unsigned char handle_id)
{
    audio_source_control_side_tone_control_t *ctl = audio_source_control_side_tone_get_empty_control();
    if (ctl == NULL) {
        LOG_MSGID_E(AUDIO_SOURCE_CONTROL_SIDE_TONE, "audio_source_control_side_tone_init, All control has been used, no empty for ID : 0x%02x", 1, handle_id);
        return;
    }

    ctl->ctl_is_used = true;
    ctl->ctl_handle_id = handle_id;
    ctl->ctl_side_tone_id = ami_get_vendor_se_id();

    if (ctl->ctl_side_tone_id == -1) {
        LOG_MSGID_E(AUDIO_SOURCE_CONTROL_SIDE_TONE, "audio_source_control_side_tone_init, Get side tone failed with ID : 0x%02x", 1, handle_id);
        ctl->ctl_is_used = false;
        ctl->ctl_handle_id = 0;
        return;
    }

    bt_sink_srv_am_result_t am_se_result = ami_register_vendor_se(ctl->ctl_side_tone_id, audio_source_control_side_tone_vendor_se_callback);
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL_SIDE_TONE, "audio_source_control_side_tone_init, register vendor se (ID : 0x%02x) callback result : 0x%02x",
                    2, ctl->ctl_side_tone_id, am_se_result);
}

bool audio_source_control_side_tone_need_take_resource_first(unsigned char handle_id)
{
    return false;
}

void audio_source_control_side_tone_handler(unsigned char handle_id, audio_source_control_request_result_t result)
{
    LOG_MSGID_I(AUDIO_SOURCE_CONTROL_SIDE_TONE, "audio_source_control_side_tone_handler, request handle ID : 0x%02x, result : %d", 2, handle_id, result);

    bt_sink_srv_am_result_t ami_result;

    bool to_remote = false;

    if (result == AUDIO_SOURCE_CONTROL_REQUEST_RESULT_REQUEST_SUCCEED) {
        ami_result = am_audio_side_tone_enable();
        if (ami_result != AUD_EXECUTION_SUCCESS) {
            LOG_MSGID_I(AUDIO_SOURCE_CONTROL_SIDE_TONE, "audio_source_control_side_tone_handler, Enable side tone failed", 0);
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
            to_remote = true;
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE */
            audio_source_control_send_result(handle_id, to_remote, AUDIO_SOURCE_CONTROL_REQUEST_RESULT_REQUEST_FAILED);
        }
    } else if (result == AUDIO_SOURCE_CONTROL_REQUEST_RESULT_RELEASE_SUCCEED) {
        ami_result = am_audio_side_tone_disable();
        if (ami_result != AUD_EXECUTION_SUCCESS) {
            LOG_MSGID_I(AUDIO_SOURCE_CONTROL_SIDE_TONE, "audio_source_control_side_tone_handler, Disable side tone failed", 0);
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
            to_remote = true;
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE */
            audio_source_control_send_result(handle_id, to_remote, AUDIO_SOURCE_CONTROL_REQUEST_RESULT_RELEASE_FAILED);
        }
    }
}

audio_source_control_checker_result_t audio_source_control_side_tone_resource_checker(unsigned char handle_id)
{
#if defined (AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined (AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)

#if defined (AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
    /* Dual Chip Master Role */
    if (handle_id == AUDIO_SOURCE_CONTROL_ID_SIDE_TONE) {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL_SIDE_TONE, "audio_source_control_side_tone_resource_checker, Master role and MIC exist, return 1", 0);
        return AUDIO_SOURCE_CONTROL_CHECKER_RESULT_EXIST;
    }
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE */

#if defined (AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
    /* Dual Chip Slave Role */
    if (handle_id == AUDIO_SOURCE_CONTROL_ID_SIDE_TONE) {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL_SIDE_TONE, "audio_source_control_side_tone_resource_checker, Slave role and MIC DO not exist, return 0", 0);
        return AUDIO_SOURCE_CONTROL_CHECKER_RESULT_NON_EXIST;
    }
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */

#else
    /* Single Chip Project */
    if (handle_id == AUDIO_SOURCE_CONTROL_ID_SIDE_TONE) {
        LOG_MSGID_I(AUDIO_SOURCE_CONTROL_SIDE_TONE, "audio_source_control_side_tone_resource_checker, Single chip mode, return 1", 0);
        return AUDIO_SOURCE_CONTROL_CHECKER_RESULT_EXIST;
    }
#endif

    LOG_MSGID_I(AUDIO_SOURCE_CONTROL_SIDE_TONE, "audio_source_control_side_tone_resource_checker, Unknown state, return -1", 0);
    return AUDIO_SOURCE_CONTROL_CHECKER_RESULT_INVALID;
}




