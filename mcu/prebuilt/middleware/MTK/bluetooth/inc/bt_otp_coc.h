/*
* (C) 2021  Airoha Technology Corp. All rights reserved.
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

#ifndef __BT_OTP_COC_H__
#define __BT_OTP_COC_H__

/**
 *  @brief Defines the OTP COC event.
 */
#define BT_OTP_COC_CONNECTED_IND                       0   /**< This event indicates the connection oriented channel connected #bt_otp_coc_connected_ind_t. */
#define BT_OTP_COC_CONNECTION_REQUEST_FAILED_IND       1   /**< This event indicates the connection oriented channel connection request failed #bt_otp_coc_conn_req_failed_ind_t. */
#define BT_OTP_COC_DISCONNECTED_IND                    2   /**< This event indicates the connection oriented channel disconnected #bt_otp_coc_disconnected_ind_t. */
#define BT_OTP_COC_DATA_IN_IND                         3   /**< This event indicates the connection oriented channel data in #bt_otp_coc_data_in_ind_t. */
typedef uint8_t bt_otp_coc_event_type_t;    /**< The type of OTP COC events.*/

/**
 *  @brief Defines the connection request failed reason.
 */
#define BT_OTP_COC_CONN_REQ_FAILED_DUE_TO_INVALID_RESPONSE_FROM_REMOTE          0x00    /**< Failed due to invalid response from remote.*/
#define BT_OTP_COC_CONN_REQ_REFUSED_BY_REMOTE_PSM_NOT_SUPPORT                   0x02    /**< Refused by remote PSM not support.*/
#define BT_OTP_COC_CONN_REQ_REFUSED_BY_REMOTE_INSUFFICIENT_AUTHENTICAIOTN       0x05    /**< Refused by remote Insufficient authentication.*/
#define BT_OTP_COC_CONN_REQ_REFUSED_BY_REMOTE_INSUFFICIENT_AUTHORIZATION        0x06    /**< Refused by remote Insufficient authorization.*/
#define BT_OTP_COC_CONN_REQ_REFUSED_BY_REMOTE_INSUFFICIENT_ENC_KEY_SIZE         0x07    /**< Refused by remote Insufficient encryption key size.*/
#define BT_OTP_COC_CONN_REQ_REFUSED_BY_REMOTE_INSUFFICIENT_ENC                  0x08    /**< Refused by remote Insufficient encryption.*/
#define BT_OTP_COC_CONN_REQ_REFUSED_BY_REMOTE_UNACCEPTABLE_PARAMETERS           0x0B    /**< Refused by remote unacceptable parameters.*/
typedef uint8_t bt_otp_coc_conn_req_failed_reason_t;    /**< The type of connection request failed reason.*/

/**
 *  @brief Defines the disconnected reason.
 */
#define BT_OTP_COC_DISCONNECTED_DUE_TO_LOCAL_REQUEST                    0x00    /**< Local request.*/
#define BT_OTP_COC_DISCONNECTED_DUE_TO_DUPLICATED_CID                   0x01    /**< Duplicated CID.*/
#define BT_OTP_COC_DISCONNECTED_DUE_TO_CREDIT_OVERFLOW                  0x02    /**< Credit overflow.*/
#define BT_OTP_COC_DISCONNECTED_DUE_TO_CREDIT_UNDERFLOW                 0x03    /**< Credit underflow.*/
#define BT_OTP_COC_DISCONNECTED_DUE_TO_INVALID_MTU_CONFIGURATION        0x04    /**< Invalid MTU configuration.*/
#define BT_OTP_COC_DISCONNECTED_DUE_TO_REMOTE_REQUEST                   0x05    /**< Remote request.*/
typedef uint8_t bt_otp_coc_disconnect_reason_t;     /**< The type of disconnected reason.*/

/**
 * @defgroup Bluetoothble_OTP_COC_struct Struct
 * @{
 * This section defines basic data structures for the OTP_COC.
 */

/**
 *  @brief This structure defines the parameter data type for event #BT_OTP_COC_CONNECTED_IND.
 */
typedef struct {
    bt_handle_t handle;         /**< Connection handle.*/
    uint16_t    remote_cid;     /**< Remote CID.*/
    uint8_t     is_active;      /**< Is active.*/
} bt_otp_coc_connected_ind_t;

/**
 *  @brief This structure defines the parameter data type for event #BT_OTP_COC_CONNECTION_REQUEST_FAILED_IND.
 */
typedef struct {
    bt_handle_t handle;                             /**< Connection handle.*/
    bt_otp_coc_conn_req_failed_reason_t reason;     /**< Connection request failed reason.*/
} bt_otp_coc_conn_req_failed_ind_t;

/**
 *  @brief This structure defines the parameter data type for event #BT_OTP_COC_DISCONNECTED_IND.
 */
typedef struct {
    bt_handle_t handle;         /**< Connection handle.*/
    uint16_t    remote_cid;     /**< Remote CID.*/
    bt_otp_coc_disconnect_reason_t  reason;     /**< Disconnected reason.*/
} bt_otp_coc_disconnected_ind_t;

/**
 *  @brief This structure defines the parameter data type for event #BT_OTP_COC_DATA_IN_IND.
 */
typedef struct {
    bt_handle_t handle;         /**< Connection handle.*/
    uint16_t    remote_cid;     /**< Remote CID.*/
    uint32_t    data_length;    /**< Data length.*/
    uint8_t     *p_data;        /**< Data.*/
} bt_otp_coc_data_in_ind_t;

typedef void (* bt_otp_coc_callback_t)  (bt_otp_coc_event_type_t event, const void *parameter);
/**
 * @}
 */

/**
 * @brief                       This function init object transfer profile COC.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] callback          is the callback.
 * @param[in] max_link          is the supported maximum COC link.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 *                              #BT_STATUS_CONNECTION_NOT_FOUND, the connection is not found.
 */
bt_status_t bt_otp_coc_init(bt_otp_coc_callback_t callback, uint32_t max_link);

/**
 * @brief                       This function connect COC.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 *                              #BT_STATUS_OUT_OF_MEMORY, the operation is out of memory.
 */
bt_status_t bt_otp_coc_connect(bt_handle_t handle);

/**
 * @brief                       This function disconnect COC.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] remote_cid        is the CID of remote.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_otp_coc_disconnect_channel(bt_handle_t handle, uint16_t remote_cid);

/**
 * @brief                       This function send data.
 * @param[in] handle            is the connection handle of the Bluetooth link.
 * @param[in] remote_cid        is the CID of remote.
 * @param[in] data              is the data.
 * @param[in] length            is the data length.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_otp_coc_send_data(bt_handle_t handle, uint16_t remote_cid, void *data, uint32_t length);

#endif  /*__BT_OTP_COC_H__ */

