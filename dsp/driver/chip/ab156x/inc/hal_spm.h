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
 
#ifndef __HAL_SPM_H__
#define __HAL_SPM_H__

#include "hal_platform.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "stdio.h"
#include "hal_uart.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include <xtensa/hal.h>
#include <xtensa/xtruntime.h>
#include <xtensa/hal.h>

#define SPM_BASE    (0xA2110000)

#define SPM_CM4_WAKEUP_SOURCE_MASK           ((volatile uint32_t*)(SPM_BASE + 0x0000))
#define SPM_AUDIO_I2S_WAKEUP_SOURCE_MASK     ((volatile uint32_t*)(SPM_BASE + 0x0004))
#define SPM_DSP_WAKEUP_SOURCE_MASK           ((volatile uint32_t*)(SPM_BASE + 0x000C))
#define SPM_SFC_EMI_READY                    ((volatile uint32_t*)(SPM_BASE + 0x01D0))
#define SPM_SKIP_SFC_EMI_TASK                ((volatile uint32_t*)(SPM_BASE + 0x01D4))
#define SPM_SCENARIO_SELECT                  ((volatile uint32_t*)(SPM_BASE + 0x01E0))
#define SPM_INFRA_SYSRAM0_CONTROL_0          ((volatile uint32_t*)(SPM_BASE + 0x0278))
#define SPM_INFRA_SYSRAM1_CONTROL_0          ((volatile uint32_t*)(SPM_BASE + 0x0288))
#define SPM_PCM_CON0                         ((volatile uint32_t*)(SPM_BASE + 0x0318))
#define SPM_PCM_IM_HOST_RW_PTR               ((volatile uint32_t*)(SPM_BASE + 0x0330))
#define SPM_PCM_EVENT_VECTOR0                ((volatile uint32_t*)(SPM_BASE + 0x0340))
#define SPM_PCM_EVENT_VECTOR1                ((volatile uint32_t*)(SPM_BASE + 0x0344))
#define SPM_PCM_EVENT_VECTOR2                ((volatile uint32_t*)(SPM_BASE + 0x0348))
#define SPM_PCM_EVENT_VECTOR3                ((volatile uint32_t*)(SPM_BASE + 0x034C))
#define SPM_PCM_EVENT_VECTOR4                ((volatile uint32_t*)(SPM_BASE + 0x0350))
#define SPM_PCM_EVENT_VECTOR5                ((volatile uint32_t*)(SPM_BASE + 0x0354))
#define SPM_PCM_EVENT_VECTOR6                ((volatile uint32_t*)(SPM_BASE + 0x0358))
#define SPM_PCM_EVENT_VECTOR7                ((volatile uint32_t*)(SPM_BASE + 0x035C))
#define SPM_DSP_WAKEUP_SOURCE_STA            ((volatile uint32_t*)(SPM_BASE + 0x07F4))
#define SPM_WAKEUP_SOURCE_STA                ((volatile uint32_t*)(SPM_BASE + 0x0800))
#define SPM_PWR_STATUS                       ((volatile uint32_t*)(SPM_BASE + 0x0808))
#define SPM_SLEEP_ISR_STATUS                 ((volatile uint32_t*)(SPM_BASE + 0x0820))
#define SPM_SLEEP_ISR_RAW_STA                ((volatile uint32_t*)(SPM_BASE + 0x0830))
#define SPM_PCM_SW_INT_SET                   ((volatile uint32_t*)(SPM_BASE + 0x0B90))
#define SPM_PCM_SW_INT_CLEAR                 ((volatile uint32_t*)(SPM_BASE + 0x0B94))
#define SPM_SKIP_SFC_EMI_TASK_DSP            ((volatile uint32_t*)(SPM_BASE + 0x1014))
#define SPM_SLEEP_ISR_STATUS_DSP             ((volatile uint32_t*)(SPM_BASE + 0x1024))
#define SPM_PCM_SW_INT_CLEAR_DSP             ((volatile uint32_t*)(SPM_BASE + 0x1030))

#define HAL_SPM_DEEBUG_ENABLE
#ifdef  HAL_SPM_DEEBUG_ENABLE
#define log_hal_debug(_message,...)     printf(_message, ##__VA_ARGS__)
#define SPM_SET_DSP0_SW_STATE(x)        *((volatile uint32_t*)(SPM_BASE + 0x0810)) = (x + 0x90)
#define SPM_SET_DSP1_SW_STATE(x)        *((volatile uint32_t*)(SPM_BASE + 0x0810)) = (x + 0xA0)
#else
#define log_hal_debug(_message,...)
#define SPM_SET_DSP0_SW_STATE(x)   
#define SPM_SET_DSP1_SW_STATE(x)
#endif

#define SPM_SET_LOCK_INFRA              *((volatile uint32_t*)(SPM_BASE + 0x0C00)) = *((volatile uint32_t*)(SPM_BASE + 0x0C00)) | 0x1
#define SPM_CLEAR_LOCK_INFRA            *((volatile uint32_t*)(SPM_BASE + 0x0C00)) = *((volatile uint32_t*)(SPM_BASE + 0x0C00)) & 0xFFFFFFFE       
#define SPM_INFRA_OFF_FLAG              *((volatile uint32_t*)(SPM_BASE + 0x0CB4))

int spm_init(void);
void spm_mask_wakeup_source(uint32_t wakeup_source);
void spm_unmask_wakeup_source(uint32_t wakeup_source);

#define SPM_FPGA_DVT 0
#if SPM_FPGA_DVT
void spm_dvt_dsp_basic_configuration();
#endif
#endif
#endif
