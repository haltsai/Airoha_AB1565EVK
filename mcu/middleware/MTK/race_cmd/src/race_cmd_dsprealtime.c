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


#include "race_cmd_feature.h"
#ifdef RACE_DSP_REALTIME_CMD_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hal.h"
#include "race_cmd.h"
#include "race_cmd_dsprealtime.h"
#include "race_xport.h"
#include "race_noti.h"
#if defined(HAL_AUDIO_MODULE_ENABLED)
#include "hal_ccni.h"
#include "memory_attribute.h"
#include "hal_audio_cm4_dsp_message.h"
#include "hal_audio_message_struct.h"
#include "hal_audio_internal.h"
#include "bt_sink_srv_ami.h"
#include "bt_sink_srv_common.h"
#include "bt_connection_manager_internal.h"
#include "hal_resource_assignment.h"
#endif
#ifndef MTK_ANC_V2
#include "at_command_audio_ata_test.h"
#endif
#include <math.h>
#ifdef MTK_AIRDUMP_EN_MIC_RECORD
#include "record_control.h"
#include "hal_dvfs.h"
#endif
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
#include "race_event.h"
#include "leakage_detection_control.h"
#endif
#if defined(MTK_USER_TRIGGER_FF_ENABLE) || defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
#include "race_cmd_relay_cmd.h"
#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
#include "user_trigger_adaptive_ff.h"
#include "apps_config_vp_manager.h"
#endif
#endif
#if defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
#include "anc_monitor.h"
#ifdef ADAPIVE_ANC_STREAM_CONTROL
bool g_adaptive_ANC_stream_MP_control_flag = false; //true for adaptive ANC is currently under MP mode.
bool g_adaptive_ANC_stream_suspend_control_flag = false; ////true for adaptive ANC stream is suspended by race
extern uint8_t g_uu_enable;
extern uint8_t g_wnd_enable;
extern uint8_t g_ed_enable;
#endif
#endif
#include "race_event.h"
#include "race_event_internal.h"
#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
#include "audio_transmitter_internal.h"
#endif
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
#include "race_cmd_co_sys.h"
#include "race_cmd_hostaudio.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define dsp_realtime_min(x,y) (x > y) ? y : x
#define PEQ_SPECIAL_PHASE   (0xFF)


//////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
static SemaphoreHandle_t g_race_dsprealtime_mutex = NULL;
#endif
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
#define PEQ_NVKEY_BUF_SIZE (sizeof(AMI_AWS_MCE_PEQ_PACKT_t) + sizeof(DSP_PEQ_NVKEY_t))
uint8_t g_peq_nvkey[PEQ_NVKEY_BUF_SIZE];
uint8_t g_peq_nvkey_available = 1;
#endif
#ifdef MTK_ANC_ENABLE
static uint8_t g_anc_race_ch_id;
#if !defined(MTK_ANC_V2) | defined(MTK_LEAKAGE_DETECTION_ENABLE)
static SemaphoreHandle_t g_race_anc_mutex = NULL;
#endif
#ifndef MTK_ANC_V2
static uint32_t g_anc_notify_on; //bit[0]:agent_done, bit[1]:partner_done, bit[16:23]:agent_filter, bit[24:31]:parnter_filter
static uint32_t g_anc_notify_off; //bit[0]:agent_done, bit[1]:partner_done, bit[31]: request_from_race
static uint32_t g_anc_set_agent_vol; //anc_sw_gain_t
static uint32_t g_anc_set_partner_vol; //anc_sw_gain_t
#endif
#endif
#ifdef MTK_AIRDUMP_EN
uint8_t g_airdump_race_ch_id;
uint8_t g_airdump_cnt_past = 0;
static TimerHandle_t g_airdump_timer = NULL;
#endif
#ifdef MTK_AIRDUMP_EN_MIC_RECORD
uint8_t g_airdump_common_race_ch_id;
uint8_t g_airdump_common_race_request;
uint8_t g_airdump_common_cnt = 0;
TimerHandle_t g_airdump_common_timer = NULL;
int16_t g_record_airdump_data[256];  /*TODO: Change to dynamic alloc.*/
extern uint16_t g_dump;
extern bool g_record_airdump;
#if 0
const int16_t g_128_1ktone[128] =
{
0, 6284, 11612, 15173, 16423, 15171, 11612, 6284, -1, -6284, -11613, -15172, -16422, -15173, -11613, -6284,
0, 6284, 11612, 15173, 16423, 15171, 11612, 6284, -1, -6284, -11613, -15172, -16422, -15173, -11613, -6284,
0, 6284, 11612, 15173, 16423, 15171, 11612, 6284, -1, -6284, -11613, -15172, -16422, -15173, -11613, -6284,
0, 6284, 11612, 15173, 16423, 15171, 11612, 6284, -1, -6284, -11613, -15172, -16422, -15173, -11613, -6284,
0, 6284, 11612, 15173, 16423, 15171, 11612, 6284, -1, -6284, -11613, -15172, -16422, -15173, -11613, -6284,
0, 6284, 11612, 15173, 16423, 15171, 11612, 6284, -1, -6284, -11613, -15172, -16422, -15173, -11613, -6284,
0, 6284, 11612, 15173, 16423, 15171, 11612, 6284, -1, -6284, -11613, -15172, -16422, -15173, -11613, -6284,
0, 6284, 11612, 15173, 16423, 15171, 11612, 6284, -1, -6284, -11613, -15172, -16422, -15173, -11613, -6284,
};
const int16_t g_128_2ktone[128] =
{
0, 11612, 16422, 11612, 0, -11612, -16422, -11612, 1, 11612, 16422, 11613, -1, -11612, -16422, -11613,
0, 11612, 16422, 11612, 0, -11612, -16422, -11612, 1, 11612, 16422, 11613, -1, -11612, -16422, -11613,
0, 11612, 16422, 11612, 0, -11612, -16422, -11612, 1, 11612, 16422, 11613, -1, -11612, -16422, -11613,
0, 11612, 16422, 11612, 0, -11612, -16422, -11612, 1, 11612, 16422, 11613, -1, -11612, -16422, -11613,
0, 11612, 16422, 11612, 0, -11612, -16422, -11612, 1, 11612, 16422, 11613, -1, -11612, -16422, -11613,
0, 11612, 16422, 11612, 0, -11612, -16422, -11612, 1, 11612, 16422, 11613, -1, -11612, -16422, -11613,
0, 11612, 16422, 11612, 0, -11612, -16422, -11612, 1, 11612, 16422, 11613, -1, -11612, -16422, -11613,
0, 11612, 16422, 11612, 0, -11612, -16422, -11612, 1, 11612, 16422, 11613, -1, -11612, -16422, -11613,
};
#endif
#endif
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
uint8_t g_leakage_detection_race_ch_id = 0;
static uint16_t g_LD_result_agent = 0;  //bit[0:7]:result, bit[15]:done_or_not
static uint16_t g_LD_result_partner = 0;//bit[0:7]:result, bit[15]:done_or_not
static TimerHandle_t   s_xLeakageDetectionOneShotTimer = NULL;
#endif
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
#define USER_TRIGGER_FF_STATUS_RUNNING 2
#define USER_TRIGGER_FF_STATUS_DONE 3
#define USER_TRIGGER_FF_STATUS_ABORT 4
#define USER_TRIGGER_FF_STATUS_SMALL_VOLUME 5
#define USER_TRIGGER_FF_STATUS_Compare 6
#endif
#endif

//////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
bool race_dsprealtime_relay_handler (ptr_race_pkt_t pRaceHeaderCmd, uint16_t cmd_length, uint8_t channel_id, uint8_t send_to_follower, uint8_t cosys_result);
bool race_dsprealtime_relay_check (ptr_race_pkt_t pRaceHeaderCmd);
#endif

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
static void race_dsprealtime_mutex_take(void)
{
    if(g_race_dsprealtime_mutex == NULL) {
        g_race_dsprealtime_mutex = xSemaphoreCreateMutex();
        if (g_race_dsprealtime_mutex == NULL) {
            RACE_LOG_MSGID_E("g_race_anc_mutex create error\r\n", 0);
        }
    }
    if(g_race_dsprealtime_mutex != NULL)
    {
        if (xSemaphoreTake(g_race_dsprealtime_mutex, portMAX_DELAY) == pdFALSE) {
            RACE_LOG_MSGID_E("g_race_dsprealtime_mutex error\r\n", 0);
        }
    }
}
static void race_dsprealtime_mutex_give(void)
{
    if(g_race_dsprealtime_mutex != NULL)
    {
        if (xSemaphoreGive(g_race_dsprealtime_mutex) == pdFALSE) {
            RACE_LOG_MSGID_E("g_race_dsprealtime_mutex error\r\n", 0);
        }
    }
}
#endif

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
/**
 * race_dsprt_peq_report
 *
 * For agent and partner, g_peq_nvkey buffer is used to store new PEQ coefficients and parameters.
 * Release g_peq_nvkey with set g_peq_nvkey_available as 1.
 *
 * @pu2PeqParam : reserved.
 */
void race_dsprt_peq_report(uint16_t *pu2PeqParam)
{
    RACE_LOG_MSGID_I("race_dsprt_peq_report\n", 0);
    g_peq_nvkey_available = 1;
}

/**
 * race_dsprt_peq_realtime_data
 *
 * For agent, prepare AMI parameters for ami event, AMI_EVENT_PEQ_REALTIME, and send IF packet which contain PEQ data to partner.
 * For partner, prepare AMI paramters for ami event, AMI_EVENT_PEQ_REALTIME.
 *
 * @setting_mode  : [Agent only] 0:PEQ_DIRECT  1:PEQ_SYNC.
 * @target_bt_clk   : [Agent only] a bt clock for agent/partner to apply PEQ simultaneously.
 * @p_coef            : [Agent only] new PEQ coefficients.
 * @coef_size        : [Agent only] new PEQ coefficient size (bytes).
 */
uint32_t race_dsprt_peq_realtime_data(uint8_t phase_id, uint8_t setting_mode, uint32_t target_bt_clk, uint8_t *p_coef, uint32_t coef_size, am_feature_type_t audio_path_id)
{
    uint32_t ret = 0;
    uint8_t *peq_coef_buf;
    bt_sink_srv_am_feature_t am_feature;

    if(bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_AGENT) //Agent (Transmitter)
    {
        AMI_AWS_MCE_PEQ_PACKT_t *peq_packet = (AMI_AWS_MCE_PEQ_PACKT_t *)race_mem_alloc(sizeof(AMI_AWS_MCE_PEQ_PACKT_t) + coef_size);
        if (peq_packet) {
            uint32_t size = ((uint32_t)&peq_packet->peq_data.peq_data_rt.peq_coef - (uint32_t)peq_packet) + coef_size;
            peq_packet->phase_id = phase_id;
            peq_packet->setting_mode = setting_mode;
            peq_packet->target_bt_clk = target_bt_clk;
            peq_packet->peq_data.peq_data_rt.peq_coef_size = coef_size;
            memcpy((void *)&peq_packet->peq_data.peq_data_rt.peq_coef, (void *)p_coef, coef_size);
            bt_sink_srv_aws_mce_ami_data(AMI_EVENT_PEQ_REALTIME, (uint8_t *)peq_packet, size, false);
            race_mem_free(peq_packet);
        } else {
            RACE_LOG_MSGID_E("peq realtime : 1st malloc failed, size:%d\n",1, sizeof(AMI_AWS_MCE_PEQ_PACKT_t) + coef_size);
            configASSERT(0);
        }
    }
    peq_coef_buf = race_mem_alloc(coef_size);
    if (peq_coef_buf) {
        memcpy((void *)peq_coef_buf, (void *)p_coef, coef_size);
        memset(&am_feature, 0, sizeof(bt_sink_srv_am_feature_t));
        am_feature.type_mask                             = audio_path_id;
        am_feature.feature_param.peq_param.enable        = 1;
        am_feature.feature_param.peq_param.sound_mode    = PEQ_SOUND_MODE_REAlTIME;
        am_feature.feature_param.peq_param.u2ParamSize   = (uint16_t)coef_size;
        am_feature.feature_param.peq_param.pu2Param      = (uint16_t *)peq_coef_buf;
        am_feature.feature_param.peq_param.target_bt_clk = target_bt_clk;
        am_feature.feature_param.peq_param.setting_mode  = setting_mode;
        am_feature.feature_param.peq_param.phase_id      = phase_id;
        am_feature.feature_param.peq_param.peq_notify_cb = race_dsprt_peq_report;
        ret = am_audio_set_feature(FEATURE_NO_NEED_ID, &am_feature);
    } else {
        ret = -1;
        RACE_LOG_MSGID_E("peq realtime : 2nd malloc failed, size:%d\n",1, coef_size);
        configASSERT(0);
    }
    return ret;
}

/**
 * race_dsprt_peq_change_mode_data
 *
 * For agent, prepare AMI parameters for ami event, AMI_EVENT_PEQ_CHANGE_MODE, and send IF packet which contain PEQ data to partner.
 * For partner, prepare AMI paramters for ami event, AMI_EVENT_PEQ_CHANGE_MODE.
 *
 * @setting_mode  : [Agent only] 0:PEQ_DIRECT  1:PEQ_SYNC.
 * @target_bt_clk   : [Agent only] a bt clock for agent/partner to apply PEQ simultaneously.
 * @enable            : [Agent only] going to enable PEQ or disable PEQ.
 * @sound_mode    : [Agent only] going to change to sound_mode mode.
 */
uint32_t race_dsprt_peq_change_mode_data(uint8_t phase_id, uint8_t setting_mode, uint32_t target_bt_clk, uint8_t enable, uint8_t sound_mode, am_feature_type_t audio_path_id)
{
    uint32_t ret = 0;
    bt_sink_srv_am_feature_t am_feature;

    if(bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_AGENT) //Agent (Transmitter)
    {
        AMI_AWS_MCE_PEQ_PACKT_t *peq_packet = (AMI_AWS_MCE_PEQ_PACKT_t *)race_mem_alloc(sizeof(AMI_AWS_MCE_PEQ_PACKT_t));
        uint32_t size = (uint32_t)&peq_packet->peq_data.peq_data_cm.reserved - (uint32_t)peq_packet;
        if (peq_packet) {
            peq_packet->phase_id = phase_id;
            peq_packet->setting_mode = setting_mode;
            peq_packet->target_bt_clk = target_bt_clk;
            peq_packet->peq_data.peq_data_cm.enable = enable;
            peq_packet->peq_data.peq_data_cm.sound_mode = sound_mode;
            bt_sink_srv_aws_mce_ami_data(AMI_EVENT_PEQ_CHANGE_MODE, (uint8_t *)peq_packet, size, false);
            race_mem_free(peq_packet);
        } else {
            RACE_LOG_MSGID_E("peq change : 1st malloc failed, size:%d\n",1, sizeof(AMI_AWS_MCE_PEQ_PACKT_t));
            configASSERT(0);
        }
    }
    memset(&am_feature, 0, sizeof(bt_sink_srv_am_feature_t));
    am_feature.type_mask                             = audio_path_id;
    am_feature.feature_param.peq_param.enable        = enable;
    am_feature.feature_param.peq_param.sound_mode    = sound_mode;
    am_feature.feature_param.peq_param.u2ParamSize   = 0;
    am_feature.feature_param.peq_param.pu2Param      = 0;
    am_feature.feature_param.peq_param.target_bt_clk = target_bt_clk;
    am_feature.feature_param.peq_param.setting_mode  = setting_mode;
    am_feature.feature_param.peq_param.phase_id      = phase_id;
    am_feature.feature_param.peq_param.peq_notify_cb = race_dsprt_peq_report;
    ret = am_audio_set_feature(FEATURE_NO_NEED_ID, &am_feature);
    return ret;
}
#ifdef MTK_AWS_MCE_ENABLE
/**
 * race_dsprt_peq_collect_data
 *
 * For partner, to collect IF packets which contain PEQ data and call function to prepare AMI paramters according to ami event ...
 *
 * @info : report info from aws_mce_report service
 */
int32_t race_dsprt_peq_collect_data(bt_aws_mce_report_info_t *info)
{
    AMI_AWS_MCE_PACKET_HDR_t *ami_pkt_header = (AMI_AWS_MCE_PACKET_HDR_t *)info->param;
    AMI_AWS_MCE_PEQ_PACKT_t *peq_param = (AMI_AWS_MCE_PEQ_PACKT_t *)((uint8_t *)info->param + sizeof(AMI_AWS_MCE_PACKET_HDR_t));

    static ami_event_t _ami_event = 0;
    static uint32_t _save_len = 0;
    static uint32_t _total_pkt = 0;
    static int32_t _pre_pkt = 0;

    uint32_t temp_size;
    int32_t ret = 1;

    if((ami_pkt_header->ami_event != AMI_EVENT_PEQ_REALTIME) && (ami_pkt_header->ami_event != AMI_EVENT_PEQ_CHANGE_MODE)) {
        RACE_LOG_MSGID_W("race_dsprt_peq_collect_data wrong ami event:%d\n", 1, ami_pkt_header->ami_event);
        return -2;
    }

    if(ami_pkt_header->SubPktId == 0)
    {
        if(g_peq_nvkey_available != 1) {
            RACE_LOG_MSGID_W("g_peq_nvkey is unavailable, (%d) %d %d, %d %d\n", 5, ami_pkt_header->ami_event, ami_pkt_header->numSubPkt, ami_pkt_header->SubPktId, _total_pkt, _pre_pkt);
        }
        _ami_event = ami_pkt_header->ami_event;
        _save_len = 0;
        _total_pkt = (uint32_t)ami_pkt_header->numSubPkt;
        _pre_pkt = -1;
        g_peq_nvkey_available = 0;
    }

    if((ami_pkt_header->SubPktId == (_pre_pkt + 1)) && (ami_pkt_header->numSubPkt == _total_pkt) && (ami_pkt_header->ami_event == _ami_event) && (_total_pkt >= 1))
    {
        temp_size = (uint32_t)info->param_len - sizeof(AMI_AWS_MCE_PACKET_HDR_t);
        memcpy(g_peq_nvkey + _save_len, peq_param, temp_size);
        _save_len += temp_size;
        _pre_pkt = (uint32_t)ami_pkt_header->SubPktId;
        if(_save_len > PEQ_NVKEY_BUF_SIZE) {
            RACE_LOG_MSGID_E("PEQ collect data from IF\n", 0);
            configASSERT(0);
        }
    }
    else
    {
        RACE_LOG_MSGID_E("PEQ packet header is wrong, (%d) %d %d, %d %d\n", 5, ami_pkt_header->ami_event, ami_pkt_header->numSubPkt, ami_pkt_header->SubPktId, _total_pkt, _pre_pkt);
        return -2;
    }

    if((_pre_pkt + 1) == _total_pkt)
    {
        bt_sink_srv_am_feature_t am_feature;
        memset(&am_feature, 0, sizeof(bt_sink_srv_am_feature_t));
        switch(ami_pkt_header->ami_event)
        {
            case AMI_EVENT_PEQ_REALTIME:
            {
                AMI_AWS_MCE_PEQ_PACKT_t *peq_packet = (AMI_AWS_MCE_PEQ_PACKT_t *)g_peq_nvkey;
                uint32_t size = ((uint32_t)&peq_packet->peq_data.peq_data_rt.peq_coef - (uint32_t)peq_packet) + peq_packet->peq_data.peq_data_rt.peq_coef_size;
                if (_save_len == size) {
                    uint16_t peq_coef_buf_size = peq_packet->peq_data.peq_data_rt.peq_coef_size;
                    uint8_t *peq_coef_buf = (uint8_t *)race_mem_alloc(peq_coef_buf_size);
                    if (peq_coef_buf) {
                        memcpy(peq_coef_buf, &peq_packet->peq_data.peq_data_rt.peq_coef, peq_coef_buf_size);
                        am_feature.type_mask                             = AM_A2DP_PEQ;
                        am_feature.feature_param.peq_param.enable        = 1;
                        am_feature.feature_param.peq_param.sound_mode    = PEQ_SOUND_MODE_REAlTIME;
                        am_feature.feature_param.peq_param.pu2Param      = (uint16_t *)peq_coef_buf;
                        am_feature.feature_param.peq_param.u2ParamSize   = peq_coef_buf_size;
                        am_feature.feature_param.peq_param.target_bt_clk = peq_packet->target_bt_clk;
                        am_feature.feature_param.peq_param.setting_mode  = peq_packet->setting_mode;
                        am_feature.feature_param.peq_param.phase_id      = peq_packet->phase_id;
                        am_feature.feature_param.peq_param.peq_notify_cb = race_dsprt_peq_report;
                        ret = am_audio_set_feature(FEATURE_NO_NEED_ID, &am_feature);
                        RACE_LOG_MSGID_I("peq collect data - realtime peq_param_length=%u target_bt_clk=0x%x ret=%d\n",3,am_feature.feature_param.peq_param.u2ParamSize,am_feature.feature_param.peq_param.target_bt_clk,ret);
                    } else {
                        RACE_LOG_MSGID_E("peq collect data - realtime malloc fail, size:%d\n",1, peq_coef_buf_size);
                        configASSERT(0);
                        ret = -1;
                    }
                    g_peq_nvkey_available = 1;
                } else {
                    ret = -1;
                    RACE_LOG_MSGID_E("PEQ acc packet size is wrong, %d %d\n",2, am_feature.feature_param.peq_param.u2ParamSize, size);
                    configASSERT(0);
                }
                break;
            }
            case AMI_EVENT_PEQ_CHANGE_MODE:
            {
                AMI_AWS_MCE_PEQ_PACKT_t *peq_packet = (AMI_AWS_MCE_PEQ_PACKT_t *)g_peq_nvkey;
                uint32_t size = (uint32_t)&peq_packet->peq_data.peq_data_cm.reserved - (uint32_t)peq_packet;
                if(_save_len == size) {
                    am_feature.type_mask                             = AM_A2DP_PEQ;
                    am_feature.feature_param.peq_param.enable        = peq_packet->peq_data.peq_data_cm.enable;
                    am_feature.feature_param.peq_param.sound_mode    = peq_packet->peq_data.peq_data_cm.sound_mode;
                    am_feature.feature_param.peq_param.pu2Param      = 0;
                    am_feature.feature_param.peq_param.u2ParamSize   = 0;
                    am_feature.feature_param.peq_param.target_bt_clk = peq_packet->target_bt_clk;
                    am_feature.feature_param.peq_param.setting_mode  = peq_packet->setting_mode;
                    am_feature.feature_param.peq_param.phase_id      = peq_packet->phase_id;
                    am_feature.feature_param.peq_param.peq_notify_cb = race_dsprt_peq_report;
                    ret = am_audio_set_feature(FEATURE_NO_NEED_ID, &am_feature);
                    g_peq_nvkey_available = 1;
                    RACE_LOG_MSGID_I("enable=%d sound_mode=%d target_bt_clk=%d ret=%d\n",4,am_feature.feature_param.peq_param.enable,am_feature.feature_param.peq_param.sound_mode,am_feature.feature_param.peq_param.target_bt_clk,ret);
                } else {
                    ret = -1;
                    RACE_LOG_MSGID_E("PEQ packet size is wrong, %d %d\n",2, am_feature.feature_param.peq_param.u2ParamSize, size);
                    configASSERT(0);
                }
                break;
            }
            default:
            {
                ret = -1;
                RACE_LOG_MSGID_E("un-expected ami event : %d\n",1, ami_pkt_header->ami_event);
                break;
            }
        }
        _ami_event = 0;
        _save_len = 0;
        _total_pkt = 0;
        _pre_pkt = 0;
    }

    if(ret == 0) {
        RACE_LOG_MSGID_I("race_dsprt_peq_collect_data SUCCESS\n", 0);
    } else if (ret == 1) {
        RACE_LOG_MSGID_I("race_dsprt_peq_collect_data CONTINUE\n", 0);
    } else {
        g_peq_nvkey_available = 1;
        RACE_LOG_MSGID_E("race_dsprt_peq_collect_data FAIL\n", 0);
    }

    return ret;
}

int32_t race_dsprt_peq_get_target_bt_clk(bt_aws_mce_role_t role, uint8_t *setting_mode, bt_clock_t *target_bt_clk)
{
    bt_clock_t current_bt_clk = {0};
    int32_t diff = 0;
    uint32_t ret;
    if(role == BT_AWS_MCE_ROLE_AGENT)
    {
        *setting_mode = PEQ_SYNC;
        ret = bt_sink_srv_bt_clock_addition(&current_bt_clk, NULL, 0);
        if(ret == BT_STATUS_FAIL) RACE_LOG_MSGID_W("get current bt clock FAIL\n",0);
        ret = bt_sink_srv_bt_clock_addition(target_bt_clk, NULL, PEQ_FW_LATENCY*1000);
        if(ret == BT_STATUS_FAIL) RACE_LOG_MSGID_W("get target bt clock FAIL with duration %d us\n", 1,PEQ_FW_LATENCY*1000);
        diff = (((int32_t)target_bt_clk->nclk - (int32_t)current_bt_clk.nclk)*625/2 + ((int32_t)target_bt_clk->nclk_intra - (int32_t)current_bt_clk.nclk_intra));
        if((diff > PEQ_FW_LATENCY*1000+10000) || (diff < PEQ_FW_LATENCY*1000-10000)) {
            RACE_LOG_MSGID_W("get cur: 0x%x.0x%x tar: 0x%x.0x%x  diff: %d xxxxxxxxxxxx\n",5,current_bt_clk.nclk,current_bt_clk.nclk_intra, target_bt_clk->nclk,target_bt_clk->nclk_intra,diff);
        } else {
            RACE_LOG_MSGID_I("get cur: %x.%x tar: %x.%x \n",4,current_bt_clk.nclk,current_bt_clk.nclk_intra, target_bt_clk->nclk,target_bt_clk->nclk_intra);
        }
        if(ret == BT_STATUS_FAIL) { // for agent only case
            *setting_mode = PEQ_DIRECT;
        }
    } else {
        *setting_mode = PEQ_DIRECT;
    }
    return 0;
}
#endif
#endif

void race_dsp_realtime_send_notify_msg(uint16_t race_id, uint16_t race_event, uint16_t result)
{
    RACE_ERRCODE notify_ret = RACE_ERRCODE_FAIL;
    race_dsprealtime_notify_struct *notify_param = (race_dsprealtime_notify_struct *)race_mem_alloc(sizeof(race_dsprealtime_notify_struct));
    if (notify_param){
        notify_param->dsp_realtime_race_id   = race_id;
        notify_param->dsp_realtime_race_evet = race_event;
        notify_param->result                 = result;
    }
    notify_ret = race_send_event_notify_msg(RACE_EVENT_TYPE_AUDIO_DSP_REALTIME, notify_param);
    if (RACE_ERRCODE_SUCCESS != notify_ret)
    {
        RACE_LOG_MSGID_E("Failed to send EVENT_NOTIFY_DSP_REALTIME ret:%x.",1, notify_ret);
        if (notify_param)
        {
            race_mem_free(notify_param);
        }
    }
}

#ifdef MTK_ANC_ENABLE
#if !defined(MTK_ANC_V2) | defined(MTK_LEAKAGE_DETECTION_ENABLE)
static void race_anc_mutex_take(void)
{
    if(g_race_anc_mutex == NULL) {
        g_race_anc_mutex = xSemaphoreCreateMutex();
        if (g_race_anc_mutex == NULL) {
            RACE_LOG_MSGID_E("g_race_anc_mutex create error\r\n", 0);
        }
    }
    if(g_race_anc_mutex != NULL)
    {
        if (xSemaphoreTake(g_race_anc_mutex, portMAX_DELAY) == pdFALSE) {
            RACE_LOG_MSGID_E("g_race_anc_mutex error\r\n", 0);
        }
    }
}
static void race_anc_mutex_give(void)
{
    if(g_race_anc_mutex != NULL)
    {
        if (xSemaphoreGive(g_race_anc_mutex) == pdFALSE) {
            RACE_LOG_MSGID_E("g_race_anc_mutex error\r\n", 0);
        }
    }
}
#endif

#ifndef MTK_ANC_V2
#ifdef MTK_AWS_MCE_ENABLE
uint32_t race_dsprt_anc_feedback_data(aws_mce_report_anc_param_t *anc_param)
{
    race_general_msg_t msg_queue_item;
    PTR_RACE_COMMON_HDR_STRU race_pkt_hdr;
    race_dsprealtime_anc_struct *anc_r_nv_cmd;
    uint8_t channel_id = (uint8_t)(anc_param->ch_info & 0xFF);
    RACE_ERRCODE error;

    switch (channel_id)
    {
        case RACE_SERIAL_PORT_TYPE_UART:
            msg_queue_item.dev_t = SERIAL_PORT_DEV_UART_0;
            break;
        case RACE_SERIAL_PORT_TYPE_SPP:
            msg_queue_item.dev_t = SERIAL_PORT_DEV_BT_SPP;
            break;
        case RACE_SERIAL_PORT_TYPE_BLE:
            msg_queue_item.dev_t = SERIAL_PORT_DEV_BT_LE;
            break;
        case RACE_SERIAL_PORT_TYPE_AIRUPDATE:
            msg_queue_item.dev_t = SERIAL_PORT_DEV_BT_AIRUPDATE;
            break;
        default:
            msg_queue_item.dev_t = SERIAL_PORT_DEV_UNDEFINED;
            break;
    }
    //msg_queue_item.dev_t = SERIAL_PORT_DEV_UART_0;
    msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_RSP_NOTIFY_IND;
    msg_queue_item.msg_data = race_mem_alloc(sizeof(RACE_COMMON_HDR_STRU) + sizeof(race_dsprealtime_anc_struct));
    if(msg_queue_item.msg_data) {
        race_pkt_hdr = (PTR_RACE_COMMON_HDR_STRU)msg_queue_item.msg_data;
        race_pkt_hdr->pktId.value = (uint8_t)(anc_param->ch_info >> 8);
        race_pkt_hdr->type = RACE_TYPE_COMMAND;
        race_pkt_hdr->length = sizeof(RACE_COMMON_HDR_STRU) + sizeof(race_dsprealtime_anc_struct) - 4;
        race_pkt_hdr->id = RACE_DSPREALTIME_ANC;
        anc_r_nv_cmd = (race_dsprealtime_anc_struct *)(msg_queue_item.msg_data + sizeof(RACE_COMMON_HDR_STRU));
        anc_r_nv_cmd->status = 0x80 | (anc_param->ch_info & 0xFF); //channel_id
        anc_r_nv_cmd->anc_id = RACE_ANC_READ_PARTNER_NVDM;
        anc_r_nv_cmd->param.gain.val = anc_param->arg;
        RACE_LOG_MSGID_I("race_dsprt_anc_feedback_data channel:%d, gain=%d %d \n",3, anc_r_nv_cmd->status, anc_r_nv_cmd->param.gain.gain_index_l,anc_r_nv_cmd->param.gain.gain_index_r);
        error = race_send_msg(&msg_queue_item);
        if(error != RACE_ERRCODE_SUCCESS) {
            RACE_LOG_MSGID_E("race_dsprt_anc_feedback_data error:%x",1,error);
        }
    } else {
        RACE_LOG_MSGID_E("race_dsprt_anc_feedback_data malloc fail",0);
        configASSERT(0);
        error = RACE_ERRCODE_FAIL;
    }
    return error;
}
#endif

void race_dsprt_anc_notify(anc_control_event_t event_id, uint8_t fromPartner, uint32_t arg)
{
    if (anc_get_sync_time() == 0) {
        bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
        race_anc_mutex_take();
        if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
            if (event_id == ANC_CONTROL_EVENT_ON) {
                uint32_t filter = (g_anc_notify_on >> 8) & 0xFF;
                if ((filter == 0) || (arg == 0) || (filter != arg)) {
                    race_anc_mutex_give();
                    RACE_LOG_MSGID_W("race_dsprt_anc_notify, event:on, wrong filter:0x%x 0x%x",2, filter, arg);
                    return;
                }
                if (fromPartner == 0) {
                    g_anc_notify_on |= RACE_ANC_AGENT;
                    g_anc_notify_on |= (arg << 16);
                    if ((g_anc_notify_on & RACE_ANC_PARTNER) && (filter != ((g_anc_notify_on >> 24) & 0xFF))) {
                         g_anc_notify_on &= (~RACE_ANC_PARTNER);
                         g_anc_notify_on &= (~(0xFF << 24));
                    }
                } else {
                    g_anc_notify_on |= RACE_ANC_PARTNER;
                    g_anc_notify_on |= (arg << 24);
                    if ((g_anc_notify_on & RACE_ANC_AGENT) && (filter != ((g_anc_notify_on >> 16) & 0xFF))) {
                         g_anc_notify_on &= (~RACE_ANC_AGENT);
                         g_anc_notify_on &= (~(0xFF << 16));
                    }
                }
#ifdef MTK_AWS_MCE_ENABLE
                if ((g_anc_notify_on & RACE_ANC_BOTH) == RACE_ANC_BOTH)
#else
                if (g_anc_notify_on & RACE_ANC_AGENT)
#endif
                {
                    race_dsprealtime_anc_struct *pEvt = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION, RACE_DSPREALTIME_ANC, sizeof(race_dsprealtime_anc_struct), g_anc_race_ch_id);
                    if (pEvt != NULL) {
                        memset(pEvt, 0, sizeof(race_dsprealtime_anc_struct));
                        pEvt->anc_id = RACE_ANC_ON;
                        pEvt->param.anc_on_param.anc_filter_type = (uint8_t)(arg & ANC_FILTER_TYPE_MASK);
                        pEvt->param.anc_on_param.anc_mode = (uint8_t)((arg & ANC_FF_ONLY_BIT_MASK) ? 1 : (arg & ANC_FB_ONLY_BIT_MASK) ? 2 : 0);
                        RACE_LOG_MSGID_I("MP_Step__4__ set anc on filter:0x%x, 0x%x",2,arg,g_anc_notify_on);
                        race_flush_packet((void *)pEvt, g_anc_race_ch_id);
                        g_anc_notify_on = 0;
                    }
                }
            } else if (event_id == ANC_CONTROL_EVENT_SET_VOLUME) {
                uint32_t *p_set_vol = (fromPartner == 0) ? (&g_anc_set_agent_vol) : (&g_anc_set_partner_vol);
                if ((*p_set_vol != arg)) {
                    race_anc_mutex_give();
                    RACE_LOG_MSGID_W("race_dsprt_anc_notify, event:set_%c_vol, wrong vol:0x%x 0x%x",3,(fromPartner==0)?'A':'P', *p_set_vol, arg);
                    return;
                } else {
                    race_dsprealtime_anc_struct *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE, RACE_DSPREALTIME_ANC, sizeof(race_dsprealtime_anc_struct), g_anc_race_ch_id);
                    if (pEvt != NULL) {
                        memset(pEvt, 0, sizeof(race_dsprealtime_anc_struct));
                        pEvt->param.gain.val = arg;
                        if (fromPartner == 0) {
                            pEvt->anc_id = RACE_ANC_SET_AGENT_VOL;
                            RACE_LOG_MSGID_I("MP_Step__3__ set A gain: 0x%x race_dsprt_anc_notify",1,arg);
                        } else {
                            pEvt->anc_id = RACE_ANC_SET_PARTNER_VOL;
                            RACE_LOG_MSGID_I("MP_Step__6__ set P gain: 0x%x race_dsprt_anc_notify",1,arg);
                        }
                        race_flush_packet((void *)pEvt, g_anc_race_ch_id);
                        *p_set_vol = 0;
                    }
                }
            } else if (event_id == ANC_CONTROL_EVENT_OFF) {
                if (g_anc_notify_off == 0) {
                    race_anc_mutex_give();
                    RACE_LOG_MSGID_I("race_dsprt_anc_notify, event:off, request is not from race cmd",0);
                    return;
                }
                if (fromPartner == 0) {
                    g_anc_notify_off |= RACE_ANC_AGENT;
                } else {
                    g_anc_notify_off |= RACE_ANC_PARTNER;
                }
#ifdef MTK_AWS_MCE_ENABLE
                if ((g_anc_notify_off & RACE_ANC_BOTH) == RACE_ANC_BOTH)
#else
                if (g_anc_notify_off & RACE_ANC_AGENT)
#endif
                {
                    race_dsprealtime_anc_struct *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE, RACE_DSPREALTIME_ANC, sizeof(race_dsprealtime_anc_struct), g_anc_race_ch_id);
                    if (pEvt != NULL) {
                        memset(pEvt, 0, sizeof(race_dsprealtime_anc_struct));
                        pEvt->anc_id = RACE_ANC_OFF;
                        RACE_LOG_MSGID_I("MP_Step__4__ set anc off",0);
                        race_flush_packet((void *)pEvt, g_anc_race_ch_id);
                        g_anc_notify_off = 0;
                    }
                }
            }
        }
#ifdef MTK_AWS_MCE_ENABLE
        else { //partner
            if (event_id == ANC_CONTROL_EVENT_ON) {
                if (arg == 0) {
                    race_anc_mutex_give();
                    RACE_LOG_MSGID_E("race_dsprt_anc_notify, event:on, partner wrong filter:0",0);
                    return;
                }
                anc_send_aws_mce_param(ANC_DIRECT, ANC_CONTROL_EVENT_ON, (uint32_t)arg, 0);
#ifndef MTK_ANC_V2
            } else if (event_id == ANC_CONTROL_EVENT_SET_VOLUME) {
                anc_send_aws_mce_param(ANC_DIRECT, ANC_CONTROL_EVENT_SET_VOLUME, (uint32_t)arg, 0);
#endif
            } else if (event_id == ANC_CONTROL_EVENT_OFF) {
                anc_send_aws_mce_param(ANC_DIRECT, ANC_CONTROL_EVENT_OFF, (uint32_t)arg, 0);
            }
        }
#endif
        race_anc_mutex_give();
    }
}

#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
static void anc_user_trigger_adaptive_ff_callback(int32_t Cal_status)
{
    extern anc_user_trigger_ff_param_t g_user_trigger_ff_info;
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_status_t ret = BT_SINK_SRV_STATUS_FAIL;

    RACE_LOG_MSGID_I("[user_trigger_ff]anc_user_trigger_adaptive_ff_callback, recieve status:%d ", Cal_status);
    if (Cal_status != USER_TRIGGER_FF_STATUS_DONE) {
        typedef struct
        {
            uint8_t status;
            uint8_t mode;
        }PACKED RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU;

        RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                                         RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK,
                                                         sizeof(RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU),
                                                         g_anc_race_ch_id);

        audio_anc_user_trigger_ff_stop();
        audio_anc_user_trigger_ff_recover_anc(Cal_status);

        if (pEvt) {
            pEvt->status = Cal_status;
            pEvt->mode = ADAPTIVE_FF_ANC_MODE;

            if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
                race_flush_packet((void *)pEvt, g_anc_race_ch_id);

            } else {/*partner send agent result*/
                race_send_pkt_t* pSndPkt;
                pSndPkt = (void *)race_pointer_cnv_pkt_to_send_pkt((void *)pEvt);
                race_pkt_t      *pret;
                race_send_pkt_t *psend;
                psend = (race_send_pkt_t *)pSndPkt;
                pret = &psend->race_data;
                #if (RACE_DEBUG_PRINT_ENABLE)
                race_dump((uint8_t *)pret, RACE_DBG_EVT);
                #endif

                if(pSndPkt) {
                    ret = bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, g_anc_race_ch_id, RACE_CMD_RSP_FROM_PARTNER,0);
                    if (ret != BT_STATUS_SUCCESS) {
                        RACE_LOG_MSGID_I("[relay_cmd][user_trigger_ff]partner send relay req FAIL \n", 0);
                    } else {
                        RACE_LOG_MSGID_I("[relay_cmd][user_trigger_ff]anc_user_trigger_adaptive_ff_callback, send status:%d success", pEvt->status);
//                        peq_relay_dbg.send_idx++;
                    }
                    race_mem_free(pSndPkt);
                }
            }
        }

    } else {
        typedef struct
        {
            uint8_t status;
            uint8_t mode;
            int32_t array[300];
        }PACKED RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU;

        RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                                         RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK,
                                                         sizeof(RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU),
                                                         g_anc_race_ch_id);

        if (pEvt) {
            pEvt->status = Cal_status;
            pEvt->mode = ADAPTIVE_FF_ANC_MODE;

            memcpy(pEvt->array, (g_user_trigger_ff_info.report_array+1), sizeof(int32_t) * 300);

            race_send_pkt_t* pSndPkt;
            uint32_t port_handle, ret_size, size;
            uint8_t *ptr;

            if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
                pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pEvt);
                port_handle = race_get_port_handle_by_channel_id(g_anc_race_ch_id);
                ret_size = race_port_send_data(port_handle, (uint8_t*)&pSndPkt->race_data, pSndPkt->length);

                size = pSndPkt->length;
                ptr = (uint8_t*)&pSndPkt->race_data;
                size -= ret_size;
                ptr += ret_size;
                while(size > 0)
                {
                    ret_size = race_port_send_data(port_handle, ptr, size);
                    size -= ret_size;
                    ptr += ret_size;
                }
                race_mem_free(pSndPkt);

            } else {
                pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pEvt);
                race_pkt_t      *pret;
                race_send_pkt_t *psend;
                psend = (race_send_pkt_t *)pSndPkt;
                pret = &psend->race_data;
                race_dump((uint8_t *)pret, RACE_DBG_EVT);
                RACE_LOG_MSGID_I("[relay_cmd][user_trigger_ff]anc_user_trigger_adaptive_ff_callback, status1:%d success", pEvt->status);
                if(pSndPkt) {
                    ret = bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, g_anc_race_ch_id, RACE_CMD_RSP_FROM_PARTNER,0);
                    if (ret != BT_STATUS_SUCCESS) {
                        RACE_LOG_MSGID_I("[relay_cmd][user_trigger_ff]partner send relay req FAIL \n", 0);
                    } else {
                        RACE_LOG_MSGID_I("[relay_cmd][user_trigger_ff]anc_user_trigger_adaptive_ff_callback, send status:%d success", pEvt->status);

                    }
                    race_mem_free(pSndPkt);
                }
            }
        }
    }

    RACE_LOG_MSGID_I("[user_trigger_ff]anc user trigger adaptive ff send status:%d, and waiting for response",1, Cal_status);

}

static void anc_user_trigger_adaptive_ff_receive_filter_callback(int32_t Cal_status)
{
    extern anc_user_trigger_ff_param_t g_user_trigger_ff_info;
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_status_t ret = BT_SINK_SRV_STATUS_FAIL;

    typedef struct
    {
        uint8_t status;
        uint8_t mode;
    }PACKED RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_EVT_STRU;

    RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_EVT_STRU *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                                         RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT,
                                                         sizeof(RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_EVT_STRU),
                                                         g_anc_race_ch_id);
    RACE_LOG_MSGID_I("[user_trigger_ff]anc_user_trigger_adaptive_ff_receive_filter_callback, send Cal_status:%d", 1, (int)Cal_status);

    if (pEvt) {
        pEvt->status = (uint8_t)Cal_status;
        pEvt->mode = ADAPTIVE_FF_ANC_MODE;
        if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
            race_flush_packet((void *)pEvt, g_anc_race_ch_id);

        } else {
            race_send_pkt_t* pSndPkt;
            pSndPkt = (void *)race_pointer_cnv_pkt_to_send_pkt((void *)pEvt);
            race_pkt_t      *pret;
            race_send_pkt_t *psend;
            psend = (race_send_pkt_t *)pSndPkt;
            pret = &psend->race_data;
            #if (RACE_DEBUG_PRINT_ENABLE)
            race_dump((uint8_t *)pret, RACE_DBG_EVT);
            #endif

            if(pSndPkt) {
                ret = bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, g_anc_race_ch_id, RACE_CMD_RSP_FROM_PARTNER,0);
                if (ret != BT_STATUS_SUCCESS) {
                    RACE_LOG_MSGID_I("[relay_cmd][user_trigger_ff]partner send relay req FAIL \n", 0);
                } else {
                    RACE_LOG_MSGID_I("[relay_cmd][user_trigger_ff]anc_user_trigger_adaptive_ff_receive_filter_callback, send status:%d success", pEvt->status);

                }
                race_mem_free(pSndPkt);
            }
        }
    }

    audio_anc_user_trigger_ff_stop();
    /*turn on ANC if needed*/
    audio_anc_user_trigger_ff_recover_anc(Cal_status);
    race_send_event_notify_msg(RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_END, NULL);
}
#endif
#endif

#else

#ifdef MTK_USER_TRIGGER_FF_ENABLE
void user_trigger_adaptive_ff_racecmd_response(uint8_t mode, uint8_t data_id, uint8_t *data_buff, uint16_t data_len, uint8_t start_or_stop, uint8_t seq_num, uint32_t completed_len)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_status_t ret = BT_SINK_SRV_STATUS_FAIL;
    RACE_LOG_MSGID_I("[user_trigger_ff] user_trigger_adaptive_ff_racecmd_response 1, mode:%d, data_id:%d data_len:%d data_buff:0x%x, start_or_stop:%d, seq_num:%d, completed_len:%d", 7, mode, data_id , data_len, data_buff, start_or_stop, seq_num, completed_len);
    typedef struct
    {
        adaptive_check_notify_t header;
        uint8_t data_id;
        uint8_t seq_num;
        uint8_t data[data_len];
    } PACKED user_trigger_adaptive_ff_notify_t;

    if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
        user_trigger_adaptive_ff_notify_t *pEvt = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                                                             RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK,
                                                             sizeof(user_trigger_adaptive_ff_notify_t),
                                                             g_anc_race_ch_id);
        if (pEvt) {
            pEvt->header.start_or_stop     = start_or_stop;
            pEvt->header.mode              = ADAPTIVE_FF_ANC_MODE;
            pEvt->header.bit_per_sample    = 2;
            pEvt->header.channel_num       = 0;
            pEvt->header.frame_size        = 0;
            pEvt->header.seq_num           = 0;
            pEvt->header.total_data_length = data_len+2;
            pEvt->header.data_length       = data_len;
            pEvt->data_id = data_id;
            pEvt->seq_num = seq_num;
            memcpy(pEvt->data, data_buff, data_len);


            race_send_pkt_t* pSndPkt;
            uint32_t port_handle, ret_size, size;
            uint8_t *ptr;
            uint32_t send_res = 0;

            pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pEvt);
            port_handle = race_get_port_handle_by_channel_id(g_anc_race_ch_id);

            size = pSndPkt->length;
            ptr = (uint8_t*)&pSndPkt->race_data;
            size -= completed_len;
            ptr += completed_len;

            #if (RACE_DEBUG_PRINT_ENABLE)
            race_dump((uint8_t *)ptr, RACE_DBG_EVT);
            #endif
            ret_size = race_port_send_data(port_handle, ptr, size);
            send_res = (ret_size & (1<<31));
            ret_size &= ~(1<<31);

            RACE_LOG_MSGID_I("[user_trigger_ff] (Agent)send_size:%u, send_res:%u, ret_size:%u", 3, size, send_res , ret_size);

            if (ret_size == size) {
                //transmit success
//                if (completed_len) {
//                    bt_aws_mce_report_info_t info;
//                    info.module_id = BT_AWS_MCE_REPORT_MODULE_USR_TRIGR_FF;
//                    info.param_len = sizeof(uint32_t);
//                    uint32_t event_id = USER_TRIGGER_ADAPTIVE_FF_PARTNER_CONTINUE_SEND_DATA;//let partner resume timer
//                    info.param = &event_id;
//                    if (bt_aws_mce_report_send_event(&info) != BT_STATUS_SUCCESS) {
//                        RACE_LOG_MSGID_E("[user_trigger_ff] inform Partner fail", 0);
//                    }
//                }
//                user_trigger_adaptive_ff_remove_item_from_queue(true);
                RACE_LOG_MSGID_I("[user_trigger_ff] (Agent)Send data to APK, data_id:%d seq_num:%d", 2,data_id , seq_num);
            } else if (ret_size == 0){
                RACE_LOG_MSGID_E("[user_trigger_ff] (Agent)pkt loss, wait for next time", 0);
            } else {
//                bt_aws_mce_report_info_t info;
//                info.module_id = BT_AWS_MCE_REPORT_MODULE_USR_TRIGR_FF;
//                info.param_len = sizeof(uint32_t);
//                uint32_t event_id = USER_TRIGGER_ADAPTIVE_FF_AGENT_SEND_RACE_FAIL;//let partner stop sending data
//                info.param = &event_id;
//                if (bt_aws_mce_report_send_event(&info) != BT_STATUS_SUCCESS) {
//                    RACE_LOG_MSGID_E("[user_trigger_ff] inform Partner fail", 0);
//                }

                RACE_LOG_MSGID_E("[user_trigger_ff] (Agent)remain len:%d, wait for next time", 1, size-ret_size);
//                user_trigger_adaptive_ff_replace_item_from_queue(true, completed_len+ret_size);
            }

            race_mem_free(pSndPkt);
        }

    } else {
        user_trigger_adaptive_ff_notify_t *pEvt = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                                                             RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK,
                                                             sizeof(user_trigger_adaptive_ff_notify_t),
                                                             g_anc_race_ch_id);
        if (pEvt) {
            pEvt->header.start_or_stop     = start_or_stop;
            pEvt->header.mode              = ADAPTIVE_FF_ANC_MODE;
            pEvt->header.bit_per_sample    = 2;
            pEvt->header.channel_num       = 0;
            pEvt->header.frame_size        = 0;
            pEvt->header.seq_num           = 0;
            pEvt->header.total_data_length = data_len+2;
            pEvt->header.data_length       = data_len;
            pEvt->data_id = data_id;
            pEvt->seq_num = seq_num;
            memcpy(pEvt->data, data_buff, data_len);



            race_send_pkt_t* pSndPkt;

            pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pEvt);
            race_pkt_t      *pret;
            race_send_pkt_t *psend;
            psend = (race_send_pkt_t *)pSndPkt;
            pret = &psend->race_data;
            #if (RACE_DEBUG_PRINT_ENABLE)
            race_dump((uint8_t *)pret, RACE_DBG_EVT);
            #endif

            if(pSndPkt) {

                ret = bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, g_anc_race_ch_id, RACE_CMD_RSP_FROM_PARTNER,0);

                if (ret != BT_STATUS_SUCCESS) {
                    RACE_LOG_MSGID_E("[relay_cmd][user_trigger_ff]partner send relay req FAIL \n", 0);
                } else {
//                    audio_user_trigger_adaptive_ff_set_sending_data_status(true);
                    RACE_LOG_MSGID_I("[relay_cmd][user_trigger_ff]partner send relay req, send id:%d, seq_num:%d success", 2, pEvt->data_id, pEvt->seq_num);
                }
                race_mem_free(pSndPkt);
            }
        }
    }
}

void user_trigger_adaptive_ff_racecmd_response_extend(uint8_t race_type, uint8_t status, uint16_t data_len, void *data_buff)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_status_t ret = BT_SINK_SRV_STATUS_FAIL;
    RACE_LOG_MSGID_I("[user_trigger_ff] user_trigger_adaptive_ff_racecmd_response_extend, race_type:%x, status:%d data_len:%d data_buff:0x%x", 4, race_type, status , data_len, data_buff);

    typedef struct {
        uint8_t status;
        uint8_t mode;
        uint16_t data_len;
        uint8_t data;
    } PACKED user_trigger_ff_extend_cmd_resp_t;

    user_trigger_ff_extend_cmd_resp_t *pEvt = RACE_ClaimPacket(race_type,
                                                             RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND,
                                                             sizeof(user_trigger_ff_extend_cmd_resp_t)+data_len-1,
                                                             g_anc_race_ch_id);
    if (pEvt) {
        pEvt->status = status;
        pEvt->mode = ADAPTIVE_FF_ANC_MODE;
        pEvt->data_len = data_len;
        memcpy(&pEvt->data, data_buff, data_len);
//        vPortFree(data_buff);

        if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
            race_send_pkt_t* pSndPkt;
            uint32_t port_handle, ret_size, size;
            uint8_t *ptr;
            uint32_t send_res = 0;

            pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pEvt);
            port_handle = race_get_port_handle_by_channel_id(g_anc_race_ch_id);

            size = pSndPkt->length;
            ptr = (uint8_t*)&pSndPkt->race_data;

            #if (RACE_DEBUG_PRINT_ENABLE)
            race_dump((uint8_t *)ptr, RACE_DBG_EVT);
            #endif

            ret_size = race_port_send_data(port_handle, ptr, size);
            send_res = (ret_size & (1<<31));
            ret_size &= ~(1<<31);

            RACE_LOG_MSGID_I("[user_trigger_ff] (Agent)send_size:%u, send_res:%u, ret_size:%u", 3, size, send_res , ret_size);
            race_mem_free(pSndPkt);

        } else {//role == BT_AWS_MCE_ROLE_PARTNER
            race_send_pkt_t* pSndPkt;

            pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pEvt);
            race_pkt_t      *pret;
            race_send_pkt_t *psend;
            psend = (race_send_pkt_t *)pSndPkt;
            pret = &psend->race_data;
            #if (RACE_DEBUG_PRINT_ENABLE)
            race_dump((uint8_t *)pret, RACE_DBG_EVT);
            #endif
            if(pSndPkt) {
                ret = bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, g_anc_race_ch_id, RACE_CMD_RSP_FROM_PARTNER,0);

                if (ret != BT_STATUS_SUCCESS) {
                    RACE_LOG_MSGID_E("[relay_cmd][user_trigger_ff]partner send relay req FAIL \n", 0);
                }
                race_mem_free(pSndPkt);
            }
        }
    }
}

#endif

#endif

#ifdef MTK_LEAKAGE_DETECTION_ENABLE
void audio_anc_leakage_detection_timer_check_result(void);

void anc_leakage_detection_racecmd_callback(uint16_t leakage_status)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    RACE_LOG_MSGID_I("[RECORD_LC] anc_leakage_detection_racecmd_callback result:%d for role:0x%x",2, leakage_status, role);

    audio_anc_leakage_compensation_stop();

    if (leakage_status == LD_STATUS_PASS) {
        leakage_status = 0; //re-map
    }

    if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
        anc_leakage_detection_racecmd_response(leakage_status, 0);

    } else {
        #ifndef MTK_ANC_V2
        anc_send_aws_mce_param(ANC_DIRECT, ANC_CONTROL_EVENT_LD_START, (uint32_t)leakage_status, 0);
        #else
//        audio_anc_control_cap_t anc_cap;
//        anc_cap.target_device = TO_THEOTHER;
//        anc_cap.control_misc.extend_use_parameters = (uint32_t)leakage_status;
//        anc_send_aws_mce_param(ANC_DIRECT, ANC_CONTROL_EVENT_LD_START, &anc_cap, 0);
        audio_anc_leakage_detection_sync_para_t arg;
        arg.target_device = TO_THEOTHER;
        arg.extend_param = (uint32_t)leakage_status;
        audio_anc_leakage_detection_send_aws_mce_param(LD_DIRECT, AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_START, &arg, 0);
        #endif
    }

    //no need to resume a2dp because a2dp will be resumed by AM when VP close.
    //am_audio_dl_resume();

}

void anc_leakage_detection_racecmd_response(uint16_t leakage_status, uint8_t fromPartner)
{
    typedef struct
    {
        adaptive_check_notify_t header;
        uint8_t agent_status;
        uint8_t partner_status;
    } PACKED leakage_detection_notify_t;

    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    race_anc_mutex_take();
    if (fromPartner == 0) {
        g_LD_result_agent = leakage_status;
        g_LD_result_agent |= 0x8000;
        RACE_LOG_MSGID_I("[RECORD_LC] Get agent LD result : %d",1,leakage_status);
    } else {
        g_LD_result_partner = leakage_status;
        g_LD_result_partner |= 0x8000;
        RACE_LOG_MSGID_I("[RECORD_LC] Get partner LD result : %d",1,leakage_status);
    }

    if ((role == BT_AWS_MCE_ROLE_NONE)
        || ((bt_sink_srv_cm_get_special_aws_device() != NULL) && (bt_sink_srv_cm_get_aws_link_state() != BT_AWS_MCE_AGENT_STATE_ATTACHED))
        || ((g_LD_result_agent & 0x8000) && (g_LD_result_partner & 0x8000))) {
        leakage_detection_notify_t *pEvt = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                                                         RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK,
                                                         sizeof(leakage_detection_notify_t),
                                                         g_leakage_detection_race_ch_id);

        if (g_leakage_detection_race_ch_id == 0) {
            RACE_LOG_MSGID_E("[RECORD_LC] g_leakage_detection_race_ch_id == 0", 0);
        }

        if ((g_LD_result_partner & 0x8000) == 0) {
            g_LD_result_partner = LD_STATUS_ABSENT;
        }
        if (pEvt) {
            pEvt->header.start_or_stop     = 1;
            pEvt->header.mode              = LEAKAGE_DETECTION_MODE;
            pEvt->header.bit_per_sample    = 2;
            pEvt->header.channel_num       = 0;
            pEvt->header.frame_size        = 0;
            pEvt->header.seq_num           = 0;
            pEvt->header.total_data_length = 2;
            pEvt->header.data_length       = 2;
            pEvt->agent_status             = (uint8_t)(g_LD_result_agent & 0xFF);
            pEvt->partner_status           = (uint8_t)(g_LD_result_partner & 0xFF);
            race_flush_packet((void *)pEvt, g_leakage_detection_race_ch_id);
            RACE_LOG_MSGID_I("[RECORD_LC] Send LD result to APK : %d %d",2,pEvt->agent_status,pEvt->partner_status);
        }
        g_LD_result_agent = 0;
        g_LD_result_partner = 0;
        #ifdef MTK_LEAKAGE_DETECTION_ENABLE
        // race_cmd_vp_struct* param = (race_cmd_vp_struct*)race_mem_alloc(sizeof(race_cmd_vp_struct));
        // if(param){
        //     param->vp_type = RACE_CMD_VP_LEAKAGE_DETECTION;
        //     param->play_flag = false;
        //     RACE_ERRCODE ret = race_send_event_notify_msg(RACE_EVENT_TYPE_CMD_VP, param);
        //     if(RACE_ERRCODE_SUCCESS != ret){
        //         race_mem_free(param);
        //     }
        // }
        #endif
        #ifdef MTK_ANC_V2
        audio_anc_leakage_detection_resume_dl();
        #endif
    } else {
        //waiting the result of the other device for 1 sec
        audio_anc_leakage_detection_timer_check_result();
    }
    race_anc_mutex_give();
}

static void audio_anc_leakage_detection_oneshot_timer_callback(TimerHandle_t xTimer)
{
    RACE_LOG_MSGID_I("[RECORD_LC]audio_anc_leakage_detection_oneshot_timer_callback\r\n", 0);
    if (((g_LD_result_agent& 0x8000) != 0) && ((g_LD_result_partner & 0x8000) == 0)) {
        RACE_LOG_MSGID_I("[RECORD_LC]stop waiting for partner's result\r\n", 0);
        anc_leakage_detection_racecmd_response(LD_STATUS_ABSENT, 1);
    }
}

void audio_anc_leakage_detection_timer_check_result(void)
{
    if (s_xLeakageDetectionOneShotTimer == NULL) {
        s_xLeakageDetectionOneShotTimer = xTimerCreate("UserTriggerFFOneShot",
                                               1000 / portTICK_PERIOD_MS,
                                               pdFALSE,
                                               0,
                                               audio_anc_leakage_detection_oneshot_timer_callback);
        if (s_xLeakageDetectionOneShotTimer == NULL) {
            RACE_LOG_MSGID_I("[RECORD_LC]create one_shot Timer error.\n", 0);

        } else {
            if (xTimerStart(s_xLeakageDetectionOneShotTimer, 0) != pdPASS) {
                RACE_LOG_MSGID_I("[RECORD_LC]Timer start error.\r\n", 0);

            }
        }
    } else {
        if (xTimerReset(s_xLeakageDetectionOneShotTimer, 0) != pdPASS) {
            RACE_LOG_MSGID_I("[RECORD_LC]Timer reset error.\r\n", 0);

        }
    }

}
#endif
#endif

/**
 * RACE_DSPREALTIME_SUSPEND_HDR
 *
 * RACE_DSPREALTIME_SUSPEND_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_DSPREALTIME_SUSPEND_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
	typedef struct
	{
		uint8_t Status;
	}PACKED RACE_DSPREALTIME_SUSPEND_EVT_STRU;

	RACE_LOG_MSGID_I("RACE_DSPREALTIME_SUSPEND_HDR() enter, channel_id = %x \r\n",1, channel_id);

	RACE_DSPREALTIME_SUSPEND_EVT_STRU* pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_DSPREALTIME_SUSPEND, (uint16_t)sizeof(RACE_DSPREALTIME_SUSPEND_EVT_STRU), channel_id);
    bt_sink_srv_am_result_t am_status;

    if (pEvt != NULL)
    {
        am_status = am_audio_set_pause();
        pEvt->Status = am_status != AUD_EXECUTION_SUCCESS ? RACE_ERRCODE_FAIL : RACE_ERRCODE_SUCCESS;
    }

	return pEvt;
}


/**
 * RACE_DSPREALTIME_RESUME_HDR
 *
 * RACE_DSPREALTIME_RESUME_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_DSPREALTIME_RESUME_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
	typedef struct
	{
		uint8_t Status;
	}PACKED RACE_DSPREALTIME_RESUME_EVT_STRU;

	RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);

	RACE_DSPREALTIME_RESUME_EVT_STRU* pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_DSPREALTIME_RESUME, (uint16_t)sizeof(RACE_DSPREALTIME_RESUME_EVT_STRU), channel_id);
    bt_sink_srv_am_result_t am_status;


    if (pEvt != NULL)
    {
        //API
        am_status = am_audio_set_resume();
        if (am_status != AUD_EXECUTION_SUCCESS) pEvt->Status = (uint8_t)RACE_ERRCODE_FAIL;
        else pEvt->Status = (uint8_t)RACE_ERRCODE_SUCCESS;
    }

    return pEvt;
}

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
/**
 * RACE_DSPREALTIME_PEQ_HDR
 *
 * RACE_DSPREALTIME_PEQ_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_DSPREALTIME_PEQ_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t peq_PhaseAndPath;    //0x00:a2dp pre peq  0x01:a2dp  post peq  0x10:LINE_IN pre peq  0x11:LINE_IN post peq
        uint8_t Peq_param[1];
    }PACKED RACE_DSPREALTIME_PEQ_CMD_STRU;

    typedef struct
    {
        uint8_t Status;
    }PACKED RACE_DSPREALTIME_PEQ_EVT_STRU;

    RACE_DSPREALTIME_PEQ_CMD_STRU *pCmd = (RACE_DSPREALTIME_PEQ_CMD_STRU *)pCmdMsg;
    uint16_t Peq_param_length = (pCmd->Hdr.length >= 3) ? (pCmd->Hdr.length - 3) : 0; //Hdr.id: 2bytes, peq_phase: 1byte
    uint32_t ret = 1;

    RACE_LOG_MSGID_I("RACE_DSPREALTIME_PEQ_HDR Peq_param_length=%d\n",1, Peq_param_length);

    RACE_DSPREALTIME_PEQ_EVT_STRU* pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_DSPREALTIME_PEQ, (uint16_t)sizeof(RACE_DSPREALTIME_PEQ_EVT_STRU), channel_id);
    if(((pCmd->peq_PhaseAndPath != 0) && (pCmd->peq_PhaseAndPath != 1) && (pCmd->peq_PhaseAndPath != 0x10)&& (pCmd->peq_PhaseAndPath != 0x11)&& (pCmd->peq_PhaseAndPath != PEQ_SPECIAL_PHASE))
        || ((pCmd->peq_PhaseAndPath != PEQ_SPECIAL_PHASE) && (Peq_param_length < (5*1*2+2+1)))
        || ((pCmd->peq_PhaseAndPath == PEQ_SPECIAL_PHASE) && (Peq_param_length < 1))) {
        RACE_LOG_MSGID_E("Un-supported phase id: %d or abnormal param length: %d\n",2,pCmd->peq_PhaseAndPath,Peq_param_length);
        if (pEvt != NULL) {
            pEvt->Status = (uint8_t)RACE_ERRCODE_FAIL;
        }
        return pEvt;
    }

    if (pEvt != NULL)
    {
        bt_clock_t target_bt_clk = {0};
        uint8_t setting_mode = PEQ_DIRECT;
        bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
        bt_sink_srv_am_type_t cur_type = NONE;
        uint8_t LINE_INENABLE = 0;
        am_feature_type_t audio_path_id = 0;

        cur_type = bt_sink_srv_ami_get_current_scenario();
#ifdef AIR_WIRED_AUDIO_ENABLE
        audio_transmitter_scenario_list_t audio_transmitter_scenario_list[]  =  {
            {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_IN},
            {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER},
            {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0},
            {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1},
        };

        LINE_INENABLE = audio_transmitter_get_is_running_by_scenario_list(audio_transmitter_scenario_list, sizeof(audio_transmitter_scenario_list)/sizeof(audio_transmitter_scenario_list_t));
#endif
        if ((cur_type == A2DP || cur_type == BLE) && (pCmd->peq_PhaseAndPath == 0x00 || pCmd->peq_PhaseAndPath == 0x01)) {
            audio_path_id = AM_A2DP_PEQ;
        } else if (LINE_INENABLE == true && (pCmd->peq_PhaseAndPath == 0x10 || pCmd->peq_PhaseAndPath == 0x11)) {
            audio_path_id = AM_LINEIN_PEQ;
        } else if(cur_type == LINE_IN || LINE_INENABLE == true){
            //compatible for old version of tool of line in realtime PEQ setting
            audio_path_id = AM_LINEIN_PEQ;
            }
        else {
            audio_path_id = AM_A2DP_PEQ;//to fix return fail while nothing playing
            RACE_LOG_MSGID_E("Un-supported scenario: %d\n",1,cur_type);
        }
#ifdef MTK_AWS_MCE_ENABLE
        race_dsprt_peq_get_target_bt_clk(role, &setting_mode, &target_bt_clk);
#endif
        if((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
            if(pCmd->peq_PhaseAndPath == PEQ_SPECIAL_PHASE) {
                ret = race_dsprt_peq_change_mode_data(0, setting_mode, target_bt_clk.nclk, pCmd->Peq_param[0], PEQ_SOUND_MODE_FORCE_DRC, audio_path_id);

                #ifndef MTK_ANC_ENABLE
                ret = race_dsprt_peq_change_mode_data(1, setting_mode, target_bt_clk.nclk, pCmd->Peq_param[0], PEQ_SOUND_MODE_FORCE_DRC, audio_path_id);
                #else
                if (pCmd->Peq_param[0] == 1) {
                #ifndef MTK_ANC_V2
                    uint8_t anc_enable;
                    uint8_t hybrid_enable;
                    anc_get_status(&anc_enable, NULL, &hybrid_enable);
                    if ((anc_enable > 0) && (hybrid_enable > 0)) {
                        ret = race_dsprt_peq_change_mode_data(1, setting_mode, target_bt_clk.nclk, pCmd->Peq_param[0], PEQ_SOUND_MODE_FORCE_DRC, audio_path_id);
                    } else {
                        ret = race_dsprt_peq_change_mode_data(1, setting_mode, target_bt_clk.nclk, 0, PEQ_SOUND_MODE_UNASSIGNED, audio_path_id);
                    }
                #else
                    ret = race_dsprt_peq_change_mode_data(1, setting_mode, target_bt_clk.nclk, 0, PEQ_SOUND_MODE_UNASSIGNED, audio_path_id);
                #endif
                } else {
                    ret = race_dsprt_peq_change_mode_data(1, setting_mode, target_bt_clk.nclk, pCmd->Peq_param[0], PEQ_SOUND_MODE_FORCE_DRC, audio_path_id);
                }
                #endif
            } else {
                if(pCmd->peq_PhaseAndPath == 0x00 || pCmd->peq_PhaseAndPath == 0x10){
                #ifdef AIR_A2DP_LINEIN_SEPERATE_PEQ_ENABLE
                    ret = race_dsprt_peq_realtime_data(0, setting_mode, target_bt_clk.nclk,(uint8_t *)&pCmd->Peq_param,(uint32_t)Peq_param_length, audio_path_id);
                #else
                    if(cur_type == A2DP || cur_type == BLE) {
                        ret = race_dsprt_peq_realtime_data(0, setting_mode, target_bt_clk.nclk,(uint8_t *)&pCmd->Peq_param,(uint32_t)Peq_param_length, AM_A2DP_PEQ);
                    } else if(cur_type == LINE_IN || LINE_INENABLE == true) {
                        ret = race_dsprt_peq_realtime_data(0, setting_mode, target_bt_clk.nclk,(uint8_t *)&pCmd->Peq_param,(uint32_t)Peq_param_length, AM_LINEIN_PEQ);
                    } else if(cur_type == USB_AUDIO_IN) {
                        ret = race_dsprt_peq_realtime_data(0, setting_mode, target_bt_clk.nclk,(uint8_t *)&pCmd->Peq_param,(uint32_t)Peq_param_length, AM_A2DP_PEQ);
                    }
                #endif
                }
                else{
                    ret = race_dsprt_peq_realtime_data(1, setting_mode, target_bt_clk.nclk,(uint8_t *)&pCmd->Peq_param,(uint32_t)Peq_param_length, audio_path_id);
                }
            }
        } else {
            ret = 1;
            RACE_LOG_MSGID_W("RACE_DSPREALTIME_PEQ_HDR role: 0x%x error\n",1,role);
        }

        if(ret == 0) {
            pEvt->Status = (uint8_t)RACE_ERRCODE_SUCCESS;
        } else {
            pEvt->Status = (uint8_t)RACE_ERRCODE_FAIL;
        }
    }

    return pEvt;
}
#endif

/**
 * RACE_DSPREALTIME_GET_REFRENCE_GAIN_HDR
 *
 * RACE_DSPREALTIME_GET_REFRENCE_GAIN_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_DSPREALTIME_GET_REFRENCE_GAIN_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        uint8_t status;
    }PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    RSP *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                 RACE_DSPREALTIME_GET_REFRENCE_GAIN,
                                 sizeof(RSP),
                                 channel_id);
    if (pEvt)
    {
        race_dsprealtime_get_refrence_gain_noti_struct *noti = NULL;

        /* A1. Execute the cmd. */
        uint8_t *ref_gain = (uint8_t *)hal_audio_query_race_cmd_audio_buf();

        if (ref_gain)
        {
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_REQ_GET_REALTIME_REF_GAIN,
                                                  0,
                                                  (uint32_t)ref_gain,
                                                  true);
            /* A2. Create the noti. */
            noti = (void *)RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                                            RACE_DSPREALTIME_GET_REFRENCE_GAIN,
                                            sizeof(race_dsprealtime_get_refrence_gain_noti_struct),
                                            channel_id);
            if (noti)
            {
                /* A3. Set the noti parameters with the cmd results.  */
                memcpy(noti->Data, ref_gain, sizeof(noti->Data));

                /* A4. Send the noti. */
                ret = race_noti_send(noti, channel_id, TRUE);
                if (RACE_ERRCODE_SUCCESS != ret)
                {
                    /* A5. Free the noti if needed. */
                    RACE_FreePacket(noti);
                    noti = NULL;
                }
            }
            else
            {
                ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
            }
        }

        pEvt->status = ret;
    }

    return pEvt;
}

#ifdef MTK_AIRDUMP_EN_MIC_RECORD
void airdump_common_timer_callback(void);
airdump_timer_result_t airdump_common_timer_create(void)
{
    airdump_timer_result_t ret = AIRDUMP_EXECUTION_SUCCESS;
    int32_t duration_ms = 8;
    if(g_airdump_common_timer == NULL) {
        g_airdump_common_timer = xTimerCreate("AirCMDumpTimer",
                                       (duration_ms / portTICK_PERIOD_MS),
                                       pdTRUE,
                                       0,
                                       (TimerCallbackFunction_t)airdump_common_timer_callback);
        if(g_airdump_common_timer == NULL) {
            RACE_LOG_MSGID_I("[AirDump][COMMON][CM4] create timer failed\n", 0);
            ret = AIRDUMP_EXECUTION_FAIL;
        }else {
            RACE_LOG_MSGID_I("[AirDump][COMMON][CM4] create timer success\n", 0);
            ret = AIRDUMP_EXECUTION_SUCCESS;
        }
    }
    return ret;
}

void airdump_common_timer_delete(void)
{
    if(g_airdump_common_timer != NULL)
    {
        if (pdPASS == xTimerDelete(g_airdump_common_timer, 0))
        {
            RACE_LOG_MSGID_I("[AirDump][COMMON]airdump_timer_delete\n", 0);
            g_airdump_common_timer = NULL;
        }
    }
}
void airdump_common_timer_callback(void)
{
#if 0
    uint32_t curr_cnt = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
    RACE_LOG_MSGID_I("[AirDump][COMMON] hisr(%d)(%d): timer start);\r\n", 2,(curr_cnt / 1000), curr_cnt);
#endif
	race_dsprealtime_airdump_common_notify_struct *pRacePkt;
#if 1
    //send race cmd back to APP
    uint32_t frameSize = 128 * sizeof(uint16_t);
    uint32_t dspBufsize = audio_record_control_get_share_buf_data_byte_count();
    record_control_result_t result = RECORD_CONTROL_EXECUTION_FAIL;
    if(dspBufsize >= frameSize){
        result = audio_record_control_read_data(&g_record_airdump_data, frameSize);
        if(result != RECORD_CONTROL_EXECUTION_SUCCESS){
            RACE_LOG_MSGID_E("[AirDump][COMMON] audio_record_control_read_data fail\n",0);
        }
        pRacePkt = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION, (uint16_t)RACE_DSPREALTIME_AIRDUMP_ENTRY, sizeof(race_dsprealtime_airdump_common_notify_struct), g_airdump_common_race_ch_id);
        pRacePkt->airdump_id = Airdump_mic_record;
        pRacePkt->param.air_dump_mic_record_notify.Data_sequence_number = g_airdump_common_cnt;
        pRacePkt->param.air_dump_mic_record_notify.Data_size_per_sample = 2;
        pRacePkt->param.air_dump_mic_record_notify.Dump_Request = g_airdump_common_race_request;
        pRacePkt->param.air_dump_mic_record_notify.Data_length = 128;
        memcpy((int16_t *)pRacePkt->airdump_data, (int16_t *)&g_record_airdump_data, pRacePkt->param.air_dump_mic_record_notify.Data_length * sizeof(uint16_t) * 2);
        race_flush_packet((void *)pRacePkt, g_airdump_common_race_ch_id);
    } else {
        RACE_LOG_MSGID_I("[AirDump][COMMON] Airdump bufer size low %d", 1, dspBufsize);
        return;
    }
#else
    pRacePkt = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION, (uint16_t)RACE_DSPREALTIME_AIRDUMP_ENTRY, sizeof(race_dsprealtime_airdump_common_notify_struct), g_airdump_common_race_ch_id);
    if(pRacePkt != NULL)
    {
        pRacePkt->airdump_id = Airdump_mic_record;
        pRacePkt->param.air_dump_mic_record_notify.Data_sequence_number = g_airdump_common_cnt;
        pRacePkt->param.air_dump_mic_record_notify.Data_size_per_sample = 2;
        pRacePkt->param.air_dump_mic_record_notify.Dump_Request = g_airdump_common_race_request;
        pRacePkt->param.air_dump_mic_record_notify.Data_length = 128;
        memcpy((int16_t *)pRacePkt->airdump_data, (int16_t *)&g_128_1ktone, pRacePkt->param.air_dump_mic_record_notify.Data_length * sizeof(uint16_t));
        memcpy((int16_t *)pRacePkt->airdump_data + (pRacePkt->param.air_dump_mic_record_notify.Data_length), (uint8_t *)&g_128_2ktone, pRacePkt->param.air_dump_mic_record_notify.Data_length * sizeof(uint16_t));
        race_flush_packet((void *)pRacePkt, g_airdump_common_race_ch_id);
    }
#endif
    g_airdump_common_cnt++;
#if 0
    /*For debug dump.*/
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
    RACE_LOG_MSGID_I("[AirDump][COMMON] hisr(%d)(%d): timer end);\r\n", 2, (curr_cnt / 1000), curr_cnt);
#endif
}

/**
 * RACE_DSPREALTIME_AIRDUMP_COMMON_HDR
 *
 * RACE_DSPREALTIME_AIRDUMP_COMMON_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_DSPREALTIME_AIRDUMP_COMMON_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    race_dsprealtime_airdump_common_response_struct *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                   (uint16_t)RACE_DSPREALTIME_AIRDUMP_ENTRY,
                                                                   (uint16_t)sizeof(race_dsprealtime_airdump_common_response_struct),
                                                                   channel_id);
    g_airdump_common_race_ch_id = channel_id;
    record_control_result_t Airdump_common_ret = RECORD_CONTROL_EXECUTION_FAIL;
    if (pEvt)
    {
        race_dsprealtime_airdump_common_request_struct *pAirdump_common_Cmd;
        //uint16_t Airdump_common_param_length;
        pAirdump_common_Cmd = (race_dsprealtime_airdump_common_request_struct *)pCmdMsg->payload;
        memset(pEvt, 0, sizeof(race_dsprealtime_airdump_common_response_struct));
        RACE_LOG_MSGID_I("RACE_DSPREALTIME_AIRDUMP_COMMON_HDR payload: %d %d %d\n", 3, pAirdump_common_Cmd->airdump_id, pAirdump_common_Cmd->param.air_dump_mic_record_request.Dump_Enable, pAirdump_common_Cmd->param.air_dump_mic_record_request.Dump_Request);
        switch (pAirdump_common_Cmd->airdump_id) {
            case Airdump_mic_record:
                if(pAirdump_common_Cmd->param.air_dump_mic_record_request.Dump_Enable == 1){
                    g_airdump_common_race_request = pAirdump_common_Cmd->param.air_dump_mic_record_request.Dump_Request;
                    hal_audio_set_dvfs_control(HAL_AUDIO_DVFS_MAX_SPEED, HAL_AUDIO_DVFS_LOCK);
                    /*For debug dump.*/
                    //g_dump = true;
                    g_record_airdump = true;
                    Airdump_common_ret = audio_record_control_airdump(true , pAirdump_common_Cmd->param.air_dump_mic_record_request.Dump_Request);
                    airdump_common_timer_create();
                    xTimerStart(g_airdump_common_timer, 0);
                } else if(pAirdump_common_Cmd->param.air_dump_mic_record_request.Dump_Enable == 0){
                    Airdump_common_ret = audio_record_control_airdump(false, pAirdump_common_Cmd->param.air_dump_mic_record_request.Dump_Request);
                    airdump_common_timer_delete();
                    /*For debug dump.*/
                    //g_dump = false;
                    g_record_airdump = false;
                    hal_audio_set_dvfs_control(HAL_AUDIO_DVFS_MAX_SPEED, HAL_AUDIO_DVFS_UNLOCK);
                    g_airdump_common_cnt = 0;
                } else {
                    pEvt->status = RACE_ERRCODE_PARAMETER_ERROR;
                    return pEvt;
                }
                break;
            default:
                pEvt->status = RACE_ERRCODE_PARAMETER_ERROR;
                return pEvt;
        }
        pEvt->status = (Airdump_common_ret == RECORD_CONTROL_EXECUTION_SUCCESS) ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
    }
    RACE_LOG_MSGID_E("RACE_DSPREALTIME_AIRDUMP_COMMON_HDR pEvt->status: %d  Airdump_common_ret: %d\n",2,pEvt->status,Airdump_common_ret);
    return pEvt;
}
#endif

#ifdef MTK_AIRDUMP_EN
airdump_timer_result_t airdump_timer_create(void)
{
    airdump_timer_result_t ret = AIRDUMP_EXECUTION_SUCCESS;
    int32_t duration = 20000;
    if(g_airdump_timer == NULL) {
        g_airdump_timer = xTimerCreate("AirDumpTimer",
                                       (duration / 1000 / portTICK_PERIOD_MS),
                                       pdTRUE,
                                       0,
                                       (TimerCallbackFunction_t)airdump_timer_callback);
        if(g_airdump_timer == NULL) {
            RACE_LOG_MSGID_I("[AirDump][CM4] create timer failed\n", 0);
            ret = AIRDUMP_EXECUTION_FAIL;
        }else {
            RACE_LOG_MSGID_I("[AirDump][CM4] create timer success\n", 0);
            ret = AIRDUMP_EXECUTION_SUCCESS;
        }
    }
    return ret;
}

void airdump_timer_delete(void)
{
    if(g_airdump_timer != NULL)
    {
        if (pdPASS == xTimerDelete(g_airdump_timer, 0))
        {
            RACE_LOG_MSGID_I("[AirDump] airdump_timer_delete\n", 0);
            g_airdump_timer = NULL;
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AIRDUMP_EN, 0, 0, false);
        }
    }
}

void airdump_timer_callback(void)
{
    //RACE_LOG_MSGID_I("[AirDump][CM4] airdump_timer_callback start\n", 0);
    AIRDUMP_BUFFER_INFO *buffer;
    buffer = (AIRDUMP_BUFFER_INFO *)hal_audio_query_hfp_air_dump();
    uint8_t airdump_cnt_now = buffer->notify_count;
    if(g_airdump_cnt_past != airdump_cnt_now)
    {
        //send race cmd back to APP
        RACE_AirDump_Send((uint8_t)g_airdump_race_ch_id,
                          (uint8_t)RACE_TYPE_NOTIFICATION,
                          (uint8_t *)(&buffer->data[0] + buffer->read_offset),
                          (uint16_t)sizeof(RACE_DSPREALTIME_AIRDUMP_EVT_NOTI_STRU));

        uint32_t ro = (buffer->read_offset + AEC_AIRDUMP_FRAME_SIZE*AEC_AIRDUMP_FRAME_NUM_HALF)%buffer->length; // move frame size
        buffer->read_offset = ro;
        //RACE_LOG_MSGID_I("[AirDump][CM4] update ro: %d, wo:%d, cnt_past:%d, cnt_now:%d",2,buffer->read_offset, buffer->write_offset, g_airdump_cnt_past, airdump_cnt_now);
        g_airdump_cnt_past = airdump_cnt_now;
    }
}

void RACE_AirDump_Send(uint8_t channelId,uint8_t RaceType, uint8_t *ptr, uint16_t len)
{
	RACE_DSPREALTIME_AIRDUMP_EVT_NOTI_STRU *pRacePkt = RACE_ClaimPacket(RaceType, (uint16_t)RACE_DSPREALTIME_AIRDUMP_ON_OFF, len, channelId);
	if(pRacePkt != NULL)
	{
		memcpy((uint8_t *)pRacePkt->data, ptr, len);
		race_flush_packet((void *)pRacePkt, g_airdump_race_ch_id);
	}
}

/**
 * RACE_DSPREALTIME_AIRDUMP_HDR
 *
 * RACE_DSPREALTIME_AIRDUMP_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_DSPREALTIME_AIRDUMP_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    g_airdump_race_ch_id = channel_id;
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t AirDumpEn;
    }PACKED RACE_DSPREALTIME_AIRDUMP_CMD_STRU;

    typedef struct
    {
        uint8_t status;
    }PACKED RACE_DSPREALTIME_AIRDUMP_EVT_RES_STRU;

    RACE_DSPREALTIME_AIRDUMP_EVT_RES_STRU *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                   (uint16_t)RACE_DSPREALTIME_AIRDUMP_ON_OFF,
                                                                   (uint16_t)sizeof(RACE_DSPREALTIME_AIRDUMP_EVT_RES_STRU),
                                                                   channel_id);
    if (pEvt)
    {
        RACE_DSPREALTIME_AIRDUMP_CMD_STRU *pCmd = (RACE_DSPREALTIME_AIRDUMP_CMD_STRU *)pCmdMsg;

        if(pCmd)
        {
            if(pCmd->AirDumpEn == 1)
            {
                RACE_LOG_MSGID_I("[RACE_DSPREALTIME_AIRDUMP_HDR] Start, AirDumpEn:%d\n",1, pCmd->AirDumpEn);
                bt_sink_srv_am_type_t type = bt_sink_srv_ami_get_current_scenario();
                if(type == HFP)
                {
                    airdump_timer_create();
                    xTimerStart(g_airdump_timer, 0);
                    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AIRDUMP_EN, 0, pCmd->AirDumpEn, false);
                } else {
                    RACE_LOG_MSGID_I("[RACE_DSPREALTIME_AIRDUMP_HDR] Please Start eSCO first",0);
                }
            }
            else if(pCmd->AirDumpEn == 0)
            {
                RACE_LOG_MSGID_I("[RACE_DSPREALTIME_AIRDUMP_HDR] Stop, AirDumpEn:%d\n",1, pCmd->AirDumpEn);
                airdump_timer_delete();
                //hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AIRDUMP_EN, 0, pCmd->AirDumpEn, false);
            }
        }

        pEvt->status = (uint8_t)RACE_ERRCODE_SUCCESS;
    }

    return pEvt;
}
#endif


/**
 * RACE_DSPREALTIME_2MIC_SWAP_HDR
 *
 * RACE_DSPREALTIME_2MIC_SWAP_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_DSPREALTIME_2MIC_SWAP_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t MicSwapSel;
    }PACKED RACE_DSPREALTIME_MIC_SWAP_CMD_STRU;

    typedef struct
    {
        uint8_t status;
    }PACKED RACE_DSPREALTIME_MIC_SWAP_EVT_RES_STRU;

    RACE_DSPREALTIME_MIC_SWAP_EVT_RES_STRU *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                   (uint16_t)RACE_DSPREALTIME_2MIC_SWAP,
                                                                   (uint16_t)sizeof(RACE_DSPREALTIME_MIC_SWAP_EVT_RES_STRU),
                                                                   channel_id);
    if (pEvt)
    {
        RACE_DSPREALTIME_MIC_SWAP_CMD_STRU *pCmd = (RACE_DSPREALTIME_MIC_SWAP_CMD_STRU *)pCmdMsg;

        if(pCmd)
        {
            if(pCmd->MicSwapSel == 1)
            {
                RACE_LOG_MSGID_I("[RACE_DSPREALTIME_MIC_SWAP] SWAP to R MIC, MicSwapSel:%d",1, pCmd->MicSwapSel);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 1, MIC_R_ONLY, false);
                }
            else if(pCmd->MicSwapSel == 0)
            {
                RACE_LOG_MSGID_I("[RACE_DSPREALTIME_MIC_SWAP] SWAP to L MIC, MicSwapSel:%d",1, pCmd->MicSwapSel);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 1, MIC_L_ONLY, false);
            }
#if defined(HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE)
            else if(pCmd->MicSwapSel == 2)
            {
                RACE_LOG_MSGID_I("[RACE_DSPREALTIME_MIC_SWAP] SWAP to Multi MIC, MicSwapSel:%d",1, pCmd->MicSwapSel);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 1, MIC_3_ONLY, false);
            }
            else if(pCmd->MicSwapSel == 3)
            {
                RACE_LOG_MSGID_I("[RACE_DSPREALTIME_MIC_SWAP] SWAP to Multi MIC, MicSwapSel:%d",1, pCmd->MicSwapSel);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 1, MIC_4_ONLY, false);
            }
            else if(pCmd->MicSwapSel == 4)
            {
                RACE_LOG_MSGID_I("[RACE_DSPREALTIME_MIC_SWAP] SWAP to Multi MIC, MicSwapSel:%d",1, pCmd->MicSwapSel);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 1, MIC_5_ONLY, false);
            }
            else if(pCmd->MicSwapSel == 5)
            {
                RACE_LOG_MSGID_I("[RACE_DSPREALTIME_MIC_SWAP] SWAP to Multi MIC, MicSwapSel:%d",1, pCmd->MicSwapSel);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 1, MIC_6_ONLY, false);
            }
#endif
            else
            {
                RACE_LOG_MSGID_I("[RACE_DSPREALTIME_MIC_SWAP] SWAP to Multi MIC %d wrong number!!!",1,pCmd->MicSwapSel);
            }
        }

        pEvt->status = (uint8_t)RACE_ERRCODE_SUCCESS;
    }

    return pEvt;
}

/**
 * RACE_DSPREALTIME_OPEN_ALL_MIC_HDR
 *
 * RACE_DSPREALTIME_OPEN_ALL_MIC_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 * @channel_id : race cmd channel id
 *
 */
void* RACE_DSPREALTIME_OPEN_ALL_MIC_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t OpenAllMic;
    }PACKED RACE_DSPREALTIME_OPEN_ALL_MIC_CMD_STRU;

    typedef struct
    {
        uint8_t status;
    }PACKED RACE_DSPREALTIME_OPEN_ALL_MIC_EVT_RES_STRU;

    RACE_DSPREALTIME_OPEN_ALL_MIC_EVT_RES_STRU *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                   (uint16_t)RACE_DSPREALTIME_OPEN_ALL_MIC,
                                                                   (uint16_t)sizeof(RACE_DSPREALTIME_OPEN_ALL_MIC_EVT_RES_STRU),
                                                                   channel_id);
    if (pEvt)
    {
        RACE_DSPREALTIME_OPEN_ALL_MIC_CMD_STRU *pCmd = (RACE_DSPREALTIME_OPEN_ALL_MIC_CMD_STRU *)pCmdMsg;

        if(pCmd)
        {
            if(pCmd->OpenAllMic == 1)
            {
                if(AUD_EXECUTION_SUCCESS != ami_set_audio_device(STREAM_IN, AU_DSP_VOICE, HAL_AUDIO_DEVICE_MAIN_MIC_DUAL, HAL_AUDIO_INTERFACE_1, NOT_REWRITE)){
                    RACE_LOG_MSGID_E("[RACE_DSPREALTIME_OPEN_ALL_MIC] Open All Mic Fail!!!!!!",0);
                } else {
                    RACE_LOG_MSGID_I("[RACE_DSPREALTIME_OPEN_ALL_MIC] Open All Mic, OpenAllMic:%d\n",1, pCmd->OpenAllMic);
            }
            }
            else if(pCmd->OpenAllMic == 0)
            {
                /*if(AUD_EXECUTION_FAIL == audio_nvdm_configure_init()){
                    RACE_LOG_MSGID_E("RACE_DSPREALTIME_OPEN_ALL_MIC]] Default Mic Setting, OpenAllMic Faill!!!!!!",0);
                }*/
            }
        }

        pEvt->status = (uint8_t)RACE_ERRCODE_SUCCESS;
    }

    return pEvt;
}
#if defined(HAL_AUDIO_MODULE_ENABLED)
/**
 * RACE_DSPREALTIME_OPEN_ALL_MIC_HDR_EXTEND
 *
 * RACE_DSPREALTIME_OPEN_ALL_MIC_HDR_EXTEND Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 * @channel_id : race cmd channel id
 *
 */
void* RACE_DSPREALTIME_OPEN_ALL_MIC_HDR_EXTEND(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU Hdr;
        multic_mic_config_param_t mic_param;
    } PACKED RACE_DSPREALTIME_OPEN_ALL_MIC_EXTEND_CMD_STRU;

    typedef struct {
        uint8_t status;
    } PACKED RACE_DSPREALTIME_OPEN_ALL_MIC_EXTEND_EVT_RES_STRU;

    RACE_DSPREALTIME_OPEN_ALL_MIC_EXTEND_EVT_RES_STRU *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                   (uint16_t)RACE_DSPREALTIME_OPEN_ALL_MIC_EXTEND,
                                                                   (uint16_t)sizeof(RACE_DSPREALTIME_OPEN_ALL_MIC_EXTEND_EVT_RES_STRU),
                                                                   channel_id);
    if (pEvt)
    {
        RACE_DSPREALTIME_OPEN_ALL_MIC_EXTEND_CMD_STRU *pCmd = (RACE_DSPREALTIME_OPEN_ALL_MIC_EXTEND_CMD_STRU *)pCmdMsg;

        if(pCmd)
        {
            if (pCmd->Hdr.length != 8) {
                pEvt->status = (uint8_t)RACE_ERRCODE_FAIL;
                return pEvt;
            }
            if (ami_set_audio_device_extend(STREAM_IN, AU_DSP_VOICE, &pCmd->mic_param, false) != AUD_EXECUTION_SUCCESS) {
                RACE_LOG_MSGID_E("[RACE_DSPREALTIME_OPEN_ALL_MIC_HDR_EXTEND] Open Mic Extend Fail!",0);
            } else {
                RACE_LOG_MSGID_I("[RACE_DSPREALTIME_OPEN_ALL_MIC_HDR_EXTEND] Open Mic Extend Success.",0);
            }
        }
        pEvt->status = (uint8_t)RACE_ERRCODE_SUCCESS;
    }

    return pEvt;
}
#endif

/**
 * RACE_DSPREALTIME_AECNR_ON_OFF_HDR
 *
 * RACE_DSPREALTIME_AECNR_ON_OFF_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_DSPREALTIME_AECNR_ON_OFF_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t AecNrEn;
    }PACKED RACE_DSPREALTIME_AECNR_ON_OFF_CMD_STRU;

    typedef struct
    {
        uint8_t status;
    }PACKED RACE_DSPREALTIME_AECNR_ON_OFF_EVT_RES_STRU;

    RACE_DSPREALTIME_AECNR_ON_OFF_EVT_RES_STRU *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                   (uint16_t)RACE_DSPREALTIME_AECNR_ON_OFF,
                                                                   (uint16_t)sizeof(RACE_DSPREALTIME_AECNR_ON_OFF_EVT_RES_STRU),
                                                                   channel_id);
    if (pEvt)
    {
        RACE_DSPREALTIME_AECNR_ON_OFF_CMD_STRU *pCmd = (RACE_DSPREALTIME_AECNR_ON_OFF_CMD_STRU *)pCmdMsg;

        if(pCmd)
        {
            if(pCmd->AecNrEn == 1)
            {
                RACE_LOG_MSGID_I("[RACE_DSPREALTIME_AECNR_ON_OFF_HDR] AECNR on, AecNrEn:%d\n",1, pCmd->AecNrEn);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AEC_NR_EN, 0, AEC_NR_ENABLE, false);
            }
            else if(pCmd->AecNrEn == 0)
            {
                RACE_LOG_MSGID_I("[RACE_DSPREALTIME_AECNR_ON_OFF_HDR] AECNR off, AecNrEn:%d\n",1, pCmd->AecNrEn);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AEC_NR_EN, 0, AEC_NR_DISABLE, false);
            }
        }

        pEvt->status = (uint8_t)RACE_ERRCODE_SUCCESS;
    }

    return pEvt;
}

/**
 * RACE_DSPREALTIME_QUERY_LIB_VERSION_HDR
 *
 * RACE_DSPREALTIME_QUERY_LIB_VERSION_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_DSPREALTIME_QUERY_LIB_VERSION_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        uint8_t status;
    }PACKED RSP;
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t lib_type;
    }PACKED RACE_DSPREALTIME_QUERY_LIB_VERSION_CMD_STRU;

    int32_t ret = RACE_ERRCODE_FAIL;
    RSP *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                 RACE_DSPREALTIME_QUERY_LIB_VERSION,
                                 sizeof(RSP),
                                 channel_id);
    if (pEvt)
    {
        race_dsprealtime_query_lib_version_noti_struct_t *ver_buf;
        race_dsprealtime_query_lib_version_noti_struct_t *noti = NULL;
        RACE_DSPREALTIME_QUERY_LIB_VERSION_CMD_STRU *pCmd;

        /* A1. Execute the cmd. */
        ver_buf = (race_dsprealtime_query_lib_version_noti_struct_t *)hal_audio_query_race_cmd_audio_buf();

        if (ver_buf)
        {
            pCmd = (RACE_DSPREALTIME_QUERY_LIB_VERSION_CMD_STRU *)pCmdMsg;
            if (pCmd != NULL) {
                ver_buf->type = pCmd->lib_type;
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_REQ_GET_REALTIME_LIB_VERSION,
                                                      0,
                                                      (uint32_t)ver_buf,
                                                      true);
                /* A2. Create the noti. */
                noti = (void *)RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                                                RACE_DSPREALTIME_QUERY_LIB_VERSION,
                                                sizeof(race_dsprealtime_query_lib_version_noti_struct_t),
                                                channel_id);
                if (noti)
                {
                    /* A3. Set the noti parameters with the cmd results.  */
                    memcpy((uint8_t *)noti, (uint8_t *)ver_buf, 8 + ver_buf->length);
                
                    /* A4. Send the noti. */
                    ret = race_noti_send(noti, channel_id, TRUE);
                    if (RACE_ERRCODE_SUCCESS != ret)
                    {
                        /* A5. Free the noti if needed. */
                        RACE_FreePacket(noti);
                        noti = NULL;
                    }
                }
                else
                {
                    ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
                }
            }
        }

        pEvt->status = ret;
    }

    return pEvt;
}

/**
 * RACE_DSPREALTIME_SWGAIN_BYPASS_HDR
 *
 * RACE_DSPREALTIME_SWGAIN_BYPASS_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_DSPREALTIME_SWGAIN_EN_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t SW_GAIN_EN;
    }PACKED RACE_DSPREALTIME_SWGAIN_EN_CMD_STRU;

    typedef struct
    {
        uint8_t status;
    }PACKED RACE_DSPREALTIME_SWGAIN_EN_EVT_RES_STRU;

    RACE_DSPREALTIME_SWGAIN_EN_EVT_RES_STRU *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                   (uint16_t)RACE_DSPREALTIME_SWGAIN_BYPASS,
                                                                   (uint16_t)sizeof(RACE_DSPREALTIME_SWGAIN_EN_EVT_RES_STRU),
                                                                   channel_id);
    if (pEvt)
    {
        RACE_DSPREALTIME_SWGAIN_EN_CMD_STRU *pCmd = (RACE_DSPREALTIME_SWGAIN_EN_CMD_STRU *)pCmdMsg;

        if(pCmd)
        {
            if(pCmd->SW_GAIN_EN == 0)
            {
                RACE_LOG_MSGID_I("[RACE_DSPREALTIME_SWGAIN_BYPASS] Bypass SW Gain, ByPassStatus:%d\n",1, pCmd->SW_GAIN_EN);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SW_GAIN_EN, 0, SW_GAIN_DISABLE, false);
            } else if(pCmd->SW_GAIN_EN == 1)
            {
                RACE_LOG_MSGID_I("[RACE_DSPREALTIME_SWGAIN_BYPASS] Don't Bypass SW Gain, ByPassStatus:%d\n",1, pCmd->SW_GAIN_EN);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SW_GAIN_EN, 0, SW_GAIN_ENABLE, false);
            }
        }

        pEvt->status = (uint8_t)RACE_ERRCODE_SUCCESS;
    }

    return pEvt;
}

#ifdef MTK_ANC_ENABLE
#ifndef MTK_ANC_V2
/**
 * RACE_DSPREALTIME_ANC_HDR
 *
 * RACE_DSPREALTIME_ANC_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_DSPREALTIME_ANC_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    race_dsprealtime_anc_struct *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                                         RACE_DSPREALTIME_ANC,
                                                         sizeof(race_dsprealtime_anc_struct),
                                                         channel_id);
    g_anc_race_ch_id = channel_id;
    if (pEvt)
    {
        race_dsprealtime_anc_struct *pAnc_cmd;
        anc_control_result_t anc_ret = ANC_CONTROL_EXECUTION_NONE;
        uint16_t anc_param_length;

        pAnc_cmd = (race_dsprealtime_anc_struct *)pCmdMsg->payload;
        memset(pEvt, 0, sizeof(race_dsprealtime_anc_struct));
        pEvt->anc_id = pAnc_cmd->anc_id;

        RACE_LOG_MSGID_I("RACE_DSPREALTIME_ANC_HDR msg_length:%d payload: %d %d %d\n",4,pCmdMsg->hdr.length,pAnc_cmd->status,pAnc_cmd->anc_id,pAnc_cmd->param.anc_on_param.anc_filter_type);

        if (pCmdMsg->hdr.length >= 4) { //hdr.length = hdr.id(2bytes) + pAnc_cmd->status(1byte) + pAnc_cmd->anc_id(1byte) + pAnc_cmd->param(0/1/2/4 bytes)
            anc_param_length = pCmdMsg->hdr.length - 4;
            switch (pAnc_cmd->anc_id) {
                case RACE_ANC_ON:
                    if (anc_param_length > 1) {
                        anc_filter_type_t filter_type = (anc_filter_type_t)pAnc_cmd->param.anc_on_param.anc_filter_type;
                        pEvt->param.anc_on_param = pAnc_cmd->param.anc_on_param;
                        if (pAnc_cmd->param.anc_on_param.anc_mode == 1) {
                            filter_type |= ANC_FF_ONLY_BIT_MASK;
                        } else if (pAnc_cmd->param.anc_on_param.anc_mode == 2) {
                            filter_type |= ANC_FB_ONLY_BIT_MASK;
                        }
                        race_anc_mutex_take();
                        g_anc_notify_on = ((uint32_t)filter_type << 8);
                        g_anc_notify_off = 0;
                        race_anc_mutex_give();
                        RACE_LOG_MSGID_I("MP_Step__1__ set anc on: 0x%x",1,filter_type);
                        anc_ret = audio_anc_enable(filter_type, ANC_UNASSIGNED_GAIN, NULL);
                    } else if (anc_param_length > 0) {
                        anc_filter_type_t filter_type = (anc_filter_type_t)pAnc_cmd->param.anc_on_param.anc_filter_type;
                        pEvt->param.anc_on_param.anc_filter_type = pAnc_cmd->param.anc_on_param.anc_filter_type;
                        race_anc_mutex_take();
                        g_anc_notify_on = ((uint32_t)filter_type << 8);
                        g_anc_notify_off = 0;
                        race_anc_mutex_give();
                        anc_ret = audio_anc_enable(filter_type, ANC_UNASSIGNED_GAIN, NULL);
                    }
                    break;
                case RACE_ANC_OFF:
                    race_anc_mutex_take();
                    g_anc_notify_off |= (1 << 31);
                    g_anc_notify_on = 0;
                    race_anc_mutex_give();
                    RACE_LOG_MSGID_I("MP_Step__1__ set anc off",0);
                    anc_ret = audio_anc_disable(NULL);
                    if (anc_get_sync_time() == 0) {
                        RACE_FreePacket(pEvt);
                        return NULL;
                    }
                    break;
                case RACE_ANC_SET_VOL:
                    if (anc_param_length >= 4) {
                        pEvt->param.gain.val = pAnc_cmd->param.gain.val;
                        anc_ret = audio_anc_set_volume(pAnc_cmd->param.gain, TO_BOTH);
                    }
                    break;
                case RACE_ANC_SET_RUNTIME_VOL:
                    if (anc_param_length >= 2) {
                        pEvt->param.runtime_gain = pAnc_cmd->param.runtime_gain;
                        anc_ret = audio_anc_set_runtime_volume(pAnc_cmd->param.runtime_gain);
                    }
                    break;
                case RACE_ANC_READ_NVDM:
                    anc_ret = audio_anc_read_volume_nvdm(&pEvt->param.gain, TO_AGENT);
                    break;
                case RACE_ANC_WRITE_NVDM:
                    if (anc_param_length >= 4) {
                        pEvt->param.gain.val = pAnc_cmd->param.gain.val;
                        anc_ret = audio_anc_write_volume_nvdm(&pAnc_cmd->param.gain, TO_AGENT);
                    }
                    break;
#ifdef MTK_AWS_MCE_ENABLE
                case RACE_ANC_SET_AGENT_VOL:
                    if (anc_param_length >= 4) {
                        pEvt->param.gain.val = pAnc_cmd->param.gain.val;
                        race_anc_mutex_take();
                        g_anc_set_agent_vol = pAnc_cmd->param.gain.val;
                        race_anc_mutex_give();
                        RACE_LOG_MSGID_I("MP_Step__1__ set A gain: 0x%x",1,pAnc_cmd->param.gain.val);
                        anc_ret = audio_anc_set_volume(pAnc_cmd->param.gain, TO_AGENT);
                        if (anc_get_sync_time() == 0) {
                            RACE_FreePacket(pEvt);
                            return NULL;
                        }
                    }
                    break;
                case RACE_ANC_SET_PARTNER_VOL:
                    if (anc_param_length >= 4) {
                        pEvt->param.gain.val = pAnc_cmd->param.gain.val;
                        race_anc_mutex_take();
                        g_anc_set_partner_vol = pAnc_cmd->param.gain.val;
                        race_anc_mutex_give();
                        RACE_LOG_MSGID_I("MP_Step__4__ set P gain: 0x%x",1,pAnc_cmd->param.gain.val);
                        anc_ret = audio_anc_set_volume(pAnc_cmd->param.gain, TO_PARTNER);
                        if (anc_get_sync_time() == 0) {
                            RACE_FreePacket(pEvt);
                            return NULL;
                        }
                    }
                    break;
                case RACE_ANC_READ_PARTNER_NVDM:
                    if (pAnc_cmd->status == 0) {
                        uint32_t pktId = (uint32_t)pCmdMsg->hdr.pktId.value;
                        if ((bt_sink_srv_cm_get_special_aws_device() != NULL) && (bt_sink_srv_cm_get_aws_link_state() == BT_AWS_MCE_AGENT_STATE_ATTACHED)) {
                            anc_ret = anc_send_aws_mce_param(ANC_DIRECT, ANC_CONTROL_EVENT_READ_VOLUME_NVDM, 0, ((pktId << 8) | channel_id));
                            RACE_FreePacket(pEvt);
                            return NULL;
                        } else {
                            RACE_LOG_MSGID_E("ANC read partner nvdm error : bt power off or aws link state isn't attached", 0);
                            pEvt->status = RACE_ERRCODE_FAIL;
                            return pEvt;
                        }
                    } else {
                        uint8_t true_channel_id = pAnc_cmd->status & (~0x80);
                        RACE_LOG_MSGID_I("anc get partner's vol nvdm: %d %d\n", 2, pAnc_cmd->param.gain.gain_index_l, pAnc_cmd->param.gain.gain_index_r);
                        pEvt->status = 0;
                        pEvt->param.gain.val = pAnc_cmd->param.gain.val;
                        race_flush_packet((void *)pEvt, true_channel_id);
                        return NULL;
                    }
                case RACE_ANC_WRITE_PARTNER_NVDM:
                    if (anc_param_length >= 4) {
                        pEvt->param.gain.val = pAnc_cmd->param.gain.val;
                        anc_ret = audio_anc_write_volume_nvdm(&pAnc_cmd->param.gain, TO_PARTNER);
                    }
                    break;
                case RACE_ANC_SET_SYNC_TIME:
                    if (anc_param_length >= 2) {
                        pEvt->param.u2param = pAnc_cmd->param.u2param;
                        anc_set_sync_time(pAnc_cmd->param.u2param);
                        anc_ret = ANC_CONTROL_EXECUTION_SUCCESS;
                        if ((bt_sink_srv_cm_get_special_aws_device() != NULL) && (bt_sink_srv_cm_get_aws_link_state() == BT_AWS_MCE_AGENT_STATE_ATTACHED)) {
                            anc_ret = anc_send_aws_mce_param(ANC_DIRECT, ANC_CONTROL_EVENT_SET_SYNC_TIME, (uint32_t)pAnc_cmd->param.u2param, 0);
                        } else {
                            RACE_LOG_MSGID_E("ANC set sync time error : bt power off or aws link state isn't attached", 0);
                            pEvt->status = RACE_ERRCODE_FAIL;
                            return pEvt;
                        }
                    }
                    break;
#endif
                case RACE_ANC_GET_HYBRID_CAP:
                    anc_ret = anc_get_hybrid_capability(&pEvt->param.hybrid_cap);
                    break;
                default:
                    pEvt->status = RACE_ERRCODE_NOT_SUPPORT;
                    return pEvt;
            }
            pEvt->status = (anc_ret == ANC_CONTROL_EXECUTION_SUCCESS) ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
        }
        else
        {
            pEvt->status = RACE_ERRCODE_PARAMETER_ERROR;
        }
        if(pEvt->status != RACE_ERRCODE_SUCCESS) {
            RACE_LOG_MSGID_E("RACE_DSPREALTIME_ANC_HDR pEvt->status: %d  anc_ret: %d\n",2,pEvt->status,anc_ret);
        }
    }

    return pEvt;
}

/**
 * RACE_DSPREALTIME_PASSTHRU_HDR
 *
 * RACE_DSPREALTIME_PASSTHRU_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_DSPREALTIME_PASSTHRU_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t enable;
    }PACKED RACE_DSPREALTIME_PASSTHRU_CMD_STRU;

    typedef struct
    {
        uint8_t status;
    }PACKED RACE_DSPREALTIME_PASSTHRU_EVT_STRU;

    RACE_DSPREALTIME_PASSTHRU_CMD_STRU *pCmd = (RACE_DSPREALTIME_PASSTHRU_CMD_STRU *)pCmdMsg;
    RACE_DSPREALTIME_PASSTHRU_EVT_STRU *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                                         pCmd->Hdr.id,
                                                         sizeof(RACE_DSPREALTIME_PASSTHRU_EVT_STRU),
                                                         channel_id);
    if (pEvt)
    {
        anc_control_result_t anc_ret = ANC_CONTROL_EXECUTION_NONE;
        if (pCmd->Hdr.id == RACE_DSPREALTIME_PASS_THROUGH_ON_OFF) {
            if (pCmd->enable == 0) {
                uint8_t enable;
                uint32_t runtime_info;
                anc_get_status(&enable, &runtime_info, NULL);
                if ((enable == 1) && ((runtime_info & ANC_FILTER_TYPE_MASK) != FILTER_5)) {
                    anc_ret = ANC_CONTROL_EXECUTION_SUCCESS;
                } else {
                    anc_ret = audio_anc_disable(NULL);
                }
            } else if (pCmd->enable == 1) {
                anc_ret = audio_anc_enable(FILTER_5, ANC_UNASSIGNED_GAIN, NULL);
            }
        }
        pEvt->status = anc_ret;
    }

    return pEvt;
}

void* RACE_DSPREALTIME_PASSTHRU_TEST_MUTE_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t LR_mic;
    }PACKED RACE_DSPREALTIME_PASSTHRU_TEST_CMD_STRU;

    typedef struct
    {
        uint8_t status;
    }PACKED RACE_DSPREALTIME_PASSTHRU_TEST_EVT_RES_STRU;

    typedef struct
    {
        uint32_t unknown;
        int db_value;
        uint32_t freq;
    }PACKED RACE_DSPREALTIME_PASSTHRU_TEST_NOTIFICATION_STRU;


    int32_t ret = RACE_ERRCODE_FAIL;
    RACE_DSPREALTIME_PASSTHRU_TEST_EVT_RES_STRU *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                 RACE_DSPREALTIME_PASS_THROUGH_TEST,
                                 sizeof(RACE_DSPREALTIME_PASSTHRU_TEST_EVT_RES_STRU),
                                 channel_id);
    RACE_DSPREALTIME_PASSTHRU_TEST_CMD_STRU *pCmd = (RACE_DSPREALTIME_PASSTHRU_TEST_CMD_STRU *)pCmdMsg;

    if (pEvt)
    {
        PTT_u4Freq_Mag_data result = {0, 0};

        if (pCmd)
        {
            RACE_DSPREALTIME_PASSTHRU_TEST_NOTIFICATION_STRU *noti = NULL;

            /* A1. Execute the cmd. */
            pass_through_test(PTT_AMIC_DCC, pCmd->LR_mic, PTT_MUTE, &result);
            RACE_LOG_MSGID_I("PASS_THROUGH_TEST, Freq=%d, Mag=%d, db=%f\r\n", 3, result.freq_data, result.mag_data, result.db_data);

            /* A2. Create the noti. */
            noti = (void *)RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                                            RACE_DSPREALTIME_PASS_THROUGH_TEST,
                                            sizeof(RACE_DSPREALTIME_PASSTHRU_TEST_NOTIFICATION_STRU),
                                            channel_id);
            if (noti)
            {
                /* A3. Set the noti parameters with the cmd results.  */
                noti->db_value = (int)round(result.db_data);
                noti->freq = result.freq_data;

                /* A4. Send the noti. */
                ret = race_noti_send(noti, channel_id, TRUE);
                if (RACE_ERRCODE_SUCCESS != ret)
                {
                    /* A5. Free the noti if needed. */
                    RACE_FreePacket(noti);
                    noti = NULL;
                    pEvt->status = ret;
                    return pEvt;
                }
            }
            else
            {
                ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
                pEvt->status = ret;
                return pEvt;
            }


        }

        pEvt->status = ret;
    }

    return pEvt;

}


#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
void race_dsprealtime_anc_adaptive_trigger_ff()
{
    RACE_LOG_MSGID_I("[user_trigger_ff]race cmd user trigger adaptive FF start", 0);
    audio_anc_user_trigger_ff_start(anc_user_trigger_adaptive_ff_callback);
}

void race_dsprealtime_anc_adaptive_notify_end()
{
    race_send_event_notify_msg(RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_END, NULL);
}

void race_dsprealtime_anc_adaptive_abort()
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    typedef struct
    {
        uint8_t status;
        uint8_t mode;
    }PACKED RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_EVT_STRU;

    RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_EVT_STRU *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                                         RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK,
                                                         sizeof(RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_EVT_STRU),
                                                         g_anc_race_ch_id);

    if (pEvt) {
        pEvt->status = USER_TRIGGER_FF_STATUS_ABORT;
        pEvt->mode = ADAPTIVE_FF_ANC_MODE;
        if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
            race_flush_packet((void *)pEvt, g_anc_race_ch_id);
        } else {/*partner send agent result*/
            race_send_pkt_t* pSndPkt;
            pSndPkt = (void *)race_pointer_cnv_pkt_to_send_pkt((void *)pEvt);
            race_pkt_t      *pret;
            race_send_pkt_t *psend;
            psend = (race_send_pkt_t *)pSndPkt;
            pret = &psend->race_data;
            #if (RACE_DEBUG_PRINT_ENABLE)
            race_dump((uint8_t *)pret, RACE_DBG_EVT);
            #endif

            if(pSndPkt) {
                bt_status_t ret = bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, g_anc_race_ch_id, RACE_CMD_RSP_FROM_PARTNER,0);
                if (ret != BT_STATUS_SUCCESS) {
                    RACE_LOG_MSGID_I("[relay_cmd][user_trigger_ff]partner send relay req FAIL \n", 0);
                } else {
                    RACE_LOG_MSGID_I("[relay_cmd][user_trigger_ff]race_dsprealtime_anc_adaptive_abort, send status:%d success", pEvt->status);
                }
                race_mem_free(pSndPkt);
            }
        }
    } else {
        RACE_LOG_MSGID_I("[user_trigger_ff]race_dsprealtime_anc_adaptive_abort, pkt claim fail", 0);
    }

}
#endif
#endif


#else
/**
 * RACE_DSPREALTIME_ANC_HDR
 *
 * RACE_DSPREALTIME_ANC_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
extern uint16_t g_anc_race_cmd_length;
void* RACE_DSPREALTIME_ANC_PASSTHRU_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    race_dsprealtime_anc_struct *pEvt = NULL;
    g_anc_race_ch_id = channel_id;

    race_dsprealtime_anc_struct *pAnc_cmd;
    audio_anc_control_result_t anc_ret = AUDIO_ANC_CONTROL_EXECUTION_NONE;
    pAnc_cmd = (race_dsprealtime_anc_struct *)pCmdMsg->payload;
    g_anc_race_cmd_length = pCmdMsg->hdr.length - 4;
    //RACE_LOG_MSGID_I("RACE_DSPREALTIME_ANC_HDR msg_length:%d payload: %d %d %d\n",4,pCmdMsg->hdr.length,pAnc_cmd->status,pAnc_cmd->param.header.ancId,pAnc_cmd->param.anc_on_param.anc_filter_type);
    RACE_LOG_MSGID_E("[RACE][ANC_PTHR]channel_id: %d, ANC_id: %d\n", 2, channel_id, pAnc_cmd->param.header.ancId);

    if (pCmdMsg->hdr.length >= 4) { //hdr.length = hdr.id(2bytes) + pAnc_cmd->status(1byte) + pAnc_cmd->anc_id(1byte) + pAnc_cmd->param(0/1/2/4 bytes)
        switch (pAnc_cmd->param.header.ancId) {
            case RACE_ANC_ON:{
                anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_RACE, 0, (void *)pAnc_cmd);
                /* Notify APP Layer ANC on */
                race_dsp_realtime_send_notify_msg(RACE_DSPREALTIME_ANC, RACE_ANC_ON, anc_ret);

                pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                        RACE_DSPREALTIME_ANC,
                                        sizeof(RACE_RSP_ANC_PASSTHRU_ON_PARAM),
                                        channel_id);
                if(pEvt != NULL){
                    memset(pEvt, 0, sizeof(RACE_RSP_ANC_PASSTHRU_ON_PARAM));
                }else{
                    goto _HDR_END;
                }
                pEvt->param.onRsp.header.ancId  = pAnc_cmd->param.onCmd.header.ancId;
                pEvt->param.onRsp.header.status = anc_ret;
                pEvt->param.onRsp.flash_no      = pAnc_cmd->param.onCmd.flash_no;
                if(pAnc_cmd->param.onCmd.flash_no < AUDIO_ANC_CONTROL_PASS_THRU_FILTER_DEFAULT){
                    pEvt->param.onRsp.ancType   = pAnc_cmd->param.onCmd.ancType;
                } else {
                    pEvt->param.onRsp.ancType   = AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF;
                }
                pEvt->param.onRsp.syncMode      = pAnc_cmd->param.onCmd.syncMode;
                break;
            }
            case RACE_ANC_OFF:{
                anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_RACE, 0, (void *)pAnc_cmd);
                /* Notify ANC off */
                race_dsp_realtime_send_notify_msg(RACE_DSPREALTIME_ANC, RACE_ANC_OFF, anc_ret);

                pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                        RACE_DSPREALTIME_ANC,
                                        sizeof(RACE_RSP_ANC_PASSTHRU_OFF_PARAM),
                                        channel_id);
                if(pEvt != NULL){
                    memset(pEvt, 0, sizeof(RACE_RSP_ANC_PASSTHRU_OFF_PARAM));
                }else{
                    goto _HDR_END;
                }
                pEvt->param.offRsp.header.ancId  = pAnc_cmd->param.offCmd.header.ancId;
                pEvt->param.offRsp.header.status = anc_ret;
                pEvt->param.offRsp.syncMode      = pAnc_cmd->param.offCmd.syncMode;
                if (audio_anc_control_get_sync_time() == 0) {
                    RACE_FreePacket(pEvt);
                    return NULL;
                }
                break;
            }
            case RACE_ANC_SET_VOL:{
                anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_RACE, 0, (void *)pAnc_cmd);
                pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                        RACE_DSPREALTIME_ANC,
                                        sizeof(RACE_RSP_ANC_PASSTHRU_SET_VOL_PARAM),
                                        channel_id);
                if(pEvt != NULL){
                    memset(pEvt, 0, sizeof(RACE_RSP_ANC_PASSTHRU_SET_VOL_PARAM));
                }else{
                    goto _HDR_END;
                }
                pEvt->param.gainRsp.header.ancId  = pAnc_cmd->param.gainCmd.header.ancId;
                pEvt->param.gainRsp.header.status = anc_ret;
                pEvt->param.gainRsp.gainFFl       = pAnc_cmd->param.gainCmd.gainFFl;
                pEvt->param.gainRsp.gainFBl       = pAnc_cmd->param.gainCmd.gainFBl;
                pEvt->param.gainRsp.gainFFr       = pAnc_cmd->param.gainCmd.gainFFr;
                pEvt->param.gainRsp.gainFBr       = pAnc_cmd->param.gainCmd.gainFBr;
                break;
            }
            case RACE_ANC_READ_NVDM:{
                race_dsprealtime_anc_struct pAnc_rsp;
                pAnc_rsp.param.readNVDMRsp.header.ancId = pAnc_cmd->param.readNVDMCmd.header.ancId;
                anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_RACE, 0, (void *)&pAnc_rsp);
                pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                        RACE_DSPREALTIME_ANC,
                                        sizeof(RACE_RSP_ANC_PASSTHRU_READ_NVDM),
                                        channel_id);
                if(pEvt != NULL){
                    memset(pEvt, 0, sizeof(RACE_RSP_ANC_PASSTHRU_READ_NVDM));
                }else{
                    goto _HDR_END;
                }
                pEvt->param.readNVDMRsp.header.ancId  = pAnc_rsp.param.readNVDMRsp.header.ancId;
                pEvt->param.readNVDMRsp.header.status = anc_ret;
                pEvt->param.readNVDMRsp.gainFFl       = pAnc_rsp.param.readNVDMRsp.gainFFl;
                pEvt->param.readNVDMRsp.gainFBl       = pAnc_rsp.param.readNVDMRsp.gainFBl;
                pEvt->param.readNVDMRsp.gainFFr       = pAnc_rsp.param.readNVDMRsp.gainFFr;
                pEvt->param.readNVDMRsp.gainFBr       = pAnc_rsp.param.readNVDMRsp.gainFBr;
                break;
            }
            case RACE_ANC_WRITE_NVDM:{
                anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_RACE, 0, (void *)pAnc_cmd);
                pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                        RACE_DSPREALTIME_ANC,
                                        sizeof(RACE_RSP_ANC_PASSTHRU_WRITE_NVDM),
                                        channel_id);
                if(pEvt != NULL){
                    memset(pEvt, 0, sizeof(RACE_RSP_ANC_PASSTHRU_WRITE_NVDM));
                }else{
                    goto _HDR_END;
                }
                pEvt->param.writeNVDMRsp.header.ancId  = pAnc_cmd->param.writeNVDMCmd.header.ancId;
                pEvt->param.writeNVDMRsp.header.status = anc_ret;
                pEvt->param.writeNVDMRsp.gainFFl       = pAnc_cmd->param.writeNVDMCmd.gainFFl;
                pEvt->param.writeNVDMRsp.gainFBl       = pAnc_cmd->param.writeNVDMCmd.gainFBl;
                pEvt->param.writeNVDMRsp.gainFFr       = pAnc_cmd->param.writeNVDMCmd.gainFFr;
                pEvt->param.writeNVDMRsp.gainFBr       = pAnc_cmd->param.writeNVDMCmd.gainFBr;
                break;
            }
            case RACE_ANC_SET_RUNTIME_VOL:{
                anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_RACE, 0, (void *)pAnc_cmd);
                pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                        RACE_DSPREALTIME_ANC,
                                        sizeof(RACE_RSP_ANC_PASSTHRU_SET_RUNTIME_VOL),
                                        channel_id);
                if(pEvt != NULL){
                    memset(pEvt, 0, sizeof(RACE_RSP_ANC_PASSTHRU_SET_RUNTIME_VOL));
                }else{
                    goto _HDR_END;
                }
                pEvt->param.runtimeGainRsp.header.ancId  = pAnc_cmd->param.runtimeGainCmd.header.ancId;
                pEvt->param.runtimeGainRsp.header.status = anc_ret;
                pEvt->param.runtimeGainRsp.gain          = pAnc_cmd->param.runtimeGainCmd.gain;
                pEvt->param.runtimeGainRsp.syncMode      = pAnc_cmd->param.runtimeGainCmd.syncMode;
                break;
            }
#if 0//def MTK_AWS_MCE_ENABLE
            case RACE_ANC_SET_AGENT_VOL:
            case RACE_ANC_SET_PARTNER_VOL:
            case RACE_ANC_READ_PARTNER_NVDM:
            case RACE_ANC_WRITE_PARTNER_NVDM:
                pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                        RACE_DSPREALTIME_ANC,
                                        sizeof(RACE_ANC_PASSTHRU_HEADER),
                                        channel_id);
                if(pEvt != NULL){
                    memset(pEvt, 0, sizeof(RACE_ANC_PASSTHRU_HEADER));
                }else{
                    goto _HDR_END;
                }
                pEvt->param.header.status = RACE_ERRCODE_NOT_SUPPORT;
                return pEvt;
                break;
#endif
            case RACE_ANC_SET_SYNC_TIME:{
                if(pAnc_cmd->param.setSyncTimeCmd.sync_time != audio_anc_control_get_sync_time()){
                    audio_anc_control_set_sync_time(pAnc_cmd->param.setSyncTimeCmd.sync_time);
                }
                audio_anc_control_set_attach_enable( (pAnc_cmd->param.setSyncTimeCmd.sync_time == 0)? false : true );
                anc_ret = AUDIO_ANC_CONTROL_EXECUTION_SUCCESS;
                pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                        RACE_DSPREALTIME_ANC,
                                        sizeof(RACE_RSP_ANC_PASSTHRU_SET_SYNC_TIME),
                                        channel_id);
                if(pEvt != NULL){
                    memset(pEvt, 0, sizeof(RACE_RSP_ANC_PASSTHRU_SET_SYNC_TIME));
                }else{
                    goto _HDR_END;
                }
                pEvt->param.setSyncTimeRsp.header.ancId  = pAnc_cmd->param.setSyncTimeCmd.header.ancId;
                pEvt->param.setSyncTimeRsp.header.status = anc_ret;
                pEvt->param.setSyncTimeRsp.sync_time     = pAnc_cmd->param.setSyncTimeCmd.sync_time;
                break;
            }
            case RACE_ANC_GET_HYBRID_CAP_V2:{
                uint8_t support_hybrid;
                audio_anc_control_get_status(NULL, NULL, NULL, NULL, &support_hybrid, NULL);
                anc_ret = AUDIO_ANC_CONTROL_EXECUTION_SUCCESS;
                pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                        RACE_DSPREALTIME_ANC,
                                        sizeof(RACE_RSP_ANC_PASSTHRU_GET_HYBRID_CAP),
                                        channel_id);
                if(pEvt != NULL){
                    memset(pEvt, 0, sizeof(RACE_RSP_ANC_PASSTHRU_GET_HYBRID_CAP));
                }else{
                    goto _HDR_END;
                }
                pEvt->param.getHybridRsp.header.ancId  = pAnc_cmd->param.header.ancId;
                pEvt->param.getHybridRsp.header.status = anc_ret;
                pEvt->param.getHybridRsp.hybridEnable  = support_hybrid;
                break;
            }
            case RACE_ANC_REATIME_UPDATE_COEF:{
                anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_RACE, 0, (void *)pAnc_cmd);
                pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                        RACE_DSPREALTIME_ANC,
                                        sizeof(RACE_RSP_ANC_PASSTHRU_SET_COEF),
                                        channel_id);
                if(pEvt != NULL){
                    memset(pEvt, 0, sizeof(RACE_RSP_ANC_PASSTHRU_SET_COEF));
                }else{
                    goto _HDR_END;
                }
                pEvt->param.setCoefRsp.header.ancId  = pAnc_cmd->param.setCoefCmd.header.ancId;
                pEvt->param.setCoefRsp.header.status = anc_ret;
                break;
            }
            case RACE_ANC_ENTER_MP_MODE:{
                anc_ret = AUDIO_ANC_CONTROL_EXECUTION_SUCCESS;
                /* Enter MP mode, set anc sync time to 0. */
                audio_anc_control_set_sync_time(0);

                /* Enter MP mode, disable Adaptive ANC. */
                /* Important, DO NOT SET any NVDM. MP mode was temp control flow for MP test.*/
                //;
                //if anc is enabled --> turn off adaptive ANC stream
                #ifdef ADAPIVE_ANC_STREAM_CONTROL
                RACE_LOG_MSGID_I("[RACE][Adaptive ANC Stream] Enter MP Mode, suspend Adaptive ANC Stream.", 0);
                audio_anc_monitor_adaptive_anc_stream_suspend_resume(FALSE); //suspend Adaptive ANC stream
                g_adaptive_ANC_stream_MP_control_flag = true; //enter MP mode
                #endif
                /* Enter MP mode, reset extend gain. */
                audio_anc_control_extend_ramp_cap_t init_ramp_cap;
                init_ramp_cap.extend_gain_1 = 0;
                init_ramp_cap.extend_gain_2 = 0;
                init_ramp_cap.gain_type = AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE;
                anc_ret = audio_anc_control_set_extend_gain(init_ramp_cap.gain_type, &init_ramp_cap, NULL);
                init_ramp_cap.gain_type = AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE;
                anc_ret = audio_anc_control_set_extend_gain(init_ramp_cap.gain_type, &init_ramp_cap, NULL);
                init_ramp_cap.gain_type = AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION;
                anc_ret = audio_anc_control_set_extend_gain(init_ramp_cap.gain_type, &init_ramp_cap, NULL);

                pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                        RACE_DSPREALTIME_ANC,
                                        sizeof(RACE_CMD_ANC_PASSTHRU_ENTER_LEAVE_MP_MODE),
                                        channel_id);
                if(pEvt != NULL){
                    memset(pEvt, 0, sizeof(RACE_CMD_ANC_PASSTHRU_ENTER_LEAVE_MP_MODE));
                }else{
                    goto _HDR_END;
                }
                pEvt->param.MPmodeRsp.header.ancId  = pAnc_cmd->param.MPmodeCmd.header.ancId;
                pEvt->param.MPmodeRsp.header.status = anc_ret;
                break;
            }
            case RACE_ANC_LEAVE_MP_MODE:{
                anc_ret = AUDIO_ANC_CONTROL_EXECUTION_SUCCESS;
                /* Leave MP mode, enable Adaptive ANC. */
                /* Important, DO NOT SET any NVDM. MP mode was temp control flow for MP test.*/
                //;
                #ifdef ADAPIVE_ANC_STREAM_CONTROL
                RACE_LOG_MSGID_I("[RACE][Adaptive ANC Stream] Leave MP Mode, resume Adaptive ANC Stream.", 0);
                g_adaptive_ANC_stream_MP_control_flag = false; //leave MP mode
                audio_anc_monitor_adaptive_anc_stream_suspend_resume(TRUE); //resume Adaptive ANC Stream
                #endif
                pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                        RACE_DSPREALTIME_ANC,
                                        sizeof(RACE_CMD_ANC_PASSTHRU_ENTER_LEAVE_MP_MODE),
                                        channel_id);
                if(pEvt != NULL){
                    memset(pEvt, 0, sizeof(RACE_CMD_ANC_PASSTHRU_ENTER_LEAVE_MP_MODE));
                }else{
                    goto _HDR_END;
                }
                pEvt->param.MPmodeRsp.header.ancId  = pAnc_cmd->param.MPmodeCmd.header.ancId;
                pEvt->param.MPmodeRsp.header.status = anc_ret;
                break;
            }
            case RACE_ANC_EM_CMD:{
                anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_RACE, 0, (void *)pAnc_cmd);
                pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                        RACE_DSPREALTIME_ANC,
                                        sizeof(RACE_RSP_ANC_PASSTHRU_EM_MODE_PARAM),
                                        channel_id);
                if(pEvt != NULL){
                    memset(pEvt, 0, sizeof(RACE_RSP_ANC_PASSTHRU_EM_MODE_PARAM));
                }else{
                    goto _HDR_END;
                }
                pEvt->param.EMCmdRsp.header.ancId  = pAnc_cmd->param.EMCmd.header.ancId;
                pEvt->param.EMCmdRsp.header.status = anc_ret;
                break;
            }
            default:{
                pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                        RACE_DSPREALTIME_ANC,
                                        sizeof(RACE_ANC_PASSTHRU_HEADER),
                                        channel_id);
                if(pEvt != NULL){
                    memset(pEvt, 0, sizeof(RACE_ANC_PASSTHRU_HEADER));
                }else{
                    goto _HDR_END;
                }
                pEvt->param.header.status = RACE_ERRCODE_NOT_SUPPORT;
                return pEvt;
                break;
            }
        }
        if(pEvt != NULL){
            pEvt->param.header.status = (anc_ret == AUDIO_ANC_CONTROL_EXECUTION_SUCCESS) ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
        }
    } else {
        pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                RACE_DSPREALTIME_ANC,
                                sizeof(RACE_ANC_PASSTHRU_HEADER),
                                channel_id);
        if(pEvt != NULL){
            memset(pEvt, 0, sizeof(RACE_ANC_PASSTHRU_HEADER));
        }else{
            goto _HDR_END;
        }
        pEvt->param.header.status = RACE_ERRCODE_PARAMETER_ERROR;
    }

    if(pEvt != NULL){
        if(pEvt->param.header.status != RACE_ERRCODE_SUCCESS) {
            RACE_LOG_MSGID_E("RACE_DSPREALTIME_ANC_HDR pEvt->status: %d  anc_ret: %d\n",2,pEvt->param.header.status,anc_ret);
        }
    }
_HDR_END:
    return pEvt;
}
#endif
void* RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t mode;
        uint8_t enable;
    }PACKED RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_CMD_STRU;

    typedef struct
    {
        uint8_t status;
        uint8_t mode;
        uint8_t enable;
    }PACKED RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU;

    RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_CMD_STRU *pCmd = (RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_CMD_STRU *)pCmdMsg;
    RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU *pEvt = NULL;

    switch (pCmd->mode) {
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
        case LEAKAGE_DETECTION_MODE:
        {
            g_leakage_detection_race_ch_id = channel_id;
            audio_anc_leakage_detection_send_aws_mce_race_ch_id(g_leakage_detection_race_ch_id);
            //send race ch id to partner to prevent from sending race cmd fail in RHO case.

            race_anc_mutex_take();
            g_LD_result_agent = 0;
            g_LD_result_partner = 0;
            race_anc_mutex_give();
            pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE, pCmd->Hdr.id, sizeof(RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU) , channel_id);
            if (pEvt) {
                pEvt->mode = LEAKAGE_DETECTION_MODE;
                pEvt->enable = pCmd->enable;
                pEvt->status = 0;
                if (pCmd->enable == 1) {
                    if (bt_sink_srv_ami_get_current_scenario() == HFP) {
                        RACE_LOG_MSGID_I("[RECORD_LC] anc leakage detection is terminated by HFP",0);
                        race_flush_packet((void *)pEvt, g_leakage_detection_race_ch_id);
                        anc_leakage_detection_racecmd_response(LD_STATUS_TERMINATE, 0);
                        anc_leakage_detection_racecmd_response(LD_STATUS_TERMINATE, 1);
                        pEvt = NULL;
                    } else {
                        RACE_LOG_MSGID_I("[RECORD_LC] receive race cmd",0);
                        #ifndef MTK_ANC_V2
                        audio_anc_control_result_t anc_ret = audio_anc_leakage_detection_start(anc_leakage_detection_racecmd_callback);
                        #else
                        audio_anc_control_result_t anc_ret = audio_anc_leakage_detection_prepare(anc_leakage_detection_racecmd_callback);
                        #endif
                        if (anc_ret == AUDIO_ANC_CONTROL_EXECUTION_SUCCESS) {
//                            race_cmd_vp_struct* param = (race_cmd_vp_struct*)race_mem_alloc(sizeof(race_cmd_vp_struct));
//                            if(param){
//                                param->vp_type = RACE_CMD_VP_LEAKAGE_DETECTION;
//                                param->play_flag = true;
//                                RACE_ERRCODE ret = race_send_event_notify_msg(RACE_EVENT_TYPE_CMD_VP, param);
//                                if(RACE_ERRCODE_SUCCESS != ret){
//                                    race_mem_free(param);
//                                }
//                            }
                        } else {
                            anc_leakage_detection_racecmd_response(LD_STATUS_TERMINATE, 0);
                            RACE_LOG_MSGID_I("[RECORD_LC] anc leakage detection is not allowed:%d",1, anc_ret);
                        }
                        pEvt->status = (anc_ret == AUDIO_ANC_CONTROL_EXECUTION_SUCCESS) ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
                    }
                } else {
                    audio_anc_control_result_t anc_ret = audio_anc_leakage_detection_stop();
                    pEvt->status = (anc_ret == AUDIO_ANC_CONTROL_EXECUTION_SUCCESS) ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
                    RACE_LOG_MSGID_I("[RECORD_LC] anc leakage detection is terminated by end-user",0);
                }
            }
            break;
        }
#endif
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
        case ADAPTIVE_FF_ANC_MODE:
        {
            g_anc_race_ch_id = channel_id;
            if (pCmd->enable) {
                /*
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &user_trigger_ff_start_count);
                RACE_LOG_MSGID_I("[user_trigger_ff]race cmd user trigger adaptive FF start, start count:%d", 1, user_trigger_ff_start_count);
                audio_anc_user_trigger_ff_start(anc_user_trigger_adaptive_ff_callback);
                */
                race_send_event_notify_msg(RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_START, NULL); //Add new api to app do start
                RACE_FreePacket(pEvt);
                pEvt = NULL;
            } else {/*cancelled by user*/
                if (audio_anc_user_trigger_ff_get_status()) {
                    RACE_LOG_MSGID_I("[user_trigger_ff]user trigger adaptive FF is cancelled by end-user", 0);
                    audio_anc_user_trigger_ff_stop();
                    audio_anc_user_trigger_ff_recover_anc(ANC_K_STATUS_KEEP_DEFAULT);
                    pEvt->status = 0;
                    race_send_event_notify_msg(RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_CANCEL, NULL);
                }
            }
            break;
        }
#else
        case ADAPTIVE_FF_ANC_MODE:
        {
            g_anc_race_ch_id = channel_id;
#if 0
            pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE, pCmd->Hdr.id, sizeof(RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU) , channel_id);
            if (pEvt) {
                pEvt->mode = ADAPTIVE_FF_ANC_MODE;
                pEvt->enable = pCmd->enable;
                pEvt->status = 0;
                if (pCmd->enable) {
                    audio_user_trigger_adaptive_ff_register_vp_callback(user_trigger_adaptive_ff_vp_callback);

                    user_trigger_adaptive_ff_result_t reg_cb_res = audio_user_trigger_adaptive_ff_register_event_callback((anc_user_trigger_ff_event_callback_t)user_trigger_adaptive_ff_event_callback, USER_TRIGGER_ADAPTIVE_FF_EVENT_ALL);
                    RACE_LOG_MSGID_I("[user_trigger_ff]RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_HDR, reg_cb_res=%d", 1, reg_cb_res);

                    user_trigger_adaptive_ff_result_t res = audio_user_trigger_adaptive_ff_request((anc_user_trigger_ff_callback_t)user_trigger_adaptive_ff_racecmd_response);
                    RACE_LOG_MSGID_I("[user_trigger_ff]RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_HDR, res=%d", 1, res);
                    pEvt->status = res;

                } else {/*cancelled by user*/
                    RACE_LOG_MSGID_I("[user_trigger_ff]RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_HDR, cancelled by user", 0);
                    audio_user_trigger_adaptive_ff_terminate();
                }
            }
#endif
            RACE_LOG_MSGID_I("[user_trigger_ff]race user trigger adaptive FF enable %d", 1, pCmd->enable);
            if (pCmd->enable) {
                race_send_event_notify_msg(RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_START, NULL);
            } else {
                race_send_event_notify_msg(RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_CANCEL, NULL);
            }
            pEvt = NULL;
            break;
        }
#endif
#endif
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
        case USER_UNAWARE_DUBUG_MODE: {
            typedef struct
            {
                uint8_t status;
                uint8_t mode;
                uint8_t enable;
                user_unaware_info data;
            }PACKED RACE_DSPREALTIME_USER_UNAWARE_EVT_STRU;

            typedef struct
            {
                adaptive_check_notify_t header;
                uint8_t status;
                user_unaware_info data;
            }PACKED RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU;

            uint8_t *uu_info = NULL;
            bt_aws_mce_role_t role;
            //send 5B
            pEvt = NULL;
            RACE_DSPREALTIME_USER_UNAWARE_EVT_STRU *pEvtUU = NULL;
            pEvtUU = RACE_ClaimPacket(RACE_TYPE_RESPONSE, pCmd->Hdr.id, sizeof(RACE_DSPREALTIME_USER_UNAWARE_EVT_STRU), channel_id);
            if (pEvtUU) {
                pEvtUU->mode = USER_UNAWARE_DUBUG_MODE;
                pEvtUU->enable = pCmd->enable;
                pEvtUU->status = 0;

                //uint8_t *uu_gain_info = NULL;
                uu_info = pvPortMallocNC(sizeof(user_unaware_info));
                if(uu_info){
                    memset(uu_info, 0, sizeof(user_unaware_info));
                    audio_user_unaware_get_gain_info(uu_info);
                    memcpy((uint8_t*)&(pEvtUU->data), uu_info, sizeof(user_unaware_info));
                }

                role = bt_connection_manager_device_local_info_get_aws_role();
                if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
                    race_flush_packet((void *)pEvtUU, channel_id);
                } else {
                    race_send_pkt_t* pSndPkt;
                    pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pEvtUU);
                    race_pkt_t      *pret;
                    race_send_pkt_t *psend;
                    psend = (race_send_pkt_t *)pSndPkt;
                    pret = &psend->race_data;
                    #if (RACE_DEBUG_PRINT_ENABLE)
                    race_dump((uint8_t *)pret, RACE_DBG_EVT);
                    #endif
                    if(pSndPkt) {
                        if (BT_STATUS_SUCCESS != bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, channel_id, RACE_CMD_RSP_FROM_PARTNER,0)) {
                            RACE_LOG_MSGID_E("[relay_cmd][User Unaware]partner send relay req FAIL \n", 0);
                        }
                        race_mem_free(pSndPkt);
                    }
                }
            }
            //send 5D
            RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU *pNotiUU = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION, pCmd->Hdr.id, sizeof(RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU), channel_id);
            if (pNotiUU) {
                memset((uint8_t*)&(pNotiUU->header.start_or_stop), 0, sizeof(RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU));
                pNotiUU->header.mode = USER_UNAWARE_DUBUG_MODE;
                pNotiUU->header.total_data_length = sizeof(user_unaware_info)+1;
                pNotiUU->header.data_length = sizeof(user_unaware_info)+1;

                if (uu_info) {
                    pNotiUU->status = 0;
                    memcpy((uint8_t*)&(pNotiUU->data), uu_info, sizeof(user_unaware_info));
                } else {
                    pNotiUU->status = 0xFF;
                    memset((uint8_t*)&(pNotiUU->data), 0, sizeof(user_unaware_info));
                }
                role = bt_connection_manager_device_local_info_get_aws_role();
                if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
                    race_flush_packet((void *)pNotiUU, channel_id);
                } else {
                    race_send_pkt_t* pSndPkt;
                    pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pNotiUU);
                    race_pkt_t      *pret;
                    race_send_pkt_t *psend;
                    psend = (race_send_pkt_t *)pSndPkt;
                    pret = &psend->race_data;
                    #if (RACE_DEBUG_PRINT_ENABLE)
                    race_dump((uint8_t *)pret, RACE_DBG_EVT);
                    #endif
                    if(pSndPkt) {
                        if (BT_STATUS_SUCCESS != bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, channel_id, RACE_CMD_RSP_FROM_PARTNER,0)) {
                            RACE_LOG_MSGID_E("[relay_cmd][User Unaware]partner send relay req FAIL \n", 0);
                        }
                        race_mem_free(pSndPkt);
                    }
                }
            }
            if (uu_info) {
                vPortFreeNC(uu_info);
            }
            return NULL;
            break;
        }
        case USER_UNAWARE_MODE: {
            bt_aws_mce_role_t role;
            //send 5B
            pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE, pCmd->Hdr.id, sizeof(RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU) , channel_id);
            if (pEvt) {
                pEvt->enable = pCmd->enable;
                pEvt->mode = pCmd->mode;
                pEvt->status = 0;
                if (pCmd->enable) {
                    //Enable UU
                    audio_user_unaware_save_enable_status_to_nvdm(true);
                    audio_user_unaware_enable(true);
                } else {
                    //Disable UU
                    audio_user_unaware_save_enable_status_to_nvdm(false);
                    audio_user_unaware_enable(false);
                }
                role = bt_connection_manager_device_local_info_get_aws_role();
                if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
                    race_flush_packet((void *)pEvt, channel_id);
                } else {
                    race_send_pkt_t* pSndPkt;
                    pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pEvt);
                    race_pkt_t      *pret;
                    race_send_pkt_t *psend;
                    psend = (race_send_pkt_t *)pSndPkt;
                    pret = &psend->race_data;
                    #if (RACE_DEBUG_PRINT_ENABLE)
                    race_dump((uint8_t *)pret, RACE_DBG_EVT);
                    #endif
                    if(pSndPkt) {
                        if (BT_STATUS_SUCCESS != bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, channel_id, RACE_CMD_RSP_FROM_PARTNER,0)) {
                            RACE_LOG_MSGID_E("[relay_cmd][User Unaware]partner send relay req FAIL \n", 0);
                        }
                        race_mem_free(pSndPkt);
                    }
                }
                pEvt = NULL;
            }
            //send 5D
            typedef struct
            {
                adaptive_check_notify_t header;
                uint8_t status;
            }PACKED RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU;
            RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU *pNoti = NULL;
            pNoti = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION, pCmd->Hdr.id, sizeof(RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU), channel_id);
            if (pNoti) {
                memset((uint8_t*)&(pNoti->header.start_or_stop), 0, sizeof(RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU));
                pNoti->header.mode = USER_UNAWARE_MODE;
                pNoti->header.total_data_length = 1;
                pNoti->header.data_length = 1;
                pNoti->status = 0;
                role = bt_connection_manager_device_local_info_get_aws_role();
                if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
                    race_flush_packet((void *)pNoti, channel_id);
                } else {
                    race_send_pkt_t* pSndPkt;
                    pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pNoti);
                    race_pkt_t      *pret;
                    race_send_pkt_t *psend;
                    psend = (race_send_pkt_t *)pSndPkt;
                    pret = &psend->race_data;
                    #if (RACE_DEBUG_PRINT_ENABLE)
                    race_dump((uint8_t *)pret, RACE_DBG_EVT);
                    #endif
                    if(pSndPkt) {
                        if (BT_STATUS_SUCCESS != bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, channel_id, RACE_CMD_RSP_FROM_PARTNER,0)) {
                            RACE_LOG_MSGID_E("[relay_cmd][User Unaware]partner send relay req FAIL \n", 0);
                        }
                        race_mem_free(pSndPkt);
                    }
                }
            }

            RACE_LOG_MSGID_I("[User Unaware]enable %d by race", 1, pCmd->enable);
            break;
        }
#endif
#ifdef ADAPIVE_ANC_STREAM_CONTROL
        case ADAPTIVE_ANC_STREAM_MODE:{
            bt_aws_mce_role_t role;
            //send 5B
            pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE, pCmd->Hdr.id, sizeof(RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU) , channel_id);
            if (pEvt) {
                pEvt->enable = pCmd->enable;
                pEvt->mode = pCmd->mode;
                pEvt->status = 0;
                if (pCmd->enable) { //resume stream
                    RACE_LOG_MSGID_I("[RACE][Adapt ANC] Resume Adaptive ANC Stream.", 0);
                    g_adaptive_ANC_stream_suspend_control_flag = false; //resume Adaptive ANC Stream
                    #ifdef AIR_ANC_WIND_DETECTION_ENABLE
                    //wind enable
                    audio_wind_detection_enable(true);
                    g_wnd_enable = true;
                    RACE_LOG_MSGID_I("[RACE][Adapt ANC][Wind]resume WND. wnd %d, uu %d, ed %d\n", 3, g_wnd_enable, g_uu_enable, g_ed_enable);
                    //check if first enable
                    if((!g_uu_enable)&&(!g_ed_enable)&&(g_wnd_enable)){
                        RACE_LOG_MSGID_I("[RACE][Adapt ANC][Wind]first enable.\n", 0);
                        audio_anc_monitor_adaptive_anc_stream_suspend_resume(TRUE);
                    }
                    #else
                    audio_anc_monitor_adaptive_anc_stream_suspend_resume(TRUE);
                    #endif
                } else{ //suspend
                    RACE_LOG_MSGID_I("[RACE][Adapt ANC] Suspend Adaptive ANC Stream.", 0);
                    #ifdef AIR_ANC_WIND_DETECTION_ENABLE
                    //wind disable
                    audio_wind_detection_enable(false);
                    g_wnd_enable = false;
                    RACE_LOG_MSGID_I("[RACE][Adapt ANC][Wind]suspend WND. wnd %d, uu %d, ed %d\n", 3, g_wnd_enable, g_uu_enable, g_ed_enable);
                    //check if last disable
                    if((!g_uu_enable)&&(!g_ed_enable)&&(!g_wnd_enable)){
                        RACE_LOG_MSGID_I("[RACE][Adapt ANC][Wind]last disable.\n", 0);
                        audio_anc_monitor_adaptive_anc_stream_suspend_resume(FALSE);
                    }
                    #else
                    audio_anc_monitor_adaptive_anc_stream_suspend_resume(FALSE);
                    g_adaptive_ANC_stream_suspend_control_flag = true; //suspend Adaptive ANC Stream
                    #endif
                    //g_adaptive_ANC_stream_suspend_control_flag = true; //suspend Adaptive ANC Stream
                    #if 0
                    /* suspend Adaptive ANC Stream , reset extend gain. */
                    audio_anc_control_result_t anc_ret = AUDIO_ANC_CONTROL_EXECUTION_SUCCESS;
                    audio_anc_control_extend_ramp_cap_t init_ramp_cap;
                    init_ramp_cap.extend_gain_1 = 0;
                    init_ramp_cap.extend_gain_2 = 0;
                    init_ramp_cap.gain_type = AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE;
                    anc_ret = audio_anc_control_set_extend_gain(init_ramp_cap.gain_type, &init_ramp_cap, NULL);
                    init_ramp_cap.gain_type = AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE;
                    anc_ret = audio_anc_control_set_extend_gain(init_ramp_cap.gain_type, &init_ramp_cap, NULL);
                    init_ramp_cap.gain_type = AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION;
                    anc_ret = audio_anc_control_set_extend_gain(init_ramp_cap.gain_type, &init_ramp_cap, NULL);
                    RACE_LOG_MSGID_I("[RACE][Adaptive ANC Stream] Reset extend gain, result %d", 1, anc_ret);
                    #endif
                }
                role = bt_connection_manager_device_local_info_get_aws_role();
                if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
                    race_flush_packet((void *)pEvt, channel_id);
                } else {
                    race_send_pkt_t* pSndPkt;
                    pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pEvt);
                    race_pkt_t      *pret;
                    race_send_pkt_t *psend;
                    psend = (race_send_pkt_t *)pSndPkt;
                    pret = &psend->race_data;
                    #if (RACE_DEBUG_PRINT_ENABLE)
                    race_dump((uint8_t *)pret, RACE_DBG_EVT);
                    #endif
                    if(pSndPkt) {
                        if (BT_STATUS_SUCCESS != bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, channel_id, RACE_CMD_RSP_FROM_PARTNER,0)) {
                            RACE_LOG_MSGID_E("[relay_cmd][Adapt ANC]partner send relay req FAIL \n", 0);
                        }
                        race_mem_free(pSndPkt);
                    }
                }
                pEvt = NULL;
            }
            //send 5D
            typedef struct
            {
                adaptive_check_notify_t header;
                uint8_t status;
            }PACKED RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU;
            RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU *pNoti = NULL;
            pNoti = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION, pCmd->Hdr.id, sizeof(RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU), channel_id);
            if (pNoti) {
                memset((uint8_t*)&(pNoti->header.start_or_stop), 0, sizeof(RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU));
                pNoti->header.mode = ADAPTIVE_ANC_STREAM_MODE;
                pNoti->header.total_data_length = 1;
                pNoti->header.data_length = 1;
                pNoti->status = 0;
                role = bt_connection_manager_device_local_info_get_aws_role();
                if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
                    race_flush_packet((void *)pNoti, channel_id);
                } else {
                    race_send_pkt_t* pSndPkt;
                    pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pNoti);
                    race_pkt_t      *pret;
                    race_send_pkt_t *psend;
                    psend = (race_send_pkt_t *)pSndPkt;
                    pret = &psend->race_data;
                    #if (RACE_DEBUG_PRINT_ENABLE)
                    race_dump((uint8_t *)pret, RACE_DBG_EVT);
                    #endif
                    if(pSndPkt) {
                        if (BT_STATUS_SUCCESS != bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, channel_id, RACE_CMD_RSP_FROM_PARTNER,0)) {
                            RACE_LOG_MSGID_E("[relay_cmd][Adapt ANC]partner send relay req FAIL \n", 0);
                        }
                        race_mem_free(pSndPkt);
                    }
                }
            }
            break;
        }
        case ADAPTIVE_ANC_STREAM_STATUS:{
            RACE_LOG_MSGID_I("[RACE][Adapt ANC] Query Adaptive ANC Stream Status.", 0);
            bt_aws_mce_role_t role;
            //send 5B
            pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE, pCmd->Hdr.id, sizeof(RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU) , channel_id);
            if (pEvt) {
                pEvt->enable = pCmd->enable;
                pEvt->mode = pCmd->mode;
                pEvt->status = 0;
                uint8_t adapt_anc_stream_status;
                audio_anc_monitor_adaptive_anc_stream_get_status(&adapt_anc_stream_status);
                RACE_LOG_MSGID_I("[RACE][Adapt ANC] Query Adaptive ANC Stream Status, %d", 1, adapt_anc_stream_status);
                pEvt->enable = adapt_anc_stream_status;

                role = bt_connection_manager_device_local_info_get_aws_role();
                if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
                    race_flush_packet((void *)pEvt, channel_id);
                } else {
                    race_send_pkt_t* pSndPkt;
                    pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pEvt);
                    race_pkt_t      *pret;
                    race_send_pkt_t *psend;
                    psend = (race_send_pkt_t *)pSndPkt;
                    pret = &psend->race_data;
                    #if (RACE_DEBUG_PRINT_ENABLE)
                    race_dump((uint8_t *)pret, RACE_DBG_EVT);
                    #endif
                    if(pSndPkt) {
                        if (BT_STATUS_SUCCESS != bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, channel_id, RACE_CMD_RSP_FROM_PARTNER,0)) {
                            RACE_LOG_MSGID_E("[relay_cmd][Adapt ANC]partner send relay req FAIL \n", 0);
                        }
                        race_mem_free(pSndPkt);
                    }
                }
                pEvt = NULL;
            }
            //send 5D
            typedef struct
            {
                adaptive_check_notify_t header;
                uint8_t status;
            }PACKED RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU;
            RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU *pNoti = NULL;
            pNoti = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION, pCmd->Hdr.id, sizeof(RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU), channel_id);
            if (pNoti) {
                memset((uint8_t*)&(pNoti->header.start_or_stop), 0, sizeof(RACE_DSPREALTIME_USER_UNAWARE_NOTI_STRU));
                pNoti->header.mode = ADAPTIVE_ANC_STREAM_STATUS;
                pNoti->header.total_data_length = 1;
                pNoti->header.data_length = 1;
                pNoti->status = 0;
                role = bt_connection_manager_device_local_info_get_aws_role();
                if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
                    race_flush_packet((void *)pNoti, channel_id);
                } else {
                    race_send_pkt_t* pSndPkt;
                    pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pNoti);
                    race_pkt_t      *pret;
                    race_send_pkt_t *psend;
                    psend = (race_send_pkt_t *)pSndPkt;
                    pret = &psend->race_data;
                    #if (RACE_DEBUG_PRINT_ENABLE)
                    race_dump((uint8_t *)pret, RACE_DBG_EVT);
                    #endif
                    if(pSndPkt) {
                        if (BT_STATUS_SUCCESS != bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, channel_id, RACE_CMD_RSP_FROM_PARTNER,0)) {
                            RACE_LOG_MSGID_E("[relay_cmd][Adapt ANC]partner send relay req FAIL \n", 0);
                        }
                        race_mem_free(pSndPkt);
                    }
                }
            }
            break;
        }
#endif
        default:{
            RACE_LOG_MSGID_E("[RACE] RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_HDR, no such cmd.", 0);
            break;
        }
    }

    return pEvt;
}

#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
void race_dsprealtime_anc_adaptive_response(bool success, bool enable)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    typedef struct
    {
        uint8_t status;
        uint8_t mode;
        uint8_t enable;
    }PACKED RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU;

    RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                                         RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK,
                                                         sizeof(RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_EVT_STRU),
                                                         g_anc_race_ch_id);

    if (pEvt) {
        if (success) {
            pEvt->status = 0x00;
        } else {
            pEvt->status = 0xFF;
        }
        pEvt->mode = ADAPTIVE_FF_ANC_MODE;
        pEvt->enable = enable;
        if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
            race_flush_packet((void *)pEvt, g_anc_race_ch_id);
        } else {/*partner send agent result*/
            race_send_pkt_t* pSndPkt;
            pSndPkt = (void *)race_pointer_cnv_pkt_to_send_pkt((void *)pEvt);
            race_pkt_t      *pret;
            race_send_pkt_t *psend;
            psend = (race_send_pkt_t *)pSndPkt;
            pret = &psend->race_data;
            #if (RACE_DEBUG_PRINT_ENABLE)
            race_dump((uint8_t *)pret, RACE_DBG_EVT);
            #endif

            if(pSndPkt) {
                bt_status_t ret = bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, g_anc_race_ch_id, RACE_CMD_RSP_FROM_PARTNER,0);
                if (ret != BT_STATUS_SUCCESS) {
                    RACE_LOG_MSGID_E("[relay_cmd][user_trigger_ff]partner send relay req FAIL \n", 0);
                } else {
                    RACE_LOG_MSGID_I("[relay_cmd][user_trigger_ff]race_dsprealtime_anc_adaptive_response, send status:%d success", pEvt->status);
                }
                race_mem_free(pSndPkt);
            }
        }

        //send 5D
        user_trigger_adaptive_ff_racecmd_response(0, 0xF, NULL, 0, 0, enable, 0);
    } else {
        RACE_LOG_MSGID_I("[user_trigger_ff]race_dsprealtime_anc_adaptive_response, pkt claim fail", 0);
    }
}
#endif

void* RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("[user_trigger_ff]RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_HDR\n",0);
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t status;
        uint8_t mode;
#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
        uint8_t data0_id;
        uint16_t data0_len;
        short mobile_status;
        uint8_t data1_id;
        uint16_t data1_len;
        int32_t UU_alpha_para;
#endif
        uint8_t filter[1];
    }PACKED RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_CMD_STRU;

    typedef struct
    {
        uint8_t status;
        uint8_t mode;
#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
        uint8_t filter_selection;
#endif
    }PACKED RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_EVT_STRU;

    RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_CMD_STRU *pCmd = (RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_CMD_STRU *)pCmdMsg;
    RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_EVT_STRU *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                                         pCmd->Hdr.id,
                                                         sizeof(RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_EVT_STRU),
                                                         channel_id);
    g_anc_race_ch_id = channel_id;

    if (pEvt) {
        pEvt->status = 0xFF;
        switch (pCmd->mode) {
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
            case ADAPTIVE_FF_ANC_MODE:
            {

                if (audio_anc_user_trigger_ff_get_status()) {
                    if (pCmd->status == ANC_K_STATUS_Compare) {
                        uint16_t filter_length = (pCmd->Hdr.length >= 4) ? (pCmd->Hdr.length - 4) : 0; //Hdr.id: 2bytes, status: 1byte, mode: 1byte
                        RACE_LOG_MSGID_I("[user_trigger_ff]RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_HDR, status:%d, filter_length=%d\n",2, pCmd->status, filter_length);
                        #ifdef MTK_USER_TRIGGER_FF_ENABLE
                            audio_anc_user_trigger_ff_receive_filter(anc_user_trigger_adaptive_ff_receive_filter_callback, (uint8_t *)&pCmd->filter, (uint32_t)filter_length);
                            RACE_FreePacket(pEvt);
                            pEvt = NULL;
                        #endif
                    } else {
                        RACE_LOG_MSGID_I("[user_trigger_ff]RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_HDR, status:%d, keep default and stop record\n",1, pCmd->status);
                        pEvt->status = ANC_K_STATUS_KEEP_DEFAULT;
                        pEvt->mode = ADAPTIVE_FF_ANC_MODE;

                        //test, force to compare
        //                    uint32_t buf_size = sizeof(anc_fwd_iir_t);
        //                    anc_fwd_iir_t *tmp_buffer = (anc_fwd_iir_t *)pvPortMalloc(buf_size);
        //                    flash_memory_read_nvdm_data(NVKEY_DSP_PARA_ANC_L_FILTER2, (uint8_t *)tmp_buffer, &buf_size);
        //                    audio_anc_user_trigger_ff_receive_filter(NULL, (uint8_t *)tmp_buffer, buf_size);

                        audio_anc_user_trigger_ff_stop();
                        audio_anc_user_trigger_ff_recover_anc(ANC_K_STATUS_KEEP_DEFAULT);
                        race_send_event_notify_msg(RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_END, NULL);
                    }

                /*user trigger FF was terminated*/
                } else {
                    pEvt->status = ANC_K_STATUS_ABORT;
                    pEvt->mode = ADAPTIVE_FF_ANC_MODE;
                }
            }
            break;
#else
            case ADAPTIVE_FF_ANC_MODE:{
                RACE_LOG_MSGID_I("[user_trigger_ff]RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_HDR, status:%d, data0_id:0x%x, data0_len=%d, data1_id:0x%x, data1_len=%dn", 5, pCmd->status, pCmd->data0_id, pCmd->data0_len, pCmd->data1_id, pCmd->data1_len);

                pEvt->mode = ADAPTIVE_FF_ANC_MODE;

                if (pCmd->data0_id == USER_TRIGGER_ADAPTIVE_FF_DATA_MOBILE_STAT) {
                    if (pCmd->status == 0) {

                        RACE_LOG_MSGID_I("[user_trigger_ff][STAT]mobile status:%d\n", 1, pCmd->mobile_status);

                        if (pCmd->mobile_status == 0) {//change coef
                            if (pCmd->data1_len >= 188) {
                                typedef struct {
                                    uint8_t rw_filter;
                                    uint8_t *filter;
                                    S32 UU_alpha_par;
                                } user_trigger_adaptive_ff_rw_anc_filter;
                                user_trigger_adaptive_ff_rw_anc_filter rw_filter_cmd;
                                rw_filter_cmd.rw_filter = USER_TRIGGER_FF_WRITE_ANC_COEF;
                                rw_filter_cmd.filter = (uint8_t *)&pCmd->filter;
                                #ifdef AIR_ANC_USER_UNAWARE_ENABLE
                                rw_filter_cmd.UU_alpha_par = pCmd->UU_alpha_para;
                                #else
                                rw_filter_cmd.UU_alpha_par = -1;
                                #endif
                                int res = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_UTFF, 0, &rw_filter_cmd);
                                if (res > 0) {
                                    pEvt->status = (uint8_t)(res & 0xF);
                                    pEvt->filter_selection = (uint8_t)((res & 0xF0) >> 4);
                                } else {
                                    pEvt->status = 0xFF;
                                    pEvt->filter_selection = 0;
                                }

    //                            rw_filter_cmd.rw_filter = USER_TRIGGER_FF_READ_ANC_COEF;
    //                            rw_filter_cmd.filter = NULL;
    //                            audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_UTFF, 0, &rw_filter_cmd);
                                RACE_LOG_MSGID_I("[user_trigger_ff][RES]status:%d, filter_selection:%d\n", 2, pEvt->status, pEvt->filter_selection);

                            } else {
                                pEvt->status = 0xFF;
                                RACE_LOG_MSGID_E("[user_trigger_ff]wrong filter length\n", 0);
                            }
                        } else if (pCmd->mobile_status == 1) {
                            pEvt->status = 0;
                        }
                    }
                } else if (pCmd->data0_id == USER_TRIGGER_ADAPTIVE_FF_DATA_FACTORY_SETTING) {
                    RACE_LOG_MSGID_I("[user_trigger_ff]restore to factory settings\n", 0);
                    typedef struct {
                        uint8_t rw_filter;
                        uint8_t *filter;
                        S32 UU_alpha_par;
                    } user_trigger_adaptive_ff_rw_anc_filter;
                    user_trigger_adaptive_ff_rw_anc_filter rw_filter_cmd;
                    rw_filter_cmd.rw_filter = USER_TRIGGER_FF_FACTORY_SETTING;
                    rw_filter_cmd.filter = NULL;
                    #ifdef AIR_ANC_USER_UNAWARE_ENABLE
                    rw_filter_cmd.UU_alpha_par = 0;
                    #else
                    rw_filter_cmd.UU_alpha_par = -1;
                    #endif
                    int res = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_UTFF, 0, &rw_filter_cmd);
                    pEvt->status = (uint8_t)(res & 0xF);
                    pEvt->filter_selection = (uint8_t)((res & 0xF0) >> 4);
                    RACE_LOG_MSGID_I("[user_trigger_ff][RES]status:%d, filter_selection:%d\n", 2, pEvt->status, pEvt->filter_selection);

                } else {
                    pEvt->status = 0xFF;
                    RACE_LOG_MSGID_E("[user_trigger_ff]wrong data id\n", 0);
                }
                break;
            }

#endif
#endif
            default:
                break;
        }
    }

    return pEvt;
}


typedef enum {
    RACE_ANC_NOTIFY_GAIN_TYPE_USER_UNAWARE   = 0,
    RACE_ANC_NOTIFY_GAIN_TYPE_HOWLING_CONTROL,
    RACE_ANC_NOTIFY_GAIN_TYPE_WIND_NOISE,
    RACE_ANC_NOTIFY_GAIN_TYPE_ENVIRONMENT_DETECTION
} anc_notfiy_gain_type_t;

void* RACE_DSPREALTIME_ANC_GAIN_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        uint8_t type;
    } PACKED RACE_DSPREALTIME_ANC_GAIN_CMD_EVT_STRU;

    g_anc_race_ch_id = channel_id;

    RACE_DSPREALTIME_ANC_GAIN_CMD_EVT_STRU *pCmd = (RACE_DSPREALTIME_ANC_GAIN_CMD_EVT_STRU *)pCmdMsg->payload;
    RACE_LOG_MSGID_I("[anc_gain] recv ANC_GAIN RACE CMD, type=%d", 1, pCmd->type);
    switch (pCmd->type) {
        case RACE_ANC_NOTIFY_GAIN_TYPE_USER_UNAWARE: {
            race_send_event_notify_msg(RACE_EVENT_TYPE_ANC_GAIN_USER_UNAWARE, NULL);
            break;
        }
        case RACE_ANC_NOTIFY_GAIN_TYPE_WIND_NOISE: {
            race_send_event_notify_msg(RACE_EVENT_TYPE_ANC_GAIN_WIND_NOISE, NULL);
            break;
        }
        case RACE_ANC_NOTIFY_GAIN_TYPE_ENVIRONMENT_DETECTION: {
            race_send_event_notify_msg(RACE_EVENT_TYPE_ANC_GAIN_ENVIRONMENT_DETECTION, NULL);
            break;
        }
        default:
            break;
    }

    return NULL;
}

void* RACE_DSPREALTIME_ANC_GAIN_CONTROL_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        uint8_t type;
        uint8_t action;
    } PACKED RACE_DSPREALTIME_ANC_GAIN_CONTROL_CMD_EVT_STRU;

    g_anc_race_ch_id = channel_id;

    RACE_DSPREALTIME_ANC_GAIN_CONTROL_CMD_EVT_STRU *pCmd = (RACE_DSPREALTIME_ANC_GAIN_CONTROL_CMD_EVT_STRU *)pCmdMsg->payload;
    RACE_LOG_MSGID_I("[anc_gain] recv ANC_GAIN_CONTROL RACE CMD, type=%d action=%d",
                     2, pCmd->type, pCmd->action);

    anc_gain_control_param_t *param = (anc_gain_control_param_t *)pvPortMalloc(sizeof(anc_gain_control_param_t));
    if (param != NULL) {
        param->type = pCmd->type;
        param->action = pCmd->action;
        race_send_event_notify_msg(RACE_EVENT_TYPE_ANC_GAIN_CONTROL, (void *)param);
    }
    return NULL;
}

void* RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t mode;
        uint16_t data_length;
        uint8_t data[1];
    }PACKED RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND_CMD_STRU;

    typedef struct
    {
        uint8_t status;
        uint8_t mode;
        uint16_t data_length;
        uint8_t data[1];
    }PACKED RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND_EVT_STRU;

    RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND_CMD_STRU *pCmd = (RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND_CMD_STRU *)pCmdMsg;
    RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND_EVT_STRU *pEvt = NULL;

    g_anc_race_ch_id = channel_id;
    switch (pCmd->mode) {
        #ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
        case ADAPTIVE_FF_ANC_MODE: {
            race_dsprealtime_anc_adaptive_extend_t *pEvt2 = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                                         RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND,
                                                         sizeof(race_dsprealtime_anc_adaptive_extend_t),
                                                         channel_id);
            RACE_FreePacket(pEvt2);
            audio_user_trigger_adaptive_ff_extend_cmd_hdr(pCmd->data_length, pCmd->data, (anc_user_trigger_ff_extend_cmd_callback_t)user_trigger_adaptive_ff_racecmd_response_extend);
            break;
        }
        #endif
        #ifdef AIR_ANC_USER_UNAWARE_ENABLE
        case USER_UNAWARE_MODE: {
            race_dsprealtime_anc_adaptive_extend_t *pCmd2 = (race_dsprealtime_anc_adaptive_extend_t *)pCmdMsg->payload;
            RACE_LOG_MSGID_I("[User Unaware]extend cmd, mode:%d, sub_mode:%d", 2, pCmd2->param.UUCmd.hdr.mode, pCmd2->param.UUCmd.sub_mode);
            if (pCmd2->param.UUCmd.sub_mode == AUDIO_USER_UNAWARE_RACE_EXTEND_SUBMODE_GET_EN_STAT) {
                RACE_RSP_ANC_ADAPTIVE_EXTEND_USER_UNAWARE_GET_EN_STAT_T *pEvt2 = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                                                                                     RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND,
                                                                                                     sizeof(RACE_RSP_ANC_ADAPTIVE_EXTEND_USER_UNAWARE_GET_EN_STAT_T),
                                                                                                     channel_id);
                pEvt2->hdr.status = 0;
                pEvt2->hdr.mode = pCmd2->param.UUCmd.hdr.mode;
                pEvt2->hdr.data_len = sizeof(RACE_RSP_ANC_ADAPTIVE_EXTEND_USER_UNAWARE_GET_EN_STAT_T)-sizeof(RACE_RSP_ANC_ADAPTIVE_EXTEND_HDR_T);
                pEvt2->sub_mode = pCmd2->param.UUCmd.sub_mode;
                pEvt2->real_data_length = 1;
                pEvt2->enable = audio_user_unaware_get_enable_state();

                RACE_RSP_ANC_ADAPTIVE_EXTEND_USER_UNAWARE_GET_EN_STAT_T *pNoti2 = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                                                                                                     RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND,
                                                                                                     sizeof(RACE_RSP_ANC_ADAPTIVE_EXTEND_USER_UNAWARE_GET_EN_STAT_T),
                                                                                                     channel_id);
                memcpy((uint8_t*)&(pNoti2->hdr.status), (uint8_t*)&(pEvt2->hdr.status), sizeof(RACE_RSP_ANC_ADAPTIVE_EXTEND_USER_UNAWARE_GET_EN_STAT_T));

                bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
                if ((role == BT_AWS_MCE_ROLE_AGENT) || (role == BT_AWS_MCE_ROLE_NONE)) {
                    race_flush_packet((void *)pEvt2, channel_id);
                    race_flush_packet((void *)pNoti2, channel_id);
                } else {
                    race_send_pkt_t* pSndPkt;
                    //send 5B
                    pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pEvt2);
                    race_pkt_t      *pret;
                    race_send_pkt_t *psend;
                    psend = (race_send_pkt_t *)pSndPkt;
                    pret = &psend->race_data;
                    #if (RACE_DEBUG_PRINT_ENABLE)
                    race_dump((uint8_t *)pret, RACE_DBG_EVT);
                    #endif
                    if(pSndPkt) {
                        if (BT_STATUS_SUCCESS != bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, channel_id, RACE_CMD_RSP_FROM_PARTNER,0)) {
                            RACE_LOG_MSGID_E("[relay_cmd][User Unaware]partner send relay req FAIL \n", 0);
                        }
                        race_mem_free(pSndPkt);
                    }
                    //send 5D
                    pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)pNoti2);
                    psend = (race_send_pkt_t *)pSndPkt;
                    pret = &psend->race_data;
                    #if (RACE_DEBUG_PRINT_ENABLE)
                    race_dump((uint8_t *)pret, RACE_DBG_EVT);
                    #endif
                    if(pSndPkt) {
                        if (BT_STATUS_SUCCESS != bt_send_aws_mce_race_cmd_data(&pSndPkt->race_data, pSndPkt->length, channel_id, RACE_CMD_RSP_FROM_PARTNER,0)) {
                            RACE_LOG_MSGID_E("[relay_cmd][User Unaware]partner send relay req FAIL \n", 0);
                        }
                        race_mem_free(pSndPkt);
                    }

                }
            }
            break;
        }
        #endif

        default:
            break;
    }

    return pEvt;
}
#endif

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
typedef struct
{
    uint8_t status;
    uint8_t ancId;
}PACKED RACE_ANC_PASSTHRU_COSYS_HEADER;

typedef union RACE_ANC_PASSTHRU_COSYS
{
    RACE_ANC_PASSTHRU_COSYS_HEADER header;
}PACKED RACE_ANC_PASSTHRU_COSYS_UNION;

typedef struct
{
    RACE_ANC_PASSTHRU_COSYS_UNION param;
} PACKED race_dsprealtime_anc_cosys_struct;

static uint8_t g_anc_race_cosys_ch_id;
uint8_t        g_mmi_anc_race_cosys_ch_id;
uint16_t       g_anc_race_cosys_cmd_length = 0;
uint8_t        g_anc_race_cosys_enable_state = 0;
extern void race_mmi_get_anc_status(uint8_t *anc_status, uint8_t *status);
extern void race_mmi_get_passthru_gain(uint8_t *gain, uint8_t *status);
void RACE_DSPREALTIME_COSYS_GET_PARAM(am_feature_type_t audio_feature, void *data)
{
    switch(audio_feature){
        case AM_ANC:{
            uint8_t *p_get_status = data;
            race_dsprealtime_mutex_take();
            if (p_get_status != NULL) {
                *p_get_status = g_anc_race_cosys_enable_state;
            }
            race_dsprealtime_mutex_give();
        }
            break;
        default:
            RACE_LOG_MSGID_E("[RACE][COSYS] Audio GET_PARAM(%d) error.\n", 1, audio_feature);
            break;
    }
}
void RACE_DSPREALTIME_ANC_PASSTHRU_RELAY_COSYS_CALLBACK(bool is_critical, uint8_t *buff, uint32_t len)
{
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
#ifdef MTK_ANC_ENABLE
#ifdef MTK_ANC_V2
    /* Master: Get relay co-sys race cmd and process ANC event. */
    race_pkt_t *pCmdMsg = (ptr_race_pkt_t)buff;
    ptr_race_send_pkt_t pEvt = NULL;
    if(pCmdMsg->hdr.id == RACE_DSPREALTIME_ANC){
        pEvt = RACE_DSPREALTIME_ANC_PASSTHRU_HDR((ptr_race_pkt_t)buff, g_anc_race_cosys_ch_id);
    }else{
        RACE_MMI_GET_ENUM_CMD_STRU *pCmd = (RACE_MMI_GET_ENUM_CMD_STRU *)buff;
        RACE_MMI_GET_ENUM_EVT_STRU *pEvt_MMI = NULL;
        switch(pCmd->module){
            case RACE_MMI_MODULE_ANC_STATUS: {
                pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_MMI_GET_ENUM, (uint16_t)sizeof(RACE_MMI_GET_ENUM_EVT_STRU) + sizeof(uint16_t), g_mmi_anc_race_cosys_ch_id);
                if (pEvt) {
                    pEvt_MMI = (RACE_MMI_GET_ENUM_EVT_STRU *)pEvt;
                    pEvt_MMI->module = RACE_MMI_MODULE_ANC_STATUS;
                    race_mmi_get_anc_status(&pEvt_MMI->data[0], &pEvt_MMI->status);
                }
                break;
            }
            case RACE_MMI_MODULE_PASSTHRU_GAIN: {
                pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_MMI_GET_ENUM, (uint16_t)sizeof(RACE_MMI_GET_ENUM_EVT_STRU) + sizeof(uint8_t), g_mmi_anc_race_cosys_ch_id);
                if (pEvt) {
                    pEvt_MMI = (RACE_MMI_GET_ENUM_EVT_STRU *)pEvt;
                    pEvt_MMI->module = RACE_MMI_MODULE_PASSTHRU_GAIN;
                    race_mmi_get_passthru_gain(&pEvt_MMI->data[0], &pEvt_MMI->status);
                }
                break;
            }
            default:
                RACE_LOG_MSGID_E("[RACE][COSYS] Audio MMI module(%d) error.\n", 1, pCmd->module);
                break;
        }
    }
    /* Master: Send response to slave and ack to source. */
    if (pEvt) {
        pEvt = (void *)race_pointer_cnv_pkt_to_send_pkt(pEvt);

        race_cosys_send_data(RACE_COSYS_MODULE_ID_ANC_PASSTHROUGH, FALSE, (uint8_t *)&(pEvt->race_data), pEvt->race_data.hdr.length + 4);
        race_mem_free(pEvt); //Need free the space the ANC process malloc.
    } else {
        RACE_LOG_MSGID_E("[RACE][DSPREALTIME] ANC_PASSTHRU_RELAY_COSYS_CALLBACK malloc fail!", 0);
    }

#endif
#else
    RACE_LOG_MSGID_I("[RACE][COSYS] callback not enable ANC\n", 0);
#endif
#elif defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
    /* Slave: Get master response and ack to source. */
    ptr_race_send_pkt_t pEvt = NULL;
    uint8_t race_ch_id = 0;
    ptr_race_pkt_t response = (ptr_race_pkt_t)buff;
    RACE_LOG_MSGID_I("[RACE][COSYS] response (0x%x) (0x%x) (0x%x) (0x%x)\n", 4, response->hdr.pktId, response->hdr.type, response->hdr.length, response->hdr.id);
    if(response->hdr.id == RACE_DSPREALTIME_ANC){
        if(response->hdr.type == RACE_TYPE_RESPONSE){
            race_ch_id = g_anc_race_cosys_ch_id;
            pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                    RACE_DSPREALTIME_ANC,
                                    response->hdr.length - 2,
                                    race_ch_id);
        }else{
            RACE_DSPREALTIME_COSYS_EVT_STRU *cosys_anc_event = (RACE_DSPREALTIME_COSYS_EVT_STRU *)response->payload;
            RACE_LOG_MSGID_I("[RACE][COSYS] Slave get Master local ANC active(%d)\n", 1, cosys_anc_event->event_id);
            race_dsprealtime_mutex_take();
            if(cosys_anc_event->event_id == 1 << 0){
                g_anc_race_cosys_enable_state = 1; //anc_enable
            } else {
                g_anc_race_cosys_enable_state = 0; //anc_disable
            }
            race_dsprealtime_mutex_give();
            bt_sink_srv_am_feature_t feature_param;
            memset(&feature_param, 0, sizeof(bt_sink_srv_am_feature_t));
            feature_param.type_mask = AM_AUDIO_COSYS;
            feature_param.feature_param.cosys_param.sub_type    = AM_ANC;
            feature_param.feature_param.cosys_param.cosys_event = cosys_anc_event->event_id;
            am_audio_set_feature(FEATURE_NO_NEED_ID,&feature_param);
        }
    }else{
        race_ch_id = g_mmi_anc_race_cosys_ch_id;
        pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                RACE_MMI_GET_ENUM,
                                response->hdr.length - 2,
                                race_ch_id);
    }
    if (pEvt) {
        memcpy(pEvt, &(response->payload),response->hdr.length - 2);
        race_flush_packet((void *)pEvt, race_ch_id);
    }
#endif
    return;
}

/**
 * RACE_DSPREALTIME_ANC_COSYS_HDR
 *
 * RACE_DSPREALTIME_ANC_COSYS_HDR Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_DSPREALTIME_ANC_PASSTHRU_COSYS_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    /* Slave: no define ANC enable, send relay co-sys race cmd to master. */
    bool ret = FALSE;
    g_anc_race_cosys_ch_id = channel_id;

    race_dsprealtime_anc_cosys_struct *pAnc_cmd;
    pAnc_cmd = (race_dsprealtime_anc_cosys_struct *)pCmdMsg->payload;
    g_anc_race_cosys_cmd_length = pCmdMsg->hdr.length - 4;
    ret = race_cosys_send_data(RACE_COSYS_MODULE_ID_ANC_PASSTHROUGH, FALSE, (uint8_t *)pCmdMsg, pCmdMsg->hdr.length + 4);
    RACE_LOG_MSGID_I("[RACE][ANC_PTHR][COSYS]send cosys data ret:%d, type(0x%x) msg_length:%d anc_id:%d\n", 4, ret, pCmdMsg->hdr.type, pCmdMsg->hdr.length, pAnc_cmd->param.header.ancId);
    return NULL;
}

void RACE_DSPREALTIME_callback(uint32_t event_id)
{
    ptr_race_send_pkt_t pEvt = NULL;
    RACE_DSPREALTIME_COSYS_EVT_STRU *pEvt_cosys = NULL;
    pEvt = RACE_ClaimPacket(RACE_TYPE_COMMAND,
                            RACE_DSPREALTIME_ANC,
                            sizeof(RACE_DSPREALTIME_COSYS_EVT_STRU),
                            0);
    if (pEvt) {
        pEvt_cosys = (RACE_DSPREALTIME_COSYS_EVT_STRU *)pEvt;
        pEvt_cosys->event_id = event_id;
        pEvt = (void *)race_pointer_cnv_pkt_to_send_pkt(pEvt);

        race_cosys_send_data(RACE_COSYS_MODULE_ID_ANC_PASSTHROUGH, FALSE, (uint8_t *)&(pEvt->race_data), pEvt->race_data.hdr.length + 4);
        race_mem_free(pEvt); //Need free the space the ANC process malloc.
    } else {
        RACE_LOG_MSGID_E("[RACE][DSPREALTIME] DSPREALTIME_callback malloc fail!", 0);
    }

}
#endif

void* race_cmd_dsprealtime_handler(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id)
{
    void* ptr = NULL;

    RACE_LOG_MSGID_I("RACE_CmdHandler_DSPREALTIME, type[0x%X], race_id[0x%X], channel_id[%d]", 3,
        pRaceHeaderCmd->hdr.type, pRaceHeaderCmd->hdr.id, channel_id);

    switch (pRaceHeaderCmd->hdr.id)
    {
        case RACE_DSPREALTIME_SUSPEND:
        {
            ptr = RACE_DSPREALTIME_SUSPEND_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_DSPREALTIME_RESUME:
        {
            ptr = RACE_DSPREALTIME_RESUME_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
        case RACE_DSPREALTIME_PEQ:
        {
            ptr = RACE_DSPREALTIME_PEQ_HDR(pRaceHeaderCmd, channel_id);
        }
        break;
#endif

        case RACE_DSPSOUND_EFFECT:
        {
            // reserved.
        }
        break;

        case RACE_DSPREALTIME_GET_REFRENCE_GAIN:
        {
            ptr = RACE_DSPREALTIME_GET_REFRENCE_GAIN_HDR(pRaceHeaderCmd, channel_id);
        }
        break;
#ifdef MTK_AIRDUMP_EN_MIC_RECORD
        case RACE_DSPREALTIME_AIRDUMP_ENTRY:
        {
            ptr = RACE_DSPREALTIME_AIRDUMP_COMMON_HDR(pRaceHeaderCmd, channel_id);
        }
        break;
#endif
#ifdef MTK_AIRDUMP_EN
        case RACE_DSPREALTIME_AIRDUMP_ON_OFF:
        {
            ptr = RACE_DSPREALTIME_AIRDUMP_HDR(pRaceHeaderCmd, channel_id);
        }
        break;
#endif
        case RACE_DSPREALTIME_2MIC_SWAP:
        {
            ptr = RACE_DSPREALTIME_2MIC_SWAP_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_DSPREALTIME_AECNR_ON_OFF:
        {
            ptr = RACE_DSPREALTIME_AECNR_ON_OFF_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_DSPREALTIME_QUERY_LIB_VERSION:
        {
            ptr = RACE_DSPREALTIME_QUERY_LIB_VERSION_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_DSPREALTIME_SWGAIN_BYPASS:
        {
            ptr = RACE_DSPREALTIME_SWGAIN_EN_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_DSPREALTIME_OPEN_ALL_MIC:
        {
            ptr = RACE_DSPREALTIME_OPEN_ALL_MIC_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

#if defined(HAL_AUDIO_MODULE_ENABLED)
        case RACE_DSPREALTIME_OPEN_ALL_MIC_EXTEND:
        {
            ptr = RACE_DSPREALTIME_OPEN_ALL_MIC_HDR_EXTEND(pRaceHeaderCmd, channel_id);
        }
        break;
#endif

#ifdef MTK_ANC_ENABLE
#ifdef MTK_ANC_V2
        case RACE_DSPREALTIME_ANC:
        {
            ptr = RACE_DSPREALTIME_ANC_PASSTHRU_HDR(pRaceHeaderCmd, channel_id);
        }
        break;
#else
        case RACE_DSPREALTIME_ANC:
        {
            ptr = RACE_DSPREALTIME_ANC_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_DSPREALTIME_PASS_THROUGH_ON_OFF:
        case RACE_DSPREALTIME_PASS_THROUGH_TEST:
        {
            ptr = RACE_DSPREALTIME_PASSTHRU_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_DSPREALTIME_PASS_THROUGH_TEST_MUTE:
        {
            ptr = RACE_DSPREALTIME_PASSTHRU_TEST_MUTE_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

#endif
        case RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK:
        {
            ptr = RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT:
        {
            ptr = RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_DSPREALTIME_ANC_NOTIFY_GAIN:
        {
            ptr = RACE_DSPREALTIME_ANC_GAIN_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND:
        {
            ptr = RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_DSPREALTIME_ANC_GAIN_CONTROL:
        {
            ptr = RACE_DSPREALTIME_ANC_GAIN_CONTROL_HDR(pRaceHeaderCmd, channel_id);
        }
        break;
#else
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
        case RACE_DSPREALTIME_ANC:
        {
            ptr = RACE_DSPREALTIME_ANC_PASSTHRU_COSYS_HDR(pRaceHeaderCmd, channel_id);
        }
        break;
#endif
#endif

        default:
            break;
    }

    return ptr;
}


void* RACE_CmdHandler_DSPREALTIME(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id)
{

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
    if (race_dsprealtime_relay_check(pRaceHeaderCmd)) {
        //Relay cmd
        race_dsprealtime_relay_handler (pRaceHeaderCmd, length, channel_id, true, (uint8_t)RACE_ERRCODE_SUCCESS);
        return NULL;
    } else {
        return race_cmd_dsprealtime_handler(pRaceHeaderCmd, length, channel_id);
    }
#else
    return race_cmd_dsprealtime_handler(pRaceHeaderCmd, length, channel_id);
#endif
}



#ifdef MTK_ANC_ENABLE

typedef struct
{
    uint8_t     status;
    uint8_t     type;
    int16_t     gain_1;
    int16_t     gain_2;
    uint8_t     level;
    int16_t     local_stationary_noise;
    int16_t     peer_stationary_noise;
} PACKED RACE_DSPREALTIME_ANC_REPLY_GAIN_EVT_STRU;

typedef struct
{
    uint8_t     type;
    int16_t     gain_1;
    int16_t     gain_2;
    uint8_t     level;
    int16_t     local_stationary_noise;
    int16_t     peer_stationary_noise;
} PACKED RACE_DSPREALTIME_ANC_NOTIFY_GAIN_EVT_STRU;

typedef struct
{
    uint8_t     status;
    uint8_t     type;
    bool        enable;
} PACKED RACE_DSPREALTIME_ANC_GAIN_CONTROL_RSP_EVT_STRU;

void race_dsprealtime_anc_notify_gain_error(uint8_t status, uint8_t event_id)
{
    uint8_t type = 0;
    if (event_id == RACE_EVENT_TYPE_ANC_GAIN_USER_UNAWARE) {
        type = RACE_ANC_NOTIFY_GAIN_TYPE_USER_UNAWARE;
    } else if (event_id == RACE_EVENT_TYPE_ANC_GAIN_WIND_NOISE) {
        type = RACE_ANC_NOTIFY_GAIN_TYPE_WIND_NOISE;
    } else if (event_id == RACE_EVENT_TYPE_ANC_GAIN_ENVIRONMENT_DETECTION) {
        type = RACE_ANC_NOTIFY_GAIN_TYPE_ENVIRONMENT_DETECTION;
    } else {
        RACE_LOG_MSGID_E("[anc_gain] anc_notify_gain_error, invalid event_id=%d", 1, event_id);
        return;
    }

    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    RACE_LOG_MSGID_I("[anc_gain] anc_notify_gain_error, [%02X] status=%d type=%d", 3, role, status, type);
    if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
        RACE_LOG_MSGID_E("[anc_gain] anc_notify_gain_error, not agent role", 0);
        return;
    }

    RACE_DSPREALTIME_ANC_REPLY_GAIN_EVT_STRU *event = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                                                       RACE_DSPREALTIME_ANC_NOTIFY_GAIN,
                                                                       sizeof(RACE_DSPREALTIME_ANC_REPLY_GAIN_EVT_STRU),
                                                                       g_anc_race_ch_id);
    if (event != NULL) {
        event->status = status;
        event->type = type;
        event->gain_1 = 0;
        event->gain_2 = 0;
        event->level = 0;
        event->local_stationary_noise = 0;
        event->peer_stationary_noise = 0;
        race_status_t race_status = race_flush_packet((void *)event, g_anc_race_ch_id);
        RACE_LOG_MSGID_I("[anc_gain] anc_notify_gain_error, race_status=%d", 1, race_status);
    } else {
        RACE_LOG_MSGID_E("[anc_gain] anc_notify_gain_error, pkt claim fail", 0);
    }
}

void race_dsprealtime_anc_gain_control_response(uint8_t status, uint8_t type, bool enable)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    RACE_LOG_MSGID_I("[anc_gain] anc_gain_control_response, [%02X] status=%d type=%d enable=%d",
                     4, role, status, type, enable);
    if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
        RACE_LOG_MSGID_E("[anc_gain] anc_gain_control_response, not agent role", 0);
        return;
    }

    RACE_DSPREALTIME_ANC_GAIN_CONTROL_RSP_EVT_STRU *event = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                                                             RACE_DSPREALTIME_ANC_GAIN_CONTROL,
                                                                             sizeof(RACE_DSPREALTIME_ANC_GAIN_CONTROL_RSP_EVT_STRU),
                                                                             g_anc_race_ch_id);
    if (event != NULL) {
        event->status = status;
        event->type = type;
        event->enable = enable;
        race_status_t race_status = race_flush_packet((void *)event, g_anc_race_ch_id);
        RACE_LOG_MSGID_I("[anc_gain] anc_gain_control_response, race_status=%d", 1, race_status);
    } else {
        RACE_LOG_MSGID_E("[anc_gain] anc_gain_control_response, pkt claim fail", 0);
    }
}

#if (defined AIR_ANC_USER_UNAWARE_ENABLE) || (defined AIR_ANC_WIND_DETECTION_ENABLE) || (defined AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
static void race_dsprealtime_anc_notify_gain_result(anc_notfiy_gain_type_t type,
                                                    bool is_response,
                                                    int16_t gain_1,
                                                    int16_t gain_2,
                                                    uint8_t level,
                                                    int16_t local_stationary_noise,
                                                    int16_t peer_stationary_noise)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    RACE_LOG_MSGID_I("[anc_gain] anc_notify_gain, [%02X] type=%d is_response=%d gain_1=%d gain_2=%d level=%d local_stationary_noise=%d peer_stationary_noise=%d",
                     8, role, type, is_response, gain_1, gain_2, level, local_stationary_noise, peer_stationary_noise);

    if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
        RACE_LOG_MSGID_E("[anc_gain] anc_notify_gain, not agent role", 0);
        return;
    }

    if (is_response) {
        RACE_DSPREALTIME_ANC_REPLY_GAIN_EVT_STRU *event = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                                                           RACE_DSPREALTIME_ANC_NOTIFY_GAIN,
                                                                           sizeof(RACE_DSPREALTIME_ANC_REPLY_GAIN_EVT_STRU),
                                                                           g_anc_race_ch_id);
        if (event != NULL) {
            event->status = RACE_ANC_GAIN_STATUS_SUCCESS;
            event->type = type;
            event->gain_1 = gain_1;
            event->gain_2 = gain_2;
            event->level = level;
            event->local_stationary_noise = local_stationary_noise;
            event->peer_stationary_noise = peer_stationary_noise;
            race_status_t status = race_flush_packet((void *)event, g_anc_race_ch_id);
            RACE_LOG_MSGID_I("[anc_gain] anc_notify_gain, race_status=%d", 1, status);
        } else {
            RACE_LOG_MSGID_E("[anc_gain] anc_notify_gain, pkt claim fail", 0);
        }
    } else {
        RACE_DSPREALTIME_ANC_NOTIFY_GAIN_EVT_STRU *event = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                                                                            RACE_DSPREALTIME_ANC_NOTIFY_GAIN,
                                                                            sizeof(RACE_DSPREALTIME_ANC_NOTIFY_GAIN_EVT_STRU),
                                                                            g_anc_race_ch_id);
        if (event != NULL) {
            event->type = type;
            event->gain_1 = gain_1;
            event->gain_2 = gain_2;
            event->level = level;
            event->local_stationary_noise = local_stationary_noise;
            event->peer_stationary_noise = peer_stationary_noise;
            race_status_t status = race_flush_packet((void *)event, g_anc_race_ch_id);
            RACE_LOG_MSGID_I("[anc_gain] anc_notify_gain, race_status=%d", 1, status);
        } else {
            RACE_LOG_MSGID_E("[anc_gain] anc_notify_gain, pkt claim fail", 0);
        }
    }

}
#endif

// User Unaware
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
void race_dsprealtime_anc_user_unaware_response(int16_t gain)
{
    race_dsprealtime_anc_notify_gain_result(RACE_ANC_NOTIFY_GAIN_TYPE_USER_UNAWARE, TRUE, gain, 0, 0, 0, 0);
}

void race_dsprealtime_anc_user_unaware_notify(int16_t gain)
{
    race_dsprealtime_anc_notify_gain_result(RACE_ANC_NOTIFY_GAIN_TYPE_USER_UNAWARE, FALSE, gain, 0, 0, 0, 0);
}
#endif

#ifdef AIR_ANC_WIND_DETECTION_ENABLE
void race_dsprealtime_anc_wind_noise_response(int16_t gain)
{
    race_dsprealtime_anc_notify_gain_result(RACE_ANC_NOTIFY_GAIN_TYPE_WIND_NOISE, TRUE, gain, 0, 0, 0, 0);
}

void race_dsprealtime_anc_wind_noise_notify(int16_t gain)
{
    race_dsprealtime_anc_notify_gain_result(RACE_ANC_NOTIFY_GAIN_TYPE_WIND_NOISE, FALSE, gain, 0, 0, 0, 0);
}
#endif

// Environment Detection
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
void race_dsprealtime_anc_environment_detection_response(uint8_t level, int16_t ff_gain, int16_t fb_gain,
                                                         int16_t local_stationary_noise, int16_t peer_stationary_noise)
{
    race_dsprealtime_anc_notify_gain_result(RACE_ANC_NOTIFY_GAIN_TYPE_ENVIRONMENT_DETECTION, TRUE,
                                            ff_gain, fb_gain, level,
                                            local_stationary_noise, peer_stationary_noise);
}

void race_dsprealtime_anc_environment_detection_notify(uint8_t level, int16_t ff_gain, int16_t fb_gain,
                                                       int16_t local_stationary_noise, int16_t peer_stationary_noise)
{
    race_dsprealtime_anc_notify_gain_result(RACE_ANC_NOTIFY_GAIN_TYPE_ENVIRONMENT_DETECTION,
                                            FALSE, ff_gain, fb_gain, level,
                                            local_stationary_noise, peer_stationary_noise);
}
#endif

#endif /* MTK_ANC_ENABLE */

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
static const uint16_t g_race_dsprealtime_relay_id_lists[] = RACE_DSPREALTIME_RELAY_ID_LIST;
static const uint8_t g_race_dsprealtime_relay_0xE06_subid_lists[] = RACE_DSPREALTIME_RELAY_0x0E06_SUBID_LIST;

bool race_dsprealtime_relay_check (ptr_race_pkt_t pRaceHeaderCmd)
{
    uint32_t i, loop_count;
    bool found_in_list = false;
#if 0
    if (pRaceHeaderCmd->hdr.id == RACE_DSPREALTIME_ANC) {
        race_dsprealtime_anc_struct *pAnc_cmd;
        pAnc_cmd = (race_dsprealtime_anc_struct *)pRaceHeaderCmd->payload;
        loop_count = sizeof(g_race_dsprealtime_relay_0xE06_subid_lists)/sizeof(uint8_t);
        for (i=0 ; i<loop_count ; i++) {
            if (pAnc_cmd->param.header.ancId == g_race_dsprealtime_relay_0xE06_subid_lists[i]) {
                found_in_list = true;
                break;
            }
        }
#else
    if (0) {
#endif
    } else {

        loop_count = sizeof(g_race_dsprealtime_relay_id_lists)/sizeof(uint16_t);
        for (i=0 ; i<loop_count ; i++) {
            if (pRaceHeaderCmd->hdr.id == g_race_dsprealtime_relay_id_lists[i]) {
                found_in_list = true;
                break;
            }
        }
    }
    return found_in_list;
}

uint8_t race_dsprealtime_relay_check_evt_result (void *p_evt)
{
    typedef struct
    {
        uint8_t Status;
    }PACKED RACE_DSPREALTIME_EVT_STRU;
    RACE_DSPREALTIME_EVT_STRU *evt_ptr = p_evt;
    if (evt_ptr->Status == (uint8_t)RACE_ERRCODE_SUCCESS) {
        return RACE_ERRCODE_SUCCESS;
    } else {
        return RACE_ERRCODE_FAIL;
    }
}

bool race_dsprealtime_relay_handler (ptr_race_pkt_t pRaceHeaderCmd, uint16_t cmd_length, uint8_t channel_id, uint8_t send_to_follower, uint8_t cosys_result)
{
    race_dsprealtime_relay_packet_ptr_t relay_ptr;
    uint32_t relay_length = sizeof(race_dsprealtime_relay_packet_t) + cmd_length;

    relay_ptr = (race_dsprealtime_relay_packet_ptr_t)race_mem_alloc(relay_length);

    if (relay_ptr) {
        relay_ptr->source_channel_id = channel_id;
        relay_ptr->send_to_follower = send_to_follower;
        relay_ptr->follower_result = cosys_result;
        memcpy(&relay_ptr->race_cmd_pkt, pRaceHeaderCmd, sizeof(pRaceHeaderCmd)+cmd_length);
        if (!race_cosys_send_data(RACE_COSYS_MODULE_ID_DSP_COMMON, FALSE, (uint8_t *)relay_ptr, relay_length)) {
            RACE_LOG_MSGID_E("[RACE][DSPREALTIME][COSYS] send fail \n", 0);
            assert(0);
        }
        RACE_LOG_MSGID_I("[RACE][DSPREALTIME][COSYS] Send cosys cmd ID:%d, relay_length:%d \n", 2, pRaceHeaderCmd->hdr.id, relay_length);
        race_mem_free(relay_ptr);
    } else {
        RACE_LOG_MSGID_E("[RACE][DSPREALTIME][COSYS] alloc fail \n", 0);
        assert(0);
    }
    return true;
}

void race_dsprealtime_cosys_relay_callback(bool is_critical, uint8_t *buff, uint32_t len)
{
    void* ptr;
    race_dsprealtime_relay_packet_ptr_t relay_ptr = (race_dsprealtime_relay_packet_ptr_t)buff;
    ptr_race_pkt_t cmd_ptr = &relay_ptr->race_cmd_pkt;

    if (!relay_ptr->send_to_follower) {
        /* Receive get response from  Follower*/
        if (relay_ptr->follower_result) {
            //
            RACE_LOG_MSGID_W("[RACE][DSPREALTIME][COSYS] Receiver return FAIL from Follower, ID:%d \n", 1, cmd_ptr->hdr.id);
        }
        {
            //Process CMD
            ptr = race_cmd_dsprealtime_handler(cmd_ptr, cmd_ptr->hdr.length, relay_ptr->source_channel_id);
            if (ptr) {
                if (race_dsprealtime_relay_check_evt_result(ptr)) {
                    RACE_LOG_MSGID_W("[RACE][DSPREALTIME][COSYS] Receiver cmd handler FAIL, ID:%d\n", 1, cmd_ptr->hdr.id);
                }


                //Send result to cmd source
                race_send_pkt_t *pEvt;
                uint32_t send_length;
                pEvt = (void *)race_pointer_cnv_pkt_to_send_pkt(ptr);
                send_length = race_port_send_data(race_get_port_handle_by_channel_id(relay_ptr->source_channel_id), (uint8_t*)&pEvt->race_data, (uint32_t)pEvt->length);
                if (send_length != pEvt->length) {
                    RACE_LOG_MSGID_W("[RACE][DSPREALTIME][COSYS] Receiver cmd responset to send to source FAIL, ID:%d\n", 1, cmd_ptr->hdr.id);
                }
                race_mem_free(pEvt);


            } else {
                RACE_LOG_MSGID_W("[RACE][DSPREALTIME][COSYS] Receiver cmd handler dosen't have return, ID:%d\n", 1, cmd_ptr->hdr.id);
            }
        }



    } else {
        /* Follower receive the CMD */

        //Process CMD
        ptr = race_cmd_dsprealtime_handler(cmd_ptr, cmd_ptr->hdr.length, relay_ptr->source_channel_id);
        if (ptr) {
            relay_ptr->follower_result = race_dsprealtime_relay_check_evt_result(ptr);
            if (relay_ptr->follower_result) {
                RACE_LOG_MSGID_W("[RACE][DSPREALTIME][COSYS] Follower cmd handler FAIL, ID:%d\n", 1, cmd_ptr->hdr.id);
            }
            //Reply to received CMD chip
            race_dsprealtime_relay_handler (cmd_ptr, cmd_ptr->hdr.length, relay_ptr->source_channel_id, false, relay_ptr->follower_result);

            //Free evt mem
            ptr = (void *)race_pointer_cnv_pkt_to_send_pkt(ptr);
            race_mem_free(ptr);
        } else {
            RACE_LOG_MSGID_W("[RACE][DSPREALTIME][COSYS] Follower cmd handler dosen't have return, ID:%d\n", 1, cmd_ptr->hdr.id);
        }
    }
    RACE_LOG_MSGID_I("[RACE][DSPREALTIME][COSYS]callback id:%d, is_follower:%d \n", 2, cmd_ptr->hdr.id, relay_ptr->send_to_follower);
}
#endif

#endif /* RACE_DSP_REALTIME_CMD_ENABLE */

