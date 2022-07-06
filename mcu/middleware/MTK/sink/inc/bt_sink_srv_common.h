/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __BT_SINK_SRV_COMMON_H__
#define __BT_SINK_SRV_COMMON_H__

#include "bt_sink_srv.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MTK_AUDIO_SYNC_ENABLE
#define BT_SINK_SRV_A2DP_VOLUME_TYPE (0x01)
#define BT_SINK_SRV_CALL_VOLUME_TYPE (0x02)
#ifdef AIR_A2DP_SYNC_STOP_ENABLE
#define BT_SINK_SRV_MUSIC_STOP_TYPE  (0x03)
#endif
#define BT_SINK_SRV_VP_SYNC_TYPE     (0x04)
typedef uint8_t bt_sink_srv_sync_feature_t;

#define BT_SINK_SRV_SYNC_SUCCESS    (0x00000000)
#define BT_SINK_SRV_SYNC_TIMEOUT    (0x00000002)
typedef uint32_t bt_sink_srv_sync_status_t;
#endif

//#define BT_SINK_ENABLE_CALL_LOCAL_RINGTONE

#define SINK_MODULE_MASK_OFFSET(value) (1<<((value & BT_SINK_MODULE_MASK) >> BT_SINK_MODULE_OFFSET)) /**< To get the module offset. */
#define SINK_MODULE_MASK_COMMON    SINK_MODULE_MASK_OFFSET(BT_SINK_MODULE_COMMON)       /**< To get the module offset of the #BT_MODULE_HFP. */
#define SINK_MODULE_MASK_HFP       SINK_MODULE_MASK_OFFSET(BT_SINK_MODULE_HFP)          /**< To get the module offset of the #BT_MODULE_HSP. */
#define SINK_MODULE_MASK_HSP       SINK_MODULE_MASK_OFFSET(BT_SINK_MODULE_HSP)          /**< To get the module offset of the #BT_MODULE_SPP. */
#define SINK_MODULE_MASK_A2DP      SINK_MODULE_MASK_OFFSET(BT_SINK_MODULE_A2DP)         /**< To get the module offset of the #BT_MODULE_AVRCP. */
#define SINK_MODULE_MASK_AVRCP     SINK_MODULE_MASK_OFFSET(BT_SINK_MODULE_AVRCP)        /**< To get the module offset of the #BT_MODULE_A2DP. */
#define SINK_MODULE_MASK_PBAPC     SINK_MODULE_MASK_OFFSET(BT_SINK_MODULE_PBAPC)        /**< To get the module offset of the #BT_MODULE_PBAPC. */
#define SINK_MODULE_MASK_AWS_MCE   SINK_MODULE_MASK_OFFSET(BT_SINK_MODULE_AWS_MCE)      /**< To get the module offset of the #BT_MODULE_AWS. */
typedef uint32_t bt_sink_module_mask_t; /**< Type definition of a module mask. */

#define BT_SINK_SRV_MAX_DEVICE_NUM  (3)

typedef enum{
    BT_SINK_SRV_EDR = 0,
    BT_SINK_SRV_LE_AUDIO
} bt_sink_srv_type_t;

/**
*@brief The struct of #get device info.
*/
typedef struct {
    uint32_t handle;
    uint8_t type;
    uint8_t mask;
} bt_sink_srv_device_info_t;

void bt_sink_srv_config_features(bt_sink_feature_config_t *features);

const bt_sink_feature_config_t *bt_sink_srv_get_features_config(void);

bt_status_t bt_sink_srv_set_clock_offset_ptr_to_dsp(const bt_bd_addr_t * address);

typedef bt_status_t (*bt_sink_action_callback_t)(bt_sink_srv_action_t action, void *parameters);

typedef struct {
    bt_sink_module_mask_t module;
    bt_sink_action_callback_t callback;
} bt_sink_srv_action_callback_table_t;

#ifdef MTK_AUDIO_SYNC_ENABLE
typedef struct {
    bt_sink_srv_sync_feature_t type;
    uint8_t length;
    void *data;
    uint32_t duration;
    uint32_t timeout_duration;
}bt_sink_srv_get_sync_data_parameter_t;

typedef struct {
    bt_sink_srv_sync_feature_t type;
    uint8_t length;
    void *data;
    uint32_t gpt_count;
}bt_sink_srv_sync_callback_data_t;

typedef void (*bt_sink_srv_sync_feature_callback_t)(bt_sink_srv_sync_status_t sync_status, bt_sink_srv_sync_callback_data_t *sync_data);

typedef struct {
    bt_sink_srv_sync_feature_t type;
    bt_sink_srv_sync_feature_callback_t sync_callback;
} bt_sink_srv_sync_callback_t;

bt_status_t bt_sink_srv_request_sync_gpt(bt_sink_srv_get_sync_data_parameter_t *sync_parameters);
#endif

#ifdef MTK_AWS_MCE_ENABLE

/**
  * @brief                             Get the duration between target bt clock and base clock.
  * @param[in] target_clk       the target bt clock.
  * @param[in] base_clk         the base bt clock, if base_clk is NULL, it means that user want use current bt clock as base to calculate duration, or base_clk should be used as base.
  * @param[out] duration         used to save the duration between target clock and base clock, it`s unit is us.
  * @return                            #BT_STATUS_SUCCESS, calculate target bt clock success.
  *                                        #BT_STATUS_FAIL, partner is not attached or parameters error.
  */
bt_status_t bt_sink_srv_bt_clock_get_duration(bt_clock_t *target_clk, bt_clock_t 
    *base_clk, int32_t *duration);

void bt_sink_srv_register_aws_mce_report_callback(void);
#endif /*MTK_AWS_MCE_ENABLE*/

void bt_sink_srv_register_callback_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BT_SINK_SRV_COMMON_H__ */
