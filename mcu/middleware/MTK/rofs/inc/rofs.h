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


#ifndef _ROFS_H_
#define _ROFS_H_

 /**
 * @addtogroup ROFS Read-only File System (ROFS)
 * @{
 */


typedef struct ROFS_BasicInfo{
	unsigned long ulStartPosition; // Offset in this ROFS image.
	unsigned long ulFileSize; // In bytes.
}ROFS_BASICINFO_T;

// VP: Voice Prompt
typedef struct ROFS_VPInfo{
	unsigned short usLCID;
	unsigned char ucCodecSettings;
    unsigned char ucPadding;
}ROFS_VPINFO_T;

// VC: Voice Command
typedef struct ROFS_VCInfo{
	unsigned short usLCID;
    unsigned char ucPadding[2];
}ROFS_VCINFO_T;

// Database:
typedef struct ROFS_DBInfo{
	unsigned short usLCID;
    unsigned char ucPadding[2];
}ROFS_DBINFO_T;

typedef struct ROFS_FileInfo{
	unsigned short FileID;
    unsigned char ucPartition;
    unsigned char ucPadding;
    char szFileName[32];
	unsigned HasBasicInfo: 1; // 0: No information; 1: Has information
	unsigned HasVPInfo: 1; // 0: No information; 1: Has information
	unsigned HasVCInfo: 1; // 0: No information; 1: Has information
	unsigned HasDBInfo: 1; // 0: No information; 1: Has information
    unsigned char ucPadding2[3];
	ROFS_BASICINFO_T BasicInfo;
	ROFS_VPINFO_T VPInfo;
	ROFS_VCINFO_T VCInfo;
	ROFS_DBINFO_T DBInfo;
    unsigned char ucPadding3[12];
}ROFS_FILEINFO_T;
 
/*!
  @brief Get a specified media id's file related information.
  
  @return NULL: Not found. Others: Successful.
*/
ROFS_FILEINFO_T* ROFS_fopen(uint16_t usFileID);

/*!
  @brief Get the address of the specified file.
  
  @return 0xFFFFFFFF: Not found. Others: Successful.
*/
uint32_t ROFS_address(ROFS_FILEINFO_T *pFileInfo);

/*!
  @brief Get a specified media id's file related information. This is an extended version of ROFS_fopen().
  
  @return NULL: Not found. Others: Successful.
*/
ROFS_FILEINFO_T* ROFS_fopen_ext(uint16_t usFileID, uint32_t ulFlashAddress);

/*!
  @brief Get the address of the specified file. This is an extended version of ROFS_address().
  
  @return 0xFFFFFFFF: Not found. Others: Successful.
*/
uint32_t ROFS_address_ext(ROFS_FILEINFO_T *pFileInfo, uint32_t ulFlashAddress);

/**
* @}
*/
#endif
