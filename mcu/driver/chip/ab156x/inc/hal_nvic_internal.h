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

#ifndef _HAL_NVIC_INTERNAL_H_
#define _HAL_NVIC_INTERNAL_H_
#include "hal_nvic.h"

#ifdef HAL_NVIC_MODULE_ENABLED
/* Please refer to configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY= 1 in freertos_config.h */
/* DEFAULT_IRQ_PRIORITY is from configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY  to  configKERNEL_INTERRUPT_PRIORITY -1 = (0xFF - 1)
    as pend_sv should be lowest irq priority to avoid unneccessary freert_os schedule */
#define DEFAULT_IRQ_PRIORITY                2//(FREERTOS_ISR_API_PRIORITY + 1)     /* please keep the setting */

#define RESERVED_IRQ_PRIORITY       (DEFAULT_IRQ_PRIORITY + 13)

#define OS_GPT_IRQ_PRIORITY         (DEFAULT_IRQ_PRIORITY + 2)
#define MCU_DMA_IRQ_PRIORITY        (DEFAULT_IRQ_PRIORITY + 4)
#define SLOW_DMA0_IRQ_PRIORITY      (DEFAULT_IRQ_PRIORITY + 4)
#define SLOW_DMA1_IRQ_PRIORITY      (DEFAULT_IRQ_PRIORITY + 4)
#define SPI_MST0_IRQ_PRIORITY       (DEFAULT_IRQ_PRIORITY + 11)
#define SPI_MST1_IRQ_PRIORITY       (DEFAULT_IRQ_PRIORITY + 11)
#define SPI_MST2_IRQ_PRIORITY       (DEFAULT_IRQ_PRIORITY + 11)
#define SPI_SLV_IRQ_PRIORITY        (DEFAULT_IRQ_PRIORITY + 11)
#define SDIO_MST0_IRQ_PRIORITY      (DEFAULT_IRQ_PRIORITY + 11)
#define UART0_IRQ_PRIORITY          (DEFAULT_IRQ_PRIORITY + 11)
#define UART1_IRQ_PRIORITY          (DEFAULT_IRQ_PRIORITY + 11)
#define UART2_IRQ_PRIORITY          (DEFAULT_IRQ_PRIORITY + 11)
#define CRYPTO_IRQ_PRIORITY         (DEFAULT_IRQ_PRIORITY + 12)
#define TRNG_IRQ_PRIORITY           (DEFAULT_IRQ_PRIORITY + 9)
#define I2S_SLAVE_IRQ_PRIORITY      (DEFAULT_IRQ_PRIORITY + 8)
#define I2C0_IRQ_PRIORITY           (DEFAULT_IRQ_PRIORITY + 11)
#define I2C1_IRQ_PRIORITY           (DEFAULT_IRQ_PRIORITY + 11)
#define I2C2_IRQ_PRIORITY           (DEFAULT_IRQ_PRIORITY + 11)
#define I2C_AO_IRQ_PRIORITY         (DEFAULT_IRQ_PRIORITY + 11)
#define RTC_IRQ_PRIORITY            (DEFAULT_IRQ_PRIORITY + 5)
#define GPT_IRQ_PRIORITY            (DEFAULT_IRQ_PRIORITY + 5)
#define SPM_IRQ_PRIORITY            (DEFAULT_IRQ_PRIORITY + 7)
#define WDT_IRQ_PRIORITY            (DEFAULT_IRQ_PRIORITY + 13)
#define EINT_IRQ_PRIORITY           (DEFAULT_IRQ_PRIORITY + 8)
#define SFC_IRQ_PRIORITY            (DEFAULT_IRQ_PRIORITY + 13)
#define ESC_IRQ_PRIORITY            (DEFAULT_IRQ_PRIORITY + 13)
#define USB_IRQ_PRIORITY            (DEFAULT_IRQ_PRIORITY + 3)
#define DPS0_IRQ_PRIORITY           (DEFAULT_IRQ_PRIORITY + 3)
#define CAP_TOUCH_IRQ_PRIORITY      (DEFAULT_IRQ_PRIORITY + 8)
#define AUDIOSYS0_IRQ_PRIORITY      (DEFAULT_IRQ_PRIORITY + 10)
#define AUDIOSYS1_IRQ_PRIORITY      (DEFAULT_IRQ_PRIORITY + 10)
#define AUDIOSYS2_IRQ_PRIORITY      (DEFAULT_IRQ_PRIORITY + 10)
#define ANC_IRQ_PRIORITY            (DEFAULT_IRQ_PRIORITY + 6)
#define VAD_IRQ_PRIORITY            (DEFAULT_IRQ_PRIORITY + 7)
#define BT_IRQ_PRIORITY             (DEFAULT_IRQ_PRIORITY + 0)
#define BT_AURX_IRQ_PRIORITY        (DEFAULT_IRQ_PRIORITY + 10)
#define BT_AUTX_IRQ_PRIORITY        (DEFAULT_IRQ_PRIORITY + 10)
#define BT_TIMER_IRQ_PRIORITY       (DEFAULT_IRQ_PRIORITY + 1)
#define VOW_SNR_IRQ_PRIORITY        (DEFAULT_IRQ_PRIORITY + 10)
#define VOW_FIFO_IRQ_PRIORITY       (DEFAULT_IRQ_PRIORITY + 10)
#define DSP_ERR_IRQ_PRIORITY        (DEFAULT_IRQ_PRIORITY + 4)
#define CM4_reserved0_IRQ_PRIORITY  (RESERVED_IRQ_PRIORITY)


uint32_t save_and_set_interrupt_mask(void);
void restore_interrupt_mask(uint32_t mask);
hal_nvic_status_t nvic_irq_software_trigger(hal_nvic_irq_t irq_number);
int32_t hal_nvic_query_exception_number(void);
hal_nvic_status_t hal_nvic_save_and_set_interrupt_mask_special(uint32_t *mask);
hal_nvic_status_t hal_nvic_restore_interrupt_mask_special(uint32_t mask);
#endif //HAL_NVIC_MODULE_ENABLED
#endif //_HAL_NVIC_INTERNAL_H_

