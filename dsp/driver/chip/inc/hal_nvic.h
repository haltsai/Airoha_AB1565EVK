/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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
 
#ifndef __HAL_NVIC_H__
#define __HAL_NVIC_H__
#include "hal_platform.h"

#ifdef HAL_NVIC_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup NVIC
 * @{
 * This section introduces the Nested Vectored Interrupt Controller(NVIC) HAL APIs including terms and acronyms, details on how to use this driver, supported features, enums, structures and functions.
 * It uses #hal_nvic_register_isr_handler function to improve software efficiency and debugging.
 *
 * @section HAL_NVIC_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------|------------------------------------------------------------------------|
 * |\b NVIC                 | Nested Vectored Interrupt Controller. NVIC is the interrupt controller of DSP Core. </a>.|
 * |\b ISR                  | Interrupt service routine.|
 * |\b IRQ                  | Interrupt request.|
 *
 *
 *
 * @section HAL_NVIC_Driver_Usage_Chapter How to use this driver
 *
 * - Using the NVIC. \n
 *
 *  - Step 1. Call the #hal_nvic_init() function to initialize the NVIC IRQ priority according to the exception number. By default, the lower the exception number the higher is the priority.
 *  - Step 2. Call the #hal_nvic_register_isr_handler() function to register an ISR handler before using the NVIC IRQ.
 *  - Step 3. Call the #hal_nvic_enable_irq() to enable the IRQ.
 *  - Sample code:
 *    @code
 *       // Define a callback (an ISR) function.
 *       void isr_handler(hal_nvic_irq_t irq_number)
 *       {
 *           // Do something.
 *       }
 *    @endcode
 *    @code
 *       ret = hal_nvic_init();    // Should be called only once at initialization.
 *       if (HAL_NVIC_STATUS_OK != ret) {
 *               // Error handler;
 *       }
 *       ret = hal_nvic_register_isr_handler(irq_number, isr_handler);   // Register an IRQ handler.
 *       if (HAL_NVIC_STATUS_OK !=  ret) {
 *               // Error handler;
 *       }
 *       hal_nvic_enable_irq(irq_number);
 *
 *    @endcode
 *    @code
 *       void function_task(void)
 *       {
 *           // function_task() runs only in thread mode.
 *       }
 *
 *       void function_isr(void)
 *       {
 *           // function_task() runs only in handler mode.
 *       }
 *
 *    @endcode
 *    @code
 *
 *       // It needs to take different actions according to the current exception status.
 *       if (HAL_NVIC_QUERY_EXCEPTION_NUMBER < 0) {
 *           // In thread mode.
 *           function_task();
 *       } else {
 *           // Handling the exception and the exception number is HAL_NVIC_QUERY_EXCEPTION_NUMBER.
 *           function_isr();
 *       }
 *
 *    @endcode
 *    @code
 *       // Preserve the original IRQ settings.
 *       void example(void)
 *       {
 *           uint32_t mask;
 *           hal_nvic_save_and_set_interrupt_mask(&mask); // Save the current IRQ settings.
 *           // Do something with the protected critical resource when the IRQ is disabled.
 *           hal_nvic_restore_interrupt_mask(mask);       // Restore the IRQ settings.
 *       }
 *
 *       void test(void)
 *       {
 *           uint32_t mask;
 *           hal_nvic_save_and_set_interrupt_mask(&mask);   // Save the current IRQ settings.
 *           example();
 *           // The IRQ is still disabled;
 *           // Do something with the protected critical resouce when the IRQ is disabled.
 *           hal_nvic_restore_interrupt_mask(mask);  // Restore the original IRQ settings in the mask.
 *       }
 *    @endcode
 *
 */


#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup hal_nvic_enum Enums
  * @{
  */

/** @brief  This enum defines the return status of the NVIC API.  */
typedef enum {
    HAL_NVIC_STATUS_ERROR_NO_ISR      = -4,         /**< ISR routine is null. */
    HAL_NVIC_STATUS_ERROR_IRQ_NUMBER  = -3,         /**< Incorrect IRQ number. */
    HAL_NVIC_STATUS_INVALID_PARAMETER = -2,         /**< Invalid parameter. */
    HAL_NVIC_STATUS_ERROR             = -1,         /**< NVIC unspecified error. */
    HAL_NVIC_STATUS_OK                = 0           /**< No error occurred. */
} hal_nvic_status_t;


/**
  * @}
  */


/** @defgroup hal_nvic_typedef Typedef
  * @{
  */
/** @brief  This defines the callback function prototype.
 *          User should register a callback (an ISR) for each NVIC IRQ in use.
 *          The callback function is called in the NVIC ISR after the NVIC IRQ is triggered.
 *          For more details about the callback, please refer to #hal_nvic_register_isr_handler().
 *  @param [in] irq_number is given by driver to notify the current NVIC IRQ.
 */
typedef void (*hal_nvic_isr_t)(hal_nvic_irq_t irq_number);
/**
  * @}
  */


/** @defgroup hal_nvic_macro Define
  * @{
  */
/** This macro is used to query the current exception status of DSP.
 *  If the value is less than zero, the DSP is running thread mode, otherwise an exception occured and the exception number is the value of the macro.
 * @code
 *  // sample 1: Judge in ISR handler mode.
 *  if (HAL_NVIC_QUERY_EXCEPTION_NUMBER > HAL_NVIC_NOT_EXCEPTION ) {
 *      // handler mode process
 *  }
 *
 *  // sample 2: Judge in thread mode.
 *  if (HAL_NVIC_QUERY_EXCEPTION_NUMBER == HAL_NVIC_NOT_EXCEPTION ) {
 *      // thread mode process
 *  }
 *
 *  // sample 3: Get the IRQn number.
 *  int32_t irq_number;
 *
 *  irq_number = HAL_NVIC_QUERY_EXCEPTION_NUMBER;
 *
 *  @endcode
*/
extern int32_t nvic_irq_execution_number;

/** @brief This macro is used to query the current exception status of DSP*/
#define HAL_NVIC_QUERY_EXCEPTION_NUMBER (nvic_irq_execution_number)

/** @brief This macro defines no exception occured.*/
#define HAL_NVIC_NOT_EXCEPTION (-1)

/**
  * @}
  */


/*****************************************************************************
* Functions
*****************************************************************************/

/**
 * @brief This function is used to register a callback (an ISR) for NVIC IRQ.
 * @param[in] irq_number is the NVIC IRQ number.
 * @param[in] isr_handler is the NVIC IRQ's ISR.
 * @return    To indicate whether this function call is successful.
 *            If the return value is #HAL_NVIC_STATUS_OK, the operation completed successfully;
 *            If the return value is #HAL_NVIC_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 */
hal_nvic_status_t hal_nvic_register_isr_handler(hal_nvic_irq_t irq_number, hal_nvic_isr_t isr_handler);


/**
 * @brief This function initializes the NVIC IRQ with default priority. The lower the exception number, the higher the priority.
 */
hal_nvic_status_t hal_nvic_init(void);

/**
 * @brief This function enables a device-specific interrupt in the NVIC interrupt controller.
 * @param[in] irq_number is the NVIC IRQ number.
 */
hal_nvic_status_t hal_nvic_enable_irq(hal_nvic_irq_t irq_number);

/**
 * @brief This function disables a device-specific interrupt in the NVIC interrupt controller.
 * @param[in] irq_number is the NVIC IRQ number.
 */
hal_nvic_status_t hal_nvic_disable_irq(hal_nvic_irq_t irq_number);

/**
 * @brief This function saves the current IRQ settings in a temporary variable, and then disables the IRQ by setting the IRQ mask.
 *  It should be used in conjunction with #hal_nvic_restore_interrupt_mask() to protect the critical resources. 
 *  When the IRQ is masked, please don't print any log, it maybe lead to some unexpected errors.
 * @param[out] mask is used to store the current IRQ settings, upon the return of this function.
 * @return    To indicate whether this function call is successful.
 *            If the return value is #HAL_NVIC_STATUS_OK, the operation completed successfully.
 *            If the return value is #HAL_NVIC_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 */
hal_nvic_status_t hal_nvic_save_and_set_interrupt_mask(uint32_t *mask);

/**
 * @brief This function restores the IRQ settings as specified in the mask. It should be used in conjunction with
 *  #hal_nvic_save_and_set_interrupt_mask() to protect critical resources.
 * @param[in] mask is an unsigned integer to specify the IRQ settings.
 * @return    Indicates whether this function call is successful.
 *            If the return value is #HAL_NVIC_STATUS_OK, the operation completed successfully;
 *            If the return value is #HAL_NVIC_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 */
hal_nvic_status_t hal_nvic_restore_interrupt_mask(uint32_t mask);


/**
 * @brief This function is for trigger DSP SW IRQ
 * @param[in] irq_number is IRQ number.
 * @return    Indicates whether this function call is successful.
 *            If the return value is #HAL_NVIC_STATUS_OK, the operation completed successfully;
 *            If the return value is #HAL_NVIC_STATUS_INVALID_PARAMETER, a wrong parameter is given, the parameter must be verified.
 */
hal_nvic_status_t hal_nvic_irq_software_trigger(hal_nvic_irq_t irq_number);

#ifdef __cplusplus
}
#endif


/**
* @}
* @}
*/

#endif /*HAL_NVIC_MODULE_ENABLED*/
#endif /* __HAL_NVIC_H__ */

