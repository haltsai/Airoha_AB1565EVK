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

#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "boolean.h"



////////////////////////////////////////////////////////////////////////////////
// CHIP MODEL DEFINITIONS //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/**
 * ASIC / FPGA mode selection
 */
//- don't touch
#define ASIC 0
#define FPGA 1
#define FPGA_BUILD                                                              (BUILD_OPTION == FPGA)
#define ASIC_BUILD                                                              (BUILD_OPTION == ASIC)
//- configuration
#define BUILD_OPTION                                                            FPGA

/**
 * RF selction
 */
#define OPERATION_IN_RF_AB1530                                                  (TRUE)

/**
 * System mode selction
 */
#define FEA_FORCE_BT_DEVICE_MODE                                                (FALSE)

/**
 * Debug
 */
#define FEA_DBG_PRINT                                                           (FALSE)

/**
 * Momery Configurations
 */

/**
 * Common Assembly Speedup Options
 */
#define FEA_BT_TIMER_SPEEDUP                                                    (TRUE)

/**
 * OS Assembly Speedup Options
 */
#define FEA_OSMEM_SPEEDUP                                                       (TRUE)
#define FEA_OSMEM_LIST_SPEEDUP                                                  (TRUE)
#define FEA_OSMEM_DLIST_SPEEDUP                                                 (TRUE)
#define FEA_OSTASK_SPEEDUP                                                      (FALSE)
#define FEA_OST_SPEEDUP                                                         (TRUE)

/**
 * RC
 */
#define FEA_SYS_SPEEDUP                                                         (TRUE)
#define FEA_BT_CLOCK_SPEEDUP                                                    (TRUE)
#define FEA_CRC16_SPEEDUP                                                       (TRUE)
#define FEA_SUPP_LAB_TEST                                                       (TRUE)

/**
 * Driver Assembly Speedup Options
 */
#define FEA_3WIRE_SPEEDUP                                                       (FALSE)
#define FEA_DMA_SPEEDUP                                                         (TRUE)
#define FEA_GPIO_SPEEDUP                                                        (TRUE)

/**
 * DSP
 */
#define FEA_SUPP_DSP                                                            (FALSE)

/**
 * LC
 */
#define FEA_LC_SPEEDUP                                                          (TRUE)

/**
 * NEW_OSMEM FEATUREs
 */
#define NUM_OF_SUPPORTED_LE_ACL_LINK                                            (4)
#define NUM_OF_SUPPORTED_BT_ACL_LINK                                            (2)
#define NUM_OF_SUPPORTED_ACL_LINK                                               (NUM_OF_SUPPORTED_LE_ACL_LINK + NUM_OF_SUPPORTED_BT_ACL_LINK)
#define NUM_OF_SUPPORTED_LE_ACL_LINK_IN_DEVICE_MODE                             (1)

/**
 * I2C
 */
#define FEA_SUPP_I2C                                                            (TRUE)

/**
 * EEPROM
 */
#define FEA_SUPP_EEPROM                                                         (TRUE && FEA_SUPP_I2C)

/**
 * Flash Programmer
 */

/**
 * Watchdog
 */
#define FEA_SUPP_WDT                                                            (TRUE)

/**
 * GPIO
 */
#define FEA_SUPP_GPIO                                                           (TRUE)

// TEMP
#define MMI_HOST_RELATED														(FALSE)
#define FLOAT_CAL_REMOVE_RELATED												(TRUE)
#define HC_CMD_TAB_REMOVE_RELATED							        			(TRUE)

/**
 * Start-up Calibration
 */
#define START_UP_CAL                                                            (TRUE && ASIC_BUILD)

/**
 * Temperature Compensation
 */
#define TEMP_COMPENSATION                                                       (TRUE && ASIC_BUILD)


////////////////////////////////////////////////////////////////////////////////
// Legacy Definitions //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// OS related define
#define OS_DEBUG
#define OS_TRAP_DBG
#define OS_ASSERT_DBG
#define FEA_TREAT_DBGPRINT_AS_HCI_EVENT     (1)
#define CHARLES_DEBUG                       (1)

#define OS_PRIORITY                                                              (TRUE)



////////////////////////////////////////////////////////////////////////////////
// Configuration Checks ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif /* _CONFIG_H_ */

