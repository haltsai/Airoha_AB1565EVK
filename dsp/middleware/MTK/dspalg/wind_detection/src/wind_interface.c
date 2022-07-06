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

/* Includes ------------------------------------------------------------------*/
#include "dsp_feature_interface.h"
#include "dsp_audio_process.h"
#include "dsp_dump.h"
#include "memory_attribute.h"
#include "preloader_pisplit.h"
#include "memory_attribute.h"
#include "dsp_memory.h"
#include "dsp_scenario.h"

#include "wind_interface.h"

/* Private define ------------------------------------------------------------*/
#define WIND_VALID_MEMORY_CHECK_VALUE   ((U32)0x00580058)
#define DSP_WIND_INSTANCE_MEMSIZE       (sizeof(WIND_INSTANCE))
#define WIND_DETECTION_PROCESS_FRAME    (240)//Unit: sample



/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
WIND_INSTANCE_PTR wind_instance;
void *wind_instance_nvkey_ptr = NULL;

/* Public variables ----------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/
BOOL wind_check_memory (VOID)
{
    if (NULL != wind_instance)
    {
        if (WIND_VALID_MEMORY_CHECK_VALUE == wind_instance->memory_check)
        {
            return TRUE;
        }
    }
    return FALSE;
}

bool wind_push_data(int16_t *data, uint32_t size_of_sample)
{
    if ((size_of_sample + wind_instance->buf_ctrl.buffer_cnt)<= DSP_WIND_DETECTION_BUFFERINF_SIZE) {
        memcpy(&wind_instance->buf_ctrl.buffer[wind_instance->buf_ctrl.buffer_cnt], data, sizeof(int16_t)*size_of_sample);
        wind_instance->buf_ctrl.buffer_cnt += size_of_sample;
    } else {
        assert(0);
    }
    return false;
}

bool wind_pop_data(uint32_t size_of_sample)
{
    if (wind_instance->buf_ctrl.buffer_cnt >= size_of_sample) {
        memcpy(&wind_instance->buf_ctrl.buffer[0], &wind_instance->buf_ctrl.buffer[size_of_sample], sizeof(int16_t)*(wind_instance->buf_ctrl.buffer_cnt-size_of_sample));
        wind_instance->buf_ctrl.buffer_cnt -= size_of_sample;
    } else {
        assert(0);
    }
    return false;
}

uint32_t wind_get_data_cnt(void)
{
    return wind_instance->buf_ctrl.buffer_cnt;
}

int16_t *wind_get_read_buffer_ptr(void)
{
    return &wind_instance->buf_ctrl.buffer[0];
}

bool wind_change_status(int16_t detect_result)
{
    uint32_t interrupt_mask;
    audio_extend_gain_control_t gain_control;
    int16_t attenuation = 0;
    hal_nvic_save_and_set_interrupt_mask(&interrupt_mask);
    wind_instance->detect_wind = detect_result;
    wind_instance->consecutive_times = 0;
    if (wind_instance->detect_wind) {
        attenuation = wind_instance->nvkey.attenuation;
    }
    hal_nvic_restore_interrupt_mask(interrupt_mask);

    #ifdef AIR_ANC_USER_UNAWARE_ENABLE
    extern int16_t g_UU_detect_wind;
    g_UU_detect_wind = wind_instance->detect_wind;
    #endif

    //Call CCNI
    gain_control.gain_type = AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE;
    gain_control.misc = 0;
    gain_control.gain[0] = attenuation;
    gain_control.gain[1] = 0;
    dsp_ccni_set_extend_gain(&gain_control);
    DSP_MW_LOG_I("[Wind Detection] Change Detect Result : %d ", 1, wind_instance->detect_wind);
    return false;
}


/* Public functions ----------------------------------------------------------*/
bool stream_function_wind_load_nvkey(void *nvkey)
{
    if (wind_check_memory()) {
        memcpy(&wind_instance->nvkey, nvkey, sizeof(DSP_PARA_WIND_STRU));
        wind_instance->nvkey_ready = true;

        if(wind_instance_nvkey_ptr) {
            vPortFree(wind_instance_nvkey_ptr);
            DSP_MW_LOG_I("[Wind Detection] memory heap free", 0);
        }
        wind_instance_nvkey_ptr = NULL;
        return true;
    } else {
        if (!wind_instance_nvkey_ptr) {
            wind_instance_nvkey_ptr = pvPortMalloc(sizeof(DSP_PARA_WIND_STRU));
            DSP_MW_LOG_I("[Wind Detection] memory heap allocate 0x%x ", 1, wind_instance_nvkey_ptr);
        }

        if(wind_instance_nvkey_ptr) {
            memcpy(wind_instance_nvkey_ptr, nvkey, sizeof(DSP_PARA_WIND_STRU));
        }
    }
    return false;
}

bool stream_function_wind_get_status(void)
{
    if (wind_check_memory()) {
        return (bool) wind_instance->detect_wind;
    }
    return false;
}

bool stream_function_wind_reset_status(void)
{
    if (wind_check_memory()) {
        wind_instance->consecutive_times = 0;
        wind_change_status(false);
    }
    return false;
}


ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_wind_initialize(void *para)
{
    uint32_t scratch_memory_size;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);

    if (!wind_check_memory()) {
        scratch_memory_size = WindDet_get_memsize();
        wind_instance = (WIND_INSTANCE_PTR)DSPMEM_tmalloc(stream_function_get_task(para), DSP_WIND_INSTANCE_MEMSIZE+scratch_memory_size, stream_ptr);
        wind_instance->memory_check = WIND_VALID_MEMORY_CHECK_VALUE;
        wind_instance->nvkey_ready = false;
        wind_instance->init_done = false;
        wind_change_status(false);
        wind_instance->buf_ctrl.buffer_cnt = 0;
    }
    //Re-read Nvkey when create stream late
    if (wind_instance_nvkey_ptr) {
        stream_function_wind_load_nvkey(wind_instance_nvkey_ptr);
    }

    if (wind_instance->nvkey_ready) {
        WindDet_Init(&wind_instance->scratch_memory, &wind_instance->nvkey);
        wind_instance->init_done = true;
        wind_instance->detection_period = (wind_instance->nvkey.option) ? 1 : wind_instance->nvkey.vad_leave;
    }

    DSP_MW_LOG_I("[Wind Detection] initialize:  init = %d, nvkey = %d", 2, wind_instance->init_done, wind_instance->nvkey_ready);

    return false;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_wind_process(void *para)
{

    S16* input_buffer = stream_function_get_1st_inout_buffer(para);
    uint32_t frame_size;
    S16 detect_result;

    if (!wind_check_memory() || !wind_instance->init_done) {
        stream_function_wind_initialize(para);
    }

    if (!wind_instance->init_done) {
        DSP_MW_LOG_I("[Wind Detection] haven't init = %d, nvkey = %d", 2, wind_instance->init_done,wind_instance->nvkey_ready);
        return false;
    }

    if (stream_function_get_output_resolution(para) == RESOLUTION_32BIT) {
    }

    frame_size = stream_function_get_output_size(para)/sizeof(int16_t);
    wind_push_data(input_buffer, frame_size);

    while (wind_get_data_cnt() >= WIND_DETECTION_PROCESS_FRAME) {
        LOG_AUDIO_DUMP((U8*)wind_get_read_buffer_ptr(), (U32)WIND_DETECTION_PROCESS_FRAME*sizeof(int16_t), AUDIO_SOUNDBAR_INPUT);
        WindDet_Prcs(&wind_instance->scratch_memory , wind_get_read_buffer_ptr(), &detect_result);
        memset((U8*)wind_get_read_buffer_ptr(), detect_result*100, (U32)WIND_DETECTION_PROCESS_FRAME*sizeof(int16_t));
        LOG_AUDIO_DUMP((U8*)wind_get_read_buffer_ptr(), (U32)WIND_DETECTION_PROCESS_FRAME*sizeof(int16_t), AUDIO_SOUNDBAR_TX);

        wind_pop_data(WIND_DETECTION_PROCESS_FRAME);
        if (detect_result >= 0) {
            //Report detected result
            if ((wind_instance->nvkey._reserved) && (!(wind_instance->process_cnt%wind_instance->nvkey.vad_leave))) {
                DSP_MW_LOG_I("[Wind Detection] report status:%d , cnt:%d ", 2, detect_result, wind_instance->process_cnt);
            }
            if (detect_result != wind_instance->detect_wind) {
                wind_instance->consecutive_times+=wind_instance->detection_period;
                if (!detect_result && (wind_instance->consecutive_times >= wind_instance->nvkey.release_count)) {
                    //Silent environment
                    wind_change_status(detect_result);
                } else if (detect_result && (wind_instance->consecutive_times >= wind_instance->nvkey.attack_count)){
                    //Wind environment
                    wind_change_status(detect_result);
                }
            } else {
                //Reset counter if result is unchanged
                wind_instance->consecutive_times = 0;
            }
        }
        wind_instance->process_cnt++;
    }

    return false;
}


