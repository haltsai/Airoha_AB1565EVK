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
#ifdef RACE_CAPTOUCH_CMD_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "syslog.h"
#include "hal.h"
#include "race_util.h"
#include "race_xport.h"
#include "race_lpcomm_aws.h"
#include "race_cmd_bluetooth.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_trans.h"
#include "race_lpcomm_conn.h"
#include "race_lpcomm_msg_struct.h"
#include "race_noti.h"
#include "race_lpcomm_ps_noti.h"
#include "race_fota_util.h"
#include "hal_captouch_internal.h"
#include "ept_keypad_drv.h"
#include "race_cmd_captouch.h"
#include "timers.h"


////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//static void race_captouch_real_data_callback(TimerHandle_t xTimer);
static void race_captouch_multi_channel_real_data_callback(TimerHandle_t xTimer);

typedef struct {
    TimerHandle_t timer_handle;
    uint8_t channel;
    uint8_t channel_id;
    race_pkt_t cmdMsg;
    uint16_t race_cmd;
    void (*callback)(TimerHandle_t);
}race_captouch_t;

//#define SINGLE_CHANNEL  0
#define MULTI_CHANNEL   0
static race_captouch_t race_captouch[1] = {
    {.race_cmd = RACE_CAPTOUCH_CTRL_MULTI_CHANNEL_REAL_DATA,
     .callback = race_captouch_multi_channel_real_data_callback},
};
//////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifndef HAL_CPT_FEATURE_4CH
static void *race_captouch_get_setting(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        uint8_t status;
        uint8_t valid_ch;
        uint8_t en_ch;
        uint8_t mavg_r;
        uint8_t avg_s;
        uint8_t coarse_cap[8];
        int16_t thr_h[8];
        int16_t thr_l[8];
    } PACKED RSP;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                      (uint16_t)RACE_CAPTOUCH_GET_SETTING,
                                      (uint16_t)sizeof(RSP),
                                      channel_id);
    uint16_t rdata, i;

    RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);

    if (pEvt) {

        pEvt->valid_ch = DRV_KBD_CAPTOUCH_SEL;
        rdata = captouch_analog_read_data(TOUCH_CON0);
        pEvt->en_ch    = (rdata >> 8) & 0xff;
        rdata = captouch_analog_read_data(TOUCH_CON1);
        pEvt->mavg_r = (rdata >> 3) & 0xf;
        pEvt->avg_s  =  rdata & 0x7;

        for (i = 0; i < 8; i++) {
            if (pEvt->valid_ch & (1 << i)) {
                pEvt->coarse_cap[i] = captouch_get_coarse_cap(i);
                pEvt->thr_h[i] = captouch_9signed_to_16signed(captouch_analog_read_data(CAPTOUCH_ANALOG.THR_H[i]));
                pEvt->thr_l[i] = captouch_9signed_to_16signed(captouch_analog_read_data(CAPTOUCH_ANALOG.THR_L[i]));
            } else {
                pEvt->coarse_cap[i] = 0;
                pEvt->thr_h[i]      = 0;
                pEvt->thr_l[i]      = 0;
            }

        }
        pEvt->status = RACE_ERRCODE_SUCCESS;
    }

    return pEvt;
}

static void *race_captouch_set_settting(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t valid_ch;
        uint8_t mavg_r;
        uint8_t avg_s;
        uint8_t coarse_cap[8];
        int16_t thr_h[8];
        int16_t thr_l[8];
    } PACKED CMD;

    typedef struct {
        uint8_t status;
    } PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                      (uint16_t)RACE_CAPTOUCH_SET_SETTING,
                                      (uint16_t)sizeof(RSP),
                                      channel_id);
    uint16_t wdata, i;
    RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);

    if (pEvt) {
        for (i = 0; i < 8; i++) {
            hal_captouch_channel_disable(i);
        }

        wdata = captouch_analog_read_data(TOUCH_CON1);
        wdata &= ~0x7f;
        wdata |= (pCmd->mavg_r << 3) | (pCmd->avg_s);
        captouch_analog_write_data(TOUCH_CON1, wdata);


        for (i = 0; i < 8; i++) {
            if (pCmd->valid_ch & (1 << i)) {
                //captouch_set_coarse_cap(i,pCmd->coarse_cap[i]);
                //captouch_set_threshold(i,pCmd->thr_h[i],pCmd->thr_l[i]);
                hal_captouch_set_coarse_cap(i, (uint32_t)pCmd->coarse_cap[i]);
                hal_captouch_set_threshold(i, (int32_t)pCmd->thr_h[i], (int32_t)pCmd->thr_l[i]);
                hal_captouch_channel_enable(i);
            }
        }

        pEvt->status = RACE_ERRCODE_SUCCESS;
    }

    return pEvt;
}
#endif
#if 0
static void race_captouch_real_data_callback(TimerHandle_t xTimer)
{
    int16_t  avg_adc;
    int32_t  ret;
    race_captouch_t *race_cap = &race_captouch[SINGLE_CHANNEL];
    hal_captouch_tune_data_t data;

    race_captouch_noti_real_data_t *noti;

    captouch_get_tune_state(race_cap->channel, &data);
    avg_adc  = data.avg_adc;

    noti = RACE_ClaimPacketAppID(race_cap->cmdMsg.hdr.pktId.field.app_id,
                                 RACE_TYPE_NOTIFICATION,
                                 race_cap->race_cmd,
                                 sizeof(race_captouch_noti_real_data_t),
                                 race_cap->channel_id);
    if (noti) {
        noti->status  = RACE_ERRCODE_SUCCESS;
        noti->avg_data = avg_adc;

        ret = race_noti_send((void *)noti, race_cap->channel_id, false);
        if (RACE_ERRCODE_SUCCESS != ret) {
            RACE_FreePacket((void *)noti);
            noti = NULL;
        }
    } else {

        RACE_LOG_MSGID_I("race_captouch_real_data_callback can't allocate memory ", 0);
    }
}
#endif
static void race_captouch_multi_channel_real_data_callback(TimerHandle_t xTimer)
{
    uint32_t i,time_current;
    hal_captouch_tune_data_t data;

    race_captouch_t *race_cap = &race_captouch[MULTI_CHANNEL];

    race_captouch_noti_multi_channel_real_data_t *noti;

    noti = RACE_ClaimPacketAppID(race_cap->cmdMsg.hdr.pktId.field.app_id,
                                 RACE_TYPE_NOTIFICATION,
                                 race_cap->race_cmd,
                                 sizeof(race_captouch_noti_multi_channel_real_data_t),
                                 race_cap->channel_id);
    if (noti == NULL) {
        RACE_LOG_MSGID_I("race_captouch_real_data_callback can't allocate memory ", 0);
        return;
    }

    for (i = 0; i < HAL_CAPTOUCH_CHANNEL_MAX; i++) {
        if (race_cap->channel & (1 << i)) {
            captouch_get_tune_state(i,&data);
            noti->avg_data[i]        = data.avg_adc;
            noti->vadc_data[i]       = data.vadc;
            noti->cal_data[i]        = data.fine_cap;
#ifdef HAL_CPT_FEATURE_4CH
            noti->ear_detect_base[i] = data.ear_detect_base;
            noti->mavg_adc_dbg[i]    = data.mavg_dbg;
            noti->vadc_debounce[i]   = data.adc_debounce;
            noti->ear_detect_data[i] = data.ear_detect_data;
            noti->noise_cnt_race[i]  = data.noise_cnt_race;
#endif
        } else {
            noti->avg_data[i]        = 0;
            noti->vadc_data[i]       = 0;
            noti->cal_data[i]        = 0;
#ifdef HAL_CPT_FEATURE_4CH
            noti->ear_detect_base[i] = 0;
            noti->mavg_adc_dbg[i]    = 0;
            noti->vadc_debounce[i]   = 0;
            noti->ear_detect_data[i] = 0;
            noti->noise_cnt_race[i]  = 0;
#endif
        }
    }
    //printf("captouch race timer");
    noti->status  = RACE_ERRCODE_SUCCESS;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &time_current);
    noti->time = time_current;
    if (RACE_ERRCODE_SUCCESS != race_noti_send((void *)noti, race_cap->channel_id, false)) {
        RACE_FreePacket((void *)noti);
        noti = NULL;
    }
}
static void *race_captouch_real_data(ptr_race_pkt_t pCmdMsg, uint8_t channel_id, race_captouch_t *race_cap)
{
    typedef struct {
        uint8_t status;
    } PACKED RSP;

    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t real_time_en;
        uint8_t real_time_ch;
        uint16_t real_time_time;
    } PACKED CMD;

    RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);

    CMD *pCmd = (CMD *)pCmdMsg;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                      race_cap->race_cmd,
                                      (uint16_t)sizeof(RSP),
                                      channel_id);

    if (pEvt) {
        race_cap->channel = pCmd->real_time_ch;
        race_cap->channel_id = channel_id;

        RACE_LOG_MSGID_I("real data: en=%d,ch=%d,time=%d,ch_id=%d\r\n",4, \
                    pCmd->real_time_en, race_cap->channel, pCmd->real_time_time, race_cap->channel_id);

        if ( pCmd->real_time_en) {

            memcpy(&race_cap->cmdMsg, pCmdMsg, sizeof(race_pkt_t));

            if (race_cap->timer_handle == NULL) {
                race_cap->timer_handle = xTimerCreate(NULL, \
                                             pCmd->real_time_time / portTICK_PERIOD_MS , \
                                             pdTRUE, NULL, \
                                             race_cap->callback);
            }

            xTimerStart(race_cap->timer_handle, 0);

        } else {
            if (race_cap->timer_handle != NULL) {
                xTimerStop(race_cap->timer_handle, 0);
                xTimerDelete(race_cap->timer_handle, 0);
            }
            race_cap->timer_handle = NULL;
        }

        pEvt->status = RACE_ERRCODE_SUCCESS;
    }

    return pEvt;
}

static void *race_captouch_coarse_cap_calibration(ptr_race_pkt_t pCmdMsg, uint8_t channel_id, race_captouch_t *race_cap)
{
    UNUSED(race_cap);

    uint8_t i;

    typedef struct
    {
        uint8_t status;
        hal_captouch_cal_result_t cal_result[4];
    } PACKED THIS_EVT_HDR_STRU, *PTR_THIS_EVT_HDR_STRU;

    PTR_THIS_EVT_HDR_STRU pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                      RACE_CAPTOUCH_COARSE_CAP_CALIBRATION,
                                      (uint16_t)sizeof(THIS_EVT_HDR_STRU),
                                      channel_id);

    hal_captouch_coarse_cal_t coarse_cal_result;
    if (pEvt != NULL)
    {
        hal_captouch_coarse_cap_tune(&coarse_cal_result);
        pEvt->status = RACE_ERRCODE_SUCCESS;
        for(i=0;i<4;i++)
        {
            pEvt->cal_result[i].result = coarse_cal_result.coarse_cal[i].result;
            pEvt->cal_result[i].coarse_value = coarse_cal_result.coarse_cal[i].coarse_value;
            //printf("captouch race cmd %d %d", pEvt->cal_result[i].result, pEvt->cal_result[i].coarse_value);
        }
    }
    return pEvt;
}

static void *race_captouch_set_auto_tune_feature(ptr_race_pkt_t pCmdMsg, uint8_t channel_id, race_captouch_t *race_cap)
{
    UNUSED(race_cap);
    hal_captouch_tune_data_t data;
    
    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t en;
    } PACKED *PTR_THIS_CMD_HDR_STRU;

    typedef struct
    {
        uint8_t status;
        uint8_t en;
    } PACKED THIS_EVT_HDR_STRU, *PTR_THIS_EVT_HDR_STRU;

    PTR_THIS_CMD_HDR_STRU pCmd = (PTR_THIS_CMD_HDR_STRU)pCmdMsg;
    PTR_THIS_EVT_HDR_STRU pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                      RACE_CAPTOUCH_SET_AUTO_TUNE_FEATURE,
                                      (uint16_t)sizeof(THIS_EVT_HDR_STRU),
                                      channel_id);
    if (pEvt != NULL)
    {
        captouch_get_tune_state(0,&data);
        pEvt->en = captouch_set_auto_tune_feature(pCmd->en);
        pEvt->status = RACE_ERRCODE_SUCCESS;
        //printf("captouch race pCmd->en:%d, pEvt->en:%d", pCmd->en, pEvt->en);
    }
    return pEvt;
}

void *RACE_CmdHandler_captouch(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id)
{
    void *ptr = NULL;

    RACE_LOG_MSGID_I("RACE_CmdHandler_captouch, type[0x%X], race_id[0x%X], channel_id[%d]", 3,
        pRaceHeaderCmd->hdr.type, pRaceHeaderCmd->hdr.id, channel_id);

    if (pRaceHeaderCmd->hdr.type == RACE_TYPE_COMMAND) {
        switch (pRaceHeaderCmd->hdr.id) {
            case RACE_CAPTOUCH_GET_SETTING : {
                #ifndef HAL_CPT_FEATURE_4CH
                ptr = race_captouch_get_setting(pRaceHeaderCmd, channel_id);
                //printf("captouch race_captouch_get_setting");
                #endif
            }
            break;

            case RACE_CAPTOUCH_SET_SETTING : {
                #ifndef HAL_CPT_FEATURE_4CH
                ptr = race_captouch_set_settting(pRaceHeaderCmd, channel_id);
                //printf("captouch race_captouch_set_setting");
                #endif
            }
            break;

            case RACE_CAPTOUCH_COARSE_CAP_CALIBRATION: {
                ptr = race_captouch_coarse_cap_calibration(pRaceHeaderCmd, channel_id, NULL);
            }
            break;

            case RACE_CAPTOUCH_SET_AUTO_TUNE_FEATURE: {
                ptr = race_captouch_set_auto_tune_feature(pRaceHeaderCmd, channel_id, NULL);
            }
            break;

            case RACE_CAPTOUCH_CTRL_MULTI_CHANNEL_REAL_DATA: {
                ptr = race_captouch_real_data(pRaceHeaderCmd, channel_id, &race_captouch[MULTI_CHANNEL]);
            }
            break;

            default: {
                break;
            }
        }
    }

    return ptr;
}

#endif /* RACE_CAPTOUCH_CMD_ENABLE */

