/* Copyright Statement:
 *
 * (C) 2018  Airoha Technology Corp. All rights reserved.
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

#ifndef _RACE_CMD_CO_SYS_H_
#define _RACE_CMD_CO_SYS_H_

#include "race_cmd.h"

/**
 *  @brief This enum defines the user module ID of the co_system communication.
 */
typedef enum {
    RACE_COSYS_MODULE_ID_BEGIN = 0,
    RACE_COSYS_MODULE_ID_RELAY_CMD = RACE_COSYS_MODULE_ID_BEGIN,
    RACE_COSYS_MODULE_ID_APP_COMMON,        /**< For APP layer common messages. */
    RACE_COSYS_MODULE_ID_SINK,              /**< For Sink Music/Call messages. */
    RACE_COSYS_MODULE_ID_LPCOMM,            /**< For local peer communication. */
    RACE_COSYS_MODULE_ID_ANC_PASSTHROUGH,   /**< For ANC/PT common event. */
    RACE_COSYS_MODULE_ID_NVKEY,             /**< For nvkey sync between co-system. */
    RACE_COSYS_MODULE_ID_AUDIO_SOURCE_CONTROL, /**< For audio source control sync between co-system. */
    RACE_COSYS_MODULE_ID_DSP_COMMON,        /**< For DSP common messages. */
    RACE_COSYS_MODULE_ID_HOST_AUDIO_SET,    /**< For Host Audio set event. */
    RACE_COSYS_MODULE_ID_NUM,
} race_cosys_module_id_t;


/**
 * @brief The prototype of the data callback function.
 * @param[in] is_critical, if the data is critical, the rx callback is running in IRQ.
 * @param[in] buff, the data buffer pointer.
 * @param[in] len, the data length in the buffer.
*/
typedef void (*race_cosys_data_callback_t)(bool from_irq, uint8_t *buff, uint32_t len);

/**
 * @brief     This function registers data callback.
 * @param[in] module_id, the module_id.
 * @param[in] callback, the data callback function.
 * @return    TRUE if register succeed.
 */
bool race_cosys_register_data_callback(race_cosys_module_id_t module_id, race_cosys_data_callback_t callback);

/**
 * @brief     This function send module data.
 * @param[in] module_id, the module ID.
 * @param[in] is_critical, if the data is critical, the rx callback will be called in IRQ.
 * @param[in] buff, the data buffer.
 * @param[in] len, the data length.
 * @return    TRUE if send data succeed.
 */
bool race_cosys_send_data(race_cosys_module_id_t module_id, bool is_critical, uint8_t *buff, uint32_t len);


void* RACE_CmdHandler_co_sys(ptr_race_pkt_t pCmdMsg, uint16_t Length, uint8_t channel_id);


#endif /* _RACE_CMD_CO_SYS_H_ */

