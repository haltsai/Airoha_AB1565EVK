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


#ifndef __RACE_LPCOMM_CONN_H__
#define __RACE_LPCOMM_CONN_H__


#include "race_cmd_feature.h"
#ifdef RACE_LPCOMM_ENABLE
#include "race_lpcomm_util.h"


#define RACE_LPCOMM_CONN_MAX_NUM  (1)

/* Only one device */
#define RACE_LPCOMM_DEFAULT_DEVICE_ID    (1)


/* Multiple link: Multiple SPs to one Agent or multiple Partners to one Agent. No other case.
  * Even there're multiple partners the role is still Agent or partner(or client).
  * It will not be Agent[0-n] or Partner[0-m].
  */
typedef struct
{
    bool is_used;
    uint8_t device_id;
    race_lpcomm_role_enum device_role;      /* The role of the device attached not the role of the current device. */
    race_lpcomm_trans_method_enum trans_method;
    /* Reserved for the future design. Transport modules may provide some info to identify
          * different devices. Match that info with the device_id. */
    void *device_data;
}race_lpcomm_conn_info_struct;


race_lpcomm_conn_info_struct *race_lpcomm_find_conn_info(void *device_data,
                                                                  uint32_t device_data_len,
                                                                  race_lpcomm_role_enum device_role,
                                                                  race_lpcomm_trans_method_enum trans_method);

RACE_ERRCODE race_lpcomm_attach_proc(uint8_t *device_id,
                                         void *device_data,
                                         uint32_t device_data_len,
                                         race_lpcomm_role_enum device_role,
                                         race_lpcomm_trans_method_enum trans_method);

RACE_ERRCODE race_lpcomm_deattach_proc(void *device_data,
                                                uint32_t device_data_len,
                                                race_lpcomm_role_enum device_role,
                                                race_lpcomm_trans_method_enum trans_method);

bool race_lpcomm_is_attached(uint8_t device_id);

#endif /* RACE_LPCOMM_ENABLE */
#endif /* __RACE_LPCOMM_CONN_H__ */

