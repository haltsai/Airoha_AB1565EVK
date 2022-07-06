/*
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
#ifdef AIR_LE_AUDIO_ENABLE
#include <stdlib.h>
#include "bt_callback_manager.h"
#include "bt_sdp.h"
#include "bt_connection_manager.h"
#include "bt_sink_srv_ami.h"

#include "nvdm.h"
#include "nvdm_id_list.h"
#include "bt_customer_config.h"
#include "multi_ble_adv_manager.h"

#include "bt_le_audio_msglog.h"
/**************************************************************************************************
* Define
**************************************************************************************************/
#define DHSS_LE_ADDR_LENGTH         7   /* Addr type(1 byte) + Addr(6 bytes) */
#define DHSS_LE_ADDR_LIST_LENGTH    14

/* DUAL_HEADSET_SINK */
#define DHSS_UUID   0x32, 0xbf, 0x2f, 0xe6, 0x29, 0x7c, 0x4c, 0xeb, 0x83, 0x9c, 0x1b, 0xf2, 0x6a, 0x11, 0x55, 0xac

#define DHSS_ATTRIBUTE_ID_DUAL_HEADSET_PAIR_ADDRESS  0x84F6

#define DHSS_DATA_ELEMENT_DUAL_HEADSET_PAIR_ADDR    0x1C

#define DHSS_DATA_OFFSET_LE_ADDR_TYPE_1         3
#define DHSS_DATA_OFFSET_LE_ADDR_TYPE_2         10
#define DHSS_DATA_OFFSET_LE_ADDR_1              4
#define DHSS_DATA_OFFSET_LE_ADDR_2              11

static const uint8_t app_dhss_service_class_id[] = {
    BT_SDP_ATTRIBUTE_HEADER_8BIT(17),
    BT_SDP_UUID_128BIT(DHSS_UUID),
};

static const uint8_t app_dhss_protocol_descriptor_list[] = {
    BT_SDP_ATTRIBUTE_HEADER_8BIT(5),            /* Data element sequence */
    BT_SDP_ATTRIBUTE_HEADER_8BIT(3),            /* Data element sequence for L2CAP, 3 bytes */
    BT_SDP_UUID_16BIT(BT_SDP_PROTOCOL_L2CAP),   /* Uuid16 L2CAP */
};

static uint8_t app_dhss_dual_headset_pair_address[] = {
    BT_SDP_ATTRIBUTE_HEADER_8BIT(17),
    DHSS_DATA_ELEMENT_DUAL_HEADSET_PAIR_ADDR,
    0,                                          /* DHSS_DATA_OFFSET_LE_ADDR_TYPE_1 */
    0, 0, 0, 0, 0, 0,                           /* DHSS_DATA_OFFSET_LE_ADDR_1 */
    0,                                          /* DHSS_DATA_OFFSET_LE_ADDR_TYPE_2 */
    0, 0, 0, 0, 0, 0,                           /* DHSS_DATA_OFFSET_LE_ADDR_2 */
    0, 0,
};

static const bt_sdps_attribute_t app_dhss_sdp_attributes[] = {
    /* Service Class ID List attribute */
    {BT_SDP_ATTRIBUTE_ID_SERVICE_CLASS_ID_LIST, sizeof(app_dhss_service_class_id), app_dhss_service_class_id},
    /* Protocol Descriptor List attribute */
    {BT_SDP_ATTRIBUTE_ID_PROTOCOL_DESC_LIST, sizeof(app_dhss_protocol_descriptor_list), app_dhss_protocol_descriptor_list},
    /* DUAL_HEADSET_PAIR_ADDRESS */
    {DHSS_ATTRIBUTE_ID_DUAL_HEADSET_PAIR_ADDRESS, sizeof(app_dhss_dual_headset_pair_address), app_dhss_dual_headset_pair_address},
};

static const bt_sdps_record_t app_dhss_sdps_record = {
    .attribute_list_length = sizeof(app_dhss_sdp_attributes),
    .attribute_list = app_dhss_sdp_attributes,
};

static bool g_dhss_is_device_paired = false;
static bool g_dhss_read_paired_le_addr = false;
static bool g_dhss_read_local_le_addr = false;

#ifdef MTK_AWS_MCE_ENABLE
static uint8_t g_dhss_le_addr_offset_local = 0;
static uint8_t g_dhss_le_addr_offset_paired = 0;
#endif
/**************************************************************************************************
* Prototype
**************************************************************************************************/

/**************************************************************************************************
* Static function
**************************************************************************************************/
static void app_dhss_reverse_copy(uint8_t *dst, uint8_t *src)
{
    uint8_t i = 0;

    for (i = 0; i < BT_BD_ADDR_LEN; i++) {
        dst[i] = src[BT_BD_ADDR_LEN - 1 - i];
    }
}

static bool app_dhss_reverse_compare(uint8_t *dst, uint8_t *src)
{
    uint8_t i = 0;

    for (i = 0; i < BT_BD_ADDR_LEN; i++) {
        if (dst[i] != src[BT_BD_ADDR_LEN - 1 - i]) {
            return false;
        }
    }
    return true;
}

#ifdef MTK_AWS_MCE_ENABLE
static bool app_dhss_update_le_addr_offset(void)
{
    audio_channel_t channel = 0;

    /* Address: | L address| R address| */
    if ((0 != g_dhss_le_addr_offset_local) && (0 != g_dhss_le_addr_offset_paired)) {
        return true;
    }
    /* Get local audio location */
    channel = ami_get_audio_channel();
    switch (channel) {
        case AUDIO_CHANNEL_L:    /* Local: L */
            g_dhss_le_addr_offset_local = DHSS_DATA_OFFSET_LE_ADDR_TYPE_1;
            g_dhss_le_addr_offset_paired = DHSS_DATA_OFFSET_LE_ADDR_TYPE_2;
            break;
        case AUDIO_CHANNEL_R:    /* Local: R */
            g_dhss_le_addr_offset_local = DHSS_DATA_OFFSET_LE_ADDR_TYPE_2;
            g_dhss_le_addr_offset_paired = DHSS_DATA_OFFSET_LE_ADDR_TYPE_1;
            break;
        default:
            LE_AUDIO_MSGLOG_I("[APP][DHSS] update_le_addr_offset, unknown audio location", 0);
            return false;
    }

    return true;
}
#endif

/**************************************************************************************************
* Public function
**************************************************************************************************/
void app_dhss_update_eir(void)
{
    const bt_cm_config_t *p_config = NULL;

    LE_AUDIO_MSGLOG_I("[APP][DHSS] app_dhss_update_eir", 0);
    p_config = bt_customer_config_get_cm_config();
    bt_gap_set_extended_inquiry_response((uint8_t *)(p_config->eir_data.data), p_config->eir_data.length);
}

bool app_dhss_read_paired_le_addr(void)
{
#ifdef MTK_NVDM_ENABLE
    nvdm_status_t status;
    uint8_t offset = DHSS_DATA_OFFSET_LE_ADDR_TYPE_2;
    uint8_t addr[DHSS_LE_ADDR_LENGTH] = {0};
    uint32_t size = DHSS_LE_ADDR_LENGTH;
#endif

    if (g_dhss_read_paired_le_addr) {
        //LE_AUDIO_MSGLOG_I("[APP][DHSS] read_paired_le_addr, already read!", 0);
        return true;
    }

    g_dhss_read_paired_le_addr = true;

#ifdef MTK_NVDM_ENABLE
    status = nvdm_read_data_item(NVDM_GROUP_DHSS, NVDM_DHSS_ITEM_PAIRED_LE_ADDR, addr, &size);
    if (NVDM_STATUS_OK != status) {
        LE_AUDIO_MSGLOG_I("[APP][DHSS] read_paired_le_addr, NVDM not exist!", 0);
        return false;
    }

#ifdef MTK_AWS_MCE_ENABLE
    /* Address: | L address| R address| */
    if (!app_dhss_update_le_addr_offset()) {
        return false;
    }
    offset = g_dhss_le_addr_offset_paired;
#endif

    memcpy(&app_dhss_dual_headset_pair_address[offset], addr, DHSS_LE_ADDR_LENGTH);
    g_dhss_is_device_paired = true;
    LE_AUDIO_MSGLOG_I("[APP][DHSS] read_paired_le_addr, offset:%d type:%x addr:%x %x %x %x %x %x", 8,
                                                            offset,
                                                            app_dhss_dual_headset_pair_address[offset],
                                                            app_dhss_dual_headset_pair_address[offset+1],
                                                            app_dhss_dual_headset_pair_address[offset+2],
                                                            app_dhss_dual_headset_pair_address[offset+3],
                                                            app_dhss_dual_headset_pair_address[offset+4],
                                                            app_dhss_dual_headset_pair_address[offset+5],
                                                            app_dhss_dual_headset_pair_address[offset+6]);

#endif

    return true;
}

bool app_dhss_read_local_le_addr(void)
{
    uint32_t nvdm_size = sizeof(bt_bd_addr_t) * MULTI_ADV_INSTANCE_MAX_COUNT;
    uint32_t count = 0;
    uint8_t temp_addr[sizeof(bt_bd_addr_t) * MULTI_ADV_INSTANCE_MAX_COUNT] = {0};
    uint8_t offset = DHSS_DATA_OFFSET_LE_ADDR_TYPE_1;

    if (g_dhss_read_local_le_addr) {
        //LE_AUDIO_MSGLOG_I("[APP][DHSS] read_local_le_addr, already read!", 0);
        return true;
    }

    if (NVDM_STATUS_OK != nvdm_read_data_item(NVDM_GROUP_MMI_APP, NVDM_MULTI_ADV_LE_ADDR, temp_addr, &nvdm_size)) {
        LE_AUDIO_MSGLOG_I("[APP][DHSS] read_local_le_addr, NVDM not exist!", 0);
        return false;
    }

    count = (nvdm_size/sizeof(bt_bd_addr_t));
    if (count <= MULTI_ADV_INSTANCE_NOT_RHO) {
        LE_AUDIO_MSGLOG_I("[APP][DHSS] read_local_le_addr, not exist!", 0);
        return false;
    }

#ifdef MTK_AWS_MCE_ENABLE
    /* Address: | L address| R address| */
    if (!app_dhss_update_le_addr_offset()) {
        return false;
    }
    offset = g_dhss_le_addr_offset_local;
#endif

    app_dhss_dual_headset_pair_address[offset] = BT_ADDR_RANDOM;
    app_dhss_reverse_copy(&app_dhss_dual_headset_pair_address[offset+1], (temp_addr + sizeof(bt_bd_addr_t) * MULTI_ADV_INSTANCE_NOT_RHO));
    g_dhss_read_local_le_addr = true;

    LE_AUDIO_MSGLOG_I("[APP][DHSS] read_local_le_addr, offset:%d type:%x addr:%x %x %x %x %x %x", 8,
                                                            offset,
                                                            app_dhss_dual_headset_pair_address[offset],
                                                            app_dhss_dual_headset_pair_address[offset+1],
                                                            app_dhss_dual_headset_pair_address[offset+2],
                                                            app_dhss_dual_headset_pair_address[offset+3],
                                                            app_dhss_dual_headset_pair_address[offset+4],
                                                            app_dhss_dual_headset_pair_address[offset+5],
                                                            app_dhss_dual_headset_pair_address[offset+6]);

    return true;
}

bt_status_t app_dhss_get_le_addr_list(uint8_t *dst)
{
    if (NULL == dst) {
        return BT_STATUS_FAIL;
    }

    app_dhss_read_paired_le_addr();

    if (!g_dhss_is_device_paired) {
        //LE_AUDIO_MSGLOG_I("[APP][DHSS] get_le_addr_list, not paired!", 0);
        return BT_STATUS_FAIL;
    }

    if (!app_dhss_read_local_le_addr()) {
        LE_AUDIO_MSGLOG_I("[APP][DHSS] get_le_addr_list, FAIL", 0);
        return BT_STATUS_FAIL;
    }

    memcpy(dst, &app_dhss_dual_headset_pair_address[DHSS_DATA_OFFSET_LE_ADDR_TYPE_1], DHSS_LE_ADDR_LIST_LENGTH);

#ifdef MTK_AWS_MCE_ENABLE
    if ((0 != g_dhss_le_addr_offset_local) && (0 != g_dhss_le_addr_offset_paired)) {
        /* Address: | L address| R address| */
        if (DHSS_DATA_OFFSET_LE_ADDR_TYPE_1 == g_dhss_le_addr_offset_local) {
            LE_AUDIO_MSGLOG_I("[APP][DHSS] addr_1: L (local), addr_2: R (paired)", 0);
        } else {
            LE_AUDIO_MSGLOG_I("[APP][DHSS] addr_1: L (paired), addr_2: R (local)", 0);
        }
    }
#endif

    LE_AUDIO_MSGLOG_I("[APP][DHSS] addr_1, type:%x addr:%x %x %x %x %x %x", 7,
                                                            app_dhss_dual_headset_pair_address[3],
                                                            app_dhss_dual_headset_pair_address[4],
                                                            app_dhss_dual_headset_pair_address[5],
                                                            app_dhss_dual_headset_pair_address[6],
                                                            app_dhss_dual_headset_pair_address[7],
                                                            app_dhss_dual_headset_pair_address[8],
                                                            app_dhss_dual_headset_pair_address[9]);
    LE_AUDIO_MSGLOG_I("[APP][DHSS] addr_2, type:%x addr:%x %x %x %x %x %x", 7,
                                                            app_dhss_dual_headset_pair_address[10],
                                                            app_dhss_dual_headset_pair_address[11],
                                                            app_dhss_dual_headset_pair_address[12],
                                                            app_dhss_dual_headset_pair_address[13],
                                                            app_dhss_dual_headset_pair_address[14],
                                                            app_dhss_dual_headset_pair_address[15],
                                                            app_dhss_dual_headset_pair_address[16]);
    return BT_STATUS_SUCCESS;
}

bt_status_t app_dhss_get_paired_le_addr(bt_addr_t *dst)
{
    uint8_t offset = DHSS_DATA_OFFSET_LE_ADDR_TYPE_2;

    if (NULL == dst) {
        return BT_STATUS_FAIL;
    }

    app_dhss_read_paired_le_addr();

    if (!g_dhss_is_device_paired) {
        //LE_AUDIO_MSGLOG_I("[APP][DHSS] get_paired_le_addr, not paired!", 0);
        return BT_STATUS_FAIL;
    }

#ifdef MTK_AWS_MCE_ENABLE
    /* Address: | L address| R address| */
    offset = g_dhss_le_addr_offset_paired;
#endif

#if 0
    LE_AUDIO_MSGLOG_I("[APP][DHSS] get_paired_le_addr, offset:%d type:%x addr:%x %x %x %x %x %x", 8,
                                                           offset,
                                                           app_dhss_dual_headset_pair_address[offset],
                                                           app_dhss_dual_headset_pair_address[offset+1],
                                                           app_dhss_dual_headset_pair_address[offset+2],
                                                           app_dhss_dual_headset_pair_address[offset+3],
                                                           app_dhss_dual_headset_pair_address[offset+4],
                                                           app_dhss_dual_headset_pair_address[offset+5],
                                                           app_dhss_dual_headset_pair_address[offset+6]);
#endif

    dst->type = app_dhss_dual_headset_pair_address[offset];
    memcpy(dst->addr, &app_dhss_dual_headset_pair_address[offset+1], BT_BD_ADDR_LEN);

    return BT_STATUS_SUCCESS;
}

bt_status_t app_dhss_get_local_le_addr(bt_addr_t *dst)
{
    uint8_t offset = DHSS_DATA_OFFSET_LE_ADDR_TYPE_1;

    if (NULL == dst) {
        return BT_STATUS_FAIL;
    }

    if (!app_dhss_read_local_le_addr()) {
        LE_AUDIO_MSGLOG_I("[APP][DHSS] get_local_le_addr, FAIL", 0);
        return BT_STATUS_FAIL;
    }

#ifdef MTK_AWS_MCE_ENABLE
    /* Address: | L address| R address| */
    offset = g_dhss_le_addr_offset_local;
#endif

#if 0
    LE_AUDIO_MSGLOG_I("[APP][DHSS] get_local_le_addr, offset:%d type:%x addr:%x %x %x %x %x %x", 8,
                                                            offset,
                                                            app_dhss_dual_headset_pair_address[offset],
                                                            app_dhss_dual_headset_pair_address[offset+1],
                                                            app_dhss_dual_headset_pair_address[offset+2],
                                                            app_dhss_dual_headset_pair_address[offset+3],
                                                            app_dhss_dual_headset_pair_address[offset+4],
                                                            app_dhss_dual_headset_pair_address[offset+5],
                                                            app_dhss_dual_headset_pair_address[offset+6]);
#endif

    dst->type = app_dhss_dual_headset_pair_address[offset];
    memcpy(dst->addr, &app_dhss_dual_headset_pair_address[offset+1], BT_BD_ADDR_LEN);

    return BT_STATUS_SUCCESS;
}

void app_dhss_set_paired_le_addr(bt_addr_type_t type, uint8_t *addr)
{
#ifdef MTK_NVDM_ENABLE
    nvdm_status_t status;
    uint32_t size = DHSS_LE_ADDR_LENGTH;
#endif
    uint8_t offset = DHSS_DATA_OFFSET_LE_ADDR_TYPE_2;

    if (NULL == addr) {
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][DHSS] set_paired_le_addr, is_paired:%x", 1, g_dhss_is_device_paired);

#ifdef MTK_AWS_MCE_ENABLE
    /* Address: | L address| R address| */
    if (!app_dhss_update_le_addr_offset()) {
        return;
    }
    offset = g_dhss_le_addr_offset_paired;
#endif

    if ((g_dhss_is_device_paired) &&
        (app_dhss_reverse_compare(&app_dhss_dual_headset_pair_address[offset+1], addr))) {
        LE_AUDIO_MSGLOG_I("[APP][DHSS] set_paired_le_addr, already set!", 0);
        return;
    }

    g_dhss_is_device_paired = true;
    app_dhss_dual_headset_pair_address[offset] = type;
    app_dhss_reverse_copy(&app_dhss_dual_headset_pair_address[offset+1], addr);

#ifdef MTK_NVDM_ENABLE
    status = nvdm_write_data_item(NVDM_GROUP_DHSS, NVDM_DHSS_ITEM_PAIRED_LE_ADDR, NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&app_dhss_dual_headset_pair_address[offset], size);
    g_dhss_read_paired_le_addr = true;
    LE_AUDIO_MSGLOG_I("[APP][DHSS] set_paired_le_addr, write_status:%x", 1, status);
#endif

    LE_AUDIO_MSGLOG_I("[APP][DHSS] set_paired_le_addr, offset:%d type:%x addr:%x %x %x %x %x %x", 8,
                                                            offset,
                                                            app_dhss_dual_headset_pair_address[offset],
                                                            app_dhss_dual_headset_pair_address[offset+1],
                                                            app_dhss_dual_headset_pair_address[offset+2],
                                                            app_dhss_dual_headset_pair_address[offset+3],
                                                            app_dhss_dual_headset_pair_address[offset+4],
                                                            app_dhss_dual_headset_pair_address[offset+5],
                                                            app_dhss_dual_headset_pair_address[offset+6]);

    /* check update EIR data: need local and paired le addr */
    if (app_dhss_read_local_le_addr()) {
        app_dhss_update_eir();
    }
}

void app_dhss_set_local_le_addr(bt_addr_type_t type, bt_bd_addr_t addr)
{
    uint8_t offset = DHSS_DATA_OFFSET_LE_ADDR_TYPE_1;

#ifdef MTK_AWS_MCE_ENABLE
    /* Address: | L address| R address| */
    if (!app_dhss_update_le_addr_offset()) {
        return;
    }
    offset = g_dhss_le_addr_offset_local;
#endif

    if (app_dhss_reverse_compare(&app_dhss_dual_headset_pair_address[offset+1], (uint8_t *)addr)) {
        LE_AUDIO_MSGLOG_I("[APP][DHSS] set_local_le_addr, already set!", 0);
        return;
    }

    app_dhss_dual_headset_pair_address[offset] = type;
    app_dhss_reverse_copy(&app_dhss_dual_headset_pair_address[offset+1], (uint8_t *)addr);
    g_dhss_read_local_le_addr = true;

    LE_AUDIO_MSGLOG_I("[APP][DHSS] set_local_le_addr, offset:%d type:%x addr:%x %x %x %x %x %x", 8,
                                                            offset,
                                                            app_dhss_dual_headset_pair_address[offset],
                                                            app_dhss_dual_headset_pair_address[offset+1],
                                                            app_dhss_dual_headset_pair_address[offset+2],
                                                            app_dhss_dual_headset_pair_address[offset+3],
                                                            app_dhss_dual_headset_pair_address[offset+4],
                                                            app_dhss_dual_headset_pair_address[offset+5],
                                                            app_dhss_dual_headset_pair_address[offset+6]);

    /* check update EIR data: need local and paired le addr */
    app_dhss_read_paired_le_addr();
    if (g_dhss_is_device_paired) {
        app_dhss_update_eir();
    }
}

bool app_dhss_is_data_ready(void)
{
    app_dhss_read_paired_le_addr();
    app_dhss_read_local_le_addr();

    if (g_dhss_is_device_paired && g_dhss_read_local_le_addr) {
        return true;
    }

    LE_AUDIO_MSGLOG_I("[APP][DHSS] app_dhss_is_data_ready, is_device_paired:%x local_le_addr:%x", 2, g_dhss_is_device_paired, g_dhss_read_local_le_addr);
    return false;
}

bt_status_t app_dhss_sdp_init(void)
{
    app_dhss_read_paired_le_addr();
    app_dhss_read_local_le_addr();
    bt_callback_manager_add_sdp_customized_record(&app_dhss_sdps_record);

    return BT_STATUS_SUCCESS;
}

#endif  /* AIR_LE_AUDIO_ENABLE */

