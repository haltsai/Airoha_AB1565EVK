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
#include "hal_nvic.h"
#include "hal_log.h"
#ifdef __cplusplus
extern "C" {
#endif

GPIO_BASE_REGISTER_T *gpio_base = (GPIO_BASE_REGISTER_T *)(GPIO_BASE);
GPIO_CFG0_REGISTER_T *gpio_cfg0 = (GPIO_CFG0_REGISTER_T *)(IO_CFG_0_BASE);

TOP_MISC_CFG_T *clk_out_control = TOP_MISC_CFG;

hal_gpio_status_t hal_gpio_init(hal_gpio_pin_t gpio_pin)
{
    gpio_pin = gpio_pin;
    return HAL_GPIO_STATUS_OK;
}


hal_gpio_status_t hal_gpio_deinit(hal_gpio_pin_t gpio_pin)
{
    gpio_pin = gpio_pin;
    return HAL_GPIO_STATUS_OK;
}

hal_gpio_status_t hal_gpio_set_direction(hal_gpio_pin_t gpio_pin, hal_gpio_direction_t gpio_direction)
{
    uint32_t reg_num;
    uint32_t index_num;
    uint32_t mask;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    reg_num   = gpio_pin / GPIO_DIR_REG_CTRL_PIN_NUM;
    index_num = gpio_pin % GPIO_DIR_REG_CTRL_PIN_NUM;

    if (gpio_direction == HAL_GPIO_DIRECTION_INPUT) {
        gpio_base->GPIO_DIR.CLR[reg_num] = (GPIO_REG_ONE_BIT_SET_CLR << index_num);
    } else {
        gpio_base->GPIO_DIR.SET[reg_num] = (GPIO_REG_ONE_BIT_SET_CLR << index_num);
    }
    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPIO_STATUS_OK;

}

hal_gpio_status_t hal_gpio_get_direction(hal_gpio_pin_t gpio_pin, hal_gpio_direction_t *gpio_direction)
{
    uint32_t reg_num;
    uint32_t index_num;
    uint32_t mask;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    if (NULL == gpio_direction) {
        return HAL_GPIO_STATUS_INVALID_PARAMETER;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    reg_num   = gpio_pin / GPIO_DIR_REG_CTRL_PIN_NUM;
    index_num = gpio_pin % GPIO_DIR_REG_CTRL_PIN_NUM;

    if (gpio_base->GPIO_DIR.RW[reg_num] & (GPIO_REG_ONE_BIT_SET_CLR << index_num)) {
        *gpio_direction = HAL_GPIO_DIRECTION_OUTPUT;
    } else {
        *gpio_direction = HAL_GPIO_DIRECTION_INPUT;
    }
    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPIO_STATUS_OK;

}



hal_pinmux_status_t hal_pinmux_set_function(hal_gpio_pin_t gpio_pin, uint8_t function_index)
{
    uint32_t no;
    uint32_t remainder;
    uint32_t mask;
    uint32_t temp;

    log_hal_msgid_info("[pinmux] pin=%d mode=%d\r\n", 2, gpio_pin, function_index);

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_PINMUX_STATUS_ERROR_PORT;
    }

    /* check whether the function index is right as one function is corresponding to 4 bits of oen pin */
    if (function_index >= GPIO_MODE_MAX_NUMBER) {
        return HAL_PINMUX_STATUS_INVALID_FUNCTION;
    }

    /* protect the configuration to prevent possible interrupt */
    hal_nvic_save_and_set_interrupt_mask(&mask);

    /* get the register number corresponding to the pin as one register can control 8 pins*/
    no = gpio_pin / GPIO_MODE_REG_CTRL_PIN_NUM;

    /* get the bit offset within the register as one register can control 8 pins*/
    remainder = gpio_pin % GPIO_MODE_REG_CTRL_PIN_NUM;

    temp = gpio_base->GPIO_MODE.RW[no];
    temp &= ~(GPIO_REG_FOUR_BIT_SET_CLR << (remainder * GPIO_MODE_FUNCTION_CTRL_BITS));
    temp |= (function_index << (remainder * GPIO_MODE_FUNCTION_CTRL_BITS));
    gpio_base->GPIO_MODE.RW[no] = temp;

    /* switch to digital mode for leakage, ADC driver change it to analog mode. */
    if (gpio_pin <= HAL_GPIO_12) {
        gpio_cfg0->GPIO_G.SET = 1 << gpio_pin;
    }
    hal_nvic_restore_interrupt_mask(mask);

    return HAL_PINMUX_STATUS_OK;

}



hal_gpio_status_t hal_gpio_get_input(hal_gpio_pin_t gpio_pin, hal_gpio_data_t *gpio_data)
{
    uint32_t reg_num;
    uint32_t index_num;
    uint32_t mask;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    if (NULL == gpio_data) {
        return HAL_GPIO_STATUS_INVALID_PARAMETER;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    reg_num   = gpio_pin / GPIO_DIN_REG_CTRL_PIN_NUM;
    index_num = gpio_pin % GPIO_DIN_REG_CTRL_PIN_NUM;

    if (gpio_base->GPIO_DIN.R[reg_num] & (GPIO_REG_ONE_BIT_SET_CLR << index_num)) {
        *gpio_data = HAL_GPIO_DATA_HIGH;
    } else {
        *gpio_data = HAL_GPIO_DATA_LOW;
    }
    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPIO_STATUS_OK;

}


hal_gpio_status_t hal_gpio_set_output(hal_gpio_pin_t gpio_pin, hal_gpio_data_t gpio_data)
{
    uint32_t reg_num;
    uint32_t index_num;
    uint32_t mask;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    reg_num   = gpio_pin / GPIO_DOUT_REG_CTRL_PIN_NUM;
    index_num = gpio_pin % GPIO_DOUT_REG_CTRL_PIN_NUM;

    if (gpio_data) {
        gpio_base->GPIO_DOUT.SET[reg_num] = (GPIO_REG_ONE_BIT_SET_CLR << index_num);
    } else {
        gpio_base->GPIO_DOUT.CLR[reg_num] = (GPIO_REG_ONE_BIT_SET_CLR << index_num);
    }
    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPIO_STATUS_OK;
}


hal_gpio_status_t hal_gpio_get_output(hal_gpio_pin_t gpio_pin, hal_gpio_data_t *gpio_data)
{
    uint32_t reg_num;
    uint32_t index_num;
    uint32_t mask;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    if (NULL == gpio_data) {
        return HAL_GPIO_STATUS_INVALID_PARAMETER;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    reg_num   = gpio_pin / GPIO_DOUT_REG_CTRL_PIN_NUM;
    index_num = gpio_pin % GPIO_DOUT_REG_CTRL_PIN_NUM;

    if (gpio_base->GPIO_DOUT.RW[reg_num] & (GPIO_REG_ONE_BIT_SET_CLR << index_num)) {
        *gpio_data = HAL_GPIO_DATA_HIGH;
    } else {
        *gpio_data = HAL_GPIO_DATA_LOW;
    }
    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPIO_STATUS_OK;

}

hal_gpio_status_t hal_gpio_toggle_pin(hal_gpio_pin_t gpio_pin)
{

    uint32_t reg_num = 0;
    uint32_t index_num = 0;
    uint32_t mask;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    reg_num   = gpio_pin / GPIO_DOUT_REG_CTRL_PIN_NUM;
    index_num = gpio_pin % GPIO_DOUT_REG_CTRL_PIN_NUM;

    if (gpio_base->GPIO_DOUT.RW[reg_num] & (GPIO_REG_ONE_BIT_SET_CLR << index_num)) {

        gpio_base->GPIO_DOUT.CLR[reg_num] = (GPIO_REG_ONE_BIT_SET_CLR << index_num);
    } else {
        gpio_base->GPIO_DOUT.SET[reg_num] = (GPIO_REG_ONE_BIT_SET_CLR << index_num);
    }
    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPIO_STATUS_OK;
}

hal_gpio_status_t hal_gpio_pull_up(hal_gpio_pin_t gpio_pin)
{
    uint32_t mask;
    uint8_t  shift;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    shift = gpio_cfg_table[gpio_pin].pupd_shift;
    if (shift != 0xff) {
        hal_nvic_save_and_set_interrupt_mask(&mask);
        GPIO_REG32(gpio_cfg_table[gpio_pin].pd_reg + GPIO_CLR_ADDR) = 1 << shift;
        GPIO_REG32(gpio_cfg_table[gpio_pin].pu_reg + GPIO_SET_ADDR) = 1 << shift;
        hal_nvic_restore_interrupt_mask(mask);
    } else {
#ifdef HAL_GPIO_FEATURE_PUPD
        return hal_gpio_set_pupd_register(gpio_pin, 0, 1, 0); //pull up R0
#endif
    }

    return HAL_GPIO_STATUS_OK;

}



hal_gpio_status_t hal_gpio_pull_down(hal_gpio_pin_t gpio_pin)
{
    uint32_t mask;
    uint8_t  shift;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    shift = gpio_cfg_table[gpio_pin].pupd_shift;
    if (shift != 0xff) {
        hal_nvic_save_and_set_interrupt_mask(&mask);
        GPIO_REG32(gpio_cfg_table[gpio_pin].pu_reg + GPIO_CLR_ADDR) = 1 << shift;
        GPIO_REG32(gpio_cfg_table[gpio_pin].pd_reg + GPIO_SET_ADDR) = 1 << shift;
        hal_nvic_restore_interrupt_mask(mask);
    } else {
#ifdef HAL_GPIO_FEATURE_PUPD
        return hal_gpio_set_pupd_register(gpio_pin, 1, 1, 0); //pull down R0
#endif
    }

    return HAL_GPIO_STATUS_OK;

}



hal_gpio_status_t hal_gpio_disable_pull(hal_gpio_pin_t gpio_pin)
{
    uint32_t mask;
    uint8_t  shift;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    shift = gpio_cfg_table[gpio_pin].pupd_shift;
    if (shift != 0xff) {
        hal_nvic_save_and_set_interrupt_mask(&mask);
        GPIO_REG32(gpio_cfg_table[gpio_pin].pu_reg + GPIO_CLR_ADDR) = 1 << shift;
        GPIO_REG32(gpio_cfg_table[gpio_pin].pd_reg + GPIO_CLR_ADDR) = 1 << shift;
        hal_nvic_restore_interrupt_mask(mask);
    } else {
#ifdef HAL_GPIO_FEATURE_PUPD
        return hal_gpio_set_pupd_register(gpio_pin, 0, 0, 0); //disable pull
#endif
    }

    return HAL_GPIO_STATUS_OK;
}



#ifdef HAL_GPIO_FEATURE_CLOCKOUT

hal_gpio_status_t hal_gpio_set_clockout(hal_gpio_clock_t gpio_clock_num, hal_gpio_clock_mode_t clock_mode)
{
    uint32_t temp;

    if (gpio_clock_num >= HAL_GPIO_CLOCK_MAX) {
        return HAL_GPIO_STATUS_INVALID_PARAMETER;
    }

    if (gpio_clock_num < HAL_GPIO_CLOCK_4) {
        temp = clk_out_control->GPIO_CLKO_CTRL_A;
        temp &= ~(0xf << gpio_clock_num);
        temp |= clock_mode << gpio_clock_num;
        clk_out_control->GPIO_CLKO_CTRL_A = temp;
    } else {
        temp = clk_out_control->GPIO_CLKO_CTRL_B;
        temp &= ~(0xf << (gpio_clock_num - 4));
        temp |= clock_mode << gpio_clock_num;
        clk_out_control->GPIO_CLKO_CTRL_B = temp;
    }

    return HAL_GPIO_STATUS_OK;
}
#endif

#ifdef HAL_GPIO_FEATURE_SET_SCHMITT
hal_gpio_status_t hal_gpio_set_schmitt(hal_gpio_pin_t gpio_pin)
{
    uint32_t mask;
    uint8_t  shift;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);

    shift = gpio_cfg_table[gpio_pin].smt_sr_shift;
    if (shift != 0xff) {
        GPIO_REG32(gpio_cfg_table[gpio_pin].smt_reg + GPIO_SET_ADDR) = 1 << shift;
    }
    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPIO_STATUS_OK;
}

hal_gpio_status_t hal_gpio_clear_schmitt(hal_gpio_pin_t gpio_pin)
{
    uint32_t mask;
    uint8_t  shift;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);

    shift = gpio_cfg_table[gpio_pin].smt_sr_shift;
    if (shift != 0xff) {
        GPIO_REG32(gpio_cfg_table[gpio_pin].smt_reg + GPIO_CLR_ADDR) = 1 << shift;
    }
    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPIO_STATUS_OK;
}
#endif



#ifdef HAL_GPIO_FEATURE_SET_SLEW_RATE
hal_gpio_status_t hal_gpio_set_slew_rate(hal_gpio_pin_t gpio_pin)
{
    uint32_t mask;
    uint8_t  shift;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);

    shift = gpio_cfg_table[gpio_pin].smt_sr_shift;
    if (shift != 0xff) {
        GPIO_REG32(gpio_cfg_table[gpio_pin].sr_reg + GPIO_SET_ADDR) = 1 << shift;
    }
    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPIO_STATUS_OK;
}

hal_gpio_status_t hal_gpio_clear_slew_rate(hal_gpio_pin_t gpio_pin)
{
    uint32_t mask;
    uint8_t  shift;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);

    shift = gpio_cfg_table[gpio_pin].smt_sr_shift;
    if (shift != 0xff) {
        GPIO_REG32(gpio_cfg_table[gpio_pin].sr_reg + GPIO_CLR_ADDR) = 1 << shift;
    }
    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPIO_STATUS_OK;
}
#endif


#ifdef HAL_GPIO_FEATURE_PUPD
hal_gpio_status_t hal_gpio_set_pupd_register(hal_gpio_pin_t gpio_pin, uint8_t gpio_pupd, uint8_t gpio_r0, uint8_t gpio_r1)
{
    uint32_t mask;
    uint8_t  shift;
    uint8_t  shift_addr;


    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);

    shift = 1 << gpio_cfg_table[gpio_pin].pupd_r0_r1_shift;

    if (gpio_cfg_table[gpio_pin].pupd_r0_r1_shift != 0xff) {
        if (gpio_pupd) {
            shift_addr = GPIO_SET_ADDR;
        } else {
            shift_addr = GPIO_CLR_ADDR;
        }

        GPIO_REG32(gpio_cfg_table[gpio_pin].pupd_reg + shift_addr) = shift;

        if (gpio_r0) {
            shift_addr = GPIO_SET_ADDR;
        } else {
            shift_addr = GPIO_CLR_ADDR;
        }

        GPIO_REG32(gpio_cfg_table[gpio_pin].r0_reg + shift_addr) = shift;

        if (gpio_r1) {
            shift_addr = GPIO_SET_ADDR;
        } else {
            shift_addr = GPIO_CLR_ADDR;
        }

        GPIO_REG32(gpio_cfg_table[gpio_pin].r1_reg + shift_addr) = shift;
    } else {
        hal_nvic_restore_interrupt_mask(mask);
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPIO_STATUS_OK;
}
#endif

#ifdef HAL_GPIO_FEATURE_HIGH_Z
hal_gpio_status_t hal_gpio_set_high_impedance(hal_gpio_pin_t gpio_pin)
{
    hal_pinmux_status_t ret1;
    hal_gpio_status_t   ret2;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    /* set GPIO mode of pin */
    ret1 = hal_pinmux_set_function(gpio_pin, 0);
    if (ret1 != HAL_PINMUX_STATUS_OK) {
        return HAL_GPIO_STATUS_ERROR;
    }

    /* set input direction of pin */
    ret2 = hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_INPUT);
    if (ret2 != HAL_GPIO_STATUS_OK) {
        return ret2;
    }

    /* Disable input buffer enable function of pin */
    GPIO_REG32(gpio_cfg_table[gpio_pin].ies_reg + GPIO_CLR_ADDR) = 1 << gpio_cfg_table[gpio_pin].ies_shift;

    /* disable pull function of pin */
    ret2 = hal_gpio_disable_pull(gpio_pin);
    if (ret2 != HAL_GPIO_STATUS_OK) {
        return ret2;
    }

    return HAL_GPIO_STATUS_OK;
}


hal_gpio_status_t hal_gpio_clear_high_impedance(hal_gpio_pin_t gpio_pin)
{

    hal_pinmux_status_t ret1;
    hal_gpio_status_t   ret2;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    /* set GPIO mode of pin. */
    ret1 = hal_pinmux_set_function(gpio_pin, 0);
    if (ret1 != HAL_PINMUX_STATUS_OK) {
        return HAL_GPIO_STATUS_ERROR;
    }

    /* set input direction of pin. */
    ret2 = hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_INPUT);
    if (ret2 != HAL_GPIO_STATUS_OK) {
        return ret2;
    }

    /* Enable input buffer enable function of pin */
    GPIO_REG32(gpio_cfg_table[gpio_pin].ies_reg + GPIO_SET_ADDR) = 1 << gpio_cfg_table[gpio_pin].ies_shift;

    /* enable pull down of pin. */
    ret2 = hal_gpio_pull_down(gpio_pin);
    if (ret2 != HAL_GPIO_STATUS_OK) {
        return ret2;
    }

    return HAL_GPIO_STATUS_OK;
}
#endif

#ifdef HAL_GPIO_FEATURE_SET_DRIVING
hal_gpio_status_t hal_gpio_set_driving_current(hal_gpio_pin_t gpio_pin, hal_gpio_driving_current_t driving)
{
    uint32_t mask;
    uint8_t  shift;
    uint32_t temp;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);

    shift = gpio_cfg_table[gpio_pin].drv_shift;

    temp = GPIO_REG32(gpio_cfg_table[gpio_pin].drv_reg);
    temp &= ~(GPIO_REG_TWO_BIT_SET_CLR << shift);
    temp |= (driving << shift);
    GPIO_REG32(gpio_cfg_table[gpio_pin].drv_reg) = temp;

    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPIO_STATUS_OK;
}


hal_gpio_status_t hal_gpio_get_driving_current(hal_gpio_pin_t gpio_pin, hal_gpio_driving_current_t *driving)
{

    uint32_t mask;
    uint8_t  shift;
    uint32_t temp;

    if (gpio_pin >= HAL_GPIO_MAX) {
        return HAL_GPIO_STATUS_ERROR_PIN;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);

    shift = gpio_cfg_table[gpio_pin].drv_shift;

    temp = GPIO_REG32(gpio_cfg_table[gpio_pin].drv_reg);
    temp = (temp >> shift) & GPIO_REG_TWO_BIT_SET_CLR;

    *driving = (hal_gpio_driving_current_t)(temp);

    hal_nvic_restore_interrupt_mask(mask);

    return HAL_GPIO_STATUS_OK;

}
#endif

#ifdef __cplusplus
}
#endif

#endif  /* HAL_GPIO_MODULE_ENABLED */

