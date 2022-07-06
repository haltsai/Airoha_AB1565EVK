/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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


#define AAC_IN_SIZE_WORKAROUND  (0)

#include <string.h>
#include "aac_dec_interface.h"
#include "dsp_feature_interface.h"
#include "dsp_utilities.h"
#include "dsp_buffer.h"
#include "aac_dec_api_internal.h"
#include "dsp_dump.h"
#if (AAC_IN_SIZE_WORKAROUND)
    #include "drv_usb_cdc.h"
    #include "usb_verify_interface.h"
#endif

/**
 *
 *  Definition
 *
 */
// #define AAC_IN_SIZE_WORKAROUND  (0)
#define AAC_DECODER_OUT_SAMPLES ((U16)1024)
#define AAC_USB_IN_FRAME_SIZE   ((U32)(512))
#define AAC_IN_CBUF_SIZE        ((U32)(4096))
#define AAC_OUT_CBUF_SIZE       ((U32)(8192))

extern xa_codec_handle_t p_xa_process_api_obj;

/**
 *
 *  Type Definition
 *
 */
typedef struct AAC_IN_BUF_s
{
    U16 wo;
    U16 ro;
    U16 count;
    BOOL enable;
    U16 threshold;
} AAC_IN_BUF_t;

typedef struct AAC_HEADER_STRUCT_s
{
    U32 Header1:    12;
    U32 Header2:    12;
} AAC_HEADER_STRUCT_t;



/**
 *
 *  Buffer & Control
 *
 */
DSP_ALIGN8 AAC_HAD_PTR AAC_INSTANCE;
DSP_ALIGN8 U8 AACHeader[3];



/**
 *
 * Function Prototype
 *
 */
bool stream_codec_decoder_aac_initialize (void *para);
bool stream_codec_decoder_aac_process (void *para);
VOID DSP_AAC_DeinterleaveStream (U8* inBuf, U8* splitBuf_L, U8* splitBuf_R, U32 inLen, VOID* para);
VOID AAC_Decoder_Algorithm_Init (VOID);
VOID AAC_Decoder_Deinitialization (VOID);
VOID AAC_Decoder_DataPushToScratch (S8* Ptr, U32 Length);
VOID AAC_Decoder_DataPopFromScratch (S8* Ptr, U32 Length);
BOOL AAC_Decoder_Process (U8* OutBufL, U8* OutBufR, VOID* para);


/**
 * stream_codec_decoder_aac_initialize
 *
 * This function is used to init memory space for AAC decoder
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
bool stream_codec_decoder_aac_initialize (void *para)
{
    if (AAC_INSTANCE == (AAC_HAD*)stream_codec_get_workingbuffer(para))
    {
        AAC_Decoder_Deinitialization();

        return FALSE;
    }
    else
    {
        AAC_INSTANCE = (AAC_HAD*)stream_codec_get_workingbuffer(para);

        AAC_Decoder_Deinitialization();

        AAC_INSTANCE->FrameworkReset = 0;
    }

    #if (AAC_IN_SIZE_WORKAROUND)
    DSP_UsbDataInit();
    #endif

    return FALSE;
}


/**
 * stream_codec_decoder_aac_process
 *
 * This function decodes a AAC frame into dual output channels
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
bool stream_codec_decoder_aac_process (void *para)
{
    U8* InBuf = stream_codec_get_1st_input_buffer(para);
    U8* OutBufL = stream_codec_get_1st_output_buffer(para);
    U8* OutBufR = stream_codec_get_2nd_output_buffer(para);
    U16 OutLength;
    U16 InLength = stream_codec_get_input_size(para);
    //U8  CodecOutSR = stream_codec_get_output_samplerate(para);
    U8* pCodecOutSR = stream_codec_get_output_samplerate_pointer(para);

    U32 len;
    U8  dec_error = 1;


    #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
                LOG_AUDIO_DUMP(InBuf, (InLength + 1) & (~1), AUDIO_CODEC_IN);
                LOG_AUDIO_DUMP((U8*)&InLength, (U32)2, AUDIO_CODEC_IN_LENGTH);
    #endif
	if (stream_codec_get_output_resolution(para) == RESOLUTION_16BIT)
	{
		stream_codec_modify_output_size(para, AAC_DECODER_OUT_SAMPLES * sizeof(U16));
	}
	else if (stream_codec_get_output_resolution(para)  == RESOLUTION_32BIT)
	{
		stream_codec_modify_output_size(para, AAC_DECODER_OUT_SAMPLES * sizeof(U32));
	}
	stream_codec_modify_resolution(para, stream_codec_get_output_resolution(para));
    OutLength = stream_codec_get_output_size(para);

#if (AAC_IN_SIZE_WORKAROUND)
    if (AAC_INSTANCE->FrameworkReset == 1)
    {
        AAC_INSTANCE->LengthRead = (((AACHeader[1] & 0x0F)<<8) | AACHeader[2]);

        len = AAC_INSTANCE->LengthRead;

        AAC_Decoder_DataPopFromScratch(AAC_INSTANCE->inp_buffer.pb_inp_buf+3, AAC_INSTANCE->LengthRead);

        AAC_INSTANCE->FrameworkReset = 0;
    }
    else
    {
        if (AAC_INSTANCE->LengthRead == 0)
        {
            while (1)
            {
                len = DSP_UsbPureDataInput(&AACHeader[0], 1);

                if (len == 1)
                {
                    if (AACHeader[0] == 0)
                    {
                        continue;
                    }
                    else
                    {
                        len = DSP_UsbPureDataInput(&AACHeader[1], 2);
                        len = 3;
                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            if (len == 3)
            {
                if ((AACHeader[0] == 0xAB) && (AACHeader[1] == 0x15) && (AACHeader[2] == 0x30))
                {
                    stream_feature_reinitialize(para);
                    return FALSE;
                }
                else
                {
                    AAC_INSTANCE->LengthRead = (((AACHeader[1] & 0x0F)<<8) | AACHeader[2]);
                }
            }
            else
            {
                memset(OutBufL, 0, OutLength);
                memset(OutBufR, 0, OutLength);
                return FALSE;
            }

            len = DSP_UsbPureDataInput((U8*)(AAC_INSTANCE->inp_buffer.pb_inp_buf+3), AAC_INSTANCE->LengthRead);
        }
    }
#else
    AAC_INSTANCE->LengthRead = InLength;//temp
    len = AAC_INSTANCE->LengthRead; /* LengthRead is from SourceSize of L2CAP */
    memcpy(AAC_INSTANCE->inp_buffer.pb_inp_buf + 3, InBuf, len);//temp
#endif

    if (AAC_INSTANCE->InitDone == 0)
    {
        if (len > 0)
        {
            len = FILL_AAC_DATA(&AAC_INSTANCE->inp_buffer, AAC_INSTANCE->LengthRead);

            AAC_INSTANCE->InitDone = AAC_DECODER_INIT2(&AAC_INSTANCE->inp_buffer,
                                                       len,
                                                       24,
                                                       XA_AACDEC_EBITSTREAM_TYPE_AAC_LOAS);

            if (AAC_INSTANCE->InitDone != 0)
            {
                get_params(p_xa_process_api_obj,
                           &AAC_INSTANCE->i_num_chan,
                           &AAC_INSTANCE->acmod,
                           &AAC_INSTANCE->i_samp_freq,
                           &AAC_INSTANCE->sbrType,
                           &AAC_INSTANCE->bsfmt,
                           &AAC_INSTANCE->i_data_rate,
                           &AAC_INSTANCE->pcm_sample_size);
// printf("i_num_chan %d, acmod %d, i_samp_freq %d, sbrType %d, bsfmt %d, i_data_rate %d, pcm_sample_size %d\r\n",AAC_INSTANCE->i_num_chan, AAC_INSTANCE->acmod, AAC_INSTANCE->i_samp_freq, AAC_INSTANCE->sbrType, AAC_INSTANCE->bsfmt, AAC_INSTANCE->i_data_rate, AAC_INSTANCE->pcm_sample_size);
                *pCodecOutSR = (AAC_INSTANCE->i_samp_freq/1000);

                // if (CodecOutSR != *pCodecOutSR)
                // {
                    // AAC_INSTANCE->FrameworkReset = 1;

                    // AAC_Decoder_DataPushToScratch(AAC_INSTANCE->inp_buffer.pb_inp_buf+3, AAC_INSTANCE->LengthRead);
                    // stream_feature_reinitialize(para);
                    // return FALSE;
                // }

                if (AAC_Decoder_Process(OutBufL, OutBufR, para))
                {
                    //stream_codec_modify_output_size(para, (AAC_INSTANCE->i_out_bytes)>>2);
                    AAC_INSTANCE->LengthRead = 0;
                    dec_error = 0;
                    //return FALSE;
                }

            }
            else
                {
                     stream_codec_modify_output_size(para, OutLength);
                }

            {
                AAC_INSTANCE->LengthRead = 0;
            }
        }
    }
    else
    {
        if (len > 0)
        {
            len = FILL_AAC_DATA(&AAC_INSTANCE->inp_buffer, AAC_INSTANCE->LengthRead);
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
            LOG_AUDIO_DUMP((U8*)AAC_INSTANCE->inp_buffer.pb_inp_buf, (U32)(AAC_INSTANCE->LengthRead+6), AUDIO_CODEC_IN);
            LOG_AUDIO_DUMP((U8*)&len, (U32)2, AUDIO_CODEC_IN_LENGTH);
#endif
            if (AAC_Decoder_Process(OutBufL, OutBufR, para))
            {
                //stream_codec_modify_output_size(para, (AAC_INSTANCE->i_out_bytes)>>2);
                AAC_INSTANCE->LengthRead = 0;
                dec_error = 0;
                //return FALSE;
            }
        }
    }

    if(dec_error == 1) {
        memset(OutBufL, 0, OutLength);
        memset(OutBufR, 0, OutLength);
        //printf("AAC_Decoder error init:%d len:%d \n",AAC_INSTANCE->InitDone,len);
    }

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)OutBufL, (U32)OutLength, AUDIO_SOURCE_IN_L);
    LOG_AUDIO_DUMP((U8*)OutBufR, (U32)OutLength, AUDIO_SOURCE_IN_R);
#endif

    return FALSE;
}



VOID DSP_AAC_DeinterleaveStream(U8* inBuf, U8* splitBuf_L, U8* splitBuf_R, U32 inLen, VOID* para)
{
    U32 i;
	if (stream_codec_get_output_resolution(para) == RESOLUTION_16BIT)
	{
	    for (i = 0 ; i < inLen/(2*sizeof(U32)) ; i++)
	    {
	        splitBuf_L[2*i]         =   inBuf[8*i+2];
	        splitBuf_L[2*i+1]       =   inBuf[8*i+3];

	        /* Should always have enough buffer size in splitBuf to do so */
	        #if 1
	        splitBuf_R[2*i]         =   inBuf[8*i+4+2];
	        splitBuf_R[2*i+1]       =   inBuf[8*i+4+3];
	        #else
	        UNUSED(splitBuf);
	        #endif
	    }
	}
	else
	{
		for (i = 0 ; i < inLen/(2*sizeof(U32)) ; i++)
	    {
	        *(U32*)&(splitBuf_L[4*i]) = *(U32*)&(inBuf[8*i]);
	        /* Should always have enough buffer size in splitBuf to do so */
	        #if 1
	        *(U32*)&(splitBuf_R[4*i]) = *(U32*)&(inBuf[8*i+4]);
	        #else
	        UNUSED(splitBuf);
	        #endif
	    }
	}
}



/**
 * AAC_Decoder_Algorithm_Init
 *
 * This function is used to intialize AAC algorithm
 *
 *
 *
 */
VOID AAC_Decoder_Algorithm_Init (VOID)
{
    AAC_DECODER_INIT1(&AAC_INSTANCE->inp_buffer,
                      AAC_INSTANCE->AAC_STUCT,
                      AAC_INSTANCE->AAC_MEMTABS,
                      AAC_INSTANCE->AAC_INPUT_BUF,
                      AAC_INSTANCE->AAC_OUTPUT_BUF,
                      AAC_INSTANCE->AAC_PERRAM,
                      AAC_INSTANCE->AAC_SCRAM);
}


/**
 * AAC_Decoder_Deinitialization
 *
 * This function is used to de-intialize AAC process
 *
 *
 *
 */
VOID AAC_Decoder_Deinitialization (VOID)
{
    AAC_Decoder_Algorithm_Init();

    AAC_INSTANCE->LengthRead    = 0;
    AAC_INSTANCE->InitDone      = 0;
    AAC_INSTANCE->ExecDone      = 0;
    AAC_INSTANCE->i_out_bytes   = 0;
}


/**
 * AAC_Decoder_DataPushToScratch
 *
 * This function is used to temporary push input pattern into scratch memory
 *
 *
 *
 */
VOID AAC_Decoder_DataPushToScratch (S8* Ptr, U32 Length)
{
    memcpy(&AAC_INSTANCE->AAC_SCRAM[0], Ptr, Length);
}


/**
 * AAC_Decoder_DataPopFromScratch
 *
 * This function is used to temporary pop output pattern from scratch memory
 *
 *
 *
 */
VOID AAC_Decoder_DataPopFromScratch (S8* Ptr, U32 Length)
{
    memcpy(Ptr, &AAC_INSTANCE->AAC_SCRAM[0], Length);
}

/**
 * AAC_Decoder_Process
 *
 * This function is used to perform actual decoding process of AAC
 *
 *
 *
 */
BOOL AAC_Decoder_Process (U8* OutBufL, U8* OutBufR, VOID* para)
{
    S32 err_code = 0;

    /* Execute process */
    AAC_API_CALL(XA_API_CMD_EXECUTE, XA_CMD_TYPE_DO_EXECUTE,
                 NULL,
                 "");

    if(err_code != 0) {
        DSP_MW_LOG_I("AAC_Decoder_Process ret error: %d\n",1, err_code);
    }

    /* Get the output bytes */
    AAC_API_CALL(XA_API_CMD_EXECUTE,
                 XA_CMD_TYPE_DONE_QUERY,
                 &AAC_INSTANCE->ExecDone,
                 "");

    AAC_API_CALL(XA_API_CMD_GET_OUTPUT_BYTES,
                 0,
                 &AAC_INSTANCE->i_out_bytes,
                 "");


    if (AAC_INSTANCE->i_out_bytes > 0)
    {
        DSP_AAC_DeinterleaveStream((U8*)AAC_INSTANCE->AAC_OUTPUT_BUF,
                                   (U8*)OutBufL,
                                   (U8*)OutBufR,
								   AAC_INSTANCE->i_out_bytes,
								   para);

        #if 0
        #if (AAC_IN_SIZE_WORKAROUND)
        while (DRV_USB_CDC_ACM1_Send(AAC_INSTANCE->AAC_OUTPUT_BUF, (AAC_INSTANCE->i_out_bytes/4)) == 0)
        {
            osTaskSuspendRequest(osTaskQueryCurrentTaskID());
            osTaskTaskingRequest();
        }
        #endif
        #endif

        //memcpy(OutBufL, AAC_INSTANCE->AAC_OUTPUT_BUF, AAC_INSTANCE->i_out_bytes/(2*(sizeof(U32)/sizeof(U16))));

        return TRUE;
    }

    return FALSE;

}

#ifdef PRELOADER_ENABLE
BOOL AAC_Decoder_Open  (VOID* para)
{
	DSP_MW_LOG_I("[PIC] AAC Decoder Open", 0);
	UNUSED(para);
	return TRUE;
}

BOOL AAC_Decoder_Close (VOID* para)
{
	DSP_MW_LOG_I("[PIC] AAC Decoder Close", 0);
	UNUSED(para);
	return TRUE;
}
#endif
