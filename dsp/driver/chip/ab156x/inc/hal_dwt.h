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

#ifndef __HAL_DWT_H__
#define __HAL_DWT_H__

#include "hal_platform.h"

#ifdef HAL_DWT_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup DWT
 * @{
 * This section introduces the DSP Debug Watchpoint and Trace(DWT) APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, enums, structures and functions.
 *
 * @section HAL_DWT_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b DWT                        | DWT is the Debug Watchpoint and Trace Unit of DSP. For more details. |
 *
 * @section HAL_DWT_Features_Chapter Supported features
 *
 * - \b Support \b allocate \b a \b DWT \b channel \b for \b watchpoints. \n
 *   Allocate a DWT channel by #hal_dwt_request_watchpoint() \n
 *   For address matching, the comparator can use a mask, so it can match a specific address and also support match a range of addresses. \n
 *   It will generate a DebugMonitor exception on a successful match. \n
 *   \n
 *
 * - \b Support \b enable \b and \b disable \b the \b DWT \b channel. \n
 *   Enable or disable the DWT channel by #hal_dwt_control_watchpoint() \n
 *   \n
 *
 * @section HAL_DWT_Driver_Usage_Chapter How to use this driver
 *  - step1: Masking DWT channel number that you want to use,You need to mask the line of funtion hal_dwt_request_watchpoint(channel1 or channel2) in port.c file
 *  - step2: call #hal_dwt_init() to init the DWT unit.
 *  - step3: call #hal_dwt_request_watchpoint() to allocate a DWT channel masked in step1 and start the watchpoints.
 *  - step4: call #hal_dwt_control_watchpoint() to enable or disable the DWT channel if needed.
 *  - sample code:
 *    @code
 *       int ret;
 *       uint32_t mask;
 *       static uint32_t check_addr = 0x0;
 *
 *       hal_dwt_init();
 *
 *       //allocate DWT channel 0 to monitor the address range [0, (2^3-1)].
 *       //the start_addr must be aligned with 2^3.
 *       //only write access(WDE_DATA_WO) to the specific address range [0, (2^3-1)] will generate a DebugMonitor exception.
 *       ret = hal_dwt_request_watchpoint(HAL_DWT_0, (uint32_t)&check_addr, 0x3, WDE_DATA_WO);
 *       if(0 != ret) {
 *             //error handle
 *       }
 *
 *      //#hal_dwt_control_watchpoint() can be used to enable or disable the monitor.
 *      hal_nvic_save_and_set_interrupt_mask(&mask);
 *      hal_dwt_control_watchpoint(HAL_DWT_0, HAL_DWT_DISABLE); //disable the monitor.
 *      check_addr = 0x1; //not update the value,the check_addr was the pass second parameter in the last hal_dwt_request_watchpoint called.
 *      hal_dwt_control_watchpoint(HAL_DWT_0, HAL_DWT_ENABLE); //re-start the monitor.
 *      hal_nvic_restore_interrupt_mask(mask);
 *
 *    @endcode
 */

/** @defgroup hal_dwt_enum Enum
  * @{
  */
/** @brief  This enum define the DWT function type. */
typedef enum {
    COMP_DISABLE = 0,
    WDE_DATA_RO,         /**< Generate watchpoint debug event when read data accesses. */
    WDE_DATA_WO,         /**< Generate watchpoint debug event when write data accesses. */
    WDE_DATA_RW          /**< Generate watchpoint debug event when read/write data accesses.*/
} hal_dwt_function_t;

/** @brief  This enum define the DWT channel. */
typedef enum {
    HAL_DWT_0 = 0,       /**< DWT channel 0. already used for the task stack overflow check */
    HAL_DWT_1 = 1,       /**< DWT channel 1. already used for the check the 0x0 of writing or reading */
    HAL_DWT_MAX
} hal_dwt_channel_t;

/** @brief  This enum define the DWT channel control. */
typedef enum {
    HAL_DWT_DISABLE = 0,  /**< disable the channel. */
    HAL_DWT_ENABLE  = 1    /**< enable the channel. */
} hal_dwt_control_type_t;

/**
  * @}
  */

/**
 * @brief This function is mainly used to initialize the DWT unit. It is used to enable the DebugMonitor exception.
 * @par       Example
 * Sample code please refer to @ref HAL_DWT_Driver_Usage_Chapter
 */
void hal_dwt_init(void);

/**
 * @brief Allocate a DWT channel and monitor specific a range of addresses
 * @param[in] index is the channel that will be allocated. This parameter can only be the value of type #hal_dwt_channel_t.
 * @param[in] addr_base is the start address to be monitored, the addr_base must be aligned with the 2^addr_mask
 * @param[in] addr_mask is the ignore mask applied to address range matching
 * @param[in] func is which kind of compared accesses will generate watchpoint debug event. This parameter can only be the value of type #hal_dwt_function_t
 * @return    Allocate channel status, -1: fail; 0: success
 * @note      DWT channel 0 and DWT channel 1 already used ,if you want to use,you need mask the channel follow demo code step1.
 * @par       Example
 * Sample code please refer to @ref HAL_DWT_Driver_Usage_Chapter
 */
int32_t hal_dwt_request_watchpoint(uint32_t index, uint32_t addr_base, uint32_t addr_mask, hal_dwt_function_t func);

/**
 * @brief Control the DWT channel
 * @param[in] index is the channel that will be allocated. This parameter can only be the value of type #hal_dwt_channel_t.
 * @param[in] is_enable to enable or disable the channel. This parameter can only be the value of type #hal_dwt_control_type_t.
 * @return    Control channel status, -1: fail; 0:success
 * @par       Example
 * Sample code please refer to @ref HAL_DWT_Driver_Usage_Chapter
 */
int32_t hal_dwt_control_watchpoint(hal_dwt_channel_t index, hal_dwt_control_type_t is_enable);

/**
 * @brief Reset the DWT Unit, all user settings will be reset to the default value.
 */
void hal_dwt_reset(void);

/**
 * @}
 * @}
*/
#endif /* HAL_DWT_MODULE_ENABLED */
#endif /* __DWT_H__ */
