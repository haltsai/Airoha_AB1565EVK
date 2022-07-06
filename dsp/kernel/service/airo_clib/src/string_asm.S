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

#define FEA_OSMEM_SPEEDUP
  // ***************************************************************************
  // * Global symbol
  // ***************************************************************************
     .global memset
     .global memcpy
     .global memmove
     .global memcmp
  // ***************************************************************************
  // * External symbol
  // ***************************************************************************

////////////////////////////////////////////////////////////////////////////////
// macro ///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
.macro dlist_append ax , ay
                l32i    a4 , \ay , 0
                s32i    \ay, \ax , 4
                s32i    a4 , \ax , 0
                s32i    \ax, a4  , 4
                s32i    \ax, \ay , 0
.endm


.macro dlist_prepend ax , ay
                s32i.n	\ay, \ax, 0
                l32i.n	 a4, \ay, 4
                s32i.n	 a4, \ax, 4
                s32i.n	\ax, \ay, 4
                s32i.n	\ax,  a4, 0
.endm


.macro dlist_remove ax 
                l32i	a3 , \ax, 0
                l32i	a5 , \ax, 4
                s32i	a5 , a3 , 4
                l32i	a4 , \ax, 4
                s32i	a3 , a4 , 0
                s32i	\ax, \ax, 0
                s32i	\ax, \ax, 4
.endm
//***************************************************************************
//* VOID OS_memset(
//*   IN      OS_DATA_PTR dst,
//*   IN      U8 value,
//*   IN      U16 num);
//***************************************************************************
#ifdef FEA_OSMEM_SPEEDUP

.section ".text.memset", "ax"        
.align 4
memset:
                //a2 (dst)
                //a3 (value)
                //a4 (num)
                beqz      a2 , os_memset_dead1
                beqz      a4 , os_memset_dead1

                mov       a8 , a2
                
                movi      a5 , 3
                and       a6 , a2 , a5                
                beqz      a6 , os_memset_body
os_memset_head:
                movi      a7 , 4
                sub       a7 , a7 , a6
                sub       a4 , a4 , a7
                loopnez   a7 , os_memset_body
                s8i       a3 , a2 , 0
                addi      a2 , a2 , 1                                   
os_memset_body:
                //its 4-byte align here
                slli      a5 , a3 , 8
                add       a5 , a5 , a3
                slli      a5 , a5 , 8
                add       a5 , a5 , a3
                slli      a5 , a5 , 8
                add       a5 , a5 , a3
                
                srli	  a7 , a4 , 2
                slli      a6 , a7 , 2
                sub       a4 , a4 , a6    

                addi      a2 , a2 , -4
                ae_cvtq48a32s aeq0, a5
                
                loopnez   a7 , os_memset_body1
                ae_sq32f.iu aeq0, a2, 4                
                //s32i      a5 , a2 , 0
                //addi      a2 , a2 , 4
os_memset_body1:
                addi      a2 , a2 , 4
os_memset_tail:
                loopnez   a4 , os_memset_exit
                s8i       a3 , a2 , 0
                addi      a2 , a2 , 1                                   

os_memset_exit:
                mov       a2 , a8
                ret   
os_memset_dead1:
                movi a5,0
                wsr a5,INTENABLE
                rsync    
os_memset_dead2:
os_memset_dead3:
                j os_memset_dead3
                
//***************************************************************************
//* VOID OS_memcpy(
//*   IN      OS_DATA_PTR dst,
//*   IN      OS_DATA_PTR src,
//*   IN      U16 num);
//***************************************************************************     
.section ".text.memcpy", "ax"    
.align 4
memcpy:
                //a2 (dst)
                //a3 (src)
                //a4 (num)
                beqz      a2 , os_memcpy_dead1
                beqz      a3 , os_memcpy_dead1
                beqz      a4 , os_memcpy_dead1

                /* save return value */
                mov       a8 , a2

                /* get diff between dst and src */
                movi      a5 , 3
                and       a6 , a2 , a5
                and       a7 , a3 , a5
                beq       a6 , a7 , os_memcpy_4byte_head
                bge       a7 , a6 , os_memcpy1
                sub       a5 , a6 , a7
                j         os_memcpy_body 
os_memcpy1:
                sub       a5 , a7 , a6
                
os_memcpy_body:
                beqi      a5 , 1 , os_memcpy_1byte
                beqi      a5 , 3 , os_memcpy_1byte
                beqi      a5 , 2 , os_memcpy_2byte_head

os_memcpy_1byte:  
                /* if diff = 1 and 3 , move 1 byte once */
                loopnez   a4 , os_memcpy_1byte_exit               
                l8ui      a5 , a3 , 0
                s8i       a5 , a2 , 0
                addi      a2 , a2 , 1                                   
                addi      a3 , a3 , 1         
os_memcpy_1byte_exit:
                mov       a2 , a8
                ret 

os_memcpy_2byte_head:
                /* if diff = 2 , move 2 byte once */
                movi      a5 , 1
                and       a6 , a2 , a5
                beqz      a6 , os_memcpy_2byte_body

                l8ui      a5 , a3 , 0
                s8i       a5 , a2 , 0
                addi      a2 , a2 , 1                                   
                addi      a3 , a3 , 1
                addi      a4 , a4 , -1
                
os_memcpy_2byte_body:
                //its 2-byte align here                
                srli	  a7 , a4 , 1
                slli      a6 , a7 , 1
                sub       a4 , a4 , a6

                addi      a3 , a3 , -2
                addi      a2 , a2 , -2
                
                loopnez   a7 , os_memcpy_2byte_body1
                ae_lp16f.iu     aep0, a3, 2
                ae_sp16f.l.iu	aep0, a2, 2
                //l16ui     a5 , a3 , 0
                //s16i      a5 , a2 , 0
                //addi      a2 , a2 , 2
                //addi      a3 , a3 , 2
os_memcpy_2byte_body1:
                addi      a3 , a3 , 2
                addi      a2 , a2 , 2                

os_memcpy_2byte_tail:
                beqz      a4 , os_memcpy_exit
                l8ui      a5 , a3 , 0
                s8i       a5 , a2 , 0
                mov       a2 , a8
                ret

os_memcpy_4byte_head:
                /* if diff = 0 , move 4 byte once */
                movi      a5 , 3
                and       a6 , a2 , a5                
                beqz      a6 , os_memcpy_4byte_body


                movi      a7 , 4
                sub       a7 , a7 , a6
				min       a7 , a7 , a4                
                sub       a4 , a4 , a7
                loopnez   a7 , os_memcpy_4byte_body
                l8ui      a5 , a3 , 0
                s8i       a5 , a2 , 0
                addi      a2 , a2 , 1                                   
                addi      a3 , a3 , 1
                
os_memcpy_4byte_body:
                //its 4-byte align here                
                srli	  a7 , a4 , 2
                slli      a6 , a7 , 2
                sub       a4 , a4 , a6

                addi      a3 , a3 , -4
                addi      a2 , a2 , -4
                
                loopnez   a7 , os_memcpy_4byte_body1                
                
                ae_lq32f.iu aeq0, a3, 4
                ae_sq32f.iu aeq0, a2, 4
                //l32i      a5 , a3 , 0
                //s32i      a5 , a2 , 0
                //addi      a2 , a2 , 4
                //addi      a3 , a3 , 4

os_memcpy_4byte_body1:
                addi      a3 , a3 , 4
                addi      a2 , a2 , 4                
os_memcpy_4byte_tail:
                loopnez   a4 , os_memcpy_exit
                l8ui      a5 , a3 , 0
                s8i       a5 , a2 , 0
                addi      a2 , a2 , 1                                   
                addi      a3 , a3 , 1                                
os_memcpy_exit:
                mov       a2 , a8
                ret   
os_memcpy_dead1:
                movi      a5 , 0
                wsr       a5 , INTENABLE
                rsync  
                s32i      a0 , a1 , 0
os_memcpy_dead2:
                j os_memcpy_dead2

//***************************************************************************
//* VOID OS_memmove(
//*   IN      OS_DATA_PTR dst,
//*   IN      OS_DATA_PTR src,
//*   IN      U16 num);
//***************************************************************************     
.section ".text.memmove", "ax"    
.align 4
memmove:
                //a2 (dst)
                //a3 (src)
                //a4 (num)
                beqz      a2 , OS_memmove_dead1
                beqz      a3 , OS_memmove_dead1
                beqz      a4 , OS_memmove_dead1

                /* save return value */
                mov       a8 , a2

                /* move address to end */
                //addi      a5 , a4 , -1
                add       a2 , a2 , a4
                add       a3 , a3 , a4
                
                /* get diff between dst and src */
                movi      a5 , 3
                and       a6 , a2 , a5
                and       a7 , a3 , a5
                beq       a6 , a7 , OS_memmove_4byte_head
                bge       a7 , a6 , OS_memmove1
                sub       a5 , a6 , a7
                j         OS_memmove_body 
OS_memmove1:
                sub       a5 , a7 , a6
                
OS_memmove_body:
                beqi      a5 , 1 , OS_memmove_1byte
                beqi      a5 , 3 , OS_memmove_1byte
                beqi      a5 , 2 , OS_memmove_2byte_head

OS_memmove_1byte:  
                //addi      a2 , a2 , 1
                //addi      a3 , a3 , 1
                
                /* if diff = 1 and 3 , move 1 byte once */
                loopnez   a4 , OS_memmove_1byte_exit               
                addi      a2 , a2 , -1
                addi      a3 , a3 , -1
                l8ui      a5 , a3 , 0
                s8i       a5 , a2 , 0

OS_memmove_1byte_exit:                 
                mov       a2 , a8
                ret 

OS_memmove_2byte_head:
                /* if diff = 2 , move 2 byte once */
                movi      a5 , 1
                and       a6 , a2 , a5
                beqz      a6 , OS_memmove_2byte_body

                addi      a2 , a2 , -1
                addi      a3 , a3 , -1
                addi      a4 , a4 , -1
                l8ui      a5 , a3 , 0
                s8i       a5 , a2 , 0

                
OS_memmove_2byte_body:
                //its 2-byte align here                
                srli	  a7 , a4 , 1
                slli      a6 , a7 , 1
                sub       a4 , a4 , a6

                //addi      a3 , a3 , 2
                //addi      a2 , a2 , 2
                
                loopnez   a7 , OS_memmove_2byte_body1
                ae_lp16f.iu     aep0, a3, -2
                ae_sp16f.l.iu	aep0, a2, -2
                //l16ui     a5 , a3 , 0
                //s16i      a5 , a2 , 0
                //addi      a2 , a2 , 2
                //addi      a3 , a3 , 2
OS_memmove_2byte_body1:
                //addi      a3 , a3 , -2
                //addi      a2 , a2 , -2                

OS_memmove_2byte_tail:
                beqz      a4 , OS_memmove_exit
                addi      a2 , a2 , -1
                addi      a3 , a3 , -1                
                l8ui      a5 , a3 , 0
                s8i       a5 , a2 , 0
                mov       a2 , a8
                ret

OS_memmove_4byte_head:
                //addi      a2 , a2 , 1
                //addi      a3 , a3 , 1
                /* if diff = 0 , move 4 byte once */
                movi      a5 , 3
                and       a6 , a2 , a5                
                beqz      a6 , OS_memmove_4byte_body

                movi      a7 , 0
                sub       a7 , a6 , a7
                min       a7 , a7 , a4
                sub       a4 , a4 , a7
                loopnez   a7 , OS_memmove_4byte_body
                addi      a2 , a2 , -1
                addi      a3 , a3 , -1                
                l8ui      a5 , a3 , 0
                s8i       a5 , a2 , 0

                
OS_memmove_4byte_body:
                //its 4-byte align here                
                srli	  a7 , a4 , 2
                slli      a6 , a7 , 2
                sub       a4 , a4 , a6

                //addi      a3 , a3 , 4
                //addi      a2 , a2 , 4
                
                loopnez   a7 , OS_memmove_4byte_body1                
                
                ae_lq32f.iu aeq0, a3, -4
                ae_sq32f.iu aeq0, a2, -4
                //l32i      a5 , a3 , 0
                //s32i      a5 , a2 , 0
                //addi      a2 , a2 , 4
                //addi      a3 , a3 , 4

OS_memmove_4byte_body1:
                //addi      a3 , a3 , -4
                //addi      a2 , a2 , -4                
OS_memmove_4byte_tail:
                loopnez   a4 , OS_memmove_exit
                addi      a2 , a2 , -1
                addi      a3 , a3 , -1
                l8ui      a5 , a3 , 0
                s8i       a5 , a2 , 0
                            
OS_memmove_exit:
                mov       a2 , a8
                ret   
OS_memmove_dead1:
                movi      a5 , 0
                wsr       a5 , INTENABLE
                rsync
                s32i      a0 , a1 , 0
OS_memmove_dead2:
                j OS_memmove_dead2
//***************************************************************************
//* U8 OS_memcmp(
//*         OS_DATA_PTR ptr1,
//*         OS_DATA_PTR ptr2,
//*         U16 num);
//***************************************************************************     
.section ".text.memcmp", "ax"    
.align 4
memcmp:
                //a2 (dst)
                //a3 (src)
                //a4 (num)
                beqz      a2 , os_memcmp_dead1
                beqz      a3 , os_memcmp_dead1
                beqz      a4 , os_memcmp_dead1

                /* get diff between dst and src */
                movi      a5 , 3
                and       a6 , a2 , a5
                and       a7 , a3 , a5
                beq       a6 , a7 , OS_memcmp_4byte_head
                bge       a7 , a6 , OS_memcmp1
                sub       a5 , a6 , a7
                j         OS_memcmp_body 
OS_memcmp1:
                sub       a5 , a7 , a6
                
OS_memcmp_body:
                beqi      a5 , 1 , OS_memcmp_1byte
                beqi      a5 , 3 , OS_memcmp_1byte
                beqi      a5 , 2 , OS_memcmp_2byte_head

OS_memcmp_1byte:  
                /* if diff = 1 and 3 , move 1 byte once */
                loopnez   a4 , OS_memcmp_1byte_exit
                l8ui      a5 , a3 , 0
                l8ui      a6 , a2 , 0
                sub       a6 , a6 , a5
                bnez      a6 , os_memcmp_exit2
                addi      a2 , a2 , 1                                   
                addi      a3 , a3 , 1         
OS_memcmp_1byte_exit:
                j os_memcmp_exit1


OS_memcmp_2byte_head:
                /* if diff = 2 , move 2 byte once */
                movi      a5 , 1
                and       a6 , a2 , a5
                beqz      a6 , OS_memcmp_2byte_body

                l8ui      a5 , a3 , 0
                l8ui      a6 , a2 , 0
                sub       a6 , a6 , a5
                bnez      a6 , os_memcmp_exit2
                addi      a2 , a2 , 1                                   
                addi      a3 , a3 , 1
                addi      a4 , a4 , -1
                
OS_memcmp_2byte_body:
                //its 2-byte align here                
                srli	  a7 , a4 , 1
                slli      a6 , a7 , 1
                sub       a4 , a4 , a6                
                loopnez   a7 , OS_memcmp_2byte_tail
                l16ui     a5 , a3 , 0
                l16ui     a6 , a2 , 0
                sub       a6 , a6 , a5
                bnez      a6 , os_memcmp_exit2
                addi      a2 , a2 , 2
                addi      a3 , a3 , 2
                
OS_memcmp_2byte_tail:
                beqz      a4 , os_memcmp_exit1
                l8ui      a5 , a3 , 0
                l8ui      a6 , a2 , 0
                bne       a5 , a6 , os_memcmp_exit2
                j         os_memcmp_exit1

OS_memcmp_4byte_head:
                /* if diff = 0 , move 4 byte once */
                movi      a5 , 3
                and       a6 , a2 , a5                
                beqz      a6 , OS_memcmp_4byte_body

                movi      a7 , 4
                sub       a7 , a7 , a6
                min       a7 , a7 , a4
                sub       a4 , a4 , a7
                loopnez   a7 , OS_memcmp_4byte_body
                
                l8ui      a5 , a3 , 0
                l8ui      a6 , a2 , 0
                sub       a6 , a6 , a5
                bnez      a6 , os_memcmp_exit2
                addi      a2 , a2 , 1                                   
                addi      a3 , a3 , 1
OS_memcmp_4byte_body:
                //its 4-byte align here                
                srli	  a7 , a4 , 2
                slli      a6 , a7 , 2
                sub       a4 , a4 , a6                
                loopnez   a7 , OS_memcmp_4byte_tail
                l32i      a5 , a3 , 0
                l32i      a6 , a2 , 0
                sub       a6 , a6 , a5
                bnez      a6 , os_memcmp_exit2
                addi      a2 , a2 , 4  
                addi      a3 , a3 , 4
OS_memcmp_4byte_tail:
                loopnez   a4 , os_memcmp_exit1
                l8ui      a5 , a3 , 0
                l8ui      a6 , a2 , 0
                sub       a6 , a6 , a5
                bnez      a6 , os_memcmp_exit2
                addi      a2 , a2 , 1                                   
                addi      a3 , a3 , 1                                
os_memcmp_exit1 :
                movi      a2 , 0
                ret
os_memcmp_exit2 :
                mov       a2 , a6
                ret                  
os_memcmp_dead1:
                movi      a5 , 0
                wsr       a5 , INTENABLE
                rsync
                s32i      a0 , a1 , 0
os_memcmp_dead2:
                j os_memcmp_dead2
                //end                                  

#endif
