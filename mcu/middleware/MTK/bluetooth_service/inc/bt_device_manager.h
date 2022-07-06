/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __BT_DEVICE_MANAGER_H__
#define __BT_DEVICE_MANAGER_H__

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothService_EDR_DM Device Manager
 * @{
 * This section provides APIs to get Bluetooth device information and handle the bonding procedures.
 *
 * @section device_manager_api_usage How to use this module
 * - Step 1. Initialize the device manager at boot up.
 *  - Sample Code:
 *     @code
 *                bt_device_manager_init();
 *     @endcode
 * - Step 2. Use device manager APIs to access the paired device list.
 *  - Sample code:
 *     @code
 *               uint32_t read_number = 5;
 *               uint32_t i;
 *               bt_device_manager_paired_infomation_t paired_devices[5];
 *               bt_device_manager_get_paired_list(paired_devices, &read_number);
 *               for(i = 0; i < read_number; i++)
 *               {
 *                   printf("paired index: %d, device name: %s", i, paired_devices->name);
 *               }
 *     @endcode
 * - Step 3. Delete a paired device.
 *  - Sample code:
 *     @code
 *               bt_bd_addr_t address = {0x00, 0x33, 0x63, 0x56, 0xE0, 0x4C};
 *               if (bt_device_manager_is_paired((bt_bd_addr_ptr_t)address)) {
 *                   bt_device_manager_delete_paired_device((bt_bd_addr_ptr_t) address);
 *               }
 *     @endcode
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "bt_type.h"
#include "bt_gap.h"
#include "bt_aws_mce.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @defgroup BT_device_manager_Service_define Define
 * @{
 * Define Bluetooth device manager service data types and values.
 */

#define BT_DEVICE_MANAGER_PAIRED_ADD            (0x00)  /**< This event is generated after adding a new paired device. */
#define BT_DEVICE_MANAGER_PAIRED_UNPAIR         (0x01)  /**< This event is generated after removing a paired device through #bt_device_manager_delete_paired_device(). */
#define BT_DEVICE_MANAGER_PAIRED_KEY_MISS       (0x02)  /**<This event is generated after removing a paired device because the information of the paired device has expired. */
#define BT_DEVICE_MANAGER_PAIRED_LIST_FULL      (0x03)  /**< This event is generated after removing a paired device in order to pair with a new device, because the paired device list is full. */
#define BT_DEVICE_MANAGER_PAIRED_UNPAIR_ALL     (0x04)  /**<This event is generated after removing all paired devices through #bt_device_manager_unpair_all(). */
typedef uint8_t bt_device_manager_paired_event_t;   /**< The event type of paired information has changed. */

#define BT_DEVICE_MANAGER_DB_FLUSH_BLOCK        (0x00)  /**< Indicate the data base block flush to nvdm. */
#define BT_DEVICE_MANAGER_DB_FLUSH_NON_BLOCK    (0x01)  /**< Indicate the data base non-block flush to nvdm. */
typedef uint8_t bt_device_manager_db_flush_t;       /**< The method that data base flush to nvdm . */

#define BT_DEVICE_MANAGER_NORMAL_MODE             0x00     /**< This mode is the default mode when the BT powers on. */
#define BT_DEVICE_MANAGER_TEST_MODE               0x01     /**< This mode will disable the bt sdk power manager strategy. */
typedef uint8_t bt_device_manager_mode_type_t;       /**< The device mode. */

/**
 * @}
 */

/**
 * @defgroup BT_device_manager_Service_struct Struct
 * @{
 * This section defines structures for the Bluetooth device manager service.
 */

/**
 *  @brief This structure defines the bt controller version information about the paired device.
 */
typedef struct {
    uint8_t     version;            /**< The version of the LMP in the remote controller. Please refer to <a href="https://www.bluetooth.com/specifications/assigned-numbers/link-manager">The Link Manager Version parameter field</a>.*/
    uint16_t    manufacturer_id;    /**< The manufacturer id of the remote controller. Please refer to <a href="https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers">Bluetooth Assigned Numbers</a>. */
    uint16_t    subversion;         /**< The subversion of LMP in the remote controller. */
} bt_device_manager_paired_controller_version_t;

/**
 *  @brief This structure defines the information about the paired device.
 */
typedef struct {
    bt_bd_addr_t                          address;                             /**< Address of the paired device. */
    char                                  name[BT_GAP_MAX_DEVICE_NAME_LENGTH]; /**< Name string of the paired device. */
} bt_device_manager_paired_infomation_t;

/**
 *  @brief This structure defines the complete information about the paired device.
 */
typedef struct {
    bt_gap_link_key_notification_ind_t paired_key;  /**< Link key of the paired device. */
    char name[BT_GAP_MAX_DEVICE_NAME_LENGTH + 1];   /**< Name string of the paired device. */
} bt_device_manager_paired_device_complete_infomation_t;

/**
 * @}
 */

/**
 * @brief     This function initializes the Bluetooth device manager. It's recommended to call this function at the boot up.
 * @return    None.
 */
void bt_device_manager_init(void);

/**
 * @brief      This function gets the number of the paired devices.
 * @return     The number of the paired devices.
 */
uint32_t bt_device_manager_get_paired_number(void);

/**
 * @brief         This function gets the paired device list.
 * @param[out]    info        is a list containing the information of paired devices.
 * @param[in, out] read_number  specifies the length of the info list when calling this function.
 *                              Upon the return of this function call, the read_number contains the actual number of paired devices in the info list.
 * @return        None.
 */
void bt_device_manager_get_paired_list(bt_device_manager_paired_infomation_t* info, uint32_t* read_number);

/**
 * @brief         This function gets the status of a device.
 * @param[in]     address     is the address of the device.
 * @return        true, if the device is paired.
 *                false, if the device is not paired.
 */
bool bt_device_manager_is_paired(bt_bd_addr_ptr_t address);

/**
 * @brief         This function unpairs an already paired device. If the specified device is unpaired, the return status is #BT_STATUS_FAIL.
 * @param[in]     address     is the address of the already paired device.
 * @return        #BT_STATUS_SUCCESS, if the device is unpaired successfully.
 *                #BT_STATUS_FAIL, if an error occurred.
 */
bt_status_t bt_device_manager_delete_paired_device(bt_bd_addr_ptr_t address);

/**
 * @brief         This function unpairs all paired devices.
 * @return        #BT_STATUS_SUCCESS, if the function executed successfully.
 *                #BT_STATUS_FAIL, if an error occurred.
 */
bt_status_t bt_device_manager_unpair_all(void);

/**
 * @brief         This function gets the complete paired device list.
 * @param[out]    info        is a list containing the information of paired devices.
 * @param[in, out] read_count  specifies the length of the info list when calling this function.
 * @return        Actual number of paired devices in the info list
 */
uint32_t bt_device_manager_get_complete_paired_list(bt_device_manager_paired_device_complete_infomation_t* info, uint32_t read_count);

/**
 * @brief         This function gets the complete pair info of a device.
 * @param[in]     address     is the address of the device.
 * @param[out]   info       is the complete infomation of the device.
 * @return          #BT_STATUS_SUCCESS, if the device exist.
 *                #BT_STATUS_FAIL, if an error occurred.
 */
bt_status_t bt_device_manager_get_complete_paired_info(bt_bd_addr_ptr_t address, bt_device_manager_paired_device_complete_infomation_t *info);

/**
 * @brief         This function gets the link key of a device.
 * @param[in]     address     is the address of the device.
 * @param[in]     info      is the complete infomation of the device.
 * @return          #BT_STATUS_SUCCESS, if the device exist.
 *                #BT_STATUS_FAIL, if an error occurred.
 */
bt_status_t bt_device_manager_set_complete_paired_info(bt_bd_addr_ptr_t address, bt_device_manager_paired_device_complete_infomation_t *info);

#ifdef __MTK_AVM_DIRECT__
void bt_device_manager_set_io_capability(bt_gap_io_capability_t io_capability);
#endif

/**
 * @brief         This function sets the local bluetooth address.
 * @param[in]     address     is the address of the device.
 * @return         None.
 */
void bt_device_manager_store_local_address(bt_bd_addr_t *address);

/**
 * @brief         This function gets the local bluetooth address.
 * @return     The bluetooth local address.
 */
bt_bd_addr_t* bt_device_manager_get_local_address(void);

/**
 * @brief         This function used to flush all data base managed by device manager.
 * @param[in]     block    is device manager data base flush type.
 * @return          None.
 */
void bt_device_manager_db_flush_all(bt_device_manager_db_flush_t block);

/**
 * @brief         This function used to get the paired devices number.
 * @return          The paired device number..
 */
uint32_t bt_device_manager_remote_get_paired_num(void);

/**
 * @brief         This function used to set the indicate device to the first postion in the paired list.
 * @param[in]     addr     is the address of the remote device.
 * @return          #BT_STATUS_SUCCESS, excute success.
 *                #BT_STATUS_FAIL, if an error occurred.
 */
bt_status_t bt_device_manager_remote_top(bt_bd_addr_t addr);

/**
 * @brief         This function used to set the indicate device's sequence in the paired list.
 * @param[in]     addr     is the address of the remote device.
 * @param[in]     sequence     is the sequence number.
 * @return          #BT_STATUS_SUCCESS, excute success.
 *                #BT_STATUS_FAIL, if an error occurred.
 */
bt_status_t bt_device_manager_remote_set_seq_num(bt_bd_addr_t addr, uint8_t sequence);

/**
 * @brief         This function used to get the remote device's address by sequence number in paired list.
 * @param[in]     sequence     is the sequence number in the paired list.
 * @return         The remote device's address.
 */
bt_bd_addr_t *bt_device_manager_remote_get_dev_by_seq_num(uint8_t sequence);

/**
 * @brief         This function used to get the aws mce fixed address.
 * @return         The aws mce fixed address.
 */
bt_bd_addr_t* bt_device_manager_aws_local_info_get_fixed_address(void);

/**
 * @brief         This function used to store the aws mce locl address.
 * @param[in]     addr    is the aws mce local address.
 * @return         None.
 */
void bt_device_manager_aws_local_info_store_local_address(bt_bd_addr_t *addr);

/**
 * @brief         This function used to get the aws mce local address.
 * @return         The aws mce local address.
 */
bt_bd_addr_t* bt_device_manager_aws_local_info_get_local_address(void);

/**
 * @brief         This function store the aws mce peer address.
 * @param[in]     addr     is the aws mce peer address.
 * @return         None.
 */
void bt_device_manager_aws_local_info_store_peer_address(bt_bd_addr_t *addr);

/**
 * @brief         This function used to get the aws mce peer address
 * @return         The aws mce peer address.
 */
bt_bd_addr_t* bt_device_manager_aws_local_info_get_peer_address(void);

/**
 * @brief         This function used to store the aws mce role.
 * @param[in]     aws_role is the role of aws mce feature.
 * @return         None.
 */
void bt_device_manager_aws_local_info_store_role(bt_aws_mce_role_t aws_role);

/**
 * @brief         This function used to get the aws mce role.
 * @return         The aws mce role.
 */
bt_aws_mce_role_t bt_device_manager_aws_local_info_get_role(void);

/**
 * @deprecated    This function is deprecated.
 * @brief         This function used to get the bluetooth device mode.
 * @return         The bluetooth device mode.
 */
bt_device_manager_mode_type_t bt_device_manager_get_device_mode(void);

/**
 * @}
 */
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __BT_DEVICE_MANAGER_H__ */

