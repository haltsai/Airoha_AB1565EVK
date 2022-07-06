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


#ifndef __BT_AWS_MCE_REPORT_H__
#define __BT_AWS_MCE_REPORT_H__

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothServices_Report_Service BT_AWS_MCE_REPORT
 * @{
 * This section defines the Bluetooth AWS_MCE_REPORT API to manage all users who need to send or receive report_info(0x85).
 * @{
 *
 *
 * @section bt_aws_mce_report_api_usage How to use this module
 *
 * - A user that meets any of the following conditions must register the bt_aws_mce_report module.
 *  - 1) It needs to send report_info(0x85) to the other device.
 *  - 2) It needs to receive report_info(0x85) from the other device.
 * - No matter whether the user needs send or receive, it should register the callback function.
 * - Example code:
 *  - 1. Add your own module type in this file.
 *     @code
 *     #define BT_AWS_MCE_REPORT_YOUR_MODULE        (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 6)
 *    @endcode
 *  -2. Register your callback function.
 *    @code
 *    void your_callback(bt_aws_mce_report_info_t *info)
 *    {
 *        uint32_t len = info->param_len;
 *        your_app_param_t *param = info->param;
 *        ...
 *    }
 *
 *    void your_app_init(void)
 *    {
 *        #if defined(MTK_AWS_MCE_ENABLE)
 *        bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_YOUR_MODULE, your_callback);
 *        #endif
 *    }
 *
 *    void your_app_deinit(void)
 *    {
 *        #if defined(MTK_AWS_MCE_ENABLE)
 *        bt_aws_mce_report_deregister_callback(BT_AWS_MCE_REPORT_YOUR_MODULE);
 *       #endif
 *    }
 *    @endcode
 *
 *  -3. Send your event or sync event (it must be done at the same time for both Agent and Partner).
 *    @code
 *    bt_status_t your_app_send_data( void * data, uint32_t len)
 *    {
 *        #if defined(MTK_AWS_MCE_ENABLE)
 *        bt_status_t status;
 *        bt_aws_mce_report_info_t info;
 *        info.param = (void *)malloc(len*sizeof(uint8_t));
 *        memcpy(info.param, data, len);
 *        info.module_id = BT_AWS_MCE_REPORT_YOUR_MODULE;
 *        info.is_sync = false;
 *        info.sync_time = 0;
 *        info.param_len = len;
 *        status= bt_aws_mce_report_send_event(&info);
 *        free(info.param);
 *        return status;
 *    }
 *
 *    bt_status_t your_app_send_sync_data( void * data, uint32_t len)
 *    {
 *        #if defined(MTK_AWS_MCE_ENABLE)
 *        bt_status_t status;
 *        bt_aws_mce_report_info_t info;
 *        info.param = (void *)malloc(len*sizeof(uint8_t));
 *        memcpy(info.param, data, len);
 *        info.module_id = BT_AWS_MCE_REPORT_YOUR_MODULE;
 *        info.is_sync = true;
 *        info.sync_time = 500; //uint:ms
 *        info.param_len = len;
 *        status= bt_aws_mce_report_send_sync_event(&info);
 *        free(info.param);
 *        return status;
 *    }
*    @endcode
* */




#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "bt_hci.h"
#include "bt_type.h"
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @defgroup BluetoothServices_AWS_MCE_Report_define Define
 * @{
 * Define the AWS_MCE module types.
 */

/**
 * @brief Define the module type to register.
 * For the middleware modules, please define the module type by using #BT_AWS_MCE_REPORT_MODULE_START and offset.
 * For the applications, please define the module type by using #BT_AWS_MCE_REPORT_MODULE_CUSTOM_START and offset.
 * The bt_aws_mce_report module calls back to these users according to the module id of the packet.
 */
#define BT_AWS_MCE_REPORT_MODULE_START          (0x30)                                       /**< The start value of the bt_aws_mce_report  module type. */
#define BT_AWS_MCE_REPORT_MODULE_RANGE          (0x10)                                       /**< The maximum number of bt_aws_mce_report modules that are supported. */
#define BT_AWS_MCE_REPORT_MODULE_CM             (BT_AWS_MCE_REPORT_MODULE_START)             /**< The module type of BT connection manager. */
#define BT_AWS_MCE_REPORT_MODULE_SINK_MUSIC     (BT_AWS_MCE_REPORT_MODULE_START + 1)         /**< The module type of sink music. */
#define BT_AWS_MCE_REPORT_MODULE_SINK_CALL      (BT_AWS_MCE_REPORT_MODULE_START + 2)         /**< The module type of sink call. */
#define BT_AWS_MCE_REPORT_MODULE_BT_AIR         (BT_AWS_MCE_REPORT_MODULE_START + 3)         /**< The module type of BT air. */
#define BT_AWS_MCE_REPORT_MODULE_FOTA           (BT_AWS_MCE_REPORT_MODULE_START + 4)         /**< The module type of FOTA. */
#define BT_AWS_MCE_REPORT_MODULE_TEST           (BT_AWS_MCE_REPORT_MODULE_START + 5)         /**< The module type for test. */
#define BT_AWS_MCE_REPORT_MODULE_RELAY_CMD      (BT_AWS_MCE_REPORT_MODULE_START + 6)         /**< The module type of race relay cmd. */
#define BT_AWS_MCE_REPORT_MODULE_HFP_AVC        (BT_AWS_MCE_REPORT_MODULE_START + 7)         /**< The module type of hfp avc. */
#define BT_AWS_MCE_REPORT_MODULE_GSOUND         (BT_AWS_MCE_REPORT_MODULE_START + 8)         /**< The module type of gsound. */
#define BT_AWS_MCE_REPORT_MODULE_DM             (BT_AWS_MCE_REPORT_MODULE_START + 9)         /**< The module type of bt device manager. */
#define BT_AWS_MCE_REPORT_MODULE_ANC            (BT_AWS_MCE_REPORT_MODULE_START + 10)        /**< The module type of anc. */
#define BT_AWS_MCE_REPORT_MODULE_PEQ            (BT_AWS_MCE_REPORT_MODULE_START + 11)        /**< The module type of PEQ. */
#define BT_AWS_MCE_REPORT_MODULE_MCSYNC_SHARE   (BT_AWS_MCE_REPORT_MODULE_START + 12)        /**< The module type of mcsync share. */
#define BT_AWS_MCE_REPORT_MODULE_ULL            (BT_AWS_MCE_REPORT_MODULE_START + 13)        /**< The module type of ULL. */


#define BT_AWS_MCE_REPORT_MODULE_CUSTOM_START   (BT_AWS_MCE_REPORT_MODULE_START + BT_AWS_MCE_REPORT_MODULE_RANGE )  /**< The start value of the customized bt_aws_mce_report module type. */
#define BT_AWS_MCE_REPORT_MODULE_CUSTOM_RANGE   (0x10)                                                              /**< The maximum number of customized bt_aws_mce_report module types that are supported. */
#define BT_AWS_MCE_REPORT_MODULE_VP             (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START)         /**< The module type of voice prompts. */
#define BT_AWS_MCE_REPORT_MODULE_LED            (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 1)     /**< The module type of LED. */
#define BT_AWS_MCE_REPORT_MODULE_BATTERY        (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 2)     /**< The module type of battery. */
#define BT_AWS_MCE_REPORT_MODULE_APP_ACTION     (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 3)     /**< The module type of app action. */
#define BT_AWS_MCE_REPORT_MODULE_BLE_APP        (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 4)     /**< The module type of ble app. */
#define BT_AWS_MCE_REPORT_MODULE_FAST_PAIR      (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 5)     /**< The module type of fast pair. */
#define BT_AWS_MCE_REPORT_MODULE_SMCHARGER      (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 6)     /**< The module type of smart charger. */
#define BT_AWS_MCE_REPORT_MODULE_IN_EAR         (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 7)     /**< The module type of in-ear detection. */
#define BT_AWS_MCE_REPORT_MODULE_AP             (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 8)     /**< The module type of ap earbuds configuration. */
#define BT_AWS_MCE_REPORT_MODULE_USR_TRIGR_FF   (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 9)     /**< The module type of User triggered adaptive ANC FF */
#define BT_AWS_MCE_REPORT_MODULE_LEAKAGE_DET    (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 10)    /**< The module type of leakage detection. */


/**
 * @brief Add AWS MCE VA xiaowei support
 *
 */
#define BT_AWS_MCE_REPORT_MODULE_VA_XIAOWEI     (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 11)     /**< The module type of VA xiaowei. */
#define BT_AWS_MCE_REPORT_MODULE_XIAOAI         (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 12)     /**< The module type of VA xiaoai. */
#define BT_AWS_MCE_REPORT_MODULE_APP_EMP        (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 13)     /**< The module type of APP EMP. */
#define BT_AWS_MCE_REPORT_MODULE_CHARGER_CASE   (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 14)     /**< The module type of SmartCharger Case. */
#define BT_AWS_MCE_REPORT_MODULE_MS_TEAMS       (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 15)     /**< The module type of MS TEAMS. */
#define BT_AWS_MCE_REPORT_MODULE_MS_CFU         (BT_AWS_MCE_REPORT_MODULE_CUSTOM_START + 16)     /**< The module type of MS CFU. */

#define BT_AWS_MCE_REPORT_MODULE_MAX            (BT_AWS_MCE_REPORT_MODULE_RANGE + BT_AWS_MCE_REPORT_MODULE_CUSTOM_RANGE)  /**< The maximum module type. */
#define BT_AWS_MCE_REPORT_INVALID_MODULE_ID     (0xFF)                                           /**< The invalid awsm mce module ID. */

typedef uint8_t bt_aws_mce_report_module_id_t; /**< Type definition of the module. */

/**
 * @brief define the transfer type for module id by using #bt_aws_mce_report_get_mapped_module_id
 *
 */
#define BT_AWS_MCE_REPORT_MODULE_ID_MAP_TYPE_NEW_TO_OLD (0x01)      /**< The mapped from new to old id. */
#define BT_AWS_MCE_REPORT_MODULE_ID_MAP_TYPE_OLD_TO_NEW (0x02)      /**< The mapped from old to new id. */
typedef uint8_t bt_aws_mce_report_module_id_map_t;        /**< Type definition of the module id transfer type. */


/**
 * @brief define the transfer type for module id by using #bt_aws_mce_report_get_mapped_module_id
 *
 */
#define BT_AWS_MCE_REPORT_MODULE_ID_MAP_TYPE_NEW_TO_OLD (0x01)      /**< The mapped from new to old id. */
#define BT_AWS_MCE_REPORT_MODULE_ID_MAP_TYPE_OLD_TO_NEW (0x02)      /**< The mapped from old to new id. */
typedef uint8_t bt_aws_mce_report_module_id_map_t;        /**< Type definition of the module id transfer type. */


/**
 * @brief Define the max data length.
 */
#define BT_AWS_MCE_MAX_DATA_LENGTH  (872)      /**<This value is the maximum AWS MCE packet length. */
#define BT_AWS_MCE_SYNC_MAX_DATA_LENGTH  (868) /**<This value is the maximum AWS MCE sync event packet length. */
/**
 * @}
 */

/**
* @defgroup BluetoothServices_AWS_MCE_Report_struct Struct
* @{
*/

/**
 *  @brief This structure defines the entering parameters that module owner should transfer ".
 */
typedef struct {
    bt_aws_mce_report_module_id_t    module_id;    /**< The owner of this action. */
    bool    is_sync;                               /**< The parameter indicates whether this event syncs between Agent and Partner or not. Note: this param will be removed with #bt_aws_mce_report_send_sync_event.  */
    uint32_t    sync_time;                         /**< The time duration. The event acts synchronously after this period of time. Unit: ms. Note: this param will be removed with #bt_aws_mce_report_send_sync_event.*/
    uint32_t    param_len;                         /**< The length of the parameter. */
    void *param;                                   /**< The parameter's pointer of the app_report info. */
} bt_aws_mce_report_info_t;
/**
 * @}
 */

/**
 * @addtogroup BluetoothServices_AWS_MCE_Report_define Define
 * @{
 * Define the AWS_MCE_Report types.
 */

/**
 * @brief Define the callback functions to register.
 */

/** @brief The prototype of the callback function, which needs to be registered to send events for a different owner.
  *   @param[in] param    is the event information to handle.
  */
typedef void (*bt_aws_mce_report_callback_t)(bt_aws_mce_report_info_t *param);

/**
 * @brief     This function registers bt event callback.
 * @return         void.
 */
void bt_aws_mce_report_init(void);


/**
 * @brief     This function registers a callback sent to the bt_aws_mce_report module. It is suggested to call this function when the module is initialized.
 * @param[in] module_id          is the module type to register.
 * @param[in] callback           is the callback function to register.
 * @return
 * #BT_STATUS_SUCCESS, if the operation is successful.
 * #BT_STATUS_FAIL, if the module type is incorrect or already registered.
 */
bt_status_t bt_aws_mce_report_register_callback(bt_aws_mce_report_module_id_t module_id, bt_aws_mce_report_callback_t callback);

#ifdef MTK_MUX_AWS_MCE_ENABLE
/** @brief The prototype of the callback function, which needs to be registered to send events for a different owner.
  *   @param[in] param    is the event information to handle.
  */
typedef void (*bt_aws_mce_report_mux_callback_t)(bt_msg_type_t msg, void *param, uint32_t len);

/**
 * @brief     This function registers a callback sent to the bt_aws_mce_report module via mux. It is suggested to call this function when the module is initialized.
 * @param[in] callback           is the callback function to register.
 * @return
 * #BT_STATUS_SUCCESS, if the operation is successful.
 * #BT_STATUS_FAIL, if the module type is incorrect or already registered.
 */
bt_status_t bt_aws_mce_report_register_mux_callback(bt_aws_mce_report_mux_callback_t callback);
#endif


/**
 * @brief     This function deregisters the callback from the bt_aws_mce_report module. It is suggested to call this function when the module is deinitialized.
 * @param[in] module_id          is the module type to unregister.
 * @return
 * #BT_STATUS_SUCCESS, if the operation is successful.
 * #BT_STATUS_FAIL, if the module type is incorrect or unregistered.
 */
bt_status_t bt_aws_mce_report_deregister_callback(bt_aws_mce_report_module_id_t  module_id);


/**
 * @brief     This function should be called by the user when it is necessary to send sync app_report info between Agent and Partner.
 *            Note: this function will be removed in the feature.If you want send sync event, you should get the target bt_clock and send it by
 *            #bt_aws_mce_report_send_urgent_event.
 * @param[in] info          is the sync event information to send.
 * @return
 * #BT_STATUS_SUCCESS, if the operation is successful.
 * #BT_STATUS_FAIL, if the event is not sent successfully.
 */
bt_status_t bt_aws_mce_report_send_sync_event(bt_aws_mce_report_info_t *info);

/**
 * @brief     This function should be called by the user when it is necessary to send app_report info between Agent and Partner.
 * @param[in] info          is the event information to send.
 * @return
 * #BT_STATUS_SUCCESS, if the operation is successful.
 * #BT_STATUS_FAIL, if the event is not sent successfully.
 */
bt_status_t bt_aws_mce_report_send_event(bt_aws_mce_report_info_t *info);

/**
 * @brief     This function should be called by the user when it is necessary for Agent to send urgent app_report info.
 * @param[in] info          is the event information to send.
 * @return
 * #BT_STATUS_SUCCESS, if the operation is successful.
 * #BT_STATUS_FAIL, if the event is not sent successfully.
 */
bt_status_t bt_aws_mce_report_send_urgent_event(bt_aws_mce_report_info_t *info);

/**
 * @brief     This function should be called by the user when it is necessary to use mux port to send app_report info  between Agent and Partner.
 * @param[in] packet          is the data to send.
 * @param[in] len                 is the length of the data.
 * @param[in] urgent          is the flag that indicate use the urgent channel or not.
 * @return
 * #BT_STATUS_SUCCESS, if the operation is successful.
 * #BT_STATUS_FAIL, if the event is not sent successfully.
 */

uint32_t bt_aws_mce_report_mux_send(uint8_t *packet, uint32_t len, bool urgent);


/**
 * @brief     This callback need be implement by app to get the mapped module id.
 * @param[in] type          module id mapped type.
 * @param[in] module_id     the source module id.
 * @return                  the dest module id after mapped or #BT_AWS_MCE_REPORT_INVALID_MODULE_ID.
 */
uint8_t bt_aws_mce_report_get_mapped_module_id(bt_aws_mce_report_module_id_map_t type, uint8_t module_id);


#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 * @}
 * @}
 */


#endif /* __BT_AWS_MCE_REPORT_H__ */

