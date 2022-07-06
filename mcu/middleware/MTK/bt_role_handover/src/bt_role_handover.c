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

#if defined(MTK_AWS_MCE_ENABLE)
#include "bt_role_handover.h"
#include "bt_role_handover_internal.h"
#include "bt_callback_manager.h"
#include "bt_type.h"
#include "bt_debug.h"
#include "bt_os_layer_api.h"
#include "FreeRTOS.h"
#include <timers.h>
#include "bt_connection_manager.h"

#define BT_ROLE_HANDOVER_RETRY_TIME   (100)
#define BT_ROLE_HANDOVER_PREPARE_TIME (5000)

//task event
#define BT_ROLE_HANDOVER_TASK_EVENT_RETRY_TIMER            (1 << 0)
#define BT_ROLE_HANDOVER_TASK_EVENT_PREPARE_TIMER          (1 << 1)
#define BT_ROLE_HANDOVER_TASK_EVENT_RX_STATE_CHANGE_IND_OK (1 << 2)

#define BT_ROLE_HANDOVER_DATA_HEADER_PER_MODULE (2)

#define BT_ROLE_HANDOVER_HEAP_FOOTER    (0xEFBEADDE)

log_create_module(BT_RHO, PRINT_LEVEL_INFO);

bt_role_handover_context_t bt_rho_srv_context;
bt_role_handover_callbacks_t bt_rho_srv_callbacks[BT_ROLE_HANDOVER_MODULE_MAX];

static bool bt_rho_srv_init_flag = false;
#define BT_ROLE_HANDOVER_IS_NOT_RHO_EVENT(event) \
    (((event) != BT_AWS_MCE_CONNECTED) && \
     ((event) != BT_AWS_MCE_DISCONNECTED) && \
     ((event) != BT_AWS_MCE_STATE_CHANGED_IND) && \
     ((event) != BT_AWS_MCE_PREPARE_ROLE_HANDOVER_CNF) && \
     ((event) != BT_AWS_MCE_ROLE_HANDOVER_CNF) && \
     ((event) != BT_AWS_MCE_ROLE_HANDOVER_IND))

void bt_role_handover_notify_agent_end(bt_status_t status);
bt_status_t RHOS_MULTIPOINT(bt_role_handover_start_internal)(void);
static bt_status_t bt_role_handover_is_allowed(void);
static bt_status_t bt_role_handover_prepare_rho(void);
static void bt_role_handover_set_aws_handle(uint32_t handle, bt_aws_mce_agent_state_type_t state);
static void bt_role_handover_store_rho_data(uint8_t *data, uint16_t length);
static void bt_role_handover_parse_rho_data(uint8_t *data, uint16_t length);
static void bt_role_handover_connect_handler(bt_status_t status, void *param);
static void bt_role_handover_disconnect_handler(bt_status_t status, void *param);
static void bt_role_handover_state_change_ind_handler(bt_status_t status, void *param);
static void bt_role_handover_prepare_cnf_handler(bt_status_t status, void *param);
static void bt_role_handover_rho_ind_handler(bt_status_t status, void *param);
static bt_status_t bt_role_handover_aws_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff);
static bt_status_t bt_role_handover_prepare_rho(void);
static void bt_role_handover_free_rho_data(void);
static void bt_role_handover_free_profile_info(void);

extern void bt_os_take_stack_mutex(void);
extern void bt_os_give_stack_mutex(void);

#define RHOS_MUTEX_LOCK() bt_os_take_stack_mutex()
#define RHOS_MUTEX_UNLOCK() bt_os_give_stack_mutex()

uint16_t bt_role_handover_get_max_length(void);

uint16_t default_bt_role_handover_get_max_length(void)
{
    return 255;
}

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_role_handover_get_max_length=_default_bt_role_handover_get_max_length")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_role_handover_get_max_length = default_bt_role_handover_get_max_length
#else
#error "Unsupported Platform"
#endif

static uint8_t *bt_role_handover_alloc(uint16_t size)
{
    uint32_t footer = BT_ROLE_HANDOVER_HEAP_FOOTER;
    uint8_t *p = (uint8_t *)pvPortMalloc(size + sizeof(footer));
    LOG_MSGID_I(BT_RHO, "alloc memory:0x%x, size:0x%x", 2, p, size + sizeof(footer));

    if (p != NULL) {
        memset(p, 0, size + sizeof(footer));
        memcpy(p + size, &footer, sizeof(footer));
    }

    return ((p == NULL) ? NULL : p);
}

static void bt_role_handover_free(uint8_t *p)
{
    LOG_MSGID_I(BT_RHO, "free memory:0x%x", 1, p);
    vPortFree((void *)p);
}

/*RHO service cant use BT external timer because external timer is stopped during RHO.
For busy case, RHO service needs to start a timer to retry */
static uint32_t bt_role_handover_is_timer_active(TimerHandle_t timer_id)
{
    if ((timer_id != NULL) && (xTimerIsTimerActive(timer_id) != pdFALSE)) {
        return 1;
    } else {
        return 0;
    }
}

static void bt_role_handover_stop_timer(TimerHandle_t timer_id)
{
    BaseType_t ret;

    if  ((timer_id != NULL) && (bt_role_handover_is_timer_active(timer_id) == 1)) {
        ret = xTimerStop(timer_id, 0);
        if (ret != pdPASS) {
            LOG_MSGID_E(BT_RHO, "stop_timer fail 0x%x", 1, ret);
            configASSERT(0);
        }
    }
}

static void bt_role_handover_start_timer_ext(char *name, TimerHandle_t *timer_id, bool is_repeat, uint32_t ms, TimerCallbackFunction_t pxCallbackFunction)
{
    BaseType_t ret;

    if (*timer_id == NULL) {
        *timer_id = xTimerCreate((const char *const)name, 0xffff, is_repeat, NULL, pxCallbackFunction);
        LOG_MSGID_E(BT_RHO, "create timer_id 0x%x", 1, *timer_id);
    }
    uint32_t time_length = ms / portTICK_PERIOD_MS + 1;
    if (*timer_id == NULL) {
        return;
    }
    ret = xTimerChangePeriod(*timer_id, time_length, 0);
    if (ret != pdPASS) {
        LOG_MSGID_E(BT_RHO, "start_timer_ext fail 0x%x", 1, ret);
        configASSERT(0);
    }
}

static void bt_role_handover_timer_expired_notify(TimerHandle_t id)
{
    uint32_t mask;
    bt_os_layer_disable_interrupt(&mask);
    if (id == bt_rho_srv_context.prepare_timer_handle) {
        bt_rho_srv_context.task_events |= BT_ROLE_HANDOVER_TASK_EVENT_PREPARE_TIMER;
    } else if (id == bt_rho_srv_context.retry_timer_handle) {
        bt_rho_srv_context.task_events |= BT_ROLE_HANDOVER_TASK_EVENT_RETRY_TIMER;
    }
    bt_os_layer_enable_interrupt(mask);
    bt_trigger_interrupt(0);
}

static void bt_role_handover_reset_state(void)
{
    bt_rho_srv_context.state = BT_ROLE_HANDOVER_STATE_IDLE;
}

static void bt_role_handover_status_callback(bt_role_handover_event_t event, bt_status_t status)
{
    uint32_t i;
    if (event == BT_ROLE_HANDOVER_START_IND) {
        LOG_MSGID_E(BT_RHO, "[BT_RHO]status_callback, RHO start, role 0x%x\r\n", 1, bt_rho_srv_context.role);
        //todo, use os layer for bluetooth service later
        bt_rho_srv_context.rho_time = bt_os_layer_get_hal_gpt_time();
    } else if (event == BT_ROLE_HANDOVER_COMPLETE_IND){
        uint32_t current = bt_os_layer_get_hal_gpt_time();
        LOG_MSGID_E(BT_RHO, "[BT_RHO]status_callback, RHO SRV end, role 0x%x, status 0x%x, time(ms):%ld\r\n", 3, bt_rho_srv_context.role, (unsigned int)status, current - bt_rho_srv_context.rho_time);
    }
    for (i = 0; i < BT_ROLE_HANDOVER_MODULE_MAX; i++){
        if (bt_rho_srv_callbacks[i].status_cb != NULL) {
            bt_rho_srv_callbacks[i].status_cb((const bt_bd_addr_t *)&(bt_rho_srv_context.remote_addr), bt_rho_srv_context.role, event, status);
        }
    }
}

//for agent
static void bt_role_handover_stop_timers()
{
    LOG_MSGID_I(BT_RHO, "stop_timer", 0);

    bt_role_handover_stop_timer(bt_rho_srv_context.prepare_timer_handle);
    bt_role_handover_stop_timer(bt_rho_srv_context.retry_timer_handle);
}

void bt_role_handover_notify_agent_end(bt_status_t status)
{
    LOG_MSGID_I(BT_RHO, "notify_agent_end, status 0x%x, task_events 0x%x", 2, status, bt_rho_srv_context.task_events);

    /* Stop timer firstly when success, then notify success when BT task run again. */
    if (status != BT_STATUS_SUCCESS) {
        bt_role_handover_stop_timers();
    }

    bt_role_handover_reset_state();
    bt_rho_srv_context.prepare_pending_flag = 0;
    bt_rho_srv_context.application_pending_flag = 0;
    bt_rho_srv_context.task_events = 0;
    bt_role_handover_status_callback(BT_ROLE_HANDOVER_COMPLETE_IND, status);
}

//for partner
static void bt_role_handover_notify_partner_end(bt_status_t status)
{
    LOG_MSGID_I(BT_RHO, "notify_partner_end, status 0x%x, task_events 0x%x", 2, status, bt_rho_srv_context.task_events);
    bt_role_handover_free_rho_data();
    bt_role_handover_free_profile_info();
    bt_role_handover_reset_state();
    bt_rho_srv_context.task_events = 0;
    bt_role_handover_status_callback(BT_ROLE_HANDOVER_COMPLETE_IND, status);
}

//switch to BT task to handle timer to avoid timer task blocking by BT mutex.
static void bt_role_handover_post_timeout_handler(TimerHandle_t id)
{
    bt_role_handover_timer_expired_notify(id);
}

static void bt_role_handover_prepare_timeout_handler(TimerHandle_t xTimer)
{
    LOG_MSGID_I(BT_RHO, "prepare_timeout_handler, flag 0x%x, application_flag:0x%x", 2,
        bt_rho_srv_context.prepare_pending_flag,
        bt_rho_srv_context.application_pending_flag);
    bt_role_handover_notify_agent_end(BT_STATUS_TIMEOUT);
}

static void bt_role_handover_retry_timeout_handler(TimerHandle_t xTimer)
{
    LOG_MSGID_I(BT_RHO, "retry_timeout_handler\r\n", 0);
    //todo, add max retry times.
    bt_status_t status = bt_role_handover_prepare_rho();
    if (status != BT_STATUS_BUSY && status != BT_STATUS_SUCCESS) {
        bt_role_handover_notify_agent_end(status);
    }
}

static bt_status_t bt_role_handover_prepare_rho(void) 
{
    bt_status_t status;
    //prepare RHO
    bt_bd_addr_t *partner_address = bt_connection_manager_device_local_info_get_peer_aws_address();
    uint32_t current = bt_os_layer_get_hal_gpt_time();
    LOG_MSGID_E(BT_RHO, "[BT_RHO]prepare_rho, role 0x%x, time(ms):%ld\r\n", 2, bt_rho_srv_context.role, current - bt_rho_srv_context.rho_time);

    if (bt_rho_srv_context.aws_state != BT_AWS_MCE_AGENT_STATE_ATTACHED) {
        LOG_MSGID_I(BT_RHO, "[BT_RHO]prepare rho, Partner not attached!", 0);
        return BT_STATUS_FAIL;
    }

    status = bt_aws_mce_prepare_role_handover(bt_rho_srv_context.aws_handle, (const bt_bd_addr_t *)partner_address);
    if (status == BT_STATUS_BUSY) {
        //start a retry timer.
        bt_role_handover_start_timer_ext("RHOS retry", &bt_rho_srv_context.retry_timer_handle, false, BT_ROLE_HANDOVER_RETRY_TIME, bt_role_handover_post_timeout_handler);
    } else {
        //waiting for prepare cnf or failed.
    }
    return status;
}

static void bt_role_handover_rho_cnf_handler(bt_status_t status, void *buff)
{
    if (status == BT_STATUS_SUCCESS) {
        //waiting for status change ind.
    } else if (status == BT_STATUS_BUSY) {
        //start a retry timer
        bt_role_handover_start_timer_ext("RHOS retry", &bt_rho_srv_context.retry_timer_handle, false, BT_ROLE_HANDOVER_RETRY_TIME, bt_role_handover_post_timeout_handler);
    } else {
        //notify fail
        bt_role_handover_notify_agent_end(status);
    }

}

//for partner
static void bt_role_handover_free_rho_data(void)
{
    if (bt_rho_srv_context.rho_data != NULL) {
        bt_role_handover_free(bt_rho_srv_context.rho_data);
        bt_rho_srv_context.rho_data = NULL;
        memset(bt_rho_srv_context.rho_data_len, 0, sizeof(bt_rho_srv_context.rho_data_len));
    }
}

//for partner
static void bt_role_handover_free_profile_info(void)
{
    if (bt_rho_srv_context.profile_info != NULL) {
        bt_role_handover_free((uint8_t *)bt_rho_srv_context.profile_info);
        bt_rho_srv_context.profile_info = NULL;
    }
}

static void bt_role_handover_state_change_ind_notify(void)
{
    uint32_t mask;
    bt_os_layer_disable_interrupt(&mask);
    bt_rho_srv_context.task_events |= BT_ROLE_HANDOVER_TASK_EVENT_RX_STATE_CHANGE_IND_OK;
    bt_os_layer_enable_interrupt(mask);
    bt_trigger_interrupt(0);
}

static void bt_role_handover_update_handler(bt_status_t status)
{
    if (bt_rho_srv_context.role == BT_AWS_MCE_ROLE_AGENT) {
        bt_role_handover_notify_agent_end(status);
    } else {
        bt_role_handover_notify_partner_end(status);
    }
    
    //update role, handle is same, addr will be updated when start rho again.
    if (status == BT_STATUS_SUCCESS) {
        bt_rho_srv_context.role = (bt_rho_srv_context.role == BT_AWS_MCE_ROLE_AGENT) ? BT_AWS_MCE_ROLE_PARTNER : BT_AWS_MCE_ROLE_AGENT;
    }

}

static bool bt_role_handover_is_valid_state_change_ind(bt_aws_mce_agent_state_type_t pre_state)
{
    //state change might be reported before RHO state change.
    if ((bt_rho_srv_context.role == BT_AWS_MCE_ROLE_AGENT && \
        (pre_state == BT_AWS_MCE_AGENT_STATE_ATTACHED && bt_rho_srv_context.aws_state == BT_AWS_MCE_AGENT_STATE_NONE)) || \
        (bt_rho_srv_context.role == BT_AWS_MCE_ROLE_PARTNER && \
        (pre_state == BT_AWS_MCE_AGENT_STATE_NONE && bt_rho_srv_context.aws_state == BT_AWS_MCE_AGENT_STATE_ATTACHED))) {
        return true;
    }
    LOG_MSGID_E(BT_RHO, "ignore invalid state change ind for RHO SRV, role 0x%x, pre_state 0x%x, aws_state 0x%x", 3, 
        bt_rho_srv_context.role,
        pre_state,
        bt_rho_srv_context.aws_state);
    return false;
}

static bt_status_t bt_role_handover_is_allowed(void)
{
    const bt_bd_addr_t *address = NULL;
    bt_rho_srv_context.role = bt_connection_manager_device_local_info_get_aws_role();

    if (bt_rho_srv_context.state != BT_ROLE_HANDOVER_STATE_IDLE) {
        return BT_STATUS_ROLE_HANDOVER_ONGOING;
    }

    if (bt_rho_srv_context.aws_handle == 0) {
        return BT_STATUS_ROLE_HANDOVER_AWS_CONNECTION_NOT_FOUND;
    } else {
        /* Get remote address again */
        address = bt_aws_mce_get_bd_addr_by_handle(bt_rho_srv_context.aws_handle);

        if (address == NULL) {
            return BT_STATUS_ROLE_HANDOVER_AWS_CONNECTION_NOT_FOUND;
        } else {
            memcpy(&(bt_rho_srv_context.remote_addr), address, sizeof(bt_bd_addr_t));
        }
    }

    if (bt_rho_srv_context.role != BT_AWS_MCE_ROLE_AGENT) {
        return BT_STATUS_ROLE_HANDOVER_NOT_AGENT;
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t bt_role_handover_register_callbacks(bt_role_handover_module_type_t type, bt_role_handover_callbacks_t *callbacks)
{
    bt_role_handover_init();

    if ((type >= BT_ROLE_HANDOVER_MODULE_MAX) || (callbacks->status_cb == NULL)) {
        LOG_MSGID_I(BT_RHO, "register callbacks, wrong type 0x%x or status callback", 1, type);
        return BT_STATUS_FAIL;
    }

    if (bt_rho_srv_callbacks[type].status_cb == NULL) {
        LOG_MSGID_D(BT_RHO, "register callbacks, type 0x%x registed", 1, type);
        memcpy(&bt_rho_srv_callbacks[type], callbacks, sizeof(bt_role_handover_callbacks_t));
    } else {
        if (bt_rho_srv_callbacks[type].status_cb != callbacks->status_cb) {
            LOG_MSGID_I(BT_RHO, "regiter callbacks, type 0x%x registed different callbacks", 1, type);
            return BT_STATUS_FAIL;
        }
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t bt_role_handover_deregister_callbacks(bt_role_handover_module_type_t type)
{
    if (type >= BT_ROLE_HANDOVER_MODULE_MAX) {
        LOG_MSGID_I(BT_RHO, "deregister callbacks, wrong type 0x%x", 1, type);
        return BT_STATUS_FAIL;
    }

    if (bt_rho_srv_callbacks[type].status_cb != NULL) {
        LOG_MSGID_D(BT_RHO, "deregister callbacks, type 0x%x deregisted", 1, type);
        memset(&bt_rho_srv_callbacks[type], 0, sizeof(bt_role_handover_callbacks_t));
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t bt_role_handover_start(void)
{
    RHOS_MUTEX_LOCK();
    bt_status_t status = BT_STATUS_SUCCESS;

    LOG_MSGID_I(BT_RHO, "start, state:%d aws_handle:0x%x role:0x%x", 3,
        bt_rho_srv_context.state, bt_rho_srv_context.aws_handle, bt_rho_srv_context.role);

    /* Check RHO is allowed. */
    status = bt_role_handover_is_allowed();
    if (status != BT_STATUS_SUCCESS) {
        RHOS_MUTEX_UNLOCK();
        return status;
    }

    /* Notify user RHO started. */
    bt_rho_srv_context.state = BT_ROLE_HANDOVER_STATE_ONGOING;
    bt_role_handover_status_callback(BT_ROLE_HANDOVER_START_IND, BT_STATUS_SUCCESS);

    status = bt_role_handover_start_internal();

    RHOS_MUTEX_UNLOCK();
    return status;
}

bt_status_t bt_role_handover_reply_prepare_request(bt_role_handover_module_type_t type)
{
    RHOS_MUTEX_LOCK();
    bt_status_t status = BT_STATUS_SUCCESS;

    LOG_MSGID_I(BT_RHO, "reply prepare request, type:0x%02x, flag:0x%x application_flag:0x%04x", 3,
        type, bt_rho_srv_context.prepare_pending_flag, bt_rho_srv_context.application_pending_flag);

#ifdef AIR_MULTI_POINT_ENABLE
    LOG_MSGID_I(BT_RHO, "reply prepare request, flag:0x%x", 1, bt_rho_srv_context.flag);

    if ((bt_rho_srv_context.flag & BT_ROLE_HANDOVER_FLAG_CM_PREPARE) != 0) {
        bt_rho_srv_context.flag &= ~BT_ROLE_HANDOVER_FLAG_CM_PREPARE;
        RHOS_MULTIPOINT(bt_role_handover_start_internal)();
        RHOS_MUTEX_UNLOCK();
        return BT_STATUS_SUCCESS;
    }
#endif /* AIR_MULTI_POINT_ENABLE */

    if ((type >= BT_ROLE_HANDOVER_MODULE_MAX) ||
        ((bt_rho_srv_context.prepare_pending_flag == 0) && (bt_rho_srv_context.application_pending_flag == 0))) {
        LOG_MSGID_I(BT_RHO, "reply prepare request, wrong type or all ready", 0);
        RHOS_MUTEX_UNLOCK();
        return BT_STATUS_FAIL;
    }

    if (type < BT_ROLE_HANDOVER_MODULE_CUSTOM_START) {
        bt_rho_srv_context.prepare_pending_flag &= ~(1 << type);
    } else {
        bt_rho_srv_context.application_pending_flag &= ~(1 << (type - BT_ROLE_HANDOVER_MODULE_CUSTOM_START));
    }

    if ((bt_rho_srv_context.prepare_pending_flag == 0) && (bt_rho_srv_context.application_pending_flag == 0)) {
        bt_role_handover_stop_timer(bt_rho_srv_context.prepare_timer_handle);
        status = bt_role_handover_prepare_rho();
        if ((status != BT_STATUS_SUCCESS) && (status != BT_STATUS_BUSY)) {
            bt_role_handover_notify_agent_end(status);
            RHOS_MUTEX_UNLOCK();
            return BT_STATUS_FAIL;
        }
    }

    RHOS_MUTEX_UNLOCK();
    return BT_STATUS_SUCCESS;
}

bt_role_handover_state_t bt_role_handover_get_state(void)
{
    return bt_rho_srv_context.state;
}

void RHOS_MULTIPOINT(bt_role_handover_init)(void)
{
    if (!bt_rho_srv_init_flag) {
        bt_rho_srv_init_flag = true;
        memset(&bt_rho_srv_context, 0, sizeof(bt_role_handover_context_t));
        bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_AWS_MCE, bt_role_handover_aws_event_callback);
    }
}

bt_status_t RHOS_MULTIPOINT(bt_role_handover_start_internal)(void)
{
    /* Check user is allowed. */
    bt_status_t status
        = bt_role_handover_is_user_allowed((const bt_bd_addr_t *)(&(bt_rho_srv_context.remote_addr)));

    if (status != BT_STATUS_SUCCESS) {
        return status;
    }

    /* Handle pending user. */
    LOG_MSGID_I(BT_RHO, "start, flag:0x%x application_flag:0x%x", 2,
        bt_rho_srv_context.prepare_pending_flag, bt_rho_srv_context.application_pending_flag);

    if ((bt_rho_srv_context.prepare_pending_flag != 0) ||
        (bt_rho_srv_context.application_pending_flag != 0)) {
        bt_role_handover_start_timer_ext("RHOS Prepare", &(bt_rho_srv_context.prepare_timer_handle), false, BT_ROLE_HANDOVER_PREPARE_TIME, bt_role_handover_post_timeout_handler);
        bt_role_handover_status_callback(BT_ROLE_HANDOVER_PREPARE_REQ_IND, BT_STATUS_SUCCESS);
    } else {
        bt_role_handover_status_callback(BT_ROLE_HANDOVER_PREPARE_REQ_IND, BT_STATUS_SUCCESS);

        /* Prepare RHO to BT stack. */
        status = bt_role_handover_prepare_rho();

        if ((status != BT_STATUS_SUCCESS) && (status != BT_STATUS_BUSY)) {
            bt_role_handover_notify_agent_end(status);
        }
    }

    return status;
}

bt_status_t RHOS_MULTIPOINT(bt_role_handover_is_user_allowed)(const bt_bd_addr_t *address)
{
    for (uint32_t i = 0; i < BT_ROLE_HANDOVER_MODULE_MAX; i++) {
        if (bt_rho_srv_callbacks[i].allowed_cb != NULL) {
            bt_status_t status = bt_rho_srv_callbacks[i].allowed_cb(address);

            if (status == BT_STATUS_PENDING) {
                if (i < BT_ROLE_HANDOVER_MODULE_CUSTOM_START) {
                    bt_rho_srv_context.prepare_pending_flag |= (1 << i);
                } else {
                    bt_rho_srv_context.application_pending_flag |= (1 << (i - BT_ROLE_HANDOVER_MODULE_CUSTOM_START));
                }
            } else {
                if (status != BT_STATUS_SUCCESS) {
                    bt_role_handover_notify_agent_end(status);
                    return status;
                }
            }
        }
    }

    return BT_STATUS_SUCCESS;
}

uint16_t RHOS_MULTIPOINT(bt_role_handover_get_user_length)(const bt_bd_addr_t *address, uint8_t *length_array)
{
    uint16_t total_length = 0;

    if (address == NULL) {
        LOG_MSGID_I(BT_RHO, "get user length, address:NULL", 0);
    } else {
        LOG_MSGID_I(BT_RHO, "get user length, address:0x%02x-%02x-%02x-%02x-%02x-%02x", 6,
            (*address)[0], (*address)[1], (*address)[2], (*address)[3], (*address)[4], (*address)[5]);
    }

    for (uint32_t type = 0; type < BT_ROLE_HANDOVER_MODULE_MAX; type++) {
        if (bt_rho_srv_callbacks[type].get_len_cb != NULL) {
            length_array[type] = bt_rho_srv_callbacks[type].get_len_cb(address);
            LOG_MSGID_I(BT_RHO, "get user length, type:0x%02x length:%d", 2, type, length_array[type]);

            if (length_array[type] > 0) {
                total_length += length_array[type];
                total_length += BT_ROLE_HANDOVER_DATA_HEADER_PER_MODULE;
            }
        }
    }

    return total_length;
}

uint16_t RHOS_MULTIPOINT(bt_role_handover_get_user_data)(const bt_bd_addr_t *address, uint8_t *data, uint8_t *length_array)
{
    uint16_t total_length = 0;
    uint8_t *module_data = data;
    bt_status_t status = BT_STATUS_SUCCESS;

    if (address == NULL) {
        LOG_MSGID_I(BT_RHO, "get user data, adddress:NULL", 0);
    } else {
        LOG_MSGID_I(BT_RHO, "get user data, address:0x%02x-%02x-%02x-%02x-%02x-%02x", 6,
            (*address)[0], (*address)[1], (*address)[2], (*address)[3], (*address)[4], (*address)[5]);
    }

    for (uint32_t type = 0; type < BT_ROLE_HANDOVER_MODULE_MAX; type++) {
        if ((length_array[type] != 0) && (bt_rho_srv_callbacks[type].get_data_cb != NULL)) {
            module_data[0] = type;
            module_data[1] = length_array[type];
            status = bt_rho_srv_callbacks[type].get_data_cb(address, module_data + BT_ROLE_HANDOVER_DATA_HEADER_PER_MODULE);

            if (status == BT_STATUS_SUCCESS) {
                module_data += length_array[type];
                module_data += BT_ROLE_HANDOVER_DATA_HEADER_PER_MODULE;
                total_length += length_array[type];
                total_length += BT_ROLE_HANDOVER_DATA_HEADER_PER_MODULE;
            }
        }
    }

    return total_length;
}

void RHOS_MULTIPOINT(bt_role_handover_update_user)(bt_role_handover_module_type_t type, bt_role_handover_update_info_t *update_info)
{
    if (bt_rho_srv_callbacks[type].update_cb != NULL) {
        bt_rho_srv_callbacks[type].update_cb(update_info);
    }
}

void RHOS_MULTIPOINT(bt_role_handover_update_agent_user)(const bt_bd_addr_t *address)
{
    bt_role_handover_update_info_t update_info = {0};

    if (address == NULL) {
        LOG_MSGID_I(BT_RHO, "update agent user, address:NULL", 0);
    } else {
        LOG_MSGID_I(BT_RHO, "update agent user, address:0x%02x-%02x-%02x-%02x-%02x-%02x", 6,
            (*address)[0], (*address)[1], (*address)[2], (*address)[3], (*address)[4], (*address)[5]);
    }

    update_info.addr = address;
    update_info.role = bt_rho_srv_context.role;

    if ((address != NULL) &&
        (memcmp(&(bt_rho_srv_context.remote_addr), address, sizeof(bt_bd_addr_t)) == 0)) {
        update_info.is_active = true;
    }

    for (int32_t i = BT_ROLE_HANDOVER_MODULE_MAX - 1; i >= 0; i--) {
        bt_role_handover_update_user(i, &update_info);
    }
}

void RHOS_MULTIPOINT(bt_role_handover_update_partner_user)(const bt_bd_addr_t *address, uint8_t *data, uint16_t length)
{
    uint8_t *module_data = data;
    bt_role_handover_update_info_t update_info = {0};

    if (address == NULL) {
        LOG_MSGID_I(BT_RHO, "update partner user, address:NULL", 0);
    } else {
        LOG_MSGID_I(BT_RHO, "update partner user, address:0x%02x-%02x-%02x-%02x-%02x-%02x", 6,
            (*address)[0], (*address)[1], (*address)[2], (*address)[3], (*address)[4], (*address)[5]);
    }

    /* Parse RHO module length. */
    bt_role_handover_parse_rho_data(data, length);

    update_info.addr = address;
    update_info.role = bt_rho_srv_context.role;
    update_info.profile_info = bt_rho_srv_context.profile_info;

    if ((address != NULL) &&
        (memcmp(&(bt_rho_srv_context.remote_addr), address, sizeof(bt_bd_addr_t)) == 0)) {
        update_info.is_active = true;
    }

    for (uint32_t i = 0; i < BT_ROLE_HANDOVER_MODULE_MAX; i++) {
        if (bt_rho_srv_context.rho_data_len[i] != 0) {
            update_info.data = module_data + BT_ROLE_HANDOVER_DATA_HEADER_PER_MODULE;
            update_info.length = bt_rho_srv_context.rho_data_len[i];
            module_data += bt_rho_srv_context.rho_data_len[i];
            module_data += BT_ROLE_HANDOVER_DATA_HEADER_PER_MODULE;
        } else {
            update_info.data = NULL;
            update_info.length = 0;
        }

        bt_role_handover_update_user(i, &update_info);
    }
}

static void bt_role_handover_parse_rho_data(uint8_t *data, uint16_t length)
{
    uint16_t parse_length = 0;
    uint8_t *parse_data = data;

    /* Clear length array */
    memset(&(bt_rho_srv_context.rho_data_len), 0, sizeof(bt_rho_srv_context.rho_data_len));

    while (parse_length < length) {
        uint8_t type = parse_data[0];
        configASSERT(type <= BT_ROLE_HANDOVER_MODULE_MAX);

        uint8_t data_len = parse_data[1];
        bt_rho_srv_context.rho_data_len[type] = data_len;
        LOG_MSGID_I(BT_RHO, "parse rho data, type:0x%02x length:%d", 2, type, data_len);

        parse_length += data_len;
        parse_length += BT_ROLE_HANDOVER_DATA_HEADER_PER_MODULE;
        parse_data += data_len;
        parse_data += BT_ROLE_HANDOVER_DATA_HEADER_PER_MODULE;
    }
}

static void bt_role_handover_store_rho_data(uint8_t *data, uint16_t length)
{
    if ((data == NULL) || (length == 0)) {
        return;
    }

    /* Allocate memory for RHO data. */
    bt_rho_srv_context.rho_data = (uint8_t *)bt_role_handover_alloc(length);

    if (bt_rho_srv_context.rho_data != NULL) {
        bt_rho_srv_context.total_len = length;
        memcpy(bt_rho_srv_context.rho_data, data, length);
    } else {
        LOG_MSGID_I(BT_RHO, "store rho data, length %d out of memory", 1, length);
        bt_role_handover_notify_partner_end(BT_STATUS_OUT_OF_MEMORY);
    }
}

static void bt_role_handover_dump_profile_info(bt_aws_mce_role_handover_profile_t *profile_info)
{
    LOG_MSGID_I(BT_RHO, "dump profile info, gap_handle:0x%x hfp_handle:0x%x a2dp_handle:0x%x "
        "avrcp_handle:0x%x hsp_handle:0x%x spp_handle:0x%x gap_le_handle:0x%x gatt_handle:0x%x", 8,
        profile_info->gap_handle, profile_info->hfp_handle, profile_info->a2dp_handle, profile_info->avrcp_handle,
        profile_info->hsp_handle, profile_info->spp_handle, profile_info->gap_le_handle, profile_info->gatt_handle);
}

void bt_role_handover_store_profile_info(bt_aws_mce_role_handover_profile_t *profile_info)
{
    if (profile_info == NULL) {
        return;
    }

    if (bt_rho_srv_context.profile_info == NULL) {
        bt_rho_srv_context.profile_info = (bt_aws_mce_role_handover_profile_t *)bt_role_handover_alloc(sizeof(bt_aws_mce_role_handover_profile_t));
    }

    if (bt_rho_srv_context.profile_info != NULL) {
        bt_role_handover_dump_profile_info(profile_info);
        memcpy(bt_rho_srv_context.profile_info, profile_info, sizeof(bt_aws_mce_role_handover_profile_t));
    } else {
        LOG_MSGID_I(BT_RHO, "store profile info, out of memory", 0);
        bt_role_handover_notify_partner_end(BT_STATUS_OUT_OF_MEMORY);
    }
}

static void bt_role_handover_set_aws_handle(uint32_t handle, bt_aws_mce_agent_state_type_t state)
{
    const bt_bd_addr_t *remote_addr = bt_aws_mce_get_bd_addr_by_handle(handle);
    bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();

    if (memcmp(remote_addr, local_addr, sizeof(bt_bd_addr_t)) != 0) {
        LOG_MSGID_I(BT_RHO, "set aws handle, switch to handle 0x%x", 1, handle);
        bt_rho_srv_context.aws_handle = handle;
        bt_rho_srv_context.aws_state = state;
        memcpy(&(bt_rho_srv_context.remote_addr), remote_addr, sizeof(bt_bd_addr_t));
    } else {
        LOG_MSGID_I(BT_RHO, "set aws handle, handle 0x%x is special link", 1, handle);
    }
}

static void bt_role_handover_connect_handler(bt_status_t status, void *param)
{
    bt_aws_mce_connected_t *connected = (bt_aws_mce_connected_t *)param;

    if ((status == BT_STATUS_SUCCESS) && (bt_rho_srv_context.aws_handle == 0)) {
        bt_role_handover_set_aws_handle(connected->handle, BT_AWS_MCE_AGENT_STATE_NONE);
    }
}

static void bt_role_handover_disconnect_handler(bt_status_t status, void *param)
{
    bt_aws_mce_disconnected_t *disconnected = (bt_aws_mce_disconnected_t *)param;
    bt_role_handover_state_t state = bt_role_handover_get_state();

    if ((state == BT_ROLE_HANDOVER_STATE_ONGOING) &&
        (disconnected->handle == bt_rho_srv_context.aws_handle)) {
        if (bt_rho_srv_context.role == BT_AWS_MCE_ROLE_AGENT) {
            bt_role_handover_notify_agent_end(disconnected->reason);
        } else {
            bt_role_handover_notify_partner_end(disconnected->reason);
        }
    }

    /* Reset aws handle. */
    if (disconnected->handle == bt_rho_srv_context.aws_handle) {
        LOG_MSGID_I(BT_RHO, "disconnect handler, reset to handle 0x0", 0);
        bt_rho_srv_context.aws_handle = 0;
        bt_rho_srv_context.aws_state = BT_AWS_MCE_AGENT_STATE_NONE;
        memset(&(bt_rho_srv_context.remote_addr), 0, sizeof(bt_bd_addr_t));
    } else {
        LOG_MSGID_I(BT_RHO, "disconnect handler, handle 0x%x != context handle 0x%x", 2,
            disconnected->handle, bt_rho_srv_context.aws_handle);
    }
}

static void bt_role_handover_state_change_ind_handler(bt_status_t status, void *param)
{
    bt_aws_mce_agent_state_type_t pre_state = BT_AWS_MCE_AGENT_STATE_NONE;
    bt_aws_mce_state_change_ind_t *state_change = (bt_aws_mce_state_change_ind_t *)param;
    bt_role_handover_state_t state = bt_role_handover_get_state();

    if (state_change->handle == bt_rho_srv_context.aws_handle) {
        pre_state = bt_rho_srv_context.aws_state;
        bt_rho_srv_context.aws_state = state_change->state;
    } else {
        if ((state_change->state == BT_AWS_MCE_AGENT_STATE_ATTACHED) &&
            (bt_rho_srv_context.aws_state != BT_AWS_MCE_AGENT_STATE_ATTACHED)) {
            bt_role_handover_set_aws_handle(state_change->handle, BT_AWS_MCE_AGENT_STATE_ATTACHED);
        } else {
            LOG_MSGID_I(BT_RHO, "state change handler, handle 0x%x != context handle 0x%x", 2,
                state_change->handle, bt_rho_srv_context.aws_handle);
        }
        return;
    }

    if ((state == BT_ROLE_HANDOVER_STATE_ONGOING) &&
        (bt_role_handover_is_valid_state_change_ind(pre_state))) {
        if (status == BT_STATUS_SUCCESS) {
            if (bt_rho_srv_context.role == BT_AWS_MCE_ROLE_AGENT) {
                bt_role_handover_update_agent_user((const bt_bd_addr_t *)(&(bt_rho_srv_context.remote_addr)));
            } else {
                bt_role_handover_update_partner_user((const bt_bd_addr_t *)(&(bt_rho_srv_context.remote_addr)), bt_rho_srv_context.rho_data, bt_rho_srv_context.total_len);
            }
            
            /* Notify success when BT task run again. */
            bt_role_handover_stop_timers();
            bt_role_handover_state_change_ind_notify();
        } else {
            bt_role_handover_update_handler(status);
        }
    }
}

static void bt_role_handover_prepare_cnf_handler(bt_status_t status, void *param)
{
    uint16_t total_length = 0;
    bt_status_t result = status;

    /* Skip prepare cnf after cancel RHO */
    if (bt_rho_srv_context.state == BT_ROLE_HANDOVER_STATE_IDLE) {
        LOG_MSGID_I(BT_RHO, "prepare cnf handler, skip in IDLE state", 0);
        return;
    }

    if (status == BT_STATUS_SUCCESS) {
        total_length = bt_role_handover_get_user_length((const bt_bd_addr_t *)(&(bt_rho_srv_context.remote_addr)), bt_rho_srv_context.rho_data_len);

        if (total_length > bt_role_handover_get_max_length()) {
            LOG_MSGID_I(BT_RHO, "prepare cnf handler, length %d is too large", 1, total_length);
            bt_role_handover_notify_agent_end(BT_STATUS_UNSUPPORTED);
            return;
        }

        /* Get user data */
        bt_rho_srv_context.rho_data = bt_role_handover_alloc(total_length);
        
        if (bt_rho_srv_context.rho_data != NULL) {
            total_length = bt_role_handover_get_user_data((const bt_bd_addr_t *)(&(bt_rho_srv_context.remote_addr)), bt_rho_srv_context.rho_data, bt_rho_srv_context.rho_data_len);
            result = bt_aws_mce_role_handover(bt_rho_srv_context.aws_handle, bt_rho_srv_context.rho_data, total_length);
        } else {
            LOG_MSGID_I(BT_RHO, "prepare cnf handler, length %d out of memory", 1, total_length);
            bt_role_handover_notify_agent_end(BT_STATUS_OUT_OF_MEMORY);
            return;
        }
    }

    if (bt_rho_srv_context.rho_data != NULL) {
        bt_role_handover_free(bt_rho_srv_context.rho_data);
        bt_rho_srv_context.rho_data = NULL;
    }

    if (result == BT_STATUS_SUCCESS) {
        /* Waiting Role Change */
    } else if (result == BT_STATUS_BUSY) {
        bt_role_handover_start_timer_ext("RHOS_RETRY", &(bt_rho_srv_context.retry_timer_handle), false, BT_ROLE_HANDOVER_RETRY_TIME, bt_role_handover_post_timeout_handler);
    } else {
        bt_role_handover_notify_agent_end(result);
    }
}

static void bt_role_handover_rho_ind_handler(bt_status_t status, void *param)
{
    bt_aws_mce_role_handover_ind_t *rho_ind = (bt_aws_mce_role_handover_ind_t *)param;
    bt_rho_srv_context.role = bt_connection_manager_device_local_info_get_aws_role();

    if (status == BT_STATUS_SUCCESS) {
        /* Get remote address again. */
        const bt_bd_addr_t *address = bt_aws_mce_get_bd_addr_by_handle(bt_rho_srv_context.aws_handle);

        if (address == NULL) {
            return;
        }

        bt_rho_srv_context.state = BT_ROLE_HANDOVER_STATE_ONGOING;
        memcpy(&(bt_rho_srv_context.remote_addr), address, sizeof(bt_bd_addr_t));
        bt_role_handover_status_callback(BT_ROLE_HANDOVER_START_IND, BT_STATUS_SUCCESS);

        /* Store data and profile info, and waiting for status change ind. */
        bt_role_handover_store_rho_data(rho_ind->data, rho_ind->length);
        bt_role_handover_store_profile_info(&(rho_ind->profile_info));
    }
}

bt_status_t bt_role_handover_aws_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    if (BT_ROLE_HANDOVER_IS_NOT_RHO_EVENT(msg)) {
        return BT_STATUS_FAIL;
    }

    LOG_MSGID_I(BT_RHO, "event callback, msg 0x%x status 0x%x, rho state 0x%x", 3, msg, status, bt_role_handover_get_state());

    /* if bt task is blocked by other high priority tasks when handle rx,
     * need to handle rho event firstly before handle other aws events. */
    if (bt_rho_srv_context.task_events & BT_ROLE_HANDOVER_TASK_EVENT_RX_STATE_CHANGE_IND_OK) {
        LOG_MSGID_I(BT_RHO, "notify previous rho result firstly, task_events 0x%x", 1, bt_rho_srv_context.task_events);
        bt_rho_srv_context.task_events &= ~BT_ROLE_HANDOVER_TASK_EVENT_RX_STATE_CHANGE_IND_OK;
        bt_role_handover_update_handler(BT_STATUS_SUCCESS);
    }

    switch (msg) {
        case BT_AWS_MCE_CONNECTED:
        {
            bt_role_handover_connect_handler(status, buff);
            break;
        }
        case BT_AWS_MCE_DISCONNECTED:
        {
            bt_role_handover_disconnect_handler(status, buff);
            break;
        }
        case BT_AWS_MCE_STATE_CHANGED_IND:
        {
            bt_role_handover_state_change_ind_handler(status, buff);
            break;
        }
        case BT_AWS_MCE_PREPARE_ROLE_HANDOVER_CNF:
        {
            bt_role_handover_prepare_cnf_handler(status, buff);
            break;
        }
        case BT_AWS_MCE_ROLE_HANDOVER_CNF:
        {
            bt_role_handover_rho_cnf_handler(status, buff);
            break;
        }
        case BT_AWS_MCE_ROLE_HANDOVER_IND:
        {
            bt_role_handover_rho_ind_handler(status, buff);
            break;
        }
    }

    return BT_STATUS_SUCCESS;
}

int32_t bt_role_handover_handle_interrupt(void)
{
    uint32_t current_events, mask;
    RHOS_MUTEX_LOCK();
    bt_os_layer_disable_interrupt(&mask);
    current_events = bt_rho_srv_context.task_events;
    bt_rho_srv_context.task_events = 0;
    bt_os_layer_enable_interrupt(mask);

    if (current_events & BT_ROLE_HANDOVER_TASK_EVENT_RETRY_TIMER) {
        bt_role_handover_retry_timeout_handler(NULL);
    }

    if (current_events & BT_ROLE_HANDOVER_TASK_EVENT_PREPARE_TIMER) {
        bt_role_handover_prepare_timeout_handler(NULL);
    }

    if (current_events & BT_ROLE_HANDOVER_TASK_EVENT_RX_STATE_CHANGE_IND_OK) {
        bt_role_handover_update_handler(BT_STATUS_SUCCESS);
    }
    
    RHOS_MUTEX_UNLOCK();
    return BT_STATUS_SUCCESS;
}
#endif /*#if defined(MTK_AWS_MCE_ENABLE) */

