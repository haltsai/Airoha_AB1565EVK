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

#ifndef __NVDM_ROM_BOOK_H__
#define __NVDM_ROM_BOOK_H__

const uint8_t T_NVDM_BEEF[] =
{
0xA5, 0xFF,
};

// g_nvdm_default_table table with nvdm ID, nvdm length and nvdm memory pointer.
const mem_nvdm_info_t g_nvdm_default_table[] = {
    {0xE007, 160, NULL },  //sizeof(T_NVDM_E007)
    {0xE101, 264, NULL },  //sizeof(T_NVDM_E101)
    {0xE103, 264, NULL },  //sizeof(T_NVDM_E103)
    {0xE104, 264, NULL },  //sizeof(T_NVDM_E104)
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
    {0xE106, 944, NULL },  //sizeof(T_NVDM_E106)
    {0xE107, 944, NULL },  //sizeof(T_NVDM_E107)
#endif
    {0xE109,  16, NULL },  //sizeof(T_NVDM_E109)
#ifdef MTK_LINEIN_INS_ENABLE
    {0xE110,  44, NULL },  //sizeof(T_NVDM_E110)
#endif
    {0xE140,  46, NULL },  //sizeof(T_NVDM_E140)
    {0xE150, 984, NULL },  //sizeof(T_NVDM_E150)
    {0xE161, 498, NULL },  //sizeof(T_NVDM_E161)
    {0xE162, 498, NULL },  //sizeof(T_NVDM_E162)
    {0xE163, 242, NULL },  //sizeof(T_NVDM_E163)
    {0xE164, 242, NULL },  //sizeof(T_NVDM_E164)
    {0xE190,  76, NULL },  //sizeof(T_NVDM_E190)
    {0xE191,  76, NULL },  //sizeof(T_NVDM_E191)
    {0xE192,  76, NULL },  //sizeof(T_NVDM_E192)
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
    {0xE193, 984, NULL },
    {0xE194, 242, NULL },
    {0xE195, 498, NULL },
    {0xE196, 264, NULL },
    {0xE197, 184, NULL },
    {0xE198, 184, NULL },
#endif
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
    {0xE167, 498, NULL },  //sizeof(T_NVDM_E167)
#endif
    {0xE168, 130, NULL },  //sizeof(T_NVDM_E168)
#if defined(MTK_3RD_PARTY_NR)
    {0xE169, 184, NULL },  //sizeof(T_NVDM_E169)
    {0xE16A, 184, NULL },  //sizeof(T_NVDM_E16A)
#endif
#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    {0xF504, 400, NULL },
    {0xF505,   4, NULL },
    {0xF506, 400, NULL },
    {0xF507, 400, NULL },
    {0xF508,  96, NULL },
    {0xF509, 960, NULL },
    {0xF50A,  12, NULL },
#endif
#if defined(AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE)
    {0xF50C, 132, NULL },
#endif
    {0xBEEF, sizeof(T_NVDM_BEEF), &T_NVDM_BEEF },
    {0xFFFF,                   0,         NULL } // NVKEY_END
};

uint32_t DSP0_NVDM_ITEM_MAX	= (sizeof(g_nvdm_default_table) / sizeof(mem_nvdm_info_t));

#endif  /* __NVDM_ROM_BOOK_H__ */
