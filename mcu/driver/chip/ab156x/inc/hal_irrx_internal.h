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

#ifndef __HAL_IRRX_INTERAL_H
#define __HAL_IRRX_INTERAL_H


#include "hal_platform.h"

#ifdef HAL_IRRX_MODULE_ENABLED
#include "hal_irrx.h"

#define IRRX_DEBUG_LOG printf
//#define IRRX_DEBUG

typedef enum {
    IRRX_NEC_PROTOCOL = 0,
    IRRX_RC5_PROTOCOL = 1,
    IRRX_RC6_PROTOCOL = 2,
    IRRX_RCMM_PROTOCOL = 3,
    IRRX_SIRC_BIT20_PROTOCOL = 4,
} irrx_protocol_t;

typedef struct {
    uint8_t cmd; /*7 bit(lsb)*/
    uint8_t addr; /*5 bit(lsb)*/
} irrx_sirc_bit12_protocol_t;

typedef struct {
    uint8_t cmd; /*7 bit(lsb)*/
    uint8_t addr; /*8 bit(lsb)*/
} irrx_sirc_bit15_protocol_t;

typedef struct {
    uint8_t cmd; /*7 bit(lsb)*/
    uint8_t addr; /*5 bit(lsb)*/
    uint8_t extend; /*8 bit(lsb)*/
} irrx_sirc_bit20_protocol_t;

typedef struct {
    uint8_t addr; /*8 bit (lsb)*/
    uint8_t addr_inverse; /*8 bit (lsb)*/
    uint8_t cmd; /*8 bit (lsb)*/
    uint8_t cmd_inverse; /*8 bit*/
} irrx_nec_protocol_t; /*8 bit (lsb)*/

typedef struct {
    uint8_t toggle_bit; /*1 or 0*/
    uint8_t addr; /*5 bit (msb)*/
    uint8_t cmd; /*6 bit (msb)*/
} irrx_rc5_protocol_t;

typedef struct {
    uint8_t header; /*6 bit*/
    uint8_t ctrl; /*8 bit*/
    uint8_t info; /*8 bit*/
} irrx_rc6_mode0_protocol_t;

typedef struct {
    uint8_t length;
    uint8_t data[112]; /*hardware max 112 byte*/
} irrx_rcmm_protocol_t;

/*irrx base address define*/
#define IRRX_REG  ((IRRX_REGISTER_T *)0xA2200000) /*defined in ab156x.h*/


//test
typedef enum {
    HAL_IRRX_WAKEUP_ANY_KEY = 0,      /**< any key for wakeup*/
    HAL_IRRX_WAKEUP_SPECIAL_KEY = 1   /**< special key for wakeup */
} hal_irrx_wakeup_type_t;

typedef struct {
    uint32_t m; //for config register
    uint32_t l; // for config register
    uint8_t wakeup_bit_cnt;
} hal_irrx_wakeup_key;

void irrx_nvic_set(bool enable);
void irrx_power_set(bool is_power_on);
void irrx_register_callback(hal_irrx_callback_t callback);
void irrx_read_hardware_mode_data(uint8_t *data, uint8_t length);
void irrx_wakeup_callback(void *user_data);
void irrx_register_dump(void);
void irrx_decode_nec_data(uint8_t *data, uint8_t length, irrx_nec_protocol_t *protocol);
void irrx_decode_rc5_data(uint8_t *data, uint8_t length, irrx_rc5_protocol_t *protocol);
void irrx_decode_rc6_data(uint8_t *data, uint8_t length, irrx_rc6_mode0_protocol_t *protocol);
void irrx_decode_rcmm_data(uint8_t *data, uint8_t length, irrx_rcmm_protocol_t *protocol);
void irrx_decode_sirc_12bit_data(uint8_t *data, uint8_t length, irrx_sirc_bit12_protocol_t *protocol);
void irrx_decode_sirc_15bit_data(uint8_t *data, uint8_t length, irrx_sirc_bit15_protocol_t *protocol);
void irrx_decode_sirc_20bit_data(uint8_t *data, uint8_t length, irrx_sirc_bit20_protocol_t *protocol);

#endif

#endif
