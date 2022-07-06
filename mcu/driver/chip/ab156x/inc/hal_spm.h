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
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_platform.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include "hal.h"

#define SPM_BASE    (0xA2110000)


#define SPM_CM4_WAKEUP_SOURCE_MASK                 ((volatile uint32_t*)(SPM_BASE + 0x0000))
#define SPM_AUDIO_I2S_WAKEUP_SOURCE_MASK           ((volatile uint32_t*)(SPM_BASE + 0x0004))
#define SPM_CONN_WAKEUP_SOURCE_MASK                ((volatile uint32_t*)(SPM_BASE + 0x0008))
#define SPM_DSP_WAKEUP_SOURCE_MASK                 ((volatile uint32_t*)(SPM_BASE + 0x000C))
#define SPM_POWER_ON_VAL0                          ((volatile uint32_t*)(SPM_BASE + 0x0010))
#define SPM_POWER_ON_VAL1                          ((volatile uint32_t*)(SPM_BASE + 0x0014))
#define SPM_SEQUENCER_32K_REG_0                    ((volatile uint32_t*)(SPM_BASE + 0x0100))
#define SPM_SEQUENCER_32K_REG_1                    ((volatile uint32_t*)(SPM_BASE + 0x0104))
#define SPM_SEQUENCER_32K_REG_2                    ((volatile uint32_t*)(SPM_BASE + 0x0108))
#define SPM_SEQUENCER_32K_REG_3                    ((volatile uint32_t*)(SPM_BASE + 0x010C))
#define SPM_SEQUENCER_32K_REG_4                    ((volatile uint32_t*)(SPM_BASE + 0x0110))
#define SPM_SEQUENCER_32K_REG_5                    ((volatile uint32_t*)(SPM_BASE + 0x0114))
#define SPM_FSM_XO_REG_6                           ((volatile uint32_t*)(SPM_BASE + 0x0118))
#define SPM_SEQUENCER_26M_REG_0                    ((volatile uint32_t*)(SPM_BASE + 0x0140))
#define SPM_SEQUENCER_26M_REG_1                    ((volatile uint32_t*)(SPM_BASE + 0x0144))
#define SPM_IGNORE_CPU_ACTIVE                      ((volatile uint32_t*)(SPM_BASE + 0x0150))
#define SPM_CLK_HANDLE_SELECT                      ((volatile uint32_t*)(SPM_BASE + 0x0170))
#define SPM_CM4_REQUEST_XO_CON                     ((volatile uint32_t*)(SPM_BASE + 0x0174))
#define SPM_DSP_REQUEST_XO_CON                     ((volatile uint32_t*)(SPM_BASE + 0x0178))
#define SPM_REQUEST_XO_MASK                        ((volatile uint32_t*)(SPM_BASE + 0x0180))
#define SPM_REQUEST_XO_GATE                        ((volatile uint32_t*)(SPM_BASE + 0x0184))
#define SPM_SELECT_XO_RDY_SRC                      ((volatile uint32_t*)(SPM_BASE + 0x0188))
#define SPM_XO_SETTLE_TIME                         ((volatile uint32_t*)(SPM_BASE + 0x018C))
#define SPM_XO_RESOURCE_TIMING                     ((volatile uint32_t*)(SPM_BASE + 0x0190))
#define SPM_SPM_EMI_GENA                           ((volatile uint32_t*)(SPM_BASE + 0x01A0))
#define SPM_SPM_EMI_DSRAM                          ((volatile uint32_t*)(SPM_BASE + 0x01A4))
#define SPM_SPM_EMI_ARB_A0_MAX_GNT_CNT             ((volatile uint32_t*)(SPM_BASE + 0x01B0))
#define SPM_SPM_EMI_ARB_B0_MAX_GNT_CNT             ((volatile uint32_t*)(SPM_BASE + 0x01B4))
#define SPM_SPM_EMI_ARB_C0_MAX_GNT_CNT             ((volatile uint32_t*)(SPM_BASE + 0x01B8))
#define SPM_SFC_ID_0                               ((volatile uint32_t*)(SPM_BASE + 0x01C0))
#define SPM_SFC_ID_1                               ((volatile uint32_t*)(SPM_BASE + 0x01C4))
#define SPM_SFC_EMI_READY                          ((volatile uint32_t*)(SPM_BASE + 0x01D0))
#define SPM_SKIP_SFC_EMI_TASK                      ((volatile uint32_t*)(SPM_BASE + 0x01D4))
#define SPM_SFC_EMI_WAIT_TIME_IN_SLEEP             ((volatile uint32_t*)(SPM_BASE + 0x01D8))
#define SPM_SFC_EMI_WAIT_TIME_IN_WAKEUP            ((volatile uint32_t*)(SPM_BASE + 0x01DC))
#define SPM_SCENARIO_SELECT                        ((volatile uint32_t*)(SPM_BASE + 0x01E0))
#define SPM_DSP_ALIGN                              ((volatile uint32_t*)(SPM_BASE + 0x01E4))
#define SPM_CONN_AUDIO_ABB_SIDEBAND_MASK           ((volatile uint32_t*)(SPM_BASE + 0x01E8))
#define SPM_CONN_SRCLKENA_ACK_SW_CON               ((volatile uint32_t*)(SPM_BASE + 0x01EC))
#define SPM_CM4_PWR_CON                            ((volatile uint32_t*)(SPM_BASE + 0x0200))
#define SPM_INFRA_PWR_CON                          ((volatile uint32_t*)(SPM_BASE + 0x0204))
#define SPM_CONN_TOP_ON_PWR_CON                    ((volatile uint32_t*)(SPM_BASE + 0x0208))
#define SPM_CONN_TOP_OFF_PWR_CON                   ((volatile uint32_t*)(SPM_BASE + 0x020C))
#define SPM_AUDIO_PWR_CON                          ((volatile uint32_t*)(SPM_BASE + 0x0210))
#define SPM_DSP_PWR_CON                            ((volatile uint32_t*)(SPM_BASE + 0x0214))
#define SPM_CM4_TCM2K_CONTROL_0                    ((volatile uint32_t*)(SPM_BASE + 0x0220))
#define SPM_CM4_TCM2K_CONTROL_1                    ((volatile uint32_t*)(SPM_BASE + 0x0224))
#define SPM_CM4_TCM2K_CONTROL_2                    ((volatile uint32_t*)(SPM_BASE + 0x0228))
#define SPM_CM4_TCM2K_CONTROL_3                    ((volatile uint32_t*)(SPM_BASE + 0x022C))
#define SPM_CM4_TCM4K_CONTROL_0                    ((volatile uint32_t*)(SPM_BASE + 0x0240))
#define SPM_CM4_TCM4K_CONTROL_1                    ((volatile uint32_t*)(SPM_BASE + 0x0244))
#define SPM_CM4_TCM4K_CONTROL_2                    ((volatile uint32_t*)(SPM_BASE + 0x0248))
#define SPM_CM4_TCM4K_CONTROL_3                    ((volatile uint32_t*)(SPM_BASE + 0x024C))
#define SPM_CM4_TCM8K_CONTROL_0                    ((volatile uint32_t*)(SPM_BASE + 0x0250))
#define SPM_CM4_TCM8K_CONTROL_1                    ((volatile uint32_t*)(SPM_BASE + 0x0254))
#define SPM_CM4_TCM8K_CONTROL_2                    ((volatile uint32_t*)(SPM_BASE + 0x0258))
#define SPM_CM4_TCM8K_CONTROL_3                    ((volatile uint32_t*)(SPM_BASE + 0x025C))
#define SPM_CM4_TCM16K_CONTROL_0                   ((volatile uint32_t*)(SPM_BASE + 0x0260))
#define SPM_CM4_TCM16K_CONTROL_1                   ((volatile uint32_t*)(SPM_BASE + 0x0264))
#define SPM_CM4_TCM16K_CONTROL_2                   ((volatile uint32_t*)(SPM_BASE + 0x0268))
#define SPM_CM4_TAGRAM_CONTROL_0                   ((volatile uint32_t*)(SPM_BASE + 0x026C))
#define SPM_CM4_TAGRAM_CONTROL_1                   ((volatile uint32_t*)(SPM_BASE + 0x0270))
#define SPM_CM4_ROM_CONTROL                        ((volatile uint32_t*)(SPM_BASE + 0x0274))
#define SPM_INFRA_SYSRAM0_CONTROL_0                ((volatile uint32_t*)(SPM_BASE + 0x0278))
#define SPM_INFRA_SYSRAM0_CONTROL_1                ((volatile uint32_t*)(SPM_BASE + 0x027C))
#define SPM_INFRA_SYSRAM0_CONTROL_2                ((volatile uint32_t*)(SPM_BASE + 0x0280))
#define SPM_INFRA_SYSRAM0_CONTROL_3                ((volatile uint32_t*)(SPM_BASE + 0x0284))
#define SPM_INFRA_SYSRAM1_CONTROL_0                ((volatile uint32_t*)(SPM_BASE + 0x0288))
#define SPM_INFRA_SYSRAM1_CONTROL_1                ((volatile uint32_t*)(SPM_BASE + 0x028C))
#define SPM_INFRA_SYSRAM1_CONTROL_2                ((volatile uint32_t*)(SPM_BASE + 0x0290))
#define SPM_INFRA_SYSRAM1_CONTROL_3                ((volatile uint32_t*)(SPM_BASE + 0x0294))
#define SPM_INFRA_SYSRAM2_CONTROL_0                ((volatile uint32_t*)(SPM_BASE + 0x0298))
#define SPM_INFRA_SYSRAM2_CONTROL_1                ((volatile uint32_t*)(SPM_BASE + 0x029C))
#define SPM_INFRA_SYSRAM2_CONTROL_2                ((volatile uint32_t*)(SPM_BASE + 0x0300))
#define SPM_INFRA_SYSRAM2_CONTROL_3                ((volatile uint32_t*)(SPM_BASE + 0x0304))
#define SPM_INFRA_SYSRAM3_CONTROL_0                ((volatile uint32_t*)(SPM_BASE + 0x0308))
#define SPM_INFRA_SYSRAM3_CONTROL_1                ((volatile uint32_t*)(SPM_BASE + 0x030C))
#define SPM_DSP_ROM_CONTROL                        ((volatile uint32_t*)(SPM_BASE + 0x0310))
#define SPM_PCM_CON0                               ((volatile uint32_t*)(SPM_BASE + 0x0318))
#define SPM_PCM_CON1                               ((volatile uint32_t*)(SPM_BASE + 0x031C))
#define SPM_PCM_IM_PTR                             ((volatile uint32_t*)(SPM_BASE + 0x0320))
#define SPM_PCM_IM_LEN                             ((volatile uint32_t*)(SPM_BASE + 0x0324))
#define SPM_PCM_IM_HOST_RW_PTR                     ((volatile uint32_t*)(SPM_BASE + 0x0330))
#define SPM_PCM_IM_HOST_RW_DAT                     ((volatile uint32_t*)(SPM_BASE + 0x0334))
#define SPM_PCM_REG_DATA_INI                       ((volatile uint32_t*)(SPM_BASE + 0x0338))
#define SPM_PCM_EVENT_VECTOR0                      ((volatile uint32_t*)(SPM_BASE + 0x0340))
#define SPM_PCM_EVENT_VECTOR1                      ((volatile uint32_t*)(SPM_BASE + 0x0344))
#define SPM_PCM_EVENT_VECTOR2                      ((volatile uint32_t*)(SPM_BASE + 0x0348))
#define SPM_PCM_EVENT_VECTOR3                      ((volatile uint32_t*)(SPM_BASE + 0x034C))
#define SPM_PCM_EVENT_VECTOR4                      ((volatile uint32_t*)(SPM_BASE + 0x0350))
#define SPM_PCM_EVENT_VECTOR5                      ((volatile uint32_t*)(SPM_BASE + 0x0354))
#define SPM_PCM_EVENT_VECTOR6                      ((volatile uint32_t*)(SPM_BASE + 0x0358))
#define SPM_PCM_EVENT_VECTOR7                      ((volatile uint32_t*)(SPM_BASE + 0x035C))
#define SPM_PCM_PWR_IO_EN                          ((volatile uint32_t*)(SPM_BASE + 0x0360))
#define SPM_PCM_TIMER_VAL                          ((volatile uint32_t*)(SPM_BASE + 0x0364))
#define SPM_PCM_REGC_WAKEUP_MASK                   ((volatile uint32_t*)(SPM_BASE + 0x0368))
#define SPM_SLEEP_WAKEUP_EVENT_MASK                ((volatile uint32_t*)(SPM_BASE + 0x0380))
#define SPM_SW_DEEP_SLEEP                          ((volatile uint32_t*)(SPM_BASE + 0x0384))
#define SPM_SLEEP_ISR_MASK                         ((volatile uint32_t*)(SPM_BASE + 0x0390))
#define SPM_SIDEBAND_CONTROL_0                     ((volatile uint32_t*)(SPM_BASE + 0x03A0))
#define SPM_SIDEBAND_CONTROL_1                     ((volatile uint32_t*)(SPM_BASE + 0x03A4))
#define SPM_SIDEBAND_CONTROL_2                     ((volatile uint32_t*)(SPM_BASE + 0x03A8))
#define SPM_RESOURCE_CONTROL_0                     ((volatile uint32_t*)(SPM_BASE + 0x03AC))
#define SPM_SPM_STATE_CONTROL_0                    ((volatile uint32_t*)(SPM_BASE + 0x03B0))
#define SPM_EVENT_CONTROL_0                        ((volatile uint32_t*)(SPM_BASE + 0x03B4))
#define SPM_EVENT_CONTROL_1                        ((volatile uint32_t*)(SPM_BASE + 0x03B8))
#define SPM_SPM_32K_CLKMUX_CONTROL                 ((volatile uint32_t*)(SPM_BASE + 0x03BC))
#define SPM_CLK_SETTLE                             ((volatile uint32_t*)(SPM_BASE + 0x03C0))
#define SPM_SPM_DEBUG_SELECT                       ((volatile uint32_t*)(SPM_BASE + 0x03D0))
#define SPM_SPM_DEBUG_CON                          ((volatile uint32_t*)(SPM_BASE + 0x03D4))
#define SPM_MONITOR_WAKEUP_EVENT_EN                ((volatile uint32_t*)(SPM_BASE + 0x03D8))
#define SPM_SPM_ERROR_FLAG                         ((volatile uint32_t*)(SPM_BASE + 0x03E0))
#define SPM_EDGE_WAKEUP_SOURCE                     ((volatile uint32_t*)(SPM_BASE + 0x03F0))
#define SPM_SLEEP_CM4_PWR_MASK_B                   ((volatile uint32_t*)(SPM_BASE + 0x03FC))
#define SPM_SLEEP_INFRA_PWR_MASK_B                 ((volatile uint32_t*)(SPM_BASE + 0x0400))
#define SPM_SLEEP_CONN_TOP_ON_PWR_MASK_B           ((volatile uint32_t*)(SPM_BASE + 0x0404))
#define SPM_SLEEP_CONN_TOP_OFF_PWR_MASK_B          ((volatile uint32_t*)(SPM_BASE + 0x0408))
#define SPM_SLEEP_AUDIO_PWR_MASK_B                 ((volatile uint32_t*)(SPM_BASE + 0x040C))
#define SPM_SLEEP_DSP_PWR_MASK_B                   ((volatile uint32_t*)(SPM_BASE + 0x0410))
#define SPM_SLEEP_PERISYS_PWR_MASK_B               ((volatile uint32_t*)(SPM_BASE + 0x0414))
#define SPM_SLEEP_VOW_PWR_MASK_B                   ((volatile uint32_t*)(SPM_BASE + 0x0418))
#define SPM_SLEEP_SFC_PAD_MACRO_PWR_MASK_B         ((volatile uint32_t*)(SPM_BASE + 0x041C))
#define SPM_DSP_DRAM0H_CONTROL_0                   ((volatile uint32_t*)(SPM_BASE + 0x0420))
#define SPM_DSP_DRAM0H_CONTROL_1                   ((volatile uint32_t*)(SPM_BASE + 0x0424))
#define SPM_DSP_DRAM0H_CONTROL_2                   ((volatile uint32_t*)(SPM_BASE + 0x0428))
#define SPM_DSP_DRAM0L_CONTROL_0                   ((volatile uint32_t*)(SPM_BASE + 0x042C))
#define SPM_DSP_DRAM0L_CONTROL_1                   ((volatile uint32_t*)(SPM_BASE + 0x0430))
#define SPM_DSP_DRAM0L_CONTROL_2                   ((volatile uint32_t*)(SPM_BASE + 0x0434))
#define SPM_DSP_DRAM1H_CONTROL_0                   ((volatile uint32_t*)(SPM_BASE + 0x0438))
#define SPM_DSP_DRAM1H_CONTROL_1                   ((volatile uint32_t*)(SPM_BASE + 0x043C))
#define SPM_DSP_DRAM1H_CONTROL_2                   ((volatile uint32_t*)(SPM_BASE + 0x0440))
#define SPM_DSP_DRAM1L_CONTROL_0                   ((volatile uint32_t*)(SPM_BASE + 0x0444))
#define SPM_DSP_DRAM1L_CONTROL_1                   ((volatile uint32_t*)(SPM_BASE + 0x0448))
#define SPM_DSP_DRAM1L_CONTROL_2                   ((volatile uint32_t*)(SPM_BASE + 0x044C))
#define SPM_DSP_IRAM0H_CONTROL_0                   ((volatile uint32_t*)(SPM_BASE + 0x0450))
#define SPM_DSP_IRAM0H_CONTROL_1                   ((volatile uint32_t*)(SPM_BASE + 0x0454))
#define SPM_DSP_IRAM0H_CONTROL_2                   ((volatile uint32_t*)(SPM_BASE + 0x0458))
#define SPM_DSP_IRAM0L_CONTROL_0                   ((volatile uint32_t*)(SPM_BASE + 0x045C))
#define SPM_DSP_IRAM0L_CONTROL_1                   ((volatile uint32_t*)(SPM_BASE + 0x0460))
#define SPM_DSP_IRAM0L_CONTROL_2                   ((volatile uint32_t*)(SPM_BASE + 0x0464))
#define SPM_DSP_IRAM1_CONTROL_0                    ((volatile uint32_t*)(SPM_BASE + 0x0468))
#define SPM_DSP_IRAM1_CONTROL_1                    ((volatile uint32_t*)(SPM_BASE + 0x046C))
#define SPM_DSP_IRAM1_CONTROL_2                    ((volatile uint32_t*)(SPM_BASE + 0x0470))
#define SPM_DSP_D_TAG_CACHE_CONTROL_0              ((volatile uint32_t*)(SPM_BASE + 0x0474))
#define SPM_DSP_D_TAG_CACHE_CONTROL_1              ((volatile uint32_t*)(SPM_BASE + 0x0478))
#define SPM_DSP_I_TAG_CACHE_CONTROL_0              ((volatile uint32_t*)(SPM_BASE + 0x047C))
#define SPM_DSP_I_TAG_CACHE_CONTROL_1              ((volatile uint32_t*)(SPM_BASE + 0x0480))
#define SPM_AUDIO_G_CONTROL                        ((volatile uint32_t*)(SPM_BASE + 0x0498))
#define SPM_AUDIO_DFX_CH01_CONTROL                 ((volatile uint32_t*)(SPM_BASE + 0x049C))
#define SPM_AUDIO_DFX_CH23_CONTROL                 ((volatile uint32_t*)(SPM_BASE + 0x0500))
#define SPM_AUDIO_DFX_CH45_CONTROL                 ((volatile uint32_t*)(SPM_BASE + 0x0504))
#define SPM_AUDIO_UL1_CONTROL                      ((volatile uint32_t*)(SPM_BASE + 0x0508))
#define SPM_AUDIO_UL2_CONTROL                      ((volatile uint32_t*)(SPM_BASE + 0x050C))
#define SPM_AUDIO_UL3_CONTROL                      ((volatile uint32_t*)(SPM_BASE + 0x0510))
#define SPM_AUDIO_UL4_CONTROL                      ((volatile uint32_t*)(SPM_BASE + 0x0514))
#define SPM_AUDIO_DL_CONTROL                       ((volatile uint32_t*)(SPM_BASE + 0x0518))
#define SPM_AUDIO_AFE_MEM_IF_CONTROL_0             ((volatile uint32_t*)(SPM_BASE + 0x051C))
#define SPM_AUDIO_AFE_MEM_IF_CONTROL_1             ((volatile uint32_t*)(SPM_BASE + 0x0520))
#define SPM_AUDIO_AFE_MEM_IF_CONTROL_2             ((volatile uint32_t*)(SPM_BASE + 0x0524))
#define SPM_AUDIO_ASRC1_CONTROL_0                  ((volatile uint32_t*)(SPM_BASE + 0x052C))
#define SPM_AUDIO_ASRC1_CONTROL_1                  ((volatile uint32_t*)(SPM_BASE + 0x0530))
#define SPM_AUDIO_ASRC1_CONTROL_2                  ((volatile uint32_t*)(SPM_BASE + 0x0534))
#define SPM_AUDIO_ASRC2_CONTROL_0                  ((volatile uint32_t*)(SPM_BASE + 0x053C))
#define SPM_AUDIO_ASRC2_CONTROL_1                  ((volatile uint32_t*)(SPM_BASE + 0x0540))
#define SPM_AUDIO_ASRC2_CONTROL_2                  ((volatile uint32_t*)(SPM_BASE + 0x0544))
#define SPM_SKIP_SFC_EMI_TASK_DSP1                 ((volatile uint32_t*)(SPM_BASE + 0x054C))
#define SPM_VOW_SRAM_CONTROL                       ((volatile uint32_t*)(SPM_BASE + 0x0560))
#define SPM_VOW_SRAM_CONTROL_RET                   ((volatile uint32_t*)(SPM_BASE + 0x0564))
#define SPM_USB_SRAM_CONTROL                       ((volatile uint32_t*)(SPM_BASE + 0x0568))
#define SPM_USB_SRAM_CONTROL_RET                   ((volatile uint32_t*)(SPM_BASE + 0x056C))
#define SPM_AUDIO_ANC_SRAM_CONTROL                 ((volatile uint32_t*)(SPM_BASE + 0x0700))
#define SPM_DSP_WAKEUP_SOURCE_STA_IN_B             ((volatile uint32_t*)(SPM_BASE + 0x07F0))
#define SPM_DSP_WAKEUP_SOURCE_STA                  ((volatile uint32_t*)(SPM_BASE + 0x07F4))
#define SPM_DSP_WAKEUP_SOURCE_STA_32K              ((volatile uint32_t*)(SPM_BASE + 0x07F8))
#define SPM_CM4_WAKEUP_SOURCE_STA_IN_B             ((volatile uint32_t*)(SPM_BASE + 0x07FC))
#define SPM_CM4_WAKEUP_SOURCE_STA                  ((volatile uint32_t*)(SPM_BASE + 0x0800))
#define SPM_CM4_WAKEUP_SOURCE_STA_32K              ((volatile uint32_t*)(SPM_BASE + 0x0804))
#define SPM_PWR_STATUS                             ((volatile uint32_t*)(SPM_BASE + 0x0808))
#define SPM_PWR_STATUS_2ND                         ((volatile uint32_t*)(SPM_BASE + 0x080C))
#define SPM_STATE_STATUS                           ((volatile uint32_t*)(SPM_BASE + 0x0810))
#define SPM_SLEEP_ISR_STATUS                       ((volatile uint32_t*)(SPM_BASE + 0x0820))
#define SPM_BUS_IDLE_STATUS                        ((volatile uint32_t*)(SPM_BASE + 0x0824))
#define SPM_SLEEP_ISR_RAW_STA                      ((volatile uint32_t*)(SPM_BASE + 0x0830))
#define SPM_CONN_AUDIO_ABB_XO_SIDEBAND             ((volatile uint32_t*)(SPM_BASE + 0x0834))
#define SPM_CONN_SIDEBAND_0                        ((volatile uint32_t*)(SPM_BASE + 0x0838))
#define SPM_CONN_SIDEBAND_1                        ((volatile uint32_t*)(SPM_BASE + 0x083C))
#define SPM_PCM_REG0_DATA                          ((volatile uint32_t*)(SPM_BASE + 0x0900))
#define SPM_PCM_REG1_DATA                          ((volatile uint32_t*)(SPM_BASE + 0x0904))
#define SPM_PCM_REG2_DATA                          ((volatile uint32_t*)(SPM_BASE + 0x0908))
#define SPM_PCM_REG3_DATA                          ((volatile uint32_t*)(SPM_BASE + 0x090C))
#define SPM_PCM_REG4_DATA                          ((volatile uint32_t*)(SPM_BASE + 0x0910))
#define SPM_PCM_REG5_DATA                          ((volatile uint32_t*)(SPM_BASE + 0x0914))
#define SPM_PCM_REG6_DATA                          ((volatile uint32_t*)(SPM_BASE + 0x0918))
#define SPM_PCM_REG7_DATA                          ((volatile uint32_t*)(SPM_BASE + 0x091C))
#define SPM_PCM_REG8_DATA                          ((volatile uint32_t*)(SPM_BASE + 0x0920))
#define SPM_PCM_REG9_DATA                          ((volatile uint32_t*)(SPM_BASE + 0x0924))
#define SPM_PCM_REG10_DATA                         ((volatile uint32_t*)(SPM_BASE + 0x0928))
#define SPM_PCM_REG11_DATA                         ((volatile uint32_t*)(SPM_BASE + 0x092C))
#define SPM_PCM_REG12_DATA                         ((volatile uint32_t*)(SPM_BASE + 0x0930))
#define SPM_PCM_REG13_DATA                         ((volatile uint32_t*)(SPM_BASE + 0x0934))
#define SPM_PCM_REG14_DATA                         ((volatile uint32_t*)(SPM_BASE + 0x0938))
#define SPM_PCM_REG15_DATA                         ((volatile uint32_t*)(SPM_BASE + 0x093C))
#define SPM_PCM_TIMER_OUT                          ((volatile uint32_t*)(SPM_BASE + 0x0980))
#define SPM_PCM_EVENT_REG_STA                      ((volatile uint32_t*)(SPM_BASE + 0x09E0))
#define SPM_PCM_FSM_STA                            ((volatile uint32_t*)(SPM_BASE + 0x09E4))
#define SPM_PCM_RESERVE                            ((volatile uint32_t*)(SPM_BASE + 0x0B00))
#define SPM_PCM_RESERVE2                           ((volatile uint32_t*)(SPM_BASE + 0x0B04))
#define SPM_PCM_FLAGS                              ((volatile uint32_t*)(SPM_BASE + 0x0B10))
#define SPM_SPM_AP_SEMA                            ((volatile uint32_t*)(SPM_BASE + 0x0B40))
#define SPM_SPM_SPM_SEMA                           ((volatile uint32_t*)(SPM_BASE + 0x0B44))
#define SPM_PCM_SW_INT_SET                         ((volatile uint32_t*)(SPM_BASE + 0x0B90))
#define SPM_PCM_SW_INT_CLEAR                       ((volatile uint32_t*)(SPM_BASE + 0x0B94))
#define SPM_PCM_WDT_TIMER_VAL                      ((volatile uint32_t*)(SPM_BASE + 0x0C00))
#define SPM_PCM_WDT_TIMER_OUT                      ((volatile uint32_t*)(SPM_BASE + 0x0C04))
#define SPM_PCM_WDT_LATCH                          ((volatile uint32_t*)(SPM_BASE + 0x0C20))
#define SPM_SLEEP_TWAM_CON_0                       ((volatile uint32_t*)(SPM_BASE + 0x0C80))
#define SPM_SLEEP_TWAM_CON_1                       ((volatile uint32_t*)(SPM_BASE + 0x0C84))
#define SPM_SLEEP_TWAM_CON_2                       ((volatile uint32_t*)(SPM_BASE + 0x0C88))
#define SPM_SLEEP_TWAM_LAST_STATUS0                ((volatile uint32_t*)(SPM_BASE + 0x0C90))
#define SPM_SLEEP_TWAM_CURR_STATUS0                ((volatile uint32_t*)(SPM_BASE + 0x0CA0))
#define SPM_SLEEP_TWAM_TIMER_OUT                   ((volatile uint32_t*)(SPM_BASE + 0x0CB0))
#define SPM_SLEEP_TWAM_WINDOW_LEN                  ((volatile uint32_t*)(SPM_BASE + 0x0CB4))
#define SPM_SW_DBG_STATE_STATUS                    ((volatile uint32_t*)(SPM_BASE + 0x0CB8))
#define SPM_DUMMY_REG_A                            ((volatile uint32_t*)(SPM_BASE + 0x0CBC))
#define SPM_DUMMY_REG_B                            ((volatile uint32_t*)(SPM_BASE + 0x0CC0))
#define SPM_CONN_TOP_ON_MTCMOS_OFF                 ((volatile uint32_t*)(SPM_BASE + 0x0CF0))
#define SPM_CONN_TOP_OFF_MTCMOS_OFF                ((volatile uint32_t*)(SPM_BASE + 0x0CF4))
#define SPM_CONN_TOP_OFF_RESTORE_RDY               ((volatile uint32_t*)(SPM_BASE + 0x0CF8))
#define SPM_PERISYS_PWR_CON                        ((volatile uint32_t*)(SPM_BASE + 0x1000))
#define SPM_VOW_PWR_CON                            ((volatile uint32_t*)(SPM_BASE + 0x1004))
#define SPM_SFC_PAD_MACRO_PWR_CON                  ((volatile uint32_t*)(SPM_BASE + 0x100C))
#define SPM_SKIP_SFC_EMI_TASK_DSP                  ((volatile uint32_t*)(SPM_BASE + 0x1014))
#define SPM_SLEEP_ISR_STATUS_DSP                   ((volatile uint32_t*)(SPM_BASE + 0x1024))
#define SPM_PCM_SW_INT_SET_DSP                     ((volatile uint32_t*)(SPM_BASE + 0x1028))
#define SPM_PCM_SW_INT_CLEAR_DSP                   ((volatile uint32_t*)(SPM_BASE + 0x1030))
#define SPM_CM4_TCM2K_CONTROL_4                    ((volatile uint32_t*)(SPM_BASE + 0x1034))
#define SPM_CM4_TCM4K_CONTROL_4                    ((volatile uint32_t*)(SPM_BASE + 0x1038))
#define SPM_CM4_TCM8K_CONTROL_4                    ((volatile uint32_t*)(SPM_BASE + 0x103C))
#define SPM_CM4_TCM16K_CONTROL_4                   ((volatile uint32_t*)(SPM_BASE + 0x1040))
#define SPM_INFRA_SYSRAM0_CONTROL_4                ((volatile uint32_t*)(SPM_BASE + 0x1044))
#define SPM_INFRA_SYSRAM1_CONTROL_4                ((volatile uint32_t*)(SPM_BASE + 0x1048))
#define SPM_INFRA_SYSRAM2_CONTROL_4                ((volatile uint32_t*)(SPM_BASE + 0x104C))
#define SPM_INFRA_SYSRAM3_CONTROL_4                ((volatile uint32_t*)(SPM_BASE + 0x1050))
#define SPM_DSP_DRAM0H_CONTROL_4                   ((volatile uint32_t*)(SPM_BASE + 0x1054))
#define SPM_DSP_DRAM0L_CONTROL_4                   ((volatile uint32_t*)(SPM_BASE + 0x1058))
#define SPM_DSP_DRAM1H_CONTROL_4                   ((volatile uint32_t*)(SPM_BASE + 0x105C))
#define SPM_DSP_DRAM1L_CONTROL_4                   ((volatile uint32_t*)(SPM_BASE + 0x1060))
#define SPM_DSP_IRAM0H_CONTROL_4                   ((volatile uint32_t*)(SPM_BASE + 0x1064))
#define SPM_DSP_IRAM0L_CONTROL_4                   ((volatile uint32_t*)(SPM_BASE + 0x1068))
#define SPM_DSP_IRAM1_CONTROL_4                    ((volatile uint32_t*)(SPM_BASE + 0x106C))
#define SPM_AUDIO_AFE_MEM_IF_CONTROL_4             ((volatile uint32_t*)(SPM_BASE + 0x1070))
#define SPM_AUDIO_ASRC1_CONTROL_4                  ((volatile uint32_t*)(SPM_BASE + 0x1074))
#define SPM_AUDIO_ASRC2_CONTROL_4                  ((volatile uint32_t*)(SPM_BASE + 0x1078))

#define INFRA_MISC_CFG 0xA2200000
#define INFRA_CFG_MTCMOS1                          ((volatile uint32_t*)(INFRA_MISC_CFG + 0x44))
#define INFRA_CFG_MTCMOS2                          ((volatile uint32_t*)(INFRA_MISC_CFG + 0x48))
#define INFRA_CFG_MTCMOS3                          ((volatile uint32_t*)(INFRA_MISC_CFG + 0x4C))
#define INFRA_CFG_MTCMOS4                          ((volatile uint32_t*)(INFRA_MISC_CFG + 0x50))
#define INFRA_CFG_MTCMOS5                          ((volatile uint32_t*)(INFRA_MISC_CFG + 0x54))

#define HAL_SPM_DEEBUG_ENABLE
#ifdef  HAL_SPM_DEEBUG_ENABLE
#define log_hal_debug(_message,...)                printf(_message, ##__VA_ARGS__)
#define SPM_SET_CM4_SW_STATE(x)                    *((volatile uint32_t*)(SPM_STATE_STATUS)) = (x + 0x80)
#else
#define log_hal_debug(_message,...)
#define SPM_SET_CM4_SW_STATE(x)
#endif

//#define SPM_DSP0_WAKEUP_SOURCE_MASK                  ((volatile uint32_t*)(SPM_BASE + 0x0000))
//#define SPM_DSP1_WAKEUP_SOURCE_MASK                  ((volatile uint32_t*)(SPM_BASE + 0x0000))

#define SPM_INFRA_OFF_FLAG                         *((volatile uint32_t*)(SPM_BASE + 0x0CB4))

int spm_init(void);

typedef enum {
    SPM_MTCMOS_CONN_TOP_ON  = 0,
    SPM_MTCMOS_CONN_TOP_OFF = 1,
    SPM_MTCMOS_AUDIO        = 2,
    SPM_MTCMOS_DSP          = 3,
    SPM_MTCMOS_PERISYS      = 4,
    SPM_MTCMOS_VOW          = 5,
} spm_mtcmos_type_t;


typedef enum {
    SPM_MTCMOS_PWR_DISABLE  = 0,
    SPM_MTCMOS_PWR_ENABLE   = 1
} spm_mtcmos_ctrl_t;

typedef enum {
    SPM_DISABLE  = 0,
    SPM_ENABLE   = 1
} spm_ctrl_t;

typedef enum {
    SPM_STATE1      = 0,
    SPM_STATE3      = 1,
    SPM_STATE4      = 2,
    SPM_STATE_MAX   = 3
} spm_sleep_state_t;

typedef enum {
    SPM_AUDIO_REQUEST   = 0,
    SPM_PMIC_REQUEST    = 1,
    SPM_ISINK_REQUEST   = 2,
    SPM_I2S_CLK_OUTPUT  = 3,
    SPM_RTC             = 4,
    SPM_REQUEST_MAX     = 5
} spm_request_t;

uint32_t spm_control_mtcmos(spm_mtcmos_type_t mtcmos, spm_mtcmos_ctrl_t ctrl);
void spm_mask_wakeup_source(uint32_t wakeup_source);
void spm_unmask_wakeup_source(uint32_t wakeup_source);
void spm_audio_lowpower_setting(spm_sleep_state_t sleep_state, spm_ctrl_t enable);
void spm_force_on_pmic(spm_request_t spm_request,spm_ctrl_t enable);

void spm_debug_io(unsigned int debug_bus);

void spm_mtcmos_conn_top_on_on(void);
void spm_mtcmos_conn_top_on_off(void);
void spm_mtcmos_conn_top_off_on(void);
void spm_mtcmos_conn_top_off_off(void);
void spm_mtcmos_audio_on(void);
void spm_mtcmos_audio_off(void);
void spm_mtcmos_dsp_on(void);
void spm_mtcmos_dsp_off(void);
void spm_mtcmos_perisys_on(void);
void spm_mtcmos_perisys_off(void);
void spm_mtcmos_vow_on(void);
void spm_mtcmos_vow_off(void);
void spm_force_sleep_state(spm_request_t user, spm_sleep_state_t state, spm_ctrl_t enable);

#endif
#endif

