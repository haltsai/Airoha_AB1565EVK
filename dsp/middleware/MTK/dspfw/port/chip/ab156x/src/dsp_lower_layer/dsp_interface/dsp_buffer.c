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
 
#include "config.h"
////#include "os.h"
////#include "os_memory.h"
#include "dsp_task.h"
////#include "rc.h"
#include "dsp_buffer.h"
#include "dsp_memory.h"
#include "dsp_utilities.h"
#include <string.h>


/******************************************************************************
 * Function Declaration
 ******************************************************************************/
VOID DSP_C2C_BufferCopy(VOID* DestBuf,
                        VOID* SrcBuf,
                        U16 CopySize,
                        VOID* DestCBufStart,
                        U16 DestCBufSize,
                        VOID* SrcCBufStart,
                        U16 SrcCBufSize);

VOID DSP_D2C_BufferCopy(VOID* DestBuf,
                        VOID* SrcBuf,
                        U16 CopySize,
                        VOID* CBufStart,
                        U16 DestCBufSize);

VOID DSP_C2D_BufferCopy(VOID* DestBuf,
                        VOID* SrcBuf,
                        U32 CopySize,
                        VOID* CBufStart,
                        U32 SrcCBufSize);
#if 0
VOID DSP_AudioBufferInit(U8 TaskID, DSP_BUFFER_ID_t DfeID, U16 BufSize);
VOID DSP_AudioBufferDeInit(DSP_BUFFER_ID_t DfeID);
VOID DSP_IdfePushSamplesToAuInBuf(DSP_BUFFER_ID_t DfeID);
BOOL DSP_GetSampleFromAuInBuf(DSP_BUFFER_ID_t DfeID, U16 Samples, VOID* DestBuf);
VOID DSP_OdfeGetSamplesFromAuOutBuf(DSP_BUFFER_ID_t DfeID);
BOOL DSP_PushSampleToAuOutBuf(DSP_BUFFER_ID_t DfeID, U16 Samples, VOID* SrcBuf);
#endif


/******************************************************************************
 * Variables
 ******************************************************************************/
#if 0
STATIC DSP_BUFFER_PTR_t DSP_BufferAddrManager[DFE_ID_TOTAL_NO];
STATIC VOID* DSP_AdmaAddrManager[DFE_ID_TOTAL_NO];
#endif

#if 0
/**
 * DSP_BUF_CheckAudioSamples
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
STATIC INLINE U16 DSP_BUF_CheckAudioSamples(DSP_BUFFER_ID_t DfeID)
{
    return (DSP_BufferAddrManager[DfeID]) ? DSP_BufferAddrManager[DfeID]->Header.BufferCount : 0;
}


/**
 * DSP_BUF_GetBufSize
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
STATIC INLINE U16 DSP_BUF_GetBufSize(DSP_BUFFER_ID_t DfeID)
{
    return (DSP_BufferAddrManager[DfeID]) ? DSP_BufferAddrManager[DfeID]->Header.BufferSize : 0;
}

/**
 * DSP_BUF_GetAudioDmaOffset
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
STATIC INLINE U16 DSP_BUF_GetAudioDmaOffset(DSP_BUFFER_ID_t DfeID)
{
    return (DSP_BufferAddrManager[DfeID]) ? DSP_BufferAddrManager[DfeID]->Header.DmaOffset : 0;
}


/**
 * DSP_BUF_GetHeader
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
STATIC INLINE DSP_BUFFER_HEADER_PTR_t DSP_BUF_GetHeader(DSP_BUFFER_ID_t DfeID)
{
    return (DSP_BufferAddrManager[DfeID]) ? &DSP_BufferAddrManager[DfeID]->Header : NULL;
}


/**
 * DSP_BUF_SetOutputThreshold
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_BUF_SetOutputThreshold(DSP_BUFFER_ID_t DfeID, U16 Threshold)
{
    DSP_BufferAddrManager[DfeID]->Header.OutputThreshold = Threshold;
}


/**
 * DSP_BUF_GetBufAddress
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
STATIC INLINE U8* DSP_BUF_GetBufAddress(DSP_BUFFER_ID_t DfeID)
{
    return (DSP_BufferAddrManager[DfeID]) ? DSP_BufferAddrManager[DfeID]->Buffer : NULL;
}


/**
 * DSP_DfeIDMappingToAdma
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
STATIC INLINE AU_DFE_ADMA_PTR_t DSP_DfeIDMappingToAdma(DSP_BUFFER_ID_t DfeID)
{
    AU_DFE_ADMA_PTR_t MappingAdma = 0;
    switch (DfeID)
    {

        case DFE_ID_ODFE_AU_L:
            MappingAdma = (AU_DFE_ADMA_PTR_t)&AU_ODFE_CH0_RADMA;
            break;

        case DFE_ID_ODFE_AU_R:
            MappingAdma = (AU_DFE_ADMA_PTR_t)&AU_ODFE_CH1_RADMA;
            break ;

        case DFE_ID_ODFE_VP:
            MappingAdma = (AU_DFE_ADMA_PTR_t)&AU_ODFE_CH2_RADMA;
            break ;

        case DFE_ID_ODFE_DBG_L:
            MappingAdma = (AU_DFE_ADMA_PTR_t)&AU_ODFE_CH3_RADMA;
            break;

        case DFE_ID_ODFE_DBG_R:
            MappingAdma = (AU_DFE_ADMA_PTR_t)&AU_ODFE_CH4_RADMA;
            break;

        case DFE_ID_IDFE_AU_0:
            MappingAdma = (AU_DFE_ADMA_PTR_t)&AU_IDFE_CH0_WADMA;
            break;

        case DFE_ID_IDFE_AU_1:
            MappingAdma = (AU_DFE_ADMA_PTR_t)&AU_IDFE_CH1_WADMA;
            break;

        case DFE_ID_IDFE_AU_2:
            MappingAdma = (AU_DFE_ADMA_PTR_t)&AU_IDFE_CH2_WADMA;
            break;

        case DFE_ID_IDFE_AU_3:
            MappingAdma = (AU_DFE_ADMA_PTR_t)&AU_IDFE_CH3_WADMA;
            break;

        case DFE_ID_IDFE_EC:
            MappingAdma = (AU_DFE_ADMA_PTR_t)&AU_IDFE_CH4_WADMA;
            break;

        default:
            break;
    }

    return MappingAdma;
}


/**
 * DSP_GetAdmaPara
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
STATIC INLINE VOID DSP_GetAdmaPara(DSP_BUFFER_ID_t DfeID, U16* FrameSize, U16* BufSize, U32* Addr, U16* NextHwOffset)
{
    AU_DFE_ADMA_PTR_t VOLATILE DfeAdma;

    DfeAdma          = DSP_DfeIDMappingToAdma(DfeID);
    *FrameSize       = (DfeAdma->SET.field.THD_SIZE)*4;
    *BufSize         = (DfeAdma->SET.field.BUF_SIZE)*4;
    *Addr            = (DfeAdma->INIT.field.ADDR);
    *NextHwOffset    = (DfeAdma->NEXT.field.ADDR - *Addr);
}
#endif


/**
 * DSP_C2D_BufferCopy
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM VOID DSP_C2D_BufferCopy(VOID* DestBuf,
                        VOID* SrcBuf,
                        U32 CopySize,
                        VOID* SrcCBufStart,
                        U32 SrcCBufSize)
{
    U8* SrcCBufEnd     	=  (U8*)((U8*)SrcCBufStart +  SrcCBufSize);
    U32 UnwrapSize      = (U8*)SrcCBufEnd - (U8*)SrcBuf; /* Remove + 1 to sync more common usage */
    S32 WrapSize        = CopySize - UnwrapSize;

    configASSERT((SrcCBufEnd >= (U8*)SrcBuf) && (SrcBuf >= SrcCBufStart));

    if (WrapSize > 0)
    {
        memcpy(DestBuf, SrcBuf, UnwrapSize);

        while ((U32)WrapSize > SrcCBufSize)
        {
            memcpy((U8*)DestBuf + UnwrapSize, SrcCBufStart, SrcCBufSize);
            WrapSize -= SrcCBufSize;
        }

        memcpy((U8*)DestBuf + UnwrapSize, SrcCBufStart, WrapSize);
    }
    else
    {
        memcpy(DestBuf, SrcBuf, CopySize);
    }
}


/**
 * DSP_D2C_BufferCopy
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM VOID DSP_D2C_BufferCopy(VOID* DestBuf,
                        VOID* SrcBuf,
                        U16 CopySize,
                        VOID* DestCBufStart,
                        U16 DestCBufSize)
{
	U8* DestCBufEnd		= (U8*)((U8*)DestCBufStart + DestCBufSize);
    U16 UnwrapSize      = (U8*)DestCBufEnd - (U8*)DestBuf; /* Remove + 1 to sync more common usage */
    S16 WrapSize        = CopySize - UnwrapSize;

    configASSERT((DestCBufEnd >= (U8*)DestBuf) && (DestBuf >= DestCBufStart));

    if (WrapSize > 0)
    {
        memcpy(DestBuf, SrcBuf, UnwrapSize);

        while (WrapSize > DestCBufSize)
        {
            memcpy(DestCBufStart, (U8*)SrcBuf + UnwrapSize, DestCBufSize);
            WrapSize -= DestCBufSize;
        }

        memcpy(DestCBufStart, (U8*)SrcBuf + UnwrapSize, WrapSize);
    }
    else
    {
        memcpy(DestBuf, SrcBuf, CopySize);
    }
}




/**
 * DSP_C2C_BufferCopy
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_C2C_BufferCopy(VOID* DestBuf,
                        VOID* SrcBuf,
                        U16 CopySize,
                        VOID* DestCBufStart,
                        U16 DestCBufSize,
                        VOID* SrcCBufStart,
                        U16 SrcCBufSize)
{
	U8* DestCBufEnd     = (U8*)((U8*)DestCBufStart + DestCBufSize);
    U16 DestUnwrapSize  = (U8*)DestCBufEnd - (U8*)DestBuf; /* Remove + 1 to sync more common usage */
    S16 DestWrapSize    = CopySize - DestUnwrapSize;

	U8* SrcCBufEnd		= (U8*)((U8*)SrcCBufStart + SrcCBufSize);
    U16 SrcUnwrapSize   = (U8*)SrcCBufEnd - (U8*)SrcBuf; /* Remove + 1 to sync more common usage */
    S16 SrcWrapSize     = CopySize - SrcUnwrapSize;

    configASSERT((DestCBufEnd >= (U8*)DestBuf) && (DestBuf >= DestCBufStart));
    configASSERT((SrcCBufEnd >= (U8*)SrcBuf) && (SrcBuf >= SrcCBufStart));

    if ((DestWrapSize > 0) && (SrcWrapSize > 0))
    {
        if (DestUnwrapSize > SrcUnwrapSize)
        {
            /* Src Buf wrap first */
            memcpy(DestBuf, SrcBuf, SrcUnwrapSize);

            while (SrcWrapSize > SrcCBufSize)
            {
                DSP_D2C_BufferCopy((U8*)DestBuf + SrcUnwrapSize, SrcCBufStart, SrcCBufSize, DestCBufStart, DestCBufSize);
                SrcWrapSize -= SrcCBufSize;
            }

            DSP_D2C_BufferCopy((U8*)DestBuf + SrcUnwrapSize, SrcCBufStart, CopySize - SrcUnwrapSize, DestCBufStart, DestCBufSize);
        }
        else
        {
            /* Dest Buf wrap first */
            memcpy(DestBuf, SrcBuf, DestUnwrapSize);

            while (DestWrapSize > DestCBufSize)
            {
                DSP_C2D_BufferCopy(DestCBufStart, (U8*)SrcBuf + DestUnwrapSize, DestCBufSize, SrcCBufStart, SrcCBufSize);
                DestWrapSize -= DestCBufSize;
            }

            DSP_C2D_BufferCopy(DestCBufStart, (U8*)SrcBuf + DestUnwrapSize, CopySize - DestUnwrapSize, SrcCBufStart, SrcCBufSize);
        }
    }
    else if (DestWrapSize > 0) /* Actual D2C */
    {
        DSP_D2C_BufferCopy(DestBuf, SrcBuf, CopySize, DestCBufStart, DestCBufSize);
    }
    else if (SrcWrapSize > 0) /* Actual C2D */
    {
        DSP_C2D_BufferCopy(DestBuf, SrcBuf, CopySize, SrcCBufStart, SrcCBufSize);
    }
    else
    {
        memcpy(DestBuf, SrcBuf, CopySize);
    }
}

/**
 * DSP_D2I_BufferCopy
 *
 * Direct buffer copy to interleaved buffer
 */
ATTR_TEXT_IN_IRAM VOID DSP_D2I_BufferCopy(U8* DestBuf,
                        U8* SrcBuf1,
                        U8* SrcBuf2,
                        U8* DestIBufStart,
                        U16 DestIBufSize,
                        U16 CopySize,
                        U16 FormatBytes)
{
	U8* DestIBufEnd		= (U8*)((U8*)DestIBufStart + DestIBufSize);
    uint32_t i, j, sample;
    uint32_t wpt, rpt, src_offset, dst_offset, toggle = 0;
    src_offset = 0;
    dst_offset = DestBuf - DestIBufStart;
    toggle = 0;
    configASSERT((DestIBufEnd >= (U8*)DestBuf) && (DestBuf >= DestIBufStart));

    //format_bytes
    sample = (CopySize<<1)/FormatBytes;

    for (i=0 ; i<sample ; i++) {
        for (j=0 ; j<FormatBytes ; j++ ) {
            wpt = (dst_offset + j) % DestIBufSize;
            rpt = src_offset + j;
            if ((toggle == 1) && (SrcBuf2 != NULL)) {
                *(DestIBufStart+wpt) = *(SrcBuf2+rpt);
            } else {
                *(DestIBufStart+wpt) = *(SrcBuf1+rpt);
            }
        }

        dst_offset = (dst_offset + FormatBytes)%DestIBufSize;
        toggle ^= 1;
        if (toggle==0) {
            src_offset = src_offset + FormatBytes;
        }

    }

}


/**
 * DSP_I2D_BufferCopy
 *
 * Interleaved buffer copy to direct buffer
 */
ATTR_TEXT_IN_IRAM VOID DSP_I2D_BufferCopy(U8* DestBuf1,
                        U8* DestBuf2,
                        U8* SrcBuf,
                        U8* SrcIBufStart,
                        U16 SrcIBufSize,
                        U16 CopySize,
                        U16 FormatBytes)
{
	U8* SrcIBufEnd		= (U8*)((U8*)SrcIBufStart + SrcIBufSize);
    uint32_t i, j, sample;
    uint32_t rpt, wpt, src_offset, dst_offset, toggle = 0;
    dst_offset = 0;
    src_offset = SrcBuf - SrcIBufStart;
    toggle = 0;
    configASSERT((SrcIBufEnd >= (U8*)SrcBuf) && (SrcBuf >= SrcIBufStart));

    //format_bytes
    sample = (CopySize<<1)/FormatBytes;

    for (i=0 ; i<sample ; i++) {
        for (j=0 ; j<FormatBytes ; j++ ) {
            rpt = (src_offset + j) % SrcIBufSize;
            wpt = dst_offset+j;
            if ((toggle == 1) && (DestBuf2 != NULL)) {
                *(DestBuf2+wpt) = *(SrcIBufStart+rpt);
            } else if (toggle == 0){
                *(DestBuf1+wpt) = *(SrcIBufStart+rpt);
            }
        }

        src_offset = (src_offset + FormatBytes)%SrcIBufSize;
        toggle ^= 1;
        if (toggle==0) {
            dst_offset = dst_offset + FormatBytes;
        }
    }
}

#if 0
/**
 * DSP_AudioDmaInit
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_AudioDmaInit(U8 TaskID, DSP_BUFFER_ID_t DfeID, U16 BufSize, U16 ThresholdSize)
{
    VOID* BufBlock;
    AU_DFE_ADMA_PTR_t VOLATILE DfeAdma;
    DfeAdma = DSP_DfeIDMappingToAdma(DfeID);

    configASSERT(DfeID < DFE_ID_TOTAL_NO);
    configASSERT(DSP_AdmaAddrManager[DfeID] == 0);
    configASSERT((BufSize) && (ThresholdSize));

    BufBlock = DSPMEM_tmalloc(TaskID, BufSize);

    DfeAdma->CTL.field.SW_RESET                = 1;
    DfeAdma->SET.field.BUF_SIZE                = BufSize/4;
    DfeAdma->SET.field.THD_SIZE                = ThresholdSize/4;
    DfeAdma->INIT.field.ADDR                   = (U32)BufBlock;
    DfeAdma->CTL.field.ENABLE                  = 1;

    memset(BufBlock, 0, BufSize);

    DSP_AdmaAddrManager[DfeID] = BufBlock;
}


/**
 * DSP_AudioBufferDeInit
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_AudioDmaDeInit(DSP_BUFFER_ID_t DfeID)
{
    AU_DFE_ADMA_PTR_t VOLATILE DfeAdma;

    configASSERT(DfeID < DFE_ID_TOTAL_NO);

    DfeAdma = DSP_DfeIDMappingToAdma(DfeID);
    DfeAdma->CTL.field.ERR_INTR_MASK           = 1;
    DfeAdma->CTL.field.THD_INTR_MASK           = 1;
    DfeAdma->CTL.field.ENABLE                  = 0;

    DSP_AdmaAddrManager[DfeID] = 0;
}

/**
 * DSP_AudioBufferInit
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_AudioBufferInit(U8 TaskID, DSP_BUFFER_ID_t DfeID, U16 BufSize)
{
    DSP_BUFFER_PTR_t BufBlock;
    DSP_BUFFER_HEADER_PTR_t pBufHead;

    configASSERT(DfeID < DFE_ID_TOTAL_NO);
    configASSERT(DSP_BufferAddrManager[DfeID] == 0);

    BufBlock = DSPMEM_tmalloc(TaskID, BufSize + sizeof(DSP_BUFFER_HEADER_t)/sizeof(U8));

    pBufHead = &BufBlock->Header;
    pBufHead->DmaOffset         = 0;
    pBufHead->WriteOffset       = 0;
    pBufHead->ReadOffset        = 0;
    pBufHead->BufferCount       = 0;
    pBufHead->BufferSize        = BufSize;
    pBufHead->Circular.BufStart = BufBlock->Buffer;
    pBufHead->Circular.BufEnd   = BufBlock->Buffer + BufSize - 1;
    pBufHead->OutputThreshold   = 0xFFFF;

    memset(BufBlock->Buffer, 0, BufSize);

    DSP_BufferAddrManager[DfeID] = BufBlock;
}


/**
 * DSP_AudioBufferDeInit
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_AudioBufferDeInit(DSP_BUFFER_ID_t DfeID)
{
    configASSERT(DfeID < DFE_ID_TOTAL_NO);

    DSP_BufferAddrManager[DfeID] = 0;
}


/**
 * DSP_IdfePushSamplesToAuInBuf
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_IdfePushSamplesToAuInBuf(DSP_BUFFER_ID_t DfeID)
{
    U8* SrcBuf = DSP_BUF_GetBufAddress(DfeID);
    U16 AdmaFrameSize, AdmaBufSize, NextHwWo, CurrHwWo, Samples;
    U32 AdmaBuf;
    DSP_BUFFER_HEADER_PTR_t pBufHead = DSP_BUF_GetHeader(DfeID);

    configASSERT(pBufHead != NULL);

    DSP_GetAdmaPara(DfeID, &AdmaFrameSize, &AdmaBufSize, &AdmaBuf, &NextHwWo);


    NextHwWo    = AdmaFrameSize * (((NextHwWo + AdmaFrameSize) % AdmaBufSize) / AdmaFrameSize);
    CurrHwWo    = DSP_WrapSubtraction(NextHwWo, AdmaFrameSize, AdmaBufSize);
    Samples     = DSP_WrapSubtraction(CurrHwWo, pBufHead->DmaOffset, AdmaBufSize);

    if ((pBufHead->BufferCount + Samples <= pBufHead->BufferSize) && (Samples > 0))
    {
        DSP_C2C_BufferCopy(SrcBuf + pBufHead->WriteOffset,
                           (VOID*)(AdmaBuf + pBufHead->DmaOffset),
                           Samples,
                           pBufHead->Circular.BufStart,
                           pBufHead->Circular.BufEnd,
                           (VOID*)(AdmaBuf),
                           (VOID*)(AdmaBuf + AdmaBufSize - 1));

        pBufHead->DmaOffset     = (pBufHead->DmaOffset + Samples) % AdmaBufSize;
        pBufHead->WriteOffset   = (pBufHead->WriteOffset + Samples) % pBufHead->BufferSize;
        pBufHead->BufferCount   += Samples;
    }
    else
    {
        if (Samples != 0)
        {
            configASSERT(FALSE);
        }

        /* Audio Buffer Full */
    }
}


/**
 * DSP_GetSampleFromAuInBuf
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
BOOL DSP_GetSampleFromAuInBuf(DSP_BUFFER_ID_t DfeID, U16 Samples, VOID* DestBuf)
{
    DSP_BUFFER_HEADER_PTR_t pBufHead = DSP_BUF_GetHeader(DfeID);
    U8* SrcBuf = DSP_BUF_GetBufAddress(DfeID);

    configASSERT(pBufHead != NULL);

    if (pBufHead->BufferCount >= Samples)
    {
        DSP_C2D_BufferCopy(DestBuf,
                           SrcBuf + pBufHead->ReadOffset,
                           Samples,
                           pBufHead->Circular.BufStart,
                           pBufHead->Circular.BufEnd);

        OS_PS ps = OS_ENTER_CRITICAL();
        pBufHead->ReadOffset   = (pBufHead->ReadOffset + Samples) % pBufHead->BufferSize;
        pBufHead->BufferCount  -= Samples;
        OS_EXIT_CRITICAL(ps);

        return TRUE;
    }

    return FALSE;
}


/**
 * DSP_OdfeGetSamplesFromAuOutBuf
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_OdfeGetSamplesFromAuOutBuf(DSP_BUFFER_ID_t DfeID)
{
    U8* SrcBuf = DSP_BUF_GetBufAddress(DfeID);
    U16 AdmaFrameSize, AdmaBufSize, NextHwRo, CurrHwRo;
    U32 AdmaBuf, FrameNo;
    DSP_BUFFER_HEADER_PTR_t pBufHead = DSP_BUF_GetHeader(DfeID);

    DSP_GetAdmaPara(DfeID, &AdmaFrameSize, &AdmaBufSize, &AdmaBuf, &NextHwRo);

    configASSERT(pBufHead != NULL);

    NextHwRo        = AdmaFrameSize * (((NextHwRo + AdmaFrameSize) % AdmaBufSize) / AdmaFrameSize);
    CurrHwRo        = DSP_WrapSubtraction(NextHwRo, AdmaFrameSize, AdmaBufSize);
    FrameNo         = (pBufHead->BufferCount / AdmaFrameSize);

    if (pBufHead->BufferCount >= pBufHead->OutputThreshold)
    {
        pBufHead->DmaOffset = NextHwRo;
        pBufHead->OutputThreshold = 0xDEAD;
    }
    else if (pBufHead->OutputThreshold != 0xDEAD)
    {
        /* Initial Condition not met */
        return;
    }

    while (FrameNo > 0)
    {
        if (pBufHead->DmaOffset != CurrHwRo)
        {
            DSP_C2C_BufferCopy((VOID*)(AdmaBuf + pBufHead->DmaOffset),
                               SrcBuf + pBufHead->ReadOffset,
                               AdmaFrameSize,
                               (VOID*)(AdmaBuf),
                               (VOID*)(AdmaBuf + AdmaBufSize - 1),
                               pBufHead->Circular.BufStart,
                               pBufHead->Circular.BufEnd);

            pBufHead->DmaOffset     = (pBufHead->DmaOffset + AdmaFrameSize) % AdmaBufSize;
            pBufHead->ReadOffset    = (pBufHead->ReadOffset + AdmaFrameSize) % pBufHead->BufferSize;
            pBufHead->BufferCount   -= AdmaFrameSize;

            FrameNo--;
        }
        else
        {
            /* Audio ODFE Buffer Full */
            break;
        }
    }
}


/**
 * DSP_PushSampleToAuOutBuf
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
BOOL DSP_PushSampleToAuOutBuf(DSP_BUFFER_ID_t DfeID, U16 Samples, VOID* SrcBuf)
{
    DSP_BUFFER_HEADER_PTR_t VOLATILE pBufHead = DSP_BUF_GetHeader(DfeID);
    U8* VOLATILE DestBuf = DSP_BUF_GetBufAddress(DfeID);

    configASSERT(pBufHead != NULL);

    if (pBufHead->BufferCount + Samples <= pBufHead->BufferSize)
    {
        DSP_D2C_BufferCopy(DestBuf + pBufHead->WriteOffset,
                           SrcBuf,
                           Samples,
                           pBufHead->Circular.BufStart,
                           pBufHead->Circular.BufEnd);

        OS_PS ps = OS_ENTER_CRITICAL();
        pBufHead->WriteOffset  = (pBufHead->WriteOffset + Samples) % pBufHead->BufferSize;
        pBufHead->BufferCount  += Samples;
        OS_EXIT_CRITICAL(ps);

        return TRUE;
    }

    return FALSE;
}
#endif
