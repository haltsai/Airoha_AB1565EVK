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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#endif
#include "hal_trng.h"
#include "bt_system.h"
#include "bt_type.h"
#include "bt_hfp.h"
#include "bt_a2dp.h"
#include "bt_init.h"
#include "bt_platform.h"
#include "bt_gap_le.h"
#include "bt_gap.h"
#include "bt_callback_manager.h"
#include "bt_sink_srv.h"
#include "memory_attribute.h"
#include "bt_avm.h"
#include "hal_gpt.h"
#include "hal_audio_internal.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "bt_sink_srv_a2dp.h"
#include "bt_customer_config.h"
#include "bt_power_on_config.h"
#include "bt_device_manager.h"
#include "bt_os_layer_api.h"
ATTR_SHARE_ZIBT_4BYTE_ALIGN char tx_buf[BT_TX_BUF_SIZE] __attribute__((aligned(4)));

ATTR_ZIDATA_IN_NONCACHED_SYSRAM char rx_buf[BT_RX_BUF_SIZE] __attribute__((aligned(4)));

ATTR_SHARE_ZIDATA char le_connection_cb_buf[BT_LE_CONNECTION_BUF_SIZE] __attribute__((aligned(4)));

ATTR_SHARE_ZIDATA char bt_connection_cb_buf[BT_CONNECTION_BUF_SIZE] __attribute__((aligned(4)));

ATTR_SHARE_ZIDATA char bt_clock_offset_share_buf[BT_CLOCK_OFFSET_SHARE_BUF_SIZE] __attribute__((aligned(4)));

#ifdef AIR_LE_AUDIO_ENABLE
ATTR_SHARE_ZIDATA char bt_cis_connection_cb_buf[BT_CIS_CONNECTION_BUF_SIZE] __attribute__((aligned(4)));
#endif

BT_ALIGNMENT4(
    static char timer_cb_buf[BT_TIMER_BUF_SIZE]//one timer control block is 20 bytes
);
BT_ALIGNMENT4(
    static char bt_rfcomm_link_cb_buf[BT_RFCOMM_LINK_BUF_SIZE]
);
BT_ALIGNMENT4(
    static char bt_hfp_link_cb_buf[BT_HFP_LINK_BUF_SIZE]
);
BT_ALIGNMENT4(
    static char bt_hsp_link_cb_buf[BT_HSP_LINK_BUF_SIZE]
);

#ifdef MTK_BT_AVRCP_BROWSE_ENABLE
BT_ALIGNMENT4(
static char bt_avrcp_ex_link_cb_buf[BT_AVRCP_EX_LINK_BUF_SIZE]
);
#else
BT_ALIGNMENT4(
    static char bt_avrcp_link_cb_buf[BT_AVRCP_LINK_BUF_SIZE]
);
#endif
BT_ALIGNMENT4(
    static char bt_a2dp_sep_cb_buf[BT_A2DP_SEP_BUF_SIZE]
);

BT_ALIGNMENT4(
    static char bt_a2dp_link_cb_buf[BT_A2DP_LINK_BUF_SIZE]
);

BT_ALIGNMENT4(
    static char bt_spp_connection_cb_buf[BT_SPP_CONNECTION_BUF_SIZE]
);

BT_ALIGNMENT4(
    static char bt_pbapc_connection_cb_buf[BT_PBAPC_CONNECTION_BUF_SIZE]
);

BT_ALIGNMENT4(
    static char bt_aws_mce_connection_cb_buf[BT_AWS_MCE_CONNECTION_BUF_SIZE]
);

BT_ALIGNMENT4(
    static char bt_airupdate_connection_cb_buf[BT_AIRUPDATE_CONNECTION_BUF_SIZE]
);

BT_ALIGNMENT4(
    static char bt_gap_advertising_set_cb_buf[BT_GAP_ADVERTISING_SET_BUF_SIZE]
);
BT_ALIGNMENT4(
    static char bt_gatt_connection_cb_buf[BT_GATT_CONNECTION_BUFF_SIZE]
);

#ifdef MTK_BT_HID_ENABLE
BT_ALIGNMENT4(
    static char bt_hid_link_cb_buf[BT_HID_LINK_BUF_SIZE]
);
#endif

static bool bt_demo_hci_log_status = false;

bool bt_hci_log_enabled(void);

void bt_demo_hci_log_switch(bool on_off);

static void bt_demo_generate_local_address(bt_bd_addr_t *addr)
{
    int8_t i;
    uint32_t random_seed;

    LOG_MSGID_I(common, "[BT]generate_random_address", 0);
#ifdef HAL_TRNG_MODULE_ENABLED
    hal_trng_status_t ret = HAL_TRNG_STATUS_OK;

    ret = hal_trng_init();
    if (HAL_TRNG_STATUS_OK != ret) {
        LOG_MSGID_I(common, "[BT]generate_random_address--error 1", 0);
    }
    for (i = 0; i < 30; ++i) {
        ret = hal_trng_get_generated_random_number(&random_seed);
        if (HAL_TRNG_STATUS_OK != ret) {
            LOG_MSGID_I(common, "[BT]generate_random_address--error 2", 0);
        }
        LOG_MSGID_I(common, "[BT]generate_random_address--trn: 0x%x", 1, random_seed);
    }
    /* randomly generate address */
    ret = hal_trng_get_generated_random_number(&random_seed);
    if (HAL_TRNG_STATUS_OK != ret) {
        LOG_MSGID_I(common, "[BT]generate_random_address--error 3", 0);
    }
    LOG_MSGID_I(common, "[BT]generate_random_address--trn: 0x%x", 1, random_seed);
    (*addr)[0] = random_seed & 0xFF;
    (*addr)[1] = (random_seed >> 8) & 0xFF;
    (*addr)[2] = (random_seed >> 16) & 0xFF;
    (*addr)[3] = (random_seed >> 24) & 0xFF;
    ret = hal_trng_get_generated_random_number(&random_seed);
    if (HAL_TRNG_STATUS_OK != ret) {
        LOG_MSGID_I(common, "[BT]generate_random_address--error 3", 0);
    }
    LOG_MSGID_I(common, "[BT]generate_random_address--trn: 0x%x", 1, random_seed);
    (*addr)[4] = random_seed & 0xFF;
    (*addr)[5] = (random_seed >> 8) & 0xCF;
    hal_trng_deinit();
#else
//    (*addr)[0] = 0xCD;
//    (*addr)[1] = 0x77;
//    (*addr)[2] = 0xE2;
//    (*addr)[3] = 0xE1;
//    (*addr)[4] = 0x90;
//    (*addr)[5] = 0x87;
//    (*addr)[0] = 0x78;
//    (*addr)[1] = 0x78;
//    (*addr)[2] = 0x78;
//    (*addr)[3] = 0x78;
//    (*addr)[4] = 0x78;
//    (*addr)[5] = 0x78;
    srand((unsigned int)bt_os_layer_get_hal_gpt_time());
    for (i = 0; i < BT_BD_ADDR_LEN; i++) {
        (*addr)[i] = rand() % 0xFF;
    }
    LOG_MSGID_I(common, "[BT]random address [%02X:%02X:%02X:%02X:%02X:%02X]\n", 6, (*addr)[0],
                (*addr)[1], (*addr)[2], (*addr)[3], (*addr)[4], (*addr)[5]);
#endif
}

static void bt_demo_init_local_address(void)
{
    bt_bd_addr_t temp_addr = {0};
    bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
    if (!memcmp(local_addr, temp_addr, sizeof(bt_bd_addr_t))) {
        bt_demo_generate_local_address(&temp_addr);
        bt_device_manager_store_local_address(&temp_addr);
    } else {
        LOG_MSGID_I(common, "[BT]Local public address exists.", 0);
    }
}
void bt_mm_init()
{
    bt_memory_init_packet(BT_MEMORY_TX_BUFFER, tx_buf, BT_TX_BUF_SIZE);
    bt_memory_init_packet(BT_MEMORY_RX_BUFFER, rx_buf, BT_RX_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_TIMER, timer_cb_buf, BT_TIMER_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION, le_connection_cb_buf, BT_LE_CONNECTION_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_EDR_CONNECTION, bt_connection_cb_buf, BT_CONNECTION_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_RFCOMM, bt_rfcomm_link_cb_buf, BT_RFCOMM_LINK_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_HFP, bt_hfp_link_cb_buf, BT_HFP_LINK_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_HSP, bt_hsp_link_cb_buf, BT_HSP_LINK_BUF_SIZE);

#ifdef MTK_BT_AVRCP_BROWSE_ENABLE
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_AVRCP_EX, bt_avrcp_ex_link_cb_buf, BT_AVRCP_EX_LINK_BUF_SIZE);
#else
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_AVRCP, bt_avrcp_link_cb_buf, BT_AVRCP_LINK_BUF_SIZE);
#endif
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_A2DP_SEP, bt_a2dp_sep_cb_buf, BT_A2DP_SEP_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_A2DP, bt_a2dp_link_cb_buf, BT_A2DP_LINK_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_SPP, bt_spp_connection_cb_buf, BT_SPP_CONNECTION_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_PBAPC, bt_pbapc_connection_cb_buf, BT_PBAPC_CONNECTION_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_AWS_MCE, bt_aws_mce_connection_cb_buf, BT_AWS_MCE_CONNECTION_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_AIRUPDATE, bt_airupdate_connection_cb_buf, BT_AIRUPDATE_CONNECTION_BUF_SIZE);
#ifdef AIR_LE_AUDIO_ENABLE
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_LE_CIS_CONNECTION, bt_cis_connection_cb_buf, BT_CIS_CONNECTION_BUF_SIZE);
#endif
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_LE_ADV_SET, bt_gap_advertising_set_cb_buf, BT_GAP_ADVERTISING_SET_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_GATT, bt_gatt_connection_cb_buf, BT_GATT_CONNECTION_BUFF_SIZE);
#ifdef MTK_BT_HID_ENABLE
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_HID, bt_hid_link_cb_buf, BT_HID_LINK_BUF_SIZE);
#endif
}

static bt_status_t bt_avm_share_buffer_init_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    if (msg == BT_POWER_ON_CNF) {
        //Set DSP & N9 share buffer.
        bt_avm_share_buffer_info_t audio_buf = {0};
        audio_buf.a2dp_address = (uint32_t)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);
        audio_buf.sco_dl_address = (uint32_t)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_BT_VOICE_DL);
        audio_buf.sco_up_address = (uint32_t)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_BT_VOICE_UL);
        audio_buf.clock_mapping_address = (uint32_t)hal_audio_query_share_info(AUDIO_RESERVE_TYPE_QUERY_RCDC);
        bt_avm_set_share_buffer(&audio_buf);

#ifdef AIR_BT_CODEC_BLE_ENABLED
        // Set LE audio share buffer
        #define LE_AUDIO_BUFFER_COUNT 2
        bt_avm_leaudio_buffer_info_t *leaudio_buf = pvPortMalloc(LE_AUDIO_BUFFER_COUNT*sizeof(bt_leaudio_buffer_set_t)+sizeof(uint32_t));
        configASSERT(leaudio_buf != NULL);
        {
            leaudio_buf->count = LE_AUDIO_BUFFER_COUNT;
            leaudio_buf->buffer[0].dl_address = (uint32_t)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL);
            leaudio_buf->buffer[0].ul_address = (uint32_t)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL);
            leaudio_buf->buffer[1].dl_address = (uint32_t)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_BLE_AUDIO_SUB_DL);
            leaudio_buf->buffer[1].ul_address = (uint32_t)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_BLE_AUDIO_SUB_UL);

            bt_avm_set_leaudio_buffer(leaudio_buf);
            vPortFree(leaudio_buf);
        }
#endif

        // Set Clock offset share buffer.
        bt_avm_set_clock_offset_share_buffer((uint8_t *)bt_clock_offset_share_buf, 4);
        LOG_MSGID_I(common, "[BT] share buffer set done", 0);
    }

    return BT_STATUS_SUCCESS;
}

static bt_status_t bt_customer_config_get_tx_power_config(bt_tx_power_config_version_t ver, void *tx_power_config)
{
    bt_status_t ret = BT_STATUS_FAIL;
    nvkey_status_t nvkey_ret = NVKEY_STATUS_ERROR;
    uint32_t tx_config_size = 0;

    switch (ver) {
        case BT_TX_POWER_CONFIG_VERSION1: {
            bt_config_tx_power_version1_t default_tx_cfg = {
                .bdr_init_tx_power_level = 7,
                .reserved = 0,
                .fixed_tx_power_enable = 0,
                .fixed_tx_power_level = 0,
                .le_init_tx_power_level = 5,
                .bt_max_tx_power_level = 7,
                .bdr_tx_power_level_offset = 1,
                .bdr_fine_tx_power_level_offset = 0,
                .edr_tx_power_level_offset = 1,
                .edr_fine_tx_power_level_offset = 0,
                .ble_tx_power_level_offset = 1,
                .ble_fine_tx_power_level_offset = 0,
                .edr_init_tx_power_level = 7
            };
            tx_config_size = sizeof(bt_config_tx_power_version1_t);
            nvkey_ret = nvkey_read_data(NVKEYID_BT_DEFAULT_TX_POWER_EXTEND, (uint8_t *)(&default_tx_cfg), &tx_config_size);
            LOG_MSGID_I(common, "read tx power config nvdm ret:%d\r\n", 1, nvkey_ret);
            LOG_MSGID_I(common, "bdr_init_tx:%d, bt_max_tx:%d, le_init_tx:%d, fixed_tx_enable:%d, fixed_tx:%d, bdr_offset:%d, bdr_fine_offset:%d, edr_offset:%d, edr_fine_offset:%d, ble_offset:%d, ble_fine_offset:%d, edr_init_tx:%d",
                        12, default_tx_cfg.bdr_init_tx_power_level, default_tx_cfg.bt_max_tx_power_level,
                        default_tx_cfg.le_init_tx_power_level, default_tx_cfg.fixed_tx_power_enable, default_tx_cfg.fixed_tx_power_level,
                        default_tx_cfg.bdr_tx_power_level_offset, default_tx_cfg.bdr_fine_tx_power_level_offset,
                        default_tx_cfg.edr_tx_power_level_offset, default_tx_cfg.edr_fine_tx_power_level_offset,
                        default_tx_cfg.ble_tx_power_level_offset, default_tx_cfg.ble_fine_tx_power_level_offset,
                        default_tx_cfg.edr_init_tx_power_level);

            memcpy(tx_power_config, &default_tx_cfg, sizeof(bt_config_tx_power_version1_t));
            ret = BT_STATUS_SUCCESS;
            break;
        }
        default: {
            break;
        }
    }
    return ret;
}

static void bt_sink_callback_init(void)
{
    bt_callback_manager_register_callback(bt_callback_type_app_event,
                                          (uint32_t) MODULE_MASK_SYSTEM,
                                          (void *)bt_avm_share_buffer_init_callback);
    bt_callback_manager_register_callback(bt_callback_type_gap_get_local_configuration,
                                          0,
                                          (void *)bt_customer_config_get_gap_config);
    bt_callback_manager_register_callback(bt_callback_type_hfp_get_init_params,
                                          0,
                                          (void *)bt_customer_config_hf_get_init_params);
    bt_callback_manager_register_callback(bt_callback_type_get_feature_mask_configuration,
                                          0,
                                          (void *)bt_customer_config_get_feature_mask_configuration);
    bt_callback_manager_register_callback(bt_callback_type_gap_get_tx_power_configuration,
                                          0,
                                          (void *)bt_customer_config_get_tx_power_config);
}

void bt_sink_init(void)
{
    nvkey_status_t ret;
    bt_config_tx_power_gain_t tx_pwr_BTD_BDR_table[32] = {{0x00BB, 0x1214}, {0x00D1, 0x1214}, {0x00EB, 0x1214}, {0x0100, 0x1214},
        {0x00E4, 0x1215}, {0x0100, 0x1215}, {0x00DE, 0x1216}, {0x0100, 0x1216},
        {0x00E4, 0x1217}, {0x0100, 0x1217}, {0x00E4, 0x2225}, {0x0100, 0x2225},
        {0x00E4, 0x2226}, {0x0100, 0x2226}, {0x00E4, 0x2227}, {0x00D1, 0x2A2F},
        {0x00D1, 0x4A4D}, {0x00EB, 0x4A4D}, {0x00D1, 0x4A4E}, {0x00E4, 0x4A4E},
        {0x00D1, 0x4A4F}, {0x00E4, 0x4A4F}, {0x00C6, 0x6267}, {0x00DE, 0x6267},
        {0x00C6, 0x7A7F}, {0x00DE, 0x7A7F}, {0x00CB, 0x9A9F}, {0x00E4, 0x9A9F},
        {0x00CB, 0xC2C7}, {0x00E4, 0xC2C7}, {0x00E4, 0xDADF}, {0x0100, 0xE2E7}
    };

    bt_config_tx_power_gain_t tx_pwr_BTD_EDR_table[32] = {{0x00B5, 0x1214}, {0x00CB, 0x1214}, {0x00E4, 0x1214}, {0x0100, 0x1214},
        {0x0099, 0x1A1D}, {0x00AB, 0x1A1D}, {0x0099, 0x1A1E}, {0x00F9, 0x1216},
        {0x0099, 0x1A1F}, {0x00AB, 0x1A1F}, {0x00BB, 0x2A2D}, {0x00CB, 0x2A2D},
        {0x00B5, 0x2A2E}, {0x00CB, 0x2A2E}, {0x00BB, 0x2A2F}, {0x00CB, 0x2A2F},
        {0x00B5, 0x5255}, {0x00CB, 0x5255}, {0x00B5, 0x5256}, {0x00CB, 0x5256},
        {0x00B5, 0x5257}, {0x00CB, 0x5257}, {0x00C0, 0x6267}, {0x00D8, 0x6267},
        {0x00C0, 0x7A7F}, {0x00D8, 0x7A7F}, {0x00BB, 0xA2A7}, {0x00D1, 0xA2A7},
        {0x00C0, 0xCACF}, {0x00D8, 0xCACF}, {0x00C0, 0xFAFF}, {0x00D8, 0xFAFF}
    };
    uint32_t BDR_table_size = sizeof(tx_pwr_BTD_BDR_table);
    uint32_t EDR_table_size = sizeof(tx_pwr_BTD_EDR_table);

    bt_firmware_type_set(BT_FIRMWARE_TYPE_EARBUDS);
    ret = nvkey_read_data(NVKEYID_BT_TX_PWR_BDR_GAIN_TABLE, (uint8_t *)(&tx_pwr_BTD_BDR_table), &BDR_table_size);
    LOG_MSGID_I(common, "read nvkey of BDR tain table ret:%d\r\n", 1, ret);

    ret = nvkey_read_data(NVKEYID_BT_TX_PWR_EDR_GAIN_TABLE, (uint8_t *)(&tx_pwr_BTD_EDR_table), &EDR_table_size);
    LOG_MSGID_I(common, "read nvkey of EDR tain table ret:%d\r\n", 1, ret);

    bt_config_tx_power_gain_table(tx_pwr_BTD_BDR_table, tx_pwr_BTD_EDR_table);
#ifdef MTK_BT_A2DP_AAC_ENABLE
    bool aac_on = true;
    uint32_t aac_config_size = sizeof(bool);
    ret = nvkey_read_data(NVKEYID_BT_MUSIC_AAC_ENABLE, (uint8_t *)&aac_on, &aac_config_size);
    LOG_MSGID_I(common, "ret:%d, aac_on:%d\r\n", 2, ret, aac_on);
    bt_sink_srv_a2dp_enable_aac(aac_on);
#endif
    bt_mm_init();
#ifdef MTK_AUDIO_TUNING_ENABLED
    bt_hci_log_enabled();
    bt_demo_hci_log_switch(false);
#endif

    bt_device_manager_init();
    bt_demo_init_local_address();

    bt_sink_callback_init();
#if 0 /*remove to main.c*/
    log_config_print_switch(BTHCI, DEBUG_LOG_OFF);
    log_config_print_switch(BTSPP, DEBUG_LOG_OFF);
    log_config_print_switch(atci, DEBUG_LOG_OFF);
    log_config_print_switch(BTL2CAP, DEBUG_LOG_OFF);
    log_config_print_switch(BTRFCOMM, DEBUG_LOG_OFF);
    log_config_print_switch(gain_value_mapping, DEBUG_LOG_OFF);
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
    bt_gatt_over_bredr_switch(true);
#endif
}

bt_status_t bt_demo_power_on(void)
{
    bt_bd_addr_t *bt_local_public_addr = bt_device_manager_get_local_address();
    return bt_power_on((bt_bd_addr_ptr_t)bt_local_public_addr, NULL);
}

bt_status_t bt_demo_power_off(void)
{
    return bt_power_off();
}

void bt_demo_hci_log_switch(bool on_off)
{
    if (bt_demo_hci_log_status != on_off) {
#ifdef MTK_NVDM_ENABLE
        nvdm_write_data_item("hci_log_switch_group", "hci_log_switch_item",
                             NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&on_off, sizeof(on_off));
#endif // MTK_NVDM_ENABLE
        bt_demo_hci_log_status = on_off;
    }
}

void bt_demo_syslog_switch(bool on_off)
{
    log_switch_t syslog_switch;

    if (on_off) {
        syslog_switch = DEBUG_LOG_ON;
    } else {
        syslog_switch = DEBUG_LOG_OFF;
    }
    log_config_print_switch(BTHCI, syslog_switch);
    log_config_print_switch(BTL2CAP, syslog_switch);
    log_config_print_switch(BTRFCOMM, syslog_switch);
    log_config_print_switch(BTSPP, syslog_switch);
    log_config_print_switch(BT, syslog_switch);
    log_config_print_switch(hal, syslog_switch);
    log_config_print_switch(atci, syslog_switch);
}

bool bt_hci_log_enabled(void)
{
#ifdef MTK_NVDM_ENABLE
    static bool hci_log_enable_init = false;
    if (hci_log_enable_init == false) {
        bool hci_switch;
        uint32_t hci_log_switch_size = sizeof(bt_demo_hci_log_status);
        nvdm_status_t status = nvdm_read_data_item("hci_log_switch_group", "hci_log_switch_item",
                                                   (uint8_t *)&hci_switch, &hci_log_switch_size);

        if (status == NVDM_STATUS_OK) {
            bt_demo_hci_log_status = hci_switch;
        } else {
            bt_demo_hci_log_status = true;
        }
        hci_log_enable_init = true;

    }
#endif //MTK_NVDM_ENABLE
    return bt_demo_hci_log_status;
}

