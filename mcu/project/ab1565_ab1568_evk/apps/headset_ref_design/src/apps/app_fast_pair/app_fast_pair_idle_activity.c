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
 * File: app_fast_pair_idle_activity.c
 *
 * Description: This file could initialize fast pair, and receive events to update fast pair status.
 *
 * Note: See doc/Airoha_IoT_SDK_Application_Developers_Guide.pdf for fast pair APP.
 *
 */

#include <stdint.h>
#include "FreeRTOS.h"
#include "portable.h"
#include "syslog.h"
#include "bt_fast_pair.h"
#include "apps_events_event_group.h"
#include "apps_config_features_dynamic_setting.h"
#include "app_fast_pair.h"
#include "bt_sink_srv.h"
#include "ui_shell_manager.h"
#include "bt_app_common.h"
#include "bt_device_manager_le.h"
#include "apps_debug.h"
#include "bt_init.h"
#include "bt_gap_le.h"
#include "bt_callback_manager.h"
#include "apps_events_bt_event.h"
#include "multi_ble_adv_manager.h"
#include "multi_va_event_id.h"
#include "app_bt_takeover_service.h"
#include "nvkey.h"
#include "nvdm.h"
#include "nvkey_id_list.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#include "bt_sink_srv_ami.h"
#include "bt_aws_mce_srv.h"
#endif
#ifdef RACE_FIND_ME_ENABLE
#include "app_fm_activity.h"
#endif
#include "apps_aws_sync_event.h"
#ifdef MTK_SMART_CHARGER_ENABLE
#include "app_smcharger_idle_activity.h"
#else
#include "app_battery_transient_activity.h"
#endif
#include "apps_events_battery_event.h"
#include "battery_management.h"
#include "nvdm_id_list.h"
#include "nvkey_id_list.h"
#include "bt_customer_config.h"
#include "verno.h"
#include "atci.h"

#define LOG_TAG     "[app_fast_pair] "
#define APP_ONLY_ALLOW_TAKE_OVER_BY_FAST_PAIR       (false) /* When it's true, only allow BT take over by fast pair flow. */

#define APP_FAST_PAIR_DISCOVER_MODE_ADV_INTERVAL    (0xA0)  /* The adv interval when BT is discoverable. 0xA0 means 100ms */
#define APP_FAST_PAIR_NONDISCOVER_MODE_ADV_INTERVAL (0x190) /* The adv interval when BT is not discoverable. 0x190 means 250ms */
#define REMAINING_BATTERY_TIME_WHEN_FULL            (240)   /* The remaining battery time in minutes when battery is 100%. */

/* Customer can store 10 model_set data in NVkey and there is a seleceted_set to indicate which model set is useful. */
#define APP_FAST_PAIR_MODEL_SET_MAX_NUMBER          (10)

#define FAST_PAIR_PRIVATE_PROTECTED     /* If defined, the module id is encrypted and stored in NVKEY. */
#ifdef FAST_PAIR_PRIVATE_PROTECTED

/* Encrypt and decrypt key, it's must as same as the "Private Key" in Page "Encryption_NvKey" of config Tool.
 * The config tool while use the same key to encrypt fast pair related nvdm. FW can use the same private key to decrypt it.
*/
const char app_fast_pair_nvdm_key[32] = {0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, \
                                         0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, \
                                         0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78, \
                                         0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78
                                        };
#endif

#ifdef MTK_AWS_MCE_ENABLE
#define APP_FAST_PAIR_COMPONENT_NUM                 (3)         /* 3 component means left, right and charger case */
#define APP_FAST_PAIR_AWS_CONTEXT_SYNC_ID           (0xE0)      /* The event id of event group EVENT_GROUP_UI_SHELL_BT_FAST_PAIR */
#else
#define APP_FAST_PAIR_COMPONENT_NUM                 (1)         /* 1 component means headset itself. */
#endif

/* If Fast pair related nvkey is invalid, will use below config. */
#define APP_FAST_PAIR_DEFAULT_MAX_ACCOUNT           (5)         /* Maximum google account. */
#define APP_FAST_PAIR_DEFAULT_TX_POWER_LEVEL        (-20)       /* TX Power level set in fast pair adv data. -20 means -20db. */
#define APP_FAST_PAIR_AIROHA_TEST_MODEL_ID          (0xB2DC3E)  /* Default model ID. It's useful if customer not set any model set in config tool. */
#ifdef MTK_AWS_MCE_ENABLE
#define APP_FAST_PAIR_PERSONALIZED_NAME             "Airoha_fast_pair_earbuds"      /* Default name. Because we use EDR name as default, it's not useful now. */
#else
#define APP_FAST_PAIR_PERSONALIZED_NAME             "Airoha_fast_pair_headset"      /* Default name. Because we use EDR name as default, it's not useful now. */
#endif

/* When there is not set any model id, use the default private key. */
#define APP_FAST_PAIR_AIROHA_TEST_PRIVATE_KEY       {0XDD, 0XE2, 0X5B, 0X48, 0X20, 0X8B, 0X6D, 0X34, \
                                                     0X6E, 0X42, 0X76, 0X91, 0X9E, 0X9D, 0X17, 0XC8, \
                                                     0X6C, 0X40, 0X99, 0XA0, 0XAA, 0XBF, 0X4B, 0X4F, \
                                                     0X99, 0XD7, 0X86, 0XB3, 0X0B, 0X04, 0X1B, 0X80}

#define APP_FAST_PAIR_COMPONENT_LEFT                (0) /* In fast pair spec, the battery value of left earbud is bit 0. */
#define APP_FAST_PAIR_COMPONENT_RIGHT               (1) /* In fast pair spec, the battery value of right earbud is bit 1. */
#define APP_FAST_PAIR_COMPONENT_CASE                (2) /* In fast pair spec, the battery value of charger case is bit 2. */

#define APP_FAST_PAIR_PERSONALIZED_NAME_SIZE        (128)   /* The size of maximum size of personalized name. */

typedef struct {
    uint8_t tone;
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE 
    uint8_t peer_tone;
#endif
} app_fast_pair_find_me_state_t;

/**
 *  @brief This structure defines the data format of NVKEYID_APP_FAST_PAIR_CONFIGURE.
 */
typedef struct {
    uint8_t     fps_enable;             /**< True means fast pair service feature is enable. */
    uint8_t     max_account;            /**< Maximum google account to support. */
    uint8_t     seleceted_set;          /**< Which model set is selected. */
    uint8_t     tx_power_available;     /**< True means set Tx power value in Adv data. */
    int8_t      tx_power_level;         /**< The value should be smaller than the Tx power value when level the device 1 meter. */
    uint8_t     component_num;          /**< The count of the components. In AWS project, it should be 3(left, right and charger case). */
    char        personalized_name[APP_FAST_PAIR_PERSONALIZED_NAME_SIZE];    /* The personalized name which is set in config tool. It's not changed when device is configured a personalized devcie by smart phone or do factory reset. */
    uint8_t     reserved[64];           /**< Reserved data. */
} PACKED app_fast_pair_nvkey_t;

/**
 *  @brief This structure defines the format of a model set. After the model is registered, Google will distribute a Model ID and Anti-Spoofing Public/Private Key Pair.
 */
typedef struct {
    uint32_t    model_id;           /**< The model id of the model. */
    uint8_t     private_key[32];    /**< The private key of the model. */
} PACKED app_fast_pair_set_t;

/**
 *  @brief This structure defines the format of a model stored in NVkey, it's the encrypted data from model set and app_fast_pair_nvdm_key.
 */
typedef struct {
    app_fast_pair_set_t sets[APP_FAST_PAIR_MODEL_SET_MAX_NUMBER];   /**< The encrypted data. */
    uint32_t            CRC;                                        /**< The CRC of the encrypted data. */
} PACKED app_fast_pair_protected_t;

#define APP_FAST_PAIR_PROTECTED_CRY_LEN             (sizeof(app_fast_pair_protected_t)) /* The size to read NVkey. */

/**
 *  @brief This structure defines the format of the context of the activity.
 */
typedef struct {
    bool                            is_vis_on;          /* When it's true, smart phone should display the notification. */
    app_fast_pair_nvkey_t           nvkey;              /* The configuration in NVKEYID_APP_FAST_PAIR_CONFIGURE, refer to app_fast_pair_nvkey_t. */
    bt_fast_pair_battery_t          battery;            /* The battery status, refer to bt_fast_pair_battery_t. */
    bt_fast_pair_account_key_list_t *account_key_list;  /* The paired google account. */
    bt_gap_le_advertising_handle_t  adv_handle;         /* The value indicate which advertising handle is for fast pair */
#if APP_ONLY_ALLOW_TAKE_OVER_BY_FAST_PAIR
    bool                            waiting_connect;    /* When it's true, means waiting EDR connection */
#endif
#ifdef AIR_TILE_ENABLE
    bool                            is_classic_bt_on;      /* When it's true, gfp can broadcast adv */
#endif
} app_fast_pair_local_context_t;

static app_fast_pair_local_context_t g_app_fast_pair_context;   /* The variable records context */

#ifdef RACE_FIND_ME_ENABLE
static app_fast_pair_find_me_state_t s_app_fast_pair_find_me_state;
#endif

#ifdef FAST_PAIR_PRIVATE_PROTECTED
/**
* @brief      This function decrypt data by a private key.
* @param[in]  p_key, the private key.
* @param[in]  p_in, the input data, it is encrypted.
* @param[in]  len, the length of p_in. Note: the buffer of the p_out should be larger than (len - 4). 4 is the size of CRC.
* @param[in]  p_out, the buffer of decrypted output data.
* @return     -1 means CRC fail, > 0 means the output length.
*/
extern int  nvkey_decrypt(unsigned char *p_key, unsigned char *p_in, int len, unsigned char *p_out);
#endif

/**
 * @brief      The ATCI cmd handler, refer to at_cmd_hdlr_fp.
 * @param[in]  parse_cmd, The value is defined in #atci_parse_cmd_param_t. This parameter is given by the ATCI
 *             parser to indicate the input command data to be transferred to the command handler.
 * @return     ATCI_STATUS_OK means success, otherwise means fail.
 */
static atci_status_t _fast_pair_test_cmd_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
    char *cmd_params = parse_cmd->string_ptr + parse_cmd->name_len + 1; /* At the position behind '='. */
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            if (0 == memcmp(cmd_params, "Silence_mode,", strlen("Silence_mode,"))) {
                bool enable = *(cmd_params + strlen("Silence_mode,")) == '1' ? true : false;
                bt_fast_pair_set_silence_mode(NULL, enable);
            }
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        }
        default:
            break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

/* The atci command table. */
static atci_cmd_hdlr_item_t fast_pair_test_atci_cmd[] = {
    {
        .command_head = "AT+FASTPAIRTEST",    /**< Test fast pair */
        .command_hdlr = _fast_pair_test_cmd_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
};

/**
* @brief      This function read nvkey to get fast pair configuration.
* @param[in]  context, the context of the app.
* @param[in]  context, the pointer of the a fast_pair_protected data buffer.
* @return     0 means success, -1 means parameter error, -2 means read NVkey fail.
*/
static int8_t app_fast_pair_load_configure_info(app_fast_pair_local_context_t *context, app_fast_pair_protected_t *protected_set)
{
    nvkey_status_t status;
    if (NULL == context) {
        APPS_LOG_MSGID_E(LOG_TAG" wrongly parameter : 0x%x", 1, context);
        return -1;
    }

    uint32_t size = sizeof(app_fast_pair_nvkey_t);
    if (NVKEY_STATUS_OK != (status = nvkey_read_data(NVKEYID_APP_FAST_PAIR_CONFIGURE, (uint8_t *)&(context->nvkey), &size))) {
        APPS_LOG_MSGID_E(LOG_TAG" load configure data failed : %d", 1, status);
        return -2;
    }
    APPS_LOG_MSGID_I(LOG_TAG" load configure data size %d", 1, size);
    APPS_LOG_DUMP_I(LOG_TAG" load configure data", &(context->nvkey), size);

    size = APP_FAST_PAIR_PERSONALIZED_NAME_SIZE;
    if (NVDM_STATUS_OK == nvdm_read_data_item(NVDM_GROUP_FAST_PAIR_APP, NVDM_FAST_PAIR_PERSONALIZED_NAME,
                                              (uint8_t *)context->nvkey.personalized_name, &size)) {
        APPS_LOG_MSGID_I(LOG_TAG"read name from PERSONALIZED_NAME", 0);
    }

    uint8_t *cry = (uint8_t *)pvPortMalloc(APP_FAST_PAIR_PROTECTED_CRY_LEN);
    if (cry) {
        memset(cry, 0, APP_FAST_PAIR_PROTECTED_CRY_LEN);
    }
    size = APP_FAST_PAIR_PROTECTED_CRY_LEN;
    if (NULL == cry || NVKEY_STATUS_OK != (status = nvkey_read_data(NVKEYID_APP_FAST_PAIR_PROTECTED, cry, &size))) {
        APPS_LOG_MSGID_E(LOG_TAG" load protected data failed : %d", 1, status);
        if (cry) {
            vPortFree(cry);
        }
        return -2;
    }
    APPS_LOG_MSGID_I(LOG_TAG" load protected data size %d", 1, size);
    APPS_LOG_DUMP_I(LOG_TAG" load protected data-before cry", cry, size);
#ifdef FAST_PAIR_PRIVATE_PROTECTED
    /* Decrypt the model set. */
    int ret = nvkey_decrypt((unsigned char *)app_fast_pair_nvdm_key, cry, APP_FAST_PAIR_PROTECTED_CRY_LEN, (unsigned char *) protected_set);
    APPS_LOG_MSGID_I(LOG_TAG" load configure decry ret %d", 1, ret);
    APPS_LOG_DUMP_I(LOG_TAG" load protected data-after cry", protected_set, sizeof(app_fast_pair_protected_t));
#else
    memcpy(protected_set, cry, sizeof(app_fast_pair_protected_t));
#endif
    vPortFree(cry);
    return 0;
}

/**
* @brief      This function read nvkey to get paired accounts.
* @param[in]  context, the context of the app.
*/
static void app_fast_pair_load_account_key(app_fast_pair_local_context_t *context)
{
    nvdm_status_t status;
    uint8_t saved_max_number = context->nvkey.max_account;
    uint32_t size = sizeof(bt_fast_pair_account_key_list_t) +
                    sizeof(bt_fast_pair_account_key_t) * saved_max_number - sizeof(bt_fast_pair_account_key_t);
    memset(context->account_key_list, 0, size);
    status = nvdm_read_data_item(NVDM_GROUP_FAST_PAIR_APP, NVDM_GROUP_FAST_PAIR_ACCOUNT_KEY, (uint8_t *)(context->account_key_list), &size);
    if (NVDM_STATUS_OK != status) {
        context->account_key_list->max_key_number = context->nvkey.max_account;
        APPS_LOG_MSGID_E(LOG_TAG" account key load failed : %d", 1, status);
        return;
    }
    APPS_LOG_MSGID_I(LOG_TAG" account key load num %d, need num %d", 2, context->account_key_list->max_key_number, saved_max_number);
}

/**
* @brief      This function write nvkey to update paired accounts.
* @param[in]  account_key, the array of account_key.
*/
static void app_fast_pair_store_account_key(bt_fast_pair_account_key_list_t *account_key)
{
    nvdm_status_t status;
    uint32_t size = sizeof(bt_fast_pair_account_key_list_t) +
                    sizeof(bt_fast_pair_account_key_t) * account_key->max_key_number - sizeof(bt_fast_pair_account_key_t);

    APPS_LOG_MSGID_E(LOG_TAG" Account key write nvdm size: %d", 1, size);
    status = nvdm_write_data_item(NVDM_GROUP_FAST_PAIR_APP, NVDM_GROUP_FAST_PAIR_ACCOUNT_KEY, NVDM_DATA_ITEM_TYPE_RAW_DATA,
                                  (uint8_t *)(account_key), size);
    if (NVDM_STATUS_OK != status) {
        APPS_LOG_MSGID_E(LOG_TAG" Account key write nvdm failed : %x", 1, status);
        return;
    }
}

/**
* @brief      This function is the callback function to implement, refer to get_ble_adv_data_func_t.
* @param[in]  adv_data, a structure need to fill data in the callback function.
*/
static uint32_t app_fast_pair_get_adv_data(multi_ble_adv_info_t *adv_data)
{
    bt_fast_pair_advertising_data_t type;
    bt_fast_pair_status_t ret;
    uint16_t interval;

    if (g_app_fast_pair_context.is_vis_on) {
        type = BT_FAST_PAIR_ADVERTISING_DATA_MODEL_ID;
        interval = APP_FAST_PAIR_DISCOVER_MODE_ADV_INTERVAL;
    } else {
        type = BT_FAST_PAIR_ADVERTISING_DATA_ACCOUNT_AND_BATTERY;
        if (g_app_fast_pair_context.battery.ui_show) {
            interval = APP_FAST_PAIR_NONDISCOVER_MODE_ADV_INTERVAL;
        } else {
            interval = APP_FAST_PAIR_DISCOVER_MODE_ADV_INTERVAL;
        }
    }

    if (adv_data->adv_param) {
        bt_app_common_generate_default_adv_data(adv_data->adv_param, NULL, NULL, NULL, 0);
        adv_data->adv_param->primary_advertising_interval_min = interval;
        /* Interval should be no larger than 100ms when discoverable */
        adv_data->adv_param->primary_advertising_interval_max = interval;
        bt_bd_addr_t peer_addr[1] = {
            {0, 0, 0, 0, 0, 0}
        };
        uint8_t count = 1;
        bt_device_manager_le_get_bonded_list(peer_addr, &count);
        if (0 == count) {
            bt_gap_le_bonding_info_t bond_info;
            bt_app_common_generate_random_address(peer_addr[0]);
            memset(&bond_info, 0, sizeof(bond_info));
            bond_info.identity_addr.address.type = BT_ADDR_RANDOM;
            memcpy(bond_info.identity_addr.address.addr, peer_addr, sizeof(peer_addr));
            bond_info.key_security_mode = BT_GAP_LE_SECURITY_BONDED_MASK;
            memcpy(bond_info.local_key.identity_info.irk, bt_app_common_get_ble_local_irk(), sizeof(bond_info.local_key.identity_info.irk));
            memset(&bond_info.identity_info.irk, 0xFF, sizeof(bond_info.identity_info.irk));
            bt_device_manager_le_set_bonding_info_by_addr(&(bond_info.identity_addr.address), &bond_info);
            adv_data->adv_param->peer_address = bond_info.identity_addr.address;
            adv_data->adv_param->own_address_type = BT_ADDR_RANDOM_IDENTITY;
            APPS_LOG_MSGID_I(LOG_TAG" create RPA address %02X:%02X:%02X:%02X:%02X:%02X", 6,
                bond_info.identity_addr.address.addr[0],
                bond_info.identity_addr.address.addr[1],
                bond_info.identity_addr.address.addr[2],
                bond_info.identity_addr.address.addr[3],
                bond_info.identity_addr.address.addr[4],
                bond_info.identity_addr.address.addr[5]);
        } else {
            bt_gap_le_bonding_info_t *bonded_info = NULL;
            bonded_info = bt_device_manager_le_get_bonding_info_by_addr(&peer_addr[0]);
            if (bonded_info != NULL) {
                adv_data->adv_param->peer_address = bonded_info->identity_addr.address;
                adv_data->adv_param->own_address_type = BT_ADDR_RANDOM_IDENTITY;
                APPS_LOG_MSGID_I(LOG_TAG" use RPA address %02X:%02X:%02X:%02X:%02X:%02X", 6,
                    bonded_info->identity_addr.address.addr[0],
                    bonded_info->identity_addr.address.addr[1],
                    bonded_info->identity_addr.address.addr[2],
                    bonded_info->identity_addr.address.addr[3],
                    bonded_info->identity_addr.address.addr[4],
                    bonded_info->identity_addr.address.addr[5]);
            }
        }
    }

    if (adv_data->adv_data) {
        uint8_t data_len = adv_data->adv_data->data_length > 0xFF ? 0xFF : adv_data->adv_data->data_length;
        /* Call API in middleware to get adv data. */
        ret = bt_fast_pair_get_advertising_data(adv_data->adv_data->data, &data_len, type);
        if (ret != BT_FAST_PAIR_STATUS_SUCCESS) {
            APPS_LOG_MSGID_E(LOG_TAG" adv get data failed %d", 1, ret);
            return 0;
        } else {
            adv_data->adv_data->data_length = data_len;
        }
    }
    if (adv_data->scan_rsp) {
        adv_data->scan_rsp->data_length = 0;
    }

    return 0;
}

/**
* @brief      This function update the fast pair adv.
* @param[in]  local_context, the context of the app.
*/
static void app_fast_pair_trigger_advertising(app_fast_pair_local_context_t *local_context)
{
#ifdef AIR_TILE_ENABLE
    if (!g_app_fast_pair_context.is_classic_bt_on) {
        APPS_LOG_MSGID_I(LOG_TAG"check classic bt on status before trigger adv %d", 1, g_app_fast_pair_context.is_classic_bt_on);
        return;
    }
#endif
    /* For re-start ADV if connected by unexpected devices */
    APPS_LOG_MSGID_I(LOG_TAG" Trigger advertising visibility status: %d", 1, g_app_fast_pair_context.is_vis_on);

    if (apps_config_features_is_mp_test_mode()) {
        return;
    }
#if 0
    if (bt_sink_srv_get_connected_devices_number() >= 1) {
        /* Once connected one ACL link, then don't start ADV to avoid
         * conflict with other BLE module. */
        APPS_LOG_MSGID_W("[APP_FAST_PAIR]ACL link already exited: %d", 1, is_vis_on);
        return;
    }
#endif
    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_FAST_PAIR, app_fast_pair_get_adv_data);
    multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_FAST_PAIR, app_fast_pair_get_adv_data, 1);
    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_FAST_PAIR);
}

#ifdef AIR_SPOT_ENABLE
static uint32_t app_fast_pair_spot_get_adv_data(multi_ble_adv_info_t *adv_data)
{
    bt_fast_pair_advertising_data_t type;
    bt_fast_pair_status_t ret;
    uint16_t interval;

    if (g_app_fast_pair_context.is_vis_on) {
        type = BT_FAST_PAIR_ADVERTISING_DATA_MODEL_ID;
        interval = APP_FAST_PAIR_DISCOVER_MODE_ADV_INTERVAL;
    } else {
        type = BT_FAST_PAIR_ADVERTISING_DATA_ACCOUNT_AND_BATTERY;
        interval = APP_FAST_PAIR_NONDISCOVER_MODE_ADV_INTERVAL;
    }

    if (adv_data->adv_param) {
        bt_app_common_generate_default_adv_data(adv_data->adv_param, NULL, NULL, NULL, 0);
        adv_data->adv_param->primary_advertising_interval_min = interval;
        /* Interval should be no larger than 100ms when discoverable */
        adv_data->adv_param->primary_advertising_interval_max = interval;
    }

    if (adv_data->adv_data != NULL) {
        uint8_t *buf = adv_data->adv_data->data;
        uint16_t *len = &adv_data->adv_data->data_length;

        bool ret = bt_fast_pair_get_spot_adv_data(FALSE, buf, len);
        if (!ret) {
            APPS_LOG_MSGID_E(LOG_TAG"[SPOT] gen adv data failed", 1, 0);
            adv_data->adv_data->data_length = 0;
        }

        APPS_LOG_MSGID_I(LOG_TAG"[SPOT] get adv data, data_length=%d eid=%02X:%02X:%02X:%02X:%02X",
                         6, adv_data->adv_data->data_length, buf[8], buf[9], buf[10], buf[11], buf[12]);
    }
    if (adv_data->scan_rsp) {
        adv_data->scan_rsp->data_length = 0;
    }

    return 0;
}

static void app_fast_pair_spot_adv_callback(bt_fast_pair_spot_adv_event event)
{
    APPS_LOG_MSGID_I(LOG_TAG"[SPOT] adv_callback, event=0x%04X", 1, event);
    // Need to switch to APP task
    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BT_FAST_PAIR,
                        event, NULL, 0, NULL, 0);
}

static void app_fast_pair_spot_enable_advertising(bool enable)
{
    APPS_LOG_MSGID_I(LOG_TAG"[SPOT] enable_advertising, enable=%d", 1, enable);
    if (enable) {
    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_FAST_PAIR, app_fast_pair_spot_get_adv_data);
    multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_FAST_PAIR, app_fast_pair_spot_get_adv_data, 1);
    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_FAST_PAIR);
    } else {
        multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_FAST_PAIR, app_fast_pair_spot_get_adv_data);
        multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_FAST_PAIR);
    }
}

uint32_t bt_fast_pair_spot_get_eid_clock_callback() {
    /* Notice!!!
     * The customer should implement this function! If your implement support RTC timer, return the RTC timer
     * with seconds. And your RTC timer must support power loss recover. It's means you should backup the
     * RTC timer to the NVDM every a few hours. If your device power loss and the RTC timer cleared, you should
     * use the value in the NVDM to recover it.
     * If you implement it by the system clock, you should keep the clock is count-up. It's means that you
     * should add the system clock to a base counter, you could set the base counter when the product leaves
     * the factory. And storage the sum of base counter and current system clock to the NVDM every time the
     * system power off and read it as the new base counter when the system boot up again.
     */
    return xTaskGetTickCount() / portTICK_PERIOD_MS / 1000;
}

static void app_fast_pair_stop_find_me(void);
static void app_fast_pair_ring_request_handle(bt_fast_pair_ring_request_t *ring);
/* !Notice: this function just for reference, please implement it by your ring design. */
bt_fast_pair_status_t bt_fast_pair_spot_ring_callback(bt_fast_pair_spot_ring_req_type_t req_type, uint8_t *data, uint16_t *remain_time)
{
    static bt_fast_pair_spot_ring_type_t s_current_ring_type = BT_FAST_PAIR_SPOT_RING_TYPE_NONE;
    static uint16_t s_remain_time = 0;

    APPS_LOG_MSGID_I(LOG_TAG"[SPOT] ring req, %d, data: %d, remain_time: %d", 3, req_type, *data, *remain_time);
    switch (req_type) {
        case BT_FAST_PAIR_SPOT_RING_REQ_RING:
            /* Ring req, the *data is ring type */
            s_current_ring_type = *data;
            /* Notice! the time is deciseconds. */
            s_remain_time = *remain_time;
            if (s_current_ring_type == BT_FAST_PAIR_SPOT_RING_TYPE_NONE) {
                /* BT_FAST_PAIR_SPOT_RING_TYPE_NONE means stop ring. */
                s_remain_time = 0;
                /* Every time ringing stops, must call bt_fast_pair_spot_ring_state_update to udpate state.
                 * Because the ringing is stopped by SPOT req, so the state should be BT_FAST_PAIR_SPOT_RING_STOPPED_BY_GATT.
                 * If the ringing is stopped by other reasons, must call this api too.
                 */
                bt_fast_pair_spot_ring_state_update(BT_FAST_PAIR_SPOT_RING_STOPPED_BY_GATT, s_current_ring_type, 0);
                #ifdef MTK_SMART_CHARGER_ENABLE
                app_fast_pair_stop_find_me();
                #endif
            } else {
                bt_fast_pair_ring_request_t ring_request = {{true, true, true}, s_remain_time/10};
                app_fast_pair_ring_request_handle(&ring_request);
            }
            /* Every time ringing starts, must call bt_fast_pair_spot_ring_state_update to udpate state. */
            bt_fast_pair_spot_ring_state_update(BT_FAST_PAIR_SPOT_RING_STA_STARTED, s_current_ring_type, s_remain_time);
            break;
        case BT_FAST_PAIR_SPOT_RING_REQ_GET_RING_CAPABLE:
            /* Get the ring capable. */
            *data = BT_FAST_PAIR_SPOT_RING_CAPABLE_SINGLE;
            break;
        case BT_FAST_PAIR_SPOT_RING_REQ_GET_RING_TYPE:
            /* Get the current ring type and remain time, if current not in ringing, the s_remain_time must be zero. */
            *data = s_current_ring_type;
            *remain_time = s_remain_time;
            break;
    }
    return BT_FAST_PAIR_STATUS_SUCCESS;
}
#endif

/**
* @brief      This function update the battery information.
* @param[in]  context, the context of the app.
* @param[in]  battery, the new battery information.
*/
static void app_fast_pair_update_battery(app_fast_pair_local_context_t *context, bt_fast_pair_battery_t *battery)
{
    if (NULL == context || NULL == battery) {
        return;
    }
    bool need_update = false;
    uint8_t lowest_battery_value = 0xFF; /* The device which is not charging and battery level is lowest. */
    bt_fast_pair_battery_t *cur_cntx = &(context->battery);

    /* When data changed */
    if (cur_cntx->ui_show != battery->ui_show ||
        cur_cntx->battery[APP_FAST_PAIR_COMPONENT_RIGHT].charging != battery->battery[APP_FAST_PAIR_COMPONENT_RIGHT].charging ||
        cur_cntx->battery[APP_FAST_PAIR_COMPONENT_LEFT].charging != battery->battery[APP_FAST_PAIR_COMPONENT_LEFT].charging ||
        cur_cntx->battery[APP_FAST_PAIR_COMPONENT_CASE].charging != battery->battery[APP_FAST_PAIR_COMPONENT_CASE].charging ||
        cur_cntx->battery[APP_FAST_PAIR_COMPONENT_RIGHT].battery_value != battery->battery[APP_FAST_PAIR_COMPONENT_RIGHT].battery_value ||
        cur_cntx->battery[APP_FAST_PAIR_COMPONENT_LEFT].battery_value != battery->battery[APP_FAST_PAIR_COMPONENT_LEFT].battery_value ||
        cur_cntx->battery[APP_FAST_PAIR_COMPONENT_CASE].battery_value != battery->battery[APP_FAST_PAIR_COMPONENT_CASE].battery_value) {
        need_update = true;
        if (0 == battery->battery[APP_FAST_PAIR_COMPONENT_RIGHT].charging) {
            lowest_battery_value = battery->battery[APP_FAST_PAIR_COMPONENT_RIGHT].battery_value;
        }
        if (0 == battery->battery[APP_FAST_PAIR_COMPONENT_LEFT].charging && lowest_battery_value > battery->battery[APP_FAST_PAIR_COMPONENT_LEFT].battery_value) {
            lowest_battery_value = battery->battery[APP_FAST_PAIR_COMPONENT_LEFT].battery_value;
        }
        if (lowest_battery_value != 0xFF) {
            battery->remaining_time = REMAINING_BATTERY_TIME_WHEN_FULL * lowest_battery_value / 100;
        } else {
            battery->remaining_time = BT_FAST_PAIR_REMAINING_TIME_INVALID_VALUE;
        }
    }
    if (true == need_update) {
        context->battery = *battery;
        /* Call middleware API to updata battery information. */
        bt_fast_pair_update_battery(cur_cntx);
        /* Because the battery information is contained in adv data, must update adv once. */
        app_fast_pair_trigger_advertising(context);
    }
}

#ifdef RACE_FIND_ME_ENABLE
static void app_fast_pair_notify_ring_state_change(void)
{
    uint8_t message_buffer[sizeof(bt_fast_pair_message_stream_t)];
    bt_fast_pair_message_stream_t *cont = (bt_fast_pair_message_stream_t *)&message_buffer;
    cont->group_ID = BT_FAST_PAIR_MESSAGE_GROUP_DEVICE_ACTION_EVENT;
    cont->code_ID = BT_FAST_PAIR_MESSAGE_CODE_DEVICE_ACTION_EVENT_RING;
    cont->data_length = 0x01;
    cont->data[0] = 0;
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    if ((BT_AWS_MCE_ROLE_PARTNER | BT_AWS_MCE_ROLE_CLINET) & aws_role) {
        return;
    }
    if (AUDIO_CHANNEL_R == ami_get_audio_channel()) {
        cont->data[0] = (s_app_fast_pair_find_me_state.tone & 0x1) | ((s_app_fast_pair_find_me_state.peer_tone & 0x1) << 1);
    } else {
        cont->data[0] = (s_app_fast_pair_find_me_state.peer_tone & 0x1) | ((s_app_fast_pair_find_me_state.tone & 0x1) << 1);
    }
#else
    cont->data[0] = s_app_fast_pair_find_me_state.tone & 0x1;
#endif
    APPS_LOG_MSGID_I(LOG_TAG" Ringing state change : %x.", 1, cont->data[0]);
    bt_fast_pair_send_message_stream(NULL, cont);
}
#endif

#ifdef MTK_SMART_CHARGER_ENABLE
/**
* @brief      This function send event to stop find me.
*/
static void app_fast_pair_stop_find_me(void)
{
#ifdef RACE_FIND_ME_ENABLE
    app_find_me_param_struct *find_self_param = (app_find_me_param_struct *)pvPortMalloc(sizeof(app_find_me_param_struct));
    find_self_param->blink = 0;
    find_self_param->duration_seconds = 0;
    find_self_param->tone = 0;
    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_FINDME,
                        APP_FIND_ME_EVENT_ID_TRIGGER, find_self_param, sizeof(app_find_me_param_struct), NULL, 0);
#endif
}
#endif

/**
* @brief      This function process the fast pair ring request from smart phone.
* @param[in]  ring, the structure of ring request.
*/
static void app_fast_pair_ring_request_handle(bt_fast_pair_ring_request_t *ring)
{
    bt_fast_pair_acknowledgements_t ack = {
        .action = BT_FAST_PAIR_MESSAGE_CODE_ACKNOWLEDGEMENT_ACK,
        .groupID = BT_FAST_PAIR_MESSAGE_GROUP_DEVICE_ACTION_EVENT,
        .codeID = BT_FAST_PAIR_MESSAGE_CODE_DEVICE_ACTION_EVENT_RING,
        .reason = BT_FAST_PAIR_ACKNOLEGEMENT_NAK_REASON_NOT_SUPPORTED
    };
    APPS_LOG_MSGID_I(LOG_TAG" Recived ring request from remote device R:%d, L:%d, duration:%d",
                     3, ring->ring_status[0], ring->ring_status[1], ring->ring_duration);
    bt_fast_pair_send_acknowledgements(NULL, &ack);
#ifdef RACE_FIND_ME_ENABLE
    app_find_me_param_struct *find_self_param = (app_find_me_param_struct *)pvPortMalloc(sizeof(app_find_me_param_struct));
    if (ring->ring_duration == 0xFF) {
        ring->ring_duration = 0;
    }
    if (NULL == find_self_param) {
        APPS_LOG_MSGID_E(LOG_TAG" Allocate ring req buffer fail", 0);
        return;
    }
    find_self_param->blink = 0;
    find_self_param->duration_seconds = ring->ring_duration;
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    app_find_me_param_struct peer_param = {
        .blink = 0,
        .duration_seconds = ring->ring_duration
    };
    /* Because in the fast pair spec, the component bit define of ring request is different from the bit define of battery value,
       must convert the left and right. */
    uint8_t battery_item = (AUDIO_CHANNEL_R == ami_get_audio_channel() ? APP_FAST_PAIR_COMPONENT_RIGHT : APP_FAST_PAIR_COMPONENT_LEFT);
    if (APP_FAST_PAIR_COMPONENT_RIGHT == battery_item) {
        find_self_param->tone = ring->ring_status[APP_FAST_PAIR_COMPONENT_LEFT];
        peer_param.tone = ring->ring_status[APP_FAST_PAIR_COMPONENT_RIGHT];
    } else {
        find_self_param->tone = ring->ring_status[APP_FAST_PAIR_COMPONENT_RIGHT];
        peer_param.tone = ring->ring_status[APP_FAST_PAIR_COMPONENT_LEFT];
    }
    s_app_fast_pair_find_me_state.peer_tone = peer_param.tone;
    apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_BT_FAST_PAIR,
                                   BT_FAST_PAIR_APP_EVENT_RING_REQUEST, &peer_param, sizeof(peer_param));
#else
    find_self_param->tone = ring->ring_status[0];
#endif
    s_app_fast_pair_find_me_state.tone = find_self_param->tone;
    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_FINDME,
                        APP_FIND_ME_EVENT_ID_TRIGGER, find_self_param, sizeof(app_find_me_param_struct), NULL, 0);
#endif
}

/**
* @brief      This function process additional data from smart phone.
* @param[in]  local_context, context, the context of the app.
* @param[in]  data, the content of the additional data.
*/
static void app_fast_pair_additional_data_handle(app_fast_pair_local_context_t *local_context, bt_fast_pair_additional_data_t *data, uint32_t data_len)
{
    nvdm_status_t status = 0;
    APPS_LOG_MSGID_I(LOG_TAG" Additional data recived data_id %d, data_length %d.", 2, data->data_id, data->data_length);
    /* If the additional data is changing personalized name. */
    if (BT_FAST_PAIR_ADDITIONAL_DATA_ID_PERSONALIZED_NAME == data->data_id) {
        if (data->data_length > sizeof(g_app_fast_pair_context.nvkey.personalized_name) - 1) {
            data->data_length = sizeof(g_app_fast_pair_context.nvkey.personalized_name) - 1;
        }
        memcpy(g_app_fast_pair_context.nvkey.personalized_name, data->data, data->data_length);
        g_app_fast_pair_context.nvkey.personalized_name[data->data_length] = 0;
        if (NVDM_STATUS_OK != (status = nvdm_write_data_item(NVDM_GROUP_FAST_PAIR_APP, NVDM_FAST_PAIR_PERSONALIZED_NAME,
                                                             NVDM_DATA_ITEM_TYPE_RAW_DATA,
                                                             (uint8_t *) & (g_app_fast_pair_context.nvkey.personalized_name),
                                                             APP_FAST_PAIR_PERSONALIZED_NAME_SIZE))) {
            APPS_LOG_MSGID_E(LOG_TAG" PERSONALIZED_NAME write failed : 0x%x", 1, status);
        }
    }
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_BT_FAST_PAIR,
                                   BT_FAST_PAIR_APP_EVENT_ADDITIONAL_DATA, data, data_len);
#endif
}

/**
* @brief      This function process the events of EVENT_GROUP_UI_SHELL_BT_FAST_PAIR group.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool app_fast_pair_app_event_callback(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    assert(NULL != self);
    APPS_LOG_MSGID_I(LOG_TAG" Fast pair app event id:0x%x, len:%d.", 2, event_id, data_len);
    switch (event_id) {
        case BT_FAST_PAIR_APP_EVENT_NEED_STORE_ACCOUNT_KEY: {
            bt_fast_pair_account_key_list_t *real_key_list = g_app_fast_pair_context.account_key_list;
            bt_fast_pair_account_key_list_t *key_list = (bt_fast_pair_account_key_list_t *)extra_data;
            assert(key_list == real_key_list);
            APPS_LOG_MSGID_I(LOG_TAG" Account key update change fast pair data.", 0);
            if (!g_app_fast_pair_context.is_vis_on) {
                app_fast_pair_trigger_advertising(&g_app_fast_pair_context);
            }
            app_fast_pair_store_account_key(key_list);
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            uint32_t size_account = sizeof(bt_fast_pair_account_key_list_t) +
                                    sizeof(bt_fast_pair_account_key_t) * (g_app_fast_pair_context.account_key_list->max_key_number - 1);
            apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_BT_FAST_PAIR,
                                           BT_FAST_PAIR_APP_EVENT_NEED_STORE_ACCOUNT_KEY, g_app_fast_pair_context.account_key_list, size_account);
#endif
            break;
        }
#if APP_ONLY_ALLOW_TAKE_OVER_BY_FAST_PAIR
        case BT_FAST_PAIR_APP_EVENT_PAIR_COMPLETE: {
            g_app_fast_pair_context.waiting_connect = false;
            break;
        }
#endif
        case BT_FAST_PAIR_APP_EVENT_SERVICE_CONNECTED: {
            APPS_LOG_MSGID_I(LOG_TAG" Spp service connected.", 0);
            break;
        }
        case BT_FAST_PAIR_APP_EVENT_SERVICE_DISCONNECTED: {
            APPS_LOG_MSGID_I(LOG_TAG" Spp service disconnected.", 0);
            break;
        }
        case BT_FAST_PAIR_APP_EVENT_RING_REQUEST: {
            app_fast_pair_ring_request_handle(extra_data);
            break;
        }
        case BT_FAST_PAIR_APP_EVENT_MESSAGE_STREAM: {
            APPS_LOG_MSGID_I(LOG_TAG" Message stream recived.", 0);
            break;
        }
        case BT_FAST_PAIR_APP_EVENT_ADDITIONAL_DATA: {
            app_fast_pair_additional_data_handle(&g_app_fast_pair_context, extra_data, data_len);
            break;
        }
        case BT_FAST_PAIR_APP_EVENT_ACTIVE_COMPONENTS_REQ: {
            bt_bd_addr_t *bt_addr = (bt_bd_addr_t *)extra_data;
            uint8_t message_buffer[sizeof(bt_fast_pair_message_stream_t)] = { 0 };
            uint8_t components_active_state = 0x01; /* For headset, 0x01 means active. */
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            components_active_state = 0;
            if (0 == g_app_fast_pair_context.battery.battery[APP_FAST_PAIR_COMPONENT_RIGHT].charging
                    && g_app_fast_pair_context.battery.battery[APP_FAST_PAIR_COMPONENT_RIGHT].battery_value != 0xFF) {
                components_active_state |= 0x1;
            }
            if (0 == g_app_fast_pair_context.battery.battery[APP_FAST_PAIR_COMPONENT_LEFT].charging
                    && g_app_fast_pair_context.battery.battery[APP_FAST_PAIR_COMPONENT_LEFT].battery_value != 0xFF) {
                components_active_state |= 0x1 << 1;
            }
#endif

            bt_fast_pair_message_stream_t *cont = (bt_fast_pair_message_stream_t *)&message_buffer;
            cont->group_ID = BT_FAST_PAIR_MESSAGE_GROUP_DEVICE_INFO_EVENT;
            cont->code_ID = BT_FAST_PAIR_MESSAGE_CODE_DEVICE_INFO_EVENT_ACTIVE_COMPONENTS_RSP;
            cont->data_length = 0x01;
            cont->data[0] = components_active_state;
            APPS_LOG_MSGID_I(LOG_TAG" ACTIVE COMPONENTS_RSP : %x.", 1, cont->data[0]);
            bt_fast_pair_send_message_stream(bt_addr, cont);
            break;
        }
#if APP_ONLY_ALLOW_TAKE_OVER_BY_FAST_PAIR
        case BT_FAST_PAIR_APP_EVENT_PAIR_STARTED: {
            g_app_fast_pair_context.waiting_connect = true;
            break;
        }
#endif
#ifdef AIR_SPOT_ENABLE
        case BT_FAST_PAIR_SPOT_EVENT_START_ADV: {
            // Use last old EID
            app_fast_pair_spot_enable_advertising(TRUE);
            break;
        }

        case BT_FAST_PAIR_SPOT_EVENT_STOP_ADV: {
            app_fast_pair_spot_enable_advertising(FALSE);
            break;
        }

        case BT_FAST_PAIR_SPOT_EVENT_EID_READY: {
            // Use new EID when first generate EID successfully
            app_fast_pair_spot_enable_advertising(TRUE);
            break;
        }

        case BT_FAST_PAIR_SPOT_EVENT_EID_UPDATED: {
            // Use new EID every 1024 sec
            app_fast_pair_spot_enable_advertising(TRUE);
            break;
        }
#endif
        default:
            break;
    }
    return false;
}

#ifdef MTK_SMART_CHARGER_ENABLE
/**
* @brief      This function process the events of EVENT_GROUP_UI_SHELL_CHARGER_CASE group.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool app_fast_pair_proc_charger_case_group(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    uint8_t battery_item = (AUDIO_CHANNEL_R == ami_get_audio_channel() ? APP_FAST_PAIR_COMPONENT_RIGHT : APP_FAST_PAIR_COMPONENT_LEFT);
    bt_fast_pair_battery_t update_battery = g_app_fast_pair_context.battery;

    switch (event_id) {
        case EVENT_ID_SMCHARGER_NOTIFY_PUBLIC_EVENT: {
            app_smcharger_public_event_para_t *event_para = (app_smcharger_public_event_para_t *)extra_data;
            if (event_para->action == SMCHARGER_CHARGER_IN_ACTION) {
                APPS_LOG_MSGID_I(LOG_TAG", SMCharger Charger_in action", 0);
                update_battery.battery[battery_item].charging = 1;
                app_fast_pair_stop_find_me();
            } else if (event_para->action == SMCHARGER_OPEN_LID_ACTION) {
                APPS_LOG_MSGID_I(LOG_TAG", SMCharger Open_lid action", 0);
                update_battery.ui_show = true;
            } else if (event_para->action == SMCHARGER_CLOSE_LID_ACTION) {
                APPS_LOG_MSGID_I(LOG_TAG", SMCharger Close_lid action", 0);
                update_battery.ui_show = false;
            } else if (event_para->action == SMCHARGER_CHARGER_OUT_ACTION) {
                APPS_LOG_MSGID_I(LOG_TAG", SMCharger Charger_out action", 0);
                update_battery.ui_show = true;
                update_battery.battery[battery_item].charging = 0;
                app_fast_pair_stop_find_me();
            } else if (event_para->action == SMCHARGER_CASE_BATTERY_REPORT_ACTION) {
                uint8_t case_battery = (uint8_t)((uint32_t)event_para->data);
                APPS_LOG_MSGID_I(LOG_TAG", BATTERY_LEVEL_REPORT=%d", 1, case_battery);
                update_battery.battery[APP_FAST_PAIR_COMPONENT_CASE].charging = ((case_battery & 0x80) ? 1 : 0);
                update_battery.battery[APP_FAST_PAIR_COMPONENT_CASE].battery_value = (case_battery & 0x7F);
            }
        }
        break;

        default:
            return ret;
    }
    app_fast_pair_update_battery(&g_app_fast_pair_context, &update_battery);
    return ret;
}
#endif

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#ifdef MTK_SMART_CHARGER_ENABLE
/**
* @brief      This function process the aws data of BT_AWS_MCE_REPORT_MODULE_SMCHARGER.
* @param[in]  context, the context of the app.
* @param[in]  report, the aws report data.
*/
static void app_fast_pair_aws_mce_data_smart_charge_callback(app_fast_pair_local_context_t *context,  bt_aws_mce_report_info_t *report)
{
#if 0
    received_charger_exist = (smcharger_context.peer_smcharger_state != STATE_SMCHARGER_OUT_OF_CASE) ? 1 : 0;
    context->battery.battery[battery_item].charging = received_charger_exist;
    if (!(context->battery.battery[APP_FAST_PAIR_COMPONENT_RIGHT].charging) &&
        !(context->battery.battery[APP_FAST_PAIR_COMPONENT_LEFT].charging)) {
        context->battery.ui_show = false;
    } else {
        context->battery.ui_show = true;
    }
    bt_fast_pair_update_battery(&(context->battery));
    app_fast_pair_trigger_advertising(context);
#endif
}
#endif

/**
* @brief      This function process the AWS data of BT_AWS_MCE_REPORT_MODULE_BATTERY.
* @param[in]  context, the context of the app.
* @param[in]  report, the AWS report data.
*/
static void app_fast_pair_aws_mce_data_battery_callback(app_fast_pair_local_context_t *context, bt_aws_mce_report_info_t *report)
{
    uint8_t received_charger_exist = 0;
    uint8_t received_bat_percent = 0;
    uint8_t battery_item = (AUDIO_CHANNEL_R == ami_get_audio_channel() ? APP_FAST_PAIR_COMPONENT_LEFT : APP_FAST_PAIR_COMPONENT_RIGHT);
    bt_fast_pair_battery_t update_battery = context->battery;

#ifdef MTK_SMART_CHARGER_ENABLE
    app_smcharger_context_t smcharger_context;
    memcpy(&smcharger_context.peer_battery_percent, report->param, sizeof(smcharger_context.peer_battery_percent));
    APPS_LOG_MSGID_I(LOG_TAG", [Agent] Received partner_bat_percent = %d", 1, smcharger_context.peer_battery_percent);
    received_bat_percent = smcharger_context.peer_battery_percent & (~ PARTNER_BATTERY_CHARGING);
    received_charger_exist = (smcharger_context.peer_battery_percent & PARTNER_BATTERY_CHARGING) ? 1 : 0;    /* The bit 7 is charger state */
#else
    battery_local_context_type_t battery_context;
    memcpy(&battery_context.partner_battery_percent, report->param, sizeof(battery_context.partner_battery_percent));
    APPS_LOG_MSGID_I(LOG_TAG", [Agent] Received partner_bat_percent = %d", 1, battery_context.partner_battery_percent);
    received_bat_percent = battery_context.partner_battery_percent & (~ PARTNER_BATTERY_CHARGING);
    received_charger_exist = (battery_context.partner_battery_percent & PARTNER_BATTERY_CHARGING) ? 1 : 0;      /* The bit 7 is charger state */
#endif
    update_battery.battery[battery_item].charging = received_charger_exist;
    update_battery.battery[battery_item].battery_value = received_bat_percent;
    app_fast_pair_update_battery(context, &update_battery);
}

/**
* @brief      This function process the AWS data of BT_AWS_MCE_REPORT_MODULE_APP_ACTION, it's fast pair internal AWS events.
* @param[in]  context, the context of the app.
* @param[in]  report, the AWS report data.
*/
static void app_fast_pair_aws_mce_data_app_action_callback(app_fast_pair_local_context_t *context, bt_aws_mce_report_info_t *report)
{
    uint32_t    event_group = 0, event_id = 0, extra_aws_len = 0;
    void        *extra_aws = NULL;
    apps_aws_sync_event_decode_extra(report, &event_group, &event_id, &extra_aws, &extra_aws_len);
    if (EVENT_GROUP_UI_SHELL_BT_FAST_PAIR != event_group || NULL == extra_aws) {
        return;
    }
    switch (event_id) {
        case BT_FAST_PAIR_APP_EVENT_NEED_STORE_ACCOUNT_KEY: {
            /* Received store account key request from agent. */
            bt_fast_pair_account_key_list_t *new_account = (bt_fast_pair_account_key_list_t *)extra_aws;
            if (NULL == new_account || NULL == context->account_key_list ||
                context->account_key_list->max_key_number != new_account->max_key_number
                || extra_aws_len != sizeof(bt_fast_pair_account_key_list_t) +
                                    sizeof(bt_fast_pair_account_key_t) * (g_app_fast_pair_context.account_key_list->max_key_number - 1)) {
                APPS_LOG_MSGID_E(LOG_TAG", Agent(%d) & Partner(%d) max key number are not same! Stop update flow!", 2,
                                 context->account_key_list ? context->account_key_list->max_key_number : 0,
                                 new_account ? new_account->max_key_number : 0);
                return;
            }
            if (memcmp(context->account_key_list, new_account, extra_aws_len)) {
                memcpy(context->account_key_list, new_account, extra_aws_len);
                app_fast_pair_store_account_key(context->account_key_list);
            }
            break;
        }
        case BT_FAST_PAIR_APP_EVENT_RING_REQUEST: {
#ifdef RACE_FIND_ME_ENABLE
            /* Received ring request from agent. */
            if (extra_aws_len == sizeof(app_find_me_param_struct)) {
                app_find_me_param_struct *find_me_param =
                    (app_find_me_param_struct *)pvPortMalloc(sizeof(app_find_me_param_struct));
                if (find_me_param) {
                    memcpy(find_me_param, extra_aws, sizeof(app_find_me_param_struct));
                    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE,
                                        EVENT_GROUP_UI_SHELL_FINDME,
                                        APP_FIND_ME_EVENT_ID_TRIGGER,
                                        find_me_param,
                                        sizeof(app_find_me_param_struct),
                                        NULL, 0);
                }
            }
#endif
            break;
        }
        case BT_FAST_PAIR_APP_EVENT_ADDITIONAL_DATA: {
            /* Received process additional data from agent. */
            bt_fast_pair_additional_data_t *data = (bt_fast_pair_additional_data_t *)extra_aws;
            APPS_LOG_MSGID_I(LOG_TAG" Aws packet additional data recived data_id %d, data_length %d.", 2, data->data_id, data->data_length);
            if (BT_FAST_PAIR_ADDITIONAL_DATA_ID_PERSONALIZED_NAME == data->data_id) {
                nvdm_status_t status = 0;
                bt_fast_pair_update_parameters_t params;
                if (data->data_length > sizeof(context->nvkey.personalized_name) - 1) {
                    data->data_length = sizeof(context->nvkey.personalized_name) - 1;
                }
                memcpy(context->nvkey.personalized_name, data->data, data->data_length);
                context->nvkey.personalized_name[data->data_length] = 0;
                params.tx_power_level = NULL;
                if (0 != context->nvkey.tx_power_available) {
                    params.tx_power_level = &(context->nvkey.tx_power_level);
                }
                params.addr_type = BT_ADDR_RANDOM;
                params.personalized_name = context->nvkey.personalized_name;
                if (NVDM_STATUS_OK != (status = nvdm_write_data_item(NVDM_GROUP_FAST_PAIR_APP, NVDM_FAST_PAIR_PERSONALIZED_NAME,
                                                                     NVDM_DATA_ITEM_TYPE_RAW_DATA,
                                                                     (uint8_t *) &(g_app_fast_pair_context.nvkey.personalized_name),
                                                                     APP_FAST_PAIR_PERSONALIZED_NAME_SIZE))) {
                    APPS_LOG_MSGID_E(LOG_TAG" PERSONALIZED_NAME write failed : 0x%x", 1, status);
                }
                bt_fast_pair_update_parameters(&params);
            }
            break;
        }
        case APP_FAST_PAIR_AWS_CONTEXT_SYNC_ID: {
            app_fast_pair_nvkey_t *nvkey_cntx = (app_fast_pair_nvkey_t *)extra_aws;
            APPS_LOG_MSGID_I(LOG_TAG" Aws cntext sync data recived selected_set %d, component_num %d.", 2, nvkey_cntx->seleceted_set, nvkey_cntx->component_num);
            memcpy(&(context->nvkey), nvkey_cntx, (sizeof(context->nvkey) - sizeof(context->nvkey.reserved)));
        }
        break;
        default:
            break;
    }
}

/**
* @brief      This function process the events of EVENT_GROUP_UI_SHELL_AWS_DATA group.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool app_fast_pair_aws_mce_data_callback(struct _ui_shell_activity *self,  uint32_t event_id, void *extra_data, size_t data_len)
{
    assert(NULL != self);
    bt_aws_mce_report_info_t *report = (bt_aws_mce_report_info_t *)extra_data;
    if (NULL == report) {
        APPS_LOG_MSGID_E(LOG_TAG", report data or context is NULL", 0);
        return false;
    }
    switch (report->module_id) {
        case BT_AWS_MCE_REPORT_MODULE_BATTERY:
            app_fast_pair_aws_mce_data_battery_callback(&g_app_fast_pair_context, report);
            break;
#ifdef MTK_SMART_CHARGER_ENABLE
        case BT_AWS_MCE_REPORT_MODULE_SMCHARGER:
            app_fast_pair_aws_mce_data_smart_charge_callback(&g_app_fast_pair_context, report);
            break;
#endif
        case BT_AWS_MCE_REPORT_MODULE_APP_ACTION:
            app_fast_pair_aws_mce_data_app_action_callback(&g_app_fast_pair_context, report);
            break;
        default:
            APPS_LOG_MSGID_I(LOG_TAG", report moudle id:0x%x not support.", 1, report->module_id);
            return false;
    }
    return false;
}
#endif

/**
* @brief      This function process the events of EVENT_GROUP_UI_SHELL_MULTI_VA group.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool app_fast_pair_multi_va_event_proc(ui_shell_activity_t *self,  uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    switch (event_id) {
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        case MULTI_VA_EVENT_NOTIFY_BLE_ADDR_CHANGED: {
            bt_bd_addr_t random_addr;
            if (multi_ble_adv_manager_get_random_addr_and_adv_handle(MULTI_ADV_INSTANCE_FAST_PAIR, &random_addr, NULL)) {
                APPS_LOG_MSGID_I(LOG_TAG", ble address changed : %x:%x:%x:%x:%x:%x", 6,
                    random_addr[0], random_addr[1], random_addr[2],
                    random_addr[3], random_addr[4], random_addr[5]);
                bt_fast_pair_set_random_ble_address(false, &random_addr);
            }
            break;
        }
#endif
        default:
            break;
    }

    return ret;
}


/**
* @brief      This function to init fast pair.
* @param[in]  self, the context pointer of the activity.
* @return     initialized success or not.
*/
static bool app_fast_pair_init(ui_shell_activity_t *self)
{
    bt_fast_pair_status_t           ret;
    uint32_t    app_fast_pair_model_id = APP_FAST_PAIR_AIROHA_TEST_MODEL_ID;
    uint8_t     app_fast_pair_private_key[32] = APP_FAST_PAIR_AIROHA_TEST_PRIVATE_KEY;
    app_fast_pair_protected_t       *protect_sets;
    bt_bd_addr_t random_addr;
    atci_register_handler(fast_pair_test_atci_cmd, sizeof(fast_pair_test_atci_cmd) / sizeof(atci_cmd_hdlr_item_t));
    protect_sets = (app_fast_pair_protected_t *)pvPortMalloc(sizeof(app_fast_pair_protected_t));
    if (protect_sets != NULL && 0 == app_fast_pair_load_configure_info(&g_app_fast_pair_context, protect_sets)) {
        if (g_app_fast_pair_context.nvkey.seleceted_set >= APP_FAST_PAIR_MODEL_SET_MAX_NUMBER) {
            g_app_fast_pair_context.nvkey.seleceted_set = APP_FAST_PAIR_MODEL_SET_MAX_NUMBER - 1;
        }
        app_fast_pair_model_id = protect_sets->sets[g_app_fast_pair_context.nvkey.seleceted_set].model_id;
        memcpy(app_fast_pair_private_key, &(protect_sets->sets[g_app_fast_pair_context.nvkey.seleceted_set].private_key), 32 * sizeof(uint8_t));
        vPortFree(protect_sets);
        APPS_LOG_MSGID_W(LOG_TAG" init model id 0x%x, max account %d, tx_power_exist %d, tx_power %d, component num %d select %d!",
                         6, app_fast_pair_model_id, g_app_fast_pair_context.nvkey.max_account, g_app_fast_pair_context.nvkey.tx_power_available,
                         g_app_fast_pair_context.nvkey.tx_power_level, g_app_fast_pair_context.nvkey.component_num, g_app_fast_pair_context.nvkey.seleceted_set);
        if (!g_app_fast_pair_context.nvkey.fps_enable) {
            APPS_LOG_MSGID_W(LOG_TAG" init disable!", 0);
            return false;
        }
    } else {
        /* Load fail must stop init process */
        g_app_fast_pair_context.nvkey.fps_enable = 1;
        g_app_fast_pair_context.nvkey.max_account = APP_FAST_PAIR_DEFAULT_MAX_ACCOUNT;
        g_app_fast_pair_context.nvkey.seleceted_set = 0;
        g_app_fast_pair_context.nvkey.tx_power_available = 1;
        g_app_fast_pair_context.nvkey.tx_power_level = APP_FAST_PAIR_DEFAULT_TX_POWER_LEVEL;
        g_app_fast_pair_context.nvkey.component_num = APP_FAST_PAIR_COMPONENT_NUM;
        /* memset(g_app_fast_pair_context.nvkey.personalized_name, 0, sizeof(g_app_fast_pair_context.nvkey.personalized_name)); */
    }
    g_app_fast_pair_context.account_key_list = (bt_fast_pair_account_key_list_t *)pvPortMalloc(sizeof(bt_fast_pair_account_key_list_t) +
                                                                                               (g_app_fast_pair_context.nvkey.max_account - 1) * sizeof(bt_fast_pair_account_key_t));
    if (NULL == g_app_fast_pair_context.account_key_list) {
        APPS_LOG_MSGID_E(LOG_TAG" Init allocate failed", 0);
        return false;
    }
    app_fast_pair_load_account_key(&g_app_fast_pair_context);
    APPS_LOG_MSGID_I(LOG_TAG" Init account ", 0);
    bt_fast_pair_init_parameters_t init_param;
    memset(&init_param, 0, sizeof(init_param));
    init_param.model_id = app_fast_pair_model_id;
    init_param.private_key = app_fast_pair_private_key;
    init_param.tx_power_level = NULL;
    init_param.addr_type = BT_ADDR_RANDOM;
    init_param.component_num = g_app_fast_pair_context.nvkey.component_num;
    init_param.personalized_name = g_app_fast_pair_context.nvkey.personalized_name;
    init_param.firmware_revision = MTK_FW_VERSION;
    if (0 != g_app_fast_pair_context.nvkey.tx_power_available) {
        init_param.tx_power_level = &(g_app_fast_pair_context.nvkey.tx_power_level);
    }
    ret = bt_fast_pair_init(&init_param);
    if (multi_ble_adv_manager_get_random_addr_and_adv_handle(MULTI_ADV_INSTANCE_FAST_PAIR,
                                                             &random_addr,
                                                             &(g_app_fast_pair_context.adv_handle))) {
        bt_fast_pair_set_random_ble_address(false, &random_addr);
    }

    g_app_fast_pair_context.battery.ui_show = false;
    g_app_fast_pair_context.battery.component_num = APP_FAST_PAIR_COMPONENT_NUM;
    g_app_fast_pair_context.battery.remaining_time = BT_FAST_PAIR_REMAINING_TIME_INVALID_VALUE;
    for (uint8_t i = 0; i < APP_FAST_PAIR_COMPONENT_NUM; i++) {
        g_app_fast_pair_context.battery.battery[i].charging = 0;
        g_app_fast_pair_context.battery.battery[i].battery_value = 0xFF;
    }
    if (ret != BT_FAST_PAIR_STATUS_SUCCESS) {
        APPS_LOG_MSGID_E(LOG_TAG" fast pair service init failed %d", 1, ret);
        vPortFree(g_app_fast_pair_context.account_key_list);
        g_app_fast_pair_context.account_key_list = NULL;
        return false;
    }
    return true;
}

#if APP_ONLY_ALLOW_TAKE_OVER_BY_FAST_PAIR
static bool app_fast_pair_bt_takeover_allow(bt_bd_addr_t device_addr)
{
    return g_app_fast_pair_context.waiting_connect;
}
#endif

/**
* @brief      This function process the events of EVENT_GROUP_UI_SHELL_SYSTEM group.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool app_proc_fast_pair_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true; /* UI shell internal event must process by this activity, so default is true. */
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
#ifdef AIR_SPOT_ENABLE
            bt_fast_pair_spot_set_adv_callback(app_fast_pair_spot_adv_callback);
#endif
            memset(&g_app_fast_pair_context, 0, sizeof(app_fast_pair_local_context_t));
#if APP_ONLY_ALLOW_TAKE_OVER_BY_FAST_PAIR
            app_bt_takeover_service_user_register(APP_BT_TAKEOVER_ID_FASTPAIR, app_fast_pair_bt_takeover_allow);
#endif
            ret = app_fast_pair_init(self);
            if (ret == true) {
                /* BT event is not suitable for UI shell scheme.
                * Instead, choose bt callback manager to handle bt related event. */
#if SUPPORT_ROLE_HANDOVER_SERVICE
                uint8_t battery_item = (AUDIO_CHANNEL_R == ami_get_audio_channel() ? APP_FAST_PAIR_COMPONENT_RIGHT : APP_FAST_PAIR_COMPONENT_LEFT);
#else
                uint8_t battery_item = 0;
#endif
                bt_fast_pair_battery_t update_battery = g_app_fast_pair_context.battery;
                update_battery.battery[battery_item].charging = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);
                update_battery.battery[battery_item].battery_value = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
#ifdef MTK_SMART_CHARGER_ENABLE
                uint8_t case_battery = app_smcharger_read_case_battery_nvkey();
                update_battery.battery[APP_FAST_PAIR_COMPONENT_CASE].charging = ((case_battery & 0x80) ? 1 : 0);
                update_battery.battery[APP_FAST_PAIR_COMPONENT_CASE].battery_value = (case_battery & 0x7F);
#endif
                app_fast_pair_update_battery(&g_app_fast_pair_context, &update_battery);
                extern void bt_fast_pair_at_cmd_init();
                bt_fast_pair_at_cmd_init();
            }
            APPS_LOG_MSGID_I(LOG_TAG" activity: %d", 1, ret);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            if (g_app_fast_pair_context.account_key_list) {
                vPortFree(g_app_fast_pair_context.account_key_list);
                g_app_fast_pair_context.account_key_list = NULL;
            }
            APPS_LOG_MSGID_I(LOG_TAG" destory", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            break;
        }
        default:
            break;
    }
    return ret;
}

/**
* @brief      This function process the events of EVENT_GROUP_UI_SHELL_BATTERY group.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool app_fast_pair_proc_battery_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    assert(NULL != self);
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    uint8_t battery_item = (AUDIO_CHANNEL_R == ami_get_audio_channel() ? APP_FAST_PAIR_COMPONENT_RIGHT : APP_FAST_PAIR_COMPONENT_LEFT);
#else
    uint8_t battery_item = 0;
#endif
    bt_fast_pair_battery_t update_battery = g_app_fast_pair_context.battery;
    switch (event_id) {
        case APPS_EVENTS_BATTERY_PERCENT_CHANGE: {
            APPS_LOG_MSGID_I(LOG_TAG"APPS_EVENTS_BATTERY_PERCENT_CHANGE : %d", 1, (uint32_t)extra_data);
            update_battery.battery[battery_item].battery_value = (uint32_t)extra_data;
            break;
        }
        case APPS_EVENTS_BATTERY_CHARGER_EXIST_CHANGE: {
#if 0
            APPS_LOG_MSGID_I(LOG_TAG"APPS_EVENTS_BATTERY_CHARGER_EXIST_CHANGE : %d", 1, (uint32_t)extra_data);
            g_app_fast_pair_context.battery.battery[battery_item].charging = (uint32_t)extra_data;
            if (!(g_app_fast_pair_context.battery.battery[APP_FAST_PAIR_COMPONENT_RIGHT].charging) &&
                !(g_app_fast_pair_context.battery.battery[APP_FAST_PAIR_COMPONENT_LEFT].charging)) {
                g_app_fast_pair_context.battery.ui_show = false;
            } else {
                g_app_fast_pair_context.battery.ui_show = true;
            }
#endif
        }
            return ret;
        default:
            return ret;
    }
    app_fast_pair_update_battery(&g_app_fast_pair_context, &update_battery);
    return ret;
}

static bool app_fast_pair_proc_bt_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        case BT_GAP_LE_RPA_ROTAION_IND: {
            bt_gap_le_rpa_rotation_ind_t *rotation = NULL;
            apps_bt_event_data_t *bt_event_data = (apps_bt_event_data_t *)extra_data;
            if (bt_event_data != NULL) {
                rotation = (bt_gap_le_rpa_rotation_ind_t *)bt_event_data->buffer;
            }
            if (rotation != NULL) {
                bt_gap_le_advertising_handle_t adv_handle = 0;
                multi_ble_adv_manager_get_random_addr_and_adv_handle(MULTI_ADV_INSTANCE_FAST_PAIR,
                                                                 NULL,
                                                                 &adv_handle);
                APPS_LOG_MSGID_W(LOG_TAG"BT_GAP_LE_RPA_ROTAION_IND, handle %d?=%d, %x:%x:%x:%x:%x:%x", 8,
                    rotation->handle, adv_handle,
                    rotation->address[0], rotation->address[1], rotation->address[2],
                    rotation->address[3], rotation->address[4], rotation->address[5]);
                if (rotation->handle == adv_handle) {
                    bt_fast_pair_set_random_ble_address(true, &(rotation->address));
                }
            }
            break;
        }
        default:
            break;
    }
    return ret;
}

/**
* @brief      This function process the events of EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER group.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool app_fast_pair_proc_bt_cm_group(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    /* APPS_LOG_MSGID_I("[APP_FAST_PAIR], bt cm event_id : 0x%x", 1, event_id); */
#ifdef MTK_AWS_MCE_ENABLE
    /* Only handle bt cm related event in Agent role */
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    if ((BT_AWS_MCE_ROLE_PARTNER | BT_AWS_MCE_ROLE_CLINET) & aws_role) {
        APPS_LOG_MSGID_W("[APP_FAST_PAIR] Don't init on wrongly role: %d", 1, aws_role);
        return ret;
    }
#endif
    switch (event_id) {
        case BT_CM_EVENT_VISIBILITY_STATE_UPDATE: {
            bt_cm_visibility_state_update_ind_t *visible_update = (bt_cm_visibility_state_update_ind_t *)extra_data;
            g_app_fast_pair_context.is_vis_on = visible_update->visibility_state;
            APPS_LOG_MSGID_I("[APP_FAST_PAIR] Visibilty update chage fast pair data.", 0);
            app_fast_pair_trigger_advertising(&g_app_fast_pair_context);
#ifdef AIR_SPOT_ENABLE
            bt_fast_pair_spot_update_pairing_mode_state(visible_update->visibility_state);
#endif
        }
        break;
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            bt_cm_profile_service_mask_t profile_srv = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP) | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK);
            if ((remote_update->pre_connected_service & profile_srv) && !(remote_update->connected_service & profile_srv)) {
                /* For fast pair service add workaround, if HFP and A2DP all disconnected, then we need disconnected fast pair service actively. */
                /* bt_fast_pair_terminate(&(remote_update->address)); */
                bt_cm_connect_t disconnect_param;
                memcpy(disconnect_param.address, remote_update->address, sizeof(bt_bd_addr_t));
                disconnect_param.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_FAST_PAIR);
                bt_cm_disconnect(&disconnect_param);
            }
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            if (BT_CM_ACL_LINK_CONNECTED <= remote_update->acl_state &&
                !(remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)) &&
                (remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                uint32_t size_account = sizeof(bt_fast_pair_account_key_list_t) +
                                        sizeof(bt_fast_pair_account_key_t) * (g_app_fast_pair_context.account_key_list->max_key_number - 1);
                /* Only agent role will enable this module, so don't need judge the role
                           * in event handle. */
                APPS_LOG_MSGID_I("[APP_FAST_PAIR]AWS attached.", 0);
                apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_BT_FAST_PAIR,
                                               BT_FAST_PAIR_APP_EVENT_NEED_STORE_ACCOUNT_KEY, g_app_fast_pair_context.account_key_list, size_account);
                apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_BT_FAST_PAIR,
                                               APP_FAST_PAIR_AWS_CONTEXT_SYNC_ID, &(g_app_fast_pair_context.nvkey), sizeof(g_app_fast_pair_context.nvkey) - sizeof(g_app_fast_pair_context.nvkey.reserved));
            } else if ((remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)) &&
                !(remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                uint8_t peer_battery_item = (AUDIO_CHANNEL_R != ami_get_audio_channel() ? APP_FAST_PAIR_COMPONENT_RIGHT : APP_FAST_PAIR_COMPONENT_LEFT);
                bt_fast_pair_battery_t update_battery = g_app_fast_pair_context.battery;
                update_battery.battery[peer_battery_item].battery_value = 0xFF;
                app_fast_pair_update_battery(&g_app_fast_pair_context, &update_battery);
            }
#endif
            break;
        }
        default:
            break;
    }
    return ret;
}

/**
* @brief      This function process the events of EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER group.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool app_fast_pair_proc_bt_dm_group(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    bt_device_manager_power_event_t evt;
    bt_device_manager_power_status_t status;
    bt_event_get_bt_dm_event_and_status(event_id, &evt, &status);
    switch (evt) {
        case BT_DEVICE_MANAGER_POWER_EVT_ACTIVE_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status && !apps_config_features_is_mp_test_mode()) {
                APPS_LOG_MSGID_I("[APP_FAST_PAIR] Power on trigger fast pair advertising.", 0);
#ifdef AIR_TILE_ENABLE
                g_app_fast_pair_context.is_classic_bt_on = true;
                APPS_LOG_MSGID_I(LOG_TAG"classic bt on %d", 1, g_app_fast_pair_context.is_classic_bt_on);
#endif
                app_fast_pair_trigger_advertising(&g_app_fast_pair_context);
            }
            break;
        }
#ifdef AIR_TILE_ENABLE
        case BT_DEVICE_MANAGER_POWER_EVT_CLASSIC_ACTIVE_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status && !apps_config_features_is_mp_test_mode()) {
                g_app_fast_pair_context.is_classic_bt_on = true;
                APPS_LOG_MSGID_I(LOG_TAG"Classic power on trigger fast pair advertising, classic bt on %d", 1, g_app_fast_pair_context.is_classic_bt_on);
                app_fast_pair_trigger_advertising(&g_app_fast_pair_context);
            }
            break;
        }
#endif
        case BT_DEVICE_MANAGER_POWER_EVT_STANDBY_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status && !apps_config_features_is_mp_test_mode()) {
                g_app_fast_pair_context.is_vis_on = false;
                multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_FAST_PAIR, app_fast_pair_get_adv_data);
            }
            break;
        }
        default:
            break;
    }

    return ret;
}

#ifdef MTK_AWS_MCE_ENABLE
/**
* @brief      This function process the events of EVENT_GROUP_UI_SHELL_AWS group.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool app_fast_pair_proc_aws_event(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        case BT_AWS_MCE_SRV_EVENT_AIR_PAIRING_COMPLETE:
            break;
        default:
            break;
    }
    return ret;
}
#endif


/**
* @brief      This function process the events of EVENT_GROUP_UI_SHELL_APP_INTERACTION.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/

bool app_fast_pair_proc_app_interaction_event(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    switch(event_id) {
#ifdef AIR_TILE_ENABLE
        case APPS_EVENTS_INTERACTION_STOP_FAST_PAIR_ADV: {
            app_fast_pair_stop_advertising();
            g_app_fast_pair_context.is_classic_bt_on = false;
            APPS_LOG_MSGID_I(LOG_TAG"APPS_EVENTS_INTERACTION_STOP_FAST_PAIR_ADV, classic bt on %d", 1, g_app_fast_pair_context.is_classic_bt_on);
            break;
        }
#endif
#ifdef RACE_FIND_ME_ENABLE
        case APPS_EVENTS_INTERACTION_NOTIFY_FIND_ME_STATE: {
            const app_find_me_context_t *fm_context = app_find_me_idle_activity_get_context();
            if (fm_context->tone != s_app_fast_pair_find_me_state.tone
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
                || fm_context->peer_tone != s_app_fast_pair_find_me_state.peer_tone
#endif
            ) {
                s_app_fast_pair_find_me_state.tone = fm_context->tone;
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
                s_app_fast_pair_find_me_state.peer_tone = fm_context->peer_tone;
#endif
                app_fast_pair_notify_ring_state_change();
            }
            break;
        }
#endif
        default:
            break;
    }
    return ret;
}

bool app_fast_pair_idle_activity_proc(struct _ui_shell_activity *self, uint32_t event_group, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    /* APPS_LOG_MSGID_I("[APP_FAST_PAIR]APP_FAST_PAIR_IDLE_ACT, event_group : %x, evt: %x", 2, event_group, event_id); */
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = app_proc_fast_pair_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BATTERY: {
            ret = app_fast_pair_proc_battery_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT:
            ret = app_fast_pair_proc_bt_group(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            /* Trigger fast pair behavior */
            ret = app_fast_pair_proc_bt_cm_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_DEVICE_MANAGER: {
            /* Trigger fast pair behavior */
            ret = app_fast_pair_proc_bt_dm_group(self, event_id, extra_data, data_len);
            break;
        }
#ifdef MTK_AWS_MCE_ENABLE
        case EVENT_GROUP_UI_SHELL_AWS:
            ret = app_fast_pair_proc_aws_event(self, event_id, extra_data, data_len);
            break;
#endif
#ifdef MTK_SMART_CHARGER_ENABLE
        case EVENT_GROUP_UI_SHELL_CHARGER_CASE:
            ret = app_fast_pair_proc_charger_case_group(self, event_id, extra_data, data_len);
            break;
#endif
        case EVENT_GROUP_UI_SHELL_BT_FAST_PAIR: {
            ret = app_fast_pair_app_event_callback(self, event_id, extra_data, data_len);
            break;
        }
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        case EVENT_GROUP_UI_SHELL_AWS_DATA: {
            /* Just group currently */
            ret = app_fast_pair_aws_mce_data_callback(self, event_id, extra_data, data_len);
            break;
        }
#endif
        case EVENT_GROUP_UI_SHELL_MULTI_VA:
            ret = app_fast_pair_multi_va_event_proc(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            ret = app_fast_pair_proc_app_interaction_event(self, event_id, extra_data, data_len);
            break;

        default:
            break;
    }

    /* APPS_LOG_MSGID_I("[APP_FAST_PAIR]APP_FAST_PAIR_IDLE_ACT, ret : %x", 1, ret); */
    return ret;
}

/**
 * @brief     The implement of weak symbol function that returns the account key list buffer to fast pair module.
 * @return    A pointer to the account key buffer containing the account key buffer numbers and account key buffer. The pointer should not be NULL and should point to a global variable.
 */
bt_fast_pair_account_key_list_t *bt_fast_pair_get_account_key_list()
{
    return g_app_fast_pair_context.account_key_list;
}

void app_fast_pair_set_tx_power_level(int8_t tx_power)
{
    nvkey_status_t status = 0;
    app_fast_pair_nvkey_t temp_nvkey;
    uint32_t nvkey_size = sizeof(app_fast_pair_nvkey_t);
    if (NVKEY_STATUS_OK != (status = nvkey_read_data(NVKEYID_APP_FAST_PAIR_CONFIGURE,
                                                     (uint8_t *)&temp_nvkey, &nvkey_size))) {
        APPS_LOG_MSGID_E("load configure failed : 0x%x", 1, status);
        return;
    }
    g_app_fast_pair_context.nvkey.tx_power_level = tx_power;
    temp_nvkey.tx_power_level = tx_power;
    if (NVKEY_STATUS_OK != (status = nvkey_write_data(NVKEYID_APP_FAST_PAIR_CONFIGURE,
                                                      (uint8_t *)&temp_nvkey, sizeof(app_fast_pair_nvkey_t)))) {
        APPS_LOG_MSGID_E("load configure write failed : 0x%x", 1, status);
    }
}

void app_fast_pair_stop_advertising(void)
{
    APPS_LOG_MSGID_I(LOG_TAG" stop_advertising", 0);

    if (apps_config_features_is_mp_test_mode()) {
        return;
    }

    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_FAST_PAIR, app_fast_pair_get_adv_data);
    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_FAST_PAIR);
}


bool app_fast_pair_get_is_waiting_connect(void)
{
#if APP_ONLY_ALLOW_TAKE_OVER_BY_FAST_PAIR
    return g_app_fast_pair_context.waiting_connect;
#else
    return true;
#endif
}
