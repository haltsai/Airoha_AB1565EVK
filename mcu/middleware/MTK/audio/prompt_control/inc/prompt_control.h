/* Copyright Statement:
 *
 * (C) 2005-2017  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __PROMPT_CONTROL_H__
#define __PROMPT_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MTK_MP3_TASK_DEDICATE
#define VP_LOCAL_MASK_MUTE_SHARED_BUF  0x00000001
#define MTK_PROMPT_SOUND_USING_CONFIG_DEFAULT_GAIN_LEVEL
#endif
/**
 * @addtogroup Audio
 * @{
 * @addtogroup prompt_control
 * @{
 *
 * The prompt_control is used to provide interface for prompt sound control (for example, wav and MP3 prompt sound).
 *
 * @section prompt_control_api_usage How to use this module
 *
 * - An example on how to use the prompt_control APIs.
 *  - 1.  Voice Prompt application select a prompt sound.
 *        Use #prompt_control_play_tone() to play the prompt sound.
 *  - 2.  Voice Prompt application may halt the playback by using #prompt_control_stop_tone().
 *  - 3.  Voice Prompt application may adjust volume weighting by using #voice_prompt_mix_set_mixer_gain.
 *    - Sample code:
 *     @code
 *
 *
 *     @endcode
 */

/** @defgroup prompt_control_enum Enum
  * @{
  */

/** @brief Prompt Control Tone Type. */
typedef enum {
    PROMPT_CONTROL_RETURN_ERROR = -1,    /**< The audio codec reported an error. */
    PROMPT_CONTROL_RETURN_OK    = 0,     /**< The audio codec works normally. */
} prompt_control_status_t;

/** @brief Prompt Control Tone Type. */
typedef enum {
    VPC_WAV,
    VPC_MP3,
    VPC_OPUS,
} prompt_control_tone_type_t;

/** @brief Prompt Control Events. */
typedef enum {
    PROMPT_CONTROL_MEDIA_PLAY,   /**< Play vp playback */
    PROMPT_CONTROL_MEDIA_STOP,   /**< Stop vp playback */
    PROMPT_CONTROL_MEDIA_END,    /**< end of vp playback */
#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
    PROMPT_CONTROL_DUMMY_SOURCE_START,  /**< Start VP Dummy Source playback. */
    PROMPT_CONTROL_DUMMY_SOURCE_STOP,   /**< Stop VP Dummy Source playback. */
#endif
} prompt_control_event_t;

#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
/**
 *  @brief This enum defines VP dummy source handler state type.
 */
typedef enum {
    PROMPT_CONTROL_DUMMY_SOURCE_IDLE_STATE,
    PROMPT_CONTROL_DUMMY_SOURCE_PLAY_STATE,
    PROMPT_CONTROL_DUMMY_SOURCE_STOPING_STATE,
    PROMPT_CONTROL_DUMMY_SOURCE_CLOSE_STATE,
    PROMPT_CONTROL_DUMMY_SOURCE_SUSPEND_STATE,
    PROMPT_CONTROL_DUMMY_SOURCE_RESUME_STATE,
} prompt_control_dummy_source_state_t;

typedef enum {
    DUMMY_SOURCE_MODE_ONE_SHOT,
    DUMMY_SOURCE_MODE_LOOP,
} prompt_control_dummy_source_mode_t;

/** @brief VP Dummy Source Parameters. */
typedef struct {
    uint8_t index;
    prompt_control_dummy_source_mode_t mode;
    uint8_t vol_lev;
} prompt_control_dummy_source_param_t;

/** @brief VP Dummy Source Handle. */
typedef struct {
    prompt_control_dummy_source_state_t state;
    prompt_control_dummy_source_param_t param_info;
} prompt_control_dummy_source_handle_t;
#endif

/** @brief Define prompt_control callback function prototype. The user should register a callback function when user play prompt sound.
 *  @param[in] event_id is the value defined in #mp3_codec_event_t. This parameter is given by the driver to notify the user about the data flow behavior.
 */
typedef void (*prompt_control_callback_t)(prompt_control_event_t event_id);


/**
  * @}
  */


/**
 * @brief     This function is used to play prompt sound for APP control interface.
 * @param[in] tone_type is an enumeration to indicate the audio format.
 * @param[in] tone_buf is the bitstream buffer address of the prompt sound.
 * @param[in] tone_size is the size of bitstream buffer.
 * @param[in] callback is the callback to notify application that the playback of prompt sound is complete.
 * @return    Return true if the operation is successful. Otherwise, error occurs.
 * @sa        #prompt_control_play_tone()
 */
bool prompt_control_play_tone(prompt_control_tone_type_t tone_type, uint8_t *tone_buf, uint32_t tone_size, prompt_control_callback_t callback);

/**
 * @brief     This function is used to play prompt sound in middelware level.
 * @param[in] tone_type is an enumeration to indicate the audio format.
 * @param[in] tone_buf is the bitstream buffer address of the prompt sound.
 * @param[in] tone_size is the size of bitstream buffer.
 * @param[in] callback is the callback to notify application that the playback of prompt sound is complete.
 * @return    Return true if the operation is successful. Otherwise, error occurs.
 * @sa        #prompt_control_play_tone_internal()
 */
bool prompt_control_play_tone_internal(prompt_control_tone_type_t tone_type, uint8_t *tone_buf, uint32_t tone_size, uint32_t sync_time, prompt_control_callback_t callback);

#ifdef MTK_MP3_TASK_DEDICATE
/**
 * @brief     This function is to query the prompt sound exit or not for APP control interface..
 * @param[in] None.
 * @return    Return true if the VP was playing. Otherwise, return false.
 * @sa        #prompt_control_query_state()
 */
bool prompt_control_query_state(void);

/**
 * @brief     This function is used to play synced prompt sound for APP control interface.
 * @param[in] tone_type is an enumeration to indicate the audio format.
 * @param[in] tone_buf is the bitstream buffer address of the prompt sound.
 * @param[in] tone_size is the size of bitstream buffer.
 * @param[in] sync_time is the time whcih AGENT and PARTNER want to sync.
 * @param[in] callback is the callback to notify application that the playback of prompt sound is complete.
 * @return    Return true if the operation is successful. Otherwise, error occurs.
 * @sa        #prompt_control_play_sync_tone()
 */
bool prompt_control_play_sync_tone(prompt_control_tone_type_t tone_type, uint8_t *tone_buf, uint32_t tone_size, uint32_t sync_time, prompt_control_callback_t callback);
#endif

/**
 * @brief     This function is to stop the prompt sound for APP control interface..
 * @param[in] None.
 * @return    None.
 * @sa        #prompt_control_stop_tone()
 */
void prompt_control_stop_tone(void);

/**
 * @brief     This function is to stop the prompt sound  in middelware level..
 * @param[in] None.
 * @return    None.
 * @sa        #prompt_control_stop_tone_internal()
 */
void prompt_control_stop_tone_internal(void);

/**
 * @brief     This function is used to adjust mixer gain.
 * @param[in] main_gain is the digital gain of main track. Format is Q.15, which means 0x8000 is unit gain = 1.
 * @param[in] side_gain is the digital gain of side track. Format is Q.15, which means 0x8000 is unit gain = 1.
 * @return    None.
 * @sa        #prompt_control_set_mixer_gain()
 */
void prompt_control_set_mixer_gain(uint16_t main_gain, uint16_t side_gain);

#ifdef MTK_AVM_DIRECT
void prompt_control_set_volume(void);
uint8_t prompt_control_get_level();
#ifndef MTK_PROMPT_SOUND_USING_CONFIG_DEFAULT_GAIN_LEVEL
void prompt_control_set_level(uint8_t vol_level);
#endif

/**
 * @brief     This function is used to mute / un-mute VP at DSP side.
 * @param[in] mute  When mute is "True", Mute VP(DL2) path. "False" is means un-mute VP(DL2) path.
 * @return    None.
 * @sa        #prompt_control_set_mute()
 */
void prompt_control_set_mute(bool mute);
#endif

#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
/**
 * @brief     This function is to query the dummy source prompt sound exit or not for APP control interface..
 * @param[in] None.
 * @return    Return true if the VP_dummy source was playing. Otherwise, return false.
 * @sa        #prompt_control_dummy_source_query_state()
 */
bool prompt_control_dummy_source_query_state(void);

/**
 * @brief     This function is used to adjust dummy source prompt sound gain. (DSP DL2.)
 * @param[in] vol_level range: Level 0~15 for now. Middleware will check the VP gain table in NV-Key to get digital and analog gain value.
 *                                         And set volume gain to DSP.
 * @return    None.
 * @sa        #prompt_control_dummy_source_set_volume()
 */
void prompt_control_dummy_source_set_volume(uint8_t vol_level);

/**
 * @brief     This function is used to adjust dummy source prompt sound gain. (DSP DL2.)
 * @param[in] None.
 * @return    gain_level. range: Level 0~15 for now.
 * @sa        #prompt_control_dummy_source_get_volume_level()
 */
uint8_t prompt_control_dummy_source_get_volume_level(void);

/**
 * @brief     This function is used to change dummy source index and mode for APP control interface.
 * @param[in] param is an structure to indicate the audio dummy source format.
 * @return    Return PROMPT_CONTROL_RETURN_ERROR if VP dummy Source not exist.
 *                Return PROMPT_CONTROL_RETURN_OK when send VP dummy source param to DSP successly.
 * @sa        #prompt_control_dummy_source_change_feature()
 */
prompt_control_status_t prompt_control_dummy_source_change_feature(prompt_control_dummy_source_param_t param);

/**
 * @brief     This function is used to play dummy source prompt sound for APP control interface.
 * @param[in] param is an structure to indicate the audio dummy source format.
 * @return    Return PROMPT_CONTROL_RETURN_ERROR if HFP exist or VP exist.
 *                Return PROMPT_CONTROL_RETURN_OK when send start VP dummy source to Audio Manager successly.
 * @sa        #prompt_control_dummy_source_start()
 */
prompt_control_status_t prompt_control_dummy_source_start(prompt_control_dummy_source_param_t param);

/**
 * @brief     This function is to stop the dummy source prompt sound for APP control interface..
 * @param[in] None.
 * @return    None.
 * @sa        #prompt_control_dummy_source_stop()
 */
void prompt_control_dummy_source_stop(void);
#endif

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/

#endif  /*__PROMPT_CONTROL_H__*/
