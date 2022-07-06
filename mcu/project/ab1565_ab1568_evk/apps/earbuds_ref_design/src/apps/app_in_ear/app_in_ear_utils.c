/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

/**
 * File: app_in_ear_utils.c
 *
 * Description: this file provide common functions for in_ear_app.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#ifdef MTK_IN_EAR_FEATURE_ENABLE

#include "FreeRTOS.h"
#include "app_in_ear_idle_activity.h"
#include "app_in_ear_utils.h"
#include "apps_events_interaction_event.h"
#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "bt_sink_srv.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#endif
#include "apps_config_key_remapper.h"
#ifdef MTK_EINT_KEY_ENABLE
#include "bsp_eint_key.h"
#endif
#include "app_bt_state_service.h"
#ifdef MTK_FOTA_ENABLE
#include "app_fota_idle_activity.h"
#endif

#define APP_INEAR_UTILS "[In ear]utils"

/* Notify other apps that the wearing status has changed. */
static void app_in_ear_send_status(apps_in_ear_local_context_t *ctx)
{
    app_in_ear_sta_info_t *sta_info = (app_in_ear_sta_info_t *)pvPortMalloc(sizeof(sta_info));
    if (sta_info) {
        sta_info->previous = ctx->preState;
        sta_info->current = ctx->curState;
        /* Notify other apps that the wearing status has changed. */
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_IN_EAR_UPDATE_STA,
                (void *)sta_info, sizeof(app_in_ear_sta_info_t), NULL, 0);
        APPS_LOG_MSGID_I(APP_INEAR_UTILS" send interaction, currentSta=%d, previousSta=%d",
                2, ctx->curState, ctx->preState);
    }
}

void app_in_ear_update_status(apps_in_ear_local_context_t *ctx)
{
    app_in_ear_state_t currentStaTemp;
    bool ota_ongoing = FALSE;

#ifdef MTK_FOTA_ENABLE
     ota_ongoing = app_fota_get_ota_ongoing();
#endif
    const app_bt_state_service_status_t * curr_state = app_bt_connection_service_get_current_status();
    /* Check current state. */
#ifdef MTK_AWS_MCE_ENABLE
    if (ctx->isInEar) {
        /* Agent is in the ear. */
        if (ctx->isPartnerInEar && curr_state->aws_connected) {
            currentStaTemp = APP_IN_EAR_STA_BOTH_IN;
        } else {
            currentStaTemp = APP_IN_EAR_STA_AIN_POUT;
        }
    } else {
        /* Agent is not in the ear. */
        if (ctx->isPartnerInEar && curr_state->aws_connected) {
            currentStaTemp = APP_IN_EAR_STA_AOUT_PIN;
        } else {
            currentStaTemp = APP_IN_EAR_STA_BOTH_OUT;
        }
    }
#else
    if (ctx->isInEar) {
        currentStaTemp = APP_IN_EAR_STA_BOTH_IN;
    } else {
        currentStaTemp = APP_IN_EAR_STA_BOTH_OUT;
    }
#endif

    /* Need to update state. */
    if (ctx->curState != currentStaTemp) {
        ctx->preState = ctx->curState;
        ctx->curState = currentStaTemp;
        APPS_LOG_MSGID_I(APP_INEAR_UTILS" status changed, currentSta=%d, previousSta=%d", 2,
                         ctx->curState, ctx->preState);
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        if (ctx->isInRho) {
            ctx->eventDone = false;
        } else if (ota_ongoing) {
            /* Notify the new agent to handle this event. */
            ctx->eventDone = false;
            APPS_LOG_MSGID_I(APP_INEAR_UTILS" [FOTA ON DOING]", 0);
            app_in_ear_send_status(ctx);
        } else {
            ctx->eventDone = true;

            if (ctx->rhoEnable && ctx->curState == APP_IN_EAR_STA_AOUT_PIN) {
                if (curr_state->aws_connected) {
                    ctx->isInRho = true;
                    /* Notify the new agent to handle this event. */
                    ctx->eventDone = false;
                    APPS_LOG_MSGID_I(APP_INEAR_UTILS" trigger RHO.", 0);
                    /* Trigger RHO when agent is not in the ear but partner is in the ear. */
                    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                        APPS_EVENTS_INTERACTION_TRIGGER_RHO, NULL, 0,
                                        NULL, 0);
                } else {
                    /* Can not trigger RHO when aws has disconnected, So need notify current status to other apps.*/
                    app_in_ear_send_status(ctx);
                }
            } else {
                app_in_ear_send_status(ctx);
            }
        }
#else
        app_in_ear_send_status(ctx);
#endif
    }
}

#ifdef MTK_AWS_MCE_ENABLE
void app_in_ear_send_aws_data(apps_in_ear_local_context_t *ctx, app_in_ear_event_t event)
{
    app_in_ear_aws_data_t data;
    data.event = event;
    data.isInEar = ctx->isInEar;

    bt_aws_mce_report_info_t aws_data;
    aws_data.module_id = BT_AWS_MCE_REPORT_MODULE_IN_EAR;
    aws_data.is_sync = false;
    aws_data.sync_time = 0;
    aws_data.param_len = sizeof(app_in_ear_aws_data_t);
    aws_data.param = (void *)&data;
    bt_status_t bst = bt_aws_mce_report_send_event(&aws_data);
    if (bst == BT_STATUS_FAIL) {
        APPS_LOG_MSGID_I(APP_INEAR_UTILS" send aws data failed.", 0);
    }
    APPS_LOG_MSGID_I(APP_INEAR_UTILS" send aws data, event=%d, isInEar=%d, module_id=%d", 3, data.event, data.isInEar, aws_data.module_id);

}
#endif

#ifdef MTK_EINT_KEY_ENABLE
bool app_in_ear_get_wearing_status(void)
{
    bsp_eint_key_event_t status;
    bool isInEar = FALSE;
    if (bsp_eint_key_get_status(EINT_KEY_2, &status) == 0) {
        /* Check status parameter to get in ear status.
         * status=BSP_EINT_KEY_PRESS :in ear ;
         * status=BSP_EINT_KEY_RELEASE: out ear */
        if (status == BSP_EINT_KEY_PRESS) {
            isInEar = TRUE;
        } else {
            isInEar = FALSE;
        }
        APPS_LOG_MSGID_I(APP_INEAR_UTILS" get init wearing status:%d.", 1, isInEar);
    } else {
        /* Parameter error. */
        isInEar = FALSE;
        APPS_LOG_MSGID_I(APP_INEAR_UTILS" get init wearing status error.", 0);
    }

    return isInEar;
}
#endif

#ifdef IN_EAR_DEBUG
static char *sta0 = "BOTH_IN ";
static char *sta1 = "BOTH_OUT ";
static char *sta2 = "A_IN_P_OUT ";
static char *sta3 = "A_OUT_P_IN ";
void printStaToSysUART(apps_in_ear_local_context_t *ctx)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_OK};
    const char *p_sta = NULL;

    switch (ctx->curState) {
        case APP_IN_EAR_STA_BOTH_IN:
            p_sta = sta0;
            break;
        case APP_IN_EAR_STA_BOTH_OUT:
            p_sta = sta1;
            break;
        case APP_IN_EAR_STA_AIN_POUT:
            p_sta = sta2;
            break;
        case APP_IN_EAR_STA_AOUT_PIN:
            p_sta = sta3;
            break;
        default:
            p_sta = "IN_EAR_UNKNOWN";
    }

    memcpy(response.response_buf, p_sta, strlen(p_sta));
    response.response_len = strlen(p_sta);
    atci_send_response(&response);
}
/* IN_EAR_DEBUG */
#endif

/* MTK_IN_EAR_FEATURE_ENABLE */
#endif

