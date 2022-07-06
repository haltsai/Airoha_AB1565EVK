/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

    @ retarget memcpy
    @ prototype
    @ void memcpy(void *dest, const void *src, size_t n);
    #if defined(MT2822)||defined(AB156X)
    .section ".tcm_code","ax"
    #else
    .section ".text.memcpy","ax"
    #endif
    .global __wrap_memcpy
    .align 3
    .type __wrap_memcpy,STT_FUNC
__wrap_memcpy:
    @ r0=dest, r1=src, r2=length
    CMP     r2, #3              @ if (length < 4) goto _memcpy_lastbytes
    BLS     _memcpy2_memcpy_lastbytes
    ANDS    r12, r0, #3         @ if (dest % 4 == 0) goto dest_aligned
    BEQ     _memcpy2_dest_aligned

    @ storing to dest tail, make dest aligned
    LDRB    r3, [r1], #1
    STRB    r3, [r0], #1        @ at least need to save the 1st byte

    CMP     r12, #2
    ITT     LS                  @ dest 1byte or 2byte offset
    LDRBLS  r3, [r1], #1
    STRBLS  r3, [r0], #1        @ save the 2nd byte for dest is 1byte or 2byte offset

    CMP     r12, #1
    ITT     EQ
    LDRBEQ  r3, [r1], #1
    STRBEQ  r3, [r0], #1        @ save the 3rd byte for dest is 1byte offset

    ADD     r2, r2, r12
    SUB     r2, r2, #4          @ adjust the left copy length

_memcpy2_dest_aligned:
    ANDS    r3, r1, #3          @ if (src % 4 == 0) goto memcpy_burst
    BEQ     memcpy_burst
_memcpy2_src_not_aligned:
    CMP     r2, #3
    BLS     _memcpy2_memcpy_lastbytes
    SUB     r2, r2, #4
    SUB     r1, r3
    LDR     r12, [r1]
    CMP     r3, #2              @ if (src % 4 == 1) goto src_mod4_1
    BEQ     _memcpy2_src_mod4_2 @ if (src % 4 == 2) goto src_mod4_2
    BHI     _memcpy2_src_mod4_3 @ if (src % 4 == 3) goto src_mod4_3
_memcpy2_src_mod4_1:
    LSR     r3, r12, #8
    LDR     r12, [r1, #4]!
    SUBS    r2, r2, #4
    ORR     r3, r3, r12, LSL #24
    STR     r3, [r0], #4
    BCS     _memcpy2_src_mod4_1
    ADD     r2, r2, #4
    ADD     r1, r1, #1
    B       _memcpy2_memcpy_lastbytes
_memcpy2_src_mod4_2:
    LSR     r3, r12, #16
    LDR     r12, [r1, #4]!
    SUBS    r2, r2, #4
    ORR     r3, r3, r12, LSL #16
    STR     r3, [r0], #4
    BCS     _memcpy2_src_mod4_2
    ADD     r2, r2, #4
    ADD     r1, r1, #2
    B       _memcpy2_memcpy_lastbytes
_memcpy2_src_mod4_3:
    LSR     r3, r12, #24
    LDR     r12, [r1, #4]!
    SUBS    r2, r2, #4
    ORR     r3, r3, r12, LSL #8
    STR     r3, [r0], #4
    BCS     _memcpy2_src_mod4_3
    ADD     r2, r2, #4
    ADD     r1, r1, #3

_memcpy2_memcpy_lastbytes:
    @; copy from r0=src to r1=dest, r2=length <= 3
    CMP      r2, #0
    IT       EQ
    BXEQ     lr                 @ return if length is 0

    LDRB    r3, [r1], #1
    STRB    r3, [r0], #1        @ execute when length is 1/2/3

    CMP     r2, #2
    ITT     CS                  @ execute when length is 2/3
    LDRBCS  r3, [r1], #1
    STRBCS  r3, [r0], #1

    CMP     r2, #3
    ITT     EQ                  @ execute when length is 3
    LDRBEQ  r3, [r1], #1
    STRBEQ  r3, [r0], #1
    BX      lr
 
    .global __aeabi_memcpy4
    .type __aeabi_memcpy4,STT_FUNC
memcpy_burst:

__aeabi_memcpy4:
    CMP     r2,#0x20            @ if (length >= 32)
    IT      CS
    STMDBCS r13!,{r4-r8,r14}    @ push six registers
    IT      CC
    STMDBCC r13!,{r4,r14}       @ else push two registers

    @; long burst or short burst
    SUBS    r2,r2,#0x20
    BCC     _memcpy4_lastbytes_short
_memcpy4_burst_long:
    @; burst-8
    ITTT      CS
    LDMIACS r1!,{r3-r8,r12,r14} @ (4x8) bytes
    STMIACS r0!,{r3-r8,r12,r14}
    SUBSCS  r2,r2,#0x20
    BCS     _memcpy4_burst_long

_memcpy4_lastbytes_long:
    LSLS    r12,r2,#0x1C
    ITT     CS
    LDMIACS r1!,{r3-r4,r12,r14} @ (4x4) bytes
    STMIACS r0!,{r3-r4,r12,r14}
    ITT    MI
    LDMIAMI r1!,{r3-r4}         @ (4x2) bytes
    STMIAMI r0!,{r3-r4}

    LDMIA   r13!,{r4-r8,r14}    @ pop six registers
    LSLS    r12,r2,#0x1E
    ITT     CS
    LDRCS   r3,[r1],#0x4        @ 4 bytes
    STRCS   r3,[r0],#0x4

    IT      EQ
    BXEQ    lr
    B       _memcpy_lastbytes_aligned

_memcpy4_lastbytes_short:
    LSLS    r12,r2,#0x1C
    ITT     CS
    LDMIACS r1!,{r3-r4,r12,r14} @ (4x4) bytes
    STMIACS r0!,{r3-r4,r12,r14}

    ITT    MI
    LDMIAMI r1!,{r3-r4}         @ (4x2) bytes
    STMIAMI r0!,{r3-r4}
    LDMIA   r13!,{r4,r14}       @ pop two registers

    LSLS    r12,r2,#0x1E
    ITT     CS
    LDRCS   r3,[r1],#0x4        @ 4 bytes
    STRCS   r3,[r0],#0x4

    IT      EQ
    BXEQ    lr

_memcpy_lastbytes_aligned:
    LSLS    r2,r2,#0x1F
    ITT     CS
    LDRHCS  r3,[r1],#0x2    @ 2 bytes
    STRHCS  r3,[r0],#0x2

    ITT     MI
    LDRBMI  r2,[r1],#0x1    @ 1 byte
    STRBMI  r2,[r0],#0x1
    BX      lr

    #if defined(MT2822)||defined(AB156X)
    .section ".tcm_code","ax"
    #else
    .section ".text.memset","ax"
    #endif
@; Memset/Memclr Improvement
@; Retarget ARMABI memset
@; extern _ARMABI void *memset(void * /*s*/, int /*c*/, size_t /*n*/);
    .global __wrap_memset
    .align 3
    .type __wrap_memset,STT_FUNC
__wrap_memset:
    AND     r3,r1,#0xFF
    MOV     r1,r2
    STMDB   r13!,{r0,r14}
    ORR     r2,r3,r3,lsl #0x8
    ORR     r2,r2,r2,lsl #0x10
    BL      _memset_8
    LDMIA   r13!,{r0,r14}
    BX      r14

    .global __aeabi_memset
    .type __aeabi_memset,STT_FUNC
__aeabi_memset:
    AND     r3,r2,#0xFF
    ORR     r2,r3,r3,lsl #0x8
    ORR     r2,r2,r2,lsl #0x10
    B       _memset_8

    .global __wrap_memclr
    .type __wrap_memclr,STT_FUNC
__wrap_memclr:
    MOV     r2,#0x0
_memset_8:                          @r0=>Target address, r1=>size(4-byte aligned), r2=>Store value
    CMP     r1,#0x4
    BCC     _memset_8_last_bytes
    ANDS    r12,r0,#0x3
    BEQ     _memset_w_8             @If size >= 4 and address aligned 4-byte, goto memset_w
    RSB     r12,r12,#0x4            @Else Store 1 ~ 3 bytes to make address align 4-byte
    CMP     r12,#0x2
    IT      NE
    STRBNE  r2,[r0],#0x1
    SUB     r1,r1,r12
    IT      GE
    STRHGE  r2,[r0],#0x2
    B       _memset_w_8             @Finally goto memset_w
_memset_8_last_bytes:                @If size < 4, use STRB store 1 ~ 3 bytes
    LSLS    r12,r1,#0x1F
    ITT     CS
    STRBCS  r2,[r0],#0x1
    STRBCS  r2,[r0],#0x1
    IT      MI
    STRBMI  r2,[r0],#0x1
    BX      r14

@; Rewrite memset function to issue longer burst if possible
    .global __aeabi_memclr4
    .type __aeabi_memclr4,STT_FUNC
    .global __aeabi_memclr8
    .type __aeabi_memclr8,STT_FUNC
    .global __rt_memclr_w
    .type __rt_memclr_w,STT_FUNC
__aeabi_memclr4:
__aeabi_memclr8:
__rt_memclr_w:
    MOV     r2,#0x0
_memset_w_8:                         @r0=>Target address, r1=>size(4-byte aligned), r2=>Store value
    STMDB   r13!,{r14}
    MOV     r3,r2
    MOV     r12,r2
    MOV     r14,r2
    SUBS    r1,r1,#0x20
    BCC     _memset_w_8_last_bytes
    STMDB   r13!,{r4-r7}            @If size >= 32, push 4 extra registers
    MOV     r4,r2
    MOV     r5,r2
    MOV     r6,r2
    MOV     r7,r2
_memset_w_8_store_32:
    STMIA   r0!,{r2-r7,r12,r14}     @And issue STM8 (longest possible data transaction)
    SUBS    r1,r1,#0x20
    BCS     _memset_w_8_store_32
    LDMIA   r13!,{r4-r7}
_memset_w_8_last_bytes:              @Last 1 ~ 31 bytes process
    LSLS    r1,r1,#0x1C
    IT      CS
    STMIACS r0!,{r2-r3,r12,r14}     @Store 16bytes
    IT      MI
    STMIAMI r0!,{r2-r3}             @Store 8bytes
    LDMIA   r13!,{r14}
    LSLS    r1,r1,#0x2
    IT      CS
    STRCS   r2,[r0],#0x4            @Store 4bytes
    IT      EQ
    BXEQ    r14
    IT      MI
    STRHMI  r2,[r0],#0x2            @Store 2bytes
    TST     r1,#0x40000000
    IT      NE
    STRBNE  r2,[r0],#0x1            @Store 1byte
    BX      r14


    .end
