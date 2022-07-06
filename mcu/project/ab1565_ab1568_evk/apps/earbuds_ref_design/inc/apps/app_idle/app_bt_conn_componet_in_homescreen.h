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

/**
 * File: app_bt_conn_componet_in_homescreen.h
 *
 * Description: This file defines the interface of app_bt_conn_componet_in_homescreen.c.
 *
 * Note: See doc/Airoha_IoT_SDK_Application_Developers_Guide.pdf for Homescreen APP.
 *
 */

#ifndef __APP_BT_CONN_COMPONENT_IN_HOMESCREEN_H__
#define __APP_BT_CONN_COMPONENT_IN_HOMESCREEN_H__

#include "ui_shell_activity.h"
#include "apps_config_state_list.h"
#include "bt_sink_srv.h"

#ifdef AIR_MULTI_POINT_ENABLE
#define APP_CONN_MAX_DEVICE_NUM  3
#else
#define APP_CONN_MAX_DEVICE_NUM  2
#endif

/**
 *  @brief This enum defines the states of Homescreen APP.
 */
typedef enum {
    APP_HOME_SCREEN_STATE_IDLE,
    APP_HOME_SCREEN_STATE_POWERING_OFF,
    APP_HOME_SCREEN_STATE_REBOOT
} app_home_screen_state_t;

/**
 *  @brief This enum defines the BT power state of Homescreen APP.
 */
typedef enum {
    APP_HOME_SCREEN_BT_POWER_STATE_DISABLED,
    APP_HOME_SCREEN_BT_POWER_STATE_ENABLED,
    APP_HOME_SCREEN_BT_POWER_STATE_DISABLING,
    APP_HOME_SCREEN_BT_POWER_STATE_ENABLING
} app_home_screen_bt_power_state_t;

typedef struct {
    bt_bd_addr_t addr;
    bt_cm_profile_service_mask_t conn_service;
} app_conn_device_info_t;

#ifdef AIR_MULTI_POINT_ENABLE
#define APP_BT_CONN_WAIT_DISCONN_VP_PLAYED   0x01
#define APP_BT_CONN_WAIT_DISCONN_NO_VP       0x02
#endif

/**
 *  @brief This structure defines the context of Homescreen APP.
 */
typedef struct {
    app_home_screen_state_t state;                              /**<  The state of Homescreen APP. */
    app_home_screen_bt_power_state_t bt_power_state;            /**<  The current BT power state of Homescreen APP. */
    app_home_screen_bt_power_state_t target_bt_power_state;     /**<  The target BT power state of Homescreen APP. */
    bool connection_state;                                      /**<  Record whether BT EDR connected. */
    bool power_off_waiting_time_out;                            /**<  Record whether need wait timeout then power off after start to turn of BT. */
#ifdef APPS_TRIGGER_RHO_BY_KEY
    bool key_trigger_waiting_rho;                               /**<  Record whether trigger RHO via key. */
#endif
    bool aws_connected;                                         /**<  Record whether BT AWS connected. */
    bool is_bt_visiable;                                        /**<  Record whether BT visibility. */
    bool bt_power_off;                                          /**<  BT power off set by other user. */
    uint8_t power_off_waiting_release_key;                      /**<  Current power off key ID. */
    app_conn_device_info_t conn_device[APP_CONN_MAX_DEVICE_NUM];
    uint8_t conn_device_num;
#ifdef AIR_MULTI_POINT_ENABLE
    bool partner_aws_switch;
    bool agent_synced_disconn;
    uint8_t wait_disconnect;
    bool partner_lost;
#endif
} home_screen_local_context_type_t;


/**
* @brief      This function is used to handle APP interaction events and update Homescreen APP context.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool bt_conn_component_app_interaction_event_proc(ui_shell_activity_t *self,
                                                  uint32_t event_id,
                                                  void *extra_data,
                                                  size_t data_len);


/**
* @brief      This function is used to handle BT CM event and update Homescreen APP context.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool bt_conn_component_bt_cm_event_proc(ui_shell_activity_t *self,
                                        uint32_t event_id,
                                        void *extra_data,
                                        size_t data_len);

/**
* @brief      This function is used to handle BT sink event and update Homescreen APP context.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool bt_conn_component_bt_sink_event_proc(ui_shell_activity_t *self,
                                          uint32_t event_id,
                                          void *extra_data,
                                          size_t data_len);

/**
* @brief      This function is used to handle AWS Data event and update Homescreen APP context.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool bt_conn_component_aws_data_proc(ui_shell_activity_t *self,
                                     uint32_t event_id,
                                     void *extra_data,
                                     size_t data_len);

void app_bt_conn_sync_addr_list(home_screen_local_context_type_t *local_ctx);

#endif /* __APP_BT_CONN_COMPONENT_IN_HOMESCREEN_H__ */


