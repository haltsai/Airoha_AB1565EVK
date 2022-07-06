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

#ifndef _STREAM_BT_COMMON_H_
#define _STREAM_BT_COMMON_H_

/* Includes ------------------------------------------------------------------*/
#include "config.h"
#include "types.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "source.h"
#include "sink.h"
#include "transform_inter.h"

#include "stream_audio_hardware.h"
#include "stream_audio_setting.h"
#include "stream_audio_driver.h"
#include "dsp_audio_msg_define.h"
#include "dsp_audio_msg.h"
#include "hal_hw_semaphore.h"
#include "hal_resource_assignment.h"

/* Public define -------------------------------------------------------------*/
#define BT_COMMON_STATUS_DEINIT                 0
#define BT_COMMON_STATUS_INIT                   1
#define BT_COMMON_STATUS_START                  2
#define BT_COMMON_STATUS_RUNNING                3
#define BT_COMMON_STATUS_OK                     4
#define BT_COMMON_STATUS_PLC_ERROR              5
#define BT_COMMON_STATUS_BYPASS_DECODER_ERROR   6

/* Public typedef ------------------------------------------------------------*/
typedef void (*f_bt_common_ccni_callback_t)(hal_ccni_event_t event, void * msg);

/* Public functions ----------------------------------------------------------*/
extern void bt_common_register_isr_handler(f_bt_common_ccni_callback_t callback);
extern void bt_common_unregister_isr_handler(f_bt_common_ccni_callback_t callback);
extern void SourceInit_bt_common(SOURCE source);
extern void SinkInit_bt_common(SINK sink);

#endif /* _STREAM_BT_COMMON_H_ */
