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
/**
 * File: dcxo_capid.c
 *
 * Description: The file implements the interface of load capid value.
 *
 */

#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#include "hal.h"
#include "nvkey_id_list.h"
#include "nvkey.h"
#include <string.h>
#define NVKEY_CAPID
#define CAPID_MASK    0x1FF    /*  Dcxo capid range : 0 ~ 511. */
extern uint32_t dcxo_capid;
#endif

typedef struct {
    uint8_t trim_spec;
    uint16_t cap_value;
    uint8_t reserved;
} __attribute__((packed)) xo_info_t;


void dcxo_load_capid(void)
{
#ifdef MTK_NVDM_ENABLE
    uint32_t size = 0;
#ifdef DCXO_CAPIO
    size = sizeof(dcxo_capid);
    if (nvdm_read_data_item("DCXO", "CAPID Value", (uint8_t *)&dcxo_capid, &size) != NVDM_STATUS_OK) {
        LOG_MSGID_E(common, "CAPID item in  NVDM(dcxo) is empty, use default value.\r\n", 0);
        return;
    }
#elif defined(NVKEY_CAPID)
    uint8_t nvkey[4] = {0};
    size = sizeof(xo_info_t);
    if (nvkey_read_data(NVKEYID_MP_CAL_XO_26M_CRTSTAL_TRIM, nvkey, &size) == NVKEY_STATUS_OK) {
        dcxo_capid = (((xo_info_t *)nvkey) -> cap_value) & CAPID_MASK;
        LOG_MSGID_I(common, "CAPID item in NVDM(nvkey) is available(0x%x).\r\n", 1, dcxo_capid);
    } else {
        LOG_MSGID_E(common, "CAPID item in  NVDM(nvkey) is empty, use default value.\r\n", 0);
        return;
    }
#endif
#endif
}
