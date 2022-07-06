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
#include "exception_config.h"

// ***************************************************************************
// * Global symbol
// ***************************************************************************
    .global Exception_Handler
    .global Double_Exception_Handler

// ***************************************************************************
// * External symbol
// ***************************************************************************
    .extern exception_dsp_fault_handler
    .extern exception_stack_pointer
    .extern exception_context_pointer

// ***************************************************************************
// * Text Offset
// ***************************************************************************
#if (EXCEPTION_AR_COUNT == 32)
    EXCEPTION_FRAME_SIZE                = 424                   // it should 16byte align
    /* General core registers */
    EXCEPTION_OFFSET_A0                 = 0
    EXCEPTION_OFFSET_A1                 = 4
    EXCEPTION_OFFSET_A2                 = 8
    EXCEPTION_OFFSET_A3                 = 12
    EXCEPTION_OFFSET_A4                 = 16
    EXCEPTION_OFFSET_A5                 = 20
    EXCEPTION_OFFSET_A6                 = 24
    EXCEPTION_OFFSET_A7                 = 28
    EXCEPTION_OFFSET_A8                 = 32
    EXCEPTION_OFFSET_A9                 = 36
    EXCEPTION_OFFSET_A10                = 40
    EXCEPTION_OFFSET_A11                = 44
    EXCEPTION_OFFSET_A12                = 48
    EXCEPTION_OFFSET_A13                = 52
    EXCEPTION_OFFSET_A14                = 56
    EXCEPTION_OFFSET_A15                = 60
    EXCEPTION_OFFSET_A16                = 64
    EXCEPTION_OFFSET_A17                = 68
    EXCEPTION_OFFSET_A18                = 72
    EXCEPTION_OFFSET_A19                = 76
    EXCEPTION_OFFSET_A20                = 80
    EXCEPTION_OFFSET_A21                = 84
    EXCEPTION_OFFSET_A22                = 88
    EXCEPTION_OFFSET_A23                = 92
    EXCEPTION_OFFSET_A24                = 96
    EXCEPTION_OFFSET_A25                = 100
    EXCEPTION_OFFSET_A26                = 104
    EXCEPTION_OFFSET_A27                = 108
    EXCEPTION_OFFSET_A28                = 112
    EXCEPTION_OFFSET_A29                = 116
    EXCEPTION_OFFSET_A30                = 120
    EXCEPTION_OFFSET_A31                = 124
    /* Window option special registers */
    EXCEPTION_OFFSET_WINDOWBASE         = 128
    EXCEPTION_OFFSET_WINDOWSTART        = 132
    /* Loop option special registers */
    EXCEPTION_OFFSET_LBEG               = 136
    EXCEPTION_OFFSET_LEND               = 140
    EXCEPTION_OFFSET_LCOUNT             = 144
    /* Shift amount special registers */
    EXCEPTION_OFFSET_SAR                = 148
    /* Comparison special registers */
    EXCEPTION_OFFSET_SCOMPARE           = 152
    /* Exception and Interrupt option special registers */
    EXCEPTION_OFFSET_EXCCAUSE           = 156
    EXCEPTION_OFFSET_EXCVADDR           = 160
    EXCEPTION_OFFSET_PC                 = 164
    EXCEPTION_OFFSET_EPC1               = 168
    EXCEPTION_OFFSET_EPC2               = 172
    EXCEPTION_OFFSET_EPC3               = 176
    EXCEPTION_OFFSET_EPC4               = 180
    EXCEPTION_OFFSET_EPC5               = 184
    EXCEPTION_OFFSET_EPC6               = 188
    EXCEPTION_OFFSET_EPCNMI             = 192
    EXCEPTION_OFFSET_DEPC               = 196
    EXCEPTION_OFFSET_PS                 = 200
    EXCEPTION_OFFSET_EPS2               = 204
    EXCEPTION_OFFSET_EPS3               = 208
    EXCEPTION_OFFSET_EPS4               = 212
    EXCEPTION_OFFSET_EPS5               = 216
    EXCEPTION_OFFSET_EPS6               = 220
    EXCEPTION_OFFSET_EPSNMI             = 224
    EXCEPTION_OFFSET_EXCSAVE1           = 228
    EXCEPTION_OFFSET_EXCSAVE2           = 232
    EXCEPTION_OFFSET_EXCSAVE3           = 236
    EXCEPTION_OFFSET_EXCSAVE4           = 240
    EXCEPTION_OFFSET_EXCSAVE5           = 244
    EXCEPTION_OFFSET_EXCSAVE6           = 248
    EXCEPTION_OFFSET_EXCSAVENMI         = 252
    EXCEPTION_OFFSET_INTENABLE          = 256
    EXCEPTION_OFFSET_INTERRUPT          = 260
    /* Bool option special registers */
    EXCEPTION_OFFSET_BR                 = 264
    /* Coprocessor option special registers */
    EXCEPTION_OFFSET_CPENABLE           = 268
    /* Debug option special registers */
    EXCEPTION_OFFSET_DEBUGCAUSE         = 272
    EXCEPTION_OFFSET_IBREAKENABLE       = 276
    EXCEPTION_OFFSET_IBREAKA0           = 280
    EXCEPTION_OFFSET_IBREAKA1           = 284
    EXCEPTION_OFFSET_DBREAKA0           = 288
    EXCEPTION_OFFSET_DBREAKA1           = 292
    EXCEPTION_OFFSET_DBREAKC0           = 296
    EXCEPTION_OFFSET_DBREAKC1           = 300
    /* DSP engine special registers */
    EXCEPTION_OFFSET_AEP0               = 304
    EXCEPTION_OFFSET_AEP1               = 312
    EXCEPTION_OFFSET_AEP2               = 320
    EXCEPTION_OFFSET_AEP3               = 328
    EXCEPTION_OFFSET_AEP4               = 336
    EXCEPTION_OFFSET_AEP5               = 344
    EXCEPTION_OFFSET_AEP6               = 352
    EXCEPTION_OFFSET_AEP7               = 360
    EXCEPTION_OFFSET_AEQ0               = 368
    EXCEPTION_OFFSET_AEQ1               = 376
    EXCEPTION_OFFSET_AEQ2               = 384
    EXCEPTION_OFFSET_AEQ3               = 392
    EXCEPTION_OFFSET_AE_OVF_SAR         = 400
    EXCEPTION_OFFSET_AE_BITHEAD         = 404
    EXCEPTION_OFFSET_AE_TS_FTS_BU_BP    = 408
    EXCEPTION_OFFSET_AE_SD_NO           = 412
    EXCEPTION_OFFSET_AE_CBEGIN0         = 416
    EXCEPTION_OFFSET_AE_CEND0           = 420
#elif (EXCEPTION_AR_COUNT == 64)
    EXCEPTION_FRAME_SIZE                = 552                   // it should 16byte align
    /* General core registers */
    EXCEPTION_OFFSET_A0                 = 0
    EXCEPTION_OFFSET_A1                 = 4
    EXCEPTION_OFFSET_A2                 = 8
    EXCEPTION_OFFSET_A3                 = 12
    EXCEPTION_OFFSET_A4                 = 16
    EXCEPTION_OFFSET_A5                 = 20
    EXCEPTION_OFFSET_A6                 = 24
    EXCEPTION_OFFSET_A7                 = 28
    EXCEPTION_OFFSET_A8                 = 32
    EXCEPTION_OFFSET_A9                 = 36
    EXCEPTION_OFFSET_A10                = 40
    EXCEPTION_OFFSET_A11                = 44
    EXCEPTION_OFFSET_A12                = 48
    EXCEPTION_OFFSET_A13                = 52
    EXCEPTION_OFFSET_A14                = 56
    EXCEPTION_OFFSET_A15                = 60
    EXCEPTION_OFFSET_A16                = 64
    EXCEPTION_OFFSET_A17                = 68
    EXCEPTION_OFFSET_A18                = 72
    EXCEPTION_OFFSET_A19                = 76
    EXCEPTION_OFFSET_A20                = 80
    EXCEPTION_OFFSET_A21                = 84
    EXCEPTION_OFFSET_A22                = 88
    EXCEPTION_OFFSET_A23                = 92
    EXCEPTION_OFFSET_A24                = 96
    EXCEPTION_OFFSET_A25                = 100
    EXCEPTION_OFFSET_A26                = 104
    EXCEPTION_OFFSET_A27                = 108
    EXCEPTION_OFFSET_A28                = 112
    EXCEPTION_OFFSET_A29                = 116
    EXCEPTION_OFFSET_A30                = 120
    EXCEPTION_OFFSET_A31                = 124
    EXCEPTION_OFFSET_A32                = 128
    EXCEPTION_OFFSET_A33                = 132
    EXCEPTION_OFFSET_A34                = 136
    EXCEPTION_OFFSET_A35                = 140
    EXCEPTION_OFFSET_A36                = 144
    EXCEPTION_OFFSET_A37                = 148
    EXCEPTION_OFFSET_A38                = 152
    EXCEPTION_OFFSET_A39                = 156
    EXCEPTION_OFFSET_A40                = 160
    EXCEPTION_OFFSET_A41                = 164
    EXCEPTION_OFFSET_A42                = 168
    EXCEPTION_OFFSET_A43                = 172
    EXCEPTION_OFFSET_A44                = 176
    EXCEPTION_OFFSET_A45                = 180
    EXCEPTION_OFFSET_A46                = 184
    EXCEPTION_OFFSET_A47                = 188
    EXCEPTION_OFFSET_A48                = 192
    EXCEPTION_OFFSET_A49                = 196
    EXCEPTION_OFFSET_A50                = 200
    EXCEPTION_OFFSET_A51                = 204
    EXCEPTION_OFFSET_A52                = 208
    EXCEPTION_OFFSET_A53                = 212
    EXCEPTION_OFFSET_A54                = 216
    EXCEPTION_OFFSET_A55                = 220
    EXCEPTION_OFFSET_A56                = 224
    EXCEPTION_OFFSET_A57                = 228
    EXCEPTION_OFFSET_A58                = 232
    EXCEPTION_OFFSET_A59                = 236
    EXCEPTION_OFFSET_A60                = 240
    EXCEPTION_OFFSET_A61                = 244
    EXCEPTION_OFFSET_A62                = 248
    EXCEPTION_OFFSET_A63                = 252
    /* Window option special registers */
    EXCEPTION_OFFSET_WINDOWBASE         = 256
    EXCEPTION_OFFSET_WINDOWSTART        = 260
    /* Loop option special registers */
    EXCEPTION_OFFSET_LBEG               = 264
    EXCEPTION_OFFSET_LEND               = 268
    EXCEPTION_OFFSET_LCOUNT             = 272
    /* Shift amount special registers */
    EXCEPTION_OFFSET_SAR                = 276
    /* Comparison special registers */
    EXCEPTION_OFFSET_SCOMPARE           = 280
    /* Exception and Interrupt option special registers */
    EXCEPTION_OFFSET_EXCCAUSE           = 284
    EXCEPTION_OFFSET_EXCVADDR           = 288
    EXCEPTION_OFFSET_PC                 = 292
    EXCEPTION_OFFSET_EPC1               = 296
    EXCEPTION_OFFSET_EPC2               = 300
    EXCEPTION_OFFSET_EPC3               = 304
    EXCEPTION_OFFSET_EPC4               = 308
    EXCEPTION_OFFSET_EPC5               = 312
    EXCEPTION_OFFSET_EPC6               = 316
    EXCEPTION_OFFSET_EPCNMI             = 320
    EXCEPTION_OFFSET_DEPC               = 324
    EXCEPTION_OFFSET_PS                 = 328
    EXCEPTION_OFFSET_EPS2               = 332
    EXCEPTION_OFFSET_EPS3               = 336
    EXCEPTION_OFFSET_EPS4               = 340
    EXCEPTION_OFFSET_EPS5               = 344
    EXCEPTION_OFFSET_EPS6               = 348
    EXCEPTION_OFFSET_EPSNMI             = 352
    EXCEPTION_OFFSET_EXCSAVE1           = 356
    EXCEPTION_OFFSET_EXCSAVE2           = 360
    EXCEPTION_OFFSET_EXCSAVE3           = 364
    EXCEPTION_OFFSET_EXCSAVE4           = 368
    EXCEPTION_OFFSET_EXCSAVE5           = 372
    EXCEPTION_OFFSET_EXCSAVE6           = 376
    EXCEPTION_OFFSET_EXCSAVENMI         = 380
    EXCEPTION_OFFSET_INTENABLE          = 384
    EXCEPTION_OFFSET_INTERRUPT          = 388
    /* Bool option special registers */
    EXCEPTION_OFFSET_BR                 = 392
    /* Coprocessor option special registers */
    EXCEPTION_OFFSET_CPENABLE           = 396
    /* Debug option special registers */
    EXCEPTION_OFFSET_DEBUGCAUSE         = 400
    EXCEPTION_OFFSET_IBREAKENABLE       = 404
    EXCEPTION_OFFSET_IBREAKA0           = 408
    EXCEPTION_OFFSET_IBREAKA1           = 412
    EXCEPTION_OFFSET_DBREAKA0           = 416
    EXCEPTION_OFFSET_DBREAKA1           = 420
    EXCEPTION_OFFSET_DBREAKC0           = 424
    EXCEPTION_OFFSET_DBREAKC1           = 428
    /* DSP engine special registers */
    EXCEPTION_OFFSET_AEP0               = 432
    EXCEPTION_OFFSET_AEP1               = 440
    EXCEPTION_OFFSET_AEP2               = 448
    EXCEPTION_OFFSET_AEP3               = 456
    EXCEPTION_OFFSET_AEP4               = 464
    EXCEPTION_OFFSET_AEP5               = 472
    EXCEPTION_OFFSET_AEP6               = 480
    EXCEPTION_OFFSET_AEP7               = 488
    EXCEPTION_OFFSET_AEQ0               = 496
    EXCEPTION_OFFSET_AEQ1               = 504
    EXCEPTION_OFFSET_AEQ2               = 512
    EXCEPTION_OFFSET_AEQ3               = 520
    EXCEPTION_OFFSET_AE_OVF_SAR         = 528
    EXCEPTION_OFFSET_AE_BITHEAD         = 532
    EXCEPTION_OFFSET_AE_TS_FTS_BU_BP    = 536
    EXCEPTION_OFFSET_AE_SD_NO           = 540
    EXCEPTION_OFFSET_AE_CBEGIN0         = 544
    EXCEPTION_OFFSET_AE_CEND0           = 548
#endif

// ***************************************************************************
// * Exception_Handler
// * <null>
// *
// * exception handler
// ***************************************************************************
    .section .iram, "ax"
    .align 4
    .literal_position
Exception_Handler:
        /* save context to exceptionContext */
        movi    a0 , exception_context_pointer                         // change a0 to the address of exception_context_pointer
        l32i    a0 , a0 , 0                                            // change a0 to the address of exceptionContext
        /* save general core registers */
        s32i    a1 , a0 , EXCEPTION_OFFSET_A1
        s32i    a2 , a0 , EXCEPTION_OFFSET_A2
        s32i    a3 , a0 , EXCEPTION_OFFSET_A3
        s32i    a4 , a0 , EXCEPTION_OFFSET_A4
        s32i    a5 , a0 , EXCEPTION_OFFSET_A5
        s32i    a6 , a0 , EXCEPTION_OFFSET_A6
        s32i    a7 , a0 , EXCEPTION_OFFSET_A7
        s32i    a8 , a0 , EXCEPTION_OFFSET_A8
        s32i    a9 , a0 , EXCEPTION_OFFSET_A9
        s32i    a10 , a0 , EXCEPTION_OFFSET_A10
        s32i    a11 , a0 , EXCEPTION_OFFSET_A11
        s32i    a12 , a0 , EXCEPTION_OFFSET_A12
        s32i    a13 , a0 , EXCEPTION_OFFSET_A13
        s32i    a14 , a0 , EXCEPTION_OFFSET_A14
        s32i    a15 , a0 , EXCEPTION_OFFSET_A15
        rsr     a3 , EXCSAVE_1                                         // restore a0 to a3
        s32i    a3 , a0 , EXCEPTION_OFFSET_A0
        /* save window special registers */
        rsr     a3 , WindowBase
        s32i    a3 , a0 , EXCEPTION_OFFSET_WINDOWBASE
        rsr     a3 , WindowStart
        s32i    a3 , a0 , EXCEPTION_OFFSET_WINDOWSTART
        /* save Loop option special registers */
        rsr     a3 , LBEG
        s32i    a3 , a0 , EXCEPTION_OFFSET_LBEG
        rsr     a3 , LEND
        s32i    a3 , a0 , EXCEPTION_OFFSET_LEND
        rsr     a3 , LCOUNT
        s32i    a3 , a0 , EXCEPTION_OFFSET_LCOUNT
        /* save Shift amount special registers */
        rsr     a3 , SAR
        s32i    a3 , a0 , EXCEPTION_OFFSET_SAR
        /* save Comparison special registers */
        rsr     a3 , SCOMPARE1
        s32i    a3 , a0 , EXCEPTION_OFFSET_SCOMPARE
        /* save Exception and Interrupt option special registers */
        rsr     a3 , EXCCAUSE
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCCAUSE
        rsr     a3 , EXCVADDR
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCVADDR
        rsr     a3 , EPC_1
        s32i    a3 , a0 , EXCEPTION_OFFSET_PC                          // hw save PC to EPC_1
        rsr     a3 , EPC_1
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPC1
        rsr     a3 , EPC_2
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPC2
        rsr     a3 , EPC_3
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPC3
        rsr     a3 , EPC_4
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPC4
        rsr     a3 , EPC_5
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPC5
        rsr     a3 , EPC_6
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPC6
        rsr     a3 , EPC + XCHAL_NMILEVEL
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPCNMI
        rsr     a3 , DEPC
        s32i    a3 , a0 , EXCEPTION_OFFSET_DEPC
        /* clear PS.EXCM because PS.EXCM is 0 when first exception happens */
        movi    a2 , 0xffffffef
        rsr     a3 , PS
        and     a3 , a3 , a2
        s32i    a3 , a0 , EXCEPTION_OFFSET_PS
        rsr     a3 , EPS2
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPS2
        rsr     a3 , EPS3
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPS3
        rsr     a3 , EPS4
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPS4
        rsr     a3 , EPS5
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPS5
        rsr     a3 , EPS6
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPS6
        rsr     a3 , EPS + XCHAL_NMILEVEL
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPSNMI
        rsr     a3 , EXCSAVE_1
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCSAVE1
        rsr     a3 , EXCSAVE_2
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCSAVE2
        rsr     a3 , EXCSAVE_3
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCSAVE3
        rsr     a3 , EXCSAVE_4
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCSAVE4
        rsr     a3 , EXCSAVE_5
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCSAVE5
        rsr     a3 , EXCSAVE_6
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCSAVE6
        rsr     a3 , EXCSAVE + XCHAL_NMILEVEL
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCSAVENMI
        rsr     a3 , INTENABLE
        s32i    a3 , a0 , EXCEPTION_OFFSET_INTENABLE
        rsr     a3 , INTERRUPT
        s32i    a3 , a0 , EXCEPTION_OFFSET_INTERRUPT
        /* save Bool option special registers */
        rsr     a3 , BR
        s32i    a3 , a0 , EXCEPTION_OFFSET_BR
        /* save Coprocessor option special registers */
        rsr     a3 , CPENABLE
        s32i    a3 , a0 , EXCEPTION_OFFSET_CPENABLE
        /* save Debug option special registers */
        rsr     a3 , DEBUGCAUSE
        s32i    a3 , a0 , EXCEPTION_OFFSET_DEBUGCAUSE
        rsr     a3 , IBREAKENABLE
        s32i    a3 , a0 , EXCEPTION_OFFSET_IBREAKENABLE
        rsr     a3 , IBREAKA0
        s32i    a3 , a0 , EXCEPTION_OFFSET_IBREAKA0
        rsr     a3 , IBREAKA1
        s32i    a3 , a0 , EXCEPTION_OFFSET_IBREAKA1
        rsr     a3 , DBREAKA0
        s32i    a3 , a0 , EXCEPTION_OFFSET_DBREAKA0
        rsr     a3 , DBREAKA1
        s32i    a3 , a0 , EXCEPTION_OFFSET_DBREAKA1
        rsr     a3 , DBREAKC0
        s32i    a3 , a0 , EXCEPTION_OFFSET_DBREAKC0
        rsr     a3 , DBREAKC1
        s32i    a3 , a0 , EXCEPTION_OFFSET_DBREAKC1
        /* save DSP engine special registers */
        /* enable dsp co-processor */
        movi    a3 , 2
        wsr     a3 , CPENABLE
        isync
        addi    a0 , a0 , EXCEPTION_OFFSET_AEP0 - 8
        ae_sp24x2s.iu	aep0,  a0, 8
        ae_sp24x2s.iu	aep1,  a0, 8
        ae_sp24x2s.iu	aep2,  a0, 8
        ae_sp24x2s.iu	aep3,  a0, 8
        ae_sp24x2s.iu	aep4,  a0, 8
        ae_sp24x2s.iu	aep5,  a0, 8
        ae_sp24x2s.iu	aep6,  a0, 8
        ae_sp24x2s.iu	aep7,  a0, 8
        ae_sq56s.iu	    aeq0 , a0, 8
        ae_sq56s.iu	    aeq1 , a0, 8
        ae_sq56s.iu	    aeq2 , a0, 8
        ae_sq56s.iu	    aeq3 , a0, 8
        addi    a0 , a0 , -EXCEPTION_OFFSET_AEQ3                       // change a0 to pointer to exceptionContext
        rur.AE_OVF_SAR       a3
        s32i                 a3 , a0, EXCEPTION_OFFSET_AE_OVF_SAR
        rur.AE_BITHEAD       a3
        s32i                 a3 , a0, EXCEPTION_OFFSET_AE_BITHEAD
        rur.AE_TS_FTS_BU_BP  a3
        s32i                 a3 , a0, EXCEPTION_OFFSET_AE_TS_FTS_BU_BP
        rur.AE_SD_NO         a3
        s32i                 a3 , a0, EXCEPTION_OFFSET_AE_SD_NO
#if defined(CORE_DSP0)
        //rur.AE_CBEGIN0       a3
        //s32i                 a3 , a0, EXCEPTION_OFFSET_AE_CBEGIN0
        //rur.AE_CEND0         a3
        //s32i                 a3 , a0, EXCEPTION_OFFSET_AE_CEND0
#endif
#ifdef __XTENSA_CALL0_ABI__
        /* clear PS.EXCM and set PS.Level to 0xf*/
        movi    a3 , 0x2f
#else
        /* use ROTW instruction move Window */
        //PS.EXCM is 1, so CWOE is 0. That means window overflow check is disable in Exception Flow
        mov     a8  , a0
        rotw    2                                                      // window offset 8 AR registers
        isync
        s32i    a8  , a0 , EXCEPTION_OFFSET_A16
        s32i    a9  , a0 , EXCEPTION_OFFSET_A17
        s32i    a10 , a0 , EXCEPTION_OFFSET_A18
        s32i    a11 , a0 , EXCEPTION_OFFSET_A19
        s32i    a12 , a0 , EXCEPTION_OFFSET_A20
        s32i    a13 , a0 , EXCEPTION_OFFSET_A21
        s32i    a14 , a0 , EXCEPTION_OFFSET_A22
        s32i    a15 , a0 , EXCEPTION_OFFSET_A23
        mov     a8  , a0
        rotw    2                                                      // window offset 8 AR registers
        isync
        s32i    a8  , a0 , EXCEPTION_OFFSET_A24
        s32i    a9  , a0 , EXCEPTION_OFFSET_A25
        s32i    a10 , a0 , EXCEPTION_OFFSET_A26
        s32i    a11 , a0 , EXCEPTION_OFFSET_A27
        s32i    a12 , a0 , EXCEPTION_OFFSET_A28
        s32i    a13 , a0 , EXCEPTION_OFFSET_A29
        s32i    a14 , a0 , EXCEPTION_OFFSET_A30
        s32i    a15 , a0 , EXCEPTION_OFFSET_A31
#if (EXCEPTION_AR_COUNT == 64)
        mov     a8  , a0
        rotw    2                                                      // window offset 8 AR registers
        isync
        s32i    a8  , a0 , EXCEPTION_OFFSET_A32
        s32i    a9  , a0 , EXCEPTION_OFFSET_A33
        s32i    a10 , a0 , EXCEPTION_OFFSET_A34
        s32i    a11 , a0 , EXCEPTION_OFFSET_A35
        s32i    a12 , a0 , EXCEPTION_OFFSET_A36
        s32i    a13 , a0 , EXCEPTION_OFFSET_A37
        s32i    a14 , a0 , EXCEPTION_OFFSET_A38
        s32i    a15 , a0 , EXCEPTION_OFFSET_A39
        mov     a8  , a0
        rotw    2                                                      // window offset 8 AR registers
        isync
        s32i    a8  , a0 , EXCEPTION_OFFSET_A40
        s32i    a9  , a0 , EXCEPTION_OFFSET_A41
        s32i    a10 , a0 , EXCEPTION_OFFSET_A42
        s32i    a11 , a0 , EXCEPTION_OFFSET_A43
        s32i    a12 , a0 , EXCEPTION_OFFSET_A44
        s32i    a13 , a0 , EXCEPTION_OFFSET_A45
        s32i    a14 , a0 , EXCEPTION_OFFSET_A46
        s32i    a15 , a0 , EXCEPTION_OFFSET_A47
        mov     a8  , a0
        rotw    2                                                      // window offset 8 AR registers
        isync
        s32i    a8  , a0 , EXCEPTION_OFFSET_A48
        s32i    a9  , a0 , EXCEPTION_OFFSET_A49
        s32i    a10 , a0 , EXCEPTION_OFFSET_A50
        s32i    a11 , a0 , EXCEPTION_OFFSET_A51
        s32i    a12 , a0 , EXCEPTION_OFFSET_A52
        s32i    a13 , a0 , EXCEPTION_OFFSET_A53
        s32i    a14 , a0 , EXCEPTION_OFFSET_A54
        s32i    a15 , a0 , EXCEPTION_OFFSET_A55
        mov     a8  , a0
        rotw    2                                                      // window offset 8 AR registers
        isync
        s32i    a8  , a0 , EXCEPTION_OFFSET_A56
        s32i    a9  , a0 , EXCEPTION_OFFSET_A57
        s32i    a10 , a0 , EXCEPTION_OFFSET_A58
        s32i    a11 , a0 , EXCEPTION_OFFSET_A59
        s32i    a12 , a0 , EXCEPTION_OFFSET_A60
        s32i    a13 , a0 , EXCEPTION_OFFSET_A61
        s32i    a14 , a0 , EXCEPTION_OFFSET_A62
        s32i    a15 , a0 , EXCEPTION_OFFSET_A63
#endif
        /* init window registers' status */
        movi    a3  , 0
        wsr     a3  , WindowBase
        isync
        movi    a3  , 1
        wsr     a3  , WindowStart
        isync
        /* clear PS.EXCM and set PS.Level to 0xf and enable window check*/
        movi    a3 , 0x0004002f
#endif
        wsr     a3 , PS
        isync
        /* restore a0 value */
        //rsr     a0 , EXCSAVE_1
#ifdef __XTENSA_CALL0_ABI__
        /* change sp to exception stack */
        movi    a2 , exception_stack_pointer                           // change a2 to the address of exception_stack_pointer
        l32i    a1 , a2 , 0                                            // change a1 to the address of exception_stack
        movi    a0 , exception_dsp_fault_handler
        callx0  a0
#else
        /* stack init and change sp to exception stack - 16  */
        movi    a2 , exception_stack_pointer                                  // change a2 to the address of exception_stack_pointer
        l32i    a4 , a2 , 0                                            // change a4 to the address of exception_stack
        addi    a1 , a4 , -16                                          // change a1 to the address of exception_stack - 16
        addi    a4 , a1 , 32
        s32e    a4 , a1 , -12
        movi    a8 , exception_dsp_fault_handler
        callx8  a8
#endif

// ***************************************************************************
// * Double_Exception_Handler
// * <null>
// *
// * Double exception handler
// ***************************************************************************
    .section .iram, "ax"
    .align 4
    .literal_position
Double_Exception_Handler:
        /* save context to exceptionContext */
        movi    a0 , exception_context_pointer                         // change a0 to the address of exception_context_pointer
        l32i    a0 , a0 , 0                                            // change a0 to the address of exceptionContext
        /* save general core registers */
        s32i    a1 , a0 , EXCEPTION_OFFSET_A1
        s32i    a2 , a0 , EXCEPTION_OFFSET_A2
        s32i    a3 , a0 , EXCEPTION_OFFSET_A3
        s32i    a4 , a0 , EXCEPTION_OFFSET_A4
        s32i    a5 , a0 , EXCEPTION_OFFSET_A5
        s32i    a6 , a0 , EXCEPTION_OFFSET_A6
        s32i    a7 , a0 , EXCEPTION_OFFSET_A7
        s32i    a8 , a0 , EXCEPTION_OFFSET_A8
        s32i    a9 , a0 , EXCEPTION_OFFSET_A9
        s32i    a10 , a0 , EXCEPTION_OFFSET_A10
        s32i    a11 , a0 , EXCEPTION_OFFSET_A11
        s32i    a12 , a0 , EXCEPTION_OFFSET_A12
        s32i    a13 , a0 , EXCEPTION_OFFSET_A13
        s32i    a14 , a0 , EXCEPTION_OFFSET_A14
        s32i    a15 , a0 , EXCEPTION_OFFSET_A15
        rsr     a3 , EXCSAVE + XCHAL_NMILEVEL                         // restore a0 to a3
        s32i    a3 , a0 , EXCEPTION_OFFSET_A0
        /* save window special registers */
        rsr     a3 , WindowBase
        s32i    a3 , a0 , EXCEPTION_OFFSET_WINDOWBASE
        rsr     a3 , WindowStart
        s32i    a3 , a0 , EXCEPTION_OFFSET_WINDOWSTART
        /* save Loop option special registers */
        rsr     a3 , LBEG
        s32i    a3 , a0 , EXCEPTION_OFFSET_LBEG
        rsr     a3 , LEND
        s32i    a3 , a0 , EXCEPTION_OFFSET_LEND
        rsr     a3 , LCOUNT
        s32i    a3 , a0 , EXCEPTION_OFFSET_LCOUNT
        /* save Shift amount special registers */
        rsr     a3 , SAR
        s32i    a3 , a0 , EXCEPTION_OFFSET_SAR
        /* save Comparison special registers */
        rsr     a3 , SCOMPARE1
        s32i    a3 , a0 , EXCEPTION_OFFSET_SCOMPARE
        /* save Exception and Interrupt option special registers */
        rsr     a3 , EXCCAUSE
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCCAUSE
        rsr     a3 , EXCVADDR
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCVADDR
        rsr     a3 , DEPC
        s32i    a3 , a0 , EXCEPTION_OFFSET_PC                          // hw save PC to DEPC
        rsr     a3 , EPC_1
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPC1
        rsr     a3 , EPC_2
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPC2
        rsr     a3 , EPC_3
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPC3
        rsr     a3 , EPC_4
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPC4
        rsr     a3 , EPC_5
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPC5
        rsr     a3 , EPC_6
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPC6
        rsr     a3 , EPC + XCHAL_NMILEVEL
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPCNMI
        rsr     a3 , DEPC
        s32i    a3 , a0 , EXCEPTION_OFFSET_DEPC
        /* do not need to clear PS.EXCM because it is not first exception */
        rsr     a3 , PS
        s32i    a3 , a0 , EXCEPTION_OFFSET_PS
        rsr     a3 , EPS2
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPS2
        rsr     a3 , EPS3
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPS3
        rsr     a3 , EPS4
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPS4
        rsr     a3 , EPS5
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPS5
        rsr     a3 , EPS6
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPS6
        rsr     a3 , EPS + XCHAL_NMILEVEL
        s32i    a3 , a0 , EXCEPTION_OFFSET_EPSNMI
        rsr     a3 , EXCSAVE_1
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCSAVE1
        rsr     a3 , EXCSAVE_2
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCSAVE2
        rsr     a3 , EXCSAVE_3
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCSAVE3
        rsr     a3 , EXCSAVE_4
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCSAVE4
        rsr     a3 , EXCSAVE_5
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCSAVE5
        rsr     a3 , EXCSAVE_6
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCSAVE6
        rsr     a3 , EXCSAVE + XCHAL_NMILEVEL
        s32i    a3 , a0 , EXCEPTION_OFFSET_EXCSAVENMI
        rsr     a3 , INTENABLE
        s32i    a3 , a0 , EXCEPTION_OFFSET_INTENABLE
        rsr     a3 , INTERRUPT
        s32i    a3 , a0 , EXCEPTION_OFFSET_INTERRUPT
        /* save Bool option special registers */
        rsr     a3 , BR
        s32i    a3 , a0 , EXCEPTION_OFFSET_BR
        /* save Coprocessor option special registers */
        rsr     a3 , CPENABLE
        s32i    a3 , a0 , EXCEPTION_OFFSET_CPENABLE
        /* save Debug option special registers */
        rsr     a3 , DEBUGCAUSE
        s32i    a3 , a0 , EXCEPTION_OFFSET_DEBUGCAUSE
        rsr     a3 , IBREAKENABLE
        s32i    a3 , a0 , EXCEPTION_OFFSET_IBREAKENABLE
        rsr     a3 , IBREAKA0
        s32i    a3 , a0 , EXCEPTION_OFFSET_IBREAKA0
        rsr     a3 , IBREAKA1
        s32i    a3 , a0 , EXCEPTION_OFFSET_IBREAKA1
        rsr     a3 , DBREAKA0
        s32i    a3 , a0 , EXCEPTION_OFFSET_DBREAKA0
        rsr     a3 , DBREAKA1
        s32i    a3 , a0 , EXCEPTION_OFFSET_DBREAKA1
        rsr     a3 , DBREAKC0
        s32i    a3 , a0 , EXCEPTION_OFFSET_DBREAKC0
        rsr     a3 , DBREAKC1
        s32i    a3 , a0 , EXCEPTION_OFFSET_DBREAKC1
        /* save DSP engine special registers */
        /* enable dsp co-processor */
        movi    a3 , 2
        wsr     a3 , CPENABLE
        isync
        addi    a0 , a0 , EXCEPTION_OFFSET_AEP0 - 8
        ae_sp24x2s.iu	aep0,  a0, 8
        ae_sp24x2s.iu	aep1,  a0, 8
        ae_sp24x2s.iu	aep2,  a0, 8
        ae_sp24x2s.iu	aep3,  a0, 8
        ae_sp24x2s.iu	aep4,  a0, 8
        ae_sp24x2s.iu	aep5,  a0, 8
        ae_sp24x2s.iu	aep6,  a0, 8
        ae_sp24x2s.iu	aep7,  a0, 8
        ae_sq56s.iu	    aeq0 , a0, 8
        ae_sq56s.iu	    aeq1 , a0, 8
        ae_sq56s.iu	    aeq2 , a0, 8
        ae_sq56s.iu	    aeq3 , a0, 8
        addi    a0 , a0 , -EXCEPTION_OFFSET_AEQ3                       // change a0 to pointer to exceptionContext
        rur.AE_OVF_SAR       a3
        s32i                 a3 , a0, EXCEPTION_OFFSET_AE_OVF_SAR
        rur.AE_BITHEAD       a3
        s32i                 a3 , a0, EXCEPTION_OFFSET_AE_BITHEAD
        rur.AE_TS_FTS_BU_BP  a3
        s32i                 a3 , a0, EXCEPTION_OFFSET_AE_TS_FTS_BU_BP
        rur.AE_SD_NO         a3
        s32i                 a3 , a0, EXCEPTION_OFFSET_AE_SD_NO
#if defined(CORE_DSP0)
        //rur.AE_CBEGIN0       a3
        //s32i                 a3 , a0, EXCEPTION_OFFSET_AE_CBEGIN0
        //rur.AE_CEND0         a3
        //s32i                 a3 , a0, EXCEPTION_OFFSET_AE_CEND0
#endif
#ifdef __XTENSA_CALL0_ABI__
        /* clear PS.EXCM and set PS.Level to 0xf*/
        movi    a3 , 0x2f
#else
        /* use ROTW instruction move Window */
        //PS.EXCM is 1, so CWOE is 0. That means window overflow check is disable in Exception Flow
        mov     a8  , a0
        rotw    2                                                      // window offset 8 AR registers
        isync
        s32i    a8  , a0 , EXCEPTION_OFFSET_A16
        s32i    a9  , a0 , EXCEPTION_OFFSET_A17
        s32i    a10 , a0 , EXCEPTION_OFFSET_A18
        s32i    a11 , a0 , EXCEPTION_OFFSET_A19
        s32i    a12 , a0 , EXCEPTION_OFFSET_A20
        s32i    a13 , a0 , EXCEPTION_OFFSET_A21
        s32i    a14 , a0 , EXCEPTION_OFFSET_A22
        s32i    a15 , a0 , EXCEPTION_OFFSET_A23
        mov     a8  , a0
        rotw    2                                                      // window offset 8 AR registers
        isync
        s32i    a8  , a0 , EXCEPTION_OFFSET_A24
        s32i    a9  , a0 , EXCEPTION_OFFSET_A25
        s32i    a10 , a0 , EXCEPTION_OFFSET_A26
        s32i    a11 , a0 , EXCEPTION_OFFSET_A27
        s32i    a12 , a0 , EXCEPTION_OFFSET_A28
        s32i    a13 , a0 , EXCEPTION_OFFSET_A29
        s32i    a14 , a0 , EXCEPTION_OFFSET_A30
        s32i    a15 , a0 , EXCEPTION_OFFSET_A31
#if (EXCEPTION_AR_COUNT == 64)
        mov     a8  , a0
        rotw    2                                                      // window offset 8 AR registers
        isync
        s32i    a8  , a0 , EXCEPTION_OFFSET_A32
        s32i    a9  , a0 , EXCEPTION_OFFSET_A33
        s32i    a10 , a0 , EXCEPTION_OFFSET_A34
        s32i    a11 , a0 , EXCEPTION_OFFSET_A35
        s32i    a12 , a0 , EXCEPTION_OFFSET_A36
        s32i    a13 , a0 , EXCEPTION_OFFSET_A37
        s32i    a14 , a0 , EXCEPTION_OFFSET_A38
        s32i    a15 , a0 , EXCEPTION_OFFSET_A39
        mov     a8  , a0
        rotw    2                                                      // window offset 8 AR registers
        isync
        s32i    a8  , a0 , EXCEPTION_OFFSET_A40
        s32i    a9  , a0 , EXCEPTION_OFFSET_A41
        s32i    a10 , a0 , EXCEPTION_OFFSET_A42
        s32i    a11 , a0 , EXCEPTION_OFFSET_A43
        s32i    a12 , a0 , EXCEPTION_OFFSET_A44
        s32i    a13 , a0 , EXCEPTION_OFFSET_A45
        s32i    a14 , a0 , EXCEPTION_OFFSET_A46
        s32i    a15 , a0 , EXCEPTION_OFFSET_A47
        mov     a8  , a0
        rotw    2                                                      // window offset 8 AR registers
        isync
        s32i    a8  , a0 , EXCEPTION_OFFSET_A48
        s32i    a9  , a0 , EXCEPTION_OFFSET_A49
        s32i    a10 , a0 , EXCEPTION_OFFSET_A50
        s32i    a11 , a0 , EXCEPTION_OFFSET_A51
        s32i    a12 , a0 , EXCEPTION_OFFSET_A52
        s32i    a13 , a0 , EXCEPTION_OFFSET_A53
        s32i    a14 , a0 , EXCEPTION_OFFSET_A54
        s32i    a15 , a0 , EXCEPTION_OFFSET_A55
        mov     a8  , a0
        rotw    2                                                      // window offset 8 AR registers
        isync
        s32i    a8  , a0 , EXCEPTION_OFFSET_A56
        s32i    a9  , a0 , EXCEPTION_OFFSET_A57
        s32i    a10 , a0 , EXCEPTION_OFFSET_A58
        s32i    a11 , a0 , EXCEPTION_OFFSET_A59
        s32i    a12 , a0 , EXCEPTION_OFFSET_A60
        s32i    a13 , a0 , EXCEPTION_OFFSET_A61
        s32i    a14 , a0 , EXCEPTION_OFFSET_A62
        s32i    a15 , a0 , EXCEPTION_OFFSET_A63
#endif
        /* init window registers' status */
        movi    a3  , 0
        wsr     a3  , WindowBase
        isync
        movi    a3  , 1
        wsr     a3  , WindowStart
        isync
        /* clear PS.EXCM and set PS.Level to 0xf and enable window check*/
        movi    a3 , 0x0004002f
#endif
        wsr     a3 , PS
        isync
        /* restore a0 value */
        //rsr     a0 , EXCSAVE_1
#ifdef __XTENSA_CALL0_ABI__
        /* change sp to exception stack */
        movi    a2 , exception_stack_pointer                           // change a2 to the address of exception_stack_pointer
        l32i    a1 , a2 , 0                                            // change a1 to the address of exception_stack
        movi    a0 , exception_dsp_fault_handler
        callx0  a0
#else
        /* stack init and change sp to exception stack - 16  */
        movi    a2 , exception_stack_pointer                                  // change a2 to the address of exception_stack_pointer
        l32i    a4 , a2 , 0                                            // change a4 to the address of exception_stack
        addi    a1 , a4 , -16                                          // change a1 to the address of exception_stack - 16
        addi    a4 , a1 , 32
        s32e    a4 , a1 , -12
        movi    a8 , exception_dsp_fault_handler
        callx8  a8
#endif
