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
 

#include "bt_aws_mce_ls_association.h"
#include <stdio.h>
void bt_aws_ls_association_start()
{
;
}

bt_status_t bt_aws_ls_association_write_address(bt_bd_addr_t *addr)
{
#ifdef MTK_NVDM_ENABLE
    int8_t i;
    uint8_t buffer[18] = {0};
    nvdm_status_t status;

    /* save address to NVDM */
    for (i = 0; i < 6; ++i) {
        sprintf((char *)buffer + 2 * i, "%02X", (*addr)[i]);
    }
    LOG_I(common, "[BT]address to write:%s len:%d \r\n", buffer, strlen((char *)buffer));
    status = nvdm_write_data_item(NVDM_GROUP_BT_APP, NVDM_BT_APP_ITEM_FIX_ADDR, NVDM_DATA_ITEM_TYPE_STRING, buffer, strlen((char *)buffer));
    if (NVDM_STATUS_OK != status) {
        LOG_MSGID_I(common, "[BT]Failed to store address!\r\n", 0);
        return BT_STATUS_FAIL;
    } else {
        LOG_MSGID_I(common, "[BT]Successfully store address to NVDM [%02X:%02X:%02X:%02X:%02X:%02X]\r\n", 6, addr[0],
              (*addr)[1], (*addr)[2], (*addr)[3], (*addr)[4], (*addr)[5]);
    }
#endif
    return BT_STATUS_SUCCESS;

}


