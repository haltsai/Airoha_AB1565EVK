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

#include "syslog.h"

#if !defined(MTK_DEBUG_LEVEL_NONE)
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_001[] = LOG_ERROR_PREFIX(BT_FAST_PAIR) "bt_fast_pair_get_execute_write_result, error evt %02X";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_002[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_key_based_pairing_write_char_callback, rw is %d, size is %d, offset is %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_003[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "size %d exceed the maximum size %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_004[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "unsupport rw type %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_005[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_passkey_write_char_callback:rw is %d,size is %d, offset is %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_006[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_account_key_write_char_callback:rw is %d, size is %d, offset is %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_007[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_additional_data_write_char_callback:rw is %d, size is %d, offset is %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_008[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_model_id_read_char_callback:rw is %d,size is %d, offset is %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_009[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_key_based_pairing_characteristic_send resp:0x%x, status:0x%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_010[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_passkey_characteristic_send key:0x%x, status:0x%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_011[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_additional_data_characteristic_send data:0x%x, status:0x%x";

    ATTR_LOG_STRING_LIB BT_FAST_PAIR_012[] = LOG_ERROR_PREFIX(BT_FAST_PAIR) "bt_fast_pair_app_event_callback must be implemented";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_013[] = LOG_ERROR_PREFIX(BT_FAST_PAIR) "bt_fast_pair_get_account_key_list must be implemented";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_014[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_data_block_print: %02X %02X %02X %02X";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_015[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_data_block_print: %02X %02X %02X";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_016[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_data_block_print: %02X %02X";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_017[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_data_block_print: %02X";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_018[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "USER CONFIRM IND value : %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_019[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "LE disconnect IND status %02X, reason %02X";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_020[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_decrypt_pairing_requset !!!";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_021[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_decrypt_pairing_requset fail, mesg:%02X, flags:%02X";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_022[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_decrypt_pairing_requset encrypt block:";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_023[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_decrypt_pairing_requset key block:";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_024[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_decrypt_pairing_requset decrypt block:";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_025[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_decrypt_pairing_requset provider addr:%02X:%02X:%02X:%02X:%02X:%02X";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_026[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_decrypt_pairing_requset seeker addr:%02X:%02X:%02X:%02X:%02X:%02X";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_027[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_decrypt_pairing_requset random addr:%02X:%02X:%02X:%02X:%02X:%02X";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_028[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_interupt_pairing !!!";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_029[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "gpfs pairing not start !!!";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_030[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_interupt_pairing fail status: %x !!!";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_031[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_key_based_pairing_handle !!!";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_032[] = LOG_ERROR_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_key_based_pairing_handle ecdh256 fail !!!";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_033[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "uECC_shared_secret fail public key :";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_034[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "uECC_shared_secret fail private key :";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_035[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_key_based_pairing_handle aes key can't decryped";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_036[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "uECC_shared_secret public key :";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_037[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "uECC_shared_secret private key :";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_038[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "uECC_shared_secret dh key :";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_039[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "uECC_shared_secret dh key sha 256:";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_040[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_key_based_pairing_handle account key decryped fail num: %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_041[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_key_based_pairing_handle account key buffer is null";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_042[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_key_based_pairing_handle decryped response";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_043[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_passkey_handle !!!";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_044[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_passkey_handle decryped failed, mesg:%02X, passkey:%d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_045[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_passkey_handle decryped mesg0 passkey %06X";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_046[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_account_key_handle start !!!";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_047[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_excute_bloom_filter salt_size:%d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_048[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_excute_bloom_filter account key buffer is null";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_049[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_event_handler conn_handle %04X, evt %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_050[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_event_handler unkown evt !!!";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_051[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_get_advertising_data data_type %d, buffer length %d buffer 0x%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_052[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_get_advertising_data buffer can't be null";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_053[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_get_advertising_data buffer length not enough";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_054[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_get_advertising_data buffer%x,%x,%x,%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_055[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_056[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_get_advertising_data 2 buffer%x,%x,%x,%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_057[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_058[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_get_advertising_data 3 buffer%x,%x,%x,%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_059[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_get_advertising_data data type[%d] invalid";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_060[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_init init_param can't be null";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_061[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_init model_id 0x%x, private_key 0x%x, tx_power_level 0x%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_062[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_init private key can't be null";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_064[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_model_id_read_char_callback, unsupport rw type %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_065[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_get_characteristic_value_callback, type %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_066[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_get_characteristic_value_callback, unknown type";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_067[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_send_additional_data length %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_068[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_recevied_additional_data";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_069[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "fast pair spp event id:0x%x, status:0x%x, buffer:0x%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_070[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "fast pair spp connected handle:0x%x, local server id:0x%x, addr:0x%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_071[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "fast pair bt action fail status 0x%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_072[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "fast pair spp reject spp had connected !!!";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_073[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "fast pair spp reject no resource !!!";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_074[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "fast pair spp disconnected handle:0x%x, find device:0x%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_075[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "fast pair spp data received handle:0x%x, packet length:%d, packet:0x%x, spp_dev:0x%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_076[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "fast pair message stream received group id:0x%x, code id:0x%x, data_length:%d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_077[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "fast pair unsupport device action !!!";    
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_078[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "fast pair send message stream group ID :0x%x, code ID :0x%x, data length :%d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_079[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "Send address 0x%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_080[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_send_companion_app_events event %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_081[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_set_silence_mode addr 0x%x, enable %d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_082[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_update_bettary";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_083[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_optional_init";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_084[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "fast pair optionnal init fail status 0x%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_085[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt fast pair gap event rpa rotation handle 0x%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_086[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "bt_fast_pair_core_account_key_handle wrong account key";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_087[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt fast pair gap event rpa rotation, address 0x%02x:%02x:%02x:%02x:%02x:%02x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_088[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "fast pair key based pairing message type:0x%x, flags:0x%x";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_089[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "message stream received position:%d, all_len:%d, recived_len:%d";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_090[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "bt_fast_pair_firmware_revision_callback:rw is %d,size is %d, offset is %d";
#else /* !defined(MTK_DEBUG_LEVEL_NONE) */
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_001[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_002[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_003[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_004[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_005[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_006[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_007[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_008[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_009[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_010[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_011[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_012[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_013[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_014[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_015[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_016[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_017[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_018[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_019[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_020[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_021[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_022[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_023[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_024[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_025[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_026[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_027[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_028[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_029[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_030[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_031[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_032[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_033[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_034[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_035[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_036[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_037[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_038[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_039[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_040[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_041[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_042[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_043[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_044[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_045[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_046[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_047[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_048[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_049[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_050[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_051[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_052[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_053[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_054[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_055[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_056[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_057[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_058[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_059[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_060[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_061[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_062[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_063[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_064[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_065[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_066[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_067[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_068[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_069[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_070[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_071[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_072[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_073[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_074[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_075[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_076[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_077[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_078[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_079[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_080[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_081[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_082[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_083[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_084[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_085[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_086[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_087[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_088[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_089[] = "";
    ATTR_LOG_STRING_LIB BT_FAST_PAIR_090[] = "";
#endif /* !defined(MTK_DEBUG_LEVEL_NONE) */

ATTR_LOG_STRING_LIB SPOT_MALLOC_FAIL[] = LOG_ERROR_PREFIX(BT_FAST_PAIR) "[SPOT]malloc fail";
ATTR_LOG_STRING_LIB SPOT_WRIE_EPH_KEY_FAIL[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "[SPOT]write eph key fail, %d";
ATTR_LOG_STRING_LIB SPOT_READ_DEF_EPH_KEY_FAIL[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "[SPOT]read default eph key fail, %d";
ATTR_LOG_STRING_LIB SPOT_READ_INIT_COUNTER_FAIL[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "[SPOT]read init counter failed: %d";
ATTR_LOG_STRING_LIB SPOT_WRITE_INIT_COUNTER_FAIL[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "[SPOT]write init counter failed: %d";
ATTR_LOG_STRING_LIB SPOT_SEND_NOTIFY_FAIL[] = LOG_WARNING_PREFIX(BT_FAST_PAIR) "[SPOT]send notify failed: %x";
ATTR_LOG_STRING_LIB SPOT_EPH_UPDATE[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT]eph key update";
ATTR_LOG_STRING_LIB SPOT_EPH_KEY_CLEAN[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT]eph key clean";;
ATTR_LOG_STRING_LIB SPOT_RING_STA_UPDATE[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT]ring sta update: 0x%x, 0x%x, 0x%x";
ATTR_LOG_STRING_LIB SPOT_RING_TYPE[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT]ring type: 0x%x";
ATTR_LOG_STRING_LIB SPOT_RING_STA[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT]ring sta: 0x%x, 0x%x, 0x%x";
ATTR_LOG_STRING_LIB SPOT_TIME_TO_UPDATE_COUNTER[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT]time to update counter";
ATTR_LOG_STRING_LIB SPOT_RESONSE_ERROR_CODE[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT]response with error code: 0x%x";
ATTR_LOG_STRING_LIB SPOT_CCCD_WRITE[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT]spot cccd write: 0x%x";
ATTR_LOG_STRING_LIB SPOT_START_TIMER_FAIL[] = LOG_ERROR_PREFIX(BT_FAST_PAIR) "[SPOT]timer start fail";
ATTR_LOG_STRING_LIB SPOT_SAVE_TIMER_CYCLE[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT]init the save timer with %d";

ATTR_LOG_STRING_LIB SPOT_LOG_SET_ADVERTISING[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] set advertising, enable=%d";
ATTR_LOG_STRING_LIB SPOT_LOG_GET_ADV_DATA_FAIL1[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] get_adv_data, fail - parameter error";
ATTR_LOG_STRING_LIB SPOT_LOG_GET_ADV_DATA_FAIL2[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] get_adv_data, fail - eid null";
ATTR_LOG_STRING_LIB SPOT_LOG_GET_ADV_DATA_RE_CAL[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] get_adv_data, re_calculate counter=%d eid=%02X:%02X:%02X:%02X:%02X";
ATTR_LOG_STRING_LIB SPOT_LOG_EID_TIMER_CALLBACK[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] eid_timer_callback, pre_calculate_timer=%d is_adv_triggered=%d";
ATTR_LOG_STRING_LIB SPOT_LOG_EID_TIMER_CALLBACK_NO_NEED_UPDATE_RESTART_TIMER[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] eid_timer_callback, no need to notify_eid_updated and restart timer";
ATTR_LOG_STRING_LIB SPOT_LOG_EID_TIMER_CALLBACK_UPDATED[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] eid_timer_callback, updated eid=%02X:%02X:%02X:%02X:%02X";
ATTR_LOG_STRING_LIB SPOT_LOG_EID_TEMP_TASK_START[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] eid_temp_task, first=%d counter=%d->%d sec";
ATTR_LOG_STRING_LIB SPOT_LOG_EID_TEMP_TASK_START_FAIL[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] eid_temp_task, start fail - not_advertising";
ATTR_LOG_STRING_LIB SPOT_LOG_EID_TEMP_TASK_FAIL[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] eid_temp_task, gen fail";
ATTR_LOG_STRING_LIB SPOT_LOG_EID_TEMP_TASK_EID[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] eid_temp_task, first=%d running_time=%d (%d sec) eid=%02X:%02X:%02X:%02X:%02X";
ATTR_LOG_STRING_LIB SPOT_LOG_START_EID_TIMER[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] start_eid_timer, ret=%d period_ms=%d (%d sec)";
ATTR_LOG_STRING_LIB SPOT_LOG_START_EID_TIMER_FAIL[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] start_eid_timer, fail - not_advertising";
ATTR_LOG_STRING_LIB SPOT_LOG_WRITE_EID_NVKEY[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] write_eid_nvkey, status=%d eid=%02X:%02X:%02X:%02X:%02X";
ATTR_LOG_STRING_LIB SPOT_LOG_READ_EID_NVKEY[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] read_eid_nvkey, status=%d read_len=%d eid=%02X:%02X:%02X:%02X:%02X";
ATTR_LOG_STRING_LIB SPOT_LOG_GET_EID_IS_VALID[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] get_eid, is_valid=%d";
ATTR_LOG_STRING_LIB SPOT_LOG_ADVERTISING[] = LOG_INFO_PREFIX(BT_FAST_PAIR) "[SPOT] advertising, adv_enable=%d->%d adv_event_cb=0x%08X eid=%02X:%02X:%02X:%02X:%02X";

