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
 * File: app_in_ear_utils.h
 *
 * Description: This file defines the common structure and functions of in ear app.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#ifndef __APP_IN_EAR_UTILS_H__
#define __APP_IN_EAR_UTILS_H__

#ifdef MTK_IN_EAR_FEATURE_ENABLE

#include "bt_sink_srv.h"
#include "ui_shell_manager.h"
#include "ui_shell_activity.h"
#include "apps_config_key_remapper.h"
#include "atci.h"

#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif

/**
 *  @brief This enumeration defines the in ear detection state.
 */
typedef enum {
    APP_IN_EAR_STA_BOTH_IN,         /**<  The agent and partner are in the ear. */
    APP_IN_EAR_STA_BOTH_OUT,        /**<  The agent and partner are not in the ear. */
    APP_IN_EAR_STA_AIN_POUT,        /**<  Only agent is in the ear. */
    APP_IN_EAR_STA_AOUT_PIN,        /**<  Only partner is in the ear. */
} app_in_ear_state_t;

/**
 *  @brief This enumeration defines the OHD(On-Head Detection) state.
 */
typedef enum {
    APP_IN_EAR_OHD_DISABLED,        /**<  The in-ear detection is not enabled. */
    APP_IN_EAR_OHD_NONE_DETECTED,   /**<  The device is not on the head. */

    APP_IN_EAR_OHD_LEFT_DETECTED,   /**<  Only left side on the head. */
    APP_IN_EAR_OHD_RIGHT_DETECTED,  /**<  Only right side on the head. */
    APP_IN_EAR_OHD_BOTH_DETECTED,   /**<  Both left and right on the head. */

    APP_IN_EAR_OHD_DETECTED,        /**<  The headset is on the head. */
} app_in_ear_ohd_state_t;

/**
 *  @brief This structure defines the state information.
 */
typedef struct {
    app_in_ear_state_t previous;      /**<  The previous state of earbuds. */
    app_in_ear_state_t current;       /**<  The current state of earbuds. */
} app_in_ear_sta_info_t;

/**
 *  @brief This structure defines the in ear app's context
 */
typedef struct {
    bool isInEar;                   /**<  Record whether the earbud is in the ear. */
#ifdef MTK_AWS_MCE_ENABLE
    bool isPartnerInEar;            /**<  Record whether the partner is in the ear. */
    bool rhoEnable;                 /**<  Indicates whether to trigger RHO when the in ear state changes. */
    bool isInRho;                   /**<  Indicates whether RHO has started. */
#endif
    bool eventDone;                 /**<  Record whether the actions related to in ear detection have been completed. */
    bool eventOutEnable;            /**<  Indicates whether to notify other apps of the in ear state. */
    app_in_ear_state_t preState;    /**<  The previous state of earbuds. */
    app_in_ear_state_t curState;    /**<  The current state of earbuds. */
} PACKED apps_in_ear_local_context_t;

/**
 *  @brief This enumeration defines the event between agent and partner.
 */
typedef enum {
    APP_IN_EAR_EVENT_UPDATE_STA,    /**<  The event that notifies the other side to update the status. */
    APP_IN_EAR_EVENT_ANC_ENABLE,    /**<  The event that notifies the other side to enable ANC. */
    APP_IN_EAR_EVENT_ANC_DISABLE    /**<  The event that notifies the other side to disable ANC. */
} app_in_ear_event_t;


/**
 *  @brief This structure defines the AWS data packet between agent and partner.
 */
typedef struct {
    app_in_ear_event_t event;       /**<  The event sent to the other side. */
    bool isInEar;                   /**<  The earbud's state of in ear. */
} app_in_ear_aws_data_t;


/**
* @brief      This function is used to handle the events when in ear state changes.
* @param[in]  ctx, the context pointer of the activity.
* @return     None.
*/
void app_in_ear_update_status(apps_in_ear_local_context_t *ctx);


/**
* @brief      This function is used to send event to other side.
* @param[in]  ctx, the context pointer of the activity.
* @param[in]  event, the event to be sent to the other side.
* @return     None.
*/
void app_in_ear_send_aws_data(apps_in_ear_local_context_t *ctx, app_in_ear_event_t event);

/**
* @brief      This function is used to get own in_ear state.
* @return     bool in_ear.
*/
bool app_in_ear_get_own_state();

/**
* @brief      This function is used to return in_ear current state.
* @return     app_in_ear_state_t.
*/
app_in_ear_state_t app_in_ear_get_state();

/**
* @brief      This function is used to get the on-head detection state.
* @return     app_in_ear_state_t.
*/
app_in_ear_ohd_state_t app_in_ear_get_ohd_state();

/**
* @brief      This function is used to get wearing state.
* @return     Return true is on wearing.
*/
bool app_in_ear_get_wearing_status(void);

#ifdef IN_EAR_DEBUG
/**
* @brief      This function is used to print debugging information through system serial port.
* @param[in]  ctx, the context pointer of the activity.
* @return     None.
*/
void printStaToSysUART(apps_in_ear_local_context_t *ctx);
#endif

#endif /* MTK_IN_EAR_FEATURE_ENABLE */

#endif /* __APP_IN_EAR_UTILS_H__ */

