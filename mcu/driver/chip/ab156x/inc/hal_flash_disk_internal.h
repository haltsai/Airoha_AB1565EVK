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
 
#ifndef __FLASH_DISK_INTERNAL_H__
#define __FLASH_DISK_INTERNAL_H__

/*******************************************
*   Include File
********************************************/

#include "hal_flash_disk.h"
#include "hal_flash.h"
#ifdef HAL_FLASH_MODULE_ENABLED
#include "hal_flash_custom_memorydevice.h"
#include "hal_flash_opt.h"
#include "hal_flash_mtd.h"
#include "ab156x.h"
#include <stdio.h>
#include <stdlib.h>
#include "hal_flash_drvflash.h"



#ifndef __SECURITY_OTP__
#define  __SECURITY_OTP__
#define  __SECURITY_OTP_WRITE__
#endif

/*******************************************
*   Structure and Macro Definition
********************************************/
#define RAW_DISK0 0
#define RAW_DISK1 1



// Flash raw disk driver's data
typedef struct {
    uint32_t   DiskSize;
    NOR_MTD_Driver *MTDDriver;       ///< MTD driver
    void    *MTDData;                ///< MTD data
    NOR_MTD_FlashInfo FlashInfo;     ///< Flash information
    bool is_mount;
} NOR_FLASH_DISK_Data;

/*******************************************
*   Function and Variable Definition
********************************************/
#ifdef __NOR_SUPPORT_RAW_DISK__
extern NOR_FLASH_DISK_Data FlashDiskDriveData[NOR_BOOTING_NOR_DISK_NUM];
#endif

extern bool         INT_QueryExceptionStatus(void);
extern uint32_t       BlockIndex(void *DriverData, uint32_t blk_addr);
//extern kal_mutexid      fdm_reclaim_mutex;

hal_flash_status_t get_NOR_init_status(void);
/* Raw disk API */
int32_t readRawDiskData(NOR_FLASH_DISK_Data *D, uint32_t addr, uint8_t *data_ptr, uint32_t len);
#if defined(__NOR_SUPPORT_RAW_DISK__) || defined(__NOR_FULL_DRIVER__)
int32_t writeRawDiskData(NOR_FLASH_DISK_Data *D, uint32_t addr, uint8_t *data_ptr, uint32_t len);
int32_t eraseRawDiskBlock(NOR_FLASH_DISK_Data *D, uint32_t block_baseaddr);
#endif //__NOR_SUPPORT_RAW_DISK__ || __NOR_FULL_DRIVER__

//read flash ID
void NOR_ReadID(const uint16_t CS, volatile uint16_t *BaseAddr, uint16_t *flashid);
//get flash disk size
hal_flash_status_t hal_flash_get_disk_size(uint32_t* size);
//get flash base addrss
uint32_t INT_RetrieveFlashBaseAddr();
//erase chip
int32_t NOR_EraseChip();
int32_t SF_DAL_Erase_chip(void *MTDData, const uint16_t CS);

#ifdef __SECURITY_OTP__

typedef enum {
    OTP_BANK_LB1 = 1,
    OTP_BANK_LB2 = 2,
    OTP_BANK_LB3 = 3
}OTP_BANK_enum;

typedef enum {
    OTP_BANK_UNLOCKED = 0x0,
    OTP_BANK_LB1_LOCKED = 0x1,
    OTP_BANK_LB2_LOCKED = 0x2,
    OTP_BANK_LB1_2_LOCKED = 0x3,
    OTP_BANK_LB3_LOCKED = 0x4,
    OTP_BANK_LB1_3_LOCKED = 0x5,
    OTP_BANK_LB2_3_LOCKED = 0x6,
    OTP_BANK_ALL_LOCKED = 0x7
}OTP_BANK_LOCK_enum;


/**
 * @brief     flash write OTP
 * @param[in]  address is starting address to write from. The address must less than 0x200.
 * @param[in]  length is data length, the length add length need less than 0x200.
 * @param[in]  data is source data to be written.
 * @return
 * #HAL_FLASH_STATUS_OK on success
 */
hal_flash_status_t hal_flash_otp_write(uint32_t address, const uint8_t *data, uint32_t length);

/**
 * @brief     flash read OTP
 * @param[in]  start_address is starting address to read the data from.
 * @param[out]  buffer is place to hold the incoming data.
 * @param[in]  length is the length of the data content.
 * @return
 * #HAL_FLASH_STATUS_OK on success
 */
hal_flash_status_t hal_flash_otp_read(uint32_t start_address, uint8_t *buffer, uint32_t length);

/**
 * @brief     flash read security register
 * @param[in]  security register number.
 * @param[in]   offset is the offset in current bank.
 * @param[out]  buffer is place to hold the incoming data.
 the length add length need less than 0x200.
 * @return
 * #HAL_FLASH_STATUS_OK on success
 */
hal_flash_status_t hal_flash_security_register_read(uint32_t bank,uint32_t offset, uint8_t *buffer, uint32_t length);

/**
 * @brief     flash write security register
 * @param[in]  security register number.
 * @param[in]  offset is the offset in current bank.
 * @param[out] buffer is place to hold the incoming data.
 * @param[in]  length is the length of the data content.
 * @return
 * #HAL_FLASH_STATUS_OK on success
 */
hal_flash_status_t hal_flash_security_register_write(uint32_t bank,uint32_t offset, uint8_t *buffer, uint32_t length);

/**
 * @brief     flash lock OTP, after call this API to lock down OTP area, you can't write data into the OTP area any more.
 * @return
 * #HAL_FLASH_STATUS_OK on success
 */
hal_flash_status_t hal_flash_otp_lock(void);

#if defined(SF_DAL_WINBOND) || defined(SF_DAL_GIGADEVICE)
//get OTP status, return 1 once any of bank is locked
hal_flash_status_t hal_flash_otp_status(uint32_t *lock);
//get OTP total size,it should be 3 * 256 for 1565
hal_flash_status_t hal_flash_otp_size(uint32_t *size);
int SF_DAL_OTPLock_blank(void *MTDData, OTP_BANK_enum bank);
// lock bank
hal_flash_status_t hal_flash_otp_lock_bank(OTP_BANK_enum bank);

//erase OTP bank, it is 0 ~ 2 
hal_flash_status_t hal_flash_otp_erase(uint32_t bank);

//protect the address from start to start + length
//should check the Flash datasheet before used this API.
//at defualt it only supports to protect address from 0 to 128KB.
hal_flash_status_t hal_flash_protect(uint32_t start, uint32_t length);
hal_flash_status_t hal_flash_unprotect(void);
#endif
#endif

#endif
#endif //__FLASH_DISK_H__
