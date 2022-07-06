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

#ifndef __HAL_GPIO_INTERNAL_H__
#define __HAL_GPIO_INTERNAL_H__

#include "hal_platform.h"
#include "hal_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef HAL_GPIO_MODULE_ENABLED

#define GPIO_REG_ONE_BIT_SET_CLR            0x1
#define GPIO_REG_TWO_BIT_SET_CLR            0x3
#define GPIO_REG_FOUR_BIT_SET_CLR           0xF
#define GPIO_MODE_MIN_NUMBER                0       /*  minimum function index of GPIO  */
#define GPIO_MODE_MAX_NUMBER                11      /*  maximum function index of GPIO  */
#define GPIO_REG_CTRL_PIN_NUM_OF_32         32      /*  pin munber controlled by one register is 32  */
#define GPIO_MODE_FUNCTION_CTRL_BITS        4       /*  mode function is controlled by 4 bits  */
#define GPIO_TDSEL_FUNCTION_CTRL_BITS       2       /*  TDSEL function is controlled by 2 bits  */
#define GPIO_DRV_FUNCTION_CTRL_BITS         2       /*  DRV function is controlled by 2 bits  */
#define GPIO_DIR_REG_CTRL_PIN_NUM           32      /*  pin munber controlled by direction register is 32  */
#define GPIO_MODE_REG_CTRL_PIN_NUM          8       /*  pin munber controlled by mode register is 8  */
#define GPIO_DIN_REG_CTRL_PIN_NUM           32      /*  pin munber controlled by inputd data register is 32  */
#define GPIO_DOUT_REG_CTRL_PIN_NUM          32      /*  pin munber controlled by output data register is 32  */
#ifdef HAL_GPIO_FEATURE_INVERSE
#define GPIO_DINV_REG_CTRL_PIN_NUM          32      /*  pin munber controlled by inputd data inverse register is 32  */
#endif
#define GPIO_PULL_CTRL_REG_CTRL_PIN_NUM     32      /*  pin munber controlled by pull control register is 32  */
#ifdef HAL_GPIO_FEATURE_PUPD
#define GPIO_PUPD_CTRL_REG_CTRL_PIN_NUM     32      /*  pin munber controlled by PUPD control register is 32  */
#endif
#define GPIO_TDSEL_REG_CTRL_PIN_NUM         16      /*  pin munber controlled by TDSEL register is 16  */
#define GPIO_IES_REG_CTRL_PIN_NUM           32      /*  pin munber controlled by IES register is 32  */
#define GPIO_SR_REG_CTRL_PIN_NUM            32      /*  pin munber controlled by SR register is 32  */
#define GPIO_DRV_REG_CTRL_PIN_NUM           16      /*  pin munber controlled by DRV register is 16  */

#define GPIO_PU_ENABLE                      1
#define GPIO_PD_ENABLE                      1
#define GPIO_PU_DISABLE                     0
#define GPIO_PD_DISABLE                     0

#define GPIO_REG32(x)                       (*((volatile uint32_t*)(x)))
#define GPIO_RW_ADDR                        0x0
#define GPIO_SET_ADDR                       0x4
#define GPIO_CLR_ADDR                       0x8

typedef enum {
    GPIO_NO_PULL  = 0,
    GPIO_PU_R     = 1,
    GPIO_PD_R     = 2,
    GPIO_PU_R0    = 3,
    GPIO_PD_R0    = 4,
    GPIO_PU_R0_R1 = 5,
    GPIO_PD_R0_R1 = 6,
    GPIO_PUPD_ERR = 7,
    GPIO_PU_R1    = 8,
    GPIO_PD_R1    = 9,
} gpio_pull_type_t;


typedef struct {
    uint32_t    drv_reg;
    uint8_t     drv_shift;
    uint32_t    ies_reg;
    uint8_t     ies_shift;
    uint32_t    pd_reg;
    uint32_t    pu_reg;
    uint8_t     pupd_shift;
    uint32_t    pupd_reg;
    uint32_t    r0_reg;
    uint32_t    r1_reg;
    uint8_t     pupd_r0_r1_shift;
    uint32_t    rdsel_reg;
    uint8_t     resel_shift;
    uint32_t    smt_reg;
    uint32_t    sr_reg;
    uint8_t     smt_sr_shift;
} hal_gpio_cfg_reg_t;


typedef struct {
    uint8_t mode;
    uint8_t dir;
    hal_gpio_data_t din;
    hal_gpio_data_t dout;
    gpio_pull_type_t pull_type;
    hal_gpio_driving_current_t current_type;
} gpio_state_t;


extern GPIO_BASE_REGISTER_T *gpio_base;
extern GPIO_CFG0_REGISTER_T *gpio_cfg0;

extern const hal_gpio_cfg_reg_t gpio_cfg_table[];

void gpio_get_state(hal_gpio_pin_t gpio_pin, gpio_state_t* gpio_state);

#ifdef __cplusplus
}
#endif

#endif /*HAL_GPIO_MODULE_ENABLED*/
#endif /*__HAL_GPIO_INTERNAL_H__*/

