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

#include "hal_usb.h"
#include "hal_usb_internal.h"

#include "hal_eint.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "hal_pmu.h"
#include "hal_pmu_wrap_interface.h"

#ifdef HAL_USB_MODULE_ENABLED

/* MT2822 chip version */
#define CHIP_VERSION_MT2822M 0x06
extern uint32_t sub_chip_version_get(void);

void hal_usbphy_poweron_initialize(void)
{
    DRV_WriteReg32(0xA2090070, 0x01220121);
    hal_gpt_delay_ms(1);
    DRV_WriteReg32(0xA2090070, 0x00220021);
    DRV_WriteReg32((USB_PHY_MSTAR+0x868), 0x1420A620);    //U2PLL enable
    DRV_WriteReg32((USB_PHY_MSTAR+0x808), 0x80404084);    //utmi
    DRV_WriteReg32((USB_PHY_MSTAR+0x81C), 0x008880A1);    //utmi
    DRV_WriteReg32((USB_PHY_MSTAR+0x800), 0x94006BC3);    //utmi
    hal_gpt_delay_us(2000);
    DRV_WriteReg32((USB_PHY_MSTAR+0x800), 0x940069C3);    //utmi
    hal_gpt_delay_us(2000);
    DRV_WriteReg32((USB_PHY_MSTAR+0x800), 0x94000001);    //utmi
    hal_gpt_delay_us(2000);
    DRV_WriteReg32((USB_PHY_MSTAR+0x810), 0x01FF0040);    //utmi
    DRV_WriteReg32((USB_PHY_MSTAR+0x810), 0x00FF0040);    //utmi
    DRV_WriteReg32((USB_PHY_MSTAR+0x808), 0x80404004);    //utmi
    hal_gpt_delay_us(7000);
    DRV_WriteReg32((USB_PHY_MSTAR+0x838), 0x00000001);    //utmi
    DRV_WriteReg32((USB_PHY_MSTAR+0x838), 0x00000000);    //utmi

    DRV_WriteReg32((USB_PHY_MSTAR+0x800), 0x28840001);    //utmi
    DRV_WriteReg32((USB_PHY_MSTAR+0x804), 0x00403064);    //utmi
    DRV_WriteReg32((USB_PHY_MSTAR+0x808), 0x8040D507);    //utmi
    DRV_WriteReg32((USB_PHY_MSTAR+0x804), 0x00433064);    //utmi
    DRV_WriteReg32((USB_PHY_MSTAR+0x804), 0x00403064);    //utmi
    DRV_WriteReg32((USB_PHY_MSTAR+0x814), 0x0000000B);    //utmi

/* The setting of offset 0x0828
    - AB1568  : 3'b101 (Default)
    - AB1565  : 3'b100 (Default)
    - AB1565  + UMC PMIC : 3'b100
    - AB1565  + PMU PMIC : 3'b101
    - MT2822A : 3'b101 (Default)
    - MT2822M : 3'b111
*/
#if (defined(MT2822) || defined(AB1568))
    uint32_t chip_version;
    chip_version = sub_chip_version_get();

    if(chip_version == CHIP_VERSION_MT2822M)
    {
        /* MT2822M */
        DRV_WriteReg32((USB_PHY_MSTAR+0x828), (1<<7) | (1<<8) | (1<<9));
    } else {
        /* MT2822A or AB1568*/
        DRV_WriteReg32((USB_PHY_MSTAR+0x828), (1<<7) | (1<<9));
    }
#else
    /* use different setting base on a-die kgd */
    if (adie_kgd == 0x01) {
        /* AB1565 + UMC */
        log_hal_msgid_info("hal_usbphy_poweron_initialize, AB1565 + UMC PMIC, set 0x828 [9:7] as 3'b100", 0);
        DRV_WriteReg32((USB_PHY_MSTAR+0x828), (1<<9));
    } else if (adie_kgd == 0x02) {
        /* AB1565 + DBH */
        log_hal_msgid_info("hal_usbphy_poweron_initialize, AB1565 + DBH PMIC, set 0x828 [9:7] as 3'b101", 0);
        DRV_WriteReg32((USB_PHY_MSTAR+0x828), (1<<9) | (1<<7));
    } else {
        log_hal_msgid_info("hal_usbphy_poweron_initialize, AB1565(Default), set 0x828 [9:7] as 3'b100", 0);
        DRV_WriteReg32((USB_PHY_MSTAR+0x828), (1<<9));
    }
#endif

    /* Set USB PHY register from eFuse */
    uint32_t *usb_p1, *usb_p2, usb_data;
    usb_p1 = (uint32_t *)(USB_PHY_MSTAR+0x824);

    usb_p2 = (uint32_t *)0xA20A080C;
    usb_data = (((*usb_p2<< 9) & 0x00000600) | *usb_p1 );
    DRV_WriteReg32((USB_PHY_MSTAR+0x824), usb_data);

    usb_p2 = (uint32_t *)0xA20A0428;
    usb_data = (((*usb_p2<< 5) & 0x000001E0) | *usb_p1);
    DRV_WriteReg32((USB_PHY_MSTAR+0x824), usb_data);

    usb_p2 = (uint32_t *)0xA20A0810;
    usb_data = (((*usb_p2<<13) & 0x00006000) | *usb_p1);
    DRV_WriteReg32((USB_PHY_MSTAR+0x824), usb_data);

    /**
     * Set reg_test_bus_select bit 3:0 to 5
     * To get linestate(DM,DP) from 0xA1030854
     */
    USB_DRV_ClearBits32(0xA1030810, (0x0F<<0));
    USB_DRV_SetBits32(0xA1030810, (5<<0));
}

void hal_usbphy_save_current(void)
{

}

void hal_usbphy_save_current_for_charge(void)
{

}

void hal_usbphy_recover(void)
{

}

void hal_usbphy_suspend(void)
{
    /**
     * HW(0)/SW(1)      bit 0  = 1
     * RG_REF_PD        bit 2  = 1
     * RG_R_DP_PU_EN    bit 3  = 0
     * RG_R_DM_PU_EN    bit 4  = 0
     * RG_R_DP_PD_EN    bit 6  = 0
     * RG_R_DM_PD_EN    bit 7  = 0
     * DA_HS_DM_PD      bit 8  = 1
     * DA_HS_TED_PD     bit 10 = 1
     * DA_HS_PREAMP_PD  bit 11 = 1
     * DA_FL_XCVR_PD    bit 12 = 1,
     * RG_IREF_PD       bit 14 = 1
     * RG_REG_PD        bit 15 = 1 (if set, no resume)
     */
    DRV_WriteReg32(0xA1030800, 0x28840001 | 0x5D04);
    USB_DRV_ClearBits32(0xA1030810, 1<<6);  // reg_utmi_int_clr = 0
    USB_DRV_SetBits32(0xA1030808, 1<<6);    // reg_clktest_en = 1, for RG_USB_RESETB_SERDES
    USB_DRV_SetBits32(0xA1030854, 1<<4);    // RG_UPLL_PD = 1
}

void hal_usbphy_resume(void)
{
    DRV_WriteReg32(0xA1030800, 0x28840001);
    USB_DRV_SetBits32(0xA1030810, 1<<6);    // reg_utmi_int_clr = 1
    USB_DRV_ClearBits32(0xA1030808, 1<<6);  // reg_clktest_en = 0, for RG_USB_RESETB_SERDES
    USB_DRV_ClearBits32(0xA1030854, 1<<4);  // RG_UPLL_PD = 0
}

bool hal_usbphy_detect_linestate_change(void)
{
    return (USB_DRV_Reg32(0xA1030854) >> 24) & 0x01;
}

void hal_usbphy_reset_linestate_change(void)
{
    USB_DRV_SetBits32(0xA1030854, 1<<25);
    while (hal_usbphy_detect_linestate_change()) {
        ;
    }
    USB_DRV_ClearBits32(0xA1030854, 1<<25);
}

hal_usb_linestate_t hal_usbphy_detect_linestate(void)
{
    hal_usb_linestate_t ls = (USB_DRV_Reg32(0xA1030854) >> 14) & 0x03;
    // log_hal_msgid_info("hal_usbphy_detect_linestate 0x%0X DM_%d_DP_%d", 3,
    //                    USB_DRV_Reg32(0xA1030854), (ls >> 1) & 0x01, ls & 0x01);
    return ls;
}

#define USB_HS_SLEW_RATE_CAL_TIME_WINDOW 0x400
#define USB_HS_SLEW_RATE_CAL_A 32
#define USB_HS_SLEW_RATE_CAL_FRA (1000)

void hal_usbphy_slew_rate_calibration(void)
{

}

#endif /*HAL_USB_MODULE_ENABLED*/

void hal_usbphy_deinit_case(void)
{
    DRV_WriteReg32(0xA2090070, 0x01220121);
    hal_gpt_delay_ms(1);
    DRV_WriteReg32(0xA2090070, 0x00220021);
    USB_DRV_SetBits32((USB_PHY_MSTAR+0x86C), (1<<3));
}
