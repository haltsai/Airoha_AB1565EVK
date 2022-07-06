/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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

#ifndef _SY3088_H_
#define _SY3088_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal.h"

#define SY3088PXS_I2C_ADDRESS   0x1E //7 bit Address
#define SY3088PXS_VENDOR_ID     0x27

/* REGISTER ADDRESS */
#define REG_ID                  0x00 // Product ID: 0x27
#define REG_CON0                0x01
#define REG_CON1                0x02
#define REG_PXS_TL              0x03 // PS Threshold Low
#define REG_PXS_TH              0x04 // PS Threshold High
#define REG_PXS_DATA            0x05 // PS DATA
#define REG_INT_FLAG            0x08 // INT FLAG
#define SY3088AS_MAX_REG        9

/* Reg01 Parameter */
#define AP_EN_PXS               0x40

/* Reg02 Parameter */
/*PXS Interrupt Type*/
#define PXS_PITYPE_WINDOW       0x00
#define PXS_PITYPE_EVENT        0x80 //Hysteresis type

/*PXS Waiting time(ADC cycle time)*/
#define PXS_WAIT_6_25ms         0x00
#define PXS_WAIT_12_5ms         0x10
#define PXS_WAIT_25ms           0x20
#define PXS_WAIT_50ms           0x30

/*PXS Current Setting*/
#define PXS_IR_DRV_10mA         0x00
#define PXS_IR_DRV_15mA         0x08

/*PXS Interrupt Persistence*/
/*  000, interrupt is disabled
    001, set PXS_FLAG if 1 reading trips the threshold value
    010, set PXS_FLAG if 2 consecutive readings trip the threshold value
    011, set PXS_FLAG if 4 consecutive readings trip the threshold value
    100, set PXS_FLAG if 8 consecutive readings trip the threshold value
    101, set PXS_FLAG if 16 consecutive readings trip the threshold value
    110, set PXS_FLAG if 32 consecutive readings the threshold value
    111, every proximity cycle generates an interrupt */
#define PXS_DISABLE_INTR        0x00
#define PXS_PRST_1              0x01
#define PXS_PRST_2              0x02
#define PXS_PRST_4              0x03
#define PXS_PRST_8              0x04
#define PXS_PRST_16             0x05
#define PXS_PRST_32             0x06
#define PXS_PRST_EVERY_CYCLE    0x07

/* Interrupt flag mask bit*/
#define PXS_INT_FLAG_BG_OVF_MASK    0x10 /* When=1, background light overflows. PXS_DATA is 0; when=0, background light does not overflow */
#define PXS_INT_FLAG_EVENT_MASK     0x80 /* when=1, a PXS interrupt event occurred; when=0, no PXS interrupt has occurred since power-onor last "clear" */

/* Special Function */
#define CLEAR_PXS_FLAG          0x21
#define SOFTWARE_RESET          0x22

/* SENSOR SETTING For Customer */
#define PXS_IR_DRV              PXS_IR_DRV_10mA
#define INTR_TYPE               PXS_PITYPE_WINDOW /* low level trigger or falling edge trigger */
#define PXS_WAIT                PXS_WAIT_50ms
#define PXS_PRST                PXS_PRST_4
#define PXS_OFFSET_MAX          150
#define PXS_FAR_DIFF            10              /* user can adjust far distance */
#define PXS_NEAR_DIFF           100             /* user can adjust near distance */
#define PXS_LOW_DIFF            5
#define PXS_STABLE_SLOPE        10

/* PS THRESHOLD MAX VALUE */
#define PXS_FAR_MAX             255             /* ADC 8 bit Max */

#define PXS_ABS(x) (x) >= 0 ? (x):(x)*(-1)

#define PXS_DATA_BUFFER_SIZE 5

typedef enum {
    PXS_STATUS_FAR_AWAY,
    PXS_STATUS_NEAR_BY,
    PXS_STATUS_INIT,
} sy3088_status_t;

typedef struct {
    uint8_t   baseline;
    uint8_t   low_threshold;
    uint8_t   high_threshold;
    bool      is_init_saturated;
    bool      is_initalized;
    sy3088_status_t  status;
    sy3088_status_t  last_status;
} sy3088_context_t;

typedef struct {
    uint32_t average;
    bool is_stable;
    uint8_t index;
    uint8_t data[PXS_DATA_BUFFER_SIZE];
} sy3088_pxs_sample_t;


typedef int32_t (*sy3088_write_ptr)(uint8_t addr, uint8_t reg_addr, uint8_t* buffer, uint32_t buffer_len);
typedef int32_t (*sy3088_read_ptr)(uint8_t addr, uint8_t reg_addr, uint8_t* buffer, uint32_t buffer_len);

int sy3088_init(sy3088_write_ptr write_ptr, sy3088_read_ptr read_ptr);
void sy3088_deinit(void);
bool sy3088_get_sensor_status(sy3088_status_t *status);

#endif //_SY3088_H_
