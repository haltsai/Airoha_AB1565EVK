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
#include "scenario_gsensor.h"

/*------------------------------------------------PORT----MTK_SENSOR_SOURCE_ENABLE------------------------------------------------------------------*/
#if defined(MTK_SENSOR_SOURCE_ENABLE)

#define GSENSOR_DATA_SIZE   (384)
#define GSENSOR_BLOCK_SIZE  (GSENSOR_DATA_SIZE + 4)
#define GSENSOR_BLOCK_NUM   4

static ATTR_SHARE_ZIDATA uint8_t g_share_gsensor_buffer[GSENSOR_BLOCK_SIZE * GSENSOR_BLOCK_NUM];
static ATTR_SHARE_ZIDATA n9_dsp_share_info_t gsensor_audio_transmitter_share_info;

void audio_transmitter_gsensor_open_playback(audio_transmitter_config_t *config, mcu2dsp_open_param_t *open_param)
{
    open_param->stream_out_param.data_ul.scenario_type   = config->scenario_type;
    open_param->stream_out_param.data_ul.scenario_sub_id = config->scenario_sub_id;
    open_param->param.stream_in  = STREAM_IN_GSENSOR;
    if (config->scenario_sub_id == 0) {
        open_param->param.stream_out  = STREAM_OUT_VIRTUAL;
    } else {
        memset(&gsensor_audio_transmitter_share_info,0,sizeof(n9_dsp_share_info_t));
        open_param->param.stream_out  = STREAM_OUT_AUDIO_TRANSMITTER;
        open_param->stream_out_param.data_ul.data_notification_frequency = 1;
        open_param->stream_out_param.data_ul.p_share_info = &gsensor_audio_transmitter_share_info;
        open_param->stream_out_param.data_ul.max_payload_size = GSENSOR_DATA_SIZE;
        audio_transmitter_reset_share_info_by_block(open_param->stream_out_param.data_ul.p_share_info, (uint32_t)g_share_gsensor_buffer, GSENSOR_BLOCK_SIZE * GSENSOR_BLOCK_NUM, GSENSOR_DATA_SIZE);
    }
}

void audio_transmitter_gsensor_start_playback(audio_transmitter_config_t *config, mcu2dsp_start_param_t *start_param)
{
    start_param->param.stream_in = STREAM_IN_GSENSOR;
    if (config->scenario_sub_id == 0) {
        start_param->param.stream_out  = STREAM_OUT_VIRTUAL;
    } else {
        start_param->param.stream_out  = STREAM_OUT_AUDIO_TRANSMITTER;
    }
}
#endif
