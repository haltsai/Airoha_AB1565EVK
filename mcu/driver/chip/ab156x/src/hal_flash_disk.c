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

#include "hal_flash.h" //MT2523 common flash IOT API
#ifdef HAL_FLASH_MODULE_ENABLED

/********************************************************/
/*                  Include Header Files                */
/********************************************************/

#include "hal_flash_custom_memorydevice.h"
#include "string.h"
#include "hal_flash_disk_internal.h"
#include "hal_flash_mtd_internal.h"
#include "hal_flash_disk.h"
#include "hal_flash_mtd_sf_dal.h"
#include "hal_flash.h"
#include "memory_attribute.h"
#include "hal_cache.h"
#include "hal_nvic.h"
#include "hal_gpt.h"
#include "hal_log.h"
#include "hal_core_status.h"
#ifndef __EXT_DA__
#include "exception_handler.h"
#endif
#ifdef MTK_LAYOUT_PARTITION_ENABLE
#include "layout_partition.h"
#endif

extern NOR_FLASH_DISK_Data EntireDiskDriveData;

#define ENTIRE_DISK_DRIVER_DATA &EntireDiskDriveData


#define RAW_DISK0_DRIVER_DATA &FlashDiskDriveData[RAW_DISK0]
#define TC01_DISK_DRIVER_DATA &FlashDiskDriveData[TC01_DISK]


/*******************************************************************//**
 * Invoke before flash read/write/erase operation.
 * used when __NOR_SUPPORT_RAW_DISK__ or __UBL_NOR_FULL_DRIVER__ is defined.
 * accessed by backup partition and LG partition. (!__UBL__)
 * also access LG partition in bootloader (__UBL__ || __UBL_NOR_FULL_DRIVER__)
 **********************************************************************/
static void MountDevice(NOR_FLASH_DISK_Data *D)
{
    int32_t Result;

    if (D->is_mount) {
        return;
    }

    Result = D->MTDDriver->MountDevice(D->MTDData, (void *)&D->FlashInfo);
    if (Result < FS_NO_ERROR) {
        return;
    }

    D->is_mount = true;
}


/*******************************************************************//**
 * Read data from flash raw disk
 *
 * @par Catagory:
 * NOR RAW DISK
 *
 * @param[in] disk The number of disk to be read
 * @param[in] addr Start address relative to specified disk
 * @param[in] data_ptr Buffer for storing read data
 * @param[in] len The size of data_ptr
 *
 * @remarks
 * 1. File system non-block operation will be finished and then do raw disk read opertaion
 *
 * @return
 * RAW_DISK_ERR_WRONG_ADDRESS: Illegal read address
 * RAW_DISK_ERR_NONE: Read successful
 **********************************************************************/
ATTR_TEXT_IN_RAM int32_t readRawDiskData(NOR_FLASH_DISK_Data *D, uint32_t addr, uint8_t *data_ptr, uint32_t len)
{
    uint32_t phyAddr;
    uint32_t diskSize = D->DiskSize;

    if ((addr >= diskSize) || (len > diskSize) || ((addr + len) > diskSize)) {
        return RAW_DISK_ERR_WRONG_ADDRESS;
    }
    phyAddr = (uint32_t)((NOR_Flash_MTD_Data *)D->MTDData)->BaseAddr + addr;

    if (!D->is_mount) {
        MountDevice(D);
        if (!D->is_mount) {
            return ERROR_NOR_READ;
        }
    }

    memcpy(data_ptr, (void *)phyAddr, len);

    return RAW_DISK_ERR_NONE;
}


#ifdef __NOR_FULL_DRIVER__

/*******************************************************************//**
 * Write data into flash raw disk
 *
 * @par Catagory:
 * NOR RAW DISK
 *
 * @param[in] disk The number of disk to be read
 * @param[in] addr Start address relative to specified disk
 * @param[in] data_ptr Buffer for storing read data
 * @param[in] len The size of data_ptr
 *
 * @remarks
 * 1. File system non-block operation will be finished and then do raw disk read opertaion
 *
 * @return
 * RAW_DISK_ERR_WRONG_ADDRESS: Illegal program start address
 * RAW_DISK_ERR_NO_SPACE: No enough space to write len bytes data
 * RAW_DISK_ERR_PROG_FAIL: Program fail
 * RAW_DISK_ERR_NONE: Program successful
 **********************************************************************/


int32_t writeRawDiskData(NOR_FLASH_DISK_Data *D, uint32_t addr, uint8_t *data_ptr, uint32_t len)
{
    uint32_t dest, src;
    uint32_t diskSize = D->DiskSize;
    int32_t result = FLASH_DISK_DONE;
    uint32_t DoneLength = 0;
    uint32_t mask;

    if (addr >= diskSize) {
        return RAW_DISK_ERR_WRONG_ADDRESS;
    }

    if ((len > diskSize) || ((addr + len) > diskSize)) {
        return RAW_DISK_ERR_NO_SPACE;
    }

    if (!D->is_mount) {
        MountDevice(D);
        if (!D->is_mount) {
            return ERROR_NOR_PROGRAM;
        }
    }

    while (DoneLength < len) {
        dest = (uint32_t)((NOR_Flash_MTD_Data *)D->MTDData)->BaseAddr + addr + DoneLength;
        src = (uint32_t)data_ptr + DoneLength;
        hal_nvic_save_and_set_interrupt_mask(&mask);
        D->MTDDriver->MapWindow(D->MTDData, BlockIndex((void *)D->MTDData, addr + DoneLength), 0);
        hal_nvic_restore_interrupt_mask(mask);

        // if dest address not word align or write length is one, write one byte at a time
        if (((uint32_t)dest & (sizeof(uint16_t) - 1)) || ((len - DoneLength) == 1)) {
            result = D->MTDDriver->ProgramData(D->MTDData, (void *)dest, (void *)src, 1);
            if (result != FLASH_DISK_DONE) {
                break;
            }
            DoneLength++;
        } // dest address is word align
        else {
            uint32_t blockSize = BlockSize(D->MTDData, BlockIndex(D->MTDData, addr + DoneLength));
            uint32_t programBytes = len - DoneLength;
            //calculate the block address boundary
            uint32_t nextBlkAddr = (dest + blockSize) & (~(blockSize - 1));

            //uint32_t next128ByteAddr = (dest+ 128) & (~(128 -1));


            //if the data write across block boundary, shrink into a block
            if (dest + programBytes > (nextBlkAddr)) {
                programBytes = nextBlkAddr - dest;
            }

            //round down to multiple of word
            programBytes = (programBytes) & (~(sizeof(uint16_t) - 1));

            // program a word should be word align (MTD limitation)
            if (programBytes == sizeof(uint16_t) && (src % sizeof(uint16_t))) {
                uint16_t Cell;
                uint8_t *b = (uint8_t *) &Cell;
                b[0] = *((uint8_t *)src);
                b[1] = *((uint8_t *)src + 1);
                result = D->MTDDriver->ProgramData(D->MTDData, (void *)dest, (void *)&Cell, programBytes);
                if (result != FLASH_DISK_DONE) {
                    break;
                }
            } else {
                if (programBytes & 0x1) { //must be Sibley flash
                    log_hal_msgid_error("writeRawDiskData error programBytes = %d \r\n", 1, programBytes);
                } else {
                    result = D->MTDDriver->ProgramData(D->MTDData, (void *)dest, (void *)src, programBytes);
                }
                if (result != FLASH_DISK_DONE) {
                    break;
                }
            }
            DoneLength += programBytes;
        }
    }


    if (result != FLASH_DISK_DONE) {
        return RAW_DISK_ERR_PROG_FAIL;
    }
    return RAW_DISK_ERR_NONE;

}

/*******************************************************************//**
 * Erase a block of flash raw disk
 *
 * @par Catagory:
 * NOR RAW DISK
 *
 * @param[in] disk The number of disk to be read
 * @param[in] blkIdx The block index to be erased
 *
 * @remarks
 * 1. File system non-block operation will be finished and then do raw disk read opertaion
 *
 * @return
 * RAW_DISK_ERR_WRONG_ADDRESS: Illegal block index to be erased
 * RAW_DISK_ERR_ERASE_FAIL: Erase fail
 * RAW_DISK_ERR_NONE: Program successful
 **********************************************************************/
int32_t eraseRawDiskBlock(NOR_FLASH_DISK_Data *D, uint32_t blkIdx)
{

    int32_t result;
    uint32_t block_index = (blkIdx & 0xFFFFFFF);
    if (!D->is_mount) {
        MountDevice(D);
        if (!D->is_mount) {
            return ERROR_NOR_ERASE;
        }
    }

    if (block_index >= D->FlashInfo.TotalBlocks) { //TotalBlocks are initialized in MountDevice()
        return RAW_DISK_ERR_WRONG_ADDRESS;
    }

    result = D->MTDDriver->EraseBlock(D->MTDData, blkIdx);

    if (result == FLASH_DISK_DONE) {
        return RAW_DISK_ERR_NONE;
    }
    return RAW_DISK_ERR_ERASE_FAIL;
}

#endif



extern int32_t Custom_NOR_Init(void);
extern uint8_t nor_id[4];
extern int32_t cmem_nor_index;

hal_flash_status_t flash_init_status = HAL_FLASH_STATUS_ERROR_NO_INIT;
int32_t NOR_init(void)
{
    int32_t result = ERROR_NOR_SUCCESS;
    //only init flash one time
    if (flash_init_status == HAL_FLASH_STATUS_ERROR_NO_INIT) {
        result = Custom_NOR_Init();
        if (result != 0) {
            log_hal_msgid_error("NOR_init failed: %d, ID: 0x%x,0x%x,0x%x\r\n", 4, result, nor_id[0], nor_id[1], nor_id[2]);
            ASSERT_RET(0, HAL_FLASH_STATUS_ERROR);
            return result;
        } else {
            log_hal_msgid_info("NOR_init ok. Idx: %d, ID: 0x%x 0x%x 0x%x\r\n", 4, cmem_nor_index, nor_id[0], nor_id[1], nor_id[2]);
        }
        MountDevice(ENTIRE_DISK_DRIVER_DATA);
        flash_init_status = HAL_FLASH_STATUS_OK;
    }
    return ERROR_NOR_SUCCESS;
}

ATTR_TEXT_IN_RAM hal_flash_status_t get_NOR_init_status(void)
{
    return flash_init_status;
}

int32_t NOR_ReadPhysicalPage(uint32_t block_idx, uint32_t page_idx, uint8_t *data_ptr)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    NOR_Flash_MTD_Data *mtdData = (NOR_Flash_MTD_Data *)D->MTDData;
    int32_t result;

    uint32_t addr = ((uint32_t)BlockAddress((void *)mtdData , block_idx) - (uint32_t)mtdData->BaseAddr)  + (page_idx << NOR_FLASH_PAGE_SHIFT);

    result = readRawDiskData(D, addr, data_ptr, NOR_FLASH_PAGE_SIZE);

    if (result != RAW_DISK_ERR_NONE) {
        return ERROR_NOR_READ;
    }
    return ERROR_NOR_SUCCESS;
}

#if defined(__NOR_FULL_DRIVER__)
int32_t NOR_ProgramPhysicalPage(uint32_t block_idx, uint32_t page_idx, uint8_t *data_ptr)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    NOR_Flash_MTD_Data *mtdData = (NOR_Flash_MTD_Data *)D->MTDData;
    int32_t result;

    uint32_t addr = ((uint32_t)BlockAddress((void *)mtdData , block_idx) - (uint32_t)mtdData->BaseAddr)  + (page_idx << NOR_FLASH_PAGE_SHIFT);

    result = writeRawDiskData(D, addr, data_ptr, NOR_FLASH_PAGE_SIZE);
    if (result != RAW_DISK_ERR_NONE) {
        return ERROR_NOR_PROGRAM;
    }
    return ERROR_NOR_SUCCESS;
}

int32_t NOR_ErasePhysicalBlock(uint32_t block_idx)
{
    int32_t result;
    result = eraseRawDiskBlock(ENTIRE_DISK_DRIVER_DATA, block_idx);
    if (result != RAW_DISK_ERR_NONE) {
        return ERROR_NOR_ERASE;
    }
    return ERROR_NOR_SUCCESS;
}
#endif //__UBL_NOR_FULL_DRIVER__

uint32_t NOR_BlockSize(uint32_t block_idx)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    NOR_Flash_MTD_Data *mtdData = (NOR_Flash_MTD_Data *)D->MTDData;
    return BlockSize(mtdData, block_idx);
}

uint32_t NOR_BlockIndex(uint32_t block_addr)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    NOR_Flash_MTD_Data *mtdData = (NOR_Flash_MTD_Data *)D->MTDData;

    return BlockIndex(mtdData, block_addr);
}

ATTR_TEXT_IN_RAM int32_t NOR_EraseChip()
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    NOR_Flash_MTD_Data *mtdData = (NOR_Flash_MTD_Data *)D->MTDData;
    return SF_DAL_Erase_chip(mtdData, 0);
}

// Return value:
//    ERROR_NOR_OVERRANGE: address out of NOR flash size
//    ERROR_NOR_SUCCESS: block_addr to *block_index, *offset translation successful
int32_t NOR_Addr2BlockIndexOffset(uint32_t block_addr, uint32_t *block_index, uint32_t *offset)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    NOR_Flash_MTD_Data *mtdData = (NOR_Flash_MTD_Data *)D->MTDData;

#ifdef __UBL__
    block_addr = block_addr & (~(SFC_GENERIC_FLASH_BANK_MASK));
#endif

    *block_index = BlockIndex(mtdData, block_addr);
    if (*block_index == INVALID_BLOCK_INDEX) {
        return ERROR_NOR_OVERRANGE;
    }

    *offset = block_addr % BlockSize(mtdData, *block_index);

    return ERROR_NOR_SUCCESS;
}

// Translate block_index and offset to block address (*addr)
// Return value:
//    ERROR_NOR_SUCCESS
int32_t NOR_BlockIndexOffset2Addr(uint32_t block_index, uint32_t offset, uint32_t *addr)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    NOR_Flash_MTD_Data *mtdData = (NOR_Flash_MTD_Data *)D->MTDData;

    *addr = (uint32_t)BlockAddress(mtdData, block_index) - (uint32_t)mtdData->BaseAddr + offset;

    return ERROR_NOR_SUCCESS;
}

extern uint8_t org_data;
extern uint8_t readback_data;
extern uint32_t error_addr;
extern uint32_t error_status;
//-----------------------------------------------------------------------------------------------
// MT2523 Raw Disk API for IOT
//-----------------------------------------------------------------------------------------------
hal_flash_status_t get_rawdisk_error_code(int32_t ori_err_code)
{
    if (ori_err_code == RAW_DISK_ERR_WRONG_ADDRESS) {
        return HAL_FLASH_STATUS_ERROR_WRONG_ADDRESS;
    } else if (ori_err_code == RAW_DISK_ERR_NO_SPACE) {
        return HAL_FLASH_STATUS_ERROR_NO_SPACE;
    } else if (ori_err_code == RAW_DISK_ERR_PROG_FAIL) {
        log_hal_msgid_error("address:0x%x, write:0x%x, readback:0x%x, SR = 0x%x\r\n", 4, error_addr, org_data, readback_data, error_status);
        if((org_data | readback_data) != org_data) {
            log_hal_msgid_error("write 0 to 1 -> wrote data changed during writing\r\n", 0);
        }
        return HAL_FLASH_STATUS_ERROR_PROG_FAIL;
    } else if (ori_err_code == RAW_DISK_ERR_ERASE_FAIL) {
        return HAL_FLASH_STATUS_ERROR_ERASE_FAIL;
    } else if (ori_err_code == HAL_FLASH_STATUS_ERROR_LOCKED) { //mutex lock
        return HAL_FLASH_STATUS_ERROR_ERASE_FAIL;
    } else {
        return HAL_FLASH_STATUS_ERROR_ERASE_FAIL;
    }
}
hal_flash_block_t get_block_size_from_address(uint32_t address)
{
    uint32_t block_size;
    uint32_t block_index;

    block_index = NOR_BlockIndex(address);
    if (block_index == INVALID_BLOCK_INDEX) {
        return HAL_FLASH_BLOCK_4K;
    }
    block_size = NOR_BlockSize(block_index);
    if (block_size == 0x1000) {
        return HAL_FLASH_BLOCK_4K;
    } else if (block_size == 0x8000) {
        return HAL_FLASH_BLOCK_32K;
    } else if (block_size == 0x10000) {
        return HAL_FLASH_BLOCK_64K;
    } else {
        return HAL_FLASH_BLOCK_4K;;
    }
}

extern uint32_t sf_erase_flag;
extern uint32_t sf_program_flag;
extern bool NOR_FLASH_BUSY;
extern uint32_t sf_erase_suspend;
extern uint32_t sf_wait_count_check_ready;
extern uint32_t sf_wait_count_check_ready_and_resume;

ATTR_RWDATA_IN_NONCACHED_RAM volatile bool sfc_interrupted_by_ch2_or_ch3 = false;
ATTR_TEXT_IN_SYSRAM void sfc_isr(hal_nvic_irq_t irq_number)
{
    uint32_t mac_ctl = 0;
    uint32_t count;
    uint32_t mask;
    uint32_t write_count = 0;

    //hal_nvic_disable_irq(SFC_IRQn);
    *(volatile uint32_t *)0xe000e100 &= ~(0x01000000);
    //handler
    mac_ctl = SFC->RW_SF_MAC_CTL;

    if ((SFC->RW_SF_MAC_CTL) & SFI_IRQ_ABORT) {
        /*clear flag must write corresponding bit 1 and write  0 again, because use rasing edge to clear flag.*/
        log_hal_msgid_error("abort: SFC+0 = 0x%x, SFC+0x40 = 0x%x, SFC+0x44 = 0x%x\r\n", 3, SFC->RW_SF_MAC_CTL, SFC->RW_SF_MISC_CTL3, *(volatile uint32_t *)0xA0020060);
        SFC->RW_SF_MAC_CTL |= SFI_ABORT_CLEAR;
        SFC->RW_SF_MAC_CTL &= (~SFI_ABORT_CLEAR);
    }

    if (mac_ctl & (SFI_CH3_AHB_IS_HANGING_FLAG | SFI_CH2_AHB_IS_HANGING_FLAG | SFI_CH1_AHB_IS_HANGING_FLAG)) {
        /*clear flag must write corresponding bit 1 and write  0 again, because use rasing edge to clear flag.
          Write ten times from DE advice */
        hal_nvic_save_and_set_interrupt_mask(&mask);
        while(write_count < 10) {
            SFC->RW_SF_MAC_CTL |= (SFI_CLEAR_CH3_HANGING_FLAG | SFI_CLEAR_CH2_HANGING_FLAG | SFI_CLEAR_CH1_HANGING_FLAG);
            write_count++;
        }

        write_count = 0;
        while(write_count < 10) {
            SFC->RW_SF_MAC_CTL &= ~(SFI_CLEAR_CH3_HANGING_FLAG | SFI_CLEAR_CH2_HANGING_FLAG | SFI_CLEAR_CH1_HANGING_FLAG);
            write_count++;
        }
        hal_nvic_restore_interrupt_mask(mask);

        sfc_interrupted_by_ch2_or_ch3 = true;

        //sfc_timer_flag = false;
        //Set the timeout to 10 ms, and register the callback at the same time
        //hal_gpt_start_timer_ms(HAL_GPT_1, 1000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
    }

    SFC->RW_SF_MAC_CTL |= SFI_IRQ_ACK;
    SFC->RW_SF_MAC_CTL &= ~SFI_IRQ_ACK;
    //enable SFC_IRQn
    *(volatile uint32_t *)0xe000e100 |= 0x01000000;

    if ((sfc_interrupted_by_ch2_or_ch3 == true) && NOR_FLASH_BUSY) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &count);
        log_hal_msgid_info("[sfc_isr]: count = %d, MISC_CTL3 = 0x%x, Erase_flag = 0x%x, W_Flag = 0x%x,wait-cnt1 = %x, wait-cnt2 = %x\r\n", 6,
          (unsigned int)count, SFC->RW_SF_MISC_CTL3, sf_erase_flag, sf_program_flag, sf_wait_count_check_ready, sf_wait_count_check_ready_and_resume);
    }
}


void sfc_interrupt_init(void)
{
#ifndef __UBL__

    //if(HAL_GPT_STATUS_OK != hal_gpt_init(HAL_GPT_1)) {
    //error handler
    //}

    //hal_gpt_register_callback(HAL_GPT_1, sfc_timer_callback, NULL); //Register a user callback.

    hal_nvic_disable_irq(SFC_IRQn);
    hal_nvic_register_isr_handler(SFC_IRQn, sfc_isr);
    SFC->RW_SF_MAC_CTL |= SFI_IRQ_EN;

    SFC->RW_SF_MISC_CTL3 |= SFI_CH1_UNDER_MASK_IRQ_EN | SFI_CH2_UNDER_MASK_IRQ_EN | SFI_CH3_UNDER_MASK_IRQ_EN;
    hal_nvic_enable_irq(SFC_IRQn);
#endif
}

//BL and FreeRTOS always do flash init
hal_flash_status_t hal_flash_init(void)
{
    if ( 0 != NOR_init()) {
        return HAL_FLASH_STATUS_ERROR;
    }
    sfc_interrupt_init();

    return HAL_FLASH_STATUS_OK;
}
//Do nothing for flash deinit
hal_flash_status_t hal_flash_deinit(void)
{
    return HAL_FLASH_STATUS_OK;
}

static int32_t flash_sw_protected_check(uint32_t address)
{
    int32_t result = 0;

#if defined(__UBL__) || defined(__EXT_DA__)
    return result;
#else
    uint32_t index,i;
    typedef struct {
        uint32_t address;
        uint32_t length;
    } flash_sw_protect_area_config_t;

    flash_sw_protect_area_config_t region_cfg_tbl[] = {
        /* Protect areas include: ROM_PARTITION_TABLE + SEC_HEADER1_LENGTH + SEC_HEADER2_LENGTH + BL_LENGTH */
        {ROM_BASE, (0x1000 + SEC_HEADER1_LENGTH + SEC_HEADER2_LENGTH + BL_LENGTH)},
        {DSP0_BASE, DSP0_LENGTH},
        {CM4_BASE, CM4_LENGTH},
    };

    address = address | ROM_BASE;
    index = (sizeof(region_cfg_tbl) / sizeof(region_cfg_tbl[0]));
    for(i = 0; i < index; i++) {
        if ((address >= region_cfg_tbl[i].address) && (address < ( region_cfg_tbl[i].address + region_cfg_tbl[i].length))) {
            result = 1;
        }
    }
    return result;
#endif
}

//Notice: please call get_block_size_from_address() first to get block size
hal_flash_status_t hal_flash_erase(uint32_t start_address, hal_flash_block_t block)
{
    int32_t result;
    uint32_t block_index;
    uint32_t erase_type = 0;
    int32_t address;
    int32_t length = 0;
    hal_flash_status_t status = HAL_FLASH_STATUS_OK;
    uint32_t erase_time, erase_count_s,erase_count_e;

#ifndef __EXT_DA__
    if (0 != flash_sw_protected_check(start_address)){
        if (HAL_CORE_EXCEPTION == hal_core_status_read(HAL_CORE_CM4)) {
            exception_printf("[hal_flash_erase] address: 0x%x , LR: 0x%x \r\n", (unsigned int)start_address, (unsigned int)__builtin_return_address(0));
        } else {
            log_hal_msgid_error("[hal_flash_erase] address: 0x%x , LR: 0x%x \r\n", 2, start_address, (uint32_t)__builtin_return_address(0));
        }
        assert(0);
        return HAL_FLASH_STATUS_ERROR;
    }
#endif

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &erase_count_s);
    if (HAL_FLASH_BLOCK_4K == block) {
        length = 0x1000;
        if ((start_address & 0xFFF) != 0) {
            status = HAL_FLASH_STATUS_ERROR_WRONG_ADDRESS;
        }
        erase_type = 0x1;
    } else if (HAL_FLASH_BLOCK_32K == block) {
        length = 0x8000;
        if ((start_address & 0x7FFF) != 0) {
            status = HAL_FLASH_STATUS_ERROR_WRONG_ADDRESS;
        }
        erase_type = 0x2;
    } else if (HAL_FLASH_BLOCK_64K == block) {
        length = 0x10000;
        if ((start_address & 0xFFFF) != 0) {
            status = HAL_FLASH_STATUS_ERROR_WRONG_ADDRESS;
        }
        erase_type = 0x4;
    } else {
        status = HAL_FLASH_STATUS_ERROR_WRONG_ADDRESS;
    }

#ifdef MTK_LAYOUT_PARTITION_ENABLE
    bool is_readonly = false;
    start_address |= HAL_FLASH_BASE_ADDRESS;
    lp_is_readonly(start_address, length, &is_readonly);
    assert(is_readonly == false);
#endif

    start_address = start_address & (~HAL_FLASH_BASE_ADDRESS);
    block_index = NOR_BlockIndex(start_address);
    if (block_index == INVALID_BLOCK_INDEX) {
        status = HAL_FLASH_STATUS_ERROR_WRONG_ADDRESS;
    }

    block_index |= (erase_type << 28);
    log_hal_msgid_info("[hal_flash_erase] address = 0x%x, block = 0x%x, suspend = %d \r\n", 3, start_address, block_index, sf_erase_suspend);
    if (status != HAL_FLASH_STATUS_OK) {
        log_hal_msgid_error("[hal_flash_erase] error=%d, address=0x%x, block=0x%x \r\n", 3, status, start_address, block_index);
        return status;
    }
    result = eraseRawDiskBlock(ENTIRE_DISK_DRIVER_DATA, block_index);
    if (result != RAW_DISK_ERR_NONE) {
        log_hal_msgid_error("[hal_flash_erase] error=%d, address=0x%x, block=0x%x, suspend =%d \r\n", 4, result, start_address, block_index, sf_erase_suspend);
        return get_rawdisk_error_code(result);
    }

#ifdef HAL_CACHE_MODULE_ENABLED
    address = start_address | HAL_FLASH_BASE_ADDRESS;

    /* Address should be alignment with cashe line size*/
    if (hal_cache_is_cacheable(address)) {
        uint32_t addr;
        for (addr = (address & CACHE_LINE_ALIGNMENT_MASK); addr <= ((address + length + CACHE_LINE_SIZE) & CACHE_LINE_ALIGNMENT_MASK); addr += CACHE_LINE_SIZE) {
            hal_cache_invalidate_one_cache_line(addr);
        }
    }
#endif
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &erase_count_e);
    hal_gpt_get_duration_count(erase_count_s, erase_count_e, &erase_time);
    log_hal_msgid_info("[hal_flash_erase] address = 0x%x, block = %d suspend = %d, time = %d result ok\r\n", 4, start_address, block_index, sf_erase_suspend, erase_time);
    return HAL_FLASH_STATUS_OK;
}

hal_flash_status_t hal_flash_read(uint32_t start_address, uint8_t *buffer, uint32_t length)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    int32_t result;

    start_address = start_address & (~HAL_FLASH_BASE_ADDRESS);
    result = readRawDiskData(D, start_address, buffer, length);

    if (result != RAW_DISK_ERR_NONE) {
        return get_rawdisk_error_code(result);
    }
    return HAL_FLASH_STATUS_OK;
}

hal_flash_status_t hal_flash_write(uint32_t address, const uint8_t *data, uint32_t length)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    int32_t result;
    uint32_t erase_time, erase_count_s,erase_count_e;

#ifndef __EXT_DA__
    if (0 != flash_sw_protected_check(address)) {
        if (HAL_CORE_EXCEPTION == hal_core_status_read(HAL_CORE_CM4)){
            exception_printf("[hal_flash_write] address: 0x%x, LR: 0x%x\r\n", (unsigned int)address, (unsigned int)__builtin_return_address(0));
        } else {
            log_hal_msgid_error("[hal_flash_write] address: 0x%x, LR: 0x%x\r\n", 2, address, (uint32_t)__builtin_return_address(0));
        }
        assert(0);
        return HAL_FLASH_STATUS_ERROR;
    }
#endif

    if (hal_core_status_read(HAL_CORE_CM4) == HAL_CORE_ACTIVE) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &erase_count_s);
    }
#ifdef MTK_LAYOUT_PARTITION_ENABLE
    bool is_readonly = false;
    address |= HAL_FLASH_BASE_ADDRESS;
    lp_is_readonly(address, length, &is_readonly);
    assert(is_readonly == false);
#endif

    address = (address & (~HAL_FLASH_BASE_ADDRESS));
    if (hal_core_status_read(HAL_CORE_CM4) == HAL_CORE_ACTIVE) {
        log_hal_msgid_info("[hal_flash_write] address=0x%x, length =%d\r\n", 2, address, length);
    }

    result = writeRawDiskData(D, address, (uint8_t *)data, length);
    if (result != RAW_DISK_ERR_NONE) {
        log_hal_msgid_error("[hal_flash_write] error=%d, address=0x%x, len=%d \r\n", 3, result, address, length);
        return get_rawdisk_error_code(result);
    }

    #ifdef HAL_CACHE_MODULE_ENABLED
    address |= HAL_FLASH_BASE_ADDRESS;
    /* Address should be alignment with cashe line size*/
    if(hal_cache_is_cacheable(address)){
       uint32_t addr;
       for(addr=(address&CACHE_LINE_ALIGNMENT_MASK); addr<=((address+length+CACHE_LINE_SIZE) & CACHE_LINE_ALIGNMENT_MASK); addr+=CACHE_LINE_SIZE) {
            hal_cache_invalidate_one_cache_line(addr);
       }
    }
    #endif
    if (hal_core_status_read(HAL_CORE_CM4) == HAL_CORE_ACTIVE) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &erase_count_e);
        hal_gpt_get_duration_count(erase_count_s, erase_count_e, &erase_time);
        log_hal_msgid_info("[hal_flash_write] address=0x%x, length = %d result ok!\r\n", 3, (address & (~HAL_FLASH_BASE_ADDRESS)), length, erase_time);
    }
    return HAL_FLASH_STATUS_OK;
}

/* should be called after hal_flash_init  */
hal_flash_status_t hal_flash_get_disk_size(uint32_t* size)
{
    if (size == NULL) {
        return HAL_FLASH_STATUS_ERROR;
    }
    
    *size = EntireDiskDriveData.DiskSize;
    if (*size == 0) {
        return HAL_FLASH_STATUS_ERROR;
    } else {
        return HAL_FLASH_STATUS_OK;
    }
}

#ifdef __SECURITY_OTP__
#ifdef __SECURITY_OTP_WRITE__

int32_t OTP_Write_Data(NOR_FLASH_DISK_Data *D, uint32_t otp_addr, uint8_t *data_ptr, uint32_t len)
{
    int32_t result = FLASH_DISK_DONE;

    if (otp_addr >= ((SF_MTD_Data *)D->MTDData)->OTPLength) {
        return RAW_DISK_ERR_WRONG_ADDRESS;
    }

    if ((otp_addr + len) > (((SF_MTD_Data *)D->MTDData)->OTPLength + ((SF_MTD_Data *)D->MTDData)->OTPBase)) {
        return RAW_DISK_ERR_NO_SPACE;
    }

    if (!D->is_mount) {
        MountDevice(D);
        if (!D->is_mount) {
            return ERROR_NOR_PROGRAM;
        }
    }

    if (D->MTDDriver->OTPAccess) {
        result = D->MTDDriver->OTPAccess(D->MTDData, FS_OTP_WRITE, (uint32_t)otp_addr, (void *)data_ptr, len);
    }

    if (result != FLASH_DISK_DONE) {
        return RAW_DISK_ERR_PROG_FAIL;
    }
    return RAW_DISK_ERR_NONE;

}
#endif

int32_t OTP_Read_Data(NOR_FLASH_DISK_Data *D, uint32_t otp_addr, uint8_t *data_ptr, uint32_t len)
{
    int32_t result = FLASH_DISK_DONE;

    if (otp_addr >= ((SF_MTD_Data *)D->MTDData)->OTPLength) {
        return RAW_DISK_ERR_WRONG_ADDRESS;
    }

    if ((otp_addr + len) > (((SF_MTD_Data *)D->MTDData)->OTPLength + ((SF_MTD_Data *)D->MTDData)->OTPBase)) {
        return RAW_DISK_ERR_NO_SPACE;
    }

    if (!D->is_mount) {
        MountDevice(D);
        if (!D->is_mount) {
            return RAW_DISK_ERR_PROG_FAIL;
        }
    }

    if (D->MTDDriver->OTPAccess) {
        result = D->MTDDriver->OTPAccess(D->MTDData, FS_OTP_READ, (uint32_t)otp_addr, (void *)data_ptr, len);
    }

    if (result != FLASH_DISK_DONE) {
        return RAW_DISK_ERR_PROG_FAIL;
    }
    return RAW_DISK_ERR_NONE;

}

#ifdef __SECURITY_OTP_WRITE__

int32_t OTP_Lock(NOR_FLASH_DISK_Data *D)
{
    int32_t result = FLASH_DISK_DONE;

    if (!D->is_mount) {
        MountDevice(D);
        if (!D->is_mount) {
            return HAL_FLASH_STATUS_ERROR;
        }
    }

    if (D->MTDDriver->OTPAccess) {
        result = D->MTDDriver->OTPAccess(D->MTDData, FS_OTP_LOCK, 0, NULL, 0);
    }

    if (result != FLASH_DISK_DONE) {
        return RAW_DISK_ERR_PROG_FAIL;
    }
    return RAW_DISK_ERR_NONE;

}


hal_flash_status_t hal_flash_otp_write(uint32_t address, const uint8_t *data, uint32_t length)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    int32_t result;

    result = OTP_Write_Data(D, address, (uint8_t *)data, length);
    if (result != RAW_DISK_ERR_NONE) {
        return get_rawdisk_error_code(result);
    }
    return HAL_FLASH_STATUS_OK;
}


hal_flash_status_t hal_flash_security_register_write(uint32_t bank,uint32_t offset, uint8_t *buffer, uint32_t length)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    uint32_t address;
    // low level driver only supports on GD and Winbond flash;
    // winbond is 3 * 256     GD is 3 * 1024
    // the bank size should be 256 as custom don't care which flash the chip sipped.
    // OTPBase should be 256, otherwise it shold check driver carefully.
    address = bank * (((SF_MTD_Data *)D->MTDData)->OTPBase) + offset;
    return hal_flash_otp_write(address, buffer, length);
}


#if defined(SF_DAL_WINBOND) || defined(SF_DAL_GIGADEVICE)

extern uint32_t SF_DAL_OTP_Size_WINBOND(void *MTDData);
extern int SF_DAL_OTP_LOCK_Status_WINBOND(void *MTDData);

hal_flash_status_t hal_flash_otp_status(uint32_t *lockstatus)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    *lockstatus = SF_DAL_OTP_LOCK_Status_WINBOND(D->MTDData);
    return HAL_FLASH_STATUS_OK;
}

hal_flash_status_t hal_flash_otp_size(uint32_t *size)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    *size = SF_DAL_OTP_Size_WINBOND(D->MTDData);
    return HAL_FLASH_STATUS_OK;
}

extern int SF_DAL_OTP_ERASE_WINBOND(void *MTDData, uint32_t block);

hal_flash_status_t hal_flash_otp_erase(uint32_t bank)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    int32_t result;
    log_hal_msgid_info("hal_flash_otp_erase bank = %d \r\n", 1, bank);
    //it is same for WINBOND and GIGADEVICE
    result = SF_DAL_OTP_ERASE_WINBOND(D->MTDData,bank);
    if (result != RAW_DISK_ERR_NONE) {
        log_hal_msgid_error("hal_flash_otp_erase bank = %d, result = %d \r\n", 2, bank, result);
        return result;
    }
    log_hal_msgid_info("hal_flash_otp_erase bank = %d, result = %d \r\n", 2, bank, result);
    return HAL_FLASH_STATUS_OK;
}

extern int SF_DAL_Flash_Protect(void *MTDData, uint32_t start, uint32_t length);

hal_flash_status_t hal_flash_protect(uint32_t start, uint32_t length)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    int32_t result;
    result = SF_DAL_Flash_Protect(D->MTDData, start, length);

    if (result != RAW_DISK_ERR_NONE) {
        return result;
    }
    return HAL_FLASH_STATUS_OK;
}

extern int SF_DAL_Flash_UnProtect(void *MTDData);
hal_flash_status_t hal_flash_unprotect(void)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    int32_t result;
    result = SF_DAL_Flash_UnProtect(D->MTDData);

    if (result != RAW_DISK_ERR_NONE) {
        return result;
    }
    return HAL_FLASH_STATUS_OK;
}


#endif
#endif

hal_flash_status_t hal_flash_otp_read(uint32_t start_address, uint8_t *buffer, uint32_t length)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    int32_t result;

    result = OTP_Read_Data(D, start_address, buffer, length);
    if (result != RAW_DISK_ERR_NONE) {
        return get_rawdisk_error_code(result);
    }
    return HAL_FLASH_STATUS_OK;
}

hal_flash_status_t hal_flash_security_register_read(uint32_t bank,uint32_t offset, uint8_t *buffer, uint32_t length)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    uint32_t address;
    // low level driver only supports on GD and Winbond flash;
    // winbond is 3 * 256     GD is 3 * 1024
    // the bank size should be 256 as custom don't care which flash the chip sipped.
    // OTPBase should be 256, otherwise it shold check driver carefully.
    address = bank * (((SF_MTD_Data *)D->MTDData)->OTPBase) + offset;
    return hal_flash_otp_read(address, buffer, length);
}

#ifdef __SECURITY_OTP_WRITE__
hal_flash_status_t hal_flash_otp_lock(void)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    int32_t result;

    result = OTP_Lock(D);
    if (result != RAW_DISK_ERR_NONE) {
        return get_rawdisk_error_code(result);
    }
    return HAL_FLASH_STATUS_OK;
}

hal_flash_status_t hal_flash_otp_lock_bank(OTP_BANK_enum bank)
{
    NOR_FLASH_DISK_Data *D = ENTIRE_DISK_DRIVER_DATA;
    int32_t result;
    result = SF_DAL_OTPLock_blank(D->MTDData, bank);
    return result;
}
#endif
#endif

#else //! HAL_FLASH_MODULE_ENABLED
signed int NOR_init(void)
{
    return 0;
}
#endif //#ifdef HAL_FLASH_MODULE_ENABLED

