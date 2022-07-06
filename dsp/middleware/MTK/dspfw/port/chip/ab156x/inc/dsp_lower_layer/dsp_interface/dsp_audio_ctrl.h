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

#ifndef __DSP_AUDIOCTRL_H__
#define __DSP_AUDIOCTRL_H__

#define A2DP_DBG_PORT (0)
#include "types.h"

/******************************************************************************
 * Type Definitions
 ******************************************************************************/

/* Common Section */

typedef enum AU_DSP_STATUS_LIST_e
{
    AU_INPUT_DSP                    = 0,
    AU_INPUT_MINI_DSP               = 1,
    AU_STATUS_INPUT                 = 2,

    AU_OUTPUT_DSP                   = 3,
    AU_OUTPUT_DSP_SUBPATH           = 4,
    AU_OUTPUT_MINI_DSP              = 5,
    AU_STATUS_OUTPUT                = 6,

    AU_STATUS_ALL                   = 7,
} AU_DSP_STATUS_LIST_t;

typedef enum AU_DSP_STATUS_e
{
    AU_DSP_STATUS_OFF               = 0,
    AU_DSP_STATUS_ON                = 1,
} AU_DSP_STATUS_CH_t;

/* AFE Section */
#include "dsp_drv_afe.h"

/* Audio eSCO Section */

typedef enum AU_ESCO_MODE_e
{
    AUDIO_ESCO_CVSD_MODE            = 0,
    AUDIO_ESCO_MSBC_MODE            = 1,
} AU_ESCO_MODE_t;

typedef struct AU_ESCO_CTRL_s
{
    AU_ESCO_MODE_t          Mode;
} AU_ESCO_CTRL_t;

/* Gain Control Section */
#include "dsp_gain_control.h"


/* Collection of Sections */
typedef struct DSP_AUDIO_CTRL_s
{
    AU_DSP_STATUS_CH_t  Status[AU_STATUS_ALL+1];
    AU_AFE_CTRL_t       Afe;
    DSP_GAIN_CTRL_t		Gc;
} DSP_AUDIO_CTRL_t, *DSP_AUDIO_CTRL_PTR_t;


/******************************************************************************
 * Global Variables
 ******************************************************************************/
extern DSP_AUDIO_CTRL_t gAudioCtrl;


/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/
extern VOID DSP_CTRL_Initialization (VOID);
extern VOID DSP_CTRL_ChangeStatus (AU_DSP_STATUS_LIST_t Component, AU_DSP_STATUS_CH_t Stat);

#endif /* __DSP_AUDIOCTRL_H__ */

