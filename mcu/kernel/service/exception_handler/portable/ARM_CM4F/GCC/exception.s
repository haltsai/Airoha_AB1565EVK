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

.syntax unified
.cpu cortex-m4
.thumb

.global  exception_stack_pointer
.global  exception_context_pointer
.global  exception_cm4_fault_handler
#if defined(MTK_MEMORY_MONITOR_ENABLE)
.global  memorymonitor_exception_enter_trace
#endif

.section  .exception_code
.weak  CommonFault_Handler
.type  CommonFault_Handler, %function
CommonFault_Handler:
    mrs r2, primask                   /* move primask to r2          */
    cpsid i                           /* disable irq                 */
    ldr r0, =exception_context_pointer
    ldr r0, [r0]                      /* r0 := exception_context_pointer*/
    tst r12, #4                       /* thread or handler mode?     */
    ite eq
    mrseq r1, msp
    mrsne r1, psp
    ldr r3, [r1, #0]                  /* read r0 from stack          */
    str r3, [r0, #0]                  /* store r0                    */
    ldr r3, [r1, #4]                  /* read r1 from stack          */
    str r3, [r0, #4]                  /* store r1                    */
    ldr r3, [r1, #8]                  /* read r2 from stack          */
    str r3, [r0, #8]                  /* store r2                    */
    ldr r3, [r1, #12]                 /* read r3 from stack          */
    str r3, [r0, #12]                 /* store r3                    */
    ldr r3, [r1, #16]                 /* read r12 from stack         */
    str r3, [r0, #48]                 /* store r12                   */
    ldr r3, [r1, #20]                 /* read lr from stack          */
    str r3, [r0, #56]                 /* store lr                    */
    ldr r3, [r1, #24]                 /* read pc from stack          */
    str r3, [r0, #60]                 /* store pc                    */
    ldr r3, [r1, #28]                 /* read xpsr from stack        */
    str r3, [r0, #64]                 /* store xpsr                  */
    add r0, r0, #16                   /* point to context.r4         */
    stmia r0!, {r4-r11}               /* store r4-r11                */
    mov r5, r12                       /* r5 := EXC_RETURN            */
    add r0, r0, #20                   /* point to context.control    */
    mrs r1, control                   /* move CONTROL to r1          */
    str r1, [r0], #4                  /* store CONTROL               */
    str r5, [r0], #4                  /* store EXC_RETURN            */
    mrs r4, msp                       /* r4 := MSP                   */
    str r4, [r0], #4                  /* store MSP                   */
    mrs r1, psp                       /* move PSP to r1              */
    str r1, [r0], #4                  /* store PSP                   */
    mrs r1, basepri                   /* move basepri to r1          */
    str r1, [r0], #4                  /* store basepri               */
    mov r1, r2                        /* move primask to r1          */
    str r1, [r0], #4                  /* store primask               */
    mrs r1, faultmask                 /* move faultmask to r1        */
    str r1, [r0]                      /* store faultmask             */
    tst r5, #0x10                     /* FPU context?                */
    itt eq
    addeq r0, r0, #68                 /* point to contex.s16         */
    vstmeq r0, {s16-s31}              /* store s16-s31               */
    ldr r3, =exception_stack_pointer
    ldr r3, [r3]                      /* r3 := exception_stack_pointer*/
    cmp r3, #0                        /* if (!exception_stack_pointer)*/
    it ne
    movne sp, r3                      /* update msp                  */
    push {lr}
    bl exception_init
    pop {lr}
    tst r5, #4                        /* thread or handler mode?     */
    ite eq
    moveq r0, r4
    mrsne r0, psp
    bx lr
.size  CommonFault_Handler, .-CommonFault_Handler

.section  .exception_code
.weak  NMI_Handler
.type  NMI_Handler, %function
NMI_Handler:
    mov r12, lr
    bl CommonFault_Handler
    mov r1, #2
    ldr r3, =exception_cm4_fault_handler
    blx r3
.size  NMI_Handler, .-NMI_Handler

.section  .exception_code
.weak  HardFault_Handler
.type  HardFault_Handler, %function
HardFault_Handler:
    mov r12, lr
    bl CommonFault_Handler
    mov r1, #3
    ldr r3, =exception_cm4_fault_handler
    blx r3
.size  HardFault_Handler, .-HardFault_Handler

.section  .exception_code
.weak  MemManage_Handler
.type  MemManage_Handler, %function
MemManage_Handler:
    mov r12, lr
    bl CommonFault_Handler
    mov r1, #4
    ldr r3, =exception_cm4_fault_handler
    blx r3
.size  MemManage_Handler, .-MemManage_Handler

.section  .exception_code
.weak  BusFault_Handler
.type  BusFault_Handler, %function
BusFault_Handler:
    mov r12, lr
    bl CommonFault_Handler
    mov r1, #5
    ldr r3, =exception_cm4_fault_handler
    blx r3
.size  BusFault_Handler, .-BusFault_Handler

.section  .exception_code
.weak  UsageFault_Handler
.type  UsageFault_Handler, %function
UsageFault_Handler:
    mov r12, lr
    bl CommonFault_Handler
    mov r1, #6
    ldr r3, =exception_cm4_fault_handler
    blx r3
.size  UsageFault_Handler, .-UsageFault_Handler

.section  .exception_code
.weak  DebugMon_Handler
.type  DebugMon_Handler, %function
DebugMon_Handler:
#if !defined(MTK_MEMORY_MONITOR_ENABLE)
    mov r12, lr
    bl CommonFault_Handler
    mov r1, #12
    ldr r3, =exception_cm4_fault_handler
    blx r3
#else
    push {r0-r12, r14}
    tst lr, #4
    ite eq
    mrseq r0, msp
    mrsne r0, psp
    mov r1, sp
    ldr r3, =memorymonitor_exception_enter_trace
    blx r3
    cbnz r0, 1f
    pop {r0-r12, r14}
    bx r14
1:
    pop {r0-r12, r14}
    mov r12, lr
    bl CommonFault_Handler
    mov r1, #12
    ldr r3, =exception_cm4_fault_handler
    blx r3
#endif
.size  DebugMon_Handler, .-DebugMon_Handler
