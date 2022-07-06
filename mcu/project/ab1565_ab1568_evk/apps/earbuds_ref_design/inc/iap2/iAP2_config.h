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

/**
 * File:iAP2_config.h
 *
 * Description: The file defines the interface of iAP2_config.c.
 *
 */

#ifndef _IAP2_CONFIG_H_
#define _IAP2_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
*  @brief IAP2 Identification Params
*/

/**
 * @brief IAP2 ExternalAccessoryProtocol  paramter group IDs.
 */
#define IAP2_SUBPARAM_EA_PROTOCOL_IDENTIFIER                0
#define IAP2_SUBPARAM_EA_PROTOCOL_NAME                      1
#define IAP2_SUBPARAM_EA_PROTOCOL_MATCH_ACTION              2
#define IAP2_SUBPARAM_NATIVE_TRANSPORT_COMPONENT_IDENTIFIER 3

/**
 * @brief IAP2 BluetoothTransportComponent  paramter group IDs.
 */
#define IAP2_SUBPARAM_TRANSPORT_COMPONENT_IDENTIFIER        0
#define IAP2_SUBPARAM_TRANSPORT_COMPONENT_NAME              1
#define IAP2_SUBPARAM_TRANSPORT_SUPPORTS_IAP2_CONNECTION    2
#define IAP2_SUBPARAM_BT_TRANSPORT_MEDIA_ACCESS_ADDRESS     3

/**
 * @brief IAP2 TransportComponentIdentifier.
 */
#define IAP2_AIROHA_BT_TRANSPORT_IDENTIFER                  0x4254  /* "BT" */


/**
 * @brief IAP2 iAP2HIDCommponent  paramter group IDs.
 */
#define IAP2_SUBPARAM_HID_COMPONENT_IDENTIFIER              0
#define IAP2_SUBPARAM_HID_COMPONENT_NAME                    1
#define IAP2_SUBPARAM_HID_COMPONENT_FUNCTION                2

/**
 * @brief IAP2 HIDCommponentidentifier.
 */
#define IAP2_AIROHA_HID_COMPONENT_IDENTIFIER_1              0x0A

/**
 * @brief IAP2 iAP2AppLaunch paramter group IDs.
 */
#define IAP2_SUBPARAM_APP_LAUNCH_BUNDLE_IDENTIFIER          0
#define IAP2_SUBPARAM_APP_LAUNCH_METHOD                     1

#ifdef __cplusplus
}
#endif

#endif

