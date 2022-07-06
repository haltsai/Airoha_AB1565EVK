/* Copyright Statement:
 *
 * (C) 2014  Airoha Technology Corp. All rights reserved.
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
#include "types.h"
#include "source_inter.h"
#include "dtm.h"
#include "sink_inter.h"

#include "transform_inter.h"
U8* tempaddr ;

#define UNUSED(p) ((void)(p))



BOOL SinkCompareMemory(SINK sink, U32 compare_addr);

BOOL SourceReadBuf_Memory(SOURCE source, U8 *dst_addr, U32 length)
{
    if ((length*source->param.memory.remain_len) != 0)
    {
        memcpy(dst_addr,source->streamBuffer.BufferInfo.startaddr[0] + source->streamBuffer.BufferInfo.length - source->param.memory.remain_len, length);
    }
    return TRUE;
}
U32 SourceSize_Memory(SOURCE source)
{
    return MIN(source->param.memory.max_output_length,source->param.memory.remain_len);
}

U8* SourceMap_Memory(SOURCE source)
{
    SourceReadBuf_Memory(source,MapAddr,SourceSize_Memory(source));
    return MapAddr;
}


VOID SourceDrop_Memory(SOURCE source, U32 amount)
{
    
    if (amount > source->param.memory.remain_len)
    {
        source->param.memory.remain_len = 0;
    }
    else
    {
        source->param.memory.remain_len -= amount;
    }
//    #if RF_TESTDSPOPEN
    #if 1
    if (source->param.memory.remain_len == 0)
    {
        source->param.memory.remain_len = source->streamBuffer.BufferInfo.length;
    }
    #else
    if ((source->param.memory.remain_len == 0)&&(source->param.memory.handler != NULL))
    {
        //MSG_MessageSend(source->param.memory.handler, MESSAGE_SOURCE_FINISHED_READ, 0);
    }
    return;
    #endif
}

BOOL SourceConfigure_Memory(SOURCE source, stream_config_type type, U32 value)
{
    switch (type)
    {
            case MEMORY_SOURCE_MAX_DATA_READ:
                source->param.memory.max_output_length = value;
                break;
            case MEMORY_SOURCE_SET_HANDLE:
                source->param.memory.handler = (VOID*)value;
                break;
            case MEMORY_SOURCE_UPDATE_MEM_ADDR:
                source->param.memory.remain_len = 0;
                source->streamBuffer.BufferInfo.startaddr[0]   = (U8*)value;
                break;
            case MEMORY_SOURCE_UPDATE_MEM_LEN:
                source->param.memory.remain_len = value;
                source->streamBuffer.BufferInfo.length = value;
                break;
            case MEMORY_SOURCE_FORCE_DATA_PUT:
                //MSG_MessageSend(&gMemoryHandle, MESSAGE_MORE_SPACE, 0);
                break;
            default :
                break;
    }
    return TRUE;    
}

BOOL SourceClose_Memory(SOURCE source)
{
    UNUSED(source);
    return TRUE;
}



VOID SourceInit_Memory(SOURCE source)
{
    /* buffer init */
    source->type = SOURCE_TYPE_MEMORY;
    source->buftype = BUFFER_TYPE_CIRCULAR_BUFFER;
    /* interface init */    
    source->sif.SourceSize        = SourceSize_Memory;
    source->sif.SourceMap         = SourceMap_Memory;
    source->sif.SourceConfigure   = SourceConfigure_Memory;
    source->sif.SourceDrop        = SourceDrop_Memory;
    source->sif.SourceClose       = SourceClose_Memory;
    source->sif.SourceReadBuf     = SourceReadBuf_Memory;
    source->param.memory.max_output_length = 512;
    source->param.memory.remain_len = source->streamBuffer.BufferInfo.length;
}
U32 SinkSizeMemory(SINK sink)
{
    return (sink->streamBuffer.BufferInfo.length - sink->streamBuffer.BufferInfo.WriteOffset);
}

BOOL SinkWriteBufMemory(SINK sink, U8 *src_addr, U32 length)
{
    BOOL report = FALSE;
    U8* dst_addr = sink->streamBuffer.BufferInfo.startaddr[0] + sink->streamBuffer.BufferInfo.WriteOffset;
    if (length <=  SinkSizeMemory(sink))
    {
        if (sink->param.memory.memory_type == Flash_addr)
        {
            //DRV_SFlash_ByteProgram(dst_addr, src_addr, length);
        }
        else
        {
            memcpy(dst_addr, src_addr, length);
        }
        report = TRUE;
    }

    return TRUE;
}


U8* SinkMapMemory(SINK sink)
{
    return sink->streamBuffer.BufferInfo.startaddr[0];
}


BOOL SinkDropMemory(SINK sink, U32 amount)
{
    BOOL report = FALSE; 
    if (amount <=  SinkSizeMemory(sink))
    {
        sink->streamBuffer.BufferInfo.WriteOffset += amount;
        report = TRUE;
    }
    return report;

}

BOOL SinkConfigureMemory(SINK sink, stream_config_type type, U32 value)
{
    BOOL report = TRUE; 
    switch (type)
    {
            case MEMORY_SINK_SET_WRITE_OFFSET:
                sink->streamBuffer.BufferInfo.WriteOffset = value;
                break;
            case MEMORY_SINK_FORCE_DATA_FILL:
                //MSG_MessageSend(&gMemoryHandle, MESSAGE_MORE_DATA, 0);
                break;
            case MEMORY_SINK_DATA_COMPARE:
                report = SinkCompareMemory(sink,value);
                break;
            case MEMORY_SINK_SET_COMPARE_OFFSET:
                sink->param.memory.remain_len = value;
                break;
            default :
                break;
    }
    return report;    
}

BOOL SinkCloseMemory(SINK sink)
{
    UNUSED(sink);
    return TRUE;
}



VOID SinkInit_Memory(SINK sink)
{
    /* buffer init */
    sink->buftype = BUFFER_TYPE_CIRCULAR_BUFFER;
    /* interface init */    
    sink->sif.SinkSlack        = SinkSizeMemory;
    sink->sif.SinkMap          = SinkMapMemory;
    sink->sif.SinkConfigure    = SinkConfigureMemory;
    sink->sif.SinkFlush        = SinkDropMemory;
    sink->sif.SinkClose        = SinkCloseMemory;
    sink->sif.SinkWriteBuf     = SinkWriteBufMemory;
}

BOOL SinkCompareMemory(SINK sink, U32 compare_addr)
{
    U32 i;
    BOOL report = TRUE;
    tempaddr = &(sink->param.memory.temp4copy);
    
    for (i = sink->param.memory.remain_len ; i < sink->streamBuffer.BufferInfo.WriteOffset; i++)
    {
        if (sink->param.memory.memory_type == Flash_addr) 
        {
            //DRV_SFlash_ByteRead((sink->streamBuffer.BufferInfo.startaddr[0] + i),tempaddr, 1);
            if (*tempaddr != *(U8*)(compare_addr + i))
            {
                report = FALSE;
            }
            else
            {
                *tempaddr = 0;
            }
        }
        else
        {
            if (*(U8*)(sink->streamBuffer.BufferInfo.startaddr[0] + i) != *(U8*)(compare_addr + i))
            {
                report = FALSE;
            }
            else
            {
                *(U8*)(sink->streamBuffer.BufferInfo.startaddr[0] + i) = 0;
            }
        }
    }
    return report;
}


