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

#include "hal_core_status.h"
#include "hal_platform.h"
#include "hal_resource_assignment.h"
#include "hal_gpt_internal.h"

#ifdef HAL_HW_RESOURCE_ASSIGNMENT_CHECK

#ifdef HAL_EINT_MODULE_ENABLED
const uint8_t hal_eint_resource_assignment[HAL_EINT_NUMBER_MAX] ={0};
const uint8_t hal_eint_resource_used[HAL_EINT_NUMBER_MAX] ={0}; // just for DSP power off, void the IRQ triggered before this module init when next power on.
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
const uint8_t hal_gpio_resource_assignment[HAL_GPIO_MAX] ={0};
#endif

#ifdef HAL_GPT_MODULE_ENABLED
const uint8_t hal_gpt_resource_assignment[HAL_GPT_MAX_PORT] ={0};
const uint8_t hal_gpt_resource_used[HAL_GPT_MAX_PORT] ={0}; // just for DSP power off, void the IRQ triggered before this module init when next power on.
#endif

#ifdef HAL_I2S_MODULE_ENABLED
const uint8_t hal_i2s_resource_assignment[HAL_I2S_MAX] ={0};
const uint8_t hal_i2s_resource_used[HAL_I2S_MAX] ={0};// just for DSP power off, void the IRQ triggered before this module init when next power on.
#endif

#ifdef HAL_I2C_MODULE_ENABLED
const uint8_t hal_i2c_resource_assignment[HAL_I2C_MASTER_MAX] ={0};
#endif

#ifdef HAL_SPI_MASTER_MODULE_ENABLED
const uint8_t hal_spi_master_resource_assignment[HAL_SPI_MASTER_MAX] ={0};
#endif

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED
const uint8_t hal_spi_slave_resource_assignment[1] ={0};
const uint8_t hal_spi_slave_resource_used[1] ={0};
#endif

#ifdef HAL_UART_MODULE_ENABLED
const uint8_t hal_uart_resource_assignment[HAL_UART_MAX] ={0};
#endif


/*example***********************************************************
uint8_t hal_xxx_resource_assignment[HAL_XXX_NUMBER_MAX] ={
        {1<<HAL_CORE_CM4 | 1<<HAL_CORE_DSP0},//for port0,means CM4 and DSP0 both can access this HW port
        {1<<HAL_CORE_DSP0}, //for port0,means just DSP0 both can access this HW port
        {1<<HAL_CORE_DSP1}, //for port0,means just DSP1 both can access this HW port
        {1<<HAL_CORE_N9},   //for port0,means just N9 both can access this HW port
    };

//usage:
if(hal_eint_resource_assignment[pin] & HAL_CORE_CM4 == HAL_CORE_CM4)
{}
************************************************************/

#endif /* HAL_HW_RESOURCE_ASSIGNMENT_CHECK */

/* Memory View Transform */

/* In the following nouns
    CM4  means: non-cacheable region of CM4.
    dsp0 means: non-cacheable region of dsp0.
    infrasys means: physical address. Could be non-cacheable in CM4 view, cacheable in dsp0 view.
*/

/* CM4 non-cacheable convert to DSP0 non-cacheable
    such as 0x042x_xxxx to 0x842x_xxxx
*/
uint32_t hal_memview_cm4_to_dsp0(uint32_t cm4_address)
{
    uint32_t dsp0_address = 0;

    if((cm4_address < 0x10000000))
    {
        dsp0_address = cm4_address + 0x80000000;
    }
    else
    {
        dsp0_address = cm4_address;
    }

    return dsp0_address;
}

/* CM4 non-cacheable convert to infrasys (physical) */
uint32_t hal_memview_cm4_to_infrasys(uint32_t cm4_address)
{
    uint32_t infrasys_address = 0;

    infrasys_address = cm4_address;

    return infrasys_address;
}

/* dsp0 non-cacheable convert to CM4 non-cacheable */
uint32_t hal_memview_dsp0_to_cm4(uint32_t dsp0_address)
{
    uint32_t cm4_address = 0;

    if((dsp0_address >= 0x80000000) && (dsp0_address < 0xa0000000))
    {
        cm4_address = dsp0_address - 0x80000000;
    }
    else
    {
        cm4_address = dsp0_address;
    }

    return cm4_address;
}

/* dsp0 non-cacheable convert to infrasys(physical) */
uint32_t hal_memview_dsp0_to_infrasys(uint32_t dsp0_address)
{
    uint32_t infrasys_address = 0;

    if((dsp0_address >= 0x80000000) && (dsp0_address < 0xa0000000))
    {
        infrasys_address = dsp0_address - 0x80000000;
    }
    else
    {
        infrasys_address = dsp0_address;
    }

    return infrasys_address;
}

/* infrasys(physical) convert to CM4 non-cacheable */
uint32_t hal_memview_infrasys_to_cm4(uint32_t infrasys_address)
{
    uint32_t cm4_address = 0;

    cm4_address = infrasys_address;

    return cm4_address;
}

/* infrasys(physical) convert to dsp0 non-cacheable */
uint32_t hal_memview_infrasys_to_dsp0(uint32_t infrasys_address)
{
    uint32_t dsp0_address = 0;

    if((infrasys_address < 0x10000000))
    {
        dsp0_address = infrasys_address + 0x80000000;
    }
    else
    {
        dsp0_address = infrasys_address;
    }

    return dsp0_address;
}
/* Memory View Transform end*/

