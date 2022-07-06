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

#include "environment_detection_interface.h"

/* Private define ------------------------------------------------------------*/
#define ENVIRONMENT_DETECTION_VALID_MEMORY_CHECK_VALUE   ((U32)0x00600060)
#define DSP_ENVIRONMENT_DETECTION_INSTANCE_MEMSIZE       (sizeof(ENVIRONMENT_DETECTION_INSTANCE))
#define ENVIRONMENT_DETECTION_PROCESS_FRAME              (240)//Unit: sample
#define ENVIRONMENT_DETECTION_DEBUG_PRINT_PERIOD         (30)//Unit: 15ms

#define ENVIRONMENT_DETECTION_DEBUG_FORCE_CHANGE_STATUS  (0)// For gain sync verifiaction


/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ENVIRONMENT_DETECTION_INSTANCE_PTR environment_detection_instance;
void *environment_detection_instance_nvkey_ptr = NULL;

/* Public variables ----------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/
BOOL environment_detection_check_memory (VOID)
{
    if (NULL != environment_detection_instance)
    {
        if (ENVIRONMENT_DETECTION_VALID_MEMORY_CHECK_VALUE == environment_detection_instance->memory_check)
        {
            return TRUE;
        }
    }
    return FALSE;
}

bool environment_detection_push_data(int16_t *data, uint32_t size_of_sample)
{
    if ((size_of_sample + environment_detection_instance->buf_ctrl.buffer_cnt)<= DSP_ENVIRONMENT_DETECTION_BUFFERINF_SIZE) {
        memcpy(&environment_detection_instance->buf_ctrl.buffer[environment_detection_instance->buf_ctrl.buffer_cnt], data, sizeof(int16_t)*size_of_sample);
        environment_detection_instance->buf_ctrl.buffer_cnt += size_of_sample;
    } else {
        assert(0);
    }
    return false;
}

bool environment_detection_pop_data(uint32_t size_of_sample)
{
    if (environment_detection_instance->buf_ctrl.buffer_cnt >= size_of_sample) {
        memcpy(&environment_detection_instance->buf_ctrl.buffer[0], &environment_detection_instance->buf_ctrl.buffer[size_of_sample], sizeof(int16_t)*(environment_detection_instance->buf_ctrl.buffer_cnt-size_of_sample));
        environment_detection_instance->buf_ctrl.buffer_cnt -= size_of_sample;
    } else {
        assert(0);
    }
    return false;
}

uint32_t environment_detection_get_data_cnt(void)
{
    return environment_detection_instance->buf_ctrl.buffer_cnt;
}

int16_t *environment_detection_get_read_buffer_ptr(void)
{
    return &environment_detection_instance->buf_ctrl.buffer[0];
}

bool environment_detection_change_status(environment_detection_status_t noise_level)
{
    audio_extend_gain_control_t gain_control;
    int16_t attenuation_ff = 0, attenuation_fb = 0;
    environment_detection_instance->current_noise_level = noise_level;
    environment_detection_instance->consecutive_times = 0;
    if ((environment_detection_instance->current_noise_level >= DSP_ENVIRONMENT_DETECTION_LEVEL_1) &&
        (environment_detection_instance->current_noise_level <= environment_detection_instance->nvkey.level_numbers)) {
        attenuation_ff = environment_detection_instance->nvkey.dsp_environment_detection_levels[environment_detection_instance->current_noise_level-DSP_ENVIRONMENT_DETECTION_LEVEL_1].attenuation_ff;
        attenuation_fb = environment_detection_instance->nvkey.dsp_environment_detection_levels[environment_detection_instance->current_noise_level-DSP_ENVIRONMENT_DETECTION_LEVEL_1].attenuation_fb;
    }

    //Call CCNI
    gain_control.gain_type = AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION;
    gain_control.misc = (uint8_t)noise_level;
    gain_control.gain[0] = attenuation_ff;
    gain_control.gain[1] = attenuation_fb;
    dsp_ccni_set_extend_gain(&gain_control);
    DSP_MW_LOG_I("[ED] Change Level:%d ", 1, environment_detection_instance->current_noise_level);
    return false;
}
int16_t environment_detection_get_stationary_noise(void)
{
    if (environment_detection_check_memory()) {
        return environment_detection_instance->stationary_noise;
    } else {
        return -1;
    }
}

environment_detection_status_t environment_detection_convert_noise_to_level_status (S16 noise_value)
{
    environment_detection_instance->stationary_noise = noise_value;
#if (!ENVIRONMENT_DETECTION_DEBUG_FORCE_CHANGE_STATUS)
    environment_detection_status_t new_noise_level = environment_detection_instance->current_noise_level;
    environment_detection_status_t attack_level, release_level;
    bool find_new_status = false;

    //Suspend attenuation, Keep at disabled status.
    if (environment_detection_instance->nvkey.pause_attenuation){
        return DSP_ENVIRONMENT_DETECTION_DISABLE;
    }

    while (!find_new_status) {

        if (new_noise_level == DSP_ENVIRONMENT_DETECTION_DISABLE) {
            attack_level = environment_detection_instance->nvkey.level_numbers;
            release_level = DSP_ENVIRONMENT_DETECTION_INVALID;
        } else {

            attack_level = new_noise_level-1;
            if(attack_level == DSP_ENVIRONMENT_DETECTION_DISABLE) {
                attack_level = DSP_ENVIRONMENT_DETECTION_INVALID;
            }
            release_level = new_noise_level;
        }

        if ((attack_level != DSP_ENVIRONMENT_DETECTION_INVALID) &&
            (noise_value <= environment_detection_instance->nvkey.dsp_environment_detection_levels[attack_level-DSP_ENVIRONMENT_DETECTION_LEVEL_1].attack_threshold) ) {
            new_noise_level = attack_level;

            if (new_noise_level==DSP_ENVIRONMENT_DETECTION_DISABLE) {
                new_noise_level = DSP_ENVIRONMENT_DETECTION_LEVEL_1;
                find_new_status = true;
            }
        } else if ((release_level != DSP_ENVIRONMENT_DETECTION_INVALID) &&
                   (noise_value >= environment_detection_instance->nvkey.dsp_environment_detection_levels[release_level-DSP_ENVIRONMENT_DETECTION_LEVEL_1].release_threshold) ) {
            new_noise_level++;

            if (new_noise_level > environment_detection_instance->nvkey.level_numbers) {
                new_noise_level = DSP_ENVIRONMENT_DETECTION_DISABLE;
                find_new_status = true;
            }
        } else {
            find_new_status = true;
        }
    }
    //DSP_MW_LOG_I("[ED] convert level status Noise:%d, Current_Level:%d, New Level:%d ====", 3, noise_value, environment_detection_instance->current_noise_level, new_noise_level);
    return new_noise_level;
#elif (ENVIRONMENT_DETECTION_DEBUG_FORCE_CHANGE_STATUS == 1)
    //Debug APP CONTROL 1
    static environment_detection_status_t level_status = DSP_ENVIRONMENT_DETECTION_DISABLE;
    UNUSED(noise_value);
    if (!(environment_detection_instance->process_cnt%600)) {
        if (level_status==DSP_ENVIRONMENT_DETECTION_DISABLE) {
            level_status = DSP_ENVIRONMENT_DETECTION_LEVEL_1;
        } else {
            level_status++;
            if (level_status>DSP_ENVIRONMENT_DETECTION_LEVEL_2) {
                level_status = DSP_ENVIRONMENT_DETECTION_DISABLE;
            }
        }
    }
    return level_status;
#else
    //Debug APP CONTROL 2
    static environment_detection_status_t level_status = DSP_ENVIRONMENT_DETECTION_DISABLE;
    UNUSED(noise_value);
    if (!(environment_detection_instance->process_cnt%500)) {
        if (level_status==DSP_ENVIRONMENT_DETECTION_DISABLE) {
            level_status = DSP_ENVIRONMENT_DETECTION_LEVEL_1;
        } else {
            level_status = DSP_ENVIRONMENT_DETECTION_DISABLE;
        }
    }
    return level_status;
#endif

    return DSP_ENVIRONMENT_DETECTION_DISABLE;
}


/* Public functions ----------------------------------------------------------*/
bool stream_function_environment_detection_load_nvkey(void *nvkey)
{
    if (environment_detection_check_memory()) {
        memcpy(&environment_detection_instance->nvkey, nvkey, sizeof(DSP_PARA_ENVIRONMENT_DETECTION_STRU));
        environment_detection_instance->nvkey_ready = true;

        if(environment_detection_instance_nvkey_ptr) {
            vPortFree(environment_detection_instance_nvkey_ptr);
            DSP_MW_LOG_I("[ED] memory heap free", 0);

        }
        environment_detection_instance_nvkey_ptr = NULL;
        return true;
    } else {
        if (!environment_detection_instance_nvkey_ptr) {
            environment_detection_instance_nvkey_ptr = pvPortMalloc(sizeof(DSP_PARA_ENVIRONMENT_DETECTION_STRU));
            DSP_MW_LOG_I("[ED] memory heap allocate 0x%x ", 1, environment_detection_instance_nvkey_ptr);
        }

        if(environment_detection_instance_nvkey_ptr) {
            memcpy(environment_detection_instance_nvkey_ptr, nvkey, sizeof(DSP_PARA_ENVIRONMENT_DETECTION_STRU));
        }
    }
    return false;
}


bool stream_function_environment_detection_get_status(void)
{
    if (environment_detection_check_memory()) {
        return (bool) environment_detection_instance->current_noise_level;
    }
    return false;
}

bool stream_function_environment_detection_reset_status(void)
{
    if (environment_detection_check_memory()) {
        environment_detection_change_status(DSP_ENVIRONMENT_DETECTION_DISABLE);
    }
    return false;
}

bool stream_function_environment_detection_pause_attenuation(bool is_pause)
{
    if (environment_detection_check_memory()) {
        environment_detection_instance->nvkey.pause_attenuation = is_pause;
        stream_function_environment_detection_reset_status();
    }
    DSP_MW_LOG_I("[ED] Pause Procedure : %d", 1, is_pause);
    return false;
}

void stream_function_environment_detection_set_value(audio_anc_monitor_set_info_t set_type, uint32_t data)
{
    if (set_type == AUDIO_ANC_MONITOR_SET_ENVIRONMENT_DETECTION_SUSPEND) {
        stream_function_environment_detection_pause_attenuation((bool)data);
    } else {
        DSP_MW_LOG_W ("[ED] Unsupported Get type : %d", 1, set_type);
    }
}

uint32_t stream_function_environment_detection_get_value(audio_anc_monitor_get_info_t get_type)
{
    uint32_t data;
    if (get_type == AUDIO_ANC_MONITOR_GET_ENVIRONMENT_DETECTION_STATIONARY_NOISE) {
        data = (uint32_t)environment_detection_get_stationary_noise();
    } else {
        DSP_MW_LOG_W ("[ED] Unsupported Set type : %d", 1, get_type);
    }
    return data;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_environment_detection_initialize(void *para)
{
    uint32_t scratch_memory_size = 0;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);

    if (!environment_detection_check_memory()) {
        scratch_memory_size = get_ne_memsize();
        environment_detection_instance = (ENVIRONMENT_DETECTION_INSTANCE_PTR)DSPMEM_tmalloc(stream_function_get_task(para), DSP_ENVIRONMENT_DETECTION_INSTANCE_MEMSIZE+scratch_memory_size, stream_ptr);
        environment_detection_instance->memory_check = ENVIRONMENT_DETECTION_VALID_MEMORY_CHECK_VALUE;
        environment_detection_instance->nvkey_ready = false;
        environment_detection_instance->init_done = false;
        environment_detection_change_status(DSP_ENVIRONMENT_DETECTION_DISABLE);
        environment_detection_instance->buf_ctrl.buffer_cnt = 0;
    }
    //Re-read Nvkey when create stream late
    if (environment_detection_instance_nvkey_ptr) {
        stream_function_environment_detection_load_nvkey(environment_detection_instance_nvkey_ptr);
    }

    if (environment_detection_instance->nvkey_ready) {
        Noise_Est_init(&environment_detection_instance->scratch_memory, &environment_detection_instance->nvkey);
        environment_detection_instance->init_done = true;
    }
    DSP_MW_LOG_I("[ED] initialize:  init = %d, nvkey = %d", 2, environment_detection_instance->init_done, environment_detection_instance->nvkey_ready);

    return false;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_environment_detection_process(void *para)
{

    int16_t* input_buffer = stream_function_get_1st_inout_buffer(para);
    uint32_t frame_size;
    environment_detection_status_t noise_level = DSP_ENVIRONMENT_DETECTION_DISABLE;
    int16_t noise_value;

    if (!environment_detection_check_memory() || !environment_detection_instance->init_done) {
        stream_function_environment_detection_initialize(para);
    }

    if (!environment_detection_instance->init_done) {
        DSP_MW_LOG_I("[ED] haven't init = %d, nvkey = %d", 2, environment_detection_instance->init_done,environment_detection_instance->nvkey_ready);
        return false;
    }

    //Disable by Nvkey
    if (!environment_detection_instance->nvkey.enable) {
        return false;
    }

    if (stream_function_get_output_resolution(para) == RESOLUTION_32BIT) {
    }

    frame_size = stream_function_get_output_size(para)/sizeof(int16_t);
    environment_detection_push_data(input_buffer, frame_size);

    while (environment_detection_get_data_cnt() >= ENVIRONMENT_DETECTION_PROCESS_FRAME) {
        LOG_AUDIO_DUMP((U8*)environment_detection_get_read_buffer_ptr(), (U32)ENVIRONMENT_DETECTION_PROCESS_FRAME*sizeof(int16_t), AUDIO_WOOFER_RX);

        noise_value = (int16_t)Noise_Est_Prcs(&environment_detection_instance->scratch_memory , environment_detection_get_read_buffer_ptr());
        noise_level = environment_detection_convert_noise_to_level_status(noise_value);

        memset((U8*)environment_detection_get_read_buffer_ptr(), (noise_value>>8), (U32)ENVIRONMENT_DETECTION_PROCESS_FRAME*sizeof(int16_t));
        LOG_AUDIO_DUMP((U8*)environment_detection_get_read_buffer_ptr(), (U32)ENVIRONMENT_DETECTION_PROCESS_FRAME*sizeof(int16_t), AUDIO_WOOFER_UPSAMPLE_8K);
        environment_detection_pop_data(ENVIRONMENT_DETECTION_PROCESS_FRAME);

        //Report detected result
        if ((environment_detection_instance->nvkey.print_result) && (!(environment_detection_instance->process_cnt%ENVIRONMENT_DETECTION_DEBUG_PRINT_PERIOD))) {
            DSP_MW_LOG_I("[ED] report value:%d, Level:%d , cnt:%d ", 3, noise_value, noise_level, environment_detection_instance->process_cnt);
        }


        if (noise_level != environment_detection_instance->current_noise_level) {

            if (noise_level < environment_detection_instance->previous_noise_level) {
                //Reset counter if noise level is lower.
                environment_detection_instance->consecutive_times = 0;
            }
            environment_detection_instance->consecutive_times++;
            environment_detection_instance->previous_noise_level = noise_level;

            if (((environment_detection_instance->current_noise_level < environment_detection_instance->previous_noise_level) ||
                 (environment_detection_instance->previous_noise_level==DSP_ENVIRONMENT_DETECTION_DISABLE)) &&
                (environment_detection_instance->consecutive_times >= environment_detection_instance->nvkey.release_count)) {
                // more noise environment
                environment_detection_change_status(noise_level);
            } else if ((environment_detection_instance->current_noise_level > environment_detection_instance->previous_noise_level) &&
                       (environment_detection_instance->consecutive_times >= environment_detection_instance->nvkey.attack_count)) {
                // less noise environment
                environment_detection_change_status(noise_level);
            }
        } else {
            //Reset counter if result is unchanged
            environment_detection_instance->consecutive_times = 0;
        }

        environment_detection_instance->process_cnt++;
    }

    return false;
}


