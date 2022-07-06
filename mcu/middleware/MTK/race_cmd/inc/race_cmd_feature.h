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


#ifndef __RACE_CMD_FEATURE_H__
#define __RACE_CMD_FEATURE_H__

#ifdef MTK_RACE_CMD_ENABLE

#ifdef MTK_FOTA_VIA_RACE_CMD
#include "fota_util.h"
#endif
#include "hal_feature_config.h"


#ifdef MTK_RACE_DUAL_CMD_ENABLE
/* Enable LPCOMM */
#define RACE_LPCOMM_ENABLE
#ifdef RACE_LPCOMM_ENABLE
#define RACE_LPCOMM_RETRY_ENABLE
// #define RACE_LPCOMM_MULTIPLE_LINK_ENABLE
#endif

/* Enable aws related feature in race_cmd. To simplify, this macro is depend on
 * MTK_RACE_DUAL_CMD_ENABLE. However, if dual cmd can be based on other protocol such as
 * UART, SPP or BLE, this dependency should be removed.
 */
#ifdef MTK_AWS_MCE_ENABLE
#define RACE_AWS_ENABLE
#endif

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
/* Uncomment to support COSYS. */
#define RACE_COSYS_ENABLE
#endif

/* When enabled, the high 4 bits of packet_type of race_lpcomm_packet_struct is reused as the sender role.
 * When both earbuds, naming earbud A and earbud B, can connect to two different remote devices separately, such as a PC
 * and a smartphone(SP), the sender role is needed for the REQ and RSP exchange between the earbud A and the earbud B.
 * Take the following connections as an example.
 * PC <-> earbud A <-> earbud B <-> SP
 * If the exchange is for the remote device that connects with the device directly, the device works as an Agent for the exchange.
 * Otherwise it works as a Partner. For the same REQ or RSP, the Agent and the Partner handle them differently. For different
 * exchanges, the same device may work as an Agent and a Partner at the same time.
 * The sender role is the role of the device that sends the REQ or the RSP in a particular exchange. It is contained in the REQ and
 * the RSP (high 4 bits of packet_type). On receiving the REQ or the RSP, the device will check the sender role, decide the role of
 * itself for the current exchange and deliver the REQ or the RSP to the proper handler.
 */
#define RACE_LPCOMM_SENDER_ROLE_ENABLE
#endif

/* Enable STORAGE_CMD */
#define RACE_STORAGE_CMD_ENABLE

/* Enable BLUETOOTH_CMD */
#define RACE_BT_CMD_ENABLE

/* Enable CAPTOUCH_CMD */
#ifdef HAL_CAPTOUCH_MODULE_ENABLED
#define RACE_CAPTOUCH_CMD_ENABLE
#endif

/* Enable COMMIN Register read/write */
#define RACE_RG_READ_WRITE_ENABLE

/* Enable STORAGE_CMD for BSP_FLASH */
#ifdef RACE_STORAGE_CMD_ENABLE
#ifdef MTK_RACE_BSP_FLASH_SUPPORT
#define RACE_STORAGE_BSP_FLASH_ENABLE
#endif
#endif

/* Enable FOTA_CMD */
#ifdef MTK_FOTA_VIA_RACE_CMD
#define RACE_FOTA_CMD_ENABLE
#ifdef RACE_FOTA_CMD_ENABLE
#define RACE_FOTA_SPP_ENABLE
#define RACE_FOTA_BLE_ENABLE

/* RACE_FOTA_DELAY_COMMIT_ENABLE is supposed to enable sending commit cmd at any time after FOTA
  * download is done. However, it does not work, so do not enable it. FOTA DL state is changed to
  * RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT when the state is 0x03 11 0x 03 11. But it's not
  * correct when the FOTA package is downloaded completely last time without committing. Because of
  * FOTA resumming, the device does not know the exact WAIT_FOR_COMMIT moment. SP need send cmd to
  * tell the device. There's no such requirement, so just disable it instead of adding new race cmd.
  */
//#define RACE_FOTA_DELAY_COMMIT_ENABLE

/* Define this macro to support to cancel FOTA during RHO. However, it's not useful actually for normal cases.
 * But RHO takes very little time and it's not a big problem for the chance to cancel FOTA during RHO is very rare.
 * If Agent cancells FOTA during RHO, FOTA stop procedure will only be executed after RHO. By then Agent
 * changes to Partner and Partner resets itself without telling new Agent about it. At this time, SP sends START_CMD
 * and new Agent will send START REQ to new Partner, and Partner will start FOTA again! FOTA is not cancelled!!!
 * Only if Partner tells Agent about FOTA stop requirement, will this be useful.
 */
//#define RACE_FOTA_STOP_DURING_RHO_SUPPORT

/* FOTA package is stored in the external flash. */
#ifdef MTK_FOTA_STORE_IN_EXTERNAL_FLASH
#define RACE_FOTA_STORE_IN_EXTERNAL_FLASH
#endif

#define RACE_FOTA_ACTIVE_MODE_ENABLE

#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
/* Do not disable this macro for it may cause a BT reset in some remote devices.
 * When this macro is enabled,
 * 1) When active fota is running and there's a call incomming, active fota will be stopped.
 * 2) When there's a call ongoing, starting the active fota will be rejected.
 * If it's disabled,
 * 1) Whenever active fota is started, HFP will be disconnected.
 * 2) When there's a call ongoing, starting the active fota will be rejected.
 */
#define RACE_FOTA_ACTIVE_MODE_KEEP_HFP

/* When this macro is enabled, pause A2DP instead of disconnecting A2DP during the active FOTA. */
#define RACE_FOTA_ACTIVE_MODE_KEEP_A2DP

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
/* Uncomment to support acitve mode using ULL dongle.
  * Because of the limited bandwidth, when using ULL dongle, FOTA data cannot be sent with chat or audio
  * data at the same time. The priority is FOTA > chat/gaming. When there is chat on-going/comming or
  * gaming audio playing/comming, chat/gaming audio will be paused before or after FOTA starts.
  */
#define RACE_FOTA_ACTIVE_MODE_ULL_SUPPORT
#endif

#ifdef AIR_BLE_AUDIO_DONGLE_ENABLE
/* Uncomment to support acitve mode using LE AUDIO dongle.
  *when using LE AUDIO dongle in DFU FOTA active mode,The priority is FOTA > chat/gaming,When there is
  *chat on-going/comming or gaming audio playing/comming, chat/gaming audio will be paused
  *after FOTA starts.
  */
#define RACE_FOTA_DFU_LE_ACTIVE_MODE_SUPPORT
#endif
#endif

#if defined(AB156X)
#define RACE_FOTA_ADAPTIVE_MODE_ENABLE
#endif

#if (PRODUCT_VERSION == 2822 || defined(AB156X))
#ifdef RACE_LPCOMM_ENABLE
/* When the fota package file is large, it will take seconds to finish the integrity check. The retry inteval
  * between agent and partner is RACE_LPCOMM_RETRY_TIMEOUT_IN_MS. If the process time is more than
  * the retry interval, the integrity check request will be resent and more time will be consumed. When this
  * macro is enabled, if the preivous integrity check request is not finished in partner side, the new arrived
  * request will be dropped. No change in agent integrity check process, because SP will resend the integrity
  * check cmd to Agent after 9s, which is long enough to process the cmd.
  */
#define RACE_FOTA_SMART_INTEGRITY_CHECK

#define RACE_FOTA_CANCEL_FROM_PARTNER_ENABLE
#endif

/* SP can check the FOTA package integrity of the Partner via IF packet without RHO. */
#define RACE_FOTA_INTEGRITY_CHECK_ENHANCE_ENABLE
#endif
#endif /* RACE_FOTA_CMD_ENABLE */
#endif

/* Enable CTRL_BASEBAND_CMD */
#define RACE_CTRL_BASEBAND_CMD_ENABLE

/* Enable DSP_REALTIME_CMD */
#define RACE_DSP_REALTIME_CMD_ENABLE

/* Enable NVDM_CMD */
#ifdef MTK_NVDM_ENABLE
#define RACE_NVDM_CMD_ENABLE
#endif

//#define RACE_BT_EVENT_MSG_HDL
#ifdef RACE_BT_EVENT_MSG_HDL
#define RACE_BT_EVENT_REGISTER_ENABLE
#endif

/* Enable HOSTAUDIO_CMD */
#define RACE_HOSTAUDIO_CMD_ENABLE

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#define RACE_ROLE_HANDOVER_SERVICE_ENABLE
#if defined(RACE_ROLE_HANDOVER_SERVICE_ENABLE) && defined(BT_ROLE_HANDOVER_WITH_SPP_BLE)
#define RACE_RHO_WITHOUT_SMARTPHONE_DISCONNECT_ENABLE
#endif
#endif

#ifdef MTK_BOOTREASON_CHECK_ENABLE
#define RACE_BOOTREASON_CMD_ENABLE
#endif /* MTK_BOOTREASON_CHECK_ENABLE */

//#ifdef MTK_SAVE_LOG_TO_FLASH_ENABLE
#define RACE_OFFLINE_LOG_CMD_ENABLE
//#endif

#if !defined(MTK_DEBUG_PLAIN_LOG_ENABLE) && defined(MTK_MUX_ENABLE) && !defined(MTK_DEBUG_LEVEL_PRINTF)
#define RACE_SYSLOG_CMD_ENABLE
#endif

#if (PRODUCT_VERSION == 2822 || defined(AB156X))
#define RACE_VERSION_CODE_CMD_ENABLE
#ifdef HAL_I2C_MASTER_MODULE_ENABLED
#define RACE_I2C_MASTER_CMD_ENABLE
#endif
#ifdef HAL_SPI_MASTER_MODULE_ENABLED
#define RACE_SPI_MASTER_CMD_ENABLE
#endif
#endif

#ifdef MTK_ONLINE_LOG_TO_APK_ENABLE
#define RACE_ONLINE_LOG_CMD_ENABLE
#endif

/* Enable RACE_RELAY_CMD */
#ifdef MTK_RACE_RELAY_CMD_ENABLE
#define RACE_RELAY_CMD_ENABLE
#else
#define RACE_DUMMY_RELAY_CMD_ENABLE
#endif

#define RACE_ROFS_CMD_ENABLE

#if (PRODUCT_VERSION == 2822 || defined(AB156X))
#define MTK_RACE_EVENT_ID_ENABLE
#endif

#ifdef AIR_CFU_ENABLE
#define RACE_CFU_ENABLE
#ifdef AIR_CFU_BUILDER_ENABLE
#define RACE_CFU_BUILDER_ENABLE
#else
#define RACE_CFU_HANDLER_ENABLE
#endif
#endif

// TODO: undefine following macros for final products
/******************************Start Temp macro for UT/IT/Workaround******************************/
//#define RACE_BT_BATTERY_FAKE_VALUE
/******************************End Temp macro for UT/IT/Workaround******************************/

#endif /* MTK_RACE_CMD_ENABLE */

/* Feature dependency check */
#if defined(RACE_FOTA_CMD_ENABLE) && \
    (!defined(RACE_STORAGE_CMD_ENABLE) || \
     !defined(RACE_BT_CMD_ENABLE) || \
     !defined(RACE_CTRL_BASEBAND_CMD_ENABLE))
#error "Enable RACE_STORAGE_CMD_ENABLE and RACE_BT_CMD_ENABLE and RACE_CTRL_BASEBAND_CMD_ENABLE to support RACE_FOTA_CMD_ENABLE"
#endif

#if defined(RACE_ROLE_HANDOVER_SERVICE_ENABLE) && !defined(RACE_AWS_ENABLE)
#error "Enable RACE_AWS_ENABLE to support RACE_ROLE_HANDOVER_SERVICE_ENABLE"
#endif

#if defined(RACE_ROLE_HANDOVER_SERVICE_ENABLE) && !defined(RACE_LPCOMM_ENABLE)
#error "RHO is enabled while RACE_LPCOMM_ENABLE is not!"
#endif

#if defined(RACE_LPCOMM_ENABLE) && !defined(RACE_AWS_ENABLE) && !defined(RACE_COSYS_ENABLE)
#error "RACE_LPCOMM_ENABLE is defined with AWS & COSYS not supported!"
#endif

#endif /* __RACE_CMD_FEATURE_H__ */

