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
 
#ifndef __AIRUPDATE_H__
#define __AIRUPDATE_H__
#ifdef MTK_AIRUPDATE_ENABLE

#ifdef __cplusplus
extern "C" {
#endif

#include "bt_type.h"

#define AIRUPDATE_HEADER_LEN           (0x06)

#define AIRUPDATE_LEN_OFFSET           (0x02)
#define AIRUPDATE_OGF_OFFSET           (0x04)
#define AIRUPDATE_OCF_OFFSET           (0x05)

#define AIRUPDATE_OGF_RACE             (0xEE)

#define AIRUPDATE_OCF_CMD              (0x02)
#define AIRUPDATE_OCF_RSP              (0x03)

#define AIRUPDATE_OGF_OCF_LEN          (0x02)

#define AIRUPDATE_U16_LEN(x)      (uint16_t)((*(uint8_t*)(x) << 8) | *((uint8_t*)(x)+1))

typedef struct
{
    uint8_t  bt_addr[BT_BD_ADDR_LEN];   /**< Bluetooth Device Address defined in the specification. */
    uint16_t max_packet_size; 
    uint32_t conn_handle;
    bool     connected;
    bool     ready_to_send;
} airupdate_cntx_t;

typedef struct Node
{
    uint8_t *packet;
    uint16_t packet_length;
    struct Node *next;
} airupdate_node_t;

airupdate_node_t *airupdate_create_list(void);
void airupdate_add_node(airupdate_node_t *head, uint8_t *packet, uint16_t packet_length);
uint32_t airupdate_get_node_length(airupdate_node_t *head);
uint32_t airupdate_delete_node(airupdate_node_t * head, uint8_t *packet, uint16_t packet_length);
uint32_t airupdate_find_node(airupdate_node_t *head, uint8_t *packet, uint16_t packet_length);
airupdate_node_t *airupdate_find_node_by_index(airupdate_node_t * head, int index);


#ifdef __cplusplus
}
#endif

#endif
#endif //__AIRUPDATE_H__

