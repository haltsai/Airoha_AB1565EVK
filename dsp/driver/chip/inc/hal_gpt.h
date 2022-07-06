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
 
#ifndef __HAL_GPT_H__
#define __HAL_GPT_H__
#include "hal_platform.h"

#ifdef HAL_GPT_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * This section introduces the General Purpose Timer(GPT) driver APIs including terms and acronyms, supported features, software architecture, details on how to use this driver, GPT function groups, enums, structures and functions.
 *
 * @section HAL_GPT_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b GPT                        | General Purpose Timer(GPT) is used as an alarm clock for timing.|
 * |\b NVIC                       | The Nested Vectored Interrupt Controller (NVIC) is the interrupt controller of ARM Cortex-M. For more details, please refer to <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100166_0001_00_en/ric1417175922867.html"> NVIC introduction in ARM Cortex-M4 Processor Technical Reference Manual </a>.|
 *
 * @section HAL_GPT_Features_Chapter Supported features
 *
 * This controller has a generic design to support various combinations of the timer functionality.
 * - \b Support \b delay \b function. \n
 *   The GPT driver supports delays in milliseconds #hal_gpt_delay_ms() and microseconds #hal_gpt_delay_us().
 *   It utilizes the free run mode of the timer and the polling mechanism to determine if the timeout for the timer is reached.\n
 *   The delay function is a hardware delay. It is for some programs to delay for a specified period without the need for the context switch. \n
 * @}
 * @}
 */

#ifdef HAL_GPT_SW_GPT_FEATURE
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * - \b Software \b timer \b supporting \b multiple \b users. \n
 *   The software timer supports multiple users.
 *   It is based on GPT driver, and occupies the hardware GPT port. \n
 *   The software timer uses GPT oneshot mode. The maximum number of users is 32.
 *   The unit of the software timer is milliseconds.
 * @}
 * @}
 */
#endif

/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * @section HAL_GPT_Driver_Usage_Chapter How to use this driver
 * - \b Using \b GPT \b in \b free \b run \b mode. \n
 *    Call #hal_gpt_get_free_run_count() to get the first stamp of timer ticks.\n
 *    Call #hal_gpt_get_free_run_count() again to get the second stamp of timer ticks.\n
 *    If HAL_GPT_CLOCK_SOURCE_32K parameter is in use, the unit of tick is 1/32768 seconds.
 *    The difference between the first and the second stamps is the counting ticks.
 *    In a free run mode, the GPT runs continuously and never stops. It does not provide interrupt feature.
 *    The GPT driver also uses this function implementation similar to the delay function #hal_gpt_delay_ms().\n
 *  - Step1: Call #hal_gpt_get_free_run_count() to get the first tick count.
 *  - Step2: Call #hal_gpt_get_free_run_count() to get the second tick count.
 *  - Step3: Call #hal_gpt_get_duration_count() to get the duration time.
 *  - Sample code:
 *    @code
 *       hal_gpt_status_t         ret_status;
 *       uint32_t                 count1, count2, duration_count;
 *       uint32_t                 time;
 *
 *       //Get the first value.
 *       if(HAL_GPT_STATUS_OK != hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count1)) {
 *          //error handle
 *       }
 *
 *       hal_gpt_delay_ms(10);                               //Delay for 10 ms.
 *
 *       //Get the second value.
 *       if(HAL_GPT_STATUS_OK != hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count2)) {
 *          //error handle
 *       }
 *
 *       //Calculate count1 and count2 durations, the duration unit is 1/32768 seconds.
 *       if(HAL_GPT_STATUS_OK != hal_gpt_get_duration_count(count1, count2, &duration_count)) {
 *          //error handle
 *       }
 *
 *       time = (duration_count*1000)/32768;                //The time duration in milliseconds.
 *    @endcode
 * @}
 * @}
 */

/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 *  -
 *    If the parameter HAL_GPT_CLOCK_SOURCE_1M is used in #hal_gpt_get_free_run_count(),
 *    the unit of the tick count is in microseconds.
 *    For more details about HAL_GPT_CLOCK_SOURCE_1M, please refer to @ref hal_gpt_enum about hal_gpt_clock_source_t.
 *  - Sample code:
 *    @code
 *       hal_gpt_status_t         ret_status;
 *       uint32_t                 count1, count2, duration_count;
 *       uint32_t                 time;
 *
 *       //Get the first value.
 *       if(HAL_GPT_STATUS_OK != hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &count1)) {
 *          //error handle
 *       }
 *
 *       hal_gpt_delay_us(10);                       //Delay for 10 us.
 *
 *       //Get the second value.
 *       if(HAL_GPT_STATUS_OK != hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &count2)) {
 *          //error handle
 *       }
 *
 *       //Calculate count1 and count2 durations, the duration unit is 1 microseconds.
 *       if(HAL_GPT_STATUS_OK != hal_gpt_get_duration_count(count1, count2, &duration_count)) {
 *          //error handle
 *       }
 *
 *       time = duration_count;                      //The time duration in microseconds.
 *    @endcode
 * @}
 * @}
 */

/**
* @addtogroup HAL
* @{
* @addtogroup GPT
* @{
* - \b Using \b the \b GPT \b delay \b function. \n
*   Call #hal_gpt_delay_ms() function to set the delay time.
*   When in a delay, the GPT driver uses free run mode and polls register value until it expires.
*  - Sample code:
*    @code
*       hal_gpt_delay_ms(10);  //Delay for 10 ms.
*    @endcode
* @}
* @}
*/

#ifdef HAL_GPT_FEATURE_US_TIMER
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 *  - Sample code:
 *    @code
 *       hal_gpt_delay_us(10);  //Delay for 10 us.
 *    @endcode
 * @}
 * @}
 */
#endif

#ifdef HAL_GPT_SW_GPT_FEATURE
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * - \b Using \b software \b timer. \n
 *    Call #hal_gpt_sw_get_timer() to allocate a software timer handle.\n
 *    Call #hal_gpt_sw_start_timer_ms() to register the callback function and start the timer.\n
 *    The unit of the software timer is milliseconds.\n
 *    The software timer is a oneshot timer. To run the software timer continuously, call it again in its callback function.
 *    Call #hal_gpt_sw_stop_timer_ms() to stop the running timer.
 *    If the timer is not running, the #HAL_GPT_STATUS_ERROR is returned.
 *    Call #hal_gpt_sw_get_remaining_time_ms() to get the remaining timeout value at any time.
 *    Call #hal_gpt_sw_free_timer() to free the allocated timer resource.
 *  - Step1: Call #hal_gpt_sw_get_timer() to allocate a software timer handle.
 *  - Step2: Call #hal_gpt_sw_start_timer_ms() to register the callback function and start the timer.
 *  - Step3: Call #hal_gpt_sw_free_timer() to free the allocated timer if it's no longer in use.
 *  - Sample code:
 *    @code
 *       uint32_t                 handle;
 *
 *       //Get the allocated timer handle
 *       if (HAL_GPT_STATUS_OK != hal_gpt_sw_get_timer(&handle) ) {
 *          //error handler
 *       }
 *
 *       //Set the timeout to 10 ms, and register the callback at the same time
 *       if(HAL_GPT_STATUS_OK != hal_gpt_sw_start_timer_ms(handle, 10, user_gpt_callback, NULL)) {
 *          //error handler
 *       }
 *
 *       //....
 *       //Waiting for the timer to timeout.
 *    @endcode
 *    @code
 *       // Callback function.
 *       void user_gpt_callback (void *user_data)
 *       {
 *           // user's handler
 *           hal_gpt_sw_free_timer(handle);                          //If it's no longer in use.
 *       }
 *
 *    @endcode
 * @}
 * @}
 */
#endif

#ifdef HAL_GPT_SW_FEATURE_ABSOLUTE_COUNT
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * - \b Using \b software \b timer. \n
 *   Call #hal_gpt_sw_get_timer() to allocate a software timer handle.\n
 *   Call #hal_gpt_get_free_run_count() to get current count in 32K timeline.\n
 *   Call #hal_gpt_sw_start_timer_for_absolute_tick_32K() to register the callback function and start the timer.\n
 *   The unit of the software timer is using the same timeline as the ms channel .\n
 *   The software timer is a oneshot timer. To run the software timer continuously, call it again in its callback function.\n
 *   Call #hal_gpt_sw_stop_timer_ms() to stop the running timer.
 *   If the timer is not running, the #HAL_GPT_STATUS_ERROR is returned.
 *   Call #hal_gpt_sw_free_timer() to free the allocated timer resource.
 * - step1: Call #hal_gpt_sw_get_timer() to allocate a software timer handle.
 * - step2: Call #hal_gpt_get_free_run_count() to get current count in 32K timeline.
 * - step3: Call #hal_gpt_sw_start_timer_for_absolute_tick_32K() to register the callback function and start the timer.
 * - step4: Call #hal_gpt_sw_free_timer() to free the allocated timer if it's no longer in use.
 * - sample code:
 *   @code
 *    hal_sw_gpt_absolute_parameter_t  absolute_parameter;
 *    uint32_t count_1 , handle;
 *     //Get the allocated timer handle.
 *    if (HAL_GPT_STATUS_OK != hal_gpt_sw_get_timer(&handle) ) {
 *       //error handle 
 *    }
 *      // get current count in 32K timeline.
 *    if(HAL_GPT_STATUS_OK != hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count1)){
 *       //error handle
 *     }
 *     // init the start timer parameter include the expired absolute_count , callback function, user data, maxdelay_time_count.
 *     // constant is setted by user.
 *     absolute_parameter.absolute_count = count_1 + constant;
 *     absolute_parameter.callback = user_gpt_callback;
 *     absolute_parameter.user_data = user_data;
 *     absolute_parameter.maxdelay_time_count = constant;
 *    // pass the absolute_parameter to function.
 *    if(HAL_GPT_STATUS_OK != hal_gpt_sw_start_timer_for_absolute_tick_32K(handle,&absolute_parameter)) {
 *      //error handle
 *     }
 *    //......
 *    //Waiting for the timer to timeout.
 *   @endcode
 *   @code
 *    // Callback function.
 *    void user_gpt_callback (void *user_data){
 *     user's handler
 *    hal_gpt_sw_free_timer(handle);                  // if it's no longer in use.
 *    }
 *
 *   @endcode
 * @}
 * @}
 */
#endif

/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup hal_gpt_typedef Typedef
  * @{
  */

/** @brief  Register a callback function when using GPT oneshot or repeat modes.
  *         The callback is called after the timer expires and triggers an interrupt in the GPT ISR routine.
  * @param[in] user_data is the pointer to the user defined data to pass to the callback function.
  */
typedef void(* hal_gpt_callback_t)(void *user_data);

/**
  * @}
  */
/** @defgroup hal_gpt_enum Enum
  * @{
  */
/** @brief This enum defines the GPT timer type. */
typedef enum {
    HAL_GPT_TIMER_TYPE_ONE_SHOT = 0,                /**< Set the GPT oneshot mode.  */
    HAL_GPT_TIMER_TYPE_REPEAT   = 1                 /**< Set the GPT repeat  mode.  */
} hal_gpt_timer_type_t;


/** @brief This enum defines the GPT status. */
typedef enum {
    HAL_GPT_STATUS_ERROR_RESTART_ERROR   = -7,    /**< The SW GPT is restart the same timer that has not expired. */
    HAL_GPT_STATUS_ERROR_START_TOO_LONG  = -6,    /**< The user start time_out  is too long. */
    HAL_GPT_STATUS_ERROR_PORT_USE_FULL   = -5,    /**< The SW GPT users full. */
    HAL_GPT_STATUS_ERROR_PORT_USED   = -4,         /**< The timer has beed used. */
    HAL_GPT_STATUS_ERROR             = -3,         /**< GPT function error occurred. */
    HAL_GPT_STATUS_ERROR_PORT        = -2,         /**< A wrong GPT port is set. */
    HAL_GPT_STATUS_INVALID_PARAMETER = -1,         /**< An invalid parameter. */
    HAL_GPT_STATUS_OK   = 0                        /**< No error occurred during the function call.*/
} hal_gpt_status_t;


/** @brief This enum defines the GPT running status. */
typedef enum {
    HAL_GPT_STOPPED = 0,                            /**< The GPT has stopped. */
    HAL_GPT_RUNNING = 1                             /**< The GPT is running. */
} hal_gpt_running_status_t;

#ifdef HAL_GPT_SW_FEATURE_ABSOLUTE_COUNT
/** @brief This struct gives definitions for parameter passed to the hal_gpt_sw_start_timer_for_absolute_time_ms/us . */
typedef struct{
    uint32_t               absolute_time_count;    /**< The  absolute time count on the ms/us timeline. */
    hal_gpt_callback_t     callback;               /**< This callback will be called when the timer time out. */
    void                   *user_data;             /**< user_data is the address of data to pass to the callback function. */
    uint32_t               maxdelay_time_count;    /**< the user should pass the parameter to ensure that the expire_time is smaller than 
                                                         maxdelay_time_count which is the same as the ms or us channel . */
}hal_sw_gpt_absolute_parameter_t;
#endif

/**
  * @}
  */

/**
 * @brief      This function gets the current count of timer in the free run mode.
 * @param[in]  clock_source is the clock source of the timer in the free run mode.
 *             For more details, please refer to @ref hal_gpt_enum about hal_gpt_clock_source_t.
 * @param[out] count is the user's pointer to a parameter to get the count value.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 */
hal_gpt_status_t hal_gpt_get_free_run_count(hal_gpt_clock_source_t clock_source, uint32_t *count);

/**
 * @brief     This function sets the delay time in milliseconds.
 *            The maximum delay time = 1/GPT_clock * 0xffffffff.
 * @param[in] ms is the delay time in milliseconds.
 * @return    #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *            #HAL_GPT_STATUS_ERROR_PORT, if the gpt_port value is wrong.
 */
hal_gpt_status_t hal_gpt_delay_ms(uint32_t ms);

/**
 * @brief     This function sets delay time in microseconds.
 *            The maximum delay time = 1/GPT_clock * 0xffffffff.
 * @param[in] us is the delay time in microseconds.
 * @return    #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *            #HAL_GPT_STATUS_ERROR_PORT, if the gpt_port value is wrong.
 */
hal_gpt_status_t hal_gpt_delay_us(uint32_t us);


/**
 * @brief      This function calculates the count duration. This API is to help user cover count value rollback problem.
 *             For example, if start_count < end_count, it means the count value has grown up from 0xffffffff to 0.
 * @param[in]  start_count is the value of start count.
 * @param[in]  end_count is the value of end count.
 * @param[out] duration_count is the user's parameter pointer to get count duration.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the duration_count is null.
 */
hal_gpt_status_t hal_gpt_get_duration_count(uint32_t start_count, uint32_t end_count, uint32_t *duration_count);

#ifdef HAL_GPT_SW_GPT_FEATURE
/**
 * @brief      This function allocates timer handle.
 * @param[out] handle is an unsigned integer for accessing this timer. It's equivalent to an ID number of the timer.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_ERROR, if a timer cannot be allocated.
 */
hal_gpt_status_t hal_gpt_sw_get_timer(uint32_t *handle);

/**
 * @brief      This function frees timer.
 * @param[in]  handle is the handle of the timer to be freed.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the handle is invalid. \n
 *             #HAL_GPT_STATUS_ERROR, for all other errors.
 */
hal_gpt_status_t hal_gpt_sw_free_timer(uint32_t handle);

/**
 * @brief      This function starts the software timer.
 * @param[in]  handle is the handle of the timer to be started.
 * @param[in]  timeout_time_ms is the timeout value in milliseconds.
 * @param[in]  callback is the callback to be registered with this timer.
 *             This callback will be called when the timer times out.
 * @param[in]  user_data is the pointer to the user defined data to pass to the callback function.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the parameter is invalid. \n
 *             #HAL_GPT_STATUS_ERROR, for all other errors.
 */
hal_gpt_status_t hal_gpt_sw_start_timer_ms(uint32_t handle, uint32_t timeout_time_ms, hal_gpt_callback_t callback, void *user_data);

/**
 * @brief      This function stops the specified software timer.
 * @param[in]  handle is the handle of the timer to be stopped.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the handle is invalid. \n
 *             #HAL_GPT_STATUS_ERROR, for all other errors.
 */
hal_gpt_status_t hal_gpt_sw_stop_timer_ms(uint32_t handle);

/**
 * @brief      This function gets the remaining timeout value of the specified software timer.
 * @param[in]  handle is the handle of the timer to get the remaining time from.
 * @param[out] remaing_time is the pointer to the value of the remaining timeout, after the return of this function.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the handle is invalid. \n
 *             #HAL_GPT_STATUS_ERROR, if the handle is not allocated.
 */
hal_gpt_status_t hal_gpt_sw_get_remaining_time_ms(uint32_t handle, uint32_t *remaing_time);
#ifdef HAL_GPT_SW_FEATURE_ABSOLUTE_COUNT
/**
 * @brief      This function starts the software timer by setting the absolute time count for future expirations on the 32K timeline.
 * @param[in]  handle is the handle of the timer to be started.
 * @param[in]  absolute_parameter specifies parameters for configuring start timer.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the handle is invalid or parameter is invalid.\n
 *             #HAL_GPT_STATUS_ERROR, if the time count after expiration has exceeded the set absolute_parameter.maxdelay_time_count. \n
 *             #HAL_GPT_STATUS_ERROR_RESTART_ERROR ,if the handle timer has been started.
 */
hal_gpt_status_t hal_gpt_sw_start_timer_for_absolute_tick_32K(uint32_t handle , hal_sw_gpt_absolute_parameter_t *absolute_parameter);
#endif
#ifdef HAL_GPT_SW_GPT_US_FEATURE
/**
 * @brief      This function starts the software timer.
 * @param[in]  handle is the handle of the timer to be started.
 * @param[in]  timeout_time_us is the timeout value in microseconds.
 * @param[in]  callback is the callback to be registered with this timer.
 *             This callback will be called when the timer times out.
 * @param[in]  user_data is the pointer to the user defined data to pass to the callback function.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the parameter is invalid. \n
 *             #HAL_GPT_STATUS_ERROR, for all other errors.
 */
hal_gpt_status_t hal_gpt_sw_start_timer_us(uint32_t handle, uint32_t timeout_time_us, hal_gpt_callback_t callback, void *user_data);
/**
 * @brief      This function stops the specified software timer.
 * @param[in]  handle is the handle of the timer to be stopped.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the handle is invalid. \n
 *             #HAL_GPT_STATUS_ERROR, for all other errors.
 */
hal_gpt_status_t hal_gpt_sw_stop_timer_us(uint32_t handle);
/**
 * @brief      This function gets the remaining timeout value of the specified software timer.
 * @param[in]  handle is the handle of the timer to get the remaining time from.
 * @param[out] remaing_time is the pointer to the value of the remaining timeout in microseconds, after the return of this function.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the handle is invalid. \n
 *             #HAL_GPT_STATUS_ERROR, if the handle is not allocated.
 */
hal_gpt_status_t hal_gpt_sw_get_remaining_time_us(uint32_t handle, uint32_t *remaing_time);
#ifdef HAL_GPT_SW_FEATURE_ABSOLUTE_COUNT
/**
 * @brief      This function starts the software timer by setting the absolute time count for future expirations on the 1M chanel timeline.
 * @param[in]  handle is the handle of the timer to be started.
 * @param[in]  absolute_parameter specifies parameters for configuring start timer.
 * @return     #HAL_GPT_STATUS_OK, if the operation is successful.\n
 *             #HAL_GPT_STATUS_INVALID_PARAMETER, if the handle is invalid or parameter is invalid.\n
 *             #HAL_GPT_STATUS_ERROR, the time count after expiration has exceeded the set absolute_parameter.maxdelay_time_count. \n
 *             #HAL_GPT_STATUS_ERROR_RESTART_ERROR ,if the handle timer has been started.
 */
hal_gpt_status_t hal_gpt_sw_start_timer_for_absolute_tick_1M(uint32_t handle , hal_sw_gpt_absolute_parameter_t *absolute_parameter);
#endif
#endif
#endif




#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/
#endif /*HAL_GPT_MODULE_ENABLED*/
#endif /* __HAL_GPT_H__ */


