/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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

#if defined(AIR_GAMING_MODE_DONGLE_ENABLE)

/* Includes ------------------------------------------------------------------*/
#include "types.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "dsp_buffer.h"
#include "dsp_memory.h"
#include "dsp_callback.h"
#include "dsp_temp.h"
#include "dsp_dump.h"
#include "dsp_scenario.h"
#include "dsp_audio_msg_define.h"
#include "dsp_audio_process.h"
#include "stream_audio_transmitter.h"
#include "bt_types.h"
#include "hal_gpt.h"
#include "hal_sleep_manager.h"
#include "memory_attribute.h"
#include "scenario_ull_audio.h"
#include "stream_bt_common.h"
#ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
#include "volume_estimator_interface.h"
#include "dtm.h"
#include "preloader_pisplit.h"
#include "audio_nvdm_common.h"
#endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */
#ifdef MTK_CELT_DEC_ENABLE
#include "celt_dec_interface.h"
#endif /* MTK_CELT_DEC_ENABLE */

/* Private define ------------------------------------------------------------*/
#define GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG          0
#define GAMING_MODE_MUSIC_DONGLE_DEBUG_DUMP         1
#define GAMING_MODE_MUSIC_DONGLE_DEBUG_LANTENCY     0
#define GAMING_MODE_VOICE_DONGLE_DEBUG_LOG          0
#define GAMING_MODE_VOICE_DONGLE_DEBUG_DUMP         1

// #define GAMING_MODE_VOICE_PLAYEN_DELAY              (24*2)
// #define GAMING_MODE_VOICE_FIRST_PACKET_SAFE_DELAY   (24*2)
#define GAMING_MODE_VOICE_PLAYEN_DELAY              (24+13)
#define GAMING_MODE_VOICE_FIRST_PACKET_SAFE_DELAY   (24+8)
#define GAMING_MODE_VOICE_USB_FRAME_SAMPLES         (40)
#define GAMING_MODE_VOICE_BT_FRAME_SAMPLES          (120)
#define GAMING_MODE_VOICE_FRAME_INDEX_MAX           (3)

#ifndef min
#define min(_a, _b)   (((_a)<(_b))?(_a):(_b))
#endif

/* Private typedef -----------------------------------------------------------*/
typedef enum {
    GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_L = 0,
    GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_R,
    GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_DUL,
    GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_L,
    GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_R,
    GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_DUL,
    GAMING_MODE_CONFIG_OP_MUSIC_MIX,
    GAMING_MODE_CONFIG_OP_MUSIC_UNMIX,
    GAMING_MODE_CONFIG_OP_LATENCY_SWITCH,
    GAMING_MODE_CONFIG_OP_MAX               = 0xffffffff
} gaming_mode_runtime_config_operation_t;

typedef struct {
    int32_t gain_1;
    int32_t gain_2;
} op_vol_level_param_t;

typedef struct {
    uint8_t scenario_id;
    uint8_t sub_id;
} dl_mixer_param_t;

typedef struct {
    uint32_t latency_us;
} latency_param_t;

typedef union {
    op_vol_level_param_t      vol_level;
    dl_mixer_param_t          dl_mixer_param;
    latency_param_t           latency_param;
} gaming_mode_runtime_config_operation_param_t, *gaming_mode_runtime_config_operation_param_p;

typedef struct {
    gaming_mode_runtime_config_operation_t          config_operation;
    gaming_mode_runtime_config_operation_param_t    config_param;
} gaming_mode_runtime_config_param_t, *gaming_mode_runtime_config_param_p;

typedef struct{
    uint16_t DCXO_CAPID_EFUSE_SEL;
    uint16_t DCXO_CAPID_EFUSE;
}__attribute__((packed)) dcxo_capid_efuse_t;
#define DCXO_CAPID_EFUSE_REG            (*((dcxo_capid_efuse_t*)0xA2060018))

#ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
typedef struct {
    int16_t enable;
    uint16_t gain_setting_update_done;
    volume_estimator_port_t *port;
    sw_gain_port_t *game_path_gain_port;
    void *nvkey_buf;
    uint32_t data_size;
    void *data_buf;
    int32_t current_db;
    int32_t failure_threshold_db;
    int32_t effective_threshold_db;
    int32_t adjustment_amount_db;
    int32_t up_step_db;
    int32_t down_step_db;
    uint32_t effective_delay_ms;
    uint32_t effective_delay_us_count;
    uint32_t failure_delay_ms;
    uint32_t failure_delay_us_count;
    bool active_flag;
    op_vol_level_param_t chat_path_default_vol_level;
    op_vol_level_param_t game_path_default_vol_level;
    op_vol_level_param_t game_path_balance_vol_level;
} gaming_mode_vol_balance_handle_t;

typedef struct {
    uint16_t *data_buf_1_head;
    uint16_t *data_buf_1_tail;
    uint16_t *data_buf_1_cur;
    uint16_t *data_buf_2_head;
    uint16_t *data_buf_2_tail;
    uint16_t *data_buf_2_cur;
} gaming_mode_vol_balance_buffer_msg_t;

typedef struct {
    int32_t     enable;
    int32_t     effective_threshold_db;
    uint32_t    effective_delay_ms;
    int32_t     failure_threshold_db;
    uint32_t    failure_delay_ms;
    int32_t     adjustment_amount_db;
    int32_t     up_step_db;
    int32_t     down_step_db;
    DSP_PARA_GAME_CHAT_VOL_STRU chat_vol_nvkey;
} gaming_mode_vol_balance_nvkey_t;
#endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t crc32_tab[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

/* Public variables ----------------------------------------------------------*/
volatile uint32_t gaming_mode_music_ccni_bt_clk = 0;
volatile uint32_t gaming_mode_music_ccni_gpt_count = 0;
volatile uint32_t gaming_mode_music_data_in_gpt_count = 0;
gaming_mode_dongle_dl_handle_t *gaming_mode_dongle_first_dl_handle = NULL;
gaming_mode_dongle_ul_handle_t *gaming_mode_dongle_first_ul_handle = NULL;
#if GAMING_MODE_MUSIC_DONGLE_DEBUG_LANTENCY
volatile uint32_t gaming_mode_music_ccni_gpt_count1 = 0;
volatile uint32_t gaming_mode_music_ccni_gpt_count2[2];
#endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LANTENCY */
volatile uint32_t gaming_mode_music_bt_out_gpt_count = 0;
volatile uint32_t gaming_mode_music_first_packet_bt_clk = 0;
volatile uint32_t gaming_mode_voice_usb_out_gpt_count = 0;
SINK gaming_mode_dongle_sink = NULL;
#ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
gaming_mode_vol_balance_buffer_msg_t gaming_mode_vol_buffer_msg;
gaming_mode_vol_balance_handle_t gaming_mode_vol_balance_handle;
#endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

#ifdef AIR_GAMING_MODE_DONGLE_ECNR_ENABLE
static uint8_t g_post_ec_gain;
#endif /* AIR_GAMING_MODE_DONGLE_ECNR_ENABLE */

/* Private functions ---------------------------------------------------------*/
#ifdef AIR_GAMING_MODE_DONGLE_ECNR_ENABLE
uint8_t AEC_NR_Port_Get_PostEC_Gain(void)
{
    return g_post_ec_gain;
}
#endif /* AIR_GAMING_MODE_DONGLE_ECNR_ENABLE */

static gaming_mode_dongle_ul_first_packet_status_t gaming_mode_dongle_ul_first_packet_check(gaming_mode_dongle_ul_handle_t *dongle_handle, uint32_t anchor)
{
    uint32_t bt_clk;
    uint16_t bt_phase;
    uint32_t anchor_safe;
    uint32_t anchor_last;
    UNUSED(dongle_handle);

    /* get current bt clock */
    extern VOID MCE_GetBtClk(BTCLK* pCurrCLK, BTPHASE* pCurrPhase,BT_CLOCK_OFFSET_SCENARIO type);
    MCE_GetBtClk((BTCLK*)&bt_clk, (BTPHASE*)&bt_phase, BT_CLK_Offset);

    anchor_safe = (anchor+GAMING_MODE_VOICE_FIRST_PACKET_SAFE_DELAY) & 0x0fffffff;
    anchor_last = (anchor+0x10000000-24) & 0x0fffffff;

    if ((anchor_safe > anchor) && (anchor > anchor_last))
    {
        if ((bt_clk >= anchor_last) && (bt_clk < anchor_safe))
        {
            /* --------- ........ --------- anchor_last --------- anchor ---------- bt_clk ---------- anchor_safe --------- ........ -------- */
            return GAMING_MODE_DONGLE_UL_FIRST_PACKET_READY;
        }
        else
        {
            /* --------- ........ --------- anchor_last --------- anchor ---------- anchor_safe ---------- bt_clk --------- ........ -------- */
            return GAMING_MODE_DONGLE_UL_FIRST_PACKET_TIMEOUT;
        }
    }
    else if ((anchor_safe > anchor) && (anchor < anchor_last))
    {
        if ((bt_clk >= anchor) && (bt_clk < anchor_safe))
        {
            /* --------- ........ --------- anchor ---------- bt_clk ---------- anchor_safe --------- ........ --------- anchor_last -------- */
            return GAMING_MODE_DONGLE_UL_FIRST_PACKET_READY;
        }
        else if (bt_clk >= anchor_last)
        {
            /* --------- ........ --------- anchor  ---------- anchor_safe --------- ........ --------- anchor_last ---------- bt_clk -------- */
            return GAMING_MODE_DONGLE_UL_FIRST_PACKET_READY;
        }
        else if (bt_clk < anchor)
        {
            /* ---------- bt_clk --------- ........ --------- anchor  ---------- anchor_safe --------- ........ --------- anchor_last  -------- */
            return GAMING_MODE_DONGLE_UL_FIRST_PACKET_READY;
        }
        else
        {
            /* --------- ........ --------- anchor  ---------- anchor_safe ---------- bt_clk --------- ........ --------- anchor_last -------- */
            return GAMING_MODE_DONGLE_UL_FIRST_PACKET_TIMEOUT;
        }
    }
    else
    {
        if ((bt_clk >= anchor) || (bt_clk < anchor_safe))
        {
            /* --------- anchor_safe --------- ........ ---------- ........ --------- anchor -------- bt_clk ---------- */
            return GAMING_MODE_DONGLE_UL_FIRST_PACKET_READY;
        }
        else if ((bt_clk < anchor) && (bt_clk >= anchor_last))
        {
            /* --------- anchor_safe --------- ........ ---------- ........ --------- anchor_last -------- bt_clk --------- anchor  ---------- */
            return GAMING_MODE_DONGLE_UL_FIRST_PACKET_READY;
        }
        else
        {
            /* --------- anchor_safe --------- ........ -------- bt_clk ---------- ........ --------- anchor_last --------- anchor  ---------- */
            return GAMING_MODE_DONGLE_UL_FIRST_PACKET_TIMEOUT;
        }
    }

    return GAMING_MODE_DONGLE_UL_FIRST_PACKET_NOT_READY;
}

static uint32_t gaming_mode_dongle_ul_check_play(gaming_mode_dongle_ul_handle_t *dongle_handle)
{
    uint32_t bt_clk;
    uint16_t bt_phase;

    /* get current bt clock */
    extern VOID MCE_GetBtClk(BTCLK* pCurrCLK, BTPHASE* pCurrPhase,BT_CLOCK_OFFSET_SCENARIO type);
    MCE_GetBtClk((BTCLK*)&bt_clk, (BTPHASE*)&bt_phase, BT_CLK_Offset);

    /* Note: first_packet_bt_clk should be later than first_anchor_bt_clk */
    // if (bt_clk >= dongle_handle->first_packet_bt_clk)
    // {
    //     if ((bt_clk - dongle_handle->first_packet_bt_clk) >= GAMING_MODE_VOICE_PLAYEN_DELAY)
    //     {
    //         /* --------- ........ --------- first_packet_bt_clk ---------- >= GAMING_MODE_VOICE_PLAYEN_DELAY ---------- bt_clk --------- ........ -------- */
    //         return 1;
    //     }
    // }
    // else
    // {
    //     if ((bt_clk + 0x0fffffff + 1 - dongle_handle->first_packet_bt_clk) >= GAMING_MODE_VOICE_PLAYEN_DELAY)
    //     {
    //         /* --------- >= GAMING_MODE_VOICE_PLAYEN_DELAY --------- bt_clk ---------- ........ ---------- first_packet_bt_clk --------- ........ -------- */
    //         return 1;
    //     }
    // }

    if (dongle_handle->play_en_overflow)
    {
        if ((dongle_handle->play_en_bt_clk <= bt_clk) && (dongle_handle->first_anchor_bt_clk <= dongle_handle->first_packet_bt_clk) && (dongle_handle->first_anchor_bt_clk > bt_clk))
        {
            /* --------- play_en_bt_clk --------- ........ ---------- bt_clk ---------- ........ --------- first_anchor_bt_clk -------- first_packet_bt_clk -------- */
            return 1;
        }
        else if ((dongle_handle->play_en_bt_clk <= bt_clk) && (dongle_handle->first_anchor_bt_clk > dongle_handle->first_packet_bt_clk) && (dongle_handle->first_packet_bt_clk > bt_clk))
        {
            /* --------- play_en_bt_clk --------- ........ ---------- bt_clk ---------- ........ -------- first_packet_bt_clk --------- first_anchor_bt_clk  -------- */
            return 1;
        }
    }
    else
    {
        if (dongle_handle->play_en_bt_clk <= bt_clk)
        {
            /* --------- ........ --------- first_anchor_bt_clk -------- first_packet_bt_clk ---------- play_en_bt_clk ---------- bt_clk --------- ........ -------- */
            /* --------- ........ --------- first_packet_bt_clk -------- first_anchor_bt_clk ---------- play_en_bt_clk ---------- bt_clk --------- ........ -------- */
            return 1;
        }
        else if ((dongle_handle->play_en_bt_clk > bt_clk) && (dongle_handle->first_anchor_bt_clk > bt_clk) && (dongle_handle->first_packet_bt_clk > bt_clk))
        {
            /* --------- bt_clk --------- ........ -------- first_packet_bt_clk ---------- first_anchor_bt_clk ---------- play_en_bt_clk --------- ........ -------- */
            return 1;
        }

    }

    return 0;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 static uint32_t CRC32_Generate(uint8_t *ptr, uint32_t length, uint32_t crc_init)
{
    const uint8_t *p;

    p = ptr;
    crc_init = crc_init ^ ~0U;

    while (length--) {
        crc_init = crc32_tab[(crc_init ^ *p++) & 0xFF] ^ (crc_init >> 8);
    }

    return crc_init ^ ~0U;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 void ShareBufferCopy_I2D_16bit(uint32_t* src_buf, uint16_t* dest_buf1, uint16_t* dest_buf2, uint32_t samples)
{
    uint32_t data;
    uint32_t i;

    for (i = 0; i < samples; i++)
    {
        data = src_buf[i];
        dest_buf1[i] = (uint16_t)(data>>0);
        dest_buf2[i] = (uint16_t)(data>>16);
    }
}

/* Public functions ----------------------------------------------------------*/
/******************************************************************************/
/*           Gaming mode dongle game/chat volume balance functions            */
/******************************************************************************/
#ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
void gaming_mode_dongle_game_chat_volume_smart_balance_enable(void *msg)
{
    uint32_t saved_mask;
    void *owner = msg;
    volume_estimator_port_t *port = NULL;
    volume_estimator_config_t config;
    void *data_buf;
    void *nvkey_buf;

    nvkey_buf = preloader_pisplit_malloc_memory( PRELOADER_D_HIGH_PERFORMANCE , sizeof(gaming_mode_vol_balance_nvkey_t));
    if (nvkey_buf == NULL)
    {
        configASSERT(0);
    }
    if (nvkey_read_full_key(NVKEY_DSP_PARA_GAME_CHAT_VOLUME_SMART_BALANCE, nvkey_buf, sizeof(gaming_mode_vol_balance_nvkey_t)) != NVDM_STATUS_NAT_OK)
    {
        configASSERT(0);
    }

    /* get chat volume estimator port */
    port = volume_estimator_get_port(owner);
    if (port == NULL)
    {
        configASSERT(0);
    }

    /* init chat volume estimator port */
    config.channel_num = 2;
    config.frame_size = 480; /* 2.5ms*48K*stereo*16bit */
    config.resolution = RESOLUTION_16BIT;
    config.mode = VOLUME_ESTIMATOR_CHAT_INSTANT_MODE;
    config.sample_rate = 48000;
    config.nvkey_para = (void *)&(((gaming_mode_vol_balance_nvkey_t *)nvkey_buf)->chat_vol_nvkey);
    volume_estimator_init(port, &config);

    /* malloc memory for queue stereo pcm data */
    data_buf = preloader_pisplit_malloc_memory( PRELOADER_D_HIGH_PERFORMANCE, 480);
    if (data_buf == NULL)
    {
        configASSERT(0);
    }

    /* update state machine */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (gaming_mode_vol_balance_handle.port != NULL)
    {
        configASSERT(0);
    }
    gaming_mode_vol_balance_handle.port = port;
    gaming_mode_vol_balance_handle.nvkey_buf = nvkey_buf;
    gaming_mode_vol_balance_handle.enable = (int16_t)(((gaming_mode_vol_balance_nvkey_t *)nvkey_buf)->enable);
    gaming_mode_vol_balance_handle.gain_setting_update_done = 0;
    gaming_mode_vol_balance_handle.current_db = -144*100;
    gaming_mode_vol_balance_handle.effective_threshold_db = ((gaming_mode_vol_balance_nvkey_t *)nvkey_buf)->effective_threshold_db;
    gaming_mode_vol_balance_handle.effective_delay_ms = ((gaming_mode_vol_balance_nvkey_t *)nvkey_buf)->effective_delay_ms;
    gaming_mode_vol_balance_handle.effective_delay_us_count = 0;
    gaming_mode_vol_balance_handle.failure_threshold_db = ((gaming_mode_vol_balance_nvkey_t *)nvkey_buf)->failure_threshold_db;
    gaming_mode_vol_balance_handle.failure_delay_ms = ((gaming_mode_vol_balance_nvkey_t *)nvkey_buf)->failure_delay_ms;
    gaming_mode_vol_balance_handle.failure_delay_us_count = 0;
    gaming_mode_vol_balance_handle.adjustment_amount_db = ((gaming_mode_vol_balance_nvkey_t *)nvkey_buf)->adjustment_amount_db;
    gaming_mode_vol_balance_handle.up_step_db = ((gaming_mode_vol_balance_nvkey_t *)nvkey_buf)->up_step_db;
    gaming_mode_vol_balance_handle.down_step_db = ((gaming_mode_vol_balance_nvkey_t *)nvkey_buf)->down_step_db;
    gaming_mode_vol_balance_handle.active_flag = false;
    gaming_mode_vol_balance_handle.data_size = 480;
    gaming_mode_vol_balance_handle.data_buf = data_buf;
    hal_nvic_restore_interrupt_mask(saved_mask);

    DSP_MW_LOG_I("[Game/Chat balance][enable]:%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 16,
                gaming_mode_vol_balance_handle.enable,
                gaming_mode_vol_balance_handle.active_flag,
                gaming_mode_vol_balance_handle.current_db,
                gaming_mode_vol_balance_handle.effective_threshold_db,
                gaming_mode_vol_balance_handle.effective_delay_ms,
                gaming_mode_vol_balance_handle.failure_threshold_db,
                gaming_mode_vol_balance_handle.failure_delay_ms,
                gaming_mode_vol_balance_handle.adjustment_amount_db,
                gaming_mode_vol_balance_handle.up_step_db,
                gaming_mode_vol_balance_handle.down_step_db,
                gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_1,
                gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_2,
                gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1,
                gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2,
                gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1,
                gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2);
}

void gaming_mode_dongle_game_chat_volume_smart_balance_disable(void *msg)
{
    uint32_t saved_mask;
    volume_estimator_port_t *port;
    UNUSED(msg);

    /* check state machine */
    if ((gaming_mode_vol_balance_handle.port == NULL) || (gaming_mode_vol_balance_handle.data_buf == NULL) || (gaming_mode_vol_balance_handle.nvkey_buf == NULL) || (gaming_mode_vol_balance_handle.data_size != 480))
    {
        configASSERT(0);
    }
    port = gaming_mode_vol_balance_handle.port;

    /* deinit chat volume estimator port */
    volume_estimator_deinit(port);

    /* free memory */
    preloader_pisplit_free_memory(gaming_mode_vol_balance_handle.data_buf);
    preloader_pisplit_free_memory(gaming_mode_vol_balance_handle.nvkey_buf);

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if ((gaming_mode_vol_balance_handle.enable) && (gaming_mode_vol_balance_handle.game_path_gain_port != NULL))
    {
        /* process L-channel */
        if (gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1 != gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1)
        {
            /* recover gain to the default settings */
            stream_function_sw_gain_configure_gain_target(gaming_mode_vol_balance_handle.game_path_gain_port, 1, gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1);
            gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 = gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1;
        }

        /* process R-channel */
        if (gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2 != gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2)
        {
            /* recover gain to the default settings */
            stream_function_sw_gain_configure_gain_target(gaming_mode_vol_balance_handle.game_path_gain_port, 2, gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2);
            gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 = gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2;
        }

        /* restore ramp-up and ramp-down setting */
        stream_function_sw_gain_configure_gain_up(gaming_mode_vol_balance_handle.game_path_gain_port,   1,  1, 2);
        stream_function_sw_gain_configure_gain_up(gaming_mode_vol_balance_handle.game_path_gain_port,   2,  1, 2);
        stream_function_sw_gain_configure_gain_down(gaming_mode_vol_balance_handle.game_path_gain_port, 1, -1, 2);
        stream_function_sw_gain_configure_gain_down(gaming_mode_vol_balance_handle.game_path_gain_port, 2, -1, 2);
    }

    /* reset state machine */
    gaming_mode_vol_balance_handle.port = NULL;
    gaming_mode_vol_balance_handle.nvkey_buf = NULL;
    gaming_mode_vol_balance_handle.enable = 0;
    gaming_mode_vol_balance_handle.gain_setting_update_done = 0;
    gaming_mode_vol_balance_handle.current_db = -144*100;
    gaming_mode_vol_balance_handle.effective_threshold_db = 0;
    gaming_mode_vol_balance_handle.effective_delay_ms = 0;
    gaming_mode_vol_balance_handle.effective_delay_us_count = 0;
    gaming_mode_vol_balance_handle.failure_threshold_db = 0;
    gaming_mode_vol_balance_handle.failure_delay_ms = 0;
    gaming_mode_vol_balance_handle.failure_delay_us_count = 0;
    gaming_mode_vol_balance_handle.adjustment_amount_db = 0;
    gaming_mode_vol_balance_handle.up_step_db = 0;
    gaming_mode_vol_balance_handle.down_step_db = 0;
    gaming_mode_vol_balance_handle.active_flag = false;
    gaming_mode_vol_balance_handle.data_size = 0;
    gaming_mode_vol_balance_handle.data_buf = NULL;
    hal_nvic_restore_interrupt_mask(saved_mask);

    DSP_MW_LOG_I("[Game/Chat balance][disable]:%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 16,
                gaming_mode_vol_balance_handle.enable,
                gaming_mode_vol_balance_handle.active_flag,
                gaming_mode_vol_balance_handle.current_db,
                gaming_mode_vol_balance_handle.effective_threshold_db,
                gaming_mode_vol_balance_handle.effective_delay_ms,
                gaming_mode_vol_balance_handle.failure_threshold_db,
                gaming_mode_vol_balance_handle.failure_delay_ms,
                gaming_mode_vol_balance_handle.adjustment_amount_db,
                gaming_mode_vol_balance_handle.up_step_db,
                gaming_mode_vol_balance_handle.down_step_db,
                gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_1,
                gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_2,
                gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1,
                gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2,
                gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1,
                gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2);
}

void gaming_mode_dongle_game_chat_volume_smart_balance_do_process(void *msg)
{
    uint32_t saved_mask;
    uint32_t samples;
    uint32_t remain_samples;
    volume_estimator_port_t *port;
    int32_t new_gain_1;
    int32_t new_gain_2;
    bool gain_change = false;
    gaming_mode_vol_balance_buffer_msg_t buf_msg;

    /* check state machine */
    if ((gaming_mode_vol_balance_handle.port == NULL) || (gaming_mode_vol_balance_handle.data_buf == NULL) || (gaming_mode_vol_balance_handle.nvkey_buf == NULL) || (gaming_mode_vol_balance_handle.data_size != 480))
    {
        configASSERT(0);
    }
    port = gaming_mode_vol_balance_handle.port;

    /* check if there are new data on the chat path */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    buf_msg.data_buf_1_head = ((gaming_mode_vol_balance_buffer_msg_t *)msg)->data_buf_1_head;
    buf_msg.data_buf_1_tail = ((gaming_mode_vol_balance_buffer_msg_t *)msg)->data_buf_1_tail;
    buf_msg.data_buf_1_cur  = ((gaming_mode_vol_balance_buffer_msg_t *)msg)->data_buf_1_cur;
    buf_msg.data_buf_2_head = ((gaming_mode_vol_balance_buffer_msg_t *)msg)->data_buf_2_head;
    buf_msg.data_buf_2_tail = ((gaming_mode_vol_balance_buffer_msg_t *)msg)->data_buf_2_tail;
    buf_msg.data_buf_2_cur  = ((gaming_mode_vol_balance_buffer_msg_t *)msg)->data_buf_2_cur;
    hal_nvic_restore_interrupt_mask(saved_mask);
    if ((buf_msg.data_buf_1_cur == NULL) && (buf_msg.data_buf_2_cur == NULL))
    {
        /* there are not new data on the chat path */
        /* we will set zeros into internal buffer, and the gaming_mode_vol_balance_handle.current_db will be very low in 200ms later. Then we recover the volume on the game path */
        memset(gaming_mode_vol_balance_handle.data_buf, 0, gaming_mode_vol_balance_handle.data_size);
    }
    else
    {
        /* there are new data on the chat path */
        /* copy data into the internal buffer */
        /* buf_msg.data_buf_1_cur & buf_msg.data_buf_2_cur is pointer the frame end address */
        samples = 480/2/sizeof(int16_t);
        if (((buf_msg.data_buf_1_cur-samples) < buf_msg.data_buf_1_head) || ((buf_msg.data_buf_2_cur-samples) < buf_msg.data_buf_2_head))
        {
            /* wrapper case */
            remain_samples = buf_msg.data_buf_1_cur - buf_msg.data_buf_1_head;
            if (remain_samples > samples)
            {
                configASSERT(0);
            }
            DSP_D2I_BufferCopy_16bit((U16 *)(gaming_mode_vol_balance_handle.data_buf),
                                    (buf_msg.data_buf_1_tail-(samples-remain_samples)),
                                    (buf_msg.data_buf_2_tail-(samples-remain_samples)),
                                    samples-remain_samples);
            DSP_D2I_BufferCopy_16bit((U16 *)(gaming_mode_vol_balance_handle.data_buf),
                                    (buf_msg.data_buf_1_head),
                                    (buf_msg.data_buf_2_head),
                                    remain_samples);
        }
        else
        {
            /* normal case */
            DSP_D2I_BufferCopy_16bit((U16 *)(gaming_mode_vol_balance_handle.data_buf),
                                    (buf_msg.data_buf_1_cur-samples),
                                    (buf_msg.data_buf_2_cur-samples),
                                    samples);
        }
    }

    /* update ramp-up/ramp-down settings at the first time */
    if ((gaming_mode_vol_balance_handle.enable) && (gaming_mode_vol_balance_handle.game_path_gain_port != NULL) && (gaming_mode_vol_balance_handle.gain_setting_update_done == 0))
    {
        /* do ramp-up and ramp-down setting */
        stream_function_sw_gain_configure_gain_up(gaming_mode_vol_balance_handle.game_path_gain_port,   1, gaming_mode_vol_balance_handle.up_step_db,   2);
        stream_function_sw_gain_configure_gain_up(gaming_mode_vol_balance_handle.game_path_gain_port,   2, gaming_mode_vol_balance_handle.up_step_db,   2);
        stream_function_sw_gain_configure_gain_down(gaming_mode_vol_balance_handle.game_path_gain_port, 1, gaming_mode_vol_balance_handle.down_step_db, 2);
        stream_function_sw_gain_configure_gain_down(gaming_mode_vol_balance_handle.game_path_gain_port, 2, gaming_mode_vol_balance_handle.down_step_db, 2);

        /* set flag */
        gaming_mode_vol_balance_handle.gain_setting_update_done = 1;
    }

    /* get the latest volume on chat path and the current_db will be update every 200ms */
    if (volume_estimator_process(port, gaming_mode_vol_balance_handle.data_buf, 480, &gaming_mode_vol_balance_handle.current_db) != VOLUME_ESTIMATOR_STATUS_OK)
    {
        configASSERT(0);
    }
    // DSP_MW_LOG_I("[Game/Chat balance][get chat gain]:%d\r\n", 1, gaming_mode_vol_balance_handle.current_db);

    /* check if needs to do the balance */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if ((gaming_mode_vol_balance_handle.enable) && (gaming_mode_vol_balance_handle.game_path_gain_port != NULL))
    {
        if ((gaming_mode_vol_balance_handle.current_db >= gaming_mode_vol_balance_handle.effective_threshold_db) &&
            (gaming_mode_vol_balance_handle.effective_delay_us_count >= gaming_mode_vol_balance_handle.effective_delay_ms*1000))
        {
            /* there is chat voice on the chat path */
            /* process L-channel */
            new_gain_1 = (gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_1 + gaming_mode_vol_balance_handle.adjustment_amount_db);
            new_gain_1 = min(new_gain_1, gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1);
            if (gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 != new_gain_1)
            {
                /* the default gain is larger than the target and the current gain is not equal to the target, so we need to ramp down the volume on the game path */
                stream_function_sw_gain_configure_gain_target(gaming_mode_vol_balance_handle.game_path_gain_port, 1, new_gain_1);
                gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 = new_gain_1;
                gain_change = true;
            }

            /* process R-channel */
            new_gain_2 = (gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_2 + gaming_mode_vol_balance_handle.adjustment_amount_db);
            new_gain_2 = min(new_gain_2, gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2);
            if (gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 != new_gain_2)
            {
                /* the default gain is larger than the target and the current gain is not equal to the target, so we need to ramp down the volume on the game path */
                stream_function_sw_gain_configure_gain_target(gaming_mode_vol_balance_handle.game_path_gain_port, 2, new_gain_2);
                gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 = new_gain_2;
                gain_change = true;
            }

            /* set active flag */
            gaming_mode_vol_balance_handle.active_flag = true;

            /* reset failure_delay_us_count */
            gaming_mode_vol_balance_handle.failure_delay_us_count = 0;
        }
        else if ((gaming_mode_vol_balance_handle.current_db >= gaming_mode_vol_balance_handle.effective_threshold_db) &&
                (gaming_mode_vol_balance_handle.effective_delay_us_count < gaming_mode_vol_balance_handle.effective_delay_ms*1000))
        {
            gaming_mode_vol_balance_handle.effective_delay_us_count += 2500;

            /* reset failure_delay_us_count */
            gaming_mode_vol_balance_handle.failure_delay_us_count = 0;
        }
        else if ((gaming_mode_vol_balance_handle.current_db <= gaming_mode_vol_balance_handle.failure_threshold_db) &&
                (gaming_mode_vol_balance_handle.failure_delay_us_count >= gaming_mode_vol_balance_handle.failure_delay_ms*1000))
        {
            /* there is no chat voice on the chat path */
            /* process L-channel */
            if (gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1 != gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1)
            {
                /* recover gain to the default settings */
                stream_function_sw_gain_configure_gain_target(gaming_mode_vol_balance_handle.game_path_gain_port, 1, gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1);
                gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 = gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1;
                gain_change = true;
            }

            /* process R-channel */
            if (gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2 != gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2)
            {
                /* recover gain to the default settings */
                stream_function_sw_gain_configure_gain_target(gaming_mode_vol_balance_handle.game_path_gain_port, 2, gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2);
                gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 = gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2;
                gain_change = true;
            }

            /* set active flag */
            gaming_mode_vol_balance_handle.active_flag = false;

            /* reset effective_delay_us_count */
            gaming_mode_vol_balance_handle.effective_delay_us_count = 0;
        }
        else if ((gaming_mode_vol_balance_handle.current_db <= gaming_mode_vol_balance_handle.failure_threshold_db) &&
                (gaming_mode_vol_balance_handle.failure_delay_us_count < gaming_mode_vol_balance_handle.failure_delay_ms*1000))
        {
            gaming_mode_vol_balance_handle.failure_delay_us_count += 2500;

            /* reset effective_delay_us_count */
            gaming_mode_vol_balance_handle.effective_delay_us_count = 0;

            /* process L-channel */
            if ((gaming_mode_vol_balance_handle.active_flag) && (gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_1 < gaming_mode_vol_balance_handle.effective_threshold_db))
            {
                /* In this stage, we should recover gain to the default settings because the chat gain is too less */
                new_gain_1 = gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1;
                if (gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 != new_gain_1)
                {
                    stream_function_sw_gain_configure_gain_target(gaming_mode_vol_balance_handle.game_path_gain_port, 1, new_gain_1);
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 = new_gain_1;
                    gain_change = true;
                }
            }
            else if (gaming_mode_vol_balance_handle.active_flag)
            {
                /* In this stage, we should set gain to the minimum in default gain and smart-target gain */
                new_gain_1 = (gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_1 + gaming_mode_vol_balance_handle.adjustment_amount_db);
                new_gain_1 = min(new_gain_1, gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1);
                if (gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 != new_gain_1)
                {
                    /* set gain to the minimun settings */
                    stream_function_sw_gain_configure_gain_target(gaming_mode_vol_balance_handle.game_path_gain_port, 1, new_gain_1);
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 = new_gain_1;
                    gain_change = true;
                }
            }
            else
            {
                if (gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1 != gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1)
                {
                    /* recover gain to the default settings */
                    stream_function_sw_gain_configure_gain_target(gaming_mode_vol_balance_handle.game_path_gain_port, 1, gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1);
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 = gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1;
                    gain_change = true;
                }
            }

            /* process R-channel */
            if ((gaming_mode_vol_balance_handle.active_flag) && (gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_2 < gaming_mode_vol_balance_handle.effective_threshold_db))
            {
                /* In this stage, we should recover gain to the default settings because the chat gain is too less */
                new_gain_2 = gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2;
                if (gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 != new_gain_2)
                {
                    stream_function_sw_gain_configure_gain_target(gaming_mode_vol_balance_handle.game_path_gain_port, 2, new_gain_2);
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 = new_gain_2;
                    gain_change = true;
                }
            }
            else if (gaming_mode_vol_balance_handle.active_flag)
            {
                new_gain_2 = (gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_2 + gaming_mode_vol_balance_handle.adjustment_amount_db);
                new_gain_2 = min(new_gain_2, gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2);
                if (gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 != new_gain_2)
                {
                    /* recover gain to the minimun settings */
                    stream_function_sw_gain_configure_gain_target(gaming_mode_vol_balance_handle.game_path_gain_port, 2, new_gain_2);
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 = new_gain_2;
                    gain_change = true;
                }
            }
            else
            {
                if (gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2 != gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2)
                {
                    /* recover gain to the default settings */
                    stream_function_sw_gain_configure_gain_target(gaming_mode_vol_balance_handle.game_path_gain_port, 2, gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2);
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 = gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2;
                    gain_change = true;
                }
            }
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    if (gain_change)
    {
        DSP_MW_LOG_I("[Game/Chat balance][change gain]:%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 13,
                    gaming_mode_vol_balance_handle.active_flag,
                    gaming_mode_vol_balance_handle.current_db,
                    gaming_mode_vol_balance_handle.effective_threshold_db,
                    gaming_mode_vol_balance_handle.effective_delay_us_count,
                    gaming_mode_vol_balance_handle.failure_threshold_db,
                    gaming_mode_vol_balance_handle.failure_delay_us_count,
                    gaming_mode_vol_balance_handle.adjustment_amount_db,
                    gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_1,
                    gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_2,
                    gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1,
                    gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2,
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1,
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2);
    }
}
#endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

/******************************************************************************/
/*                  Gaming mode dongle music path functions                   */
/******************************************************************************/
#ifdef AIR_SOFTWARE_MIXER_ENABLE
ATTR_TEXT_IN_IRAM_LEVEL_1 void gaming_mode_dongle_mixer_postcallback(sw_mixer_member_t *member, void *para, uint32_t *out_frame_size)
{
    int32_t i;
    gaming_mode_dongle_dl_handle_t *c_handle = NULL;
    SOURCE source = NULL;
    bool all_streams_in_mixer = true;

    UNUSED(member);
    UNUSED(para);

    /* change this handle data status */
    source = (SOURCE)(member->owner);
    c_handle = (gaming_mode_dongle_dl_handle_t *)(source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.handle);
    c_handle->data_status = GAMING_MODE_STREAM_DATA_IN_MIXER;
    if (c_handle->mixer_status == GAMING_MODE_STREAM_MIXER_UNMIX)
    {
        /* clear this stream's input buffer */
        stream_function_sw_mixer_member_input_buffers_clean(c_handle->mixer_member, false);

        /* change this handle data status */
        c_handle->data_status = GAMING_MODE_STREAM_DATA_EMPTY;

        /* this stream is unmixed now, so we can directly return here */
        return;
    }
    else if (source->param.data_dl.current_notification_index == 0)
    {
        /* clear this stream's input buffer */
        stream_function_sw_mixer_member_input_buffers_clean(c_handle->mixer_member, false);

        /* change this handle data status */
        c_handle->data_status = GAMING_MODE_STREAM_DATA_EMPTY;

        /* in here, it means it is the first packet, we need to drop the later sink flow */
        *out_frame_size = 0;

        return;
    }

    /* check if all mixed stream data is in this mixer */
    c_handle = gaming_mode_dongle_first_dl_handle;
    for (i = 0; i < gaming_mode_dongle_first_dl_handle->total_number; i++)
    {
        source = (SOURCE)(c_handle->owner);
        if ((c_handle->data_status == GAMING_MODE_STREAM_DATA_IN_STREAM) &&
            (c_handle->mixer_status != GAMING_MODE_STREAM_MIXER_UNMIX) &&
            (source->param.data_dl.current_notification_index != 0))
        {
            /* this stream should be mixed, but its data is not in this mixer now.
               so we need to bypass this mix result in this time */
            all_streams_in_mixer = false;
            break;
        }

        /* switch to the next dl stream */
        c_handle = c_handle->next_dl_handle;
    }

    /* check if the output data in this time includes all stream data */
    if ((*out_frame_size != 0) && (all_streams_in_mixer == true))
    {
        /* all_streams_in_mixer is true, so all stream data is in this mixer.
           So we can clean all mixed streams' input buffers now and the mix result are sent to the sink */
        c_handle = gaming_mode_dongle_first_dl_handle;
        for (i = 0; i < gaming_mode_dongle_first_dl_handle->total_number; i++)
        {
            if (c_handle->mixer_status != GAMING_MODE_STREAM_MIXER_UNMIX)
            {
                /* clear this stream's input buffer */
                stream_function_sw_mixer_member_input_buffers_clean(c_handle->mixer_member, false);

                /* change this handle data status */
                c_handle->data_status = GAMING_MODE_STREAM_DATA_EMPTY;
            }

            /* switch to the next dl stream */
            c_handle = c_handle->next_dl_handle;
        }
    }
    else
    {
        /* all_streams_in_mixer is false, so some stream data is not in this mixer.
           So we need to bypass this mix result */
        *out_frame_size = 0;
    }
}
#endif /* AIR_SOFTWARE_MIXER_ENABLE */

gaming_mode_dongle_dl_handle_t *gaming_mode_dongle_get_dl_handle(void *owner)
{
    uint32_t saved_mask;
    int16_t count;
    gaming_mode_dongle_dl_handle_t *dongle_handle = NULL;
    gaming_mode_dongle_dl_handle_t *c_handle = NULL;

    dongle_handle = malloc(sizeof(gaming_mode_dongle_dl_handle_t));
    if (dongle_handle == NULL)
    {
        configASSERT(0);
    }
    memset(dongle_handle, 0, sizeof(gaming_mode_dongle_dl_handle_t));

    dongle_handle->total_number = -1;
    dongle_handle->index = -1;
    dongle_handle->owner = owner;
    dongle_handle->next_dl_handle = NULL;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if (gaming_mode_dongle_first_dl_handle == NULL)
    {
        /* there is no item on the handle list */
        dongle_handle->total_number = 1;
        dongle_handle->index = 1;

        /* set this handle as the head of the handle list */
        gaming_mode_dongle_first_dl_handle = dongle_handle;
    }
    else
    {
        /* there are other items on the handle list */
        count = 1;
        c_handle = gaming_mode_dongle_first_dl_handle;
        while (c_handle != NULL)
        {
            count += 1;

            c_handle->total_number += 1;
            if (c_handle->total_number <= 0)
            {
                /* error status */
                configASSERT(0);
            }

            if (c_handle->next_dl_handle == NULL)
            {
                /* c_handle is the last item on the list now */
                dongle_handle->total_number = c_handle->total_number;
                dongle_handle->index = c_handle->total_number;

                /* dongle_handle is the last item on the list now */
                c_handle->next_dl_handle = dongle_handle;

                break;
            }

            c_handle = c_handle->next_dl_handle;
        }
        if ((c_handle == NULL) || (dongle_handle->total_number != count))
        {
            configASSERT(0);
        }
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    return dongle_handle;
}

void gaming_mode_dongle_release_dl_handle(gaming_mode_dongle_dl_handle_t *handle)
{
    uint32_t saved_mask, i, count;
    gaming_mode_dongle_dl_handle_t *l_handle = NULL;
    gaming_mode_dongle_dl_handle_t *c_handle = NULL;
    gaming_mode_dongle_dl_handle_t *dongle_handle = NULL;

    if ((handle == NULL) || (gaming_mode_dongle_first_dl_handle == NULL))
    {
        configASSERT(0);
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if (gaming_mode_dongle_first_dl_handle->total_number <= 0)
    {
        /* error status */
        configASSERT(0);
    }
    else if ((gaming_mode_dongle_first_dl_handle->total_number == 1) &&
            (gaming_mode_dongle_first_dl_handle == handle))
    {
        /* this handle is only one item on handle list */
        if (gaming_mode_dongle_first_dl_handle->next_dl_handle != NULL)
        {
            configASSERT(0);
        }

        /* there is no item on the handle list */
        gaming_mode_dongle_first_dl_handle = NULL;
    }
    else if ((gaming_mode_dongle_first_dl_handle->total_number > 1) &&
            (gaming_mode_dongle_first_dl_handle == handle))
    {
        /* this handle is the first item on handle list, but there are other handles on the list */
        c_handle = gaming_mode_dongle_first_dl_handle;
        count = gaming_mode_dongle_first_dl_handle->total_number;
        for (i=0; i < count; i++)
        {
            c_handle->total_number -= 1;
            c_handle->index -= 1;

            c_handle = c_handle->next_dl_handle;
        }
        if (c_handle != NULL)
        {
            /* error status */
            configASSERT(0);
        }

        /* change the next iten to the head */
        gaming_mode_dongle_first_dl_handle = handle->next_dl_handle;
    }
    else
    {
        /* this handle is the middle item on handle list */
        c_handle = gaming_mode_dongle_first_dl_handle;
        count = gaming_mode_dongle_first_dl_handle->total_number;
        if (count == 1)
        {
            /* error status */
            configASSERT(0);
        }
        for (i=0; i < count; i++)
        {
            if (c_handle == handle)
            {
                /* find out the handle on the list */
                dongle_handle = handle;
                l_handle->next_dl_handle = c_handle->next_dl_handle;
            }

            if (dongle_handle == NULL)
            {
                c_handle->total_number -= 1;
            }
            else
            {
                c_handle->total_number -= 1;
                c_handle->index -= 1;
            }

            l_handle = c_handle;
            c_handle = c_handle->next_dl_handle;
        }
        if ((c_handle != NULL) || (dongle_handle == NULL))
        {
            /* error status */
            configASSERT(0);
        }
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    free(handle);
}

void gaming_mode_dongle_dl_init(SOURCE source, audio_transmitter_open_param_p open_param)
{
    gaming_mode_dongle_dl_handle_t *dongle_handle;

    /* register ccni handler */
    audio_transmitter_register_isr_handler(gaming_mode_dongle_dl_ccni_handler);

    /* get handle for application config */
    dongle_handle = gaming_mode_dongle_get_dl_handle(source);
    source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.handle = (void *)dongle_handle;

    #ifdef AIR_SOFTWARE_GAIN_ENABLE
    /* sw gain config */
    int32_t default_gain;
    sw_gain_config_t default_config;
    default_config.resolution = RESOLUTION_16BIT;
    default_config.target_gain = open_param->scenario_param.gaming_mode_param.gain_default_L;
    default_config.up_step = 1;
    default_config.up_samples_per_step = 2;
    default_config.down_step = -1;
    default_config.down_samples_per_step = 2;
    dongle_handle->gain_port = stream_function_sw_gain_get_port(source);
    stream_function_sw_gain_init(dongle_handle->gain_port, 2, &default_config);
    default_gain = open_param->scenario_param.gaming_mode_param.gain_default_L;
    stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 1, default_gain);
    DSP_MW_LOG_I("[audio transmitter][config]scenario %d-%d change channel %d gain to %d*0.01dB\r\n", 4,
                open_param->scenario_type,
                open_param->scenario_sub_id,
                1,
                default_gain);
    default_gain = open_param->scenario_param.gaming_mode_param.gain_default_R;
    stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 2, default_gain);
    DSP_MW_LOG_I("[audio transmitter][config]scenario %d-%d change channel %d gain to %d*0.01dB\r\n", 4,
                open_param->scenario_type,
                open_param->scenario_sub_id,
                2,
                default_gain);
    #endif /* AIR_SOFTWARE_GAIN_ENABLE */

    #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
    if (open_param->scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0)
    {
        gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_1 = open_param->scenario_param.gaming_mode_param.gain_default_L;
        gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_2 = open_param->scenario_param.gaming_mode_param.gain_default_R;
        DTM_enqueue(DTM_EVENT_ID_GAMING_MODE_VOLUME_SMART_BALANCE_ENABLE, (uint32_t)source, false);
    }
    else
    {
        gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1 = open_param->scenario_param.gaming_mode_param.gain_default_L;
        gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2 = open_param->scenario_param.gaming_mode_param.gain_default_R;
        gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 = open_param->scenario_param.gaming_mode_param.gain_default_L;
        gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 = open_param->scenario_param.gaming_mode_param.gain_default_R;
        gaming_mode_vol_balance_handle.game_path_gain_port = dongle_handle->gain_port;
    }
    #endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

    #ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
    sw_clk_skew_config_t sw_clk_skew_config;
    /* clock skew config */
    source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.clk_skew.last_timestamp = 0;
    source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.clk_skew.act_time = 0;
    source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.clk_skew.act_count = 0;
    source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.clk_skew.act_diff = 0;
    source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.clk_skew.ave_diff = 0;
    source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.clk_skew.compen_samples = 0;
    dongle_handle->clk_skew_port = stream_function_sw_clk_skew_get_port(source);
    sw_clk_skew_config.channel = 2;
    sw_clk_skew_config.bits = 16;
    sw_clk_skew_config.order = C_Flp_Ord_5;
    sw_clk_skew_config.skew_io_mode = C_Skew_Inp;
    sw_clk_skew_config.skew_compensation_mode = SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_1_FRAME;
    sw_clk_skew_config.skew_work_mode = SW_CLK_SKEW_CONTINUOUS;
    sw_clk_skew_config.max_output_size = 480;
    sw_clk_skew_config.continuous_frame_size = 120*sizeof(int16_t);
    stream_function_sw_clk_skew_init(dongle_handle->clk_skew_port, &sw_clk_skew_config);
    #endif /* AIR_SOFTWARE_CLK_SKEW_ENABLE */

    #ifdef AIR_SOFTWARE_BUFFER_ENABLE
    sw_buffer_config_t buffer_config;
    buffer_config.mode = SW_BUFFER_MODE_FIXED_OUTPUT_LENGTH;
    buffer_config.total_channels = 2;
    buffer_config.watermark_max_size = 10*48*sizeof(int16_t);  /* must be 2.5ms*N */
    buffer_config.watermark_min_size = 0;
    buffer_config.output_size = 120*sizeof(int16_t);
    dongle_handle->buffer_port = stream_function_sw_buffer_get_port(source);
    stream_function_sw_buffer_init(dongle_handle->buffer_port, &buffer_config);
    #endif /* AIR_SOFTWARE_BUFFER_ENABLE */

    #ifdef AIR_SOFTWARE_MIXER_ENABLE
    /* sw mixer config */
    sw_mixer_callback_config_t       callback_config;
    sw_mixer_input_channel_config_t  in_ch_config;
    sw_mixer_output_channel_config_t out_ch_config;

    stream_function_sw_mixer_init(SW_MIXER_PORT_0);

    callback_config.preprocess_callback = NULL;
    extern void gaming_mode_dongle_mixer_postcallback(sw_mixer_member_t *member, void *para, uint32_t *out_frame_size);
    callback_config.postprocess_callback = gaming_mode_dongle_mixer_postcallback;
    in_ch_config.total_channel_number = 2;
    in_ch_config.resolution = RESOLUTION_16BIT;
    in_ch_config.input_mode = SW_MIXER_CHANNEL_MODE_FORCED_ENOUGH;
    in_ch_config.buffer_size = sizeof(int16_t) * 120; // 2.5ms * 48K
    out_ch_config.total_channel_number = 2;
    out_ch_config.resolution = RESOLUTION_16BIT;
    dongle_handle->mixer_member= stream_function_sw_mixer_member_create((void *)source,
                                                                        SW_MIXER_MEMBER_MODE_NO_BYPASS,
                                                                        &callback_config,
                                                                        &in_ch_config,
                                                                        &out_ch_config);

    if (dongle_handle->index == 1)
    {
        /* it is the first dl stream */
        stream_function_sw_mixer_member_register(SW_MIXER_PORT_0, dongle_handle->mixer_member, true);
    }
    else
    {
        /* it is not the first dl stream, so we need to disconnect default connection */
        stream_function_sw_mixer_member_register(SW_MIXER_PORT_0, dongle_handle->mixer_member, false);
    }
    #endif /* AIR_SOFTWARE_MIXER_ENABLE */

    /* lock sleep because sleep wake-up time will consume the stream process time */
    hal_sleep_manager_lock_sleep(SLEEP_LOCK_AUDIO);
}

void gaming_mode_dongle_dl_deinit(SOURCE source, SINK sink)
{
    uint32_t saved_mask;
    gaming_mode_dongle_dl_handle_t *dongle_handle = (gaming_mode_dongle_dl_handle_t *)(source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.handle);
    UNUSED(sink);

    /* application deinit */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    /* unregister ccni handler */
    if (dongle_handle->total_number == 1)
    {
        audio_transmitter_unregister_isr_handler(gaming_mode_dongle_dl_ccni_handler);
    }

    /* restore cap id to the default value */
    if ((dongle_handle->index == 1) && (dongle_handle->cap_id_default != 0))
    {
        DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE_SEL = 0x1;
        DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE     = dongle_handle->cap_id_default;
    }

    #ifdef AIR_SOFTWARE_GAIN_ENABLE
    stream_function_sw_gain_deinit(dongle_handle->gain_port);
    #endif /* AIR_SOFTWARE_GAIN_ENABLE */

    #ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
    stream_function_sw_clk_skew_deinit(dongle_handle->clk_skew_port);
    #endif /* AIR_SOFTWARE_CLK_SKEW_ENABLE */

    #ifdef AIR_SOFTWARE_BUFFER_ENABLE
    stream_function_sw_buffer_deinit(dongle_handle->buffer_port);
    #endif /* AIR_SOFTWARE_BUFFER_ENABLE */

    #ifdef AIR_SOFTWARE_MIXER_ENABLE
    stream_function_sw_mixer_member_unregister(SW_MIXER_PORT_0, dongle_handle->mixer_member);
    stream_function_sw_mixer_member_delete(dongle_handle->mixer_member);
    stream_function_sw_mixer_deinit(SW_MIXER_PORT_0);
    #endif /* AIR_SOFTWARE_MIXER_ENABLE */

    #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
    if (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1)
    {
        gaming_mode_vol_balance_handle.game_path_gain_port = NULL;
    }
    #endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

    /* release handle */
    gaming_mode_dongle_release_dl_handle(dongle_handle);
    source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.handle = NULL;

    hal_nvic_restore_interrupt_mask(saved_mask);

    #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
    if (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0)
    {
        DTM_enqueue(DTM_EVENT_ID_GAMING_MODE_VOLUME_SMART_BALANCE_DISABLE, 0, false);
    }
    #endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

    /* unlock sleep */
    hal_sleep_manager_unlock_sleep(SLEEP_LOCK_AUDIO);
}

ATTR_TEXT_IN_IRAM_LEVEL_1 void gaming_mode_dongle_dl_ccni_handler(hal_ccni_event_t event, void * msg)
{
    SOURCE source = NULL;
    uint32_t saved_mask = 0;
    uint32_t i, count;
    hal_ccni_message_t *ccni_msg = msg;
    gaming_mode_dongle_dl_handle_t *c_handle = NULL;
    UNUSED(event);

    if (gaming_mode_dongle_first_dl_handle == NULL)
    {
        DSP_MW_LOG_E("[audio transmitter][source]dl handle is NULL\r\n", 0);
        goto _ccni_return;
    }

    /* timestamp for debug */
    gaming_mode_music_ccni_bt_clk = ccni_msg->ccni_message[0];
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&gaming_mode_music_ccni_gpt_count);

    c_handle = gaming_mode_dongle_first_dl_handle;
    count = gaming_mode_dongle_first_dl_handle->total_number;
    for (i=0; i < count; i++)
    {
        /* get source */
        source = (SOURCE)c_handle->owner;
        if ((source == NULL) || (source->transform == NULL))
        {
            DSP_MW_LOG_E("[audio transmitter][source]source or transform is NULL\r\n", 0);
            /* switch to the next dl stream */
            c_handle = c_handle->next_dl_handle;
            continue;
        }

        /* wakeup stream */
        if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_A2DP_SOURCE) {

        }
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE) {
            /* Dongle side, music path, usb in case */
            if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
                (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
                uint32_t current_timestamp = 0;
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
                DSP_MW_LOG_I("[audio transmitter][source][ccni callback entry]: %u, %d", 2, current_timestamp, hal_nvic_query_exception_number());
                #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */

                hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                /* set fetch flag to indicate that the next flow can fetch packets form buffer */
                if(source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.fetch_flag == 0)
                {
                    source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.fetch_flag = 1;
                }
                /* send bt ccni clock stemp to bt common */
                if (source->transform->sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_status == 0)
                {
                    source->transform->sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_timestamp = ccni_msg->ccni_message[0];
                }
                hal_nvic_restore_interrupt_mask(saved_mask);

                /* Handler the stream */
                AudioCheckTransformHandle(source->transform);

                #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LANTENCY
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&gaming_mode_music_ccni_gpt_count2[i]);
                #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LANTENCY */

                #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
                DSP_MW_LOG_I("[audio transmitter][source][ccni callback exit]: %u, %d", 2, current_timestamp, hal_nvic_query_exception_number());
                #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */
            }
        }
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */
        else
        {
            configASSERT(0);
        }

        /* switch to the next dl stream */
        c_handle = c_handle->next_dl_handle;
    }
    if (c_handle != NULL)
    {
        configASSERT(0);
    }

_ccni_return:
    #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LANTENCY
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&gaming_mode_music_ccni_gpt_count1);
    #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LANTENCY */

    return;
}

bool gaming_mode_dongle_dl_config(SOURCE source, stream_config_type type, U32 value)
{
    uint32_t saved_mask = 0;
    extern CONNECTION_IF * port_audio_transmitter_get_connection_if(audio_transmitter_scenario_t scenario_id, audio_transmitter_scenario_sub_id_t sub_id);
    gaming_mode_runtime_config_param_p config_param = (gaming_mode_runtime_config_param_p)value;
    gaming_mode_dongle_dl_handle_t *dongle_handle = (gaming_mode_dongle_dl_handle_t *)(source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.handle);
    #ifdef AIR_SOFTWARE_GAIN_ENABLE
    int32_t new_gain;
    int32_t new_gain2;
    sw_gain_config_t old_config;
    #endif /* AIR_SOFTWARE_GAIN_ENABLE */
    #ifdef AIR_SOFTWARE_MIXER_ENABLE
    audio_transmitter_scenario_t scenario_id_0;
    audio_transmitter_scenario_sub_id_t sub_id_0;
    audio_transmitter_scenario_t scenario_id_1;
    audio_transmitter_scenario_sub_id_t sub_id_1;
    CONNECTION_IF *application_ptr_0 = NULL;
    CONNECTION_IF *application_ptr_1 = NULL;
    gaming_mode_dongle_dl_handle_t *dongle_handle_0;
    gaming_mode_dongle_dl_handle_t *dongle_handle_1;
    #endif /* AIR_SOFTWARE_MIXER_ENABLE */
    UNUSED(dongle_handle);
    UNUSED(saved_mask);
    UNUSED(type);
    UNUSED(value);

    switch (config_param->config_operation)
    {
        case GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_L:
            #ifdef AIR_SOFTWARE_GAIN_ENABLE
            /* left channel */
            new_gain = config_param->config_param.vol_level.gain_1;
            stream_function_sw_gain_get_config(dongle_handle->gain_port, 1, &old_config);
            DSP_MW_LOG_I("[audio transmitter][config]scenario %d-%d change channel %d gain from %d*0.01dB to %d*0.01dB\r\n", 5,
                        source->param.data_dl.scenario_type,
                        source->param.data_dl.scenario_sub_id,
                        1,
                        old_config.target_gain,
                        new_gain);

            #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            if (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0)
            {
                gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_1 = config_param->config_param.vol_level.gain_1;
            }
            else
            {
                gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1 = config_param->config_param.vol_level.gain_1;
                if ((gaming_mode_vol_balance_handle.enable) && (gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 < config_param->config_param.vol_level.gain_1))
                {
                    new_gain = gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1;
                }
                else if ((gaming_mode_vol_balance_handle.enable) && (gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 >= config_param->config_param.vol_level.gain_1))
                {
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 = config_param->config_param.vol_level.gain_1;
                }
                else
                {
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 = config_param->config_param.vol_level.gain_1;
                }
            }
            hal_nvic_restore_interrupt_mask(saved_mask);
            #endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

            stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 1, new_gain);
            #endif /* AIR_SOFTWARE_GAIN_ENABLE */
            break;

        case GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_R:
            #ifdef AIR_SOFTWARE_GAIN_ENABLE
            new_gain = config_param->config_param.vol_level.gain_2;
            /* right channel */
            stream_function_sw_gain_get_config(dongle_handle->gain_port, 2, &old_config);
            DSP_MW_LOG_I("[audio transmitter][config]scenario %d-%d change channel %d gain from %d*0.01dB to %d*0.01dB\r\n", 5,
                        source->param.data_dl.scenario_type,
                        source->param.data_dl.scenario_sub_id,
                        2,
                        old_config.target_gain,
                        new_gain);

            #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            if (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0)
            {
                gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_2 = config_param->config_param.vol_level.gain_2;
            }
            else
            {
                gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2 = config_param->config_param.vol_level.gain_2;
                if ((gaming_mode_vol_balance_handle.enable) && (gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 < config_param->config_param.vol_level.gain_2))
                {
                    new_gain = gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2;
                }
                else if ((gaming_mode_vol_balance_handle.enable) && (gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 >= config_param->config_param.vol_level.gain_2))
                {
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 = config_param->config_param.vol_level.gain_2;
                }
                else
                {
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 = config_param->config_param.vol_level.gain_2;
                }
            }
            hal_nvic_restore_interrupt_mask(saved_mask);
            #endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

            stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 2, new_gain);
            #endif /* AIR_SOFTWARE_GAIN_ENABLE */
            break;

        case GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_DUL:
            #ifdef AIR_SOFTWARE_GAIN_ENABLE
            /* left channel */
            new_gain = config_param->config_param.vol_level.gain_1;
            stream_function_sw_gain_get_config(dongle_handle->gain_port, 1, &old_config);
            DSP_MW_LOG_I("[audio transmitter][config]scenario %d-%d change channel %d gain from %d*0.01dB to %d*0.01dB\r\n", 5,
                        source->param.data_dl.scenario_type,
                        source->param.data_dl.scenario_sub_id,
                        1,
                        old_config.target_gain,
                        new_gain);

            /* right channel */
            stream_function_sw_gain_get_config(dongle_handle->gain_port, 2, &old_config);
            new_gain2 = config_param->config_param.vol_level.gain_2;
            DSP_MW_LOG_I("[audio transmitter][config]scenario %d-%d change channel %d gain from %d*0.01dB to %d*0.01dB\r\n", 5,
                        source->param.data_dl.scenario_type,
                        source->param.data_dl.scenario_sub_id,
                        2,
                        old_config.target_gain,
                        new_gain2);

            #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            if (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0)
            {
                gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_1 = config_param->config_param.vol_level.gain_1;
                gaming_mode_vol_balance_handle.chat_path_default_vol_level.gain_2 = config_param->config_param.vol_level.gain_2;
            }
            else
            {
                gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_1 = config_param->config_param.vol_level.gain_1;
                if ((gaming_mode_vol_balance_handle.enable) && (gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 < config_param->config_param.vol_level.gain_1))
                {
                    new_gain = gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1;
                }
                else if ((gaming_mode_vol_balance_handle.enable) && (gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 >= config_param->config_param.vol_level.gain_1))
                {
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 = config_param->config_param.vol_level.gain_1;
                }
                else
                {
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_1 = config_param->config_param.vol_level.gain_1;
                }
                gaming_mode_vol_balance_handle.game_path_default_vol_level.gain_2 = config_param->config_param.vol_level.gain_2;
                if ((gaming_mode_vol_balance_handle.enable) && (gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 < config_param->config_param.vol_level.gain_2))
                {
                    new_gain2 = gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2;
                }
                else if ((gaming_mode_vol_balance_handle.enable) && (gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 >= config_param->config_param.vol_level.gain_2))
                {
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 = config_param->config_param.vol_level.gain_2;
                }
                else
                {
                    gaming_mode_vol_balance_handle.game_path_balance_vol_level.gain_2 = config_param->config_param.vol_level.gain_2;
                }
            }
            hal_nvic_restore_interrupt_mask(saved_mask);
            #endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

            stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 1, new_gain);
            stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 2, new_gain2);
            #endif /* AIR_SOFTWARE_GAIN_ENABLE */
            break;

        case GAMING_MODE_CONFIG_OP_MUSIC_MIX:
            #ifdef AIR_SOFTWARE_MIXER_ENABLE
            /* mix stream */
            scenario_id_0 = source->param.data_dl.scenario_type;
            sub_id_0.gamingmode_id = source->param.data_dl.scenario_sub_id;
            scenario_id_1 = config_param->config_param.dl_mixer_param.scenario_id;
            sub_id_1.gamingmode_id = config_param->config_param.dl_mixer_param.sub_id;
            application_ptr_0 = port_audio_transmitter_get_connection_if(scenario_id_0, sub_id_0);
            application_ptr_1 = port_audio_transmitter_get_connection_if(scenario_id_1, sub_id_1);
            if ((application_ptr_0 == NULL) || (application_ptr_1 == NULL))
            {
                configASSERT(0);
            }

            /* get application handle */
            dongle_handle_0 = (gaming_mode_dongle_dl_handle_t *)(application_ptr_0->source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.handle);
            dongle_handle_1 = (gaming_mode_dongle_dl_handle_t *)(application_ptr_1->source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.handle);

            hal_nvic_save_and_set_interrupt_mask(&saved_mask);

            /* do mix connections */
            if (dongle_handle_0 == dongle_handle_1)
            {
                stream_function_sw_mixer_channel_connect(dongle_handle_0->mixer_member, 1, SW_MIXER_CHANNEL_ATTRIBUTE_MAIN, dongle_handle_0->mixer_member, 1);
                stream_function_sw_mixer_channel_connect(dongle_handle_0->mixer_member, 2, SW_MIXER_CHANNEL_ATTRIBUTE_MAIN, dongle_handle_0->mixer_member, 2);
            }
            else
            {
                stream_function_sw_mixer_channel_connect(dongle_handle_0->mixer_member, 1, SW_MIXER_CHANNEL_ATTRIBUTE_MAIN, dongle_handle_0->mixer_member, 1);
                stream_function_sw_mixer_channel_connect(dongle_handle_0->mixer_member, 2, SW_MIXER_CHANNEL_ATTRIBUTE_MAIN, dongle_handle_0->mixer_member, 2);
                stream_function_sw_mixer_channel_connect(dongle_handle_1->mixer_member, 1, SW_MIXER_CHANNEL_ATTRIBUTE_NORMAL, dongle_handle_0->mixer_member, 1);
                stream_function_sw_mixer_channel_connect(dongle_handle_1->mixer_member, 2, SW_MIXER_CHANNEL_ATTRIBUTE_NORMAL, dongle_handle_0->mixer_member, 2);

                stream_function_sw_mixer_channel_connect(dongle_handle_1->mixer_member, 1, SW_MIXER_CHANNEL_ATTRIBUTE_MAIN, dongle_handle_1->mixer_member, 1);
                stream_function_sw_mixer_channel_connect(dongle_handle_1->mixer_member, 2, SW_MIXER_CHANNEL_ATTRIBUTE_MAIN, dongle_handle_1->mixer_member, 2);
                stream_function_sw_mixer_channel_connect(dongle_handle_0->mixer_member, 1, SW_MIXER_CHANNEL_ATTRIBUTE_NORMAL, dongle_handle_1->mixer_member, 1);
                stream_function_sw_mixer_channel_connect(dongle_handle_0->mixer_member, 2, SW_MIXER_CHANNEL_ATTRIBUTE_NORMAL, dongle_handle_1->mixer_member, 2);
            }

            /* update mixer status */
            dongle_handle_0->mixer_status = GAMING_MODE_STREAM_MIXER_MIX;
            dongle_handle_1->mixer_status = GAMING_MODE_STREAM_MIXER_MIX;

            hal_nvic_restore_interrupt_mask(saved_mask);

            DSP_MW_LOG_I("[audio transmitter][config] mix done. %d, %d, %d, %d\r\n", 4, scenario_id_0, sub_id_0.gamingmode_id, scenario_id_1, sub_id_1.gamingmode_id);
            #endif /* AIR_SOFTWARE_MIXER_ENABLE */
            break;

        case GAMING_MODE_CONFIG_OP_MUSIC_UNMIX:
            #ifdef AIR_SOFTWARE_MIXER_ENABLE
            /* unmix stream */
            scenario_id_0 = source->param.data_dl.scenario_type;
            sub_id_0.gamingmode_id = source->param.data_dl.scenario_sub_id;

            application_ptr_0 = port_audio_transmitter_get_connection_if(scenario_id_0, sub_id_0);
            if (application_ptr_0 == NULL)
            {
                configASSERT(0);
            }

            /* get application handle */
            dongle_handle_0 = (gaming_mode_dongle_dl_handle_t *)(application_ptr_0->source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.handle);

            hal_nvic_save_and_set_interrupt_mask(&saved_mask);

            /* update mixer status */
            dongle_handle_0->mixer_status = GAMING_MODE_STREAM_MIXER_UNMIX;

            /* do disconnections */
            stream_function_sw_mixer_channel_disconnect_all(dongle_handle_0->mixer_member);

            hal_nvic_restore_interrupt_mask(saved_mask);

            DSP_MW_LOG_I("[audio transmitter][config] unmix done. %d, %d\r\n", 2, scenario_id_0, sub_id_0.gamingmode_id);
            #endif /* AIR_SOFTWARE_MIXER_ENABLE */
            break;

        default:
            DSP_MW_LOG_E("[audio transmitter][config] unknow cmd:%d\r\n", 1, config_param->config_operation);
            break;
    }

    return true;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 bool gaming_mode_dongle_dl_source_get_avail_size(SOURCE source, uint32_t *avail_size)
{
    uint32_t frame_count = 0;
    gaming_mode_dongle_dl_handle_t *dongle_handle = (gaming_mode_dongle_dl_handle_t *)(source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.handle);
    uint32_t remain_samples = 0;

    if (source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.fetch_flag == 0)
    {
        *avail_size = 0;
    }
    else
    {
        if (source->streamBuffer.ShareBufferInfo.ReadOffset < source->streamBuffer.ShareBufferInfo.WriteOffset)
        {
            /* normal case */
            *avail_size = source->streamBuffer.ShareBufferInfo.WriteOffset - source->streamBuffer.ShareBufferInfo.ReadOffset;
        }
        else if (source->streamBuffer.ShareBufferInfo.ReadOffset == source->streamBuffer.ShareBufferInfo.WriteOffset)
        {
            if(source->streamBuffer.ShareBufferInfo.bBufferIsFull == true)
            {
                /* buffer is full, so ReadOffset == WriteOffset */
                *avail_size = source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_num;
                DSP_MW_LOG_E("[audio transmitter][source]: buffer is full, index = %u", 1, source->streamBuffer.ShareBufferInfo.ReadOffset);
            }
            else
            {
                /* buffer is empty, so ReadOffset == WriteOffset */
                *avail_size = 0;
                dongle_handle->process_frames = 0;

                #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
                uint32_t current_timestamp = 0;
                uint32_t duration = 0;
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
                hal_gpt_get_duration_count( source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.data_timestamp,
                                            current_timestamp,
                                            &duration);
                DSP_MW_LOG_E("[audio transmitter][source]: buffer is empty in a long time, index = %u", 1, source->streamBuffer.ShareBufferInfo.ReadOffset);
                DSP_MW_LOG_E("[audio transmitter][source]: duration = %u, last time = %u, current time = %u", 3,
                            duration,
                            source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.data_timestamp,
                            current_timestamp);
                #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */
            }
        }
        else
        {
            /* buffer wrapper case */
            *avail_size = source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_num
                        - source->streamBuffer.ShareBufferInfo.ReadOffset
                        + source->streamBuffer.ShareBufferInfo.WriteOffset;
        }

        if (*avail_size != 0)
        {
            /* if this is the first time, check if the data in usb buffer is larger than 4ms */
            if (((uint32_t)source->param.data_dl.current_notification_index == 0) &&
                (*avail_size < (4 * (uint32_t)source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size)))
            {
                /* the data is not enough, so change avail_size to zero */
                *avail_size = 0;
                if (hal_nvic_query_exception_number() > 0)
                {
                    dongle_handle->data_status = GAMING_MODE_STREAM_DATA_EMPTY;
                }
            }
            else
            {
                /* change avail_size to actual data size in the buffer */
                frame_count = *avail_size / source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size;
                *avail_size = *avail_size - frame_count * sizeof(audio_transmitter_frame_header_t);

                if (hal_nvic_query_exception_number() > 0)
                {
                    #ifdef AIR_SOFTWARE_BUFFER_ENABLE
                    /* check if there are at least 2.5ms data */
                    remain_samples = stream_function_sw_buffer_get_channel_used_size(dongle_handle->buffer_port, 1) / sizeof(int16_t);
                    #endif /* AIR_SOFTWARE_BUFFER_ENABLE */

                    if ((frame_count*48+remain_samples) >= 120)
                    {
                        /* update data status to indicate there are data in stream and the mixer needs to wait this stream */
                        dongle_handle->data_status = GAMING_MODE_STREAM_DATA_IN_STREAM;
                    }
                    else
                    {
                        #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
                        /* When in irq level, there should be at least 2ms data in share buffer because the period of ccni is 2.5ms */
                        DSP_MW_LOG_E("[audio transmitter][source]: data is not enough, %u", 1, *avail_size);
                        #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */

                        /* update data status to indicate there are not enough data in stream and the mixer do not need to wait this stream */
                        dongle_handle->data_status = GAMING_MODE_STREAM_DATA_EMPTY;
                        *avail_size = 0;
                        dongle_handle->process_frames = 0;
                    }
                }
            }
        }
    }

    #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[audio transmitter][source][get_avail_size]:%u, %u, %d, %u, %d", 5,
                source->streamBuffer.ShareBufferInfo.ReadOffset,
                source->streamBuffer.ShareBufferInfo.WriteOffset,
                *avail_size,
                current_timestamp,
                hal_nvic_query_exception_number());
    #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */

    return true;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 uint32_t gaming_mode_dongle_dl_source_copy_payload(SOURCE source, uint8_t *src_buf, uint8_t *dst_buf, uint32_t length)
{
    DSP_STREAMING_PARA_PTR stream = NULL;
    audio_transmitter_frame_header_t *frame_header = NULL;
    uint32_t total_frames = 0;
    uint32_t total_samples = 0;
    uint32_t current_frame_samples = 0;
    uint32_t current_frame = 0;
    uint32_t avail_size = 0;
    uint32_t total_buffer_size = 0;
    uint32_t ReadOffset = 0;
    gaming_mode_dongle_dl_handle_t *dongle_handle = (gaming_mode_dongle_dl_handle_t *)(source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.handle);
    UNUSED(dongle_handle);
    UNUSED(dst_buf);

    /* update data receive timestamp */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.data_timestamp);

    /* get the stream */
    stream = DSP_Streaming_Get(source, source->transform->sink);

    /* get the total frame number and buffer size in share buffer */
    total_frames = length / (source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size - sizeof(audio_transmitter_frame_header_t));
    total_buffer_size = source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size *
                        source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_num;

    /* copy the pcm data in share buffer into the stream buffers */
    if (source->param.data_dl.current_notification_index == 0)
    {
        gaming_mode_dongle_dl_source_get_avail_size(source, &avail_size);
        if(avail_size > length)
        {
            /* at the first time, maybe the actual data size is larger than stream->callback.EntryPara.in_malloc_size */
            /* in dsp_callback.c, DSP_Callback_Processing will cut length to the stream->callback.EntryPara.in_malloc_size */
            /* in here, we need to change length back to the actual data size */
            length = avail_size;
            total_frames = length / (source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size - sizeof(audio_transmitter_frame_header_t));
        }

        /* parameters check */
        if (total_frames < 4)
        {
            DSP_MW_LOG_E("[audio transmitter][source]: error parameters %d, %d", 2, total_frames, length);
            configASSERT(0);
        }

        /* Only copy the last 3.5ms data from the share buffer and the remian data in the head are dropped */
        total_samples = 0;
        current_frame = 0;
        ReadOffset =(source->streamBuffer.ShareBufferInfo.ReadOffset +
                        source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * (total_frames-4)
                    ) % total_buffer_size;
        src_buf = (uint8_t *)(source->streamBuffer.ShareBufferInfo.startaddr + ReadOffset);
        while(current_frame < 4)
        {
            /* get current frame info and current frame samples */
            frame_header = (audio_transmitter_frame_header_t *)src_buf;
            current_frame_samples = frame_header->payload_len / (sizeof(U16) * 2);
            if (current_frame == 0)
            {
                /* only copy 0.5ms(48K*0.5ms=24samples) data at the first time */
                current_frame_samples = 24;
                /* offset 0.5ms data in src_buf */
                src_buf = src_buf + 24 * (sizeof(U16) * 2);
            }

            /* copy usb audio data from share buffer */
            ShareBufferCopy_I2D_16bit((U32 *)(src_buf + sizeof(audio_transmitter_frame_header_t)),
                                    (U16 *)(stream->callback.EntryPara.in_ptr[0])+total_samples,
                                    (U16 *)(stream->callback.EntryPara.in_ptr[1])+total_samples,
                                    current_frame_samples);

            if (current_frame == 0)
            {
                /* offset back src_buf to the original position */
                src_buf = src_buf - 24 * (sizeof(U16) * 2);
            }

            /* update total samples */
            total_samples += current_frame_samples;

            /* update copied frame number */
            current_frame += 1;

            /* change to the next frame */
            ReadOffset =(source->streamBuffer.ShareBufferInfo.ReadOffset +
                        source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * (total_frames-4+current_frame)
                        ) % total_buffer_size;
            src_buf = (uint8_t *)(source->streamBuffer.ShareBufferInfo.startaddr + ReadOffset);

            #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
            // DSP_MW_LOG_I("[audio transmitter][source]:copy samples %u, %d, %d, %d, %d, %d", 6,
            //                 src_buf,
            //                 length,
            //                 total_frames,
            //                 current_frame,
            //                 current_frame_samples,
            //                 total_samples);
            #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */
        }
        // if(total_samples != 168)
        // {
        //     DSP_MW_LOG_E("[audio transmitter][source]: error copy samples %d, %d", 2, length, total_samples);
        //     configASSERT(0);
        // }

        /* update processed frames */
        source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.process_frames = total_frames;

        #ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
        /* get default capid */
        if (dongle_handle->index == 1)
        {
            dongle_handle->cap_id_default = DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE;
            dongle_handle->cap_id_cur = dongle_handle->cap_id_default;
        }
        else
        {
            dongle_handle->cap_id_default = gaming_mode_dongle_first_dl_handle->cap_id_default;
            dongle_handle->cap_id_cur = gaming_mode_dongle_first_dl_handle->cap_id_cur;
        }
        #endif /* AIR_SOFTWARE_CLK_SKEW_ENABLE */

        #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
        uint32_t current_timestamp = 0;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
        DSP_MW_LOG_I("[audio transmitter][source][copy_payload]: get first data packet time = %u.", 1, current_timestamp);
        #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */
    }
    else
    {
        /* parameters check */
        if (total_frames > 5)
        {
            /* in here, it means there are at least 6 packet data in share buffer. */
            DSP_MW_LOG_E("[audio transmitter][source]: too much data in share buffer %d, %d, %u, %u, 0x%x, 0x%x", 6,
                        total_frames,
                        length,
                        source->streamBuffer.ShareBufferInfo.ReadOffset,
                        source->streamBuffer.ShareBufferInfo.WriteOffset,
                        gaming_mode_music_data_in_gpt_count,
                        gaming_mode_music_ccni_gpt_count);

            // configASSERT(0);

            /* get the new read index */
            source->streamBuffer.ShareBufferInfo.ReadOffset = (source->streamBuffer.ShareBufferInfo.WriteOffset + total_buffer_size -
                                                                source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * 3
                                                                ) % total_buffer_size;

            /* set WriteOffset - 3 as the new ReadOffset, it will drop the other data in the share buffer */
            extern VOID audio_transmitter_share_information_update_read_offset(SOURCE source, U32 ReadOffset);
            audio_transmitter_share_information_update_read_offset(source, source->streamBuffer.ShareBufferInfo.ReadOffset);

            /* get new share buffer info*/
            extern VOID audio_transmitter_share_information_fetch(SOURCE source, SINK sink);
            audio_transmitter_share_information_fetch(source, NULL);

            /* only process the last 3 packet data */
            total_frames = 3;
            length = 3 * (source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size - sizeof(audio_transmitter_frame_header_t));

            DSP_MW_LOG_E("[audio transmitter][source]: change status, %d, %d, %u, %u", 4,
                        total_frames,
                        length,
                        source->streamBuffer.ShareBufferInfo.ReadOffset,
                        source->streamBuffer.ShareBufferInfo.WriteOffset);
        }

        #ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
        int32_t compensatory_samples = 0;
        uint32_t remain_samples = 0;

        /* get remain samples */
        remain_samples = stream_function_sw_buffer_get_channel_used_size(dongle_handle->buffer_port, 1) / sizeof(int16_t);

        /* choose which way to do compensatory */
        if (dongle_handle->index == 1)
        {
            dongle_handle->compen_method = GAMING_MODE_COMPENSATORY_METHOD_CAPID_ADJUST;
        }
        else if (dongle_handle->compen_method == GAMING_MODE_COMPENSATORY_METHOD_SW_CLK_SKEW)
        {
            /* do nothing because sw clk skew is under running */
        }
        else if ((gaming_mode_dongle_first_dl_handle->data_status == GAMING_MODE_STREAM_DATA_EMPTY) &&
                (dongle_handle->compen_method == GAMING_MODE_COMPENSATORY_METHOD_DISABLE))
        {
            if (dongle_handle->cap_id_count < 10)
            {
                dongle_handle->cap_id_count += 1;
            }

            if (dongle_handle->cap_id_count == 10)
            {
                dongle_handle->compen_method = GAMING_MODE_COMPENSATORY_METHOD_CAPID_ADJUST;
                dongle_handle->compen_count = 0;
                dongle_handle->compen_flag = 0;
            }
        }
        else if ((gaming_mode_dongle_first_dl_handle->data_status != GAMING_MODE_STREAM_DATA_EMPTY) &&
                (dongle_handle->compen_method == GAMING_MODE_COMPENSATORY_METHOD_CAPID_ADJUST))
        {
            dongle_handle->cap_id_count = 0;
            dongle_handle->compen_method = GAMING_MODE_COMPENSATORY_METHOD_DISABLE;
            dongle_handle->compen_count = 0;
            dongle_handle->compen_flag = 0;
            dongle_handle->total_compen_samples = 0;
        }
        else if ((gaming_mode_dongle_first_dl_handle->data_status != GAMING_MODE_STREAM_DATA_EMPTY) &&
                (dongle_handle->total_compen_samples != 0) &&
                (dongle_handle->compen_method == GAMING_MODE_COMPENSATORY_METHOD_DISABLE))
        {
            dongle_handle->compen_method = GAMING_MODE_COMPENSATORY_METHOD_SW_CLK_SKEW;
        }

        /* do compensatory */
        if (dongle_handle->compen_method == GAMING_MODE_COMPENSATORY_METHOD_CAPID_ADJUST)
        {
            /* calculator compensatory samples */
            if ((dongle_handle->total_compen_samples == 0) && (total_frames == 2) && ((remain_samples+total_frames*48-120) <= 0))
            {
                if (dongle_handle->compen_count < 200)
                {
                    /* in here, it means there may be +1ms compensatory but its persistent period is less than 1 second */
                    dongle_handle->compen_count += 1;
                    dongle_handle->compen_flag = 1;
                }
                else
                {
                    /* in here, it means there is +1ms compensatory and its persistent period is more than 1 second */
                    /* So we need to adjust 26M clock to compensatory +1ms data as soon as possible */
                    dongle_handle->total_compen_samples = 48;
                    dongle_handle->compen_count = 0;
                    dongle_handle->compen_flag = 0;
                }
            }
            else if ((dongle_handle->total_compen_samples == 0) && (dongle_handle->compen_flag == 1) && (total_frames == 2) && ((remain_samples+total_frames*48-120) > 0))
            {
                /* in here, it means maybe the +1ms compensatory is false alarm and the usb irq is just not stable in this time */
                if (dongle_handle->compen_count != 0)
                {
                    dongle_handle->compen_count -= 1;
                }
                if (dongle_handle->compen_count == 0)
                {
                    dongle_handle->compen_flag = 0;
                }
            }
            else if ((dongle_handle->total_compen_samples > 0) && (total_frames == 2) && ((remain_samples+total_frames*48-120) > 0))
            {
                if (dongle_handle->compen_count < 200)
                {
                    /* in here, it means maybe the +1ms compensatory is finished but its persistent period is less than 1 second */
                    dongle_handle->compen_count += 1;
                }
                else
                {
                    /* in here, it means maybe the +1ms compensatory is finished and its persistent period is more than 1 second */
                    /* So we need to restore 26M clock source to default value */
                    dongle_handle->total_compen_samples = 0;
                    dongle_handle->compen_count = 0;
                    dongle_handle->compen_flag = 0;
                }
            }
            else if ((dongle_handle->total_compen_samples == 0) && (total_frames == 3) && ((remain_samples+total_frames*48-120) >= 96))
            {
                if (dongle_handle->compen_count < 200)
                {
                    /* in here, it means there may be -1ms compensatory but its persistent period is less than 1 second */
                    dongle_handle->compen_count += 1;
                    dongle_handle->compen_flag = -1;
                }
                else
                {
                    /* in here, it means there is -1ms compensatory and its persistent period is more than 1 second */
                    /* So we need to adjust 26M clock to compensatory -1ms data as soon as possible */
                    dongle_handle->total_compen_samples = -48;
                    dongle_handle->compen_count = 0;
                }
            }
            else if ((dongle_handle->total_compen_samples == 0) && (dongle_handle->compen_flag == -1) && (total_frames == 3) && ((remain_samples+total_frames*48-120) < 96))
            {
                /* in here, it means maybe the -1ms compensatory is false alarm and the usb irq is just not stable in this time */
                if (dongle_handle->compen_count != 0)
                {
                    dongle_handle->compen_count -= 1;
                }
                if (dongle_handle->compen_count == 0)
                {
                    dongle_handle->compen_flag = 0;
                }
            }
            else if ((dongle_handle->total_compen_samples < 0) && (total_frames == 3) && ((remain_samples+total_frames*48-120) < 96))
            {
                if (dongle_handle->compen_count < 200)
                {
                    /* in here, it means maybe the -1ms compensatory is finished but its persistent period is less than 1 second */
                    dongle_handle->compen_count += 1;
                }
                else
                {
                    /* in here, it means maybe the -1ms compensatory is finished and its persistent period is more than 1 second */
                    /* So we need to restore 26M clock source to default value */
                    dongle_handle->total_compen_samples = 0;
                    dongle_handle->compen_count = 0;
                    dongle_handle->compen_flag = 0;
                }
            }

            /* adjust 26M clock source by requirment */
            if (dongle_handle->total_compen_samples > 0)
            {
                /* adjust 26M clock source to the lowest and it will cause +1ms data after a time */
                DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE_SEL = 0x1;
                DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE     = 511;
                dongle_handle->cap_id_cur = 511;
            }
            else if (dongle_handle->total_compen_samples < 0)
            {
                /* adjust 26M clock source to the highest and it will cause -1ms data after a time */
                DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE_SEL = 0x1;
                DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE     = 1;
                dongle_handle->cap_id_cur = 1;
            }
            else
            {
                /* adjust 26M clock source to the default setting */
                if (DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE != dongle_handle->cap_id_default)
                {
                    DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE_SEL = 0x1;
                    DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE     = dongle_handle->cap_id_default;
                    dongle_handle->cap_id_cur = dongle_handle->cap_id_default;
                }
            }

            /* always output 120 samples */
            stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 1, 120*sizeof(int16_t));
            stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 2, 120*sizeof(int16_t));

            /* bypass sw clk skew */
            compensatory_samples = 0;
            stream_function_sw_clk_skew_configure_compensation_samples(dongle_handle->clk_skew_port, compensatory_samples);
            source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.clk_skew.compen_samples = compensatory_samples;
        }
        else if (dongle_handle->compen_method == GAMING_MODE_COMPENSATORY_METHOD_SW_CLK_SKEW)
        {
            /* double check if the compen_samples is right */
            if ((dongle_handle->total_compen_samples > 0) && ((remain_samples+total_frames*48-120) >= 48))
            {
                dongle_handle->total_compen_samples = 48 - (remain_samples+total_frames*48-120);
            }
            else if ((dongle_handle->total_compen_samples < 0) && ((remain_samples+total_frames*48-120) <= 0))
            {
                dongle_handle->total_compen_samples = 48 - (remain_samples+total_frames*48-120);
            }

            if (dongle_handle->total_compen_samples > 0)
            {
                /* buffer output 119 samples, clk skew will change them to 120 samples */
                compensatory_samples = 1;
                dongle_handle->total_compen_samples -= 1;
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 1, 119*sizeof(int16_t));
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 2, 119*sizeof(int16_t));
            }
            else if (dongle_handle->total_compen_samples < 0)
            {
                /* buffer output 121 samples, clk skew will change them to 120 samples */
                compensatory_samples = -1;
                dongle_handle->total_compen_samples += 1;
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 1, 121*sizeof(int16_t));
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 2, 121*sizeof(int16_t));
            }
            else
            {
                /* buffer output 120 samples */
                compensatory_samples = 0;
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 1, 120*sizeof(int16_t));
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 2, 120*sizeof(int16_t));
            }
            stream_function_sw_clk_skew_configure_compensation_samples(dongle_handle->clk_skew_port, compensatory_samples);
            source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.clk_skew.compen_samples = compensatory_samples;

            if (dongle_handle->total_compen_samples == 0)
            {
                dongle_handle->compen_method = GAMING_MODE_COMPENSATORY_METHOD_DISABLE;
                dongle_handle->compen_count = 0;
                dongle_handle->compen_flag = 0;
            }
        }
        else
        {
            /* calculator compensatory samples */
            if ((dongle_handle->total_compen_samples == 0) && (total_frames == 2) && ((remain_samples+total_frames*48-120) <= 0))
            {
                if (dongle_handle->compen_count < 10)
                {
                    /* in here, it means there may be +1ms compensatory but its persistent period is less than 50ms */
                    dongle_handle->compen_count += 1;
                    dongle_handle->compen_flag = 1;
                }
                else
                {
                    /* in here, it means there is +1ms compensatory and its persistent period is more than 50ms */
                    dongle_handle->total_compen_samples = 48;
                    dongle_handle->compen_count = 0;
                    dongle_handle->compen_flag = 0;
                }
            }
            else if ((dongle_handle->total_compen_samples == 0) && (dongle_handle->compen_flag == 1) && (total_frames == 2) && ((remain_samples+total_frames*48-120) > 0))
            {
                /* in here, it means maybe the +1ms compensatory is false alarm and the usb irq is just not stable in this time */
                if (dongle_handle->compen_count != 0)
                {
                    dongle_handle->compen_count -= 1;
                }
                if (dongle_handle->compen_count == 0)
                {
                    dongle_handle->compen_flag = 0;
                }
            }
            else if ((dongle_handle->total_compen_samples == 0) && (total_frames == 3) && ((remain_samples+total_frames*48-120) >= 96))
            {
                if (dongle_handle->compen_count < 10)
                {
                    /* in here, it means there may be -1ms compensatory but its persistent period is less than 50ms */
                    dongle_handle->compen_count += 1;
                    dongle_handle->compen_flag = -1;
                }
                else
                {
                    /* in here, it means there is -1ms compensatory and its persistent period is more than 50ms*/
                    dongle_handle->total_compen_samples = 48 - (remain_samples+total_frames*48-120);
                    dongle_handle->compen_count = 0;
                }
            }
            else if ((dongle_handle->total_compen_samples == 0) && (dongle_handle->compen_flag == -1) && (total_frames == 3) && ((remain_samples+total_frames*48-120) < 96))
            {
                /* in here, it means maybe the -1ms compensatory is false alarm and the usb irq is just not stable in this time */
                if (dongle_handle->compen_count != 0)
                {
                    dongle_handle->compen_count -= 1;
                }
                if (dongle_handle->compen_count == 0)
                {
                    dongle_handle->compen_flag = 0;
                }
            }

            /* always output 120 samples */
            stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 1, 120*sizeof(int16_t));
            stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 2, 120*sizeof(int16_t));

            /* bypass sw clk skew */
            compensatory_samples = 0;
            stream_function_sw_clk_skew_configure_compensation_samples(dongle_handle->clk_skew_port, compensatory_samples);
            source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.clk_skew.compen_samples = compensatory_samples;
        }
        #endif /* AIR_SOFTWARE_CLK_SKEW_ENABLE */

        /* Copy all data from the share buffer */
        total_samples = 0;
        current_frame = 0;
        ReadOffset = source->streamBuffer.ShareBufferInfo.ReadOffset;
        src_buf = (uint8_t *)(source->streamBuffer.ShareBufferInfo.startaddr + ReadOffset);
        while(current_frame < total_frames)
        {
            /* get current frame info and current frame samples */
            frame_header = (audio_transmitter_frame_header_t *)src_buf;
            current_frame_samples = frame_header->payload_len / (sizeof(U16) * 2);

            /* copy usb audio data from share buffer */
            ShareBufferCopy_I2D_16bit((U32 *)(src_buf + sizeof(audio_transmitter_frame_header_t)),
                                    (U16 *)(stream->callback.EntryPara.in_ptr[0])+total_samples,
                                    (U16 *)(stream->callback.EntryPara.in_ptr[1])+total_samples,
                                    current_frame_samples);

            /* update total samples */
            total_samples += current_frame_samples;

            /* update copied frame number */
            current_frame += 1;

            /* change to the next frame */
            ReadOffset =(source->streamBuffer.ShareBufferInfo.ReadOffset +
                        source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * current_frame
                        ) % total_buffer_size;
            src_buf = (uint8_t *)(source->streamBuffer.ShareBufferInfo.startaddr + ReadOffset);

            #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
            // DSP_MW_LOG_I("[audio transmitter][source]:copy samples %u, %d, %d, %d, %d, %d", 6,
            //                 src_buf,
            //                 length,
            //                 total_frames,
            //                 current_frame,
            //                 current_frame_samples,
            //                 total_samples);
            #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */
        }
        if(total_samples != (length / (sizeof(S16) * 2)))
        {
            DSP_MW_LOG_E("[audio transmitter][source]: error copy samples %d, %d", 2, length, total_samples);
            configASSERT(0);
        }

        /* update processed frames */
        source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.process_frames = total_frames;
    }

    /* update stream status */
    stream->callback.EntryPara.in_size = total_samples * sizeof(S16);
    stream->callback.EntryPara.in_channel_num = 2;
    stream->callback.EntryPara.in_sampling_rate = FS_RATE_48K;
    stream->callback.EntryPara.resolution.process_res = RESOLUTION_16BIT;

    /* update state machine */
    dongle_handle = (gaming_mode_dongle_dl_handle_t *)(source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.handle);
    dongle_handle->compen_samples = source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.clk_skew.compen_samples;
    dongle_handle->process_frames = source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.process_frames;
    dongle_handle->read_offset    = source->streamBuffer.ShareBufferInfo.ReadOffset;

    #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[audio transmitter][source][copy_payload]: %d, %d, %u, %d", 4, total_samples, length, current_timestamp, hal_nvic_query_exception_number());
    #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */

    /* avoid payload length check error in here */
    length = (length > source->param.data_dl.max_payload_size) ? source->param.data_dl.max_payload_size : length;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&gaming_mode_music_data_in_gpt_count);
    dongle_handle->data_in_gpt_count = gaming_mode_music_data_in_gpt_count;

    return length;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 uint32_t gaming_mode_dongle_dl_source_get_new_read_offset(SOURCE source, U32 amount)
{
    uint32_t total_buffer_size;
    SHARE_BUFFER_INFO_PTR ShareBufferInfo;
    uint32_t ReadOffset;
    UNUSED(amount);

    /* get new read offset */
    ShareBufferInfo = &source->streamBuffer.ShareBufferInfo;
    total_buffer_size = ShareBufferInfo->sub_info.block_info.blk_size * ShareBufferInfo->sub_info.block_info.blk_num;
    ReadOffset =(ShareBufferInfo->ReadOffset +
                ShareBufferInfo->sub_info.block_info.blk_size * source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.process_frames
                ) % total_buffer_size;

    #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[audio transmitter][source][get_new_read_offset]: %u, %u, %d", 3, ReadOffset, current_timestamp, hal_nvic_query_exception_number());
    #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */

    return ReadOffset;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 void gaming_mode_dongle_dl_source_drop_postprocess(SOURCE source, uint32_t amount)
{
    uint32_t saved_mask = 0;
    UNUSED(amount);

    /* update index */
    if (source->param.data_dl.current_notification_index == 0)
    {
        source->param.data_dl.current_notification_index = 1;
    }

    #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
    extern CELT_ENC_PARA gCeltEncPara;
    uint32_t remain_samples = 0;
    gaming_mode_dongle_dl_handle_t *dongle_handle = (gaming_mode_dongle_dl_handle_t *)(source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.handle);
    remain_samples = stream_function_sw_buffer_get_channel_remain_size(dongle_handle->buffer_port, 1) / sizeof(int16_t);
    DSP_MW_LOG_I("[audio transmitter][source][drop_postprocess]: %d, %d, %d, %d, %d", 5,
                    source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.clk_skew.compen_samples,
                    remain_samples,
                    gCeltEncPara.RemainSamples,
                    source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.process_frames,
                    amount);
    #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */

    /* update state machine */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.fetch_flag = 0;
    source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.process_frames = 0;
    hal_nvic_restore_interrupt_mask(saved_mask);

    #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[audio transmitter][source][drop_postprocess]: %u, %d", 2, current_timestamp, hal_nvic_query_exception_number());
    #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */
}

bool gaming_mode_dongle_dl_source_close(SOURCE source)
{
    if (source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.timer.handle)
    {
        hal_gpt_sw_stop_timer_us(source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.timer.handle);
        hal_gpt_sw_free_timer(source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.timer.handle);
        source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.timer.handle = 0;
    }

    #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[audio transmitter][source][close]: %u, %d", 2, current_timestamp, hal_nvic_query_exception_number());
    #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */

    return true;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 bool gaming_mode_dongle_dl_sink_get_avail_size(SINK sink, uint32_t *avail_size)
{
    if (sink->streamBuffer.ShareBufferInfo.ReadOffset < sink->streamBuffer.ShareBufferInfo.WriteOffset)
    {
        /* normal case */
        *avail_size = sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_num
                    - sink->streamBuffer.ShareBufferInfo.WriteOffset
                    + sink->streamBuffer.ShareBufferInfo.ReadOffset;
    }
    else if (sink->streamBuffer.ShareBufferInfo.ReadOffset == sink->streamBuffer.ShareBufferInfo.WriteOffset)
    {
        if(sink->streamBuffer.ShareBufferInfo.bBufferIsFull == true)
        {
            /* buffer is full, so ReadOffset == WriteOffset */
            // *avail_size = 0;
            /* In this case, overwrite is allowed*/
            *avail_size = sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size;

            #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
            DSP_MW_LOG_I("[bt common][sink]: buffer is full, index = %u, status = %u", 2,
                            sink->streamBuffer.ShareBufferInfo.ReadOffset,
                            sink->param.bt_ul.status);
            #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */
        }
        else
        {
            /* buffer is empty, so ReadOffset == WriteOffset */
            *avail_size = sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_num;
        }
    }
    else
    {
        /* buffer wrapper case */
        *avail_size = sink->streamBuffer.ShareBufferInfo.ReadOffset - sink->streamBuffer.ShareBufferInfo.WriteOffset;
    }
    /* change avail_size to blk_size-sizeof(bt_common_gaming_mode_music_frame_header_t) if the avail_size is too large */
    if (*avail_size >= sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size)
    {
        *avail_size = sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size - sizeof(bt_common_gaming_mode_music_frame_header_t);
    }

    #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[bt common][sink][get_avail_size]: %d, %u, %d", 3, *avail_size, current_timestamp, hal_nvic_query_exception_number());
    #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */

    return true;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 uint32_t gaming_mode_dongle_dl_sink_copy_payload(SINK sink, uint8_t *src_buf, uint32_t length)
{
    uint32_t i = 0;
    uint32_t saved_mask = 0;
    uint32_t *dst_buf = NULL;
    uint32_t *src_buf0 = NULL;
    uint32_t diff_clk;
    /* make sure that game_frame_header's start address is 4B aligned on the stack for word accessing */
    __attribute__((__aligned__(4))) bt_common_gaming_mode_music_frame_header_t game_frame_header;
    uint32_t crc32;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (gaming_mode_dongle_sink == NULL)
    {
        gaming_mode_dongle_sink = sink;
    }
    else if (gaming_mode_dongle_sink != sink)
    {
        /* in here, it means the sink has been changed because the mixer and we need sync sink setting at now for avoiding seq number is not continuous */
        sink->param.bt_ul.current_notification_index                                         = gaming_mode_dongle_sink->param.bt_ul.current_notification_index;
        sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.data_timestamp  = gaming_mode_dongle_sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.data_timestamp;
        sink->param.bt_ul.seq_num                                                            = gaming_mode_dongle_sink->param.bt_ul.seq_num;
        sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_timestamp = gaming_mode_dongle_sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_timestamp;
        sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_status    = gaming_mode_dongle_sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_status;
        sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_failcount = gaming_mode_dongle_sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_failcount;

        /* change gaming mode dongle sink to this current sink */
        gaming_mode_dongle_sink = sink;
    }

    /* check if it is not the first time */
    if (sink->param.bt_ul.current_notification_index > 0)
    {
        /* get bt clk diff */
        if (gaming_mode_music_ccni_bt_clk >= sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.data_timestamp)
        {
            diff_clk = gaming_mode_music_ccni_bt_clk - sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.data_timestamp;
        }
        else
        {
            diff_clk = gaming_mode_music_ccni_bt_clk + (0x0fffffff - sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.data_timestamp + 1);
        }

        /* if the bt clock diff is larger than 2 packet, the seq number need to be compensatory */
        if(diff_clk >= 16)
        {
            #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
            DSP_MW_LOG_I("[bt common][sink][copy_payload]old seq num = %u, last time = %u, current time = %u", 3,
                        sink->param.bt_ul.seq_num,
                        sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.data_timestamp,
                        bt_clk);
            #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */

            sink->param.bt_ul.seq_num = sink->param.bt_ul.seq_num  + diff_clk/8 - 1;
            sink->param.bt_ul.seq_num = sink->param.bt_ul.seq_num & 0xff;

            #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
            DSP_MW_LOG_I("[bt common][sink][copy_payload]new seq num = %u", 1, sink->param.bt_ul.seq_num);
            #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */
        }

        /* update time stamp */
        sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.data_timestamp = gaming_mode_music_ccni_bt_clk;
    }
    else
    {
        /* if it is first time, just update time stamp */
        sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.data_timestamp = gaming_mode_music_ccni_bt_clk;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    /* dst_buf and src_buf must be 4-byte aligned */
    dst_buf = (uint32_t *)(sink->streamBuffer.ShareBufferInfo.startaddr + sink->streamBuffer.ShareBufferInfo.WriteOffset);
    src_buf0 = (uint32_t *)src_buf;

    /* copy payload header */
    game_frame_header.seq_num = (uint8_t)(sink->param.bt_ul.seq_num);
    game_frame_header.frame_size = 0;
    game_frame_header.check_sum = 0;
    for(; game_frame_header.frame_size < length; game_frame_header.frame_size++)
    {
        game_frame_header.check_sum += *src_buf++;
    }
    game_frame_header.check_sum += game_frame_header.seq_num;
    crc32 = 0xFFFFFF00;
    game_frame_header.crc32 = CRC32_Generate((uint8_t *)src_buf0, game_frame_header.frame_size, crc32);
    // memcpy(dst_buf, &game_frame_header, sizeof(bt_common_gaming_mode_music_frame_header_t));
    for (i=0; i < (sizeof(bt_common_gaming_mode_music_frame_header_t)/4); i++)
    {
        /* use word access for better performance */
        *dst_buf++ = *((uint32_t *)(&game_frame_header) + i);
    }

    /* copy payload */
    // memcpy(dst_buf + sizeof(bt_common_gaming_mode_music_frame_header_t), src_buf, length);
    if (length%4)
    {
        configASSERT(0);
    }
    for (i=0; i < (length/4); i++)
    {
        /* use word access for better performance */
        *dst_buf++ = *src_buf0++;
    }

    /* update seq_num */
    sink->param.bt_ul.seq_num += 1;
    sink->param.bt_ul.seq_num = sink->param.bt_ul.seq_num & 0xff;

    #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[bt common][sink][copy_payload]:%u, %d, %u, %d", 4, game_frame_header.seq_num, length, current_timestamp, hal_nvic_query_exception_number());
    #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */

    return length;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 bool gaming_mode_dongle_dl_sink_get_new_write_offset(SINK sink, U32 amount, uint32_t *new_write_offset)
{
    bool ret = false;
    uint32_t bt_clk;
    uint16_t bt_phase;
    uint32_t diff_clk;
    UNUSED(amount);
    UNUSED(new_write_offset);

    if(sink->param.bt_ul.current_notification_index == 0)
    {
        /* get current bt clock */
        extern VOID MCE_GetBtClk(BTCLK* pCurrCLK, BTPHASE* pCurrPhase,BT_CLOCK_OFFSET_SCENARIO type);
        MCE_GetBtClk((BTCLK*)&bt_clk, (BTPHASE*)&bt_phase,BT_CLK_Offset);
        /* get bt clk diff */
        if (bt_clk >= sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_timestamp)
        {
            diff_clk = bt_clk - sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_timestamp;
        }
        else
        {
            diff_clk = bt_clk + (0x0fffffff - sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_timestamp + 1);
        }

        if (diff_clk > 2)
        {
            /* if the generation time of this packet is too larger(> 2*312.5us), drop this packet and not update write offset */
            *new_write_offset = sink->streamBuffer.ShareBufferInfo.WriteOffset;
            /* change event status to 0 to indicate that first packet has not been created */
            sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_status = 0;
            /* do not update write offset */
            ret = false;
            /* update fail count */
            sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_failcount += 1;
            if (sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_failcount > 100)
            {
                DSP_MW_LOG_E("[bt common][sink]: get first packet error 0x%x, 0x%x.", 2,
                            sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_timestamp,
                            bt_clk);
                configASSERT(0);
            }
        }
        else
        {
            /* if the generation time of this packet is ok, send this packet as the first packet to bt controller */
            *new_write_offset = (sink->streamBuffer.ShareBufferInfo.WriteOffset + sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size) %
                        (sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_num);
            /* change event status to 1 to indicate that first packet has been created */
            sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_status = 1;
            /* change event timestamp to current bt clock */
            sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_timestamp = bt_clk;
            /* update write offset */
            ret = true;
        }
        gaming_mode_music_first_packet_bt_clk = diff_clk;
    }
    else
    {
        *new_write_offset = (sink->streamBuffer.ShareBufferInfo.WriteOffset + sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size) %
                        (sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_num);
        /* update write offset */
        ret = true;
    }

    return ret;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 bool gaming_mode_dongle_dl_sink_query_notification(SINK sink, bool *notification_flag)
{
    if (sink->param.bt_ul.current_notification_index == 0)
    {
        if (sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_status == 0)
        {
            /* Becasue the first packet has not been created, so do not update index and send notification */
            /* the data in the buffer is not enough, so do not need to do notification */
            *notification_flag = false;
        }
        else
        {
            /* the data in the buffer is enough at first time, so do need to do notification */
            *notification_flag = true;
            /* increase the index */
            sink->param.bt_ul.current_notification_index += 1;
        }
    }
    else
    {
        /* the data in the buffer is enough and notification has been sent, so do not need to do notification */
        *notification_flag = false;
    }

    return true;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 bool gaming_mode_dongle_dl_sink_send_data_ready_notification(SINK sink)
{
    hal_ccni_message_t msg;
    UNUSED(msg);

    /* prepare message */
    msg.ccni_message[0] = AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0;
    /* in here, event_timestamp means the bt clock of the first packet flush and use it as the bt anchor time */
    msg.ccni_message[1] = sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_timestamp;

    /* send ccni messge to controller */
    hal_ccni_set_event(CCNI_DSP0_TO_CM4_EVENT2, &msg);
    DSP_MW_LOG_I("[bt common][sink]: send ccni to bt controller done, %u.", 1,
                sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_timestamp);

    return true;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 void gaming_mode_dongle_dl_sink_flush_postprocess(SINK sink, uint32_t amount)
{
    UNUSED(amount);
    uint32_t bt_clk;
    uint16_t bt_phase;
    VOID MCE_GetBtClk(BTCLK* pCurrCLK, BTPHASE* pCurrPhase,BT_CLOCK_OFFSET_SCENARIO type);
    MCE_GetBtClk((BTCLK*)&bt_clk, (BTPHASE*)&bt_phase,BT_CLK_Offset);

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&gaming_mode_music_bt_out_gpt_count);

    #if defined(AIR_SOFTWARE_MIXER_ENABLE) && defined(AIR_CELT_ENC_ENABLE)
    if (gaming_mode_music_first_packet_bt_clk >= 2)
    {
        DSP_MW_LOG_W("[bt common][sink]: get first packet warning 0x%x, 0x%x, 0x%x, 0x%x, 0x%x.", 5,
                    gaming_mode_music_ccni_bt_clk,
                    bt_clk,
                    gaming_mode_music_ccni_gpt_count,
                    gaming_mode_music_data_in_gpt_count,
                    gaming_mode_music_bt_out_gpt_count);
        gaming_mode_music_first_packet_bt_clk = 0;
    }
    #endif /* defined(AIR_SOFTWARE_MIXER_ENABLE) && defined(AIR_CELT_ENC_ENABLE) */

    #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    #if GAMING_MODE_MUSIC_DONGLE_DEBUG_DUMP
    #ifdef AIR_CELT_ENC_ENABLE
    extern CELT_ENC_PARA gCeltEncPara;
    /* dump encoder's input data */
    LOG_AUDIO_DUMP( (uint8_t *)(gCeltEncPara.RemainMemPtr),
                    gCeltEncPara.InputSamples*4,
                    AUDIO_CODEC_IN);
    #endif /* AIR_CELT_ENC_ENABLE */
    #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_DUMP */
    #endif /* MTK_AUDIO_DUMP_BY_CONFIGTOOL */

    int32_t i;
    uint32_t remain_samples = 0;
    gaming_mode_dongle_dl_handle_t *c_handle = NULL;
    extern gaming_mode_dongle_dl_handle_t *gaming_mode_dongle_first_dl_handle;
    c_handle = gaming_mode_dongle_first_dl_handle;
    for (i = 0; i < gaming_mode_dongle_first_dl_handle->total_number; i++)
    {
        SOURCE source = (SOURCE)(c_handle->owner);

        if (source->transform != NULL)
        {
            #ifdef AIR_SOFTWARE_BUFFER_ENABLE
            remain_samples = stream_function_sw_buffer_get_channel_used_size(c_handle->buffer_port, 1) / sizeof(int16_t);
            #endif /* AIR_SOFTWARE_BUFFER_ENABLE */
            DSP_MW_LOG_I("[bt common][sink][flush_postprocess]: handle 0x%x: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, 0x%x, 0x%x, 0x%x, 0x%x", 15,
                            c_handle,
                            sink->param.bt_ul.seq_num,
                            c_handle->compen_samples,
                            remain_samples,
                            c_handle->process_frames,
                            c_handle->cap_id_default,
                            c_handle->cap_id_cur,
                            c_handle->cap_id_count,
                            c_handle->total_compen_samples,
                            c_handle->compen_count,
                            c_handle->compen_method,
                            sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.data_timestamp,
                            bt_clk,
                            gaming_mode_music_ccni_gpt_count,
                            gaming_mode_music_bt_out_gpt_count);

            #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
            extern volatile uint32_t celt_enc_finish_gpt_count;
            DSP_MW_LOG_I("[bt common][sink][flush_postprocess]: handle 0x%x: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x", 9,
                            c_handle,
                            gaming_mode_music_ccni_gpt_count,
                            c_handle->data_in_gpt_count,
                            c_handle->gain_port->finish_gpt_count,
                            c_handle->buffer_port->finish_gpt_count,
                            c_handle->clk_skew_port->finish_gpt_count,
                            c_handle->mixer_member->finish_gpt_count,
                            celt_enc_finish_gpt_count,
                            gaming_mode_music_bt_out_gpt_count);
            #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */

            /* sync sink settings */
            if (source->transform->sink != sink)
            {
                /* set current sink's settings into other sinks */
                source->transform->sink->param.bt_ul.current_notification_index                                         = sink->param.bt_ul.current_notification_index;
                source->transform->sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.data_timestamp  = sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.data_timestamp;
                source->transform->sink->param.bt_ul.seq_num                                                            = sink->param.bt_ul.seq_num;
                source->transform->sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_timestamp = sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_timestamp;
                source->transform->sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_status    = sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_status;
                source->transform->sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_failcount = sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.event_failcount;
            }
        }

        #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
        #if GAMING_MODE_MUSIC_DONGLE_DEBUG_DUMP
        uint32_t j, address, read_offset, total_buffer_size;
        total_buffer_size = source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_num;
        for (j=0; j < c_handle->process_frames; j++)
        {
            read_offset = (c_handle->read_offset + source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * j) % total_buffer_size;
            address = source->streamBuffer.ShareBufferInfo.startaddr + read_offset + sizeof(audio_transmitter_frame_header_t);
            /* dump source raw data */
            LOG_AUDIO_DUMP( (uint8_t *)address,
                            source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size - sizeof(audio_transmitter_frame_header_t),
                            SOURCE_IN2+(source->param.data_dl.scenario_sub_id - AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0));
        }
        #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_DUMP */
        #endif /* MTK_AUDIO_DUMP_BY_CONFIGTOOL */

        #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
        if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) && (source->param.data_dl.current_notification_index != 0) && (c_handle->process_frames != 0))
        {
            gaming_mode_vol_buffer_msg.data_buf_1_cur  = (uint16_t *)stream_function_sw_buffer_get_channel_read_pointer(c_handle->buffer_port, 1);
            gaming_mode_vol_buffer_msg.data_buf_1_head = (uint16_t *)stream_function_sw_buffer_get_channel_start_pointer(c_handle->buffer_port, 1);
            gaming_mode_vol_buffer_msg.data_buf_1_tail = (uint16_t *)stream_function_sw_buffer_get_channel_end_pointer(c_handle->buffer_port, 1);
            gaming_mode_vol_buffer_msg.data_buf_2_cur  = (uint16_t *)stream_function_sw_buffer_get_channel_read_pointer(c_handle->buffer_port, 2);
            gaming_mode_vol_buffer_msg.data_buf_2_head = (uint16_t *)stream_function_sw_buffer_get_channel_start_pointer(c_handle->buffer_port, 2);
            gaming_mode_vol_buffer_msg.data_buf_2_tail = (uint16_t *)stream_function_sw_buffer_get_channel_end_pointer(c_handle->buffer_port, 2);
            DTM_enqueue(DTM_EVENT_ID_GAMING_MODE_VOLUME_SMART_BALANCE_PROCESS, (uint32_t)&gaming_mode_vol_buffer_msg, false);
        }
        else if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) && (source->param.data_dl.current_notification_index != 0) && (c_handle->process_frames == 0))
        {
            gaming_mode_vol_buffer_msg.data_buf_1_cur = NULL;
            gaming_mode_vol_buffer_msg.data_buf_2_cur = NULL;
            DTM_enqueue(DTM_EVENT_ID_GAMING_MODE_VOLUME_SMART_BALANCE_PROCESS, (uint32_t)&gaming_mode_vol_buffer_msg, false);
        }
        #endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

        /* switch to the next dl stream */
        c_handle = c_handle->next_dl_handle;
    }

    #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[bt common][sink][flush_postprocess]: %u, %d", 2, current_timestamp, hal_nvic_query_exception_number());
    #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */
}

bool gaming_mode_dongle_dl_sink_close(SINK sink)
{
    extern gaming_mode_dongle_dl_handle_t *gaming_mode_dongle_first_dl_handle;
    SOURCE source;
    uint32_t saved_mask = 0;

    if (sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.codec_type == AUDIO_DSP_CODEC_TYPE_OPUS)
    {
        #ifdef AIR_CELT_ENC_ENABLE
        stream_codec_encoder_celt_deinit();
        #endif /* AIR_CELT_ENC_ENABLE */
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (gaming_mode_dongle_sink == sink)
    {
        if (gaming_mode_dongle_first_dl_handle == NULL)
        {
            gaming_mode_dongle_sink = NULL;
        }
        else
        {
            source = (SOURCE)(gaming_mode_dongle_first_dl_handle->owner);
            gaming_mode_dongle_sink = source->transform->sink;
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    #if GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[bt common][sink][sink_close]: %u, %d", 2, current_timestamp, hal_nvic_query_exception_number());
    #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LOG */

    return true;
}
/******************************************************************************/
/*                  Gaming mode dongle voice path functions                   */
/******************************************************************************/
gaming_mode_dongle_ul_handle_t *gaming_mode_dongle_get_ul_handle(void *owner)
{
    uint32_t saved_mask;
    int16_t count;
    gaming_mode_dongle_ul_handle_t *dongle_handle = NULL;
    gaming_mode_dongle_ul_handle_t *c_handle = NULL;

    dongle_handle = malloc(sizeof(gaming_mode_dongle_ul_handle_t));
    if (dongle_handle == NULL)
    {
        configASSERT(0);
    }
    memset(dongle_handle, 0, sizeof(gaming_mode_dongle_ul_handle_t));

    dongle_handle->total_number = -1;
    dongle_handle->index = -1;
    dongle_handle->owner = owner;
    dongle_handle->next_ul_handle = NULL;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if (gaming_mode_dongle_first_ul_handle == NULL)
    {
        /* there is no item on the handle list */
        dongle_handle->total_number = 1;
        dongle_handle->index = 1;

        /* set this handle as the head of the handle list */
        gaming_mode_dongle_first_ul_handle = dongle_handle;
    }
    else
    {
        /* there are other items on the handle list */
        count = 1;
        c_handle = gaming_mode_dongle_first_ul_handle;
        while (c_handle != NULL)
        {
            count += 1;

            c_handle->total_number += 1;
            if (c_handle->total_number <= 0)
            {
                /* error status */
                configASSERT(0);
            }

            if (c_handle->next_ul_handle == NULL)
            {
                /* c_handle is the last item on the list now */
                dongle_handle->total_number = c_handle->total_number;
                dongle_handle->index = c_handle->total_number;

                /* dongle_handle is the last item on the list now */
                c_handle->next_ul_handle = dongle_handle;

                break;
            }

            c_handle = c_handle->next_ul_handle;
        }
        if ((c_handle == NULL) || (dongle_handle->total_number != count))
        {
            configASSERT(0);
        }
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    return dongle_handle;
}

void gaming_mode_dongle_release_ul_handle(gaming_mode_dongle_ul_handle_t *handle)
{
    uint32_t saved_mask, i, count;
    gaming_mode_dongle_ul_handle_t *l_handle = NULL;
    gaming_mode_dongle_ul_handle_t *c_handle = NULL;
    gaming_mode_dongle_ul_handle_t *dongle_handle = NULL;

    if ((handle == NULL) || (gaming_mode_dongle_first_ul_handle == NULL))
    {
        configASSERT(0);
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if (gaming_mode_dongle_first_ul_handle->total_number <= 0)
    {
        /* error status */
        configASSERT(0);
    }
    else if ((gaming_mode_dongle_first_ul_handle->total_number == 1) &&
            (gaming_mode_dongle_first_ul_handle == handle))
    {
        /* this handle is only one item on handle list */
        if (gaming_mode_dongle_first_ul_handle->next_ul_handle != NULL)
        {
            configASSERT(0);
        }

        /* there is no item on the handle list */
        gaming_mode_dongle_first_ul_handle = NULL;
    }
    else if ((gaming_mode_dongle_first_ul_handle->total_number > 1) &&
            (gaming_mode_dongle_first_ul_handle == handle))
    {
        /* this handle is the first item on handle list, but there are other handles on the list */
        c_handle = gaming_mode_dongle_first_ul_handle;
        count = gaming_mode_dongle_first_ul_handle->total_number;
        for (i=0; i < count; i++)
        {
            c_handle->total_number -= 1;
            c_handle->index -= 1;

            c_handle = c_handle->next_ul_handle;
        }
        if (c_handle != NULL)
        {
            /* error status */
            configASSERT(0);
        }

        /* change the next iten to the head */
        gaming_mode_dongle_first_ul_handle = handle->next_ul_handle;
    }
    else
    {
        /* this handle is the middle item on handle list */
        c_handle = gaming_mode_dongle_first_ul_handle;
        count = gaming_mode_dongle_first_ul_handle->total_number;
        if (count == 1)
        {
            /* error status */
            configASSERT(0);
        }
        for (i=0; i < count; i++)
        {
            if (c_handle == handle)
            {
                /* find out the handle on the list */
                dongle_handle = handle;
                l_handle->next_ul_handle = c_handle->next_ul_handle;
            }

            if (dongle_handle == NULL)
            {
                c_handle->total_number -= 1;
            }
            else
            {
                c_handle->total_number -= 1;
                c_handle->index -= 1;
            }

            l_handle = c_handle;
            c_handle = c_handle->next_ul_handle;
        }
        if ((c_handle != NULL) || (dongle_handle == NULL))
        {
            /* error status */
            configASSERT(0);
        }
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    free(handle);
}

void gaming_mode_dongle_ul_init(SOURCE source, SINK sink, audio_transmitter_open_param_p audio_transmitter_open_param, bt_common_open_param_p bt_common_open_param)
{
    gaming_mode_dongle_ul_handle_t *dongle_handle;
    UNUSED(sink);

    /* register ccni handler */
    bt_common_register_isr_handler(gaming_mode_dongle_ul_ccni_handler);

    /* get handle for application config */
    dongle_handle = gaming_mode_dongle_get_ul_handle(source);
    source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.handle = (void *)dongle_handle;
    /* latency default is 40ms */
    dongle_handle->latency_us = 40000;

    #ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
    sw_clk_skew_config_t sw_clk_skew_config;
    /* clock skew config */
    dongle_handle->compen_samples = 0;
    dongle_handle->clk_skew_count = 0;
    dongle_handle->clk_skew_port = stream_function_sw_clk_skew_get_port(source);
    sw_clk_skew_config.channel = 2;
    sw_clk_skew_config.bits = 16;
    sw_clk_skew_config.order = C_Flp_Ord_5;
    sw_clk_skew_config.skew_io_mode = C_Skew_Oup;
    sw_clk_skew_config.skew_compensation_mode = SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_8_FRAME;
    sw_clk_skew_config.skew_work_mode = SW_CLK_SKEW_CONTINUOUS;
    sw_clk_skew_config.max_output_size = 15*16*sizeof(int16_t); /* 15ms/16K buffer */
    sw_clk_skew_config.continuous_frame_size = 120*sizeof(int16_t); /* fixed 7.5ms/16K input */
    stream_function_sw_clk_skew_init(dongle_handle->clk_skew_port, &sw_clk_skew_config);
    #endif /* AIR_SOFTWARE_CLK_SKEW_ENABLE */

    #ifdef AIR_SOFTWARE_BUFFER_ENABLE
    sw_buffer_config_t buffer_config;
    #ifdef AIR_GAMING_MODE_DONGLE_ECNR_ENABLE
    /* init sw buffer list */
    dongle_handle->ecnr_buffer_list = stream_function_sw_buffer_get_list(source);
    stream_function_sw_buffer_list_init(dongle_handle->ecnr_buffer_list, 3);

    /* config the first buffer */
    buffer_config.mode = SW_BUFFER_MODE_MULTI_BUFFERS;
    buffer_config.total_channels = 2;
    buffer_config.watermark_max_size = 240*sizeof(int16_t);
    buffer_config.watermark_min_size = 0;
    buffer_config.output_size = 240*sizeof(int16_t); /* Output with 15ms */
    dongle_handle->ecnr_in_buffer_port = stream_function_sw_buffer_get_unused_port(source);
    stream_function_sw_buffer_init(dongle_handle->ecnr_in_buffer_port, &buffer_config);
    /* Prefill 12.5ms for ECNR process in the 1st time */
    stream_function_sw_buffer_config_channel_prefill_size(dongle_handle->ecnr_in_buffer_port, 1, 120*sizeof(int16_t), true);
    stream_function_sw_buffer_config_channel_prefill_size(dongle_handle->ecnr_in_buffer_port, 2, 120*sizeof(int16_t), true);
    stream_function_sw_buffer_list_insert_buffer(dongle_handle->ecnr_buffer_list, dongle_handle->ecnr_in_buffer_port, 0);

    /* config the second buffer */
    buffer_config.mode = SW_BUFFER_MODE_MULTI_BUFFERS;
    buffer_config.total_channels = 2;
    buffer_config.watermark_max_size = 360*sizeof(int16_t);  /* must be 2.5ms*N */
    buffer_config.watermark_min_size = 0;
    buffer_config.output_size = 120*sizeof(int16_t);
    dongle_handle->ecnr_out_buffer_port = stream_function_sw_buffer_get_unused_port(source);
    stream_function_sw_buffer_init(dongle_handle->ecnr_out_buffer_port, &buffer_config);
    stream_function_sw_buffer_list_insert_buffer(dongle_handle->ecnr_buffer_list, dongle_handle->ecnr_out_buffer_port, 1);

    /* config the third buffer */
    buffer_config.mode = SW_BUFFER_MODE_MULTI_BUFFERS;
    buffer_config.total_channels = 2;
    buffer_config.watermark_max_size = 2400*sizeof(int16_t); /* 150ms * 16K */
    buffer_config.watermark_min_size = 0;
    buffer_config.output_size = 120*sizeof(int16_t);
    dongle_handle->buffer_port = stream_function_sw_buffer_get_unused_port(source);
    stream_function_sw_buffer_init(dongle_handle->buffer_port, &buffer_config);
    /* set default 15ms audio buffer */
    dongle_handle->buffer_watermark = 240;
    stream_function_sw_buffer_list_insert_buffer(dongle_handle->ecnr_buffer_list, dongle_handle->buffer_port, 2);
    #else
    buffer_config.mode = SW_BUFFER_MODE_FIXED_OUTPUT_LENGTH;
    buffer_config.total_channels = 2;
    buffer_config.watermark_max_size = 2400*sizeof(int16_t); /* 150ms * 16K */
    buffer_config.watermark_min_size = 0;
    buffer_config.output_size = 120*sizeof(int16_t); /* 7.5ms * 16K */
    dongle_handle->buffer_port = stream_function_sw_buffer_get_port(source);
    stream_function_sw_buffer_init(dongle_handle->buffer_port, &buffer_config);
    /* set default 15ms audio buffer */
    dongle_handle->buffer_watermark = 240;
    #endif /* AIR_GAMING_MODE_DONGLE_ECNR_ENABLE */
    #endif /* AIR_SOFTWARE_BUFFER_ENABLE */

    #ifdef AIR_SOFTWARE_SRC_ENABLE
    sw_src_config_t sw_src_config;
    sw_src_config.mode = SW_SRC_MODE_NORMAL;
    sw_src_config.channel_num = 1;
    sw_src_config.in_res = RESOLUTION_16BIT;
    sw_src_config.in_sampling_rate  = 16000;
    sw_src_config.in_frame_size_max = GAMING_MODE_VOICE_USB_FRAME_SAMPLES*sizeof(int16_t);
    sw_src_config.out_res           = RESOLUTION_16BIT;
    sw_src_config.out_sampling_rate = audio_transmitter_open_param->scenario_param.gaming_mode_param.codec_param.pcm.sample_rate;
    sw_src_config.out_frame_size_max= audio_transmitter_open_param->scenario_param.gaming_mode_param.codec_param.pcm.sample_rate/16000*GAMING_MODE_VOICE_USB_FRAME_SAMPLES*sizeof(int16_t);
    dongle_handle->src_port = stream_function_sw_src_get_port(source);
    stream_function_sw_src_init(dongle_handle->src_port, &sw_src_config);
    DSP_MW_LOG_I("[bt common][config][SW_SRC][scenario %d-%d] %u, %u, %u, %u, %u, %u\r\n", 8,
                audio_transmitter_open_param->scenario_type,
                audio_transmitter_open_param->scenario_sub_id,
                sw_src_config.in_res,
                sw_src_config.in_sampling_rate,
                sw_src_config.in_frame_size_max,
                sw_src_config.out_res,
                sw_src_config.out_sampling_rate,
                sw_src_config.out_frame_size_max);
    #endif /* AIR_SOFTWARE_SRC_ENABLE */

    #ifdef AIR_SOFTWARE_GAIN_ENABLE
    /* configuare sw gain */
    int32_t default_gain;
    sw_gain_config_t default_config;
    default_config.resolution = RESOLUTION_16BIT;
    default_config.target_gain = bt_common_open_param->scenario_param.gaming_mode_param.gain_default_L;
    default_config.up_step = 25;
    default_config.up_samples_per_step = 16;
    default_config.down_step = -25;
    default_config.down_samples_per_step = 16;
    dongle_handle->gain_port = stream_function_sw_gain_get_port(source);
    stream_function_sw_gain_init(dongle_handle->gain_port, 2, &default_config);
    default_gain = bt_common_open_param->scenario_param.gaming_mode_param.gain_default_L;
    stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 1, default_gain);
    DSP_MW_LOG_I("[bt common][config]change channel %d gain to %d*0.01dB\r\n", 2,
                1,
                default_gain);
    default_gain = bt_common_open_param->scenario_param.gaming_mode_param.gain_default_R;
    stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 2, default_gain);
    DSP_MW_LOG_I("[bt common][config]change channel %d gain to %d*0.01dB\r\n", 2,
                2,
                default_gain);
    #endif /* AIR_SOFTWARE_GAIN_ENABLE */

    #ifdef MTK_CELT_DEC_ENABLE
    if (bt_common_open_param->scenario_param.gaming_mode_param.codec_param.opus.bit_rate == 32000)
    {
        stream_codec_decoder_celt_set_input_frame_size(30);
        DSP_MW_LOG_I("[bt common][config] codec frame size, %u\r\n", 1, 30);
    }
    else if (bt_common_open_param->scenario_param.gaming_mode_param.codec_param.opus.bit_rate == 50133)
    {
        stream_codec_decoder_celt_set_input_frame_size(47);
        DSP_MW_LOG_I("[bt common][config] codec frame size, %u\r\n", 1, 47);
    }
    else
    {
        DSP_MW_LOG_E("[bt common][config] error bitrate, %u\r\n", 1, bt_common_open_param->scenario_param.gaming_mode_param.codec_param.opus.bit_rate);
        configASSERT(0);
    }
    #endif /* MTK_CELT_DEC_ENABLE */

    /* lock sleep because sleep wake-up time will consume the stream process time */
    hal_sleep_manager_lock_sleep(SLEEP_LOCK_AUDIO);
}

void gaming_mode_dongle_ul_deinit(SOURCE source, SINK sink)
{
    gaming_mode_dongle_ul_handle_t *dongle_handle = (gaming_mode_dongle_ul_handle_t *)(source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.handle);
    UNUSED(sink);

    /* unregister ccni handler */
    bt_common_unregister_isr_handler(gaming_mode_dongle_ul_ccni_handler);

    /* application deinit */
    #ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
    stream_function_sw_clk_skew_deinit(dongle_handle->clk_skew_port);
    #endif /* AIR_SOFTWARE_CLK_SKEW_ENABLE */

    #ifdef AIR_SOFTWARE_BUFFER_ENABLE
    #ifdef AIR_GAMING_MODE_DONGLE_ECNR_ENABLE
        stream_function_sw_buffer_deinit(dongle_handle->buffer_port);
        stream_function_sw_buffer_deinit(dongle_handle->ecnr_in_buffer_port);
        stream_function_sw_buffer_deinit(dongle_handle->ecnr_out_buffer_port);
        stream_function_sw_buffer_list_deinit(dongle_handle->ecnr_buffer_list);
    #else
        stream_function_sw_buffer_deinit(dongle_handle->buffer_port);
    #endif /* AIR_GAMING_MODE_DONGLE_ECNR_ENABLE */
    #endif /* AIR_SOFTWARE_BUFFER_ENABLE */

    #ifdef AIR_SOFTWARE_SRC_ENABLE
    stream_function_sw_src_deinit(dongle_handle->src_port);
    #endif /* AIR_SOFTWARE_SRC_ENABLE */

    #ifdef AIR_SOFTWARE_GAIN_ENABLE
    stream_function_sw_gain_deinit(dongle_handle->gain_port);
    #endif /* AIR_SOFTWARE_GAIN_ENABLE */

    /* release handle */
    gaming_mode_dongle_release_ul_handle(dongle_handle);
    source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.handle = NULL;

    /* unlock sleep */
    hal_sleep_manager_unlock_sleep(SLEEP_LOCK_AUDIO);
}

void gaming_mode_dongle_ul_start(SOURCE source)
{
    uint32_t saved_mask = 0;
    gaming_mode_dongle_ul_handle_t *dongle_handle = (gaming_mode_dongle_ul_handle_t *)(source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.handle);

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    dongle_handle->fetch_flag = 0;
    dongle_handle->play_en_status = 0;
    dongle_handle->stream_status = GAMING_MODE_UL_STREAM_INIT;
    dongle_handle->frame_count = 0;
    dongle_handle->frame_index = 1;
    dongle_handle->compen_samples = 0;

    hal_nvic_restore_interrupt_mask(saved_mask);
}

ATTR_TEXT_IN_IRAM_LEVEL_1 void gaming_mode_dongle_ul_ccni_handler(hal_ccni_event_t event, void * msg)
{
    gaming_mode_dongle_ul_handle_t *dongle_handle = gaming_mode_dongle_first_ul_handle;
    SOURCE source;
    uint32_t saved_mask;
    UNUSED(event);
    UNUSED(msg);

    if (dongle_handle == NULL)
    {
        DSP_MW_LOG_E("[bt common][source]first_ul_handle is NULL\r\n", 0);
        goto _ccni_return;
    }

    source = (SOURCE)(dongle_handle->owner);
    if ((source == NULL) || (source->transform == NULL))
    {
        DSP_MW_LOG_E("[bt common][source]source or transform is NULL\r\n", 0);
        goto _ccni_return;
    }

    if (dongle_handle->stream_status != GAMING_MODE_UL_STREAM_DEINIT)
    {
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        if (dongle_handle->fetch_flag == 0)
        {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &dongle_handle->data_in_gpt_count);
        }
        /* set fetch flag to trigger stream flow */
        dongle_handle->fetch_flag += 1;
        hal_nvic_restore_interrupt_mask(saved_mask);

        /* Handler the stream */
        AudioCheckTransformHandle(source->transform);
    }

_ccni_return:
    return;
}

bool gaming_mode_dongle_ul_config(SOURCE source, stream_config_type type, U32 value)
{
    uint32_t saved_mask;
    gaming_mode_runtime_config_param_p config_param = (gaming_mode_runtime_config_param_p)value;
    gaming_mode_dongle_ul_handle_t *dongle_handle = (gaming_mode_dongle_ul_handle_t *)(source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.handle);
    UNUSED(type);
    UNUSED(value);
    #ifdef AIR_SOFTWARE_GAIN_ENABLE
    int32_t new_gain;
    sw_gain_config_t old_config;
    #endif /* AIR_SOFTWARE_GAIN_ENABLE */
    UNUSED(dongle_handle);

    switch (config_param->config_operation)
    {
        case GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_L:
            #ifdef AIR_SOFTWARE_GAIN_ENABLE
            /* left channel */
            new_gain = config_param->config_param.vol_level.gain_1;
            stream_function_sw_gain_get_config(dongle_handle->gain_port, 1, &old_config);
            DSP_MW_LOG_I("[bt common][config]change channel%d gain from %d*0.01dB to %d*0.01dB\r\n", 3,
                        1,
                        old_config.target_gain,
                        new_gain);
            stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 1, new_gain);
            #endif /* AIR_SOFTWARE_GAIN_ENABLE */
            break;

        case GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_R:
            #ifdef AIR_SOFTWARE_GAIN_ENABLE
            /* right channel */
            new_gain = config_param->config_param.vol_level.gain_2;
            stream_function_sw_gain_get_config(dongle_handle->gain_port, 2, &old_config);
            DSP_MW_LOG_I("[bt common][config]change channel%d gain from %d*0.01dB to %d*0.01dB\r\n", 3,
                        2,
                        old_config.target_gain,
                        new_gain);
            stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 2, new_gain);
            #endif /* AIR_SOFTWARE_GAIN_ENABLE */
            break;

        case GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_DUL:
            #ifdef AIR_SOFTWARE_GAIN_ENABLE
            /* left channel */
            new_gain = config_param->config_param.vol_level.gain_1;
            stream_function_sw_gain_get_config(dongle_handle->gain_port, 1, &old_config);
            DSP_MW_LOG_I("[bt common][config]change channel%d gain from %d*0.01dB to %d*0.01dB\r\n", 3,
                        1,
                        old_config.target_gain,
                        new_gain);
            stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 1, new_gain);

            /* right channel */
            new_gain = config_param->config_param.vol_level.gain_2;
            stream_function_sw_gain_get_config(dongle_handle->gain_port, 2, &old_config);
            DSP_MW_LOG_I("[bt common][config]change channel%d gain from %d*0.01dB to %d*0.01dB\r\n", 3,
                        2,
                        old_config.target_gain,
                        new_gain);
            stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 2, new_gain);
            #endif /* AIR_SOFTWARE_GAIN_ENABLE */
            break;

        case GAMING_MODE_CONFIG_OP_LATENCY_SWITCH:
            if (config_param->config_param.latency_param.latency_us == 25000)
            {
                /* 15ms audio buffer */
                hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                dongle_handle->buffer_watermark = 200;
                hal_nvic_restore_interrupt_mask(saved_mask);
            }
            else if (config_param->config_param.latency_param.latency_us == 40000)
            {
                /* 15ms audio buffer */
                hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                dongle_handle->buffer_watermark = 240;
                hal_nvic_restore_interrupt_mask(saved_mask);
            }
            else if (config_param->config_param.latency_param.latency_us == 60000)
            {
                /* 60ms audio buffer */
                hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                dongle_handle->buffer_watermark = 960;
                hal_nvic_restore_interrupt_mask(saved_mask);
            }
            else
            {
                DSP_MW_LOG_E("[bt common][config] unknow latency:%d\r\n", 1, config_param->config_param.latency_param.latency_us);
                break;
            }

            /* switch latency */
            DSP_MW_LOG_I("[bt common][config]latency switch from %d us to %d us\r\n", 2,
                        dongle_handle->latency_us,
                        config_param->config_param.latency_param.latency_us);
            dongle_handle->latency_us = config_param->config_param.latency_param.latency_us;
            break;

        default:
            DSP_MW_LOG_E("[bt common][config] unknow cmd:%d\r\n", 1, config_param->config_operation);
            break;
    }

    return true;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 bool gaming_mode_dongle_ul_source_get_avail_size(SOURCE source, uint32_t *avail_size)
{
    gaming_mode_dongle_ul_handle_t *dongle_handle = (gaming_mode_dongle_ul_handle_t *)(source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.handle);

    if (dongle_handle->fetch_flag != 0)
    {
        /* If there is fetch requirement, we must wake up the stream even there is no packet on the share buffer */
        *avail_size = source->streamBuffer.AVMBufferInfo.MemBlkSize;
    }
    else if ((source->streamBuffer.AVMBufferInfo.ReadIndex != source->streamBuffer.AVMBufferInfo.WriteIndex) &&
            (dongle_handle->stream_status == GAMING_MODE_UL_STREAM_RUNNING))
    {
        /* If there is data in the share buffer, we must wake up the stream to process it */
        *avail_size = source->streamBuffer.AVMBufferInfo.MemBlkSize;
    }
    else
    {
        *avail_size = 0;
    }

    if (*avail_size != 0)
    {
        /* change avail_size to actual frame size */
        *avail_size = source->streamBuffer.AVMBufferInfo.MemBlkSize - sizeof(bt_common_gaming_mode_voice_frame_header_t) - 1;

        /* update data receive timestamp */
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.data_timestamp);
    }

    #if GAMING_MODE_VOICE_DONGLE_DEBUG_LOG
    uint32_t current_timestamp;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[bt common][source][get_avail_size]: %u, %u, %u, %d, %u, %d", 6,
                source->param.bt_dl.seq_num,
                source->streamBuffer.AVMBufferInfo.ReadIndex,
                source->streamBuffer.AVMBufferInfo.WriteIndex,
                *avail_size,
                current_timestamp,
                hal_nvic_query_exception_number());
    #endif /* GAMING_MODE_VOICE_DONGLE_DEBUG_LOG */

    return true;
}

uint32_t gaming_mode_dongle_ul_source_copy_payload(SOURCE source, uint8_t *dst_buf, uint32_t length)
{
    uint8_t *src_buf = NULL;
    uint32_t saved_mask;
    uint32_t remain_samples = 0;
    uint8_t seq_num = 0;
    uint16_t check_sum = 0;
    bt_common_gaming_mode_voice_frame_header_t frame_header;
    DSP_STREAMING_PARA_PTR stream = NULL;
    uint32_t total_frames = 0;
    gaming_mode_dongle_ul_first_packet_status_t first_packet_status;
    uint32_t bt_clk;
    uint16_t bt_phase;
    gaming_mode_dongle_ul_handle_t *dongle_handle = (gaming_mode_dongle_ul_handle_t *)(source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.handle);
    int32_t compensatory_samples = 0;
    uint16_t frame_index;

    extern void bt_common_share_information_fetch(SOURCE source, SINK sink);
    extern void bt_common_share_information_update_read_offset(SOURCE source, U32 ReadOffset);
    extern VOID MCE_GetBtClk(BTCLK* pCurrCLK, BTPHASE* pCurrPhase,BT_CLOCK_OFFSET_SCENARIO type);

    /* dongle status check */
    switch (dongle_handle->stream_status)
    {
        /* In this status stage, it means the stream is not ready */
        case GAMING_MODE_UL_STREAM_DEINIT:
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            dongle_handle->fetch_flag = 0;
            hal_nvic_restore_interrupt_mask(saved_mask);
            break;

        /* In this status stage, we will update ReadIndex to the lastest WriteIndex and the old packets are dropped */
        case GAMING_MODE_UL_STREAM_INIT:
            /* get the latest share buffer info */
            bt_common_share_information_fetch(source, NULL);

            /* get the total frames in the buffer */
            total_frames = (source->streamBuffer.AVMBufferInfo.WriteIndex +
                            source->streamBuffer.AVMBufferInfo.MemBlkNum -
                            source->streamBuffer.AVMBufferInfo.ReadIndex) % source->streamBuffer.AVMBufferInfo.MemBlkNum;

            /* check if there is packet in the share buffer */
            if (total_frames != 0)
            {
                /* set WriteIndex as the new ReadIndex at the first time, it will drop the all data in the share buffer */
                bt_common_share_information_update_read_offset(source, source->streamBuffer.AVMBufferInfo.WriteIndex);

                /* update stream status */
                dongle_handle->stream_status = GAMING_MODE_UL_STREAM_START;

                /* in here, it means there is at least 1 packet in share buffer at the first time */
                DSP_MW_LOG_I("[bt common][source]: there is new data in share buffer, seq_num = %u, r_index = %u, w_index = %u, status = %u", 4,
                            source->param.bt_dl.seq_num,
                            source->streamBuffer.AVMBufferInfo.ReadIndex,
                            source->streamBuffer.AVMBufferInfo.WriteIndex,
                            dongle_handle->stream_status);
            }

            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            dongle_handle->fetch_flag = 0;
            hal_nvic_restore_interrupt_mask(saved_mask);
            break;

        /* In this status stage, we will check if the newest packet is suitable for playing */
        case GAMING_MODE_UL_STREAM_START:
            /* get the latest share buffer info */
            bt_common_share_information_fetch(source, NULL);

            /* get the total frames in the buffer */
            total_frames = (source->streamBuffer.AVMBufferInfo.WriteIndex +
                            source->streamBuffer.AVMBufferInfo.MemBlkNum -
                            source->streamBuffer.AVMBufferInfo.ReadIndex) % source->streamBuffer.AVMBufferInfo.MemBlkNum;

            /* check if this packet is suitable for playing */
            if (total_frames != 0)
            {
                /* get the first packet's payload header */
                src_buf = (uint8_t *)(source->streamBuffer.AVMBufferInfo.StartAddr
                                    + source->streamBuffer.AVMBufferInfo.ReadIndex * source->streamBuffer.AVMBufferInfo.MemBlkSize);
                memcpy(&frame_header, src_buf, sizeof(bt_common_gaming_mode_voice_frame_header_t));

                /* check if the first packet is suitable */
                /* the anchor time in frame is larger 24 than the true bt clock that the packet sent time */
                frame_header.anchor = (frame_header.anchor+0x0fffffff+1-24)&0x0fffffff;
                first_packet_status = gaming_mode_dongle_ul_first_packet_check(dongle_handle, frame_header.anchor);
                switch (first_packet_status)
                {
                    case GAMING_MODE_DONGLE_UL_FIRST_PACKET_NOT_READY:
                        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                        dongle_handle->fetch_flag = 0;
                        hal_nvic_restore_interrupt_mask(saved_mask);
                        break;

                    case GAMING_MODE_DONGLE_UL_FIRST_PACKET_TIMEOUT:
                        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                        dongle_handle->fetch_flag = 0;
                        hal_nvic_restore_interrupt_mask(saved_mask);

                        /* drop this packet */
                        source->streamBuffer.AVMBufferInfo.ReadIndex = (source->streamBuffer.AVMBufferInfo.ReadIndex + 1) % source->streamBuffer.AVMBufferInfo.MemBlkNum;
                        bt_common_share_information_update_read_offset(source, source->streamBuffer.AVMBufferInfo.ReadIndex);
                        DSP_MW_LOG_I("[bt common][source]: this first packet is dropped, seq_num = %u, r_index = %u, w_index = %u, status = %u", 4,
                                    source->param.bt_dl.seq_num,
                                    source->streamBuffer.AVMBufferInfo.ReadIndex,
                                    source->streamBuffer.AVMBufferInfo.WriteIndex,
                                    dongle_handle->stream_status);
                        break;

                    case GAMING_MODE_DONGLE_UL_FIRST_PACKET_READY:
                        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                        dongle_handle->fetch_flag = 0;
                        hal_nvic_restore_interrupt_mask(saved_mask);

                        /* update stream status */
                        dongle_handle->stream_status = GAMING_MODE_UL_STREAM_RUNNING;

                        /* set the seq num to the first packet's seq num */
                        src_buf = (uint8_t *)(source->streamBuffer.AVMBufferInfo.StartAddr
                                            + source->streamBuffer.AVMBufferInfo.ReadIndex * source->streamBuffer.AVMBufferInfo.MemBlkSize);
                        seq_num = *(src_buf + sizeof(bt_common_gaming_mode_voice_frame_header_t));
                        source->param.bt_dl.seq_num = seq_num;

                        /* get current bt clock */
                        MCE_GetBtClk((BTCLK*)&bt_clk, (BTPHASE*)&bt_phase, BT_CLK_Offset);

                        /* first_packet_bt_clk should be later than first_anchor_bt_clk */
                        dongle_handle->first_packet_bt_clk = bt_clk;
                        /* first_anchor_bt_clk is the packet sent time on headset/earbuds side */
                        dongle_handle->first_anchor_bt_clk = frame_header.anchor;
                        if ((dongle_handle->first_anchor_bt_clk + GAMING_MODE_VOICE_PLAYEN_DELAY) > 0x0fffffff)
                        {
                            dongle_handle->play_en_overflow = 1;
                        }
                        else
                        {
                            dongle_handle->play_en_overflow = 0;
                        }
                        /* play_en_bt_clk is the play time */
                        dongle_handle->play_en_bt_clk = (dongle_handle->first_anchor_bt_clk + GAMING_MODE_VOICE_PLAYEN_DELAY)&0x0fffffff;

                        DSP_MW_LOG_I("[bt common][source]: play will at 0x%x, first anchor = 0x%x, cur bt clk = 0x%x, seq_num = %u, r_index = %u, w_index = %u, status = %u", 7,
                                    dongle_handle->play_en_bt_clk,
                                    dongle_handle->first_anchor_bt_clk,
                                    bt_clk,
                                    source->param.bt_dl.seq_num,
                                    source->streamBuffer.AVMBufferInfo.ReadIndex,
                                    source->streamBuffer.AVMBufferInfo.WriteIndex,
                                    dongle_handle->stream_status);
                        break;

                    default:
                        configASSERT(0);
                        break;
                }
            }
            break;

        /* In this status stage, the stream is started and we will set flag to fetch a new packet */
        case GAMING_MODE_UL_STREAM_RUNNING:
            break;

        /* Error status */
        default:
            configASSERT(0);
            break;
    }

    /* check if the play is started */
    if ((dongle_handle->stream_status == GAMING_MODE_UL_STREAM_RUNNING) && (dongle_handle->play_en_status == 0) && gaming_mode_dongle_ul_check_play(dongle_handle))
    {
        dongle_handle->play_en_status = 1;

        /* get current bt clock */
        extern VOID MCE_GetBtClk(BTCLK* pCurrCLK, BTPHASE* pCurrPhase,BT_CLOCK_OFFSET_SCENARIO type);
        uint32_t bt_clk;
        uint16_t bt_phase;
        MCE_GetBtClk((BTCLK*)&bt_clk, (BTPHASE*)&bt_phase,BT_CLK_Offset);

        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        /* avoid fetch flag is too much here */
        dongle_handle->fetch_flag = 1;
        hal_nvic_restore_interrupt_mask(saved_mask);

        DSP_MW_LOG_I("[bt common][source]: play at now 0x%x, seq_num = %u, r_index = %u, w_index = %u, status = %u", 5,
                    bt_clk,
                    source->param.bt_dl.seq_num,
                    source->streamBuffer.AVMBufferInfo.ReadIndex,
                    source->streamBuffer.AVMBufferInfo.WriteIndex,
                    dongle_handle->stream_status);
    }

    /* process the frame */
    source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.drop_frames = 0;
    if (dongle_handle->stream_status == GAMING_MODE_UL_STREAM_RUNNING)
    {
        /* get the process frames in the buffer */
        total_frames = (source->streamBuffer.AVMBufferInfo.WriteIndex +
                        source->streamBuffer.AVMBufferInfo.MemBlkNum -
                        source->streamBuffer.AVMBufferInfo.ReadIndex) % source->streamBuffer.AVMBufferInfo.MemBlkNum;
        if (total_frames > 2)
        {
            /* in here, it means there is at least 2 packet in share buffer. */
            DSP_MW_LOG_I("[bt common][source]: there is too much data in share buffer, seq_num = %u, r_index = %u, w_index = %u", 3,
                        source->param.bt_dl.seq_num,
                        source->streamBuffer.AVMBufferInfo.ReadIndex,
                        source->streamBuffer.AVMBufferInfo.WriteIndex);

            /* get the new read index */
            source->streamBuffer.AVMBufferInfo.ReadIndex = (source->streamBuffer.AVMBufferInfo.WriteIndex +
                                                            source->streamBuffer.AVMBufferInfo.MemBlkNum -
                                                            2) % source->streamBuffer.AVMBufferInfo.MemBlkNum;

            /* set WriteIndex - 2 as the new ReadIndex at the first time, it will drop the other data in the share buffer */
            extern void bt_common_share_information_update_read_offset(SOURCE source, U32 ReadOffset);
            bt_common_share_information_update_read_offset(source, source->streamBuffer.AVMBufferInfo.ReadIndex);

            /* get the latest share buffer info */
            extern void bt_common_share_information_fetch(SOURCE source, SINK sink);
            bt_common_share_information_fetch(source, NULL);

            /* update seq_num and process_frames */
            src_buf = (uint8_t *)(source->streamBuffer.AVMBufferInfo.StartAddr +
                                source->streamBuffer.AVMBufferInfo.ReadIndex * source->streamBuffer.AVMBufferInfo.MemBlkSize);
            seq_num = *(src_buf + sizeof(bt_common_gaming_mode_voice_frame_header_t));
            source->param.bt_dl.seq_num = seq_num;

            /* set process_frames */
            source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.process_frames = 2;

            DSP_MW_LOG_I("[bt common][source]: change status, seq_num = %u, r_index = %u, w_index = %u", 3,
                        source->param.bt_dl.seq_num,
                        source->streamBuffer.AVMBufferInfo.ReadIndex,
                        source->streamBuffer.AVMBufferInfo.WriteIndex);
        }
        else if (total_frames == 0)
        {
            /* set process_frames */
            source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.process_frames = 0;
        }
        else
        {
            /* set process_frames */
            source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.process_frames = total_frames;
        }

        /* check copy data or do PLC or bypass the decoder */
        source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.drop_frames = 0;
#ifdef AIR_GAMING_MODE_DONGLE_ECNR_ENABLE
        remain_samples = stream_function_sw_buffer_get_channel_used_size(dongle_handle->buffer_port, 1) / sizeof(int16_t);
        if ((dongle_handle->play_en_status != 0) && (dongle_handle->fetch_flag != 0) && (remain_samples >= GAMING_MODE_VOICE_USB_FRAME_SAMPLES))
        {
            /* speical flow: output voice data into the share buffer firstly, then process uplink packet from air */
            dongle_handle->data_status = GAMING_MODE_DONGLE_UL_DATA_BYPASS_DECODER;
            goto _do_packet_process;
        }
#endif
        if (source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.process_frames == 0)
        {
            remain_samples = stream_function_sw_buffer_get_channel_used_size(dongle_handle->buffer_port, 1) / sizeof(int16_t);
            if (dongle_handle->fetch_flag != 0)
            {
                total_frames = 1;
            }
            else
            {
                total_frames = 0;
            }
            if(remain_samples >= GAMING_MODE_VOICE_USB_FRAME_SAMPLES*total_frames)
            {
                /* in this case, it means there is no new data but remain sample is enough, so we bypass the decoder */
                /* set flag to indicate that needs to bypass decoder */
                dongle_handle->data_status = GAMING_MODE_DONGLE_UL_DATA_BYPASS_DECODER;
            }
            else
            {
                /* in this case, it means there is no new data and remain sample is not enough, so we need to do PLC here to prevent buffer empty */
                /* set flag to indicate that there is the packet lost */
                dongle_handle->data_status = GAMING_MODE_DONGLE_UL_DATA_PLC;
                if (dongle_handle->play_en_status == 0)
                {
                    dongle_handle->data_status = GAMING_MODE_DONGLE_UL_DATA_BYPASS_DECODER;
                }
                source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.process_frames += 1;

                DSP_MW_LOG_I("[bt common][source]: The data packet is not on-time arrived, current seq_num = %u, status = %d",
                            2,
                            source->param.bt_dl.seq_num,
                            dongle_handle->data_status);

                if (dongle_handle->play_en_status != 0)
                {
                    source->param.bt_dl.seq_num = (source->param.bt_dl.seq_num+1) & 0xff;
                }
            }

            /* do not update read index */
            source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.drop_frames = 0;
        }
        else
        {
            /* in this case, it means there is new data (at most 2 packet) */
            /* find out suitable packet in this while loop */
            dongle_handle->data_status = GAMING_MODE_DONGLE_UL_DATA_EMPTY;
            while (dongle_handle->data_status == GAMING_MODE_DONGLE_UL_DATA_EMPTY)
            {
                /* get the frame base address */
                src_buf = (uint8_t *)(source->streamBuffer.AVMBufferInfo.StartAddr
                                    + source->streamBuffer.AVMBufferInfo.ReadIndex * source->streamBuffer.AVMBufferInfo.MemBlkSize);

                /* get payload header */
                memcpy(&frame_header, src_buf, sizeof(bt_common_gaming_mode_voice_frame_header_t));
                seq_num = *(src_buf + sizeof(bt_common_gaming_mode_voice_frame_header_t));

                /* frame check */
                if ((frame_header.frame_num != 1) ||
                    (frame_header.frame_size != length) ||
                    (frame_header.frame_size > (source->streamBuffer.AVMBufferInfo.MemBlkSize - sizeof(bt_common_gaming_mode_voice_frame_header_t) - 1)))
                {
                    /* in here, it means the frame is not right */
                    DSP_MW_LOG_E("[bt common][source]: The data packet frame is not right, read_index = %u, frame_num = %u, frame_size = %u, length = %u",
                                4,
                                source->streamBuffer.AVMBufferInfo.ReadIndex,
                                frame_header.frame_num,
                                frame_header.frame_size,
                                length);
                    /* set flag to indicate that there is the packet lost */
                    dongle_handle->data_status = GAMING_MODE_DONGLE_UL_DATA_PLC;
                    /* drop this packet */
                    source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.drop_frames += 1;
                    source->param.bt_dl.seq_num = (source->param.bt_dl.seq_num+1) & 0xff;
                    continue;
                    // configASSERT(0);
                }

                /* checksum check */
                check_sum = 0;
                for(; frame_header.frame_size > 0; frame_header.frame_size--)
                {
                    check_sum += *(src_buf + sizeof(bt_common_gaming_mode_voice_frame_header_t) + 1 + frame_header.frame_size - 1);
                }
                check_sum += seq_num;
                check_sum += (frame_header.anchor >>  0) & 0xff;
                check_sum += (frame_header.anchor >>  8) & 0xff;
                check_sum += (frame_header.anchor >> 16) & 0xff;
                check_sum += (frame_header.anchor >> 24) & 0xff;
                if(check_sum != frame_header.check_sum)
                {
                    /* in here, it means the data in this packet is not right */
                    DSP_MW_LOG_E("[bt common][source]: The data packet checksum is not right, read_index = %u, local check_sum = %u, frame check_sum = %u",
                                3,
                                source->streamBuffer.AVMBufferInfo.ReadIndex,
                                check_sum,
                                frame_header.check_sum);
                    /* set flag to indicate that there is the packet lost */
                    dongle_handle->data_status = GAMING_MODE_DONGLE_UL_DATA_PLC;
                    /* drop this packet */
                    source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.drop_frames += 1;
                    source->param.bt_dl.seq_num = (source->param.bt_dl.seq_num+1) & 0xff;
                    continue;
                    // configASSERT(0);
                }

                /* check if there is the packet lost */
                if(source->param.bt_dl.seq_num == seq_num)
                {
                    /* The data packet is continuous */
                    dongle_handle->data_status = GAMING_MODE_DONGLE_UL_DATA_NORMAL;
                    /* drop this packet */
                    source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.drop_frames += 1;
                }
                else if(((seq_num > source->param.bt_dl.seq_num) && ((seq_num - source->param.bt_dl.seq_num) <= 1)) ||
                        ((seq_num < source->param.bt_dl.seq_num) && ((seq_num + 256 - source->param.bt_dl.seq_num) <= 1)))
                {
                    /* The data packet is dropped endurably that at most 1 lost pakcets are allowed */
                    /* Just do decode here, do not PLC */
                    dongle_handle->data_status = GAMING_MODE_DONGLE_UL_DATA_NORMAL;
                    DSP_MW_LOG_E("[bt common][source]: The data packet is dropped, current seq_num = %u, latest seq_num = %u",
                                2,
                                source->param.bt_dl.seq_num,
                                seq_num);
                    /* set seq_num to the newest number for keeping data is continuous */
                    source->param.bt_dl.seq_num = seq_num;
                    /* drop this packet */
                    source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.drop_frames += 1;
                }
                else if(((seq_num > source->param.bt_dl.seq_num) && ((source->param.bt_dl.seq_num + 256 - seq_num) <= 1)) ||
                        ((seq_num < source->param.bt_dl.seq_num) && ((source->param.bt_dl.seq_num - seq_num) <= 1)))
                {
                    /* The data packet is replicated */
                    DSP_MW_LOG_E("[bt common][source]: The data packet is replicated, current seq_num = %u, latest seq_num = %u",
                                2,
                                source->param.bt_dl.seq_num,
                                seq_num);

                    /* get the next packet */
                    source->streamBuffer.AVMBufferInfo.ReadIndex = (source->streamBuffer.AVMBufferInfo.ReadIndex + 1) % source->streamBuffer.AVMBufferInfo.MemBlkNum;
                    source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.drop_frames += 1;

                    if (source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.process_frames > 1)
                    {
                        source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.process_frames -= 1;
                    }
                    else
                    {
                        remain_samples = stream_function_sw_buffer_get_channel_used_size(dongle_handle->buffer_port, 1) / sizeof(int16_t);
                        if (dongle_handle->fetch_flag != 0)
                        {
                            total_frames = 1;
                        }
                        else
                        {
                            total_frames = 0;
                        }
                        if(remain_samples >= GAMING_MODE_VOICE_USB_FRAME_SAMPLES*total_frames)
                        {
                            /* in this case, it means the only one packet is replicated but remain sample is enough, so we bypass the decoder */
                            /* set flag to indicate that needs to bypass */
                            dongle_handle->data_status = GAMING_MODE_DONGLE_UL_DATA_BYPASS_DECODER;
                            source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.process_frames -= 1;
                        }
                        else
                        {
                            /* in this case, it means only one packet is replicated and remain sample is not enough, so we need to do PLC here to prevent buffer empty */
                            /* set flag to indicate that there is the packet lost */
                            dongle_handle->data_status = GAMING_MODE_DONGLE_UL_DATA_PLC;
                            if (dongle_handle->play_en_status == 0)
                            {
                                dongle_handle->data_status = GAMING_MODE_DONGLE_UL_DATA_BYPASS_DECODER;
                            }
                        }

                        DSP_MW_LOG_E("[bt common][source]: The only one packet is replicated, %d", 1, dongle_handle->data_status);
                    }
                }
                else
                {
                    /* The data packet is dropped heavily */
                    /* Just do decode here, do not PLC */
                    dongle_handle->data_status = GAMING_MODE_DONGLE_UL_DATA_NORMAL;
                    DSP_MW_LOG_E("[bt common][source]: The data packet is dropped heavily, current seq_num = %u, latest seq_num = %u",
                                2,
                                source->param.bt_dl.seq_num,
                                seq_num);
                    // configASSERT(0);
                    /* set seq_num to the newest number for keeping data is continuous */
                    source->param.bt_dl.seq_num = seq_num;
                    /* drop this packet */
                    source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.drop_frames += 1;
                }
            }
        }

#ifdef AIR_GAMING_MODE_DONGLE_ECNR_ENABLE
_do_packet_process:
        /* Substact the gain byte */
        length -= 1;
#endif

        /* copy payload */
        if (dongle_handle->data_status == GAMING_MODE_DONGLE_UL_DATA_NORMAL)
        {
            /* The data packet is continuous or is dropped heavily */
            memcpy(dst_buf, src_buf + sizeof(bt_common_gaming_mode_voice_frame_header_t) + 1, length);

#ifdef AIR_GAMING_MODE_DONGLE_ECNR_ENABLE
            /* Get the Post EC gain from headset side */
            g_post_ec_gain = *(src_buf + sizeof(bt_common_gaming_mode_voice_frame_header_t) + 1 + length);
            DSP_MW_LOG_I("[AEC] receive PostEC_Gain %d, length %d", 2, g_post_ec_gain, length);
#endif

            /* set bt clock for latency debug */
            dongle_handle->bt_clk = frame_header.anchor;

            #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
            #if GAMING_MODE_VOICE_DONGLE_DEBUG_DUMP
            LOG_AUDIO_DUMP(dst_buf, length, VOICE_TX_MIC_0);
            #endif /* GAMING_MODE_VOICE_DONGLE_DEBUG_DUMP */
            #endif /* MTK_AUDIO_DUMP_BY_CONFIGTOOL */
        }
        else if (dongle_handle->data_status == GAMING_MODE_DONGLE_UL_DATA_BYPASS_DECODER)
        {
            /* there is drop requirment */
            *(dst_buf+0) = 0;
            *(dst_buf+1) = 0;
            *(dst_buf+2) = 0xa5;
            *(dst_buf+3) = 0xa5;

            /* set bt clock for latency debug */
            dongle_handle->bt_clk = 0;
        }
        else
        {
            /* there is the packet lost */
            /* set zero to the first four byte of the dst_buf to indicate there is the packet lost */
            *(dst_buf+0) = 0;
            *(dst_buf+1) = 0;
            *(dst_buf+2) = 0;
            *(dst_buf+3) = 0;

            /* set bt clock for latency debug */
            dongle_handle->bt_clk = 0;

            #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
            #if GAMING_MODE_VOICE_DONGLE_DEBUG_DUMP
            LOG_AUDIO_DUMP(dst_buf, length, VOICE_TX_MIC_0);
            #endif /* GAMING_MODE_VOICE_DONGLE_DEBUG_DUMP */
            #endif /* MTK_AUDIO_DUMP_BY_CONFIGTOOL */
        }

        /* update stream status */
        stream = DSP_Streaming_Get(source, source->transform->sink);
        stream->callback.EntryPara.in_size = length;
        stream->callback.EntryPara.in_channel_num = 1;

        #ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
        /* do clock skew if the play is started and there is new data */
        if ((dongle_handle->play_en_status != 0) && (dongle_handle->data_status != GAMING_MODE_DONGLE_UL_DATA_BYPASS_DECODER))
        {
            /* get remain samples */
            remain_samples = stream_function_sw_buffer_get_channel_used_size(dongle_handle->buffer_port, 1) / sizeof(int16_t);
            if (dongle_handle->fetch_flag != 0)
            {
                total_frames = 1;
                frame_index = dongle_handle->frame_index;
            }
            else
            {
                total_frames = 0;
                frame_index = (dongle_handle->frame_index+GAMING_MODE_VOICE_FRAME_INDEX_MAX-1)%GAMING_MODE_VOICE_FRAME_INDEX_MAX;
            }
            /* calculator compensatory samples */
            compensatory_samples = (int32_t)remain_samples+GAMING_MODE_VOICE_BT_FRAME_SAMPLES-GAMING_MODE_VOICE_USB_FRAME_SAMPLES*total_frames;
            dongle_handle->compen_samples = 0;
            if (compensatory_samples > (int32_t)(dongle_handle->buffer_watermark+(GAMING_MODE_VOICE_FRAME_INDEX_MAX-1-frame_index)*GAMING_MODE_VOICE_USB_FRAME_SAMPLES))
            {
                if (dongle_handle->clk_skew_count > -10)
                {
                    dongle_handle->clk_skew_count = dongle_handle->clk_skew_count - 1;
                    if ((dongle_handle->clk_skew_count > 0) && (dongle_handle->predicted_compen_times != 0))
                    {
                        dongle_handle->clk_skew_count = dongle_handle->clk_skew_count + 1;
                        dongle_handle->compen_samples = 1;
                        dongle_handle->predicted_compen_times -= 1;
                    }
                    else if (dongle_handle->clk_skew_count <= -10)
                    {
                        dongle_handle->predicted_compen_times = GAMING_MODE_VOICE_USB_FRAME_SAMPLES * dongle_handle->compensation_mode;
                    }
                }
                if (dongle_handle->clk_skew_count <= -10)
                {
                    /* we will do compensatory till the buffer water is dismatch */
                    dongle_handle->compen_samples = -1;
                    dongle_handle->predicted_compen_times -= 1;
                    if (dongle_handle->predicted_compen_times < 0)
                    {
                        dongle_handle->predicted_compen_count += 1;
                        if (dongle_handle->predicted_compen_count > 8)
                        {
                            /* in here, it means the current compensatroy mode is too slow, so we need to use the more aggressive compensatroy mode */
                            dongle_handle->compensation_mode = SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_1_FRAME;
                            stream_function_sw_clk_skew_configure_compensation_mode(dongle_handle->clk_skew_port, dongle_handle->compensation_mode);
                            dongle_handle->predicted_compen_times = compensatory_samples - (int32_t)(dongle_handle->buffer_watermark+(GAMING_MODE_VOICE_FRAME_INDEX_MAX-1-frame_index)*GAMING_MODE_VOICE_USB_FRAME_SAMPLES);
                        }
                        else
                        {
                            dongle_handle->compensation_mode = SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_8_FRAME;
                            stream_function_sw_clk_skew_configure_compensation_mode(dongle_handle->clk_skew_port, dongle_handle->compensation_mode);
                            dongle_handle->predicted_compen_times = (int32_t)(dongle_handle->buffer_watermark+(GAMING_MODE_VOICE_FRAME_INDEX_MAX-1-frame_index)*GAMING_MODE_VOICE_USB_FRAME_SAMPLES) - compensatory_samples;
                        }
                    }
                }
            }
            else if (compensatory_samples < (int32_t)(dongle_handle->buffer_watermark+(GAMING_MODE_VOICE_FRAME_INDEX_MAX-1-frame_index)*GAMING_MODE_VOICE_USB_FRAME_SAMPLES))
            {
                if (dongle_handle->clk_skew_count < 10)
                {
                    dongle_handle->clk_skew_count = dongle_handle->clk_skew_count + 1;
                    if ((dongle_handle->clk_skew_count < 0) && (dongle_handle->predicted_compen_times != 0))
                    {
                        dongle_handle->clk_skew_count = dongle_handle->clk_skew_count - 1;
                        dongle_handle->compen_samples = -1;
                        dongle_handle->predicted_compen_times -= 1;
                    }
                    else if (dongle_handle->clk_skew_count >= 10)
                    {
                        dongle_handle->predicted_compen_times = GAMING_MODE_VOICE_USB_FRAME_SAMPLES * dongle_handle->compensation_mode;
                    }
                }
                if (dongle_handle->clk_skew_count >= 10)
                {
                    /* we will do compensatory till the buffer water is dismatch */
                    dongle_handle->compen_samples = 1;
                    dongle_handle->predicted_compen_times -= 1;
                    if (dongle_handle->predicted_compen_times < 0)
                    {
                        dongle_handle->predicted_compen_count += 1;
                        if (dongle_handle->predicted_compen_count > 8)
                        {
                            /* in here, it means the current compensatroy mode is too slow, so we need to use the more aggressive compensatroy mode  */
                            dongle_handle->compensation_mode = SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_1_FRAME;
                            stream_function_sw_clk_skew_configure_compensation_mode(dongle_handle->clk_skew_port, dongle_handle->compensation_mode);
                            dongle_handle->predicted_compen_times = (int32_t)(dongle_handle->buffer_watermark+(GAMING_MODE_VOICE_FRAME_INDEX_MAX-1-frame_index)*GAMING_MODE_VOICE_USB_FRAME_SAMPLES) - compensatory_samples;
                        }
                        else
                        {
                            dongle_handle->compensation_mode = SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_8_FRAME;
                            stream_function_sw_clk_skew_configure_compensation_mode(dongle_handle->clk_skew_port, dongle_handle->compensation_mode);
                            dongle_handle->predicted_compen_times = (int32_t)(dongle_handle->buffer_watermark+(GAMING_MODE_VOICE_FRAME_INDEX_MAX-1-frame_index)*GAMING_MODE_VOICE_USB_FRAME_SAMPLES) - compensatory_samples;
                        }
                    }
                }
            }
            else
            {
                dongle_handle->clk_skew_count = 0;
                dongle_handle->compen_samples = 0;
                dongle_handle->predicted_compen_times = 0;
                dongle_handle->predicted_compen_count = 0;
                dongle_handle->compensation_mode = SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_8_FRAME;
                stream_function_sw_clk_skew_configure_compensation_mode(dongle_handle->clk_skew_port, dongle_handle->compensation_mode);
            }
            /* reset fetch frame count */
            dongle_handle->frame_count = 0;
        }
        else
        {
            dongle_handle->compen_samples = 0;
        }
        /* do compensatory */
        stream_function_sw_clk_skew_configure_compensation_samples(dongle_handle->clk_skew_port, dongle_handle->compen_samples);
        #endif /* AIR_SOFTWARE_CLK_SKEW_ENABLE */
    }
    else
    {
        /* there is drop requirment */
        *(dst_buf+0) = 0;
        *(dst_buf+1) = 0;
        *(dst_buf+2) = 0xa5;
        *(dst_buf+3) = 0xa5;

        /* update stream status */
        stream = DSP_Streaming_Get(source, source->transform->sink);
        stream->callback.EntryPara.in_size = 0;
        stream->callback.EntryPara.in_channel_num = 1;
    }

    /* config output */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (dongle_handle->fetch_flag != 0)
    {
        #ifdef AIR_SOFTWARE_BUFFER_ENABLE
        if (dongle_handle->play_en_status != 0)
        {
            /* only output 2.5ms*16K data to usb at every ccni interrupt trigger after the play is started*/
            stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 1, GAMING_MODE_VOICE_USB_FRAME_SAMPLES*sizeof(int16_t));
            stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 2, GAMING_MODE_VOICE_USB_FRAME_SAMPLES*sizeof(int16_t));
            #ifdef AIR_GAMING_MODE_DONGLE_ECNR_ENABLE
            uint32_t remain_samples1 = stream_function_sw_buffer_get_channel_used_size(dongle_handle->ecnr_out_buffer_port, 1) / sizeof(int16_t);
            uint32_t remain_samples2 = stream_function_sw_buffer_get_channel_used_size(dongle_handle->ecnr_in_buffer_port, 1) / sizeof(int16_t);
            if (dongle_handle->data_status != GAMING_MODE_DONGLE_UL_DATA_BYPASS_DECODER)
            {
                if (remain_samples2 == 0)
                {
                    /* after this time process, there is 7.5ms data in ecnr_in_buffer and there is no data in ecnr_out_buffer */
                    stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 1, remain_samples1*sizeof(int16_t));
                    stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 2, remain_samples1*sizeof(int16_t));
                }
                else
                {
                    /* after this time process, there is no data in ecnr_in_buffer and there is 7.5ms data in ecnr_out_buffer */
                    stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 1, GAMING_MODE_VOICE_BT_FRAME_SAMPLES*sizeof(int16_t));
                    stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 2, GAMING_MODE_VOICE_BT_FRAME_SAMPLES*sizeof(int16_t));
                }
            }
            else
            {
                /* do not output data in ecnr_out_buffer */
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 1, 0);
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 2, 0);
            }
            #endif /* AIR_GAMING_MODE_DONGLE_ECNR_ENABLE */
            dongle_handle->buffer_output_size = GAMING_MODE_VOICE_USB_FRAME_SAMPLES;
            dongle_handle->frame_count += 1;
            dongle_handle->frame_index = (dongle_handle->frame_index+1)%GAMING_MODE_VOICE_FRAME_INDEX_MAX;

            /* decrease fetch flag */
            dongle_handle->fetch_flag -= 1;
        }
        else
        {
            /* do not output data to usb at every ccni interrupt trigger because the play is not started*/
            stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 1, 0);
            stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 2, 0);
            #ifdef AIR_GAMING_MODE_DONGLE_ECNR_ENABLE
            uint32_t remain_samples1 = stream_function_sw_buffer_get_channel_used_size(dongle_handle->ecnr_out_buffer_port, 1) / sizeof(int16_t);
            uint32_t remain_samples2 = stream_function_sw_buffer_get_channel_used_size(dongle_handle->ecnr_in_buffer_port, 1) / sizeof(int16_t);
            if (dongle_handle->data_status != GAMING_MODE_DONGLE_UL_DATA_BYPASS_DECODER)
            {
                if (remain_samples2 == 0)
                {
                    /* after this time process, there is 7.5ms data in ecnr_in_buffer and there is no data in ecnr_out_buffer */
                    stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 1, remain_samples1*sizeof(int16_t));
                    stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 2, remain_samples1*sizeof(int16_t));
                }
                else
                {
                    /* after this time process, there is no data in ecnr_in_buffer and there is 7.5ms data in ecnr_out_buffer */
                    stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 1, GAMING_MODE_VOICE_BT_FRAME_SAMPLES*sizeof(int16_t));
                    stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 2, GAMING_MODE_VOICE_BT_FRAME_SAMPLES*sizeof(int16_t));
                }
            }
            else
            {
                /* do not output data in ecnr_out_buffer */
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 1, 0);
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 2, 0);
            }
            #endif /* AIR_GAMING_MODE_DONGLE_ECNR_ENABLE */
            dongle_handle->buffer_output_size = 0;

            /* reset fetch flag */
            dongle_handle->fetch_flag = 0;
        }
        #endif /* AIR_SOFTWARE_BUFFER_ENABLE */
    }
    else
    {
        #ifdef AIR_SOFTWARE_BUFFER_ENABLE
        /* keep these data in buffer and not output them to usb */
        stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 1, 0);
        stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 2, 0);
        #ifdef AIR_GAMING_MODE_DONGLE_ECNR_ENABLE
        uint32_t remain_samples1 = stream_function_sw_buffer_get_channel_used_size(dongle_handle->ecnr_out_buffer_port, 1) / sizeof(int16_t);
        uint32_t remain_samples2 = stream_function_sw_buffer_get_channel_used_size(dongle_handle->ecnr_in_buffer_port, 1) / sizeof(int16_t);
        if (dongle_handle->data_status != GAMING_MODE_DONGLE_UL_DATA_BYPASS_DECODER)
        {
            if (remain_samples2 == 0)
            {
                /* after this time process, there is 7.5ms data in ecnr_in_buffer and there is no data in ecnr_out_buffer */
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 1, remain_samples1*sizeof(int16_t));
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 2, remain_samples1*sizeof(int16_t));
            }
            else
            {
                /* after this time process, there is no data in ecnr_in_buffer and there is 7.5ms data in ecnr_out_buffer */
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 1, GAMING_MODE_VOICE_BT_FRAME_SAMPLES*sizeof(int16_t));
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 2, GAMING_MODE_VOICE_BT_FRAME_SAMPLES*sizeof(int16_t));
            }
        }
        else
        {
            /* do not output data in ecnr_out_buffer */
            stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 1, 0);
            stream_function_sw_buffer_config_channel_output_size(dongle_handle->ecnr_out_buffer_port, 2, 0);
        }
        #endif /* AIR_GAMING_MODE_DONGLE_ECNR_ENABLE */
        dongle_handle->buffer_output_size = 0;
        #endif /* AIR_SOFTWARE_BUFFER_ENABLE */
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    #if GAMING_MODE_VOICE_DONGLE_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[bt common][source][copy_payload]: %u, %u, %u, %d, %u, %d", 6,
    source->param.bt_dl.seq_num,
    source->streamBuffer.AVMBufferInfo.ReadIndex,
    source->streamBuffer.AVMBufferInfo.WriteIndex,
    length,
    current_timestamp,
    hal_nvic_query_exception_number());
    #endif /* GAMING_MODE_VOICE_DONGLE_DEBUG_LOG */

    return length;
}

bool gaming_mode_dongle_ul_source_get_new_read_offset(SOURCE source, U32 amount, U32 *new_read_offset)
{
    gaming_mode_dongle_ul_handle_t *dongle_handle = (gaming_mode_dongle_ul_handle_t *)(source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.handle);
    UNUSED(amount);

    if (dongle_handle->stream_status == GAMING_MODE_UL_STREAM_RUNNING)
    {
        /* there is no the packet lost, so needs to update the read offset */
        *new_read_offset = (source->streamBuffer.AVMBufferInfo.ReadIndex + source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.drop_frames) % source->streamBuffer.AVMBufferInfo.MemBlkNum;
    }
    else
    {
        /* do not update read index */
        *new_read_offset = source->streamBuffer.AVMBufferInfo.ReadIndex;
    }

    return true;
}

void gaming_mode_dongle_ul_source_drop_postprocess(SOURCE source, uint32_t amount)
{
    gaming_mode_dongle_ul_handle_t *dongle_handle = (gaming_mode_dongle_ul_handle_t *)(source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.handle);
    uint32_t remain_samples = stream_function_sw_buffer_get_channel_used_size(dongle_handle->buffer_port, 1) / sizeof(int16_t);
    UNUSED(amount);

    /* get current bt clock */
    extern VOID MCE_GetBtClk(BTCLK* pCurrCLK, BTPHASE* pCurrPhase,BT_CLOCK_OFFSET_SCENARIO type);
    uint32_t bt_clk;
    uint16_t bt_phase;
    MCE_GetBtClk((BTCLK*)&bt_clk, (BTPHASE*)&bt_phase,BT_CLK_Offset);

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&gaming_mode_voice_usb_out_gpt_count);

    #ifdef AIR_GAMING_MODE_DONGLE_ECNR_ENABLE
    uint32_t remain_samples1 = stream_function_sw_buffer_get_channel_used_size(dongle_handle->ecnr_out_buffer_port, 1) / sizeof(int16_t);
    uint32_t remain_samples2 = stream_function_sw_buffer_get_channel_used_size(dongle_handle->ecnr_in_buffer_port, 1) / sizeof(int16_t);
    DSP_MW_LOG_I("[bt common][source][drop_postprocess]: %u, %u, %u, %u, %u, %u, %u, %u, %u, %d, %d, %u, %u, %u, %d, %d, 0x%x, 0x%x, 0x%x, 0x%x", 20,
                dongle_handle->stream_status,
                dongle_handle->data_status,
                source->param.bt_dl.seq_num,
                dongle_handle->fetch_flag,
                source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.process_frames,
                source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.drop_frames,
                dongle_handle->buffer_output_size,
                (dongle_handle->frame_index+GAMING_MODE_VOICE_FRAME_INDEX_MAX-1)%GAMING_MODE_VOICE_FRAME_INDEX_MAX,
                dongle_handle->frame_count,
                dongle_handle->clk_skew_count,
                dongle_handle->compen_samples,
                remain_samples,
                remain_samples1,
                remain_samples2,
                dongle_handle->predicted_compen_times,
                dongle_handle->compensation_mode,
                dongle_handle->bt_clk,
                bt_clk,
                dongle_handle->data_in_gpt_count,
                gaming_mode_voice_usb_out_gpt_count);
    #else
    DSP_MW_LOG_I("[bt common][source][drop_postprocess]: %u, %u, %u, %u, %u, %u, %u, %u, %u, %d, %d, %u, %d, %d, 0x%x, 0x%x, 0x%x, 0x%x", 18,
                dongle_handle->stream_status,
                dongle_handle->data_status,
                source->param.bt_dl.seq_num,
                dongle_handle->fetch_flag,
                source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.process_frames,
                source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.drop_frames,
                dongle_handle->buffer_output_size,
                (dongle_handle->frame_index+GAMING_MODE_VOICE_FRAME_INDEX_MAX-1)%GAMING_MODE_VOICE_FRAME_INDEX_MAX,
                dongle_handle->frame_count,
                dongle_handle->clk_skew_count,
                dongle_handle->compen_samples,
                remain_samples,
                dongle_handle->predicted_compen_times,
                dongle_handle->compensation_mode,
                dongle_handle->bt_clk,
                bt_clk,
                dongle_handle->data_in_gpt_count,
                gaming_mode_voice_usb_out_gpt_count);
    #endif

    /* update seq_num when the stream is running */
    if ((dongle_handle->stream_status == GAMING_MODE_UL_STREAM_RUNNING) &&
        (dongle_handle->data_status == GAMING_MODE_DONGLE_UL_DATA_NORMAL))
    {
        source->param.bt_dl.seq_num += 1;
        source->param.bt_dl.seq_num = source->param.bt_dl.seq_num & 0xff;
    }

    #if GAMING_MODE_VOICE_DONGLE_DEBUG_LOG
    /* get the latest share buffer info */
    extern void bt_common_share_information_fetch(SOURCE source, SINK sink);
    bt_common_share_information_fetch(source, NULL);
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[bt common][source][drop_postprocess]: %u, %u, %u, %u, %d", 5,
                source->param.bt_dl.seq_num,
                source->streamBuffer.AVMBufferInfo.ReadIndex,
                source->streamBuffer.AVMBufferInfo.WriteIndex,
                current_timestamp,
                hal_nvic_query_exception_number());
    #endif /* GAMING_MODE_VOICE_DONGLE_DEBUG_LOG */
}

bool gaming_mode_dongle_ul_source_close(SOURCE source)
{
    if (source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.timer.handle)
    {
        hal_gpt_sw_stop_timer_us(source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.timer.handle);
        hal_gpt_sw_free_timer(source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.timer.handle);
        source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.timer.handle = 0;
    }

    #if GAMING_MODE_VOICE_DONGLE_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[bt common][source][close]: %u, %d", 2, current_timestamp, hal_nvic_query_exception_number());
    #endif /* GAMING_MODE_VOICE_DONGLE_DEBUG_LOG */

    return true;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 bool gaming_mode_dongle_ul_sink_get_avail_size(SINK sink, uint32_t *avail_size)
{
    if (sink->streamBuffer.ShareBufferInfo.ReadOffset < sink->streamBuffer.ShareBufferInfo.WriteOffset)
    {
        /* normal case */
        *avail_size = sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_num
                    - sink->streamBuffer.ShareBufferInfo.WriteOffset
                    + sink->streamBuffer.ShareBufferInfo.ReadOffset;
    }
    else if (sink->streamBuffer.ShareBufferInfo.ReadOffset == sink->streamBuffer.ShareBufferInfo.WriteOffset)
    {
        if(sink->streamBuffer.ShareBufferInfo.bBufferIsFull == true)
        {
            /* buffer is full, so ReadOffset == WriteOffset */
            *avail_size = 0;
        }
        else
        {
            /* buffer is empty, so ReadOffset == WriteOffset */
            *avail_size = sink->streamBuffer.ShareBufferInfo.length;
        }
    }
    else
    {
        /* buffer wrapper case */
        *avail_size = sink->streamBuffer.ShareBufferInfo.ReadOffset - sink->streamBuffer.ShareBufferInfo.WriteOffset;
    }
    /* change avail_size to max_payload_size if the avail_size is too large */
    if (*avail_size > sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size)
    {
        *avail_size = sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size;
    }

    return true;
}

uint32_t gaming_mode_dongle_ul_sink_copy_payload(SINK sink, uint8_t *src_buf, uint8_t *dst_buf, uint32_t length)
{
    TRANSFORM transform =  sink->transform;
    DSP_STREAMING_PARA_PTR stream = NULL;
    UNUSED(src_buf);

    // DSP_MW_LOG_I("[audio transmitter][source]: length = %d", 1, length);
    // length = length - sizeof(audio_transmitter_frame_header_t) - 1;

    /* copy usb auido data into the share buffer */
    // memcpy(dst_buf, src_buf, length);
    stream = DSP_Streaming_Get(transform->source, sink);

    if ((length%2) != 0)
    {
        configASSERT(0);
    }
    // DSP_D2I_BufferCopy_16bit((U16 *)dst_buf,
    //                             stream->callback.EntryPara.out_ptr[0],
    //                             stream->callback.EntryPara.out_ptr[1],
    //                             length / 2);
    memcpy(dst_buf, stream->callback.EntryPara.out_ptr[0], length);

    #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    #if GAMING_MODE_VOICE_DONGLE_DEBUG_DUMP
    LOG_AUDIO_DUMP(dst_buf, length, VOICE_TX_MIC_1);
    #endif /* GAMING_MODE_VOICE_DONGLE_DEBUG_DUMP */
    #endif /* MTK_AUDIO_DUMP_BY_CONFIGTOOL */

    // length = length * 2;

    return length;
}

bool gaming_mode_dongle_ul_sink_close(SINK sink)
{
    UNUSED(sink);

    /* application deinit */
    return true;
}

#endif /* AIR_GAMING_MODE_DONGLE_ENABLE */
