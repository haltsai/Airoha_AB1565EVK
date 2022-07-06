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

#include "hal_platform.h"

#ifndef __HAL_SLEEP_MANAGER_PLATFORM_H__
#define __HAL_SLEEP_MANAGER_PLATFORM_H__

#ifdef HAL_SLEEP_MANAGER_ENABLED

typedef enum {
    /* PERISYS */
    SLEEP_LOCK_ESC_AESOTF       = 0,
    SLEEP_LOCK_CRYPTO           = 1,
    SLEEP_LOCK_TRNG             = 2,
    SLEEP_LOCK_ESC              = 3,
    SLEEP_LOCK_SPI_MST1         = 4,
    SLEEP_LOCK_SPI_MST2         = 5,
    SLEEP_LOCK_SPI_SLV          = 6,
    SLEEP_LOCK_UART1            = 7,
    SLEEP_LOCK_UART2            = 8,
    SLEEP_LOCK_I2C1             = 9,
    SLEEP_LOCK_I2C2             = 10,
    SLEEP_LOCK_AUXADC           = 11,
    SLEEP_LOCK_SLOW_DMA1        = 12,
    SLEEP_LOCK_DCXO_CFG         = 13,
    SLEEP_LOCK_PERISYS_MAX      = 14,
    /* INFRASYS */
    SLEEP_LOCK_BATTERY_MANAGEMENT = 15,
    SLEEP_LOCK_DMA              = 16,
    SLEEP_LOCK_BATTERY          = 17,
    SLEEP_LOCK_UART0            = 18,
    SLEEP_LOCK_SLOW_DMA0        = 19,
    SLEEP_LOCK_I2C0             = 20,
    SLEEP_LOCK_SPI_MST0         = 21,
    SLEEP_LOCK_MSDC             = 22,
    SLEEP_LOCK_USB              = 24,
    /* BT */
    SLEEP_LOCK_BT_CONTROLLER    = 23,
    SLEEP_LOCK_CHARGER_CASE     = 25,
    SLEEP_LOCK_BT_CONTROLLER_A2DP=26,
    SLEEP_LOCK_USER_START_ID    = 27,
    SLEEP_LOCK_INVALID_ID       = 0xFF
} sleep_management_lock_request_t;

typedef enum {
    /* PERISYS */
    SLEEP_BACKUP_RESTORE_ESC            = 0,
    SLEEP_BACKUP_RESTORE_SPI_MST1       = 1,
    SLEEP_BACKUP_RESTORE_SPI_MST2       = 2,
    SLEEP_BACKUP_RESTORE_SPI_SLV        = 3,
    SLEEP_BACKUP_RESTORE_UART1          = 4,
    SLEEP_BACKUP_RESTORE_UART2          = 5,
    SLEEP_BACKUP_RESTORE_I2C1           = 6,
    SLEEP_BACKUP_RESTORE_I2C2           = 7,
    SLEEP_BACKUP_RESTORE_AUXADC         = 8,
    SLEEP_BACKUP_RESTORE_PERISYS_MAX    = 9,
    /* INFRASYS */
    SLEEP_BACKUP_RESTORE_DMA            = 10,
    SLEEP_BACKUP_RESTORE_FLASH          = 11,
    SLEEP_BACKUP_RESTORE_UART           = 12,   //UART0
    SLEEP_BACKUP_RESTORE_I2C            = 13,   //I2C0
    SLEEP_BACKUP_RESTORE_SPI_MST0       = 14,
    SLEEP_BACKUP_RESTORE_MSDC           = 15,
    SLEEP_BACKUP_RESTORE_USB            = 16,
    SLEEP_BACKUP_RESTORE_PMU            = 17,
    SLEEP_BACKUP_RESTORE_PWM            = 18,
    SLEEP_BACKUP_RESTORE_MODULE_MAX     = 19,
    SLEEP_BACKUP_RESTORE_USER           = 20,
} sleep_management_backup_restore_module_t;

#define SLEEP_BACKUP_RESTORE_USER_CALLBACK_FUNC_MAX 8
#define DEEP_SLEEP_GPT HAL_GPT_0

#endif /* HAL_SLEEP_MANAGER_ENABLED */
#endif

