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

#ifdef AIR_RACE_CO_SYS_ENABLE

#include "apps_race_cmd_co_sys_event.h"

#include "ui_shell_manager.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_debug.h"
#include "FreeRTOS.h"
#include "verno.h"

#define LOG_TAG     "[APP_RACE_DUAL_EVENT]"

void app_race_cmd_co_sys_send_event(uint32_t event_group, uint32_t event_id, void *extra_data, uint32_t extra_data_len, bool is_critical)
{
    uint32_t total_len = sizeof(apps_race_cmd_co_sys_common_cmd_buf_t) + extra_data_len;
    apps_race_cmd_co_sys_common_cmd_buf_t *send_buf = (apps_race_cmd_co_sys_common_cmd_buf_t *)pvPortMalloc(total_len);
    if (send_buf) {
        send_buf->group_id = event_group;
        send_buf->event_id = event_id;
        send_buf->data_len = extra_data ? extra_data_len : 0;
        if (extra_data_len > 0 && extra_data) {
            memcpy(send_buf->data, extra_data, extra_data_len);
        }
        race_cosys_send_data(RACE_COSYS_MODULE_ID_APP_COMMON, is_critical, (uint8_t *)send_buf, total_len);
        vPortFree(send_buf);
    }
}

static void app_race_cmd_co_sys_data_received_callback(bool is_critical, uint8_t *buff, uint32_t len)
{
    apps_race_cmd_co_sys_common_cmd_buf_t *cmd_content = (apps_race_cmd_co_sys_common_cmd_buf_t *)buff;
    void *extra_data = NULL;
    bool process_done = false;  /* Whe it's false, means send ui shell event to other app to process. */
    if (!cmd_content || cmd_content->data_len + sizeof(apps_race_cmd_co_sys_common_cmd_buf_t) != len) {
        APPS_LOG_MSGID_E(LOG_TAG", receive co sys data not correct: buff:0x%x", 1, buff);
        return;
    }

    switch (cmd_content->group_id) {
        case EVENT_GROUP_UI_SHELL_DUAL_CHIP_CMD: {
            switch (cmd_content->event_id) {
                case APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_GET_TIMESTAMP: {
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
                    app_race_cmd_co_sys_send_event(EVENT_GROUP_UI_SHELL_DUAL_CHIP_CMD, APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_GET_TIMESTAMP,
                            build_date_time_str, strlen(build_date_time_str) + 1, __bool_true_false_are_defined);
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
                    if (cmd_content->data_len > 0) {
                        APPS_LOG_I(LOG_TAG", timestamp. master:%s, slave:%s", build_date_time_str, cmd_content->data);
                    }
#endif
                    process_done = true;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    /* Send ui event. */
    if (!process_done) {
        APPS_LOG_MSGID_I(LOG_TAG", receive co_sys APP data from remote: group:0x%x, event_id: 0x%x, extra_data_len: %d", 3, cmd_content->group_id, cmd_content->event_id, cmd_content->data_len);
        if (cmd_content->data_len > 0) {
            /* Some event use value as a pointer directly. */
            bool directly_use_data = false;

            switch (cmd_content->group_id) {
                case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
                    switch (cmd_content->event_id) {
                        case APPS_EVENTS_INTERACTION_REQUEST_POWER_OFF:
                        case APPS_EVENTS_INTERACTION_REQUEST_REBOOT:
                        case APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT:
                        case APPS_EVENTS_INTERACTION_REQUEST_CLASSIC_BT_OFF:
                            directly_use_data = true;
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
            if (directly_use_data) {
                uint32_t data_value = 0;
                memcpy(&data_value, cmd_content->data, cmd_content->data_len);
                cmd_content->data_len = 0;
                extra_data = (void *)data_value;
            } else {
                extra_data = pvPortMalloc(cmd_content->data_len);
                if (extra_data) {
                    memcpy(extra_data, cmd_content->data, cmd_content->data_len);
                } else {
                    return;
                }
            }
        }
        ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, cmd_content->group_id, cmd_content->event_id, extra_data, cmd_content->data_len, NULL, 0);
    }
}

void apps_race_cmd_co_sys_event_init(void)
{
    race_cosys_register_data_callback(RACE_COSYS_MODULE_ID_APP_COMMON, app_race_cmd_co_sys_data_received_callback);
}

#endif /* #ifdef MTK_RACE_CMD_ENABLE */
