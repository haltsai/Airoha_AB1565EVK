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

#ifndef __HAL_CAPTOUCH_INTERNAL_H__
#define __HAL_CAPTOUCH_INTERNAL_H__

#include "hal_captouch.h"
#ifdef HAL_CAPTOUCH_MODULE_ENABLED
#include "ept_keypad_drv.h"
#include "FreeRTOS.h"
#include "timers.h"

#ifdef __cplusplus
extern "C"
{
#endif

//#define CAPTOUCH_DEBUG_1

#define cap_reg32(x)    (*(volatile uint32_t*)(x))


#ifdef DRV_KBD_CAPTOUCH_SEL
#define CAPTOUCH_USED_CHANNEL_MASK      DRV_KBD_CAPTOUCH_SEL     /*indicate the used channel by ept*/
#else
#define CAPTOUCH_USED_CHANNEL_MASK      0xff                     /*indicate the used channel by default*/
#endif

#define CAPTOUCH_BUFFER_SIZE            (64)    /* key position buffer size */

#define CAPTOUCH_FINE_CAP_MAX           (63)   /* the fine cap max value */
#define CAPTOUCH_COARSE_CAP_MAX         (0x7)   /* the coarse cap max value */
#define CAPTOUCH_MAVG_MAX               (0xF)   /* the mavg max value */
#define CAPTOUCH_AVG_MAX                (0x7)   /* the avg max value */

#define captouch_enable                 (true)
#define captouch_disable                (false)
#define EAR_DETECT_DISABLE              (0xff)


/* TOUCH_CON0 */
#define TOUCH_INT_EN                    (1<<9)
#define TOUCH_WAKE_EN                   (1<<10)
/* TOUCH_CON1 */
#define TOUCH_BACK2BACK_EN              (1<<17)
#define TOUCH_AUTO_DISABLE_CH           (1<<18)
#define TOUCH_PER_CH_GATING_EN          (1<<20)      //Enable per-channel power saving.
/* TOUCH_ANACFG1 */
#define TOUCH_ADC_EN                    (1<<8)
#define TOUCH_OP_EN                     (1<<12)
#define TOUCH_LDO_EN                    (1<<13)
/* TOUCH_BASELINE */
#define TOUCH_NTHRH_EN                  (1<<31)
#define TOUCH_NTHRL_EN                  (1<<30)
#define TOUCH_THRH_EN                   (1<<29)
#define TOUCH_THRL_EN                   (1<<28)
/* TOUCH_BASELINE */
#define TOUCH_LPWU_INT_EN               (1<<9)
#define TOUCH_LPWU_WAKE_EN              (1<<10)
/* TOUCH_LPSDCON */
#define TOUCH_LPSD_MASK                 (1<<16)
/* TOUCH_LPWUCON */
#define TOUCH_LPWU_MASK                 (1<<16)
#define BIT_FIELD_CLEAR32(r,p,l)    (((uint32_t)(r)) & ~((((uint32_t)1 << (l)) - 1) << (p)))
#define BIT_FIELD_EXTRACT32(r,p,l)  (((uint32_t)(r) >> (p)) & (((uint32_t)1 << (l)) - 1))
#define BIT_FIELD_INSERT32(r,p,l,v) (BIT_FIELD_CLEAR32((r),(p),(l)) | ((uint32_t)(v) << (p)))

#define CAPTOUCH_CHANNEL_MAX 4

/* Long press time unit  32k_clock_period.*/
#define LONGPRESS_TIME_S 32768

#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif

typedef struct {
    hal_captouch_event_t data[CAPTOUCH_BUFFER_SIZE];
    uint32_t write_index;
    uint32_t read_index;
    uint32_t press_count;
} captouch_buffer_t;

typedef struct {
    bool       state;
    uint8_t   channel;
    uint32_t   time_stamp;
} captouch_event_t;

typedef struct {
    captouch_event_t captouch_event[CAPTOUCH_CHANNEL_MAX];
} captouch_event_temp_t;

typedef struct {
    bool        has_initilized;
    bool        is_running;
    uint32_t    used_channel_map;
    int16_t     fine_cap_base[4];
    uint8_t     mavg_r[4];
    uint8_t     avg_s[4];
    uint8_t     swDebounceTime;
    int16_t     debounce_state[4];
    uint8_t     noise_cnt_race[4];
    int16_t     highThre[4];
    int16_t     lowThre[4];
    hal_captouch_callback_context_t captouch_callback;
#if (PRODUCT_VERSION == 2822)
    uint8_t     swDebounceTime_Rls;
#endif
} captouch_context_t;

typedef struct {
    bool      stop_avg ;
    uint8_t   detect_ch;
    uint8_t   earBaseKFrozen;
    uint8_t   earinstatus;
    uint8_t   baseKFrozeTime;
    int16_t   ear_detect_base[CAPTOUCH_CHANNEL_MAX];
    uint16_t  mavg_num;
    int32_t   fine_sum;
} captouch_ear_detect_t;


typedef struct {
  uint8_t channel;
  uint8_t delay_count;
} captouch_autosuspend_event_t;

typedef struct {
    captouch_autosuspend_event_t captouch_autosuspend_event[CAPTOUCH_CHANNEL_MAX];
    uint8_t ch_bitmap;
    uint8_t time[CAPTOUCH_CHANNEL_MAX];
} captouch_autosuspend_timeout_context_t;



//KeyID 0xD101
typedef struct {
    bool     is_key[4];
    bool     is_rs[4];
    bool     is_thr[4];
    uint8_t  hw_ch_map;
    uint8_t  mavg_r[4];
    uint8_t  avg_s[4];
    uint8_t  en_ch_map;
    uint8_t  coarse_cap[4];
    int16_t  thr_h[4];
    int16_t  thr_l[4];
    int8_t   fine_cap[4];
    uint8_t  swtune_en;
    uint8_t  swDebounceTime;
    uint8_t  reserve[2];

}PACKED hal_captouch_nvdm_data;


//KeyID 0xD102
typedef struct
{
    uint8_t sdwu_en;
    uint8_t sdtime;
    uint8_t wutime;
}PACKED hal_captouch_sdwu_nvdm_data;


////KeyID 0xD103
typedef struct
{
    uint8_t  IsCapTouchEnable;
}PACKED hal_captouch_feature_nvdm_data;

////KeyID 0xD104
typedef struct
{
    uint8_t  detect_ch;
    int16_t  earCheckBase[4];
    uint8_t  BaseAvgNum;
    uint8_t  baseKFrozeTime;
    uint8_t  reserved[5];
}PACKED hal_captouch_eardetect_nvdm_data;

////KeyID 0xD105
typedef struct
{
    uint8_t enable_ch;
    int16_t pre_thr;
    int16_t rel_thr;
    int16_t lev_thr;
    int16_t cba_thr;
    int16_t ini_delay;
    uint8_t  reserved;
}PACKED hal_captouch_forcetouch_nvdm_data;


////KeyID 0xD106
typedef struct
{
    uint8_t coarseCapValue[4];
    int8_t  fineCap[4];
    int16_t earCheckBase[4];
}PACKED hal_captouch_tune_nvdm_data;

////KeyID 0xD107
typedef struct
{
    uint8_t  lpsd_ch_bitmap;
    uint8_t  ch_bitmap;
    uint8_t  lpwu_ch_bitmap;
    uint8_t  time[4];
    uint8_t  reserved[1];
}PACKED hal_captouch_autosuspend_timeout_nvdm_data;

typedef enum {
    EAR_BAKSEK_FROZEN_INIT  = 0,
    EAR_BAKSEK_FROZEN_START = 1,
    EAR_BAKSEK_FROZEN_CLOSE = 2
} captouch_earBaseKFrozen_condition_t;

typedef enum {
    CAPTOUCH_IS_KEY_ID = 0,
    CAPTOUCH_IS_RS_ID,
    CAPTOUCH_IS_THR_ID,
    CAPTOUCH_HW_CH_MAP_ID,
    CAPTOUCH_MAVG_R_ID,
    CAPTOUCH_AVG_s_ID,
    CAPTOUCH_EN_CH_MAP_ID,
    CAPTOUCH_COARSE_CAP_ID,
    CAPTOUCH_THR_H_ID,
    CAPTOUCH_THR_L_ID,
    CAPTOUCH_FINE_CAP_ID,
    CAPTOUCH_SW_TUNE_ID,
    CAPTOUCH_SW_DEB_TIME_ID,
} hal_captouch_nvdm_id_t;


typedef enum {
    SEND_RELEASSE  = 0,
    SEND_PRESS     = 1
} hal_captouch_send_type_t;


/********* varible extern *************/
//extern CAPTOUCH_REGISTER_T *captouch ;
extern captouch_context_t captouch_context;
extern captouch_buffer_t captouch_buffer;
extern const uint8_t captouch_mapping_keydata[];
extern CAPTOUCH_REGISTER_T *captouch;
extern CAPTOUCH_REGISTERHIF_T *captouch_hif ;
/******** funtion extern **************/
void captouch_push_one_event_to_buffer(uint32_t channel,hal_captouch_key_state_t state,uint32_t time_stamp);
void captouch_pop_one_event_from_buffer(hal_captouch_event_t *key_event);
uint32_t captouch_get_buffer_left_size(void);
uint32_t captouch_get_buffer_data_size(void);
bool captouch_get_event_from_fifo(uint32_t *event,uint32_t *timestap);
void captouch_interrupt_handler(hal_nvic_irq_t irq_number);
void captouch_call_user_callback(void);
bool captouch_lpsd_handler(void);
void captouch_register_nvic_callback(void);
void captouch_clk_control(bool is_clock_on);
void captouch_set_clk(hal_captouch_lowpower_type_t power_type,hal_captouch_clock_t clock);
void captouch_set_mavg(hal_captouch_channel_t channel,uint8_t mavg);
void captouch_set_avg(hal_captouch_channel_t channel,uint8_t avg);
void captouch_set_fine_cap(hal_captouch_channel_t channel,int8_t fine_tune);
void captouch_set_coarse_cap(hal_captouch_channel_t channel, uint8_t coarse_tune);
uint8_t captouch_get_coarse_cap(hal_captouch_channel_t channel);
void captouch_set_threshold(hal_captouch_channel_t channel,int32_t high_thr, int32_t low_thr);
void captouch_set_nthreshold(hal_captouch_channel_t channel,int32_t high_thr, int32_t low_thr);
void captouch_set_dynamic_threshold(bool nthr_en,bool thr_en,int16_t rangeH, int16_t rangeL);
void captouch_set_control_manual(hal_captouch_channel_t channel,bool is_auto);
void captouch_set_autok_suspend(uint8_t channel_bit_map, bool en);
void captouch_set_autok_Nsuspend(uint8_t channel_bit_map, bool en);
void captouch_channel_sense_control(uint8_t channel_bit_map, bool en);
void captouch_channel_sensing_control(uint8_t channel_bit_map, bool en);
void captouch_int_control(bool en);
void captouch_longpress_int_control(bool en);
void captouch_channel_int_control(uint8_t channel_bit_map, bool en);
void captouch_wakeup_setting(uint8_t channel_bit_map, bool en);
void captouch_longpress_channel_control(hal_captouch_longpress_type_t type, uint32_t count);
void captouch_longpress_channel_select_control(hal_captouch_longpress_type_t type,uint8_t channel_bit_map,uint32_t count);
void captouch_switch_debug_sel(hal_captouch_channel_t channel);
int16_t captouch_to16signed(uint16_t bit, int16_t data);
void captouch_get_tune_state(hal_captouch_channel_t channel,hal_captouch_tune_data_t *tune_data);
bool captouch_sw_auto_tune(hal_captouch_channel_t channel, hal_captouch_tune_data_t *tune_data);
bool captouch_hw_auto_tune(hal_captouch_channel_t channel, hal_captouch_tune_data_t *tune_data );
bool captouch_rtc_clk_control(bool is_en);
bool captouch_rtc_lpm_control(hal_captouch_lowpower_type_t lowpower_type);
void captouch_analog_init(void);
void captouch_analog_deinit(void);
void captouch_set_lpsd_chk_vbus(bool is_en);
void captouch_set_vbus_flag(bool is_chgin);
void captouch_get_tune_delay_time(uint8_t clk_k, uint8_t mavg_r);
void captouch_get_det_delay_time(hal_captouch_config_t *config, uint8_t clk_k);
void captouch_find_baseline(hal_captouch_channel_t channel);
bool captouch_get_channel_trigger(hal_captouch_channel_t channel);
bool captouch_get_control_manual_state(hal_captouch_channel_t channel);
void captouch_findbase_after_ctrl_man(void );
void captouch_fine_tune_delay_handler(TimerHandle_t xTimer);
int8_t captouch_write_nvkey(hal_captouch_nvdm_id_t id ,hal_captouch_channel_t channel ,int16_t value );
int8_t captouch_get_fine_cap(hal_captouch_channel_t channel);
bool captouch_intr_bounce_check(hal_captouch_channel_t channel, uint32_t *pre_time_stamp , uint32_t time_stamp );
void captouch_key_press_event_handler( void  *user_data);
void captouch_channel_debounce_check(hal_captouch_channel_t channel, hal_captouch_key_state_t state, uint32_t time_stamp);
void captouch_key_release_event_handler(TimerHandle_t channel_timer);
void captouch_protect_buffer(hal_captouch_channel_t channel ,bool *buffer_flag ,hal_captouch_send_type_t type );
bool captouch_get_lpwu_int_flag(void);
bool captouch_get_charge_in_status(void);
bool captouch_is_feature_enable(void);
uint8_t captouch_sdwusetting_from_NVKEY(void);
uint8_t captouch_coarsecap_from_nvkey(uint8_t channel);
int8_t captouch_coarsecap_to_nvkey(uint8_t channel ,uint8_t value );
int8_t captouch_fine_cap_from_nvkey(uint8_t channel);
int8_t captouch_fine_cap_to_nvkey(uint8_t channel,int8_t value);
int16_t  captouch_get_avg_adc(hal_captouch_channel_t channel);
int16_t  captouch_get_mavg_adc(hal_captouch_channel_t channel);
uint8_t captouch_eardetect_enable(void);
void captouch_eardetect_base_to_nvkey(uint8_t channel,int16_t value);
int16_t captouch_eardetect_base_from_nvkey(uint8_t channel);
void captouch_sw_ear_check(TimerHandle_t xTimer);
void captouch_stop_earcheck_basek_timeout(TimerHandle_t xTimer);
int16_t captouch_round(int32_t val1, int16_t val2);
bool captouch_autosuspend_timeout_enable(void);
void captouch_close_autosuspend_timeout_handler( void *user_data);
void captouch_close_autosuspend_timeout(uint8_t channel);
void captouch_open_autosuspend_timeout_handler(void *user_data);
void captouch_init_parameter_print(bool capcon_state, bool lpwu_flag, bool isChargerIn, uint8_t ear_detect_ch, bool autosuspend_timeout_en);
void captouch_send_press_msg(void);
uint8_t captouch_set_auto_tune_feature(uint8_t en);

#if (PRODUCT_VERSION == 2822)
void captouch_key_event_handler(void *user_data);
void captouch_set_debounce_timer(uint8_t time_ms_p, uint8_t time_ms_r);
#endif

#ifdef __cplusplus
}
#endif

#endif /* HAL_CAPTOUCH_MODULE_ENABLED */

#endif /* __HAL_CAPTOUCH_INTERNAL_H__ */

