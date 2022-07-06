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

#include "hci_log.h"
#include <stdio.h>
#include <string.h>
#include "syslog.h"
#include "FreeRTOS.h"

#include "hal_gpt.h"

log_create_module(HCI_LOG, PRINT_LEVEL_INFO);
#define RESERVE_BUF_LEN (3)
#define HCI_MAGIC_HI    0xab
#define HCI_MAGIC_LO    0xcd
#define HCI_ATCI_HEADER_LENGTH    (5)
#define HEADER_BUF_LEN (7)
#define PACKET_HEADER_LEN (0x05)

#ifdef HCI_LOG_MIX_WITH_SYSLOG
static void *hci_log_buf_arr[3] = {NULL};
static uint32_t hci_buf_len_arr[2] = {0};

static void hci_log_fill_header(uint8_t *header, unsigned char pkt_type, int32_t pkt_len)
{
    uint32_t gpt_count = 0;
    hal_gpt_status_t gpt_status = HAL_GPT_STATUS_OK;

    // Length
    header[0] = pkt_len & 0xFF;
    header[1] = (pkt_len >> 8) & 0xFF;

    gpt_status = hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_count);
    // Timestamp
    if (gpt_status == HAL_GPT_STATUS_OK) {
        gpt_count = ((uint64_t)gpt_count * 1000) >> 15;
        header[2] = gpt_count & 0xFF;
        header[3] = (gpt_count >> 8) & 0xFF;
        header[4] = (gpt_count >> 16) & 0xFF;
        header[5] = (gpt_count >> 24) & 0xFF;
    }
    // Type
    header[HEADER_BUF_LEN - 1] = pkt_type;
    /* LOG_I(common, "[HCI_LOG], send data() ts:0x%08x, pkt_type: %02d, len: 0x%08x",
                    gpt_count, pkt_type, pkt_len);
    */
}
#endif

#ifdef MTK_SYSLOG_VERSION_2
static int32_t hci_log(unsigned char type, unsigned char *buf, int32_t length)
{
#if 1
    // |--------|----------------|----|---------------------------------------|
    //   Len(2B)       TS(4B)    ind(1B)        data

    uint8_t pkt_header[HEADER_BUF_LEN] = {0};

    hci_log_fill_header(pkt_header, type, PACKET_HEADER_LEN + length);

    hci_log_buf_arr[0] = (void *)pkt_header;
    hci_log_buf_arr[1] = (void *)buf;

    hci_buf_len_arr[0] = HEADER_BUF_LEN;
    hci_buf_len_arr[1] = length;

    do {
        uint32_t ret_len = 0;
        LOG_TLVDUMP_I(HCI_LOG, LOG_TYPE_HCI_DATA, hci_log_buf_arr, hci_buf_len_arr, ret_len);
        (void)(ret_len);
    } while (0);

#else
#include "atci.h"
    atci_response_t *output = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));
    if (output == NULL) {
        return 0;
    }
    output->response_buf[0] = HCI_MAGIC_HI;
    output->response_buf[1] = HCI_MAGIC_LO;
    output->response_buf[2] = type;
    output->response_buf[3] = (uint8_t)((length & 0xff00) >> 8);
    output->response_buf[4] = (uint8_t)(length & 0xff);

    memcpy((void *)(output->response_buf + 5), (void *)buf, length);
    output->response_len = length + 5;
    output->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;

    atci_send_response(output);
    vPortFree(output);
#endif

    return 1;
}

int32_t hci_log_cmd(unsigned char *buf, int32_t length)
{
    return hci_log(HCI_COMMAND, buf, length);
}

int32_t hci_log_event(unsigned char *buf, int32_t length)
{
    return hci_log(HCI_EVENT, buf, length);
}

int32_t hci_log_acl_out(unsigned char *buf, int32_t length)
{
    return hci_log(HCI_ACL_OUT, buf, length);
}

int32_t hci_log_acl_in(unsigned char *buf, int32_t length)
{
    return hci_log(HCI_ACL_IN, buf, length);
}

int32_t hci_log_iso_in(unsigned char* buf, int32_t length)
{
    return hci_log(HCI_ISO_IN, buf, length);
}

int32_t hci_log_iso_out(unsigned char* buf, int32_t length)
{
    return hci_log(HCI_ISO_OUT, buf, length);
}

#endif

