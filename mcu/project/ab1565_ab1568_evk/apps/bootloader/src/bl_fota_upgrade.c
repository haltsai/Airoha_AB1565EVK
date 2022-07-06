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


#include <stddef.h>
#include "LZMA_decoder.h"
#include "bl_fota_def.h"
#include "bl_fota_util.h"
#include "fota_util.h"
#include "fota.h"
#include "fota_flash.h"
#include "fota_multi_info.h"
#include "hal_wdt.h"
#include "bsp_flash.h"
#include "hal_aes.h"
#include "bl_fota_flash_ctrl.h"

////////////////////////////////////////////////////////////////////////////////
// EXTERN///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Definition///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


#define FOTA_WRITE_READ_LENGTH  (1024)

/* SzAlloc() was called twice. One is to alloc 15980 bytes and the other is to alloc dictory_size bytes.
  * level  dictory_size
  * 0       16K
  * 1       64K
  * 2       256K
  */
/* LZMA level=1, when AES is enabled, TCM rans out if level 1 is used. */
//#define LZMA_MALLOC_BUFFER_SIZE  (80 * 1024)
/* LZMA level=0 */
#define LZMA_MALLOC_BUFFER_SIZE  (32 * 1024)
U8 g_lzma_malloc_buffer[LZMA_MALLOC_BUFFER_SIZE];

static void *SzAlloc(ISzAllocPtr p, size_t size)
{
    UNUSED(p);
   static unsigned int nUsed=0;
   void *addr = NULL;

   nUsed += size;

   addr = (void*)(&(g_lzma_malloc_buffer[nUsed - size]));
   FOTA_LOG_D("SzAlloc nUsed:%d size:%d addr:%x", nUsed, size, addr);
   return addr;
}

static void SzFree(ISzAllocPtr p, void *address)
{
    UNUSED(p);
    UNUSED(address);

    FOTA_LOG_D("SzFree addr:%x", address);
 return;
}

static ISzAlloc g_Alloc = { SzAlloc, SzFree };

#define IN_BUF_SIZE 0x100
#define OUT_BUF_SIZE 0x1000
__attribute__ ((__aligned__(4)))U8 inBuf[IN_BUF_SIZE];
__attribute__ ((__aligned__(4)))U8 outBuf[OUT_BUF_SIZE];

#ifdef FOTA_UPGRADE_AES_SUPPORT
__attribute__ ((__aligned__(4)))U8 inAesBuf[IN_BUF_SIZE] = {0};
U8 g_aes_key[] = OTA_ENC_KEY;
U8 g_aes_iv[]  = OTA_ENC_IV;
#endif

U8 eraseFlag;

DFU_ERRCODE bl_fota_lzma_decompress(U32 src_addr, U32 srcLen)
{
    U32 unpackSize;  // current unpacked data size
    int i;
    SRes res = SZ_OK;
    int err = DFU_ERR_SUCCESS;
    U32 srcIdx = IN_BUF_SIZE;  // index of the un-read data in src_addr
    U32 data_4k_index = 0;

    CLzmaDec state;
    unsigned char header[LZMA_PROPS_SIZE + 8];

    FOTA_LOG_I("lzma start");

#ifdef FOTA_UPGRADE_AES_SUPPORT

    if (Lzma_Aes == g_compression_type)
    {
        FOTA_LOG_I("AES init");
    }
#endif

#ifdef FOTA_UPGRADE_AES_SUPPORT
    if (Lzma_Aes == g_compression_type)
    {
        err = bsp_flash_read(src_addr, inAesBuf, 256);
    }
    else
    {
        err = bsp_flash_read(src_addr, inBuf, 256);
    }
#else
    err = bsp_flash_read(src_addr, inBuf, 256);
#endif

    if (BSP_FLASH_STATUS_OK != err)
    {
        return DFU_ERR_FLASH_CTRL_NVRAM_PAGE_READ_FAIL;
    }
    FOTA_LOG_D("count:1 Read one page to inBuf (256 bytes)");

#ifdef FOTA_UPGRADE_AES_SUPPORT
    if (Lzma_Aes == g_compression_type)
    {
        hal_aes_buffer_t aes_input;
        hal_aes_buffer_t aes_output;
        hal_aes_buffer_t aes_key;
        aes_input.length = sizeof(inAesBuf);
        aes_input.buffer = inAesBuf;
        aes_output.length = sizeof(inBuf);
        aes_output.buffer = inBuf;
        aes_key.length = sizeof(g_aes_key);
        aes_key.buffer = g_aes_key;
        err = hal_aes_cbc_decrypt(&aes_output, &aes_input, &aes_key, g_aes_iv);
        if (HAL_AES_STATUS_OK != err)
        {
            FOTA_LOG_E("AES decrypt failed. err:%d", err);
            return DFU_ERR_AES_DECRYPT_FAIL;
        }
    }
#endif

    for (unsigned int g = 0; g < sizeof(header); ++g)
    {
        header[g] = inBuf[g];
        FOTA_LOG_D("header[%d]:%x", g, header[g]);
    }

    unpackSize = 0;

    /* size in header is 8 bytes, but we use 4 bytes only */
    for (i = 0; i < 4; ++i)
    {
        unsigned char b = header[LZMA_PROPS_SIZE + i];
        unpackSize += (U32)b << (i * 8);
    }

    FOTA_LOG_I("fota_package unpackSize:%d", unpackSize);

    LzmaDec_Construct(&state);
    res = LzmaDec_Allocate(&state, header, LZMA_PROPS_SIZE, &g_Alloc);
    if (res != SZ_OK)
    {
        FOTA_LOG_E("LzmaDec_Allocate() failed res:%d", res);
        return DFU_ERR_AES_DECRYPT_FAIL;
    }

    {
        // inSize:
        // inPos:         // inPos: used size in inBuffer.
        // outPos: used size in outBuffer.
        // inSize: size of each segment of source data (0 bytes < 1 segment <= IN_BUF_SIZE bytes).
        size_t inPos = 13, inSize = IN_BUF_SIZE, outPos = 0;
        size_t count = 1;
        LzmaDec_Init(&state);

        for (;;)
        {
            /* reset wdt dog timer every loop*/
            hal_wdt_feed (HAL_WDT_FEED_MAGIC);

            /* 1 segment of source data has been processed completely. */
            if (inPos == inSize)
            {
                // read one page to inBuf (256 bytes)
#ifdef FOTA_UPGRADE_AES_SUPPORT
                if (Lzma_Aes == g_compression_type)
                {
                    err = bsp_flash_read(src_addr + srcIdx, inAesBuf, 256);
                    //err = bsp_flash_read(src_addr + srcIdx, inBuf, 256);
                }
                else
                {
                    err = bsp_flash_read(src_addr + srcIdx, inBuf, 256);
                }
#else
                err = bsp_flash_read(src_addr + srcIdx, inBuf, 256);
#endif
                if (FOTA_ERRCODE_SUCCESS != err)
                {
                    FOTA_LOG_E("fota_flash_read() failed err:%d", err);
                    err = DFU_ERR_FLASH_CTRL_NVRAM_PAGE_READ_FAIL;
                    break;
                }

#ifdef FOTA_UPGRADE_AES_SUPPORT
                if (Lzma_Aes == g_compression_type)
                {
                    hal_aes_buffer_t aes_input;
                    hal_aes_buffer_t aes_output;
                    hal_aes_buffer_t aes_key;
                    aes_input.length  = sizeof(inAesBuf);
                    aes_input.buffer  = inAesBuf;
                    aes_output.length = sizeof(inBuf);
                    aes_output.buffer = inBuf;
                    aes_key.length    = sizeof(g_aes_key);
                    aes_key.buffer    = g_aes_key;
                    err = hal_aes_cbc_decrypt(&aes_output, &aes_input, &aes_key, g_aes_iv);
                    /* Loop time: 256 / 16 = 16 */
                    if (HAL_AES_STATUS_OK != err)
                    {
                        FOTA_LOG_E("AES decrypt failed. err:%d", err);
                        err = DFU_ERR_AES_DECRYPT_FAIL;
                        break;
                    }
                }
#endif
                inSize = IN_BUF_SIZE;
                if (srcIdx + IN_BUF_SIZE > srcLen)
                {
                    inSize = srcLen - srcIdx;
                }

                inPos = 0;
                srcIdx += inSize;
            }

            // inProcessed: un-decompressed data size in each segment of source data.
            SizeT inProcessed = inSize - inPos;

            // outProcessed: unused space size in outBuf.
            SizeT outProcessed = OUT_BUF_SIZE - outPos;
            ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
            ELzmaStatus status;

            if (outProcessed > unpackSize)
            {
                outProcessed = (SizeT)unpackSize;
                finishMode = LZMA_FINISH_END;
            }

            res = LzmaDec_DecodeToBuf(&state, outBuf + outPos, &outProcessed,
                                      inBuf + inPos, &inProcessed, finishMode, &status);
            if (res != SZ_OK)
            {
                FOTA_LOG_E("LzmaDec_DecodeToBuf failed. res:%d", res);
                return DFU_ERR_AES_IV_NOT_MATCH;
            }

            if (inProcessed == 0 && outProcessed == 0)
            {
                if (status != LZMA_STATUS_FINISHED_WITH_MARK)
                {
                    res = SZ_ERROR_DATA;
                    FOTA_LOG_E("LzmaDec status error. status:%d res:%d", status, res);
                }
                break;
            }

            inPos += (UInt32)inProcessed;
            outPos += outProcessed;
            unpackSize -= outProcessed;


            // Write the decompressed data into the Flash when the size of the decompressed data reaches 4K.
            if (outPos == OUT_BUF_SIZE)
            {
                U32 target_address = g_fota_data_4k_target_table[data_4k_index];

#ifdef  FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION
                /* erase temp area in last 4K in bootloader */
                if(target_address == 0)
                {
                    FOTA_LOG_I("erase temp partition table address");
                    DFU_ERRCODE dfu_err_code = bl_fota_flash_erase(FOTA_BL_FLASH_ADDRESS, 0x1000);
                    if (dfu_err_code != DFU_ERR_SUCCESS)
                    {
                        FOTA_LOG_E("Erase flash failed. data_4k_index:%d, addr:%x, err:%d", data_4k_index, target_address, dfu_err_code);
                        return DFU_ERR_MOVE_DATA_ERASE_FAIL;
                    }
                }
                else
#endif
                {
                    if(eraseFlag == FALSE)
                    {
                        for (U32 i = 0; i < g_number_of_movers; ++i)
                        {
                            U32 targetAddr  = g_movers[i].destination_address;
                            U32 totalLength = g_movers[i].length;

                            while(totalLength)
                            {
                                U32 eraseLength = totalLength;

                                /* erase 256K once*/
                                if(totalLength > 0x40000)
                                {
                                    eraseLength = 0x40000;
                                }

                                DFU_ERRCODE dfu_err_code = bl_fota_flash_erase(targetAddr, eraseLength);

                                targetAddr  += eraseLength;
                                totalLength -= eraseLength;

                                if (dfu_err_code != DFU_ERR_SUCCESS)
                                {
                                    FOTA_LOG_E("Erase flash failed. data_4k_index:%d, addr:%x, err:%d", data_4k_index, target_address, dfu_err_code);
                                    return DFU_ERR_MOVE_DATA_ERASE_FAIL;
                                }
                            }
                        }
                        eraseFlag = TRUE;
                    }
                }

                U32 page_count = 0x1000 / 0x100;
                U32 offset = 0;

                FOTA_LOG_D("Count = %d , Write %d bytes decompressed data into Flash address %x ",count, OUT_BUF_SIZE, target_address);
                count++;

#ifdef  FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION
                /* erase temp area in last 4K in bootloader */
                if(target_address == 0)
                {
                    FOTA_LOG_I("write temp partition table address");
                    for (U32 z = 0; z < page_count; ++z)
                    {
                        err = bsp_flash_write(FOTA_BL_FLASH_ADDRESS + offset, outBuf + offset, 256);

                        if(err != BSP_FLASH_STATUS_OK)
                        {
                            FOTA_LOG_E("err=%d count=%d offset=%d Address=%x",err,count,offset,target_address + offset);
                        }

                        offset += 0x100;
                    }
                }
                else
#endif
                {
                    for (U32 z = 0; z < page_count; ++z)
                    {
                        err = bsp_flash_write(target_address + offset, outBuf + offset, 256);

                        if(err != BSP_FLASH_STATUS_OK)
                        {
                            FOTA_LOG_E("err=%d count=%d offset=%d Address=%x",err,count,offset,target_address + offset);
                        }

                        offset += 0x100;
                    }
                }
                outPos = 0;
                ++data_4k_index;
            }

            if (unpackSize == 0)
            {
                FOTA_LOG_I("unpackSize is 0 now. All compressed data has been processed");
                break;
            }
        }
    }

    /* fota integrity check */
    if(g_is_sha_info_found == TRUE)
    {
        for (U32 i = 0; i < g_number_of_movers; ++i)
        {
            U32 oriSHA[8];
            U32 sha256[8];
            U32 is_int;

            memset(oriSHA,0,sizeof(sha256));
            memset(sha256,0,sizeof(sha256));

#ifdef BSP_EXTERNAL_SERIAL_FLASH_ENABLED
            /* decide internel or external flash */
            if(!(g_movers[i].destination_address & SPI_SERIAL_FLASH_ADDRESS))
#endif
            {
                is_int = 1;
            }
#ifdef BSP_EXTERNAL_SERIAL_FLASH_ENABLED
            else
            {
                is_int = 0;
            }
#endif

#ifdef  FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION
            /* check temp area in last 4K in bootloader */
            if(g_movers[i].destination_address == 0)
            {
                err = fota_sha256_generate((unsigned char*)sha256 ,FOTA_BL_FLASH_ADDRESS ,g_movers[i].length , is_int);
            }
            else
#endif
            {
                err = fota_sha256_generate((unsigned char*)sha256 ,g_movers[i].destination_address ,g_movers[i].length , is_int);
            }

            if(err != DFU_ERR_SUCCESS)
            {
                FOTA_LOG_E("sha 256 generate fail :%d",err);
                break;
            }

            bsp_flash_read(g_sha_info_start_address + 32*i, (uint8_t *)oriSHA, 32);

            if (memcmp(oriSHA ,sha256 ,sizeof(sha256)))
            {
                FOTA_LOG_E("sha number not match, mover:%d",i);

                for(U32 j = 0; j < 8; j++)
                {
                    FOTA_LOG_E("gen_sha[%d]=%u",j,sha256[j]);
                }

                for(U32 j = 0; j < 8; j++)
                {
                    FOTA_LOG_E("ori_sha[%d]=%u",j,oriSHA[j]);
                }

                err = DFU_ERR_SHA_MATCH_FAIL;
                break;
            }
        }
    }

    LzmaDec_Free(&state, &g_Alloc);

    FOTA_LOG_I("err:%d res:%d", err, res);

    if (DFU_ERR_SUCCESS == err && SZ_OK != res)
    {
        err = DFU_ERR_FAIL;
    }

    return err;
}

DFU_ERRCODE bl_fota_move_data(VOID)
{
    U32 i;
    DFU_ERRCODE err_code;
    uint32_t src_addr = 0, dst_addr = 0, total_written_len = 0, written_len = FOTA_WRITE_READ_LENGTH;

    FOTA_LOG_I("bl_fota_move_data");
    if (g_fota_partition_start_address == 0) // impossible address
    {
        FOTA_LOG_E("OTA is 0");
        return DFU_ERR_INVALID_FOTA_ADDR;
    }

    if (!bl_fota_is_mover_info_valid())
    {
        FOTA_LOG_E("mover is not valid");
        return DFU_ERR_FAIL;
    }

    FOTA_LOG_I("g_compression_type = %d",g_compression_type);

    if (Lzma == g_compression_type || Lzma_Aes == g_compression_type)
    {
        bl_gen_each_4k_target_addr_table();

        return bl_fota_lzma_decompress(g_fota_data_start_address, g_fota_data_length);
    }

    for (i = 0; i < g_number_of_movers; ++i)
    {
        dst_addr = g_movers[i].destination_address;
        
#ifdef  FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION
        if (dst_addr == 0) {
            if (0x1000 < g_movers[i].length) {
                FOTA_LOG_E("Invalid destination_address:%x length:%x in the mover info.", dst_addr, g_movers[i].length);
                return DFU_ERR_FAIL;
            }
            dst_addr = FOTA_BL_PHYSICAL_ADDRESS;
            is_fota_partition_upgrade = TRUE;
        }
#endif

        err_code = bl_fota_flash_erase(dst_addr, g_movers[i].length);
        if (err_code != DFU_ERR_SUCCESS)
        {
            return err_code;
        }

        err_code = bl_fota_flash_verify_erase(dst_addr, g_movers[i].length);
        if (err_code != DFU_ERR_SUCCESS)
        {
            return err_code;
        }

        src_addr = g_fota_partition_start_address + g_movers[i].source_address;
        
        total_written_len = 0;
        written_len = FOTA_WRITE_READ_LENGTH;

        while (g_movers[i].length && total_written_len < g_movers[i].length)
        {
            if (FOTA_WRITE_READ_LENGTH > (g_movers[i].length - total_written_len))
            {
                written_len = g_movers[i].length - total_written_len;
            }

            if (BSP_FLASH_STATUS_OK != bsp_flash_read(src_addr, outBuf, written_len))
            {
                return DFU_ERR_FLASH_CTRL_NVRAM_PAGE_READ_FAIL;
            }

            if (BSP_FLASH_STATUS_OK != bsp_flash_write(dst_addr, outBuf, written_len))
            {
                return DFU_ERR_FLASH_CTRL_NVRAM_PAGE_WRITE_FAIL;
            }

            if (DFU_ERR_SUCCESS != bl_fota_flash_verify_data(dst_addr, outBuf, written_len))
            {
                return err_code;
            }

            total_written_len += written_len;
            FOTA_LOG_D("src_addr:%x, dst_addr:%x, total_written_len:%d, written_len:%d", src_addr, dst_addr, total_written_len, written_len);
            src_addr += written_len;
            dst_addr += written_len;

        }
    }

    return DFU_ERR_SUCCESS;
}

void bl_fota_process (void)
{
    DFU_ERRCODE err_code = DFU_ERR_FAIL;
    uint32_t state;
    uint32_t eraseLength;
    uint32_t result;

    FOTA_LOG_I("%s", __FUNCTION__);
    fota_init_flash();
#if FOTA_STORE_IN_EXTERNAL_FLASH
    fota_flash_config_fota_partition_in_external_flash(FOTA_EXT_RESERVED_BASE_DEFAULT,
                                                       FOTA_EXT_RESERVED_LENGTH_DEFAULT);
#endif

#ifdef  FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION
    U32 page_count = 0xC00 / 0x100;
    int partition_valid = FALSE;


    if(*(int*)FOTA_BL_SYNCWORD_FLASH_ADDRESS != 0x65261688)
    {
        partition_valid = TRUE;
    }

    FOTA_LOG_I("check partition table is valid or not partition_valid[%d] syncword[%x]",partition_valid,*(int*)FOTA_BL_SYNCWORD_FLASH_ADDRESS);

    /* partition erase last time , copy from temp buffer */
    if(partition_valid == FALSE)
    {
        U32 offset = 0;
        int err = DFU_ERR_SUCCESS;

        bl_fota_flash_erase(0, 0x1000);
        for (U32 i = 0; i < page_count; i++)
        {
            err = bsp_flash_write(offset, (U8*)FOTA_BL_FLASH_ADDRESS + offset, 256);

            if(err != BSP_FLASH_STATUS_OK)
            {
                FOTA_LOG_E("err=%d offset=%d Address=%x",err,offset,offset);
            }

            offset += 0x100;
        }

        /* erase temp buffer */
        bl_fota_flash_erase(FOTA_BL_FLASH_ADDRESS, 0x1000);

        /* just reset */
        hal_wdt_software_reset();
    }
#endif

    err_code = fota_state_read((uint16_t*)&state);
    if(err_code != DFU_ERR_SUCCESS)
    {
        FOTA_LOG_I("FOTA state read error");
    }
    else
    {

    }

    if (state == FOTA_STATE_LOADER_START_DATA_MOVING)
    {
    }
    else if (state == FOTA_STATE_LOADER_ERASE_PACKAGE)
    {
        FOTA_LOG_I("FOTA erase package not finish");

        FotaStorageType storage_type;
        U32 length;

        if (fota_flash_get_fota_partition_info(&storage_type, (uint32_t *)&g_fota_partition_start_address, (uint32_t *)&length) != FOTA_ERRCODE_SUCCESS)
        {
            return;
        }

        if( storage_type == ExternalFlash)
        {
#ifdef BSP_EXTERNAL_SERIAL_FLASH_ENABLED
            g_fota_partition_start_address |= SPI_SERIAL_FLASH_ADDRESS;
#else
            FOTA_LOG_E("External flash is not supported!");
            return;
#endif
        }

        /* erase all partition */
        bl_fota_flash_erase((U32)g_fota_partition_start_address, length - 0x1000);

        goto fota_OK;
    }
    else
    {
        int flag = fota_upgrade_flag_is_set();

        if (flag == 0)
        {
            FOTA_LOG_E("upgrade_flag is not set.");
            return;
        }
    }

    FOTA_LOG_I("Begin FOTA upgrade");

    if (fota_check_fota_package_integrity(!FOTA_FLASH_TYPE) != FOTA_ERRCODE_SUCCESS)
    {
        FOTA_LOG_E("Integrity Check fail");

        result = FOTA_STATE_LOADER_PACKAGE_NG;

        goto fota_fail;
    }

    if (!bl_fota_init())
    {
        FOTA_LOG_E("bl_fota_init fail");

        result = FOTA_STATE_LOADER_PACKAGE_NG;

        goto fota_fail;
    }

    fota_state_write(FOTA_STATE_LOADER_START_DATA_MOVING);

    err_code = bl_fota_move_data();

    if (err_code != DFU_ERR_SUCCESS)
    {
        FOTA_LOG_E("bl_fota_move_data() failed. err:%x", err_code);

        if(err_code == DFU_ERR_AES_IV_NOT_MATCH || err_code == DFU_ERR_AES_DECRYPT_FAIL)
        {
            result = FOTA_STATE_LOADER_PACKAGE_NG;
        }
        else
        {
            result = FOTA_STATE_LOADER_DATA_MOVING_FAIL;
        }

        goto fota_fail;
    }

#ifdef  FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION
        /* update partition here */
        if (is_fota_partition_upgrade == TRUE)
        {
            U32 page_count = 0xC00 / 0x100;
            U32 offset = 0;
            int err = DFU_ERR_SUCCESS;

            /* write state in last 1K in bootloader */
            U32 syncWord = 0x65261688;

            err = bsp_flash_write(FOTA_BL_SYNCWORD_PHYSICAL_ADDRESS, (U8*)&syncWord, 4);

            if(err != BSP_FLASH_STATUS_OK)
            {
                FOTA_LOG_E("err=%d offset=%d Address=%x",err,offset,FOTA_BL_SYNCWORD_PHYSICAL_ADDRESS+offset);
            }

            FOTA_LOG_I("update partition table");

            bl_fota_flash_erase(0, 0x1000);

            for (U32 z = 0; z < page_count; ++z)
            {
                err = bsp_flash_write(offset, (U8*)FOTA_BL_FLASH_ADDRESS + offset, 256);

                if(err != BSP_FLASH_STATUS_OK)
                {
                    FOTA_LOG_E("err=%d offset=%d Address=%x",err,offset,offset);
                }

                offset += 0x100;
            }

            /* erase temp buffer */
            bl_fota_flash_erase(FOTA_BL_FLASH_ADDRESS, 0x1000);

        }
#endif

    hal_wdt_feed (HAL_WDT_FEED_MAGIC);

    /* set fota version*/
    fota_version_set(version, versionLength, FOTA_VERSION_TYPE_STORED);

    /* go to erase package state */
    fota_state_write(FOTA_STATE_LOADER_ERASE_PACKAGE);

    eraseLength = g_fota_data_start_address - g_fota_partition_start_address + g_fota_data_length;

    /* erase fota package */
    bl_fota_flash_erase((U32)g_fota_partition_start_address, eraseLength);

fota_OK:

    FOTA_LOG_I("FOTA upgrade complete");

    result = FOTA_STATE_LOADER_COMPLETE_DATA_MOVING;

fota_fail:

    if(result == FOTA_STATE_LOADER_PACKAGE_NG || result == FOTA_STATE_LOADER_COMPLETE_DATA_MOVING)
    {
        fota_upgrade_flag_clear();
    }

    fota_state_write(result);

    fota_device_reboot();
}
