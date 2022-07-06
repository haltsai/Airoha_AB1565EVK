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
 
#include "exception_handler.h"
#include "ab156x.h"
#include "hal_resource_assignment.h"

/******************************************************************************/
/*            Memory Regions Definition                                       */
/******************************************************************************/
#if defined(__GNUC__)

extern unsigned int _ResetVector_text_start[];
extern unsigned int _ResetVector_text_end[];
extern unsigned int _text_start[];
extern unsigned int _text_end[];
extern unsigned int _DynamicVectors_start[];
extern unsigned int _DynamicVectors_end[];
extern unsigned int _iram_text_start[];
extern unsigned int _iram_text_end[];
extern unsigned int _stack_start[];
extern unsigned int __stack[];
extern unsigned int port_IntStack[];
extern unsigned int port_IntStackTop[];
extern unsigned int _data_start[];
extern unsigned int _bss_end[];
extern unsigned int _preloader_pic_iram_start[];
extern unsigned int _preloader_pic_iram_end[];
extern unsigned int _preloader_pic_dram_start[];
extern unsigned int _preloader_pic_dram_end[];


const memory_region_type memory_regions[] =
{
    {"reset_vector", _ResetVector_text_start, _ResetVector_text_end, 0},
    {"text", _text_start, _text_end, 0},
    {"dynamic_vector", _DynamicVectors_start, _DynamicVectors_end, 1},
    {"iram_text", _iram_text_start, _iram_text_end, 1},
    {"stack", _stack_start, __stack, 1},
    {"system_stack", port_IntStack, port_IntStackTop, 1},
    {"data", _data_start, _bss_end, 1},
    {"private_memory", (unsigned int *)(HW_SYSRAM_PRIVATE_MEMORY_START), (unsigned int *)(HW_SYSRAM_PRIVATE_MEMORY_START + HW_SYSRAM_PRIVATE_MEMORY_LEN), 1},
    {"preloader_pic_iram", _preloader_pic_iram_start, _preloader_pic_iram_end, 1},
    {"preloader_pic_dram", _preloader_pic_dram_start, _preloader_pic_dram_end, 1},
    {0, 0, 0, 0}
};

#ifdef MTK_SWLA_ENABLE
extern unsigned int _swla_start[];
extern unsigned int _swla_end[];

void SLA_get_region(uint32_t *pxBase, uint32_t *pxLen)
{
    *pxBase = ((uint32_t)_swla_start & (0x10 - 1))? (((uint32_t)_swla_start + 0x10) & ~(0x10 - 1)) : (uint32_t)_swla_start;  /* align up to 16Byte for cacheline alignment */
    *pxLen = (uint32_t)(((uint32_t)_swla_end - *pxBase) ) & ~(0x10 - 1);
}
#endif /* MTK_SWLA_ENABLE */

#endif /* __GNUC__ */

#if defined (__CC_ARM)

const memory_region_type memory_regions[] =
{
    {0},
    {0},
    {0},
    {0},
    {0}

};

#endif /* __CC_ARM */

#if defined(__ICCARM__)

const memory_region_type memory_regions[] =
{
    {0},
    {0},
    {0},
    {0},
    {0}
};

#endif /* __ICCARM__ */


