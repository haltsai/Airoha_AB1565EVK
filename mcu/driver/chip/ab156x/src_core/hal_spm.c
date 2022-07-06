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

#include "hal_spm.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED

#include <stdio.h>
#include <string.h>
#include "hal_log.h"
#include "memory_attribute.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "assert.h"
#include "hal_gpt.h"
#include "hal_platform.h"
#include "hal_wdt_internal.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"

/* SPM Code Rev : pcm_suspend_20210701.spm */
const unsigned int pcm_event_vector_parameter[8] = {0x31,0xd60033,0x100035,0xab0030,0x350034,0x0,0x0,0x0};

#define PCM_IM_MAX_LENGTH 1458

static const unsigned int spm_im[PCM_IM_MAX_LENGTH] = {
0x18c0001f,0xa2110810,0xe0e0004f,0x18c0001f,0xa21103a0,0xe0e00000,0x1940001f,0xa2110184,0xe1600000,
0xe8208000,0xa2110318,0x0b16ff08,0x1b00001f,0x00090001,0xf0000000,0x17c07c1f,0x18c0001f,0xa2110810,
0xe0e0003d,0x1b00001f,0x00800001,0xf0000000,0x17c07c1f,0x18c0001f,0xa2110b90,0xe0e00001,0x18c0001f,
0xa2110c80,0xe0e00100,0x1900001f,0xa21103a0,0xe1200000,0x1b00001f,0x00200001,0x1940001f,0x00000001,
0xf0000000,0x17c07c1f,0x18c0001f,0xa2111028,0xe0e00001,0x18c0001f,0xa2110c84,0xe0e00100,0x1900001f,
0xa21103a0,0xe1200000,0x1b00001f,0x00200001,0x1940001f,0x00000001,0xf0000000,0x17c07c1f,0x18c0001f,
0xa2110810,0xe0e00031,0x1910001f,0xa21103b0,0x81081001,0xc8c014e4,0x17c07c1f,0xc8e00824,0x17c07c1f,
0xf0000000,0x17c07c1f,0x18c0001f,0xa21103a0,0x1980001f,0x00060000,0xe0c00006,0x1910001f,0xa21103a0,
0x810c9001,0x1940001f,0x00000000,0x1990001f,0xa21107f0,0x1a10001f,0xa21107fc,0xc92004c6,0x17c07c1f,
0xc92002e8,0x17c07c1f,0xd82008c4,0x17c07c1f,0x1910001f,0xa21103a0,0x810d1001,0x1940001f,0x00000000,
0x1990001f,0xa21107f0,0x1a10001f,0xa21107fc,0xc92004c6,0x17c07c1f,0xc92002e8,0x17c07c1f,0xd8200aa4,
0x17c07c1f,0x1980001f,0xa2200050,0xe1a00001,0x1910001f,0xa2200050,0x81041001,0xd8200ce4,0x17c07c1f,
0x18c0001f,0xa2110810,0xe0e00033,0x18c0001f,0xa2111000,0x1940001f,0x00000004,0xc0c02ca0,0x17c07c1f,
0x18c0001f,0xa21103a0,0x1980001f,0x00010000,0xe0c00006,0x1910001f,0xa21103a0,0x810c1001,0x1940001f,
0x00000000,0x1990001f,0xa21107f0,0x1a10001f,0xa21107fc,0xc92004c6,0x17c07c1f,0xc92002e8,0x17c07c1f,
0xd8200f44,0x17c07c1f,0x18c0001f,0xa2110810,0xe0e00032,0x18c0001f,0xa2110204,0x1940001f,0x00000001,
0x1910001f,0xa21103ac,0x81041001,0xc8e02ca4,0x17c07c1f,0x18c0001f,0xa211100c,0x1940001f,0x00000000,
0xc0c02ca0,0x17c07c1f,0x1940001f,0xa2110cb4,0xe1600001,0x1910001f,0xa2110b00,0x91201001,0xa0121000,
0xa0108400,0x1b00001f,0x00090001,0xf0000000,0x17c07c1f,0x1b00001f,0x00200001,0xf0000000,0x17c07c1f,
0x18c0001f,0xa2110810,0xe0e00041,0x1940001f,0xa2110184,0xe1600001,0x1990001f,0xa2110368,0x81841801,
0x1910001f,0xa21103b0,0x810c1001,0xa1001804,0xc8c017c4,0x17c07c1f,0xc8e01904,0x17c07c1f,0xf0000000,
0x17c07c1f,0x18c0001f,0xa2110810,0xe0e00044,0x1940001f,0xa2110184,0xe1600000,0x1b00001f,0x00090001,
0xf0000000,0x17c07c1f,0xa0120400,0x1b00001f,0x00020001,0x18c0001f,0xa21103a0,0xe0e00001,0xe8208000,
0xa2110318,0x0b16ff0c,0xe8208000,0xa2110318,0x0b16ff08,0xf0000000,0x17c07c1f,0x18c0001f,0xa2110810,
0xe0e0003f,0x80308400,0x1b80001f,0x200005a7,0x80320400,0x18c0001f,0xa2110810,0xe0e0003e,0x18c0001f,
0xa2110204,0x1940001f,0x00000001,0x1910001f,0xa2110808,0x81009001,0xc8e02024,0x17c07c1f,0x1940001f,
0x00000000,0x18c0001f,0xa211100c,0xc0c02020,0x17c07c1f,0x1940001f,0x00000004,0x18c0001f,0xa2111000,
0xc0c02020,0x17c07c1f,0x1940001f,0xa2200050,0xe1600000,0x1910001f,0xa2200050,0x81041001,0xd8001f04,
0x17c07c1f,0x1b00001f,0x00300001,0xf0000000,0x17c07c1f,0xe0e00016,0x1b80001f,0x20000003,0xe0e0001e,
0x1b80001f,0x20000003,0x81801401,0xd8002346,0x17c07c1f,0x81809401,0xd80026e6,0x17c07c1f,0x81811401,
0xd8002246,0x17c07c1f,0xd0002b80,0x17c07c1f,0x1900001f,0xa2110568,0x1980001f,0x01000100,0xe1200100,
0xe1000006,0xd0002b80,0x17c07c1f,0x1900001f,0xa211027c,0xe1200007,0xe1200003,0xe1200001,0xe1200000,
0x1980001f,0x01000000,0xe1000006,0x1900001f,0xa211028c,0xe1200007,0xe1200003,0xe1200001,0xe1200000,
0xe1000006,0x1900001f,0xa211029c,0xe1200007,0xe1200003,0xe1200001,0xe1200000,0xe1000006,0x1900001f,
0xa211030c,0xe1200000,0xe1000006,0xd0002b80,0x17c07c1f,0x1980001f,0x01000000,0x1900001f,0xa2110224,
0xe1200000,0x1900001f,0xa2110244,0xe1200000,0x1900001f,0xa2110254,0xe1200000,0x1900001f,0xa2110264,
0xe1200000,0x1900001f,0xa2110274,0xe1200000,0x1900001f,0xa2110270,0xe1200000,0x1900001f,0xa2110224,
0xe1000006,0x1900001f,0xa2110244,0xe1000006,0x1900001f,0xa2110254,0xe1000006,0x1900001f,0xa2110264,
0xe1000006,0x1900001f,0xa2110270,0xe1000006,0xd0002b80,0x17c07c1f,0xe0e0000e,0xe0e0000c,0xe0e0001c,
0x1b80001f,0x20000004,0xe0e0001d,0xe0e0000d,0xf0000000,0x17c07c1f,0xe0e0001d,0xe0e0001f,0x81801401,
0xd8002f06,0x17c07c1f,0x81809401,0xd8003266,0x17c07c1f,0x81811401,0xd8002e46,0x17c07c1f,0xd0003720,
0x17c07c1f,0x1900001f,0xa2110568,0xe1200100,0xe1200101,0xd0003720,0x17c07c1f,0x1900001f,0xa211027c,
0xe1200000,0xe1200001,0xe1200003,0xe1200007,0xe120000f,0x1900001f,0xa211028c,0xe1200000,0xe1200001,
0xe1200003,0xe1200007,0xe120000f,0x1900001f,0xa211029c,0xe1200000,0xe1200001,0xe1200003,0xe1200007,
0xe120000f,0x1900001f,0xa211030c,0xe1200000,0xe1200001,0xd0003720,0x17c07c1f,0x1900001f,0xa2110224,
0xe1200000,0x1900001f,0xa2110244,0xe1200000,0x1900001f,0xa2110254,0xe1200000,0x1900001f,0xa2110264,
0xe1200000,0x1900001f,0xa2110270,0xe1200000,0x1900001f,0xa2110274,0xe1200001,0x1900001f,0xa2110224,
0xe1200001,0xe1200003,0xe1200007,0xe120000f,0x1900001f,0xa2110244,0xe120000f,0x1900001f,0xa2110254,
0xe120000f,0x1900001f,0xa2110264,0xe120000f,0x1900001f,0xa2110270,0xe120000f,0xd0003720,0x17c07c1f,
0xe0e0001e,0xe0e0001a,0xe0e00012,0xf0000000,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,
0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x17c07c1f,0x1840001f,0x00000001,
0x18c0001f,0xa2110810,0xe0e00001,0x81f08407,0xa1d00407,0x18c0001f,0xa2110380,0xe0f07fff,0xe8208000,
0xa2110318,0x0b16ff03,0x18c0001f,0xa2110810,0xe0e00017,0x1b00001f,0x07400001,0x18d0001f,0xa2110930,
0x80cc8c01,0xd80016a3,0x17c07c1f,0x1b00001f,0x07400001,0x18d0001f,0xa2110930,0x80cc0c01,0xd8000fe3,
0x17c07c1f,0x1b00001f,0x07400001,0x18d0001f,0xa2110930,0x80cb0c01,0x1910001f,0xa21103b0,0x81001001,
0x80e01003,0xd82001a3,0x17c07c1f,0x18c0001f,0xa21101c0,0x1910001f,0xa0020004,0xe0c00004,0x18c0001f,
0xa21101c4,0x1910001f,0xa0020008,0xe0c00004,0x18c0001f,0xa21101a0,0x1910001f,0xa002000c,0xe0c00004,
0x18c0001f,0xa20900c0,0x1910001f,0xa0020020,0xe0c00004,0x18c0001f,0xa20900c4,0x1910001f,0xa0020028,
0xe0c00004,0x18c0001f,0xa2110810,0xe0e00021,0x18c0001f,0xa21101d0,0xe0e00000,0x18d0001f,0xa21101d4,
0x81000c01,0x80c40c01,0xd8000d03,0x17c07c1f,0x18c0001f,0xa2110810,0xe0e00022,0x18c0001f,0xa0020800,
0xe0e000b9,0x18c0001f,0xa0020010,0xe0e00001,0x18c0001f,0xa0020014,0xe0e00000,0x18c0001f,0xa0020000,
0xe0e0000c,0x18d0001f,0xa0020000,0x80c00c01,0xd8000ba3,0x17c07c1f,0x18c0001f,0xa0020000,0xe0e00000,
0xa0128400,0x1b80001f,0x20000398,0x1b00001f,0x00300001,0x1b80001f,0xd0100001,0x1b80001f,0x90100001,
0x18d0001f,0xa2110808,0x81008c01,0x1950001f,0xa2110830,0x814a9401,0x81801404,0xd8200d46,0x17c07c1f,
0xd0002e60,0x17c07c1f,0xd00001a0,0x17c07c1f,0x18c0001f,0xa2110810,0xe0e000ff,0xf0000000,0x18c0001f,
0xa2110810,0xe0e00011,0x18c0001f,0xa2110200,0x1940001f,0x00000002,0x1910001f,0xa2110808,0x81001001,
0xd82013a4,0x17c07c1f,0x81f10407,0xc8e01364,0x1980001f,0xa2200040,0xe1a00001,0x1910001f,0xa2200040,
0x81041001,0xd8201204,0x17c07c1f,0xc0c02ca0,0x17c07c1f,0x1900001f,0xa2110cbc,0x1144041f,0xe1000005,
0xd00003c0,0x17c07c1f,0x1a00001f,0xa2110b90,0xe2200001,0xc8e02024,0x17c07c1f,0xa1d10407,0x1940001f,
0xa2200040,0xe1600000,0x1910001f,0xa2200040,0x81041001,0xd80014c4,0x17c07c1f,0xd00003c0,0x17c07c1f,
0x1a00001f,0xa2110b90,0xe2200001,0x1a00001f,0xa2110c80,0xe2200100,0xd00003c0,0x17c07c1f,0x18c0001f,
0xa2110810,0xe0e00012,0x18c0001f,0xa2110214,0x1940001f,0x00000004,0x1910001f,0xa2110808,0x81029001,
0xd8202184,0x17c07c1f,0x1940001f,0xa21f0018,0xe1600001,0x1980001f,0xa2200044,0xe1a00001,0x1910001f,
0xa2200044,0x81041001,0xd82018e4,0x17c07c1f,0xe0e0001d,0xe0e0001f,0x1a00001f,0x00000000,0x1a40001f,
0x000000ff,0x1900001f,0xa2110424,0xe1000008,0xe1000009,0x1900001f,0xa2110430,0xe1000008,0xe1000009,
0x1900001f,0xa211043c,0xe1000008,0xe1000009,0x1900001f,0xa2110448,0xe1000008,0xe1000009,0x1900001f,
0xa2110454,0xe1000008,0xe1000009,0x1900001f,0xa2110460,0xe1000008,0xe1000009,0x1900001f,0xa211046c,
0xe1000008,0xe1000009,0x1900001f,0xa2110478,0xe1000008,0xe1200001,0xe1200003,0xe1200007,0xe120000f,
0xe120001f,0xe120003f,0xe120007f,0xe120007f,0x1900001f,0xa2110480,0xe1000008,0xe1200001,0xe1200003,
0xe1200007,0xe120000f,0xe120001f,0xe120003f,0xe120007f,0xe120007f,0xe0e0001e,0xe0e0001a,0xe0e00012,
0x1900001f,0xa2110cbc,0xe1200001,0xd00002e0,0x17c07c1f,0x1a00001f,0xa2111028,0xe2200001,0xe0e00016,
0x1b80001f,0x20000003,0xe0e0001e,0x1b80001f,0x20000003,0x1900001f,0xa2110424,0xe1200000,0x1940001f,
0xa2110430,0xe1600000,0x1980001f,0xa211043c,0xe1a00000,0x1900001f,0xa2110448,0xe1200000,0x1940001f,
0xa2110454,0xe1600000,0x1980001f,0xa2110460,0xe1a00000,0x1900001f,0xa211046c,0xe1200000,0x1940001f,
0xa2110478,0xe16000f7,0xe16000f3,0xe16000f1,0xe16000f0,0xe1600070,0xe1600030,0xe1600010,0xe1600000,
0x1980001f,0xa2110480,0xe1a000f7,0xe1a000f3,0xe1a000f1,0xe1a000f0,0xe1a00070,0xe1a00030,0xe1a00010,
0xe1a00000,0xe0e0000e,0xe0e0001e,0xe0e0001c,0x1b80001f,0x2000001a,0xe0e0001d,0x1a40001f,0x01000000,
0xe1000009,0xe1400009,0xe1800009,0x1900001f,0xa2110424,0x1940001f,0xa2110430,0x1980001f,0xa211043c,
0xe1000009,0xe1400009,0xe1800009,0x1900001f,0xa2110448,0x1940001f,0xa2110454,0x1980001f,0xa2110460,
0xe1000009,0xe1400009,0xe1800009,0xe0e0000d,0x1980001f,0xa2200044,0x1a00001f,0x00000000,0xe1800008,
0x1910001f,0xa2200044,0x81041001,0xd8002c24,0x17c07c1f,0x1940001f,0xa21f0018,0xe1600000,0xd00002e0,
0x17c07c1f,0x1a00001f,0xa2111028,0xe2200001,0x1a00001f,0xa2110c84,0xe2200001,0xd00002e0,0x17c07c1f,
0x18c0001f,0xa0020004,0x1910001f,0xa21101c0,0xe0c00004,0x18c0001f,0xa0020008,0x1910001f,0xa21101c4,
0xe0c00004,0x18c0001f,0xa002000c,0x1910001f,0xa21101a0,0xe0c00004,0x18c0001f,0xa0020020,0x1910001f,
0xa20900c0,0xe0c00004,0x18c0001f,0xa0020028,0x1910001f,0xa20900c4,0xe0c00004,0x18d0001f,0xa21101d4,
0x81000c01,0x80c40c01,0xd80035a3,0x17c07c1f,0x18c0001f,0xa2110810,0xe0e0002f,0x80328400,0x18c0001f,
0xa0020800,0xe0e000ab,0x18c0001f,0xa0020010,0xe0e00001,0x18c0001f,0xa0020014,0xe0e00000,0x18c0001f,
0xa0020000,0xe0e00008,0xe0e0000c,0x18d0001f,0xa0020000,0x80c00c01,0xd8003463,0x17c07c1f,0x18c0001f,
0xa0020000,0xe0e00000,0x1b80001f,0x20000a50,0x18c0001f,0xa21101d0,0xe0e00101,0xd00001a0,0x17c07c1f,
};

static struct {
    uint32_t conn_top_on;
    uint32_t conn_top_off;
    uint32_t audio;
    uint32_t dsp;
    uint32_t perisys;
    uint32_t vow;
} mtcmos_resource;

uint32_t manual_spm_read_im(uint32_t addr);
void manual_spm_write_im(uint32_t addr, uint32_t data);

void manual_spm_write_im(uint32_t addr, uint32_t data)
{
    *SPM_PCM_IM_HOST_RW_PTR = 0x01010000 | addr;
    *SPM_PCM_IM_HOST_RW_DAT = data;
    return;
}

uint32_t manual_spm_read_im(uint32_t addr)
{
    uint32_t data;
    *SPM_PCM_IM_HOST_RW_PTR = 0x01000000 | addr;
    data = *SPM_PCM_IM_HOST_RW_DAT;
    *SPM_PCM_IM_HOST_RW_PTR = 0;
    return (data);
}

int spm_init(void)
{
    int result = 0;
    uint32_t i, status;

    //spm_debug_io(0x43424342);
    /* [4]: Resets PCM */
    *SPM_PCM_CON0 = 0x0B160000 | 0x10;
    hal_gpt_delay_us(10);
    *SPM_PCM_CON0 = 0x0B160000;
    hal_gpt_delay_us(10);

    /* Load PCM_EVENT_VECTOR[0-7] */
    *SPM_PCM_EVENT_VECTOR0 = pcm_event_vector_parameter[0];
    *SPM_PCM_EVENT_VECTOR1 = pcm_event_vector_parameter[1];
    *SPM_PCM_EVENT_VECTOR2 = pcm_event_vector_parameter[2];
    *SPM_PCM_EVENT_VECTOR3 = pcm_event_vector_parameter[3];
    *SPM_PCM_EVENT_VECTOR4 = pcm_event_vector_parameter[4];
    *SPM_PCM_EVENT_VECTOR5 = pcm_event_vector_parameter[5];
    *SPM_PCM_EVENT_VECTOR6 = pcm_event_vector_parameter[6];
    *SPM_PCM_EVENT_VECTOR7 = pcm_event_vector_parameter[7];

    /* PCM kick*/
    *SPM_POWER_ON_VAL0 = 0;             /* POWER_ON_VAL0 */
    *SPM_POWER_ON_VAL1 = 0x84;          /* POWER_ON_VAL1 */
    *SPM_PCM_REG_DATA_INI = *SPM_POWER_ON_VAL0 | 0x1; /* [0]: sc_xo_cg_en default =1 */
    *SPM_PCM_PWR_IO_EN = 0x00010000;    /* for R0 */
    *SPM_PCM_PWR_IO_EN = 0x00000000;
    *SPM_PCM_REG_DATA_INI = *SPM_POWER_ON_VAL1;
    *SPM_PCM_PWR_IO_EN = 0x00800000;    /* for R7 */
    *SPM_PCM_PWR_IO_EN = 0x00000000;
    *SPM_CLK_SETTLE = 0x3;           /* CLK_SETTLE */
    *SPM_PCM_PWR_IO_EN = 0x1;

    *SPM_SLEEP_WAKEUP_EVENT_MASK = 0x0;           /* mask all AP wakeup event */

    /* Init IM Length and pointer */
    *SPM_PCM_IM_LEN = PCM_IM_MAX_LENGTH;
    *SPM_PCM_IM_PTR = (uint32_t)spm_im;
    /* manual load spm code */
    for (i = 0; i < PCM_IM_MAX_LENGTH; i++) {
        manual_spm_write_im(i, spm_im[i]);
    }
    *SPM_PCM_CON1 = 0x0B169C49;
    /* Kick IM process */
    *SPM_PCM_CON0 = 0x0B160002;

    /* Wait ready state */
    do {
        status = (*SPM_PCM_FSM_STA >> 9) & 0x1;
    } while (status != 0x01);

    *SPM_PCM_PWR_IO_EN = 0x1;        // enable R0 & R7 output
    *SPM_SEQUENCER_26M_REG_1 = 0x1B;

    *((volatile uint32_t *)(SPM_BASE + 0x0104)) = 0x0;         /* rg_active_guard_time */
    *((volatile uint32_t *)(SPM_BASE + 0x0108)) = 0x0;         /* rg_sleep_guard_time */
    *((volatile uint32_t *)(SPM_BASE + 0x010C)) = 0x0;        /* pmu_en_delay = 0,osc_en_delay = 0,xo_en_delay = 7 */
    *((volatile uint32_t *)(SPM_BASE + 0x0110)) = 0x50;        /* rg_vcore_xo_settle_time */

    /* Wakeup Source mask setting */
    *SPM_CM4_WAKEUP_SOURCE_MASK = 0x10;
    *SPM_DSP_WAKEUP_SOURCE_MASK = 0x0;

    *SPM_SKIP_SFC_EMI_TASK |= (1 << 8);    /* SKIP SFC */

    for (i = 0; i < PCM_IM_MAX_LENGTH; i++) {
        /* Check SPM Code */
        if (manual_spm_read_im(i) != spm_im[i]) {
            log_hal_msgid_info("SPM Code loading Fail\r\n",0);
            return (result++);
        }
    }

#if 0
    *((volatile uint32_t *)(0xA2060010)) |= 0x1000000;          /* xo ready enable */
    *((volatile uint32_t *)(SPM_BASE + 0x0110)) = 0x1D;         /* rg_vcore_xo_settle_time */
#endif

    /* [4]: Resets PCM */
    *SPM_PCM_CON0 = 0x0B160000 | 0x10;
    hal_gpt_delay_us(10);
    *SPM_PCM_CON0 = 0x0B160000;
    hal_gpt_delay_us(10);

    /* Disable DCXO in sleep mode */
    *SPM_CONN_AUDIO_ABB_XO_SIDEBAND = 0;

    /* SPM Kick Start */
    *SPM_PCM_CON0 = 0x0B160103;

    //SPM_CLEAR_LOCK_INFRA;
    //SPM_INFRA_OFF_FLAG = 0;

    /* Clear INFRA Power off flag */
    SPM_INFRA_OFF_FLAG = 0;
    return (result);
}

void spm_control_mtcmos_internal(spm_mtcmos_type_t mtcmos, spm_mtcmos_ctrl_t ctrl)
{
    bool audio_clock_old_status;

    if (mtcmos == SPM_MTCMOS_CONN_TOP_ON) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            /* CONN TOP ON MTCMOS OFF */
            spm_mtcmos_conn_top_on_off();
        } else {
            /* CONN TOP ON MTCMOS ON */
            spm_mtcmos_conn_top_on_on();
        }
        return;
    }

    if (mtcmos == SPM_MTCMOS_CONN_TOP_OFF) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            /* CONN TOP OFF MTCMOS OFF */
            spm_mtcmos_conn_top_off_off();
        } else {
            /* CONN TOP OFF MTCMOS ON */
            spm_mtcmos_conn_top_off_on();
        }
        return;
    }

    if (mtcmos == SPM_MTCMOS_AUDIO) {

        audio_clock_old_status = hal_clock_is_enabled(HAL_CLOCK_CG_AUD_INTBUS);
        if (audio_clock_old_status == false) {
            hal_clock_enable(HAL_CLOCK_CG_AUD_INTBUS);
        }

        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            /* AUDIO MTCMOS OFF */
            spm_mtcmos_audio_off();
        } else {
            /* AUDIO MTCMOS ON */
            spm_mtcmos_audio_on();
        }

        if (audio_clock_old_status == false) {
            hal_clock_disable(HAL_CLOCK_CG_AUD_INTBUS);
        }
        return;
    }

    if (mtcmos == SPM_MTCMOS_DSP) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            /* DSP MTCMOS OFF */
            spm_mtcmos_dsp_off();
        } else {
            /* DSP MTCMOS ON */
            spm_mtcmos_dsp_on();
        }
        return;
    }

    if (mtcmos == SPM_MTCMOS_PERISYS) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            /* PERISYS MTCMOS OFF */
            spm_mtcmos_perisys_off();
        } else {
            /* PERISYS MTCMOS ON */
            spm_mtcmos_perisys_on();
        }
        return;
    }

    if (mtcmos == SPM_MTCMOS_VOW) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            /* SPM_MTCMOS_VOW MTCMOS OFF */
            spm_mtcmos_vow_off();
        } else {
            /* SPM_MTCMOS_VOW MTCMOS ON */
            spm_mtcmos_vow_on();
        }
        return;
    }
}

uint32_t spm_control_mtcmos(spm_mtcmos_type_t mtcmos, spm_mtcmos_ctrl_t ctrl)
{
    if (mtcmos == SPM_MTCMOS_CONN_TOP_ON) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            if (mtcmos_resource.conn_top_on == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_CONN_TOP_ON, SPM_MTCMOS_PWR_DISABLE);
                return 0;
            }
            mtcmos_resource.conn_top_on--;
            if (mtcmos_resource.conn_top_on == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_CONN_TOP_ON, SPM_MTCMOS_PWR_DISABLE);
            }
        } else {
            mtcmos_resource.conn_top_on++;
            if (mtcmos_resource.conn_top_on == 1) {
                spm_control_mtcmos_internal(SPM_MTCMOS_CONN_TOP_ON, SPM_MTCMOS_PWR_ENABLE);
            }
        }
        return (mtcmos_resource.conn_top_on);
    }

    if (mtcmos == SPM_MTCMOS_CONN_TOP_OFF) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            if (mtcmos_resource.conn_top_off == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_CONN_TOP_OFF, SPM_MTCMOS_PWR_DISABLE);
                return 0;
            }
            mtcmos_resource.conn_top_off--;
            if (mtcmos_resource.conn_top_off == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_CONN_TOP_OFF, SPM_MTCMOS_PWR_DISABLE);
            }
        } else {
            mtcmos_resource.conn_top_off++;
            if (mtcmos_resource.conn_top_off == 1) {
                spm_control_mtcmos_internal(SPM_MTCMOS_CONN_TOP_OFF, SPM_MTCMOS_PWR_ENABLE);
            }
        }
        return (mtcmos_resource.conn_top_off);
    }

    if (mtcmos == SPM_MTCMOS_AUDIO) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            if (mtcmos_resource.audio == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_AUDIO, SPM_MTCMOS_PWR_DISABLE);
                return 0;
            }
            mtcmos_resource.audio--;
            if (mtcmos_resource.audio == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_AUDIO, SPM_MTCMOS_PWR_DISABLE);
            }
        } else {
            mtcmos_resource.audio++;
            if (mtcmos_resource.audio == 1) {
                spm_control_mtcmos_internal(SPM_MTCMOS_AUDIO, SPM_MTCMOS_PWR_ENABLE);
            }
        }
        return (mtcmos_resource.audio);
    }

    if (mtcmos == SPM_MTCMOS_DSP) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            if (mtcmos_resource.dsp == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_DSP, SPM_MTCMOS_PWR_DISABLE);
                return 0;
            }
            mtcmos_resource.dsp--;
            if (mtcmos_resource.dsp == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_DSP, SPM_MTCMOS_PWR_DISABLE);
            }
        } else {
            mtcmos_resource.dsp++;
            if (mtcmos_resource.dsp == 1) {
                spm_control_mtcmos_internal(SPM_MTCMOS_DSP, SPM_MTCMOS_PWR_ENABLE);
            }
        }
        return (mtcmos_resource.dsp);
    }

    if (mtcmos == SPM_MTCMOS_PERISYS) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            if (mtcmos_resource.perisys == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_PERISYS, SPM_MTCMOS_PWR_DISABLE);
                return 0;
            }
            mtcmos_resource.perisys--;
            if (mtcmos_resource.perisys == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_PERISYS, SPM_MTCMOS_PWR_DISABLE);
            }
        } else {
            mtcmos_resource.perisys++;
            if (mtcmos_resource.perisys == 1) {
                spm_control_mtcmos_internal(SPM_MTCMOS_PERISYS, SPM_MTCMOS_PWR_ENABLE);
            }
        }
        return (mtcmos_resource.perisys);
    }

    if (mtcmos == SPM_MTCMOS_VOW) {
        if (ctrl == SPM_MTCMOS_PWR_DISABLE) {
            if (mtcmos_resource.vow == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_VOW, SPM_MTCMOS_PWR_DISABLE);
                return 0;
            }
            mtcmos_resource.vow--;
            if (mtcmos_resource.vow == 0) {
                spm_control_mtcmos_internal(SPM_MTCMOS_VOW, SPM_MTCMOS_PWR_DISABLE);
            }
        } else {
            mtcmos_resource.vow++;
            if (mtcmos_resource.vow == 1) {
                spm_control_mtcmos_internal(SPM_MTCMOS_VOW, SPM_MTCMOS_PWR_ENABLE);
            }
        }
        return (mtcmos_resource.vow);
    }
    return 0;
}

void spm_mask_wakeup_source(uint32_t wakeup_source)
{
    if (wakeup_source == HAL_SLEEP_MANAGER_WAKEUP_SOURCE_ALL) {
        *SPM_CM4_WAKEUP_SOURCE_MASK = 0xFF;
    } else {
        *SPM_CM4_WAKEUP_SOURCE_MASK |= (1 << wakeup_source);
    }
}

void spm_unmask_wakeup_source(uint32_t wakeup_source)
{
    if (wakeup_source == HAL_SLEEP_MANAGER_WAKEUP_SOURCE_ALL) {
        *SPM_CM4_WAKEUP_SOURCE_MASK = 0x00;
    } else {
        *SPM_CM4_WAKEUP_SOURCE_MASK &= ~(1 << wakeup_source);
    }
}

void spm_audio_lowpower_setting(spm_sleep_state_t sleep_state, spm_ctrl_t enable)
{
    if (sleep_state == SPM_STATE1) {
        if (enable == SPM_ENABLE) {
            spm_force_sleep_state(SPM_AUDIO_REQUEST, SPM_STATE1, SPM_ENABLE);
            /* CM4 notify DSP0 lock sleep , if CM4 have any audio request */
            *SPM_PCM_RESERVE2 |= 0x80000000; /* use bit31 */
        } else {
            spm_force_sleep_state(SPM_AUDIO_REQUEST, SPM_STATE1, SPM_DISABLE);
            /* CM4 notify DSP0 unlock sleep , if CM4 have not any audio request */
            *SPM_PCM_RESERVE2 &= 0x7FFFFFFF; /* use bit31 */
        }
        return;
    }

    if (sleep_state == SPM_STATE4) {
        if (enable == SPM_ENABLE) {
            spm_force_sleep_state(SPM_AUDIO_REQUEST, SPM_STATE4, SPM_ENABLE);
        } else {
            spm_force_sleep_state(SPM_AUDIO_REQUEST, SPM_STATE4, SPM_DISABLE);
        }
        return;
    }
}

void spm_force_sleep_state(spm_request_t user, spm_sleep_state_t state, spm_ctrl_t enable)
{
    uint32_t mask;
    static uint8_t resource_control[SPM_STATE_MAX];

   log_hal_msgid_info("[spm_debug]user=0x%x,state=0x%x,enable=0x%x,resource_control[state]=0x%x\r\n",4,user,state,enable,resource_control[state]);
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (enable == SPM_ENABLE) {
        resource_control[state] |= (1 << user);
    } else {
        resource_control[state] &= ~(1 << user);
    }

    if(state == SPM_STATE1) {
        if (resource_control[state] == 0) {
            *SPM_SPM_STATE_CONTROL_0 &= 0xFFFFFFFE;
        } else {
            *SPM_SPM_STATE_CONTROL_0 |= 0x1;
        }
    }

    if(state == SPM_STATE3) {
        if (resource_control[state] == 0) {
            *SPM_SPM_STATE_CONTROL_0 &= 0xFFFEFFFF;
        } else {
            *SPM_SPM_STATE_CONTROL_0 |= 0x10000;
        }
    }

    if(state == SPM_STATE4) {
        if (resource_control[state] == 0) {
            *SPM_SPM_STATE_CONTROL_0 &= 0xFEFFFFFF;
        } else {
            *SPM_SPM_STATE_CONTROL_0 |= 0x1000000;
        }
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void spm_force_on_pmic(spm_request_t spm_request,spm_ctrl_t enable)
{
    static uint8_t resource_control = 0;
    log_hal_msgid_info("[spm_debug]spm_request=0x%x,enable=0x%x,resource_control=0x%x\r\n",3,spm_request,enable,resource_control);

    if(enable == SPM_ENABLE) {
        resource_control |= (1 << spm_request);
    }else{
        resource_control &= ~(1 << spm_request);
    }

    if(resource_control == 0){
        *SPM_SEQUENCER_32K_REG_0 &= 0xFFFFFEFF;       /* Unforce PMIC On */
    }else{
        *SPM_SEQUENCER_32K_REG_0 |= 0x100;            /* Force PMIC On */
    }
}

void spm_debug_io(unsigned int debug_bus)
{
    log_hal_msgid_info("spm_debug_io\r\n",0);
    //Set GPIO
#if 0
    hal_pinmux_set_function(HAL_GPIO_4, 10);
    hal_pinmux_set_function(HAL_GPIO_5,10);
    hal_pinmux_set_function(HAL_GPIO_6, 10);
    hal_pinmux_set_function(HAL_GPIO_23, 10);

    hal_pinmux_set_function(HAL_GPIO_8, 10);
    hal_pinmux_set_function(HAL_GPIO_9, 10);
    hal_pinmux_set_function(HAL_GPIO_10, 10);
    hal_pinmux_set_function(HAL_GPIO_11, 10);
#endif

    //*TOP_DEBUG(0xA2010008) = 0x14;
    *(volatile uint32_t *)(0xA2010008) = 0x14;
    //*INFRA_CFG_DBGMON__AO_DBGMON_SEL(0xA2200030)  = 0x0
    *(volatile uint32_t *)(0xA2200030) = 0x0;
    //*SPM_DEBUG_CON(0xA21103D4)=0x1;
    *(volatile uint32_t *)(0xA21103D4) = 0x1;
    //0xA21103D0
    *SPM_SPM_DEBUG_SELECT = debug_bus;

    log_hal_msgid_info("SPM_SPM_DEBUG_SELECT=0x%x\r\n\r\n",1, *SPM_SPM_DEBUG_SELECT);
}

/****************************************************************************************************************************/
//1568 SPM FPGA DVT
void spm_check_debug_bus()
{
    spm_debug_io(0x43424342);

    unsigned int i = 0, delay = 0;

    while (1) {
        *SPM_STATE_STATUS = i;
        for (delay = 0; delay < 512; delay++);
        if (i++ >= 512) {
            i = 0;
        }
    }
}

void spm_dvt_test_case_mtcmos()
{
    spm_debug_io(0x45444342);//check MTCMOS status
#if 1
    while (1) {
        *SPM_STATE_STATUS = 1;
        hal_gpt_delay_us(100);

        *SPM_STATE_STATUS = 2;
        spm_control_mtcmos_internal(SPM_MTCMOS_CONN_TOP_ON, SPM_MTCMOS_PWR_DISABLE);
        hal_gpt_delay_us(100);
        *SPM_STATE_STATUS = 3;
        spm_control_mtcmos_internal(SPM_MTCMOS_CONN_TOP_ON, SPM_MTCMOS_PWR_ENABLE);
        hal_gpt_delay_us(100);

        *SPM_STATE_STATUS = 4;
        spm_control_mtcmos_internal(SPM_MTCMOS_CONN_TOP_OFF, SPM_MTCMOS_PWR_DISABLE);
        hal_gpt_delay_us(100);
        *SPM_STATE_STATUS = 5;
        spm_control_mtcmos_internal(SPM_MTCMOS_CONN_TOP_OFF, SPM_MTCMOS_PWR_ENABLE);
        hal_gpt_delay_us(100);

        *SPM_STATE_STATUS = 6;
        spm_control_mtcmos_internal(SPM_MTCMOS_AUDIO, SPM_MTCMOS_PWR_DISABLE);
        hal_gpt_delay_us(100);
        *SPM_STATE_STATUS = 7;
        spm_control_mtcmos_internal(SPM_MTCMOS_AUDIO, SPM_MTCMOS_PWR_ENABLE);

        *SPM_STATE_STATUS = 8;
        spm_control_mtcmos_internal(SPM_MTCMOS_DSP, SPM_MTCMOS_PWR_DISABLE);
        hal_gpt_delay_us(100);
        *SPM_STATE_STATUS = 9;
        spm_control_mtcmos_internal(SPM_MTCMOS_DSP, SPM_MTCMOS_PWR_ENABLE);
        hal_gpt_delay_us(100);

        *SPM_STATE_STATUS = 0xA;
        spm_control_mtcmos_internal(SPM_MTCMOS_PERISYS, SPM_MTCMOS_PWR_DISABLE);
        hal_gpt_delay_us(100);
        *SPM_STATE_STATUS = 0xB;
        spm_control_mtcmos_internal(SPM_MTCMOS_PERISYS, SPM_MTCMOS_PWR_ENABLE);
        hal_gpt_delay_us(100);

        *SPM_STATE_STATUS = 0xC;
        spm_control_mtcmos_internal(SPM_MTCMOS_VOW, SPM_MTCMOS_PWR_DISABLE);
        hal_gpt_delay_us(100);
        *SPM_STATE_STATUS = 0xD;
        spm_control_mtcmos_internal(SPM_MTCMOS_VOW, SPM_MTCMOS_PWR_ENABLE);
        hal_gpt_delay_us(100);
    }
#endif
}

void spm_dvt_test_case_deep_sleep()
{
    uint32_t mask;
    spm_debug_io(0x5C434242);

    int i;

    for (i = 0; i < 32; i++) {

        if (hal_nvic_get_pending_irq(i) == 1) {
            log_hal_msgid_info("pending_irq:%d\r\n", 1, i);
            hal_nvic_clear_pending_irq(i);
        }
    }

    hal_nvic_disable_irq(12);
    hal_nvic_disable_irq(21);

    log_hal_msgid_info("start test\r\n",0);

    //*SPM_IGNORE_CPU_ACTIVE |= 0x100;    //IGNORE DSP ACTIVE
    while (1) {
        hal_sleep_manager_set_sleep_time(100);

        hal_nvic_clear_pending_irq(20);
        *SPM_CONN_AUDIO_ABB_XO_SIDEBAND = 0;        //for sleep test, need BT or Audio Control.
        hal_nvic_save_and_set_interrupt_mask(&mask);
        sleep_management_enter_deep_sleep(0);
        hal_nvic_restore_interrupt_mask(mask);

        *SPM_CONN_AUDIO_ABB_XO_SIDEBAND = 0x101;    //for sleep test, need BT or Audio Control.
    }
}

int spm_dvt_test_case_select_value = 2;
void spm_dvt_test_case_select()
{
    while (1) {
        if (spm_dvt_test_case_select_value == 0) {
            spm_check_debug_bus();
        }
        if (spm_dvt_test_case_select_value == 1) {
            spm_dvt_test_case_mtcmos();
        }
        if (spm_dvt_test_case_select_value == 2) {
            //*SPM_SKIP_SFC_EMI_TASK |= 1;
            //*SPM_SKIP_SFC_EMI_TASK |= (1 << 8);
            *SPM_CM4_WAKEUP_SOURCE_MASK = 0xFFFFFFFE; //Only unmask GPT wakeup_source
            spm_dvt_test_case_deep_sleep();
        }
        /*
        if(spm_dvt_test_case_select_value == 3){
            *SPM_SKIP_SFC_EMI_TASK |= 1;
            *SPM_DSP_WAKEUP_SOURCE_MASK = 0xFFFFFFFE; //Only unmask GPT wakeup_source
             spm_dvt_test_case_deep_sleep_wi_CPU_off_but_UART_is_working();
        }*/

    }
}
/****************************************************************************************************************************/

void spm_mtcmos_audio_on()
{
    /*TINFO = " --- AUDIO power on by CM4 API "*/
    //*SPM_AUDIO_PWR_CON = 0x12; sleep
    *SPM_AUDIO_PWR_CON = 0x16; // [2]: pwr_on = 1
    hal_gpt_delay_us(1);
    *SPM_AUDIO_PWR_CON = 0x1E; // [3]: pwr_on_2nd = 1
    hal_gpt_delay_us(1); // 1us

    // --- AUDIO SRAM begin ------
    *SPM_AUDIO_AFE_MEM_IF_CONTROL_1 = 0x00000301;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us

    *SPM_AUDIO_AFE_MEM_IF_CONTROL_1 = 0x00000300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN

    *SPM_AUDIO_ASRC1_CONTROL_1      = 0x00000300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    *SPM_AUDIO_ASRC2_CONTROL_1      = 0x00000300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN

    *SPM_AUDIO_G_CONTROL        = 0x00000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN

    *SPM_AUDIO_DFX_CH01_CONTROL     = 0x00000301;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us

    *SPM_AUDIO_DFX_CH01_CONTROL     = 0x00000300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN

    hal_gpt_delay_us(1); // 1us

    *SPM_AUDIO_DFX_CH23_CONTROL     = 0x00000301;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us

    *SPM_AUDIO_DFX_CH23_CONTROL     = 0x00000300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN

    hal_gpt_delay_us(1); // 1us

    *SPM_AUDIO_DFX_CH45_CONTROL     = 0x00000301;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us

    *SPM_AUDIO_DFX_CH45_CONTROL     = 0x00000300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN

    *SPM_AUDIO_UL1_CONTROL      = 0x00000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_UL2_CONTROL      = 0x00000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_UL3_CONTROL      = 0x00000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_UL4_CONTROL      = 0x00000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_DL_CONTROL       = 0x00000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN

    // --- AUDIO SRAM end ------
    *SPM_AUDIO_PWR_CON = 0xE;   // [4]: clk_dis = 0
    hal_gpt_delay_us(1); // 1us

    *SPM_AUDIO_PWR_CON = 0x1E;  // [4]: clk_dis = 1
    hal_gpt_delay_us(1); // 1us

    *SPM_AUDIO_PWR_CON = 0x1C;  // [4]: clk_dis = 1, iso = 0
    hal_gpt_delay_us(1); // 1us

    *SPM_AUDIO_PWR_CON = 0x1D;  // [0]: rstb = 1
    hal_gpt_delay_us(1); // 1us

    // --- AUDIO SRAM MEM_ISO_EN_B begin ------
    *SPM_AUDIO_AFE_MEM_IF_CONTROL_1 = 0x01000300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us

    *SPM_AUDIO_ASRC1_CONTROL_1      = 0x01000300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    *SPM_AUDIO_ASRC2_CONTROL_1      = 0x01000300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN

    *SPM_AUDIO_G_CONTROL        = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_DFX_CH01_CONTROL     = 0x01000300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    *SPM_AUDIO_DFX_CH23_CONTROL     = 0x01000300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    *SPM_AUDIO_DFX_CH45_CONTROL     = 0x01000300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    *SPM_AUDIO_UL1_CONTROL      = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_UL2_CONTROL      = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_UL3_CONTROL      = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_UL4_CONTROL      = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_DL_CONTROL       = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    // --- AUDIO SRAM end ------

    *SPM_AUDIO_PWR_CON = 0xD;   // [4]: clk_dis = 0
    hal_gpt_delay_us(1); // 1us

    //turn AUDIO protect_en=0
    *INFRA_CFG_MTCMOS3 = 0x0;
    //wait AUDIO protect ready=0
    while (*INFRA_CFG_MTCMOS3 != 0x0);
}

void spm_mtcmos_audio_off(void)
{
    //turn AUDIO protect_en=1
    *INFRA_CFG_MTCMOS3 = 0x1;
    //wait AUDIO protect ready=1
    while (*INFRA_CFG_MTCMOS3 != 0x101);

    /*TINFO = " --- AUDIO power down by CM4 API "*/
    *SPM_AUDIO_PWR_CON = 0x1D; // [4]: clk_dis = 1
    hal_gpt_delay_us(1); // 1us

    *SPM_AUDIO_PWR_CON = 0x1F; // [1]: iso = 1
    hal_gpt_delay_us(1); // 1us

    //// --- AUDIO SRAM Power Down begin ------
    *SPM_AUDIO_AFE_MEM_IF_CONTROL_1 = 0x00030300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us

    //switch ret2n/ret1n first
    *SPM_AUDIO_ASRC1_CONTROL_1      = 0x00030300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    *SPM_AUDIO_ASRC2_CONTROL_1      = 0x00030300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN

    *SPM_AUDIO_G_CONTROL        = 0x00010100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_DFX_CH01_CONTROL     = 0x00030300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    *SPM_AUDIO_DFX_CH23_CONTROL     = 0x00030300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    *SPM_AUDIO_DFX_CH45_CONTROL     = 0x00030300;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    *SPM_AUDIO_UL1_CONTROL      = 0x00010100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_UL2_CONTROL      = 0x00010100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_UL3_CONTROL      = 0x00010100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_UL4_CONTROL      = 0x00010100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_DL_CONTROL       = 0x00010100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN

    //switch pgen
    *SPM_AUDIO_AFE_MEM_IF_CONTROL_1 = 0x00030301;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us
    *SPM_AUDIO_AFE_MEM_IF_CONTROL_1 = 0x00030303;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us
    *SPM_AUDIO_ASRC1_CONTROL_1      = 0x00030301;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us
    *SPM_AUDIO_ASRC1_CONTROL_1      = 0x00030303;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    *SPM_AUDIO_ASRC2_CONTROL_1      = 0x00030301;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us
    *SPM_AUDIO_ASRC2_CONTROL_1      = 0x00030303;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    *SPM_AUDIO_G_CONTROL        = 0x00010101;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_DFX_CH01_CONTROL     = 0x00030301;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us
    *SPM_AUDIO_DFX_CH01_CONTROL     = 0x00030303;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us
    *SPM_AUDIO_DFX_CH23_CONTROL     = 0x00030301;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us
    *SPM_AUDIO_DFX_CH23_CONTROL     = 0x00030303;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us
    *SPM_AUDIO_DFX_CH45_CONTROL     = 0x00030301;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN
    hal_gpt_delay_us(1); // 1us
    *SPM_AUDIO_DFX_CH45_CONTROL     = 0x00030303;//[24]:MEM_ISO_EN_B [17:16]:RET2N [9:8]:RET1N [1:0]:PGEN

    *SPM_AUDIO_UL1_CONTROL      = 0x00010101;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_UL2_CONTROL      = 0x00010101;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_UL3_CONTROL      = 0x00010101;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_UL4_CONTROL      = 0x00010101;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_AUDIO_DL_CONTROL       = 0x00010101;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    //// --- AUDIO SRAM end Power Down------

    *SPM_AUDIO_PWR_CON = 0x1E; // [0]: rstb = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_AUDIO_PWR_CON = 0x1A; // [2]: pwr_on = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_AUDIO_PWR_CON = 0x12; // [3]: pwr_on_2nd = 0
    hal_gpt_delay_us(1); // 1us
}

void spm_mtcmos_conn_top_on_on(void)
{
    /*TINFO = " --- CONN_TOP_ON power on API "*/
    //*SPM_CONN_TOP_ON_PWR_CON = 0x12; sleep
    *SPM_CONN_TOP_ON_PWR_CON = 0x16; // [2]: pwr_on = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_ON_PWR_CON = 0x1E; // [3]: pwr_on_2nd = 1
    hal_gpt_delay_us(1); // 1us

    *SPM_CONN_TOP_ON_PWR_CON = 0xE;   // [4]: clk_dis = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_ON_PWR_CON = 0x1E;  // [4]: clk_dis = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_ON_PWR_CON = 0x1C;  // [4]: clk_dis = 1, iso = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_ON_PWR_CON = 0x1D;  // [0]: rstb = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_ON_PWR_CON = 0xD;   // [4]: clk_dis = 0
    hal_gpt_delay_us(1); // 1us

    //turn CONN_TOP_OFF protect_en=0
    *INFRA_CFG_MTCMOS2 = 0x0;
    //wait CONN_TOP_OFF protect ready=0
    while (*INFRA_CFG_MTCMOS2 != 0x0);
}

void spm_mtcmos_conn_top_on_off(void)
{
    //turn CONN_TOP_OFF protect_en=1
    *INFRA_CFG_MTCMOS2 = 0x1;
    //wait CONN_TOP_OFF protect ready=1
    while (*INFRA_CFG_MTCMOS2 != 0x101);

    /*TINFO = " --- CONN_TOP_ON power down API "*/
    *SPM_CONN_TOP_ON_PWR_CON = 0x1D; // [4]: clk_dis = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_ON_PWR_CON = 0x1F; // [1]: iso = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_ON_PWR_CON = 0x1E; // [0]: rstb = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_ON_PWR_CON = 0x1A; // [2]: pwr_on = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_ON_PWR_CON = 0x12; // [3]: pwr_on_2nd = 0
    hal_gpt_delay_us(1); // 1us
}

void spm_mtcmos_conn_top_off_on(void)
{
    //=======WK start
    //turn CONN_TOP_OFF protect_en=1
    *INFRA_CFG_MTCMOS2 = 0x1;
    //wait CONN_TOP_OFF protect ready=1
    while (*INFRA_CFG_MTCMOS2 != 0x101);
    //=======WK end

    /*TINFO = " --- CONN_TOP_OFF power on "*/
    //*SPM_CONN_TOP_OFF_PWR_CON = 0x12; sleep
    *SPM_CONN_TOP_OFF_PWR_CON = 0x16; // [2]: pwr_on = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_OFF_PWR_CON = 0x1E; // [3]: pwr_on_2nd = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_OFF_PWR_CON = 0xE;   // [4]: clk_dis = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_OFF_PWR_CON = 0x1E;  // [4]: clk_dis = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_OFF_PWR_CON = 0x1C;  // [4]: clk_dis = 1, iso = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_OFF_PWR_CON = 0x1D;  // [0]: rstb = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_OFF_PWR_CON = 0xD;   // [4]: clk_dis = 0
    hal_gpt_delay_us(1); // 1us

    //turn CONN_TOP_OFF protect_en=0
    *INFRA_CFG_MTCMOS2 = 0x0;
    //wait CONN_TOP_OFF protect ready=0
    while (*INFRA_CFG_MTCMOS2 != 0x0);
}

void spm_mtcmos_conn_top_off_off(void)
{
    //turn CONN_TOP_OFF protect_en=1
    *INFRA_CFG_MTCMOS2 = 0x1;
    //wait CONN_TOP_OFF protect ready=1
    while (*INFRA_CFG_MTCMOS2 != 0x101);

    /*TINFO = " --- CONN_TOP_OFF power down by API "*/
    *SPM_CONN_TOP_OFF_PWR_CON = 0x1D; // [4]: clk_dis = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_OFF_PWR_CON = 0x1F; // [1]: iso = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_OFF_PWR_CON = 0x1E; // [0]: rstb = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_OFF_PWR_CON = 0x1A; // [2]: pwr_on = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_CONN_TOP_OFF_PWR_CON = 0x12; // [3]: pwr_on_2nd = 0
    hal_gpt_delay_us(1); // 1us

    //=======WK start
    //turn CONN_TOP_OFF protect_en=0
    *INFRA_CFG_MTCMOS2 = 0x0;
    //wait CONN_TOP_OFF protect ready=0
    while (*INFRA_CFG_MTCMOS2 != 0x0);
    //=======WK end
}

void spm_mtcmos_dsp_on(void)
{
    /*TINFO = " --- DSP power on by DSP API "*/
    //*SPM_DSP_PWR_CON = 0x12; sleep
    *SPM_DSP_PWR_CON = 0x16; // [2]: pwr_on = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_DSP_PWR_CON = 0x1E; // [3]: pwr_on_2nd = 1
    hal_gpt_delay_us(1); // 1us

    // --- DSP SRAM begin ------
    *SPM_DSP_DRAM0H_CONTROL_1     = 0x00000000;
    *SPM_DSP_DRAM0L_CONTROL_1     = 0x00000000;
    *SPM_DSP_DRAM1H_CONTROL_1     = 0x00000000;
    *SPM_DSP_DRAM1L_CONTROL_1     = 0x00000000;
    *SPM_DSP_IRAM0H_CONTROL_1     = 0x00000000;
    *SPM_DSP_IRAM0L_CONTROL_1     = 0x00000000;
    *SPM_DSP_IRAM1_CONTROL_1      = 0x00000000;

    *SPM_DSP_D_TAG_CACHE_CONTROL_1  = 0x000000F7;
    *SPM_DSP_D_TAG_CACHE_CONTROL_1  = 0x000000F3;
    *SPM_DSP_D_TAG_CACHE_CONTROL_1  = 0x000000F1;
    *SPM_DSP_D_TAG_CACHE_CONTROL_1  = 0x000000F0;
    *SPM_DSP_D_TAG_CACHE_CONTROL_1  = 0x00000070;
    *SPM_DSP_D_TAG_CACHE_CONTROL_1  = 0x00000030;
    *SPM_DSP_D_TAG_CACHE_CONTROL_1  = 0x00000010;
    *SPM_DSP_D_TAG_CACHE_CONTROL_1  = 0x00000000;

    *SPM_DSP_I_TAG_CACHE_CONTROL_1  = 0x000000F7;
    *SPM_DSP_I_TAG_CACHE_CONTROL_1  = 0x000000F3;
    *SPM_DSP_I_TAG_CACHE_CONTROL_1  = 0x000000F1;
    *SPM_DSP_I_TAG_CACHE_CONTROL_1  = 0x000000F0;
    *SPM_DSP_I_TAG_CACHE_CONTROL_1  = 0x00000070;
    *SPM_DSP_I_TAG_CACHE_CONTROL_1  = 0x00000030;
    *SPM_DSP_I_TAG_CACHE_CONTROL_1  = 0x00000010;
    *SPM_DSP_I_TAG_CACHE_CONTROL_1  = 0x00000000;
    // --- DSP SRAM end ------

    *SPM_DSP_PWR_CON = 0xE;   // [4]: clk_dis = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_DSP_PWR_CON = 0x1E;  // [4]: clk_dis = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_DSP_PWR_CON = 0x1C;  // [4]: clk_dis = 1, iso = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_DSP_PWR_CON = 0x1D;  // [0]: rstb = 1
    hal_gpt_delay_us(1); // 1us

    // --- DSP SRAM begin ------
    *SPM_DSP_DRAM0H_CONTROL_1     = 0x01000000;
    *SPM_DSP_DRAM0L_CONTROL_1     = 0x01000000;
    *SPM_DSP_DRAM1H_CONTROL_1     = 0x01000000;
    *SPM_DSP_DRAM1L_CONTROL_1     = 0x01000000;
    *SPM_DSP_IRAM0H_CONTROL_1     = 0x01000000;
    *SPM_DSP_IRAM0L_CONTROL_1     = 0x01000000;
    *SPM_DSP_IRAM1_CONTROL_1      = 0x01000000;
    *SPM_DSP_D_TAG_CACHE_CONTROL_1  = 0x01000000;
    *SPM_DSP_I_TAG_CACHE_CONTROL_1  = 0x01000000;

    // --- DSP SRAM end ------
    *SPM_DSP_PWR_CON = 0xD;   // [4]: clk_dis = 0
    hal_gpt_delay_us(1); // 1us

    //turn DSP protect_en=0
    *INFRA_CFG_MTCMOS1 = 0x0;
    //wait DSP protect ready=0
    while (*INFRA_CFG_MTCMOS1 != 0x0);

}

void spm_mtcmos_dsp_off(void)
{
    //turn DSP protect_en=1
    *INFRA_CFG_MTCMOS1 = 0x1;
    //wait DSP protect ready=1
    while (*INFRA_CFG_MTCMOS1 != 0x101);

    /*TINFO = " --- DSP power down by DSP API "*/
    *SPM_DSP_PWR_CON = 0x1D; // [4]: clk_dis = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_DSP_PWR_CON = 0x1F; // [1]: iso = 1
    hal_gpt_delay_us(1); // 1us


    //// --- DSP SRAM Power Down begin ------
    *SPM_DSP_DRAM0H_CONTROL_1     = 0x00000000;
    *SPM_DSP_DRAM0H_CONTROL_1     = 0x000000FF;
    *SPM_DSP_DRAM0L_CONTROL_1     = 0x00000000;
    *SPM_DSP_DRAM0L_CONTROL_1     = 0x000000FF;
    *SPM_DSP_DRAM1H_CONTROL_1     = 0x00000000;
    *SPM_DSP_DRAM1H_CONTROL_1     = 0x000000FF;
    *SPM_DSP_DRAM1L_CONTROL_1     = 0x00000000;
    *SPM_DSP_DRAM1L_CONTROL_1     = 0x000000FF;
    *SPM_DSP_IRAM0H_CONTROL_1     = 0x00000000;
    *SPM_DSP_IRAM0H_CONTROL_1     = 0x000000FF;
    *SPM_DSP_IRAM0L_CONTROL_1     = 0x00000000;
    *SPM_DSP_IRAM0L_CONTROL_1     = 0x000000FF;
    *SPM_DSP_IRAM1_CONTROL_1      = 0x00000000;
    *SPM_DSP_IRAM1_CONTROL_1      = 0x000000FF;

    *SPM_DSP_D_TAG_CACHE_CONTROL_1  = 0x00000000;
    *SPM_DSP_D_TAG_CACHE_CONTROL_1  = 0x00000001;
    *SPM_DSP_D_TAG_CACHE_CONTROL_1  = 0x00000003;
    *SPM_DSP_D_TAG_CACHE_CONTROL_1  = 0x00000007;
    *SPM_DSP_D_TAG_CACHE_CONTROL_1  = 0x0000000F;
    *SPM_DSP_I_TAG_CACHE_CONTROL_1  = 0x00000000;
    *SPM_DSP_I_TAG_CACHE_CONTROL_1  = 0x00000001;
    *SPM_DSP_I_TAG_CACHE_CONTROL_1  = 0x00000003;
    *SPM_DSP_I_TAG_CACHE_CONTROL_1  = 0x00000007;
    *SPM_DSP_I_TAG_CACHE_CONTROL_1  = 0x0000000F;
    //// --- DSP SRAM end Power Down------

    *SPM_DSP_PWR_CON = 0x1E; // [0]: rstb = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_DSP_PWR_CON = 0x1A; // [2]: pwr_on = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_DSP_PWR_CON = 0x12; // [3]: pwr_on_2nd = 0
    hal_gpt_delay_us(1); // 1us
}

void spm_mtcmos_perisys_on(void)
{
    /*TINFO = " --- PERISYS power on by PERISYS API "*/
    //*SPM_PERISYS_PWR_CON = 0x12; sleep
    *SPM_PERISYS_PWR_CON = 0x16; // [2]: pwr_on = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_PERISYS_PWR_CON = 0x1E; // [3]: pwr_on_2nd = 1
    hal_gpt_delay_us(1); // 1us

    // --- PERISYS SRAM begin ------
    *SPM_USB_SRAM_CONTROL = 0x00000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    // --- PERISYS SRAM end ------

    *SPM_PERISYS_PWR_CON = 0xE;   // [4]: clk_dis = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_PERISYS_PWR_CON = 0x1E;  // [4]: clk_dis = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_PERISYS_PWR_CON = 0x1C;  // [4]: clk_dis = 1, iso = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_PERISYS_PWR_CON = 0x1D;  // [0]: rstb = 1
    hal_gpt_delay_us(1); // 1us

    // --- PERISYS SRAM MEM_ISO_EN_B begin ------
    *SPM_USB_SRAM_CONTROL = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    // --- PERISYS SRAM end ------

    *SPM_PERISYS_PWR_CON = 0xD;   // [4]: clk_dis = 0
    hal_gpt_delay_us(1); // 1us

    //turn PERISYS protect_en=0
    *INFRA_CFG_MTCMOS4 = 0x0;
    //wait PERISYS protect ready=0
    while (*INFRA_CFG_MTCMOS4 != 0x0);

    //TODO : enable MSDC/USB clock
}
void spm_mtcmos_perisys_off(void)
{
    //TODO : disable MSDC/USB clock

    /*TINFO = " --- PERISYS power down by PERISYS API "*/
    *SPM_PERISYS_PWR_CON = 0x1D; // [4]: clk_dis = 1
    hal_gpt_delay_us(1); // 1us

    *SPM_PERISYS_PWR_CON = 0x1F; // [1]: iso = 1
    hal_gpt_delay_us(1); // 1us

    //// --- PERISYS SRAM Power Down begin ------
    *SPM_USB_SRAM_CONTROL = 0x00000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    *SPM_USB_SRAM_CONTROL = 0x00000101;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    //// --- PERISYS SRAM end Power Down------

    *SPM_PERISYS_PWR_CON = 0x1E; // [0]: rstb = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_PERISYS_PWR_CON = 0x1A; // [2]: pwr_on = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_PERISYS_PWR_CON = 0x12; // [3]: pwr_on_2nd = 0
    hal_gpt_delay_us(1); // 1us

    //turn PERISYS protect_en=1
    *INFRA_CFG_MTCMOS4 = 0x1;
    //wait PERISYS protect ready=1
    while (*INFRA_CFG_MTCMOS4 != 0x101);
}

void spm_mtcmos_vow_on(void)
{
    /*TINFO = " --- VOW power on by CM4 API "*/
    //*SPM_VOW_PWR_CON = 0x12; sleep
    *SPM_VOW_PWR_CON = 0x16; // [2]: pwr_on = 1
    hal_gpt_delay_us(1); // 1us

    *SPM_VOW_PWR_CON = 0x1E; // [3]: pwr_on_2nd = 1
    hal_gpt_delay_us(1); // 1us

    // --- VOW SRAM begin ------
    *SPM_VOW_SRAM_CONTROL_RET = 0x00000001; //[16]:RET2N [0]:RET1N
    *SPM_VOW_SRAM_CONTROL     = 0x00000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    // --- VOW SRAM end ------

    *SPM_VOW_PWR_CON = 0xE;   // [4]: clk_dis = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_VOW_PWR_CON = 0x1E;  // [4]: clk_dis = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_VOW_PWR_CON = 0x1C;  // [4]: clk_dis = 1, iso = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_VOW_PWR_CON = 0x1D;  // [0]: rstb = 1
    hal_gpt_delay_us(1); // 1us

    // --- VOW SRAM MEM_ISO_EN_B begin ------
    *SPM_VOW_SRAM_CONTROL = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    // --- VOW SRAM end ------
    *SPM_VOW_PWR_CON = 0xD;   // [4]: clk_dis = 0
    hal_gpt_delay_us(1); // 1us

    //turn VOW protect_en=0
    *INFRA_CFG_MTCMOS5 = 0x0;
    //wait VOW protect ready=0
    while (*INFRA_CFG_MTCMOS5 != 0x0);

}

void spm_mtcmos_vow_off(void)
{
    //turn VOW protect_en=1
    *INFRA_CFG_MTCMOS5 = 0x1;
    //wait VOW protect ready=1
    while (*INFRA_CFG_MTCMOS5 != 0x101);

    /*TINFO = " --- VOW power down by CM4 API "*/
    *SPM_VOW_PWR_CON = 0x1D; // [4]: clk_dis = 1
    hal_gpt_delay_us(1); // 1us

    *SPM_VOW_PWR_CON = 0x1F; // [1]: iso = 1
    hal_gpt_delay_us(1); // 1us

    //// --- VOW SRAM Power Down begin ------
    *SPM_VOW_SRAM_CONTROL_RET = 0x00000001; //[16]:RET2N [0]:RET1N
    *SPM_VOW_SRAM_CONTROL     = 0x00000101;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    //// --- VOW SRAM end Power Down------

    *SPM_VOW_PWR_CON = 0x1E; // [0]: rstb = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_VOW_PWR_CON = 0x1A; // [2]: pwr_on = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_VOW_PWR_CON = 0x12; // [3]: pwr_on_2nd = 0
    hal_gpt_delay_us(1); // 1us
}


#endif
