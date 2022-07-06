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

#ifndef __ANC_CONTROL_API_H__
#define __ANC_CONTROL_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hal_audio.h"

#define PACKED  __attribute__((packed))

#ifndef MTK_ANC_V2
#define MTK_ANC_V2
#endif

/**
 * @addtogroup Audio
 * @{
 * @addtogroup anc_control
 * @{
 *
 * The ANC is used for active noise cancellation.
 * The Pass-through is used for ambient sound.
 *
 * @section anc_control_api_usage How to use this module
 *
 * - An example on how to use the ANC APIs.
 *  - 1. Get current ANC status or get support hybrid ANC capability. (Support for hybrid ANC capability fixes one project but not during the runtime change.)
 *  - 2. Enable ANC with filter_id and anc_type.
 *  - 3. Set runtime gain if user want to change. AUDIO_ANC_CONTROL_UNASSIGNED_GAIN can skip this action.
 *  - 4. Set/get ANC status in flash during system shutdown/boot-up.
 *    - Sample code:
 *     @code
 *         void anc_enable_demo()
 *        {
 *             uint8_t                                 anc_enable;
 *             audio_anc_control_filter_id_t   anc_current_filter_id;
 *             audio_anc_control_type_t        anc_current_type;
 *             int16_t                                  anc_runtime_gain;
 *             uint8_t                                  support_hybrid_enable;
 *             audio_anc_control_get_status(&anc_enable, &anc_current_filter_id, &anc_current_type, &anc_runtime_gain, &support_hybrid_enable, NULL);
 *
 *             audio_anc_control_result_t         control_ret;
 *             audio_anc_control_filter_id_t      target_filter_id;
 *             audio_anc_control_type_t           target_anc_type;
 *             if(support_hybrid_enable){
 *                 target_anc_type = AUDIO_ANC_CONTROL_TYPE_HYBRID;
 *              }else{
 *                 target_anc_type = AUDIO_ANC_CONTROL_TYPE_FF;
 *              }
 *              target_filter_id = AUDIO_ANC_CONTROL_ANC_FILTER_DEFAULT;
 *              control_ret = audio_anc_control_set_runtime_gain(AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, target_anc_type); // If user wants to change runtime gain, AUDIO_ANC_CONTROL_UNASSIGNED_GAIN can skip this action.
 *              control_ret = audio_anc_control_enable(target_filter_id, target_anc_type, NULL);
 *         }
 *
 *         void passthrough_enable_demo()
 *        {
 *             audio_anc_control_result_t         control_ret;
 *             audio_anc_control_filter_id_t      target_filter_id;
 *             audio_anc_control_type_t           target_anc_type;
 *             target_anc_type = AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF;
 *             target_filter_id = AUDIO_ANC_CONTROL_PASS_THRU_FILTER_DEFAULT;
 *             control_ret = audio_anc_control_set_runtime_gain(AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, target_anc_type); // If user wants to change runtime gain, AUDIO_ANC_CONTROL_UNASSIGNED_GAIN can skip this action.
 *             control_ret = audio_anc_control_enable(target_filter_id, target_anc_type, NULL);
 *         }
 *
 *         void anc_passthrough_disable_demo()
 *        {
 *             audio_anc_control_result_t         control_ret;
 *             control_ret = audio_anc_control_disable(NULL);
 *         }
 *
 *         void shutdown_system_anc_passthrough_disable_demo()
 *        {
 *             audio_anc_control_result_t         control_ret;
 *             uint8_t                                 anc_enable;
 *             audio_anc_control_filter_id_t   anc_current_filter_id;
 *             audio_anc_control_type_t        anc_current_type;
 *             int16_t                                  anc_runtime_gain;
 *             audio_anc_control_get_status(&anc_enable, &anc_current_filter_id, &anc_current_type, &anc_runtime_gain, NULL, NULL);
 *             control_ret = audio_anc_control_set_status_into_flash(anc_enable, anc_current_filter_id, anc_current_type, anc_runtime_gain, NULL);
 *             control_ret = audio_anc_control_disable(NULL);
 *         }
 *
 *         void bootup_system_anc_passthrough_enable_demo()
 *        {
 *             audio_anc_control_result_t         control_ret;
 *             uint8_t                                    anc_save_enable;
 *             audio_anc_control_filter_id_t      anc_save_filter_id;
 *             audio_anc_control_type_t           anc_save_type;
 *             int16_t                                    anc_save_runtime_gain;
 *             control_ret = audio_anc_control_get_status_from_flash(&anc_save_enable, &anc_save_filter_id, &anc_save_type, &anc_save_runtime_gain, NULL);
 *             if(anc_save_enable){
 *                 control_ret = audio_anc_control_set_runtime_gain(anc_save_runtime_gain, anc_save_type);
 *                 control_ret = audio_anc_control_enable(anc_save_filter_id, anc_save_type, NULL);
 *             }
 *         }
 *
 *         void set_extend_ramp_gain_control_for_wind_noise_detection_demo()
 *        {
 *             audio_anc_control_result_t                    control_ret;
 *             audio_anc_control_extend_ramp_cap_t    anc_extend_ramp_cap;
 *             if(wind_noise_trigger){
 *                 anc_extend_ramp_cap.extend_gain_1 = -6000;
 *                 anc_extend_ramp_cap.extend_gain_2 = AUDIO_ANC_CONTROL_UNASSIGNED_GAIN;
 *             }else{
 *                 anc_extend_ramp_cap.extend_gain_1 = 0;
 *                 anc_extend_ramp_cap.extend_gain_2 = AUDIO_ANC_CONTROL_UNASSIGNED_GAIN;
 *             }
 *             control_ret = audio_anc_control_set_extend_gain(AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE, &anc_extend_ramp_cap, NULL);
 *         }
 *     @endcode
 */

/** @defgroup anc_control_define Define
  * @{
  */

/** @brief The mask of the filter type. */
#define AUDIO_ANC_CONTROL_FILTER_TYPE_MASK         (0xF)
/** @brief Unassigning ANC gain. */
#define AUDIO_ANC_CONTROL_UNASSIGNED_GAIN          (0x7FFF)

/** @brief The atci cmd source of the audio_anc_control_command_handler. */
#define AUDIO_ANC_CONTROL_SOURCE_FROM_ATCI         (1)
/** @brief The race cmd source of the audio_anc_control_command_handler. */
#define AUDIO_ANC_CONTROL_SOURCE_FROM_RACE         (2)
/** @brief The audio manager source of the audio_anc_control_command_handler. */
#define AUDIO_ANC_CONTROL_SOURCE_FROM_AM           (3)
/** @brief The User trigger adaptive FF source of the audio_anc_control_command_handler. */
#define AUDIO_ANC_CONTROL_SOURCE_FROM_UTFF         (4)


/** @brief The all level of callback service level. */
#define AUDIO_ANC_CONTROL_CB_LEVEL_ALL             (0)
/** @brief The success only level of callback service level. */
#define AUDIO_ANC_CONTROL_CB_LEVEL_SUCCESS_ONLY    (1)

/** @brief The default filter number of enable ANC. */
#define AUDIO_ANC_CONTROL_ANC_FILTER_DEFAULT       (1)
/** @brief The default filter number of enable Pass-through. */
#define AUDIO_ANC_CONTROL_PASS_THRU_FILTER_DEFAULT (9)
/** @brief The default filter number of enable Pass-through in Sidetone use. */
#define AUDIO_ANC_CONTROL_PASS_THRU_SIDETONE_FILTER_DEFAULT (12)

/** @brief The max gain of dgain. */
#define AUDIO_ANC_CONTROL_DG_GAIN_MAX              (600)
/** @brief The max gain of ANC gain. */
#define AUDIO_ANC_CONTROL_GAIN_MAX                 (1800)
/** @brief The max gain of ANC ramp gain. */
#define AUDIO_ANC_CONTROL_RAMP_GAIN_MAX            (600)
/** @brief The min gain of ANC ramp gain. */
#define AUDIO_ANC_CONTROL_RAMP_GAIN_MIN            (-9000)

/**
  * @}
  */

/** @defgroup anc_enum Enum
  * @{
  */

/** @brief ANC and Pass-through control return values. */
typedef enum {
    AUDIO_ANC_CONTROL_EXECUTION_NONE        = -2,  /**< The anc process result was in initail status.   */
    AUDIO_ANC_CONTROL_EXECUTION_FAIL        = -1,  /**< The anc process result was fail.   */
    AUDIO_ANC_CONTROL_EXECUTION_SUCCESS     =  0,  /**< The anc process result was successful.   */
    AUDIO_ANC_CONTROL_EXECUTION_CANCELLED   =  1,  /**< The anc process result was cancelled.   */
    AUDIO_ANC_CONTROL_EXECUTION_NOT_ALLOWED =  2,  /**< The anc process result was not allowed.   */
} audio_anc_control_result_t;

/** @brief ANC and Pass-through control sub state. */
typedef enum {
    AUDIO_ANC_CONTROL_SUB_STATE_PROCESS_DONE    = 0,  /**< The anc process done sub-state.   */
    AUDIO_ANC_CONTROL_SUB_STATE_SETTING_PARAM   = 1,  /**< The anc setting param sub-state.   */
    AUDIO_ANC_CONTROL_SUB_STATE_WAITING_DISABLE = 2,  /**< The anc waiting enable sub-state for tws sync.   */
    AUDIO_ANC_CONTROL_SUB_STATE_WAITING_ENABLE  = 3,  /**< The anc waiting disable sub-state for tws sync.   */
} audio_anc_control_sub_state_t;

/** @brief ANC and Pass-through control event values. */
typedef enum {
    AUDIO_ANC_CONTROL_EVENT_ON                = 1 << 0,  /**< The event to enable anc.   */
    AUDIO_ANC_CONTROL_EVENT_OFF               = 1 << 1,  /**< The event to disable anc.   */
    AUDIO_ANC_CONTROL_EVENT_COPY_FILTER       = 1 << 2,  /**< The event to set anc filter coef.   */
    AUDIO_ANC_CONTROL_EVENT_SET_REG           = 1 << 3,  /**< The event to set anc param.   */
    AUDIO_ANC_CONTROL_EVENT_FORCE_OFF         = 1 << 4,  /**< The event to force off anc.   */
    AUDIO_ANC_CONTROL_EVENT_HOWLING           = 1 << 5,  /**< The event related to anc howling control.   */
    AUDIO_ANC_CONTROL_EVENT_SUSPEND_NOTIFY    = 1 << 6,  /**< The event related to anc suspend control.   */
    AUDIO_ANC_CONTROL_EVENT_RESUME_NOTIFY     = 1 << 7,  /**< The event related to anc resume control.   */
} audio_anc_control_event_t;

/** @brief ANC and Pass-through type values. */
typedef enum {
    AUDIO_ANC_CONTROL_TYPE_HYBRID       = 0,  /**< The hybrid anc type.   */
    AUDIO_ANC_CONTROL_TYPE_FF           = 1,  /**< The feedforward anc type.   */
    AUDIO_ANC_CONTROL_TYPE_FB           = 2,  /**< The feedback anc type.   */
    AUDIO_ANC_CONTROL_TYPE_USER_DEFINED = 3,  /**< The user defined anc type.   */
    AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF  = 4,  /**< The pass-through feedforward anc type.   */

    AUDIO_ANC_CONTROL_TYPE_DUMMY        = 0x7FFFFFFF,
} audio_anc_control_type_t;

/** @brief ANC and Pass-through filter number values. */
typedef enum {
    AUDIO_ANC_CONTROL_FILTER_ID_NONE,
    AUDIO_ANC_CONTROL_FILTER_ID_ANC_BEGIN       = 1,
    AUDIO_ANC_CONTROL_FILTER_ID_ANC_1           = AUDIO_ANC_CONTROL_FILTER_ID_ANC_BEGIN,        /**< The 1st filter number for anc filter behavior.   */
    AUDIO_ANC_CONTROL_FILTER_ID_ANC_2,                                                          /**< The 2nd filter number for anc filter behavior.   */
    AUDIO_ANC_CONTROL_FILTER_ID_ANC_3,                                                          /**< The 3rd filter number for anc filter behavior.   */
    AUDIO_ANC_CONTROL_FILTER_ID_ANC_4,                                                          /**< The 4th filter number for anc filter behavior.   */
    AUDIO_ANC_CONTROL_FILTER_ID_ANC_END,
    AUDIO_ANC_CONTROL_FILTER_ID_PASS_THRU_BEGIN = 9,
    AUDIO_ANC_CONTROL_FILTER_ID_PASS_THRU_1     = AUDIO_ANC_CONTROL_FILTER_ID_PASS_THRU_BEGIN,  /**< The 1st filter number for pass-through filter behavior.   */
    AUDIO_ANC_CONTROL_FILTER_ID_PASS_THRU_2,                                                    /**< The 2nd filter number for pass-through filter behavior.   */
    AUDIO_ANC_CONTROL_FILTER_ID_PASS_THRU_3,                                                    /**< The 3rd filter number for pass-through filter behavior.   */
    AUDIO_ANC_CONTROL_FILTER_ID_PASS_THRU_END,

    AUDIO_ANC_CONTROL_FILTER_FLASH_ID_DUMMY = 0x7F,
} audio_anc_control_filter_id_t;

/** @brief Set extend ramp gain types. */
typedef enum {
    AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_WIND_NOISE = 0,               /**< The wind detect type to set anc extend ramp gain control.   */
    AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_HOWLING_CONTROL,              /**< No use for now.   */
    AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_USER_UNAWARE,                 /**< The user unaware type to set anc extend ramp gain control.   */
    AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_ENVIRONMENT_DETECTION,        /**< The noise gate type to set anc extend ramp gain control.   */
    AUDIO_ANC_CONTROL_EXTEND_RAMP_TYPE_NUM,
} audio_anc_control_extend_ramp_gain_type_t;

/** @brief ANC and Pass-through set event values. */
typedef enum {
    AUDIO_ANC_CONTROL_SET_REG_DGAIN            = 1 << 0,             /**< The flag to set anc calibration gain.   */
    AUDIO_ANC_CONTROL_SET_REG_BIT_SHIFT        = 1 << 1,             /**< The flag to set anc bit shift.   */
    AUDIO_ANC_CONTROL_SET_REG_BQ_GAIN          = 1 << 2,             /**< The flag to set anc filter gain.   */
    AUDIO_ANC_CONTROL_SET_REG_RAMP_GAIN        = 1 << 3,             /**< The flag to set anc ramp gain.   */
    AUDIO_ANC_CONTROL_SET_REG_RAMP_GAIN_EXTEND = 1 << 4,             /**< The flag to set anc ramp gain extend function.   */
    AUDIO_ANC_CONTROL_SET_REG_RAMP_CAP         = 1 << 5,             /**< The flag to set anc ramp capability.   */
    AUDIO_ANC_CONTROL_SET_REG_LM_THD_L         = 1 << 6,             /**< The flag to set anc limiter 1st-threshold.   */
    AUDIO_ANC_CONTROL_SET_REG_LM_THD_H         = 1 << 7,             /**< The flag to set anc limiter 2nd-threshold.   */
    AUDIO_ANC_CONTROL_SET_REG_LM_AT_TIME       = 1 << 8,             /**< The flag to set anc limiter attack time.   */
    AUDIO_ANC_CONTROL_SET_REG_LM_RS_TIME       = 1 << 9,             /**< The flag to set anc limiter release time.   */
    AUDIO_ANC_CONTROL_SET_REG_DEQ_DLY_TIME     = 1 << 10,            /**< The flag to set anc deq delay time.   */
    AUDIO_ANC_CONTROL_SET_REG_REALTIME_UPDATE  = 1 << 11,            /**< The flag to real time update anc filter.   */
    AUDIO_ANC_CONTROL_SET_REG_DEBUG_MODE       = 1 << 12,            /**< The flag to trigger anc debug mode.   */
} audio_anc_control_set_reg_t;

/**
  * @}
  */

/** @defgroup anc_control_typedef Typedef
  * @{
  */

/** @brief The source of the audio_anc_control_command_handler. */
typedef uint8_t audio_anc_control_from_user_t;
/** @brief The callback service level. */
typedef uint8_t audio_anc_control_callback_level_t;
/** @brief The gain of ANC control. */
typedef int16_t audio_anc_control_gain_t;

/** @brief Define anc_control callback function prototype. The user should register a callback function when user register or deregister ANC callback service.
 *  @param[in] event_id is the value defined in #audio_anc_control_event_t. This parameter is given by the driver to notify the user about the data flow behavior.
 *  @param[in] result is the value defined in #audio_anc_control_result_t. This parameter is given by the driver to notify the user about the data flow behavior.
 */
typedef void (*audio_anc_control_callback_t)(audio_anc_control_event_t event_id, audio_anc_control_result_t result);

/**
  * @}
  */

/** @defgroup anc_control_struct Struct
  * @{
  */

/** @brief The support calibrate gain structure of the ANC path. */
typedef struct audio_anc_control_calibrate_gain_s {
    union {
        struct {
            audio_anc_control_gain_t  gain_index_ff_l;  /**< Calibrate gain to L-ch feedforward anc.   */
            audio_anc_control_gain_t  gain_index_fb_l;  /**< Calibrate gain to L-ch feedback anc.   */
            audio_anc_control_gain_t  gain_index_ff_r;  /**< Calibrate gain to R-ch feedforward anc.   */
            audio_anc_control_gain_t  gain_index_fb_r;  /**< Calibrate gain to R-ch feedback anc.   */
        };
        uint32_t val;
    };
} audio_anc_control_calibrate_gain_t;

/** @brief The support ramp capability structure of the ANC path. */
typedef struct audio_anc_control_ramp_cap_s {
    uint8_t delay_time;         /**< Delay time for ramp capability.   */
    uint8_t ramp_up_dly_step;   /**< Ramp up delay step for ramp capability.   */
    uint8_t ramp_dn_dly_step;   /**< Ramp down delay step for ramp capability.   */
} audio_anc_control_ramp_cap_t;

/** @brief The support extend ramp capability structure of the ANC path. */
typedef struct audio_anc_control_extend_ramp_cap_s {
    uint8_t target_device;                                /**< No use for now.   */
    audio_anc_control_extend_ramp_gain_type_t gain_type;  /**< The type to set anc extend ramp gain control.   */
    audio_anc_control_gain_t extend_gain_1;               /**< Extend ramp gain_1 value for setting.   */
    audio_anc_control_gain_t extend_gain_2;               /**< Extend ramp gain_2 value for setting.   */
} audio_anc_control_extend_ramp_cap_t;

/** @brief The support change settings capability structure of the ANC path. */
typedef union {
    audio_anc_control_gain_t     Dgain;                 /**< Calibrate gain value for setting parameter.   */
    uint8_t                      bit_shift;             /**< Bit shift value for setting parameter.   */
    audio_anc_control_gain_t     bq_gain;               /**< Filter gain value for setting parameter.   */
    audio_anc_control_gain_t     ramp_gain;             /**< Ramp gain value for setting parameter.   */
    audio_anc_control_ramp_cap_t ramp;                  /**< Ramp capability structure for setting parameter.   */
    uint32_t                     limiter_threshold;     /**< Limiter threshold value for setting parameter.   */
    uint32_t                     limiter_attack_time;   /**< Limiter attack time value for setting parameter.   */
    uint32_t                     limiter_release_time;  /**< Limiter release time value for setting parameter.   */
    uint8_t                      deq_delay;             /**< Deq delay time value for setting parameter.   */
    uint8_t                      realtime_update_ch;    /**< Real time update anc filter channel value for setting parameter.   */
    audio_anc_control_extend_ramp_cap_t extend_ramp_cap; /**< Extend ramp capability structure for setting parameter.   */
}audio_anc_control_reg_param_union;

/** @brief The support filter capability structure of the ANC path. */
typedef struct audio_anc_control_filter_cap_s {
    audio_anc_control_type_t          type;            /**< Anc filter type.   */
    uint8_t                           filter_id;       /**< Anc filter number.   */
    uint8_t                           sram_bank;       /**< Sram bank in anc filter.   */
    uint32_t                          filter_mask;     /**< Filter mask in anc path.   */
    uint32_t                          sram_bank_mask;  /**< Sram bank mask in anc path.   */
    audio_anc_control_set_reg_t       set_reg_mask;    /**< Set event mask in setting parameter flow.   */
    void                              *filter_coef;    /**< Pointer to get anc filter information.   */
    audio_anc_control_reg_param_union param;           /**< Set param structure in setting parameter flow.   */
} audio_anc_control_filter_cap_t;

/** @brief The support misc settings capability structure of the ANC path. */
typedef struct audio_anc_control_misc_s {
    uint8_t  disable_with_suspend;                     /**< Extend use. Dummy disable flag to suspend ANC performance.   */
    uint8_t  enable_with_resume;                       /**< Extend use. Dummy enable flag to resume ANC performance.   */
    audio_anc_control_sub_state_t sub_state;           /**< Extend use. Get the sub state in ANC sub state machine.   */
    audio_anc_control_type_t      enable_type;         /**< Extend use. Get the target type when sub status was AUDIO_ANC_CONTROL_SUB_STATE_WAITING_ENABLE.   */
    uint32_t extend_use_parameters;                    /**< Extend use.   */
    uint32_t target_gpt_count;                         /**< Extend use.   */
} audio_anc_control_misc_t;

/** @brief The support whole settings capability structure of the ANC path. */
typedef struct audio_anc_control_cap_s {
    uint8_t                         target_device;       /**< Target device in anc control flow.   */
    audio_anc_control_filter_cap_t  filter_cap;          /**< Anc filter capability in anc control flow.    */
    audio_anc_control_misc_t        control_misc;        /**< Extend use structure in anc control flow */
} audio_anc_control_cap_t;
/**
  * @}
  */

/**
 * @brief     This function is used to initial ANC module.
 * @return    Return AUDIO_ANC_CONTROL_EXECUTION_SUCCESS if the operation is successful. Otherwise, error occurs.
 * @sa        #audio_anc_control_init()
 */
audio_anc_control_result_t audio_anc_control_init(void);

/**
 * @brief     This function is used to enable ANC path.
 * @param[in] filter_id is the number of ANC filter nvkey which include filter information.
 * @param[in] type is the ANC path type which user want to be enabled.
 * @param[in] *control_misc is the other parameter which user want to be set.
 * @return    Return AUDIO_ANC_CONTROL_EXECUTION_SUCCESS if the operation is successful. Otherwise, error occurs.
 * @sa        #audio_anc_control_enable()
 */
audio_anc_control_result_t audio_anc_control_enable(audio_anc_control_filter_id_t filter_id, audio_anc_control_type_t type, audio_anc_control_misc_t* control_misc);

/**
 * @brief     This function is used to disable ANC path.
 * @param[in] *control_misc is the other parameter which user wants to be set.
 * @return    Return AUDIO_ANC_CONTROL_EXECUTION_SUCCESS if the operation is successful. Otherwise, error occurs.
 * @sa        #audio_anc_control_disable()
 */
audio_anc_control_result_t audio_anc_control_disable(audio_anc_control_misc_t* control_misc);

/**
 * @brief     This function is used to register callback function in ANC callback service.
 * @param[in] callback is the callback function address which want to be call in ANC callback service.
 * @param[in] event_mask is the event mask in ANC callback service.
 * @param[in] level is the level mask in ANC callback service.
 * @return    Return AUDIO_ANC_CONTROL_EXECUTION_SUCCESS if the operation is successful. Otherwise, error occurs.
 * @sa        #audio_anc_control_register_callback()
 */
audio_anc_control_result_t audio_anc_control_register_callback(audio_anc_control_callback_t callback, audio_anc_control_event_t event_mask, audio_anc_control_callback_level_t level);

/**
 * @brief     This function is used to deregister callback function in ANC callback service.
 * @param[in] callback is the callback function address which want to be deregister in ANC callback service.
 * @return    Return AUDIO_ANC_CONTROL_EXECUTION_SUCCESS if the operation is successful. Otherwise, error occurs.
 * @sa        #audio_anc_control_deregister_callback()
 */
audio_anc_control_result_t audio_anc_control_deregister_callback(audio_anc_control_callback_t callback);

/**
 * @brief     This function is used to get ANC status which was restore in nvkey.
 * @param[out] *enable would be the enable/disable status in nvkey.
 * @param[out] *filter_id would be the number of ANC filter in nvkey.
 * @param[out] *type would be ANC path type in nvkey.
 * @param[out] *runtime_gain would be the ANC ramp gain in nvkey.
 * @param[out] *control_misc would be the other parameter in nvkey.
 * @return    Return AUDIO_ANC_CONTROL_EXECUTION_SUCCESS if the operation is successful. Otherwise, error occurs.
 * @sa        #audio_anc_control_get_status_from_flash()
 */
audio_anc_control_result_t audio_anc_control_get_status_from_flash(uint8_t *enable, audio_anc_control_filter_id_t *filter_id, audio_anc_control_type_t *type, audio_anc_control_gain_t *runtime_gain, audio_anc_control_misc_t* control_misc);

/**
 * @brief     This function is used to save ANC status which into nvkey.
 * @param[in] enable would be the enable/disable status saved into nvkey.
 * @param[in] filter_id would be the number of ANC filters saved into nvkey.
 * @param[in] type would be ANC path type saved into nvkey.
 * @param[in] runtime_gain would be the ANC ramp gain saved into nvkey.
 * @param[in] *control_misc would be the other parameter saved into nvkey.
 * @return    Return AUDIO_ANC_CONTROL_EXECUTION_SUCCESS if the operation is successful. Otherwise, error occurs.
 * @sa        #audio_anc_control_set_status_into_flash()
 */
audio_anc_control_result_t audio_anc_control_set_status_into_flash(uint8_t enable, audio_anc_control_filter_id_t filter_id, audio_anc_control_type_t type, audio_anc_control_gain_t runtime_gain, audio_anc_control_misc_t* control_misc);

/**
 * @brief     This function is used to get ANC current status.
 * @param[out] *enable would be the enable/disable current status.
 * @param[out] *filter_id would be the number of ANC filter in current status.
 * @param[out] *type would be ANC path type in current status.
 * @param[out] *runtime_gain would be the ANC ramp gain in current status.
 * @param[out] *support_hybrid would be the support hybrid ANC capability in current project.
 * @param[out] *control_misc would be the other parameter in current status.
 * @return    none.
 * @sa        #audio_anc_control_get_status()
 */
void audio_anc_control_get_status(uint8_t *enable, audio_anc_control_filter_id_t *filter_id, audio_anc_control_type_t *type, int16_t *runtime_gain, uint8_t *support_hybrid, audio_anc_control_misc_t *control_misc);

/**
 * @brief     This function is used to get the ramp gain when ANC was active.
 * @param[out] *runtime_gain would be the ANC ramp gain in current status.
 * @param[in] type is whitch ANC type user want to get the runtime gain is.
 * @return    Return AUDIO_ANC_CONTROL_EXECUTION_SUCCESS if the operation is successful. Otherwise, error occurs.
 * @sa        #audio_anc_control_get_runtime_gain()
 */
audio_anc_control_result_t audio_anc_control_get_runtime_gain(audio_anc_control_gain_t *runtime_gain, audio_anc_control_type_t type);

/**
 * @brief     This function is used to set the ramp gain in ANC path.
 * @param[in] runtime_gain is the ANC ramp gain whitch user wants to be set.
 * @param[in] type is whitch ANC type user want to set the runtime gain is.
 * @return    Return AUDIO_ANC_CONTROL_EXECUTION_SUCCESS if the operation is successful. Otherwise, error occurs.
 * @sa        #audio_anc_control_set_runtime_gain()
 */
audio_anc_control_result_t audio_anc_control_set_runtime_gain(audio_anc_control_gain_t runtime_gain, audio_anc_control_type_t type);

/**
 * @brief     This function is used to set the extend ramp gain in ANC path.
 * @param[in] gain_type is whitch ANC extend ramp type user wants to set the extend ramp gain is.
 * @param[in] *gain_cap is the ANC extend ramp gain capability which user wants to be set.
 * @param[in] *control_misc would be the other parameter for extend use.
 * @return    Return AUDIO_ANC_CONTROL_EXECUTION_SUCCESS if the operation is successful. Otherwise, an error occurs.
 * @sa        #audio_anc_control_set_extend_gain()
 */
audio_anc_control_result_t audio_anc_control_set_extend_gain(audio_anc_control_extend_ramp_gain_type_t gain_type, audio_anc_control_extend_ramp_cap_t* gain_cap, audio_anc_control_misc_t *control_misc);

/**
 * @brief     This function is used to get the calibrate gain mask in nvkey.
 * @param[out] *gain_mask would be the calibrate gain in nvkey.
 * @return    Return AUDIO_ANC_CONTROL_EXECUTION_SUCCESS if the operation is successful. Otherwise, error occurs.
 * @sa        #audio_anc_control_get_calibrate_gain_from_flash()
 */
audio_anc_control_result_t audio_anc_control_get_calibrate_gain_from_flash(audio_anc_control_calibrate_gain_t *gain_mask);

/**
 * @brief     This function is used to set the calibrate gain mask into nvkey.
 * @param[in] gain_mask is the calibrate gain mask which want to be set into nvkey.
 * @return    Return AUDIO_ANC_CONTROL_EXECUTION_SUCCESS if the operation is successful. Otherwise, error occurs.
 */
audio_anc_control_result_t audio_anc_control_set_calibrate_gain_into_flash(audio_anc_control_calibrate_gain_t gain_mask);

/**
 * @brief     This function is used to set the calibrate gain mask into ANC path.
 * @param[in] gain_mask is the calibrate gain mask which want to be set into ANC path.
 * @return    Return AUDIO_ANC_CONTROL_EXECUTION_SUCCESS if the operation is successful. Otherwise, error occurs.
 */
audio_anc_control_result_t audio_anc_control_set_calibrate_gain(audio_anc_control_calibrate_gain_t gain_mask);

/**
 * @brief     This function is used to get status of the ANC attach request in MP calibration flow.
 * @return    Return true if this device opens ANC attach. Otherwise, this device does not open ANC attach.
 */
uint8_t audio_anc_control_get_attach_enable(void);

/**
 * @brief     This function is used to set status of the ANC attach request in MP calibration flow.
 * @param[in] enable is the request whitch user wants to open ANC attach or not.
 * @return    none.
 */
void audio_anc_control_set_attach_enable(uint8_t enable);

/**
 * @brief     This function is used to get sync time of the ANC sync service.
 * @return    Return current sync time.
 */
uint16_t audio_anc_control_get_sync_time(void);

/**
 * @brief     This function is used to set sync time of the ANC sync service.
 * @param[in] sync_time is the delay time whitch user wants to set in ANC sync service.
 * @return    none.
 */
void audio_anc_control_set_sync_time(uint16_t sync_time);

/**
 * @brief     This function re-initializes the nvkey memory for ANC and pass-through.
 * @return    Return AUDIO_ANC_CONTROL_EXECUTION_SUCCESS if the operation is successful. Otherwise, error occurs.
 */
audio_anc_control_result_t audio_anc_control_reinit_nvdm(void);

#ifdef MTK_ANC_ENABLE
audio_anc_control_result_t audio_anc_control_command_handler(audio_anc_control_from_user_t source, audio_anc_control_event_t event, void* command);
#endif

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/
#endif  /*__ANC_CONTROL_API_H__*/

