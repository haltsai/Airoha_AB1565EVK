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

#include "hal_audio.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

#include <string.h>
#include "hal_nvic.h"
#include "hal_audio_afe_control.h"
#include "hal_audio_afe_define.h"
#include "hal_audio_afe_clock.h"
#include "hal_audio_afe_connection.h"
#include "hal_gpt.h"
#include "hal_gpio.h"
#include "hal_log.h"
#include "assert.h"
#include "memory_attribute.h"
#include "dtm.h"
#ifdef  AIR_AUDIO_HW_LOOPBACK_ENABLE
#include "hal_audio_driver.h"
#endif

#define SRAM_BLOCK_SIZE (4096)
extern afe_t afe;
extern void platform_assert(const char *expr, const char *file, int line);
static afe_audio_memif_attr_t m_audio_memif[AUDIO_DIGITAL_BLOCK_NUM_OF_DIGITAL_BLOCK];
afe_sram_manager_t audio_sram_manager;

static uint32_t afe_sram_mode_size[2] = {
    AFE_INTERNAL_SRAM_NORMAL_SIZE,
    AFE_INTERNAL_SRAM_COMPACT_SIZE
};

#if 0
/* audio I2S GPIO group */
static const uint32_t afe_i2s_gpio[I2S_GPIO_NUM][4] = {
    {HAL_GPIO_14, HAL_GPIO_18, HAL_GPIO_37, HAL_GPIO_60},//MCLK
    {HAL_GPIO_16, HAL_GPIO_22, HAL_GPIO_39, HAL_GPIO_56},//CK
    {HAL_GPIO_17, HAL_GPIO_21, HAL_GPIO_41, HAL_GPIO_58},//WS
    {HAL_GPIO_15, HAL_GPIO_23, HAL_GPIO_38, HAL_GPIO_59},//TX
    {HAL_GPIO_19, HAL_GPIO_20, HAL_GPIO_40, HAL_GPIO_57},//RX

};
#endif

ATTR_TEXT_IN_IRAM_LEVEL_2 uint32_t word_size_align(uint32_t in_size)
{
    uint32_t align_size;
    align_size = in_size & 0xFFFFFFFC; //4 bytes align
    return align_size;
}

bool afe_clear_memory_block(afe_block_t *afe_block, audio_digital_block_t mem_block)
{
    // only clear R/W, data remained in the physical address
    if (mem_block < AUDIO_DIGITAL_BLOCK_NUM_OF_MEM_INTERFACE) {
        afe_block->u4WriteIdx      = 0;
        afe_block->u4ReadIdx       = 0;
        afe_block->u4DataRemained  = 0;
    } else {
        return false;
    }
    return true;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 uint32_t afe_get_memory_path_enable(audio_digital_block_t aud_block)
{
    if (aud_block < AUDIO_DIGITAL_BLOCK_NUM_OF_DIGITAL_BLOCK) {
        if (m_audio_memif[aud_block].mState) {
            return m_audio_memif[aud_block].mUserCount;
        }
    }
    return false;
}

bool afe_set_memory_path_enable(audio_digital_block_t aud_block, bool modify_reg, bool enable)
{
    uint32_t mask;
    if (aud_block >= AUDIO_DIGITAL_BLOCK_NUM_OF_DIGITAL_BLOCK) {
        return false;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    /* set for counter */
    if (enable == true) {
        if (m_audio_memif[aud_block].mUserCount == 0) {
            m_audio_memif[aud_block].mState = true;
            afe_set_enable(true);
        }
        m_audio_memif[aud_block].mUserCount++;
    } else {
        m_audio_memif[aud_block].mUserCount--;
        if (m_audio_memif[aud_block].mUserCount == 0) {
            m_audio_memif[aud_block].mState = false;
            afe_set_enable(false);
        }
        if (m_audio_memif[aud_block].mUserCount < 0) {
            m_audio_memif[aud_block].mUserCount = 0;
        }
    }
    hal_nvic_restore_interrupt_mask(mask);

    if (aud_block >= AUDIO_DIGITAL_BLOCK_NUM_OF_MEM_INTERFACE) {
        return true;
    }

    if (!modify_reg) {
        return true;
    }
    //modify for ab1568
    #if 0
    if ((enable == true) && (m_audio_memif[aud_block].mUserCount == 1)) { //avoid re-entry
        afe_set_memory_path_enable_reg(aud_block, enable);
    } else if ((enable == false) && (m_audio_memif[aud_block].mUserCount == 0)) {
        afe_set_memory_path_enable_reg(aud_block, enable);
    }
    #endif
    return true;
}

static bool afe_set_irq_enable(afe_irq_mode_t irq_mode, bool enable)
{
    const audio_register_bit_info_t *irq_on_reg, *irq_en_reg, *irq_clr_reg, *irq_miss_reg;

    if (irq_mode >= AFE_IRQ_NUM) {
        return false;
    }

    irq_on_reg = &afe_get_irq_control_reg(irq_mode)->on;
    AFE_SET_REG(irq_on_reg->reg, (enable << irq_on_reg->sbit), (irq_on_reg->mask << irq_on_reg->sbit));

    irq_en_reg = &afe_get_irq_control_reg(irq_mode)->en;
    AFE_SET_REG(irq_en_reg->reg, (enable << irq_en_reg->sbit), (irq_en_reg->mask << irq_en_reg->sbit));

    /* clear irq status */
    if (enable == false) {
        irq_clr_reg = &afe_get_irq_control_reg(irq_mode)->clr;
        AFE_SET_REG(irq_clr_reg->reg, (1 << irq_clr_reg->sbit), (irq_clr_reg->mask << irq_clr_reg->sbit));

        irq_miss_reg = &afe_get_irq_control_reg(irq_mode)->missclr;
        AFE_SET_REG(irq_miss_reg->reg, (1 << irq_miss_reg->sbit), (irq_miss_reg->mask << irq_miss_reg->sbit));
    }
    return true;
}

bool afe_set_irq_samplerate(afe_irq_mode_t irq_mode, uint32_t samplerate)
{
    uint32_t sr_idx = afe_samplerate_transform(samplerate, 0);
    const audio_register_bit_info_t *irq_mode_reg;

    if (irq_mode >= AFE_IRQ_NUM) {
        return false;
    }

    irq_mode_reg = &afe_get_irq_control_reg(irq_mode)->mode;
    AFE_SET_REG(irq_mode_reg->reg, sr_idx << irq_mode_reg->sbit, irq_mode_reg->mask << irq_mode_reg->sbit);
    return true;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 bool afe_set_irq_counter(afe_irq_mode_t irq_mode, uint32_t counter)
{
    const audio_register_bit_info_t *irq_cnt_reg;

    if (irq_mode >= AFE_IRQ_NUM) {
        return false;
    }

    irq_cnt_reg = &afe_get_irq_control_reg(irq_mode)->cnt;
    AFE_SET_REG(irq_cnt_reg->reg, counter, irq_cnt_reg->mask);

    return true;
}

/* IRQ Manager */
int32_t afe_enable_audio_irq(afe_irq_mode_t _irq, uint32_t _rate, uint32_t _count)
{
    UNUSED(_irq);
    UNUSED(_rate);
    UNUSED(_count);
    return 0;//modify for ab1568
    #if 0
    afe_set_irq_samplerate(_irq, _rate);
    afe_set_irq_counter(_irq, _count);
    afe_set_irq_enable(_irq, true);
    return 0;
    #endif
}

int32_t afe_disable_audio_irq(afe_irq_mode_t _irq)
{
    afe_set_irq_enable(_irq, false);
    afe_set_irq_counter(_irq, 0);
    return 0;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 int32_t afe_update_audio_irq_cnt(afe_irq_mode_t _irq, uint32_t _count)
{
    afe_set_irq_counter(_irq, _count);
    return 0;
}

static void update_sram_block_valid(afe_sram_mode_t mode)
{
    uint32_t i;

    for (i = 0; i < audio_sram_manager.mBlocknum; i++) {
        if ((i + 1) * audio_sram_manager.mBlockSize > afe_sram_mode_size[mode]) {
             audio_sram_manager.mAud_sram_block[i].mValid = false;
        }
    }
}

static void afe_init_sram_manager(uint32_t sram_block_size)
{
    uint32_t i = 0;

    memset((void *)&audio_sram_manager, 0, sizeof(afe_sram_manager_t));
    audio_sram_manager.mSram_phys_addr = afe_get_sram_phys_addr();
    audio_sram_manager.mSramLength =  afe_get_afe_sram_length();
    audio_sram_manager.mBlockSize = sram_block_size;
    audio_sram_manager.mBlocknum = (audio_sram_manager.mSramLength / audio_sram_manager.mBlockSize);

    for (i = 0; i < audio_sram_manager.mBlocknum ; i++) {
        audio_sram_manager.mAud_sram_block[i].mValid = true;
        audio_sram_manager.mAud_sram_block[i].mLength = audio_sram_manager.mBlockSize;
        audio_sram_manager.mAud_sram_block[i].mUser = NULL;
        audio_sram_manager.mAud_sram_block[i].msram_phys_addr = audio_sram_manager.mSram_phys_addr + (sram_block_size * i);
    }

    /* init for normal mode or compact mode */
    audio_sram_manager.sram_mode = afe_get_prefer_sram_mode();
    update_sram_block_valid(audio_sram_manager.sram_mode);
}

static void afe_reset_control(void)
{
    uint32_t mask;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    memset((void *)&m_audio_memif, 0, AUDIO_DIGITAL_BLOCK_NUM_OF_DIGITAL_BLOCK * sizeof(afe_audio_memif_attr_t));
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_control_init(void)
{
    uint32_t mask;
    afe_reset_control();
    afe_clock_variable_init();

    hal_nvic_save_and_set_interrupt_mask(&mask);
     /* allocate memory for pointers */
    memset((void *)&m_audio_memif, 0, AUDIO_DIGITAL_BLOCK_NUM_OF_DIGITAL_BLOCK * sizeof(afe_audio_memif_attr_t));
    afe_init_sram_manager(SRAM_BLOCK_SIZE);
    //afe_register_audio_irq(AUDIOSYS0_IRQn);//modfiy ab1568
    #if (AFE_REGISTER_ASRC_IRQ)
    afe_register_audio_irq(AUDIOSYS2_IRQn);
    #endif
    hal_nvic_restore_interrupt_mask(mask);
}

static unsigned long long is_init = 0;

hal_nvic_status_t afe_register_audio_irq(uint32_t irq_number)
{
    hal_nvic_status_t ret = HAL_NVIC_STATUS_OK;

    //[ToDo: other irq] 15:AudioSys, 16/17: HWSRC, 19/20/21/22: I2S0~3
    if (!(is_init & (1<<irq_number))) {
        hal_nvic_disable_irq(irq_number);
        ret = hal_nvic_register_isr_handler(irq_number, (hal_nvic_isr_t)audsys_irq_handler);
        if (ret != HAL_NVIC_STATUS_OK) {
            return ret;
        }
        ret = hal_nvic_enable_irq(irq_number);
        is_init |= (1<<irq_number);
    }
    return ret;
}

static uint32_t afe_adda_dl_samplerate_transform(uint32_t samplerate)
{
    switch (samplerate) {
        case 8000:
            return AFE_ADDA_DL_SAMPLERATE_8K;
        case 11025:
            return AFE_ADDA_DL_SAMPLERATE_11K;
        case 12000:
            return AFE_ADDA_DL_SAMPLERATE_12K;
        case 16000:
            return AFE_ADDA_DL_SAMPLERATE_16K;
        case 22050:
            return AFE_ADDA_DL_SAMPLERATE_22K;
        case 24000:
            return AFE_ADDA_DL_SAMPLERATE_24K;
        case 32000:
            return AFE_ADDA_DL_SAMPLERATE_32K;
        case 44100:
            return AFE_ADDA_DL_SAMPLERATE_44K;
        case 48000:
            return AFE_ADDA_DL_SAMPLERATE_48K;
        case 96000:
            return AFE_ADDA_DL_SAMPLERATE_96K;
        case 192000:
            return AFE_ADDA_DL_SAMPLERATE_192K;
        default:
            return AFE_ADDA_DL_SAMPLERATE_44K;
    }
}

static uint32_t afe_adda_ul_samplerate_transform(uint32_t samplerate)
{
    switch (samplerate) {
        case 8000:
            return AFE_ADDA_UL_SAMPLERATE_8K;
        case 16000:
            return AFE_ADDA_UL_SAMPLERATE_16K;
        case 32000:
            return AFE_ADDA_UL_SAMPLERATE_32K;
        case 48000:
            return AFE_ADDA_UL_SAMPLERATE_48K;
        case 96000:
            return AFE_ADDA_UL_SAMPLERATE_96K;
        case 192000:
            return AFE_ADDA_UL_SAMPLERATE_192K;
        default:
            return AFE_ADDA_UL_SAMPLERATE_16K;
    }
}

uint32_t afe_general_samplerate_transform(uint32_t samplerate)
{
    switch (samplerate) {
        case 8000:
            return AFE_GENERAL_SAMPLERATE_8K;
        case 11025:
            return AFE_GENERAL_SAMPLERATE_11K;
        case 12000:
            return AFE_GENERAL_SAMPLERATE_12K;
        case 16000:
            return AFE_GENERAL_SAMPLERATE_16K;
        case 22050:
            return AFE_GENERAL_SAMPLERATE_22K;
        case 24000:
            return AFE_GENERAL_SAMPLERATE_24K;
        case 32000:
            return AFE_GENERAL_SAMPLERATE_32K;
        case 44100:
            return AFE_GENERAL_SAMPLERATE_44K;
        case 48000:
            return AFE_GENERAL_SAMPLERATE_48K;
        case 88200:
            return AFE_GENERAL_SAMPLERATE_88K;
        case 96000:
            return AFE_GENERAL_SAMPLERATE_96K;
        case 176400:
            return AFE_GENERAL_SAMPLERATE_176K;
        case 192000:
            return AFE_GENERAL_SAMPLERATE_192K;
        default:
            return AFE_GENERAL_SAMPLERATE_44K;
    }
}

uint32_t afe_samplerate_transform(uint32_t samplerate, audio_digital_block_t aud_block)
{
    switch (aud_block) {
        case AUDIO_DIGITAL_BLOCK_ADDA_DL:
            return afe_adda_dl_samplerate_transform(samplerate);
        case AUDIO_DIGITAL_BLOCK_ADDA_UL1:
        case AUDIO_DIGITAL_BLOCK_ADDA_UL2:
        case AUDIO_DIGITAL_BLOCK_ADDA_UL3:
            return afe_adda_ul_samplerate_transform(samplerate);
        default:
            return afe_general_samplerate_transform(samplerate);
    }
}

static uint32_t afe_adda_dl_reg_transform(uint32_t reg)
{
    switch (reg) {
        case AFE_ADDA_DL_SAMPLERATE_8K:
            return 8000;
        case AFE_ADDA_DL_SAMPLERATE_11K:
            return 11025;
        case AFE_ADDA_DL_SAMPLERATE_12K:
            return 12000;
        case AFE_ADDA_DL_SAMPLERATE_16K:
            return 16000;
        case AFE_ADDA_DL_SAMPLERATE_22K:
            return 22050;
        case AFE_ADDA_DL_SAMPLERATE_24K:
            return 24000;
        case AFE_ADDA_DL_SAMPLERATE_32K:
            return 32000;
        case AFE_ADDA_DL_SAMPLERATE_44K:
            return 44100;
        case AFE_ADDA_DL_SAMPLERATE_48K:
            return 48000;
        case AFE_ADDA_DL_SAMPLERATE_96K:
            return 96000;
        case AFE_ADDA_DL_SAMPLERATE_192K:
            return 192000;
        default:
            return 44100;
    }
}

static uint32_t afe_adda_ul_reg_transform(uint32_t reg)
{
    switch (reg) {
        case AFE_ADDA_UL_SAMPLERATE_8K:
            return 8000;
        case AFE_ADDA_UL_SAMPLERATE_16K:
            return 16000;
        case AFE_ADDA_UL_SAMPLERATE_32K:
            return 32000;
        case AFE_ADDA_UL_SAMPLERATE_48K:
            return 48000;
        case AFE_ADDA_UL_SAMPLERATE_96K:
            return 96000;
        case AFE_ADDA_UL_SAMPLERATE_192K:
            return 192000;
        default:
            return 16000;
    }
}

uint32_t afe_general_reg_transform(uint32_t reg)
{
    switch (reg) {
        case AFE_GENERAL_SAMPLERATE_8K:
            return 8000;
        case AFE_GENERAL_SAMPLERATE_11K:
            return 11025;
        case AFE_GENERAL_SAMPLERATE_12K:
            return 12000;
        case AFE_GENERAL_SAMPLERATE_16K:
            return 16000;
        case AFE_GENERAL_SAMPLERATE_22K:
            return 22050;
        case AFE_GENERAL_SAMPLERATE_24K:
            return 24000;
        case AFE_GENERAL_SAMPLERATE_32K:
            return 32000;
        case AFE_GENERAL_SAMPLERATE_44K:
            return 44100;
        case AFE_GENERAL_SAMPLERATE_48K:
            return 48000;
        case AFE_GENERAL_SAMPLERATE_88K:
            return 88200;
        case AFE_GENERAL_SAMPLERATE_96K:
            return 96000;
        case AFE_GENERAL_SAMPLERATE_176K:
            return 176400;
        case AFE_GENERAL_SAMPLERATE_192K:
            return 192000;
        default:
            return 44100;
    }
}

uint32_t afe_reg_value_transform(uint32_t reg, audio_digital_block_t aud_block)
{
    switch (aud_block) {
        case AUDIO_DIGITAL_BLOCK_ADDA_DL:
            return afe_adda_dl_reg_transform(reg);
        case AUDIO_DIGITAL_BLOCK_ADDA_UL1:
        case AUDIO_DIGITAL_BLOCK_ADDA_UL2:
        case AUDIO_DIGITAL_BLOCK_ADDA_UL3:
            return afe_adda_ul_reg_transform(reg);
        default:
            return afe_general_reg_transform(reg);
    }
}


static bool check_sram_available(uint32_t sram_length, uint32_t *sram_block_idx, uint32_t *sram_block_num)
{
    uint32_t max_sram_size = 0;
    uint32_t start_record = false;
    afe_sram_block_t *sram_block = NULL;
    uint32_t i = 0;
    *sram_block_idx = 0;

    for (i = 0; i < audio_sram_manager.mBlocknum; i++) {
        sram_block = &audio_sram_manager.mAud_sram_block[i];
        if ((sram_block->mUser == NULL) && sram_block->mValid) {
            max_sram_size += audio_sram_manager.mBlockSize;
            if (start_record == false) {
                start_record = true;
                *sram_block_idx = i;
            }
            (*sram_block_num)++;

            /* can allocate sram */
            if (max_sram_size >= sram_length) {
                break;
            }
        }
        /* when reach allocate buffer , reset condition*/
        if ((sram_block->mUser != NULL) && sram_block->mValid) {
            max_sram_size = 0;
            *sram_block_num = 0;
            *sram_block_idx = 0;
            start_record = false;
        }

        if (sram_block->mValid == 0) {
            break;
        }
    }
    if (max_sram_size >= sram_length) {
        return true;
    } else {
        return false;
    }
}

uint32_t afe_allocate_audio_sram(afe_block_t *afe_block, afe_pcm_format_t format, uint32_t sram_length, uint32_t force_normal)
{
    uint32_t sram_block_num = 0;
    uint32_t sram_block_idx = 0;
    afe_sram_block_t *sram_block = NULL;
    afe_sram_mode_t request_sram_mode;
    bool has_user = false;
    int32_t ret = 0;
    uint32_t i, mask;

    hal_nvic_save_and_set_interrupt_mask(&mask);

    /* check if sram has user */
    for (i = 0; i < audio_sram_manager.mBlocknum; i++) {
        sram_block = &audio_sram_manager.mAud_sram_block[i];
        if (sram_block->mValid == true && sram_block->mUser != NULL) {
            has_user = true;
            break;
        }
    }

    /* get sram mode for this request */
    if (force_normal) {
        request_sram_mode = AFE_SRAM_NORMAL_MODE;
    } else {
        if (format == AFE_PCM_FORMAT_S32_LE || format == AFE_PCM_FORMAT_U32_LE) {
            request_sram_mode = has_user ? audio_sram_manager.sram_mode : afe_get_prefer_sram_mode();
        } else {
            request_sram_mode = AFE_SRAM_NORMAL_MODE;
        }
    }

    /* change sram mode if needed */
    if (audio_sram_manager.sram_mode != request_sram_mode) {
        if (has_user) {
            hal_nvic_restore_interrupt_mask(mask);
            return -1;
        }
        audio_sram_manager.sram_mode = request_sram_mode;
        update_sram_block_valid(audio_sram_manager.sram_mode);
    }

    afe_set_sram_mode(audio_sram_manager.sram_mode);

    if (check_sram_available(sram_length, &sram_block_idx, &sram_block_num) == true) {
        afe_block->phys_buffer_addr = audio_sram_manager.mAud_sram_block[sram_block_idx].msram_phys_addr;
        /* set aud sram with user*/
        while (sram_block_num) {
            audio_sram_manager.mAud_sram_block[sram_block_idx].mUser = afe_block;
            sram_block_num--;
            sram_block_idx++;
         }
    } else {
        ret =  -1;
    }

    hal_nvic_restore_interrupt_mask(mask);
    return ret;
}

uint32_t afe_get_dac_enable(void)
{
    return m_audio_memif[AUDIO_DIGITAL_BLOCK_ADDA_DL].mState;
}

uint32_t afe_get_i2s0_enable(void)
{
    return (m_audio_memif[AUDIO_DIGITAL_BLOCK_I2S0_OUT].mState || m_audio_memif[AUDIO_DIGITAL_BLOCK_I2S0_IN].mState);
}
#if 0//modify for ab1568
uint32_t afe_get_i2s_enable(afe_i2s_num_t i2s_module)
{
    audio_digital_block_t in_block, out_block;
    in_block = AUDIO_DIGITAL_BLOCK_I2S0_IN + i2s_module;
    out_block = AUDIO_DIGITAL_BLOCK_I2S0_OUT + i2s_module;
    return (m_audio_memif[in_block].mState || m_audio_memif[out_block].mState);
}
#endif
uint32_t afe_get_all_i2s_enable(void)
{
    uint32_t i = 0;
    for (i = AUDIO_DIGITAL_BLOCK_I2S0_OUT; i <= AUDIO_DIGITAL_BLOCK_I2S3_IN; i++) {
        if ((m_audio_memif[i].mState) == true) {
            return true;
        }
    }
    return false;
}


void afe_set_dac_enable(bool enable)
{
    if (enable) {
        /* Enable DL SRC order:
        * DL clock (AUDIO_TOP_CON0) -> AFE (AFE_DAC_CON0) ->
        * ADDA UL DL (AFE_ADDA_UL_DL_CON0) ->
        * ADDA DL SRC (AFE_ADDA_DL_SRC2_CON0)
        */
        afe_dac_clock_on();
        afe_set_lr_swap(true);   //AB155x: NLE Digital & analog gain mismatch (ana L with Dig R / ana R with Dig L) -> Swap digital L/R
        afe_set_enable(true);
        afe_set_adda_enable(true);
        afe_set_dl_src_enable(true);
        //GVA-9682 Restore analog gain
        afe_audio_set_output_analog_gain();
    } else {
        /* Disable DL SRC order: (reverse)
        * ADDA DL SRC (AFE_ADDA_DL_SRC2_CON0) ->
        * ADDA UL DL (AFE_ADDA_UL_DL_CON0) ->
        * AFE (AFE_DAC_CON0) -> DL clock (AUDIO_TOP_CON0)
        */
        //GVA-9682 mute analog gain -32db
        AFE_SET_REG (ZCD_CON2, 0xB2C, 0xFFF);
        afe_set_dl_src_enable(false);
        afe_set_adda_enable(false);
        afe_set_enable(false);
        afe_dac_clock_off();
    }
}

void afe_set_adc_enable(bool enable, hal_audio_interface_t mic_interface)
{
    if (enable) {
        /* Enable UL SRC order:
        * UL clock (AUDIO_TOP_CON0) -> AFE (AFE_DAC_CON0) ->
        * ADDA UL DL (AFE_ADDA_UL_DL_CON0) ->
        * ADDA UL SRC (AFE_ADDA_UL_SRC_CON0)
        */
        // TODO:  Hires clock check the sampling rate
        afe_adc_clock_on();
        AFE_SET_REG(AFE_ADDA_UL_DL_CON0, 0, 0x20202000); //rst fifo
        afe_set_enable(true);
        afe_set_adda_enable(true);
        afe_set_ul_src_enable(true, mic_interface);
    } else {
        /* Disable UL SRC order: (reverse)
        * ADDA UL SRC (AFE_ADDA_UL_SRC_CON0) ->
        * ADDA UL DL (AFE_ADDA_UL_DL_CON0) ->
        * AFE (AFE_DAC_CON0) -> UL clock (AUDIO_TOP_CON0)
        */
        afe_set_ul_src_enable(false, mic_interface);
        afe_set_adda_enable(false);
        hal_gpt_delay_us(125);
        // TODO: Hires clock check the sampling rate
        afe_set_enable(false);
        AFE_SET_REG(AFE_ADDA_UL_DL_CON0, 0x20202000, 0x20202000); //rst fifo
        afe_adc_clock_off();
    }
}

/*static bool check_memif_enable(void)
{
    uint32_t i = 0;
    for (i = 0; i < AUDIO_DIGITAL_BLOCK_NUM_OF_DIGITAL_BLOCK; i++) {
        if ((m_audio_memif[i].mState) == true) {
            return true;
        }
    }
    return false;
}*/

static bool afe_on = false;
static uint32_t apll_samplerate;

bool afe_get_audio_apll_enable(void)
{
    return afe_on;
}

void afe_audio_apll_enable(bool enable, uint32_t samplerate)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);

    if (true == enable) {
        if (!afe_on) {
            afe_set_apll_for_i2s_reg(true, samplerate);
            afe_enable_apll_by_samplerate(samplerate);
        }
        afe_set_enable(true);
        if (!afe_on) {
            afe_enable_apll_tuner_by_samplerate(samplerate);
        }
        afe_on = true;
        apll_samplerate = samplerate;
   } else if (false == enable) {
        if (afe_on) {
            afe_disable_apll_tuner_by_samplerate(apll_samplerate);
        }
        afe_set_enable(false);
        if (afe_on) {
            afe_disable_apll_by_samplerate(apll_samplerate);
            afe_set_apll_for_i2s_reg(false, apll_samplerate);
        }
        afe_on = false;
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_set_adda_enable(bool enable)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);

    if (enable == true) {
        afe_set_adda_reg(true);
    } else {
        if (m_audio_memif[AUDIO_DIGITAL_BLOCK_ADDA_DL].mState == false &&
            m_audio_memif[AUDIO_DIGITAL_BLOCK_ADDA_UL1].mState == false &&
            m_audio_memif[AUDIO_DIGITAL_BLOCK_ADDA_UL2].mState == false &&
            m_audio_memif[AUDIO_DIGITAL_BLOCK_ADDA_UL3].mState == false &&
            afe_get_dl_src_reg_status() == false ) {
            afe_set_adda_reg(false);
        }
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_set_dl_src_enable(bool enable)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);

    if (enable == true) {
        afe_set_dl_src_reg(true);
    } else {
        if (m_audio_memif[AUDIO_DIGITAL_BLOCK_ADDA_DL].mState == false) {
            afe_set_dl_src_reg(false);
        }
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_set_ul_src_enable(bool enable, hal_audio_interface_t mic_interface)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);

    if (enable == true) {
        afe_set_ul_src_reg(true, mic_interface);
    } else {
        afe_set_ul_src_reg(false, mic_interface);
    }
    hal_nvic_restore_interrupt_mask(mask);
}
#if 0//modify for ab1568
void afe_set_i2s_enable(bool enable, afe_i2s_num_t i2s_module, afe_i2s_role_t mode)
{
    uint32_t mask;
    UNUSED(mode);
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (enable) {
        afe_set_i2s_mclk_reg(enable, i2s_module);
        hal_audio_afe_i2s_clock_on(i2s_module);
        afe_i2s_enable(true, i2s_module);
    } else {
        afe_i2s_enable(false, i2s_module);
        hal_audio_afe_i2s_clock_off(i2s_module);
        afe_set_i2s_mclk_reg(enable, i2s_module);
    }
    hal_nvic_restore_interrupt_mask(mask);
}
#endif
bool afe_set_memif_fetch_format_per_sample(uint32_t interface_type, uint32_t fetch_format)
{
    m_audio_memif[interface_type].mFetchFormatPerSample = fetch_format;
    return afe_set_memif_format_reg(interface_type, fetch_format);
}

void afe_audio_device_enable(bool enable, hal_audio_device_t audio_device, hal_audio_interface_t audio_interface, afe_pcm_format_t format, uint32_t rate, afe_misc_parms_t misc_parms)
{
    UNUSED(enable);
    UNUSED(audio_device);
    UNUSED(format);
    UNUSED(audio_interface);
    UNUSED(rate);
    UNUSED(misc_parms);
    #if 0//modify for ab1568
    afe_i2s_num_t i2s_module = AFE_I2S0;
    audio_digital_block_t digital_block = AUDIO_DIGITAL_BLOCK_I2S0_OUT;
    log_hal_msgid_info("DSP audio device enable:%d, device:%d, interface:%d, rate:%d, I2sClk:%d, Micbias:%d\r\n",6, enable, audio_device, audio_interface, rate, misc_parms.I2sClkSourceType, misc_parms.MicbiasSourceType);

    if (audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        if (audio_interface == HAL_AUDIO_INTERFACE_1) {
            digital_block = AUDIO_DIGITAL_BLOCK_I2S0_OUT;
            i2s_module = AFE_I2S0;
        } else if (audio_interface == HAL_AUDIO_INTERFACE_2) {
            digital_block = AUDIO_DIGITAL_BLOCK_I2S1_OUT;
            i2s_module = AFE_I2S1;
        } else if (audio_interface == HAL_AUDIO_INTERFACE_3) {
            digital_block = AUDIO_DIGITAL_BLOCK_I2S2_OUT;
            i2s_module = AFE_I2S2;
        } else if (audio_interface == HAL_AUDIO_INTERFACE_4) {
            digital_block = AUDIO_DIGITAL_BLOCK_I2S3_OUT;
            i2s_module = AFE_I2S3;
        }
    }

    if (audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        /*I2S master out*/
        if (enable) {
            if ((misc_parms.I2sClkSourceType != I2S_CLK_SOURCE_DCXO) && (misc_parms.I2sClkSourceType != I2S_CLK_SOURCE_APLL)) {
                log_hal_msgid_error("I2S Master misc_parms = %d is failed",1, misc_parms.I2sClkSourceType);
                platform_assert("I2S Master misc_parms is failed", __FILE__, __LINE__);
            }
            if ((afe_get_audio_apll_enable() == false) && (misc_parms.I2sClkSourceType == I2S_CLK_SOURCE_APLL)) {
                afe_audio_apll_enable(true, rate);
            }
            if (afe_get_i2s_enable(i2s_module) == false) {


                afe_set_i2s_reg(i2s_module,
                                I2S_MASTER,
                                (format<=AFE_PCM_FORMAT_U16_BE) ? I2S_16BIT : I2S_32BIT,
                                afe_samplerate_transform(rate, AUDIO_DIGITAL_BLOCK_I2S0_OUT),
                                I2S_NOSWAP,
                                misc_parms.I2sClkSourceType);
                afe_set_i2s_enable(true, i2s_module, I2S_MASTER);
            }
            afe_set_memory_path_enable(digital_block, true, true);
        } else {
            afe_set_memory_path_enable(digital_block, true, false);
            if (afe_get_i2s_enable(i2s_module) == false) {
                afe_set_i2s_enable(false, i2s_module, I2S_MASTER);
            }
            if ((afe_get_all_i2s_enable() == false) && (afe_get_audio_apll_enable() == true)) {
                afe_audio_apll_enable(false, rate);
            }
        }
    } else if (audio_device & HAL_AUDIO_DEVICE_DAC_DUAL) {
        /* start DAC out */
        if (enable) {
            #ifdef MTK_ANC_ENABLE
            //Configurate src rate when only ANC
            if ((afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_ADDA_DL)==1) && afe_get_anc_enable(audio_device)) {
                if (afe_get_dac_in_samplerate() != rate) {
                    uint32_t target_ZCD_CON2;
                    afe_audio_set_output_analog_gain_with_ramp(true, &target_ZCD_CON2);
                    afe_set_dl_src_reg(false);
                    afe_set_dac_in(rate);
                    afe_set_dl_src_reg(true);
                    afe_audio_set_output_analog_gain_with_ramp(false, &target_ZCD_CON2);
                    log_hal_msgid_info("DL rate changed to %d , there may be a pop. (avoid by ramp analog gain)", 1, rate);
                }
            }
            #endif

            if (format >= AFE_PCM_FORMAT_S24_LE) {
                afe_dac_hires_clock(false);
            }
            if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_ADDA_DL) == false) {
#ifdef ENABLE_AMP_TIMER
                if ((afe.amp_handle->open_handler==NULL) || (afe.amp_handle->open_handler(rate) == true)) {
#endif
                    afe_set_dac_in(rate);
                    afe_set_dac_enable(true);
                    //open hp amp
                    afe_amp_enable(true, audio_device);
#ifdef ENABLE_AMP_TIMER
                }
#endif
            }
            afe_set_memory_path_enable(AUDIO_DIGITAL_BLOCK_ADDA_DL, true, true);
        } else {
            afe_set_memory_path_enable(AUDIO_DIGITAL_BLOCK_ADDA_DL, true, false);
            if (afe_get_dac_enable() == false) {
#ifdef ENABLE_AMP_TIMER
                if ((afe.amp_handle->closure_handler==NULL) || (afe.amp_handle->closure_handler() == true)) {
#endif
                    //close hp amp
                    afe_amp_enable(false, audio_device);
                    afe_set_dac_enable(false);
                    afe_dac_hires_clock(true);//pdn dac hires clock
#ifdef ENABLE_AMP_TIMER
                }
#endif
            }
        }
    } else if ((audio_device & HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) || (audio_device & HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL) || (audio_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL)){
#if 0
        if ((((*((volatile uint32_t*)0xA2120B04))>>3)&0x01) == 0) {
            //DMIC Driver
            audio_device = HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL;
        }
        if ((((*((volatile uint32_t*)0xA2120B04))>>3)&0x01) == 1 || (((*((volatile uint32_t*)0xA2120B04))>>4)&0x01) == 1) {
            //AMIC Driver
            if(audio_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL) {
                audio_device = HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL;
            }
            else {
                audio_device = HAL_AUDIO_DEVICE_MAIN_MIC_DUAL;
            }
        }
        if(((*((volatile uint32_t*)(0xA2120B04)) >> 5)&0x01) == 1) {
            //AMIC mono L
            audio_device = HAL_AUDIO_DEVICE_MAIN_MIC_L;
        }
        if(((*((volatile uint32_t*)(0xA2120B04)) >> 6)&0x01) == 1) {
            //AMIC mono R
            audio_device = HAL_AUDIO_DEVICE_MAIN_MIC_R;
        }
#endif
        //audio_interface for digital-mic only
        if (!(audio_device&HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL)) {
            audio_interface = HAL_AUDIO_INTERFACE_1;
        }
        if ((misc_parms.MicbiasSourceType != MICBIAS_0) && (misc_parms.MicbiasSourceType != MICBIAS_1) && (misc_parms.MicbiasSourceType != MICBIAS_ALL)) {
            log_hal_msgid_error("Mic misc_parms = %d is failed",1, misc_parms.MicbiasSourceType);
            platform_assert("Mic misc_parms is failed", __FILE__, __LINE__);
        }

        digital_block = afe_get_digital_block_by_audio_device(audio_device, audio_interface, true);
        if (enable) {
            #ifdef MTK_ANC_ENABLE
            //Configurate src rate when only ANC
            if ((afe_get_memory_path_enable(digital_block)==1) && afe_get_anc_enable(audio_device)) {
                afe_set_adc_in(rate);
            }
            #endif
            if (format >= AFE_PCM_FORMAT_S24_LE) {
                afe_adc_hires_clock(false);
            }

            /* start ADC in */
            if (afe_get_memory_path_enable(digital_block) == false) {
                /*open dmic dcc mic*/
                afe_dcc_mic_enable(true, audio_device, misc_parms.MicbiasSourceType);
                afe_set_adc_in(rate);

                //DMIC Driver
                if (audio_device & HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL) {
                    afe_set_dmic_samplerate(audio_interface, rate);
                    afe_set_dmic_path(true, audio_interface, rate);
                }

                afe_set_adc_enable(true, audio_interface);
                hal_gpt_delay_us(20000);

            }
            afe_set_memory_path_enable(digital_block, true, true);
        } else {
            /* stop ADC input */
            afe_set_memory_path_enable(digital_block, true, false);
            if (afe_get_memory_path_enable(digital_block) == false) {

                afe_set_adc_enable(false, audio_interface);
                afe_adc_hires_clock(true);

                //DMIC Driver
                if (audio_device & HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL) {
                    afe_set_dmic_path(false, audio_interface, rate);
                }

                /*close mic*/
                afe_dcc_mic_enable(false, audio_device, misc_parms.MicbiasSourceType);
            }
        }
    } else {
        //other device out config
    }
    #endif
}

uint32_t afe_get_audio_device_samplerate(hal_audio_device_t audio_device, hal_audio_interface_t audio_interface)
{
    UNUSED(audio_device);
    UNUSED(audio_interface);
    return 0;
    #if 0//modify for ab1568
    afe_i2s_num_t i2s_module = AFE_I2S0;
    uint32_t samplerate = 0;

    if (audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        if (audio_interface == HAL_AUDIO_INTERFACE_1) {
            i2s_module = AFE_I2S0;
        } else if (audio_interface == HAL_AUDIO_INTERFACE_2) {
            i2s_module = AFE_I2S1;
        } else if (audio_interface == HAL_AUDIO_INTERFACE_3) {
            i2s_module = AFE_I2S2;
        } else if (audio_interface == HAL_AUDIO_INTERFACE_4) {
            i2s_module = AFE_I2S3;
        }
        samplerate = afe_get_i2s_master_samplerate(i2s_module);
    } else if (audio_device & HAL_AUDIO_DEVICE_DAC_DUAL) {
        samplerate = afe_get_dac_in_samplerate();
    } else if ((audio_device & HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) || (audio_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL)){
        samplerate = afe_get_adc_in_samplerate();
    } else if (audio_device & HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL){
        samplerate = afe_get_dmic_samplerate(audio_interface);
    } else {
        //other device out config
        samplerate = 0;
    }

    return samplerate;
    #endif
}


audio_digital_block_t afe_get_digital_block_by_audio_device (hal_audio_device_t device, hal_audio_interface_t audio_interface, bool is_input) {
    UNUSED(device);
    UNUSED(audio_interface);
    UNUSED(is_input);
    return 0;
    #if 0//modify for ab1568
    audio_digital_block_t digital_block = 0;
    uint32_t offset;
    switch (audio_interface) {
        case HAL_AUDIO_INTERFACE_1 :
        default:
            offset = 0;
            break;
        case HAL_AUDIO_INTERFACE_2 :
            offset = 1;
            break;
        case HAL_AUDIO_INTERFACE_3 :
            offset = 2;
            break;
        case HAL_AUDIO_INTERFACE_4 :
            offset = 3;
            break;
    }

    switch (device) {
        case HAL_AUDIO_DEVICE_NONE:                         /**<  No audio device is on. */
        case HAL_AUDIO_DEVICE_EXT_CODEC:
        default:
            break;
        case HAL_AUDIO_DEVICE_MAIN_MIC_L:
        case HAL_AUDIO_DEVICE_MAIN_MIC_R:
        case HAL_AUDIO_DEVICE_MAIN_MIC_DUAL:
        case HAL_AUDIO_DEVICE_LINEINPLAYBACK_L:
        case HAL_AUDIO_DEVICE_LINEINPLAYBACK_R:
        case HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL:
            digital_block = AUDIO_DIGITAL_BLOCK_ADDA_UL1;
            break;
        case HAL_AUDIO_DEVICE_DIGITAL_MIC_L:
        case HAL_AUDIO_DEVICE_DIGITAL_MIC_R:
        case HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL:
            digital_block = AUDIO_DIGITAL_BLOCK_ADDA_UL1 + offset;
            break;
        case HAL_AUDIO_DEVICE_DAC_L:
        case HAL_AUDIO_DEVICE_DAC_R:
        case HAL_AUDIO_DEVICE_DAC_DUAL:
            digital_block = AUDIO_DIGITAL_BLOCK_ADDA_DL;
            break;
        case HAL_AUDIO_DEVICE_I2S_MASTER:
            if (is_input) {
                digital_block = AUDIO_DIGITAL_BLOCK_I2S0_IN + offset;
            } else {
                digital_block = AUDIO_DIGITAL_BLOCK_I2S0_OUT+ offset;
            }
            break;
        case HAL_AUDIO_DEVICE_I2S_SLAVE:
            break;
    }
    return digital_block;
    #endif
}

audio_afe_io_block_t afe_get_io_block_by_audio_device (hal_audio_device_t device, hal_audio_interface_t audio_interface, bool is_input) {
    UNUSED(device);
    UNUSED(audio_interface);
    UNUSED(is_input);
    return 0;
    #if 0//modify for ab1568
    audio_afe_io_block_t io_block = 0;
    uint32_t offset;
    switch (audio_interface) {
        case HAL_AUDIO_INTERFACE_1 :
        default:
            offset = 0;
            break;
        case HAL_AUDIO_INTERFACE_2 :
            offset = 1;
            break;
        case HAL_AUDIO_INTERFACE_3 :
            offset = 2;
            break;
        case HAL_AUDIO_INTERFACE_4 :
            offset = 3;
            break;
    }

    switch (device) {
        case HAL_AUDIO_DEVICE_NONE:                         /**<  No audio device is on. */
        default:
            break;
        case HAL_AUDIO_DEVICE_MAIN_MIC_L:
        case HAL_AUDIO_DEVICE_MAIN_MIC_DUAL:
        case HAL_AUDIO_DEVICE_LINEINPLAYBACK_L:
        case HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL:
        case HAL_AUDIO_DEVICE_DIGITAL_MIC_L:
        case HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL:
            io_block = AUDIO_AFE_IO_BLOCK_ADDA_UL1 + 2*offset;
            break;

        case HAL_AUDIO_DEVICE_MAIN_MIC_R:
        case HAL_AUDIO_DEVICE_DIGITAL_MIC_R:
        case HAL_AUDIO_DEVICE_LINEINPLAYBACK_R:
            io_block = AUDIO_AFE_IO_BLOCK_ADDA_UL1_CH2 + 2*offset;
            break;
        case HAL_AUDIO_DEVICE_DAC_R:
            io_block = AUDIO_AFE_IO_BLOCK_ADDA_DL_CH2;
            break;
        case HAL_AUDIO_DEVICE_DAC_L:
        case HAL_AUDIO_DEVICE_DAC_DUAL:
            io_block = AUDIO_AFE_IO_BLOCK_ADDA_DL;
            break;
        case HAL_AUDIO_DEVICE_EXT_CODEC://TEMP!!
        case HAL_AUDIO_DEVICE_I2S_MASTER:
            if (is_input) {
                io_block = AUDIO_AFE_IO_BLOCK_I2S0_IN + 2*offset;
            } else {
                io_block = AUDIO_AFE_IO_BLOCK_I2S0_OUT + 2*offset;
            }
            break;
        case HAL_AUDIO_DEVICE_I2S_SLAVE:
            break;
    }
    return io_block;
    #endif
}

afe_sidetone_path_t afe_get_sidetone_output_path_by_audio_device (hal_audio_device_t device, hal_audio_interface_t interface)
{
    UNUSED(device);
    UNUSED(interface);
    return 0;
    #if 0//modify for ab1568
    afe_sidetone_path_t sidetone_path;
    if (device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        switch (interface) {
            default:
            case HAL_AUDIO_INTERFACE_1:
                sidetone_path = AFE_SIDETONE_I2S0;
                break;
            case HAL_AUDIO_INTERFACE_2:
                sidetone_path = AFE_SIDETONE_I2S1;
                break;
            case HAL_AUDIO_INTERFACE_3:
                sidetone_path = AFE_SIDETONE_I2S2;
                break;
            case HAL_AUDIO_INTERFACE_4:
                sidetone_path = AFE_SIDETONE_I2S3;
                break;
        }
    } else {
        sidetone_path = AFE_SIDETONE_DL;
    }
    return sidetone_path;
    #endif
}

afe_stream_channel_t afe_get_stream_channel_by_audio_device (hal_audio_channel_selection_t channel)
{
    afe_stream_channel_t stream_channel;
    switch (channel) {
        default:
        case HAL_AUDIO_DIRECT:
        case HAL_AUDIO_SWAP_L_R:     // need extra modification
            stream_channel = STREAM_S_AFE_S;
            break;
        case HAL_AUDIO_MIX_L_R:
        case HAL_AUDIO_MIX_SHIFT_L_R:// need extra modification
            stream_channel = STREAM_B_AFE_B;
            break;
        case HAL_AUDIO_BOTH_L:
        case HAL_AUDIO_BOTH_R:       // need extra modification
            stream_channel = STREAM_M_AFE_S;
            break;
    }
    return stream_channel;
}
extern hal_audio_bias_selection_t micbias_para_convert(uint32_t  in_misc_parms);
extern hal_audio_interconn_selection_t stream_audio_convert_control_to_interconn(hal_audio_control_t audio_control, hal_audio_path_port_parameter_t port_parameter, uint32_t connection_sequence, bool is_input);

#ifdef AIR_SIDETONE_ENABLE
void sidetone_tone_stop_entry(void){
    DTM_enqueue(DTM_EVENT_ID_SIDETONE_STOP, 0, false);
    DSP_MW_LOG_I("sidetone_tone_stop_entry",0);
}
void sidetone_in_device_para(hal_audio_device_parameter_t *device_handle_in,afe_sidetone_param_t* param){
    hal_audio_i2s_word_length_t word_length = HAL_AUDIO_I2S_WORD_LENGTH_16BIT;
    if ((device_handle_in->common.audio_device)&HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL) {
        device_handle_in->analog_mic.mic_interface = param->in_interface;
        device_handle_in->analog_mic.bias_voltage[0] = (param->in_misc_parms)&0xF;
        device_handle_in->analog_mic.bias_voltage[1] = ((param->in_misc_parms)>>4)&0xF;
        device_handle_in->analog_mic.bias_voltage[2] = ((param->in_misc_parms)>>8)&0xF;
        device_handle_in->analog_mic.bias_voltage[3] = ((param->in_misc_parms)>>12)&0xF;
        device_handle_in->analog_mic.bias_voltage[4] = ((param->in_misc_parms)>>16)&0xF;
        device_handle_in->analog_mic.bias_select = (param->in_misc_parms) >> 20;//micbias_para_convert(param->in_misc_parms);//param.in_misc_parms.MicbiasSourceType;//HAL_AUDIO_BIAS_SELECT_ALL;
        device_handle_in->analog_mic.iir_filter = HAL_AUDIO_UL_IIR_5HZ_AT_48KHZ;
        device_handle_in->analog_mic.with_external_bias = false;
        device_handle_in->analog_mic.with_bias_lowpower = false;
        device_handle_in->analog_mic.bias1_2_with_LDO0 = false;
        device_handle_in->analog_mic.adc_parameter.adc_mode = HAL_AUDIO_ANALOG_INPUT_ACC10K;
        device_handle_in->analog_mic.adc_parameter.performance = AFE_PEROFRMANCE_NORMAL_MODE;
        DSP_MW_LOG_I("sidetone dev 0x%x,if 0x%x,bis_v 0x%x,bis_sel 0x%x,iir 0x%x,ext 0x%x,bis_Low 0x%x,1_2_LDO 0x%x,adc_md 0x%x,pfm 0x%x",10,
            device_handle_in->common.audio_device,device_handle_in->analog_mic.mic_interface,device_handle_in->analog_mic.bias_voltage,
            device_handle_in->analog_mic.bias_select,device_handle_in->analog_mic.iir_filter,device_handle_in->analog_mic.with_external_bias ,
            device_handle_in->analog_mic.with_bias_lowpower,device_handle_in->analog_mic.bias1_2_with_LDO0,device_handle_in->analog_mic.adc_parameter.adc_mode,
            device_handle_in->analog_mic.adc_parameter.performance);
    } else if ((device_handle_in->common.audio_device)&HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL) {
        device_handle_in->linein.bias_voltage[0] = (param->in_misc_parms)&0xF;
        device_handle_in->linein.bias_voltage[1] = ((param->in_misc_parms)>>4)&0xF;
        device_handle_in->linein.bias_voltage[2] = ((param->in_misc_parms)>>8)&0xF;
        device_handle_in->linein.bias_voltage[3] = ((param->in_misc_parms)>>12)&0xF;
        device_handle_in->linein.bias_voltage[4] = ((param->in_misc_parms)>>16)&0xF;
        device_handle_in->linein.bias_select = (param->in_misc_parms) >> 20;//micbias_para_convert(param->in_misc_parms.MicbiasSourceType);//HAL_AUDIO_BIAS_SELECT_ALL;
        device_handle_in->linein.iir_filter = HAL_AUDIO_UL_IIR_DISABLE;
        device_handle_in->linein.adc_parameter.adc_mode = HAL_AUDIO_ANALOG_INPUT_ACC10K;
        device_handle_in->linein.adc_parameter.performance = AFE_PEROFRMANCE_NORMAL_MODE;
    } else if ((device_handle_in->common.audio_device)&HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL) {
        device_handle_in->digital_mic.mic_interface = param->in_interface;
        device_handle_in->digital_mic.dmic_selection = HAL_AUDIO_DMIC_GPIO_DMIC0;
        device_handle_in->digital_mic.adc_parameter.performance = AFE_PEROFRMANCE_NORMAL_MODE;
        device_handle_in->digital_mic.bias_voltage[0] = (param->in_misc_parms)&0xF;
        device_handle_in->digital_mic.bias_voltage[1] = ((param->in_misc_parms)>>4)&0xF;
        device_handle_in->digital_mic.bias_voltage[2] = ((param->in_misc_parms)>>8)&0xF;
        device_handle_in->digital_mic.bias_voltage[3] = ((param->in_misc_parms)>>12)&0xF;
        device_handle_in->digital_mic.bias_voltage[4] = ((param->in_misc_parms)>>16)&0xF;
        device_handle_in->digital_mic.bias_select = (param->in_misc_parms) >> 20;//micbias_para_convert(param->in_misc_parms.MicbiasSourceType);//HAL_AUDIO_BIAS_SELECT_ALL;
        device_handle_in->digital_mic.iir_filter = HAL_AUDIO_UL_IIR_DISABLE;
        device_handle_in->digital_mic.with_external_bias = false;
        device_handle_in->digital_mic.with_bias_lowpower = false;
        device_handle_in->digital_mic.bias1_2_with_LDO0 = false;
        DSP_MW_LOG_I("DIGITAL_MIC in_interface %d,dmic_selection %d, bias_voltage %d",3,device_handle_in->digital_mic.mic_interface,device_handle_in->digital_mic.dmic_selection,device_handle_in->digital_mic.bias_voltage[0]);

    } else if ((device_handle_in->common.audio_device)&HAL_AUDIO_CONTROL_DEVICE_VAD) {

    } else if ((device_handle_in->common.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER) || (device_handle_in->common.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L) || (device_handle_in->common.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R)) {
        device_handle_in->i2s_master.i2s_interface = param->in_interface;
        device_handle_in->i2s_master.i2s_format = HAL_AUDIO_I2S_I2S;
        device_handle_in->i2s_master.word_length = word_length;
        device_handle_in->i2s_master.mclk_divider = 2;
        device_handle_in->i2s_master.with_mclk = false;
        device_handle_in->i2s_master.is_low_jitter = false;
        device_handle_in->i2s_master.is_recombinant = false;
    } else if ((device_handle_in->common.audio_device)&HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        device_handle_in->i2s_slave.i2s_interface = param->in_interface;
        device_handle_in->i2s_slave.i2s_format = HAL_AUDIO_I2S_I2S;
        device_handle_in->i2s_slave.word_length = word_length;
        device_handle_in->i2s_slave.is_vdma_mode = false;
    }else if ((device_handle_in->common.audio_device)&HAL_AUDIO_CONTROL_DEVICE_SIDETONE) {
        hal_audio_path_port_parameter_t input_port_parameters, output_port_parameters;
        input_port_parameters.device_interface = param->in_interface;
        DSP_MW_LOG_I("in device_interface %d in_device %d,out device_interface %d out_device %d\r\n",4,input_port_parameters.device_interface,param->in_device,output_port_parameters.device_interface,param->out_device);
        device_handle_in->sidetone.input_device = param->in_device;
        device_handle_in->sidetone.input_interface = param->in_interface;
        device_handle_in->sidetone.output_device = param->out_device;
        device_handle_in->sidetone.output_interface =  param->out_interface;
        device_handle_in->sidetone.input_interconn_select = stream_audio_convert_control_to_interconn(param->in_device, input_port_parameters, 0, true);
        device_handle_in->sidetone.sidetone_gain = param->gain;
        device_handle_in->sidetone.p_sidetone_FIR_coef = param->FIR_nvdm_param;
        //DSP_MW_LOG_I("[M DEBUG][sidetone_in_device_para] 0x%x %x %d",3,&param->FIR_nvdm_param,param->FIR_nvdm_param,*(param->FIR_nvdm_param));
        device_handle_in->sidetone.is_sidetone_gain_register_value = false;
        #ifdef ENABLE_SIDETONE_RAMP_TIMER
        device_handle_in->sidetone.with_gain_ramp = true;
        #else
        device_handle_in->sidetone.with_gain_ramp = false;
        #endif
        device_handle_in->sidetone.sidetone_stop_done_entry = sidetone_tone_stop_entry;
        DSP_MW_LOG_I("sidetone with_gain_ramp %d,entry 0x%x sidetone_gain %d\r\n",3,device_handle_in->sidetone.with_gain_ramp,device_handle_in->sidetone.sidetone_stop_done_entry,device_handle_in->sidetone.sidetone_gain);

    }
}
void afe_set_sidetone_enable(bool enable, afe_sidetone_param_t *param, bool sidetone_rampdown_done_flag)
{
    //return 0;
    #if 0//modify for ab1568
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (enable) {
        hal_audio_afe_clock_on();
        afe_audio_device_enable(enable, param.in_device, param.in_interface, AFE_PCM_FORMAT_S16_LE, param.sample_rate, param.in_misc_parms);
        afe_audio_device_enable(enable, param.out_device, param.out_interface, AFE_PCM_FORMAT_S16_LE, param.sample_rate, param.out_misc_parms);
        afe_set_sidetone_input_channel(param.in_device);
        afe_set_sidetone_input_path(afe_get_digital_block_by_audio_device(param.in_device, param.in_interface, true));
        afe_set_sidetone_output_path(afe_get_sidetone_output_path_by_audio_device(param.out_device, param.out_interface), enable);

        hal_audio_afe_set_intf_connection_state(AUDIO_INTERCONNECTION_CONNECT,
                                                afe_get_stream_channel_by_audio_device(param.in_channel),
                                                afe_get_io_block_by_audio_device(param.in_device, param.in_interface, true),
                                                AUDIO_AFE_IO_BLOCK_STF);

        afe_set_sidetone_filter(enable);
    } else {
        afe_set_sidetone_filter(enable);
        hal_audio_afe_set_intf_connection_state(AUDIO_INTERCONNECTION_DISCONNECT,
                                                afe_get_stream_channel_by_audio_device(param.in_channel),
                                                afe_get_io_block_by_audio_device(param.in_device, param.in_interface, true),
                                                AUDIO_AFE_IO_BLOCK_STF);
        afe_set_sidetone_output_path(AFE_SIDETONE_DL, enable);
        afe_audio_device_enable(enable, param.in_device, param.in_interface, AFE_PCM_FORMAT_S16_LE, param.sample_rate, param.in_misc_parms);
        afe_audio_device_enable(enable, param.out_device, param.out_interface, AFE_PCM_FORMAT_S16_LE, param.sample_rate, param.out_misc_parms);
        hal_audio_afe_clock_off();
    }
    hal_nvic_restore_interrupt_mask(mask);
    #else
    hal_audio_device_parameter_t *device_handle_in = &param->device_handle_in;//modify for ab1568
    hal_audio_device_parameter_t *device_handle_out= &param->device_handle_out;//modify for ab1568
    hal_audio_device_parameter_t *device_handle_in_side_tone= &param->device_handle_in_side_tone;//modify for ab1568

    if(enable){

        device_handle_in->common.rate = param->sample_rate;
        device_handle_in->common.audio_device = param->in_device;
        sidetone_in_device_para(device_handle_in,param);

        //MIC
        hal_audio_set_device(device_handle_in, device_handle_in->common.audio_device, HAL_AUDIO_CONTROL_ON);
        DSP_MW_LOG_I("[SIDETONE] MIC set done %d rate %d\r\n",2,device_handle_in->common.audio_device,device_handle_in->common.rate);
        //Sidetone
        device_handle_in_side_tone->sidetone.rate = param->sample_rate;
        device_handle_in_side_tone->sidetone.audio_device = HAL_AUDIO_CONTROL_DEVICE_SIDETONE;
        sidetone_in_device_para(device_handle_in_side_tone,param);
        hal_audio_set_device(device_handle_in_side_tone, device_handle_in_side_tone->sidetone.audio_device, HAL_AUDIO_CONTROL_ON);
        DSP_MW_LOG_I("[SIDETONE] Sidetone set done %d, rate=%d\r\n",2,device_handle_in_side_tone->sidetone.audio_device,device_handle_in_side_tone->sidetone.rate);

        //output device
        device_handle_out->common.audio_device = param->out_device;
        device_handle_out->common.rate = param->sample_rate;
        if ((device_handle_out->common.audio_device) & HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL) {
            device_handle_out->dac.dac_mode = afe.stream_out.dac_mode;
            device_handle_out->dac.dc_compensation_value = afe.stream_out.dc_compensation_value;
            device_handle_out->dac.with_high_performance = false;
            device_handle_out->dac.with_phase_inverse = false;
            device_handle_out->dac.with_force_change_rate = false;
        } else if ((device_handle_out->common.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER) || (device_handle_out->common.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L) || (device_handle_out->common.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R)) {
            device_handle_out->i2s_master.i2s_interface = param->out_interface;//HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
            device_handle_out->i2s_master.i2s_format = HAL_AUDIO_I2S_I2S;
            device_handle_out->i2s_master.word_length = HAL_AUDIO_I2S_WORD_LENGTH_16BIT;
            device_handle_out->i2s_master.mclk_divider = 2;
            device_handle_out->i2s_master.with_mclk = false;
            device_handle_out->i2s_master.is_low_jitter = false;
            device_handle_out->i2s_master.is_recombinant = false;
        } else if ((device_handle_out->common.audio_device) & HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
            device_handle_out->i2s_slave.i2s_interface = param->out_interface;//HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
            device_handle_out->i2s_slave.i2s_format = HAL_AUDIO_I2S_I2S;
            device_handle_out->i2s_slave.word_length = HAL_AUDIO_I2S_WORD_LENGTH_16BIT;
            device_handle_out->i2s_slave.is_vdma_mode = false;
        } else if ((device_handle_out->common.audio_device) & HAL_AUDIO_CONTROL_DEVICE_SPDIF) {
            device_handle_out->spdif.i2s_setting.i2s_interface = param->out_interface;//HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
            device_handle_out->spdif.i2s_setting.i2s_format = HAL_AUDIO_I2S_I2S;
            device_handle_out->spdif.i2s_setting.word_length = HAL_AUDIO_I2S_WORD_LENGTH_16BIT;
            device_handle_out->spdif.i2s_setting.mclk_divider = 2;
            device_handle_out->spdif.i2s_setting.with_mclk = false;
            device_handle_out->spdif.i2s_setting.is_low_jitter = false;
            device_handle_out->spdif.i2s_setting.is_recombinant = false;
        }
        DSP_MW_LOG_I("[SIDETONE] device_handle_out %d out_device %d, rate %d",3,device_handle_out->common.audio_device,param->out_device,device_handle_out->common.rate);
        hal_audio_set_device(device_handle_out, device_handle_out->common.audio_device, HAL_AUDIO_CONTROL_ON);
    }else{
            if (sidetone_rampdown_done_flag == true) {
                hal_audio_set_device(device_handle_in, device_handle_in->common.audio_device, HAL_AUDIO_CONTROL_OFF);
                hal_audio_set_device(device_handle_out, device_handle_out->common.audio_device, HAL_AUDIO_CONTROL_OFF);
            } else {
                hal_audio_set_device(device_handle_in_side_tone, device_handle_in_side_tone->sidetone.audio_device, HAL_AUDIO_CONTROL_OFF);
            }
    }
    #endif
}
#endif

void afe_set_loopback_enable(bool enable, afe_loopback_param_p param)
{
    uint32_t i;

    hal_audio_device_parameter_t *device_handle_in = &param->device_handle_in;//modify for ab1568
    hal_audio_device_parameter_t *device_handle_out= &param->device_handle_out;//modify for ab1568
    hal_audio_path_parameter_t *path_handle = &param->path_handle;;
    if(enable){
    //path
    path_handle->audio_path_input_rate = param->sample_rate;//afe_get_audio_device_samplerate(pAudPara->audio_device, pAudPara->audio_interface);
    path_handle->audio_path_output_rate = param->sample_rate;//afe_get_audio_device_samplerate(pAudPara->audio_device, pAudPara->audio_interface);
    path_handle->connection_selection = param->stream_channel;//pAudPara->stream_channel;
    path_handle->connection_number = param->stream_channel;
    path_handle->connection_number = 2;


    hal_audio_path_port_parameter_t input_port_parameters, output_port_parameters;
    input_port_parameters.device_interface = param->in_interface;
    output_port_parameters.device_interface = param->out_interface;
    DSP_MW_LOG_I("in device_interface %d in_device %d,out device_interface %d out_device %d\r\n",4,input_port_parameters.device_interface,param->in_device,output_port_parameters.device_interface,param->out_device);
    for (i=0 ; i<path_handle->connection_number ; i++) {
        path_handle->input.interconn_sequence[i]  = stream_audio_convert_control_to_interconn(param->in_device, input_port_parameters, i, true);
        path_handle->output.interconn_sequence[i] = stream_audio_convert_control_to_interconn(param->out_device, output_port_parameters, i, false);
    }
    path_handle->with_hw_gain = param->with_hw_gain ;
    path_handle->with_upwdown_sampler = false;

#ifdef  AIR_AUDIO_HW_LOOPBACK_ENABLE
if((param->in_device)&(HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L|HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R| \
HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_L|HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_R))
{
    path_handle->connection_number = 1;
    path_handle->with_upwdown_sampler = true;
    path_handle->audio_path_input_rate = 16000;
    path_handle->audio_path_output_rate = 48000;
}
#endif
    //for hal_audio_set_device
    if ((param->in_device)&(HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L|HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R| \
    HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL|HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_ANC| \
    HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER|HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L|HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R| \
    HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_L|HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_R)) {
        device_handle_in->common.rate = param->sample_rate;
        DSP_MW_LOG_I("set device common.rate %d,source rate %d",2,device_handle_in->common.rate,param->sample_rate);
    }

    #if 1//modify for ab1568
    device_handle_in->common.audio_device = param->in_device;
    if ((param->in_device)&(HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L|HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R| \
    HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_L|HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_R)) {
        device_handle_in->analog_mic.rate = param->sample_rate;//AUDIO_SOURCE_DEFAULT_ANALOG_VOICE_RATE;
        device_handle_in->analog_mic.with_external_bias = false;
        device_handle_in->analog_mic.with_bias_lowpower = false;
        device_handle_in->analog_mic.bias1_2_with_LDO0 = false;
        DSP_MW_LOG_I("ANALOG_MIC in_interface %d,adc_mode %d,performance %d",3,device_handle_in->analog_mic.mic_interface,device_handle_in->analog_mic.adc_parameter.adc_mode,device_handle_in->analog_mic.adc_parameter.performance);
    } else if ((param->in_device)&HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL) {
        device_handle_in->linein.rate =  param->sample_rate;//AUDIO_SOURCE_DEFAULT_ANALOG_AUDIO_RATE;
    } else if ((param->in_device)&HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL) {
        device_handle_in->digital_mic.rate = param->sample_rate;//AUDIO_SOURCE_DEFAULT_ANALOG_VOICE_RATE;
        device_handle_in->digital_mic.with_external_bias = false;
        device_handle_in->digital_mic.with_bias_lowpower = false;
        device_handle_in->digital_mic.bias1_2_with_LDO0 = false;
        DSP_MW_LOG_I("DIGITAL_MIC in_interface %d,dmic_selection %d, bias_voltage %d",3,device_handle_in->digital_mic.mic_interface,device_handle_in->digital_mic.dmic_selection,device_handle_in->digital_mic.bias_voltage[0]);
        //printf("DIGITAL_MIC in_interface %d,dmic_selection %d, bias_voltage %d",device_handle_in->digital_mic.mic_interface,device_handle_in->digital_mic.dmic_selection,device_handle_in->digital_mic.bias_voltage);
} else if ((param->in_device)&HAL_AUDIO_CONTROL_DEVICE_VAD) {
        device_handle_in->vad.rate = param->sample_rate;;
    } else if ((param->in_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER) || (param->in_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L) || (param->in_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R)) {
        device_handle_in->i2s_master.rate = param->sample_rate;//48000;
        device_handle_in->i2s_master.i2s_format = HAL_AUDIO_I2S_I2S;
        device_handle_in->i2s_master.word_length = HAL_AUDIO_I2S_WORD_LENGTH_32BIT;
        device_handle_in->i2s_master.mclk_divider = 2;
        device_handle_in->i2s_master.with_mclk = false;
        device_handle_in->i2s_master.i2s_interface = input_port_parameters.device_interface;
        if(param->in_misc_parms.I2sClkSourceType ==I2S_CLK_SOURCE_APLL){
            device_handle_in->i2s_master.is_low_jitter = true;
        }else{
            device_handle_in->i2s_master.is_low_jitter = false;
        }
        device_handle_in->i2s_master.is_recombinant = false;
    } else if (param->in_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        device_handle_in->i2s_slave.rate = param->sample_rate;//48000;
        device_handle_in->i2s_slave.i2s_interface = input_port_parameters.device_interface;
        device_handle_in->i2s_slave.i2s_format = HAL_AUDIO_I2S_I2S;
        device_handle_in->i2s_slave.word_length = HAL_AUDIO_I2S_WORD_LENGTH_32BIT;
        device_handle_in->i2s_slave.is_vdma_mode = false;
    }

    device_handle_out->common.audio_device = param->out_device;
    if (param->out_device & HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL) {
        device_handle_out->dac.rate = param->sample_rate;
        device_handle_out->dac.dac_mode= afe.stream_out.dac_mode;
//#ifdef ANALOG_OUTPUT_CLASSD_ENABLE
//        device_handle_out->dac.dac_mode= HAL_AUDIO_ANALOG_OUTPUT_CLASSD;
//#else
//        device_handle_out->dac.dac_mode= HAL_AUDIO_ANALOG_OUTPUT_CLASSAB;
//#endif
        device_handle_out->dac.dc_compensation_value = afe.stream_out.dc_compensation_value;
#ifdef  LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA
        device_handle_out->dac.with_high_performance = param->device_handle_out.dac.with_high_performance;
        DSP_MW_LOG_I("DAC with_high_performance %d dc_compensation_value 0x%x dac_mode 0x%x",3,device_handle_out->dac.with_high_performance,device_handle_out->dac.dc_compensation_value,device_handle_out->dac.dac_mode);
#else
        device_handle_out->dac.with_high_performance = false;
#endif
        device_handle_out->dac.with_force_change_rate = false;
    } else if ((param->out_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER) || (param->out_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L) || (param->out_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R)) {
        device_handle_out->i2s_master.rate = param->sample_rate;
#ifdef  LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA
        device_handle_out->i2s_master.i2s_interface = param->out_interface;
#else
        device_handle_out->i2s_master.i2s_interface = param->out_interface;
#endif
        device_handle_out->i2s_master.i2s_format = HAL_AUDIO_I2S_I2S;
        device_handle_out->i2s_master.word_length = HAL_AUDIO_I2S_WORD_LENGTH_32BIT;
        device_handle_out->i2s_master.mclk_divider = 2;
        device_handle_out->i2s_master.with_mclk = false;
        if(param->out_misc_parms.I2sClkSourceType == I2S_CLK_SOURCE_APLL){
            device_handle_out->i2s_master.is_low_jitter = true;
        }else{
            device_handle_out->i2s_master.is_low_jitter = false;
        }
        device_handle_out->i2s_master.is_recombinant = false;
    } else if (param->in_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        device_handle_out->i2s_slave.rate = param->sample_rate;
        device_handle_out->i2s_slave.i2s_interface = output_port_parameters.device_interface;
        device_handle_out->i2s_slave.i2s_format = HAL_AUDIO_I2S_I2S;
        device_handle_out->i2s_slave.word_length = HAL_AUDIO_I2S_WORD_LENGTH_32BIT;
        device_handle_out->i2s_slave.is_vdma_mode = false;
    } else if (param->out_device & HAL_AUDIO_CONTROL_DEVICE_SPDIF) {
        device_handle_out->spdif.i2s_setting.rate = param->sample_rate;
        device_handle_out->spdif.i2s_setting.i2s_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
        device_handle_out->spdif.i2s_setting.i2s_format = HAL_AUDIO_I2S_I2S;
        device_handle_out->spdif.i2s_setting.word_length = HAL_AUDIO_I2S_WORD_LENGTH_32BIT;
        device_handle_out->spdif.i2s_setting.mclk_divider = 2;
        device_handle_out->spdif.i2s_setting.with_mclk = false;
        if(param->out_misc_parms.I2sClkSourceType == I2S_CLK_SOURCE_APLL){
            device_handle_out->spdif.i2s_setting.is_low_jitter = true;
        }else{
            device_handle_out->spdif.i2s_setting.is_low_jitter = false;
        }
        device_handle_out->spdif.i2s_setting.is_recombinant = false;
        device_handle_out->spdif.i2s_setting.is_rx_swap = false;
        device_handle_out->spdif.i2s_setting.is_tx_swap = false;
        device_handle_out->spdif.i2s_setting.is_internal_loopback = false;
    }
}
    if(enable){
            hal_audio_set_path(path_handle, HAL_AUDIO_CONTROL_ON);
            #if 0
                //Sine generator for FGPA verification TEMP!!!
                hal_audio_sine_generator_parameter_t sine_generator;
                sine_generator.enable = true;
                sine_generator.rate = device_handle_in->common.rate;
                sine_generator.audio_control = device_handle_in->common.audio_device;
                sine_generator.port_parameter.device_interface = device_handle_in->common.device_interface;
                sine_generator.is_input_port = true;
                DSP_MW_LOG_I("in audio_controol %d rate %d audio_interface %d sine_generator.rate %d\r\n",4,sine_generator.audio_control,sine_generator.rate,sine_generator.port_parameter.device_interface,sine_generator.rate);
                hal_audio_set_value((hal_audio_set_value_parameter_t *)&sine_generator, HAL_AUDIO_SET_SINE_GENERATOR);
            #endif
            #if 0
                //Sine generator for FGPA verification TEMP!!!
                //hal_audio_sine_generator_parameter_t sine_generator;
                hal_audio_sine_generator_parameter_t sine_generator;
                sine_generator.enable = true;
                sine_generator.rate = device_handle_out->common.rate;
                sine_generator.audio_control = device_handle_out->common.audio_device;
                sine_generator.port_parameter.device_interface = device_handle_out->common.device_interface;
                sine_generator.is_input_port = false;
                DSP_MW_LOG_I("out audio_controol %d rate %d audio_interface %d sine_generator.rate %d\r\n",4,sine_generator.audio_control,sine_generator.rate,sine_generator.port_parameter.device_interface,sine_generator.rate);
                hal_audio_set_value((hal_audio_set_value_parameter_t *)&sine_generator, HAL_AUDIO_SET_SINE_GENERATOR);
            #endif
            #if 0//Desense data from DL1 to I2S master out and config DMIC UL IIR to 5HZ_AT_48KHZ
            DSP_MW_LOG_I("data from DL1 to I2S master out indev %d,outdev %d\r\n",2,param->in_device,param->out_device);
            device_handle_in->digital_mic.iir_filter = HAL_AUDIO_UL_IIR_5HZ_AT_48KHZ;
            hal_audio_memory_parameter_t mem_handle;
            hal_audio_control_t memory_interface = HAL_AUDIO_CONTROL_MEMORY_INTERFACE;
            mem_handle.audio_path_rate = param->sample_rate;
            mem_handle.buffer_length = 7680;//96KHz 10ms
            mem_handle.buffer_addr = (uint32_t)NULL;
            mem_handle.initial_buffer_offset = 0;
            mem_handle.irq_counter = 960;////96KHz 10ms
            mem_handle.memory_select = HAL_AUDIO_MEMORY_DL_DL1;
            mem_handle.pcm_format = HAL_AUDIO_PCM_FORMAT_S32_LE;
            mem_handle.pure_agent_with_src = false;
            mem_handle.sync_status = HAL_AUDIO_MEMORY_SYNC_NONE;

            hal_audio_set_memory(&mem_handle, memory_interface, HAL_AUDIO_CONTROL_ON);
            if(mem_handle.buffer_addr != (uint32_t)NULL){
                memset((U8*)mem_handle.buffer_addr,0xAA,mem_handle.buffer_length);
            }else{
                DSP_MW_LOG_I("mem_handle.buffer_addr 0x%x\r\n",1,mem_handle.buffer_addr);
            }
            path_handle->audio_path_input_rate = param->sample_rate;//afe_get_audio_device_samplerate(pAudPara->audio_device, pAudPara->audio_interface);
            path_handle->audio_path_output_rate = param->sample_rate;//afe_get_audio_device_samplerate(pAudPara->audio_device, pAudPara->audio_interface);
            path_handle->connection_selection = param->stream_channel;//pAudPara->stream_channel;
            path_handle->connection_number = param->stream_channel;
            path_handle->connection_number = 2;

            hal_audio_path_port_parameter_t input_port_parameters, output_port_parameters;
            //input_port_parameters.device_interface = param->in_interface;
            input_port_parameters.memory_select = HAL_AUDIO_MEMORY_DL_DL1;
            output_port_parameters.device_interface = param->in_interface;

            for (i=0 ; i<path_handle->connection_number ; i++) {
            path_handle->input.interconn_sequence[i]  = stream_audio_convert_control_to_interconn(HAL_AUDIO_CONTROL_MEMORY_INTERFACE, input_port_parameters, i, true);
            path_handle->output.interconn_sequence[i] = stream_audio_convert_control_to_interconn(HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER, output_port_parameters, i, false);
            }
            path_handle->with_hw_gain = false;
            path_handle->with_upwdown_sampler = false;
            hal_audio_set_path(path_handle, HAL_AUDIO_CONTROL_ON);
            #endif
            hal_audio_set_device(device_handle_in, param->in_device, HAL_AUDIO_CONTROL_ON);
            hal_audio_set_device(device_handle_out, param->out_device, HAL_AUDIO_CONTROL_ON);

    }else{
#ifdef  LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA
            DSP_MW_LOG_I("dac mode %d\r\n",1,device_handle_out->dac.dac_mode);
#endif
            hal_audio_set_path(path_handle, HAL_AUDIO_CONTROL_OFF);
            hal_audio_set_device(device_handle_in, param->in_device, HAL_AUDIO_CONTROL_OFF);
            hal_audio_set_device(device_handle_out, param->out_device, HAL_AUDIO_CONTROL_OFF);
    }

    #else
    uint32_t mask;
    audio_afe_io_block_t input_io = afe_get_io_block_by_audio_device(param->in_device, param->in_interface, true);
    audio_afe_io_block_t output_io = afe_get_io_block_by_audio_device(param->out_device, param->out_interface, false);
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (enable) {
        hal_audio_afe_clock_on();
        afe_audio_device_enable(enable, param->in_device, param->in_interface, param->format, param->sample_rate, param->in_misc_parms);
        afe_audio_device_enable(enable, param->out_device, param->out_interface, param->format, param->sample_rate, param->out_misc_parms);

        if (param->with_hw_gain) {
            afe_set_hardware_digital_gain_mode(AFE_HW_DIGITAL_GAIN1, afe_get_audio_device_samplerate(param->out_device, param->out_interface));
            afe_enable_hardware_digital_gain(AFE_HW_DIGITAL_GAIN1, true);

            if (param->format >= AFE_PCM_FORMAT_S24_LE) {
                hal_audio_afe_set_connection_format(AFE_OUTPUT_DATA_FORMAT_24BIT, AUDIO_AFE_IO_BLOCK_HW_GAIN1_IN);
            } else  {
                hal_audio_afe_set_connection_format(AFE_OUTPUT_DATA_FORMAT_16BIT, AUDIO_AFE_IO_BLOCK_HW_GAIN1_IN);
            }
            hal_audio_afe_set_intf_connection_state(AUDIO_INTERCONNECTION_CONNECT, STREAM_S_AFE_S, input_io, AUDIO_AFE_IO_BLOCK_HW_GAIN1_IN);
            input_io = AUDIO_AFE_IO_BLOCK_HW_GAIN1_OUT;
        }

        if (param->format >= AFE_PCM_FORMAT_S24_LE) {
            hal_audio_afe_set_connection_format(AFE_OUTPUT_DATA_FORMAT_24BIT, output_io);
        } else  {
            hal_audio_afe_set_connection_format(AFE_OUTPUT_DATA_FORMAT_16BIT, output_io);
        }
        hal_audio_afe_set_intf_connection_state(AUDIO_INTERCONNECTION_CONNECT,
                                                afe_get_stream_channel_by_audio_device(param->stream_channel),
                                                input_io,
                                                output_io);

    } else {

        if (param->with_hw_gain) {
            afe_enable_hardware_digital_gain(AFE_HW_DIGITAL_GAIN1, false);

            hal_audio_afe_set_intf_connection_state(AUDIO_INTERCONNECTION_DISCONNECT, STREAM_S_AFE_S, input_io, AUDIO_AFE_IO_BLOCK_HW_GAIN1_IN);
            input_io = AUDIO_AFE_IO_BLOCK_HW_GAIN1_OUT;
        }
        hal_audio_afe_set_intf_connection_state(AUDIO_INTERCONNECTION_DISCONNECT,
                                                afe_get_stream_channel_by_audio_device(param->stream_channel),
                                                input_io,
                                                output_io);

        afe_audio_device_enable(enable, param->in_device, param->in_interface, param->format, param->sample_rate, param->in_misc_parms);
        afe_audio_device_enable(enable, param->out_device, param->out_interface, param->format, param->sample_rate, param->out_misc_parms);
        hal_audio_afe_clock_off();
    }
    hal_nvic_restore_interrupt_mask(mask);
    #endif
}


void afe_mute_digital_gain(bool mute, afe_digital_gain_t digital_gain)
{
    afe_hardware_digital_gain_t hw_digital_gain;

    if (digital_gain == AUDIO_HW_GAIN) {
        hw_digital_gain = AFE_HW_DIGITAL_GAIN1;
    } else {
        hw_digital_gain = AFE_HW_DIGITAL_GAIN2;
    }

    if(mute) {
        afe_set_hardware_digital_gain(hw_digital_gain, 0);
    } else if (!afe_get_hardware_digital_gain(hw_digital_gain)){
        afe_audio_set_output_digital_gain(digital_gain);
    }

}

afe_asrc_tracking_clock_t afe_set_asrc_tracking_clock(hal_audio_interface_t audio_interface)
{
    afe_asrc_tracking_clock_t tracking_clock;

    if (audio_interface == HAL_AUDIO_INTERFACE_2) {
        tracking_clock = MEM_ASRC_TRACKING_I2S2;
    } else if (audio_interface == HAL_AUDIO_INTERFACE_3) {
        tracking_clock = MEM_ASRC_TRACKING_I2S3;
    } else if (audio_interface == HAL_AUDIO_INTERFACE_4) {
        tracking_clock = MEM_ASRC_TRACKING_I2S4;
    } else {
        tracking_clock = MEM_ASRC_TRACKING_I2S1;
    }

    return tracking_clock;
}

void afe_set_asrc_enable(bool enable, afe_mem_asrc_id_t asrc_id, afe_asrc_config_p asrc_config)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (enable) {
        afe_asrc_clock_on(asrc_id);
        afe_power_on_asrc(asrc_id, true);
        afe_set_asrc_iir(asrc_id, asrc_config->input_buffer.rate, asrc_config->output_buffer.rate);
        afe_set_asrc(asrc_id, asrc_config, true);

    } else {
        afe_set_asrc(asrc_id, asrc_config, false);
        afe_power_on_asrc(asrc_id, false);
        afe_asrc_clock_off(asrc_id);
    }
    hal_nvic_restore_interrupt_mask(mask);
}
#endif /*HAL_AUDIO_MODULE_ENABLED*/
