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

/*************************************************************************
 * Included header files
 *************************************************************************/
#include "hal_eint.h"

#ifdef HAL_EINT_MODULE_ENABLED
#include "hal_eint_internal.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "memory_attribute.h"


hal_eint_status_t hal_eint_init(hal_eint_number_t eint_number, const hal_eint_config_t *eint_config)
{
    hal_eint_status_t status;
    uint32_t mask;
    static uint32_t ini_state = 0;

    if (ini_state == 0) {
        eint_ack_interrupt(eint_number);
        hal_nvic_register_isr_handler(EINT_IRQn, (hal_nvic_isr_t)hal_eint_isr);
        ini_state = 1;
    }

    if (eint_number >= HAL_EINT_NUMBER_MAX || eint_config == NULL) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    eint_function_table[eint_number].eint_callback = NULL;
    eint_function_table[eint_number].user_data = NULL;
    hal_nvic_restore_interrupt_mask(mask);
    status  = hal_eint_set_trigger_mode(eint_number, eint_config->trigger_mode);
    status |= hal_eint_set_debounce_time(eint_number, eint_config->debounce_time);

    eint_ack_interrupt(eint_number);
    eint_ack_wakeup_event(eint_number);

    return status;

}

hal_eint_status_t hal_eint_deinit(hal_eint_number_t eint_number)
{
    uint32_t mask;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_ERROR_EINT_NUMBER;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    eint_function_table[eint_number].eint_callback = NULL;
    eint_function_table[eint_number].user_data = NULL;

    eint_ack_interrupt(eint_number);
    hal_eint_mask(eint_number);
    hal_nvic_restore_interrupt_mask(mask);
    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t hal_eint_register_callback(hal_eint_number_t eint_number,
        hal_eint_callback_t eint_callback,
        void *user_data)
{
    uint32_t mask;

    if (eint_number >= HAL_EINT_NUMBER_MAX || eint_callback == NULL) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    eint_function_table[eint_number].eint_callback = eint_callback;
    eint_function_table[eint_number].user_data = user_data;
    hal_nvic_restore_interrupt_mask(mask);
    return HAL_EINT_STATUS_OK;
}


/***************************************** *******************************
    sensitivity:
                            1         level  (default)
                            0         edge
    polarity:               0         negative polarity  (default)
                            1         positive polarity

 *************************************************************************/
hal_eint_status_t hal_eint_set_trigger_mode(hal_eint_number_t eint_number,
        hal_eint_trigger_mode_t trigger_mode)
{
    uint32_t mask;
    uint32_t shift;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);

	if (eint_number < EINT_GROUP_MAX_NUMBER) {
    	shift = (1 << eint_number);
		if (trigger_mode == HAL_EINT_LEVEL_LOW) {
	        EINT_REGISTER->EINT_SENS_SET   = shift;
	        EINT_REGISTER->EINT_POL_CLR    = shift;
	    } else if (trigger_mode == HAL_EINT_LEVEL_HIGH) {
	        EINT_REGISTER->EINT_SENS_SET   = shift;
	        EINT_REGISTER->EINT_POL_SET    = shift;
	    } else if (trigger_mode == HAL_EINT_EDGE_FALLING) {
	        EINT_REGISTER->EINT_SENS_CLR   = shift;
	        EINT_REGISTER->EINT_POL_CLR    = shift;
	        EINT_REGISTER->EINT_DUALEDGE_SENS_CLR = shift;
	    } else if (trigger_mode == HAL_EINT_EDGE_RISING) {
	        EINT_REGISTER->EINT_SENS_CLR   = shift;
	        EINT_REGISTER->EINT_POL_SET    = shift;
	        EINT_REGISTER->EINT_DUALEDGE_SENS_CLR = shift;
	    } else if (trigger_mode == HAL_EINT_EDGE_FALLING_AND_RISING) {
	        /* DUALEGE depends on SENS */
	        EINT_REGISTER->EINT_SENS_CLR          = shift;
	        EINT_REGISTER->EINT_DUALEDGE_SENS_SET = shift;
	    } else {
	        hal_nvic_restore_interrupt_mask(mask);
	        return HAL_EINT_STATUS_INVALID_PARAMETER;
	    }
	} else {
    	shift = (1 << (eint_number & (EINT_GROUP_MAX_NUMBER - 1)));
		if (trigger_mode == HAL_EINT_LEVEL_LOW) {
			EINT_REGISTER->EINT_SENS_SET1   = shift;
			EINT_REGISTER->EINT_POL_CLR1    = shift;
		} else if (trigger_mode == HAL_EINT_LEVEL_HIGH) {
			EINT_REGISTER->EINT_SENS_SET1   = shift;
			EINT_REGISTER->EINT_POL_SET1    = shift;
		} else if (trigger_mode == HAL_EINT_EDGE_FALLING) {
			EINT_REGISTER->EINT_SENS_CLR1   = shift;
			EINT_REGISTER->EINT_POL_CLR1    = shift;
			EINT_REGISTER->EINT_DUALEDGE_SENS_CLR1 = shift;
		} else if (trigger_mode == HAL_EINT_EDGE_RISING) {
			EINT_REGISTER->EINT_SENS_CLR1   = shift;
			EINT_REGISTER->EINT_POL_SET1    = shift;
			EINT_REGISTER->EINT_DUALEDGE_SENS_CLR1 = shift;
		} else if (trigger_mode == HAL_EINT_EDGE_FALLING_AND_RISING) {
			/* DUALEGE depends on SENS */
			EINT_REGISTER->EINT_SENS_CLR1		  = shift;
			EINT_REGISTER->EINT_DUALEDGE_SENS_SET1 = shift;
		} else {
			hal_nvic_restore_interrupt_mask(mask);
			return HAL_EINT_STATUS_INVALID_PARAMETER;
		}
	}

    hal_nvic_restore_interrupt_mask(mask);
    hal_gpt_delay_us(100);
    return HAL_EINT_STATUS_OK;
}

/* every count = 1/32k*/
hal_eint_status_t hal_eint_set_debounce_count(hal_eint_number_t eint_number, uint32_t count)
{
    uint32_t mask, eint_con;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    if ((count & (~EINT_CON_DBC_CNT_MASK))) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    eint_con =  (count | EINT_CON_DBC_EN_MASK |(EINT_CON_PRESCALER_MASK & (EINT_CON_PRESCALER_32KHZ << EINT_CON_PRESCALER_OFFSET)));
    hal_nvic_restore_interrupt_mask(mask);


    if (count == 0) {
        /*disenable debounce bit*/
        EINT_REGISTER->EINT_CON_REGISTER[eint_number].EINT_CON_CELLS.DBC_EN = EINT_CON_DBC_DISABLE;
        /* can't reset debounce as it will cause 1/32k delay by debounce */
    }
    else {
        EINT_REGISTER->EINT_CON_REGISTER[eint_number].EINT_CON = 0;
        EINT_REGISTER->EINT_CON_REGISTER[eint_number].EINT_CON_CELLS.DBC_EN = EINT_CON_DBC_ENABLE;
        EINT_REGISTER->EINT_CON_REGISTER[eint_number].EINT_CON_CELLS.RSTDBC = EINT_CON_RSTD_BC_MASK;
        hal_gpt_delay_us(125);
        EINT_REGISTER->EINT_CON_REGISTER[eint_number].EINT_CON = eint_con;
        EINT_REGISTER->EINT_CON_REGISTER[eint_number].EINT_CON_CELLS.RSTDBC = EINT_CON_RSTD_BC_MASK;
    }

    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t hal_eint_set_debounce_time(hal_eint_number_t eint_number, uint32_t time_ms)
{
    uint32_t mask, count, eint_con;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    count = eint_caculate_debounce_time(time_ms);
    hal_nvic_save_and_set_interrupt_mask(&mask);
    eint_con = count;
    hal_nvic_restore_interrupt_mask(mask);


    if (time_ms == 0) {
        /*disenable debounce bit*/
        EINT_REGISTER->EINT_CON_REGISTER[eint_number].EINT_CON_CELLS.DBC_EN = EINT_CON_DBC_DISABLE;
        /* can't reset debounce as it will cause 1/32k delay by debounce */
    }
    else {
        EINT_REGISTER->EINT_CON_REGISTER[eint_number].EINT_CON = 0;
        EINT_REGISTER->EINT_CON_REGISTER[eint_number].EINT_CON_CELLS.DBC_EN = EINT_CON_DBC_ENABLE;
        EINT_REGISTER->EINT_CON_REGISTER[eint_number].EINT_CON_CELLS.RSTDBC = EINT_CON_RSTD_BC_MASK;
        hal_gpt_delay_us(125);
        EINT_REGISTER->EINT_CON_REGISTER[eint_number].EINT_CON = eint_con;
        EINT_REGISTER->EINT_CON_REGISTER[eint_number].EINT_CON_CELLS.RSTDBC = EINT_CON_RSTD_BC_MASK;

    }

    return HAL_EINT_STATUS_OK;

}

hal_eint_status_t hal_eint_set_software_trigger(hal_eint_number_t eint_number)
{
    uint32_t mask;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
	if (eint_number < EINT_GROUP_MAX_NUMBER) {
	    EINT_REGISTER->EINT_SOFT_SET = (1 << eint_number);
	} else {
	    EINT_REGISTER->EINT_SOFT_SET1 = (1 << (eint_number & (EINT_GROUP_MAX_NUMBER - 1)));
	}
    hal_nvic_restore_interrupt_mask(mask);

    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t hal_eint_clear_software_trigger(hal_eint_number_t eint_number)
{
    uint32_t mask;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_ERROR;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
	if (eint_number < EINT_GROUP_MAX_NUMBER) {
		EINT_REGISTER->EINT_SOFT_CLR = (1 << eint_number);
	} else {
	    EINT_REGISTER->EINT_SOFT_CLR1 = (1 << (eint_number & (EINT_GROUP_MAX_NUMBER - 1)));
	}
    hal_nvic_restore_interrupt_mask(mask);
    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t eint_mask_wakeup_source(hal_eint_number_t eint_number)
{
    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

	if (eint_number < EINT_GROUP_MAX_NUMBER) {
		EINT_REGISTER->EINT_WAKEUP_MASK_SET = (1 << eint_number);
	} else {
	    EINT_REGISTER->EINT_WAKEUP_MASK_SET1 = (1 << (eint_number & (EINT_GROUP_MAX_NUMBER - 1)));
	}

    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t eint_unmask_wakeup_source(hal_eint_number_t eint_number)
{

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

	if (eint_number < EINT_GROUP_MAX_NUMBER) {
		EINT_REGISTER->EINT_WAKEUP_MASK_CLR = (1 << eint_number);
	} else {
	    EINT_REGISTER->EINT_WAKEUP_MASK_CLR1 = (1 << (eint_number & (EINT_GROUP_MAX_NUMBER - 1)));
	}

    return HAL_EINT_STATUS_OK;
}

#ifdef HAL_EINT_FEATURE_MASK
hal_eint_status_t hal_eint_mask(hal_eint_number_t eint_number)
{
    uint32_t mask;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);

	if (eint_number < EINT_GROUP_MAX_NUMBER) {
		EINT_REGISTER->DOMAIN[1].CLR[0] = (1 << eint_number);
		EINT_REGISTER->EINT_MASK_SET = (1 << eint_number);
	} else {
		EINT_REGISTER->DOMAIN[1].CLR[1] = (1 << (eint_number & (EINT_GROUP_MAX_NUMBER - 1)));
        EINT_REGISTER->EINT_MASK_SET1 = (1 << (eint_number & (EINT_GROUP_MAX_NUMBER - 1)));
	}
    eint_mask_wakeup_source(eint_number);
    hal_nvic_restore_interrupt_mask(mask);
    return HAL_EINT_STATUS_OK;
}

hal_eint_status_t hal_eint_unmask(hal_eint_number_t eint_number)
{
    uint32_t mask;

    if (eint_number >= HAL_EINT_NUMBER_MAX) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    /*eint and firq only one work*/
    hal_nvic_save_and_set_interrupt_mask(&mask);

	if (eint_number < EINT_GROUP_MAX_NUMBER) {
        EINT_REGISTER->DOMAIN[1].SET[0] = (1 << eint_number);
		EINT_REGISTER->EINT_MASK_CLR = (1 << eint_number);
	} else {
		EINT_REGISTER->DOMAIN[1].SET[1] = (1 << (eint_number & (EINT_GROUP_MAX_NUMBER - 1)));
        EINT_REGISTER->EINT_MASK_CLR1 = (1 << (eint_number & (EINT_GROUP_MAX_NUMBER - 1)));
	}
    hal_nvic_enable_irq(EINT_IRQn);
    eint_unmask_wakeup_source(eint_number);
    hal_nvic_restore_interrupt_mask(mask);
    return HAL_EINT_STATUS_OK;
}
#endif

#ifdef HAL_EINT_FEATURE_MUX_MAPPING
hal_eint_status_t hal_eint_set_mux_mapping(hal_eint_number_t eint_number, hal_gpio_pin_t gpio_number)
{

    if ((gpio_number>= HAL_GPIO_MAX) || (eint_number>HAL_EINT_NUMBER_22)) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    EINT_SRC_SELECT->EINT[eint_number] = (uint8_t)(gpio_number);

    return HAL_EINT_STATUS_OK;
}


hal_eint_status_t hal_eint_get_mux_mapping(hal_eint_number_t eint_number, hal_gpio_pin_t *gpio_number)
{

    if ((gpio_number== NULL) || (eint_number>HAL_EINT_NUMBER_22)) {
        return HAL_EINT_STATUS_INVALID_PARAMETER;
    }

    *gpio_number = (hal_gpio_pin_t)EINT_SRC_SELECT->EINT[eint_number];

    return HAL_EINT_STATUS_OK;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* HAL_EINT_MODULE_ENABLED */

