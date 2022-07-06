/* Copyright Statement:
 *
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

#ifndef _VOLUME_ESTIMATOR_INTERFACE_H_
#define _VOLUME_ESTIMATOR_INTERFACE_H_

#if defined(AIR_VOLUME_ESTIMATOR_ENABLE)

/* Includes ------------------------------------------------------------------*/
#include "dsp_utilities.h"
#include "dsp_buffer.h"
#include "dsp_feature_interface.h"
#include "dsp_callback.h"
#include "dsp_sdk.h"
#include <stdint.h>
#include "dsp_para_chat_vol.h"

/* Public define -------------------------------------------------------------*/
#define VOLUME_ESTIMATOR_PORT_MAX            1

/* Public typedef ------------------------------------------------------------*/
typedef enum
{
    VOLUME_ESTIMATOR_STATUS_ERROR = 0,
    VOLUME_ESTIMATOR_STATUS_OK = 1
} volume_estimator_status_t;

typedef enum
{
    VOLUME_ESTIMATOR_PORT_STATUS_DEINIT = 0,
    VOLUME_ESTIMATOR_PORT_STATUS_INIT = 1,
    VOLUME_ESTIMATOR_PORT_STATUS_RUNNING = 2
} volume_estimator_port_status_t;

typedef enum
{
    /* In VOLUME_ESTIMATOR_CHAT_NORMAL_MODE mode, there is a delay on the volume output of the input data */
    VOLUME_ESTIMATOR_CHAT_NORMAL_MODE,
    /* In VOLUME_ESTIMATOR_CHAT_NORMAL_MODE mode, there is no delay on the volume output of the input data */
    VOLUME_ESTIMATOR_CHAT_INSTANT_MODE,
    VOLUME_ESTIMATOR_MODE_MAX,
} volume_estimator_mode_t;

typedef struct {
    uint16_t channel_num;
    uint16_t frame_size;
    stream_resolution_t resolution;
    volume_estimator_mode_t mode;
    uint32_t sample_rate;
    void *nvkey_para;
} volume_estimator_config_t;

typedef struct {
    volume_estimator_port_status_t status;
    void *owner;
    void *handle;
    uint16_t channel_num;
    uint16_t frame_size;
    stream_resolution_t resolution;
    volume_estimator_mode_t mode;
    uint32_t sample_rate;
    void *nvkey_para;
} volume_estimator_port_t;

/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
extern volume_estimator_port_t * volume_estimator_get_port(void *owner);
extern volume_estimator_status_t volume_estimator_init(volume_estimator_port_t *port, volume_estimator_config_t *config);
extern volume_estimator_status_t volume_estimator_deinit(volume_estimator_port_t *port);
extern volume_estimator_status_t volume_estimator_process(volume_estimator_port_t *port, void *data_buf, uint32_t data_size, int32_t *out_db);


#endif /* AIR_VOLUME_ESTIMATOR_ENABLE */

#endif /* _VOLUME_ESTIMATOR_INTERFACE_H_ */
