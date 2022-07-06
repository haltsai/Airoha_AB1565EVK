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

.file "startup_ab156x.s"
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.equ    WDT_Base,           0xA2090000
.equ    WDT_Disable,        0x10
.equ    WDT_Length_Value,   0x02000012  /* 15.625*N << 16 | 0x12 = 8s */
.equ    WDT_RST_Key,        0x1456789A
.equ    WDT_RST_Mask,       0x00260000
.equ    WDT1_Base,          0xA2090030
.equ    WDT1_Disable,       0x40
.equ    Remap_Base,         0xE0181000
.equ    Remap_Entry_HI0,    0x14200015
.equ    Remap_Entry_LO0,    0x04200000
.equ    Remap_Entry_HI1,    0x10000023
.equ    Remap_Entry_LO1,    0x0

.global  g_pfnVectors
.global  Default_Handler

/**
 * @brief  reset_handler is the entry point that processor starts to boot
 * @param  None
 * @retval : None
*/

  .section  .reset_handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:

/* set stack pointer */
  ldr  sp, =_stack_end

/* interrupt disable */
  cpsid i

/* config watch dog channel-0 */
#if 1
  ldr  r0, =WDT_Base
  ldr  r1, =WDT_Length_Value
  str  r1, [r0, #0x4]
  ldr  r1, =WDT_RST_Mask
  str  r1, [r0, #0x84]
  ldr  r1, =WDT_RST_Key
  str  r1, [r0, #0xC]
#else
  ldr  r0, =WDT_Base
  ldr  r1, =WDT_Disable
  str  r1, [r0, #0]
#endif /* MTK_BOOT_UP_DISABLE_WDT */

/* watch dog channel-1 disable */
  ldr  r0, =WDT1_Base
  ldr  r1, =WDT1_Disable
  str  r1, [r0, #0]

/* preinit cache to accelerate region init progress */
  bl CachePreInit

/* make virtual space available */
  ldr  r0, =Remap_Entry_HI0
  ldr  r1, =Remap_Entry_LO0
  ldr  r3, =Remap_Entry_HI1
  ldr  r4, =Remap_Entry_LO1
  ldr  r2, =Remap_Base
  str  r0, [r2], #4
  str  r1, [r2], #4
  str  r3, [r2], #4
  str  r4, [r2, #0]

/* stack space zero init */
  movs  r0, #0
  ldr  r1, =_stack_start
  ldr  r2, =_stack_end
FillZero:
  str  r0, [r1], #4
  cmp  r1, r2
  blo  FillZero

/* tcm section init */
  ldr  r1, =_tcm_text_load
  ldr  r2, =_tcm_text_start
  ldr  r3, =_tcm_text_end
  bl  Data_Init

  ldr  r2, =_tcm_zi_start
  ldr  r3, =_tcm_zi_end
  bl  Bss_Init

/* sysram_text section init */
  ldr  r1, =_sysram_code_load
  ldr  r2, =_sysram_code_start
  ldr  r3, =_sysram_code_end
  bl  Data_Init

/* cached_sysram_data section init */
  ldr  r1, =_cached_sysram_data_load
  ldr  r2, =_cached_sysram_data_start
  ldr  r3, =_cached_sysram_data_end
  bl  Data_Init

/* noncached_sysram_data section init */
  ldr  r1, =_noncached_sysram_rw_load
  ldr  r2, =_noncached_sysram_rw_start
  ldr  r3, =_noncached_sysram_rw_end
  bl  Data_Init

/* share data section init */
  ldr  r1, =_share_data_load
  ldr  r2, =_share_data_start
  ldr  r3, =_share_data_end
  bl  Data_Init

/* bt share data section init */
  ldr  r1, =_bt_sysram_rw_data_load
  ldr  r2, =_bt_sysram_rw_data_start
  ldr  r3, =_bt_sysram_rw_data_end
  bl  Data_Init

/* cached_sysram_zidata section init */
  ldr  r2, =_cached_sysram_bss_start
  ldr  r3, =_cached_sysram_bss_end
  bl  Bss_Init

/* noncached_sysram_zidata section init */
  ldr  r2, =_noncached_sysram_zi_start
  ldr  r3, =_noncached_sysram_zi_end
  bl  Bss_Init

/* share zidata section init */
  ldr  r2, =_share_bss_start
  ldr  r3, =_share_bss_end
  bl  Bss_Init

/* bt share zidata section init */
  ldr  r2, =_bt_sysram_zi_data_start
  ldr  r3, =_bt_sysram_zi_data_end
  bl  Bss_Init

/* Call the clock system intitialization function.*/
  ldr r0, =SystemInit
  blx r0

/* Call the application's entry point.*/
  ldr r0, =main
  bx r0
  bx  lr
.size  Reset_Handler, .-Reset_Handler

/**
 * @brief  This is data init sub-function
 * @param  None
 * @retval None
*/
  .section  .reset_handler.Data_Init,"ax",%progbits
Data_Init:
CopyDataLoop:
  cmp     r2, r3
  ittt    lo
  ldrlo   r0, [r1], #4
  strlo   r0, [r2], #4
  blo     CopyDataLoop
  bx  lr
  .size  Data_Init, .-Data_Init

/**
 * @brief  This is bss init sub-function
 * @param  None
 * @retval None
*/
  .section  .reset_handler.Bss_Init,"ax",%progbits
Bss_Init:
ZeroBssLoop:
  cmp     r2, r3
  ittt    lo
  movlo   r0, #0
  strlo   r0, [r2], #4
  blo     ZeroBssLoop
  bx  lr
  .size  Bss_Init, .-Bss_Init

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 * @param  None
 * @retval None
*/
  .section  .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b  Infinite_Loop
  .size  Default_Handler, .-Default_Handler


/******************************************************************************
*
* The minimal vector table for a Cortex M4. Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
*******************************************************************************/
  .section  .isr_vector,"a",%progbits
  .type  g_pfnVectors, %object
  .size  g_pfnVectors, .-g_pfnVectors


g_pfnVectors:
  .word  _stack_end
  .word  Reset_Handler
  .word  NMI_Handler
  .word  HardFault_Handler
  .word  MemManage_Handler
  .word  BusFault_Handler
  .word  UsageFault_Handler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  SVC_Handler
  .word  DebugMon_Handler
  .word  0
  .word  PendSV_Handler
  .word  SysTick_Handler

/* External Interrupts */
  .word     isrC_main     /*16:  OS GPT         */
  .word     isrC_main     /*17:  MCU DMA        */
  .word     isrC_main     /*18:  I2C DMA        */
  .word     isrC_main     /*19:  SPI master 0   */
  .word     isrC_main     /*20:  SPI master 1   */
  .word     isrC_main     /*21:  SPI master 2   */
  .word     isrC_main     /*22:  SPI slave      */
  .word     isrC_main     /*23:  SDIO master    */
  .word     isrC_main     /*24:  SDIO master    */
  .word     isrC_main     /*25:  SDIO master    */
  .word     isrC_main     /*26:  SDIO master    */
  .word     isrC_main     /*27:  TRNG           */
  .word     isrC_main     /*28:  CRYPTO_ENGINE  */
  .word     isrC_main     /*29:  UART0          */
  .word     isrC_main     /*30:  UART1          */
  .word     isrC_main     /*31:  UART2          */
  .word     isrC_main     /*32:  I2C0           */
  .word     isrC_main     /*33:  I2C1           */
  .word     isrC_main     /*34:  I2C2           */
  .word     isrC_main     /*35:  I2C_AO         */
  .word     isrC_main     /*36:  RTC            */
  .word     isrC_main     /*37:  GPTIMER        */
  .word     isrC_main     /*38:  SPM            */
  .word     isrC_main     /*39:  RGU            */
  .word     isrC_main     /*40:  EINT           */
  .word     isrC_main     /*41:  SFC            */
  .word     isrC_main     /*42:  USB            */
  .word     isrC_main     /*43:  IRRX           */
  .word     isrC_main     /*44:  DSP0 IRQ gen   */
  .word     isrC_main     /*45:  DSP1 IRQ gen   */
  .word     isrC_main     /*46:  N9 IRQ gen     */
  .word     isrC_main     /*47:  Cap_touch      */
  .word     isrC_main     /*48:  audiosys       */
  .word     isrC_main     /*49:  audiosys       */
  .word     isrC_main     /*50:  audiosys       */
  .word     isrC_main     /*51:  ANC            */
  .word     isrC_main     /*52:  I2S DMA        */
  .word     isrC_main     /*53:  I2S0           */
  .word     isrC_main     /*54:  I2S1           */
  .word     isrC_main     /*55:  I2S2           */
  .word     isrC_main     /*56:  I2S3           */
  .word     isrC_main     /*57:  Security       */
  .word     isrC_main     /*58:  CM4 reserved   */
  .word     isrC_main     /*59:  CM4 reserved   */


/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

  .weak      NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak      HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak      MemManage_Handler
  .thumb_set MemManage_Handler,Default_Handler

  .weak      BusFault_Handler
  .thumb_set BusFault_Handler,Default_Handler

  .weak      UsageFault_Handler
  .thumb_set UsageFault_Handler,Default_Handler

  .weak      SVC_Handler
  .thumb_set SVC_Handler,Default_Handler

  .weak      DebugMon_Handler
  .thumb_set DebugMon_Handler,Default_Handler

  .weak      PendSV_Handler
  .thumb_set PendSV_Handler,Default_Handler

  .weak      SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler




