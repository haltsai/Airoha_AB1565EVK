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

#include "hal_flash.h"
#ifdef HAL_FLASH_MODULE_ENABLED

#define __NOR_LEAKAGE__

#include "memory_attribute.h"
#include "hal_flash_opt.h"
#include "hal_flash_mtd_internal.h"
#include "hal_flash_sf.h"
#include "hal_flash_mtd_sf_dal.h"
#include "hal_flash_drvflash.h"
#include "hal_flash_fs_type.h"
#include "hal_flash_sfi_hw.h"
#include "ab156x.h"
#include "hal_flash_combo_init.h"
#include "hal_flash_custom_memorydevice.h"
#include <stdio.h>
#include "hal_flash_disk_internal.h"

#include "hal_gpt.h"
#include "hal_cache.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_log.h"


ATTR_TEXT_IN_TCM uint32_t ust_get_current_time(void)
{
    uint32_t counter = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &counter);
    return counter;
}

ATTR_TEXT_IN_TCM uint32_t get_current_32K_counter(void)
{
    uint32_t counter = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &counter);
    return counter;
}

ATTR_TEXT_IN_TCM void ust_busy_wait(uint32_t delay_us)
{
    hal_gpt_delay_ms(delay_us);
}


ATTR_TEXT_IN_TCM  uint32_t SaveAndSetIRQMask(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask_special(&mask);
    return mask;

}

ATTR_TEXT_IN_TCM  void RestoreIRQMask(uint32_t mask)
{
    hal_nvic_restore_interrupt_mask_special(mask);
}

int32_t SF_DAL_Init_Common(NOR_MTD_Driver *driver, SF_MTD_Data *D);       // Caller: SF_DAL_Init_Vendor()
int32_t SF_DAL_Init_Vendor(NOR_MTD_Driver *driver, SF_MTD_Data *D);       // Caller: SF_DAL_Init_Vendor()
int32_t SF_DAL_Init_SST(NOR_MTD_Driver *driver, SF_MTD_Data *D);          // Caller: SF_DAL_Init_Vendor()
int32_t SF_DAL_Init_MXIC(NOR_MTD_Driver *driver, SF_MTD_Data *D);         // Caller: SF_DAL_Init_Vendor()
int32_t SF_DAL_Init_EON(NOR_MTD_Driver *driver, SF_MTD_Data *D);          // Caller: SF_DAL_Init_Vendor()
int32_t SF_DAL_Init_WINBOND(NOR_MTD_Driver *driver, SF_MTD_Data *D);      // Caller: SF_DAL_Init_Vendor(), SF_DAL_Init_GIGADEVICE()
int32_t SF_DAL_Init_WINBOND_OTP(NOR_MTD_Driver *driver, SF_MTD_Data *D);  // Caller: SF_DAL_Init_Vendor()
int32_t SF_DAL_Init_GIGADEVICE(NOR_MTD_Driver *driver, SF_MTD_Data *D);   // Caller: SF_DAL_Init_Vendor()
int32_t SF_DAL_Init_GIGADEVICE_OTP(NOR_MTD_Driver *driver, SF_MTD_Data *D);  // Caller: SF_DAL_Init_Vendor()
int32_t SF_DAL_Init_ESMT(NOR_MTD_Driver *driver, SF_MTD_Data *D);         // Caller: SF_DAL_Init_Vendor()

ATTR_TEXT_IN_SYSRAM  int SF_DAL_UnlockBlock_None(void *MTDData)
{
    return FS_NO_ERROR;
}
int SF_DAL_UnlockBlock_SST(void *MTDData);   // Caller: SF_DAL_IOCtrl()
int SF_DAL_UnlockBlock_MXIC(void *MTDData);  // Caller: SF_DAL_IOCtrl()
int SF_DAL_OTPRead_MXIC(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length);
int SF_DAL_OTPWrite_MXIC(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length);
int SF_DAL_OTPLock_MXIC(void *MTDData);
int SF_DAL_FailCheck(void *MTDData);
int SF_DAL_OTPRead_WINBOND(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length);
int SF_DAL_OTPWrite_WINBOND(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length);
int SF_DAL_OTPLock_WINBOND(void *MTDData);
int SF_DAL_OTPRead_GIGADEVICE(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length);
int SF_DAL_OTPWrite_GIGADEVICE(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length);
int SF_DAL_OTPLock_GIGADEVICE(void *MTDData);
int SF_DAL_OTPRead_SST(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length);
int SF_DAL_OTPWrite_SST(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length);
int SF_DAL_OTPLock_SST(void *MTDData);

//-----------------------------------------------------------------------------
// [DAL] Device Driver Componets (Interface to lower driver or physical device)
//-----------------------------------------------------------------------------
void SF_DAL_Dev_EraseBlock(SF_MTD_Data *D, uint32_t addr);
void SF_DAL_Dev_ReadID(SF_MTD_Data *D, uint8_t *id);
void SF_DAL_Dev_WriteEnable(SF_MTD_Data *D);
void SF_DAL_Dev_Resume(SF_MTD_Data *D);
void SF_DAL_Dev_Suspend(SF_MTD_Data *D);
uint32_t SF_DAL_Dev_ReadStatus(SF_MTD_Data *D, uint8_t cmd);
void SF_DAL_Dev_Command(SF_MTD_Data *D, const uint32_t cmd, uint8_t outlen);
void SF_DAL_Dev_Command_Ext(const uint16_t CS, uint8_t *cmd, uint8_t *data, const uint16_t outl, const uint16_t inl);
void SF_DAL_Dev_CommandAddress(SF_MTD_Data *D, const uint32_t cmd, const uint32_t address);

void SF_DAL_FLAG_BUSY_SET(void);
void SF_DAL_FLAG_BUSY_CLR(void);
void SF_DAL_FLAG_SUS_SET(void);
void SF_DAL_FLAG_SUS_CLR(void);


//-----------------------------------------------------------------------------
// [DAL] System Init Stage Functions
//-----------------------------------------------------------------------------
void SF_DAL_Dev_WaitReady_EMIINIT(SF_MTD_Data *D);

//
// Note: Following functions do not have dev lock
//       SF_DAL_CheckDeviceReady()
//       SF_DAL_CheckReadyAndResume()
//       Flash_ReturnReady()
//

//-----------------------------------------------------------------------------
// External Functions
//-----------------------------------------------------------------------------
extern uint32_t EMIINIT_CMEM_NOR_GetUniformBlock(const uint16_t CS);
extern void DelayAWhile_UST(uint32_t delay);  // defined in flash_mtd.c
extern bool   INT_QueryExceptionStatus(void);
extern bool CMEM_EMIINIT_CheckValidDeviceID(uint8_t *id);
extern bool CMEM_CheckValidDeviceID(uint8_t *id);
extern uint32_t custom_get_NORFLASH_Size(void);
extern uint32_t custom_get_fat_addr(void);

extern NOR_FLASH_DISK_Data EntireDiskDriveData;


//-----------------------------------------------------------------------------
// External Variables
//-----------------------------------------------------------------------------
extern uint32_t          PAGE_BUFFER_SIZE;       // (custom_flash.c)
extern bool            NOR_FLASH_BUSY;         // (flash_mtd.c) hould be modified only when I-bit is disabled
extern bool            NOR_FLASH_SUSPENDED;    // (flash_mtd.c) should be modified only when I-bit is disabled
extern uint32_t          Image$$ROM$$Base;

extern NOR_FLASH_DRV_Data  FlashDriveData;         // (custom_flash.c)


typedef enum {
    sf_fix_winbond_cv   = 0x00000001,
    sf_fix_winbond_dw   = 0x00000002,
    sf_fix_mxic_35e     = 0x00000004,
    sf_fix_fidelix_q32a = 0x00000008,
    sf_fix_gigadevice_b = 0x00000010
} Enum_SF_FIX;

#define SF_FIX_WINBOND_DW
#define SF_FIX_MXIC_35E
#define SF_FIX_GIGADEVICE_B


typedef enum {
    SF_256MS = 0x8000,
    SF_512MS = 0x9000,
    SF_1S    = 0xA000,
    SF_2S    = 0xB000,
    SF_4S    = 0xC000,
    SF_8S    = 0xD000,
    SF_16S   = 0xE000,
    SF_32S   = 0xF000,
    SF_BUSY  = 0x0001,
    SF_SUS   = 0x0002
} Enum_SF_NVReg;



#define SF_DAL_FLAG_BUSY()      (NOR_FLASH_BUSY)
#define SF_DAL_FLAG_SUS()       (NOR_FLASH_SUSPENDED)

static uint8_t tmp_data_buffer[BUFFER_PROGRAM_ITERATION_LENGTH * 2];

//-----------------------------------------------------------------------------
// Internal Variables
//-----------------------------------------------------------------------------
uint32_t sf_dal_resume_time = NOR_DEFAULT_RESUME_TIME;

#if !defined(__FUE__) && !defined(__UBL__)
//-----------------------------------------------------------------------------
/*!
  @brief

   Data Compare

  @param[in] Address Pointer to the physical address that to be compared
  @param[in] Data Pointer to the data buffer
  @param[in] Size The length of data to be compared
*/

ATTR_TEXT_IN_SYSRAM  bool CompareData(void *Address, void *Data, uint32_t Length)
{
    uint32_t i;

    if ((((uint32_t)Address & 0x3) == 0) && ((uint32_t)Data & 0x3) == 0) {
        uint32_t *Source = (uint32_t *)Address;
        uint32_t *Dest = (uint32_t *)Data;
        for (i = 0; i < (Length >> SFC_GENERIC_2_BIT_OFFSET); i++) {
            if (Source[i] != Dest[i]) {
                return true;
            }
        }
    } else if (((uint32_t)Address & 0x1) == 0 && ((uint32_t)Data & 0x1) == 0) {
        uint16_t *Source = (uint16_t *)Address;
        uint16_t *Dest = (uint16_t *)Data;
        for (i = 0; i < (Length >> SFC_GENERIC_1_BIT_OFFSET); i++) {
            if (Source[i] != Dest[i]) {
                return true;
            }
        }
    } else {
        uint8_t *Source = (uint8_t *)Address;
        uint8_t *Dest = (uint8_t *)Data;
        for (i = 0; i < Length; i++) {
            if (Source[i] != Dest[i]) {
                return true;
            }
        }
    }
    return false;
}



//-----------------------------------------------------------------------------
/*!
  @brief
    Wait until the deivce is ready
  @remarks
    !NOTE! The API should be invoked only when interrupt is disabled
    Since system calls are not allowed while I bit is disabled,
    the device lock is prohibted.
*/
ATTR_TEXT_IN_TCM void Flash_ReturnReady(void)
{
    SF_MTD_Data         *D = NULL;
    SF_Status            status;
    uint32_t      savedMask;

    savedMask = SaveAndSetIRQMask();

    if ((!SF_DAL_FLAG_SUS()) && SF_DAL_FLAG_BUSY()) {
        D = (SF_MTD_Data *)EntireDiskDriveData.MTDData;
        ASSERT((~D->Signature == (uint32_t)D->RegionInfo));
        status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);

        // if flash is busy, suspend any on-going operations
        if (0 != (status & D->StatusMap[SF_SR_BUSY])) {
            // 1. Issue suspend command
            SF_DAL_Dev_Suspend(D);

            // 2. wait for device ready
            while (1) {
                status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);
                if (0 == (status & D->StatusMap[SF_SR_BUSY])) {
                    break;
                }

            }

#if defined(SF_DAL_MXIC) && defined(SF_FIX_MXIC_35E)
            // erase resume only
            if ((D->sf_dal_fix & sf_fix_mxic_35e) && (D->DriverStatus == SFDRV_ERASING))  {
                // WREN
                SFC_GPRAM->RW_SF_GPRAM_DATA = 0x00000006;
                SFC->RW_SF_MAC_INL = 0;
                SFC->RW_SF_MAC_OUTL = 1; // 1B cmd
                SFI_MacEnable(D->CS);
                SFI_MacWaitReady(D->CS);

                // Program
                SFC_GPRAM->RW_SF_GPRAM_DATA_OF_4 = 0xFFFFFFFF;
                SFC->RW_SF_MAC_OUTL = 6; // 1B cmd, 3B adrr, 2B data
                SFC_GPRAM->RW_SF_GPRAM_DATA = 0x00000002;
                SFI_MacEnable(D->CS);
                SFI_MacWaitReady(D->CS);

                // Wait Ready
                do {
                    SFC_GPRAM->RW_SF_GPRAM_DATA = 0x00000005;
                    SFC->RW_SF_MAC_INL = 1;
                    SFC->RW_SF_MAC_OUTL = 1;
                    SFI_MacEnable(D->CS);
                    SFI_MacWaitReady(D->CS);
                    status = ((SFC_GPRAM->RW_SF_GPRAM_DATA) >> SFC_GENERIC_8_BIT_OFFSET) & SFC_GENERIC_0x1_MASK;
                } while (status);
            }
#endif

            SF_DAL_FLAG_SUS_SET();
        } else {
#if defined(SF_DAL_WINBOND) && defined(SF_FIX_WINBOND_DW)
            if ((D->sf_dal_fix & sf_fix_winbond_dw) && (D->DeviceLockOwner == 0)) {
                status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadSuspendReg);
                if ((0 == (status & D->StatusMap[SF_SR_WSE])) &&
                        (0 == (status & D->StatusMap[SF_SR_WSP]))) {
                    SFI_Dev_Command(D->CS, SF_CMD_WINBOND_RESUME);
                    SFI_Dev_Command(D->CS, SF_CMD_WINBOND_RESUME);
                }
            }
#endif
            SF_DAL_FLAG_BUSY_CLR();
        }
    }

    /* If NOR_FLASH_SUSPENDED==NOR_FLASH_BUSY==TRUE, must unmask SFC AHB Channel 2 and 3 to make SF accessible to them. */
    SFI_MaskAhbChannel(0);

    if ((SFC->RW_SF_MAC_CTL) & SFI_IRQ_ABORT || ((SFC->RW_SF_MAC_CTL) & 0x3000))  {
        log_hal_msgid_error("[Returnready] abort_req_src = 0x%x, abort_req_addr = 0x%x \r\n",2, SFC->RW_SF_MAC_CTL, *(volatile uint32_t *)0xA0020060);

        SFC->RW_SF_MAC_CTL |= SFI_ABORT_CLEAR;
        SFC->RW_SF_MAC_CTL &= (~SFI_ABORT_CLEAR);
    }

    RestoreIRQMask(savedMask);

    return;
}


#else // !(__FUE__) && !(__UBL__)

ATTR_TEXT_IN_RAM void Flash_ReturnReady(void)
{
    return;
}

#endif // __UBL__


#if defined(SF_DAL_SST)
int32_t SF_DAL_Init_SST(NOR_MTD_Driver *driver, SF_MTD_Data *D)
{
    SF_Status          *smap;

    // Status map
    smap = D->StatusMap;
    smap[SF_SR_BUSY] = SF_SST_SR_BUSY;
    smap[SF_SR_WEL]  = SF_SST_SR_WEL;
    smap[SF_SR_WSE]  = SF_SST_SR_WSE;
    smap[SF_SR_WSP]  = SF_SST_SR_WSP;

    // Status Registers
    D->CMD->ReadSuspendReg = SF_CMD_READ_SR;
    D->CMD->ReadSecurityReg = SF_CMD_SST_READ_SECURITY_ID;
    D->SuspendLatency = SF_DAL_DEFAULT_TERS;

    switch (D->FlashID[2])    {
        case 0x28:  // SST WF: 8 Mbit (1 MB), 1.8V
            D->BPRBitCount = 32;
            break;
        case 0x21:  // SST WF: 16 Mbit (2 MB), 1.8V
            D->BPRBitCount = 48;
            break;
        case 0x22:  // SST WF: 32 Mbit (4 MB), 1.8V
        case 0x02:  // SST VF: 32 Mbit (4 MB), 2.8V
            D->BPRBitCount = 80;
            break;
        case 0x23:  // SST WF: 64 Mbit (8 MB), 1.8V
        case 0x03:  // SST VF: 64 Mbit (8 MB), 2.8V
            D->BPRBitCount = 144;
            break;
        default:
            return FS_FLASH_MOUNT_ERROR;
    }

    // Abstract Functions
    D->UnlockBlock = SF_DAL_UnlockBlock_SST;

#ifdef __SECURITY_OTP__
    D->OTPLength = 24;
    D->OTPBase = 8;
    D->OTPRead = SF_DAL_OTPRead_SST;
    D->OTPWrite = SF_DAL_OTPWrite_SST;
    D->OTPLock = SF_DAL_OTPLock_MXIC;
    D->CMD->OTPProg = SF_CMD_SST_PROG_SECURITY_ID;
    D->CMD->OTPRead = SF_CMD_SST_READ_SECURITY_ID;
    D->CMD->OTPLock = SF_CMD_SST_LOCK_SECURITY_ID;
    smap[SF_SR_OTPLOCK] = SF_SST_SR_SEC;
#endif

    return FS_NO_ERROR;
}
#endif  //  defined(SF_DAL_SST)

//-----------------------------------------------------------------------------
/*!
  @brief
   MXIC serial Flash init function, determine the device ID of the MXIC Serial
   NOR Flash, initialize status map and driver abstract

  @param[in] driver Pointer to the driver-interface
  @param[in] data Pointer to the driver-data
  @retval
     FS_FLASH_MOUNT_ERROR: The device is not recognizable
     FS_NO_ERROR: Init successful
  @remarks
     Caller: SF_DAL_Init_Driver()
     MXIC Support compile option "SF_DAL_MXIC"
*/
#if defined(SF_DAL_MXIC)
int32_t SF_DAL_Init_MXIC(NOR_MTD_Driver *driver, SF_MTD_Data *D)
{
    SF_Status          *smap;

    // Status map
    smap = D->StatusMap;
    smap[SF_SR_BUSY] = SF_MXIC_SR_BUSY;
    smap[SF_SR_WEL]  = SF_MXIC_SR_WEL;

    smap[SF_SR_WSE]  = SF_MXIC_SR_WSE;
    smap[SF_SR_WSP]  = SF_MXIC_SR_WSP;

    smap[SF_SR_BP]   = SF_MXIC_SR_BP;
    smap[SF_SR_SRWD] = SF_MXIC_SR_SRWD;
    smap[SF_SR_FAIL] = SF_MXIC_SSR_PFAIL | SF_MXIC_SSR_EFAIL;

    // Status Registers
    D->CMD->ReadSuspendReg = SF_CMD_MXIC_READ_SEC_SR;
    D->CMD->ReadSecurityReg = SF_CMD_MXIC_READ_SEC_SR;
    D->CMD->WriteSecurityReg = SF_CMD_MXIC_WRITE_SEC_SR;
    D->CMD->ReadBPReg = SF_CMD_READ_SR;
    D->CMD->WriteBPReg = SF_CMD_WRITE_SR;
    D->CMD->ReadFailReg = SF_CMD_MXIC_READ_SEC_SR;
    D->SuspendLatency = SF_DAL_MXIC_TERS;

    //__SF_MXIC_KH_FLASH__ will be generated in hal_flash_opt_gen.h for KH25L64 and KH25L128
    //Only MX25L128(0xC2,0x20,0x18) , KH25L64(0xC2,0x25,0x37) use 0x75 and 0x7A as Suspend/Resume command
    if ((D->FlashID[1] == 0x20 && D->FlashID[2] == 0x18)) {
        //KH25L128 should not use 0x75/0x7A command
#if !defined(__SF_MXIC_KH_FLASH__)
        D->CMD->Suspend = SF_CMD_WINBOND_SUSPEND;
        D->CMD->Resume = SF_CMD_WINBOND_RESUME;
#endif
    }
    //Only MX25L128(0xC2,0x20,0x18) , KH25L64(0xC2,0x25,0x37) use 0x75 and 0x7A as Suspend/Resume command
    if ((D->FlashID[1] == 0x25 && D->FlashID[2] == 0x37)) {
        //MX25U64 should not use 0x75/0x7A command
#if defined(__SF_MXIC_KH_FLASH__)
        D->CMD->Suspend = SF_CMD_WINBOND_SUSPEND;
        D->CMD->Resume = SF_CMD_WINBOND_RESUME;
#endif
    }

    //MX25L3291FWJI-09 uses 0x75 and 0x7A as Suspend/Resume command
    if ((D->FlashID[1] == 0x20 && D->FlashID[2] == 0x16)) {
        D->CMD->Suspend = SF_CMD_WINBOND_SUSPEND;
        D->CMD->Resume = SF_CMD_WINBOND_RESUME;
    }

    // Device Data
    // Abstract Functions
    D->UnlockBlock = SF_DAL_UnlockBlock_MXIC;

#ifdef __SECURITY_OTP__
    D->OTPLength = 448; //0x200 - 0x40 = 448
    D->OTPBase = 0x40; //MXIC KGD 6 byte mark from 0x3A to 0x3F, thus OTP program region from 0x40
    D->OTPRead = SF_DAL_OTPRead_MXIC;
    D->OTPWrite = SF_DAL_OTPWrite_MXIC;
    D->OTPLock = SF_DAL_OTPLock_MXIC;
    D->CMD->OTPProg = SF_CMD_PAGE_PROG;
    D->CMD->OTPEnter = SF_CMD_MXIC_ENTER_SECURED_OTP;
    D->CMD->OTPExit = SF_CMD_MXIC_EXIT_SECURED_OTP;
    D->CMD->OTPLock = SF_CMD_MXIC_WRITE_SEC_SR;
    smap[SF_SR_OTPLOCK] = SF_MXIC_SSR_LDSO;
#endif

    return FS_NO_ERROR;
}
#endif  // defined(SF_DAL_MXIC)


//-----------------------------------------------------------------------------
/*!
  @brief
   ESMT serial Flash init function, determine the device ID of the ESMT Serial
   NOR Flash, initialize status map and driver abstract

  @param[in] driver Pointer to the driver-interface
  @param[in] data Pointer to the driver-data
  @retval
     FS_FLASH_MOUNT_ERROR: The device is not recognizable
     FS_NO_ERROR: Init successful
  @remarks
     Caller: SF_DAL_Init_Driver()
     ESMT Support compile option "SF_DAL_ESMT"
*/
#if defined(SF_DAL_ESMT)
int32_t SF_DAL_Init_ESMT(NOR_MTD_Driver *driver, SF_MTD_Data *D)
{
    SF_Status          *smap;

    // Status map
    smap = D->StatusMap;
    smap[SF_SR_BUSY] = SF_MXIC_SR_BUSY;
    smap[SF_SR_WEL]  = SF_MXIC_SR_WEL;

    smap[SF_SR_WSE]  = SF_MXIC_SR_WSE;
    smap[SF_SR_WSP]  = SF_MXIC_SR_WSP;

    smap[SF_SR_BP]   = SF_MXIC_SR_BP;
    smap[SF_SR_SRWD] = SF_MXIC_SR_SRWD;
    smap[SF_SR_FAIL] = SF_MXIC_SSR_PFAIL | SF_MXIC_SSR_EFAIL;

    // Status Registers
    D->CMD->ReadSuspendReg = SF_CMD_MXIC_READ_SEC_SR;
    D->CMD->ReadSecurityReg = SF_CMD_MXIC_READ_SEC_SR;
    D->CMD->WriteSecurityReg = SF_CMD_MXIC_WRITE_SEC_SR;
    D->CMD->ReadBPReg = SF_CMD_READ_SR;
    D->CMD->WriteBPReg = SF_CMD_WRITE_SR;
    D->CMD->ReadFailReg = SF_CMD_MXIC_READ_SEC_SR;
    D->SuspendLatency = SF_DAL_DEFAULT_TERS;

    // Device Data
    // Abstract Functions
    D->UnlockBlock = SF_DAL_UnlockBlock_MXIC;


    return FS_NO_ERROR;
}
#endif  // defined(SF_DAL_ESMT)


//-----------------------------------------------------------------------------
/*!
  @brief
   EON serial Flash init function, determine the device ID of the EON Serial
   NOR Flash, initialize status map and driver abstract

  @param[in] driver Pointer to the driver-interface
  @param[in] data Pointer to the driver-data
  @retval
     FS_FLASH_MOUNT_ERROR: The device is not recognizable
     FS_NO_ERROR: Init successful
  @remarks
     Caller: SF_DAL_Init_Driver()
     EON Support compile option "EON_DAL_MXIC"
*/
#if defined(SF_DAL_EON)

int32_t SF_DAL_Init_EON(NOR_MTD_Driver *driver, SF_MTD_Data *D)
{
    SF_Status          *smap;

    // Status map
    smap = D->StatusMap;
    smap[SF_SR_BUSY] = SF_MXIC_SR_BUSY;
    smap[SF_SR_WEL]  = SF_MXIC_SR_WEL;
    smap[SF_SR_WSE]  = SF_EON_SSR_WSE;
    smap[SF_SR_WSP]  = SF_EON_SSR_WSP;

    smap[SF_SR_BP]   = SF_MXIC_SR_BP;
    smap[SF_SR_SRWD] = SF_MXIC_SR_SRWD;

    // Command Override
    D->CMD->ReadID_QPI = SF_CMD_READ_ID;
    // Status Registers
    D->CMD->ReadSuspendReg = SF_CMD_EON_READ_SUSPEND_SR;
    D->CMD->ReadSecurityReg = SF_CMD_EON_READ_SUSPEND_SR;
    D->CMD->ReadBPReg = SF_CMD_READ_SR;
    D->CMD->WriteBPReg = SF_CMD_WRITE_SR;
    D->SuspendLatency = SF_DAL_DEFAULT_TERS;

    // Device Data
    // Abstract Functions
    D->UnlockBlock = SF_DAL_UnlockBlock_MXIC;

    return FS_NO_ERROR;
}
#endif // defined(SF_DAL_EON)

#if defined(SF_DAL_GIGADEVICE) || defined(SF_DAL_WINBOND)
//-----------------------------------------------------------------------------
/*!
  @brief
   Winbond serial Flash init function, determine the device ID of the WINBOND Serial
   NOR Flash, initialize status map and driver abstract

  @param[in] driver Pointer to the driver-interface
  @param[in] data Pointer to the driver-data
  @retval
     FS_FLASH_MOUNT_ERROR: The device is not recognizable
     FS_NO_ERROR: Init successful
  @remarks
     Caller: SF_DAL_Init_Driver()
     Winbond Support compile option "SF_DAL_WINBOND"
*/
int32_t SF_DAL_Init_WINBOND(NOR_MTD_Driver *driver, SF_MTD_Data *D)
{
    SF_Status          *smap;

    // Status map
    smap = D->StatusMap;
    smap[SF_SR_BUSY] = SF_MXIC_SR_BUSY;
    smap[SF_SR_WEL]  = SF_MXIC_SR_WEL;
    smap[SF_SR_WSE]  = SF_WINBOND_SSR_SUS;
    smap[SF_SR_WSP]  = SF_WINBOND_SSR_SUS;
    smap[SF_SR_BP]   = SF_MXIC_SR_BP;
    smap[SF_SR_SRWD] = SF_MXIC_SR_SRWD;

    // Command Override
    D->CMD->ReadID_QPI = SF_CMD_READ_ID;
    // Status Registers
    D->CMD->ReadSuspendReg = SF_CMD_WINBOND_READ_SUSPEND_SR;
    D->CMD->ReadSecurityReg = SF_CMD_WINBOND_READ_SUSPEND_SR;
    D->CMD->ReadBPReg = SF_CMD_READ_SR;
    D->CMD->WriteBPReg = SF_CMD_WRITE_SR;
    D->CMD->Suspend = SF_CMD_WINBOND_SUSPEND;
    D->CMD->Resume = SF_CMD_WINBOND_RESUME;
    D->SuspendLatency = SF_DAL_WINBOND_TERS;

    // Device Data
    // Abstract Functions
    D->UnlockBlock = SF_DAL_UnlockBlock_None;

    return FS_NO_ERROR;
}
#endif

#if defined(SF_DAL_WINBOND)
int32_t SF_DAL_Init_WINBOND_OTP(NOR_MTD_Driver *driver, SF_MTD_Data *D)
{
#ifdef __SECURITY_OTP__
    SF_Status          *smap;
    // Status map
    smap = D->StatusMap;
    D->OTPLength = 0x300;
    D->OTPBase = 0x100; //Security register 0,3 is reserved for Winbond, ony 1,2 can be used
    D->OTPRead = SF_DAL_OTPRead_WINBOND;
    D->OTPWrite = SF_DAL_OTPWrite_WINBOND;
    D->OTPLock = SF_DAL_OTPLock_WINBOND;
    D->CMD->OTPEnter = SF_CMD_SST_QPIRST;
    D->CMD->OTPExit = SF_CMD_SST_QPIEN;
    D->CMD->OTPProg = SF_CMD_WINBOND_PROG_SECURITY_REG;
    D->CMD->OTPRead = SF_CMD_WINBOND_READ_SECURITY_REG;
    smap[SF_SR_OTPLOCK] = SF_WINBOND_SSR_OTP;
#endif

    return FS_NO_ERROR;
}

#endif // defined(SF_DAL_WINBOND)

#if defined(SF_DAL_GIGADEVICE)
int32_t SF_DAL_Init_GIGADEVICE_OTP(NOR_MTD_Driver *driver, SF_MTD_Data *D)
{
#ifdef __SECURITY_OTP__
    SF_Status          *smap;
    // Status map
    smap = D->StatusMap;
    D->OTPLength = 1024;
    D->OTPBase = 0x100; //Security register 0 is reserved for GigaDevice
    D->OTPRead = SF_DAL_OTPRead_GIGADEVICE;
    D->OTPWrite = SF_DAL_OTPWrite_GIGADEVICE;
    D->OTPLock = SF_DAL_OTPLock_GIGADEVICE;
    D->CMD->OTPEnter = SF_CMD_SST_QPIRST;
    D->CMD->OTPExit = SF_CMD_SST_QPIEN;
    D->CMD->OTPProg = SF_CMD_WINBOND_PROG_SECURITY_REG;
    D->CMD->OTPRead = SF_CMD_WINBOND_READ_SECURITY_REG;
    smap[SF_SR_OTPLOCK] = SF_WINBOND_SSR_OTP;
#endif

    return FS_NO_ERROR;
}
#endif
//-----------------------------------------------------------------------------
/*!
  @brief
   Giga Device serial Flash init function, determine the device ID,
   initialize status map and driver abstract

  @param[in] driver Pointer to the driver-interface
  @param[in] data Pointer to the driver-data
  @retval
     FS_FLASH_MOUNT_ERROR: The device is not recognizable
     FS_NO_ERROR: Init successful
  @remarks
     Caller: SF_DAL_Init_Driver()
     GIGADEVICE Support compile option "SF_DAL_GIGADEVICE"
*/
#if defined(SF_DAL_GIGADEVICE)
int32_t SF_DAL_Init_GIGADEVICE(NOR_MTD_Driver *driver, SF_MTD_Data *D)
{
    SF_Status *smap = D->StatusMap;

    SF_DAL_Init_WINBOND(driver, D);

    // Config status map
    smap[SF_SR_WSE]  = 0;
    smap[SF_SR_WSP]  = 0;
    D->SuspendLatency = SF_DAL_GIGADEVICE_TERS;

    return FS_NO_ERROR;
}
#endif


//-----------------------------------------------------------------------------
/*!
  @brief
   Numonyx serial Flash init function, determine the device ID,
   initialize status map and driver abstract

  @param[in] driver Pointer to the driver-interface
  @param[in] data Pointer to the driver-data
  @retval
     FS_FLASH_MOUNT_ERROR: The device is not recognizable
     FS_NO_ERROR: Init successful
  @remarks
     Caller: SF_DAL_Init_Driver()
     NUMONYX Support compile option "SF_DAL_NUMONYX"
*/

#if defined(SF_DAL_NUMONYX)

int32_t SF_DAL_Init_NUMONYX(NOR_MTD_Driver *driver, SF_MTD_Data *D)
{
    SF_Status          *smap;

    // Status map
    smap = D->StatusMap;
    smap[SF_SR_BUSY] = SF_MXIC_SR_BUSY;
    smap[SF_SR_WEL]  = SF_MXIC_SR_WEL;
    smap[SF_SR_WSE]  = SF_NUMONYX_SSR_WSE;
    smap[SF_SR_WSP]  = SF_NUMONYX_SSR_WSP;
    smap[SF_SR_BP]   = SF_MXIC_SR_BP;
    smap[SF_SR_SRWD] = SF_MXIC_SR_SRWD;

    // Command Override
    D->CMD->ReadID_QPI = SF_CMD_READ_ID;
    // Status Registers
    D->CMD->ReadSuspendReg = SF_CMD_NUMONYX_READ_FLAG_SR;
    D->CMD->ReadSecurityReg = SF_CMD_NUMONYX_READ_LOCK_SR;
    D->CMD->ReadBPReg = SF_CMD_READ_SR;
    D->CMD->WriteBPReg = SF_CMD_WRITE_SR;
    D->CMD->Suspend = SF_CMD_WINBOND_SUSPEND;
    D->CMD->Resume = SF_CMD_WINBOND_RESUME;
    D->SuspendLatency = SF_DAL_DEFAULT_TERS;

    // Abstract Functions
    D->UnlockBlock = SF_DAL_UnlockBlock_None;

    return FS_NO_ERROR;
}

#endif // defined(SF_DAL_Init_NUMONYX)

/*
// Init Function Tempalte
#if defined(SF_DAL_XXX)
int32_t SF_DAL_Init_XXX(NOR_MTD_Driver *driver, SF_MTD_Data *D)
{
    SF_Status          *smap;

    // Status map
    smap = D->StatusMap;

    // Command Override
    // Status Registers
    // Abstract Functions
    return FS_NO_ERROR;
}
#endif // defined(SF_DAL_XXX)
*/


//-----------------------------------------------------------------------------
/*!
  @brief
   Common serial Flash init function, initialize status map and driver abstract.

  @param[in] driver Pointer to the driver-interface
  @param[in] data Pointer to the driver-data

  @remarks
     Caller: SF_DAL_Init_Driver()
*/
int32_t SF_DAL_Init_Common(NOR_MTD_Driver *driver, SF_MTD_Data *D)
{
    // Initialize First Abstract Layer Functions (Interface to NOR FDM)
    driver->MountDevice = SF_DAL_MountDevice;
    driver->ShutDown = SF_DAL_ShutDown;
    driver->MapWindow = MapWindow;
    driver->EraseBlock = SF_DAL_EraseBlock;
    driver->ProgramData = SF_DAL_ProgramData;
    driver->NonBlockEraseBlock = SF_DAL_NonBlockEraseBlock;
    driver->CheckDeviceReady = SF_DAL_CheckDeviceReady;
    driver->SuspendErase = SF_DAL_SuspendErase;
    driver->ResumeErase = SF_DAL_ResumeErase;
    driver->BlankCheck = NULL;

#ifdef __SECURITY_OTP__
    driver->OTPAccess = SF_DAL_OTPAccess;
    driver->OTPQueryLength = SF_DAL_OTPQueryLength;
    D->OTPLength = 0;
#else
    driver->OTPAccess = NULL;
    driver->OTPQueryLength = NULL;
#endif

    driver->IsEraseSuspended = NULL;
    driver->IOCtrl = SF_DAL_IOCtrl;

#if defined(__SFI_4BYTES_ADDRESS__)
    if (custom_get_NORFLASH_Size() > 0x1000000)
    {
        D->AddressCycle = 4;
    } else
#endif
    {
        D->AddressCycle = 3;
    }

    // Initialize Common Deivce Commands
    D->CMD->ReadFailReg = 0;
    D->CMD->ReadID_QPI = SF_CMD_READ_ID_QPI;
    D->CMD->ReadID_SPI = SF_CMD_READ_ID;
    D->CMD->PageProg = SF_CMD_PAGE_PROG;
    D->CMD->Erase4K = SF_CMD_ERASE_SECTOR;
    D->CMD->Erase32K = SF_CMD_ERASE_SMALL_BLOCK;
    D->CMD->Erase64K = SF_CMD_ERASE_BLOCK;
    D->CMD->Suspend = SF_CMD_SUSPEND;
    D->CMD->Resume = SF_CMD_RESUME;
    D->CMD->WriteEnable = SF_CMD_WREN;
    D->CMD->EnterDPD = SF_CMD_ENTER_DPD;
    D->CMD->LeaveDPD = SF_CMD_LEAVE_DPD;

    // Status Registers
    D->CMD->ReadReadyReg = SF_CMD_READ_SR;

    // Block Protection Bits (SST only)
    D->BPRBitCount = 0;

    // Mutex
    D->DeviceLockOwner = 0;
    D->DeviceLock = (uint32_t)NULL;
    D->DeviceLockDepth = 0;
    D->SuspendLatency = SF_DAL_DEFAULT_TERS;

    return FS_NO_ERROR;
}

//-----------------------------------------------------------------------------
/*!
  @brief
   Common serial Flash init function, initialize status map and driver abstract.

  @param[in] driver Pointer to the driver-interface
  @param[in] data Pointer to the driver-data

  @remarks
     Caller: SF_DAL_Init_Driver()
*/
int32_t SF_DAL_Init_Vendor(NOR_MTD_Driver *driver, SF_MTD_Data *D)
{
    int32_t result = FS_FLASH_MOUNT_ERROR;

    switch (D->FlashID[0]) {
#if defined(SF_DAL_SST)
        case SF_DAL_TYPE_SST:
            result = SF_DAL_Init_SST(driver, D);
            break;
#endif
#if defined(SF_DAL_MXIC)
        case SF_DAL_TYPE_MXIC:
#if defined(SF_FIX_MXIC_35E)
            D->sf_dal_fix |= sf_fix_mxic_35e;
#endif
            result = SF_DAL_Init_MXIC(driver, D);
            break;
#endif
#if defined(SF_DAL_ESMT)
        case SF_DAL_TYPE_ESMT:
            result = SF_DAL_Init_ESMT(driver, D);
            break;
#endif
#if defined(SF_DAL_EON)
        case SF_DAL_TYPE_EON:
            result = SF_DAL_Init_EON(driver, D);
            break;
#endif
#if defined(SF_DAL_WINBOND)
        case SF_DAL_TYPE_WINBOND:
#if defined(SF_FIX_WINBOND_DW)
            D->sf_dal_fix |= sf_fix_winbond_dw;
#endif
            result  = SF_DAL_Init_WINBOND(driver, D);
            result |= SF_DAL_Init_WINBOND_OTP(driver, D);
            break;
        case SF_DAL_TYPE_FIDELIX:
#if defined(SF_FIX_FIDELIX_Q32A)
            D->sf_dal_fix |= sf_fix_fidelix_q32a;
#endif
            result = SF_DAL_Init_WINBOND(driver, D);
            break;
#endif
#if defined(SF_DAL_GIGADEVICE)
        case SF_DAL_TYPE_GIGADEVICE:
        case SF_DAL_TYPE_GIGADEVICE_MD:
            if (D->FlashID[1] == 0x40 && D->FlashID[0] != 0x51) { //Enter HPM mode Only for GD 3V device, 3V 8Mbits device has suspend/resume cmd  nor does HPM mode
                D->sf_dal_fix |= sf_fix_gigadevice_b;
            }
            result = SF_DAL_Init_GIGADEVICE(driver, D);
            result |= SF_DAL_Init_GIGADEVICE_OTP(driver, D);
            break;
#endif
#if defined(SF_DAL_NUMONYX)
        case SF_DAL_TYPE_NUMONYX:
        case SF_DAL_TYPE_NUMONYX_W:
            result = SF_DAL_Init_NUMONYX(driver, D);
            break;
#endif
        default:
            result = FS_FLASH_MOUNT_ERROR;
            break;
    }

    return result;
}

//-----------------------------------------------------------------------------
/*!
  @brief
   Driver Abstract Layer Main Init Function

  @param[in] driver Pointer to the driver-interface
  @param[in] data Pointer to the driver-data
  @param[in] baseaddr Base address of the serial Flash
  @param[in] paramter Device parameter (Uniform Block Size)

  @retval
     FS_FLASH_MOUNT_ERROR: The device is not recognizable
     FS_NO_ERROR: Init successful
*/
int32_t SF_DAL_Init_Driver(NOR_MTD_Driver *driver, SF_MTD_Data *D, uint32_t baseaddr, uint32_t parameter)
{
    int32_t result;

    // Check All pointers are valid
    if (driver == NULL || D == NULL) {
        return FS_FLASH_MOUNT_ERROR;
    }

    if (D->CMD == NULL || D->StatusMap == NULL)   {
        return FS_FLASH_MOUNT_ERROR;
    }

    D->BaseAddr = (uint8_t *)baseaddr;
    D->UniformBlock = parameter;

    // Common Initiailzation
    SF_DAL_Init_Common(driver, D);

    // Read Vendor ID
    D->CMD->ReadID_QPI = SF_CMD_READ_ID_QPI;
    SF_DAL_Dev_ReadID(D, D->FlashID);

    if (D->FlashID[0] == 0xFF || D->FlashID[0] == 0x00 || (CMEM_CheckValidDeviceID(D->FlashID) == false))    {
        D->CMD->ReadID_QPI = SF_CMD_READ_ID;
        SF_DAL_Dev_ReadID(D, D->FlashID);
    }

    // Vendor Specific Initialization
    result = SF_DAL_Init_Vendor(driver, D);

    return result;
}


extern bool SFI_Dev_GetUniqueID(uint8_t *buffer);

//-----------------------------------------------------------------------------
// [DAL] First Level Abstract (Interface to upper driver)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*!
  @brief
    MTD Layer Mount Device, unlock all blocks to allow read/write access.

  @param[in] MTDData Pointer to the MTD driver data
  @param[in] Info Pointer to the FlashInfo structure

  @retval
    FS_NO_ERROR: Successful

  @remarks
    The device lock is created in this routine.
    This function will invoke UnlockBlock() functional pointer given in the MTD data.
*/
ATTR_TEXT_IN_RAM int32_t SF_DAL_MountDevice(void *MTDData, void *Info)
{
    NOR_MTD_FlashInfo  *FlashInfo = Info;
    SF_MTD_Data        *D         = MTDData;
    uint32_t          i         = 0;
    //uint8_t buf[8];
    int32_t result;
#if defined(__NOR_FDM5__)
    NORLayoutInfo *Layout = D->LayoutInfo;
    uint32_t TotalPhysicalPages;
    uint32_t TblEntryShift = 0, j = 1;
#endif

#if defined(__NOR_FDM5__)
    // check and assign D->LayoutInfo
    TotalPhysicalPages = (Layout->BlkSize) / (Layout->PageSize) * (Layout->TotalBlks);

    while (j < TotalPhysicalPages) {
        j = j << SFC_GENERIC_1_BIT_OFFSET;
        TblEntryShift++;
    }
    TblEntryShift = TblEntryShift >> SFC_GENERIC_1_BIT_OFFSET;
    if ((TblEntryShift) > 8) {
        TblEntryShift = 8;
    }
    Layout->TblEntrys = SFC_GENERIC_1_BIT_OFFSET << TblEntryShift;

    if ((Layout->TblEntrys * Layout->TotalLSMT) < TotalPhysicalPages) {
        ASSERT_RET(0, 0);
    }

#endif// defined(__NOR_FDM5__)

    // config geometry (FlashInfo)
    FlashInfo->TotalBlocks = 0;

    while (D->RegionInfo[i].BlockSize != 0) {
        FlashInfo->BlockSize[i] = D->RegionInfo[i].BlockSize;
        FlashInfo->RegionBlocks[i] = D->RegionInfo[i].RegionBlocks;
        FlashInfo->TotalBlocks += D->RegionInfo[i].RegionBlocks;

        i++;
    }

    FlashInfo->ActualRegionNum = i;
    FlashInfo->PageSize = 512;

    // Fota and Card DL may directly use SF_DAL_MountDevice to mount MTD in bootloader.
    SF_DAL_Dev_Resume(D); //Issue a resume command because GD has no suspend flags, and device suspend flag is not set yet during WDT.
    // Resume previous suspend operation if any, clear NV busy flag, otherwise, bootrom will wait extra 4 seconds.
    do   {
        result = SF_DAL_CheckDeviceReady(D, 0);
    } while (FS_FLASH_ERASE_BUSY == result);


#if defined(__UBL__)
    D->UnlockBlock(D);
#endif //__UBL__

    return FS_NO_ERROR;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Shutdown the serial Flash, resume and wait any unfinished operations.

  @param[in] MTDData Pointer to the MTD driver data

  @retval
    FS_NO_ERROR: Successful
  @remarks
    This function must be placed on RAM because flash may be busy/resumed inside!
    GPRAM utilization is 1+1. A device lock is not necessary.
*/
ATTR_TEXT_IN_RAM int32_t SF_DAL_ShutDown(void *MTDData)
{
    SF_MTD_Data            *D = MTDData;
    SF_Status               status;
    uint32_t              saved_mask;

        while (1) {
            saved_mask = SaveAndSetIRQMask();

            // resume suspended operation issued by ISR (ESB only)
            if (SF_DAL_FLAG_SUS()) {
                SF_DAL_Dev_Resume(D);
            }

            // read status register
            status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);

            // if flash is ready, break, else loops again
            if (0 == (status & D->StatusMap[SF_SR_BUSY])) {
                SF_DAL_FLAG_BUSY_CLR();
                RestoreIRQMask(saved_mask);
                break;
            } else {
                RestoreIRQMask(saved_mask);
            }
        }

        D->CurrAddr = NULL;

    return FS_NO_ERROR;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Erase / Lock / Unlock a block

  @param[in] MTDData Pointer to the MTD driver data
  @param[in] CtrlAction Enumeration value of control actions
  @param[in] CtrlData Pointer to the control data

  @retval
    FS_NO_ERROR: Successful
  @remarks
    Enumeration of Control actions are listed as follows:
      SF_IOCTRL_UNLOCK_BLOCK
      SF_IOCTRL_ENABLE_QPI (deprecated)
      SF_IOCTRL_CONFIG_BURST_LEN (deprecated)
      SF_IOCTRL_QUERY_READ_COMMAND (deprecated)

  @remarks
    The device lock is inserted in the sub-routines, not in IOCtrl()
*/
int32_t SF_DAL_IOCtrl(void *MTDData, uint32_t CtrlAction, void *CtrlData)
{
    SF_MTD_Data        *D  = MTDData;
    if (CtrlAction == SF_IOCTRL_UNLOCK_BLOCK)     {
        D->UnlockBlock(D);
    } else if (CtrlAction == SF_IOCTRL_GET_PROGRAM_FAIL_HANDLE_TYPE)  {
        if ((D->FlashID[0] == SF_DAL_TYPE_MXIC) && (D->RegionInfo[0].BlockSize == 0x40000))   {
            return SF_DAL_TYPE_MXIC;
        }
    } else {
        //ASSERT_RET(0, NOR_MTD_UNSUPPORTED_FUNCTION);
    }
    return FS_NO_ERROR;
}


ATTR_TEXT_IN_RAM void SF_DAL_FLAG_BUSY_SET()
{
    uint32_t savedMask;
    savedMask = SaveAndSetIRQMask();

    NOR_FLASH_BUSY = true;
    RestoreIRQMask(savedMask);
}
ATTR_TEXT_IN_RAM void SF_DAL_FLAG_BUSY_CLR()
{
    uint32_t savedMask;
    savedMask = SaveAndSetIRQMask();

    NOR_FLASH_BUSY = false;
    SFI_MaskAhbChannel(0);
    RestoreIRQMask(savedMask);
}
ATTR_TEXT_IN_RAM void SF_DAL_FLAG_SUS_SET()
{
    uint32_t savedMask;
    savedMask = SaveAndSetIRQMask();

    NOR_FLASH_SUSPENDED = true;
    SFI_MaskAhbChannel(0);
    RestoreIRQMask(savedMask);
}
ATTR_TEXT_IN_RAM void SF_DAL_FLAG_SUS_CLR()
{
    uint32_t savedMask;
    savedMask = SaveAndSetIRQMask();

    NOR_FLASH_SUSPENDED = false;
    RestoreIRQMask(savedMask);
}

//debug flag
uint32_t sf_erase_flag;
uint32_t sf_program_flag;
uint32_t sf_erase_suspend;
uint32_t sf_wait_count_check_ready;
uint32_t sf_wait_count_check_ready_and_resume;


ATTR_TEXT_IN_RAM int32_t SF_DAL_ProgramData(void *MTDData, void *Address, void *Data, uint32_t Length)
{
    SF_MTD_Data     *D = MTDData;
    uint32_t      savedMask;
    uint32_t      address         = (uint32_t)Address;
    uint8_t      *p_data          = Data;
    uint8_t      *p_data_first;           // to save the last byte for data compare in CheckReadyAndResume()
    int32_t       result          = RESULT_FLASH_BUSY;
    uint32_t      written;                // length for single round program
    uint32_t      length;                 // length for single round program
    uint32_t      cmd1;

    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    /*
     * SFI's GPRAM (General purpose SRAM) only supports 4-byte data transfer.
     * The reason is GPRAM is connected with APB bus which does not support byte access.
     *
     * (Only AHB bus supports data size option HSIZE[]: 8, 16 or 32 bits.
     * Please refer to AMBA 2.0 spec for details.)
     *
     * Thus driver must check source buffer address first and cover non 4-byte aligned case.
     * For example, move source data to a temporary aligned buffer using byte read / write,
     * then move data from aligned buffer to GPRAM using 4-byte data transfer.
     */

    /*
     *  Page program loop
     */
    while (0 != Length) {
        p_data_first    = p_data;
        /*
         * Calculate how many bytes to write this loop
         *
         * !NOTE! BUFFER_PROGRAM_ITERATION_LENGTH should be larger than SF_MAX_PAGE_PROG_SIZE
         */

        // step1. length = calculate the offset to the page boundary
        length = PAGE_BUFFER_SIZE * 2 - (address & (PAGE_BUFFER_SIZE * 2 - 1));

        // stpe2. If program Length is greater than the offset to the page boundary
        length = written = ((Length > length) ? length : Length);

retry:
        do {
            sf_program_flag = 0x1;
            result = SF_DAL_CheckDeviceReady(D, 0);
        } while (FS_FLASH_ERASE_BUSY == result);

        savedMask = SaveAndSetIRQMask();
        if (NOR_FLASH_SUSPENDED) {
            RestoreIRQMask(savedMask);
            goto retry;
        }
        sf_program_flag = 0x2;

        if ((uint32_t)p_data & SFC_GENERIC_FLASH_BANK_MASK) {
            //only copy data to RAM when data is on flash.
            uint32_t j;
            for (j = 0; j < length; j++) {
                tmp_data_buffer[j] = p_data[j];
            }
            p_data_first = tmp_data_buffer;
        }

        D->DriverStatus = SFDRV_PROGRAMMING;
        SF_DAL_Dev_WriteEnable(D);

        // step3. Handling 3/4 bytes address
#if defined(__SFI_4BYTES_ADDRESS__)
        if (D->AddressCycle == 4) {
            cmd1 = SFI_GPRAM_Write_C1A4(D->CMD->PageProg, address, &p_data, &length);
            SFI_GPRAM_Write(8, p_data, length);
        } else
#endif
        {
            cmd1 = SFI_GPRAM_Write_C1A3(D->CMD->PageProg, address);
            SFI_GPRAM_Write(4, p_data, length);
        }

        // step 4: copy bufer to GPRAM
#if defined(SF_DAL_GIGADEVICE)   //For GD 3V device, WE cmd should put into IRQMask Region to avoid enter HPM mode again before Program.
        if (D->sf_dal_fix & sf_fix_gigadevice_b)   {
            SFI_Dev_Command(D->CS, D->CMD->WriteEnable);
        }
#endif
        SFC_GPRAM->RW_SF_GPRAM_DATA = cmd1;
        SFC->RW_SF_MAC_OUTL = (written + D->AddressCycle + 1);
        SFC->RW_SF_MAC_INL = 0;
        SF_DAL_FLAG_BUSY_SET();
        SFI_MacEnable(D->CS);
        SFI_MacWaitReady(D->CS);

#if defined(SF_DAL_WINBOND) && defined(SF_FIX_FIDELIX_Q32A)
        if (D->sf_dal_fix & sf_fix_fidelix_q32a) {
            ust_busy_wait(2);
            SFC->RW_SF_MAC_OUTL = 1;
            SFC_GPRAM->RW_SF_GPRAM_DATA = SF_CMD_WINBOND_SUSPEND;
            SFI_MacEnable(D->CS);
            SFI_MacWaitReady(D->CS);
            ust_busy_wait(6);
            SFC_GPRAM->RW_SF_GPRAM_DATA = SF_CMD_WINBOND_RESUME;
            SFI_MacEnable(D->CS);
            SFI_MacWaitReady(D->CS);
            sf_dal_resume_time = ust_get_current_time();
        }
#endif
        sf_program_flag = 0x3;

        RestoreIRQMask(savedMask);

        do  {
                sf_program_flag = 0x4;
                result = SF_DAL_CheckReadyAndResume(D, (uint32_t)address + written - 1, *(p_data_first + written - 1));
                sf_program_flag = 0x5;
        } while (RESULT_FLASH_BUSY == result);

        if (PAGE_BUFFER_SIZE >= 16) {
        }

        p_data += length;
        Length -= written;
        address += written;

        if (RESULT_FLASH_DONE != result) {
            break;
        }
    }

    D->DriverStatus = SFDRV_READY;
    if (result == RESULT_FLASH_DONE) {
        return FS_NO_ERROR;
    }
    return result;
}

ATTR_TEXT_IN_RAM int32_t SF_DAL_EraseBlock(void *MTDData, uint32_t BlockIndex)
{
    int32_t            result = 0;
    uint32_t           blocksize;
    uint32_t           eraseunit;
    uint32_t           eraseaddr;
    uint32_t           iteration = 1;
    SF_MTD_Data         *D = MTDData;
    uint32_t           erase_block = 0;
    uint32_t           savedMask;
    uint32_t           cur_address;

    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    //blockindex only suppurting 0x0FFFFFFF
    erase_block = ((BlockIndex >> 28) & 0x0F);
    BlockIndex = (BlockIndex & 0xFFFFFFF);

    // 0. wait device ready before issue another erase command
    do {
        sf_erase_flag =0x1;
        result = SF_DAL_CheckDeviceReady(D, BlockIndex);
    } while (FS_FLASH_ERASE_BUSY == result);

    // BlockIndex b31=0: Use Combo Memory Block Size
    if (BlockIndex >> SFC_GENERIC_31_BIT_OFFSET == 0) {
        uint32_t a, b, c;
        {
            blocksize = CMEM_BlockSize((uint32_t)D->CurrAddr);
            D->CurrAddr = (void *)((uint32_t)BlockAddress(D, BlockIndex));
            cur_address = (uint32_t)(D->CurrAddr);
        }

        blocksize = CMEM_BlockSize((uint32_t)D->CurrAddr);  //will get peseudo block size
        // Default Erase Unit (in case of non-pseudo-size block)
        eraseunit = blocksize;

        // Find the smallest erase unit from Block Size
        // a: supported uniform blocks
        // b: pseudo block size
        //
        // consider (4, 32, 64 KB uniform blocks)
        // pseido 8KB block
        // c=0           | c=1       | c=2, erase unit = 2^(2+10)=4KB  | c=3
        // a: 0110 0100  > 0011 0010 > 0001 1001 (a%2==1 save b        > 0000 1100
        // b: 0000 1000  > 0000 0100 > 0000 0010  iteration=2, and c)  > 0000 0001 (b%1==1 stop search)
        a = D->UniformBlock;
        b = (blocksize >> 10);
        //If that erase unit is natively supported by the device, skip pseudo block search.
        if (0 == (a & b)) {
            for (c = 0; (b & 0x1) == 0 ; a = a >> SFC_GENERIC_1_BIT_OFFSET, b = b >> SFC_GENERIC_1_BIT_OFFSET, c++)    {
                if (a & 0x1) {
                    iteration = b;
                    eraseunit = SFC_GENERIC_1_BIT_OFFSET << (c + 10);
                }
            }
        }

        if ((erase_block != 0x1) && (erase_block != 0x2) && (erase_block != 0x4)) {
           //support pseudo block
           switch (eraseunit) {
              case 0x1000:  D->CMD->Erase = D->CMD->Erase4K;  break; // 4KB
              case 0x8000:  D->CMD->Erase = D->CMD->Erase32K; break; // 32KB
              case 0x10000: D->CMD->Erase = D->CMD->Erase64K; break; // 64KB
              default:
                 return (RESULT_FLASH_FAIL);
          }
      } else {
            //SDK api request erase type is high priority
            switch (erase_block) {
                case 0x1:  D->CMD->Erase = D->CMD->Erase4K;  break; // 4KB
                case 0x2:  D->CMD->Erase = D->CMD->Erase32K; break; // 32KB
                case 0x4:  D->CMD->Erase = D->CMD->Erase64K; break; // 64KB
            }
        }
    }
    // BlockIndex b31=1: The erase unit is b30~b0, and the erase command is already placed in D->CMD
    else {
        eraseunit = (BlockIndex << SFC_GENERIC_1_BIT_OFFSET) >> SFC_GENERIC_1_BIT_OFFSET;
    }

    // Erase Iterations
    for (eraseaddr = cur_address; iteration > 0 ; iteration--, eraseaddr += eraseunit) {
retry:
        do {
            sf_erase_flag =2;
            result = SF_DAL_CheckDeviceReady(D, BlockIndex);
        } while (FS_FLASH_ERASE_BUSY == result);

        savedMask = SaveAndSetIRQMask();
        if (NOR_FLASH_SUSPENDED) {
            RestoreIRQMask(savedMask);
            goto retry;
        }

        D->DriverStatus = SFDRV_ERASING;
        //write enable
        SF_DAL_Dev_WriteEnable(D);

        SF_DAL_Dev_EraseBlock(D, eraseaddr);
        sf_erase_suspend = 0;
        RestoreIRQMask(savedMask);

        sf_erase_flag = 0x3;
        // 3. wait ready
        while (1)        {
            result = SF_DAL_CheckDeviceReady(D, BlockIndex);
            if (FS_FLASH_ERASE_BUSY != result) {
                break;
            }
        }
         sf_erase_flag = 0x4;
    }

    D->DriverStatus = SFDRV_READY;

    return result;
}

ATTR_TEXT_IN_RAM int32_t SF_DAL_Erase_chip(void *MTDData, const uint16_t CS)
{
    uint32_t savedMask;
    uint8_t cmd;
    int32_t result;
    SF_MTD_Data *D = MTDData;

    savedMask = SaveAndSetIRQMask();
    cmd = SF_CMD_WREN;
    SFI_Dev_Command_Ext(CS, &cmd, NULL, 1, 0);
    cmd = SF_CMD_ERASE_CHIP;
    SFI_Dev_Command_Ext(CS, &cmd, NULL, 1, 0);
    RestoreIRQMask(savedMask);

    while (1) {
        result = SF_DAL_CheckDeviceReady(D, 0);
        if (FS_FLASH_ERASE_BUSY != result) {
            break;
        }
    }
    return result;
}


//-----------------------------------------------------------------------------
/*!
  @brief
    Erase a block

  @param[in] MTDData Pointer to the MTD driver data
  @param[in] BlockIndex Deserted, don't care.

  @retval
    Always FS_UNSUPPORTED_DRIVER_FUNCTION

  @remarks
    This function is an empty function. Currently all serial Flashes are
    single bank devices, and non-blocking operations are forbidden.
*/
ATTR_TEXT_IN_RAM int32_t SF_DAL_NonBlockEraseBlock(void *MTDData, uint32_t BlockIndex)
{
    return FS_UNSUPPORTED_DRIVER_FUNCTION;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Check if the program is completed, failed or busy.

  @param[in] MTDData Pointer to the MTD driver data
  @param[in] addr The last programmed address in Flash.
  @param[in] data The last byte of the data buffer in RAM.

  @retval
    RESULT_FLASH_DONE: The program is completed without error.
    RESULT_FLASH_BUSY: The device is busy.
    RESULT_FLASH_FAIL: Program fail

  @remarks
    After device ready, it compares the last byte in the program buffer and
    the programmed address, returns FAIL if the values are different.
    GPRAM utilization is 1+1B: A device lock is not necessary.
*/
extern bool sfc_interrupted_by_ch2_or_ch3;
extern bool sfc_timer_flag;
volatile uint32_t g_flash_pingpang_flag = 5000;  //us
uint8_t org_data;
uint8_t readback_data;
uint32_t error_addr;
uint32_t error_status;

ATTR_TEXT_IN_RAM int32_t SF_DAL_CheckReadyAndResume(void *MTDData, uint32_t addr, uint8_t data)
{
    uint32_t          savedMask;
    SF_MTD_Data        *D               = MTDData;
    int32_t           result          = RESULT_FLASH_BUSY;    // default result is busy
    uint8_t           check_data;
    uint16_t          status_busy;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;
    uint32_t dsp_request_flag = 0;

    // Read device status
    savedMask = SaveAndSetIRQMask();
    status_busy = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);

    // Flash is suspended due to interrupt => Resume
    if (SF_DAL_FLAG_SUS()) {
        ASSERT_RET(SF_DAL_FLAG_BUSY(), NOR_MTD_ESB_BUSY_FLAG_MISMATCH);

        if (sfc_interrupted_by_ch2_or_ch3) {
            dsp_request_flag = 1;
        }

        if (dsp_request_flag == 0) {
            SFI_MaskAhbChannel(1);
            SF_DAL_Dev_Resume(D);
        }
    }
    // Flash is not suspended and ready => Validate programmed data
    else  if (0 == (status_busy & D->StatusMap[SF_SR_BUSY])) {
        //savedMask = SaveAndSetIRQMask();

        SF_DAL_FLAG_BUSY_CLR();

        // Compare last programmed byte
        check_data = *(volatile uint8_t *)addr;

        if (check_data == data) {
            result = RESULT_FLASH_DONE;
        } else {
            result = RESULT_FLASH_FAIL;
            error_addr = addr;
            org_data = data;
            readback_data = check_data;
        #ifdef HAL_CACHE_MODULE_ENABLED
            if(hal_cache_is_cacheable(addr)) {
                /* 1, Need check if failed as cacheable address;
                   2, should alignment the address to cashe line size*/
                hal_cache_invalidate_one_cache_line(addr & CACHE_LINE_ALIGNMENT_MASK);
                if ((*(volatile uint8_t *)addr) == data) {
                    result = RESULT_FLASH_DONE;
                }
            }
        #endif
        }
        //RestoreIRQMask(savedMask);
    } else {
        //RestoreIRQMask(savedMask);
    }

    if (result == RESULT_FLASH_DONE)
        if (SF_DAL_FailCheck(D) != FS_NO_ERROR) {
            RestoreIRQMask(savedMask);
            return RESULT_FLASH_FAIL;
        }

    RestoreIRQMask(savedMask);

    ///*Wait 1s*/
    if (dsp_request_flag == 1) {
        sf_wait_count_check_ready_and_resume = 0;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &start_count);
        while (count < g_flash_pingpang_flag) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);
            hal_gpt_get_duration_count(start_count, end_count, &count);
        }
        savedMask = SaveAndSetIRQMask();
        sfc_interrupted_by_ch2_or_ch3 = false;
        dsp_request_flag = 0;
        RestoreIRQMask(savedMask);
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);
        //log_hal_msgid_info("[CheckReadyAndResume]: flash wait %d us done, count = %d \r\n", 2, g_flash_pingpang_flag, end_count);
        sf_wait_count_check_ready_and_resume++;
    }

#if defined(SF_DAL_PROG_STATISTICS)
    if (D->DriverStatus == SFDRV_PROGRAMMING) {
        SF_Prog_End();
        if (USQCNT(ProgTotal) > SF_PROG_TIME_LIMIT) {
            SFUT_IntBusySuspend = 0;
        }
        ProgTotal = 0;
    }
#endif

    return result;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Check if the device is ready.

  @param[in] MTDData Pointer to the MTD driver data
  @param[in] BlockIndex The physical block ID (not used in SF driver)

  @retval
    FS_NO_ERROR: The device is ready
    FS_FLASH_ERASE_BUSY: The device is busy.

  @remarks
    GPRAM utilization is 1+1B: A device lock is not necessary.
*/
ATTR_TEXT_IN_RAM int32_t SF_DAL_CheckDeviceReady(void *MTDData, uint32_t BlockIndex)
{
    int32_t               result = FS_FLASH_ERASE_BUSY;
    uint32_t              savedMask       = 0;
    SF_MTD_Data            *D               = MTDData;
    SF_Status               status_busy, status_suspend;
    uint32_t start_count = 0;
    uint32_t end_count = 0;
    uint32_t count = 0;
    uint32_t dsp_request_flag = 0;

    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    /* ensure that the status check is atomic */
    savedMask = SaveAndSetIRQMask();

    // Read Status
    status_busy    = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);
    status_suspend = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadSuspendReg);

    if (0 == (status_busy & D->StatusMap[SF_SR_BUSY])) {
        SF_Status check_status = D->StatusMap[SF_SR_WSE] | D->StatusMap[SF_SR_WSP] ;

        // erase suspended or program suspended
        if ((0 != (status_suspend & check_status)) ||    // check suspend flags
                ((0 == check_status) && SF_DAL_FLAG_SUS())) {    // devices that do not have suspend flags => check driver flag
            //MTD_SNOR_ASSERT(SF_DAL_FLAG_BUSY(), SF_DAL_FLAG_SUS(), status_suspend, status_busy, NOR_MTD_ESB_BUSY_FLAG_MISMATCH);
            ASSERT_RET(SF_DAL_FLAG_BUSY(), NOR_MTD_ESB_BUSY_FLAG_MISMATCH);

            if (sfc_interrupted_by_ch2_or_ch3) {
                dsp_request_flag = 1;
            }


            if (dsp_request_flag == 0) {
                // issue resume command
                SFI_MaskAhbChannel(1);
                SF_DAL_Dev_Resume(D);
                result =  FS_FLASH_ERASE_BUSY;
            }
        } else { // flash is neither busy nor suspended
#if defined(SF_DAL_WINBOND) && defined(SF_FIX_WINBOND_DW)
            if (D->sf_dal_fix & sf_fix_winbond_dw)   {
                SFI_Dev_Command(D->CS, SF_CMD_WINBOND_RESUME);
                SFI_Dev_Command(D->CS, SF_CMD_WINBOND_RESUME);
            }
#endif
            SF_DAL_FLAG_BUSY_CLR();
            /********************************************//**
             * If an interrupt comes during program/erase, in Flash_ReturnReady(), the device may deny the
             * "Suspend Erase/Program" command because the device is near/already ready. However,
             * NOR_FLASH_SUSPENDED is still be set to true.
             *
             * In such case, after back to erase/program operation, CheckDeviceReady will reach here
             * because flash is not busy and not erase suspended (but with NOR_FLASH_SUSPENDED = true). If NOR_FLASH_SUSPENDED
             * is not set to false here, next time when an interrupt comes during erase/program
             * operation, Flash_ReturnReady() will be misleaded by wrong NOR_FLASH_SUSPENDED and return
             * to IRQ handler directly even if flash is still erasing/programing, leading to an execution
             * error! (Since W08.28)
             ***********************************************/
            SF_DAL_FLAG_SUS_CLR();

            result = FS_NO_ERROR;
        }
    } else {
        result = FS_FLASH_ERASE_BUSY;
    }

#if 0//defined(SF_DAL_ERASE_STATISTICS)
    if (D->DriverStatus == SFDRV_ERASING) {
        SF_Erase_End();
        if (US32K(EraseTotal) > SF_ERASE_TIME_LIMIT)  {
            SFUT_IntBusySuspend = 0;
        }
        EraseTotal = 0;
    }
#endif
    RestoreIRQMask(savedMask);

    if (dsp_request_flag == 1) {
        sf_wait_count_check_ready = 0;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &start_count);
        while (count < g_flash_pingpang_flag) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);
            hal_gpt_get_duration_count(start_count, end_count, &count);
        }
        savedMask = SaveAndSetIRQMask();
        sfc_interrupted_by_ch2_or_ch3 = false;
        dsp_request_flag = 0;
        RestoreIRQMask(savedMask);

        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);
        sf_wait_count_check_ready++;
        //log_hal_msgid_info("[CheckDeviceReady]: flash wait %d us done, count = %d \r\n", 2, g_flash_pingpang_flag, end_count);
    }

    if (FS_NO_ERROR == result) {
        result = SF_DAL_FailCheck(D);
    }

    return result;
}

ATTR_TEXT_IN_RAM int32_t SF_DAL_SuspendErase(void *MTDData, uint32_t BlockIndex)
{
    SF_MTD_Data        *D  = MTDData;
    SF_Status           status;
    uint32_t          savedMask;

    savedMask = SaveAndSetIRQMask();

    // 1. Issue suspend command
    SF_DAL_Dev_Suspend(D);

    // 2. wait for device ready

    while (1) {
        status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);

        if (0 == (status & D->StatusMap[SF_SR_BUSY])) {
            break;
        }
    }

    SF_DAL_FLAG_SUS_SET();

    RestoreIRQMask(savedMask);

    return FS_NO_ERROR;
}

ATTR_TEXT_IN_RAM int32_t SF_DAL_ResumeErase(void *MTDData, uint32_t BlockIndex)
{
    SF_MTD_Data        *D = MTDData;

    SF_DAL_Dev_Resume(D);

    return FS_NO_ERROR;
}


//-----------------------------------------------------------------------------
// [DAL] Lower Abstract
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/*!
  @brief
    Program Fail / Erase Fail check
  @param[in] MTDData Pointer to the MTD driver data
  @retval
    FS_NO_ERROR: Program/Erase successful, or fail check is not supported.
    RESULT_FLASH_FAIL: Program/Erase fail
  @remarks
    Caller: SF_DAL_CheckReadyAndResume, SF_DAL_CheckDeviceReady
    GPRAM utilization is 1+1B. A device lock is not necessary for GPRAM.
*/
ATTR_TEXT_IN_RAM int SF_DAL_FailCheck(void *MTDData)
{
    SF_MTD_Data    *D = (SF_MTD_Data *)MTDData;
    uint16_t     status;

#if defined(SF_DAL_GIGADEVICE)
    if (D->sf_dal_fix & sf_fix_gigadevice_b) {
        SF_DAL_Dev_Command(D, SF_CMD_GIGADEVICE_HPM_ENTER_CMD, 4);
    }
#endif

    if (D->CMD->ReadFailReg) {
        status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadFailReg);
        if (status & D->StatusMap[SF_SR_FAIL]) {
            error_status = status;
            return RESULT_FLASH_FAIL;
        }
    }
    return FS_NO_ERROR;
}

//-----------------------------------------------------------------------------
// [DAL] Device Driver Componets (Interface to lower driver or physical device)
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*!
  @brief
    Read 3 bytes ID (Vendor + Density + Device)

  @param[in] MTDData Pointer to the MTD driver data
  @param[out] id The pointer to the array that ID to be stored
*/
ATTR_TEXT_IN_RAM void SF_DAL_Dev_ReadID(SF_MTD_Data *D, uint8_t *id)
{
    uint8_t cmd;
    if (SFI_DevMode != 0)  {
        cmd = D->CMD->ReadID_QPI;
    } else {
        cmd = D->CMD->ReadID_SPI;
    }
    SF_DAL_Dev_Command_Ext(D->CS, &cmd, id, 1, SF_FLASH_ID_LENGTH);
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Command wrapper function <Suspend>

  @param[in] MTDData Pointer to the MTD driver data
*/
ATTR_TEXT_IN_RAM void SF_DAL_Dev_Suspend(SF_MTD_Data *D)
{
    uint32_t savedMask;
    uint32_t currtime, difftime;
    if (sf_dal_resume_time != NOR_DEFAULT_RESUME_TIME)    {
        currtime = ust_get_current_time();
        difftime = ust_get_duration(sf_dal_resume_time, currtime);
        if (difftime < D->SuspendLatency)    {
            DelayAWhile_UST(D->SuspendLatency - difftime);
        }
    }

    savedMask = SaveAndSetIRQMask();
    SFI_Dev_Command(D->CS, D->CMD->Suspend);
#if defined(SF_DAL_GIGADEVICE)
    if (D->sf_dal_fix & sf_fix_gigadevice_b) {
        SFI_Dev_Command(D->CS, SF_CMD_GIGADEVICE_HPM_ENTER);
    }
#endif
    sf_erase_suspend++;
    RestoreIRQMask(savedMask);
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Command wrapper function <Resume>

  @param[in] MTDData Pointer to the MTD driver data
*/
ATTR_TEXT_IN_RAM void SF_DAL_Dev_Resume(SF_MTD_Data *D)
{
    uint32_t savedMask;

    savedMask = SaveAndSetIRQMask();
#if defined(SF_DAL_GIGADEVICE)
    if (D->sf_dal_fix & sf_fix_gigadevice_b) {
        SFI_Dev_Command(D->CS, SF_CMD_GIGADEVICE_HPM_EXIT);
    }
#endif
    SFI_Dev_Command(D->CS, D->CMD->Resume);
    if (D->SuspendLatency != 0) {
        sf_dal_resume_time = ust_get_current_time();
    }
    SF_DAL_FLAG_SUS_CLR();
    RestoreIRQMask(savedMask);
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Command wrapper function <WriteEnable>

  @param[in] MTDData Pointer to the MTD driver data
*/
ATTR_TEXT_IN_RAM void SF_DAL_Dev_WriteEnable(SF_MTD_Data *D)
{
    SF_DAL_Dev_Command(D, D->CMD->WriteEnable, 1);
}

ATTR_TEXT_IN_RAM void SF_DAL_Dev_EraseBlock(SF_MTD_Data *D, uint32_t addr)
{
    uint32_t savedMask;

    if (!(D->sf_dal_fix & sf_fix_gigadevice_b))   {
        SF_DAL_Dev_WriteEnable(D);
    }

    savedMask = SaveAndSetIRQMask();

#if defined(SF_DAL_GIGADEVICE)
    if (D->sf_dal_fix & sf_fix_gigadevice_b)   {
        SFI_Dev_Command(D->CS, D->CMD->WriteEnable);
    }
#endif

    SF_DAL_FLAG_BUSY_SET();
    SFI_Dev_CommandAddress(D->CS, D->CMD->Erase, addr, D->AddressCycle);

    RestoreIRQMask(savedMask);
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Generic reads a status register (SR)

  @param[in] cmd The SR read command, it can be ready SR, security SR, or suspend SR
  @retval Status register's state
*/

ATTR_TEXT_IN_RAM uint32_t SF_DAL_Dev_ReadStatus(SF_MTD_Data *D, uint8_t cmd)
{
    uint32_t savedMask;
    uint32_t result;

    savedMask = SaveAndSetIRQMask();
    SFI_MacEnable(D->CS);
    SFC_GPRAM->RW_SF_GPRAM_DATA = cmd;
    SFC->RW_SF_MAC_OUTL = 1;
    SFC->RW_SF_MAC_INL = 1;
    SFI_MacWaitReady(D->CS);
    result = (unsigned char)((SFC_GPRAM->RW_SF_GPRAM_DATA) >> SFC_GENERIC_8_BIT_OFFSET);
    RestoreIRQMask(savedMask);

    return result;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Issues a command to serial Flash
  @param[in] The command to be issued
  @remarks
    Mask out IRQs to prevent faulty read in MAC mode
*/
ATTR_TEXT_IN_RAM void SF_DAL_Dev_Command(SF_MTD_Data *D, const uint32_t cmd, uint8_t outlen)
{
    uint32_t savedMask;

    savedMask = SaveAndSetIRQMask();
    SFI_MacEnable(D->CS);
    SFC_GPRAM->RW_SF_GPRAM_DATA = cmd;
    SFC->RW_SF_MAC_OUTL = outlen;
    SFC->RW_SF_MAC_INL = 0;
    SFI_MacWaitReady(D->CS);
    RestoreIRQMask(savedMask);
    return;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Issue generic command to serial Flash, and read results.

  @param[in] cmd Pointer to the commands that to be sent
  @param[out] data Pointer to the data buffer that to be stored
  @param[in] outl Length of commands (in bytes)
  @param[in] intl Length of read data
*/
ATTR_TEXT_IN_RAM void SF_DAL_Dev_Command_Ext(const uint16_t CS, uint8_t *cmd, uint8_t *data, const uint16_t outl, const uint16_t inl)
{
    uint32_t savedMask;

    savedMask = SaveAndSetIRQMask();
    SFI_Dev_Command_Ext(CS, cmd, data, outl, inl);
    RestoreIRQMask(savedMask);
    return;
}

#ifdef __SECURITY_OTP__

#if defined(SF_DAL_MXIC) || defined(SF_DAL_SST)
//-----------------------------------------------------------------------------
/*!
  @brief

  @param[in] DriveData Pointer to the MTD driver data

  @see
    SF_DAL_OTPAccess()

  @retval
    FS_NO_ERROR: no error
*/
ATTR_TEXT_IN_RAM int SF_DAL_OTPLock_MXIC(void *MTDData)
{
    uint32_t           savedMask;
    int32_t            result;
    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    // Write Lock Register
    savedMask = SaveAndSetIRQMask();
    SF_DAL_Dev_WriteEnable(D);
    SF_DAL_Dev_Command(D, D->CMD->OTPLock, 1);
    RestoreIRQMask(savedMask);

    // Wait ready
    do   {
        result = SF_DAL_CheckDeviceReady(D, 0);
    } while (FS_FLASH_ERASE_BUSY == result);


    return FS_NO_ERROR;
}
#endif  // SF_DAL_MXIC || SF_DAL_SST

#if defined(SF_DAL_MXIC)
//-----------------------------------------------------------------------------
/*!
  @brief

  @param[in] MTDData Pointer to the MTD driver data
  @param[in] Offset The start address of the OTP registers to be read (in bytes)
  @param[in] BufferPtr: The data buffer that OTP-data to be stored
  @param[in] Length Transfer length, it shall not exceeds the length of OTP registers (in bytes)

  @see
    OTPAccess_MXIC()

  @retval
    FS_NO_ERROR: no error
    FS_FLASH_OTP_OVERSCOPE: writing range is out of OTP range
    FS_FLASH_OTP_LOCK_ALREADY: OTP area is already locked

  @remarks
    This function shall only invoked before MAUI initialization (ex: bootloader),
    Otherwise, the caller must be aware of 60qbits assertion in MAUI.
*/
ATTR_TEXT_IN_RAM int SF_DAL_OTPWrite_MXIC(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length)
{
    uint32_t           savedMask;
    uint32_t           i;
    uint32_t           cmd;
    uint32_t           optr;
    uint8_t           *bptr;
    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;
    uint32_t           status;

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    // Check if the device is already locked
    status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadSecurityReg);

    if (status & D->StatusMap[SF_SR_OTPLOCK]) {
        return FS_FLASH_OTP_LOCK_ALREADY;
    }

    // check if read scope is out of range
    if (0 == Length) {
        return FS_NO_ERROR;
    }
    if ((Offset > D->OTPLength) ||
            (Offset + Length) > D->OTPLength) {
        return FS_FLASH_OTP_OVERSCOPE;
    }

    bptr = BufferPtr;
    optr = D->OTPBase + Offset;

    // Program data into OTP registers.
    // In order to avoid 60qbits assert, program 1 byte at a time.
    // Keep the operation atomic.
    for (i = 0; i < Length; i++, bptr++, optr++)    {
        savedMask = SaveAndSetIRQMask();
        // Enter Secured OTP (ENSO) mode
        SF_DAL_Dev_Command(D, D->CMD->OTPEnter, 1);
        SF_DAL_Dev_WriteEnable(D);
        cmd = (SFI_ReverseByteOrder(optr) & SFC_GENERIC_0xFFFFFF00_MASK) | D->CMD->OTPProg;
        SFC_GPRAM->RW_SF_GPRAM_DATA =  cmd;
        SFC_GPRAM->RW_SF_GPRAM_DATA_OF_4 = *bptr;
        SFC->RW_SF_MAC_OUTL = 5;
        SFC->RW_SF_MAC_INL = 0;
        SFI_MacEnable(D->CS);
        SFI_MacWaitReady(D->CS);
        // Wait Device Ready
        do {
            status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);
        } while (status & D->StatusMap[SF_SR_BUSY]);
        // Leave Secured OTP (ENSO) mode
        SF_DAL_Dev_Command(D, D->CMD->OTPExit, 1);
        RestoreIRQMask(savedMask);
    }

    return FS_NO_ERROR;
}

//-----------------------------------------------------------------------------
/*!
  @brief

  @param[in] MTDData Pointer to the MTD driver data
  @param[in] Offset The start address of the OTP registers to be read (in bytes)
  @param[in] BufferPtr: The data buffer that OTP-data to be stored
  @param[in] Length Transfer length, it shall not exceeds the length of OTP registers (in bytes)

  @see
    OTPAccess_MXIC()

  @retval
    FS_NO_ERROR: Operation successful
    FS_FLASH_OTP_OVERSCOPE: Out of OTP writing range
*/
ATTR_TEXT_IN_RAM int SF_DAL_OTPRead_MXIC(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length)
{
    uint32_t           savedMask;
    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;
    int32_t            i, j;      // iterator
    uint8_t           *bptr;   // pointer to BufferPtr
    uint8_t           *optr;   // offset to OTP Registers

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    /* check if read scope is out of range */
    if (0 == Length) {
        return FS_NO_ERROR;
    }
    if ((Offset > D->OTPLength) ||
            (Offset + Length) > D->OTPLength) {
        return FS_FLASH_OTP_OVERSCOPE;
    }

    optr = D->BaseAddr + D->OTPBase + Offset;
    bptr = BufferPtr;

    // To avoid 60qbits assert, read most 32 bytes at a time.
    // => The size of SFC read buffer is 32 bytes
    for (i = 0; i < Length; i += 32)   {
        savedMask = SaveAndSetIRQMask();
        // Enter Secured OTP (ENSO) mode
        SF_DAL_Dev_Command(D, D->CMD->OTPEnter, 1);
        // Read Secured OTP
        // [Note]
        // 1. Always use FS base address as the Flash base addr.
        //    =>After entering OTP mode, OTP data are available on every block offsets
        //      (depends on vendor).
        //    =>FS area are not cached, so that there's no need to disable caches.
        // 2. Do not use memcpy(), because it is an XIP function on Flash.
        for (j = i; j < i + 32 && j < Length; j++, bptr++, optr++)  {
            *bptr = *optr;
        }
        // Leave Secured OTP (ENSO) mode
        SF_DAL_Dev_Command(D, D->CMD->OTPExit, 1);
        RestoreIRQMask(savedMask);
    }

    return FS_NO_ERROR;
}
#endif // SF_DAL_MXIC

#if defined(SF_DAL_WINBOND)
//-----------------------------------------------------------------------------
/*!
  @brief

  @param[in] DriveData Pointer to the MTD driver data

  @see
    SF_DAL_OTPAccess()

  @retval
    FS_NO_ERROR: no error
*/
ATTR_TEXT_IN_RAM int SF_DAL_OTPLock_WINBOND(void *MTDData)
{
    uint32_t          data_SR1, data_SR2, savedMask;
    int32_t           result;
    uint32_t          cmd;
    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);


    // write enable
    SF_DAL_Dev_WriteEnable(D);

    data_SR1 = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadBPReg);
    data_SR2 = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadSuspendReg);

    cmd = D->CMD->WriteBPReg;
    data_SR1 = data_SR1 << SFC_GENERIC_8_BIT_OFFSET;
    data_SR2 = data_SR2 | SF_WINBOND_SSR_OTP;

    cmd = (cmd << SFC_GENERIC_16_BIT_OFFSET) | data_SR1 | data_SR2;

    // write status resgister
    savedMask = SaveAndSetIRQMask();
    SF_DAL_FLAG_BUSY_SET();
    SFI_Dev_Command(D->CS, cmd);
    RestoreIRQMask(savedMask);

    do   {
        result = SF_DAL_CheckDeviceReady(D, 0);
    } while (FS_FLASH_ERASE_BUSY == result);


    return FS_NO_ERROR;
}

//-----------------------------------------------------------------------------
/*!
  @brief

  @param[in] MTDData Pointer to the MTD driver data
  @param[in] Offset The start address of the OTP registers to be read (in bytes)
  @param[in] BufferPtr: The data buffer that OTP-data to be stored
  @param[in] Length Transfer length, it shall not exceeds the length of OTP registers (in bytes)

  @see
    OTPAccess()

  @retval
    FS_NO_ERROR: no error
    FS_FLASH_OTP_OVERSCOPE: writing range is out of OTP range
    FS_FLASH_OTP_LOCK_ALREADY: OTP area is already locked

  @remarks
    The physical discontinuous address is covered by the funciton. It's continuous to upper layer users.
*/
ATTR_TEXT_IN_RAM int SF_DAL_OTPWrite_WINBOND(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length)
{
    uint32_t          savedMask;
    uint32_t          i;
    uint32_t          cmd;
    uint32_t          optr_logical, optr_physical;  //offset pointer
    uint8_t           *bptr;
    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;
    uint32_t          status;
    //uint32_t          save_QPI;
    //uint32_t          save_DIR_CTRL;
    //uint32_t          save_WRAP;

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    // Check if the device is already locked
    status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadSecurityReg);
    if (status & D->StatusMap[SF_SR_OTPLOCK]) {
        return FS_FLASH_OTP_LOCK_ALREADY;
    }

    // check if read scope is out of range
    if (0 == Length) {
        return FS_NO_ERROR;
    }
    if ((Offset > D->OTPLength) ||
            (Offset + Length) > D->OTPLength) {
        return FS_FLASH_OTP_OVERSCOPE;
    }

    bptr = BufferPtr;
    optr_logical = D->OTPBase + Offset;

    // Program data into OTP registers.
    // In order to avoid 60qbits assert, program 1 byte at a time.
    // Keep the operation atomic.
    for (i = 0; i < Length; i++, bptr++, optr_logical++)    {
        //make the upper application transparent to the physical address
        optr_physical = ((optr_logical << SFC_GENERIC_4_BIT_OFFSET) & SFC_GENERIC_0xF000_MASK) | (optr_logical & SFC_GENERIC_0x00FF_MASK);
        cmd = (SFI_ReverseByteOrder(optr_physical) & SFC_GENERIC_0xFFFFFF00_MASK) | D->CMD->OTPProg;
        //Save current mode
        //save_QPI = SFI_DevMode;
        //save_DIR_CTRL = SFC->RW_SF_DIRECT_CTL;
        //save_WRAP = SFC->RW_SF_MISC_CTL2;
        //switch to SPI mode
        savedMask = SaveAndSetIRQMask();

        #if 0
        // Note: Winbond 0xFF command reserves wrap settings
        if (D->FlashID[1] != 0x40 && D->FlashID[1] != 0x70) { //Only switch to SPI for QPI device
            SFI_Dev_Command(D->CS, D->CMD->OTPEnter);  // Switch device to SPI mode
            SFI_SetDevMode(D->CS, 0); // Switch SFC to SPI mode
            SFC->RW_SF_DIRECT_CTL = (0xEB0B5770); //make flash accessible during SPI-Q mode
            SFC->RW_SF_MISC_CTL2 = (0x0); //no-wrap
            RestoreIRQMask(savedMask); //restore IRQ here to avoid 60Qbits assert
            savedMask = SaveAndSetIRQMask();
        }
        #endif

        //program data
        SF_DAL_Dev_WriteEnable(D);
        SFC_GPRAM->RW_SF_GPRAM_DATA = cmd;
        SFC_GPRAM->RW_SF_GPRAM_DATA_OF_4 = *bptr;
        SFC->RW_SF_MAC_OUTL = 5;
        SFC->RW_SF_MAC_INL = 0;
        SFI_MacEnable(D->CS);
        SFI_MacWaitReady(D->CS);

        // Wait Device Ready
        do {
            status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);
        } while (status & D->StatusMap[SF_SR_BUSY]);

#if 0
        //switch to previous mode
        if (D->FlashID[1] != 0x40 && D->FlashID[1] != 0x70) { //Only switch to SPI for QPI device, Quad bit is default set to 1 and Wrap setting is not changed by 0xFF command for QPI device
            RestoreIRQMask(savedMask); //restore IRQ here to avoid 60Qbits assert
            savedMask = SaveAndSetIRQMask();
            SFI_Dev_Command(D->CS, D->CMD->OTPExit);  // Switch device to QPI mode
            SFI_SetDevMode(D->CS, save_QPI); // Switch SFC to QPI mode
            SFC->RW_SF_DIRECT_CTL = (save_DIR_CTRL); //Restore QPI setting
            SFC->RW_SF_MISC_CTL2 = (save_WRAP); //restore-wrap
            SFI_Dev_Command(D->CS, 0xC032);
        }
#endif
        RestoreIRQMask(savedMask);
    }

    return FS_NO_ERROR;
}

//-----------------------------------------------------------------------------
/*!
  @brief

  @param[in] MTDData Pointer to the MTD driver data
  @param[in] Offset The start address of the OTP registers to be read (in bytes)
  @param[in] BufferPtr: The data buffer that OTP-data to be stored
  @param[in] Length Transfer length, it shall not exceed the length of OTP registers (in bytes)

  @see
    OTPAccess_MXIC()

  @retval
    FS_NO_ERROR: Operation successful
    FS_FLASH_OTP_OVERSCOPE: Out of OTP writing range

  @remarks
    The physical discontinuous address is covered by the funciton. It's continuous to upper layer users.
*/
ATTR_TEXT_IN_RAM int SF_DAL_OTPRead_WINBOND(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length)
{
    uint32_t          savedMask;
    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;
    int32_t           i;
    uint8_t           *bptr;   // pointer to BufferPtr
    uint32_t          optr_logical, optr_physical;  //offset pointer
    uint32_t           cmd;
    //uint32_t          save_QPI;

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    /* check if read scope is out of range */
    if (0 == Length) {
        return FS_NO_ERROR;
    }
    if ((Offset > D->OTPLength) ||
            (Offset + Length) > D->OTPLength) {
        return FS_FLASH_OTP_OVERSCOPE;
    }

    bptr = BufferPtr;
    optr_logical = D->OTPBase + Offset;

    for (i = 0; i < Length; i++, bptr++, optr_logical++)  {
        //make the upper application transparent to the physical address
        optr_physical = ((optr_logical << SFC_GENERIC_4_BIT_OFFSET) & SFC_GENERIC_0xF000_MASK) | (optr_logical & SFC_GENERIC_0x00FF_MASK);
        cmd = (SFI_ReverseByteOrder(optr_physical) & SFC_GENERIC_0xFFFFFF00_MASK) | D->CMD->OTPRead;

        //Save current mode
        //save_QPI = SFI_DevMode;

        //switch to SPI mode
        savedMask = SaveAndSetIRQMask();
        #if 0
        if (D->FlashID[1] != 0x40 && D->FlashID[1] != 0x70) { //Only switch to SPI for QPI device
            SFI_Dev_Command(D->CS, D->CMD->OTPEnter);  // Switch the device to SPI mode

            SFI_SetDevMode(D->CS, 0); // Switch SFC to SPI mode
        }
        #endif
        //Read data
        SFC_GPRAM->RW_SF_GPRAM_DATA = cmd;
        SFC->RW_SF_MAC_OUTL = 4;
        SFC->RW_SF_MAC_INL = 2;//read 1 byte + 1 dummy cycle
        SFI_MacEnable(D->CS);
        SFI_MacWaitReady(D->CS);

        *bptr = SFI_ReadReg8(RW_SFI_GPRAM_DATA + 5);

        #if 0
        if (D->FlashID[1] != 0x40 && D->FlashID[1] != 0x70) { //Only switch to SPI for QPI device, Quad bit is default set to 1 and Wrap setting is not changed by 0xFF command for QPI device
            SFI_Dev_Command(D->CS, D->CMD->OTPExit); // Switch the device to QPI mode
            SFI_SetDevMode(D->CS, save_QPI); // Switch SFC to QPI mode
            SFI_Dev_Command(D->CS, 0xC032); //Set dummy cycle
        }
        #endif
        RestoreIRQMask(savedMask);
    }

    return FS_NO_ERROR;
}
#endif // SF_DAL_WINBOND
#if defined(SF_DAL_GIGADEVICE)
//-----------------------------------------------------------------------------
/*!
  @brief

  @param[in] DriveData Pointer to the MTD driver data

  @see
    SF_DAL_OTPAccess()

  @retval
    FS_NO_ERROR: no error
*/
ATTR_TEXT_IN_RAM int SF_DAL_OTPLock_GIGADEVICE(void *MTDData)
{

    uint32_t          data_SR1, data_SR2, savedMask;
    int32_t           result;
    uint32_t          cmd;
    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    // write enable
    SF_DAL_Dev_WriteEnable(D);

    data_SR1 = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadBPReg);
    data_SR2 = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadSuspendReg);

    cmd = D->CMD->WriteBPReg;
    data_SR1 = data_SR1 << SFC_GENERIC_8_BIT_OFFSET;
    data_SR2 = data_SR2 | SF_WINBOND_SSR_OTP;

    cmd = (cmd << SFC_GENERIC_16_BIT_OFFSET) | data_SR1 | data_SR2;

    // write status resgister
    savedMask = SaveAndSetIRQMask();
    SF_DAL_FLAG_BUSY_SET();
    SFI_Dev_Command(D->CS, cmd);
    RestoreIRQMask(savedMask);

    do   {
        result = SF_DAL_CheckDeviceReady(D, 0);
    } while (FS_FLASH_ERASE_BUSY == result);


    return FS_NO_ERROR;


}

//-----------------------------------------------------------------------------
/*!
  @brief

  @param[in] MTDData Pointer to the MTD driver data
  @param[in] Offset The start address of the OTP registers to be read (in bytes)
  @param[in] BufferPtr: The data buffer that OTP-data to be stored
  @param[in] Length Transfer length, it shall not exceeds the length of OTP registers (in bytes)

  @see
    OTPAccess()

  @retval
    FS_NO_ERROR: no error
    FS_FLASH_OTP_OVERSCOPE: writing range is out of OTP range
    FS_FLASH_OTP_LOCK_ALREADY: OTP area is already locked

  @remarks
    The physical discontinuous address is covered by the funciton. It's continuous to upper layer users.
*/
ATTR_TEXT_IN_RAM int SF_DAL_OTPWrite_GIGADEVICE(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length)
{
    uint32_t          savedMask;
    uint32_t          i;
    uint32_t          cmd;
    uint32_t          optr_logical, optr_physical;  //offset pointer
    uint8_t           *bptr;
    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;
    uint32_t          status;
    uint32_t          save_QPI;

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    // Check if the device is already locked
    status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadSecurityReg);
    if (status & D->StatusMap[SF_SR_OTPLOCK]) {
        return FS_FLASH_OTP_LOCK_ALREADY;
    }

    // check if read scope is out of range
    if (0 == Length) {
        return FS_NO_ERROR;
    }
    if ((Offset > D->OTPLength) ||
            (Offset + Length) > D->OTPLength) {
        return FS_FLASH_OTP_OVERSCOPE;
    }

    bptr = BufferPtr;
    optr_logical = D->OTPBase + Offset;

    // Program data into OTP registers.
    // In order to avoid 60qbits assert, program 1 byte at a time.
    // Keep the operation atomic.
    for (i = 0; i < Length; i++, bptr++, optr_logical++)    {
        //make the upper application transparent to the physical address
        optr_physical = ((optr_logical << SFC_GENERIC_4_BIT_OFFSET) & SFC_GENERIC_0xF000_MASK) | (optr_logical & SFC_GENERIC_0x00FF_MASK);
        cmd = (SFI_ReverseByteOrder(optr_physical) & SFC_GENERIC_0xFFFFFF00_MASK) | D->CMD->OTPProg;
        //Save current mode
        save_QPI = SFI_DevMode;
        //switch to SPI mode
        savedMask = SaveAndSetIRQMask();
        // Note: Winbond 0xFF command reserves wrap settings
        SFI_Dev_Command(D->CS, D->CMD->OTPEnter);  // Switch device to SPI mode
        SFI_SetDevMode(D->CS, 0); // Switch SFC to SPI mode

        //program data
        SF_DAL_Dev_WriteEnable(D);
        SFC_GPRAM->RW_SF_GPRAM_DATA = cmd;
        SFC_GPRAM->RW_SF_GPRAM_DATA_OF_4 = *bptr;
        SFC->RW_SF_MAC_OUTL = 5;
        SFC->RW_SF_MAC_INL = 0;
        SFI_MacEnable(D->CS);
        SFI_MacWaitReady(D->CS);

        // Wait Device Ready
        do {
            status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);
        } while (status & D->StatusMap[SF_SR_BUSY]);

        //switch to previous mode
        SFI_Dev_Command(D->CS, D->CMD->OTPExit);  // Switch device to QPI mode
        SFI_SetDevMode(D->CS, save_QPI); // Switch SFC to QPI mode
        if (D->FlashID[2] == 0x18) { //Set dummy cycle and Wrap
            SFI_Dev_Command(D->CS, 0xC012);
        } else {
            SFI_Dev_Command(D->CS, 0xC022);
        }
        RestoreIRQMask(savedMask);
    }

    return FS_NO_ERROR;


}

//-----------------------------------------------------------------------------
/*!
  @brief

  @param[in] MTDData Pointer to the MTD driver data
  @param[in] Offset The start address of the OTP registers to be read (in bytes)
  @param[in] BufferPtr: The data buffer that OTP-data to be stored
  @param[in] Length Transfer length, it shall not exceed the length of OTP registers (in bytes)

  @see
    OTPAccess_MXIC()

  @retval
    FS_NO_ERROR: Operation successful
    FS_FLASH_OTP_OVERSCOPE: Out of OTP writing range

  @remarks
    The physical discontinuous address is covered by the funciton. It's continuous to upper layer users.
*/
ATTR_TEXT_IN_RAM int SF_DAL_OTPRead_GIGADEVICE(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length)
{

    uint32_t          savedMask;
    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;
    int32_t           i;
    uint8_t           *bptr;   // pointer to BufferPtr
    uint32_t          optr_logical, optr_physical;  //offset pointer
    uint32_t           cmd;
    //uint32_t          save_QPI;

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    /* check if read scope is out of range */
    if (0 == Length) {
        return FS_NO_ERROR;
    }
    if ((Offset > D->OTPLength) ||
            (Offset + Length) > D->OTPLength) {
        return FS_FLASH_OTP_OVERSCOPE;
    }

    bptr = BufferPtr;
    optr_logical = D->OTPBase + Offset;

    for (i = 0; i < Length; i++, bptr++, optr_logical++)  {
        //make the upper application transparent to the physical address
        optr_physical = ((optr_logical << SFC_GENERIC_4_BIT_OFFSET) & SFC_GENERIC_0xF000_MASK) | (optr_logical & SFC_GENERIC_0x00FF_MASK);
        cmd = (SFI_ReverseByteOrder(optr_physical) & SFC_GENERIC_0xFFFFFF00_MASK) | D->CMD->OTPRead;

        //Save current mode
        //save_QPI = SFI_DevMode;

        //switch to SPI mode
        savedMask = SaveAndSetIRQMask();
        #if 0
        SFI_Dev_Command(D->CS, D->CMD->OTPEnter);  // Switch the device to SPI mode
        SFI_SetDevMode(D->CS, 0); // Switch SFC to SPI mode
        #endif

        //Read data
        SFC_GPRAM->RW_SF_GPRAM_DATA =  cmd;
        SFC->RW_SF_MAC_OUTL = 4;
        SFC->RW_SF_MAC_INL = 2;//read 1 byte + 1 dummy cycle
        SFI_MacEnable(D->CS);
        SFI_MacWaitReady(D->CS);

        *bptr = SFI_ReadReg8(RW_SFI_GPRAM_DATA + 5);

        //switch to previous mode
        #if 0
        SFI_Dev_Command(D->CS, D->CMD->OTPExit); // Switch the device to QPI mode
        SFI_SetDevMode(D->CS, save_QPI); // Switch SFC to QPI mode
        if (D->FlashID[2] == 0x18) { //Set dummy cycle and Wrap
            SFI_Dev_Command(D->CS, 0xC012);
        } else {
            SFI_Dev_Command(D->CS, 0xC022);
        }
        #endif
        RestoreIRQMask(savedMask);
    }

    return FS_NO_ERROR;

}
#endif // SF_DAL_GIGADEVICE


#if defined(SF_DAL_GIGADEVICE) || defined(SF_DAL_WINBOND)
ATTR_TEXT_IN_RAM uint32_t SF_DAL_OTP_Size_WINBOND(void *MTDData)
{
    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;

    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    return D->OTPLength;
}

ATTR_TEXT_IN_RAM OTP_BANK_LOCK_enum SF_DAL_OTP_LOCK_Status_WINBOND(void *MTDData)
{
    SF_MTD_Data *D = (SF_MTD_Data *)MTDData;
    uint32_t status;
    uint32_t lock_status;

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    // Check if the device is already locked
    status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadSecurityReg);
    lock_status = (status & D->StatusMap[SF_SR_OTPLOCK]) >> 3;

    return (OTP_BANK_LOCK_enum)lock_status;
}


ATTR_TEXT_IN_RAM int SF_DAL_OTPLock_blank(void *MTDData, OTP_BANK_enum bank)
{
    uint32_t          data_SR1, data_SR2, savedMask;
    int32_t           result;
    uint32_t          cmd;
    uint8_t  lock_bank;
    OTP_BANK_LOCK_enum lock_status;

    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    lock_status = SF_DAL_OTP_LOCK_Status_WINBOND(MTDData);
    if(lock_status & bank) {
        return FS_FLASH_OTP_LOCK_ALREADY;
    }

    // write enable
    SF_DAL_Dev_WriteEnable(D);

    data_SR1 = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadBPReg);
    data_SR2 = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadSuspendReg);

    cmd = D->CMD->WriteBPReg;
    switch (bank) {
        case OTP_BANK_LB1:
            lock_bank = 0x8;
            break;
        case OTP_BANK_LB2:
            lock_bank = 0x10;
            break;
        case OTP_BANK_LB3:
            lock_bank = 0x20;
            break;
        default:
            lock_bank = 0x0;;
            break;
    }

    if (lock_bank == 0 ) {
        return FS_FLASH_OTP_WRITEFAIL;
    }

    data_SR1 = data_SR1 << SFC_GENERIC_8_BIT_OFFSET;
    data_SR2 = data_SR2 | lock_bank;

    cmd = (cmd << SFC_GENERIC_16_BIT_OFFSET) | data_SR1 | data_SR2;

    // write status resgister
    savedMask = SaveAndSetIRQMask();
    SF_DAL_FLAG_BUSY_SET();
    SFI_Dev_Command(D->CS, cmd);
    RestoreIRQMask(savedMask);

    do   {
        result = SF_DAL_CheckDeviceReady(D, 0);
    } while (FS_FLASH_ERASE_BUSY == result);

    return FS_NO_ERROR;
}


#define SFC_OTP_ERASE_CMD_WINBOND   (0x44)
ATTR_TEXT_IN_RAM int SF_DAL_OTP_ERASE_WINBOND(void *MTDData, uint32_t block)
{
    uint32_t          cmd;
    SF_MTD_Data       *D = (SF_MTD_Data *)MTDData;
    uint32_t          addr;
    uint32_t          addr2;
    uint32_t          status;
    uint32_t          savedMask;

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    // Check if the device is locked
    //status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadSecurityReg);
    status = SF_DAL_OTP_LOCK_Status_WINBOND(D);
    if (status & D->StatusMap[SF_SR_OTPLOCK]) {
        return FS_FLASH_OTP_LOCK_ALREADY;
    }

    //program data
    savedMask = SaveAndSetIRQMask();
    SF_DAL_Dev_WriteEnable(D);

    if (block == 0) {
        addr = 0x1000;
    } else if (block == 1) {
        addr = 0x2000;
    } else if (block == 2) {
        addr = 0x3000;
    } else {
        return FS_FLASH_OTP_OVERSCOPE;
    }

    cmd = SFC_OTP_ERASE_CMD_WINBOND;
    addr2 = SFI_ReverseByteOrder(addr);
    SFC_GPRAM->RW_SF_GPRAM_DATA = (addr2 & 0xFFFFFF00) | cmd;

    SFC->RW_SF_MAC_OUTL = 4;
    SFC->RW_SF_MAC_INL = 0;
    SFI_MacEnable(D->CS);
    SFI_MacWaitReady(D->CS);
    RestoreIRQMask(savedMask);
    
    // Wait Device Ready
    do {
        status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);
    } while (status & D->StatusMap[SF_SR_BUSY]);

    return FS_NO_ERROR;
}

#include "syslog.h"

ATTR_TEXT_IN_RAM int SF_DAL_Flash_Protect(void *MTDData, uint32_t start, uint32_t length)
{
    uint32_t          cmd;
    SF_MTD_Data       *D = (SF_MTD_Data *)MTDData;
    uint32_t          status;
    uint8_t           sr1,sr2;
    uint32_t          savedMask;

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    if (start != 0 && length != 0x20000) {
        return (status = FLASH_WRONG_PROTECT_REGION);
    }


    savedMask = SaveAndSetIRQMask();
    SF_DAL_Dev_WriteEnable(D);
    sr1 = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);

    cmd = 1;    //WRSR;
    //set BP0 and TP to 1 for W25Q64
    //set BP0 and BP3 to 1 for GD25LE64
    //please should mind it is not same for different sise of devices.
    sr2 = 0x24 | sr1;    //lock the top 128KB 
    SFC_GPRAM->RW_SF_GPRAM_DATA = ((sr2<<8) & 0xFF00) | cmd;

    SFC->RW_SF_MAC_OUTL = 2;
    SFC->RW_SF_MAC_INL = 0;
    SFI_MacEnable(D->CS);
    SFI_MacWaitReady(D->CS);
    RestoreIRQMask(savedMask);

    // Wait Device Ready
    do {
        status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);
    } while (status & D->StatusMap[SF_SR_BUSY]);

    return FS_NO_ERROR;
}

ATTR_TEXT_IN_RAM int SF_DAL_Flash_UnProtect(void *MTDData)
{
    uint32_t          cmd;
    SF_MTD_Data       *D = (SF_MTD_Data *)MTDData;
    uint32_t          status;
    uint8_t           sr1,sr2;
    uint32_t          savedMask;

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    savedMask = SaveAndSetIRQMask();
    SF_DAL_Dev_WriteEnable(D);
    sr1 = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);


    if ((sr1 & 0x1C) == 0) {   //BP0,BP1,BP2 = 0  no protect area
    } else {
        cmd = 1;    //WRSR;
        //set BP0,BP1,BP2 to 0
        sr2 = (sr1 & 0x83);    //unlock the device 
        SFC_GPRAM->RW_SF_GPRAM_DATA = ((sr2<<8) & 0xFF00) | cmd;

        SFC->RW_SF_MAC_OUTL = 2;
        SFC->RW_SF_MAC_INL = 0;
        SFI_MacEnable(D->CS);
        SFI_MacWaitReady(D->CS);
        RestoreIRQMask(savedMask);

        // Wait Device Ready
        do {
            status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);
        } while (status & D->StatusMap[SF_SR_BUSY]);
    }

    return FS_NO_ERROR;
}

#else
ATTR_TEXT_IN_RAM int SF_DAL_OTPLock_blank(void *MTDData)
{
    return FS_FLASH_OTP_UNKNOWERR;
}
#endif


#if defined(SF_DAL_SST)

//-----------------------------------------------------------------------------
/*!
  @brief

  @param[in] MTDData Pointer to the MTD driver data
  @param[in] Offset The start address of the OTP registers to be read (in bytes)
  @param[in] BufferPtr: The data buffer that OTP-data to be stored
  @param[in] Length Transfer length, it shall not exceeds the length of OTP registers (in bytes)

  @see
    OTPAccess_MXIC()

  @retval
    FS_NO_ERROR: no error
    FS_FLASH_OTP_OVERSCOPE: writing range is out of OTP range
    FS_FLASH_OTP_LOCK_ALREADY: OTP area is already locked

  @remarks
    This function shall only invoked before MAUI initialization (ex: bootloader),
    Otherwise, the caller must be aware of 60qbits assertion in MAUI.
    The physical discontinuous address is covered by the funciton. It's continuous to upper layer users.
*/
ATTR_TEXT_IN_RAM int SF_DAL_OTPWrite_SST(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length)
{
    uint32_t           savedMask;
    uint32_t           i;
    uint32_t           cmd;
    //uint32_t           optr;
    uint32_t          optr;  //offset pointer
    uint8_t           *bptr;
    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;
    uint32_t           status;

    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    // Check if the device is already locked
    status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadSecurityReg);

    if (status & D->StatusMap[SF_SR_OTPLOCK]) {
        return FS_FLASH_OTP_LOCK_ALREADY;
    }

    // check if read scope is out of range
    if (0 == Length) {
        return FS_NO_ERROR;
    }
    if ((Offset > D->OTPLength) ||
            (Offset + Length) > D->OTPLength) {
        return FS_FLASH_OTP_OVERSCOPE;
    }

    bptr = BufferPtr;
    optr = D->OTPBase + Offset;

    // Program data into OTP registers.
    // In order to avoid 60qbits assert, program 1 byte at a time.
    // Keep the operation atomic.
    for (i = 0; i < Length; i++, bptr++, optr++)    {
        cmd = (*bptr << SFC_GENERIC_16_BIT_OFFSET) | (optr << SFC_GENERIC_8_BIT_OFFSET) | D->CMD->OTPProg;
        savedMask = SaveAndSetIRQMask();

        //program data
        SF_DAL_Dev_WriteEnable(D);
        SFC_GPRAM->RW_SF_GPRAM_DATA =  cmd;
        SFC->RW_SF_MAC_OUTL = 3;// 1 cmd + 1 address + 1 data
        SFC->RW_SF_MAC_INL = 0;
        SFI_MacEnable(D->CS);
        SFI_MacWaitReady(D->CS);
        // Wait Device Ready
        do {
            status = SF_DAL_Dev_ReadStatus(D, D->CMD->ReadReadyReg);
        } while (status & D->StatusMap[SF_SR_BUSY]);

        RestoreIRQMask(savedMask);
    }

    return FS_NO_ERROR;
}

//-----------------------------------------------------------------------------
/*!
  @brief

  @param[in] MTDData Pointer to the MTD driver data
  @param[in] Offset The start address of the OTP registers to be read (in bytes)
  @param[in] BufferPtr: The data buffer that OTP-data to be stored
  @param[in] Length Transfer length, it shall not exceeds the length of OTP registers (in bytes)

  @see
    OTPAccess_MXIC()

  @retval
    FS_NO_ERROR: Operation successful
    FS_FLASH_OTP_OVERSCOPE: Out of OTP writing range

  @remarks
    The physical discontinuous address is covered by the funciton. It's continuous to upper layer users.
*/
ATTR_TEXT_IN_RAM int SF_DAL_OTPRead_SST(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length)
{
    uint32_t           savedMask;
    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;
    int32_t            i;
    uint8_t           *bptr, *indata_ptr;   // pointer to BufferPtr
    uint32_t          optr;  //offset pointer
    uint32_t           cmd;
    // Asserts, if the device has not mounted yet.
    ASSERT_RET((~D->Signature == (uint32_t)D->RegionInfo), 0);

    /* check if read scope is out of range */
    if (0 == Length) {
        return FS_NO_ERROR;
    }
    if ((Offset > D->OTPLength) ||
            (Offset + Length) > D->OTPLength) {
        return FS_FLASH_OTP_OVERSCOPE;
    }

    bptr = BufferPtr;
    optr = D->OTPBase + Offset;
    cmd = (optr << SFC_GENERIC_8_BIT_OFFSET) | D->CMD->OTPRead;

    savedMask = SaveAndSetIRQMask();

    //Read data
    SFC_GPRAM->RW_SF_GPRAM_DATA =  cmd;
    SFC->RW_SF_MAC_OUTL = 2;// 1 cmd + 1 address cycle
    SFC->RW_SF_MAC_INL = Length + 2; //read length + 2 dummy cycles
    SFI_MacEnable(D->CS);
    SFI_MacWaitReady(D->CS);

    indata_ptr = (uint8_t *)(RW_SFI_GPRAM_DATA + 4); //shift out 1 cmd, 1 address and 2 dummy cycles
    for (i = 0; i < Length; i++, bptr++, indata_ptr++)  {
        *bptr = *indata_ptr;
    }

    RestoreIRQMask(savedMask);

    return FS_NO_ERROR;
}
#endif
//-----------------------------------------------------------------------------
/*!
  @brief
    Query the length of OTP Registers

  @param[in] MTDData Pointer to the MTD driver data
  @param[out] LengthPtr Pointer to the unsigned integer to be stored

  @see
    OTPAccess_MXIC()

  @retval
    Always FS_NO_ERROR
*/
ATTR_TEXT_IN_RAM int32_t SF_DAL_OTPQueryLength(void *MTDData, uint32_t *LengthPtr)
{
    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;
    *LengthPtr = D->OTPLength;
    return 0;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Access one time programmable (OTP) registers, the operations  can be one of
    FS_OTP_READ, FS_OTP_WRITE, or FS_OTP_LOCK. The operation enumerations are
    defined in hal_flash_mtd_sf_dal.h.

  @param[in] MTDData Pointer to the MTD driver data
  @param[in] accesstype FS_OTP_READ, FS_OTP_WRITE, or FS_OTP_LOCK
  @param[in] Offset The start address of the OTP registers to be accessed (in bytes)
  @param[in] BufferPtr: The data buffer that OTP-data to be stored
  @param[in] Length Transfer length, it shall not exceed the length of OTP registers (in bytes)

  @see
    OTPAccess_MXIC()

  @retval
    FS_UNSUPPORTED_DRIVER_FUNCTION: The driver/deivce does not support OTP.
    FS_FLASH_OTP_OVERSCOPE: writing range is out of OTP range
    FS_FLASH_OTP_LOCK_ALREADY: OTP area is already locked
    FS_NO_ERROR: Operation Successful.

  @remarks
    Once OTP locked, the OTP registers are no longer writable, and FS_OTP_WRITE will
    return FS_FLASH_OTP_LOCK_ALREADY.

*/
ATTR_TEXT_IN_RAM int32_t SF_DAL_OTPAccess(void *MTDData, int32_t accesstype, uint32_t Offset, void *BufferPtr, uint32_t Length)
{
    SF_MTD_Data         *D = (SF_MTD_Data *)MTDData;

    if (D->OTPLength == 0)    {
        return FS_UNSUPPORTED_DRIVER_FUNCTION;
    }

    switch (accesstype)  {
        case FS_OTP_READ:
            return D->OTPRead(D, Offset, BufferPtr, Length);
        case FS_OTP_WRITE:
            return D->OTPWrite(D, Offset, BufferPtr, Length);
        case FS_OTP_LOCK:
            return D->OTPLock(D);
        default:
            break;
    }

    return FS_UNSUPPORTED_DRIVER_FUNCTION;
}
#endif //__SECURITY_OTP__


//-----------------------------------------------------------------------------
/*!
  @brief
    Wait device ready in init stage.
  @param[in] MTDData Pointer to the MTD driver data
*/
ATTR_TEXT_IN_RAM void SF_DAL_Dev_WaitReady_EMIINIT(SF_MTD_Data *D)
{
    SF_Status    status;
    do   {
        SFI_Dev_Command_Ext(D->CS, &D->CMD->ReadReadyReg, &status, 1, 1);
    } while (0 != (status & D->StatusMap[SF_SR_BUSY]));
}

/*
   Block unlock functions for leakage load.
*/

//-----------------------------------------------------------------------------
/*!
  @brief
    <SST> Block unlock procedure
  @param[in] MTDData Pointer to the MTD driver data
  @retval
    FS_NO_ERROR: Successful
  @remarks
    Caller: SF_DAL_IOCtrl()
    GPRAM utilization is 1+1B. A device lock is not necessary for GPRAM.
*/
#if defined(SF_DAL_SST)
ATTR_TEXT_IN_RAM int SF_DAL_UnlockBlock_SST(void *MTDData)
{
    SF_MTD_Data    *D = (SF_MTD_Data *)MTDData;
    uint32_t      byte_BPR, i;
    uint32_t     *p_ram = (unsigned int *)RW_SFI_GPRAM_DATA;

    SFI_Dev_Command(D->CS, D->CMD->WriteEnable);
    SF_DAL_Dev_WaitReady_EMIINIT(D);

    byte_BPR = (D->BPRBitCount + 7) / 8;

    // clear all block protection bits
    SFI_MacEnable(D->CS);
    SFI_WriteReg32(p_ram++, SF_CMD_WBPR);           // including 3 bytes zeros

    for (i = 0; i < byte_BPR - 3; i += 4, p_ram++) { // 3 bytes are with command
        SFI_WriteReg32(p_ram, 0);
    }

    SFC->RW_SF_MAC_OUTL = 1 + byte_BPR;
    SFC->RW_SF_MAC_INL = 0;
    SFI_MacWaitReady(D->CS);
    SF_DAL_Dev_WaitReady_EMIINIT(D);

    return FS_NO_ERROR;
}

#endif   // defined(SF_DAL_SST)

//-----------------------------------------------------------------------------
/*!
  @brief
    <MXIC><EON> Block unlock procedure
  @param[in] MTDData Pointer to the MTD driver data
  @retval
    FS_NO_ERROR: Successful
  @remarks
    Caller: SF_DAL_IOCtrl()
    GPRAM utilization is 1+1B. A device lock is not necessary for GPRAM.
*/
#if defined(SF_DAL_MXIC) || defined(SF_DAL_EON) || defined(SF_DAL_WINBOND)|| defined(SF_DAL_ESMT)
ATTR_TEXT_IN_RAM int SF_DAL_UnlockBlock_MXIC(void *MTDData)
{
    SF_MTD_Data  *D  = MTDData;
    uint32_t    unlock;
    uint8_t     cmd;
    if (D->FlashID[0] == 0xC2 && D->FlashID[1] == 0x20 && D->FlashID[2] == 0x16) {
        return FS_NO_ERROR;    //MXIC 3.0V 24Mbits flash program status register will cause Program fail bit raised in security register.
    }

    // Write Enable
    SFI_Dev_Command(D->CS, D->CMD->WriteEnable);
    SF_DAL_Dev_WaitReady_EMIINIT(D);

    // Read BP Register and clear all block protect bits
    SFI_Dev_Command_Ext(D->CS, &D->CMD->ReadBPReg, &cmd, 1, 1);
    unlock = D->CMD->WriteBPReg;
    unlock = (unlock << SFC_GENERIC_8_BIT_OFFSET) | (cmd & ~(D->StatusMap[SF_SR_BP] | D->StatusMap[SF_SR_SRWD]));

    // Write BP Register
    SFI_Dev_Command(D->CS, unlock);
    SF_DAL_Dev_WaitReady_EMIINIT(D);

    return FS_NO_ERROR;
}

#endif // defined(SF_DAL_MXIC) || defined(SF_DAL_EON) || defined(SF_DAL_WINBOND)

#else //! HAL_FLASH_MODULE_ENABLED

void Flash_ReturnReady(void)
{
    return;
}

#endif //#ifdef HAL_FLASH_MODULE_ENABLED

