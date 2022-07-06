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

#define ATTR_ALIGN(alignment)                           __attribute__ ((__aligned__(alignment)))
#define ATTR_SECTION_W_LINE_1(sec, line)                __attribute__ ((__section__(sec #line)))
#define ATTR_SECTION_W_LINE(sec, line)                  ATTR_SECTION_W_LINE_1(sec, line)

#if defined(CORE_CM4)

#define ATTR_TEXT_IN_TCM                                ATTR_SECTION_W_LINE(".tcm_code.", __LINE__)
#define ATTR_RODATA_IN_TCM                              ATTR_SECTION_W_LINE(".tcm_rodata.", __LINE__)
#define ATTR_RWDATA_IN_TCM                              ATTR_SECTION_W_LINE(".tcm_rwdata.", __LINE__)
#define ATTR_ZIDATA_IN_TCM                              ATTR_SECTION_W_LINE(".tcm_zidata.", __LINE__)

#define ATTR_TEXT_IN_SYSRAM                             ATTR_SECTION_W_LINE(".sysram_code.", __LINE__)
#define ATTR_RODATA_IN_SYSRAM                           ATTR_SECTION_W_LINE(".sysram_rodata.", __LINE__)
#define ATTR_RWDATA_IN_CACHED_SYSRAM                    ATTR_SECTION_W_LINE(".cached_sysram_rwdata.", __LINE__)
#define ATTR_ZIDATA_IN_CACHED_SYSRAM                    ATTR_SECTION_W_LINE(".cached_sysram_zidata.", __LINE__)
#define ATTR_RWDATA_IN_NONCACHED_SYSRAM                 ATTR_SECTION_W_LINE(".noncached_sysram_rwdata.", __LINE__)
#define ATTR_ZIDATA_IN_NONCACHED_SYSRAM                 ATTR_SECTION_W_LINE(".noncached_sysram_zidata.", __LINE__)
#define ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN     ATTR_RWDATA_IN_NONCACHED_SYSRAM ATTR_ALIGN(4)
#define ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN     ATTR_ZIDATA_IN_NONCACHED_SYSRAM ATTR_ALIGN(4)
#define ATTR_RWDATA_IN_NONCACHED_SYSRAM_8BYTE_ALIGN     ATTR_RWDATA_IN_NONCACHED_SYSRAM ATTR_ALIGN(8)
#define ATTR_ZIDATA_IN_NONCACHED_SYSRAM_8BYTE_ALIGN     ATTR_ZIDATA_IN_NONCACHED_SYSRAM ATTR_ALIGN(8)
#define ATTR_RWDATA_IN_NONCACHED_SYSRAM_16BYTE_ALIGN    ATTR_RWDATA_IN_NONCACHED_SYSRAM ATTR_ALIGN(16)
#define ATTR_ZIDATA_IN_NONCACHED_SYSRAM_16BYTE_ALIGN    ATTR_ZIDATA_IN_NONCACHED_SYSRAM ATTR_ALIGN(16)

#define ATTR_SHARE_RWDATA                               ATTR_SECTION_W_LINE(".share_rwdata.", __LINE__)
#define ATTR_SHARE_ZIDATA                               ATTR_SECTION_W_LINE(".share_zidata.", __LINE__)

#define ATTR_SHARE_RWBT                                 ATTR_SECTION_W_LINE(".share_rwbt.", __LINE__)
#define ATTR_SHARE_ZIBT                                 ATTR_SECTION_W_LINE(".share_zibt.", __LINE__)
#define ATTR_SHARE_RWBT_4BYTE_ALIGN                     ATTR_SHARE_RWBT ATTR_ALIGN(4)
#define ATTR_SHARE_ZIBT_4BYTE_ALIGN                     ATTR_SHARE_ZIBT ATTR_ALIGN(4)
#define ATTR_SHARE_RWBT_8BYTE_ALIGN                     ATTR_SHARE_RWBT ATTR_ALIGN(8)
#define ATTR_SHARE_ZIBT_8BYTE_ALIGN                     ATTR_SHARE_ZIBT ATTR_ALIGN(8)
#define ATTR_SHARE_RWBT_16BYTE_ALIGN                    ATTR_SHARE_RWBT ATTR_ALIGN(16)
#define ATTR_SHARE_ZIBT_16BYTE_ALIGN                    ATTR_SHARE_ZIBT ATTR_ALIGN(16)

#endif


//#if defined(CORE_CM4) || defined(CORE_DSP0)
#ifdef CORE_CM4

#define ATTR_TEXT_IN_RAM                                ATTR_TEXT_IN_SYSRAM
#define ATTR_RODATA_IN_RAM                              ATTR_RODATA_IN_SYSRAM
#define ATTR_RWDATA_IN_NONCACHED_RAM                    ATTR_RWDATA_IN_NONCACHED_SYSRAM
#define ATTR_ZIDATA_IN_NONCACHED_RAM                    ATTR_ZIDATA_IN_NONCACHED_SYSRAM

#define ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN        ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN
#define ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN        ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN
#define ATTR_RWDATA_IN_NONCACHED_RAM_8BYTE_ALIGN        ATTR_RWDATA_IN_NONCACHED_SYSRAM_8BYTE_ALIGN
#define ATTR_ZIDATA_IN_NONCACHED_RAM_8BYTE_ALIGN        ATTR_ZIDATA_IN_NONCACHED_SYSRAM_8BYTE_ALIGN
#define ATTR_RWDATA_IN_NONCACHED_RAM_16BYTE_ALIGN       ATTR_RWDATA_IN_NONCACHED_SYSRAM_16BYTE_ALIGN
#define ATTR_ZIDATA_IN_NONCACHED_RAM_16BYTE_ALIGN       ATTR_ZIDATA_IN_NONCACHED_SYSRAM_16BYTE_ALIGN

#endif

#if defined(CORE_DSP0) || defined(CORE_DSP1)

#define ATTR_TEXT_IN_IRAM                               ATTR_SECTION_W_LINE(".iram.", __LINE__)
#define ATTR_RWDATA_IN_DRAM                             __attribute__ ((__section__(".data")))
#define ATTR_ZIDATA_IN_DRAM                             __attribute__ ((__section__(".bss")))
#define ATTR_RWDATA_IN_DRAM_4BYTE_ALIGN                 __attribute__ ((__section__(".data"),__aligned__(4)))
#define ATTR_ZIDATA_IN_DRAM_4BYTE_ALIGN                 __attribute__ ((__section__(".bss"),__aligned__(4)))
#define ATTR_RWDATA_IN_DRAM_8BYTE_ALIGN                 __attribute__ ((__section__(".data"),__aligned__(8)))
#define ATTR_ZIDATA_IN_DRAM_8BYTE_ALIGN                 __attribute__ ((__section__(".bss"),__aligned__(8)))
#define ATTR_RWDATA_IN_DRAM_16BYTE_ALIGN                __attribute__ ((__section__(".data"),__aligned__(16)))
#define ATTR_ZIDATA_IN_DRAM_16BYTE_ALIGN                __attribute__ ((__section__(".bss"),__aligned__(16)))
#endif

#define ATTR_LOG_STRING                                 __attribute__ ((__section__(".log_string"))) static const char
#ifndef MTK_DEBUG_PLAIN_LOG_ENABLE
#define ATTR_LOG_STRING_LIB                             __attribute__ ((__section__(".log_string"))) const char
#else
#define ATTR_LOG_STRING_LIB                             const char
#endif
#define ATTR_LOG_FILTER(module)                         __attribute__ ((__section__(".log_filter." #module)))
#define ATTR_LOG_FILTER_MIRROR(module)                  __attribute__ ((__section__(".log_filter_mirror." #module))) __attribute__((used)) const
#define ATTR_LOG_VERSION                                __attribute__ ((__section__(".log_version"))) __attribute__((used)) static const char
#define ATTR_LOG_TIMESTAMP                              __attribute__ ((__section__(".log_timestamp"))) __attribute__((used)) static const char

#endif
