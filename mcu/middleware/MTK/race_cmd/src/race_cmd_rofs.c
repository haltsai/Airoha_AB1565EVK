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
#include "race_cmd.h"
#include "race_xport.h"
#include "rofs.h"

/***************************************************************/
/*                   Defines                                   */
/***************************************************************/
#define RACE_ID_ROFS_GET_FILE_INFO 0x1D00
#define RACE_ID_ROFS_GET_BASE_ADDR 0x1D01
#define RACE_ID_ROFS_READ_FILE     0x1D02

typedef struct stru_race_rofs_file_info_rsp
{
    unsigned char ucFound;
    unsigned long ulStartPosition;
    unsigned long ulFileSize;
}PACKED RACE_ROFS_FILE_INFO_RSP_STRU, *PTR_RACE_ROFS_FILE_INFO_RSP_STRU;

typedef struct stru_race_rofs_address_rsp
{
    unsigned long baseAddress;
}PACKED RACE_ROFS_ADDR_RSP_STRU, *PTR_RACE_ROFS_ADDR_RSP_STRU;

typedef struct stru_race_rofs_read_file
{
    RACE_COMMON_HDR_STRU cmdhdr;
    unsigned short ROFS_FILE_ID;
    unsigned long start_offset;
    unsigned long read_length;
}PACKED RACE_ROFS_READ_FILE_STRU, *PTR_RACE_ROFS_READ_FILE_STRU;

typedef struct stru_race_rofs_read_file_rsp
{
    unsigned char status;
    unsigned char data[0];
}PACKED RACE_ROFS_READ_FILE_RSP_STRU, *PTR_RACE_ROFS_READ_FILE_RSP_STRU;

void* RACE_CmdHandlerROFS(ptr_race_pkt_t pRaceHeaderCmd, uint16_t Length, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("RACE_CmdHandlerROFS, type[0x%X], race_id[0x%X], channel_id[%d]", 3,
        pRaceHeaderCmd->hdr.type, pRaceHeaderCmd->hdr.id, channel_id);

    //PTR_ROFS_FILE_INFO_STRU pRaceROFSFileInfo;
    PTR_RACE_ROFS_FILE_INFO_RSP_STRU pRaceROFSFileInfoRsp;
    PTR_RACE_ROFS_ADDR_RSP_STRU pRaceROFSaddrRsp;
    ROFS_FILEINFO_T *pROFSFile;
    unsigned short ROFS_FILE_ID = 0;
    uint32_t data_addr;
    PTR_RACE_ROFS_READ_FILE_STRU pRaceROFSReadFile;
    PTR_RACE_ROFS_READ_FILE_RSP_STRU pRaceROFSReadFileRsp;
    unsigned long read_length;

    switch (pRaceHeaderCmd->hdr.id)
    {
        case RACE_ID_ROFS_GET_FILE_INFO:
            if(Length == 4){ // cmd id(2 bytes) + rofs file id(2 bytes)
                ROFS_FILE_ID = ((pRaceHeaderCmd->payload[1]<<8) | pRaceHeaderCmd->payload[0]);
                RACE_LOG_MSGID_I("rofs get file info cmd, id: 0x%04x, 0x%04x",2, ROFS_FILE_ID, *((unsigned short*)pRaceHeaderCmd->payload));
            }else{
                RACE_LOG_MSGID_E("rofs id legth should be 2 bytes.", 0);
            }
            pRaceROFSFileInfoRsp = RACE_ClaimPacket(RACE_TYPE_RESPONSE, RACE_ID_ROFS_GET_FILE_INFO, sizeof(RACE_ROFS_FILE_INFO_RSP_STRU), channel_id);
            if(!pRaceROFSFileInfoRsp){
                return NULL;
            }

            pROFSFile=ROFS_fopen(ROFS_FILE_ID);
            if(pROFSFile!=NULL && pROFSFile->HasBasicInfo==1){
                pRaceROFSFileInfoRsp->ulStartPosition=pROFSFile->BasicInfo.ulStartPosition;
                pRaceROFSFileInfoRsp->ulFileSize=pROFSFile->BasicInfo.ulFileSize;
                pRaceROFSFileInfoRsp->ucFound=0; // File found.
            }
            else{
                pRaceROFSFileInfoRsp->ulStartPosition=0;
                pRaceROFSFileInfoRsp->ulFileSize=0;
                pRaceROFSFileInfoRsp->ucFound=1; // File not found.
            }
            return pRaceROFSFileInfoRsp;

        case RACE_ID_ROFS_GET_BASE_ADDR:
            pRaceROFSaddrRsp = RACE_ClaimPacket(RACE_TYPE_RESPONSE, RACE_ID_ROFS_GET_BASE_ADDR, sizeof(RACE_ROFS_ADDR_RSP_STRU), channel_id);
            if(!pRaceROFSaddrRsp){
                return NULL;
            }

            pRaceROFSaddrRsp->baseAddress = ROFS_BASE;
            return pRaceROFSaddrRsp;

        case RACE_ID_ROFS_READ_FILE:
            pRaceROFSReadFile = (PTR_RACE_ROFS_READ_FILE_STRU)pRaceHeaderCmd;

            read_length = pRaceROFSReadFile->read_length;

            pROFSFile=ROFS_fopen(pRaceROFSReadFile->ROFS_FILE_ID);

            RACE_LOG_MSGID_I("RACE_ID_ROFS_READ_FILE  id[0x%x]",1, pRaceROFSReadFile->ROFS_FILE_ID);

            if(pROFSFile==NULL)
            {
                pRaceROFSReadFileRsp = RACE_ClaimPacket(RACE_TYPE_RESPONSE, RACE_ID_ROFS_READ_FILE, sizeof(RACE_ROFS_READ_FILE_RSP_STRU), channel_id);
                if(!pRaceROFSReadFileRsp){
                    return NULL;
                }
                pRaceROFSReadFileRsp->status = RACE_ERRCODE_FAIL;
                return pRaceROFSReadFileRsp;
            }
            else
            {
                pRaceROFSReadFileRsp = RACE_ClaimPacket(RACE_TYPE_RESPONSE, RACE_ID_ROFS_READ_FILE, sizeof(RACE_ROFS_READ_FILE_RSP_STRU) + read_length, channel_id);
                if(!pRaceROFSReadFileRsp){
                    return NULL;
                }
                else{
                    data_addr = ROFS_address(pROFSFile);
                    memcpy(pRaceROFSReadFileRsp->data, (char*)data_addr+pRaceROFSReadFile->start_offset, read_length);
                    pRaceROFSReadFileRsp->status = RACE_ERRCODE_SUCCESS;
                    return pRaceROFSReadFileRsp;
                }
            }

        default:
            RACE_LOG_MSGID_E("unknown rofs race cmd, race id[%d]", 1, pRaceHeaderCmd->hdr.id);
            break;
    }
    return NULL;
}
