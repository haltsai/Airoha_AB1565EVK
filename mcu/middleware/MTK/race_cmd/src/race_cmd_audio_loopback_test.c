/* Copyright Statement:
 *
 * (C) 2018  Airoha Technology Corp. All rights reserved.
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

#ifdef MTK_AUDIO_LOOPBACK_TEST_ENABLE

#include "race_cmd.h"
#include "race_xport.h"
#include "race_cmd_audio_loopback_test.h"
#if defined (MTK_AUDIO_TRANSMITTER_ENABLE)
#include "audio_transmitter_control.h"
#endif
#include "FreeRTOS.h"
#include "timers.h"
#include "bt_sink_srv_ami.h"

/**************************************************************************************************
* Define
**************************************************************************************************/
#define RACE_AUDIO_LOOPBACK_TEST_RSP_SUCCESS     0x00
#define RACE_AUDIO_LOOPBACK_TEST_RSP_FAIL        0xFF

#define RACE_AUDIO_LOOPBACK_TEST_RSP_LEN         0x01

#define RACE_AUDIO_LOOPBACK_TEST_TEST_CMD        0x1000
#define RACE_AUDIO_LOOPBACK_TEST_CHANGE_TIME_CMD 0x1001
#define RACE_AUDIO_LOOPBACK_TEST_TEST_RESULT_NEW 0x1002

#define RACE_AUDIO_LOOPBACK_TEST_MODE_ADC0       0x02
#define RACE_AUDIO_LOOPBACK_TEST_MODE_ADC1       0x03
#define RACE_AUDIO_LOOPBACK_TEST_MODE_ADC2       0x07
#define RACE_AUDIO_LOOPBACK_TEST_MODE_ADC3       0x08
#define RACE_AUDIO_LOOPBACK_TEST_MODE_ADC4       0x09
#define RACE_AUDIO_LOOPBACK_TEST_MODE_ADC5       0x0A

#define RACE_AUDIO_LOOPBACK_TEST_POW_INIT        (-192)

#define RACE_AUDIO_LOOPBACK_TEST_TIMER_TEST      "race_audio_loopback_test_test"
#define RACE_AUDIO_LOOPBACK_TEST_TIMER_CHECK     "race_audio_loopback_test_check"

typedef struct {
    uint8_t channel_id;
    uint16_t test_interval; //unit: 1ms
    uint16_t checking_interval; //unit: 1ms
    bool print_checking_log;
    bool first_checking_skipped;
    int32_t THD_N;
    int32_t SNP;
    int32_t TOTAL_POW;
    TimerHandle_t test_timer;
    TimerHandle_t check_timer;
#if defined (MTK_AUDIO_TRANSMITTER_ENABLE)
    audio_transmitter_id_t test_id;
#endif
} race_audio_loopback_test_context_t;

static race_audio_loopback_test_context_t g_race_audio_loopback_test_context = {0};

static uint8_t race_cmd_audio_loopback_test_change_time_cmd(race_audio_loopback_test_change_time_cmd_t *pRaceCmd)
{
    if (pRaceCmd->test_interval == 0) {
        RACE_LOG_MSGID_W("[race_audio_loopback_test] change_time_cmd, interval 0", 0);
        return RACE_AUDIO_LOOPBACK_TEST_RSP_FAIL;
    }

    g_race_audio_loopback_test_context.test_interval = pRaceCmd->test_interval * 100;
    g_race_audio_loopback_test_context.checking_interval = pRaceCmd->checking_interval;
    g_race_audio_loopback_test_context.print_checking_log = (bool) (pRaceCmd->is_print_checking_log);
    RACE_LOG_MSGID_I("[race_audio_loopback_test] change_time_cmd, test_interval %d ms, check interval %d ms, is_print %d", 3,
            g_race_audio_loopback_test_context.test_interval, g_race_audio_loopback_test_context.checking_interval, g_race_audio_loopback_test_context.print_checking_log);
    return RACE_AUDIO_LOOPBACK_TEST_RSP_SUCCESS;
}

#if defined (MTK_AUDIO_TRANSMITTER_ENABLE)static void race_cmd_audio_loopback_test_notify_test_result()
{
    uint8_t *pRacePkt = (uint8_t *)RACE_ClaimPacket(RACE_TYPE_NOTIFICATION, RACE_AUDIO_LOOPBACK_TEST_TEST_RESULT_NEW, sizeof(race_audio_loopback_test_result_t), g_race_audio_loopback_test_context.channel_id);

    RACE_LOG_MSGID_I("[race_audio_loopback_test] send_result channel %d, THD_N %d SNP %d TOTAL_POW %d", 4, g_race_audio_loopback_test_context.channel_id,
            g_race_audio_loopback_test_context.THD_N, g_race_audio_loopback_test_context.SNP, g_race_audio_loopback_test_context.TOTAL_POW);

    if(pRacePkt) {
        ((race_audio_loopback_test_result_t *)pRacePkt)->THD_N = g_race_audio_loopback_test_context.THD_N;
        ((race_audio_loopback_test_result_t *)pRacePkt)->SNP = g_race_audio_loopback_test_context.SNP;
        ((race_audio_loopback_test_result_t *)pRacePkt)->TOTAL_POW = g_race_audio_loopback_test_context.TOTAL_POW;
        race_flush_packet(pRacePkt, g_race_audio_loopback_test_context.channel_id);
    }
}

static void race_audio_loopback_test_set_result(int32_t THD_N, int32_t SNP, int32_t TOTAL_POW)
{
    g_race_audio_loopback_test_context.SNP = SNP;
    g_race_audio_loopback_test_context.THD_N = THD_N;
    g_race_audio_loopback_test_context.TOTAL_POW = TOTAL_POW;
}

static void race_cmd_audio_loopback_test_result_report(bool is_over)
{
    int32_t THD_N = 0;
    int32_t SNP = 0;
    int32_t TOTAL_POW = 0;

    n9_dsp_share_info_t *share_info = hal_audio_query_bt_audio_dl_share_info();

    if (share_info->next < 4) {
        RACE_LOG_MSGID_I("[race_audio_loopback_test] lib process not ready, skip flag %d", 1, share_info->next);
        return;
    }

    THD_N = (int32_t)(share_info->start_addr) / 32;
    SNP = (int32_t)(share_info->read_offset) / 32;
    TOTAL_POW = (int32_t)(share_info->write_offset) / 32;

    RACE_LOG_MSGID_I("[race_audio_loopback_test] get result THD_N %d SNP %d TOTAL_POW %d", 3, THD_N, SNP, TOTAL_POW);

/*
    if (g_race_audio_loopback_test_context.print_checking_log) {
        race_audio_loopback_test_set_result(THD_N, SNP, TOTAL_POW);
    } else {
        if (g_race_audio_loopback_test_context.first_checking_skipped) {
            if ((g_race_audio_loopback_test_context.TOTAL_POW == 0) || ((g_race_audio_loopback_test_context.TOTAL_POW - TOTAL_POW) < 0)) {
                race_audio_loopback_test_set_result(THD_N, SNP, TOTAL_POW);
            }
        } else {
            g_race_audio_loopback_test_context.first_checking_skipped = true;
            if (is_over) {
                race_audio_loopback_test_set_result(THD_N, SNP, TOTAL_POW);
            }
        }
    }
*/
    if ((g_race_audio_loopback_test_context.TOTAL_POW == RACE_AUDIO_LOOPBACK_TEST_POW_INIT) || ((g_race_audio_loopback_test_context.TOTAL_POW - TOTAL_POW) < 0)) {
        race_audio_loopback_test_set_result(THD_N, SNP, TOTAL_POW);
    }

    if(is_over || g_race_audio_loopback_test_context.print_checking_log) {
        race_cmd_audio_loopback_test_notify_test_result();
    }
}


static void race_audio_loopback_test_test_timer_callback(TimerHandle_t xTimer)
{
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    audio_transmitter_status_t status;

    RACE_LOG_MSGID_I("[race_audio_loopback_test] test_cmd, test end", 0);

    xTimerStopFromISR(g_race_audio_loopback_test_context.check_timer, &pxHigherPriorityTaskWoken);

    if (pxHigherPriorityTaskWoken != pdFALSE) {
        portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    }

    xTimerDelete(g_race_audio_loopback_test_context.check_timer, 0);
    xTimerDelete(g_race_audio_loopback_test_context.test_timer, 0);

#if defined (MTK_AUDIO_TRANSMITTER_ENABLE)
    status = audio_transmitter_stop(g_race_audio_loopback_test_context.test_id);
    RACE_LOG_MSGID_I("[race_audio_loopback_test] audio_transmitter_stop ret %d", 1, status);
#endif

}

static void race_audio_loopback_test_check_timer_callback(TimerHandle_t xTimer)
{
    RACE_LOG_MSGID_I("[race_audio_loopback_test] test_cmd, check interval", 0);
    race_cmd_audio_loopback_test_result_report(false);
}

static void race_audio_loopback_test_transmitter_rcv_callback(audio_transmitter_event_t event, void *data, void *user_data)
{
    switch (event) {
        case AUDIO_TRANSMITTER_EVENT_START_SUCCESS:
            RACE_LOG_MSGID_I("[race_audio_loopback_test] AUDIO_TRANSMITTER_EVENT_START_SUCCESS", 0);
            break;
        case AUDIO_TRANSMITTER_EVENT_START_FAIL:
            RACE_LOG_MSGID_I("[race_audio_loopback_test] AUDIO_TRANSMITTER_EVENT_START_FAIL", 0);
            break;
        case AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS:
            RACE_LOG_MSGID_I("[race_audio_loopback_test] AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS", 0);
            if (g_race_audio_loopback_test_context.test_timer) {
                audio_transmitter_status_t status = audio_transmitter_deinit(g_race_audio_loopback_test_context.test_id);
                RACE_LOG_MSGID_I("[race_audio_loopback_test] audio_transmitter_deinit ret %d", 1, status);
                race_cmd_audio_loopback_test_result_report(true);
                memset(&g_race_audio_loopback_test_context, 0, sizeof(race_audio_loopback_test_context_t));
            }
            break;
        default:
            break;
    }
}

static uint8_t race_cmd_audio_loopback_test_start_test(hal_audio_device_t device, hal_audio_interface_t interface)
{
    bt_sink_srv_am_feature_t feature_param = {0, {{0}}};
    feature_param.type_mask = AM_AUDIO_LOOPBACK;
    am_audio_set_feature(FEATURE_NO_NEED_ID,&feature_param);

    audio_transmitter_status_t status;
    audio_transmitter_config_t config;

    config.scenario_type = AUDIO_TRANSMITTER_TEST;
    config.scenario_sub_id = AUDIO_TRANSMITTER_TEST_AUDIO_LOOPBACK;
    config.msg_handler = race_audio_loopback_test_transmitter_rcv_callback;
    config.user_data = (void *)0;
    config.scenario_config.audio_transmitter_test_config.audio_loopback_test_config.audio_device = device;
    config.scenario_config.audio_transmitter_test_config.audio_loopback_test_config.audio_interface = interface;

    RACE_LOG_MSGID_I("[race_audio_loopback_test] start test ,device %d, interface %d", 2, device, interface);

    audio_transmitter_id_t id = audio_transmitter_init(&config);
    if (id < 0) {
        RACE_LOG_MSGID_E("[race_audio_loopback_test] audio_transmitter_init fail", 0);
        return RACE_AUDIO_LOOPBACK_TEST_RSP_FAIL;
    }

    status = audio_transmitter_start(id);
    if (status != AUDIO_TRANSMITTER_STATUS_SUCCESS) {
        RACE_LOG_MSGID_E("[race_audio_loopback_test] audio_transmitter_start fail", 0);
        return RACE_AUDIO_LOOPBACK_TEST_RSP_FAIL;
    }

    RACE_LOG_MSGID_I("[race_audio_loopback_test] test started, id %x", 1, id);

    g_race_audio_loopback_test_context.test_id = id;
    g_race_audio_loopback_test_context.TOTAL_POW = RACE_AUDIO_LOOPBACK_TEST_POW_INIT;
    return RACE_AUDIO_LOOPBACK_TEST_RSP_SUCCESS;
}
#endif

static uint8_t race_cmd_audio_loopback_test_test_cmd(race_audio_loopback_test_test_cmd_t *pRaceCmd, uint8_t channelId)
{
#if defined (MTK_AUDIO_TRANSMITTER_ENABLE)
    uint8_t status = RACE_AUDIO_LOOPBACK_TEST_RSP_FAIL;

    if (g_race_audio_loopback_test_context.test_interval == 0) {
        RACE_LOG_MSGID_W("[race_audio_loopback_test] test_cmd, interval 0", 0);
        return RACE_AUDIO_LOOPBACK_TEST_RSP_FAIL;
    }
    if (g_race_audio_loopback_test_context.test_timer) {
        RACE_LOG_MSGID_W("[race_audio_loopback_test] test_cmd, test ongoing", 0);
        return RACE_AUDIO_LOOPBACK_TEST_RSP_FAIL;
    }

    g_race_audio_loopback_test_context.channel_id = channelId;
    RACE_LOG_MSGID_I("[race_audio_loopback_test] test_cmd, mode %x, channel %x", 2, pRaceCmd->mode, channelId);

    switch (pRaceCmd->mode) {
        case RACE_AUDIO_LOOPBACK_TEST_MODE_ADC0:
            status = race_cmd_audio_loopback_test_start_test(HAL_AUDIO_DEVICE_MAIN_MIC_L, HAL_AUDIO_INTERFACE_1);
            break;
        case RACE_AUDIO_LOOPBACK_TEST_MODE_ADC1:
            status = race_cmd_audio_loopback_test_start_test(HAL_AUDIO_DEVICE_MAIN_MIC_R, HAL_AUDIO_INTERFACE_1);
            break;
        case RACE_AUDIO_LOOPBACK_TEST_MODE_ADC2:
            status = race_cmd_audio_loopback_test_start_test(HAL_AUDIO_DEVICE_MAIN_MIC_L, HAL_AUDIO_INTERFACE_2);
            break;
        case RACE_AUDIO_LOOPBACK_TEST_MODE_ADC3:
            status = race_cmd_audio_loopback_test_start_test(HAL_AUDIO_DEVICE_MAIN_MIC_R, HAL_AUDIO_INTERFACE_2);
            break;
        case RACE_AUDIO_LOOPBACK_TEST_MODE_ADC4:
            status = race_cmd_audio_loopback_test_start_test(HAL_AUDIO_DEVICE_MAIN_MIC_L, HAL_AUDIO_INTERFACE_3);
            break;
        case RACE_AUDIO_LOOPBACK_TEST_MODE_ADC5:
            status = race_cmd_audio_loopback_test_start_test(HAL_AUDIO_DEVICE_MAIN_MIC_R, HAL_AUDIO_INTERFACE_3);
            break;
        default:
            break;
    }

    if (status != RACE_AUDIO_LOOPBACK_TEST_RSP_SUCCESS) {
        return status;
    }

    g_race_audio_loopback_test_context.test_timer = xTimerCreate(RACE_AUDIO_LOOPBACK_TEST_TIMER_TEST,
                                                        (g_race_audio_loopback_test_context.test_interval / portTICK_PERIOD_MS),
                                                        pdFALSE,
                                                        0,
                                                        race_audio_loopback_test_test_timer_callback);

    g_race_audio_loopback_test_context.check_timer = xTimerCreate(RACE_AUDIO_LOOPBACK_TEST_TIMER_CHECK,
                                                         (g_race_audio_loopback_test_context.checking_interval / portTICK_PERIOD_MS),
                                                         pdTRUE,
                                                         0,
                                                         race_audio_loopback_test_check_timer_callback);

    if (g_race_audio_loopback_test_context.test_timer == NULL || g_race_audio_loopback_test_context.check_timer == NULL) {
        RACE_LOG_MSGID_E("[race_audio_loopback_test] test_cmd timer create fail", 0);
        return RACE_AUDIO_LOOPBACK_TEST_RSP_FAIL;
    }

    RACE_LOG_MSGID_I("[race_audio_loopback_test] test_cmd mode %d, test timer %x, check timer %x", 3,
            pRaceCmd->mode, g_race_audio_loopback_test_context.test_timer, g_race_audio_loopback_test_context.check_timer);

    if((xTimerStart(g_race_audio_loopback_test_context.test_timer, 0) != pdPASS) || (xTimerStart(g_race_audio_loopback_test_context.check_timer, 0) != pdPASS)) {
        RACE_LOG_MSGID_E("[race_audio_loopback_test] test_cmd timer start fail", 0);
        return RACE_AUDIO_LOOPBACK_TEST_RSP_FAIL;
    }

    return RACE_AUDIO_LOOPBACK_TEST_RSP_SUCCESS;
#else
    return RACE_AUDIO_LOOPBACK_TEST_RSP_FAIL;
#endif
}



/**************************************************************************************************
* Public Functions (Handler)
**************************************************************************************************/

void* RACE_CmdHandler_audio_loopback_test(ptr_race_pkt_t pCmdMsg, uint16_t Length, uint8_t channel_id)
{
    uint8_t* pAudioRsp = NULL;
    uint16_t raceCmdId = pCmdMsg->hdr.id;
    uint8_t rsp = RACE_AUDIO_LOOPBACK_TEST_RSP_FAIL;

    switch(raceCmdId)
    {
        case RACE_AUDIO_LOOPBACK_TEST_TEST_CMD:
            RACE_LOG_MSGID_I("[race_audio_loopback_test] receive test_cmd", 0);
            rsp = race_cmd_audio_loopback_test_test_cmd((race_audio_loopback_test_test_cmd_t *)pCmdMsg, channel_id);
            break;
        case RACE_AUDIO_LOOPBACK_TEST_CHANGE_TIME_CMD:
            RACE_LOG_MSGID_I("[race_audio_loopback_test] receive change_time_cmd", 0);
            rsp = race_cmd_audio_loopback_test_change_time_cmd((race_audio_loopback_test_change_time_cmd_t *)pCmdMsg);
            break;
        default:
            break;
    }

    if(pCmdMsg->hdr.type == RACE_TYPE_COMMAND && (pAudioRsp = RACE_ClaimPacket(RACE_TYPE_RESPONSE, raceCmdId, RACE_AUDIO_LOOPBACK_TEST_RSP_LEN, channel_id)) != NULL)
    {
        (*pAudioRsp) = rsp;
    }
    return pAudioRsp;
}

#endif
