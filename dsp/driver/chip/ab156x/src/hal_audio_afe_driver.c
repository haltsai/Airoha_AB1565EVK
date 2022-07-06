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
#include <stdint.h>
#include "ab156x.h"
#include "hal_audio_afe_control.h"
#include "hal_audio_afe_define.h"
#include "hal_audio_afe_connection.h"
#include "hal_audio_afe_clock.h"
#include "hal_nvic.h"
#include "hal_gpt.h"
#include "hal_log.h"
#include "audio_afe_common.h"

#define AUD_DSP0_IRQ_MASK    (0x7BFF)
#define ReadREG(_addr)          (*(volatile uint32_t *)(_addr))
#define HWGAIN_MUTE_VALUE    (0x80000000)
#define ANC_AFE_0DB_REGISTER_VALUE 32768

afe_t afe;

static int16_t aud_afe_set_cntr;

extern uint32_t afe_calculate_digital_gain_index(int32_t digital_gain_in_01unit_db, uint32_t digital_0db_register_value);

/* reg, bit position, bit mask */
static const uint32_t afe_memif_samplerate[AUDIO_DIGITAL_BLOCK_MEM_AWB2+1][3] = {
    [AUDIO_DIGITAL_BLOCK_MEM_DL1]  = {AFE_DAC_CON1, 0,  0xf},
    [AUDIO_DIGITAL_BLOCK_MEM_DL2]  = {AFE_DAC_CON1, 4,  0xf},
    [AUDIO_DIGITAL_BLOCK_MEM_DL3]  = {AFE_DAC_CON2, 8,  0xf},
    [AUDIO_DIGITAL_BLOCK_MEM_DL12] = {AFE_DAC_CON0, 16, 0xf},
    [AUDIO_DIGITAL_BLOCK_MEM_VUL1] = {AFE_DAC_CON1, 16, 0xf},
    [AUDIO_DIGITAL_BLOCK_MEM_VUL2] = {AFE_DAC_CON2, 4,  0xf},
    [AUDIO_DIGITAL_BLOCK_MEM_AWB]  = {AFE_DAC_CON1, 12, 0xf},
    [AUDIO_DIGITAL_BLOCK_MEM_AWB2] = {AFE_DAC_CON2, 16, 0xf},
};

/* reg, bit position, bit mask */
static const uint32_t afe_memif_channels[AUDIO_DIGITAL_BLOCK_MEM_AWB2+1][3] = {
    [AUDIO_DIGITAL_BLOCK_MEM_DL1]  = {AFE_DAC_CON1, 21, 0x1},
    [AUDIO_DIGITAL_BLOCK_MEM_DL2]  = {AFE_DAC_CON1, 22, 0x1},
    [AUDIO_DIGITAL_BLOCK_MEM_DL3]  = {AFE_DAC_CON1, 23, 0x1},
    [AUDIO_DIGITAL_BLOCK_MEM_DL12] = {AFE_DAC_CON1, 20, 0x1},
    [AUDIO_DIGITAL_BLOCK_MEM_VUL1] = {AFE_DAC_CON1, 27, 0x1},
    [AUDIO_DIGITAL_BLOCK_MEM_VUL2] = {AFE_DAC_CON2, 0,  0x1},
    [AUDIO_DIGITAL_BLOCK_MEM_AWB]  = {AFE_DAC_CON1, 24, 0x1},
    [AUDIO_DIGITAL_BLOCK_MEM_AWB2] = {AFE_DAC_CON2, 20, 0x1},
};

/* audio block, reg, bit position */
static const uint32_t afe_mem_block_enable_reg[][3] = {
    {AUDIO_DIGITAL_BLOCK_MEM_DL1,  AFE_DAC_CON0, 1 },
    {AUDIO_DIGITAL_BLOCK_MEM_DL2,  AFE_DAC_CON0, 2 },
    {AUDIO_DIGITAL_BLOCK_MEM_VUL1, AFE_DAC_CON0, 3 },
    {AUDIO_DIGITAL_BLOCK_MEM_DL3,  AFE_DAC_CON0, 5 },
    {AUDIO_DIGITAL_BLOCK_MEM_AWB,  AFE_DAC_CON0, 6 },
    {AUDIO_DIGITAL_BLOCK_MEM_DL12, AFE_DAC_CON0, 8 },
    {AUDIO_DIGITAL_BLOCK_MEM_VUL2, AFE_DAC_CON0, 27},
    {AUDIO_DIGITAL_BLOCK_MEM_AWB2, AFE_DAC_CON0, 29},
};

static const uint32_t AFE_MEM_BLOCK_ENABLE_REG_NUM = ARRAY_SIZE(afe_mem_block_enable_reg);

const afe_irq_ctrl_reg_t afe_irq_control_regs[AFE_IRQ_ALL_NUM] = {
    {/*IRQ0*/
        {AFE_IRQ_MCU_CON0,   0, 0x1},      /* irq on */
        {AFE_IRQ_MCU_CON1,   0, 0xf},      /* irq mode */
        {AFE_IRQ_MCU_CNT0,   0, 0x3ffff},  /* irq count */
        {AFE_IRQ_MCU_CLR,    0, 0x1},      /* irq clear */
        {AFE_IRQ_MCU_CLR,    16,0x1},      /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 0, 0x1},      /* irq status */
        {AFE_IRQ_MCU_EN1,    0, 0x1},      /* irq enable */
        true
    },
    {/*IRQ1*/
        {AFE_IRQ_MCU_CON0,   1, 0x1},      /* irq on */
        {AFE_IRQ_MCU_CON1,   4, 0xf},      /* irq mode */
        {AFE_IRQ_MCU_CNT1,   0, 0x3ffff},  /* irq count */
        {AFE_IRQ_MCU_CLR,    1, 0x1},      /* irq clear */
        {AFE_IRQ_MCU_CLR,    17,0x1},      /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 1, 0x1},      /* irq status */
        {AFE_IRQ_MCU_EN1,    1, 0x1},      /* irq enable */
        true
    },
    {/*IRQ2*/
        {AFE_IRQ_MCU_CON0,   2, 0x1},      /* irq on */
        {AFE_IRQ_MCU_CON1,   8, 0xf},      /* irq mode */
        {AFE_IRQ_MCU_CNT2,   0, 0x3ffff},  /* irq count */
        {AFE_IRQ_MCU_CLR,    2, 0x1},      /* irq clear */
        {AFE_IRQ_MCU_CLR,    18,0x1},      /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 2, 0x1},      /* irq status */
        {AFE_IRQ_MCU_EN1,    2, 0x1},      /* irq enable */
        true
    },
    {/*IRQ3*/
        {AFE_IRQ_MCU_CON0,   3, 0x1},      /* irq on */
        {AFE_IRQ_MCU_CON1,   12,0xf},      /* irq mode */
        {AFE_IRQ_MCU_CNT3,   0, 0x3ffff},  /* irq count */
        {AFE_IRQ_MCU_CLR,    3, 0x1},      /* irq clear */
        {AFE_IRQ_MCU_CLR,    19,0x1},      /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 3, 0x1},      /* irq status */
        {AFE_IRQ_MCU_EN1,    3, 0x1},      /* irq enable */
        true
    },
    {/*IRQ4*/
        {AFE_IRQ_MCU_CON0,   4, 0x1},      /* irq on */
        {AFE_IRQ_MCU_CON1,   16,0xf},      /* irq mode */
        {AFE_IRQ_MCU_CNT4,   0, 0x3ffff},  /* irq count */
        {AFE_IRQ_MCU_CLR,    4, 0x1},      /* irq clear */
        {AFE_IRQ_MCU_CLR,    20,0x1},      /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 4, 0x1},      /* irq status */
        {AFE_IRQ_MCU_EN1,    4, 0x1},      /* irq enable */
        true
    },
    {/*IRQ5*/
        {AFE_IRQ_MCU_CON0,   5, 0x1},      /* irq on */
        {AFE_IRQ_MCU_CON1,   20,0xf},      /* irq mode */
        {AFE_IRQ_MCU_CNT5,   0, 0x3ffff},  /* irq count */
        {AFE_IRQ_MCU_CLR,    5, 0x1},      /* irq clear */
        {AFE_IRQ_MCU_CLR,    21,0x1},      /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 5, 0x1},      /* irq status */
        {AFE_IRQ_MCU_EN1,    5, 0x1},      /* irq enable */
        true
    },
    {/*IRQ6*/
        {AFE_IRQ_MCU_CON0,   6, 0x1},      /* irq on */
        {AFE_IRQ_MCU_CON1,   24,0xf},      /* irq mode */
        {AFE_IRQ_MCU_CNT6,   0, 0x3ffff},  /* irq count */
        {AFE_IRQ_MCU_CLR,    6, 0x1},      /* irq clear */
        {AFE_IRQ_MCU_CLR,    22,0x1},      /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 6, 0x1},      /* irq status */
        {AFE_IRQ_MCU_EN1,    6, 0x1},      /* irq enable */
        true
    },
    {/*IRQ7*/
        {AFE_IRQ_MCU_CON0,   7, 0x1},      /* irq on */
        {AFE_IRQ_MCU_CON1,   28,0xf},      /* irq mode */
        {AFE_IRQ_MCU_CNT7,   0, 0x3ffff},  /* irq count */
        {AFE_IRQ_MCU_CLR,    7, 0x1},      /* irq clear */
        {AFE_IRQ_MCU_CLR,    23,0x1},      /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 7, 0x1},      /* irq status */
        {AFE_IRQ_MCU_EN1,    7, 0x1},      /* irq enable */
        true
    },
    {/*IRQ8*/ // ANC_DET_LCH_L_MCU_CLR
        {AFE_REG_UNDEFINED,  0, 0x1},      /* irq on */
        {AFE_REG_UNDEFINED,  0, 0x0},      /* irq mode */
        {AFE_REG_UNDEFINED,  0, 0x0},      /* irq count */
        {AFE_IRQ_MCU_CLR,    8, 0x1},      /* irq clear */
        {AFE_REG_UNDEFINED,  0 ,0x1},      /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 8, 0x1},      /* irq status */
        {AFE_IRQ_MCU_EN1,    8, 0x1},      /* irq enable */
        false                              /* irq enable */
    },
    {/*IRQ9*/ // ANC_DET_LCH_H_MCU_CLR
        {AFE_REG_UNDEFINED,  0, 0x1},      /* irq on */
        {AFE_REG_UNDEFINED,  0, 0x0},      /* irq mode */
        {AFE_REG_UNDEFINED,  0, 0x0},      /* irq count */
        {AFE_IRQ_MCU_CLR,    9, 0x1},      /* irq clear */
        {AFE_REG_UNDEFINED,  0, 0x1},      /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 9, 0x1},      /* irq status */
        {AFE_IRQ_MCU_EN1,    9, 0x1},      /* irq enable */
        true
    },
    {/*IRQ10*/
        {AFE_REG_UNDEFINED,  0, 0x1},      /* irq on */
        {AFE_REG_UNDEFINED,  0, 0x0},      /* irq mode */
        {AFE_REG_UNDEFINED,  0, 0x0},      /* irq count */
        {AFE_IRQ_MCU_CLR,    10,0x1},      /* irq clear */
        {AFE_REG_UNDEFINED,  0, 0x1},      /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 10,0x1},      /* irq status */
        {AFE_IRQ_MCU_EN1,    10,0x1},      /* irq enable */
        false
    },

    {/*IRQ11*/
        {AFE_IRQ_MCU_CON0,   11,0x1},      /* irq on */
        {AFE_IRQ_MCU_CON2,   0, 0xf},      /* irq mode */
        {AFE_IRQ_MCU_CNT11,  0, 0x3ffff},  /* irq count */
        {AFE_IRQ_MCU_CLR,    11,0x1},      /* irq clear */
        {AFE_IRQ_MCU_CLR,    27,0x1},      /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 11,0x1},      /* irq status */
        {AFE_IRQ_MCU_EN1,    11,0x1},      /* irq status */
        true                               /* irq enable */
    },
    {/*IRQ12*/
        {AFE_IRQ_MCU_CON0,   12,0x1},     /* irq on */
        {AFE_IRQ_MCU_CON2,   4, 0xf},     /* irq mode */
        {AFE_IRQ_MCU_CNT12,  0, 0x3ffff}, /* irq count */
        {AFE_IRQ_MCU_CLR,    12,0x1},     /* irq clear */
        {AFE_IRQ_MCU_CLR,    28,0x1},     /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 12,0x1},     /* irq status */
        {AFE_IRQ_MCU_EN1,    12,0x1},     /* irq enable */
        true
    },
    {/*IRQ13*/ // ANC_DET_RCH_L_MCU_CLR
        {AFE_REG_UNDEFINED,  0, 0x1},      /* irq on */
        {AFE_REG_UNDEFINED,  0, 0x0},      /* irq mode */
        {AFE_REG_UNDEFINED,  0, 0x0},      /* irq count */
        {AFE_IRQ_MCU_CLR,    13,0x1},      /* irq clear */
        {AFE_REG_UNDEFINED,  0 ,0x1},      /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 13,0x1},      /* irq status */
        {AFE_IRQ_MCU_EN1,    13,0x1},      /* irq enable */
        false
    },
    {/*IRQ14*/ // ANC_DET_RCH_H_MCU_CLR
        {AFE_REG_UNDEFINED,  0, 0x1},      /* irq on */
        {AFE_REG_UNDEFINED,  0, 0x0},      /* irq mode */
        {AFE_REG_UNDEFINED,  0, 0x0},      /* irq count */
        {AFE_IRQ_MCU_CLR,    14,0x1},      /* irq clear */
        {AFE_REG_UNDEFINED,  0, 0x1},      /* irq miss clear */
        {AFE_IRQ_MCU_STATUS, 14,0x1},      /* irq status */
        {AFE_IRQ_MCU_EN1,    14,0x1},      /* irq enable */
        true
    },
};
#if 0
/*Irq handler function array*/
static void afe_irq1_handler(void)
{
    if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_DL1)) {
        afe.func_handle->afe_dl1_interrupt_handler();
    }
    #if 0
        if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_DL2)) {
            afe.func_handle->afe_dl2_interrupt_handler();
        }
    #endif
    if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_DL3)) {
        //afe.func_handle->afe_dl3_interrupt_handler();
    }
}

static void afe_irq2_handler(void)
{
    if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_VUL1)) {
        afe.func_handle->afe_vul1_interrupt_handler();
    }
    if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_VUL2)) {
        //afe.func_handle->afe_vul2_interrupt_handler();
    }
    if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_AWB)) {
        //afe.func_handle->afe_awb_interrupt_handler();
    }
    if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_AWB2)) {
        //afe.func_handle->afe_awb2_interrupt_handler();
    }
}

static void afe_irq3_handler(void)
{
#ifdef MTK_PROMPT_SOUND_ENABLE
    if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_DL2)) {

        afe.func_handle->afe_dl2_interrupt_handler();

    }
#endif
}



static void afe_irq6_handler(void)
{
    if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_DL12)) {
        //afe.func_handle->afe_dl12_interrupt_handler();
    }
}

#ifdef MTK_ANC_ENABLE
static void afe_irq9_handler(void)
{
    log_hal_msgid_info("afe_irq9_hnadler (L-ch high) ++++++++++++++\n", 0);
    afe.func_handle->afe_anc_pwrdet_interrupt_handler(AFE_IRQ9);
}

static void afe_irq14_handler(void)
{
    log_hal_msgid_info("afe_irq14_hnadler (R-ch high) ++++++++++++++\n", 0);
    afe.func_handle->afe_anc_pwrdet_interrupt_handler(AFE_IRQ14);
}
#else
#define afe_irq9_handler NULL
#define afe_irq14_handler NULL
#endif
#endif

#if 0
static void (*afe_irq_handler_funcs[AFE_IRQ_ALL_NUM])(void) = {
    NULL,
    afe_irq1_handler,       /* DL1, DL3 */
    afe_irq2_handler,       /* VUL1, VUL2, AWB, AWB2 */
    afe_irq3_handler,       /* DL2*/
    NULL,
    NULL,
    afe_irq6_handler,       /* DL12, for VP */
    NULL,                   /* Reserved */
    NULL,
    afe_irq9_handler,       /* Lch High ANC pwr det */
    NULL,
    NULL,
    NULL,
    NULL,                  /*For AWS*/
    afe_irq14_handler       /* Rch High ANC pwr det */
};
#endif
#if 0
static void afe_irq_handler_active(uint32_t irq_idx)
{
    if (afe_irq_handler_funcs[irq_idx] != NULL) {
        afe_irq_handler_funcs[irq_idx]();
    }
    else {
        //No such handler
    }
}
#endif
static hal_asrc_irq_callback_function_t afe_asrc_irq_handler_function;
#if 0

static void afe_asrc_irq_handler_active(afe_mem_asrc_id_t asrc_id)
{
    if (afe_asrc_irq_handler_function != NULL) {
        afe_asrc_irq_handler_function(asrc_id);
    }
}
#endif

void afe_register_asrc_irq_callback_function(hal_asrc_irq_callback_function_t function)
{
    afe_asrc_irq_handler_function = function;
}

afe_irq_mode_t afe_irq_request_number(audio_digital_block_t mem_block)
{
    switch (mem_block) {
        case AUDIO_DIGITAL_BLOCK_MEM_DL1:
        case AUDIO_DIGITAL_BLOCK_MEM_DL3:
            return AFE_IRQ1;
        case AUDIO_DIGITAL_BLOCK_MEM_DL2:
            return AFE_IRQ3;
        case AUDIO_DIGITAL_BLOCK_MEM_DL12:
            return AFE_IRQ6;
        case AUDIO_DIGITAL_BLOCK_MEM_VUL1:
        case AUDIO_DIGITAL_BLOCK_MEM_VUL2:
        case AUDIO_DIGITAL_BLOCK_MEM_AWB:
        case AUDIO_DIGITAL_BLOCK_MEM_AWB2:
            return AFE_IRQ2;
        default:
            //should not reach here
            return AFE_IRQ1;
    }
}

void afe_set_mem_block_addr(audio_digital_block_t mem_block, afe_block_t *afe_block)
{
    uint32_t addr = afe_block->phys_buffer_addr;
    int32_t  size = (afe_block->u4asrcflag)
                      ? afe_block->u4asrc_buffer_size
                      : afe_block->u4BufferSize;
    switch (mem_block) {
        case AUDIO_DIGITAL_BLOCK_MEM_DL1:
            AFE_SET_REG(AFE_DL1_BASE, addr, 0xffffffff);
            AFE_SET_REG(AFE_DL1_END, addr + (size - 1), 0xffffffff);
            break;
        case AUDIO_DIGITAL_BLOCK_MEM_DL2:
            AFE_SET_REG(AFE_DL2_BASE, addr, 0xffffffff);
            AFE_SET_REG(AFE_DL2_END, addr + (size - 1), 0xffffffff);
            break;
        case AUDIO_DIGITAL_BLOCK_MEM_DL3:
            AFE_SET_REG(AFE_DL3_BASE, addr, 0xffffffff);
            AFE_SET_REG(AFE_DL3_END, addr + (size - 1), 0xffffffff);
            break;
        case AUDIO_DIGITAL_BLOCK_MEM_DL12:
            AFE_SET_REG(AFE_DL12_BASE, addr, 0xffffffff);
            AFE_SET_REG(AFE_DL12_END, addr + (size - 1), 0xffffffff);
            break;
        case AUDIO_DIGITAL_BLOCK_MEM_VUL1:
            AFE_SET_REG(AFE_VUL_BASE, addr, 0xffffffff);
            AFE_SET_REG(AFE_VUL_END, addr + (size - 1), 0xffffffff);
            break;
        case AUDIO_DIGITAL_BLOCK_MEM_VUL2:
            AFE_SET_REG(AFE_VUL2_BASE, addr, 0xffffffff);
            AFE_SET_REG(AFE_VUL2_END, addr + (size - 1), 0xffffffff);
            break;
        case AUDIO_DIGITAL_BLOCK_MEM_AWB:
            AFE_SET_REG(AFE_AWB_BASE, addr, 0xffffffff);
            AFE_SET_REG(AFE_AWB_END, addr + (size - 1), 0xffffffff);
            break;
        case AUDIO_DIGITAL_BLOCK_MEM_AWB2:
            AFE_SET_REG(AFE_AWB2_BASE, addr, 0xffffffff);
            AFE_SET_REG(AFE_AWB2_END, addr + (size - 1), 0xffffffff);
            break;
        default:
            //should not reach here
            break;
    }
}

ATTR_TEXT_IN_IRAM_LEVEL_2 const afe_irq_ctrl_reg_t *afe_get_irq_control_reg(uint32_t irq_idx)
{
    return &afe_irq_control_regs[irq_idx];
}

void afe_set_enable(bool enable)
{
    if (enable) {
        aud_afe_set_cntr++;
        if (aud_afe_set_cntr == 1) {
            AFE_SET_REG(AFE_DAC_CON0, 0x1, 0x1);
        }
    } else {
        aud_afe_set_cntr--;
        if (aud_afe_set_cntr == 0) {
            log_hal_msgid_info("disable AFE_DAC_CON0: AFE_ON bit\n", 0);
            AFE_SET_REG(AFE_DAC_CON0, 0x0, 0x1);
        } else if (aud_afe_set_cntr < 0) {
            aud_afe_set_cntr = 0;
        }

        /*{
            uint32_t retry = 0;
            while ((AFE_GET_REG(AFE_DAC_MON) & 0x1) && ++retry < 100000) {
                hal_gpt_delay_us(10);
            }
        }*/
    }
}

/*[ToDo] Digital gain control functions*/

void afe_set_adda_reg(bool enable)
{
    if (enable) {
        AFE_SET_REG(AFE_ADDA_UL_DL_CON0, 0x1, 0x1);
    } else {
        AFE_SET_REG(AFE_ADDA_UL_DL_CON0, 0x0, 0x1);
    }
}

void afe_set_dl_src_reg(bool enable)
{
    if (enable) {
        AFE_SET_REG(AFE_ADDA_DL_SRC2_CON0, 0x1, 0x1);
    }
    else {
        AFE_SET_REG(AFE_ADDA_DL_SRC2_CON0, 0x0, 0x1);
    }
}

bool afe_get_dl_src_reg_status(void)
{
    return AFE_GET_REG(AFE_ADDA_DL_SRC2_CON0)& 0x1 ;
}


void afe_set_ul_src_reg(bool enable, hal_audio_interface_t mic_interface)
{
    uint32_t ul_rg = afe_get_ul_src_addr(mic_interface);
    if (enable) {
        AFE_SET_REG(ul_rg, 0x1, 0x1);
        AFE_SET_REG(ul_rg, 0x400, 0x400);
    }
    else {
        AFE_SET_REG(ul_rg, 0x0, 0x1);
        AFE_SET_REG(ul_rg, 0x0, 0x400);
    }
}

void audsys_irq_handler(hal_nvic_irq_t irq)
{
    UNUSED(irq);

#if 0//modify for ab1568
    uint32_t irq_en, irq_index;
    uint32_t volatile irq_status;
    const audio_register_bit_info_t *irq_status_reg;
    if (irq == AUDIOSYS0_IRQn) {
        irq_status = AFE_GET_REG(AFE_IRQ_MCU_STATUS) & AUD_DSP0_IRQ_MASK;

        if (irq_status == 0) {
            irq_en = AFE_GET_REG(AFE_IRQ_MCU_EN1);
            AFE_SET_REG(AFE_IRQ_MCU_CLR, irq_en, 0xFFFF);   //Clears the MCU IRQ for AFE while all IRQ status are 0
            goto audsys_irq_handler_exit;
        }

        /* clear irq */
        AFE_SET_REG(AFE_IRQ_MCU_CLR, irq_status, AUD_DSP0_IRQ_MASK);

        /*call each IRQ handler function*/
        for (irq_index = 0; irq_index < AFE_IRQ_NUM; irq_index++) {
            if (afe_get_irq_control_reg(irq_index)->afe_irq_available != true) {
                continue;
            }

            irq_status_reg = &afe_get_irq_control_reg(irq_index)->status;
            if (irq_status & (0x1 << irq_status_reg->sbit)) {
                afe_irq_handler_active(irq_index);
            }
        }
    } else if ((irq == AUDIOSYS1_IRQn) || (irq == AUDIOSYS2_IRQn)) {
        uint32_t asrc_irq_status_reg;

        if (irq == AUDIOSYS1_IRQn) {
            /*AFE_MEM_ASRC_1*/
            asrc_irq_status_reg = ASM_IFR;
            irq_index = AFE_MEM_ASRC_1;
        } else if (irq ==AUDIOSYS2_IRQn) {
            /*AFE_MEM_ASRC_2*/
            asrc_irq_status_reg = ASM2_IFR;
            irq_index  = AFE_MEM_ASRC_2;
        }

        /*AFE_MEM_ASRC_1*/
        irq_status = AFE_GET_REG(asrc_irq_status_reg) & ASM_IFR_MASK;

        /*call IRQ handler function*/
        if (irq_status != 0) {
            afe_asrc_irq_handler_active((afe_mem_asrc_id_t)irq_index);
        }

        /* clear irq */
        AFE_SET_REG(asrc_irq_status_reg, irq_status, ASM_IFR_MASK);


    }

audsys_irq_handler_exit:
    __asm__ __volatile__ ("nop");
#endif
}

afe_sram_mode_t afe_get_prefer_sram_mode(void)
{
    //return AFE_SRAM_COMPACT_MODE; //[ToChk, sp normal mode]
    return AFE_SRAM_NORMAL_MODE;
}

void afe_set_sram_mode(afe_sram_mode_t sram_mode)
{
    if (sram_mode == AFE_SRAM_COMPACT_MODE) {  // all memif use compact mode
        AFE_SET_REG(AFE_MEMIF_HDALIGN, 0 << 16, 0x7fff << 16);
        AFE_SET_REG(AFE_MEMIF_MSB, 1 << 29, 1 << 29);// cpu use compact mode when access sram data
    } else { // all memif use normal mode
        AFE_SET_REG(AFE_MEMIF_HDALIGN, 0x40bf << 16, 0x7fff << 16);
        AFE_SET_REG(AFE_MEMIF_MSB, 0 << 29, 1 << 29); // cpu use normal mode when access sram data
    }
}

static uint32_t afe_get_enable_aud_blk_reg_info(audio_digital_block_t aud_block, int index)
{
    uint32_t i = 0;

    for (i = 0; i < AFE_MEM_BLOCK_ENABLE_REG_NUM; i++) {
        if (afe_mem_block_enable_reg[i][0] == aud_block) {
            return afe_mem_block_enable_reg[i][index];
        }
    }
    return 0; /* 0: no such bit */
}

static uint32_t afe_get_enable_aud_blk_reg_addr(audio_digital_block_t aud_block)
{
    return afe_get_enable_aud_blk_reg_info(aud_block, 1);
}

static uint32_t afe_get_enable_aud_blk_reg_offset(audio_digital_block_t aud_block)
{
    return afe_get_enable_aud_blk_reg_info(aud_block, 2);
}

bool afe_set_memory_path_enable_reg(audio_digital_block_t aud_block, bool enable)
{
    uint32_t offset = afe_get_enable_aud_blk_reg_offset(aud_block);
    uint32_t reg = afe_get_enable_aud_blk_reg_addr(aud_block);
    if (reg == 0) {
        return false;
    }
    AFE_SET_REG(reg, enable << offset, 1 << offset);
    return true;
}

bool afe_set_memif_format_reg(uint32_t interface_type, uint32_t fetch_format)
{
    uint32_t is_align = (fetch_format == AFE_WLEN_32_BIT_ALIGN_24BIT_DATA_8BIT_0) ? 1 : 0;
    uint32_t is_hd = fetch_format == AFE_WLEN_16_BIT ? 0 : 1;

    /* force cpu use 8_24 format when writing 32bit data */
    AFE_SET_REG(AFE_MEMIF_MSB, 0 << 28, 1 << 28);

    switch (interface_type) {
    case AUDIO_DIGITAL_BLOCK_MEM_DL1:
        AFE_SET_REG(AFE_MEMIF_HDALIGN, is_align << 0, 1 << 0);
        AFE_SET_REG(AFE_MEMIF_HD_MODE, is_hd    << 0, 3 << 0);
        break;
    case AUDIO_DIGITAL_BLOCK_MEM_DL12:
        AFE_SET_REG(AFE_MEMIF_HDALIGN, is_align << 1, 1 << 1);
        AFE_SET_REG(AFE_MEMIF_HD_MODE, is_hd    << 2, 3 << 2);
        break;
    case AUDIO_DIGITAL_BLOCK_MEM_DL2:
        AFE_SET_REG(AFE_MEMIF_HDALIGN, is_align << 2, 1 << 2);
        AFE_SET_REG(AFE_MEMIF_HD_MODE, is_hd    << 4, 3 << 4);
        break;
    case AUDIO_DIGITAL_BLOCK_MEM_DL3:
        AFE_SET_REG(AFE_MEMIF_HDALIGN, is_align << 3, 1 << 3);
        AFE_SET_REG(AFE_MEMIF_HD_MODE, is_hd    << 6, 3 << 6);
        break;
    case AUDIO_DIGITAL_BLOCK_MEM_AWB:
        AFE_SET_REG(AFE_MEMIF_HDALIGN, is_align << 4, 1 << 4);
        AFE_SET_REG(AFE_MEMIF_HD_MODE, is_hd    << 8, 3 << 8);
        break;
    case AUDIO_DIGITAL_BLOCK_MEM_VUL1:
        AFE_SET_REG(AFE_MEMIF_HDALIGN, is_align << 5, 1 << 5);
        AFE_SET_REG(AFE_MEMIF_HD_MODE, is_hd    << 10,3 << 10);
        break;
    case AUDIO_DIGITAL_BLOCK_MEM_VUL2:
        AFE_SET_REG(AFE_MEMIF_HDALIGN, is_align << 7, 1 << 7);
        AFE_SET_REG(AFE_MEMIF_HD_MODE, is_hd    << 14,3 << 14);
        break;
    case AUDIO_DIGITAL_BLOCK_MEM_AWB2:
        AFE_SET_REG(AFE_MEMIF_HDALIGN, is_align << 14,1 << 14);
        AFE_SET_REG(AFE_MEMIF_HD_MODE, is_hd    << 28,3 << 28);
        break;
    default:
        return false;
    }
    return true;
}

bool afe_set_samplerate(audio_digital_block_t aud_block, uint32_t sampleRate)
{
    sampleRate = afe_samplerate_transform(sampleRate, aud_block);
    switch (aud_block) {
        case AUDIO_DIGITAL_BLOCK_MEM_DL1:
        case AUDIO_DIGITAL_BLOCK_MEM_DL2:
        case AUDIO_DIGITAL_BLOCK_MEM_DL3:
        case AUDIO_DIGITAL_BLOCK_MEM_DL12:
        case AUDIO_DIGITAL_BLOCK_MEM_AWB:
        case AUDIO_DIGITAL_BLOCK_MEM_AWB2:
        case AUDIO_DIGITAL_BLOCK_MEM_VUL1:
        case AUDIO_DIGITAL_BLOCK_MEM_VUL2:
            AFE_SET_REG(afe_memif_samplerate[aud_block][0], sampleRate << afe_memif_samplerate[aud_block][1],
                 afe_memif_samplerate[aud_block][2] << afe_memif_samplerate[aud_block][1]);
            break;
        default:
            return false;
    }
    return true;
}

bool afe_set_channels(audio_digital_block_t aud_block, uint32_t channel)
{
    const bool mono = (channel == 1) ? true : false;

    switch (aud_block) {
        case AUDIO_DIGITAL_BLOCK_MEM_DL1:
        case AUDIO_DIGITAL_BLOCK_MEM_DL2:
        case AUDIO_DIGITAL_BLOCK_MEM_DL3:
        case AUDIO_DIGITAL_BLOCK_MEM_DL12:
        case AUDIO_DIGITAL_BLOCK_MEM_AWB:
        case AUDIO_DIGITAL_BLOCK_MEM_AWB2:
        case AUDIO_DIGITAL_BLOCK_MEM_VUL1:
        case AUDIO_DIGITAL_BLOCK_MEM_VUL2:
            AFE_SET_REG(afe_memif_channels[aud_block][0], mono << afe_memif_channels[aud_block][1],
                afe_memif_channels[aud_block][2] << afe_memif_channels[aud_block][1]);
            break;
        default:
            return false;
    }
    return true;
}
void afe_set_digital_mic0_enable(hal_audio_interface_t dmic_interface)
{
    UNUSED(dmic_interface);
    //return 0;
    #if 0//modify for ab1568
    hal_audio_dmic_clock_selection_t dmic0_clock;
    uint32_t dmic_data_mask;
    switch (dmic_interface){
        case HAL_AUDIO_INTERFACE_1:
        case HAL_AUDIO_INTERFACE_4:
        default:
            dmic0_clock = HAL_AUDIO_DMIC_CLOCK_0;
            break;
        case HAL_AUDIO_INTERFACE_2:
            dmic0_clock = HAL_AUDIO_DMIC_CLOCK_1;
            break;
        case HAL_AUDIO_INTERFACE_3:
            dmic0_clock = HAL_AUDIO_DMIC_CLOCK_2;
            break;
    }
    dmic_data_mask = ((uint32_t)dmic0_clock)<<1 ;
    AFE_SET_REG(AFE_DMIC_CK_SEL, dmic0_clock, 0x3);
    AFE_SET_REG(AFE_DMIC_DAT_SEL, 0x0<<dmic_data_mask, (0x3)<<dmic_data_mask);
    log_hal_msgid_info("DSP afe_set_digital_mic0_enable %d: ck:0x%x, data:0x%x \r\n", 3, dmic_interface, AFE_GET_REG(AFE_DMIC_CK_SEL), AFE_GET_REG(AFE_DMIC_DAT_SEL));
    #endif
}

bool afe_set_dmic_path(bool enable, hal_audio_interface_t dmic_interface, uint32_t sample_rate)
{
    UNUSED(enable);
    UNUSED(dmic_interface);
    UNUSED(sample_rate);
    return 0;
    #if 0//modify for ab1568
    uint32_t voice_mode = afe_samplerate_transform(sample_rate, AUDIO_DIGITAL_BLOCK_ADDA_UL1);
    uint32_t ul_rg = afe_get_ul_src_addr(dmic_interface);

    if (enable) {
        AFE_SET_REG(ul_rg, 0x1 << 1, 0x1 << 1); // Select SDM 3-level mode (DMIC)
        if (voice_mode >= AFE_ADDA_UL_SAMPLERATE_96K) {
            /* hires */
        } else {
            /* normal 8~48k, use 3.25M */
            AFE_SET_REG(ul_rg, 0x3 << 21, 0x3 << 21);
            //phase setting of L/R
            AFE_SET_REG(ul_rg, 0x1F  << 24, 0x3f000000);
        }
        afe_set_digital_mic0_enable(dmic_interface);

    } else {
        //DMIC interface mux
        AFE_SET_REG(AFE_DMIC_CK_SEL,0x0, 0x3);
        AFE_SET_REG(AFE_DMIC_DAT_SEL,0x0, 0xff);

        AFE_SET_REG(ul_rg, 0x0 << 1, 0x1 << 1);
    }
    return true;
    #endif
}

uint32_t afe_get_sram_phys_addr(void)
{
    return AFE_INTERNAL_SRAM_PHY_BASE; //0x70002000
}

uint32_t afe_get_afe_sram_length(void)
{
    return AFE_INTERNAL_SRAM_SIZE;
}

void afe_set_adc_in(uint32_t samplerate)
{
    uint32_t voice_mode = 0;

    /* Using Internal ADC */
    AFE_SET_REG(AFE_ADDA_TOP_CON0, 0, 0x1 << 0);
    voice_mode = afe_samplerate_transform(samplerate, AUDIO_DIGITAL_BLOCK_ADDA_UL1);
    AFE_SET_REG(AFE_ADDA_UL_SRC_CON0, voice_mode << 17, 0x000e0000);
}

uint32_t afe_get_adc_in_samplerate(void)
{
    uint32_t reg;
    reg = (AFE_GET_REG(AFE_ADDA_UL_SRC_CON0)&0x000e0000)>>17;
    return afe_reg_value_transform(reg, AUDIO_DIGITAL_BLOCK_ADDA_UL1);
}


uint32_t afe_get_ul_src_addr(hal_audio_interface_t mic_interface)
{
    uint32_t reg = AFE_ADDA_UL_SRC_CON0;
    switch (mic_interface) {
        case HAL_AUDIO_INTERFACE_1:
        case HAL_AUDIO_INTERFACE_4:
        default:
            //reg = AFE_ADDA_UL_SRC_CON0;
            break;
        case HAL_AUDIO_INTERFACE_2:
            reg = AFE_ADDA2_UL_SRC_CON0;
            break;
        case HAL_AUDIO_INTERFACE_3:
            reg = AFE_ADDA6_UL_SRC_CON0;
            break;
    }
    return reg;
}


void afe_set_dmic_samplerate(hal_audio_interface_t dmic_interface, uint32_t samplerate)
{
    uint32_t voice_mode = 0;
    voice_mode = afe_samplerate_transform(samplerate, AUDIO_DIGITAL_BLOCK_ADDA_UL1);
    AFE_SET_REG(afe_get_ul_src_addr(dmic_interface), voice_mode << 17, 0x000e0000);
}

uint32_t afe_get_dmic_samplerate(hal_audio_interface_t dmic_interface)
{
    uint32_t reg;
    reg = (AFE_GET_REG(afe_get_ul_src_addr(dmic_interface))&0x000e0000)>>17;
    return afe_reg_value_transform(reg, AUDIO_DIGITAL_BLOCK_ADDA_UL1);
}

void afe_set_lr_swap(bool enable)
{
    AFE_SET_REG(AFE_ADDA_DL_SDM_DCCOMP_CON, enable << 20, 0x1 << 20);
}

void afe_set_dac_in(uint32_t samplerate)
{
    uint32_t audio_mode = 0;

    audio_mode = afe_samplerate_transform(samplerate, AUDIO_DIGITAL_BLOCK_ADDA_DL);
    AFE_SET_REG(AFE_ADDA_DL_SRC2_CON0, audio_mode << 28, 0xf0000000);
}

uint32_t afe_get_dac_in_samplerate(void)
{
    uint32_t reg = 0;
    reg = ((AFE_GET_REG(AFE_ADDA_DL_SRC2_CON0)&0xf0000000)>> 28);
    return afe_reg_value_transform(reg, AUDIO_DIGITAL_BLOCK_ADDA_DL);

}

void afe_set_predistortion(bool enable)
{
    AFE_SET_REG(AFE_ADDA_PREDIS_CON0, enable << 31, 0x80000000);
    AFE_SET_REG(AFE_ADDA_PREDIS_CON1, enable << 31, 0x80000000);
}

void afe_predistortion_switch(void)
{
    //select 5-th predistortion
    AFE_SET_REG(AFE_ADDA_PREDIS_CON2, 0x1 << 31, 0x1 << 31);
    //Set coef.
    //AFE_SET_REG(AFE_ADDA_PREDIS_CON0, 0, 0x0fff0fff);
    //AFE_SET_REG(AFE_ADDA_PREDIS_CON1, 0, 0x0fff0fff);
    //AFE_SET_REG(AFE_ADDA_PREDIS_CON2, 0, 0x0fff0fff);
    //AFE_SET_REG(AFE_ADDA_PREDIS_CON3, 0, 0x0fff0fff);
}

/*********************************************************************
*                         Analog control                             *
*********************************************************************/
#if 0//modify for ab1568

static void afe_top_power_enable(bool enable)
{
    if (enable) {
        AFE_SET_REG(AFUNC_AUD_CON2, 0x6,    0x009f);    //sdm audio fifo clock power on
        AFE_SET_REG(AFUNC_AUD_CON0, 0xcba1, 0xffff);    //scrambler clock on enable
        AFE_SET_REG(AFUNC_AUD_CON2, 0x3,    0x009f);    //sdm power on
        AFE_SET_REG(AFUNC_AUD_CON2, 0xb,    0x009f);    //sdm fifo enable
        AFE_SET_REG(AFE_ANA_GAIN_MUX, 0x5a, 0x005f);    //set HP gain control to ZCD module
        /*[TEMP]Class G setting: should have a function and check if DL case*/
        AFE_WRITE(0x70000EE4, 0x0000040A);              //AFE_CLASSG_CFG3 TH ~-23.9dBV (0.25mW)
        AFE_WRITE(0x70000EE8, 0x08250825);              //classg threshold initial setting, allen cmm no this line
        AFE_WRITE(0x70000EEC, 0x00000000);              //classg level up & level down transition time selection
        AFE_WRITE(0x70000EE0, 0x00181831);              //set classg preview window to 500us
        AFE_WRITE(0x70000EDC, 0x11004001);              //level down hold time setting and classg enable

        ANA_SET_REG(AUDDEC_ANA_CON9,0, 0x1 << 4);       //enable AUDGLB
        afe.top_enable = true;
    } else {
        ANA_SET_REG(AUDDEC_ANA_CON9,0x1 << 4, 0x1 << 4);//disable AUDGLB
        afe.top_enable = false;
    }
}
#endif
void afe_amp_keep_enable_state(bool enable)
{
    afe.amp_open_state = enable;
}

void afe_amp_enable(bool enable, hal_audio_device_t audio_device)
{
    UNUSED(enable);
    UNUSED(audio_device);
    #if 0//modify for ab1568
    uint32_t temp_ZCD_CON2 = 0, target_ZCD_CON2 = 0;
    uint16_t dc_compensation_value = 0;
    int32_t hp_aux_feedback_count = 0;
    if(enable) {
        afe_audio_set_output_analog_gain();
    }
    target_ZCD_CON2 = ReadREG(ZCD_CON2);
    dc_compensation_value = afe.stream_out.dc_compensation_value;
    uint32_t Lch=0, Rch=0;
    if (audio_device == HAL_AUDIO_DEVICE_DAC_R) {
        Rch = 1;
    }
    if (audio_device == HAL_AUDIO_DEVICE_DAC_L){
        Lch = 1;
    }
    if (audio_device == HAL_AUDIO_DEVICE_DAC_DUAL){
        Rch = 1;
        Lch = 1;
    }
#if 0
    //[TEMP]: Add AT Cmd to switch L/R Ch
    if(((*((volatile uint32_t*)(0xA2120B04)))&0x03) == 3) {
        Rch = 1;
        Lch = 1;
    }
    if(((*((volatile uint32_t*)(0xA2120B04)))&0x03) == 1) {
        Rch = 1;
        Lch = 0;
    }
    if(((*((volatile uint32_t*)(0xA2120B04)))&0x03) == 2) {
        Rch = 0;
        Lch = 1;
    }
#endif
    if (enable) {
        log_hal_msgid_info("target_ZCD_CON2=0x%x, dc_compensation_value=0x%x\n",2, target_ZCD_CON2, dc_compensation_value);
        if (afe.headphone_on == true) {
            if(afe.amp_open_state == TRUE) {
                return;
            }
            log_hal_msgid_info("Afe_amp_enable is failed to open Amp again\n", 0);
            //platform_assert("Afe_amp_enable is failed to open Amp again", __FILE__, __LINE__);
        }
        //reset to default values
        ANA_SET_REG (AUDDEC_ANA_CON10, 0,      0xffff);
        ANA_SET_REG (ZCD_CON2,     0xfff,       0xfff);
        ANA_SET_REG (AUDDEC_ANA_CON2,  0,      0xffff);
        ANA_SET_REG (AUDDEC_ANA_CON3,  0,      0xffff);
        ANA_SET_REG (AUDDEC_ANA_CON1,  0,      0xffff);
        ANA_SET_REG (AUDDEC_ANA_CON0,  0,      0xffff);

        afe_classg_clock_on();
        afe_top_power_enable(true);
        ANA_SET_REG (AUDDEC_ANA_CON9,  0, 0x10);                                                        //enable 2.4v LDO
        ANA_SET_REG (AUDDEC_ANA_CON9,  0x3 <<12, 0x7000);                                               //enable 2.4v LDO
        ANA_SET_REG (AUDDEC_ANA_CON11, 0x1 << 2, 0x1ff);                                                //set VCM at 0v
        hal_gpt_delay_us(50);
        ANA_SET_REG (AUDDEC_ANA_CON11, 0x183, 0x1fff);                                                  //SW enable VCM tracker to set Vcm voltage
        hal_gpt_delay_us(50);
        ANA_SET_REG (AUDDEC_ANA_CON11, 0,     0x1);                                                     //SW disable VCM tracker, use HW to control VCM tracker
        hal_gpt_delay_us(100);
        ANA_SET_REG (AUDDEC_ANA_CON9,  0x1 << 8,  0x1 << 8);                                            //enable 2.4v LDO core
        ANA_SET_REG (AUDDEC_ANA_CON9,  0x1 << 9,  0x1 << 9);                                            //enable 2.4v LDO DEC
        ANA_SET_REG (AUDDEC_ANA_CON9,  0x1 <<10,  0x1 <<10);                                            //enable 2.4v LDO DAC
        ANA_SET_REG (AUDDEC_ANA_CON9,  0x1 << 5,  0x3 << 5);                                            //Set 1.5V LDO voltage to 1.5V
        hal_gpt_delay_us(100);
        ANA_SET_REG (AUDDEC_ANA_CON10, 0x1 << 4, 0x1 << 4);                                             //enable HCLDO
        ANA_SET_REG (AUDDEC_ANA_CON10, 0x1 << 5, 0x1 << 5);                                             //enable LCLDO
        ANA_SET_REG (AUDDEC_ANA_CON10, 0x1 << 6, 0x1 << 6);                                             //enable LDLDO_DAC
        ANA_SET_REG (AUDDEC_ANA_CON10, 0x1 << 8, 0x1 << 8);                                             //enable HCLDO remote sense
        ANA_SET_REG (AUDDEC_ANA_CON10, 0x1 <<12, 0x1 <<12);                                             //enable LCLDO remote sense
        ANA_SET_REG (AUDDEC_ANA_CON10, 0x1 <<15, 0x1 <<15);                                             //enable LDLDO_DAC remote sense
        hal_gpt_delay_us(100);
        AFE_SET_REG (ZCD_CON0,         0,        0x1);                                                  //disable ZCD
        ANA_SET_REG (AUDDEC_ANA_CON1,  0x3 <<12, 0x3000);                                               //disable headphone short-ckt protection
        ANA_SET_REG (AUDDEC_ANA_CON3,  0x1 << 8, 0x1 << 8);                                             //reduce ESD resistance of AU_REFN
        ANA_SET_REG (AUDDEC_ANA_CON3,  0x1 <<15, 0x1 <<15);                                             //Enable HP trim function
        ANA_SET_REG (AUDDEC_ANA_CON4,  dc_compensation_value, (Rch<<8|Rch<<9|Rch<<10|Rch<<11|Rch<<12|Rch<<13|Rch<<14|Rch<<15)|(Lch|Lch<<1|Lch<<2|Lch<<3|Lch<<4|Lch<<5|Lch<<6|Lch<<7)); //Set HPL/R trim code value
        ANA_SET_REG (AUDDEC_ANA_CON8,  0,        0x8000);                                               //enable IBIST
        ANA_SET_REG (AUDDEC_ANA_CON7,  0x103,    0x0fff);                                               //set HP ibais & DR bias current opt.
        ANA_SET_REG (AUDDEC_ANA_CON8,  0,        0x3f);                                                 //set HP ZCD bias current opt. 2.5uA
        ANA_SET_REG (AUDDEC_ANA_CON10, 0x3,      (Rch << 1)|Lch);                                       //enable VCM buffer for depop

        if ((((*((volatile uint32_t*)0xA2120B04))>>8)&0x01) == 0) {
            ANA_SET_REG (AUDDEC_ANA_CON5,  0,        0x7 << 4);                                             //Set HP driver i/p pair bias current to 20uA
            log_hal_msgid_info("DL_NM", 0);
        }
        else
        {
            ANA_SET_REG (AUDDEC_ANA_CON5,  0x3<<4,        0x7 << 4);                                        //Set HP driver input pair bias current to 120uA (HPM)
            log_hal_msgid_info("DL_HP", 0);
        }

        AFE_SET_REG (ZCD_CON2,         0xb2c,    (Rch<<6|Rch<<7|Rch<<8|Rch<<9|Rch<<10|Rch<<11)|(Lch|Lch<<1|Lch<<2|Lch<<3|Lch<<4|Lch<<5));//Set HPR/L gain to min -32dB
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0, 0,  0x7 << 19);                                            //Set HPR/L o/p stage to min size
        ANA_SET_REG (AUDDEC_ANA_CON2,  0x24,     (Rch << 5)|(Lch << 2));                                //HPR/L o/p stage STB enhance.
        ANA_SET_REG (AUDDEC_ANA_CON3,  0x1 << 9, 0x1 << 9);                                             //HP driver positive o/p stage short to negative o/p stage
        ANA_SET_REG (AUDDEC_ANA_CON3,  0,        (Rch<<4|Rch<<5|Rch<<6|Rch<<7)|(Lch|Lch<<1|Lch<<2|Lch<<3));//HP aux loop gain setting to max
        ANA_SET_REG (AUDDEC_ANA_CON2,  0x3 << 7,(Rch << 8)|(Lch << 7));                                 //enable HP auxiliary feedback loop
        ANA_SET_REG (AUDDEC_ANA_CON2,  (Rch << 13)|(Lch << 12),(Rch<<13|Rch<<15)|(Lch<<12|Lch<<14));    //enable HP aux. CMFB loop
        ANA_SET_REG (AUDDEC_ANA_CON1,  0x000c,   (Rch << 3)|(Lch << 2));                                //enable HP aux output stage
        ANA_SET_REG (AUDDEC_ANA_CON1,  0x00c0,   (Rch << 7)|(Lch << 6));                                //enable HP drivers bias circuits
        ANA_SET_REG (AUDDEC_ANA_CON1,  0x0030,   (Rch << 5)|(Lch << 4));                                //enable HP driver core circuits
        ANA_SET_REG (AUDDEC_ANA_CON2,  0x0c00,   (Rch << 11)|(Lch << 10));                              //Short HPL/HPR main o/p to HP aux o/p stage
        ANA_SET_REG (AUDDEC_ANA_CON2,  0xc000,   (Rch << 15)|(Lch << 14));                              //enable HP main CMFB loop
        hal_gpt_delay_us(50);
        ANA_SET_REG (AUDDEC_ANA_CON2,  0, Rch<<13|Lch<<12);                                             //disable HP aux CMFB loop
        ANA_SET_REG (AUDDEC_ANA_CON1,  0x0003,   (Rch << 1)|Lch);                                       //enable HP main o/p stage
        hal_gpt_delay_us(600);
//Class G gain
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,  0x1 << 19, 0x380000);
        hal_gpt_delay_us(600);
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,  0x1 << 20, 0x380000);
        hal_gpt_delay_us(600);
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,  0x3 << 19, 0x380000);
        hal_gpt_delay_us(600);
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,  0x1 << 21, 0x380000);
        hal_gpt_delay_us(600);
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,  0x5 << 19, 0x380000);
        hal_gpt_delay_us(600);
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,  0x3 << 20, 0x380000);
        hal_gpt_delay_us(600);
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,  0x7 << 19, 0x380000);
        hal_gpt_delay_us(600);

        for (hp_aux_feedback_count = 0x0+0x11; hp_aux_feedback_count <= 0xff; hp_aux_feedback_count += 0x11) {
            ANA_SET_REG (AUDDEC_ANA_CON3,  hp_aux_feedback_count,  (Rch<<4|Rch<<5|Rch<<6|Rch<<7)|(Lch|Lch<<1|Lch<<2|Lch<<3));//HP aux feedback loop gain step by step
            hal_gpt_delay_us(600);
        }

        ANA_SET_REG (AUDDEC_ANA_CON2,  0, (Rch << 8)|(Lch << 7));                                       //disable HP auxiliary feedback loop
        //HP gain ramp up from -32dB to target gain, 1dB/step
        for (temp_ZCD_CON2 = 0xb2c-0x41; temp_ZCD_CON2 > target_ZCD_CON2; temp_ZCD_CON2 -= 0x41) {
            AFE_SET_REG (ZCD_CON2,   temp_ZCD_CON2,  0xfff);                                            //Increase HPR/HPL gain to normal gain step by step
            hal_gpt_delay_us(100);
        }
        AFE_SET_REG (ZCD_CON2,   target_ZCD_CON2,  0xfff);
        ANA_SET_REG (AUDDEC_ANA_CON1,  0, (Rch << 3)|(Lch << 2));                                       //disable HP aux o/p loop
        ANA_SET_REG (AUDDEC_ANA_CON2,  0, (Rch << 11)|(Lch << 10));                                     //unshort HPL/HPR main o/p to HP aux output stage
        ANA_SET_REG (AUDDEC_ANA_CON10, 0, (Rch << 1)|Lch);                                              //disable VCM buffer
        hal_gpt_delay_us(100);
        if (afe.mic_bias == false) {
            ANA_SET_REG (AUDDEC_ANA_CON9,  1,      0x1);                                                //enable AUD_CLK & set DL operation clock to 13MHz
        }
        ANA_SET_REG (AUDDEC_ANA_CON12, 0x100, 0xff00);                                                  //enable CLK2DL path
        hal_gpt_delay_us(100);
        if ((((*((volatile uint32_t*)0xA2120B04))>>8)&0x01) == 0) {
            ANA_SET_REG (AUDDEC_ANA_CON0, 0x1 << 6, 0x60);                                                  //reduce bias current in NM
            ANA_SET_REG (AUDDEC_ANA_CON0, 0x1 << 8, 0x0100);                                                //DAC current at NM
        }
        else
        {
            ANA_SET_REG (AUDDEC_ANA_CON0, 0x60, 0x60);                                                      //Increase bias current in HPM
            ANA_SET_REG (AUDDEC_ANA_CON0,  0, 0x0100);                                                      //DAC current at HPM
        }

        ANA_SET_REG (AUDDEC_ANA_CON0, ((Rch << 1)|(Rch << 3))|(Lch|(Lch << 2)), 0xf);                   //enable audio DAC
        hal_gpt_delay_us(100);
        ANA_SET_REG (AUDDEC_ANA_CON1,  0x500 , ((Rch<<10)|(Rch<<11))|((Lch<<8)|(Lch<<9)));              //Switch HPL/HPL MUX to audio DAC
        ANA_SET_REG (AUDDEC_ANA_CON3,  0, 0x1 << 9);                                                    //HP driver positive o/p stage unshort to negative o/p stage
        afe.headphone_on = true;
        afe.amp_open_state = FALSE;
    } else {
        if ((afe.headphone_on == false)||(afe.amp_open_state == TRUE)) {
            return;
        }
        ANA_SET_REG (AUDDEC_ANA_CON3,  0x1 << 9, 0x1 << 9);                             //HP driver positive o/p stage short to negative o/p stage
        ANA_SET_REG (AUDDEC_ANA_CON10, 0x3, (Rch << 1)|(Lch << 0));                     //enable VCM buffer for depop
        ANA_SET_REG (AUDDEC_ANA_CON1,  0, (Rch<<10|Rch<<11)|(Lch<<8|Lch<<9));           //Switch HPL/HPL MUX to open
        ANA_SET_REG (AUDDEC_ANA_CON0,  0, 0x000f);                                      //disable audio DAC
        ANA_SET_REG (AUDDEC_ANA_CON12, 0, 0xff00);                                      //disable CLK2DL path
        if (afe.mic_bias == false) {
            ANA_SET_REG (AUDDEC_ANA_CON9,  0, 0x1);                                     //disable AUD_CLK & set DL operatoin clock to 13MHz
        }
        hal_gpt_delay_us(50);
        ANA_SET_REG (AUDDEC_ANA_CON2,  0x3 << 10, (Rch << 11)|(Lch << 10));             //short HPL/HPR main o/p to HP aux o/p stage
        ANA_SET_REG (AUDDEC_ANA_CON1,  0x3 << 2, (Rch << 3)|(Lch << 2));                //enable HP aux output stage
        hal_gpt_delay_us(50);
        //HP gain ramp down from target gain to -32dB, 1dB/step
        for (temp_ZCD_CON2 = target_ZCD_CON2+0x41; temp_ZCD_CON2 <= 0xb2c; temp_ZCD_CON2 +=0x41) {
            AFE_SET_REG (ZCD_CON2,   temp_ZCD_CON2,  0xfff);                                            //Decrease HPR/HPL gain to -32dB step by step
            hal_gpt_delay_us(100);
        }
        ANA_SET_REG (AUDDEC_ANA_CON3,  0xff, (Rch<<4|Rch<<5|Rch<<6|Rch<<7)|(Lch<<0|Lch<<1|Lch<<2|Lch<<3));  //HP aux feedback loop gain step by step
        ANA_SET_REG (AUDDEC_ANA_CON2,  0x3 << 7, (Rch<<8|Lch<<7));                      //Enable HP auxiliary feedback loop
        hal_gpt_delay_us(600);

        for (hp_aux_feedback_count = 0xff-0x11; hp_aux_feedback_count >= 0x0; hp_aux_feedback_count -= 0x11) {
            ANA_SET_REG (AUDDEC_ANA_CON3,  hp_aux_feedback_count,  (Rch<<4|Rch<<5|Rch<<6|Rch<<7)|(Lch|Lch<<1|Lch<<2|Lch<<3));//HP aux feedback loop gain step by step
            hal_gpt_delay_us(600);
        }

        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,  0x7 << 19, 0x380000);
        hal_gpt_delay_us(600);
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,  0x3 << 20, 0x380000);
        hal_gpt_delay_us(600);
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,  0x5 << 19, 0x380000);
        hal_gpt_delay_us(600);
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,  0x1 << 21, 0x380000);
        hal_gpt_delay_us(600);
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,  0x3 << 19, 0x380000);
        hal_gpt_delay_us(600);
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,  0x1 << 20, 0x380000);
        hal_gpt_delay_us(600);
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,  0x1 << 19, 0x380000);
        hal_gpt_delay_us(600);
        AFE_SET_REG (AFE_CLASSG_LPSLCH_CFG0,          0, 0x380000);
        hal_gpt_delay_us(600);
        ANA_SET_REG (AUDDEC_ANA_CON1,  0, (Rch<<1|Lch<<0));                             //disable HP main o/p stage
        ANA_SET_REG (AUDDEC_ANA_CON2,  0x3 << 12, (Rch<<13|Lch<<12));                   //enable HP aux. CMFB loop
        hal_gpt_delay_us(50);
        ANA_SET_REG (AUDDEC_ANA_CON2,  0, (Rch<<15|Lch<<14));                           //disable HP main CMFB loop
        ANA_SET_REG (AUDDEC_ANA_CON2,  0, (Rch<<11|Lch<<10));                           //unshort HPL/HPR main o/p to HP aux output stage
        ANA_SET_REG (AUDDEC_ANA_CON1,  0, (Rch<<5|Lch<<4));                             //disable HP driver core circuits
        ANA_SET_REG (AUDDEC_ANA_CON1,  0, (Rch<<7|Lch<<6));                             //disable HP drivers bias circuits
        ANA_SET_REG (AUDDEC_ANA_CON1,  0, (Rch<<3|Lch<<2));                             //disable HP aux output stage
        ANA_SET_REG (AUDDEC_ANA_CON2,  0x3 << 14, (Rch<<13|Rch<<15|Lch<<12|Lch<<14));   //disable HP aux CMFB loop and enable HP main CMFB loop
        ANA_SET_REG (AUDDEC_ANA_CON2,  0, (Rch<<8|Lch<<7));                             //disable HP auxiliary feedback loop
        ANA_SET_REG (AUDDEC_ANA_CON10, 0, (Rch<<1|Lch<<0));                             //disable VCM buffer
        ANA_SET_REG (AUDDEC_ANA_CON3,  0, 0x1 << 9);                                    //HP driver positive o/p stage unshort to negative o/p stage
        ANA_SET_REG (AUDDEC_ANA_CON3,  0, 0x1 <<15);                                    //Disable HP trim function
        AFE_SET_REG (ZCD_CON0,         0, 0x1);                                         //disable ZCD
        ANA_SET_REG (AUDDEC_ANA_CON8,  0x1 << 15, 0x1 << 15);                           //disable IBIST
        hal_gpt_delay_us(100);
        ANA_SET_REG (AUDDEC_ANA_CON10, 0, 0x1 << 4);                                    //disable HCLDO
        ANA_SET_REG (AUDDEC_ANA_CON10, 0, 0x1 << 5);                                    //disable LCLDO
        ANA_SET_REG (AUDDEC_ANA_CON10, 0, 0x1 << 6);                                    //disable LDLDO_DAC
        hal_gpt_delay_us(100);
        ANA_SET_REG (AUDDEC_ANA_CON9,  0,  0x1 <<10);                                   //disable 2.4v LDO DAC
        ANA_SET_REG (AUDDEC_ANA_CON9,  0,  0x1 << 9);                                   //disable 2.4v LDO DEC
        ANA_SET_REG (AUDDEC_ANA_CON9,  0,  0x1 << 8);                                   //disable 2.4v LDO core
        hal_gpt_delay_us(100);
        ANA_SET_REG (AUDDEC_ANA_CON11, 0x1 << 2, 0x1ff);                                //set VCM at 0v
        hal_gpt_delay_us(50);
        if (afe.mic_bias == false) {
            afe_top_power_enable(false);
        }
        afe_classg_clock_off();
        afe.headphone_on = false;
        log_hal_msgid_info("Afe_amp_enable disable AMP\n", 0);
    }
    #endif
}

/*[Lanus] Do DCC first, 3Vmic, gpio mode*/
void afe_dcc_mic_enable(bool enable, hal_audio_device_t audio_device, uint8_t MicbiasSourceType)
{
    UNUSED(enable);
    UNUSED(audio_device);
    UNUSED(MicbiasSourceType);
    #if 0 //modify for ab1568
    if (enable) {
        if (afe.mic_bias == true) {
            return;
        }
        afe_top_power_enable(true);

        if (audio_device & HAL_AUDIO_DEVICE_MAIN_MIC_DUAL || audio_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL) {
            //TOP
            ANA_SET_REG (AUDDEC_ANA_CON9,         0, 0x1 << 4); //Enable audio globe bias (Default on)
            if (afe.headphone_on == false) {
                ANA_SET_REG (AUDDEC_ANA_CON9,     1,      0x1); //Enable CLKSQ 26MHz
            }
            ANA_SET_REG (AUDENC_ANA_CON3,  0x1 << 4, 0x1 << 4); //Enable CLK from DL to UL
            ANA_SET_REG (AUDDEC_ANA_CON9,         0, 0x1 << 2); //AudioDL 26MHz clock input selection, (0):from DCXO, (1):AVSS18
            ANA_SET_REG (AUDENC_ANA_CON2,  0x1 << 1, 0x1 << 1); //current optimize

            //Capless LDO
            ANA_SET_REG (AUDENC_ANA_CON9,         1, 0x1 << 0); //enable for LCLDO_ENC
            ANA_SET_REG (AUDENC_ANA_CON9,         0, 0x1 << 1); //LCLDO power down discharge enable
            ANA_SET_REG (AUDENC_ANA_CON9,  0x1 << 2, 0x1 << 2); //LCLDO transient boost enable
            ANA_SET_REG (AUDENC_ANA_CON9,         0, 0x1 << 3); //LCLDO remote sense function selection: local sense
            ANA_SET_REG (AUDENC_ANA_CON9,         0, 0x1 << 4); //LCLDO power down discharge test

            //MICBIAS
            if (MicbiasSourceType & MICBIAS_0) {
                ANA_SET_REG (AUDENC_ANA_CON12,      0x19, 0x007f);  //enable MICBIAS0
            } else {
                ANA_SET_REG (AUDENC_ANA_CON12,      0x18, 0x007f);  //disable MICBIAS0
            }
            if (audio_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL) {
                if (MicbiasSourceType & MICBIAS_0) {
                    ANA_SET_REG (AUDENC_ANA_CON12,      0x69, 0x007f);  //enable MICBIAS0
                } else {
                    ANA_SET_REG (AUDENC_ANA_CON12,      0x68, 0x007f);  //disable MICBIAS0
                }
            }
            ANA_SET_REG (AUDENC_ANA_CON12,         0, 0x0700);  //enable DCC mode MICBIAS0 interanl resistors
            ANA_SET_REG (AUDENC_ANA_CON12,  0x1 <<12, 0x1000);  //enable MICBIAS0 pull low when disable MICBIAS
            if (MicbiasSourceType & MICBIAS_1) {
                ANA_SET_REG (AUDENC_ANA_CON13,       0x1, 0x007f);  //enable MICBIAS1
            } else {
                ANA_SET_REG (AUDENC_ANA_CON13,       0x0, 0x007f);  //disable MICBIAS1
            }
            ANA_SET_REG (AUDENC_ANA_CON13,         0, 0x0700);  //enable DCC mode MICBIAS1 interanl resistors
            ANA_SET_REG (AUDENC_ANA_CON13,  0x1 <<12, 0x1000);  //enable MICBIAS1 pull low when disable MICBIAS

            //PGA setting
            if ((audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_L) || (audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) || (audio_device == HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL)) {
                ANA_SET_REG (AUDENC_ANA_CON0,              0x4,  0x0004);  //audio L preamplifier DCC precharge
                ANA_SET_REG (AUDENC_ANA_CON0,             0x41,  0x00C1);  //audio L preamplifier input sel : AIN0 / enable audio L PGA
                //ANA_SET_REG (AUDENC_ANA_CON0,  target_PGA_GAIN,  0x0700);  //setting audio L PGA gain as same as target gain, 6dB/step
                ANA_SET_REG (AUDENC_ANA_CON0,         0x1 << 1,  0x0002);  //audio L preamplifier DCCEN
            }
            if ((audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_R) || (audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) || (audio_device == HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL)) {
                ANA_SET_REG (AUDENC_ANA_CON1,              0x4,  0x0004);  //audio R preamplifier DCC precharge
                ANA_SET_REG (AUDENC_ANA_CON1,             0x81,  0x00C1);  //audio R preamplifier input sel : AIN1 / enable audio R PGA
                //ANA_SET_REG (AUDENC_ANA_CON1,  target_PGA_GAIN,  0x0700);  //setting audio R PGA gain as same as target gain, 6dB/step
                ANA_SET_REG (AUDENC_ANA_CON1,         0x1 << 1,  0x0002);  //audio R preamplifier DCCEN
            }
            afe_audio_set_input_analog_gain();

            // ACC mode[temp]
            if ((((*((volatile uint32_t*)0xA2120B04))>>4)&0x01) == 1) {
                log_hal_msgid_info("AMIC ACC", 0);
                ANA_SET_REG (AUDENC_ANA_CON1,         0,  0x0002);  //audio R preamplifier DCCEN
                ANA_SET_REG (AUDENC_ANA_CON0,         0,  0x0002);  //audio L preamplifier DCCEN
            } else {
                log_hal_msgid_info("AMIC DCC", 0);
            }

            //DCC 50k CLK
            AFE_SET_REG (AFE_DCCLK_CFG,  0x103 <<11,  0x3FF800); //clk div
            AFE_SET_REG (AFE_DCCLK_CFG,           0,  0x1 << 9);
            AFE_SET_REG (AFE_DCCLK_CFG,    0x1 << 8,  0x1 << 8);
            AFE_SET_REG (AFE_DCCLK_CFG,    0x1 << 4,  0x1 << 4);

            //ADC
            ANA_SET_REG (AUDENC_ANA_CON3,         0,      0x3); //Audio ADC clock source: 6.5M
            ANA_SET_REG (AUDENC_ANA_CON3,         0,      0xc); //ADC CLK from: 00_13MHz from CLKSQ (Default)
            if ((audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_L) || (audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) || (audio_device == HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL)) {
                ANA_SET_REG (AUDENC_ANA_CON0,  0x5 <<12,   0x7000); //audio L ADC input sel : L PGA / enable audio L ADC
            }
            if ((audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_R) || (audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) || (audio_device == HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL)) {
                ANA_SET_REG (AUDENC_ANA_CON1,  0x5 <<12,   0x7000); //audio R ADC input sel : R PGA / enable audio R ADC
            }
            if ((((*((volatile uint32_t*)0xA2120B04))>>9)&0x01) == 0) {
                ANA_SET_REG (AUDENC_ANA_CON7,  0x1 << 1,      0x3); //CLK divided by 2 for LP mode
                ANA_SET_REG (AUDENC_ANA_CON2,       0x1,      0x1); //UL GLB bias current divided by 2 for LP mode
                log_hal_msgid_info("UL_NM", 0);
            }
            else
            {
                ANA_SET_REG (AUDENC_ANA_CON7,         0,      0x2); //CLK divided by 2 for LP mode
                ANA_SET_REG (AUDENC_ANA_CON2,         0,      0x1); //UL GLB bias current divided by 2 for LP mode
                log_hal_msgid_info("UL_HP", 0);
            }
            hal_gpt_delay_us(100);
            if ((audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_R) || (audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) || (audio_device == HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL)) {
                ANA_SET_REG (AUDENC_ANA_CON1,  0, 0x0004);          //audio R preamplifier DCC precharge off
            }
            if ((audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_L) || (audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) || (audio_device == HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL)) {
                ANA_SET_REG (AUDENC_ANA_CON0,  0, 0x0004);          //audio L preamplifier DCC precharge off
            }
        }

        if (audio_device & HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL) {
            log_hal_msgid_info("DMIC", 0);
            //TOP
            ANA_SET_REG (AUDDEC_ANA_CON9,          0,0x1 << 4); //Enable audio globe bias (Default on)

            // sequence not have, maybe need open later
            if (afe.headphone_on == false) {
                ANA_SET_REG (AUDDEC_ANA_CON9,      1,     0x1); //enable AUD_CLK & set DL operation clock to 13MHz
            }

            //MICBIAS
            if (MicbiasSourceType & MICBIAS_0) {
                ANA_SET_REG (AUDENC_ANA_CON12,      0x19, 0x007f);  //enable MICBIAS0
            } else {
                ANA_SET_REG (AUDENC_ANA_CON12,      0x18, 0x007f);  //disable MICBIAS0
            }
            ANA_SET_REG (AUDENC_ANA_CON12,         0,  0x0700); //enable DCC mode MICBIAS0 interanl resistors
            ANA_SET_REG (AUDENC_ANA_CON12,  0x1 <<12,  0x1000); //enable MICBIAS0 pull low when disable MICBIAS
            if (MicbiasSourceType & MICBIAS_1) {
                ANA_SET_REG (AUDENC_ANA_CON13,       0x1, 0x007f);  //enable MICBIAS1
            } else {
                ANA_SET_REG (AUDENC_ANA_CON13,       0x0, 0x007f);  //disable MICBIAS1
            }
            ANA_SET_REG (AUDENC_ANA_CON13,         0,  0x0700); //enable DCC mode MICBIAS1 interanl resistors
            ANA_SET_REG (AUDENC_ANA_CON13,  0x1 <<12,  0x1000); //enable MICBIAS1 pull low when disable MICBIAS

            //DMIC
            ANA_SET_REG (AUDENC_ANA_CON10,       0x1,   0xfff); //Enable DMIC0 (Power from MICBIAS0)
            ANA_SET_REG (AUDENC_ANA_CON11,       0x1,  0xffff); //Enable DMIC1 (Power from MICBIAS1)
        }
        afe.mic_bias = true;
    } else {
        if (afe.mic_bias == false) {
            return;
        }
        if (audio_device & HAL_AUDIO_DEVICE_MAIN_MIC_DUAL || audio_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL) {
            //ADC
            if ((audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_R) || (audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) || (audio_device == HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL)) {
                ANA_SET_REG (AUDENC_ANA_CON1,    0,  0x7000);       //audio R ADC input sel : off / disable audio R ADC
            }
            if ((audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_L) || (audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) || (audio_device == HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL)) {
                ANA_SET_REG (AUDENC_ANA_CON0,    0,  0x7000);       //audio L ADC input sel : off / disable audio L ADC
            }
            ANA_SET_REG (AUDENC_ANA_CON3,    0,     0xc);       //ADC CLK from: 00_13MHz from CLKSQ (Default)
            ANA_SET_REG (AUDENC_ANA_CON3,    0,     0x3);       //Audio ADC clock source: 6.5M
            ANA_SET_REG (AUDENC_ANA_CON7,  0x1,     0x1);       //CLK reset for LP mode

            //PGA setting
            if ((audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_L) || (audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) || (audio_device == HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL)) {
                ANA_SET_REG (AUDENC_ANA_CON0,    0,  0x0002);       //Audio L preamplifier DCCEN
                ANA_SET_REG (AUDENC_ANA_CON0,    0,  0x7<<8);       //Audio L preamplifier gain adjust(PGA): 0dB
                ANA_SET_REG (AUDENC_ANA_CON0,    0,  0x00C1);       //audio L preamplifier input sel : none / disable audio L PGA
                ANA_SET_REG (AUDENC_ANA_CON0,    0,  0x0004);       //audio L preamplifier DCC precharge
            }
            if ((audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_R) || (audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) || (audio_device == HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL)) {
                ANA_SET_REG (AUDENC_ANA_CON1,    0,  0x0002);       //audio R preamplifier DCCEN
                ANA_SET_REG (AUDENC_ANA_CON1,    0,  0x7<<8);       //Audio R preamplifier gain adjust(PGA): 0dB
                ANA_SET_REG (AUDENC_ANA_CON1,    0,  0x00C1);       //audio R preamplifier input sel : none / disable audio R PGA
                ANA_SET_REG (AUDENC_ANA_CON1,    0,  0x0004);       //audio R preamplifier DCC precharge
            }

            //DCC 50k CLK
            AFE_SET_REG (AFE_DCCLK_CFG,            0,  0x1 << 8);
            AFE_SET_REG (AFE_DCCLK_CFG,     0x1 << 9,  0x1 << 9);
            AFE_SET_REG (AFE_DCCLK_CFG,  0x103 << 11,  0x3FF800); //clk div

            //Micbias
            ANA_SET_REG (AUDENC_ANA_CON12,      0x10,  0x007f); //disable MICBIAS0
            ANA_SET_REG (AUDENC_ANA_CON12,         0,  0x0700); //disable DCC mode MICBIAS0 interanl resistors
            ANA_SET_REG (AUDENC_ANA_CON12,  0x1 <<12,  0x1000); //disable MICBIAS0 pull low when disable MICBIAS
            ANA_SET_REG (AUDENC_ANA_CON13,         0,  0x007f); //disable MICBIAS1
            ANA_SET_REG (AUDENC_ANA_CON13,         0,  0x0700); //disable DCC mode MICBIAS1 interanl resistors
            ANA_SET_REG (AUDENC_ANA_CON13,  0x1 <<12,  0x1000); //disable MICBIAS1 pull low when disable MICBIAS

            //Capless LDO
            ANA_SET_REG (AUDENC_ANA_CON9,    0,    0x0001);     //disable for LCLDO_ENC

            if ((((*((volatile uint32_t*)0xA2120B04))>>9)&0x01) == 0) {
                ANA_SET_REG (AUDENC_ANA_CON9,    0,    0x0004);     //LCLDO transient boost disable
            }
            else
            {
                ANA_SET_REG (AUDENC_ANA_CON9,    0,    0x0005);     //LCLDO transient boost disable
            }

            //Top
            ANA_SET_REG (AUDENC_ANA_CON2,    0,  0x1 << 1);     //current optimize
            ANA_SET_REG (AUDDEC_ANA_CON9,    0,  0x1 << 2);     //AudioDL 26MHz clock input selection, (0):from DCXO, (1):AVSS18
            ANA_SET_REG (AUDENC_ANA_CON3,    0,  0x1 << 4);     //disable CLK from DL to UL

            if (afe.headphone_on == false) {
                ANA_SET_REG (AUDDEC_ANA_CON9,0,       0x1);     //disable CLKSQ 26MHz
                ANA_SET_REG (AUDDEC_ANA_CON9,0x1 << 4,0x1 << 4);//disable audio globe bias (Default on)
            }
        }

        if (audio_device & HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL) {
            //Micbias
            ANA_SET_REG (AUDENC_ANA_CON12,      0x10,  0x007f); //disable MICBIAS0
            ANA_SET_REG (AUDENC_ANA_CON12,         0,  0x0700); //disable DCC mode MICBIAS0 interanl resistors
            ANA_SET_REG (AUDENC_ANA_CON12,  0x1 <<12,  0x1000); //disable MICBIAS0 pull low when disable MICBIAS
            ANA_SET_REG (AUDENC_ANA_CON13,         0,  0x007f); //disable MICBIAS1
            ANA_SET_REG (AUDENC_ANA_CON13,         0,  0x0700); //disable DCC mode MICBIAS1 interanl resistors
            ANA_SET_REG (AUDENC_ANA_CON13,  0x1 <<12,  0x1000); //disable MICBIAS1 pull low when disable MICBIAS

            //DMIC
            ANA_SET_REG (AUDENC_ANA_CON10,         0,   0xfff); //Disable DMIC0 (Power from MICBIAS0)
            ANA_SET_REG (AUDENC_ANA_CON11,         0,  0xffff); //Disable DMIC1 (Power from MICBIAS1)

            //TOP
            if (afe.headphone_on == false) {
                ANA_SET_REG (AUDDEC_ANA_CON9,0x1 << 4,0x1 << 4);//disable audio globe bias (Default on)
            }
        }
        if (afe.headphone_on == false) {
            afe_top_power_enable(false);
        }
        afe.mic_bias = false;
    }
    #endif
}

/*********************************************************************
*                         Volume control                             *
*********************************************************************/
/*
dB = 20logX => X = 10^(dB/20)
*/
#define DIGITAL_AFE_0DB_REGISTER_VALUE 524288 //(2^19)  align AFE_GAIN1(2) target gain range
#define LN10 2.3025851  // ln(10)
#if 0//moidfy for ab1568

// (float x)^(int n)
float afe_fast_pow_function(float x, int n)
{
    float result = 1.0;
    float temp = x;
    unsigned m = (n >= 0) ? n : -n;
    while (m) {
        if (m & 1) {
            result *= temp;
        }
        temp *= temp;
        m >>= 1;
    }
    return (n >= 0) ? result : (float)1.0 / result;
}
#endif

#if 0//moidfy for ab1568

// e^x
float afe_exp_power_function(const float x, const float minimum_acceptable_error)
{
    float ans1 ;
    float ans2 = 1.0;
    float fact = 1, xn = x, cnt = 2.0;
    do {
        ans1 = ans2;
        ans2 = ans1 + xn / fact;
        fact *= cnt;
        xn = xn * x;
        cnt = cnt + (float)1.0;
    } while ((ans1 > ans2 + minimum_acceptable_error) || (ans2 > ans1 + minimum_acceptable_error));
    return ans2;
}
#endif

#if 0//moidfy for ab1568

void afe_seperate_int_and_decimal(float input, float *int_part, float *decimal_part)
{
    int b = 0;
    float c = 0;
    b = (int)input;
    c = input - (float)b;

    *int_part = (float)b;
    *decimal_part = c;
}
#endif
#if 0//moidfy for ab1568
// e^x fast function
float afe_exp_fast_function(const float exponment, const float minimum_acceptable_error)
{
    const float Euler = 2.718281828459045;
    //float rst=1.0;
    float p1 = 0, p2 = 0;
    afe_seperate_int_and_decimal(exponment, &p1, &p2);

    if (exponment > (float)709.0) {
        p1 = 1.0;
        p2 = 0.0;
        return 0xFFFFFFFF;  // too big not to calculate
    } else if (exponment < (float)(-709.0)) {
        return 0.0;
    } else {
        return afe_exp_power_function(p2, minimum_acceptable_error) * afe_fast_pow_function(Euler, (int)p1);
    }
}
#endif
#if 0//moidfy for ab1568
uint32_t afe_calculate_digital_gain_index(uint32_t digital_gain_in_01unit_db, uint32_t digital_0db_register_value)
{
    uint32_t digital_gain_index = 0;
    int32_t temp_int32_digital_gain = (int32_t)digital_gain_in_01unit_db;
    int32_t temp_int32_digital_gain_register_value = 0;
    float temp_float_digital_gain_register_value = 0;
    float temp_float_digital_gain_in_unit_db = (float)temp_int32_digital_gain / 100;
    float temp_digital_0db_register_value = (float)digital_0db_register_value;
    float exp_exponment = temp_float_digital_gain_in_unit_db / (float)20 * (float)LN10;
    temp_float_digital_gain_register_value = temp_digital_0db_register_value * afe_exp_fast_function(exp_exponment, 1 / temp_digital_0db_register_value);
    temp_int32_digital_gain_register_value = (int32_t)temp_float_digital_gain_register_value;
    digital_gain_index = (uint32_t)temp_int32_digital_gain_register_value;

    return digital_gain_index;
}
#endif
static void afe_truncate_out_of_range_value(int16_t *truncate_value, int32_t minimum, int32_t maximum)
{
    *truncate_value = *truncate_value < minimum ? minimum : *truncate_value;
    *truncate_value = *truncate_value > maximum ? maximum : *truncate_value;
}

static uint32_t afe_to_register_value(int16_t input_db, int32_t max_db_value, int32_t min_db_value, int32_t db_step_value, uint32_t max_reg_value, uint32_t min_reg_value, uint32_t min_db_to_min_reg_value)
{
    int32_t input_db_to_db_step = 0;
    uint32_t register_value = 0;

    afe_truncate_out_of_range_value(&input_db, min_db_value, max_db_value);

    input_db_to_db_step = (input_db - min_db_value) / db_step_value;

    if (min_db_to_min_reg_value) {
        register_value = min_reg_value + (uint32_t)input_db_to_db_step;
    } else {
        register_value = max_reg_value - (uint32_t)input_db_to_db_step;
    }

    if (register_value > max_reg_value) {
        register_value = max_reg_value;
    }

    if ((int)register_value < 0) {
        register_value = 0;
    }

    return register_value;
}

uint32_t afe_audio_get_output_analog_gain_index(void)
{
    return afe.stream_out.analog_gain_index;
}

uint32_t afe_audio_get_input_analog_gain_index(void)
{
    return afe.stream_in.analog_gain_index[INPUT_ANALOG_GAIN_FOR_MIC0_L];
}

void afe_audio_set_output_analog_gain_with_ramp(bool ramp_down, uint32_t *value)
{
    uint32_t temp_ZCD_CON2;
    uint32_t target_ZCD_CON2;
    if (ramp_down) {
        //HP gain ramp down from target gain to -32dB, 1dB/step
        target_ZCD_CON2 = AFE_READ(ZCD_CON2) & 0xfff;
        *value = target_ZCD_CON2;
        for (temp_ZCD_CON2 = target_ZCD_CON2+0x41; temp_ZCD_CON2 <= 0xb2c; temp_ZCD_CON2 +=0x41) {
            AFE_SET_REG (ZCD_CON2,   temp_ZCD_CON2,  0xfff);                                            //Decrease HPR/HPL gain to -32dB step by step
            hal_gpt_delay_us(100);
        }
    } else {
        //HP gain ramp up from -32dB to target gain, 1dB/step
        target_ZCD_CON2 = *value;
        for (temp_ZCD_CON2 = 0xb2c-0x41; temp_ZCD_CON2 > target_ZCD_CON2; temp_ZCD_CON2 -= 0x41) {
            AFE_SET_REG (ZCD_CON2,   temp_ZCD_CON2,  0xfff);                                            //Increase HPR/HPL gain to normal gain step by step
            hal_gpt_delay_us(100);
        }
        AFE_SET_REG (ZCD_CON2,   target_ZCD_CON2,  0xfff);
    }
}

void afe_audio_set_output_analog_gain(void)
{
    uint32_t amp_gain = 0;
    int32_t db_unit = 0;
    int32_t db_max = 0;
    int32_t db_min = 0;
    int32_t db_step = 1;
    uint32_t reg_max = 0;
    uint32_t reg_min = 0;
    uint32_t minimum_db_to_minimum_reg_value = 0;
    hal_audio_afe_clock_on();

    db_unit = ((int32_t)afe.stream_out.analog_gain_index) / 100;
    db_max = 12;
    db_min = -32;
    db_step = 1;
    reg_max = 44;
    reg_min = 0;
    amp_gain = afe_to_register_value(db_unit, db_max, db_min, db_step, reg_max, reg_min, minimum_db_to_minimum_reg_value);
    //set R gain = L gain
    amp_gain = (uint32_t) amp_gain | (uint32_t)amp_gain << 6 ;
    AFE_SET_REG (ZCD_CON2, amp_gain, 0xfff);

    log_hal_msgid_info("DSP afe_audio_set_output_analog_gain :0x%x \r\n", 1, amp_gain);
    hal_audio_afe_clock_off();
}

void afe_audio_set_input_analog_gain(void)
{
    uint32_t amp_gain = 0;
    int32_t db_unit = 0;
    int32_t db_max = 0;
    int32_t db_min = 0;
    int32_t db_step = 1;
    uint32_t reg_max = 0;
    uint32_t reg_min = 0;
    uint32_t minimum_db_to_minimum_reg_value = 1;
    hal_audio_afe_clock_on();
    if (afe.stream_in.mute_flag) {
    } else {
        db_unit = ((int32_t)afe.stream_in.analog_gain_index) / 100;
        db_max = 30;
        db_min = 0;
        db_step = 6;
        reg_max = 5;
        reg_min = 0;
        amp_gain = afe_to_register_value(db_unit, db_max, db_min, db_step, reg_max, reg_min, minimum_db_to_minimum_reg_value);
        ANA_SET_REG (AUDENC_ANA_CON0, amp_gain << 8, 0x0700);
        ANA_SET_REG (AUDENC_ANA_CON1, amp_gain << 8, 0x0700);
    }
    hal_audio_afe_clock_off();
}

void afe_audio_set_output_digital_gain(afe_digital_gain_t gain_type)
{
    UNUSED(gain_type);
#if 0//modify for ab1568
    int32_t gain_index;
    uint32_t temp_digital_gain;
    switch (gain_type) {
        case AUDIO_HW_GAIN:
            if (afe.stream_out.mute_flag == true) {
                gain_index = HWGAIN_MUTE_VALUE;
            } else if (afe_get_hardware_digital_status(AFE_HW_DIGITAL_GAIN2)) {
                gain_index = afe.stream_out.digital_gain_index + afe.stream_out.digital_gain_index_compensation;
            } else {
                gain_index = afe.stream_out.digital_gain_index;
            }
            temp_digital_gain = afe_calculate_digital_gain_index((uint32_t)gain_index, DIGITAL_AFE_0DB_REGISTER_VALUE);
            afe_set_hardware_digital_gain(AFE_HW_DIGITAL_GAIN1, temp_digital_gain);
            log_hal_msgid_info("afe_audio_set_output_digital_gain. gain1 index:0x%x , Rg:0x%x \r\n", 2, gain_index, temp_digital_gain);
            break;
        case AUDIO_HW_GAIN2:
            if (afe.stream_out.mute_flag2 == true) {
                gain_index = HWGAIN_MUTE_VALUE;
            } else if (afe_get_hardware_digital_status(AFE_HW_DIGITAL_GAIN1)) {
                gain_index = afe.stream_out.digital_gain_index2 + afe.stream_out.digital_gain_index2_compensation;
            } else {
                gain_index = afe.stream_out.digital_gain_index2;
            }

            temp_digital_gain = afe_calculate_digital_gain_index((uint32_t)gain_index, DIGITAL_AFE_0DB_REGISTER_VALUE);
            afe_set_hardware_digital_gain(AFE_HW_DIGITAL_GAIN2, temp_digital_gain);
            log_hal_msgid_info("afe_audio_set_output_digital_gain. gain2 index:0x%x , Rg:0x%x \r\n", 2, gain_index, temp_digital_gain);
            break;
        case AUDIO_SW_GAIN:
            //set to compander
            break;
        default:
            break;
    }
#endif
}

int32_t afe_audio_get_input_digital_gain(afe_input_digital_gain_t index)
{
   return afe.stream_in.digital_gain_index[index];
}




void hal_audio_set_gain_parameters(int16_t gain1_recoup, int16_t gain2_recoup, uint16_t gain1_per_step, uint16_t gain2_per_step)
{
    afe.stream_out.digital_gain_index_compensation  = (int32_t)gain1_recoup;
    afe.stream_out.digital_gain_index2_compensation = (int32_t)gain2_recoup;
    afe.stream_out.digital_gain_1_sample_per_step   = gain1_per_step;
    afe.stream_out.digital_gain_2_sample_per_step   = gain2_per_step;

    log_hal_msgid_info("hal_audio_set_gain_parameters. gain1: recoup=%d, sample per step=%d, gain2: recoup=%d, sample per step=%d \r\n", 4, afe.stream_out.digital_gain_index_compensation, afe.stream_out.digital_gain_1_sample_per_step, afe.stream_out.digital_gain_index2_compensation, afe.stream_out.digital_gain_2_sample_per_step);
}

/*
Use HW gain 1/2
1. set interconnection of memif to HW GAIN block
2. set target gain value: afe_set_hardware_digital_gain
3. set gain mode: afe_set_hardware_digital_gain_mode
   note: if want to change gain step:
       gain up step: AFE_GAIN1(2)_CON3
            default 0.25dB   0x83bcd
                    0.5dB    0x8795a
                    1dB      0x8f9e4
       gain down step: AFE_GAIN1(2)_CON2
            default -0.25dB  0x7c5e5
                    -0.5dB   0x78d6f
                    -1dB     0x72148
4. enable gain: afe_enable_hardware_digital_gain
*/
static int16_t aud_afe_digital_gain_ctrl;

void afe_set_hardware_digital_gain(afe_hardware_digital_gain_t gain_type, uint32_t gain)
{
    hal_audio_afe_clock_on();
    switch (gain_type) {
        case AFE_HW_DIGITAL_GAIN1:
            AFE_SET_REG(AFE_GAIN1_CON1, gain, 0xfffff);
            break;
        case AFE_HW_DIGITAL_GAIN2:
            AFE_SET_REG(AFE_GAIN2_CON1, gain, 0xfffff);
            break;
        default:
            break;
    }
    hal_audio_afe_clock_off();
}

uint32_t afe_get_hardware_digital_gain(afe_hardware_digital_gain_t gain_type)
{
    uint32_t register_value;
    hal_audio_afe_clock_on();
    switch (gain_type) {
        case AFE_HW_DIGITAL_GAIN1:
            register_value = AFE_GET_REG(AFE_GAIN1_CON1);
            break;
        case AFE_HW_DIGITAL_GAIN2:
            register_value = AFE_GET_REG(AFE_GAIN2_CON1);
            break;
        default:
            break;
    }
    hal_audio_afe_clock_off();
    return register_value;
}

void afe_enable_hardware_digital_gain(afe_hardware_digital_gain_t gain_type, bool enable)
{
    UNUSED(gain_type);
    UNUSED(enable);
#if 0//modify for ab1568

#if 0
        switch (gain_type) {
        case AFE_HW_DIGITAL_GAIN1:
            if (enable) { /*ramp up from 0*/
                AFE_SET_REG(AFE_GAIN1_CUR, 0, 0xfffff);
            }
            AFE_SET_REG(AFE_GAIN1_CON0, enable, 0x1);
            break;
        case AFE_HW_DIGITAL_GAIN2:
            if (enable) { /*ramp up from 0*/
                AFE_SET_REG(AFE_GAIN2_CUR, 0, 0xfffff);
            }
            AFE_SET_REG(AFE_GAIN2_CON0, enable, 0x1);
            break;
        default:
            break;
    }
#else
    //GVA-1653 workaround
    if (enable) {
        aud_afe_digital_gain_ctrl |= gain_type;
    } else {
        aud_afe_digital_gain_ctrl &= ~(gain_type);
    }

    switch (gain_type) {
        case AFE_HW_DIGITAL_GAIN1:
            AFE_SET_REG(AFE_GAIN1_CUR, 0, 0xfffff);/*ramp up from 0*/
            if (enable) {
                AFE_SET_REG(AFE_GAIN1_CON0, enable, 0x1);
            } else {
                afe_set_hardware_digital_gain(AFE_HW_DIGITAL_GAIN1, 0);
            }

            break;
        case AFE_HW_DIGITAL_GAIN2:
            AFE_SET_REG(AFE_GAIN2_CUR, 0, 0xfffff);/*ramp up from 0*/
            if (enable) {
                AFE_SET_REG(AFE_GAIN2_CON0, enable, 0x1);
            } else {
                afe_set_hardware_digital_gain(AFE_HW_DIGITAL_GAIN2, 0);
            }

            break;
        default:
            break;
    }

    if (aud_afe_digital_gain_ctrl == 0) {
        AFE_SET_REG(AFE_GAIN1_CON0, enable, 0x1);
        AFE_SET_REG(AFE_GAIN2_CON0, enable, 0x1);
    } else {
        if (aud_afe_digital_gain_ctrl&AFE_HW_DIGITAL_GAIN1) {
            afe_audio_set_output_digital_gain(AUDIO_HW_GAIN);
        }
        if (aud_afe_digital_gain_ctrl&AFE_HW_DIGITAL_GAIN2) {
            afe_audio_set_output_digital_gain(AUDIO_HW_GAIN2);
        }
    }
#endif
#endif
}

bool afe_get_hardware_digital_status(afe_hardware_digital_gain_t gain_type)
{
    return (aud_afe_digital_gain_ctrl&gain_type);
}


void afe_set_hardware_digital_gain_mode(afe_hardware_digital_gain_t gain_type, uint32_t sample_rate)
{
    UNUSED(gain_type);
    UNUSED(sample_rate);
#if 0
    uint32_t *gain_rg = NULL;
    uint32_t sample_per_step, value;

    switch (gain_type) {
        case AFE_HW_DIGITAL_GAIN1:
            gain_rg = (uint32_t *)AFE_GAIN1_CON0;
            sample_per_step = (uint32_t)afe.stream_out.digital_gain_1_sample_per_step;
            break;
        case AFE_HW_DIGITAL_GAIN2:
            gain_rg = (uint32_t *)AFE_GAIN2_CON0;
            sample_per_step = (uint32_t)afe.stream_out.digital_gain_2_sample_per_step;
            break;
        default:
            break;
    }
    value = (sample_per_step << 8) | (afe_general_samplerate_transform(sample_rate) << 4);
    if (gain_rg!=NULL) {
        AFE_SET_REG(gain_rg, value, 0xfff0);
    }
#endif
}

/*********************************************************************
*                         Sidetone filter                            *
*********************************************************************/
/*
Use Sidetone gain
0. set STF coefficient to table if needed
1. set interconnection (O10, O11) and call: afe_set_sidetone_input_path, afe_set_sidetone_output_path
2. set sidetone gain value: afe_set_sidetone_volume
4. enable sidtone: afe_set_sidetone_filter
*/

#define SIDETONE_AFE_0DB_REGISTER_VALUE 32767 //(2^15-1)

static const uint16_t sidetone_coefficient_table_16k[] = { //13-tap
    0x0127, 0x027A, 0x0278, 0x0227,
    0xFFD5, 0xFD22, 0xFABF, 0xFAEB,
    0xFE90, 0x05EB, 0x0F47, 0x180A,
    0x1D4E};

static const uint16_t sidetone_coefficient_table_32k[] = { //29-tap
    0xFF58, 0x0063, 0x0086, 0x00BF,
    0x0100, 0x013D, 0x0169, 0x0178,
    0x0160, 0x011C, 0x00AA, 0x0011,
    0xFF5D, 0xFEA1, 0xFDF6, 0xFD75,
    0xFD39, 0xFD5A, 0xFDE8, 0xFEEA,
    0x005F, 0x0237, 0x0458, 0x069F,
    0x08E2, 0x0AF7, 0x0CB2, 0x0DF0,
    0x0E96};

static const uint16_t sidetone_coefficient_table_48k[] = { //31-tap
    0x0100, 0xFFEC, 0xFFD6, 0xFFB3,
    0xFF84, 0xFF4A, 0xFF08, 0xFEC2,
    0xFE7C, 0xFE3A, 0xFE03, 0xFDDB,
    0xFDC9, 0xFDD2, 0xFDF9, 0xFE44,
    0xFEB3, 0xFF47, 0x0000, 0x00D9,
    0x01CE, 0x02D9, 0x03F0, 0x050C,
    0x0622, 0x0729, 0x0816, 0x08DF,
    0x097E, 0x09EC, 0x0A24};

void afe_sidetone_init(void)
{
    //enable all STF path
    afe.sidetone.sidetone_out_path = (1 << AFE_SIDETONE_I2S0) |
                                     (1 << AFE_SIDETONE_I2S1) |
                                     (1 << AFE_SIDETONE_I2S2) |
                                     (1 << AFE_SIDETONE_I2S3) |
                                     (1 << AFE_SIDETONE_DL);

    afe.sidetone.sample_rate = AFE_GENERAL_SAMPLERATE_16K;
}

void afe_set_sidetone_input_path(audio_digital_block_t input_block)
{
    afe.sidetone.sidetone_in_path = input_block;
}

#if 0//modify for ab1568
static afe_general_samplerate_t afe_sidetone_samplerate_transform(afe_adda_ul_samplerate_t sampling_rate)
{
    switch (sampling_rate) {
        case AFE_ADDA_UL_SAMPLERATE_16K:
        default:
            return AFE_GENERAL_SAMPLERATE_16K;
        case AFE_ADDA_UL_SAMPLERATE_32K:
            return AFE_GENERAL_SAMPLERATE_32K;
        case AFE_ADDA_UL_SAMPLERATE_48K:
            return AFE_GENERAL_SAMPLERATE_48K;
    }
}

static void afe_set_sidetone_samplerate(audio_digital_block_t input_block)
{
    uint32_t sample_rate = AFE_GENERAL_SAMPLERATE_16K;
    switch (input_block) {
        case AUDIO_DIGITAL_BLOCK_ADDA_UL1:
            sample_rate = (AFE_GET_REG(AFE_ADDA_UL_SRC_CON0) & 0x60000) >> 17;
            afe.sidetone.sample_rate = afe_sidetone_samplerate_transform(sample_rate);
            AFE_SET_REG(AFE_SIDETONE_DEBUG, 4<<16 , 0x00070000);
            break;
        case AUDIO_DIGITAL_BLOCK_ADDA_UL2:
            sample_rate = (AFE_GET_REG(AFE_ADDA2_UL_SRC_CON0) & 0x60000) >> 17;
            afe.sidetone.sample_rate = afe_sidetone_samplerate_transform(sample_rate);
            AFE_SET_REG(AFE_SIDETONE_DEBUG, 5<<16 , 0x00070000);
            break;
        case AUDIO_DIGITAL_BLOCK_ADDA_UL3:
            sample_rate = (AFE_GET_REG(AFE_ADDA6_UL_SRC_CON0) & 0x60000) >> 17;
            afe.sidetone.sample_rate = afe_sidetone_samplerate_transform(sample_rate);
            AFE_SET_REG(AFE_SIDETONE_DEBUG, 6<<16 , 0x00070000);
            break;
        case AUDIO_DIGITAL_BLOCK_I2S0_IN:
            afe.sidetone.sample_rate = (AFE_GET_REG(AFE_I2S0_CON) & 0xf00) >> 8;
            AFE_SET_REG(AFE_SIDETONE_DEBUG, 0<<16 , 0x00070000);
            break;
        case AUDIO_DIGITAL_BLOCK_I2S1_IN:
            afe.sidetone.sample_rate = (AFE_GET_REG(AFE_I2S1_CON) & 0xf00) >> 8;
            AFE_SET_REG(AFE_SIDETONE_DEBUG, 1<<16 , 0x00070000);
            break;
        case AUDIO_DIGITAL_BLOCK_I2S2_IN:
            afe.sidetone.sample_rate = (AFE_GET_REG(AFE_I2S2_CON) & 0xf00) >> 8;
            AFE_SET_REG(AFE_SIDETONE_DEBUG, 2<<16 , 0x00070000);
            break;
        case AUDIO_DIGITAL_BLOCK_I2S3_IN:
            afe.sidetone.sample_rate = (AFE_GET_REG(AFE_I2S3_CON) & 0xf00) >> 8;
            AFE_SET_REG(AFE_SIDETONE_DEBUG, 3<<16 , 0x00070000);
            break;
        default:
            afe.sidetone.sample_rate = AFE_GENERAL_SAMPLERATE_16K;
            AFE_SET_REG(AFE_SIDETONE_DEBUG, 4<<16 , 0x00070000);
            break;
    }
}


bool afe_set_sidetone_filter(bool enable)
{
    if (enable == false) {
        //bypass STF & disable
        AFE_SET_REG (AFE_SIDETONE_CON1, 0 , 0x1f000100);
        //set gain = 0
        AFE_SET_REG (AFE_SIDETONE_GAIN, 0, 0x7ffff);
    } else {
        uint8_t sidetone_half_tap_num = 0;
        uint32_t coef_addr = 0;
        const uint16_t *sidetone_coefficient_table;

        /*get coef. support 16/32/48*/
        afe_set_sidetone_samplerate(afe.sidetone.sidetone_in_path);
        switch (afe.sidetone.sample_rate) {
            case AFE_GENERAL_SAMPLERATE_16K:
                sidetone_half_tap_num = sizeof(sidetone_coefficient_table_16k) / sizeof(uint16_t);
                sidetone_coefficient_table = sidetone_coefficient_table_16k;
                break;
            case AFE_GENERAL_SAMPLERATE_32K:
                sidetone_half_tap_num = sizeof(sidetone_coefficient_table_32k) / sizeof(uint16_t);
                sidetone_coefficient_table = sidetone_coefficient_table_32k;
                break;
            case AFE_GENERAL_SAMPLERATE_48K:
                sidetone_half_tap_num = sizeof(sidetone_coefficient_table_48k) / sizeof(uint16_t);
                sidetone_coefficient_table = sidetone_coefficient_table_48k;
            default:
                break;
        }
        /*enable STF and set coef*/
        {
            uint32_t ready_value = AFE_GET_REG(AFE_SIDETONE_CON0);
            //set STF path & enable & set half tap num */
            uint32_t write_reg_val = afe.sidetone.sidetone_out_path | (enable << 8) | sidetone_half_tap_num;
            AFE_SET_REG(AFE_SIDETONE_CON1, write_reg_val , 0x1f00013f);

            /*write coeff.*/
            for (coef_addr = 0; coef_addr < sidetone_half_tap_num; coef_addr++) {
                bool old_w_rdy = (ready_value >> 29) & 0x1;
                bool new_w_rdy = 0;
                uint32_t try_cnt = 0;
                bool ch = 0; //0:L ch, 1:R ch
                //enable w/r, read ops, select LCH as STF input
                write_reg_val = (1 << 25) | (1 << 24) | (ch << 23)| (coef_addr << 16) | sidetone_coefficient_table[coef_addr];
                AFE_SET_REG (AFE_SIDETONE_CON0, write_reg_val , 0x39fffff);

                /* wait until flag write_ready changed*/
                for (try_cnt = 0; try_cnt < 10; try_cnt++) { /* max try 10 times [align phone]*/
                    ready_value = AFE_GET_REG(AFE_SIDETONE_CON0);
                    new_w_rdy = (ready_value >> 29) & 0x1;
                    if (new_w_rdy == old_w_rdy) {
                        hal_gpt_delay_us(3);
                        if (try_cnt == 9) {
                            //[TBD: do error handle] notify
                            #if 0
                                hal_audio_afe_clock_off();
                                log_hal_msgid_warning("DSP sidetone time out\r\n", 0);
                                return;
                            #else
                                log_hal_msgid_info("DSP sidetone time out %d:%X\r\n",2, coef_addr, sidetone_coefficient_table[coef_addr]);
                            #endif
                        }
                    } else { /* state flip: ok */
                        break;
                    }
                }

                AFE_SET_REG (AFE_SIDETONE_CON0, (0 << 24) ,     0x01000000);
            }
        }
    }
    return enable;
}
#endif

void afe_set_sidetone_output_path(afe_sidetone_path_t path, bool enable)
{
    //default all off: afe.sidetone_path = 0x1f00_0000
    if (enable) {
        afe.sidetone.sidetone_out_path &= ~( 0x1 << path);
    } else {
        afe.sidetone.sidetone_out_path |= (0x1 << path);
    }
}

static void afe_set_sidetone_gain(int32_t gain)
{
    uint32_t temp_stf_gain = afe_calculate_digital_gain_index(gain, SIDETONE_AFE_0DB_REGISTER_VALUE);
    //log_hal_msgid_info("DSP sidetone negative gain %x, reg value:%x\r\n",2, gain, temp_stf_gain);
    AFE_SET_REG (AFE_SIDETONE_GAIN, (uint16_t)temp_stf_gain, 0xffff);
}

/*0dB:0 6dB:1  12dB:2  18dB:3  24dB:4*/
static void afe_set_sidetone_positve_gain(int32_t gain_01unit_db)
{
    uint32_t pos_gain =  (uint32_t)(gain_01unit_db / 600);
    if (pos_gain > 4) {
        pos_gain = 4;
    }
    //log_hal_msgid_info("DSP sidetone pos gain %x, reg value:%x\r\n",2, gain_01unit_db, pos_gain);
    AFE_SET_REG (AFE_SIDETONE_GAIN, pos_gain<<16, 0x7<<16);
}

void afe_set_sidetone_volume(int32_t gain)
{
    //call afe_set_sidetone_gain & afe_set_sidetone_positve_gain to adjust sidetone vol.
    //sidetone gain = gain + positive gain
    int32_t positve_gain, negative_gain;
    log_hal_msgid_info("DSP sidetone set volume %d\r\n",1, gain);

    if (gain>0) {
        positve_gain = gain + 599; // carry
        negative_gain = (gain%600)
                          ? (gain%600) - 600
                          : 0 ;
    } else {
        positve_gain = 0;
        negative_gain = gain;
    }

    afe_set_sidetone_positve_gain(positve_gain);
    afe_set_sidetone_gain(negative_gain);
}

void afe_set_sidetone_enable_flag(BOOL is_enable,int32_t gain)
{
    afe.sidetone.sidetone_gain = gain;
    afe.sidetone.start_flag = is_enable;
}

bool afe_get_sidetone_enable_flag(VOID)
{
    return afe.sidetone.start_flag;
}

int32_t afe_get_sidetone_gain(VOID)
{
    return afe.sidetone.sidetone_gain;
}

void afe_set_sidetone_input_channel(hal_audio_device_t audio_device)
{
    UNUSED(audio_device);
    #if 0//modify for ab1568
    afe.sidetone.channel_flag = false;// false: L_ch, true: R_ch
    if ((audio_device&HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) || (audio_device&HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL)) {
        if(((*((volatile uint32_t*)(0xA2120B04)) >> 6)&0x01) == 1) {
            afe.sidetone.channel_flag = true;
        }
    }
    #endif
}

bool afe_get_sidetone_input_channel(void)
{
    return afe.sidetone.channel_flag;
}
/*********************************************************************
*                            I2S control                             *
*********************************************************************/
#if 0//modify for ab1568
static uint32_t afe_get_i2s_reg(afe_i2s_num_t i2s_module)
{
    uint32_t i2s_reg;
    switch (i2s_module) {
        case AFE_I2S0:
        default:
            i2s_reg = AFE_I2S0_CON;
            break;
        case AFE_I2S1:
            i2s_reg = AFE_I2S1_CON;
            break;
        case AFE_I2S2:
            i2s_reg = AFE_I2S2_CON;
            break;
        case AFE_I2S3:
            i2s_reg = AFE_I2S3_CON;
            break;
    }
    return i2s_reg;
}
#endif
void afe_set_apll_for_i2s_reg(bool enable, uint32_t samplerate)
{

    log_hal_msgid_info("DSP afe_set_apll_for_i2s_reg APLL:%d, enable:%d, rate:%d,\r\n", 3, afe_get_apll_by_samplerate(samplerate), enable, samplerate);
    if (true == enable) {
        // Clear upper layer audio CG
        AFE_SET_REG(0xA2030B20, 0x040001FC, 0xFFFFFFFF);//XO_PDN_CLRD0
        AFE_SET_REG(0xA2020238, 0x01020000, 0xFFFFFFFF);//CKSYS_CLK_CFG_2

        switch (afe_get_apll_by_samplerate(samplerate)) {
            case AFE_APLL1:
                //Enable APLL 1
                AFE_WRITE8(APLL1_CTL0__F_RG_APLL1_V2I_EN, 1);
                AFE_WRITE8(APLL1_CTL0__F_RG_APLL1_DDS_PWR_ON, 1);
                hal_gpt_delay_us(5);
                AFE_WRITE8(APPL1_CTL0__F_RG_APLL1_DDS_ISO_EN, 0);
                AFE_WRITE8(APLL1_CTL1__F_RG_APLL1_EN, 1);
                AFE_WRITE8(APLL1_CTL11__F_RG_APLL1_LCDDS_PWDB, 1);
                hal_gpt_delay_us(30);

                //Setting APLL1 Tuner
                AFE_SET_REG(APLL1_CTL14__F_RG_APLL1_LCDDS_TUNER_PCW_NCPO, 0x0DE517A9, 0xFFFFFFFF);
                AFE_SET_REG(APLL1_CTL12__F_RG_APLL1_LCDDS_TUNER_PCW_NCPO, 0x00000100, 0xFFFFFFFF);
                AFE_SET_REG(APLL1_CTL13__F_RG_APLL1_LCDDS_TUNER_EN, 0x00000001, 0xFFFFFFFF);
                break;
            case AFE_APLL2:
                //Enable APLL 2
                AFE_WRITE8(APLL2_CTL0__F_RG_APLL2_V2I_EN, 1);
                AFE_WRITE8(APLL2_CTL0__F_RG_APLL2_DDS_PWR_ON, 1);
                hal_gpt_delay_us(5);
                AFE_WRITE8(APPL2_CTL0__F_RG_APLL2_DDS_ISO_EN, 0);
                AFE_WRITE8(APLL2_CTL1__F_RG_APLL2_EN, 1);
                AFE_WRITE8(APLL2_CTL11__F_RG_APLL2_LCDDS_PWDB, 1);
                hal_gpt_delay_us(30);

                //Setting APLL2 Tuner
                AFE_SET_REG(APLL2_CTL14__F_RG_APLL2_LCDDS_TUNER_PCW_NCPO, 0x0F1FAA4C, 0xFFFFFFFF);
                AFE_SET_REG(APLL2_CTL12__F_RG_APLL2_LCDDS_TUNER_PCW_NCPO, 0x00000100, 0xFFFFFFFF);
                AFE_SET_REG(APLL2_CTL13__F_RG_APLL2_LCDDS_TUNER_EN, 0x00000001, 0xFFFFFFFF);
                break;
            default:
                log_hal_msgid_warning("I2S Master not turn on APLL1 and APLL2", 0);
                break;
        }
    } else {
        switch (afe_get_apll_by_samplerate(samplerate)) {
            case AFE_APLL1:
                // Disable APLL1
                AFE_WRITE8(APLL1_CTL13__F_RG_APLL1_LCDDS_TUNER_EN, 0);
                AFE_WRITE8(APLL1_CTL1__F_RG_APLL1_EN, 0);
                AFE_WRITE8(APLL1_CTL11__F_RG_APLL1_LCDDS_PWDB, 0);
                AFE_WRITE8(APPL1_CTL0__F_RG_APLL1_DDS_ISO_EN, 1);
                AFE_WRITE8(APLL1_CTL0__F_RG_APLL1_DDS_PWR_ON, 0);
                AFE_WRITE8(APLL1_CTL0__F_RG_APLL1_V2I_EN, 0);
                AFE_WRITE8(APLL1_CTL12__F_RG_APLL1_LCDDS_TUNER_PCW_NCPO, 0);
                hal_gpt_delay_us(5);
                break;
            case AFE_APLL2:
                // Disable APLL2
                AFE_WRITE8(APLL2_CTL13__F_RG_APLL2_LCDDS_TUNER_EN, 0);
                AFE_WRITE8(APLL2_CTL1__F_RG_APLL2_EN, 0);
                AFE_WRITE8(APLL2_CTL11__F_RG_APLL2_LCDDS_PWDB, 0);
                AFE_WRITE8(APPL2_CTL0__F_RG_APLL2_DDS_ISO_EN, 1);
                AFE_WRITE8(APLL2_CTL0__F_RG_APLL2_DDS_PWR_ON, 0);
                AFE_WRITE8(APLL2_CTL0__F_RG_APLL2_V2I_EN, 0);
                AFE_WRITE8(APLL2_CTL12__F_RG_APLL2_LCDDS_TUNER_PCW_NCPO, 0);
                hal_gpt_delay_us(5);
                break;
            default:
                log_hal_msgid_warning("I2S Master not turn off APLL1 and APLL2", 0);
                break;
        }
    }
}
#if 0//moidfy for ab1568
void afe_set_i2s_mclk_reg(bool enable, afe_i2s_num_t i2s_module)
{
    uint32_t clock_divider_reg;
    uint32_t clock_divider_shift;

    if ((i2s_module == AFE_I2S0) || (i2s_module == AFE_I2S1)) {
        clock_divider_reg = 0xA2020308;
    } else {
        clock_divider_reg = 0xA202030C;
    }
    if ((i2s_module == AFE_I2S0) || (i2s_module == AFE_I2S2)) {
        clock_divider_shift = 0;
    } else {
        clock_divider_shift = 16;
    }

    if (enable) {
        if (AFE_I2S_SETTING_MCLK_SOURCE_49M) {
            AFE_WRITE(0xA2020304, 0x01010101); // I2S0/1/2/3 from hf_faud_2_ck MCLK_49M
        } else {
            AFE_WRITE(0xA2020304, 0x00000000); // I2S0/1/2/3 from hf_faud_1_ck MCLK_45M
        }
        AFE_WRITE(0xA2020238, 0x01020000); // aud_interface1=45M/aud_interface0=49M

        /* Setting audio clock divider */  //Toggled to apply apll_ck_div bit-8 or bit-24
        AFE_WRITE(clock_divider_reg, AFE_I2S_SETTING_MCLK_DIVIDER<<clock_divider_shift);
        AFE_WRITE(clock_divider_reg, (AFE_I2S_SETTING_MCLK_DIVIDER | 0x00000100)<<clock_divider_shift);
        AFE_WRITE(clock_divider_reg, AFE_I2S_SETTING_MCLK_DIVIDER<<clock_divider_shift);

        AFE_SET_REG(0xA2020300, 0<<(8*i2s_module), 1<<(8*i2s_module));
    } else {
        AFE_SET_REG(0xA2020300, 1<<(8*i2s_module), 1<<(8*i2s_module));
    }
}

void afe_set_i2s_reg(afe_i2s_num_t i2s_module, afe_i2s_role_t mode, afe_i2s_wlen_t data_length, afe_general_samplerate_t rate, afe_i2s_swap_t swap, uint32_t misc_parms)
{
    uint32_t i2s_reg = afe_get_i2s_reg(i2s_module);
    uint32_t is_i2s_format;
    is_i2s_format = (I2S_I2S == AFE_I2S_SETTING_FORMAT) ? true : false;

    AFE_SET_REG(i2s_reg, (AFE_I2S_SETTING_WORD_LENGTH)<<1, 0x00000002);// wlen
    AFE_SET_REG(i2s_reg, (mode)<<2,  0x00000004);// master mode

    AFE_SET_REG(i2s_reg, (is_i2s_format)<<3,  0x00000008);// format
    if (I2S_LJ == AFE_I2S_SETTING_FORMAT) {
        AFE_SET_REG(i2s_reg, 1<<5, 1<<5);// WS_INV
        AFE_SET_REG(i2s_reg, 0<<22, 1<<22);// out RJ mode enable(0:LJ 1: RJ)
        AFE_SET_REG(i2s_reg, 0<<30, 1<<30);// in  RJ mode enable(0:LJ 1: RJ)
    } else if (I2S_RJ == AFE_I2S_SETTING_FORMAT) {
        AFE_SET_REG(i2s_reg, 1<<5, 1<<5);// WS_INV
        AFE_SET_REG(i2s_reg, 1<<22, 1<<22);// out RJ mode enable(0:LJ 1: RJ)
        AFE_SET_REG(i2s_reg, 1<<30, 1<<30);// in  RJ mode enable(0:LJ 1: RJ)
        if (I2S_32BIT == AFE_I2S_SETTING_WORD_LENGTH) {
            // note: 24 bit data in i2s 32bit RJ mode, we need to shift 8 bit. if in 16 bit data, we have to shfit 16 bit
            if (I2S_32BIT == data_length) {
                AFE_SET_REG(i2s_reg, 8<<16, 0x1F<<16);// mI2S_OUT_SHIFT_NUM
                AFE_SET_REG(i2s_reg, 8<<24, 0x1F<<24);// mI2S_IN_SHIFT_NUM
            } else {
                AFE_SET_REG(i2s_reg, 16<<16, 0x1F<<16);// mI2S_OUT_SHIFT_NUM
                AFE_SET_REG(i2s_reg, 16<<24, 0x1F<<24);// mI2S_IN_SHIFT_NUM
            }
        }
    }
    AFE_SET_REG(i2s_reg, (rate)<<8,  0x00000F00); // sampling rate

    if(misc_parms == I2S_CLK_SOURCE_APLL) {
        AFE_SET_REG(i2s_reg, (1)<<12, 0x00001000);// LowJitterMode
    }else {
        AFE_SET_REG(i2s_reg, (0)<<12, 0x00001000);// NM Mode
    }

    AFE_SET_REG(i2s_reg, (swap)<<13, 0x00002000); // swap
    AFE_SET_REG(i2s_reg, (swap)<<14, 0x00004000); // swap
}

uint32_t afe_get_i2s_master_samplerate(afe_i2s_num_t i2s_module)
{
    uint32_t reg_value = 0;
    uint32_t i2s_reg = afe_get_i2s_reg(i2s_module);

    reg_value = (AFE_GET_REG(i2s_reg)&0x00000F00)>>8;
    return afe_reg_value_transform(reg_value, AUDIO_DIGITAL_BLOCK_I2S0_OUT);
}

void afe_i2s_enable(bool enable, afe_i2s_num_t i2s_module)
{
    uint32_t i2s_reg = afe_get_i2s_reg(i2s_module);
    AFE_SET_REG(i2s_reg, enable,  0x00000001);// enable
}


/*********************************************************************
*                         Test and dump                              *
*********************************************************************/

void afe_set_sine_gen_samplerate(afe_general_samplerate_t sample_rate)
{
    AFE_SET_REG(AFE_SGEN_CON0, sample_rate << 20, 0xf << 20);
    AFE_SET_REG(AFE_SGEN_CON0, sample_rate << 8,  0xf << 8);
}


void afe_set_sine_gen_amplitude(afe_sgen_amp_div_t amp_divide)
{
    AFE_SET_REG(AFE_SGEN_CON0, amp_divide << 17, 0x7 << 17);
    AFE_SET_REG(AFE_SGEN_CON0, amp_divide << 5,  0x7 << 5);
}


void afe_sine_gen_enable(uint32_t connection, audio_memif_direction_t direction, bool enable)
{
    if (enable) {
        AFE_SET_REG(AUDIO_TOP_CON0, 0<<AUDIO_TOP_CON0_PDN_TML_POS , AUDIO_TOP_CON0_PDN_TML_MASK);
        if (direction == AUDIO_AFE_MEMIF_DIRECTION_INPUT) {
            switch (connection) {
                case AUDIO_INTERCONNECTION_INPUT_I00:   // I2S0_IN
                case AUDIO_INTERCONNECTION_INPUT_I01:
                    AFE_SET_REG(AFE_SGEN_CON2, 0,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_INPUT_I02:   // I2S1_IN
                case AUDIO_INTERCONNECTION_INPUT_I03:
                    AFE_SET_REG(AFE_SGEN_CON2, 1,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_INPUT_I04:   // I2S2_IN
                case AUDIO_INTERCONNECTION_INPUT_I05:
                    AFE_SET_REG(AFE_SGEN_CON2, 2,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_INPUT_I06:   // I2S3_IN
                case AUDIO_INTERCONNECTION_INPUT_I07:
                    AFE_SET_REG(AFE_SGEN_CON2, 3,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_INPUT_I08:   // ADDA UL1
                case AUDIO_INTERCONNECTION_INPUT_I09:
                    AFE_SET_REG(AFE_SGEN_CON2, 4,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_INPUT_I10:   // ADDA UL2
                case AUDIO_INTERCONNECTION_INPUT_I11:
                    AFE_SET_REG(AFE_SGEN_CON2, 5,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_INPUT_I12:   // ADDA UL3
                case AUDIO_INTERCONNECTION_INPUT_I13:
                    AFE_SET_REG(AFE_SGEN_CON2, 6,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_INPUT_I14:   // HW GAIN 1 IN
                case AUDIO_INTERCONNECTION_INPUT_I15:
                    AFE_SET_REG(AFE_SGEN_CON2, 7,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_INPUT_I16:   // HW GAIN 2 IN
                case AUDIO_INTERCONNECTION_INPUT_I17:
                    AFE_SET_REG(AFE_SGEN_CON2, 8,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_INPUT_I18:   // DL1 agent
                case AUDIO_INTERCONNECTION_INPUT_I19:
                    AFE_SET_REG(AFE_SGEN_CON2, 9,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_INPUT_I20:   // DL2 agent
                case AUDIO_INTERCONNECTION_INPUT_I21:
                    AFE_SET_REG(AFE_SGEN_CON2, 10,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_INPUT_I22:   // DL3 agent
                case AUDIO_INTERCONNECTION_INPUT_I23:
                    AFE_SET_REG(AFE_SGEN_CON2, 11,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_INPUT_I24:   // DL12 agent
                case AUDIO_INTERCONNECTION_INPUT_I25:
                    AFE_SET_REG(AFE_SGEN_CON2, 12,  0x003f);
                    break;
                default:
                    break;
            }
        } else { // AUDIO_AFE_MEMIF_DIRECTION_OUTPUT
            switch (connection) {
                case AUDIO_INTERCONNECTION_OUTPUT_O00:  // I2S0_OUT
                case AUDIO_INTERCONNECTION_OUTPUT_O01:
                    AFE_SET_REG(AFE_SGEN_CON2, 32,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_OUTPUT_O02:  // I2S1_OUT
                case AUDIO_INTERCONNECTION_OUTPUT_O03:
                    AFE_SET_REG(AFE_SGEN_CON2, 33,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_OUTPUT_O04:  // I2S2_OUT
                case AUDIO_INTERCONNECTION_OUTPUT_O05:
                    AFE_SET_REG(AFE_SGEN_CON2, 34,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_OUTPUT_O06:  // I2S3_OUT
                case AUDIO_INTERCONNECTION_OUTPUT_O07:
                    AFE_SET_REG(AFE_SGEN_CON2, 35,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_OUTPUT_O08:  // ADDA DL1 OUT
                case AUDIO_INTERCONNECTION_OUTPUT_O09:
                    AFE_SET_REG(AFE_SGEN_CON2, 36,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_OUTPUT_O10:  // STF
                case AUDIO_INTERCONNECTION_OUTPUT_O11:
                    AFE_SET_REG(AFE_SGEN_CON2, 37,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_OUTPUT_O12:  // HW GAIN 1 OUT
                case AUDIO_INTERCONNECTION_OUTPUT_O13:
                    AFE_SET_REG(AFE_SGEN_CON2, 38,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_OUTPUT_O14:  // HW GAIN 2 OUT
                case AUDIO_INTERCONNECTION_OUTPUT_O15:
                    AFE_SET_REG(AFE_SGEN_CON2, 39,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_OUTPUT_O16:  // VUL1 agent
                case AUDIO_INTERCONNECTION_OUTPUT_O17:
                    AFE_SET_REG(AFE_SGEN_CON2, 40,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_OUTPUT_O18:  // VUL2 agent
                case AUDIO_INTERCONNECTION_OUTPUT_O19:
                    AFE_SET_REG(AFE_SGEN_CON2, 41,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_OUTPUT_O20:  // AWB agent
                case AUDIO_INTERCONNECTION_OUTPUT_O21:
                    AFE_SET_REG(AFE_SGEN_CON2, 42,  0x003f);
                    break;
                case AUDIO_INTERCONNECTION_OUTPUT_O22:  // AWB2 agent
                case AUDIO_INTERCONNECTION_OUTPUT_O23:
                    AFE_SET_REG(AFE_SGEN_CON2, 43,  0x003f);
                    break;
                default:
                    break;
            }
        }
        AFE_SET_REG(AFE_SGEN_CON0, 0x1 << 26,  0x1 << 26); //enable sine
    } else {
        AFE_SET_REG(AFE_SGEN_CON0, 0,  0x1 << 26);
        AFE_SET_REG(AFE_SGEN_CON2, 0,  0x003f);
        AFE_SET_REG(AUDIO_TOP_CON0, 1<<AUDIO_TOP_CON0_PDN_TML_POS , AUDIO_TOP_CON0_PDN_TML_MASK);
    }
}


void afe_dump_analog_reg(void)
{
    log_hal_msgid_info("AUDENC_ANA_CON0 =0x%x\r\n",1,  ANA_READ(AUDENC_ANA_CON0));
    log_hal_msgid_info("AUDENC_ANA_CON1 =0x%x\r\n",1,  ANA_READ(AUDENC_ANA_CON1));
    log_hal_msgid_info("AUDENC_ANA_CON2 =0x%x\r\n",1,  ANA_READ(AUDENC_ANA_CON2));
    log_hal_msgid_info("AUDENC_ANA_CON3 =0x%x\r\n",1,  ANA_READ(AUDENC_ANA_CON3));
    log_hal_msgid_info("AUDENC_ANA_CON4 =0x%x\r\n",1,  ANA_READ(AUDENC_ANA_CON4));
    log_hal_msgid_info("AUDENC_ANA_CON5 =0x%x\r\n",1,  ANA_READ(AUDENC_ANA_CON5));
    log_hal_msgid_info("AUDENC_ANA_CON6 =0x%x\r\n",1,  ANA_READ(AUDENC_ANA_CON6));
    log_hal_msgid_info("AUDENC_ANA_CON7 =0x%x\r\n",1,  ANA_READ(AUDENC_ANA_CON7));
    log_hal_msgid_info("AUDENC_ANA_CON8 =0x%x\r\n",1,  ANA_READ(AUDENC_ANA_CON8));
    log_hal_msgid_info("AUDENC_ANA_CON9 =0x%x\r\n",1,  ANA_READ(AUDENC_ANA_CON9));
    log_hal_msgid_info("AUDENC_ANA_CON10 =0x%x\r\n",1, ANA_READ(AUDENC_ANA_CON10));
    log_hal_msgid_info("AUDENC_ANA_CON11 =0x%x\r\n",1, ANA_READ(AUDENC_ANA_CON11));
    log_hal_msgid_info("AUDENC_ANA_CON12 =0x%x\r\n",1, ANA_READ(AUDENC_ANA_CON12));
    log_hal_msgid_info("AUDENC_ANA_CON13 =0x%x\r\n",1, ANA_READ(AUDENC_ANA_CON13));
    log_hal_msgid_info("AUDENC_ANA_CON14 =0x%x\r\n",1, ANA_READ(AUDENC_ANA_CON14));

    log_hal_msgid_info("AUDDEC_ANA_CON0 =0x%x\r\n",1,  ANA_READ(AUDDEC_ANA_CON0));
    log_hal_msgid_info("AUDDEC_ANA_CON1 =0x%x\r\n",1,  ANA_READ(AUDDEC_ANA_CON1));
    log_hal_msgid_info("AUDDEC_ANA_CON2 =0x%x\r\n",1,  ANA_READ(AUDDEC_ANA_CON2));
    log_hal_msgid_info("AUDDEC_ANA_CON3 =0x%x\r\n",1,  ANA_READ(AUDDEC_ANA_CON3));
    log_hal_msgid_info("AUDDEC_ANA_CON4 =0x%x\r\n",1,  ANA_READ(AUDDEC_ANA_CON4));
    log_hal_msgid_info("AUDDEC_ANA_CON5 =0x%x\r\n",1,  ANA_READ(AUDDEC_ANA_CON5));
    log_hal_msgid_info("AUDDEC_ANA_CON6 =0x%x\r\n",1,  ANA_READ(AUDDEC_ANA_CON6));
    log_hal_msgid_info("AUDDEC_ANA_CON7 =0x%x\r\n",1,  ANA_READ(AUDDEC_ANA_CON7));
    log_hal_msgid_info("AUDDEC_ANA_CON8 =0x%x\r\n",1,  ANA_READ(AUDDEC_ANA_CON8));
    log_hal_msgid_info("AUDDEC_ANA_CON9 =0x%x\r\n",1,  ANA_READ(AUDDEC_ANA_CON9));
    log_hal_msgid_info("AUDDEC_ANA_CON10 =0x%x\r\n",1, ANA_READ(AUDDEC_ANA_CON10));
    log_hal_msgid_info("AUDDEC_ANA_CON11 =0x%x\r\n",1, ANA_READ(AUDDEC_ANA_CON11));
    log_hal_msgid_info("AUDDEC_ANA_CON12 =0x%x\r\n",1, ANA_READ(AUDDEC_ANA_CON12));
    log_hal_msgid_info("AUDDEC_ANA_CON13 =0x%x\r\n",1, ANA_READ(AUDDEC_ANA_CON13));
    log_hal_msgid_info("AUDDEC_ANA_CON14 =0x%x\r\n",1, ANA_READ(AUDDEC_ANA_CON14));
}
#endif

#if 0//moidfy for ab1568
afe_audio_bt_sync_enable_t afe_get_bt_sync_enable_bit (audio_digital_block_t mem_block)
{
    afe_audio_bt_sync_enable_t bt_sync_bit = 0;

    switch (mem_block) {
        case AUDIO_DIGITAL_BLOCK_MEM_DL1:
            bt_sync_bit = AUDIO_BT_SYNC_DL1_EN;
            break;
        case AUDIO_DIGITAL_BLOCK_MEM_DL2:
            bt_sync_bit = AUDIO_BT_SYNC_DL2_EN;
            break;
        case AUDIO_DIGITAL_BLOCK_MEM_DL3:
            bt_sync_bit = AUDIO_BT_SYNC_DL3_EN;
            break;
        case AUDIO_DIGITAL_BLOCK_MEM_DL12:
            bt_sync_bit = AUDIO_BT_SYNC_DL12_EN;
            break;
        default:
            break;
    }
    return bt_sync_bit;
}
#endif

void afe_bt_sync_enable(bool enable, audio_digital_block_t mem_block)
{
    afe_audio_bt_sync_enable_t sync_agent = afe_get_bt_sync_enable_bit(mem_block);
    if (enable) {
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0200,    0x0200);            //init_dly_cnt powered on
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0400,    0x0400);            //DL all agents auto enable on
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0, sync_agent,   sync_agent);            //switch auto enable agents on
    } else {
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0, ~sync_agent,    sync_agent);           //switch auto enable agents off
        if (!(AFE_READ(AFE_AUDIO_BT_SYNC_CON0)&0xF0000)) {
            AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0, 0x0000,     0x0200);        //init_dly_cnt powered off
            AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0, 0x0000,     0x0400);        //DL all agents auto enable off
        }
    }
}

uint32_t afe_get_bt_sync_monitor(audio_digital_block_t mem_block)
{
    uint32_t reg_value;

    reg_value = AFE_GET_REG(AFE_AUDIO_BT_SYNC_MON1);
    if(mem_block == AUDIO_DIGITAL_BLOCK_MEM_DL1) {
        reg_value = reg_value&0xFFFF;
    } else {
        reg_value = (reg_value>>16)&0xFFFF;
    }
    return reg_value;
}

uint32_t afe_get_bt_sync_monitor_state(audio_digital_block_t mem_block)
{
    uint32_t reg_value;

    reg_value = AFE_GET_REG(AFE_AUDIO_BT_SYNC_MON2);
    if(mem_block == AUDIO_DIGITAL_BLOCK_MEM_DL1) {
        reg_value = reg_value&0x10;
    } else if (mem_block == AUDIO_DIGITAL_BLOCK_MEM_DL2) {
        reg_value = reg_value&0x20;
    } else if (mem_block == AUDIO_DIGITAL_BLOCK_MEM_DL3) {
        reg_value = reg_value&0x80;
    } else if (mem_block == AUDIO_DIGITAL_BLOCK_MEM_DL12) {
        reg_value = reg_value&0x40;
    }
    return reg_value;
}

void afe_26m_xtal_cnt_enable(bool enable)
{
    if (enable) {
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x1000,    0x1000);            //reset the xtal_cnt to zero
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0000,    0x1000);

        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0100,    0x0100);            //xtal_cnt powered on

        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0000,    0x0010);            //audio_get_cnt_en from BT

        #if 1
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0000,    0x0001);            //audio_play_en from BT
        #else
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0001,    0x0001);            //audio_play_en controlled by audio_play_en_int
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0002,    0x0002);            //audio_play_en_int
        #endif
    } else {
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0000,    0x0100);            //xtal_cnt powered off
    }
}

int32_t afe_power_on_asrc(afe_mem_asrc_id_t asrc_id, bool on)
{
    UNUSED(asrc_id);
    UNUSED(on);

    uint32_t pos;

    if (asrc_id >= MEM_ASRC_NUM) {
        log_hal_msgid_error("afe_power_on_asrc() error: invalid asrc[%d]\n", 1, asrc_id);
        return -1;
    }
    if (on) {
        /* asrc_ck select asm_h_ck(270M) */
        pos = asrc_id * 3;
        AFE_SET_REG(PWR2_ASM_CON2, 0x2 << pos, 0x3 << pos);

        pos = PWR2_TOP_CON_PDN_MEM_ASRC1_POS + asrc_id;
        AFE_SET_REG(PWR2_TOP_CON, 0<<pos, 1<<pos);

        /* force reset */
        pos = PWR2_ASM_CON2_MEM_ASRC_1_RESET_POS + asrc_id;
        AFE_SET_REG(PWR2_ASM_CON2, 1<<pos, 1<<pos);
        AFE_SET_REG(PWR2_ASM_CON2, 0<<pos, 1<<pos);
    } else {
        pos = PWR2_TOP_CON_PDN_MEM_ASRC1_POS + asrc_id;
        AFE_SET_REG(PWR2_TOP_CON, 0<<pos, 1<<pos);
    }
    return 0;

}

static inline uint32_t afe_asrc_get_samplingrate_to_palette(uint32_t rate)
{
    UNUSED(rate);

    return rate * 4096 / 100;
}

static uint32_t afe_asrc_get_period_palette(uint32_t rate, bool clk45m)
{
    UNUSED(rate);
    UNUSED(clk45m);

    switch (rate) {
    case 8000:
        return clk45m ? 0x058332 : 0x060000;
    case 12000:
        return clk45m ? 0x03accc : 0x040000;
    case 16000:
        return clk45m ? 0x02c199 : 0x030000;
    case 24000:
        return clk45m ? 0x01d666 : 0x020000;
    case 32000:
        return clk45m ? 0x0160cc : 0x018000;
    case 48000:
        return clk45m ? 0x00eb33 : 0x010000;
    case 96000:
        return clk45m ? 0x007599 : 0x008000;
    case 192000:
        return clk45m ? 0x003acd : 0x004000;
    case 384000:
        return clk45m ? 0x001d66 : 0x002000;
    case 7350:
        return clk45m ? 0x060000 : 0x0687D8;
    case 11025:
        return clk45m ? 0x040000 : 0x045A90;
    case 14700:
        return clk45m ? 0x030000 : 0x0343EC;
    case 22050:
        return clk45m ? 0x020000 : 0x022D48;
    case 29400:
        return clk45m ? 0x018000 : 0x01A1F6;
    case 44100:
        return clk45m ? 0x010000 : 0x0116A4;
    case 88200:
        return clk45m ? 0x008000 : 0x008B52;
    case 176400:
        return clk45m ? 0x004000 : 0x0045A9;
    case 352800:
        return clk45m ? 0x002000 : 0x0022D4;    /* ??? */
    default:
        return 0x0;
    }
    return 0;
}
#define MEMASRC_IIR_COEF_SIZE (48)


//IIR_COEF_384_TO_192
static const uint32_t iir_coefficient_2_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x04ec93a4, 0x000a1bf6, 0x04ec93a4, 0x1a59368b, 0xc11e7000, 0x00000001, 0x0b9bf8b5, 0x00b44721,
    0x0b9bf8b5, 0x0e62af3f, 0xe1c4dfac, 0x00000002, 0x09fe478b, 0x01c85505, 0x09fe478b, 0x112b48ef,
    0xe34aeed8, 0x00000002, 0x07b86f3c, 0x02fd7211, 0x07b86f3c, 0x15cb1caf, 0xe558423d, 0x00000002,
    0x0a1e67fb, 0x0754a987, 0x0a1e67fb, 0x1c63a076, 0xe80ae60d, 0x00000002, 0x0522203a, 0x0635518b,
    0x0522203a, 0x245624f0, 0xeb304910, 0x00000002, 0x01bfb83a, 0x030c2050, 0x01bfb83a, 0x2b7a0a3d,
    0xedfa64fe, 0x00000002, 0x00000000, 0x2c9308ca, 0x2c9308ca, 0x02e75633, 0x00000000, 0x00000005
};

//IIR_COEF_384_TO_128
static const uint32_t iir_coefficient_3_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x02894a52, 0xfd770f7a, 0x02894a52, 0x27008598, 0xe06656ba, 0x00000002, 0x0b4d22b9, 0xf51b5d44,
    0x0b4d22b9, 0x274e7f44, 0xe1463b51, 0x00000002, 0x0976ef3e, 0xf7ac51fb, 0x0976ef3e, 0x28672e65,
    0xe26470ac, 0x00000002, 0x06e6a0fe, 0xfb12646f, 0x06e6a0fe, 0x2a5a082f, 0xe3e6aafa, 0x00000002,
    0x081c5ea5, 0xfcccf1de, 0x081c5ea5, 0x2d20d986, 0xe5d97751, 0x00000002, 0x036a7c7a, 0x00c2723e,
    0x036a7c7a, 0x305e8d68, 0xe80a0765, 0x00000002, 0x00e763b5, 0x0123b6ba, 0x00e763b5, 0x332864f5,
    0xe9e51ce6, 0x00000002, 0x00000000, 0x38c17ba1, 0x38c17ba1, 0x068927ea, 0x00000000, 0x00000004
};

//IIR_COEF_384_TO_256
static const uint32_t iir_coefficient_3_to_2[MEMASRC_IIR_COEF_SIZE] = {
    0x0539819f, 0x054951e5, 0x0539819f, 0xda57122a, 0xc12ba871, 0x00000001, 0x0cf35b0b, 0x0da330e8,
    0x0cf35b0b, 0xef0f9d73, 0xe1dc9491, 0x00000002, 0x0bb8d79a, 0x0d5e3977, 0x0bb8d79a, 0xf2db2d6f,
    0xe385013c, 0x00000002, 0x09e15952, 0x0ca91405, 0x09e15952, 0xf9444c2e, 0xe5e42680, 0x00000002,
    0x0ecd2494, 0x15e07e27, 0x0ecd2494, 0x03307784, 0xe954c12e, 0x00000002, 0x08f68ac2, 0x0f542aa7,
    0x08f68ac2, 0x10cf62d9, 0xedef5cfc, 0x00000002, 0x074829ae, 0x0df259cb, 0x074829ae, 0x3e16cf8a,
    0xe566834f, 0x00000001, 0x00000000, 0x2263d8b7, 0x2263d8b7, 0x012d626d, 0x00000000, 0x00000006
};

//IIR_COEF_384_TO_96
static const uint32_t iir_coefficient_4_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x023fb27b, 0xfcd2f985, 0x023fb27b, 0x326cca6b, 0xe056495c, 0x00000002, 0x0a37a3a1, 0xf1d5e48b,
    0x0a37a3a1, 0x3265763d, 0xe110322b, 0x00000002, 0x084e4d06, 0xf4ffdd82, 0x084e4d06, 0x32d2c1e7,
    0xe1f44efd, 0x00000002, 0x05c29422, 0xf91777ea, 0x05c29422, 0x33ae7ff0, 0xe3183fb3, 0x00000002,
    0x064224ff, 0xfa1849cc, 0x064224ff, 0x34e75649, 0xe47c15ec, 0x00000002, 0x0253663f, 0xff1b8654,
    0x0253663f, 0x36483578, 0xe5f577b6, 0x00000002, 0x00816bda, 0x006b4ca3, 0x00816bda, 0x376c1778,
    0xe725c430, 0x00000002, 0x00000000, 0x28b0e793, 0x28b0e793, 0x06fbe6c3, 0x00000000, 0x00000004
};

//IIR_COEF_256_TO_192
static const uint32_t iir_coefficient_4_to_3[MEMASRC_IIR_COEF_SIZE] = {
    0x02c5a70f, 0x03eb90a0, 0x02c5a70f, 0xdc8ebb5e, 0xe08256c1, 0x00000002, 0x0df345b2, 0x141cbadb,
    0x0df345b2, 0xde56eee1, 0xe1a284a2, 0x00000002, 0x0d03b121, 0x138292e8, 0x0d03b121, 0xe1c608f0,
    0xe3260cbe, 0x00000002, 0x0b8948fb, 0x1254a1d4, 0x0b8948fb, 0xe7c0c6a5, 0xe570d1c6, 0x00000002,
    0x12a7cba4, 0x1fe15ef0, 0x12a7cba4, 0xf1bd349d, 0xe911d52a, 0x00000002, 0x0c882b79, 0x1718bc3a,
    0x0c882b79, 0x013ec33c, 0xee982998, 0x00000002, 0x0b5bd89b, 0x163580e0, 0x0b5bd89b, 0x2873f220,
    0xea9edbcb, 0x00000001, 0x00000000, 0x2c155c70, 0x2c155c70, 0x00f204d6, 0x00000000, 0x00000006
};

//IIR_COEF_384_TO_64
static const uint32_t iir_coefficient_6_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x029d40ba, 0xfb79d6e0, 0x029d40ba, 0x392d8096, 0xe034dd0f, 0x00000002, 0x05a2a7fa, 0xf64f6c2e,
    0x05a2a7fa, 0x1c817399, 0xf0551425, 0x00000003, 0x0953d637, 0xf036ffec, 0x0953d637, 0x38ff5268,
    0xe14354e5, 0x00000002, 0x06952665, 0xf53ce19a, 0x06952665, 0x391c1353, 0xe2158fe4, 0x00000002,
    0x072672d6, 0xf538294e, 0x072672d6, 0x39539b19, 0xe32755ee, 0x00000002, 0x027d1bb9, 0xfd10e42d,
    0x027d1bb9, 0x399a1610, 0xe45ace52, 0x00000002, 0x006a8eb3, 0xfff50a50, 0x006a8eb3, 0x39d826e9,
    0xe55db161, 0x00000002, 0x00000000, 0x1c6ede8b, 0x1c6ede8b, 0x073e2bc6, 0x00000000, 0x00000004
};

//IIR_COEF_384_TO_48
static const uint32_t iir_coefficient_8_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x02d975a0, 0xfabc9d25, 0x02d975a0, 0x3bece279, 0xe024cd2f, 0x00000002, 0x05f3612a, 0xf50b0a4f,
    0x05f3612a, 0x1de062d7, 0xf03c03e5, 0x00000003, 0x04ff0085, 0xf6df7cf4, 0x04ff0085, 0x1dd063ae,
    0xf074a058, 0x00000003, 0x072ec2f0, 0xf31a41fd, 0x072ec2f0, 0x3b84724d, 0xe18bfdf7, 0x00000002,
    0x07ec0168, 0xf2572ea5, 0x07ec0168, 0x3b67118c, 0xe269bd00, 0x00000002, 0x02ba0b16, 0xfbd5e151,
    0x02ba0b16, 0x3b48b8a8, 0xe36d4fda, 0x00000002, 0x00662a40, 0xffb4d0a9, 0x00662a40, 0x3b2fa355,
    0xe44f3782, 0x00000002, 0x00000000, 0x2bc64b7d, 0x2bc64b7d, 0x0ec96668, 0x00000000, 0x00000003
};

//IIR_COEF_256_TO_96
static const uint32_t iir_coefficient_8_to_3[MEMASRC_IIR_COEF_SIZE] = {
    0x02545a7f, 0xfe3ea711, 0x02545a7f, 0x2238ae5c, 0xe07931d4, 0x00000002, 0x0ac115ca, 0xf860f5e9,
    0x0ac115ca, 0x22bbb3c8, 0xe17e093d, 0x00000002, 0x08f408eb, 0xfa958f42, 0x08f408eb, 0x244477a8,
    0xe2c04629, 0x00000002, 0x0682ca04, 0xfd584bf8, 0x0682ca04, 0x26e0e9c4, 0xe463563b, 0x00000002,
    0x07baafe9, 0xff9d399b, 0x07baafe9, 0x2a7e74b7, 0xe66ef1dd, 0x00000002, 0x0362b43a, 0x01f07186,
    0x0362b43a, 0x2e9e4f86, 0xe8abd681, 0x00000002, 0x00f873cc, 0x016b4c05, 0x00f873cc, 0x321b967a,
    0xea8835e8, 0x00000002, 0x00000000, 0x1f82bf79, 0x1f82bf79, 0x03381d0d, 0x00000000, 0x00000005
};

//IIR_COEF_352_TO_32
static const uint32_t iir_coefficient_11_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x0314039e, 0xfa16c22d, 0x0314039e, 0x3dc6a869, 0xe0185784, 0x00000002, 0x063c1999, 0xf40b68ae,
    0x063c1999, 0x1ed08ff8, 0xf0283846, 0x00000003, 0x0550267e, 0xf5d9545e, 0x0550267e, 0x1ebccb61,
    0xf04ff1ec, 0x00000003, 0x07cc8c28, 0xf137f60c, 0x07cc8c28, 0x3d467c42, 0xe1176705, 0x00000002,
    0x08cb7062, 0xefa566fe, 0x08cb7062, 0x3d01dd91, 0xe1c1daac, 0x00000002, 0x030c40f1, 0xfaa57571,
    0x030c40f1, 0x3cafcf6a, 0xe2923943, 0x00000002, 0x00685a9f, 0xff7ab6e7, 0x00685a9f, 0x3c665fb6,
    0xe34e3424, 0x00000002, 0x00000000, 0x2046052b, 0x2046052b, 0x0f11f8dd, 0x00000000, 0x00000003
};

//IIR_COEF_352_TO_64
static const uint32_t iir_coefficient_11_to_2[MEMASRC_IIR_COEF_SIZE] = {
    0x02a6b37b, 0xfb888156, 0x02a6b37b, 0x37f859ff, 0xe0386456, 0x00000002, 0x05b20926, 0xf679dff1,
    0x05b20926, 0x1be93c6e, 0xf05adf16, 0x00000003, 0x09753fb7, 0xf07a0567, 0x09753fb7, 0x37dcb603,
    0xe15a4054, 0x00000002, 0x06b68dd9, 0xf56b6f20, 0x06b68dd9, 0x3811806b, 0xe23d69ee, 0x00000002,
    0x075ca584, 0xf5743cd8, 0x075ca584, 0x386b6017, 0xe367180a, 0x00000002, 0x029bfc8d, 0xfd367105,
    0x029bfc8d, 0x38da2ce4, 0xe4b768fc, 0x00000002, 0x007521f2, 0x0006baaa, 0x007521f2, 0x393b06d9,
    0xe5d3fa99, 0x00000002, 0x00000000, 0x1ee87e92, 0x1ee87e92, 0x072c4a47, 0x00000000, 0x00000004
};

//IIR_COEF_352_TO_96
static const uint32_t iir_coefficient_11_to_3[MEMASRC_IIR_COEF_SIZE] = {
    0x0217fde7, 0xfd41917c, 0x0217fde7, 0x30c91f13, 0xe060d9b0, 0x00000002, 0x09b6b0aa, 0xf39321ee,
    0x09b6b0aa, 0x30cfc632, 0xe12f3348, 0x00000002, 0x07d62437, 0xf68e9f4a, 0x07d62437, 0x3163f76d,
    0xe2263924, 0x00000002, 0x05647e38, 0xfa4bf362, 0x05647e38, 0x327c9f18, 0xe3598145, 0x00000002,
    0x05d9333f, 0xfb86a733, 0x05d9333f, 0x3400e913, 0xe4c6093b, 0x00000002, 0x02366680, 0xffa5d34a,
    0x02366680, 0x35ada3bf, 0xe63fbbf7, 0x00000002, 0x0082c4df, 0x0084bd96, 0x0082c4df, 0x370ab423,
    0xe76b048a, 0x00000002, 0x00000000, 0x2b47dcde, 0x2b47dcde, 0x06f26612, 0x00000000, 0x00000004
};

//IIR_COEF_352_TO_128
static const uint32_t iir_coefficient_11_to_4[MEMASRC_IIR_COEF_SIZE] = {
    0x02449fa8, 0xfe2712a2, 0x02449fa8, 0x24311459, 0xe0787f65, 0x00000002, 0x0a84fc5d, 0xf7e60ffe,
    0x0a84fc5d, 0x24a509fa, 0xe17ae496, 0x00000002, 0x08b52aad, 0xfa3093f6, 0x08b52aad, 0x2612ff28,
    0xe2b72e39, 0x00000002, 0x064680dc, 0xfd0ab6a4, 0x064680dc, 0x2882e116, 0xe44dae31, 0x00000002,
    0x075b08c6, 0xff2a08c3, 0x075b08c6, 0x2bdcc672, 0xe6431f3f, 0x00000002, 0x032b3b9b, 0x01a2f50e,
    0x032b3b9b, 0x2fa5cc8a, 0xe860c732, 0x00000002, 0x00e4486e, 0x01476fb8, 0x00e4486e, 0x32d1c2b2,
    0xea1e3cba, 0x00000002, 0x00000000, 0x3c8f8f79, 0x3c8f8f79, 0x0682ad88, 0x00000000, 0x00000004
};

//IIR_COEF_352_TO_192
static const uint32_t iir_coefficient_11_to_6[MEMASRC_IIR_COEF_SIZE] = {
    0x04bd6a83, 0x015bba25, 0x04bd6a83, 0x0d7cdab3, 0xc137e9aa, 0x00000001, 0x0b80613f, 0x03ed2155,
    0x0b80613f, 0x0839ad63, 0xe1ea8af7, 0x00000002, 0x09f38e84, 0x049744b0, 0x09f38e84, 0x0b793c54,
    0xe38a003b, 0x00000002, 0x0f90a19e, 0x0a5f3f69, 0x0f90a19e, 0x21996102, 0xcb6599b5, 0x00000001,
    0x0a71465a, 0x0a3901b2, 0x0a71465a, 0x185f028c, 0xe8856f0e, 0x00000002, 0x057f4d69, 0x07aa76ed,
    0x057f4d69, 0x2185beab, 0xebd12f96, 0x00000002, 0x01f3ece3, 0x0389a7a8, 0x01f3ece3, 0x29ccd8d7,
    0xeec1a5bb, 0x00000002, 0x00000000, 0x30ea5629, 0x30ea5629, 0x02d46d34, 0x00000000, 0x00000005
};

//IIR_COEF_352_TO_256
static const uint32_t iir_coefficient_11_to_8[MEMASRC_IIR_COEF_SIZE] = {
    0x02b9c432, 0x038d00b6, 0x02b9c432, 0xe15c8a85, 0xe0898362, 0x00000002, 0x0da7c09d, 0x12390f60,
    0x0da7c09d, 0xe3369d52, 0xe1b8417b, 0x00000002, 0x0ca047cb, 0x11b06e35, 0x0ca047cb, 0xe6d2ee1d,
    0xe34b164a, 0x00000002, 0x0b06ae31, 0x109ad63c, 0x0b06ae31, 0xed0d2221, 0xe5a278a0, 0x00000002,
    0x116c5a39, 0x1cb58b13, 0x116c5a39, 0xf7341949, 0xe93da732, 0x00000002, 0x0b5486de, 0x147ddf59,
    0x0b5486de, 0x0657d3a0, 0xee813f4a, 0x00000002, 0x09e7f722, 0x13449867, 0x09e7f722, 0x2fcbde4b,
    0xe91f9b09, 0x00000001, 0x00000000, 0x28cd51d5, 0x28cd51d5, 0x01061d85, 0x00000000, 0x00000006
};

//IIR_COEF_384_TO_32
static const uint32_t iir_coefficient_12_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x032f10f1, 0xf9d95961, 0x032f10f1, 0x3e1158bc, 0xe01583e6, 0x00000002, 0x065bac8d, 0xf3bb21b9,
    0x065bac8d, 0x1ef747e9, 0xf023c270, 0x00000003, 0x0574b803, 0xf580c27d, 0x0574b803, 0x1ee3c915,
    0xf047d1e6, 0x00000003, 0x08179a23, 0xf0885da5, 0x08179a23, 0x3d927e61, 0xe0fe5dc7, 0x00000002,
    0x093e2f0c, 0xee9b6c3c, 0x093e2f0c, 0x3d48a322, 0xe19f928b, 0x00000002, 0x033d2e85, 0xfa2f0f19,
    0x033d2e85, 0x3cedff76, 0xe2689466, 0x00000002, 0x006ded19, 0xff67ddb5, 0x006ded19, 0x3c9b5197,
    0xe320f682, 0x00000002, 0x00000000, 0x1e1581e2, 0x1e1581e2, 0x0f1e283c, 0x00000000, 0x00000003
};

//IIR_COEF_384_TO_352
static const uint32_t iir_coefficient_12_to_11[MEMASRC_IIR_COEF_SIZE] = {
    0x0303b6d0, 0x05cdf456, 0x0303b6d0, 0xc416d6b5, 0xe0363059, 0x00000002, 0x07cc27b7, 0x0f0ba03c,
    0x07cc27b7, 0xe25f513e, 0xf058cf70, 0x00000003, 0x07aa57c1, 0x0eda62f1, 0x07aa57c1, 0xe2f03496,
    0xf0b1a683, 0x00000003, 0x076f8c07, 0x0e7fe0ff, 0x076f8c07, 0xe3f982eb, 0xf1488afb, 0x00000003,
    0x0e01e7ef, 0x1b8868ec, 0x0e01e7ef, 0xe607f80e, 0xf26bcf29, 0x00000003, 0x0c3237da, 0x182c4952,
    0x0c3237da, 0xea8a804f, 0xf4e4c6ab, 0x00000003, 0x1082b5c7, 0x20f0495b, 0x1082b5c7, 0xe93bbad7,
    0xf4ce9040, 0x00000002, 0x00000000, 0x2b58b702, 0x2b58b702, 0xfff6882b, 0x00000000, 0x00000007
};

//IIR_COEF_384_TO_24
static const uint32_t iir_coefficient_16_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x038fcda4, 0xf9036a1f, 0x038fcda4, 0x3eda69b0, 0xe00d87a5, 0x00000002, 0x06bff5be, 0xf2c44847,
    0x06bff5be, 0x1f609be0, 0xf016fc9b, 0x00000003, 0x05eecea4, 0xf46276e0, 0x05eecea4, 0x1f4fdbce,
    0xf02ffbe3, 0x00000003, 0x0490e7d9, 0xf716aef0, 0x0490e7d9, 0x1f35bb3e, 0xf059477f, 0x00000003,
    0x05790864, 0xf56662d5, 0x05790864, 0x1f0d7f0d, 0xf09a0d56, 0x00000003, 0x0405a7b9, 0xf863223b,
    0x0405a7b9, 0x3dafa23c, 0xe1e1ec18, 0x00000002, 0x0087c12c, 0xff1dcc3e, 0x0087c12c, 0x3d46e81a,
    0xe28bc951, 0x00000002, 0x00000000, 0x2e64814a, 0x2e64814a, 0x1e8c999c, 0x00000000, 0x00000002
};

//IIR_COEF_256_TO_48
static const uint32_t iir_coefficient_16_to_3[MEMASRC_IIR_COEF_SIZE] = {
    0x02b72fb4, 0xfb7c5152, 0x02b72fb4, 0x374ab8ef, 0xe039095c, 0x00000002, 0x05ca62de, 0xf673171b,
    0x05ca62de, 0x1b94186a, 0xf05c2de7, 0x00000003, 0x09a9656a, 0xf05ffe29, 0x09a9656a, 0x37394e81,
    0xe1611f87, 0x00000002, 0x06e86c29, 0xf54bf713, 0x06e86c29, 0x37797f41, 0xe24ce1f6, 0x00000002,
    0x07a6b7c2, 0xf5491ea7, 0x07a6b7c2, 0x37e40444, 0xe3856d91, 0x00000002, 0x02bf8a3e, 0xfd2f5fa6,
    0x02bf8a3e, 0x38673190, 0xe4ea5a4d, 0x00000002, 0x007e1bd5, 0x000e76ca, 0x007e1bd5, 0x38da5414,
    0xe61afd77, 0x00000002, 0x00000000, 0x2038247b, 0x2038247b, 0x07212644, 0x00000000, 0x00000004
};

//IIR_COEF_352_TO_48
static const uint32_t iir_coefficient_22_to_3[MEMASRC_IIR_COEF_SIZE] = {
    0x02f3c075, 0xfaa0f2bb, 0x02f3c075, 0x3b1e2ce5, 0xe0266497, 0x00000002, 0x061654a7, 0xf4f6e06e,
    0x061654a7, 0x1d7a0223, 0xf03eea63, 0x00000003, 0x0525c445, 0xf6c06638, 0x0525c445, 0x1d6cc79b,
    0xf07b5ae0, 0x00000003, 0x077bc6f3, 0xf2d1482b, 0x077bc6f3, 0x3ac6a73a, 0xe1a7aca5, 0x00000002,
    0x0861aac3, 0xf1e8c6c3, 0x0861aac3, 0x3ab6aa60, 0xe29d3957, 0x00000002, 0x02f20c88, 0xfbb246f6,
    0x02f20c88, 0x3aa813c9, 0xe3c18c32, 0x00000002, 0x0072d6df, 0xffba3768, 0x0072d6df, 0x3a9ca779,
    0xe4c37362, 0x00000002, 0x00000000, 0x2ffa2764, 0x2ffa2764, 0x0ea60a6b, 0x00000000, 0x00000003
};

//IIR_COEF_384_TO_176
static const uint32_t iir_coefficient_24_to_11[MEMASRC_IIR_COEF_SIZE] = {
    0x04c0f379, 0xfedb44d2, 0x04c0f379, 0x298f4134, 0xc1174e69, 0x00000001, 0x0b2dc719, 0xfde5dcb2,
    0x0b2dc719, 0x15c4fe25, 0xe1b82c15, 0x00000002, 0x097dabd2, 0xff571c5d, 0x097dabd2, 0x182d7cb1,
    0xe32d9b4e, 0x00000002, 0x072993e5, 0x01099884, 0x072993e5, 0x1c2c2780, 0xe51a57e5, 0x00000002,
    0x090f2fa1, 0x048cdc85, 0x090f2fa1, 0x21c3b5c2, 0xe7910e77, 0x00000002, 0x04619e1a, 0x0491e3db,
    0x04619e1a, 0x28513621, 0xea59a9d0, 0x00000002, 0x016b9a38, 0x0261dc20, 0x016b9a38, 0x2e08c9d1,
    0xecbe259f, 0x00000002, 0x00000000, 0x27d8fdc3, 0x27d8fdc3, 0x03060265, 0x00000000, 0x00000005
};

//IIR_COEF_352_TO_24
static const uint32_t iir_coefficient_44_to_3[MEMASRC_IIR_COEF_SIZE] = {
    0x035e047f, 0xf96b61f8, 0x035e047f, 0x3ea9e77e, 0xe0103d3f, 0x00000002, 0x068ecd54, 0xf331816d,
    0x068ecd54, 0x1f468083, 0xf01b4754, 0x00000003, 0x05b1d20a, 0xf4e638ca, 0x05b1d20a, 0x1f347549,
    0xf037d0fd, 0x00000003, 0x0899975f, 0xef4dc655, 0x0899975f, 0x3e339e50, 0xe0ca7788, 0x00000002,
    0x0506416b, 0xf657152f, 0x0506416b, 0x1ef29626, 0xf0a9d1d5, 0x00000003, 0x0397fd6f, 0xf94a745b,
    0x0397fd6f, 0x3d80a443, 0xe204ec85, 0x00000002, 0x007807fd, 0xff417ee3, 0x007807fd, 0x3d21cefe,
    0xe2aca225, 0x00000002, 0x00000000, 0x1902d021, 0x1902d021, 0x0f3e4f5a, 0x00000000, 0x00000003
};

//IIR_COEF_384_TO_88
static const uint32_t iir_coefficient_48_to_11[MEMASRC_IIR_COEF_SIZE] = {
    0x02a62553, 0xfc064b8f, 0x02a62553, 0x334dac81, 0xe046a1ac, 0x00000002, 0x0b7129c3, 0xef0c6c3b,
    0x0b7129c3, 0x333f4ca6, 0xe0e333d7, 0x00000002, 0x0988a73e, 0xf251f92c, 0x0988a73e, 0x338970d0,
    0xe1afffe0, 0x00000002, 0x06d3ff07, 0xf6dd0d45, 0x06d3ff07, 0x342bbad5, 0xe2ca2e85, 0x00000002,
    0x07a6625e, 0xf756da5d, 0x07a6625e, 0x3520a02f, 0xe43bc0b9, 0x00000002, 0x02dcefe5, 0xfe24470b,
    0x02dcefe5, 0x364422e9, 0xe5ddb642, 0x00000002, 0x0095d0b1, 0x00542b8f, 0x0095d0b1, 0x374028ca,
    0xe7400a46, 0x00000002, 0x00000000, 0x27338238, 0x27338238, 0x06f4c3b3, 0x00000000, 0x00000004
};

//IIR_COEF_384_TO_44
static const uint32_t iir_coefficient_96_to_11[MEMASRC_IIR_COEF_SIZE] = {
    0x03039659, 0xfa5c23d4, 0x03039659, 0x3c785635, 0xe01fb163, 0x00000002, 0x0628eac7, 0xf4805b4c,
    0x0628eac7, 0x1e27797e, 0xf03424d0, 0x00000003, 0x053a5bfc, 0xf64cf807, 0x053a5bfc, 0x1e15b5a9,
    0xf066ea58, 0x00000003, 0x07a260b2, 0xf206beb1, 0x07a260b2, 0x3c048d2d, 0xe16472aa, 0x00000002,
    0x0892f686, 0xf0ccdf29, 0x0892f686, 0x3bd501d2, 0xe23831f8, 0x00000002, 0x02fd4044, 0xfb2f4ecd,
    0x02fd4044, 0x3b9ec25d, 0xe3376e4d, 0x00000002, 0x006d4eef, 0xff9b0b4d, 0x006d4eef, 0x3b6f651a,
    0xe41af1bb, 0x00000002, 0x00000000, 0x28bbbe8a, 0x28bbbe8a, 0x0ed6fd4e, 0x00000000, 0x00000003
};

static const afe_asrc_iir_coef_t iir_coef_maping_table[] = {
    { 2, 1, iir_coefficient_2_to_1},
    { 3, 1, iir_coefficient_3_to_1},
    { 3, 2, iir_coefficient_3_to_2},
    { 4, 1, iir_coefficient_4_to_1},
    { 4, 3, iir_coefficient_4_to_3},
    { 6, 1, iir_coefficient_6_to_1},
    { 8, 1, iir_coefficient_8_to_1},
    { 8, 3, iir_coefficient_8_to_3},
    {11, 1, iir_coefficient_11_to_1},
    {11, 2, iir_coefficient_11_to_2},

    {11, 3, iir_coefficient_11_to_3},
    {11, 4, iir_coefficient_11_to_4},
    {11, 6, iir_coefficient_11_to_6},
    {11, 8, iir_coefficient_11_to_8},
    {12, 1, iir_coefficient_12_to_1},
    {12,11, iir_coefficient_12_to_11},
    {16, 1, iir_coefficient_16_to_1},
    {16, 3, iir_coefficient_16_to_3},
    {22, 3, iir_coefficient_22_to_3},
    {24,11, iir_coefficient_24_to_11},

    {44, 3, iir_coefficient_44_to_3},
    {48,11, iir_coefficient_48_to_11},
    {96,11, iir_coefficient_96_to_11},
};

static const uint32_t *afe_get_asrc_iir_coef(uint32_t input_rate, uint32_t output_rate, uint32_t *count)
{
    UNUSED(input_rate);
    UNUSED(output_rate);
    UNUSED(count);

    uint32_t in_ratio, out_ratio, gct, i;
    const uint32_t *coef_ptr = NULL;

    if (input_rate == 44100) {
        input_rate = 44000;
    }
    if (output_rate == 44100) {
        output_rate = 44000;
    }
    if (input_rate == 88200) {
        input_rate = 88000;
    }
    if (output_rate == 88200) {
        output_rate = 88000;
    }
    gct = audio_get_gcd(input_rate, output_rate);
    in_ratio = input_rate/gct;
    out_ratio = output_rate/gct;
    if (in_ratio > out_ratio) {
        for (i = 0 ; i< ARRAY_SIZE(iir_coef_maping_table) ; i++) {
            if ((iir_coef_maping_table[i].in_ratio == in_ratio) && (iir_coef_maping_table[i].out_ratio == out_ratio)) {
                coef_ptr = iir_coef_maping_table[i].coef;
                break;
            }
        }
        if (coef_ptr != NULL) {
            *count = MEMASRC_IIR_COEF_SIZE;
            log_hal_msgid_info("DSP asrc load iir in:%d, out:%d, coef:0x%x\r\n",3, in_ratio, out_ratio, coef_ptr);
        } else {
            log_hal_msgid_warning("DSP warning asrc unsupported ratio in:%d, out:%d !!!!!!!!!!\r\n",2, in_ratio, out_ratio);
        }
    }

    return coef_ptr;
    return 0;
}

void afe_set_asrc_iir(afe_mem_asrc_id_t asrc_id, uint32_t input_rate, uint32_t output_rate)
{
    UNUSED(asrc_id);
    UNUSED(input_rate);
    UNUSED(output_rate);

    const uint32_t *coef_ptr;
    uint32_t coef_count = 0, addr_offset;
    addr_offset = asrc_id * 0x100;
    coef_ptr = afe_get_asrc_iir_coef(input_rate, output_rate, &coef_count);
    if (coef_ptr) {
        uint32_t i;

        AFE_SET_REG(ASM_CH01_CNFG + addr_offset, ((coef_count/6 -1 )<<ASM_CH01_CNFG_IIR_STAGE_POS), ASM_CH01_CNFG_IIR_STAGE_MASK);    /* set IIR_stage-1 */

        /* turn on IIR coef setting path */
        AFE_SET_REG(ASM_GEN_CONF + addr_offset, 1<<ASM_GEN_CONF_DSP_CTRL_COEFF_SRAM_POS, ASM_GEN_CONF_DSP_CTRL_COEFF_SRAM_MASK);

        /* Load Coef */
        AFE_SET_REG(ASM_IIR_CRAM_ADDR + addr_offset, 0<<ASM_IIR_CRAM_ADDR_POS, ASM_IIR_CRAM_ADDR_MASK);
        for (i = 0; i < coef_count; i++) {
            //auto increase by 1 when read/write ASM_IIR_CRAM_DATA
            AFE_WRITE(ASM_IIR_CRAM_DATA + addr_offset, coef_ptr[i]<<ASM_IIR_CRAM_DATA_POS);
           // hal_gpt_delay_us(1);
        }
        #if 0
        /* Read reg to verify */
        AFE_SET_REG(ASM_IIR_CRAM_ADDR + addr_offset, 0<<ASM_IIR_CRAM_ADDR_POS, ASM_IIR_CRAM_ADDR_MASK);
        AFE_GET_REG(ASM_IIR_CRAM_DATA+ addr_offset);
        for (i = 0; i < coef_count; i++) {
            uint32_t read_value = AFE_GET_REG(ASM_IIR_CRAM_DATA+ addr_offset);
            log_hal_msgid_info("DSP asrc iir coef %d:0x%x reg %d:0x%x\r\n" 4, i, coef_ptr[i], ((AFE_GET_REG(ASM_IIR_CRAM_ADDR+ addr_offset))>>ASM_IIR_CRAM_ADDR_POS), read_value);
        }
        #endif

        AFE_SET_REG(ASM_IIR_CRAM_ADDR + addr_offset, 0<<ASM_IIR_CRAM_ADDR_POS, ASM_IIR_CRAM_ADDR_MASK);
        /* turn off IIR coe setting path */
        AFE_SET_REG(ASM_GEN_CONF + addr_offset, 0<<ASM_GEN_CONF_DSP_CTRL_COEFF_SRAM_POS, ASM_GEN_CONF_DSP_CTRL_COEFF_SRAM_MASK);

        AFE_SET_REG(ASM_CH01_CNFG + addr_offset, (1<<ASM_CH01_CNFG_IIR_EN_POS), ASM_CH01_CNFG_IIR_EN_MASK);

        /*
        AFE_SET_REG(ASM_GEN_CONF + addr_offset,
                    (1<<ASM_GEN_CONF_CH_CLEAR_POS) | (1<<ASM_GEN_CONF_CH_EN_POS),
                    ASM_GEN_CONF_CH_CLEAR_MASK | ASM_GEN_CONF_CH_EN_MASK);
        */
    } else {
        AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 0<<ASM_CH01_CNFG_IIR_EN_POS, ASM_CH01_CNFG_IIR_EN_MASK);
    }
    /*
    log_hal_msgid_info("DSP reg 0x%x:0x%x\r\n", 2, AUDIO_TOP_CON0, AFE_GET_REG(AUDIO_TOP_CON0));
    log_hal_msgid_info("DSP reg 0x%x:0x%x\r\n", 2, AUDIO_TOP_CON1, AFE_GET_REG(AUDIO_TOP_CON1));
    log_hal_msgid_info("DSP reg 0x%x:0x%x\r\n", 2, ASM_GEN_CONF + addr_offset, AFE_GET_REG(ASM_GEN_CONF+ addr_offset));
    log_hal_msgid_info("DSP reg 0x%x:0x%x\r\n", 2, ASM_CH01_CNFG + addr_offset, AFE_GET_REG(ASM_CH01_CNFG+ addr_offset));
    */
}

void afe_set_asrc_irq_enable(afe_mem_asrc_id_t asrc_id, bool enable)
{

    uint32_t addr = ASM_IER + asrc_id * 0x100;
    uint32_t val = (enable)
                     ? (1<<ASM_IER_IBUF_EMPTY_INTEN_POS) | (0<<ASM_IER_IBUF_AMOUNT_INTEN_POS) | (0<<ASM_IER_OBUF_OV_INTEN_POS) | (0<<ASM_IER_OBUF_AMOUNT_INTEN_POS)
                     : (0<<ASM_IER_IBUF_EMPTY_INTEN_POS) | (0<<ASM_IER_IBUF_AMOUNT_INTEN_POS) | (0<<ASM_IER_OBUF_OV_INTEN_POS) | (0<<ASM_IER_OBUF_AMOUNT_INTEN_POS);
    uint32_t mask;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    AFE_WRITE(addr, val);
    hal_nvic_restore_interrupt_mask(mask);
    return;

}

bool afe_get_asrc_irq_is_enabled(afe_mem_asrc_id_t asrc_id, uint32_t interrupt)
{
    uint32_t addr = ASM_IER + asrc_id * 0x100;
    return (AFE_READ(addr) & interrupt) ? 1 : 0;
}

void afe_clear_asrc_irq(afe_mem_asrc_id_t asrc_id, uint32_t status)
{
    UNUSED(asrc_id);
    UNUSED(status);

#if 0//modify for ab1568
    uint32_t addr = ASM_IFR + asrc_id * 0x100;

    AFE_WRITE(addr, status);
#endif
}

uint32_t afe_get_asrc_irq_status(afe_mem_asrc_id_t asrc_id)
{
    UNUSED(asrc_id);

#if 0
    uint32_t addr = ASM_IFR + asrc_id * 0x100;

    return AFE_READ(addr);
#endif
return 0;
}

void afe_set_asrc(afe_mem_asrc_id_t asrc_id, afe_asrc_config_p config, bool enable)
{
    UNUSED(asrc_id);
    UNUSED(config);
    UNUSED(enable);

    uint32_t  addr_offset;
    uint32_t pos;
    addr_offset = asrc_id * 0x100;

    if (enable) {
    #if 0
        /*We have check with ASRC HW designer. This hardware busy bit is used for monitoring. Not default to used by SW.*/
        if (AFE_GET_REG(ASM_GEN_CONF + addr_offset)&ASM_GEN_CONF_ASRC_BUSY_MASK) {
            log_hal_msgid_warning("%s() error: asrc[%d] is running\r\n", 2, __FUNCTION__, asrc_id);
            return;
        }
    #endif

        //KH test
        AFE_SET_REG(ASM_GEN_CONF + addr_offset, 1<<ASM_GEN_CONF_CH_CLEAR_POS, ASM_GEN_CONF_CH_CLEAR_MASK);

        //PT test!!!!!
        AFE_SET_REG(ASM_GEN_CONF + addr_offset, (config->hw_update_obuf_rdpnt)<<ASM_GEN_CONF_HW_UPDATE_OBUF_RDPNT_POS, ASM_GEN_CONF_HW_UPDATE_OBUF_RDPNT_MASK);


        /* when there is only 1 block data left in the input buffer, issue interrupt */
        /* times of 512bit. */
        AFE_SET_REG(ASM_IBUF_INTR_CNT0 + addr_offset, 32<<ASM_IBUF_INTR_CNT0_POS, ASM_IBUF_INTR_CNT0_MASK);
        /* when there is only 1 block space in the output buffer, issue interrupt */
        /* times of 512bit. 0xFF means if more than 16kB, send interrupt */
        AFE_SET_REG(ASM_OBUF_INTR_CNT0 + addr_offset, 32<<ASM_OBUF_INTR_CNT0_POS, ASM_OBUF_INTR_CNT0_MASK);

        /* clear all interrupt flag */
        afe_clear_asrc_irq(asrc_id, ASM_IFR_IBUF_EMPTY_INT_MASK | ASM_IFR_IBUF_AMOUNT_INT_MASK | ASM_IFR_OBUF_OV_INT_MASK | ASM_IFR_OBUF_AMOUNT_INT_MASK);

        /* enable interrupt */
        #if (AFE_REGISTER_ASRC_IRQ)
        afe_set_asrc_irq_enable(asrc_id, true);
        #else
        afe_set_asrc_irq_enable(asrc_id, false);
        #endif

        /* set input buffer's base and size */
        AFE_SET_REG(ASM_IBUF_SADR + addr_offset, config->input_buffer.addr<<ASM_IBUF_SADR_POS, ASM_IBUF_SADR_MASK);
        AFE_SET_REG(ASM_IBUF_SIZE + addr_offset, config->input_buffer.size<<ASM_IBUF_SIZE_POS, ASM_IBUF_SIZE_MASK);
        /* set input buffer's rp and wp */
        if (config->ul_mode) {
            AFE_SET_REG(ASM_CH01_IBUF_WRPNT + addr_offset, (config->input_buffer.addr)<<ASM_CH01_IBUF_RDPNT_POS, ASM_CH01_IBUF_RDPNT_MASK);
            AFE_SET_REG(ASM_CH01_IBUF_RDPNT + addr_offset, (config->input_buffer.addr + config->input_buffer.size - config->input_buffer.offset)<<ASM_CH01_IBUF_WRPNT_POS, ASM_CH01_IBUF_WRPNT_MASK);
        } else {
            AFE_SET_REG(ASM_CH01_IBUF_WRPNT + addr_offset, (config->input_buffer.addr + config->input_buffer.offset)<<ASM_CH01_IBUF_RDPNT_POS, ASM_CH01_IBUF_RDPNT_MASK);
            AFE_SET_REG(ASM_CH01_IBUF_RDPNT + addr_offset, (config->input_buffer.addr)<<ASM_CH01_IBUF_WRPNT_POS, ASM_CH01_IBUF_WRPNT_MASK);
        }

        /* set output buffer's base and size */
        AFE_SET_REG(ASM_OBUF_SADR + addr_offset, config->output_buffer.addr<<ASM_OBUF_SADR_POS, ASM_OBUF_SADR_MASK);
        AFE_SET_REG(ASM_OBUF_SIZE + addr_offset, config->output_buffer.size<<ASM_OBUF_SIZE_POS, ASM_OBUF_SIZE_MASK);
        /* set output buffer's rp and wp */
        if (config->ul_mode) {
            AFE_SET_REG(ASM_CH01_OBUF_RDPNT + addr_offset, (config->output_buffer.addr + config->output_buffer.offset)<<ASM_CH01_OBUF_RDPNT_POS, ASM_CH01_OBUF_RDPNT_MASK);
            AFE_SET_REG(ASM_CH01_OBUF_WRPNT + addr_offset, (config->output_buffer.addr)<<ASM_CH01_OBUF_WRPNT_POS, ASM_CH01_OBUF_WRPNT_MASK);
        } else {
            AFE_SET_REG(ASM_CH01_OBUF_RDPNT + addr_offset, (config->output_buffer.addr)<<ASM_CH01_OBUF_RDPNT_POS, ASM_CH01_OBUF_RDPNT_MASK);
            AFE_SET_REG(ASM_CH01_OBUF_WRPNT + addr_offset, (config->output_buffer.addr + config->output_buffer.offset)<<ASM_CH01_OBUF_WRPNT_POS, ASM_CH01_OBUF_WRPNT_MASK);
        }

        /* set Bit-width Selection*/
        AFE_SET_REG(ASM_CH01_CNFG + addr_offset, ((config->input_buffer.format<=AFE_PCM_FORMAT_U16_BE)?true:false)<<ASM_CH01_CNFG_IBIT_WIDTH_POS, ASM_CH01_CNFG_IBIT_WIDTH_MASK);
        AFE_SET_REG(ASM_CH01_CNFG + addr_offset, ((config->output_buffer.format<=AFE_PCM_FORMAT_U16_BE)?true:false)<<ASM_CH01_CNFG_OBIT_WIDTH_POS, ASM_CH01_CNFG_OBIT_WIDTH_MASK);

        /* set channel number*/
        AFE_SET_REG(ASM_CH01_CNFG + addr_offset, ((config->stereo)?false:true)<<ASM_CH01_CNFG_MONO_POS, ASM_CH01_CNFG_MONO_MASK);


        AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 0x8<<ASM_CH01_CNFG_CLAC_AMOUNT_POS, ASM_CH01_CNFG_CLAC_AMOUNT_MASK);
        AFE_SET_REG(ASM_MAX_OUT_PER_IN0 + addr_offset, 0<<ASM_MAX_OUT_PER_IN0_POS, ASM_MAX_OUT_PER_IN0_MASK);

        if (config->tracking_mode != MEM_ASRC_NO_TRACKING) {
            /* check freq cali status */
            if (AFE_READ(ASM_FREQ_CALI_CTRL + addr_offset)&ASM_FREQ_CALI_CTRL_FREQ_CALC_BUSY_MASK)
                log_hal_msgid_warning("warning: freq calibration is busy\r\n", 0);
            if (AFE_READ(ASM_FREQ_CALI_CTRL + addr_offset)&ASM_FREQ_CALI_CTRL_CALI_EN_POS)
                log_hal_msgid_warning("warning: freq calibration has been already enabled\r\n", 0);
            /* freq_mode = (denominator/period_mode)*0x800000 */
            #if 0
            AFE_WRITE(ASM_FREQ_CALI_CYC + addr_offset, 0x3F00);
            AFE_WRITE(ASM_CALI_DENOMINATOR + addr_offset, 0x3C00);//
            AFE_WRITE(ASM_FREQ_CALI_CTRL + addr_offset, 0x18500);
            #else
            AFE_WRITE(ASM_CALI_DENOMINATOR + addr_offset, 0x1FBD);
            #endif

            log_hal_msgid_info("asrc tracking_mode=%d, tracking_clock=%d\r\n",2, config->tracking_mode, config->tracking_clock);
            switch (config->tracking_clock) {
                case MEM_ASRC_TRACKING_I2S1:
                case MEM_ASRC_TRACKING_I2S2:
                case MEM_ASRC_TRACKING_I2S3:
                case MEM_ASRC_TRACKING_I2S4:
                    pos = (MEM_ASRC_TRAC_CON1_CALC_LRCK_SEL_POS + 3 * asrc_id);
                    //AFE_SET_REG(MEM_ASRC_TRAC_CON1,    (config->tracking_clock - MEM_ASRC_TRACKING_I2S1 + 1)<<pos, MEM_ASRC_TRAC_CON1_CALC_LRCK_SEL_MASK<<pos);
                    AFE_SET_REG(ASM_FREQ_CALI_CTRL + addr_offset, (config->tracking_clock) << ASM_FREQ_CALI_CTRL_SRC_SEL_POS, ASM_FREQ_CALI_CTRL_SRC_SEL_MASK);
                    break;
                case MEM_ASRC_TRACKING_SPDIFIN:
                    //AFE_SET_REG(AFE_SPDIFIN_CFG1, 1<<AFE_SPDIFIN_CFG1_SEL_SPDIFIN_CLK_EN_POS, AFE_SPDIFIN_CFG1_SEL_SPDIFIN_CLK_EN_MASK);
                    AFE_SET_REG(ASM_FREQ_CALI_CTRL + addr_offset, 0x0 << ASM_FREQ_CALI_CTRL_SRC_SEL_POS, ASM_FREQ_CALI_CTRL_SRC_SEL_MASK);
                    break;
                default:
                    break;
            }

            if (config->tracking_mode == MEM_ASRC_TRACKING_MODE_RX) {
                AFE_WRITE(ASM_FREQ_CALI_CYC + addr_offset, 0x3F00);
                AFE_SET_REG(ASM_FREQ_CALI_CTRL + addr_offset, 1<<ASM_FREQ_CALI_CTRL_FREQ_UPDATE_FS2_POS, ASM_FREQ_CALI_CTRL_FREQ_UPDATE_FS2_MASK); /* Rx->FreqMode Bit9=1 */
                AFE_SET_REG(ASM_FREQUENCY_0 + addr_offset, afe_asrc_get_samplingrate_to_palette(config->output_buffer.rate),  0xFFFFFF);/*FrequencyPalette(output_fs)*/
                AFE_SET_REG(ASM_FREQUENCY_2 + addr_offset, afe_asrc_get_samplingrate_to_palette(config->input_buffer.rate),  0xFFFFFF);/*FrequencyPalette(input_fs)*/
            } else {
                AFE_WRITE(ASM_FREQ_CALI_CYC + addr_offset, (config->input_buffer.rate == 44100 || config->output_buffer.rate == 44100) ? 0x1B800 : 0x5F00);
                AFE_SET_REG(ASM_FREQUENCY_0 + addr_offset, afe_asrc_get_period_palette(config->input_buffer.rate, 0), 0xFFFFFF);
                AFE_SET_REG(ASM_FREQUENCY_2 + addr_offset, afe_asrc_get_period_palette(config->output_buffer.rate, 0),0xFFFFFF);
            }
            AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 2<<ASM_CH01_CNFG_IFS_POS, ASM_CH01_CNFG_IFS_MASK);
            AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 0<<ASM_CH01_CNFG_OFS_POS, ASM_CH01_CNFG_OFS_MASK);
            /* important: firstly POS_CALI_EN, then POS_AUTO_FS2_UPDATE */
            AFE_SET_REG(ASM_FREQ_CALI_CTRL + addr_offset, 0x1<<ASM_FREQ_CALI_CTRL_AUTO_FS2_UPDATE_POS | 1<<ASM_FREQ_CALI_CTRL_AUTO_RESTART_POS,  ASM_FREQ_CALI_CTRL_AUTO_FS2_UPDATE_MASK|ASM_FREQ_CALI_CTRL_AUTO_RESTART_MASK);
            AFE_SET_REG(ASM_FREQ_CALI_CTRL + addr_offset, 0x1<<ASM_FREQ_CALI_CTRL_CALI_EN_POS, ASM_FREQ_CALI_CTRL_CALI_EN_MASK);
        } else {
            AFE_SET_REG(ASM_FREQUENCY_0 + addr_offset, afe_asrc_get_samplingrate_to_palette(config->input_buffer.rate),  0xFFFFFF);
            AFE_SET_REG(ASM_FREQUENCY_1 + addr_offset, afe_asrc_get_samplingrate_to_palette(config->output_buffer.rate), 0xFFFFFF);
            AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 0<<ASM_CH01_CNFG_IFS_POS, ASM_CH01_CNFG_IFS_MASK);
            AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 1<<ASM_CH01_CNFG_OFS_POS, ASM_CH01_CNFG_OFS_MASK);
        }

    } else {
        /* disable interrupt */
        afe_set_asrc_irq_enable(asrc_id, false);
    }
}

#ifdef ENABLE_HWSRC_CLKSKEW
void afe_set_asrc_compensating_sample(afe_mem_asrc_id_t asrc_id, uint32_t output_buffer_rate, afe_asrc_compensating_t cp_point)
{
    uint32_t  addr_offset;
    addr_offset = asrc_id * 0x100;
    float general_xppm_table[AFE_HWSRC_STEP_NUM] = {
        (4096-4),(4096-3.75),(4096-3.5),(4096-3.25),
        (4096-3),(4096-2.75),(4096-2.5),(4096-2.25),
        (4096-2),(4096-1.75),(4096-1.5),(4096-1.25),
        (4096-1),(4096-0.75),(4096-0.5),(4096-0.25),
        (4096),
        (4096+0.25),(4096+0.5),(4096+0.75),(4096+1),
        (4096+1.25),(4096+1.5),(4096+1.75),(4096+2),
        (4096+2.25),(4096+2.5),(4096+2.75),(4096+3),
        (4096+3.25),(4096+3.5),(4096+2.75),(4096+4),
        };

    if(cp_point <= COMPENSATING_ADD_976_XPPM){
        cp_point = (output_buffer_rate *general_xppm_table[cp_point]) / 100;
    }else{
        log_hal_msgid_error("asrc set wrong compensating sample %d",1,cp_point);
    }

    AFE_SET_REG(ASM_FREQUENCY_1 + addr_offset, cp_point, 0xFFFFFF);
}
#endif

int32_t afe_mem_asrc_enable(afe_mem_asrc_id_t asrc_id, bool enable)
{
    UNUSED(asrc_id);
    UNUSED(enable);
    uint32_t addr;

    if (asrc_id >= MEM_ASRC_NUM) {
        log_hal_msgid_error("DSP afe_mem_asrc_enable() error: invalid id %u\n", 1, asrc_id);
        return -1;
    }
    addr = ASM_GEN_CONF + asrc_id * 0x100;
    if (enable) {
        AFE_SET_REG(addr,
                    (1<<ASM_GEN_CONF_CH_CLEAR_POS) | (1<<ASM_GEN_CONF_CH_EN_POS) | (1<<ASM_GEN_CONF_ASRC_EN_POS),
                    ASM_GEN_CONF_CH_CLEAR_MASK | ASM_GEN_CONF_CH_EN_MASK | ASM_GEN_CONF_ASRC_EN_MASK);

    } else {
        AFE_SET_REG(addr,
                    (0<<ASM_GEN_CONF_ASRC_EN_POS) | (0<<ASM_GEN_CONF_CH_EN_POS),
                    ASM_GEN_CONF_ASRC_EN_MASK | ASM_GEN_CONF_CH_EN_MASK);

        addr = ASM_FREQ_CALI_CTRL + asrc_id * 0x100;
        AFE_SET_REG(addr,
                    (0<<ASM_FREQ_CALI_CTRL_CALI_EN_POS)|(0<<ASM_FREQ_CALI_CTRL_AUTO_FS2_UPDATE_POS),
                    ASM_FREQ_CALI_CTRL_CALI_EN_MASK | ASM_FREQ_CALI_CTRL_AUTO_FS2_UPDATE_MASK);

    }
    return 0;

    return 0;
}


#endif /*HAL_AUDIO_MODULE_ENABLED*/
