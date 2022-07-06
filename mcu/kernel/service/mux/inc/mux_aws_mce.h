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

#ifndef __MUX_AWS_MCE_H__
#define __MUX_AWS_MCE_H__


#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#include "bt_system.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Define the module type to register.
 * For the middleware modules, please define the module type by using #MUX_AWS_MCE_USER_START and offset.
 * The mux_aws_mce module calls back to these users according to the module id of the packet.
 */
#define MUX_AWS_MCE_USER_START          (0x70)                                   /**< The start value of the bt_aws_mce_report  module type. */
#define MUX_AWS_MCE_USER_RANGE          (0x20)                                   /**< The maximum number of bt_aws_mce_report modules that are supported. */
#define MUX_AWS_MCE_USER_CM (MUX_AWS_MCE_USER_START)                     /**< The module type of BT connection manager. */
#define MUX_AWS_MCE_USER_SINK_MUSIC        (MUX_AWS_MCE_USER_START + 1)  /**< The module type of sink music. */
#define MUX_AWS_MCE_USER_SINK_CALL     (MUX_AWS_MCE_USER_START + 2)      /**< The module type of sink call. */
#define MUX_AWS_MCE_USER_BT_AIR     (MUX_AWS_MCE_USER_START + 3)         /**< The module type of BT air. */
#define MUX_AWS_MCE_USER_FOTA     (MUX_AWS_MCE_USER_START + 4)           /**< The module type of FOTA. */
#define MUX_AWS_MCE_USER_TEST   (MUX_AWS_MCE_USER_START + 5)             /**< The module type for test. */
#define MUX_AWS_MCE_USER_RELAY_CMD   (MUX_AWS_MCE_USER_START + 6)        /**< The module type of race relay cmd. */
#define MUX_AWS_MCE_USER_HFP_AVC        (MUX_AWS_MCE_USER_START + 7)     /**< The module type of hfp avc. */
#define MUX_AWS_MCE_USER_GSOUND         (MUX_AWS_MCE_USER_START + 8)     /**< The module type of gsound. */
#define MUX_AWS_MCE_USER_DM             (MUX_AWS_MCE_USER_START + 9)     /**< The module type of bt device manager. */
#define MUX_AWS_MCE_USER_VP   (MUX_AWS_MCE_USER_START +10)                                       /**< The module type of voice prompts. */
#define MUX_AWS_MCE_USER_LED        (MUX_AWS_MCE_USER_START + 11)                             /**< The module type of LED. */
#define MUX_AWS_MCE_USER_PEQ         (MUX_AWS_MCE_USER_START + 12)                            /**< The module type of PEQ. */
#define MUX_AWS_MCE_USER_BATTERY         (MUX_AWS_MCE_USER_START + 13)                        /**< The module type of battery. */
#define MUX_AWS_MCE_USER_APP_ACTION         (MUX_AWS_MCE_USER_START + 14)                     /**< The module type of app action. */
#define MUX_AWS_MCE_USER_ANC         (MUX_AWS_MCE_USER_START + 15)                            /**< The module type of anc. */
#define MUX_AWS_MCE_USER_BLE_APP     (MUX_AWS_MCE_USER_START + 16)                            /**< The module type of ble app. */
#define MUX_AWS_MCE_USER_FAST_PAIR  (MUX_AWS_MCE_USER_START + 17)                             /**< The module type of fast pair. */
#define MUX_AWS_MCE_USER_SMCHARGER  (MUX_AWS_MCE_USER_START + 18)                             /**< The module type of smart charger. */
#define MUX_AWS_MCE_USER_IN_EAR    (MUX_AWS_MCE_USER_START + 19)
#define MUX_AWS_MCE_USER_AP        (MUX_AWS_MCE_USER_START + 20)

#define MUX_AWS_MCE_USER_MAX            (MUX_AWS_MCE_USER_RANGE + MUX_AWS_MCE_USER_RANGE-1)  /**< The maximum module type. */
typedef uint8_t mux_aws_mce_user_id_t; /**< Type definition of the module. */

typedef struct {
    mux_aws_mce_user_id_t aws_mce_user_id;
    const char aws_mce_user_name[50];
} mux_aws_mce_user_table_t;


/**
 *  @brief This structure defines the entering parameters that module owner should transfer ".
 */
typedef struct {
    uint8_t    user_id;    /**< The owner of this action. */
    uint32_t  len;
} mux_aws_mce_header_t;

/**
 *  @brief This structure defines the entering parameters that module owner should transfer ".
 */
typedef struct {
    uint8_t    user_id;    /**< The owner of this action. */
    uint32_t  len;
    bt_clock_t clk;
} mux_aws_mce_urgent_header_t;

mux_status_t port_mux_aws_mce_init();

#ifdef __cplusplus
}
#endif


#endif//__MUX_AWS_MCE_H__

