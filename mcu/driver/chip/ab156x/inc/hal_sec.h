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

/*
** $Log: hal_Sec.h $
**
** 01 12 2015 leo.hung
** [DVT][Crypto]
** 1. Update crypto kery from efuse KeK and usecret.
**
** 01 08 2015 leo.hung
** [DVT][Crypto]
** 1. Add key source from efuse.
**
** 12 19 2014 leo.hung
** [DVT][Crypto]
** 1. Update Crypto Ring/Double buffer control.
**
** 12 11 2014 leo.hung
** [DVT][PWM][IRDA][RTC][Crypto][WDT]
** 1. Update PWM, IRDA, RTC Crypto_AES/DES, WDT.
**
** 12 08 2014 leo.hung
** [DVT][PWM][IRDA][RTC][Crypto]
** 1. Update PWM, IRDA, RTC Crypto_AES/DES.
**
** 11 29 2014 leo.hung
** [DVT][IrRx][Crypto][RTC]
** 1. Update IrRx, Crypto, RTC.
**
**
*/

#ifndef __HAL_SEC_H__
#define __HAL_SEC_H__

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

#define IOT_CRYPTO_DECRYPT  (0)
#define IOT_CRYPTO_ENCRYPT  (1)

#define IOT_CRYPTO_ECB      (0)
#define IOT_CRYPTO_CBC      (1)

#define IOT_CRYPTO_HWKEY_EFUSE1          (0)  // < First EFUSE Key >
#define IOT_CRYPTO_HWKEY_EFUSE2          (1)  // < Second EFUSE Key >
#define IOT_CRYPTO_SWKEY                 (2)  // < Software>
#define IOT_CRYPTO_HWKEY_RTL             (3)  // < RTL >
#define IOT_CRYPTO_HWKEY_CKDF1           (4)  // < First EFUSE Key >
#define IOT_CRYPTO_HWKEY_CKDF2           (5)  // < Second EFUSE Key >
#define IOT_CRYPTO_KEY_FEEDBACK          (6)  // < Output Feedback Key>

#define IOT_CRYPTO_KEY_BANK_KEK          (1)
#define IOT_CRYPTO_KEY_BANK_USECRET      (2)

#define IOT_AESOTF_INIT_MODE             (0)
#define IOT_AESOTF_ENABLE_MODE           (1)

#define IOT_AESOTF_HWKEY_EFUSE1          (0)
#define IOT_AESOTF_HWKEY_EFUSE2          (1)
#define IOT_AESOTF_SWKEY                 (2)
#define IOT_AESOTF_HWKEY_CKDF1           (3)
#define IOT_AESOTF_HWKEY_CKDF2           (4)

#define IOT_AESOTF_REGION_0              (0)
#define IOT_AESOTF_REGION_1              (1)
#define IOT_AESOTF_REGION_2              (2)
#define IOT_AESOTF_REGION_3              (3)
#define IOT_AESOTF_REGION_4              (4)
#define IOT_AESOTF_REGION_5              (5)
#define IOT_AESOTF_REGION_6              (6)
#define IOT_AESOTF_REGION_7              (7)
#define IOT_AESOTF_REGION_ALL            (8)
#define IOT_AESOTF_REGION_INIT           (9)

#define IOT_AESOTF_ESC_REGION_0          (0)
#define IOT_AESOTF_ESC_REGION_1          (1)
#define IOT_AESOTF_ESC_REGION_2          (2)
#define IOT_AESOTF_ESC_REGION_3          (3)
#define IOT_AESOTF_ESC_REGION_4          (4)
#define IOT_AESOTF_ESC_REGION_5          (5)
#define IOT_AESOTF_ESC_REGION_6          (6)
#define IOT_AESOTF_ESC_REGION_7          (7)
#define IOT_AESOTF_ESC_REGION_ALL        (8)
#define IOT_AESOTF_ESC_REGION_INIT       (9)

#define IOT_AESOTF_ESC_INIT_MODE         (0)
#define IOT_AESOTF_ESC_ENABLE_MODE       (1)

#define IOT_AESOTF_ESC_HWKEY_EFUSE1      (0)
#define IOT_AESOTF_ESC_HWKEY_EFUSE2      (1)
#define IOT_AESOTF_ESC_SWKEY             (2)
#define IOT_AESOTF_ESC_HWKEY_CKDF1       (3)
#define IOT_AESOTF_ESC_HWKEY_CKDF2       (4)

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

typedef struct _CRYPTO_KEY {
    uint32_t u4Key1;
    uint32_t u4Key2;
    uint32_t u4Key3;
    uint32_t u4Key4;
    uint32_t u4Key5;
    uint32_t u4Key6;
    uint32_t u4Key7;
    uint32_t u4Key8;
} CRYPTO_KEY_T, *P_CRYPTO_KEY_T;

typedef struct _CRYPTO_IV {
    uint32_t u4IV1;
    uint32_t u4IV2;
    uint32_t u4IV3;
    uint32_t u4IV4;
} CRYPTO_IV_T, *P_CRYPTO_IV_T;

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/
enum CryptoEngineID {
    CryptoMD5 = 0,
    CryptoSHA1,
    CryptoSHA224,
    CryptoSHA256,
    CryptoSHA384,
    CryptoSHA512
};

/*
========================================================================
Routine Description:

Note:
========================================================================
*/
extern void halSecSetAESDESKeyBank(uint8_t ucKeyBank);
extern void halSecCryptoKick(void);

extern void halSecDMA1Config(uint8_t ucEnable, uint16_t u2LenWPPT, uint8_t *ptrAddrWPTO);
extern void halSecDMA2Config(uint8_t ucEnable, uint16_t u2LenWPPT, uint8_t *ptrAddrWPTO);

extern void halSecAESConfig(uint8_t *ptrInputText, uint8_t *ptrOuputText, uint32_t u4DataLen, CRYPTO_KEY_T cryptoKey, uint32_t u4KeyLen, uint8_t ucKeySource, uint8_t ucMode, uint8_t ucType, CRYPTO_IV_T cryptoIV);
extern void halSecAES(uint8_t *ptrInputText, uint8_t *ptrOuputText, uint32_t u4DataLen, CRYPTO_KEY_T cryptoKey, uint32_t u4KeyLen, uint8_t ucKeySource, uint8_t ucMode, uint8_t ucType, CRYPTO_IV_T cryptoIV);

extern void halSecDESConfig(uint8_t *ptrInputText, uint8_t *ptrOuputText, uint32_t u4DataLen, CRYPTO_KEY_T cryptoKey, uint32_t u4KeyLen, uint8_t ucKeySource, uint8_t ucMode, uint8_t ucType, CRYPTO_IV_T cryptoIV);
extern void halSecDES(uint8_t *ptrInputText, uint8_t *ptrOuputText, uint32_t u4DataLen, CRYPTO_KEY_T cryptoKey, uint32_t u4KeyLen, uint8_t ucKeySource, uint8_t ucMode, uint8_t ucType, CRYPTO_IV_T cryptoIV);

extern void halSecAES_DMA_Buff(uint8_t *ptrInputText, uint8_t *ptrOuputText, uint32_t u4DataLen, uint16_t u2LenWPPT, uint8_t *ptrAddrWPTO, CRYPTO_KEY_T cryptoKey, uint32_t u4KeyLen, uint8_t ucKeySource, uint8_t ucMode, uint8_t ucType, CRYPTO_IV_T cryptoIV);
extern void halSecAES_DMA_Buff_Dest(uint8_t *ptrInputText, uint8_t *ptrOuputText, uint32_t u4DataLen, uint16_t u2LenWPPT, uint8_t *ptrAddrWPTO, CRYPTO_KEY_T cryptoKey, uint32_t u4KeyLen, uint8_t ucKeySource, uint8_t ucMode, uint8_t ucType, CRYPTO_IV_T cryptoIV);

extern void halSecAESOTFConfig(CRYPTO_KEY_T cryptoKey, uint32_t u4KeyLen, uint8_t ucKeySource, CRYPTO_IV_T cryptoIV, uint8_t ucMode);
extern void halSecAESOTF(CRYPTO_KEY_T cryptoKey, uint32_t u4KeyLen, uint8_t ucKeySource, CRYPTO_IV_T cryptoIV, uint8_t ucMode);
extern void halSecAESOTFSetRegion(uint32_t begin_address, uint32_t end_address, uint8_t region);

extern void halSecAESOTFESCConfig(CRYPTO_KEY_T cryptoKey, uint32_t u4KeyLen, uint8_t ucKeySource, CRYPTO_IV_T cryptoIV, uint8_t ucMode);
extern void halSecAESOTFESC(CRYPTO_KEY_T cryptoKey, uint32_t u4KeyLen, uint8_t ucKeySource, CRYPTO_IV_T cryptoIV, uint8_t ucMode);
extern void halSecAESOTFESCSetRegion(uint32_t begin_address, uint32_t end_address, uint8_t region);

#endif //#ifndef __HAL_SEC_H__
