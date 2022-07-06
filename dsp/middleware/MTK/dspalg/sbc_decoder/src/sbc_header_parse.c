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
#include "sbc_interface.h"
#include "sbc.h"

U16 sbc_header_parse(U16 SBC_FRAME_HEADER_WORD0, U16 SBC_FRAME_HEADER_WORD1, SBC_DEC_STATE* st_ptr)
{
	// bit_count = 0;

	//if ((SBC_FRAME_HEADER_WORD0 >> 8) != 0x9c)
	//	return SBC_ERR_SYNCWORD_ERROR;
#if 0
	SBC_FS = ((SBC_FRAME_HEADER_WORD0 >> 6) & 0x3);

	switch((SBC_FRAME_HEADER_WORD0 >> 4) & 0x3) {
	case 0: SBC_BLOCKS = 4;		break;
	case 1: SBC_BLOCKS = 8;		break;
	case 2: SBC_BLOCKS = 12;	break;
	case 3: SBC_BLOCKS = 16;	break;
	}

	SBC_CHANNEL_MODE = ((SBC_FRAME_HEADER_WORD0 >> 2) & 0x3);
	SBC_ALLOC_METHOD = ((SBC_FRAME_HEADER_WORD0 >> 1) & 0x1);

	SBC_SUBBANDS = ((SBC_FRAME_HEADER_WORD0 & 0x1) == 0) ? 4u:8u;
	SBC_CHANNELS = (SBC_CHANNEL_MODE == SBC_MONO_CHANNEL) ? 1u:2u;
	SBC_BITPOOL = ((SBC_FRAME_HEADER_WORD1 >> 8) & 0xff);
//	SBC_CRC	= (SBC_FRAME_HEADER_WORD1 & 0xff);

	if (SBC_CHANNEL_MODE == SBC_MONO_CHANNEL
		|| SBC_CHANNEL_MODE == SBC_DUAL_CHANNEL)
	{
		SBC_FRAME_LENGTH  = 4 + (4 * SBC_SUBBANDS * SBC_CHANNELS)/8
			+ ((SBC_BLOCKS * SBC_CHANNELS * SBC_BITPOOL) + 7)/8;
	}
	else if (SBC_CHANNEL_MODE == SBC_STEREO_CHANNEL)
	{
		SBC_FRAME_LENGTH = 4 + (4 * SBC_SUBBANDS * SBC_CHANNELS)/8
			+ ((SBC_BLOCKS * SBC_BITPOOL) + 7)/8;
	}
	else
	{
		SBC_FRAME_LENGTH  = 4 + (4 * SBC_SUBBANDS * SBC_CHANNELS)/8
			+ (( SBC_SUBBANDS + (SBC_BLOCKS * SBC_BITPOOL) ) + 7)/8;
	}

	SBC_SAMPLES_PER_BLOCK = SBC_CHANNELS*SBC_SUBBANDS;
	SBC_SAMPLES_PER_FRAME = SBC_SAMPLES_PER_BLOCK*SBC_BLOCKS;
#endif
	/***************************************************/
	/*****************  Structualized   ****************/
	st_ptr->SBC_FS = ((SBC_FRAME_HEADER_WORD0 >> 6) & 0x3);

	switch((SBC_FRAME_HEADER_WORD0 >> 4) & 0x3) {
	case 0: st_ptr->SBC_BLOCKS = 4;		break;
	case 1: st_ptr->SBC_BLOCKS = 8;		break;
	case 2: st_ptr->SBC_BLOCKS = 12;	break;
	case 3: st_ptr->SBC_BLOCKS = 16;	break;
	}

	st_ptr->SBC_CHANNEL_MODE = ((SBC_FRAME_HEADER_WORD0 >> 2) & 0x3);
	st_ptr->SBC_ALLOC_METHOD = ((SBC_FRAME_HEADER_WORD0 >> 1) & 0x1);

	st_ptr->SBC_SUBBANDS = ((SBC_FRAME_HEADER_WORD0 & 0x1) == 0) ? 4u:8u;
	st_ptr->SBC_CHANNELS = (st_ptr->SBC_CHANNEL_MODE == SBC_MONO_CHANNEL) ? 1u:2u;
	st_ptr->SBC_BITPOOL = ((SBC_FRAME_HEADER_WORD1 >> 8) & 0xff);
//	SBC_CRC	= (SBC_FRAME_HEADER_WORD1 & 0xff);

	if (st_ptr->SBC_CHANNEL_MODE == SBC_MONO_CHANNEL
		|| st_ptr->SBC_CHANNEL_MODE == SBC_DUAL_CHANNEL)
	{
		st_ptr->SBC_FRAME_LENGTH  = 4 + (4 * st_ptr->SBC_SUBBANDS * st_ptr->SBC_CHANNELS)/8
			+ ((st_ptr->SBC_BLOCKS * st_ptr->SBC_CHANNELS * st_ptr->SBC_BITPOOL) + 7)/8;
	}
	else if (st_ptr->SBC_CHANNEL_MODE == SBC_STEREO_CHANNEL)
	{
		st_ptr->SBC_FRAME_LENGTH = 4 + (4 * st_ptr->SBC_SUBBANDS * st_ptr->SBC_CHANNELS)/8
			+ ((st_ptr->SBC_BLOCKS * st_ptr->SBC_BITPOOL) + 7)/8;
	}
	else
	{
		st_ptr->SBC_FRAME_LENGTH  = 4 + (4 * st_ptr->SBC_SUBBANDS * st_ptr->SBC_CHANNELS)/8
			+ (( st_ptr->SBC_SUBBANDS + (st_ptr->SBC_BLOCKS * st_ptr->SBC_BITPOOL) ) + 7)/8;
	}

	st_ptr->SBC_SAMPLES_PER_BLOCK = st_ptr->SBC_CHANNELS*st_ptr->SBC_SUBBANDS;
	st_ptr->SBC_SAMPLES_PER_FRAME = st_ptr->SBC_SAMPLES_PER_BLOCK*st_ptr->SBC_BLOCKS;
	st_ptr->OUTPUT_SAMPLES = st_ptr->SBC_SUBBANDS * st_ptr->SBC_BLOCKS;

	return SBC_NO_ERROR;
}


U16 SBC_CalculateInFrameSize (VOID* HeaderPtr)
{
    SBCFrameHeaderStruPtr FrameHdr;
	U16 framesize;
	U16 blocks;
	U16 subbands;
	U16 bitpool;
	U16 channels;

	FrameHdr    = (SBCFrameHeaderStruPtr)HeaderPtr;
	channels 	= (FrameHdr->Byte1.bit_alloc.CHANNEL_MODE == SBC_MONO_CHANNEL) ? (U16)1:(U16)2;
	subbands 	= (FrameHdr->Byte1.bit_alloc.SUBBANDS == 0) ? (U16)4:(U16)8;
	bitpool 	= FrameHdr->Bitpool;
	blocks 		= (FrameHdr->Byte1.bit_alloc.BLOCKS +1) * (U16)4;

    if (FrameHdr->SyncWord != 0x9c)
    {
        return 0;
    }

	switch(FrameHdr->Byte1.bit_alloc.CHANNEL_MODE)
	{
		case SBC_MONO_CHANNEL:
		case SBC_DUAL_CHANNEL:
			framesize = ((blocks * channels * bitpool) + 7)/8 + 4 + (4 * subbands * channels)/8;
			break;
		case SBC_STEREO_CHANNEL:
	        framesize = ((blocks * bitpool) + 7)/8 + 4 + (4 * subbands * channels)/8;
			break;
		default:
			framesize  = ((subbands + (blocks * bitpool)) + 7)/8 + 4 + (4 * subbands * channels)/8;
			break;
	}
	return framesize;
}


U16 SBC_CalculateOutFrameSize (VOID* HeaderPtr)
{
    SBCFrameHeaderStruPtr FrameHdr;
	U16 blocks;
	U16 subbands;

	FrameHdr    = (SBCFrameHeaderStruPtr)HeaderPtr;
	subbands 	= (FrameHdr->Byte1.bit_alloc.SUBBANDS == 0) ? (U16)4:(U16)8;
	blocks 		= (FrameHdr->Byte1.bit_alloc.BLOCKS + 1) * (U16)4;

    if (FrameHdr->SyncWord != 0x9c)
    {
        return 0;
    }

    #if 1//TEMP!!
	return (subbands * blocks * sizeof(U16));
    #else
    return (128 * sizeof(U16));
    #endif
}



SBC_FS_t SBC_CalculateSampleFrequency (VOID* HeaderPtr)
{
    SBCFrameHeaderStruPtr FrameHdr;

	FrameHdr = (SBCFrameHeaderStruPtr)HeaderPtr;

    if (FrameHdr->SyncWord != 0x9c)
    {
        return (SBC_FS_t)SBC_FS_INVALID;
    }
    else
    {
        return (SBC_FS_t)(FrameHdr->Byte1.bit_alloc.SAMPLING_FREQ);
    }
}


