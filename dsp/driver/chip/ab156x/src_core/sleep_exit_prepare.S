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

#include <xtensa/coreasm.h>

// ***************************************************************************
// * Global symbol
// ***************************************************************************
    .global sleep_exit_prepare

 // ***************************************************************************
 // * External symbol
 // ***************************************************************************
    .section .iram, "ax"
    .align 4
    .literal_position
sleep_exit_prepare:
    movi    a2, 0xA2110CBC
    l32i    a3, a2, 0
    beqz    a3, leave_restore
    movi    a2, 0
    wsr     a2, INTENABLE

    wsr     a2, DBREAKC0
    wsr     a2, DBREAKC1
    dsync

    movi    a2, 0
    wsr     a2, EXCSAVE_1
    wsr     a2, EXCSAVE_2
    wsr     a2, EXCSAVE_3
    wsr     a2, EXCSAVE_4
    wsr     a2, EXCCAUSE
    wsr     a2, EPC_1
    wsr     a2, EPC_2
    wsr     a2, EPC_3
    wsr     a2, EPC_4
    wsr     a2, EPS_2
    wsr     a2, EPS_3
    wsr     a2, EPS_4

    wsr     a2, LCOUNT
    wsr     a2, LBEG
    wsr     a2, LEND

    wsr     a2, BR
    wsr     a2, CPENABLE

    movi    a2 , _DynamicVectors_start
    wsr     a2 , VECBASE
    movi    a2 , 0

    #ifdef __XTENSA_CALL0_ABI__
    movi    sp , __stack
    movi    a3 , PS_UM | PS_INTLEVEL(4) //PS.UM = 1, PS.EXCM = 0, PS.INTLEVEL = 4
    #else
    movi    sp , __stack - 16
    addi    a4 , sp , 32
    s32e    a4 , sp , -12
    movi    a3 , PS_UM | PS_INTLEVEL(4) //PS.WOE = 1, PS.UM = 1, PS.EXCM = 0, PS.INTLEVEL = 4
    #endif
    wsr     a3 , PS
    rsync

    /* enable dsp co-processor */
    movi    a3 , 2
    wsr     a3 , CPENABLE

    /* init window registers' status */
    movi    a3  , 0
    wsr     a3  , WindowBase
    isync
    movi    a3  , 0
    wsr     a3  , WindowStart
    isync

    /* invalidate dcache */
    movi    a2 , 64
    movi    a3 , 0
    loop    a2 , icache_invalidate
    dii     a3 , 0
    dii     a3 , 32
    addi    a3 , a3, 64
    /* invalidate icache */
icache_invalidate:
    movi    a2 , 16
    movi    a3 , 0
    loop    a2 , cache_remap
    iii     a3 , 0
    iii     a3 , 64
    iii     a3 , 128
    iii     a3 , 192
    addi    a3 , a3, 256
cache_remap:
    isync
    /* set Region0 I/D bus cacheable */
    movi    a4 , 0x00000004
    movi    a3 , 0x00000000
    witlb   a4 , a3
    isync
    wdtlb   a4 , a3
    dsync
    /* remap Region1 to Region3 and set Region1 non-cacheable */
    movi    a4 , 0x40000002
    movi    a3 , 0x20000000
    witlb   a4 , a3
    isync
    wdtlb   a4 , a3
    dsync
    /* set Region2 non-cacheable */
    movi    a4 , 0x40000002
    movi    a3 , 0x40000000
    witlb   a4 , a3
    isync
    wdtlb   a4 , a3
    dsync
    /* set Region3 non-cacheable */
    movi    a4 , 0x60000002
    movi    a3 , 0x60000000
    witlb   a4 , a3
    isync
    wdtlb   a4 , a3
    dsync
    /* remap Region4 to Region0 and set Region4 non-cacheable*/
    movi    a4 , 0x00000002
    movi    a3 , 0x80000000
    witlb   a4 , a3
    isync
    wdtlb   a4 , a3
    dsync
    /* set Region5 non-cacheable */
    movi    a4 , 0xa0000002
    movi    a3 , 0xa0000000
    witlb   a4 , a3
    isync
    wdtlb   a4 , a3
    dsync
    /* set Region6 non-cacheable */
    movi    a4 , 0xc0000002
    movi    a3 , 0xc0000000
    witlb   a4 , a3
    isync
    wdtlb   a4 , a3
    dsync
    /* set Region7 non-cacheable */
    movi    a4 , 0xe0000002
    movi    a3 , 0xe0000000
    witlb   a4 , a3
    isync
    wdtlb   a4 , a3
    dsync
leave_restore:
    ret
