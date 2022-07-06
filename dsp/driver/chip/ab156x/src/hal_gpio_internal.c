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

#include "hal_gpio.h"

#ifdef HAL_GPIO_MODULE_ENABLED
#include "hal_gpio_internal.h"
#include "hal_log.h"


const hal_gpio_cfg_reg_t gpio_cfg_table[] = {
    //   DRV_REG shift   IES_REG   shift PD_REG      PU_REG    shift PUPD_REG    R0_REG      R1_REG     shift    RDSEL_RG  shift SMT_REG     SR_REG    shift
    {   0XA20C0000, 0 , 0XA20C0040, 0 , 0XA20C0070, 0XA20C00A0, 0 ,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00E0, 0 , 0XA20C0120, 0XA20C0140, 0 , },  // HAL_GPIO_0
    {   0XA20C0000, 2 , 0XA20C0040, 1 , 0XA20C0070, 0XA20C00A0, 1 ,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00E0, 2 , 0XA20C0120, 0XA20C0140, 1 , },  // HAL_GPIO_1
    {   0XA20C0000, 4 , 0XA20C0040, 2 , 0XA20C0070, 0XA20C00A0, 2 ,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00E0, 4 , 0XA20C0120, 0XA20C0140, 2 , },  // HAL_GPIO_2
    {   0XA20C0000, 6 , 0XA20C0040, 3 , 0XA20C0070, 0XA20C00A0, 3 ,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00E0, 6 , 0XA20C0120, 0XA20C0140, 3 , },  // HAL_GPIO_3
    {   0XA20C0000, 8 , 0XA20C0040, 4 , 0XA20C0070, 0XA20C00A0, 4 ,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00E0, 8 , 0XA20C0120, 0XA20C0140, 4 , },  // HAL_GPIO_4
    {   0XA20C0000, 10, 0XA20C0040, 5 , 0XA20C0070, 0XA20C00A0, 5 ,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00E0, 10, 0XA20C0120, 0XA20C0140, 5 , },  // HAL_GPIO_5
    {   0XA20C0000, 12, 0XA20C0040, 6 , 0XA20C0070, 0XA20C00A0, 6 ,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00E0, 12, 0XA20C0120, 0XA20C0140, 6 , },  // HAL_GPIO_6
    {   0XA20C0000, 14, 0XA20C0040, 7 , 0XA20C0070, 0XA20C00A0, 7 ,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00E0, 14, 0XA20C0120, 0XA20C0140, 7 , },  // HAL_GPIO_7
    {   0XA20C0000, 16, 0XA20C0040, 8 , 0XA20C0070, 0XA20C00A0, 8 ,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00E0, 16, 0XA20C0120, 0XA20C0140, 8 , },  // HAL_GPIO_8
    {   0XA20C0000, 18, 0XA20C0040, 9 , 0XA20C0070, 0XA20C00A0, 9 ,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00E0, 18, 0XA20C0120, 0XA20C0140, 9 , },  // HAL_GPIO_9
    {   0XA20C0000, 20, 0XA20C0040, 10, 0XA20C0070, 0XA20C00A0, 10,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00E0, 20, 0XA20C0120, 0XA20C0140, 10, },  // HAL_GPIO_10
    {   0XA20C0000, 22, 0XA20C0040, 11, 0XA20C0070, 0XA20C00A0, 11,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00E0, 22, 0XA20C0120, 0XA20C0140, 11, },  // HAL_GPIO_11
    {   0XA20C0000, 24, 0XA20C0040, 12, 0XA20C0070, 0XA20C00A0, 12,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00E0, 24, 0XA20C0120, 0XA20C0140, 12, },  // HAL_GPIO_12
    {   0XA20C0000, 26, 0XA20C0040, 13, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF, 0XA20C0090, 0XA20C00C0, 0XA20C00D0,   0 ,   0XA20C00E0, 26, 0XA20C0120, 0XA20C0140, 13, },  // HAL_GPIO_13
    {   0XA20C0000, 28, 0XA20C0040, 14, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF, 0XA20C0090, 0XA20C00C0, 0XA20C00D0,   1 ,   0XA20C00E0, 28, 0XA20C0120, 0XA20C0140, 14, },  // HAL_GPIO_14
    {   0XA20C0000, 30, 0XA20C0040, 15, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF, 0XA20C0090, 0XA20C00C0, 0XA20C00D0,   2 ,   0XA20C00E0, 30, 0XA20C0120, 0XA20C0140, 15, },  // HAL_GPIO_15
    {   0XA20C0010, 0 , 0XA20C0040, 16, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF, 0XA20C0090, 0XA20C00C0, 0XA20C00D0,   3 ,   0XA20C00F0, 0 , 0XA20C0120, 0XA20C0140, 16, },  // HAL_GPIO_16
    {   0XA20C0010, 2 , 0XA20C0040, 17, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF, 0XA20C0090, 0XA20C00C0, 0XA20C00D0,   4 ,   0XA20C00F0, 2 , 0XA20C0120, 0XA20C0140, 17, },  // HAL_GPIO_17
    {   0XA20C0010, 4 , 0XA20C0040, 18, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF, 0XA20C0090, 0XA20C00C0, 0XA20C00D0,   5 ,   0XA20C00F0, 4 , 0XA20C0120, 0XA20C0140, 18, },  // HAL_GPIO_18
    {   0XA20C0010, 6 , 0XA20C0040, 19, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF, 0XA20C0090, 0XA20C00C0, 0XA20C00D0,   6 ,   0XA20C00F0, 6 , 0XA20C0120, 0XA20C0140, 19, },  // HAL_GPIO_19
    {   0XA20C0010, 8 , 0XA20C0040, 20, 0XA20C0070, 0XA20C00A0, 13,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00F0, 8 , 0XA20C0120, 0XA20C0140, 20, },  // HAL_GPIO_20
    {   0XA20C0010, 10, 0XA20C0040, 21, 0XA20C0070, 0XA20C00A0, 14,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00F0, 10, 0XA20C0120, 0XA20C0140, 21, },  // HAL_GPIO_21
    {   0XA20C0010, 12, 0XA20C0040, 22, 0XA20C0070, 0XA20C00A0, 15,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00F0, 12, 0XA20C0120, 0XA20C0140, 22, },  // HAL_GPIO_22
    {   0XA20C0010, 14, 0XA20C0040, 23, 0XA20C0070, 0XA20C00A0, 16,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00F0, 14, 0XA20C0120, 0XA20C0140, 23, },  // HAL_GPIO_23
    {   0XA20C0010, 16, 0XA20C0040, 24, 0XA20C0070, 0XA20C00A0, 17,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00F0, 16, 0XA20C0120, 0XA20C0140, 24, },  // HAL_GPIO_24
    {   0XA20C0010, 18, 0XA20C0040, 25, 0XA20C0070, 0XA20C00A0, 18,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00F0, 18, 0XA20C0120, 0XA20C0140, 25, },  // HAL_GPIO_25
    {   0XA20C0010, 20, 0XA20C0040, 26, 0XA20C0070, 0XA20C00A0, 19,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00F0, 20, 0XA20C0120, 0XA20C0140, 26, },  // HAL_GPIO_26
    {   0XA20C0010, 22, 0XA20C0040, 27, 0XA20C0070, 0XA20C00A0, 20,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00F0, 22, 0XA20C0120, 0XA20C0140, 27, },  // HAL_GPIO_27
    {   0XA20C0010, 24, 0XA20C0040, 28, 0XA20C0070, 0XA20C00A0, 21,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00F0, 24, 0XA20C0120, 0XA20C0140, 28, },  // HAL_GPIO_28
    {   0XA20C0010, 26, 0XA20C0040, 29, 0XA20C0070, 0XA20C00A0, 22,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00F0, 26, 0XA20C0120, 0XA20C0140, 29, },  // HAL_GPIO_29
    {   0XA20C0010, 28, 0XA20C0040, 30, 0XA20C0070, 0XA20C00A0, 23,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00F0, 28, 0XA20C0120, 0XA20C0140, 30, },  // HAL_GPIO_30
    {   0XA20C0010, 30, 0XA20C0040, 31, 0XA20C0070, 0XA20C00A0, 24,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C00F0, 30, 0XA20C0120, 0XA20C0140, 31, },  // HAL_GPIO_31
    {   0XA20C0020, 0 , 0XA20C0060, 0 , 0XA20C0070, 0XA20C00A0, 25,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C0100, 0 , 0XA20C0130, 0XA20C0150, 0 , },  // HAL_GPIO_32
    {   0XA20C0020, 2 , 0XA20C0060, 1 , 0XA20C0070, 0XA20C00A0, 26,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C0100, 2 , 0XA20C0130, 0XA20C0150, 1 , },  // HAL_GPIO_33
    {   0XA20C0020, 4 , 0XA20C0060, 2 , 0XA20C0070, 0XA20C00A0, 27,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C0100, 4 , 0XA20C0130, 0XA20C0150, 2 , },  // HAL_GPIO_34
    {   0XA20C0020, 6 , 0XA20C0060, 3 , 0XA20C0070, 0XA20C00A0, 28,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C0100, 6 , 0XA20C0130, 0XA20C0150, 3 , },  // HAL_GPIO_35
    {   0XA20C0020, 8 , 0XA20C0060, 4 , 0XA20C0070, 0XA20C00A0, 29,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C0100, 8 , 0XA20C0130, 0XA20C0150, 4 , },  // HAL_GPIO_36
    {   0XA20C0020, 10, 0XA20C0060, 5 , 0XFFFFFFFF, 0XFFFFFFFF, 0XFF, 0XA20C0090, 0XA20C00C0, 0XA20C00D0, 7,      0XA20C0100, 10, 0XA20C0130, 0XA20C0150, 5 , },  // HAL_GPIO_37
    {   0XA20C0020, 12, 0XA20C0060, 6 , 0XFFFFFFFF, 0XFFFFFFFF, 0XFF, 0XA20C0090, 0XA20C00C0, 0XA20C00D0, 8,      0XA20C0100, 12, 0XA20C0130, 0XA20C0150, 6 , },  // HAL_GPIO_38
    {   0XA20C0020, 14, 0XA20C0060, 7 , 0XA20C0070, 0XA20C00A0, 30,   0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF, 0XFF,   0XA20C0100, 14, 0XA20C0130, 0XA20C0150, 7 , },  // HAL_GPIO_39
};

void gpio_get_state(hal_gpio_pin_t gpio_pin, gpio_state_t *gpio_state)
{

    uint32_t mode;
    uint32_t dir;
    uint32_t din;
    uint32_t dout;
    uint32_t pu;
    uint32_t pd;
    uint32_t pupd;
    uint32_t r0;
    uint32_t r1;

    gpio_pull_type_t pull_type;
    uint32_t temp;
    uint32_t shift;
    uint32_t reg_index;
    uint32_t bit_index;
    hal_gpio_driving_current_t driving_value;

    //const char *direct[2] = {"input", "output"};
    //const char *pull_state[10] = {"disable_pull", "PU_R", "PD_R", "PU_R0","PD_R0", "PU_R0_R1", "PD_R0_R1", "PUPD_Error","PU_R1","PD_R1"};


    reg_index = gpio_pin / 8;
    bit_index = (gpio_pin % 8) * 4;
    mode = (gpio_base->GPIO_MODE.RW[reg_index] >> (bit_index) & 0xf);

    reg_index = gpio_pin / 32;
    bit_index = gpio_pin % 32;
    dir  = (gpio_base->GPIO_DIR.RW[reg_index] >> (bit_index) & 0x1);
    din  = (gpio_base->GPIO_DIN.R[reg_index] >> (bit_index) & 0x1);
    dout = (gpio_base->GPIO_DOUT.RW[reg_index] >> (bit_index) & 0x1);

    pu = 0xf;
    pd = 0xf;
    pupd = 0xf;
    r0   = 0xf;
    r1   = 0xf;

    shift = 0xff;
    pull_type = GPIO_PUPD_ERR;

    if (gpio_cfg_table[gpio_pin].pupd_shift != 0xff) {
        shift = gpio_cfg_table[gpio_pin].pupd_shift;
        pu = (GPIO_REG32(gpio_cfg_table[gpio_pin].pu_reg) >> shift) & 0x01;
        pd = (GPIO_REG32(gpio_cfg_table[gpio_pin].pd_reg) >> shift) & 0x01;

        temp = (pu << 4) + pd;

        //log_hal_msgid_info("pu=%d pd=%d, temp=%.3x\r\n", 3, pu,pd,temp);

        if (temp == 0x00) {
            pull_type = GPIO_NO_PULL;
        } else if (temp == 0x10) {
            pull_type = GPIO_PU_R;
        } else if (temp == 0x01) {
            pull_type = GPIO_PD_R;
        } else if (temp == 0xff) {
            pull_type = GPIO_PUPD_ERR;
            log_hal_msgid_info("error pu = %x, pd= %x\r\n", 2, pu, pd);
        }
    } else if (gpio_cfg_table[gpio_pin].pupd_r0_r1_shift != 0xff) {
        shift = gpio_cfg_table[gpio_pin].pupd_r0_r1_shift;
        pupd = (GPIO_REG32(gpio_cfg_table[gpio_pin].pupd_reg) >> shift) & 0x01;
        r0 = (GPIO_REG32(gpio_cfg_table[gpio_pin].r0_reg) >> shift) & 0x01;
        r1 = (GPIO_REG32(gpio_cfg_table[gpio_pin].r1_reg) >> shift) & 0x01;

        temp = (pupd << 8) + (r0 << 4) + r1;
        //log_hal_msgid_info("pupd=%d r0=%d, r1=%d, temp=%.3x\r\n", 4, pupd,r0,r1,temp);

        if (temp == 0x010) {
            pull_type = GPIO_PU_R0;
        } else if (temp == 0x001) {
            pull_type = GPIO_PU_R1;
        } else if (temp == 0x110) {
            pull_type = GPIO_PD_R0;
        } else if (temp == 0x101) {
            pull_type = GPIO_PD_R1;
        } else if (temp == 0x011) {
            pull_type = GPIO_PU_R0_R1;
        } else if (temp == 0x111) {
            pull_type = GPIO_PD_R0_R1;
        } else if ((temp == 0x100) || (temp == 0x000)) {
            pull_type = GPIO_NO_PULL;
        } else if (temp == 0xfff) {
            pull_type = GPIO_PUPD_ERR;
            log_hal_msgid_info("error pupd-r0-r1 = %x\r\n", 1, temp);
        }

    }


    hal_gpio_get_driving_current((hal_gpio_pin_t)gpio_pin, &driving_value);

    gpio_state->mode = mode;
    gpio_state->dir  = dir;
    gpio_state->din  = din;
    gpio_state->dout = dout;
    gpio_state->pull_type = pull_type;
    gpio_state->current_type = (uint8_t)driving_value;

    //log_hal_msgid_info("GPIO%d, mode=%d, dir=%d, din=%d, dout=%d, pull_type=%d\r\n", 6, gpio_pin, mode, dir, din, dout, pull_type);
}

#endif

