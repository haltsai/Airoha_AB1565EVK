/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

/**
 * File: app_voice_prompt.h
 *
 * Description: This file defines the interface of app_voice_prompt.c.
 *
 */

#ifndef APP_VOICE_PROMPT_H
#define APP_VOICE_PROMPT_H

#include "bt_sink_srv.h"
#include "bt_sink_srv_common.h"
#include "bt_gap.h"

#ifdef __cplusplus
extern "C" {
#endif


#define app_voice_prompt_report(_message,...)       LOG_I(VOICE_PROMPT_APP, (_message), ##__VA_ARGS__)
#define app_voice_prompt_msgid_report(_message,...) LOG_MSGID_I(VOICE_PROMPT_APP, _message, ##__VA_ARGS__)
#define app_voice_prompt_msgid_warning(_message,...) LOG_MSGID_W(VOICE_PROMPT_APP, _message, ##__VA_ARGS__)
#define app_voice_prompt_msgid_error(_message,...) LOG_MSGID_E(VOICE_PROMPT_APP, _message, ##__VA_ARGS__)

#define VP_VOICE_DEFAULT_DELAY 600     /* Default voice ringtone delay time. */
#define VP_LANG_CODEC_EN_US 0x409      /* Codec ID of EN_US. */
#define VP_LANG_CODEC_ZH_TW 0x404      /* Codec ID of ZH_TW. */
#define VP_LANG_CODEC_FR_FR 0x40C      /* Codec ID of FR_FR. */

#define APP_VOICE_PROMPT_QUEUE_MAX_LEN  10    /* Maximum number of every VP queue. */

#define APP_VOICE_PROMPT_SYNC_PLAY 0x01       /* Sync ID of VP play. */
#define APP_VOICE_PROMPT_SYNC_STOP 0x02       /* Sync ID of VP stop. */
#define APP_VOICE_PROMPT_SYNC_LANG 0x03       /* Sync ID of VP language. */

typedef uint16_t app_vp_lang_codec_t;  /* Type of language codec. */

/**
 *  @brief This enum defines the priority of VP.
 */
typedef enum {
    VOICE_PROMPT_PRIO_LOW,     /**<  Lowest priority VP. */
    VOICE_PROMPT_PRIO_MEDIUM,  /**<  Common VP. */
    VOICE_PROMPT_PRIO_HIGH,    /**<  High priority VP. */
    VOICE_PROMPT_PRIO_ULTRA,   /**<  Voice broadcast & ringtone. */
    VOICE_PROMPT_PRIO_EXTREME, /**<  Only for power off VP. */
    VOICE_PROMPT_PRIO_TOTAL    /**<  Total priority, not used. */
} app_vp_prio_t;

/**
 *  @brief This enum defines the type of VP.
 */
typedef enum {
    VP_TYPE_VP = 0,    /**<  Voice prompt. */
    VP_TYPE_VB,        /**<  Voice broadcast. */
    VP_TYPE_RT,        /**<  Ringtone. */
    VP_TYPE_NONE       /**<  Type none. */
} app_vp_type;

/**
 *  @brief This enum defines the error code of VP module.
 */
typedef enum {
    VP_ERR_CODE_SUCCESS = 0,    /**<  The playback is successful. */
    VP_ERR_CODE_START_PLAY,     /**<  The playback is started. */
    VP_ERR_CODE_FAIL,           /**<  The playback is failed. */
    VP_ERR_CODE_FILE_NO_FOUND,  /**<  The VP file is not found. */
    VP_ERR_CODE_STOP,           /**<  The playback is stopped. */
    VP_ERR_CODE_PREEMPTED,      /**<  The playback is preempted by higher priority VP. */
    VP_ERR_CODE_SYNC_FAIL       /**<  A sync VP sync fail due to AWS disconnect. */
} vp_err_code;

typedef void (*app_vp_play_callback_t)(uint32_t idx, vp_err_code err);  /* Type of APP VP callback function. */

/**
 *  @brief This enum defines the BT sniff mode status in the module.
 */
typedef enum {
    VP_BT_STATE_SNIFF,              /**<  BT in sniff mode, or the BT sniff mode has not been changed by the module. */
    VP_BT_STATE_SNIFF_EXITING,      /**<  The BT sniff mode is exiting. */
    VP_BT_STATE_ACTIVE,             /**<  The BT controller is active and not in sniff mode. */
} app_vp_bt_state;

/**
 *  @brief This enum defines the list status.
 */
typedef enum {
    VOICE_PROMPT_LIST_SUCCESS,      /**<  0, success. */
    VOICE_PROMPT_LIST_FAIL,         /**<  1, fail. */
} app_voice_prompt_list_status_t;

/**
 *  @brief This enum defines the event type of msgs sent to ui_realtime task.
 */
typedef enum {
    VOICE_PROMPT_EVENT_SNIFF_CHANGE,      /**<  Notify sniff mode changed and need sync VP. */
    VOICE_PROMPT_EVENT_LOCAL_PLAY,        /**<  Notify need play local VP after sync IF packet. */
    VOICE_PROMPT_EVENT_AWS_PLAY,          /**<  Notify need sync VP to partner and play. */
    VOICE_PROMPT_EVENT_SYNC_PLAY,         /**<  Notify received VP sync from agent and need play. */
    VOICE_PROMPT_EVENT_SYNC_STOP,         /**<  Notify received VP sync stop from agent and need stop. */
    VOICE_PROMPT_EVENT_CONNECTED_VP,      /**<  Notify need play CONNECTED VP after sync IF packet. */
    VOICE_PROMPT_EVENT_PLAY_START,        /**<  Notify a voice prompt playing started. */
    VOICE_PROMPT_EVENT_REMOTE_DISCONNECT, /**<  Notify that remote device has disconnect. */
} app_voice_prompt_event_t;

/**
 *  @brief This structure defines the msg sent to ui_realtime task.
 */
typedef struct {
    app_voice_prompt_event_t type;     /**<  Event type of the msg. */
    void *data;                        /**<  Extra data of the msg. */
} app_vp_task_msg_t;

/**
 *  @brief This structure defines the extra data of VOICE_PROMPT_EVENT_SNIFF_CHANGE event.
 */
typedef struct {
    bt_status_t status;                     /**<  BT status. */
    bt_gap_sniff_mode_changed_ind_t ind;    /**<  Extra data of bt_gap ind. */
} app_voice_prompt_sniff_change_t;

/**
 *  @brief This structure defines the extra data of VOICE_PROMPT_EVENT_LOCAL_PLAY event.
 */
typedef struct {
    uint32_t gpt_count;                 /**<  Target gpt count to play. */
    uint32_t play_index;                /**<  VP index to play. */
    app_vp_play_callback_t callback;    /**<  User callback function. */
} app_voice_prompt_local_play_t;

/**
 *  @brief This structure defines the extra data of VOICE_PROMPT_EVENT_AWS_PLAY event.
 */
typedef struct {
    uint32_t tone_idx;                /**<  VP index to play. */
    uint32_t delay_time;              /**<  Delay_time user set to play sync VP. */
    app_vp_type type;                 /**<  VP type. */
    app_vp_prio_t level;              /**<  VP priority. */
    bool isCleanup;                   /**<  Flag of whether need clean up all the VP in queue. */
    bool repeat;                      /**<  Flag of whether need loop playback. */
    app_vp_play_callback_t callback;  /**<  User callback function. */
} app_voice_prompt_aws_play_t;

/**
 *  @brief This structure defines the extra data of VOICE_PROMPT_EVENT_CONNECTED_VP event.
 */
typedef struct {
    uint32_t gpt_count;               /**<  Target gpt count to play. */
    app_vp_prio_t level;              /**<  VP priority. */
    bool cleanup;                     /**<  Flag of whether need clean up all the VP in queue. */
    app_vp_play_callback_t callback;  /**<  User callback function. */
} app_voice_prompt_connected_vp_t;

/**
 *  @brief This structure defines the extra data of VOICE_PROMPT_EVENT_SYNC_PLAY event.
 */
typedef struct {
    uint32_t play_index;       /**<  VP index to play. */
    app_vp_type type;          /**<  VP type. */
    uint32_t gpt_count;        /**<  Target gpt count to play. */
    uint32_t delay_time;       /**<  Delay time for sync VP, used after RHO. */
    app_vp_prio_t level;       /**<  VP priority. */
    uint16_t count;            /**<  VP play count. */
    bool cleanup;              /**<  Flag of whether need clean up all the VP in queue. */
    bool repeat;               /**<  Flag of whether need loop playback. */
} app_voice_prompt_synced_play_t;

/**
 *  @brief This structure defines the extra data of VOICE_PROMPT_EVENT_SYNC_STOP event.
 */
typedef struct {
    uint16_t id;           /**<  Play ID. */
    app_vp_type type;      /**<  VP type. */
    bool preempted;        /**<  Flag of whether the VP is stopped by preempted. */
} app_voice_prompt_synced_stop_t;


/**
 *  @brief This structure defines the data sent when sync VP.
 */
typedef struct {
    uint32_t            play_index;    /**<  VP index to play. */
    uint32_t            delay_time;    /**<  Delay time for sync VP, used after RHO. */
    bt_clock_t          play_time;     /**<  Target BT clock to play VP. */
    app_vp_play_callback_t callback;   /**<  User callback function. */
    uint16_t            count;         /**<  VP play count. */
    app_vp_prio_t       level;         /**<  VP priority. */
    app_vp_type         type;          /**<  VP type. */
    bool                isCleanup;     /**<  Flag of whether need clean up all the VP in queue. */
    bool                repeat;        /**<  Flag of whether need loop playback. */
} app_voice_prompt_play_sync_t;

/**
 *  @brief This structure defines the data sent when sync stop VP.
 */
typedef struct {
    uint16_t            id;          /**<  Play ID. */
    app_vp_type         type;        /**<  VP type. */
    bool                preempted;   /**<  Flag of whether the VP is stopped by preempted. */
    bt_clock_t          stop_time;   /**<  Target BT clock to stop, not use now. */
} app_voice_prompt_stop_sync_t;

/**
 *  @brief This structure defines the data sent when sync VP language.
 */
typedef struct {
    uint8_t lang_index;    /**<  Selected language index. */
} app_voice_prompt_lang_sync_t;

/**
 *  @brief This structure defines the item of the VP queue.
 */
typedef struct {
    app_vp_prio_t level;                /**<  VP priority. */
    app_vp_type   type;                 /**<  VP type. */
    bool          need_sync;            /**<  Flag of whether need sync play. */
    bool          repeat;               /**<  Flag of whether need loop playback. */
    uint16_t      id;                   /**<  Play ID. */
    uint32_t      index;                /**<  VP index to play. */
    uint32_t      delay_time;           /**<  Delay_time user set to play sync VP. */
    app_vp_play_callback_t callback;    /**<  User callback function. */
    bool          played;               /**<  Flag of whether the VP item is actually played. */
} app_voice_prompt_list_data_t;

typedef app_voice_prompt_list_data_t DataType;

/**
 *  @brief This structure defines the node of the VP queue.
 */
typedef struct node {
    DataType data;           /**<  The data item. */
    struct node *next;       /**<  Pointer of the next node. */
} vp_node;

/**
 *  @brief This structure defines the VP queue.
 */
typedef struct {
    vp_node *front;         /**<  Pointer of front node. */
    vp_node *rear;          /**<  Pointer of rear node. */
    uint16_t length;        /**<  Length of the queue. */
} vp_queue;

/**
* @brief      This function process a VOICE_PROMPT_EVENT_SNIFF_CHANGE event.
* @param[in]  status, BT status reported by bt_gap.
* @param[in]  ind, bt_gap ind reported by bt_gap.
*/
void app_voice_exit_sniff_cnf(bt_status_t status, bt_gap_sniff_mode_changed_ind_t *ind);

/**
* @brief      This function process a VOICE_PROMPT_EVENT_SYNC_PLAY event.
* @param[in]  play_data, parameters need to play sync VP.
*/
void app_voice_prompt_synced_play(app_voice_prompt_synced_play_t *play_data);

/**
* @brief      This function process a VOICE_PROMPT_EVENT_SYNC_STOP event.
* @param[in]  play_data, parameters need to stop sync VP.
*/
void app_voice_prompt_synced_stop(app_voice_prompt_synced_stop_t *stop_data);

/**
* @brief      This function process a VOICE_PROMPT_EVENT_AWS_PLAY event.
* @param[in]  tone_idx, VP index.
* @param[in]  delay_time, delay_time to play VP.
* @param[in]  type, VP type.
* @param[in]  level, VP priority.
* @param[in]  isCleanup, flag of whether to clean up the VP item in queue.
* @param[in]  repeat, flag of whether to loop.
* @param[in]  callback, user callback.
*/
void app_voice_prompt_aws_play_impl(uint32_t tone_idx, uint32_t delay_time, app_vp_type type, app_vp_prio_t level, bool isCleanup, bool repeat, app_vp_play_callback_t callback);

/**
* @brief      This function is to initialize VP module, and called in app_init.c.
*/
void app_voice_prompt_init();

/**
* @brief      This function require play a VP, there will occur preemption according with priority, otherwise, the VP will queue after a VP who the priority the same with.
* @param[in]  tone_idx is a VP index which requires play, see apps_config_vp_index_list.h.
* @param[in]  need_sync, it mean that VP need sync to partner at the same time.
* @param[in]  delay_time, after the time, agent and partner play at the same time.
* @param[in]  level, see app_vp_prio_t.
* @param[in]  cleanup, if true, will empty all queueing VP and reject any new VP from now on, this flag should use by power off only!!!
* @param[in]  callback, user callback.
* @param[in]  gpt_count, target gpt count to play, should be 0 when user trigger VP.
* @return     If the operation completed successfully, the return value is ID(should > 0) which use in stop procedure, otherwise 0 is play fail.
*/
uint16_t app_voice_prompt_play(uint32_t tone_idx, bool need_sync, uint32_t delay_time, app_vp_prio_t level, bool cleanup, app_vp_play_callback_t callback, uint32_t gpt_count);

/**
* @brief      This function require stop an VP by idx, no matter the VP is playing or not.
* @param[in]  id, a id return by app_voice_prompt_play.
* @param[in]  need_sync, it mean that VP need sync to partner at the same time.
* @param[in]  delay_time, after the time, agent and partner play at the same time.
* @return     If the operation completed successfully, return true, otherwise return false.
*/
bool app_voice_prompt_stop(uint16_t id, bool need_sync, uint32_t delay_time);

/**
* @brief      This function require play a voice number(incoming call number) or ringtone.
* @param[in]  tone_idx is a voice number or ringtone index which require play, see apps_config_vp_index_list.h.
* @param[in]  need_sync, it mean that vp need sync to partner at the same time.
* @param[in]  delay_time, after the time, agent and partner play at the same time.
* @param[in]  level, see app_vp_prio_t.
* @param[in]  ringtone, requirement is ringtone if true and loop play, otherwise it is voice number.
* @param[in]  repeat, ringtone or voice broadcast need to repeat play or not.
* @param[in]  callback, user callback.
* @param[in]  gpt_count, target gpt count to play, should be 0 when user trigger voice play.
* @return     If the operation completed successfully, the return value is ID which use in stop procedure.
*/
uint16_t app_voice_prompt_voice_play(uint32_t tone_idx, bool need_sync, uint32_t delay_time, app_vp_prio_t level, bool ringtone, bool repeat, app_vp_play_callback_t callback, uint32_t gpt_count);

/**
* @brief      This function require stop a voice number or ringtone.
* @param[in]  need_sync, it mean that VP need sync to partner at the same time.
* @param[in]  delay_time, after the time, agent and partner play at the same time.
* @param[in]  ringtone, stop ringtone if true, other stop current voice number and empty all voice number which are queueing.
* @return     If the operation completed successfully, return true, otherwise return false.
*/
bool app_voice_prompt_voice_stop(bool need_sync, uint32_t delay_time, bool ringtone);

/**
* @brief      This function get current play type. VP VC or ringtone.
* @return     Ref VP_TYPE_VP, VP_TYPE_VOICE, VP_TYPE_RINGTONE or VP_TYPE_NONE.
*/
app_vp_type app_voice_prompt_get_current_type();

/**
* @brief      This function get current play index.
* @return     Ref apps_config_vp_index_list.h, if fail, return 0xFFFF.
*/
uint16_t app_voice_prompt_get_current_index();

/**
* @brief      This function get current play id.
* @return     An id for stop procedure, 0x0 for nothing playing.
*/
uint16_t app_voice_prompt_get_current_id();

/**
* @brief      This function require set language for VP.
* @param[in]  langCodec, a language codec which want to set (app_vp_lang_codec_t), ref https://docs.microsoft.com/en-us/previous-versions/ms776294(v=vs.85).
* @param[in]  need_sync, it mean that VP need sync to partner at the same time.
* @return     If the operation completed successfully, return true, otherwise return false.
*/
bool app_voice_prompt_setLangByLCID(app_vp_lang_codec_t langCodec, bool need_sync);


/**
* @brief      This function require set language for VP.
* @param[in]  language index, a index in language support array.
* @param[in]  need_sync, it mean that VP need sync to partner at the same time.
* @return     If the operation completed successfully, return true, otherwise return false.
*/
bool app_voice_prompt_setLang(uint8_t index, bool need_sync);

/**
* @brief      This function require get language idx for VP.
* @return     If the operation completed successfully, return language index, otherwise return 0xFF.
*/
uint8_t app_voice_prompt_getLang();

/**
* @brief      This function require get count which support.
* @return     If the operation completed successfully, return language count, otherwise return 0x0.
*/
uint16_t app_voice_prompt_getLangCount();

/**
* @brief      This function require get count which support.
* @param[out] a buffer to storage language LICD.
* @return     If the operation completed successfully, return true, otherwise return false.
*/
bool app_voice_prompt_getSupportLang(uint16_t *buffer);


/**
* @brief      This function require set language for VP.
* @param[in]  sync count form agent.
* @return     void.
*/
void app_voice_prompt_sync_count(uint16_t);


/**
* @brief      This function require dump all queuing VP, VC or ringtone.
* @return     void.
*/
void app_voice_prompt_dump_all();


/**
* @brief      This function set sync_immediately flag. Only allow called before trigger a sync CONNECTED VP.
* @return     void.
*/
void app_voice_prompt_set_sync_immediat();

/**
* @brief      This function set same_level_preempt flag, which means the incoming VP can preempt same level previous VP.
* @return     void.
*/
void app_voice_prompt_set_preempt();

/**
* @brief      This function notify app_voice_prompt that remote device has been disconnect.
* @return     void.
*/
void app_voice_prompt_remote_disconnect();

#ifdef __cplusplus
}
#endif

#endif /* BT_SINK_APP_VOICE_PROMPT_H */

