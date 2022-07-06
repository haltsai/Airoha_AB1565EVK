/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef SBC_ENCODER_H
#define SBC_ENCODER_H

#include "types.h"

#define MSBC_GSOUND
#ifndef _WIN32
#define XTENSA
#endif
#define  SINT16 short
#define  SINT32 long
//#define  S16   short
//#define  S32    long

//for XT,
#define SINT16_XT  long  //1530
//#define SINT16_XT  short //1520

#define MSBC_OPTIMAL_1




#ifdef XTENSA
#define XT_ALIGN4 __attribute__((aligned(4)))
#define XT_ALIGN8 __attribute__((aligned(8)))
#else
#define XT_ALIGN4 __declspec(align(4))
#define XT_ALIGN8 __declspec(align(8))
#endif

#ifndef DSP_ALIGN4
#define DSP_ALIGN4 XT_ALIGN4
#endif

#ifndef DSP_ALIGN8
#define DSP_ALIGN8 XT_ALIGN8
#endif

/*DEFINES*/
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

#define SBC_MAX_NUM_OF_SUBBANDS 8
#define SBC_MAX_NUM_OF_CHANNELS 1 //larry
#ifdef  MSBC_GSOUND
#define SBC_MAX_NUM_OF_BLOCKS   16
#else
#define SBC_MAX_NUM_OF_BLOCKS   15
#endif
#define SBC_LOUDNESS    0
#define SBC_SNR 1

#define SUB_BANDS_8 8
#define SUB_BANDS_4 4

#define SBC_sf16000 0


#define SBC_MONO    0
#define SBC_DUAL    1
#define SBC_STEREO  2
#define SBC_JOINT_STEREO    3

#define SBC_BLOCK_0 4
#define SBC_BLOCK_1 8
#define SBC_BLOCK_2 12
#ifdef  MSBC_GSOUND
#define SBC_BLOCK_3_gs 16
#endif
#define SBC_BLOCK_3 15

#define SBC_NULL    0

#ifndef SBC_MAX_NUM_FRAME
#define SBC_MAX_NUM_FRAME 1
#endif

#ifndef SBC_DSP_OPT
#define SBC_DSP_OPT FALSE
#endif

/* Set SBC_USE_ARM_PRAGMA to TRUE to use "#pragma arm section zidata" */
#ifndef SBC_USE_ARM_PRAGMA
#define SBC_USE_ARM_PRAGMA FALSE
#endif

/* Set SBC_ARM_ASM_OPT to TRUE in case the target is an ARM */
/* this will replace all the 32 and 64 bit mult by in line assembly code */
#ifndef SBC_ARM_ASM_OPT
#define SBC_ARM_ASM_OPT FALSE
#endif

/* green hill compiler option -> Used to distinguish the syntax for inline assembly code*/
#ifndef SBC_GHS_COMPILER
#define SBC_GHS_COMPILER FALSE
#endif

/* ARM compiler option -> Used to distinguish the syntax for inline assembly code */
#ifndef SBC_ARM_COMPILER
#define SBC_ARM_COMPILER FALSE
#endif

/* Set SBC_IPAQ_OPT to TRUE in case the target is an ARM */
/* 32 and 64 bit mult will be performed using SINT64 ( usualy __int64 ) cast that usualy give optimal performance if supported */
#ifndef SBC_IPAQ_OPT
#define SBC_IPAQ_OPT FALSE
#endif

/* Debug only: set SBC_IS_64_MULT_IN_WINDOW_ACCU to TRUE to use 64 bit multiplication in the windowing */
/* -> not recomended, more MIPS for the same restitution.  */
#ifndef SBC_IS_64_MULT_IN_WINDOW_ACCU
#define SBC_IS_64_MULT_IN_WINDOW_ACCU  FALSE
#endif /*SBC_IS_64_MULT_IN_WINDOW_ACCU */

/* Set SBC_IS_64_MULT_IN_IDCT to TRUE to use 64 bits multiplication in the DCT of Matrixing */
/* -> more MIPS required for a better audio quality. comparasion with the SIG utilities shows a division by 10 of the RMS */
/* CAUTION: It only apply in the if SBC_FAST_DCT is set to TRUE */
#ifndef SBC_IS_64_MULT_IN_IDCT
#define SBC_IS_64_MULT_IN_IDCT  FALSE
#endif /*SBC_IS_64_MULT_IN_IDCT */

/* set SBC_IS_64_MULT_IN_QUANTIZER to TRUE to use 64 bits multiplication in the quantizer */
/* setting this flag to FALSE add whistling noise at 5.5 and 11 KHz usualy not perceptible by human's hears. */
#ifndef SBC_IS_64_MULT_IN_QUANTIZER
#define SBC_IS_64_MULT_IN_QUANTIZER  TRUE
#endif /*SBC_IS_64_MULT_IN_IDCT */

/* Debug only: set this flag to FALSE to disable fast DCT algorithm */
#ifndef SBC_FAST_DCT
//#define SBC_FAST_DCT  TRUE
#define SBC_FAST_DCT  FALSE
#endif /*SBC_FAST_DCT */





#ifndef SBC_NO_PCM_CPY_OPTION
#define SBC_NO_PCM_CPY_OPTION FALSE
#endif

#define MINIMUM_ENC_VX_BUFFER_SIZE (8*10*2)

#define EncMaxShiftCounter (((ENC_VX_BUFFER_SIZE-8*10)>>3)<<3)
#ifndef ENC_VX_BUFFER_SIZE
#define ENC_VX_BUFFER_SIZE (MINIMUM_ENC_VX_BUFFER_SIZE + 64)
/*#define ENC_VX_BUFFER_SIZE MINIMUM_ENC_VX_BUFFER_SIZE + 1024*/
#endif

#ifndef SBC_FOR_EMBEDDED_LINUX
#define SBC_FOR_EMBEDDED_LINUX FALSE
#endif



#ifndef SBC_TYPES_H
#define SBC_TYPES_H




#ifndef DATA_TYPES_H
#define DATA_TYPES_H





typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned long   UINT32;






//#define PACKED


#ifndef BIG_ENDIAN
#define BIG_ENDIAN FALSE
#endif

#define UINT16_LOW_BYTE(x)      ((x) & 0xff)
#define UINT16_HI_BYTE(x)       ((x) >> 8)


#endif




#define abs32(x) ( (x >= 0) ? x : (-x) )

#endif




typedef struct{
	SINT16_XT	bit_pack_ptr;	//	for bit_packing
    UINT16 bit_pack_buffer;  // for bit_packing
    SINT16_XT packed_len ;   // for bit_packing
    SINT16 *poutput16  ;

} my_bit_pack_s;

typedef struct SBC_ENC_HANDLE
{

	//SbcAnalysisFilter8
    XT_ALIGN8  SINT32   s32DCTY[16]  ;
    XT_ALIGN8  SINT32   s32X[ENC_VX_BUFFER_SIZE/2];  //²¾¦Ü SBC_ENC_HANDLE

    XT_ALIGN8  SINT16_XT  s16ScartchMemForBitAlloc[16];
    XT_ALIGN8  SINT16_XT as16ScaleFactor[SBC_MAX_NUM_OF_CHANNELS*SBC_MAX_NUM_OF_SUBBANDS];

    XT_ALIGN8  SINT16_XT as16Bits[SBC_MAX_NUM_OF_CHANNELS*SBC_MAX_NUM_OF_SUBBANDS];
    XT_ALIGN8  SINT32  s32SbBuffer[SBC_MAX_NUM_OF_CHANNELS * SBC_MAX_NUM_OF_SUBBANDS * SBC_MAX_NUM_OF_BLOCKS];

	 SINT16_XT s16SamplingFreq;                         /* 16k, 32k, 44.1k or 48k*/
    SINT16_XT s16ChannelMode;                          /* mono, dual, streo or joint streo*/
    SINT16_XT s16NumOfSubBands;                        /* 4 or 8 */
    SINT16_XT s16NumOfBlocks;                          /* 4, 8, 12 or 16*/
    SINT16_XT s16AllocationMethod;                     /* loudness or SNR*/
    SINT16_XT s16BitPool;                              /* 16*numOfSb for mono & dual;
                                                       32*numOfSb for stereo & joint stereo */
    SINT16_XT u16BitRate;

    SINT16_XT s16MaxBitNeed;

    SINT16 *ps16NextPcmBuffer;


    //UINT8  *pu8NextPacket;replace by word
	 SINT16 *p16NextPacket;
    SINT16_XT FrameHeader;
    SINT16_XT u16PacketLength;

	 SINT16_XT ShiftCounter;

	 //for bit packingn
	 my_bit_pack_s bpack;

	 #ifdef  MSBC_GSOUND

     SINT16_XT s16withHead; //add by archin  0 msbc 1: gsound without head 3: gsound with head

	#endif
	 // for mSBC frame count
	SINT16 mSBC_frame_cnt;

}SBC_ENC_PARAMS;






typedef SINT32 (*MSBC_ENC_ENTRY)(SINT16 *input, SINT16 *output,SBC_ENC_PARAMS *pstrEncParams);

extern SINT32 mSBC_Encoder_Process(SINT16 *input, SINT16 *output,SBC_ENC_PARAMS *pstrEncParams);
extern void SBC_Encoder_Init(SBC_ENC_PARAMS *pstrEncParams);

/* GSound related */
extern SINT32 mSBC_GSENC_Process(SINT16 *input, SINT16 *output,SBC_ENC_PARAMS *pstrEncParams);
extern void SBC_GSENC_INIT(int bitpool,int withheader,SBC_ENC_PARAMS *pstrEncParams);

#endif






#ifndef SBC_FUNCDECLARE_H
#define SBC_FUNCDECLARE_H

/*#include "sbc_encoder.h"*/
/* Global data */


/* Global functions*/

void sbc_enc_bit_alloc_mono(SBC_ENC_PARAMS *CodecParams);
void sbc_enc_bit_alloc_ste(SBC_ENC_PARAMS *CodecParams);

void SbcAnalysisInit (SBC_ENC_PARAMS *pstrEncParams);

void SbcAnalysisFilter4(SBC_ENC_PARAMS *strEncParams);
void SbcAnalysisFilter8(SBC_ENC_PARAMS *strEncParams);

void NEW_SBC_FastIDCT8 (SINT32 *pInVect, SINT32 *pOutVect);
void SBC_FastIDCT8 (SINT32 *pInVect, SINT32 *pOutVect);
extern void mSBC_SW_SN(SINT16 *out_buf, SINT16 *mSBC_frame_cnt_ptr);
extern void mSBC_SW_SN_init(SINT16 *mSBC_frame_cnt_ptr);

//void SBC_FastIDCT4 (SINT32 *x0, SINT32 *pOutVect);

void EncPacking(SBC_ENC_PARAMS *strEncParams);
void EncQuantizer(SBC_ENC_PARAMS *);
#if (SBC_DSP_OPT==TRUE)
    SINT32 SBC_Multiply_32_16_Simplified(SINT32 s32In2Temp,SINT32 s32In1Temp);
#endif
#endif



