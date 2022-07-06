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


#ifndef __RACE_CMD_CAPTOUCH_H__
#define __RACE_CMD_CAPTOUCH_H__


#include "race_cmd_feature.h"
#ifdef RACE_CAPTOUCH_CMD_ENABLE
#include "stdint.h"
#include "race_cmd.h"
#include "hal_captouch.h"

////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RACE_CAPTOUCH_GET_SETTING                       0x1600
#define RACE_CAPTOUCH_SET_SETTING                       0x1601
#define RACE_CAPTOUCH_COARSE_CAP_CALIBRATION        0x1602
#define RACE_CAPTOUCH_SET_AUTO_TUNE_FEATURE         0x1603
#define RACE_CAPTOUCH_CTRL_MULTI_CHANNEL_REAL_DATA      0x1605

////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    uint8_t status;
    int16_t avg_data;
} PACKED race_captouch_noti_real_data_t;

typedef struct {
    uint8_t  status;
    uint32_t time;
    int16_t  avg_data[HAL_CAPTOUCH_CHANNEL_MAX];
    int16_t  vadc_data[HAL_CAPTOUCH_CHANNEL_MAX];
    int8_t   cal_data[HAL_CAPTOUCH_CHANNEL_MAX];
#ifdef HAL_CPT_FEATURE_4CH
    int16_t  ear_detect_base[4];
    int16_t  mavg_adc_dbg[4];
    int16_t  vadc_debounce[4];
    int16_t  ear_detect_data[4];
    uint8_t  noise_cnt_race[4];
#endif
} PACKED race_captouch_noti_multi_channel_real_data_t;




////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*!
  @brief Process BLUETOOTH related RACE commands.

  @param pRaceHeaderCmd This parameter represents the raw data such as "05 5A...".
  @param Lenth Total bytes of this RACE command.
  @param channel_id Channel identifier
*/
void *RACE_CmdHandler_captouch(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id);
#endif /* RACE_CAPTOUCH_CMD_ENABLE */
#endif /* __RACE_CMD_CAPTOUCH_H__ */

