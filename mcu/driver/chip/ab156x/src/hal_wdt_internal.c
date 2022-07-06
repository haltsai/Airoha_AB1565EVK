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

#include "hal_wdt.h"

#ifdef HAL_WDT_MODULE_ENABLED
#include "hal.h"
#include "hal_wdt_internal.h"

#define     WDT_HW_BLOCK_TICK       8
#define     WDT_OP_FEED_DOG         1
#define     WDT_OP_SET_LENGHTH      2

typedef enum {
    WDT_O_SAFE = 0,
    WDT_O_IGNORE,
    WDT_O_DELAY
}wdt_op_statue_t;


/*For resolve WDT HW limitation*/
wdt_op_statue_t     wdt_check_operate_is_safe(uint32_t  channel, uint32_t curr_stat)
{
    static uint32_t prev_tick[2] = {0,0};
    static uint32_t prev_stat[2] = {0,0};

    uint32_t        curr_tick = 0;
    uint32_t        dura_tick = 0;
    uint32_t        status;
    wdt_op_statue_t ret;

    /*Disable IRQ */
    hal_nvic_save_and_set_interrupt_mask(&status);

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &curr_tick);
    hal_gpt_get_duration_count(prev_tick[channel], curr_tick, &dura_tick);

    if (dura_tick < WDT_HW_BLOCK_TICK) {
        switch (prev_stat[channel]) {
            case WDT_OP_FEED_DOG:{
                    if(curr_stat == WDT_OP_FEED_DOG){
                        ret = WDT_O_IGNORE;
                    } else {
                        ret = WDT_O_DELAY;
                    }
                }break;
            case WDT_OP_SET_LENGHTH:{
                    if(curr_stat == WDT_OP_FEED_DOG){
                        ret = WDT_O_SAFE;
                    } else {
                        ret = WDT_O_DELAY;
                    }
                }break;
            default:{
                ret = WDT_O_SAFE;
                }break;
        }

    }else {
        ret = WDT_O_SAFE;
    }
    /* Update tick only operate WDT HW */
    if (ret != WDT_O_IGNORE) {
        prev_tick[channel] = curr_tick;
        prev_stat[channel] = curr_stat;
    }
    /*Enable IRQ */
    hal_nvic_restore_interrupt_mask(status);
    return ret;
}


void wdt_set_length(uint32_t channel, uint32_t seconds)
{
    uint32_t length_register_value = 0;
    wdt_op_statue_t  status;

    length_register_value = (seconds * 10000) / WDT_1_TICK_LENGTH;
    length_register_value <<= WDT_LENGTH_OFFSET;

    status = wdt_check_operate_is_safe(channel, WDT_OP_SET_LENGHTH);
    switch(status){
        case WDT_O_DELAY:{
                hal_gpt_delay_us(WDT_HW_BLOCK_TICK * 32);
            }break;
        case WDT_O_SAFE:{
            }break;
        default:            break;
    }

    if (channel == 0) {
        /* write the length register */
        length_register_value |= WDT_LENGTH_KEY << WDT_LENGTH_KEY_OFFSET;
        WDT_REGISTER->WDT_LENGTH = length_register_value;
        /*restart WDT to let the new value take effect */
        WDT_REGISTER->WDT_SW_RESTART = WDT_SW_RESTART_KEY;
    } else {
        /* write the length register */
        length_register_value |= WDT1_LENGTH_KEY << WDT_LENGTH_KEY_OFFSET;
        WDT_REGISTER->WDT1_LENGTH = length_register_value;
        /*restart WDT to let the new value take effect */
        WDT_REGISTER->WDT1_SW_RESTART = WDT1_SW_RESTART_KEY;
    }
}

void wdt_set_mode(uint32_t channel, uint32_t value)
{
    if (value == HAL_WDT_MODE_INTERRUPT) {
        if (channel == 0) {
            WDT_REGISTER->WDT_IE =  (WDT_IRQ_IE_MASK | (WDT_IRQ_IE_KEY << WDT_IRQ_IE_KEY_OFFSET)) |
                                    (WDT_NMI_IE_MASK | (WDT_NMI_IE_KEY << WDT_NMI_IE_KEY_OFFSET));
        } else {
            WDT_REGISTER->WDT1_IE = (WDT_IRQ_IE_MASK | (WDT1_IRQ_IE_KEY << WDT_IRQ_IE_KEY_OFFSET)) |
                                    (WDT_NMI_IE_MASK | (WDT1_NMI_IE_KEY << WDT_NMI_IE_KEY_OFFSET));
        }
    } else {
        if (channel == 0) {
            WDT_REGISTER->WDT_IE =  (WDT_IRQ_IE_KEY << WDT_IRQ_IE_KEY_OFFSET) |
                                    (WDT_NMI_IE_KEY << WDT_NMI_IE_KEY_OFFSET);
        } else {
            WDT_REGISTER->WDT1_IE = (WDT1_IRQ_IE_KEY << WDT_IRQ_IE_KEY_OFFSET) |
                                    (WDT1_NMI_IE_KEY << WDT_NMI_IE_KEY_OFFSET);
        }
    }
}

void wdt_set_restart(uint32_t channel)
{
    wdt_op_statue_t  status;

    status = wdt_check_operate_is_safe(channel, WDT_OP_FEED_DOG);

    switch(status){
        case WDT_O_IGNORE:  {
                return;
            };
        case WDT_O_DELAY:{
                hal_gpt_delay_us(WDT_HW_BLOCK_TICK * 32);
            }break;
        case WDT_O_SAFE:{
            }break;
        default:            break;
    }
    if (channel == 0) {
        WDT_REGISTER->WDT_SW_RESTART = WDT_SW_RESTART_KEY;
    } else {
        WDT_REGISTER->WDT1_SW_RESTART = WDT1_SW_RESTART_KEY;
    }
}

void wdt_set_sw_rst(uint32_t channel)
{
    if (channel == 0) {
        WDT_REGISTER->WDT_SW_RST = WDT_SW_RST_KEY;
    } else {
        WDT_REGISTER->WDT1_SW_RST = WDT1_SW_RST_KEY;
    }
}

void wdt_set_enable(uint32_t channel, uint32_t enable)
{
    if (channel == 0) {
        if (enable) {
            WDT_REGISTER->WDT_EN = (WDT_EN_MASK | (WDT_EN_KEY << WDT_EN_KEY_OFFSET));
            WDT_REGISTER->WDT_AUTO_RESTART_EN = (WDT_AUTO_RESTART_EN_MASK | (WDT_AUTO_RESTART_EN_KEY << WDT_AUTO_RESTART_EN_KEY_OFFSET));
        } else {
            WDT_REGISTER->WDT_EN = (WDT_EN_KEY << WDT_EN_KEY_OFFSET);
            WDT_REGISTER->WDT_AUTO_RESTART_EN = (WDT_AUTO_RESTART_EN_KEY << WDT_AUTO_RESTART_EN_KEY_OFFSET);
        }
    } else {
        if (enable) {
            WDT_REGISTER->WDT1_EN = (WDT_EN_MASK | (WDT1_EN_KEY << WDT_EN_KEY_OFFSET));
            WDT_REGISTER->WDT1_AUTO_RESTART_EN = (WDT_AUTO_RESTART_EN_MASK | (WDT1_AUTO_RESTART_EN_KEY << WDT_AUTO_RESTART_EN_KEY_OFFSET));
        } else {
            WDT_REGISTER->WDT1_EN = (WDT1_EN_KEY << WDT_EN_KEY_OFFSET);
            WDT_REGISTER->WDT1_AUTO_RESTART_EN = (WDT1_AUTO_RESTART_EN_KEY << WDT_AUTO_RESTART_EN_KEY_OFFSET);
        }
    }
}

hal_wdt_reset_status_t wdt_get_reset_status(uint32_t channel)
{
    uint32_t status_register_value;

    if (channel == 0) {
        status_register_value = WDT_REGISTER->WDT_STA;
    } else {
        status_register_value = WDT_REGISTER->WDT1_STA;
    }

    if (status_register_value & WDT_HW_STA_MASK) {
        return HAL_WDT_TIMEOUT_RESET;
    } else if (status_register_value & WDT_SW_STA_MASK) {
        return HAL_WDT_SOFTWARE_RESET;
    }

    return HAL_WDT_NONE_RESET;
}

bool wdt_get_enable_status(uint32_t channel)
{
    if (channel == 0) {
        if (WDT_REGISTER->WDT_EN & WDT_EN_MASK) {
            return true;
        } else {
            return false;
        }
    } else {
        if (WDT_REGISTER->WDT1_EN & WDT_EN_MASK) {
            return true;
        } else {
            return false;
        }
    }
}

hal_wdt_mode_t wdt_get_mode_status(uint32_t channel)
{
    if (channel == 0) {
        if (WDT_REGISTER->WDT_IE & (WDT_IRQ_IE_MASK | WDT_NMI_IE_MASK)) {
            return HAL_WDT_MODE_INTERRUPT;
        } else {
            return HAL_WDT_MODE_RESET;
        }
    } else {
        if (WDT_REGISTER->WDT1_IE & (WDT_IRQ_IE_MASK | WDT_NMI_IE_MASK)) {
            return HAL_WDT_MODE_INTERRUPT;
        } else {
            return HAL_WDT_MODE_RESET;
        }
    }
}

bool wdt_clear_irq(uint32_t channel)
{
    uint32_t status;

    if (channel == 0) {
        status = WDT_REGISTER->WDT_INT;
    } else {
        status = WDT_REGISTER->WDT1_INT;
    }

    if (status & WDT_INT_MASK) {
        return true;
    } else {
        return false;
    }
}

void wdt_set_pmu_mask(uint32_t enable)
{
    if (enable) {
        WDT_REGISTER->WDT_MASK1_UNION.WDT_MASK1_CELLS.PMU_RESET = (WDT_PMU_MASK_SW_RST_MASK | (WDT_PMU_MASK_SW_RST_KEY << WDT_PMU_MASK_SW_RST_KEY_OFFSET));
    } else {
        WDT_REGISTER->WDT_MASK1_UNION.WDT_MASK1_CELLS.PMU_RESET = (WDT_PMU_MASK_SW_RST_KEY << WDT_PMU_MASK_SW_RST_KEY_OFFSET);
    }
}

void wdt_set_pmu_reset_polarity(uint32_t pmu_rst_inv_en)
{
    if (0 == pmu_rst_inv_en) {
        /*PMU reset active low*/
        WDT_REGISTER->WDT_FUNC_CONT1_UNION.WDT_FUNC_CONT1_CELLS.PMU_RST_INV_EN = (WDT_PMU_SW_RST_POLARITY_KEY << WDT_PMU_SW_RST_POLARITY_KEY_OFFSET);
    } else {
        /*PMU reset active high*/
        WDT_REGISTER->WDT_FUNC_CONT1_UNION.WDT_FUNC_CONT1_CELLS.PMU_RST_INV_EN = (WDT_PMU_SW_RST_POLARITY_MASK | (WDT_PMU_SW_RST_POLARITY_KEY << WDT_PMU_SW_RST_POLARITY_KEY_OFFSET));
    }
}

void wdt_clear_reset_status(wdt_reset_type_t type)
{
    switch (type) {
        case SYSTEM_RESET:
            WDT_REGISTER->WDT_SW_RSTSYS_UNION.WDT_SW_RSTSYS_CELLS.SYSTEM_SW_RST = (WDT_SW_RSTSYS_MASK | (WDT_SW_RSTSYS_KEY << WDT_SW_RSTSYS_KEY_OFFSET));
            WDT_REGISTER->WDT_SW_RSTSYS_UNION.WDT_SW_RSTSYS_CELLS.SYSTEM_SW_RST = (WDT_SW_RSTSYS_KEY << WDT_SW_RSTSYS_KEY_OFFSET);
            break;
        case DSPSYS_DEBUG_RESET:
            WDT_REGISTER->WDT_RST0_UNION.WDT_RST0_CELLS.DSPSYS_DEBUG_SW_RST = (WDT_DSPSYS_DEBUG_SW_RST_MASK | (WDT_DSPSYS_DEBUG_SW_RST_KEY << WDT_DSPSYS_DEBUG_SW_RST_KEY_OFFSET));
            WDT_REGISTER->WDT_RST0_UNION.WDT_RST0_CELLS.DSPSYS_DEBUG_SW_RST = (WDT_DSPSYS_DEBUG_SW_RST_KEY << WDT_DSPSYS_DEBUG_SW_RST_KEY_OFFSET);
            break;
        case DSPSYS_CORE_RESET:
            WDT_REGISTER->WDT_RST0_UNION.WDT_RST0_CELLS.DSPSYS_CORE_SW_RST = (WDT_DSPSYS_CORE_SW_RST_MASK | (WDT_DSPSYS_CORE_SW_RST_KEY << WDT_DSPSYS_CORE_SW_RST_KEY_OFFSET));
            WDT_REGISTER->WDT_RST0_UNION.WDT_RST0_CELLS.DSPSYS_CORE_SW_RST = (WDT_DSPSYS_CORE_SW_RST_KEY << WDT_DSPSYS_CORE_SW_RST_KEY_OFFSET);
            break;
        case CONNSYS_RESET:
            WDT_REGISTER->WDT_RST1_UNION.WDT_RST1_CELLS.CONNSYS_SW_RST = (WDT_CONNSYS_SW_RST_MASK | (WDT_CONNSYS_SW_RST_KEY << WDT_CONNSYS_SW_RST_KEY_OFFSET));
            WDT_REGISTER->WDT_RST1_UNION.WDT_RST1_CELLS.CONNSYS_SW_RST = (WDT_CONNSYS_SW_RST_KEY << WDT_CONNSYS_SW_RST_KEY_OFFSET);
            break;
        case AUDIOSYS_RESET:
            WDT_REGISTER->WDT_RST2_UNION.WDT_RST2_CELLS.AUDIOSYS_SW_RST = (WDT_AUDIOSYS_SW_RST_MASK | (WDT_AUDIOSYS_SW_RST_KEY << WDT_AUDIOSYS_SW_RST_KEY_OFFSET));
            WDT_REGISTER->WDT_RST2_UNION.WDT_RST2_CELLS.AUDIOSYS_SW_RST = (WDT_AUDIOSYS_SW_RST_KEY << WDT_AUDIOSYS_SW_RST_KEY_OFFSET);
            break;
        case USB_RESET:
            WDT_REGISTER->WDT_RST3_UNION.WDT_RST3_CELLS.USB_SW_RST = (WDT_USB_SW_RST_MASK | (WDT_USB_SW_RST_KEY << WDT_USB_SW_RST_KEY_OFFSET));
            WDT_REGISTER->WDT_RST3_UNION.WDT_RST3_CELLS.USB_SW_RST = (WDT_USB_SW_RST_KEY << WDT_USB_SW_RST_KEY_OFFSET);
            break;
        case USBIF_RESET:
            WDT_REGISTER->WDT_RST3_UNION.WDT_RST3_CELLS.USBSIF_SW_RST = (WDT_USBIF_SW_RST_MASK | (WDT_USBIF_SW_RST_KEY << WDT_USBIF_SW_RST_KEY_OFFSET));
            WDT_REGISTER->WDT_RST3_UNION.WDT_RST3_CELLS.USBSIF_SW_RST = (WDT_USBIF_SW_RST_KEY << WDT_USBIF_SW_RST_KEY_OFFSET);
            break;
        default:
            break;
    }
}

void wdt_reset_default_value(uint32_t channel)
{
    uint32_t temp_value;

    if (channel == 0) {
        temp_value = WDT_REGISTER->WDT_INT;
        temp_value = temp_value;
        WDT_REGISTER->WDT_EN = (WDT_EN_KEY << WDT_EN_KEY_OFFSET);
        WDT_REGISTER->WDT_LENGTH = 0x07FF0000 | (WDT_LENGTH_KEY << WDT_LENGTH_KEY_OFFSET);
        WDT_REGISTER->WDT_INTERVAL = 0x0FFF0000 | (WDT_INTERVAL_KEY << WDT_INTERVAL_KEY_OFFSET);
        //WDT_REGISTER->WDT_SW_RESTART = ;
        //WDT_REGISTER->WDT_SW_RST = ;
        WDT_REGISTER->WDT_AUTO_RESTART_EN = WDT_AUTO_RESTART_EN_KEY << WDT_AUTO_RESTART_EN_KEY_OFFSET;
        //WDT_REGISTER->WDT_STA = ;
        WDT_REGISTER->WDT_IE = (WDT_IRQ_IE_KEY << WDT_IRQ_IE_KEY_OFFSET) | (WDT_NMI_IE_KEY << WDT_NMI_IE_KEY_OFFSET);
        WDT_REGISTER->WDT_WAKEUP_EN = WDT_WAKEUP_EN_KEY << WDT_WAKEUP_EN_KEY_OFFSET;
        temp_value = WDT_REGISTER->WDT_WAKEUP;
        temp_value = temp_value;
    } else {
        temp_value = WDT_REGISTER->WDT1_INT;
        temp_value = temp_value;
        WDT_REGISTER->WDT1_EN = (WDT1_EN_KEY << WDT_EN_KEY_OFFSET);
        WDT_REGISTER->WDT1_LENGTH = 0x07FF0000 | (WDT1_LENGTH_KEY << WDT_LENGTH_KEY_OFFSET);
        WDT_REGISTER->WDT1_INTERVAL = 0x0FFF0000 | (WDT1_INTERVAL_KEY << WDT_INTERVAL_KEY_OFFSET);
        //WDT_REGISTER->WDT1_SW_RESTART = ;
        //WDT_REGISTER->WDT1_SW_RST = ;
        WDT_REGISTER->WDT1_AUTO_RESTART_EN = WDT1_AUTO_RESTART_EN_KEY << WDT_AUTO_RESTART_EN_KEY_OFFSET;
        //WDT_REGISTER->WDT1_STA = ;
        WDT_REGISTER->WDT1_IE = (WDT1_IRQ_IE_KEY << WDT_IRQ_IE_KEY_OFFSET) | (WDT1_NMI_IE_KEY << WDT_NMI_IE_KEY_OFFSET);
        WDT_REGISTER->WDT1_WAKEUP_EN = WDT1_WAKEUP_EN_KEY << WDT_WAKEUP_EN_KEY_OFFSET;
        temp_value = WDT_REGISTER->WDT1_WAKEUP;
        temp_value = temp_value;
    }
}

void    wdt_dump_register(uint32_t channel)
{
    static uint8_t  idx = 0;

    idx = idx;
    log_hal_msgid_error("wdt channel %d dump(%d):", 2, channel, idx++);
    if (channel == 0) {
        log_hal_msgid_error("WDT_EN(%x),WDT_LEN(%x),WDT_INTV(%x),WDT_AUTO_RESTR(%x),WDT_PMU(%x)", 5,
                            WDT_REGISTER->WDT_EN,
                            WDT_REGISTER->WDT_LENGTH,
                            WDT_REGISTER->WDT_INTERVAL,
                            WDT_REGISTER->WDT_AUTO_RESTART_EN,
                            WDT_REGISTER->WDT_MASK1_UNION.WDT_MASK1
                            );
        log_hal_msgid_error("WDT_STA(%x),WDT_IE(%x),WDT_INT(%x),WDT_WK_EN(%x),WDT_WKUP(%x)", 5,
                            WDT_REGISTER->WDT_STA,
                            WDT_REGISTER->WDT_IE,
                            WDT_REGISTER->WDT_INT,
                            WDT_REGISTER->WDT_WAKEUP_EN,
                            WDT_REGISTER->WDT_WAKEUP
                            );
    } else {
        log_hal_msgid_error("WDT_EN(%x),WDT_LEN(%x),WDT_INTV(%x),WDT_AUTO_RESTR(%x),WDT_PMU(%x)", 5,
                            WDT_REGISTER->WDT1_EN,
                            WDT_REGISTER->WDT1_LENGTH,
                            WDT_REGISTER->WDT1_INTERVAL,
                            WDT_REGISTER->WDT1_AUTO_RESTART_EN,
                            WDT_REGISTER->WDT_MASK1_UNION.WDT_MASK1
                            );
        log_hal_msgid_error("WDT_STA(%x),WDT_IE(%x),WDT_INT(%x),WDT_WK_EN(%x),WDT_WKUP(%x)", 5,
                            WDT_REGISTER->WDT1_STA,
                            WDT_REGISTER->WDT1_IE,
                            WDT_REGISTER->WDT1_INT,
                            WDT_REGISTER->WDT1_WAKEUP_EN,
                            WDT_REGISTER->WDT1_WAKEUP
                            );

    }
}


#endif

