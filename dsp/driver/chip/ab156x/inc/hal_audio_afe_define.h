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

#ifndef __HAL_AUDIO_AFE_DEF_H__
#define __HAL_AUDIO_AFE_DEF_H__

#include "hal_audio.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

#include <stdint.h>
#include <stdbool.h>
#include "hal_audio_register.h"

#define AFE_READ(addr)          *((volatile uint32_t *)(addr))
#define AFE_WRITE(addr, val)    *((volatile uint32_t *)(addr)) = val
#define AFE_WRITE8(addr, val)   *((volatile uint8_t *)(addr)) = val
//#define AFE_SET_REG(addr, val, msk)  AFE_WRITE((addr), ((AFE_READ(addr) & (~(msk))) | ((val) & (msk))))
//#define AFE_GET_REG(addr)       *((volatile uint32_t *)(addr))

//#define ANA_READ(addr)          *((volatile uint16_t *)(addr))
#define ANA_WRITE(addr, val)    *((volatile uint16_t *)(addr)) = val
//#define ANA_SET_REG(addr, val, msk)  ANA_WRITE((addr), ((ANA_READ(addr) & (~(msk))) | ((val) & (msk))))
//#define ANA_GET_REG(addr)       *((volatile uint16_t *)(addr))

#define AFE_BASE          (0xC0000000)
#define ABB_BASE          (0xA2070000)

/* Internal sram */
#define AFE_INTERNAL_SRAM_VP_PHY_BASE   (AFE_BASE + 0x2000)
#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
#define AFE_INTERNAL_SRAM_VP_SIZE       (0x3000)
#else
#define AFE_INTERNAL_SRAM_VP_SIZE       (0x4000)
#endif
#define AFE_INTERNAL_SRAM_PHY_BASE      (AFE_BASE + 0x2000 + AFE_INTERNAL_SRAM_VP_SIZE) //(AFE_BASE + 0x2000)
#define AFE_INTERNAL_SRAM_NORMAL_SIZE   (0xA100)            //(0xE100)                            /* for normal mode, 56.25KB */
#define AFE_INTERNAL_SRAM_COMPACT_SIZE  (0xEC00)            //(0x12C00)                           /* for compact mode, 75KB*/
#define AFE_INTERNAL_SRAM_SIZE          (AFE_INTERNAL_SRAM_COMPACT_SIZE)

#ifdef ENABLE_HWSRC_CLKSKEW
#define AFE_REGISTER_ASRC_IRQ           (true)// true: asrc interrupt, false:DL interrupt
#else
#define AFE_REGISTER_ASRC_IRQ           (false)// true: asrc interrupt, false:DL interrupt
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/* I2S Setting*/
#define AFE_I2S_SETTING_FORMAT          (I2S_I2S)    //I2S_I2S, I2S_LJ, I2S_RJ
#define AFE_I2S_SETTING_WORD_LENGTH     (I2S_32BIT) //I2S_16BIT, I2S_32BIT

#define AFE_I2S_SETTING_LOW_JITTER      (true)

#define AFE_I2S_SETTING_MCLK_SOURCE_49M (true)      //true:49M, false:45M
#define AFE_I2S_SETTING_MCLK_DIVIDER    (7)        /*MCLK = clock_source/(1+n), clock_source : AFE_I2S_SETTING_MCLK_SOURCE,
                                                                                 n : AFE_I2S_SETTING_MCLK_DIVIDER)*/
typedef enum {
    INPUT_DIGITAL_GAIN_FOR_MIC0_L                   = 0,            /**< input digital gain for DMIC0_L path.  */
    INPUT_DIGITAL_GAIN_FOR_MIC0_R                   = 1,            /**< input digital gain for DMIC0_R path.  */
    INPUT_DIGITAL_GAIN_FOR_MIC1_L                   = 2,            /**< input digital gain for DMIC1_L path.  */
    INPUT_DIGITAL_GAIN_FOR_MIC1_R                   = 3,            /**< input digital gain for DMIC1_R path.  */
    INPUT_DIGITAL_GAIN_FOR_MIC2_L                   = 4,            /**< input digital gain for DMIC2_L path.  */
    INPUT_DIGITAL_GAIN_FOR_MIC2_R                   = 5,            /**< input digital gain for DMIC2_R path.  */
    INPUT_DIGITAL_GAIN_FOR_I2S0_L                   = 6,            /**< input digital gain for I2S0 L.  */
    INPUT_DIGITAL_GAIN_FOR_I2S0_R                   = 7,            /**< input digital gain for I2S0 R.  */
    INPUT_DIGITAL_GAIN_FOR_I2S1_L                   = 8,            /**< input digital gain for I2S1 L.  */
    INPUT_DIGITAL_GAIN_FOR_I2S1_R                   = 9,            /**< input digital gain for I2S1 R.  */
    INPUT_DIGITAL_GAIN_FOR_I2S2_L                   = 10,            /**< input digital gain for I2S2 L.  */
    INPUT_DIGITAL_GAIN_FOR_I2S2_R                   = 11,            /**< input digital gain for I2S2 L.  */
    INPUT_DIGITAL_GAIN_FOR_LINEIN_L                 = 12,            /**< input digital gain for LINEIN L.  */
    INPUT_DIGITAL_GAIN_FOR_LINEIN_R                 = 13,            /**< input digital gain for LINEIN R.  */
    INPUT_DIGITAL_GAIN_FOR_ECHO_PATH                = 14,            /**< input digital gain for echo path.  */

#ifdef MTK_SPECIAL_FUNCTIONS_ENABLE
    INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_BASE    = 18,
    INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_MIC0_L  = INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_BASE,
    INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_MIC0_R  = 19,
    INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_MIC1_L  = 20,
    INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_MIC1_R  = 21,
    INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_MIC2_L  = 22,
    INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_MIC2_R  = 23,
    INPUT_DIGITAL_GAIN_FOR_SPECIAL_FUNCTION_ECHO    = 24,
#endif
    INPUT_DIGITAL_GAIN_NUM,           /**< Specifies the number of input digital gain.  */

} afe_input_digital_gain_t;

typedef enum {
    INPUT_ANALOG_GAIN_FOR_MIC0_L                 = 0,            /**< input analog gain for analog microphone_0 L.  */
    INPUT_ANALOG_GAIN_FOR_MIC0_R                 = 1,            /**< input analog gain for analog microphone_0 R.  */
    INPUT_ANALOG_GAIN_FOR_MIC1_L                 = 2,            /**< input analog gain for analog microphone_1 R.  */
    INPUT_ANALOG_GAIN_FOR_MIC1_R                 = 3,            /**< input analog gain for analog microphone_1 L.  */
    INPUT_ANALOG_GAIN_FOR_MIC2_L                 = 4,            /**< input analog gain for analog microphone_2 R.  */
    INPUT_ANALOG_GAIN_FOR_MIC2_R                 = 5,            /**< input analog gain for analog microphone_3 L.  */
    INPUT_ANALOG_GAIN_NUM                        = 6,            /**< Specifies the number of input analog gain.  */
} afe_input_analog_gain_t;

typedef struct {
    hal_audio_channel_number_t  stream_channel;
    hal_audio_device_t          audio_device;
    int32_t                     digital_gain_index[INPUT_DIGITAL_GAIN_NUM];
    int32_t                     digital_gain_index_recoup;
    uint32_t                    analog_gain_index[INPUT_ANALOG_GAIN_NUM];
    bool                        mute_flag;
} audio_stream_in_mode_t;

typedef struct {
    hal_audio_channel_number_t  stream_channel;
    hal_audio_device_t          audio_device;
    int32_t                     digital_gain_index;
    int32_t                     digital_gain_index_compensation;
    int32_t                     digital_gain_index2;
    int32_t                     digital_gain_index2_compensation;
    uint32_t                    analog_gain_index;
    #ifdef HAL_AUDIO_ENABLE_PATH_MEM_DEVICE
    hal_audio_analog_mdoe_t     dac_mode;/**< for dac_mode */
    #endif
    uint16_t                    dc_compensation_value;
    uint16_t                    digital_gain_1_sample_per_step;
    uint16_t                    digital_gain_2_sample_per_step;
    bool                        mute_flag;
    bool                        mute_flag2;
} audio_stream_out_mode_t;


typedef struct {
    int32_t    sidetone_gain;
    uint32_t   sidetone_out_path;
    uint32_t   sidetone_in_path;
    uint32_t   sample_rate;
    bool       start_flag;
    bool       channel_flag;//0: L channel as STF input, 1: R channel as STF input
} afe_sidetone_config_t;

typedef struct {
    int8_t adc_l_state;
    int8_t adc_r_state;
#ifdef ENABLE_2A2D_TEST
        int8_t adc_dmic_state;
#endif
} afe_adc_state_t;

typedef struct {
    int8_t micbias0_state;
    int8_t micbias1_state;
} afe_micbias_state_t;

typedef struct {
    bool       top_enable;
    bool       dl1_enable;
    bool       dl2_enable;
    bool       ul1_enable;
    bool       mic_bias;
    bool       headphone_on;
    bool       amp_open_state;
    afe_sidetone_config_t sidetone;
    audio_stream_out_mode_t stream_out;
    audio_stream_in_mode_t  stream_in;
    const  hal_audio_irq_callback_function_t *func_handle;
#ifdef ENABLE_AMP_TIMER
    hal_amp_function_t *amp_handle;
#endif
    afe_adc_state_t adc_state;
    afe_micbias_state_t micbias_state;
} afe_t;

typedef struct {
    int8_t pos;
    int8_t len;
} bit_t;
#if 0//modify for ab1568
typedef enum {
    AFE_I2S0 = 0,
    AFE_I2S1,
    AFE_I2S2,
    AFE_I2S3
} afe_i2s_num_t;
#endif
typedef enum {
    I2S_EIAJ = 0x0,
    I2S_I2S  = 0x1,
    I2S_LJ   = 0x2, // Left-justified
    I2S_RJ   = 0x3  // Right-justified
} afe_i2s_format_t;

typedef enum {
    I2S_16BIT = 0x0,
    I2S_32BIT = 0x1
} afe_i2s_wlen_t;

typedef enum {
    I2S_NOSWAP = 0x0,
    I2S_LRSWAP = 0x1
} afe_i2s_swap_t;

typedef enum {
    I2S_DISABLE = 0x0,
    I2S_ENABLE  = 0x1
} afe_i2s_enable_t;

typedef enum {
    I2S_CONNSYS = 0x0, //FM
    I2S_IOMUX   = 0x1  //MATV
} afe_i2s_input_src_t;

typedef enum {
    I2S_MASTER = 0x0,
    I2S_SLAVE  = 0x1
} afe_i2s_role_t;

typedef enum {
    I2S_OUT = 0x0,
    I2S_IN  = 0x1
} afe_i2s_direction_t;

typedef enum {
    I2S_MCLK= 0x0,
    I2S_CK,
    I2S_WS,
    I2S_TX,
    I2S_RX,
    I2S_GPIO_NUM,
} afe_i2s_gpio_t;

#if 0//modify for ab1568
/*Do not modify number, align AFE RG setting*/
typedef enum  {
    AFE_GENERAL_SAMPLERATE_8K   = 0,
    AFE_GENERAL_SAMPLERATE_11K  = 1,
    AFE_GENERAL_SAMPLERATE_12K  = 2,
    AFE_GENERAL_SAMPLERATE_16K  = 4,
    AFE_GENERAL_SAMPLERATE_22K  = 5,
    AFE_GENERAL_SAMPLERATE_24K  = 6,
    AFE_GENERAL_SAMPLERATE_32K  = 8,
    AFE_GENERAL_SAMPLERATE_44K  = 9,
    AFE_GENERAL_SAMPLERATE_48K  = 10,
    AFE_GENERAL_SAMPLERATE_88K  = 11,
    AFE_GENERAL_SAMPLERATE_96K  = 12,
    AFE_GENERAL_SAMPLERATE_176K = 13,
    AFE_GENERAL_SAMPLERATE_192K = 14,
} afe_general_samplerate_t;

typedef enum {
    AFE_ADDA_DL_SAMPLERATE_8K    = 0,
    AFE_ADDA_DL_SAMPLERATE_11K   = 1,
    AFE_ADDA_DL_SAMPLERATE_12K   = 2,
    AFE_ADDA_DL_SAMPLERATE_16K   = 3,
    AFE_ADDA_DL_SAMPLERATE_22K   = 4,
    AFE_ADDA_DL_SAMPLERATE_24K   = 5,
    AFE_ADDA_DL_SAMPLERATE_32K   = 6,
    AFE_ADDA_DL_SAMPLERATE_44K   = 7,
    AFE_ADDA_DL_SAMPLERATE_48K   = 8,
    AFE_ADDA_DL_SAMPLERATE_96K   = 9,
    AFE_ADDA_DL_SAMPLERATE_192K  = 10
}afe_adda_dl_samplerate_t;

typedef enum {
    AFE_ADDA_UL_SAMPLERATE_8K    = 0,
    AFE_ADDA_UL_SAMPLERATE_16K   = 1,
    AFE_ADDA_UL_SAMPLERATE_32K   = 2,
    AFE_ADDA_UL_SAMPLERATE_48K   = 3,
    AFE_ADDA_UL_SAMPLERATE_96K   = 4,
    AFE_ADDA_UL_SAMPLERATE_192K  = 5,
    AFE_ADDA_UL_SAMPLERATE_48K_HD= 6
} afe_adda_ul_samplerate_t;
#endif

typedef enum {
    AFE_IRQ0 = 0,
    AFE_IRQ1,
    AFE_IRQ2,
    AFE_IRQ3,
    AFE_IRQ4,
    AFE_IRQ5,
    AFE_IRQ6,
    AFE_IRQ7,
    AFE_IRQ8,
    AFE_IRQ9,
    AFE_IRQ10,
    AFE_IRQ11,
    AFE_IRQ12,
    AFE_IRQ13,
    AFE_IRQ14,
    AFE_IRQ_NUM,
    AFE_IRQ_ALL_NUM = 15
} afe_irq_mode_t;

typedef enum {
    STREAM_M_AFE_M,
    STREAM_M_AFE_S,
    STREAM_S_AFE_M,
    STREAM_S_AFE_S,
    STREAM_B_AFE_B, // both channel: CH1+CH2
    STREAM_AFE_NUM_CH_TYPE
} afe_stream_channel_t;

typedef enum {
    AFE_WLEN_16_BIT = 0,
    AFE_WLEN_32_BIT_ALIGN_8BIT_0_24BIT_DATA = 1,
    AFE_WLEN_32_BIT_ALIGN_24BIT_DATA_8BIT_0 = 3,
} afe_fetch_format_per_sampel_t;

typedef enum {
    AFE_SRAM_NORMAL_MODE,
    AFE_SRAM_COMPACT_MODE,
} afe_sram_mode_t;

typedef enum {
    AFE_PCM_FORMAT_S8 = 0,
    AFE_PCM_FORMAT_U8,
    AFE_PCM_FORMAT_S16_LE,
    AFE_PCM_FORMAT_S16_BE,
    AFE_PCM_FORMAT_U16_LE,
    AFE_PCM_FORMAT_U16_BE,
    AFE_PCM_FORMAT_S24_LE,
    AFE_PCM_FORMAT_S24_BE,
    AFE_PCM_FORMAT_U24_LE,
    AFE_PCM_FORMAT_U24_BE,
    AFE_PCM_FORMAT_S32_LE,
    AFE_PCM_FORMAT_S32_BE,
    AFE_PCM_FORMAT_U32_LE,
    AFE_PCM_FORMAT_U32_BE,
    AFE_PCM_FORMAT_LAST,
    AFE_PCM_FORMAT_DUMMY  = 0xFFFFFFFF,   /**<  for DSP structrue alignment */
} afe_pcm_format_t;

typedef enum {
    AFE_SGEN_AMP_DIV_128 = 0,
    AFE_SGEN_AMP_DIV_64  = 1,
    AFE_SGEN_AMP_DIV_32  = 2,
    AFE_SGEN_AMP_DIV_16  = 3,
    AFE_SGEN_AMP_DIV_8   = 4,
    AFE_SGEN_AMP_DIV_4   = 5,
    AFE_SGEN_AMP_DIV_2   = 6,
    AFE_SGEN_AMP_DIV_1   = 7
} afe_sgen_amp_div_t;

typedef enum {
    AUDIO_HW_GAIN,
    AUDIO_HW_GAIN2,
    AUDIO_SW_GAIN
} afe_digital_gain_t;


typedef enum {
    AUDIO_BT_SYNC_DL1_EN    = 0x10000,
    AUDIO_BT_SYNC_DL2_EN    = 0x20000,
    AUDIO_BT_SYNC_DL12_EN   = 0x40000,
    AUDIO_BT_SYNC_DL3_EN    = 0x80000,
} afe_audio_bt_sync_enable_t;


/*Align AFE_SIDETONE_CON1 bit operation*/
typedef enum  {
    AFE_SIDETONE_I2S0  = 24,
    AFE_SIDETONE_I2S1  = 25,
    AFE_SIDETONE_I2S2  = 26,
    AFE_SIDETONE_I2S3  = 27,
    AFE_SIDETONE_DL    = 28,
} afe_sidetone_path_t;


typedef enum  {
	AFE_MEM_ASRC_1 = 0,
	AFE_MEM_ASRC_2 = 1,
	MEM_ASRC_NUM = 2
} afe_mem_asrc_id_t;

#ifdef ENABLE_HWSRC_CLKSKEW
typedef enum  {
    COMPENSATING_SUB_976_XPPM = 0,
    COMPENSATING_SUB_915_XPPM = 1,
    COMPENSATING_SUB_854_XPPM = 2,
    COMPENSATING_SUB_793_XPPM = 3,
    COMPENSATING_SUB_732_XPPM = 4,
    COMPENSATING_SUB_671_XPPM = 5,
    COMPENSATING_SUB_610_XPPM = 6,
    COMPENSATING_SUB_549_XPPM = 7,
    COMPENSATING_SUB_488_XPPM = 8,
    COMPENSATING_SUB_427_XPPM = 9,
    COMPENSATING_SUB_366_XPPM = 10,
    COMPENSATING_SUB_305_XPPM = 11,
    COMPENSATING_SUB_244_XPPM = 12,
    COMPENSATING_SUB_183_XPPM = 13,
    COMPENSATING_SUB_122_XPPM = 14,
    COMPENSATING_SUB_61_XPPM  = 15,
    COMPENSATING_ADD_0_XPPM   = 16,
    COMPENSATING_ADD_61_XPPM  = 17,
    COMPENSATING_ADD_122_XPPM = 18,
    COMPENSATING_ADD_183_XPPM = 19,
    COMPENSATING_ADD_244_XPPM = 20,
    COMPENSATING_ADD_305_XPPM = 21,
    COMPENSATING_ADD_366_XPPM = 22,
    COMPENSATING_ADD_427_XPPM = 23,
    COMPENSATING_ADD_488_XPPM = 24,
    COMPENSATING_ADD_549_XPPM = 25,
    COMPENSATING_ADD_610_XPPM = 26,
    COMPENSATING_ADD_671_XPPM = 27,
    COMPENSATING_ADD_732_XPPM = 28,
    COMPENSATING_ADD_793_XPPM = 29,
    COMPENSATING_ADD_854_XPPM = 30,
    COMPENSATING_ADD_915_XPPM = 31,
    COMPENSATING_ADD_976_XPPM = 32,
    AFE_HWSRC_STEP_NUM,
} afe_asrc_compensating_t;


//--------------------------------------------
// CLK SKEW Mode
//--------------------------------------------
typedef enum {
    CLK_SKEW_V1 = 0, /* sw clk skew */
    CLK_SKEW_V2 = 1, /* hwsrc clk skew */
    CLK_SKEW_DUMMY = 0xFFFFFFFF,
} clkskew_mode_t;

#endif

typedef enum {
	MEM_ASRC_NO_TRACKING,
	MEM_ASRC_TRACKING_MODE_RX,
	MEM_ASRC_TRACKING_MODE_TX
}afe_asrc_tracking_mode_t;

typedef enum  {
	MEM_ASRC_TRACKING_I2S1,
	MEM_ASRC_TRACKING_I2S2,
	MEM_ASRC_TRACKING_I2S3,
	MEM_ASRC_TRACKING_I2S4,
	MEM_ASRC_TRACKING_SPDIFIN,
}afe_asrc_tracking_clock_t;

typedef struct {
    bool     I2sClkSourceType;
    uint8_t  MicbiasSourceType;
} afe_misc_parms_t;

typedef struct  {
	uint32_t addr;		/* physical */
	uint32_t size;
	uint32_t rate;
	uint32_t offset;
	afe_pcm_format_t format;
} afe_asrc_buffer_t;

typedef struct {
	afe_asrc_buffer_t input_buffer;
	afe_asrc_buffer_t output_buffer;
	afe_asrc_tracking_mode_t tracking_mode;
	afe_asrc_tracking_clock_t tracking_clock; /* valid only if tracking_mode != MEM_ASRC_NO_TRACKING */
	bool stereo;
    bool hw_update_obuf_rdpnt;
    bool ul_mode;
} afe_asrc_config_t, *afe_asrc_config_p;

typedef struct {
	uint32_t in_ratio;
	uint32_t out_ratio;
	const uint32_t *coef;
} afe_asrc_iir_coef_t, *afe_asrc_iir_coef_p;

typedef void (*hal_asrc_irq_callback_function_t) (afe_mem_asrc_id_t asrc_id);
#if 0//modify for ab1568

/*****************************************************************************
 *                  R E G I S T E R       D E F I N I T I O N
 *****************************************************************************/
#define AUDIO_TOP_CON0                (AFE_BASE + 0x0000)
#define AUDIO_TOP_CON0_PDN_AFE_POS              (2)
#define AUDIO_TOP_CON0_PDN_AFE_MASK             (1<<AUDIO_TOP_CON0_PDN_AFE_POS)
#define AUDIO_TOP_CON0_PDN_22M_POS              (8)
#define AUDIO_TOP_CON0_PDN_22M_MASK             (1<<AUDIO_TOP_CON0_PDN_22M_POS)
#define AUDIO_TOP_CON0_PDN_24M_POS              (9)
#define AUDIO_TOP_CON0_PDN_24M_MASK             (1<<AUDIO_TOP_CON0_PDN_24M_POS)
#define AUDIO_TOP_CON0_APB_W2T_POS              (12)
#define AUDIO_TOP_CON0_APB_W2T_MASK             (1<<AUDIO_TOP_CON0_APB_W2T_POS)
#define AUDIO_TOP_CON0_APB_R2T_POS              (13)
#define AUDIO_TOP_CON0_APB_R2T_MASK             (1<<AUDIO_TOP_CON0_APB_R2T_POS)
#define AUDIO_TOP_CON0_APB3_SEL_POS             (14)
#define AUDIO_TOP_CON0_APB3_SEL_MASK            (1<<AUDIO_TOP_CON0_APB3_SEL_POS)
#define AUDIO_TOP_CON0_PDN_NLE_POS              (15)
#define AUDIO_TOP_CON0_PDN_NLE_MASK             (1<<AUDIO_TOP_CON0_PDN_NLE_POS)
#define AUDIO_TOP_CON0_PDN_APLL2_TUNER_POS      (18)
#define AUDIO_TOP_CON0_PDN_APLL2_TUNER_MASK     (1<<AUDIO_TOP_CON0_PDN_APLL2_TUNER_POS)
#define AUDIO_TOP_CON0_PDN_APLL_TUNER_POS       (19)
#define AUDIO_TOP_CON0_PDN_APLL_TUNER_MASK      (1<<AUDIO_TOP_CON0_PDN_APLL_TUNER_POS)
#define AUDIO_TOP_CON0_PDN_ADC_POS              (24)
#define AUDIO_TOP_CON0_PDN_ADC_MASK             (1<<AUDIO_TOP_CON0_PDN_ADC_POS)
#define AUDIO_TOP_CON0_PDN_DAC_POS              (25)
#define AUDIO_TOP_CON0_PDN_DAC_MASK             (1<<AUDIO_TOP_CON0_PDN_DAC_POS)
#define AUDIO_TOP_CON0_PDN_DAC_PREDIS_POS       (26)
#define AUDIO_TOP_CON0_PDN_DAC_PREDIS_MASK      (1<<AUDIO_TOP_CON0_PDN_DAC_PREDIS_POS)
#define AUDIO_TOP_CON0_PDN_TML_POS              (27)
#define AUDIO_TOP_CON0_PDN_TML_MASK             (1<<AUDIO_TOP_CON0_PDN_TML_POS)
#define AUDIO_TOP_CON0_PDN_CLASSG_POS           (28)
#define AUDIO_TOP_CON0_PDN_CLASSG_MASK          (1<<AUDIO_TOP_CON0_PDN_CLASSG_POS)
#define AUDIO_TOP_CON0_AHB_IDLE_EN_EXT_POS      (29)
#define AUDIO_TOP_CON0_AHB_IDLE_EN_EXT_MASK     (1<<AUDIO_TOP_CON0_AHB_IDLE_EN_EXT_POS)
#define AUDIO_TOP_CON0_AHB_IDLE_EN_INT_POS      (30)
#define AUDIO_TOP_CON0_AHB_IDLE_EN_INT_MASK     (1<<AUDIO_TOP_CON0_AHB_IDLE_EN_INT_POS)
#define AUDIO_TOP_CON1                (AFE_BASE + 0x0004)
#define AUDIO_TOP_CON1_PDN_I2S0_POS             (0)
#define AUDIO_TOP_CON1_PDN_I2S0_MASK            (1<<AUDIO_TOP_CON1_PDN_I2S0_POS)
#define AUDIO_TOP_CON1_PDN_I2S1_POS             (1)
#define AUDIO_TOP_CON1_PDN_I2S1_MASK            (1<<AUDIO_TOP_CON1_PDN_I2S1_POS)
#define AUDIO_TOP_CON1_PDN_I2S2_POS             (2)
#define AUDIO_TOP_CON1_PDN_I2S2_MASK            (1<<AUDIO_TOP_CON1_PDN_I2S2_POS)
#define AUDIO_TOP_CON1_PDN_I2S3_POS             (3)
#define AUDIO_TOP_CON1_PDN_I2S3_MASK            (1<<AUDIO_TOP_CON1_PDN_I2S3_POS)
#define AUDIO_TOP_CON1_PDN_ADC_HIRES_POS        (16)
#define AUDIO_TOP_CON1_PDN_ADC_HIRES_MASK       (1<<AUDIO_TOP_CON1_PDN_ADC_HIRES_POS)
#define AUDIO_TOP_CON1_PDN_ADC_HIRES_TML_POS    (17)
#define AUDIO_TOP_CON1_PDN_ADC_HIRES_TML_MASK   (1<<AUDIO_TOP_CON1_PDN_ADC_HIRES_TML_POS)
#define AUDIO_TOP_CON1_PDN_ADDA2_POS            (19)
#define AUDIO_TOP_CON1_PDN_ADDA2_MASK           (1<<AUDIO_TOP_CON1_PDN_ADDA2_POS)
#define AUDIO_TOP_CON1_PDN_ADDA6_POS            (20)
#define AUDIO_TOP_CON1_PDN_ADDA6_MASK           (1<<AUDIO_TOP_CON1_PDN_ADDA6_POS)
#define AUDIO_TOP_CON1_PDN_DAC_HIRES_POS        (23)
#define AUDIO_TOP_CON1_PDN_DAC_HIRES_MASK       (1<<AUDIO_TOP_CON1_PDN_DAC_HIRES_POS)
#define AUDIO_TOP_CON1_PDN_ASRC1_POS            (28)
#define AUDIO_TOP_CON1_PDN_ASRC1_MASK           (1<<AUDIO_TOP_CON1_PDN_ASRC1_POS)
#define AUDIO_TOP_CON1_PDN_ASRC2_POS            (29)
#define AUDIO_TOP_CON1_PDN_ASRC2_MASK           (1<<AUDIO_TOP_CON1_PDN_ASRC2_POS)
#define AUDIO_TOP_CON1_PDN_DRAM_BRIDGE_POS      (30)
#define AUDIO_TOP_CON1_PDN_DRAM_BRIDGE_MASK     (1<<AUDIO_TOP_CON1_PDN_DRAM_BRIDGE_POS)
#define AUDIO_TOP_CON1_PDN_ALL_MASK             (0x709b000f)


#define AUDIO_TOP_CON2                (AFE_BASE + 0x0008)
#define AUDIO_TOP_CON3                (AFE_BASE + 0x000C)

#define AFE_DAC_CON0                  (AFE_BASE + 0x0010)
#define AFE_DAC_CON1                  (AFE_BASE + 0x0014)
#define AFE_DAC_CON2                  (AFE_BASE + 0x02e0)
#define AFE_DAC_MON                   (AFE_BASE + 0x02ec)

#define AFE_CONN_24BIT                (AFE_BASE + 0x006c)

/*AFE connection REG*/
#define AFE_CONN0                     (AFE_BASE + 0x0020)
#define AFE_CONN1                     (AFE_BASE + 0x0024)
#define AFE_CONN2                     (AFE_BASE + 0x0028)
#define AFE_CONN3                     (AFE_BASE + 0x002c)
#define AFE_CONN4                     (AFE_BASE + 0x0030)
#define AFE_CONN5                     (AFE_BASE + 0x005c)
#define AFE_CONN6                     (AFE_BASE + 0x00bc)
#define AFE_CONN7                     (AFE_BASE + 0x0420)
#define AFE_CONN8                     (AFE_BASE + 0x0438)
#define AFE_CONN9                     (AFE_BASE + 0x0440)
#define AFE_CONN10                    (AFE_BASE + 0x0444)
#define AFE_CONN11                    (AFE_BASE + 0x0448)
#define AFE_CONN12                    (AFE_BASE + 0x044c)
#define AFE_CONN13                    (AFE_BASE + 0x0450)
#define AFE_CONN14                    (AFE_BASE + 0x0454)
#define AFE_CONN15                    (AFE_BASE + 0x0458)
#define AFE_CONN16                    (AFE_BASE + 0x045c)
#define AFE_CONN17                    (AFE_BASE + 0x0460)
#define AFE_CONN18                    (AFE_BASE + 0x0464)
#define AFE_CONN19                    (AFE_BASE + 0x0468)
#define AFE_CONN20                    (AFE_BASE + 0x046c)
#define AFE_CONN21                    (AFE_BASE + 0x0470)
#define AFE_CONN22                    (AFE_BASE + 0x0474)
#define AFE_CONN23                    (AFE_BASE + 0x0478)

#define AFE_DL1_BASE                  (AFE_BASE + 0x0040)
#define AFE_DL1_CUR                   (AFE_BASE + 0x0044)
#define AFE_DL1_END                   (AFE_BASE + 0x0048)

#define AFE_DL2_BASE                  (AFE_BASE + 0x0050)
#define AFE_DL2_CUR                   (AFE_BASE + 0x0054)
#define AFE_DL2_END                   (AFE_BASE + 0x0058)

#define AFE_AWB_BASE                  (AFE_BASE + 0x0070)
#define AFE_AWB_END                   (AFE_BASE + 0x0078)
#define AFE_AWB_CUR                   (AFE_BASE + 0x007c)

#define AFE_VUL_BASE                  (AFE_BASE + 0x0080)
#define AFE_VUL_END                   (AFE_BASE + 0x0088)
#define AFE_VUL_CUR                   (AFE_BASE + 0x008c)

#define AFE_DL12_BASE                 (AFE_BASE + 0x0340)
#define AFE_DL12_CUR                  (AFE_BASE + 0x0344)
#define AFE_DL12_END                  (AFE_BASE + 0x0348)

#define AFE_DL3_BASE                  (AFE_BASE + 0x0360)
#define AFE_DL3_CUR                   (AFE_BASE + 0x0364)
#define AFE_DL3_END                   (AFE_BASE + 0x0368)

#define AFE_VUL2_BASE                 (AFE_BASE + 0x02f0)
#define AFE_VUL2_END                  (AFE_BASE + 0x02f8)
#define AFE_VUL2_CUR                  (AFE_BASE + 0x02fc)

#define AFE_AWB2_BASE                 (AFE_BASE + 0x0bd0)
#define AFE_AWB2_END                  (AFE_BASE + 0x0bd8)
#define AFE_AWB2_CUR                  (AFE_BASE + 0x0bdc)

#define AFE_DL1_BASE_MSB              (AFE_BASE + 0x0b00)
#define AFE_DL1_CUR_MSB               (AFE_BASE + 0x0b04)
#define AFE_DL1_END_MSB               (AFE_BASE + 0x0b08)

#define AFE_DL2_BASE_MSB              (AFE_BASE + 0x0b10)
#define AFE_DL2_CUR_MSB               (AFE_BASE + 0x0b14)
#define AFE_DL2_END_MSB               (AFE_BASE + 0x0b18)

#define AFE_AWB_BASE_MSB              (AFE_BASE + 0x0b20)
#define AFE_AWB_END_MSB               (AFE_BASE + 0x0b28)
#define AFE_AWB_CUR_MSB               (AFE_BASE + 0x0b2c)

#define AFE_VUL_BASE_MSB              (AFE_BASE + 0x0b30)
#define AFE_VUL_END_MSB               (AFE_BASE + 0x0b38)
#define AFE_VUL_CUR_MSB               (AFE_BASE + 0x0b3c)

#define AFE_VUL2_BASE_MSB             (AFE_BASE + 0x0b50)
#define AFE_VUL2_END_MSB              (AFE_BASE + 0x0b58)
#define AFE_VUL2_CUR_MSB              (AFE_BASE + 0x0b5c)

#define AFE_DL1_D2_BASE_MSB           (AFE_BASE + 0x0b70)
#define AFE_DL1_D2_CUR_MSB            (AFE_BASE + 0x0b74)
#define AFE_DL1_D2_END_MSB            (AFE_BASE + 0x0b78)

#define AFE_DL3_BASE_MSB              (AFE_BASE + 0x0b90)
#define AFE_DL3_CUR_MSB               (AFE_BASE + 0x0b94)
#define AFE_DL3_END_MSB               (AFE_BASE + 0x0b98)

#define AFE_AWB2_BASE_MSB             (AFE_BASE + 0x0be0)
#define AFE_AWB2_END_MSB              (AFE_BASE + 0x0be8)
#define AFE_AWB2_CUR_MSB              (AFE_BASE + 0x0bec)

#define AFE_MEMIF_MSB                 (AFE_BASE + 0x00cc)
#define AFE_MEMIF_MON0                (AFE_BASE + 0x00d0)
#define AFE_MEMIF_MON1                (AFE_BASE + 0x00d4)
#define AFE_MEMIF_MON2                (AFE_BASE + 0x00d8)
#define AFE_MEMIF_MON3                (AFE_BASE + 0x00dc)
#define AFE_MEMIF_MON4                (AFE_BASE + 0x00e0)
#define AFE_MEMIF_MON5                (AFE_BASE + 0x00e4)
#define AFE_MEMIF_MON6                (AFE_BASE + 0x00e8)
#define AFE_MEMIF_MON7                (AFE_BASE + 0x00ec)
#define AFE_MEMIF_MON8                (AFE_BASE + 0x00f0)
#define AFE_MEMIF_MON9                (AFE_BASE + 0x00f4)
#define AFE_MEMIF_MINLEN              (AFE_BASE + 0x03d0)
#define AFE_MEMIF_MAXLEN              (AFE_BASE + 0x03d4)
#define AFE_MEMIF_PBUF_SIZE           (AFE_BASE + 0x03d8)
#define AFE_MEMIF_HD_MODE             (AFE_BASE + 0x03f8)
#define AFE_MEMIF_HDALIGN             (AFE_BASE + 0x03fc)

#define AFE_ADDA_DL_SRC2_CON0         (AFE_BASE + 0x0108)
#define AFE_ADDA_DL_SRC2_CON1         (AFE_BASE + 0x010c)
#define AFE_ADDA_UL_SRC_CON0          (AFE_BASE + 0x0114)
#define AFE_ADDA_UL_SRC_CON1          (AFE_BASE + 0x0118)
#define AFE_ADDA_TOP_CON0             (AFE_BASE + 0x0120)
#define AFE_ADDA_UL_DL_CON0           (AFE_BASE + 0x0124)
#define AFE_ADDA_SRC_DEBUG            (AFE_BASE + 0x012c)
#define AFE_ADDA_SRC_DEBUG_MON0       (AFE_BASE + 0x0130)
#define AFE_ADDA_SRC_DEBUG_MON1       (AFE_BASE + 0x0134)
#define AFE_ADDA_UL_SRC_MON0          (AFE_BASE + 0x0148)
#define AFE_ADDA_UL_SRC_MON1          (AFE_BASE + 0x014c)
#define AFE_ADDA_PREDIS_CON0          (AFE_BASE + 0x0260)
#define AFE_ADDA_PREDIS_CON1          (AFE_BASE + 0x0264)

#define AFE_ADDA2_UL_SRC_CON0         (AFE_BASE + 0x0604)
#define AFE_ADDA2_UL_SRC_CON1         (AFE_BASE + 0x0608)
#define AFE_ADDA2_SRC_DEBUG           (AFE_BASE + 0x060c)
#define AFE_ADDA2_SRC_DEBUG_MON0      (AFE_BASE + 0x0610)
#define AFE_ADDA2_UL_SRC_MON0         (AFE_BASE + 0x0618)
#define AFE_ADDA2_UL_SRC_MON1         (AFE_BASE + 0x061c)

#define AFE_ADDA6_UL_SRC_CON0         (AFE_BASE + 0x0a84)
#define AFE_ADDA6_UL_SRC_CON1         (AFE_BASE + 0x0a88)
#define AFE_ADDA6_SRC_DEBUG           (AFE_BASE + 0x0a8c)
#define AFE_ADDA6_SRC_DEBUG_MON0      (AFE_BASE + 0x0a90)
#define AFE_ADDA6_UL_SRC_MON0         (AFE_BASE + 0x0ae4)
#define AFE_ADDA6_UL_SRC_MON1         (AFE_BASE + 0x0ae8)

/*Sine gen*/
#define AFE_SGEN_CON0                 (AFE_BASE + 0x01f0)
#define AFE_SGEN_CON2                 (AFE_BASE + 0x01dc)

/*STF RGs*/
#define AFE_SIDETONE_DEBUG            (AFE_BASE + 0x01d0)
#define AFE_SIDETONE_MON              (AFE_BASE + 0x01d4)
#define AFE_SIDETONE_CON0             (AFE_BASE + 0x01e0)
#define AFE_SIDETONE_COEFF            (AFE_BASE + 0x01e4)
#define AFE_SIDETONE_CON1             (AFE_BASE + 0x01e8)
#define AFE_SIDETONE_GAIN             (AFE_BASE + 0x01ec)

#define AFE_BUS_CFG                   (AFE_BASE + 0x0240)
#define AFE_BUS_MON0                  (AFE_BASE + 0x0244)

#define AFE_IRQ_MCU_CON0              (AFE_BASE + 0x03a0)
#define AFE_IRQ_MCU_CON1              (AFE_BASE + 0x02e4)
#define AFE_IRQ_MCU_CON2              (AFE_BASE + 0x02e8)

#define AFE_IRQ_MCU_EN1               (AFE_BASE + 0x030c)
#define AFE_IRQ_MCU_EN                (AFE_BASE + 0x03b4)
#define AFE_IRQ_MCU_STATUS            (AFE_BASE + 0x03a4)
#define AFE_IRQ_MCU_CLR               (AFE_BASE + 0x03a8)
#define AFE_IRQ_MCU_MON2              (AFE_BASE + 0x03b8)
#define AFE_IRQ1_MCU_EN_CNT_MON       (AFE_BASE + 0x03c8)

#define AFE_IRQ0_MCU_CNT_MON          (AFE_BASE + 0x0310)
#define AFE_IRQ1_MCU_CNT_MON          (AFE_BASE + 0x03c0)
#define AFE_IRQ2_MCU_CNT_MON          (AFE_BASE + 0x03c4)
#define AFE_IRQ3_MCU_CNT_MON          (AFE_BASE + 0x0398)
#define AFE_IRQ4_MCU_CNT_MON          (AFE_BASE + 0x039c)
#define AFE_IRQ5_MCU_CNT_MON          (AFE_BASE + 0x03cc)
#define AFE_IRQ6_MCU_CNT_MON          (AFE_BASE + 0x0314)
#define AFE_IRQ7_MCU_CNT_MON          (AFE_BASE + 0x03e0)
#define AFE_IRQ8_MCU_CNT_MON          (AFE_BASE + 0x05e4)
#define AFE_IRQ11_MCU_CNT_MON         (AFE_BASE + 0x05e8)
#define AFE_IRQ12_MCU_CNT_MON         (AFE_BASE + 0x05ec)

#define AFE_IRQ_MCU_CNT0              (AFE_BASE + 0x0300)
#define AFE_IRQ_MCU_CNT1              (AFE_BASE + 0x03ac)
#define AFE_IRQ_MCU_CNT2              (AFE_BASE + 0x03b0)
#define AFE_IRQ_MCU_CNT3              (AFE_BASE + 0x03e4)
#define AFE_IRQ_MCU_CNT4              (AFE_BASE + 0x03e8)
#define AFE_IRQ_MCU_CNT5              (AFE_BASE + 0x03bc)
#define AFE_IRQ_MCU_CNT6              (AFE_BASE + 0x0304)
#define AFE_IRQ_MCU_CNT7              (AFE_BASE + 0x03dc)
#define AFE_IRQ_MCU_CNT11             (AFE_BASE + 0x03ec)
#define AFE_IRQ_MCU_CNT12             (AFE_BASE + 0x040c)

#define AFE_APLL1_TUNER_CFG           (AFE_BASE + 0x03f0)
#define AFE_APLL2_TUNER_CFG           (AFE_BASE + 0x03f4)

#define AFE_GAIN1_CON0                (AFE_BASE + 0x0410)
#define AFE_GAIN1_CON1                (AFE_BASE + 0x0414)
#define AFE_GAIN1_CON2                (AFE_BASE + 0x0418)
#define AFE_GAIN1_CON3                (AFE_BASE + 0x041c)
#define AFE_GAIN1_CUR                 (AFE_BASE + 0x0424)

#define AFE_GAIN2_CON0                (AFE_BASE + 0x0428)
#define AFE_GAIN2_CON1                (AFE_BASE + 0x042c)
#define AFE_GAIN2_CON2                (AFE_BASE + 0x0430)
#define AFE_GAIN2_CON3                (AFE_BASE + 0x0434)
#define AFE_GAIN2_CUR                 (AFE_BASE + 0x043c)

#define AFE_CONN_RS                   (AFE_BASE + 0x0494)
#define AFE_CONN_DI                   (AFE_BASE + 0x0498)
#define AFE_SRAM_DELSEL_CON0          (AFE_BASE + 0x04f0)
#define AFE_SRAM_DELSEL_CON1          (AFE_BASE + 0x04f4)
#define AFE_SRAM_DELSEL_CON2          (AFE_BASE + 0x04f8)
#define AFE_SPDIFIN_CFG1              (AFE_BASE + 0x504)
#define AFE_SPDIFIN_CFG1_SEL_SPDIFIN_CLK_EN_POS     (1)
#define AFE_SPDIFIN_CFG1_SEL_SPDIFIN_CLK_EN_MASK    (1<<AFE_SPDIFIN_CFG1_SEL_SPDIFIN_CLK_EN_POS)


#define FPGA_CFG0                     (AFE_BASE + 0x05b0)
#define FPGA_CFG1                     (AFE_BASE + 0x05b4)
#define FPGA_CFG2                     (AFE_BASE + 0x05c0)
#define FPGA_CFG3                     (AFE_BASE + 0x05c4)
#define AUDIO_TOP_DBG_CON             (AFE_BASE + 0x05c8)
#define AUDIO_TOP_DBG_MON0            (AFE_BASE + 0x05cc)
#define AUDIO_TOP_DBG_MON1            (AFE_BASE + 0x05d0)
#define AUDIO_TOP_DBG_MON2            (AFE_BASE + 0x05d4)
#define PWR2_TOP_CON                  (AFE_BASE + 0x0634)
#define PWR2_TOP_CON_PDN_MEM_ASRC1_POS              (10)
#define PWR2_TOP_CON_PDN_MEM_ASRC1_MASK             (1<<PWR2_TOP_CON_PDN_MEM_ASRC1_POS)

#define AFE_GENERAL_REG0              (AFE_BASE + 0x0800)
#define AFE_GENERAL_REG1              (AFE_BASE + 0x0804)
#define AFE_GENERAL_REG2              (AFE_BASE + 0x0808)
#define AFE_GENERAL_REG3              (AFE_BASE + 0x080c)
#define AFE_GENERAL_REG4              (AFE_BASE + 0x0810)
#define AFE_GENERAL_REG5              (AFE_BASE + 0x0814)
#define AFE_GENERAL_REG6              (AFE_BASE + 0x0818)
#define AFE_GENERAL_REG7              (AFE_BASE + 0x081c)
#define AFE_GENERAL_REG8              (AFE_BASE + 0x0820)
#define AFE_GENERAL_REG9              (AFE_BASE + 0x0824)
#define AFE_GENERAL_REG10             (AFE_BASE + 0x0828)
#define AFE_GENERAL_REG11             (AFE_BASE + 0x082c)
#define AFE_GENERAL_REG12             (AFE_BASE + 0x0830)
#define AFE_GENERAL_REG13             (AFE_BASE + 0x0834)
#define AFE_GENERAL_REG14             (AFE_BASE + 0x0838)
#define AFE_GENERAL_REG15             (AFE_BASE + 0x083c)
#define AFE_CBIP_CFG0                 (AFE_BASE + 0x0840)
#define AFE_CBIP_MON0                 (AFE_BASE + 0x0844)
#define AFE_CBIP_SLV_MUX_MON0         (AFE_BASE + 0x0848)
#define AFE_CBIP_SLV_DECODER_MON0     (AFE_BASE + 0x084c)

#define AFE_I2S0_CON                  (AFE_BASE + 0x0860)
#define AFE_I2S1_CON                  (AFE_BASE + 0x0864)
#define AFE_I2S2_CON                  (AFE_BASE + 0x0868)
#define AFE_I2S3_CON                  (AFE_BASE + 0x086c)
#define AFE_I2S_TOP_CON               (AFE_BASE + 0x0870)
#define AFE_I2S_CK_ENABLE_MON         (AFE_BASE + 0x0874)
#define AFE_I2S_BCOUNT_MON            (AFE_BASE + 0x0878)

#define AFE_ADDA_DL_SDM_DCCOMP_CON    (AFE_BASE + 0x0c50)
#define AFE_ADDA_DL_SDM_TEST          (AFE_BASE + 0x0c54)
#define AFE_ADDA_DL_DC_COMP_CFG0      (AFE_BASE + 0x0c58)
#define AFE_ADDA_DL_DC_COMP_CFG1      (AFE_BASE + 0x0c5c)
#define AFE_ADDA_DL_SDM_FIFO_MON      (AFE_BASE + 0x0c60)
#define AFE_ADDA_DL_SRC_LCH_MON       (AFE_BASE + 0x0c64)
#define AFE_ADDA_DL_SRC_RCH_MON       (AFE_BASE + 0x0c68)
#define AFE_ADDA_DL_SDM_OUT_MON       (AFE_BASE + 0x0c6c)
#define AFE_ADDA_PREDIS_CON2          (AFE_BASE + 0x0d40)
#define AFE_ADDA_PREDIS_CON3          (AFE_BASE + 0x0d44)

#define AFE_MEMIF_MON12               (AFE_BASE + 0x0d70)
#define AFE_MEMIF_MON13               (AFE_BASE + 0x0d74)
#define AFE_MEMIF_MON14               (AFE_BASE + 0x0d78)
#define AFE_MEMIF_MON15               (AFE_BASE + 0x0d7c)
#define AFE_MEMIF_MON16               (AFE_BASE + 0x0d80)
#define AFE_MEMIF_MON17               (AFE_BASE + 0x0d84)
#define AFE_MEMIF_MON18               (AFE_BASE + 0x0d88)
#define AFE_MEMIF_MON19               (AFE_BASE + 0x0d8c)
#define AFE_MEMIF_MON20               (AFE_BASE + 0x0d90)
#define AFE_MEMIF_MON21               (AFE_BASE + 0x0d94)
#define AFE_MEMIF_MON22               (AFE_BASE + 0x0d98)
#define AFE_MEMIF_MON23               (AFE_BASE + 0x0d9c)
#define AFE_MEMIF_MON24               (AFE_BASE + 0x0da0)

#define AFUNC_AUD_CON0                (AFE_BASE + 0x0900)
#define AFUNC_AUD_CON1                (AFE_BASE + 0x0904)
#define AFUNC_AUD_CON2                (AFE_BASE + 0x0908)
#define AFUNC_AUD_CON3                (AFE_BASE + 0x090c)
#define AFUNC_AUD_CON4                (AFE_BASE + 0x0910)

#define AFE_ANA_GAIN_MUX              (AFE_BASE + 0x0e6c)
#define AFE_CLASSG_LPSRCH_CFG0        (AFE_BASE + 0x0ec4)
#define AFE_CLASSG_LPSLCH_CFG0        (AFE_BASE + 0x0ed0)


#define AFE_HD_ENGEN_ENABLE           (AFE_BASE + 0x0dd0)
#define AFE_DMIC_CK_SEL               (AFE_BASE + 0x0f48)
#define AFE_DMIC_DAT_SEL              (AFE_BASE + 0x0f4c)

#define ZCD_CON0                      (AFE_BASE + 0x0f50)
#define ZCD_CON1                      (AFE_BASE + 0x0f54)
#define ZCD_CON2                      (AFE_BASE + 0x0f58)
#define ZCD_CON5                      (AFE_BASE + 0x0f64)
#define ZCD_CON6                      (AFE_BASE + 0x0f68)

#define AFE_MBIST_MODE                (AFE_BASE + 0x0f80)
#define AFE_MBIST_HOLDB               (AFE_BASE + 0x0f84)
#define AFE_MBIST_BACKGROUND          (AFE_BASE + 0x0f88)
#define AFE_MBIST_DONE                (AFE_BASE + 0x0f8c)
#define AFE_MBIST_FAIL                (AFE_BASE + 0x0f90)
#define AFE_MBIST_BSEL                (AFE_BASE + 0x0f94)
#define AFE_DCCLK_CFG                 (AFE_BASE + 0x0f98)
#define AFE_AUDIO_BT_SYNC_CON0        (AFE_BASE + 0x0fd0)
#define AFE_AUDIO_BT_SYNC_MON0        (AFE_BASE + 0x0fd4)
#define AFE_AUDIO_BT_SYNC_MON1        (AFE_BASE + 0x0fd8)
#define AFE_AUDIO_BT_SYNC_MON2        (AFE_BASE + 0x0fdc)
#define AFE_AUDIO_BT_SYNC_MON3        (AFE_BASE + 0x0fe0)
#define AFE_AUDIO_BT_SYNC_MON4        (AFE_BASE + 0x0fe4)

#define MEM_ASRC_TOP_CON0             (AFE_BASE + 0x1000)
#define MEM_ASRC_TOP_CON1             (AFE_BASE + 0x1004)
#define MEM_ASRC_TOP_CON2             (AFE_BASE + 0x1008)
#define MEM_ASRC_TOP_CON3             (AFE_BASE + 0x100c)
#define MEM_ASRC_TOP_MON0             (AFE_BASE + 0x1018)
#define MEM_ASRC_TOP_MON1             (AFE_BASE + 0x101c)
#define PWR2_ASM_CON2                 (AFE_BASE + 0x1074)
#define PWR2_ASM_CON2_MEM_ASRC_1_RESET_POS          (20)
#define PWR2_ASM_CON2_MEM_ASRC_1_RESET_MASK         (1<<PWR2_ASM_CON2_MEM_ASRC_1_RESET_POS)

#define MEM_ASRC_TRAC_CON1            (AFE_BASE + 0x108C)
#define MEM_ASRC_TRAC_CON1_CALC_LRCK_SEL_POS        (0)
#define MEM_ASRC_TRAC_CON1_CALC_LRCK_SEL_MASK       (7<<MEM_ASRC_TRAC_CON1_CALC_LRCK_SEL_POS)

#define ASM_GEN_CONF                  (AFE_BASE + 0x1100)

#define ASM_GEN_CONF_HW_UPDATE_OBUF_RDPNT_POS       (28)
#define ASM_GEN_CONF_HW_UPDATE_OBUF_RDPNT_MASK      (1<<ASM_GEN_CONF_HW_UPDATE_OBUF_RDPNT_POS)
#define ASM_GEN_CONF_CH_CNTX_SWEN_POS               (20)
#define ASM_GEN_CONF_CH_CNTX_SWEN_MASK              (1<<ASM_GEN_CONF_CH_CNTX_SWEN_POS)
#define ASM_GEN_CONF_CH_CLEAR_POS                   (16)
#define ASM_GEN_CONF_CH_CLEAR_MASK                  (1<<ASM_GEN_CONF_CH_CLEAR_POS)
#define ASM_GEN_CONF_CH_EN_POS                      (12)
#define ASM_GEN_CONF_CH_EN_MASK                     (1<<ASM_GEN_CONF_CH_EN_POS)
#define ASM_GEN_CONF_DSP_CTRL_COEFF_SRAM_POS        (11)
#define ASM_GEN_CONF_DSP_CTRL_COEFF_SRAM_MASK       (1<<ASM_GEN_CONF_DSP_CTRL_COEFF_SRAM_POS)
#define ASM_GEN_CONF_ASRC_BUSY_POS                  (9)
#define ASM_GEN_CONF_ASRC_BUSY_MASK                 (1<<ASM_GEN_CONF_ASRC_BUSY_POS)
#define ASM_GEN_CONF_ASRC_EN_POS                    (8)
#define ASM_GEN_CONF_ASRC_EN_MASK                   (1<<ASM_GEN_CONF_ASRC_EN_POS)
#define ASM_IER                       (AFE_BASE + 0x1104)
#define ASM_IER_IBUF_EMPTY_INTEN_POS	            (20)
#define ASM_IER_IBUF_EMPTY_INTEN_MASK	            (1<<ASM_IER_IBUF_EMPTY_INTEN_POS)
#define ASM_IER_IBUF_AMOUNT_INTEN_POS               (16)
#define ASM_IER_IBUF_AMOUNT_INTEN_MASK              (1<<ASM_IER_IBUF_AMOUNT_INTEN_POS)
#define ASM_IER_OBUF_OV_INTEN_POS                   (12)
#define ASM_IER_OBUF_OV_INTEN_MASK                  (1<<ASM_IER_OBUF_OV_INTEN_POS)
#define ASM_IER_OBUF_AMOUNT_INTEN_POS               (8)
#define ASM_IER_OBUF_AMOUNT_INTEN_MASK              (1<<ASM_IER_OBUF_AMOUNT_INTEN_POS)
#define ASM_IFR                       (AFE_BASE + 0x1108)
#define ASM_IFR_IBUF_EMPTY_INT_POS	                (20)
#define ASM_IFR_IBUF_EMPTY_INT_MASK	                (1<<ASM_IFR_IBUF_EMPTY_INT_POS)
#define ASM_IFR_IBUF_AMOUNT_INT_POS                 (16)
#define ASM_IFR_IBUF_AMOUNT_INT_MASK                (1<<ASM_IFR_IBUF_AMOUNT_INT_POS)
#define ASM_IFR_OBUF_OV_INT_POS                     (12)
#define ASM_IFR_OBUF_OV_INT_MASK                    (1<<ASM_IFR_OBUF_OV_INT_POS)
#define ASM_IFR_OBUF_AMOUNT_INT_POS                 (8)
#define ASM_IFR_OBUF_AMOUNT_INT_MASK                (1<<ASM_IFR_OBUF_AMOUNT_INT_POS)
#define ASM_IFR_MASK                                (ASM_IFR_IBUF_EMPTY_INT_MASK|ASM_IFR_IBUF_AMOUNT_INT_MASK|ASM_IFR_OBUF_OV_INT_MASK|ASM_IFR_OBUF_AMOUNT_INT_MASK)

#define ASM_CH01_CNFG                 (AFE_BASE + 0x1110)
#define ASM_CH01_CNFG_CLR_IIR_BUF_POS               (23)
#define ASM_CH01_CNFG_CLR_IIR_BUF_MASK              (1<<ASM_CH01_CNFG_CLR_IIR_BUF_POS)
#define ASM_CH01_CNFG_OBIT_WIDTH_POS                (22)
#define ASM_CH01_CNFG_OBIT_WIDTH_MASK               (1<<ASM_CH01_CNFG_OBIT_WIDTH_POS)
#define ASM_CH01_CNFG_IBIT_WIDTH_POS                (21)
#define ASM_CH01_CNFG_IBIT_WIDTH_MASK               (1<<ASM_CH01_CNFG_IBIT_WIDTH_POS)
#define ASM_CH01_CNFG_MONO_POS                      (20)
#define ASM_CH01_CNFG_MONO_MASK                     (1<<ASM_CH01_CNFG_MONO_POS)
#define ASM_CH01_CNFG_OFS_POS                       (18)
#define ASM_CH01_CNFG_OFS_MASK                      (3<<ASM_CH01_CNFG_OFS_POS)
#define ASM_CH01_CNFG_IFS_POS                       (16)
#define ASM_CH01_CNFG_IFS_MASK                      (3<<ASM_CH01_CNFG_IFS_POS)
#define ASM_CH01_CNFG_CLAC_AMOUNT_POS               (8)
#define ASM_CH01_CNFG_CLAC_AMOUNT_MASK              (0xFF<<ASM_CH01_CNFG_CLAC_AMOUNT_POS)
#define ASM_CH01_CNFG_IIR_EN_POS                    (7)
#define ASM_CH01_CNFG_IIR_EN_MASK                   (1<<ASM_CH01_CNFG_IIR_EN_POS)
#define ASM_CH01_CNFG_IIR_STAGE_POS                 (4)
#define ASM_CH01_CNFG_IIR_STAGE_MASK                (7<<ASM_CH01_CNFG_IIR_STAGE_POS)
#define ASM_FREQUENCY_0               (AFE_BASE + 0x1120)
#define ASM_FREQUENCY_1               (AFE_BASE + 0x1124)
#define ASM_FREQUENCY_2               (AFE_BASE + 0x1128)
#define ASM_FREQUENCY_3               (AFE_BASE + 0x112c)
#define ASM_IBUF_SADR                 (AFE_BASE + 0x1130)
#define ASM_IBUF_SADR_POS                           (0)//align 128-bit
#define ASM_IBUF_SADR_MASK                          (0xFFFFFFFF<<ASM_IBUF_SADR_POS)
#define ASM_IBUF_SIZE                 (AFE_BASE + 0x1134)
#define ASM_IBUF_SIZE_POS                           (0)
#define ASM_IBUF_SIZE_MASK                          (0xFFFFF<<ASM_IBUF_SIZE_POS)
#define ASM_OBUF_SADR                 (AFE_BASE + 0x1138)
#define ASM_OBUF_SADR_POS                           (0)
#define ASM_OBUF_SADR_MASK                          (0xFFFFFFFF<<ASM_OBUF_SADR_POS)
#define ASM_OBUF_SIZE                 (AFE_BASE + 0x113c)
#define ASM_OBUF_SIZE_POS                           (0)
#define ASM_OBUF_SIZE_MASK                          (0xFFFFF<<ASM_OBUF_SIZE_POS)
#define ASM_CH01_IBUF_RDPNT           (AFE_BASE + 0x1140)
#define ASM_CH01_IBUF_RDPNT_POS                     (0)
#define ASM_CH01_IBUF_RDPNT_MASK                    (0xFFFFFFFF<<ASM_CH01_IBUF_RDPNT_POS)
#define ASM_CH01_IBUF_WRPNT           (AFE_BASE + 0x1150)
#define ASM_CH01_IBUF_WRPNT_POS                     (0)
#define ASM_CH01_IBUF_WRPNT_MASK                    (0xFFFFFFFF<<ASM_CH01_IBUF_WRPNT_POS)
#define ASM_CH01_OBUF_WRPNT           (AFE_BASE + 0x1160)
#define ASM_CH01_OBUF_WRPNT_POS                     (0)
#define ASM_CH01_OBUF_WRPNT_MASK                    (0xFFFFFFFF<<ASM_CH01_OBUF_WRPNT_POS)
#define ASM_CH01_OBUF_RDPNT           (AFE_BASE + 0x1170)
#define ASM_CH01_OBUF_RDPNT_POS                     (0)
#define ASM_CH01_OBUF_RDPNT_MASK                    (0xFFFFFFFF<<ASM_CH01_OBUF_RDPNT_POS)
#define ASM_IBUF_INTR_CNT0            (AFE_BASE + 0x1180)
#define ASM_IBUF_INTR_CNT0_POS                      (8)
#define ASM_IBUF_INTR_CNT0_MASK                     (0x3FFF<<ASM_IBUF_INTR_CNT0_POS)
#define ASM_OBUF_INTR_CNT0            (AFE_BASE + 0x1188)
#define ASM_OBUF_INTR_CNT0_POS                      (8)
#define ASM_OBUF_INTR_CNT0_MASK                     (0x3FFF<<ASM_OBUF_INTR_CNT0_POS)
#define ASM_BAK_REG                   (AFE_BASE + 0x1190)
#define ASM_BAK_REG_RESULT_SEL_POS                  (0)
#define ASM_BAK_REG_RESULT_SEL_MASK                 (7<<ASM_BAK_REG_RESULT_SEL_POS)
#define ASM_FREQ_CALI_CTRL            (AFE_BASE + 0x1194)
#define ASM_FREQ_CALI_CTRL_FREQ_CALC_BUSY_POS       (20)
#define ASM_FREQ_CALI_CTRL_FREQ_CALC_BUSY_MASK      (1<<ASM_FREQ_CALI_CTRL_FREQ_CALC_BUSY_POS)
#define ASM_FREQ_CALI_CTRL_COMP_FREQRES_EN_POS      (19)
#define ASM_FREQ_CALI_CTRL_COMP_FREQRES_EN_MASK     (1<<ASM_FREQ_CALI_CTRL_COMP_FREQRES_EN_POS)
#define ASM_FREQ_CALI_CTRL_SRC_SEL_POS              (16)
#define ASM_FREQ_CALI_CTRL_SRC_SEL_MASK             (3<<ASM_FREQ_CALI_CTRL_SRC_SEL_POS)
#define ASM_FREQ_CALI_CTRL_BYPASS_DEGLITCH_POS      (15)
#define ASM_FREQ_CALI_CTRL_BYPASS_DEGLITCH_MASK     (1<<ASM_FREQ_CALI_CTRL_BYPASS_DEGLITCH_POS)
#define ASM_FREQ_CALI_CTRL_MAX_GWIDTH_POS           (12)
#define ASM_FREQ_CALI_CTRL_MAX_GWIDTH_MASK          (7<<ASM_FREQ_CALI_CTRL_MAX_GWIDTH_POS)
#define ASM_FREQ_CALI_CTRL_AUTO_FS2_UPDATE_POS      (11)
#define ASM_FREQ_CALI_CTRL_AUTO_FS2_UPDATE_MASK     (1<<ASM_FREQ_CALI_CTRL_AUTO_FS2_UPDATE_POS)
#define ASM_FREQ_CALI_CTRL_AUTO_RESTART_POS         (10)
#define ASM_FREQ_CALI_CTRL_AUTO_RESTART_MASK        (1<<ASM_FREQ_CALI_CTRL_AUTO_RESTART_POS)
#define ASM_FREQ_CALI_CTRL_FREQ_UPDATE_FS2_POS      (9)
#define ASM_FREQ_CALI_CTRL_FREQ_UPDATE_FS2_MASK     (1<<ASM_FREQ_CALI_CTRL_FREQ_UPDATE_FS2_POS)
#define ASM_FREQ_CALI_CTRL_CALI_EN_POS              (8)
#define ASM_FREQ_CALI_CTRL_CALI_EN_MASK             (1<<ASM_FREQ_CALI_CTRL_CALI_EN_POS)
#define ASM_FREQ_CALI_CYC             (AFE_BASE + 0x1198)
#define ASM_FREQ_CALI_CYC_POS                       (8)
#define ASM_FREQ_CALI_CYC_MASK                      (0xFFFF<<ASM_FREQ_CALI_CYC_POS)
#define ASM_PRD_CALI_RESULT           (AFE_BASE + 0x119c)
#define ASM_PRD_CALI_RESULT_POS                     (0)
#define ASM_PRD_CALI_RESULT_MASK                    (0xFFFFFF<<ASM_PRD_CALI_RESULT_POS)
#define ASM_FREQ_CALI_RESULT          (AFE_BASE + 0x11a0)
#define ASM_FREQ_CALI_RESULT_POS                    (0)
#define ASM_FREQ_CALI_RESULT_MASK                   (0xFFFFFF<<ASM_FREQ_CALI_RESULT_POS)
#define ASM_CALI_DENOMINATOR          (AFE_BASE + 0x11d8)
#define ASM_CALI_DENOMINATOR_POS                    (0)
#define ASM_CALI_DENOMINATOR_MASK                   (0xFFFFFF<<ASM_CALI_DENOMINATOR_POS)
#define ASM_MAX_OUT_PER_IN0           (AFE_BASE + 0x11e0)
#define ASM_MAX_OUT_PER_IN0_POS                     (8)
#define ASM_MAX_OUT_PER_IN0_MASK                    (0xF<<ASM_MAX_OUT_PER_IN0_POS)
#define ASM_IN_BUF_MON0               (AFE_BASE + 0x11e8)
#define ASM_IN_BUF_MON1               (AFE_BASE + 0x11ec)
#define ASM_IIR_CRAM_ADDR             (AFE_BASE + 0x11f0)
#define ASM_IIR_CRAM_ADDR_POS                       (8)
#define ASM_IIR_CRAM_ADDR_MASK                      (0xFF<<ASM_IIR_CRAM_ADDR_POS)
#define ASM_IIR_CRAM_DATA             (AFE_BASE + 0x11f4)
#define ASM_IIR_CRAM_DATA_POS                       (0)
#define ASM_IIR_CRAM_DATA_MASK                      (0xFFFFFFFF<<ASM_IIR_CRAM_DATA_POS)
#define ASM_OUT_BUF_MON0              (AFE_BASE + 0x11f8)
#define ASM_OUT_BUF_MON0_WDLE_CNT_POS               (8)
#define ASM_OUT_BUF_MON0_WDLE_CNT_MASK              (0xFF<<ASM_OUT_BUF_MON0_WDLE_CNT_POS)
#define ASM_OUT_BUF_MON0_ASRC_WRITE_DONE_POS        (0)
#define ASM_OUT_BUF_MON0_ASRC_WRITE_DONE_MASK       (1<<ASM_OUT_BUF_MON0_ASRC_WRITE_DONE_POS)
#define ASM_OUT_BUF_MON1              (AFE_BASE + 0x11fc)
#define ASM_OUT_BUF_MON1_ASRC_WR_ADR_POS            (4)
#define ASM_OUT_BUF_MON1_ASRC_WR_ADR_MASK           (0x0FFFFFFF<<ASM_OUT_BUF_MON1_ASRC_WR_ADR_POS)
#define ASM2_GEN_CONF                 (AFE_BASE + 0x1200)
#define ASM2_IER                      (AFE_BASE + 0x1204)
#define ASM2_IFR                      (AFE_BASE + 0x1208)
#define ASM2_CH01_CNFG                (AFE_BASE + 0x1210)
#define ASM2_FREQUENCY_0              (AFE_BASE + 0x1220)
#define ASM2_FREQUENCY_1              (AFE_BASE + 0x1224)
#define ASM2_FREQUENCY_2              (AFE_BASE + 0x1228)
#define ASM2_FREQUENCY_3              (AFE_BASE + 0x122c)
#define ASM2_IBUF_SADR                (AFE_BASE + 0x1230)
#define ASM2_IBUF_SIZE                (AFE_BASE + 0x1234)
#define ASM2_OBUF_SADR                (AFE_BASE + 0x1238)
#define ASM2_OBUF_SIZE                (AFE_BASE + 0x123c)
#define ASM2_CH01_IBUF_RDPNT          (AFE_BASE + 0x1240)
#define ASM2_CH01_IBUF_WRPNT          (AFE_BASE + 0x1250)
#define ASM2_CH01_OBUF_WRPNT          (AFE_BASE + 0x1260)
#define ASM2_CH01_OBUF_RDPNT          (AFE_BASE + 0x1270)
#define ASM2_IBUF_INTR_CNT0           (AFE_BASE + 0x1280)
#define ASM2_OBUF_INTR_CNT0           (AFE_BASE + 0x1288)
#define ASM2_BAK_REG                  (AFE_BASE + 0x1290)
#define ASM2_FREQ_CALI_CTRL           (AFE_BASE + 0x1294)
#define ASM2_FREQ_CALI_CYC            (AFE_BASE + 0x1298)
#define ASM2_PRD_CALI_RESULT          (AFE_BASE + 0x129c)
#define ASM2_FREQ_CALI_RESULT         (AFE_BASE + 0x12a0)
#define ASM2_CALI_DENOMINATOR         (AFE_BASE + 0x12d8)
#define ASM2_MAX_OUT_PER_IN0          (AFE_BASE + 0x12e0)
#define ASM2_IN_BUF_MON0              (AFE_BASE + 0x12e8)
#define ASM2_IN_BUF_MON1              (AFE_BASE + 0x12ec)
#define ASM2_IIR_CRAM_ADDR            (AFE_BASE + 0x12f0)
#define ASM2_IIR_CRAM_DATA            (AFE_BASE + 0x12f4)
#define ASM2_OUT_BUF_MON0             (AFE_BASE + 0x12f8)
#define ASM2_OUT_BUF_MON1             (AFE_BASE + 0x12fc)

#define AFE_MAXLENGTH                 (AFE_BASE + 0x1320)
#define AFE_REG_UNDEFINED             (AFE_MAXLENGTH + 0x1)
/*********************
Analog Reg definition
*********************/

#define AUDENC_ANA_CON0               (ABB_BASE + 0x0100)
#define AUDENC_ANA_CON1               (ABB_BASE + 0x0104)
#define AUDENC_ANA_CON2               (ABB_BASE + 0x0108)
#define AUDENC_ANA_CON3               (ABB_BASE + 0x010C)
#define AUDENC_ANA_CON4               (ABB_BASE + 0x0110)
#define AUDENC_ANA_CON5               (ABB_BASE + 0x0114)
#define AUDENC_ANA_CON6               (ABB_BASE + 0x0118)
#define AUDENC_ANA_CON7               (ABB_BASE + 0x011C)
#define AUDENC_ANA_CON8               (ABB_BASE + 0x0120)
#define AUDENC_ANA_CON9               (ABB_BASE + 0x0124)
#define AUDENC_ANA_CON10              (ABB_BASE + 0x0128)
#define AUDENC_ANA_CON11              (ABB_BASE + 0x012C)
#define AUDENC_ANA_CON12              (ABB_BASE + 0x0130)
#define AUDENC_ANA_CON13              (ABB_BASE + 0x0134)
#define AUDENC_ANA_CON14              (ABB_BASE + 0x0138)

#define AUDDEC_ANA_CON0               (ABB_BASE + 0x0200)
#define AUDDEC_ANA_CON1               (ABB_BASE + 0x0204)
#define AUDDEC_ANA_CON2               (ABB_BASE + 0x0208)
#define AUDDEC_ANA_CON3               (ABB_BASE + 0x020C)
#define AUDDEC_ANA_CON4               (ABB_BASE + 0x0210)
#define AUDDEC_ANA_CON5               (ABB_BASE + 0x0214)
#define AUDDEC_ANA_CON6               (ABB_BASE + 0x0218)
#define AUDDEC_ANA_CON7               (ABB_BASE + 0x021C)
#define AUDDEC_ANA_CON8               (ABB_BASE + 0x0220)
#define AUDDEC_ANA_CON9               (ABB_BASE + 0x0224)
#define AUDDEC_ANA_CON10              (ABB_BASE + 0x0228)
#define AUDDEC_ANA_CON11              (ABB_BASE + 0x022C)
#define AUDDEC_ANA_CON12              (ABB_BASE + 0x0230)
#define AUDDEC_ANA_CON13              (ABB_BASE + 0x0234)
#define AUDDEC_ANA_CON14              (ABB_BASE + 0x0238)

#define PMU2_ANA_CON0                 (ABB_BASE + 0x0500)
#define PMU2_ANA_RO                   (ABB_BASE + 0x0510)

/*********************
PLL Reg definition
*********************/
#define APLL1_CTL0__F_RG_APLL1_DDS_PWR_ON               (XPLL_CTRL_BASE+0x0000)
#define APPL1_CTL0__F_RG_APLL1_DDS_ISO_EN               (XPLL_CTRL_BASE+0x0001)
#define APLL1_CTL0__F_RG_APLL1_V2I_EN                   (XPLL_CTRL_BASE+0x0003)
#define APLL1_CTL1__F_RG_APLL1_EN                       (XPLL_CTRL_BASE+0x0004)
#define APLL1_CTL11__F_RG_APLL1_LCDDS_PWDB              (XPLL_CTRL_BASE+0x002C)

#define APLL1_CTL10__F_RG_APLL1_LCDDS_TUNER_PCW_NCPO    (XPLL_CTRL_BASE+0x0028)
#define APLL1_CTL14__F_RG_APLL1_LCDDS_TUNER_PCW_NCPO    (XPLL_CTRL_BASE+0x0038)
#define APLL1_CTL12__F_RG_APLL1_LCDDS_TUNER_PCW_NCPO    (XPLL_CTRL_BASE+0x0030)
#define APLL1_CTL13__F_RG_APLL1_LCDDS_TUNER_EN          (XPLL_CTRL_BASE+0x0034)

#define APLL2_CTL0__F_RG_APLL2_DDS_PWR_ON               (XPLL_CTRL_BASE+0x0100)
#define APPL2_CTL0__F_RG_APLL2_DDS_ISO_EN               (XPLL_CTRL_BASE+0x0101)
#define APLL2_CTL0__F_RG_APLL2_V2I_EN                   (XPLL_CTRL_BASE+0x0103)
#define APLL2_CTL1__F_RG_APLL2_EN                       (XPLL_CTRL_BASE+0x0104)
#define APLL2_CTL11__F_RG_APLL2_LCDDS_PWDB              (XPLL_CTRL_BASE+0x012C)

#define APLL2_CTL10__F_RG_APLL2_LCDDS_TUNER_PCW_NCPO    (XPLL_CTRL_BASE+0x0128)
#define APLL2_CTL14__F_RG_APLL2_LCDDS_TUNER_PCW_NCPO    (XPLL_CTRL_BASE+0x0138)
#define APLL2_CTL12__F_RG_APLL2_LCDDS_TUNER_PCW_NCPO    (XPLL_CTRL_BASE+0x0130)
#define APLL2_CTL13__F_RG_APLL2_LCDDS_TUNER_EN          (XPLL_CTRL_BASE+0x0134)
#endif
#endif //#ifdef HAL_AUDIO_MODULE_ENABLED
#endif /* __HAL_AUDIO_AFE_DEF_H__ */
