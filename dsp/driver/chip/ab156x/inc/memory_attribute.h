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
 
/*****************************************************
  Usage:
   1. function: (a or b)
      a. ATTR_TEXT_IN_TCM int func(int par);
      b. ATTR_TEXT_IN_TCM int func(int par)
        {
        }
   2. RO data:
      a. ATTR_RODATA_IN_TCM const int b = 8;
   3. RW data:
      a. ATTR_RWDATA_IN_TCM int b = 8;
   4. ZI data:
      a. ATTR_ZIDATA_IN_TCM int b;

  Note: must put these attributes at HEAD of declaration.
*****************************************************/

#ifndef MEMORY_ATTRIBUTE_H_
#define MEMORY_ATTRIBUTE_H_

#if defined(CORE_CM4)

#define ATTR_TEXT_IN_TCM              __attribute__ ((__section__(".tcm_code")))
#define ATTR_RODATA_IN_TCM            __attribute__ ((__section__(".tcm_rodata")))
#define ATTR_RWDATA_IN_TCM            __attribute__ ((__section__(".tcm_rwdata")))
#define ATTR_ZIDATA_IN_TCM            __attribute__ ((__section__(".tcm_zidata")))

#define ATTR_TEXT_IN_SYSRAM              __attribute__ ((__section__(".sysram_code")))
#define ATTR_RODATA_IN_SYSRAM            __attribute__ ((__section__(".sysram_rodata")))
#define ATTR_RWDATA_IN_CACHED_SYSRAM     __attribute__ ((__section__(".cached_sysram_rwdata")))
#define ATTR_ZIDATA_IN_CACHED_SYSRAM     __attribute__ ((__section__(".cached_sysram_zidata")))
#define ATTR_RWDATA_IN_NONCACHED_SYSRAM  __attribute__ ((__section__(".noncached_sysram_rwdata")))
#define ATTR_ZIDATA_IN_NONCACHED_SYSRAM  __attribute__ ((__section__(".noncached_sysram_zidata")))
#define ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN  __attribute__ ((__section__(".noncached_sysram_rwdata"),__aligned__(4)))
#define ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN  __attribute__ ((__section__(".noncached_sysram_zidata"),__aligned__(4)))
#define ATTR_RWDATA_IN_NONCACHED_SYSRAM_8BYTE_ALIGN  __attribute__ ((__section__(".noncached_sysram_rwdata"),__aligned__(8)))
#define ATTR_ZIDATA_IN_NONCACHED_SYSRAM_8BYTE_ALIGN  __attribute__ ((__section__(".noncached_sysram_zidata"),__aligned__(8)))
#define ATTR_RWDATA_IN_NONCACHED_SYSRAM_16BYTE_ALIGN  __attribute__ ((__section__(".noncached_sysram_rwdata"),__aligned__(16)))
#define ATTR_ZIDATA_IN_NONCACHED_SYSRAM_16BYTE_ALIGN  __attribute__ ((__section__(".noncached_sysram_zidata"),__aligned__(16)))

#define ATTR_SHARE_RWDATA              __attribute__ ((__section__(".share_rwdata")))
#define ATTR_SHARE_ZIDATA              __attribute__ ((__section__(".share_zidata")))

#endif


//#if defined(CORE_CM4) || defined(CORE_DSP0)
#ifdef CORE_CM4

#ifdef MT2811A
#define ATTR_TEXT_IN_RAM              __attribute__ ((__section__(".ram_code")))
#define ATTR_RODATA_IN_RAM            __attribute__ ((__section__(".ram_rodata")))
#define ATTR_RWDATA_IN_NONCACHED_RAM  __attribute__ ((__section__(".noncached_ram_rwdata")))
#define ATTR_ZIDATA_IN_NONCACHED_RAM  __attribute__ ((__section__(".noncached_ram_zidata")))

#define ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  __attribute__ ((__section__(".noncached_ram_rwdata"),__aligned__(4)))
#define ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  __attribute__ ((__section__(".noncached_ram_zidata"),__aligned__(4)))
#define ATTR_RWDATA_IN_NONCACHED_RAM_8BYTE_ALIGN  __attribute__ ((__section__(".noncached_ram_rwdata"),__aligned__(8)))
#define ATTR_ZIDATA_IN_NONCACHED_RAM_8BYTE_ALIGN  __attribute__ ((__section__(".noncached_ram_zidata"),__aligned__(8)))
#define ATTR_RWDATA_IN_NONCACHED_RAM_16BYTE_ALIGN  __attribute__ ((__section__(".noncached_ram_rwdata"),__aligned__(16)))
#define ATTR_ZIDATA_IN_NONCACHED_RAM_16BYTE_ALIGN  __attribute__ ((__section__(".noncached_ram_zidata"),__aligned__(16)))
#else
#define ATTR_TEXT_IN_RAM              ATTR_TEXT_IN_SYSRAM
#define ATTR_RODATA_IN_RAM            ATTR_RODATA_IN_SYSRAM
#define ATTR_RWDATA_IN_NONCACHED_RAM  ATTR_RWDATA_IN_NONCACHED_SYSRAM
#define ATTR_ZIDATA_IN_NONCACHED_RAM  ATTR_ZIDATA_IN_NONCACHED_SYSRAM

#define ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN
#define ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN  ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN
#define ATTR_RWDATA_IN_NONCACHED_RAM_8BYTE_ALIGN  ATTR_RWDATA_IN_NONCACHED_SYSRAM_8BYTE_ALIGN
#define ATTR_ZIDATA_IN_NONCACHED_RAM_8BYTE_ALIGN  ATTR_ZIDATA_IN_NONCACHED_SYSRAM_8BYTE_ALIGN
#define ATTR_RWDATA_IN_NONCACHED_RAM_16BYTE_ALIGN  ATTR_RWDATA_IN_NONCACHED_SYSRAM_16BYTE_ALIGN
#define ATTR_ZIDATA_IN_NONCACHED_RAM_16BYTE_ALIGN  ATTR_ZIDATA_IN_NONCACHED_SYSRAM_16BYTE_ALIGN
#endif

#endif

#if defined(CORE_DSP0) || defined(CORE_DSP1)
#define ATTR_TEXT_IN_IRAM              __attribute__ ((__section__(".iram")))
#define ATTR_RWDATA_IN_DRAM            __attribute__ ((__section__(".data")))
#define ATTR_ZIDATA_IN_DRAM            __attribute__ ((__section__(".bss")))
#define ATTR_RWDATA_IN_DRAM_4BYTE_ALIGN    __attribute__ ((__section__(".data"),__aligned__(4)))
#define ATTR_ZIDATA_IN_DRAM_4BYTE_ALIGN    __attribute__ ((__section__(".bss"),__aligned__(4)))
#define ATTR_RWDATA_IN_DRAM_8BYTE_ALIGN    __attribute__ ((__section__(".data"),__aligned__(8)))
#define ATTR_ZIDATA_IN_DRAM_8BYTE_ALIGN    __attribute__ ((__section__(".bss"),__aligned__(8)))
#define ATTR_RWDATA_IN_DRAM_16BYTE_ALIGN    __attribute__ ((__section__(".data"),__aligned__(16)))
#define ATTR_ZIDATA_IN_DRAM_16BYTE_ALIGN    __attribute__ ((__section__(".bss"),__aligned__(16)))

#ifndef MTK_LOWPOWER_LEVEL
#define MTK_LOWPOWER_LEVEL 0
#endif
#if (MTK_LOWPOWER_LEVEL == 0)
#define ATTR_TEXT_IN_IRAM_LEVEL_1
#define ATTR_TEXT_IN_IRAM_LEVEL_2
#elif (MTK_LOWPOWER_LEVEL == 1)
#define ATTR_TEXT_IN_IRAM_LEVEL_1   ATTR_TEXT_IN_IRAM
#define ATTR_TEXT_IN_IRAM_LEVEL_2
#elif (MTK_LOWPOWER_LEVEL == 2)
#define ATTR_TEXT_IN_IRAM_LEVEL_1   ATTR_TEXT_IN_IRAM
#define ATTR_TEXT_IN_IRAM_LEVEL_2   ATTR_TEXT_IN_IRAM
#else
#define ATTR_TEXT_IN_IRAM_LEVEL_1
#define ATTR_TEXT_IN_IRAM_LEVEL_2
#endif

#endif

#define ATTR_LOG_STRING               __attribute__ ((__section__(".log_string"))) static const char
#ifndef MTK_DEBUG_PLAIN_LOG_ENABLE
#define ATTR_LOG_STRING_LIB           __attribute__ ((__section__(".log_string"))) const char
#else
#define ATTR_LOG_STRING_LIB           const char
#endif
#define ATTR_LOG_FILTER(module)                         __attribute__ ((__section__(".log_filter." #module)))
#define ATTR_LOG_FILTER_MIRROR(module)                  __attribute__ ((__section__(".log_filter_mirror." #module))) __attribute__((used))
#define ATTR_LOG_VERSION                                __attribute__ ((__section__(".log_version"))) __attribute__((used)) static const char
#define ATTR_LOG_TIMESTAMP                              __attribute__ ((__section__(".log_timestamp"))) __attribute__((used)) static const char

#endif

