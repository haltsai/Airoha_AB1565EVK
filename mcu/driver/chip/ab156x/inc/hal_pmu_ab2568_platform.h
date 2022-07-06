/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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

#ifndef __HAL_PMU_PLATFORM_H__
#define __HAL_PMU_PLATFORM_H__
#ifdef HAL_PMU_MODULE_ENABLED

#define PMU_BASE    (0x0000)
#define PMU_HWCID                    (PMU_BASE + 0x0000)
#define PMU_SWCID                    (PMU_BASE + 0x0002)
#define PMU_IOCFG0                   (PMU_BASE + 0x0004)
#define PMU_IOCFG1                   (PMU_BASE + 0x0006)
#define PMU_IOCFG2                   (PMU_BASE + 0x0008)
#define PMU_IOCFG3                   (PMU_BASE + 0x000A)
#define PMU_IOCFG4                   (PMU_BASE + 0x000C)
#define PMU_IOCFG5                   (PMU_BASE + 0x000E)
#define PMU_IOCFG6                   (PMU_BASE + 0x0010)
#define PMU_IOCFG7                   (PMU_BASE + 0x0012)
#define PMU_TEST_OUT                 (PMU_BASE + 0x0014)
#define PMU_TEST_CON0                (PMU_BASE + 0x0016)
#define PMU_DBGCTL0                  (PMU_BASE + 0x0018)
#define PMU_DBGCTL1                  (PMU_BASE + 0x001A)
#define PMU_CKCFG0                   (PMU_BASE + 0x001C)
#define PMU_CKCFG1                   (PMU_BASE + 0x001E)
#define PMU_CKCFG2                   (PMU_BASE + 0x0020)
#define PMU_CKCFG3                   (PMU_BASE + 0x0022)
#define PMU_I2C_OUT_TYPE             (PMU_BASE + 0x0024)
#define PMU_RSTCFG0                  (PMU_BASE + 0x0026)
#define PMU_RSTCFG1                  (PMU_BASE + 0x0028)
#define PMU_RSTCFG2                  (PMU_BASE + 0x002A)
#define PMU_RSTCFG3                  (PMU_BASE + 0x002C)
#define PMU_PORFLAG                  (PMU_BASE + 0x002E)
#define PMU_VBATLDOSW                (PMU_BASE + 0x0030)
#define PMU_VAUD18DRV_MODE           (PMU_BASE + 0x0032)
#define PMU_AO_EFUSE_RSV             (PMU_BASE + 0x0034)
#define PMU_TESTKEY                  (PMU_BASE + 0x0036)
#define PMU_CAP_LPSD_CON0            (PMU_BASE + 0x0038)
#define PMU_CAP_LPSD_CON1            (PMU_BASE + 0x003A)
#define PMU_INT_CON0                 (PMU_BASE + 0x003C)
#define PMU_INT_CON0_SET             (PMU_BASE + 0x003E)
#define PMU_INT_CON0_CLR             (PMU_BASE + 0x0040)
#define PMU_INT_CON1                 (PMU_BASE + 0x0042)
#define PMU_INT_CON1_SET             (PMU_BASE + 0x0044)
#define PMU_INT_CON1_CLR             (PMU_BASE + 0x0046)
#define PMU_INT_CON2                 (PMU_BASE + 0x0048)
#define PMU_INT_CON2_SET             (PMU_BASE + 0x004A)
#define PMU_INT_CON2_CLR             (PMU_BASE + 0x004C)
#define PMU_INT_CON3                 (PMU_BASE + 0x004E)
#define PMU_INT_CON3_SET             (PMU_BASE + 0x0050)
#define PMU_INT_CON3_CLR             (PMU_BASE + 0x0052)
#define PMU_INT_MASK_CON0            (PMU_BASE + 0x0054)
#define PMU_INT_MASK_CON0_SET        (PMU_BASE + 0x0056)
#define PMU_INT_MASK_CON0_CLR        (PMU_BASE + 0x0058)
#define PMU_INT_MASK_CON1            (PMU_BASE + 0x005A)
#define PMU_INT_MASK_CON1_SET        (PMU_BASE + 0x005C)
#define PMU_INT_MASK_CON1_CLR        (PMU_BASE + 0x005E)
#define PMU_INT_MASK_CON2            (PMU_BASE + 0x0060)
#define PMU_INT_MASK_CON2_SET        (PMU_BASE + 0x0062)
#define PMU_INT_MASK_CON2_CLR        (PMU_BASE + 0x0064)
#define PMU_INT_MASK_CON3            (PMU_BASE + 0x0066)
#define PMU_INT_MASK_CON3_SET        (PMU_BASE + 0x0068)
#define PMU_INT_MASK_CON3_CLR        (PMU_BASE + 0x006A)
#define PMU_INT_STATUS0              (PMU_BASE + 0x006C)
#define PMU_INT_STATUS1              (PMU_BASE + 0x006E)
#define PMU_INT_STATUS2              (PMU_BASE + 0x0070)
#define PMU_INT_STATUS3              (PMU_BASE + 0x0072)
#define PMU_INT_RAW_STATUS0          (PMU_BASE + 0x0074)
#define PMU_INT_RAW_STATUS1          (PMU_BASE + 0x0076)
#define PMU_INT_RAW_STATUS2          (PMU_BASE + 0x0078)
#define PMU_INT_RAW_STATUS3          (PMU_BASE + 0x007A)
#define PMU_INT_MISC_CON             (PMU_BASE + 0x007C)
#define PMU_PONSTS                   (PMU_BASE + 0x007E)
#define PMU_PSTSCTL1                 (PMU_BASE + 0x0080)
#define PMU_PSTSCTL2                 (PMU_BASE + 0x0082)
#define PMU_PSSTS2                   (PMU_BASE + 0x0084)
#define PMU_OC_SDN_STS0              (PMU_BASE + 0x0086)
#define PMU_PG_DEB_STS0              (PMU_BASE + 0x0088)
#define PMU_PG_SDN_STS0              (PMU_BASE + 0x008A)
#define PMU_THMSTS                   (PMU_BASE + 0x008C)
#define PMU_LBAT_INT_EN              (PMU_BASE + 0x008E)
#define PMU_TPO_CON0                 (PMU_BASE + 0x0090)
#define PMU_TPO_CON1                 (PMU_BASE + 0x0092)
#define PMU_TPO_CON2                 (PMU_BASE + 0x0094)
#define PMU_TPO_CON3                 (PMU_BASE + 0x0096)
#define PMU_TPO_CON4                 (PMU_BASE + 0x0098)
#define PMU_TPO_SUSPEND_MODE         (PMU_BASE + 0x009A)
#define PMU_TPO_CON5                 (PMU_BASE + 0x009C)
#define PMU_TPO_CON6                 (PMU_BASE + 0x009E)
#define PMU_PWRHOLD                  (PMU_BASE + 0x00A0)
#define PMU_STRUP_CON0               (PMU_BASE + 0x00A2)
#define PMU_STRUP_CON6               (PMU_BASE + 0x00A4)
#define PMU_STRUP_Interface_ANA_CON0 (PMU_BASE + 0x00A6)
#define PMU_STRUP_Interface_ANA_CON1 (PMU_BASE + 0x00A8)
#define PMU_STRUP_Interface_ELR_0    (PMU_BASE + 0x00AA)
#define PMU_VREF_Interface_ANA_CON0  (PMU_BASE + 0x00AC)
#define PMU_VREF_Interface_ANA_CON1  (PMU_BASE + 0x00AE)
#define PMU_VREF_Interface_ANA_CON2  (PMU_BASE + 0x00B0)
#define PMU_VREF_Interface_ANA_CON3  (PMU_BASE + 0x00B2)
#define PMU_VREF_Interface_ELR_0     (PMU_BASE + 0x00B4)
#define PMU_VREF_Interface_ELR_1     (PMU_BASE + 0x00B6)
#define PMU_VREF_Interface_ELR_2     (PMU_BASE + 0x00B8)
#define PMU_VREF_Interface_ELR_3     (PMU_BASE + 0x00BA)
#define PMU_OSC32_ANA_CON0           (PMU_BASE + 0x00BC)
#define PMU_OSC32_ELR_0              (PMU_BASE + 0x00BE)
#define PMU_CHR_AO_CON0              (PMU_BASE + 0x0100)
#define PMU_CHR_AO_DBG0              (PMU_BASE + 0x0102)
#define PMU_CHR_AO_CON1              (PMU_BASE + 0x0104)
#define PMU_CHR_AO_CON2              (PMU_BASE + 0x0106)
#define PMU_CHR_AO_ICL_0             (PMU_BASE + 0x0108)
#define PMU_CHR_AO_ICC_0             (PMU_BASE + 0x010A)
#define PMU_CHR_AO_SYSLDO            (PMU_BASE + 0x010C)
#define PMU_CHR_AO_ITERM             (PMU_BASE + 0x010E)
#define PMU_CHR_AO_VCV               (PMU_BASE + 0x0110)
#define PMU_CHR_AO_FORCE_EN          (PMU_BASE + 0x0112)
#define PMU_CHR_AO_RSV               (PMU_BASE + 0x0114)
#define PMU_LCHR_DIG_CON14           (PMU_BASE + 0x0116)
#define PMU_LCHR_DIG_CON15           (PMU_BASE + 0x0118)
#define PMU_LCHR_DIG_CON16           (PMU_BASE + 0x011A)
#define PMU_LCHR_DIG_CON17           (PMU_BASE + 0x011C)
#define PMU_CORE_CORE_ANA_AO_CON0    (PMU_BASE + 0x011E)
#define PMU_CORE_CORE_AO_ELR_0       (PMU_BASE + 0x0120)
#define PMU_CORE_CORE_AO_ELR_1       (PMU_BASE + 0x0122)
#define PMU_CORE_CORE_AO_ELR_2       (PMU_BASE + 0x0124)
#define PMU_CORE_CORE_AO_ELR_3       (PMU_BASE + 0x0126)
#define PMU_CORE_CORE_AO_CTRL_0      (PMU_BASE + 0x0128)
#define PMU_CORE_CORE_AO_CTRL_1      (PMU_BASE + 0x012A)
#define PMU_RTC_MISC_DBG0            (PMU_BASE + 0x012C)
#define PMU_TOP_CLK_TRIM_L           (PMU_BASE + 0x012E)
#define PMU_TOP_CLK_TRIM_H           (PMU_BASE + 0x0130)
#define PMU_LDO_VA18_CON0            (PMU_BASE + 0x0200)
#define PMU_LDO_VA18_OP_MODE         (PMU_BASE + 0x0202)
#define PMU_LDO_VA18_EN_CFG          (PMU_BASE + 0x0204)
#define PMU_LDO_VA18_LP_CFG          (PMU_BASE + 0x0206)
#define PMU_LDO_VA18_CON1            (PMU_BASE + 0x0208)
#define PMU_LDO_VA18_CON2            (PMU_BASE + 0x020A)
#define PMU_LDO_VLDO33_CON0          (PMU_BASE + 0x020C)
#define PMU_LDO_VLDO33_OP_MODE       (PMU_BASE + 0x020E)
#define PMU_LDO_VLDO33_EN_CFG        (PMU_BASE + 0x0210)
#define PMU_LDO_VLDO33_LP_CFG        (PMU_BASE + 0x0212)
#define PMU_LDO_VLDO33_CON1          (PMU_BASE + 0x0214)
#define PMU_LDO_VLDO33_CON2          (PMU_BASE + 0x0216)
#define PMU_LDO_OCFB0                (PMU_BASE + 0x0218)
#define PMU_LDO_LP_PROTECTION        (PMU_BASE + 0x021A)
#define PMU_LDO_AO_RSV_CON0          (PMU_BASE + 0x021C)
#define PMU_LDO_AO_RSV_CON1          (PMU_BASE + 0x021E)
#define PMU_LDO_AO_EFUSERSVED0       (PMU_BASE + 0x0220)
#define PMU_LDO_AO_EFUSERSVED1       (PMU_BASE + 0x0222)
#define PMU_LDO_TOP_CLK_DCM_CON0     (PMU_BASE + 0x0224)
#define PMU_LDO_TOP_CLK_VA18_CON0    (PMU_BASE + 0x0226)
#define PMU_LDO_TOP_CLK_VA18_CON1    (PMU_BASE + 0x0228)
#define PMU_LDO_TOP_CLK_VLDO33_CON0  (PMU_BASE + 0x022A)
#define PMU_LDO_TOP_CLK_VLDO33_CON1  (PMU_BASE + 0x022C)
#define PMU_LDO_TOP_CLK_VRF11_CON0   (PMU_BASE + 0x022E)
#define PMU_LDO_TOP_CLK_VRF11_CON1   (PMU_BASE + 0x0230)
#define PMU_LDO_TOP_CLK_VSRAM_CON0   (PMU_BASE + 0x0232)
#define PMU_LDO_TOP_CLK_VSRAM_CON1   (PMU_BASE + 0x0234)
#define PMU_LDO_ANA_STATUS           (PMU_BASE + 0x0236)
#define PMU_LDO_VRF11_CON0           (PMU_BASE + 0x0238)
#define PMU_LDO_VRF11_OP_MODE        (PMU_BASE + 0x023A)
#define PMU_LDO_VRF11_EN_CFG         (PMU_BASE + 0x023C)
#define PMU_LDO_VRF11_LP_CFG         (PMU_BASE + 0x023E)
#define PMU_LDO_VRF11_CON1           (PMU_BASE + 0x0240)
#define PMU_LDO_VRF11_CON2           (PMU_BASE + 0x0242)
#define PMU_LDO_VSRAM_CON0           (PMU_BASE + 0x0244)
#define PMU_LDO_VSRAM_OP_MODE        (PMU_BASE + 0x0246)
#define PMU_LDO_VSRAM_EN_CFG         (PMU_BASE + 0x0248)
#define PMU_LDO_VSRAM_LP_CFG         (PMU_BASE + 0x024A)
#define PMU_LDO_VSRAM_CON1           (PMU_BASE + 0x024C)
#define PMU_LDO_VSRAM_CON2           (PMU_BASE + 0x024E)
#define PMU_LDO_SRAM_CON3            (PMU_BASE + 0x0250)
#define PMU_VCORE_SRAM_SW_CON0       (PMU_BASE + 0x0252)
#define PMU_VCORE_SRAM_SW_EN_CFG     (PMU_BASE + 0x0254)
#define PMU_SRAM_LDO_NDIS_CTRL       (PMU_BASE + 0x0256)
#define PMU_VDIG18_ANA_CON1          (PMU_BASE + 0x0258)
#define PMU_LDOTOP_ANA_CON0          (PMU_BASE + 0x025A)
#define PMU_LDOTOP_ANA_CON1          (PMU_BASE + 0x025C)
#define PMU_LDOTOP_ANA_CON2          (PMU_BASE + 0x025E)
#define PMU_VDIG18_ELR_0             (PMU_BASE + 0x0260)
#define PMU_VDIG18_ELR_1             (PMU_BASE + 0x0262)
#define PMU_LDOTOP_ELR_0             (PMU_BASE + 0x0264)
#define PMU_LDOTOP_ELR_1             (PMU_BASE + 0x0266)
#define PMU_LDOTOP_ANA_CON3          (PMU_BASE + 0x0268)
#define PMU_LDOTOP_ANA_CON4          (PMU_BASE + 0x026A)
#define PMU_LDOTOP_ANA_CON5          (PMU_BASE + 0x026C)
#define PMU_LDOTOP_ANA_CON6          (PMU_BASE + 0x026E)
#define PMU_LDOTOP_ELR_2             (PMU_BASE + 0x0270)
#define PMU_LDOTOP_ELR_3             (PMU_BASE + 0x0272)
#define PMU_BUCK_VCORE_CON0          (PMU_BASE + 0x0300)
#define PMU_BUCK_VCORE_CON1          (PMU_BASE + 0x0302)
#define PMU_BUCK_VCORE_CFG0          (PMU_BASE + 0x0304)
#define PMU_BUCK_VCORE_CFG1          (PMU_BASE + 0x0306)
#define PMU_BUCK_VCORE_OP_MODE       (PMU_BASE + 0x0308)
#define PMU_BUCK_VCORE_EN_CFG        (PMU_BASE + 0x030A)
#define PMU_BUCK_VCORE_LP_CFG        (PMU_BASE + 0x030C)
#define PMU_BUCK_VCORE_OC_CFG        (PMU_BASE + 0x030E)
#define PMU_BUCK_VCORE_DBG0          (PMU_BASE + 0x0310)
#define PMU_BUCK_VCORE_DBG1          (PMU_BASE + 0x0312)
#define PMU_BUCK_VCORE_DBG2          (PMU_BASE + 0x0314)
#define PMU_BUCK_VCORE_ELR0          (PMU_BASE + 0x0316)
#define PMU_BUCK_VIO18_CON0          (PMU_BASE + 0x0318)
#define PMU_BUCK_VIO18_CON1          (PMU_BASE + 0x031A)
#define PMU_BUCK_VIO18_CFG0          (PMU_BASE + 0x031C)
#define PMU_BUCK_VIO18_CFG1          (PMU_BASE + 0x031E)
#define PMU_BUCK_VIO18_OP_MODE       (PMU_BASE + 0x0320)
#define PMU_BUCK_VIO18_EN_CFG        (PMU_BASE + 0x0322)
#define PMU_BUCK_VIO18_LP_CFG        (PMU_BASE + 0x0324)
#define PMU_BUCK_VIO18_OC_CFG        (PMU_BASE + 0x0326)
#define PMU_BUCK_VIO18_DBG0          (PMU_BASE + 0x0328)
#define PMU_BUCK_VIO18_DBG1          (PMU_BASE + 0x032A)
#define PMU_BUCK_VIO18_DBG2          (PMU_BASE + 0x032C)
#define PMU_BUCK_VIO18_ELR0          (PMU_BASE + 0x032E)
#define PMU_BUCK_VRF_CON0            (PMU_BASE + 0x0330)
#define PMU_BUCK_VRF_CON1            (PMU_BASE + 0x0332)
#define PMU_BUCK_VRF_CFG0            (PMU_BASE + 0x0334)
#define PMU_BUCK_VRF_CFG1            (PMU_BASE + 0x0336)
#define PMU_BUCK_VRF_OP_MODE         (PMU_BASE + 0x0338)
#define PMU_BUCK_VRF_EN_CFG          (PMU_BASE + 0x033A)
#define PMU_BUCK_VRF_LP_CFG          (PMU_BASE + 0x033C)
#define PMU_BUCK_VRF_OC_CFG          (PMU_BASE + 0x033E)
#define PMU_BUCK_VRF_DBG0            (PMU_BASE + 0x0340)
#define PMU_BUCK_VRF_DBG1            (PMU_BASE + 0x0342)
#define PMU_BUCK_VRF_DBG2            (PMU_BASE + 0x0344)
#define PMU_BUCK_VRF_ELR0            (PMU_BASE + 0x0346)
#define PMU_BUCK_VAUD18_CON0         (PMU_BASE + 0x0348)
#define PMU_BUCK_VAUD18_CON1         (PMU_BASE + 0x034A)
#define PMU_BUCK_VAUD18_CFG0         (PMU_BASE + 0x034C)
#define PMU_BUCK_VAUD18_CFG1         (PMU_BASE + 0x034E)
#define PMU_BUCK_VAUD18_OP_MODE      (PMU_BASE + 0x0350)
#define PMU_BUCK_VAUD18_EN_CFG       (PMU_BASE + 0x0352)
#define PMU_BUCK_VAUD18_LP_CFG       (PMU_BASE + 0x0354)
#define PMU_BUCK_VAUD18_OC_CFG       (PMU_BASE + 0x0356)
#define PMU_BUCK_VAUD18_DBG0         (PMU_BASE + 0x0358)
#define PMU_BUCK_VAUD18_DBG1         (PMU_BASE + 0x035A)
#define PMU_BUCK_VAUD18_DBG2         (PMU_BASE + 0x035C)
#define PMU_BUCK_VAUD18_ELR0         (PMU_BASE + 0x035E)
#define PMU_BUCK_TOP_STB_CON         (PMU_BASE + 0x0360)
#define PMU_BUCK_TOP_SLP_CON0        (PMU_BASE + 0x0362)
#define PMU_BUCK_TOP_SLP_CON1        (PMU_BASE + 0x0364)
#define PMU_BUCK_TOP_SLP_CON2        (PMU_BASE + 0x0366)
#define PMU_BUCK_TOP_OC_CON0         (PMU_BASE + 0x0368)
#define PMU_BUCK_TOP_WDTDBG0         (PMU_BASE + 0x036A)
#define PMU_BUCK_TOP_WDTDBG1         (PMU_BASE + 0x036C)
#define PMU_BUCK_TOP_AO_ELR0         (PMU_BASE + 0x036E)
#define PMU_BUCK_TOP_AO_ELR1         (PMU_BASE + 0x0370)
#define PMU_BUCK_AO_RSV_CON0         (PMU_BASE + 0x0372)
#define PMU_BUCK_AO_RSV_CON1         (PMU_BASE + 0x0374)
#define PMU_BUCK_AO_EFUSERSVED0      (PMU_BASE + 0x0376)
#define PMU_BUCK_AO_EFUSERSVED1      (PMU_BASE + 0x0378)
#define PMU_BUCK_TOP_CLK_HWEN_CON0   (PMU_BASE + 0x037A)
#define PMU_BUCK_VCORE_IPEAK_CON0    (PMU_BASE + 0x037C)
#define PMU_BUCK_VCORE_IPEAK_CON1    (PMU_BASE + 0x037E)
#define PMU_BUCK_VIO18_IPEAK_CON0    (PMU_BASE + 0x0380)
#define PMU_BUCK_VIO18_IPEAK_CON1    (PMU_BASE + 0x0382)
#define PMU_BUCK_VRF_IPEAK_CON0      (PMU_BASE + 0x0384)
#define PMU_BUCK_VRF_IPEAK_CON1      (PMU_BASE + 0x0386)
#define PMU_BUCK_VAUD18_IPEAK_CON0   (PMU_BASE + 0x0388)
#define PMU_BUCK_VAUD18_IPEAK_CON1   (PMU_BASE + 0x038A)
#define PMU_BUCK_TOP_K_CON0          (PMU_BASE + 0x038C)
#define PMU_BUCK_TOP_K_CON1          (PMU_BASE + 0x038E)
#define PMU_BUCK_TOP_K_CON2          (PMU_BASE + 0x0390)
#define PMU_BUCK_TOP_ELR0            (PMU_BASE + 0x0392)
#define PMU_BUCK_CO_RSV_CON0         (PMU_BASE + 0x0394)
#define PMU_BUCK_CO_RSV_CON1         (PMU_BASE + 0x0396)
#define PMU_BUCK_CO_EFUSERSVED0      (PMU_BASE + 0x0398)
#define PMU_BUCK_CO_EFUSERSVED1      (PMU_BASE + 0x039A)
#define PMU_SMPS_ANA_CON0            (PMU_BASE + 0x0400)
#define PMU_VCORE_ANA_CON0           (PMU_BASE + 0x0402)
#define PMU_VCORE_ANA_CON1           (PMU_BASE + 0x0404)
#define PMU_VCORE_ANA_CON2           (PMU_BASE + 0x0406)
#define PMU_VCORE_ANA_CON3           (PMU_BASE + 0x0408)
#define PMU_VIO18_ANA_CON0           (PMU_BASE + 0x040A)
#define PMU_VIO18_ANA_CON1           (PMU_BASE + 0x040C)
#define PMU_VIO18_ANA_CON2           (PMU_BASE + 0x040E)
#define PMU_VIO18_ANA_CON3           (PMU_BASE + 0x0410)
#define PMU_VRF_ANA_CON0             (PMU_BASE + 0x0412)
#define PMU_VRF_ANA_CON1             (PMU_BASE + 0x0414)
#define PMU_VRF_ANA_CON2             (PMU_BASE + 0x0416)
#define PMU_VRF_ANA_CON3             (PMU_BASE + 0x0418)
#define PMU_VAUD18_ANA_CON0          (PMU_BASE + 0x041A)
#define PMU_VAUD18_ANA_CON1          (PMU_BASE + 0x041C)
#define PMU_VAUD18_ANA_CON2          (PMU_BASE + 0x041E)
#define PMU_VAUD18_ANA_CON3          (PMU_BASE + 0x0420)
#define PMU_SMPS_ELR_0               (PMU_BASE + 0x0422)
#define PMU_SMPS_ELR_1               (PMU_BASE + 0x0424)
#define PMU_VCORE_ELR_0              (PMU_BASE + 0x0426)
#define PMU_VIO18_ELR_0              (PMU_BASE + 0x0428)
#define PMU_VRF_ELR_0                (PMU_BASE + 0x042A)
#define PMU_VAUD18_ELR_0             (PMU_BASE + 0x042C)
#define PMU_AUXADC_AD_ADC0           (PMU_BASE + 0x0500)
#define PMU_AUXADC_AD_ADC2           (PMU_BASE + 0x0502)
#define PMU_AUXADC_AD_ADC3           (PMU_BASE + 0x0504)
#define PMU_AUXADC_AD_ADC4           (PMU_BASE + 0x0506)
#define PMU_AUXADC_AD_ADC13          (PMU_BASE + 0x0508)
#define PMU_AUXADC_AD_ADC14          (PMU_BASE + 0x050A)
#define PMU_AUXADC_AD_ADC15          (PMU_BASE + 0x050C)
#define PMU_AUXADC_AD_ADC20          (PMU_BASE + 0x050E)
#define PMU_AUXADC_AD_ADC22          (PMU_BASE + 0x0510)
#define PMU_AUXADC_AD_ADC25          (PMU_BASE + 0x0512)
#define PMU_AUXADC_AD_ADC33          (PMU_BASE + 0x0514)
#define PMU_AUXADC_AD_ADC35          (PMU_BASE + 0x0516)
#define PMU_AUXADC_AD_ADC37          (PMU_BASE + 0x0518)
#define PMU_AUXADC_AD_STA0           (PMU_BASE + 0x051A)
#define PMU_AUXADC_AD_STA1           (PMU_BASE + 0x051C)
#define PMU_AUXADC_AD_RQST0          (PMU_BASE + 0x051E)
#define PMU_AUXADC_AD_CON0           (PMU_BASE + 0x0520)
#define PMU_AUXADC_AD_CON1           (PMU_BASE + 0x0522)
#define PMU_AUXADC_AD_CON2           (PMU_BASE + 0x0524)
#define PMU_AUXADC_AD_CON3           (PMU_BASE + 0x0526)
#define PMU_AUXADC_AD_CON4           (PMU_BASE + 0x0528)
#define PMU_AUXADC_AD_CON5           (PMU_BASE + 0x052A)
#define PMU_AUXADC_AD_CON6           (PMU_BASE + 0x052C)
#define PMU_AUXADC_AD_CON7           (PMU_BASE + 0x052E)
#define PMU_AUXADC_AD_CON8           (PMU_BASE + 0x0530)
#define PMU_AUXADC_AD_CON9           (PMU_BASE + 0x0532)
#define PMU_AUXADC_AD_CON10          (PMU_BASE + 0x0534)
#define PMU_AUXADC_AD_CON11          (PMU_BASE + 0x0536)
#define PMU_AUXADC_AD_CON12          (PMU_BASE + 0x0538)
#define PMU_AUXADC_AD_CON13          (PMU_BASE + 0x053A)
#define PMU_AUXADC_AD_CON14          (PMU_BASE + 0x053C)
#define PMU_AUXADC_AD_CON15          (PMU_BASE + 0x053E)
#define PMU_AUXADC_AD_CON16          (PMU_BASE + 0x0540)
#define PMU_AUXADC_AD_CON17          (PMU_BASE + 0x0542)
#define PMU_AUXADC_AD_AUTORPT0       (PMU_BASE + 0x0544)
#define PMU_AUXADC_AD_LBAT0          (PMU_BASE + 0x0546)
#define PMU_AUXADC_AD_LBAT1          (PMU_BASE + 0x0548)
#define PMU_AUXADC_AD_LBAT2          (PMU_BASE + 0x054A)
#define PMU_AUXADC_AD_LBAT3          (PMU_BASE + 0x054C)
#define PMU_AUXADC_AD_LBAT4          (PMU_BASE + 0x054E)
#define PMU_AUXADC_AD_LBAT5          (PMU_BASE + 0x0550)
#define PMU_AUXADC_AD_LBAT6          (PMU_BASE + 0x0552)
#define PMU_AUXADC_AD_ACCDET         (PMU_BASE + 0x0554)
#define PMU_AUXADC_AD_THR0           (PMU_BASE + 0x0556)
#define PMU_AUXADC_AD_THR1           (PMU_BASE + 0x0558)
#define PMU_AUXADC_AD_THR2           (PMU_BASE + 0x055A)
#define PMU_AUXADC_AD_THR3           (PMU_BASE + 0x055C)
#define PMU_AUXADC_AD_THR4           (PMU_BASE + 0x055E)
#define PMU_AUXADC_AD_THR5           (PMU_BASE + 0x0560)
#define PMU_AUXADC_AD_THR6           (PMU_BASE + 0x0562)
#define PMU_AUXADC_AD_EFUSE0         (PMU_BASE + 0x0564)
#define PMU_AUXADC_AD_EFUSE1         (PMU_BASE + 0x0566)
#define PMU_AUXADC_AD_EFUSE2         (PMU_BASE + 0x0568)
#define PMU_AUXADC_AD_EFUSE3         (PMU_BASE + 0x056A)
#define PMU_AUXADC_AD_EFUSE4         (PMU_BASE + 0x056C)
#define PMU_AUXADC_AD_EFUSE5         (PMU_BASE + 0x056E)
#define PMU_AUXADC_AD_DBG0           (PMU_BASE + 0x0570)
#define PMU_AUXADC_AD_LBAT2_0        (PMU_BASE + 0x0572)
#define PMU_AUXADC_AD_LBAT2_1        (PMU_BASE + 0x0574)
#define PMU_AUXADC_AD_LBAT2_2        (PMU_BASE + 0x0576)
#define PMU_AUXADC_AD_LBAT2_3        (PMU_BASE + 0x0578)
#define PMU_AUXADC_AD_LBAT2_4        (PMU_BASE + 0x057A)
#define PMU_AUXADC_AD_LBAT2_5        (PMU_BASE + 0x057C)
#define PMU_AUXADC_AD_LBAT2_6        (PMU_BASE + 0x057E)
#define PMU_AUXADC_AD_JEITA_0        (PMU_BASE + 0x0580)
#define PMU_AUXADC_AD_JEITA_1        (PMU_BASE + 0x0582)
#define PMU_AUXADC_AD_JEITA_2        (PMU_BASE + 0x0584)
#define PMU_AUXADC_AD_JEITA_3        (PMU_BASE + 0x0586)
#define PMU_AUXADC_AD_JEITA_4        (PMU_BASE + 0x0588)
#define PMU_AUXADC_AD_JEITA_5        (PMU_BASE + 0x058A)
#define PMU_AUXADC_AD_JEITA_6        (PMU_BASE + 0x058C)
#define PMU_AUXADC_AD_JEITA_7        (PMU_BASE + 0x058E)
#define PMU_AUXADC_AD_JEITA_8        (PMU_BASE + 0x0590)
#define PMU_AUXADC_AD_JEITA_9        (PMU_BASE + 0x0592)
#define PMU_AUXADC_AD_VBAT0          (PMU_BASE + 0x0594)
#define PMU_AUXADC_AD_VBAT1          (PMU_BASE + 0x0596)
#define PMU_AUXADC_AD_VBAT2          (PMU_BASE + 0x0598)
#define PMU_AUXADC_AD_VBAT3          (PMU_BASE + 0x059A)
#define PMU_AUXADC_AD_VBAT4          (PMU_BASE + 0x059C)
#define PMU_AUXADC_AD_VBAT5          (PMU_BASE + 0x059E)
#define PMU_AUXADC_AD_VBAT6          (PMU_BASE + 0x05A0)
#define PMU_AUXADC_AD_VBAT7          (PMU_BASE + 0x05A2)
#define PMU_AUXADC_AD_VBAT8          (PMU_BASE + 0x05A4)
#define PMU_AUXADC_AD_VBAT9          (PMU_BASE + 0x05A6)
#define PMU_AUXADC_AD_CHR_CON        (PMU_BASE + 0x05A8)
#define PMU_AUXADC_AD_ANA            (PMU_BASE + 0x05AA)
#define PMU_PPCCTL0                  (PMU_BASE + 0x0600)
#define PMU_PPCCTL2                  (PMU_BASE + 0x0602)
#define PMU_PPCCFG0                  (PMU_BASE + 0x0604)
#define PMU_PPCTST0                  (PMU_BASE + 0x0606)
#define PMU_PPCRSV0                  (PMU_BASE + 0x0608)
#define PMU_STRUP_CON1               (PMU_BASE + 0x060A)
#define PMU_STRUP_CON4               (PMU_BASE + 0x060C)
#define PMU_STRUP_CON14              (PMU_BASE + 0x060E)
#define PMU_STRUP_CON16              (PMU_BASE + 0x0610)
#define PMU_STRUP_CON18              (PMU_BASE + 0x0612)
#define PMU_CPSPSA0                  (PMU_BASE + 0x0614)
#define PMU_CPSPSA1                  (PMU_BASE + 0x0616)
#define PMU_CPSPSA2                  (PMU_BASE + 0x0618)
#define PMU_CPSPSA3                  (PMU_BASE + 0x061A)
#define PMU_CPSPSA4                  (PMU_BASE + 0x061C)
#define PMU_CPSPSA5                  (PMU_BASE + 0x061E)
#define PMU_PSEQ_ELR7                (PMU_BASE + 0x0620)
#define PMU_PSEQ_ELR8                (PMU_BASE + 0x0622)
#define PMU_PSEQ_ELR9                (PMU_BASE + 0x0624)
#define PMU_CO_EFUSE_RSV             (PMU_BASE + 0x0626)
#define PMU_CPSWKEY                  (PMU_BASE + 0x0628)
#define PMU_CPSCFG0                  (PMU_BASE + 0x062A)
#define PMU_CPSDSA0                  (PMU_BASE + 0x062C)
#define PMU_CPSDSA1                  (PMU_BASE + 0x062E)
#define PMU_CPSDSA2                  (PMU_BASE + 0x0630)
#define PMU_CPSDSA3                  (PMU_BASE + 0x0632)
#define PMU_CPSDSA4                  (PMU_BASE + 0x0634)
#define PMU_CPSDSA5                  (PMU_BASE + 0x0636)
#define PMU_ISINK0_CON0              (PMU_BASE + 0x0638)
#define PMU_ISINK0_CON1              (PMU_BASE + 0x063A)
#define PMU_ISINK0_CON2              (PMU_BASE + 0x063C)
#define PMU_ISINK0_CON3              (PMU_BASE + 0x063E)
#define PMU_ISINK1_CON0              (PMU_BASE + 0x0640)
#define PMU_ISINK1_CON1              (PMU_BASE + 0x0642)
#define PMU_ISINK1_CON2              (PMU_BASE + 0x0644)
#define PMU_ISINK1_CON3              (PMU_BASE + 0x0646)
#define PMU_DRIVER_ANA_CON0          (PMU_BASE + 0x0648)
#define PMU_DRIVER_ELR_0             (PMU_BASE + 0x064A)
#define PMU_ISINK0_REPT_CYCLE        (PMU_BASE + 0x064C)
#define PMU_ISINK1_REPT_CYCLE        (PMU_BASE + 0x064E)
#define PMU_ISINK_EN_CTRL            (PMU_BASE + 0x0650)
#define PMU_ISINK_MODE_CTRL          (PMU_BASE + 0x0652)
#define PMU_ISINK_ANA1               (PMU_BASE + 0x0654)
#define PMU_OTP_CON0                 (PMU_BASE + 0x0656)
#define PMU_OTP_CON1                 (PMU_BASE + 0x0658)
#define PMU_OTP_CON2                 (PMU_BASE + 0x065A)
#define PMU_OTP_CON3                 (PMU_BASE + 0x065C)
#define PMU_OTP_CON4                 (PMU_BASE + 0x065E)
#define PMU_OTP_CON5                 (PMU_BASE + 0x0660)
#define PMU_OTP_CON6                 (PMU_BASE + 0x0662)
#define PMU_OTP_CON7                 (PMU_BASE + 0x0664)
#define PMU_OTP_CON8                 (PMU_BASE + 0x0666)
#define PMU_OTP_CON9                 (PMU_BASE + 0x0668)
#define PMU_OTP_CON10                (PMU_BASE + 0x066A)
#define PMU_OTP_CON11                (PMU_BASE + 0x066C)
#define PMU_OTP_CON12                (PMU_BASE + 0x066E)
#define PMU_OTP_CON13                (PMU_BASE + 0x0670)
#define PMU_OTP_CON14                (PMU_BASE + 0x0672)
#define PMU_CORE_CORE_ANA_CON0       (PMU_BASE + 0x0700)
#define PMU_CORE_CORE_ANA_CON1       (PMU_BASE + 0x0702)
#define PMU_CORE_CORE_ANA_CON2       (PMU_BASE + 0x0704)
#define PMU_CORE_CORE_ANA_CON3       (PMU_BASE + 0x0706)
#define PMU_CORE_CORE_ANA_CON4       (PMU_BASE + 0x0708)
#define PMU_CORE_CORE_ANA_CON5       (PMU_BASE + 0x070A)
#define PMU_CORE_CORE_ANA_CON6       (PMU_BASE + 0x070C)
#define PMU_CORE_CORE_ANA_CON7       (PMU_BASE + 0x070E)
#define PMU_CORE_CORE_ANA_CON8       (PMU_BASE + 0x0710)
#define PMU_CORE_CORE_ANA_CON9       (PMU_BASE + 0x0712)
#define PMU_CORE_CORE_ANA_CON10      (PMU_BASE + 0x0714)
#define PMU_CORE_CORE_ANA_CON11      (PMU_BASE + 0x0716)
#define PMU_CORE_CORE_ANA_CON12      (PMU_BASE + 0x0718)
#define PMU_CORE_CORE_ANA_CON13      (PMU_BASE + 0x071A)
#define PMU_CORE_CORE_ANA_CON14      (PMU_BASE + 0x071C)
#define PMU_CORE_CORE_ELR_0          (PMU_BASE + 0x071E)
#define PMU_CORE_CORE_ELR_1          (PMU_BASE + 0x0720)
#define PMU_CORE_CORE_ELR_2          (PMU_BASE + 0x0722)
#define PMU_CORE_CORE_ELR_3          (PMU_BASE + 0x0724)
#define PMU_CORE_CORE_ELR_4          (PMU_BASE + 0x0726)
#define PMU_CORE_CORE_ELR_5          (PMU_BASE + 0x0728)
#define PMU_CORE_CORE_ELR_6          (PMU_BASE + 0x072A)
#define PMU_LCHR_DIG_CON0            (PMU_BASE + 0x072C)
#define PMU_LCHR_DIG_CON1            (PMU_BASE + 0x072E)
#define PMU_LCHR_DIG_CON2            (PMU_BASE + 0x0730)
#define PMU_LCHR_DIG_CON3            (PMU_BASE + 0x0732)
#define PMU_LCHR_DIG_CON4            (PMU_BASE + 0x0734)
#define PMU_LCHR_DIG_CON5            (PMU_BASE + 0x0736)
#define PMU_LCHR_DIG_CON6            (PMU_BASE + 0x0738)
#define PMU_LCHR_DIG_CON7            (PMU_BASE + 0x073A)
#define PMU_LCHR_DIG_CON8            (PMU_BASE + 0x073C)
#define PMU_LCHR_DIG_CON9            (PMU_BASE + 0x073E)
#define PMU_LCHR_DIG_CON10           (PMU_BASE + 0x0740)
#define PMU_LCHR_DIG_CON11           (PMU_BASE + 0x0742)
#define PMU_LCHR_DIG_CON12           (PMU_BASE + 0x0744)
#define PMU_LCHR_DIG_CON13           (PMU_BASE + 0x0746)
#define PMU_LCHR_DIG_DEBUG0          (PMU_BASE + 0x0748)
#define PMU_LCHR_DIG_DEBUG1          (PMU_BASE + 0x074A)
#define PMU_LCHR_DIG_DEBUG2          (PMU_BASE + 0x074C)
#define PMU_LCHR_DIG_DEBUG3          (PMU_BASE + 0x074E)
#define PMU_LCHR_DIG_DEBUG4          (PMU_BASE + 0x0750)
#define PMU_LCHR_DIG_DEBUG5          (PMU_BASE + 0x0752)
#define PMU_LCHR_DIG_DEBUG6          (PMU_BASE + 0x0754)
#define PMU_LCHR_DIG_DEBUG7          (PMU_BASE + 0x0756)
#define PMU_LCHR_DIG_DEBUG8          (PMU_BASE + 0x0758)
#define PMU_LCHR_DIG_EVENT0          (PMU_BASE + 0x075A)
#define PMU_LCHR_DIG_EVENT1          (PMU_BASE + 0x075C)
#define PMU_LCHR_DIG_RSV             (PMU_BASE + 0x075E)
#define PMU_TOPTST0                  (PMU_BASE + 0x0760)
#define PMU_TOPTST3                  (PMU_BASE + 0x0762)
#define PMU_PPCTST1                  (PMU_BASE + 0x0764)
#define PMU_PPCTST2                  (PMU_BASE + 0x0766)
#define PMU_INTRTST0                 (PMU_BASE + 0x0768)
#define PMU_INTRTST1                 (PMU_BASE + 0x076A)
// mask is HEX;  shift is Integer
#define PMU_HWCID_MASK                                0xFFFF
#define PMU_HWCID_SHIFT                               0
#define PMU_SWCID_MASK                                0xFFFF
#define PMU_SWCID_SHIFT                               0
#define PMU_RG_VDIG18_RDSEL_MASK                      0x1
#define PMU_RG_VDIG18_RDSEL_SHIFT                     0
#define PMU_RG_VDIG18_TDSEL_MASK                      0x1
#define PMU_RG_VDIG18_TDSEL_SHIFT                     1
#define PMU_RG_VIO18_RDSEL_MASK                       0x1
#define PMU_RG_VIO18_RDSEL_SHIFT                      2
#define PMU_RG_VIO18_TDSEL_MASK                       0x1
#define PMU_RG_VIO18_TDSEL_SHIFT                      3
#define PMU_RG_I2C_DAT_OC_MASK                        0x7
#define PMU_RG_I2C_DAT_OC_SHIFT                       0
#define PMU_RG_TESTMODE_OC_MASK                       0x7
#define PMU_RG_TESTMODE_OC_SHIFT                      4
#define PMU_RG_PMU_INT_OC_MASK                       0x7
#define PMU_RG_PMU_INT_OC_SHIFT                      8
#define PMU_RG_SRCLKEN_OC_MASK                        0x7
#define PMU_RG_SRCLKEN_OC_SHIFT                       12
#define PMU_RG_RTC_ALARM_OC_MASK                      0x7
#define PMU_RG_RTC_ALARM_OC_SHIFT                     4
#define PMU_RG_SYSRSTB_OC_MASK                        0x7
#define PMU_RG_SYSRSTB_OC_SHIFT                       8
#define PMU_RG_VAUD18DRV_VOSEL_OC_MASK                0x7
#define PMU_RG_VAUD18DRV_VOSEL_OC_SHIFT               12
#define PMU_RG_I2C_DAT_IC_MASK                        0x7
#define PMU_RG_I2C_DAT_IC_SHIFT                       0
#define PMU_RG_I2C_CLK_IC_MASK                        0x7
#define PMU_RG_I2C_CLK_IC_SHIFT                       4
#define PMU_RG_CAP_LPSD_IC_MASK                       0x7
#define PMU_RG_CAP_LPSD_IC_SHIFT                      8
#define PMU_RG_SRCLKEN_IC_MASK                        0x7
#define PMU_RG_SRCLKEN_IC_SHIFT                       12
#define PMU_RG_WDTRSTB_IN_IC_MASK                     0x7
#define PMU_RG_WDTRSTB_IN_IC_SHIFT                    0
#define PMU_RG_RTC_ALARM_IC_MASK                      0x7
#define PMU_RG_RTC_ALARM_IC_SHIFT                     4
#define PMU_RG_SYSRSTB_IC_MASK                        0x7
#define PMU_RG_SYSRSTB_IC_SHIFT                       8
#define PMU_RG_VAUD18DRV_VOSEL_IC_MASK                0x7
#define PMU_RG_VAUD18DRV_VOSEL_IC_SHIFT               12
#define PMU_RG_SYSRSTB_FILTER_MASK                    0x1
#define PMU_RG_SYSRSTB_FILTER_SHIFT                   0
#define PMU_RG_VAUD18DRV_VOSEL_FILTER_MASK            0x1
#define PMU_RG_VAUD18DRV_VOSEL_FILTER_SHIFT           1
#define PMU_RG_SRCLKEN_FILTER_MASK                    0x1
#define PMU_RG_SRCLKEN_FILTER_SHIFT                   2
#define PMU_RG_WDTRSTB_IN_FILTER_MASK                 0x1
#define PMU_RG_WDTRSTB_IN_FILTER_SHIFT                3
#define PMU_RG_PMU_INT_FILTER_MASK                   0x1
#define PMU_RG_PMU_INT_FILTER_SHIFT                  4
#define PMU_RG_I2C_CLK_FILTER_MASK                    0x1
#define PMU_RG_I2C_CLK_FILTER_SHIFT                   5
#define PMU_RG_I2C_DAT_FILTER_MASK                    0x1
#define PMU_RG_I2C_DAT_FILTER_SHIFT                   6
#define PMU_RG_RTC_ALARM_FILTER_MASK                  0x1
#define PMU_RG_RTC_ALARM_FILTER_SHIFT                 7
#define PMU_RG_CAP_LPSD_FILTER_MASK                   0x1
#define PMU_RG_CAP_LPSD_FILTER_SHIFT                  8
#define PMU_RG_SYSRSTB_RCSEL_MASK                     0x1
#define PMU_RG_SYSRSTB_RCSEL_SHIFT                    0
#define PMU_RG_VAUD18DRV_VOSEL_RCSEL_MASK             0x1
#define PMU_RG_VAUD18DRV_VOSEL_RCSEL_SHIFT            1
#define PMU_RG_SRCLKEN_RCSEL_MASK                     0x1
#define PMU_RG_SRCLKEN_RCSEL_SHIFT                    2
#define PMU_RG_WDTRSTB_IN_RCSEL_MASK                  0x1
#define PMU_RG_WDTRSTB_IN_RCSEL_SHIFT                 3
#define PMU_RG_PMU_INT_RCSEL_MASK                    0x1
#define PMU_RG_PMU_INT_RCSEL_SHIFT                   4
#define PMU_RG_I2C_CLK_RCSEL_MASK                     0x1
#define PMU_RG_I2C_CLK_RCSEL_SHIFT                    5
#define PMU_RG_I2C_DAT_RCSEL_MASK                     0x1
#define PMU_RG_I2C_DAT_RCSEL_SHIFT                    6
#define PMU_RG_RTC_ALARM_RCSEL_MASK                   0x1
#define PMU_RG_RTC_ALARM_RCSEL_SHIFT                  7
#define PMU_RG_CAP_LPSD_RCSEL_MASK                    0x1
#define PMU_RG_CAP_LPSD_RCSEL_SHIFT                   8
#define PMU_RG_VAUD18DRV_VOSEL_MASK                   0x1
#define PMU_RG_VAUD18DRV_VOSEL_SHIFT                  0
#define PMU_TEST_OUT_MASK                             0xF
#define PMU_TEST_OUT_SHIFT                            0
#define PMU_RG_MON_FLAG_SEL_MASK                      0xFF
#define PMU_RG_MON_FLAG_SEL_SHIFT                     0
#define PMU_RG_MON_GRP_SEL_MASK                       0x1F
#define PMU_RG_MON_GRP_SEL_SHIFT                      8
#define PMU_RG_DBGOUT_EN_0_MASK                       0x1
#define PMU_RG_DBGOUT_EN_0_SHIFT                      0
#define PMU_RG_DBGOUT_EN_1_MASK                       0x1
#define PMU_RG_DBGOUT_EN_1_SHIFT                      1
#define PMU_RG_DBGOUT_EN_2_MASK                       0x1
#define PMU_RG_DBGOUT_EN_2_SHIFT                      2
#define PMU_RG_DBGOUT_EN_3_MASK                       0x1
#define PMU_RG_DBGOUT_EN_3_SHIFT                      3
#define PMU_RG_DBGOUT_SEL_0_MASK                      0x3
#define PMU_RG_DBGOUT_SEL_0_SHIFT                     0
#define PMU_RG_DBGOUT_SEL_1_MASK                      0x3
#define PMU_RG_DBGOUT_SEL_1_SHIFT                     2
#define PMU_RG_DBGOUT_SEL_2_MASK                      0x3
#define PMU_RG_DBGOUT_SEL_2_SHIFT                     4
#define PMU_RG_DBGOUT_SEL_3_MASK                      0x3
#define PMU_RG_DBGOUT_SEL_3_SHIFT                     6
#define PMU_RG_SMPS_CK_TSTSEL_MASK                    0x1
#define PMU_RG_SMPS_CK_TSTSEL_SHIFT                   0
#define PMU_RG_F32K_CK_TSTSEL_MASK                    0x1
#define PMU_RG_F32K_CK_TSTSEL_SHIFT                   1
#define PMU_RG_AUXADC_CK_TSTSEL_MASK                  0x1
#define PMU_RG_AUXADC_CK_TSTSEL_SHIFT                 2
#define PMU_RG_EFUSE_CK_TSTSEL_MASK                   0x1
#define PMU_RG_EFUSE_CK_TSTSEL_SHIFT                  3
#define PMU_RG_BUCK_MX_F32K_CK_TSTSEL_MASK            0x1
#define PMU_RG_BUCK_MX_F32K_CK_TSTSEL_SHIFT           4
#define PMU_RG_BGR_TEST_CK_TSTSEL_MASK                0x1
#define PMU_RG_BGR_TEST_CK_TSTSEL_SHIFT               5
#define PMU_RG_SMPS_CK_TST_DIS_MASK                   0x1
#define PMU_RG_SMPS_CK_TST_DIS_SHIFT                  8
#define PMU_RG_F32K_CK_TST_DIS_MASK                   0x1
#define PMU_RG_F32K_CK_TST_DIS_SHIFT                  9
#define PMU_RG_EFUSE_CK_PDN_HWEN_MASK                 0x1
#define PMU_RG_EFUSE_CK_PDN_HWEN_SHIFT                0
#define PMU_RG_BUCK_F2M_CK_PDN_HWEN_MASK              0x1
#define PMU_RG_BUCK_F2M_CK_PDN_HWEN_SHIFT             1
#define PMU_RG_AUXADC_RNG_CK_PDN_HWEN_MASK            0x1
#define PMU_RG_AUXADC_RNG_CK_PDN_HWEN_SHIFT           2
#define PMU_RG_AUXADC_CK_PDN_HWEN_MASK                0x1
#define PMU_RG_AUXADC_CK_PDN_HWEN_SHIFT               3
#define PMU_RG_AUXADC_CK_CKSEL_MASK                   0x3
#define PMU_RG_AUXADC_CK_CKSEL_SHIFT                  8
#define PMU_RG_LDO_F100K_CK_PDN_HWEN_MASK             0x1
#define PMU_RG_LDO_F100K_CK_PDN_HWEN_SHIFT            10
#define PMU_RG_LDO_STB_CK_PDN_HWEN_MASK               0x1
#define PMU_RG_LDO_STB_CK_PDN_HWEN_SHIFT              11
#define PMU_RG_CHR_IND_CK_PDN_HWEN_MASK               0x1
#define PMU_RG_CHR_IND_CK_PDN_HWEN_SHIFT              12
#define PMU_RG_CHR_F6M_CK_PDN_HWEN_MASK               0x1
#define PMU_RG_CHR_F6M_CK_PDN_HWEN_SHIFT              13
#define PMU_RG_CHR_F1M_CK_PDN_HWEN_MASK               0x1
#define PMU_RG_CHR_F1M_CK_PDN_HWEN_SHIFT              14
#define PMU_RG_PSEQ_F32K_SYS_CK_PDN_MASK              0x1
#define PMU_RG_PSEQ_F32K_SYS_CK_PDN_SHIFT             0
#define PMU_RG_AUXADC_AO_CK_PDN_MASK                  0x1
#define PMU_RG_AUXADC_AO_CK_PDN_SHIFT                 1
#define PMU_RG_AUXADC_CK_PDN_MASK                     0x1
#define PMU_RG_AUXADC_CK_PDN_SHIFT                    2
#define PMU_RG_AUXADC_RNG_CK_PDN_MASK                 0x1
#define PMU_RG_AUXADC_RNG_CK_PDN_SHIFT                3
#define PMU_RG_AUXADC_F32K_CK_PDN_MASK                0x1
#define PMU_RG_AUXADC_F32K_CK_PDN_SHIFT               4
#define PMU_RG_INTCTL_F32K_CK_PDN_MASK                0x1
#define PMU_RG_INTCTL_F32K_CK_PDN_SHIFT               5
#define PMU_RG_ANA_F32K_CK_PDN_MASK                   0x1
#define PMU_RG_ANA_F32K_CK_PDN_SHIFT                  6
#define PMU_RG_BGR_TEST_CK_PDN_MASK                   0x1
#define PMU_RG_BGR_TEST_CK_PDN_SHIFT                  7
#define PMU_RG_EFUSE_CK_PDN_MASK                      0x1
#define PMU_RG_EFUSE_CK_PDN_SHIFT                     8
#define PMU_RG_CLK_TRIM_F32K_CK_PDN_MASK              0x1
#define PMU_RG_CLK_TRIM_F32K_CK_PDN_SHIFT             9
#define PMU_RG_DRV_F32K_CK_PDN_MASK                   0x1
#define PMU_RG_DRV_F32K_CK_PDN_SHIFT                  10
#define PMU_RG_DRV_ISINK0_CK_PDN_MASK                 0x1
#define PMU_RG_DRV_ISINK0_CK_PDN_SHIFT                11
#define PMU_RG_DRV_ISINK1_CK_PDN_MASK                 0x1
#define PMU_RG_DRV_ISINK1_CK_PDN_SHIFT                12
#define PMU_RG_BUCK_F2M_CK_PDN_MASK                   0x1
#define PMU_RG_BUCK_F2M_CK_PDN_SHIFT                  0
#define PMU_RG_BUCK_MX_F32K_CK_PDN_MASK               0x1
#define PMU_RG_BUCK_MX_F32K_CK_PDN_SHIFT              1
#define PMU_RG_BUCK_SMPS_CK_PDN_MASK                  0x1
#define PMU_RG_BUCK_SMPS_CK_PDN_SHIFT                 2
#define PMU_RG_LDO_F100K_CK_PDN_MASK                  0x1
#define PMU_RG_LDO_F100K_CK_PDN_SHIFT                 3
#define PMU_RG_LDO_STB_CK_PDN_MASK                    0x1
#define PMU_RG_LDO_STB_CK_PDN_SHIFT                   4
#define PMU_RG_CHR_IND_CK_PDN_MASK                    0x1
#define PMU_RG_CHR_IND_CK_PDN_SHIFT                   11
#define PMU_RG_CHR_F6M_CK_PDN_MASK                    0x1
#define PMU_RG_CHR_F6M_CK_PDN_SHIFT                   12
#define PMU_RG_CHR_F1M_CK_PDN_MASK                    0x1
#define PMU_RG_CHR_F1M_CK_PDN_SHIFT                   13
#define PMU_I2C_CONFIG_MASK                           0x1
#define PMU_I2C_CONFIG_SHIFT                          0
#define PMU_RG_DRIVER_BL_RST_MASK                     0x1
#define PMU_RG_DRIVER_BL_RST_SHIFT                    6
#define PMU_RG_DRIVER_CI_RST_MASK                     0x1
#define PMU_RG_DRIVER_CI_RST_SHIFT                    7
#define PMU_RG_EFUSE_MAN_RST_MASK                     0x1
#define PMU_RG_EFUSE_MAN_RST_SHIFT                    8
#define PMU_RG_AUXADC_RST_MASK                        0x1
#define PMU_RG_AUXADC_RST_SHIFT                       9
#define PMU_RG_AUXADC_REG_RST_MASK                    0x1
#define PMU_RG_AUXADC_REG_RST_SHIFT                   10
#define PMU_RG_CLK_TRIM_RST_MASK                      0x1
#define PMU_RG_CLK_TRIM_RST_SHIFT                     13
#define PMU_RG_TST_RST_MASK                           0x1
#define PMU_RG_TST_RST_SHIFT                          14
#define PMU_RG_WDTRSTB_EN_MASK                        0x1
#define PMU_RG_WDTRSTB_EN_SHIFT                       4
#define PMU_RG_WDTRSTB_MODE_MASK                      0x1
#define PMU_RG_WDTRSTB_MODE_SHIFT                     5
#define PMU_WDTRSTB_STATUS_MASK                       0x1
#define PMU_WDTRSTB_STATUS_SHIFT                      6
#define PMU_WDTRSTB_STATUS_CLR_MASK                   0x1
#define PMU_WDTRSTB_STATUS_CLR_SHIFT                  7
#define PMU_RG_WDTRSTB_FB_EN_MASK                     0x1
#define PMU_RG_WDTRSTB_FB_EN_SHIFT                    8
#define PMU_RG_WDTRSTB_DEB_MASK                       0x1
#define PMU_RG_WDTRSTB_DEB_SHIFT                      9
#define PMU_RG_WDTRST_EN_MASK                         0x1
#define PMU_RG_WDTRST_EN_SHIFT                        10
#define PMU_RG_WDTRST_ACT_MASK                        0x3
#define PMU_RG_WDTRST_ACT_SHIFT                       11
#define PMU_RG_STRUP_LONG_PRESS_RST_MASK              0x1
#define PMU_RG_STRUP_LONG_PRESS_RST_SHIFT             13
#define PMU_RG_FT_VR_SYSRSTB_MASK                     0x1
#define PMU_RG_FT_VR_SYSRSTB_SHIFT                    14
#define PMU_SYSRSTB_STATUS_CLR_MASK                   0x1
#define PMU_SYSRSTB_STATUS_CLR_SHIFT                  7
#define PMU_SYSRSTB_STATUS_MASK                       0x1
#define PMU_SYSRSTB_STATUS_SHIFT                      8
#define PMU_RG_SYSRSTB_DEB_MASK                       0x1
#define PMU_RG_SYSRSTB_DEB_SHIFT                      9
#define PMU_RG_SYSRSTB_EN_MASK                        0x1
#define PMU_RG_SYSRSTB_EN_SHIFT                       10
#define PMU_RG_STRUP_ENVTEM_MASK                      0x1
#define PMU_RG_STRUP_ENVTEM_SHIFT                     14
#define PMU_RG_STRUP_ENVTEM_CTRL_MASK                 0x1
#define PMU_RG_STRUP_ENVTEM_CTRL_SHIFT                15
#define PMU_RG_STRUP_LONG_PRESS_EXT_SEL_MASK          0x3
#define PMU_RG_STRUP_LONG_PRESS_EXT_SEL_SHIFT         0
#define PMU_RG_STRUP_LONG_PRESS_EXT_TD_MASK           0x3
#define PMU_RG_STRUP_LONG_PRESS_EXT_TD_SHIFT          2
#define PMU_RG_STRUP_LONG_PRESS_EXT_EN_MASK           0x1
#define PMU_RG_STRUP_LONG_PRESS_EXT_EN_SHIFT          4
#define PMU_RG_STRUP_LONG_PRESS_EXT_CHR_CTRL_MASK     0x1
#define PMU_RG_STRUP_LONG_PRESS_EXT_CHR_CTRL_SHIFT    5
#define PMU_RG_STRUP_LONG_PRESS_EXT_PWRKEY_CTRL_MASK  0x1
#define PMU_RG_STRUP_LONG_PRESS_EXT_PWRKEY_CTRL_SHIFT 6
#define PMU_RG_STRUP_LONG_PRESS_EXT_RTCA_CTRL_MASK    0x1
#define PMU_RG_STRUP_LONG_PRESS_EXT_RTCA_CTRL_SHIFT   8
#define PMU_RG_PWRKEY_RST_TD_MASK                     0x3
#define PMU_RG_PWRKEY_RST_TD_SHIFT                    9
#define PMU_RG_PWRKEY_RST_EN_MASK                     0x1
#define PMU_RG_PWRKEY_RST_EN_SHIFT                    11
#define PMU_RG_DDUVLO_DEB_EN_MASK                     0x1
#define PMU_RG_DDUVLO_DEB_EN_SHIFT                    12
#define PMU_RG_PSEQ_PWRMSK_RST_SEL_MASK               0x1
#define PMU_RG_PSEQ_PWRMSK_RST_SEL_SHIFT              13
#define PMU_RG_POR_FLAG_MASK                          0x1
#define PMU_RG_POR_FLAG_SHIFT                         0
#define PMU_RG_VBATLDO_VLDO33_SW_SEL_MASK             0x1
#define PMU_RG_VBATLDO_VLDO33_SW_SEL_SHIFT            0
#define PMU_RG_VBATLDO_VLDO33_SW_MASK                 0x1
#define PMU_RG_VBATLDO_VLDO33_SW_SHIFT                1
#define PMU_RG_VAUD18DRV_VOSEL_HW_MODE_MASK           0x1
#define PMU_RG_VAUD18DRV_VOSEL_HW_MODE_SHIFT          0
#define PMU_RG_VAUD18DRV_VOSEL_SW_MASK                0x1
#define PMU_RG_VAUD18DRV_VOSEL_SW_SHIFT               1
#define PMU_RG_AO_EFUSE_RSV_MASK                      0xFFFF
#define PMU_RG_AO_EFUSE_RSV_SHIFT                     0
#define PMU_TST_W_KEY_MASK                            0xFFFF
#define PMU_TST_W_KEY_SHIFT                           0
#define PMU_RG_CAP_LPSD_EN_MASK                       0x1
#define PMU_RG_CAP_LPSD_EN_SHIFT                      0
#define PMU_RG_CAP_LPSD_EN_ONLY_VBUS_PLUGIN_MASK      0x1
#define PMU_RG_CAP_LPSD_EN_ONLY_VBUS_PLUGIN_SHIFT     1
#define PMU_RG_CAP_LPSD_SW_MODE_MASK                  0x1
#define PMU_RG_CAP_LPSD_SW_MODE_SHIFT                 0
#define PMU_RG_CAP_LPSD_MASK                          0x1
#define PMU_RG_CAP_LPSD_SHIFT                         1
#define PMU_RG_INT_EN_PWRKEY_MASK                     0x1
#define PMU_RG_INT_EN_PWRKEY_SHIFT                    0
#define PMU_RG_INT_EN_PWRKEY_R_MASK                   0x1
#define PMU_RG_INT_EN_PWRKEY_R_SHIFT                  1
#define PMU_RG_INT_EN_VLDO33_LBAT_DET_MASK            0x1
#define PMU_RG_INT_EN_VLDO33_LBAT_DET_SHIFT           2
#define PMU_RG_INT_EN_VBAT_RECHG_MASK                 0x1
#define PMU_RG_INT_EN_VBAT_RECHG_SHIFT                3
#define PMU_RG_INT_EN_JEITA_HOT_MASK                  0x1
#define PMU_RG_INT_EN_JEITA_HOT_SHIFT                 4
#define PMU_RG_INT_EN_JEITA_WARM_MASK                 0x1
#define PMU_RG_INT_EN_JEITA_WARM_SHIFT                5
#define PMU_RG_INT_EN_JEITA_COOL_MASK                 0x1
#define PMU_RG_INT_EN_JEITA_COOL_SHIFT                6
#define PMU_RG_INT_EN_JEITA_COLD_MASK                 0x1
#define PMU_RG_INT_EN_JEITA_COLD_SHIFT                7
#define PMU_RG_INT_EN_BATOV_MASK                      0x1
#define PMU_RG_INT_EN_BATOV_SHIFT                     8
#define PMU_RG_INT_EN_CHRDET_MASK                     0x1
#define PMU_RG_INT_EN_CHRDET_SHIFT                    9
#define PMU_RG_INT_EN_CHGSTATINT_MASK                 0x1
#define PMU_RG_INT_EN_CHGSTATINT_SHIFT                10
#define PMU_RG_INT_EN_VBUS_OVP_MASK                   0x1
#define PMU_RG_INT_EN_VBUS_OVP_SHIFT                  11
#define PMU_RG_INT_EN_VBUS_UVLO_MASK                  0x1
#define PMU_RG_INT_EN_VBUS_UVLO_SHIFT                 12
#define PMU_RG_INT_EN_ICHR_ITERM_MASK                 0x1
#define PMU_RG_INT_EN_ICHR_ITERM_SHIFT                13
#define PMU_RG_INT_EN_ICHR_CHG_CUR_MASK               0x1
#define PMU_RG_INT_EN_ICHR_CHG_CUR_SHIFT              14
#define PMU_RG_INT_EN_SAFETY_TIMEOUT_MASK             0x1
#define PMU_RG_INT_EN_SAFETY_TIMEOUT_SHIFT            15
#define PMU_INT_CON0_SET_MASK                         0xFFFF
#define PMU_INT_CON0_SET_SHIFT                        0
#define PMU_INT_CON0_CLR_MASK                         0xFFFF
#define PMU_INT_CON0_CLR_SHIFT                        0
#define PMU_RG_INT_EN_AD_LBAT_LV_MASK                 0x1
#define PMU_RG_INT_EN_AD_LBAT_LV_SHIFT                0
#define PMU_RG_INT_EN_THM_OVER40_MASK                 0x1
#define PMU_RG_INT_EN_THM_OVER40_SHIFT                1
#define PMU_RG_INT_EN_THM_OVER55_MASK                 0x1
#define PMU_RG_INT_EN_THM_OVER55_SHIFT                2
#define PMU_RG_INT_EN_THM_OVER110_MASK                0x1
#define PMU_RG_INT_EN_THM_OVER110_SHIFT               3
#define PMU_RG_INT_EN_THM_OVER125_MASK                0x1
#define PMU_RG_INT_EN_THM_OVER125_SHIFT               4
#define PMU_RG_INT_EN_THM_UNDER40_MASK                0x1
#define PMU_RG_INT_EN_THM_UNDER40_SHIFT               5
#define PMU_RG_INT_EN_THM_UNDER55_MASK                0x1
#define PMU_RG_INT_EN_THM_UNDER55_SHIFT               6
#define PMU_RG_INT_EN_THM_UNDER110_MASK               0x1
#define PMU_RG_INT_EN_THM_UNDER110_SHIFT              7
#define PMU_RG_INT_EN_THM_UNDER125_MASK               0x1
#define PMU_RG_INT_EN_THM_UNDER125_SHIFT              8
#define PMU_RG_INT_EN_BAT2_H_R_MASK                   0x1
#define PMU_RG_INT_EN_BAT2_H_R_SHIFT                  9
#define PMU_RG_INT_EN_BAT_H_LV_MASK                   0x1
#define PMU_RG_INT_EN_BAT_H_LV_SHIFT                  10
#define PMU_RG_INT_EN_BAT_L_LV_MASK                   0x1
#define PMU_RG_INT_EN_BAT_L_LV_SHIFT                  11
#define PMU_RG_INT_EN_THR_H_R_MASK                    0x1
#define PMU_RG_INT_EN_THR_H_R_SHIFT                   12
#define PMU_RG_INT_EN_THR_H_F_MASK                    0x1
#define PMU_RG_INT_EN_THR_H_F_SHIFT                   13
#define PMU_RG_INT_EN_THR_L_R_MASK                    0x1
#define PMU_RG_INT_EN_THR_L_R_SHIFT                   14
#define PMU_RG_INT_EN_THR_L_F_MASK                    0x1
#define PMU_RG_INT_EN_THR_L_F_SHIFT                   15
#define PMU_INT_CON1_SET_MASK                         0xFFFF
#define PMU_INT_CON1_SET_SHIFT                        0
#define PMU_INT_CON1_CLR_MASK                         0xFFFF
#define PMU_INT_CON1_CLR_SHIFT                        0
#define PMU_RG_INT_EN_VCORE_OC_MASK                   0x1
#define PMU_RG_INT_EN_VCORE_OC_SHIFT                  0
#define PMU_RG_INT_EN_VIO18_OC_MASK                   0x1
#define PMU_RG_INT_EN_VIO18_OC_SHIFT                  1
#define PMU_RG_INT_EN_VRF_OC_MASK                     0x1
#define PMU_RG_INT_EN_VRF_OC_SHIFT                    2
#define PMU_RG_INT_EN_VAUD18_OC_MASK                  0x1
#define PMU_RG_INT_EN_VAUD18_OC_SHIFT                 3
#define PMU_RG_INT_EN_VLDO33_OC_MASK                  0x1
#define PMU_RG_INT_EN_VLDO33_OC_SHIFT                 4
#define PMU_RG_INT_EN_VA18_OC_MASK                    0x1
#define PMU_RG_INT_EN_VA18_OC_SHIFT                   5
#define PMU_RG_INT_EN_VRF11_OC_MASK                   0x1
#define PMU_RG_INT_EN_VRF11_OC_SHIFT                  6
#define PMU_RG_INT_EN_VSRAM_OC_MASK                   0x1
#define PMU_RG_INT_EN_VSRAM_OC_SHIFT                  7
#define PMU_INT_CON2_SET_MASK                         0xFFFF
#define PMU_INT_CON2_SET_SHIFT                        0
#define PMU_INT_CON2_CLR_MASK                         0xFFFF
#define PMU_INT_CON2_CLR_SHIFT                        0
#define PMU_RG_INT_EN_ILIMINT_MASK                    0x1
#define PMU_RG_INT_EN_ILIMINT_SHIFT                   0
#define PMU_RG_INT_EN_THERMREGINT_MASK                0x1
#define PMU_RG_INT_EN_THERMREGINT_SHIFT               1
#define PMU_RG_INT_EN_AVDD50_OC_MASK                  0x1
#define PMU_RG_INT_EN_AVDD50_OC_SHIFT                 2
#define PMU_RG_INT_EN_VSYS_DPM_MASK                   0x1
#define PMU_RG_INT_EN_VSYS_DPM_SHIFT                  3
#define PMU_INT_CON3_SET_MASK                         0xFFFF
#define PMU_INT_CON3_SET_SHIFT                        0
#define PMU_INT_CON3_CLR_MASK                         0xFFFF
#define PMU_INT_CON3_CLR_SHIFT                        0
#define PMU_RG_INT_MASK_PWRKEY_MASK                   0x1
#define PMU_RG_INT_MASK_PWRKEY_SHIFT                  0
#define PMU_RG_INT_MASK_PWRKEY_R_MASK                 0x1
#define PMU_RG_INT_MASK_PWRKEY_R_SHIFT                1
#define PMU_RG_INT_MASK_VLDO33_LBAT_DET_MASK          0x1
#define PMU_RG_INT_MASK_VLDO33_LBAT_DET_SHIFT         2
#define PMU_RG_INT_MASK_VBAT_RECHG_MASK               0x1
#define PMU_RG_INT_MASK_VBAT_RECHG_SHIFT              3
#define PMU_RG_INT_MASK_JEITA_HOT_MASK                0x1
#define PMU_RG_INT_MASK_JEITA_HOT_SHIFT               4
#define PMU_RG_INT_MASK_JEITA_WARM_MASK               0x1
#define PMU_RG_INT_MASK_JEITA_WARM_SHIFT              5
#define PMU_RG_INT_MASK_JEITA_COOL_MASK               0x1
#define PMU_RG_INT_MASK_JEITA_COOL_SHIFT              6
#define PMU_RG_INT_MASK_JEITA_COLD_MASK               0x1
#define PMU_RG_INT_MASK_JEITA_COLD_SHIFT              7
#define PMU_RG_INT_MASK_BATOV_MASK                    0x1
#define PMU_RG_INT_MASK_BATOV_SHIFT                   8
#define PMU_RG_INT_MASK_CHRDET_MASK                   0x1
#define PMU_RG_INT_MASK_CHRDET_SHIFT                  9
#define PMU_RG_INT_MASK_CHGSTATINT_MASK               0x1
#define PMU_RG_INT_MASK_CHGSTATINT_SHIFT              10
#define PMU_RG_INT_MASK_VBUS_OVP_MASK                 0x1
#define PMU_RG_INT_MASK_VBUS_OVP_SHIFT                11
#define PMU_RG_INT_MASK_VBUS_UVLO_MASK                0x1
#define PMU_RG_INT_MASK_VBUS_UVLO_SHIFT               12
#define PMU_RG_INT_MASK_ICHR_ITERM_MASK               0x1
#define PMU_RG_INT_MASK_ICHR_ITERM_SHIFT              13
#define PMU_RG_INT_MASK_ICHR_CHG_CUR_MASK             0x1
#define PMU_RG_INT_MASK_ICHR_CHG_CUR_SHIFT            14
#define PMU_RG_INT_MASK_SAFETY_TIMEOUT_MASK           0x1
#define PMU_RG_INT_MASK_SAFETY_TIMEOUT_SHIFT          15
#define PMU_INT_MASK_CON0_SET_MASK                    0xFFFF
#define PMU_INT_MASK_CON0_SET_SHIFT                   0
#define PMU_INT_MASK_CON0_CLR_MASK                    0xFFFF
#define PMU_INT_MASK_CON0_CLR_SHIFT                   0
#define PMU_RG_INT_MASK_AD_LBAT_LV_MASK               0x1
#define PMU_RG_INT_MASK_AD_LBAT_LV_SHIFT              0
#define PMU_RG_INT_MASK_THM_OVER40_MASK               0x1
#define PMU_RG_INT_MASK_THM_OVER40_SHIFT              1
#define PMU_RG_INT_MASK_THM_OVER55_MASK               0x1
#define PMU_RG_INT_MASK_THM_OVER55_SHIFT              2
#define PMU_RG_INT_MASK_THM_OVER110_MASK              0x1
#define PMU_RG_INT_MASK_THM_OVER110_SHIFT             3
#define PMU_RG_INT_MASK_THM_OVER125_MASK              0x1
#define PMU_RG_INT_MASK_THM_OVER125_SHIFT             4
#define PMU_RG_INT_MASK_THM_UNDER40_MASK              0x1
#define PMU_RG_INT_MASK_THM_UNDER40_SHIFT             5
#define PMU_RG_INT_MASK_THM_UNDER55_MASK              0x1
#define PMU_RG_INT_MASK_THM_UNDER55_SHIFT             6
#define PMU_RG_INT_MASK_THM_UNDER110_MASK             0x1
#define PMU_RG_INT_MASK_THM_UNDER110_SHIFT            7
#define PMU_RG_INT_MASK_THM_UNDER125_MASK             0x1
#define PMU_RG_INT_MASK_THM_UNDER125_SHIFT            8
#define PMU_RG_INT_MASK_BAT2_H_R_MASK                 0x1
#define PMU_RG_INT_MASK_BAT2_H_R_SHIFT                9
#define PMU_RG_INT_MASK_BAT_H_LV_MASK                 0x1
#define PMU_RG_INT_MASK_BAT_H_LV_SHIFT                10
#define PMU_RG_INT_MASK_BAT_L_LV_MASK                 0x1
#define PMU_RG_INT_MASK_BAT_L_LV_SHIFT                11
#define PMU_RG_INT_MASK_THR_H_R_MASK                  0x1
#define PMU_RG_INT_MASK_THR_H_R_SHIFT                 12
#define PMU_RG_INT_MASK_THR_H_F_MASK                  0x1
#define PMU_RG_INT_MASK_THR_H_F_SHIFT                 13
#define PMU_RG_INT_MASK_THR_L_R_MASK                  0x1
#define PMU_RG_INT_MASK_THR_L_R_SHIFT                 14
#define PMU_RG_INT_MASK_THR_L_F_MASK                  0x1
#define PMU_RG_INT_MASK_THR_L_F_SHIFT                 15
#define PMU_INT_MASK_CON1_SET_MASK                    0xFFFF
#define PMU_INT_MASK_CON1_SET_SHIFT                   0
#define PMU_INT_MASK_CON1_CLR_MASK                    0xFFFF
#define PMU_INT_MASK_CON1_CLR_SHIFT                   0
#define PMU_RG_INT_MASK_VCORE_OC_MASK                 0x1
#define PMU_RG_INT_MASK_VCORE_OC_SHIFT                0
#define PMU_RG_INT_MASK_VIO18_OC_MASK                 0x1
#define PMU_RG_INT_MASK_VIO18_OC_SHIFT                1
#define PMU_RG_INT_MASK_VRF_OC_MASK                   0x1
#define PMU_RG_INT_MASK_VRF_OC_SHIFT                  2
#define PMU_RG_INT_MASK_VAUD18_OC_MASK                0x1
#define PMU_RG_INT_MASK_VAUD18_OC_SHIFT               3
#define PMU_RG_INT_MASK_VLDO33_OC_MASK                0x1
#define PMU_RG_INT_MASK_VLDO33_OC_SHIFT               4
#define PMU_RG_INT_MASK_VA18_OC_MASK                  0x1
#define PMU_RG_INT_MASK_VA18_OC_SHIFT                 5
#define PMU_RG_INT_MASK_VRF11_OC_MASK                 0x1
#define PMU_RG_INT_MASK_VRF11_OC_SHIFT                6
#define PMU_RG_INT_MASK_VSRAM_OC_MASK                 0x1
#define PMU_RG_INT_MASK_VSRAM_OC_SHIFT                7
#define PMU_INT_MASK_CON2_SET_MASK                    0xFFFF
#define PMU_INT_MASK_CON2_SET_SHIFT                   0
#define PMU_INT_MASK_CON2_CLR_MASK                    0xFFFF
#define PMU_INT_MASK_CON2_CLR_SHIFT                   0
#define PMU_RG_INT_MASK_ILIMINT_MASK                  0x1
#define PMU_RG_INT_MASK_ILIMINT_SHIFT                 0
#define PMU_RG_INT_MASK_THERMREGINT_MASK              0x1
#define PMU_RG_INT_MASK_THERMREGINT_SHIFT             1
#define PMU_RG_INT_MASK_AVDD50_OC_MASK                0x1
#define PMU_RG_INT_MASK_AVDD50_OC_SHIFT               2
#define PMU_RG_INT_MASK_VSYS_DPM_MASK                 0x1
#define PMU_RG_INT_MASK_VSYS_DPM_SHIFT                3
#define PMU_INT_MASK_CON3_SET_MASK                    0xFFFF
#define PMU_INT_MASK_CON3_SET_SHIFT                   0
#define PMU_INT_MASK_CON3_CLR_MASK                    0xFFFF
#define PMU_INT_MASK_CON3_CLR_SHIFT                   0
#define PMU_RG_INT_STATUS_PWRKEY_MASK                 0x1
#define PMU_RG_INT_STATUS_PWRKEY_SHIFT                0
#define PMU_RG_INT_STATUS_PWRKEY_R_MASK               0x1
#define PMU_RG_INT_STATUS_PWRKEY_R_SHIFT              1
#define PMU_RG_INT_STATUS_VLDO33_LBAT_DET_MASK        0x1
#define PMU_RG_INT_STATUS_VLDO33_LBAT_DET_SHIFT       2
#define PMU_RG_INT_STATUS_VBAT_RECHG_MASK             0x1
#define PMU_RG_INT_STATUS_VBAT_RECHG_SHIFT            3
#define PMU_RG_INT_STATUS_JEITA_HOT_MASK              0x1
#define PMU_RG_INT_STATUS_JEITA_HOT_SHIFT             4
#define PMU_RG_INT_STATUS_JEITA_WARM_MASK             0x1
#define PMU_RG_INT_STATUS_JEITA_WARM_SHIFT            5
#define PMU_RG_INT_STATUS_JEITA_COOL_MASK             0x1
#define PMU_RG_INT_STATUS_JEITA_COOL_SHIFT            6
#define PMU_RG_INT_STATUS_JEITA_COLD_MASK             0x1
#define PMU_RG_INT_STATUS_JEITA_COLD_SHIFT            7
#define PMU_RG_INT_STATUS_BATOV_MASK                  0x1
#define PMU_RG_INT_STATUS_BATOV_SHIFT                 8
#define PMU_RG_INT_STATUS_CHRDET_MASK                 0x1
#define PMU_RG_INT_STATUS_CHRDET_SHIFT                9
#define PMU_RG_INT_STATUS_CHGSTATINT_MASK             0x1
#define PMU_RG_INT_STATUS_CHGSTATINT_SHIFT            10
#define PMU_RG_INT_STATUS_VBUS_OVP_MASK               0x1
#define PMU_RG_INT_STATUS_VBUS_OVP_SHIFT              11
#define PMU_RG_INT_STATUS_VBUS_UVLO_MASK              0x1
#define PMU_RG_INT_STATUS_VBUS_UVLO_SHIFT             12
#define PMU_RG_INT_STATUS_ICHR_ITERM_MASK             0x1
#define PMU_RG_INT_STATUS_ICHR_ITERM_SHIFT            13
#define PMU_RG_INT_STATUS_ICHR_CHG_CUR_MASK           0x1
#define PMU_RG_INT_STATUS_ICHR_CHG_CUR_SHIFT          14
#define PMU_RG_INT_STATUS_SAFETY_TIMEOUT_MASK         0x1
#define PMU_RG_INT_STATUS_SAFETY_TIMEOUT_SHIFT        15
#define PMU_RG_INT_STATUS_AD_LBAT_LV_MASK             0x1
#define PMU_RG_INT_STATUS_AD_LBAT_LV_SHIFT            0
#define PMU_RG_INT_STATUS_THM_OVER40_MASK             0x1
#define PMU_RG_INT_STATUS_THM_OVER40_SHIFT            1
#define PMU_RG_INT_STATUS_THM_OVER55_MASK             0x1
#define PMU_RG_INT_STATUS_THM_OVER55_SHIFT            2
#define PMU_RG_INT_STATUS_THM_OVER110_MASK            0x1
#define PMU_RG_INT_STATUS_THM_OVER110_SHIFT           3
#define PMU_RG_INT_STATUS_THM_OVER125_MASK            0x1
#define PMU_RG_INT_STATUS_THM_OVER125_SHIFT           4
#define PMU_RG_INT_STATUS_THM_UNDER40_MASK            0x1
#define PMU_RG_INT_STATUS_THM_UNDER40_SHIFT           5
#define PMU_RG_INT_STATUS_THM_UNDER55_MASK            0x1
#define PMU_RG_INT_STATUS_THM_UNDER55_SHIFT           6
#define PMU_RG_INT_STATUS_THM_UNDER110_MASK           0x1
#define PMU_RG_INT_STATUS_THM_UNDER110_SHIFT          7
#define PMU_RG_INT_STATUS_THM_UNDER125_MASK           0x1
#define PMU_RG_INT_STATUS_THM_UNDER125_SHIFT          8
#define PMU_RG_INT_STATUS_BAT2_H_R_MASK               0x1
#define PMU_RG_INT_STATUS_BAT2_H_R_SHIFT              9
#define PMU_RG_INT_STATUS_BAT_H_LV_MASK               0x1
#define PMU_RG_INT_STATUS_BAT_H_LV_SHIFT              10
#define PMU_RG_INT_STATUS_BAT_L_LV_MASK               0x1
#define PMU_RG_INT_STATUS_BAT_L_LV_SHIFT              11
#define PMU_RG_INT_STATUS_THR_H_R_MASK                0x1
#define PMU_RG_INT_STATUS_THR_H_R_SHIFT               12
#define PMU_RG_INT_STATUS_THR_H_F_MASK                0x1
#define PMU_RG_INT_STATUS_THR_H_F_SHIFT               13
#define PMU_RG_INT_STATUS_THR_L_R_MASK                0x1
#define PMU_RG_INT_STATUS_THR_L_R_SHIFT               14
#define PMU_RG_INT_STATUS_THR_L_F_MASK                0x1
#define PMU_RG_INT_STATUS_THR_L_F_SHIFT               15
#define PMU_RG_INT_STATUS_VCORE_OC_MASK               0x1
#define PMU_RG_INT_STATUS_VCORE_OC_SHIFT              0
#define PMU_RG_INT_STATUS_VIO18_OC_MASK               0x1
#define PMU_RG_INT_STATUS_VIO18_OC_SHIFT              1
#define PMU_RG_INT_STATUS_VRF_OC_MASK                 0x1
#define PMU_RG_INT_STATUS_VRF_OC_SHIFT                2
#define PMU_RG_INT_STATUS_VAUD18_OC_MASK              0x1
#define PMU_RG_INT_STATUS_VAUD18_OC_SHIFT             3
#define PMU_RG_INT_STATUS_VLDO33_OC_MASK              0x1
#define PMU_RG_INT_STATUS_VLDO33_OC_SHIFT             4
#define PMU_RG_INT_STATUS_VA18_OC_MASK                0x1
#define PMU_RG_INT_STATUS_VA18_OC_SHIFT               5
#define PMU_RG_INT_STATUS_VRF11_OC_MASK               0x1
#define PMU_RG_INT_STATUS_VRF11_OC_SHIFT              6
#define PMU_RG_INT_STATUS_VSRAM_OC_MASK               0x1
#define PMU_RG_INT_STATUS_VSRAM_OC_SHIFT              7
#define PMU_RG_INT_STATUS_ILIMINT_MASK                0x1
#define PMU_RG_INT_STATUS_ILIMINT_SHIFT               0
#define PMU_RG_INT_STATUS_THERMREGINT_MASK            0x1
#define PMU_RG_INT_STATUS_THERMREGINT_SHIFT           1
#define PMU_RG_INT_STATUS_AVDD50_OC_MASK              0x1
#define PMU_RG_INT_STATUS_AVDD50_OC_SHIFT             2
#define PMU_RG_INT_STATUS_VSYS_DPM_MASK               0x1
#define PMU_RG_INT_STATUS_VSYS_DPM_SHIFT              3
#define PMU_RG_INT_RAW_STATUS_PWRKEY_MASK             0x1
#define PMU_RG_INT_RAW_STATUS_PWRKEY_SHIFT            0
#define PMU_RG_INT_RAW_STATUS_PWRKEY_R_MASK           0x1
#define PMU_RG_INT_RAW_STATUS_PWRKEY_R_SHIFT          1
#define PMU_RG_INT_RAW_STATUS_VLDO33_LBAT_DET_MASK    0x1
#define PMU_RG_INT_RAW_STATUS_VLDO33_LBAT_DET_SHIFT   2
#define PMU_RG_INT_RAW_STATUS_VBAT_RECHG_MASK         0x1
#define PMU_RG_INT_RAW_STATUS_VBAT_RECHG_SHIFT        3
#define PMU_RG_INT_RAW_STATUS_JEITA_HOT_MASK          0x1
#define PMU_RG_INT_RAW_STATUS_JEITA_HOT_SHIFT         4
#define PMU_RG_INT_RAW_STATUS_JEITA_WARM_MASK         0x1
#define PMU_RG_INT_RAW_STATUS_JEITA_WARM_SHIFT        5
#define PMU_RG_INT_RAW_STATUS_JEITA_COOL_MASK         0x1
#define PMU_RG_INT_RAW_STATUS_JEITA_COOL_SHIFT        6
#define PMU_RG_INT_RAW_STATUS_JEITA_COLD_MASK         0x1
#define PMU_RG_INT_RAW_STATUS_JEITA_COLD_SHIFT        7
#define PMU_RG_INT_RAW_STATUS_BATOV_MASK              0x1
#define PMU_RG_INT_RAW_STATUS_BATOV_SHIFT             8
#define PMU_RG_INT_RAW_STATUS_CHRDET_MASK             0x1
#define PMU_RG_INT_RAW_STATUS_CHRDET_SHIFT            9
#define PMU_RG_INT_RAW_STATUS_CHGSTATINT_MASK         0x1
#define PMU_RG_INT_RAW_STATUS_CHGSTATINT_SHIFT        10
#define PMU_RG_INT_RAW_STATUS_VBUS_OVP_MASK           0x1
#define PMU_RG_INT_RAW_STATUS_VBUS_OVP_SHIFT          11
#define PMU_RG_INT_RAW_STATUS_VBUS_UVLO_MASK          0x1
#define PMU_RG_INT_RAW_STATUS_VBUS_UVLO_SHIFT         12
#define PMU_RG_INT_RAW_STATUS_ICHR_ITERM_MASK         0x1
#define PMU_RG_INT_RAW_STATUS_ICHR_ITERM_SHIFT        13
#define PMU_RG_INT_RAW_STATUS_ICHR_CHG_CUR_MASK       0x1
#define PMU_RG_INT_RAW_STATUS_ICHR_CHG_CUR_SHIFT      14
#define PMU_RG_INT_RAW_STATUS_SAFETY_TIMEOUT_MASK     0x1
#define PMU_RG_INT_RAW_STATUS_SAFETY_TIMEOUT_SHIFT    15
#define PMU_RG_INT_RAW_STATUS_AD_LBAT_LV_MASK         0x1
#define PMU_RG_INT_RAW_STATUS_AD_LBAT_LV_SHIFT        0
#define PMU_RG_INT_RAW_STATUS_THM_OVER40_MASK         0x1
#define PMU_RG_INT_RAW_STATUS_THM_OVER40_SHIFT        1
#define PMU_RG_INT_RAW_STATUS_THM_OVER55_MASK         0x1
#define PMU_RG_INT_RAW_STATUS_THM_OVER55_SHIFT        2
#define PMU_RG_INT_RAW_STATUS_THM_OVER110_MASK        0x1
#define PMU_RG_INT_RAW_STATUS_THM_OVER110_SHIFT       3
#define PMU_RG_INT_RAW_STATUS_THM_OVER125_MASK        0x1
#define PMU_RG_INT_RAW_STATUS_THM_OVER125_SHIFT       4
#define PMU_RG_INT_RAW_STATUS_THM_UNDER40_MASK        0x1
#define PMU_RG_INT_RAW_STATUS_THM_UNDER40_SHIFT       5
#define PMU_RG_INT_RAW_STATUS_THM_UNDER55_MASK        0x1
#define PMU_RG_INT_RAW_STATUS_THM_UNDER55_SHIFT       6
#define PMU_RG_INT_RAW_STATUS_THM_UNDER110_MASK       0x1
#define PMU_RG_INT_RAW_STATUS_THM_UNDER110_SHIFT      7
#define PMU_RG_INT_RAW_STATUS_THM_UNDER125_MASK       0x1
#define PMU_RG_INT_RAW_STATUS_THM_UNDER125_SHIFT      8
#define PMU_RG_INT_RAW_STATUS_BAT2_H_R_MASK           0x1
#define PMU_RG_INT_RAW_STATUS_BAT2_H_R_SHIFT          9
#define PMU_RG_INT_RAW_STATUS_BAT_H_LV_MASK           0x1
#define PMU_RG_INT_RAW_STATUS_BAT_H_LV_SHIFT          10
#define PMU_RG_INT_RAW_STATUS_BAT_L_LV_MASK           0x1
#define PMU_RG_INT_RAW_STATUS_BAT_L_LV_SHIFT          11
#define PMU_RG_INT_RAW_STATUS_THR_H_R_MASK            0x1
#define PMU_RG_INT_RAW_STATUS_THR_H_R_SHIFT           12
#define PMU_RG_INT_RAW_STATUS_THR_H_F_MASK            0x1
#define PMU_RG_INT_RAW_STATUS_THR_H_F_SHIFT           13
#define PMU_RG_INT_RAW_STATUS_THR_L_R_MASK            0x1
#define PMU_RG_INT_RAW_STATUS_THR_L_R_SHIFT           14
#define PMU_RG_INT_RAW_STATUS_THR_L_F_MASK            0x1
#define PMU_RG_INT_RAW_STATUS_THR_L_F_SHIFT           15
#define PMU_RG_INT_RAW_STATUS_VCORE_OC_MASK           0x1
#define PMU_RG_INT_RAW_STATUS_VCORE_OC_SHIFT          0
#define PMU_RG_INT_RAW_STATUS_VIO18_OC_MASK           0x1
#define PMU_RG_INT_RAW_STATUS_VIO18_OC_SHIFT          1
#define PMU_RG_INT_RAW_STATUS_VRF_OC_MASK             0x1
#define PMU_RG_INT_RAW_STATUS_VRF_OC_SHIFT            2
#define PMU_RG_INT_RAW_STATUS_VAUD18_OC_MASK          0x1
#define PMU_RG_INT_RAW_STATUS_VAUD18_OC_SHIFT         3
#define PMU_RG_INT_RAW_STATUS_VLDO33_OC_MASK          0x1
#define PMU_RG_INT_RAW_STATUS_VLDO33_OC_SHIFT         4
#define PMU_RG_INT_RAW_STATUS_VA18_OC_MASK            0x1
#define PMU_RG_INT_RAW_STATUS_VA18_OC_SHIFT           5
#define PMU_RG_INT_RAW_STATUS_VRF11_OC_MASK           0x1
#define PMU_RG_INT_RAW_STATUS_VRF11_OC_SHIFT          6
#define PMU_RG_INT_RAW_STATUS_VSRAM_OC_MASK           0x1
#define PMU_RG_INT_RAW_STATUS_VSRAM_OC_SHIFT          7
#define PMU_RG_INT_RAW_STATUS_ILIMINT_MASK            0x1
#define PMU_RG_INT_RAW_STATUS_ILIMINT_SHIFT           0
#define PMU_RG_INT_RAW_STATUS_THERMREGINT_MASK        0x1
#define PMU_RG_INT_RAW_STATUS_THERMREGINT_SHIFT       1
#define PMU_RG_INT_RAW_STATUS_AVDD50_OC_MASK          0x1
#define PMU_RG_INT_RAW_STATUS_AVDD50_OC_SHIFT         2
#define PMU_RG_INT_RAW_STATUS_VSYS_DPM_MASK           0x1
#define PMU_RG_INT_RAW_STATUS_VSYS_DPM_SHIFT          3
#define PMU_POLARITY_MASK                             0x1
#define PMU_POLARITY_SHIFT                            0
#define PMU_RG_PWRKEY_RISING_EN_MASK                  0x1
#define PMU_RG_PWRKEY_RISING_EN_SHIFT                 1
#define PMU_RG_PWRKEY_FALLING_EN_MASK                 0x1
#define PMU_RG_PWRKEY_FALLING_EN_SHIFT                2
#define PMU_RG_CHRDET_RISING_EN_MASK                  0x1
#define PMU_RG_CHRDET_RISING_EN_SHIFT                 3
#define PMU_RG_CHRDET_FALLING_EN_MASK                 0x1
#define PMU_RG_CHRDET_FALLING_EN_SHIFT                4
#define PMU_STS_PWRKEY_MASK                           0x1
#define PMU_STS_PWRKEY_SHIFT                          0
#define PMU_STS_RTCA_MASK                             0x1
#define PMU_STS_RTCA_SHIFT                            1
#define PMU_STS_CHRIN_MASK                            0x1
#define PMU_STS_CHRIN_SHIFT                           2
#define PMU_STS_SPAR_MASK                             0x1
#define PMU_STS_SPAR_SHIFT                            3
#define PMU_STS_RBOOT_MASK                            0x1
#define PMU_STS_RBOOT_SHIFT                           4
#define PMU_RG_PONSTS_CLR_MASK                        0x1
#define PMU_RG_PONSTS_CLR_SHIFT                       8
#define PMU_RG_POFFSTS_EN_MASK                        0x3FFF
#define PMU_RG_POFFSTS_EN_SHIFT                       1
#define PMU_PWRKEY_VAL_MASK                           0x1
#define PMU_PWRKEY_VAL_SHIFT                          0
#define PMU_CHRDET_VAL_MASK                           0x1
#define PMU_CHRDET_VAL_SHIFT                          1
#define PMU_PSI1B_VAL_MASK                            0x1
#define PMU_PSI1B_VAL_SHIFT                           2
#define PMU_OSC32_EN_MASK                             0x1
#define PMU_OSC32_EN_SHIFT                            3
#define PMU_SMPS_EN_MASK                              0x1
#define PMU_SMPS_EN_SHIFT                             4
#define PMU_OSC_EN_MASK                               0x1
#define PMU_OSC_EN_SHIFT                              5
#define PMU_IVGEN_ON_MASK                             0x1
#define PMU_IVGEN_ON_SHIFT                            6
#define PMU_BATDET_ON_MASK                            0x1
#define PMU_BATDET_ON_SHIFT                           7
#define PMU_BGR_HANDOVER_MASK                         0x1
#define PMU_BGR_HANDOVER_SHIFT                        8
#define PMU_BGR_ON_MASK                               0x1
#define PMU_BGR_ON_SHIFT                              9
#define PMU_PP_EN_MASK                                0x1
#define PMU_PP_EN_SHIFT                               10
#define PMU_AD_QI_SHIPMODE_MASK                       0x1
#define PMU_AD_QI_SHIPMODE_SHIFT                      11
#define PMU_ISO_EN_MASK                               0x1
#define PMU_ISO_EN_SHIFT                              12
#define PMU_CO_ON_MASK                                0x1
#define PMU_CO_ON_SHIFT                               13
#define PMU_CO_RDY_MASK                               0x1
#define PMU_CO_RDY_SHIFT                              14
#define PMU_STS_BOOT_MODE_MASK                        0x1
#define PMU_STS_BOOT_MODE_SHIFT                       15
#define PMU_STRUP_VRF_OC_STATUS_MASK                  0x1
#define PMU_STRUP_VRF_OC_STATUS_SHIFT                 12
#define PMU_STRUP_VAUD18_OC_STATUS_MASK               0x1
#define PMU_STRUP_VAUD18_OC_STATUS_SHIFT              13
#define PMU_STRUP_VIO18_OC_STATUS_MASK                0x1
#define PMU_STRUP_VIO18_OC_STATUS_SHIFT               14
#define PMU_STRUP_VCORE_OC_STATUS_MASK                0x1
#define PMU_STRUP_VCORE_OC_STATUS_SHIFT               15
#define PMU_VRF11_PG_DEB_MASK                         0x1
#define PMU_VRF11_PG_DEB_SHIFT                        8
#define PMU_VSRAM_PG_DEB_MASK                         0x1
#define PMU_VSRAM_PG_DEB_SHIFT                        9
#define PMU_VLDO33_PG_DEB_MASK                        0x1
#define PMU_VLDO33_PG_DEB_SHIFT                       10
#define PMU_VA18_PG_DEB_MASK                          0x1
#define PMU_VA18_PG_DEB_SHIFT                         11
#define PMU_VRF_PG_DEB_MASK                           0x1
#define PMU_VRF_PG_DEB_SHIFT                          12
#define PMU_VAUD18_PG_DEB_MASK                        0x1
#define PMU_VAUD18_PG_DEB_SHIFT                       13
#define PMU_VIO18_PG_DEB_MASK                         0x1
#define PMU_VIO18_PG_DEB_SHIFT                        14
#define PMU_VCORE_PG_DEB_MASK                         0x1
#define PMU_VCORE_PG_DEB_SHIFT                        15
#define PMU_STRUP_VRF11_PG_STATUS_MASK                0x1
#define PMU_STRUP_VRF11_PG_STATUS_SHIFT               8
#define PMU_STRUP_VSRAM_PG_STATUS_MASK                0x1
#define PMU_STRUP_VSRAM_PG_STATUS_SHIFT               9
#define PMU_STRUP_VLDO33_PG_STATUS_MASK               0x1
#define PMU_STRUP_VLDO33_PG_STATUS_SHIFT              10
#define PMU_STRUP_VA18_PG_STATUS_MASK                 0x1
#define PMU_STRUP_VA18_PG_STATUS_SHIFT                11
#define PMU_STRUP_VRF_PG_STATUS_MASK                  0x1
#define PMU_STRUP_VRF_PG_STATUS_SHIFT                 12
#define PMU_STRUP_VAUD18_PG_STATUS_MASK               0x1
#define PMU_STRUP_VAUD18_PG_STATUS_SHIFT              13
#define PMU_STRUP_VIO18_PG_STATUS_MASK                0x1
#define PMU_STRUP_VIO18_PG_STATUS_SHIFT               14
#define PMU_STRUP_VCORE_PG_STATUS_MASK                0x1
#define PMU_STRUP_VCORE_PG_STATUS_SHIFT               15
#define PMU_STS_THM_OVER40_MASK                       0x1
#define PMU_STS_THM_OVER40_SHIFT                      0
#define PMU_STS_THM_OVER55_MASK                       0x1
#define PMU_STS_THM_OVER55_SHIFT                      1
#define PMU_STS_THM_OVER110_MASK                      0x1
#define PMU_STS_THM_OVER110_SHIFT                     2
#define PMU_STS_THM_OVER125_MASK                      0x1
#define PMU_STS_THM_OVER125_SHIFT                     3
#define PMU_STS_THM_RAW_STS_MASK                      0x1
#define PMU_STS_THM_RAW_STS_SHIFT                     4
#define PMU_STS_THM_DEB_STS_MASK                      0x1
#define PMU_STS_THM_DEB_STS_SHIFT                     5
#define PMU_RG_LBAT_INT_EN_MASK                       0x1
#define PMU_RG_LBAT_INT_EN_SHIFT                      0
#define PMU_RG_UVLO_SLEEP_EN_MASK                     0x1
#define PMU_RG_UVLO_SLEEP_EN_SHIFT                    1
#define PMU_RG_BGR_SLEEP_EN_MASK                      0x1
#define PMU_RG_BGR_SLEEP_EN_SHIFT                     2
#define PMU_RG_PWROFF_MODE_MASK                       0x1
#define PMU_RG_PWROFF_MODE_SHIFT                      0
#define PMU_RG_SRCLKEN_MASK                           0x1
#define PMU_RG_SRCLKEN_SHIFT                          0
#define PMU_RG_SRCLKEN_HW_MODE_MASK                   0x1
#define PMU_RG_SRCLKEN_HW_MODE_SHIFT                  1
#define PMU_RG_AON_MASK                               0x1
#define PMU_RG_AON_SHIFT                              0
#define PMU_RG_CO_ON_WAIT_RDY_MASK                    0x1
#define PMU_RG_CO_ON_WAIT_RDY_SHIFT                   1
#define PMU_RG_CO_HW_PDN_EN_MASK                      0x1
#define PMU_RG_CO_HW_PDN_EN_SHIFT                     2
#define PMU_RG_OSC32_AUTO_PDN_MASK                    0x1
#define PMU_RG_OSC32_AUTO_PDN_SHIFT                   3
#define PMU_RG_CO_RSTB_CNT_MASK                       0x3
#define PMU_RG_CO_RSTB_CNT_SHIFT                      4
#define PMU_RG_CO_ON_SW_MODE_MASK                     0x1
#define PMU_RG_CO_ON_SW_MODE_SHIFT                    6
#define PMU_RG_CO_ON_MASK                             0x1
#define PMU_RG_CO_ON_SHIFT                            7
#define PMU_RG_CO_ISO_ON_SW_MODE_MASK                 0x1
#define PMU_RG_CO_ISO_ON_SW_MODE_SHIFT                8
#define PMU_RG_CO_ISO_ON_MASK                         0x1
#define PMU_RG_CO_ISO_ON_SHIFT                        9
#define PMU_RG_CO_RSTB_SW_MODE_MASK                   0x1
#define PMU_RG_CO_RSTB_SW_MODE_SHIFT                  10
#define PMU_RG_CO_RSTB_MASK                           0x1
#define PMU_RG_CO_RSTB_SHIFT                          11
#define PMU_RG_SHIPMODE_EXIT_SW_MODE_MASK             0x1
#define PMU_RG_SHIPMODE_EXIT_SW_MODE_SHIFT            12
#define PMU_RG_SHIPMODE_EXIT_MASK                     0x1
#define PMU_RG_SHIPMODE_EXIT_SHIFT                    13
#define PMU_RG_PP_EN_SW_MODE_MASK                     0x1
#define PMU_RG_PP_EN_SW_MODE_SHIFT                    14
#define PMU_RG_PP_EN_MASK                             0x1
#define PMU_RG_PP_EN_SHIFT                            15
#define PMU_RG_BGR_ON_SLEEPOFF_MASK                   0x1
#define PMU_RG_BGR_ON_SLEEPOFF_SHIFT                  0
#define PMU_RG_BATDET_ON_SLEEPOFF_MASK                0x1
#define PMU_RG_BATDET_ON_SLEEPOFF_SHIFT               1
#define PMU_RG_BGR_ON_SW_MODE_MASK                    0x1
#define PMU_RG_BGR_ON_SW_MODE_SHIFT                   2
#define PMU_RG_BGR_ON_MASK                            0x1
#define PMU_RG_BGR_ON_SHIFT                           3
#define PMU_RG_BGR_HANDOVER_SW_MODE_MASK              0x1
#define PMU_RG_BGR_HANDOVER_SW_MODE_SHIFT             4
#define PMU_RG_BGR_HANDOVER_MASK                      0x1
#define PMU_RG_BGR_HANDOVER_SHIFT                     5
#define PMU_RG_BATDET_ON_SW_MODE_MASK                 0x1
#define PMU_RG_BATDET_ON_SW_MODE_SHIFT                6
#define PMU_RG_BATDET_ON_MASK                         0x1
#define PMU_RG_BATDET_ON_SHIFT                        7
#define PMU_RG_OSC_EN_SW_MODE_MASK                    0x1
#define PMU_RG_OSC_EN_SW_MODE_SHIFT                   8
#define PMU_RG_OSC_EN_MASK                            0x1
#define PMU_RG_OSC_EN_SHIFT                           9
#define PMU_RG_SMPS_EN_SW_MODE_MASK                   0x1
#define PMU_RG_SMPS_EN_SW_MODE_SHIFT                  10
#define PMU_RG_SMPS_EN_MASK                           0x1
#define PMU_RG_SMPS_EN_SHIFT                          11
#define PMU_RG_IVGEN_ON_SW_MODE_MASK                  0x1
#define PMU_RG_IVGEN_ON_SW_MODE_SHIFT                 12
#define PMU_RG_IVGEN_ON_MASK                          0x1
#define PMU_RG_IVGEN_ON_SHIFT                         13
#define PMU_RG_PWRKEY_RESET_SW_MODE_MASK              0x1
#define PMU_RG_PWRKEY_RESET_SW_MODE_SHIFT             14
#define PMU_RG_PWRKEY_RESET_MASK                      0x1
#define PMU_RG_PWRKEY_RESET_SHIFT                     15
#define PMU_RG_POOR_CHR_OSC_DIS_MASK                  0x1
#define PMU_RG_POOR_CHR_OSC_DIS_SHIFT                 0
#define PMU_RG_CHR_SUSPEND_MODE_MASK                  0x1
#define PMU_RG_CHR_SUSPEND_MODE_SHIFT                 1
#define PMU_RG_SUSPEND_MODE_MASK                      0x3
#define PMU_RG_SUSPEND_MODE_SHIFT                     0
#define PMU_RG_LOW_IBUS_EN_LATCH_MASK                 0x1
#define PMU_RG_LOW_IBUS_EN_LATCH_SHIFT                0
#define PMU_RG_LOW_IBUS_EN_MASK                       0x1
#define PMU_RG_LOW_IBUS_EN_SHIFT                      1
#define PMU_AD_LOW_IBUS_EN_LAT_MASK                   0x1
#define PMU_AD_LOW_IBUS_EN_LAT_SHIFT                  4
#define PMU_AD_QI_PLUGOUT_ALARM_MASK                  0x1
#define PMU_AD_QI_PLUGOUT_ALARM_SHIFT                 5
#define PMU_RG_OC_EN_MASK                             0x1
#define PMU_RG_OC_EN_SHIFT                            8
#define PMU_RG_OC_EN_LAT_RELOAD_MASK                  0x1
#define PMU_RG_OC_EN_LAT_RELOAD_SHIFT                 9
#define PMU_RG_EOC_RTC_EN_MASK                        0x1
#define PMU_RG_EOC_RTC_EN_SHIFT                       0
#define PMU_RG_EOC_RTC_IVGEN_OFF_MASK                 0x1
#define PMU_RG_EOC_RTC_IVGEN_OFF_SHIFT                1
#define PMU_RG_EOC_RTC_6M_OFF_MASK                    0x1
#define PMU_RG_EOC_RTC_6M_OFF_SHIFT                   2
#define PMU_RG_PWRHOLD_MASK                           0x1
#define PMU_RG_PWRHOLD_SHIFT                          0
#define PMU_RG_THM_SHDN_EN_MASK                       0x1
#define PMU_RG_THM_SHDN_EN_SHIFT                      0
#define PMU_RG_THM_SHDN_SEL_MASK                      0x1
#define PMU_RG_THM_SHDN_SEL_SHIFT                     1
#define PMU_RG_STRUP_AUXADC_START_SW_MASK             0x1
#define PMU_RG_STRUP_AUXADC_START_SW_SHIFT            0
#define PMU_RG_STRUP_AUXADC_RSTB_SW_MASK              0x1
#define PMU_RG_STRUP_AUXADC_RSTB_SW_SHIFT             1
#define PMU_RG_STRUP_AUXADC_START_SEL_MASK            0x1
#define PMU_RG_STRUP_AUXADC_START_SEL_SHIFT           2
#define PMU_RG_STRUP_AUXADC_RSTB_SEL_MASK             0x1
#define PMU_RG_STRUP_AUXADC_RSTB_SEL_SHIFT            3
#define PMU_RG_STRUP_AUXADC_RPCNT_MAX_MASK            0x7F
#define PMU_RG_STRUP_AUXADC_RPCNT_MAX_SHIFT           4
#define PMU_RG_THRDET_SEL_MASK                        0x1
#define PMU_RG_THRDET_SEL_SHIFT                       0
#define PMU_RG_THR_TMODE_MASK                         0x1
#define PMU_RG_THR_TMODE_SHIFT                        3
#define PMU_RG_VREF_BG_MASK                           0x7
#define PMU_RG_VREF_BG_SHIFT                          4
#define PMU_RG_RST_DRVSEL_MASK                        0x1
#define PMU_RG_RST_DRVSEL_SHIFT                       7
#define PMU_RG_IVGEN_TRIM_EN_MASK                     0x1
#define PMU_RG_IVGEN_TRIM_EN_SHIFT                    8
#define PMU_RG_TM_OUT_MASK                            0x3
#define PMU_RG_TM_OUT_SHIFT                           10
#define PMU_RG_PMU_RSV_MASK                           0xF
#define PMU_RG_PMU_RSV_SHIFT                          12
#define PMU_RGS_ANA_CHIP_ID_MASK                      0x7
#define PMU_RGS_ANA_CHIP_ID_SHIFT                     1
#define PMU_RG_STRUP_IREF_TRIM_MASK                   0x3F
#define PMU_RG_STRUP_IREF_TRIM_SHIFT                  0
#define PMU_RG_BGR_UNCHOP_MASK                        0x1
#define PMU_RG_BGR_UNCHOP_SHIFT                       0
#define PMU_RG_BGR_UNCHOP_PH_MASK                     0x1
#define PMU_RG_BGR_UNCHOP_PH_SHIFT                    1
#define PMU_RG_BGR_RSEL_MASK                          0xF
#define PMU_RG_BGR_RSEL_SHIFT                         2
#define PMU_RG_BGR_TRIM_EN_MASK                       0x1
#define PMU_RG_BGR_TRIM_EN_SHIFT                      6
#define PMU_RG_BGR_TEST_RSTB_MASK                     0x1
#define PMU_RG_BGR_TEST_RSTB_SHIFT                    7
#define PMU_RG_BGR_TEST_EN_MASK                       0x1
#define PMU_RG_BGR_TEST_EN_SHIFT                      8
#define PMU_RG_UVLO_VTHL_MASK                         0x1F
#define PMU_RG_UVLO_VTHL_SHIFT                        11
#define PMU_RG_LBAT_INT_VTH_MASK                      0x1F
#define PMU_RG_LBAT_INT_VTH_SHIFT                     1
#define PMU_RG_BGRNOC_RSEL_MASK                       0xF
#define PMU_RG_BGRNOC_RSEL_SHIFT                      0
#define PMU_RG_BGRNOC_TRIMEN_MASK                     0x1
#define PMU_RG_BGRNOC_TRIMEN_SHIFT                    4
#define PMU_RG_IVGENNOC_TRIMEN_MASK                   0x1
#define PMU_RG_IVGENNOC_TRIMEN_SHIFT                  5
#define PMU_RG_AO_TRIM_REG_RELOAD_MASK                0x1
#define PMU_RG_AO_TRIM_REG_RELOAD_SHIFT               10
#define PMU_RG_IVGENNOC_I_ADJ_MASK                    0x3
#define PMU_RG_IVGENNOC_I_ADJ_SHIFT                   11
#define PMU_RG_POFFSTS_CLR_MASK                       0x1
#define PMU_RG_POFFSTS_CLR_SHIFT                      14
#define PMU_RGS_POFFSTS_MASK                          0x1F
#define PMU_RGS_POFFSTS_SHIFT                         0
#define PMU_AD_OC_STATUS_MASK                         0x1
#define PMU_AD_OC_STATUS_SHIFT                        5
#define PMU_RG_BGR_TRIM_MASK                          0x1F
#define PMU_RG_BGR_TRIM_SHIFT                         0
#define PMU_RG_BGRNOC_TRIM_MASK                       0x1F
#define PMU_RG_BGRNOC_TRIM_SHIFT                      5
#define PMU_RG_IVGENNOC_TRIM_MASK                     0x1F
#define PMU_RG_IVGENNOC_TRIM_SHIFT                    10
#define PMU_RG_UVLO_VTHH_MASK                         0x7
#define PMU_RG_UVLO_VTHH_SHIFT                        0
#define PMU_RG_VIO18_VSLEEP_TRIM_MASK                 0x1F
#define PMU_RG_VIO18_VSLEEP_TRIM_SHIFT                3
#define PMU_RG_VRF11_VSLEEP_TRIM_MASK                 0x1F
#define PMU_RG_VRF11_VSLEEP_TRIM_SHIFT                8
#define PMU_RG_VCORE_VSLEEP_V10_TRIM_MASK             0x1F
#define PMU_RG_VCORE_VSLEEP_V10_TRIM_SHIFT            0
#define PMU_RG_VCORE_VSLEEP_V08_TRIM_MASK             0x1F
#define PMU_RG_VCORE_VSLEEP_V08_TRIM_SHIFT            5
#define PMU_RG_VCORE_VSLEEP_V07_TRIM_MASK             0x1F
#define PMU_RG_VCORE_VSLEEP_V07_TRIM_SHIFT            10
#define PMU_RG_VAUD18_VSLEEP_V10_TRIM_MASK            0x1F
#define PMU_RG_VAUD18_VSLEEP_V10_TRIM_SHIFT           0
#define PMU_RG_VAUD18_VSLEEP_V09_TRIM_MASK            0x1F
#define PMU_RG_VAUD18_VSLEEP_V09_TRIM_SHIFT           5
#define PMU_RG_VAUD18_VSLEEP_V08_TRIM_MASK            0x1F
#define PMU_RG_VAUD18_VSLEEP_V08_TRIM_SHIFT           10
#define PMU_RG_EOSC_CHOP_EN_MASK                      0x1
#define PMU_RG_EOSC_CHOP_EN_SHIFT                     0
#define PMU_RG_EOSC_VCT_EN_MASK                       0x1
#define PMU_RG_EOSC_VCT_EN_SHIFT                      3
#define PMU_RG_DEGLITCH_LVSH_EN_MASK                  0x1
#define PMU_RG_DEGLITCH_LVSH_EN_SHIFT                 4
#define PMU_RG_EOSC_RSV_MASK                          0xF
#define PMU_RG_EOSC_RSV_SHIFT                         5
#define PMU_RG_EOSC_STP_CALI_MASK                     0x3
#define PMU_RG_EOSC_STP_CALI_SHIFT                    0
#define PMU_RG_DINV_ENB_MASK                          0x1
#define PMU_RG_DINV_ENB_SHIFT                         2
#define PMU_RG_CHR_LOW_IBUS_EN_LAT_SW_SEL_MASK        0x1
#define PMU_RG_CHR_LOW_IBUS_EN_LAT_SW_SEL_SHIFT       0
#define PMU_RG_CHR_LOW_IBUS_EN_LAT_SW_MASK            0x1
#define PMU_RG_CHR_LOW_IBUS_EN_LAT_SW_SHIFT           1
#define PMU_RG_CHR_VBUS_UVLO_DB_SW_SEL_MASK           0x1
#define PMU_RG_CHR_VBUS_UVLO_DB_SW_SEL_SHIFT          2
#define PMU_RG_CHR_VBUS_UVLO_DB_SW_MASK               0x1
#define PMU_RG_CHR_VBUS_UVLO_DB_SW_SHIFT              3
#define PMU_RG_CHR_VBUS_OVP_DB_SW_SEL_MASK            0x1
#define PMU_RG_CHR_VBUS_OVP_DB_SW_SEL_SHIFT           4
#define PMU_RG_CHR_VBUS_OVP_DB_SW_MASK                0x1
#define PMU_RG_CHR_VBUS_OVP_DB_SW_SHIFT               5
#define PMU_RG_CHR_REF_EN_SW_SEL_MASK                 0x1
#define PMU_RG_CHR_REF_EN_SW_SEL_SHIFT                6
#define PMU_RG_CHR_REF_EN_SW_MASK                     0x1
#define PMU_RG_CHR_REF_EN_SW_SHIFT                    7
#define PMU_RG_CHR_PLUGIN_DB_SW_SEL_MASK              0x1
#define PMU_RG_CHR_PLUGIN_DB_SW_SEL_SHIFT             8
#define PMU_RG_CHR_PLUGIN_DB_SW_MASK                  0x1
#define PMU_RG_CHR_PLUGIN_DB_SW_SHIFT                 9
#define PMU_RG_CHR_VBUS_UVLO_SW_SEL_MASK              0x1
#define PMU_RG_CHR_VBUS_UVLO_SW_SEL_SHIFT             10
#define PMU_RG_CHR_VBUS_UVLO_SW_MASK                  0x1
#define PMU_RG_CHR_VBUS_UVLO_SW_SHIFT                 11
#define PMU_RG_CHR_VBUS_OVP_SW_SEL_MASK               0x1
#define PMU_RG_CHR_VBUS_OVP_SW_SEL_SHIFT              12
#define PMU_RG_CHR_VBUS_OVP_SW_MASK                   0x1
#define PMU_RG_CHR_VBUS_OVP_SW_SHIFT                  13
#define PMU_RG_CHR_VBUS_GT_POR_SW_SEL_MASK            0x1
#define PMU_RG_CHR_VBUS_GT_POR_SW_SEL_SHIFT           14
#define PMU_RG_CHR_VBUS_GT_POR_SW_MASK                0x1
#define PMU_RG_CHR_VBUS_GT_POR_SW_SHIFT               15
#define PMU_DA_QI_VBUS_UVLO_DB_MASK                   0x1
#define PMU_DA_QI_VBUS_UVLO_DB_SHIFT                  0
#define PMU_DA_QI_VBUS_OVP_DB_MASK                    0x1
#define PMU_DA_QI_VBUS_OVP_DB_SHIFT                   1
#define PMU_DA_QI_CHR_REF_EN_MASK                     0x1
#define PMU_DA_QI_CHR_REF_EN_SHIFT                    2
#define PMU_DA_QI_CHR_PLUGIN_DB_MASK                  0x1
#define PMU_DA_QI_CHR_PLUGIN_DB_SHIFT                 3
#define PMU_AD_QI_VBUS_UVLO_MASK                      0x1
#define PMU_AD_QI_VBUS_UVLO_SHIFT                     4
#define PMU_AD_QI_VBUS_OVP_MASK                       0x1
#define PMU_AD_QI_VBUS_OVP_SHIFT                      5
#define PMU_AD_QI_VBUS_GT_POR_MASK                    0x1
#define PMU_AD_QI_VBUS_GT_POR_SHIFT                   6
#define PMU_RG_ENVTEM_D_MASK                          0x1
#define PMU_RG_ENVTEM_D_SHIFT                         0
#define PMU_RG_ENVTEM_EN_MASK                         0x1
#define PMU_RG_ENVTEM_EN_SHIFT                        1
#define PMU_QI_ENVTEM_MASK                            0x1
#define PMU_QI_ENVTEM_SHIFT                           4
#define PMU_RG_CHR_LPSD_DIS_VA18_MASK                 0x1
#define PMU_RG_CHR_LPSD_DIS_VA18_SHIFT                0
#define PMU_RG_CHR_THERMAL_DIS_VA18_MASK              0x1
#define PMU_RG_CHR_THERMAL_DIS_VA18_SHIFT             1
#define PMU_RG_CHR_VA18_EN_MODE_MASK                  0x1
#define PMU_RG_CHR_VA18_EN_MODE_SHIFT                 2
#define PMU_RG_TSTEP_ICL_MASK                         0x3
#define PMU_RG_TSTEP_ICL_SHIFT                        0
#define PMU_RG_ICL_ITH_MASK                           0xF
#define PMU_RG_ICL_ITH_SHIFT                          2
#define PMU_RG_ICC_JW_MASK                            0x7
#define PMU_RG_ICC_JW_SHIFT                           5
#define PMU_RG_ICC_JC_MASK                            0x7
#define PMU_RG_ICC_JC_SHIFT                           8
#define PMU_RG_TSTEP_ICC_MASK                         0x3
#define PMU_RG_TSTEP_ICC_SHIFT                        11
#define PMU_RG_SYSLDO_EN_MASK                         0x1
#define PMU_RG_SYSLDO_EN_SHIFT                        0
#define PMU_RG_T_ITERM_EXT_MASK                       0x3
#define PMU_RG_T_ITERM_EXT_SHIFT                      0
#define PMU_RG_VCV_VTH_MASK                           0xF
#define PMU_RG_VCV_VTH_SHIFT                          0
#define PMU_RG_FORCE_CHR_EN_MASK                      0x1
#define PMU_RG_FORCE_CHR_EN_SHIFT                     0
#define PMU_RG_CHR_AO_RSV0_MASK                       0xF
#define PMU_RG_CHR_AO_RSV0_SHIFT                      0
#define PMU_RG_CHR_AO_RSV1_MASK                       0xF
#define PMU_RG_CHR_AO_RSV1_SHIFT                      4
#define PMU_RG_CHR_AO_RSV2_MASK                       0xF
#define PMU_RG_CHR_AO_RSV2_SHIFT                      8
#define PMU_RG_CHR_AO_RSV3_MASK                       0xF
#define PMU_RG_CHR_AO_RSV3_SHIFT                      12
#define PMU_RG_VC_ICC_RREF_T_PRECC_MASK               0x3
#define PMU_RG_VC_ICC_RREF_T_PRECC_SHIFT              0
#define PMU_RG_VC_ICC_RREF_PRECC_MASK                 0xF
#define PMU_RG_VC_ICC_RREF_PRECC_SHIFT                4
#define PMU_RG_VC_ICC_RFB_PRECC_MASK                  0xF
#define PMU_RG_VC_ICC_RFB_PRECC_SHIFT                 8
#define PMU_RG_VC_ICC_RREF_T_FASTCC_MASK              0x3
#define PMU_RG_VC_ICC_RREF_T_FASTCC_SHIFT             0
#define PMU_RG_VC_ICC_RREF_FASTCC_MASK                0xF
#define PMU_RG_VC_ICC_RREF_FASTCC_SHIFT               4
#define PMU_RG_VC_ICC_RFB_FASTCC_MASK                 0xF
#define PMU_RG_VC_ICC_RFB_FASTCC_SHIFT                8
#define PMU_RG_VC_ITERM_RREF_T_CHG_IRQ_MASK           0x1
#define PMU_RG_VC_ITERM_RREF_T_CHG_IRQ_SHIFT          2
#define PMU_RG_VC_ITERM_RREF_CHG_IRQ_MASK             0xF
#define PMU_RG_VC_ITERM_RREF_CHG_IRQ_SHIFT            4
#define PMU_RG_VC_ITERM_RFB_CHG_IRQ_MASK              0x7
#define PMU_RG_VC_ITERM_RFB_CHG_IRQ_SHIFT             8
#define PMU_RG_VC_ITERM_RREF_T_CHG_EOC_MASK           0x1
#define PMU_RG_VC_ITERM_RREF_T_CHG_EOC_SHIFT          2
#define PMU_RG_VC_ITERM_RREF_CHG_EOC_MASK             0xF
#define PMU_RG_VC_ITERM_RREF_CHG_EOC_SHIFT            4
#define PMU_RG_VC_ITERM_RFB_CHG_EOC_MASK              0x7
#define PMU_RG_VC_ITERM_RFB_CHG_EOC_SHIFT             8
#define PMU_RG_FOFF_BATOC_MASK                        0x1
#define PMU_RG_FOFF_BATOC_SHIFT                       0
#define PMU_RG_FOFF_FONCMP_MASK                       0x1
#define PMU_RG_FOFF_FONCMP_SHIFT                      1
#define PMU_RG_FOFF_PPFET_CTRL_MASK                   0x1
#define PMU_RG_FOFF_PPFET_CTRL_SHIFT                  2
#define PMU_RG_FORCE_PPFET_FASTOFF_MASK               0x1
#define PMU_RG_FORCE_PPFET_FASTOFF_SHIFT              3
#define PMU_RG_FORCE_PPFET_FASTON_MASK                0x1
#define PMU_RG_FORCE_PPFET_FASTON_SHIFT               4
#define PMU_RG_FASTON_TESTMODE_MASK                   0x1
#define PMU_RG_FASTON_TESTMODE_SHIFT                  5
#define PMU_RG_PPFET_CTRL_LP_MODE_MASK                0x1
#define PMU_RG_PPFET_CTRL_LP_MODE_SHIFT               6
#define PMU_RG_EFUSE_OC_MIN_TRIM_MASK                 0xF
#define PMU_RG_EFUSE_OC_MIN_TRIM_SHIFT                0
#define PMU_RG_EFUSE_OC_PEAK_TRIM_MASK                0xF
#define PMU_RG_EFUSE_OC_PEAK_TRIM_SHIFT               4
#define PMU_RG_EFUSE_OC_SLOPE_TRIM_MASK               0xF
#define PMU_RG_EFUSE_OC_SLOPE_TRIM_SHIFT              8
#define PMU_RG_EFUSE_FON_FALL_TRIM_MASK               0x3F
#define PMU_RG_EFUSE_FON_FALL_TRIM_SHIFT              0
#define PMU_RG_EFUSE_FON_RISE_TRIM_MASK               0x3F
#define PMU_RG_EFUSE_FON_RISE_TRIM_SHIFT              8
#define PMU_RG_EFUSE_PREG_OC_LIM_MASK                 0x1
#define PMU_RG_EFUSE_PREG_OC_LIM_SHIFT                0
#define PMU_RG_EFUSE_PREG_VTRIM_MASK                  0x1F
#define PMU_RG_EFUSE_PREG_VTRIM_SHIFT                 1
#define PMU_RG_EFUSE_PREG_TCTRIM_MASK                 0xF
#define PMU_RG_EFUSE_PREG_TCTRIM_SHIFT                8
#define PMU_RG_EFUSE_PPFET_FON_RSV_TRIM_MASK          0xFF
#define PMU_RG_EFUSE_PPFET_FON_RSV_TRIM_SHIFT         0
#define PMU_RG_MODE_SEL_MASK                          0x1
#define PMU_RG_MODE_SEL_SHIFT                         0
#define PMU_RG_BUS_SNS_LOGIC_SEL_MASK                 0x1
#define PMU_RG_BUS_SNS_LOGIC_SEL_SHIFT                4
#define PMU_RG_OCFB_EN_MASK                           0x1
#define PMU_RG_OCFB_EN_SHIFT                          8
#define PMU_RG_RSV_MASK                               0x3
#define PMU_RG_RSV_SHIFT                              0
#define PMU_D2D_CAP_LPSD_MASK                         0x1
#define PMU_D2D_CAP_LPSD_SHIFT                        4
#define PMU_PAD_VAUD18DRV_VOSEL_MASK                  0x1
#define PMU_PAD_VAUD18DRV_VOSEL_SHIFT                 5
#define PMU_PAD_SRCLKEN_MASK                          0x1
#define PMU_PAD_SRCLKEN_SHIFT                         6
#define PMU_D2D_RTC_ALARM_MASK                        0x1
#define PMU_D2D_RTC_ALARM_SHIFT                       7
#define PMU_DA_EOSC_CALI_MASK                         0x1F
#define PMU_DA_EOSC_CALI_SHIFT                        0
#define PMU_RG_OSC_32K_TRIM_EN_MASK                   0x1
#define PMU_RG_OSC_32K_TRIM_EN_SHIFT                  5
#define PMU_RG_OSC_32K_TRIM_RATE_MASK                 0x3
#define PMU_RG_OSC_32K_TRIM_RATE_SHIFT                6
#define PMU_RG_OSC_32K_TRIM_MASK                      0x1F
#define PMU_RG_OSC_32K_TRIM_SHIFT                     0
#define PMU_RG_LDO_VA18_EN_MASK                       0x1
#define PMU_RG_LDO_VA18_EN_SHIFT                      0
#define PMU_RG_LDO_VA18_LP_MASK                       0x1
#define PMU_RG_LDO_VA18_LP_SHIFT                      1
#define PMU_RG_LDO_VA18_ON_MODE_MASK                  0x1
#define PMU_RG_LDO_VA18_ON_MODE_SHIFT                 0
#define PMU_RG_LDO_VA18_LP_MODE_MASK                  0x1
#define PMU_RG_LDO_VA18_LP_MODE_SHIFT                 1
#define PMU_RG_LDO_ACT_VA18_EN_MASK                   0x1
#define PMU_RG_LDO_ACT_VA18_EN_SHIFT                  0
#define PMU_RG_LDO_SLP_VA18_EN_MASK                   0x1
#define PMU_RG_LDO_SLP_VA18_EN_SHIFT                  1
#define PMU_RG_LDO_ACT_VA18_LP_MASK                   0x1
#define PMU_RG_LDO_ACT_VA18_LP_SHIFT                  0
#define PMU_RG_LDO_SLP_VA18_LP_MASK                   0x1
#define PMU_RG_LDO_SLP_VA18_LP_SHIFT                  1
#define PMU_DA_QI_VA18_MODE_MASK                      0x1
#define PMU_DA_QI_VA18_MODE_SHIFT                     8
#define PMU_RG_LDO_VA18_STBTD_MASK                    0x3
#define PMU_RG_LDO_VA18_STBTD_SHIFT                   9
#define PMU_DA_QI_VA18_STB_MASK                       0x1
#define PMU_DA_QI_VA18_STB_SHIFT                      14
#define PMU_DA_QI_VA18_EN_MASK                        0x1
#define PMU_DA_QI_VA18_EN_SHIFT                       15
#define PMU_RG_LDO_VA18_AUXADC_PWDB_EN_MASK           0x1
#define PMU_RG_LDO_VA18_AUXADC_PWDB_EN_SHIFT          0
#define PMU_RG_LDO_VLDO33_EN_MASK                     0x1
#define PMU_RG_LDO_VLDO33_EN_SHIFT                    0
#define PMU_RG_LDO_VLDO33_LP_MASK                     0x1
#define PMU_RG_LDO_VLDO33_LP_SHIFT                    1
#define PMU_RG_LDO_VLDO33_ON_MODE_MASK                0x1
#define PMU_RG_LDO_VLDO33_ON_MODE_SHIFT               0
#define PMU_RG_LDO_VLDO33_LP_MODE_MASK                0x1
#define PMU_RG_LDO_VLDO33_LP_MODE_SHIFT               1
#define PMU_RG_LDO_ACT_VLDO33_EN_MASK                 0x1
#define PMU_RG_LDO_ACT_VLDO33_EN_SHIFT                0
#define PMU_RG_LDO_SLP_VLDO33_EN_MASK                 0x1
#define PMU_RG_LDO_SLP_VLDO33_EN_SHIFT                1
#define PMU_RG_LDO_ACT_VLDO33_LP_MASK                 0x1
#define PMU_RG_LDO_ACT_VLDO33_LP_SHIFT                0
#define PMU_RG_LDO_SLP_VLDO33_LP_MASK                 0x1
#define PMU_RG_LDO_SLP_VLDO33_LP_SHIFT                1
#define PMU_DA_QI_VLDO33_MODE_MASK                    0x1
#define PMU_DA_QI_VLDO33_MODE_SHIFT                   8
#define PMU_RG_LDO_VLDO33_STBTD_MASK                  0x3
#define PMU_RG_LDO_VLDO33_STBTD_SHIFT                 9
#define PMU_DA_QI_VLDO33_STB_MASK                     0x1
#define PMU_DA_QI_VLDO33_STB_SHIFT                    14
#define PMU_DA_QI_VLDO33_EN_MASK                      0x1
#define PMU_DA_QI_VLDO33_EN_SHIFT                     15
#define PMU_RG_LDO_VLDO33_OCFB_EN_MASK                0x1
#define PMU_RG_LDO_VLDO33_OCFB_EN_SHIFT               9
#define PMU_DA_QI_VLDO33_OCFB_EN_MASK                 0x1
#define PMU_DA_QI_VLDO33_OCFB_EN_SHIFT                10
#define PMU_LDO_DEGTD_SEL_MASK                        0x3
#define PMU_LDO_DEGTD_SEL_SHIFT                       14
#define PMU_RG_LDO_LP_PROT_DISABLE_MASK               0x1
#define PMU_RG_LDO_LP_PROT_DISABLE_SHIFT              0
#define PMU_RG_LDO_AO_RSV0_MASK                       0xFFFF
#define PMU_RG_LDO_AO_RSV0_SHIFT                      0
#define PMU_RG_LDO_AO_RSV1_MASK                       0xFFFF
#define PMU_RG_LDO_AO_RSV1_SHIFT                      0
#define PMU_RG_LDO_AO_EFUSE_RSVED0_MASK               0xFF
#define PMU_RG_LDO_AO_EFUSE_RSVED0_SHIFT              0
#define PMU_RG_LDO_AO_EFUSE_RSVED1_MASK               0xFF
#define PMU_RG_LDO_AO_EFUSE_RSVED1_SHIFT              0
#define PMU_RG_LDO_DCM_MODE_MASK                      0x1
#define PMU_RG_LDO_DCM_MODE_SHIFT                     0
#define PMU_RG_LDO_VA18_CK_SW_MODE_MASK               0x1
#define PMU_RG_LDO_VA18_CK_SW_MODE_SHIFT              0
#define PMU_RG_LDO_VA18_CK_SW_EN_MASK                 0x1
#define PMU_RG_LDO_VA18_CK_SW_EN_SHIFT                1
#define PMU_RG_LDO_VA18_OC_CK_SW_MODE_MASK            0x1
#define PMU_RG_LDO_VA18_OC_CK_SW_MODE_SHIFT           0
#define PMU_RG_LDO_VA18_OC_CK_SW_EN_MASK              0x1
#define PMU_RG_LDO_VA18_OC_CK_SW_EN_SHIFT             1
#define PMU_RG_LDO_VLDO33_CK_SW_MODE_MASK             0x1
#define PMU_RG_LDO_VLDO33_CK_SW_MODE_SHIFT            0
#define PMU_RG_LDO_VLDO33_CK_SW_EN_MASK               0x1
#define PMU_RG_LDO_VLDO33_CK_SW_EN_SHIFT              1
#define PMU_RG_LDO_VLDO33_OC_CK_SW_MODE_MASK          0x1
#define PMU_RG_LDO_VLDO33_OC_CK_SW_MODE_SHIFT         0
#define PMU_RG_LDO_VLDO33_OC_CK_SW_EN_MASK            0x1
#define PMU_RG_LDO_VLDO33_OC_CK_SW_EN_SHIFT           1
#define PMU_RG_LDO_VRF11_CK_SW_MODE_MASK              0x1
#define PMU_RG_LDO_VRF11_CK_SW_MODE_SHIFT             0
#define PMU_RG_LDO_VRF11_CK_SW_EN_MASK                0x1
#define PMU_RG_LDO_VRF11_CK_SW_EN_SHIFT               1
#define PMU_RG_LDO_VRF11_OC_CK_SW_MODE_MASK           0x1
#define PMU_RG_LDO_VRF11_OC_CK_SW_MODE_SHIFT          0
#define PMU_RG_LDO_VRF11_OC_CK_SW_EN_MASK             0x1
#define PMU_RG_LDO_VRF11_OC_CK_SW_EN_SHIFT            1
#define PMU_RG_LDO_VSRAM_CK_SW_MODE_MASK              0x1
#define PMU_RG_LDO_VSRAM_CK_SW_MODE_SHIFT             0
#define PMU_RG_LDO_VSRAM_CK_SW_EN_MASK                0x1
#define PMU_RG_LDO_VSRAM_CK_SW_EN_SHIFT               1
#define PMU_RG_LDO_VSRAM_OC_CK_SW_MODE_MASK           0x1
#define PMU_RG_LDO_VSRAM_OC_CK_SW_MODE_SHIFT          0
#define PMU_RG_LDO_VSRAM_OC_CK_SW_EN_MASK             0x1
#define PMU_RG_LDO_VSRAM_OC_CK_SW_EN_SHIFT            1
#define PMU_VBAT_DOWN_DETECT_STATUS_CLR_MASK          0x1
#define PMU_VBAT_DOWN_DETECT_STATUS_CLR_SHIFT         4
#define PMU_VBAT_DOWN_DETECT_STATUS_MASK              0x1
#define PMU_VBAT_DOWN_DETECT_STATUS_SHIFT             5
#define PMU_VBAT_UP_DETECT_STATUS_CLR_MASK            0x1
#define PMU_VBAT_UP_DETECT_STATUS_CLR_SHIFT           6
#define PMU_VBAT_UP_DETECT_STATUS_MASK                0x1
#define PMU_VBAT_UP_DETECT_STATUS_SHIFT               7
#define PMU_VBST_I2_EN_STATUS_CLR_MASK                0x1
#define PMU_VBST_I2_EN_STATUS_CLR_SHIFT               10
#define PMU_VBST_I2_EN_STATUS_MASK                    0x1
#define PMU_VBST_I2_EN_STATUS_SHIFT                   11
#define PMU_UDSH_PROTECT_STATUS_CLR_MASK              0x1
#define PMU_UDSH_PROTECT_STATUS_CLR_SHIFT             12
#define PMU_UDSH_PROTECT_STATUS_MASK                  0x1
#define PMU_UDSH_PROTECT_STATUS_SHIFT                 13
#define PMU_BAT_RISING_FLAG_STATUS_CLR_MASK           0x1
#define PMU_BAT_RISING_FLAG_STATUS_CLR_SHIFT          14
#define PMU_BAT_RISING_FLAG_STATUS_MASK               0x1
#define PMU_BAT_RISING_FLAG_STATUS_SHIFT              15
#define PMU_RG_LDO_VRF11_EN_MASK                      0x1
#define PMU_RG_LDO_VRF11_EN_SHIFT                     0
#define PMU_RG_LDO_VRF11_LP_MASK                      0x1
#define PMU_RG_LDO_VRF11_LP_SHIFT                     1
#define PMU_RG_LDO_VRF11_ON_MODE_MASK                 0x1
#define PMU_RG_LDO_VRF11_ON_MODE_SHIFT                0
#define PMU_RG_LDO_VRF11_LP_MODE_MASK                 0x1
#define PMU_RG_LDO_VRF11_LP_MODE_SHIFT                1
#define PMU_RG_LDO_ACT_VRF11_EN_MASK                  0x1
#define PMU_RG_LDO_ACT_VRF11_EN_SHIFT                 0
#define PMU_RG_LDO_SLP_VRF11_EN_MASK                  0x1
#define PMU_RG_LDO_SLP_VRF11_EN_SHIFT                 1
#define PMU_RG_LDO_ACT_VRF11_LP_MASK                  0x1
#define PMU_RG_LDO_ACT_VRF11_LP_SHIFT                 0
#define PMU_RG_LDO_SLP_VRF11_LP_MASK                  0x1
#define PMU_RG_LDO_SLP_VRF11_LP_SHIFT                 1
#define PMU_DA_QI_VRF11_MODE_MASK                     0x1
#define PMU_DA_QI_VRF11_MODE_SHIFT                    8
#define PMU_RG_LDO_VRF11_STBTD_MASK                   0x3
#define PMU_RG_LDO_VRF11_STBTD_SHIFT                  9
#define PMU_DA_QI_VRF11_STB_MASK                      0x1
#define PMU_DA_QI_VRF11_STB_SHIFT                     14
#define PMU_DA_QI_VRF11_EN_MASK                       0x1
#define PMU_DA_QI_VRF11_EN_SHIFT                      15
#define PMU_RG_LDO_VRF11_OCFB_EN_MASK                 0x1
#define PMU_RG_LDO_VRF11_OCFB_EN_SHIFT                9
#define PMU_DA_QI_VRF11_OCFB_EN_MASK                  0x1
#define PMU_DA_QI_VRF11_OCFB_EN_SHIFT                 10
#define PMU_RG_LDO_VSRAM_EN_MASK                      0x1
#define PMU_RG_LDO_VSRAM_EN_SHIFT                     0
#define PMU_RG_LDO_VSRAM_LP_MASK                      0x1
#define PMU_RG_LDO_VSRAM_LP_SHIFT                     1
#define PMU_RG_LDO_VSRAM_ON_MODE_MASK                 0x1
#define PMU_RG_LDO_VSRAM_ON_MODE_SHIFT                0
#define PMU_RG_LDO_VSRAM_LP_MODE_MASK                 0x1
#define PMU_RG_LDO_VSRAM_LP_MODE_SHIFT                1
#define PMU_RG_LDO_ACT_VSRAM_EN_MASK                  0x1
#define PMU_RG_LDO_ACT_VSRAM_EN_SHIFT                 0
#define PMU_RG_LDO_SLP_VSRAM_EN_MASK                  0x1
#define PMU_RG_LDO_SLP_VSRAM_EN_SHIFT                 1
#define PMU_RG_LDO_ACT_VSRAM_LP_MASK                  0x1
#define PMU_RG_LDO_ACT_VSRAM_LP_SHIFT                 0
#define PMU_RG_LDO_SLP_VSRAM_LP_MASK                  0x1
#define PMU_RG_LDO_SLP_VSRAM_LP_SHIFT                 1
#define PMU_DA_QI_VSRAM_MODE_MASK                     0x1
#define PMU_DA_QI_VSRAM_MODE_SHIFT                    8
#define PMU_RG_LDO_VSRAM_STBTD_MASK                   0x3
#define PMU_RG_LDO_VSRAM_STBTD_SHIFT                  9
#define PMU_DA_QI_VSRAM_STB_MASK                      0x1
#define PMU_DA_QI_VSRAM_STB_SHIFT                     14
#define PMU_DA_QI_VSRAM_EN_MASK                       0x1
#define PMU_DA_QI_VSRAM_EN_SHIFT                      15
#define PMU_RG_LDO_VSRAM_OCFB_EN_MASK                 0x1
#define PMU_RG_LDO_VSRAM_OCFB_EN_SHIFT                9
#define PMU_DA_QI_VSRAM_OCFB_EN_MASK                  0x1
#define PMU_DA_QI_VSRAM_OCFB_EN_SHIFT                 10
#define PMU_RG_VSRAM_VOSEL_MASK                       0x3
#define PMU_RG_VSRAM_VOSEL_SHIFT                      0
#define PMU_RG_VSRAM_VOSEL_SLEEP_MASK                 0x3
#define PMU_RG_VSRAM_VOSEL_SLEEP_SHIFT                4
#define PMU_RG_VSRAM_VOSEL_SW_MODE_MASK               0x1
#define PMU_RG_VSRAM_VOSEL_SW_MODE_SHIFT              8
#define PMU_RG_VSRAM_VOSEL_SW_MASK                    0xF
#define PMU_RG_VSRAM_VOSEL_SW_SHIFT                   12
#define PMU_RG_PSW_VCORE_SRAM_ON_MODE_MASK            0x1
#define PMU_RG_PSW_VCORE_SRAM_ON_MODE_SHIFT           0
#define PMU_RG_PSW_VCORE_SRAM_EN_MASK                 0x1
#define PMU_RG_PSW_VCORE_SRAM_EN_SHIFT                1
#define PMU_RG_PSW_VCORE_SRAM_ACT_EN_MASK             0x1
#define PMU_RG_PSW_VCORE_SRAM_ACT_EN_SHIFT            0
#define PMU_RG_PSW_VCORE_SRAM_SLP_EN_MASK             0x1
#define PMU_RG_PSW_VCORE_SRAM_SLP_EN_SHIFT            1
#define PMU_RG_SRAM_NDIS_SW_EN_MASK                   0x1
#define PMU_RG_SRAM_NDIS_SW_EN_SHIFT                  0
#define PMU_RG_VDIG18_RSV_MASK                        0x1
#define PMU_RG_VDIG18_RSV_SHIFT                       10
#define PMU_RG_VDIG18_OC_TRIM_EN_MASK                 0x1
#define PMU_RG_VDIG18_OC_TRIM_EN_SHIFT                11
#define PMU_RG_VDIG18_IBIAS_SELECT_MASK               0x3
#define PMU_RG_VDIG18_IBIAS_SELECT_SHIFT              12
#define PMU_RG_VLDO33_VOSEL_MASK                      0xF
#define PMU_RG_VLDO33_VOSEL_SHIFT                     8
#define PMU_RG_VLDO33_NDIS_EN_MASK                    0x1
#define PMU_RG_VLDO33_NDIS_EN_SHIFT                   0
#define PMU_RG_VLDO33_RSV_MASK                        0x3
#define PMU_RG_VLDO33_RSV_SHIFT                       1
#define PMU_RG_VLDO33_OC_DETECT_FORCE_EN_MASK         0x1
#define PMU_RG_VLDO33_OC_DETECT_FORCE_EN_SHIFT        3
#define PMU_RG_VLDO33_PG_DETECT_FORCE_ON_MASK         0x1
#define PMU_RG_VLDO33_PG_DETECT_FORCE_ON_SHIFT        4
#define PMU_RG_VLDO33_CLAMP_FORCE_EN_MASK             0x1
#define PMU_RG_VLDO33_CLAMP_FORCE_EN_SHIFT            5
#define PMU_RG_VLDO33_LEAKCOMPEN_EN_MASK              0x1
#define PMU_RG_VLDO33_LEAKCOMPEN_EN_SHIFT             6
#define PMU_RG_VLDO33_IBIAS_SELECT_MASK               0x3
#define PMU_RG_VLDO33_IBIAS_SELECT_SHIFT              7
#define PMU_RG_VLDO33_IBIAS_PN_TM_MASK                0x1
#define PMU_RG_VLDO33_IBIAS_PN_TM_SHIFT               9
#define PMU_RG_VLDO33_I2BST_EN_MASK                   0x1
#define PMU_RG_VLDO33_I2BST_EN_SHIFT                  10
#define PMU_RGS_VLDO33_NMOS_LEAK_GD_STATUS_MASK       0x1
#define PMU_RGS_VLDO33_NMOS_LEAK_GD_STATUS_SHIFT      11
#define PMU_RGS_VLDO33_OC_DETECT_EN_STATUS_MASK       0x1
#define PMU_RGS_VLDO33_OC_DETECT_EN_STATUS_SHIFT      12
#define PMU_RG_VA18_NDIS_EN_MASK                      0x1
#define PMU_RG_VA18_NDIS_EN_SHIFT                     13
#define PMU_RG_VA18_RSV_MASK                          0x3
#define PMU_RG_VA18_RSV_SHIFT                         14
#define PMU_RG_VA18_IBIAS_SELECT_MASK                 0x3
#define PMU_RG_VA18_IBIAS_SELECT_SHIFT                0
#define PMU_RGS_VA18_OC_STATUS_MASK                   0x1
#define PMU_RGS_VA18_OC_STATUS_SHIFT                  2
#define PMU_RG_VDIG18_VOTRIM_MASK                     0xF
#define PMU_RG_VDIG18_VOTRIM_SHIFT                    0
#define PMU_RG_VDIG18_OC_TRIM_MASK                    0xF
#define PMU_RG_VDIG18_OC_TRIM_SHIFT                   0
#define PMU_RG_VLDO33_VOTRIM_MASK                     0xF
#define PMU_RG_VLDO33_VOTRIM_SHIFT                    0
#define PMU_RG_VA18_VOTRIM_MASK                       0xF
#define PMU_RG_VA18_VOTRIM_SHIFT                      0
#define PMU_RG_VRF11_VOSEL_MASK                       0xF
#define PMU_RG_VRF11_VOSEL_SHIFT                      8
#define PMU_RG_VRF11_NDIS_EN_MASK                     0x1
#define PMU_RG_VRF11_NDIS_EN_SHIFT                    0
#define PMU_RG_VRF11_OC_DETECT_FORCE_EN_MASK          0x1
#define PMU_RG_VRF11_OC_DETECT_FORCE_EN_SHIFT         3
#define PMU_RG_VRF11_PG_DETECT_FORCE_ON_MASK          0x1
#define PMU_RG_VRF11_PG_DETECT_FORCE_ON_SHIFT         4
#define PMU_RG_VRF11_CLAMP_FORCE_EN_MASK              0x1
#define PMU_RG_VRF11_CLAMP_FORCE_EN_SHIFT             5
#define PMU_RG_VRF11_LEAKCOMPEN_EN_MASK               0x1
#define PMU_RG_VRF11_LEAKCOMPEN_EN_SHIFT              6
#define PMU_RG_VRF11_IBIAS_SELECT_MASK                0x3
#define PMU_RG_VRF11_IBIAS_SELECT_SHIFT               7
#define PMU_RG_VRF11_IBIAS_PN_TM_MASK                 0x1
#define PMU_RG_VRF11_IBIAS_PN_TM_SHIFT                9
#define PMU_RG_VRF11_I2BST_EN_MASK                    0x1
#define PMU_RG_VRF11_I2BST_EN_SHIFT                   10
#define PMU_RGS_VRF11_NMOS_LEAK_GD_STATUS_MASK        0x1
#define PMU_RGS_VRF11_NMOS_LEAK_GD_STATUS_SHIFT       11
#define PMU_RGS_VRF11_OC_DETECT_EN_STATUS_MASK        0x1
#define PMU_RGS_VRF11_OC_DETECT_EN_STATUS_SHIFT       12
#define PMU_RG_VRF11_OC_TRIM_EN_MASK                  0x1
#define PMU_RG_VRF11_OC_TRIM_EN_SHIFT                 13
#define PMU_RG_VSRAM_OC_DETECT_FORCE_EN_MASK          0x1
#define PMU_RG_VSRAM_OC_DETECT_FORCE_EN_SHIFT         3
#define PMU_RG_VSRAM_PG_DETECT_FORCE_ON_MASK          0x1
#define PMU_RG_VSRAM_PG_DETECT_FORCE_ON_SHIFT         4
#define PMU_RG_VSRAM_LEAKCOMPEN_EN_MASK               0x1
#define PMU_RG_VSRAM_LEAKCOMPEN_EN_SHIFT              6
#define PMU_RG_VSRAM_IBIAS_SELECT_MASK                0x3
#define PMU_RG_VSRAM_IBIAS_SELECT_SHIFT               7
#define PMU_RG_VSRAM_IBIAS_PN_TM_MASK                 0x1
#define PMU_RG_VSRAM_IBIAS_PN_TM_SHIFT                9
#define PMU_RG_VSRAM_I2BST_EN_MASK                    0x1
#define PMU_RG_VSRAM_I2BST_EN_SHIFT                   10
#define PMU_RGS_VSRAM_NMOS_LEAK_GD_STATUS_MASK        0x1
#define PMU_RGS_VSRAM_NMOS_LEAK_GD_STATUS_SHIFT       11
#define PMU_RGS_VSRAM_OC_DETECT_EN_STATUS_MASK        0x1
#define PMU_RGS_VSRAM_OC_DETECT_EN_STATUS_SHIFT       12
#define PMU_RG_VSRAM_OC_TRIM_EN_MASK                  0x1
#define PMU_RG_VSRAM_OC_TRIM_EN_SHIFT                 13
#define PMU_RG_VSRAM_RSV_MASK                         0x3F
#define PMU_RG_VSRAM_RSV_SHIFT                        0
#define PMU_RG_VRF11_RSV_MASK                         0x3F
#define PMU_RG_VRF11_RSV_SHIFT                        8
#define PMU_RG_VRF11_VOTRIM_MASK                      0x1F
#define PMU_RG_VRF11_VOTRIM_SHIFT                     0
#define PMU_RG_VSRAM_VOTRIM_MASK                      0x1F
#define PMU_RG_VSRAM_VOTRIM_SHIFT                     0
#define PMU_RG_BUCK_VCORE_EN_MASK                     0x1
#define PMU_RG_BUCK_VCORE_EN_SHIFT                    0
#define PMU_RG_BUCK_VCORE_LP_MASK                     0x1
#define PMU_RG_BUCK_VCORE_LP_SHIFT                    1
#define PMU_RG_BUCK_VCORE_VOSEL_SLEEP_MASK            0x7F
#define PMU_RG_BUCK_VCORE_VOSEL_SLEEP_SHIFT           0
#define PMU_RG_BUCK_VCORE_SFCHG_FRATE_MASK            0x7F
#define PMU_RG_BUCK_VCORE_SFCHG_FRATE_SHIFT           0
#define PMU_RG_BUCK_VCORE_SFCHG_FEN_MASK              0x1
#define PMU_RG_BUCK_VCORE_SFCHG_FEN_SHIFT             7
#define PMU_RG_BUCK_VCORE_SFCHG_RRATE_MASK            0x7F
#define PMU_RG_BUCK_VCORE_SFCHG_RRATE_SHIFT           8
#define PMU_RG_BUCK_VCORE_SFCHG_REN_MASK              0x1
#define PMU_RG_BUCK_VCORE_SFCHG_REN_SHIFT             15
#define PMU_RG_BUCK_VCORE_DVS_EN_TD_MASK              0x3
#define PMU_RG_BUCK_VCORE_DVS_EN_TD_SHIFT             0
#define PMU_RG_BUCK_VCORE_DVS_EN_CTRL_MASK            0x3
#define PMU_RG_BUCK_VCORE_DVS_EN_CTRL_SHIFT           4
#define PMU_RG_BUCK_VCORE_DVS_EN_ONCE_MASK            0x1
#define PMU_RG_BUCK_VCORE_DVS_EN_ONCE_SHIFT           6
#define PMU_RG_BUCK_VCORE_ON_MODE_MASK                0x1
#define PMU_RG_BUCK_VCORE_ON_MODE_SHIFT               0
#define PMU_RG_BUCK_VCORE_LP_MODE_MASK                0x1
#define PMU_RG_BUCK_VCORE_LP_MODE_SHIFT               1
#define PMU_RG_BUCK_ACT_VCORE_EN_MASK                 0x1
#define PMU_RG_BUCK_ACT_VCORE_EN_SHIFT                0
#define PMU_RG_BUCK_SLP_VCORE_EN_MASK                 0x1
#define PMU_RG_BUCK_SLP_VCORE_EN_SHIFT                1
#define PMU_RG_BUCK_ACT_VCORE_LP_MASK                 0x1
#define PMU_RG_BUCK_ACT_VCORE_LP_SHIFT                0
#define PMU_RG_BUCK_SLP_VCORE_LP_MASK                 0x1
#define PMU_RG_BUCK_SLP_VCORE_LP_SHIFT                1
#define PMU_RG_BUCK_VCORE_OC_DEG_EN_MASK              0x1
#define PMU_RG_BUCK_VCORE_OC_DEG_EN_SHIFT             1
#define PMU_RG_BUCK_VCORE_OC_WND_MASK                 0x3
#define PMU_RG_BUCK_VCORE_OC_WND_SHIFT                2
#define PMU_RG_BUCK_VCORE_OC_THD_MASK                 0x3
#define PMU_RG_BUCK_VCORE_OC_THD_SHIFT                6
#define PMU_DA_VCORE_VOSEL_MASK                       0x7F
#define PMU_DA_VCORE_VOSEL_SHIFT                      0
#define PMU_DA_NI_VCORE_VOSEL_GRAY_MASK               0x7F
#define PMU_DA_NI_VCORE_VOSEL_GRAY_SHIFT              8
#define PMU_DA_QI_VCORE_EN_MASK                       0x1
#define PMU_DA_QI_VCORE_EN_SHIFT                      0
#define PMU_DA_QI_VCORE_VSLEEP_SEL_MASK               0x1
#define PMU_DA_QI_VCORE_VSLEEP_SEL_SHIFT              2
#define PMU_DA_NI_VCORE_R2R_PDN_MASK                  0x1
#define PMU_DA_NI_VCORE_R2R_PDN_SHIFT                 3
#define PMU_DA_NI_VCORE_DVS_EN_MASK                   0x1
#define PMU_DA_NI_VCORE_DVS_EN_SHIFT                  4
#define PMU_DA_QI_VCORE_SSH_MASK                      0x1
#define PMU_DA_QI_VCORE_SSH_SHIFT                     6
#define PMU_RG_BUCK_VCORE_OC_FLAG_CLR_SEL_MASK        0x1
#define PMU_RG_BUCK_VCORE_OC_FLAG_CLR_SEL_SHIFT       4
#define PMU_RG_BUCK_VCORE_CK_SW_MODE_MASK             0x1
#define PMU_RG_BUCK_VCORE_CK_SW_MODE_SHIFT            6
#define PMU_RG_BUCK_VCORE_CK_SW_EN_MASK               0x1
#define PMU_RG_BUCK_VCORE_CK_SW_EN_SHIFT              7
#define PMU_RG_BUCK_VCORE_VOSEL_MASK                  0x7F
#define PMU_RG_BUCK_VCORE_VOSEL_SHIFT                 0
#define PMU_RG_BUCK_VIO18_EN_MASK                     0x1
#define PMU_RG_BUCK_VIO18_EN_SHIFT                    0
#define PMU_RG_BUCK_VIO18_LP_MASK                     0x1
#define PMU_RG_BUCK_VIO18_LP_SHIFT                    1
#define PMU_RG_BUCK_VIO18_VOSEL_SLEEP_MASK            0x7F
#define PMU_RG_BUCK_VIO18_VOSEL_SLEEP_SHIFT           0
#define PMU_RG_BUCK_VIO18_SFCHG_FRATE_MASK            0x7F
#define PMU_RG_BUCK_VIO18_SFCHG_FRATE_SHIFT           0
#define PMU_RG_BUCK_VIO18_SFCHG_FEN_MASK              0x1
#define PMU_RG_BUCK_VIO18_SFCHG_FEN_SHIFT             7
#define PMU_RG_BUCK_VIO18_SFCHG_RRATE_MASK            0x7F
#define PMU_RG_BUCK_VIO18_SFCHG_RRATE_SHIFT           8
#define PMU_RG_BUCK_VIO18_SFCHG_REN_MASK              0x1
#define PMU_RG_BUCK_VIO18_SFCHG_REN_SHIFT             15
#define PMU_RG_BUCK_VIO18_DVS_EN_TD_MASK              0x3
#define PMU_RG_BUCK_VIO18_DVS_EN_TD_SHIFT             0
#define PMU_RG_BUCK_VIO18_DVS_EN_CTRL_MASK            0x3
#define PMU_RG_BUCK_VIO18_DVS_EN_CTRL_SHIFT           4
#define PMU_RG_BUCK_VIO18_DVS_EN_ONCE_MASK            0x1
#define PMU_RG_BUCK_VIO18_DVS_EN_ONCE_SHIFT           6
#define PMU_RG_BUCK_VIO18_ON_MODE_MASK                0x1
#define PMU_RG_BUCK_VIO18_ON_MODE_SHIFT               0
#define PMU_RG_BUCK_VIO18_LP_MODE_MASK                0x1
#define PMU_RG_BUCK_VIO18_LP_MODE_SHIFT               1
#define PMU_RG_BUCK_ACT_VIO18_EN_MASK                 0x1
#define PMU_RG_BUCK_ACT_VIO18_EN_SHIFT                0
#define PMU_RG_BUCK_SLP_VIO18_EN_MASK                 0x1
#define PMU_RG_BUCK_SLP_VIO18_EN_SHIFT                1
#define PMU_RG_BUCK_ACT_VIO18_LP_MASK                 0x1
#define PMU_RG_BUCK_ACT_VIO18_LP_SHIFT                0
#define PMU_RG_BUCK_SLP_VIO18_LP_MASK                 0x1
#define PMU_RG_BUCK_SLP_VIO18_LP_SHIFT                1
#define PMU_RG_BUCK_VIO18_OC_DEG_EN_MASK              0x1
#define PMU_RG_BUCK_VIO18_OC_DEG_EN_SHIFT             1
#define PMU_RG_BUCK_VIO18_OC_WND_MASK                 0x3
#define PMU_RG_BUCK_VIO18_OC_WND_SHIFT                2
#define PMU_RG_BUCK_VIO18_OC_THD_MASK                 0x3
#define PMU_RG_BUCK_VIO18_OC_THD_SHIFT                6
#define PMU_DA_VIO18_VOSEL_MASK                       0x7F
#define PMU_DA_VIO18_VOSEL_SHIFT                      0
#define PMU_DA_NI_VIO18_VOSEL_GRAY_MASK               0x7F
#define PMU_DA_NI_VIO18_VOSEL_GRAY_SHIFT              8
#define PMU_DA_QI_VIO18_EN_MASK                       0x1
#define PMU_DA_QI_VIO18_EN_SHIFT                      0
#define PMU_DA_QI_VIO18_VSLEEP_SEL_MASK               0x1
#define PMU_DA_QI_VIO18_VSLEEP_SEL_SHIFT              2
#define PMU_DA_NI_VIO18_R2R_PDN_MASK                  0x1
#define PMU_DA_NI_VIO18_R2R_PDN_SHIFT                 3
#define PMU_DA_NI_VIO18_DVS_EN_MASK                   0x1
#define PMU_DA_NI_VIO18_DVS_EN_SHIFT                  4
#define PMU_DA_QI_VIO18_SSH_MASK                      0x1
#define PMU_DA_QI_VIO18_SSH_SHIFT                     6
#define PMU_RG_BUCK_VIO18_OC_FLAG_CLR_SEL_MASK        0x1
#define PMU_RG_BUCK_VIO18_OC_FLAG_CLR_SEL_SHIFT       4
#define PMU_RG_BUCK_VIO18_CK_SW_MODE_MASK             0x1
#define PMU_RG_BUCK_VIO18_CK_SW_MODE_SHIFT            6
#define PMU_RG_BUCK_VIO18_CK_SW_EN_MASK               0x1
#define PMU_RG_BUCK_VIO18_CK_SW_EN_SHIFT              7
#define PMU_RG_BUCK_VIO18_VOSEL_MASK                  0x7F
#define PMU_RG_BUCK_VIO18_VOSEL_SHIFT                 0
#define PMU_RG_BUCK_VRF_EN_MASK                       0x1
#define PMU_RG_BUCK_VRF_EN_SHIFT                      0
#define PMU_RG_BUCK_VRF_LP_MASK                       0x1
#define PMU_RG_BUCK_VRF_LP_SHIFT                      1
#define PMU_RG_BUCK_VRF_VOSEL_SLEEP_MASK              0x7F
#define PMU_RG_BUCK_VRF_VOSEL_SLEEP_SHIFT             0
#define PMU_RG_BUCK_VRF_SFCHG_FRATE_MASK              0x7F
#define PMU_RG_BUCK_VRF_SFCHG_FRATE_SHIFT             0
#define PMU_RG_BUCK_VRF_SFCHG_FEN_MASK                0x1
#define PMU_RG_BUCK_VRF_SFCHG_FEN_SHIFT               7
#define PMU_RG_BUCK_VRF_SFCHG_RRATE_MASK              0x7F
#define PMU_RG_BUCK_VRF_SFCHG_RRATE_SHIFT             8
#define PMU_RG_BUCK_VRF_SFCHG_REN_MASK                0x1
#define PMU_RG_BUCK_VRF_SFCHG_REN_SHIFT               15
#define PMU_RG_BUCK_VRF_DVS_EN_TD_MASK                0x3
#define PMU_RG_BUCK_VRF_DVS_EN_TD_SHIFT               0
#define PMU_RG_BUCK_VRF_DVS_EN_CTRL_MASK              0x3
#define PMU_RG_BUCK_VRF_DVS_EN_CTRL_SHIFT             4
#define PMU_RG_BUCK_VRF_DVS_EN_ONCE_MASK              0x1
#define PMU_RG_BUCK_VRF_DVS_EN_ONCE_SHIFT             6
#define PMU_RG_BUCK_VRF_ON_MODE_MASK                  0x1
#define PMU_RG_BUCK_VRF_ON_MODE_SHIFT                 0
#define PMU_RG_BUCK_VRF_LP_MODE_MASK                  0x1
#define PMU_RG_BUCK_VRF_LP_MODE_SHIFT                 1
#define PMU_RG_BUCK_ACT_VRF_EN_MASK                   0x1
#define PMU_RG_BUCK_ACT_VRF_EN_SHIFT                  0
#define PMU_RG_BUCK_SLP_VRF_EN_MASK                   0x1
#define PMU_RG_BUCK_SLP_VRF_EN_SHIFT                  1
#define PMU_RG_BUCK_ACT_VRF_LP_MASK                   0x1
#define PMU_RG_BUCK_ACT_VRF_LP_SHIFT                  0
#define PMU_RG_BUCK_SLP_VRF_LP_MASK                   0x1
#define PMU_RG_BUCK_SLP_VRF_LP_SHIFT                  1
#define PMU_RG_BUCK_VRF_OC_DEG_EN_MASK                0x1
#define PMU_RG_BUCK_VRF_OC_DEG_EN_SHIFT               1
#define PMU_RG_BUCK_VRF_OC_WND_MASK                   0x3
#define PMU_RG_BUCK_VRF_OC_WND_SHIFT                  2
#define PMU_RG_BUCK_VRF_OC_THD_MASK                   0x3
#define PMU_RG_BUCK_VRF_OC_THD_SHIFT                  6
#define PMU_DA_VRF_VOSEL_MASK                         0x7F
#define PMU_DA_VRF_VOSEL_SHIFT                        0
#define PMU_DA_NI_VRF_VOSEL_GRAY_MASK                 0x7F
#define PMU_DA_NI_VRF_VOSEL_GRAY_SHIFT                8
#define PMU_DA_QI_VRF_EN_MASK                         0x1
#define PMU_DA_QI_VRF_EN_SHIFT                        0
#define PMU_DA_QI_VRF_VSLEEP_SEL_MASK                 0x1
#define PMU_DA_QI_VRF_VSLEEP_SEL_SHIFT                2
#define PMU_DA_NI_VRF_R2R_PDN_MASK                    0x1
#define PMU_DA_NI_VRF_R2R_PDN_SHIFT                   3
#define PMU_DA_NI_VRF_DVS_EN_MASK                     0x1
#define PMU_DA_NI_VRF_DVS_EN_SHIFT                    4
#define PMU_DA_QI_VRF_SSH_MASK                        0x1
#define PMU_DA_QI_VRF_SSH_SHIFT                       6
#define PMU_RG_BUCK_VRF_OC_FLAG_CLR_SEL_MASK          0x1
#define PMU_RG_BUCK_VRF_OC_FLAG_CLR_SEL_SHIFT         4
#define PMU_RG_BUCK_VRF_CK_SW_MODE_MASK               0x1
#define PMU_RG_BUCK_VRF_CK_SW_MODE_SHIFT              6
#define PMU_RG_BUCK_VRF_CK_SW_EN_MASK                 0x1
#define PMU_RG_BUCK_VRF_CK_SW_EN_SHIFT                7
#define PMU_RG_BUCK_VRF_VOSEL_MASK                    0x7F
#define PMU_RG_BUCK_VRF_VOSEL_SHIFT                   0
#define PMU_RG_BUCK_VAUD18_EN_MASK                    0x1
#define PMU_RG_BUCK_VAUD18_EN_SHIFT                   0
#define PMU_RG_BUCK_VAUD18_LP_MASK                    0x1
#define PMU_RG_BUCK_VAUD18_LP_SHIFT                   1
#define PMU_RG_BUCK_VAUD18_VOSEL_SLEEP_MASK           0x7F
#define PMU_RG_BUCK_VAUD18_VOSEL_SLEEP_SHIFT          0
#define PMU_RG_BUCK_VAUD18_SFCHG_FRATE_MASK           0x7F
#define PMU_RG_BUCK_VAUD18_SFCHG_FRATE_SHIFT          0
#define PMU_RG_BUCK_VAUD18_SFCHG_FEN_MASK             0x1
#define PMU_RG_BUCK_VAUD18_SFCHG_FEN_SHIFT            7
#define PMU_RG_BUCK_VAUD18_SFCHG_RRATE_MASK           0x7F
#define PMU_RG_BUCK_VAUD18_SFCHG_RRATE_SHIFT          8
#define PMU_RG_BUCK_VAUD18_SFCHG_REN_MASK             0x1
#define PMU_RG_BUCK_VAUD18_SFCHG_REN_SHIFT            15
#define PMU_RG_BUCK_VAUD18_DVS_EN_TD_MASK             0x3
#define PMU_RG_BUCK_VAUD18_DVS_EN_TD_SHIFT            0
#define PMU_RG_BUCK_VAUD18_DVS_EN_CTRL_MASK           0x3
#define PMU_RG_BUCK_VAUD18_DVS_EN_CTRL_SHIFT          4
#define PMU_RG_BUCK_VAUD18_DVS_EN_ONCE_MASK           0x1
#define PMU_RG_BUCK_VAUD18_DVS_EN_ONCE_SHIFT          6
#define PMU_RG_BUCK_VAUD18_ON_MODE_MASK               0x1
#define PMU_RG_BUCK_VAUD18_ON_MODE_SHIFT              0
#define PMU_RG_BUCK_VAUD18_LP_MODE_MASK               0x1
#define PMU_RG_BUCK_VAUD18_LP_MODE_SHIFT              1
#define PMU_RG_BUCK_ACT_VAUD18_EN_MASK                0x1
#define PMU_RG_BUCK_ACT_VAUD18_EN_SHIFT               0
#define PMU_RG_BUCK_SLP_VAUD18_EN_MASK                0x1
#define PMU_RG_BUCK_SLP_VAUD18_EN_SHIFT               1
#define PMU_RG_BUCK_ACT_VAUD18_LP_MASK                0x1
#define PMU_RG_BUCK_ACT_VAUD18_LP_SHIFT               0
#define PMU_RG_BUCK_SLP_VAUD18_LP_MASK                0x1
#define PMU_RG_BUCK_SLP_VAUD18_LP_SHIFT               1
#define PMU_RG_BUCK_VAUD18_OC_DEG_EN_MASK             0x1
#define PMU_RG_BUCK_VAUD18_OC_DEG_EN_SHIFT            1
#define PMU_RG_BUCK_VAUD18_OC_WND_MASK                0x3
#define PMU_RG_BUCK_VAUD18_OC_WND_SHIFT               2
#define PMU_RG_BUCK_VAUD18_OC_THD_MASK                0x3
#define PMU_RG_BUCK_VAUD18_OC_THD_SHIFT               6
#define PMU_DA_VAUD18_VOSEL_MASK                      0x7F
#define PMU_DA_VAUD18_VOSEL_SHIFT                     0
#define PMU_DA_NI_VAUD18_VOSEL_GRAY_MASK              0x7F
#define PMU_DA_NI_VAUD18_VOSEL_GRAY_SHIFT             8
#define PMU_DA_QI_VAUD18_EN_MASK                      0x1
#define PMU_DA_QI_VAUD18_EN_SHIFT                     0
#define PMU_DA_QI_VAUD18_VSLEEP_SEL_MASK              0x1
#define PMU_DA_QI_VAUD18_VSLEEP_SEL_SHIFT             2
#define PMU_DA_NI_VAUD18_R2R_PDN_MASK                 0x1
#define PMU_DA_NI_VAUD18_R2R_PDN_SHIFT                3
#define PMU_DA_NI_VAUD18_DVS_EN_MASK                  0x1
#define PMU_DA_NI_VAUD18_DVS_EN_SHIFT                 4
#define PMU_DA_QI_VAUD18_SSH_MASK                     0x1
#define PMU_DA_QI_VAUD18_SSH_SHIFT                    6
#define PMU_RG_BUCK_VAUD18_OC_FLAG_CLR_SEL_MASK       0x1
#define PMU_RG_BUCK_VAUD18_OC_FLAG_CLR_SEL_SHIFT      4
#define PMU_RG_BUCK_VAUD18_CK_SW_MODE_MASK            0x1
#define PMU_RG_BUCK_VAUD18_CK_SW_MODE_SHIFT           6
#define PMU_RG_BUCK_VAUD18_CK_SW_EN_MASK              0x1
#define PMU_RG_BUCK_VAUD18_CK_SW_EN_SHIFT             7
#define PMU_RG_BUCK_VAUD18_VOSEL_MASK                 0x7F
#define PMU_RG_BUCK_VAUD18_VOSEL_SHIFT                0
#define PMU_RG_BUCK_STB_MAX_MASK                      0x7FF
#define PMU_RG_BUCK_STB_MAX_SHIFT                     0
#define PMU_RG_BUCK_LP_PROT_DISABLE_MASK              0x1
#define PMU_RG_BUCK_LP_PROT_DISABLE_SHIFT             11
#define PMU_RG_BUCK_VSLEEP_SRC0_MASK                  0x1FF
#define PMU_RG_BUCK_VSLEEP_SRC0_SHIFT                 0
#define PMU_RG_BUCK_VSLEEP_SRC1_MASK                  0x7F
#define PMU_RG_BUCK_VSLEEP_SRC1_SHIFT                 9
#define PMU_RG_BUCK_R2R_SRC0_MASK                     0x1FF
#define PMU_RG_BUCK_R2R_SRC0_SHIFT                    0
#define PMU_RG_BUCK_R2R_SRC1_MASK                     0x1F
#define PMU_RG_BUCK_R2R_SRC1_SHIFT                    11
#define PMU_RG_BUCK_LP_SEQ_COUNT_MASK                 0x1FF
#define PMU_RG_BUCK_LP_SEQ_COUNT_SHIFT                0
#define PMU_RG_BUCK_ON_SEQ_COUNT_MASK                 0x7F
#define PMU_RG_BUCK_ON_SEQ_COUNT_SHIFT                9
#define PMU_RG_BUCK_VCORE_OC_SDN_STATUS_MASK          0x1
#define PMU_RG_BUCK_VCORE_OC_SDN_STATUS_SHIFT         0
#define PMU_RG_BUCK_VIO18_OC_SDN_STATUS_MASK          0x1
#define PMU_RG_BUCK_VIO18_OC_SDN_STATUS_SHIFT         1
#define PMU_RG_BUCK_VRF_OC_SDN_STATUS_MASK            0x1
#define PMU_RG_BUCK_VRF_OC_SDN_STATUS_SHIFT           2
#define PMU_RG_BUCK_VAUD18_OC_SDN_STATUS_MASK         0x1
#define PMU_RG_BUCK_VAUD18_OC_SDN_STATUS_SHIFT        3
#define PMU_BUCK_VCORE_WDTDBG_VOSEL_MASK              0x7F
#define PMU_BUCK_VCORE_WDTDBG_VOSEL_SHIFT             0
#define PMU_BUCK_VIO18_WDTDBG_VOSEL_MASK              0x7F
#define PMU_BUCK_VIO18_WDTDBG_VOSEL_SHIFT             8
#define PMU_BUCK_VRF_WDTDBG_VOSEL_MASK                0x7F
#define PMU_BUCK_VRF_WDTDBG_VOSEL_SHIFT               0
#define PMU_BUCK_VAUD18_WDTDBG_VOSEL_MASK             0x7F
#define PMU_BUCK_VAUD18_WDTDBG_VOSEL_SHIFT            8
#define PMU_RG_BUCK_VCORE_OC_SDN_EN_MASK              0x1
#define PMU_RG_BUCK_VCORE_OC_SDN_EN_SHIFT             0
#define PMU_RG_BUCK_VIO18_OC_SDN_EN_MASK              0x1
#define PMU_RG_BUCK_VIO18_OC_SDN_EN_SHIFT             1
#define PMU_RG_BUCK_VRF_OC_SDN_EN_MASK                0x1
#define PMU_RG_BUCK_VRF_OC_SDN_EN_SHIFT               2
#define PMU_RG_BUCK_VAUD18_OC_SDN_EN_MASK             0x1
#define PMU_RG_BUCK_VAUD18_OC_SDN_EN_SHIFT            3
#define PMU_RG_BUCK_OC_SDN_EN_SEL_MASK                0x1
#define PMU_RG_BUCK_OC_SDN_EN_SEL_SHIFT               4
#define PMU_RG_BUCK_VCORE_VOSEL_LIMIT_SEL_MASK        0x3
#define PMU_RG_BUCK_VCORE_VOSEL_LIMIT_SEL_SHIFT       0
#define PMU_RG_BUCK_VIO18_VOSEL_LIMIT_SEL_MASK        0x3
#define PMU_RG_BUCK_VIO18_VOSEL_LIMIT_SEL_SHIFT       2
#define PMU_RG_BUCK_VRF_VOSEL_LIMIT_SEL_MASK          0x3
#define PMU_RG_BUCK_VRF_VOSEL_LIMIT_SEL_SHIFT         4
#define PMU_RG_BUCK_VAUD18_VOSEL_LIMIT_SEL_MASK       0x3
#define PMU_RG_BUCK_VAUD18_VOSEL_LIMIT_SEL_SHIFT      6
#define PMU_RG_BUCK_AO_RSV0_MASK                      0xFFFF
#define PMU_RG_BUCK_AO_RSV0_SHIFT                     0
#define PMU_RG_BUCK_AO_RSV1_MASK                      0xFFFF
#define PMU_RG_BUCK_AO_RSV1_SHIFT                     0
#define PMU_RG_BUCK_AO_EFUSE_RSVED0_MASK              0xFF
#define PMU_RG_BUCK_AO_EFUSE_RSVED0_SHIFT             0
#define PMU_RG_BUCK_AO_EFUSE_RSVED1_MASK              0xFF
#define PMU_RG_BUCK_AO_EFUSE_RSVED1_SHIFT             0
#define PMU_RG_BUCK2M_CK_PDN_HWEN_MASK                0x1
#define PMU_RG_BUCK2M_CK_PDN_HWEN_SHIFT               0
#define PMU_RG_BUCK2M_CK_PDN_MASK                     0x1
#define PMU_RG_BUCK2M_CK_PDN_SHIFT                    1
#define PMU_RG_BUCK_DCM_MODE_MASK                     0x1
#define PMU_RG_BUCK_DCM_MODE_SHIFT                    3
#define PMU_RG_BUCK_VCORE_NM_PFM_IPEAK_MASK           0x1F
#define PMU_RG_BUCK_VCORE_NM_PFM_IPEAK_SHIFT          0
#define PMU_RG_BUCK_VCORE_LP_PFM_IPEAK_MASK           0x1F
#define PMU_RG_BUCK_VCORE_LP_PFM_IPEAK_SHIFT          8
#define PMU_DA_VCORE_PFM_IPEAK_MASK                   0x3F
#define PMU_DA_VCORE_PFM_IPEAK_SHIFT                  0
#define PMU_RG_BUCK_VCORE_DVS_PFM_IPEAK_MASK          0x1F
#define PMU_RG_BUCK_VCORE_DVS_PFM_IPEAK_SHIFT         8
#define PMU_RG_BUCK_VIO18_NM_PFM_IPEAK_MASK           0x1F
#define PMU_RG_BUCK_VIO18_NM_PFM_IPEAK_SHIFT          0
#define PMU_RG_BUCK_VIO18_LP_PFM_IPEAK_MASK           0x1F
#define PMU_RG_BUCK_VIO18_LP_PFM_IPEAK_SHIFT          8
#define PMU_DA_VIO18_PFM_IPEAK_MASK                   0x3F
#define PMU_DA_VIO18_PFM_IPEAK_SHIFT                  0
#define PMU_RG_BUCK_VRF_NM_PFM_IPEAK_MASK             0x1F
#define PMU_RG_BUCK_VRF_NM_PFM_IPEAK_SHIFT            0
#define PMU_RG_BUCK_VRF_LP_PFM_IPEAK_MASK             0x1F
#define PMU_RG_BUCK_VRF_LP_PFM_IPEAK_SHIFT            8
#define PMU_DA_VRF_PFM_IPEAK_MASK                     0x3F
#define PMU_DA_VRF_PFM_IPEAK_SHIFT                    0
#define PMU_RG_BUCK_VAUD18_NM_PFM_IPEAK_MASK          0x1F
#define PMU_RG_BUCK_VAUD18_NM_PFM_IPEAK_SHIFT         0
#define PMU_RG_BUCK_VAUD18_LP_PFM_IPEAK_MASK          0x1F
#define PMU_RG_BUCK_VAUD18_LP_PFM_IPEAK_SHIFT         8
#define PMU_DA_VAUD18_PFM_IPEAK_MASK                  0x3F
#define PMU_DA_VAUD18_PFM_IPEAK_SHIFT                 0
#define PMU_RG_BUCK_VAUD18_DVS_PFM_IPEAK_MASK         0x1F
#define PMU_RG_BUCK_VAUD18_DVS_PFM_IPEAK_SHIFT        8
#define PMU_RG_BUCK_K_RST_DONE_MASK                   0x1
#define PMU_RG_BUCK_K_RST_DONE_SHIFT                  0
#define PMU_RG_BUCK_K_MAP_SEL_MASK                    0x1
#define PMU_RG_BUCK_K_MAP_SEL_SHIFT                   1
#define PMU_RG_BUCK_K_ONCE_EN_MASK                    0x1
#define PMU_RG_BUCK_K_ONCE_EN_SHIFT                   2
#define PMU_RG_BUCK_K_ONCE_MASK                       0x1
#define PMU_RG_BUCK_K_ONCE_SHIFT                      3
#define PMU_RG_BUCK_K_START_MANUAL_MASK               0x1
#define PMU_RG_BUCK_K_START_MANUAL_SHIFT              4
#define PMU_RG_BUCK_K_SRC_SEL_MASK                    0x1
#define PMU_RG_BUCK_K_SRC_SEL_SHIFT                   5
#define PMU_RG_BUCK_K_AUTO_EN_MASK                    0x1
#define PMU_RG_BUCK_K_AUTO_EN_SHIFT                   6
#define PMU_RG_BUCK_K_INV_MASK                        0x1
#define PMU_RG_BUCK_K_INV_SHIFT                       7
#define PMU_RG_BUCK_K_CK_EN_MASK                      0x1
#define PMU_RG_BUCK_K_CK_EN_SHIFT                     8
#define PMU_BUCK_K_RESULT_MASK                        0x1
#define PMU_BUCK_K_RESULT_SHIFT                       0
#define PMU_BUCK_K_DONE_MASK                          0x1
#define PMU_BUCK_K_DONE_SHIFT                         1
#define PMU_BUCK_K_CONTROL_MASK                       0x3F
#define PMU_BUCK_K_CONTROL_SHIFT                      2
#define PMU_DA_SMPS_OSC_CAL_MASK                      0x3F
#define PMU_DA_SMPS_OSC_CAL_SHIFT                     8
#define PMU_RG_BUCK_K_BUCK_CK_CNT_MASK                0x3FF
#define PMU_RG_BUCK_K_BUCK_CK_CNT_SHIFT               0
#define PMU_RG_BUCK_K_CONTROL_SMPS_MASK               0x3F
#define PMU_RG_BUCK_K_CONTROL_SMPS_SHIFT              7
#define PMU_RG_BUCK_CO_RSV0_MASK                      0xFFFF
#define PMU_RG_BUCK_CO_RSV0_SHIFT                     0
#define PMU_RG_BUCK_CO_RSV1_MASK                      0xFFFF
#define PMU_RG_BUCK_CO_RSV1_SHIFT                     0
#define PMU_RG_BUCK_CO_EFUSE_RSVED0_MASK              0xFF
#define PMU_RG_BUCK_CO_EFUSE_RSVED0_SHIFT             0
#define PMU_RG_BUCK_CO_EFUSE_RSVED1_MASK              0xFF
#define PMU_RG_BUCK_CO_EFUSE_RSVED1_SHIFT             0
#define PMU_RG_AUTOK_RST_MASK                         0x1
#define PMU_RG_AUTOK_RST_SHIFT                        1
#define PMU_RG_VCORE_DISPG_MASK                       0x1
#define PMU_RG_VCORE_DISPG_SHIFT                      0
#define PMU_RG_VCORE_SLEEP_TIME_MASK                  0x3
#define PMU_RG_VCORE_SLEEP_TIME_SHIFT                 1
#define PMU_RG_VCORE_UG_SR_MASK                       0x3
#define PMU_RG_VCORE_UG_SR_SHIFT                      3
#define PMU_RG_VCORE_LG_SR_MASK                       0x3
#define PMU_RG_VCORE_LG_SR_SHIFT                      5
#define PMU_RG_VCORE_NDIS_EN_MASK                     0x1
#define PMU_RG_VCORE_NDIS_EN_SHIFT                    7
#define PMU_RG_VCORE_TMDL_MASK                        0x1
#define PMU_RG_VCORE_TMDL_SHIFT                       8
#define PMU_RG_VCORE_FUGON_MASK                       0x1
#define PMU_RG_VCORE_FUGON_SHIFT                      9
#define PMU_RG_VCORE_FLGON_MASK                       0x1
#define PMU_RG_VCORE_FLGON_SHIFT                      10
#define PMU_RGS_VCORE_OC_STATUS_MASK                  0x1
#define PMU_RGS_VCORE_OC_STATUS_SHIFT                 11
#define PMU_RG_VCORE_DISAUTOK_MASK                    0x1
#define PMU_RG_VCORE_DISAUTOK_SHIFT                   12
#define PMU_RGS_VCORE_TRIMOK_STATUS_MASK              0x1
#define PMU_RGS_VCORE_TRIMOK_STATUS_SHIFT             13
#define PMU_RG_VCORE_IBIAS_SELECT_MASK                0x3
#define PMU_RG_VCORE_IBIAS_SELECT_SHIFT               14
#define PMU_RGS_VCORE_DIG_MON_MASK                    0xFF
#define PMU_RGS_VCORE_DIG_MON_SHIFT                   0
#define PMU_RG_VCORE_RSV_MASK                         0x7F
#define PMU_RG_VCORE_RSV_SHIFT                        0
#define PMU_RG_VCORE_SLEEP_VOLTAGE_MASK               0x3
#define PMU_RG_VCORE_SLEEP_VOLTAGE_SHIFT              0
#define PMU_RG_VCORE_OV_DETECT_EN_MASK                0x1
#define PMU_RG_VCORE_OV_DETECT_EN_SHIFT               2
#define PMU_RG_VCORE_TRIM_IBIAS_TMDL_MASK             0x1
#define PMU_RG_VCORE_TRIM_IBIAS_TMDL_SHIFT            3
#define PMU_RG_VCORE_IBIAS_LPCMP_MASK                 0x3
#define PMU_RG_VCORE_IBIAS_LPCMP_SHIFT                8
#define PMU_RG_VCORE_FB_BYPASS_MASK                   0x1
#define PMU_RG_VCORE_FB_BYPASS_SHIFT                  10
#define PMU_RG_VIO18_DISPG_MASK                       0x1
#define PMU_RG_VIO18_DISPG_SHIFT                      0
#define PMU_RG_VIO18_SLEEP_TIME_MASK                  0x3
#define PMU_RG_VIO18_SLEEP_TIME_SHIFT                 1
#define PMU_RG_VIO18_UG_SR_MASK                       0x3
#define PMU_RG_VIO18_UG_SR_SHIFT                      3
#define PMU_RG_VIO18_LG_SR_MASK                       0x3
#define PMU_RG_VIO18_LG_SR_SHIFT                      5
#define PMU_RG_VIO18_NDIS_EN_MASK                     0x1
#define PMU_RG_VIO18_NDIS_EN_SHIFT                    7
#define PMU_RG_VIO18_TMDL_MASK                        0x1
#define PMU_RG_VIO18_TMDL_SHIFT                       8
#define PMU_RG_VIO18_FUGON_MASK                       0x1
#define PMU_RG_VIO18_FUGON_SHIFT                      9
#define PMU_RG_VIO18_FLGON_MASK                       0x1
#define PMU_RG_VIO18_FLGON_SHIFT                      10
#define PMU_RGS_VIO18_OC_STATUS_MASK                  0x1
#define PMU_RGS_VIO18_OC_STATUS_SHIFT                 11
#define PMU_RG_VIO18_DISAUTOK_MASK                    0x1
#define PMU_RG_VIO18_DISAUTOK_SHIFT                   12
#define PMU_RGS_VIO18_TRIMOK_STATUS_MASK              0x1
#define PMU_RGS_VIO18_TRIMOK_STATUS_SHIFT             13
#define PMU_RG_VIO18_IBIAS_SELECT_MASK                0x3
#define PMU_RG_VIO18_IBIAS_SELECT_SHIFT               14
#define PMU_RGS_VIO18_DIG_MON_MASK                    0xFF
#define PMU_RGS_VIO18_DIG_MON_SHIFT                   0
#define PMU_RG_VIO18_RSV_MASK                         0x7F
#define PMU_RG_VIO18_RSV_SHIFT                        0
#define PMU_RG_VIO18_SLEEP_VOLTAGE_MASK               0x3
#define PMU_RG_VIO18_SLEEP_VOLTAGE_SHIFT              0
#define PMU_RG_VIO18_OV_DETECT_EN_MASK                0x1
#define PMU_RG_VIO18_OV_DETECT_EN_SHIFT               2
#define PMU_RG_VIO18_TRIM_IBIAS_TMDL_MASK             0x1
#define PMU_RG_VIO18_TRIM_IBIAS_TMDL_SHIFT            3
#define PMU_RG_VIO18_IBIAS_LPCMP_MASK                 0x3
#define PMU_RG_VIO18_IBIAS_LPCMP_SHIFT                8
#define PMU_RG_VIO18_FB_BYPASS_MASK                   0x1
#define PMU_RG_VIO18_FB_BYPASS_SHIFT                  10
#define PMU_RG_VRF_DISPG_MASK                         0x1
#define PMU_RG_VRF_DISPG_SHIFT                        0
#define PMU_RG_VRF_SLEEP_TIME_MASK                    0x3
#define PMU_RG_VRF_SLEEP_TIME_SHIFT                   1
#define PMU_RG_VRF_UG_SR_MASK                         0x3
#define PMU_RG_VRF_UG_SR_SHIFT                        3
#define PMU_RG_VRF_LG_SR_MASK                         0x3
#define PMU_RG_VRF_LG_SR_SHIFT                        5
#define PMU_RG_VRF_NDIS_EN_MASK                       0x1
#define PMU_RG_VRF_NDIS_EN_SHIFT                      7
#define PMU_RG_VRF_TMDL_MASK                          0x1
#define PMU_RG_VRF_TMDL_SHIFT                         8
#define PMU_RG_VRF_FUGON_MASK                         0x1
#define PMU_RG_VRF_FUGON_SHIFT                        9
#define PMU_RG_VRF_FLGON_MASK                         0x1
#define PMU_RG_VRF_FLGON_SHIFT                        10
#define PMU_RGS_VRF_OC_STATUS_MASK                    0x1
#define PMU_RGS_VRF_OC_STATUS_SHIFT                   11
#define PMU_RG_VRF_DISAUTOK_MASK                      0x1
#define PMU_RG_VRF_DISAUTOK_SHIFT                     12
#define PMU_RGS_VRF_TRIMOK_STATUS_MASK                0x1
#define PMU_RGS_VRF_TRIMOK_STATUS_SHIFT               13
#define PMU_RG_VRF_IBIAS_SELECT_MASK                  0x3
#define PMU_RG_VRF_IBIAS_SELECT_SHIFT                 14
#define PMU_RGS_VRF_DIG_MON_MASK                      0xFF
#define PMU_RGS_VRF_DIG_MON_SHIFT                     8
#define PMU_RG_VRF_RSV_MASK                           0x7F
#define PMU_RG_VRF_RSV_SHIFT                          0
#define PMU_RG_VRF_SLEEP_VOLTAGE_MASK                 0x3
#define PMU_RG_VRF_SLEEP_VOLTAGE_SHIFT                0
#define PMU_RG_VRF_OV_DETECT_EN_MASK                  0x1
#define PMU_RG_VRF_OV_DETECT_EN_SHIFT                 2
#define PMU_RG_VRF_TRIM_IBIAS_TMDL_MASK               0x1
#define PMU_RG_VRF_TRIM_IBIAS_TMDL_SHIFT              3
#define PMU_RG_VRF_IBIAS_LPCMP_MASK                   0x3
#define PMU_RG_VRF_IBIAS_LPCMP_SHIFT                  8
#define PMU_RG_VRF_FB_BYPASS_MASK                     0x1
#define PMU_RG_VRF_FB_BYPASS_SHIFT                    10
#define PMU_RG_VAUD18_DISPG_MASK                      0x1
#define PMU_RG_VAUD18_DISPG_SHIFT                     0
#define PMU_RG_VAUD18_SLEEP_TIME_MASK                 0x3
#define PMU_RG_VAUD18_SLEEP_TIME_SHIFT                1
#define PMU_RG_VAUD18_UG_SR_MASK                      0x3
#define PMU_RG_VAUD18_UG_SR_SHIFT                     3
#define PMU_RG_VAUD18_LG_SR_MASK                      0x3
#define PMU_RG_VAUD18_LG_SR_SHIFT                     5
#define PMU_RG_VAUD18_NDIS_EN_MASK                    0x1
#define PMU_RG_VAUD18_NDIS_EN_SHIFT                   7
#define PMU_RG_VAUD18_TMDL_MASK                       0x1
#define PMU_RG_VAUD18_TMDL_SHIFT                      8
#define PMU_RG_VAUD18_FUGON_MASK                      0x1
#define PMU_RG_VAUD18_FUGON_SHIFT                     9
#define PMU_RG_VAUD18_FLGON_MASK                      0x1
#define PMU_RG_VAUD18_FLGON_SHIFT                     10
#define PMU_RGS_VAUD18_OC_STATUS_MASK                 0x1
#define PMU_RGS_VAUD18_OC_STATUS_SHIFT                11
#define PMU_RG_VAUD18_DISAUTOK_MASK                   0x1
#define PMU_RG_VAUD18_DISAUTOK_SHIFT                  12
#define PMU_RGS_VAUD18_TRIMOK_STATUS_MASK             0x1
#define PMU_RGS_VAUD18_TRIMOK_STATUS_SHIFT            13
#define PMU_RG_VAUD18_IBIAS_SELECT_MASK               0x3
#define PMU_RG_VAUD18_IBIAS_SELECT_SHIFT              14
#define PMU_RGS_VAUD18_DIG_MON_MASK                   0xFF
#define PMU_RGS_VAUD18_DIG_MON_SHIFT                  0
#define PMU_RG_VAUD18_RSV_MASK                        0x7F
#define PMU_RG_VAUD18_RSV_SHIFT                       0
#define PMU_RG_VAUD18_SLEEP_VOLTAGE_MASK              0x3
#define PMU_RG_VAUD18_SLEEP_VOLTAGE_SHIFT             0
#define PMU_RG_VAUD18_OV_DETECT_EN_MASK               0x1
#define PMU_RG_VAUD18_OV_DETECT_EN_SHIFT              2
#define PMU_RG_VAUD18_TRIM_IBIAS_TMDL_MASK            0x1
#define PMU_RG_VAUD18_TRIM_IBIAS_TMDL_SHIFT           3
#define PMU_RG_VAUD18_IBIAS_LPCMP_MASK                0x3
#define PMU_RG_VAUD18_IBIAS_LPCMP_SHIFT               8
#define PMU_RG_VAUD18_FB_BYPASS_MASK                  0x1
#define PMU_RG_VAUD18_FB_BYPASS_SHIFT                 10
#define PMU_RG_SMPS_RSV_MASK                          0x3
#define PMU_RG_SMPS_RSV_SHIFT                         0
#define PMU_RG_VCORE_TRIMH_MASK                       0x1F
#define PMU_RG_VCORE_TRIMH_SHIFT                      8
#define PMU_RG_VIO18_TRIMH_MASK                       0x1F
#define PMU_RG_VIO18_TRIMH_SHIFT                      0
#define PMU_RG_VRF_TRIMH_MASK                         0x1F
#define PMU_RG_VRF_TRIMH_SHIFT                        5
#define PMU_RG_VAUD18_TRIMH_MASK                      0x1F
#define PMU_RG_VAUD18_TRIMH_SHIFT                     10
#define PMU_RG_VCORE_ZC_TRIM_MASK                     0x3
#define PMU_RG_VCORE_ZC_TRIM_SHIFT                    0
#define PMU_RG_VCORE_CSPSLP_TRIM_MASK                 0xF
#define PMU_RG_VCORE_CSPSLP_TRIM_SHIFT                5
#define PMU_RG_VCORE_IBIAS_TRIM_MASK                  0x1F
#define PMU_RG_VCORE_IBIAS_TRIM_SHIFT                 9
#define PMU_RG_VIO18_ZC_TRIM_MASK                     0x3
#define PMU_RG_VIO18_ZC_TRIM_SHIFT                    0
#define PMU_RG_VIO18_CSPSLP_TRIM_MASK                 0xF
#define PMU_RG_VIO18_CSPSLP_TRIM_SHIFT                5
#define PMU_RG_VIO18_IBIAS_TRIM_MASK                  0x1F
#define PMU_RG_VIO18_IBIAS_TRIM_SHIFT                 9
#define PMU_RG_VRF_ZC_TRIM_MASK                       0x3
#define PMU_RG_VRF_ZC_TRIM_SHIFT                      0
#define PMU_RG_VRF_CSPSLP_TRIM_MASK                   0xF
#define PMU_RG_VRF_CSPSLP_TRIM_SHIFT                  5
#define PMU_RG_VRF_IBIAS_TRIM_MASK                    0x1F
#define PMU_RG_VRF_IBIAS_TRIM_SHIFT                   9
#define PMU_RG_VAUD18_ZC_TRIM_MASK                    0x3
#define PMU_RG_VAUD18_ZC_TRIM_SHIFT                   0
#define PMU_RG_VAUD18_CSPSLP_TRIM_MASK                0xF
#define PMU_RG_VAUD18_CSPSLP_TRIM_SHIFT               5
#define PMU_RG_VAUD18_IBIAS_TRIM_MASK                 0x1F
#define PMU_RG_VAUD18_IBIAS_TRIM_SHIFT                9
#define PMU_AUXADC_ADC_OUT_CH0_MASK                   0x7FFF
#define PMU_AUXADC_ADC_OUT_CH0_SHIFT                  0
#define PMU_AUXADC_ADC_RDY_CH0_MASK                   0x1
#define PMU_AUXADC_ADC_RDY_CH0_SHIFT                  15
#define PMU_AUXADC_ADC_OUT_CH2_MASK                   0xFFF
#define PMU_AUXADC_ADC_OUT_CH2_SHIFT                  0
#define PMU_AUXADC_ADC_RDY_CH2_MASK                   0x1
#define PMU_AUXADC_ADC_RDY_CH2_SHIFT                  15
#define PMU_AUXADC_ADC_OUT_CH3_MASK                   0xFFF
#define PMU_AUXADC_ADC_OUT_CH3_SHIFT                  0
#define PMU_AUXADC_ADC_RDY_CH3_MASK                   0x1
#define PMU_AUXADC_ADC_RDY_CH3_SHIFT                  15
#define PMU_AUXADC_ADC_OUT_CH4_MASK                   0xFFF
#define PMU_AUXADC_ADC_OUT_CH4_SHIFT                  0
#define PMU_AUXADC_ADC_RDY_CH4_MASK                   0x1
#define PMU_AUXADC_ADC_RDY_CH4_SHIFT                  15
#define PMU_AUXADC_ADC_OUT_THR_HW_MASK                0xFFF
#define PMU_AUXADC_ADC_OUT_THR_HW_SHIFT               0
#define PMU_AUXADC_ADC_RDY_THR_HW_MASK                0x1
#define PMU_AUXADC_ADC_RDY_THR_HW_SHIFT               15
#define PMU_AUXADC_ADC_OUT_LBAT_MASK                  0xFFF
#define PMU_AUXADC_ADC_OUT_LBAT_SHIFT                 0
#define PMU_AUXADC_ADC_RDY_LBAT_MASK                  0x1
#define PMU_AUXADC_ADC_RDY_LBAT_SHIFT                 12
#define PMU_AUXADC_ADC_OUT_LBAT2_MASK                 0xFFF
#define PMU_AUXADC_ADC_OUT_LBAT2_SHIFT                0
#define PMU_AUXADC_ADC_RDY_LBAT2_MASK                 0x1
#define PMU_AUXADC_ADC_RDY_LBAT2_SHIFT                15
#define PMU_AUXADC_ADC_OUT_PWRON_PCHR_MASK            0x7FFF
#define PMU_AUXADC_ADC_OUT_PWRON_PCHR_SHIFT           0
#define PMU_AUXADC_ADC_RDY_PWRON_PCHR_MASK            0x1
#define PMU_AUXADC_ADC_RDY_PWRON_PCHR_SHIFT           15
#define PMU_AUXADC_ADC_OUT_WAKEUP_PCHR_MASK           0x7FFF
#define PMU_AUXADC_ADC_OUT_WAKEUP_PCHR_SHIFT          0
#define PMU_AUXADC_ADC_RDY_WAKEUP_PCHR_MASK           0x1
#define PMU_AUXADC_ADC_RDY_WAKEUP_PCHR_SHIFT          15
#define PMU_AUXADC_ADC_OUT_CH0_BY_AP_MASK             0x7FFF
#define PMU_AUXADC_ADC_OUT_CH0_BY_AP_SHIFT            0
#define PMU_AUXADC_ADC_RDY_CH0_BY_AP_MASK             0x1
#define PMU_AUXADC_ADC_RDY_CH0_BY_AP_SHIFT            15
#define PMU_AUXADC_ADC_OUT_RAW_MASK                   0x7FFF
#define PMU_AUXADC_ADC_OUT_RAW_SHIFT                  0
#define PMU_AUXADC_ADC_OUT_JEITA_MASK                 0xFFF
#define PMU_AUXADC_ADC_OUT_JEITA_SHIFT                0
#define PMU_AUXADC_ADC_RDY_JEITA_MASK                 0x1
#define PMU_AUXADC_ADC_RDY_JEITA_SHIFT                15
#define PMU_AUXADC_ADC_OUT_VBATSNS_DET_MASK           0xFFF
#define PMU_AUXADC_ADC_OUT_VBATSNS_DET_SHIFT          0
#define PMU_AUXADC_ADC_RDY_VBATSNS_DET_MASK           0x1
#define PMU_AUXADC_ADC_RDY_VBATSNS_DET_SHIFT          15
#define PMU_AUXADC_ADC_BUSY_IN_MASK                   0xFFF
#define PMU_AUXADC_ADC_BUSY_IN_SHIFT                  0
#define PMU_AUXADC_ADC_BUSY_IN_LBAT_MASK              0x1
#define PMU_AUXADC_ADC_BUSY_IN_LBAT_SHIFT             12
#define PMU_AUXADC_ADC_BUSY_IN_LBAT2_MASK             0x1
#define PMU_AUXADC_ADC_BUSY_IN_LBAT2_SHIFT            13
#define PMU_AUXADC_ADC_BUSY_IN_WAKEUP_MASK            0x1
#define PMU_AUXADC_ADC_BUSY_IN_WAKEUP_SHIFT           15
#define PMU_AUXADC_ADC_BUSY_IN_VBATSNS_DET_MASK       0x1
#define PMU_AUXADC_ADC_BUSY_IN_VBATSNS_DET_SHIFT      2
#define PMU_AUXADC_ADC_BUSY_IN_PWRON_MASK             0x1
#define PMU_AUXADC_ADC_BUSY_IN_PWRON_SHIFT            3
#define PMU_AUXADC_ADC_BUSY_IN_JEITA_MASK             0x1
#define PMU_AUXADC_ADC_BUSY_IN_JEITA_SHIFT            5
#define PMU_AUXADC_ADC_BUSY_IN_THR_HW_MASK            0x1
#define PMU_AUXADC_ADC_BUSY_IN_THR_HW_SHIFT           14
#define PMU_AUXADC_RQST_CH0_MASK                      0x1
#define PMU_AUXADC_RQST_CH0_SHIFT                     0
#define PMU_AUXADC_RQST_CH2_MASK                      0x1
#define PMU_AUXADC_RQST_CH2_SHIFT                     2
#define PMU_AUXADC_RQST_CH3_MASK                      0x1
#define PMU_AUXADC_RQST_CH3_SHIFT                     3
#define PMU_AUXADC_RQST_CH4_MASK                      0x1
#define PMU_AUXADC_RQST_CH4_SHIFT                     4
#define PMU_AUXADC_CK_ON_EXTD_MASK                    0x3F
#define PMU_AUXADC_CK_ON_EXTD_SHIFT                   0
#define PMU_AUXADC_SRCLKEN_SRC_SEL_MASK               0x3
#define PMU_AUXADC_SRCLKEN_SRC_SEL_SHIFT              6
#define PMU_AUXADC_ADC_PWDB_MASK                      0x1
#define PMU_AUXADC_ADC_PWDB_SHIFT                     8
#define PMU_AUXADC_ADC_PWDB_SWCTRL_MASK               0x1
#define PMU_AUXADC_ADC_PWDB_SWCTRL_SHIFT              9
#define PMU_AUXADC_STRUP_CK_ON_ENB_MASK               0x1
#define PMU_AUXADC_STRUP_CK_ON_ENB_SHIFT              10
#define PMU_AUXADC_ADC_RDY_WAKEUP_CLR_MASK            0x1
#define PMU_AUXADC_ADC_RDY_WAKEUP_CLR_SHIFT           11
#define PMU_AUXADC_SRCLKEN_CK_EN_MASK                 0x1
#define PMU_AUXADC_SRCLKEN_CK_EN_SHIFT                12
#define PMU_AUXADC_CK_AON_MASK                        0x1
#define PMU_AUXADC_CK_AON_SHIFT                       15
#define PMU_AUXADC_AVG_NUM_SMALL_MASK                 0x7
#define PMU_AUXADC_AVG_NUM_SMALL_SHIFT                0
#define PMU_AUXADC_AVG_NUM_LARGE_MASK                 0x7
#define PMU_AUXADC_AVG_NUM_LARGE_SHIFT                3
#define PMU_AUXADC_SPL_NUM_MASK                       0x3FF
#define PMU_AUXADC_SPL_NUM_SHIFT                      6
#define PMU_AUXADC_AVG_NUM_SEL_MASK                   0xFFF
#define PMU_AUXADC_AVG_NUM_SEL_SHIFT                  0
#define PMU_AUXADC_AVG_NUM_SEL_LBAT_MASK              0x1
#define PMU_AUXADC_AVG_NUM_SEL_LBAT_SHIFT             13
#define PMU_AUXADC_AVG_NUM_SEL_RECHG_MASK             0x1
#define PMU_AUXADC_AVG_NUM_SEL_RECHG_SHIFT            14
#define PMU_AUXADC_AVG_NUM_SEL_WAKEUP_MASK            0x1
#define PMU_AUXADC_AVG_NUM_SEL_WAKEUP_SHIFT           15
#define PMU_AUXADC_SPL_NUM_LARGE_MASK                 0x3FF
#define PMU_AUXADC_SPL_NUM_LARGE_SHIFT                0
#define PMU_AUXADC_SPL_NUM_SLEEP_MASK                 0x3FF
#define PMU_AUXADC_SPL_NUM_SLEEP_SHIFT                0
#define PMU_AUXADC_SPL_NUM_SLEEP_SEL_MASK             0x1
#define PMU_AUXADC_SPL_NUM_SLEEP_SEL_SHIFT            15
#define PMU_AUXADC_SPL_NUM_SEL_MASK                   0xFFF
#define PMU_AUXADC_SPL_NUM_SEL_SHIFT                  0
#define PMU_AUXADC_SPL_NUM_SEL_LBAT_MASK              0x1
#define PMU_AUXADC_SPL_NUM_SEL_LBAT_SHIFT             13
#define PMU_AUXADC_SPL_NUM_SEL_RECHG_MASK             0x1
#define PMU_AUXADC_SPL_NUM_SEL_RECHG_SHIFT            14
#define PMU_AUXADC_SPL_NUM_SEL_WAKEUP_MASK            0x1
#define PMU_AUXADC_SPL_NUM_SEL_WAKEUP_SHIFT           15
#define PMU_AUXADC_TRIM_CH0_SEL_MASK                  0x3
#define PMU_AUXADC_TRIM_CH0_SEL_SHIFT                 0
#define PMU_AUXADC_TRIM_CH2_SEL_MASK                  0x3
#define PMU_AUXADC_TRIM_CH2_SEL_SHIFT                 4
#define PMU_AUXADC_TRIM_CH3_SEL_MASK                  0x3
#define PMU_AUXADC_TRIM_CH3_SEL_SHIFT                 6
#define PMU_AUXADC_TRIM_CH4_SEL_MASK                  0x3
#define PMU_AUXADC_TRIM_CH4_SEL_SHIFT                 8
#define PMU_AUXADC_ADC_2S_COMP_ENB_MASK               0x1
#define PMU_AUXADC_ADC_2S_COMP_ENB_SHIFT              14
#define PMU_AUXADC_ADC_TRIM_COMP_MASK                 0x1
#define PMU_AUXADC_ADC_TRIM_COMP_SHIFT                15
#define PMU_AUXADC_SW_GAIN_TRIM_MASK                  0x7FFF
#define PMU_AUXADC_SW_GAIN_TRIM_SHIFT                 0
#define PMU_AUXADC_SW_OFFSET_TRIM_MASK                0x7FFF
#define PMU_AUXADC_SW_OFFSET_TRIM_SHIFT               0
#define PMU_AUXADC_RNG_EN_MASK                        0x1
#define PMU_AUXADC_RNG_EN_SHIFT                       0
#define PMU_AUXADC_DATA_REUSE_SEL_MASK                0x3
#define PMU_AUXADC_DATA_REUSE_SEL_SHIFT               1
#define PMU_AUXADC_TEST_MODE_MASK                     0x1
#define PMU_AUXADC_TEST_MODE_SHIFT                    3
#define PMU_AUXADC_BIT_SEL_MASK                       0x1
#define PMU_AUXADC_BIT_SEL_SHIFT                      4
#define PMU_AUXADC_START_SW_MASK                      0x1
#define PMU_AUXADC_START_SW_SHIFT                     5
#define PMU_AUXADC_START_SWCTRL_MASK                  0x1
#define PMU_AUXADC_START_SWCTRL_SHIFT                 6
#define PMU_AUXADC_TS_VBE_SEL_MASK                    0x1
#define PMU_AUXADC_TS_VBE_SEL_SHIFT                   7
#define PMU_AUXADC_TS_VBE_SEL_SWCTRL_MASK             0x1
#define PMU_AUXADC_TS_VBE_SEL_SWCTRL_SHIFT            8
#define PMU_AUXADC_VBUF_EN_MASK                       0x1
#define PMU_AUXADC_VBUF_EN_SHIFT                      9
#define PMU_AUXADC_VBUF_EN_SWCTRL_MASK                0x1
#define PMU_AUXADC_VBUF_EN_SWCTRL_SHIFT               10
#define PMU_AUXADC_OUT_SEL_MASK                       0x1
#define PMU_AUXADC_OUT_SEL_SHIFT                      11
#define PMU_AUXADC_DA_DAC_MASK                        0xFFF
#define PMU_AUXADC_DA_DAC_SHIFT                       0
#define PMU_AUXADC_DA_DAC_SWCTRL_MASK                 0x1
#define PMU_AUXADC_DA_DAC_SWCTRL_SHIFT                12
#define PMU_AD_AUXADC_COMP_MASK                       0x1
#define PMU_AD_AUXADC_COMP_SHIFT                      15
#define PMU_RG_AUXADC_CALI_MASK                       0xF
#define PMU_RG_AUXADC_CALI_SHIFT                      0
#define PMU_RG_AUX_RSV_MASK                           0xF
#define PMU_RG_AUX_RSV_SHIFT                          4
#define PMU_RG_VBUF_BYP_MASK                          0x1
#define PMU_RG_VBUF_BYP_SHIFT                         8
#define PMU_RG_VBUF_CALEN_MASK                        0x1
#define PMU_RG_VBUF_CALEN_SHIFT                       9
#define PMU_RG_VBUF_EXTEN_MASK                        0x1
#define PMU_RG_VBUF_EXTEN_SHIFT                       10
#define PMU_RG_TS_VBE_SEL_MASK                        0x7
#define PMU_RG_TS_VBE_SEL_SHIFT                       11
#define PMU_AUXADC_ADCIN_VSEN_EN_MASK                 0x1
#define PMU_AUXADC_ADCIN_VSEN_EN_SHIFT                0
#define PMU_AUXADC_ADCIN_VBAT_EN_MASK                 0x1
#define PMU_AUXADC_ADCIN_VBAT_EN_SHIFT                1
#define PMU_AUXADC_ADCIN_VSEN_MUX_EN_MASK             0x1
#define PMU_AUXADC_ADCIN_VSEN_MUX_EN_SHIFT            2
#define PMU_AUXADC_ADCIN_VSEN_EXT_BATON_EN_MASK       0x1
#define PMU_AUXADC_ADCIN_VSEN_EXT_BATON_EN_SHIFT      3
#define PMU_AUXADC_ADCIN_CHR_EN_MASK                  0x1
#define PMU_AUXADC_ADCIN_CHR_EN_SHIFT                 4
#define PMU_AUXADC_ADCIN_BATON_TDET_EN_MASK           0x1
#define PMU_AUXADC_ADCIN_BATON_TDET_EN_SHIFT          5
#define PMU_AUXADC_ACCDET_ANASWCTRL_EN_MASK           0x1
#define PMU_AUXADC_ACCDET_ANASWCTRL_EN_SHIFT          6
#define PMU_AUXADC_DIG0_RSV0_MASK                     0xF
#define PMU_AUXADC_DIG0_RSV0_SHIFT                    7
#define PMU_AUXADC_CHSEL_MASK                         0xF
#define PMU_AUXADC_CHSEL_SHIFT                        11
#define PMU_AUXADC_SWCTRL_EN_MASK                     0x1
#define PMU_AUXADC_SWCTRL_EN_SHIFT                    15
#define PMU_AUXADC_SOURCE_LBAT_SEL_MASK               0x1
#define PMU_AUXADC_SOURCE_LBAT_SEL_SHIFT              0
#define PMU_AUXADC_SOURCE_LBAT2_SEL_MASK              0x1
#define PMU_AUXADC_SOURCE_LBAT2_SEL_SHIFT             1
#define PMU_AUXADC_START_EXTD_MASK                    0x7F
#define PMU_AUXADC_START_EXTD_SHIFT                   2
#define PMU_AUXADC_DAC_EXTD_MASK                      0xF
#define PMU_AUXADC_DAC_EXTD_SHIFT                     11
#define PMU_AUXADC_DAC_EXTD_EN_MASK                   0x1
#define PMU_AUXADC_DAC_EXTD_EN_SHIFT                  15
#define PMU_AUXADC_PMU_THR_PDN_SW_MASK                0x1
#define PMU_AUXADC_PMU_THR_PDN_SW_SHIFT               2
#define PMU_AUXADC_PMU_THR_PDN_SEL_MASK               0x1
#define PMU_AUXADC_PMU_THR_PDN_SEL_SHIFT              3
#define PMU_AUXADC_PMU_THR_PDN_STATUS_MASK            0x1
#define PMU_AUXADC_PMU_THR_PDN_STATUS_SHIFT           4
#define PMU_AUXADC_DIG0_RSV1_MASK                     0x7
#define PMU_AUXADC_DIG0_RSV1_SHIFT                    5
#define PMU_AUXADC_START_SHADE_NUM_MASK               0x3FF
#define PMU_AUXADC_START_SHADE_NUM_SHIFT              0
#define PMU_AUXADC_START_SHADE_EN_MASK                0x1
#define PMU_AUXADC_START_SHADE_EN_SHIFT               14
#define PMU_AUXADC_START_SHADE_SEL_MASK               0x1
#define PMU_AUXADC_START_SHADE_SEL_SHIFT              15
#define PMU_AUXADC_ADC_RDY_FGADC_CLR_MASK             0x1
#define PMU_AUXADC_ADC_RDY_FGADC_CLR_SHIFT            14
#define PMU_AUXADC_ADC_RDY_PWRON_CLR_MASK             0x1
#define PMU_AUXADC_ADC_RDY_PWRON_CLR_SHIFT            15
#define PMU_AUXADC_AUTORPT_PRD_MASK                   0x3FF
#define PMU_AUXADC_AUTORPT_PRD_SHIFT                  0
#define PMU_AUXADC_AUTORPT_EN_MASK                    0x1
#define PMU_AUXADC_AUTORPT_EN_SHIFT                   15
#define PMU_AUXADC_LBAT_DEBT_MIN_SEL_MASK             0x3
#define PMU_AUXADC_LBAT_DEBT_MIN_SEL_SHIFT            0
#define PMU_AUXADC_LBAT_DEBT_MAX_SEL_MASK             0x3
#define PMU_AUXADC_LBAT_DEBT_MAX_SEL_SHIFT            0
#define PMU_AUXADC_LBAT_DET_PRD_SEL_MASK              0x3
#define PMU_AUXADC_LBAT_DET_PRD_SEL_SHIFT             0
#define PMU_AUXADC_LBAT_VOLT_MAX_MASK                 0xFFF
#define PMU_AUXADC_LBAT_VOLT_MAX_SHIFT                0
#define PMU_AUXADC_LBAT_IRQ_EN_MAX_MASK               0x1
#define PMU_AUXADC_LBAT_IRQ_EN_MAX_SHIFT              12
#define PMU_AUXADC_LBAT_EN_MAX_MASK                   0x1
#define PMU_AUXADC_LBAT_EN_MAX_SHIFT                  13
#define PMU_AUXADC_LBAT_MAX_IRQ_B_MASK                0x1
#define PMU_AUXADC_LBAT_MAX_IRQ_B_SHIFT               15
#define PMU_AUXADC_LBAT_VOLT_MIN_MASK                 0xFFF
#define PMU_AUXADC_LBAT_VOLT_MIN_SHIFT                0
#define PMU_AUXADC_LBAT_IRQ_EN_MIN_MASK               0x1
#define PMU_AUXADC_LBAT_IRQ_EN_MIN_SHIFT              12
#define PMU_AUXADC_LBAT_EN_MIN_MASK                   0x1
#define PMU_AUXADC_LBAT_EN_MIN_SHIFT                  13
#define PMU_AUXADC_LBAT_MIN_IRQ_B_MASK                0x1
#define PMU_AUXADC_LBAT_MIN_IRQ_B_SHIFT               15
#define PMU_AUXADC_LBAT_DEBOUNCE_COUNT_MAX_MASK       0x1F
#define PMU_AUXADC_LBAT_DEBOUNCE_COUNT_MAX_SHIFT      0
#define PMU_AUXADC_LBAT_DEBOUNCE_COUNT_MIN_MASK       0x1F
#define PMU_AUXADC_LBAT_DEBOUNCE_COUNT_MIN_SHIFT      0
#define PMU_AUXADC_ACCDET_AUTO_SPL_MASK               0x1
#define PMU_AUXADC_ACCDET_AUTO_SPL_SHIFT              0
#define PMU_AUXADC_ACCDET_AUTO_RQST_CLR_MASK          0x1
#define PMU_AUXADC_ACCDET_AUTO_RQST_CLR_SHIFT         1
#define PMU_AUXADC_ACCDET_DIG1_RSV0_MASK              0x3F
#define PMU_AUXADC_ACCDET_DIG1_RSV0_SHIFT             2
#define PMU_AUXADC_ACCDET_DIG0_RSV0_MASK              0xFF
#define PMU_AUXADC_ACCDET_DIG0_RSV0_SHIFT             8
#define PMU_AUXADC_THR_DEBT_MIN_SEL_MASK              0x3
#define PMU_AUXADC_THR_DEBT_MIN_SEL_SHIFT             0
#define PMU_AUXADC_THR_DEBT_MAX_SEL_MASK              0x3
#define PMU_AUXADC_THR_DEBT_MAX_SEL_SHIFT             0
#define PMU_AUXADC_THR_DET_PRD_SEL_MASK               0x3
#define PMU_AUXADC_THR_DET_PRD_SEL_SHIFT              0
#define PMU_AUXADC_THR_VOLT_MAX_MASK                  0xFFF
#define PMU_AUXADC_THR_VOLT_MAX_SHIFT                 0
#define PMU_AUXADC_THR_IRQ_EN_MAX_MASK                0x1
#define PMU_AUXADC_THR_IRQ_EN_MAX_SHIFT               12
#define PMU_AUXADC_THR_EN_MAX_MASK                    0x1
#define PMU_AUXADC_THR_EN_MAX_SHIFT                   13
#define PMU_AUXADC_THR_MAX_IRQ_B_MASK                 0x1
#define PMU_AUXADC_THR_MAX_IRQ_B_SHIFT                15
#define PMU_AUXADC_THR_VOLT_MIN_MASK                  0xFFF
#define PMU_AUXADC_THR_VOLT_MIN_SHIFT                 0
#define PMU_AUXADC_THR_IRQ_EN_MIN_MASK                0x1
#define PMU_AUXADC_THR_IRQ_EN_MIN_SHIFT               12
#define PMU_AUXADC_THR_EN_MIN_MASK                    0x1
#define PMU_AUXADC_THR_EN_MIN_SHIFT                   13
#define PMU_AUXADC_THR_MIN_IRQ_B_MASK                 0x1
#define PMU_AUXADC_THR_MIN_IRQ_B_SHIFT                15
#define PMU_AUXADC_THR_DEBOUNCE_COUNT_MAX_MASK        0x1F
#define PMU_AUXADC_THR_DEBOUNCE_COUNT_MAX_SHIFT       0
#define PMU_AUXADC_THR_DEBOUNCE_COUNT_MIN_MASK        0x1F
#define PMU_AUXADC_THR_DEBOUNCE_COUNT_MIN_SHIFT       0
#define PMU_EFUSE_GAIN_CH4_TRIM_MASK                  0xFFF
#define PMU_EFUSE_GAIN_CH4_TRIM_SHIFT                 0
#define PMU_EFUSE_OFFSET_CH4_TRIM_MASK                0x7FF
#define PMU_EFUSE_OFFSET_CH4_TRIM_SHIFT               0
#define PMU_EFUSE_GAIN_CH0_TRIM_MASK                  0xFFF
#define PMU_EFUSE_GAIN_CH0_TRIM_SHIFT                 0
#define PMU_EFUSE_OFFSET_CH0_TRIM_MASK                0x7FF
#define PMU_EFUSE_OFFSET_CH0_TRIM_SHIFT               0
#define PMU_EFUSE_GAIN_CH7_TRIM_MASK                  0xFFF
#define PMU_EFUSE_GAIN_CH7_TRIM_SHIFT                 0
#define PMU_EFUSE_OFFSET_CH7_TRIM_MASK                0x7FF
#define PMU_EFUSE_OFFSET_CH7_TRIM_SHIFT               0
#define PMU_AUXADC_FGADC_START_SW_MASK                0x1
#define PMU_AUXADC_FGADC_START_SW_SHIFT               0
#define PMU_AUXADC_FGADC_START_SEL_MASK               0x1
#define PMU_AUXADC_FGADC_START_SEL_SHIFT              1
#define PMU_AUXADC_FGADC_R_SW_MASK                    0x1
#define PMU_AUXADC_FGADC_R_SW_SHIFT                   2
#define PMU_AUXADC_FGADC_R_SEL_MASK                   0x1
#define PMU_AUXADC_FGADC_R_SEL_SHIFT                  3
#define PMU_AUXADC_DBG_DIG0_RSV2_MASK                 0x3F
#define PMU_AUXADC_DBG_DIG0_RSV2_SHIFT                4
#define PMU_AUXADC_DBG_DIG1_RSV2_MASK                 0x3F
#define PMU_AUXADC_DBG_DIG1_RSV2_SHIFT                10
#define PMU_AUXADC_LBAT2_DEBT_MIN_SEL_MASK            0x3
#define PMU_AUXADC_LBAT2_DEBT_MIN_SEL_SHIFT           0
#define PMU_AUXADC_LBAT2_DEBT_MAX_SEL_MASK            0x3
#define PMU_AUXADC_LBAT2_DEBT_MAX_SEL_SHIFT           0
#define PMU_AUXADC_LBAT2_DET_PRD_SEL_MASK             0x3
#define PMU_AUXADC_LBAT2_DET_PRD_SEL_SHIFT            0
#define PMU_AUXADC_LBAT2_VOLT_MAX_MASK                0xFFF
#define PMU_AUXADC_LBAT2_VOLT_MAX_SHIFT               0
#define PMU_AUXADC_LBAT2_IRQ_EN_MAX_MASK              0x1
#define PMU_AUXADC_LBAT2_IRQ_EN_MAX_SHIFT             12
#define PMU_AUXADC_LBAT2_EN_MAX_MASK                  0x1
#define PMU_AUXADC_LBAT2_EN_MAX_SHIFT                 13
#define PMU_AUXADC_LBAT2_MAX_IRQ_B_MASK               0x1
#define PMU_AUXADC_LBAT2_MAX_IRQ_B_SHIFT              15
#define PMU_AUXADC_LBAT2_VOLT_MIN_MASK                0xFFF
#define PMU_AUXADC_LBAT2_VOLT_MIN_SHIFT               0
#define PMU_AUXADC_LBAT2_IRQ_EN_MIN_MASK              0x1
#define PMU_AUXADC_LBAT2_IRQ_EN_MIN_SHIFT             12
#define PMU_AUXADC_LBAT2_EN_MIN_MASK                  0x1
#define PMU_AUXADC_LBAT2_EN_MIN_SHIFT                 13
#define PMU_AUXADC_LBAT2_MIN_IRQ_B_MASK               0x1
#define PMU_AUXADC_LBAT2_MIN_IRQ_B_SHIFT              15
#define PMU_AUXADC_LBAT2_DEBOUNCE_COUNT_MAX_MASK      0x1F
#define PMU_AUXADC_LBAT2_DEBOUNCE_COUNT_MAX_SHIFT     0
#define PMU_AUXADC_LBAT2_DEBOUNCE_COUNT_MIN_MASK      0x1F
#define PMU_AUXADC_LBAT2_DEBOUNCE_COUNT_MIN_SHIFT     0
#define PMU_AUXADC_JEITA_IRQ_EN_MASK                  0x1
#define PMU_AUXADC_JEITA_IRQ_EN_SHIFT                 0
#define PMU_AUXADC_JEITA_EN_MASK                      0x1
#define PMU_AUXADC_JEITA_EN_SHIFT                     1
#define PMU_AUXADC_JEITA_HWEN_MASK                    0x1
#define PMU_AUXADC_JEITA_HWEN_SHIFT                   2
#define PMU_AUXADC_JEITA_DET_PRD_SEL_MASK             0x3
#define PMU_AUXADC_JEITA_DET_PRD_SEL_SHIFT            3
#define PMU_AUXADC_JEITA_DEBT_SEL_MASK                0x3
#define PMU_AUXADC_JEITA_DEBT_SEL_SHIFT               5
#define PMU_AUXADC_JEITA_MIPI_DIS_MASK                0x1
#define PMU_AUXADC_JEITA_MIPI_DIS_SHIFT               11
#define PMU_AUXADC_JEITA_VOLT_HOT_MASK                0xFFF
#define PMU_AUXADC_JEITA_VOLT_HOT_SHIFT               0
#define PMU_AUXADC_JEITA_HOT_IRQ_MASK                 0x1
#define PMU_AUXADC_JEITA_HOT_IRQ_SHIFT                15
#define PMU_AUXADC_JEITA_VOLT_WARM_MASK               0xFFF
#define PMU_AUXADC_JEITA_VOLT_WARM_SHIFT              0
#define PMU_AUXADC_JEITA_WARM_IRQ_MASK                0x1
#define PMU_AUXADC_JEITA_WARM_IRQ_SHIFT               15
#define PMU_AUXADC_JEITA_VOLT_COOL_MASK               0xFFF
#define PMU_AUXADC_JEITA_VOLT_COOL_SHIFT              0
#define PMU_AUXADC_JEITA_COOL_IRQ_MASK                0x1
#define PMU_AUXADC_JEITA_COOL_IRQ_SHIFT               15
#define PMU_AUXADC_JEITA_VOLT_COLD_MASK               0xFFF
#define PMU_AUXADC_JEITA_VOLT_COLD_SHIFT              0
#define PMU_AUXADC_JEITA_COLD_IRQ_MASK                0x1
#define PMU_AUXADC_JEITA_COLD_IRQ_SHIFT               15
#define PMU_AUXADC_JEITA_DEBOUNCE_COUNT_HOT_MASK      0x1F
#define PMU_AUXADC_JEITA_DEBOUNCE_COUNT_HOT_SHIFT     0
#define PMU_AUXADC_JEITA_DEBOUNCE_COUNT_WARM_MASK     0x1F
#define PMU_AUXADC_JEITA_DEBOUNCE_COUNT_WARM_SHIFT    0
#define PMU_AUXADC_JEITA_DEBOUNCE_COUNT_COOL_MASK     0x1F
#define PMU_AUXADC_JEITA_DEBOUNCE_COUNT_COOL_SHIFT    0
#define PMU_AUXADC_JEITA_DEBOUNCE_COUNT_COLD_MASK     0x1F
#define PMU_AUXADC_JEITA_DEBOUNCE_COUNT_COLD_SHIFT    0
#define PMU_AUXADC_JEITA_EN_DD_MASK                   0x1
#define PMU_AUXADC_JEITA_EN_DD_SHIFT                  0
#define PMU_AUXADC_VBAT_DET_DEBT_SEL_RECHG_MASK       0x3
#define PMU_AUXADC_VBAT_DET_DEBT_SEL_RECHG_SHIFT      0
#define PMU_AUXADC_VBAT_DET_DEBT_SEL_OVCHG_MASK       0x3
#define PMU_AUXADC_VBAT_DET_DEBT_SEL_OVCHG_SHIFT      2
#define PMU_AUXADC_VBAT_DET_PRD_SEL_MASK              0x3
#define PMU_AUXADC_VBAT_DET_PRD_SEL_SHIFT             0
#define PMU_AUXADC_VBAT_DET_VOLT_RECHG_MASK           0xFFF
#define PMU_AUXADC_VBAT_DET_VOLT_RECHG_SHIFT          0
#define PMU_AUXADC_VBAT_IRQ_EN_MASK                   0x1
#define PMU_AUXADC_VBAT_IRQ_EN_SHIFT                  12
#define PMU_AUXADC_VBAT_EN_ADC_RECHG_MASK             0x1
#define PMU_AUXADC_VBAT_EN_ADC_RECHG_SHIFT            13
#define PMU_AUXADC_VBAT_EN_MODE_SEL_MASK              0x1
#define PMU_AUXADC_VBAT_EN_MODE_SEL_SHIFT             14
#define PMU_AUXADC_VBAT_RECHG_IRQ_B_MASK              0x1
#define PMU_AUXADC_VBAT_RECHG_IRQ_B_SHIFT             15
#define PMU_AUXADC_VBAT_DET_VOLT_OVCHG_MASK           0xFFF
#define PMU_AUXADC_VBAT_DET_VOLT_OVCHG_SHIFT          0
#define PMU_AUXADC_VBAT_VTH_MODE_SEL_RECHG_MASK       0x1
#define PMU_AUXADC_VBAT_VTH_MODE_SEL_RECHG_SHIFT      0
#define PMU_AUXADC_VBAT_VTH_MODE_SEL_OVCHG_MASK       0x1
#define PMU_AUXADC_VBAT_VTH_MODE_SEL_OVCHG_SHIFT      1
#define PMU_AUXADC_VBAT_DEBOUNCE_CNT_LOW_RECHG_MASK   0x3F
#define PMU_AUXADC_VBAT_DEBOUNCE_CNT_LOW_RECHG_SHIFT  0
#define PMU_AUXADC_VBAT_DEBOUNCE_CNT_HIGH_RECHG_MASK  0x3F
#define PMU_AUXADC_VBAT_DEBOUNCE_CNT_HIGH_RECHG_SHIFT 6
#define PMU_AUXADC_VBAT_RECHG_MASK                    0x1
#define PMU_AUXADC_VBAT_RECHG_SHIFT                   15
#define PMU_AUXADC_VBAT_DEBOUNCE_CNT_LOW_OVCHG_MASK   0x3F
#define PMU_AUXADC_VBAT_DEBOUNCE_CNT_LOW_OVCHG_SHIFT  0
#define PMU_AUXADC_VBAT_DEBOUNCE_CNT_HIGH_OVCHG_MASK  0x3F
#define PMU_AUXADC_VBAT_DEBOUNCE_CNT_HIGH_OVCHG_SHIFT 6
#define PMU_AUXADC_VBAT_OVCHG_MASK                    0x1
#define PMU_AUXADC_VBAT_OVCHG_SHIFT                   15
#define PMU_AUXADC_VBAT_EN_ADC_RECHG_DD_MASK          0x1
#define PMU_AUXADC_VBAT_EN_ADC_RECHG_DD_SHIFT         0
#define PMU_AUXADC_VBAT_DET_VOLT_RECHG_DD_MASK        0xFFF
#define PMU_AUXADC_VBAT_DET_VOLT_RECHG_DD_SHIFT       0
#define PMU_AUXADC_VBAT_DET_VOLT_OVCHG_DD_MASK        0xFFF
#define PMU_AUXADC_VBAT_DET_VOLT_OVCHG_DD_SHIFT       0
#define PMU_ADCIN_VSEN_MUX_EN_MASK                    0x1
#define PMU_ADCIN_VSEN_MUX_EN_SHIFT                   0
#define PMU_BATON_TDET_EN_MASK                        0x1
#define PMU_BATON_TDET_EN_SHIFT                       1
#define PMU_ADCIN_VSEN_EXT_BATON_EN_MASK              0x1
#define PMU_ADCIN_VSEN_EXT_BATON_EN_SHIFT             2
#define PMU_ADCIN_VBAT_EN_MASK                        0x1
#define PMU_ADCIN_VBAT_EN_SHIFT                       3
#define PMU_ADCIN_VSEN_EN_MASK                        0x1
#define PMU_ADCIN_VSEN_EN_SHIFT                       4
#define PMU_ADCIN_CHR_EN_MASK                         0x1
#define PMU_ADCIN_CHR_EN_SHIFT                        5
#define PMU_VBUF_EN_MASK                              0x1
#define PMU_VBUF_EN_SHIFT                             6
#define PMU_DA_ADCIN_VSEN_MUX_EN_MASK                 0x1
#define PMU_DA_ADCIN_VSEN_MUX_EN_SHIFT                0
#define PMU_DA_BATON_TDET_EN_MASK                     0x1
#define PMU_DA_BATON_TDET_EN_SHIFT                    1
#define PMU_DA_ADCIN_VSEN_EXT_BATON_EN_MASK           0x1
#define PMU_DA_ADCIN_VSEN_EXT_BATON_EN_SHIFT          2
#define PMU_DA_ADCIN_VBAT_EN_MASK                     0x1
#define PMU_DA_ADCIN_VBAT_EN_SHIFT                    3
#define PMU_DA_ADCIN_VSEN_EN_MASK                     0x1
#define PMU_DA_ADCIN_VSEN_EN_SHIFT                    4
#define PMU_DA_ADCIN_CHR_EN_MASK                      0x1
#define PMU_DA_ADCIN_CHR_EN_SHIFT                     5
#define PMU_DA_VBUF_EN_MASK                           0x1
#define PMU_DA_VBUF_EN_SHIFT                          6
#define PMU_RG_CRST_INTV_MASK                         0x3
#define PMU_RG_CRST_INTV_SHIFT                        0
#define PMU_RG_WRST_INTV_MASK                         0x3
#define PMU_RG_WRST_INTV_SHIFT                        2
#define PMU_RG_CRST_MASK                              0x1
#define PMU_RG_CRST_SHIFT                             8
#define PMU_RG_WRST_MASK                              0x1
#define PMU_RG_WRST_SHIFT                             9
#define PMU_RG_PSEQ_LCHOUT_XCPT_MASK_MASK             0x1
#define PMU_RG_PSEQ_LCHOUT_XCPT_MASK_SHIFT            4
#define PMU_RG_PSEQ_RTCA_XCPT_MASK_MASK               0x1
#define PMU_RG_PSEQ_RTCA_XCPT_MASK_SHIFT              5
#define PMU_RG_KEYPWR_VCORE_OPT_MASK                  0x1
#define PMU_RG_KEYPWR_VCORE_OPT_SHIFT                 0
#define PMU_RG_PSEQ_FORCE_ON_MASK                     0x1
#define PMU_RG_PSEQ_FORCE_ON_SHIFT                    0
#define PMU_RG_PSEQ_FORCE_ALL_DOFF_MASK               0x1
#define PMU_RG_PSEQ_FORCE_ALL_DOFF_SHIFT              8
#define PMU_RG_PSEQ_RSV0_MASK                         0xFF
#define PMU_RG_PSEQ_RSV0_SHIFT                        0
#define PMU_DDUVLO_DEB_EN_MASK                        0x1
#define PMU_DDUVLO_DEB_EN_SHIFT                       0
#define PMU_RG_STRUP_OSC_EN_MASK                      0x1
#define PMU_RG_STRUP_OSC_EN_SHIFT                     2
#define PMU_RG_STRUP_OSC_EN_SEL_MASK                  0x1
#define PMU_RG_STRUP_OSC_EN_SEL_SHIFT                 3
#define PMU_RG_STRUP_FT_CTRL_MASK                     0x3
#define PMU_RG_STRUP_FT_CTRL_SHIFT                    4
#define PMU_RG_BIASGEN_FORCE_MASK                     0x1
#define PMU_RG_BIASGEN_FORCE_SHIFT                    7
#define PMU_RG_BIASGEN_MASK                           0x1
#define PMU_RG_BIASGEN_SHIFT                          10
#define PMU_RG_BIASGEN_SEL_MASK                       0x1
#define PMU_RG_BIASGEN_SEL_SHIFT                      11
#define PMU_RG_THR_DET_DIS_MASK                       0x1
#define PMU_RG_THR_DET_DIS_SHIFT                      12
#define PMU_RG_THR_TEST_MASK                          0x3
#define PMU_RG_THR_TEST_SHIFT                         13
#define PMU_RG_STRUP_SMPS_EN_MASK                     0x1
#define PMU_RG_STRUP_SMPS_EN_SHIFT                    0
#define PMU_RG_STRUP_SMPS_EN_SEL_MASK                 0x1
#define PMU_RG_STRUP_SMPS_EN_SEL_SHIFT                1
#define PMU_CLR_JUST_RST_MASK                         0x1
#define PMU_CLR_JUST_RST_SHIFT                        4
#define PMU_RG_STRUP_PWRKEY_COUNT_RESET_MASK          0x1
#define PMU_RG_STRUP_PWRKEY_COUNT_RESET_SHIFT         8
#define PMU_JUST_PWRKEY_RST_MASK                      0x1
#define PMU_JUST_PWRKEY_RST_SHIFT                     14
#define PMU_DA_QI_OSC_EN_MASK                         0x1
#define PMU_DA_QI_OSC_EN_SHIFT                        15
#define PMU_RG_STRUP_VRF11_PG_H2L_EN_MASK             0x1
#define PMU_RG_STRUP_VRF11_PG_H2L_EN_SHIFT            7
#define PMU_RG_STRUP_VSRAM_PG_H2L_EN_MASK             0x1
#define PMU_RG_STRUP_VSRAM_PG_H2L_EN_SHIFT            8
#define PMU_RG_STRUP_VLDO33_PG_H2L_EN_MASK            0x1
#define PMU_RG_STRUP_VLDO33_PG_H2L_EN_SHIFT           9
#define PMU_RG_STRUP_VA18_PG_H2L_EN_MASK              0x1
#define PMU_RG_STRUP_VA18_PG_H2L_EN_SHIFT             11
#define PMU_RG_STRUP_VRF_PG_H2L_EN_MASK               0x1
#define PMU_RG_STRUP_VRF_PG_H2L_EN_SHIFT              12
#define PMU_RG_STRUP_VAUD18_PG_H2L_EN_MASK            0x1
#define PMU_RG_STRUP_VAUD18_PG_H2L_EN_SHIFT           13
#define PMU_RG_STRUP_VIO18_PG_H2L_EN_MASK             0x1
#define PMU_RG_STRUP_VIO18_PG_H2L_EN_SHIFT            14
#define PMU_RG_STRUP_VCORE_PG_H2L_EN_MASK             0x1
#define PMU_RG_STRUP_VCORE_PG_H2L_EN_SHIFT            15
#define PMU_RG_STRUP_VRF11_PG_ENB_MASK                0x1
#define PMU_RG_STRUP_VRF11_PG_ENB_SHIFT               7
#define PMU_RG_STRUP_VSRAM_PG_ENB_MASK                0x1
#define PMU_RG_STRUP_VSRAM_PG_ENB_SHIFT               8
#define PMU_RG_STRUP_VLDO33_PG_ENB_MASK               0x1
#define PMU_RG_STRUP_VLDO33_PG_ENB_SHIFT              9
#define PMU_RG_STRUP_VA18_PG_ENB_MASK                 0x1
#define PMU_RG_STRUP_VA18_PG_ENB_SHIFT                11
#define PMU_RG_STRUP_VRF_PG_ENB_MASK                  0x1
#define PMU_RG_STRUP_VRF_PG_ENB_SHIFT                 12
#define PMU_RG_STRUP_VAUD18_PG_ENB_MASK               0x1
#define PMU_RG_STRUP_VAUD18_PG_ENB_SHIFT              13
#define PMU_RG_STRUP_VIO18_PG_ENB_MASK                0x1
#define PMU_RG_STRUP_VIO18_PG_ENB_SHIFT               14
#define PMU_RG_STRUP_VCORE_PG_ENB_MASK                0x1
#define PMU_RG_STRUP_VCORE_PG_ENB_SHIFT               15
#define PMU_RG_RTC_ALARM_DEB_EN_MASK                  0x1
#define PMU_RG_RTC_ALARM_DEB_EN_SHIFT                 9
#define PMU_RG_STRUP_VRF_OC_ENB_MASK                  0x1
#define PMU_RG_STRUP_VRF_OC_ENB_SHIFT                 12
#define PMU_RG_STRUP_VAUD18_OC_ENB_MASK               0x1
#define PMU_RG_STRUP_VAUD18_OC_ENB_SHIFT              13
#define PMU_RG_STRUP_VIO18_OC_ENB_MASK                0x1
#define PMU_RG_STRUP_VIO18_OC_ENB_SHIFT               14
#define PMU_RG_STRUP_VCORE_OC_ENB_MASK                0x1
#define PMU_RG_STRUP_VCORE_OC_ENB_SHIFT               15
#define PMU_RG_VA18_USA_MASK                          0x1F
#define PMU_RG_VA18_USA_SHIFT                         0
#define PMU_RG_HWZCV_USA_MASK                         0x1F
#define PMU_RG_HWZCV_USA_SHIFT                        8
#define PMU_RG_VSRAM_NDIS_EN_USA_MASK                 0x1F
#define PMU_RG_VSRAM_NDIS_EN_USA_SHIFT                0
#define PMU_RG_VCORE_USA_MASK                         0x1F
#define PMU_RG_VCORE_USA_SHIFT                        8
#define PMU_RG_VIO18_USA_MASK                         0x1F
#define PMU_RG_VIO18_USA_SHIFT                        0
#define PMU_RG_VSRAM_USA_MASK                         0x1F
#define PMU_RG_VSRAM_USA_SHIFT                        8
#define PMU_RG_SRAM_SW_OFF_USA_MASK                   0x1F
#define PMU_RG_SRAM_SW_OFF_USA_SHIFT                  0
#define PMU_RG_VLDO33_USA_MASK                        0x1F
#define PMU_RG_VLDO33_USA_SHIFT                       8
#define PMU_RG_VRF_USA_MASK                           0x1F
#define PMU_RG_VRF_USA_SHIFT                          0
#define PMU_RG_VAUD18_USA_MASK                        0x1F
#define PMU_RG_VAUD18_USA_SHIFT                       8
#define PMU_RG_VRF11_USA_MASK                         0x1F
#define PMU_RG_VRF11_USA_SHIFT                        0
#define PMU_RG_SEQ_LEN_MASK                           0x1F
#define PMU_RG_SEQ_LEN_SHIFT                          0
#define PMU_RG_SLOT_INTV_UP_MASK                      0x3
#define PMU_RG_SLOT_INTV_UP_SHIFT                     9
#define PMU_RG_PSPG_SHDN_EN_MASK                      0x3
#define PMU_RG_PSPG_SHDN_EN_SHIFT                     0
#define PMU_RG_PSEQ_F75K_FORCE_MASK                   0x1
#define PMU_RG_PSEQ_F75K_FORCE_SHIFT                  3
#define PMU_RG_PSEQ_1MS_TK_EXT_MASK                   0x1
#define PMU_RG_PSEQ_1MS_TK_EXT_SHIFT                  4
#define PMU_RG_PSEQ_IVGEN_SEL_MASK                    0x1
#define PMU_RG_PSEQ_IVGEN_SEL_SHIFT                   5
#define PMU_RG_STRUP_LONG_PRESS_RESET_EXTEND_MASK     0x1
#define PMU_RG_STRUP_LONG_PRESS_RESET_EXTEND_SHIFT    6
#define PMU_RG_CPS_S0EXT_ENB_MASK                     0x1
#define PMU_RG_CPS_S0EXT_ENB_SHIFT                    7
#define PMU_RG_CPS_S0EXT_TD_MASK                      0x1
#define PMU_RG_CPS_S0EXT_TD_SHIFT                     8
#define PMU_RG_SDN_DLY_ENB_MASK                       0x1
#define PMU_RG_SDN_DLY_ENB_SHIFT                      9
#define PMU_RG_CHRDET_DEB_TD_MASK                     0x1
#define PMU_RG_CHRDET_DEB_TD_SHIFT                    10
#define PMU_EFUSE_IVGEN_ENB_SEL_MASK                  0x1
#define PMU_EFUSE_IVGEN_ENB_SEL_SHIFT                 12
#define PMU_RG_CO_EFUSE_RSV_MASK                      0xFFFF
#define PMU_RG_CO_EFUSE_RSV_SHIFT                     0
#define PMU_RG_CPS_W_KEY_MASK                         0xFFFF
#define PMU_RG_CPS_W_KEY_SHIFT                        0
#define PMU_RG_SLOT_INTV_DOWN_MASK                    0x3
#define PMU_RG_SLOT_INTV_DOWN_SHIFT                   0
#define PMU_RG_DSEQ_LEN_MASK                          0x1F
#define PMU_RG_DSEQ_LEN_SHIFT                         8
#define PMU_RG_VRF_DSA_MASK                           0x1F
#define PMU_RG_VRF_DSA_SHIFT                          0
#define PMU_RG_VLDO33_DSA_MASK                        0x1F
#define PMU_RG_VLDO33_DSA_SHIFT                       8
#define PMU_RG_SRAM_SW_ON_DSA_MASK                    0x1F
#define PMU_RG_SRAM_SW_ON_DSA_SHIFT                   0
#define PMU_RG_VSRAM_DSA_MASK                         0x1F
#define PMU_RG_VSRAM_DSA_SHIFT                        8
#define PMU_RG_VIO18_DSA_MASK                         0x1F
#define PMU_RG_VIO18_DSA_SHIFT                        0
#define PMU_RG_VSRAM_NDIS_EN_DSA_MASK                 0x1F
#define PMU_RG_VSRAM_NDIS_EN_DSA_SHIFT                8
#define PMU_RG_VCORE_DSA_MASK                         0x1F
#define PMU_RG_VCORE_DSA_SHIFT                        0
#define PMU_RG_HWZCV_DSA_MASK                         0x1F
#define PMU_RG_HWZCV_DSA_SHIFT                        8
#define PMU_RG_VA18_DSA_MASK                          0x1F
#define PMU_RG_VA18_DSA_SHIFT                         0
#define PMU_RG_VAUD18_DSA_MASK                        0x1F
#define PMU_RG_VAUD18_DSA_SHIFT                       8
#define PMU_RG_VRF11_DSA_MASK                         0x1F
#define PMU_RG_VRF11_DSA_SHIFT                        0
#define PMU_ISINK_DIM0_FSEL_MASK                      0xFFFF
#define PMU_ISINK_DIM0_FSEL_SHIFT                     0
#define PMU_ISINK_DIM0_DUTY_MASK                      0x1F
#define PMU_ISINK_DIM0_DUTY_SHIFT                     0
#define PMU_ISINK_CH0_STEP_MASK                       0x7
#define PMU_ISINK_CH0_STEP_SHIFT                      5
#define PMU_ISINK0_RSV1_MASK                          0xF
#define PMU_ISINK0_RSV1_SHIFT                         8
#define PMU_ISINK0_RSV0_MASK                          0xF
#define PMU_ISINK0_RSV0_SHIFT                         12
#define PMU_ISINK_BREATH0_TF2_SEL_MASK                0xF
#define PMU_ISINK_BREATH0_TF2_SEL_SHIFT               0
#define PMU_ISINK_BREATH0_TF1_SEL_MASK                0xF
#define PMU_ISINK_BREATH0_TF1_SEL_SHIFT               4
#define PMU_ISINK_BREATH0_TR2_SEL_MASK                0xF
#define PMU_ISINK_BREATH0_TR2_SEL_SHIFT               8
#define PMU_ISINK_BREATH0_TR1_SEL_MASK                0xF
#define PMU_ISINK_BREATH0_TR1_SEL_SHIFT               12
#define PMU_ISINK_BREATH0_TOFF_SEL_MASK               0xF
#define PMU_ISINK_BREATH0_TOFF_SEL_SHIFT              0
#define PMU_ISINK_BREATH0_TON_SEL_MASK                0xF
#define PMU_ISINK_BREATH0_TON_SEL_SHIFT               4
#define PMU_ISINK_DIM1_FSEL_MASK                      0xFFFF
#define PMU_ISINK_DIM1_FSEL_SHIFT                     0
#define PMU_ISINK_DIM1_DUTY_MASK                      0x1F
#define PMU_ISINK_DIM1_DUTY_SHIFT                     0
#define PMU_ISINK_CH1_STEP_MASK                       0x7
#define PMU_ISINK_CH1_STEP_SHIFT                      5
#define PMU_ISINK1_RSV1_MASK                          0xF
#define PMU_ISINK1_RSV1_SHIFT                         8
#define PMU_ISINK1_RSV0_MASK                          0xF
#define PMU_ISINK1_RSV0_SHIFT                         12
#define PMU_ISINK_BREATH1_TF2_SEL_MASK                0xF
#define PMU_ISINK_BREATH1_TF2_SEL_SHIFT               0
#define PMU_ISINK_BREATH1_TF1_SEL_MASK                0xF
#define PMU_ISINK_BREATH1_TF1_SEL_SHIFT               4
#define PMU_ISINK_BREATH1_TR2_SEL_MASK                0xF
#define PMU_ISINK_BREATH1_TR2_SEL_SHIFT               8
#define PMU_ISINK_BREATH1_TR1_SEL_MASK                0xF
#define PMU_ISINK_BREATH1_TR1_SEL_SHIFT               12
#define PMU_ISINK_BREATH1_TOFF_SEL_MASK               0xF
#define PMU_ISINK_BREATH1_TOFF_SEL_SHIFT              0
#define PMU_ISINK_BREATH1_TON_SEL_MASK                0xF
#define PMU_ISINK_BREATH1_TON_SEL_SHIFT               4
#define PMU_RG_ISINK_TRIM_EN_MASK                     0x1
#define PMU_RG_ISINK_TRIM_EN_SHIFT                    0
#define PMU_RG_ISINK_TRIM_SEL_MASK                    0x3
#define PMU_RG_ISINK_TRIM_SEL_SHIFT                   1
#define PMU_RG_ISINK_RSV_MASK                         0xF
#define PMU_RG_ISINK_RSV_SHIFT                        3
#define PMU_RG_ISINK0_CHOP_EN_MASK                    0x1
#define PMU_RG_ISINK0_CHOP_EN_SHIFT                   7
#define PMU_RG_ISINK1_CHOP_EN_MASK                    0x1
#define PMU_RG_ISINK1_CHOP_EN_SHIFT                   8
#define PMU_RG_ISINK0_DOUBLE_MASK                     0x1
#define PMU_RG_ISINK0_DOUBLE_SHIFT                    9
#define PMU_RG_ISINK1_DOUBLE_MASK                     0x1
#define PMU_RG_ISINK1_DOUBLE_SHIFT                    10
#define PMU_RG_ISINK_TRIM_BIAS_MASK                   0x7
#define PMU_RG_ISINK_TRIM_BIAS_SHIFT                  0
#define PMU_RG_ISINK0_OFF_CYCLE_MASK                  0xFF
#define PMU_RG_ISINK0_OFF_CYCLE_SHIFT                 0
#define PMU_RG_ISINK0_ON_CYCLE_MASK                   0xFF
#define PMU_RG_ISINK0_ON_CYCLE_SHIFT                  8
#define PMU_RG_ISINK1_OFF_CYCLE_MASK                  0xFF
#define PMU_RG_ISINK1_OFF_CYCLE_SHIFT                 0
#define PMU_RG_ISINK1_ON_CYCLE_MASK                   0xFF
#define PMU_RG_ISINK1_ON_CYCLE_SHIFT                  8
#define PMU_ISINK_CH0_EN_MASK                         0x1
#define PMU_ISINK_CH0_EN_SHIFT                        0
#define PMU_ISINK_CH1_EN_MASK                         0x1
#define PMU_ISINK_CH1_EN_SHIFT                        1
#define PMU_ISINK_CHOP0_EN_MASK                       0x1
#define PMU_ISINK_CHOP0_EN_SHIFT                      2
#define PMU_ISINK_CHOP1_EN_MASK                       0x1
#define PMU_ISINK_CHOP1_EN_SHIFT                      3
#define PMU_ISINK_CH0_BIAS_EN_MASK                    0x1
#define PMU_ISINK_CH0_BIAS_EN_SHIFT                   4
#define PMU_ISINK_CH1_BIAS_EN_MASK                    0x1
#define PMU_ISINK_CH1_BIAS_EN_SHIFT                   5
#define PMU_ISINK0_PWM_MODE_MASK                      0x1
#define PMU_ISINK0_PWM_MODE_SHIFT                     0
#define PMU_ISINK1_PWM_MODE_MASK                      0x1
#define PMU_ISINK1_PWM_MODE_SHIFT                     1
#define PMU_ISINK_CH1_MODE_MASK                       0x3
#define PMU_ISINK_CH1_MODE_SHIFT                      4
#define PMU_ISINK_CH0_MODE_MASK                       0x3
#define PMU_ISINK_CH0_MODE_SHIFT                      6
#define PMU_RG_ISINK0_REPEAT_EN_MASK                  0x1
#define PMU_RG_ISINK0_REPEAT_EN_SHIFT                 8
#define PMU_RG_ISINK1_REPEAT_EN_MASK                  0x1
#define PMU_RG_ISINK1_REPEAT_EN_SHIFT                 9
#define PMU_ISINK_RSV_MASK                            0x3
#define PMU_ISINK_RSV_SHIFT                           12
#define PMU_ISINK_CHOP0_SW_MASK                       0x1
#define PMU_ISINK_CHOP0_SW_SHIFT                      14
#define PMU_ISINK_CHOP1_SW_MASK                       0x1
#define PMU_ISINK_CHOP1_SW_SHIFT                      15
#define PMU_AD_NI_ISINK0_STATUS_MASK                  0x1
#define PMU_AD_NI_ISINK0_STATUS_SHIFT                 0
#define PMU_AD_NI_ISINK1_STATUS_MASK                  0x1
#define PMU_AD_NI_ISINK1_STATUS_SHIFT                 1
#define PMU_DA_NI_ISINK0_EN_MASK                      0x1
#define PMU_DA_NI_ISINK0_EN_SHIFT                     2
#define PMU_DA_NI_ISINK1_EN_MASK                      0x1
#define PMU_DA_NI_ISINK1_EN_SHIFT                     3
#define PMU_DA_NI_ISINK0_BIAS_EN_MASK                 0x1
#define PMU_DA_NI_ISINK0_BIAS_EN_SHIFT                4
#define PMU_DA_NI_ISINK1_BIAS_EN_MASK                 0x1
#define PMU_DA_NI_ISINK1_BIAS_EN_SHIFT                5
#define PMU_DA_NI_ISINK0_CHOP_CLK_MASK                0x1
#define PMU_DA_NI_ISINK0_CHOP_CLK_SHIFT               6
#define PMU_DA_NI_ISINK1_CHOP_CLK_MASK                0x1
#define PMU_DA_NI_ISINK1_CHOP_CLK_SHIFT               7
#define PMU_DA_QI_ISINK0_STEP_MASK                    0x7
#define PMU_DA_QI_ISINK0_STEP_SHIFT                   8
#define PMU_DA_QI_ISINK1_STEP_MASK                    0x7
#define PMU_DA_QI_ISINK1_STEP_SHIFT                   11
#define PMU_RG_OTP_PA_MASK                            0xFF
#define PMU_RG_OTP_PA_SHIFT                           0
#define PMU_RG_OTP_PDIN_MASK                          0xFF
#define PMU_RG_OTP_PDIN_SHIFT                         0
#define PMU_RG_OTP_PTM_MASK                           0x3
#define PMU_RG_OTP_PTM_SHIFT                          0
#define PMU_RG_OTP_PWE_MASK                           0x1
#define PMU_RG_OTP_PWE_SHIFT                          0
#define PMU_RG_OTP_PPROG_MASK                         0x1
#define PMU_RG_OTP_PPROG_SHIFT                        0
#define PMU_RG_OTP_PWE_SRC_MASK                       0x1
#define PMU_RG_OTP_PWE_SRC_SHIFT                      0
#define PMU_RG_OTP_PROG_PKEY_MASK                     0xFFFF
#define PMU_RG_OTP_PROG_PKEY_SHIFT                    0
#define PMU_RG_OTP_RD_PKEY_MASK                       0xFFFF
#define PMU_RG_OTP_RD_PKEY_SHIFT                      0
#define PMU_RG_OTP_RD_TRIG_MASK                       0x1
#define PMU_RG_OTP_RD_TRIG_SHIFT                      0
#define PMU_RG_RD_RDY_BYPASS_MASK                     0x1
#define PMU_RG_RD_RDY_BYPASS_SHIFT                    0
#define PMU_RG_SKIP_OTP_OUT_MASK                      0x1
#define PMU_RG_SKIP_OTP_OUT_SHIFT                     0
#define PMU_RG_OTP_RD_SW_MASK                         0x1
#define PMU_RG_OTP_RD_SW_SHIFT                        0
#define PMU_RG_OTP_DOUT_SW_MASK                       0xFFFF
#define PMU_RG_OTP_DOUT_SW_SHIFT                      0
#define PMU_RG_OTP_RD_BUSY_MASK                       0x1
#define PMU_RG_OTP_RD_BUSY_SHIFT                      0
#define PMU_EFUSE_READ_RDY_CO_MASK                    0x1
#define PMU_EFUSE_READ_RDY_CO_SHIFT                   1
#define PMU_RG_OTP_RD_ACK_MASK                        0x1
#define PMU_RG_OTP_RD_ACK_SHIFT                       2
#define PMU_OTP_READ_RDY_DLY_MASK                     0x1
#define PMU_OTP_READ_RDY_DLY_SHIFT                    3
#define PMU_RG_OTP_PA_SW_MASK                         0x7F
#define PMU_RG_OTP_PA_SW_SHIFT                        0
#define PMU_RG_CP_CLK_R_MASK                          0x3
#define PMU_RG_CP_CLK_R_SHIFT                         0
#define PMU_RG_CP_CLK_C_MASK                          0x3
#define PMU_RG_CP_CLK_C_SHIFT                         2
#define PMU_RG_CP_DRVMODE_SEL_MASK                    0x1
#define PMU_RG_CP_DRVMODE_SEL_SHIFT                   4
#define PMU_RG_LOOP_GM_EN_MASK                        0x7F
#define PMU_RG_LOOP_GM_EN_SHIFT                       8
#define PMU_RG_LOOP_TUNE_ICL_MASK                     0xFF
#define PMU_RG_LOOP_TUNE_ICL_SHIFT                    0
#define PMU_RG_LOOP_TUNE_SYSCV_MASK                   0xFF
#define PMU_RG_LOOP_TUNE_SYSCV_SHIFT                  8
#define PMU_RG_LOOP_TUNE_BUSDPM_MASK                  0xFF
#define PMU_RG_LOOP_TUNE_BUSDPM_SHIFT                 0
#define PMU_RG_LOOP_TUNE_ICC_MASK                     0xFF
#define PMU_RG_LOOP_TUNE_ICC_SHIFT                    8
#define PMU_RG_LOOP_TUNE_BATCV_MASK                   0xFF
#define PMU_RG_LOOP_TUNE_BATCV_SHIFT                  0
#define PMU_RG_LOOP_TUNE_SYSDPM_MASK                  0xFF
#define PMU_RG_LOOP_TUNE_SYSDPM_SHIFT                 8
#define PMU_RG_LOOP_TUNE_ICCTHR_MASK                  0xFF
#define PMU_RG_LOOP_TUNE_ICCTHR_SHIFT                 0
#define PMU_RG_LOOP_GM_BIAS_ICL_MASK                  0xF
#define PMU_RG_LOOP_GM_BIAS_ICL_SHIFT                 8
#define PMU_RG_LOOP_GM_BIAS_SYSCV_MASK                0xF
#define PMU_RG_LOOP_GM_BIAS_SYSCV_SHIFT               12
#define PMU_RG_LOOP_GM_BIAS_BUSDPM_MASK               0xF
#define PMU_RG_LOOP_GM_BIAS_BUSDPM_SHIFT              0
#define PMU_RG_LOOP_GM_BIAS_ICC_MASK                  0xF
#define PMU_RG_LOOP_GM_BIAS_ICC_SHIFT                 4
#define PMU_RG_LOOP_GM_BIAS_BATCV_MASK                0xF
#define PMU_RG_LOOP_GM_BIAS_BATCV_SHIFT               8
#define PMU_RG_LOOP_GM_BIAS_SYSDPM_MASK               0xF
#define PMU_RG_LOOP_GM_BIAS_SYSDPM_SHIFT              12
#define PMU_RG_LOOP_GM_BIAS_ICCTHR_MASK               0xF
#define PMU_RG_LOOP_GM_BIAS_ICCTHR_SHIFT              0
#define PMU_RG_LOOP_TAIL_SYSLDO_MASK                  0xF
#define PMU_RG_LOOP_TAIL_SYSLDO_SHIFT                 4
#define PMU_RG_MULTI_CAP_SYSLDO_MASK                  0xF
#define PMU_RG_MULTI_CAP_SYSLDO_SHIFT                 11
#define PMU_RG_LOOP_TAIL_CHRLDO_MASK                  0xF
#define PMU_RG_LOOP_TAIL_CHRLDO_SHIFT                 0
#define PMU_RG_LOOP_RC_CHRLDO_MASK                    0x7
#define PMU_RG_LOOP_RC_CHRLDO_SHIFT                   4
#define PMU_RG_MULTI_CAP_CHRLDO_MASK                  0x7
#define PMU_RG_MULTI_CAP_CHRLDO_SHIFT                 7
#define PMU_RG_ICCTHR_STATUS_SEL_MASK                 0x1
#define PMU_RG_ICCTHR_STATUS_SEL_SHIFT                0
#define PMU_RG_BATCV_STATUS_SEL_MASK                  0x1
#define PMU_RG_BATCV_STATUS_SEL_SHIFT                 1
#define PMU_RG_SYSDPM_STATUS_SEL_MASK                 0x1
#define PMU_RG_SYSDPM_STATUS_SEL_SHIFT                2
#define PMU_RG_SYSCV_STATUS_SEL_MASK                  0x1
#define PMU_RG_SYSCV_STATUS_SEL_SHIFT                 3
#define PMU_RG_BUSDPM_STATUS_SEL_MASK                 0x1
#define PMU_RG_BUSDPM_STATUS_SEL_SHIFT                4
#define PMU_RG_BUSDPM_DELTA_VTH_MASK                  0x3
#define PMU_RG_BUSDPM_DELTA_VTH_SHIFT                 5
#define PMU_RG_LOOP_SYSLDO_SB_DIS_MASK                0xFF
#define PMU_RG_LOOP_SYSLDO_SB_DIS_SHIFT               0
#define PMU_RG_LOOP_CHRLDO_SB_DIS_MASK                0xFF
#define PMU_RG_LOOP_CHRLDO_SB_DIS_SHIFT               8
#define PMU_RG_PRECC_ITH_MASK                         0x3
#define PMU_RG_PRECC_ITH_SHIFT                        0
#define PMU_RG_SYSDPM_VTH_MASK                        0xF
#define PMU_RG_SYSDPM_VTH_SHIFT                       2
#define PMU_RG_ICCTHR_SEL_MASK                        0x7
#define PMU_RG_ICCTHR_SEL_SHIFT                       6
#define PMU_RG_FOFF_SYSRCOMP_MASK                     0x1
#define PMU_RG_FOFF_SYSRCOMP_SHIFT                    10
#define PMU_RG_FOFF_CHRLDO_MCAP_MASK                  0x1
#define PMU_RG_FOFF_CHRLDO_MCAP_SHIFT                 13
#define PMU_RG_ITERM_ITH_MASK                         0x3
#define PMU_RG_ITERM_ITH_SHIFT                        14
#define PMU_RG_ICL_OC_MASK                            0x3F
#define PMU_RG_ICL_OC_SHIFT                           0
#define PMU_RG_FOFF_TIMER_MASK                        0x1
#define PMU_RG_FOFF_TIMER_SHIFT                       6
#define PMU_RG_CLAMP_FOFF_MASK                        0x1
#define PMU_RG_CLAMP_FOFF_SHIFT                       7
#define PMU_RG_REVFET_FOFF_MASK                       0x1
#define PMU_RG_REVFET_FOFF_SHIFT                      8
#define PMU_RG_RSET_DET_CLK_SEL_MASK                  0x1
#define PMU_RG_RSET_DET_CLK_SEL_SHIFT                 9
#define PMU_RG_F_NON_AUTO_RSET_DET_MASK               0x1
#define PMU_RG_F_NON_AUTO_RSET_DET_SHIFT              10
#define PMU_RG_RSET_REF_SEL_MASK                      0xF
#define PMU_RG_RSET_REF_SEL_SHIFT                     11
#define PMU_RG_PPFET_FON_TESTMODE_RSV_MASK            0x3
#define PMU_RG_PPFET_FON_TESTMODE_RSV_SHIFT           0
#define PMU_RG_LCHR_ATESTMODE_MASK                    0xF
#define PMU_RG_LCHR_ATESTMODE_SHIFT                   9
#define PMU_RG_PRECC_DET_MASK                         0x7
#define PMU_RG_PRECC_DET_SHIFT                        13
#define PMU_RG_F_ITERM_EN_MASK                        0x1
#define PMU_RG_F_ITERM_EN_SHIFT                       0
#define PMU_RG_F_NO_GATING_ITERM_MASK                 0x1
#define PMU_RG_F_NO_GATING_ITERM_SHIFT                1
#define PMU_RG_F_PRECC_EN_MASK                        0x1
#define PMU_RG_F_PRECC_EN_SHIFT                       2
#define PMU_RG_TRIM_LMT_MASK                          0xF
#define PMU_RG_TRIM_LMT_SHIFT                         4
#define PMU_RG_EFUSE_ICL_TRIM_MASK                    0x3F
#define PMU_RG_EFUSE_ICL_TRIM_SHIFT                   0
#define PMU_RG_EFUSE_ICL_TUNE_TRIM_MASK               0x3
#define PMU_RG_EFUSE_ICL_TUNE_TRIM_SHIFT              6
#define PMU_RG_EFUSE_BATCV_TRIM_MASK                  0xFF
#define PMU_RG_EFUSE_BATCV_TRIM_SHIFT                 0
#define PMU_RG_EFUSE_FOFF_SYS_OVP_TRIM_MASK           0x1
#define PMU_RG_EFUSE_FOFF_SYS_OVP_TRIM_SHIFT          8
#define PMU_RG_EFUSE_FOFF_BAT_OVP_TRIM_MASK           0x1
#define PMU_RG_EFUSE_FOFF_BAT_OVP_TRIM_SHIFT          9
#define PMU_RG_EFUSE_SYSLDO_RSV_TRIM_MASK             0xFF
#define PMU_RG_EFUSE_SYSLDO_RSV_TRIM_SHIFT            0
#define PMU_RG_EFUSE_CHRLDO_RSV_TRIM_MASK             0xFF
#define PMU_RG_EFUSE_CHRLDO_RSV_TRIM_SHIFT            8
#define PMU_RG_EFUSE_ICC_IDAC_TRIM_MASK               0x3F
#define PMU_RG_EFUSE_ICC_IDAC_TRIM_SHIFT              0
#define PMU_RG_EFUSE_ICC_TRIM_EN_MASK                 0x1
#define PMU_RG_EFUSE_ICC_TRIM_EN_SHIFT                8
#define PMU_RG_EFUSE_IDAC_REVFET_TRIM_MASK            0xFF
#define PMU_RG_EFUSE_IDAC_REVFET_TRIM_SHIFT           0
#define PMU_RG_EFUSE_ITERM_IDAC_TRIM_MASK             0x3F
#define PMU_RG_EFUSE_ITERM_IDAC_TRIM_SHIFT            8
#define PMU_RG_EFUSE_ITERM_TRIM_EN_MASK               0x1
#define PMU_RG_EFUSE_ITERM_TRIM_EN_SHIFT              0
#define PMU_RG_EFUSE_BATOC_IDAC_TRIM_MASK             0x3F
#define PMU_RG_EFUSE_BATOC_IDAC_TRIM_SHIFT            1
#define PMU_RG_EFUSE_BATOC_TRIM_EN_MASK               0x1
#define PMU_RG_EFUSE_BATOC_TRIM_EN_SHIFT              9
#define PMU_RG_EFUSE_RSET_REF_SEL_TRIM_MASK           0x3F
#define PMU_RG_EFUSE_RSET_REF_SEL_TRIM_SHIFT          10
#define PMU_RG_EFUSE_ICC_GAIN_TRIM_MASK               0xFF
#define PMU_RG_EFUSE_ICC_GAIN_TRIM_SHIFT              0
#define PMU_RG_EFUSE_ITERM_GAIN_TRIM_MASK             0xFF
#define PMU_RG_EFUSE_ITERM_GAIN_TRIM_SHIFT            8
#define PMU_RG_EN_HWJEITA_MASK                        0x1
#define PMU_RG_EN_HWJEITA_SHIFT                       0
#define PMU_RG_DISWARMCOOL_MASK                       0x1
#define PMU_RG_DISWARMCOOL_SHIFT                      1
#define PMU_RG_IBAT_TUNE_TRIM_MASK                    0x7
#define PMU_RG_IBAT_TUNE_TRIM_SHIFT                   2
#define PMU_RG_VCV_JW_MASK                            0xF
#define PMU_RG_VCV_JW_SHIFT                           4
#define PMU_RG_VCV_JC_MASK                            0xF
#define PMU_RG_VCV_JC_SHIFT                           8
#define PMU_RG_EN_DIG_THR_MASK                        0x1
#define PMU_RG_EN_DIG_THR_SHIFT                       0
#define PMU_RG_EFUSE_EN_ICL_SFSTR_MASK                0x1
#define PMU_RG_EFUSE_EN_ICL_SFSTR_SHIFT               1
#define PMU_RG_EFUSE_RSET_HIGH_EN_MASK                0x1
#define PMU_RG_EFUSE_RSET_HIGH_EN_SHIFT               2
#define PMU_RG_LCHR_1M_CK_PDN_MASK                    0x1
#define PMU_RG_LCHR_1M_CK_PDN_SHIFT                   0
#define PMU_RG_LCHR_1K_CK_PDN_MASK                    0x1
#define PMU_RG_LCHR_1K_CK_PDN_SHIFT                   1
#define PMU_RG_EN_CHR_MASK                            0x1
#define PMU_RG_EN_CHR_SHIFT                           0
#define PMU_RG_SYSDPM_THR_EN_MASK                     0x1
#define PMU_RG_SYSDPM_THR_EN_SHIFT                    1
#define PMU_RG_FASTON_FLAG_EN_MASK                    0x1
#define PMU_RG_FASTON_FLAG_EN_SHIFT                   2
#define PMU_RG_EN_SAFETY_TMR_PRECC_MASK               0x1
#define PMU_RG_EN_SAFETY_TMR_PRECC_SHIFT              3
#define PMU_RG_EN_SAFETY_TMR_CC_MASK                  0x1
#define PMU_RG_EN_SAFETY_TMR_CC_SHIFT                 4
#define PMU_RG_SAFETY_TMR_CLR_MASK                    0x1
#define PMU_RG_SAFETY_TMR_CLR_SHIFT                   5
#define PMU_RG_EXTEND_TMR_CLR_MASK                    0x1
#define PMU_RG_EXTEND_TMR_CLR_SHIFT                   6
#define PMU_RG_CC_TMR_SEL_MASK                        0x3
#define PMU_RG_CC_TMR_SEL_SHIFT                       0
#define PMU_RG_PRECC_TMR_SEL_MASK                     0x3
#define PMU_RG_PRECC_TMR_SEL_SHIFT                    2
#define PMU_RG_PRE_TMR_SEL_MASK                       0x3
#define PMU_RG_PRE_TMR_SEL_SHIFT                      4
#define PMU_RG_EN_TERM_MASK                           0x1
#define PMU_RG_EN_TERM_SHIFT                          0
#define PMU_RG_ITERM_DB_SEL_MASK                      0x3
#define PMU_RG_ITERM_DB_SEL_SHIFT                     1
#define PMU_RG_ITERM_CHG_CUR_EN_MASK                  0x1
#define PMU_RG_ITERM_CHG_CUR_EN_SHIFT                 4
#define PMU_RG_ITERM_CHG_CUR_DB_SEL_MASK              0x7
#define PMU_RG_ITERM_CHG_CUR_DB_SEL_SHIFT             5
#define PMU_RG_EN_RECHG_MASK                          0x1
#define PMU_RG_EN_RECHG_SHIFT                         0
#define PMU_RG_VRECHG_MASK                            0x3
#define PMU_RG_VRECHG_SHIFT                           1
#define PMU_RG_EN_OVCHG_MASK                          0x1
#define PMU_RG_EN_OVCHG_SHIFT                         3
#define PMU_RG_VOVCHG_MASK                            0x3
#define PMU_RG_VOVCHG_SHIFT                           4
#define PMU_RG_LCHR_FLAG_SEL_MASK                     0x1F
#define PMU_RG_LCHR_FLAG_SEL_SHIFT                    0
#define PMU_RGS_CHR_STATE_SW_MASK                     0x7
#define PMU_RGS_CHR_STATE_SW_SHIFT                    0
#define PMU_RGS_CHR_STATE_MASK                        0xF
#define PMU_RGS_CHR_STATE_SHIFT                       3
#define PMU_RG_CHR_SWJEITA_CTL_MASK                   0x1
#define PMU_RG_CHR_SWJEITA_CTL_SHIFT                  0
#define PMU_RG_CHR_SWJEITA_HOT_CTL_MASK               0x1
#define PMU_RG_CHR_SWJEITA_HOT_CTL_SHIFT              1
#define PMU_RG_CHR_SWJEITA_WARM_CTL_MASK              0x1
#define PMU_RG_CHR_SWJEITA_WARM_CTL_SHIFT             2
#define PMU_RG_CHR_SWJEITA_COOL_CTL_MASK              0x1
#define PMU_RG_CHR_SWJEITA_COOL_CTL_SHIFT             3
#define PMU_RG_CHR_SWJEITA_COLD_CTL_MASK              0x1
#define PMU_RG_CHR_SWJEITA_COLD_CTL_SHIFT             4
#define PMU_RG_FORCE_EOC_SWSEL_MASK                   0x1
#define PMU_RG_FORCE_EOC_SWSEL_SHIFT                  5
#define PMU_RG_FORCE_EOC_EN_MASK                      0x1
#define PMU_RG_FORCE_EOC_EN_SHIFT                     6
#define PMU_RG_LCHR_1S_SPEEDUP_MASK                   0x1
#define PMU_RG_LCHR_1S_SPEEDUP_SHIFT                  7
#define PMU_RG_EN_ICC_MULTIPLE_SFSTR_MASK             0x1
#define PMU_RG_EN_ICC_MULTIPLE_SFSTR_SHIFT            4
#define PMU_RG_ICC_MULTIPLE_ITH_MASK                  0x7
#define PMU_RG_ICC_MULTIPLE_ITH_SHIFT                 5
#define PMU_RG_TSTEP_ICC_MULTIPLE_MASK                0x3
#define PMU_RG_TSTEP_ICC_MULTIPLE_SHIFT               8
#define PMU_RG_EN_ICC_BJT_SFSTR_MASK                  0x1
#define PMU_RG_EN_ICC_BJT_SFSTR_SHIFT                 10
#define PMU_RG_ICC_BJT_ITH_MASK                       0x7
#define PMU_RG_ICC_BJT_ITH_SHIFT                      11
#define PMU_RG_TSTEP_ICC_BJT_MASK                     0x3
#define PMU_RG_TSTEP_ICC_BJT_SHIFT                    14
#define PMU_RG_CHR2BJT_EN_MASK                        0x1
#define PMU_RG_CHR2BJT_EN_SHIFT                       1
#define PMU_RG_DIS_BJT_MASK                           0x1
#define PMU_RG_DIS_BJT_SHIFT                          2
#define PMU_RG_DEBUG_LATCH_CLEAR_MASK                 0x1
#define PMU_RG_DEBUG_LATCH_CLEAR_SHIFT                3
#define PMU_RG_CHARGER_RST_SW_MASK                    0x1
#define PMU_RG_CHARGER_RST_SW_SHIFT                   0
#define PMU_RG_LCHR_STATE_RST_SW_SEL_MASK             0x1
#define PMU_RG_LCHR_STATE_RST_SW_SEL_SHIFT            1
#define PMU_RG_LCHR_STATE_RST_SW_MASK                 0x1
#define PMU_RG_LCHR_STATE_RST_SW_SHIFT                2
#define PMU_DA_QI_WEAKBUS_DB_MASK                     0x1
#define PMU_DA_QI_WEAKBUS_DB_SHIFT                    0
#define PMU_AD_QI_WEAKBUS_MASK                        0x1
#define PMU_AD_QI_WEAKBUS_SHIFT                       1
#define PMU_DA_QI_VBAT_LT_PRECC_DB_MASK               0x1
#define PMU_DA_QI_VBAT_LT_PRECC_DB_SHIFT              2
#define PMU_AD_QI_VBAT_LT_PRECC_MASK                  0x1
#define PMU_AD_QI_VBAT_LT_PRECC_SHIFT                 3
#define PMU_DA_QI_CP_EN_MASK                          0x1
#define PMU_DA_QI_CP_EN_SHIFT                         4
#define PMU_DA_QI_SYS_MODE_EN_MASK                    0x1
#define PMU_DA_QI_SYS_MODE_EN_SHIFT                   5
#define PMU_DA_QI_FASTON_EN_MASK                      0x1
#define PMU_DA_QI_FASTON_EN_SHIFT                     6
#define PMU_DA_QI_CCCV_MODE_EN_MASK                   0x1
#define PMU_DA_QI_CCCV_MODE_EN_SHIFT                  7
#define PMU_AD_QI_CV_MODE_MASK                        0x1
#define PMU_AD_QI_CV_MODE_SHIFT                       8
#define PMU_AD_QI_ITERM_FLAG_MASK                     0x1
#define PMU_AD_QI_ITERM_FLAG_SHIFT                    9
#define PMU_AD_QI_ICC_LT_HALF_MASK                    0x1
#define PMU_AD_QI_ICC_LT_HALF_SHIFT                   10
#define PMU_AD_QI_ICC_LT_TWENTY_MASK                  0x1
#define PMU_AD_QI_ICC_LT_TWENTY_SHIFT                 11
#define PMU_DA_QI_ICC_LT_HALF_DB_MASK                 0x1
#define PMU_DA_QI_ICC_LT_HALF_DB_SHIFT                12
#define PMU_DA_QI_ICC_LT_TWENTY_DB_MASK               0x1
#define PMU_DA_QI_ICC_LT_TWENTY_DB_SHIFT              13
#define PMU_AD_QI_ICL_MODE_MASK                       0x1
#define PMU_AD_QI_ICL_MODE_SHIFT                      14
#define PMU_AD_QI_THR_MODE_MASK                       0x1
#define PMU_AD_QI_THR_MODE_SHIFT                      15
#define PMU_AD_QI_VBUSDPM_MODE_MASK                   0x1
#define PMU_AD_QI_VBUSDPM_MODE_SHIFT                  0
#define PMU_AD_QI_SYSDPM_MODE_MASK                    0x1
#define PMU_AD_QI_SYSDPM_MODE_SHIFT                   1
#define PMU_DA_QI_ICL_MASK                            0xF
#define PMU_DA_QI_ICL_SHIFT                           2
#define PMU_DA_QI_SYS_DISCHR_EN_MASK                  0x1
#define PMU_DA_QI_SYS_DISCHR_EN_SHIFT                 6
#define PMU_AD_QI_VSYS_OVP_MASK                       0x1
#define PMU_AD_QI_VSYS_OVP_SHIFT                      7
#define PMU_AD_QI_VBAT_OVP_MASK                       0x1
#define PMU_AD_QI_VBAT_OVP_SHIFT                      8
#define PMU_DD_QI_FASTON_FLAG_DB_MASK                 0x1
#define PMU_DD_QI_FASTON_FLAG_DB_SHIFT                9
#define PMU_DA_QI_VBAT_OVP_DB_MASK                    0x1
#define PMU_DA_QI_VBAT_OVP_DB_SHIFT                   11
#define PMU_DA_QI_VSYS_OVP_DB_MASK                    0x1
#define PMU_DA_QI_VSYS_OVP_DB_SHIFT                   12
#define PMU_AD_QI_SYS_MODE_MASK                       0x1
#define PMU_AD_QI_SYS_MODE_SHIFT                      13
#define PMU_AD_QI_ICC_MODE_MASK                       0x1
#define PMU_AD_QI_ICC_MODE_SHIFT                      14
#define PMU_AD_QI_FASTON_FLAG_MASK                    0x1
#define PMU_AD_QI_FASTON_FLAG_SHIFT                   15
#define PMU_AD_QI_SYSDPM_LEVEL_FLAG_MASK              0x1
#define PMU_AD_QI_SYSDPM_LEVEL_FLAG_SHIFT             0
#define PMU_AD_QI_RSET_LEVEL1_MASK                    0x1
#define PMU_AD_QI_RSET_LEVEL1_SHIFT                   1
#define PMU_AD_QI_RSET_LEVEL2_MASK                    0x1
#define PMU_AD_QI_RSET_LEVEL2_SHIFT                   2
#define PMU_AD_QI_RSET_LEVEL3_MASK                    0x1
#define PMU_AD_QI_RSET_LEVEL3_SHIFT                   3
#define PMU_AD_QI_RSET_LEVEL4_MASK                    0x1
#define PMU_AD_QI_RSET_LEVEL4_SHIFT                   4
#define PMU_AD_QI_RSET_LEVEL5_MASK                    0x1
#define PMU_AD_QI_RSET_LEVEL5_SHIFT                   5
#define PMU_AD_QI_RSET_LEVEL6_MASK                    0x1
#define PMU_AD_QI_RSET_LEVEL6_SHIFT                   6
#define PMU_AD_QI_RSET_LEVEL7_MASK                    0x1
#define PMU_AD_QI_RSET_LEVEL7_SHIFT                   7
#define PMU_AD_QI_RSET_LEVEL8_MASK                    0x1
#define PMU_AD_QI_RSET_LEVEL8_SHIFT                   8
#define PMU_AD_QI_SHORT_RSET_MASK                     0x1
#define PMU_AD_QI_SHORT_RSET_SHIFT                    9
#define PMU_AD_QI_VALID_RSET_MASK                     0x1
#define PMU_AD_QI_VALID_RSET_SHIFT                    10
#define PMU_AD_QI_VREF_SET_OK_MASK                    0x1
#define PMU_AD_QI_VREF_SET_OK_SHIFT                   11
#define PMU_AD_QI_RSET_HIGH_MASK                      0x1
#define PMU_AD_QI_RSET_HIGH_SHIFT                     12
#define PMU_DA_QI_PMU_UVLO_CHR_DB_MASK                0x1
#define PMU_DA_QI_PMU_UVLO_CHR_DB_SHIFT               13
#define PMU_DA_QI_CCCV_REF_EN_MASK                    0x1
#define PMU_DA_QI_CCCV_REF_EN_SHIFT                   14
#define PMU_DA_QI_RSET_DET_EN_MASK                    0x1
#define PMU_DA_QI_RSET_DET_EN_SHIFT                   15
#define PMU_DA_QI_IBAT_TUNE_TRIM_MASK                 0x7
#define PMU_DA_QI_IBAT_TUNE_TRIM_SHIFT                0
#define PMU_DA_QI_VCVREG_VTH_MASK                     0xF
#define PMU_DA_QI_VCVREG_VTH_SHIFT                    3
#define PMU_DA_QI_BATON_WARMCOOL_MODE_MASK            0x1
#define PMU_DA_QI_BATON_WARMCOOL_MODE_SHIFT           7
#define PMU_DA_QI_ICC_MULTIPLE_ITH_MASK               0x7
#define PMU_DA_QI_ICC_MULTIPLE_ITH_SHIFT              8
#define PMU_DA_QI_ICC_BJT_ITH_MASK                    0x7
#define PMU_DA_QI_ICC_BJT_ITH_SHIFT                   11
#define PMU_DA_QI_SYSCV_DVS_EN_MASK                   0x1
#define PMU_DA_QI_SYSCV_DVS_EN_SHIFT                  14
#define PMU_DA_QI_BJT_MODE_EN_MASK                    0x1
#define PMU_DA_QI_BJT_MODE_EN_SHIFT                   15
#define PMU_RG_SYS_DISCHR_EN_SW_MASK                  0x1
#define PMU_RG_SYS_DISCHR_EN_SW_SHIFT                 0
#define PMU_RG_SYS_DISCHR_EN_SW_SEL_MASK              0x1
#define PMU_RG_SYS_DISCHR_EN_SW_SEL_SHIFT             1
#define PMU_RG_WEAKBUS_SW_MASK                        0x1
#define PMU_RG_WEAKBUS_SW_SHIFT                       2
#define PMU_RG_WEAKBUS_SW_SEL_MASK                    0x1
#define PMU_RG_WEAKBUS_SW_SEL_SHIFT                   3
#define PMU_RG_VBAT_LT_PRECC_SW_MASK                  0x1
#define PMU_RG_VBAT_LT_PRECC_SW_SHIFT                 4
#define PMU_RG_VBAT_LT_PRECC_SW_SEL_MASK              0x1
#define PMU_RG_VBAT_LT_PRECC_SW_SEL_SHIFT             5
#define PMU_RG_CV_MODE_SW_MASK                        0x1
#define PMU_RG_CV_MODE_SW_SHIFT                       6
#define PMU_RG_CV_MODE_SW_SEL_MASK                    0x1
#define PMU_RG_CV_MODE_SW_SEL_SHIFT                   7
#define PMU_RG_ITERM_FLAG_SW_MASK                     0x1
#define PMU_RG_ITERM_FLAG_SW_SHIFT                    8
#define PMU_RG_ITERM_FLAG_SW_SEL_MASK                 0x1
#define PMU_RG_ITERM_FLAG_SW_SEL_SHIFT                9
#define PMU_RG_ICC_LT_HALF_SW_MASK                    0x1
#define PMU_RG_ICC_LT_HALF_SW_SHIFT                   10
#define PMU_RG_ICC_LT_HALF_SW_SEL_MASK                0x1
#define PMU_RG_ICC_LT_HALF_SW_SEL_SHIFT               11
#define PMU_RG_ICC_LT_TWENTY_SW_MASK                  0x1
#define PMU_RG_ICC_LT_TWENTY_SW_SHIFT                 12
#define PMU_RG_ICC_LT_TWENTY_SW_SEL_MASK              0x1
#define PMU_RG_ICC_LT_TWENTY_SW_SEL_SHIFT             13
#define PMU_RG_CP_EN_SW_MASK                          0x1
#define PMU_RG_CP_EN_SW_SHIFT                         14
#define PMU_RG_CP_EN_SW_SEL_MASK                      0x1
#define PMU_RG_CP_EN_SW_SEL_SHIFT                     15
#define PMU_RG_ICL_MODE_SW_MASK                       0x1
#define PMU_RG_ICL_MODE_SW_SHIFT                      0
#define PMU_RG_ICL_MODE_SW_SEL_MASK                   0x1
#define PMU_RG_ICL_MODE_SW_SEL_SHIFT                  1
#define PMU_RG_VBUSDPM_MODE_SW_MASK                   0x1
#define PMU_RG_VBUSDPM_MODE_SW_SHIFT                  2
#define PMU_RG_VBUSDPM_MODE_SW_SEL_MASK               0x1
#define PMU_RG_VBUSDPM_MODE_SW_SEL_SHIFT              3
#define PMU_RG_SYSDPM_MODE_SW_MASK                    0x1
#define PMU_RG_SYSDPM_MODE_SW_SHIFT                   4
#define PMU_RG_SYSDPM_MODE_SW_SEL_MASK                0x1
#define PMU_RG_SYSDPM_MODE_SW_SEL_SHIFT               5
#define PMU_RG_THR_MODE_SW_MASK                       0x1
#define PMU_RG_THR_MODE_SW_SHIFT                      6
#define PMU_RG_THR_MODE_SW_SEL_MASK                   0x1
#define PMU_RG_THR_MODE_SW_SEL_SHIFT                  7
#define PMU_RG_VSYS_OVP_SW_MASK                       0x1
#define PMU_RG_VSYS_OVP_SW_SHIFT                      8
#define PMU_RG_VSYS_OVP_SW_SEL_MASK                   0x1
#define PMU_RG_VSYS_OVP_SW_SEL_SHIFT                  9
#define PMU_RG_VBAT_OVP_SW_MASK                       0x1
#define PMU_RG_VBAT_OVP_SW_SHIFT                      10
#define PMU_RG_VBAT_OVP_SW_SEL_MASK                   0x1
#define PMU_RG_VBAT_OVP_SW_SEL_SHIFT                  11
#define PMU_RG_SYS_MODE_SW_MASK                       0x1
#define PMU_RG_SYS_MODE_SW_SHIFT                      12
#define PMU_RG_SYS_MODE_SW_SEL_MASK                   0x1
#define PMU_RG_SYS_MODE_SW_SEL_SHIFT                  13
#define PMU_RG_ICC_MODE_SW_MASK                       0x1
#define PMU_RG_ICC_MODE_SW_SHIFT                      14
#define PMU_RG_ICC_MODE_SW_SEL_MASK                   0x1
#define PMU_RG_ICC_MODE_SW_SEL_SHIFT                  15
#define PMU_RG_FASTON_FLAG_SW_MASK                    0x1
#define PMU_RG_FASTON_FLAG_SW_SHIFT                   0
#define PMU_RG_FASTON_FLAG_SW_SEL_MASK                0x1
#define PMU_RG_FASTON_FLAG_SW_SEL_SHIFT               1
#define PMU_RG_SYS_MODE_EN_SW_MASK                    0x1
#define PMU_RG_SYS_MODE_EN_SW_SHIFT                   4
#define PMU_RG_SYS_MODE_EN_SW_SEL_MASK                0x1
#define PMU_RG_SYS_MODE_EN_SW_SEL_SHIFT               5
#define PMU_RG_FASTON_EN_SW_MASK                      0x1
#define PMU_RG_FASTON_EN_SW_SHIFT                     6
#define PMU_RG_FASTON_EN_SW_SEL_MASK                  0x1
#define PMU_RG_FASTON_EN_SW_SEL_SHIFT                 7
#define PMU_RG_CCCV_MODE_EN_SW_MASK                   0x1
#define PMU_RG_CCCV_MODE_EN_SW_SHIFT                  8
#define PMU_RG_CCCV_MODE_EN_SW_SEL_MASK               0x1
#define PMU_RG_CCCV_MODE_EN_SW_SEL_SHIFT              9
#define PMU_RG_PMU_UVLO_CHR_SW_MASK                   0x1
#define PMU_RG_PMU_UVLO_CHR_SW_SHIFT                  10
#define PMU_RG_PMU_UVLO_CHR_SW_SEL_MASK               0x1
#define PMU_RG_PMU_UVLO_CHR_SW_SEL_SHIFT              11
#define PMU_RG_CCCV_REF_EN_SW_MASK                    0x1
#define PMU_RG_CCCV_REF_EN_SW_SHIFT                   12
#define PMU_RG_CCCV_REF_EN_SW_SEL_MASK                0x1
#define PMU_RG_CCCV_REF_EN_SW_SEL_SHIFT               13
#define PMU_RG_RSET_DET_EN_SW_MASK                    0x1
#define PMU_RG_RSET_DET_EN_SW_SHIFT                   14
#define PMU_RG_RSET_DET_EN_SW_SEL_MASK                0x1
#define PMU_RG_RSET_DET_EN_SW_SEL_SHIFT               15
#define PMU_RG_VCVREG_VTH_SW_MASK                     0xF
#define PMU_RG_VCVREG_VTH_SW_SHIFT                    0
#define PMU_RG_VCVREG_VTH_SW_SEL_MASK                 0x1
#define PMU_RG_VCVREG_VTH_SW_SEL_SHIFT                4
#define PMU_RG_ICL_SW_MASK                            0xF
#define PMU_RG_ICL_SW_SHIFT                           5
#define PMU_RG_ICL_SW_SEL_MASK                        0x1
#define PMU_RG_ICL_SW_SEL_SHIFT                       9
#define PMU_RG_BATON_WARMCOOL_MODE_SW_MASK            0x1
#define PMU_RG_BATON_WARMCOOL_MODE_SW_SHIFT           10
#define PMU_RG_BATON_WARMCOOL_MODE_SW_SEL_MASK        0x1
#define PMU_RG_BATON_WARMCOOL_MODE_SW_SEL_SHIFT       11
#define PMU_RG_IBAT_TUNE_TRIM_SW_MASK                 0x7
#define PMU_RG_IBAT_TUNE_TRIM_SW_SHIFT                12
#define PMU_RG_IBAT_TUNE_TRIM_SW_SEL_MASK             0x1
#define PMU_RG_IBAT_TUNE_TRIM_SW_SEL_SHIFT            15
#define PMU_RG_RSET_HIGH_SW_SEL_MASK                  0x1
#define PMU_RG_RSET_HIGH_SW_SEL_SHIFT                 2
#define PMU_RG_RSET_HIGH_SW_MASK                      0x1
#define PMU_RG_RSET_HIGH_SW_SHIFT                     3
#define PMU_RG_ICC_BJT_ITH_SW_SEL_MASK                0x1
#define PMU_RG_ICC_BJT_ITH_SW_SEL_SHIFT               4
#define PMU_RG_ICC_BJT_ITH_SW_MASK                    0x7
#define PMU_RG_ICC_BJT_ITH_SW_SHIFT                   5
#define PMU_RG_BJT_MODE_EN_SW_SEL_MASK                0x1
#define PMU_RG_BJT_MODE_EN_SW_SEL_SHIFT               8
#define PMU_RG_BJT_MODE_EN_SW_MASK                    0x1
#define PMU_RG_BJT_MODE_EN_SW_SHIFT                   9
#define PMU_RG_SYSCV_DVS_EN_SW_SEL_MASK               0x1
#define PMU_RG_SYSCV_DVS_EN_SW_SEL_SHIFT              10
#define PMU_RG_SYSCV_DVS_EN_SW_MASK                   0x1
#define PMU_RG_SYSCV_DVS_EN_SW_SHIFT                  11
#define PMU_RG_ICC_MULTIPLE_ITH_SW_SEL_MASK           0x1
#define PMU_RG_ICC_MULTIPLE_ITH_SW_SEL_SHIFT          12
#define PMU_RG_ICC_MULTIPLE_ITH_SW_MASK               0x7
#define PMU_RG_ICC_MULTIPLE_ITH_SW_SHIFT              13
#define PMU_CHR_PLUGIN_MASK                           0x1
#define PMU_CHR_PLUGIN_SHIFT                          0
#define PMU_CHR_PLUGOUT_MASK                          0x1
#define PMU_CHR_PLUGOUT_SHIFT                         1
#define PMU_CHR_WEAKBUS_MASK                          0x1
#define PMU_CHR_WEAKBUS_SHIFT                         2
#define PMU_CHR_ICC_LT_HALF_MASK                      0x1
#define PMU_CHR_ICC_LT_HALF_SHIFT                     3
#define PMU_CHR_ICC_LT_TWENTY_MASK                    0x1
#define PMU_CHR_ICC_LT_TWENTY_SHIFT                   4
#define PMU_CHR_BATOVP_MASK                           0x1
#define PMU_CHR_BATOVP_SHIFT                          5
#define PMU_CHR_SYSOVP_MASK                           0x1
#define PMU_CHR_SYSOVP_SHIFT                          6
#define PMU_CHR_VBUSDPM_MASK                          0x1
#define PMU_CHR_VBUSDPM_SHIFT                         7
#define PMU_CHR_BAT_HT_MASK                           0x1
#define PMU_CHR_BAT_HT_SHIFT                          8
#define PMU_CHR_BAT_LT_MASK                           0x1
#define PMU_CHR_BAT_LT_SHIFT                          9
#define PMU_CHR_SAFETMROUT_PRECC_MASK                 0x1
#define PMU_CHR_SAFETMROUT_PRECC_SHIFT                10
#define PMU_CHR_SAFETMROUT_CC_MASK                    0x1
#define PMU_CHR_SAFETMROUT_CC_SHIFT                   11
#define PMU_CHR_ICC_MASK                              0x1
#define PMU_CHR_ICC_SHIFT                             12
#define PMU_CHR_SYSCV_MASK                            0x1
#define PMU_CHR_SYSCV_SHIFT                           13
#define PMU_CHR_BATCV_MASK                            0x1
#define PMU_CHR_BATCV_SHIFT                           14
#define PMU_CHR_RECHG_MASK                            0x1
#define PMU_CHR_RECHG_SHIFT                           15
#define PMU_CHR_FASTON_MASK                           0x1
#define PMU_CHR_FASTON_SHIFT                          0
#define PMU_CHR_STATE_SYS_PWR_ON_PRE_MASK             0x1
#define PMU_CHR_STATE_SYS_PWR_ON_PRE_SHIFT            1
#define PMU_CHR_STATE_SYS_PWR_ON_MASK                 0x1
#define PMU_CHR_STATE_SYS_PWR_ON_SHIFT                2
#define PMU_CHR_STATE_SYS_SHORT_MASK                  0x1
#define PMU_CHR_STATE_SYS_SHORT_SHIFT                 3
#define PMU_CHR_STATE_SUSPEND0_MASK                   0x1
#define PMU_CHR_STATE_SUSPEND0_SHIFT                  4
#define PMU_CHR_STATE_SUSPEND1_MASK                   0x1
#define PMU_CHR_STATE_SUSPEND1_SHIFT                  5
#define PMU_CHR_STATE_TRANSFER0_MASK                  0x1
#define PMU_CHR_STATE_TRANSFER0_SHIFT                 6
#define PMU_CHR_STATE_TRANSFER1_MASK                  0x1
#define PMU_CHR_STATE_TRANSFER1_SHIFT                 7
#define PMU_CHR_STATE_PRECC_MASK                      0x1
#define PMU_CHR_STATE_PRECC_SHIFT                     8
#define PMU_CHR_STATE_FASTCC_MASK                     0x1
#define PMU_CHR_STATE_FASTCC_SHIFT                    9
#define PMU_CHR_STATE_BACKGROUND_MASK                 0x1
#define PMU_CHR_STATE_BACKGROUND_SHIFT                10
#define PMU_CHR_STATE_EOC_MASK                        0x1
#define PMU_CHR_STATE_EOC_SHIFT                       11
#define PMU_CHR_STATE_SYS_PWR_OFF_MASK                0x1
#define PMU_CHR_STATE_SYS_PWR_OFF_SHIFT               12
#define PMU_CHR_STATE_IDLE_MASK                       0x1
#define PMU_CHR_STATE_IDLE_SHIFT                      13
#define PMU_CHR_STATE_SUSPEND2_MASK                   0x1
#define PMU_CHR_STATE_SUSPEND2_SHIFT                  14
#define PMU_CHR_OVCHG_MASK                            0x1
#define PMU_CHR_OVCHG_SHIFT                           15
#define PMU_RG_LCHR_CO_RSV0_MASK                      0xFF
#define PMU_RG_LCHR_CO_RSV0_SHIFT                     0
#define PMU_RG_LCHR_CO_RSV1_MASK                      0xFF
#define PMU_RG_LCHR_CO_RSV1_SHIFT                     8
#define PMU_RG_EFUSE_W_MODE_MASK                      0x1
#define PMU_RG_EFUSE_W_MODE_SHIFT                     0
#define PMU_RG_AUXADC_TEST_MODE_MASK                  0x1
#define PMU_RG_AUXADC_TEST_MODE_SHIFT                 4
#define PMU_RG_NANDTREE_MODE_MASK                     0x1
#define PMU_RG_NANDTREE_MODE_SHIFT                    5
#define PMU_RG_ATST_SMPS_CLK_EN_MASK                  0x1
#define PMU_RG_ATST_SMPS_CLK_EN_SHIFT                 6
#define PMU_RG_ATST_PMU_CLK32K_EN_MASK                0x1
#define PMU_RG_ATST_PMU_CLK32K_EN_SHIFT               7
#define PMU_RG_ATST_RTCMISC_CLK_EN_MASK               0x1
#define PMU_RG_ATST_RTCMISC_CLK_EN_SHIFT              4
#define PMU_RG_ATST_MON_OUT_EN_MASK                   0x1
#define PMU_RG_ATST_MON_OUT_EN_SHIFT                  5
#define PMU_RG_ATPG_INC_OB_MASK                       0x1
#define PMU_RG_ATPG_INC_OB_SHIFT                      6
#define PMU_RG_ATPG_INC_CT_MASK                       0x1
#define PMU_RG_ATPG_INC_CT_SHIFT                      7
#define PMU_RG_VIOPG_SWCTL_MASK                       0x1
#define PMU_RG_VIOPG_SWCTL_SHIFT                      0
#define PMU_RG_SWBAT_SWCTL_MASK                       0x1
#define PMU_RG_SWBAT_SWCTL_SHIFT                      1
#define PMU_RG_IVGEN_SWCTL_MASK                       0x1
#define PMU_RG_IVGEN_SWCTL_SHIFT                      2
#define PMU_RG_OSCEN_SWCTL_MASK                       0x1
#define PMU_RG_OSCEN_SWCTL_SHIFT                      3
#define PMU_RG_BGP_ULP_SWCTL_MASK                     0x1
#define PMU_RG_BGP_ULP_SWCTL_SHIFT                    4
#define PMU_RG_UVLO_U1U2_SWCTL_MASK                   0x1
#define PMU_RG_UVLO_U1U2_SWCTL_SHIFT                  7
#define PMU_RG_VIOPG_SWMD_MASK                        0x1
#define PMU_RG_VIOPG_SWMD_SHIFT                       0
#define PMU_RG_SWBAT_SWMD_MASK                        0x1
#define PMU_RG_SWBAT_SWMD_SHIFT                       1
#define PMU_RG_IVGEN_SWMD_MASK                        0x1
#define PMU_RG_IVGEN_SWMD_SHIFT                       2
#define PMU_RG_OSCEN_SWMD_MASK                        0x1
#define PMU_RG_OSCEN_SWMD_SHIFT                       3
#define PMU_RG_BGP_ULP_SWMD_MASK                      0x1
#define PMU_RG_BGP_ULP_SWMD_SHIFT                     4
#define PMU_RG_UVLO_U1U2_SWMD_MASK                    0x1
#define PMU_RG_UVLO_U1U2_SWMD_SHIFT                   7
#define PMU_RG_INTR_TEST_SEL_MASK                     0x3F
#define PMU_RG_INTR_TEST_SEL_SHIFT                    0
#define PMU_RG_INTR_TEST_EN_MASK                      0x1
#define PMU_RG_INTR_TEST_EN_SHIFT                     7
#define PMU_RG_INTR_TEST_EVENT_MASK                   0x1
#define PMU_RG_INTR_TEST_EVENT_SHIFT                  0


#endif
#endif
