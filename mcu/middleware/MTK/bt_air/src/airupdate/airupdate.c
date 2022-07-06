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
 
#ifdef MTK_PORT_SERVICE_BT_ENABLE
#ifdef MTK_AIRUPDATE_ENABLE
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "bt_airupdate.h"
#include "bt_callback_manager.h"
#include "bt_system.h"
#include "airupdate.h"
#include "airupdate_interface.h"
#include "syslog.h"


log_create_module(AIRUPDATE, PRINT_LEVEL_INFO);

typedef struct {
    bool in_use;
    airupdate_notify_callback callback;
} airupdate_callback_node_t;

static airupdate_node_t g_airupdate_list;
static airupdate_node_t *g_airupdate_head = NULL;
static uint32_t airupdate_mutex;

static airupdate_cntx_t airupdate_cntx = {{0}};

static airupdate_callback_node_t airupdate_cb_list[AIRUPDATE_SUPPORT_CB_MAX_NUM] = {{0}};

static airupdate_node_t *airupdate_get_head(void);
static uint32_t airupdate_check_node_buffer(uint8_t *data, uint16_t data_size);
static bt_status_t airupdate_check_user(void);
static void airupdate_clear_node_list();


//MUTEX LOCK
static uint32_t airupdate_create_mutex(void)
{
    return (uint32_t)xSemaphoreCreateRecursiveMutex();
}

static void airupdate_take_mutex(uint32_t mutex_id)
{
    if (taskSCHEDULER_NOT_STARTED == xTaskGetSchedulerState()) {
        return;
    }
    if (0 == mutex_id) {
        configASSERT(0 && "Airupdate is not initialized.");
    }
        
    xSemaphoreTakeRecursive((SemaphoreHandle_t)mutex_id, portMAX_DELAY);
}

static void airupdate_give_mutex(uint32_t mutex_id)
{
    if (taskSCHEDULER_NOT_STARTED == xTaskGetSchedulerState()) {
        return;
    }
    xSemaphoreGiveRecursive((SemaphoreHandle_t)mutex_id);
}

/*
static void airupdate_delete_mutex(uint32_t mutex_id)
{
    vSemaphoreDelete((SemaphoreHandle_t)mutex_id);
}
*/


#if defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE)
#include "bt_role_handover.h"
BT_PACKED (
typedef struct {
    bool     ready_to_send;
    uint16_t max_packet_size; 
    uint32_t conn_handle;
}) airupdate_rho_data_t;

static bt_status_t airupdate_rho_allowed_cb(const bt_bd_addr_t *addr)
{
    //App need call airupdate_get_rx_available() to check if any data in RX queue, if have, need get all data, then to allow RHO
    return BT_STATUS_SUCCESS;
}

static uint8_t airupdate_rho_get_data_length_cb(const bt_bd_addr_t *addr)
{
    if (addr && (true == airupdate_cntx.connected) && 
            (0 == memcmp((airupdate_cntx.bt_addr), addr, BT_BD_ADDR_LEN))) {
        return sizeof(airupdate_rho_data_t);
    }
    return 0;
}

static bt_status_t airupdate_rho_get_data_cb(const bt_bd_addr_t *addr, void * data)
{
    if (addr && data && (true == airupdate_cntx.connected) && 
            (0 == memcmp((airupdate_cntx.bt_addr), addr, BT_BD_ADDR_LEN))) {
        airupdate_rho_data_t *rho_data = (airupdate_rho_data_t *)data;

        rho_data->conn_handle = airupdate_cntx.conn_handle;
        rho_data->ready_to_send = airupdate_cntx.ready_to_send;
        rho_data->max_packet_size = airupdate_cntx.max_packet_size;
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}


static void airupdate_rho_status_cb(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    switch (event) {
        case BT_ROLE_HANDOVER_COMPLETE_IND: {
            if ((BT_AWS_MCE_ROLE_AGENT == role) && (BT_STATUS_SUCCESS == status)) {
                airupdate_clear_node_list();
                memset(&airupdate_cntx, 0, sizeof(airupdate_cntx_t));
            }

        }
            break;

        default:
            break;
    }
}

static bt_status_t airupdate_rho_update_cb(bt_role_handover_update_info_t *info)
{
    if (info && (BT_AWS_MCE_ROLE_PARTNER == info->role)) {
        if ((info->length > 0) && (info->data)) {//copy data to context
            airupdate_rho_data_t *rho_data = (airupdate_rho_data_t *)info->data;

            memcpy(airupdate_cntx.bt_addr, info->addr, BT_BD_ADDR_LEN);
            airupdate_cntx.connected = true;
            airupdate_cntx.conn_handle = rho_data->conn_handle;
            airupdate_cntx.ready_to_send = rho_data->ready_to_send;
            airupdate_cntx.max_packet_size = rho_data->max_packet_size;

        } else {
            //error log
            return BT_STATUS_FAIL;
        }
    }
    return BT_STATUS_SUCCESS;
}

bt_role_handover_callbacks_t airupdate_rho_callbacks = {
    .allowed_cb = airupdate_rho_allowed_cb,/*optional if always allowed*/
    .get_len_cb = airupdate_rho_get_data_length_cb,  /*optional if no RHO data to partner*/
    .get_data_cb = airupdate_rho_get_data_cb,   /*optional if no RHO data to partner*/
    .update_cb = airupdate_rho_update_cb,       /*optional if no RHO data to partner*/
    .status_cb = airupdate_rho_status_cb, /*Mandatory for all users.*/
};

#endif /*__MTK_AWS_MCE_ENABLE__ */

static void airupdate_event_callback(airupdate_event_t event_id, void *param)
{
    uint8_t i = 0;

    for (i = 0; i < AIRUPDATE_SUPPORT_CB_MAX_NUM; i++) {
        if ((airupdate_cb_list[i].in_use) 
            && (NULL != airupdate_cb_list[i].callback)) {
            airupdate_cb_list[i].callback(event_id, param);
        }
    }      
}
    
static bt_status_t airupdate_event_callback_int(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    switch (msg) {
        case BT_AIRUPDATE_CONNECTED_IND: {/* transport connection is established. */
            bt_airupdate_connected_ind_t *conn_ind_p = (bt_airupdate_connected_ind_t *)buff;
            //if (true == airupdate_cntx.connected) {//Only support the first connection
	      //  LOG_MSGID_E(AIRUPDATE, "Airupdate can only support the 1st link, and the link has existed!\r\n", 0); 
		//return BT_STATUS_FAIL;
	    //}
            LOG_MSGID_I(AIRUPDATE, "BT_AIRUPDATE_CONNECTED_IND, handle: 0x%4x, status 0x%4x\r\n", 2, conn_ind_p->handle, status);          
            airupdate_cntx.connected = true;
            airupdate_cntx.conn_handle = conn_ind_p->handle;
            airupdate_cntx.max_packet_size = conn_ind_p->max_packet_length - AIRUPDATE_HEADER_LEN;
            memcpy(airupdate_cntx.bt_addr, conn_ind_p->address, BT_BD_ADDR_LEN);
            if (BT_STATUS_SUCCESS == airupdate_check_user()) {
                airupdate_connect_ind_t connect_param;
                memset(&connect_param, 0x0, sizeof(airupdate_connect_ind_t));
                connect_param.handle = airupdate_cntx.conn_handle;
                connect_param.max_packet_length = airupdate_cntx.max_packet_size;
                memcpy(&connect_param.address, &airupdate_cntx.bt_addr, BT_BD_ADDR_LEN);                        
                airupdate_event_callback(AIRUPDATE_CONNECT_IND, (void *)&connect_param);
            }
        }
        break;
                
        case BT_AIRUPDATE_DISCONNECTED_IND: {
            bt_airupdate_disconnected_ind_t *disc_ind_p = (bt_airupdate_disconnected_ind_t *)buff;
            if ((true == airupdate_cntx.connected) && (airupdate_cntx.conn_handle == disc_ind_p->handle)) { 
                LOG_MSGID_I(AIRUPDATE, "BT_AIRUPDATE_DISCONNECTED_IND, handle: 0x%4x, reason:0x%4x\r\n", 2, disc_ind_p->handle,disc_ind_p->reason); 
                if (BT_STATUS_SUCCESS == airupdate_check_user()) {
                    airupdate_disconnect_ind_t disconnect_param;
                    memset(&disconnect_param, 0x0, sizeof(airupdate_disconnect_ind_t));
                    disconnect_param.handle = airupdate_cntx.conn_handle;   
                    disconnect_param.reason = disc_ind_p->reason;
                    airupdate_event_callback(AIRUPDATE_DISCONNECT_IND, (void *)&disconnect_param);
                }
                airupdate_clear_node_list();
                memset(&airupdate_cntx, 0, sizeof(airupdate_cntx_t));     
            } else {                 
                LOG_MSGID_I(AIRUPDATE, "BT_AIRUPDATE_DISCONNECTED_IND, Wrong conn handle or not find the existed connection!\r\n", 0); 
            }
        }
        break;

        case BT_AIRUPDATE_DATA_RECIEVED_IND: {
            bt_airupdate_data_recieved_ind_t *data_ind_p = (bt_airupdate_data_recieved_ind_t *)buff;
            if ((true == airupdate_cntx.connected) && (airupdate_cntx.conn_handle == data_ind_p->handle)) {
                if (BT_STATUS_SUCCESS == airupdate_check_user()) {
                    //add parse function
                    LOG_MSGID_I(AIRUPDATE, "BT_AIRUPDATE_DATA_RECIEVED_IND, handle: 0x%4x, dataLen is %d\r\n", 2, data_ind_p->handle, data_ind_p->packet_length); 
                    airupdate_data_received_ind_t recieve_param;
                    memset(&recieve_param, 0x0, sizeof(airupdate_data_received_ind_t));
                    recieve_param.handle = airupdate_cntx.conn_handle;
                    recieve_param.packet_length = data_ind_p->packet_length - AIRUPDATE_HEADER_LEN;
                    bt_airupdate_hold_data(data_ind_p->packet);
                    airupdate_add_node(airupdate_get_head(), data_ind_p->packet, data_ind_p->packet_length);  
                    airupdate_event_callback(AIRUPDATE_RECIEVED_DATA_IND, (void *)&recieve_param);
                }
            } else {
                LOG_MSGID_I(AIRUPDATE, "BT_AIRUPDATE_DATA_RECIEVED_IND, Wrong Airupdate handle or not find the existed connection!\r\n", 0); 
            }
        }
        break;
        
        case BT_MEMORY_TX_BUFFER_AVAILABLE_IND: {    
            bt_memory_tx_buffer_available_ind_t *ready_send_p = (bt_memory_tx_buffer_available_ind_t *)buff;
            if ((true == airupdate_cntx.connected) && (airupdate_cntx.conn_handle != AIRUPDATE_INVALID_HANDLE)) {
                if ((true == airupdate_cntx.ready_to_send) && 
                    (BT_STATUS_SUCCESS == airupdate_check_user())) {
                    LOG_MSGID_I(AIRUPDATE, "BT_MEMORY_TX_BUFFER_AVAILABLE_IND, tx available length is %d\r\n", 1, ready_send_p->size); 
                    airupdate_ready_to_send_ind_t ready_send;
                    memset(&ready_send, 0x0, sizeof(airupdate_ready_to_send_ind_t));
                    ready_send.handle = airupdate_cntx.conn_handle; 
                    airupdate_cntx.ready_to_send = false;
                    airupdate_event_callback(AIRUPDATE_READY_TO_SEND_IND, (void *)&ready_send);
                }
            }   
        }
        break;

        default:
            break;
    } 
    return BT_STATUS_SUCCESS;
}

static bt_status_t airupdate_check_user(void)
{
    uint8_t i = 0;
    bt_status_t status = 0;

    for (i = 0; i < AIRUPDATE_SUPPORT_CB_MAX_NUM; i++) {
        if (airupdate_cb_list[i].in_use) {
            LOG_MSGID_I(AIRUPDATE, "airupdate_check_user existed!\r\n", 0);
            return status;
        }
    }
    if (i == AIRUPDATE_SUPPORT_CB_MAX_NUM) {
        LOG_MSGID_I(AIRUPDATE, "not find any users existed!\r\n", 0);
        status = AIRUPDATE_STATUS_FAIL;
    }             
    return status;
}

static void airupdate_connection_status_notify(airupdate_notify_callback callback)
{
    if (true == airupdate_cntx.connected) {
        airupdate_connect_ind_t connect_param;
        memset(&connect_param, 0x0, sizeof(airupdate_connect_ind_t));
        connect_param.handle = airupdate_cntx.conn_handle;
        connect_param.max_packet_length = airupdate_cntx.max_packet_size;
        memcpy(&connect_param.address, &airupdate_cntx.bt_addr, BT_BD_ADDR_LEN);            
        callback(AIRUPDATE_CONNECT_IND, (void *)&connect_param);
    }
    return ;
}

void airupdate_main(void)
{
    airupdate_status_t result = 0;
    memset(&airupdate_cntx, 0, sizeof(airupdate_cntx_t));

    /* mutex init */
    if (airupdate_mutex == 0){
        airupdate_mutex = airupdate_create_mutex();
    }

    g_airupdate_head = airupdate_create_list();
    if (NULL == g_airupdate_head) {
        LOG_MSGID_I(AIRUPDATE, "airupdate_create_list fail, because of no memory\r\n", 0);
    }
    result = bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_MM | MODULE_MASK_AIRUPDATE, (void*)airupdate_event_callback_int);
#if defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE)
    result += bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_AIRUPDATE, &airupdate_rho_callbacks);
#endif

    LOG_MSGID_I(AIRUPDATE, "airupdate_main, register CB result: 0x%4x\r\n", 1, result); 
}

airupdate_status_t airupdate_init(airupdate_notify_callback callback)
{
    uint8_t i = 0;
    airupdate_status_t status = 0;
    LOG_MSGID_I(AIRUPDATE, "airupdate_init callback %x\r\n", 1, callback);

    for (i = 0; i < AIRUPDATE_SUPPORT_CB_MAX_NUM; i++) {
        if (!(airupdate_cb_list[i].in_use)) {
            airupdate_cb_list[i].callback = callback;
            airupdate_cb_list[i].in_use = true;
            airupdate_connection_status_notify(callback);
            break;
        }
    }
    if (i == AIRUPDATE_SUPPORT_CB_MAX_NUM) {
        
        LOG_MSGID_I(AIRUPDATE, "all are in use, please extend the value of AIRUPDATE_SUPPORT_CB_MAX_NUM!\r\n", 0);
        status = AIRUPDATE_STATUS_FAIL;
    }             
    return status;
}

airupdate_status_t airupdate_deinit(airupdate_notify_callback callback)
{
    uint8_t i = 0;
    airupdate_status_t status = 0;

    LOG_MSGID_I(AIRUPDATE, "airupdate_deinit callback %x\r\n", 1, callback);

    for (i = 0; i < AIRUPDATE_SUPPORT_CB_MAX_NUM; i++) {
        if (airupdate_cb_list[i].in_use && airupdate_cb_list[i].callback == callback) {
            airupdate_cb_list[i].callback = NULL;
            airupdate_cb_list[i].in_use = false;
            break;
        }
    }
    if (i == AIRUPDATE_SUPPORT_CB_MAX_NUM) {
        LOG_MSGID_I(AIRUPDATE, "airupdate_deinit delete fail, because of not find the callback\r\n", 0);
        status = AIRUPDATE_STATUS_FAIL;
    }  

    if ((AIRUPDATE_STATUS_FAIL == airupdate_check_user()) && (true == airupdate_cntx.connected)) {
        if (true == airupdate_cntx.ready_to_send) {
            airupdate_cntx.ready_to_send = false;
        }
        airupdate_clear_node_list();
    }
    return status;
}

static uint16_t airupdate_fill_packet(uint8_t *dst_packet, uint8_t* data, uint16_t data_size)
{
    uint8_t *ptr = dst_packet;
    uint16_t send_len = data_size + AIRUPDATE_HEADER_LEN;
    uint16_t Newlen = data_size + AIRUPDATE_OGF_OCF_LEN;
    if (NULL == ptr) {
        return 0;
    }
    
    *(ptr)   = 0x0F;
    *(ptr + 1) = 0x00;
    *(ptr + AIRUPDATE_LEN_OFFSET)   = Newlen >> 8;
    *(ptr + AIRUPDATE_LEN_OFFSET + 1) = Newlen;
    *(ptr + AIRUPDATE_OGF_OFFSET)   = AIRUPDATE_OGF_RACE;
    *(ptr + AIRUPDATE_OCF_OFFSET)   = AIRUPDATE_OCF_RSP;

    LOG_MSGID_I(AIRUPDATE, "filled packet size, NewLen L8bits is 0x%x, H is 0x%x\r\n", 2, *(ptr + AIRUPDATE_LEN_OFFSET), *(ptr + AIRUPDATE_LEN_OFFSET + 1)); 
    memcpy((dst_packet + AIRUPDATE_HEADER_LEN), data, data_size);
    return send_len;
}

uint32_t airupdate_write_data(uint32_t handle, uint8_t *data, uint16_t data_size)
{
    bt_status_t status;
    uint32_t send_size = 0;
    
    if ((AIRUPDATE_INVALID_HANDLE != handle) && (handle == airupdate_cntx.conn_handle)) {
        if (airupdate_cntx.max_packet_size < data_size) {
            send_size = airupdate_cntx.max_packet_size;
        } else {
            send_size = data_size;
        }
        
        uint8_t *air_packet = (uint8_t *)pvPortMalloc(send_size + AIRUPDATE_HEADER_LEN);        
        uint16_t send_len = airupdate_fill_packet(air_packet, data, send_size);
        status = bt_airupdate_send_data(handle, air_packet, send_len);
        if (air_packet) 
            vPortFree(air_packet);
        
        if (BT_STATUS_SUCCESS == status) {
            LOG_MSGID_I(AIRUPDATE, "airupdate_write_data: send_size[%d]\r\n", 1, send_size);
            return send_size;
        } else if (BT_STATUS_OUT_OF_MEMORY == status) {
            LOG_MSGID_I(AIRUPDATE, "airupdate_write_data: OOM!\r\n", 0);
            airupdate_cntx.ready_to_send = true;
        }
    }
    
    LOG_MSGID_I(AIRUPDATE, "airupdate_send_data, fail!\r\n", 0);  
    return 0;
}

uint32_t airupdate_read_data(uint32_t handle, uint8_t *data, uint16_t data_size)
{   
    LOG_MSGID_I(AIRUPDATE, "airupdate_read_data, handle:0x%4x, data: 0x%4x, data_size: %d\r\n", 3, handle, data, data_size); 
    
    if ((handle != AIRUPDATE_INVALID_HANDLE) && (handle == airupdate_cntx.conn_handle)) {
        return airupdate_check_node_buffer(data, data_size);
    } 
    return 0;
}

uint32_t airupdate_get_rx_available(uint32_t handle)
{
    airupdate_take_mutex(airupdate_mutex);
    uint32_t rx_remain_len = 0;

    if ((handle != AIRUPDATE_INVALID_HANDLE) && (handle == airupdate_cntx.conn_handle)) {
        uint32_t count = airupdate_get_node_length(g_airupdate_head);

        if (0 != airupdate_get_node_length(g_airupdate_head)) {//list is null?
            airupdate_node_t *node = airupdate_find_node_by_index(g_airupdate_head, count);
            rx_remain_len = (node->packet_length - AIRUPDATE_HEADER_LEN);
            airupdate_give_mutex(airupdate_mutex);
            return rx_remain_len;
        }
    } 
    airupdate_give_mutex(airupdate_mutex);
    return 0;
}


//********************************************************Node Function****************************************************************************//

/**
 * @brief          This function is for create a list.
 * @param[in]  void.
 * @return       the head of the list.
 */
airupdate_node_t *airupdate_create_list(void)
{
    airupdate_node_t *head;
    //head = (airupdate_node_t *)pvPortMalloc(sizeof(airupdate_node_t));
    head = &g_airupdate_list;
    if (head) {
        head->packet = NULL;
        head->packet_length = 0;
        head->next = NULL;
    }
    return head;
}

/**
 * @brief          This function is for add a node into list.
 * @param[in]  head      is the head of the list.
 * @param[in]  packet   is the data point.
 * @param[in]  packet_length       is the data length.
 * @return       void.
 */

void airupdate_add_node(airupdate_node_t *head, uint8_t *packet, uint16_t packet_length) 
{//insert start
    airupdate_take_mutex(airupdate_mutex);

    airupdate_node_t *p = (airupdate_node_t*)pvPortMalloc(sizeof(airupdate_node_t));
    if (p) {
        p->packet = packet;
        p->packet_length = packet_length;
        p->next = head->next;
        head->next = p;
    }
    airupdate_give_mutex(airupdate_mutex);
}

/**
 * @brief          This function is for get the list length.
 * @param[in]  head      is the head of the list.
 * @return       the length of the list.
 */

uint32_t airupdate_get_node_length(airupdate_node_t *head)//get list length
{
    //airupdate_take_mutex(airupdate_mutex);

    uint32_t n=0;
    airupdate_node_t *p;
    p=head->next;
    while(p)
    {
       n++;
       p=p->next;
    }
    
    //airupdate_give_mutex(airupdate_mutex);
    return n;
}

/**
 * @brief          This function is for delete a node into list.
 * @param[in]  head      is the head of the list.
 * @param[in]  packet   is the data point.
 * @param[in]  packet_length       is the data length.
 * @return       delete success or not.
 */

uint32_t airupdate_delete_node(airupdate_node_t * head, uint8_t *packet, uint16_t packet_length) 
{
    //airupdate_take_mutex(airupdate_mutex);

    airupdate_node_t * p;
    airupdate_node_t * q;
    for(p = head; ((NULL != p) && (NULL != p->next)); p = p->next) {
        if(p->next->packet == packet) {
            q = p->next;
            p->next = q->next;
            LOG_MSGID_I(AIRUPDATE, "airupdate_delete_node, node: 0x%4x, packet: 0x%4x, pak_len: %d\r\n", 3, q, q->packet, q->packet_length); 
            vPortFree(q);
            //airupdate_give_mutex(airupdate_mutex);
            return 1; 
        }
    }
    
    //airupdate_give_mutex(airupdate_mutex);
    return 0; 
}

/**
 * @brief          This function is for find a node into list.
 * @param[in]  head      is the head of the list.
 * @param[in]  packet   is the data point.
 * @param[in]  packet_length       is the data length.
 * @return       the exit node or not.
 */

uint32_t airupdate_find_node(airupdate_node_t *head, uint8_t *packet, uint16_t packet_length) 
{
    airupdate_take_mutex(airupdate_mutex);

    airupdate_node_t *p = head->next;
    while (p) {
        if(p->packet == packet) {
            airupdate_give_mutex(airupdate_mutex);
            return 1; 
        }
        p = p->next;
    }
    airupdate_give_mutex(airupdate_mutex);
    return 0; 
}

/**
 * @brief          This function is for add a node by the index of the list.
 * @param[in]  head      is the head of the list.
 * @param[in]  packet   is the data point.
 * @param[in]  packet_length       is the data length.
 * @return       the node.
 */

airupdate_node_t *airupdate_find_node_by_index(airupdate_node_t * head, int index)
{
    //airupdate_take_mutex(airupdate_mutex);

    int i;
    airupdate_node_t * p;
    p = head->next;
    for(i = 0; i < (index - 1); i++) {  
        if (NULL == p) {
            break;
        }
                 p = p->next;
    }

    if (NULL == p) {
        //airupdate_give_mutex(airupdate_mutex);
        
        LOG_MSGID_I(AIRUPDATE, "airupdate_find_node_by_index, node not find!\r\n", 0); 
        return NULL;
    }
    
    LOG_MSGID_I(AIRUPDATE, "airupdate_find_node_by_index, find node: 0x%4x, packet: 0x%4x, pak_len: %d\r\n", 3, p, p->packet, p->packet_length); 
    //airupdate_give_mutex(airupdate_mutex);
    return p;
}


static airupdate_node_t *airupdate_get_head(void)
{
    return g_airupdate_head;
}

static uint32_t airupdate_parse_packet(uint8_t *dst_data, uint8_t *packet, uint16_t packet_size)
{
    uint8_t* pDataPayload = packet;

    if (pDataPayload == NULL)
        return 0;


    LOG_MSGID_I(AIRUPDATE, "Airupdate header, 0x%x 0x%x 0x%x 0x%x 0x%x\r\n", 5, *(pDataPayload),*(pDataPayload+1),*(pDataPayload+2),*(pDataPayload+3),*(pDataPayload+4)); 

    if( *(pDataPayload + AIRUPDATE_OGF_OFFSET)== AIRUPDATE_OGF_RACE &&
        *(pDataPayload + AIRUPDATE_OCF_OFFSET)== AIRUPDATE_OCF_CMD     ) {
        uint16_t Newlen = AIRUPDATE_U16_LEN(pDataPayload + AIRUPDATE_LEN_OFFSET) - AIRUPDATE_OGF_OCF_LEN;
        LOG_MSGID_I(AIRUPDATE, "New Length, 0x%x\r\n", 1, Newlen); 

        memcpy(dst_data, (uint8_t*)(pDataPayload + AIRUPDATE_HEADER_LEN), (packet_size - AIRUPDATE_HEADER_LEN));
        return Newlen;
    } else {
        return 0;
    }

}

uint32_t airupdate_check_node_buffer(uint8_t *data, uint16_t data_size)
{
    airupdate_take_mutex(airupdate_mutex);

    uint32_t r_size = 0;
    uint32_t count = airupdate_get_node_length(g_airupdate_head);
    
    LOG_MSGID_I(AIRUPDATE, "airupdate_check_node_buffer, count: %d\r\n", 1, count); 
    if (count != 0 ) {
        uint8_t *pak;
        uint16_t pak_len;
        
        airupdate_node_t *node = airupdate_find_node_by_index(g_airupdate_head, count);
        if (node) {
            if (data_size > node->packet_length) {
                r_size = airupdate_parse_packet(data, node->packet, node->packet_length);
            } else {
                configASSERT(0 && "enlarge the read buffer");
            }

            pak = node->packet;
            pak_len = node->packet_length;
                
            LOG_MSGID_I(AIRUPDATE, "airupdate_check_node_buffer, pak: 0x%4x, pak_len: %d\r\n", 2, pak, pak_len); 
            airupdate_delete_node(g_airupdate_head, node->packet, node->packet_length); //deleteElem must before bt_airupdate_release_data, or the sequence of the packets will be wrong
            if (pak || (pak_len != 0)) {
                bt_airupdate_release_data(pak);
            }
        }
    }
    airupdate_give_mutex(airupdate_mutex);
    return r_size;
}

void airupdate_clear_node_list()
{
    airupdate_take_mutex(airupdate_mutex);

    uint32_t count = airupdate_get_node_length(g_airupdate_head);
    while(count > 0){
        uint8_t *pak;
        uint16_t pak_len;
        
        airupdate_node_t *node = airupdate_find_node_by_index(g_airupdate_head, count);
        pak = node->packet;
        pak_len = node->packet_length;
        airupdate_delete_node(g_airupdate_head, node->packet, node->packet_length);
        
        if (pak || (pak_len != 0)) {
            bt_airupdate_release_data(pak);
        }
        count --;
    }
    
    airupdate_give_mutex(airupdate_mutex);
}


#endif //MTK_AIRUPDATE_ENABLE

#endif //MTK_PORT_SERVICE_BT_ENABLE



