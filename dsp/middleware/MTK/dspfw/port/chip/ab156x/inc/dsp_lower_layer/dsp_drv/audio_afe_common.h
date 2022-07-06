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

#ifndef __DSP_DRV_AFE_COMMON_H__
#define __DSP_DRV_AFE_COMMON_H__

#include "types.h"
#include "sink.h"
#include "source.h"
#if (PRODUCT_VERSION == 2822)
#include "mt2822.h"
#endif
#if (PRODUCT_VERSION == 1565)
#include "ab156x.h"
#endif
#include "hal_nvic.h"
#include "hal_audio.h"
#include "hal_audio_afe_control.h"

#define PLAY_EN_DELAY_TOLERENCE 500
#define PLAY_EN_TRIGGER_REINIT_MAGIC_NUM 0xAB
#define PLAY_EN_REINIT_DONE_MAGIC_NUM 0xFF


typedef struct audio_sink_pcm_ops_s {
    int32_t (*probe)(SINK sink);
    int32_t (*open)(SINK sink);
    int32_t (*close)(SINK sink);
    int32_t (*hw_params)(SINK sink);
    int32_t (*trigger)(SINK sink, int cmd);
    int32_t (*copy)(SINK sink, void *buf, uint32_t count);
} audio_sink_pcm_ops_t;

typedef struct audio_source_pcm_ops_s{
    int32_t (*probe)(SOURCE source);
    int32_t (*open)(SOURCE source);
    int32_t (*close)(SOURCE source);
    int32_t (*hw_params)(SOURCE source);
    int32_t (*trigger)(SOURCE source, int cmd);
    int32_t (*copy)(SOURCE source, void *buf, uint32_t count);
} audio_source_pcm_ops_t;

typedef struct audio_pcm_ops_s{
    int32_t (*probe)(void *para);
    int32_t (*open)(void *para);
    int32_t (*close)(void *para);
    int32_t (*hw_params)(void *para);
    int32_t (*trigger)(void *para, int cmd);
    int32_t (*copy)(void *para, void *buf, uint32_t count);
} audio_pcm_ops_t, *audio_pcm_ops_p;


typedef struct  {
    const audio_sink_pcm_ops_t *sink_ops;
    const audio_source_pcm_ops_t *source_ops;
} afe_platform_ops_t;

typedef enum {
    AFE_SOURCE,
    AFE_SINK,
    AFE_SINK_VP,
} afe_stream_type_t;

/*hook source/ sink operation function*/
void audio_afe_set_sink_ops(hal_audio_device_t device);
void audio_afe_set_source_ops(hal_audio_device_t device);
#ifdef MTK_PROMPT_SOUND_ENABLE
void audio_afe2_set_sink_ops(hal_audio_device_t device);
#endif

/*audio operation */
int32_t audio_ops_probe(void *param);
int32_t audio_ops_hw_params(void *param);
int32_t audio_ops_open(void *param);
bool audio_ops_close(void *param);
int32_t audio_ops_trigger(void *param, int cmd);
bool audio_ops_copy(void *param, void *src, uint32_t count);
void audio_afe_set_ops(void *param);




uint32_t afe_get_dl1_query_data_amount(void);
#ifdef MTK_PROMPT_SOUND_ENABLE
void afe_dl2_query_data_amount(uint32_t *sink_data_count, uint32_t *afe_sram_data_count);
#endif
#if 0
/*sink operation */
int32_t audio_sink_ops_probe(SINK sink);
int32_t audio_sink_ops_hw_params(SINK sink);
int32_t audio_sink_ops_open(SINK sink);
bool audio_sink_ops_close(SINK sink);
int32_t audio_sink_ops_trigger(SINK sink, int cmd);
bool audio_sink_ops_copy(SINK sink, void *src, uint32_t count);
/*source operation */
int32_t audio_source_ops_probe(SOURCE source);
int32_t audio_source_ops_hw_params(SOURCE source);
int32_t audio_source_ops_open(SOURCE source);
bool audio_source_ops_close(SOURCE source);
int32_t audio_source_ops_trigger(SOURCE source, int cmd);
bool audio_source_ops_copy(SOURCE source, void *src, uint32_t count);
#ifdef MTK_PROMPT_SOUND_ENABLE
int32_t audio_sink_vp_ops_probe(SINK sink);
int32_t audio_sink_vp_ops_hw_params(SINK sink);
int32_t audio_sink_vp_ops_open(SINK sink);
bool audio_sink_vp_ops_close(SINK sink);
int32_t audio_sink_vp_ops_trigger(SINK sink, int cmd);
bool audio_sink_vp_ops_copy(SINK sink, void *src, uint32_t count);
#endif
#endif
/*misc*/
uint32_t word_size_align(uint32_t in_size);
void afe_sink_prefill_silence_data(SINK sink);
void afe_source_prefill_silence_data(SOURCE source);
int32_t afe_set_mem_block(afe_stream_type_t type, audio_digital_block_t mem_blk);
void afe_register_irq_ops(void);

void afe_send_amp_status_ccni(bool enable);
void afe_send_silence_status_ccni(bool SilenceFlag);

#ifdef ENABLE_AMP_TIMER
void afe_register_amp_handler(void);
#endif

void afe_set_asrc_ul_configuration_parameters(SOURCE source, afe_asrc_config_p asrc_config);
void afe_set_asrc_dl_configuration_parameters(SINK sink, afe_asrc_config_p asrc_config);
#ifdef ENABLE_HWSRC_CLKSKEW
extern void clock_skew_asrc_set_compensated_sample(afe_asrc_compensating_t cp_point);
extern void clock_skew_asrc_get_compensated_sample(uint32_t *accumulate_array);
extern uint32_t clock_skew_asrc_get_input_sample_size(void);
#endif
uint32_t audio_get_gcd(uint32_t m, uint32_t n);
void vRegSetBit(uint32_t addr, uint32_t bit);
void vRegResetBit(uint32_t addr, uint32_t bit);

bool audio_ops_distinguish_audio_source(void *param);


void stream_audio_srcl_interrupt(void);
void afe_vul1_interrupt_handler(void);
void afe_subsource_interrupt_handler(void);
#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
void afe_subsource2_interrupt_handler(void);
#endif

bool audio_ops_distinguish_audio_sink(void *param);
bool audio_ops_distinguish_audio_source(void *param);

void afe_send_ccni_anc_switch_filter(uint32_t id);

#endif /* __DSP_DRV_AFE_COMMON_H__ */
