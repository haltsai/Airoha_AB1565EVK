/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
 
#ifndef __RACE_CMD_SHARE_MODE__
#define __RACE_CMD_SHARE_MODE__
#ifdef AIR_MCSYNC_SHARE_ENABLE

#include "race_cmd.h"
#include "race_xport.h"

////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RACE_CMD_SHARE_MODE_SET_ENABLE_SHARE    1
#define RACE_CMD_SHARE_MODE_SET_ENABLE_FOLLOWER 2
#define RACE_CMD_SHARE_MODE_SET_DISABLE         0

#define RACE_CMD_SHARE_MODE_STA_NORMAL      0
#define RACE_CMD_SHARE_MODE_STA_PREPAIRING  1
#define RACE_CMD_SHARE_MODE_STA_SHARING     2
#define RACE_CMD_SHARE_MODE_STA_LEAVING     3

////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef uint8_t race_share_mode_set_cmd;
typedef uint8_t race_share_mode_state_type;

typedef uint8_t (*race_share_mode_set_callback)(race_share_mode_set_cmd cmd);
typedef race_share_mode_state_type (*race_share_mode_get_state_callback)(void);

typedef struct {
    race_share_mode_set_callback set_callback;
    race_share_mode_get_state_callback get_callback;
}race_share_mode_config;

/************************************* NOTI Definition Begin *************************************/
/************************************* NOTI Definition End *************************************/


////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/*!
  @brief register share mode callback.

  @param config, the share mode configuration.
*/
void race_share_mode_register(race_share_mode_config *config);


/*!
  @brief register share mode callback.

  @param new_sta, the current state of share mode.
*/
void race_share_mode_notify_state_change(race_share_mode_state_type new_sta);

#endif /* FACTORY_TEST_RACE_ENABLE */
#endif /* __RACE_CMD_SHARE_MODE__ */

