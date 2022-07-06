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

#include <string.h>
#include <assert.h>

#include "anc_monitor.h"
#include "bt_sink_srv_ami.h"
#include "anc_control_api.h"
#include "hal_audio_cm4_dsp_message.h"
#include "hal_resource_assignment.h"
#include "semphr.h"

#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
#include "dsp_para_environment_detection.h"
#endif

#if defined(MTK_ANC_SURROUND_MONITOR_ENABLE)
// ======== Definition ========

#ifdef ADAPIVE_ANC_STREAM_CONTROL
extern bool g_adaptive_ANC_stream_MP_control_flag;
extern bool g_adaptive_ANC_stream_suspend_control_flag;
uint8_t g_uu_enable = 0;
uint8_t g_wnd_enable = 0;
uint8_t g_ed_enable = 0;
#endif
bool g_adaptive_ANC_suspend_by_ANC = false;
static SemaphoreHandle_t g_adaptive_ANC_semaphore = NULL;
bool g_adaptive_ANC_getinfo_DSP_write_done = false;


// ======== Static Structure ========
uint32_t anc_monitor_message_date;
audio_transmitter_monitor_config_t anc_monitor_transmitter;


// ======== Function Declaration ========



// ======== Private utility functions ========

void audio_anc_monitor_transmitter_send_ccni(uint16_t nvkey_id)
{
    uint32_t nvkey_length = 0;
    void *malloc_ptr = NULL;
    void *p_param_share;
    sysram_status_t nvdm_status;

    nvdm_status =  flash_memory_query_nvdm_data_length(nvkey_id, &nvkey_length);
    if (nvdm_status || !nvkey_length) {
        TRANSMITTER_LOG_E("[ANC_Monitor] Read Nvkey length Fail id:0x%x, status:%d ", 2, nvkey_id, nvdm_status);
    }
    malloc_ptr = pvPortMallocNC(nvkey_length);
    if (malloc_ptr) {
        nvdm_status = flash_memory_read_nvdm_data(nvkey_id, (uint8_t *)malloc_ptr, &nvkey_length);
        if (nvdm_status || !nvkey_length) {
            TRANSMITTER_LOG_E("[ANC_Monitor] Read Nvkey data Fail id:0x%x, status:%d ", 2, nvkey_id, nvdm_status);
        }
        p_param_share = hal_audio_dsp_controller_put_paramter(malloc_ptr, nvkey_length, AUDIO_MESSAGE_TYPE_COMMON);
        vPortFreeNC(malloc_ptr);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_ALGORITHM_PARAM, nvkey_id, (uint32_t)p_param_share, true);
    } else {
        TRANSMITTER_LOG_E("[ANC_Monitor] malloc Fail", 0);
        assert(0);
    }
}


void audio_anc_monitor_transmitter_set_parameters(void)
{
    //Wind detect
#ifdef AIR_ANC_WIND_DETECTION_ENABLE
    audio_anc_monitor_transmitter_send_ccni(NVKEY_DSP_PARA_WIND_DETECT);
#endif

    //User unaware
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
    audio_anc_monitor_transmitter_send_ccni(NVKEY_DSP_PARA_USR_UNAWARE);
#endif
    //Environment Detection
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
    audio_anc_monitor_transmitter_send_ccni(NVKEY_DSP_PARA_ENVIRONMENT_DETECTION);
#endif

}

static void audio_anc_monitor_transmitter_receive_callback(audio_transmitter_event_t event, void *data, void *user_data)
{
    (void)(event);
    (void)(data);
    (void)(user_data);
     switch (event) {
        case AUDIO_TRANSMITTER_EVENT_START_SUCCESS:
            //Load Nvkey
            audio_anc_monitor_transmitter_set_parameters();
            break;
        case AUDIO_TRANSMITTER_EVENT_START_FAIL:
            if (anc_monitor_transmitter.is_enable) {
                audio_transmitter_start(anc_monitor_transmitter.receive_id);
            }
            break;
        default:
            break;
    }
}


// ======== Public functions ========
void audio_anc_monitor_anc_init(void)
{
    audio_transmitter_config_t config;
    config.scenario_type = AUDIO_TRANSMITTER_ANC_MONITOR_STREAM;
    config.scenario_sub_id = 0;
    config.msg_handler = audio_anc_monitor_transmitter_receive_callback;
    config.user_data = NULL;
    anc_monitor_transmitter.receive_id = audio_transmitter_init(&config);

    #ifdef ADAPIVE_ANC_STREAM_CONTROL
    //read adaptive ANC status from nvdm
    uint8_t wnd_enable;
    uint8_t uu_enable;
    uint8_t ed_enable;
    audio_anc_monitor_get_adaptive_anc_enable_status_from_nvdm(&wnd_enable, &uu_enable, &ed_enable);
    g_uu_enable = uu_enable;
    g_wnd_enable = wnd_enable;
    g_ed_enable = ed_enable;
    TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC] wnd %d, uu %d, ed %d", 3, g_wnd_enable, g_uu_enable, g_ed_enable);
    #endif
    if(g_adaptive_ANC_semaphore == NULL)
    {
        g_adaptive_ANC_semaphore = xSemaphoreCreateBinary();
        if(!g_adaptive_ANC_semaphore) {
            TRANSMITTER_LOG_E("audio_anc_monitor_anc_init create get_info_semaphore FAIL \n", 0);
        }else{
            xSemaphoreGive(g_adaptive_ANC_semaphore);
        }
    }
}

void audio_anc_monitor_anc_callback(audio_anc_control_event_t event_id, audio_anc_control_result_t result)
{
    audio_transmitter_status_t transmitter_receive_status;
    uint8_t anc_is_enabled;
    audio_anc_control_get_status(&anc_is_enabled, NULL, NULL, NULL, NULL, NULL);
    TRANSMITTER_LOG_I("[ANC_Monitor] ANC callback - event:%d, result:%d, dsp status:%d, ANC_suspend: %d, ANC_enable: %d", 5, event_id, result, anc_monitor_transmitter.is_enable, g_adaptive_ANC_suspend_by_ANC, anc_is_enabled);

    if((event_id == AUDIO_ANC_CONTROL_EVENT_RESUME_NOTIFY)){
        g_adaptive_ANC_suspend_by_ANC = false;
        if(!anc_is_enabled){
            return;
        }
    }else if((event_id == AUDIO_ANC_CONTROL_EVENT_SUSPEND_NOTIFY)){
        g_adaptive_ANC_suspend_by_ANC = true; //ANC is suspend, also suspend Adaptive ANC
    }

#ifdef ADAPIVE_ANC_STREAM_CONTROL
    if(((event_id == AUDIO_ANC_CONTROL_EVENT_ON)||(event_id == AUDIO_ANC_CONTROL_EVENT_RESUME_NOTIFY))&&(!g_uu_enable)&&(!g_ed_enable)&&(!g_wnd_enable)){
        TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC]all features are disabled, audio_anc_monitor_callback START FAIL", 0);
        return;
    }
    if((g_adaptive_ANC_stream_MP_control_flag == true)||(g_adaptive_ANC_stream_suspend_control_flag == true)){ //under MP mode or suspended
        TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC]suspended or under MP mode, audio_anc_monitor_callback START/STOP FAIL", 0);
    }else{ //
#endif

#ifdef MTK_ANC_V2
    if (result == AUDIO_ANC_CONTROL_EXECUTION_SUCCESS) {
        if (((event_id == AUDIO_ANC_CONTROL_EVENT_ON)||(event_id == AUDIO_ANC_CONTROL_EVENT_RESUME_NOTIFY)) && (!anc_monitor_transmitter.is_enable) && (!g_adaptive_ANC_suspend_by_ANC)) {
            anc_monitor_transmitter.is_enable = true;
            transmitter_receive_status = audio_transmitter_start(anc_monitor_transmitter.receive_id);
            if (transmitter_receive_status != AUDIO_TRANSMITTER_STATUS_SUCCESS) {
                TRANSMITTER_LOG_E("[ANC_Monitor]audio_anc_monitor_callback START FAIL", 0);
                anc_monitor_transmitter.is_enable = false;
            }
        } else if (((event_id == AUDIO_ANC_CONTROL_EVENT_OFF)||(event_id == AUDIO_ANC_CONTROL_EVENT_SUSPEND_NOTIFY)) && (anc_monitor_transmitter.is_enable)) {
            transmitter_receive_status = audio_transmitter_stop(anc_monitor_transmitter.receive_id);
            if (transmitter_receive_status == AUDIO_TRANSMITTER_STATUS_SUCCESS) {
                anc_monitor_transmitter.is_enable = false;
            } else {
                TRANSMITTER_LOG_E("[ANC_Monitor]audio_anc_monitor_callback STOP FAIL", 0);
            }
        } else {
            TRANSMITTER_LOG_E("[ANC_Monitor]audio_anc_monitor_callback START/STOP FAIL, event_id %d, is_enable %d, suspend_by_ANC %d", 3, event_id, anc_monitor_transmitter.is_enable, g_adaptive_ANC_suspend_by_ANC);
        }
    } else {
        TRANSMITTER_LOG_E("[ANC_Monitor]audio_anc_monitor_callback even Fail", 0);
    }
#endif

#ifdef ADAPIVE_ANC_STREAM_CONTROL
    }
#endif

}

void audio_anc_monitor_set_info(audio_anc_monitor_set_info_t set_type, uint32_t para)
{
    //send to AM
    bt_sink_srv_am_feature_t feature_para;
    memset(&feature_para, 0, sizeof(bt_sink_srv_am_feature_t));
    feature_para.type_mask = AM_ANC_MONITOR;
    feature_para.feature_param.anc_monitor_param.event = AUDIO_ANC_MONITOR_SET_INFO;
    feature_para.feature_param.anc_monitor_param.event_type = set_type;
    feature_para.feature_param.anc_monitor_param.param = para;
    am_audio_set_feature(FEATURE_NO_NEED_ID,&feature_para);
    TRANSMITTER_LOG_I("[ANC_Monitor]Set info, type:%d, para:%d", 2, set_type, para);
}

uint32_t audio_anc_monitor_get_info(audio_anc_monitor_get_info_t get_type)
{
    //send to AM
    uint32_t share_addr = (uint32_t)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_ANC_MONITOR_ADAPTIVE_ANC);
    bt_sink_srv_am_feature_t feature_para;
    memset(&feature_para, 0, sizeof(bt_sink_srv_am_feature_t));
    feature_para.type_mask = AM_ANC_MONITOR;
    feature_para.feature_param.anc_monitor_param.event = AUDIO_ANC_MONITOR_GET_INFO;
    feature_para.feature_param.anc_monitor_param.event_type = get_type;
    feature_para.feature_param.anc_monitor_param.param = share_addr; //addr
    am_audio_set_feature(FEATURE_NO_NEED_ID,&feature_para);
    TRANSMITTER_LOG_I("[ANC_Monitor]Get info, type:%d, share_addr 0x%x", 2, get_type, share_addr);

    return false;
}

#ifdef ADAPIVE_ANC_STREAM_CONTROL
void audio_anc_monitor_adaptive_anc_stream_get_status(uint8_t* enable)
{   if(enable != NULL){
        //*enable = g_adaptive_ANC_stream_suspend_control_flag;
        *enable = anc_monitor_transmitter.is_enable;
    }
}

void audio_anc_monitor_adaptive_anc_stream_suspend_resume(bool enable)
{
    uint8_t anc_is_enabled;
    audio_anc_control_get_status(&anc_is_enabled, NULL, NULL, NULL, NULL, NULL);
    TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC]adaptive_anc_stream_suspend_resume, enable %d, anc_enable %d", 2, enable, anc_is_enabled);
    if(enable){ //resume Adaptive ANC Stream
        if(anc_is_enabled){
            /* if ANC is ON, enable Adaptive ANC. */
            audio_anc_monitor_set_info(AUDIO_ANC_MONITOR_STREAM_CONTROL, TRUE);
            audio_anc_monitor_anc_callback(AUDIO_ANC_CONTROL_EVENT_ON, AUDIO_ANC_CONTROL_CB_LEVEL_ALL);
        }
     }else{ //suspend Adaptive ANC Stream
        if(anc_is_enabled){
            /* if ANC is ON, disable Adaptive ANC. */
            audio_anc_monitor_set_info(AUDIO_ANC_MONITOR_SET_USER_UNAWARE_ENABLE, FALSE);
            TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC]adaptive_anc_stream_suspend_resume, wait for suspend",0);
            hal_gpt_delay_ms(1500); //delay 1.5s for UU ramping to tight
            audio_anc_monitor_set_info(AUDIO_ANC_MONITOR_STREAM_CONTROL, FALSE);
            audio_anc_monitor_anc_callback(AUDIO_ANC_CONTROL_EVENT_OFF, AUDIO_ANC_CONTROL_CB_LEVEL_ALL);

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
            TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC]reset ANC extend gain, ret = %d", 1, anc_ret);
        }
        TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC]adaptive_anc_stream_suspend_resume, suspend done",0);
    }
}
#endif
#if defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
void audio_anc_monitor_get_adaptive_anc_enable_status_from_nvdm(uint8_t* wnd_enable,uint8_t* uu_enable,uint8_t* ed_enable)
{
    //WND
    if(wnd_enable != NULL){
        #ifdef AIR_ANC_WIND_DETECTION_ENABLE
        audio_wind_detection_get_enable_status_from_nvdm(wnd_enable);
        #else
        *wnd_enable = false;
        #endif
    }
    //UU
    if(uu_enable != NULL){
        #ifdef AIR_ANC_USER_UNAWARE_ENABLE
        audio_user_unaware_get_enable_status_from_nvdm(uu_enable);
        #else
        *uu_enable = false;
        #endif
    }
    //ED
    if(ed_enable != NULL){
        #ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
        audio_environment_detection_get_suspend_procedure_status_from_nvdm(ed_enable);
        #else
        *ed_enable = false;
        #endif
    }
}
#endif

#ifdef AIR_ANC_WIND_DETECTION_ENABLE
void audio_wind_detection_get_enable_status_from_nvdm(uint8_t* enable)
{
    //read WND status from nvdm
    typedef struct stru_dsp_wind_para_s
    {
        //For algorithm
        U8 ENABLE;                      /**< @Value   0x01 @Desc 1 */
        U8 REVISION;                    /**< @Value   0x01 @Desc 1 */
        S16 _reserved0;                 /**< @Value      0 @Desc 0 */

        S16 option;                     /**< @Value      1 @Desc 0 for regular det mode.; 1 for continuous det mode.*/
        S16 w_thr0;                     /**< @Value  20000 @Desc 0.7; smooth wind first thr. (0 ~ 32767)        */
        S16 w_thr1;                     /**< @Value  11000 @Desc 0.25; smooth wind sec thr.  (0 ~ 32767)        */
        S16 wind_sm_len;                /**< @Value     30 @Desc smooth len for wind. (1 ~ 30)                  */
        S16 egy_sm_len;                 /**< @Value     30 @Desc smooth len for energy. (1 ~ 30)                */
        S16 vad_leave;                  /**< @Value     80 @Desc how many fr does RNN predict for each decision */
        S32 c_thr0;                     /**< @Value  15000000 @Desc smooth energy first thr                        */
        S32 c_thr1;                     /**< @Value   8500 @Desc smooth energy sec thr                          */
        S32 c_thr2;                     /**< @Value   6500 @Desc smooth energy thrd thr                         */

        //For middleware
        U16 _reserved;                  /**< @Value     0  @Desc Set 1 to print detect result*/
        S16 attenuation;                /**< @Value -3000  @Desc attenuation value of 0.01db*/
        U16 attack_count;               /**< @Value    30  @Desc attack counter for wind detection, unit:15ms */
        U16 release_count;              /**< @Value  2000  @Desc release counter for non-wind detection, unit:15ms */

    } PACKED wind_detection_para_t;
    sysram_status_t status;
    uint32_t wnd_tableSize = 0;
    uint32_t wnd_buff_size = sizeof(wind_detection_para_t);
    wind_detection_para_t *pWNDpara = (wind_detection_para_t*)pvPortMalloc(wnd_buff_size);
    if (pWNDpara){
        status = flash_memory_query_nvdm_data_length(NVKEY_DSP_PARA_WIND_DETECT, &wnd_tableSize);
        if(status || !wnd_tableSize){
            TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][Wind]query_nvdm_data_length err, status %d, size %d", 2, status, wnd_tableSize);
        }
        if(wnd_tableSize != wnd_buff_size){
            TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][Wind]query_nvdm_data_length err, query_size %d, buff_size", 2, wnd_tableSize, wnd_buff_size);
        }
        status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_WIND_DETECT, (uint8_t *)pWNDpara, &wnd_tableSize);
        if (status != NVDM_STATUS_NAT_OK) {
            TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][Wind]read_nvdm_data err, ID 0x%x, status %d", 2, NVKEY_DSP_PARA_WIND_DETECT, status);
            if(enable != NULL){
                *enable = false;
            }
        }else{
            if(enable != NULL){
                *enable = pWNDpara->ENABLE;
            }
            TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][Wind]wind_detection_get_enable_status_from_nvdm, enable %d", 1, *enable);
        }
        vPortFree(pWNDpara);
    }else {
        TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][Wind] malloc Fail", 0);
        assert(0);
    }
}

void audio_wind_detection_store_enable_status_to_nvdm(bool enable)
{
    //WND nvkey structure
    typedef struct stru_dsp_wind_para_s
    {
        //For algorithm
        U8 ENABLE;                      /**< @Value   0x01 @Desc 1 */
        U8 REVISION;                    /**< @Value   0x01 @Desc 1 */
        S16 _reserved0;                 /**< @Value      0 @Desc 0 */

        S16 option;                     /**< @Value      1 @Desc 0 for regular det mode.; 1 for continuous det mode.*/
        S16 w_thr0;                     /**< @Value  20000 @Desc 0.7; smooth wind first thr. (0 ~ 32767)        */
        S16 w_thr1;                     /**< @Value  11000 @Desc 0.25; smooth wind sec thr.  (0 ~ 32767)        */
        S16 wind_sm_len;                /**< @Value     30 @Desc smooth len for wind. (1 ~ 30)                  */
        S16 egy_sm_len;                 /**< @Value     30 @Desc smooth len for energy. (1 ~ 30)                */
        S16 vad_leave;                  /**< @Value     80 @Desc how many fr does RNN predict for each decision */
        S32 c_thr0;                     /**< @Value  15000000 @Desc smooth energy first thr                        */
        S32 c_thr1;                     /**< @Value   8500 @Desc smooth energy sec thr                          */
        S32 c_thr2;                     /**< @Value   6500 @Desc smooth energy thrd thr                         */

        //For middleware
        U16 _reserved;                  /**< @Value     0  @Desc Set 1 to print detect result*/
        S16 attenuation;                /**< @Value -3000  @Desc attenuation value of 0.01db*/
        U16 attack_count;               /**< @Value    30  @Desc attack counter for wind detection, unit:15ms */
        U16 release_count;              /**< @Value  2000  @Desc release counter for non-wind detection, unit:15ms */
    } PACKED wind_detection_para_t;
    sysram_status_t status;
    uint32_t wnd_tableSize = 0;
    uint32_t wnd_buff_size = sizeof(wind_detection_para_t);
    wind_detection_para_t *pWNDpara = (wind_detection_para_t*)pvPortMalloc(wnd_buff_size);
    if (pWNDpara){
        status = flash_memory_query_nvdm_data_length(NVKEY_DSP_PARA_WIND_DETECT, &wnd_tableSize);
        if(status || !wnd_tableSize){
            TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][Wind]query_nvdm_data_length err, status %d, size %d", 2, status, wnd_tableSize);
        }
        if(wnd_tableSize != wnd_buff_size){
            TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][Wind]query_nvdm_data_length err, query_size %d, buff_size", 2, wnd_tableSize, wnd_buff_size);
        }
        status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_WIND_DETECT, (uint8_t *)pWNDpara, &wnd_tableSize);
        if (status != NVDM_STATUS_NAT_OK) {
            TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][Wind]read_nvdm_data err, ID 0x%x, status %d", 2, NVKEY_DSP_PARA_WIND_DETECT, status);
        }

        pWNDpara->ENABLE = (uint8_t)enable;

        status = flash_memory_write_nvdm_data(NVKEY_DSP_PARA_WIND_DETECT, (uint8_t *)pWNDpara, wnd_buff_size);
        if (status) {
            TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][Wind]Store Write Nvkey data Fail id:0x%x, status:%d ", 2, NVKEY_DSP_PARA_WIND_DETECT, status);
        }else{
            TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][Wind]wind_detection_store_enable_status_to_nvdm, enable %d", 1, enable);
        }
        vPortFree(pWNDpara);
    }else {
        TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][Wind] malloc Fail", 0);
        assert(0);
    }
}


void audio_wind_detection_enable(bool enable)
{
    TRANSMITTER_LOG_I("[ANC_Monitor][Wind]wind_detection_enable %d", 1, enable);
    audio_wind_detection_store_enable_status_to_nvdm(enable);
    if(enable){
        g_wnd_enable = true;
        TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][Wind]resume WND. wnd %d, uu %d, ed %d\n", 3, g_wnd_enable, g_uu_enable, g_ed_enable);
        //check if first enable
        if((!g_uu_enable)&&(!g_ed_enable)&&(g_wnd_enable)){
            TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][Wind]first enable.\n", 0);
            audio_anc_monitor_adaptive_anc_stream_suspend_resume(TRUE);
        }
    }else{
        g_wnd_enable = false;
        TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][Wind]suspend WND. wnd %d, uu %d, ed %d\n", 3, g_wnd_enable, g_uu_enable, g_ed_enable);
        //check if last disable
        if((!g_uu_enable)&&(!g_ed_enable)&&(!g_wnd_enable)){
            TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][Wind]last disable.\n", 0);
            audio_anc_monitor_adaptive_anc_stream_suspend_resume(FALSE);
        }
    }
    //anc monitor set info
}

#endif

#ifdef AIR_ANC_USER_UNAWARE_ENABLE
void audio_user_unaware_get_enable_status_from_nvdm(uint8_t* enable)
{
    //read UU status from nvdm
    typedef struct
    {
        U8 ENABLE;
        U8 REVISION;

        S16 alpha_par;
        S32 thd;
        S32 switch_par;
    } PACKED user_unaware_para_t;
    sysram_status_t status;
    uint32_t UU_tableSize = 0;
    uint32_t UU_buff_size = sizeof(user_unaware_para_t);
    user_unaware_para_t *pUUpara = (user_unaware_para_t*)pvPortMalloc(UU_buff_size);
    if (pUUpara){
        status = flash_memory_query_nvdm_data_length(NVKEY_DSP_PARA_USR_UNAWARE, &UU_tableSize);
        if(status || !UU_tableSize){
            TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][User Unaware]query_nvdm_data_length err, status %d, size %d", 2, status, UU_tableSize);
        }
        if(UU_tableSize != UU_buff_size){
            TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][User Unaware]query_nvdm_data_length err, query_size %d, buff_size", 2, UU_tableSize, UU_buff_size);
        }
        status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_USR_UNAWARE, (uint8_t *)pUUpara, &UU_tableSize);
        if (status != NVDM_STATUS_NAT_OK) {
            TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][User Unaware]read_nvdm_data err, ID 0x%x, status %d", 2, NVKEY_DSP_PARA_USR_UNAWARE, status);
            if(enable != NULL){
                *enable = false;
            }
        }else{
            if(enable != NULL){
                *enable = pUUpara->ENABLE;
            }
            TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][User Unaware]user_unaware_get_enable_status_from_nvdm, enable %d", 1, *enable);
        }
        vPortFree(pUUpara);
    }else {
        TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][User Unaware] malloc Fail", 0);
        assert(0);
    }
}


void audio_user_unaware_save_enable_status_to_nvdm(bool enable)
{
    TRANSMITTER_LOG_I("[User Unaware]user_unaware_save_enable_status_to_nvdm, enable %d", 1, enable);
    //save UU status to nvdm
    typedef struct
    {
        U8 ENABLE;
        U8 REVISION;

        S16 alpha_par;
        S32 thd;
        S32 switch_par;
    } PACKED user_unaware_para_t;
    sysram_status_t status;
    uint32_t UU_tableSize = 0;
    uint32_t UU_buff_size = sizeof(user_unaware_para_t);
    user_unaware_para_t *pUUpara = (user_unaware_para_t*)pvPortMalloc(UU_buff_size);
    if (pUUpara){
        status = flash_memory_query_nvdm_data_length(NVKEY_DSP_PARA_USR_UNAWARE, &UU_tableSize);
        if(status || !UU_tableSize){
            TRANSMITTER_LOG_E("[User Unaware]query_nvdm_data_length err, status %d, size %d", 2, status, UU_tableSize);
        }
        if(UU_tableSize != UU_buff_size){
            TRANSMITTER_LOG_E("[User Unaware]query_nvdm_data_length err, query_size %d, buff_size", 2, UU_tableSize, UU_buff_size);
        }
        status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_USR_UNAWARE, (uint8_t *)pUUpara, &UU_tableSize);
        if (status != NVDM_STATUS_NAT_OK) {
            TRANSMITTER_LOG_E("[User Unaware]read_nvdm_data err, ID 0x%x, status %d", 2, NVKEY_DSP_PARA_USR_UNAWARE, status);
        }
        pUUpara->ENABLE = (U8)enable;
        status = flash_memory_write_nvdm_data(NVKEY_DSP_PARA_USR_UNAWARE, (uint8_t *)pUUpara, UU_buff_size);
        if (status != NVDM_STATUS_NAT_OK) {
            TRANSMITTER_LOG_E("[User Unaware]write_nvdm_data err, ID 0x%x, status %d", 2, NVKEY_DSP_PARA_USR_UNAWARE, status);
        }
        vPortFree(pUUpara);
    }else{
        TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][User Unaware] malloc Fail", 0);
        assert(0);
    }
}
void audio_user_unaware_enable(bool enable)
{//app layer & race cmd
    audio_anc_monitor_set_info(AUDIO_ANC_MONITOR_SET_USER_UNAWARE_ENABLE, (uint32_t)enable);

    if(enable){
        g_uu_enable = true;
        TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][User Unaware]resume UU. wnd %d, uu %d, ed %d\n", 3, g_wnd_enable, g_uu_enable, g_ed_enable);
        //check if first enable
        if((g_uu_enable)&&(!g_ed_enable)&&(!g_wnd_enable)){
            TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][User Unaware]first enable.\n", 0);
            audio_anc_monitor_adaptive_anc_stream_suspend_resume(TRUE);
        }
    }else{
        g_uu_enable = false;
        TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][User Unaware]suspend UU. wnd %d, uu %d, ed %d\n", 3, g_wnd_enable, g_uu_enable, g_ed_enable);
        //check if last disable
        if((!g_uu_enable)&&(!g_ed_enable)&&(!g_wnd_enable)){
            TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][User Unaware]last disable.\n", 0);
            audio_anc_monitor_adaptive_anc_stream_suspend_resume(FALSE);
        }
    }
}

U8 audio_user_unaware_get_enable_state(void)
{
#if 0
    U32 *enable = pvPortMallocNC(sizeof(U32));
    U8 res;
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AUDIO_ANC_ADAPTIVE_GET,
                                                      AUDIO_USER_UNAWARE_GET_INFO_TYPE_ENABLE_STAT,
                                                      hal_memview_cm4_to_infrasys((uint32_t)enable),
                                                      true);
    res = (U8)(*enable);
    vPortFreeNC(enable);
#else
    return (U8)audio_anc_monitor_get_info(AUDIO_ANC_MONITOR_GET_USER_UNAWARE_ENABLE_STAT);
#endif
}

void audio_user_unaware_get_gain_info(uint8_t* info_ptr)
{
    if(xSemaphoreTake(g_adaptive_ANC_semaphore, portMAX_DELAY) == pdPASS){//take semaphore
        uint16_t i;
        user_unaware_info* gain_ptr = (user_unaware_info*)info_ptr;
        uint32_t share_addr = (uint32_t)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_ANC_MONITOR_ADAPTIVE_ANC);
        memset(gain_ptr, 0, sizeof(user_unaware_info));

        g_adaptive_ANC_getinfo_DSP_write_done = false;
        audio_anc_monitor_get_info(AUDIO_ANC_MONITOR_GET_USER_UNAWARE_GAIN_INFO); //send to AM

        for(i = 0; ;i++){
            if(g_adaptive_ANC_getinfo_DSP_write_done){
                memcpy(gain_ptr, (user_unaware_info*)share_addr, sizeof(user_unaware_info));
                xSemaphoreGive(g_adaptive_ANC_semaphore);//give semaphore
                TRANSMITTER_LOG_I("[User Unaware]Get gain info, %d, %d, %d, %d, %d, %dms", 6, gain_ptr->log0[0], gain_ptr->log0[1], gain_ptr->log2, gain_ptr->log3, gain_ptr->log4, i*2);
                break;
            }
            if ((i % 500) == 0) {
                TRANSMITTER_LOG_I("[User Unaware]audio_user_unaware_get_gain_info start waiting\r\n", 0);
                if(i == 500){
                    TRANSMITTER_LOG_E("[User Unaware]audio_user_unaware_get_gain_info 1s timeout", 0);
                    assert(0);
                }
            }
            hal_gpt_delay_ms(2);
        }
    }
}
#endif

#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
void audio_environment_detection_get_suspend_procedure_status_from_nvdm(uint8_t* enable)
{
    uint32_t nvkey_length = 0;
    DSP_PARA_ENVIRONMENT_DETECTION_STRU *malloc_ptr = NULL;
    uint16_t nvkey_id;
    sysram_status_t nvdm_status;

    nvkey_id = NVKEY_DSP_PARA_ENVIRONMENT_DETECTION;
    nvdm_status =  flash_memory_query_nvdm_data_length(nvkey_id, &nvkey_length);
    if (nvdm_status || !nvkey_length) {
        TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][env_detect]Query Nvkey length Fail id:0x%x, status:%d ", 2, nvkey_id, nvdm_status);
    }
    malloc_ptr = pvPortMallocNC(nvkey_length);
    if (malloc_ptr) {
        nvdm_status = flash_memory_read_nvdm_data(nvkey_id, (uint8_t *)malloc_ptr, &nvkey_length);
        if (nvdm_status || !nvkey_length) {
            TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][env_detect]Read Nvkey data Fail id:0x%x, status:%d ", 2, nvkey_id, nvdm_status);
        }

        //malloc_ptr->pause_attenuation = (uint16_t)suspend;

        if (nvdm_status != NVDM_STATUS_NAT_OK) {
            TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][env_detect]read_nvdm_data err, ID 0x%x, status %d", 2, nvkey_id, nvdm_status);
            if(enable != NULL){
                *enable = false;
            }
        }else{
            if(enable != NULL){
                if(malloc_ptr->pause_attenuation){ //true for suspend ED
                    *enable = false;
                }else{
                    *enable = true;
                }
            }
            TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][env_detect]ed_get_enable_status_from_nvdm, enable %d", 1, *enable);
        }
        vPortFreeNC(malloc_ptr);
    } else {
        TRANSMITTER_LOG_E("[ANC_Monitor][Adapt ANC][env_detect] malloc Fail", 0);
        assert(0);
    }
}
void audio_environment_detection_store_suspend_procedure(bool suspend)
{
    uint32_t nvkey_length = 0;
    DSP_PARA_ENVIRONMENT_DETECTION_STRU *malloc_ptr = NULL;
    uint16_t nvkey_id;
    sysram_status_t nvdm_status;

    nvkey_id = NVKEY_DSP_PARA_ENVIRONMENT_DETECTION;
    nvdm_status =  flash_memory_query_nvdm_data_length(nvkey_id, &nvkey_length);
    if (nvdm_status || !nvkey_length) {
        TRANSMITTER_LOG_E("[ANC_Monitor][env_detect]Store Nvkey length Fail id:0x%x, status:%d ", 2, nvkey_id, nvdm_status);
    }
    malloc_ptr = pvPortMallocNC(nvkey_length);
    if (malloc_ptr) {
        nvdm_status = flash_memory_read_nvdm_data(nvkey_id, (uint8_t *)malloc_ptr, &nvkey_length);
        if (nvdm_status || !nvkey_length) {
            TRANSMITTER_LOG_E("[ANC_Monitor][env_detect]Store Read Nvkey data Fail id:0x%x, status:%d ", 2, nvkey_id, nvdm_status);
        }

        malloc_ptr->pause_attenuation = (uint16_t)suspend;

        nvdm_status = flash_memory_write_nvdm_data(nvkey_id, (uint8_t *)malloc_ptr, nvkey_length);
        if (nvdm_status) {
            TRANSMITTER_LOG_E("[ANC_Monitor][env_detect]Store Write Nvkey data Fail id:0x%x, status:%d ", 2, nvkey_id, nvdm_status);
        }
        vPortFreeNC(malloc_ptr);
    } else {
        TRANSMITTER_LOG_E("[ANC_Monitor][env_detect] malloc Fail", 0);
        assert(0);
    }
}


void audio_environment_detection_suspend_procedure(bool suspend)
{
    //suspend ED
    audio_environment_detection_store_suspend_procedure(suspend);
    audio_anc_monitor_set_info(AUDIO_ANC_MONITOR_SET_ENVIRONMENT_DETECTION_SUSPEND, (uint32_t)suspend);

    //true for suspend, false for resume
    if(suspend){
        g_ed_enable = false;
        TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][env_detect]suspend ED. wnd %d, uu %d, ed %d\n", 3, g_wnd_enable, g_uu_enable, g_ed_enable);
        //check if last disable
        if((!g_uu_enable)&&(!g_ed_enable)&&(!g_wnd_enable)){
            TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][env_detect]last disable.\n", 0);
            audio_anc_monitor_adaptive_anc_stream_suspend_resume(FALSE);
        }
    }else{
        g_ed_enable = true;
        TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][env_detect]resume ED. wnd %d, uu %d, ed %d\n", 3, g_wnd_enable, g_uu_enable, g_ed_enable);
        //check if first enable
        if((!g_uu_enable)&&(g_ed_enable)&&(!g_wnd_enable)){
            TRANSMITTER_LOG_I("[ANC_Monitor][Adapt ANC][env_detect]first enable.\n", 0);
            audio_anc_monitor_adaptive_anc_stream_suspend_resume(TRUE);
        }

    }
}

int16_t audio_environment_detection_get_stationary_noise(void)
{
    //ED get info
    uint16_t i;
    int16_t info = 0;
    uint32_t share_addr = (uint32_t)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_ANC_MONITOR_ADAPTIVE_ANC);
    if(xSemaphoreTake(g_adaptive_ANC_semaphore, portMAX_DELAY) == pdPASS){//take semaphore
        g_adaptive_ANC_getinfo_DSP_write_done = false;
        audio_anc_monitor_get_info(AUDIO_ANC_MONITOR_GET_ENVIRONMENT_DETECTION_STATIONARY_NOISE); //send to AM

        for(i = 0; ;i++){
            if(g_adaptive_ANC_getinfo_DSP_write_done){
                memcpy(&info, (int16_t*)share_addr, sizeof(info));
                xSemaphoreGive(g_adaptive_ANC_semaphore);//give semaphore
                TRANSMITTER_LOG_I("[env_detect]Get info, %d, %dms", 2, info, i*2);
                break;
            }
            if ((i % 500) == 0) {
                TRANSMITTER_LOG_I("[env_detect]audio_environment_detection_get_stationary_noise start waiting\r\n", 0);
                if(i == 500){
                    TRANSMITTER_LOG_E("[env_detect]audio_environment_detection_get_stationary_noise 1s timeout", 0);
                    assert(0);
                }
            }
            hal_gpt_delay_ms(2);
        }
    }
    return info;
}
#endif

#if 0
void audio_dsp_adaptive_message_callback(hal_audio_event_t event, void *user_data)
{
    bool is_sync_control = false;
    audio_anc_control_result_t anc_control_result = 0;
    audio_anc_control_extend_ramp_cap_t anc_extend_ramp_cap;

    anc_extend_ramp_cap.gain_type = ((uint8_t)event);
    anc_extend_ramp_cap.extend_gain_1 = (*(uint32_t *)user_data) & 0xFFFF;
    anc_extend_ramp_cap.extend_gain_2 = ((*(uint32_t *)user_data) >> 16) & 0xFFFF;
    if (anc_extend_ramp_cap.gain_type ==  AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE
        || anc_extend_ramp_cap.gain_type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE
        || anc_extend_ramp_cap.gain_type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION) {
        is_sync_control = true;
    } else if (anc_extend_ramp_cap.gain_type ==  AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_HOWLING_CONTROL) {
        is_sync_control = false;
    }
    if (anc_extend_ramp_cap.gain_type == AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION) {
        uint8_t noise_level;
        noise_level = (uint8_t)((anc_monitor_temp>>8)&0xFF);
        printf(" gain_dsp_callback, Noise Gate Level %d @@@@", noise_level);
    }

    TRANSMITTER_LOG_E(" gain_dsp_callback, type=%d, is_sync=%d gain1=%d gain2=%d",
                     4, anc_extend_ramp_cap.gain_type, is_sync_control,
                     anc_extend_ramp_cap.extend_gain_1, anc_extend_ramp_cap.extend_gain_2);
#if 0
    if (is_sync_control) {
        /* Notify APP. */
        app_anc_extend_gain_handle_callback(anc_extend_ramp_cap.gain_type, anc_extend_ramp_cap.extend_gain_1, anc_extend_ramp_cap.extend_gain_2);
    } else {
        /* ANC control. */
        anc_control_result = audio_anc_control_set_extend_gain(anc_extend_ramp_cap.gain_type, &anc_extend_ramp_cap, NULL);
        APPS_LOG_MSGID_I(LOG_TAG" gain_dsp_callback, HOWLING_CONTROL result %d", 1, anc_control_result);
    }
    #endif
}
#endif

#endif

