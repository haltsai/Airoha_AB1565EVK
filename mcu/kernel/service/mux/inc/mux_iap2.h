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

#ifndef __MUX_IAP2_H__
#define __MUX_IAP2_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MTK_IAP2_VIA_MUX_ENABLE
#include "bt_type.h"

#define MAX_MUX_IAP2_NUM                4
#define MAX_MUX_IAP2_PACKET_NUM         (MAX_MUX_IAP2_NUM * 3)

#define MAX_MUX_IAP2_SESSION_NUMBER     3 /*max iap2 session number in one port*/

#define MUX_IAP2_SESSION1_PROTOCOL_ID   0x31U
#define MUX_IAP2_SESSION2_PROTOCOL_ID   0x32U
#define MUX_IAP2_SESSION3_PROTOCOL_ID   0x33U
#define MUX_IAP2_SESSION4_PROTOCOL_ID   0x34U

#define MUX_IAP2_INVALID_SESSION_ID     0x0000

typedef struct {
    uint16_t session_id;
} mux_iap2_header_t;

typedef struct {
    uint16_t session_id;
    uint16_t max_packet_size;
    bt_bd_addr_t remote_address;
} mux_iap2_connection_t;

typedef struct {
    uint16_t session_id;
} mux_iap2_disconnection_t;

void mux_iap2_register_callbacks(void);

void mux_iap2_send_app_launch_request(uint8_t *app_id, bool is_alert);

#endif

#ifdef __cplusplus
}
#endif

#endif
