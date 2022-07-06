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

#ifndef __BT_INIT_H__
#define __BT_INIT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* max supported connection number */
#define BT_LE_CONNECTION_NUM   4

/* max supported connection number */
#define BT_CONNECTION_MAX   1

#define BT_CIS_CONNECTION_MAX   4

/* max supported EDR connection number */
/* Temp for multi link */
#define BT_MAX_LINK_NUM 3

/* max supported connection number for AWS*/
/* Temp for multi link */
#define BT_MAX_CONNECTION_NUM_WITH_AWS 3

/* max timer count */
#define BT_TIMER_NUM 29

#define BT_TX_BUF_SIZE (6*1024 + 512)
#ifndef GSOUND_LIBRARY_ENABLE
#define BT_RX_BUF_SIZE (6*1024 + 512)
#else
#define BT_RX_BUF_SIZE (8*1024 + 512)
#endif

#define BT_TIMER_BUF_SIZE (BT_TIMER_NUM * BT_CONTROL_BLOCK_SIZE_OF_TIMER)
#define BT_LE_CONNECTION_BUF_SIZE (BT_LE_CONNECTION_NUM * BT_CONTROL_BLOCK_SIZE_OF_LE_CONNECTION)
#define BT_CONNECTION_BUF_SIZE (BT_MAX_CONNECTION_NUM_WITH_AWS * BT_CONTROL_BLOCK_SIZE_OF_EDR_CONNECTION)
#define BT_CIS_CONNECTION_BUF_SIZE (BT_CIS_CONNECTION_MAX* BT_CONTROL_BLOCK_SIZE_OF_LE_CIS_CONNECTION)

#define BT_RFCOMM_TOTAL_LINK_NUM BT_MAX_LINK_NUM /**<[IMPORTANT!]total num = N, N is the acl link num that rfcomm support*/
#define BT_RFCOMM_LINK_BUF_SIZE (BT_RFCOMM_TOTAL_LINK_NUM * BT_CONTROL_BLOCK_SIZE_OF_RFCOMM)

#define BT_HFP_TOTAL_LINK_NUM BT_MAX_LINK_NUM
#define BT_HFP_LINK_BUF_SIZE (BT_HFP_TOTAL_LINK_NUM * BT_CONTROL_BLOCK_SIZE_OF_HFP)

#define BT_HSP_TOTAL_LINK_NUM BT_MAX_LINK_NUM
#define BT_HSP_LINK_BUF_SIZE (BT_HSP_TOTAL_LINK_NUM * BT_CONTROL_BLOCK_SIZE_OF_HSP)

#define BT_AVRCP_TOTAL_LINK_NUM BT_MAX_LINK_NUM
#define BT_AVRCP_LINK_BUF_SIZE (BT_AVRCP_TOTAL_LINK_NUM * BT_CONTROL_BLOCK_SIZE_OF_AVRCP)

#define BT_AVRCP_EX_TOTAL_LINK_NUM BT_MAX_LINK_NUM
#define BT_AVRCP_EX_LINK_BUF_SIZE (BT_AVRCP_EX_TOTAL_LINK_NUM *BT_CONTROL_BLOCK_SIZE_OF_AVRCP_EX)

#define BT_A2DP_SEP_TOTAL_NUM 9
#define BT_A2DP_SEP_BUF_SIZE (BT_A2DP_SEP_TOTAL_NUM * BT_CONTROL_BLOCK_SIZE_OF_A2DP_SEP)

#define BT_A2DP_TOTAL_LINK_NUM BT_MAX_LINK_NUM
#define BT_A2DP_LINK_BUF_SIZE (BT_A2DP_TOTAL_LINK_NUM * BT_CONTROL_BLOCK_SIZE_OF_A2DP)

#define BT_SPP_TOTAL_CONNECTION_NUM  (3 + 2) /**<[IMPORTANT!]total num = N1 + N2 + ..., Nx is the really used connection num for link-x, each link may different*/
#define BT_SPP_CONNECTION_BUF_SIZE (BT_SPP_TOTAL_CONNECTION_NUM * BT_CONTROL_BLOCK_SIZE_OF_SPP)

#define BT_PBAPC_TOTAL_CONNECTION_NUM  0 /**<[IMPORTANT!]total num = N1 + N2 + ..., Nx is the really used connection num for link-x, each link may different*/
#define BT_PBAPC_CONNECTION_BUF_SIZE (BT_PBAPC_TOTAL_CONNECTION_NUM * BT_CONTROL_BLOCK_SIZE_OF_PBAPC)

/* Temp for multi link */
#define BT_AWS_MCE_TOTAL_CONNECTION_NUM  3 /**<[IMPORTANT!]total num = N1 + N2 + ..., Nx is the really used connection num for link-x, each link may different*/
#define BT_AWS_MCE_CONNECTION_BUF_SIZE (BT_AWS_MCE_TOTAL_CONNECTION_NUM * BT_CONTROL_BLOCK_SIZE_OF_AWS_MCE)

#define BT_AIRUPDATE_TOTAL_CONNECTION_NUM  BT_MAX_LINK_NUM
#define BT_AIRUPDATE_CONNECTION_BUF_SIZE (BT_AIRUPDATE_TOTAL_CONNECTION_NUM * BT_CONTROL_BLOCK_SIZE_OF_AIRUPDATE)

#define BT_CLCK_OFFSET_BUF_UNIT (12)
#define BT_CLOCK_OFFSET_SHARE_BUF_SIZE (4 * BT_CLCK_OFFSET_BUF_UNIT)


#define BT_GAP_ADVERTISING_SET_NUM  5
#define BT_GAP_ADVERTISING_SET_BUF_SIZE (BT_GAP_ADVERTISING_SET_NUM * BT_CONTROL_BLOCK_SIZE_OF_LE_ADV_SET)


#define BT_HID_TOTAL_LINK_NUM BT_MAX_LINK_NUM
#define BT_HID_LINK_BUF_SIZE (BT_HID_TOTAL_LINK_NUM * BT_CONTROL_BLOCK_SIZE_OF_HID)
#define BT_GATT_CONNECTION_BUFF_SIZE (BT_CONTROL_BLOCK_SIZE_OF_GATT * 3)

void bt_sink_init(void);

bt_status_t bt_demo_power_on(void);

bt_status_t bt_demo_power_off(void);

void bt_demo_hci_log_switch(bool on_off);

void bt_demo_syslog_switch(bool on_off);

#ifdef __cplusplus
}
#endif

#endif /* __BT_INIT_H__  */

