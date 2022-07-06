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

#include "apps_aws_sync_event.h"
#include "FreeRTOS.h"
#include "apps_debug.h"

bt_status_t apps_aws_sync_event_send_extra(uint32_t event_group,
                                           uint32_t event_id, void *extra_data, uint32_t extra_data_len)
{
    bt_status_t ret;
    uint8_t info_array[sizeof(bt_aws_mce_report_info_t)];
    bt_aws_mce_report_info_t *aws_data = (bt_aws_mce_report_info_t *)&info_array;
    uint8_t *data_array = pvPortMalloc(sizeof(uint32_t) + sizeof(uint32_t) + extra_data_len);
    if (!data_array) {
        ret = BT_STATUS_OUT_OF_MEMORY;
    } else {
        aws_data->param = data_array;
        aws_data->is_sync = false;
        aws_data->sync_time = 0;
        aws_data->module_id = BT_AWS_MCE_REPORT_MODULE_APP_ACTION;
        aws_data->param_len = 0;
        memcpy(data_array, &event_group, sizeof(uint32_t));
        aws_data->param_len += sizeof(uint32_t);
        memcpy(data_array + aws_data->param_len, &event_id, sizeof(event_id));
        aws_data->param_len += sizeof(uint32_t);
        if (extra_data && extra_data_len) {
            memcpy(data_array + aws_data->param_len, extra_data, extra_data_len);
            aws_data->param_len += extra_data_len;
        }
        APPS_LOG_DUMP_I("apps_aws_sync_event_send_extra dump", aws_data->param,
                        aws_data->param_len);
        ret = bt_aws_mce_report_send_event(aws_data);
        vPortFree(data_array);
    }
    return ret;
}

bt_status_t apps_aws_sync_event_send(uint32_t event_group, uint32_t event_id)
{
    return apps_aws_sync_event_send_extra(event_group, event_id, NULL, 0);
}

void apps_aws_sync_event_decode_extra(bt_aws_mce_report_info_t *aws_data_ind, uint32_t *event_group, uint32_t *event_id,
                                      void **p_extra_data, uint32_t *p_extra_data_len)
{
    uint8_t *p_data = (uint8_t *)aws_data_ind->param;
    size_t data_len = aws_data_ind->param_len;
    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        APPS_LOG_DUMP_I("apps_aws_sync_event_decode_extra dump", aws_data_ind->param,
                        aws_data_ind->param_len);
        *event_group = *(uint32_t *)p_data;
        p_data += sizeof(uint32_t);
        data_len -= sizeof(uint32_t);

        *event_id = *(uint32_t *)p_data;
        p_data += sizeof(uint32_t);
        data_len -= sizeof(uint32_t);

        if (p_extra_data && p_extra_data_len) {
            *p_extra_data_len = data_len;
            if (data_len) {
                *p_extra_data = p_data;
            }
        }
    }
}

void apps_aws_sync_event_decode(bt_aws_mce_report_info_t *aws_data_ind, uint32_t *event_group, uint32_t *event_id)
{
    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        *event_group = *(uint32_t *)aws_data_ind->param;
        *event_id = *(uint32_t *)(aws_data_ind->param + sizeof(uint32_t));
    }
}
