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

#include "hal_irrx_internal.h"
#include "hal_nvic.h"
#include "hal_sleep_manager_platform.h"

#ifdef HAL_IRRX_MODULE_ENABLED

static hal_irrx_callback_t irrx_callback;

void irrx_power_set(bool is_power_on)
{
    if (true == is_power_on) {
        hal_clock_enable(HAL_CLOCK_CG_IRRX_26M);
        hal_clock_enable(HAL_CLOCK_CG_IRRX);
    } else {
        hal_clock_disable(HAL_CLOCK_CG_IRRX_26M);
        hal_clock_disable(HAL_CLOCK_CG_IRRX);
    }
}

void irrx_nvic_set(bool enable)
{
    hal_nvic_irq_t irq_number;

    irq_number = IRRX_IRQn;

    if (enable) {
        hal_nvic_enable_irq(irq_number);
    } else {
        hal_nvic_disable_irq(irq_number);
    }
}

void irrx_wakeup_callback(void *user_data)
{
    IRRX_REGISTER_T *irrx_register_base = IRRX_REG;

#ifdef IRRX_DEBUG
    IRRX_DEBUG_LOG("irrx wakeup event\r\n");
#endif

    /*config the power done out*/
    irrx_register_base->PDREG_PDWN_OUT = 0x0;

    /*clear wake up status*/
    irrx_register_base->PDREG_WAKECLR |= 0x01;

    /*close the wakeup */
    irrx_register_base->PDREG_WAKEEN |= 0x0;
}

void irrx_isr(hal_nvic_irq_t irq_number)
{
    IRRX_REGISTER_T *irrx_register_base = IRRX_REG;

    irrx_nvic_set(false);

#ifdef IRRX_DEBUG
    IRRX_DEBUG_LOG("irrx_isr interrupt trigger!\r\n");
#endif

    if ((IRRX_IRQn == irq_number) && (irrx_callback)) {
        /*check data valid*/
        /*check IRM IRL data is 0*/
        if ((0 == irrx_register_base->PDREG_IRM) && (0 == irrx_register_base->PDREG_IRL)) {
#ifdef IRRX_DEBUG
            IRRX_DEBUG_LOG("irrx_isr:invalid data\r\n");
#endif
            irrx_nvic_set(true);

            /*clear interrupt*/
            irrx_register_base->PDREG_IR_INTCLR |= PDREG_IR_INTCLR_IR_INTCLR_MASK;
            irrx_register_base->PDREG_IRCLR |= PDREG_IRCLR_IRCLR_MASK;

            return;
        }
        /*check bit cnt valid*/
        if ((irrx_register_base->PDREG_IRH & PDREG_IRH_BIT_CNT_MASK) == 0) {
#ifdef IRRX_DEBUG
            IRRX_DEBUG_LOG("irrx_isr:invalid cnt\r\n");
#endif
            irrx_nvic_set(true);


            /*clear interrupt*/
            irrx_register_base->PDREG_IR_INTCLR |= PDREG_IR_INTCLR_IR_INTCLR_MASK;
            irrx_register_base->PDREG_IRCLR |= PDREG_IRCLR_IRCLR_MASK;

            return;
        }
        /*check NEC potocol*/

        /*check RC5 potocol*/
        irrx_callback(HAL_IRRX_EVENT_TRANSACTION_SUCCESS, NULL);
    }

    irrx_nvic_set(true);
}

void irrx_register_callback(hal_irrx_callback_t callback)
{
    if (HAL_NVIC_STATUS_OK != hal_nvic_register_isr_handler(IRRX_IRQn, irrx_isr)) {
#ifdef IRRX_DEBUG
        IRRX_DEBUG_LOG("irrx_register_callback:error!\r\n");
#endif
        return;
    }
    irrx_callback = callback;
}

void irrx_read_hardware_mode_data(uint8_t *data, uint8_t length)
{
    IRRX_REGISTER_T *irrx_register_base = IRRX_REG;
    uint8_t *pdata = data;
    uint8_t i;
    uint32_t tmp = 0;

    if ((!data) || (length == 0)) {
        log_hal_msgid_info("irrx_hardware_read_data param err!\r\n", 0);
        return;
    }

    tmp = irrx_register_base->PDREG_IRM;
    *(uint32_t *)pdata = tmp;
    pdata += 4;
    tmp = irrx_register_base->PDREG_IRL;
    *(uint32_t *)pdata = tmp;
}

void irrx_byte_bit_inverse(uint8_t *pdata, uint8_t inverse_byte_number)
{
    uint8_t data = 0;
    uint8_t tmp = 0;
    uint8_t i;

    for (i = 0; i < inverse_byte_number; i++) {
        data = *(pdata + i);
        tmp = ~data;
        *(pdata + i) = tmp;
    }
}

uint8_t irrx_byte_bit_shift(uint8_t data)
{
    data = (data << 4) | (data >> 4);
    data = ((data << 2) & 0xcc) | ((data >> 2) & 0x33);
    data = ((data << 1) & 0xaa) | ((data >> 1) & 0x55);

    return data;
}

void irrx_decode_sirc_12bit_data(uint8_t *data, uint8_t length, irrx_sirc_bit12_protocol_t *protocol)
{
    /*decode potocol*/
    protocol->cmd  = irrx_byte_bit_shift((*data) & 0x7F);
    protocol->addr = irrx_byte_bit_shift(((*data) & 0x80) | (((*(data + 1)) & 0xF0) >> 1));

    IRRX_DEBUG_LOG("SIRC 12bit decode:cmd = 0x%02x\r\n", protocol->cmd);
    IRRX_DEBUG_LOG("SIRC 12bit decode:addr = 0x%02x\r\n", protocol->addr);
}

void irrx_decode_sirc_15bit_data(uint8_t *data, uint8_t length, irrx_sirc_bit15_protocol_t *protocol)
{
    /*decode potocol*/
    protocol->cmd  = irrx_byte_bit_shift((*data) & 0x7F);
    protocol->addr = irrx_byte_bit_shift(((*data) & 0x80) | (((*(data + 1)) & 0xF0) >> 1));

    IRRX_DEBUG_LOG("SIRC 15bit decode:cmd = 0x%02x\r\n", protocol->cmd);
    IRRX_DEBUG_LOG("SIRC 15bit decode:addr = 0x%02x\r\n", protocol->addr);
}

void irrx_decode_sirc_20bit_data(uint8_t *data, uint8_t length, irrx_sirc_bit20_protocol_t *protocol)
{
    /*decode potocol*/
    protocol->cmd  = irrx_byte_bit_shift((*data) & 0x7F);
    protocol->addr = irrx_byte_bit_shift(((*data) & 0x80) | (((*(data + 1)) & 0xF0) >> 1));
    protocol->extend = irrx_byte_bit_shift((((*(data + 1)) & 0x0F) << 4) | (((*(data + 2)) & 0xF0) >> 4));

    IRRX_DEBUG_LOG("SIRC 20bit decode:cmd = 0x%02x\r\n", protocol->cmd);
    IRRX_DEBUG_LOG("SIRC 20bit decode:addr = 0x%02x\r\n", protocol->addr);
    IRRX_DEBUG_LOG("SIRC 20bit decode:extend = 0x%02x\r\n", protocol->extend);
}

void irrx_decode_nec_data(uint8_t *data, uint8_t length, irrx_nec_protocol_t *protocol)
{
    /*decode potocol*/
    protocol->addr = *data;
    protocol->addr_inverse = *(data + 1);
    protocol->cmd = *(data + 2);
    protocol->cmd_inverse = *(data + 3);

    IRRX_DEBUG_LOG("NEC decode:addr = 0x%02x\r\n", protocol->addr);
    IRRX_DEBUG_LOG("NEC decode:addr_inverse = 0x%02x\r\n", protocol->addr_inverse);
    IRRX_DEBUG_LOG("NEC decode:cmd = 0x%02x\r\n", protocol->cmd);
    IRRX_DEBUG_LOG("NEC decode:cmd = 0x%02x\r\n", protocol->cmd_inverse);
}

void irrx_decode_rc5_data(uint8_t *data, uint8_t length, irrx_rc5_protocol_t *protocol)
{
    /*decode potocol*/
    protocol->toggle_bit = ((*data) & 0x80) >> 7;
    protocol->addr = ((*data) & 0x7c) >> 2;
    protocol->cmd = (((*data) & 0x03) << 4) | (((*(data + 1)) & 0xf0) >> 4);

    IRRX_DEBUG_LOG("RC5 decode:toggle bit = 0x%02x\r\n", protocol->toggle_bit);
    IRRX_DEBUG_LOG("RC5 decode:addr = 0x%02x\r\n", protocol->addr);
    IRRX_DEBUG_LOG("RC5 decode:cmd = 0x%02x\r\n", protocol->cmd);
}

void irrx_decode_rc6_data(uint8_t *data, uint8_t length, irrx_rc6_mode0_protocol_t *protocol)
{
    /*decode potocol*/
    protocol->header = ((*data) & 0xfc) >> 2;
    protocol->ctrl = (((*data) & 0x03) << 6) | (((*(data + 1)) & 0xfc) >> 2);
    protocol->info = (((*data + 1) & 0x03) << 6) | (((*(data + 3)) & 0xfc) >> 2);

    IRRX_DEBUG_LOG("RC6 decode:header bit = 0x%02x\r\n", protocol->header);
    IRRX_DEBUG_LOG("RC6 decode:ctrl = 0x%02x\r\n", protocol->ctrl);
    IRRX_DEBUG_LOG("RC6 decode:info = 0x%02x\r\n", protocol->info);
}

void irrx_decode_rcmm_data(uint8_t *data, uint8_t length, irrx_rcmm_protocol_t *protocol)
{
    uint32_t i;

    /*decode potocol*/
    protocol->length = length;
    for (i = 0; i < length; i++) {
        (protocol->data)[i] = data[i];
    }

    IRRX_DEBUG_LOG("RCMM decode:length = 0x%02x\r\n", protocol->length);
    for (i = 0; i < length; i++) {
        IRRX_DEBUG_LOG("RCMM decode:data[%d] = 0x%02x\r\n", i, (protocol->data)[i]);
    }
}

void irrx_register_dump(void)
{
    IRRX_REGISTER_T *irrx_register_base = IRRX_REG;

    IRRX_DEBUG_LOG("irrx register dump:\r\n");
    IRRX_DEBUG_LOG("PDREG_IRH = 0x%08x\r\n", irrx_register_base->PDREG_IRH);
    IRRX_DEBUG_LOG("PDREG_IRM = 0x%08x\r\n", irrx_register_base->PDREG_IRM);
    IRRX_DEBUG_LOG("PDREG_IRL = 0x%08x\r\n", irrx_register_base->PDREG_IRL);
    IRRX_DEBUG_LOG("PDREG_IRCFGH = 0x%08x\r\n", irrx_register_base->PDREG_IRCFGH);
    IRRX_DEBUG_LOG("PDREG_IRCFGL = 0x%08x\r\n", irrx_register_base->PDREG_IRCFGL);
    IRRX_DEBUG_LOG("PDREG_IRTHD = 0x%08x\r\n", irrx_register_base->PDREG_IRTHD);
    IRRX_DEBUG_LOG("PDREG_IRRCM_THD = 0x%08x\r\n", irrx_register_base->PDREG_IRRCM_THD);
    IRRX_DEBUG_LOG("PDREG_IRRCM_THD_0 = 0x%08x\r\n", irrx_register_base->PDREG_IRRCM_THD_0);
    IRRX_DEBUG_LOG("PDREG_IRCLR = 0x%08x\r\n", irrx_register_base->PDREG_IRCLR);
    IRRX_DEBUG_LOG("PDREG_IREXP_EN = 0x%08x\r\n", irrx_register_base->PDREG_IREXP_EN);
    IRRX_DEBUG_LOG("PDREG_EXP_BCNT = 0x%08x\r\n", irrx_register_base->PDREG_EXP_BCNT);
    IRRX_DEBUG_LOG("PDREG_ENEXP_IRM = 0x%08x\r\n", irrx_register_base->PDREG_ENEXP_IRM);
    IRRX_DEBUG_LOG("PDREG_ENEXP_IRL = 0x%08x\r\n", irrx_register_base->PDREG_ENEXP_IRL);
    IRRX_DEBUG_LOG("PDREG_EXP_IRL0 = 0x%08x\r\n", irrx_register_base->PDREG_EXP_IRL0);
    IRRX_DEBUG_LOG("PDREG_EXP_IRM0 = 0x%08x\r\n", irrx_register_base->PDREG_EXP_IRM0);
    IRRX_DEBUG_LOG("PDREG_IR_INTCLR = 0x%08x\r\n", irrx_register_base->PDREG_IR_INTCLR);
    //IRRX_DEBUG_LOG("PDREG_WAKEEN = 0x%08x\r\n",irrx_register_base->PDREG_WAKEEN);
}

#endif

