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
 
#include "hal_flash_custom_memorydevice.h"
#include "hal_flash_opt_gen.h"

/*
 *******************************************************************************
 PART 2:
   FLASH FDM FEATURE CONFIG PARAMETERS translated from Manual custom_Memorydevice.h
 *******************************************************************************
*/

/* NOTE:
   BUFFER_PROGRAM_ITERATION_LENGTH specifies the maximum interrupt
   disabled period while buffer program operation is applied
*/

/* NOTE:
   ERASE_QUEUE_SIZE specifies the number of reserved blocks,
   to trade off between write throughput and available FAT space
*/
#ifndef NOR_FDM4_ESB_PARAMETER_ERASE_QUEUE_SIZE
#define SNOR_ERASE_QUEUE_SIZE   (5)
#else  /* NOR_FDM4_ESB_PARAMETER_ERASE_QUEUE_SIZE */
#define SNOR_ERASE_QUEUE_SIZE   (NOR_FDM4_ESB_PARAMETER_ERASE_QUEUE_SIZE)
#endif /* NOR_FDM4_ESB_PARAMETER_ERASE_QUEUE_SIZE */



/*
 *******************************************************************************
 PART 3:
   FLASH GEOMETRY translated from MEMORY DEVICE DATABASE
 *******************************************************************************
*/


/*
 *******************************************************************************
 PART 4:
   FLASH FAT CONFIG translated from Manual custom_Memorydevice.h
 *******************************************************************************
*/

/* Generic NOR-XIP Case */

/*-----------------------------------------------------------------------------------------
 * FS base address is automatically adjusted with update package storage size as
 * NOR_BOOTING_NOR_FS_BASE_ADDRESS + FOTA_PACKAGE_STORAGE_SIZE (defined in custom_img_config.h)
 *-----------------------------------------------------------------------------------------*/

#ifndef NOR_BOOTING_NOR_FS_BASE_ADDRESS
#define NOR_FLASH_BASE_ADDRESS     (NOR_FLASH_BASE_ADDRESS_DEFAULT + FOTA_DM_FS_OFFSET)
#else
#define NOR_FLASH_BASE_ADDRESS     (NOR_BOOTING_NOR_FS_BASE_ADDRESS + FOTA_DM_FS_OFFSET)
#endif   /* !NOR_FLASH_BASE_ADDRESS */

/*-----------------------------------------------------------------------------------------
 * FS size is automatically decreased by update package storage size as
 * NOR_BOOTING_NOR_FS_SIZE - FOTA_PACKAGE_STORAGE_SIZE (defined in custom_img_config.h)
 *-----------------------------------------------------------------------------------------*/
#ifdef __NVRAM_BACKUP_PARTITION__
#include "nvram_lid_statistics.h"

#ifndef NOR_BOOTING_NOR_DISK_NUM
#define NOR_BOOTING_NOR_DISK_NUM   (1)
#endif

#ifndef NOR_BOOTING_NOR_DISK0_SIZE
#define NOR_BOOTING_NOR_DISK0_SIZE  ((NVRAM_BACKUP_PARTITION_SIZE + NOR_DISK0_BLOCK_SIZE - 1) & ~(NOR_DISK0_BLOCK_SIZE - 1))
#else
#if NOR_BOOTING_NOR_DISK0_SIZE < ((NVRAM_BACKUP_PARTITION_SIZE + NOR_DISK0_BLOCK_SIZE - 1) & ~(NOR_DISK0_BLOCK_SIZE - 1))
#error "Backup partition size is not enough, please modify custom_memoryDevice.h and enlarge NOR_BOOTING_NOR_DISK0_SIZE."
#endif
#endif

#ifndef NOR_BOOTING_NOR_DISK0_BASE_ADDRESS
#define NOR_BOOTING_NOR_DISK0_BASE_ADDRESS  (NOR_FLASH_BASE_ADDRESS_DEFAULT + NOR_ALLOCATED_FAT_SPACE_DEFAULT - NOR_BOOTING_NOR_DISK0_SIZE)
#endif



#if   defined(NOR_BOOTING_NOR_FS_SIZE) && (NOR_FLASH_BASE_ADDRESS + NOR_BOOTING_NOR_FS_SIZE > NOR_BOOTING_NOR_DISK0_BASE_ADDRESS)
#define FS_BKP_OVERLAP_SIZE (NOR_FLASH_BASE_ADDRESS + NOR_BOOTING_NOR_FS_SIZE - NOR_BOOTING_NOR_DISK0_BASE_ADDRESS)
#elif !defined(NOR_BOOTING_NOR_FS_SIZE) && (NOR_FLASH_BASE_ADDRESS + NOR_ALLOCATED_FAT_SPACE_DEFAULT > NOR_BOOTING_NOR_DISK0_BASE_ADDRESS )
#define FS_BKP_OVERLAP_SIZE (NOR_FLASH_BASE_ADDRESS + NOR_ALLOCATED_FAT_SPACE_DEFAULT - NOR_BOOTING_NOR_DISK0_BASE_ADDRESS)
#else
#define FS_BKP_OVERLAP_SIZE 0
#endif

#ifdef DISK0_REGION_INFO_LAYOUT
#undef DISK0_REGION_INFO_LAYOUT
#endif

#define DISK0_REGION_INFO_LAYOUT {(NOR_FLASH_BASE_ADDRESS_DEFAULT + NOR_ALLOCATED_FAT_SPACE_DEFAULT - ((NVRAM_BACKUP_PARTITION_SIZE + NOR_BLOCK_SIZE - 1) & ~(NOR_BLOCK_SIZE - 1))), NOR_DISK0_BLOCK_SIZE}
#endif /* __NVRAM_BACKUP_PARTITION__ */



#ifndef NOR_BOOTING_NOR_FS_SIZE

#define NOR_ALLOCATED_FAT_SPACE    (NOR_ALLOCATED_FAT_SPACE_DEFAULT - FOTA_DM_FS_OFFSET)

#else
#define NOR_ALLOCATED_FAT_SPACE    (NOR_BOOTING_NOR_FS_SIZE - FOTA_DM_FS_OFFSET)

#endif   /* !NOR_ALLOCATED_FAT_SPACE */

/*-----------------------------------------------------------------------------------------
 * FS first drive size is automatically decreased by update package storage size as
 * NOR_BOOTING_NOR_FS_FIRST_DRIVE_SECTORS - FOTA_PACKAGE_STORAGE_SIZE / 512 (defined in custom_img_config.h)
 *-----------------------------------------------------------------------------------------*/

#ifndef NOR_BOOTING_NOR_FS_FIRST_DRIVE_SECTORS
#define NOR_PARTITION_SECTORS      (0)
#else
#define NOR_PARTITION_SECTORS      (NOR_BOOTING_NOR_FS_FIRST_DRIVE_SECTORS - FOTA_DM_FS_SECTOR_OFFSET)
#endif   /* !NOR_FAT_PARTITION_SECTORS */


/*
 * Define __PUBLIC_DRIVE_AVAILABLE__ if public drive is available.
 * This definition will be referenced by File System. Please inform FS owner if any changes.
 */

#if defined(NOR_PARTITION_SECTORS)  // NOR flash has public drive
#if (NOR_PARTITION_SECTORS > 0)
#if ((NOR_BOOTING_NOR_FS_FIRST_DRIVE_SECTORS - 0) < 21)
#error "custom\system\{project}\hal_flash_custom_memorydevice.h: NOR_BOOTING_NOR_FS_FIRST_DRIVE_SECTORS should be zero Or (NOR_BOOTING_NOR_FS_FIRST_DRIVE_SECTORS - 0) should be more than 20 sectors."
#endif

#ifndef __PUBLIC_DRIVE_AVAILABLE__
#define __PUBLIC_DRIVE_AVAILABLE__
#endif
#endif
#endif


