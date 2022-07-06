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

#ifndef __HAL_DVFS_H__
#define __HAL_DVFS_H__

#include "hal_platform.h"

#ifdef HAL_DVFS_MODULE_ENABLED


/*****************************************************************************
* Enum
*****************************************************************************/
/** @defgroup hal_dvfs_enum Enum
  * @{
  */
/** @brief This enum defines return status of certain DVFS HAL APIs. User should check return value after calling these APIs. */
typedef enum {
    HAL_DVFS_STATUS_NOT_PERMITTED  = -5,      /**< The operation is not permitted. */
    HAL_DVFS_STATUS_BUSY           = -4,      /**< Device or resource is busy. */
    HAL_DVFS_STATUS_UNINITIALIZED  = -3,      /**< Non-initialized. */
    HAL_DVFS_STATUS_INVALID_PARAM  = -2,      /**< Invalid parameter value. */
    HAL_DVFS_STATUS_ERROR          = -1,      /**< The DVFS function detected a common error. */
    HAL_DVFS_STATUS_OK             =  0       /**< The DVFS function executed successfully. */
} hal_dvfs_status_t;

/** @brief This enum defines the relationship between the target frequency and the final frequency. */
typedef enum {
    HAL_DVFS_FREQ_RELATION_L = 0,             /**< The lowest frequency at or above the target level. */
    HAL_DVFS_FREQ_RELATION_H = 1              /**< The highest frequency below or at the target level. */
} hal_dvfs_freq_relation_t;

/**
  * @}
  */

/*****************************************************************************
* extern global function
*****************************************************************************/
/**
 * @brief       This function adjusts the CPU frequency to a suitable target frequency according to the passed-in relation.
 * @param[in]   target_freq    is the target frequency in kHz.
 * @param[in]   relation       is the relationship between the target frequency and the final frequency.
 *              #HAL_DVFS_FREQ_RELATION_L, the lowest frequency at or above the target level.\n
 *              #HAL_DVFS_FREQ_RELATION_H, the highest frequency below or at the target level.\n
 *              Example 1.\n
 *              #hal_dvfs_target_cpu_frequency(103000, #HAL_DVFS_FREQ_RELATION_L). The final CPU frequency is equal to or greater than 103MHz.\n
 * @image html hal_dvfs_example1.png
 *              Example 2.\n
 *              #hal_dvfs_target_cpu_frequency(103000, #HAL_DVFS_FREQ_RELATION_H). The final CPU frequency is equal to or less than 103MHz.\n
 * @image html hal_dvfs_example2.png
 * @return      #HAL_DVFS_STATUS_OK, if OK.\n
 *              #HAL_DVFS_STATUS_INVALID_PARAM, if the parameter value is invalid.\n
 *              #HAL_DVFS_STATUS_ERROR, if an error occurred.\n
 */
hal_dvfs_status_t hal_dvfs_target_cpu_frequency(uint32_t target_freq, hal_dvfs_freq_relation_t relation);

/**
 * @brief       This function gets the current CPU frequency.
 * @return      The current CPU frequency is in kHz.
 */
uint32_t hal_dvfs_get_cpu_frequency(void);

/**
 * @brief       This function gets all adjustable CPU frequencies listed in the OPP table.
 * @param[out]   list          is a double pointer to the frequency table.
 * @param[out]   list_num      is a pointer to the number of the entries in the frequency table.
 * @return      #HAL_DVFS_STATUS_OK, if the operation completed successfully.\n
 *              #HAL_DVFS_STATUS_ERROR, if an error occurred during the operation..\n
 */
hal_dvfs_status_t hal_dvfs_get_cpu_frequency_list(const uint32_t **list, uint32_t *list_num);

/**
* @}
* @}
*/

#endif /* HAL_DVFS_MODULE_ENABLED */

#endif /* __HAL_DVFS_H__ */
