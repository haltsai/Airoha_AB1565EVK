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



#include <string.h>
#include <stdint.h>
#include "rofs.h"
#include "memory_map.h"

#define ROFS_HEADER_ADDRESS_INVALID 0xFFFFFFFF

struct ROFS_Header
{
    char cMarker[4]; // Fixed as 'R' 'O' 'F' 'S'.
    unsigned long ulNextTableOffset;
    unsigned long ulPartitionSize;
    unsigned long ulFileNumber; // Record files in this table.
};

static uint32_t ROFS_partitionbase(void)
{
#if 0
    PTR_FILE_SYSTEM_PARAMETER pFileSystemParameters;

    ///////////////////////////////////////////////////////
    // Initialize related definitions...
    // Previously are macro definitions, and now changed to use function calls.
    ///////////////////////////////////////////////////////
    pFileSystemParameters=(PTR_FILE_SYSTEM_PARAMETER)EFPT_Parameter(EFPT_FILE_SYSTEM);
    if(pFileSystemParameters==NULL || pFileSystemParameters->fsystemAddr==NULL)
        return 0;
    return (uint32_t)(pFileSystemParameters->fsystemAddr);
    ///////////////////////////////////////////////////////

#endif
    return (uint32_t)ROFS_BASE;

}

/*
NULL: Not found.
*/
ROFS_FILEINFO_T* ROFS_fopen(uint16_t usFileID)
{
    return ROFS_fopen_ext(usFileID, ROFS_partitionbase());
}

uint32_t ROFS_address(ROFS_FILEINFO_T *pFileInfo)
{
    return ROFS_address_ext(pFileInfo, ROFS_partitionbase());
}

ROFS_FILEINFO_T* ROFS_fopen_ext(unsigned short usFileID, uint32_t ulFlashAddress)
{
    unsigned long i;
    struct ROFS_Header *pROFS_Header;
    ROFS_FILEINFO_T *ptr;
    uint32_t ROFS_FLASH_ADDRESS_BASE;

    ROFS_FLASH_ADDRESS_BASE=ulFlashAddress;
    if(ROFS_FLASH_ADDRESS_BASE==0)
        return NULL;

    pROFS_Header=(struct ROFS_Header *)ROFS_FLASH_ADDRESS_BASE;
    while(pROFS_Header && memcmp(pROFS_Header->cMarker, "ROFS", 4)==0){
        ptr=(ROFS_FILEINFO_T*)((uint32_t)pROFS_Header+sizeof(struct ROFS_Header));
        for(i=0; i<pROFS_Header->ulFileNumber; i++){
            if(ptr[i].FileID==usFileID){
                return &ptr[i];
            }
        }
        if(pROFS_Header->ulNextTableOffset!=ROFS_HEADER_ADDRESS_INVALID)
            pROFS_Header=(struct ROFS_Header *)(ROFS_FLASH_ADDRESS_BASE+pROFS_Header->ulNextTableOffset);
        else
            break;
    }

    return NULL;
}

uint32_t ROFS_address_ext(ROFS_FILEINFO_T *pFileInfo, uint32_t ulFlashAddress)
{
    uint32_t address;
    struct ROFS_Header *pROFS_Header;
    uint32_t ROFS_FLASH_ADDRESS_BASE;

    ROFS_FLASH_ADDRESS_BASE=ulFlashAddress;

    if(ROFS_FLASH_ADDRESS_BASE==0 || pFileInfo==NULL || pFileInfo->HasBasicInfo==0)
        return 0xFFFFFFFF;

    address=ROFS_FLASH_ADDRESS_BASE;
    for(int i=0; i<pFileInfo->ucPartition; i++){
        pROFS_Header=(struct ROFS_Header *)address;
        address+=pROFS_Header->ulPartitionSize;
    }

    return address+pFileInfo->BasicInfo.ulStartPosition;
}

