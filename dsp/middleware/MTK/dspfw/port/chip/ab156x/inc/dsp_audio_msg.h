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
 
#ifndef _DSP_AUDIOMSG_H_
#define _DSP_AUDIOMSG_H_

#include "hal_ccni.h"
#include "hal_ccni_config.h"
#include "dsp_audio_msg_define.h"
#include "dsp_temp.h"

#define AUDIO_TX_RETRY_TIMES    40

#define AIR_RX_TX_CALLBACK_MEMORY_DEDICATE_ENABLE

typedef enum {
    AUDIO_MSG_STATUS_ERROR             = 0,
    AUDIO_MSG_STATUS_OK                = 1
} aud_msg_status_t;

typedef struct aud_msg_node_t {
    hal_ccni_message_t msg;
    struct aud_msg_node_t *next;
} aud_msg_node_t;

typedef struct ccni_msg_memo {
    hal_ccni_message_t exec_;
    hal_ccni_message_t enq_;
    hal_ccni_message_t deq_;
}ccni_memo_t;

typedef enum {
    AUDIO_MSG_TX                = 0,
    AUDIO_MSG_RX
} aud_msg_queue_t;

typedef struct {
    void (*callback)(hal_ccni_message_t msg);
} aud_tx_msg_callback_t;

typedef struct {
    void (*callback)(hal_ccni_message_t msg, hal_ccni_message_t *ack);
} aud_rx_msg_callback_t;

typedef struct aud_msg_cb_info_t {
    uint16_t msg_id;
    uint8_t ack_option;
    void *cb_func;
}aud_msg_cb_info_t;

typedef struct aud_msg_cb_node_t {
    aud_msg_cb_info_t cb_info;
    struct aud_msg_cb_node_t *next;
}aud_msg_cb_node_t;

aud_msg_status_t aud_msg_init(void);
extern aud_msg_status_t aud_msg_tx_handler(hal_ccni_message_t msg, uint8_t to_the_front, uint8_t from_ISR);
aud_msg_status_t aud_msg_tx_ack_register_callback(DSP2MCU_AUDIO_MSG ID, void *tx_ack_callback);
aud_msg_status_t aud_msg_rx_register_callback(MCU2DSP_AUDIO_MSG ID, void *rx_callback, uint8_t ack_opt);
extern void aud_msg_ack(DSP2MCU_AUDIO_MSG ID, uint8_t from_ISR);

#endif //_DSP_AUDIOMSG_H_
